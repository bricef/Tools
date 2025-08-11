
#include <stdbool.h>
#include "raylib.h"

#ifndef CMENU_H
#define CMENU_H

#define MAX_INPUT_LENGTH 255

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
    bool vertical;
    int vlines; // Number of vertical lines to draw
    int border_width;
    bool persist;
    Color active_background;
    Color active_foreground;
    Color prompt_background;
    Color prompt_foreground;
    Color input_background;
    Color input_foreground;
    Color normal_background;
    Color normal_foreground;
} Config;

Config* config_from_args(int argc, char** argv);
void config_free(Config* config);

typedef struct {
    const char** options;
    int input_count;
} Input;

Input* input_from_stdin(void);
void input_free(Input* input);

typedef struct {
    int active;
    int scrollIndex;
    const char** options;
    int options_count;
    int filtered_options_count;
    const char** filtered_options;
    int focus;
    char text[MAX_INPUT_LENGTH];
    bool show_shortcuts;
} State;

State* new_state(const Config* config, const Input* input);
void state_free(State* state);
void state_select_next(State* state);
void state_select_previous(State* state);
void state_filter(State* state);


// Utils
void error(const char* message);
void panic_if_null(void* ptr);

#endif // CMENU_H
