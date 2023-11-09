#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <unistd.h> 
#include "remoteHaShaHandler.h"

//*******************Internal API**********************//
loginMsg FormatMsgForSending(char* pcUser, char* pcPwd);
loginMsg ParsingMsgReceived(char* pcRevMSG);

//*******************Define API**********************//
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
    socklen_t addrlen = sizeof(st_address);
    *p_iNewChan = accept(iServerFD, (struct sockaddr*)&st_address, &addrlen);
    if(*p_iNewChan < 0) {
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

bool SendLoginMsg(int iClientFD, char* sUser, char* sPwd) {
    // Format msg before sending
    loginMsg loginInfo = FormatMsgForSending(sUser, sPwd);

    // Packaging loginInfo for sending 
    struct iovec iov[2];
    iov[0].iov_base = loginInfo.pcUserLogin;
    iov[0].iov_len = strlen(loginInfo.pcUserLogin);
    iov[1].iov_base = loginInfo.pcUserPsw;
    iov[1].iov_len = strlen(loginInfo.pcUserPsw);

    struct msghdr msg = {};
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
    char buff[100] = {};
    // read msg from client
    if(recv(iChanFD, buff, sizeof(buff), 0) == -1)
    {
        perror("recvmsg failed");
        return false;
    }

    // Parsing login info from buffer
    *pLoginInfo = ParsingMsgReceived(buff);

    return true;
}

loginMsg FormatMsgForSending(char* pcUser, char* pcPwd) {
    loginMsg retMSG = {};
    strcpy(retMSG.pcUserLogin, pcUser);  
    strcat(retMSG.pcUserLogin, "-");     
    strcpy(retMSG.pcUserPsw, pcPwd);
    return retMSG;
}

loginMsg ParsingMsgReceived(char* pcRevMSG) {
    loginMsg parsedMsg = {};

    // Find the position of the dash '-'
    const char* dashPosition = strchr(pcRevMSG, '-');

    if (dashPosition != NULL) {
        size_t userLength = dashPosition - pcRevMSG;
        
        strncpy(parsedMsg.pcUserLogin, pcRevMSG, userLength);
        parsedMsg.pcUserLogin[userLength] = '\0';

        strcpy(parsedMsg.pcUserPsw, dashPosition + 1);
    } else {
        // If no dash found, consider the whole string as the user part
        strcpy(parsedMsg.pcUserLogin, pcRevMSG);
        parsedMsg.pcUserPsw[0] = '\0'; 
    }

    return parsedMsg;
}