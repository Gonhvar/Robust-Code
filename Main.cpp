#include "InterfaceGraphique.hpp"
#include "Rasberry.hpp"
#include "ControlMoteur.hpp"
#include "modele.hpp"


#include <gtk/gtk.h>
#include <thread>



int main(int argc, char *argv[])
{


    InterfaceGraphique interfaceGraphique;
    ControlMoteur controlMoteur;
    Rasberry rasberry;







    interfaceGraphique.waitEnd();
    controlMoteur.waitEnd();
    rasberry.waitEnd();




    return 0;
}



