



#ifndef INCLUDED_KAPS_LEGACY_H
#define INCLUDED_KAPS_LEGACY_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif


typedef struct kaps_nlm_allocator kaps_nlm_allocator;




typedef void *(*kaps_nlm_allocator_alloc_t) (kaps_nlm_allocator *, size_t);

typedef void *(*kaps_nlm_allocator_sys_malloc_t) (size_t);


typedef void *(*kaps_nlm_allocator_calloc_t)
 (kaps_nlm_allocator *, size_t, size_t);


typedef void *(*kaps_nlm_allocator_resize_t)
 (kaps_nlm_allocator *, void *, size_t, size_t);


typedef void (*kaps_nlm_allocator_free_t)
 (kaps_nlm_allocator *, void *);

typedef void (*kaps_nlm_allocator_sys_free_t)
 (void *);


typedef void (*kaps_nlm_allocator_dtor_t)
 (kaps_nlm_allocator *);


typedef struct kaps_nlm_allocator_vtbl {
    const char *className;      
    kaps_nlm_allocator_alloc_t m_malloc;   
    kaps_nlm_allocator_calloc_t m_calloc;  
    kaps_nlm_allocator_resize_t m_resize;  
    kaps_nlm_allocator_free_t m_free;      
    kaps_nlm_allocator_dtor_t m_dtor;      
    const char *className1;     
    kaps_nlm_allocator_sys_malloc_t m_sysMalloc;    
    kaps_nlm_allocator_sys_free_t m_sysFree;        
} kaps_nlm_allocator_vtbl;


struct kaps_nlm_allocator {
    kaps_nlm_allocator_vtbl m_vtbl;        
    kaps_nlm_allocator_vtbl *m_vtbl_p;     
    void *m_clientData_p;       
    void *m_arena_info;              

};


extern kaps_nlm_allocator *kaps_nlm_allocator_ctor(kaps_nlm_allocator * self);


extern void kaps_nlm_allocator_config(kaps_nlm_allocator * self, kaps_nlm_allocator_vtbl * vtbl);




extern void kaps_nlm_allocator_dtor_body(kaps_nlm_allocator *);


void *kaps_nlm_allocator_malloc(kaps_nlm_allocator * self, size_t size);


void *kaps_nlm_allocator_calloc(kaps_nlm_allocator * self, size_t cnt, size_t size);


void *kaps_nlm_allocator_resize(kaps_nlm_allocator * self, void *memblk, size_t newSize, size_t oldSize);


void kaps_nlm_allocator_free(kaps_nlm_allocator * self, void *memblk);




#ifdef __cplusplus
}
#endif

#endif

