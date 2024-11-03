#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>
#include <stdlib.h>

typedef struct taskStructure {
    int taskId;
    char taskName[50];
    char taskDescription[100];
    bool taskCompleted;
} sTs;

void clearScreen();
void displayTaskList_T(sTs taskList[], size_t taskSize);
extern sTs taskList[10];
extern char headerText[50];
extern char headerTimeText[15];
extern char footerOptionsText[100];
extern char dividerText[100];
extern char lastOutputText[100];

#endif // UTIL_H
