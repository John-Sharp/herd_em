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
typedef struct jty_shape jty_shape; /* Generic shape */
typedef enum jty_shape_type { JTY_CIRCLE, JTY_RECT } jty_shape_type; /* Definitions of
                                              different types of shape */
typedef struct jty_map jty_map;
typedef struct jty_sprite jty_sprite;
typedef struct jty_actor jty_actor;
typedef struct jty_actor_ls jty_actor_ls;
typedef struct jty_actor_i_ls jty_actor_i_ls;
typedef struct jty_actor_i_ls jty_actor_i_ls;
typedef struct jty_map_handle_ls jty_map_handle_ls;
typedef struct jty_actor_handle_ls jty_actor_handle_ls;
typedef struct jty_c_info jty_c_info; /* Describes collision information */

typedef void (*jty_i_handler)(struct jty_actor *actor);
typedef void (*jty_m_handler)(
        jty_actor *actor,
        int i,
        int j,
        char tile_type,
        jty_c_info *c_info);
typedef void (*jty_a_handler)(
        jty_actor *actor1,
        jty_actor *actor2,
        jty_c_info *c_info);

typedef struct jty_overlap_l jty_overlap_l;
typedef struct jty_overlap jty_overlap;

void print_c_info(struct jty_c_info *c_info);

typedef struct jty_vector jty_vector; /* Geometric vector */

struct jty_overlap_l{ /* Struct for describing a linear overlap */
    double a1_offset;    /* Offset of object 1 from the start of the overlap */
    double a2_offset;    /* Offset of object 2 from the start of the overlap */
    double overlap;      /* Size of the overlap */
};

struct jty_overlap{  /* Struct for describing an overlap */
    struct jty_overlap_l x; /* Linear overlap in x-direction */
    struct jty_overlap_l y; /* Linear overlap in y-direction */
};

struct jty_vector {
    float x;
    float y;
};

struct jty_c_info {
    jty_vector normal; /* Vector describing normal of the collision */
    float penetration; /* Distance of penetration */
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

struct jty_shape {
    jty_vector centre; /* Centre of shape */
    float radius; /* If the shape is a JTY_CIRCLE can be used for storing 
                     the radius of the circle */
    float w, h;   /* Width and height of the shape */
    jty_shape_type type; /* Type of the shape */
};

/* Returns a jty_shape of type `JTY_RECT` centred at (`x`,`y`),
 * with width `w`, height `h`
 */
jty_shape jty_new_shape_rect(float x, float y, float w, float h);

struct jty_sprite {
    int w, h;                   /* Width and height of sprite */
    int p2w, p2h;               /* Width and height of sprite to nearest power
                                   of two (as required by openGL) */

    int num_of_frames;          /* Number of frames sprite has */
    GLuint *textures;           /* Pointer to texture sprites of this
                                   actor */

    jty_shape **c_shapes;         /* Collision shapes of sprite */
};

struct jty_actor{       /* A character in the game */
    unsigned int uid;           /* Unique identifier for actor */
    unsigned int groupnum;      /* Group number of the actor */

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
    jty_actor_handle_ls *a_h_ls; /* List of actor collision handlers */
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

struct jty_actor_handle_ls{
    jty_actor_handle_ls *next;
    unsigned int groupnum;      /* Group of actors that will fire when actor
                                   this list belongs to collides with */
    jty_a_handler actor_handler; /* Pointer to the actor collision handler
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
 * sprites are given by the succession of 'w', 'h', 'c_r', 'sprite_filename'
 * arguments, giving the width, height, sprite 
 * filename and collilsion shapes of the individual sprites.
 * You are responsible for freeing the 'c_shape' arrays after 
 * actor is freed.
 */
jty_actor *jty_new_actor(
        unsigned int groupnum,
        int num_of_sprites,
        int w, int h, const char *sprite_filename, jty_shape **c_shapes,
        ...
        );

/* Add a handler that will get called once each logic frame for the actor */
void jty_actor_add_i_handler(jty_actor *actor,
                             void (*i_handler)(struct jty_actor *));

/* Add a handler that will get called each time 'actor' hits a tile in 'tiles'.
 * The tiles are given as a string in the same way as the map gets specified,
 * and the same key as was is used in the actor's primary map. */
void jty_actor_add_m_handler(jty_actor *actor,
                             jty_m_handler map_hander,
                             char *tiles);

/* Removes the map handler */
void jty_actor_rm_m_handler(jty_actor *actor,
                            jty_m_handler map_handler);

/* Add a handler that will get called each time an actor in a group bitwise AND
 * matching with 'groupnum1' collides with an actor in a group bitwises AND matching
 * with 'groupnum2' */
void jty_eng_add_a_a_handler(unsigned int groupnum1,
        unsigned int groupnum2,
        jty_a_handler actor_handler);

/* Removes the actor-actor handler */
void jty_eng_rm_a_a_handler(unsigned int groupnum1,
        unsigned int groupnum2,
        jty_a_handler actor_handler);

/* Frees all resources allocated to the engine */
void jty_eng_free(void);

/* Creates the engine */
jty_eng *jty_eng_create(unsigned int win_w, unsigned int win_h);

/* Paint everything onto the screen */
void jty_paint(void);

/* Carry out a logic frame */
void jty_iterate(void);

#endif
