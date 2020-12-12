// -------------------------------------------------------- 
// combo.c 
// -------------------------------------------------------- 
// Copyright (c) Kronoman 
// En memoria de mi querido padre 
// -------------------------------------------------------- 
// Esto contiene combos para agregar explosiones+particulas
// y tocar los sonidos, musica, etc
// -------------------------------------------------------- 
#ifndef COMBO_C
#define COMBO_C

#include "allegro.h"
#include "partic.h"
#include "explos.h"
#include "azar.h"
#include "combo.h"
#include "global.h"
/*
   agrega muchas particulas  a la vez
   cant es la cantidad
   si col < 0, usara colores en gama de fuego (naranjas)
*/
void poner_muchas_particulas( int x,  int y,
                              int dx, int dy,
                              int vida,
                              int col, int r, int t,
                              BITMAP *spr,
                              int cant )
{
int i, c2;
c2 = col;

/* si el nivel de detalle es bajo, limita la cantidad de particulas a 10 por round */
if (nivel_detalle < 4 && cant > 10) cant = 10;
if (nivel_detalle < 2 && cant > 5) cant = 5;

for (i = 0; i < cant; i++)
        {
        if (col < 0) c2  = makecol(255,rand()%255,0);

           agrega_particula( itofix(x),  itofix(y),
                             ftofix((rand_ex(-100*dx, 100*dx))/100.0), ftofix((rand_ex(-100*dy, 100*dy))/100.0),
                             rand_ex(vida/4, vida),
                             c2, rand_ex(r/2,r)+1, t,0,0,
                             spr );
        }
}

/*
 agrega una explosion y particulas en x,y
 con cantidad c y radio r (tambien indica duracion particulas * 2)
 las particulas son trozos de nave y fuego
 el radio para la explosion es el ancho/alto en pixeles...
 si bmpp es !=0, pondra chispas con bitmaps tambien (Explosion final de algo grande)
 */
void poner_explosion_nave(int x, int y, int c, int r, int bmpp)
{

/* DEBUG: funcion mal hecha, no agrega bitmaps, solo poligonos */
poner_muchas_particulas(  x,   y,
                          5, 5,
                         r*2,
                         -1, 4, 3,
                         NULL,
                         c );


pone_explo_pixel(&ptr_explo_arriba, x, y, r, r*2, ftofix(0.01));

if (bmpp && nivel_detalle > 7 ) { /* particulas con bitmaps */
    int z1;
    for (z1 = 0; z1 < 3; z1++)
        poner_muchas_particulas(  x,   y,
                                  5, 5,
                                 r*rand_ex(2, 4),
                                 -1, 4, 3,
                                 particula_cache_bmp[z1],
                                 c/rand_ex(2,3) ); /* poner 1/3 o 1/2 de particulas */
    }

}

/*
Esta funcion coloca una explosion en x,y
con radio r, el radio expresado en PIXELS!
v es la vida, v2 la combustion
NOTA: la explosion se limita en tama~o para no sobrecargar el juego
NOTA: pasarle el puntero a la capa de explosion con &puntero! 
*/
void pone_explo_pixel(EXPLOSION **explo, int x, int y, int r, int v, fixed v2)
{
float rpx; /* para calculos luego */
BITMAP *tmp;

/* Limitar a 150 pixels de radio */
if (r > 150) r= 150;

if (nivel_detalle < 2 && r > 50) r = 50; /* el detalle limita el tama~o tambien */

/* explosion, calculo la proporcion entre pixeles y el tama~o del
   sprite de la explosion y luego la agrego */

tmp = explo_cache_bmp[rand()%3];
rpx = (float)r / (float)tmp->w;

agrega_explosion( explo,
                  itofix(x),  itofix(y),
                  0,0,
                  v,
                  ftofix(rpx), v2,
                  rand()%255,
                  tmp );

}

#endif
