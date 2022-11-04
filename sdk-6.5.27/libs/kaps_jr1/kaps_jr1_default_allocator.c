

#include "kaps_jr1_default_allocator.h"
#include "kaps_jr1_portable.h"

struct kaps_jr1_default_allocator_handle
{
    struct kaps_jr1_default_allocator_stats stats;
    uint64_t nbytes;            
};

struct kaps_jr1_default_allocator_header
{
    uint64_t nbytes;
};

static void *
default_malloc(
    void *cookie,
    uint32_t size)
{
    struct kaps_jr1_default_allocator_header *hdr = NULL;

    if (size == 0)
        kaps_jr1_assert(0, "malloc of size zero is invalid");

    hdr = kaps_jr1_sysmalloc(size + sizeof(struct kaps_jr1_default_allocator_header));
    if (hdr)
    {
        struct kaps_jr1_default_allocator_handle *handle = (struct kaps_jr1_default_allocator_handle *) cookie;
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

static void *
default_calloc(
    void *cookie,
    uint32_t nelem,
    uint32_t size)
{
    uint32_t tot_size = nelem * size;
    struct kaps_jr1_default_allocator_header *hdr = NULL;

    if (tot_size == 0)
        kaps_jr1_assert(0, "calloc of size zero is invalid");

    hdr = kaps_jr1_syscalloc(1, tot_size + sizeof(struct kaps_jr1_default_allocator_header));
    if (hdr)
    {
        struct kaps_jr1_default_allocator_handle *handle = (struct kaps_jr1_default_allocator_handle *) cookie;
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

static void
default_free(
    void *cookie,
    void *ptr)
{
    if (ptr)
    {
        struct kaps_jr1_default_allocator_handle *handle = (struct kaps_jr1_default_allocator_handle *) cookie;
        struct kaps_jr1_default_allocator_header *hdr =
            (struct kaps_jr1_default_allocator_header *) ((uint8_t *) ptr - sizeof(struct kaps_jr1_default_allocator_header));
        kaps_jr1_sassert(handle->nbytes >= hdr->nbytes);
        handle->nbytes -= hdr->nbytes;
        handle->stats.nfrees++;
        kaps_jr1_sysfree(hdr);
    }
}

kaps_jr1_status
kaps_jr1_default_allocator_create(
    struct kaps_jr1_allocator **alloc)
{
    struct kaps_jr1_allocator *ret;
    struct kaps_jr1_default_allocator_handle *handle;

    if (!alloc)
        return KAPS_JR1_INVALID_ARGUMENT;

    ret = kaps_jr1_sysmalloc(sizeof(*ret));
    if (!ret)
    	return KAPS_JR1_OUT_OF_MEMORY;

    handle = kaps_jr1_syscalloc(1, sizeof(*handle));

    if (!handle) {
    	kaps_jr1_sysfree(ret);
        return KAPS_JR1_OUT_OF_MEMORY;
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
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_default_allocator_destroy(
    struct kaps_jr1_allocator * alloc)
{
    if (!alloc)
        return KAPS_JR1_INVALID_ARGUMENT;

    kaps_jr1_sysfree(alloc->cookie);
    kaps_jr1_sysfree(alloc);
    return KAPS_JR1_OK;
}

kaps_jr1_status
kaps_jr1_default_allocator_get_stats(
    struct kaps_jr1_allocator * alloc,
    struct kaps_jr1_default_allocator_stats * stats)
{
    struct kaps_jr1_default_allocator_handle *handle;

    if (!alloc || !stats)
        return KAPS_JR1_INVALID_ARGUMENT;

    handle = (struct kaps_jr1_default_allocator_handle *) alloc->cookie;
    kaps_jr1_memcpy(stats, &handle->stats, sizeof(*stats));
    return KAPS_JR1_OK;
}
