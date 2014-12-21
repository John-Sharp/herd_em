/* The main jaunty engine, responsible for keeping track of 
 * levels, actors, etc... */
#include "jaunty.h"

#include <stdio.h>

/* Engine functions */

/* Sets up the video surface */
static void initialise_video(unsigned int win_w,
        unsigned int win_h)
{
    const SDL_VideoInfo *info = NULL;
    int flags = SDL_OPENGL;
    
    if(SDL_Init(SDL_INIT_VIDEO) == -1){
        fprintf(stderr, "Video initialisation failed: %s\n", SDL_GetError());
        exit(1);
    }

    info = SDL_GetVideoInfo();
    jty_engine.screen = SDL_SetVideoMode(win_w, win_h,
            info->vfmt->BitsPerPixel, flags);

    if(!jty_engine.screen){
        fprintf(stderr, "Failed to open screen!\n");
        exit(1);
    }

    /* OpenGL initialisation */
    glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glDisable(GL_DEPTH);  /* This is a 2d program, no need for depth test */

    return;
}

void jty_eng_free(void)
{
    SDL_Quit();        

    return;
}

jty_eng *jty_eng_create(unsigned int win_w, unsigned int win_h)
{
    initialise_video(win_w, win_h);

    jty_engine.map = NULL;
    return &jty_engine;
}

/* Map functions */

void jty_map_free(jty_map *map)
{
    SDL_FreeSurface(map->tilepalette);
    glDeleteTextures(1, &(map->texname));
    free(map);
    return ;
}

static int jty_map_from_string(jty_map *map,
                               const char *k,
                               const char *m)
{
    SDL_Rect dst, src;
    SDL_Surface *map_image;
    int j, i, z = 0;
    unsigned char (*map_ptr)[map->w];
    unsigned char index;

    map_ptr = malloc(sizeof(unsigned char) * map->w * map->h);
    if(!map_ptr){
        return 0;
    }

    for(j = 0; j < map->h; j++) 
        for(i = 0; i < map->w; i++){
            index = (strchr(k, m[z]) - k) / (sizeof(unsigned char));
            map_ptr[j][i] = index;
            z++;
        }

    map_image = SDL_CreateRGBSurface(SDL_SWSURFACE, map->w * map->tw, map->h * map->th, 32,
            RMASK, GMASK, BMASK, AMASK);

    if(!map_image){
        return 0;
    }

    src.w = map->tw;
    src.h = map->th;

    for(j = 0; j < map->h; j++)
        for(i = 0; i < map->w; i++){
            dst.x = i * map->tw;
            dst.y = j * map->th ;

            index = map_ptr[j][i];
            src.x = (index % (map->tilepalette->w / map->tw)) * map->tw;
            src.y = (index / (map->tilepalette->w / map->tw)) * map->th;

            SDL_BlitSurface(map->tilepalette, &src, map_image, &dst); 
        }

    /* create an openGL texture and bind the sprite's image
     * to it */
    glGenTextures(1, &(map->texname));
    glBindTexture(GL_TEXTURE_2D, map->texname);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, map_image->w,
            map_image->h, 0, GL_RGBA, GL_UNSIGNED_BYTE,
            map_image->pixels);

    free(map_ptr);
    SDL_FreeSurface(map_image);

    return 1;
}

static int jty_map_set_cmap(jty_map *map, const char *ck,
                            const char *cm)
{
    map->c_map = malloc(sizeof(*(map->c_map)) * strlen(cm));

    if(map->c_map == NULL){
        return 0;
    }

    int i, j;
    int z = 0;
    int index;
    unsigned char (*map_ptr)[map->w] = (unsigned char(*)[map->w])map->c_map;

    for(j = map->h - 1; j >= 0; j--) 
        for(i = 0; i < map->w; i++){
            index = (strchr(ck, cm[z]) - ck) / (sizeof(unsigned char)) + 1;
            map_ptr[j][i] = index;
            z++;
        }

    return 1;
}

jty_map *jty_new_map(
        int w, int h, int tw, int th,
        const char *filename, const char *k, const char *m,
        const char *ck, const char *cm)
{
    jty_map *map = malloc(sizeof(*map));
    if(!map)
        return NULL;

    map->w = w;
    map->h = h;

    map->tw = tw;
    map->th = th;

    map->map_rect.x = 0;
    map->map_rect.y = 0;
    map->map_rect.w = jty_engine.screen->w;
    map->map_rect.h = jty_engine.screen->h;

    map->tilepalette = IMG_Load(filename);
    if(!map->tilepalette){
        jty_map_free(map);
        return NULL;
    }

    if(!jty_map_from_string(map, k, m)){
        jty_map_free(map);
        return NULL;
    }

    if(!jty_map_set_cmap(map, ck, cm)){
        jty_map_free(map);
        return NULL;
    }

    return map;
}

void jty_map_paint(jty_map *map)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(map->map_rect.x, map->map_rect.y, map->map_rect.w,
             map->map_rect.h);

    glOrtho(0, map->map_rect.w, map->map_rect.h, 0, 1, -1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, map->texname);

    glBegin(GL_QUADS);

    glTexCoord2d(0.0, 0.0);
    glVertex2i(0, 0);

    glTexCoord2d(0.0, 1.0);
    glVertex2i(0, map->h * map->th);

    glTexCoord2d(1.0, 1.0);
    glVertex2i(map->w * map->tw, map->h * map->th);

    glTexCoord2d(1.0, 0.0);
    glVertex2i(map->w * map->tw, 0);

    glEnd();

    return;
}

void jty_paint(void)
{
    /* Paint map */
    jty_map_paint(jty_engine.map);

    return;
}
