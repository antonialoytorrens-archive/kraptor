// ------------------------------------
// savedlg.c
// ------------------------------------
// Sistema de salva / carga de juego
// Por Kronoman
// En memoria de mi querido padre
// Copyright (c) 2002, Kronoman
// ------------------------------------

#ifndef SAVEDLG_H
#define SAVEDLG_H

#include <allegro.h>
#include "jugador.h"
#include "global.h"

// cantidad de slots de savegame (el dialogo tiene soporte para 7)
#define SAVE_GAME_SLOTS 7

// Estructura de salvado del juego a disco
// No puedo salvar directamente el JUGADOR, porque tiene punteros y boludeces que no sirven
typedef struct SAVE_GAME_ST
{
     char desc[41]; // descripcion del savegame
     int es_valido; // el savegame es valido? 0 = NO, != 0 = si
     int vida;      /* vida que le queda, energia  */
     int dinero;    /* dinero (puntos) que tiene */
     int arma[MAX_ARM_CLASS]; /* armamentos que tiene (<= 0 no tiene el arma, > 0 posee el arma, cantidad de balas que tiene) */
     int arma_actual; /* arma seleccionada */
     int bombas; /* cantidad de bombas que le quedan */
     int nivel_actual; // nivel que va jugando
     int nivel_de_dificultad; // nivel de dificultad
} SAVE_GAME_ST;

// Prototipos
void salvar_juego_menu();
int cargar_juego_menu();
#endif
