

#ifndef __KAPS_DEFAULT_ALLOCATOR_H
#define __KAPS_DEFAULT_ALLOCATOR_H

#include <stdint.h>

#include "kaps_allocator.h"
#include "kaps_errors.h"

#ifdef __cplusplus
extern "C" {
#endif





kaps_status kaps_default_allocator_create(struct kaps_allocator **alloc);



kaps_status kaps_default_allocator_destroy(struct kaps_allocator *alloc);



struct kaps_default_allocator_stats {
    uint64_t peak_bytes;        
    uint64_t cumulative_bytes;  
    uint64_t nallocs;           
    uint64_t nfrees;            
};



kaps_status kaps_default_allocator_get_stats(struct kaps_allocator *alloc, struct kaps_default_allocator_stats *stats);



#ifdef __cplusplus
}
#endif
#endif                          
