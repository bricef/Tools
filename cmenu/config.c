#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "raylib.h"
#include "args.h"

#include "cmenu.h"

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

Config* config_new(
    int monitor,
    bool bottom,
    bool center,
    const char* prompt,
    int x,
    int y,
    int width,
    int font_size,
    int padding
){
    Config* config = (Config*) malloc(sizeof(Config));
    config->monitor = monitor;
    config->bottom = bottom;
    config->center = center;
    config->prompt = prompt;
    config->x = x;
    config->y = y;
    config->height = font_size + padding * 2;
    config->width = width;
    config->font_size = font_size;
    config->padding = padding;
    return config;
}

void config_free(Config* config){
    free(config);
}

Config* config_from_args(int argc, char** argv){
    
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
    Config* config = config_new(
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


void config_print(const Config* config){
    fprintf(stderr, "Config.bottom: %d\n", config->bottom);
    fprintf(stderr, "Config.monitor: %d\n", config->monitor);
    fprintf(stderr, "Config.prompt: \"%s\"\n", config->prompt);
    fprintf(stderr, "Config.x: %d\n", config->x);
    fprintf(stderr, "Config.y: %d\n", config->y);
    fprintf(stderr, "Config.width: %d\n", config->width);
    fprintf(stderr, "Config.height: %d\n", config->height);
}


int config_get_width(const Config* config){
    return config->width ? config->width : GetMonitorWidth(config->monitor);
}

int config_get_height(const Config* config){
    return config->height ? config->height : 50;
}

int config_get_x(const Config* config){
    return config->x;
}

int config_get_y(const Config* config){
    return config->y;
}
