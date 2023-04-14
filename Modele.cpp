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

void increment_moteur_from_pos(double pos_X_effecteur, double pos_Y_effecteur,double *increment_moteur_A, double *increment_moteur_B, double *increment_moteur_C) {
    double size_cable_A;
    double size_cable_B;
    double size_cable_C;

    _model_position_inverse(pos_X_effecteur,pos_Y_effecteur,&size_cable_A,&size_cable_B,&size_cable_C);

    *increment_moteur_A = _size_cable_to_increment(size_cable_A+OFFSET_CABLE_A);
    *increment_moteur_B = _size_cable_to_increment(size_cable_B+OFFSET_CABLE_B);
    *increment_moteur_C = _size_cable_to_increment(size_cable_C+OFFSET_CABLE_C);
}


Point _intersection_cercles_proche_point(Point centreA, double rayonA,  double rayonB, Point centreB, Point pointC) {
    // source : https://members.loria.fr/DRoegel/loc/note0001.pdf
    double a = 2*(centreB.x-centreA.x);
    double b = 2*(centreB.y-centreA.y);
    printf("a %f\n",a);
    printf("b %f\n",b);
    double c = pow((centreB.x-centreA.x),2)+pow((centreB.y-centreA.y),2)-pow(rayonB,2)+pow(rayonA,2);
    printf("c %f\n",c);
    double delta = pow((2*a*c),2)-4*(a*a+b*b)*(c*c-b*b*rayonA*rayonA);
    printf("delta %f\n",delta);
    double x_p = centreA.x + (2*a*c-sqrt(delta))/(2*(a*a+b*b));
    double x_q = centreA.x + (2*a*c+sqrt(delta))/(2*(a*a+b*b));
    double y_p, y_q;
    if (b !=0 ) {
        y_p = centreA.y + (c-a*(x_p-centreA.x))/b;
        y_q = centreA.y + (c+a*(x_q-centreA.x))/b;
    } 
    else { //b==0
        y_p = centreA.y + b/2 + sqrt(rayonB*rayonB-pow((2*c-a*a)/(2*a),2));
        y_q = centreA.y + b/2 - sqrt(rayonB*rayonB-pow((2*c-a*a)/(2*a),2));
    }

    printf("xQ : %f , yQ : %f \n",x_q,y_q);
    printf("xP : %f , yP : %f \n",x_p,y_p);

    Point p;
    return p;

    
}





double distance(PointD p1, PointD p2) {
    return sqrt(pow((p2.x - p1.x), 2) + pow((p2.y - p1.y), 2));
}

int intersection_cercles(PointD centre1, double rayon1, PointD centre2, double rayon2, PointD *intersection1, PointD *intersection2) {
    double d = distance(centre1, centre2);
    
    if (d > (rayon1 + rayon2) || d < fabs(rayon1 - rayon2)) {
        return 0; // pas d'intersection
    }

    double a = (pow(rayon1, 2) - pow(rayon2, 2) + pow(d, 2)) / (2 * d);
    double h = sqrt(pow(rayon1, 2) - pow(a, 2));

    PointD p2;
    p2.x = centre1.x + (a * (centre2.x - centre1.x)) / d;
    p2.y = centre1.y + (a * (centre2.y - centre1.y)) / d;

    if (d == rayon1 + rayon2 || d == fabs(rayon1 - rayon2)) {
        *intersection1 = p2;
        return 1; // une seule intersection
    }

    double x3 = p2.x + (h * (centre2.y - centre1.y)) / d;
    double y3 = p2.y - (h * (centre2.x - centre1.x)) / d;

    double x4 = p2.x - (h * (centre2.y - centre1.y)) / d;
    double y4 = p2.y + (h * (centre2.x - centre1.x)) / d;

    PointD p3 = {x3, y3};
    PointD p4 = {x4, y4};

    *intersection1 = p3;
    *intersection2 = p4;

    return 2; // deux intersections
}
