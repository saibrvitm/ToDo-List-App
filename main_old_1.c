#include <gtk/gtk.h>

/*
sudo apt-get install libgtk-3-dev
gcc -o gtk_todo_list gtk_todo_list.c `pkg-config --cflags --libs gtk+-3.0`
./gtk_todo_list
*/

typedef struct {
    GtkWidget *window;
    GtkWidget *list;
    GtkWidget *entry;
} TodoApp;

void add_task(GtkWidget *widget, TodoApp *app) {
    const gchar *task = gtk_entry_get_text(GTK_ENTRY(app->entry));
    if (strlen(task) > 0) {
        GtkWidget *row = gtk_list_box_row_new();
        GtkWidget *label = gtk_label_new(task);
        gtk_container_add(GTK_CONTAINER(row), label);
        gtk_list_box_insert(GTK_LIST_BOX(app->list), row, -1);
        gtk_entry_set_text(GTK_ENTRY(app->entry), "");
    }
}

void remove_task(GtkWidget *widget, TodoApp *app) {
    GtkListBoxRow *row = gtk_list_box_get_selected_row(GTK_LIST_BOX(app->list));
    if (row != NULL) {
        gtk_container_remove(GTK_CONTAINER(app->list), GTK_WIDGET(row));
    }
}

void activate(GtkApplication *app, gpointer user_data) {
    TodoApp *todo_app = g_new(TodoApp, 1);

    // Create the main window
    todo_app->window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(todo_app->window), "GTK Todo List");
    gtk_window_set_default_size(GTK_WINDOW(todo_app->window), 300, 400);

    // Create a vertical box to hold our widgets
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(todo_app->window), box);

    // Create an entry for new tasks
    todo_app->entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box), todo_app->entry, FALSE, FALSE, 0);

    // Create buttons
    GtkWidget *add_button = gtk_button_new_with_label("Add Task");
    g_signal_connect(add_button, "clicked", G_CALLBACK(add_task), todo_app);
    gtk_box_pack_start(GTK_BOX(box), add_button, FALSE, FALSE, 0);

    GtkWidget *remove_button = gtk_button_new_with_label("Remove Task");
    g_signal_connect(remove_button, "clicked", G_CALLBACK(remove_task), todo_app);
    gtk_box_pack_start(GTK_BOX(box), remove_button, FALSE, FALSE, 0);

    // Create a scrolled window for the list
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
                                   GTK_POLICY_AUTOMATIC,
                                   GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(box), scrolled_window, TRUE, TRUE, 0);

    // Create the list
    todo_app->list = gtk_list_box_new();
    gtk_container_add(GTK_CONTAINER(scrolled_window), todo_app->list);

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
