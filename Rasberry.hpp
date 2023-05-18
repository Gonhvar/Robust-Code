#ifndef RASBERRY
#define RASBERRY

#include <thread>


// /!\ A instancier une seule fois
class Rasberry {
    private :
        
        std::thread *rasberryThread;

        static int nombreInstance; //garantie qu'on instancie qu'une fois, on utilise pas singleton patern car thread
        
        // [!] A IMPLEMENTER PAR OLIVIER
        // va etre lance dans un thread
        void runRasberry();

    public :

        // [!] A IMPLEMENTER PAR OLIVIER
        Rasberry();

        // [!] A IMPLEMENTER PAR OLIVIER
        // affecte a coupleX et coupeY le couple engendree par la sonde sur l'effecteur en N.mm
        void getAngles(double &coupleX, double &coupeY);

        // fonction blocante qui attend que le thread se finisse
        // /!\ appeler qu'une fois
        void waitEnd();

};
















#endif