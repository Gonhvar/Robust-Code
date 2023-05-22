#include "Rasberry.hpp"
#include <iostream>


int Rasberry::nombreInstance=0;

Rasberry::Rasberry() {
    if (nombreInstance>0) {
        throw std::runtime_error("Rasberry::Rasberry -> impossible d'instancier plus d'une fois une Rasberry\n");
    }
    nombreInstance++;
    // [!] A COMPLETER PAR OLIVIER

    rasberryThread = new std::thread(&Rasberry::runRasberry,this);
}

void Rasberry::runRasberry() {
    while (true)
    {
        //printf("Debug : runRasberry\n");
    }
}


void Rasberry::getAngles(double &coupleX, double &coupleY) {
    // [!] A IMPLEMENTER PAR OLIVIER
    
    // POUR TESTE :
    coupleX = 10;
    coupleY = 10;



}

void Rasberry::waitEnd() {
    rasberryThread->join();
}