#include "ControlMoteur.hpp"
#include <iostream>

int ControlMoteur::nombreInstance =0;

ControlMoteur::ControlMoteur() {
    if (nombreInstance>0) {
        throw std::runtime_error("ControlMoteur::ControlMoteur() -> impossible d'instancier plus d'une fois un ControlMoteur\n");
    }
    nombreInstance++;


    asservissement = POSITION;
    powerOn = false;
    controlMoteurThread = new std::thread(&ControlMoteur::runControlMoteur,this);
}

// [!] A IMPLEMENTER PAR OLIVIER
void ControlMoteur::runControlMoteur() {
    while (true)
    {
        printf("Debug : ControlMoteur\n");
    }
    
} 

void ControlMoteur::waitEnd() {
    controlMoteurThread->join();
}

void ControlMoteur::getForce(double &forceX, double &forceY) {
    // [!] A IMPLEMENTER PAR OLIVIER
    
    // POUR TESTE :
    forceX = 15;
    forceY = 15;

}

// [!] A COMPLETER PAR OLIVIER
void ControlMoteur::changeAsservissement(){
    if (asservissement==POSITION) {
        asservissement=HAPTIC;
        printf("Debug : ControlMoteur en asservissement HAPTIC\n");
        // [!] A COMPLETER PAR OLIVIER
    } else {
        asservissement=POSITION;
        printf("Debug : ControlMoteur en asservissement POSITION\n");
        // [!] A COMPLETER PAR OLIVIER
    }
}


void ControlMoteur::getPosition(double &positionX, double &positionY) {
    // [!] A IMPLEMENTER PAR OLIVIER
    
    // POUR TESTE :
    positionX = 34;
    positionY = 35;
}

void ControlMoteur::changePower() {
    if (powerOn) {
        powerOn=false;
        printf("Debug : moteur eteint\n");
        // [!] A COMPLETER PAR OLIVIER
    } else {
        powerOn=true;
        printf("Debug : moteur allume\n");
        // [!] A COMPLETER PAR OLIVIER
    }
}


void ControlMoteur::goTo(double positionX,double positionY) {
    if (asservissement==POSITION) {
        printf("Debug : va vers la position\n");
        // [!] A COMPLETER PAR OLIVIER
    } else {
        printf("[!] impossible d'aller a une positon en mode haptic\n");
    }
}

// [!] A IMPLEMENTER PAR OLIVIER
void ControlMoteur::setVitesse(double vitesse) {
    printf("Debug : vitesse affecte\n");
}

// [!] A IMPLEMENTER PAR OLIVIER
void ControlMoteur::setRaideur(double raideur) {
    printf("Debug : raideur affecte\n");
}

// [!] A IMPLEMENTER PAR OLIVIER
void ControlMoteur::setViscosite(double viscosite) {
    printf("Debug : viscosite affecte\n"); 
}

// [!] A IMPLEMENTER PAR OLIVIER
void ControlMoteur::reset() {
    printf("Debug : reset\n"); 
}


