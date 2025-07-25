#include "raylib.h"
#include <unistd.h>
#include "config.c"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "args.h"

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

void error(const char* message){
    fprintf(stderr, "Error: %s\n", message);
    exit(1);
}

const char* helptext = "Usage: cmenu [options]\n"
"\n"
"Options:\n"
"\n"
"    -b --bottom            Appears at the bottom of the screen. (default: false)\n"
"    -c --center            Centers the window. (default: false)\n"
"    -m --monitor MONITOR   Try to display on monitor number supplied. Monitor\n"
"                           numbers are starting from 0. (default: 0)\n"
"    -p --prompt PROMPT     Prompt to be displayed to the left of the input field.\n"
"                           (default: None)\n"
"    -x POSITION            X position of the window. (default: 0)\n"
"    -y POSITION            Y position of the window. (default: 0)\n"
"    -w --width WIDTH       Width of the window. (default: screen width)\n"
"    --font-size FONT_SIZE  Font size of the prompt and input. (default: 16px)\n"
"\n"
"Behaviour:\n"
"\n"
"    The menu will be read from STDIN as newline separated strings.\n"
"\n"
"    If the user input is a match for an option and the user has not selected an\n" 
"    option, the first matching option is printed to stdout and the program \n"
"    exits with code 0.\n"
"\n"
"    If the user input is not a match for an option, the user input is printed \n"
"    to stdout and the program exits with code 2.\n"
"\n"
"    If the user input is empty, nothing is printed and the program exits with \n"
"    code 0.\n"
"\n"
"    If the user presses ESC, the program exits with code 0.\n"
;

const char* version = "0.0.1";

Config parse_args(int argc, char** argv){
    

    // Set up parser
    ArgParser* parser = ap_new_parser();
    ap_set_helptext(parser, helptext);
    ap_set_version(parser, version);
    ap_add_flag(parser, "b bottom");
    ap_add_flag(parser, "c center");
    ap_add_int_opt(parser, "m monitor", 0);
    ap_add_str_opt(parser, "p prompt", "");
    ap_add_int_opt(parser, "x", 0);
    ap_add_int_opt(parser, "y", 0);
    ap_add_int_opt(parser, "w width", 0);
    ap_add_int_opt(parser, "font-size", 16);

    // Parse args
    ap_parse(parser, argc, argv);

    // Set config
    Config config = config_new(
        ap_get_int_value(parser, "monitor"),
        ap_found(parser, "b"),
        ap_found(parser, "c"),
        ap_get_str_value(parser, "p"),
        ap_get_int_value(parser, "x"),
        ap_get_int_value(parser, "y"),
        ap_get_int_value(parser, "w"),
        ap_get_int_value(parser, "font-size"),
        8 // Padding
    );

    ap_free(parser);
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

void position_window(const Config* config){
    // Get monitor info
    int monitor_width = GetMonitorWidth(config->monitor);
    int monitor_height = GetMonitorHeight(config->monitor);

    // Get window info
    int width = config->width ? config->width : GetMonitorWidth(config->monitor);
    int height = config->height ? config->height : 50;
    
    int y = config->y;
    int x = config->x;
    
    SetWindowSize(width, height);

    if (config->bottom && !config->center){
        y = monitor_height - config->height;
    }

    if (config->center && !config->bottom){
        x = (monitor_width / 2) - (width / 2);
        y = (monitor_height / 2) - (height / 2);
    }

    if (config->bottom && config->center){
        y = monitor_height - config->height;
        x = (monitor_width / 2) - (width / 2);
    }

    SetWindowPosition(x, y);
}

void handle_input(State* state){
    if(IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_RIGHT)) {
        // printf("DOWN\n");
        state->active++;
        if (state->active >= state->filtered_options_count) state->active = 0;
    }

    if(IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_LEFT)) {
        // printf("UP\n");
        state->active--;
        if (state->active < 0) state->active = state->filtered_options_count - 1;
    }

    if(IsKeyPressed(KEY_ENTER)) {
        if (state->text[0] == '\0'){
            exit(0);
        }

        if (state->filtered_options_count > 0){
            printf("%s\n", state->filtered_options[state->active]);
            exit(0);
        } 

        printf("%s\n", state->text);
        exit(2);
        
    }
}


void run(State* state, const Config* config){
    while (!WindowShouldClose()){
        position_window(config);
        handle_input(state);
        
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
            int offset_x = 0;
            int offset_y = 0;
            
            if(config->prompt != NULL && config->prompt[0] != '\0'){
                int prompt_width = MeasureText(config->prompt, config->font_size) + config->padding * 2;
                DrawText(
                    config->prompt,
                    offset_x + config->padding,
                    offset_y + config->padding,
                    config->font_size,
                    BLACK // TODO: Get color from config
                );
                offset_x += prompt_width;
            }

            int input_width = 200;

            // Draw input
            GuiTextBox(
                (Rectangle){ offset_x, offset_y, input_width, config->height }, 
                state->text, 
                30, 
                true
            );
            offset_x += input_width;
    
            for (int i = 0; i < state->filtered_options_count; i++){
                int text_width = MeasureText(state->filtered_options[i], config->font_size);
                int box_width = text_width + config->padding * 2;

                if (i == state->active){
                    DrawRectangle(offset_x, offset_y, box_width, config->height, RED);
                }

                DrawText(
                    state->filtered_options[i],
                    offset_x + config->padding,
                    offset_y + config->padding,
                    config->font_size,
                    BLACK // TODO: Get color from config
                );

                offset_x += box_width;
            }

            // GuiListViewEx(
            //     (Rectangle){ offset_x, offset_y, 200, 200 }, 
            //     state->filtered_options, 
            //     state->filtered_options_count,  
            //     &state->scrollIndex, 
            //     &state->active, 
            //     &state->focus
            // );

        EndDrawing();
    }
}


void setup(const Config* config){
    // Ensure window gets rendered without decorations
    SetTraceLogLevel(LOG_ERROR); 
    SetWindowState(FLAG_WINDOW_UNDECORATED|FLAG_WINDOW_HIGHDPI);

    InitWindow(config->width, config->height, "cmenu");
    SetWindowPosition(config->x, config->y);
    SetWindowMonitor(config->monitor);
    
    SetTargetFPS(60);
}

int main(int argc, char** argv)
{
    
    const Config config = parse_args(argc, argv);
    //config_print(&config);

    const Input input = parse_input(&config);

    setup(&config);

    State state = new_state(&config, &input);

    run(&state, &config);    

    CloseWindow();
    return 0;
}
