#ifndef LEVELS_H
#define LEVELS_H

#include <stdbool.h>

void set_up_inter_level(void (*clean_up_function)());
bool is_inter_level_finished();
void clean_up_inter_level(void (*next_level_function)());

void set_up_level_one();
bool is_level_one_finished();
void clean_up_level_one();

void set_up_inter_level_one();
void clean_up_inter_level_one();

void set_up_level_two();
bool is_level_two_finished();
void clean_up_level_two();

void set_up_inter_level_two();
void clean_up_inter_level_two();

void set_up_level_three();
bool is_level_three_finished();
void clean_up_level_three();

void set_up_inter_level_three();
void clean_up_inter_level_three();

void set_up_level_four();
bool is_level_four_finished();
void clean_up_level_four();

void set_up_inter_level_four();
void clean_up_inter_level_four();

void set_up_level_five();
bool is_level_five_finished();
void clean_up_level_five();

void set_up_inter_level_five();
void clean_up_inter_level_five();

void set_up_level_six();
bool is_level_six_finished();
void clean_up_level_six();

void set_up_inter_level_six();
void clean_up_inter_level_six();

void set_up_level_seven();
bool is_level_seven_finished();
void clean_up_level_seven();

void set_up_inter_level_seven();
void clean_up_inter_level_seven();

#endif
