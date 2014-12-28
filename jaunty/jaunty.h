/* The main jaunty engine, responsible for keeping track of levels, actors,
 * etc... */

#ifndef JTY_H
#define JTY_H
#include <GL/gl.h>
#include "SDL.h"
#include <SDL_image.h>
#include <math.h>
#include <limits.h>
#include <stdarg.h>

/* 
 * width and height (in pixels per bit) of 
 * engine's collision tiles
 */
#define JTY_CTW 5
# define JTY_CTH 5

/* Width, in bits, of the 'jty_bf_t' type */
#define JTY_BFBW (CHAR_BIT * sizeof(jty_bf_t))

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
typedef struct jty_sprite jty_sprite;
typedef struct jty_actor jty_actor;
typedef struct jty_actor_ls jty_actor_ls;
typedef struct jty_actor_i_ls jty_actor_i_ls;
typedef struct jty_actor_i_ls jty_actor_i_ls;
typedef struct jty_map_handle_ls jty_map_handle_ls;

typedef void (*jty_i_handler)(struct jty_actor *actor);
typedef void (*jty_m_handler)(jty_actor *actor, int i, int j, char tile_type);

typedef struct jty_overlap_l jty_overlap_l;
typedef struct jty_overlap jty_overlap;

/* 
 * Type that is used to store the bit-fields
 *
 */
typedef unsigned long jty_bf_t;

struct jty_overlap_l{ /* Struct for describing a linear overlap */
    double a1_offset;    /* Offset of object 1 from the start of the overlap */
    double a2_offset;    /* Offset of object 2 from the start of the overlap */
    double overlap;      /* Size of the overlap */
};

struct jty_overlap{  /* Struct for describing an overlaps */
    struct jty_overlap_l x; /* Linear overlap in x-direction */
    struct jty_overlap_l y; /* Linear overlap in y-direction */
};



struct jty_map{

    int w, h;          /* Size of map (tiles) */
    int tw, th;        /* Size of one tile sprite (pixels) */

    char *c_map; /* Array of collision tile types */

    SDL_Rect map_rect; /* SDL_Rect describing the position, width
                          and height of map that is visible */

    SDL_Surface *tilepalette; /* Tile palette image */

    GLuint texname;    /* Texture containing the map */

};

struct jty_sprite{
    int w, h;                   /* Width and height of sprite */
    int p2w, p2h;               /* Width and height of sprite to nearest power
                                   of two (as required by openGL) */

    int num_of_frames;          /* Number of frames sprite has */
    GLuint *textures;           /* Pointer to texture sprites of this
                                   actor */

    jty_bf_t *c_fields;         /* Collision bitfields of sprite */
    int ba_h;                   /* Height of these bitfield arrays */
};

struct jty_actor{       /* A character in the game */
    unsigned int uid;           /* Unique identifier for actor */
    GLfloat x, y;               /* x, y coordinates of actor */
    GLfloat px, py;             /* x, y coordinates from previous frame */
    GLfloat gx, gy;             /* x, y coordinates of actor's interpolated
                                  position in fractional frame */

    double vx, vy;              /* x and y components of velocity */
    double ax, ay;              /* x and y components of acceleration */
                                   
    jty_sprite **sprites;       /* Sprites of this actor */
    int num_of_sprites;         /* Number of sprites this actor has */
    int current_sprite;         /* Current sprite that is being displayed */
    int current_frame;          /* Current frame that is being displayed */

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
    jty_m_handler map_handler;
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

/* Creates a new actor, consisting of 'num_of_sprites' sprites. The 
 * details of these
 * sprites are given by the succession of 'w', 'h', 'sprite_filename',
 * 'c_sprite_filename' arguments, giving the width, height, sprite 
 * filename and collilsion sprite filename of the individual sprites.
 */
jty_actor *jty_new_actor(
        int num_of_sprites,
        int w, int h, const char *sprite_filename, const char *c_sprite_filename,
        ...
        );

/* Add a handler that will get called once each logic frame for the actor */
void jty_actor_add_i_handler(jty_actor *actor,
                             void (*i_handler)(struct jty_actor *));

/* Add a handler that will get caled each time 'actor' hits a tile in 'tiles'.
 * The tiles are given as a string in the same way as the map gets specified,
 * and the same key as was is used in the actor's primary map. */
void jty_actor_add_m_handler(jty_actor *actor,
                             jty_m_handler map_hander,
                             char *tiles);

/* Removes the map handler */
void jty_actor_rm_m_handler(jty_actor *actor,
                            jty_m_handler map_handler);

/* Frees all resources allocated to the engine */
void jty_eng_free(void);

/* Creates the engine */
jty_eng *jty_eng_create(unsigned int win_w, unsigned int win_h);

/* Paint everything onto the screen */
void jty_paint(void);

/* Carry out a logic frame */
void jty_iterate(void);

#endif
