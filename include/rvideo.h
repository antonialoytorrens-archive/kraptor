// ----------------------------------------------------------------
// rvideo.h
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

#ifndef RVIDEO_H
#define RVIDEO_H
#include <allegro.h>

// Globales de configuracion
extern char rvideo_filen[1024];
extern int rvideo_cadafsave;
extern int rvideo_step;
extern int rvideo_f_actual;
extern int rvideo_is_recording;
extern int rvideo_resize;
extern int rvideo_w, rvideo_h;

// Prototipos
void init_rvideo();
void rvideo_record(BITMAP *bmp, RGB *pal);
#endif

