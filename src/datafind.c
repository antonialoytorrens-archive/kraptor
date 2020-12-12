// -------------------------------------------------------- 
// datafind.c
// Funciones de busqueda avanzada de objetos dentro de un archivo DAT
// -------------------------------------------------------- 
// Copyright (c) 2002, Kronoman
// Escrito por Kronoman - Republica Argentina
// En memoria de mi querido padre 
// -------------------------------------------------------- 
// Funcion para buscar un objeto en un datafile.
// Busca primero el verdadero, si no existe, busca por aproximacion!!!
// COOL!
// --------------------------------------------------------

#ifndef _KRONO_DATAFIND_C
#define _KRONO_DATAFIND_C

#include <allegro.h>
#include "datafind.h"

// --------------------------------------------------------
// fuzzy_find_datafile_object
// Busca un objeto por nombre aproximado en un archivo DAT
// basado en el codigo de allegro en el archivo datafile.c
// IGNORA MAYUSCULAS/MINUSCULAS
// IGNORA TIPO DE OBJETO (puede devolver cualquier tipo)
// --------------------------------------------------------
DATAFILE *fuzzy_find_datafile_object(AL_CONST DATAFILE *dat, AL_CONST char *objectname)
{
   char name[512];
   char name2[512];
   int recurse = FALSE;
   int pos, c;


   // primero ver si hay un archivo con el mismo nombre en la manera estandard
//   DATAFILE *tmp = find_datafile_object(dat, objectname); // debug
//   if (tmp != NULL) return tmp;
   

   // sacar el nombre de archivo
   pos = 0;

   while ((c = ugetxc(&objectname)) != 0) {
      if ((c == '#') || (c == '/') || (c == OTHER_PATH_SEPARATOR)) {
	 recurse = TRUE;
	 break;
      }
      pos += usetc(name+pos, c);
   }

   usetc(name+pos, 0);

   ustrupr(name); // pasar a mayusculas

   // buscar el objeto pedido  (busca nombre aproximado)
   for (pos=0; dat[pos].type != DAT_END; pos++) {
         // obtener nombre y pasarlo a mayusculas
         ustrcpy(name2, get_datafile_property(dat+pos, DAT_NAME) );
         ustrupr(name2);

      if (ustrstr( name2 , name  ) == NULL) {
      	 if (recurse) {
             if (dat[pos].type == DAT_FILE)
              return fuzzy_find_datafile_object(dat[pos].dat, objectname);
            	    else
              return NULL;
           }
          }
	 else
	    return (DATAFILE*)dat+pos; // lo encontro
   }

   // el objeto no esta... shit!
   return NULL; 
}

// --------------------------------------------------------
// find_datafile_object_type
// busca un objeto en un datafile del tipo requerido
// en type_required pasar el tipo, ejemplo: DAT_BITMAP
// --------------------------------------------------------
DATAFILE *find_datafile_object_type(AL_CONST DATAFILE *dat, AL_CONST char *objectname, int type_required)
{
   char name[512];
   int recurse = FALSE;
   int pos, c;

   // obtener el nombre de archivo
   pos = 0;

   while ((c = ugetxc(&objectname)) != 0) {
      if ((c == '#') || (c == '/') || (c == OTHER_PATH_SEPARATOR)) {
	 recurse = TRUE;
	 break;
      }
      pos += usetc(name+pos, c);
   }

   usetc(name+pos, 0);

   // buscar el objeto 
   for (pos=0; dat[pos].type != DAT_END; pos++) {
      if (ustricmp(name, get_datafile_property(dat+pos, DAT_NAME)) == 0) {
	 if (recurse) {
	    if (dat[pos].type == DAT_FILE)
           return find_datafile_object_type(dat[pos].dat, objectname, type_required);
	    else
	       return NULL;
	 }
	 else
        if (dat[pos].type == type_required) return (DATAFILE*)dat+pos; // lo encontro
      }
   }

   // no esta, shit...
   return NULL; 
}



// --------------------------------------------------------
// fuzzy_find_datafile_object_type
// Busca un objeto por nombre aproximado en un archivo DAT
// basado en el codigo de allegro en el archivo datafile.c
// IGNORA MAYUSCULAS/MINUSCULAS
// NO IGNORA TIPO DE OBJETO
// Pasar en type_required el tipo de objeto; ej: DAT_BITMAP
// --------------------------------------------------------
DATAFILE *fuzzy_find_datafile_object_type(AL_CONST DATAFILE *dat, AL_CONST char *objectname, int type_required)
{
   char name[512];
   char name2[512];
   int recurse = FALSE;
   int pos, c;


   // primero ver si hay un archivo con el mismo nombre en la manera estandard
//   DATAFILE *tmp = find_datafile_object_type(dat, objectname, type_required);
//   if (tmp != NULL) return tmp;
   

   // sacar el nombre de archivo
   pos = 0;

   while ((c = ugetxc(&objectname)) != 0) {
      if ((c == '#') || (c == '/') || (c == OTHER_PATH_SEPARATOR)) {
	 recurse = TRUE;
	 break;
      }
      pos += usetc(name+pos, c);
   }

   usetc(name+pos, 0);

   ustrupr(name); // pasar a mayusculas

   // buscar el objeto pedido  (busca nombre aproximado)
   // se fija si el objeto coincide con el tipo requerido

   for (pos=0; dat[pos].type != DAT_END; pos++) {
         // obtener nombre y pasarlo a mayusculas
         ustrcpy(name2, get_datafile_property(dat+pos, DAT_NAME) );
         ustrupr(name2);

      if (ustrstr( name2 , name  ) == NULL) {
      	 if (recurse) {
             if (dat[pos].type == DAT_FILE)
              return fuzzy_find_datafile_object_type(dat[pos].dat, objectname, type_required);
            	    else
              return NULL;
           }
          }
	 else
        if (dat[pos].type == type_required ) return (DATAFILE*)dat+pos; // lo encontro
   }

   // el objeto no esta... shit!
   return NULL; 
}


#endif
