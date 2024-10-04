#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

GtkWidget *result_window;
GtkTextBuffer *text_buffer;

void show_results(const char *results) {
    gtk_text_buffer_set_text(text_buffer, results, -1);
    gtk_widget_show_all(result_window);
}

void execute_command(const char *command) {
    char buffer[128];
    char result[1024]=""; // Buffer to store command results
    FILE *fp;

    // Execute the command and read the output
    fp=popen(command, "r");
    if (fp == NULL) {
        show_results("Failed to run command.\n");
        return;
    }

    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        strcat(result, buffer); // Append command output
    }

    pclose(fp);
    show_results(result); // Show the command output in the result window
}

void ping(const char *target) {
    char command[256];
    snprintf(command, sizeof(command), "ping -c 4 %s", target); // For Linux/Unix-based systems
    execute_command(command);
}

void ping_of_death(const char *target) {
    char command[256];
    snprintf(command, sizeof(command), "ping -f -c 1000 %s", target); // -f is flood ping, use cautiously
    execute_command(command);
}

void netstat_info() {
    execute_command("netstat");
}

void on_ping_button_clicked(GtkWidget *widget, gpointer data) {
    const gchar *target=gtk_entry_get_text(GTK_ENTRY(data));
    ping(target);
}

void on_ping_of_death_button_clicked(GtkWidget *widget, gpointer data) {
    const gchar *target=gtk_entry_get_text(GTK_ENTRY(data));
    ping_of_death(target);
}

void on_netstat_button_clicked(GtkWidget *widget) {
    netstat_info();
}

void on_about_button_clicked(GtkWidget *widget) {
    GtkWidget *about_dialog;
    about_dialog=gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_INFO,
                                           GTK_BUTTONS_OK, "Network Ping Utility\nAuthor: Jay Mee @ J~Net 2024\nThis utility can ping a target and perform a flood ping.");
    gtk_dialog_run(GTK_DIALOG(about_dialog));
    gtk_widget_destroy(about_dialog);
}

void create_result_window() {
    result_window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(result_window), "Command Results");
    gtk_window_set_default_size(GTK_WINDOW(result_window), 400, 300);
    
    GtkWidget *text_view=gtk_text_view_new();
    text_buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    gtk_text_view_set_editable(GTK_TEXT_VIEW(text_view), FALSE);
    
    gtk_container_add(GTK_CONTAINER(result_window), text_view);
    g_signal_connect(result_window, "destroy", G_CALLBACK(gtk_widget_hide), NULL);
}

void create_main_window() {
    GtkWidget *window, *vbox, *entry, *ping_button, *ping_of_death_button, *netstat_button, *about_button;

    window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Network Ping Utility");
    gtk_window_set_default_size(GTK_WINDOW(window), 300, 200);
    
    vbox=gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    entry=gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(vbox), entry, FALSE, FALSE, 0);

    ping_button=gtk_button_new_with_label("Ping");
    g_signal_connect(ping_button, "clicked", G_CALLBACK(on_ping_button_clicked), entry);
    gtk_box_pack_start(GTK_BOX(vbox), ping_button, FALSE, FALSE, 0);

    ping_of_death_button=gtk_button_new_with_label("Ping of Death");
    g_signal_connect(ping_of_death_button, "clicked", G_CALLBACK(on_ping_of_death_button_clicked), entry);
    gtk_box_pack_start(GTK_BOX(vbox), ping_of_death_button, FALSE, FALSE, 0);

    netstat_button=gtk_button_new_with_label("Show Netstat");
    g_signal_connect(netstat_button, "clicked", G_CALLBACK(on_netstat_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), netstat_button, FALSE, FALSE, 0);

    about_button=gtk_button_new_with_label("About");
    g_signal_connect(about_button, "clicked", G_CALLBACK(on_about_button_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(vbox), about_button, FALSE, FALSE, 0);

    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(window);
}

void show_usage() {
    printf("Usage: ping-gui [options]\n");
    printf("Options:\n");
    printf("  -h    Show this help message\n");
    printf("  No options: Launch GUI\n");
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    if (argc > 1) {
        if (strcmp(argv[1], "-h") == 0) {
            show_usage();
            return 0;
        } else {
            printf("Invalid option. Use -h for help.\n");
            return 1;
        }
    }

    create_main_window();
    create_result_window();
    gtk_main();

    return 0;
}

