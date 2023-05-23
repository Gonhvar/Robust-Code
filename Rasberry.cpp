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
    const char* devicePath = "/dev/ttyACM0";
    std::ifstream serial(devicePath);

    if (!serial) {
        std::cerr << "Attention ! Raspberry non-connecté !" << std::endl;
        //return 1;
    }

    while (true)
    {
        //printf("Debug : runRasberry\n");
        std::string line;
        std::string token;
        const char* seperator = " "; // space  
        char *ptr; 

        while (std::getline(serial, line)) {
            //Ici, s'occuper des données reçues
            //std::cout << line << std::endl;
            char* cstr = new char[line.length() + 1];
            std::strcpy(cstr, line.c_str());

            ptr = std::strtok(cstr, seperator);

            while (ptr != NULL)  
            {  
                std::cout << ptr  << std::endl; // print the string token  
                ptr = strtok (NULL, seperator);
            }  

            delete[] cstr;
        }
    }
    serial.close();
}


void Rasberry::getCouples(double &coupleX, double &coupleY) {
    //std::cout << "Rasberry::getCouples" << std::endl;
    coupleX=this->coupleX;
    coupleY=this->coupleY;
}

void Rasberry::waitEnd() {
    rasberryThread->join();
}