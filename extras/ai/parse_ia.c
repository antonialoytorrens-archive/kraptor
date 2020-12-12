/*----------------------
Aqui va el compilador de IA de Kraptor
Ver ia.txt para mas info.

Uso:
parse_ia [entrada.txt]  [salida.ia]

Sintaxis de entrada.txt:
[x1],[x2],[y1],[y2],[weapon],[loop]

Por Kronoman
En memoria de mi querido padre.
Copyright (c) 2003, Kronoman
---------------------*/

#include <stdio.h>
#include <string.h>

// Esta funcion hace la joda de parsear y compilar
// in, out son archivo de entrada y salida
void do_parsing(char *in, char *out)
{
FILE *fin, *fout;
int nlinea =0; // numero de linea, para debugging del programador
char linea[300]; // buffer de texto
char *ret; // para strtok
int params[6]; // contenedor de parametros, temporal
int idx; // indice para params[]
	
	fin = fopen(in, "r");
	if (fin == NULL) 
	{
       printf("ERROR:\nNo se pudo abrir %s\n\n", in);
       return;
    }
	
	fout = fopen(out, "w");
	if (fout == NULL) 
	{
       printf("ERROR:\nNo se pudo abrir %s\n\n", out);
       return;
    }

	while (fgets(linea, 256, fin) !=  NULL) // tomar linea
	{
     nlinea++;
     printf("[%04d] '%s'\n\n", nlinea, linea);
	 
     // Realizar el parsing adecuado, es decir, separar la linea en sus parametros
		
	 	ret = strtok(linea,",");
		
		idx = 0;
		while (ret != NULL)
		{
			params[idx] = atoi(ret); // convertir a entero
			printf("-> %s = %d \n", ret, params[idx]);

			ret = strtok(NULL, ",");
			idx ++;
			if (idx > 6)
			{
			  printf("ERROR!\nDemasiados parametros!\n");
			  return;
			}
		}
                        if (idx < 6)
			{
                          printf("ERROR!\nInsuficientes parametros!\n");
			  return;
			}
		
		// escribir, los parametros del bytecode (x1,x2,y1,y2,weapon,loop)
		for (idx=0; idx<6;idx++)
                     {
                     // escribo los enteros como strings, por PORTABILIDAD!
                     sprintf(linea,"%d", params[idx]);!
		     fputs(linea, fout); // como esto convierte en UTF-8, no la uso
                     
                     putc(',', fout); // separador logico
		     }
	}; 

	// listo
	fclose(fout);
	fclose(fin);
}







int main(int argc, char *argv[] )
{

	
  if (argc < 2)
  {
    printf("ERROR:\nNo especifico [entrada.txt] y [salida.ia]\n\n");
    return -1;
  }

  printf( "%s -> %s \n", argv[1], argv[2] );
  do_parsing(argv[1], argv[2]);
  

return 0;
}

