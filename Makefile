GTK_HEADER = -I/usr/include/gtk-3.0  
GLIB_HEADER = -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include
PANGO_HEADER = -pthread -I/usr/include/pango-1.0 -I/usr/include/libmount -I/usr/include/blkid -I/usr/include/fribidi -I/usr/include/cairo -I/usr/include/pixman-1 -I/usr/include/uuid -I/usr/include/harfbuzz -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/freetype2 -I/usr/include/libpng16
PIXBUF_HEADER = -pthread -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/libpng16 -I/usr/include/x86_64-linux-gnu -I/usr/include/libmount -I/usr/include/blkid -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include
ATK_HEADER = -I/usr/include/atk-1.0 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include





prog: InterfaceGraphique.o Main.o modele.o Rasberry.o ControlMoteur.o 
	g++ -o prog Main.o InterfaceGraphique.o modele.o Rasberry.o ControlMoteur.o -lpcanbasic -lm `pkg-config --cflags --libs gtk+-3.0`

modele.o : modele.cpp modele.hpp 
	g++ -Wall modele.cpp -c

Rasberry.o : Rasberry.cpp Rasberry.hpp 
	g++ -Wall Rasberry.cpp -c

ControlMoteur.o : ControlMoteur.cpp ControlMoteur.hpp modele.hpp PCANBasic.h
	g++ -Wall ControlMoteur.cpp -c

InterfaceGraphique.o : InterfaceGraphique.cpp InterfaceGraphique.hpp
	g++ -Wall InterfaceGraphique.cpp -c $(GTK_HEADER)  $(GLIB_HEADER) $(PANGO_HEADER) $(PIXBUF_HEADER) $(ATK_HEADER)

Main.o : Main.cpp 
	g++ -Wall Main.cpp -c $(GTK_HEADER)  $(GLIB_HEADER) $(PANGO_HEADER) $(PIXBUF_HEADER) $(ATK_HEADER)


clean:
	rm -f *.o

vclean: clean
	rm -f prog && rm -f pcanbasic_dbg.log




# INSTALLATIONS NEEDED :

#sudo apt-get install libgtk-3-dev

#sudo apt-get install libglib2.0-dev

#sudo apt-get install libpango1.0-dev

#sudo apt-get install libatk1.0-dev

#sudo apt-get install libgdk-pixbuf2.0-dev
