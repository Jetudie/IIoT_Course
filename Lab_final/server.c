#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include "open62541.h"

static UA_Int32 Red = 0;
static UA_Int32 Green = 0;
static UA_Int32 Blue = 0;

UA_Boolean running = true;
UA_Boolean threading = 0;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_t t;

static void stopHandler(int);
static void *runDetection(void*);
static void writeStatusVariable(UA_Server*, int);
static void writeRGBVariable(UA_Server*, int);
static UA_StatusCode switchMethodCallback(UA_Server*,const UA_NodeId*, void*, const UA_NodeId*,
	      			   	  void *,const UA_NodeId*, void*, size_t, const UA_Variant*,
					  size_t, UA_Variant*);
static void addSwitchMethod(UA_Server*, UA_NodeId);
static void addARMObject(UA_Server*, char*);


static void stopHandler(int sign) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
    running = false;
}

static void *runDetection(void *server){    
    char* str = "thread ends";

    pthread_mutex_lock( &mutex1 ); 

    if(threading){
	    pthread_exit((void*) str);
	    return 0;
    }

    threading = 1;
    pthread_mutex_unlock( &mutex1 ); 

    int i = 0;
    int ret = 3;
    char str[30] = "python3 grasp.py 0";
    UA_Server *s1 = (UA_Server*) server;

    sleep(3);

    while(running && threading){
	    printf("Detecting... (%d)\n", ++i);
	    writeStatusVariable(s1, 2); 
	    system("python3 trytry.py; echo $?>log.txt");

	    FILE *file = fopen("log.txt", "r");
	    if(file == NULL){
		printf("Fail to open file!\n");
	    }

	    /* set hum and temp according to log.txt */
	    fscanf(file, "%d", &ret);
	    fclose(file);
	    printf("return value: %d\n", ret);

	    str[17] += ret; 
	    writeRGBVariable(s1, ret);
	    writeStatusVariable(s1, 1); 
	    ret = system(str);
	    sleep(2);
    }

    pthread_mutex_lock( &mutex1 ); 
    threading = 0;
    pthread_mutex_unlock( &mutex1 ); 
    writeStatusVariable(s1, 0); 
    pthread_exit((void*) str);
}

static void writeStatusVariable(UA_Server *server, int status) {
    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, "status");

    /* Determine myString */
    UA_String myString;
    if(status == 1)
        myString = UA_STRING("Grasping");
    else if(status == 2)
        myString = UA_STRING("Waiting");
    else
        myString = UA_STRING("Off");
	
    /* Write status variable */
    UA_Variant myVar;
    UA_Variant_init(&myVar);
    UA_Variant_setScalar(&myVar, &myString, &UA_TYPES[UA_TYPES_STRING]);
    UA_Server_writeValue(server, myIntegerNodeId, myVar);

    /* Set the status code of the value to an error code. The function
     * UA_Server_write provides access to the raw service. The above
     * UA_Server_writeValue is syntactic sugar for writing a specific node
     * attribute with the write service. */
    UA_WriteValue wv;
    UA_WriteValue_init(&wv);
    wv.nodeId = myIntegerNodeId;
    wv.attributeId = UA_ATTRIBUTEID_VALUE;
    wv.value.status = UA_STATUSCODE_BADNOTCONNECTED;
    wv.value.hasStatus = true;

    UA_Server_write(server, &wv);

    /* Reset the variable to a good statuscode with a value */
    wv.value.hasStatus = false;
    wv.value.value = myVar;
    wv.value.hasValue = true;
    UA_Server_write(server, &wv);
}

static void
writeRGBVariable(UA_Server *server, int x) {
    /* Write a different value according to current status. 
     * In here only myString is used */
    UA_NodeId myIntegerNodeId;
    UA_Int32 myInt;
    switch(x){
	case 1:
	    myIntegerNodeId = UA_NODEID_STRING(1, "blue");
	    myInt = ++Red; 
	    break;
	case 2:
	    myIntegerNodeId = UA_NODEID_STRING(1, "green");
	    myInt = ++Green; 
	    break;
	case 3:
	    myIntegerNodeId = UA_NODEID_STRING(1, "red");
	    myInt = ++Blue; 
	    break;
	default:
	    myIntegerNodeId = UA_NODEID_STRING(1, "blue");
	    myInt = ++Red; 
    }

    UA_Variant myVar;
    UA_Variant_init(&myVar);
    UA_Variant_setScalar(&myVar, &myInt, &UA_TYPES[UA_TYPES_INT32]);
    UA_Server_writeValue(server, myIntegerNodeId, myVar);

    /* Set the status code of the value to an error code. The function
     * UA_Server_write provides access to the raw service. The above
     * UA_Server_writeValue is syntactic sugar for writing a specific node
     * attribute with the write service. */
    UA_WriteValue wv;
    UA_WriteValue_init(&wv);
    wv.nodeId = myIntegerNodeId;
    wv.attributeId = UA_ATTRIBUTEID_VALUE;
    wv.value.status = UA_STATUSCODE_BADNOTCONNECTED;
    wv.value.hasStatus = true;

    UA_Server_write(server, &wv);

    /* Reset the variable to a good statuscode with a value */
    wv.value.hasStatus = false;
    wv.value.value = myVar;
    wv.value.hasValue = true;
    UA_Server_write(server, &wv);
}

static UA_StatusCode
switchMethodCallback(UA_Server *server,
                         const UA_NodeId *sessionId, void *sessionHandle,
                         const UA_NodeId *methodId, void *methodContext,
                         const UA_NodeId *objectId, void *objectContext,
                         size_t inputSize, const UA_Variant *input,
                         size_t outputSize, UA_Variant *output) {

    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Run method was called");

    /* call monitorDHT.py use BCM pin 26 and set time to 1 sec */
    //char str[30] = "python3 monitorDHT.py 26 2";
    //printf("%s\n", str);
    //system(str);
    pthread_mutex_lock( &mutex1 ); 
    if(threading){
	    threading = 0;
    }
    else
	    pthread_create(&t, NULL, runDetection, (void*) server);
	    

    pthread_mutex_unlock( &mutex1 );

    return UA_STATUSCODE_GOOD;
}

static void
addSwitchMethod(UA_Server *server, UA_NodeId nodeid) {
    UA_Argument inputArgument;
    UA_Argument_init(&inputArgument);
    inputArgument.description = UA_LOCALIZEDTEXT("en-US", "A String");
    inputArgument.name = UA_STRING("MyInput");
    inputArgument.dataType = UA_TYPES[UA_TYPES_STRING].typeId;
    inputArgument.valueRank = -1; /* scalar */

    UA_Argument outputArgument;
    UA_Argument_init(&outputArgument);
    outputArgument.description = UA_LOCALIZEDTEXT("en-US", "A String");
    outputArgument.name = UA_STRING("MyOutput");
    outputArgument.dataType = UA_TYPES[UA_TYPES_STRING].typeId;
    outputArgument.valueRank = -1; /* scalar */

    UA_MethodAttributes helloAttr = UA_MethodAttributes_default;
    helloAttr.description = UA_LOCALIZEDTEXT("en-US","Switch for detection");
    helloAttr.displayName = UA_LOCALIZEDTEXT("en-US","Switch for detection");
    helloAttr.executable = true;
    helloAttr.userExecutable = true;
    UA_NodeId myNodeId = UA_NODEID_STRING(1, "switch");
    UA_Server_addMethodNode(server, myNodeId, nodeid,
                          UA_NODEID_NUMERIC(0, UA_NS0ID_HASORDEREDCOMPONENT),
                          UA_QUALIFIEDNAME(1, "switch"),
                          helloAttr, &switchMethodCallback,
                          1, &inputArgument, 1, &outputArgument, NULL, NULL);
}

static void addARMObject(UA_Server *server, char* objectname){
	UA_ObjectAttributes oAttr;
	UA_ObjectAttributes_init(&oAttr);
	oAttr.displayName = UA_LOCALIZEDTEXT("en-US", objectname);
	UA_NodeId myObjectNodeId = UA_NODEID_STRING(1, objectname);
	UA_QualifiedName myObjectName = UA_QUALIFIEDNAME(1, objectname);
	UA_Server_addObjectNode(server, myObjectNodeId,
		UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
		UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
		myObjectName, UA_NODEID_NULL,
		oAttr, NULL, NULL);
	
	/* add variable red */
	UA_VariableAttributes redAttr = UA_VariableAttributes_default;	
	
	UA_NodeId redNodeId = UA_NODEID_STRING(1, "red");
        UA_QualifiedName redName = UA_QUALIFIEDNAME(1, "red");
        
        redAttr.description = UA_LOCALIZEDTEXT("en-US","red");
        redAttr.displayName = UA_LOCALIZEDTEXT("en-US","red");
        redAttr.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
	redAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    	
        UA_Variant_setScalar(&redAttr.value, &Red, &UA_TYPES[UA_TYPES_INT32]);
        UA_Server_addVariableNode(server, redNodeId, myObjectNodeId,
                                  UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                  redName,
                                  UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), redAttr, NULL, NULL);

	/* add variable green */
	UA_VariableAttributes greenAttr = UA_VariableAttributes_default;	
	
	UA_NodeId greenNodeId = UA_NODEID_STRING(1, "green");
	UA_QualifiedName greenName = UA_QUALIFIEDNAME(1, "green");
        
       	greenAttr.description = UA_LOCALIZEDTEXT("en-US","green");
	greenAttr.displayName = UA_LOCALIZEDTEXT("en-US","green");
	greenAttr.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
	greenAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    	
    	UA_Variant_setScalar(&greenAttr.value, &Green, &UA_TYPES[UA_TYPES_INT32]);
    	UA_Server_addVariableNode(server, greenNodeId, myObjectNodeId,
                                  UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                  greenName,
                                  UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), greenAttr, NULL, NULL);

	/* add variable blue */
	UA_VariableAttributes blueAttr = UA_VariableAttributes_default;	
	
	UA_NodeId blueNodeId = UA_NODEID_STRING(1, "blue");
	UA_QualifiedName blueName = UA_QUALIFIEDNAME(1, "blue");
        
        blueAttr.description = UA_LOCALIZEDTEXT("en-US","blue");
	blueAttr.displayName = UA_LOCALIZEDTEXT("en-US","blue");
	blueAttr.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
	blueAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    	
    	UA_Variant_setScalar(&blueAttr.value, &Blue, &UA_TYPES[UA_TYPES_INT32]);
    	UA_Server_addVariableNode(server, blueNodeId, myObjectNodeId,
                                  UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                  blueName,
                                  UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), blueAttr, NULL, NULL);

	/* add variable status */
	UA_VariableAttributes modelAttr = UA_VariableAttributes_default;	
	UA_String myString = UA_STRING("Waiting");
	
	UA_NodeId statusNodeId = UA_NODEID_STRING(1, "status");
    	UA_QualifiedName statusName = UA_QUALIFIEDNAME(1, "status");
        
    	modelAttr.description = UA_LOCALIZEDTEXT("en-US","status");
    	modelAttr.displayName = UA_LOCALIZEDTEXT("en-US","status");
    	modelAttr.dataType = UA_TYPES[UA_TYPES_STRING].typeId;
	modelAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    	
    	UA_Variant_setScalar(&modelAttr.value, &myString, &UA_TYPES[UA_TYPES_STRING]);
    	UA_Server_addVariableNode(server, statusNodeId, myObjectNodeId,
                                  UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                                  statusName,
                                  UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), modelAttr, NULL, NULL);
    /* add method */
	addSwitchMethod(server, myObjectNodeId);
}

int main(void) {
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    void *ret;

    UA_ServerConfig *config = UA_ServerConfig_new_default();
    UA_Server *server = UA_Server_new(config);

    /* Add obejcts with methods and variables underneath */
    addARMObject(server, "ARM");

    pthread_create(&t, NULL, runDetection, (void*) server);
    UA_StatusCode retval = UA_Server_run(server, &running);
    pthread_join(t, &ret);
    char *result = (char*) ret;
    printf("result = %s\n", result);
    UA_Server_delete(server);
    UA_ServerConfig_delete(config);
    return (int)retval;
}
