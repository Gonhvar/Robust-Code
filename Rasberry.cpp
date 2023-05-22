#include "Rasberry.hpp"
#include <iostream>


int Rasberry::nombreInstance=0;

Rasberry::Rasberry() {
    if (nombreInstance>0) {
        throw std::runtime_error("Rasberry::Rasberry -> impossible d'instancier plus d'une fois une Rasberry\n");
    }
    nombreInstance++;

    // [!] POUR TESTS :
    coupleX = -0.1;
    coupleY = -5.1;


    rasberryThread = new std::thread(&Rasberry::runRasberry,this);
}

// [!] A IMPLEMENTER PAR OLIVIER
void Rasberry::runRasberry() {
    // FAIRE ICI L'ASSERVISSEMENT ~100 Hz ?
    // actualiser dans l'asservissement coupleX, coupleY

    while (true)
    {
        //printf("Debug : runRasberry\n");
    }
}


void Rasberry::getCouples(double &coupleX, double &coupleY) {
    //std::cout << "Rasberry::getCouples" << std::endl;
    coupleX=this->coupleX;
    coupleY=this->coupleY;
}

void Rasberry::waitEnd() {
    rasberryThread->join();
}