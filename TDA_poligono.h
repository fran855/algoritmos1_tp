#ifndef TDA_POLIGONO_H
#define TDA_POLIGONO_H

#include <SDL2/SDL.h>

#define PI_M 3.14159265358979323846

typedef struct poligono poligono_t;

//Como precondicion, todas las funciones que reciban por parametro poligono_t * asumen que esta creado correctamente

//Precondicion: vertices validos. Poscondicion: devuelve poligono creado
poligono_t *poligono_crear(float vertices[][2], size_t n);
poligono_t *poligono_crear_circulo( float radio, float cx, float cy );
void poligono_destruir(poligono_t *poligono);

//Devuelve false si no puede reservar memoria
bool poligono_agregar_vertice(poligono_t *poligono, float x, float y);
//Reserva memoria y devuelve el nuevo poligono. Devuelve NULL si falla
poligono_t *poligono_clonar(const poligono_t *poligono);
//Precondicion: renderer y rgb validos. Devuelve por nombre el renderer
void poligono_dibujar(poligono_t *cuerpo,SDL_Renderer *Renderer, uint32_t rgb);

bool poligono_obtener_vertice(const poligono_t *poligono, size_t pos, float *x, float *y);
size_t poligono_cantidad_vertices(const poligono_t *poligono);
//da true si el punto esta dentro de la figura
bool punto_en_poligono(poligono_t * pol, float px, float py);
bool punto_en_triangulo(float px, float py, float ax, float ay, float bx, float by, float cx, float cy);

//el angulo debe estar en radianes
void poligono_rotar(float poligono[][2], size_t n, double rad);
void poligono_trasladar(poligono_t *cuerpo, float dx, float dy);
void poligono_rotar_respecto_al_centro(poligono_t *cuerpo,float cx , float cy , float rad);

double poligono_distancia(const poligono_t *p, float xp, float yp, float *nor_x, float *nor_y);

float producto_interno(float ax, float ay, float bx, float by);
void punto_mas_cercano(float x0, float y0, float x1, float y1, float xp, float yp, float *x, float *y);
void reflejar(float norm_x, float norm_y, float *cx, float *cy, float *vx, float *vy);

double distancia(float x1, float y1, float x2, float y2);

//Devuelve todo el arreglo de vertices del poligono
float ** poligono_obtener_todos_vertices(poligono_t * poligono);

#endif //POLIGONO_H
