#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

void filter_by_prefix(
    const char* prefix, 
    const char** options, 
    const int options_count, 
    const char** filtered_options, 
    int* filtered_options_count
) {
    if (prefix == NULL || prefix[0] == '\0') {
        *filtered_options_count = options_count;
        for (int i = 0; i < options_count; i++) {
            filtered_options[i] = options[i];
        }
        return;
    }

    *filtered_options_count = 0;
    for (int i = 0; i < options_count; i++) {
        if (strncasecmp(options[i], prefix, strlen(prefix)) == 0) {
            filtered_options[*filtered_options_count] = options[i];
            (*filtered_options_count)++;
        }
    }
}

typedef struct {
    // int window_width;
    // int window_height;
} Config;


void setup(const Config* config){
    // Ensure window gets rendered without decorations
    SetTraceLogLevel(LOG_ERROR); 
    SetWindowState(FLAG_WINDOW_UNDECORATED|FLAG_WINDOW_HIGHDPI);

    InitWindow(600, 300, "rmenu");
    SetTargetFPS(60);
}


/*
-b     dmenu appears at the bottom of the screen.

-i     dmenu matches menu items case insensitively.

-l lines
        dmenu lists items vertically, with the given number of lines.

-m monitor
        dmenu is displayed on the monitor number supplied. Monitor numbers are starting from 0.

-p prompt
        defines the prompt to be displayed to the left of the input field.

-fn font
        defines the font or font set used.

-nb color
        defines the normal background color.  #RGB, #RRGGBB, and X color names are supported.

-nf color
        defines the normal foreground color.

-sb color
        defines the selected background color.

-sf color
        defines the selected foreground color.

-v     prints version information to stdout, then exits.

-w windowid
        embed into windowid.
*/

Config parse_args(int argc, char** argv){
    Config config;
    // config.window_width = 600;
    // config.window_height = 300;    
    return config;
}

typedef struct {
    const char** options;
    int input_count;
} Input;

Input parse_input(const Config* config){
    Input input;
    input.input_count = 6;
    input.options = (const char**) malloc(input.input_count * sizeof(const char*));
    input.options[0] = "Alpha";
    input.options[1] = "Bravo";
    input.options[2] = "Charlie";    
    input.options[3] = "Aardvark";
    input.options[4] = "Bottle";
    input.options[5] = "Calendar";
    return input;
}

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

State new_state(const Config* config, const Input* input){
    State state;
    state.active = 0;
    state.scrollIndex = 0;
    state.focus = 0;
    state.text[0] = '\0';
    state.options_count = input->input_count;
    state.options = input->options;
    state.filtered_options_count = 0;
    state.filtered_options = (const char**) malloc(input->input_count * sizeof(const char*));
    return state;
}

void run(State* state){
    while (!WindowShouldClose()){
        
        if(IsKeyPressed(KEY_DOWN)) {
            // printf("DOWN\n");
            state->active++;
            if (state->active >= state->filtered_options_count) state->active = 0;
        }

        if(IsKeyPressed(KEY_UP)) {
            // printf("UP\n");
            state->active--;
            if (state->active < 0) state->active = state->filtered_options_count - 1;
        }
        char active_str[10];
        sprintf(active_str, "Active: %d", state->active);

        filter_by_prefix(
            state->text, 
            state->options, 
            state->options_count, 
            state->filtered_options, 
            &state->filtered_options_count
        );

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

            GuiTextBox(
                (Rectangle){ 10, 10, 200, 30 }, 
                state->text, 
                30, 
                true
            );
            
            GuiListViewEx(
                (Rectangle){ 250, 40, 200, 200 }, 
                state->filtered_options, 
                state->filtered_options_count,  
                &state->scrollIndex, 
                &state->active, 
                &state->focus
            );

        EndDrawing();
    }
}

int main(int argc, char** argv)
{
    
    const Config config = parse_args(argc, argv);
    const Input input = parse_input(&config);

    setup(&config);

    State state = new_state(&config, &input);

    run(&state);    

    CloseWindow();
    return 0;
}