#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arg_parser.h"

// Returns 1 if string starts with '-'
int isFlag(const char *s) {
    return s != NULL && s[0] == '-';
}

// Parse "22", "1-1024", "22,80,443" into a malloc'd int array
// Returns NULL on error, sets count
int *parsePorts(const char *s, int *count) {
    *count = 0;
    int *ports = NULL;

    // Comma-separated list: "22,80,443"
    if (strchr(s, ',') != NULL) {
        char *copy = malloc(strlen(s) + 1);
        strcpy(copy, s);
        char *token = strtok(copy, ",");
        while (token != NULL) {
            int p = atoi(token);
            if (p < 1 || p > 65535) {
                fprintf(stderr, "Error: port out of range: %s\n", token);
                free(copy);
                free(ports);
                return NULL;
            }
            ports = realloc(ports, sizeof(int) * (*count + 1));
            ports[(*count)++] = p;
            token = strtok(NULL, ",");
        }
        free(copy);
        return ports;
    }

    // Range: "1-1024"
    char *dash = strchr(s, '-');
    if (dash != NULL) {
        int lo = atoi(s);
        int hi = atoi(dash + 1);
        if (lo < 1 || hi > 65535 || lo > hi) {
            fprintf(stderr, "Error: invalid port range: %s\n", s);
            return NULL;
        }
        *count = hi - lo + 1;
        ports = malloc(sizeof(int) * (*count));
        for (int i = 0; i < *count; i++) {
            ports[i] = lo + i;
        }
        return ports;
    }

    // Single port: "22"
    int p = atoi(s);
    if (p < 1 || p > 65535) {
        fprintf(stderr, "Error: port out of range: %s\n", s);
        return NULL;
    }
    ports = malloc(sizeof(int));
    ports[0] = p;
    *count = 1;
    return ports;
}

args *parseArgs(int argc, char **argv) {
    args *parsed_args = malloc(sizeof(args));

    // Set defaults
    parsed_args->interface      = NULL;
    parsed_args->host           = NULL;
    parsed_args->tcp_ports      = NULL;
    parsed_args->tcp_port_count = 0;
    parsed_args->udp_ports      = NULL;
    parsed_args->udp_port_count = 0;
    parsed_args->timeout_ms     = 1000;
    parsed_args->mode           = SCAN;

    if (argc < 2) {
        fprintf(stderr, "No arguments specified, use -h or --help for usage\n");
        parsed_args->mode = ARG_ERROR;
        return parsed_args;
    }

    // First pass - check for -h/--help anywhere (priority)
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            parsed_args->mode = HELP;
            return parsed_args;
        }
    }

    // Second pass - parse everything else
    for (int i = 1; i < argc; i++) {

        // -i [interface]
        if (strcmp(argv[i], "-i") == 0) {
            if (i + 1 >= argc || isFlag(argv[i + 1])) {
                parsed_args->mode = INTERFACES;
                return parsed_args;
            }
            parsed_args->interface = malloc(strlen(argv[i+1]) + 1);
            strcpy(parsed_args->interface, argv[++i]);
        }

        // -t PORTS
        else if (strcmp(argv[i], "-t") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: -t requires a port specification\n");
                parsed_args->mode = ARG_ERROR;
                return parsed_args;
            }
            parsed_args->tcp_ports = parsePorts(argv[++i], &parsed_args->tcp_port_count);
            if (parsed_args->tcp_ports == NULL) {
                parsed_args->mode = ARG_ERROR;
                return parsed_args;
            }
        }

        // -u PORTS
        else if (strcmp(argv[i], "-u") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: -u requires a port specification\n");
                parsed_args->mode = ARG_ERROR;
                return parsed_args;
            }
            parsed_args->udp_ports = parsePorts(argv[++i], &parsed_args->udp_port_count);
            if (parsed_args->udp_ports == NULL) {
                parsed_args->mode = ARG_ERROR;
                return parsed_args;
            }
        }

        // -w TIMEOUT
        else if (strcmp(argv[i], "-w") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "Error: -w requires a timeout value\n");
                parsed_args->mode = ARG_ERROR;
                return parsed_args;
            }
            parsed_args->timeout_ms = atoi(argv[++i]);
            if (parsed_args->timeout_ms <= 0) {
                fprintf(stderr, "Error: -w timeout must be a positive number\n");
                parsed_args->mode = ARG_ERROR;
                return parsed_args;
            }
        }

        // Positional HOST
        else if (!isFlag(argv[i])) {
            if (parsed_args->host != NULL) {
                fprintf(stderr, "Error: multiple HOST arguments given\n");
                parsed_args->mode = ARG_ERROR;
                return parsed_args;
            }
            parsed_args->host = malloc(strlen(argv[i]) + 1);
            strcpy(parsed_args->host, argv[i]);
        }

        // Unknown flag
        else {
            fprintf(stderr, "Error: unknown argument: %s\n", argv[i]);
            parsed_args->mode = ARG_ERROR;
            return parsed_args;
        }
    }

    // Semantic validation
    if (parsed_args->mode == SCAN) {
        if (parsed_args->interface == NULL) {
            fprintf(stderr, "Error: missing required argument -i INTERFACE\n");
            parsed_args->mode = ARG_ERROR;
            return parsed_args;
        }
        if (parsed_args->host == NULL) {
            fprintf(stderr, "Error: missing required argument HOST\n");
            parsed_args->mode = ARG_ERROR;
            return parsed_args;
        }
        if (parsed_args->tcp_port_count == 0 && parsed_args->udp_port_count == 0) {
            fprintf(stderr, "Error: at least one of -t or -u must be specified\n");
            parsed_args->mode = ARG_ERROR;
            return parsed_args;
        }
    }

    return parsed_args;
}

void printUsage() {
    printf("Usage:\n");
    printf("  -i INTERFACE [-t PORTS] [-u PORTS] [-w TIMEOUT] HOST\n");
    printf("  -i            # list active interfaces\n");
    printf("  -h|--help\n\n");
    printf("Options:\n");
    printf("  -i INTERFACE   Network interface to use\n");
    printf("  -t PORTS       TCP ports  (e.g. 22 | 1-1024 | 22,80,443)\n");
    printf("  -u PORTS       UDP ports  (e.g. 53 | 1-1024 | 53,67)\n");
    printf("  -w TIMEOUT     Timeout in milliseconds (default: 1000)\n");
    printf("  -h, --help     Show this help and exit\n");
}

void freeArgs(args* argStruct){
    if(argStruct->tcp_ports) free(argStruct->tcp_ports);
    if(argStruct->udp_ports) free(argStruct->udp_ports);
    if(argStruct->host)      free(argStruct->host);
    if(argStruct->interface) free(argStruct->interface);

    free(argStruct);
}