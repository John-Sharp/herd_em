/* The main jaunty engine, responsible for keeping track of 
 * levels, actors, etc... */
#include "jaunty.h"

#include <stdio.h>

/* Utility functions */

/* returns a number that is  a number, 'a', converted into the nearest number
 * that is a whole power of 2 (rounding up) */ 
#define mkp2(a) (int)powf(2.0, ceilf(logf((float)a)/logf(2.0)))


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



jty_eng *jty_eng_create(unsigned int win_w, unsigned int win_h)
{
    initialise_video(win_w, win_h);

    jty_engine.map = NULL;
    return &jty_engine;
}

/* Map functions */

void jty_map_free(jty_map *map)
{

#ifdef DEBUG_MODE
    fprintf(stderr, "Deleting map\n");
#endif

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

static int jty_map_set_cmap(jty_map *map, const char *cm)
{
    map->c_map = malloc(sizeof(*(map->c_map)) * map->w * map->h);
    memcpy(map->c_map, cm, sizeof(*(map->c_map)) * map->w * map->h);

    if(map->c_map == NULL){
        return 0;
    }

    return 1;
}

jty_map *jty_new_map(
        int w, int h, int tw, int th,
        const char *filename, const char *k, const char *m,
        const char *cm)
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

    if(!jty_map_set_cmap(map, cm)){
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

jty_actor_ls *jty_actor_ls_add(jty_actor_ls *ls, jty_actor *actor)
{
    jty_actor_ls *next;
    next = ls;

    if((ls = malloc(sizeof(*(ls)))) == NULL){
                    fprintf(stderr, "Error allocating list node\n");
                    exit(1);
    }

    ls->next = next;
    ls->actor = actor;
    return ls;
}

/* Actor functions */

void jty_actor_free(jty_actor *actor)
{
#ifdef DEBUG_MODE
    fprintf(stderr, "Deleting actor uid: %d\n", actor->uid);
#endif

    /* Delete the texture */
    if(glIsTexture(actor->texture))
        glDeleteTextures(1, &(actor->texture));

    free(actor);
    return;
}

static jty_actor *jty_actor_create(int w, int h, const char *sprite_filename)
{
    jty_actor *actor;
    Uint32 colourkey;
    int max_size, xpad, ypad; 
    SDL_Surface *sprite, *image;
    SDL_Rect dst, src;

    if((actor = malloc(sizeof(*actor))) == NULL){
        fprintf(stderr, "Error allocating memory for actor.\n");
        exit(1);
    }

    actor->x = actor->y = actor->px = actor->py = 0;
    actor->vx = actor->vy = actor->ax = actor->ay = 0;

    actor->w = w;
    actor->h = h;

    actor->p2w = mkp2(w);
    actor->p2h = mkp2(h);

    actor->i_ls = NULL;
    actor->m_h_ls = NULL;

    /* Check sprite size doesn't exceed openGL's maximum texture size */
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_size);

    if(actor->p2w > max_size || actor->p2h > max_size){
        fprintf(stderr, "Image size (%d, %d) exceeds "
                "maximum texture size (%d)\n",
                actor->p2w, actor->p2h, max_size);
        return NULL;
    }

    /* Load the image file that will contain the sprite */
    image = IMG_Load(sprite_filename);
    if(!image){
        fprintf(stderr, "Error! Could not load %s\n", sprite_filename);
        return NULL;
    }

    /* Make SDL copy the alpha channel of the image */
    SDL_SetAlpha(image, 0, SDL_ALPHA_OPAQUE);
    colourkey = SDL_MapRGBA(image->format, 0xff, 0x00, 0xff, 0);

    xpad = (actor->p2w - actor->w)/2;
    ypad = (actor->p2h - actor->h)/2;

    sprite = SDL_CreateRGBSurface(SDL_SWSURFACE, actor->p2w,
            actor->p2h, 32, RMASK, GMASK, BMASK, AMASK);
    if(!sprite){
        fprintf(stderr, "Error creating a surface for the sprites\n");
        jty_actor_free(actor);
        return NULL;
    }

    dst.x = xpad;
    dst.y = ypad;
    dst.w = actor->w;
    dst.h = actor->h;
    src.w = actor->w;
    src.h = actor->h;
    src.x = 0;
    src.y = 0;

    SDL_FillRect(sprite, NULL, colourkey);
    SDL_SetColorKey(image, SDL_SRCCOLORKEY, colourkey);
    SDL_BlitSurface(image, &src, sprite, &dst);

    /* Create an openGL texture and bind the sprite's image to it */
    glGenTextures(1, &(actor->texture));
    glBindTexture(GL_TEXTURE_2D, actor->texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, actor->p2w, actor->p2h,
            0, GL_RGBA, GL_UNSIGNED_BYTE, sprite->pixels);

    SDL_FreeSurface(image);
    SDL_FreeSurface(sprite);

    return actor;
}

jty_actor *jty_new_actor(int w, int h, const char *sprite_filename)
{
    jty_actor *actor;
    static unsigned int uid = 0;

    actor = jty_actor_create(w, h, sprite_filename);

    actor->uid = uid;
    uid++;

#ifdef DEBUG_MODE
    fprintf(stderr, "\nCreating actor %d\n", actor->uid);
#endif

    /* Put the actor in the map's actor list */
    jty_engine.actors = jty_actor_ls_add(jty_engine.actors, actor);

#ifdef DEBUG_MODE
    jty_actor_ls *q;

    fprintf(stderr, "List of engine's actors: ");
    for(q=jty_engine.actors; q!=NULL; q=q->next){
        fprintf(stderr, "%d, ", q->actor->uid);
    }
    fprintf(stderr, "\n");
#endif

    return actor;
}

/* Paint the 'actor' */
static int jty_actor_paint(jty_actor *actor)
{
    double frame = jty_engine.elapsed_frames;
    double fframe = frame - floor(frame);  /* fframe holds what fraction we
                                              through the current frame */

    /* Calculating the point where the actor should be drawn */
    actor->gx = fframe * actor->x + (1 - fframe) * actor->px;
    actor->gy = fframe * actor->y + (1 - fframe) * actor->py;

    /* Paint the actor's texture in the right place */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    /* Load the actor's texture */
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    /* Draw the actor */

    /* translating the actor's matrix to the point where the 
     * the actor should be drawn */
    glTranslatef(actor->gx, actor->gy, 0);
    glBindTexture(GL_TEXTURE_2D, actor->texture);

    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);

    glTexCoord2d(0, 0);
    glVertex2i(-actor->p2w/2, -actor->p2h/2);

    glTexCoord2d(0, 1);
    glVertex2i(-actor->p2w/2, actor->p2h/2);

    glTexCoord2d(1, 1);
    glVertex2i(actor->p2w/2, actor->p2h/2);

    glTexCoord2d(1, 0);
    glVertex2i(actor->p2w/2, -actor->p2h/2);


    glEnd();

    glDisable(GL_BLEND);

    return 1;
}

void jty_actor_add_i_handler(jty_actor *actor,
                             void (*i_handler)(struct jty_actor *))
{
    jty_actor_i_ls *ls;
    jty_actor_i_ls *next;
    next = actor->i_ls;

    if((ls = malloc(sizeof(*(ls)))) == NULL){
        fprintf(stderr, "Error allocating list node\n");
        exit(1);
    }

    ls->next = next;
    ls->i_handler = i_handler;

    actor->i_ls = ls;
    return;
}

void jty_actor_iterate(jty_actor *actor)
{
    actor->px = actor->x;
    actor->py = actor->y;

    actor->x += actor->vx;
    actor->y += actor->vy;

    actor->vx += actor->ax;
    actor->vy += actor->ay;

    jty_actor_i_ls *pg;
    for(pg = actor->i_ls; pg != NULL; pg = pg->next){
        pg->i_handler(actor);
    }

    /* 
     * Find which tiles are colliding with
     * this actor's bounding box 
     *
     */
    int i_min, i_max, j_min, j_max;
    int i, j, k;
    jty_map_handle_ls *mhl;
    char tile_type;
    unsigned char (*c_map)[jty_engine.map->w] =
        (void *)jty_engine.map->c_map;

    i_min = (actor->x - actor->w / 2.) / jty_engine.map->tw;
    i_max = (actor->x + actor->w / 2.) / jty_engine.map->tw;
    j_min = (actor->y - actor->h / 2.) / jty_engine.map->th;
    j_max = (actor->y + actor->h / 2.) / jty_engine.map->th;

#ifdef DEBUG_MODE
    if (jty_engine.print_messages) {
        fprintf(stderr, "Tiles between %d <= i <= %d"
                " and %d <= j <= %d collided with by actor %d\n",
                i_min, i_max, j_min, j_max, actor->uid);
    }
#endif

    /* Iterate through the map-handlers */
    for(j = j_min; j <= j_max; j++)
        for(i = i_min; i <= i_max; i++)
            for(mhl = actor->m_h_ls; mhl != NULL; mhl = mhl->next) {
                k = 0;
                while((tile_type = mhl->tiles[k])) {
                    if(tile_type == c_map[j][i])
                        mhl->map_handler(actor, i, j, tile_type);
                    k++;
                }
            }

    return;
}

static jty_map_handle_ls *jty_actor_add_m_handler_int(jty_actor *actor,
                                              jty_m_handler map_handler,
                                              char *tiles)
{
    jty_map_handle_ls *hp;
    char *tiles_copy;

    hp = malloc(sizeof(*hp));

    if(hp == NULL){
        fprintf(stderr, "Unable to allocate memory for a "
                "list handler node.\n");
        exit(1);
    }

    tiles_copy = malloc(sizeof(*tiles_copy) * (strlen(tiles) + 1));
    if(tiles_copy == NULL) {
        fprintf(stderr, "Unable to alocate memory for map "
                "handler tiles copy.\n");
    }
                
    strcpy(tiles_copy, tiles);

    hp->tiles = tiles_copy;
    hp->map_handler = map_handler;
    hp->next = actor->m_h_ls;

#ifdef DEBUG_MODE
    fprintf(stderr, "\nCreating map handler for actor %d\n"
                    "tiles %s", actor->uid, tiles_copy);
#endif

    return hp;
}

void jty_actor_add_m_handler(jty_actor *actor,
                             jty_m_handler map_handler,
                             char *tiles)
{

    actor->m_h_ls = jty_actor_add_m_handler_int(
            actor,
            map_handler,
            tiles);

    return;
}

static jty_map_handle_ls *jty_actor_rm_m_handler_int(jty_map_handle_ls *ls,
                                             jty_actor *actor,
                                             jty_m_handler map_handler)
{
    if(ls == NULL)
        return NULL;

    if(ls->map_handler == map_handler){
        jty_map_handle_ls *p = ls->next;
        free(ls->tiles);
        free(ls);
        return p;
    }

    ls->next = jty_actor_rm_m_handler_int(ls->next, actor, map_handler);
    return ls;
}

void jty_actor_rm_m_handler(jty_actor *actor,
                            jty_m_handler map_handler)
{

    actor->m_h_ls = 
        jty_actor_rm_m_handler_int(actor->m_h_ls,
            actor,
            map_handler);

    return;
}

void jty_paint(void)
{
    /* Paint map */
    jty_map_paint(jty_engine.map);

    /* Paint actors */
    jty_actor_ls *pg;
    for(pg = jty_engine.actors; pg != NULL; pg = pg->next){
        jty_actor_paint(pg->actor);
    }


    return;
}

void jty_iterate()
{
    jty_actor_ls *pg;

#ifdef DEBUG_MODE
    static double last_t = 0;
    double curr_t;

    curr_t = SDL_GetTicks();
    
    if( curr_t - last_t >= POLL_TIME * 1000){
        jty_engine.print_messages = 1;
        last_t = curr_t;
    }else
        jty_engine.print_messages = 0;

#endif

    /* Iterate each actor */
    for(pg = jty_engine.actors; pg != NULL; pg = pg->next){
        jty_actor_iterate(pg->actor);
    }

    return;
}


void jty_eng_free(void)
{
    SDL_Quit();        

    /* Free map */
    jty_map_free(jty_engine.map);

    /* Free actors */
    jty_actor_ls *pg;
    for(pg = jty_engine.actors; pg != NULL; pg = pg->next){
        jty_actor_free(pg->actor);
    }

    return;
}
