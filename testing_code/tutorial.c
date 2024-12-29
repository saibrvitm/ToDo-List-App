#include <gtk-3.0/gtk/gtk.h>

static void buttonPress(GtkApplication *app, gpointer user_data)
{
    g_print("Hello World!\n");
}

// Gtk is signal based - so we need to connect the app to an activate signal
// static void activate(GtkApplication *app, gpointer user_data)
// {
//     GtkWidget *window;
//     GtkWidget *button;
//     GtkWidget *label;
//     GtkWidget *grid;

//     // Gtk is OOP - so create a window object reference
//     window = gtk_application_window_new(app);
//     gtk_window_set_title(GTK_WINDOW(window), "To-Do List Application");
//     gtk_window_set_default_size(GTK_WINDOW(window), 1240, 660);

//     // Add a <div> to the window
//     grid = gtk_grid_new();
//     gtk_container_add(GTK_CONTAINER(window), grid);

//     // Add a button to the div
//     button = gtk_button_new_with_label("Print Me!");
//     g_signal_connect(button, "clicked", G_CALLBACK(buttonPress), NULL);
//     gtk_grid_attach(GTK_GRID(grid), button, 0, 1, 1, 1);

//     button = gtk_button_new_with_label("Print Me Too!");
//     g_signal_connect(button, "clicked", G_CALLBACK(buttonPress), NULL);
//     gtk_grid_attach(GTK_GRID(grid), button, 1, 1, 1, 1);

//     // Add a label to the div
//     label = gtk_label_new("This is a Label");
//     gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 2, 1);

//     // Update the window
//     gtk_widget_show_all(window);
// }

int main(int argc, char **argv)
{
    GtkBuilder *builder;
    GObject *window;
    GObject *button;
    GError *error = NULL;

    gtk_init(&argc, &argv);

    /* Construct a GtkBuilder instance and load our UI description */
    builder = gtk_builder_new();
    if (gtk_builder_add_from_file(GTK_BUILDER(builder), "builder.xml", &error) == 0)
    {
        g_printerr("Error loading file: %s\n", error->message);
        g_clear_error(&error);
        return 1;
    }

    /* Connect signal handlers to the constructed widgets. */
    window = gtk_builder_get_object(builder, "window");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    button = gtk_builder_get_object(builder, "button1");
    g_signal_connect(button, "clicked", G_CALLBACK(buttonPress), NULL);

    button = gtk_builder_get_object(builder, "button2");
    g_signal_connect(button, "clicked", G_CALLBACK(buttonPress), NULL);

    button = gtk_builder_get_object(builder, "quit");
    g_signal_connect(button, "clicked", G_CALLBACK(gtk_main_quit), NULL);

    gtk_main();

    return 0;
}