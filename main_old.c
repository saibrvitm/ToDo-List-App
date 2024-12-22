#include <gtk/gtk.h>

static void on_activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window;
    GtkWidget *label;

    // Create a new window with a title#include <stdio.h>
#include <gtk/gtk.h>


    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Hello, GTK!");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 200);

    // Create a label and add it to the window
    label = gtk_label_new("Hello, GTK World!");
    gtk_container_add(GTK_CONTAINER(window), label);

    // Show all widgets in the window
    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app;
    int status;

    // Create a new GTK application
    app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);

    // Connect the activate signal to the on_activate function
    g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

    // Run the application
    status = g_application_run(G_APPLICATION(app), argc, argv);

    // Clean up and exit
    g_object_unref(app);
    return status;
}
