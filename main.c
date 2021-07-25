/**************************************************
 * FIUBA - (95.11) ALGORITMOS Y PROGRAMACIÓN I
 * ESSAYA - SANTISI
 * TRABAJO PRACTICO FINAL
 * 
 * LINARES, JUAN MATEO
 * PADRON 102990
 * 
 * FRANCISCO SPALTRO
 * PADRÓN 102098
 * 
 * ESTÁNDAR ISO-C99
 * ************************************************/

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <time.h>

#include "config.h"
#include "TDA_poligono.h"
#include "niveles.h"
#include "TDA_lista.h"
#include "TDA_jugador.h"

#define MAX_ARGC 2
#define MAX_STR_FECHA 30
#define DT (1.0 / JUEGO_FPS)
#define MAX_NOMBRE 50

#ifdef TTF
#include <SDL2/SDL_ttf.h>

void escribir_texto(SDL_Renderer *renderer, TTF_Font *font, const char *s, int x, int y) {
    SDL_Color color = {255, 255, 255};
    SDL_Surface *surface = TTF_RenderText_Solid(font, s, color);
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_Rect rect;
    rect.x = x;
    rect.y = y;
    rect.w = surface->w;
    rect.h = surface->h;

    SDL_RenderCopy(renderer, texture, NULL, &rect);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}
#endif

/*************************************** MAIN ***************************************/

int main(int argc, char *argv[]) {

    if(argc != MAX_ARGC){
        fprintf(stderr, "Error de invocación. Intente %s archivo_niveles.bin\n", argv[0]);
        return 1;
    }

    //APERTURA DE ARCHIVOS NECESARIOS
    FILE * f_niveles = fopen(argv[1], "rb");
    if(f_niveles == NULL){
        fprintf(stderr, "Error al abrir archivo de niveles\n");
        return 1;
    }

    FILE * f_partida = fopen("partida.txt", "at");
    if(f_partida == NULL){
        fprintf(stderr, "Error al abrir el archivo de partidas\n");
        fclose(f_niveles);
        return 1;
    }

    //CONFIGURACION DEL JUGADOR
    char nombre[MAX_NOMBRE];
    fprintf(stdout, "Hola! Ingrese su nombre: ");

    fgets(nombre, MAX_NOMBRE, stdin);

    for(size_t i = 0; nombre[i] != '\0'; i++){
        if(nombre[i] == '\n'){
            nombre[i] = '\0';
            if(!nombre[0])
                strcpy(nombre, "s/d");
            break;
        }
    }

    jugador_t * jugador = jugador_crear(nombre);
    if(jugador == NULL){
        fclose(f_partida);
        fclose(f_niveles);
        return 1;
    }

    color_t color;
    movimiento_t movimiento;
    geometria_t geometria;
    size_t n_obstaculos;
    size_t obstaculos_naranjas_iniciales = 0;
    size_t obstaculos_naranjas;
    bool bloqueado = false;

    poligono_t *bola = NULL;
    lista_t * lista_obstaculos = NULL;
    lista_t * lista_obstaculos_auxiliar = NULL;
    lista_iter_t * iter_lista_obstaculos = NULL;

    //ESTABLECIMIENTO DE LA FECHA
    time_t t = time(NULL);
    struct tm * datos_fecha = localtime(&t);
    char fecha[MAX_STR_FECHA];
    traducir_fecha(fecha, datos_fecha);

    //IMPRESION DATOS EN PLANILLA DE TEXTO
    fprintf(f_partida, "%s\n", fecha);

    //APERTURA DE PANTALLA
    SDL_Init(SDL_INIT_VIDEO);

#ifdef TTF
    TTF_Init();
    TTF_Font* font = TTF_OpenFont("FreeSansBold.ttf", 24);
#endif

    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;

    SDL_CreateWindowAndRenderer(VENTANA_ANCHO, VENTANA_ALTO, 0, &window, &renderer);
    SDL_SetWindowTitle(window, "Peggle");

    int dormir = 0;

    float canon_angulo = 0; // Ángulo del cañón
    bool cayendo;   // ¿Hay bola disparada?

    float cx, cy;   // Centro de la bola
    float vx, vy;   // Velocidad de la bola

    float norm_x , norm_y ;

    //COMIENZA EL CICLO DE NIVELES
    int nivel = 0;
    while(1){
        cayendo = false;
        nivel ++;
        jugador_resetear_intentos(jugador);
    // SETEAR EL ESCENARIO DEL JUEGO (LOS OBSTACULOS)
        lista_obstaculos = lista_crear();
        if(!lista_obstaculos){
            fclose(f_niveles);
            fclose(f_partida);
            jugador_destruir(jugador);
            fprintf(stderr, "Error de memoria");
            return 1;
        }
        iter_lista_obstaculos = lista_iter_crear(lista_obstaculos);
        if(!iter_lista_obstaculos){
            lista_destruir(lista_obstaculos,obstaculo_destruir);
            fclose(f_niveles);
            fclose(f_partida);
            jugador_destruir(jugador);
            fprintf(stderr, "Error de memoria");
            return 1;
        }
        n_obstaculos = leer_cantidad_obstaculos(f_niveles);

        //CHEQUEO EOF (POS-LECTURA)
        if(feof(f_niveles)){
            fprintf(stdout, "Terminaste el juego. Felicitaciones!\n");
            fprintf(f_partida, "Jugador/jugadora: %s - completo\n", jugador_obtener_nombre(jugador));
            fprintf(stdout, "Tu puntajes final es %zd\n", jugador_obtener_puntaje(jugador));

            fprintf(f_partida, "Puntaje final (ganado): %zd\n\n", jugador_obtener_puntaje(jugador));

            jugador_destruir(jugador);
            lista_iter_destruir(iter_lista_obstaculos);
            lista_destruir(lista_obstaculos, obstaculo_destruir);
            goto TERMINAR_JUEGO;
        }

        obstaculos_naranjas_iniciales = 0;
        for(size_t i = 0; i < n_obstaculos; i++){
            if(!leer_encabezado(f_niveles, &color, &movimiento, &geometria)){
                fclose(f_niveles);
                fclose(f_partida);
                jugador_destruir(jugador);
                lista_iter_destruir(iter_lista_obstaculos);
                lista_destruir(lista_obstaculos, obstaculo_destruir);
                fprintf(stderr, "Archivo de niveles corrupto\n");
                return 1;
            }

            int16_t * parametros_movimiento = malloc(sizeof(int16_t)*4);
            if(parametros_movimiento == NULL){
                lista_iter_destruir(iter_lista_obstaculos);
                lista_destruir(lista_obstaculos, obstaculo_destruir);
                jugador_destruir(jugador);
                fclose(f_niveles);
                fclose(f_partida);
                fprintf(stderr, "Memoria insuficiente");
                return 1;
            }

            size_t n_parametros_movimiento;
            if(!leer_movimiento(f_niveles, movimiento, parametros_movimiento, &n_parametros_movimiento)){
                free(parametros_movimiento);
                jugador_destruir(jugador);
                lista_iter_destruir(iter_lista_obstaculos);
                lista_destruir(lista_obstaculos, obstaculo_destruir);
                fclose(f_partida);
                fclose(f_niveles);
                fprintf(stderr, "Archivo de niveles corrupto\n");
                return 1;
            }

            poligono_t *cuerpo = leer_geometria(f_niveles, geometria);
            if(cuerpo == NULL){
                free(parametros_movimiento);
                jugador_destruir(jugador);
                lista_iter_destruir(iter_lista_obstaculos);
                lista_destruir(lista_obstaculos, obstaculo_destruir);
                fclose(f_partida);
                fclose(f_niveles);
                fprintf(stderr, "Archivo de niveles corrupto\n");
                return 1;
            }

            obstaculo_t *obstaculo = obstaculo_crear(cuerpo, color, movimiento, geometria, parametros_movimiento);
            if(obstaculo == NULL){
                poligono_destruir(cuerpo);
                free(parametros_movimiento);
                jugador_destruir(jugador);
                lista_destruir(lista_obstaculos, obstaculo_destruir);
                lista_iter_destruir(iter_lista_obstaculos);
                fclose(f_niveles);
                fclose(f_partida);
                fprintf(stderr, "Archivo de niveles corrupto\n");
                return 1;
            }

            if(color == NARANJA)
                obstaculos_naranjas_iniciales++;

            //CARGA EN LISTA
            lista_insertar_ultimo(lista_obstaculos, obstaculo);
        }

        //CANTIDAD DE OBSTACULOS NARANJAS EN EL NIVEL
        obstaculos_naranjas = obstaculos_naranjas_iniciales;

        //CREACION DE LA LISTA AUXILIAR (PARA REINICIOS)
        lista_obstaculos_auxiliar  = lista_clonar(lista_obstaculos, obstaculo_clonar, obstaculo_destruir);
        if(lista_obstaculos_auxiliar == NULL){
            fclose(f_niveles);
            fclose(f_partida);
            jugador_destruir(jugador);
            lista_iter_destruir(iter_lista_obstaculos);
            lista_destruir(lista_obstaculos, obstaculo_destruir);
            fprintf(stderr, "Memoria insuficiente\n");
            return 1;
        }

        unsigned int ticks = SDL_GetTicks();

        //MIENTRAS SE ESTA JUGANDO
        while(1) {
            //SI CIERRO LA VENTANA
            if(SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT){
                    fprintf(stdout, "Puntaje final: %zd\n", jugador_obtener_puntaje(jugador));
                    fprintf(stdout, "Volve pronto!\n");

                    fprintf(f_partida, "Jugador/jugadora: %s - abandonado\n", jugador_obtener_nombre(jugador));
                    fprintf(f_partida, "Puntaje final: %zd\n\n", jugador_obtener_puntaje(jugador));

                    jugador_destruir(jugador);
                    lista_iter_destruir(iter_lista_obstaculos);
                    lista_destruir(lista_obstaculos, obstaculo_destruir);
                    lista_destruir(lista_obstaculos_auxiliar, obstaculo_destruir);
                    goto TERMINAR_JUEGO;
                }

                if(event.type == SDL_MOUSEBUTTONDOWN) {
                    if(! cayendo){
                        cayendo = true;
                        //UNA BOLA MENOS AL LANZAR
                        jugador_modificar_intentos(jugador, -1);
                    }
                }
                else if (event.type == SDL_MOUSEMOTION) {
                    canon_angulo = atan2(event.motion.x - CANON_X, event.motion.y - CANON_Y);
                    if(canon_angulo > CANON_MAX)
                        canon_angulo = CANON_MAX;
                    if(canon_angulo < -CANON_MAX)
                        canon_angulo = -CANON_MAX;
                }

                continue;
            }
            SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
            SDL_RenderClear(renderer);
            SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0x00);

    #ifdef TTF
            char str_titulo[MAX_NOMBRE + 7];
            sprintf(str_titulo, "Nivel %d: %s", nivel, jugador_obtener_nombre(jugador));
            escribir_texto(renderer, font, str_titulo, 30, 20);
    #endif //TTF

            if(cayendo) {
                // Si la bola está cayendo actualizamos su posición
                vy = computar_velocidad(vy, G, DT);
                vx *= ROZAMIENTO;
                vy *= ROZAMIENTO;
                cx = computar_posicion(cx, vx, DT);
                cy = computar_posicion(cy, vy, DT);
            }
            else {
                // Si la bola no se disparó establecemos condiciones iniciales
                cx = CANON_X + CANON_LARGO * sin(canon_angulo);
                cy = CANON_Y + CANON_LARGO * cos(canon_angulo);
                vx = BOLA_VI * sin(canon_angulo);
                vy = BOLA_VI * cos(canon_angulo);
                //CHEQUEO CANTIDAD DE BOLAS AL INICIO DEL PROXIMO NIVEL
                if(jugador_obtener_intentos(jugador) <= 0){
                    fprintf(f_partida, "Puntaje antes de perder: %zd\n", jugador_obtener_puntaje(jugador));
                    jugador_resetear_naranjas(jugador);
                    jugador_resetear_intentos(jugador);
                    jugador_resetear_puntos(jugador);
                    
                    continue;
                }
            }


            // Rebote contra las paredes:
            if(cx < MIN_X + BOLA_RADIO || cx > MAX_X - BOLA_RADIO) vx = -vx;
            if(cy < MIN_Y + BOLA_RADIO) vy = -vy;


            // Se salió de la pantalla:
            if(cy > MAX_Y - BOLA_RADIO){
                cayendo = false;
                //poligono_destruir(bola);
                //CHEQUEO CANTIDAD DE BOLAS (DURANTE UN NIVEL)
                if(jugador_obtener_intentos(jugador) == 0){
                    
                    lista_destruir(lista_obstaculos, obstaculo_destruir);
                    lista_obstaculos = lista_obstaculos_auxiliar;
                    lista_obstaculos_auxiliar = lista_clonar(lista_obstaculos, obstaculo_clonar, obstaculo_destruir);
                    if(lista_obstaculos_auxiliar == NULL){
                        fclose(f_niveles);
                        fclose(f_partida);
                        lista_iter_destruir(iter_lista_obstaculos);
                        lista_destruir(lista_obstaculos, obstaculo_destruir);
                        jugador_destruir(jugador);
                        fprintf(stderr, "Memoria insuficiente\n");
                        return 1;
                    }
                    fprintf(f_partida, "Puntaje antes de perder: %zd\n", jugador_obtener_puntaje(jugador));
                    jugador_resetear_intentos(jugador);
                    jugador_resetear_puntos(jugador);
                    jugador_resetear_naranjas(jugador);
                    obstaculos_naranjas = obstaculos_naranjas_iniciales;
                }
                //CHEQUEO SI YA DESTRUI LOS OBSTACULOS NARANJAS
                if(obstaculos_naranjas <= 0)
                    continue;

            }


            // Dibujamos el cañón:
            SDL_RenderDrawLine(renderer, CANON_X, CANON_Y, CANON_X + sin(canon_angulo) * CANON_LARGO, CANON_Y + cos(canon_angulo) * CANON_LARGO);

            // Dibujamos los textos
            #ifdef TTF
                if(jugador_obtener_intentos(jugador) >= 0){
                    char str_intentos[MAX_NOMBRE];
                    sprintf(str_intentos, "Bolas: %d", jugador_obtener_intentos(jugador));
                    escribir_texto(renderer, font, str_intentos, 280, 20);

                    char str_naranjas[MAX_NOMBRE];
                    sprintf(str_naranjas, "Naranjas: %zd", obstaculos_naranjas);
                    escribir_texto(renderer, font, str_naranjas, 430, 20);

                    char str_puntaje[MAX_NOMBRE];
                    sprintf(str_puntaje, "Puntos: %zd", jugador_obtener_puntaje(jugador));
                    escribir_texto(renderer, font, str_puntaje, 620, 20);
                }
            #endif //TTF

            // Dibujamos la bola:
            bola = poligono_crear_circulo(BOLA_RADIO, cx , cy);
            poligono_dibujar(bola, renderer, 0xffffff);

            // Dibujamos las paredes:
            SDL_SetRenderDrawColor(renderer, 0x00, 0xFF, 0xFF, 0x00);
            SDL_RenderDrawLine(renderer, MIN_X, MIN_Y, MAX_X, MIN_Y);
            SDL_RenderDrawLine(renderer, MIN_X, MAX_Y, MAX_X, MAX_Y);
            SDL_RenderDrawLine(renderer, MIN_X, MAX_Y, MIN_X, MIN_Y);
            SDL_RenderDrawLine(renderer, MAX_X, MAX_Y, MAX_X, MIN_Y);

            // Dibujamos el vector de velocidad:
            SDL_RenderDrawLine(renderer, cx  , cy , cx + vx , cy + vy );

            // Dibujamos los obstaculos aca se puede estar perdiendo memoria
            // Rebote contra los objetos:
            for(
                lista_iter_apuntar_al_principio(iter_lista_obstaculos,lista_obstaculos);
                !lista_iter_al_final(iter_lista_obstaculos);
                lista_iter_avanzar(iter_lista_obstaculos)){

    //DIBUJADO
                obstaculo_t * obs_actual = lista_iter_ver_actual(iter_lista_obstaculos);
                obstaculo_dibujar(obs_actual, renderer);

    //REBOTES
                poligono_t *cuerpo = obstaculo_obtener_cuerpo(obs_actual);
                if((poligono_distancia(cuerpo, cx+BOLA_RADIO , cy+BOLA_RADIO , &norm_x, &norm_y) < BOLA_RADIO)){
                    if(!obstaculo_esta_marcado(obs_actual)){
                        if(obstaculo_obtener_color(obs_actual) == NARANJA){
                            obstaculos_naranjas--;
                        }
                        obstaculo_ejecutar_modificaciones(obs_actual, jugador);
                        obstaculo_marcar(obs_actual);
                    }

                    reflejar(norm_x, norm_y, &cx , &cy, &vx, &vy);
                    vx *= PLASTICIDAD;
                    vy *= PLASTICIDAD;
                }

    //PROTECCION CONTRA BLOQUEO
                if(cayendo && sqrt(vx*vx + vy*vy) < VELOCIDAD_MINIMA){
                    bloqueado = true;
                }

    //ELIMINAR MARCADOS (si esta bloqueado o si la bola cayo)
                if((bloqueado && obstaculo_esta_marcado(obs_actual)) || (!cayendo && obstaculo_esta_marcado(obs_actual))){
                    lista_iter_borrar(iter_lista_obstaculos);
                    bloqueado = false;
                }

    //MOVIMIENTOS
                obstaculo_mover(obs_actual);

            }


            SDL_RenderPresent(renderer);
            ticks = SDL_GetTicks() - ticks;
            if(dormir) {
                SDL_Delay(dormir);
                dormir = 0;
            }
            else if(ticks < 1000 / JUEGO_FPS)
                SDL_Delay(1000 / JUEGO_FPS - ticks);
            ticks = SDL_GetTicks();

            if(obstaculos_naranjas == 0)
                break;
        
            poligono_destruir(bola);
        }

        //LIBERACIONES AL FINAL DE CADA NIVEL
        poligono_destruir(bola);
        lista_iter_destruir(iter_lista_obstaculos);
        lista_destruir(lista_obstaculos,obstaculo_destruir);
        lista_destruir(lista_obstaculos_auxiliar, obstaculo_destruir);
    }

    //CIERRE DE VENTANA
    TERMINAR_JUEGO:
    fclose(f_partida);
    fclose(f_niveles);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

#ifdef TTF
    TTF_CloseFont(font);
    TTF_Quit();
#endif
    SDL_Quit();
    return 0;
}