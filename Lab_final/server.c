#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include "open62541.h"

//static UA_Double hum = 0;
static UA_Double temp = 0;
static UA_Int32 position = 0;

static void writeLEDVariable(UA_Server *server, bool onoff) {
    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, "status");

    /* Determine myString */
    UA_String myString;
    if(onoff)
        myString = UA_STRING("on");
    else 
        myString = UA_STRING("off");

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

static UA_StatusCode
turnonMethodCallback(UA_Server *server,
                         const UA_NodeId *sessionId, void *sessionHandle,
                         const UA_NodeId *methodId, void *methodContext,
                         const UA_NodeId *objectId, void *objectContext,
                         size_t inputSize, const UA_Variant *input,
                         size_t outputSize, UA_Variant *output) {

    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Turn on was called");

    /* Call led.py with argument 1 to turn LED on */    
    char str[20] = "python3 led.py 1";
    printf("%s\n", str);
    system(str);
    
    /* Set status to "on" */
    writeLEDVariable(server, true); 

    return UA_STATUSCODE_GOOD;
}

static UA_StatusCode
turnoffMethodCallback(UA_Server *server,
                         const UA_NodeId *sessionId, void *sessionHandle,
                         const UA_NodeId *methodId, void *methodContext,
                         const UA_NodeId *objectId, void *objectContext,
                         size_t inputSize, const UA_Variant *input,
                         size_t outputSize, UA_Variant *output) {

    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Turn off was called");
    
    /* Call led.py with argument 1 to turn LED on */    
    char str[20] = "python3 led.py 0";
    printf("%s\n", str);
    system(str);

    /* Set status to "off" */
    writeLEDVariable(server, false); 

    return UA_STATUSCODE_GOOD;
}
static void
addTurnOnMethod(UA_Server *server, UA_NodeId nodeid) {
    UA_Argument inputArgument;
    UA_Argument_init(&inputArgument);
    inputArgument.description = UA_LOCALIZEDTEXT("en-US", "A String(ON)");
    inputArgument.name = UA_STRING("MyInput(ON)");
    inputArgument.dataType = UA_TYPES[UA_TYPES_STRING].typeId;
    inputArgument.valueRank = -1; /* scalar */

    UA_Argument outputArgument;
    UA_Argument_init(&outputArgument);
    outputArgument.description = UA_LOCALIZEDTEXT("en-US", "A String(ON)");
    outputArgument.name = UA_STRING("MyOutput(ON)");
    outputArgument.dataType = UA_TYPES[UA_TYPES_STRING].typeId;
    outputArgument.valueRank = -1; /* scalar */

    UA_MethodAttributes helloAttr = UA_MethodAttributes_default;
    helloAttr.description = UA_LOCALIZEDTEXT("en-US","Turn on LED");
    helloAttr.displayName = UA_LOCALIZEDTEXT("en-US","turnon method");
    helloAttr.executable = true;
    helloAttr.userExecutable = true;
    UA_NodeId myNodeId = UA_NODEID_STRING(1, "turnon");
    UA_Server_addMethodNode(server, myNodeId, nodeid,
                          UA_NODEID_NUMERIC(0, UA_NS0ID_HASORDEREDCOMPONENT),
                          UA_QUALIFIEDNAME(1, "hello world"),
                          helloAttr, &turnonMethodCallback,
                          1, &inputArgument, 1, &outputArgument, NULL, NULL);
                          //UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER), UA_NODEID_NUMERIC(1,62541)
}

static void
addTurnOffMethod(UA_Server *server, UA_NodeId nodeid ) {
    UA_Argument inputArgument;
    UA_Argument_init(&inputArgument);
    inputArgument.description = UA_LOCALIZEDTEXT("en-US", "A String(OFF)");
    inputArgument.name = UA_STRING("MyInput(OFF)");
    inputArgument.dataType = UA_TYPES[UA_TYPES_STRING].typeId;
    inputArgument.valueRank = -1; /* scalar */

    UA_Argument outputArgument;
    UA_Argument_init(&outputArgument);
    outputArgument.description = UA_LOCALIZEDTEXT("en-US", "A String(OFF)");
    outputArgument.name = UA_STRING("MyOutput(OFF)");
    outputArgument.dataType = UA_TYPES[UA_TYPES_STRING].typeId;
    outputArgument.valueRank = -1; /* scalar */

    UA_MethodAttributes helloAttr = UA_MethodAttributes_default;
    helloAttr.description = UA_LOCALIZEDTEXT("en-US","Turn off LED");
    helloAttr.displayName = UA_LOCALIZEDTEXT("en-US","turnoff method");
    helloAttr.executable = true;
    helloAttr.userExecutable = true;
    UA_NodeId myNodeId = UA_NODEID_STRING(1, "turnoff");
    UA_Server_addMethodNode(server, myNodeId, nodeid,
                          UA_NODEID_NUMERIC(0, UA_NS0ID_HASORDEREDCOMPONENT),
                          UA_QUALIFIEDNAME(1, "Turn_off"),
                          helloAttr, &turnoffMethodCallback,
                          1, &inputArgument, 1, &outputArgument, NULL, NULL);
}

static void addLEDObject(UA_Server *server, char* objectname){
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
	
	/* add variable status */
	UA_VariableAttributes modelAttr = UA_VariableAttributes_default;	
	UA_String myString = UA_STRING("off");
	
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

	/* add methods */
	addTurnOnMethod(server, myObjectNodeId);
	addTurnOffMethod(server, myObjectNodeId);
}


static void
writeDHTVariable(UA_Server *server, char* nodeid, int x) {
    UA_NodeId myIntegerNodeId = UA_NODEID_STRING(1, nodeid);

    /* Write a different value according to current status. 
     * In here only myString is used */
    UA_Double myDouble;
    if(x)
    	myDouble = hum;
    else
    	myDouble = temp;

    UA_Variant myVar;
    UA_Variant_init(&myVar);
    UA_Variant_setScalar(&myVar, &myDouble, &UA_TYPES[UA_TYPES_DOUBLE]);
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
getMethodCallback(UA_Server *server,
                         const UA_NodeId *sessionId, void *sessionHandle,
                         const UA_NodeId *methodId, void *methodContext,
                         const UA_NodeId *objectId, void *objectContext,
                         size_t inputSize, const UA_Variant *input,
                         size_t outputSize, UA_Variant *output) {

    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "Get method was called");

    /* call monitorDHT.py use BCM pin 26 and set time to 1 sec */
    char str[30] = "python3 monitorDHT.py 26 2";
    printf("%s\n", str);
    system(str);
    //sleep(1);

    /* read log.txt file writes by monitorDHT.py */
    FILE *file = fopen("log.txt", "r");
    if(file == NULL){
        printf("Fail to open file!\n");
    }

    /* set hum and temp according to log.txt */
    fscanf(file, "%lf\t%lf", &hum, &temp);
    printf("%.2lf and %.2lf\n", temp, hum);
    fclose(file);

    /* write hum and temp variable */
    writeDHTVariable(server, "hum", 1); 
    writeDHTVariable(server, "temp", 0); 

    return UA_STATUSCODE_GOOD;
}

static void
addGetMethod(UA_Server *server, UA_NodeId nodeid) {
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
    helloAttr.description = UA_LOCALIZEDTEXT("en-US","get data");
    helloAttr.displayName = UA_LOCALIZEDTEXT("en-US","get method");
    helloAttr.executable = true;
    helloAttr.userExecutable = true;
    UA_NodeId myNodeId = UA_NODEID_STRING(1, "getdata");
    UA_Server_addMethodNode(server, myNodeId, nodeid,
                          UA_NODEID_NUMERIC(0, UA_NS0ID_HASORDEREDCOMPONENT),
                          UA_QUALIFIEDNAME(1, "getget"),
                          helloAttr, &getMethodCallback,
                          1, &inputArgument, 1, &outputArgument, NULL, NULL);
}

static void addDHTObject(UA_Server *server, char* objectname){
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
	

	/* add variable temp */
	UA_VariableAttributes tempAttr = UA_VariableAttributes_default;	
	UA_Double myTemp = 0;
	
	UA_NodeId tempNodeId = UA_NODEID_STRING(1, "temp");
    UA_QualifiedName tempName = UA_QUALIFIEDNAME(1, "temp");
        
    tempAttr.description = UA_LOCALIZEDTEXT("en-US","temp");
    tempAttr.displayName = UA_LOCALIZEDTEXT("en-US","temp");
    tempAttr.dataType = UA_TYPES[UA_TYPES_DOUBLE].typeId;
	tempAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    	
    UA_Variant_setScalar(&tempAttr.value, &myTemp, &UA_TYPES[UA_TYPES_DOUBLE]);
    UA_Server_addVariableNode(server, tempNodeId, myObjectNodeId,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              tempName,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), tempAttr, NULL, NULL);

	/* add variable hum */
	UA_VariableAttributes humAttr = UA_VariableAttributes_default;	
	UA_Double myHum = 0;
	
	UA_NodeId humNodeId = UA_NODEID_STRING(1, "hum");
    UA_QualifiedName humName = UA_QUALIFIEDNAME(1, "hum");
        
    humAttr.description = UA_LOCALIZEDTEXT("en-US","hum");
    humAttr.displayName = UA_LOCALIZEDTEXT("en-US","hum");
    humAttr.dataType = UA_TYPES[UA_TYPES_DOUBLE].typeId;
	humAttr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    	
    	UA_Variant_setScalar(&humAttr.value, &myHum, &UA_TYPES[UA_TYPES_DOUBLE]);
    	UA_Server_addVariableNode(server, humNodeId, myObjectNodeId,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
                              humName,
                              UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE), humAttr, NULL, NULL);
    /* add method */
	addGetMethod(server, myObjectNodeId);
}


UA_Boolean running = true;
static void stopHandler(int sign) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, "received ctrl-c");
    running = false;
}


int main(void) {
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    UA_ServerConfig *config = UA_ServerConfig_new_default();
    UA_Server *server = UA_Server_new(config);

    /* Lab8: Add obejcts with methods and variables underneath */
    addDHTObject(server, "DHT11");
    addLEDObject(server, "LED");

    UA_StatusCode retval = UA_Server_run(server, &running);
    UA_Server_delete(server);
    UA_ServerConfig_delete(config);
    return (int)retval;
}
