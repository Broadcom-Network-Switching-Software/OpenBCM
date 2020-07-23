/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */
/**
 * \file utilex_occupation_bitmap.h
 *
 * Definitions and prototypes for all common utilities related
 * to occupation bitmap.
 *
 * Note that this is used by other 'utilex' packages such
 * as sorted-list utilities or hash-table utilities
 */
#ifndef UTILEX_OCCUPATION_BITMAP_H_INCLUDED
/*
 * { 
 */
#define UTILEX_OCCUPATION_BITMAP_H_INCLUDED

/*************
* INCLUDES  *
 */
/** { */
#include <shared/utilex/utilex_framework.h>
#include <shared/utilex/utilex_common.h>
#ifdef BCM_DNX_SUPPORT
#include <soc/dnx/swstate/auto_generated/types/utilex_occupation_bitmap_types.h>
/** } */

/*************
* DEFINES   *
 */
/** { */
/**
 * \brief
 * Number of bit-combinations in a byte
 * See utilex_bitmap_val_to_first_one()
 */
#define  UTILEX_OCC_BM_ARRAY_MAP_LEN SAL_BIT(UTILEX_NOF_BITS_IN_CHAR)
/**
 * \brief
 * Indication, on the various utilex_bitmap_val_to_* arrays, that
 * an entry is not meaningful (for instance, 'first non-zero bit on
 * a byte whose value is 0').
 * See utilex_bitmap_val_to_first_one()
 */
#define  UTILEX_OCC_BM_NODE_IS_FULL  UTILEX_NOF_BITS_IN_CHAR
/** } */

/*************
* MACROS    *
 */
/*
 * { 
 */
/**
 * \brief
 * Access macro into sw state. Used to make the code more readable.
 */
#define UTILEX_OCC_BITMAP_ACCESS                utilex_occ_bitmap_db
/**
 * \brief
 * Get value of byte no. '_char_indx' on level '_level_indx' from 'levels_cache_buffer'
 * or from 'levels_buffer' depending on element 'cache_enabled' on specified bit map index
 * structure ('_bmp_map_index') which is assumed to be occupied.
 * \par DIRECT INPUT
 *   \param [in] _bmp_map_index - \n
 *     Index of bit map (starting from zero) to extract info from.
 *   \param [in] _level_indx - \n
 *     Level within bit map (starting from zero) to extract info from. \n
 *     Level is from one of the following arrays, depending on 'cache_enabled' \n
 *     which is stored on SWSTATE per bit map (See UTILEX_OCC_BM_T): \n
 *     * levels_cache_buffer (if 'cache_enabled' is set)
 *     * levels_buffer (if 'cache_enabled' is NOT set)
 *   \param [in] _char_indx - \n
 *     Index within selected levels buffer to use for extraction of the byte's value
 *   \param [in] _cur_val - \n
 *     Variable into whose address to load data.
 *     \b As \b output - \n
 *     This macro loads into the address of _cur_val the value of the byte extracted from buffer 
 * \par INDIRECT INPUT
 *   * SWSTATE system (See \ref UTILEX_OCC_BITMAP_ACCESS)
 * \par DIRECT OUTPUT
 *   * None
 * \par INDIRECT OUTPUT
 *   * &_cur_val - \n
 *       See '_cur_val' above.
 * \remarks
 *   * 'unit' is assumed to be defined in the caller's scope.
 *   * 'exit' is assumed to be defined in the caller's scope.
 */
#define UTILEX_OCC_BM_ACTIVE_INST_GET(_bmp_map_index, _level_indx, _char_indx, _cur_val) \
  { \
    uint8 cache_enabled ; \
    uint32 buffer_offset ; \
    SHR_IF_ERR_EXIT(UTILEX_OCC_BITMAP_ACCESS.dss_array.cache_enabled.get(unit, _bmp_map_index, &cache_enabled)); \
    if (cache_enabled) \
    { \
      /* \
       * Get offset within 'levels_cache_buffer' (was 'pointer' on original). \
       */ \
      SHR_IF_ERR_EXIT(UTILEX_OCC_BITMAP_ACCESS.dss_array.levels_cache.get(unit, _bmp_map_index, _level_indx, &buffer_offset)); \
      SHR_IF_ERR_EXIT(UTILEX_OCC_BITMAP_ACCESS.dss_array.levels_cache_buffer.get(unit, _bmp_map_index, buffer_offset + _char_indx, &_cur_val)); \
    } \
    else \
    { \
      /* \
       * Get offset within 'levels_buffer' (was 'pointer' on original). \
       */ \
      SHR_IF_ERR_EXIT(UTILEX_OCC_BITMAP_ACCESS.dss_array.levels.get(unit, _bmp_map_index, _level_indx, &buffer_offset)); \
      SHR_IF_ERR_EXIT(UTILEX_OCC_BITMAP_ACCESS.dss_array.levels_buffer.get(unit, _bmp_map_index, buffer_offset + _char_indx, &_cur_val)); \
    } \
  }

/**
 * \brief
 * Set value of byte no. '_char_indx' on level '_level_indx' from 'levels_cache_buffer'
 * or from 'levels_buffer' depending on element 'cache_enabled' on specified bit map index
 * structure ('_bmp_map_index') which is assumed to be occupied.
 * \par DIRECT INPUT
 *   \param [in] _bmp_map_index - \n
 *     Index of bit map (starting from zero) to extract info from.
 *   \param [in] _level_indx - \n
 *     Level within bit map (starting from zero) to extract info from. \n
 *     Level is from one of the following arrays, depending on 'cache_enabled' \n
 *     which is stored on SWSTATE per bit map (See UTILEX_OCC_BM_T): \n
 *     * levels_cache_buffer (if 'cache_enabled' is set)
 *     * levels_buffer (if 'cache_enabled' is NOT set)
 *   \param [in] _char_indx - \n
 *     Index within selected levels buffer to use for extraction of the byte's value
 *   \param [in] _cur_val - \n
 *     Value to lopad into indicated address of selected buffer 
 * \par INDIRECT INPUT
 *   * SWSTATE system (See \ref UTILEX_OCC_BITMAP_ACCESS)
 * \par DIRECT OUTPUT
 *   * None
 * \par INDIRECT OUTPUT
 *   * Updated buffer (either 'levels_cache_buffer' or 'levels_buffer')
 * \remarks
 *   * 'unit' is assumed to be defined in the caller's scope.
 *   * 'exit' is assumed to be defined in the caller's scope.
 */
#define UTILEX_OCC_BM_ACTIVE_INST_SET(_bmp_map_index, _level_indx, _char_indx, _cur_val) \
  { \
    uint8 cache_enabled ; \
    uint32 buffer_offset ; \
    SHR_IF_ERR_EXIT(UTILEX_OCC_BITMAP_ACCESS.dss_array.cache_enabled.get(unit, _bmp_map_index, &cache_enabled)); \
    if (cache_enabled) \
    { \
      /* \
       * Get offset within 'levels_cache_buffer' (was 'pointer' on original). \
       */ \
      SHR_IF_ERR_EXIT(UTILEX_OCC_BITMAP_ACCESS.dss_array.levels_cache.get(unit, _bmp_map_index, _level_indx, &buffer_offset)); \
      SHR_IF_ERR_EXIT(UTILEX_OCC_BITMAP_ACCESS.dss_array.levels_cache_buffer.set(unit, _bmp_map_index, buffer_offset + _char_indx, _cur_val)); \
    } \
    else \
    { \
      /* \
       * Get offset within 'levels_buffer' (was 'pointer' on original). \
       */ \
      SHR_IF_ERR_EXIT(UTILEX_OCC_BITMAP_ACCESS.dss_array.levels.get(unit, _bmp_map_index, _level_indx, &buffer_offset)); \
      SHR_IF_ERR_EXIT(UTILEX_OCC_BITMAP_ACCESS.dss_array.levels_buffer.set(unit, _bmp_map_index, buffer_offset + _char_indx, _cur_val)); \
    } \
  }
/**
 * \brief
 * Verify specific bitmap index is marked as 'occupied'. \n
 * If not, software goes to exit with error code.
 * 
 * Notes: \n
 *   'unit' is assumed to be defined in the caller's scope.  \n
 *   'exit' is assumed to be defined in the caller's scope.  \n
 */
#define UTILEX_OCC_BM_VERIFY_BITMAP_IS_ACTIVE(_bit_map_index) \
  { \
    uint8 bit_val ; \
    uint32 max_nof_dss ; \
    SHR_IF_ERR_EXIT(UTILEX_OCC_BITMAP_ACCESS.max_nof_dss.get(unit, &max_nof_dss)) ; \
    if (_bit_map_index >= max_nof_dss) \
    { \
      /* \
       * If bitmap handle is out of range then quit with error. \
       */ \
      bit_val = 0 ; \
    } \
    else \
    { \
      SHR_IF_ERR_EXIT(UTILEX_OCC_BITMAP_ACCESS.occupied_dss.bit_get(unit, (int)_bit_map_index, &bit_val)) ; \
    } \
    if (bit_val == 0) \
    { \
      /* \
       * If bitmap structure is not indicated as 'occupied' then quit \
       * with error. \
       */ \
      SHR_SET_CURRENT_ERR(_SHR_E_MEMORY) ; \
      SHR_EXIT() ; \
    } \
  }
/**
 * \brief
 * Verify specified unit has a legal value. If not, software goes to \n
 * exit with error code.
 * 
 * Notes: \n
 *   'exit' is assumed to be defined in the caller's scope.
 */
#define UTILEX_OCC_BM_VERIFY_UNIT_IS_LEGAL(_unit) \
  if (((int)_unit < 0) || (_unit >= SOC_MAX_NUM_DEVICES)) \
  { \
    /* \
     * If this is an illegal unit identifier, quit \
     * with error. \
     */ \
    SHR_SET_CURRENT_ERR(_SHR_E_UNAVAIL) ; \
    SHR_EXIT() ; \
  }
/**
 * \brief
 * Convert input bitmap handle to index in 'occupied_dss' array.
 *
 * Indices go from 0 -> (occupied_dss - 1)
 * Handles go from 1 -> occupied_dss
 */
#define UTILEX_OCC_BM_CONVERT_HANDLE_TO_BITMAP_INDEX(_bit_map_index,_handle) (_bit_map_index = _handle - 1)
/**
 * \brief
 * Convert input index in 'occupied_dss' array to bitmap handle.
 * 
 * Indices go from 0 -> (occupied_dss - 1)
 * Handles go from 1 -> occupied_dss
 */
#define UTILEX_OCC_BM_CONVERT_BITMAP_INDEX_TO_HANDLE(_handle,_bit_map_index) (_handle = _bit_map_index + 1)
/*
 * } 
 */

/*************
* TYPE DEFS *
 */
/*
 * { 
 */

/**
 * \brief
 * Structure including the information user has to supply on bit map creation
 */
typedef struct
{
    /**
     *  The size of the occupation bitmap (in bits)
     */
    uint32 size;
    /**
     *  Initial value to fill the bitmap with.
     */
    uint8 init_val;
    /**
     *  Flag. If non zero then caching is supported
     */
    uint8 support_cache;

} UTILEX_OCC_BM_INIT_INFO;

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
 *   Initialize control structure for all bitmap instances expected.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] max_nof_dss -
 *     Maximal number of bitmaps which can be sustained simultaneously.
 * \par INDIRECT INPUT
 *   * SWSTATE system (See \ref UTILEX_OCC_BITMAP_ACCESS)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   Initialized sw_state_legacy_access[unit].shareddnx.sand.occ_bitmap: \n
 *   * Allocated array of pointers: 'dss_array' of type UTILEX_OCC_BM_T
 *   * Allocated bit map: 'occupied_dss' of type SHR_BITDCL
 *   * ...
 */
shr_error_e utilex_occ_bm_init(
    int unit,
    uint32 max_nof_dss);

#if (0)
/*
 * { 
 */
/*
 * Apparently, this is not requirted for DNX. We leave the prototype here but,
 * eventually, it should be remove.
 */
/*********************************************************************
* NAME:
*   utilex_occ_bm_get_ptr_from_handle
* TYPE:
*   PROC
* DATE:
*   May 18 2015
* FUNCTION:
*   Get bitmap structure pointer from handle.
* INPUT:
*   int                               unit -
*     Identifier of the device to access.
*   UTILEX_OCC_BM_PTR              bit_map -
*     Handle to bitmap to get pointer to.
* REMARKS:
*   This procedure is exceptional and is added here only for Arad (and
*   on Arad, for pat_tree. See utilex_pat_tree_create()).
*   Range of legal handles: 1 -> max_nof_dss
* RETURNS:
*     OK or ERROR indication.
 */
shr_error_e utilex_occ_bm_get_ptr_from_handle(
    int unit,
    UTILEX_OCC_BM_PTR bit_map,
    UTILEX_OCC_BM_T ** bit_map_ptr_ptr);
/*
 * } 
 */
#endif

/**
 * \brief
 *   Creates a new bitmap instance.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] bit_map_ptr -
 *     Pointer to memory to load output into. \n
 *     \b As \b output - \n
 *       This procedure loads pointed memory by handle (identifier) of
 *       newly created bitmap.
 *   \param [in] init_info -
 *     Pointed memory contains setup parameters required for for the
 *     creation of the bitmap. See \ref UTILEX_OCC_BM_INIT_INFO
 * \par INDIRECT INPUT
 *   SWSTATE system (Specifically, see \ref UTILEX_OCC_BITMAP_ACCESS)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   \b *bit_map_ptr \n
 *     See 'bit_map_ptr' in DIRECT INPUT above
 * \see
 *   * utilex_occ_bm_init()
 *   * utilex_occ_bm_load()
 *   * utilex_occ_bm_test_1()
 */
shr_error_e utilex_occ_bm_create(
    int unit,
    UTILEX_OCC_BM_INIT_INFO * init_info,
    UTILEX_OCC_BM_PTR * bit_map_ptr);
/**
 * \brief
 *   Free the specified bitmap instance (all corresponding memory).
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] bit_map -
 *     Handle of the bitmap to destroy.
 * \par INDIRECT INPUT
 *   SWSTATE system (Specifically, see \ref UTILEX_OCC_BITMAP_ACCESS)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   All resources of specified bitmap are freed. Eventually,
 *   root pointer at UTILEX_OCC_BITMAP_ACCESS.dss_array is cleared.
 * \see
 *   * utilex_occ_bm_create()
 */
shr_error_e utilex_occ_bm_destroy(
    int unit,
    UTILEX_OCC_BM_PTR bit_map);

/**
 * \brief
 *   Free the all occupied bitmap instances (each with all
 *   corresponding memory).
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] with_deinit -
 *     If non-zero then this procedure also disconnects 'sorted list'
 *     from sw-state block so to use it again, procedure utilex_occ_bm_init()
 *     will need to be invoked.
 *     Otherwise, all sorted list memry is released but it is still
 *     connected to the main body of sw-state memory.
 * \par INDIRECT INPUT
 *   SWSTATE system (Specifically, see \ref UTILEX_OCC_BITMAP_ACCESS)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   All resources of all bitmaps are freed and
 *   root pointer at UTILEX_OCC_BITMAP_ACCESS.dss_array is cleared.
 * \see
 *   * utilex_occ_bm_create()
 *   * utilex_occ_bm_destroy()
 */
shr_error_e utilex_occ_bm_destroy_all(
    int unit,
    int with_deinit);

/**
 * \brief
 *   Load (fill) bit map by specified initial value. (No memory is freed!)
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] bit_map -
 *     Handle of the bitmap to load by init value. See remarks.
 * \par INDIRECT INPUT
 *   SWSTATE system (Specifically, see \ref UTILEX_OCC_BITMAP_ACCESS)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   All bit map arrays are filled by 'init_val'. See remarks below.
 * \see
 *   * utilex_occ_bm_create()
 * \remarks
 *   Initial value is specified, upon create, in the 'init_val' element
 *   of UTILEX_OCC_BM_INIT_INFO (which is input to utilex_occ_bm_create()).
 *   If 'init_val' is FALSE then bit map is filled by '0'. Otherwise,
 *   it is filled by '1'.
 */
shr_error_e utilex_occ_bm_clear(
    int unit,
    UTILEX_OCC_BM_PTR bit_map);

/**
 * \brief
 *   Get the first zero bit on the bitmap (starting from bit 0 )
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] bit_map -
 *     Handle of the bitmap perform the get operation at.
 *   \param [in] place -
 *     Pointer to uint32 \n
 *     \b As \b output - \n
 *     This procedure loads pointed memory by the index of the
 *     bit which was found to be '0'. This is bit index so, say,
 *     for a byte value of 0x03, the index of the first zero bit
 *     is '2'. \n
 *     This is meaningful only if '*found' is non-zero. Otherwise,
 *     this procedure loads '0' into '*place'.
 *   \param [in] found -
 *     Pointer to uint32 \n
 *     \b As \b output - \n
 *     This procedure loads pointed memory by a non-zero value if
 *     a zero bit was, indeed, found. Otherwise, a zero is loaded.
 * \par INDIRECT INPUT
 *   SWSTATE system (Specifically, see \ref UTILEX_OCC_BITMAP_ACCESS)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * See 'place' in DIRECT INPUT
 *   * See 'found' in DIRECT INPUT
 * \see
 *   * utilex_occ_bm_alloc_next()
 *   * utilex_occ_bm_test_1()
 */
shr_error_e utilex_occ_bm_get_next(
    int unit,
    UTILEX_OCC_BM_PTR bit_map,
    uint32 *place,
    uint8 *found);

/**
 * \brief
 *   Get the first/last bit in a range with a value '0'.
 *   See 'forward'. See remarks below
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] bit_map -
 *     Handle of the bitmap perform the get operation at.
 *   \param [in] start -
 *     Index of start bit of the range (index counting starts from zero).
 *   \param [in] end -
 *     Index of end bit of the range (index counting starts from zero).
 *   \param [in] forward -
 *     * If TRUE: look forward (starting from first bit ('start'))
 *     * If FALSE: look backward (starting from last bit ('end'))
 *   \param [in] val -
 *     The val to look for (0/1)
 *   \param [in] place -
 *     Pointer to uint32 \n
 *     \b As \b output - \n
 *     This procedure loads pointed memory by the index of the
 *     bit which was found to be '0'. This is bit index so, say,
 *     for a byte value of 0x03, the index of the first zero bit
 *     is '2'. \n
 *     This is meaningful only if '*found' is non-zero. Otherwise,
 *     this procedure loads '0' into '*place'.
 *   \param [in] found -
 *     Pointer to uint32 \n
 *     \b As \b output - \n
 *     This procedure loads pointed memory by a non-zero value if
 *     a zero bit was, indeed, found. Otherwise, a zero is loaded.
 * \par INDIRECT INPUT
 *   SWSTATE system (Specifically, see \ref UTILEX_OCC_BITMAP_ACCESS)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * See 'place' in DIRECT INPUT
 *   * See 'found' in DIRECT INPUT
 * \see
 *   * utilex_occ_bm_alloc_next()
 *   * utilex_occ_bm_test_1()
 * \remarks
 *   The search includes both edges of the range. For example, if 'start'
 *   is '16' and 'end' is '19' then 4 bits are serached: 16,17,18,19
 */
shr_error_e utilex_occ_bm_get_next_in_range(
    int unit,
    UTILEX_OCC_BM_PTR bit_map,
    uint32 start,
    uint32 end,
    uint8 forward,
    uint8 val,
    uint32 *place,
    uint8 *found);

/**
 * \brief
 *   Creates a new bitmap instance: Find the first bit with a value '0'
 *   within the bit_map and set it to '1'.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] bit_map -
 *     Handle of the bitmap perform the operation on.
 *   \param [in] place -
 *     Pointer to uint32 \n
 *     \b As \b output - \n
 *     This procedure loads pointed memory by the index of the
 *     bit which was found to be '0' (and set to be '1'). This is
 *     bit index so, say, for a byte value of 0x03, the index of the
 *     first zero bit is '2'. \n
 *     This is meaningful only if '*found' is non-zero. Otherwise,
 *     this procedure loads '0' into '*place'.
 *   \param [in] found -
 *     Pointer to uint32 \n
 *     \b As \b output - \n
 *     This procedure loads pointed memory by a non-zero value if
 *     a zero bit was, indeed, found. Otherwise, a zero is loaded.
 * \par INDIRECT INPUT
 *   SWSTATE system (Specifically, see \ref UTILEX_OCC_BITMAP_ACCESS)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * See 'place' in DIRECT INPUT
 *   * See 'found' in DIRECT INPUT
 * \see
 *   * utilex_occ_bm_get_next()
 *   * utilex_occ_bm_occup_status_set()
 */
shr_error_e utilex_occ_bm_alloc_next(
    int unit,
    UTILEX_OCC_BM_PTR bit_map,
    uint32 *place,
    uint8 *found);
/**
 * \brief
 *   Set the occupation status a of the given bit location:
 *   either '1' or '0'.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] bit_map -
 *     Handle of the bitmap perform the operation on.
 *   \param [in] place -
 *     The index of the bit to be set as per 'occupied' ('0' or '1').
 *   \param [in] occupied -
 *     The status (occupied/unoccupied, '1'/'0') to set for the 
 *     indicated 'place'
 * \par INDIRECT INPUT
 *   SWSTATE system (Specifically, see \ref UTILEX_OCC_BITMAP_ACCESS)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * The changed status of specified bit (Note that if bit
 *     was already at the requested status, nothing will be changed.)
 * \see
 *   * utilex_occ_bm_occup_status_set_helper()
 *   * utilex_occ_bm_alloc_next()
 */
shr_error_e utilex_occ_bm_occup_status_set(
    int unit,
    UTILEX_OCC_BM_PTR bit_map,
    uint32 place,
    uint8 occupied);

/**
 * \brief
 *   Get the occupation status a of the given of bit:
 *   Essentially indicate whether it is '1' or '0'.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] bit_map -
 *     Handle of the bitmap perform the operation on.
 *   \param [in] place -
 *     The index of the bit to be get status of
 *   \param [in] occupied -
 *     Pointer to uint8 - \n
 *     \b As \b output - \n
 *     This procedure loads pointed memory by the status
 *     (occupied/unoccupied, '1'/'0') of the bit at the 
 *     indicated 'place'
 * \par INDIRECT INPUT
 *   SWSTATE system (Specifically, see \ref UTILEX_OCC_BITMAP_ACCESS)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * See 'occupied' in DIRECT INPUT
 * \see
 *   * utilex_occ_bm_occup_status_set_helper()
 *   * utilex_occ_bm_alloc_next()
 */
shr_error_e utilex_occ_bm_is_occupied(
    int unit,
    UTILEX_OCC_BM_PTR bit_map,
    uint32 place,
    uint8 *occupied);

/**
 * \brief
 *   Activate/deactivate cache on specified bitmap and copy
 *   bitmap buffer to its cache image. See remarks.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] bit_map -
 *     Handle of the bitmap perform the operation on.
 *   \param [in] cached -
 *     Flag indicating whether to activate the cache (non-zero)
 *     or to deactivate it. If current status is the same as requested,
 *     no operation is carried out.
 * \par INDIRECT INPUT
 *   SWSTATE system (Specifically, see \ref UTILEX_OCC_BITMAP_ACCESS)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * Changed status of activation of cache (if required status is
 *     different from current status)
 *   * Newly loaded cache image (if required status is different
 *     from current status and is 'activate')
 * \see
 *   * utilex_occ_bm_create()
 *   * UTILEX_OCC_BM_ACTIVE_INST_GET
 *   * UTILEX_OCC_BM_ACTIVE_INST_SET
 *   * utilex_occ_bm_cache_rollback()
 *   * utilex_occ_bm_cache_commit()
 *   * utilex_occ_bm_clear()
 * \remarks
 *   If required operation is to activate the cache ('cached' is non-zero)
 *   and this bitmap is not marked as suppoting cache ('support_cache' is zero)
 *   then this procedure will return with error (_SHR_E_INTERNAL)
 */
shr_error_e utilex_occ_bm_cache_set(
    int unit,
    UTILEX_OCC_BM_PTR bit_map,
    uint8 cached);
/**
 * \brief
 *   Copy cache image on specified bitmap to bitmap buffer. See remarks.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] bit_map -
 *     Handle of the bitmap perform the operation on.
 *   \param [in] flags -
 *     Control flags. Currently not in use.
 * \par INDIRECT INPUT
 *   SWSTATE system (Specifically, see \ref UTILEX_OCC_BITMAP_ACCESS)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * Newly loaded bitmap buffer (if cache is activated)
 * \see
 *   * utilex_occ_bm_create()
 *   * UTILEX_OCC_BM_ACTIVE_INST_GET
 *   * UTILEX_OCC_BM_ACTIVE_INST_SET
 *   * utilex_occ_bm_cache_rollback()
 *   * utilex_occ_bm_clear()
 * \remarks
 *   If cache is not supported
 *   then this procedure will return with error (_SHR_E_INTERNAL)
 *   If cache is not active then no operation is taken.
 */
shr_error_e utilex_occ_bm_cache_commit(
    int unit,
    UTILEX_OCC_BM_PTR bit_map,
    uint32 flags);
/**
 * \brief
 *   Copy bitmap buffer to cache image on specified bitmap. See remarks.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] bit_map -
 *     Handle of the bitmap perform the operation on.
 *   \param [in] flags -
 *     Control flags. Currently not in use.
 * \par INDIRECT INPUT
 *   SWSTATE system (Specifically, see \ref UTILEX_OCC_BITMAP_ACCESS)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * Newly loaded cache image (if cache is activated)
 * \see
 *   * utilex_occ_bm_create()
 *   * UTILEX_OCC_BM_ACTIVE_INST_GET
 *   * UTILEX_OCC_BM_ACTIVE_INST_SET
 *   * utilex_occ_bm_cache_commit()
 *   * utilex_occ_bm_clear()
 * \remarks
 *   If cache is not supported
 *   then this procedure will return with error (_SHR_E_INTERNAL)
 *   If cache is not active then no operation is taken.
 */
shr_error_e utilex_occ_bm_cache_rollback(
    int unit,
    UTILEX_OCC_BM_PTR bit_map,
    uint32 flags);

/**
 * \brief
 *   Returns the size, in bytes, of the memory buffer needed for
 *   loading the bitmap into. 'size' includes all info required for
 *   reconstruction of that bitmap.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] bit_map -
 *     Handle of the bitmap to get the size of. Bitmap is assumed
 *     to have been created.
 *   \param [in] size -
 *     Pointer to uint32. \n
 *     \b As \b output - \n
 *     This procedure loads '*size' by the number of bytes occupied by specified
 *     bitmap
 * \par INDIRECT INPUT
 *   * SWSTATE system (Specifically, see \ref UTILEX_OCC_BITMAP_ACCESS)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * See \b 'size' in DIRECT INPUT
 * \see
 *   * utilex_occ_bm_save()
 */
shr_error_e utilex_occ_bm_get_size_for_save(
    int unit,
    UTILEX_OCC_BM_PTR bit_map,
    uint32 *size);

/**
 * \brief
 *   Save specified bitmap into specified memory buffer. \n
 *   Saving includes all info required for reconstruction of that bitmap.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] bit_map -
 *     Handle of the bitmap to get the size of. Bitmap is assumed
 *     to have been created.
 *   \param [in] buffer -
 *     Pointer to uint8. \n
 *     \b As \b output - \n
 *     This procedure loads '*buffer' by all info required for future construction
 *     of specified bitmap
 *   \param [in] buffer_size_bytes -
 *     Number of bytes actually available on *buffer. Must be larger or equal to
 *     the size actually required.
 *   \param [in] actual_size_bytes -
 *     Pointer to uint32. \n
 *     \b As \b output - \n
 *     This procedure loads '*actual_size_bytes' by the number of bytes actually
 *     required for the save operation of specified bitmap
 * \par INDIRECT INPUT
 *   * SWSTATE system (Specifically, see \ref UTILEX_OCC_BITMAP_ACCESS)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * See \b 'buffer' in DIRECT INPUT
 *   * See \b 'actual_size_bytes' in DIRECT INPUT
 * \see
 *   * utilex_occ_bm_load()
 */
shr_error_e utilex_occ_bm_save(
    int unit,
    UTILEX_OCC_BM_PTR bit_map,
    uint8 *buffer,
    uint32 buffer_size_bytes,
    uint32 *actual_size_bytes);

/**
 * \brief
 *   Load specified bitmap from specified memory buffer. \n
 *   Loading includes all info required for reconstruction of that bitmap.
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] buffer -
 *     Pointer to pointer to a variable of type uint8. \n
 *     Caller loads pointed memory by the address of the buffer from which
 *     this procedure is to load all bitmap info.
 *     The size of the buffer has to be at least the value returned
 *     by utilex_occ_bm_get_size_for_save().
 *     \b As \b output - \n
 *     This procedure loads '*buffer' by pointer into original input buffer
 *     at the location just beyond that which was used to load bitmap info.
 *     Just to clarify, this pointer may be used by this procedure to further
 *     get information for this buffer.
 *   \param [in] bit_map_ptr -
 *     Pointer to UTILEX_OCC_BM_PTR
 *     \b As \b output - \n
 *     This procedure loads pointed memory by handle of the newly created bitmap.
 * \par INDIRECT INPUT
 *   * See \b 'buffer' in DIRECT INPUT
 *   * SWSTATE system (Specifically, see \ref UTILEX_OCC_BITMAP_ACCESS)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * See \b 'buffer' in DIRECT INPUT
 *   * See \b 'bit_map_ptr' in DIRECT INPUT
 * \see
 *   * utilex_occ_bm_save()
 *   * utilex_occ_bm_get_size_for_save()
 */
shr_error_e utilex_occ_bm_load(
    int unit,
    uint8 **buffer,
    UTILEX_OCC_BM_PTR * bit_map_ptr);

/**
 * \brief
 *   Clear an  'info' structure of some bitmap.
 *   The 'info' structure contains 'meta data' regarding its
 *   corresponding bitmap.
 * \par DIRECT INPUT
 *   \param [in] info -
 *     Pointer to UTILEX_OCC_BM_INIT_INFO
 *     \b As \b output - \n
 *     This procedure loads 'info' structure by its initial values
 *     (indicating 'no info').
 * \par INDIRECT INPUT
 *   * None
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * See 'info' on DIRECT INPUT
 */
shr_error_e utilex_occ_bm_init_info_clear(
    UTILEX_OCC_BM_INIT_INFO * info);

/**
 * \brief
 *   Returns the size, in bytes, of the memory buffer assigned to
 *   each of the two buffers containing all levels of the bitmap:
 *   'levels_cache_buffer' and 'levels_buffer'
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] bit_map -
 *     Handle of the bitmap to get the size of. Bitmap is assumed
 *     to have been created.
 *   \param [in] buffer_size_p -
 *     Pointer to uint32. \n
 *     \b As \b output - \n
 *     This procedure loads '*buffer_size_p' by the number of bytes occupied by
 *     each of the bimap images: 'levels_cache_buffer' and 'levels_buffer'
 * \par INDIRECT INPUT
 *   * SWSTATE system (Specifically, see \ref UTILEX_OCC_BITMAP_ACCESS)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * See \b 'buffer_size_p' in DIRECT INPUT
 * \see
 *   * utilex_occ_bm_get_size_for_save()
 */
shr_error_e utilex_occ_bm_get_buffer_size(
    int unit,
    UTILEX_OCC_BM_PTR bit_map,
    uint32 *buffer_size_p);

/**
 * \brief
 *   Returns the flag indicating whether cache is supported of
 *   specified bitmap
 * \par DIRECT INPUT
 *   \param [in] unit -
 *     Identifier of the device to access.
 *   \param [in] bit_map -
 *     Handle of the bitmap to get the size of. Bitmap is assumed
 *     to have been created.
 *   \param [in] support_cache_p -
 *     Pointer to uint8. \n
 *     \b As \b output - \n
 *     This procedure loads '*support_cache_p' by the the flag assigned to
 *     ethis bitmap, which indicates whether cache is supported (non-zero)
 *     or not.
 * \par INDIRECT INPUT
 *   * SWSTATE system (Specifically, see \ref UTILEX_OCC_BITMAP_ACCESS)
 * \par DIRECT OUTPUT
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 * \par INDIRECT OUTPUT
 *   * See \b 'support_cache_p' in DIRECT INPUT
 * \see
 *   * utilex_occ_bm_create()
 */
shr_error_e utilex_occ_bm_get_support_cache(
    int unit,
    UTILEX_OCC_BM_PTR bit_map,
    uint8 *support_cache_p);
/*
 * Get handle to occ bitmap which will be considered illegal
 * by all occ bitmap utilities.
 */
/**
 * \brief
 *   Get handle to occ bitmap which will be considered illegal
 *   by all occ bitmap utilities.
 * \par DIRECT INPUT
 *   None
 * \par INDIRECT INPUT
 *   None
 * \par DIRECT OUTPUT
 *   Value of handle which is considered 'illegal'
 * \par INDIRECT OUTPUT
 *   * None
 */
uint32 utilex_occ_bm_get_illegal_bitmap_handle(
    void);
/*********************************************************************
* NAME:
*   utilex_occ_is_bitmap_active
* TYPE:
*   PROC
* DATE:
*   May 13 2015
* FUNCTION:
*   Get indication on whether specified bitmap is currently in use.
* INPUT:
*  int unit -
*    Identifier of the device to access.
*  UTILEX_OCC_BM_PTR bit_map -
*    Handle of the bitmap to perform the check on.
*  uint8 *in_use -
*    Return a non-zero value if bit map is in use.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
 */
shr_error_e utilex_occ_is_bitmap_active(
    int unit,
    UTILEX_OCC_BM_PTR bit_map,
    uint8 *in_use);

/*********************************************************************
* NAME:
*     utilex_occ_bm_print
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     print the bitmap content.
* INPUT:
*  int    unit -
*     Identifier of the device to access.
*  UTILEX_OCC_BM_PTR bit_map -
*     The bitmap to print.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
 */
shr_error_e utilex_occ_bm_print(
    int unit,
    UTILEX_OCC_BM_PTR bit_map);

#if UTILEX_DEBUG
/*
 * { 
 */
/*********************************************************************
* NAME:
*     utilex_occ_bm_tests
* TYPE:
*   PROC
* DATE:
*   Mar  6 2008
* FUNCTION:
*     Tests the bit map module
*
*INPUT:
*   int    unit -
*     Identifier of the device to access.
*  UTILEX_DIRECT:
*    uint32 silent -
*    Indicator.
*    1 - Do not print debuging info.
*    0 - Print various debuging info.
*  None.
*OUTPUT:
*  UTILEX_DIRECT:
*    uint32 -
*    Indicator.
*    1 - Test pass.
*    0 - Test fail.
*  NON
*REMARKS:* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
 */
uint32 utilex_occ_bm_tests(
    int unit,
    uint8 silent);
/*
 * } 
 */
#endif /* UTILEX_DEBUG */

/*
 * } 
 */
#else
/**
 * This is DUMMY code. It is only for compilers that do not accept empty files
 * and is never to be used.
 */
shr_error_e utilex_occ_bm_dummy_empty_function_to_make_compiler_happy(
    int unit);
#endif
/*
 * } UTILEX_OCCUPATION_BITMAP_H_INCLUDED
 */
#endif
