// -------------------------------------------------------- 
// error.c 
// -------------------------------------------------------- 
// Copyright (c) Kronoman 
// En memoria de mi querido padre 
// -------------------------------------------------------- 
// Mensajes de error en modo texto.
// --------------------------------------------------------
#ifndef ERROR_C
#define ERROR_C

#include <stdlib.h>
#include <allegro.h>

// --------------------------------------------------------
// Vuelve a modo texto, muestra el mensaje, y finaliza.
// --------------------------------------------------------
void levantar_error(char *msg)
{
 set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);

 allegro_message("%s\n", msg);

 exit(1); // salir con error
}

#endif
