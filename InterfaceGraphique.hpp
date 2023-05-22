#include <gtk/gtk.h>
#include "Robust.hpp"
#include <thread>
#include <memory>

class Rasberry;
class ControlMoteur;


// permet de lancer l'interface graphique
// /!\ instancier qu'une seule fois
class InterfaceGraphique {
    private :
        // --- CONSTANTES ---
        const static int WIDTH_WINDOW = 1080;
        const static int HEIGHT_WINDOW = 720;

        const static int WIDTH_DRAWING_AREA = 500;
        const static int HEIGHT_DRAWING_AREA = 350;

        const static int PERIODE_RAFRAICHISSEMENT = 30; // en ms

        // --- STRUCT ---

        // permet d'envoyer des donnes aux callback
        typedef struct DataForCallBack_
        {
            InterfaceGraphique *thisAttribut;
            float data1;
            float data2;
        } DataForCallBack;

        // intitialiser cet attribut pour envoyer des valeur aux callback
        DataForCallBack dataForCallBack;
        


        // --- ATTRIBUTS ---

        enum Asservissement {POSITION,HAPTIQUE};
        Asservissement asservissement;

        bool powerOn;

        GtkWidget *window;
        std::thread *gtkThread;
        guint timeout_id; // permet de nettoyer le rafraichissement
        static int nombreInstance; //garantie qu'on instancie qu'une fois on utilise pas singleton patern car thread

        Rasberry *rasberry;
        ControlMoteur *controlMoteur;



        // CONTENEURS :
        
        // contient head, body et bottom
        GtkWidget *main;

        // haut de la page
        GtkWidget *head;
        // milieu de la page
        GtkWidget *body;
        // fin de la page
        GtkWidget *bottom;


        // ---HEAD DE LA PAGE--- :

        GtkWidget *changeModeButton;

        GtkWidget *recordDataBox; // contient les forces, positions et couples
        
        // position :
        GtkWidget *positionBox;
        GtkWidget *positionXLabel;
        GtkWidget *positionYLabel;
        
        // force :
        GtkWidget *forceBox;
        GtkWidget *forceXLabel;
        GtkWidget *forceYLabel;

        // moment :
        GtkWidget *momentBox;
        GtkWidget *momentXLabel;
        GtkWidget *momentYLabel;

        GtkWidget *changePowerButton;




        // --- BODY DE LA PAGE --- :

        // contenu change en fonction du mode haptique ou position
        GtkWidget *instructionZone; // conteneur vertiacla

        // pour position :
        GtkWidget* goToBox;  
        GtkWidget* goToButton;
        GtkWidget* positionXEntry;
        GtkWidget* positionYEntry;

        GtkWidget* setVitesseBox;
        GtkWidget* setVitesseButton;
        GtkWidget* vitesseEntry;

        // pour haptique
        GtkWidget* setRaideurBox;
        GtkWidget* setRaideurButton;
        GtkWidget* raideurEntry;
        GtkWidget* setViscositeBox;
        GtkWidget* setViscositeButton;
        GtkWidget* viscositeEntry;


        GtkWidget* drawing_area;

        // --- HEAD DE LA PAGE ---:

        GtkWidget* resetButton;




        // --- METHODES ---

        // cree les widget, les initialises, les places au bon endroit
        void initWigets();

        void initWindow();

        // va etre lance dans un thread
        void runGtkMain();

        std::string getModeName();

        // cree les widget pour un mode specifique et change les widgets en fonction du mode
        void setWigetForSpecificMode();

        // rafraichi l'interface graphique (fait des appels a rasberry et controlMoteur)
        // appele toutes les PERIODE_RAFRAICHISSEMENT
        void rafraichir();


        // met a jour l'affichage de la data
        void updateData(double positionX,double positionY, double forceX, double froceY, double coupleX, double coupleY);

        static void addCSS(GtkWidget* window);

        static void addRedBorder(GtkWidget* widget);

        static void addGreenBorder(GtkWidget* widget);

        static void changeColorRed(GtkWidget* widget);

        static void changeColorGreen(GtkWidget* widget);

        static void setMargin(GtkWidget* widget, int margin);

        // cree, met en place et initialise la section specifique a la position
        // dans le corps de instructionZone
        void setupPositionWidget();

        // cree, met en place et initialise la section specifique a l'haptique
        // dans le corps de instructionZone
        void setupHaptiqueWidget();


        void changeMode();

        void changePower();

        void goTo(float positonX, float positionY);

        void setVitesse(double vitesse);

        void setRaideur(double raideur);

        void setViscosite(double viscosite);

        void reset();

        void detruirePositonBox();

        void detruireHaptiqueBox();

    public :
        // lance l'interface dans un thread 
        InterfaceGraphique(Rasberry *rasberry, ControlMoteur *controlMoteur);

        ~InterfaceGraphique();


        // fonction blocante qui attend que le thread se finisse
        // /!\ appeler qu'une fois
        void waitEnd();



    static gboolean callbackWrapper_Rafraichir(gpointer data);





    // --- ENSEMBLE CALLBACK --- :


    // mettre this en data
    static void callBack_ChangeModeButton(GtkWidget* button, gpointer data);

    // mettre this en data
    static void callBack_ChangePowerButton(GtkWidget* button, gpointer data);

    // mettre this en data
    static void callBack_GoToButton(GtkWidget* button, gpointer data);

    // mettre this en data
    static void callBack_SetVitesseButton(GtkWidget* button, gpointer data);

    // mettre this en data
    static void callBack_SetRaideurButton(GtkWidget* button, gpointer data);

    // mettre this en data
    static void callBack_SetViscositeButton(GtkWidget* button, gpointer data);

    // mettre this en data
    static void callBack_Reset(GtkWidget* button, gpointer data);



};




