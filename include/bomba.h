// -------------------------------------------------------- 
// bomba.h
// -------------------------------------------------------- 
// Copyright (c) Kronoman 
// En memoria de mi querido padre 
// -------------------------------------------------------- 
// Este modulo contiene todo lo relacionado con las bombas
// del jugador...
// -------------------------------------------------------- 
#ifndef BOMBA_H
#define BOMBA_H

extern int bomba_esta_activa;
extern int bomba_detonacion;
extern SAMPLE *bomba_sonido;

// Prototipos
void mover_bomba();
void dibujar_bomba();
void detonar_totalmente_el_piso(int y);

#endif
