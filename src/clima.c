// ------------------------------------
// clima.c
// ------------------------------------
// Modulo de efectos climaticos
// Por Kronoman
// En memoria de mi querido padre
// Copyright (c) 2002, Kronoman
// ------------------------------------


#include <allegro.h>
#include "clima.h"
#include "azar.h"
#include "global.h"

static CLIMA_P clima_p[MAX_CLIMA_P]; // contenedor de particulas
static int clima_cant = 0; // cantidad de particulas 0..MAX_CLIMA_P

// auxiliar que reinicia una particula individual
// parametros:
// idem a init_clima,
// i es el indice de la particula
static void reinit_clima_part(BITMAP *bmp, int c, int t, int d, int i )
{
  int c2;
  int minx = 50, miny = 50 , maxx = 100 , maxy = 150; // velocidades minimas y maximas 

  // info extra       
  clima_p[i].t = t;
  clima_p[i].d = d;

  // iniciar coordenadas de las particulas y direccion
      clima_p[i].y = itofix( rand_ex(0, bmp->h) );
      clima_p[i].x = itofix( rand_ex(0, bmp->w) );

  // color y radio
  switch (t)
  {
  case 0: // lluvia
    c2 = rand_ex(200, 255);
    clima_p[i].col = makecol(0,128,c2); // tonos de azul
    clima_p[i].r = 1;
	minx = 80;
	maxx = 120;
    miny = 60;
	maxy = 180;
  break;

  case 1: // nieve
    c2 = rand_ex(192, 255);
    clima_p[i].col = makecol(c2,c2,c2); // tono de gris
    clima_p[i].r = rand_ex(1,5); // radio
	minx = 40;
	maxx = 110;
    miny = 50;
	maxy = 150;
  break;

  default: // error, el tipo no existe, no poner clima
     clima_cant = 0;
     return;
  break;
  }

// velocidad de particulas
  clima_p[i].dx = ftofix( (float)rand_ex(minx, maxx) / 10.0);
  clima_p[i].dy = ftofix( (float)rand_ex(miny, maxy) / 10.0);

// si hay que ir a la izquierda...
if (d == 0) clima_p[i].dx = fixmul(clima_p[i].dx , itofix(-1)); // a la izquierda

}

// Inicializa el clima
// Parametros:
// bmp = bitmap de buffer
// c = cantidad de particulas
// t = tipo: 0 = lluvia, 1 = nieve, etc
// d = direccion de caida: 0 = izquierda, 1 = derecha
//
void init_clima(BITMAP *bmp, int c, int t, int d)
{
int i;
if (c < 1 || c > MAX_CLIMA_P)
   {
     clima_cant = 0;
     return;
   }
// iniciar particulas

for (i=0; i < c; i ++)
{
  reinit_clima_part(bmp, c, t, d, i );
} // fin for

 clima_cant = c;
}


// mueve el clima
// parametros:
// bmp = bitmap de uso
void mover_clima(BITMAP *bmp)
{
int i;
 for (i=0; i < clima_cant; i ++)
 {
  // mover clima
  clima_p[i].x = fixadd(clima_p[i].x, clima_p[i].dx);
  clima_p[i].y = fixadd(clima_p[i].y, clima_p[i].dy);
  // reiniciar particula si se sale
  if (clima_p[i].x < 0 || fixtoi(clima_p[i].x) > bmp->w ||
      clima_p[i].y < 0 || fixtoi(clima_p[i].y) > bmp->h)
      {
        reinit_clima_part(bmp, clima_cant, clima_p[i].t, clima_p[i].d, i );
      }
 } // fin for
}


// dibuja el clima
// parametros:
// bmp = bitmap de uso
void dibuja_clima(BITMAP *bmp)
{
int i;
 for (i=0; i < clima_cant; i ++)
 {
  switch (clima_p[i].t)
  {
  case 0: // lluvia
  
  if (nivel_detalle > 9) 
  {
        drawing_mode(DRAW_MODE_TRANS, NULL, 0,0); // lluvia transparente, cool!
	triangle(bmp,
         fixtoi(clima_p[i].x),
         fixtoi(clima_p[i].y),
         fixtoi(fixadd(clima_p[i].x, clima_p[i].dx)),
         fixtoi(fixadd(clima_p[i].y, clima_p[i].dy)),
         fixtoi(clima_p[i].x),
         fixtoi(clima_p[i].y)+(rand()%4)+1,
         clima_p[i].col);
  solid_mode();
  }
  else
  {
	    line(bmp,
             fixtoi(clima_p[i].x),
             fixtoi(clima_p[i].y),
             fixtoi(fixadd(clima_p[i].x, clima_p[i].dx)),
             fixtoi(fixadd(clima_p[i].y, clima_p[i].dy)),
             clima_p[i].col);
  }
  break;

  case 1: // nieve
    circlefill(bmp,
           fixtoi(clima_p[i].x),
           fixtoi(clima_p[i].y),
           clima_p[i].r,
           clima_p[i].col);
  break;

  default: // error, el tipo no existe, no dibujar
  break;
  }
 } // fin for
 
 
}

