//
// Created by lux on 8/16/23.
//

#include <stdio.h>
#include "../include/cli.h"

void printColorText(char * format, void *content, Color color) {
    printf("\033[1;%dm", color);
    printf(format, content);
    printf("\033[0m");
}

void printColorTextLn(char * format, void *content, Color color) {
    printf("\033[1;%dm", color);
    printf(format, content);
    printf("\033[0m\n");
}