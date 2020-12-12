/*
  Editor de la matriz de mapa para Kraptor
  Kronoman 2002
  En memoria de mi querido padre
  Teclas:
  DELETE = casilla a 0
  BARRA = situar valor seleccionado
  FLECHAS = mover cursor
  + y - del keypad: alterar valor en +1
  1 y 2 : alterar valor en +10
  0 : valor a 0
  C : limpiar grilla a ceros
  S : salvar grilla
  L : cargar grilla
  V : ver fondo sin grilla
  ESC : salir

 NOTA: los mapas se guardan con las funciones especiales de Allegro
 para salvar los integers en un formato standard, de esa manera,
 me ahorro los problemas con diferentes tama~os de enteros segun
 la plataforma.

  */

#include <stdio.h>
#include <stdlib.h>
#include <allegro.h>

/* Tama¤o del fondo */
#define W_FONDO  600
#define H_FONDO  4000

/* Grilla WxH */
#define W_GR 40
#define H_GR 40

/* Grilla */
long grilla[W_FONDO / W_GR][H_FONDO / H_GR];

/* En vgrilla pasar -1 para dibujarla, o 0 para ver el bitmap suelto */
int vgrilla = -1;


int xp = 0, yp = 0, vp = 30; /* x,y, valor a colocar */

/* Bitmap cargado en RAM del fondo */
BITMAP *fnd;
PALETTE pal; /* paleta */

/* Buffer de dibujo */
BITMAP *buffer;

void limpiar_grilla() {
     int xx, yy;
      /* limpio la grilla a ceros */
     for (xx =0; xx < W_FONDO / W_GR; xx++)
     for (yy =0; yy < H_FONDO / H_GR; yy++)
         grilla[xx][yy] = 0;
}

void redibujar() {
   int iy, ix;

   clear(buffer);
   blit(fnd, buffer, 0, yp * H_GR,0,0,600,480);

   /* grilla */
   if (vgrilla)
   {
   for (ix=0; ix < W_FONDO / W_GR; ix ++ )
        line(buffer, ix*W_GR, 0, ix*W_GR, 480, makecol(255,255,255));

   for (iy=0; iy < H_FONDO / H_GR; iy ++ )
        line(buffer, 0, iy*H_GR, fnd->w, iy*H_GR, makecol(255,255,255));


   /* cursor */
   line(buffer, xp * W_GR, 0, (xp * W_GR) + W_GR, H_GR, makecol(255, 255, 255));
   line(buffer, xp * W_GR, H_GR, (xp * W_GR) + W_GR, 0, makecol(255, 255, 255));
   
   /* mostrar los valores que contiene la matriz */
   text_mode(makecol(0,0,0));
   for (ix=0; ix < W_FONDO / W_GR; ix ++ )
      for (iy=0; iy < H_FONDO / H_GR; iy ++ )
           if (yp + iy < H_FONDO / H_GR )
                    if (grilla[ix][yp+iy] != 0)
                    textprintf(buffer, font,
                               (ix*W_GR)+(W_GR/2), (iy*H_GR)+(H_GR/2), makecol(255,255,255),
                               "%d", (int)grilla[ix][yp+iy]);
   }
/* Informacion */
text_mode(-1); // texto (-1=trans, 1 solido)
textprintf(buffer, font, 600,0,makecol(255,255,255),
           "x:%d", xp);

textprintf(buffer, font, 600,20,makecol(255,255,255),
           "y:%d", yp);

textprintf(buffer, font, 600,40,makecol(255,255,255),
           "v:%d", vp);

/* mandar a pantalla */
scare_mouse();
blit(buffer, screen, 0,0, 0,0,SCREEN_W, SCREEN_H);
unscare_mouse();
}


void editar_mapa() {
   int k;
   char salvar[1024]; /* archivo a salvar */;
   salvar[0] = '\0';
   
   redibujar();
   

   while (1) {
    k = readkey() >> 8;
    
    if (k == KEY_UP) yp--;
    if (k == KEY_DOWN) yp++;
    if (k == KEY_LEFT) xp--;
    if (k == KEY_RIGHT) xp++;
    if (k == KEY_SPACE) grilla[xp][yp] = vp;
    if (k == KEY_DEL) grilla[xp][yp] = 0;
    if (k == KEY_PLUS_PAD) vp++;
    if (k == KEY_MINUS_PAD) vp--;
    if (k == KEY_0) vp = 0;
    if (k == KEY_1) vp += 10;
    if (k == KEY_2) vp -= 10;

    if (k == KEY_V) vgrilla = !vgrilla;

    if (k == KEY_ESC) {
      if ( alert("Salir de la edicion.", "Esta seguro", "Se perderan los datos no salvados", "Si", "No", 'S', 'N') == 1) return;
    }

    if (k == KEY_C) {
     if ( alert("Limpiar grilla.", "Esta seguro", NULL, "Si", "No", 'S', 'N') == 1)
        { limpiar_grilla(); alert("La grilla se reinicio a ceros (0)", NULL, NULL, "OK", NULL, 0, 0); }
    }
    
    if (k == KEY_S) {
       if (file_select_ex("Archivo de grilla a salvar?", salvar, NULL, 512, 0, 0))
       {
        PACKFILE *fp;
        int xx, yy;
        if ((fp = pack_fopen(salvar, F_WRITE)) != NULL) {

          // escribo la grilla en formato Intel de 32 bits
          for (xx =0; xx < W_FONDO / W_GR; xx++)
          for (yy =0; yy < H_FONDO / H_GR; yy++)
                  pack_iputl(grilla[xx][yy], fp);
         
         pack_fclose(fp);

         alert("Archivo salvado.", salvar, NULL, "OK", NULL, 0, 0);
        } else
        {
         alert("Fallo la apertura del archivo!", salvar, NULL, "OK", NULL, 0, 0);
        }
       };
    }

    if (k == KEY_L)
    {
       if (file_select_ex("Archivo a cargar?", salvar, NULL, 512, 0, 0))
       {
        PACKFILE *fp;
        int xx, yy;
        if ((fp = pack_fopen(salvar, F_READ)) != NULL) {
          // leo la grilla en formato Intel de 32 bits
          for (xx =0; xx < W_FONDO / W_GR; xx++)
          for (yy =0; yy < H_FONDO / H_GR; yy++)
                  grilla[xx][yy] = pack_igetl(fp);
                  
         pack_fclose(fp);

         alert("Archivo cargado.", salvar, NULL, "OK", NULL, 0, 0);
        }
        else
        {
         alert("Fallo la apertura del archivo!", salvar, NULL, "OK", NULL, 0, 0);
        }
       };
    }

    if (yp < 0) yp =0;
    if (yp > (H_FONDO / H_GR)-1) yp = (H_FONDO / H_GR)-1;

    if (xp < 0) xp =0;
    if (xp > (W_FONDO / W_GR)-1) xp = (W_FONDO / W_GR)-1;

    redibujar();
    
   }

}



int main() {
   RGB_MAP tmp_rgb; /* acelera los calculos de makecol, etc */
   char leerbmp[1024]; /* fondo a cargar */

   
   int card = GFX_AUTODETECT, w = 640 ,h = 480 ,color_depth = 8; /* agregado por Kronoman */

   allegro_init();
   install_timer();
   install_keyboard();
   install_mouse();

   srand(time(0));

   set_color_depth(color_depth);
   if (set_gfx_mode(card, w, h, 0, 0)) {
      allegro_message("set_gfx_mode(%d x %d x %d bpp): %s\n", w,h, color_depth, allegro_error);
      return 1;
   }

   leerbmp[0] = '\0';
   
   if (!file_select_ex("Cargue el fondo por favor.", leerbmp, NULL, 512, 0, 0)) return 0;

   fnd = load_bitmap(leerbmp, pal);
   if (fnd == NULL || fnd->w != W_FONDO || fnd->h != H_FONDO) {
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

     buffer=create_bitmap(SCREEN_W, SCREEN_H);
     clear(buffer);
     show_mouse(screen);


     limpiar_grilla();
     /* Rock & Roll! */
     editar_mapa();
     
return 0;
}
END_OF_MAIN();
