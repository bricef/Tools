#include <stdlib.h>
#include <string.h>
#include "cmenu.h"

State* new_state(const Config* config, const Input* input){
    State* state = (State*) malloc(sizeof(State));
    panic_if_null(state);

    state->active = 0;
    state->scrollIndex = 0;
    state->focus = 0;
    state->text[0] = '\0';
    state->options_count = input->input_count;
    state->options = input->options;
    state->filtered_options_count = 0;
    state->filtered_options = (const char**) malloc(input->input_count * sizeof(const char*));
    panic_if_null(state->filtered_options);
    
    return state;
}

void state_free(State* state){
    free(state->filtered_options);
    free(state);
}

void state_select_next(State* state){
    state->active++;
    if (state->active >= state->filtered_options_count) state->active = 0;
}

void state_select_previous(State* state){
    state->active--;
    if (state->active < 0) state->active = state->filtered_options_count - 1;
}


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

void state_filter(State* state){
    filter_by_prefix(
        state->text, 
        state->options, 
        state->options_count, 
        state->filtered_options, 
        &state->filtered_options_count
    );
}
