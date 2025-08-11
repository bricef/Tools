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
"    -c --center            Centers the window. (default: false, overrides x and y)\n"
"    -m --monitor MONITOR   Try to display on monitor number supplied. Monitor\n"
"                           numbers are starting from 0. (default: 0)\n"
"    -p --prompt PROMPT     Prompt to be displayed to the left of the input field.\n"
"                           (default: None)\n"
"    -x POSITION            X position of the window. (default: 0)\n"
"    -y POSITION            Y position of the window. (default: 0)\n"
"    -w --width WIDTH       Width of the window. (default: screen width)\n"
"    --font-size FONT_SIZE  Font size of the prompt and input. (default: 16px)\n"
"    -v --vertical          Vertical menu. (default: false)\n" // TODO: allow setting of vlines
"    --border-width WIDTH   Border width of the menu. (default: 0)\n"
"    --persist              Don't exit when the window loses focus. (default: false)\n"
"\n"
"Behaviour:\n"
"\n"
"    The menu will be read from STDIN as newline separated strings. Order is preserved.\n"
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
"\n"
"    Pressing and holding ALT whilst running will show shortcuts.\n"
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
    int padding,
    bool vertical,
    int vlines,
    int border_width,
    bool persist
){  
    Config* config = (Config*) malloc(sizeof(Config));
    panic_if_null(config);

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
    config->vertical = vertical;
    config->vlines = 10;
    config->border_width = border_width;
    config->persist = persist;

    config->active_background = RED;
    config->active_foreground = WHITE;
    config->prompt_background = GREEN;
    config->prompt_foreground = WHITE;
    config->input_background = GRAY;
    config->input_foreground = WHITE;
    config->normal_background = BLACK;
    config->normal_foreground = WHITE;
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
    ap_add_flag(parser, "v vertical");
    ap_add_int_opt(parser, "border-width", 0);
    ap_add_flag(parser, "persist");

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
        8, // Padding
        ap_found(parser, "v"),
        10, // Number of vertical lines to draw
        ap_get_int_value(parser, "border-width"),
        ap_found(parser, "persist")
    );

    ap_free(parser);
    return config;
}


void config_print(const Config* config){
    fprintf(stderr, "Config.monitor: %d\n", config->monitor);
    fprintf(stderr, "Config.prompt: \"%s\"\n", config->prompt);
    fprintf(stderr, "Config.x: %d\n", config->x);
    fprintf(stderr, "Config.y: %d\n", config->y);
    fprintf(stderr, "Config.width: %d\n", config->width);
    fprintf(stderr, "Config.height: %d\n", config->height);
    fprintf(stderr, "Config.vertical: %d\n", config->vertical);
    fprintf(stderr, "Config.vlines: %d\n", config->vlines);
    fprintf(stderr, "Config.font_size: %d\n", config->font_size);
    fprintf(stderr, "Config.padding: %d\n", config->padding);
    fprintf(stderr, "Config.active_background: %x\n", ColorToInt(config->active_background));
    fprintf(stderr, "Config.active_foreground: %x\n", ColorToInt(config->active_foreground));
    fprintf(stderr, "Config.prompt_background: %x\n", ColorToInt(config->prompt_background));
    fprintf(stderr, "Config.prompt_foreground: %x\n", ColorToInt(config->prompt_foreground));
    fprintf(stderr, "Config.input_background: %x\n", ColorToInt(config->input_background));
    fprintf(stderr, "Config.input_foreground: %x\n", ColorToInt(config->input_foreground));
    fprintf(stderr, "Config.normal_background: %x\n", ColorToInt(config->normal_background));
    fprintf(stderr, "Config.normal_foreground: %x\n", ColorToInt(config->normal_foreground));
    fprintf(stderr, "Config.center: %d\n", config->center);
    fprintf(stderr, "Config.bottom: %d\n", config->bottom);
    fprintf(stderr, "Config.border_width: %d\n", config->border_width);
}
