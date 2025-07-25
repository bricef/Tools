#include <stdio.h>
#include <stdbool.h>
#include "raylib.h"
#include "cmenu.h"

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
){
    Config config;
    config.monitor = monitor;
    config.bottom = bottom;
    config.center = center;
    config.prompt = prompt;
    config.x = x;
    config.y = y;
    config.height = font_size + padding * 2;
    config.width = width;
    config.font_size = font_size;
    config.padding = padding;
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
