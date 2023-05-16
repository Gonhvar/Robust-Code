#ifndef CONTROL_MOTEUR
#define CONTROL_MOTEUR

// /!\ A instancier une seule fois
class ControlMoteur {
    private :
        enum Mode {POSITION, HAPTIC};
        Mode mode;
    

    public:
        // [!] A IMPLEMENTER PAR OLLIVIER    
        ControlMoteur();

        // [!] A IMPLEMENTER PAR OLLIVIER
        // affecte a forceX et forceY la force engendree par les moteurs sur la sonde en N
        void getForce(double &forceX, double &forceY);

        // [!] A IMPLEMENTER PAR OLLIVIER
        void getPosition(double &positionX, double &positionY);

        // [!] A COMPLETER PAR OLLIVIER
        // switch entre le mode POSITION et HAPTIC
        void changeMode();









} ;


















#endif