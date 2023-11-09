#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h> 
#include "remoteHaShaHandler.h"

bool ServerInit(int* piServerFD, struct sockaddr_in st_address) {
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


bool WaitForClientConnect(int iServerFD, struct sockaddr_in st_address, int* p_iNewChan)
{
    *p_iNewChan = accept(iServerFD, (struct sockaddr*)&st_address, (socklen_t*)sizeof(st_address));
    
    if(*p_iNewChan == -1) {
        perror("accept failed");
        return false;        
    }
    
    return true;
}


bool ClientInit(int* iClientFD) {
    int iCliDF;
    iCliDF = socket(AF_INET, SOCK_STREAM, 0);
    
    if(iCliDF < 0)
        return false;
    
    *iClientFD = iCliDF;
    return true;
}

bool ServerConnect(int iClientFD, const char* sServerAdd, int iPort) {
    struct  sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(iPort);
    
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if (inet_pton(AF_INET, sServerAdd, &serv_addr.sin_addr) <= 0) { 
        printf("Invalid address -- Address not supported \n"); 
        return false; 
    } 
  
    if (connect(iClientFD, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) { 
        perror("connect Failed"); 
        return false; 
    }
    
    return true;
}

bool SendLoginMsg(int iClientFD, loginMsg loginInfo) {
    // Packaging loginInfo for sending 
    struct iovec iov[2];
    iov[0].iov_base = loginInfo.pcUserLogin;
    iov[0].iov_len = strlen(loginInfo.pcUserLogin);
    iov[1].iov_base = loginInfo.pcUserPsw;
    iov[1].iov_len = strlen(loginInfo.pcUserPsw);

    struct msghdr msg;
    msg.msg_iov = iov;
    msg.msg_iovlen = sizeof(iov)/sizeof(struct iovec);

    // Send msg
    if(sendmsg(iClientFD, &msg, 0) == -1)
    {
        perror("sendmsg failed");
        return false;
    }

    return true;
}

bool ReadLoginMSG(int iChanFD, loginMsg* pLoginInfo) {
    // Unpackaging loginInfo received from client 
    struct iovec iov[2];
    struct msghdr msg;

    iov[0].iov_base = pLoginInfo->pcUserLogin;
    iov[0].iov_len = sizeof(pLoginInfo->pcUserLogin);
    iov[1].iov_base = pLoginInfo->pcUserPsw;
    iov[1].iov_len = sizeof(pLoginInfo->pcUserPsw);
    
    // read msg from client
    if(recvmsg(iChanFD, &msg, 0) == -1)
    {
        perror("recvmsg failed");
        return false;
    }
    
    return true;
}
