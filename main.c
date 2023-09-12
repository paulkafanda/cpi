#include <stdlib.h>
#include <string.h>
#include "include/utils.h"



int main(int argc, char * argv[]) {
    char * command = malloc(sizeof(char) * 20);
    char * argument = malloc(sizeof(char)*20);

    switch (argc) {
        case 2: {
            strcpy(command, argv[1]);
            if(strcmp(command, LIST_ALL) == 0)
                listAll();
            else if(strcmp(command, LIST) == 0)
                list();
            else if(strcmp(command, HELP) == 0)
                help();
            else
                unknown_command(command);
            break;
        }
        case 3: {
            strcpy(command, argv[1]);
            strcpy(argument, argv[2]);

            if(strcmp(command, INSTALL) == 0) {
                if(!install(argument))
                    package_not_found(argument);
            } else if(strcmp(command, UNINSTALL) == 0) {
                if(!uninstall(argument))
                    package_not_installed(argument);
            } else {
                unknown_command(command);
            }
            break;
        }
        case 1:
        default:
            help();
            break;
    }
    free(command);
    free(argument);
    return 0;
}