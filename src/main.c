#include <stdio.h>

#include "arg_parser.h"
#include "interfaces.h"

int main(int argc, char **argv){
    args *parsed_args =  parseArgs(argc, argv);
    int return_code = 0;
    
    if(parsed_args->mode = ARG_ERROR){
        return_code = ARG_ERROR;
    }
    else{
        switch (parsed_args->mode){
            case HELP:
                printUsage();
                break;
            
            case INTERFACES:
                if(printInterfaces() != 0)
                    return_code = PERM_ERROR;
                break;
            
            case SCAN:
                printf("SCAN - TO DO\n");
                /*if(scanTcp() != 0)
                    return_code = TCP_ERROR
                  if(scanUdp() != 0)
                    return_code = UDP_ERROR
                */
                break;
        }
    }

    freeArgs(parsed_args);
    return return_code;
}