// -------------------------------------------------------- 
// jugador.c
// -------------------------------------------------------- 
// Copyright (c) Kronoman 
// En memoria de mi querido padre 
// -------------------------------------------------------- 
// Este modulo contiene todo lo relacionado con el jugador
// Tal como movimiento, disparos, etc...
// -------------------------------------------------------- 
#ifndef JUGADOR_C
#define JUGADOR_C

#include <stdio.h>
#include <allegro.h>
#include "pmask.h"
#include "azar.h"
#include "jugador.h"
#include "kfbuffer.h"
#include "sombras.h"
#include "mapa.h"
#include "combo.h"
#include "error.h"
#include "global.h"
#include "partic.h"
#include "sonido.h"
#include "bomba.h"

/* GLOBALES, son exportadas para usarlas en otros modulos... */
ARMA_JUG arma_tipo[MAX_ARM_CLASS]; /* tipos de armas */
JUGADOR jugador; /* jugador */
DISP_JUG *disp_jug_1 = NULL; /* puntero al 1er disparo del jugador */
TECLADO_JUG teclado_jug; /* teclado del jugador */

/* ----------- RUTINAS DEL JUGADOR ----------------- */

/* Esta funcion dibuja el tablero del jugador
   La barra de energia va al costado, parada (como R...) */
void dibujar_tablero(BITMAP *bmp)
{
char tablero_txt[2048]; // buffer texto del tablero
int y = 0;

/* vaciar la parte vacia.. :P */
    rectfill(bmp,
             bmp->w - 38, bmp->h - 2,
             bmp->w - 2 , 2,
             makecol(16,0,0) );

y = bmp->h - (jugador.vida * (bmp->h-2) / MAX_ENERGIA);

// Barra de energia
if (jugador.vida > 0)
    rectfill(bmp,
             bmp->w - 38, bmp->h - 2,
             bmp->w - 2 , y,
             makecol(128,0,0) );


// Colocar el % de energia que queda
if (nivel_detalle > 3)
{
	text_mode(-1);
	textprintf_centre(bmp, hud_font, bmp->w - 20, (y < bmp->h -text_height(hud_font)) ? y : bmp->h - text_height(hud_font), makecol(255,255,255), "%d%%", (jugador.vida > 0) ? jugador.vida * 100 / MAX_ENERGIA : 0); 
}

// Borde rojo
rect(bmp,
     bmp->w - 38, bmp->h - 2,
     bmp->w - 2 , 2,
     makecol(255,0,0) );

// si el nivel de detalle es muy bajo, chau...
if (nivel_detalle < 1) return;


// Dinero y cantidad de balas 
sprintf(tablero_txt,"[$%09d][%s:%04d][B:%d]", jugador.dinero, arma_tipo[jugador.arma_actual].desc_short,jugador.arma[jugador.arma_actual], jugador.bombas);

text_mode(-1);


// dependiendo del nivel de detalle, usa transparencia, o lo rebordea, o nada
if (nivel_detalle < 9)
   {
   textprintf(bmp, hud_font, 4, 4,makecol(0,0,0), "%s", tablero_txt);
   }
else
   {
    drawing_mode(DRAW_MODE_TRANS, NULL, 0,0); // DEBUG, LENTO!
    rectfill(bmp, 0,0, text_length(hud_font, tablero_txt)+6, text_height(hud_font)+2, makecol(32,32,32));
    solid_mode();
   }

textprintf(bmp, hud_font, 2, 2,makecol(255,255,255), "%s", tablero_txt);
}

/* Esta funcion setea el teclado a default */
void set_teclado_default()
{
  teclado_jug.arr = KEY_UP;
  teclado_jug.abj = KEY_DOWN;
  teclado_jug.izq = KEY_LEFT;
  teclado_jug.der = KEY_RIGHT;
  teclado_jug.sht = KEY_Z;
  teclado_jug.wpn = KEY_X;
  teclado_jug.bmb = KEY_C;
}

/*
Esto resetea totalmente el jugador.
Ideal para COMENZAR UN NUEVO JUEGO.
*/
void reset_total_jugador()
{
	int i;
	reset_parcial_jugador();
	jugador.dinero = jugador.init_money;
	jugador.bombas = jugador.init_bombas;
	
	jugador.vida = MAX_ENERGIA; /* 100 % de energia */
	
	/* no tiene ningun arma, excepto la  0 */
	for(i = 0; i < MAX_ARM_CLASS; i++)
		jugador.arma[i] = 0;
	
	jugador.arma_actual = 0; /* arma 0 seleccionada */
	jugador.arma[0] = 1; /* tiene el arma 0 */
}

/*
Esto resetea el jugador parcialmente, util para cuando se pasa de nivel
Setea la posicion al inicio del nivel, saca la aceleracion, y recarga el timer del arma
*/
void reset_parcial_jugador()
{
	jugador.x = itofix((W_FONDO - jugador.spr[1]->w) / 2);
	jugador.y = itofix(H_FONDO - jugador.spr[1]->h - 5);
	jugador.dx = jugador.dy = 0;
	jugador.lastshoot = 0;
	// Darle suficientes balas del arma 0, que es inifinita
	jugador.arma[0] = 9999;
	
	/* TRUCO / CHEAT - DEBUG - MODO 'DIOS' */
	if (cheat_god_mode) jugador.dinero += 15000; // darle plata $$$
}

/* Esta funcion cambia a la proxima arma disponible */
void jugador_proxima_arma()
{
   /* recorre el array de armas desde la actual hasta dar un loop, y
      coloca la proxima 1era arma que aparezca */
   int i, i2;
   i2 = jugador.arma_actual;
   
   for(i = 0; i < MAX_ARM_CLASS; i++)
    {
     i2++;
     if (i2 > MAX_ARM_CLASS-1) i2 = 0;
     if ( jugador.arma[i2] > 0 && jugador.arma_actual != i2)
        {
          jugador.arma_actual = i2;
          break;
        };
    }
}

/*
   Esta funcion debe ser llamada en la actualizacion logica
   para leer el teclado y actuar en consecuencia
   */
void interpreta_teclado()
{
 
 /* esta mascara de bits  es el dummy driver, que el teclado, mouse o joystick modifican */
 int tk_driver = 0; 

 #define ARR 1
 #define ABJ 2
 #define IZQ 4
 #define DER 8
 #define SHT 16
 #define WPN 32
 #define BMB 64

 /* driver del mouse */
 if (quiere_usar_mouse)
 {
   int mickeyx = 0;  int mickeyy = 0;
   
   if (mouse_needs_poll()) poll_mouse(); // debug, necesario ?
   get_mouse_mickeys(&mickeyx, &mickeyy);

   if (mickeyx < -32 + mouse_velocidad) tk_driver |= IZQ;
   if (mickeyx > 32 - mouse_velocidad) tk_driver |= DER;

   if (mickeyy < -32 + mouse_velocidad) tk_driver |= ARR;
   if (mickeyy > 32 - mouse_velocidad) tk_driver |= ABJ;
   
   if (mouse_b & 1) tk_driver |= SHT;
   if (mouse_b & 2) tk_driver |= WPN;
   if (mouse_b & 4) tk_driver |= BMB;
 }

 /* driver de joystick */
 if (quiere_usar_joystick)
 {
	 poll_joystick(); // absolutamente NECESARIO
	
	 // uso modo digital del joystick
	 if (joy[numero_de_joystick].stick[0].axis[0].d1) tk_driver |= IZQ;
	 if (joy[numero_de_joystick].stick[0].axis[0].d2) tk_driver |= DER;
	
	 if (joy[numero_de_joystick].stick[0].axis[1].d1) tk_driver |= ARR;
	 if (joy[numero_de_joystick].stick[0].axis[1].d2) tk_driver |= ABJ;
	
	 // botones - DEBUG: permitir configurar
	 // solo tomo los 3 primeros botones _existentes_
	 if (joy[numero_de_joystick].num_buttons > 0)
	        if (joy[numero_de_joystick].button[0].b) tk_driver |= SHT;
	
	 if (joy[numero_de_joystick].num_buttons > 1)
	        if (joy[numero_de_joystick].button[1].b)
	        {
	                joy[numero_de_joystick].button[1].b = 0; // hack horrible!
	                tk_driver |= WPN;
	        }
	
	 if (joy[numero_de_joystick].num_buttons > 2)
	    {
        	if (joy[numero_de_joystick].button[2].b) tk_driver |= BMB;
	    }
	    else if (joy[numero_de_joystick].num_buttons > 1) /* emular 3er boton, con los dos primeros apretados */
	    {
			if ((joy[numero_de_joystick].button[0].b) && (joy[numero_de_joystick].button[1].b)) tk_driver |= BMB;
	    }
 }

 /* driver de teclado, siempre funciona */
 if (keyboard_needs_poll()) poll_keyboard();
 
 if (key[teclado_jug.der] ) tk_driver |= DER;
 if (key[teclado_jug.izq] ) tk_driver |= IZQ;
 if (key[teclado_jug.abj] ) tk_driver |= ABJ;
 if (key[teclado_jug.arr] ) tk_driver |= ARR;

 if (key[teclado_jug.wpn] )
 {
         tk_driver |= WPN;
        key[teclado_jug.wpn] = FALSE; /* hack horrible! */
 }
 if (key[teclado_jug.sht] ) tk_driver |= SHT;
 if (key[teclado_jug.bmb] ) tk_driver |= BMB;



 /* Notar que el movimiento izq-der es instantaneo (imito a R??t?r) */
 if (tk_driver & DER) // derecha
 {
    if (jugador.dx < 0)
        jugador.dx = 0;
    else
        jugador.dx = fixadd(jugador.dx, ftofix(VEL_X_J));

 }

 if (tk_driver & IZQ) // izquierda
 {
    if (jugador.dx > 0)
        jugador.dx = 0;
    else
        jugador.dx = fixsub(jugador.dx, ftofix(VEL_X_J));

 }

 if (tk_driver & ARR)
 {
 	if (jugador.dy > 0)
 		jugador.dy = 0;
 	else
	    jugador.dy = fixsub(jugador.dy, ftofix(VEL_Y_J));
	    
 }

 if (tk_driver & ABJ)
 {
 	if (jugador.dy < 0)
 		jugador.dy = 0;
 	else
    	jugador.dy = fixadd(jugador.dy, ftofix(VEL_Y_J));
    	
 }

 if (tk_driver & SHT) agregar_disparo_jug(); /* disparar */

 if (tk_driver & WPN) /* proxima arma... */
 {
   jugador_proxima_arma();
 }

/*
Agregado 16/12/2003
 esto soluciona un 'bug' en el cual, si se presiona LEFT/RIGHT, se aprieta UP/DOWN y se suelta, 
la cosa sigue moviendo loca, no se porque 

descripcion del problema reportado por Ysaneya
For example, say your ship is at the bottom center of the screen. You press the up and left arrows at the same time, to make it go into the up-left direction. Your ship starts moving, fine.
Now, while the ship is moving, you release the up arrow, keeping the left arrow pressed. Your ship should logically move to the left, and slowly stop moving up, right ?
Well no, it continues the exact same movement in the up-left direction. That's why it's unplayable. Can't avoid any bullets.

*/
if (jugador.vida > 1)
{
	if (!(tk_driver & DER) && !(tk_driver & IZQ)) jugador.dx = 0;	
	if (!(tk_driver & ARR) && !(tk_driver & ABJ)) jugador.dy = 0;	
}

 if (tk_driver & BMB) // lanzar bomba especial
 {
	if (jugador.bombas > 0 && !bomba_esta_activa)
	{
		jugador.bombas --;
		bomba_esta_activa = TRUE;
		bomba_detonacion = 7; // tiempo de destruccion masiva... :)
		detonar_totalmente_el_piso(scroll_mapa);
		
		// sonido! 
		if (bomba_sonido != NULL) tocar_sonido(bomba_sonido, 255, 128, 1000);
	}
 }


}

/* Esta funcion mueve el jugador, y no permite que se salga de pantalla, etc
   Precisa saber el desplazamiento 'y' del mapa
   Ademas, verifica la vida que le queda al jugador,hecha humo, lo explota y lo mata... :P
*/
void mover_jugador(int fy) {

    /* TRUCO / CHEAT - DEBUG - MODO 'DIOS' */
    if (cheat_god_mode) jugador.vida = MAX_ENERGIA;

    if (jugador.dx > ftofix(MAX_VEL_J_X) ) jugador.dx = ftofix(MAX_VEL_J_X);
    if (jugador.dy > ftofix(MAX_VEL_J_Y) ) jugador.dy = ftofix(MAX_VEL_J_Y);

    if (jugador.dx < ftofix(MAX_VEL_J_X * (-1.0)) ) jugador.dx = ftofix(MAX_VEL_J_X * (-1.0));
    if (jugador.dy < ftofix(MAX_VEL_J_Y * (-1.0)) ) jugador.dy = ftofix(MAX_VEL_J_Y * (-1.0));

    jugador.x = fixadd (jugador.x, jugador.dx);
    jugador.y = fixadd (jugador.y, jugador.dy);

    if (jugador.dx > 0)    {
        jugador.dx = fixsub(jugador.dx, ftofix(FRC_X_J));
        if (jugador.dx < 0) jugador.dx = 0;
    }

    if (jugador.dx < 0)    {
        jugador.dx = fixadd(jugador.dx, ftofix(FRC_X_J));
        if (jugador.dx > 0) jugador.dx = 0;
    }

    if (jugador.dy > 0)    {
        jugador.dy = fixsub(jugador.dy, ftofix(FRC_Y_J));
        if (jugador.dy < 0) jugador.dy = 0;
    }

    if (jugador.dy < 0)    {
        jugador.dy = fixadd(jugador.dy, ftofix(FRC_Y_J));
        if (jugador.dy > 0) jugador.dy = 0;
    }

    if (jugador.y < 0) jugador.y = 0;

    if (jugador.x < 0) jugador.x = 0;

    if (fixtoi(jugador.x) > ANCHO_FB - jugador.spr[1]->w ) jugador.x = itofix(ANCHO_FB - jugador.spr[1]->w);
    
    if (fixtoi(jugador.y) < fy ) jugador.y = itofix(fy);
    if (fixtoi(jugador.y) > fy + ALTO_FB - jugador.spr[1]->h ) jugador.y = itofix(fy + ALTO_FB - jugador.spr[1]->h);

    jugador.lastshoot--; /* ir recargando arma */
    if (jugador.lastshoot < 0) jugador.lastshoot = 0;

    
    /* Energia del jugador... exploto? combustionar al maldito!
       y hacerle perder el control! */
    if (jugador.vida < 1)
        {
         jugador.vida -= rand()%3+1; /* combustiono la nave */

         if (rand()%100 < abs((jugador.vida * 100) / JUG_MUERTO) ) /* incremental al irse combustionando */
         {
         /* agregar explosiones peque~as... */
              pone_explo_pixel(&ptr_explo_arriba,
                               fixtoi(jugador.x)+rand()%jugador.spr[1]->w,
                               fixtoi(jugador.y)+rand()%jugador.spr[1]->h,
                               rand()%jugador.spr[1]->w + jugador.spr[1]->w/3,
                               rand()%30+10,
                               ftofix(0.01));
         /* descontrolar la nave */
         if (rand()%100 < 25)
         	{
            	jugador.dx = (rand()%100 < 50) ? ftofix(MAX_VEL_J_X * (-1.0)) : ftofix(MAX_VEL_J_X);
            	jugador.dy = (rand()%100 < 50) ? ftofix(MAX_VEL_J_Y * (-1.0)) : ftofix(MAX_VEL_J_Y);
         	}
         }

         /* al aproximarse al final, recontra-explotar con pedazos y todo... */
         if (jugador.vida < (JUG_MUERTO / 4) )
              poner_explosion_nave(fixtoi(jugador.x)+jugador.spr[1]->w/2, fixtoi(jugador.y)+jugador.spr[1]->h/2, rand()%20+20, rand()%40+40, (rand()%100 < 25) ? -1 : 0);

         if (jugador.vida <= JUG_MUERTO) /* ahora si murio... */
             {
              /* accionar en consecuencia... */
               salir_del_juego = TRUE;
             }
        }
        else
        {
        /* Si aun esta vivo, y la energia del jugador es < 20 %, hechar humo y chispas, proporcional al jugador */	
        	if ((jugador.vida * 100 / MAX_ENERGIA < 30) && (nivel_detalle > 8) )
        		{
        			if (rand()%((jugador.vida * 100 / MAX_ENERGIA)+1)< 15) // a medida que se quema, larga mas humo, notar el +1 impide division por 0
        			{
        			int col, it;
        			col = rand()%32+16;
        			// humo
        		   for (it=0; it < rand()%3+1; it++)        
        			agrega_particula( fixadd(jugador.x, itofix(jugador.spr[1]->w/2 + rand_ex(10,-10))),  fixadd(jugador.y, itofix(jugador.spr[1]->h + rand_ex(0, -5))),
                             fixmul(jugador.dx, ftofix( (float)rand_ex(0, -25)/100.0 ) ), ftofix((float)rand_ex(25, 70)/10.0),
                             rand_ex(5,15),
                             makecol(col,col,col), rand_ex(3,5), 1, 
                             -1, ftofix((float)rand_ex(5, 15)/10.0),
                             NULL);	
                   // chispa  
                   for (it=0; it < rand()%3+1; it++)        
	                   agrega_particula( fixadd(jugador.x, itofix(jugador.spr[1]->w/2 + rand_ex(10,-10))),  fixadd(jugador.y, itofix(jugador.spr[1]->h + rand_ex(0, -5))),
	                             ftofix( (float)rand_ex(25, -25)/10.0  ), 
	                             ftofix((float)rand_ex(10, 30)/10.0),
	                             rand_ex(3,10),
	                             makecol(255,rand()%255,0),
	                             rand_ex(1,2),
	                             1,
	                             0, 0,NULL);	
	               }
        		}
        	
        }
}


/*
   Esta funcion dibuja la nave del jugador en
   el bitmap, desplazado x,y,
   dibuja tambien una sombra que se desplaza
   en relacion al borde inferior del bitmap y el centro y del mismo
   de manera de dar un look 3D (como el viejo Rapt*r :^P )
   TIENE que estar seteado el colormap de transparencia
   */
void dibujar_nave_jugador(BITMAP *bmp, int x, int y)
{
  int idx = 0;
  /* Ver que sprite dibujar, si se esta inclinando izq-med-der */
  if (jugador.dx > ftofix(-4.5) && jugador.dx < ftofix(4.5)) /* medio.. */
  {
    idx = 1;
  }
  else
  {
   if (jugador.dx < 0) /* izq */
       idx = 0;
   else
       idx = 2;
  }
   /* colocar la sombra  */
   colocar_sombra(bmp, jugador.spr[idx+3], fixtoi(jugador.x)-x, fixtoi(jugador.y)-y);

   /* FALTA dibujar estela del motor - DEBUG */

   /* Colocar jugador */
   draw_sprite(bmp, jugador.spr[idx], fixtoi(jugador.x)-x, fixtoi(jugador.y)-y);
}

/* ----------- RUTINAS DE LA LISTA DE DISPAROS ----------------- */

/*
   Esta funcion agrega un disparo a la lista de disparos
   Automaticamente lo agrega usando el arma del jugador, etc
   Ademas, verifica el timer, y todo lo demas.
*/
void agregar_disparo_jug() 
{
    int c = 0, i = 0; // auxiliares

    if (jugador.arma_actual < 0) return; /* jugador desarmado? shit! */

    if (jugador.lastshoot > 0 ) return; /* NO es tiempo de poder disparar todavia */

    if (jugador.vida < 0) return; /* esta muerto... no tenes armas! chau salame... */

    // Verificar cantidad de balas, a menos que sea el arma 'infinita' (0)
    if (jugador.arma[jugador.arma_actual] < 1 && jugador.arma_actual != 0)
    {
      // poner el arma siguiente disponible
      jugador_proxima_arma();
      return;
    }
    else
    {
     // Descontar balas
     if (jugador.arma_actual != 0) jugador.arma[jugador.arma_actual]--;
     if (jugador.arma[jugador.arma_actual] < 0) jugador.arma[jugador.arma_actual] = 0;
    }
    
    jugador.lastshoot = arma_tipo[jugador.arma_actual].firerate;

    /* EJECUTAR LOS SONIDOS DEL DISPARO EN GLORIOSO STEREO */
    tocar_sonido_paneado(fixtoi(jugador.x),
                         arma_tipo[jugador.arma_actual].snd[0],
                         rand_ex(240, 255),
                         rand_ex(900, 1100));

    switch (arma_tipo[jugador.arma_actual].t)
    {
     case 1:
     case 0:
           c = 1;
      break;

     case 2:
     case 5:
           c = 3;
     break;

     case 3:
     case 4:
           c = 2;
     break;
     
     case 6: // flak... invento especial... :P
     	  c = 8;
     break;
     
     default:
     /* DEBUG: arma desconocida, no agregar nada.. */
         c = -1;
     break;
    }

 /* ir agregando disparos */
 for (i=0; i<c; i++) /* Si el tipo de disparo es doble, o triple, realizar el proceso varias veces */
  {
    DISP_JUG *nueva = malloc(sizeof(DISP_JUG));
    nueva->next = disp_jug_1;
    disp_jug_1  = nueva;

    if (nueva != NULL) /* si el malloc funciono, seteo los datos... */
    {
     nueva->arma = jugador.arma_actual;
     nueva->vida = arma_tipo[jugador.arma_actual].vida;

     // valores default de coordenadas, genericos para todos los disparos
     nueva->y = fixadd(jugador.y, itofix(jugador.spr[1]->h - arma_tipo[jugador.arma_actual].spr->h)); // la y sale del fondo de la nave
     nueva->x = fixadd(jugador.x, itofix(jugador.spr[1]->w/2 -  arma_tipo[jugador.arma_actual].spr->w / 2)); // la x sale del centro de la nave

     nueva->dx = arma_tipo[jugador.arma_actual].vx;
     nueva->dy = arma_tipo[jugador.arma_actual].vy;

     /* Dependiendo del tipo de disparo, setear su posicion */
     switch (arma_tipo[jugador.arma_actual].t)
     {
       case 0: /* recto */
           nueva->dx = arma_tipo[jugador.arma_actual].vx;
           nueva->dy = arma_tipo[jugador.arma_actual].vy;
       break;

       case 1: /* direccion al azar */
       	   if (arma_tipo[jugador.arma_actual].vx == 0) arma_tipo[jugador.arma_actual].vx = itofix(5);
       
           nueva->dx = ftofix((float)rand_ex(-fixtoi(arma_tipo[jugador.arma_actual].vx)*10, fixtoi(arma_tipo[jugador.arma_actual].vx)*10)/10.0);
           nueva->dy = arma_tipo[jugador.arma_actual].vy;
       break;

       case 2: /* abanico triple */
           /* verificacion de seguridad */
           if (arma_tipo[jugador.arma_actual].vx == 0) arma_tipo[jugador.arma_actual].vx = itofix(5);

           if (i == 0)  nueva->dx = arma_tipo[jugador.arma_actual].vx;
           if (i == 1)  nueva->dx = 0;
           if (i == 2)  nueva->dx = fixmul(itofix(-1), arma_tipo[jugador.arma_actual].vx);
           nueva->dy = arma_tipo[jugador.arma_actual].vy;
       break;

       case 3: /* doble recto, vx controla la apertura del disparo (debe ser >= 0) */

           if (i == 0)
           {
              nueva->x = fixadd(jugador.x, itofix(jugador.spr[1]->w - arma_tipo[jugador.arma_actual].spr->w/2));
              nueva->dx = arma_tipo[jugador.arma_actual].vx;
           }
           else
           {
             nueva->x = fixsub(jugador.x, itofix(arma_tipo[jugador.arma_actual].spr->w/2));
             nueva->dx = fixmul(itofix(-1),arma_tipo[jugador.arma_actual].vx);
           }

           nueva->dy = arma_tipo[jugador.arma_actual].vy;
       break;

       case 4: /* 1 del centro, 1 al azar por disparo */
           if (i == 0)
           {
           nueva->dx = arma_tipo[jugador.arma_actual].vx;
           nueva->dy = arma_tipo[jugador.arma_actual].vy;
           }
           else
           {
           nueva->dx = ftofix((float)rand_ex(-50, 50)/10.0);
           nueva->dy = arma_tipo[jugador.arma_actual].vy;
           }
       break;
       
       case 5: // 1 del centro, 1 de la izq, 1 de la derecha (tipo misile pod, incluye una variacion del 10% al azar en la velocidad en y, para efecto misile pod)
       	       // el disparo 0 sale del centro, por default
       	       if (i == 1)  nueva->x = fixadd(jugador.x, itofix(jugador.spr[1]->w - arma_tipo[jugador.arma_actual].spr->w/2)); 
	       if (i == 2)  nueva->x = fixsub(jugador.x, itofix(arma_tipo[jugador.arma_actual].spr->w/2));
	       nueva->dy = fixmul(nueva->dy, ftofix( (float)(100 + rand_ex(-10, 10))/100.0 ) );
       break;
       case 6: // ca~on de flak
       	       if (i == 0)  nueva->x = fixadd(jugador.x, itofix(jugador.spr[1]->w - arma_tipo[jugador.arma_actual].spr->w/2)); 
	       if (i == 1)  nueva->x = fixsub(jugador.x, itofix(arma_tipo[jugador.arma_actual].spr->w/2));
	       
	       if (i > 1) 
	       	nueva->dx = ftofix((float)rand_ex(-fixtoi(arma_tipo[jugador.arma_actual].vx)*10, fixtoi(arma_tipo[jugador.arma_actual].vx)*10)/10.0);
	       else
	       	nueva->dx = 0; 	
       
       break;
       
       default: /* ARMA desconocida - DEBUG */
           nueva->dx = 0;
           nueva->dy = itofix(-15);
       break;

     } /* fin switch */

     /* Al terminar de setear el disparo, agrego una explosion, indicando
        que de alli salio el disparo... */
      /* DEBUG, con el dibujo de la explosion se ve horrible! habria que
         dibujar algo mejor... tipo una flama larga o algo */

     // pone_explo_pixel(&ptr_explo_arriba,
     //                   fixtoi(nueva->x),
     //                   fixtoi(nueva->y),
     //                   rand()%15+10,
     //                   rand()%5+5,
     //                   ftofix(0.01));

    } /* fin != NULL */
  } /* fin for */
};

/*
  Esta funcion actualiza la logica de los disparos
  ademas de remover los que tienen vida < 0
  y colocar particulas donde se mueven (si tiene un proyector de particulas ON)
    Precisa saber el desplazamiento del mapa (fy)
*/
void mover_disparos_jug(int fy) {
    DISP_JUG **tmp_p = &disp_jug_1;
    int i1; /* auxiliar para el for de la particula */


    while (*tmp_p) {
        DISP_JUG *tmp = *tmp_p;

        /* aqui muevo */
        tmp->x = fixadd(tmp->x, tmp->dx);
        tmp->y = fixadd(tmp->y, tmp->dy);
        (tmp->vida)--; 

        /* DEBUG: agregar estela en particulas,
        esto podria ser una funcion aparte porque es bastante complicada la sintaxis... */
        if (nivel_detalle > 5)
            for (i1=0;
                 i1 < rand_ex(arma_tipo[tmp->arma].est_cant[0], arma_tipo[tmp->arma].est_cant[1]);
                 i1++)
               agrega_particula( fixadd(tmp->x, itofix(arma_tipo[tmp->arma].spr->w/2)),
                                 fixadd(tmp->y, itofix(arma_tipo[tmp->arma].spr->h)),
                                 ftofix(rand_ex(arma_tipo[tmp->arma].est_dx[0], arma_tipo[tmp->arma].est_dx[1])/100.0),
                                 ftofix(rand_ex(arma_tipo[tmp->arma].est_dy[0], arma_tipo[tmp->arma].est_dy[1])/100.0),
                                 rand_ex(arma_tipo[tmp->arma].est_vida[0], arma_tipo[tmp->arma].est_vida[1]),
                                 makecol(rand_ex(arma_tipo[tmp->arma].est_color[0][0], arma_tipo[tmp->arma].est_color[0][1]),
                                         rand_ex(arma_tipo[tmp->arma].est_color[1][0], arma_tipo[tmp->arma].est_color[1][1]),
                                         rand_ex(arma_tipo[tmp->arma].est_color[2][0], arma_tipo[tmp->arma].est_color[2][1])),
                                 rand_ex(arma_tipo[tmp->arma].est_rad[0],arma_tipo[tmp->arma].est_rad[1]),
                                 rand_ex(arma_tipo[tmp->arma].est_tipo[0], arma_tipo[tmp->arma].est_tipo[1]),
                                 arma_tipo[tmp->arma].est_transp,0,
                                 NULL );
 

        /* LA VERIFICACION SI CHOCA EL DISPARO CONTRA LOS ENEMIGOS SE HACE EN enemigo.c!!! */

        /* Verificar si rompio el fondo y quemarlo!!!! */
        /* Si explota, el disparo se elimina gradualmente */
        if (explotar_fondo((fixtoi(tmp->x)+arma_tipo[tmp->arma].spr->w/2)/W_GR,
                           (fixtoi(tmp->y)+arma_tipo[tmp->arma].spr->h/2)/H_GR,
                           arma_tipo[tmp->arma].punch*1.5))
           {
            tmp->vida /= 2;
           }

        if (fixtoi(tmp->y) < fy - ALTO_FB/4 ) tmp->vida = -1; /* el disparo se fue por arriba */
        if (fixtoi(tmp->y) > fy + ALTO_FB*2 ) tmp->vida = -1; /* el disparo se fue por abajo */

        /* Remocion de disparos muertos */
        if (tmp->vida < 0) {
          /* murio, eliminar!!! */
          *tmp_p = tmp->next;
          free(tmp);
        }
        else
        {
            tmp_p = &tmp->next; /* siguiente por favor! */
        }
    }
}

/* Dibuja los disparos, desplazados por x,y
   sobre el bitmap bmp */
void dibujar_disp_jug(BITMAP *bmp, int x, int y) {

    DISP_JUG *tmp = disp_jug_1;

    while (tmp)
    {
      draw_sprite(bmp, arma_tipo[tmp->arma].spr, fixtoi(tmp->x)-x, fixtoi(tmp->y)-y);

//        if (KRONO_QUIERE_DEBUG)
//            {
//                text_mode(0);
//                textprintf(bmp, font, fixtoi(tmp->x)-x, fixtoi(tmp->y)-y, makecol(255,255,255), "%d" , tmp->vida);
//            }

      tmp = tmp->next; /* proximo... */
    }
}


/* Esta funcion se debe llamar cuando no se precise mas la lista
   Libera la RAM usada y reinicia la lista
   */
void liberar_lista_disparos_jug() {
    DISP_JUG *tmp = disp_jug_1;
    disp_jug_1 = NULL;

    while (tmp) {
        DISP_JUG *next = tmp->next;
        free(tmp);
        tmp = next;
    }
}

#endif
