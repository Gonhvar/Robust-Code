	
prog: Robust.o
	g++ -o prog Robust.o -lpcanbasic 
	
	
Robust.o : Robust.cpp Robust.h PCANBasic.h
	g++ -c Robust.cpp
	
	
clean:
	rm -f *.o

vclean: clean
	rm -f prog




