#include <stdbool.h>
#include <stdlib.h>

#include "TDA_obstaculo.h"
#include "TDA_poligono.h"
#include "TDA_jugador.h"
#include "config.h"

uint32_t colores[] = {
    [AZUL] = 0x0000ff,
    [NARANJA] = 0xff8000,
    [VERDE] = 0x00ff00,
    [GRIS] = 0xcdcdcd,
    [AMARILLO] = 0xffe900
};

struct obstaculo{
    poligono_t * cuerpo;
    color_t color;
    geometria_t geometria;
    movimiento_t movimiento;
    int16_t * parametros_movimiento;
    bool marcado;
    float x_centro_actual;
};

obstaculo_t * obstaculo_crear(poligono_t * cuerpo, color_t color, movimiento_t movimiento, geometria_t geometria, int16_t * parametros_movimiento){
    obstaculo_t * obstaculo = malloc(sizeof(obstaculo_t));
    if(obstaculo == NULL)
        return NULL;

    obstaculo -> cuerpo = cuerpo;
    obstaculo -> color = color;
    obstaculo -> movimiento = movimiento;
    obstaculo -> geometria = geometria;
    obstaculo -> parametros_movimiento = parametros_movimiento;
    obstaculo -> marcado = false;
    obstaculo -> x_centro_actual = (float)parametros_movimiento[1];

    return obstaculo;
}

void obstaculo_marcar(obstaculo_t *obstaculo){
    if(obstaculo -> color == GRIS)  return;
    obstaculo -> marcado = true;
    obstaculo -> color = AMARILLO;
}

bool obstaculo_esta_marcado(obstaculo_t * obstaculo){
    return obstaculo -> marcado;
}

void obstaculo_dibujar(obstaculo_t *obs, SDL_Renderer *renderer){
    color_t color_indice = obs -> color;

    poligono_dibujar(obs -> cuerpo, renderer, colores[color_indice]);
}

poligono_t * obstaculo_obtener_cuerpo(obstaculo_t * obs){
    return obs -> cuerpo;
}

void _obstaculo_destruir(obstaculo_t *obstaculo){
    free(obstaculo -> parametros_movimiento);
    poligono_destruir(obstaculo -> cuerpo);
    free(obstaculo);
}

void obstaculo_destruir(void *obstaculo){
    _obstaculo_destruir(obstaculo);
}

void modificar_obs_verde(obstaculo_t * obs, jugador_t * jugador){
    jugador_modificar_puntaje(jugador, +10);
    jugador_modificar_intentos(jugador, 1);
}

void modificar_obs_azul(obstaculo_t * obs, jugador_t * jugador){
    jugador_modificar_puntaje(jugador, +10);
}

void modificar_obs_naranja(obstaculo_t * obs, jugador_t * jugador){
    jugador_modificar_puntaje(jugador, +100);
    jugador_sumar_naranja(jugador);
}

void modificar_obs_gris(obstaculo_t * obs, jugador_t * jugador){
    //nada
    return;
}

void modificar_obs_amarillo(obstaculo_t * obs, jugador_t * jugador){
    //nada
    return;
}

void (* modificaciones[])(obstaculo_t *, jugador_t *) = {
    [AZUL] = modificar_obs_azul,
    [NARANJA] = modificar_obs_naranja,
    [VERDE] = modificar_obs_verde,
    [GRIS] = modificar_obs_gris,
    [AMARILLO] = modificar_obs_amarillo

};

void obstaculo_ejecutar_modificaciones(obstaculo_t * obstaculo, jugador_t * jugador){
    modificaciones[obstaculo -> color](obstaculo, jugador);
}


void obstaculo_mover_inmovil(obstaculo_t * obstaculo){
    return;
}

void obstaculo_mover_horizontal(obstaculo_t * obstaculo){
    poligono_t * cuerpo = obstaculo_obtener_cuerpo(obstaculo);

    if(obstaculo -> x_centro_actual <= 0){
        (obstaculo->parametros_movimiento)[2] *= -1;
        poligono_trasladar( cuerpo , 1 , 0);
    }
    if( (obstaculo -> x_centro_actual > (obstaculo->parametros_movimiento)[0] )){
        (obstaculo->parametros_movimiento)[2] *= -1;
        poligono_trasladar( cuerpo , 1 , 0);
    }

    obstaculo -> x_centro_actual += (float)(obstaculo->parametros_movimiento)[2]/JUEGO_FPS;

    poligono_trasladar(cuerpo , (float)(obstaculo->parametros_movimiento)[2]/JUEGO_FPS , 0);

}

void obstaculo_mover_circular(obstaculo_t * obstaculo){
    poligono_t * cuerpo = obstaculo_obtener_cuerpo(obstaculo);
    float x = (obstaculo->parametros_movimiento)[0];
    float y = (obstaculo->parametros_movimiento)[1];
    float velocidad = (obstaculo->parametros_movimiento)[2] / (float)JUEGO_FPS ;

    poligono_rotar_respecto_al_centro(cuerpo , x , y , velocidad);
}

void (*movimiento_obstaculo[])(obstaculo_t * obstaculo) = {
    [MOV_INMOVIL] = obstaculo_mover_inmovil,
    [MOV_CIRCULAR] = obstaculo_mover_circular,
    [MOV_HORIZONTAL] = obstaculo_mover_horizontal
};

void obstaculo_mover(obstaculo_t * obstaculo){
    movimiento_obstaculo[obstaculo -> movimiento](obstaculo);
}

obstaculo_t * _obstaculo_clonar(obstaculo_t * origen){
    poligono_t * cuerpo_copia = poligono_clonar(origen -> cuerpo);
    if(cuerpo_copia == NULL)
        return NULL;

    int16_t * parametros_movimiento = malloc(sizeof(int16_t)*4);
    if(parametros_movimiento == NULL){
        free(cuerpo_copia);
        return NULL;
    }

    memcpy(parametros_movimiento, origen -> parametros_movimiento, sizeof(int16_t)*4);

    obstaculo_t * copia = obstaculo_crear(cuerpo_copia, origen -> color, origen -> movimiento, origen -> geometria, parametros_movimiento);
    if(copia == NULL){
        free(cuerpo_copia);
        free(parametros_movimiento);
        return NULL;
    }

    copia -> x_centro_actual = origen -> x_centro_actual;

    return copia;
}

void * obstaculo_clonar( void *objeto_origen ){
    return _obstaculo_clonar((obstaculo_t *)objeto_origen);
}

color_t obstaculo_obtener_color(obstaculo_t * obstaculo){
    return obstaculo -> color;
}
