/*
 --------------------------------------------------------
 cinema.c
 --------------------------------------------------------
 Copyright (c) Septiembre 2002, Kronoman
 En memoria de mi querido padre
 --------------------------------------------------------
 Engine de cinematicas mediante scripts y datafiles
 Nota: quiere_videos afecta si esto funciona o no. :D
 -------------------------------------------------------- */

#ifndef CINEMA_C
#define CINEMA_C

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "allegro.h"

#include "krstring.h"
#include "kfbuffer.h" /* dibuja en el buffer, debe estar funcionando OK... */
#include "cinema.h"
#include "error.h"
#include "wordwrap.h"
#include "datafind.h"
#include "global.h"
#include "filedata.h"

/* Listado de comandos disponibles al script principal */
CMD_SCRIPT_TYPE cmd_list[] =
{
 { "cls",          cmd_cls,              3    },
 { "fade_out",     cmd_fade_out,         1    },
 { "fade_out_color", cmd_fade_out_color, 4    },
 { "rect",         cmd_rect,             7    },
 { "rectfill",     cmd_rectfill,         7    },
 { "line",         cmd_line,             7    },
 { "locate",       cmd_locate,           3    },
 { "text_color",   cmd_text_color,       3    },
 { "text_back",    cmd_text_back ,       3    },
 { "text_font",    cmd_text_font,        0    },
 { "echo",         cmd_echo,             0    },
 { "echo_centre_x",cmd_echo_centre_x,    0    },
 { "echo_centre_xy", cmd_echo_centre_xy, 0    },
 { "rest",         cmd_rest,             1    },
 { "set_palette",  cmd_set_palette,      1    },
 { "set_palette_default", cmd_set_palette_default, 0 },
 { "blit",         cmd_blit,             3    },
 { "sprite",       cmd_sprite,           3    },
 { "stretch_blit", cmd_stretch_blit,     5    },
 { "stretch_blit",  cmd_stretch_blit,    5    },
 { "play_sample",   cmd_play_sample,     4    },
 { "play_fli",      cmd_play_fli,        7    },
 { "clear_fli_back",cmd_clear_fli_back,  3    },
 { "keyboard_cancel_fli", cmd_keyboard_cancel_fli, 1 },
 { NULL, NULL, 0 } /* fin de listado */
};

/* Globales, usadas en la interpretacion */
static DATAFILE *el_archivo = NULL; /* el archivo DAT a usar... */

static char linea_actual[MAX_LINEA]; /* linea de comando actual, completa */
static int n_linea_actual; /* numero de linea actual */

static char comando_actual[80+1]; /* comando actual leido */
static char s2nd_param[MAX_LINEA]; /* todo lo que quedo sacando el comando principal */

static char str_param[MAX_PARAMS][256]; /* parametros parseados a char, maximo 256 letras c/u */
static int c_str_param = 0; /* cantidad de parametros str leidos */

static int int_param[MAX_PARAMS]; /* parametros parseados a int */
static int c_int_param = 0; /* cantidad de parametros int leidos */

static int teclado_cancela_fli = 0; /* puedo cancelar un FLI con teclado? */
static int cls_el_fli[3] = { 0, 0, 0 }; /* color de limpieza antes de 'tocar' un FLI */

/* Referente a la salida de texto... */
static int x_echo = 0, y_echo = 0, w_echo = ANCHO_RFB; /* pos cursor */
static int echo_text_color, echo_text_back; /* colores */
static FONT *echo_font; /* font a usar */

// DEBUG: El mapa RGB fue desactivado porque causaba problemas al usar multiples paletas... sorry gordi..
// static RGB_MAP rgb_table_cine; /* mapa RGB para la paleta actual */

static PALETTE pal_cine; /* paleta actual */

/* ------------------------------------------------- */
/* Globales usadas por las funciones de los comandos */

/* DEBUG: agregar aqui las globales extra necesarias, como 'static' */

/* --------------------- Funciones de los comandos --------------- */

/* Esta funcion limpia la pantalla al color especificado */
int cmd_cls() {
 clear_to_color(kfbufferbmp, makecol(int_param[0], int_param[1], int_param[2]));
 enviar_kfbuffer2screen(); /* refrescar pantalla */
 return 0; /* todo OK */
}

/* Dibuja un rectangulo vacio */
int cmd_rect()
{
 rect(kfbufferbmp,
      int_param[0], int_param[1],
      int_param[2], int_param[3],
      makecol(int_param[4], int_param[5], int_param[6]));
 enviar_kfbuffer2screen(); /* refrescar pantalla */
 return 0;
}

/* Dibuja un rectangulo lleno */
int cmd_rectfill()
{
 rectfill(kfbufferbmp,
          int_param[0], int_param[1],
          int_param[2], int_param[3],
          makecol(int_param[4], int_param[5], int_param[6]));
 enviar_kfbuffer2screen(); /* refrescar pantalla */         
 return 0;
}

/* Traza lineas */
int cmd_line()
{
     line(kfbufferbmp,
          int_param[0], int_param[1],
          int_param[2], int_param[3],
          makecol(int_param[4], int_param[5], int_param[6]));
 enviar_kfbuffer2screen(); /* refrescar pantalla */         
 return 0;
}

/* Esta funcion es un wrapper de fade_out de Allegro */
int cmd_fade_out() {
 fade_out(int_param[0]);
 get_palette(pal_cine); // actualizar la paleta actual
 return 0;
}

/* Esta funcion es un wrapper de fade_out de Allegro
 pero ajustado para que haga un fade out al color especificado

fade_out_color [r] [g] [b] [velocidad]
   Realiza una transicion al color [r] [g] [b]
   [velocidad] 1..64 (lento-rapido)
 */
int cmd_fade_out_color() {

 RGB col; // color formato (nota: esto va de 0..63, NO  de 0..255!!)
 PALETTE pal_dest; // paleta destino
 PALETTE pal_orig; // paleta origen
 int i = 0;

 // divido por 4 para que quede en rango 0..63
 // ademas, ajusto con MIN y MAX en rango...
 col.r = MAX(0, MIN(63, int_param[0] / 4));
 col.g = MAX(0, MIN(63, int_param[1] / 4));
 col.b = MAX(0, MIN(63, int_param[2] / 4));

 for (i = 0; i < 256; i++)
 {
  pal_dest[i].r = col.r;
  pal_dest[i].g = col.g;
  pal_dest[i].b = col.b;
 }

 get_palette(pal_orig);

 fade_from(pal_orig, pal_dest, int_param[3]);

 get_palette(pal_cine); // actualizar la paleta actual
 
 return 0;
}

/* Posiciona el cursor de texto */
int cmd_locate()
{
 x_echo = int_param[0];
 y_echo = int_param[1];
 w_echo = int_param[2];
 return 0;
}

/* Color de texto para echo */
int cmd_text_color()
{
 echo_text_color = makecol(int_param[0], int_param[1], int_param[2]);
 return 0;
}

/* Color de fondo para el texto */
int cmd_text_back()
{
 if (int_param[0] < 0 || int_param[1] < 0 || int_param[2] < 0)
     echo_text_back = -1;
 else
     echo_text_back = makecol(int_param[0], int_param[1],int_param[2]);
     
 text_mode(echo_text_back);
 return 0;
}

/* Cambiar la fuente del texto */
int cmd_text_font()
{
 DATAFILE *p = NULL;
 if (c_str_param > 0)
 {
     p = find_datafile_object_type(el_archivo, str_param[0], DAT_FONT);
     if (p == NULL) return -1; /* fallo... */
     echo_font = p->dat;
 }
 else
 {
  echo_font = font;
 }

return 0;
}

/* Salida a pantalla de texto
   escribe y avanza la linea... (como el print de Qbasic) */
int cmd_echo()
{
text_mode(echo_text_back);

y_echo = imprimir_wordwrap(kfbufferbmp,
                           echo_font,
                           x_echo, y_echo, echo_text_color, w_echo,
                           s2nd_param);

enviar_kfbuffer2screen(); /* refrescar pantalla */
return 0;
}

/* Salida de texto centrado en x */
int cmd_echo_centre_x()
{
int x;
text_mode(echo_text_back);
x = text_length(echo_font, s2nd_param);
x = (kfbufferbmp->w - x)/2;

//textprintf(kfbufferbmp, echo_font, x, y_echo, echo_text_color, "%s", s2nd_param);
//y_echo += text_height(echo_font);

// usa word-wrap...
y_echo = imprimir_wordwrap(kfbufferbmp,
                           echo_font,
                           x, y_echo, echo_text_color, w_echo,
                           s2nd_param);


enviar_kfbuffer2screen(); /* refrescar pantalla */
return 0;
}

/* Salida de texto centrado en x,y */
int cmd_echo_centre_xy()
{
int x;
text_mode(echo_text_back);
x = text_length(echo_font, s2nd_param);
x = (kfbufferbmp->w - x)/2;
y_echo = (kfbufferbmp->h - text_height(echo_font)) / 2;

//textprintf(kfbufferbmp, echo_font, x, y_echo, echo_text_color, "%s", s2nd_param);
//y_echo += text_height(echo_font);

// usa word-wrap...
y_echo = imprimir_wordwrap(kfbufferbmp,
                           echo_font,
                           x, y_echo, echo_text_color, w_echo,
                           s2nd_param);


enviar_kfbuffer2screen(); /* refrescar pantalla */
return 0;
}


/* Pausa sincronica, puede ser cancelada con el teclado */
int cmd_rest()
{
long int f = 0; /* para el for */

   if (int_param[0] <= 0 )
         {
          clear_keybuf();
          readkey(); // esperar por una tecla
          clear_keybuf();
         }
       else
        {
	 clear_keybuf();
	 f = 0;	
	 // el cancelar solo funciona si la pausa es > 10 ms
	 if (int_param[0] > 10)
	 {
		 while ((f < int_param[0]) && (!keypressed()))
		 {
	          rest (10);
		  f += 10;
		 }
	 }
	 else
	 {
		 rest((long)int_param[0]);
	 }
	 clear_keybuf();
        }
return 0;
}

/* Setea la paleta y la tabla RGB */
int cmd_set_palette()
{
DATAFILE *p;
p = find_datafile_object_type(el_archivo, str_param[0], DAT_PALETTE);
if (p == NULL) return -1; /* fallo... */

memcpy(pal_cine, p->dat, sizeof(PALETTE));

// create_rgb_table(&rgb_table_cine, pal_cine, NULL);
// rgb_map = &rgb_table_cine; // DEBUG - mapa RGB desactivado

vsync(); /* evita flicker */
set_palette(pal_cine);

return 0;
}

/* Setea la paleta VGA DEFAULT y la tabla RGB */
int cmd_set_palette_default()
{
memcpy(pal_cine, default_palette, sizeof(PALETTE));

vsync(); /* evita flicker */
set_palette(pal_cine);

return 0;
}

/* blitea un bitmap a pantalla */
int cmd_blit()
{
DATAFILE *p;
p = find_datafile_object_type(el_archivo, str_param[2], DAT_BITMAP);
if (p == NULL) return -1; /* fallo... */

blit((BITMAP *)p->dat, kfbufferbmp, 0, 0, int_param[0], int_param[1], ((BITMAP *)p->dat)->w, ((BITMAP *)p->dat)->h);
enviar_kfbuffer2screen(); /* refrescar pantalla */
return 0;
}

/* sprite a pantalla */
int cmd_sprite()
{
DATAFILE *p;
p = find_datafile_object_type(el_archivo, str_param[2], DAT_BITMAP);
if (p == NULL) return -1; /* fallo... */

draw_sprite(kfbufferbmp, (BITMAP *)p->dat, int_param[0], int_param[1]);

enviar_kfbuffer2screen(); /* refrescar pantalla */
return 0;
}

/* estira un bitmap */
int cmd_stretch_blit()
{
DATAFILE *p;
p = find_datafile_object_type(el_archivo, str_param[4], DAT_BITMAP);
if (p == NULL) return -1; /* fallo... */

stretch_blit((BITMAP *)p->dat, kfbufferbmp, 0, 0, ((BITMAP *)p->dat)->w, ((BITMAP *)p->dat)->h, int_param[0], int_param[1], int_param[2], int_param[3]);
enviar_kfbuffer2screen(); /* refrescar pantalla */
return 0;
}

/* ejecuta un sonido (sample) */
int cmd_play_sample()
{
DATAFILE *p;
p = find_datafile_object_type(el_archivo, str_param[0], DAT_SAMPLE);
if (p == NULL) return -1; /* fallo... */

play_sample((SAMPLE *)p->dat, int_param[1], int_param[2], int_param[3], 0);

return 0;
}

/*
ejecuta un archivo FLI (animacion
esta funcion es complicada porque utiliza sonidos y subtitulos... :^P
NOTA: si no logra abrir el fli, NO falla
      esto es para permitir facilmente hacer demostraciones con videos 'recortados'
*/
int cmd_play_fli()
{
int ret = 0; int loop = 0;
DATAFILE *p = NULL; /* buscador */
DATAFILE *fli_p = NULL; /* FLI */
int ctxt = 0; /* encontro el [script_fli]? */
DATAFILE *psample = NULL; /* SAMPLE a ejecutar */
char tmpstr[1024]; /* string temporal de uso vario */
tmpstr[0] = '\0';

// play_fli [x] [y] [w] [h] [loop] [objeto_fli] [script_fli]
//           0   1   2   3    4      5           6
   /* buscar FLI a ejecutar */
   p = find_datafile_object_type(el_archivo, str_param[5], DAT_FLI);
   if (p == NULL) return 0; /* fallo... no hay FLI..., lo ignoro */

   fli_p = p; /* tomar el puto FLI */

   /* buscar la configuracion [script_fli], si no existe, se ignorara */
   p = find_datafile_object(el_archivo, str_param[6]);
   if (p != NULL)
      {
      ctxt = TRUE; /* lo encontro */
      set_config_data((char *)p->dat, p->size);
      }
   else
      ctxt = 0; /* no lo encontro... */

   /* mostramos el fli de la manera complicada...
      ver fli.c de Allegro para mas info */
clear_keybuf(); /* limpio el buffer de teclado */

rgb_map = NULL; /* anulo el mapa RGB */

/* ir realizando el looping adecuado */
for (loop = int_param[4]; loop > 0; loop--)
  {
     if (open_memory_fli(fli_p->dat) != FLI_OK) return -1; /* no era un FLI... */

      ret = next_fli_frame(0);
      if (ret == FLI_ERROR) return -1;

      while (ret == FLI_OK)
      {
          /* actualiza la paleta */
         if (fli_pal_dirty_from <= fli_pal_dirty_to)
                      set_palette_range(fli_palette,
                                        fli_pal_dirty_from,
                                        fli_pal_dirty_to, TRUE);

          /* actualiza la pantalla */
          if (fli_bmp_dirty_from <= fli_bmp_dirty_to)
          {
            /* debo limpiar el buffer? */
            if (cls_el_fli[0] > -1 && cls_el_fli[1] > -1 && cls_el_fli[2] > -1)
                  clear_to_color(kfbufferbmp, makecol(cls_el_fli[0], cls_el_fli[1], cls_el_fli[2]));

            stretch_blit(fli_bitmap, kfbufferbmp,
                         0, 0,
                         fli_bitmap->w, fli_bitmap->h,
                         int_param[0], int_param[1],
                         int_param[2], int_param[3]);

            vsync(); // tratar de evitar el flicker
            enviar_kfbuffer2screen(); /* refrescar pantalla */
          }
          
          reset_fli_variables();

          ret = next_fli_frame(0);
          if (ret == FLI_ERROR) return -1; /* ERROR! */

          if (ctxt)
          {
             /* ejecutar los sonidos adecuados al frame... */
           sprintf(tmpstr,"FRAME_%d", fli_frame);
           psample = find_datafile_object_type(el_archivo,  get_config_string(tmpstr, "snd", "null"), DAT_SAMPLE);
           if (psample != NULL) play_sample((SAMPLE *)p->dat, 255, 128 ,1000, 0);

             /* DEBUG: subtitular en caso necesario... */
          }
          
         do {
          /* esperar hasta proximo cuadro */
          /* si aprieta una tecla, salir... */
          if (keypressed() && teclado_cancela_fli)
              {
              ret = FLI_EOF;
              fli_timer = 1;
              loop = -1;
              }
         } while (fli_timer <= 0);
         
      }
   close_fli(); /* adios... */
 }

/* Devolver mapa RGB */
// rgb_map = &rgb_table_cine; // DEBUG: desactivado
clear_keybuf(); /* limpio el buffer de teclado */
return 0;
}

/* color de limpieza del fli */
int cmd_clear_fli_back()
{
  cls_el_fli[0] = int_param[0] % 256;
  cls_el_fli[1] = int_param[1] % 256;
  cls_el_fli[2] = int_param[2] % 256;
return 0;
}

/* ajusta la bandera de cancelar los fli con el teclado */
int cmd_keyboard_cancel_fli()
{
teclado_cancela_fli = int_param[0];

return 0;
}

/* --------------------- Funciones del motor en si --------------- */

/* Esta macro funcion se encarga de reproducir una cinematica
   desde un archivo DAT, pasarselo en char *file,
   sera cargado, ejecutado, y liberado...
   Esta es la funcion de entrada que deberia usarse para ejecutar
   una cinematica 
   la afecta quiere_videos;
   */
void ejecutar_cinematica(char *file)
{
 DATAFILE *carga = NULL;
 DATAFILE *p;
 char *guion_txt;
 char tmp[80]; // temporal... formacion del nombre del script

 if (!quiere_videos) return; // el usuario no quiere ver videos

 carga = krono_load_datafile(file);

 if  (carga == NULL) return; /* el archivo no existe... chau... */

 /* buscar el script */
 sprintf(tmp, "guion_txt_%s", lenguaje);
 p = find_datafile_object(carga, tmp);
 if (p==NULL)
 {
     // si no existe el especifico del lenguaje, buscar el generico
     p = find_datafile_object(carga, "guion_txt");
     // si no existe el generico, buscar la version en ingles
     if (p==NULL)
     {
         p = find_datafile_object(carga, "guion_txt_en");
         if (p == NULL) return; /* no hay guion... chau... */
     }
 }
 guion_txt = (char *)p->dat;

 ejecutar_script(guion_txt, p->size, carga);

 unload_datafile(carga); /* liberar... */
}


/*
   Ejecuta el script pasado en char *txt_script
   Esto podria ser un DATAFILE ->dat pasado a char, por ejemplo.
   Toma el caracter 10 como salto de linea (compatible con UNIX)
   El texto pasado en *txt_script puede ser del largo que se desee,
   pero debe ser especificado en int size.
   El char *archivo es el archivo DATAFILE que contendra
   los diferentes objetos FLI, BITMAP, etc a usar por los comandos
   Debe estar PRE-cargado en RAM
   NOTA: kfbufferbmp sera usado como doble buffer! DEBE estar iniciado!


   NOTA 2: Por pedido popular, ahora con ESC,SPACE o ENTER se puede cancelar 
           completamente la animacion
*/
void ejecutar_script(const char *txt_script, const int size, DATAFILE *archivo)
{
 int i = 0; /* posicion x en el txt_script */
 int xl = 0; /* pos en tmp[] */
 char tmp[MAX_LINEA]; /* lectura tmp */


/* Iniciar el script */
x_echo = 0, y_echo = 0, w_echo = ANCHO_RFB; /* pos cursor */

echo_font = font; /* font a usar */
echo_text_color = makecol(255,255,255); echo_text_back = -1; /* colores texto */
text_mode(-1);

teclado_cancela_fli = TRUE;
cls_el_fli[0] = cls_el_fli[1] = cls_el_fli[2] = 0;

get_palette(pal_cine); /* tomo la paleta actual */

rgb_map = NULL; /* anulo el mapa RGB */

// debug: desactivado
// create_rgb_table(&rgb_table_cine, pal_cine, NULL);
// rgb_map = &rgb_table_cine;

/* doble buffer, debe estar iniciado! */
set_clip(kfbufferbmp, 0, 0, kfbufferbmp->w-1, kfbufferbmp->h-1); /* clipping adecuado */
clear(kfbufferbmp);

n_linea_actual = 0;
el_archivo = archivo;
tmp[0] = '\0';
linea_actual[0] = '\0';

/* Revisar linea por linea del buffer */
 for (i = 0; i < size; i++)
 {
  /* Ir armando la linea */
  if (txt_script[i] == '\r' || xl == MAX_LINEA-1 || txt_script[i] == 10) /* salto de linea */
  {
   tmp[xl] = '\0';
   krtrim(linea_actual, tmp); /* remover espacios */
   n_linea_actual ++;
   xl = 0;

   if (parsear_y_ejecutar_linea()) return; /* interpretar comando, y cancelar si el usuario cancela */
   
   tmp[0] = '\0';
   linea_actual[0] = '\0';
  }
  else
  {      /* armando la linea */
   tmp[xl] = (txt_script[i] >= 32) ? txt_script[i] : ' '; /* filtra chars 'raros' */
   xl++;
  }
 }

 /* Ultima linea, interpretar tambien... */
   tmp[xl] = '\0';
   krtrim(linea_actual, tmp); /* remover espacios */
   n_linea_actual ++;
   xl = 0;

   parsear_y_ejecutar_linea(); /* interpretar comando! */

 /* Termino el show... */
}


/*
Esta funcion interpreta una linea y la parsea en sus parametros
respectivos, para luego llamar a la funcion adecuada
Si el usuario presiona una tecla, devuelve TRUE (-1) (Cancelar)
Caso contrario, devuelve 0
*/
int parsear_y_ejecutar_linea()
{
int i, i2;
char *tok;
char tmptok[MAX_LINEA];

/* es un comentario? no hacer nada... */
if (linea_actual[0] == '#' || strlen(linea_actual) < 1 ) return 0;

/* Limpio los parametros... */
for (i = 0; i < MAX_PARAMS; i++)
    {
        int_param[i] = 0;
        str_param[i][0] = '\0';
    }

comando_actual[0] = '\0';

i = c_str_param = c_int_param = 0;

/* Parsear la linea... */
sprintf(tmptok, "%s", linea_actual); /* evito modificar el original */
for (tok = strtok(tmptok, " ");tok;tok = strtok(0, " "))
     {
      if (i == 0)
         {
          krtrim(comando_actual, tok); /* el 1er comando */
         }
         else
         { /* tomar parametros */
          krtrim(str_param[i-1], tok); /* remover espacios */
          c_str_param ++;
          int_param[i-1] = atoi(tok);
          c_int_param++;
         }
      i++;
      if (i > MAX_PARAMS-1) break; /* muchos parametros */
     }

/* Tomar todo menos el primer parametro... */
tmptok[0] = s2nd_param[0] = '\0';

i2 = 0;
i = strlen(comando_actual);
while (i < strlen(linea_actual)) /* con un for no funcionaba... por que? */
    {
      tmptok[i2] = linea_actual[i];
      i2++;
      i++;
    }

tmptok[i2] = '\0';

krtrim(s2nd_param, tmptok);

/* ----------- interpretacion ----------- */

/* recorro la lista en busca del comando a usar */
i = 0;
    while (cmd_list[i].comando != NULL && cmd_list[i].proc != NULL)
    {
        /* NOTA: cambiar strcmp x stricmp para ignorar mayus/minus, pero
           no es ANSI ni POSIX */
        if (strcmp(comando_actual, cmd_list[i].comando) == 0)
            {
             /* Tiene suficientes parametros? */
             if (c_str_param < cmd_list[i].min_params && c_int_param < cmd_list[i].min_params)
                {
                 sprintf(tmptok,"ERROR:\ncinema.c:\nscript:linea(%d)='%s'\nInsuficientes parametros (requiere %d)!\n", n_linea_actual,linea_actual,cmd_list[i].min_params);
                 levantar_error(tmptok);
                }

             /* llamar a la funcion, devuelven -1 en error */
             if ( (*cmd_list[i].proc)() )
                {
                 sprintf(tmptok,"ERROR:\ncinema.c:\nscript:linea(%d)='%s'\nEl comando '%s' fallo\nPosibles parametros incorrectos o falta de objetos...\n", n_linea_actual,linea_actual,cmd_list[i].comando);
                 levantar_error(tmptok);
                }
             
				// ver si el usuario cancelo
				if (keypressed() && (key[KEY_ESC] || key[KEY_ENTER] || key[KEY_SPACE]) ) 
					return TRUE;
				else
				    return 0; /* termino, proxima linea please... */
            }
        i++;
    }

/* Comando no reconocido! SHIT! */
sprintf(tmptok, "ERROR:\ncinema.c:\nscript:linea(%d)='%s'\nComando no reconocido!\n", n_linea_actual, linea_actual);
levantar_error(tmptok);
	
return 0;
}
#endif
