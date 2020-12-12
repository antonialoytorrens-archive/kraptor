/*
 --------------------------------------------------------
 menu.c
 --------------------------------------------------------
 Sistema de menu de Kraptor (al estilo de Unre*l)
 Tiene el menu principal, y algunos otros menues cools (sonido, etc)

 NOTAS: TODOS LOS TEXTOS DEL MENU ESTAN EN *INGLES*
 PARA PODER TRADUCIRLOS AUTOMATICAMENTE
 CON EL ENGINE DE TRADUCCION.
 USAR MENUES Y DIALOGOS *LOCALES* PARA QUE AL
 ENTRAR/SALIR DE SCOPE, (CADA VEZ QUE SE LLAMA LA FUNCION)
 SE TRADUZCAN AUTOMATICAMENTE AL LENGUAJE ORIGINAL.
 --------------------------------------------------------
*/
#ifndef K_MENU_C
#define K_MENU_C

#include <allegro.h>
#include "main.h"
#include "guitrans.h"
#include "guiprocs.h"
#include "error.h"
#include "game.h"
#include "data.h"
#include "menu.h"
#include "global.h"
#include "jugador.h"
#include "savedlg.h"
#include "joymnu.h"

/* Accion seleccionada en el menu, ver los #define en menu.h */
static int menu_seleccionado = 0;

BITMAP *bmp_menu_main = NULL; /* bitmap de fondo */
PALETTE pal_menu_main; /* paleta del menu */

/* puntero a la ayuda del juego */
char *texto_ayuda_juego = NULL;
char *texto_acerca_de_juego = NULL; /* acerca de... */
BITMAP *acerca_de_bmp = NULL; /* bitmap acerca de... */

/* boton 'especial' que se usa para configurar el teclado
  cuando se da click, muestra un mensaje, espera por
  una tecla, y la almacena en la variable que apunta dp3
  dp3 debe ser un *int (ej: &jugador.arr)
  si dp3 es NULL, llama a la funcion que resetea el teclado
  del jugador.
  Muestra el nombre de la tecla asignada en dp3 (readkey() >> 8)
*/
static int keybutton_proc(int msg, DIALOG *d, int c)
{

   /* nombres de teclas, extraido del ejemplo de Allegro, exkeys.c
     y modificado para simplificar el display */
    char *key_names[] =
    {
       "(None)",     "A",          "B",          "C",
       "D",          "E",          "F",          "G",
       "H",          "I",          "J",          "K",
       "L",          "M",          "N",          "O",
       "P",          "Q",          "R",          "S",
       "T",          "U",          "V",          "W",
       "X",          "Y",          "Z",          "0",
       "1",          "2",          "3",          "4",
       "5",          "6",          "7",          "8",
       "9",          "0 Pad",      "1 Pad",      "2 Pad",
       "3 Pad",      "4 Pad",      "5 Pad",      "6 Pad",
       "7 Pad",      "8 Pad",      "9 Pad",      "F1",
       "F2",         "F3",         "F4",         "F5",
       "F6",         "F7",         "F8",         "F9",
       "F10",        "F11",        "F12",        "Esc",
       "`",          "-",          "=",          "Backspace",
       "Tab",        "[",          "]",          "Enter",
       ":",          "'",          "\\",         "\\",
       ",",          ".",          "/",          "Space",
       "Insert",     "Del",        "Home",       "End",
       "Pgup",       "Pgdn",       "Left",       "Right",
       "Up",         "Down",       "/ Pad",      "*",
       "- Pad",      "+ Pad",      "Del Pad",    "Enter Pad",
       "Prtscr",     "Pause",      "Abnt_C1",    "Yen",
       "Kana",       "Convert",    "Noconvert",  "At",
       "Circumflex", "Colon2",     "Kanji",
       "Lshift",     "Rshift",     "Lcontrol",   "Rcontrol",
       "Alt",        "Altgr",      "Lwin",       "Rwin",
       "Menu",       "Scrlock",    "Numlock",    "Capslock",
       "Max"
    };

   /* si selecciona, pedir tecla... */
      if (msg == MSG_CLICK || msg == MSG_KEY)
      {
            /* pone un ??? en el caption */
            int p = (d->flags & D_SELECTED) ? 1 : 0;
            int style = ((p) ? F_IN : 0) | ((d->flags & D_GOTFOCUS) ? F_LIGHT : 0);

        scare_mouse();

            gui_rect(screen, d->x, d->y, d->w, d->h, style);
            rectfill(screen, d->x+1, d->y+1, d->x+d->w-2, d->y+d->h-2, (d->flags & D_GOTFOCUS) ? gui_light_color : gui_back_color);
    
            text_mode(-1);
            gui_textout(screen, "???", d->x+p + d->w/2, d->y+p + (d->h-text_height(font))/2, (d->flags & D_DISABLED) ? gui_dark_color : gui_text_color, 1);

        unscare_mouse();

        clear_keybuf();
        if (d->dp3 == NULL)
          {
           set_teclado_default();
           alert(get_config_text("Default keyboard"), NULL, NULL, get_config_text("OK"), NULL, 0, 0);
           return D_REDRAW;
          }
        else
          {
          *((int *)d->dp3) = readkey() >> 8; /* leer tecla */
          }
        
      return D_REDRAWME;
      }
      else
       {

        if(msg == MSG_DRAW && d->dp3 != NULL) /* al dibujar, setear el nombre de la tecla al actual */
           {
           if ( *((int *)d->dp3) > -1 && *((int *)d->dp3) < 114) /* limitar rango */
                  d->dp = (void *)get_config_text(key_names[*((int *)d->dp3)]);  /* poner tecla en el caption, traducida y todo... */
              else
                  d->dp = (void *)get_config_text("Unknown");
           }

       return xbutton_proc(msg, d, c); /* sobrecarga la funcion base */
       }
    return D_O_K;
}





/*
   Esta funcion fabrica el menu principal
   tiene un monton de mierdas propias
   dentro del mismo scope
*/
static void hacer_el_menu_principal_helper()
{
            // comenzar un nuevo juego
            static int nuevo_juego_mnu()
            {
                DIALOG skill_select_dlg[] =
                {
                   /* (proc)       (x) (y) (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)      (dp2) (dp3) */
                   { xbox_proc,    0,  0,  160, 128, 0,   0,   0,    0,      0,   0,   NULL,     NULL, NULL },
                   { xbutton_proc, 8,  32, 144, 24,  0,   0,   0,    D_EXIT, 0,   0,   "Easy",   NULL, NULL },
                   { xbutton_proc, 8,  64, 144, 24,  0,   0,   0,    D_EXIT, 0,   0,   "Medium", NULL, NULL },
                   { xbutton_proc, 8,  96, 144, 24,  0,   0,   0,    D_EXIT, 0,   0,   "Hard",   NULL, NULL },
                   { xtext_proc,   8,  8,  144, 16,  0,   0,   0,    0,      0,   0,   "Skill",  NULL, NULL },
                   { NULL,         0,  0,  0,   0,   0,   0,   0,    0,      0,   0,   NULL,     NULL, NULL }
                };

                /* nivel de dificultad, */
                menu_seleccionado = MNU_S_NEW_GAME;

                traducir_DIALOG_dp(skill_select_dlg);
                centre_dialog(skill_select_dlg);
                set_dialog_color(skill_select_dlg, makecol(0,0,0), makecol(255,255,255));
                
                nivel_de_dificultad = do_dialog(skill_select_dlg, 2) - 1;
                if (nivel_de_dificultad < 0 || nivel_de_dificultad > 2) nivel_de_dificultad = 1;

               return D_CLOSE;
            }

            // Carga un juego
            static int cargar_mnu()
            {
               menu_seleccionado = MNU_S_LOAD_GAME;
               
               return D_CLOSE;
            }
            
            static int salir_mnu()
            {
              menu_seleccionado = MNU_S_QUIT_GAME;
            
              return D_CLOSE;
            }
            
            /* Permite seleccionar un nuevo modo de video */
            static int choose_video_mnu()
            {
             int card = 0, w = SCREEN_W, h = SCREEN_H;
            
             /* permitir elegir el modo de video */
             if (!gfx_mode_select(&card, &w, &h)) return D_REDRAW;
            
            set_color_depth(8); /* setear a 8bpp, en otros modos, no ANDA! */
            // DEBUG: FALTA INFORMAR SI FALLA EL MODO DE VIDEO
            if (set_gfx_mode(card, w, h, 0, 0))
                if (set_gfx_mode(GFX_SAFE, 640, 480, 0, 0))
                   if (set_gfx_mode(GFX_SAFE, 320, 200, 0, 0))
                       levantar_error("ERROR: imposible utilizar algun modo grafico de 8bpp!");
            
             return D_CLOSE;
            }

            /* Acerca de... */
            static int acercade_proggy_mnu()
            {
                DIALOG acerca_de_dlg[] =    
                {
                   /* (proc)        (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)             (dp2) (dp3) */
                   { xbox_proc,     0,   0,   320, 200, 0,   0,   0,    0,      0,   0,   NULL,            NULL, NULL },
                   { xtextbox_proc, 4,   20,  152, 160, 0,   0,   0,    0,      0,   0,   NULL,            NULL, NULL },
                   { xbutton_proc,  300, 4,   16,  12,  0,   0,   0,    D_EXIT, 0,   0,   "X",             NULL, NULL },
                   { xbox_proc,     164, 20,  152, 176, 0,   0,   0,    0,      0,   0,   NULL,            NULL, NULL },
                   { xtext_proc,    4,   4,   100, 12,  0,   0,   0,    0,      0,   0,   "About",         NULL, NULL },
                   { xbitmap_proc,  168, 24,  144, 168, 0,   0,   0,    0,      0,   0,   NULL,            NULL, NULL },
                   { xtext_proc,    4,   184, 100, 12,  0,   0,   0,    0,      0,   0,   "Version x.x.x", NULL, NULL },
                   { NULL,          0,   0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,            NULL, NULL }
                };

                if (texto_acerca_de_juego == NULL || acerca_de_bmp == NULL)
                    {
                        alert("ERROR:", "No hay informacion disponible!", NULL, "Aceptar", NULL, 0, 0);
                        return D_REDRAW;
                    }

                /* DEBUG: alterar los indices [x] si cambia el dialogo! */
                acerca_de_dlg[1].dp = texto_acerca_de_juego;  /* setear el texto de ayuda de disco */
                acerca_de_dlg[4].dp = (void *)get_config_text("About"); /* traducir el caption */
                acerca_de_dlg[5].dp = acerca_de_bmp;
                acerca_de_dlg[6].dp = KRAPTOR_VERSION_STR;

                centre_dialog(acerca_de_dlg);
                set_dialog_color(acerca_de_dlg, makecol(0,0,0), makecol(255,255,255));
                popup_dialog(acerca_de_dlg, 0);

                return D_REDRAW;
            }

            /* idioma -> espa¤ol */
            static int cambia_espanol(void)
            {
              menu_seleccionado = MNU_S_CFG_SPANISH;
              return D_CLOSE;
            }

            /* idioma -> ingles */
            static int cambia_ingles(void)
            {
              menu_seleccionado = MNU_S_CFG_ENGLISH;
              return D_CLOSE;
            }

    /* Menu Juego */
    MENU menu_juego[] =
    {
       // text                          proc      ,      child    ,  flags,  dp
       { "New game",                    nuevo_juego_mnu,  NULL,       0,     NULL  },
       { "Load game",                   cargar_mnu,       NULL,       0,     NULL  },
       { "",                            NULL,             NULL,       0,     NULL  },
       { "Exit game",                   salir_mnu,        NULL,       0,     NULL  },
       { NULL,                          NULL,             NULL,       0,     NULL  }
    };
    
    
    /* Menu Configuracion */
    MENU menu_cfg[] =
    {
       // text                          proc      ,      child    ,  flags,  dp
       { "Sound",                    modificar_sonido_mnu, NULL,       0,     NULL  },
       { "Controls",                 mod_teclado_jugar,   NULL,       0,     NULL  },
       { "Details",                  mod_detalle_mnu,     NULL,       0,     NULL  },
       { "",                            NULL,             NULL,       0,     NULL  },
       { "Video",                       choose_video_mnu, NULL,       0,     NULL  },
       { "",                            NULL,             NULL,       0,     NULL  },
       { "Spanish",                     cambia_espanol,   NULL,       0,     NULL  },
       { "English",                     cambia_ingles,    NULL,       0,     NULL  },
       { NULL,                          NULL,             NULL,       0,     NULL  }
    };
    
    /* Menu Ayuda */
    MENU menu_ayuda[] =
    {
       // text                          proc      ,      child    ,  flags,  dp
       { "Help",                        ayuda_proggy_mnu, NULL,       0,     NULL  },
       { "About",                     acercade_proggy_mnu,NULL,       0,     NULL  },
       { NULL,                          NULL,             NULL,       0,     NULL  }
    };
    
    
    /* Menu superior */
    MENU menu_top[] =
    {
       // text                          proc      ,      child    ,  flags,  dp
       { "&Game",                        NULL,             menu_juego, 0,     NULL  },
       { "&Configuration",               NULL,             menu_cfg,   0,     NULL  },
       { "&Help",                        NULL,             menu_ayuda, 0,     NULL  },
       { NULL,                          NULL,             NULL,       0,     NULL  }
    };
    
    
    /* Dialogo principal */
    DIALOG main_dlg[] =
    {
       /* (dialog proc)     (x)   (y)   (w)   (h)   (fg)  (bg)  (key)    (flags)     (d1)           (d2)     (dp)              (dp2) (dp3) */
       { d_menu_proc,       0,    0,    0,    0,    0,    0,    0,       0,          0,             0,       menu_top,         NULL, NULL  },
       { NULL,              0,    0,    0,    0,    0,    0,    0,       0,          0,             0,       NULL,             NULL, NULL  }
    };

  /* traducir menus 'al vuelo' */
  traducir_MENU_text(menu_juego);
  traducir_MENU_text(menu_cfg);
  traducir_MENU_text(menu_ayuda);
  traducir_MENU_text(menu_top);

  menu_seleccionado = 0;
  do_dialog(main_dlg, -1);

}


/*
Funcion que muestra el menu principal
   y actual en consecuencia */
void realizar_menu_principal()
{
 menu_seleccionado = 0;

 while (menu_seleccionado != MNU_S_QUIT_GAME)
 {
     set_palette(pal_menu_main);
     color_map = NULL;
     rgb_map = NULL;
        xset_gui_colors();

     clear_keybuf();

     set_mouse_sprite(NULL); /* acomoda puntero del mouse para que se ajuste a los colores */

       stretch_blit(bmp_menu_main,
                     screen,
                     0, 0,
                     bmp_menu_main->w,
                     bmp_menu_main->h,
                     0, 0,
                     SCREEN_W,
                     SCREEN_H);

  /* hacer menu */
   hacer_el_menu_principal_helper();

      switch (menu_seleccionado)
      {
          case MNU_S_NEW_GAME: // comenzar juego nuevo
              comenzar_juego(FALSE);  /* Rock 'n roll! */
          break;

          case MNU_S_LOAD_GAME: // cargar un juego
             if (cargar_juego_menu())
                        comenzar_juego(TRUE); //  comenzar el juego, pero sin alterar las vars cargadas
          break;

          case MNU_S_QUIT_GAME: /* Salir, confirmar primero */
             if (alert(get_config_text("Exit game"),
                       NULL,
                       NULL,
                       get_config_text("Yes"),
                       get_config_text("No"),
                       0, 0) != 1)
                          {
                              menu_seleccionado = 0;
                          }
          break;

          case MNU_S_CFG_SPANISH: // cambiar a espa~ol
                reload_config_texts("es");
                lenguaje[0]='e';
                lenguaje[1]='s';
                lenguaje[2]= '\0';
          break;

          case MNU_S_CFG_ENGLISH: // cambiar a ingles
                reload_config_texts("en");
                lenguaje[0]='e';
                lenguaje[1]='n';
                lenguaje[2]= '\0';
          break;

          default:
          break;
      }
 }



}


/*
Menu para modificar parametros de sonido y musica
SE PUEDE LLAMAR DESDE OTROS LADOS (ej: desde game.c)
*/
int modificar_sonido_mnu(void)
{
    DIALOG sonido_dlg[] =
    {
       /* (proc)        (x) (y) (w)  (h) (fg) (bg) (key) (flags) (d1) (d2) (dp)     (dp2) (dp3) */
       { xbox_proc,     0,  0,  150, 92, 0,   0,   0,    0,      0,   0,   NULL,    NULL, NULL },
       { xcheck_proc,   4,  4,  140, 12, 0,   0,   0,    0,      1,   0,   "Sound", NULL, NULL },
       { xslider_proc,  4,  20, 140, 12, 0,   0,   0,    0,      255, 255, NULL,    NULL, NULL },
       { xcheck_proc,   4,  36, 140, 12, 0,   0,   0,    0,      1,   0,   "Music", NULL, NULL },
       { xslider_proc,  4,  52, 140, 12, 0,   0,   0,    0,      255, 255, NULL,    NULL, NULL },
       { xbutton_proc,  35, 72, 80,  16, 0,   0,   0,    D_EXIT, 0,   0,   "OK",    NULL, NULL },
       { NULL,          0,  0,  0,   0,  0,   0,   0,    0,      0,   0,   NULL,    NULL, NULL }
    };


    traducir_DIALOG_dp(sonido_dlg);
    centre_dialog(sonido_dlg);
    set_dialog_color(sonido_dlg, makecol(0,0,0), makecol(255,255,255));


    /* setear parametros actuales */
    sonido_dlg[1].flags |= (quiere_snd) ?  D_SELECTED  : 0 ;
    sonido_dlg[2].d2 = volumen_sonido;
    sonido_dlg[3].flags |= (quiere_musica) ?  D_SELECTED  : 0 ;
    sonido_dlg[4].d2 = volumen_musica;

    popup_dialog(sonido_dlg, 0); /* mostrar dialogo */

    /* ajustar nuevos parametros */
    if (sonido_dlg[1].flags & D_SELECTED)
            quiere_snd = TRUE;
        else
            quiere_snd = FALSE;

    volumen_sonido = sonido_dlg[2].d2;

    if (sonido_dlg[3].flags & D_SELECTED)
            quiere_musica = TRUE;
        else
            quiere_musica = FALSE;

     volumen_musica = sonido_dlg[4].d2;

  return D_REDRAW;
}

/* Esto permite configurar el teclado solamente */
int configurar_el_teclado_solo(void)
{
        /* dialogo de configuracion */
        DIALOG dlg_teclado_config[] =
                {
                   /* (proc)         (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)        (dp2) (dp3) */
                   { xbox_proc,      0,   0,   248, 184, 0,   0,   0,    0,      0,   0,   NULL,       NULL, NULL },
                   { xtext_proc,     8,   8,   188, 12,  0,   0,   0,    0,      0,   0,   "Controls", NULL, NULL },
                   { xbutton_proc,   8,   152, 72,  24,  0,   0,   0,    D_EXIT, 0,   0,   "OK",       NULL, NULL },
                   { keybutton_proc, 80,  24,  88,  24,  0,   0,   0,    0,      0,   0,   "Up",       NULL, NULL },
                   { keybutton_proc, 8,   56,  88,  24,  0,   0,   0,    0,      0,   0,   "Left",     NULL, NULL },
                   { keybutton_proc, 80,  88,  88,  24,  0,   0,   0,    0,      0,   0,   "Down",     NULL, NULL },
                   { keybutton_proc, 152, 56,  88,  24,  0,   0,   0,    0,      0,   0,   "Right",    NULL, NULL },
                   { keybutton_proc, 8,   120, 72,  24,  0,   0,   0,    0,      0,   0,   "Shoot",    NULL, NULL },
                   { keybutton_proc, 88,  120, 72,  24,  0,   0,   0,    0,      0,   0,   "Weapon",   NULL, NULL },
                   { keybutton_proc, 168, 120, 72,  24,  0,   0,   0,    0,      0,   0,   "Special",  NULL, NULL },
                   { keybutton_proc, 168, 152, 72,  24,  0,   0,   0,    0,      0,   0,   "Default",  NULL, NULL },
                   { NULL,           0,   0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,       NULL, NULL }
                };

        /* inicializo el dialogo (el compilador no permite inicializarlo arriba) */
        dlg_teclado_config[3].dp3 = &teclado_jug.arr;
        dlg_teclado_config[4].dp3 = &teclado_jug.izq;
        dlg_teclado_config[5].dp3 = &teclado_jug.abj;
        dlg_teclado_config[6].dp3 = &teclado_jug.der;
        dlg_teclado_config[7].dp3 = &teclado_jug.sht;
        dlg_teclado_config[8].dp3 = &teclado_jug.wpn;
        dlg_teclado_config[9].dp3 = &teclado_jug.bmb;

    traducir_DIALOG_dp(dlg_teclado_config);
    centre_dialog(dlg_teclado_config);
    set_dialog_color(dlg_teclado_config, makecol(0,0,0), makecol(255,255,255));

    popup_dialog(dlg_teclado_config, 0); /* mostrar dialogo */

return D_REDRAW;
}

/* permite configurar el mouse */
int configurar_el_mouse(void)
{
DIALOG mouse_cfg_dlg[] =
{
   /* (proc)        (x) (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)      (dp2) (dp3) */
   { xbox_proc,     0,  0,   140, 128, 0,   0,   0,    0,      0,   0,   NULL,     NULL, NULL },
   { xtext_proc,    4,  4,   132, 16,  0,   0,   0,    0,      0,   0,   "Mouse",  NULL, NULL },
   { xcheck_proc,   4,  28,  132, 16,  0,   0,   0,    0,      1,   0,   "Enable", NULL, NULL },
   { xtext_proc,    4,  56,  128, 16,  0,   0,   0,    0,      0,   0,   "Speed",  NULL, NULL },
   { xslider_proc,  4,  72,  132, 16,  0,   0,   0,    0,      32,  0,   NULL,     NULL, NULL },
   { xbutton_proc,  20, 100, 100, 20,  0,   0,   0,    D_EXIT, 0,   0,   "OK",     NULL, NULL },
   { NULL,          0,  0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,     NULL, NULL }
};

    traducir_DIALOG_dp(mouse_cfg_dlg);
    centre_dialog(mouse_cfg_dlg);
    set_dialog_color(mouse_cfg_dlg, makecol(0,0,0), makecol(255,255,255));

    mouse_cfg_dlg[2].flags = (quiere_usar_mouse) ? D_SELECTED : 0 ;
    mouse_cfg_dlg[4].d2 = mouse_velocidad;
    
    popup_dialog(mouse_cfg_dlg, 0); /* mostrar dialogo */

            if (mouse_cfg_dlg[2].flags & D_SELECTED)
                { quiere_usar_mouse = TRUE; }
            else
                { quiere_usar_mouse = FALSE; }

     mouse_velocidad = mouse_cfg_dlg[4].d2;

return D_REDRAW;
}


/*
Permite configurar los controles, entre ellos, teclado, mouse, joystick
DEBUG: esta sin terminar
Puede ser llamado desde otros lados, ej: game.c
*/
int mod_teclado_jugar(void)
{
int ret = -1;
 DIALOG controls_config_dlg[] =
 {
   /* (proc)       (x) (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)               (dp2) (dp3) */
   { xbox_proc,    0,  0,   200, 168, 0,   0,   0,    0,      0,   0,   NULL,              NULL, NULL },
   { xtext_proc,   8,  4,   184, 16,  0,   0,   0,    0,      0,   0,   "Controls",        NULL, NULL },
   { xbutton_proc, 8,  24,  184, 32,  0,   0,   0,    D_EXIT, 0,   0,   "Keyboard config", NULL, NULL },
   { xbutton_proc, 8,  60,  184, 32,  0,   0,   0,    D_EXIT, 0,   0,   "Mouse config",    NULL, NULL },
   { xbutton_proc, 8,  96,  184, 32,  0,   0,   0,    D_EXIT, 0,   0,   "Joystick config", NULL, NULL },
   { xbutton_proc, 50, 136, 100, 24,  0,   0,   0,    D_EXIT, 0,   0,   "OK",              NULL, NULL },
   { NULL,         0,  0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,              NULL, NULL }
 };

    traducir_DIALOG_dp(controls_config_dlg);
    centre_dialog(controls_config_dlg);
    set_dialog_color(controls_config_dlg, makecol(0,0,0), makecol(255,255,255));

    while (ret != 5)
    {
            ret = popup_dialog(controls_config_dlg, 0); /* mostrar dialogo */
        
            if (ret == 2) configurar_el_teclado_solo();
        
            if (ret == 3) configurar_el_mouse();
        
            if (ret == 4) joystick_configuration_menu();
    }
return D_REDRAW;
}

/*
Permite configurar los detalles
Puede ser llamado desde otros lados, ej: game.c
*/
int mod_detalle_mnu(void)
{
    DIALOG detalle_dlg[] =
{
   /* (proc)       (x) (y) (w)  (h) (fg) (bg) (key) (flags) (d1) (d2) (dp)            (dp2) (dp3) */
   { xbox_proc,    0,  0,  150, 96, 0,   0,   0,    0,      0,   0,   NULL,           NULL, NULL },
   { xtext_proc,   4,  4,  140, 12, 0,   0,   0,    0,      0,   0,   "Detail level", NULL, NULL },
   { xslider_proc, 4,  18, 140, 14, 0,   0,   0,    0,      10,  10,  NULL,           NULL, NULL },
   { xbutton_proc, 32, 76, 80,  16, 0,   0,   0,    D_EXIT, 0,   0,   "OK",           NULL, NULL },
   { xcheck_proc,  4,  36, 140, 12, 0,   0,   0,    0,      1,   0,   "Auto adjust",  NULL, NULL },
   { xcheck_proc,  4,  56, 140, 12, 0,   0,   0,    0,      1,   0,   "Play videos",  NULL, NULL },
   { NULL,         0,  0,  0,   0,  0,   0,   0,    0,      0,   0,   NULL,           NULL, NULL }
};


    traducir_DIALOG_dp(detalle_dlg);
    centre_dialog(detalle_dlg);
    set_dialog_color(detalle_dlg, makecol(0,0,0), makecol(255,255,255));

    /* setear parametros actuales */
    detalle_dlg[2].d2 = nivel_detalle;
    detalle_dlg[4].flags |= (detalle_automatico) ?  D_SELECTED  : 0 ;
    detalle_dlg[5].flags |= (quiere_videos) ? D_SELECTED : 0;
    
    popup_dialog(detalle_dlg, 0); /* mostrar dialogo */

    nivel_detalle = detalle_dlg[2].d2;

    if (detalle_dlg[4].flags & D_SELECTED)
          detalle_automatico = TRUE;
    else
          detalle_automatico = FALSE;
          
    if (detalle_dlg[5].flags & D_SELECTED)
          quiere_videos = TRUE;
    else
          quiere_videos = FALSE;

 return D_REDRAW;
}

/*
Este menu generico es para ser llamado
desde game.c, presenta la configuracion 'al vuelo'
de sonido, teclado y detalles
*/
int menu_configuracion_al_vuelo(void)
{
    DIALOG configurar_dlg[] =
    {
       /* (proc)       (x) (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)             (dp2) (dp3) */
       { xbox_proc,    0,  0,   128, 152, 0,   0,   0,    0,      0,   0,   NULL,            NULL, NULL },
       { xtext_proc,   8,  8,   112, 8,   0,   0,   0,    0,      0,   0,   "Configuration", NULL, NULL },
       { xbutton_proc, 8,  24,  112, 24,  0,   0,   0,    D_EXIT,      0,   0,   "Sound",         modificar_sonido_mnu, NULL },
       { xbutton_proc, 8,  56,  112, 24,  0,   0,   0,    D_EXIT,      0,   0,   "Controls",      mod_teclado_jugar, NULL },
       { xbutton_proc, 8,  88,  112, 24,  0,   0,   0,    D_EXIT,      0,   0,   "Details",       mod_detalle_mnu, NULL },
       { xbutton_proc, 8,  128, 112, 16,  0,   0,   0,    D_EXIT, 0,   0,   "OK",            NULL, NULL },
       { NULL,         0,  0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,            NULL, NULL }
    };

    traducir_DIALOG_dp(configurar_dlg);
    centre_dialog(configurar_dlg);
    set_dialog_color(configurar_dlg, makecol(0,0,0), makecol(255,255,255));

    /* setear parametros actuales */

    popup_dialog(configurar_dlg, 0); /* mostrar dialogo */

 return D_REDRAW;
}

/* Ayuda del programa
Puede ser llamado desde otros lados, ej: game.c
*/
int ayuda_proggy_mnu()
{
    /* dialogo que muestra la ayuda */
    DIALOG ayuda_dlg[] =
        {
           /* (proc)        (x)  (y) (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)               (dp2) (dp3) */
           { xbox_proc,     0,   0,  320, 200, 0,   0,   0,    0,      0,   0,   NULL,                 NULL, NULL },
           { xtextbox_proc, 4,   20, 312, 176, 0,   0,   0,    0,      0,   0,   NULL,                 NULL, NULL },
           { xbutton_proc,  300, 4,  16,  12,  0,   0,   0,    D_EXIT, 0,   0,   "X",               NULL, NULL },
           { xtext_proc,    4,   4,  100, 12,  0,   0,   0,    0,      0,   0,   "Help",            NULL, NULL },
           { NULL,          0,   0,  0,   0,   0,   0,   0,    0,      0,   0,   NULL,              NULL, NULL }
        };

    if (texto_ayuda_juego == NULL)
        {
            alert("ERROR:", "No hay ayuda disponible!", NULL, "Aceptar", NULL, 0, 0);
            return D_REDRAW;
        }
    
    /* DEBUG: alterar los indices [x] si cambia el dialogo! */
    ayuda_dlg[1].dp = texto_ayuda_juego;  /* setear el texto de ayuda de disco */
    ayuda_dlg[3].dp = (void *)get_config_text("Help"); /* traducir el caption */

    centre_dialog(ayuda_dlg);
    set_dialog_color(ayuda_dlg, makecol(0,0,0), makecol(255,255,255));
    popup_dialog(ayuda_dlg, 0);

 return D_REDRAW;
}


#endif
