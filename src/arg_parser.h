#ifndef ARG_PARSER_H
#define ARG_PARSER_H

enum mode{
    HELP,
    INTERFACES,
    SCAN,
    ARG_ERROR = 64, //Invalid command line usage error code
};

typedef struct {
    int mode;

    char *interface;
    char *host;
    int timeout_ms;

    int *tcp_ports;
    int tcp_port_count;
    int *udp_ports;
    int udp_port_count;
} args;

args *parseArgs(int, char**);
int *parsePorts(const char*, int*);
int isFlag(const char*);
void printUsage();
void freeArgs(args*);

#endif