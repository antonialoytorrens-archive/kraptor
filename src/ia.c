// --------------------------------------------------------
// ia.c
// -------------------------------------------------------- 
// Copyright (c) Kronoman 
// En memoria de mi querido padre 
// -------------------------------------------------------- 
// Sistema de inteligencia artificial utizando "ejecutables"
// binarios contenidos en un DAT
// --------------------------------------------------------

#include "allegro.h"
#include "error.h"
#include "ia.h"
#include "filedata.h"

#include <stdio.h>

// Globales

// Primer nodo de la lista enlazada
static IA_NODE *first_ia_node = NULL;


// Busca en la lista enlazada de IAs la que corresponde
// al nombre pasado, y devuelve un puntero a ella, o NULL si no se encuentra
IA_NODE *buscar_lista_ia(const char *id)
{
  IA_NODE *nodo = first_ia_node;
 
 // recorrer la lista enlazada
 while (nodo)
 {
   if (ustricmp(id, nodo->id) == 0 ) return nodo; // lo encontro
   nodo = nodo->next;
 }     
    
 return NULL;   
}

// Libera la lista enlazada de la IA
void liberar_lista_ia()
{
IA_NODE *nodo = first_ia_node;
first_ia_node = NULL;
    
  while (nodo)
  {
    IA_NODE *next = nodo->next;
    free(nodo->code); // debug, dudoso...
    free(nodo);
    nodo = next;
  }
}


// Agrega un nodo de IA a la lista enlazada
// Pasarle un puntero al objeto conteniendo el script de IA (nodo nuevo)
static void agregar_nodo_ia(DATAFILE *dat)
{
	int i;
    char *p;
    char tmp[256]; // formacion de token
    int tok = 0; // posicion en token
	int params[6],idx = 0; // parametros y parametro leyendo ahora
    int bytecode_actual = 0; // bytecodes cargados
    IA_NODE *nodo = NULL; // nodo nuevo
    
    p = (char *)dat->dat; //stream de lectura y parsing
    
    //  alocar RAM para el nodo
    // nodo
    nodo = (IA_NODE *)malloc(sizeof(IA_NODE));
    if (nodo == NULL) levantar_error ("FATAL!: Sin memoria en agregar_nodo_ia()!");
    nodo->next = first_ia_node;
    first_ia_node = nodo;
    
    // bytecodes en el nodo, y propiedades extra
    nodo->code = NULL;

    sprintf(nodo->id, "%s", get_datafile_property(dat, DAT_NAME));
    tmp[0] = '\0'; tok = 0;
    
	for (i=0; i < dat->size; i++) 
 	{
		        
        // llego al token?
        if (p[i] == ',')
        {    
        tmp[tok] = '\0';    
        params[idx] =  atoi(tmp);
     

        idx++;
        tok = 0;    
            if (idx == 6) // fin del bytecode 
            {
                idx = 0;
                if (nodo->code == NULL)
                   nodo->code = malloc(sizeof(IA_BYTECODE));
                else
                   nodo->code = realloc(nodo->code, sizeof(nodo->code) + sizeof(IA_BYTECODE)*(bytecode_actual+1));
                
                // copiar info
                nodo->code[bytecode_actual].x1 = params[0];
                nodo->code[bytecode_actual].x2 = params[1];
                nodo->code[bytecode_actual].y1 = params[2];
                nodo->code[bytecode_actual].y2 = params[3];
                nodo->code[bytecode_actual].weapon = params[4];
                nodo->code[bytecode_actual].loop = params[5];
                
                bytecode_actual++; // otro bytecode mas...
            }
        }
        else
        {
         tmp[tok] = p[i];
         tok++;
        }
                   
	}
nodo->size = bytecode_actual;	
}


// Funcion que hace el cache de la IA en si
// Llamar solo al comienzo del juego, pasarle el nombre del archivo DAT a cargar
// Si falla, aborta el programa
// NOTA: la info de como funciona internamente el grabber, fue tomada de grabber.txt
void hacer_chache_ia(const char *file)
{
DATAFILE *tmp = NULL;
int pos;	
	
	tmp = krono_load_datafile(file);
	if (tmp == NULL ) levantar_error("ERROR: fallo hacer_cache_ia();");
	
	// Recorrer todo el archivo, buscando los de tipo IA
	// DAT_ID es una macro que genera el identificador type (ver grabber.txt y get_datafile_property)
	for (pos=0; tmp[pos].type != DAT_END; pos++) 
	{
        if (tmp[pos].type == DAT_ID('I','A',' ',' ')) // tiene ID de "IA  " ?
		{
         // encontro objeto en la IA, agregar nodo
			agregar_nodo_ia((DATAFILE *)tmp+pos);
        }
   }
	
 unload_datafile(tmp); 
}
