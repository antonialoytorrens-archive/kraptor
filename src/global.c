/*
 global.c
 Algunas globales usadas en el programa
 --------------------------------------------------------
 Copyright (c) 2002, Kronoman
 En memoria de mi querido padre
 --------------------------------------------------------
*/
#ifndef GLOBAL_C
#define GLOBAL_C

#include "allegro.h"
#include "global.h" // cantidades maximas definidas aqui

/* Quiere ver informacion de DEBUG? (!= 0) = si */
int KRONO_QUIERE_DEBUG = 0;

char lenguaje[3]; // codigo de lenguaje a usar, 2 letras: ej : es = espa~ol

/* Fonts: */
FONT *font_backup = NULL; // esto contendra el puntero a 'font' de allegro, para poder restaurarlo
FONT *hud_font  = NULL; // font para dibujar el 'hud' en el juego

/* Detalles graficos del juego 0..10 (min..max) */
int nivel_detalle = 10;
int detalle_automatico = 0; // desea que el detalle se ajuste automaticamente?

/* Nivel actual */
int nivel_actual = 0;

/* Nivel de dificultad actual: 0 = facil, 1 = medio, 2 = dificil */
int nivel_de_dificultad = 1; // skill level

/* TRUCO : CHEAT - si esto es TRUE, el jugador no muere nunca */
int cheat_god_mode = FALSE;


char game_over_cine[1024]; /* cinematica de game over */
char game_intro_cine[1024]; /* cinematica de introduccion */

/* Datos del nivel */
NIVEL_T info_nivel;

/* Esta instalado el driver de sonido? */
int driver_de_sonido = FALSE;

/* quiere sonido? */
int quiere_snd = TRUE;

/* volumen del sonido 0..255 */
int volumen_sonido = 255;

/* quiere musica */
int quiere_musica = TRUE;

/* quiere videos */
int quiere_videos = TRUE;

/* volumen de la musica 0..255 */
int volumen_musica = 255;

/* Salir del juego? */
int salir_del_juego = FALSE;

/* paso de nivel */
int paso_de_nivel = FALSE;

/* Paleta del juego */
PALETTE pal_game;

/* Mapa de transparencia */
COLOR_MAP tabla_transparencia;

/* Mapa RGB a 8 bits */
RGB_MAP tabla_RGB;



/* quiere usar el joystick como controlador ? */
int quiere_usar_joystick = FALSE;
/* que numero de joystick quiere usar? (usualmente, 0) */
int numero_de_joystick = 0;

/* quiere usar el mouse como controlador ? */
int quiere_usar_mouse = FALSE;
int mouse_velocidad = 16; // cuanto mayor el numero, mas sensible el mouse, rango: 0..32
#endif
