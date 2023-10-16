#include "userDBHandler.h"

int main(void)
{
    char userCMD;
    char cacheFile[100];
    memset(cacheFile, 0, sizeof(cacheFile));

    printf("***** Database support tool *****\n");
    printf("Usage:\n"
                "a - create file\n"
                "l - add user login authentication\n"
                "p - print file\n"
                "c - cache file name for handling later\n"
                "q - quit\n");

    while (1)
    {
        printf("\nEnter command: ");
        scanf("%c", &userCMD);

        if(userCMD == 'q')
            break;

        switch (userCMD)
        {
            case 'a':
            {
                char fileName[100];
                printf("Enter file name: ");
                scanf("%s", fileName);
                addFile(fileName);
            }
            break;
            
            case 'l':
            {
                char fileName[100];
                memcpy(fileName, cacheFile, sizeof(cacheFile));
                if(fileName[0] == '\0')
                {
                    printf("Not any cache file before, enter file name:");
                    scanf("%s", fileName);
                    getchar();
                }
                updateDB(fileName);
            }
            break;

            case 'p':
            {
                char fileName[100];
                memcpy(fileName, cacheFile, sizeof(cacheFile));
                if(fileName[0] == '\0')
                {
                    printf("Not any cache file before, enter file name:");
                    scanf("%s", fileName);
                    getchar();
                }
                printFile(fileName);
            }
            break;
            
            case 'c':
            {
                printf("Enter file name: ");
                scanf("%s", cacheFile);
                printf("Cache file: %s for handling later\n", cacheFile);
            }
            break;

            default:
                printf("Invalid command");
                break;
        }
        getchar(); // Consume the newline character from the input buffer
    }
    
    return 0;
}