/*--------------------------------------------------------
 wordwrap.c
 --------------------------------------------------------
 Copyright (c) Kronoman
 En memoria de mi querido padre
 --------------------------------------------------------
 Funciones para escribir texto con 'word-wrap'
 --------------------------------------------------------*/
#ifndef KRONO_WORDWRAP_C
#define KRONO_WORDWRAP_C


#include <string.h>
#include "allegro.h"

#include "wordwrap.h"

/*
Esta funcion imprime texto con word-wrap a 'w' caracteres...
NOTA: no puede usar palabras mayores a 1 Kb (1024 bytes!)
NOTA: no funciona muy bien con palabras muy largas (es decir, si no entran en el wrap, se salen del borde...)
NOTA: NO uso strtok porque se 'come' el separador de palabras!
NOTA: \n puede ser usado para saltar la linea
Parametros:
bmp = bitmap donde escribir
f = font a usar
x, y = coordenadas en pixeles
color = color a usar
w = ancho del borde, donde el texto 'cae' (wrap-edge)
text = texto a escribir

Devuelve:
0 si falla
si funciona, devuelve la coordenada 'y' inferior hasta donde imprimio
*/
int  imprimir_wordwrap(BITMAP *bmp,
                       const FONT *f,
                       int x, int y, int color, int w,
                       const char *text)
{
char tok[1024]; /* token actual */
char st[2]; /* caracter de busqueda (lo trato como string) */
char *limites = " ,;.<>()[]{}/*-+=\\|"; /* limitadores de token */
int i1, i2; /* auxiliares */
int xp = 0; /* posicion actual en pantalla */

xp = x;
i1 = 0;

if (w < text_length(f, "X")) return 0; /* evita bucle infinito, creo... :^P */

while (i1 < strlen(text))
  {
      /* buscar un token, e imprimirlo */
      i2 = i1;
      tok[0] = st[0] = st[1] = '\0'; /* blanquear strings */

      do {
          tok[i2-i1] = text[i2];

          st[0] = text[i2]; /* caracter de comprobacion */

          if (text[i2] == '\n') /* salto de linea */
             {
               xp = x;
               y += text_height(f); // + 1;
               break; 
             }

          i2++;
      } while (i2 < strlen(text) && !strstr(limites, st) && i2 - i1 < 1023 && x+text_length(f, tok) < x + w );

      tok[i2-i1] = '\0';

      /* avanzar: el +1 evita bucles infinitos, creo... */
      i1 += (strlen(tok) > 0) ? strlen(tok) : 1; 

      /* Si la palabra llega al borde, salto */
      if (xp+text_length(f, tok) > x + w)
          {
           xp = x;
           y += text_height(f); // + 1;
          }

      textprintf(bmp, f, xp, y, color, "%s", tok);

      xp += text_length(f, tok);
  }

return y += text_height(f); // + 1;
}
#endif
