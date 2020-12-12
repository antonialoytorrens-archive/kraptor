// -------------------------------------------------------- 
// combo.c 
// -------------------------------------------------------- 
// Copyright (c) Kronoman 
// En memoria de mi querido padre 
// -------------------------------------------------------- 
// Esto contiene combos para agregar explosiones+particulas
// y tocar los sonidos, musica, etc
// -------------------------------------------------------- 
#ifndef COMBO_H
#define COMBO_H

#include "explos.h"

/* prototipos */
void poner_explosion_nave(int x, int y, int c, int r, int bmpp);
void poner_muchas_particulas( int x,  int y,
                              int dx, int dy,
                              int vida,
                              int col, int r, int t,
                              BITMAP *spr,
                              int cant );

void pone_explo_pixel(EXPLOSION **explo,
                      int x, int y,
                      int r, int v,
                      fixed v2);
#endif
