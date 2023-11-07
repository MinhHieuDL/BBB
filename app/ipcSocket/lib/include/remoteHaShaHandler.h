#ifndef REMOTE_HANDSHAKE_HANDLER
#define REMOTE_HANDSHAKE_HANDLER

#include "stdbool.h"

typedef struct loginMsg
{
    char* pcUserLogin;
    char* pcUserPsw;
}loginMsg;

bool ServerInit(int* iServerFD);
bool ClientInit(int* iClientFD);
bool SendLoginMsg(int iClientFD, loginMsg loginInfo);
bool ReadLoginMSG(int iChanFD, loginMsg* pLoginInfo);

#endif