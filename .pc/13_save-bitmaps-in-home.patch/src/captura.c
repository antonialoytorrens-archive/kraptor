/*
 --------------------------------------------------------
 captura.c 
 -------------------------------------------------------- 
 Copyright (c) Kronoman 
 En memoria de mi querido padre 
 -------------------------------------------------------- 
 Funcion para tomar una captura de pantalla
 Automaticamente la salva en archivoxxx.bmp, etc
 -------------------------------------------------------- 
*/
#ifndef CAPTURAR_C
#define CAPTURAR_C

#include <stdio.h>
#include "allegro.h"

/* Esta funcion captura la pantalla, usando el prefijo para el archivo
   Salva un PCX de 8 bits de color.
*/
void capturar_la_pantalla(char *prefijo) 
{
      char arch[1024];
      int i = 0;

      BITMAP *bmp;
      PALETTE pal;

      /* Buscar un archivo entre 0 y 4095 para salvar el archivo
         Usa numeracion hexadecimal para que entren mas archivos */
      do {
       sprintf(arch, "%s%x.bmp", prefijo, i);
       i++;
      } while (exists(arch) && i < 4096);


      get_palette(pal);
      bmp = create_sub_bitmap(screen, 0, 0, SCREEN_W, SCREEN_H);
      save_bitmap(arch, bmp, pal);
      destroy_bitmap(bmp);
}

#endif
