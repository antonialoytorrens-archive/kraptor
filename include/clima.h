// ------------------------------------
// clima.h
// ------------------------------------
// Modulo de efectos climaticos
// Por Kronoman
// En memoria de mi querido padre
// Copyright (c) 2002, Kronoman
// ------------------------------------
#ifndef CLIMA_H
#define CLIMA_H

#include <allegro.h>

// Estructura de particula de clima, para ser usada como una array
typedef struct CLIMA_P {
    fixed x,  y;     /* posicion  */
    fixed dx, dy;    /* direccion */
    int col, r;   /* color y radio */
    int t, d; // tipo y direccion de caida
} CLIMA_P;

// Cantidad maxima de particulas climaticas
#define MAX_CLIMA_P 300


// Prototipos
void init_clima(BITMAP *bmp, int c, int t, int d);
void mover_clima(BITMAP *bmp);
void dibuja_clima(BITMAP *bmp);

#endif
