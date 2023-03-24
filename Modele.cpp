#include "Modele.hpp"



void _model_position_direct(double size_cable_1, double size_cable_2, char utilisation[2], double *pos_X, double *pos_Y) {
    //A IMPLEMENTER
}

void _model_position_inverse(double pos_X_effecteur, double pos_Y_effecteur, double *size_cable_A , double *size_cable_B, double *size_cable_C) {
    *size_cable_A = sqrt(pow((pos_X_effecteur - MOTEUR_A_POSITION_X),2)+pow((pos_Y_effecteur - MOTEUR_A_POSITION_Y),2));
    *size_cable_B = sqrt(pow((pos_X_effecteur - MOTEUR_B_POSITION_X),2)+pow((pos_Y_effecteur - MOTEUR_B_POSITION_Y),2));
    *size_cable_C = sqrt(pow((pos_X_effecteur - MOTEUR_C_POSITION_X),2)+pow((pos_Y_effecteur - MOTEUR_C_POSITION_Y),2));
}


double _size_cable_to_increment(double size_cable) {
    return size_cable*INCREMENTS_PAR_TOUR/(RAPPORT_REDUCTON*2*RAYON_ROUE*PI);

}

void increment_moteur_from_pos(double pos_X_effecteur, double pos_Y_effecteur,int *increment_moteur_A, int *increment_moteur_B, int *increment_moteur_C){
    double size_cable_A;
    double size_cable_B;
    double size_cable_C;
    _model_position_inverse(pos_X_effecteur,pos_Y_effecteur,&size_cable_A,&size_cable_B,&size_cable_C);

    *increment_moteur_A = (int) _size_cable_to_increment(size_cable_A+OFFSET_CABLE_A);
    *increment_moteur_B = (int) _size_cable_to_increment(size_cable_B+OFFSET_CABLE_B);
    *increment_moteur_C = (int) _size_cable_to_increment(size_cable_C+OFFSET_CABLE_C);
}

