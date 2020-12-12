/*
 --------------------------------------------------------
 shopping.c
 --------------------------------------------------------
 Sistema de shopping de Kraptor 

 NOTAS: TODOS LOS TEXTOS DEL MENU ESTAN EN *INGLES*
 PARA PODER TRADUCIRLOS AUTOMATICAMENTE
 CON EL ENGINE DE TRADUCCION.
 USAR MENUES Y DIALOGOS *LOCALES* PARA QUE AL
 ENTRAR/SALIR DE SCOPE, (CADA VEZ QUE SE LLAMA LA FUNCION)
 SE TRADUZCAN AUTOMATICAMENTE AL LENGUAJE ORIGINAL.
 --------------------------------------------------------
*/

#include <allegro.h>
#include <stdio.h>
#include "guitrans.h"
#include "guiprocs.h"
#include "shopping.h"
#include "jugador.h"
#include "savedlg.h"

// Imagen de fondo
BITMAP *shop_bmp = NULL;


/*
    Esta funcion es la principal
    Muestra el shop en pantalla y comienza todo.
    Llamar con la paleta de juego ya seteada.
*/
void do_shopping_principal()
{
/* variables que se muestran en el dialogo */
char str_cant[256], str_money[256], str_desc[2048], str_precio[256];
int ret = 0; /* respuesta del dialogo */
int item_idx = -2; /* indice del item mostrado, -1 = energia, -2 = bomba */

/* indices de items importantes en el dialogo */
#define IDX_PREV_BTN 3
#define IDX_NEXT_BTN 4
#define IDX_BUY_BTN  5
#define IDX_SELL_BTN 6
#define IDX_SAVE_BTN 7
#define IDX_EXIT_BTN 8
#define IDX_DESC     9
#define IDX_BMP      10
#define IDX_MONEY    12
#define IDX_CANT     14
#define IDX_PRICE    16

    DIALOG shop_main_dlg[] =
        {
           /* (proc)        (x)  (y)  (w)  (h)  (fg) (bg) (key) (flags) (d1) (d2) (dp)          (dp2) (dp3) */
           { xbox_proc,     0,   0,   320, 192, 0,   0,   0,    0,      0,   0,   NULL,         NULL, NULL },
           { xbox_proc,     76,  4,   240, 184, 0,   0,   0,    0,      1,   0,   NULL,         NULL, NULL },
           { xbox_proc,     84,  8,   228, 160, 0,   0,   0,    0,      1,   0,   NULL,         NULL, NULL },
           { xbutton_proc,  4,   4,   32,  16,  0,   0,   0,    D_EXIT, 0,   0,   "<<",         NULL, NULL },
           { xbutton_proc,  40,  4,   32,  16,  0,   0,   0,    D_EXIT, 0,   0,   ">>",         NULL, NULL },
           { xbutton_proc,  4,   28,  68,  24,  0,   0,   0,    D_EXIT, 0,   0,   "Buy",        NULL, NULL },
           { xbutton_proc,  4,   60,  68,  24,  0,   0,   0,    D_EXIT, 0,   0,   "Sell",       NULL, NULL },
           { xbutton_proc,  4,   116, 68,  44,  0,   0,   0,    D_EXIT, 0,   0,   "Save",       NULL, NULL },
           { xbutton_proc,  4,   168, 68,  20,  0,   0,   0,    D_EXIT, 0,   0,   "OK",         NULL, NULL },
           { xtextbox_proc, 200, 12,  108, 124, 0,   0,   0,    0,      0,   0,   NULL,         NULL, NULL },
           { xbitmap_proc,  88,  12,  108, 124, 0,   0,   0,    0,      0,   0,   NULL,         NULL, NULL },
           { xtext_proc,    88,  172, 68,  12,  0,   0,   0,    0,      0,   0,   "Money",      NULL, NULL },
           { xtext_proc,    160, 172, 148, 12,  0,   0,   0,    0,      0,   0,   "1234567890", NULL, NULL },
           { xtext_proc,    88,  140, 68,  12,  0,   0,   0,    0,      0,   0,   "Amount",     NULL, NULL },
           { xtext_proc,    160, 140, 148, 12,  0,   0,   0,    0,      0,   0,   "1234567890", NULL, NULL },
           { xtext_proc,    88,  152, 68,  12,  0,   0,   0,    0,      0,   0,   "Price",      NULL, NULL },
           { xtext_proc,    160, 152, 148, 12,  0,   0,   0,    0,      0,   0,   "1234567890", NULL, NULL },
           { NULL,          0,   0,   0,   0,   0,   0,   0,    0,      0,   0,   NULL,         NULL, NULL }
        };

    traducir_DIALOG_dp(shop_main_dlg);
    centre_dialog(shop_main_dlg);
    set_dialog_color(shop_main_dlg, makecol(0,0,0), makecol(255,255,255));


     clear(screen);
     if (shop_bmp != NULL)
       stretch_blit( shop_bmp,
                     screen,
                     0, 0,
                     shop_bmp->w,
                     shop_bmp->h,
                     0, 0,
                     SCREEN_W,
                     SCREEN_H); // Fondo

    /* looping principal */
    ret = 0;
 while (ret != IDX_EXIT_BTN)
 {
     /* setear variables a mostrar */
     sprintf(str_cant,"  %9d",0);
     sprintf(str_money,"$ %9d", jugador.dinero);
     sprintf(str_desc, get_config_text("Unknown"));
     sprintf(str_precio,"$ %9d", 0);
     shop_main_dlg[IDX_MONEY].dp = (void *)str_money;  /* dinero del jugador */

     shop_main_dlg[IDX_DESC].dp = (void *)str_desc;    /* descripcion */
     shop_main_dlg[IDX_BMP].dp = NULL;                 /* bitmap  */
     shop_main_dlg[IDX_CANT].dp = (void *)str_cant;    /* cantidad que tiene */
     shop_main_dlg[IDX_PRICE].dp = (void *)str_precio; /* precio */



     if (ret == IDX_NEXT_BTN) // proximo item
           {
               int i, i2;
               i2 = item_idx;

               for(i = 0; i < MAX_ARM_CLASS; i++)
                {
                 i2++;
                 if (i2 > MAX_ARM_CLASS-1) i2 = -2;
                 if (i2 > -1)
                 {
                     if ( arma_tipo[i2].precio >= 0)
                        {
                          item_idx = i2;
                          break;
                        };
                 }
                 if (i2 == -1 || i2 == -2)
                    {
                          item_idx = i2;
                          break;
                    }
                }
           } // fin proximo item

       if (ret == IDX_PREV_BTN) //  item anterior
           {
               int i, i2;
               i2 = item_idx;

               for(i = 0; i < MAX_ARM_CLASS; i++)
                {
                 i2--;
                 if (i2 < -2) i2 = MAX_ARM_CLASS-1;

                 if (i2 > -1)
                 {
                     if ( arma_tipo[i2].precio >= 0)
                        {
                          item_idx = i2;
                          break;
                        };
                 }
                 if (i2 == -1 || i2 == -2)
                    {
                          item_idx = i2;
                          break;
                    }
                }
           } // fin item anterior

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
    /* Setear informacion real */
     if (item_idx < 0) // es la energia o la bomba?
        {
         if (item_idx == -1)
         {
         /* item - energia */
         shop_main_dlg[IDX_DESC].dp = (void *)jugador.reparar_desc;

         sprintf(str_precio,"$ %9d x %4d", jugador.reparar_precio, jugador.reparar_cantidad);

         sprintf(str_cant,"%4d / %4d", jugador.vida, MAX_ENERGIA);

         shop_main_dlg[IDX_BMP].dp = (void *)jugador.reparar_bmp;
         }

         if (item_idx == -2)
         {
         /* item - bomba */
         shop_main_dlg[IDX_DESC].dp = (void *)jugador.bomba_desc;

         sprintf(str_precio,"$ %9d x %4d", jugador.bomba_precio, jugador.bomba_cantidad);

         sprintf(str_cant,"%4d / %4d", jugador.bombas, jugador.max_bombas);

         shop_main_dlg[IDX_BMP].dp = (void *)jugador.bomba_bmp;
         }
        }
      else
      {  /* arma */
       shop_main_dlg[IDX_BMP].dp = (void *)arma_tipo[item_idx].arma;
       shop_main_dlg[IDX_DESC].dp = (void *)arma_tipo[item_idx].desc;
       // si es el arma 0, es infinita...
       if (item_idx == 0)
       {
        sprintf(str_precio, "N/A");
        sprintf(str_cant, "N/A");
       }
       else // arma normal
       {
           sprintf(str_precio,"$ %9d x %4d", arma_tipo[item_idx].precio, arma_tipo[item_idx].cant_ammo);
           sprintf(str_cant,"%4d / %4d", jugador.arma[item_idx], arma_tipo[item_idx].cant_ammo_max );
       }
      }



     if (item_idx < -2) item_idx = -2;

     /* desabilita el boton 'vender' si es la energia, bombas o el arma 0 */
     if (item_idx <= 0)
          shop_main_dlg[IDX_SELL_BTN].flags = D_DISABLED | D_EXIT;
      else
      {
          shop_main_dlg[IDX_SELL_BTN].flags = D_EXIT;
	     /* desabilita el boton 'vender' si no tiene suficiente para vender */
     	     if (jugador.arma[item_idx] < arma_tipo[item_idx].cant_ammo) shop_main_dlg[IDX_SELL_BTN].flags = D_DISABLED | D_EXIT;
      }

     /* desabilita el boton 'comprar' si es el arma 0 */
     if (item_idx == 0)
          shop_main_dlg[IDX_BUY_BTN].flags = D_DISABLED | D_EXIT;
      else
          shop_main_dlg[IDX_BUY_BTN].flags = D_EXIT;
     
    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
     set_mouse_sprite(NULL); /* acomoda puntero del mouse para que se ajuste a los colores */
    
     ret = do_dialog(shop_main_dlg, ret); /* mostrar dialogo */

    // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
     /* comprar */
     if (ret == IDX_BUY_BTN)
     {
      if (item_idx > -1)
      {
       /* comprar armamento */
         if ( jugador.dinero >= arma_tipo[item_idx].precio ) // tiene la plata?
            {
                 if (jugador.arma[item_idx] + arma_tipo[item_idx].cant_ammo > arma_tipo[item_idx].cant_ammo_max) // tiene espacio para mas?
                 {
                     alert(get_config_text("You reached the maximum capacity of load"),
                           NULL, NULL,
                           get_config_text("OK"),
                           NULL, 0, 0);
                 }
                 else
                 {
                 if (alert(get_config_text("Buy this weapon"),
                           NULL,
                           NULL,
                           get_config_text("Yes"),
                           get_config_text("No"),
                           0, 0) == 1)
                              {
                                 jugador.arma[item_idx] += arma_tipo[item_idx].cant_ammo; 
                                 jugador.dinero -= arma_tipo[item_idx].precio;
                                 jugador.arma_actual = item_idx; /* arma seleccionada */
                               }
                 }
            }
            else
            {
             /* no tiene dinero */
                 alert(get_config_text("You need more money"),
                       NULL, NULL,
                       get_config_text("OK"),
                       NULL, 0, 0);
            }
      }
      else
      {
         if (item_idx == -1)
         {
         /* compro energia */
         if ( jugador.dinero >= jugador.reparar_precio )
            {
                 if (jugador.vida >= MAX_ENERGIA)
                 {
                     alert(get_config_text("You have the maximum"),
                           NULL, NULL,
                           get_config_text("OK"),
                           NULL, 0, 0);
                 }
                 else
                 {
                 if (alert(get_config_text("Buy"),
                           NULL,
                           NULL,
                           get_config_text("Yes"),
                           get_config_text("No"),
                           0, 0) == 1)
                              {
                                 jugador.vida += jugador.reparar_cantidad;
                                 if (jugador.vida >= MAX_ENERGIA) jugador.vida = MAX_ENERGIA;
                                 jugador.dinero -= jugador.reparar_precio;
                               }
                 }
            }
            else
            {
             /* no tiene dinero */
                 alert(get_config_text("You need more money"),
                       NULL, NULL,
                       get_config_text("OK"),
                       NULL, 0, 0);
            }
         }

         if (item_idx == -2)
         {
         /* compro bombas */
         if ( jugador.dinero >= jugador.bomba_precio )
            {
                 if (jugador.bombas >= jugador.max_bombas)
                 {
                     alert(get_config_text("You have the maximum"),
                           NULL, NULL,
                           get_config_text("OK"),
                           NULL, 0, 0);
                 }
                 else
                 {
                 if (alert(get_config_text("Buy"),
                           NULL,
                           NULL,
                           get_config_text("Yes"),
                           get_config_text("No"),
                           0, 0) == 1)
                              {
                                 jugador.bombas += jugador.bomba_cantidad;
                                 if (jugador.bombas >= jugador.max_bombas) jugador.bombas = jugador.max_bombas;
                                 jugador.dinero -= jugador.bomba_precio;
                               }
                 }
            }
            else
            {
             /* no tiene dinero */
                 alert(get_config_text("You need more money"),
                       NULL, NULL,
                       get_config_text("OK"),
                       NULL, 0, 0);
            }
         }
      }
     } // fin comprar

     /* vender */
     if (ret == IDX_SELL_BTN)
     {
      char tmp[512];
      sprintf(tmp, "$ %d", arma_tipo[item_idx].precio/2);

      if (item_idx > -1) /* la energia no se puede vender.. sorry.. */
      {
       /* vender armamento */
         if (jugador.arma[item_idx] >= arma_tipo[item_idx].cant_ammo) // solo si tiene suficiente
            {
                 if (alert(get_config_text("Sell this weapon"),
                           get_config_text("Price"),
                           tmp,
                           get_config_text("Yes"),
                           get_config_text("No"),
                           0, 0) == 1)
                              {
                                 jugador.arma[item_idx] -= arma_tipo[item_idx].cant_ammo;
                                 jugador.dinero += arma_tipo[item_idx].precio / 2;
                                 jugador.arma_actual = 0; /* arma 0 seleccionada, esto para evitar un bug al vender armas */
                               }
            }
       }
     } // fin vender

     if (ret == IDX_SAVE_BTN) // salvar
     {
      salvar_juego_menu();
     } // fin salvar

 } // fin loop principal
}
