/*
  shader.c
  Sombreador automatico para los mapas de Kraptor
  Copyright (c) 2002, Kronoman
  NOTA: el codigo es una porqueria, repetitivo, no optimizado, etc
        lo escribi en 15" para resolver un problema especifico

  Teclado:
    Arr, Abj = scroll del mapa
    V = ver grilla + info, si/no
    Q,W = color de la sombra
    O,P = grosor de la sombra
    T = ajustar a grilla on/off
    ESC = salir
    S = salvar bmp
    L = cargar bmp
  Mouse:
   click izquierdo: los 2 primeros setean el rectangulo de sombreado
                    el 3 click sombrea (confirma primero)

   click derecho: limpia el rectangulo de seleccion
*/

#include <stdio.h>
#include <stdlib.h>
#include <allegro.h>

/* Grilla WxH */
#define W_GR 40
#define H_GR 40

/* ancho de la sombra */
int w_sombra = 10;
// oscuridad de sombra (0 = negro, 255 = blanco)
int c_sombra = 16;

// scroll del mapa
int yp = 0;

// area del rectangulo a sombrear
// -1 indica area no seleccionada
int xr1 = -1;
int yr1 = -1;
int xr2 = -1;
int yr2 = -1;


/* En vgrilla pasar -1 para dibujarla, o 0 para ver el bitmap suelto */
int vgrilla = -1;

/* ajustar a grilla? */
int snapgrid = -1;

/* Bitmap cargado en RAM del fondo */
BITMAP *fnd;
PALETTE pal; /* paleta */
RGB_MAP tmp_rgb; /* acelera los calculos de makecol, etc */
COLOR_MAP trans_table; /* mapa de transparencia */

/* Doble buffer */
BITMAP *buffer;


void redibujar() {
   int iy, ix;

   clear(buffer);
   blit(fnd, buffer, 0, yp,0,0,fnd->w,SCREEN_H);

   /* grilla */
   if (vgrilla)
   {
   for (ix=0; ix < fnd->w / W_GR; ix ++ )
        line(buffer, ix*W_GR, 0, ix*W_GR, 480, makecol(255,255,255));

   for (iy=0; iy < fnd->h / H_GR; iy ++ )
        line(buffer, 0, iy*H_GR, fnd->w, iy*H_GR, makecol(255,255,255));

    /* Informacion */
    text_mode(-1); // texto (-1=trans, 1 solido)

    textprintf(buffer, font, 600,0,makecol(255,255,255),
               "%d", yp);

     if (xr1 >= 0 )
        {
         textprintf(buffer, font, 600,20,makecol(255,255,255),
                    "x1:%d",xr1);
         textprintf(buffer, font, 600,40,makecol(255,255,255),
                    "y1:%d",yr1);
        }

     if (xr2 >= 0)
        {
         textprintf(buffer, font, 600,60,makecol(255,255,255),
                    "x2:%d",xr2);
         textprintf(buffer, font, 600,80,makecol(255,255,255),
                    "y2:%d",yr2);
        }

     textprintf(buffer, font, 600,100,makecol(255,255,255),
                "g[%s]", (snapgrid) ? "*" : " " );

     textprintf(buffer, font, 600,120,makecol(255,255,255),
                "w:%d", w_sombra);

     textprintf(buffer, font, 600,140,makecol(255,255,255),
                "c:%d", c_sombra);
   }

   /* rectangulo de seleccion */
   if (xr1 >= 0 )
   {
    if (xr2 >= 0)
    {
    rect(buffer, xr1,  yr1-yp, xr2, yr2-yp, makecol(0,255,255) );
    line(buffer, xr1,  yr1-yp, xr2, yr2-yp, makecol(0,255,255) );
    line(buffer, xr2,  yr1-yp, xr1, yr2-yp, makecol(0,255,255) );

      // preview de como queda la sombra
      line(buffer, xr1,  yr2-yp, xr1-w_sombra, yr2-yp+w_sombra, makecol(0,255,255) );
      line(buffer, xr1,  yr1-yp, xr1-w_sombra, yr1-yp+w_sombra, makecol(0,255,255) );
      line(buffer, xr2,  yr2-yp, xr2-w_sombra, yr2-yp+w_sombra, makecol(0,255,255) );

      line(buffer, xr1-w_sombra, yr1-yp+w_sombra, xr1-w_sombra, yr2-yp+w_sombra,makecol(0,255,255) );
      line(buffer, xr1-w_sombra, yr2-yp+w_sombra, xr2-w_sombra, yr2-yp+w_sombra, makecol(0,255,255) );

      circle(buffer, xr1, yr1-yp, 5, makecol(255,0,255));
      circle(buffer, xr2, yr2-yp, 5, makecol(255,255,0));
    }
    else
    {
      line(buffer, xr1,  yr1-yp-5, xr1, yr1-yp+5, makecol(0,255,255) );
      line(buffer, xr1-5,  yr1-yp, xr1+5, yr1-yp, makecol(0,255,255) );
      circle(buffer, xr1, yr1-yp, 5, makecol(255,0,255));
    }
   }


/* mandar a pantalla */
scare_mouse();
 blit(buffer, screen, 0,0, 0,0,SCREEN_W, SCREEN_H);
unscare_mouse();
}


void editar_mapa() {
   int k;
   char salvar[1024]; /* archivo a salvar */;
   salvar[0] = '\0';

   static void reset_vars()
   {
       yp = 0;
       xr1 = -1;
       yr1 = -1;
       xr2 = -1;
       yr2 = -1;
   }

   reset_vars();
   redibujar();

   while (1) {

    while (!keypressed() && mouse_b == 0);

    if (mouse_b == 0)
       k = readkey() >> 8;
      else
       k = 0;

    // Selecciono boton?
    if (mouse_b & 1) // boton normal, seleccionar
       {
       int s; // lo uso mas abajo...

        if (xr1 < 0) // asignar primer esquina
        {
          if (snapgrid)
          {
               xr1 = (mouse_x / W_GR) * W_GR;
               yr1 = ((mouse_y + yp) / H_GR) * H_GR;
          }
          else
          {
              xr1 = mouse_x;
              yr1 = mouse_y + yp;
          }
        }
        else
        {
          if (xr2 < 0) // asignar 2nda esquina
          {
              if (snapgrid)
              {
                   xr2 = (mouse_x / W_GR) * W_GR;
                   yr2 = ((mouse_y + yp) / H_GR) * H_GR;
              }
              else
              {
                  xr2 = mouse_x;
                  yr2 = mouse_y + yp;
              }
          }
          else // Sombrear
          {
          if ( alert("Sombrear seleccion", "Esta seguro?", "No hay UNDO!", "Si", "No", 'S', 'N') == 1)
           {
            int c;

            drawing_mode(DRAW_MODE_TRANS, NULL, 0,0);
            c = makecol(c_sombra,c_sombra,c_sombra);
            for (s = 0; s < w_sombra; s++)
            {
              line(fnd, xr1-s, yr1+s, xr1-s, yr2+s, c);
              line(fnd, xr1-s+1, yr2+s, xr2-s, yr2+s, c);
            }
            solid_mode();
           }
          }
        }

        while (mouse_b);

            if (xr1 > xr2 && xr2 > -1)
              {
               s = xr2;
               xr2 = xr1;
               xr1 = s;
              }

            if (yr1 > yr2  && yr2 > -1)
              {
               s = yr2;
               yr2 = yr1;
               yr1 = s;
              }
       }

    if (mouse_b & 2) // boton derecho, de-seleccionar
      {
          xr1 = -1;
          yr1 = -1;
          xr2 = -1;
          yr2 = -1;

          while (mouse_b);
      }

    // Teclado...
    if (k == KEY_UP)    yp -= 40;
    if (k == KEY_DOWN)  yp += 40;

    if (k == KEY_V) vgrilla = !vgrilla;

    if (k == KEY_Q) c_sombra++;
    if (k == KEY_W) c_sombra--;

    if (k == KEY_O) w_sombra++;
    if (k == KEY_P) w_sombra--;

    w_sombra %=  256; //ajustar a rango 0..255
    if (w_sombra < 0) w_sombra = 255;
    c_sombra %=  256; //ajustar a rango 0..255
    if (c_sombra < 0) c_sombra = 255;

    if (k == KEY_T) snapgrid = !snapgrid;

    if (k == KEY_ESC) {
      if ( alert("Salir de la edicion.", "Esta seguro", "Se perderan los datos no salvados", "Si", "No", 'S', 'N') == 1) return;
    }
    
    if (k == KEY_S) {
       if (file_select_ex("Archivo a salvar?", salvar, NULL, 512, 0, 0))
       {
        if (!save_bitmap(salvar, fnd, pal)) { // DEBUG:falta hacer
         alert("Archivo salvado.", salvar, NULL, "OK", NULL, 0, 0);
        } else
        {
         alert("Fallo la apertura del archivo!", salvar, NULL, "OK", NULL, 0, 0);
        }
       };
    }

    if (k == KEY_L) {
       if (file_select_ex("Cargue la imagen por favor.", salvar, NULL, 512, 0, 0))
       {
        BITMAP *tmp;
        tmp = load_bitmap(salvar, pal);
        if (tmp != NULL)
        {
            destroy_bitmap(fnd);
            fnd = tmp;
            set_palette(pal);
            clear(screen);
gui_fg_color = makecol(255,255,255);
gui_bg_color = makecol(0,0,0);
set_mouse_sprite(NULL);
        
            /* esto aumenta un monton los fps (por el makecol acelerado... ) */
            create_rgb_table(&tmp_rgb, pal, NULL); /* rgb_map es una global de Allegro! */
            rgb_map = &tmp_rgb;
        
            create_trans_table(&trans_table, pal, 128, 128, 128, NULL); // transparencias
            color_map = &trans_table;

            alert("Archivo cargado.", salvar, NULL, "OK", NULL, 0, 0);
        }
        else
        {
         alert("Fallo la apertura del archivo!", salvar, NULL, "OK", NULL, 0, 0);
        }
       }
    }

    if (yp < 0) yp =0;
    if (yp > fnd->h - SCREEN_H) yp = fnd->h - SCREEN_H;

    redibujar();
    
   }

}



int main() {
   char leerbmp[1024]; /* fondo a cargar */

   int card = GFX_AUTODETECT, w = 640 ,h = 480 ,color_depth = 8;

   allegro_init();
   install_timer();
   install_keyboard();
   install_mouse();

   srand(time(0));

   set_gfx_mode(GFX_SAFE, 0,0,0,0);

   gfx_mode_select_ex(&card, &w, &h, &color_depth);

   set_color_depth(color_depth);
   if (set_gfx_mode(card, w, h, 0, 0)) {
      allegro_message("set_gfx_mode(%d x %d x %d bpp): %s\n", w,h, color_depth, allegro_error);
      return 1;
   }


   set_trans_blender(128, 128, 128, 128);

   leerbmp[0] = '\0';
   
   if (!file_select_ex("Cargue la imagen por favor.", leerbmp, NULL, 512, 0, 0)) return 0;

   fnd = load_bitmap(leerbmp, pal);
   if (fnd == NULL) {
      allegro_message("No se pueden cargar o es invalido \n %s!\n", leerbmp);
      return 1;
   }

     set_palette(pal);
     clear(screen);
gui_fg_color = makecol(255,255,255);
gui_bg_color = makecol(0,0,0);
set_mouse_sprite(NULL);

     /* esto aumenta un monton los fps (por el makecol acelerado... ) */
     create_rgb_table(&tmp_rgb, pal, NULL); /* rgb_map es una global de Allegro! */
     rgb_map = &tmp_rgb;

     create_trans_table(&trans_table, pal, 128, 128, 128, NULL); // transparencias
     color_map = &trans_table;

     buffer=create_bitmap(SCREEN_W, SCREEN_H);
     clear(buffer);
     show_mouse(screen);

     /* Rock & Roll! */
     editar_mapa();
     
return 0;
}
END_OF_MAIN();
