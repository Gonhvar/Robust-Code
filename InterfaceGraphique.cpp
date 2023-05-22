#include "InterfaceGraphique.hpp"
#include <thread>
#include "ControlMoteur.hpp"
#include "Rasberry.hpp"

#include <sstream>
#include <iomanip>


int InterfaceGraphique::nombreInstance =0 ;

InterfaceGraphique::InterfaceGraphique(Rasberry *rasberry, ControlMoteur *controlMoteur) {
    if (nombreInstance>0) {
        throw std::runtime_error("InterfaceGraphique::InterfaceGraphique() -> impossible d'instancier plus d'une fois une InterfaceGraphique\n");
    }
    nombreInstance++;
    asservissement = POSITION;

    this->rasberry = rasberry;
    this->controlMoteur = controlMoteur;

    gtk_init(nullptr, nullptr);

    initWindow();

    







    // Afficher tous les éléments de la fenêtre
    gtk_widget_show_all(window);

    // planifier le rafraichissement
    timeout_id = g_timeout_add(PERIODE_RAFRAICHISSEMENT, InterfaceGraphique::callbackWrapper_Rafraichir, this);




    gtkThread = new std::thread(&InterfaceGraphique::runGtkMain,this);

}

void InterfaceGraphique::runGtkMain() {
    gtk_main();
}

void InterfaceGraphique::initWindow() {
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Projet Robust");
    gtk_window_set_default_size(GTK_WINDOW(window), WIDTH, HEIGHT);
    g_signal_connect(GTK_WINDOW(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);






    initWigets();

    
}


void InterfaceGraphique::initWigets() {

      // Créer un conteneur GtkBox
    main = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5); // Orientation verticale, espacement de 5 pixels
    // Ajouter la GtkBox à la GtkWindow
    gtk_container_add(GTK_CONTAINER(window), main);





    // // Ajouter le GtkEntry à la GtkBox
    //   GtkWidget* entry = gtk_entry_new();
    // gtk_box_pack_start(GTK_BOX(main), entry, FALSE, FALSE, 0);



    


    head = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);
    gtk_box_pack_start(GTK_BOX(main), head, FALSE, FALSE, 0);

    body = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);
    gtk_box_pack_start(GTK_BOX(main), body, FALSE, FALSE, 0);

    bottom = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);
    gtk_box_pack_start(GTK_BOX(main), bottom, FALSE, FALSE, 0);


    // HEAD 

    // bouton change mode :
    changeModeButton = gtk_button_new_with_label("");
    gtk_box_pack_start(GTK_BOX(head), changeModeButton, FALSE, FALSE, 0);

    g_signal_connect(changeModeButton, "clicked", G_CALLBACK(callBack_ChangeModeButton), this);

    // data :
    
    recordDataBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);
    gtk_box_pack_start(GTK_BOX(head), recordDataBox, FALSE, FALSE, 0);

    // position :
    positionBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    gtk_box_pack_start(GTK_BOX(recordDataBox), positionBox, FALSE, FALSE, 0);
    positionXLabel = gtk_label_new("X");
    gtk_container_add(GTK_CONTAINER(positionBox), positionXLabel);
    positionYLabel = gtk_label_new("Y");
    gtk_container_add(GTK_CONTAINER(positionBox), positionYLabel);

    // force :
    forceBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    gtk_box_pack_start(GTK_BOX(recordDataBox), forceBox, FALSE, FALSE, 0);
    forceXLabel = gtk_label_new("Fx");
    gtk_container_add(GTK_CONTAINER(forceBox), forceXLabel);
    forceYLabel = gtk_label_new("Fy");
    gtk_container_add(GTK_CONTAINER(forceBox), forceYLabel);

    // moment :
    momentBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    gtk_box_pack_start(GTK_BOX(recordDataBox), momentBox, FALSE, FALSE, 0);
    momentXLabel = gtk_label_new("Mx");
    gtk_container_add(GTK_CONTAINER(momentBox), momentXLabel);
    momentYLabel = gtk_label_new("My");
    gtk_container_add(GTK_CONTAINER(momentBox), momentYLabel);

    // [!] rajouter la mise a jour des infos



    // BODY

    // [!] A COMPLETER

    // Ajouter le GtkDrawingArea à la GtkBox
    drawing_area = gtk_drawing_area_new();
    gtk_box_pack_start(GTK_BOX(body), drawing_area, TRUE, TRUE, 0);


    setWigetForSpecificMode();


    gtk_widget_show_all(window);


}

void InterfaceGraphique::waitEnd() {
    gtkThread->join();
}




InterfaceGraphique::~InterfaceGraphique() {
    g_source_remove(timeout_id); 
    //gtk_main_quit(); -> genere une erreur bizarrement
}


std::string InterfaceGraphique::getModeName() {
    if (asservissement == POSITION) {
        return "Position";
    }
    if (asservissement== HAPTIQUE) {
        return "Haptique";
    } 

    return "";
}

void InterfaceGraphique::setWigetForSpecificMode() {
    std::string changeModeButtonName = "Mode "+getModeName();
    gtk_button_set_label(GTK_BUTTON(changeModeButton),changeModeButtonName.c_str());



}

void InterfaceGraphique::changeMode() {
    if (asservissement==POSITION) {
        asservissement=HAPTIQUE;
        controlMoteur->setAsservissementToHAPTIC();
        printf("InterfaceGraphique::Debug : asservissement HAPTIC\n");
    } else {
        asservissement=POSITION;
        controlMoteur->setAsservissementToPOSTION();
        printf("InterfaceGraphique::Debug : asservissement POSITION\n");
    }
    setWigetForSpecificMode();
}


void InterfaceGraphique::rafraichir() {
    std::cout << "rafraichir" <<std::endl;
    double positionX=0;
    double positionY=0;

    double forceX=0;
    double forceY=0;

    double coupleX=0;
    double coupleY=0;


    controlMoteur->getPosition(positionX,positionY);
    controlMoteur->getForce(forceX,forceY);
    rasberry->getCouples(coupleX,coupleY);


    updateData(positionX,positionY,forceX,forceY,coupleX,coupleY);


}


void InterfaceGraphique::updateData(double positionX,double positionY, double forceX,double forceY, double coupleX, double coupleY) {
    int nombreDecimal = 2;

    // position :

    std::ostringstream valeur;
    valeur << std::fixed << std::setprecision(nombreDecimal) << "X : " << positionX << " mm";
    gtk_label_set_text(GTK_LABEL(positionXLabel),valeur.str().c_str());

    valeur.str(""); // vide le stream
    valeur << "Y : " << positionY << " mm";
    gtk_label_set_text(GTK_LABEL(positionYLabel),valeur.str().c_str());

    // force :
        valeur.str(""); // vide le stream
    valeur << "Fx : " << forceX << " N";
    gtk_label_set_text(GTK_LABEL(forceXLabel),valeur.str().c_str());

        valeur.str(""); // vide le stream
    valeur << "Fy : " << forceY << " N";
    gtk_label_set_text(GTK_LABEL(forceYLabel),valeur.str().c_str());


    // moment :
        valeur.str(""); // vide le stream
    valeur << "Mx : " << coupleX << " N.mm";
    gtk_label_set_text(GTK_LABEL(momentXLabel),valeur.str().c_str());

        valeur.str(""); // vide le stream
    valeur << "My : " << coupleY << " N.mm";
    gtk_label_set_text(GTK_LABEL(momentYLabel),valeur.str().c_str());



}


// --- ENSEMBLE CALLBACK --- :

gboolean InterfaceGraphique::callbackWrapper_Rafraichir(gpointer data) {
    InterfaceGraphique* interfaceGraphique = static_cast<InterfaceGraphique*>(data);
    interfaceGraphique->rafraichir();
    return G_SOURCE_CONTINUE;
}




// Fonction de callback pour le signal "clicked"
void InterfaceGraphique::callBack_ChangeModeButton(GtkWidget* button, gpointer data) {
    InterfaceGraphique* interfaceGraphique = static_cast<InterfaceGraphique*>(data);
    interfaceGraphique->changeMode();
}


