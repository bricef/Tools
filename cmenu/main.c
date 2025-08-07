#include <stdlib.h>
#include <sys/types.h>
#include "raylib.h"


#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "cmenu.h"
#include "press.h"

// #include "build/font_roboto_mono.h"
#include "build/compressed_font_roboto_mono.h"
// #include "build/font_roboto.h"
// #include "build/compressed_font_roboto.h"
// #include "build/font_pacifico.h"
// #include "build/compressed_font_pacifico.h"


Input* input_from_stdin(void){

    // TODO: Read from stdin
    Input* input = (Input*) malloc(sizeof(Input)); 
    panic_if_null(input);
    
    // Read lines from stdin into a dynamic array
    char** lines = NULL;
    size_t line_count = 0;
    size_t capacity = 16;
    
    lines = (char**) malloc(capacity * sizeof(char*));
    panic_if_null(lines);


    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    while ((read = getline(&line, &len, stdin)) != -1) {
        // Remove trailing newline if present
        if (read > 0 && line[read-1] == '\n') {
            line[read-1] = '\0';
        }

        // Resize array if needed
        if (line_count == capacity) {
            capacity *= 2;
            char** new_lines = (char**) realloc(lines, capacity * sizeof(char*));
            panic_if_null(new_lines);
            lines = new_lines;
        }

        // Copy line into array
        lines[line_count] = strdup(line);
        panic_if_null(lines[line_count]);
        line_count++;
    }

    free(line);

    // Set up input struct with read lines
    input->input_count = line_count;
    input->options = (const char**) lines;
    
    return input;
}

void input_free(Input* input){
    for (int i = 0; i < input->input_count; i++) {
        free((void *)input->options[i]);
    }
    free(input->options);
    free(input);
}


void position_window(const Config* config){
      // Get monitor info
    int monitor_width = GetMonitorWidth(config->monitor);
    int monitor_height = GetMonitorHeight(config->monitor);

    int width = config->width ? config->width : monitor_width;

    int line_height = config->font_size + config->padding * 2;

    int height;
    if (config->vertical){
        height = config->vlines * line_height;
    }else{
        height = config->height ? config->height : line_height;
    }
    
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


void select_option(int index){
    printf("SELECTING OPTION %d\n", index);
}

void handle_shortcuts(State* state){
    switch(GetKeyPressed()){
        case KEY_ONE:
        case KEY_KP_1:
            select_option(1);
            break;
        case KEY_TWO:
        case KEY_KP_2:
            select_option(2);
            break;
        case KEY_THREE:
        case KEY_KP_3:
            select_option(3);
            break;
        case KEY_FOUR:
        case KEY_KP_4:
            select_option(4);
            break;
        case KEY_FIVE:
        case KEY_KP_5:
            select_option(5);
            break;
        case KEY_SIX:
        case KEY_KP_6:
            select_option(6);
            break;
        case KEY_SEVEN:
        case KEY_KP_7:
            select_option(7);
            break;
        case KEY_EIGHT:
        case KEY_KP_8:
            select_option(8);
            break;
        case KEY_NINE:
        case KEY_KP_9:
            select_option(9);
            break;
    };
}

void handle_input(State* state){ // Could make state immutable by returning a new state

    if(IsKeyDown(KEY_LEFT_ALT) || IsKeyDown(KEY_RIGHT_ALT)){
        state->show_shortcuts = true;
        handle_shortcuts(state);
    }else{
        state->show_shortcuts = false;
    }

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


void draw_horizontal(State* state, const Config* config, Font font){
    // Draw
        BeginDrawing();
            ClearBackground(config->normal_background);
            
            int offset_x = 0;
            int offset_y = 0;
            
            if(config->prompt != NULL && config->prompt[0] != '\0'){
                int prompt_width = MeasureTextEx(font, config->prompt, config->font_size, 0.0).x + config->padding * 2;
                DrawTextEx(
                    font,
                    config->prompt,
                    (Vector2){ offset_x + config->padding, offset_y + config->padding },
                    config->font_size,
                    1,
                    config->prompt_foreground
                );
                offset_x += prompt_width;
                offset_x += config->padding;
            }

            int input_width = 200;

            // Draw input
            GuiTextBox(
                (Rectangle){ offset_x, offset_y, input_width, config->height }, 
                state->text, 
                config->font_size, 
                true
            );
            offset_x += input_width;
    
            for (int i = 0; i < state->filtered_options_count; i++){
                int text_width = MeasureText(state->filtered_options[i], config->font_size);
                int box_width = text_width + config->padding * 2;

                if (i == state->active){
                    DrawRectangle(offset_x, offset_y, box_width, config->height, config->active_background);
                    DrawTextEx(
                        font,
                        state->filtered_options[i],
                        (Vector2){ offset_x + config->padding, offset_y + config->padding },
                        config->font_size,
                        0,
                        config->active_foreground
                    );
                }else{
                    DrawTextEx(
                        font,
                        state->filtered_options[i],
                        (Vector2){ offset_x + config->padding, offset_y + config->padding },
                        config->font_size,
                        0,
                        config->normal_foreground
                    );
                }
                offset_x += box_width;
            }

            DrawRectangleLinesEx(
                (Rectangle){ 0, 0, GetScreenWidth(), GetScreenHeight() }, 
                (float) config->border_width, 
                config->normal_foreground
            );

        EndDrawing();
}

void draw_vertical(State* state, const Config* config, Font font){
    BeginDrawing();
            ClearBackground(config->normal_background);
            
            
            int offset_x = 0;
            int offset_y = 0;
            
            int prompt_height = 0;
            int prompt_width = 0;

            if(config->prompt != NULL && config->prompt[0] != '\0'){
                Vector2 prompt_size = MeasureTextEx(font, config->prompt, config->font_size, 0.0);
                prompt_height = prompt_size.y + config->padding * 2;
                prompt_width = prompt_size.x + config->padding * 2;
                DrawTextEx(
                    font,
                    config->prompt,
                    (Vector2){ offset_x + config->padding, offset_y + config->padding },
                    config->font_size,
                    1,
                    config->prompt_foreground
                );
                // offset_y += prompt_height;
                // offset_y += config->padding;
            }


            int input_width = (GetScreenWidth() - prompt_width) - (prompt_width > 0 ? config->padding : 0);


            // Draw input
            GuiTextBox(
                (Rectangle){ prompt_width + (prompt_width > 0 ? config->padding : 0), offset_y, input_width, config->height }, 
                state->text, 
                config->font_size, 
                true
            );
            offset_y += config->height;
    
            for (int i = 0; i < state->filtered_options_count; i++){
                int text_width = MeasureText(state->filtered_options[i], config->font_size);
                int box_width = text_width + config->padding * 2;
                int box_height = config->font_size + config->padding * 2;

                if (i == state->active){
                    DrawRectangle(offset_x, offset_y, box_width, box_height, config->active_background);
                    DrawTextEx(
                        font,
                        state->filtered_options[i],
                        (Vector2){ offset_x + config->padding, offset_y + config->padding },
                        config->font_size,
                        0,
                        config->active_foreground
                    );
                }else{
                    DrawTextEx(
                        font,
                        state->filtered_options[i],
                        (Vector2){ offset_x + config->padding, offset_y + config->padding },
                        config->font_size,
                        0,
                        config->normal_foreground
                    );
                }
                offset_y += box_height;
            }

            DrawRectangleLinesEx(
                (Rectangle){ 0, 0, GetScreenWidth(), GetScreenHeight() }, 
                (float) config->border_width, 
                config->normal_foreground
            );

        EndDrawing();
}


void run(State* state, const Config* config, Font font){
    while (!WindowShouldClose()){
        if (!IsWindowFocused() && !config->persist){
            break;
        }

        position_window(config);
        handle_input(state);
        state_filter(state);

        if (config->vertical){
            draw_vertical(state, config, font);
        }else{
            draw_horizontal(state, config, font);
        }
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


void set_gui_style(const Config* config, Font font){
    GuiSetFont(font);
    GuiSetStyle(DEFAULT, TEXT_SIZE, config->font_size);

    int input_fg = ColorToInt(config->input_foreground);
    int input_bg = ColorToInt(config->input_background);
    int input_border = 0xFFFFFFFF;

    GuiSetStyle(TEXTBOX, TEXT_COLOR_FOCUSED, input_fg);
    GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL, input_fg);
    GuiSetStyle(TEXTBOX, TEXT_COLOR_PRESSED, input_fg);

    GuiSetStyle(TEXTBOX, BASE_COLOR_PRESSED, input_bg);
    GuiSetStyle(TEXTBOX, BASE_COLOR_FOCUSED, input_bg);
    GuiSetStyle(TEXTBOX, BASE_COLOR_NORMAL, input_bg);

    GuiSetStyle(TEXTBOX, BORDER_COLOR_PRESSED, input_border);
    GuiSetStyle(TEXTBOX, BORDER_COLOR_FOCUSED, input_border);
    GuiSetStyle(TEXTBOX, BORDER_COLOR_NORMAL, input_border);
}


Font load_compressed_font(const unsigned char* compressed_font, int compressed_font_size){
    // Decompress font data
    int ratio = 10;

    int font_data_size_max = compressed_font_size * ratio;
    char* font_data = malloc(font_data_size_max);

    if (font_data == NULL){
        error("Failed to allocate memory for font data\n");
        exit(1);
    }

    int font_data_size = press_decompress_data (
        (const unsigned char*) compressed_font,  
        compressed_font_size, 
        font_data, 
        font_data_size_max
    );
    if (press_is_error(font_data_size)){
        fprintf(stderr, "Failed to decompress font (ERROR: %s)\n", press_get_error_name(font_data_size));
        exit(1);
    }

    // must be called after window is initialised
    Font font = LoadFontFromMemory(
        ".ttf", 
        (const unsigned char*) font_data, 
        font_data_size, 
        16, 
        0, 
        0
    );

    free(font_data);

    return font;
}



int main(int argc, char** argv)
{
    Config* config = config_from_args(argc, argv);
    Input* input = input_from_stdin();
    State* state = new_state(config, input);
    
    setup(config);
    
    // must be called after window is initialised
    Font font = load_compressed_font(compressed_font_roboto_mono, sizeof(compressed_font_roboto_mono));

    set_gui_style(config, font);

    if(!IsFontValid(font)){
        error("Failed to load font\n");
    }

    run(state, config, font);    

    UnloadFont(font);
    state_free(state);
    input_free(input);
    config_free(config);

    CloseWindow();
    return 0;
}
