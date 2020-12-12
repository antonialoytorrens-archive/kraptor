// -------------------------------------------------------- 
// datafind.h
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

#ifndef _KRONO_DATAFIND_H
#define _KRONO_DATAFIND_H

DATAFILE *fuzzy_find_datafile_object(AL_CONST DATAFILE *dat, AL_CONST char *objectname);
DATAFILE *find_datafile_object_type(AL_CONST DATAFILE *dat, AL_CONST char *objectname, int type_required);
DATAFILE *fuzzy_find_datafile_object_type(AL_CONST DATAFILE *dat, AL_CONST char *objectname, int type_required);

#endif
