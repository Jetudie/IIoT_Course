#include <stdio.h>
#include <signal.h>
#include "open62541.h"

static void handler_TheAnswerChanged(UA_Client *client, const UA_UInt32 monId, const UA_Variant *value, void *context)
{
	UA_Variant value1;
	UA_Variant_init(&value1);

	const UA_NodeId nodeId = UA_NODEID_STRING(1, "status");
	UA_StatusCode retval = UA_Client_readValueAttribute(client, nodeId, &value1);
	if(retval == UA_STATUSCODE_GOOD){
		UA_String str = *(UA_String*)value1.data;
		printf("Status changed to %.*s\n", (int)str.length, str.data);
	}
}


UA_Boolean running = true;
static void stopHandler(int sig)
{
	UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "received ctrl-c");
	running = false;
}

int main(void) {
	signal(SIGINT, stopHandler);
	signal(SIGTERM, stopHandler);

    	UA_Client *client = UA_Client_new(UA_ClientConfig_default);
    	UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://localhost:4840");
    	//UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://192.168.1.110:4840", "user1", "user1passwd")

    	if(retval != UA_STATUSCODE_GOOD) {
        	UA_Client_delete(client);
        	return (int)retval;
    	}

	/* Read the value attribute of the node. UA_Client_readValueAttribute is a
	* wrapper for the raw read service available as UA_Client_Service_read. */
	UA_Variant value; /* Variants can hold scalar values and arrays of any type */
	UA_Variant_init(&value);

	/* NodeId of the variable */
	//const UA_NodeId nodeId = UA_NODEID_STRING(1, "status");
	//retval = UA_Client_readValueAttribute(client, nodeId, &value);

	/*if(retval == UA_STATUSCODE_GOOD){
		UA_String str = *(UA_String*)value.data;
		printf("Status is: %.*s\n", (int) str.length, str.data);
	}else{
		UA_Client_delete(client);
		return (int)retval;
	}*/
	
	/* Subscription */
	UA_UInt32 subId = 0;
	UA_Client_Subscriptions_new(client, UA_SubscriptionSettings_default, &subId);
	if(subId)
		printf("Create subscription suceeded, id %u\n", subId);
	UA_NodeId monitorThis = UA_NODEID_STRING(1, "status");
	UA_UInt32 monId = 0;
	UA_Client_Subscriptions_addMonitoredItem(client, subId, monitorThis, UA_ATTRIBUTEID_VALUE, handler_TheAnswerChanged, "Handler", &monId, 1);

	if(monId)
		printf("Monitoring 'Status', id %u\n", subId);

	while(running){
		sleep(1);
		UA_Client_Subscriptions_manuallySendPublishRequest(client);
	}


	/* Clean up */
	UA_Client_disconnect(client); /* Disconnects the client internally */
}
