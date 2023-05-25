#ifndef MODELE
#define MODELE

#include "math.h"
#include "stdio.h"


typedef struct _Point
{
    int x; //en mm
    int y; //en mm
} Point;

typedef struct _PointD
{
    double x; //en mm
    double y; //en mm
} PointD;

class Model {

    private :
    public:

        Model()=delete;


        // ---CONSTANTES-----------------------------
        static constexpr float PI = 3.14159;
        static constexpr float g = 9.81; // m/(s^2)
        static constexpr float RAIDEUR_ANGULAIRE = 10; // en N.mm/rad  [!] A CHANGER

        // ---FONCTIONS---------------------------




        // --- modele direct approche --- -> suppostion que l'effecteur est ponctuel
        //
        //remplie pos_X et pos_Y en fonction des longeurs des cables [mm] depuis les sorties
        //pos_X et pos_Y depuis l'origine
        //
        // utilsation contient les cables utilises pour le calcul dans l'ordre alphabetique (ex: si A et C : utilisation='AC')
        static void _model_position_direct(double size_cable_1, double size_cable_2, char utilisation[2], double *pos_X, double *pos_Y);


        // --- modele inverse approche --- -> suppostion que l'effecteur est ponctuel
        //
        //remplie la longueur des cables [mm] depuis les sorties en fonction de la position de l'effecteur
        //pos_X et pos_Y depuis l'origine
        static void _model_position_inverse(double pos_X_effecteur, double pos_Y_effecteur, double *size_cable_A , double *size_cable_B, double *size_cable_C);

        //converti la longueur d'un cable [mm] en le nombre correspondant d'increments
        //on prend en compte le rapport de reduction
        //on ne prend pas en compte l'offset
        static double _size_cable_to_increment(double size_cable);


        

        //renvoie l'intersection entre deux cercles la plus proche du point C
        //tous les dimensions sont en mm
        // [!] depracated
        static Point _intersection_cercles_proche_point(Point centreA, double rayonA,  double rayonB, Point centreB, Point pointC);

        static double distance(PointD p1, PointD p2);

        static int intersection_cercles(PointD centre1, double rayon1, PointD centre2, double rayon2, PointD *intersection1, PointD *intersection2);



        // --- Modele statique --- 

        // convention : [ a b     ->  [a b c d]
        //                c d ]

        // origine_cable = [ x_I , y_I , x_II , y_II  , x_III , y_III]
        // position_accroche_effecteur = [ x_I , y_I , x_II , y_II  , x_III , y_III], tous x identiques et tous y identiques si effecteur ponctuel
        // jacobienne = [premier ligne [3 colonnes] , seconde ligne [3 colonnes]]
        static void _init_jacobienne(const double origine_cable[6],const double position_accroche_effecteur[6], double jacobienne[6]);

        // renvoie une matrice 2x2
        static void _multiplication_2x3_par_3x2(const double A[6],const double B[6], double C[4]) ;

        // renvoie un matrice 3x2
        static void _multiplication_3x2_par_2x2(const double A[6],const double B[4], double C[6]) ;

        static void _inverse_2x2(const double A[4] , double B[4]);

        // calcul la transpose de A
        // entree : matrice A -> 2x3
        // sortie : matrice B -> 3x2
        static void _transpose_2x3(const double A[6], double B[6]);

        // calcul la pseudo inverse a gauche de A
        // entree : matrice A -> 2x3
        // sortie : matrice B -> 3x2
        static void _left_pseudo_inverse(const double A[6], double B[6]);

        // calcul un vecteur du ker
        // entree : matrice A -> 2x3
        // sortie : matrice B -> 3x1
        // on suppose que la dernière colonne est non nulle -> si pas le cas renvoie une erreur
        // on suppose qu'il n'y a pas deux cables alignee -> si pas le cas renvoie une erreur
        static void _ker(const double A[6], double B[3]);

        // [!] A TESTER
        // renvoie le vecteur resultat de ( solution_particuliere + Alpha x solution_homogene ) avec Alpha tel que
        // le vecteur resultant a au moins une de ses composantes egale a val_min
        // ceci est possible si V_ker a toutes ses composantes de memes signes
        // -> [!] c'est le cas si l'on se trouve dans le triangle de manipulation
        // entrees : 
        // solution_particuliere -> matrice 3x1, doit minimiser la norme = Jacobienn_inv x F
        // solution_homogene -> matrice 3x1,  issu de _ker()
        // val_min -> valeur minimimum des normes des vecteurs
        // sortie :
        // solution_resultante -> matrice 3x1
        static void _renorm_min(const double solution_particuliere[3],const double solution_homogene[3],double val_min, double solution_resultante[3]);

        // calcul le determinant des vecteurs u et v
        static double det(const double u[2],const  double v[2]);



    public :

        // ---CONSTANTES-----------------------------       


        //position des moteurs (debus de la sortie du cable) depuis l'origine en [mm]
        //moteur I : en haut
        static const int MOTEUR_I_POSITION_X = 399; 
        static const int MOTEUR_I_POSITION_Y = 629; 
        //moteur II : en bas a gauche
        static const int MOTEUR_II_POSITION_X = 10; 
        static const int MOTEUR_II_POSITION_Y = 119; 
        //moteur III : en bas a droite
        static const int MOTEUR_III_POSITION_X = 790; 
        static const int MOTEUR_III_POSITION_Y = 120; 

        static const int ZONE_TRAVAIL_MIN_X = 0; //[!] A CHANGER
        static const int ZONE_TRAVAIL_MAX_X = 800; //[!] A CHANGER

        static const int ZONE_TRAVAIL_MIN_Y = 0; //[!] A CHANGER
        static const int ZONE_TRAVAIL_MAX_Y = 600; //[!] A CHANGER

        static const int RAYON_ROUE = 9; //"[mm]"
        static constexpr double RAPPORT_REDUCTON = (16.0/60.0);//  entree/sortie
        static const int INCREMENTS_PAR_TOUR = 2048;

        //longueur des cables de leur attache du cable jusqu'au dispositif lorsque le moteur a son increment a 0 en [mm]
        //defini la valeur a l'originie de OffsetCableI
        static const int OFFSET_CABLE_I_ORIGINE = 469; 
        static const int OFFSET_CABLE_II_ORIGINE  = 390; 
        static const int OFFSET_CABLE_III_ORIGINE = 390; 

        //longueur des cables de leur attache du cable jusqu'au dispositif lorsque le moteur a son increment a 0 en [mm]
        //A regler grace a setOffsetCable()
        static int OFFSET_CABLE_I; 
        static int OFFSET_CABLE_II; 
        static int OFFSET_CABLE_III; 

        static constexpr double MASSE = 0.300; // kg
        static const bool COMPENSE_POIDS = true;

        static constexpr double TENSION_MINIMUM = 1; // en N : tension minimu mise dans les cables lors de l'appel a force_moteur()

        static constexpr double NOMINAL_TORQUE = 77.5; // mNm (aussi appelle Motor Rated Torque)


        // ---FONCTIONS---------------------------

        static void increment_moteur_from_pos(double pos_X_effecteur, double pos_Y_effecteur,int *increment_moteur_A, int *increment_moteur_B, int *increment_moteur_C);

        // calcul le couple a mettre dans les moteurs (couple_moteur) pour obtenir une force_operationnelle
        // entrees : 
        // force_operationnelle : en N
        // position_effecteur : en mm depuis l'origine
        // sortie :
        // couple_moteur : en N.mm  => /!\ nullptr si pas dans triangle 
        // temps d'execution sur un processeur donnee : 0.4ms (2,5kHz)
        static void couple_moteur_for_force(double const force_operationnelle[2], double const position_effecteur[2] ,double couple_moteur[3]);

        // calcul le couple a mettre dans les moteurs (couple_moteur) en fonction de la vitesse et de la raideur
        // entrees : 
        // position_effecteur : en mm depuis l'origine
        // vitessse_effecteur : en mm/s 
        // raideur : en N/mm
        // viscosite : en kg/s
        // sortie :
        // couple_moteur : en N.mm  => /!\ nullptr si pas dans triangle 
        // temps d'execution sur un processeur donnee : 0.4ms (2,5kHz)
        static void coupleMoteurAsservissemnt(double const position_effecteur[2] ,double const vitesse_effecteur[2], double raideur, double viscosite ,double couple_moteur[3]);


        // renvoie si l'effecteur est dans le triangle forme par les positions des sorties des fils des moteurs
        static bool in_triangle(const double position_effecteur[2]);

        // renvoie le couple assoccie a l'angle des potentiomètres
        // entree : angle en degre
        // sortie : couple en N.mm
        static float coupleFromAngle(double angle);

        //definir longueur des cables de leur attache du cable jusqu'au dispositif lorsque le moteur a son increment a 0 en [mm]
        static void setOffsetCable(double offsetCable[3]);

        // entree force en N
        // target torque (1000=MotorRatedTorque) 
        static double force2targetTorque(double force);

};


#endif