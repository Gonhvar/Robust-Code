#include "InterfaceGraphique.hpp"
#include "Rasberry.hpp"
#include "ControlMoteur.hpp"
#include "modele.hpp"


#include <gtk/gtk.h>
#include <thread>

// // Callback function for the button
// void on_button_clicked(GtkWidget *widget, gpointer data)
// {
//     g_print("Button clicked!\n");
// }



int main(int argc, char *argv[])
{


    InterfaceGraphique interfaceGraphique(argc,argv);
    ControlMoteur controlMoteur;
    Rasberry rasberry;




    printf("hey!\n");


    interfaceGraphique.waitEnd();
    /controlMoteur.waitEnd();
    rasberry.waitEnd();








    // // Initialize GTK
    // gtk_init(&argc, &argv);

    // // Create the main window
    // GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    // gtk_window_set_title(GTK_WINDOW(window), "GTK Window");
    // gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
    // g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // // Create a button
    // GtkWidget *button = gtk_button_new_with_label("Click Me");
    // g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);
    // gtk_container_add(GTK_CONTAINER(window), button);

    // // Show all the widgets
    // gtk_widget_show_all(window);

    // // Start the GTK main loop
    // gtk_main();

    return 0;
}
