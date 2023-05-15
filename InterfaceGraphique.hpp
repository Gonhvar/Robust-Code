#include <gtk/gtk.h>
#include "Robust.hpp"

class InterfaceGraphique {
    private :
        enum Etat {POSITION,HAPTIQUE};
        Etat etat;
        GtkWidget *window;

        void initWindow();

        void run();


    public :
        // lance un thread 
        InterfaceGraphique(int argc, char *argv[]);







};