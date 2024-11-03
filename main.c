#include <stdio.h>
#include <string.h>

#include "client.c"

int main()
{
    while (true)
    {
        clearScreen();
        displayTaskList_T(taskList, sizeof(taskList) / sizeof(taskList[0]));

        char userChoice = getchar();
        if (userChoice == '\n')
        {
            continue;
        }

        switch (userChoice)
        {
        case 'S':
        case 's':
            strcpy(lastOutputText, "| SEARCH FOR TASK:");
            clearScreen();
            displayTaskList_T(taskList, sizeof(taskList) / sizeof(taskList[0]));

            int userInputChoice;
            scanf(" %d", &userInputChoice);

            // Check for newline input
            if (userInputChoice == '\n')
            {
                continue; // Exit the function if newline
            }

            clearScreen();
            strcpy(lastOutputText, "| DISPLAYING RESULTS... (ENTER x TO EXIT) ");

            sTs mutatedTask;

            // Search for the task with the specified ID
            for (size_t i = 0; i < sizeof(taskList) / sizeof(taskList[0]); i++)
            {
                if (taskList[i].taskId == userInputChoice)
                {
                    strcpy(mutatedTask.taskName, taskList[i].taskName);
                    strcpy(mutatedTask.taskDescription, taskList[i].taskDescription);
                    mutatedTask.taskId = taskList[i].taskId;
                    mutatedTask.taskCompleted = taskList[i].taskCompleted;
                    break;
                }
            }

            sTs newTaskArr[] = {mutatedTask};
            char newInputChoice;

            // Loop to display the task details and wait for user input
            while (true)
            {
                clearScreen();
                displayTaskList(newTaskArr, sizeof(newTaskArr) / sizeof(newTaskArr[0]), false);

                scanf(" %c", &newInputChoice);
                if (newInputChoice == 'x')
                {
                    break; // Exit the loop if user enters 'x'
                }
            }

            strcpy(lastOutputText, "Awaiting key input...");
            break;
        case 'M':
        case 'm':
            strcpy(lastOutputText, "| TASK TO MODIFY (TASK NUMBER):");
            break;
        case 'V':
        case 'v':
            strcpy(lastOutputText, "| TASK TO VIEW (TASK NUMBER):");
            break;
        case 'C':
        case 'c':
            strcpy(lastOutputText, "| TASK TO MARK COMPLETE (TASK NUMBER):");
            break;
        case 'Q':
        case 'q':
            return 0;
        default:
            strcpy(lastOutputText, "[INVLD INPT] Awaiting key input...");
            break;
        }
    }

    return 0;
}
