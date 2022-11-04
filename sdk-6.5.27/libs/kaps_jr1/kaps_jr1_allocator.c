

#include "kaps_jr1_legacy.h"
#include "kaps_jr1_arena.h"
#include "kaps_jr1_portable.h"
#include "kaps_jr1_utility.h"



#include <kaps_jr1_externcstart.h>

static void *kaps_jr1_nlm_allocator_malloc_body(
    kaps_jr1_nlm_allocator * self,
    size_t size);

static void *kaps_jr1_nlm_allocator_sys_malloc_body(
    size_t size);

static void *kaps_jr1_nlm_allocator_calloc_body(
    kaps_jr1_nlm_allocator * self,
    size_t cnt,
    size_t size);

static void *kaps_jr1_nlm_allocator_resize_body(
    kaps_jr1_nlm_allocator * self,
    void *memblk,
    size_t newSize,
    size_t oldSize);

static void kaps_jr1_nlm_allocator_free_body(
    kaps_jr1_nlm_allocator * self,
    void *memblk);

static void kaps_jr1_nlm_allocator_sys_free_body(
    void *memblk);


static kaps_jr1_nlm_allocator_vtbl defaultVtbl = {
    "kaps_jr1_nlm_allocator_default",
    kaps_jr1_nlm_allocator_malloc_body,
    kaps_jr1_nlm_allocator_calloc_body,
    kaps_jr1_nlm_allocator_resize_body,
    kaps_jr1_nlm_allocator_free_body,
    kaps_jr1_nlm_allocator_dtor_body,
    "kaps_jr1_nlm_allocator_default",
    kaps_jr1_nlm_allocator_sys_malloc_body,
    kaps_jr1_nlm_allocator_sys_free_body,
};

kaps_jr1_nlm_allocator *
kaps_jr1_nlm_allocator_ctor(
    kaps_jr1_nlm_allocator * self)
{
    self->m_vtbl_p = &(self->m_vtbl);
    kaps_jr1_memcpy(self->m_vtbl_p, &defaultVtbl, sizeof(kaps_jr1_nlm_allocator_vtbl));
    self->m_clientData_p = self;
    self->m_arena_info = NULL;

    return self;
}

void
kaps_jr1_nlm_allocator_dtor_body(
    kaps_jr1_nlm_allocator * self)
{
    (void) self;
}

void
kaps_jr1_nlm_allocator_config(
    kaps_jr1_nlm_allocator * self,
    kaps_jr1_nlm_allocator_vtbl * vtbl)
{
    kaps_jr1_memcpy(&(self->m_vtbl), vtbl, sizeof(kaps_jr1_nlm_allocator_vtbl));
}

void *
kaps_jr1_nlm_allocator_malloc_body(
    kaps_jr1_nlm_allocator * self,
    size_t size)
{
    void *ptr;

    (void) self;

    ptr = kaps_jr1_sysmalloc(size);

    return ptr;
}

void *
kaps_jr1_nlm_allocator_sys_malloc_body(
    size_t size)
{
    return kaps_jr1_sysmalloc(size);
}

void *
kaps_jr1_nlm_allocator_calloc_body(
    kaps_jr1_nlm_allocator * self,
    size_t cnt,
    size_t size)
{
    
    void *ptr;
    size_t tot_size;

    (void) self;

    tot_size = cnt * size;

    ptr = kaps_jr1_sysmalloc(tot_size);
    if (ptr)
    {
        kaps_jr1_memset(ptr, 0, tot_size);
    }

    return ptr;
}

void *
kaps_jr1_nlm_allocator_resize_body(
    kaps_jr1_nlm_allocator * self,
    void *memblk,
    size_t newSize,
    size_t oldSize)
{
    void *newblk = 0;

    if (!memblk)
    {
        kaps_jr1_assert(0 == oldSize, "old size must be 0 if memblk is NULL");
        return kaps_jr1_nlm_allocator_malloc(self, newSize);
    }
    else if (newSize == oldSize)
        return memblk;

    

    if (0 != (newblk = kaps_jr1_nlm_allocator_malloc(self, newSize)))
    {
        kaps_jr1_memcpy(newblk, memblk, KAPS_JR1_MIN(newSize, oldSize));
        kaps_jr1_nlm_allocator_free(self, memblk);
    }
    else if (newSize < oldSize)
        return memblk;

    return newblk;
}

void
kaps_jr1_nlm_allocator_free_body(
    kaps_jr1_nlm_allocator * self,
    void *memblk)
{
    (void) self;
    

    if (memblk)
        kaps_jr1_sysfree(memblk);
}

void
kaps_jr1_nlm_allocator_sys_free_body(
    void *memblk)
{
    
    if (memblk)
        kaps_jr1_sysfree(memblk);

}

void *
kaps_jr1_nlm_allocator_malloc(
    kaps_jr1_nlm_allocator * self,
    size_t size)
{
    void *ptr;

    ptr = self->m_vtbl.m_malloc((kaps_jr1_nlm_allocator *) self->m_clientData_p, size);

    if (!ptr)
        ptr = kaps_jr1_arena_allocate(self, size);

    return ptr;
}

void *
kaps_jr1_nlm_allocator_calloc(
    kaps_jr1_nlm_allocator * self,
    size_t cnt,
    size_t size)
{
    void *ptr;
    size_t tot_size;

    tot_size = cnt * size;

    ptr = self->m_vtbl.m_calloc((kaps_jr1_nlm_allocator *) self->m_clientData_p, cnt, size);
    if (!ptr)
    {
        ptr = kaps_jr1_arena_allocate(self, tot_size);
        if (ptr)
        {
            kaps_jr1_memset(ptr, 0, tot_size);
        }
    }

    return ptr;
}

void *
kaps_jr1_nlm_allocator_resize(
    kaps_jr1_nlm_allocator * self,
    void *memblk,
    size_t newSize,
    size_t oldSize)
{
    void *newblk = 0;

    newblk = self->m_vtbl.m_resize((kaps_jr1_nlm_allocator *) self, memblk, newSize, oldSize);
    if (!newblk)
    {
        newblk = kaps_jr1_arena_allocate(self, newSize);
        if (newblk && memblk)
        {
            kaps_jr1_memcpy(newblk, memblk, KAPS_JR1_MIN(newSize, oldSize));
            kaps_jr1_nlm_allocator_free(self, memblk);
        }
    }

    return newblk;
}

void
kaps_jr1_nlm_allocator_free(
    kaps_jr1_nlm_allocator * self,
    void *memblk)
{
    if (memblk)
    {
        if (!kaps_jr1_arena_free_if_arena_memory(self, memblk))
            self->m_vtbl.m_free((kaps_jr1_nlm_allocator *) self->m_clientData_p, memblk);
    }
}

#include <kaps_jr1_externcend.h>


