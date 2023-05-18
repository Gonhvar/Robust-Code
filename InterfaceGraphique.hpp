#include <gtk/gtk.h>
#include "Robust.hpp"
#include <thread>


// permet de lancer l'interface graphique
// /!\ instancier qu'une seule fois
class InterfaceGraphique {
    private :
        enum Etat {POSITION,HAPTIQUE};
        Etat etat;
        GtkWidget *window;
        std::thread *gtkThread;

        void initWindow();

        // va etre lance dans un thread
        void runGtkMain();

        


    public :
        // lance l'interface dans un thread 
        InterfaceGraphique(int argc, char *argv[]);

        ~InterfaceGraphique();


        // fonction blocante qui attend que le thread se finisse
        // /!\ appeler qu'une fois
        void waitEnd();







};