/*--------------------------------------------------------
 wordwrap.h
 --------------------------------------------------------
 Copyright (c) Kronoman
 En memoria de mi querido padre
 --------------------------------------------------------
 Funciones para escribir texto con 'word-wrap'
 --------------------------------------------------------*/
#ifndef KRONO_WORDWRAP_H
#define KRONO_WORDWRAP_H

#include "allegro.h"

/* Prototipos */
int  imprimir_wordwrap(BITMAP *bmp,
                       const FONT *f,
                       int x, int y, int color, int w,
                       const char *text);

#endif
