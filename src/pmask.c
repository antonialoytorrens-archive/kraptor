#include <stdlib.h>
#include <string.h>
#include "pmask.h"

#ifdef USE_ALLEGRO
#include <allegro.h>
#endif

#ifdef main
#undef main
#endif

#ifdef USE_SDL
#include <SDL_video.h>
#include <SDL_endian.h>
#endif

#define COMPILE_TIME_ASSERT(condition) typedef char _compile_time_assert__[(condition) ? 1 : -1];

#define MAX_INTVAL(int_type) ((((unsigned int_type)(-1))-1)/2)

int get_pmask_pixel(struct PMASK *mask, int x, int y) {
	return 1 & (mask->mask[(mask->h * (x >> MASK_WORD_BITBITS)) + y] >> (x & (MASK_WORD_BITS-1)));
}
void set_pmask_pixel(struct PMASK *mask, int x, int y, int value) {
	if (value) {
		mask->mask[(mask->h * (x >> MASK_WORD_BITBITS)) + y] |= 1 << (x & (MASK_WORD_BITS-1));
	} else {
		mask->mask[(mask->h * (x >> MASK_WORD_BITBITS)) + y] &=~(1 << (x & (MASK_WORD_BITS-1)));
	}
}

void install_pmask() {
	COMPILE_TIME_ASSERT((1 << MASK_WORD_BITBITS) == MASK_WORD_BITS);
	return;
}

void init_pmask       (struct PMASK *mask, int w, int h)
{
    int words, total_words, x;

	if ((w > MAX_INTVAL(short int)) || (h > MAX_INTVAL(short int)) || (w < 0) || (h < 0)) 
	{
		mask->w = mask->h = 0;
#ifndef MASK_SINGLE_MEMORY_BLOCK
		mask->mask = NULL;
#endif
		return;
	}

	words = 1 + ((w-1) >> MASK_WORD_BITBITS);
	
	total_words = words * h;

#ifdef MASK_SINGLE_MEMORY_BLOCK

#else 
	mask->mask = (MASK_WORD_TYPE *) malloc(
		MASK_WORD_SIZE * total_words);
	if (!mask->mask) {
		mask->w = mask->h = 0;
		return;
	}
#endif

	//Now we initialize some of the fields of the structure...
	mask->w = w;
	mask->h = h;

#ifdef CLEAR_pmask
	//Now we have a proper mask structure allocated and mostly initialized, but the mask data has garbage! We have to initialize it to 0's:
	for(x=0; x < total_words; x+=1) {
		maskt->mask[x] = 0;
	}
#else
	//only clear right hand edge if CLEAR_MASK is not defined
	for(x=total_words-h; x < total_words; x+=1) {
		mask->mask[x] = 0;
	}
#endif
	return;
}

void deinit_pmask(struct PMASK *mask) {
	mask->w = 0;
	mask->h = 0;
#ifndef MASK_SINGLE_MEMORY_BLOCK
	if (mask->mask) free(mask->mask);
	mask->mask = NULL;
#endif
	return;
}

void destroy_pmask(struct PMASK *mask) {
	deinit_pmask(mask);
	free(mask);
	return;
}

PMASK *create_pmask (int w, int h) {
	struct PMASK *maskout;

#ifdef MASK_SINGLE_MEMORY_BLOCK
	int words, total_words;
	words = 1 + ((w-1) >> MASK_WORD_BITBITS);
	total_words = words * h;
	maskout = (PMASK *) malloc(
		sizeof(PMASK) + 
		MASK_WORD_SIZE * total_words );
	if(!maskout) return NULL;
#else 
	maskout = (PMASK *) malloc(sizeof(PMASK));
	if(!maskout) return NULL;
#endif

	init_pmask(maskout, w, h);

#ifndef MASK_SINGLE_MEMORY_BLOCK
	if (!maskout->mask) {
		destroy_pmask(maskout);
		return NULL;
	}
#endif

	return maskout;
}

void pmask_load_func   (struct PMASK *mask, int _x, int _y, void *surface, int trans_color, int (*func)(void*,int,int))
{
	int words, x, y, x2, w, h;
	if(!mask) return;

	w = mask->w;
	h = mask->h;

	words = 1 + ((w-1) >> MASK_WORD_BITBITS);

	//Now we have to create the bit mask array for the sprite:
	for(x=0; x < words; x+=1) {
		for(y=0; y < h; y+=1) {
			MASK_WORD_TYPE m = 0;
			for (x2=MASK_WORD_BITS-1; x2 >= 0; x2-=1) {
				int x3 = (x << MASK_WORD_BITBITS) + x2 + _x;
				m <<= 1;
				if ( x3 < w ) {
					if ( func(surface, x3, y+_y) != trans_color ) {
						m |= 1;
					}
				}
			}
			mask->mask[y+x*h] = m;
		}
	}
	return;
}

void pmask_load_pixels (struct PMASK *mask, void *pixels, int pitch, int bytes_per_pixel, int trans_color) 
{
	int words, x, y, x2, w, h;
	if(!mask) return;

	w = mask->w;
	h = mask->h;

	words = 1 + ((w-1) >> MASK_WORD_BITBITS);

	//Now we have to create the bit mask array for the sprite:
	for(x=0; x < words; x+=1) {
		for(y=0; y < h; y+=1) {
			MASK_WORD_TYPE m = 0;
			for (x2=MASK_WORD_BITS-1; x2 >= 0; x2-=1) {
				int x3 = (x << MASK_WORD_BITBITS) + x2;
				m <<= 1;
				if ( x3<w ) {
					//beware of endianness!!!!!!!!!!
					if ( memcmp(((char*)pixels) + x3 * bytes_per_pixel + y * pitch, &trans_color, bytes_per_pixel) == 0 ) {
						m |= 1;
					}
				}
			}
			mask->mask[y+x*h] = m;
		}
	}
	return;
}

#ifdef USE_ALLEGRO
void init_allegro_pmask(struct PMASK *mask, struct BITMAP *sprite) {
	pmask_load_func (mask, 0, 0, sprite, bitmap_mask_color(sprite), (int (*)(void*,int,int))getpixel);
}
PMASK *create_allegro_pmask(struct BITMAP *sprite) {
	PMASK *ret;
	ret = create_pmask(sprite->w, sprite->h);
	init_allegro_pmask(ret, sprite);
	return ret;
}
#endif

#ifdef USE_SDL
static int SDL_getpixel(void *_surface, int x, int y)
{
    int bpp = ((SDL_Surface*)_surface)->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)((SDL_Surface*)_surface)->pixels + y * ((SDL_Surface*)_surface)->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;

    case 2:
        return *(Uint16 *)p;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
        return *(Uint32 *)p;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}
void init_sdl_pmask(struct PMASK *mask, struct SDL_Surface *sprite, int trans_color) {
	pmask_load_func (mask, 0, 0, sprite, trans_color, SDL_getpixel);
}
PMASK *create_sdl_pmask(struct SDL_Surface *sprite, int trans_color) {
	PMASK *ret;
	ret = create_pmask(sprite->w, sprite->h);
	init_sdl_pmask(ret, sprite, trans_color);
	return ret;
}
#endif


int check_pmask_collision(struct PMASK *mask1, struct PMASK *mask2, int x1, int y1, int x2, int y2)
{
	int h1, h2, words1, words2, max1, max2;
	int dx1, dx2, dy1, dy2; //We will use this deltas...
	int py; //This will store the Y position...
	int maxh; //This will store the maximum height...
	int block1, block2;

	//First we do normal bounding box collision detection...
	if( !check_bb_collision(mask1, mask2, x1,y1, x2,y2) ) //If there is not bounding box collision...
		return 0; //return that there is not collision...

	if (0) { //swap 1 & 2
		int tmp;
		PMASK *mtmp;
		tmp = x1; x1 = x2; x2 = tmp;//swap x
		tmp = y1; y1 = y2; y2 = tmp;//swap y
		mtmp = mask1; mask1 = mask2; mask2 = mtmp;//swap masks
	}

	//First we need to see how much we have to shift the coordinates of the masks...
	if(x1>x2) {
		dx1=0;      //don't need to shift mask 1.
		dx2=x1-x2;  //shift mask 2 left. Why left? Because we have the mask 1 being on the right of the mask 2, so we have to move mask 2 to the left to do the proper pixel perfect collision...
	} else {
		dx1=x2-x1;  //shift mask 1 left.
		dx2=0;      //don't need to shift mask 2.
	}
	if(y1>y2) {
		dy1=0;
		dy2=y1-y2;  //we need to move this many rows up mask 2. Why up? Because we have mask 1 being down of mask 2, so we have to move mask 2 up to do the proper pixel perfect collision detection...
	} else {
		dy1=y2-y1;  //we need to move this many rows up mask 1.
		dy2=0;
	}

	block1 = dx1>>MASK_WORD_BITBITS;
	block2 = dx2>>MASK_WORD_BITBITS;
	dx1 &= MASK_WORD_BITS-1;
	dx2 &= MASK_WORD_BITS-1;

	//This will calculate the maximum height that we will reach...
	if(mask1->h-dy1 > mask2->h-dy2) {
		maxh=mask2->h-dy2;
	} else {
		maxh=mask1->h-dy1;
	}
	maxh--;

	h1 = mask1->h;
	h2 = mask2->h;
	words1 = 1 + ((mask1->w-1) >> MASK_WORD_BITBITS);
	words2 = 1 + ((mask2->w-1) >> MASK_WORD_BITBITS);
	max1 = words1 * h1;
	max2 = words2 * h2;
	block1 = block1 * h1 + dy1;
	block2 = block2 * h2 + dy2;

	while((block1<max1) && (block2<max2) ) { //search horizantolly in the outer loop
		for(py=maxh;py>=0;py--) { //Search vertically
			if( 
				(mask1->mask[py + block1] >> dx1) & 
				(mask2->mask[py + block2] >> dx2) 
				)
				return 1;
		}
		//Now we have to move to the next block...
		//we do blocks twice because of the shift
		if( (!dx1) && (!dx2) ) { //In case both masks are lined up on the x axis...
			block1 += h1;
			block2 += h2;
		} else {
			if(!dx1) {
				block2 += h2;
				dx1 = MASK_WORD_BITS - dx2;
				dx2 = 0;
			} else {
				if(!dx2) {
					block1 += h1;
					dx2 = MASK_WORD_BITS - dx1;
					dx1 = 0;
				}
			}
		}
	}
	return 0;
}

