#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include "remoteHaShaHandler.h"

bool ServerInit(int* piServerFD, int iPort, struct sockaddr_in st_address) {
    int iServerDes;
    int opt = 1;

    // Create socket file description 
    if((iServerDes = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        return false;
    }

    // Set option for able to reuse the address
    if (setsockopt(iServerDes, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        return false;
    }

    // Forcefully attaching socket to the port  
    if (bind(iServerDes, (struct sockaddr*)&st_address, sizeof(st_address)) < 0) {
        perror("bind failed");
        return false;
    }
    if(listen(iServerDes, 3) < 0){
        perror("listen");
        return false;
    }

    // return server file description
    *piServerFD = iServerDes;
    return true;
}


bool WaitForClientConnect(int iServerFD, struct sockaddr st_address, int* p_iNewChan)
{
    *p_iNewChan = accept(iServerFD, &st_address, sizeof(st_address));
    return ( (*p_iNewChan) != -1 );
}


bool ClientInit(int* iClientFD) {
    return false;
}

bool SendLoginMsg(int iClientFD, loginMsg loginInfo) {
    return false;
}

bool ReadLoginMSG(int iChanFD, loginMsg* pLoginInfo) {
    return false;
}
