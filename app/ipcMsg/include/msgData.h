#ifndef IPC_MESSAGE_FORM
#define IPC_MESSAGE_FORM

#include <sys/types.h>

typedef struct msgform
{
    long m_ltype;
    char m_text[256];
}msgform;

#endif //IPC_MESSAGE_FORM