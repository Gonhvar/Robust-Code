#include "Rasberry.hpp"
#include <iostream>


Rasberry::Rasberry() {
    // [!] A COMPLETER PAR OLIVIER

    rasberryThread = new std::thread(&Rasberry::runRasberry,this);
}

void Rasberry::runRasberry() {
    while (true)
    {
        printf("Debug : runRasberry\n");
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