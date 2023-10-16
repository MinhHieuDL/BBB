#include "fileHandler.h"

// Read next line in file
bool readNextLine(const char* pFile, char* pBuff) {
    bool bRet = true;
    int iReadByte = 0;

    FILE* pFp = fopen(pFile, "r");

    if(!pFp)
    {
        perror("Open failed!");
        return false;
    }
    if((iReadByte = getline(pBuff, iReadByte, pFp)) == -1)
    {
        perror("Read failed");
        bRet = false;
    }
    else {
        // Remove trailing newline character
        if (pBuff[iReadByte - 1] == '\n') 
            pBuff[iReadByte - 1] = '\0';
    }

    fclose(pFp);
    return bRet;
}

bool addNewLine(const char* pFile, const char* pBuff) {
    bool bRet = true;

    FILE* pFp = fopen(pFile, "a+");

    if(!pFp)
    {
        perror("Open failed!");
        return false;
    }
    if(fprintf(pFp, "%s\n", pBuff) <= 0)
    {
        perror("Write failed");
        bRet = false;
    }

    fclose(pFp);
    return bRet;
}

bool delLine(const char* pFile, short iLineNum) {
    //to do
    return false;
}

bool addFile(const char* pFile) {
    bool bRet = true; 

    FILE* file = fopen(pFile, "r");

    if (file == NULL) {
        // File doesn't exist, so create it
        file = fopen(pFile, "w");

        if (file == NULL) {
            perror("Create file failed");
            bRet = false;
        }
    }

    fclose(file); // Close the file if it exists
    return false;
}

void printFile(const char* pFile) {
    bool bRet = true;

    FILE* pFp = fopen(pFile, "r");

    if(!pFp)
    {
        perror("Open failed!");
        return false;
    }

    char *pLine;

    while (readNextLine(pFp, pLine)) {
        printf("Line: %s\n", pLine);
    }
    
    fclose(pFp);
    return bRet;
}
