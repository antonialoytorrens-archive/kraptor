// -------------------------------------------------------- 
// bomba.c
// -------------------------------------------------------- 
// Copyright (c) Kronoman 
// En memoria de mi querido padre 
// -------------------------------------------------------- 
// Este modulo contiene todo lo relacionado con las bombas
// del jugador...
// -------------------------------------------------------- 
#ifndef BOMBA_C
#define BOMBA_C

#include <allegro.h>
#include "bomba.h"
#include "mapa.h"
#include "sonido.h"
#include "explos.h"
#include "kfbuffer.h"
#include "azar.h"

/* globales exportadas, setearlas al lanzar una bomba, ver jugador.c */
int bomba_esta_activa = FALSE; // esta la bomba funcionando en este momento?
int bomba_detonacion = 0; // tiempo de detonacion (30 = 1 segundo explotando)
SAMPLE *bomba_sonido = NULL; // sonido de detonacion

/* mueve la bomba */
void mover_bomba()
{
int i;
if (!bomba_esta_activa) return;

// sonido de explosiones al azar...
if (rand()%100 < 25)
            tocar_sonido_paneado(rand()%ANCHO_FB,
                                 explo_cache_snd[rand()%3],
                                 rand_ex(128, 200),
                                 rand_ex(800, 1300));

bomba_detonacion--;
if (bomba_detonacion <= 0)
   {
    /* sonido estruendoso de explosion */
    if (rand()%100 < 95)
       for (i = 0; i < rand()%3+1; i++)
            tocar_sonido_paneado(rand()%ANCHO_FB,
                                 explo_cache_snd[rand()%3],
                                 rand_ex(196, 256),
                                 rand_ex(800, 1300));

    bomba_esta_activa = FALSE;
    bomba_detonacion = 0;
   }
}

/* dibuja la explosion de la bomba, actualmente, todo blanco... horrible! */
void dibujar_bomba(BITMAP *bmp)
{
if (!bomba_esta_activa) return;

clear_to_color(bmp, makecol(255,255,255) );

}

/* produce una detonacion total del piso del juego,
   pasarle en x el 'scroll_mapa'  */
void detonar_totalmente_el_piso(int y)
{
int x1,y1;
/* DEBUG: esto detona con retroactividad, es decir todo el mapa hacia atras! */
for (y1 = y / H_GR; y1 < GRILLA_H; y1++)
    for (x1 = 0; x1 < GRILLA_W; x1++)
           explotar_fondo(x1, y1, rand()%100+100);

}

#endif
