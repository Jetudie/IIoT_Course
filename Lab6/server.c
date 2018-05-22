#include <signal.h>
#include "open62541.h"

UA_Boolean running = true;
static void stopHandler(int sig) {
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_USERLAND, "received ctrl-c");
    running = false;
}

int main(void) {
    signal(SIGINT, stopHandler);
    signal(SIGTERM, stopHandler);

    UA_ServerConfig *config = UA_ServerConfig_new_default();
    UA_Server *server = UA_Server_new(config);

    /* Lab6 : show time and student ID */
    UA_DateTime ServerTime;
    UA_String StudentID;

    char* ID = "N96061478";
    StudentID = UA_String_fromChars(ID);
    ServerTime = UA_DateTime_now();
    UA_String Time = UA_DateTime_toString(ServerTime);

    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, StudentID.data);
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_SERVER, Time.data);
    UA_StatusCode retval = UA_Server_run(server, &running);
    UA_Server_delete(server);
    UA_ServerConfig_delete(config);
    return (int)retval;
}
