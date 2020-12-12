// -------------------------------------------------------- 
// data.h 
// -------------------------------------------------------- 
// Copyright (c) Kronoman 
// En memoria de mi querido padre 
// -------------------------------------------------------- 
// Esto se encarga de cargar y poner en memoria
// los datos leidos de disco.
// -------------------------------------------------------- 
#ifndef DATA_H
#define DATA_H


/* Globales exportadas */
extern DATAFILE *krapmain;
extern DATAFILE *datmapa;
extern DATAFILE *datmenu;

/* Prototipos */
void cargar_datos_principales();
int cargar_nivel(int nivel, int solo_verificar);

#endif
