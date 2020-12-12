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
#ifndef KFBUFFER_H
#define KFBUFFER_H

#include <allegro.h>

/* ancho y alto de renderizado del area de juego */
#define ANCHO_FB    600
#define ALTO_FB     480

/* ancho y alto area del buffer total */
#define ANCHO_RFB   640
#define ALTO_RFB    480


/* globales exportadas */
extern BITMAP *kfbufferbmp;

/* prototipos */
void enviar_kfbuffer2screen();
void interlazar_kfbuffer(int r, int g, int b);

#endif
