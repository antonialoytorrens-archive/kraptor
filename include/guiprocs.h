/* guiprocs.c - Kraptor
   Esto contiene dialogos tipo 3-D para el GUI de Allegro
   Fueron tomados de Allegro Dialog Editor
   y modificados para su uso en Kraptor
*/

/* ORIGINAL:
 * Allegro DIALOG Editor
 * by Julien Cugniere
 *
 * guiprocs.h : Some thin 3d-looking GUI procs
 */

#ifndef GUIPROCS_H
#define GUIPROCS_H

#include <allegro.h>

#define F_IN	    1
#define F_LIGHT	    2

#ifdef __cplusplus
   extern "C" {
#endif

/* colors */
extern int gui_text_color;
extern int gui_disabled_color;
extern int gui_white_color;
extern int gui_light_color;
extern int gui_back_color;
extern int gui_dark_color;
extern int gui_black_color;
void xset_gui_colors(void);

/* helpers */
void gui_rect(BITMAP *bmp, int x, int y, int w, int h, int flags);
void dotted_rect(BITMAP *bmp, int x1, int y1, int x2, int y2, int fg, int bg);

/* menus */
void xdraw_menu(int x, int y, int w, int h);
void xdraw_menu_item(MENU *m, int x, int y, int w, int h, int bar, int sel);


/* gui agregados por Kronoman: */
int xslider_proc(int msg, DIALOG* d, int c); /* slider tipo X11 */
int xbitmap_proc(int msg, DIALOG *d, int c); /* bitmap que se ajusta a w, h */

/* gui procs */
int xtext_proc     (int, DIALOG*, int);
int xctext_proc    (int, DIALOG*, int);
int xrtext_proc    (int, DIALOG*, int);
int xlist_proc     (int, DIALOG*, int);
int xtext_list_proc(int, DIALOG*, int);
int xtextbox_proc  (int, DIALOG*, int);
int xbox_proc      (int, DIALOG*, int);
int xcolorbox_proc (int, DIALOG*, int);
int xcheck_proc    (int, DIALOG*, int);
int xbutton_proc   (int, DIALOG*, int);
int xedit_proc     (int, DIALOG*, int);
int xpalette_proc  (int, DIALOG*, int);

void xset_gui_colors(void);

#ifdef __cplusplus
   }
#endif

#endif /* GUIPROCS_H */
