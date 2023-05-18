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
        static int nombreInstance; //garantie qu'on instancie qu'une fois on utilise pas singleton patern car thread

        void initWindow();

        // va etre lance dans un thread
        void runGtkMain();

        



    public :
        // lance l'interface dans un thread 
        InterfaceGraphique();

        ~InterfaceGraphique();


        // fonction blocante qui attend que le thread se finisse
        // /!\ appeler qu'une fois
        void waitEnd();







};