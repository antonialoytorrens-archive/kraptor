/*-------------------------------------------------------
 explos.h
 -------------------------------------------------------- 
 Copyright (c) 2002, Kronoman 
 En memoria de mi querido padre
 Agosto - 2002
 -------------------------------------------------------- 
 Engine de explosiones usando una lista enlazada
 muy sencilla, sirve para muchas capas de explosiones
 mediante el uso de punteros.
 --------------------------------------------------------*/ 

#ifndef EXPLOS_H
#define EXPLOS_H

#include "allegro.h"


/* estructura de datos  */
typedef struct EXPLOSION {
    fixed x,  y;     /* posicion  */
    fixed dx, dy;    /* direccion */
    int vida;      /* vida que le queda */
    fixed r, dr;   /* radio y decremento de radio por ciclo */
    BITMAP *spr;     /* sprite que lo representa */
    int rot; /* rotacion del bitmap */

    /* puntero al siguiente */
    struct EXPLOSION *next;
} EXPLOSION;



extern int cant_explosion_debug; /* Innecesaria, para testear performance solamente */

/* Cache de imagenes y sonidos, para el combo de explosion */
extern BITMAP *explo_cache_bmp[3];
extern SAMPLE *explo_cache_snd[3];

/* punteros */
extern EXPLOSION *ptr_explo_fondo;
extern EXPLOSION *ptr_explo_arriba;


/* prototipos */
void       agrega_explosion( EXPLOSION **prt_1era,
                             fixed x,  fixed y,
                             fixed dx, fixed dy,
                             int vida,
                             fixed r, fixed dr,
                             int rot,
                             BITMAP *spr );
void mover_explosiones(EXPLOSION **prt_1era);
void dibujar_explosion(EXPLOSION *prt_1era, BITMAP *bmp, int x, int y);
void liberar_lista_explosion(EXPLOSION **prt_1era);
#endif
