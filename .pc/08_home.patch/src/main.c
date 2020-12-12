// --------------------------------------------------------
// Kraptor - Call of the freedom
// --------------------------------------------------------
// main.c
// --------------------------------------------------------
// Copyright (c) 2002-2004, Kronoman
// En memoria de mi querido padre
// --------------------------------------------------------
// Este modulo es el 'arranque' del programa
// Inicia el video, carga los datos, etc...
// --------------------------------------------------------
// Ultima revision: 18/ENERO/2004

#ifndef MAIN_C
#define MAIN_C

// --------------------------------------------------------
// Inclusiones
// --------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <allegro.h>
#include <aldumb.h> /* DUMB: musica MOD, XM, etc */
#include "main.h"
#include "error.h"
#include "game.h"
#include "data.h"
#include "jugador.h"
#include "global.h"
#include "menu.h"
#include "mapa.h"
#include "config.h"


int main(int argc, char *argv[] )
{
    int i1; // para el for de los parametros
    int rx = 640, ry = 480; // para permitir que el usuario elija la resolucion
    int vid_card = GFX_AUTODETECT; // placa de video por defecto

    int snd = TRUE; // quiere sonido
    lenguaje[0]='e';
    lenguaje[1]='s';
    lenguaje[2]= '\0';

  /* Iniciar Allegro y el hardware */
  allegro_init();
  atexit(&dumb_exit);

  /* setear allegro al idioma configurado, default = en (ingles) */
  strncpy(lenguaje, get_config_string("system", "language", "en"), 2);
  reload_config_texts(lenguaje);

  /* configuracion de DUMB, por default, CASI la mas baja (1), la mas baja es 0 :^) */
  dumb_resampling_quality = get_config_int("sound", "dumb_resampling_quality", 1);
  dumb_it_max_to_mix = get_config_int("sound", "dumb_it_max_to_mix", 8);

  /* registro los IT, XM y S3M para cargarlos con DUMB */
      dumb_register_dat_it(DUMB_DAT_IT);
      dumb_register_dat_xm(DUMB_DAT_XM);
      dumb_register_dat_s3m(DUMB_DAT_S3M);


  srand(time(0));
  if (install_timer() != 0) levantar_error("ERROR: Fallo el inicio del temporizador!");
  if (install_keyboard() != 0) levantar_error("ERROR: Imposible acceder al teclado!");

  install_mouse(); /* Tambien uso el mouse para los menus! */
 
    // --------------------------------------------------------
    // Interpretar parametros
    // --------------------------------------------------------
    for (i1=1; i1<argc; i1++) {

    // -steroids (modo 'rapido', el scroll pasa 5 veces mas rapido!)
    if (stricmp(argv[i1], "-steroids") == 0) scroll_mapa_speed = 5;

    // -crazy_speed (modo 'super rapido', el scroll pasa 10 veces mas rapido!)
    if (stricmp(argv[i1], "-crazy_speed") == 0) scroll_mapa_speed = 10;

    // -kronoman (cheat: modo DIOS - no muere nunca)
    if (stricmp(argv[i1], "-kronoman") == 0) cheat_god_mode = TRUE;

    // -safe (modo seguro, video pedorro y sin sonido)
    if (stricmp(argv[i1], "-safe") == 0) { rx = 320; ry = 200; snd = FALSE; vid_card = GFX_SAFE; }

    // video: modo en ventana (solo algunas plataformas)
    if (stricmp(argv[i1], "-windowscreen")== 0)  vid_card = GFX_AUTODETECT_WINDOWED;
    // video: modo en pantalla completa
    if (stricmp(argv[i1], "-fullscreen") == 0) vid_card = GFX_AUTODETECT_FULLSCREEN;

    // video: video automatico
    if (stricmp(argv[i1], "-autoscreen") == 0) vid_card = GFX_AUTODETECT;


    // video: -320x200 (320x200)
      if (stricmp(argv[i1], "-320x200") == 0) { rx = 320; ry = 200; }

    // video: -320x240 (320x240)
      if (stricmp(argv[i1], "-320x240") == 0) { rx = 320; ry = 240; }

    // video:  -640x480 (640x480)
      if (stricmp(argv[i1], "-640x480") == 0) { rx = 640; ry = 480; }

    //  video: -800x600 (800x600)
      if (stricmp(argv[i1], "-800x600") == 0) { rx = 800; ry = 600; }

    //  video: -1024x768 (1024x768)
      if (stricmp(argv[i1], "-1024x768") == 0) { rx = 1024; ry = 768; }

    // -nosound - sin sonido
      if (stricmp(argv[i1], "-nosound") == 0) { snd=FALSE; }

    // ayuda
      if (stricmp(argv[i1],"-h")==0 || stricmp(argv[i1],"-?") == 0 || stricmp(argv[i1],"--h") == 0 || stricmp(argv[i1],"/?") == 0)
         {
         /* DEBUG: falta mostrar la ayuda!!! */
         exit(0);
         }

    // DEBUG: falta un -v para la version! */

    } // fin for


/* Iniciar video */
set_color_depth(8); /* setear a 8bpp, en otros modos, no ANDA! */

// DEBUG: probar con GFX_SAFE que tambien intenta setear el rx, ry, pero anda mejor en Linux
if (set_gfx_mode(vid_card, rx, ry, 0, 0))
  if (set_gfx_mode(GFX_SAFE, rx, ry, 0, 0))
    if (set_gfx_mode(GFX_SAFE, 640, 480, 0, 0))
       if (set_gfx_mode(GFX_SAFE, 320, 200, 0, 0))
           levantar_error("ERROR: imposible utilizar algun modo grafico de 8bpp!");

// chequear el 'depth' de color (porque GFX_SAFE puede meter otro que no sea 8bpp)
// if (bitmap_color_depth(screen) != 8); levantar_error("ERROR: el modo grafico no es de 8bpp!");

clear(screen);
textout(screen, font,"Wait... loading...",0,SCREEN_H - text_height(font)*2, makecol(255,255,255));

// textout(screen, font,"Espere... iniciando programa...",0,0, makecol(255,255,255));
// textout(screen, font,"Si el juego se detiene aqui, intente reiniciarlo con el parametro -nosound.",0,SCREEN_H - text_height(font)*2 , makecol(255,255,255));
// textout(screen, font,"Utilize el parametro -h para mas informacion.",0,SCREEN_H - text_height(font), makecol(255,255,255));

set_teclado_default();

/* DEBUG: Joystick! wow!! */
install_joystick(JOY_TYPE_AUTODETECT);

// SONIDO
// reserve_voices(8, 0); // 8 voces de sonido

// volumen por voz (def: 2, maximo volumen: 0)
// set_volume_per_voice(1); // puede causar distorsion, ojo! causa DISTORSION!

if (snd)
   {
   if (install_sound(DIGI_AUTODETECT, MIDI_NONE, NULL) == -1)
     { levantar_error("ERROR: no se puede configurar el sonido.\nPara desactivarlo use el parametro -nosound!");}

   driver_de_sonido = TRUE;
   }
else
  {
    install_sound(DIGI_NONE, MIDI_NONE, NULL);
    quiere_snd = FALSE;
    quiere_musica = FALSE;
    driver_de_sonido = FALSE;
  } // sin sonido

// volumen al mango
set_volume(255,-1);

// instalo colision de mascaras [IMPORTANTE]
install_pmask();

// Hacer backup del font default
font_backup = font;

cargar_datos_principales(); /* Cargar datos del programa */

// cargar configuracion especifica de Kraptor
cargar_configuracion();

/* menu principal */
realizar_menu_principal();

// aqui salvar la configuracion especifica de Kraptor!!!
salvar_configuracion();

return 0;
}
END_OF_MAIN();

#endif
