/*
guitrans.c
Por Kronoman
Copyrigth (c) 2002, Kronoman
En memoria de mi querido padre

Este modulo se encarga de traducir estructuras
de los GUI de Allegro al idioma actual.


NOTAS:

NOTA IMPORTANTISIMA: ESTO ES MUY PELIGROSO SI TENEMOS ESTRUCTURAS
QUE USAN 'dp' PARA OTRA COSA QUE NO SEA TEXTO (Ej: d_bitmap_proc)

Los traduce PERMANENTEMENTE, por lo tanto, es conveniente
que las estructuras se creen dinamicamente en el programa original,
de esa manera, al cambiar de idioma, todo funciona OK,
caso contrario, habra fugas de memoria, porque cambian
los punteros al cambiar el idioma!

Conviene usar el ingles como idioma base al traducir.

No usar espacios extra, o simbolos como ?, -, etc porque
la cadena no sera reconocida...
*/

#include <allegro.h>

/*
Traduce el puntero 'dp' de una estructura DIALOG completa
La estructura DEBE terminar con proc = NULL (como especifica Allegro)

DEBUG!!!
NOTA IMPORTANTISIMA: ESTO ES MUY PELIGROSO SI TENEMOS ESTRUCTURAS
QUE USAN 'dp' PARA OTRA COSA QUE NO SEA TEXTO (Ej: d_bitmap_proc)

*/
void traducir_DIALOG_dp(DIALOG *d)
{
int i = 0;

while (d[i].proc != NULL)
 {
   if (d[i].dp != NULL)  d[i].dp = (void *)get_config_text((char *)d[i].dp);
   i++;
 }

}

/*
Traduce el puntero 'text' de una estructura MENU completa
La estructura DEBE terminar con text = NULL (como especifica Allegro)
Evitar usar & y otras mierdas raras...
*/
void traducir_MENU_text(MENU *d)
{
int i = 0;

while (d[i].text != NULL)
 {
   d[i].text = (void *)get_config_text((char *)d[i].text);
   i++;
 }

}
