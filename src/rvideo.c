// ----------------------------------------------------------------
// rvideo.c
// ----------------------------------------------------------------
// Produce un video de un juego a archivos bmp sueltos
// Ideal para generar un video promocional de un juego
// Por Kronoman
// Copyrigth Enero 2003, Kronoman
// En memoria de mi querido padre
// ----------------------------------------------------------------
// Para que esto funcione, debe ser llamado en cada actualizacion
// logica del juego, de esa manera mantiene los fps constantes
// ----------------------------------------------------------------


#include <allegro.h>
#include <stdio.h>
#include "rvideo.h"

// Globales de configuracion
char rvideo_filen[1024]; // prefijo del video, setear a algo antes de empezar!
int rvideo_cadafsave = 10; // cada cuantas llamadas salvar el frame (1 =todas, 2 = la mitad, etc)
int rvideo_step = 0; // frame actual del step antes de salvar el frame (cuando esto = rvideo_cadafsave, salva el frame)
int rvideo_f_actual = 0; // frame actual de grabacion
int rvideo_is_recording = FALSE; // estoy grabando? (permite hacer pausas, etc)
int rvideo_resize = TRUE; // reacomodar el tama~o de salida? SI/NO
int rvideo_w = 320, rvideo_h = 200;

// Funcion de inicializacion, solo por seguridad... :)
void init_rvideo()
{
        rvideo_cadafsave = 10; // default 1/10 de los fps logicos del juego
        rvideo_step = 0;
        rvideo_f_actual = 1; // empezo a grabar de 0001
        sprintf(rvideo_filen, "rvideo"); // prefijo default
        rvideo_is_recording = FALSE;
        rvideo_resize = TRUE;
        rvideo_w = 320;
        rvideo_h = 200;
}

// Funcion de grabacion, llamar en las actualizaciones logicas del juego
// pasarle el bitmap que contiene la imagen, y la paleta
void rvideo_record(BITMAP *bmp, RGB *pal)
{
char filename[2048]; // archivo a salvar
BITMAP *tmp = NULL;

if (!rvideo_is_recording) return; // no estamos grabando
rvideo_step++;
if (rvideo_step < rvideo_cadafsave) return; // todavia no salvar

sprintf(filename, "%s%04d.bmp", rvideo_filen, rvideo_f_actual);

rvideo_f_actual++;
// salvar frame
if (rvideo_resize && rvideo_w != bmp->w && rvideo_h != bmp->h)
   {
   // el frame debe ser reajustado en tama~o
   tmp = create_bitmap(rvideo_w, rvideo_h);
   if (tmp == NULL) return;

   stretch_blit(bmp, tmp, 0, 0, bmp->w, bmp->h, 0, 0, tmp->w, tmp->h);
   save_bmp(filename, tmp, pal); // salvar 

   if (tmp != NULL) destroy_bitmap(tmp);
   }
else
  {
  // frame 'tal cual'
   save_bmp(filename, bmp, pal); // salvar
  }

rvideo_step = 0;
}

