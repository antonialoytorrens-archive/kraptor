/*
 --------------------------------------------------------
 game.c
 --------------------------------------------------------
 Loop principal del juego
 --------------------------------------------------------
*/
#ifndef GAME_C
#define GAME_C

#include <allegro.h>
#include <limits.h>
#include "shopping.h"
#include "sonido.h"
#include "guiprocs.h"
#include "guitrans.h"
#include "data.h"
#include "global.h"
#include "kfbuffer.h"
#include "error.h"
#include "jugador.h"
#include "enemigo.h"
#include "mapa.h"
#include "partic.h"
#include "explos.h"
#include "captura.h"
#include "cinema.h"
#include "menu.h"
#include "clima.h"
#include "humo.h"
#include "rvideo.h"
#include "bomba.h"
#include "premio.h"

/* Globales */

volatile int speed_counter = 0; /* contador de velocidad para frame skip */
volatile int frame_count, fps; /* para contar los fps en caso de debug... */

/* estos contadores son para medir los fps con mas precision, max, med y min */
volatile int fps_min, fps_max, fps_med;

static PALETTE pal_game_oscura; // paleta del juego 'oscura',
                                // sirve para cuando se ponen menus, etc
                                // de esa manera, hace un efecto 'oscuro'


// Funcion que llena la pal_game_oscura con la paleta actual oscurecida.
// Usara la paleta actual, asique setear primero la paleta adecuada
// Pasar en oscurecer la cantidad de oscurecimiento (0.01 a 1.00)
// NOTA: Ignora todos los colores que utiliza la GUI
//       o sea, los colores definidos en guiprocs.c -> xset_gui_colors()
static void llenar_pal_game_oscura(float oscurecer)
{
int i;
float h,s,v;
int r,g,b;
int ignorar = FALSE;
get_palette(pal_game_oscura);
for (i = 0; i <256; i ++ )
 {
     ignorar = FALSE;
     // observad que if mas horrible!!!
     if (i == gui_text_color ||
         i == gui_back_color ||
         i == gui_white_color ||
         i == gui_light_color ||
         i == gui_dark_color ||
         i == gui_black_color ||
         i == gui_fg_color  ||
         i == gui_mg_color ||
         i == gui_bg_color )
         {
             ignorar = TRUE; // ignorar el color porque es del GUI
         }


     if (!ignorar)
     {
         // tomar el color
         r = (int)pal_game_oscura[i].r;
         g = (int)pal_game_oscura[i].g;
         b = (int)pal_game_oscura[i].b;
         // multiplico por 4, porque el rango va de 0..63 en paleta y de 0..255 en la funcion de hsv
         rgb_to_hsv(r*4, g*4, b*4, &h, &s, &v);
        // oscurecer
        v -= oscurecer;
        if (v < 0.0) v = 0.0;
        hsv_to_rgb(h, s, v, &r, &g, &b); // generar nuevo color
        r /= 4; g /= 4; b /= 4;
        pal_game_oscura[i].r = r;
        pal_game_oscura[i].g = g;
        pal_game_oscura[i].b = b;
   }
 }
}


/* Cuenta las fps y saca promedio de fps*/
void fps_proc()
{
   fps = frame_count;

        /* ir sacando promedio */
        fps_med += fps;
        fps_med = fps_med / 2;

   frame_count = 0;
}
END_OF_FUNCTION(fps_proc);

/* Cuenta la velocidad del juego */
void increment_speed_counter()
{
 speed_counter++;
}
END_OF_FUNCTION(increment_speed_counter);


/*--------------------------------------------------------
Actualiza la logica del juego
--------------------------------------------------------*/
void update_game_logic()
{
    ejecutar_poll_musica(); /* continuar ejecutando la musica */

    rvideo_record(kfbufferbmp, pal_game); // grabar 'video' a disco - DEBUG

    scroll_mapa -= ABS(scroll_mapa_speed); 
    if (scroll_mapa < 0)
       {
        scroll_mapa = 0;

        /* ver, si no quedan mas enemigos, PASO de nivel!!!   */
        if (enemigo_1 == NULL && jugador.vida > 0) /* la lista esta vacia, no hay enemigos */
            {
             /* adelantar al jugador, llevarlo al borde superior, y hay si, salir */
            if (ABS((fixtoi(jugador.x)+(jugador.spr[1]->w/2)) - (ANCHO_FB/2)) > jugador.spr[1]->w*2)  // evita un efecto raro al acercarse al centro
             {
                 if ( ANCHO_FB/2 < fixtoi(jugador.x)+jugador.spr[1]->w/2 )
                                jugador.dx =fixsub(jugador.dx, ftofix(VEL_X_J*1.2));
                
                 if ( ANCHO_FB/2 > fixtoi(jugador.x)+jugador.spr[1]->w/2 )
                                jugador.dx =fixadd(jugador.dx, ftofix(VEL_X_J*1.2));
             }
             else
             { jugador.dx = 0; } // avanzar recto...

				if (fixtoi(jugador.y) > 5) // ir hacia la parte superior del nivel (la salida)
				{
					jugador.y = fixsub(jugador.y, ftofix(MAX_VEL_J_X));
					jugador.dy = fixsub(jugador.dy, ftofix(VEL_Y_J*1.5));
				}
				else
				{
					paso_de_nivel = TRUE;
				}
            }
       }

    /* el jugador se tiene que mover con el mapa */
    if (scroll_mapa > 0) jugador.y = fixsub(jugador.y, itofix(1));

    // obtener teclado si es necesario (algunas plataformas...)
    if (keyboard_needs_poll()) poll_keyboard();

    interpreta_teclado(); // teclado del jugador
    mover_jugador(scroll_mapa);
    mover_bomba(); /* bomba especial del jugador */
    mover_enemigos(scroll_mapa);
    mover_disparos_jug(scroll_mapa);
    mover_disparos_ene(scroll_mapa);

    if (nivel_detalle > 0) mover_particulas(0, scroll_mapa, ANCHO_FB, ALTO_FB);

    mover_explosiones(&ptr_explo_fondo);
    mover_explosiones(&ptr_explo_arriba);
    mover_emisor_humo(0, scroll_mapa, ANCHO_FB, ALTO_FB); // emisores de humo! wow!
    mover_humo(0, scroll_mapa, ANCHO_FB, ALTO_FB);
    
    mover_premio(0, scroll_mapa, ANCHO_FB, ALTO_FB); // premios...
    
    if (nivel_detalle > 1) mover_clima(kfbufferbmp); // mover clima

    /* ver si aparecen nuevos enemigos */
    scan_nuevos_enemigos(scroll_mapa / H_GR);

    /* TECLADO ADICIONAL - FUNCIONES PROPIAS DEL JUEGO */

    /* TECLA PAUSE o P, sirve para poner la PAUSA! */
    if (key[KEY_PAUSE] || key[KEY_P])
       {
           pausar_musica();

           key[KEY_PAUSE] = FALSE;
           key[KEY_P] = FALSE;
           clear_keybuf();
           set_palette(pal_game_oscura);
           // Mensaje GUI
           alert(get_config_text("Pause"), NULL, NULL, get_config_text("Continue"), NULL, 0, 0);
           clear_keybuf();
           key[KEY_PAUSE] = FALSE;
           key[KEY_P] = FALSE;
           frame_count = fps = speed_counter = 0; /* evita salto en los fps */

           continuar_musica();
           set_palette(pal_game);
       } // fin pausa


       /* ESC: menu con varias opciones */
       if (key[KEY_ESC])
       {
            DIALOG esc_key_dlg[] =
            {
               /* (proc)       (x) (y) (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)             (dp2) (dp3) */
               { xbox_proc,    0,  0,  200, 128, 0,   0,   0,    0,      0,   0,   NULL,            NULL, NULL },
               { xbutton_proc, 8,  8,  184, 32,  0,   0,   0,    D_EXIT,      0,   0,   "Configuration", NULL, NULL },
               { xbutton_proc, 8,  48, 184, 32,  0,   0,   0,    D_EXIT,      0,   0,   "Abort Game",    NULL, NULL },
               { xbutton_proc, 8,  88, 184, 32,  0,   0,   0,    D_EXIT,      0,   0,   "Resume game",   NULL, NULL },
               { NULL,         0,  0,  0,   0,   0,   0,   0,    0,      0,   0,   NULL,            NULL, NULL }
            };
             pausar_musica();

            traducir_DIALOG_dp(esc_key_dlg);
            centre_dialog(esc_key_dlg);
            set_dialog_color(esc_key_dlg, makecol(0,0,0), makecol(255,255,255));
            key[KEY_ESC] = FALSE;
        
            clear_keybuf();
            set_palette(pal_game_oscura);

            /* mostrar dialogo */
            switch (popup_dialog(esc_key_dlg, 0))
            {
            case 1: // configurar - DEBUG, truchisimo lo del F5
                key[KEY_F5] = TRUE;
            break;

            case 2: // abortar juego
            if (alert(get_config_text("Exit game"),
                       NULL,
                       NULL,
                       get_config_text("Yes"),
                       get_config_text("No"),
                       0, 0) == 1)
                          {
                              salir_del_juego = TRUE;
                          }
            break;

            default:
            break;

            }

             frame_count = fps = speed_counter = 0; /* evita salto en los fps */
             continuar_musica();
             set_palette(pal_game);
       }

       /* F5: menu de configuracion general, sonido, detalles, controles, etc */
       if (key[KEY_F5])
          {
             int tmp1 = quiere_musica;

             pausar_musica();
             set_palette(pal_game_oscura);
             key[KEY_F5] = FALSE;
             clear_keybuf();
             menu_configuracion_al_vuelo();
             frame_count = fps = speed_counter = 0; /* evita salto en los fps */

             reajustar_volumen_musica();
             continuar_musica();
             set_palette(pal_game);

             /* DEBUG: esto es para que si activa/desactiva la musica, tome efecto */
             if (tmp1 != quiere_musica)
               {
                  musica_stop();
                  musica_play(info_nivel.musica); 
               }
          }


       /* F1: ayuda del programa */
       if (key[KEY_F1])
          {
             pausar_musica();
             set_palette(pal_game_oscura);
             key[KEY_F1] = FALSE;
             clear_keybuf();
             ayuda_proggy_mnu();
             frame_count = fps = speed_counter = 0; /* evita salto en los fps */

             continuar_musica();
             set_palette(pal_game);
          }

       /* F11 prende y apaga el modo DEBUG (muestra info extra) */
       if (key[KEY_F11]) /* DEBUG, al distribuir, eliminar esto!!! */
          {
             key[KEY_F11] = FALSE;
             KRONO_QUIERE_DEBUG = !KRONO_QUIERE_DEBUG;
          }

       /* F12 sirve para capturar pantallas */
		if (key[KEY_F12])
		{
			// pausar_musica();
			
			key[KEY_F12] = FALSE;
			clear_keybuf();
			capturar_la_pantalla("krap");
				alert("Screen captured!", NULL,NULL,"OK", NULL,0,0);
			frame_count = fps = speed_counter = 0; 
			// continuar_musica();
		}

       /* F10 sirve para prender/apagar la grabacion de videos - DEBUG! */
       #ifdef SECCION_DESACTIVADA_PARA_EL_RELEASE
       if (key[KEY_F10])
       {
             rvideo_is_recording = !rvideo_is_recording;
             key[KEY_F10] = FALSE;
             clear_keybuf();
       }
       #endif 

}

/*--------------------------------------------------------
Actualiza la pantalla
--------------------------------------------------------*/
void update_display()
{

  /* Tuberia de dibujado, en orden de abajo arriba */

  set_clip(kfbufferbmp, 0, 0, mapa_fondo->w-1, kfbufferbmp->h-1); /* clipping adecuado */

// solo dibuja si la bomba especial no esta activa, sino, es desperdiciar rendering
if (!bomba_esta_activa)
{
          /* fondo */
          blit(mapa_fondo, kfbufferbmp, 0, scroll_mapa, 0, 0, mapa_fondo->w, kfbufferbmp->h);
        
          if (nivel_detalle > 3) dibujar_humo(kfbufferbmp, 0, scroll_mapa);
        
          dibujar_explosion(ptr_explo_fondo, kfbufferbmp, 0, scroll_mapa);
        
          dibujar_disp_ene(kfbufferbmp, 0, scroll_mapa);
        
          dibujar_enemigos(kfbufferbmp, 0, scroll_mapa);
        
          dibujar_disp_jug(kfbufferbmp, 0, scroll_mapa);
        
          dibujar_nave_jugador(kfbufferbmp, 0, scroll_mapa);
        
          dibujar_premio(kfbufferbmp, 0, scroll_mapa);
        
          if (nivel_detalle > 0) dibujar_particulas(kfbufferbmp, 0, scroll_mapa);
        
          dibujar_explosion(ptr_explo_arriba, kfbufferbmp, 0, scroll_mapa);
        
          if (nivel_detalle > 1)  dibuja_clima(kfbufferbmp); // clima
} // fin de ver si esta detonando la bomba
else
{
 dibujar_bomba(kfbufferbmp);
}

/* DEBUG: info de FPS, cantidades, etc */
        if (KRONO_QUIERE_DEBUG)
            {
                text_mode(0);

                /* DEBUG: lo saque, porque ya no lo preciso...
                textprintf(kfbufferbmp, font, 0, kfbufferbmp->h - text_height(font)-2, makecol(255,255,255),
                               "|fps:%5d|ener:%3d|e:%5d|p:%5d|ex:%5d|" ,
                               fps,
                               jugador.vida,
                               cant_enemigos_debug,
                               cant_particulas_debug,
                               cant_explosion_debug );
                */

                // Solo fps... por ahora - DEBUG
                textprintf(kfbufferbmp, font, 0, kfbufferbmp->h - text_height(font)-2, makecol(255,255,255),
                          "fps:%5d fps_max:%5d fps_min:%5d fps_med:%5d",
                           fps, fps_max, fps_min, fps_med);
            }


  /* Tablero */
  set_clip(kfbufferbmp, 0, 0, kfbufferbmp->w-1, kfbufferbmp->h-1); /* clipping adecuado */

  dibujar_tablero(kfbufferbmp);

  /* a pantalla */
  enviar_kfbuffer2screen();
}

/*
 Esta es la secuencia y loop principal del juego
 Llamar para comenzar un juego nuevo
 NOTA: si el parametro load_savegame != 0, se asume
       que se esta cargando un juego de disco,
       y no se alteran las variables del jugador!!
 */
void comenzar_juego(int load_savegame)
{
  /* funcion interna que libera listas enlazadas */
  void liberar_listas_interno()
  {
      /* LIBERAR LISTAS... */
      liberar_lista_enemigos();
      liberar_lista_disparos_ene();
      liberar_lista_disparos_jug();
      liberar_lista_particulas();
      liberar_lista_explosion(&ptr_explo_fondo);
      liberar_lista_explosion(&ptr_explo_arriba);
      liberar_emisores_humo();
      liberar_humo();
      liberar_premio();
  }

  /* funcion interna que limpia los colores y bufferes
     y coloca la paleta de juego, etc */
  void hacer_cleanup_mapeos()
  {
        set_palette(pal_game);
        color_map = &tabla_transparencia;  /* la rutina de sombras la precisa! */
        rgb_map = &tabla_RGB; /* acelera la fabricacion de colores de 8 bits */
        set_trans_blender(128, 128, 128, 128);  /* Transparencia 50% para > 8 bits */
        clear(kfbufferbmp); clear(screen); /* limpiar bufferes */
        clear_keybuf();
        xset_gui_colors(); /* setea el GUI */
        set_mouse_sprite(NULL); /* acomoda puntero del mouse para que se ajuste a los colores */
        llenar_pal_game_oscura(0.25); // prepara la paleta oscura para efectos en los menues
  }

  /* Funcion interna para instalar los timers */
  void pone_timers_juego()
  {
   /* Timers */
   if (install_int_ex(increment_speed_counter, BPS_TO_TIMER(30)))
         levantar_error("ERROR: no hay espacio para instalar el timer a 30 fps\n"); /* a 30 fps el juego */
         
   if (install_int(fps_proc, 1000))
         levantar_error("ERROR: no hay espacio para instalar el timer a 1000 milisegundos!\n"); /* a 1000 milisegundos (1 segundo) el contador de fps */

   /* Reiniciar contadores de velocidad... */
   frame_count = fps = speed_counter = 0; /* evita salto en los fps */
  }

  /* Funcion interna para SACAR los timers */
  void saca_timers_juego()
  {
    /* remover timers... */
    remove_int(fps_proc);
    remove_int(increment_speed_counter);
  }


salir_del_juego = FALSE;

/* Reiniciar juego, solo si no esta cargando un juego de disco */
if (!load_savegame)
 {
  nivel_actual = 1;
  reset_total_jugador();
 }
 
fps_min = INT_MAX;
fps_max = 0;
fps_med = 0;
LOCK_VARIABLE(fps_med);

/*  Frameskip timer */
LOCK_VARIABLE(speed_counter);
LOCK_FUNCTION(increment_speed_counter);

/* Fps timer */
LOCK_VARIABLE(frame_count);
LOCK_VARIABLE(fps);
LOCK_FUNCTION(fps_proc);

/* Inicia doble buffer */
kfbufferbmp = create_system_bitmap(ANCHO_RFB, ALTO_RFB);
if (kfbufferbmp == NULL) levantar_error("ERROR: fallo la creacion del doble buffer! (falta RAM?)");

hacer_cleanup_mapeos(); /* limpiar pantalla y mapeos */

/* cinematica de introduccion al juego, solo si es el primer nivel */
if (nivel_actual == 1) ejecutar_cinematica(game_intro_cine);

/* iniciar engine de grabado de videos de promocion */
init_rvideo();

/* Loop principal de juego */
while (!salir_del_juego ) {

      paso_de_nivel = FALSE;
    
      reset_parcial_jugador(); /* reposicionar jugador, etc */
   
    /*
       Verifico que el proximo nivel exista, si es asi,
       paso a dibujar las cinematicas, y luego, cargo el nivel en RAM (para ahorrar RAM)
       */
    if (!cargar_nivel(nivel_actual, -1))
    {
          liberar_listas_interno(); /* LIBERAR LISTAS... */
          scroll_mapa = H_FONDO - ALTO_FB; /* iniciar scrolling */

          hacer_cleanup_mapeos();

      /* ir al shopping */
         do_shopping_principal();

        /* cinematica de entrada al nivel */
        ejecutar_cinematica(info_nivel.cine_in); 

     /* Luego de las cinematicas, es necesario restaurar mapa RGB,
        mapa de transparencias y paleta de color... */
        hacer_cleanup_mapeos();
        
        if (cargar_nivel(nivel_actual, 0)) /* cargar el nivel en RAM, en serio... */
                levantar_error("Fallo la carga del nivel!"); /* shit! que paso - DEBUG */
    
          scan_nivel_1era_vez(); /* scanear los enemigos en la pantalla por 1era vez */

      /* Los timers deben ir EXACTAMENTE antes del while, caso contrario,
         salta tantos frames como tiempo consuma todo lo anterior! */
      pone_timers_juego(); /* instala los timers... */


      musica_play(info_nivel.musica); /* comenzar a ejecutar musica - DEBUG */

      // Iniciar efectos de clima
      init_clima(kfbufferbmp, info_nivel.clima_c, info_nivel.clima_t, info_nivel.clima_d);

      while( !paso_de_nivel && !salir_del_juego )
      {
                while (speed_counter > 0)
                {
                       update_game_logic(); // actualizar logica del juego
                       speed_counter--;
                }
                update_display(); // actualizar pantalla

                frame_count++; // contar los fps

                /* DEBUG: conteo de fps mas preciso */
                    if (fps > fps_max) fps_max = fps;
                    if (fps < fps_min && fps > 0) fps_min = fps;

                if (detalle_automatico)
                    {
                     nivel_detalle = (fps * 10)/30;
                     if (nivel_detalle > 10) nivel_detalle = 10;
                     if (nivel_detalle < 0) nivel_detalle = 0;
                    }

      } /* looping de juego en el nivel actual */

      musica_stop(); /* parar musica - DEBUG */

      saca_timers_juego(); /* desactiva los timers... */

       /* eliminar los datos del nivel de RAM */
       if (datmapa != NULL)
        {
             unload_datafile(datmapa);
             datmapa = NULL;
        }

      if (mapa_fondo != NULL)
        {
           /* liberar RAM consumida por el nivel */
          destroy_bitmap(mapa_fondo);       /* mapa de fondo */
          mapa_fondo = NULL;
        }

      /* murio? */
         if (jugador.vida <= JUG_MUERTO) /* ahora si murio... */
             {
                  paso_de_nivel = FALSE; /* sorry gordi, no pasas si estas muerto! */
                  salir_del_juego = TRUE;
    
                  /* cinematica de game over */
                  ejecutar_cinematica(game_over_cine);

                     /* Luego de las cinematicas, es necesario restaurar mapa RGB,
                        mapa de transparencias y paleta de color... */
                        hacer_cleanup_mapeos();
    
                  clear(screen); /* DEBUG, queda bien? */
                  clear_keybuf();
    
                  /* mensaje */
                  alert(get_config_text("Game Over"), NULL, NULL, get_config_text("OK"), NULL, 0, 0);
             }
    }      
    else /* no encontro el nivel */
    {
     /* DEBUG: si no encuentra el proximo nivel,
               SIGNIFICA QUE TERMINO EL JUEGO! */

     /* Luego de las cinematicas, es necesario restaurar mapa RGB,
        mapa de transparencias y paleta de color... */
      hacer_cleanup_mapeos();

      clear(screen); /* DEBUG, queda bien? */
      clear_keybuf();

      /* mensaje */
      alert(get_config_text("This is the end of the game"), NULL, NULL, get_config_text("OK"), NULL, 0, 0);

     salir_del_juego = TRUE; 
    }
    
    if (paso_de_nivel) /* funciones al pasar de nivel */
    {
       /* ejecutar cinematica salida nivel */
        ejecutar_cinematica(info_nivel.cine_out); 

      nivel_actual++;
    }

} /* looping de juego a traves de los niveles */

/* Liberar memoria */
liberar_listas_interno(); /* LIBERAR LISTAS... */
destroy_bitmap(kfbufferbmp); /*  DOBLE BUFFER */
kfbufferbmp = NULL;

}

#endif
