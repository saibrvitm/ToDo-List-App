#include <gtk-3.0/gtk/gtk.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include "user_auth.c"

#define MAX_TASKS 100

typedef struct {
    char task_name[256];
    char task_description[512];
    bool completed;
} Task;

Task tasks[MAX_TASKS];
int task_count = 0;

static void update_clock(GtkLabel *clock_label);
static void populate_task_list(GtkListBox *list_box, GtkLabel *task_count_label);
static void add_task(const char *task_name, const char *task_description, GtkListBox *task_list, GtkLabel *task_count_label);
static void on_add_task_button_clicked(GtkButton *button, gpointer user_data);
static void on_edit_task_button_clicked(GtkButton *button, gpointer user_data);
static void on_delete_task_button_clicked(GtkButton *button, gpointer user_data);
static void on_complete_task_button_clicked(GtkButton *button, gpointer user_data);

int main(int argc, char *argv[])
{
    auth();
    printf("Auth successful\n");

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
    g_timeout_add_seconds(1, (GSourceFunc)update_clock, clock_label); // Update every second

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

    /** SHOW WINDOW AND RUN APP */
    gtk_widget_show_all(window);
    gtk_window_present(GTK_WINDOW(window));
    gtk_main();

    return 0;
}

static void update_clock(GtkLabel *clock_label)
{
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_str[12]; // HH:MM:SS AM/PM format

    strftime(time_str, sizeof(time_str), "%H:%M:%S %p", tm_info);
    gtk_label_set_text(clock_label, time_str);
}

static void update_task_count_label(GtkLabel *task_count_label)
{
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
        sprintf(task_str, "[%c]  %s   -  %s", tasks[i].completed ? 'X' : ' ', tasks[i].task_name, tasks[i].task_description);

        GtkWidget *name_label = gtk_label_new(task_str);

        gtk_box_pack_start(GTK_BOX(task_box), name_label, FALSE, FALSE, 0);

        GtkListBoxRow *row = gtk_list_box_row_new();
        gtk_container_add(GTK_CONTAINER(row), task_box);
        gtk_list_box_insert(list_box, row, -1);
    }

    gtk_widget_show_all(GTK_WIDGET(list_box));
    update_task_count_label(task_count_label);
}

static void add_task(const char *task_name, const char *task_description, GtkListBox *task_list, GtkLabel *task_count_label)
{
    if (task_count >= MAX_TASKS) {
        g_print("Task list is full!\n");
        return;
    }

    // Add task to the array
    strncpy(tasks[task_count].task_name, task_name, sizeof(tasks[task_count].task_name) - 1);
    strncpy(tasks[task_count].task_description, task_description, sizeof(tasks[task_count].task_description) - 1);
    task_count++;

    GtkWidget *task_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

    char task_str[1024];
    sprintf(task_str, "[%c]  %s   -  %s", tasks[task_count - 1].completed ? 'X' : ' ', tasks[task_count - 1].task_name, tasks[task_count - 1].task_description);

    GtkWidget *name_label = gtk_label_new(task_str);

    gtk_box_pack_start(GTK_BOX(task_box), name_label, FALSE, FALSE, 0);

    GtkListBoxRow *row = gtk_list_box_row_new();

    gtk_container_add(GTK_CONTAINER(row), task_box);
    gtk_list_box_insert(task_list, row, -1);

    gtk_widget_show_all(GTK_WIDGET(task_list));
    update_task_count_label(task_count_label);
}

static void on_add_task_button_clicked(GtkButton *button, gpointer user_data)
{
    g_print("Add Task Button Clicked\n");
    GtkEntry *task_name_entry = GTK_ENTRY(((gpointer*)user_data)[0]);
    GtkEntry *task_description_entry = GTK_ENTRY(((gpointer*)user_data)[1]);
    GtkListBox *task_list = GTK_LIST_BOX(((gpointer*)user_data)[2]);
    GtkLabel *task_count_label = GTK_LABEL(((gpointer*)user_data)[3]);

    const char *task_name = gtk_entry_get_text(task_name_entry);
    const char *task_description = gtk_entry_get_text(task_description_entry);

    add_task(task_name, task_description, task_list, task_count_label);

    gtk_entry_set_text(task_name_entry, "");
    gtk_entry_set_text(task_description_entry, "");
}

static void on_edit_task_button_clicked(GtkButton *button, gpointer user_data)
{
    g_print("Edit Task Button Clicked\n");

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

    if (g_strcmp0(task_name, "") == 0 || g_strcmp0(task_description, "") == 0) {
        g_print("Task name or description cannot be empty.\n");
        return;
    }

    GtkWidget *updated_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

    char task_str[1024];
    sprintf(task_str, "[%c]  %s   -  %s", tasks[gtk_list_box_row_get_index(selected_row)].completed ? 'X' : ' ', task_name, task_description);

    GtkWidget *updated_name_label = gtk_label_new(task_str);

    gtk_box_pack_start(GTK_BOX(updated_box), updated_name_label, FALSE, FALSE, 0);

    GtkWidget *old_child = gtk_bin_get_child(GTK_BIN(selected_row));
    gtk_container_remove(GTK_CONTAINER(selected_row), old_child);
    gtk_container_add(GTK_CONTAINER(selected_row), updated_box);

    gtk_widget_show_all(GTK_WIDGET(selected_row));

    gtk_entry_set_text(task_name_entry, "");
    gtk_entry_set_text(task_description_entry, "");
    update_task_count_label(task_count_label);
}

static void on_delete_task_button_clicked(GtkButton *button, gpointer user_data)
{
    g_print("Delete Task Button Clicked\n");
    
    GtkListBox *task_list = GTK_LIST_BOX(((gpointer*)user_data)[2]);
    GtkLabel *task_count_label = GTK_LABEL(((gpointer*)user_data)[3]);
    GtkListBoxRow *selected_row = gtk_list_box_get_selected_row(task_list);
    if (selected_row == NULL) {
        g_print("No task selected!\n");
        return;
    }

    int index = gtk_list_box_row_get_index(selected_row);

    // Shift tasks in the array
    for (int i = index; i < task_count - 1; i++) {
        tasks[i] = tasks[i + 1];
    }
    task_count--;

    gtk_widget_destroy(GTK_WIDGET(selected_row));

    populate_task_list(task_list, task_count_label);
}

static void on_complete_task_button_clicked(GtkButton *button, gpointer user_data)
{
    g_print("Complete Task Button Clicked\n");
    
    GtkListBox *task_list = GTK_LIST_BOX(((gpointer*)user_data)[2]);
    GtkLabel *task_count_label = GTK_LABEL(((gpointer*)user_data)[3]);
    GtkListBoxRow *selected_row = gtk_list_box_get_selected_row(task_list);
    if (selected_row == NULL) {
        g_print("No task selected!\n");
        return;
    }

    int index = gtk_list_box_row_get_index(selected_row);

    tasks[index].completed = !tasks[index].completed;

    gtk_widget_destroy(GTK_WIDGET(selected_row));

    populate_task_list(task_list, task_count_label);
}

