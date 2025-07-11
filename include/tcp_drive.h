#ifndef LIB_TCPSERVER_H_
#define LIB_TCPSERVER_H_
#include "RespParser.h"

#ifdef __cplusplus
extern "C" {

#endif


void startServer();
void handleClient(int clientSocket);
int onConnectionInit();
int ProcessMessage(const char* buf, uint16_t len);
void ExecuteCommand(RespMessage* msg, void* arg);


#ifdef __cplusplus
}
#endif

#endif /* LIB_TCPCALLBACK_H_ */