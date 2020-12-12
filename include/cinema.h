/*
 --------------------------------------------------------
 cinema.h
 --------------------------------------------------------
 Copyright (c) Septiembre 2002, Kronoman
 En memoria de mi querido padre
 --------------------------------------------------------
 Engine de cinematicas mediante scripts y datafiles
 -------------------------------------------------------- */

#ifndef CINEMA_H
#define CINEMA_H


/* Estructura que contiene la lista de funciones que puede
   utilizar el script,
   contiene  el comando, y un puntero a la funcion...
   el ultimo item debe contener NULL en todos los parametros */
typedef struct CMD_SCRIPT_TYPE
{
  char *comando;    /* comando que ejecuta el procedimiento */
  int (*proc)();    /* procedimiento que es llamado, devuelve -1 si pasa un error, -666 si se debe cancelar la cinematica */
  int min_params;   /* cantidad minima de parametros que precisa */
} CMD_SCRIPT_TYPE;

/* Cantidad maxima de parametros que se pueden leer de un solo 'saque' */
#define MAX_PARAMS  100
/* Largo maximo de linea a interpretar */
#define MAX_LINEA 2048

/* Prototipos */
/* Funciones propias */
void ejecutar_cinematica(char *file);
void ejecutar_script(const char *txt_script, const int size, DATAFILE *archivo);
int parsear_y_ejecutar_linea();

/* Prototipos de funciones de interpretacion */
int cmd_cls();
int cmd_rect();
int cmd_rectfill();
int cmd_line();
int cmd_fade_out();
int cmd_fade_out_color();
int cmd_locate();
int cmd_text_color();
int cmd_text_back();
int cmd_text_font();
int cmd_echo();
int cmd_echo_centre_x();
int cmd_echo_centre_xy();
int cmd_rest();
int cmd_set_palette();
int cmd_set_palette_default();
int cmd_blit();
int cmd_sprite();
int cmd_stretch_blit();
int cmd_play_sample();
int cmd_play_fli();
int cmd_clear_fli_back();
int cmd_keyboard_cancel_fli();
#endif
