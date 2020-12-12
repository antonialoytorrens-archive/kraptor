/*-------------------------------------------------------
 premio.h
 -------------------------------------------------------- 
 Copyright (c) 2002, Kronoman 
 En memoria de mi querido padre
 Enero - 2003
 -------------------------------------------------------- */

#ifndef PREMIO_H
#define PREMIO_H

#include <allegro.h>

/* cantidad maxima de clases de premios */
#define MAX_PREMIO_CLASS 20

/* contenedor de las clases de premios */
typedef struct PREMIO_CLASS {
   int premiar;
   int cantidad;
   int vida;
   BITMAP *sprite;
   SAMPLE *sonido;
} PREMIO_CLASS;

/* estructura de datos que contiene cada premio individual; */
typedef struct PREMIO {
    fixed x,  y, r;     /* posicion  y rotacion (efecto cool)*/
    fixed dx, dy, dr;    /* direccion */
    int vida;        /* vida que le queda */
    int clase;       /* indice de la clase de premio */
    struct PREMIO *next;
} PREMIO;

/* globales exportadas */
extern PREMIO_CLASS premio_class[MAX_PREMIO_CLASS];

/* prototipos */
PREMIO *agrega_premio(int premio, fixed x, fixed y);
void mover_premio(int x, int y, int w, int h);
void liberar_premio();
void dibujar_premio(BITMAP *bmp, int x, int y);
#endif
