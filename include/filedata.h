// -------------------------------------------------------- 
// filedata.h
// -------------------------------------------------------- 
// Copyright (c) Kronoman 
// En memoria de mi querido padre 
// -------------------------------------------------------- 
// This file is used as a wrapper for load_datafile
// I use this for reach the datafile searching in common paths
// Is specially useful for datafiles in Windows, because most 
// of the time Windows don't start the program in the executable path, 
// and the program is unable to find the datafile.
// --------------------------------------------------------
#ifndef FILEDATA_H
#define FILEDATA_H

char *where_is_the_filename(char *buffer, const char *filename);

DATAFILE *krono_load_datafile(const char *filename);
DATAFILE *krono_load_datafile_callback(const char *filename, void (*callback)(DATAFILE *d));
DATAFILE *krono_load_datafile_object(const char *filename, const char *objectname);

#endif
