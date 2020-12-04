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
#include <shared/bitop.h>
#include <bcm/error.h>
#include <soc/cm.h>
#include <shared/shr_res_tag_bitmap.h>

/*
 *  This is a fairly brute-force implementation of bitmap, with minimal
 *  optimisations or improvements.  It could probably be enhanced somewhat by
 *  some subtleties, such as checking whether a SHR_BITDCL is all ones before
 *  scanning individual bits when looking for free space.
 */

/*
 *  Macros and other things that change according to settings...
 */
#if SHR_RES_TAG_BITMAP_SEARCH_RESUME
#define SHR_RES_TAG_BITMAP_FINAL_SEARCH_LIMIT (handle->nextAlloc)
#else /* SHR_RES_TAG_BITMAP_SEARCH_RESUME */
#define SHR_RES_TAG_BITMAP_FINAL_SEARCH_LIMIT (handle->count - count)
#endif /* SHR_RES_TAG_BITMAP_SEARCH_RESUME */


int
shr_res_tag_bitmap_create(shr_res_tag_bitmap_handle_t *handle,
                          int low_id,
                          int count,
                          int grain_size,
                          int tag_size)
{
    shr_res_tag_bitmap_handle_t desc;
    int size;

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("obligatory out argument must not be NULL\n")));
        return _SHR_E_PARAM;
    }
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("must have a positive number of elements\n")));
        return _SHR_E_PARAM;
    }
    if (0 > tag_size) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("tag size must not be negative\n")));
        return _SHR_E_PARAM;
    } else if (0 == tag_size) {
        LOG_WARN(BSL_LS_SOC_COMMON,
                 (BSL_META("tag size zero is not useful, but okay...\n")));
    }
    if (0 >= grain_size) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("must have at least one element per grain\n")));
        return _SHR_E_PARAM;
    }
    if (count % grain_size) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("count %d is not an integral number of grains %d\n"),
                   count,
                   grain_size));
        return _SHR_E_PARAM;
    }

    /* alloc memory for descriptor & data */
    /* an extra tag (zeroed) for when tag = NULL */
    size = (sizeof(*desc) +
            (SHR_BITALLOCSIZE(count) - sizeof(SHR_BITDCL)) +
            (tag_size * ((count / grain_size) + 1)));
    desc = sal_alloc(size, "tagged bitmap resource data");
    if (!desc) {
        /* alloc failed */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to allocate %u bytes for data\n"),
                   size));
        return _SHR_E_MEMORY;
    }
    /* init descriptor and data */
    sal_memset(desc, 0x00, size);
    desc->low = low_id;
    desc->count = count;
    desc->grainSize = grain_size;
    desc->tagSize = tag_size;
    desc->tagData = (uint8 *)(&(desc->data[_SHR_BITDCLSIZE(count)]));
    *handle = desc;
    /* all's well if we got here */
    return _SHR_E_NONE;
}

int
shr_res_tag_bitmap_destroy(shr_res_tag_bitmap_handle_t handle)
{
    if (handle) {
        sal_free(handle);
        return _SHR_E_NONE;
    } else {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to free NULL handle\n")));
        return _SHR_E_PARAM;
    }
}

/*
 *  An end that is grain-aligned is considered to match (this means if it
 *  starts at the initial element of a grain or ends at the final element of a
 *  grain).  An end grain that contains no allocated elements is considered to
 *  match.  Since this is called only when checking whether it is okay to
 *  allocate a block, it assumes that all grains between the first and last
 *  grain are free (otherwise the block would have already been rejected).
 *
 *  This returns 0 if both end grains match, -1 if only the first end grain
 *  does not match, or +1 if the last end grain does not match.  Note that if
 *  both end grains do not match, it still returns +1.  This difference is so
 *  the caller can shortcut to the next available grain if the last end grain
 *  does not match (so this also indicates there will not be another sufficient
 *  free block until at least the grain after the last end grain, allowing some
 *  limtied shortcut to the scanning.
 *
 *  If the tag size is zero, nothing to do here.
 */
static signed int
_shr_res_tag_bitmap_tag_check(shr_res_tag_bitmap_handle_t handle,
                              int elemIndex,
                              int elemCount,
                              const void *tag)
{
    int index;
    int offset;
    int result = 0;
    SHR_BITDCL temp;

    if (handle->tagSize) {
        offset = elemIndex % handle->grainSize;
        if (offset) {
            /* first element not first element of a grain */
            temp = 0;
            SHR_BITTEST_RANGE(handle->data,
                              elemIndex - offset,
                              handle->grainSize,
                              temp);
            if (temp) {
                /* the grain is used by at least one other block */
                index = (elemIndex / handle->grainSize);
                if (sal_memcmp(&(handle->tagData[index * handle->tagSize]),
                               tag,
                               handle->tagSize)) {
                    /* the existing key is not equal to the new key */
                    result = -1;
                }
            }
        }
        offset = (elemIndex + elemCount - 1) % handle->grainSize;
        if (offset != (handle->grainSize - 1)) {
            /* last element not last element of a grain */
            temp = 0;
            SHR_BITTEST_RANGE(handle->data,
                              (elemIndex + elemCount - 1) - offset,
                              handle->grainSize,
                              temp);
            if (temp) {
                /* the grain is used by at least one other block */
                index = (elemIndex + elemCount - 1) / handle->grainSize;
                if (sal_memcmp(&(handle->tagData[index * handle->tagSize]),
                               tag,
                               handle->tagSize)) {
                    /* the existing key is not equal to the new key */
                    result = 1;
                }
            }
        }
    }
    return result;
}

/*
 *  Scan a range as per 'check all'.  Basically this needs to check whether a
 *  range is: all free, all in-use (with the same tag), all in-use (but
 *  different tags), or a mix of free and in-use (same or different tags).
 */
static int
_shr_res_tag_bitmap_check_all_tag(shr_res_tag_bitmap_handle_t handle,
                                  const void *tag,
                                  int count,
                                  int index)
{
    int tagbase;
    int offset;
    int freed = 0;
    int inuse = 0;
    int result = _SHR_E_NONE;

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
        result = _SHR_E_FULL;
        /* now verify that all involved grains have same tag */
        if (0 < handle->tagSize) {
            tagbase = index / handle->grainSize;
            for (offset = 0;
                 offset < count;
                 offset += handle->grainSize, tagbase++) {
                if (sal_memcmp(tag,
                               &(handle->tagData[tagbase * handle->tagSize]),
                               handle->tagSize)) {
                    /* tags are not equal, so different blocks */
                    result = _SHR_E_CONFIG;
                }
            } /* for (all grains in the block) */
        } /* if (0 < handle->tagSize) */
    } else if (freed == count) {
        /* block is entirely free */
        result = _SHR_E_EMPTY;
    } else {
        /* block is partially free and partially in use */
        result = _SHR_E_EXISTS;
    }
    return result;
}

/*
 *  Ensure that all grains in an alloc are tagged accordingly.
 */

static void
_shr_res_tag_bitmap_tag_set(shr_res_tag_bitmap_handle_t handle,
                            const void* tag,
                            int elemIndex,
                            int elemCount)
{
    int index;
    int count;
    int offset;

    if (handle->tagSize) {
        index = elemIndex / handle->grainSize;
        count = (elemCount + handle->grainSize - 1) / handle->grainSize;
        for (offset = 0; offset < count; offset++) {
            sal_memcpy(&(handle->tagData[(index + offset) * handle->tagSize]),
                       tag,
                       handle->tagSize);
        }
    }
}

int
shr_res_tag_bitmap_alloc_tag(shr_res_tag_bitmap_handle_t handle,
                             uint32 flags,
                             const void *tag,
                             int count,
                             int *elem)
{
    int index;
    int offset;
    int result = _SHR_E_NONE;
    SHR_BITDCL temp;

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to alloc from NULL descriptor\n")));
        return _SHR_E_PARAM;
    }
    if (!elem) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("obligatory in/out argument must not be NULL\n")));
        return _SHR_E_PARAM;
    }
    if (count + handle->used > handle->count) {
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META("not enough free elements (%d) to satisfy request"
                   " for %d element block\n"),
                   handle->count - handle->used,
                   count));
        return _SHR_E_RESOURCE;
    }
    if (!tag) {
        /* if NULL tag, use default (all zeroes) value instead */
        tag = &(handle->tagData[(handle->count / handle->grainSize) *
                                handle->tagSize]);
    }

    if (flags & SHR_RES_TAG_BITMAP_ALLOC_WITH_ID) {
        /* WITH_ID, so only try the specifically requested block */
        if (*elem < handle->low) {
            /* not valid ID */
            result = _SHR_E_PARAM;
        }
        index = *elem - handle->low;
        if (index + count > handle->count) {
            /* not valid ID */
            result = _SHR_E_PARAM;
        }
        if (_SHR_E_NONE == result) {
            if (flags & SHR_RES_TAG_BITMAP_ALLOC_REPLACE) {
                /* trying to replace */
                result = _shr_res_tag_bitmap_check_all_tag(handle,
                                                           tag,
                                                           count,
                                                           index);
                switch (result) {
                case _SHR_E_FULL:
                    result = _SHR_E_NONE;
                    break;
                case _SHR_E_EMPTY:
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META("proposed block %p base %d count %d"
                                   " does not exist\n"),
                                   (void*)handle,
                                   *elem,
                                   count));
                        result = _SHR_E_NOT_FOUND;
                    break;
                case _SHR_E_EXISTS:
                case _SHR_E_CONFIG:
                    /*
                     *  In this context, the result _SHR_E_EXIST means that the
                     *  requested range is not either fully allocated or fully
                     *  free, while _SHR_E_CONFIG means that more than one block
                     *  exists in the range.  Both cases imply a merge or
                     *  resize, so both are handled here.
                     */
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META("proposed block %p base %d count %d"
                               " would merge/expand existing"
                               " block(s)\n"),
                               (void*)handle,
                               *elem,
                               count));
                    result = _SHR_E_RESOURCE;
                    break;
                default:
                    /* should never see this */
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META("unexpected result checking proposed"
                               " block: %d (%s)\n"),
                               result,
                               _SHR_ERRMSG(result)));
                    if (_SHR_E_NONE == result) {
                        result = _SHR_E_INTERNAL;
                    }
                }
            } else { /* if (flags & SHR_RES_TAG_BITMAP_ALLOC_REPLACE) */
                /* check whether the block is free */
                temp = 0;
                SHR_BITTEST_RANGE(handle->data, index, count, temp);
                if (temp) {
                    /* in use; can't do WITH_ID alloc of this block */
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META("proposed block %p base %d count %d"
                               " tag %p would overlap existing"
                               " block(s)\n"),
                               (void*)handle,
                               *elem,
                               count,
                               tag));
                    result = _SHR_E_RESOURCE;
                }
                if (_SHR_E_NONE == result) {
                    if (_shr_res_tag_bitmap_tag_check(handle, index, count, tag)) {
                        /* grain at one end or other has mismatching tag */
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META("proposed block %p base %d count %d"
                                   " tag %p would share end elements"
                                   " with another block that has a"
                                   " different tag value"),
                                   (void*)handle,
                                   *elem,
                                   count,
                                   tag));
                        result = _SHR_E_RESOURCE;
                    }
                }
            } /* if (flags & SHR_RES_TAG_BITMAP_ALLOC_REPLACE) */
        } /* if (_SHR_E_NONE == result) */
        /* don't adjust last free or next alloc for WITH_ID */
    } else { /* if (flags & SHR_RES_ALLOC_WITH_ID) */
#if SHR_RES_TAG_BITMAP_SEARCH_RESUME
        /* see if there are enough elements after last free */
        index = handle->lastFree;
        if (index + count < handle->count) {
            /* it might fit */
            temp = 0;
            SHR_BITTEST_RANGE(handle->data, index, count, temp);
            if (temp) {
                /* ran into non-free elements in the proposed block */
                result = _SHR_E_EXISTS;
            } else {
                offset = _shr_res_tag_bitmap_tag_check(handle,
                                                       index,
                                                       count,
                                                       tag);
                if (offset) {
                    /* at least one end had a different tag */
                    result = _SHR_E_EXISTS;
                }
            }
        } else {
            /* ran out of elements before end of proposed block */
            result = _SHR_E_EXISTS;
        }
        if (_SHR_E_NONE == result) {
            /* looks good; adjust last free to miss this block */
            handle->lastFree = index + count;
        } else { /* if (_SHR_E_NONE == result) */
            /* start searching after last successful alloc */
            index = handle->nextAlloc;
            while (index <= handle->count - count) {
                while (SHR_BITGET(handle->data, index) &&
                       (index > handle->count - count)) {
                    index++;
                }
                if (index <= handle->count - count) {
                    /* have a candidate; see if block is big enough */
                    result = _SHR_E_NONE;
                    for (offset = 0; offset < count; offset++) {
                        if (SHR_BITGET(handle->data, index + offset)) {
                            /* not big enough; skip this block */
                            result = _SHR_E_EXISTS;
                            index += offset + 1;
                            break;
                        }
                    } /* for (offset = 0; offset < count; offset++) */
                } /* if (index <= desc->count - count) */
                if (_SHR_E_NONE == result) {
                    /* found a sufficient block, check tags */
                    offset = _shr_res_tag_bitmap_tag_check(handle,
                                                           index,
                                                           count,
                                                           tag);
                    if (offset) {
                        /* at least one end had a different tag */
                        result = _SHR_E_EXISTS;
                        if (0 < offset) {
                            /* ending had wrong tag; skip to end */
                            index += (count - 1);
                        }
                        /* seek to beginning of next grain */
                        index += (handle->grainSize - (index %
                                                       handle->grainSize));
                    }
                }
                if (_SHR_E_NONE == result) {
                    /* found a sufficient block with matching end tags */
                    break;
                }
            } /* while (index <= desc->count - count) */
            if ((_SHR_E_NONE != result) && (0 < handle->nextAlloc)) {
                /* no space, so try space before last successful alloc */
#else /* SHR_RES_TAG_BITMAP_SEARCH_RESUME */
                result = _SHR_E_RESOURCE;
#endif /* SHR_RES_TAG_BITMAP_SEARCH_RESUME */
                index = 0;
                while (index < SHR_RES_TAG_BITMAP_FINAL_SEARCH_LIMIT) {
                    while (SHR_BITGET(handle->data, index) &&
                           (index < SHR_RES_TAG_BITMAP_FINAL_SEARCH_LIMIT)) {
                        index++;
                    }
                    if (index < SHR_RES_TAG_BITMAP_FINAL_SEARCH_LIMIT) {
                        /* have a candidate; see if block is big enough */
                        result = _SHR_E_NONE;
                        for (offset = 0; offset < count; offset++) {
                            if (SHR_BITGET(handle->data, index + offset)) {
                                /* not big enough; skip this block */
                                result = _SHR_E_EXISTS;
                                index += offset + 1;
                                break;
                            }
                        } /* for (offset = 0; offset < count; offset++) */
                    } /* if (index < data->next_alloc) */
                    if (_SHR_E_NONE == result) {
                        /* found a sufficient block, check tags */
                        offset = _shr_res_tag_bitmap_tag_check(handle,
                                                               index,
                                                               count,
                                                               tag);
                        if (offset) {
                            /* at least one end had a different tag */
                            result = _SHR_E_EXISTS;
                            if (0 < offset) {
                                /* ending had wrong tag; skip to end */
                                index += (count - 1);
                            }
                            /* seek to beginning of next grain */
                            index += (handle->grainSize - (index %
                                                           handle->grainSize));
                        }
                    }
                    if (_SHR_E_NONE == result) {
                        /* found a sufficient block with matching end tags */
                        break;
                    }
                } /* while (index < data->next_alloc) */
#if SHR_RES_TAG_BITMAP_SEARCH_RESUME
            } /* if (_SHR_E_NONE != result) */
#endif /* SHR_RES_TAG_BITMAP_SEARCH_RESUME */
            if (_SHR_E_NONE != result) {
                /* still no space; give up */
                result = _SHR_E_RESOURCE;
#if SHR_RES_TAG_BITMAP_SEARCH_RESUME
            } else { /* if (_SHR_E_NONE != result) */
                /* got some space; update next alloc  */
                handle->nextAlloc = index + count;
#endif /* SHR_RES_TAG_BITMAP_SEARCH_RESUME */
            } /* if (_SHR_E_NONE != result) */
#if SHR_RES_TAG_BITMAP_SEARCH_RESUME
        } /* if (_SHR_E_NONE == result) */
#endif /* SHR_RES_TAG_BITMAP_SEARCH_RESUME */
    } /* if (flags & SHR_RES_ALLOC_WITH_ID) */
    if (_SHR_E_NONE == result) {
        /* set the tag for all grains involved */
        _shr_res_tag_bitmap_tag_set(handle, tag, index, count);
        /* mark the block as in-use */
        SHR_BITSET_RANGE(handle->data, index, count);
        if (0 == (flags & SHR_RES_TAG_BITMAP_ALLOC_REPLACE)) {
            handle->used += count;
        }
        /* return the beginning element */
        *elem = index + handle->low;
    } /* if (_SHR_E_NONE == result) */
    return result;
}

int
shr_res_tag_bitmap_alloc(shr_res_tag_bitmap_handle_t handle,
                         uint32 flags,
                         int count,
                         int *elem)
{
    return shr_res_tag_bitmap_alloc_tag(handle, flags, NULL, count, elem);
}

int
shr_res_tag_bitmap_alloc_align_tag(shr_res_tag_bitmap_handle_t handle,
                                   uint32 flags,
                                   int align,
                                   int offs,
                                   const void *tag,
                                   int count,
                                   int *elem)
{
    int index;
    int offset;
    int result = _SHR_E_NONE;
    SHR_BITDCL temp;

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to alloc from NULL descriptor\n")));
        return _SHR_E_PARAM;
    }
    if (!elem) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("obligatory in/out argument must not be NULL\n")));
        return _SHR_E_PARAM;
    }
    if (count + handle->used > handle->count) {
        LOG_DEBUG(BSL_LS_SOC_COMMON,
                  (BSL_META("not enough free elements (%d) to satisfy request"
                   " for %d element block\n"),
                   handle->count - handle->used,
                   count));
        return _SHR_E_RESOURCE;
    }
    if (!tag) {
        /* if NULL tag, use default (all zeroes) value instead */
        tag = &(handle->tagData[(handle->count / handle->grainSize) *
                                handle->tagSize]);
    }

    if (flags & SHR_RES_TAG_BITMAP_ALLOC_WITH_ID) {
        /* WITH_ID, so only try the specifically requested block */
        if (*elem < handle->low) {
            /* not valid ID */
            result = _SHR_E_PARAM;
        }
        index = *elem - handle->low;
        if (index + count > handle->count) {
            /* not valid ID */
            result = _SHR_E_PARAM;
        }
        if (_SHR_E_NONE == result) {
            /* make sure caller's request is valid */
            if (flags & SHR_RES_TAG_BITMAP_ALLOC_ALIGN_ZERO) {
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
                result = _SHR_E_PARAM;
            }
        }
        if (_SHR_E_NONE == result) {
            if (flags & SHR_RES_TAG_BITMAP_ALLOC_REPLACE) {
                /* trying to replace */
                result = _shr_res_tag_bitmap_check_all_tag(handle,
                                                           tag,
                                                           count,
                                                           index);
                switch (result) {
                case _SHR_E_FULL:
                    result = _SHR_E_NONE;
                    break;
                case _SHR_E_EMPTY:
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META("proposed block %p base %d count %d"
                                   " does not exist\n"),
                                   (void*)handle,
                                   *elem,
                                   count));
                        result = _SHR_E_NOT_FOUND;
                    break;
                case _SHR_E_EXISTS:
                case _SHR_E_CONFIG:
                    /*
                     *  In this context, the result _SHR_E_EXIST means that the
                     *  requested range is not either fully allocated or fully
                     *  free, while _SHR_E_CONFIG means that more than one block
                     *  exists in the range.  Both cases imply a merge or
                     *  resize, so both are handled here.
                     */
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META("proposed block %p base %d count %d"
                               " would merge/expand existing"
                               " block(s)\n"),
                               (void*)handle,
                               *elem,
                               count));
                    result = _SHR_E_RESOURCE;
                    break;
                default:
                    /* should never see this */
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META("unexpected result checking proposed"
                               " block:  %d (%s)\n"),
                               result,
                               _SHR_ERRMSG(result)));
                    if (_SHR_E_NONE == result) {
                        result = _SHR_E_INTERNAL;
                    }
                }
            } else { /* if (flags & SHR_RES_TAG_BITMAP_ALLOC_REPLACE) */
                /* check whether the block is free */
                temp = 0;
                SHR_BITTEST_RANGE(handle->data, index, count, temp);
                if (temp) {
                    /* in use; can't do WITH_ID alloc of this block */
                    LOG_ERROR(BSL_LS_SOC_COMMON,
                              (BSL_META("proposed block %p base %d count %d"
                               " tag %p would overlap existing"
                               " block(s)\n"),
                               (void*)handle,
                               *elem,
                               count,
                               tag));
                    result = _SHR_E_RESOURCE;
                }
                if (_SHR_E_NONE == result) {
                    if (_shr_res_tag_bitmap_tag_check(handle, index, count, tag)) {
                        /* grain at one end or other has mismatching tag */
                        LOG_ERROR(BSL_LS_SOC_COMMON,
                                  (BSL_META("proposed block %p base %d count %d"
                                   " tag %p would share end elements"
                                   " with another block that has a"
                                   " different tag value"),
                                   (void*)handle,
                                   *elem,
                                   count,
                                   tag));
                        result = _SHR_E_RESOURCE;
                    }
                }
            } /* if (flags & SHR_RES_TAG_BITMAP_ALLOC_REPLACE) */
        } /* if (_SHR_E_NONE == result) */
        /* don't adjust last free or next alloc for WITH_ID */
    } else { /* if (flags & SHR_RES_ALLOC_WITH_ID) */
        if (flags & SHR_RES_TAG_BITMAP_ALLOC_ALIGN_ZERO) {
            /* alignment is against zero, not start of pool */
            offs += align - (handle->low % align);
        }
#if SHR_RES_TAG_BITMAP_SEARCH_RESUME
        /* see if there are enough elements after last free */
        index = (((handle->lastFree + align - 1) / align) * align) + offs;
        if (index + count < handle->count) {
            /* it might fit */
            temp = 0;
            SHR_BITTEST_RANGE(handle->data, index, count, temp);
            if (temp) {
                /* ran into non-free elements in the proposed block */
                result = _SHR_E_EXISTS;
            } else {
                offset = _shr_res_tag_bitmap_tag_check(handle,
                                                       index,
                                                       count,
                                                       tag);
                if (offset) {
                    /* at least one end had a different tag */
                    result = _SHR_E_EXISTS;
                }
            }
        } else {
            result = _SHR_E_EXISTS;
        }
        if (_SHR_E_NONE == result) {
            /* looks good; adjust last free to miss this block */
            handle->lastFree = index + count;
        } else { /* if (_SHR_E_NONE == result) */
            /* start searching after last successful alloc */
            index = (((handle->nextAlloc + align - 1) / align) * align) + offs;
            while (index <= handle->count - count) {
                while (SHR_BITGET(handle->data, index) &&
                       (index > handle->count - count)) {
                    index += align;
                }
                if (index <= handle->count - count) {
                    /* have a candidate; see if block is big enough */
                    result = _SHR_E_NONE;
                    for (offset = 0; offset < count; offset++) {
                        if (SHR_BITGET(handle->data, index + offset)) {
                            /* not big enough; skip this block */
                            result = _SHR_E_EXISTS;
                            /* realign after this block */
                            index = (((index + offset + align) / align) * align) + offs;
                            break;
                        }
                    } /* for (offset = 0; offset < count; offset++) */
                } /* if (index <= desc->count - count) */
                if (_SHR_E_NONE == result) {
                    /* found a sufficient block, check tags */
                    offset = _shr_res_tag_bitmap_tag_check(handle,
                                                           index,
                                                           count,
                                                           tag);
                    if (offset) {
                        /* at least one end had a different tag */
                        result = _SHR_E_EXISTS;
                        if (0 < offset) {
                            /* ending had wrong tag; skip to end */
                            index += (count - 1);
                        }
                        /* seek to beginning of next grain */
                        index += (handle->grainSize - (index %
                                                       handle->grainSize));
                        /* realign after this block */
                        index = (((index + align - 1) / align) * align) + offs;
                    }
                }
                if (_SHR_E_NONE == result) {
                    /* found a sufficient block with matching end tags */
                    break;
                }
            } /* while (index <= desc->count - count) */
            if (_SHR_E_NONE != result) {
                /* no space, so try space before last successful alloc */
#endif /* SHR_RES_TAG_BITMAP_SEARCH_RESUME */
                index = offs;
                while (index < SHR_RES_TAG_BITMAP_FINAL_SEARCH_LIMIT) {
                    while (SHR_BITGET(handle->data, index) &&
                           (index < SHR_RES_TAG_BITMAP_FINAL_SEARCH_LIMIT)) {
                        index += align;
                    }
                    if (index < SHR_RES_TAG_BITMAP_FINAL_SEARCH_LIMIT) {
                        /* have a candidate; see if block is big enough */
                        result = _SHR_E_NONE;
                        for (offset = 0; offset < count; offset++) {
                            if (SHR_BITGET(handle->data, index + offset)) {
                                /* not big enough; skip this block */
                                result = _SHR_E_EXISTS;
                                index = (((index + offset + align) / align) * align) + offs;
                                break;
                            }
                        } /* for (offset = 0; offset < count; offset++) */
                    } /* if (index < data->next_alloc) */
                    if (_SHR_E_NONE == result) {
                        /* found a sufficient block, check tags */
                        offset = _shr_res_tag_bitmap_tag_check(handle,
                                                               index,
                                                               count,
                                                               tag);
                        if (offset) {
                            /* at least one end had a different tag */
                            result = _SHR_E_EXISTS;
                            if (0 < offset) {
                                /* ending had wrong tag; skip to end */
                                index += (count - 1);
                            }
                            /* seek to beginning of next grain */
                            index += (handle->grainSize - (index %
                                                           handle->grainSize));
                            /* realign after this block */
                            index = (((index + align - 1) / align) * align) + offs;
                        }
                    }
                    if (_SHR_E_NONE == result) {
                        /* found a sufficient block with matching end tags */
                        break;
                    }
                } /* while (index < data->next_alloc) */
#if SHR_RES_TAG_BITMAP_SEARCH_RESUME
            } /* if (_SHR_E_NONE != result) */
#endif /* SHR_RES_TAG_BITMAP_SEARCH_RESUME */
            if (_SHR_E_NONE != result) {
                /* still no space; give up */
                result = _SHR_E_RESOURCE;
#if SHR_RES_TAG_BITMAP_SEARCH_RESUME
            } else {
                /* got some space; update next alloc  */
                handle->nextAlloc = index + count;
#endif /* SHR_RES_TAG_BITMAP_SEARCH_RESUME */
            }
#if SHR_RES_TAG_BITMAP_SEARCH_RESUME
        } /* if (_SHR_E_NONE == result) */
#endif /* SHR_RES_TAG_BITMAP_SEARCH_RESUME */
    } /* if (flags & SHR_RES_ALLOC_WITH_ID) */
    if (_SHR_E_NONE == result) {
        /* set the tag for all grains involved */
        _shr_res_tag_bitmap_tag_set(handle, tag, index, count);
        /* mark the block as in-use */
        SHR_BITSET_RANGE(handle->data, index, count);
        if (0 == (flags & SHR_RES_TAG_BITMAP_ALLOC_REPLACE)) {
            handle->used += count;
        }
        /* return the beginning element */
        *elem = index + handle->low;
    } /* if (_SHR_E_NONE == result) */
    return result;
}

int
shr_res_tag_bitmap_alloc_align(shr_res_tag_bitmap_handle_t handle,
                               uint32 flags,
                               int align,
                               int offs,
                               int count,
                               int *elem)
{
    return shr_res_tag_bitmap_alloc_align_tag(handle,
                                              flags,
                                              align,
                                              offs,
                                              NULL,
                                              count,
                                              elem);
}

int
shr_res_tag_bitmap_free(shr_res_tag_bitmap_handle_t handle,
                        int count,
                        int elem)
{
    int index;
    int offset;
    int result = _SHR_E_NONE;

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to alloc from NULL descriptor\n")));
        return _SHR_E_PARAM;
    }
    if (elem < handle->low) {
        /* not valid ID */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("invalid staring element %d\n"),
                   elem));
        return _SHR_E_PARAM;
    }
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("must free a positive number of elements\n")));
        return _SHR_E_PARAM;
    }
    index = elem - handle->low;
    if (index + count > handle->count) {
        /* not valid ID */
        result = _SHR_E_PARAM;
    }
    if (_SHR_E_NONE == result) {
        /* check whether the block is in use */
        for (offset = 0; offset < count; offset++) {
            if (!SHR_BITGET(handle->data, index + offset)) {
                /* not entirely in use */
                result = _SHR_E_NOT_FOUND;
                break;
            }
        } /* for (offset = 0; offset < count; offset++) */
    } /* if (_SHR_E_NONE == result) */
    if (_SHR_E_NONE == result) {
        /* looks fine, so mark the block as free */
        SHR_BITCLR_RANGE(handle->data, index, count);
        handle->used -= count;
#if SHR_RES_TAG_BITMAP_SEARCH_RESUME
        handle->lastFree = index;
#endif /* SHR_RES_TAG_BITMAP_SEARCH_RESUME */
    } /* if (_SHR_E_NONE == result) */
    /* return the result */
    return result;
}

int
shr_res_tag_bitmap_check(shr_res_tag_bitmap_handle_t handle,
                         int count,
                         int elem)
{
    int index;
    int offset;
    int result = _SHR_E_NONE;

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to alloc from NULL descriptor\n")));
        return _SHR_E_PARAM;
    }
    if (elem < handle->low) {
        /* not valid ID */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("invalid staring element %d\n"),
                   elem));
        return _SHR_E_PARAM;
    }
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("must check a positive number of elements\n")));
        return _SHR_E_PARAM;
    }
    index = elem - handle->low;
    if (index + count > handle->count) {
        /* not valid ID */
        result = _SHR_E_PARAM;
    }
    if (_SHR_E_NONE == result) {
        /* check whether the block is in use */
        result = _SHR_E_NOT_FOUND;
        for (offset = 0; offset < count; offset++) {
            if (SHR_BITGET(handle->data, index + offset)) {
                /* not entirely free */
                result = _SHR_E_EXISTS;
                break;
            }
        } /* for (offset = 0; offset < count; offset++) */
    } /* if (_SHR_E_NONE == result) */
    /* return the result */
    return result;
}

int
shr_res_tag_bitmap_check_all_tag(shr_res_tag_bitmap_handle_t handle,
                                 const void *tag,
                                 int count,
                                 int elem)
{
    int index;
    int result = _SHR_E_NONE;

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to check from NULL descriptor\n")));
        return _SHR_E_PARAM;
    }
    if (elem < handle->low) {
        /* not valid ID */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("invalid staring element %d\n"),
                   elem));
        return _SHR_E_PARAM;
    }
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("must check a positive number of elements\n")));
        return _SHR_E_PARAM;
    }
    if (!tag) {
        /* if NULL tag, use default (all zeroes) value instead */
        tag = &(handle->tagData[(handle->count / handle->grainSize) *
                                handle->tagSize]);
    }
    index = elem - handle->low;
    if (index + count > handle->count) {
        /* not valid ID */
        result = _SHR_E_PARAM;
    }
    if (_SHR_E_NONE == result) {
        result = _shr_res_tag_bitmap_check_all_tag(handle, tag, count, index);
    }
    /* return the result */
    return result;
}

int
shr_res_tag_bitmap_check_all(shr_res_tag_bitmap_handle_t handle,
                             int count,
                             int elem)
{
    int index;
    int result = _SHR_E_NONE;
    const void* tag;

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("unable to check from NULL descriptor\n")));
        return _SHR_E_PARAM;
    }
    if (elem < handle->low) {
        /* not valid ID */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("invalid staring element %d\n"),
                   elem));
        return _SHR_E_PARAM;
    }
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("must check a positive number of elements\n")));
        return _SHR_E_PARAM;
    }
    index = elem - handle->low;
    if (index + count > handle->count) {
        /* not valid ID */
        result = _SHR_E_PARAM;
    }
    if (_SHR_E_NONE == result) {
        /* here we assume the tag from the first grain in the range */
        tag = &(handle->tagData[index / handle->grainSize]);
        result = _shr_res_tag_bitmap_check_all_tag(handle, tag, count, index);
    }
    /* return the result */
    return result;
}

int
shr_res_tag_bitmap_dump(const shr_res_tag_bitmap_handle_t handle)
{
    int result;
    int error = FALSE;
    int elemsUsed;
    int grainsFree;
    int grainsFull;
    int grainUse;
    int index;
    int offset;
    int elemOffset;
    int tagOffset;
    int tagStart;

    if (!handle) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                  (BSL_META("must provide non-NULL handle\n")));
        return _SHR_E_PARAM;
    }
    LOG_CLI((BSL_META("shr_res_tag_bitmap at %p:\n"), (const void*)handle));
    LOG_CLI((BSL_META("  lowest ID     = %08X\n"), handle->low));
    LOG_CLI((BSL_META("  element count = %08X\n"), handle->count));
    LOG_CLI((BSL_META("  used elements = %08X\n"), handle->used));
    LOG_CLI((BSL_META("  grain size    = %08X elements\n"), handle->grainSize));
    LOG_CLI((BSL_META("  tag size      = %08X bytes\n"), handle->tagSize));
#if SHR_RES_TAG_BITMAP_SEARCH_RESUME
    LOG_CLI((BSL_META("  last free     = %08X %s\n"),
             handle->lastFree,
             (error |= (handle->lastFree > handle->count))?"INVALID":""));
    LOG_CLI((BSL_META("  next alloc    = %08X %s\n"),
             handle->nextAlloc,
             (error |= (handle->nextAlloc > handle->count))?"INVALID":""));
#endif /* SHR_RES_TAG_BITMAP_SEARCH_RESUME */
    LOG_CLI((BSL_META("  element map:\n")));
    LOG_CLI((BSL_META("    1st Elem (index)    State of elements (1 = used)        Tag bytes\n")));
    LOG_CLI((BSL_META("    -------- --------   --------------------------------    ----------------\n")));
    elemsUsed = 0;
    grainsFree = 0;
    grainsFull = 0;
    for (index = 0; index < handle->count; index += handle->grainSize) {
        LOG_CLI((BSL_META("    %08X %08X   "), index + handle->low, index));
        elemOffset = 0;
        tagOffset = 0;
        grainUse = 0;
        while ((elemOffset < handle->grainSize) &&
               (tagOffset < handle->tagSize)) {
            for (offset = 0; offset < 32; offset++) {
                if (elemOffset < handle->grainSize) {
                    if (SHR_BITGET(handle->data, index + elemOffset)) {
                        LOG_CLI((BSL_META("1")));
                        grainUse++;
                    } else {
                        LOG_CLI((BSL_META("0")));
                    }
                } else {
                    LOG_CLI((BSL_META(" ")));
                }
                elemOffset++;
            } /* for (offset = 0; offset < 32; offset++) */
            LOG_CLI((BSL_META("    ")));
            tagStart = (index / handle->grainSize) * handle->tagSize;
            for (offset = 0; offset < 8; offset++) {
                if (tagOffset < handle->tagSize) {
                    LOG_CLI((BSL_META("%02X"),
                             handle->tagData[tagStart + tagOffset]));
                }
                tagOffset++;
            } /* for (offset = 0; offset < 8; offset++) */
            if ((elemOffset < handle->grainSize) ||
                (tagOffset < handle->tagSize)) {
                LOG_CLI((BSL_META("\n                         ")));
            } else {
                LOG_CLI((BSL_META("\n")));
            }
        } /* while (either grain member state or tag remains to show) */
        elemsUsed += grainUse;
        if (0 == grainUse) {
            grainsFree++;
        } else if (grainUse == handle->grainSize) {
            grainsFull++;
        }
    } /* for all grains */
    LOG_CLI((BSL_META("  empty grains  = %08X\n"), grainsFree));
    LOG_CLI((BSL_META("  full grains   = %08X\n"), grainsFull));
    LOG_CLI((BSL_META("  counted elems = %08X %s\n"),
             elemsUsed,
             (error |= (elemsUsed != handle->used))?"INVALID":""));
    if (error) {
        LOG_CLI((BSL_META("tagged bitmap %p appears to be corrupt\n"),
                 (void*)handle));
        result = _SHR_E_INTERNAL;
    } else {
        result = _SHR_E_NONE;
    }
    return result;
}

