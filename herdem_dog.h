#ifndef HERDEM_DOG_H
#define HERDEM_DOG_H

typedef enum herdem_dog_consts {
    HERDEM_DOG_C_SHAPE_W = 40,
    HERDEM_DOG_C_SHAPE_H = 40,
    HERDEM_DOG_NUM_FRAMES = 4
} herdem_dog_consts;

typedef struct herdem_dog herdem_dog;

struct herdem_dog {
    jty_actor actor;
    double acceleration;
    double scare_radius_min; /* radius where sheep run away with their max. speed */
    double scare_radius_max; /* maximum radius where sheep can see dog */
    double drag;
    herdem_player_action player_actions;
};

#endif
