#ifndef CMENU_H
#define CMENU_H

#include <stdbool.h>

typedef struct {
    bool bottom;
    bool center;
    int monitor;
    const char* prompt;
    int x;
    int y;
    int width;
    int height;
    int font_size;
    int padding;
} Config;

Config config_new(
    int monitor,
    bool bottom,
    bool center,
    const char* prompt,
    int x,
    int y,
    int width,
    int font_size,
    int padding
);

typedef struct {
    const char** options;
    int input_count;
} Input;

typedef struct {
    int active;
    int scrollIndex;
    const char** options;
    int options_count;
    int filtered_options_count;
    const char** filtered_options;
    int focus;
    char text[255];
} State;

// Function declarations
State new_state(const Config* config, const Input* input);
void state_select_next(State* state);
void state_select_previous(State* state);
void state_filter(State* state);

#endif // CMENU_H
