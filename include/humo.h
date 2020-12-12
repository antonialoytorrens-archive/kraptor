/*-------------------------------------------------------
 humo.h
 -------------------------------------------------------- 
 Copyright (c) 2002, Kronoman 
 En memoria de mi querido padre
 Enero - 2003
 -------------------------------------------------------- 
 Engine de humo (para edificios en llamas)
 usando una lista enlazada  muy sencilla
 --------------------------------------------------------*/ 

#ifndef HUMO_H
#define HUMO_H

#include <allegro.h>

extern BITMAP *humo_spr;

/* estructura de datos que contiene el humo */
typedef struct HUMO {
    fixed x,  y;     /* posicion  */
    fixed dx, dy;    /* direccion */
    int vida;      /* vida que le queda a la particula */
    int col; /* color */
    fixed r, ri;   /* radio, incremental de tama~o de radio */

    struct HUMO *next;
} HUMO;

/* estructura de datos para _emisores_ de humo */
typedef struct EMISOR_HUMO {
    fixed x,  y;     /* posicion  */
    int vida; // vida (se 'apaga' a medida que vida se aproxima a 0)

    struct EMISOR_HUMO *next;
} EMISOR_HUMO;

/* prototipos */

// Emisores de humo

EMISOR_HUMO *agrega_emisor_humo(fixed x, fixed y, int vida );
void mover_emisor_humo(int x, int y, int w, int h);
void liberar_emisores_humo();

// Humo en si...
HUMO *agrega_humo( fixed x,  fixed y,
                   fixed dx, fixed dy,
                   int vida,
                   int col, fixed r, fixed ri);

void mover_humo(int x, int y, int w, int h);
void dibujar_humo(BITMAP *bmp, int x, int y);
void liberar_humo();
#endif
