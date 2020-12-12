// -------------------------------------------------------- 
// sombras.c
// -------------------------------------------------------- 
// Copyright (c) Kronoman 
// En memoria de mi querido padre 
// -------------------------------------------------------- 
// Genera las sombras del juego y dibuja sombras en pantalla tambien.
// Bastante simple. :^P
// Lo afecta el nivel de detalle.
// -------------------------------------------------------- 
#ifndef SOMBRAS_C
#define SOMBRAS_C

#include "allegro.h"
#include "sombras.h"

#include "global.h"

/* Genera una sombra del origen en destino
   origen y destino son dos bitmaps, PREVIAMENTE alocados
   destino sera borrado a negro 0, y luego, se dibuja la sombra con gris de 32 RGB
   TENER LA PALETA CARGADA, O LOS COLORES VAN A SALIR MAL
   LA SOMBRA ES ESCALADA AL TAMA~O DE DESTINO!
   */
void hacer_sombra(BITMAP *origen, BITMAP *destino) 
{
   int x, y, c;
   BITMAP *tmp; 

   tmp = create_bitmap(origen->w, origen->h); 
   if (tmp == NULL) return; /* fallo! */

   clear_bitmap(destino);
   clear_bitmap(tmp);
   
   c = makecol(32,32,32); /* color de sombra */

   solid_mode(); /* dibujar solido... */

   for (x=0; x < origen->w; x++)
       for (y=0; y < origen->h; y++)
           if (getpixel(origen, x, y) > 0 ) putpixel(tmp, x, y, c);

   stretch_blit(tmp, destino, 0, 0, tmp->w, tmp->h, 0, 0, destino->w, destino->h);

   destroy_bitmap(tmp);

}

/* Coloca la sombra de un jugador/enemigo

   NOTA: codigo original era 
   alterando la posicion de la sombra de acuerdo a la posicion del enemigo
   en una pantalla de  600x480

   Actualmente (16/12/03) la sombra se dibuja hacia la izquierda abajo, para
   concordar con el foco de luz que le da sombra a los edificios
*/
void colocar_sombra(BITMAP *bmp, BITMAP *sombra, int px, int py)
{
	/* if (nivel_detalle > 5)  draw_trans_sprite(bmp, sombra, px+(300-px)/8, py+(400-py)/10); */
	if (nivel_detalle > 5)  draw_trans_sprite(bmp, sombra, px-(sombra->w*0.15)-10, py+(sombra->h*0.15)+10);
}

#endif
