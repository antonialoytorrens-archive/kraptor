// -------------------------------------------------------- 
// filedata.c 
// -------------------------------------------------------- 
// Copyright (c) Kronoman 
// En memoria de mi querido padre 
// -------------------------------------------------------- 
// This file is used as a wrapper for load_datafile and other similar
// I use this for reach the datafile searching in common paths
// Is specially useful for datafiles in Windows, because most 
// of the time Windows don't start the program in the executable path, 
// and the program is unable to find the datafile.
// --------------------------------------------------------

#include <allegro.h>
#include <stdio.h>
#include "filedata.h"
// --------------------------------------------------------
// This checks for the filename in several places.
// Returns where the filename is located in buffer
// buffer should be a 2048 bytes char
// If the file is not found, return the filename...
// --------------------------------------------------------
char *where_is_the_filename(char *buffer, const char *filename)
{
sprintf(buffer,"/usr/share/games/kraptor/%s", filename);
return buffer;

/*
char str[2048], str2[2048]; // buffer for path making

// check in current executable path
get_executable_name(str, 2048);
replace_filename(str2, str, filename, 2048);

if (! exists(filename) )
 {
	if (exists(str2)) 
	{
		sprintf(buffer,"%s", str2);
		return buffer;
	}
	else
	{
		get_executable_name(str, 2048);
		replace_filename(str, str, "", 2048);
		if (! find_allegro_resource(str2, filename, get_extension(filename), NULL, NULL, NULL, str, 2048) )
		{
			sprintf(buffer,"%s", str2);
			return buffer;
		}
	}
 }

// default
sprintf(buffer,"%s", filename);
return buffer;
*/
}

// --------------------------------------------------------
// This functions is a wrapper of load_datafile
// Just pass a file name (without path), and this will try to seach and load it
// Will return a pointer to the DATAFILE or NULL if error.
// --------------------------------------------------------
DATAFILE *krono_load_datafile(const char *filename)
{
char str[2048];

return load_datafile(where_is_the_filename(str, filename)); // return what we found...
}


DATAFILE *krono_load_datafile_callback(const char *filename, void (*callback)(DATAFILE *d))
{
char str[2048];

return load_datafile_callback(where_is_the_filename(str, filename), callback); // return what we found...
}

DATAFILE *krono_load_datafile_object(const char *filename, const char *objectname)
{
char str[2048];

return load_datafile_object(where_is_the_filename(str, filename), objectname); // return what we found...
}
