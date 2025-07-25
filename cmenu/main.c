#include "raylib.h"
#include <unistd.h>


#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "cmenu.h"

void error(const char* message){
    fprintf(stderr, "Error: %s\n", message);
    exit(1);
}

Input* input_from_stdin(void){
    // TODO: Read from stdin
    Input* input = (Input*) malloc(sizeof(Input));
    input->input_count = 6;
    input->options = (const char**) malloc(input->input_count * sizeof(const char*));
    input->options[0] = "Alpha";
    input->options[1] = "Bravo";
    input->options[2] = "Charlie";
    input->options[3] = "Aardvark";
    input->options[4] = "Bottle";
    input->options[5] = "Calendar";
    return input;
}

void input_free(Input* input){
    free(input->options);
    free(input);
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
        state_select_next(state);
        state->active++;
        if (state->active >= state->filtered_options_count) state->active = 0;
    }

    if(IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_LEFT)) {
        // printf("UP\n");
        state_select_previous(state);
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
        
        state_filter(state);

        // Draw
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
    Config* config = config_from_args(argc, argv);
    Input* input = input_from_stdin();
    State* state = new_state(config, input);

    setup(config);
    run(state, config);    

    state_free(state);
    input_free(input);
    config_free(config);

    CloseWindow();
    return 0;
}
