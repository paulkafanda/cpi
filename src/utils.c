//
// Created by lux on 8/28/23.
//
#include "../include/utils.h"
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include "../vendor/cli/include/cli.h"


#define PACKAGE_FOLDER "/home/lux/CLionProjects/cpi/package"
#define INCLUDE_FOLDER "include"
#define SRC_FOLDER "src"
#define PACMAN "pacman.cpi"
#define VENDOR "vendor"

bool generatePacman() {
    FILE *file = fopen(PACMAN, "wb");

    if(NULL != file) {
        fclose(file);
        return true;
    }
    return false;
}

/*
 * Show the help message
 */
void help() {
    printf("\nThis is cpi, a package manager for C\n");
    printf("Usage: cpi [");
    printColorText("command", NULL, CYAN);
    printf("] [");
    printColorText("package", NULL, MAGENTA);
    printf("]\n");

    printColorTextLn("Commands:", NULL, CYAN);
    printColorText("    -i ", NULL, CYAN);
    printColorText("package_name", NULL, MAGENTA);
    printf("%2s install a package\n", ":");

    printColorText("    -u ", NULL, CYAN);
    printColorText("package_name", NULL, MAGENTA);
    printf("%2s remove a package\n", ":");

    printColorText("    -l", NULL, CYAN);
    printf("%15s list installed packages\n", ":");

    printColorText("    -la", NULL, CYAN);
    printf("%14s list all available packages\n", ":");

    printColorText("    help", NULL, CYAN);
    printf("%13s show this help\n\n", ":");
}

/*
 * Check if a package is a valid package
 * all package have to follow the structure:
 * package
 *     - include
 *     - src
 *
 * @param packageName: the name of the package to check
 * @return true if the package is valid, false otherwise
 */
bool isPackage(char *packageName) {
    //struct dirent *de;
    char *include = "include";
    char *src = "src";
    char *string = malloc(sizeof(char) * 100);

    // has `include` folder
    sprintf(string, "%s/%s/%s", PACKAGE_FOLDER, packageName, include);
    DIR *dir = opendir(string);
    if(dir == NULL) {
        //printf("Error while opening directory: %s\n", string);
        return false;
    }
    closedir(dir);

    // has `src` folder
    sprintf(string, "%s/%s/%s", PACKAGE_FOLDER, packageName, src);
    dir = opendir(string);
    if(dir == NULL) {
        //printf("Error while opening directory: %s\n", string);
        return false;
    }
    closedir(dir);
    free(string);

    return true;
}

/*
 * Check if a package is installed
 * @param packageName: the name of the package to install
 * @return true if the package is installed, false otherwise
 */
bool isInstalled(char *packageName) {
    FILE * file = fopen(PACMAN, "rb");
    if(file != NULL) {
        Package * package = malloc(sizeof(Package));
        while(fread(package, sizeof(Package), 1, file) == 1) {
            if(strcmp(package->name, packageName) == 0) {
                return true;
            }
        }
    }
    return false;
}

bool addToPacman(Package * package) {
    if(NULL != package && !isInstalled(package->name)) {
        FILE * file = fopen(PACMAN, "ab");
        if(file != NULL) {
            fwrite(package, sizeof(Package), 1, file);
            fclose(file);
            return true;
        }
    }
    return false;
}

bool removeFromPacman(char *packageName) {
    if(isInstalled(packageName)) {
        FILE * file = fopen(PACMAN, "rb");
        if(file != NULL) {
            Package * package = malloc(sizeof(Package));
            FILE * temp = fopen("temp.cpi", "wb");
            while(fread(package, sizeof(Package), 1, file) == 1) {
                if(strcmp(package->name, packageName) != 0) {
                    fwrite(package, sizeof(Package), 1, temp);
                }
            }
            fclose(file);
            fclose(temp);
            remove(PACMAN);
            rename("temp.cpi", PACMAN);
            return true;
        }
    }
    return false;
}

struct dependencies *hydrateDependencies(char *packageName) {
    struct dirent *de;
    struct dependencies *dependencies = malloc(sizeof(struct dependencies));
    char *absolute_path_to_src = malloc(sizeof(char) * 100);
    char *absolute_path_to_include = malloc(sizeof(char) * 100);

    sprintf(absolute_path_to_src, "%s/%s/%s", PACKAGE_FOLDER, packageName, SRC_FOLDER);
    sprintf(absolute_path_to_include, "%s/%s/%s", PACKAGE_FOLDER, packageName, INCLUDE_FOLDER);

    DIR *dir = opendir(absolute_path_to_src);
    if(NULL != dir) {
        dependencies->src.size = 0;
        while ((de = readdir(dir)) != NULL) {
            if (strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) {
                strcpy(dependencies->src.c[dependencies->src.size], de->d_name);
                dependencies->src.size++;
            }
        }
        closedir(dir);
    }


    dir = opendir(absolute_path_to_include);
    if(NULL != dir) {
        dependencies->header.size = 0;
        while((de = readdir(dir)) != NULL) {
            if(strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0) {
                strcpy(dependencies->header.h[dependencies->header.size], de->d_name);
                dependencies->header.size++;
            }
        }
        closedir(dir);
    }

    return dependencies;

}

bool install(char * packageName) {
    if(isPackage(packageName)) {
        Package * package = malloc(sizeof(Package));
        strcpy(package->name, packageName);
        package->dependencies = hydrateDependencies(packageName);

        char * command = malloc(sizeof(char) * 100);
        char * working_directory = getcwd(NULL, 0);
        sprintf(command, "cp -r -u -i %s/%s %s/%s", PACKAGE_FOLDER, packageName, working_directory, VENDOR);
        system(command);
        free(command);

        if(addToPacman(package)) {
            printf("Package [");
            printColorText("%s", packageName, GREEN);
            printf("] was successfully installed\n");
            free(package);
            return true;
        }
    }
    return false;
}

bool uninstall(char * packageName) {
    if(isInstalled(packageName)) {
        char * command = malloc(sizeof(char) * 100);
        char * working_directory = getcwd(NULL, 0);
        sprintf(command, "rm -r %s/%s/%s", working_directory, VENDOR, packageName);
        system(command);
        free(command);

        if(removeFromPacman(packageName)) {
            //printf("Package %s uninstalled successfully\n", packageName);
            printf("Package [");
            printColorText("%s", packageName, YELLOW);
            printf("] was successfully uninstalled\n");
            return true;
        }
    }
    return false;
}

void list() {
    FILE * file = fopen(PACMAN, "rb");
    if(file != NULL) {
        Package * package = malloc(sizeof(Package));
        int counter = 0;

        printf("-----------------------------------------------------------------------------\n");
        printColorTextLn("\t   List of all packages locally installed in %s", VENDOR, CYAN);
        printf("-----------------------------------------------------------------------------\n\n");

        while(fread(package, sizeof(Package), 1, file) == 1) {
            printf("%3d. %s\n", ++counter, package->name);
        }

        printf("\n-----------------------------------------------------------------------------\n");
        fclose(file);
        free(package);
    }
}

/*
 * List all package installed in /home/lux/CLionProjects/cpi/package
 */
void listAll() {
    struct dirent *de;
    int counter = 1;
    char *wd = getcwd(NULL, 0);
    DIR *dir = opendir(PACKAGE_FOLDER);

    if(dir == NULL) {
        printf("Error while opening directory: %s\n", PACKAGE_FOLDER);
        printColorTextLn("Please make sure that the folder %s exists\n", PACKAGE_FOLDER, RED);
        return;
    }

    printf("-----------------------------------------------------------------------------\n");
    printColorTextLn("    List of all packages installed in %s", PACKAGE_FOLDER, CYAN);
    printf("-----------------------------------------------------------------------------\n\n");

    while((de = readdir(dir)) != NULL) {
        if(strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0 && isPackage(de->d_name)) {
            printf("%3d. %s\n",counter, de->d_name);
        }
    }

    printf("\n-----------------------------------------------------------------------------\n");

    closedir(dir);
}

void unknown_command(char * command) {
    printf("Unknown command [");
    printColorText("%s", command, RED);
    printf("]\n");
}

void package_not_found(char * package) {
    printf("Package [");
    printColorText("%s", package, RED);
    printf("] not found\n");
}

void package_not_installed(char * package) {
    printf("Package [");
    printColorText("%s", package, YELLOW);
    printf("] is currently not installed\n");
}