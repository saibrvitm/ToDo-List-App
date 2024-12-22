#include <gtk/gtk.h>

typedef struct {
    GtkWidget *window;
    GtkWidget *list;
    GtkWidget *entry;
    GtkWidget *completed_label;
    GtkWidget *total_label;
    GList *tasks;  // List of tasks as strings, for simplicity
} TodoApp;

typedef struct {
    gchar *task;
    gboolean completed;
} Task;

void update_stats(TodoApp *app) {
    guint total = g_list_length(app->tasks);
    guint completed = 0;

    for (GList *l = app->tasks; l != NULL; l = l->next) {
        Task *task = (Task *)l->data;
        if (task->completed) {
            completed++;
        }
    }

    gchar *stats = g_strdup_printf("Tasks: %u | Completed: %u", total, completed);
    gtk_label_set_text(GTK_LABEL(app->total_label), stats);
    g_free(stats);
}

void toggle_task_status(GtkWidget *button, TodoApp *app) {
    // Find the row corresponding to the clicked button
    GtkWidget *row = gtk_widget_get_parent(button);  // Get the parent row of the button
    GtkWidget *box = gtk_bin_get_child(GTK_BIN(row));  // Get the box containing the label and button

    // Get the label from the box (the label is the first child of the box)
    GList *box_children = gtk_container_get_children(GTK_CONTAINER(box));  // Get the children of the box
    if (box_children != NULL) {
        GtkWidget *label = GTK_WIDGET(box_children->data);  // The label is the first child in the box

        // Get the text from the label
        const gchar *task_text = gtk_label_get_text(GTK_LABEL(label));  

        // Iterate through tasks and find the one that matches the label text
        for (GList *l = app->tasks; l != NULL; l = l->next) {
            Task *task = (Task *)l->data;
            if (g_strcmp0(task->task, task_text) == 0) {
                task->completed = !task->completed;  // Toggle the completed status

                // Update the button label and the label text based on the task's completed status
                if (task->completed) {
                    gtk_button_set_label(GTK_BUTTON(button), "Uncomplete");
                    gtk_label_set_markup(GTK_LABEL(label), g_strdup_printf("<s>%s</s>", task->task));  // Strikethrough completed task
                } else {
                    gtk_button_set_label(GTK_BUTTON(button), "Complete");
                    gtk_label_set_text(GTK_LABEL(label), task->task);  // Remove strikethrough for uncompleted task
                }
                break;
            }
        }
        // Free the GList of box children after we're done with it
        g_list_free(box_children);
    }

    // Update the task statistics after toggling
    update_stats(app);
}

void add_task(GtkWidget *widget, TodoApp *app) {
    const gchar *task_text = gtk_entry_get_text(GTK_ENTRY(app->entry));
    if (strlen(task_text) > 0) {
        Task *new_task = g_new(Task, 1);
        new_task->task = g_strdup(task_text);
        new_task->completed = FALSE;

        app->tasks = g_list_append(app->tasks, new_task);

        // Create row with task label and button
        GtkWidget *row = gtk_list_box_row_new();
        GtkWidget *label = gtk_label_new(task_text);
        GtkWidget *button = gtk_button_new_with_label("Complete");
        
        // Connect button to toggle completion status
        g_signal_connect(button, "clicked", G_CALLBACK(toggle_task_status), app);

        GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
        gtk_box_pack_start(GTK_BOX(box), label, TRUE, TRUE, 0);
        gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);
        gtk_container_add(GTK_CONTAINER(row), box);
        
        gtk_list_box_insert(GTK_LIST_BOX(app->list), row, -1);

	gtk_widget_show_all(row);

        gtk_entry_set_text(GTK_ENTRY(app->entry), "");
        update_stats(app);
    }
}

void remove_task(GtkWidget *widget, TodoApp *app) {
    // Logic to remove selected task goes here
    update_stats(app);
}

void activate(GtkApplication *app, gpointer user_data) {
    TodoApp *todo_app = g_new(TodoApp, 1);
    todo_app->tasks = NULL;  // Initialize task list

    // Create the main window
    todo_app->window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(todo_app->window), "GTK Todo List");
    gtk_window_set_default_size(GTK_WINDOW(todo_app->window), 600, 300);  // Landscape orientation

    // Create a main container (box) with vertical orientation
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(todo_app->window), main_box);

    // Create a top section for stats
    GtkWidget *top_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(main_box), top_box, FALSE, FALSE, 0);
    
    todo_app->total_label = gtk_label_new("Tasks: 0 | Completed: 0");
    todo_app->completed_label = gtk_label_new("0 completed");
    gtk_box_pack_start(GTK_BOX(top_box), todo_app->total_label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(top_box), todo_app->completed_label, FALSE, FALSE, 0);

    // Create entry for new tasks
    todo_app->entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(main_box), todo_app->entry, FALSE, FALSE, 0);

    // Create a list to display tasks
    todo_app->list = gtk_list_box_new();
    gtk_box_pack_start(GTK_BOX(main_box), todo_app->list, TRUE, TRUE, 0);

    // Create buttons for task management
    GtkWidget *bottom_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    gtk_box_pack_start(GTK_BOX(main_box), bottom_box, FALSE, FALSE, 0);

    GtkWidget *add_button = gtk_button_new_with_label("Add Task");
    g_signal_connect(add_button, "clicked", G_CALLBACK(add_task), todo_app);
    gtk_box_pack_start(GTK_BOX(bottom_box), add_button, TRUE, TRUE, 0);

    GtkWidget *remove_button = gtk_button_new_with_label("Remove Task");
    g_signal_connect(remove_button, "clicked", G_CALLBACK(remove_task), todo_app);
    gtk_box_pack_start(GTK_BOX(bottom_box), remove_button, TRUE, TRUE, 0);

    gtk_widget_show_all(todo_app->window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
