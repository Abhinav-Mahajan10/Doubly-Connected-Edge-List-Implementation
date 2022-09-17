assignment3 : DCEL.o 
	g++ DCEL.o -o assignment3

DCEL.o : DCEL.cpp
	g++ -c DCEL.cpp

clean:
	rm *.o assignment3
	
