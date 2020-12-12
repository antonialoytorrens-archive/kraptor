// -------------------------------------------------------- 
// sonido.h 
// -------------------------------------------------------- 
// Copyright (c) Kronoman 
// En memoria de mi querido padre 
// -------------------------------------------------------- 
// Modulo de sonido y musica
// -------------------------------------------------------- 
#ifndef SONIDO_H
#define SONIDO_H
#include <aldumb.h> /* DUMB: libreria de sonido */


// Prototipos

// Sonido
void tocar_sonido_paneado(int x, SAMPLE *spl, int vol, int freq);
void tocar_sonido(SAMPLE *spl, int vol, int pan, int freq);


// Musica
void musica_stop();
int musica_play(DUH *duh);
void ejecutar_poll_musica();
void reajustar_volumen_musica();
void pausar_musica();
void continuar_musica();

#endif
