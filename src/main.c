#include <stdio.h>
#include <stdlib.h>

#include "arg_parser.h"

args* parsed_args;

void cleanup(){
    freeArgs(parsed_args);
}

int main(int argc, char **argv){
    atexit(cleanup);
    
    parsed_args =  parseArgs(argc, argv);

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

    exit(0);
}