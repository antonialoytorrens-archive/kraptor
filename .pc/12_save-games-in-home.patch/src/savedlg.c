// ------------------------------------
// savedlg.c
// ------------------------------------
// Sistema de salva / carga de juego
// Por Kronoman
// En memoria de mi querido padre
// Copyright (c) 2002, Kronoman
// ------------------------------------

#include <allegro.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "savedlg.h"
#include "jugador.h"
#include "global.h"
#include "guitrans.h"
#include "guiprocs.h"

static SAVE_GAME_ST save_cache[SAVE_GAME_SLOTS]; // save games en cache... ;-P

// Auxiliar que carga los savegames de disco a cache...
static void cachear_savegames()
{
 int i;
 PACKFILE *f;
 char fname[80];
 for (i=0; i < SAVE_GAME_SLOTS; i ++) save_cache[i].es_valido = FALSE;

 for (i=0; i < SAVE_GAME_SLOTS; i ++)
 {
   sprintf(fname, "krap%02d.ksv", i);
   f = pack_fopen(fname, F_READ_PACKED); // usa compresion! ojo!
   if (f != NULL)
   {
    // leer el chunk de una...
    if (pack_fread(&save_cache[i], sizeof(SAVE_GAME_ST), f) != sizeof(SAVE_GAME_ST))
    {
      save_cache[i].es_valido = FALSE; // este slot no se pudo leer OK
    }
    pack_fclose(f);
   }
   else
   {
    save_cache[i].es_valido = FALSE; // no pudo abrir el archivo
   }
 }
}

// Auxiliar que salva los savegames de cache a disco...
static void salvar_cache_savegames()
{
 int i;
 PACKFILE *f;
 char fname[80];

 for (i=0; i < SAVE_GAME_SLOTS; i ++)
 {
   if (save_cache[i].es_valido)
   {
     sprintf(fname, "krap%02d.ksv", i);
     f = pack_fopen(fname, F_WRITE_PACKED); // usa compresion! ojo!
     if (f != NULL)
     {
      // salvar el chunk de una...
      pack_fwrite(&save_cache[i], sizeof(SAVE_GAME_ST), f);
      pack_fclose(f);
     }
   }
 }
}

// Esto presenta el menu de salvar juego
void salvar_juego_menu()
{
 int ret = 0;
 int i = 0;
 struct tm the_time; // hora/fecha actual, para la desc del savegame
 time_t current_time;
 struct tm *t; // para obtener la hora/fecha actual
 
DIALOG save_dlg[] =
{
   /* (proc)       (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)         (dp2) (dp3) */
   { xbox_proc,    96,  40,  320, 200, 0,   0,   0,    0,      0,   0,   NULL,        NULL, NULL },
   { xtext_proc,   104, 48,  144, 12,  0,   0,   0,    0,      0,   0,   "Save game", NULL, NULL },
   { xbutton_proc, 104, 64,  304, 16,  0,   0,   0,    D_EXIT, 0,   0,   "Free slot", NULL, NULL },
   { xbutton_proc, 104, 84,  304, 16,  0,   0,   0,    D_EXIT, 0,   0,   "Free slot", NULL, NULL },
   { xbutton_proc, 104, 104, 304, 16,  0,   0,   0,    D_EXIT, 0,   0,   "Free slot", NULL, NULL },
   { xbutton_proc, 104, 124, 304, 16,  0,   0,   0,    D_EXIT, 0,   0,   "Free slot", NULL, NULL },
   { xbutton_proc, 104, 144, 304, 16,  0,   0,   0,    D_EXIT, 0,   0,   "Free slot", NULL, NULL },
   { xbutton_proc, 104, 164, 304, 16,  0,   0,   0,    D_EXIT, 0,   0,   "Free slot", NULL, NULL },
   { xbutton_proc, 104, 184, 304, 16,  0,   0,   0,    D_EXIT, 0,   0,   "Free slot", NULL, NULL },
   { xbutton_proc, 208, 208, 104, 24,  0,   0,   0,    D_EXIT, 0,   0,   "OK",        NULL, NULL },
   { NULL,         0,   0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,        NULL, NULL }
};

    traducir_DIALOG_dp(save_dlg);
    centre_dialog(save_dlg);
    set_dialog_color(save_dlg, makecol(0,0,0), makecol(255,255,255));


    cachear_savegames(); // levantar los savegames en cache...
    // colocar los savegames en el menu
    for (i=0; i < SAVE_GAME_SLOTS; i ++)
          if (save_cache[i].es_valido)   save_dlg[i+2].dp = save_cache[i].desc;

   do
   {
    ret = popup_dialog(save_dlg, 0); /* mostrar dialogo */
    if (ret >= 2 && ret <= 8)
       {
        // salvar
        // confirmar!
       if (alert(get_config_text("Save game"),
           NULL,
           NULL,
           get_config_text("Yes"),
           get_config_text("No"),
           0, 0) == 1)
           {
            // Prepara un nombre de savegame descriptivo
   //         sprintf(save_cache[ret-2].desc, "Game/Juego #%d", ret-1);
              // obtener la hora actual
              current_time = time(NULL);
              t = localtime(&current_time);
              the_time = *t;
              // descripcion: hora+fecha+nivel+dinero, 
              //               5+1+10+1+4+1+10=  32 caracteres
              sprintf(save_cache[ret-2].desc,
                      "%02d:%02d %02d/%02d/%04d L%03d $%09d",
                      the_time.tm_hour,
                      the_time.tm_min,
                      the_time.tm_mday,
                      the_time.tm_mon+1,
                      the_time.tm_year+1900,
                      nivel_actual,
                      jugador.dinero );
              
            save_cache[ret-2].desc[40] = '\0';
            save_cache[ret-2].es_valido = TRUE;
            save_cache[ret-2].vida = jugador.vida;
            save_cache[ret-2].dinero = jugador.dinero;
            memcpy(save_cache[ret-2].arma, jugador.arma, sizeof(int)*MAX_ARM_CLASS);
            save_cache[ret-2].arma_actual = jugador.arma_actual;
            save_cache[ret-2].bombas = jugador.bombas;
            save_cache[ret-2].nivel_actual = nivel_actual;
            save_cache[ret-2].nivel_de_dificultad = nivel_de_dificultad;
    
            salvar_cache_savegames();
            // mensaje avisando
                 alert(get_config_text("Game saved"),
                       NULL, NULL,
                       get_config_text("OK"),
                       NULL, 0, 0);
            ret = 9; // salir
           }
       }
   } while (ret != 9);
   
};

// Esto presenta el menu de cargar juego
// Los datos son cargados en RAM
// Devuelve:
// TRUE = si se cargo un juego, por lo tanto, al salir de esta funcion, se deberia comenzar el juego...
// FALSE = si no se cargo nada
int cargar_juego_menu()
{
 int ret = 0;
 int i = 0;
 
DIALOG load_dlg[] =
{
   /* (proc)       (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)         (dp2) (dp3) */
   { xbox_proc,    96,  40,  320, 200, 0,   0,   0,    0,      0,   0,   NULL,        NULL, NULL },
   { xtext_proc,   104, 48,  144, 12,  0,   0,   0,    0,      0,   0,   "Load game", NULL, NULL },
   { xbutton_proc, 104, 64,  304, 16,  0,   0,   0,    D_EXIT, 0,   0,   "Free slot", NULL, NULL },
   { xbutton_proc, 104, 84,  304, 16,  0,   0,   0,    D_EXIT, 0,   0,   "Free slot", NULL, NULL },
   { xbutton_proc, 104, 104, 304, 16,  0,   0,   0,    D_EXIT, 0,   0,   "Free slot", NULL, NULL },
   { xbutton_proc, 104, 124, 304, 16,  0,   0,   0,    D_EXIT, 0,   0,   "Free slot", NULL, NULL },
   { xbutton_proc, 104, 144, 304, 16,  0,   0,   0,    D_EXIT, 0,   0,   "Free slot", NULL, NULL },
   { xbutton_proc, 104, 164, 304, 16,  0,   0,   0,    D_EXIT, 0,   0,   "Free slot", NULL, NULL },
   { xbutton_proc, 104, 184, 304, 16,  0,   0,   0,    D_EXIT, 0,   0,   "Free slot", NULL, NULL },
   { xbutton_proc, 208, 208, 104, 24,  0,   0,   0,    D_EXIT, 0,   0,   "OK",        NULL, NULL },
   { NULL,         0,   0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,        NULL, NULL }
};

    traducir_DIALOG_dp(load_dlg);
    centre_dialog(load_dlg);
    set_dialog_color(load_dlg, makecol(0,0,0), makecol(255,255,255));


    cachear_savegames(); // levantar los savegames en cache...
    // colocar los savegames en el menu
    for (i=0; i < SAVE_GAME_SLOTS; i ++)
          if (save_cache[i].es_valido)   load_dlg[i+2].dp = save_cache[i].desc;

   do
   {
    ret = do_dialog(load_dlg, 0); /* mostrar dialogo */
    if (ret >= 2 && ret <= 8)
       {
        // cargar
        if (save_cache[ret-2].es_valido)
        {
          jugador.vida = save_cache[ret-2].vida;
          jugador.dinero = save_cache[ret-2].dinero;
          memcpy(jugador.arma, save_cache[ret-2].arma, sizeof(int)*MAX_ARM_CLASS);
          jugador.arma_actual = save_cache[ret-2].arma_actual;
          jugador.bombas = save_cache[ret-2].bombas;
          nivel_actual = save_cache[ret-2].nivel_actual;
          nivel_de_dificultad = save_cache[ret-2].nivel_de_dificultad;

          return TRUE; // al salir de esta funcion, se deberia comenzar el juego...
        }
        else
        {
        // Avisar que no hay juego salvado
             alert(get_config_text("Is not a saved game"),
                   NULL, NULL,
                   get_config_text("OK"),
                   NULL, 0, 0);
        }
       }

   } while (ret != 9);

return FALSE;
};
