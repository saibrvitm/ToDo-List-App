#include <gtk-3.0/gtk/gtk.h>
#include "task_manager.h"

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