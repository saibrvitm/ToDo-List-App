#include <stdbool.h>
#include <stdlib.h>

struct taskStructure
{
    int taskId;
    bool taskCompleted;
    char taskName[50];
    char taskDescription[200];
};

#define sTs struct taskStructure

sTs taskList[10] = {
    {1, true, "Task 1", "Task 1 description"},
    {2, false, "Task 2", "Task 2 description"},
    {3, true, "Task 3", "Task 3 description"},
    {4, false, "Task 4", "Task 4 description"},
    {5, false, "Task 5", "Task 5 description"},
    {6, false, "Task 6", "Task 6 description"},
    {7, false, "Task 7", "Task 7 description"},
    {8, true, "Task 8", "Task 8 description"},
    {9, false, "Task 9", "Task 9 description"},
    {10, false, "Task 10", "Task 10 description"},
};

void clearScreen()
{
    // This function is used to clear the terminal screen of all characters.
    // It works by printing the ANSI escape sequence for clearing the screen.
    printf("\033[H\033[J");
}

char headerText[50] = "\t# TO-DO APPLICATION #\t\t\t\t\t";
char headerTimeText[15] = "00:00:00 AM";
char footerOptionsText[100] = "        [S]earch        [M]odify Task        [V]iew Task        [C]omplete Task        [Q]uit";
char dividerText[100] = "----------------------------------------------------------------------------------------------------";
char lastOutputText[100] = "Awaiting key input...";