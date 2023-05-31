#ifndef CONTROL_MOTEUR
#define CONTROL_MOTEUR

#include <thread>
#include <stdio.h>
#include <stdint.h>
#include <PCANBasic.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <vector>
#include <fstream>
#include <signal.h>
#include <cstdlib>
#include <time.h>

#include "modele.hpp"

using namespace std;

#define POINTS_PAR_MM 10
#define MM_PAR_POINTS 0.1

//Definition des COBID
#define COBID_ALL_CAN_SDO 0x600
#define COBID_CAN1_SDO 0x601 
#define COBID_CAN2_SDO 0x602 
#define COBID_CAN3_SDO 0x603 

//-W = write | R = Read | B = Bytes-
#define W_1B 0x2F
#define W_2B 0x2B
#define W_4B 0x23
#define R 0x40
#define R_1B 0x4F
#define R_2B 0x4B
#define R_4B 0x43

//INDEX :
#define CONTROLWORD 0x6040
#define STATUSWORD  0x6041
#define MODES_OF_OPERATION 0x6060
#define TARGET_TORQUE 0x6071
#define TARGET_POSITION 0x607A
#define HOMING_METHOD 0x6098
#define TORQUE_OFFSET 0x60B2

#define ACTUALPOSITION 0x6064
#define ACTUALVELOCITY 0x30D3
#define ACTUALTORQUE 0x6077

#define DEFAULTPOSITIONX 400
#define DEFAULTPOSITIONY 347

// CONSTANTES MOTEUR
#define MOTOR_RATED_TORQUE 75.7

typedef struct tagmsgRecu{
  uint8_t id;
  uint16_t index;
  uint8_t subIndex;
  uint32_t valData;

  uint8_t taille;
  //True si le message en question n'est qu'un message de confirmation de reception
  bool isConfirmReception;
}msgRecu; 

// /!\ A instancier une seule fois
class ControlMoteur {
    private :
        enum Asservissement {POSITION, HAPTIC, ETALONNAGE};
        bool powerOn;
        Asservissement asservissement;
        std::thread *controlMoteurThread;

        static int nombreInstance; //garantie qu'on instancie qu'une fois, on utilise pas singleton patern car thread
        
        bool enDeplacement;

        double wantedPositionX; //mm
        double wantedPositionY; //mm

        double positionX; //mm
        double positionY; //mm

        double forceX; //N
        double forceY; //N

        double raideur;
        double viscosite;

        double vitesseEffecteur[2];
        double forceCable[3];

        //En points
        int increment[3];
        //En RPM
        double velocity[3];
        //N.mm
        double torque[3];



        //================INITIALISATION_PEAK================
        TPCANHandle find_channel();
        TPCANBaudrate init_baudrate_doc();
        void initialise_CAN_USB();

        //==================MESSAGES==================

        void init_msg_SDO(TPCANMsg* msg, int id, uint8_t data_length, uint16_t index, uint8_t subIndex, uint8_t data[4]);
        void print_message(TPCANMsg received);
        void print_vectorMessage(vector<msgRecu> get);
        msgRecu get_message(TPCANMsg received);
        vector<msgRecu> read_message();
        void write_message(TPCANMsg msg);

        //==================POSITION MODE==================

        
        void def_positionAbsolue(int id);
        bool init_asservissementPosition(int id);
        void set_relativePosition(int id, int uInput);
        void set_absolutePosition(int id, int uInput);
        void checkEndTarget(uint8_t* status, int motId);
        void checkAllEndTarget();
        void check23EndTarget();
        void get_manualWantedPos(double *wantPosX , double *wantPosY);
        void set_manualUserPosition(int id);
        
        //==================TORQUE MODE==================

        bool init_asservissementForce(int id);
        void set_force(double force, int id);
        void set_manual_torque(int id);
        void set_torque(uint16_t userInput, int id);

        
        //==================ECRITURE/LECTURE FICHIER==================
        
        void readPos_fichier();
        void writePos_fichier();

        //==================QUIT==================

        void shutdown(int id);
        void shutdown_all();
        //void signal_callback_handler(int signum);

        //==================READ DATA==================
        int read_torque(int id);
        int read_velocity(int id);
        int read_position(int id);
        vector<msgRecu> get_value(TPCANMsg toSend);

        //==================HOMING MODE==================
        bool init_homingMode(int id);

        //==================PARTIE GRAPHIQUE==================
        // [!] A IMPLEMENTER PAR OLIVIER
        // va etre lance dans un thread
        void runControlMoteur();

        //==================AVEC MODELES==================

        void control_allPosition(double wantPosX, double wantPosY);
        void miseDeplacementManuelForce();
        void mise_en_position0_effecteur();
        void control_haptique();
        void findEffectorSpeed(double couple_moteur[3]);
        void updateValeurs();


        // sortie [−32768, 32767]
        static int Int16ToSingnedInt(uint16_t value);
      
        void homing(int id);
        

    public:

        ControlMoteur();

        // affecte a forceX et forceY la force engendree par les moteurs sur la sonde en N
        void getForce(double &forceX, double &forceY);

        void getCoupleMoteur(double coupleMoteur[3]);

        // affecte a positionX et positionY la position de l'effecteur en mm
        void getPosition(double &positionX, double &positionY);


        // switch entre l'Asservissement en POSITION et HAPTIC
        void changeAsservissement();
        Asservissement getAsservissement() {return asservissement;}

        // switch entre allume et eteint les moteurs
        void changePower();
        void setPowerToOff();
        void setPowerToOn();
        bool getPowerOn() {return powerOn;}

        

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
        
        // [!] A IMPLEMENTER PAR OLIVIER
        void disco();

        void techno();



        // fonction blocante qui attend que le thread se finisse
        // /!\ appeler qu'une fois
        void waitEnd();
} ;


















#endif