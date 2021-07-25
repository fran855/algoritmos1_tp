FLAGS = -Wall -pedantic -std=c99 -lm -lSDL2 -lSDL2_ttf -DTTF
CFLAG = -c
CC = gcc

peggle: main.o TDA_poligono.o niveles.o TDA_lista.o TDA_obstaculo.o TDA_jugador.o
	$(CC) main.o TDA_poligono.o niveles.o TDA_lista.o TDA_obstaculo.o TDA_jugador.o -o peggle $(FLAGS)
	
main.o: TDA_poligono.c config.h TDA_poligono.h niveles.c niveles.h TDA_lista.c TDA_lista.h TDA_jugador.c TDA_jugador.h
	$(CC) main.c $(FLAGS) $(CFLAG)

TDA_poligono.o: TDA_poligono.h 
	$(CC) TDA_poligono.c $(FLAGS) $(CFLAG)

TDA_obstaculo.o: TDA_obstaculo.h TDA_poligono.h TDA_poligono.c TDA_jugador.c TDA_jugador.h
	$(CC) TDA_obstaculo.c $(FLAGS) $(CFLAG)

niveles.o: niveles.h TDA_poligono.h TDA_poligono.c
	$(CC) niveles.c $(FLAGS) $(CFLAG)

TDA_lista.o: TDA_lista.h
	$(CC) TDA_lista.c $(FLAGS) $(CFLAG)

TDA_jugador.o: TDA_jugador.h
	$(CC) TDA_jugador.c $(FLAGS) $(CFLAG)

clean:
	rm *.o