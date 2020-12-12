// --------------------------------------------------------
// ia.h
// -------------------------------------------------------- 
// Copyright (c) Kronoman 
// En memoria de mi querido padre 
// -------------------------------------------------------- 
// Sistema de inteligencia artificial utizando "ejecutables"
// binarios contenidos en un DAT
// --------------------------------------------------------

#ifndef KRAPTOR_IA_H
#define KRAPTOR_IA_H


#include "allegro.h"


// Estructura contenedora de una instruccion 
typedef struct IA_BYTECODE
{
 int x1, y1, x2, y2; // coordenadas de movimiento
 int weapon; // arma a disparar
 int loop; // cantidad de veces a repetir

} IA_BYTECODE;


// Estructura de la lista enlazada contenedora
// de todos los "programas" de IA precargados
typedef struct IA_NODE
{
 IA_BYTECODE *code; // puntero a matriz asignada con malloc de todas las instrucciones de esta IA particular
 int size; // cantidad de bytecodes contenidos en bytecode[]
 char id[1024]; // identificacion usada para identificar esta secuencia en la IA (cadena ASCCIZ)

 struct IA_NODE *next; // puntero al siguiente
} IA_NODE;



// Prototipos
void hacer_chache_ia(const char *file);
void liberar_lista_ia();
IA_NODE *buscar_lista_ia(const char *id);

#endif
