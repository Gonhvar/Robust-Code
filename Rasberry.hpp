#ifndef RASBERRY
#define RASBERRY

#include <thread>
#include <iostream>
#include <cstdio>
#include <cstring>  
#include <fstream>
#include <vector>


#define VALMAXPOT 4010
#define VALMINPOT 2000
#define ANGLEMAX 45

// /!\ A instancier une seule fois
class Rasberry {
    private :
        
        std::thread *rasberryThread;

        static int nombreInstance; //garantie qu'on instancie qu'une fois, on utilise pas singleton patern car thread
        
        int potX = 0;
        int potY = 0;

        double angleX;
        double angleY;
        
        float coupleX; // N.mm
        float coupleY; // Nmm



        // [!] A IMPLEMENTER PAR OLIVIER
        // va etre lance dans un thread
        void runRasberry();

    public :

        // [!] A IMPLEMENTER PAR OLIVIER
        Rasberry();

        std::vector<int> splitString(std::string* line);

        // affecte a coupleX et coupeY le couple engendree par la sonde sur l'effecteur en N.mm
        void getCouples(double &coupleX, double &coupeY);

        // fonction blocante qui attend que le thread se finisse
        // /!\ appeler qu'une fois
        void waitEnd();

};

#endif