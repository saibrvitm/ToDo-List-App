#include <gtk-3.0/gtk/gtk.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#define MAX_TASKS 100
#define TASK_FILE "./tasks.txt"
#define ENTER 13
#define BCKSPC 8
#define DATA_FILE "./users.dat"

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

Task tasks[MAX_TASKS];
int task_count = 0;
char* g_username;
char GLOBAL_USERNAME[50];

static void update_clock(GtkLabel *clock_label);
static void populate_task_list(GtkListBox *list_box, GtkLabel *task_count_label, const char* filter_day);
static void save_tasks_to_file();
static void load_tasks_from_file();
static void add_task_to_file(Task task);
static void delete_task_from_file(int index);
static void update_task_in_file(int index, Task updated_task);
static void on_add_task_button_clicked(GtkButton *button, gpointer user_data);
static void on_edit_task_button_clicked(GtkButton *button, gpointer user_data);
static void on_delete_task_button_clicked(GtkButton *button, gpointer user_data);
static void on_complete_task_button_clicked(GtkButton *button, gpointer user_data);
static void on_combobox_button_clicked(GtkComboBoxText *combo_box, gpointer user_data);
static void takeInput(const char *prompt, char *input);
static void takePassword(const char *prompt, char *password);
static void signUp();
static int login();
static char* auth();

int main(int argc, char *argv[]) {
    g_username = auth();
    if (g_strcmp0(g_username, "null") == 0)
        return 0;
    printf("Authentication successful! ~ Using tasks of %s\n", g_username);

    GtkBuilder *builder;
    GtkWidget *window;
    GtkWidget *add_task_button, *edit_task_button, *delete_task_button, *complete_task_button;
    GtkComboBoxText *combo_box;
    GtkLabel *clock_label, *task_count_label;
    GtkListBox *task_list;
    GtkEntry *task_name_entry, *task_description_entry;

    gtk_init(&argc, &argv);

    /** LOAD UI FROM FILE */
    const char* ui_filename = "builder.xml";
    builder = gtk_builder_new_from_file(ui_filename);

    /** MAIN WINDOW FROM BUILDER */
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    g_signal_connect(GTK_WINDOW(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_title(GTK_WINDOW(window), "To-Do App");

    /** TASK COUNT AND CLOCK */
    task_count_label = GTK_LABEL(gtk_builder_get_object(builder, "label_hero_a"));
    clock_label = GTK_LABEL(gtk_builder_get_object(builder, "label_hero_b"));
    g_timeout_add_seconds(1, (GSourceFunc)update_clock, clock_label);

    /** TASK INFO ENTRIES AND LISTBOX */
    task_name_entry = GTK_ENTRY(gtk_builder_get_object(builder, "task_name_entry"));
    task_description_entry = GTK_ENTRY(gtk_builder_get_object(builder, "task_description_entry"));
    task_list = GTK_LIST_BOX(gtk_builder_get_object(builder, "task_list"));

    /** COMBOBOX */
    gpointer taskinfo_data[] = {task_list, task_count_label};
    combo_box = GTK_COMBO_BOX_TEXT(gtk_builder_get_object(builder, "task_day_combo_box"));
    g_signal_connect(combo_box, "changed", G_CALLBACK(on_combobox_button_clicked), taskinfo_data);

    /** FUNCTION DATA */
    gpointer user_data[] = {task_name_entry, task_description_entry, task_list, task_count_label, combo_box};

    /** BUTTONS */
    add_task_button = GTK_WIDGET(gtk_builder_get_object(builder, "create_task_button"));
    g_signal_connect(add_task_button, "clicked", G_CALLBACK(on_add_task_button_clicked), user_data);

    edit_task_button = GTK_WIDGET(gtk_builder_get_object(builder, "update_task_button"));
    g_signal_connect(edit_task_button, "clicked", G_CALLBACK(on_edit_task_button_clicked), user_data);

    delete_task_button = GTK_WIDGET(gtk_builder_get_object(builder, "delete_task_button"));
    g_signal_connect(delete_task_button, "clicked", G_CALLBACK(on_delete_task_button_clicked), user_data);

    complete_task_button = GTK_WIDGET(gtk_builder_get_object(builder, "complete_task_button"));
    g_signal_connect(complete_task_button, "clicked", G_CALLBACK(on_complete_task_button_clicked), user_data);

    /** LOAD TASKS FROM FILE */
    load_tasks_from_file();

    /** INITIALIZE UI */
    populate_task_list(task_list, task_count_label, "Default");
    gtk_widget_grab_focus(GTK_WIDGET(combo_box));
    gtk_widget_show_all(window);
    gtk_window_present(GTK_WINDOW(window));
    gtk_main();

    return 0;
}

static void update_clock(GtkLabel *clock_label) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_str[12];

    strftime(time_str, sizeof(time_str), "%H:%M:%S %p", tm_info);
    gtk_label_set_text(clock_label, time_str);
}

static void update_task_count_label(GtkLabel *task_count_label) {
    int completed_tasks = 0;
    int total_task_count = 0;
    for (int i = 0; i < task_count; i++) {
        if (g_strcmp0(g_username, tasks[i].username) != 0) continue;
        if (tasks[i].completed) {
            completed_tasks++;
        }
        total_task_count++;
    }
    char label_text[50];
    sprintf(label_text, "%d / %d Tasks Completed", completed_tasks, total_task_count);
    gtk_label_set_text(task_count_label, label_text);
}

static void save_tasks_to_file() {
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

static void load_tasks_from_file() {
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

static void add_task_to_file(Task task) {
    FILE *file = fopen(TASK_FILE, "a");
    if (!file) {
        perror("Failed to open file for appending");
        return;
    }

    fprintf(file, "%s|%s|%d|%s|%s\n", task.username, task.day, task.completed, task.task_name, task.task_description);
    fclose(file);
}

static void delete_task_from_file(int index) {
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

static void update_task_in_file(int index, Task updated_task) {
    tasks[index] = updated_task;
    save_tasks_to_file();
}

static void populate_task_list(GtkListBox *list_box, GtkLabel *task_count_label, const char *filter_day) {
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(list_box));
    for (iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    for (int i = 0; i < task_count; i++) {
        if (g_strcmp0(filter_day, "Default") != 0 && g_strcmp0(filter_day, tasks[i].day) != 0) continue;
        if (g_strcmp0(g_username, tasks[i].username) != 0) continue;
        GtkWidget *task_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

        char task_str[501];
        sprintf(task_str, "%s [%s]  %s   -  %s", tasks[i].day, tasks[i].completed ? "\xE2\x9C\x85" : "\xE2\x9D\x8C", tasks[i].task_name, tasks[i].task_description);

        GtkWidget *name_label = gtk_label_new(task_str);

        gtk_box_pack_start(GTK_BOX(task_box), name_label, FALSE, FALSE, 0);

        GtkWidget *row = gtk_list_box_row_new();
        gtk_container_add(GTK_CONTAINER(row), task_box);
        gtk_list_box_insert(list_box, row, -1);
    }

    gtk_widget_show_all(GTK_WIDGET(list_box));
    update_task_count_label(task_count_label);
}

static void on_add_task_button_clicked(GtkButton *button, gpointer user_data) {
    GtkEntry *task_name_entry = GTK_ENTRY(((gpointer*)user_data)[0]);
    GtkEntry *task_description_entry = GTK_ENTRY(((gpointer*)user_data)[1]);
    GtkListBox *task_list = GTK_LIST_BOX(((gpointer*)user_data)[2]);
    GtkLabel *task_count_label = GTK_LABEL(((gpointer*)user_data)[3]);
    GtkComboBoxText *task_day_combo_box = GTK_COMBO_BOX_TEXT(((gpointer*)user_data)[4]);

    const char *task_name = gtk_entry_get_text(task_name_entry);
    const char *task_description = gtk_entry_get_text(task_description_entry);
    const char *task_day = gtk_combo_box_text_get_active_text(task_day_combo_box);

    if (task_count >= MAX_TASKS) {
        g_print("Task list is full!\n");
        return;
    }
    if (g_strcmp0(task_name, "") == 0 || g_strcmp0(task_description, "") == 0 || !task_name || !task_description || g_strcmp0(task_day, "Default") == 0) {
        g_print("Please fill in all fields and mention a day for the task.\n");
        return;
    }

    Task new_task;
    strncpy(new_task.task_name, task_name, sizeof(new_task.task_name) - 1);
    strncpy(new_task.task_description, task_description, sizeof(new_task.task_description) - 1);
    strncpy(new_task.day, task_day, sizeof(new_task.day) - 1);
    strncpy(new_task.username, g_username, sizeof(new_task.username) - 1);
    new_task.completed = false;

    tasks[task_count] = new_task;
    task_count++;

    add_task_to_file(new_task);
    populate_task_list(task_list, task_count_label, "Default");

    gtk_entry_set_text(task_name_entry, "");
    gtk_entry_set_text(task_description_entry, "");
    gtk_combo_box_set_active(GTK_COMBO_BOX(task_day_combo_box), 0);
}

static void on_edit_task_button_clicked(GtkButton *button, gpointer user_data) {
    GtkEntry *task_name_entry = GTK_ENTRY(((gpointer*)user_data)[0]);
    GtkEntry *task_description_entry = GTK_ENTRY(((gpointer*)user_data)[1]);
    GtkListBox *task_list = GTK_LIST_BOX(((gpointer*)user_data)[2]);
    GtkLabel *task_count_label = GTK_LABEL(((gpointer*)user_data)[3]);
    GtkComboBoxText *task_day_combo_box = GTK_COMBO_BOX_TEXT(((gpointer*)user_data)[4]);

    GtkListBoxRow *selected_row = gtk_list_box_get_selected_row(task_list);
    if (!selected_row) {
        g_print("No task selected for editing.\n");
        return;
    }

    const char *task_name = gtk_entry_get_text(task_name_entry);
    const char *task_description = gtk_entry_get_text(task_description_entry);
    int index = gtk_list_box_row_get_index(selected_row);

    if (g_strcmp0(task_name, "") != 0) {
        strncpy(tasks[index].task_name, task_name, sizeof(tasks[index].task_name) - 1);
    }
    if (g_strcmp0(task_description, "") != 0) {
        strncpy(tasks[index].task_description, task_description, sizeof(tasks[index].task_description) - 1);
    }

    update_task_in_file(index, tasks[index]);
    populate_task_list(task_list, task_count_label, "Default");

    gtk_entry_set_text(task_name_entry, "");
    gtk_entry_set_text(task_description_entry, "");
    gtk_combo_box_set_active(GTK_COMBO_BOX(task_day_combo_box), 0);
}

static void on_delete_task_button_clicked(GtkButton *button, gpointer user_data) {
    GtkListBox *task_list = GTK_LIST_BOX(((gpointer*)user_data)[2]);
    GtkLabel *task_count_label = GTK_LABEL(((gpointer*)user_data)[3]);

    GList *selected_rows = gtk_list_box_get_selected_rows(task_list);
    if (g_list_length(selected_rows) == 0) {
        g_print("No tasks selected!\n");
        return;
    }

    GList *iter;
    for (iter = selected_rows; iter != NULL; iter = g_list_next(iter)) {
        GtkListBoxRow *row = GTK_LIST_BOX_ROW(iter->data);
        int index = gtk_list_box_row_get_index(row);
        delete_task_from_file(index);

        for (int i = index; i < task_count - 1; i++) {
            tasks[i] = tasks[i + 1];
        }
        task_count--;
    }

    g_list_free(selected_rows);
    populate_task_list(task_list, task_count_label, "Default");
}

static void on_complete_task_button_clicked(GtkButton *button, gpointer user_data) {
    GtkListBox *task_list = GTK_LIST_BOX(((gpointer*)user_data)[2]);
    GtkLabel *task_count_label = GTK_LABEL(((gpointer*)user_data)[3]);
    GtkListBoxRow *selected_row = gtk_list_box_get_selected_row(task_list);
    if (!selected_row) {
        g_print("No task selected!\n");
        return;
    }

    int index = gtk_list_box_row_get_index(selected_row);

    tasks[index].completed = !tasks[index].completed;

    update_task_in_file(index, tasks[index]);
    populate_task_list(task_list, task_count_label, "Default");
}

static void on_combobox_button_clicked(GtkComboBoxText *combo_box, gpointer user_data) {
    const char *selected_option = gtk_combo_box_text_get_active_text(combo_box);
    if (selected_option) {
        populate_task_list(GTK_LIST_BOX(((gpointer*)user_data)[0]), GTK_LABEL(((gpointer*)user_data)[1]), selected_option);
        g_free((gchar *)selected_option); // Free the returned string
    }
}

static void takeInput(const char *prompt, char *input) {
    printf("%s", prompt);
    fgets(input, 50, stdin);
    input[strcspn(input, "\n")] = '\0';
}

static void takePassword(const char *prompt, char *password) {
    printf("%s", prompt);
    int i = 0;
    char ch;

    while (1) {
        ch = getch();
        if (ch == ENTER) {
            password[i] = '\0';
            break;
        } else if (ch == BCKSPC) {
            if (i > 0) {
                i--;
                printf("\b \b");
            }
        } else if (i < 49) {
            password[i++] = ch;
            printf("*");
        }
    }
    printf("\n");
}

static void signUp() {
    FILE *fp;
    User newUser;
    char confirmPassword[50];

    printf("\n--- Sign Up ---\n");

    takeInput("Enter your username: ", newUser.username);
    takePassword("Enter your password: ", newUser.password);
    takePassword("Confirm your password: ", confirmPassword);

    if (strcmp(newUser.password, confirmPassword) != 0) {
        printf("\nPasswords do not match. Registration failed.\n");
        return;
    }

    fp = fopen(DATA_FILE, "ab+");
    if (fp == NULL) {
        printf("\nError: Could not open file %s\n", DATA_FILE);
        return;
    }

    fwrite(&newUser, sizeof(User), 1, fp);
    fclose(fp);

    printf("\nUser registration successful! Your username is '%s'.\n", newUser.username);
}

static int login() {
    FILE *fp;
    User existingUser;
    char username[50], password[50];
    int userFound = 0;

    printf("\n--- Login ---\n");

    takeInput("Enter your username: ", username);
    takePassword("Enter your password: ", password);

    fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        printf("\nError: Could not open file %s. No users registered yet.\n", DATA_FILE);
        return 0;
    }

    while (fread(&existingUser, sizeof(User), 1, fp)) {
        if (strcmp(existingUser.username, username) == 0) {
            userFound = 1;
            if (strcmp(existingUser.password, password) == 0) {
                fclose(fp);
                printf("\nLogin successful! Welcome, %s.\n", existingUser.username);
                strcpy(GLOBAL_USERNAME, existingUser.username);
                return 1;
            } else {
                printf("\nIncorrect password.\n");
                fclose(fp);
                return 0;
            }
        }
    }

    if (!userFound) {
        printf("\nUsername not found.\n");
    }

    fclose(fp);
    return 0;
}

static char* auth() {
    int choice;

    while (1) {
        printf("\n--- Authentication System ---\n");
        printf("1. Sign Up\n");
        printf("2. Login\n");
        printf("3. Exit\n");
        printf("Your choice: ");

        if (scanf("%d", &choice) != 1) {
            while (getchar() != '\n');
            printf("\nInvalid input. Please enter a number.\n");
            continue;
        }
        getchar();

        switch (choice) {
            case 1:
                signUp();
                break;
            case 2:
                if (login()) {
                    return GLOBAL_USERNAME;
                }
                break;
            case 3:
                printf("\nExiting... Goodbye!\n");
                exit(0);
            default:
                printf("\nInvalid choice. Try again.\n");
        }
    }
    return "null";
}
