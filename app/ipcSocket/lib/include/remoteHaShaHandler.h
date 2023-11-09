#ifndef REMOTE_HANDSHAKE_HANDLER
#define REMOTE_HANDSHAKE_HANDLER

#include <stdbool.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct loginMsg
{
    char pcUserLogin[30];
    char pcUserPsw[30];
}loginMsg;

// server API
bool ServerInit(int* iServerFD, struct sockaddr_in st_address);
bool WaitForClientConnect(int iServerFD, struct sockaddr_in st_address, int* p_iNewChan);

// Client API
bool ClientInit(int* iClientFD);
bool ServerConnect(int iServerFD, const char* sServerAdd, int iPort);

// MSG handling
bool SendLoginMsg(int iClientFD, char* sUser, char* sPwd);
bool ReadLoginMSG(int iChanFD, loginMsg* pLoginInfo);

#endif