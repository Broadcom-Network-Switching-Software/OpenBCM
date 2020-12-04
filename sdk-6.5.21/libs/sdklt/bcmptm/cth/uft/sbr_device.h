/*! \file sbr_device.h
 *
 * SBR sub-component device specific functions.
 *
 * This file contains device specific functions that are internal to SBR.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef SBR_DEVICE_H
#define SBR_DEVICE_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <sal/sal_alloc.h>
#include <shr/shr_bitop.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmptm/bcmptm_sbr_internal.h>
#include <bcmltd/bcmltd_handler.h>
/*******************************************************************************
 * Defines
 */
/*******************************************************************************
 * Typedefs
 */

/*******************************************************************************
 * Function prototypes
 */
/*!
 * \brief Get SBR device specfic info
 *
 * \param [in] unit Unit number.
 *
 * \retval SBR device specfic info ptr
 */
extern sbr_dev_info_t *
bcmptm_sbr_device_info_get(int unit);

/*!
 * \brief Get SBR device specfic drivers
 *
 * \param [in] unit Unit number.
 *
 * \retval SBR device specfic driver ptr
 */
extern sbr_driver_t *
bcmptm_sbr_driver_get(int unit);

/*!
 * \brief Initialize SBR sub-component on this device.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold (0) or warm (1) start status.
 *
 * \retval SHR_E_XXX.
 */
extern int
bcmptm_sbr_device_init(int unit, bool warm);

/*!
 * \brief Cleanup SBR sub-component non-HA state on this device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_XXX.
 */
extern int
bcmptm_sbr_device_cleanup(int unit);

/*!
 * \brief Tile mode set function.
 *
 * \param [in] unit Unit number.
 * \param [in] tile Tile for which mode is being changed.
 * \param [in] tile_mode New tile mode.
 * \param [in] only_check_rsrc Only check for resources without making the
 *                             actual tile mode change.
 * \param [in] lt_id Logical table id.
 * \param [in] op_arg Operation arguments.
 *
 * \retval SHR_E_NONE On success or when resources are available.
 * \retval SHR_E_MEMORY Memory allocation failure.
 * \retval SHR_E_RESOURCE Free entries are not available to complete the tile
 *                        mode change.
 * \retval SHR_E_PARAM Input parameter error.
 * \retval SHR_E_EXISTS A new LT.LOOKUP part of only the new tile mode, already
 *                      has a SBR entry installed in the relevant profile table.
 */
extern int
bcmptm_sbr_tile_mode_set(int unit,
                         bcmptm_sbr_tile_t tile,
                         bcmptm_sbr_tile_mode_t tile_mode,
                         bool only_check_rsrc,
                         bcmltd_sid_t lt_id,
                         const bcmltd_op_arg_t *op_arg);

/*!
 * \brief Get pointer to the SBR entry state for user-specified \c entry_index.
 * This function allows caller to access i.e. lookup or change SBR entry state.
 *
 * \param [in] unit Unit number.
 * \param [in] spt_sid SBR profile table source ID enum constant value.
 * \param [in] entry_index  SBR profile table entry index.
 * \param [out] entry_state Pointer to SBR entry state.
 * \param [out] spt_state Pointer to SBR profile table state.
 *
 * \retval SHR_E_NONE On success.
 * \retval SHR_E_PARAM If input \c spt_sid or \c entry_index is invalid.
 * \retval SHR_E_UNAVAIL Input \c spt_sid not managed by SBR-RM.
 */
extern int
bcmptm_sbr_entry_state_get(int unit,
                           bcmdrd_sid_t spt_sid,
                           uint16_t entry_index,
                           bcmptm_sbr_entry_state_t **entry_state,
                           bcmptm_sbr_pt_state_t **spt_state);

/*!
 * \brief Returns entry index into SBR profile table memory for given LT.LOOKUP.
 *
 * \param [in] unit Unit number.
 * \param [in] spt_sid SBR profile table source ID enum constant value.
 * \param [in] lt_id Enum for LT portion of LT.LOOKUP e.g. l2_narrow_host_table.
 * \param [in] lookup_type Specifies LOOKUP0, LOOKUP1.
 *
 * \param [out] entry_index Index into physical SBR profile table memory, valid
 *                          when retval is SHR_E_NONE.
 * \retval SHR_E_NOT_FOUND If SBR entry corresponding to this lt_id and
 *                         lookup_type is not yet allocated. In this case,
 *                         returned entry_index value is dont_care.
 * \retval SHR_E_NONE On success.
 */
extern int
bcmptm_sbr_entry_index_get(int unit,
                           bcmdrd_sid_t spt_sid,
                           bcmltd_sid_t lt_id,
                           bcmptm_sbr_lt_lookup_t lookup_type,
                           uint16_t *entry_index);

/*!
 * \brief Set LOOKUP_EN_VALUE and LOOKUP_EN_MASK field values in LTS_TCAM_DATA.
 *
 * This function must be called only in cold-boot mode and must never be called
 * in warm-boot mode, it is the caller responsibility to implement and enforce
 * this restriction.
 *
 * \param [in] unit Unit number.
 * \param [in] tile Tile for which LOOKUP_EN fields are being set.
 * \param [in] tile_mode Current tile mode.
 * \param [in] lookup_lt_id Tile lookup Logical Table ID.
 * \param [in] lookup_type Specifies \c lookup_lt_id lookup instance
 *                         i.e. LOOKUP0/1.
 * \param [in] lt_id CTH Logical Table ID.
 * \param [in] op_arg Operation arguments.
 * \param [in] value_lookup_en_valuef New value for LOOKUP_EN_VALUE field.
 * \param [in] value_lookup_en_maskf New value for LOOKUP_EN_MASK field.
 *
 * \retval SHR_E_NONE On success.
 * \retval SHR_E_PARAM For invalid input parameter values.
 * \retval SHR_E_MEMORY Memory allocation failure.
 * \retval SHR_E_UNAVAIL When LTS_TCAM_DATA entry details to program the field
 *                       values are not available.
 * \retval SHR_E_INTERNAL Unanticipated internal error.
 */
extern int
bcmptm_sbr_lookup_en_mask_set(int unit,
                              bcmptm_sbr_tile_t tile,
                              bcmptm_sbr_tile_mode_t tile_mode,
                              bcmltd_sid_t lookup_lt_id,
                              bcmptm_sbr_lt_lookup_t lookup_type,
                              bcmltd_sid_t lt_id,
                              const bcmltd_op_arg_t *op_arg,
                              uint16_t value_lookup_en_valuef,
                              uint16_t value_lookup_en_maskf);

/*!
 * \brief Alloc an entry or an entry pair from a SBR profile table (SPT).
 *
 * This function can be used to allocate single entry or a pair of entries from
 * a SPT. For allocating an entry pair \c paired_lookups input parameter must
 * be set to TRUE by the callee.
 *
 * This function views the input \c spt_sid SPT entries to be organized as
 * buckets with each bucket supporting two entries. So, for an input \c spt_sid
 * SPT with 32 entries, the table would support 16 buckets and 2 entries (an
 * entry pair) per bucket. The paired entries in these buckets are separated by
 * an offset index value of SPT size divided by 2 i.e. 32 / 2 = 16. So, for the
 * example SPT with 32 entries, the bucket entries pairing is bkt0:{e0, e16},
 * bkt1:{e1, e17}, bkt2:{e2, e18}...bkt15:{e15, e31}.
 *
 * For single entry alloc call, this function will try to maximize the
 * utilization of buckets by giving preference to free entry in a partially
 * filled bucket over free entries in an empty bucket.
 *
 * For paired alloc, the first bucket that has both entries free will be
 * allocated and returned by this function.
 *
 * \param [in] unit Unit number.
 * \param [in] spt_sid SBR profile table source ID enum constant value.
 * \param [in] lt_id Lookup Logical Table ID.
 * \param [in] lookup_type Lookup instance for \c lt_id, LOOKUP0 or LOOKUP1.
 * \param [in] paired_lookups Paired lookup and entry alloc call.
 * \param [in] entry_count Size of input \c entry_index array.
 * \param [out] entry_index Pointer to allocated entries array.
 *
 * \retval SHR_E_NONE On success.
 * \retval SHR_E_PARAM Invalid input parameter value \c lt_id or \c lookup_type.
 * \retval SHR_E_UNAVAIL Input \c spt_sid not managed by SBR-RM.
 * \retval SHR_E_RESOURCE Free entries are not available to complete the alloc.
 * \retval SHR_E_INTERNAL Unanticipated internal error.
 */
extern int
bcmptm_sbr_entry_alloc(int unit,
                       bcmdrd_sid_t spt_sid,
                       bcmltd_sid_t lt_id,
                       uint16_t lookup_type,
                       bool paired_lookups,
                       uint16_t entry_count,
                       uint16_t *entry_index);

/*!
 * \brief Free previously allocated SBR profile table (SPT) entries.
 *
 * This function can be used to free previously allocated SBR profile table
 * entry or entries pair by passing the \c entry_count and corresponding entry
 * index values in \c entry_index.
 *
 * \param [in] unit Unit number.
 * \param [in] spt_sid SBR profile table source ID enum constant value.
 * \param [in] entry_count Size of input \c entry_index array.
 * \param [in] entry_index Pointer to entry or entries pair to be freed.
 *
 * \retval SHR_E_NONE On success.
 * \retval SHR_E_PARAM Invalid input parameter value.
 * \retval SHR_E_NOT_FOUND Entries in \c entry_index array are not found to be
 *                         in use to free them.
 * \retval SHR_E_UNAVAIL Input \c spt_sid not managed by SBR-RM.
 */
extern int
bcmptm_sbr_entry_free(int unit,
                      bcmdrd_sid_t spt_sid,
                      uint16_t entry_count,
                      uint16_t *entry_index);

/*!
 * \brief Search for a LT and it's mapped lookup instance support on a tile.
 *
 * Given a tile \c tile, logical table ID \c lookup_lt_id and it's mapped
 * lookup instance \c lookup_type, this function will search for the LT ID in
 * the tile's LT lookup info list, this list is per lookup instance and
 * contains the list of all LT IDs supported by this tile across all it's tile
 * modes.
 *
 * If a match is found during the list search operation, then this tile can
 * support/host the input \c lookup_lt_id and \c lookup_type. If a match is not
 * found, then the input \c lookup_lt_id and \c lookup_type pair is not support
 * by this tile and cannot be hosted by this tile.
 *
 * \param [in] unit Unit number.
 * \param [in] tile Tile to check for \c lookup_lt_id and \c lookup_type
 *                  support.
 * \param [in] lookup_lt_id Tile lookup Logical Table ID.
 * \param [in] lookup_type Tile lookup instance of \ref bcmptm_sbr_lt_lookup_t
 *                         type.
 *
 * \retval SHR_E_NONE Input \c lookup_lt_id and \c lookup_type can be hosted by
 *                    this \c tile.
 * \retval SHR_E_NOT_FOUND Input \c lookup_lt_id and \c lookup_type cannot be
 *                         hosted by this \c tile.
 * \retval SHR_E_PARAM Input parameter error.
 */
extern int
bcmptm_sbr_tile_lt_lookup_find(int unit,
                               bcmptm_sbr_tile_t tile,
                               bcmltd_sid_t lookup_lt_id,
                               bcmptm_sbr_lt_lookup_t lookup_type);

#endif /* SBR_DEVICE_H */
