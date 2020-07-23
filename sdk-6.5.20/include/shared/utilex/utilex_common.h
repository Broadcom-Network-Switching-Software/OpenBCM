/** \file utilex_common.h
 *  
 * 
 * All utilities common to all the various utilex services. 
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef UTILEX_COMMON_H_INCLUDED
/** { */
#define UTILEX_COMMON_H_INCLUDED

/*************
* INCLUDES  *
 */
/** { */

#include <shared/shrextend/shrextend_debug.h>
/** } */
/*************
* DEFINES   *
 */
/** { */
/**
 * \brief
 * Verify specified unit has a legal value. If not, software goes to \n
 * exit with error code.
 * 
 * Notes: \n
 *   'exit' is assumed to be defined in the caller's scope.
 */
#define UTILEX_VERIFY_UNIT_IS_LEGAL(_unit) \
    if (((int)_unit < 0) || (_unit >= SOC_MAX_NUM_DEVICES)) \
    { \
        /* \
         * If this is an illegal unit identifier, quit \
         * with error. \
         */ \
        SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL) ; \
        SHR_EXIT() ; \
    }
/*
 * Define the various 'packed' attributes.
 * {
 */
#ifdef _MSC_VER
/* { */
/*
 * On MS-Windows platform this attribute is not defined.
 */
#ifndef __ATTRIBUTE_PACKED__
/* { */
#define __ATTRIBUTE_PACKED__
/* } */
#endif

#pragma warning(disable  : 4103)
#pragma warning(disable  : 4127)
#pragma pack(push)
#pragma pack(1)
/* } */
#elif defined(__GNUC__)
/* { */
/*
 * GNUC packing attribute.
 */
#ifndef __ATTRIBUTE_PACKED__
/* { */
#define __ATTRIBUTE_PACKED__ __attribute__ ((packed))
/* } */
#endif
/* } */
#elif defined(GHS)
/* { */
#ifndef __ATTRIBUTE_PACKED__
/* { */
#define __ATTRIBUTE_PACKED__ __attribute__ ((__packed__))
/* } */
#endif
/* } */
#else
/* { */
/*
 * Here add any other definition (custom)
 */
#define __ATTRIBUTE_PACKED__
/* } */
#endif

/*
 * }
 */

/** } */
/*************
* TYPEDEFS   *
 */
/** { */
/*
 * \brief Structure containing all the various table sizes used on initialization of UTILEX. These sizes are used for
 * the allocation of memory within SWSTATE. They are extracted at startup process from dnx_data. \see * \ref
 * dnx_utilex_init() * dnx_utilex.xml * jr2_a0_utilex.xml 
 */
typedef struct
{
    /**
     * Maximal number of bit map arrays to assign at init.
     * See utilex_occ_bm_init(), utilex_occ_bm_create()
     */
    int max_nof_dss_per_core;
    /**
     * Maximal number of hash table entries to assign at init.
     * See utilex_hash_table_init(), utilex_hash_table_create()
     */
    int max_nof_hashs_per_core;
    
    int max_nof_lists_per_core;
    /**
     * Maximal number of multi set entries to assign at init.
     * See utilex_multi_set_init(), utilex_multi_set_create()
     */
    int max_nof_multis_per_core;
    /**
     * Maximal number of res tag bitmap entries to assign at init.
     * See sw_state_res_tag_bitmap_init(), sw_state_res_tag_bitmap_create()
     */
    int max_nof_res_tag_bitmaps_per_core;
    /**
     * Maximal number of defragmented chunks to assign at init.
     * See utilex_defragmented_chunks_init(), utilex_defragmented_chunks_create()
     */
    int max_nof_defragmented_chunks_per_core;
} UTILEX_COMMON_SIZES;

/** } */

/*************
* PROTOTYPES *
 */
/** { */
/**
 * \brief
 * Initialized all common services under utilex (mostly on sw state infrastructure)
 * 
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of unit to handle
 *   \param [in] nof_cores -
 *     Number of cores for which to initialize all services. \n
 *     The number of cores is a factor in setting the number of available \n
 *     elements (entries) for each service.
 *   \param [in] utilex_common_sizes -
 *     Pointer to structure of type UTILEX_COMMON_SIZES. \n
 *     \b As \b input - \n
 *       Points to structure containing all sizes required by the \n
 *       various utilex initialization procedures.
 * \par DIRECT OUTPUT:
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT INPUT
 *   * SWSTATE system
 *   \b *utilex_common_sizes \n
 *     See 'utilex_common_sizes' in DIRECT INPUT above
 * \par INDIRECT OUTPUT
 *   Initialized services:
 *     occ_bm (occupation bitmap)
 *     hash_table
 *     sorted_list
 *     multi_set
 */
shr_error_e utilex_init(
    uint32 unit,
    uint32 nof_cores,
    UTILEX_COMMON_SIZES * utilex_common_sizes);
/**
 * \brief
 * Originally: Free all assets of all common services under utilex (mostly on \n
 * sw state infrastructure). However, since all UTILEX is on SWSTATE data base, \n
 * this procedure is empty
 * 
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of unit to handle. Currently ignored.
 * \par DIRECT OUTPUT:
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT INPUT
 *   * None (originally: SWSTATE system)
 * \par INDIRECT OUTPUT
 *   Deinitialized services (currently none).
 */
shr_error_e utilex_deinit(
    uint32 unit);
/** } */

/** } */
/** UTILEX_COMMON_H_INCLUDED  */
#endif
