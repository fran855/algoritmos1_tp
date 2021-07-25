#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <math.h>

#include "TDA_poligono.h"

#define DELTA_RAD 15*PI_M/180
#define DEFINICION_CIRCULO 28

#define RED_MASK 0xFF0000
#define GREEN_MASK 0xFF00
#define BLUE_MASK 0xFF
#define RED_SHIFT 16
#define GREEN_SHIFT 8
#define BLUE_SHIFT 0

struct poligono{
    float (*vertices)[2];
    size_t n;
};

// No es parte del .h
static void descomponer_colores(uint32_t rgb, uint8_t *red, uint8_t *green, uint8_t *blue){
    *blue = (rgb & BLUE_MASK) >> BLUE_SHIFT;
    *green = (rgb & GREEN_MASK) >> GREEN_SHIFT;
    *red = (rgb & RED_MASK) >> RED_SHIFT;
}

//pide la memoria al heap utilizada para guardar los datos de un poligono y te devuelve una putero a la estuctura poligono_t
poligono_t *poligono_crear(float vertices[][2], size_t n){
    poligono_t *poligono_creado = malloc(sizeof(poligono_t));
    if (poligono_creado == NULL)
        return NULL;

    if(n == 0){
        vertices = NULL;
        poligono_creado -> n = 0;
    }

    poligono_creado -> vertices = malloc(sizeof(float[2]) * n);
    if (poligono_creado -> vertices == NULL) {
        free(poligono_creado);
        return NULL;
    }

    for (size_t i = 0; i < n; i++) {
        poligono_creado -> vertices[i][0] = vertices[i][0];
        poligono_creado -> vertices[i][1] = vertices[i][1];
    }
    poligono_creado -> n = n;

    return poligono_creado;
}

//libera la memoria de heap usada para guardar los datos de un poligono. Como free, poligono_destruir(NULL) no hace nada
void poligono_destruir(poligono_t *poligono){
    if(poligono == NULL)
        return;
    
    free(poligono -> vertices);
    free(poligono);
}

size_t poligono_cantidad_vertices(const poligono_t *poligono){
    return poligono -> n;
}
//si es posible se devuelve por la interfaz los valores x e y del vertice
bool poligono_obtener_vertice( const poligono_t *poligono, size_t pos, float *x, float *y ){
    if ( pos >= poligono -> n )
        return false;

    *x = poligono -> vertices[pos][0];
    *y = poligono -> vertices[pos][1];
    return true;
}
//devuelve un puntero a un clon de el poligono de entrada
poligono_t *poligono_clonar( const poligono_t *poligono ){
    poligono_t *clon = poligono_crear( poligono -> vertices, poligono ->  n );
    if ( clon == NULL )
        return NULL;

    return clon;
}
//agrega el vertice (x,y) al final,redimensiona el vector y actualiza el n. Solo falla si realloc devuelve NULL
bool poligono_agregar_vertice( poligono_t *poligono, float x, float y ){
    float (*aux)[2] = realloc( poligono -> vertices, (poligono -> n + 1) * sizeof(float [2]) );
    if ( aux == NULL )
        return false;

    aux[poligono -> n][0] = x ;
    aux[poligono -> n][1] = y ;

    poligono -> n = (poligono -> n + 1) ;
    poligono -> vertices = aux ;
    return true;
}

bool punto_en_poligono(poligono_t * pol, float px, float py){

    size_t n = pol -> n;

    for (size_t i = 1 ; i < n-1; i++) {
        if(punto_en_triangulo( px, py, (pol -> vertices)[0][0], (pol -> vertices)[0][1], (pol -> vertices)[i][0], (pol -> vertices)[i][1], (pol -> vertices)[i+1][0], (pol -> vertices)[i+1][1]))
            return true;
    }
    return false;
}
/*calcula el producto cruz para dos vectores en r2 */
double determinante(double u[], double v[]){
    return (u[0] * v[1]) - (u[1] * v[0]);
}

/*da true si el punto esta dentro del triangulo*/
bool punto_en_triangulo(float px, float py, float ax, float ay, float bx, float by, float cx, float cy){
    double va_b[2]={ bx-ax , by-ay }, vb_c[2]={ cx-bx , cy-by  }, vc_a[2]={ ax-cx, ay-cy  };
    double va_p[2]={ px-ax, py-ay }, vb_p[2]={ px-bx, py-by }, vc_p[2]={ px-cx, py-cy };
    double  aux_a = determinante(va_b,va_p), aux_b = determinante(vb_c,vb_p), aux_c = determinante(vc_a,vc_p);

    return (((aux_a >= 0 && aux_b >= 0 && aux_c >= 0) || (aux_a <= 0 && aux_b <= 0 && aux_c <= 0)));
}

/*rota los vertices de un poligono, el angulo debe estar en radianes*/
void poligono_rotar(float poligono[][2], size_t n, double rad){
    float x_prima, y_prima;
    float coseno = cos(rad), seno = sin(rad);

    for (size_t i = 0; i < n; i++) {
        x_prima = poligono[i][0] * coseno - poligono[i][1] * seno;
        y_prima = poligono[i][0] * seno + poligono[i][1] * coseno;
        poligono[i][0] = x_prima;
        poligono[i][1] = y_prima;
    }
}

/*traslada los vertices de un poligono*/

void poligono_trasladar(poligono_t *cuerpo, float dx, float dy){
    for (size_t i = 0; i < cuerpo->n; i++) {
        (cuerpo->vertices)[i][0] += dx;
        (cuerpo->vertices)[i][1] += dy;
    }
}

void poligono_rotar_respecto_al_centro(poligono_t *cuerpo,float cx, float cy, float rad){
  //a probar
    poligono_trasladar(cuerpo ,-cx, -cy);
    poligono_rotar(cuerpo -> vertices, cuerpo->n, rad);
    poligono_trasladar(cuerpo, cx, cy);
}

poligono_t *poligono_crear_circulo(float radio,float cx,float cy){
    poligono_t *circular = poligono_crear(NULL, 0);
    if (!circular)
        return NULL;

    float p[1][2];
    p[0][0] = radio;
    p[0][1] = 0;

    for (size_t i = 0; i < DEFINICION_CIRCULO; i++){
        poligono_agregar_vertice(circular, p[0][0], p[0][1]);
        poligono_rotar(p , 1 , DELTA_RAD);
    }

    poligono_trasladar(circular , cx, cy);
    return circular;
}

//TDA_poligono no conoce color_t. Es mas general que algunos pocos colores
void poligono_dibujar(poligono_t *cuerpo, SDL_Renderer *renderer, uint32_t rgb){
    uint8_t red, green, blue;

    descomponer_colores(rgb, &red, &green, &blue);
    SDL_SetRenderDrawColor(renderer, red, green, blue, SDL_ALPHA_OPAQUE);

    for (size_t i = 0; i < cuerpo->n - 1 ; i++)
        SDL_RenderDrawLine(renderer, cuerpo -> vertices[i][0],cuerpo -> vertices[i][1] ,cuerpo -> vertices[i+1][0] ,cuerpo -> vertices[i+1][1] );

    SDL_RenderDrawLine(renderer,cuerpo -> vertices[cuerpo->n-1][0] ,cuerpo -> vertices[cuerpo->n-1][1],  cuerpo -> vertices[0][0],cuerpo -> vertices[0][1] );
}

double distancia(float x1, float y1, float x2, float y2){
    return sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

float producto_interno(float ax, float ay, float bx, float by){
    return ax * bx + ay * by;
}
/*
Para encontrar la distancia de un punto a una recta se proyecta el punto
ortogonalmente sobre la recta.
El producto [(X.P) / (X.X)] X es la proyección del punto P sobre X.
El coeficiente entre corchetes será la proporción de P sobre X.
Como estamos trabajando con segmentos de recta, si el coeficiente es menor a
cero o mayor a uno nos caímos del segmento.
*/
void punto_mas_cercano(float x0, float y0, float x1, float y1, float xp, float yp, float *x, float *y) {
    float ax = xp - x0;
    float ay = yp - y0;
    float bx = x1 - x0;
    float by = y1 - y0;

    float alfa = producto_interno(ax, ay, bx, by) / producto_interno(bx, by, bx, by);

    if(alfa <= 0) {
        *x = x0;
        *y = y0;
    }
    else if(alfa >= 1) {
        *x = x1;
        *y = y1;
    }
    else {
        *x = alfa * bx + x0;
        *y = alfa * by + y0;
    }
}

/*
Reflejamos según P' = P - 2 D(P.D)
*/
void reflejar(float norm_x, float norm_y, float *cx, float *cy, float *vx, float *vy) {
    float proy = producto_interno(norm_x, norm_y, *vx, *vy);

    if(proy >= 0)
        return;

    *vx -= 2 * norm_x * proy;
    *vy -= 2 * norm_y * proy;

    // Además empujamos a la bola hacia afuera para que no se pegue
    *cx += norm_x * 0.1;
    *cy += norm_y * 0.1;
}


double poligono_distancia(const poligono_t *p, float xp, float yp, float *nor_x, float *nor_y) {
    double d = 1 / 0.0;
    size_t idx = 0;

    for(size_t i = 0; i < (p -> n); i++) {
        float xi, yi;
        punto_mas_cercano(p->vertices[i][0], p->vertices[i][1], p->vertices[(i + 1) % p->n][0], p->vertices[(i + 1) % p->n][1], xp, yp, &xi, &yi);
        double di = distancia(xp, yp, xi, yi);

        if(di < d) {
            d = di;
            idx = i;
        }
    }

    float nx = p->vertices[(idx + 1) % p->n][1] - p->vertices[idx][1];
    float ny = p->vertices[idx][0] - p->vertices[(idx + 1) % p->n][0];
    float dn = distancia(nx, ny, 0, 0);

    nx /= dn;
    ny /= dn;

    *nor_x = nx;
    *nor_y = ny;

    return d;
}