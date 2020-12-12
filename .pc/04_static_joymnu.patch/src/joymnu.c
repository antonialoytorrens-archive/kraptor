/*
 --------------------------------------------------------
 joymnu.c
 Menu de calibracion del joystick
 --------------------------------------------------------
*/

#include <allegro.h>
#include "joymnu.h"
#include "global.h"
#include "guitrans.h"
#include "guiprocs.h"

/*
 Probar el joystick
 Pasarle el numero de joystick a probar
*/
void probar_el_joystick(int nj)
{
   /* este objeto para dialogo dibuja hacia donde esta apuntando el joystick
      y cambia de color cuando presiona el boton 1,2 o 3
      usa d2 para propositos internos...
      */
   static int xbox_joystick_test_proc(int msg, DIALOG *d, int c)
   {
   int new_pos = 0;
   int pos_x = 0 , pos_y = 0; // posicion (-1 = abj,izq, 1 = arr,der )
       if (msg == MSG_START)
       {
        d->d2 = -1;
       }

       if(msg == MSG_DRAW)
        {
                rectfill(screen, d->x, d->y, d->x+d->w-2, d->y+d->h-2,gui_white_color);
                gui_rect(screen, d->x, d->y, d->w - 1, d->h - 1, F_IN);

                // dibujar pos del joystick - chetoooo!
                switch (d->d2)
                {
                case 1:
                 rectfill(screen, d->x, d->y+d->h/3*2-1, d->x+d->w/3-2, d->y+d->h-2, gui_light_color);
                 gui_rect(screen, d->x, d->y+d->h/3*2-1, d->w/3, d->h/3, F_LIGHT);
                break;

                case 2:
                 rectfill(screen, d->x+d->w/3, d->y+d->h/3*2-1, d->x+d->w/3*2-2, d->y+d->h-2, gui_light_color);
                 gui_rect(screen, d->x+d->w/3, d->y+d->h/3*2-1, d->w/3, d->h/3, F_LIGHT);
                break;

                case 3:
                 rectfill(screen, d->x+d->w/3*2, d->y+d->h/3*2-1, d->x+d->w-2, d->y+d->h-2, gui_light_color);
                 gui_rect(screen, d->x+d->w/3*2, d->y+d->h/3*2-1, d->w/3, d->h/3, F_LIGHT);
                break;

                case 4:
                 rectfill(screen, d->x, d->y+d->h/3-1, d->x+d->w/3-2, d->y+d->h/3*2-2, gui_light_color);
                 gui_rect(screen, d->x, d->y+d->h/3-1, d->w/3, d->h/3, F_LIGHT);
                break;

                case 5:
                 rectfill(screen, d->x+d->w/3, d->y+d->h/3-1, d->x+d->w/3*2-2, d->y+d->h/3*2-2, gui_light_color);
                 gui_rect(screen, d->x+d->w/3, d->y+d->h/3-1, d->w/3, d->h/3, F_LIGHT);
                break;

                case 6:
                 rectfill(screen, d->x+d->w/3*2, d->y+d->h/3-1, d->x+d->w-2, d->y+d->h/3*2-2, gui_light_color);
                 gui_rect(screen, d->x+d->w/3*2, d->y+d->h/3-1, d->w/3, d->h/3, F_LIGHT);
                break;

                case 7:
                 rectfill(screen, d->x, d->y+d->h/3-2, d->x+d->w/3-2, d->y, gui_light_color);
                 gui_rect(screen, d->x, d->y, d->w/3, d->h/3, F_LIGHT);
                break;

                case 8:
                 rectfill(screen, d->x+d->w/3, d->y+d->h/3-2, d->x+d->w/3*2-2, d->y, gui_light_color);
                 gui_rect(screen, d->x+d->w/3,d->y, d->w/3, d->h/3, F_LIGHT);
                break;

                case 9:
                 rectfill(screen, d->x+d->w/3*2, d->y+d->h/3-2, d->x+d->w-2, d->y, gui_light_color);
                 gui_rect(screen, d->x+d->w/3*2,d->y, d->w/3, d->h/3, F_LIGHT);
                break;

                default:
                 msg = MSG_IDLE; // chequear que paso...
                break;
                }
        }

        /* cuando esta idle, dibujar la posicion del joystick */
        if (msg == MSG_IDLE)
        {
         poll_joystick(); // absolutamente NECESARIO

         if (joy[nj].stick[0].axis[0].d1) pos_x = -1;
         if (joy[nj].stick[0].axis[0].d2) pos_x = 1;

         if (joy[nj].stick[0].axis[1].d2) pos_y = -1;
         if (joy[nj].stick[0].axis[1].d1) pos_y = 1;
         if (pos_x == -1)
           {
            if (pos_y == -1)
                new_pos = 1;
            else
                    if (pos_y == 1) new_pos = 7;
                        else
                                new_pos = 4;
           }
         else
         if (pos_x == 1)
         {
            if (pos_y == -1)
                new_pos = 3;
            else
                    if (pos_y == 1) new_pos = 9;
                        else
                                new_pos = 6;

         }
         else
         {
            if (pos_y == -1)
                new_pos = 2;
            else
                    if (pos_y == 1) new_pos = 8;
                        else
                                new_pos = 5;
         }

         if (new_pos != d->d2)
           {
            d->d2 = new_pos;
            return D_REDRAWME;
           }
        } // fin IDLE

        return D_O_K;
    } // fin del proc de test del joystick

        /* dialogo de prueba */
        DIALOG joy_test_dlg[] =
        {
           /* (proc)       (x) (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)        (dp2) (dp3) */
           { xbox_proc,    0,  0,   160, 160, 0,   0,   0,    0,      0,   0,   NULL,       NULL, NULL },
           { xtext_proc,   4,  4,   152, 16,  0,   0,   0,    0,      0,   0,   "Joystick", NULL, NULL },
           { xtext_proc,   4,  16,  152, 16,  0,   0,   0,    0,      0,   0,   "Test",     NULL, NULL },
           { xbox_joystick_test_proc,    35, 40,  90,  90,  0,   0,   0,    0,      0,   0,   NULL,       NULL, NULL },
           { xbutton_proc, 28, 140, 104, 16,  0,   0,   0,    D_EXIT, 0,   0,   "OK",       NULL, NULL },
           { NULL,         0,  0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,       NULL, NULL }
        };

    /* ver si debo calibrar primero */
    if (joy[nj].flags & JOYFLAG_CALIBRATE) dialog_joystick_calibrate(nj);

    /* mostrar el dialogo */
    traducir_DIALOG_dp(joy_test_dlg);
    centre_dialog(joy_test_dlg);
    set_dialog_color(joy_test_dlg, makecol(0,0,0), makecol(255,255,255));

    popup_dialog(joy_test_dlg, 0);

}

/*
Calibrar el joystick
Pasarle el numero de joystick a calibrar
*/
void dialog_joystick_calibrate(int nj)
{
AL_CONST char *msg; /* mensaje de calibracion */

 /* reiniciar el joystick */
 remove_joystick();

 alert(get_config_text("Center the joystick"), NULL, NULL, get_config_text("OK"), NULL, 0, 0);

 if (install_joystick(JOY_TYPE_AUTODETECT) != 0)
 {
   alert(get_config_text("Error initialising joystick"), get_config_text(allegro_error), NULL, get_config_text("OK"), NULL, 0, 0);
   quiere_usar_joystick = FALSE;
   return;
 }

 /* Tenemos un joystick presente? */
   if (!num_joysticks)
   {
   alert(get_config_text("No joystick found"), NULL, NULL, get_config_text("OK"), NULL, 0, 0);
   quiere_usar_joystick = FALSE;
   return;
   }

   /* calibrar si es necesario */
   while (joy[nj].flags & JOYFLAG_CALIBRATE)
   {
      msg = calibrate_joystick_name(nj);

      /* informar la accion a tomar */
      alert(get_config_text(msg), get_config_text("and press ENTER"), NULL, get_config_text("ENTER"), NULL, 0, 0);

      if (calibrate_joystick(nj) != 0)
      {
           alert(get_config_text("Error calibrating joystick!"), NULL, NULL, get_config_text("OK"), NULL, 0, 0);
           return;
      }
   }
}


/*
  Esto hace el menu de configuracion y calibracion del joystick
*/
void joystick_configuration_menu()
{
int ret = -1;
        DIALOG joystick_config_dlg[] =
{
   /* (proc)       (x) (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)                  (dp2) (dp3) */
   { xbox_proc,    0,  0,   200, 136, 0,   0,   0,    0,      0,   0,   NULL,                 NULL, NULL },
   { xtext_proc,   8,  4,   180, 12,  0,   0,   0,    0,      0,   0,   "Joystick",           NULL, NULL },
   { xcheck_proc,  8,  24,  184, 16,  0,   0,   0,    0,      1,   0,   "Enable",             NULL, NULL },
   { xbutton_proc, 8,  44,  184, 24,  0,   0,   0,    D_EXIT, 0,   0,   "Test",               NULL, NULL },
   { xbutton_proc, 8,  72,  184, 24,  0,   0,   0,    D_EXIT, 0,   0,   "Calibrate",          NULL, NULL },
   { xbutton_proc, 46, 104, 100, 24,  0,   0,   0,    D_EXIT, 0,   0,   "OK",                 NULL, NULL },
   { NULL,         0,  0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,                 NULL, NULL }
};

 /* Tenemos un joystick presente? */
   if (!num_joysticks)
   {
   alert(get_config_text("No joystick found"), NULL, NULL, get_config_text("OK"), NULL, 0, 0);
   quiere_usar_joystick = FALSE;
   return;
   }

    traducir_DIALOG_dp(joystick_config_dlg);
    centre_dialog(joystick_config_dlg);
    set_dialog_color(joystick_config_dlg, makecol(0,0,0), makecol(255,255,255));

    joystick_config_dlg[2].flags = (quiere_usar_joystick) ? D_SELECTED : 0 ;

    while (ret != 5)
    {
           ret = popup_dialog(joystick_config_dlg, 0); /* mostrar dialogo */
     if (ret == 3) // testear
     {
        probar_el_joystick(numero_de_joystick);
     }

     if (ret == 4) // calibrar
     {
          /* calibrar el joystick */
          if ((numero_de_joystick < 0) || (numero_de_joystick > num_joysticks-1)) numero_de_joystick = 0;
          dialog_joystick_calibrate(numero_de_joystick);
     }
    }


   if (joystick_config_dlg[2].flags & D_SELECTED)
           quiere_usar_joystick = TRUE;
   else
           quiere_usar_joystick = FALSE;
}
