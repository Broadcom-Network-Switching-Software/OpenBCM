

#ifndef __KAPS_JR1_DEFAULT_ALLOCATOR_H
#define __KAPS_JR1_DEFAULT_ALLOCATOR_H

#include <stdint.h>

#include "kaps_jr1_allocator.h"
#include "kaps_jr1_errors.h"

#ifdef __cplusplus
extern "C" {
#endif





kaps_jr1_status kaps_jr1_default_allocator_create(struct kaps_jr1_allocator **alloc);



kaps_jr1_status kaps_jr1_default_allocator_destroy(struct kaps_jr1_allocator *alloc);



struct kaps_jr1_default_allocator_stats {
    uint64_t peak_bytes;        
    uint64_t cumulative_bytes;  
    uint64_t nallocs;           
    uint64_t nfrees;            
};



kaps_jr1_status kaps_jr1_default_allocator_get_stats(struct kaps_jr1_allocator *alloc, struct kaps_jr1_default_allocator_stats *stats);



#ifdef __cplusplus
}
#endif
#endif                          
