#ifndef MODELE
#define MODELE

#include "math.h"
#include "stdio.h"

// ---CONSTANTES-----------------------------

//position des moteurs depuis l'origine en [mm]
//moteur A : en haut
#define MOTEUR_A_POSITION_X 400 //[!] A CHANGER
#define MOTEUR_A_POSITION_Y 600 //[!] A CHANGER
//moteur B : en bas a gauche
#define MOTEUR_B_POSITION_X 0 //[!] A CHANGER
#define MOTEUR_B_POSITION_Y 0 //[!] A CHANGER
//moteur C : en bas a droite
#define MOTEUR_C_POSITION_X 800 //[!] A CHANGER
#define MOTEUR_C_POSITION_Y 0 //[!] A CHANGER

#define RAYON_ROUE 9 //"[mm]"
#define RAPPORT_REDUCTON (16.0/60.0)//  entree/sortie
#define INCREMENTS_PAR_TOUR 512

#define PI 3.14159

//longueur des cables de leur attache du cable jusqu'au dispositif lorsque le moteur a son increment a 0 en [mm]
//A regler grace a une fonction specifique
#define OFFSET_CABLE_A 100 //[!] A CHANGER
#define OFFSET_CABLE_B 100 //[!] A CHANGER
#define OFFSET_CABLE_C 100 //[!] A CHANGER

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



// ---FONCTIONS---------------------------


//modele direct approche -> suppostion que l'effecteur est ponctuel
//
//remplie pos_X et pos_Y en fonction des longeurs des cables [mm] depuis les sorties
//pos_X et pos_Y depuis l'origine
//
// utilsation contient les cables utilises pour le calcul dans l'ordre alphabetique (ex: si A et C : utilisation='AC')
void _model_position_direct(double size_cable_1, double size_cable_2, char utilisation[2], double *pos_X, double *pos_Y);


//modele inverse approche -> suppostion que l'effecteur est ponctuel
//
//remplie la longueur des cables [mm] depuis les sorties en fonction de la position de l'effecteur
//pos_X et pos_Y depuis l'origine
void _model_position_inverse(double pos_X_effecteur, double pos_Y_effecteur, double *size_cable_A , double *size_cable_B, double *size_cable_C);

//converti la longueur d'un cable [mm] en le nombre correspondant d'increments
//on prend en compte le rapport de reduction
//on ne prend pas en compte l'offset
double _size_cable_to_increment(double size_cable);


void increment_moteur_from_pos(double pos_X_effecteur, double pos_Y_effecteur,double *increment_moteur_A, double *increment_moteur_B, double *increment_moteur_C);

//renvoie l'intersection entre deux cercles la plus proche du point C
//tous les dimensions sont en mm
// [!] depracated
Point _intersection_cercles_proche_point(Point centreA, double rayonA,  double rayonB, Point centreB, Point pointC);

double distance(PointD p1, PointD p2);

int intersection_cercles(PointD centre1, double rayon1, PointD centre2, double rayon2, PointD *intersection1, PointD *intersection2);


#endif