#ifndef FILE_HANDLING
#define FILE_HANDLING

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

bool readNextLine(FILE* pFile, char** pBuff);
bool addNewLine(const char* pFile, const char* pBuff);
bool delLine(const char* pFile, short iLineNum);
bool addFile(const char* pFile);
void printFile(const char* pFile);

#endif