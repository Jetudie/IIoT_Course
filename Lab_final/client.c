#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include "open62541.h"

static void handler_TheAnswerChanged(UA_Client *client, const UA_UInt32 monId, const UA_Variant *value, void *context)
{
	UA_Variant value1; /* Variants can hold scalar values and arrays of any type */
	UA_Variant_init(&value1);

	/* NodeId of the variable holding the current time */
	const UA_NodeId nodeId1 = UA_NODEID_NUMERIC(0, UA_NS0ID_SERVER_SERVERSTATUS_CURRENTTIME);
	UA_StatusCode retval = UA_Client_readValueAttribute(client, nodeId1, &value1);

	if(retval == UA_STATUSCODE_GOOD && UA_Variant_hasScalarType(&value1, &UA_TYPES[UA_TYPES_DATETIME])) {
		UA_DateTime raw_date = *(UA_DateTime *) value1.data;
		UA_DateTimeStruct dts = UA_DateTime_toStruct(raw_date);
		UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "date is: %u-%u-%u %u:%u:%u.%03u",
			    dts.day, dts.month, dts.year, dts.hour, dts.min, dts.sec, dts.milliSec);
	}

	/* NodeId of the variable holding the context */
	const UA_NodeId nodeId2 = UA_NODEID_STRING(1, context);
	retval = UA_Client_readValueAttribute(client, nodeId2, &value1);
	if(retval == UA_STATUSCODE_GOOD){
		UA_Int32 object_num = *(UA_Int32*)value1.data;
		printf("%s: %d", context, object_num);
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

	/* create new client */
    	UA_Client *client = UA_Client_new(UA_ClientConfig_default);
    	//UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://192.168.43.193");
    	UA_StatusCode retval = UA_Client_connect(client, "opc.tcp://localhost:4840");


    	if(retval != UA_STATUSCODE_GOOD) {
        	UA_Client_delete(client);
        	return (int)retval;
    	}

	/* Read the value attribute of the node. UA_Client_readValueAttribute is a
	* wrapper for the raw read service available as UA_Client_Service_read. */
	UA_Variant value; /* Variants can hold scalar values and arrays of any type */
	UA_Variant_init(&value);
	
	/* Subscription */
	UA_UInt32 subId1 = 0;
	UA_UInt32 subId2 = 0;
	UA_UInt32 subId3 = 0;
	UA_Client_Subscriptions_new(client, UA_SubscriptionSettings_default, &subId1);
	UA_Client_Subscriptions_new(client, UA_SubscriptionSettings_default, &subId2);
	UA_Client_Subscriptions_new(client, UA_SubscriptionSettings_default, &subId3);

	if(subId1 && subId2){
		printf("Create subscription suceeded, id %u\n", subId1);
		printf("Create subscription suceeded, id %u\n", subId2);
		printf("Create subscription suceeded, id %u\n", subId3);
	}

	char hfContext[3][7] = {"blue", "green", "red"};
	UA_NodeId monitorThis[3] = {UA_NODEID_STRING(1, "blue"), UA_NODEID_STRING(1, "green"), UA_NODEID_STRING(1, "red")};
	UA_UInt32 monId1 = 0;
	UA_UInt32 monId2 = 0;
	UA_UInt32 monId3 = 0;
	UA_Client_Subscriptions_addMonitoredItem(client, subId1, monitorThis[0], UA_ATTRIBUTEID_VALUE, handler_TheAnswerChanged, hfContext[0], &monId1, 0.3);
	UA_Client_Subscriptions_addMonitoredItem(client, subId2, monitorThis[1], UA_ATTRIBUTEID_VALUE, handler_TheAnswerChanged, hfContext[1], &monId2, 0.3);
	UA_Client_Subscriptions_addMonitoredItem(client, subId3, monitorThis[2], UA_ATTRIBUTEID_VALUE, handler_TheAnswerChanged, hfContext[2], &monId2, 0.3);

	if(monId1 && monId2 && monId3){
		printf("Monitoring 'Temp' and 'Hum', id %u\n", monId1);
		printf("Monitoring 'Temp' and 'Hum', id %u\n", monId2);
		printf("Monitoring 'Temp' and 'Hum', id %u\n", monId3);
	}

	while(running){
		sleep(1);
		UA_Client_Subscriptions_manuallySendPublishRequest(client);
	}

	/* Clean up */
	UA_Client_disconnect(client); /* Disconnects the client internally */
}
