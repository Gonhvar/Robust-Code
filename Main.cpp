#include "InterfaceGraphique.hpp"
#include "Rasberry.hpp"
#include "ControlMoteur.hpp"
#include "modele.hpp"


#include <gtk/gtk.h>
#include <thread>

// // Callback function for the button
// void on_button_clicked(GtkWidget *widget, gpointer data)
// {
//     g_print("Button clicked!\n");
// }

// Structure pour stocker les coordonnées d'un dessin animé
struct AnimatedShape {
  int x, y; // Coordonnées du dessin
  int dx, dy; // Vitesse de déplacement
  int width, height; // Dimensions du dessin
  GtkWidget* drawing_area; //endroit ou va etre mit le dessin

};

// Fonction de dessin pour un dessin animé
gboolean draw_animated_shape(GtkWidget* widget, cairo_t* cr, gpointer data) {
  AnimatedShape* shape = static_cast<AnimatedShape*>(data);

  // Effacer la zone de dessin
  cairo_set_source_rgb(cr, 1.0, 1.0, 1.0); // Couleur blanche
  cairo_paint(cr);

  // Dessiner un rectangle rouge à la position actuelle du dessin
  cairo_set_source_rgb(cr, 1.0, 0.0, 0.0); // Couleur rouge
  cairo_rectangle(cr, shape->x, shape->y, shape->width, shape->height);
  cairo_fill(cr);
  return FALSE;
      
}

// Fonction de mise à jour pour le dessin animé
gboolean update_animated_shape(gpointer data) {
  AnimatedShape* shape = static_cast<AnimatedShape*>(data);


  // Mettre à jour les coordonnées en fonction de la vitesse
  shape->x += shape->dx;
  shape->y += shape->dy;

  // Obtenir la zone de dessin
  GtkWidget* drawing_area = GTK_WIDGET(shape->drawing_area);
  // Redessiner la zone de dessin
  gtk_widget_queue_draw(drawing_area);


  // Continuer l'animation
  return G_SOURCE_CONTINUE;

}

int main(int argc, char** argv) {
  // Initialiser GTK
  gtk_init(&argc, &argv);

  // Créer une fenêtre
  GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(window), "Exemple de dessin animé");
  gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
  g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

  // Créer un conteneur GtkBox
  GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5); // Orientation verticale, espacement de 5 pixels

    // Ajouter le GtkDrawingArea à la GtkBox
    GtkWidget* drawing_area = gtk_drawing_area_new();
    gtk_box_pack_start(GTK_BOX(box), drawing_area, TRUE, TRUE, 0);

    // Ajouter le GtkButton à la GtkBox
      GtkWidget* button = gtk_button_new_with_label("Cliquez-moi");
    gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);

    // Ajouter le GtkEntry à la GtkBox
      GtkWidget* entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(box), entry, FALSE, FALSE, 0);

    // Ajouter la GtkBox à la GtkWindow
    gtk_container_add(GTK_CONTAINER(window), box);


  // Créer une structure de dessin animé
  AnimatedShape shape;
  shape.x = 50;
  shape.y = 50;
  shape.dx = 2;
  shape.dy = 2;
  shape.width = 50;
  shape.height = 50;
  shape.drawing_area = drawing_area;


  // Connecter la fonction de dessin à la zone de dessin
  g_signal_connect(drawing_area, "draw", G_CALLBACK(draw_animated_shape), &shape);

  // Créer une minuterie pour mettre à jour l'animation
  g_timeout_add(30, update_animated_shape, &shape);

  // Afficher tous les éléments de la fenêtre
  gtk_widget_show_all(window);

  gtk_widget_destroy(entry);
  

  // Démarrer la boucle principale de GTK
  gtk_main();

  return 0;
}

// int main(int argc, char *argv[])
// {


//     InterfaceGraphique interfaceGraphique(argc,argv);
//     ControlMoteur controlMoteur;
//     Rasberry rasberry;




//     printf("hey!\n");


//     interfaceGraphique.waitEnd();
//     /controlMoteur.waitEnd();
//     rasberry.waitEnd();








//     // // Initialize GTK
//     // gtk_init(&argc, &argv);

//     // // Create the main window
//     // GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//     // gtk_window_set_title(GTK_WINDOW(window), "GTK Window");
//     // gtk_window_set_default_size(GTK_WINDOW(window), 200, 200);
//     // g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

//     // // Create a button
//     // GtkWidget *button = gtk_button_new_with_label("Click Me");
//     // g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);
//     // gtk_container_add(GTK_CONTAINER(window), button);

//     // // Show all the widgets
//     // gtk_widget_show_all(window);

//     // // Start the GTK main loop
//     // gtk_main();

//     return 0;
// }



// Oui, avec GTK, il est possible de faire des callbacks vers des méthodes de classe. GTK utilise le langage C, qui ne prend pas directement en charge les méthodes de classe comme dans les langages orientés objet tels que Java ou C++. Cependant, GTK fournit des mécanismes pour lier des callbacks à des méthodes de classe à l'aide de données utilisateur (user data).

// Voici comment vous pouvez réaliser cela :

// 1. Définir une fonction de callback statique : Dans votre classe, définissez une fonction de callback statique qui sera utilisée comme point d'entrée pour le callback GTK. Cette fonction statique peut ensuite appeler une méthode de classe appropriée.

// ```c
// static void myCallback(GtkWidget *widget, gpointer data) {
//     MyClass *myInstance = (MyClass *)data;
//     myInstance->myMethod();
// }
// ```

// 2. Lier la fonction de callback à la méthode de classe : Lorsque vous créez l'interface GTK et que vous liez le callback à un événement spécifique, vous pouvez passer une instance de votre classe comme données utilisateur (user data). Vous pouvez utiliser la fonction `g_signal_connect_data` pour cela.

// ```c
// MyClass myObject;
// g_signal_connect_data(widget, "event-name", G_CALLBACK(myCallback), &myObject, NULL, G_CONNECT_SWAPPED);
// ```

// Dans cet exemple, `widget` est le widget GTK auquel vous souhaitez lier l'événement, "event-name" est le nom de l'événement, `myCallback` est la fonction de callback statique, `&myObject` est un pointeur vers l'instance de la classe que vous souhaitez utiliser, et les derniers arguments sont généralement `NULL` ou utilisés pour la gestion de la mémoire.

// Lorsque l'événement se produit, la fonction de callback statique sera appelée avec le widget et les données utilisateur. À l'intérieur de cette fonction, vous pouvez récupérer l'instance de la classe à partir des données utilisateur et appeler la méthode de classe appropriée.

// Cependant, il est important de noter que cette approche nécessite une attention particulière à la gestion de la durée de vie des objets et à la gestion de la mémoire, car GTK ne fournit pas de mécanismes natifs pour suivre ou libérer les ressources associées aux objets liés. Vous devrez vous assurer que les objets restent valides aussi longtemps que les événements GTK sont liés à leurs callbacks correspondants.