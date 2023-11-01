#include "userDBHandler.h"
#include "encodeHandler.h"

#define MAX_USER_SIZE   30
#define MAX_PASSWORD_SIZE   30

//************* define api *********************


void updateDB(const char* pcDBFile)
{
    char userCMD;
    printf("\n---------------------------------------\nUpdate mode:\n"
            "a - add user and password to database\n"
            "b - back to main mode\n");

    while(1)
    {
        printf("update mode - enter command: ");
        scanf(" %c", &userCMD);
        
        if(userCMD == 'b')
            break;
        
        switch (userCMD)
        {
            case 'a':
            {
                char user[MAX_USER_SIZE];
                char passwd[MAX_PASSWORD_SIZE];
                printf("user: ");
                scanf(" %s", user);
                printf("password: ");
                scanf(" %s", passwd);
                unsigned char hash[EVP_MAX_MD_SIZE];
                if(encodePasswod(passwd, hash))
                {
                    char newLineDB[MAX_USER_SIZE + EVP_MAX_MD_SIZE * 2 + 1];
                    sprintf(newLineDB, "%s,", user);
                    for (unsigned int i = 0; i < EVP_MD_size(EVP_sha256()); i++) 
                    {
                        sprintf(newLineDB + strlen(newLineDB), "%02x", hash[i]);
                    }
                    printf("Hashed Password: %s\n", newLineDB);
                    addNewLine(pcDBFile, newLineDB);
                }
                else 
                    printf("encode password failed, please check and update again\n");
            }    
            break;

            default:
                printf("Update mode, invalid cmd: %c\n", userCMD);
            break;
        }
        getchar();
    }
    printf("Back to main mode\n");
    return;
}