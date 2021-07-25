#ifndef TDA_OBSTACULO_H
#define TDA_OBSTACULO_H

#include <stdbool.h>
#include "TDA_poligono.h"
#include "TDA_jugador.h"

typedef struct obstaculo obstaculo_t;

//ISO-C prohibe reenviar referencias a enum (no se puede desdoblar como las estructuras)
typedef enum color {AZUL, NARANJA, VERDE, GRIS, AMARILLO} color_t;
typedef enum movimiento {MOV_INMOVIL, MOV_CIRCULAR , MOV_HORIZONTAL} movimiento_t;
typedef enum geometria {GEO_CIRCULO, GEO_RECTANGULO , GEO_POLIGONO} geometria_t;

//El obstaculo se crea vacio y, por defecto, marcado es falso
obstaculo_t * obstaculo_crear(poligono_t * cuerpo, color_t color, movimiento_t movimiento, geometria_t geometria, int16_t * parametros_movimiento);
void _obstaculo_destruir(obstaculo_t *obstaculo);
//Wrapper
void obstaculo_destruir(void *obstaculo);


//Precondicion: obstaculo creado correctamente

void obstaculo_marcar(obstaculo_t *);
bool obstaculo_esta_marcado(obstaculo_t *);

void obstaculo_dibujar(obstaculo_t *obs, SDL_Renderer *renderer);
void obstaculo_mover(obstaculo_t * obstaculo);
void obstaculo_ejecutar_modificaciones(obstaculo_t * obstaculo, jugador_t * jugador);

//Clona el obstaculo. Realiza la reserva de memoria para el mismo
void * obstaculo_clonar(void* obstaculo);

poligono_t * obstaculo_obtener_cuerpo(obstaculo_t * obs);
color_t obstaculo_obtener_color(obstaculo_t * obastaculo);

#endif //TDA_OBSTACULO_H
