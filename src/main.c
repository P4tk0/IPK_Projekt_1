#include <stdio.h>
#include <stdlib.h>

#include "arg_parser.h"

int main(int argc, char **argv){
    
    args *parsed_args =  parseArgs(argc, argv);

    switch (parsed_args->mode){
        case HELP:
            printUsage();
            break;

        case INTERFACES:
            printf("Active interfaces:\n");
            break;

        case SCAN:
            printf("SCAN - TO DO\n");
            break;

        case ERROR:
            exit(ERROR);
            break;
    }
}