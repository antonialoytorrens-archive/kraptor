/* guiprocs.c - Kraptor
   Esto contiene dialogos tipo 3-D para el GUI de Kraptor
   Fueron tomados de Allegro Dialog Editor
   y modificados para su uso en Kraptor
   Ademas, se modifico ciertos graficos, y se agregaron slider,
   y otros controles 'chetos' (ver el .h)
   
   This has the dialogs for the GUI of Kraptor.
   They where originally made by Julien Cugniere,
   This source code is altered by Kronoman for the use in Kraptor.
*/

/* ORIGINAL:
 * Allegro DIALOG Editor
 * by Julien Cugniere
 *
 * guiprocs.c : Some thin, 3d-looking GUI procs
 *
 * Unless otherwise specified, d->bg is ignored.
 */
#include "guiprocs.h"
#include <allegro/internal/aintern.h>

#define SET_DIALOG(d, _p, _x, _y, _w, _h, _fg, _bg, _k, _f, _d1, _d2, _dp, _dp2, _dp3) {\
    d.proc = _p; d.x = _x; d.y = _y; d.w = _w; d.h = _h; d.fg = _fg; d.bg = _bg;        \
    d.key = _k; d.flags = _f; d.d1 = _d1; d.d2 = _d2; d.dp = _dp; d.dp2 = _dp2;         \
    d.dp3 = _dp3; }



/* additionnal colors of the 3d GUI */
int gui_text_color;
int gui_white_color;
int gui_light_color;
int gui_back_color;
int gui_dark_color;
int gui_black_color;

/* default color set for the GUI
Modificado por Kronoman para que ademas, inicie totalmente
el GUI de Allegro para funcionar como este GUI
*/
void xset_gui_colors(void)
{
    gui_text_color     = makecol(0, 0, 0);
    gui_back_color     = makecol(208, 208, 208);
    gui_white_color    = makecol(245, 245, 245);
    gui_light_color    = makecol(223, 223, 223);
    gui_dark_color     = makecol(187, 187, 187);
    gui_black_color    = makecol(156, 156, 156);

    gui_fg_color            = makecol(0, 0, 0);
    gui_mg_color            = makecol(187, 187, 187);
    gui_bg_color            = makecol(208, 208, 208);

    gui_font_baseline       = 0;
    gui_mouse_focus         = 1;
    gui_shadow_box_proc     = xbox_proc;
    gui_ctext_proc          = xctext_proc;
    gui_button_proc         = xbutton_proc;
    gui_edit_proc           = xedit_proc;
    gui_list_proc           = xlist_proc;
    gui_text_list_proc      = xtext_list_proc;
    gui_menu_draw_menu      = xdraw_menu;
    gui_menu_draw_menu_item = xdraw_menu_item;
}



/***************************************************************************/
/********************************* Helpers *********************************/
/***************************************************************************/



/* draws a gui box */
void gui_rect(BITMAP *bmp, int x, int y, int w, int h, int flags)
{
    int c1, c2, c3, c4;
    if(flags & F_IN)
    {
   c1 = gui_black_color;
   c2 = gui_dark_color;
   c3 = (flags & F_LIGHT) ? gui_white_color : gui_light_color;
   c4 = gui_white_color;
    }
    else
    {
   c1 = gui_white_color;
   c2 = (flags & F_LIGHT) ? gui_white_color : gui_light_color;
   c3 = gui_dark_color;
   c4 = gui_black_color;
    }

    putpixel(bmp, x, y, c1);
    putpixel(bmp, x+w-1, y, gui_back_color);
    putpixel(bmp, x, y+h-1, gui_back_color);
    putpixel(bmp, x+w-1, y+h-1, c4);
    hline(bmp, x+1, y, x+w-2, c2);
    vline(bmp, x, y+1, y+h-2, c2);
    hline(bmp, x+1, y+h-1, x+w-2, c3);
    vline(bmp, x+w-1, y+1, y+h-2, c3);
}



/* directly ripped from Allegro. Maybe this function should be made
 * available in aintern.h ?
 */
void dotted_rect(BITMAP *bmp, int x1, int y1, int x2, int y2, int fg, int bg)
{
    int x = ((x1+y1) & 1) ? 1 : 0;
    int i;

    for (i=x1; i<=x2; i++)
    {
   putpixel(bmp, i, y1, (((i+y1) & 1) == x) ? fg : bg);
   putpixel(bmp, i, y2, (((i+y2) & 1) == x) ? fg : bg);
    }

    for (i=y1+1; i<y2; i++)
    {
   putpixel(bmp, x1, i, (((i+x1) & 1) == x) ? fg : bg);
   putpixel(bmp, x2, i, (((i+x2) & 1) == x) ? fg : bg);
    }
}



/* 3d-looking menus */
void xdraw_menu(int x, int y, int w, int h)
{
    gui_rect(screen, x, y, w, h, 0);
}



/* 3d-looking menu item (ripped from AGUP)
*/
void xdraw_menu_item(MENU *m, int x, int y, int w, int h, int bar, int sel)
{
    int fg, bg;
    int i, j;
    char buf[512], *tok;
    
    if(m->flags & D_DISABLED)
    {
   fg = gui_dark_color;
   bg = gui_back_color;
    }
    else
    {
   fg = gui_text_color;
   bg = (sel) ? gui_light_color : gui_back_color;
    }

    rectfill(screen, x, y, x+w-1, y+h-1, bg);
    text_mode(bg);

    if(ugetc(m->text))
    {
   i = 0;
   j = ugetc(m->text);

   while((j) && (j != '\t'))
   {
       i += usetc(buf+i, j);
       j = ugetc(m->text+i);
   }
   usetc(buf+i, 0);

   gui_textout(screen, buf, x+8, y+1, fg, FALSE);

   if (j == '\t')
   {
       tok = m->text+i + uwidth(m->text+i);
       gui_textout(screen, tok, x+w-gui_strlen(tok)-10, y+1, fg, FALSE);
   }

   if((m->child) && (!bar))
   {
       vline(screen, x+w-11, y+1, y+text_height(font), (sel) ? gui_white_color : gui_light_color);
       line(screen, x+w-10, y+1, x+w-3, y+(text_height(font)/2), (sel) ? gui_white_color : gui_light_color);
       line(screen, x+w-10, y+text_height(font), x+w-3, y+(text_height(font)/2)+1, gui_dark_color);
   }
    }
    else
    {
   hline(screen, x, y+text_height(font)/2+1, x+w-1, gui_dark_color);
   hline(screen, x, y+text_height(font)/2+2, x+w-1, gui_light_color);
   putpixel(screen, x-1, y+text_height(font)/2+1, gui_back_color);
   putpixel(screen, x-1, y+text_height(font)/2+2, gui_white_color);
   putpixel(screen, x+w, y+text_height(font)/2+1, gui_black_color);
   putpixel(screen, x+w, y+text_height(font)/2+2, gui_back_color);
    }

    if(m->flags & D_SELECTED)
    {
   line(screen, x+1, y+text_height(font)/2+1, x+3, y+text_height(font)+1, fg);
   line(screen, x+3, y+text_height(font)+1, x+6, y+2, fg);
    }
}



/* draws a 3d-looking scrollable frame */
static void xdraw_scrollable_frame(DIALOG *d, int listsize, int offset, int height, int fg_color)
{
    int i, len;
    int xx, yy;

    /* possibly draw scrollbar */
    if(listsize > height)
    {
   int focus = (d->flags & D_GOTFOCUS);
   xx  = d->x+d->w-12;
   yy  = d->y;
   i   = ((d->h-4) * offset + listsize/2) / listsize;
   len = ((d->h-4) * height + listsize/2) / listsize;

   gui_rect(screen, d->x, d->y, d->w-12, d->h, F_IN);
   gui_rect(screen, xx, yy, 12, d->h, F_IN);
   rect(screen, xx+1, yy+1, xx+12-2, yy+d->h-2, gui_white_color);

        dotted_rect(screen, d->x+1, d->y+1, d->x+d->w-12-2, d->y+d->h-2,
       gui_white_color, (focus) ? gui_back_color : gui_white_color);

   if(i > 0)
       rectfill(screen, xx+2, yy+2, xx+2+8, yy+2+i-1, gui_white_color);

   gui_rect(screen, xx+2, yy+2+i, 8, len, (focus) ? F_LIGHT : 0);
   rectfill(screen, (xx+2)+1, (yy+2+i)+1, (xx+2+8-1)-1, (yy+2+i+len-1)-1,
          (focus) ? gui_light_color : gui_back_color);

   if(i+len < d->h-4)
       rectfill(screen, xx+2, yy+2+i+len, xx+2+8, yy+d->h-3, gui_white_color);
    }
    else
    {
   gui_rect(screen, d->x, d->y, d->w, d->h, F_IN);

   dotted_rect(screen, d->x+1, d->y+1, d->x+d->w-2, d->y+d->h-2,
       gui_white_color, (d->flags & D_GOTFOCUS) ? gui_back_color : gui_white_color);
    }
}



/* draws a 3d-looking list (ripped from AGUP) */
static void xdraw_list(DIALOG *d)
{
    typedef char *(*getfuncptr)(int, int*);
    int height, listsize, i, len, bar, x, y, w;
    int fg, bg;
    char *sel = d->dp2;
    char s[512];

    (*(getfuncptr)d->dp)(-1, &listsize);
    height = (d->h-4) / text_height(font);
    bar = (listsize > height);
    w = (bar ? d->w-15 : d->w-3);
    fg = (d->flags & D_DISABLED) ? gui_dark_color : gui_text_color;

    /* draw box contents */
    for(i=0; i<height; i++)
    {
   if(d->d2+i < listsize)
   {
       if((sel) && (sel[d->d2+i]))
      bg = gui_light_color;
       else
      bg = gui_white_color;
       usetc(s, 0);
       ustrncat(s, (*(getfuncptr)d->dp)(i+d->d2, NULL), sizeof(s)-ucwidth(0));
       x = d->x + 2;
       y = d->y + 2 + i*text_height(font);
       text_mode(bg);
       rectfill(screen, x, y, x+7, y+text_height(font)-1, bg);
       x += 8;
       len = ustrlen(s);
       while (text_length(font, s) >= MAX(d->w - 1 - (bar ? 22 : 10), 1))
       {
      len--;
      usetat(s, len, 0);
       }
       textout(screen, font, s, x, y, fg);
       x += text_length(font, s);
       if(x <= d->x+w)
      rectfill(screen, x, y, d->x+w, y+text_height(font)-1, bg);
   }
   else
       rectfill(screen, d->x+2,  d->y+2+i*text_height(font),
           d->x+w, d->y+1+(i+1)*text_height(font), gui_white_color);
    }

    if(d->y+2+i*text_height(font) <= d->y+d->h-3)
   rectfill(screen, d->x+2, d->y+2+i*text_height(font),
       d->x+w, d->y+d->h-3, gui_white_color);

    /* draw frame, maybe with scrollbar */
    xdraw_scrollable_frame(d, listsize, d->d2, height, 0);

    /* draw the focus item */
    if(d->d1 < d->d2+height && d->d1 >= d->d2)
    {
   y = d->y + 2 + (d->d1-d->d2)*text_height(font);

   dotted_rect(screen, d->x+2, y-1, d->x+w, y+text_height(font),
       gui_dark_color, gui_white_color);
    }
}



/*
simple text proc, aligns the text depending on d->d1:
 *  0 - left aligned
 *  1 - centered on d->x
 *  2 - centered on the middle of the object
 *  3 - right aligned
 */
static void xdraw_text(BITMAP *bmp, DIALOG *d, int align)
{
    FONT *oldfont = font;
    int rtm;
    int fg = (d->flags & D_DISABLED) ? gui_dark_color : d->fg;
    int x, center;
    
    switch(align)
    {
   case 1:  x = d->x;                        center = TRUE;  break;
   case 2:  x = d->x + d->w/2;               center = TRUE;  break;
   case 3:  x = d->x+d->w-gui_strlen(d->dp); center = FALSE; break;
   default: x = d->x;                        center = FALSE; break;
    }
    if(d->dp2)
   font = d->dp2;
    rtm = text_mode(gui_back_color);
    gui_textout(screen, d->dp, x, d->y, fg, center);
    text_mode(rtm);
    font = oldfont;
}



/***************************************************************************/
/************************* 3d-looking GUI objects **************************/
/***************************************************************************/



/* simple text proc, aligns the text depending on d->d1 (see xdraw_text)*/
int xtext_proc(int msg, DIALOG *d, int c)
{
    if(msg == MSG_DRAW)
   xdraw_text(screen, d, d->d1);
    return D_O_K;
}



/* replacement for allegro's d_ctext_proc */
int xctext_proc(int msg, DIALOG *d, int c)
{
    if(msg == MSG_DRAW)
   xdraw_text(screen, d, 1);
    return D_O_K;
}



/* replacement for allegro's d_rtext_proc */
int xrtext_proc(int msg, DIALOG *d, int c)
{
    if(msg == MSG_DRAW)
   xdraw_text(screen, d, 3);
    return D_O_K;
}



/* 3d-looking list proc */
int xlist_proc(int msg, DIALOG *d, int c)
{
    if(msg == MSG_DRAW)
    {
   xdraw_list(d);
   return D_O_K;
    }
    return d_list_proc(msg, d, c);
}



/* 3d-looking text list proc */
int xtext_list_proc(int msg, DIALOG *d, int c)
{
    if(msg == MSG_DRAW)
    {
   xdraw_list(d);
   return D_O_K;
    }
    return d_text_list_proc(msg, d, c);
}



/* 3d-looking textbox proc (ripped from AGUP) */
int xtextbox_proc(int msg, DIALOG *d, int c)
{
    if(msg == MSG_DRAW)
    {
   int height, bar;
   int fg_color = (d->flags & D_DISABLED) ? gui_dark_color : gui_text_color;
   height = (d->h-8) / text_height(font);

   /* tell the object to sort of draw, but only calculate the listsize */
   _draw_textbox(d->dp, &d->d1, 
            0, /* DONT DRAW anything */
            d->d2, !(d->flags & D_SELECTED), 8,
            d->x, d->y, d->w, d->h,
            (d->flags & D_DISABLED),
            0, 0, 0);

   if(d->d1 > height)
       bar = 11;
   else
       bar = d->d2 = 0;

   /* now do the actual drawing */
   _draw_textbox(d->dp, &d->d1, 1, d->d2,
            !(d->flags & D_SELECTED), 8,
            d->x, d->y, d->w-bar, d->h,
            (d->flags & D_DISABLED),
            fg_color, gui_white_color, gui_dark_color);

   /* draw the frame around */
   xdraw_scrollable_frame(d, d->d1, d->d2, height, 0);
   return D_O_K;
    }
    
    return d_textbox_proc(msg, d, c);
}



/* a 3d-looking box proc. set d1 to 1 to draw it inward */
int xbox_proc(int msg, DIALOG *d, int c)
{
    if(msg == MSG_DRAW)
    {
   gui_rect(screen, d->x, d->y, d->w, d->h, d->d1);
   rectfill(screen, d->x+1, d->y+1, d->x+d->w-2, d->y+d->h-2, gui_back_color);
    }
    return D_O_K;
}



/* a 3d-box displaying the color in d->bg */
int xcolorbox_proc(int msg, DIALOG *d, int c)
{
    if(msg == MSG_DRAW)
    {
   gui_rect(screen, d->x, d->y, d->w, d->h, F_IN);
   rectfill(screen, d->x+1, d->y+1, d->x+d->w-2, d->y+d->h-2, d->bg);
    }
    return D_O_K;
}



/* a 3d-looking check proc. d1 ignored
*/
int xcheck_proc(int msg, DIALOG *d, int c)
{
    d->d1 = 1;
    if(msg == MSG_DRAW)
    {
   int dis = d->flags & D_DISABLED;
   int fg = (dis) ? gui_dark_color : gui_text_color;
   int bg = (dis) ? gui_light_color : gui_white_color;
//   int sg = (dis) ? gui_black_color : gui_back_color;
   gui_rect(screen, d->x, d->y, d->h, d->h, F_IN);
   rectfill(screen, d->x+1, d->y+1, d->x+d->h-2, d->y+d->h-2, bg);

   if(d->flags & D_SELECTED)
   {
   /* modificado por kronoman para hacer un 'look' mas lindo al check box */

     rectfill(screen, d->x+3, d->y+3, d->x + d->h - 4, d->y + d->h - 4, (d->flags & D_GOTFOCUS) ? gui_light_color : gui_back_color);
     gui_rect(screen, d->x+2, d->y+2, d->h-4, d->h-4, (d->flags & D_GOTFOCUS) ? F_LIGHT : 0 );


//     line(screen, d->x+3, d->y+3, d->x+d->h-4, d->y+d->h-4, gui_light_color);
//     line(screen, d->x+3, d->y+d->h-4, d->x+d->h-4, d->y+3, gui_light_color);

   }

//   if(d->flags & D_GOTFOCUS)
//       dotted_rect(screen, d->x+1, d->y+1, d->x+d->h-2, d->y+d->h-2, sg, bg);

   text_mode(gui_back_color);
   gui_textout(screen, d->dp, d->x+d->h-1+text_height(font)/2, d->y+(d->h-(text_height(font)-gui_font_baseline))/2, fg, FALSE);
   return D_O_K;
    }
    else
   return d_check_proc(msg, d, c);
}



/* xbutton_proc
 *  a modified button proc that call a function instead of closing the dialog
 *  dp2 should contain the function: int func()
 */
int xbutton_proc(int msg, DIALOG *d, int c)
{
    int ret;
    typedef int (*func_ptr)(void);

    if(msg == MSG_DRAW)
    {
   int p = (d->flags & D_SELECTED) ? 1 : 0;
   int style = ((p) ? F_IN : 0) | ((d->flags & D_GOTFOCUS) ? F_LIGHT : 0);
   gui_rect(screen, d->x, d->y, d->w, d->h, style);
   rectfill(screen, d->x+1, d->y+1, d->x+d->w-2, d->y+d->h-2, (d->flags & D_GOTFOCUS) ? gui_light_color : gui_back_color);

   text_mode(-1);
   gui_textout(screen, d->dp, d->x+p + d->w/2, d->y+p + (d->h-text_height(font))/2, (d->flags & D_DISABLED) ? gui_dark_color : gui_text_color, 1);
   
   return D_O_K;
    }
    else
   ret = d_button_proc(msg, d, c);

    if(ret == D_CLOSE && d->dp2)
   ret = ((func_ptr)d->dp2)() | D_REDRAWME;

    return ret;
}



/* xedit_proc
 *  a tricky little mix of a proc, that behave like a d_edit_proc object, but
 *  can have a button at its right that calls a function. The function is in
 *  dp2 in the form int func(DIALOG*), and is passed the dialog calling it.
 *  Its return value is passed back to the dialog manager. PageDown simulates
 *  a click on the button.
 */
int xedit_proc(int msg, DIALOG *d, int c)
{
    int ret = D_O_K;
    typedef int (*func_ptr)(DIALOG*);
    int n = d->h-text_height(font);
    DIALOG edit, button;

    SET_DIALOG(edit,   d_edit_proc,  d->x+n/2, d->y+n/2, d->w-n, d->h-n, d->fg, gui_white_color, 0, d->flags,        d->d1, d->d2, d->dp, NULL, NULL);
    SET_DIALOG(button, xbutton_proc, 0,        d->y+1,   d->h-2, d->h-2, d->fg, gui_back_color,  0, d->flags|D_EXIT, 0,     0,     "...", NULL, NULL);

    if(d->dp2)
    {
   button.x = d->x + d->w - button.w - 1;
   edit.w -= button.w;
/*
  _default_font seems to have dissappeared in Allegro 4.2
  According to the original author, function xedit_proc is not used anywhere in the code, so commenting out
  these lines won't hurt.
  Miriam Ruiz <little_miry@yahoo.es>, January 2006
*/

/*
   if(font == &_default_font)
       button.dp = "_";
*/
    }
    switch(msg)
    {
    case MSG_START:

   /* hack to make this proc look better when used in file_select() */
   if(d->h < text_height(font)+4)
   {
       d->y -= (text_height(font)+4 - d->h)/2;
       d->h = text_height(font)+4;
   }

    case MSG_CLICK:

   if(d->dp2 && mouse_x >= button.x)
   {
       ret = button.proc(msg, &button, c);
       if(ret & D_CLOSE)
      ret = ((func_ptr)d->dp2)(d) | D_REDRAWME;
   }
   else
       ret = edit.proc(msg, &edit, c);

   break;

    case MSG_DRAW:

   gui_rect(screen, d->x, d->y, d->w, d->h, F_IN);
   rectfill(screen, d->x+1, d->y+1, (d->dp2) ? (button.x-1) : (d->x+d->w-2), d->y+d->h-2, gui_white_color);
   ret = edit.proc(msg, &edit, c);
   if(d->dp2)
       ret |= button.proc(msg, &button, c);
   break;

    case MSG_CHAR:

   if(d->dp2 && (c>>8) == KEY_PGDN)
       ret = ((func_ptr)d->dp2)(d) | D_REDRAWME;
   /* fall through */

    default:

   ret |= edit.proc(msg, &edit, c);
   break;
    }

    d->d1 = edit.d1;
    d->d2 = edit.d2;
    return ret;
}



/* xpalette_proc
 *  displays the current palette, and holds the index of the selected color
 *  in d1. w and h are ignored: the object is sized 130x130. Supports D_EXIT,
 *  allowing to return D_CLOSE on double clicks.
 */
int xpalette_proc(int msg, DIALOG* d, int c)
{
    int x, y, ret = D_O_K;

    switch(msg)
    {
    case MSG_START:

   d->w = d->h = 130;
   break;

    case MSG_DRAW:
   
   for(x=0; x<16; x++)
       for(y=0; y<16; y++)
      rectfill(screen, d->x+1 + x*8, d->y+1 + y*8, d->x+8 + x*8, d->y+8 + y*8, palette_color[x+16*y]);

   if(d->d1 >= 0)
   {
       x = d->x+1 + 8*(d->d1%16);
       y = d->y+1 + 8*(d->d1/16);
       rect(screen, x,   y,   x+8, y+8, gui_back_color);
       rect(screen, x-1, y-1, x+7, y+7, gui_text_color);
   }
   gui_rect(screen, d->x, d->y, d->w, d->h, F_IN);
   break;

    case MSG_DCLICK:

   ret = D_CLOSE;
   /* fall through */

    case MSG_CLICK:

   x = (mouse_x - (d->x+1))/8;
   y = (mouse_y - (d->y+1))/8;
   c = x + y*16;

   if(c < 256 && c != d->d1)
   {
       d->d1 = c;
       return D_REDRAWME | D_WANTFOCUS;
   }
   break;

    case MSG_WANTFOCUS:
   ret = D_WANTFOCUS;
    }

    return ret;
}

/* Slider cool, tipo X11, por Kronoman,
basado en el slider original, pero mas bonito :^) */
int xslider_proc(int msg, DIALOG* d, int c)
{
   int vert = TRUE;        /* flag: is slider vertical? */
   int slx, sly, slh, slw;
   int hh = 20;            /* handle height (width for horizontal sliders) */
   int slp;                /* slider position */
   int irange;
   fixed slratio, slmax, slpos;

   if (d->h < d->w)
      vert = FALSE;

   irange = (vert) ? d->h : d->w;
   slmax = itofix(irange-hh);
   slratio = slmax / (d->d1);
   slpos = slratio * d->d2;
   slp = fixtoi(slpos);


    if(msg == MSG_DRAW)
    {
      /* dibujar */
        rectfill(screen, d->x, d->y, d->x+d->w-2, d->y+d->h-2,gui_white_color);

        gui_rect(screen, d->x, d->y, d->w - 1, d->h -1, F_IN);
                                                              

     /* slider */
	 if (vert) {
           slx = d->x+1;
           sly = d->y+(d->h)-(hh+slp);
           slw = d->w-5;
           slh = hh-3;
        } else { /* horizontal */
           slx = d->x+slp;
           sly = d->y+2;
           slw = hh-3;
           slh = d->h-5;
       }
        rectfill(screen,
                 slx+3, sly+1,
                 slx+( slw-2), sly+slh-1,
                 (d->flags & D_GOTFOCUS) ? gui_light_color : gui_back_color);

        gui_rect(screen,
                 slx+2, sly,
                 slw-2, slh,
                 (d->flags & D_GOTFOCUS) ? F_LIGHT : 0 );

    }
    else
        return d_slider_proc(msg, d, c);

return D_O_K;
}

/* xbitmap_proc:
 *  Muestra el bitmap en dp ajustado a w y h
 */
int xbitmap_proc(int msg, DIALOG *d, int c)
{
   BITMAP *b = (BITMAP *)d->dp;

   if (msg==MSG_DRAW && d->dp != NULL)
     stretch_blit(b, screen,
                  0, 0, b->w, b->h,
                  d->x, d->y, d->w, d->h);

   return D_O_K;
}
