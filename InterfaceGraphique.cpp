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


void InterfaceGraphique::initWigets() {
    notebook = GTK_NOTEBOOK(gtk_notebook_new());



    // Ajouter le GtkNotebook à la fenêtre
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(notebook));


    // Créer les widgets pour chaque page
    pagePosition = GTK_LABEL(gtk_label_new("Position"));
    pageHaptic = GTK_LABEL(gtk_label_new("Haptic"));

    // Ajouter les widgets comme contenu des pages du GtkNotebook
    gtk_notebook_append_page(notebook, GTK_WIDGET(pagePosition),NULL);
    gtk_notebook_append_page(notebook, GTK_WIDGET(pageHaptic),NULL);
    


    

    gtk_widget_show_all(window);
    // gtk_widget_show(window);

    

}

void InterfaceGraphique::initWindow() {

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Projet Robust");
    gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    initWigets();


}


void InterfaceGraphique::waitEnd() {
    gtkThread->join();
}




InterfaceGraphique::~InterfaceGraphique() {
    
    //gtk_main_quit(); -> genere une erreur bizarrement
}


