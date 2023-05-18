#include "InterfaceGraphique.hpp"
#include <thread>


int InterfaceGraphique::nombreInstance =0 ;

InterfaceGraphique::InterfaceGraphique() {
    if (nombreInstance>0) {
        throw std::runtime_error("InterfaceGraphique::InterfaceGraphique() -> impossible d'instancier plus d'une fois une InterfaceGraphique\n");
    }
    nombreInstance++;
    etat = POSITION;

    //gtk_init(&argc, &argv);
    gtk_init(nullptr, nullptr);

    initWindow();


    gtkThread = new std::thread(&InterfaceGraphique::runGtkMain,this);

}

void InterfaceGraphique::runGtkMain() {
    gtk_main();
}

void InterfaceGraphique::initWindow() {
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Projet Robust");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_widget_show(window);
}


void InterfaceGraphique::waitEnd() {
    gtkThread->join();
}




InterfaceGraphique::~InterfaceGraphique() {
    
    //gtk_main_quit(); -> genere une erreur bizarrement
}


