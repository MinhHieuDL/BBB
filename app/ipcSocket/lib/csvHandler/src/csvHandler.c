#include "csvHandler.h"

const char* parseCSVToken(char* pData,const char* cDelim, unsigned iId)
{
    if (pData) 
    {
        char* pDataDup = strdup(pData); // Create a duplicate of pData
        const char* tok = NULL;
        for (tok = strtok(pDataDup, cDelim);
             tok && *tok;
             tok = strtok(NULL, cDelim))
        {
            if (!--iId) {
                // Allocate a copy of the token, so it remains valid after the function
                const char* result = strdup(tok);
                free(pDataDup); // Free the duplicated pDataDup
                return result;
            }
        }

        free(pDataDup); // Free the entire duplicated pDataDup if the desired token was not found
    }

    return NULL;
}