/*-------------------------------------------------------
 partic.h
 -------------------------------------------------------- 
 Copyright (c) 2002, Kronoman 
 En memoria de mi querido padre
 Agosto - 2002
 -------------------------------------------------------- 
 Engine de particulas usando una lista enlazada
 muy sencilla
 --------------------------------------------------------*/ 

#ifndef PARTIC_H
#define PARTIC_H

#include "allegro.h"

/* NOTA:
   el tipo de particula 't' puede ser:
   1 = circulo
   2 = cuadrado
   3 = linea
   o si spr != NULL, dibuja el bitmap rotando al azar (como que salto un pedazo
   */

/* estructura de datos que contiene la particula */
typedef struct PARTICULA {
    fixed x,  y;     /* posicion  */
    fixed dx, dy;    /* direccion */
    int vida;      /* vida que le queda a la particula */
    fixed r, rg; /* radio y valor de crecimiento del radio 'rg' */
    int col, t;   /* color  y tipo de grafico que la representa */
    int transp; /* la particula es transparente? -1 = si, 0 = no [solo en alto detalle ] */
    BITMAP *spr;     /* sprite que lo representa, o NULL si es una figura geometrica */
    fixed rot; /* rotacion del bitmap */

    /* puntero a la particula siguiente */
    struct PARTICULA *next;
} PARTICULA;


/* prototipos */

PARTICULA *agrega_particula( fixed x,  fixed y,
                             fixed dx, fixed dy,
                             int vida,
                             int col, int r, int t,
                             int transp, fixed rg,
                             BITMAP *spr );

void mover_particulas(int x, int y, int w, int h);

void dibujar_particulas(BITMAP *bmp, int x, int y);

void liberar_lista_particulas();



/* DEBUG - estas variables son especificas a Kraptor */

/* Cache de bmps representando particulas para 'reventar' naves */
extern BITMAP *particula_cache_bmp[3];

extern int cant_particulas_debug;

#endif
