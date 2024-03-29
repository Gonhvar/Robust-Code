#include "InterfaceGraphique.hpp"
#include <thread>
#include "ControlMoteur.hpp"
#include "Rasberry.hpp"

#include <sstream>
#include <iomanip>
#include <stdlib.h>
#include <fstream>
#include <iostream>

#include "modele.hpp"


int InterfaceGraphique::nombreInstance =0 ;

InterfaceGraphique::InterfaceGraphique(Rasberry *rasberry, ControlMoteur *controlMoteur) {
    if (nombreInstance>0) {
        throw std::runtime_error("InterfaceGraphique::InterfaceGraphique() -> impossible d'instancier plus d'une fois une InterfaceGraphique\n");
    }
    nombreInstance++;
    asservissement = POSITION;
    powerOn=false;
    recordOn=false;

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

void InterfaceGraphique::addCSS(GtkWidget* window) {
    // Création d'un fournisseur de style GTK
    GtkCssProvider* provider = gtk_css_provider_new();

    // Chargement du fichier CSS contenant les styles
    gtk_css_provider_load_from_data(provider,
        " * { font-size: 15pt; }", // Définition du style de la taille de police (14 points)
        -1,
        NULL
    );

    // Application du fournisseur de style à la fenêtre
    GtkStyleContext* context = gtk_widget_get_style_context(window);
    gtk_style_context_add_provider(context,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION
    );

}

void InterfaceGraphique::setMargin(GtkWidget* widget, int margin) {
    gtk_widget_set_margin_start(widget, margin);
    gtk_widget_set_margin_end(widget, margin);
    gtk_widget_set_margin_top(widget, margin);
    gtk_widget_set_margin_bottom(widget, margin);

}

void InterfaceGraphique::initWindow() {
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Projet Robust");
    gtk_window_set_default_size(GTK_WINDOW(window), WIDTH_WINDOW, HEIGHT_WINDOW);
    g_signal_connect(GTK_WINDOW(window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    addCSS(window); // Changer la taille de police





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
    setMargin(head,20);
    gtk_box_pack_start(GTK_BOX(main), head, FALSE, FALSE, 0);


    body = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);
    setMargin(body,20);
    gtk_box_pack_start(GTK_BOX(main), body, FALSE, FALSE, 0);

    bottom = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);
    setMargin(bottom,20);
    gtk_box_pack_start(GTK_BOX(main), bottom, FALSE, FALSE, 0);


    // --- HEAD ---

    // bouton change mode :
    changeModeButton = gtk_button_new_with_label("");
    // egalement reparti dans head :
    gtk_widget_set_hexpand(changeModeButton, TRUE);
    gtk_widget_set_halign(changeModeButton, GTK_ALIGN_FILL);
    gtk_box_pack_start(GTK_BOX(head), changeModeButton, FALSE, FALSE, 0);
    g_signal_connect(changeModeButton, "clicked", G_CALLBACK(callBack_ChangeModeButton), this);

    // data :
    
    recordDataBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);
    // egalement reparti dans head :
    gtk_widget_set_hexpand(recordDataBox, TRUE);
    gtk_widget_set_halign(recordDataBox, GTK_ALIGN_FILL);
    gtk_box_pack_start(GTK_BOX(head), recordDataBox, FALSE, FALSE, 0);

    // position :
    positionBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    setMargin(positionBox,20);
    gtk_box_pack_start(GTK_BOX(recordDataBox), positionBox, FALSE, FALSE, 0);
    positionXLabel = gtk_label_new("X");
    gtk_container_add(GTK_CONTAINER(positionBox), positionXLabel);
    positionYLabel = gtk_label_new("Y");
    gtk_container_add(GTK_CONTAINER(positionBox), positionYLabel);

    // force :
    forceBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    setMargin(forceBox,20);
    gtk_box_pack_start(GTK_BOX(recordDataBox), forceBox, FALSE, FALSE, 0);
    forceXLabel = gtk_label_new("Fx");
    gtk_container_add(GTK_CONTAINER(forceBox), forceXLabel);
    forceYLabel = gtk_label_new("Fy");
    gtk_container_add(GTK_CONTAINER(forceBox), forceYLabel);

    // moment :
    momentBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    setMargin(momentBox,20);
    gtk_box_pack_start(GTK_BOX(recordDataBox), momentBox, FALSE, FALSE, 0);
    momentXLabel = gtk_label_new("Mx");
    gtk_container_add(GTK_CONTAINER(momentBox), momentXLabel);
    momentYLabel = gtk_label_new("My");
    gtk_container_add(GTK_CONTAINER(momentBox), momentYLabel);

    // couple moteur :
    coupleMoteurBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    setMargin(coupleMoteurBox,20);
    gtk_box_pack_start(GTK_BOX(recordDataBox), coupleMoteurBox, FALSE, FALSE, 0);
    coupleMoteurILabel = gtk_label_new("couple I");
    gtk_container_add(GTK_CONTAINER(coupleMoteurBox), coupleMoteurILabel);
    coupleMoteurIILabel = gtk_label_new("couple II");
    gtk_container_add(GTK_CONTAINER(coupleMoteurBox), coupleMoteurIILabel);
    coupleMoteurIIILabel = gtk_label_new("couple III");
    gtk_container_add(GTK_CONTAINER(coupleMoteurBox), coupleMoteurIIILabel);




    // bouton change Power :
    changePowerButton = gtk_button_new_with_label("Moteur hors tension");
    addRedBorder(changePowerButton);
    // egalement reparti dans head :
    gtk_widget_set_hexpand(changePowerButton, TRUE);
    gtk_widget_set_halign(changePowerButton, GTK_ALIGN_FILL);
    gtk_box_pack_start(GTK_BOX(head), changePowerButton, FALSE, FALSE, 0);
    g_signal_connect(changePowerButton, "clicked", G_CALLBACK(callBack_ChangePowerButton), this);



    setViscositeButton=nullptr;




    // --- BODY ---

    // instructionZone
    instructionZone = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    gtk_box_pack_start(GTK_BOX(body), instructionZone, FALSE, FALSE, 0);

    setupPositionWidget();   



    // drawing area
    drawing_area = gtk_drawing_area_new();
    gtk_box_pack_start(GTK_BOX(body), drawing_area, TRUE, TRUE, 0);
    gtk_widget_set_size_request(drawing_area, WIDTH_DRAWING_AREA, HEIGHT_DRAWING_AREA);



    // --- BOTTOM ---

    // resetBox

    resetBox = gtk_box_new(GTK_ORIENTATION_VERTICAL,10);
    gtk_box_pack_start(GTK_BOX(bottom), resetBox, FALSE, FALSE, 0);

    // infoResetLabel
    infoResetLabel = gtk_label_new("Attention :\nL'effecteur doit être libre\navant l'etalonnage");
    gtk_box_pack_start(GTK_BOX(resetBox), infoResetLabel, FALSE, FALSE, 0);


    // resetButton
    resetButton = gtk_button_new_with_label("Etalonner");
    gtk_box_pack_start(GTK_BOX(resetBox), resetButton, FALSE, FALSE, 0);
    g_signal_connect(resetButton, "clicked", G_CALLBACK(callBack_Reset), this);

    // discoButton1
    discoButton1 = gtk_button_new_with_label("Disco");
    gtk_box_pack_start(GTK_BOX(bottom), discoButton1, TRUE, TRUE, 0);
    addMagentaBorder(discoButton1);
    g_signal_connect(discoButton1, "clicked", G_CALLBACK(callBack_Disco1), this);
    
    // discoButton2
    discoButton2 = gtk_button_new_with_label("Techno");
    gtk_box_pack_start(GTK_BOX(bottom), discoButton2, TRUE, TRUE, 0);
    addOrangeBorder(discoButton2);
    g_signal_connect(discoButton2, "clicked", G_CALLBACK(callBack_Disco2), this);

    // recordButton
    recordButton = gtk_button_new_with_label("Enregistrer");
    gtk_box_pack_start(GTK_BOX(bottom), recordButton, TRUE, TRUE, 0);
    g_signal_connect(recordButton, "clicked", G_CALLBACK(callBack_Record), this);
    addGrayBorder(recordButton);


    setWigetForSpecificMode();
    desactiveForPowerOff();
    gtk_widget_show_all(window);


}

void InterfaceGraphique::waitEnd() {
    gtkThread->join();
}


void InterfaceGraphique::addRedBorder(GtkWidget* widget) {
    // Définit le style CSS pour la bordure
    const gchar *css = "button { border: 4px solid red; }";
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);
}

void InterfaceGraphique::addGreenBorder(GtkWidget* widget) {
    // Définit le style CSS pour la bordure
    const gchar *css = "button { border: 4px solid green; }";
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);
}

void InterfaceGraphique::addMagentaBorder(GtkWidget* widget) {
    // Définit le style CSS pour la bordure
    const gchar *css = "button { border: 4px solid magenta; }";
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);
}

void InterfaceGraphique::addOrangeBorder(GtkWidget* widget) {
    // Définit le style CSS pour la bordure
    const gchar *css = "button { border: 4px solid orange; }";
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);
}



void InterfaceGraphique::addGrayBorder(GtkWidget* widget) {
    // Définit le style CSS pour la bordure
    const gchar *css = "button { border: 4px solid gray; }";
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);
}

void InterfaceGraphique::changeColorRed(GtkWidget* widget) {
    // Définit le style CSS pour la couleur d'arrière-plan
    const gchar *css = "button { background-color: red; }";
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);    
}

void InterfaceGraphique::changeColorGreen(GtkWidget* widget) {
    // Définit le style CSS pour la couleur d'arrière-plan
    const gchar *css = "button { background-color: green; }";
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(provider);    
}


InterfaceGraphique::~InterfaceGraphique() {
    g_source_remove(timeout_id); 
    //gtk_main_quit(); -> genere une erreur bizarrement
}


std::string InterfaceGraphique::getModeName() {
    if (asservissement == POSITION) {
        return "Position  ";
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

    controlMoteur->changeAsservissement();
    
    if (asservissement==POSITION) {
        asservissement=HAPTIQUE;
        detruirePositonBox();
        setupHaptiqueWidget();
        printf("InterfaceGraphique::Debug : asservissement HAPTIC\n");
    } else {
        asservissement=POSITION;
        detruireHaptiqueBox();
        setupPositionWidget();
        printf("InterfaceGraphique::Debug : asservissement POSITION\n");
    }
    setWigetForSpecificMode();
}


void InterfaceGraphique::changePower() {
    controlMoteur->changePower();
}

void InterfaceGraphique::goTo(float positonX, float positionY) {
    if (positonX< Model::ZONE_TRAVAIL_MIN_X || positonX>Model::ZONE_TRAVAIL_MAX_X) {
        printf("en dehors de la zone\n");
        return ;
    }
    if (positionY< Model::ZONE_TRAVAIL_MIN_Y || positionY>Model::ZONE_TRAVAIL_MAX_Y) {
        printf("en dehors de la zone\n");
        return ;
    }
    controlMoteur->goTo(positonX,positionY);
}

void InterfaceGraphique::setVitesse(double vitesse) {
    controlMoteur->setVitesse(vitesse);
}

void InterfaceGraphique::setRaideur(double raideur) {
    controlMoteur->setRaideur(raideur);
}

void InterfaceGraphique::setViscosite(double viscosite) {
    controlMoteur->setViscosite(viscosite);
}

void InterfaceGraphique::reset() {
    controlMoteur->reset();
}

void InterfaceGraphique::disco1() {
    controlMoteur->disco();
}

void InterfaceGraphique::disco2() {
    controlMoteur->techno();
}



void InterfaceGraphique::record() {
    if (recordOn == false) {
        recordOn= true;
        gtk_button_set_label(GTK_BUTTON(recordButton),"En cours d'enregistrement");
        addRedBorder(recordButton);
        fichierEnregistrement = new std::ofstream(std::string("collectedData/")+heureActuelle());
        nombreEchantillon=0;
        
    }
    else {
        recordOn = false;
        gtk_button_set_label(GTK_BUTTON(recordButton),"Enregistrer");
        addGrayBorder(recordButton);
        fichierEnregistrement->close();
        delete fichierEnregistrement;
    }
}


void InterfaceGraphique::rafraichir() {
    double positionX=0;
    double positionY=0;

    double forceX=0;
    double forceY=0;

    double coupleX=0;
    double coupleY=0;

    double coupleMoteur[3];


    controlMoteur->getPosition(positionX,positionY);
    controlMoteur->getForce(forceX,forceY);
    rasberry->getCouples(coupleX,coupleY);
    controlMoteur->getCoupleMoteur(coupleMoteur);


    updateData(positionX,positionY,forceX,forceY,coupleX,coupleY,coupleMoteur);


    updateDisplayPower();


}


void InterfaceGraphique::updateData(double positionX,double positionY, double forceX,double forceY, double coupleX, double coupleY, double coupleMoteur[3]) {
    int nombreDecimal = 2;

    if (recordOn) {
        (*fichierEnregistrement) << "numero : " << std::to_string(nombreEchantillon) << " | ";
    }

    // position :

    std::ostringstream valeur;
    valeur << std::fixed << std::setprecision(nombreDecimal) << "X : " << positionX << " mm";
    if (recordOn) {
        (*fichierEnregistrement) << valeur.str() << " | ";
    }
    gtk_label_set_text(GTK_LABEL(positionXLabel),valeur.str().c_str());

    valeur.str(""); // vide le stream
    valeur << "Y : " << positionY << " mm";
    if (recordOn) {
        (*fichierEnregistrement) << valeur.str() << " | ";
    }
    gtk_label_set_text(GTK_LABEL(positionYLabel),valeur.str().c_str());

    // force :
    valeur.str(""); // vide le stream
    valeur << "Fx : " << forceX << " N";
    if (recordOn) {
        (*fichierEnregistrement) << valeur.str() << " | ";
    }
    gtk_label_set_text(GTK_LABEL(forceXLabel),valeur.str().c_str());

        valeur.str(""); // vide le stream
    valeur << "Fy : " << forceY << " N";
    if (recordOn) {
        (*fichierEnregistrement) << valeur.str() << " | ";
    }
    gtk_label_set_text(GTK_LABEL(forceYLabel),valeur.str().c_str());


    // moment :
        valeur.str(""); // vide le stream
    valeur << "Mx : " << coupleX << " N.mm";
    if (recordOn) {
        (*fichierEnregistrement) << valeur.str() << " | ";
    }
    gtk_label_set_text(GTK_LABEL(momentXLabel),valeur.str().c_str());

        valeur.str(""); // vide le stream
    valeur << "My : " << coupleY << " N.mm";
    if (recordOn) {
        (*fichierEnregistrement) << valeur.str() << " | ";
    }
    gtk_label_set_text(GTK_LABEL(momentYLabel),valeur.str().c_str());


    // couple moteur :
    valeur.str(""); // vide le stream
    valeur << "coupleMoteurI : " << coupleMoteur[0] << " N.mm";
    if (recordOn) {
        (*fichierEnregistrement) << valeur.str() << " | ";
    }
    gtk_label_set_text(GTK_LABEL(coupleMoteurILabel),valeur.str().c_str());

    valeur.str(""); // vide le stream
    valeur << "coupleMoteurII : " << coupleMoteur[1] << " N.mm";
    if (recordOn) {
        (*fichierEnregistrement) << valeur.str() << " | ";
    }
    gtk_label_set_text(GTK_LABEL(coupleMoteurIILabel),valeur.str().c_str());

    valeur.str(""); // vide le stream
    valeur << "coupleMoteurIII : " << coupleMoteur[2] << " N.mm";
    if (recordOn) {
        (*fichierEnregistrement) << valeur.str() << " | ";
    }
    gtk_label_set_text(GTK_LABEL(coupleMoteurIIILabel),valeur.str().c_str());

    if (recordOn) {
        (*fichierEnregistrement) << std::endl;
    }

    nombreEchantillon ++;


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


void InterfaceGraphique::callBack_ChangePowerButton(GtkWidget* button, gpointer data) {
    InterfaceGraphique* interfaceGraphique = static_cast<InterfaceGraphique*>(data);
    interfaceGraphique->changePower();
}


void InterfaceGraphique::setupPositionWidget() {

    // --- section go to ---

    goToBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);
    gtk_box_pack_start(GTK_BOX(instructionZone), goToBox, FALSE, FALSE, 0);

    goToButton = gtk_button_new_with_label("Aller à : ");
    gtk_box_pack_start(GTK_BOX(goToBox), goToButton, FALSE, FALSE, 0);
    g_signal_connect(goToButton, "clicked", G_CALLBACK(callBack_GoToButton), this);

    positionXEntry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(positionXEntry), "X en mm");
    gtk_box_pack_start(GTK_BOX(goToBox), positionXEntry, FALSE, FALSE, 0);

    positionYEntry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(positionYEntry), "Y en mm");
    gtk_box_pack_start(GTK_BOX(goToBox), positionYEntry, FALSE, FALSE, 0);

    // --- section set vitesse ---

    setVitesseBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);
    gtk_box_pack_start(GTK_BOX(instructionZone), setVitesseBox, FALSE, FALSE, 0);

    setVitesseButton = gtk_button_new_with_label("Définir la vitesse à :");
    gtk_box_pack_start(GTK_BOX(setVitesseBox), setVitesseButton, FALSE, FALSE, 0);
    g_signal_connect(setVitesseButton, "clicked", G_CALLBACK(callBack_SetVitesseButton), this);

    vitesseEntry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(vitesseEntry), "points/mm");
    gtk_box_pack_start(GTK_BOX(setVitesseBox), vitesseEntry, FALSE, FALSE, 0);


    gtk_widget_show_all(window);

}

void InterfaceGraphique::setupHaptiqueWidget() {


    // --- section set raideur ---

    setRaideurBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);
    gtk_box_pack_start(GTK_BOX(instructionZone), setRaideurBox, FALSE, FALSE, 0);

    setRaideurButton = gtk_button_new_with_label("Definir la raideur à :");
    gtk_box_pack_start(GTK_BOX(setRaideurBox), setRaideurButton, FALSE, FALSE, 0);
    g_signal_connect(setRaideurButton, "clicked", G_CALLBACK(callBack_SetRaideurButton), this);

    raideurEntry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(raideurEntry), "N/mm");
    gtk_box_pack_start(GTK_BOX(setRaideurBox), raideurEntry, FALSE, FALSE, 0);



    // --- section set viscosite ---

    setViscositeBox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL,10);
    gtk_box_pack_start(GTK_BOX(instructionZone), setViscositeBox, FALSE, FALSE, 0);

    setViscositeButton = gtk_button_new_with_label("Definir la viscosite à :");
    gtk_box_pack_start(GTK_BOX(setViscositeBox), setViscositeButton, FALSE, FALSE, 0);
    g_signal_connect(setViscositeButton, "clicked", G_CALLBACK(callBack_SetViscositeButton), this);

    viscositeEntry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(viscositeEntry), "kg/s");
    gtk_box_pack_start(GTK_BOX(setViscositeBox), viscositeEntry, FALSE, FALSE, 0);


    gtk_widget_show_all(window);


}



void InterfaceGraphique::callBack_GoToButton(GtkWidget* button, gpointer data) {
    InterfaceGraphique* interfaceGraphique = static_cast<InterfaceGraphique*>(data);



    char* verification;

    float positionX = strtof(gtk_entry_get_text(GTK_ENTRY(interfaceGraphique->positionXEntry)),&verification);
    if (verification == gtk_entry_get_text(GTK_ENTRY(interfaceGraphique->positionXEntry))) {
        std::cout << "DEBUG : entree invalide position X" << std::endl;
        return;
    }
    float positionY = strtof(gtk_entry_get_text(GTK_ENTRY(interfaceGraphique->positionYEntry)),&verification);
    if (verification == gtk_entry_get_text(GTK_ENTRY(interfaceGraphique->positionYEntry))) {
        std::cout << "DEBUG : entree invalide position Y" << std::endl;
        return;
    }
    interfaceGraphique->goTo(positionX,positionY);
}


void InterfaceGraphique::callBack_SetVitesseButton(GtkWidget* button, gpointer data) {
    InterfaceGraphique* interfaceGraphique = static_cast<InterfaceGraphique*>(data);



    char* verification;

    float vitesse = strtof(gtk_entry_get_text(GTK_ENTRY(interfaceGraphique->vitesseEntry)),&verification);
    if (verification == gtk_entry_get_text(GTK_ENTRY(interfaceGraphique->vitesseEntry))) {
        std::cout << "InterfaceGraphique::DEBUG : entree invalide vitesse" << std::endl;
        return;
    }

    interfaceGraphique->setVitesse(vitesse);
}


void InterfaceGraphique::callBack_SetRaideurButton(GtkWidget* button, gpointer data) {
    InterfaceGraphique* interfaceGraphique = static_cast<InterfaceGraphique*>(data);



    char* verification;

    float raideur = strtof(gtk_entry_get_text(GTK_ENTRY(interfaceGraphique->raideurEntry)),&verification);
    if (verification == gtk_entry_get_text(GTK_ENTRY(interfaceGraphique->raideurEntry))) {
        std::cout << "InterfaceGraphique::DEBUG : entree invalide raideur" << std::endl;
        return;
    }

    interfaceGraphique->setRaideur(raideur);
}



void InterfaceGraphique::callBack_SetViscositeButton(GtkWidget* button, gpointer data) {
    InterfaceGraphique* interfaceGraphique = static_cast<InterfaceGraphique*>(data);



    char* verification;

    float viscosite = strtof(gtk_entry_get_text(GTK_ENTRY(interfaceGraphique->viscositeEntry)),&verification);
    if (verification == gtk_entry_get_text(GTK_ENTRY(interfaceGraphique->viscositeEntry))) {
        std::cout << "InterfaceGraphique::DEBUG : entree invalide viscosite" << std::endl;
        return;
    }

    interfaceGraphique->setViscosite(viscosite);
}

void InterfaceGraphique::callBack_Reset(GtkWidget* button, gpointer data) {
    InterfaceGraphique* interfaceGraphique = static_cast<InterfaceGraphique*>(data);

     interfaceGraphique->reset();
}

void InterfaceGraphique::callBack_Disco1(GtkWidget* button, gpointer data) {
    InterfaceGraphique* interfaceGraphique = static_cast<InterfaceGraphique*>(data);

    interfaceGraphique->disco1();   
}


void InterfaceGraphique::callBack_Disco2(GtkWidget* button, gpointer data) {
    InterfaceGraphique* interfaceGraphique = static_cast<InterfaceGraphique*>(data);

    interfaceGraphique->disco2();   
}

void InterfaceGraphique::callBack_Record(GtkWidget* button, gpointer data) {
    InterfaceGraphique* interfaceGraphique = static_cast<InterfaceGraphique*>(data);

    interfaceGraphique->record();   
}



void InterfaceGraphique::detruirePositonBox() {
    goToButton = nullptr;
    positionXEntry = nullptr;
    positionYEntry = nullptr;

    setVitesseButton = nullptr;
    vitesseEntry = nullptr;

    gtk_widget_destroy(setVitesseBox);

    setVitesseBox = nullptr;

    gtk_widget_destroy(goToBox);

    goToBox = nullptr;

    setViscositeButton =nullptr;

}

void InterfaceGraphique::detruireHaptiqueBox() {


    gtk_widget_destroy(setViscositeBox);
    setViscositeBox = nullptr;

    gtk_widget_destroy(setRaideurBox);
    setRaideurBox = nullptr;

}

void InterfaceGraphique::warning(std::string typeDonnee, std::string data) {
    ofstream myfile;
    myfile.open (data);
    myfile << (data+" -> "+typeDonnee) <<std::endl;
    myfile.close();
}


void InterfaceGraphique::setPowerOn() {
    powerOn=true;
    gtk_button_set_label(GTK_BUTTON(changePowerButton),"Moteur sous tension");
    addGreenBorder(changePowerButton);
    // printf("InterfaceGraphique::Debug : power ON\n");
    activeForPowerOn();
}

void InterfaceGraphique::setPowerOff() {
    powerOn=false;
    gtk_button_set_label(GTK_BUTTON(changePowerButton),"Moteur hors tension");
    addRedBorder(changePowerButton);
    // printf("InterfaceGraphique::Debug : power OFF\n");
    desactiveForPowerOff();
}

void InterfaceGraphique::updateDisplayPower() {
    if (powerOn != controlMoteur->getPowerOn()) {
        if (controlMoteur->getPowerOn()) {
            setPowerOn();
        } else {
            setPowerOff();
        }
    }
}

void InterfaceGraphique::desactiveForPowerOff() {
        gtk_widget_set_sensitive(changeModeButton, FALSE);

    if (setVitesseButton) {
        gtk_widget_set_sensitive(setVitesseButton, FALSE);
        gtk_widget_set_sensitive(goToButton, FALSE);
    }

    if (setViscositeButton) {
        gtk_widget_set_sensitive(setViscositeButton, FALSE);
        gtk_widget_set_sensitive(setRaideurButton, FALSE);
    }


}


// active et fait apparaitre les widgets pour powerON = true
void InterfaceGraphique::activeForPowerOn() {
    gtk_widget_set_sensitive(changeModeButton, TRUE);

    if (setVitesseButton) {
        gtk_widget_set_sensitive(setVitesseButton, TRUE);
        gtk_widget_set_sensitive(goToButton, TRUE);
    }

    if (setViscositeButton) {
        gtk_widget_set_sensitive(setViscositeButton, TRUE);
        gtk_widget_set_sensitive(setRaideurButton, TRUE);
    }




}


void InterfaceGraphique::desactiveResetButton() {
    gtk_widget_set_sensitive(resetButton, FALSE);
}

void InterfaceGraphique::activeResetButton() {
    gtk_widget_set_sensitive(resetButton, TRUE);
}

std::string InterfaceGraphique::heureActuelle() {
    // Obtention de l'instant actuel
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

    // Conversion de l'instant actuel en format de temps en utilisant la structure tm
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);
    std::tm* timeInfo = std::localtime(&currentTime);

    // Conversion de la structure tm en chaînes de caractères
    char buffer[80];
    std::strftime(buffer, 80, "%Y-%m-%d %H:%M:%S", timeInfo);

    // Conversion de la chaîne de caractères en std::string
    return std::string(buffer);   
}