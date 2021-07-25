#ifndef TDA_JUGADOR_H
#define TDA_JUGADOR_H

//OBSERVACION: el presente archivo no es completamente un TDA ya que podria ser de una mayor abstraccion. Se diseño especificamente para la implementacion propuesta de este juego. Es por eso que aparecen variables tales como "naranjas"

#include <stddef.h>
#include "config.h"

typedef struct jugador jugador_t;

//Crea un TDA_jugador con nombre (pide memoria). 
//Precondición: cadena nombre válida. Poscondicion: puntaje inicial 0, cantidad de naranjas iniciales 0, intentos iniciales INTENTOS_INIC
jugador_t * jugador_crear(char * nombre);
void jugador_destruir(jugador_t * jugador);

//Getters. Precondicion: jugador creado correctamente (y sin basura en sus variables)
char * jugador_obtener_nombre(jugador_t * jugador);
size_t jugador_obtener_puntaje(jugador_t * jugador);
int jugador_obtener_intentos(jugador_t * jugador);
size_t jugador_obtener_naranja(jugador_t * jugador);

//Modificadores. Precondicion: jugador creado correctamente
size_t jugador_modificar_puntaje(jugador_t * jugador, size_t delta_puntos);
int jugador_modificar_intentos(jugador_t * jugador, int delta_intentos);
size_t jugador_sumar_naranja(jugador_t * jugador);

//Resets: podrian ponerse todos juntos pero quizas en algun momento se prefiera usar uno solo. Precondicion: jugador creado correctamente
//Resetea a INTENTOS_INIC
void jugador_resetear_intentos(jugador_t * jugador);
//Resetea a 0
void jugador_resetear_puntos(jugador_t * jugador);
//Resetea a 0
void jugador_resetear_naranjas(jugador_t * jugador);

#endif //TDA_JUGADOR_H
