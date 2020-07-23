/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#ifndef __DEFAULT_ALLOCATOR_H
#define __DEFAULT_ALLOCATOR_H

#include <stdint.h>

#include "allocator.h"
#include "errors.h"

#ifdef __cplusplus
extern "C" {
#endif





kbp_status default_allocator_create(struct kbp_allocator **alloc);



kbp_status default_allocator_destroy(struct kbp_allocator *alloc);



struct default_allocator_stats {
    uint64_t peak_bytes;        
    uint64_t cumulative_bytes;  
    uint64_t nallocs;           
    uint64_t nfrees;            
};



kbp_status default_allocator_get_stats(struct kbp_allocator *alloc, struct default_allocator_stats *stats);



#ifdef __cplusplus
}
#endif
#endif                          
