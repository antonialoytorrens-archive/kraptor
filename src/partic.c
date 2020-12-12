/*-------------------------------------------------------
 partic.c
 -------------------------------------------------------- 
 Copyright (c) 2002, Kronoman 
 En memoria de mi querido padre
 Agosto - 2002
 -------------------------------------------------------- 
 Engine de particulas usando una lista enlazada
 muy sencilla
 --------------------------------------------------------*/ 

#ifndef PARTIC_C
#define PARTIC_C

#include <stdio.h>
#include "allegro.h"

#include "partic.h"


/* DEBUG - estas variables son especificas a Kraptor */

#include "global.h" // para saber el nivel de detalle unicamente...

/* Cache de bmps representando particulas para 'reventar' naves */
BITMAP *particula_cache_bmp[3];

/* esta variable NO es necesaria, solo la uso para
   ver cuantos enemigos hay en memoria, y de esa manera,
   revisar la performance... */
int cant_particulas_debug = 0;


/* --- globales internas --- */

static PARTICULA *prt_1era = NULL; /* puntero al comienzo de la lista de particulas */


/* Esta funcion agrega una nueva particula a la lista enlazada [al principio].
   Devuelve puntero a la particula recien creada.
   Los parametros son los parametros de la particula
   Si vida <= 0, NO se crea la particula... (return derecho...)
   */
PARTICULA *agrega_particula( fixed x,  fixed y,
                             fixed dx, fixed dy,
                             int vida,
                             int col, int r, int t,
                             int transp, fixed rg, 
                             BITMAP *spr )
{
    PARTICULA *nueva = NULL;

    if (vida <= 0) return NULL; /* ni pierdo el tiempo... */

    if (nivel_detalle < 1) return NULL; // nivel de detalle minimo, no hacer nada

    nueva = malloc(sizeof(PARTICULA));
    nueva->next = prt_1era;
    prt_1era = nueva;

    if (nueva != NULL) /* si el malloc funciono, seteo los datos... */
    {
        nueva->x = x;
        nueva->y = y;
        nueva->transp = transp;
        nueva->dx = dx;
        nueva->dy = dy;
        nueva->vida = vida;
        nueva->col = col;
        nueva->r = itofix(r);
        nueva->rg = rg;
        nueva->t = t;
        nueva->spr = spr;
        nueva->rot = itofix(rand()%255); /* rotacion del bitmap */
    }

    return nueva;
}

/*
   Esta funcion actualiza la logica (mueve) las particulas
   tambien las elimina si vida < 0 o si estan fuera de pantalla
   Pasar en x, y la posicion de scroll en pantalla
         en w, h el ancho y alto de la pantalla
   */
void mover_particulas(int x, int y, int w, int h)
{
    PARTICULA **tmp_p = &prt_1era;
    PARTICULA *tmp = NULL;

    cant_particulas_debug = 0; /* DEBUG: innecesario, solo para testear performance */

    while (*tmp_p) {

        cant_particulas_debug++; /* DEBUG: innecesario, solo para testear performance */

        tmp = *tmp_p;

        /* aqui muevo la particula */
        tmp->x = fixadd(tmp->x, tmp->dx);
        tmp->y = fixadd(tmp->y, tmp->dy);
        tmp->vida--;
        
        /* crecer radio si es necesario */
        tmp->r = fixadd ( tmp->r , tmp->rg );
        
        /* girar sprite (si lo hubiere... */
        tmp->rot = fixadd(tmp->rot, itofix(rand()%16)+1); /* girar */
        if (fixtoi(tmp->rot) > 255) tmp->rot = 0;

        /* DEBUG: verificar si estan fuera de pantalla (da 5 pixeles de margen para
           permitir que la particula salga totalmente de pantalla) */
        if (tmp->y < itofix(y-5)) tmp->vida = -1;
        if (tmp->y > itofix(y+h+5)) tmp->vida = -1;
        if (tmp->x < itofix(x-5)) tmp->vida = -1;
        if (tmp->x > itofix(x+w+5)) tmp->vida = -1;

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
   Esta funcion dibuja las particulas en el bitmap bmp
   Las dibuja desplazadas x,y
   */
void dibujar_particulas(BITMAP *bmp, int x, int y) {

    PARTICULA *tmp = prt_1era;

    while (tmp) {

      /* dibujar */
      
      // es transparente, y hay suf nivel de detalle?
      if ((tmp->transp) && (nivel_detalle > 9)) drawing_mode(DRAW_MODE_TRANS, NULL, 0,0);
      
      if (tmp->spr == NULL) {
          switch(tmp->t)
          {
          case 0: /* pixel */
             putpixel(bmp, fixtoi(tmp->x)-x, fixtoi(tmp->y)-y, tmp->col);
          break;

          case 1: /* circulo */
              circlefill(bmp, fixtoi(tmp->x)-x, fixtoi(tmp->y)-y, fixtoi(tmp->r), tmp->col);
          break;

          case 2: /* cuadrado (QU*KE?) */
              rectfill(bmp, fixtoi(tmp->x)-x, fixtoi(tmp->y)-y, fixtoi(tmp->x)+fixtoi(tmp->r)-x,fixtoi(tmp->y)+fixtoi(tmp->r)-y , tmp->col);
          break;

          case 3: /* linea */
             line(bmp, fixtoi(tmp->x)-x, fixtoi(tmp->y)-y, fixtoi(fixadd(tmp->x, tmp->dx))-x, fixtoi(fixadd(tmp->y, tmp->dy))-y,  tmp->col);
          break;

          case 4: /* triangulo */
              triangle(bmp,
                       fixtoi(tmp->x)-x, fixtoi(tmp->y)-y,
                       fixtoi(tmp->x)+fixtoi(tmp->r)-x, fixtoi(tmp->y)+fixtoi(tmp->r)-y,
                       fixtoi(tmp->x)-fixtoi(tmp->r)-x, fixtoi(tmp->y)+fixtoi(tmp->r)-y,
                       tmp->col);
          break;

          default: /* default: */
              /* si no coincide con ninguno, eliminar la particula... sorry */
              tmp->vida = 0;
          break;
          }
      } else { /* tiene un sprite de imagen */
          rotate_sprite(bmp, tmp->spr, fixtoi(tmp->x)-x, fixtoi(tmp->y)-y, tmp->rot);
      }
	
      solid_mode(); // sacar transparencia...
	
      tmp = tmp->next; /* proximo... */
      }
}

/* Esta funcion se debe llamar cuando no se precise mas la lista
   Libera la RAM usada y reinicia la lista
   */
void liberar_lista_particulas() {
    PARTICULA *tmp = prt_1era;
    prt_1era = NULL;

    while (tmp) {
        PARTICULA *next = tmp->next;
        free(tmp);
        tmp = next;
    }

}

#endif
