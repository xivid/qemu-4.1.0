#ifndef _OSNET_VM_TEMPLATING_H
#define _OSNET_VM_TEMPLATING_H
#include <stdbool.h>
#include <stdio.h>

#define OSNET_CREATE_VM_TEMPLATE        1
#define OSNET_MIGRATE_VM_TEMPLATING     0
#define OSNET_DEBUG                     0
#define OSNET_UDP                       0
extern bool osnet_seamless_flag;
extern int osnet_tmpfs_start;
#define OSNET_PRINT(out, ...) \
    if (out) { \
        fprintf(out, __VA_ARGS__); \
        fflush(out); \
    }

#if OSNET_MIGRATE_VM_TEMPLATING
#define OSNET_MAX_LEN 512
struct OSNETRAMBlocks {
    RAMBlock *rbs[OSNET_MAX_LEN];
    int len;
};
extern bool osnet_init_ram_state;
extern struct OSNETRAMBlocks osnet_rbs;
enum osnet_dirty_bitmap_option {OSNET_CLEAR_DIRTY_BITMAP,
                                OSNET_INIT_DIRTY_BITMAP,
                                OSNET_DEFAULT_DIRTY_BITMAP};
extern enum osnet_dirty_bitmap_option osnet_bitmap_sync;
void osnet_init_orbs(struct OSNETRAMBlocks *orbs);
#endif

#if OSNET_DEBUG
#define OSNET_DEBUG_FILE                "/tmp/debug.txt"
#define OSNET_DEBUG_OUTPUT_MODE         "a"
extern FILE *osnet_outfi;
#endif

#if OSNET_UDP
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdbool.h>
#include <unistd.h>

#define OSNET_HOST_IP   "10.128.0.51"
#define OSNET_HOST_PORT "23456"

extern int fd_udp;
extern struct addrinfo hints_udp;
extern struct addrinfo *serv_udp;
extern struct addrinfo *pos_udp;
bool osnet_send_udp(const char *msg,int fd, struct addrinfo *pos);

#endif

#endif
