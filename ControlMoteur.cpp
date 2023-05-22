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

    // [!] POUR TESTS :
    this->forceX = 15;
    this->forceY = -9.6;
    this->positionX = 5 ;
    this->positionY =-3.2 ;


    controlMoteurThread = new std::thread(&ControlMoteur::runControlMoteur,this);
}

// [!] A IMPLEMENTER PAR OLIVIER
void ControlMoteur::runControlMoteur() {
    // FAIRE ICI L'ASSERVISSEMENT ~100 Hz ?
    // actualiser dans l'asservissement forceX, forceY, positionX, positionY


    while (true)
    {
        //printf("Debug : ControlMoteur\n");
    }
    
} 

void ControlMoteur::waitEnd() {
    controlMoteurThread->join();
}

void ControlMoteur::getForce(double &forceX, double &forceY) {
    //std::cout << "ControlMoteur::getForce" << std::endl;
    forceX = this->forceX;
    forceY = this->forceY;

}

// [!] A COMPLETER PAR OLIVIER
void ControlMoteur::changeAsservissement(){
    if (asservissement==POSITION) {
        setAsservissementToHAPTIC();
    } else {
        setAsservissementToPOSTION();
    }
}

void ControlMoteur::setAsservissementToHAPTIC() {
    asservissement=HAPTIC;
        printf("Debug : ControlMoteur en asservissement HAPTIC\n");
        // [!] A COMPLETER PAR OLIVIER
}

void ControlMoteur::setAsservissementToPOSTION() {
        asservissement=POSITION;
        printf("Debug : ControlMoteur en asservissement POSITION\n");
        // [!] A COMPLETER PAR OLIVIER  
}


void ControlMoteur::getPosition(double &positionX, double &positionY) {
    //std::cout << "ControlMoteur::getPosition" << std::endl;
    positionX = this->positionX;
    positionY = this->positionY;
}

void ControlMoteur::changePower() {
    if (powerOn) {
        setPowerToOff();

    } else {
        setPowerToOn();
    }
}

void ControlMoteur::setPowerToOff() {
    // [!] A COMPLETER PAR OLIVIER
    powerOn=false;

    printf("ControlMoteur : moteur eteint\n");
}

void ControlMoteur::setPowerToOn() {
    // [!] A COMPLETER PAR OLIVIER
    powerOn=true;

    printf("ControlMoteur : moteur allume\n");
}


void ControlMoteur::goTo(double positionX,double positionY) {
    if (asservissement==POSITION) {
        printf("ControlMoteur::Debug : va vers la position %lf mm %lf mm\n",positionX,positionY);
        // [!] A COMPLETER PAR OLIVIER
    } else {
        printf("[!] impossible d'aller a une positon en mode haptic\n");
    }
}

// [!] A IMPLEMENTER PAR OLIVIER
void ControlMoteur::setVitesse(double vitesse) {
    printf("ControlMoteur::Debug : vitesse affecte a %lf mm/s\n",vitesse);
}

// [!] A IMPLEMENTER PAR OLIVIER
void ControlMoteur::setRaideur(double raideur) {
    printf("ControlMoteur::Debug : raideur affecte a %lf N/mm\n",raideur);
}

// [!] A IMPLEMENTER PAR OLIVIER
void ControlMoteur::setViscosite(double viscosite) {
    printf("ControlMoteur::Debug : viscosite affecte a %lf kg/s\n",viscosite); 
}

// [!] A IMPLEMENTER PAR OLIVIER
void ControlMoteur::reset() {
    printf("ControlMoteur::Debug : reset\n"); 
}


