prog: Robust.o Modele.o 
	g++ -o prog Robust.o Modele.o -lpcanbasic -lm

Modele.o : Modele.cpp Modele.hpp
	g++ -Wall Modele.cpp -c 

Robust.o : Robust.cpp Robust.hpp Modele.hpp PCANBasic.h
	g++ -Wall Robust.cpp -c

clean:
	rm -f *.o

vclean: clean
	rm -f prog




