/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Indexed resource management -- simple bitmap
 */
#if defined(BCM_PETRA_SUPPORT)
/* { */
#include <shared/bsl.h>
#include <shared/alloc.h>
#include <shared/bitop.h>
#include <soc/drv.h>
#include <shared/swstate/sw_state_res_tag_bitmap.h>
#include <include/shared/swstate/access/sw_state_access.h>

/*
 *  This is a fairly brute-force implementation of bitmap, with minimal
 *  optimisations or improvements.  It could probably be enhanced somewhat by
 *  some subtleties, such as checking whether a SHR_BITDCL is all ones before
 *  scanning individual bits when looking for free space.
 */

#define SW_STATE_RES_TAG_BITMAP_TAG_SIZE_MAX   20

/* 
 *  Sw state access macro.
 *   Since this module is used for both DNX (JR2) and DPP (JR+ and below), we need to seperate
 *     it by compilation.
 */
#define RES_BMP_ACCESS sw_state_access[unit].dpp.shr.res_tag_bmp_info
/*
 * Convert input bmp handle to index in 'occupied_ids' array.
 * Convert input index in 'occupied_ids' array to bmp handle.
 * Indices go from 0 -> (occupied_ids - 1)
 * Handles go from 1 -> occupied_ids
 */
#define SW_STATE_RES_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(_bmp_index,_handle) (_bmp_index = _handle - 1)
#define SW_STATE_RES_TAG_BMP_CONVERT_BMP_INDEX_TO_HANDLE(_handle,_bmp_index) (_handle = _bmp_index + 1)


#define SW_STATE_ACCESS_ERROR_CHECK(rv) \
    if (rv != _SHR_E_NONE) {             \
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,    \
                  (BSL_META("Error in sw state access\n")));    \
        return _SHR_E_INTERNAL;  \
    }


#define _SW_STATE_RES_TAG_BITMAP_DATA_SET_ALL (1)

static signed int
_sw_state_res_tag_bitmap_data_get(int unit,
                                 int bmp_index,
                                 _sw_state_res_tag_bitmap_list_t *bmp_data)
{
    int result = _SHR_E_NONE;

    result = RES_BMP_ACCESS.bitmap_lists.used.get(unit, bmp_index, &bmp_data->used);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    result = RES_BMP_ACCESS.bitmap_lists.low.get(unit, bmp_index, &bmp_data->low);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    result = RES_BMP_ACCESS.bitmap_lists.grainSize.get(unit, bmp_index, &bmp_data->grainSize);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    result = RES_BMP_ACCESS.bitmap_lists.tagSize.get(unit, bmp_index, &bmp_data->tagSize);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    result = RES_BMP_ACCESS.bitmap_lists.count.get(unit, bmp_index, &bmp_data->count);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    result = RES_BMP_ACCESS.bitmap_lists.lastFree.get(unit, bmp_index, &bmp_data->lastFree);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    result = RES_BMP_ACCESS.bitmap_lists.nextAlloc.get(unit, bmp_index, &bmp_data->nextAlloc);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    return result;
}

static signed int
_sw_state_res_tag_bitmap_data_set(int unit,
                                   uint32 flags,
                                   int bmp_index,
                                   _sw_state_res_tag_bitmap_list_t *bmp_data)
{
    int result = _SHR_E_NONE;

    result = RES_BMP_ACCESS.bitmap_lists.used.set(unit, bmp_index, bmp_data->used);
    SW_STATE_ACCESS_ERROR_CHECK(result);


    result = RES_BMP_ACCESS.bitmap_lists.lastFree.set(unit, bmp_index, bmp_data->lastFree);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    result = RES_BMP_ACCESS.bitmap_lists.nextAlloc.set(unit, bmp_index, bmp_data->nextAlloc);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    /* Set the bitmap "static" data as well. */
    if (flags & _SW_STATE_RES_TAG_BITMAP_DATA_SET_ALL) {

        result = RES_BMP_ACCESS.bitmap_lists.count.set(unit, bmp_index, bmp_data->count);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        result = RES_BMP_ACCESS.bitmap_lists.low.set(unit, bmp_index, bmp_data->low);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        result = RES_BMP_ACCESS.bitmap_lists.grainSize.set(unit, bmp_index, bmp_data->grainSize);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        result = RES_BMP_ACCESS.bitmap_lists.tagSize.set(unit, bmp_index, bmp_data->tagSize);
        SW_STATE_ACCESS_ERROR_CHECK(result);
    }


    return result;
}                                                                            

/*
 * Initialize the res tag bitmap sw state.
 */
int 
sw_state_res_tag_bitmap_init(int unit, 
                             int nof_bitmaps)
{
    int rv = _SHR_E_NONE;

    if (!SOC_WARM_BOOT(unit)) {
        /* Init the sw state. */
        rv = RES_BMP_ACCESS.bitmap_lists.ptr_alloc(unit, nof_bitmaps);
        SW_STATE_ACCESS_ERROR_CHECK(rv);

        /* Set the max nof bmps */
        rv = RES_BMP_ACCESS.max_nof_bmps.set(unit, nof_bitmaps);
        SW_STATE_ACCESS_ERROR_CHECK(rv);

        /* Allocate the occupation bitmap. */
        rv = RES_BMP_ACCESS.occupied_ids.alloc_bitmap(unit, nof_bitmaps);
        SW_STATE_ACCESS_ERROR_CHECK(rv);
    }

    return rv;
}

int
sw_state_res_tag_bitmap_create( int unit,
                                sw_state_res_tag_bitmap_handle_t *handle,
                                int low_id,
                                int count,
                                int grain_size,
                                int tag_size)
{
    int rv, found;
    uint32 nof_used_bmps, max_nof_bmps, bmp_index;
    uint8 bit_val;
    _sw_state_res_tag_bitmap_list_t bmp_data;

    /* check arguments */
    if (SOC_WARM_BOOT(unit)) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("Can't create bitmap during init\n")));
        return _SHR_E_DISABLED;
    }
    if (!handle) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("obligatory out argument must not be NULL\n")));
        return _SHR_E_PARAM;
    }
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("must have a positive number of elements\n")));
        return _SHR_E_PARAM;
    }
    if (0 > tag_size) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("tag size must not be negative\n")));
        return _SHR_E_PARAM;
    }
    if (tag_size >= SW_STATE_RES_TAG_BITMAP_TAG_SIZE_MAX) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("Tag size too large. Must be less than 20\n")));
        return _SHR_E_PARAM;
    }
    if (0 >= grain_size) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("must have at least one element per grain\n")));
        return _SHR_E_PARAM;
    }
    if (count % grain_size) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("count %d is not an integral number of grains %d\n"),
                   count,
                   grain_size));
        return _SHR_E_PARAM;
    }

    /* Make sure we can allocate another bitmap. */

    rv = RES_BMP_ACCESS.nof_in_use.get(unit, &nof_used_bmps);
    SW_STATE_ACCESS_ERROR_CHECK(rv);

    rv = RES_BMP_ACCESS.max_nof_bmps.get(unit, &max_nof_bmps);
    SW_STATE_ACCESS_ERROR_CHECK(rv);

    if (nof_used_bmps >= max_nof_bmps)
    {
      /*
       * If number of occupied bitmap structures is beyond the
       * maximum then quit with error.
       */
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
            (BSL_META("Maximum number of resources pools exceeded.\n")));
        return _SHR_E_FULL;
    }
    /*
     * Increment number of 'in_use' to cover the one we now intend to capture.
     */
    rv = RES_BMP_ACCESS.nof_in_use.set(unit, (nof_used_bmps + 1));
    SW_STATE_ACCESS_ERROR_CHECK(rv);

    /*
     * Find a free hash (a cleared bit in 'occupied_hashs'). At this point,
     * there must be one.
     */
    found = 0 ;
    for (bmp_index = 0 ; bmp_index < max_nof_bmps ; bmp_index++)
    {
      rv = RES_BMP_ACCESS.occupied_ids.bit_get(unit, bmp_index, &bit_val);
      SW_STATE_ACCESS_ERROR_CHECK(rv);
      if (bit_val == 0)
      {
        /*
         * 'hash_table_index' is now the index of a free entry.
         */
        found = 1 ;
        break ;
      }
    }
    if (!found)
    {
      LOG_ERROR(BSL_LS_SHARED_SWSTATE,
            (BSL_META("No free bitmap handle found.\n")));
        return _SHR_E_FULL;
    }
    rv = RES_BMP_ACCESS.occupied_ids.bit_set(unit, bmp_index);
    SW_STATE_ACCESS_ERROR_CHECK(rv);

    /*
     * Set the returned bmp_data. Note that legal handles start at '1', not at '0'.
     */
    SW_STATE_RES_TAG_BMP_CONVERT_BMP_INDEX_TO_HANDLE(*handle, bmp_index);

    rv = RES_BMP_ACCESS.bitmap_lists.alloc(unit, bmp_index);
    SW_STATE_ACCESS_ERROR_CHECK(rv);

    rv = RES_BMP_ACCESS.bitmap_lists.data.alloc_bitmap(unit, bmp_index, count);
    SW_STATE_ACCESS_ERROR_CHECK(rv);

    if (tag_size) {
        rv = RES_BMP_ACCESS.bitmap_lists.tagData.alloc(unit, bmp_index, tag_size * (count / grain_size));
        SW_STATE_ACCESS_ERROR_CHECK(rv);
    }
    
    /* init descriptor and data */

    sal_memset(&bmp_data, 0, sizeof(bmp_data));

    bmp_data.low = low_id;
    bmp_data.count = count;
    bmp_data.grainSize = grain_size;
    bmp_data.tagSize = tag_size;

    rv = _sw_state_res_tag_bitmap_data_set(unit, _SW_STATE_RES_TAG_BITMAP_DATA_SET_ALL, bmp_index, &bmp_data);
    SW_STATE_ACCESS_ERROR_CHECK(rv);

    /* all's well if we got here */
    return _SHR_E_NONE;
}

int
sw_state_res_tag_bitmap_destroy(int unit, 
                                sw_state_res_tag_bitmap_handle_t handle)
{
    uint32 bmp_index;
    int rv;
    uint8 used;
    uint32 nof_in_use;

    SW_STATE_RES_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(bmp_index, handle);

    rv = RES_BMP_ACCESS.occupied_ids.bit_get(unit, bmp_index, &used);
    SW_STATE_ACCESS_ERROR_CHECK(rv);

    if (used) {
        if (!SOC_IS_DETACHING(unit)) { /* If soc is detaching, SW state will be wiped out anyway.
                                    If it's warmboot, we don't want to reset SW state anyway. */
            /* Free the memory used by the pool. */
            rv = RES_BMP_ACCESS.bitmap_lists.data.free(unit, bmp_index); /* The bitmap itself */
            SW_STATE_ACCESS_ERROR_CHECK(rv);
            rv = RES_BMP_ACCESS.bitmap_lists.tagData.free(unit, bmp_index); /* Bitmap tag */
            SW_STATE_ACCESS_ERROR_CHECK(rv);
            rv = RES_BMP_ACCESS.bitmap_lists.free(unit, bmp_index); /* Bitmap struct */
            SW_STATE_ACCESS_ERROR_CHECK(rv);


            /* Remove the bitmap from the count and occupation bitmap. */
            rv = RES_BMP_ACCESS.nof_in_use.get(unit, &nof_in_use);
            SW_STATE_ACCESS_ERROR_CHECK(rv);

            nof_in_use--;

            rv = RES_BMP_ACCESS.nof_in_use.set(unit, nof_in_use);
            SW_STATE_ACCESS_ERROR_CHECK(rv);

            rv = RES_BMP_ACCESS.occupied_ids.bit_clear(unit, bmp_index);
            SW_STATE_ACCESS_ERROR_CHECK(rv);
        }

    } else {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("unable to free unallocated bitmap.\n")));
        rv = _SHR_E_PARAM;
    }

    return rv;
}


/* Compare tags by element. */
int
sw_state_res_tag_bitmap_compare_tags(int unit, 
                                     uint32 bmp_index,
                                     int tag_index,
                                     const void *tag,
                                     uint8 *equal)
{
    int rv, i;
    int tag_size;
    uint8 tmp;
    uint8 *tag_p = (uint8*) tag;

    rv = RES_BMP_ACCESS.bitmap_lists.tagSize.get(unit, bmp_index, &tag_size);
    SW_STATE_ACCESS_ERROR_CHECK(rv);

    /* Start by assuming that the tags are equal. If one element is different, change it. */
    *equal = TRUE;

    /* Compare each byte of the tag until a difference is found. */
    for (i = 0 ; i < tag_size ; i++) {
        rv = RES_BMP_ACCESS.bitmap_lists.tagData.get(unit, bmp_index, (tag_index * tag_size) + i, &tmp);
        SW_STATE_ACCESS_ERROR_CHECK(rv);

        if (tmp != tag_p[i]) {
            *equal = FALSE;
            break;
        }
    }

    return rv;
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
_sw_state_res_tag_bitmap_tag_check(int unit, 
                                   uint32 flags,
                                   sw_state_res_tag_bitmap_handle_t handle,
                                   int elemIndex,
                                   int elemCount,
                                   const void *tag,
                                   int *result)
{
    int offset = 0;
    uint8 temp, equal;
    int tag_index;
    int rv = _SHR_E_NONE;
    uint32 bmp_index; 
    uint8 always_check = (flags & SW_STATE_RES_TAG_BITMAP_ALWAYS_CHECK_TAG);
    _sw_state_res_tag_bitmap_list_t bmp_data;

    SW_STATE_RES_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(bmp_index, handle);

    rv = _sw_state_res_tag_bitmap_data_get(unit, bmp_index, &bmp_data);
    SW_STATE_ACCESS_ERROR_CHECK(rv);

    *result = 0;

    if (bmp_data.tagSize) {
        offset = elemIndex % bmp_data.grainSize;
        if (offset || always_check) {
            /* first element not first element of a grain */
            temp = 0;
            rv = RES_BMP_ACCESS.bitmap_lists.data.bit_range_test(unit, bmp_index, elemIndex - offset, bmp_data.grainSize, &temp);
            SW_STATE_ACCESS_ERROR_CHECK(rv);
            
            if (temp || always_check) {
                /* the grain is used by at least one other block */
                tag_index = (elemIndex / bmp_data.grainSize);
                rv = sw_state_res_tag_bitmap_compare_tags(unit, bmp_index, tag_index, tag, &equal);
                SW_STATE_ACCESS_ERROR_CHECK(rv);

                /* If the tags are not equal, mark it and return. */
                if (!equal) {
                    *result = -1;
                    return rv;
                }
            }
        }
        offset = (elemIndex + elemCount - 1) % bmp_data.grainSize;
        if (offset != (bmp_data.grainSize - 1) || always_check) {
            /* last element not last element of a grain */
            temp = 0;
            rv = RES_BMP_ACCESS.bitmap_lists.data.bit_range_test(unit, bmp_index, (elemIndex + elemCount - 1) - offset, bmp_data.grainSize, &temp);
            SW_STATE_ACCESS_ERROR_CHECK(rv);

            if (temp || always_check) {
                /* the grain is used by at least one other block */
                tag_index = (elemIndex + elemCount - 1) / bmp_data.grainSize;
                rv = sw_state_res_tag_bitmap_compare_tags(unit, bmp_index, tag_index, tag, &equal);
                SW_STATE_ACCESS_ERROR_CHECK(rv);

                /* If the tags are not equal, mark it */
                *result = (equal) ? 0 : 1;
            }
        }       
    }
    return rv;
}


/*
 *  Scan a range as per 'check all'.  Basically this needs to check whether a
 *  range is: all free, all in-use (with the same tag), all in-use (but
 *  different tags), or a mix of free and in-use (same or different tags).
 */
static int
_sw_state_res_tag_bitmap_check_all_tag(int unit,
                                       sw_state_res_tag_bitmap_handle_t handle,
                                       const void *tag,
                                       int count,
                                       int index)
{
    int tagbase;
    int offset;
    int freed = 0;
    int inuse = 0;
    int rv = _SHR_E_NONE, result = _SHR_E_NONE;
    uint8 bit_used = 0, equal = 0;
    uint32 bmp_index; 
    _sw_state_res_tag_bitmap_list_t bmp_data;

    SW_STATE_RES_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(bmp_index, handle);

    rv = _sw_state_res_tag_bitmap_data_get(unit, bmp_index, &bmp_data);
    SW_STATE_ACCESS_ERROR_CHECK(rv);


    /* scan the block */
    for (offset = 0; offset < count; offset++) {
        rv = RES_BMP_ACCESS.bitmap_lists.data.bit_get(unit, bmp_index, index + offset, &bit_used);
        SW_STATE_ACCESS_ERROR_CHECK(rv);
        if (bit_used) {
            inuse++;
        } else {
            freed++;
        }
    } /* for (offset = 0; offset < count; offset++) */
    if (inuse == count) {
        /* block is entirely in use */
        result = _SHR_E_FULL;
        /* now verify that all involved grains have same tag */
        if (0 < bmp_data.tagSize) {
            tagbase = index / bmp_data.grainSize;
            for (offset = 0;
                 offset < count;
                 offset += bmp_data.grainSize, tagbase++) {
                rv = sw_state_res_tag_bitmap_compare_tags(unit, bmp_index, tagbase, tag, &equal);
                SW_STATE_ACCESS_ERROR_CHECK(rv);
                if (!equal) {
                    /* tags are not equal, so different blocks */
                    result = _SHR_E_CONFIG;
                }
            } /* for (all grains in the block) */
        } /* if (0 < bmp_data.tagSize) */
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
int
sw_state_res_tag_bitmap_tag_set(int unit,
                                sw_state_res_tag_bitmap_handle_t handle,
                                const void* tag,
                                int elemIndex,
                                int elemCount)
{
    int index;
    int count;
    int offset;
    int result = _SHR_E_NONE;
    int i;
    uint8 *tag_p = (uint8*) tag;
    uint32 bmp_index; 
    _sw_state_res_tag_bitmap_list_t bmp_data;

    SW_STATE_RES_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(bmp_index, handle);

    result = _sw_state_res_tag_bitmap_data_get(unit, bmp_index, &bmp_data);
    SW_STATE_ACCESS_ERROR_CHECK(result);


    if (bmp_data.tagSize) {
        index = elemIndex / bmp_data.grainSize;
        count = (elemCount + bmp_data.grainSize - 1) / bmp_data.grainSize;
        for (offset = 0; offset < count; offset++) {
            for (i = 0 ; i < bmp_data.tagSize ; i++) {
                result = RES_BMP_ACCESS.bitmap_lists.tagData.set(unit, bmp_index, ((index + offset) * bmp_data.tagSize) + i, (uint8)tag_p[i]);
                SW_STATE_ACCESS_ERROR_CHECK(result);
            }
        }
    }

    return result;
}

int
sw_state_res_tag_bitmap_tag_get(int unit,
                                sw_state_res_tag_bitmap_handle_t handle,
                                int elemIndex,
                                const void* tag)
{
    int result = _SHR_E_NONE;
    uint32 bmp_index; 
    uint8* tag_p = (uint8*)tag;
    _sw_state_res_tag_bitmap_list_t bmp_data;

    SW_STATE_RES_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(bmp_index, handle);

    result = _sw_state_res_tag_bitmap_data_get(unit, bmp_index, &bmp_data);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    if (bmp_data.tagSize) {
        result = RES_BMP_ACCESS.bitmap_lists.tagData.get(unit, bmp_index, ((elemIndex / bmp_data.grainSize) * bmp_data.tagSize), tag_p);
        SW_STATE_ACCESS_ERROR_CHECK(result);
    }
    return result;
}



int
sw_state_res_tag_bitmap_alloc_tag(int unit,
                                  sw_state_res_tag_bitmap_handle_t handle,
                                  uint32 flags,
                                  const void *tag,
                                  int count,
                                  int *elem)
{
    /* Regular allocation is identical to aligned allocation with an alignement of 1, offset 0 */
    int offs = 0, align = 1;
    return sw_state_res_tag_bitmap_alloc_align_tag(unit, handle, flags, align, offs, tag, count, elem);
}

int
sw_state_res_tag_bitmap_alloc(int unit,
                              sw_state_res_tag_bitmap_handle_t handle,
                              uint32 flags,
                              int count,
                              int *elem)
{
    return sw_state_res_tag_bitmap_alloc_tag(unit, handle, flags, NULL, count, elem);
}

int
sw_state_res_tag_bitmap_alloc_align_tag(int unit,
                                       sw_state_res_tag_bitmap_handle_t handle,
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
    uint8 temp;
    uint32 bmp_index; 
    _sw_state_res_tag_bitmap_list_t bmp_data;
    uint8 empty_tag[SW_STATE_RES_TAG_BITMAP_TAG_SIZE_MAX];
    int bitmap_tag_compare_result = 0;
    int last_legal_index = 0;

    SW_STATE_RES_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(bmp_index, handle);

    result = _sw_state_res_tag_bitmap_data_get(unit, bmp_index, &bmp_data);
    SW_STATE_ACCESS_ERROR_CHECK(result);


    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("Illegal handle id. Must be >0\n")));
        return _SHR_E_PARAM;
    }
    if (!elem) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("obligatory in/out argument must not be NULL\n")));
        return _SHR_E_PARAM;
    }
    if (count + bmp_data.used > bmp_data.count) {
        LOG_DEBUG(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("not enough free elements (%d) to satisfy request"
                   " for %d element block\n"),
                   bmp_data.count - bmp_data.used,
                   count));
        return _SHR_E_RESOURCE;
    }
    if (!tag) {
        /* if NULL tag, use default (all zeroes) value instead */
        sal_memset(empty_tag, 0, SW_STATE_RES_TAG_BITMAP_TAG_SIZE_MAX); 
        tag = empty_tag;
    }

    if (flags & SW_STATE_RES_TAG_BITMAP_ALLOC_WITH_ID) {
        /* WITH_ID, so only try the specifically requested block */
        if (*elem < bmp_data.low) {
            /* not valid ID */
            result = _SHR_E_PARAM;
        }
        index = *elem - bmp_data.low;
        if (index + count > bmp_data.count) {
            /* not valid ID */
            result = _SHR_E_PARAM;
        }
        if (_SHR_E_NONE == result) {
            /* make sure caller's request is valid */
            if (flags & SW_STATE_RES_TAG_BITMAP_ALLOC_ALIGN_ZERO) {
                /* alignment is against zero */
                offset = (*elem) % align;
            } else {
                /* alignment is against low */
                offset = ((*elem) - bmp_data.low) % align;
            }
            if (offset != offs) {
                LOG_ERROR(BSL_LS_SHARED_SWSTATE,
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
            if (flags & SW_STATE_RES_TAG_BITMAP_ALLOC_REPLACE) {
                /* trying to replace */
                result = _sw_state_res_tag_bitmap_check_all_tag(unit,
                                                               handle,
                                                               tag,
                                                               count,
                                                               index);
                switch (result) {
                case _SHR_E_FULL:
                    result = _SHR_E_NONE;
                    break;
                case _SHR_E_EMPTY:
                        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                                  (BSL_META("proposed base %d count %d"
                                   " does not exist\n"),
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
                    LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                              (BSL_META("proposed base %d count %d"
                               " would merge/expand existing"
                               " block(s)\n"),
                               *elem,
                               count));
                    result = _SHR_E_RESOURCE;
                    break;
                default:
                    /* should never see this */
                    LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                              (BSL_META("unexpected result checking proposed"
                               " block:  %d (%s)\n"),
                               result,
                               _SHR_ERRMSG(result)));
                    if (_SHR_E_NONE == result) {
                        result = _SHR_E_INTERNAL;
                    }
                }
            } else { /* if (flags & SW_STATE_RES_TAG_BITMAP_ALLOC_REPLACE) */
                /* check whether the block is free */
                temp = 0;
                result = RES_BMP_ACCESS.bitmap_lists.data.bit_range_test(unit, bmp_index, index, count, &temp);
                SW_STATE_ACCESS_ERROR_CHECK(result);

                if (temp) {
                    /* in use; can't do WITH_ID alloc of this block */
                    LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                              (BSL_META("Given element, or some of it, already exists. Asked for %d elements "
                                        "starting from index %d\n"),
                                        count,
                                        *elem));
                    result = _SHR_E_RESOURCE;
                }
                if (_SHR_E_NONE == result) {
                    result = _sw_state_res_tag_bitmap_tag_check(unit, flags, handle, index, count, tag, &bitmap_tag_compare_result);
                    SW_STATE_ACCESS_ERROR_CHECK(result);
                    if (bitmap_tag_compare_result) {
                        /* grain at one end or other has mismatching tag */
                        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                                  (BSL_META("proposed base %d count %d"
                                   " tag %p would share end elements"
                                   " with another block that has a"
                                   " different tag value"),
                                   *elem,
                                   count,
                                   tag));
                        result = _SHR_E_RESOURCE;
                    }
                }
            } /* if (flags & SW_STATE_RES_TAG_BITMAP_ALLOC_REPLACE) */
        } /* if (_SHR_E_NONE == result) */
        /* don't adjust last free or next alloc for WITH_ID */
    } else { /* if (flags & SHR_RES_ALLOC_WITH_ID) */
        if (flags & SW_STATE_RES_TAG_BITMAP_ALLOC_ALIGN_ZERO) {
            /* alignment is against zero, not start of pool */
            offs += align - (bmp_data.low % align);
        }
        /* see if there are enough elements after last free */
        index = (((bmp_data.lastFree + align - 1) / align) * align) + offs;
        if (index + count < bmp_data.count) {
            /* it might fit */
            temp = 0;
            result = RES_BMP_ACCESS.bitmap_lists.data.bit_range_test(unit, bmp_index, index, count, &temp);
            SW_STATE_ACCESS_ERROR_CHECK(result);

            if (temp) {
                /* ran into non-free elements in the proposed block */
                result = _SHR_E_EXISTS;
            } else {
                result = _sw_state_res_tag_bitmap_tag_check(unit,
                                                            flags,
                                                            handle,
                                                            index,
                                                            count,
                                                            tag,
                                                            &bitmap_tag_compare_result);
                SW_STATE_ACCESS_ERROR_CHECK(result);
                if (bitmap_tag_compare_result) {
                    /* at least one end had a different tag */
                    result = _SHR_E_EXISTS;
                }
            }
        } else {
            result = _SHR_E_EXISTS;
        }
        if (_SHR_E_NONE == result) {
            /* looks good; adjust last free to miss this block */
            bmp_data.lastFree = index + count;
        } else { /* if (_SHR_E_NONE == result) */
            /* start searching after last successful alloc */
            index = (((bmp_data.nextAlloc + align - 1) / align) * align) + offs;
            last_legal_index = bmp_data.count - count;
            while (index <= last_legal_index) {
                /* Check if the starting index is free */
                result = RES_BMP_ACCESS.bitmap_lists.data.bit_get(unit, bmp_index, index, &temp);
                SW_STATE_ACCESS_ERROR_CHECK(result);

                while (temp &&
                       ((index + align) <= last_legal_index)) {
                    /* Find the next free index */
                    index += align;
                    result = RES_BMP_ACCESS.bitmap_lists.data.bit_get(unit, bmp_index, index, &temp);
                    SW_STATE_ACCESS_ERROR_CHECK(result);
                }

                if (index <= last_legal_index) {
                    /* We have a candidate; see if block is big enough */
                    result = _SHR_E_NONE;
                    for (offset = 0; offset < count; offset++) {

                        result = RES_BMP_ACCESS.bitmap_lists.data.bit_get(unit, bmp_index, index + offset, &temp);
                        SW_STATE_ACCESS_ERROR_CHECK(result);

                        if (temp) {
                            /* Not big enough; skip this block */
                            result = _SHR_E_EXISTS;
                            /* Realign after this block */
                            index = (((index + offset + align) / align) * align) + offs;
                            break;
                        }
                    } /* for (offset = 0; offset < count; offset++) */
                } /* if (index <= last_legal_index) */
                if (_SHR_E_NONE == result) {
                    /* found a sufficient block, check tags */
                    result = _sw_state_res_tag_bitmap_tag_check(unit,
                                                                flags,
                                                                handle,
                                                                index,
                                                                count,
                                                                tag,
                                                                &bitmap_tag_compare_result);
                    SW_STATE_ACCESS_ERROR_CHECK(result);
                    if (bitmap_tag_compare_result) {
                        /* at least one end had a different tag */
                        result = _SHR_E_EXISTS;
                        if (0 < bitmap_tag_compare_result) {
                            /* ending had wrong tag; skip to end */
                            index += (count - 1);
                        }
                        /* seek to beginning of next grain */
                        index += (bmp_data.grainSize - (index %
                                                       bmp_data.grainSize));
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
                index = offs;
                last_legal_index = bmp_data.nextAlloc - count;
                while (index <= last_legal_index) {
                    /* Check if the starting index is free */
                    result = RES_BMP_ACCESS.bitmap_lists.data.bit_get(unit, bmp_index, index, &temp);
                    SW_STATE_ACCESS_ERROR_CHECK(result);

                    while (temp &&
                           ((index + align) <= last_legal_index)) {
                        /* Find the next free index */
                        index += align;
                        result = RES_BMP_ACCESS.bitmap_lists.data.bit_get(unit, bmp_index, index, &temp);
                        SW_STATE_ACCESS_ERROR_CHECK(result);
                    }

                    if (index <= last_legal_index) {
                        /* We have a candidate; see if block is big enough */
                        result = _SHR_E_NONE;
                        for (offset = 0; offset < count; offset++) {
                            result = RES_BMP_ACCESS.bitmap_lists.data.bit_get(unit, bmp_index, index + offset, &temp);
                            SW_STATE_ACCESS_ERROR_CHECK(result);

                            if (temp) {
                                /* Not big enough; skip this block */
                                result = _SHR_E_EXISTS;
                                /* Realign after this block */
                                index = (((index + offset + align) / align) * align) + offs;
                                break;
                            }
                        } /* for (offset = 0; offset < count; offset++) */
                    } /* if (index <= last_legal_index) */
                    if (_SHR_E_NONE == result) {
                        /* found a sufficient block, check tags */
                        result = _sw_state_res_tag_bitmap_tag_check(unit,
                                                                    flags,
                                                                    handle,
                                                                    index,
                                                                    count,
                                                                    tag,
                                                                    &bitmap_tag_compare_result);
                        SW_STATE_ACCESS_ERROR_CHECK(result);
                        if (bitmap_tag_compare_result) {
                            /* at least one end had a different tag */
                            result = _SHR_E_EXISTS;
                            if (0 < bitmap_tag_compare_result) {
                                /* ending had wrong tag; skip to end */
                                index += (count - 1);
                            }
                            /* seek to beginning of next grain */
                            index += (bmp_data.grainSize - (index %
                                                           bmp_data.grainSize));
                            /* realign after this block */
                            index = (((index + align - 1) / align) * align) + offs;
                        }
                    }
                    if (_SHR_E_NONE == result) {
                        /* found a sufficient block with matching end tags */
                        break;
                    }
                } /* while (index < data->next_alloc) */
            } /* if (_SHR_E_NONE != result) */
            if (_SHR_E_NONE != result) {
                /* still no space; give up */
                result = _SHR_E_RESOURCE;
            } else {
                /* got some space; update next alloc  */
                bmp_data.nextAlloc = index + count;
            }
        } /* if (_SHR_E_NONE == result) */
    } /* if (flags & SHR_RES_ALLOC_WITH_ID) */
    if ( (_SHR_E_NONE == result) && !(flags & SW_STATE_RES_TAG_BITMAP_ALLOC_CHECK_ONLY ) ) {
        /* set the tag for all grains involved */
        sw_state_res_tag_bitmap_tag_set(unit, handle, tag, index, count);
        /* mark the block as in-use */

        result = RES_BMP_ACCESS.bitmap_lists.data.bit_range_set(unit, bmp_index, index, count);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        if (0 == (flags & SW_STATE_RES_TAG_BITMAP_ALLOC_REPLACE)) {
            bmp_data.used += count;
        }

        result = _sw_state_res_tag_bitmap_data_set(unit, 0, bmp_index, &bmp_data);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        /* return the beginning element */
        *elem = index + bmp_data.low;
    } /* if (_SHR_E_NONE == result) */
    return result;
}

int
sw_state_res_tag_bitmap_alloc_align(int unit,
                                   sw_state_res_tag_bitmap_handle_t handle,
                                   uint32 flags,
                                   int align,
                                   int offs,
                                   int count,
                                   int *elem)
{
    return sw_state_res_tag_bitmap_alloc_align_tag(unit,
                                                  handle,
                                                  flags,
                                                  align,
                                                  offs,
                                                  NULL,
                                                  count,
                                                  elem);
}

int
sw_state_res_tag_bitmap_free(int unit,
                             sw_state_res_tag_bitmap_handle_t handle,
                             int count,
                             int elem)
{
    int index;
    int offset;
    int result = _SHR_E_NONE;
    uint32 bmp_index; 
    _sw_state_res_tag_bitmap_list_t bmp_data;
    uint8 temp;


    SW_STATE_RES_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(bmp_index, handle);

    result = _sw_state_res_tag_bitmap_data_get(unit, bmp_index, &bmp_data);
    SW_STATE_ACCESS_ERROR_CHECK(result);


    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("Illegal handle id. Must be >0\n")));
        return _SHR_E_PARAM;
    }
    if (elem < bmp_data.low) {
        /* not valid ID */
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("invalid staring element %d\n"),
                   elem));
        return _SHR_E_PARAM;
    }
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("must free a positive number of elements\n")));
        return _SHR_E_PARAM;
    }
    index = elem - bmp_data.low;
    if (index + count > bmp_data.count) {
        /* not valid ID */
        result = _SHR_E_PARAM;
    }
    if (_SHR_E_NONE == result) {
        /* check whether the block is in use */
        for (offset = 0; offset < count; offset++) {
            result = RES_BMP_ACCESS.bitmap_lists.data.bit_get(unit, bmp_index, index + offset, &temp);
            SW_STATE_ACCESS_ERROR_CHECK(result);

            if (!temp) {
                /* not entirely in use */
                result = _SHR_E_NOT_FOUND;
                break;
            }
        } /* for (offset = 0; offset < count; offset++) */
    } /* if (_SHR_E_NONE == result) */
    if (_SHR_E_NONE == result) {
        /* looks fine, so mark the block as free */
        result = RES_BMP_ACCESS.bitmap_lists.data.bit_range_clear(unit, bmp_index, index, count);
        SW_STATE_ACCESS_ERROR_CHECK(result);

        bmp_data.used -= count;
        bmp_data.lastFree = index;

        result = _sw_state_res_tag_bitmap_data_set(unit, 0, bmp_index, &bmp_data);
        SW_STATE_ACCESS_ERROR_CHECK(result);
    } /* if (_SHR_E_NONE == result) */
    /* return the result */
    return result;
}

int
sw_state_res_tag_bitmap_check(int unit,
                              sw_state_res_tag_bitmap_handle_t handle,
                              int count,
                              int elem)
{
    int index;
    int offset;
    int result, rv;
    uint8 temp;
    uint32 bmp_index; 
    _sw_state_res_tag_bitmap_list_t bmp_data;

    SW_STATE_RES_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(bmp_index, handle);

    rv = _sw_state_res_tag_bitmap_data_get(unit, bmp_index, &bmp_data);
    SW_STATE_ACCESS_ERROR_CHECK(rv);

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("Illegal handle id. Must be >0\n")));
        return _SHR_E_PARAM;
    }
    if (elem < bmp_data.low) {
        /* not valid ID */
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("invalid staring element %d\n"),
                   elem));
        return _SHR_E_PARAM;
    }
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("must check a positive number of elements\n")));
        return _SHR_E_PARAM;
    }
    index = elem - bmp_data.low;
    if (index + count > bmp_data.count) {
        /* not valid ID */
        return _SHR_E_PARAM;
    }
    /* check whether the block is in use */
    result = _SHR_E_NOT_FOUND;
    for (offset = 0; offset < count; offset++) {
        rv = RES_BMP_ACCESS.bitmap_lists.data.bit_get(unit, bmp_index, index + offset, &temp);
        SW_STATE_ACCESS_ERROR_CHECK(rv);

        if (temp) {
            /* not entirely free */
            result = _SHR_E_EXISTS;
            break;
        }
    } /* for (offset = 0; offset < count; offset++) */
    /* return the result */
    return result;
}

int
sw_state_res_tag_bitmap_check_all_tag(int unit,
                                      sw_state_res_tag_bitmap_handle_t handle,
                                      const void *tag,
                                      int count,
                                      int elem)
{
    int index;
    int result = _SHR_E_NONE;
    uint32 bmp_index; 
    _sw_state_res_tag_bitmap_list_t bmp_data;
    uint8 empty_tag[SW_STATE_RES_TAG_BITMAP_TAG_SIZE_MAX];

    SW_STATE_RES_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(bmp_index, handle);

    result = _sw_state_res_tag_bitmap_data_get(unit, bmp_index, &bmp_data);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("Illegal handle id. Must be >0\n")));
        return _SHR_E_PARAM;
    }
    if (elem < bmp_data.low) {
        /* not valid ID */
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("invalid staring element %d\n"),
                   elem));
        return _SHR_E_PARAM;
    }
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("must check a positive number of elements\n")));
        return _SHR_E_PARAM;
    }
    if (!tag) {
        /* if NULL tag, use default (all zeroes) value instead */
        sal_memset(empty_tag, 0, SW_STATE_RES_TAG_BITMAP_TAG_SIZE_MAX); 
        tag = empty_tag;
    }
    index = elem - bmp_data.low;
    if (index + count > bmp_data.count) {
        /* not valid ID */
        return _SHR_E_PARAM;
    }
    if (_SHR_E_NONE == result) {
        result = _sw_state_res_tag_bitmap_check_all_tag(unit, handle, tag, count, index);
    }
    /* return the result */
    return result;
}

int
sw_state_res_tag_bitmap_check_all(int unit,
                                  sw_state_res_tag_bitmap_handle_t handle,
                                  int count,
                                  int elem)
{
    int index;
    int result = _SHR_E_NONE;
    uint32 bmp_index; 
    _sw_state_res_tag_bitmap_list_t bmp_data;
    int i;
    uint8 tag[SW_STATE_RES_TAG_BITMAP_TAG_SIZE_MAX];
    SW_STATE_RES_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(bmp_index, handle);

    result = _sw_state_res_tag_bitmap_data_get(unit, bmp_index, &bmp_data);
    SW_STATE_ACCESS_ERROR_CHECK(result);


    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("Illegal handle id. Must be >0\n")));
        return _SHR_E_PARAM;
    }
    if (elem < bmp_data.low) {
        /* not valid ID */
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("invalid staring element %d\n"),
                   elem));
        return _SHR_E_PARAM;
    }
    if (0 >= count) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("must check a positive number of elements\n")));
        return _SHR_E_PARAM;
    }
    index = elem - bmp_data.low;
    if (index + count > bmp_data.count) {
        /* not valid ID */
        return _SHR_E_PARAM;
    }

    /* here we assume the tag from the first grain in the range. Copy it to the buffer. */
    for (i = 0 ; i < bmp_data.tagSize ; i++) {
        result = RES_BMP_ACCESS.bitmap_lists.tagData.get(unit, bmp_index, i, &tag[i]);
        SW_STATE_ACCESS_ERROR_CHECK(result);
    }
    result = _sw_state_res_tag_bitmap_check_all_tag(unit, handle, (void*)tag, count, index);

    /* return the result */
    return result;
}

int
sw_state_res_tag_bitmap_nof_free_elements_get(int unit,
                                              sw_state_res_tag_bitmap_handle_t handle,
                                              int *nof_free_elements)
{
    int result = _SHR_E_NONE;
    uint32 bmp_index; 
    _sw_state_res_tag_bitmap_list_t bmp_data;

    /* check arguments */
    if (!handle) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("Illegal handle id. Must be >0\n")));
        return _SHR_E_PARAM;
    }
    if (nof_free_elements == NULL)
    {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("obligatory in/out argument must not be NULL: nof_free_elements\n")));
        return _SHR_E_PARAM;
    }

    SW_STATE_RES_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(bmp_index, handle);
    result = _sw_state_res_tag_bitmap_data_get(unit, bmp_index, &bmp_data);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    /* 
     * The number of free elements is the number of existing elements (count) - 
     *    the number of used elements.
     */
    *nof_free_elements = bmp_data.count - bmp_data.used;

    return _SHR_E_NONE;
}

int
sw_state_res_tag_bitmap_dump(int unit,
                             const sw_state_res_tag_bitmap_handle_t handle)
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
    uint32 bmp_index; 
    _sw_state_res_tag_bitmap_list_t bmp_data;
    uint8 temp;
    uint8 *tagData = NULL;
    int nofGrains;

    if (!handle) {
        LOG_ERROR(BSL_LS_SHARED_SWSTATE,
                  (BSL_META("must provide non-NULL handle\n")));
        return _SHR_E_PARAM;
    }

    /* Copy the bitmap's data. */
    SW_STATE_RES_TAG_BMP_CONVERT_HANDLE_TO_BMP_INDEX(bmp_index, handle);

    result = _sw_state_res_tag_bitmap_data_get(unit, bmp_index, &bmp_data);
    SW_STATE_ACCESS_ERROR_CHECK(result);

    /* Allocate memory to copy the bitmap's tag. */
    nofGrains = bmp_data.count / bmp_data.grainSize;
    tagData = (uint8*) sal_alloc(nofGrains * bmp_data.tagSize, "Tag data duplication");

    /* Copy the bitmap's tag. */
    for (tagOffset = 0 ; tagOffset < nofGrains * bmp_data.tagSize; tagOffset++) {
        result = RES_BMP_ACCESS.bitmap_lists.tagData.get(unit, bmp_index, tagOffset, &tagData[tagOffset]);
        if (result) {
            goto error;
        }
    }

    LOG_CLI((BSL_META("Data for %sbitmap #%d:\n"), (bmp_data.tagSize) ? "tagged " : "", handle));
    LOG_CLI((BSL_META("  lowest ID     = %08X\n"), bmp_data.low));
    LOG_CLI((BSL_META("  element count = %08X\n"), bmp_data.count));
    LOG_CLI((BSL_META("  used elements = %08X\n"), bmp_data.used));
    LOG_CLI((BSL_META("  grain size    = %08X elements\n"), bmp_data.grainSize));
    LOG_CLI((BSL_META("  tag size      = %08X bytes\n"), bmp_data.tagSize));
    LOG_CLI((BSL_META("  last free     = %08X %s\n"),
             bmp_data.lastFree,
             (error |= (bmp_data.lastFree > bmp_data.count))?"INVALID":""));
    LOG_CLI((BSL_META("  next alloc    = %08X %s\n"),
             bmp_data.nextAlloc,
             (error |= (bmp_data.nextAlloc > bmp_data.count))?"INVALID":""));
    LOG_CLI((BSL_META("  element map:\n")));
    LOG_CLI((BSL_META("    1st Elem (index)    State of elements (1 = used)        Tag bytes\n")));
    LOG_CLI((BSL_META("    -------- --------   --------------------------------    ----------------\n")));
    elemsUsed = 0;
    grainsFree = 0;
    grainsFull = 0;
    for (index = 0; index < bmp_data.count; index += bmp_data.grainSize) {
        LOG_CLI((BSL_META("    %08X %08X   "), index + bmp_data.low, index));
        elemOffset = 0;
        tagOffset = 0;
        grainUse = 0;
        while ((elemOffset < bmp_data.grainSize) &&
               (tagOffset < bmp_data.tagSize)) {
            for (offset = 0; offset < 32; offset++) {
                if (elemOffset < bmp_data.grainSize) {
                    result = RES_BMP_ACCESS.bitmap_lists.data.bit_get(unit, bmp_index, index + elemOffset, &temp);
                    if (result){
                        /* Something went wrong in sw state. Break the loop. */
                        goto error;
                    }
                    if (temp) {
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
            tagStart = (index / bmp_data.grainSize) * bmp_data.tagSize;
            for (offset = 0; offset < 8; offset++) {
                if (tagOffset < bmp_data.tagSize) {
                    LOG_CLI((BSL_META("%02X"),
                             tagData[tagStart + tagOffset]));
                }
                tagOffset++;
            } /* for (offset = 0; offset < 8; offset++) */
            if ((elemOffset < bmp_data.grainSize) ||
                (tagOffset < bmp_data.tagSize)) {
                LOG_CLI((BSL_META("\n                         ")));
            } else {
                LOG_CLI((BSL_META("\n")));
            }
        } /* while (either grain member state or tag remains to show) */
        elemsUsed += grainUse;
        if (0 == grainUse) {
            grainsFree++;
        } else if (grainUse == bmp_data.grainSize) {
            grainsFull++;
        }
    } /* for all grains */
    LOG_CLI((BSL_META("  empty grains  = %08X\n"), grainsFree));
    LOG_CLI((BSL_META("  full grains   = %08X\n"), grainsFull));
    LOG_CLI((BSL_META("  counted elems = %08X %s\n"),
             elemsUsed,
             (error |= (elemsUsed != bmp_data.used))?"INVALID":""));
error:
    if (tagData) {
        sal_free(tagData);
    }
    if (error) {
        LOG_CLI((BSL_META("tagged bitmap %d appears to be corrupt\n"),
                 handle));
        result = _SHR_E_INTERNAL;
    } else {
        result = _SHR_E_NONE;
    }
    return result;
}
/* } */
#else
int sw_state_res_tag_cfile;
#endif
