/* The main jaunty engine, responsible for keeping track of 
 * levels, actors, etc... */
#include "jaunty.h"

#include <stdio.h>

/* Utility functions */

/* returns a number that is  a number, 'a', converted into the nearest number
 * that is a whole power of 2 (rounding up) */ 
#define mkp2(a) (int)powf(2.0, ceilf(logf((float)a)/logf(2.0)))

/* Utility function to get a pixel at ('x', 'y') from a surface */
Uint32 get_pixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

void printbitssimple(unsigned long n)
{
	unsigned long i;
	i = 1UL <<(sizeof(i) * CHAR_BIT - 1);

	while (i > 0) {
		if (n & i)
			fprintf(stderr, "1");
		else
			fprintf(stderr, "0");
		i >>= 1;
	}
}

void print_overlap(struct jty_overlap *overlap)
{
    fprintf(stderr, "Overlap is: a1 offset: (%f, %f)\n"
            "   a2 offset: (%f, %f)\n"
            "   overlap: (%f, %f)\n",
            overlap->x.a1_offset, overlap->y.a1_offset,
            overlap->x.a2_offset, overlap->y.a2_offset, 
            overlap->x.overlap, overlap->y.overlap);
    return;
}

void print_c_info(struct jty_c_info *c_info)
{
    fprintf(stderr, "Collision info is: normal: (%f, %f)\n"
            "  penetration: %f\n",
            c_info->normal.x, c_info->normal.y,
            c_info->penetration);
}

void print_c_shape(jty_shape *c_shape)
{
    fprintf(stderr, "C-shape: centre: (%f, %f)\n"
            "  width, height: (%f, %f)\n",
            c_shape->centre.x, c_shape->centre.y,
            c_shape->w, c_shape->h);
}

/* Calculates the linear overlap of lines going from (a1 -> a2) and (b1->b2) */
int jty_calc_overlap_l(double a1, double a2, double b1, double b2,
        struct jty_overlap_l *overlap)
{

    if(a1 < b1){
        if(a2 <= b1){
            return 0;
        }
        overlap->a1_offset = b1 - a1;
        overlap->a2_offset = 0;

        if(a2 < b2){
            overlap->overlap = a2 - b1;
        }else{
            overlap->overlap = b2 - b1;
        }

    }else{
        if(b2 <= a1){
            return 0;
        }

        overlap->a1_offset = 0;
        overlap->a2_offset = a1 - b1;

        if(b2 < a2){
            overlap->overlap = b2 - a1;
        }else{
            overlap->overlap = a2 - a1;
        }

    }
    return 1;
}

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

/* Sprite functions */

void jty_sprite_free(jty_sprite *sprite)
{
    /* Delete the textures */
    glDeleteTextures(sprite->num_of_frames, sprite->textures);
    free(sprite->textures);

    return;
}

jty_sprite *jty_sprite_create(
        int w, int h, const char *sprite_filename, jty_shape **c_shapes
        )
{
    jty_sprite *sprite;
    Uint32 colourkey;
    int max_size, xpad, ypad;
    SDL_Surface *sprite_img, *image;
    SDL_Rect dst, src;
    int i;

    if((sprite = malloc(sizeof(*sprite))) == NULL){
        fprintf(stderr, "Error allocating memory for sprite.\n");
        exit(1);
    }

    sprite->w = w;
    sprite->h = h;

    sprite->p2w = mkp2(w);
    sprite->p2h = mkp2(h);

    /* Check sprite size doesn't exceed openGL's maximum texture size */
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_size);

    if(sprite->p2w > max_size || sprite->p2h > max_size){
        fprintf(stderr, "Image size of sprite %s, (%d, %d) exceeds "
                "maximum texture size (%d)\n",
                "plcholder", sprite->p2w, sprite->p2h, max_size);
        return NULL;
    }

    /* Load the image file that will contain the sprite */
    image = IMG_Load(sprite_filename);

    if(!image){
        fprintf(stderr, "Error! Could not load %s\n", sprite_filename);
        return NULL;
    }

    sprite->num_of_frames = image->w / w;

    /* Make SDL copy the alpha channel of the image */
    SDL_SetAlpha(image, 0, SDL_ALPHA_OPAQUE);
    colourkey = SDL_MapRGBA(image->format, 0xff, 0x00, 0xff, 0);

    xpad = (sprite->p2w - sprite->w)/2;
    ypad = (sprite->p2h - sprite->h)/2;

    sprite_img = SDL_CreateRGBSurface(SDL_SWSURFACE, sprite->p2w,
            sprite->p2h, 32, RMASK, GMASK, BMASK, AMASK);
    if(!sprite_img){
        fprintf(stderr, "Error creating a surface for the sprites\n");
        jty_sprite_free(sprite);
        return NULL;
    }

    sprite->textures = malloc(sprite->num_of_frames * sizeof(*sprite->textures));
    glGenTextures(sprite->num_of_frames, sprite->textures);

    for(i=0; i < sprite->num_of_frames; i++) {
        dst.x = xpad;
        dst.y = ypad;
        dst.w = sprite->w;
        dst.h = sprite->h;
        src.w = sprite->w;
        src.h = sprite->h;
        src.x = i * sprite->w;
        src.y = 0;

        SDL_FillRect(sprite_img, NULL, colourkey);
        SDL_SetColorKey(image, SDL_SRCCOLORKEY, colourkey);
        SDL_BlitSurface(image, &src, sprite_img, &dst);

        /* Create an openGL texture and bind the sprite's image to it */
        glBindTexture(GL_TEXTURE_2D, sprite->textures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sprite->p2w, sprite->p2h,
                0, GL_RGBA, GL_UNSIGNED_BYTE, sprite_img->pixels);
    }

    SDL_FreeSurface(image);
    SDL_FreeSurface(sprite_img);

    sprite->c_shapes = c_shapes;

    return sprite;
}

/* Actor functions */

void jty_actor_free(jty_actor *actor)
{
    int i;

#ifdef DEBUG_MODE
    fprintf(stderr, "Deleting actor uid: %d\n", actor->uid);
#endif

    for(i = 0; i < actor->num_of_sprites; i++) {
        jty_sprite_free(actor->sprites[i]);
    }

    free(actor->sprites);

    /* TODO: free iteration and map handlers */

    free(actor);
    return;
}

static jty_actor *jty_actor_create()
{
    jty_actor *actor;

    if((actor = malloc(sizeof(*actor))) == NULL){
        fprintf(stderr, "Error allocating memory for actor.\n");
        exit(1);
    }

    actor->x = actor->y = actor->px = actor->py = 0;
    actor->vx = actor->vy = actor->ax = actor->ay = 0;

    actor->current_sprite = 0;
    actor->current_frame = 0;

    actor->i_ls = NULL;
    actor->m_h_ls = NULL;
    actor->a_h_ls = NULL;

    return actor;
}

jty_actor *jty_new_actor(
        unsigned int groupnum,
        int num_of_sprites,
        int w,
        int h,
        const char *sprite_filename,
        jty_shape **c_shapes,
        ...
        )
{
    jty_actor *actor;
    jty_sprite **sprites;
    static unsigned int uid = 0;
    va_list parg;
    int i;

    sprites = malloc(sizeof(*sprites) * num_of_sprites);
    sprites[0] = jty_sprite_create(w, h, sprite_filename, c_shapes);
    
    va_start(parg, c_shapes);
    for(i=1; i< num_of_sprites; i++){
        w = va_arg(parg, int);
        h = va_arg(parg, int);
        sprite_filename = va_arg(parg, char *);
        c_shapes = va_arg(parg, jty_shape **);

        sprites[i] = jty_sprite_create(w, h, sprite_filename, c_shapes);
    }

    actor = jty_actor_create();
    actor->groupnum = groupnum;
    actor->sprites = sprites;
    actor->uid = uid;
    actor->num_of_sprites = num_of_sprites;
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
    jty_sprite *curr_sprite = actor->sprites[actor->current_sprite];

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
    glBindTexture(GL_TEXTURE_2D,
            curr_sprite->textures[actor->current_frame]);

    glBegin(GL_QUADS);
    glColor3f(1.0f, 1.0f, 1.0f);

    glTexCoord2d(0, 0);
    glVertex2i(-curr_sprite->p2w/2, -curr_sprite->p2h/2);

    glTexCoord2d(0, 1);
    glVertex2i(-curr_sprite->p2w/2, curr_sprite->p2h/2);

    glTexCoord2d(1, 1);
    glVertex2i(curr_sprite->p2w/2, curr_sprite->p2h/2);

    glTexCoord2d(1, 0);
    glVertex2i(curr_sprite->p2w/2, -curr_sprite->p2h/2);

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

void jty_actor_map_tile_overlap(jty_actor *a, int i, int j, jty_overlap *overlap)
{
    jty_sprite *curr_sprite = a->sprites[a->current_sprite];

    jty_calc_overlap_l(a->x - curr_sprite->w / 2., a->x + curr_sprite->w / 2.,
            i * jty_engine.map->tw, (i + 1) * jty_engine.map->tw,
            &(overlap->x));

    jty_calc_overlap_l(a->y - curr_sprite->h / 2., a->y + curr_sprite->h / 2.,
            j * jty_engine.map->th, (j + 1) * jty_engine.map->th,
            &(overlap->y));

    return;
}

#define jty_actor_get_sprite(actor) (actor->sprites[actor->current_sprite])

jty_shape jty_actor_get_c_shape(jty_actor *actor)
{
    jty_shape *csp = jty_actor_get_sprite(actor)
        ->c_shapes[actor->current_frame];
    jty_shape c_shape = {
        .centre = {
            .x = csp->centre.x + actor->x,
            .y = csp->centre.y + actor->y
        },
        .w = csp->w,
        .h = csp->h,
        .radius = csp->radius,
        .type = csp->type
    };

    return c_shape;
}

/*jty_geometry_c_circle_aligned_rect(
        jty_vector c_centre,
        float r,
        jty_vector r_centre,
        float w,
        float h,
        jty_c_info *c_info
        )

{
    float a , b, c, d;
    struct jty_actor normal;

    a = fabs(c_centre.x - (r_centre.x + w/2));
    b = fabs(c_centre.x - (r_centre.x - w/2));
    c = fabs(circle_centre.y - (r_centre.y + h/2));
    d = fabs(circle_centre.y - (r_centre.y - h/2));

    if (a < r + w/2 && c < r + h/2) {
        normal.x = -1;
        normal.y = 0;

        c_info->normal = normal;
        c_info->penetration = a - (r + w/2);
       
    } else if (b < r + w/2) {
        normal.x = 1;
        normal.y = 0;

        c_info->normal = normal;
        c_info->penetration = b - 

    }

}
*/

/** 
 * Calculates collision info (which is placed in the struct pointed to
 * by `c_info`) for the collision between two JTY_RECT jty_shapes, `rect1`
 * and `rect2`. `v_rel` is the relative velocity of `rect1` relative to
 * `rect2` and is used for calculating whether the collision would have
 * been between the x or y sides of the rect.
 * 1 is returned if rectangles collide, 0 if they don't.
 */
int jty_rect_rect_detect(jty_shape *rect1, jty_shape *rect2, jty_vector v_rel, jty_c_info *c_info)
{
    struct jty_overlap_l overlap_x, overlap_y;
    float t_x, t_y; /* Time before current time that rectangles
                       would have first collided in the x and 
                       y directions, respectively */
    float d_x, d_y; /* Distance to travel to get out of collision */
    jty_vector normal;

    if (
            jty_calc_overlap_l(
            rect1->centre.x - rect1->w/2, rect1->centre.x + rect1->w/2,
            rect2->centre.x - rect2->w/2, rect2->centre.x + rect2->w/2,
            &overlap_x) == 0 ||
            jty_calc_overlap_l(
            rect1->centre.y - rect1->h/2, rect1->centre.y + rect1->h/2,
            rect2->centre.y - rect2->h/2, rect2->centre.y + rect2->h/2,
            &overlap_y) == 0
       )
    {

        return 0;
    }

    if (overlap_x.a1_offset == 0) {
        normal.x = -1;
        d_x = rect2->w - overlap_x.a2_offset;
        t_x = d_x / v_rel.x * normal.x;
    } else { /* a2_offset == 0 */
        normal.x = 1;
        d_x = rect1->w - overlap_x.a1_offset;
        t_x = d_x / v_rel.x * normal.x;
    }

    if (t_x < 0 || v_rel.x == 0) {
        t_x = INFINITY;
    }
    
    if (overlap_y.a1_offset == 0) {
        normal.y = -1;
        d_y = rect2->h - overlap_y.a2_offset;
        t_y = d_y / v_rel.y * normal.y;
    } else { /* a2_offset == 0 */
        normal.y = 1;
        d_y = rect1->h - overlap_y.a1_offset;
        t_y = d_y / v_rel.y * normal.y;
    }

    if (t_y < 0 || v_rel.y == 0) {
        t_y = INFINITY;
    }

    if (fabs(v_rel.x) < 0.01 && fabs(v_rel.y) < 0.01) {
        c_info->normal.x = 0;
        c_info->normal.y = 0;
        c_info->penetration = 0;
        return 1;
    }

    if (t_x < t_y) {
        c_info->normal.x = normal.x;
        c_info->normal.y = 0;
        c_info->penetration = d_x;

#ifdef DEBUG_MODE
       fprintf(stderr, "vrel: (%f, %f)\n", v_rel.x, v_rel.y);
       fprintf(stderr, "t's: (%f, %f)\n", t_x, t_y);
       struct jty_overlap overlap = {.x = overlap_x, .y = overlap_y};
       print_overlap(&overlap);
       print_c_info(c_info);
       print_c_shape(rect1);
       print_c_shape(rect2);
#endif
        return 1;
    }

    c_info->normal.x = 0;
    c_info->normal.y = normal.y;
    c_info->penetration = d_y;

#ifdef DEBUG_MODE
       fprintf(stderr, "vrel: (%f, %f)\n", v_rel.x, v_rel.y);
       fprintf(stderr, "t's: (%f, %f)\n", t_x, t_y);
       struct jty_overlap overlap = {.x = overlap_x, .y = overlap_y};
       print_overlap(&overlap);
       print_c_info(c_info);
       print_c_shape(rect1);
       print_c_shape(rect2);
#endif

    return 1;
}


jty_shape jty_new_shape_rect(float x, float y, float w, float h)
{
    jty_shape rect = {.centre = {.x = x, .y = y},
        .radius = 0,
        .w = w,
        .h = h,
        .type = JTY_RECT};

    return rect;
}

int jty_actor_map_tile_c_detect(jty_actor *actor, int i, int j, jty_c_info *c_info)
{
    jty_shape c_shape;
    jty_map *map = jty_engine.map;
    jty_shape tile_rect = jty_new_shape_rect((i + 0.5) * map->tw,
            (j + 0.5) * map->th,
            map->tw,
            map->th);
    jty_vector v_rel = {.x = actor->pvx, .y = actor->pvy};

    c_shape = jty_actor_get_c_shape(actor);

    if (c_shape.type == JTY_RECT) {
        return jty_rect_rect_detect(&c_shape, &tile_rect, v_rel, c_info);
    }
    return 0;
}

/**
 * Calls the map handler referred to in `mhl` if it should be 
 * called. The map handler should be called if the collision
 * wouldn't push `actor` onto another tile that is handled
 * by the same collision handler. This stops the actor getting
 * stuck when it moves up the side of a set of tiles that
 * are contiguous 
 */
void process_map_collision(
        jty_map_handle_ls *mhl,
        jty_actor *actor,
        int i,
        int j,
        char tile_type,
        jty_c_info *c_info)
{
    unsigned char (*c_map)[jty_engine.map->w] =
        (void *)jty_engine.map->c_map;

    if (c_info->normal.x == -1
            && i > 0 && strchr(mhl->tiles, c_map[j][i-1])) {
        return;
    }
    if (c_info->normal.x == 1 &&
            i < jty_engine.map->w - 1 && strchr(mhl->tiles, c_map[j][i+1])) {
        return;
    }
    if (c_info->normal.y == -1 &&
            j > 0 && strchr(mhl->tiles, c_map[j -1][i])) {
        return;
    }
    if (c_info->normal.y == 1 &&
            j < jty_engine.map->h - 1 && strchr(mhl->tiles, c_map[j + 1][i])) {
        return;
    }

    mhl->map_handler(actor, i, j, tile_type, c_info);
}

void jty_actor_iterate(jty_actor *actor)
{
    actor->px = actor->x;
    actor->py = actor->y;
    actor->pvx = actor->vx;
    actor->pvy = actor->vy;

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
    jty_sprite *curr_sprite = actor->sprites[actor->current_sprite];
    jty_c_info c_info;

    i_min = (actor->x - curr_sprite->w / 2.) / jty_engine.map->tw;
    i_max = (actor->x + curr_sprite->w / 2.) / jty_engine.map->tw;
    j_min = (actor->y - curr_sprite->h / 2.) / jty_engine.map->th;
    j_max = (actor->y + curr_sprite->h / 2.) / jty_engine.map->th;

#ifdef DEBUG_MODE
    if (jty_engine.print_messages) {
        fprintf(stderr, "Tiles between %d <= i <= %d"
                " and %d <= j <= %d collided with by actor %d\n",
                i_min, i_max, j_min, j_max, actor->uid);
    }
#endif

    /* Iterate through the map-handlers */
    for(j = j_min; j <= j_max; j++)
        for(i = i_min; i <= i_max; i++){
            for(mhl = actor->m_h_ls; mhl != NULL; mhl = mhl->next) {
                k = 0;
                while((tile_type = mhl->tiles[k])) {
                    if(tile_type == c_map[j][i]) {
                        if(jty_actor_map_tile_c_detect(
                                    actor,
                                    i,
                                    j,
                                    &c_info)) {
                            process_map_collision(
                                    mhl,
                                    actor,
                                    i,
                                    j,
                                    tile_type,
                                    &c_info);
                        }
                        break;
                    }
                    k++;
                }
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

jty_actor_handle_ls *jty_actor_add_a_handler_int(jty_actor *actor,
        unsigned int order,
        unsigned int groupnum,
        jty_a_handler actor_handler)
{
    jty_actor_handle_ls *hp;

    hp = malloc(sizeof(*hp));

    if(hp==NULL){
        fprintf(stderr, "Unable to allocate memory for an "
                "actor handler list node.\n");
        exit(1);
    }

    hp->groupnum = groupnum;
    hp->order = order;
    hp->actor_handler = actor_handler;
    hp->next = actor->a_h_ls;

    return hp;
}

void jty_actor_add_a_handler(jty_actor *actor,
        unsigned int order,
        unsigned int groupnum,
        jty_a_handler actor_handler)
{
    actor->a_h_ls = jty_actor_add_a_handler_int(actor,
            order,
            groupnum,
            actor_handler);
}

void jty_eng_add_a_a_handler(unsigned int groupnum1,
        unsigned int groupnum2,
        jty_a_handler actor_handler)
{
    jty_actor_ls *p_actor_ls;
    jty_actor *actor;

    for(p_actor_ls = jty_engine.actors; p_actor_ls != NULL; p_actor_ls = p_actor_ls->next) {
        actor = p_actor_ls->actor;
        if(actor->groupnum & groupnum1) {
            if(actor->groupnum & groupnum2) {
                groupnum2 |= groupnum1;
            }
            jty_actor_add_a_handler(actor, 1, groupnum2, actor_handler);
        }else if(actor->groupnum & groupnum2) {
            jty_actor_add_a_handler(actor, 2, groupnum1, actor_handler);
        }
    }
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

int jty_actor_actor_c_detect(
        jty_actor *a1,
        jty_actor *a2,
        jty_c_info *c_info)
{
    jty_vector v_rel = {
        .x = a1->pvx - a2->pvx,
        .y = a1->pvy - a2->pvy};
    jty_shape c_shape1 = jty_actor_get_c_shape(a1);
    jty_shape c_shape2 = jty_actor_get_c_shape(a2);

    if (c_shape1.type == JTY_RECT && c_shape2.type == JTY_RECT) {
        return jty_rect_rect_detect(&c_shape1, &c_shape2, v_rel, c_info);
    }

    return 0;
}

/**
 * Calls actor collision handler referred to by the actor 
 * handler list node, `ahls_ptr`, passing
 * actors pointed to by `a1` and `a2` to `ahp` in the order
 * intended by `ahp` and providing any corrections to the 
 * collision info pointed to by `c_info` as necessary
 */
void call_actor_handler(
        jty_actor_handle_ls *ahls_ptr,
        jty_actor *a1,
        jty_actor *a2,
        jty_c_info *c_info)
{
    if (ahls_ptr->order == 1) {
        ahls_ptr->actor_handler(a1, a2, c_info);
    } else if (ahls_ptr->order == 2) {
        c_info->normal.x *= -1;
        c_info->normal.y *= -1;
        ahls_ptr->actor_handler(a2, a1, c_info);
    } else {
        abort();
    }
}

void jty_iterate()
{
    jty_actor_ls *pg, *ph;
    jty_actor *a1, *a2;
    jty_actor_handle_ls *pahls;
    jty_c_info c_info;

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

//        /* Check for collision with any other actor */
        for(ph = pg->next; ph != NULL; ph = ph->next){
            a1 = pg->actor;
            a2 = ph->actor;

            if(jty_actor_actor_c_detect(a1, a2, &c_info)){
                /* Loop through first colliding actor's collision handlers
                 * to see if any of them apply to group of second actor
                 * in collision */

                for(pahls = a1->a_h_ls; pahls != NULL; pahls = pahls->next){
                    fprintf(stderr, "groupnum a1_h: %d\n", pahls->groupnum);
                    fprintf(stderr, "groupnum a2: %d\n", a2->groupnum);
                    if(pahls->groupnum & a2->groupnum) {
                        call_actor_handler(pahls, a1, a2, &c_info);
                    }
                }
            }
        }
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
