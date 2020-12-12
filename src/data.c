// -------------------------------------------------------- 
// data.c 
// -------------------------------------------------------- 
// Copyright (c) Kronoman 
// En memoria de mi querido padre 
// -------------------------------------------------------- 
// Esto se encarga de cargar y poner en memoria
// los datos leidos de disco.
// -------------------------------------------------------- 
#ifndef DATA_C
#define DATA_C

#include <stdio.h>
#include <string.h>
#include "allegro.h"

#include "data.h"
#include "error.h"
#include "global.h"
#include "jugador.h"
#include "enemigo.h"
#include "pmask.h"
#include "sombras.h"
#include "datafind.h"
#include "explos.h"
#include "mapa.h"
#include "partic.h"
#include "menu.h"
#include "shopping.h"
#include "humo.h" 
#include "ia.h"
#include "premio.h"
#include "filedata.h"
#include "bomba.h"

/* Globales */

    /* Este datafile contiene los datos principales del juego
       tales como el script de secuencia de niveles,
       graficos de menues, paleta de juego, armamento,
       enemigos, sprites del jugador, premios, sonidos
       Se carga al inicio del programa, y se descarga al salir
       */
DATAFILE *krapmain = NULL;

    /* Cada fondo y mapa de juego esta puesto por separado
       en un DAT para evitar sobrecarga de memoria.
       Este datafile es cargado y descargado a medida que pasan
       los niveles.
       Contiene el fondo (600x4000) y el mapa de juego
       */
DATAFILE *datmapa = NULL;

/* Esto contiene los graficos para los menues */
DATAFILE *datmenu = NULL;

/* globales internas */
static int byte_acumulado = 0; /* auxiliar para contar bytes leidos (no necesario) */

/* Funcion interna callback, que muestra cuanto se ha leido...
   en la esquina superior izquierda, usando colores
   del GUI para el texto
   La primera vez que es llamado (byte_acumulado == 0) dibuja una caja de texto
   Esta dise~ado para no atrasar mucho la carga */
static void cargar_callback(DATAFILE *d)
{
 if (byte_acumulado <= 0)
    {
     byte_acumulado = 0;
     /* fondo */
     rectfill(screen, 0,0,SCREEN_W-1, (text_height(font)+5)*2, gui_bg_color);
     /* sombra (negro) */
     rectfill(screen, 5,5,SCREEN_W-5, (text_height(font)+10)*2, makecol(0,0,0));
     /* borde */
     rect(screen, 0,0,SCREEN_W-1, (text_height(font)+5)*2, gui_fg_color);
    }

 byte_acumulado += d->size;
 text_mode(gui_bg_color);
 textprintf(screen, font, 2, 2, gui_fg_color, "Wait please, loading...");
 textprintf(screen, font, 2, text_height(font)+4, gui_fg_color, "-> %d", byte_acumulado);
}


/* Esta funcion carga los datos principales del juego en memoria,
   es decir, paletas, jugador, enemigo, IA, armamento
   Afecta a  *krapmain
   MUESTRA UN MENSAJE DE CARGA EN PANTALLA! [ESQUINA SUP-IZQ]
   */
void cargar_datos_principales()
{
   DATAFILE *p = NULL; /* puntero de busqueda */
   int i, i2; /* para los fors */
   char tmpstr[1024], tmpstr2[1024]; /* uso general */

   if (krapmain != NULL) unload_datafile(krapmain);
   if (datmenu != NULL) unload_datafile(datmenu);

   set_color_conversion(COLORCONV_TOTAL | COLORCONV_KEEP_TRANS);

   /* colores para el callback que muestra el progreso de la carga */
   gui_fg_color = makecol(255,255,255);
   gui_bg_color = makecol(0,0,0);
   byte_acumulado = 0; /* contador para mostrar progreso */

   /* primero, trato de cargar SOLO la paleta de juego,
      esto es para poder convertir las imagenes a los bpp adecuados */
   krapmain = krono_load_datafile_object("krapmain.dat", "pal_game");
   if (krapmain == NULL)  levantar_error("ERROR: no existe pal_game en krapmain.dat");

       /* copiar la paleta a RAM ... */
       memcpy(pal_game, krapmain->dat, sizeof(PALETTE));
    
       /* Paletas de colores y mapas de transparencias... [ya estan cargadas, hago calculos...] */
       set_palette(pal_game); /* la seteo, la precisa la sombra... */
    
       /* Crear mapa de transparencia de 8 bits */
       create_trans_table(&tabla_transparencia, pal_game, 128,128,128, NULL);
       color_map = &tabla_transparencia;  /* la rutina de sombras la precisa! */
    
       /* Crear mapa RGB de 8 bits */
       create_rgb_table(&tabla_RGB, pal_game, NULL);
       rgb_map = &tabla_RGB;

       /* Crear mapa para 'quemado' del fondo */
 create_color_table(&tabla_quemado,
                       pal_game,
                       crear_mapa_quemazon,
                       NULL);
       
   /* ya no preciso mas la paleta en RAM */
   unload_datafile_object(krapmain);
   krapmain = NULL;

   /* Cargo el cache de inteligencia artificial */
   hacer_chache_ia("ia.dat");

   /* Ahora si, cargar TODO el archivo de datos */
   gui_bg_color = makecol(0,0,0);  gui_fg_color = makecol(255,255,255); // colores para la info de carga
   krapmain = krono_load_datafile_callback("krapmain.dat", cargar_callback);
   if (krapmain == NULL) levantar_error("ERROR: no se pudo cargar krapmain.dat");

   /* DEBUG: falta ajustar datafile al modo de video actual */
   fixup_datafile(krapmain);

   p = find_datafile_object(krapmain, "gamescript");
   if (p == NULL) levantar_error("ERROR: el gamescript no esta definido en krapmain.dat");

   set_config_data((char *)p->dat, p->size);


   /* obtener la cinematica de introduccion al primer nivel */
   sprintf(game_intro_cine, "%s", get_config_string("game_intro", "cine", "null"));

   /* obtener la cinematica de game over */
   sprintf(game_over_cine, "%s", get_config_string("game_over", "cine", "null"));

   /* font para el 'hud' */
   p = find_datafile_object(krapmain, "hud_font");
   if (p == NULL)
     hud_font = font_backup;
   else
     hud_font = (FONT *)p->dat;

   /* Cargar explosiones, particulas, sonidos y sprite quemador en cache */
    for (i = 0; i < 3; i++)
    {
     sprintf(tmpstr,"explo_bmp_%d", i);
     p = find_datafile_object_type(krapmain, tmpstr, DAT_BITMAP);
     if (p == NULL) levantar_error("ERROR: falta un objeto explo_bmp_[0..2]");
     explo_cache_bmp[i] = (BITMAP *)p->dat;

     sprintf(tmpstr,"explo_snd_%d", i);
     p = find_datafile_object_type(krapmain, tmpstr, DAT_SAMPLE);
     if (p == NULL) levantar_error("ERROR: falta un objeto explo_snd_[0..2]");
     explo_cache_snd[i] = (SAMPLE *)p->dat;
     explo_cache_snd[i]->priority = 30; /* prioridad baja a las explosiones */

     sprintf(tmpstr,"chispa_%d", i);
     p = find_datafile_object_type(krapmain, tmpstr, DAT_BITMAP);
     if (p == NULL) levantar_error("ERROR: falta un objeto chispa_[0..2]");
     particula_cache_bmp[i] = (BITMAP *)p->dat;

     sprintf(tmpstr,"burn_%d", i);
     p = find_datafile_object_type(krapmain, tmpstr, DAT_BITMAP);
     if (p == NULL) levantar_error("ERROR: falta un objeto burn_[0..2]");
     burn_cache_bmp[i] = (BITMAP *)p->dat;

    }
    // Cargar humo en cache
     p = find_datafile_object_type(krapmain, "humo", DAT_BITMAP);
     if (p == NULL)
             humo_spr = NULL;
     else
             humo_spr = (BITMAP *)p->dat;


   /* Script del jugador a partir de aqui */

   p = find_datafile_object(krapmain, "jugador");
   if (p == NULL) levantar_error("ERROR: el jugador no esta definido en krapmain.dat");

   set_config_data((char *)p->dat, p->size);

   /* Cargar sprites del jugador */
   for (i = 0; i < 3; i++)
    {
     sprintf(tmpstr,"spr_%d", i);

     p = find_datafile_object_type(krapmain, get_config_string("jugador", tmpstr, "null"), DAT_BITMAP);
     if (p == NULL) levantar_error("ERROR: en el jugador: spr_* no esta definido en krapmain.dat");
     jugador.spr[i] = (BITMAP *)p->dat;

     jugador.spr[i+3] = create_bitmap(jugador.spr[i]->w * 0.8, jugador.spr[i]->h * 0.8);
     hacer_sombra(jugador.spr[i], jugador.spr[i+3]);
    }

   /* dinero y bombas al iniciar un nuevo juego */
   jugador.init_money = get_config_int("JUGADOR","init_money", 0);
   jugador.init_bombas = get_config_int("JUGADOR","init_bombas", 0);

   /* reparacion de la nave */
   jugador.reparar_cantidad = get_config_int("JUGADOR_REPARAR","cantidad", 10);
   jugador.reparar_precio = get_config_int("JUGADOR_REPARAR","precio" , 1);
   sprintf(jugador.reparar_desc,"%s", get_config_string("JUGADOR_REPARAR", "desc", "Fix the ship") );

   p = find_datafile_object_type(krapmain, get_config_string("JUGADOR_REPARAR", "bmp", "null"), DAT_BITMAP);
   if (p == NULL)
     jugador.reparar_bmp = NULL;
   else
      jugador.reparar_bmp = (BITMAP *)p->dat;

   /* bombas especiales de la nave */
   jugador.bomba_cantidad = get_config_int("JUGADOR_BOMBA_ESPECIAL","cantidad", 10);
   jugador.bomba_precio = get_config_int("JUGADOR_BOMBA_ESPECIAL","precio" , 10);
   jugador.max_bombas = get_config_int("JUGADOR_BOMBA_ESPECIAL","max_ammo" , 5);
   sprintf(jugador.bomba_desc,"%s", get_config_string("JUGADOR_BOMBA_ESPECIAL", "desc", "Special bomb") );

   p = find_datafile_object_type(krapmain, get_config_string("JUGADOR_BOMBA_ESPECIAL", "bmp", "null"), DAT_BITMAP);
   if (p == NULL)
     jugador.bomba_bmp = NULL;
   else
      jugador.bomba_bmp = (BITMAP *)p->dat;

   p = find_datafile_object_type(krapmain, get_config_string("JUGADOR_BOMBA_ESPECIAL", "bomba_sonido", "null"), DAT_SAMPLE);
   if (p == NULL)
     bomba_sonido = NULL;
   else
     bomba_sonido = (SAMPLE *)p->dat;


   /* mascara de colision del jugador */
   jugador.mask = create_allegro_pmask(jugador.spr[1]);

 
   /* Armamento del jugador */
   for (i=0; i < MAX_ARM_CLASS; i++)
       {
        sprintf(tmpstr,"ARMA_%d", i);

        arma_tipo[i].arma = NULL;
        p = find_datafile_object_type(krapmain,  get_config_string(tmpstr, "arma", "null"), DAT_BITMAP);
        if (p != NULL )  arma_tipo[i].arma = (BITMAP *)p->dat;

        sprintf(arma_tipo[i].desc,"%s", get_config_string(tmpstr, "desc", "null"));
        sprintf(arma_tipo[i].desc_short,"%s", get_config_string(tmpstr, "desc_short", "null"));
        arma_tipo[i].desc[2048] = '\0';
        arma_tipo[i].desc_short[20] = '\0';

        arma_tipo[i].precio = get_config_int(tmpstr, "precio", -1);
        arma_tipo[i].cant_ammo = get_config_int(tmpstr, "cant_ammo", 0);
        arma_tipo[i].cant_ammo_max = get_config_int(tmpstr, "cant_ammo_max", 0);

        arma_tipo[i].spr = NULL;
        p = find_datafile_object_type(krapmain,  get_config_string(tmpstr, "spr", "null"), DAT_BITMAP);
        if (p != NULL) arma_tipo[i].spr = (BITMAP *)p->dat;

        arma_tipo[i].mask = NULL;

        if (arma_tipo[i].spr != NULL)
            {
                arma_tipo[i].mask = create_allegro_pmask(arma_tipo[i].spr);
            }

        arma_tipo[i].snd[0] = NULL;
        p = find_datafile_object_type(krapmain,  get_config_string(tmpstr, "snd_0", "null"), DAT_SAMPLE);
        if (p != NULL)
        {
            arma_tipo[i].snd[0] = (SAMPLE *)p->dat;
            arma_tipo[i].snd[0]->priority = 140; /* prioridad  */
        }

        arma_tipo[i].snd[1] = NULL;
        p = find_datafile_object_type(krapmain,  get_config_string(tmpstr, "snd_1", "null"), DAT_SAMPLE);
        if (p != NULL)
        {
            arma_tipo[i].snd[1] = (SAMPLE *)p->dat;
            arma_tipo[i].snd[1]->priority = 50; /* prioridad  */
        }

        arma_tipo[i].vx = ftofix(get_config_float(tmpstr,"vx", 0.0));
        arma_tipo[i].vy = ftofix(get_config_float(tmpstr,"vy", 0.0));
        arma_tipo[i].vida = get_config_int(tmpstr,"vida", -1);
        arma_tipo[i].punch = get_config_int(tmpstr,"punch", 0);
        arma_tipo[i].firerate = get_config_int(tmpstr,"firerate", 0);
        arma_tipo[i].t = get_config_int(tmpstr,"t", 0);

        /* Estela del disparo... */
        arma_tipo[i].est_vida[0] = get_config_int(tmpstr, "est_vida_min", 0);
        arma_tipo[i].est_vida[1] = get_config_int(tmpstr, "est_vida_max", 0);
        arma_tipo[i].est_cant[0] = get_config_int(tmpstr, "est_cant_min", 0);
        arma_tipo[i].est_cant[1] = get_config_int(tmpstr, "est_cant_max", 0);

        arma_tipo[i].est_color[0][0] = get_config_int(tmpstr, "est_color_r_min", 0);
        arma_tipo[i].est_color[0][1] = get_config_int(tmpstr, "est_color_r_max", 0);
        arma_tipo[i].est_color[1][0] = get_config_int(tmpstr, "est_color_g_min", 0);
        arma_tipo[i].est_color[1][1] = get_config_int(tmpstr, "est_color_g_max", 0);
        arma_tipo[i].est_color[2][0] = get_config_int(tmpstr, "est_color_b_min", 0);
        arma_tipo[i].est_color[2][1] = get_config_int(tmpstr, "est_color_b_max", 0);

        arma_tipo[i].est_dx[0] = get_config_int(tmpstr, "est_dx_min", 0);
        arma_tipo[i].est_dx[1] = get_config_int(tmpstr, "est_dx_max", 0);

        arma_tipo[i].est_dy[0] = get_config_int(tmpstr, "est_dy_min", 0);
        arma_tipo[i].est_dy[1] = get_config_int(tmpstr, "est_dy_max", 0);

        arma_tipo[i].est_tipo[0] = get_config_int(tmpstr, "est_tipo_min", 0);
        arma_tipo[i].est_tipo[1] = get_config_int(tmpstr, "est_tipo_max", 0);
	
	arma_tipo[i].est_rad[0] = get_config_int(tmpstr, "est_rad_min", 1);
	arma_tipo[i].est_rad[1] = get_config_int(tmpstr, "est_rad_max", 2);

	arma_tipo[i].est_transp = get_config_int(tmpstr, "est_transp", 0);
       }

   /* Cargar premios */
   p = find_datafile_object(krapmain, "premios");
   if (p == NULL) levantar_error("ERROR: los premios no estan definidos en krapmain.dat");
   set_config_data((char *)p->dat, p->size);
   
   for (i = 0; i < MAX_PREMIO_CLASS; i++)
   {
   sprintf(tmpstr,"PREMIO_%d", i);
   premio_class[i].premiar = get_config_int(tmpstr,"premiar", -666);
   premio_class[i].cantidad = get_config_int(tmpstr,"cantidad", 0);
   premio_class[i].vida = get_config_int(tmpstr,"vida", 0);

   // Obtener bitmap
   p = find_datafile_object_type(krapmain, get_config_string(tmpstr, "sprite", "null"), DAT_BITMAP);
   if (p != NULL)
      premio_class[i].sprite = (BITMAP *)p->dat;
   else
      premio_class[i].sprite = NULL;

   // obtener sonido
   p = find_datafile_object_type(krapmain, get_config_string(tmpstr, "sonido", "null"), DAT_SAMPLE);
   if (p != NULL)
      premio_class[i].sonido = (SAMPLE *)p->dat;
   else
      premio_class[i].sonido = NULL;
    
   }

   /* Script de enemigos a partir de aqui */

   /* Cargar clases de enemigos */
   p = find_datafile_object(krapmain, "enemigos");
   if (p == NULL) levantar_error("ERROR: los enemigos no estan definidos en krapmain.dat");
   set_config_data((char *)p->dat, p->size);

   /* Cargar enemigos, tipo de IA que le corresponde, y tipos de armas */
   for (i=0; i < MAX_E_CLASS; i++)
       {

        /* Seccion ENEMIGO_n */
        sprintf(tmpstr,"ENEMIGO_%d", i);

        enem_t[i].vida = get_config_int(tmpstr,"vida", -1);

        enem_t[i].peso = get_config_int(tmpstr,"peso", 0);
      
        enem_t[i].dinero = get_config_int(tmpstr,"dinero", 0);
        
        /* premios que suelta el enemigo */
        enem_t[i].premio_idx = get_config_int(tmpstr,"premio_idx", -1);
        enem_t[i].premio_prob = get_config_int(tmpstr,"premio_prob", -1);

        enem_t[i].spr_delay = get_config_int(tmpstr,"spr_delay", 10); //delay de animacion

        // bytecodes de tipo de IA   
        enem_t[i].ia_node = buscar_lista_ia(get_config_string(tmpstr,"tipo_ia","null"));  
        enem_t[i].ia_azar = get_config_int(tmpstr,"ia_azar", 0); // ia azaroza?
        enem_t[i].ia_boss = get_config_int(tmpstr,"ia_boss", 0); // ia boss?
           
      // cargar los sprites de animacion del enemigo
      for (i2 = 0; i2<4; i2++)
      {
        enem_t[i].spr[i2] = NULL;
        sprintf(tmpstr2, "spr_%d", i2);
        p = find_datafile_object_type(krapmain, get_config_string(tmpstr, tmpstr2, "null"), DAT_BITMAP);
        // intentar con spr, si no existe spr_0
        if (i2 == 0 && p == NULL) p = find_datafile_object_type(krapmain, get_config_string(tmpstr, "spr", "null"), DAT_BITMAP);
        // cuando es el primer sprite, se obvia si no existe, en caso contrario, el resto se asigna al primero
        if (p != NULL)
          { enem_t[i].spr[i2] = (BITMAP *)p->dat; }
          else
          {
          if (i2 == 0)
            { enem_t[i].spr[i2] = NULL; } // no hay sprite inicial
            else
            { enem_t[i].spr[i2] = enem_t[i].spr[0]; } // si no existe, tomo el primero
          }

        enem_t[i].mask[i2] = NULL;
        if (enem_t[i].spr[i2] != NULL) /* sombra y mascara de colision */
           {
            enem_t[i].spr_shadow[i2] = create_bitmap(enem_t[i].spr[i2]->w  * 0.8, enem_t[i].spr[i2]->h  * 0.8);
            hacer_sombra(enem_t[i].spr[i2] , enem_t[i].spr_shadow[i2]); // sombra
            enem_t[i].mask[i2] = create_allegro_pmask(enem_t[i].spr[i2]); // mascara de colision
           }
      }

        /*------ Seccion ARMA_n ------*/
        sprintf(tmpstr,"ARMA_%d", i);

        arma_ene[i].spr = NULL;
        p = find_datafile_object_type(krapmain,  get_config_string(tmpstr, "spr", "null"), DAT_BITMAP);
        if (p != NULL) arma_ene[i].spr = (BITMAP *)p->dat;

        arma_ene[i].mask = NULL;

        if (arma_ene[i].spr != NULL)
             {
                arma_ene[i].mask = create_allegro_pmask(arma_ene[i].spr);
             }

        arma_ene[i].snd[0] = NULL;
        arma_ene[i].snd[1] = NULL;

        p = find_datafile_object_type(krapmain,  get_config_string(tmpstr, "snd_0", "null"), DAT_SAMPLE);
        if (p != NULL)
        {
            arma_ene[i].snd[0] = (SAMPLE *)p->dat;
            arma_ene[i].snd[0]->priority = 100; /* prioridad  */
        }

        p = find_datafile_object_type(krapmain,  get_config_string(tmpstr, "snd_1", "null"), DAT_SAMPLE);
        if (p != NULL)
        {
            arma_ene[i].snd[1] = (SAMPLE *)p->dat;
            arma_ene[i].snd[1]->priority = 40; /* prioridad  */
        }

        arma_ene[i].vx = ftofix(get_config_float(tmpstr,"vx", 0.0));
        arma_ene[i].vy = ftofix(get_config_float(tmpstr,"vy", 0.0));
        arma_ene[i].vida = get_config_int(tmpstr,"vida", -1);
        arma_ene[i].punch = get_config_int(tmpstr,"punch", 0);
        arma_ene[i].t = get_config_int(tmpstr,"t", 0);

        /* Estela del disparo... */
        arma_ene[i].est_vida[0] = get_config_int(tmpstr, "est_vida_min", 0);
        arma_ene[i].est_vida[1] = get_config_int(tmpstr, "est_vida_max", 0);
        arma_ene[i].est_cant[0] = get_config_int(tmpstr, "est_cant_min", 0);
        arma_ene[i].est_cant[1] = get_config_int(tmpstr, "est_cant_max", 0);

        arma_ene[i].est_color[0][0] = get_config_int(tmpstr, "est_color_r_min", 0);
        arma_ene[i].est_color[0][1] = get_config_int(tmpstr, "est_color_r_max", 0);
        arma_ene[i].est_color[1][0] = get_config_int(tmpstr, "est_color_g_min", 0);
        arma_ene[i].est_color[1][1] = get_config_int(tmpstr, "est_color_g_max", 0);
        arma_ene[i].est_color[2][0] = get_config_int(tmpstr, "est_color_b_min", 0);
        arma_ene[i].est_color[2][1] = get_config_int(tmpstr, "est_color_b_max", 0);

        arma_ene[i].est_dx[0] = get_config_int(tmpstr, "est_dx_min", 100);
        arma_ene[i].est_dx[1] = get_config_int(tmpstr, "est_dx_max", 100);

        arma_ene[i].est_dy[0] = get_config_int(tmpstr, "est_dy_min", 0);
        arma_ene[i].est_dy[1] = get_config_int(tmpstr, "est_dy_max", 0);

        arma_ene[i].est_tipo[0] = get_config_int(tmpstr, "est_tipo_min", 0);
        arma_ene[i].est_tipo[1] = get_config_int(tmpstr, "est_tipo_max", 0);

	arma_ene[i].est_rad[0] = get_config_int(tmpstr, "est_rad_min", 1);
	arma_ene[i].est_rad[1] = get_config_int(tmpstr, "est_rad_max", 2);

	arma_ene[i].est_transp = get_config_int(tmpstr, "est_transp", 0);
        
       }

   // Fondo del shopping
     p = find_datafile_object_type(krapmain, "shop_bmp", DAT_BITMAP);
     if (p == NULL)
        shop_bmp = NULL;
     else
        shop_bmp = (BITMAP *)p->dat;


   /* cargar los items del menu principal  */
       if (datmenu != NULL) unload_datafile(datmenu);
       gui_bg_color = makecol(0,0,0);  gui_fg_color = makecol(255,255,255); // colores para la info de carga
       datmenu = krono_load_datafile_callback("krapmnu.dat", cargar_callback);
       if (datmenu == NULL)  levantar_error("ERROR: no se pudo abrir 'krapmnu.dat'");

       /* paleta de colores */
       p = find_datafile_object(datmenu, "main_menu_pal");
       if (p == NULL) levantar_error("ERROR: no se pudo obtener 'main_menu_pal'");
    
           /* copiar la paleta a RAM ... */
           memcpy(pal_menu_main, p->dat, sizeof(PALETTE));

       /* cargar archivo de ayuda (texto) */
       p = find_datafile_object(datmenu, "help_txt");
       if (p == NULL)
         { texto_ayuda_juego = NULL; }
        else
         {
          texto_ayuda_juego = (char *)p->dat;
          /* esto es necesario, sino, no tiene fin la cadena! */
          texto_ayuda_juego[p->size - 1] = '\0';
         }

       /* acerca de... */
       p = find_datafile_object(datmenu, "about_txt");
       if (p == NULL)
         { texto_acerca_de_juego = NULL; }
        else
         {
          texto_acerca_de_juego = (char *)p->dat;
          /* esto es necesario, sino, no tiene fin la cadena! */
          texto_acerca_de_juego[p->size - 1] = '\0';
         }

       /* imagen de acerca de... */
       p = find_datafile_object_type(datmenu, "about_bmp", DAT_BITMAP);
       if (p == NULL) 
             acerca_de_bmp  = NULL;
       else
            acerca_de_bmp = (BITMAP *)p->dat; 

       /* cargo el bitmap de fondo */
       p = find_datafile_object_type(datmenu, "main_menu_bmp", DAT_BITMAP);
       if (p == NULL)  levantar_error("ERROR: no existe 'main_menu_bmp' en 'krapmnu.dat'");
    
       bmp_menu_main = (BITMAP *)p->dat; 

   /* fin de los items del menu */

   /* DEBUG: en caso necesario agregar mas datos necesarios aqui */

}


/*
    Esta funcion puede hacer 2 cosas:
    - verificar si el proximo nivel existe
    - o verificar si existe y cargar el nivel especificado en RAM

    parametros:
    nivel => nivel a cargar
    solo_verificar => 0 indica que cargue el nivel en RAM,
                      -1 que solo verifique
                      SIEMPRE son cargados los datos basicos del nivel (cinematicas, titulo)

    devuelve 0 si funciona, -1 si falla (el nivel no existe, o el archivo no existe)

    SI SE VA A CARGAR EL NIVEL:
    MUESTRA UN MENSAJE DE CARGA EN PANTALLA! [ESQUINA SUP-IZQ]

    Nota: grilla de nivel/enemigos se guarda en enteros Intel 32 bits, para zafar en
    todas las plataformas
*/
int cargar_nivel(int nivel, int solo_verificar)
{
   DATAFILE *p = NULL; /* puntero de busqueda */
   PACKFILE *fp; // para leer las grillas
   int xx, yy; // para leer las grillas
   char tmpstr[1024]; // uso general
   char tmpstr2[1024];
   char fname_buff[2048];
   
   if (krapmain == NULL) return -1;

   p = find_datafile_object(krapmain, "gamescript");
   if (p == NULL) levantar_error("ERROR: el gamescript no esta definido en krapmain.dat");

   set_config_data((char *)p->dat, p->size);

   sprintf(tmpstr,"nivel_%d", nivel);

   /* cargar siempre la info basica del nivel... (si no, las cinematicas no andan! )*/
   sprintf(info_nivel.level_dat, "%s", get_config_string(tmpstr, "level_dat", "null"));
   sprintf(info_nivel.cine_in, "%s", get_config_string(tmpstr, "cine_in", "null"));
   sprintf(info_nivel.cine_out, "%s", get_config_string(tmpstr, "cine_out", "null"));
   sprintf(info_nivel.texto, "%s", get_config_string(tmpstr, "texto", "null"));
   sprintf(info_nivel.titulo, "%s", get_config_string(tmpstr, "titulo", "null"));
   info_nivel.clima_c = get_config_int(tmpstr, "clima_c", 0);
   info_nivel.clima_t = get_config_int(tmpstr, "clima_t", 0);
   info_nivel.clima_d = get_config_int(tmpstr, "clima_d", 0);

   info_nivel.musica = NULL; /* por ahora, no se si hay musica... */

   /* Debo SOLO verificar si el nivel existe? */
   if (solo_verificar)
      return !exists(where_is_the_filename(fname_buff, info_nivel.level_dat));

   /* ------ Cargar realmente en RAM el archivo...  ------ */

   /* colores para el callback que muestra el progreso de la carga */
   gui_fg_color = makecol(255,255,255);
   gui_bg_color = makecol(0,0,0);
   byte_acumulado = 0; /* contador para mostrar progreso */

   if (datmapa != NULL)
    {
         unload_datafile(datmapa);
         datmapa = NULL;
    }
   gui_bg_color = makecol(0,0,0);  gui_fg_color = makecol(255,255,255); // colores para la info de carga
   datmapa = krono_load_datafile_callback(info_nivel.level_dat, cargar_callback);
   if (datmapa == NULL) return -1; /* fallo la carga del nivel */

   /* copiarse las grillas */
   sprintf(tmpstr2, "%s#mapa_g", where_is_the_filename(fname_buff, info_nivel.level_dat)); // cargar el mapa_g
   fp = pack_fopen(tmpstr2, F_READ);
   if (fp == NULL) levantar_error("ERROR: no existe mapa_g en el nivel!");
          for (xx =0; xx < W_FONDO / W_GR; xx++)
          for (yy =0; yy < H_FONDO / H_GR; yy++)
                  mapa_g[xx][yy] = pack_igetl(fp);
                  
         pack_fclose(fp);
   
// grilla de enemigos, con soporte para varias dificultades
   sprintf(tmpstr2, "%s#enem_g_%d", where_is_the_filename(fname_buff, info_nivel.level_dat), nivel_de_dificultad);
   fp = pack_fopen(tmpstr2, F_READ);
   if (fp == NULL)
       {
        sprintf(tmpstr2, "%s#enem_g", where_is_the_filename(fname_buff, info_nivel.level_dat) );
        fp = pack_fopen(tmpstr2, F_READ);
        if (fp == NULL) levantar_error("ERROR: no existe enem_g en el nivel!");
       }

          for (xx =0; xx < W_FONDO / W_GR; xx++)
          for (yy =0; yy < H_FONDO / H_GR; yy++)
                 enem_g[xx][yy] = pack_igetl(fp);
                  
         pack_fclose(fp);

   /* obtener el fondo y ajustar dimensiones en un mapa nuevo...
      esto ayuda a conservar espacio en disco,
      para demos, permitiendo usar un fondo mas chico en disco! */

   p = find_datafile_object(datmapa, "mapa_fondo");
   if (p == NULL) levantar_error("ERROR: no existe mapa_fondo en el nivel!");

   mapa_fondo = create_bitmap(W_FONDO, H_FONDO); /* acordarse de liberar esto luego... en game.c */

   if (mapa_fondo == NULL) levantar_error("ERROR: data.c:cargar_nivel()\nInsuficiente memoria RAM para cargar el mapa del nivel!\n(~2.3MB requeridos!)");

   stretch_blit((BITMAP *)p->dat, mapa_fondo, 0, 0,
                 ((BITMAP *)p->dat)->w, ((BITMAP *)p->dat)->h,
                 0, 0,
                 mapa_fondo->w, mapa_fondo->h);

  /* Tomo la musica del juego */
  info_nivel.musica = NULL;
  p = find_datafile_object(datmapa, "musica");
  if (p != NULL)  info_nivel.musica = (DUH *)p->dat;

   return 0; /* Todo bien, negrita... */
}

#endif
