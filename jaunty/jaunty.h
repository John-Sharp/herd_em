/* The main jaunty engine, responsible for keeping track of levels, actors,
 * etc... */

#ifndef JEN_H
#define JEN_H
#include <GL/gl.h>
#include "SDL.h"
#include <SDL_image.h>
#include <math.h>
#include <limits.h>

#define DEBUG_MODE

#ifdef DEBUG_MODE
#define POLL_TIME 2 /* Number of seconds that must elapse before a 
                       repeated debug message from an iterator 
                       is sent out again */
#endif

#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	enum { RMASK = 0xff000000, GMASK = 0x00ff0000,
        BMASK = 0x0000ff00,	AMASK = 0x000000ff};
#else
	enum { RMASK = 0x000000ff,	GMASK = 0x0000ff00,
        BMASK = 0x00ff0000,	AMASK = 0xff000000};
#endif

typedef struct jty_eng jty_eng;
typedef struct jty_map jty_map;

struct jty_map{

    int w, h;          /* Size of map (tiles) */
    int tw, th;        /* Size of one tile sprite (pixels) */

    unsigned int *c_map; /* 2D array of collision tile indicies */

    SDL_Rect map_rect; /* SDL_Rect describing the position, width
                          and height of map that is visible */

    SDL_Surface *tilepalette; /* Tile palette image */

    GLuint texname;    /* Texture containing the map */

};

struct jty_eng{

#ifdef DEBUG_MODE
    int print_messages;
#endif

    jty_map *map; /* Current map of the game */

    SDL_Surface *screen; /* Main game window surface */

};

/* Global variable containing information about the engine */
struct jty_eng jty_engine;


/* Frees all resources allocated to the engine */
void jty_eng_free(void);

/* Creates the engine */
jty_eng *jty_eng_create(unsigned int win_w, unsigned int win_h);

/* Frees all resource allocated to 'map' */
void jty_map_free(jty_map *map);

/* Creates a map, 'w' tiles wide by 'h' tiles high. The map is made out of
 * tiles 'tw' x 'th' pixels big. The constituent tile images are located in the
 * file 'filename', these tiles are referenced by letters in the key-string 'k'
 * and the map is described by the order of the letters in the map-string 'm'.
 * The collision map is specified in a similar way, but with collision key 'ck'
 * and collision map 'cm'.  Returns a pointer to this map, or NULL if the
 * allocation failed */
jty_map *jty_new_map(
        int w, int h, int tw, int th,
        const char *filename, const char *k, const char *m,
        const char *ck, const char *cm);

/* Paint everything onto the screen */
void jty_paint(void);

#endif
