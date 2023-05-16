#include "ControlMoteur.hpp"
#include <iostream>

ControlMoteur::ControlMoteur() {
    mode = POSITION;
    // [!] A IMPLEMENTER PAR OLLIVIER 
}

void ControlMoteur::getForce(double &forceX, double &forceY) {
    // [!] A IMPLEMENTER PAR OLLIVIER
    
    // POUR TESTE :
    forceX = 15;
    forceY = 15;

}

// [!] A COMPLETER PAR OLLIVIER
void ControlMoteur::changeMode(){
    if (mode==POSITION) {
        mode=HAPTIC;
        printf("Debug : ControlMoteur en mode HAPTIC\n");
        // [!] A COMPLETER PAR OLLIVIER
    } else {
        mode=POSITION;
        printf("Debug : ControlMoteur en mode POSITION\n");
        // [!] A COMPLETER PAR OLLIVIER
    }
}