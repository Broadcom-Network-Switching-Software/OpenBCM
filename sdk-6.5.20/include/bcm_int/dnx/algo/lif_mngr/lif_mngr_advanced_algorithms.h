/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** \file lif_mngr_advanced_algorithms.h
 * 
 *
 * This file contains the callbacks for all of lif manager's advanced APIs.
 *
 * Don't include this file unless you are res_mngr_callbacks.c
 */

#ifndef  INCLUDE_LIF_MNGR_ADVANCED_ALGIRTHMS_H_INCLUDED
#define  INCLUDE_LIF_MNGR_ADVANCED_ALGIRTHMS_H_INCLUDED

/*************
 * INCLUDES  *
 */
/*
 * {
 */

/*
 * }
 */
/*************
 * DEFINES   *
 */
/*
 * {
 */

/**
 * }
 */

/*
 * }
 */
/*************
 * MACROS    *
 */
/*
 * {
 */

/*
 * }
 */
/*************
 * TYPE DEFS *
 */
/*
 * {
 */

/*
 * }
 */
/*************
 * GLOBALS   *
 */
/*
 * {
 */

/*
 * }
 */
/*************
 * FUNCTIONS *
 */
/*
 * {
 */

/**
 * \brief
 *
 * Create the advanced algorithm instance. See .c file for full description of the allocation algorithm.
 *
 * Full header description in \ref dnx_algo_res_create_cb
 *
 * \see
 *  dnx_algo_res_create_cb
 *  dnx_res_tag_bitmap_create
 */
shr_error_e dnx_algo_local_outlif_resource_create(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t * res_tag_bitmap,
    dnx_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags);

/**
 * \brief
 * See .c file for full description of the allocation algorithm.
 *
 * Full header description in \ref dnx_algo_res_allocate_cb
 *
 * \see
 * dnx_algo_res_allocate_cb
 */
shr_error_e dnx_algo_local_outlif_resource_allocate(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element);

/**
 * \brief
 * See .c file for full description of the allocation algorithm.
 *
 * Full header description in \ref dnx_algo_res_is_allocated_cb
 *
 * \see
 * dnx_algo_res_is_allocated_cb
 */
shr_error_e dnx_algo_local_outlif_resource_is_allocated(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int element,
    uint8 *is_allocated);

/**
 * \brief
 *
 * Create the advanced algorithm instance. See .c file for full description of the allocation algorithm.
 *
 * Full header description in \ref dnx_algo_res_create_cb
 *
 *
 * \ref
 *  resource_tag_bitmap_create
 */
shr_error_e dnx_lif_local_inlif_resource_create(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t * res_tag_bitmap,
    dnx_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags);

/**
 * \brief
 * See .c file for full description of the allocation algorithm.
 *
 * Full header description in \ref dnx_algo_res_allocate_cb
 *
 * \see
 * dnx_algo_res_allocate_cb
 */
shr_error_e dnx_lif_local_inlif_resource_allocate(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element);

/**
 * \brief
 * To free an entry we check its tag to get the entry's size, make sure that the element is
 * indeed the start of the entry (indicated by it being a multiple of the entry size) and then free
 * a number of entries identical to the entry size.
 *
 * \see
 * dnx_algo_res_free_cb
 * sw_state_res_tag_bitmap_tag_get
 */
shr_error_e dnx_lif_local_inlif_resource_free(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int element);

/**
 * \brief
 * The free several function is overridden in this case to release grains that were force set to illegal tag.
 * It shouldn't actually be used to free any resources.
 *
 * \see
 * dnx_algo_res_free_cb
 * resource_tag_bitmap_tag_set
 */
shr_error_e dnx_lif_local_inlif_resource_free_several(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 nof_elements,
    int element);

/**
 * \brief
 * The advanced fucntionality in this function compared to the basic algorithm is that it only returns TRUE
 * for the first index that a lif takes, instead of all the indexes like a simple bitmap would return.
 *
 * We do this because we assume that because inlif allocation is internal to the module and not exposed
 * to the rest of the SDK, it will not be necessary to ever check if a local inlif is allocated for anything other
 * than iterations. If that changes, then this needs to be reviewed.
 *
 * \see
 * dnx_algo_res_is_allocated_cb
 */
shr_error_e dnx_lif_local_inlif_resource_is_allocated(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    int element,
    uint8 *is_allocated);

/**
 * \brief
 *
 * Create the advanced algorithm instance. See .c file for full description of the allocation algorithm.
 *
 * Full header description in \ref dnx_algo_res_create_cb
 *
 * \see
 *  dnx_algo_res_create_cb
 *  resource_tag_bitmap_create
 */
shr_error_e dnx_algo_global_lif_res_create(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t * res_tag_bitmap,
    dnx_algo_res_create_data_t * create_data,
    void *extra_arguments,
    uint32 nof_elements,
    uint32 alloc_flags);

/**
 * \brief
 * Allocate a global lif instance.
 * Algorithm description in file.
 *
 * \see
 * dnx_algo_res_allocate_cb
 */
shr_error_e dnx_algo_global_lif_res_allocate(
    int unit,
    uint32 module_id,
    resource_tag_bitmap_t res_tag_bitmap,
    uint32 flags,
    void *extra_arguments,
    int *element);

/*
 * }
 */

#endif /* INCLUDE_LIF_MNGR_ADVANCED_ALGIRTHMS_H_INCLUDED */
