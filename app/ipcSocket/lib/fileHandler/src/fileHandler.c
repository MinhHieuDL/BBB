#include "fileHandler.h"

// Read next line in file
bool readNextLine(FILE* pFile, char** pBuff) {
    size_t buffLen = 0;

    if (getline(pBuff, &buffLen, pFile) == -1) {
        return false;
    }

    // Remove trailing newline character
    size_t lineLen = strlen(*pBuff);
    if (lineLen > 0 && (*pBuff)[lineLen - 1] == '\n') {
        pBuff[lineLen - 1] = '\0';
    }

    return true;
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
    FILE* pFp = fopen(pFile, "r");

    if (!pFp) {
        perror("Open failed!");
        return;
    }

    char *pLine = NULL;

    while (readNextLine(pFp, &pLine)) {
        if(pLine)
        {
            printf("%s", pLine);
            free(pLine);
            pLine = NULL;
        }
    }

    free(pLine);
    fclose(pFp);
}