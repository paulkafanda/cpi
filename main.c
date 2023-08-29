#include <stdio.h>
#include <stdlib.h>
#include "include/utils.h"



int main() {
    help();
    listAll();
//    install("cli");
     uninstall("cli");

    list();
    return 0;
}