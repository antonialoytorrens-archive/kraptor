// azar.c
// Macro para numeros al azar
// Elige un numero entre min y max
// Tomado de la internet y mejorado por Kronoman

#ifndef AZAR_C
#define AZAR_C

#include <stdlib.h>

#include "azar.h"
/*
Esta funcion explota si min - max da 0, sorry, pero la cambie...
*/
// #define rand_ex( min, max )   ( (rand() % (max - min)) + min )

/* esta funcion es ligeramente mas lenta, pero NO explota... */
int rand_ex(int min, int max)
{
int tmp;

     if (min - max == 0) return 0 + min; /* evito division por cero */

     /* correcion nueva, medio lenta, pero para corregir algunos bugs raros
        intercambio min y max si estan al reves */
     if (min > max)
     {
      tmp = min;
      min = max;
      max = tmp;
     }

     return ( rand() % (max - min) ) + min;
}

#endif
