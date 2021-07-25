#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#include "niveles.h"

#define MASK_COLOR 0xC0
#define SHIFT_COLOR 6
#define MASK_MOV 0X30
#define SHIFT_MOV 4
#define MASK_GEOMETRIA 0X0F
#define SHIFT_GEOMETRIA 0
#define MAX_PARAMETROS_CIRCULAR 3
#define MAX_PARAMETROS_HORIZONTAL 3
#define MAX_PARAMETROS_CIRCULO 3
#define MAX_PARAMETROS_RECTANGULO 5


double computar_velocidad(double vi, double a, double dt){
    return dt * a + vi;
}

double computar_posicion(double pi, double vi, double dt){
    return dt * vi + pi;
}

bool (*movimiento_leer[])(FILE *f, int16_t parametros[], size_t *n_patrametros)={
    leer_movimiento_inmovil,
    leer_movimiento_circular,
    leer_movimiento_horizontal
};

poligono_t *(*f_geometria_leer[])(FILE *f)={
    leer_geometria_circulo,
    leer_geometria_rectangulo,
    leer_geometria_poligono
};

bool leer_encabezado(FILE *f, color_t *color, movimiento_t *movimiento, geometria_t *geometria){
    int8_t registro;
    if(fread(&registro, sizeof(int8_t), 1, f) != 1)
        return false;

    *color = (registro & MASK_COLOR) >> SHIFT_COLOR;
    *movimiento = (registro & MASK_MOV) >> SHIFT_MOV;
    *geometria = (registro & MASK_GEOMETRIA) >> SHIFT_GEOMETRIA;

    return true;
}

bool leer_movimiento_inmovil(FILE *f, int16_t parametros[], size_t *n_parametros){
    *n_parametros = 0;
    return true;
}

bool leer_movimiento_circular(FILE *f, int16_t parametros[], size_t *n_parametros){

    if(fread(parametros , sizeof(int16_t) , MAX_PARAMETROS_CIRCULAR , f) != MAX_PARAMETROS_CIRCULAR){
        free(parametros);
        return false;
    }

    *n_parametros = MAX_PARAMETROS_CIRCULAR;

    return true;
}

bool leer_movimiento_horizontal(FILE *f, int16_t parametros[], size_t *n_parametros){

    if(fread(parametros, sizeof(int16_t), MAX_PARAMETROS_HORIZONTAL, f) != MAX_PARAMETROS_HORIZONTAL){
        free(parametros);
        return false;
    }

    *n_parametros = MAX_PARAMETROS_HORIZONTAL;

    return true;
}

bool leer_movimiento(FILE *f, movimiento_t movimiento, int16_t parametros[], size_t *n_parametros){

    return movimiento_leer[movimiento](f, parametros, n_parametros);
}

poligono_t *leer_geometria_circulo(FILE *f){
    poligono_t * poligono;
    int16_t parametros_circulo[MAX_PARAMETROS_CIRCULO];

    if(fread(parametros_circulo, sizeof(int16_t), MAX_PARAMETROS_CIRCULO, f) != MAX_PARAMETROS_CIRCULO)
        return NULL;

    poligono = poligono_crear_circulo(parametros_circulo[2], parametros_circulo[0], parametros_circulo[1]);
    if(poligono == NULL)
        return NULL;

    return poligono;
}

poligono_t *leer_geometria_rectangulo(FILE *f){
    poligono_t *poligono;
    int16_t parametros_rectangulo[MAX_PARAMETROS_RECTANGULO];

    if(fread(parametros_rectangulo, sizeof(int16_t), MAX_PARAMETROS_RECTANGULO, f) != MAX_PARAMETROS_RECTANGULO)
        return NULL;

    float cx, cy, ancho, alto, radianes;
    cx = parametros_rectangulo[0];
    cy = parametros_rectangulo[1];
    ancho = parametros_rectangulo[2];
    alto = parametros_rectangulo[3];
    radianes = parametros_rectangulo[4] * PI_M/180;

    float vertices[][2] = { {cx - ancho/2, cy - alto/2},
                            {cx + ancho/2, cy - alto/2},
                            {cx + ancho/2, cy + alto/2},
                            {cx - ancho/2, cy + alto/2}
                          };

    poligono = poligono_crear(vertices, 4);
    if(poligono == NULL)
        return NULL;

    poligono_rotar_respecto_al_centro(poligono, cx, cy, radianes);

    return poligono;

}

poligono_t *leer_geometria_poligono(FILE *f){
    int16_t n_puntos;
    poligono_t *poligono;

    if(fread(&n_puntos, sizeof(int16_t), 1, f) != 1)
        return NULL;

    poligono = poligono_crear(NULL, 0);

    int16_t par[2];
    for (size_t i = 0; i < n_puntos; i++){
        if(fread(&par, sizeof(int16_t), 2, f) != 2)
            return NULL;

        poligono_agregar_vertice(poligono, par[0], par[1]);
    }

    return poligono;
}

poligono_t *leer_geometria(FILE *f, geometria_t geometria){

    return f_geometria_leer[geometria](f);
}

uint16_t leer_cantidad_obstaculos(FILE *f){
    uint16_t cantidad = 0;
    fread(&cantidad, sizeof(uint16_t), 1, f);

    return cantidad;
}

float modulo(float x){
    if(x < 0)
        return -x;

    return x;
}

void traducir_fecha(char * buffer, struct tm * fecha){

    sprintf(buffer, "%d/%d/%d a las %d:%d:%d", fecha -> tm_mday, fecha -> tm_mon + 1, fecha -> tm_year + 1900, fecha -> tm_hour, fecha -> tm_min, fecha -> tm_sec);

}