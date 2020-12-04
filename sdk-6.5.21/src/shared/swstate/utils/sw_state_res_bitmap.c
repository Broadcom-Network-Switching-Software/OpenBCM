/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Indexed resource management -- simple bitmap
 */

#include <shared/error.h>
#include <shared/swstate/sw_state_res_bitmap.h>
#include <shared/swstate/sw_state_res_tag_bitmap.h>

/* 
 *  For code simplicity, we use tag bitmap as the engine for regular bitmaps. This file is just a wrapper.
 */ 

#if defined(BCM_PETRA_SUPPORT)

int
sw_state_res_bitmap_create(int unit,
                      uint32 *handle,
                      int low_id,
                      int count)
{
    /* The grain size is the entire bitmap, and the tag is non existent. */
    return sw_state_res_tag_bitmap_create(unit, handle, low_id, count, count, 0);
}

int
sw_state_res_bitmap_destroy(int unit,
                       sw_state_res_bitmap_handle_t handle)
{
    return sw_state_res_tag_bitmap_destroy(unit, handle);
}

int
sw_state_res_bitmap_alloc(int unit,
                     sw_state_res_bitmap_handle_t handle,
                     uint32 flags,
                     int count,
                     int *elem)
{
    return sw_state_res_tag_bitmap_alloc(unit, handle, flags, count, elem);
}

int
sw_state_res_bitmap_alloc_align(int unit,
                           sw_state_res_bitmap_handle_t handle,
                           uint32 flags,
                           int align,
                           int offs,
                           int count,
                           int *elem)
{
    return sw_state_res_tag_bitmap_alloc_align(unit, handle, flags, align, offs, count, elem);
}

int
sw_state_res_bitmap_alloc_align_sparse(int unit,
                                  sw_state_res_bitmap_handle_t handle,
                                  uint32 flags,
                                  int align,
                                  int offs,
                                  uint32 pattern,
                                  int length,
                                  int repeats,
                                  int *elem)
{
    return _SHR_E_UNAVAIL; /* Not currently used. Add support seperately. */
}

int
sw_state_res_bitmap_free(int unit,
                    sw_state_res_bitmap_handle_t handle,
                    int count,
                    int elem)
{
    return sw_state_res_tag_bitmap_free(unit, handle, count, elem);
}

int
sw_state_res_bitmap_free_sparse(int unit,
                           sw_state_res_bitmap_handle_t handle,
                           uint32 pattern,
                           int length,
                           int repeats,
                           int elem)
{
    return _SHR_E_UNAVAIL; /* Sparse allocation not supported. */
}

int
sw_state_res_bitmap_check(int unit,
                     sw_state_res_bitmap_handle_t handle,
                     int count,
                     int elem)
{
    return sw_state_res_tag_bitmap_check(unit, handle, count, elem);
}

int
sw_state_res_bitmap_check_all(int unit,
                         sw_state_res_bitmap_handle_t handle,
                         int count,
                         int elem)
{
    return sw_state_res_tag_bitmap_check_all(unit, handle, count, elem);
}

int
sw_state_res_bitmap_check_all_sparse(int unit,
                                sw_state_res_bitmap_handle_t handle,
                                uint32 pattern,
                                int length,
                                int repeats,
                                int elem)
{
    return _SHR_E_UNAVAIL; /* Sparse allocation not supported. */
}

int
sw_state_res_bitmap_dump(int unit,
                    const sw_state_res_bitmap_handle_t handle)
{
    return sw_state_res_tag_bitmap_dump(unit, handle);
}


#endif
