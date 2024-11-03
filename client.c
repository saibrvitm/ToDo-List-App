#include "util.c"

void displayTaskList(sTs taskList[], size_t taskSize, bool showMissingTasks)
{
    printf("%s", headerText);
    printf("%s\n", headerTimeText);
    printf("%s\n", dividerText);

    for (size_t i = 0; i < taskSize; i++)
    {
        sTs task = taskList[i];

        if (task.taskCompleted)
        {
            printf("%d. [%s] %s :: %s\n", task.taskId, "X", task.taskName, task.taskDescription);
        }
        else
        {
            printf("%d. [%s] %s :: %s\n", task.taskId, " ", task.taskName, task.taskDescription);
        }
    }

    if (taskSize != 10) {
        for (size_t i = taskSize; i < 10; i++) {
            if (showMissingTasks) {
                printf("%zd. [%s] %s :: %s\n", i + 1, " ", " ", " ");
            }
            else {
                printf("\n");
            }
        }
    }

    printf("%s\n", dividerText);
    printf("%s\n", footerOptionsText);
    printf("%s ", lastOutputText);
}

void displayTaskList_T(sTs taskList[], size_t taskSize) {
    displayTaskList(taskList, taskSize, true);
}