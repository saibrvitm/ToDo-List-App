#include <gtk-3.0/gtk/gtk.h>
#include "task_manager.h"

void update_clock(GtkLabel *clock_label) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_str[12];

    strftime(time_str, sizeof(time_str), "%H:%M:%S %p", tm_info);
    gtk_label_set_text(clock_label, time_str);
}

void update_task_count_label(GtkLabel *task_count_label) {
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

void populate_task_list(GtkListBox *list_box, GtkLabel *task_count_label, const char *filter_day) {
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