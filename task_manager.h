#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include <gtk-3.0/gtk/gtk.h>
#include <stdbool.h>

#define MAX_TASKS 100
#define TASK_FILE "data/tasks.txt"
#define ENTER 13
#define BCKSPC 8
#define DATA_FILE "data/users.dat"

typedef struct {
    char username[50];
    char password[50];
} User;

typedef struct {
    char username[50];
    char day[50];
    char task_name[256];
    char task_description[512];
    bool completed;
} Task;

extern Task tasks[MAX_TASKS];
extern int task_count;
extern char* g_username;
extern char GLOBAL_USERNAME[50];

// Function declarations
void update_clock(GtkLabel *clock_label);
void populate_task_list(GtkListBox *list_box, GtkLabel *task_count_label, const char* filter_day);
void save_tasks_to_file();
void load_tasks_from_file();
void add_task_to_file(Task task);
void delete_task_from_file(int index);
void update_task_in_file(int index, Task updated_task);
void on_add_task_button_clicked(GtkButton *button, gpointer user_data);
void on_edit_task_button_clicked(GtkButton *button, gpointer user_data);
void on_delete_task_button_clicked(GtkButton *button, gpointer user_data);
void on_complete_task_button_clicked(GtkButton *button, gpointer user_data);
void on_combobox_button_clicked(GtkComboBoxText *combo_box, gpointer user_data);
void takeInput(const char *prompt, char *input);
void takePassword(const char *prompt, char *password);
void signUp();
int login();
char* auth();

#endif // TASK_MANAGER_H
