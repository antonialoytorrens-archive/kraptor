/*-------------------------------------------------------
 humo.c
 -------------------------------------------------------- 
 Copyright (c) 2002, Kronoman 
 En memoria de mi querido padre
 Enero - 2003
 -------------------------------------------------------- 
 Engine de humo (para edificios en llamas)
 usando una lista enlazada  muy sencilla
 --------------------------------------------------------*/ 

#ifndef HUMO_C
#define HUMO_C

#include <stdio.h>
#include <allegro.h>

#include "humo.h"
#include "azar.h"

/* DEBUG - estas variables son especificas a Kraptor */
#include "global.h" // para saber el nivel de detalle unicamente...

BITMAP *humo_spr = NULL; // sprite de representacion del humo...

/* --- globales internas --- */

static HUMO *prt_1er_humo = NULL; /* puntero al comienzo de la lista */
static EMISOR_HUMO *prt1er_emisor = NULL; /* lista de emisores de humo */

//======================================================================== 
// Funciones de los emisores de humo,
// son zonas de 40x40 que emiten humo por un determinado espacio de tiempo


/* Esta funcion agrega un nuevo emisor de humo
   Devuelve puntero al emisor creado.
   Si vida <= 0, NO se crea... 
   */
EMISOR_HUMO *agrega_emisor_humo(fixed x, fixed y, int vida )
{
    EMISOR_HUMO *nueva = NULL;

    if (vida <= 0) return NULL; /* ni pierdo el tiempo... */

    if (nivel_detalle < 2) return NULL; // nivel de detalle minimo, no hacer nada

    nueva = (EMISOR_HUMO *)malloc(sizeof(EMISOR_HUMO));
    nueva->next = prt1er_emisor;
    prt1er_emisor = nueva;

    if (nueva != NULL) /* si el malloc funciono, seteo los datos... */
    {
        nueva->x = x;
        nueva->y = y;
        nueva->vida = vida;
    }

    return nueva;
}

/*
   Esta funcion actualiza la logica (mueve) los emisores de humo
   En realidad, los emisores de humo agregan particulas, pero no se mueven...
   tambien los elimina si vida < 0 o si estan fuera de pantalla
   Pasar en x, y la posicion de scroll en pantalla
         en w, h el ancho y alto de la pantalla
   */
void mover_emisor_humo(int x, int y, int w, int h)
{
    int i;
    EMISOR_HUMO **tmp_p = &prt1er_emisor;
    EMISOR_HUMO *tmp = NULL;

    while (*tmp_p) {

        tmp = *tmp_p;

        /* DEBUG: aqui agrego particulas de humo */
        // fueguito...
        if (rand()%100 < 80)
                agrega_humo( fixadd(tmp->x, itofix(rand_ex(-5,5))) ,
                             fixadd(tmp->y, itofix(rand_ex(-5,5))) ,
                             ftofix(rand_ex(-150,150) / 100.0),
                             ftofix(rand_ex(-150,150) / 100.0),
                             rand_ex(5, 15),
                             makecol(255,rand()%255,0),
                             itofix(rand_ex(2,6)),
                             ftofix(rand_ex(0, 10) / 100.0));

         // agregar un 'humo' hacia la derecha y arriba
        if (rand()%100 < 95)
               {
                i = rand_ex(0,96); // gris oscuro
                agrega_humo( fixadd(tmp->x, itofix(rand_ex(-5,5))),
                             fixadd(tmp->y, itofix(rand_ex(-5,5))),
                             ftofix(rand_ex(10,200) / 100.0),
                             ftofix(-1.0 * (rand_ex(50,300) / 100.0)),
                             rand_ex(10, 50),
                             makecol(i,i,i),
                             itofix(rand_ex(3,10)),
                             ftofix(rand_ex(0, 25) / 100.0));
               }

        tmp->vida--;

        /* verificar si estan fuera de pantalla (da 40 pixeles de margen para
           permitir que salga totalmente de pantalla) */
         // NOTA: no los elimino por arriba, sino los edificos aparecen 'apagados'
//      if (tmp->y < itofix(y-80)) tmp->vida = -1;
        if (tmp->y > itofix(y+h+80)) tmp->vida = -1;
        if (tmp->x < itofix(x-80)) tmp->vida = -1;
        if (tmp->x > itofix(x+w+80)) tmp->vida = -1;

        if (tmp->vida < 0) {
          /*  murio, eliminar!!! */
          *tmp_p = tmp->next;
          free(tmp);
        } else {
            tmp_p = &tmp->next; /* siguiente por favor! */
        }
    }
}

/* Esta funcion se debe llamar cuando no se precise
   mas la lista de emisores de humo
   Libera la RAM usada y reinicia la lista
   */
void liberar_emisores_humo() {
    EMISOR_HUMO *tmp = prt1er_emisor;
    prt1er_emisor = NULL;

    while (tmp) {
        EMISOR_HUMO *next = tmp->next;
        free(tmp);
        tmp = next;
    }
}


//======================================================================== 
//
// ---------------------- FUNCIONES DE PARTICULAS DE HUMO EN SI ----------
//
//========================================================================

/* Esta funcion agrega humo a la lista enlazada [al principio].
   Devuelve puntero a la particula recien creada.
   Los parametros son los parametros de la particula
   Si vida <= 0, NO se crea la particula... (return derecho...)
   */
HUMO *agrega_humo( fixed x,  fixed y,
                   fixed dx, fixed dy,
                   int vida,
                   int col, fixed r, fixed ri)
{
    HUMO *nueva = NULL;

    if (vida <= 0) return NULL; /* ni pierdo el tiempo... */

    if (nivel_detalle < 2) return NULL; // nivel de detalle minimo, no hacer nada

    nueva =malloc(sizeof(HUMO));
    nueva->next = prt_1er_humo;
    prt_1er_humo = nueva;

    if (nueva != NULL) /* si el malloc funciono, seteo los datos... */
    {
        nueva->x = x;
        nueva->y = y;
        nueva->dx = dx;
        nueva->dy = dy;
        nueva->vida = vida;
        nueva->col = col;
        nueva->r = r;
        nueva->ri = ri;
    }

    return nueva;
}

/*
   Esta funcion actualiza la logica (mueve) el humo
   tambien las elimina si vida < 0 o si estan fuera de pantalla
   Pasar en x, y la posicion de scroll en pantalla
         en w, h el ancho y alto de la pantalla
   */
void mover_humo(int x, int y, int w, int h)
{
    HUMO **tmp_p = &prt_1er_humo;
    HUMO *tmp = NULL;

    while (*tmp_p) {

        tmp = *tmp_p;

        /* aqui muevo la particula */
        tmp->x = fixadd(tmp->x, tmp->dx);

        // efecto 'sacudida' para simular el humo...
        tmp->x = fixadd(tmp->x, ftofix( rand_ex(-200, 200 ) / 100.0 ) );

        tmp->y = fixadd(tmp->y, tmp->dy);
        tmp->vida--;

        // aumentar el radio
        tmp->r = fixadd(tmp->r, tmp->ri);
        if (fixtoi(tmp->r) < 1) tmp->vida = -1;


        /* DEBUG: verificar si estan fuera de pantalla (da 15 pixeles de margen para
           permitir que la particula salga totalmente de pantalla) */
        if (tmp->y < itofix(y-15)) tmp->vida = -1;
        if (tmp->y > itofix(y+h+15)) tmp->vida = -1;
        if (tmp->x < itofix(x-15)) tmp->vida = -1;
        if (tmp->x > itofix(x+w+15)) tmp->vida = -1;

        if (tmp->vida < 0) {
          /* la particula murio, eliminar!!! */
          *tmp_p = tmp->next;
          free(tmp);
        } else {
            tmp_p = &tmp->next; /* siguiente por favor! */
        }
    }
}

/*
   Esta funcion dibuja el humo en el bitmap bmp
   Las dibuja desplazadas x,y
   */
void dibujar_humo(BITMAP *bmp, int x, int y)
{
    HUMO *tmp = prt_1er_humo;
    BITMAP *tbmp = NULL; // bitmap temporal para el dibujado de sprites escalados
    
    if (nivel_detalle > 5)
                drawing_mode(DRAW_MODE_TRANS, NULL, 0,0); // usar transparencias (queda joya!)

    while (tmp) {
      /* dibujar */

      // si el nivel de detalle es maximo, usara sprites transparentes
      // escalados, lo cual es *muy* lento!
      if (nivel_detalle >= 10 && fixtoi(tmp->r) > 5 && humo_spr != NULL)
        {
          tbmp = create_bitmap(fixtoi(tmp->r)*2,fixtoi(tmp->r)*2);
          if (tbmp != NULL)
          {
            stretch_blit(humo_spr, tbmp, 0, 0, humo_spr->w, humo_spr->h,0,0,tbmp->w, tbmp->h);
            draw_trans_sprite(bmp, tbmp, fixtoi(tmp->x)-x-(tbmp->w/2), fixtoi(tmp->y)-y-(tbmp->h/2));
            destroy_bitmap(tbmp);
            tbmp = NULL;
          }
        }
      else
        { circlefill(bmp, fixtoi(tmp->x)-x, fixtoi(tmp->y)-y, fixtoi(tmp->r), tmp->col);}
        
        tmp = tmp->next; /* proximo... */
      }

    solid_mode();
}

/* Esta funcion se debe llamar cuando no se precise mas el humo
   Libera la RAM usada y reinicia la lista
   */
void liberar_humo()
{
    HUMO *tmp = prt_1er_humo;
    prt_1er_humo = NULL;

    while (tmp) {
        HUMO *next = tmp->next;
        free(tmp);
        tmp = next;
    }

}

#endif
