#include <glib.h>
#include <stdio.h>

// This function will be called every second
gboolean timeout_callback(gpointer data) {
    static int count = 0;
    printf("Tick %d: %s\n", ++count, (char*)data);

    if (count >= 5) {
        // Stop the main loop after 5 ticks
        GMainLoop *loop = (GMainLoop*)data;
        g_main_loop_quit(loop);
        return FALSE; // stop the timeout
    }

    return TRUE; // continue calling the timeout
}

int main() {
    // Create a new main loop
    GMainLoop *loop = g_main_loop_new(NULL, FALSE);

    // Add a timeout function that runs every 1000ms (1 second)
    g_timeout_add_seconds(1, timeout_callback, loop);

    printf("Starting main loop...\n");

    // Run the main loop
    g_main_loop_run(loop);

    printf("Main loop exited!\n");

    // Free the loop
    g_main_loop_unref(loop);

    return 0;
}