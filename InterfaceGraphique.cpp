#include "InterfaceGraphique.hpp"

InterfaceGraphique::InterfaceGraphique(int argc, char *argv[]) {
    etat = POSITION;
    gtk_init(&argc, &argv);
    initWindow();

    run();
}


void InterfaceGraphique::initWindow() {
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Projet Robust");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
}


void InterfaceGraphique::run() {
    // Show all the widgets
    gtk_widget_show_all(window);

    // Start the GTK main loop
    gtk_main();
}