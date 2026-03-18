#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#include <net/if.h>

// IFF_UP is 0x1 per POSIX/Linux
#ifndef IFF_UP
#define IFF_UP 0x1
#endif

#include "interfaces.h"

// Print all active (UP) network interfaces with their IPv4/IPv6 addresses grouped
// Uses getifaddrs() - POSIX/Linux
int printInterfaces(void) {
    struct ifaddrs *ifaddr = NULL;

    if (getifaddrs(&ifaddr) == -1) {
        perror("getifaddrs");
        return -1;
    }

    // Dynamically allocated array of unique interface name strings
    char **names = NULL;
    int name_count = 0;

    // First pass: collect unique interface names in order of first appearance
    for (struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL) continue;
        if (!(ifa->ifa_flags & IFF_UP)) continue;

        int family = ifa->ifa_addr->sa_family;
        if (family != AF_INET && family != AF_INET6) continue;

        // Check if name already recorded
        int found = 0;
        for (int i = 0; i < name_count; i++) {
            if (strncmp(names[i], ifa->ifa_name, IF_NAMESIZE) == 0) {
                found = 1;
                break;
            }
        }

        if (!found) {
            char **tmp = realloc(names, sizeof(char *) * (name_count + 1));
            if (tmp == NULL) {
                perror("realloc");
                for (int i = 0; i < name_count; i++) free(names[i]);
                free(names);
                freeifaddrs(ifaddr);
                return -1;
            }
            names = tmp;
            names[name_count] = malloc(IF_NAMESIZE);
            if (names[name_count] == NULL) {
                perror("malloc");
                for (int i = 0; i < name_count; i++) free(names[i]);
                free(names);
                freeifaddrs(ifaddr);
                return -1;
            }
            strncpy(names[name_count], ifa->ifa_name, IF_NAMESIZE - 1);
            names[name_count][IF_NAMESIZE - 1] = '\0';
            name_count++;
        }
    }

    // Second pass: for each interface print header then all its addresses
    for (int i = 0; i < name_count; i++) {
        if (i > 0) printf("\n");
        printf("%s:\n", names[i]);

        for (struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
            if (ifa->ifa_addr == NULL) continue;
            if (!(ifa->ifa_flags & IFF_UP)) continue;
            if (strncmp(ifa->ifa_name, names[i], IF_NAMESIZE) != 0) continue;

            int family = ifa->ifa_addr->sa_family;
            if (family != AF_INET && family != AF_INET6) continue;

            char *addr_buf = calloc(INET6_ADDRSTRLEN, sizeof(char));
            if (addr_buf == NULL) {
                perror("calloc");
                for (int j = 0; j < name_count; j++) free(names[j]);
                free(names);
                freeifaddrs(ifaddr);
                return -1;
            }

            if (family == AF_INET) {
                struct sockaddr_in *sin = (struct sockaddr_in *)ifa->ifa_addr;
                inet_ntop(AF_INET, &sin->sin_addr, addr_buf, INET6_ADDRSTRLEN);
            } else {
                struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)ifa->ifa_addr;
                inet_ntop(AF_INET6, &sin6->sin6_addr, addr_buf, INET6_ADDRSTRLEN);
            }

            printf("  %s\n", addr_buf);
            free(addr_buf);
        }
    }

    // Cleanup
    for (int i = 0; i < name_count; i++) free(names[i]);
    free(names);
    freeifaddrs(ifaddr);
    return 0;
}