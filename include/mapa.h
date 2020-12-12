// -------------------------------------------------------- 
// mapa.h 
// -------------------------------------------------------- 
// Copyright (c) Kronoman 
// En memoria de mi querido padre 
// -------------------------------------------------------- 
// Esto contiene todo lo referente al mapa en si
// -------------------------------------------------------- 
#ifndef MAPA_H
#define MAPA_H

/* Tama¤o del fondo en pixeles */
#define W_FONDO  600
#define H_FONDO  4000

/* Grilla WxH en pixels */
#define W_GR 40
#define H_GR 40

/* Cantidad de cuadritos de la grilla [ancho y alto] */
#define GRILLA_W  (W_FONDO / W_GR)
#define GRILLA_H  (H_FONDO / H_GR)

/* Globales exportadas */
extern long mapa_g[GRILLA_W][GRILLA_H];
extern long enem_g[GRILLA_W][GRILLA_H];
extern int scroll_mapa;
extern int scroll_mapa_speed;
extern BITMAP *mapa_fondo;
extern COLOR_MAP tabla_quemado;
extern BITMAP *burn_cache_bmp[3];

/* Prototipos */
void scan_nivel_1era_vez();
void scan_nuevos_enemigos(int y);
void quemar_area(BITMAP *bmp, int xp, int yp, int rad1, int rad2);
int explotar_fondo(int x, int y, int punch);
void crear_mapa_quemazon(AL_CONST PALETTE pal, int x, int y, RGB *rgb);
#endif
