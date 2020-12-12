/* --------------------------------------------------------
   global.h
   Variables globales, y simbolo de debug
 -------------------------------------------------------- */
#ifndef GLOBAL_H
#define GLOBAL_H

#include <aldumb.h>

/* Info de un nivel */
typedef struct NIVEL_T {
  char titulo[80]; /* titulo nivel */
  char texto[300]; /* introduccion de texto al nivel */
  char cine_in[1024]; /* cinematica de comienzo nivel */
  char cine_out[1024]; /* cinematica de final nivel */
  char level_dat[1024]; /* DAT del nivel */
  int clima_c, clima_t, clima_d; // info de clima, cantidad, tipo, direccion
  DUH *musica; /* musica del nivel */
} NIVEL_T;

/* Globales exportadas */
extern char lenguaje[3]; // lenguaje a usar, ej : es = espa~ol
extern int nivel_detalle;
extern int detalle_automatico;
extern int nivel_actual;
extern int nivel_de_dificultad;
extern int cheat_god_mode;
extern NIVEL_T info_nivel;

extern char game_over_cine[1024]; /* cinematica de game over */
extern char game_intro_cine[1024]; /* cinematica de introduccion */
extern int driver_de_sonido;
extern int quiere_snd;
extern int volumen_sonido;
extern int quiere_musica;
extern int quiere_videos;
extern int volumen_musica;
extern int salir_del_juego;
extern int paso_de_nivel;
extern PALETTE pal_game;
extern COLOR_MAP tabla_transparencia;
extern RGB_MAP tabla_RGB;
extern int KRONO_QUIERE_DEBUG;
extern FONT *font_backup;
extern FONT *hud_font;
extern int quiere_usar_joystick;
extern int numero_de_joystick;
extern int quiere_usar_mouse;
extern int mouse_velocidad;
#endif
