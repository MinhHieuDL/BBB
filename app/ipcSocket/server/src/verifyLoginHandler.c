#include "verifyLoginHandler.h"
#include "fileHandler.h"
#include "csvHandler.h"
#include "encodeHandler.h"

#define DBFileName "userDB.csv"
#define DBDelim ","
#define USER_POS 1
#define PSW_POS 2

// internal API
char* findPswOnUser(char* pcUsername);

// define API

bool verifyPassword(char* pcUserName, char* pcPass)
{
    bool bRet = false;

    // find the password on user name
    char* pStoredPass = findPswOnUser(pcUserName);
    
    // Hash and compare the received password
    unsigned char hash[EVP_MAX_MD_SIZE];
    if(pStoredPass) {
        if(encodePsw(pcPass, hash))
        {
            bRet = (memcmp(hash, pStoredPass, EVP_MAX_MD_SIZE) == 0);
        }
        free(pStoredPass);
    }
    return bRet;
}

char* findPswOnUser(char* pcUsername)
{  
    // read database file 
    FILE* pFp = fopen(DBFileName, "r");

    if (!pFp) {
        perror("Open failed!");
        return NULL;
    }

    char *pLine = NULL;
    const char *pUser = NULL, *pPsw = NULL;

    while (readNextLine(pFp, &pLine)) {
        if(pLine)
        {
            // Parse csv token to find the password of user
            pUser = parseCSVToken(pLine, DBDelim, USER_POS);
            if(pUser)
            {
                if(strcmp(pUser, pcUsername) == 0)
                {
                    pPsw = parseCSVToken(pLine, DBDelim, PSW_POS);
                    free((void*)pUser); // Cast to void* before freeing
                    break;
                }
                free((void*)pUser); // Cast to void* before freeing
            }
            free(pLine);
            pLine = NULL;
        }
    }

    free(pLine);
    fclose(pFp);

    return (char*)pPsw; // Cast to char* to match the return type
}
