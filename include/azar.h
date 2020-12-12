// azar.h
// Macro para numeros al azar
// Elige un numero entre min y max
// Tomado de la internet

#ifndef AZAR_H
#define AZAR_H

#include <stdlib.h>

/*
Esta funcion explota si min - max da 0, sorry, pero la cambie...
*/
// #define rand_ex( min, max )   ( (rand() % (max - min)) + min )


/* Prototipo */
int rand_ex(int min, int max);

#endif
