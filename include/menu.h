/*
 --------------------------------------------------------
 menu.h
 --------------------------------------------------------
 Sistema de menu de Kraptor (al estilo de Unre*l)
 --------------------------------------------------------
*/
#ifndef K_MENU_H
#define K_MENU_H

#include <allegro.h>

/* Seleccion de menu */
#define MNU_S_NEW_GAME    1
#define MNU_S_LOAD_GAME   2
#define MNU_S_QUIT_GAME   3

#define MNU_S_CFG_SND     4
#define MNU_S_CFG_CTRL    5
#define MNU_S_CFG_DETAIL  6
#define MNU_S_CFG_VIDEO   7

#define MNU_S_CFG_SPANISH 8
#define MNU_S_CFG_ENGLISH 9

#define MNU_S_VER_HLP     10
#define MNU_S_VER_ABOUT   11

/* bitmap de fondo, y paleta de colores y musica */
extern BITMAP *bmp_menu_main;
extern PALETTE pal_menu_main;


extern char *texto_ayuda_juego; /* puntero a la ayuda del juego */
extern char *texto_acerca_de_juego; /* acerca de... */
extern BITMAP *acerca_de_bmp; /* bitmap acerca de... */

/* Prototipos */
void realizar_menu_principal(); /* entrada para el menu principal */
int modificar_sonido_mnu(void); /* menu para volumen de sonido/musica */
int mod_teclado_jugar(void); /* menu para modificar el teclado */
int mod_detalle_mnu(void); /* menu para modificar los detalles */
int menu_configuracion_al_vuelo(void); /* menu de sonido, teclado, detalle */
int ayuda_proggy_mnu(); /* ayuda */

#endif
