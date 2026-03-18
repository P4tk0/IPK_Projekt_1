#ifndef INTERFACES_H
#define INTERFACES_H

#define PERM_ERROR 77

// Print all active network interfaces with their IP addresses to stdout
// Returns 0 on success, -1 on error
int printInterfaces(void);

#endif