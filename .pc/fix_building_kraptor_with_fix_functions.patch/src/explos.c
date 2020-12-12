/*-------------------------------------------------------
 explos.c
 -------------------------------------------------------- 
 Copyright (c) 2002, Kronoman 
 En memoria de mi querido padre
 Agosto - 2002
 -------------------------------------------------------- 
 Engine de explosiones usando una lista enlazada
 muy sencilla, sirve para muchas capas de explosiones
 mediante el uso de punteros.
 --------------------------------------------------------*/

#ifndef EXPLOS_C
#define EXPLOS_C

#include <stdio.h>
#include "allegro.h"

#include "explos.h"

/* esta variable NO es necesaria, solo la uso para
   ver cuantos enemigos hay en memoria, y de esa manera,
   revisar la performance... */
int cant_explosion_debug = 0;


/* --- globales internas --- */

/* Cache de imagenes y sonidos, para el combo de explosion */
BITMAP *explo_cache_bmp[3];
SAMPLE *explo_cache_snd[3];

/* No hay puntero al 1er item de la lista, debe crearse,
   permite usar muchas listas a la vez! */

/* Estos dos punteros deben ser puestos cada vez que se quieren usar
   en las diferentes llamadas */
EXPLOSION *ptr_explo_fondo = NULL;
EXPLOSION *ptr_explo_arriba = NULL;

/*----
Esta funcion agrega una nueva explosion a la lista enlazada [al principio].
PASARLE el puntero a la primera de la lista como &puntero!
----*/
void       agrega_explosion( EXPLOSION **prt_1era,
                             fixed x,  fixed y,
                             fixed dx, fixed dy,
                             int vida,
                             fixed r, fixed dr,
                             int rot,
                             BITMAP *spr ) {

    EXPLOSION *nueva = (EXPLOSION *)malloc(sizeof(EXPLOSION));
    nueva->next = *prt_1era;
    *prt_1era = nueva;

    if (nueva != NULL) /* si el malloc funciono, seteo los datos... */
    {
        nueva->x = x;
        nueva->y = y;
        nueva->dx = dx;
        nueva->dy = dy;
        nueva->vida = vida;
        nueva->r = r;
        nueva->dr = dr;
        nueva->rot = rot;
        nueva->spr = spr;
    }

}

/*
   Esta funcion actualiza la logica (mueve)
   tambien las elimina si vida < 0
   pasarle el puntero con &puntero
   */
void mover_explosiones(EXPLOSION **prt_1era) {
    EXPLOSION **tmp_p = prt_1era;
    EXPLOSION *tmp = NULL;

    cant_explosion_debug = 0; /* DEBUG: innecesario */   

    while (*tmp_p) {

        cant_explosion_debug++; /* DEBUG: innecesario */

        tmp = *tmp_p;

        /* aqui muevo */
        tmp->x = fixadd(tmp->x, tmp->dx);
        tmp->y = fixadd(tmp->y, tmp->dy);

        tmp->vida--;

        /* girar sprite  */
        tmp->rot += rand()%32+1; /* girar */
        if (tmp->rot > 255) tmp->rot = 0;

        /* decrementar */
        tmp->r = fixsub(tmp->r, tmp->dr);

        if (tmp->r < ftofix(0.001) ) tmp->vida = -1; /* se extinguio... */

        if (tmp->vida < 0) {
          /* murio, eliminar!!! */
          *tmp_p = tmp->next;
          free(tmp);
        } else {
            tmp_p = &tmp->next; /* siguiente por favor! */
        }
    }
}

/*
   Esta funcion dibuja en el bitmap bmp
   Las dibuja desplazadas x,y
   */
void dibujar_explosion(EXPLOSION *prt_1era, BITMAP *bmp, int x, int y) {

    EXPLOSION *tmp = prt_1era;

/* calculos */
int x2, y2;


 while (tmp) {

      x2 = fixtoi(tmp->x)-x;
      x2 -= fixtoi(fmul(itofix(tmp->spr->w), tmp->r))/2;

      y2 = fixtoi(tmp->y)-y;
      y2 -= fixtoi(fmul(itofix(tmp->spr->h), tmp->r))/2;

      /* dibujar */
      rotate_scaled_sprite(bmp, tmp->spr, x2, y2, itofix(tmp->rot), tmp->r);

      tmp = tmp->next; /* proximo... */
  }
}

/* Esta funcion se debe llamar cuando no se precise mas la lista
   Libera la RAM usada y reinicia la lista
   pasar el puntero asi &puntero
   */
void liberar_lista_explosion(EXPLOSION **prt_1era) {
    EXPLOSION *tmp = *prt_1era;
    *prt_1era = NULL;

    while (tmp) {
        EXPLOSION *next = tmp->next;
        free(tmp);
        tmp = next;
    }

}

#endif
