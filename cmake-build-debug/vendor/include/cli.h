//
// Created by lux on 8/16/23.
//

#ifndef COMPILATION_PHASE_CLI_H
#define COMPILATION_PHASE_CLI_H
typedef enum {
    RED = 31,
    GREEN = 32,
    YELLOW = 33,
    BLUE = 34,
    MAGENTA = 35,
    CYAN = 36,
    WHITE = 37,
    RESET = 0
} Color;


void printColorText(char * format, void *content , Color color);
void printColorTextLn(char * format, void *content, Color color);

#endif //COMPILATION_PHASE_CLI_H
