/*
 --------------------------------------------------------
 mapa.c 
 -------------------------------------------------------- 
 Copyright (c) Kronoman 
 En memoria de mi querido padre 
 -------------------------------------------------------- 
 Esto contiene todo lo referente al mapa en si
 Hace scrollings, quema areas, impulsa nuevos enemigos, etc
 Cargar un nuevo mapa esta en data.c :^P
 Afecta el nivel de detalle a la calidad de los graficos...
 --------------------------------------------------------
*/
#ifndef MAPA_C
#define MAPA_C

#include "azar.h"
#include "kfbuffer.h"
#include "allegro.h"
#include "mapa.h"
#include "explos.h"
#include "enemigo.h"
#include "combo.h"
#include "global.h"
#include "sonido.h"
#include "humo.h"

/* Globales  */
long mapa_g[GRILLA_W][GRILLA_H]; /* Grilla del mapa explosivo */
long enem_g[GRILLA_W][GRILLA_H]; /* Grilla de aparicion de enemigos */

int scroll_mapa = 0; /* scroll del mapa, en pixeles, va subiendo desde H_FONDO-ALTO_FB a 0 */
int scroll_mapa_speed = 1; /* velocidad de scroll de mapa, normalmente 1 */

COLOR_MAP tabla_quemado; /* tabla para realizar el 'quemado' del mapa con sprites */
BITMAP *burn_cache_bmp[3]; /* cache de sprites de 'quemado' */

/* Mapa de fondo */
BITMAP *mapa_fondo = NULL; 

/*
   Esta funcion detona el fondo cuando los disparos le pegan.
   Es recursiva.
   Pasarle las coordenadas de los disparos en grilla
   y el punch del disparo, si es -666, explota si o si!
   El punch se va incrementando a medida que explota
   Devuelve 0 si no toco nada, -1 si exploto y -2 si solo toco (para desaparecer el disparo)
*/
int explotar_fondo(int x, int y, int punch)
{
  
 /* fuera de grilla ? */
 if (x < 0) return 0;
 if (y < 0) return 0;
 if (x > GRILLA_W-1) return 0;
 if (y > GRILLA_H-1) return 0;

 if (mapa_g[x][y] == 0) return 0; /* este sector no es explosivo */

 if (mapa_g[x][y] > 0)
    {
    mapa_g[x][y] -= punch;

    if (punch == -666)
    {
        mapa_g[x][y] = -1; /* explosion obligatoria */
        punch = rand()%10+10; /* evita un bug que impide explosiones, dejando areas superduras */
    }

    if (mapa_g[x][y] <= 0) /* EXPLOTO! */
        {

            /* Quemar area (le paso el punto central en x,y) */
            quemar_area(mapa_fondo, x * W_GR + W_GR/2+rand_ex(-5,5) , y * H_GR + H_GR/2+rand_ex(-5,5) , 30, 40);

            /* explosion */
            pone_explo_pixel(&ptr_explo_fondo,
                             x * W_GR + W_GR/2+rand_ex(-5,5),
                             y * H_GR + H_GR/2+rand_ex(-5,5),
                             rand()%W_GR/2+W_GR, rand()%W_GR/2+W_GR/2,
                             ftofix((float)(rand()%10+10)/1000.0) );

            /* sonido */
            tocar_sonido_paneado(x * W_GR,
                                 explo_cache_snd[rand()%3],
                                 rand_ex(32, 64),
                                 rand_ex(800, 1200));

            /* emisor de humo en el piso - WOW! */
            agrega_emisor_humo(itofix(x * W_GR + W_GR/2 + rand_ex(-5,5)),
                               itofix(y * H_GR + H_GR/2 + rand_ex(-5,5)),
                               rand_ex(30,100));

            mapa_g[x][y] = 0; /* seguridad */
            /* explotar los 4 contiguos
              (diagonales no son necesarias,
              los contiguos las atrapan ),

              DEBUG:
              si explota el contiguo, quemar el area intermedia
              para darle mas efecto. */

            /* explosion obligatoria contigua */
            explotar_fondo(x-1, y, -666);
            explotar_fondo(x+1, y, -666);
            explotar_fondo(x, y-1, -666);
            explotar_fondo(x, y+1, -666);

            return -1; /* EXPLOTO */
        }

        /* no exploto, pero toco, explotar poquisimo */
            pone_explo_pixel(&ptr_explo_fondo,
                             x * W_GR + W_GR/2+rand_ex(-5,5),
                             y * H_GR + H_GR/2+rand_ex(-5,5),
                             rand()%10+10, rand()%10+10,
                             ftofix((float)(rand()%10+10)/1000.0) );

       return -2; 
    }

 if (mapa_g[x][y] < 0) mapa_g[x][y] = 0; /* seguridad */
 return 0;
}

/*
   Esta funcion se llama cuando inicia un nuevo nivel, para escanear todas
   las lineas visibles de mapa en la primera pantalla
*/
void scan_nivel_1era_vez()
{
    int y;
    for (y=H_FONDO-ALTO_FB; y < H_FONDO; y++)  scan_nuevos_enemigos(y / H_GR);
}

/*
   Esta funcion escanea una linea de la grilla
   y agrega los nuevos enemigos.
   Pasar en y la linea de la grilla a escanear (no pixels!)
   NOTA: Por lo general sera: scroll_mapa / H_GR
*/
void scan_nuevos_enemigos(int y)
{
 int x;
 int cx, cy; // para calcular el centro, auxiliares
 
 if (y < 0) y = 0;
 if (y > GRILLA_H-1) y = GRILLA_H-1;

 for (x = 0; x < GRILLA_W; x++)
     {

	// coloca el enemigo centrado...
         if (enem_g[x][y] > 0)
         {
            // desplazamiento para centrarlo
            cx = (W_GR / 2) - (enem_t[enem_g[x][y]-1].spr[0]->w / 2);
            cy = (H_GR / 2) - (enem_t[enem_g[x][y]-1].spr[0]->h / 2);
	    
	    // agregarlo...            
            agregar_enemigo(cx + (x*W_GR), cy + (y*H_GR), enem_g[x][y]-1 );
    	 }
    	 
         /* cada linea escaneada se coloca a 0, para no volver a usarla */
         enem_g[x][y] = 0;
     }
}

/*
   Esta funcion quema un area del bitmap
   utiliza funciones de transparencia
   Pasarle las coordenadas en pixeles y el bmp a 'quemar'
   ademas del radio del impacto (rad1) y el radio de las esquirlas (rad2)

   Las coordenadas son el *centro* de la quemazon!

   NOTA: mucho fue desabilitado a proposito (aunque funciona) porque
         queda mas lindo de esta manera... ;^)

   */
void quemar_area(BITMAP *bmp, int xp, int yp, int rad1, int rad2)
{
int x = xp, y = yp, r = 0, i = 0 , c = 0, c2 = 0;
  COLOR_MAP *color_map_bak = color_map; /* para restaur el color map */

// solid_mode();

drawing_mode(DRAW_MODE_TRANS, NULL, 0,0);


/* Impacto circular */

// c = makecol(0,0,0);


// r = rand_ex(nivel_detalle/2, nivel_detalle);
// r=r+2;
//  for(i =0; i < r; i++)
//  {
//     c2 = rand()%16;
//     c = makecol(c2,c2,c2);
//     circlefill(bmp,
//                x + rand_ex(-rad1/4, rad1/4), y + rand_ex(-rad1/4, rad1/4),
//                rand_ex(rad1/4, rad1/2 ), c); /* entre 1/4 y 1/2 del radio */
// }


/* Esquirlas */
r = rand_ex(nivel_detalle/2, nivel_detalle);
r=r+2;
  for(i =0; i < r; i++)
   {
     c2 = rand()%64;
     c = makecol(c2,c2,c2);
     
     //if (rand()%100 < 30)
     //    line(bmp, x, y,  x + rand_ex(-rad2, rad2), y + rand_ex(-rad2, rad2), c);
     //else
          triangle(bmp,
                    x + rand_ex(-rad2/3, rad2/3), y + rand_ex(-rad2/3, rad2/3),
                    x + rand_ex(-rad2/3, rad2/3), y + rand_ex(-rad2/3, rad2/3),
                    x + rand_ex(-rad2, rad2), y + rand_ex(-rad2, rad2), c);
   }


/* Sub impacto triangular */
// r = rand_ex(nivel_detalle/3, nivel_detalle/2);
// r=r+2;
//  for(i =0; i < r; i++)
//  {
//     c2 = rand()%32;
//     c = makecol(c2,c2,c2);
//
//     triangle(bmp,
//                x + rand_ex(-rad1, rad1), y + rand_ex(-rad1, rad1),
//                x + rand_ex(-rad1, rad1), y + rand_ex(-rad1, rad1),
//                x + rand_ex(-rad1, rad1), y + rand_ex(-rad1, rad1), c);
//  }

solid_mode();

// DEBUG: nuevo metodo, usa un sprite transparente rotado
// y un efecto de 'burn'
color_map = &tabla_quemado;
    /* centrar el bitmap de quemazon */
    c = rand_ex(0,2);
    x = xp - burn_cache_bmp[c]->w / 2;
    y = yp - burn_cache_bmp[c]->h / 2;
    draw_trans_sprite(mapa_fondo, burn_cache_bmp[c], x, y);
color_map = color_map_bak;


}


/* Esta funcion crea el mapa de transparencias
necesario para quemar, donde los tonos de negro dejan
el color igual, y los blancos queman el fondo
llamar:
 create_color_table(&tabla_a_crear,
                       paleta_a_usar,
                       crear_mapa_quemazon,
                       NULL);

NOTA: x es el color a dibujar
      y es el color sobre el que se dibuja


*/
void crear_mapa_quemazon(AL_CONST PALETTE pal, int x, int y, RGB *rgb)
{
   int r, g, b, r2, g2, b2;
   float h,s,v, h2,s2,v2;

   r = (int)pal[x].r;
   g = (int)pal[x].g;
   b = (int)pal[x].b;

   r2 = (int)pal[y].r;
   g2 = (int)pal[y].g;
   b2 = (int)pal[y].b;

/* si el color es suficientemente claro (oscuro para quemar), usarlo */
if (r > 3 && g > 3 && b > 3) 
 {
   /* invertir el color */
   r = 63 - r;
   g = 63 - g;
   b = 63 - b;

   /* no permitir demasiado brillo */
       r = MIN(48, r);
       g = MIN(48, g);
       b = MIN(48, b);

   /* tonalizar color destino (mult/div por 4 porque es 0..255) */
   rgb_to_hsv(r*4, g*4, b*4, &h, &s, &v);

   rgb_to_hsv(r2*4, g2*4, b2*4, &h2, &s2, &v2);

   /* tomo el valor minimo de luz, para que las cosas muy oscuras
      no sean afectadas y aclaradas por mi quemazon (quedaria mal) */
   hsv_to_rgb(h2, s2, MIN(v, v2), &r, &g, &b);

   r /= 4;
   g /= 4;
   b /= 4;
 }
 else /* dejar color original */
 {
   r = r2;
   g = g2;
   b = b2;
 }

rgb->r = r;
rgb->g = g;
rgb->b = b;
} // fin crear_mapa_aditivo


#endif
