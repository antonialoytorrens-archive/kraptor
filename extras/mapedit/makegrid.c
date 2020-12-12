/*
Este programa fabrica la grilla en un bmp  
  */

#include <stdio.h>
#include <stdlib.h>
#include <allegro.h>

/* Tama¤o del fondo */
#define W_FONDO  600
#define H_FONDO  4000

/* Grilla WxH */
#define W_GR 40
#define H_GR 40


int main() {
   BITMAP *bmp;
   PALETTE pal;
   int iy, ix;

   int card = GFX_AUTODETECT, w = 640 ,h = 480 ,color_depth = 8; /* agregado por Kronoman */

   allegro_init();
   install_timer();
   install_keyboard();
   install_mouse();

   set_color_depth(color_depth);
   if (set_gfx_mode(card, w, h, 0, 0)) {
      allegro_message("set_gfx_mode(%d x %d x %d bpp): %s\n", w,h, color_depth, allegro_error);
      return 1;
   }

   set_palette(default_palette);
   clear(screen);

   get_palette(pal);
   bmp = create_bitmap(W_FONDO, H_FONDO);
   clear(bmp);

   for (ix=0; ix < W_FONDO / W_GR; ix ++ )
        line(bmp, ix*W_GR, 0, ix*W_GR, H_FONDO, makecol(255,255,255));

   for (iy=0; iy < H_FONDO / H_GR; iy ++ )
        line(bmp, 0, iy*H_GR, W_FONDO, iy*H_GR, makecol(255,255,255));


   save_bitmap("grilla.pcx", bmp, pal);
   destroy_bitmap(bmp);
    
return 0;
}
END_OF_MAIN();
