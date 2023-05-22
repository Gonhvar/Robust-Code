#include "InterfaceGraphique.hpp"
#include "Rasberry.hpp"
#include "ControlMoteur.hpp"
#include "modele.hpp"


#include <gtk/gtk.h>
#include <thread>

void runGtkMainTest() {
    gtk_main();
}

int main(int argc, char *argv[])
{
    ControlMoteur controlMoteur;
    Rasberry rasberry;

    InterfaceGraphique interfaceGraphique(&rasberry, &controlMoteur);







    interfaceGraphique.waitEnd();
    controlMoteur.waitEnd();
    rasberry.waitEnd();




    return 0;
}


// //////////////////////////////////////////////////////////////////////////

//   // Initialiser GTK
//   gtk_init(&argc, &argv);

//   // Créer une fenêtre
//   GtkWidget* window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
//   gtk_window_set_title(GTK_WINDOW(window), "Exemple de dessin animé");
//   gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
//   g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

//   // Créer un conteneur GtkBox
//   GtkWidget* box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5); // Orientation verticale, espacement de 5 pixels

//     // Ajouter le GtkDrawingArea à la GtkBox
//     GtkWidget* drawing_area = gtk_drawing_area_new();
//     gtk_box_pack_start(GTK_BOX(box), drawing_area, TRUE, TRUE, 0);

//     // Ajouter le GtkButton à la GtkBox
//       GtkWidget* button = gtk_button_new_with_label("Cliquez-moi");
//     gtk_box_pack_start(GTK_BOX(box), button, FALSE, FALSE, 0);

//     // Ajouter le GtkEntry à la GtkBox
//       GtkWidget* entry = gtk_entry_new();
//     gtk_box_pack_start(GTK_BOX(box), entry, FALSE, FALSE, 0);

//     // Ajouter la GtkBox à la GtkWindow
//     gtk_container_add(GTK_CONTAINER(window), box);



//   // Afficher tous les éléments de la fenêtre
//   gtk_widget_show_all(window);

//   // Démarrer la boucle principale de GTK
//   gtk_main();

// //////////////////////////////////////////////////////////////////////////


