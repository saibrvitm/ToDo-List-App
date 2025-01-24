#include <gtk-3.0/gtk/gtk.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "user_auth.c"

#define MAX_TASKS 100
#define TASK_FILE "data/tasks.txt"

typedef struct {
    char task_name[256];
    char task_description[512];
    bool completed;
} Task;

Task tasks[MAX_TASKS];
int task_count = 0;

static void update_clock(GtkLabel *clock_label);
static void populate_task_list(GtkListBox *list_box, GtkLabel *task_count_label);
static void save_tasks_to_file();
static void load_tasks_from_file();
static void add_task_to_file(Task task);
static void delete_task_from_file(int index);
static void update_task_in_file(int index, Task updated_task);
static void on_add_task_button_clicked(GtkButton *button, gpointer user_data);
static void on_edit_task_button_clicked(GtkButton *button, gpointer user_data);
static void on_delete_task_button_clicked(GtkButton *button, gpointer user_data);
static void on_complete_task_button_clicked(GtkButton *button, gpointer user_data);

int main(int argc, char *argv[]) {
    auth();
    printf("Authentication successful!\n");

    GtkBuilder *builder;
    GtkWidget *window;
    GtkWidget *add_task_button, *edit_task_button, *delete_task_button, *complete_task_button;
    GtkLabel *clock_label, *task_count_label;
    GtkListBox *task_list;
    GtkEntry *task_name_entry, *task_description_entry;

    gtk_init(&argc, &argv);

    /** LOAD UI FROM FILE */
    builder = gtk_builder_new_from_file("builder.xml");

    /** MAIN WINDOW FROM BUILDER */
    window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));
    g_signal_connect(GTK_WINDOW(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_window_set_title(GTK_WINDOW(window), "To-Do App");

    /** TASK NAME AND DESCRIPTION ENTRIES */
    task_name_entry = GTK_ENTRY(gtk_builder_get_object(builder, "task_name_entry"));
    task_description_entry = GTK_ENTRY(gtk_builder_get_object(builder, "task_description_entry"));
    task_list = GTK_LIST_BOX(gtk_builder_get_object(builder, "task_list"));

    /** CLOCK */
    clock_label = GTK_LABEL(gtk_builder_get_object(builder, "label_hero_b"));
    g_timeout_add_seconds(1, (GSourceFunc)update_clock, clock_label);

    /** TASK COUNT */
    task_count_label = GTK_LABEL(gtk_builder_get_object(builder, "label_hero_a"));

    gpointer user_data[] = {task_name_entry, task_description_entry, task_list, task_count_label};

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
    populate_task_list(task_list, task_count_label);
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
    for (int i = 0; i < task_count; i++) {
        if (tasks[i].completed) {
            completed_tasks++;
        }
    }
    char label_text[50];
    sprintf(label_text, "%d / %d Tasks Completed", completed_tasks, task_count);
    gtk_label_set_text(task_count_label, label_text);
}

static void save_tasks_to_file() {
    FILE *file = fopen(TASK_FILE, "w");
    if (!file) {
        perror("Failed to open file for writing");
        return;
    }

    for (int i = 0; i < task_count; i++) {
        fprintf(file, "%d|%s|%s\n", tasks[i].completed, tasks[i].task_name, tasks[i].task_description);
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
    while (fscanf(file, "%d|%255[^|]|%511[^\n]\n",
                  &tasks[task_count].completed,
                  tasks[task_count].task_name,
                  tasks[task_count].task_description) == 3) {
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

    fprintf(file, "%d|%s|%s\n", task.completed, task.task_name, task.task_description);
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
            fprintf(file, "%d|%s|%s\n", tasks[i].completed, tasks[i].task_name, tasks[i].task_description);
        }
    }

    fclose(file);
}

static void update_task_in_file(int index, Task updated_task) {
    tasks[index] = updated_task;
    save_tasks_to_file();
}

static void populate_task_list(GtkListBox *list_box, GtkLabel *task_count_label) {
    GList *children, *iter;
    children = gtk_container_get_children(GTK_CONTAINER(list_box));
    for (iter = children; iter != NULL; iter = g_list_next(iter)) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }
    g_list_free(children);

    for (int i = 0; i < task_count; i++) {
        GtkWidget *task_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

        char task_str[501];
        sprintf(task_str, "[%s]  %s   -  %s", tasks[i].completed ? "\xE2\x9C\x85" : "\xE2\x9D\x8C", tasks[i].task_name, tasks[i].task_description);

        GtkWidget *name_label = gtk_label_new(task_str);

        gtk_box_pack_start(GTK_BOX(task_box), name_label, FALSE, FALSE, 0);

        GtkListBoxRow *row = gtk_list_box_row_new();
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

    const char *task_name = gtk_entry_get_text(task_name_entry);
    const char *task_description = gtk_entry_get_text(task_description_entry);

    if (task_count >= MAX_TASKS) {
        g_print("Task list is full!\n");
        return;
    }

    Task new_task;
    strncpy(new_task.task_name, task_name, sizeof(new_task.task_name) - 1);
    strncpy(new_task.task_description, task_description, sizeof(new_task.task_description) - 1);
    new_task.completed = false;

    tasks[task_count] = new_task;
    task_count++;

    add_task_to_file(new_task);
    populate_task_list(task_list, task_count_label);

    gtk_entry_set_text(task_name_entry, "");
    gtk_entry_set_text(task_description_entry, "");
}

static void on_edit_task_button_clicked(GtkButton *button, gpointer user_data) {
    GtkEntry *task_name_entry = GTK_ENTRY(((gpointer*)user_data)[0]);
    GtkEntry *task_description_entry = GTK_ENTRY(((gpointer*)user_data)[1]);
    GtkListBox *task_list = GTK_LIST_BOX(((gpointer*)user_data)[2]);
    GtkLabel *task_count_label = GTK_LABEL(((gpointer*)user_data)[3]);

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
    populate_task_list(task_list, task_count_label);

    gtk_entry_set_text(task_name_entry, "");
    gtk_entry_set_text(task_description_entry, "");
}

static void on_delete_task_button_clicked(GtkButton *button, gpointer user_data) {
    GtkListBox *task_list = GTK_LIST_BOX(((gpointer*)user_data)[2]);
    GtkLabel *task_count_label = GTK_LABEL(((gpointer*)user_data)[3]);
    GtkListBoxRow *selected_row = gtk_list_box_get_selected_row(task_list);
    if (!selected_row) {
        g_print("No task selected!\n");
        return;
    }

    int index = gtk_list_box_row_get_index(selected_row);

    delete_task_from_file(index);

    for (int i = index; i < task_count - 1; i++) {
        tasks[i] = tasks[i + 1];
    }
    task_count--;

    populate_task_list(task_list, task_count_label);
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
    populate_task_list(task_list, task_count_label);
}
