#ifndef CONTROL_MOTEUR
#define CONTROL_MOTEUR

#include <thread>

// /!\ A instancier une seule fois
class ControlMoteur {
    private :
        enum Asservissement {POSITION, HAPTIC};
        bool powerOn;
        Asservissement asservissement;
        std::thread *controlMoteurThread;

        static int nombreInstance; //garantie qu'on instancie qu'une fois, on utilise pas singleton patern car thread

        // [!] A IMPLEMENTER PAR OLIVIER
        // va etre lance dans un thread
        void runControlMoteur();
    

    public:

        ControlMoteur();

        // [!] A IMPLEMENTER PAR OLIVIER
        // affecte a forceX et forceY la force engendree par les moteurs sur la sonde en N
        void getForce(double &forceX, double &forceY);

        // [!] A IMPLEMENTER PAR OLIVIER
        // affecte a positionX et positionY la position de l'effecteur en mm
        void getPosition(double &positionX, double &positionY);

        // [!] A COMPLETER PAR OLIVIER
        // switch entre l'Asservissement en POSITION et HAPTIC
        void changeAsservissement();

        void setAsservissementToHAPTIC();

        void setAsservissementToPOSTION();

        Asservissement getAsservissement() {return asservissement;}

        // [!] A COMPLETER PAR OLIVIER
        // switch entre allume et eteint les moteurs
        void changePower();

        bool getPowerOn() {return powerOn;}

        // [!] A COMPLETER PAR OLIVIER
        // dit d'aller a l'effecteur d'aller en (positionX,positionY) en mm
        void goTo(double positionX,double positionY);

        // [!] A IMPLEMENTER PAR OLIVIER
        // met la vitesse de l'effecteur a vitesse pour le mode POSITION
        // vitesse en mm/s
        void setVitesse(double vitesse);

        // [!] A IMPLEMENTER PAR OLIVIER
        // affecte la raideur pour le mode HAPTIC 
        // raideur en N/mm
        void setRaideur(double raideur);

        // [!] A IMPLEMENTER PAR OLIVIER
        // affecte la viscosite pour le mode HAPTIC 
        // raideur en kg/s
        void setViscosite(double viscosite);

        // [!] A IMPLEMENTER PAR OLIVIER
        // reetalone les encodeurs
        // remet l'effecteur en position centrale
        void reset();

        // fonction blocante qui attend que le thread se finisse
        // /!\ appeler qu'une fois
        void waitEnd();







} ;


















#endif