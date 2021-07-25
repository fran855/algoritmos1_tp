#include <stdlib.h>
#include <string.h>

#include "TDA_jugador.h"

struct jugador{
    char * nombre;
    int intentos;
    size_t puntos;
    size_t cant_naranjas;
};

jugador_t * jugador_crear(char * nombre){
    jugador_t * jugador = malloc(sizeof(jugador_t));
    if(jugador == NULL)
        return NULL;

    size_t longitud = strlen(nombre) + 1;
    char * jugador_nombre = malloc(longitud);
    if(jugador_nombre == NULL){
        free(jugador);
        return NULL;
    }

    memcpy(jugador_nombre, nombre, longitud);
    jugador -> nombre = jugador_nombre;

    jugador -> intentos = INTENTOS_INIC;
    jugador -> puntos = 0;
    jugador -> cant_naranjas = 0;

    return jugador;
}

void jugador_destruir(jugador_t * jugador){
    free(jugador -> nombre);
    free(jugador);
}

char * jugador_obtener_nombre(jugador_t * jugador){
    return jugador -> nombre;
}

size_t jugador_obtener_puntaje(jugador_t * jugador){
    return jugador -> puntos;
}

int jugador_obtener_intentos(jugador_t * jugador){
    return jugador -> intentos;
}

size_t jugador_modificar_puntaje(jugador_t * jugador, size_t delta_puntos){
    if(jugador -> cant_naranjas <= 10)
        jugador -> puntos += delta_puntos;
    else if(jugador -> cant_naranjas > 10 && jugador -> cant_naranjas <= 15)
        jugador -> puntos += 2*delta_puntos;
        else if(jugador -> cant_naranjas > 15 && jugador -> cant_naranjas <= 19)
            jugador -> puntos += 3*delta_puntos;
            else if(jugador -> cant_naranjas > 19 && jugador -> cant_naranjas <= 21)
                jugador -> puntos += 5*delta_puntos;
                else
                    jugador -> puntos += 10*delta_puntos;

    return jugador -> puntos;
}

int jugador_modificar_intentos(jugador_t * jugador, int delta_intentos){
    jugador -> intentos += delta_intentos;
    return jugador -> intentos;
}

size_t jugador_sumar_naranja(jugador_t * jugador){
    return ++(jugador -> cant_naranjas);
}

size_t jugador_obtener_naranja(jugador_t * jugador){
    return jugador -> cant_naranjas;
}

void jugador_resetear_intentos(jugador_t * jugador){
    jugador -> intentos = INTENTOS_INIC;
}

void jugador_resetear_puntos(jugador_t * jugador){
    jugador -> puntos = 0;
}

void jugador_resetear_naranjas(jugador_t * jugador){
    jugador -> cant_naranjas = 0;
}