#include <gtk-3.0/gtk/gtk.h>
#include "task_manager.h"

void on_add_task_button_clicked(GtkButton *button, gpointer user_data) {
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

void on_edit_task_button_clicked(GtkButton *button, gpointer user_data) {
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

void on_delete_task_button_clicked(GtkButton *button, gpointer user_data) {
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

void on_complete_task_button_clicked(GtkButton *button, gpointer user_data) {
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

void on_combobox_button_clicked(GtkComboBoxText *combo_box, gpointer user_data) {
    const char *selected_option = gtk_combo_box_text_get_active_text(combo_box);
    if (selected_option) {
        populate_task_list(GTK_LIST_BOX(((gpointer*)user_data)[0]), GTK_LABEL(((gpointer*)user_data)[1]), selected_option);
        g_free((gchar *)selected_option); // Free the returned string
    }
}
