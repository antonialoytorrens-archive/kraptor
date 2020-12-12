// -------------------------------------------------------- 
// sonido.c 
// -------------------------------------------------------- 
// Copyright (c) Kronoman 
// En memoria de mi querido padre 
// -------------------------------------------------------- 
// Modulo de sonido y musica
// -------------------------------------------------------- 
#ifndef SONIDO_C
#define SONIDO_C

#include <allegro.h>

#include <aldumb.h> /* DUMB: libreria de sonido */

#include "sonido.h"
#include "global.h"
#include "kfbuffer.h"

/* player para la musica */
static AL_DUH_PLAYER *musica_dp = NULL;


/*
Esta peque~a rutina toca un sonido
Pasarle el volumen
El paneo stereo es calculado de acuerdo a la posicion del objeto
parametros:
x = posicion x del objeto que emite el sonido (para hacer el panning correcto)
spl = puntero al sample a tocar
vol = volumen
freq = frecuencia (velocidad, 1000 = 1, 2000 = 2, etc)

*/
void tocar_sonido_paneado(int x, SAMPLE *spl, int vol, int freq)
{
int pan = 0;

if (!(quiere_snd && driver_de_sonido)) return; /* no esta usando el sonido */
if (spl == NULL) return; /* evito errores */

pan = ( x * 255 ) / ANCHO_FB;

tocar_sonido(spl, vol, pan, freq);

}

/*
Esta funcion toca un sonido, es equivalente a play_sample,
pero ajusta el volumen de manera relativa a la variable global
llamada 'volumen_sonido'.
NOTA: se deberia USAR SIEMPRE esta funcion para enviar un sonido
a la placa de sonido, de esa manera, se respetan los settings del usuario
*/
void tocar_sonido(SAMPLE *spl, int vol, int pan, int freq)
{
int vol2 = 255; 

if (!(quiere_snd && driver_de_sonido)) return; /* no esta usando el sonido */
if (spl == NULL) return; /* evito errores */

 vol2 = (vol * volumen_sonido) / 255;
 if (vol2 > 0) play_sample(spl, vol2, pan, freq, 0);
}



/* Rutina para comenzar a tocar un archivo de DUMB
pasarle el puntero al archivo
devuelve 0 si todo OK, -1 si falla
*/
int musica_play(DUH *duh)
{
float vol = 1.0;

if (duh == NULL) return -1;

if (musica_dp != NULL)  musica_stop();

if (!quiere_musica) return 0;

vol = (float)volumen_musica / 255.0;

/* ajusta el stereo y los khz de acuerdo al nivel de detalle */
 musica_dp = al_start_duh(duh,
                          (nivel_detalle > 5) ? 2 : 1,
                          0,
                          vol,
                          2048,
                          11025);

if (musica_dp == NULL) return -1;

return 0; 
}

/* Rutina para parar DEFINITIVAMENTE de tocar la musica */
void musica_stop()
{
if (musica_dp == NULL) return;

    al_stop_duh(musica_dp);
    musica_dp = NULL;
}

/* Llamar a esta funcion MUY seguido, para que la musica se escuche! */
void ejecutar_poll_musica()
{
    if (musica_dp != NULL)  al_poll_duh(musica_dp);
}

/* Llamar a esta funcion luego de cambiar el volumen
de la musica */
void reajustar_volumen_musica()
{
float vol = 1.0;

if (musica_dp == NULL) return;

vol = (float)volumen_musica / 255.0;
 
 al_duh_set_volume(musica_dp, vol);
}

/* Esta funcion pausa la musica */
void pausar_musica()
{
    if (!quiere_musica)
    {
        musica_stop();
        return;
    }

    if (musica_dp == NULL) return;

    al_pause_duh(musica_dp);
}

/* Esta funcion continua la musica */
void continuar_musica()
{
if (!quiere_musica) return;

    if (musica_dp == NULL) return;

    al_resume_duh(musica_dp);
}

#endif
