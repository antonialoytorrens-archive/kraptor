// -------------------------------------------------------- 
// jugador.h
// -------------------------------------------------------- 
// Copyright (c) Kronoman 
// En memoria de mi querido padre 
// -------------------------------------------------------- 
// Este modulo contiene todo lo relacionado con el jugador
// Tal como movimiento, disparos, etc...
// Exporta ademas unas globales utiles para verificar
// colisiones con disparos enemigos y enemigos en si.
// Tiene la estructura con las teclas de control del jugador
// -------------------------------------------------------- 
#ifndef JUGADOR_H
#define JUGADOR_H

#include <allegro.h>
#include "pmask.h"

/* Aceleracion X, Y del jugador */
#define VEL_X_J  2.5
#define VEL_Y_J  2.5

/* friccion X,Y jugador */
#define FRC_X_J  0.02
#define FRC_Y_J  0.02

/* velocidad maxima X, Y del jugador */
#define MAX_VEL_J_X 10.0
#define MAX_VEL_J_Y 10.0

/* Cantidad maxima de tipos de armas */
#define MAX_ARM_CLASS 15

/* Cantidad maxima de energia del jugador */
#define MAX_ENERGIA 200

/* Esta bandera indica cuando el jugador esta muerto
   DEBE ser un valor NEGATIVO, ej: -300
   Es para que agonize en llamas... :^P */
#define JUG_MUERTO -300

/* Tipo de datos que contiene la definicion de armas del jugador */
typedef struct ARMA_JUG {
   /* Referente al modo 'shopping' donde se compra el arma */

   BITMAP *arma;  /* dibujo del arma en el shop */
   char desc[2049]; /* descripcion del arma larga (texto ascii) */
   char desc_short[21]; /* descripcion corta para el HUD */
   int precio; /* precio del arma, si es <=0, indica que este tipo de arma NO existe */
   int cant_ammo; /* cantidad de balas x compra */
   int cant_ammo_max; /* cantidad maxima de balas */

   /* Referente al disparo en si */

   BITMAP *spr;  /* sprite que representa el disparo */
   PMASK *mask;  /* mascara de choque */
   SAMPLE *snd[2]; /* 0 = sonido al dispararlo, 1 = sonido al impactar */

   fixed vx, vy; /* velocidad x, y */
   int vida;   /* duracion del disparo */
   int punch;  /* `golpe` del arma */
   int firerate; /* ratio de disparo, referido a lastshoot */

   int t; /* tipo de disparo: esto se cambia reprogramando la seccion correspondiente en jugador.c
             0 = recto [sale del centro]
             1 = direccion al azar
             2 = abanico triple, vx especifica la apertura del abanico (default = 5)
             3 = doble recto [salen del centro (+/-) 50% del ancho nave]
             4 = 1 del centro, 1 al azar por disparo (very cool!)
             5 = 1 del centro, 1 de la izq, 1 de la derecha (tipo misile pod, incluye una variacion del 10% al azar en la velocidad en y, para efecto misile pod)
             6 = flak, 6 al azar (ca~on de flak!), y 2 rectos de las alas
          */

   /* Esto es referente a la estela de particulas que
      el arma puede dejar, o no.
      Cada parametro tiene 2 indices, el valor minimo y maximo que puede tomar
      respectivamente (para variacion al azar)
      Cabe aclarar que la dx, dy es un numero decimal multiplicado por 100 (100 = 0.01)
      Si vida[0] <= 0, no deja ninguna estela */
   int est_vida[2]; /* vida de la particula */
   int est_cant[2]; /* cantidad de particulas x frame */
   int est_color[3][2]; /* color, el primer indice indica RGB, y el 2ndo min-max */
   int est_dx[2]; /* aceleracion x inicial */
   int est_dy[2]; /* aceleracion y inicial */
   int est_tipo[2]; /* tipo(s) de particula */
   int est_rad[2]; /* radio */
   int est_transp; /* transparencia */
} ARMA_JUG;

/* Tipo de datos que contiene el jugador */
typedef struct JUGADOR {
    fixed x,  y;     /* posicion */
    fixed dx, dy;    /* direccion */

    int vida;      /* vida que le queda, energia  */

    int dinero;    /* dinero (puntos) que tiene */

    BITMAP *spr[6];  /* sprites que lo representan, 0 = izq, 1 = normal, 2 = der,
                        3,4,5 son las sombras de los sprites
                        (se generan al cargar los datos) */

    PMASK *mask;  /* mascara de colision, para spr[1] */

    int arma[MAX_ARM_CLASS]; /* armamentos que tiene (<= 0 no tiene el arma, > 0 posee el arma, cantidad de balas que tiene) */

    int arma_actual; /* arma seleccionada */

    int lastshoot; /* contador  para temporizar intervalo entre disparos */

    int bombas; /* cantidad de bombas que le quedan */

    /* Reparacion de la nave, para el shopping */
    int reparar_cantidad; /* cantidad que repara cada unidad */
    int reparar_precio;   /* precio por unidad */
    char reparar_desc[2048]; /* descripcion */
    BITMAP *reparar_bmp; /* bitmap de reparacion */

    /* Bombas de la nave, para el shopping */
    int bomba_cantidad;
    int bomba_precio;
    char bomba_desc[2048];
    BITMAP *bomba_bmp;
    int max_bombas;

    /* dinero y bombas iniciales */
    int init_money;
    int init_bombas;

} JUGADOR;

/* Nodo de disparo del jugador, en lista enlazada */
typedef struct DISP_JUG  {
    fixed x,  y;     /* posicion  */
    fixed dx, dy;    /* direccion */
    int vida;        /* vida que le queda */

    int arma; /* indice de la matriz ARMA_JUG que indica el tipo de disparo, etc */

    /* puntero al siguiente */
    struct DISP_JUG *next;
} DISP_JUG;

/* Teclas de juego, corresponden al tipo key[KEY_SPACE], etc */
typedef struct TECLADO_JUG {
  /* arriba, abajo, derecha, izquierda, disparar, cambiar arma, bomba especial */
  int arr, abj, der, izq, sht, wpn, bmb;
} TECLADO_JUG;


/* GLOBALES EXPORTADAS */
extern ARMA_JUG arma_tipo[MAX_ARM_CLASS];  /* tipos de armas */
extern JUGADOR jugador;         /* jugador */
extern DISP_JUG *disp_jug_1;    /* puntero al 1er disparo del jugador */
extern TECLADO_JUG teclado_jug; /* teclado del jugador */

/* Prototipos de funciones */
void dibujar_tablero(BITMAP *bmp);
void set_teclado_default();
void reset_total_jugador();
void reset_parcial_jugador();
void jugador_proxima_arma();
void interpreta_teclado();
void dibujar_nave_jugador(BITMAP *bmp, int x, int y);
void mover_jugador(int fy);

void agregar_disparo_jug();
void mover_disparos_jug(int fy);
void dibujar_disp_jug(BITMAP *bmp, int x, int y);
void liberar_lista_disparos_jug();

#endif
