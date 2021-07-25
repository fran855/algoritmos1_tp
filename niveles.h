#ifndef NIVELES_H
#define NIVELES_H

#include "TDA_poligono.h"
#include "TDA_obstaculo.h"

double computar_velocidad(double vi, double a, double dt);
double computar_posicion(double pi, double vi, double dt);

//Pre-condicion: archivo file abierto correctamente.
//Pos-condicion: devuelve true por valor si leyo correctamente y las variables guardadas por nombre
bool leer_encabezado(FILE *f, color_t *color, movimiento_t *movimiento, geometria_t *geometria);

//Precondicion: archivo file abierto correctamente
//Pos-condicion: devuelve true por valor si leyo correctamente, los parametros del movimiento y cantidad por nombre
bool leer_movimiento_inmovil(FILE *f, int16_t parametros[], size_t *n_parametros);
bool leer_movimiento_circular(FILE *f, int16_t parametros[], size_t *n_parametros);
bool leer_movimiento_horizontal(FILE *f, int16_t parametros[], size_t *n_parametros);
bool leer_movimiento(FILE *f, movimiento_t movimiento, int16_t parametros[], size_t *n_parametros);

//Pre-condicion: archivo file abierto correctamente
//Pos-condicion: si leyo bien y no hubo problemas de memoria, devuelve por valor el poligono creado. Si falla, devuelve NULL
poligono_t *leer_geometria_circulo(FILE *f);
poligono_t *leer_geometria_rectangulo(FILE *f);
poligono_t *leer_geometria_poligono(FILE *f);
poligono_t *leer_geometria(FILE*f, geometria_t geometria);

//Pre-condicion: archivo file abierto correctamente, cantidad de obstaculos distinta de cero
//Pos-condicion: devuelve la cantidad de obstaculos. Si falla, devuelve 0.
uint16_t leer_cantidad_obstaculos(FILE *f);

float modulo(float x);


//Pre-condicion: buffer de largo suficiente
//Pos-condicion: devuelve en buffer la cadena con fecha y hora (en español)
void traducir_fecha(char * buffer, struct tm * fecha);

#endif