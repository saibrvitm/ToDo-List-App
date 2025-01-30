#include <stdio.h>
#include "task_manager.h"

void save_tasks_to_file() {
    FILE *file = fopen(TASK_FILE, "w");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }

    for (int i = 0; i < task_count; i++) {
        fprintf(file, "%s|%s|%d|%s|%s\n", tasks[i].username, tasks[i].day, tasks[i].completed, tasks[i].task_name, tasks[i].task_description);
    }

    fclose(file);
}

void load_tasks_from_file() {
    FILE *file = fopen(TASK_FILE, "r");
    if (!file) {
        perror("Failed to open file for reading");
        return;
    }

    task_count = 0;
    while (fscanf(file, "%49[^|]|%49[^|]|%d|%255[^|]|%511[^\n]\n",
                  tasks[task_count].username,
                  tasks[task_count].day,
                  &tasks[task_count].completed,
                  tasks[task_count].task_name,
                  tasks[task_count].task_description) == 5) {
        task_count++;
        if (task_count >= MAX_TASKS) {
            break;
        }
    }

    fclose(file);
}

void add_task_to_file(Task task) {
    FILE *file = fopen(TASK_FILE, "a");
    if (!file) {
        perror("Failed to open file for appending");
        return;
    }

    fprintf(file, "%s|%s|%d|%s|%s\n", task.username, task.day, task.completed, task.task_name, task.task_description);
    fclose(file);
}

void delete_task_from_file(int index) {
    FILE *file = fopen(TASK_FILE, "w");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }

    for (int i = 0; i < task_count; i++) {
        if (i != index) {
            fprintf(file, "%s|%s|%d|%s|%s\n", tasks[i].username, tasks[i].day, tasks[i].completed, tasks[i].task_name, tasks[i].task_description);
        }
    }

    fclose(file);
}

void update_task_in_file(int index, Task updated_task) {
    tasks[index] = updated_task;
    save_tasks_to_file();
}