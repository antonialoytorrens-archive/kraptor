/*-------------------------------------------------------
 premio.c
 -------------------------------------------------------- 
 Copyright (c) 2002, Kronoman 
 En memoria de mi querido padre
 Enero - 2003
 -------------------------------------------------------- 
 Engine de premios
 usando una lista enlazada  muy sencilla
 --------------------------------------------------------*/ 

#ifndef PREMIO_C
#define PREMIO_C

#include <stdio.h>
#include <allegro.h>

#include "jugador.h"
#include "premio.h"
#include "azar.h"
#include "pmask.h"
#include "combo.h" // para las particulas
#include "global.h" // para chequear el nivel de detalle
#include "jugador.h" // para poder darle los premios
#include "sonido.h"

/* globales exportadas */
PREMIO_CLASS premio_class[MAX_PREMIO_CLASS];

/* --- globales internas --- */
static PREMIO *prt_1er_premio = NULL; /* puntero al comienzo de la lista */

/* Esta funcion agrega un nuevo premio
   Devuelve puntero al creado.
   Si vida <= 0, NO se crea...
   Pasarle el indice de la clase de premio a crear y su posicion
   La direccion es elegida al azar... :)
   */
PREMIO *agrega_premio(int premio, fixed x, fixed y)
{
    PREMIO *nueva = NULL;
    if (premio < 0 || premio > MAX_PREMIO_CLASS-1) return NULL; /* fuera de rango */
    if (premio_class[premio].vida <= 0) return NULL; /* ni pierdo el tiempo... */


    nueva = (PREMIO *)malloc(sizeof(PREMIO));
    nueva->next = prt_1er_premio;
    prt_1er_premio = nueva;

    if (nueva != NULL) /* si el malloc funciono, seteo los datos... */
    {
        nueva->x = x;
        nueva->y = y;
        nueva->r = itofix(rand()%256);
        nueva->dx = ftofix((float)rand_ex(-500, 500) / 100.0);
        nueva->dy = ftofix((float)rand_ex(-500, 500) / 100.0);
        nueva->dr = ftofix((float)rand_ex(-160, 160) / 10.0);
        nueva->vida = premio_class[premio].vida;
        nueva->clase = premio;
    }

    return nueva;
}

/*
   Esta funcion actualiza la logica (mueve) los premios
   tambien los elimina si vida < 0 o si estan fuera de pantalla
   Pasar en x, y la posicion de scroll en pantalla
         en w, h el ancho y alto de la pantalla
   Comprueba si toca al jugador, para darle el premio tambien...
   */
void mover_premio(int x, int y, int w, int h)
{
//    int i;
    PREMIO **tmp_p = &prt_1er_premio;
    PREMIO *tmp = NULL;

    while (*tmp_p) {

        tmp = *tmp_p;
         tmp->x = fixadd(tmp->x, tmp->dx);
         tmp->y = fixadd(tmp->y, tmp->dy);
         tmp->r = fixadd(tmp->r, tmp->dr); // rotar
         
         /* debug: comprobar si toco al jugador; bounding box pedorro */
         if (check_bb_collision_general(fixtoi(tmp->x),fixtoi(tmp->y),
                                    premio_class[tmp->clase].sprite->w,
                                    premio_class[tmp->clase].sprite->h,
                                    fixtoi(jugador.x),fixtoi(jugador.y),
                                    jugador.spr[1]->w ,jugador.spr[1]->h) != 0)
                                    {
                                    /* toco al jugador, dar premio! */

                                    switch (premio_class[tmp->clase].premiar)
                                    {
                                    case -2: // bombas
                                      jugador.bombas += premio_class[tmp->clase].cantidad;

                                      if (jugador.bombas > jugador.max_bombas)
                                           jugador.bombas = jugador.max_bombas;

                                    break;

                                    case -1: // energia
                                        jugador.vida += premio_class[tmp->clase].cantidad;
                                        if (jugador.vida >= MAX_ENERGIA) jugador.vida = MAX_ENERGIA;
                                    break;

                                    default: // arma
                                      jugador.arma[premio_class[tmp->clase].premiar] += premio_class[tmp->clase].cantidad;

                                      if (jugador.arma[premio_class[tmp->clase].premiar] > arma_tipo[premio_class[tmp->clase].premiar].cant_ammo_max)
                                             jugador.arma[premio_class[tmp->clase].premiar] = arma_tipo[premio_class[tmp->clase].premiar].cant_ammo_max;

                                      // seleccionar el arma - DEBUG, no se si es lo mejor...
                                      jugador.arma_actual = premio_class[tmp->clase].premiar;
                                    break;
                                    }

                                    // tocar el sonido!!
                                    if (premio_class[tmp->clase].sonido != NULL)
                                        tocar_sonido(premio_class[tmp->clase].sonido, 255, 128, 1000);

                                    tmp->vida = 0;
                                    }


        tmp->vida--;

        /* verificar si estan fuera de pantalla y hacerlos rebotar */

        if (tmp->y < itofix(y))
        {
                tmp->y = itofix(y);
                tmp->dy = fixmul(tmp->dy, itofix(-1));
        }
        if (tmp->x < itofix(x))
        {
                tmp->x = itofix(x);
                tmp->dx = fixmul(tmp->dx, itofix(-1));
        }

        if (tmp->x > itofix(x + w - premio_class[tmp->clase].sprite->w))
        {
             tmp->x = itofix(x + w - premio_class[tmp->clase].sprite->w);
             tmp->dx = fixmul(tmp->dx, itofix(-1));
        }

        if (tmp->y > itofix(y + h - premio_class[tmp->clase].sprite->h))
        {
                tmp->y = itofix(y + h - premio_class[tmp->clase].sprite->h);
                tmp->dy = fixmul(tmp->dy, itofix(-1));
        }

        if (tmp->vida < 0) {
          /*  murio, eliminar!!! */

          /* explotan con algunas particulas */
          if (nivel_detalle > 7)
             poner_muchas_particulas( fixtoi(tmp->x), fixtoi(tmp->y),
                                      5,5,
                                      10,
                                      -1, 1, 1,
                                      NULL,
                                      rand()%10+30 );


          *tmp_p = tmp->next;

          free(tmp);

        } else {
            tmp_p = &tmp->next; /* siguiente por favor! */
        }
    }
}

/* Esta funcion se debe llamar cuando no se precise
   mas la lista de premios
   Libera la RAM usada y reinicia la lista
   */
void liberar_premio() 
{
    PREMIO *tmp = prt_1er_premio;
    prt_1er_premio = NULL;

    while (tmp) {
        PREMIO *next = tmp->next;
        free(tmp);
        tmp = next;
    }
}

/*
   Esta funcion dibuja los premios en el bitmap bmp
   Las dibuja desplazado x,y
   */
void dibujar_premio(BITMAP *bmp, int x, int y)
{
    PREMIO *tmp = prt_1er_premio;

    while (tmp) {
         /* dibujar */
         if (premio_class[tmp->clase].sprite != NULL)
           rotate_sprite(bmp, premio_class[tmp->clase].sprite, fixtoi(tmp->x)-x, fixtoi(tmp->y)-y, tmp->r);
         else
           circlefill(bmp, fixtoi(tmp->x)-x, fixtoi(tmp->y)-y, 5, makecol(255,255,255));

        tmp = tmp->next; /* proximo... */
      }
}

#endif
