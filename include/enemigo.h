// -------------------------------------------------------- 
// enemigo.h
// -------------------------------------------------------- 
// Copyright (c) Kronoman 
// En memoria de mi querido padre 
// -------------------------------------------------------- 
// Todo lo relacionado con los enemigos
// --------------------------------------------------------
#ifndef ENEMIGO_H
#define ENEMIGO_H

#include "allegro.h"
#include "ia.h"
#include "pmask.h"

/* friccion X,Y  */
#define FRC_X_E  0.1
#define FRC_Y_E  0.1

/* velocidad maxima X, Y */
#define MAX_VEL_E_X 15
#define MAX_VEL_E_Y 15

/* Cantidad maxima de tipos de enemigos/armas, con 100 es mas que suficiente */
#define MAX_E_CLASS 100

/* Esta bandera indica cuando el enemigo esta muerto
   DEBE ser un valor NEGATIVO, ej: -150
   Es para que agonize en llamas... :^P
   Para los monstruos (boss) se multiplica x 2 <- DEBUG: NO IMPLEMENTADO! */
#define ENE_MUERTO -200             

/* Tipo de datos que contiene la definicion de armas de los enemigos */
typedef struct ARMA_ENE {
   BITMAP *spr;  /* sprite que representa el disparo */
   struct PMASK *mask;  /* mascara de choque */
   SAMPLE *snd[2]; /* 0 = sonido al dispararlo, 1 = sonido al impactar */

   fixed vx, vy;  /* velocidad x, y */
   int vida;      /* duracion del disparo, si es < 1, el tipo de arma no existe */
   int punch;     /* `golpe` del arma */

   int t; /* tipo de disparo:
             0 = recto [sale del centro]
             1 = direccion al azar
             2 = abanico triple
             3 = doble recto 
             4 = 1 direccionado hacia el jugador (sale hacia el jugador)
             5 = 5 tiros direccionados (apunta al jugador, solo en X)
             6 = cuadruple al azar
             7 = 20 a 30 al azar (ideal para boss)
             8 = 20 direccionados (ideal para boss)
             9 = 20 a 30 en todas direcciones (ideal para boss)
             10 = 5 a 30 en todas direcciones (ideal para boss)
             11 = 4 a 8 en todas direcciones (para un enemigo)
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
   int est_dx[2]; /* aceleracion x inicial (100 = 0.01) */
   int est_dy[2]; /* aceleracion y inicial */
   int est_tipo[2]; /* tipo(s) de particula */
   int est_rad[2]; /* radio */
   int est_transp; /* transparencia */
} ARMA_ENE;

/* Tipo de datos que contiene los tipos de enemigos */
typedef struct ENEM_T {
    int vida; /* vida inicial, si es < 1, la clase de enemigo no existe */

    int peso; /* esto indica el 'peso' del enemigo, o sea, cuanta energia
                 le quita al jugador en caso de colision */
    
    BITMAP *spr[4];         // sprites 0..3 = frames de animacion
    BITMAP *spr_shadow[4]; // sombra (autogenerada al cargar data)
    int spr_delay;          // delay de animacion en tics (30 = 1 segundo)

    PMASK *mask[4];  /* mascara de colision de cada sprite*/

    int dinero; /* dinero que vale matar a este enemigo */
    
    int premio_idx, premio_prob; /* indice de premio y probabilidad de darlo (%) */
     
    // Referente a la IA del enemigo - DEBUG, nuevo!
    int ia_azar; // la IA del enemigo debe comenzar al azar? (es decir, comienza por bytecode 0 o por cualquiera?)
    IA_NODE *ia_node; // nodo con bytecodes de la IA a ejecutar para esta clase de enemigo
    int ia_boss; // es un BOSS? -1 = si
} ENEM_T;

/* Nodos que contienen los enemigos, en lista enlazada */
typedef struct ENEMIGO {
    fixed x,  y;     /* posicion */

    int vida;      /* vida que le queda */

    int ene_t; /* tipo de enemigo (se fija en la matriz ENEM_T) */

    int spr_delay; // contador para la animacion
    int spr_actual; // sprite actual de animacion
    
    int arma_actual; // lo uso como referencia para disparar
    
    // Referente a la IA
    int bytecode_actual; // bytecode en ejecucion (indice [])
    int bytecode_loop;   // contador para realizar los bytecodes con loop (parametro "loop")
    IA_NODE *ia_node; // nodo de IA a ejecutar, se iguala a la clase de enemigo, es para evitar sintaxis complicadas... :)
    IA_BYTECODE bytecode_exe; // copia del bytecode ejecutandose (para los loops)

    /* puntero al siguiente */
    struct ENEMIGO *next;
} ENEMIGO;

/* Nodo de disparo del enemigo, en lista enlazada */
typedef struct DISP_ENE  {
    fixed x,  y;     /* posicion  */
    fixed dx, dy;    /* direccion */
    int vida;        /* vida que le queda */

    int arma; /* indice de la matriz ARMA_ENE que indica el tipo de disparo, etc */

    /* puntero al siguiente */
    struct DISP_ENE *next;
} DISP_ENE;


/* GLOBALES EXPORTADAS */
extern ARMA_ENE arma_ene[MAX_E_CLASS]; 
extern ENEM_T enem_t[MAX_E_CLASS]; 
extern ENEMIGO *enemigo_1; 
extern DISP_ENE *disp_ene_1; 
extern int cant_enemigos_debug;

/* Prototipos de funciones */
void agregar_enemigo(int x, int y, int tipo );
void IA_enemigo(ENEMIGO *ene);
void mover_enemigos(int fy);
void liberar_lista_enemigos();
void dibujar_enemigos(BITMAP *bmp, int x, int y);

void agregar_disparo_ene(ENEMIGO *ene);
void mover_disparos_ene(int fy);
void dibujar_disp_ene(BITMAP *bmp, int x, int y);
void liberar_lista_disparos_ene();
#endif
