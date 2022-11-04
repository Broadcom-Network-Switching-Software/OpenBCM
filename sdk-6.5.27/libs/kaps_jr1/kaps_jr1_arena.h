

#ifndef INCLUDED_KAPS_JR1_ARENA_H
#define INCLUDED_KAPS_JR1_ARENA_H

#include <kaps_jr1_legacy.h>
#include "kaps_jr1_internal_errors.h"
#include <kaps_jr1_externcstart.h>


#define KAPS_JR1_DEFAULT_ARENA_SIZE_IN_BYTES (400 * 1024)


NlmErrNum_t kaps_jr1_arena_init(
    kaps_jr1_nlm_allocator * alloc_p,
    uint32_t arenaSizeInBytes,
    NlmReasonCode * o_reason_p);


NlmErrNum_t kaps_jr1_arena_activate_arena(
    kaps_jr1_nlm_allocator * alloc_p,
    NlmReasonCode * o_reason_p);


NlmErrNum_t kaps_jr1_arena_deactivate_arena(
    kaps_jr1_nlm_allocator * alloc_p);


void *kaps_jr1_arena_allocate(
    kaps_jr1_nlm_allocator * alloc_p,
    uint32_t size);


int32_t kaps_jr1_arena_free_if_arena_memory(
    kaps_jr1_nlm_allocator * alloc_p,
    void *ptr);


void kaps_jr1_arena_destroy(
    kaps_jr1_nlm_allocator * alloc);

#include <kaps_jr1_externcend.h>

#endif
