#include "InterfaceGraphique.hpp"
#include "Rasberry.hpp"
#include "ControlMoteur.hpp"
#include "modele.hpp"


#include <gtk/gtk.h>
#include <thread>

void runGtkMainTest() {
    gtk_main();
}

int main(int argc, char *argv[])
{


    ControlMoteur controlMoteur;
    Rasberry rasberry;

    InterfaceGraphique interfaceGraphique(&rasberry, &controlMoteur);







    interfaceGraphique.waitEnd();
    controlMoteur.waitEnd();
    rasberry.waitEnd();




    return 0;
}





