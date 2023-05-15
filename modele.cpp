#include "modele.hpp"

#include <stdlib.h>



void Model::_model_position_direct(double size_cable_1, double size_cable_2, char utilisation[2], double *pos_X, double *pos_Y) {
    //A IMPLEMENTER
}

void Model::_model_position_inverse(double pos_X_effecteur, double pos_Y_effecteur, double *size_cable_A , double *size_cable_B, double *size_cable_C) {
    *size_cable_A = sqrt(pow((pos_X_effecteur - MOTEUR_I_POSITION_X),2)+pow((pos_Y_effecteur - MOTEUR_I_POSITION_Y),2));
    *size_cable_B = sqrt(pow((pos_X_effecteur - MOTEUR_II_POSITION_X),2)+pow((pos_Y_effecteur - MOTEUR_II_POSITION_Y),2));
    *size_cable_C = sqrt(pow((pos_X_effecteur - MOTEUR_III_POSITION_X),2)+pow((pos_Y_effecteur - MOTEUR_III_POSITION_Y),2));
}


double Model::_size_cable_to_increment(double size_cable) {
    return size_cable*INCREMENTS_PAR_TOUR/(RAPPORT_REDUCTON*2*RAYON_ROUE*PI);


}

void Model::increment_moteur_from_pos(double pos_X_effecteur, double pos_Y_effecteur,double *increment_moteur_A, double *increment_moteur_B, double *increment_moteur_C) {
    double size_cable_A;
    double size_cable_B;
    double size_cable_C;

    _model_position_inverse(pos_X_effecteur,pos_Y_effecteur,&size_cable_A,&size_cable_B,&size_cable_C);

    *increment_moteur_A = _size_cable_to_increment(size_cable_A+OFFSET_CABLE_I);
    *increment_moteur_B = _size_cable_to_increment(size_cable_B+OFFSET_CABLE_II);
    *increment_moteur_C = _size_cable_to_increment(size_cable_C+OFFSET_CABLE_III);
}


Point Model::_intersection_cercles_proche_point(Point centreA, double rayonA,  double rayonB, Point centreB, Point pointC) {
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





double Model::distance(PointD p1, PointD p2) {
    return sqrt(pow((p2.x - p1.x), 2) + pow((p2.y - p1.y), 2));
}

int Model::intersection_cercles(PointD centre1, double rayon1, PointD centre2, double rayon2, PointD *intersection1, PointD *intersection2) {
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


void Model::_init_jacobienne(const double origine_cable[6], const double position_accroche_effecteur[6], double jacobienne[6]) {
    double d1 = sqrt(pow(origine_cable[0]-position_accroche_effecteur[0],2)+pow(origine_cable[1]-position_accroche_effecteur[1],2));
    double u1[2]; u1[0]=origine_cable[0]-position_accroche_effecteur[0]; u1[1]=origine_cable[1]-position_accroche_effecteur[1];

    double d2 = sqrt(pow(origine_cable[2]-position_accroche_effecteur[2],2)+pow(origine_cable[3]-position_accroche_effecteur[3],2));
    double u2[2]; u2[0]=origine_cable[2]-position_accroche_effecteur[2]; u2[1]=origine_cable[3]-position_accroche_effecteur[3];

    double d3 = sqrt(pow(origine_cable[4]-position_accroche_effecteur[4],2)+pow(origine_cable[5]-position_accroche_effecteur[5],2));
    double u3[2]; u3[0]=origine_cable[4]-position_accroche_effecteur[4]; u3[1]=origine_cable[5]-position_accroche_effecteur[5];

    jacobienne[0] = u1[0]/d1; jacobienne[1] = u2[0]/d2; jacobienne[2] = u3[0]/d3;
    jacobienne[3] = u1[1]/d1; jacobienne[4] = u2[1]/d2; jacobienne[5] = u3[1]/d3;
}

void Model::_multiplication_2x3_par_3x2(const double A[6], const double B[6], double C[4]) {
    int nb_colonnes_A = 3;
    int nb_colonnes_B = 2;
    int nb_lignes_C = 2, nb_colonnes_C = 2;
    int parcours = 3;
    double somme;
    for (size_t i = 0; i < nb_lignes_C; i++)
    {
        for (size_t j = 0; j < nb_colonnes_C; j++)
        {
            somme=0;
            for (size_t k = 0; k < parcours; k++)
            {
                somme += A[nb_colonnes_A*i+k] * B[nb_colonnes_B*k+j];
            }
            C[nb_colonnes_C*i+j] = somme;
            
        }
        
    }
     
}

void Model::_multiplication_3x2_par_2x2(const double A[6],const double B[4], double C[6]) {
    int nb_colonnes_A = 2;
    int nb_colonnes_B = 2;
    int nb_lignes_C = 3, nb_colonnes_C = 2;
    int parcours = 2;
    double somme;
    for (size_t i = 0; i < nb_lignes_C; i++)
    {
        for (size_t j = 0; j < nb_colonnes_C; j++)
        {
            somme=0;
            for (size_t k = 0; k < parcours; k++)
            {
                somme += A[nb_colonnes_A*i+k] * B[nb_colonnes_B*k+j];
            }
            C[nb_colonnes_C*i+j] = somme;
            
        }
        
    }
}


void Model::_inverse_2x2(const double A[4] , double B[4]) {
    double det_A = A[0]*A[3]-A[1]*A[2];
    if (det_A==0) {
        printf("_inverse_2x2() -> impossible d'inverser, determinant nul");
        exit(0);
    }
    B[0]=A[3]/det_A;
    B[1]=-A[1]/det_A;
    B[2]=-A[2]/det_A;
    B[3]=A[0]/det_A;

}


void Model::_transpose_2x3(const double A[6], double B[6]) {
    B[0]= A[0]; B[1]=A[3];
    B[2]= A[1]; B[3]=A[4];
    B[4]= A[2]; B[5]=A[5];
}


void Model::_left_pseudo_inverse(const double A[6], double B[6]) {
    double transpose_A[6];
    _transpose_2x3(A,transpose_A);

    for (size_t i = 0; i < 6; i++)
    {
        //printf(" %d : transpose_A %f N\n",i,transpose_A[i]);
    }
    
    double temp[4];
    _multiplication_2x3_par_3x2(A,transpose_A,temp);

    for (size_t i = 0; i < 4; i++)
    {
        //printf(" %d : temp %f N\n",i,temp[i]);
    }

    double inverse[4];
    _inverse_2x2(temp,inverse);

    for (size_t i = 0; i < 4; i++)
    {
        //printf(" %d : inverse %f N\n",i,inverse[i]);
    }

    _multiplication_3x2_par_2x2(transpose_A,inverse,B);
}


void Model::_ker(const double A[6], double B[3]) {
    if (A[2]==0 || A[5]==0) {
        // Erreur du a la conception de l'algo, pas de rapport avec la geometrie
        printf("Erreur : Model::_ker()  -> derniere colonne de A nul\n");
        exit(0);
    }

    double temp1[4]; //matrice 2x2
    temp1[0] = -A[0]/A[2]; temp1[1]=-A[1]/A[2] ;
    temp1[2] = -A[3]/A[5]; temp1[3]=-A[4]/A[5] ;

    double inverse[4]; // matrice 2x2
    _inverse_2x2(temp1,inverse);

    double temp2[9]; // matrice 3x3
    temp2[0]=inverse[0] ; temp2[1]= inverse[1] ; temp2[2]=0;
    temp2[3]=inverse[2] ; temp2[4]= inverse[3] ; temp2[5]=0;
    temp2[6]=0          ; temp2[7]= 0          ; temp2[8]=1;

    // B = temp x [1 1 1].T
    B[0] =  temp2[0]+temp2[1]+temp2[2];
    B[1] =  temp2[3]+temp2[4]+temp2[5];
    B[2] =  temp2[6]+temp2[7]+temp2[8];


}


void Model::_renorm_min(const double solution_particuliere[3],const  double solution_homogene[3],double val_min, double solution_resultante[3]) {
    int indice_minimum = 0; // indice de la composante la plus petite de solution_particuliere
    for (size_t i = 1; i < 3; i++)
    {
        if (solution_particuliere[i]<solution_particuliere[indice_minimum]) {
            indice_minimum=i;
        }
    }

    // solution_particuliere[indice_minimum] negatif car solution_particuliere minimise la norme
    // implique aussi alpha positif ?
    double alpha = (val_min-solution_particuliere[indice_minimum])/solution_homogene[indice_minimum];
    if (alpha<0) {
        printf("Erreur : Model::_renorm_min() -> alpha < 0 => hypothese mathematique fausse\n");
        //exit(0);
    }

    for (size_t i = 0; i < 3; i++)
    {
        solution_resultante[i] = solution_particuliere[i]+alpha*solution_homogene[i];
        //printf("solution_resultante %f \n",solution_resultante[i]);
    }   
}


double  Model::det(const double u[2],const  double v[2]) {
    return u[0]*v[1] - v[0]*u[1];
}

bool Model::in_triangle(const double position_effecteur[2]) {

    double v0[2];
    v0[0]=MOTEUR_I_POSITION_X; 
    v0[1]=MOTEUR_I_POSITION_Y;

    double v1[2];
    v1[0]=MOTEUR_II_POSITION_X-MOTEUR_I_POSITION_X;
    v1[1]=MOTEUR_II_POSITION_Y-MOTEUR_I_POSITION_Y;

    double v2[2];
    v2[0]=MOTEUR_III_POSITION_X-MOTEUR_I_POSITION_X;
    v2[1]=MOTEUR_III_POSITION_Y-MOTEUR_I_POSITION_Y;

    if (det(v1,v2)==0 || det(v1,v2)==0) {
        printf("Error : in_triangle() -> sur le triangle => impossible a determiner\n");
        exit(1);
    }

    double a = (det(position_effecteur,v2)-det(v0,v2))/det(v1,v2);
    double b = -(det(position_effecteur,v1)-det(v0,v1))/det(v1,v2);

    return a>0 && b>0 && a+b<1;
}


void Model::couple_moteur(double const force_operationnelle[2], double const position_effecteur[2] ,double couple_moteur[3]) {


    //printf("force_operationnelle x : %f | y : %f",force_operationnelle[0],force_operationnelle[1]);

    double force_operationnelle_compensee[2];  // compensation de la gravite
    force_operationnelle_compensee[0] = force_operationnelle[0];
    force_operationnelle_compensee[1] = force_operationnelle[1];

    if (!in_triangle(position_effecteur)) {
        couple_moteur=nullptr;
        return;
    }

    if (COMPENSE_POIDS) {
        force_operationnelle_compensee[1]+=MASSE*g;
    }




    double origine_cable[6];
    origine_cable[0] = MOTEUR_I_POSITION_X;
    origine_cable[1] = MOTEUR_I_POSITION_Y;
    origine_cable[2] = MOTEUR_II_POSITION_X;
    origine_cable[3] = MOTEUR_II_POSITION_Y;
    origine_cable[4] = MOTEUR_III_POSITION_X;
    origine_cable[5] = MOTEUR_III_POSITION_Y;

    // on fait l'hypothese que l'effecteur est ponctuel
    double position_accroche_effecteur[6];
    position_accroche_effecteur[0]=position_effecteur[0];
    position_accroche_effecteur[1]=position_effecteur[1];
    position_accroche_effecteur[2]=position_effecteur[0];
    position_accroche_effecteur[3]=position_effecteur[1];
    position_accroche_effecteur[4]=position_effecteur[0];
    position_accroche_effecteur[5]=position_effecteur[1];

    double jacobienne[6]; // matrice 2x3
    Model::_init_jacobienne(origine_cable, position_accroche_effecteur, jacobienne);


    // for (size_t i = 0; i < 6; i++)
    // {
    //     printf(" %d : jacobienne %f N\n",i,jacobienne[i]);
    // }


    double jacobienne_pseudo_inverse[6]; // matrice 3x2
    Model::_left_pseudo_inverse(jacobienne,jacobienne_pseudo_inverse);

    // for (size_t i = 0; i < 6; i++)
    // {
    //     printf(" %d : jacobienne_pseudo_inverse %f N\n",i,jacobienne_pseudo_inverse[i]);
    // }

    // for (int i = 0; i < 2; i++)
    // {
    //     printf("%d force_operationnelle_compensee %f\n",i,force_operationnelle_compensee[i]);
    // }

    double solution_particuliere[3]; // matrice 3x1 (vecteur de dimension 3)
    // solution_particuliere = jacobienne_pseudo_inverse x force_operationnelle_compensee
    solution_particuliere[0] = jacobienne_pseudo_inverse[0]*force_operationnelle_compensee[0] + jacobienne_pseudo_inverse[1]*force_operationnelle_compensee[1];
    solution_particuliere[1] = jacobienne_pseudo_inverse[2]*force_operationnelle_compensee[0] + jacobienne_pseudo_inverse[3]*force_operationnelle_compensee[1];
    solution_particuliere[2] = jacobienne_pseudo_inverse[4]*force_operationnelle_compensee[0] + jacobienne_pseudo_inverse[5]*force_operationnelle_compensee[1];

    double solution_homogene[3]; // matrice 3x1 (vecteur de dimension 3)
    _ker(jacobienne,solution_homogene);

    // for (size_t i = 0; i < 3; i++)
    // {
    //     printf(" %d : solution_homogene %f N\n",i,solution_homogene[i]);
    // }
    // for (size_t i = 0; i < 3; i++)
    // {
    //     printf(" %d : solution_particuliere %f N\n",i,solution_particuliere[i]);
    // }

    double force_cable[3];
    _renorm_min(solution_particuliere,solution_homogene,TENSION_MINIMUM,force_cable);

    // for (size_t i = 0; i < 3; i++)
    // {
    //     printf("force_cable %d : force %f N\n",i,force_cable[i]);
    // } printf("\n");


    for (size_t i = 0; i < 3; i++)
    {
        couple_moteur[i] = force_cable[i]*RAYON_ROUE*RAPPORT_REDUCTON;
        
        //printf("couple_moteur %d :  %f N.mm\n",i,couple_moteur[i]);
    }
    
    
}

