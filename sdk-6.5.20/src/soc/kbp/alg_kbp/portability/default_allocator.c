/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


#include "default_allocator.h"
#include "kbp_portable.h"


struct default_allocator_handle {
    struct default_allocator_stats stats;
    uint64_t nbytes; 
};

struct default_allocator_header
{
    uint64_t nbytes;
};

static void *default_malloc(void *cookie, uint32_t size)
{
    struct default_allocator_header *hdr = NULL;

    if (size == 0)
        kbp_assert(0, "malloc of size zero is invalid");

    hdr = kbp_sysmalloc(size + sizeof(struct default_allocator_header));
    if (hdr) {
        struct default_allocator_handle *handle = (struct default_allocator_handle *)cookie;
        handle->stats.nallocs++;
        handle->stats.cumulative_bytes += size;
        if ((handle->nbytes + size) > handle->stats.peak_bytes)
            handle->stats.peak_bytes = handle->nbytes + size;
        hdr->nbytes = size;
        handle->nbytes += size;
        return hdr + 1;
    }
    return NULL;
}

static void *default_calloc(void *cookie, uint32_t nelem, uint32_t size)
{
    uint32_t tot_size = nelem * size;
    struct default_allocator_header *hdr = NULL;

    if (tot_size == 0)
        kbp_assert(0, "calloc of size zero is invalid");

    hdr = kbp_sysmalloc(tot_size + sizeof(struct default_allocator_header));
    if (hdr) {
        struct default_allocator_handle *handle = (struct default_allocator_handle *)cookie;
        kbp_memset(hdr, 0, tot_size + sizeof(struct default_allocator_header));
        handle->stats.nallocs++;
        handle->stats.cumulative_bytes += (nelem * size);
        if ((handle->nbytes + tot_size) > handle->stats.peak_bytes)
            handle->stats.peak_bytes = handle->nbytes + tot_size;
        hdr->nbytes = tot_size;
        handle->nbytes += tot_size;
        return hdr + 1;
    }
    return NULL;
}

static void default_free(void *cookie, void *ptr)
{
    if (ptr) {
        struct default_allocator_handle *handle = (struct default_allocator_handle *)cookie;
        struct default_allocator_header *hdr = (struct default_allocator_header *) ((uint8_t *) ptr - sizeof(struct default_allocator_header));
        kbp_sassert(handle->nbytes >= hdr->nbytes);
        handle->nbytes -= hdr->nbytes;
        handle->stats.nfrees++;
        kbp_sysfree(hdr);
    }
}

kbp_status default_allocator_create(struct kbp_allocator **alloc)
{
    struct kbp_allocator *ret;
    struct default_allocator_handle *handle;

    if (!alloc)
        return KBP_INVALID_ARGUMENT;

    ret = kbp_sysmalloc(sizeof(*ret));
    handle = kbp_syscalloc(1, sizeof(*handle));

    if (!ret || !handle) {
        if (ret)
            kbp_sysfree(ret);
        if (handle)
            kbp_sysfree(handle);

        return KBP_OUT_OF_MEMORY;
    }

    ret->cookie = handle;
    ret->xmalloc = default_malloc;
    handle->nbytes = 0;
    handle->stats.nallocs = 0;
    handle->stats.nfrees = 0;
    handle->stats.cumulative_bytes = 0;
    handle->stats.peak_bytes = 0;;
    ret->xfree = default_free;
    ret->xcalloc = default_calloc;

    *alloc = ret;
    return KBP_OK;
}

kbp_status default_allocator_destroy(struct kbp_allocator * alloc)
{
    if (!alloc)
        return KBP_INVALID_ARGUMENT;

    kbp_sysfree(alloc->cookie);
    kbp_sysfree(alloc);
    return KBP_OK;
}

kbp_status default_allocator_get_stats(struct kbp_allocator *alloc, struct default_allocator_stats *stats)
{
    struct default_allocator_handle *handle;

    if (!alloc || !stats)
        return KBP_INVALID_ARGUMENT;

    handle = (struct default_allocator_handle *)alloc->cookie;
    kbp_memcpy(stats, &handle->stats, sizeof(*stats));
    return KBP_OK;
}
