/*
 * OSNET control flags.
 */
#ifndef _OSNET_H
#define _OSNET_H
#include <stdbool.h>
#include <stdio.h>

#define OSNET_CREATE_VM_TEMPLATE        1
#define OSNET_MIGRATE_VM_TEMPLATING     1
#define OSNET_DEBUG                     1
#define OSNET_DEBUG_FILE                "/tmp/debug.txt"
#define OSNET_DEBUG_OUTPUT_MODE         "a"

#define OSNET_PRINT(out, ...) \
    if (out) { \
        fprintf(out, __VA_ARGS__); \
        fflush(out); \
    }

#if OSNET_MIGRATE_VM_TEMPLATING
extern bool osnet_init_ram_state;
enum osnet_dirty_bitmap_option {OSNET_CLEAR_DIRTY_BITMAP,
                                OSNET_INIT_DIRTY_BITMAP,
                                OSNET_DEFAULT_DIRTY_BITMAP};
extern enum osnet_dirty_bitmap_option osnet_bitmap_sync;
#endif

#if OSNET_DEBUG
extern FILE *osnet_outfi;
#endif

#endif
