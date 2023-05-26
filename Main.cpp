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

    double vitesseMoteur[3]= {166,213,147};
    double position_effecteur[2] = {550,150};
    double vitesseEffecteur[2];

    // Model::vitesseMoteur2effecteur(vitesseMoteur,position_effecteur,vitesseEffecteur);

    // printf("Vx %f Vy %f\n",vitesseEffecteur[0],vitesseEffecteur[1]);

    ControlMoteur controlMoteur;
    Rasberry rasberry;

    InterfaceGraphique interfaceGraphique(&rasberry, &controlMoteur);







    interfaceGraphique.waitEnd();
    controlMoteur.waitEnd();
    rasberry.waitEnd();




    return 0;
}





