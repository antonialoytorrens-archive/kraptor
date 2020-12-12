// --------------------------------------------------------
// enemigo.c
// -------------------------------------------------------- 
// Copyright (c) Kronoman 
// En memoria de mi querido padre 
// -------------------------------------------------------- 
// Este modulo contiene todo lo relacionado con los enemigos
// Tal como movimiento, disparos, etc...
// -------------------------------------------------------- 
#ifndef ENEMIGO_C
#define ENEMIGO_C

#include <stdio.h>
#include "allegro.h"
#include "pmask.h"
#include "azar.h"
#include "enemigo.h"
#include "kfbuffer.h"
#include "jugador.h" /* lo preciso para poder verificar
                        los disparos contra el jugador... jeje */
#include "sombras.h"
#include "combo.h"
#include "partic.h"
#include "global.h"
#include "sonido.h"
#include "bomba.h"
#include "ia.h"
#include "premio.h"
#include "error.h"
/* GLOBALES */
ARMA_ENE arma_ene[MAX_E_CLASS]; /* armamento enemigo */
ENEM_T enem_t[MAX_E_CLASS]; /* tipos de enemigos */
ENEMIGO *enemigo_1 = NULL; /* puntero al 1er enemigo activo */
DISP_ENE *disp_ene_1 = NULL; /* puntero al 1er disparo activo */

/* esta variable NO es necesaria, solo la uso para
   ver cuantos enemigos hay en memoria, y de esa manera,
   revisar la performance... */
int cant_enemigos_debug = 0;

/* Agrega un enemigo en x, y de tipo 'tipo' */
void agregar_enemigo(int x, int y, int tipo )
{
    ENEMIGO *nueva = malloc(sizeof(ENEMIGO));
    nueva->next = enemigo_1;
    enemigo_1 = nueva;

    if (nueva != NULL) /* si el malloc funciono, seteo los datos... */
    {
        nueva->x = itofix(x);
        nueva->y = itofix(y);
        nueva->vida = enem_t[tipo].vida; /* que corta es la vida de un piloto enemigo... */
        nueva->ene_t = tipo; /* que somos? yankis gallinas o pilotos de verdad? */
        // resetear la IA
        nueva->bytecode_actual = -1; // aun no ejecute nada, por eso -1
        nueva->bytecode_loop = 0; // empezar con la 1era instruccion

        nueva->ia_node = enem_t[tipo].ia_node;
        // debo comenzar la IA en un punto al azar?
        if (enem_t[tipo].ia_azar != 0) nueva->bytecode_actual = rand_ex(-1, nueva->ia_node->size-1);
        
        // animaciones, etc...
        nueva->spr_actual = rand()%4;
        nueva->spr_delay = 0;
    }
}

/* Esta funcion altera la IA del enemigo
   llamar en cada ciclo...
   */
void IA_enemigo(ENEMIGO *ene) 
{
    
// interpretar bytecodes de la IA
if (ene->ia_node == NULL) return; // no tiene IA, descerebrado...
if (ene->ia_node->size < 1) return; // IA vacia, que paso?
    
ene->bytecode_loop--; // ejecute 1 instruccion


if (ene->bytecode_loop < 0)
   {
    // proxima instruccion
    ene->bytecode_actual ++;
    if (ene->bytecode_actual > ene->ia_node->size - 1) ene->bytecode_actual = 0;
    if (ene->bytecode_actual > ene->ia_node->size-1) return; // seguridad... :)
    ene->bytecode_loop = ene->ia_node->code[ene->bytecode_actual].loop;

    // copiar bytecode, solo campos importantes... el resto indeterminados... :)
    ene->bytecode_exe.x1 = rand_ex(ene->ia_node->code[ene->bytecode_actual].x1, ene->ia_node->code[ene->bytecode_actual].x2);
    ene->bytecode_exe.y1 = rand_ex(ene->ia_node->code[ene->bytecode_actual].y1, ene->ia_node->code[ene->bytecode_actual].y2);

   }

   
if (ene->bytecode_actual > ene->ia_node->size-1) return; // seguridad... :)
    
   // mover enemigo
   ene->x = fixadd(ene->x, itofix(ene->bytecode_exe.x1));
   ene->y = fixadd(ene->y, itofix(ene->bytecode_exe.y1));


        /* debo disparar ? */
        if (ene->ia_node->code[ene->bytecode_actual].weapon > -1)
        {
            // cambio de arma del enemigo
            ene->arma_actual = ene->ia_node->code[ene->bytecode_actual].weapon;
            // disparar
            agregar_disparo_ene(ene);
        }


}


/*
   Esta funcion actualiza los enemigos
   Verifica colisiones con disparos de jugador y con el jugador mismo
   Precisa saber el desplazamiento del fondo, 'fy' ya que
   cuando el enemigo sale de pantalla, se elimina...
   */
void mover_enemigos(int fy)
{
    ENEMIGO **tmp_p = &enemigo_1;
    DISP_JUG *tmpd = NULL;
    ENEMIGO *tmp = NULL;

    cant_enemigos_debug = 0; /* DEBUG: innecesario */

    while (*tmp_p) {

        cant_enemigos_debug++; /* DEBUG: innecesario */

        tmp = *tmp_p;
          /* hacer IA del enemigo */
            IA_enemigo(tmp);

        // animacion del enemigo
        tmp->spr_delay--;
        if (tmp->spr_delay < 0)
            {
             tmp->spr_delay = enem_t[tmp->ene_t].spr_delay; // reiniciar contador
             tmp->spr_actual++; // cambiar sprite
             if (tmp->spr_actual > 3) tmp->spr_actual = 0;
            }


          /* verificar colisiones... */

          /* colision contra el jugador? */
          if (check_pmask_collision(enem_t[tmp->ene_t].mask[tmp->spr_actual], jugador.mask,
                                    fixtoi(tmp->x), fixtoi(tmp->y),
                                    fixtoi(jugador.x), fixtoi(jugador.y) ) != 0)
                                    {
            /* DEBUG: choco al jugador!, falta sonido! */

          pone_explo_pixel(&ptr_explo_arriba,
                           fixtoi(tmp->x)+rand()%enem_t[tmp->ene_t].spr[tmp->spr_actual]->w,
                           fixtoi(tmp->y)+rand()%enem_t[tmp->ene_t].spr[tmp->spr_actual]->h,
                           rand()%enem_t[tmp->ene_t].spr[tmp->spr_actual]->w+enem_t[tmp->ene_t].spr[tmp->spr_actual]->w/3,
                           rand()%30+20,
                           ftofix(0.01));

            jugador.vida -= enem_t[tmp->ene_t].peso + rand()%5; /* restar energia al jugador */
            tmp->vida -= rand()%5 + 5;  /* restarme energia tambien */
          };

          /* loop:
             recorrer disparos del jugador
             DEBUG: esto es muy lento!! */

          tmpd = disp_jug_1;
          while (tmpd) {
                /* chequear colision, solo si el disparo esta activo,
                   de la manera hecha siguiente, si el disparo esta muerto,
                   evito comprobar la mascara, que tarda mas...*/

                  if ( tmpd->vida > 0)
                  if (check_pmask_collision(enem_t[tmp->ene_t].mask[tmp->spr_actual], arma_tipo[tmpd->arma].mask,
                                            fixtoi(tmp->x), fixtoi(tmp->y),
                                            fixtoi(tmpd->x), fixtoi(tmpd->y) ) != 0 )
                                            {

                    /* DEBUG: choco al disparo del jugador, restar energia, etc! */
                    poner_explosion_nave(fixtoi(tmpd->x),
                                         fixtoi(tmpd->y),
                                         rand()%20+20, rand()%10+5+arma_tipo[tmpd->arma].punch*2, 0);

                    /* DEBUG: sonido del arma
                              del jugador pegandole al enemigo */
                    tocar_sonido_paneado(fixtoi(tmpd->x),
                                         arma_tipo[tmpd->arma].snd[1],
                                         rand_ex(200,255),
                                         rand_ex(900,1100));


                    /* Esta comparacion es para NO pagarle mas de una vez al jugador
                       por la muerte... */
                    if (tmp->vida > 0 )
                    {

                      tmp->vida -= arma_tipo[tmpd->arma].punch; /* restar energia al enemigo */

                      /* DEBUG: mato al enemigo */
                      if (tmp->vida <= 0)
                      {
                      int ttt;
                      for (ttt=0; ttt < rand()%3 + enem_t[tmp->ene_t].peso/2+1; ttt++)
                         {
                         pone_explo_pixel(&ptr_explo_arriba,
                                          fixtoi(tmp->x)+rand()%enem_t[tmp->ene_t].spr[tmp->spr_actual]->w,
                                          fixtoi(tmp->y)+rand()%enem_t[tmp->ene_t].spr[tmp->spr_actual]->h,
                                          rand()%enem_t[tmp->ene_t].spr[tmp->spr_actual]->w+enem_t[tmp->ene_t].spr[tmp->spr_actual]->w/3,
                                          rand()%30+20,
                                          ftofix(0.01));
                         }
                         // Soltar premio, si lo hubiera... -DEBUG-
                         if ( (enem_t[tmp->ene_t].premio_idx > -1) && (rand()%100+1 < enem_t[tmp->ene_t].premio_prob ) )
                                   agrega_premio(enem_t[tmp->ene_t].premio_idx, tmp->x, tmp->y);
                         
                         // Pagar... 
                        jugador.dinero += enem_t[tmp->ene_t].dinero;
                      }
                   } /* fin de no pagar 2 veces */

                    tmpd->vida = -1; /* eliminar disparo, la memoria
                                        se libera en jugador.c */
                  } /* fin chequear colision */

                tmpd = tmpd->next; /* siguiente */

            } /* fin loop de verificar disparos */

        /* Si esta activa la bomba especial, restar energia progresivamente */
        if (bomba_esta_activa && tmp->vida > 0)
           {
           tmp->vida -= rand()%10+5;
                   if (tmp->vida < 1)
                   {
                           jugador.dinero += enem_t[tmp->ene_t].dinero; // pagar
                           tmp->vida = -1; // fuerzo que se 'combustione' completamente
                   }
           }

        /* Ir combustionando el enemigo cuando muere (energia <= 0) */
        if (tmp->vida <= 0 )
           {
                 tmp->vida -= rand()%10+10;


                 /* Explosiones peque~as */
                 if (rand()%150 < abs((tmp->vida * 100) / ENE_MUERTO) ) /* incremental al irse combustionando */
                 {
                    /* sonido */
                    if (rand()%10 < 5)
                            {
                             tocar_sonido_paneado(fixtoi(tmp->x),
                                                  explo_cache_snd[rand()%3],
                                                  rand_ex(128,255),
                                                  rand_ex(900, 1000));
                            }

                     pone_explo_pixel(&ptr_explo_arriba,
                                      fixtoi(tmp->x)+rand()%enem_t[tmp->ene_t].spr[tmp->spr_actual]->w,
                                      fixtoi(tmp->y)+rand()%enem_t[tmp->ene_t].spr[tmp->spr_actual]->h,
                                      rand()%enem_t[tmp->ene_t].spr[tmp->spr_actual]->w+enem_t[tmp->ene_t].spr[tmp->spr_actual]->w/3,
                                      rand()%30+10,
                                      ftofix(0.01));
    
                    /* Descontrolar enemigo */
                    // tmp->dx = (rand()%100 < 50) ? ftofix(MAX_VEL_E_X * (-1.0)) : ftofix(MAX_VEL_E_X);
                    // tmp->dy = (rand()%100 < 50) ? ftofix(MAX_VEL_E_Y * (-1.0)) : ftofix(MAX_VEL_E_Y);
                }
    
                /* al aproximarse al final, recontra-explotar... */
                if (tmp->vida < (ENE_MUERTO / 10)*9  && nivel_detalle > 4)
                            {
                                pone_explo_pixel(&ptr_explo_arriba,
                                                  fixtoi(tmp->x)+enem_t[tmp->ene_t].spr[tmp->spr_actual]->w/2,
                                                  fixtoi(tmp->y)+enem_t[tmp->ene_t].spr[tmp->spr_actual]->h/2,
                                                  enem_t[tmp->ene_t].spr[tmp->spr_actual]->w+rand()%10,
                                                  rand()%10+10,
                                                  ftofix(0.01));

                                 /* PARTICULAS Y PEDAZOS DE NAVES AL MORIR... */
                                if (tmp->vida-10 <= ENE_MUERTO)
                                        poner_explosion_nave(fixtoi(tmp->x)+enem_t[tmp->ene_t].spr[tmp->spr_actual]->w/2,
                                                             fixtoi(tmp->y)+enem_t[tmp->ene_t].spr[tmp->spr_actual]->h/2,
                                                             rand()%20+20,
                                                             rand()%10+5, -1);
                             }
           } /* fin combustionar */


          /* verificar limites de pantalla visible */
          /* nota: la IA rebota en las X, queda cool. */
          if (tmp->x < 0) { tmp->x = 0; tmp->bytecode_exe.x1 *= -1; }

          if (fixtoi(tmp->x) > ANCHO_FB - enem_t[tmp->ene_t].spr[tmp->spr_actual]->w )
                  {
                    tmp->x = itofix(ANCHO_FB - enem_t[tmp->ene_t].spr[tmp->spr_actual]->w);
                    tmp->bytecode_exe.x1 *= -1; 
                  }

          if (fixtoi(tmp->y) < fy - (enem_t[tmp->ene_t].spr[tmp->spr_actual]->h*2)) tmp->y = itofix(fy - (enem_t[tmp->ene_t].spr[tmp->spr_actual]->h*2));
    
          if (fixtoi(tmp->y) < fy )
             {
               tmp->y = itofix(fy+2); /* DEBUG: mejorar esto para que no 'salte' desde arriba de pantalla! */
               tmp->bytecode_exe.y1 *= -1;
             }

         // es un BOSS, no pasar de 2/3 de pantalla?
         if (enem_t[tmp->ene_t].ia_boss)
         {
           if (fixtoi(tmp->y)+enem_t[tmp->ene_t].spr[tmp->spr_actual]->h > (fy+ALTO_FB) / 3 * 2 )
           {
                   tmp->y = itofix((fy+ALTO_FB) / 3 * 2 - enem_t[tmp->ene_t].spr[tmp->spr_actual]->h);
                   tmp->bytecode_exe.y1 *= -1;
           }
         }
         else
         {  // desaparecer al salir por abajo
            if (fixtoi(tmp->y) > fy + ALTO_FB*1.05) tmp->vida = ENE_MUERTO; 
         }

        /* Verificacion para ir limpiando la lista de enemigos muertos */
        if (tmp->vida <= ENE_MUERTO) {
          /* murio, eliminar y pasar al siguiente!!! */
          *tmp_p = tmp->next;
          free(tmp);
        } else {
            tmp_p = &tmp->next; /* nadie murio, siguiente por favor! */
        }
    } /* fin loop recorrer enemigos */
}

/*
   Esta funcion dibuja los enemigos en
   el bitmap, desplazado x,y,
   dibuja tambien una sombra que se desplaza
   en relacion al borde inferior del bitmap y el centro y del mismo
   de manera de dar un look 3D (como el viejo Rapt*r :^P )
   TIENE que estar seteado el colormap de transparencia
   */
void dibujar_enemigos(BITMAP *bmp, int x, int y)
{
    ENEMIGO *tmp = enemigo_1;

    while (tmp) {


       /* Colocar enemigo */
       if (enem_t[tmp->ene_t].spr[tmp->spr_actual] != NULL)
       {
       	/* colocar la sombra */
       	colocar_sombra(bmp, enem_t[tmp->ene_t].spr_shadow[tmp->spr_actual], fixtoi(tmp->x)-x, fixtoi(tmp->y)-y);
       	draw_sprite(bmp, enem_t[tmp->ene_t].spr[tmp->spr_actual], fixtoi(tmp->x)-x, fixtoi(tmp->y)-y);
	}
	else
	{
		char errstr[1024];
		sprintf(errstr, "ERROR: dibujar_enemigos, hay un enemigo sin sprite (clase: %d, sprite: %d)!", tmp->ene_t, tmp->spr_actual);
		levantar_error(errstr);
	}


        /* DEBUG: mostrar energia del enemigo */
//        if (KRONO_QUIERE_DEBUG)
//            {
//                text_mode(0);
//                textprintf(bmp, font, fixtoi(tmp->x)-x, fixtoi(tmp->y)-y, makecol(255,255,255), "%d" , tmp->vida);
//            }

       tmp = tmp->next;
    }
}


/* Esta funcion se debe llamar cuando no se precise mas la lista
   Libera la RAM usada y reinicia la lista
   */
void liberar_lista_enemigos() {
    ENEMIGO *tmp = enemigo_1;
    enemigo_1 = NULL;

    while (tmp) {
        ENEMIGO *next = tmp->next;
        free(tmp);
        tmp = next;
    }
}

/* ------- DISPAROS DE LOS ENEMIGOS A PARTIR DE AQUI -------- */

/*
   Esta funcion agrega un disparo a la lista de disparos
   Automaticamente lo agrega usando el arma del enemigo, etc
   Ademas, verifica el timer, y todo lo demas.
   Pasarle el puntero al enemigo que dispara.
*/
void agregar_disparo_ene(ENEMIGO *ene)
{
    int c = 0, i = 0;

    if (ene->arma_actual < 0) return; /* enemigo desarmado */

    /* DEBUG:
       EJECUTAR LOS SONIDOS DEL DISPARO  */
    tocar_sonido_paneado(fixtoi(ene->x),
                         arma_ene[ene->arma_actual].snd[0],
                         rand_ex(128, 200),
                         rand_ex(900, 1100));


    switch (arma_ene[ene->arma_actual].t)
    {
     case 0:
     case 1:
     case 4:
           c = 1;
     break;

     case 5:
           c = 5;
     break;

     case 2:
           c = 3;
      break;

     case 3:
           c = 2;
      break;

     case 6:
        c = 4;
     break;

     case 7:
        c = rand_ex(20, 30);
     break;
  	   
     case 8:
        c = 20;
     break;
     
     case 9:
     	c = rand_ex(20, 30);
     break;
     
     case 10:
     	c = rand_ex(5, 30);
     break;
     case 11:
     	c = rand_ex(4, 8);
     break;
     
     default:
     /* DEBUG, que paso? */
       c = -1; /* no agregar nada! */
     break;
    }

 /* Si el tipo de disparo es doble, o triple, realizar el proceso varias veces */
 for (i=0; i<c; i++)
  {
    DISP_ENE *nueva = malloc(sizeof(DISP_ENE));
    nueva->next = disp_ene_1;
    disp_ene_1  = nueva;

    if (nueva != NULL) /* si el malloc funciono, seteo los datos... */
    {

     /* Para evitar problemas, lleno todos los valores con defaults primero... */
     /* La 'y' es comun a todos los tipos de disparos */
     nueva->x = fixadd(ene->x, itofix(enem_t[ene->ene_t].spr[ene->spr_actual]->w/2));
     nueva->y = fixadd(ene->y, itofix(enem_t[ene->ene_t].spr[ene->spr_actual]->h/2));
     nueva->dx = arma_ene[ene->arma_actual].vx;
     nueva->dy = arma_ene[ene->arma_actual].vy;
     nueva->arma = ene->arma_actual;
     nueva->vida = arma_ene[nueva->arma].vida;

     /* Dependiendo del tipo de disparo, setear su posicion */
     switch (arma_ene[nueva->arma].t)
     {
       case 0: /* recto */
           nueva->x = fixadd(ene->x, itofix(rand() % enem_t[ene->ene_t].spr[ene->spr_actual]->w));
       break;

       case 1: /* direccion al azar */
       case 6:
       case 7:
           nueva->x = fixadd(ene->x, itofix(rand() % enem_t[ene->ene_t].spr[ene->spr_actual]->w));
           nueva->dx = ftofix((float)rand_ex(-fixtof(arma_ene[ene->arma_actual].vx)*10.0, fixtof(arma_ene[ene->arma_actual].vx)*10.0) / 10.0);
           if (rand()%100 < 10)  nueva->dx = 0;

           nueva->dy = fmul(arma_ene[ene->arma_actual].vy, itofix(rand_ex(1, 3)) );
       break;

       case 2: /* abanico triple */
           nueva->x = fixadd(ene->x, itofix(rand() % enem_t[ene->ene_t].spr[ene->spr_actual]->w));
           if (i == 0)  nueva->dx = arma_ene[ene->arma_actual].vx;
           if (i == 1)  nueva->dx = 0;
           if (i == 2)  nueva->dx = fixmul(arma_ene[ene->arma_actual].vx, itofix(-1)) ;
           nueva->dy = arma_ene[ene->arma_actual].vy;
       break;

       case 3: /* doble recto */
           nueva->x = fixadd(ene->x, itofix(rand() % enem_t[ene->ene_t].spr[ene->spr_actual]->w));
           nueva->dx = arma_ene[ene->arma_actual].vx;
           nueva->dy = arma_ene[ene->arma_actual].vy;
       break;

       case 4: /* rastreador y en un 10% al azar */
       case 5:
       case 8:
           nueva->x = fixadd(ene->x, itofix(rand()% enem_t[ene->ene_t].spr[ene->spr_actual]->w));

           if (jugador.x < ene->x ) // rastrear
            nueva->dx = ftofix((float)rand_ex(-150, 0)/10.0);
           else
            nueva->dx = ftofix((float)rand_ex(0, 150)/10.0);

           nueva->dy = arma_ene[ene->arma_actual].vy;

           if (rand()%100 < 15) // cada tanto, al azar
              {
               nueva->dx = ftofix((float)rand_ex(-150, 150) / 10.0);
               nueva->dy = fmul(arma_ene[ene->arma_actual].vy, itofix(rand_ex(1, 3)) );
              }

       break;
       
       case 9: // en todas direcciones
       case 10:
       case 11:
       	   nueva->x = fixadd(ene->x, itofix(rand() % enem_t[ene->ene_t].spr[ene->spr_actual]->w));

       	 if (rand()%100 < 50)
       	 	nueva->dx = ftofix((float)rand_ex(-fixtoi(arma_ene[ene->arma_actual].vx)*10, 0)/10.0);
       	 else  
		nueva->dx = ftofix((float)rand_ex(fixtoi(arma_ene[ene->arma_actual].vx)*10,  0)/10.0);		
       	   
       	 if (rand()%100 < 50)
       	 	nueva->dy = ftofix((float)rand_ex(-fixtoi(arma_ene[ene->arma_actual].vy)*10, -fixtoi(arma_ene[ene->arma_actual].vy)*5 )/10.0);
       	 else  
		nueva->dy = ftofix((float)rand_ex(fixtoi(arma_ene[ene->arma_actual].vy)*10, fixtoi(arma_ene[ene->arma_actual].vy)*5)/10.0);
       break;
	
       default: /* que paso? */
           nueva->x = fixadd(ene->x, itofix(rand() % enem_t[ene->ene_t].spr[ene->spr_actual]->w));
           nueva->dx = ftofix((float)rand_ex(-150, 150)/10.0);
           nueva->dy = arma_ene[ene->arma_actual].vy;
       break;

     } // fin switch
    } // fin != null
  } // fin for
};

/*
  Esta funcion actualiza la logica de los disparos
  y los comprueba contra el jugador
  ademas de remover los que tienen vida < 0
  Precisa saber el desplazamiento del mapa (fy)
  si la bomba especial del jugador esta activa, elimina los disparos!
*/
void mover_disparos_ene(int fy) {
    DISP_ENE **tmp_p = &disp_ene_1;
    int i1; /* auxiliar para agregar estelas a los disparos */

    while (*tmp_p) {

        DISP_ENE *tmp = *tmp_p;

        /* aqui muevo */
        tmp->x = fixadd(tmp->x, tmp->dx);
        tmp->y = fixadd(tmp->y, tmp->dy);
        (tmp->vida)--; /* DEBUG: esto es correcto?, teoricamente, si... */

        if (bomba_esta_activa) tmp->vida = -1; // matar disparo si esta la bomba

        /* verificar si disparo CHOCA CON jugador  */
       
          if (check_pmask_collision(arma_ene[tmp->arma].mask, jugador.mask,
                                    fixtoi(tmp->x), fixtoi(tmp->y),
                                    fixtoi(jugador.x), fixtoi(jugador.y) ) != 0)
                                    {

            /* choco al jugador! SONIDO de colision del arma */
                    tocar_sonido_paneado(fixtoi(tmp->x),
                                         arma_ene[tmp->arma].snd[1],
                                         rand_ex(200,255),
                                         rand_ex(900,1100));

            tmp->vida = -1;

            jugador.vida -= arma_ene[tmp->arma].punch;

            if (nivel_detalle > 0) poner_explosion_nave(fixtoi(tmp->x), fixtoi(tmp->y), rand()%20+20, rand()%10+5+arma_ene[tmp->arma].punch*2, 0);
          }
          else /* si no choco al jugador, agregar estela de particulas - DEBUG */
          {
            /* DEBUG: agregar estela en particulas, [eliminar en bajos detalles!]
                      esto podria ser una funcion aparte
                      porque es bastante complicada la sintaxis... */
            if (nivel_detalle > 7)
                for (i1=0;
                     i1 < rand_ex(arma_ene[tmp->arma].est_cant[0], arma_ene[tmp->arma].est_cant[1]);
                     i1++)
                   agrega_particula( fixadd(tmp->x, itofix(arma_ene[tmp->arma].spr->w/2)),
                                     fixadd(tmp->y, itofix(rand()%arma_ene[tmp->arma].spr->h)),
                                     ftofix(rand_ex(arma_ene[tmp->arma].est_dx[0], arma_ene[tmp->arma].est_dx[1])/100.0),
                                     ftofix(rand_ex(arma_ene[tmp->arma].est_dy[0], arma_ene[tmp->arma].est_dy[1])/100.0),
                                     rand_ex(arma_ene[tmp->arma].est_vida[0], arma_ene[tmp->arma].est_vida[1]),
                                     makecol(rand_ex(arma_ene[tmp->arma].est_color[0][0], arma_ene[tmp->arma].est_color[0][1]),
                                             rand_ex(arma_ene[tmp->arma].est_color[1][0], arma_ene[tmp->arma].est_color[1][1]),
                                             rand_ex(arma_ene[tmp->arma].est_color[2][0], arma_ene[tmp->arma].est_color[2][1])),
                                     rand_ex(arma_ene[tmp->arma].est_rad[0], arma_ene[tmp->arma].est_rad[1]),
                                     rand_ex(arma_ene[tmp->arma].est_tipo[0], arma_ene[tmp->arma].est_tipo[1]),
                                     arma_ene[tmp->arma].est_transp,0,
                                     NULL );
          };

        if (fixtoi(tmp->y) > fy + ALTO_FB ) tmp->vida = -1; /* se fue por abajo... */
        if (fixtoi(tmp->y) < fy ) tmp->vida = -1; /* se fue por arriba... */

        /* Remocion de disparos muertos */
        if (tmp->vida < 0) {
          /* murio, eliminar!!! */
          *tmp_p = tmp->next;
          free(tmp);
        } else {
            tmp_p = &tmp->next; /* siguiente por favor! */
        }
    }
}

/* Dibuja los disparos, desplazados por x,y
   sobre el bitmap bmp */
void dibujar_disp_ene(BITMAP *bmp, int x, int y) {

    DISP_ENE *tmp = disp_ene_1;

    while (tmp) {
      draw_sprite(bmp, arma_ene[tmp->arma].spr, fixtoi(tmp->x)-x, fixtoi(tmp->y)-y);
      
        /* DEBUG: mostrar energia */
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
void liberar_lista_disparos_ene() {
    DISP_ENE *tmp = disp_ene_1;
    disp_ene_1 = NULL;

    while (tmp) {
        DISP_ENE *next = tmp->next;
        free(tmp);
        tmp = next;
    }
}

#endif
