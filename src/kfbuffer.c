/*
  Todo el sistema trabaja renderizando en un bitmap
  de un tama~o fijo, el cual luego se estira al tama~o de pantalla
  Este archivo contiene el buffer, y los parametros de su tama~o
  porque varias rutinas los necesitan, especialmente
  la de descartar enemigos cuando salen por la parte inferior de pantalla

   -------------------------------------------------------- 
   Copyright (c) Kronoman 
   En memoria de mi querido padre 
   -------------------------------------------------------- 
  */
#ifndef KFBUFFER_C
#define KFBUFFER_C

#include "allegro.h"
#include "kfbuffer.h"

/* globales */

/* BITMAP de dibujado, aqui debe dibujar toda la salida del programa */
BITMAP *kfbufferbmp = NULL;

/* Funcion que oscurece el buffer (ya dibujado)
   con un interlazado en el color especificado
   Sirve para boludeces
   */
void interlazar_kfbuffer(int r, int g, int b)
{
int y = 0;
int c = makecol(r,g,b);
for (y = 0; y < kfbufferbmp->h; y+=2)
    line(kfbufferbmp, 0, y, kfbufferbmp->w, y, c);

}

/*
Funcion que envia el buffer a pantalla.
Si el buffer no esta iniciado, lo inicia.
*/
void enviar_kfbuffer2screen()
{
 if (kfbufferbmp == NULL) return; // kfbufferbmp = create_bitmap(ANCHO_RFB, ALTO_RFB);

// acquire_screen(); /* acelera todo en windoze? creo que NO */

 if (ALTO_RFB == SCREEN_H && ANCHO_RFB == SCREEN_W)
    blit(kfbufferbmp, screen, 0, 0, 0, 0, kfbufferbmp->w, kfbufferbmp->h);
 else
   stretch_blit(kfbufferbmp, screen, 0, 0, kfbufferbmp->w, kfbufferbmp->h, 0, 0, SCREEN_W, SCREEN_H);

// release_screen(); /* acordarse de hacer esto si se usa acquire_screen! */
}

#endif
