/*
 --------------------------------------------------------
 captura.h 
 -------------------------------------------------------- 
 Copyright (c) Kronoman 
 En memoria de mi querido padre 
 -------------------------------------------------------- 
 Funcion para tomar una captura de pantalla
 Automaticamente la salva en archivoxxx.pcx, etc
 -------------------------------------------------------- 
*/
#ifndef CAPTURAR_H
#define CAPTURAR_H

#include <stdio.h>
#include "allegro.h"

/* Esta funcion captura la pantalla, usando el prefijo para el archivo
   Salva un PCX de 8 bits de color.
*/
void capturar_la_pantalla(char *prefijo);

#endif


