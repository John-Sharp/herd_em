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
#define POLL_TIME 0.2 /* Number of seconds that must elapse before a 
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
typedef struct jty_actor jty_actor;
typedef struct jty_actor_ls jty_actor_ls;
typedef struct jty_actor_i_ls jty_actor_i_ls;
typedef struct jty_actor_i_ls jty_actor_i_ls;
typedef struct jty_map_handle_ls jty_map_handle_ls;

typedef void (*m_handler)(jty_actor *actor, int i, int j, char tile_type);

struct jty_map{

    int w, h;          /* Size of map (tiles) */
    int tw, th;        /* Size of one tile sprite (pixels) */

    char *c_map; /* Array of collision tile types */


    SDL_Rect map_rect; /* SDL_Rect describing the position, width
                          and height of map that is visible */

    SDL_Surface *tilepalette; /* Tile palette image */

    GLuint texname;    /* Texture containing the map */

};

struct jty_actor{       /* A character in the game */
    unsigned int uid;           /* Unique identifier for actor */
    GLfloat x, y;               /* x, y coordinates of actor */
    GLfloat px, py;             /* x, y coordinates from previous frame */
    GLfloat gx, gy;             /* x, y coordinates of actor's interpolated
                                  position in fractional frame */

    double vx, vy;              /* x and y components of velocity */
    double ax, ay;              /* x and y components of acceleration */
                                   
    int w, h;                   /* Width and height of actor */
    int p2w, p2h;               /* Width and height of actor to nearest power
                                   of two (as required by openGL) */

    GLuint texture; /* Texture that is this actor's
                                     sprite */

    jty_actor_i_ls *i_ls;      /*  List of actor's iteration handlers */

    jty_map_handle_ls *m_h_ls; /* List of map collision handlers */
};

struct jty_actor_ls{         /* List of actors */
    jty_actor *actor; /* Pointer to actor */
    jty_actor_ls *next; /* Pointer to the next node of the list */
};

struct jty_actor_i_ls{      /* List of actor iterators */
    void (*i_handler)(struct jty_actor *); /* Pointer to the iteration
                                             handler */
    jty_actor_i_ls *next;
};

struct jty_map_handle_ls{ /* A list of map collision handlers */
    jty_map_handle_ls *next;
    char *tiles; /* Array of tiles that this handler should 
                    be fired for */
    m_handler map_handler;
                /* Pointer to the 
                   collision handler
                   function */
};

struct jty_eng{

#ifdef DEBUG_MODE
    int print_messages;
#endif

    double elapsed_frames;   /* Frame that the game is on */

    jty_map *map; /* Current map of the game */

    jty_actor_ls *actors; /* List of actors that are in the engine */

    SDL_Surface *screen; /* Main game window surface */

};

/* Global variable containing information about the engine */
struct jty_eng jty_engine;


/* Frees all resources allocated to 'map' */
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
        const char *cm);

/* Creates a new actor, in 'map',
 * inside the group(s) satisfying the group number
 * 'groupnum'. Actor has width 'w', height 'h' and the image
 * file 'sprite_filename' contains the sprites to be used for this actor. */
jty_actor *jty_new_actor(int w, int h, const char *sprite_filename);

/* Add a handler that will get called once each logic frame for the actor */
void jty_actor_add_i_handler(jty_actor *actor,
                             void (*i_handler)(struct jty_actor *));

/* Add a handler that will get caled each time 'actor' hits a tile in 'tiles'.
 * The tiles are given as a string in the same way as the map gets specified,
 * and the same key as was is used in the actor's primary map. */
void jty_actor_add_m_handler(jty_actor *actor,
                             m_handler map_hander,
                             char *tiles);

/* Removes the map handler */
void jty_actor_rm_m_handler(jty_actor *actor,
                            m_handler map_handler);

/* Frees all resources allocated to the engine */
void jty_eng_free(void);

/* Creates the engine */
jty_eng *jty_eng_create(unsigned int win_w, unsigned int win_h);

/* Paint everything onto the screen */
void jty_paint(void);

/* Carry out a logic frame */
void jty_iterate(void);

#endif
