#ifndef REMOTE_HANDSHAKE_HANDLER
#define REMOTE_HANDSHAKE_HANDLER

#include <stdbool.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct loginMsg
{
    char* pcUserLogin;
    char* pcUserPsw;
}loginMsg;

// server API
bool ServerInit(int* iServerFD, int iPort, struct sockaddr_in st_address);
bool WaitForClientConnect(int iServerFD, struct sockaddr st_address, int* p_iNewChan);

// Client API
bool ClientInit(int* iClientFD);
bool ServerConnect(int iServerFD, const char* sServerAdd, int iPort);


bool SendLoginMsg(int iClientFD, loginMsg loginInfo);
bool ReadLoginMSG(int iChanFD, loginMsg* pLoginInfo);

#endif