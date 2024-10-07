#ifndef GLOBALS_H
#define GLOBALS_H

#include "clist.h"

//Server Name
extern const char *SERVER_NAME;
//Server IP
extern const char *SERVER_IP;
//Server Port
extern int        SERVER_PORT;
//Server Topic
extern const char *SERVER_TOPIC;
//Ping Send time
extern int     PING_SEND;
//Ping Timeout
extern int     PING_TIMEOUT;
// Client list
extern client_list list;

#endif
