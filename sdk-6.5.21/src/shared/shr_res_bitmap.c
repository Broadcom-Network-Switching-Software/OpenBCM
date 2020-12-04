/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Indexed resource management -- simple bitmap
 */

#include <shared/bsl.h>
#include <shared/alloc.h>
#include <bcm/error.h>
#include <soc/cm.h>
#include <shared/shr_res_bitmap.h>

/*
 *  Macros and other things that change according to settings...
 */
#if SHR_RES_BITMAP_SEARCH_RESUME
#define SHR_RES_BITMAP_FINAL_SEARCH_LIMIT (handle->nextAlloc)
#else /* SHR_RES_BITMAP_SEARCH_RESUME */
#define SHR_RES_BITMAP_FINAL_SEARCH_LIMIT (handle->count - count)
#endif /* SHR_RES_BITMAP_SEARCH_RESUME */

/* inline */ static int
_shr_res_bitmap_check_all(shr_res_bitmap_handle_t handle,
                          int count,
                          int index)
{
    int offset;
    int freed = 0;
    int inuse = 0;

    /* scan the block */
    for (offset = 0; offset < count; offset++) {
        if (SHR_BITGET(handle->data, index + offset)) {
            inuse++;
        } else {
            freed++;
        }
    } /* for (offset = 0; offset < count; offset++) */
    if (inuse == count) {
        /* block is entirely in use */
        return BCM_E_FULL;
    } else if (freed == count) {
        /* block is entirely free */
        return BCM_E_EMPTY;
    } else {
        /* block is partially free and partially in use */
        return BCM_E_EXISTS;
    }
}

/* inline */ static int
_shr_res_bitmap_check_all_sparse(shr_res_bitmap_handle_t handle,
                                 uint32 pattern,
                                 int length,
                                 int repeats,
                                 int base)
{
    int index;
    int offset;
    int elem;
    int elemCount;
    int usedCount;

    for (index = 0, elem = base, elemCount = 0, usedCount = 0;
         index < repeats;
         index++) {
        for (offset = 0; offset < length; offset++, elem++) {
            if (pattern & (1 << offset)) {
                /* this element is in the pattern */
                elemCount++;
                if (SHR_BITGET(handle->data, elem)) {
                    /* this element is in use */
                    usedCount++;
                }
            } /* if (pattern & (1 << offset)) */
        } /* for (the length of the pattern) */
    } /* for (as many times as the pattern repeats) */
    if (elemCount == usedCount) {
        /* block is entirely in use */
        return BCM_E_FULL;
    } else if (0 == usedCount) {
        /* block is entirely free */
        return BCM_E_EMPTY;
    } else {
        /* block is partially free and partially in use */
        return BCM_E_EXISTS;
    }
}

int
shr_res_bitmap_create(shr_res_bitmap_handle_t *handle,
                      int low_id,
                      int count)
{
    shr_res_bitmap_handle_t desc;

    /* check arguments */
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("must have a positive number of elements\n")));
        return BCM_E_PARAM;
    }
    if (!handle) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("obligatory out argument must not be NULL\n")));
        return BCM_E_PARAM;
    }
    /* alloc memory for descriptor & data */
    desc = sal_alloc(sizeof(*desc) +
                     SHR_BITALLOCSIZE(count) -
                     sizeof(SHR_BITDCL),
                     "bitmap resource data");
    if (!desc) {
        /* alloc failed */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to allocate %u bytes for data\n"),
                   (unsigned int)(sizeof(*desc) +
                   SHR_BITALLOCSIZE(count) -
                   sizeof(SHR_BITDCL))));
        return BCM_E_MEMORY;
    }
    /* init descriptor and data */
    sal_memset(desc,
               0x00,
               sizeof(*desc) +
               SHR_BITALLOCSIZE(count) -
               sizeof(SHR_BITDCL));
    desc->low = low_id;
    desc->count = count;
    *handle = desc;
    /* all's well if we got here */
    return BCM_E_NONE;
}

int
shr_res_bitmap_destroy(shr_res_bitmap_handle_t handle)
{
    if (handle) {
        sal_free(handle);
        return BCM_E_NONE;
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to free NULL handle\n")));
        return BCM_E_PARAM;
    }
}

int
shr_res_bitmap_alloc(shr_res_bitmap_handle_t handle,
                     uint32 flags,
                     int count,
                     int *elem)
{
    int index;
    int offset;
    int result = BCM_E_NONE;

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to alloc from NULL descriptor\n")));
        return BCM_E_PARAM;
    }
    if (!elem) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("obligatory in/out argument must not be NULL\n")));
        return BCM_E_PARAM;
    }
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("must allocate at least one element\n")));
        return BCM_E_PARAM;
    }
    if (SHR_RES_BITMAP_ALLOC_REPLACE ==
        (flags & (SHR_RES_BITMAP_ALLOC_REPLACE |
                  SHR_RES_BITMAP_ALLOC_WITH_ID))) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("must use WITH_ID when using REPLACE\n")));
        return BCM_E_PARAM;
    }

    if (flags & SHR_RES_BITMAP_ALLOC_WITH_ID) {
        /* WITH_ID, so only try the specifically requested block */
        if (*elem < handle->low) {
            /* not valid ID */
            result = BCM_E_PARAM;
        }
        index = *elem - handle->low;
        if (index + count > handle->count) {
            /* not valid ID */
            result = BCM_E_PARAM;
        }
        if (BCM_E_NONE == result) {
            result = _shr_res_bitmap_check_all(handle, count, index);
            switch (result) {
            case BCM_E_FULL:
                if (flags & SHR_RES_BITMAP_ALLOC_REPLACE) {
                    result = BCM_E_NONE;
                } else {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META("proposed block %p base %d count %d"
                               " already exists\n"),
                               (void*)handle,
                               *elem,
                               count));
                    result = BCM_E_RESOURCE;
                }
                break;
            case BCM_E_EMPTY:
                if (flags & SHR_RES_BITMAP_ALLOC_REPLACE) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META("proposed block %p base %d count %d"
                               " does not exist\n"),
                               (void*)handle,
                               *elem,
                               count));
                    result = BCM_E_NOT_FOUND;
                } else {
                    result = BCM_E_NONE;
                }
                break;
            case BCM_E_EXISTS:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META("proposed block %p base %d count %d"
                           " would merge/expand existing block(s)\n"),
                           (void*)handle,
                           *elem,
                           count));
                result = BCM_E_RESOURCE;
                break;
            default:
                /* should never see this */
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META("unexpected result checking proposed block:"
                           " %d (%s)\n"),
                           result,
                           _SHR_ERRMSG(result)));
                if (BCM_E_NONE == result) {
                    result = BCM_E_INTERNAL;
                }
            }
        }
        /* don't adjust last free or next alloc for WITH_ID */
    } else { /* if (flags & SHR_RES_ALLOC_WITH_ID) */
#if SHR_RES_BITMAP_SEARCH_RESUME
        /* see if there are enough elements after last free */
        index = handle->lastFree;
        if (index + count < handle->count) {
            /* it might fit */
            for (offset = 0; offset < count; offset++) {
                if (SHR_BITGET(handle->data, index + offset)) {
                    result = BCM_E_EXISTS;
                    break;
                }
            }
        } else {
            result = BCM_E_EXISTS;
        }
        if (BCM_E_NONE == result) {
            /* looks good; adjust last free to miss this block */
            handle->lastFree = index + count;
        } else { /* if (BCM_E_NONE == result) */
            /* start searching after last successful alloc */
            index = handle->nextAlloc;
            while (index <= handle->count - count) {
                while (SHR_BITGET(handle->data, index) &&
                       (index > handle->count - count)) {
                    index++;
                }
                if (index <= handle->count - count) {
                    /* have a candidate; see if block is big enough */
                    result = BCM_E_NONE;
                    for (offset = 0; offset < count; offset++) {
                        if (SHR_BITGET(handle->data, index + offset)) {
                            /* not big enough; skip this block */
                            result = BCM_E_EXISTS;
                            index += offset + 1;
                            break;
                        }
                    } /* for (offset = 0; offset < count; offset++) */
                } /* if (index <= desc->count - count) */
                if (BCM_E_NONE == result) {
                    /* found a sufficient block */
                    break;
                }
            } /* while (index <= desc->count - count) */
            if (BCM_E_NONE != result) {
                /* no space, so try space before last successful alloc */
#else /* SHR_RES_BITMAP_SEARCH_RESUME */
                result = BCM_E_RESOURCE;
#endif /* SHR_RES_BITMAP_SEARCH_RESUME */
                index = 0;
                while (index < SHR_RES_BITMAP_FINAL_SEARCH_LIMIT) {
                    while ((index < SHR_RES_BITMAP_FINAL_SEARCH_LIMIT) &&
                           SHR_BITGET(handle->data, index)) {
                        index++;
                    }
                    if (index < SHR_RES_BITMAP_FINAL_SEARCH_LIMIT) {
                        /* have a candidate; see if block is big enough */
                        result = BCM_E_NONE;
                        for (offset = 0; offset < count; offset++) {
                            if (SHR_BITGET(handle->data, index + offset)) {
                                /* not big enough; skip this block */
                                result = BCM_E_EXISTS;
                                index += offset + 1;
                                break;
                            }
                        } /* for (offset = 0; offset < count; offset++) */
                    } /* if (index < data->next_alloc) */
                    if (BCM_E_NONE == result) {
                        /* found a sufficient block */
                        break;
                    }
                } /* while (index < data->next_alloc) */
#if SHR_RES_BITMAP_SEARCH_RESUME
            } /* if (BCM_E_NONE != result) */
#endif /* SHR_RES_BITMAP_SEARCH_RESUME */
            if (BCM_E_NONE != result) {
                /* still no space; give up */
                result = BCM_E_RESOURCE;
#if SHR_RES_BITMAP_SEARCH_RESUME
            } else {
                /* got some space; update next alloc  */
                handle->nextAlloc = index + count;
#endif /* SHR_RES_BITMAP_SEARCH_RESUME */
            }
#if SHR_RES_BITMAP_SEARCH_RESUME
        } /* if (BCM_E_NONE == result) */
#endif /* SHR_RES_BITMAP_SEARCH_RESUME */
    } /* if (flags & SHR_RES_ALLOC_WITH_ID) */
    if (BCM_E_NONE == result) {
        /* return the beginning element */
        *elem = index + handle->low;
        /* mark the block as in use */
        SHR_BITSET_RANGE(handle->data, index, count);
        if (0 == (flags & SHR_RES_BITMAP_ALLOC_REPLACE)) {
            /* only adjust accounting if not replacing existing block */
            handle->used += count;
        }
    } /* if (BCM_E_NONE == result) */
    return result;
}

int
shr_res_bitmap_alloc_align(shr_res_bitmap_handle_t handle,
                           uint32 flags,
                           int align,
                           int offs,
                           int count,
                           int *elem)
{
    int index;
    int offset;
    int result = BCM_E_NONE;

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to alloc from NULL descriptor\n")));
        return BCM_E_PARAM;
    }
    if (!elem) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("obligatory in/out argument must not be NULL\n")));
        return BCM_E_PARAM;
    }
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("must allocate at least one element\n")));
        return BCM_E_PARAM;
    }
    if (SHR_RES_BITMAP_ALLOC_REPLACE ==
        (flags & (SHR_RES_BITMAP_ALLOC_REPLACE |
                  SHR_RES_BITMAP_ALLOC_WITH_ID))) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("must use WITH_ID when using REPLACE\n")));
        return BCM_E_PARAM;
    }

    if (flags & SHR_RES_BITMAP_ALLOC_WITH_ID) {
        /* WITH_ID, so only try the specifically requested block */
        if (*elem < handle->low) {
            /* not valid ID */
            result = BCM_E_PARAM;
        }
        index = *elem - handle->low;
        if (index + count > handle->count) {
            /* not valid ID */
            result = BCM_E_PARAM;
        }
        if (BCM_E_NONE == result) {
            /* make sure caller's request is valid */
            if (flags & SHR_RES_BITMAP_ALLOC_ALIGN_ZERO) {
                /* alignment is against zero */
                offset = (*elem) % align;
            } else {
                /* alignment is against low */
                offset = ((*elem) - handle->low) % align;
            }
            if (offset != offs) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META("provided first element %d does not conform"
                           " to provided align %d + offset %d values"
                           " (actual offset = %d)\n"),
                           *elem,
                           align,
                           offset,
                           offs));
                result = BCM_E_PARAM;
            }
        }
        if (BCM_E_NONE == result) {
            result = _shr_res_bitmap_check_all(handle, count, index);
            switch (result) {
            case BCM_E_FULL:
                if (flags & SHR_RES_BITMAP_ALLOC_REPLACE) {
                    result = BCM_E_NONE;
                } else {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META("proposed block %p base %d count %d"
                               " already exists\n"),
                               (void*)handle,
                               *elem,
                               count));
                    result = BCM_E_RESOURCE;
                }
                break;
            case BCM_E_EMPTY:
                if (flags & SHR_RES_BITMAP_ALLOC_REPLACE) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META("proposed block %p base %d count %d"
                               " does not exist\n"),
                               (void*)handle,
                               *elem,
                               count));
                    result = BCM_E_NOT_FOUND;
                } else {
                    result = BCM_E_NONE;
                }
                break;
            case BCM_E_EXISTS:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META("proposed block %p base %d count %d"
                           " would merge/expand existing block(s)\n"),
                           (void*)handle,
                           *elem,
                           count));
                result = BCM_E_RESOURCE;
                break;
            default:
                /* should never see this */
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META("unexpected result checking proposed block:"
                           " %d (%s)\n"),
                           result,
                           _SHR_ERRMSG(result)));
                if (BCM_E_NONE == result) {
                    result = BCM_E_INTERNAL;
                }
            }
        }
        /* don't adjust last free or next alloc for WITH_ID */
    } else { /* if (flags & SHR_RES_ALLOC_WITH_ID) */
        if (flags & SHR_RES_BITMAP_ALLOC_ALIGN_ZERO) {
            /* alignment is against zero, not start of pool */
            offs += align - (handle->low % align);
        }
#if SHR_RES_BITMAP_SEARCH_RESUME
        /* see if there are enough elements after last free */
        index = (((handle->lastFree + align - 1) / align) * align) + offs;
        if (index + count < handle->count) {
            /* it might fit */
            for (offset = 0; offset < count; offset++) {
                if (SHR_BITGET(handle->data, index + offset)) {
                    result = BCM_E_EXISTS;
                    break;
                }
            }
        } else {
            result = BCM_E_EXISTS;
        }
        if (BCM_E_NONE == result) {
            /* looks good; adjust last free to miss this block */
            if (0 == offs) {
                handle->lastFree = index + count;
            }
        } else { /* if (BCM_E_NONE == result) */
            /* start searching after last successful alloc */
            index = (((handle->nextAlloc + align - 1) / align) * align) + offs;
            while (index <= handle->count - count) {
                while ((index <= handle->count - count) &&
                       SHR_BITGET(handle->data, index)) {
                    index += align;
                }
                if (index <= handle->count - count) {
                    /* have a candidate; see if block is big enough */
                    result = BCM_E_NONE;
                    for (offset = 0; offset < count; offset++) {
                        if (SHR_BITGET(handle->data, index + offset)) {
                            /* not big enough; skip this block */
                            result = BCM_E_EXISTS;
                            index = (((index + offset + align) / align) * align) + offs;
                            break;
                        }
                    } /* for (offset = 0; offset < count; offset++) */
                } /* if (index <= desc->count - count) */
                if (BCM_E_NONE == result) {
                    /* found a sufficient block */
                    break;
                }
            } /* while (index <= desc->count - count) */
            if (BCM_E_NONE != result) {
#endif /* SHR_RES_BITMAP_SEARCH_RESUME */
                /* no space, so try space before last successful alloc */
                index = offs;
                while (index < SHR_RES_BITMAP_FINAL_SEARCH_LIMIT) {
                    while ((index < SHR_RES_BITMAP_FINAL_SEARCH_LIMIT) &&
                           SHR_BITGET(handle->data, index)) {
                        index += align;
                    }
                    if (index < SHR_RES_BITMAP_FINAL_SEARCH_LIMIT) {
                        /* have a candidate; see if block is big enough */
                        result = BCM_E_NONE;
                        for (offset = 0; offset < count; offset++) {
                            if (SHR_BITGET(handle->data, index + offset)) {
                                /* not big enough; skip this block */
                                result = BCM_E_EXISTS;
                                index = (((index + offset + align) / align) * align) + offs;
                                break;
                            }
                        } /* for (offset = 0; offset < count; offset++) */
                    } /* if (index < data->next_alloc) */
                    if (BCM_E_NONE == result) {
                        /* found a sufficient block */
                        break;
                    }
                } /* while (index < data->next_alloc) */
#if SHR_RES_BITMAP_SEARCH_RESUME
            } /* if (BCM_E_NONE != result) */
#endif /* SHR_RES_BITMAP_SEARCH_RESUME */
            if (BCM_E_NONE != result) {
                /* still no space; give up */
                result = BCM_E_RESOURCE;
#if SHR_RES_BITMAP_SEARCH_RESUME
            } else {
                /* got some space; update next alloc  */
                handle->nextAlloc = index + count;
#endif /* SHR_RES_BITMAP_SEARCH_RESUME */
            }
#if SHR_RES_BITMAP_SEARCH_RESUME
        } /* if (BCM_E_NONE == result) */
#endif /* SHR_RES_BITMAP_SEARCH_RESUME */
    } /* if (flags & SHR_RES_ALLOC_WITH_ID) */
    if (BCM_E_NONE == result) {
        /* return the beginning element */
        *elem = index + handle->low;
        /* mark the block as in use */
        SHR_BITSET_RANGE(handle->data, index, count);
        if (0 == (flags & SHR_RES_BITMAP_ALLOC_REPLACE)) {
            /* only adjust accounting if not replacing existing block */
            handle->used += count;
        }
    } /* if (BCM_E_NONE == result) */
    return result;
}

int
shr_res_bitmap_alloc_align_sparse(shr_res_bitmap_handle_t handle,
                                  uint32 flags,
                                  int align,
                                  int offs,
                                  uint32 pattern,
                                  int length,
                                  int repeats,
                                  int *elem)
{
    int index;
    int offset;
    int repeat;
    int current;
    int first;
    int count;
    int result = BCM_E_NONE;
    uint32 pattern_mask;

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to alloc from NULL descriptor\n")));
        return BCM_E_PARAM;
    }
    if (!elem) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("obligatory in/out argument must not be NULL\n")));
        return BCM_E_PARAM;
    }
    if (0 >= length) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("pattern must be at least one long\n")));
        return BCM_E_PARAM;
    }
    if (32 < length) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("pattern must not be longer than 32\n")));
        return BCM_E_PARAM;
    }
    if (0 >= repeats) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("must allocate at least one pattern\n")));
        return BCM_E_PARAM;
    }
    pattern_mask = (((uint32)1 << (length - 1)) - 1) | ((uint32)1 << (length - 1));
    if (0 == (pattern & pattern_mask)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("pattern must contain at least one element\n")));
        return BCM_E_PARAM;
    }
    if (pattern & ~pattern_mask) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("pattern must not contain unused bits\n")));
        return BCM_E_PARAM;
    }
    if (SHR_RES_BITMAP_ALLOC_REPLACE ==
        (flags & (SHR_RES_BITMAP_ALLOC_REPLACE |
                  SHR_RES_BITMAP_ALLOC_WITH_ID))) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("must use WITH_ID when using REPLACE\n")));
        return BCM_E_PARAM;
    }
    /* find the final set bit of the repeated pattern */
    index = length;
    count = 0;
    do {
        index--;
        if (pattern & (1 << index)) {
            count = index;
            break;
        }
    } while (index > 0);
    count += (length * (repeats - 1));
    /* find the first set bit of the repeated pattern */
    for (first = 0; first < length; first++) {
        if (pattern & (1 << first)) {
            break;
        }
    }
#if 0 
    if (first) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META("first element is not at zero; the returned block"
                  " will not point to the first element allocated,"
                  " but will point %d elements before it\n"),
                  first));
    }
#endif 

    if (flags & SHR_RES_BITMAP_ALLOC_WITH_ID) {
        /* WITH_ID, so only try the specifically requested block */
        if (*elem < handle->low) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("first element is too low\n")));
            result = BCM_E_PARAM;
        }
        index = *elem - handle->low;
        if (index + count > handle->count) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META("final element is too high\n")));
            result = BCM_E_PARAM;
        }
        if (BCM_E_NONE == result) {
            /* make sure caller's request is valid */
            if (flags & SHR_RES_BITMAP_ALLOC_ALIGN_ZERO) {
                /* alignment is against zero */
                offset = (*elem) % align;
            } else {
                /* alignment is against low */
                offset = ((*elem) - handle->low) % align;
            }
            if (offset != offs) {
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META("provided first element %d does not conform"
                           " to provided align %d + offset %d values"
                           " (actual offset = %d)\n"),
                           *elem,
                           align,
                           offset,
                           offs));
                result = BCM_E_PARAM;
            }
        } /* if (BCM_E_NONE == result) */
        if (BCM_E_NONE == result) {
            result = _shr_res_bitmap_check_all_sparse(handle,
                                                      pattern,
                                                      length,
                                                      repeats,
                                                      index);
            switch (result) {
            case BCM_E_FULL:
                if (flags & SHR_RES_BITMAP_ALLOC_REPLACE) {
                    result = BCM_E_NONE;
                } else {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META("proposed block %p base %d pattern %08X"
                               " length %d repeat %d already exists\n"),
                               (void*)handle,
                               *elem,
                               pattern,
                               length,
                               repeats));
                    result = BCM_E_RESOURCE;
                }
                break;
            case BCM_E_EMPTY:
                if (flags & SHR_RES_BITMAP_ALLOC_REPLACE) {
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META("proposed block %p base %d pattern %08X"
                               " length %d repeat %d does not exist\n"),
                               (void*)handle,
                               *elem,
                               pattern,
                               length,
                               repeats));
                    result = BCM_E_NOT_FOUND;
                } else {
                    result = BCM_E_NONE;
                }
                break;
            case BCM_E_EXISTS:
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META("proposed block %p base %d pattern %08X"
                           " length %d repeat %d would merge/expand"
                           " existing block(s)\n"),
                           (void*)handle,
                           *elem,
                           pattern,
                           length,
                           repeats));
                result = BCM_E_RESOURCE;
                break;
            default:
                /* should never see this */
                LOG_ERROR(BSL_LS_SOC_COMMON,
                          (BSL_META("unexpected result checking proposed block:"
                           " %d (%s)\n"),
                           result,
                           _SHR_ERRMSG(result)));
                if (BCM_E_NONE == result) {
                    result = BCM_E_INTERNAL;
                }
            } /* switch (result) */
        } /* if (BCM_E_NONE == result) */
        /* don't adjust last free or next alloc for WITH_ID */
    } else { /* if (flags & SHR_RES_ALLOC_WITH_ID) */
        if (flags & SHR_RES_BITMAP_ALLOC_ALIGN_ZERO) {
            /* alignment is against zero, not start of pool */
            offs += align - (handle->low % align);
        }
#if SHR_RES_BITMAP_SEARCH_RESUME
        /* see if it fits after the last free */
        index = (((handle->lastFree + align - 1) / align) * align) + offs;
        if (index + count < handle->count) {
            /* it might fit */
            for (repeat = 0, current = index; repeat < repeats; repeat++) {
                for (offset = 0; offset < length; offset++, current++) {
                    if (pattern & (1 << offset)) {
                        if (SHR_BITGET(handle->data, current)) {
                            result = BCM_E_EXISTS;
                            break;
                        }
                    }
                }
            }
        } else {
            result = BCM_E_EXISTS;
        }
        if (BCM_E_NONE == result) {
            /* looks good; adjust last free to miss this block */
            if (0 == offs) {
                handle->lastFree = index + count;
            }
        } else { /* if (BCM_E_NONE == result) */
            /* start searching after last successful alloc */
            index = (((handle->nextAlloc + align - 1) / align) * align) + offs;
            while (index <= handle->count - count) {
                while ((index <= handle->count - count) &&
                       SHR_BITGET(handle->data, index + first)) {
                    index += align;
                }
                if (index <= handle->count - count) {
                    /* have a candidate; see if block is big enough */
                    result = BCM_E_NONE;
                    for (repeat = 0, current = index;
                         repeat < repeats;
                         repeat++) {
                        for (offset = 0; offset < length; offset++, current++) {
                            if (pattern & (1 << offset)) {
                                if (SHR_BITGET(handle->data, current)) {
                                    /* an element is in use */
                                    result = BCM_E_EXISTS;
                                    /* skip to next alignment point */
                                    index += align;
                                    /* start comparing again */
                                    break;
                                } /* if (this element is in use) */
                            } /* if (this element is in the pattern) */
                        } /* for (length of the pattern) */
                    } /* for (number of repetitions of the pattern) */
                } /* if (index <= desc->count - count) */
                if (BCM_E_NONE == result) {
                    /* found a sufficient block */
                    break;
                }
            } /* while (index <= desc->count - count) */
            if (BCM_E_NONE != result) {
#endif /* SHR_RES_BITMAP_SEARCH_RESUME */
                /* no space, so try space before last successful alloc */
                index = offs;
                while (index < SHR_RES_BITMAP_FINAL_SEARCH_LIMIT) {
                    while ((index < SHR_RES_BITMAP_FINAL_SEARCH_LIMIT) &&
                           SHR_BITGET(handle->data, index + first)) {
                        index += align;
                    }
                    if (index < SHR_RES_BITMAP_FINAL_SEARCH_LIMIT) {
                        /* have a candidate; see if block is big enough */
                        result = BCM_E_NONE;
                        for (repeat = 0, current = index;
                             repeat < repeats;
                             repeat++) {
                            for (offset = 0;
                                 offset < length;
                                 offset++, current++) {
                                if (pattern & (1 << offset)) {
                                    if (SHR_BITGET(handle->data, current)) {
                                        /* an element is in use */
                                        result = BCM_E_EXISTS;
                                        /* skip to next alignment point */
                                        index += align;
                                        /* start comparing again */
                                        break;
                                    } /* if (this element is in use) */
                                } /* if (this element is in the pattern) */
                            } /* for (length of the pattern) */
                        } /* for (number of repetitions of the pattern) */
                    } /* if (index < end of possible space) */
                    if (BCM_E_NONE == result) {
                        /* found a sufficient block */
                        break;
                    }
                } /* while (index < data->next_alloc) */
#if SHR_RES_BITMAP_SEARCH_RESUME
            } /* if (BCM_E_NONE != result) */
#endif /* SHR_RES_BITMAP_SEARCH_RESUME */
            if (BCM_E_NONE != result) {
                /* still no space; give up */
                result = BCM_E_RESOURCE;
#if SHR_RES_BITMAP_SEARCH_RESUME
            } else {
                /* got some space; update next alloc  */
                handle->nextAlloc = index + count;
#endif /* SHR_RES_BITMAP_SEARCH_RESUME */
            }
#if SHR_RES_BITMAP_SEARCH_RESUME
        } /* if (BCM_E_NONE == result) */
#endif /* SHR_RES_BITMAP_SEARCH_RESUME */
    } /* if (flags & SHR_RES_ALLOC_WITH_ID) */
    if (BCM_E_NONE == result) {
        /* return the beginning element */
        *elem = index + handle->low;
        /* mark the block as in use */
        for (repeat = 0, count = 0, current = index;
             repeat < repeats;
             repeat++) {
            for (offset = 0; offset < length; offset++, current++) {
                if (pattern & (1 << offset)) {
                    SHR_BITSET(handle->data, current);
                    count++;
                } /* if (this element is in the pattern) */
            } /* for (length of the pattern) */
        } /* for (number of repetitions of the pattern) */
        if (0 == (flags & SHR_RES_BITMAP_ALLOC_REPLACE)) {
            /* only adjust accounting if not replacing existing block */
            handle->used += count;
        }
    } /* if (BCM_E_NONE == result) */
    return result;
}

int
shr_res_bitmap_free(shr_res_bitmap_handle_t handle,
                    int count,
                    int elem)
{
    int index;
    int offset;
    int result = BCM_E_NONE;

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to alloc from NULL descriptor\n")));
        return BCM_E_PARAM;
    }
    if (elem < handle->low) {
        /* not valid ID */
        result = BCM_E_PARAM;
    }
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("must free at least one element\n")));
        return BCM_E_PARAM;
    }

    index = elem - handle->low;
    if (index + count > handle->count) {
        /* not valid ID */
        result = BCM_E_PARAM;
    }
    if (BCM_E_NONE == result) {
        /* check whether the block is in use */
        for (offset = 0; offset < count; offset++) {
            if (!SHR_BITGET(handle->data, index + offset)) {
                /* not entirely in use */
                result = BCM_E_NOT_FOUND;
                break;
            }
        } /* for (offset = 0; offset < count; offset++) */
    } /* if (BCM_E_NONE == result) */
    if (BCM_E_NONE == result) {
        /* looks fine, so mark the block as free */
        SHR_BITCLR_RANGE(handle->data, index, count);
        handle->used -= count;
#if SHR_RES_BITMAP_SEARCH_RESUME
        handle->lastFree = index;
#endif /* SHR_RES_BITMAP_SEARCH_RESUME */
    } /* if (BCM_E_NONE == result) */
    /* return the result */
    return result;
}

int
shr_res_bitmap_free_sparse(shr_res_bitmap_handle_t handle,
                           uint32 pattern,
                           int length,
                           int repeats,
                           int elem)
{
    int index;
    int offset;
    int final;
    int result = BCM_E_NONE;
    uint32 pattern_mask;

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to alloc from NULL descriptor\n")));
        return BCM_E_PARAM;
    }
    if (elem < handle->low) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("first element is too low\n")));
        result = BCM_E_PARAM;
    }
    if (0 >= length) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("pattern must be at least one long\n")));
        return BCM_E_PARAM;
    }
    if (32 < length) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("pattern must not be longer than 32\n")));
        return BCM_E_PARAM;
    }
    if (0 >= repeats) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("must check at least one pattern\n")));
        return BCM_E_PARAM;
    }
    pattern_mask = (((uint32)1 << (length - 1)) - 1) | ((uint32)1 << (length - 1));
    if (0 == (pattern & pattern_mask)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("pattern must contain at least one element\n")));
        return BCM_E_PARAM;
    }
    if (pattern & ~pattern_mask) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("pattern must not contain unused bits\n")));
        return BCM_E_PARAM;
    }
    index = length;
    final = 0;
    do {
        index--;
        if (pattern & (1 << index)) {
            final = index;
            break;
        }
    } while (index > 0);
    final += (length * (repeats - 1));

    elem -= handle->low;
    if (elem + final > handle->count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("last element is too high\n")));
        result = BCM_E_PARAM;
    }
    if (BCM_E_NONE == result) {
        /* check whether the block is in use */
        result = _shr_res_bitmap_check_all_sparse(handle,
                                                  pattern,
                                                  length,
                                                  repeats,
                                                  elem);
        if (BCM_E_FULL == result) {
            /* block is fully in use */
#if SHR_RES_BITMAP_SEARCH_RESUME
            handle->lastFree = elem;
#endif /* SHR_RES_BITMAP_SEARCH_RESUME */
            for (index = 0; index < repeats; index++) {
                for (offset = 0; offset < length; offset++, elem++) {
                    if (pattern & (1 << offset)) {
                        SHR_BITCLR(handle->data, elem);
                        handle->used--;
                    } /* if (this element is in the pattern) */
                } /* for (pattern length) */
            } /* for (all repeats) */
            result = BCM_E_NONE;
        } else { /* if (BCM_E_FULL == result) */
            /* not entirely in use */
            result = BCM_E_NOT_FOUND;
        } /* if (BCM_E_FULL == result) */
    } /* if (BCM_E_NONE == result) */
    /* return the result */
    return result;
}

int
shr_res_bitmap_check(shr_res_bitmap_handle_t handle,
                     int count,
                     int elem)
{
    int index;
    int offset;
    int result = BCM_E_NONE;

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to alloc from NULL descriptor\n")));
        return BCM_E_PARAM;
    }
    if (elem < handle->low) {
        /* not valid ID */
        result = BCM_E_PARAM;
    }
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("must check at least one element\n")));
        return BCM_E_PARAM;
    }

    index = elem - handle->low;
    if (index + count > handle->count) {
        /* not valid ID */
        result = BCM_E_PARAM;
    }
    if (BCM_E_NONE == result) {
        /* check whether the block is in use */
        result = BCM_E_NOT_FOUND;
        for (offset = 0; offset < count; offset++) {
            if (SHR_BITGET(handle->data, index + offset)) {
                /* not entirely free */
                result = BCM_E_EXISTS;
                break;
            }
        } /* for (offset = 0; offset < count; offset++) */
    } /* if (BCM_E_NONE == result) */
    /* return the result */
    return result;
}

int
shr_res_bitmap_check_all(shr_res_bitmap_handle_t handle,
                         int count,
                         int elem)
{
    int index;
    int result = BCM_E_NONE;

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to alloc from NULL descriptor\n")));
        return BCM_E_PARAM;
    }
    if (elem < handle->low) {
        /* not valid ID */
        result = BCM_E_PARAM;
    }
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("must check at least one element\n")));
        return BCM_E_PARAM;
    }

    index = elem - handle->low;
    if (index + count > handle->count) {
        /* not valid ID */
        result = BCM_E_PARAM;
    }
    if (BCM_E_NONE == result) {
        result = _shr_res_bitmap_check_all(handle, count, index);
    }
    /* return the result */
    return result;
}

int
shr_res_bitmap_check_all_sparse(shr_res_bitmap_handle_t handle,
                                uint32 pattern,
                                int length,
                                int repeats,
                                int elem)
{
    int index;
    int final;
    int result = BCM_E_NONE;
    uint32 pattern_mask;

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to alloc from NULL descriptor\n")));
        return BCM_E_PARAM;
    }
    if (elem < handle->low) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("first element is too low\n")));
        result = BCM_E_PARAM;
    }
    if (0 >= length) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("pattern must be at least one long\n")));
        return BCM_E_PARAM;
    }
    if (32 < length) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("pattern must not be longer than 32\n")));
        return BCM_E_PARAM;
    }
    if (0 >= repeats) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("must check at least one pattern\n")));
        return BCM_E_PARAM;
    }
    pattern_mask = (((uint32)1 << (length - 1)) - 1) | ((uint32)1 << (length - 1));
    if (0 == (pattern & pattern_mask)) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("pattern must contain at least one element\n")));
        return BCM_E_PARAM;
    }
    if (pattern & ~pattern_mask) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("pattern must not contain unused bits\n")));
        return BCM_E_PARAM;
    }
    /* find the final set bit of the repeated pattern */
    index = length;
    final = 0;
    do {
        index--;
        if (pattern & (1 << index)) {
            final = index;
            break;
        }
    } while (index > 0);
    final += (length * (repeats - 1));

    elem -= handle->low;
    if (elem + final > handle->count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("last element is too high\n")));
        result = BCM_E_PARAM;
    }
    if (BCM_E_NONE == result) {
        result = _shr_res_bitmap_check_all_sparse(handle,
                                                  pattern,
                                                  length,
                                                  repeats,
                                                  elem);
    }
    /* return the result */
    return result;
}

int
shr_res_bitmap_dump(const shr_res_bitmap_handle_t handle)
{
    int result;
    int error = FALSE;
    int elemsUsed;
    int index;
    int offset;
    int elemOffset;
    int rowUse;

    if (!handle) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("must provide non-NULL handle\n")));
        return BCM_E_PARAM;
    }
    LOG_CLI((BSL_META("shr_res_bitmap at %p:\n"), (const void*)handle));
    LOG_CLI((BSL_META("  lowest ID     = %08X\n"), handle->low));
    LOG_CLI((BSL_META("  element count = %08X\n"), handle->count));
    LOG_CLI((BSL_META("  used elements = %08X\n"), handle->used));
#if SHR_RES_BITMAP_SEARCH_RESUME
    LOG_CLI((BSL_META("  last free     = %08X %s\n"),
             handle->lastFree,
             (error |= (handle->lastFree > handle->count))?"INVALID":""));
    LOG_CLI((BSL_META("  next alloc    = %08X %s\n"),
             handle->nextAlloc,
             (error |= (handle->nextAlloc > handle->count))?"INVALID":""));
#endif /* SHR_RES_BITMAP_SEARCH_RESUME */
    LOG_CLI((BSL_META("  element map:\n")));
    LOG_CLI((BSL_META("    1st Elem (index)    State of elements (1 = used)\n")));
    LOG_CLI((BSL_META("    -------- --------   --------------------------------------------------\n")));
    elemsUsed = 0;
    for (index = 0; index < handle->count; /* increment in loop */) {
        LOG_CLI((BSL_META("    %08X %08X   "), index + handle->low, index));
        elemOffset = 0;
        rowUse = 0;
        for (offset = 0; offset < 48; offset++) {
            if ((16 == offset) || (32 == offset)) {
                LOG_CLI((BSL_META(" ")));
            }
            if (index < handle->count) {
                if (SHR_BITGET(handle->data, index + elemOffset)) {
                    LOG_CLI((BSL_META("1")));
                    rowUse++;
                } else {
                    LOG_CLI((BSL_META("0")));
                }
                index++;
            }
        }
        LOG_CLI((BSL_META("\n")));
        elemsUsed += rowUse;
    } /* for all grains */
    LOG_CLI((BSL_META("  counted elems = %08X %s\n"),
             elemsUsed,
             (error |= (elemsUsed != handle->used))?"INVALID":""));
    if (error) {
        LOG_CLI((BSL_META("bitmap %p appears to be corrupt\n"),
                 (void*)handle));
        result = BCM_E_INTERNAL;
    } else {
        result = BCM_E_NONE;
    }
    return result;
}


