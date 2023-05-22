#include <gtk/gtk.h>
#include "Robust.hpp"
#include <thread>

class Rasberry;
class ControlMoteur;


// permet de lancer l'interface graphique
// /!\ instancier qu'une seule fois
class InterfaceGraphique {
    private :
        // --- CONSTANTES ---
        const static int WIDTH = 1080;
        const static int HEIGHT = 720;


        // --- ATTRIBUTS ---

        enum Asservissement {POSITION,HAPTIQUE};
        Asservissement asservissement;
        GtkWidget *window;
        std::thread *gtkThread;
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





        // ---BODY DE LA PAGE--- :

        GtkWidget* drawing_area;




        // --- METHODES ---

        // cree les widget, les initialises, les places au bon endroit
        void initWigets();

        void initWindow();

        // va etre lance dans un thread
        void runGtkMain();

        std::string getModeName();

        // cree les widget pour un mode specifique et change les widgets en fonction du mode
        void setWigetForSpecificMode();



    public :
        // lance l'interface dans un thread 
        InterfaceGraphique(Rasberry *rasberry, ControlMoteur *controlMoteur);

        ~InterfaceGraphique();


        // fonction blocante qui attend que le thread se finisse
        // /!\ appeler qu'une fois
        void waitEnd();

        void changeMode();









};


// ENSEMBLE CALLBACK :

void InterfaceGraphique_callBack_ChangeModeButton(GtkWidget* button, gpointer data);