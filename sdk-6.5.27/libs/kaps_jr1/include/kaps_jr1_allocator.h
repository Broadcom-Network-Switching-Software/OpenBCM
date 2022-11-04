

#ifndef __KAPS_JR1_ALLOCATOR_H
#define __KAPS_JR1_ALLOCATOR_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif





struct kaps_jr1_allocator {
    void *cookie;                                    
    void *(*xmalloc) (void *cookie, uint32_t size);  
    void *(*xcalloc) (void *cookie, uint32_t nelements, uint32_t size);  
    void (*xfree) (void *cookie, void *ptr);         
};



#ifdef __cplusplus
}
#endif
#endif                          
