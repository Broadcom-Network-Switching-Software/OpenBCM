/*! \file bcmptm_spm_internal.h
 *
 * SPM (sub-component) interface functions and definitions to UFT-MGR.
 *
 * This file contains APIs and definitions of Strength-base Profile
 * Management CTH implementation.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_SPM_INTERNAL_H
#define BCMPTM_SPM_INTERNAL_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmlrd/bcmlrd_hmf.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/generated/bcmptm_sbr_ha.h>

/*******************************************************************************
 * Defines
 */
#define BCMPTM_SPM_LTID_INVALID ((uint32_t)-1)
#define BCMPTM_SPM_INDEX_INVALID ((uint32_t)-1)

/*******************************************************************************
 * Typedef
 */

typedef struct l2p_map_info_s {
    /*! TBD */
    const bcmlrd_hmf_t *map;
} l2p_map_info_t;

/*! Info about target physical sbr entry. */
typedef struct bcmptm_sbr_hw_entry_info_s {
    /*! Mapping of logical fields to physical fields. */
    l2p_map_info_t l2p_map_info;

    /*! Physical sbr table.
     * Example: IFTA80_SBR_PROFILE_TABLE_1m. */
    bcmdrd_sid_t sid_sbr_pt;
} bcmptm_sbr_hw_entry_info_t;

typedef struct bcmptm_sbr_lt_str_pt_info_s {
    /*! Table id for logical strength profile table.
     * Example: L2_HOST_NARROW_STRENGTH_PROFILEt
     *
     * (Making part of struct allows array of lt_str_pt_info_t to be
     * sorted by ltid).
     */
    bcmltd_sid_t ltid_str_pt;

    /*! Table id for corresponding LT.LOOKUP logical table.
     * Example: L2_HOST_NARROWt
     *
     * This is used by SBR-RM for DYNAMIC_SBR.
     */
    bcmltd_sid_t ltid_lookup;

    /*! Info for underlying physical table entries.
     * Single lt_str_pt.logical_entry can map to multiple hw_entries:
     * - because single LT.LOOKUP is implemented in different tiles which
     *   may map to different sbr profile tables.
     * - single logical entry may have separate logical fields for
     *   LT.LOOKUP0 and LT.LOOKUP1 which map to different hw_entries.
     */
    const bcmptm_sbr_hw_entry_info_t *hw_entry_info;

    /*! Number of elements in hw_entry_info array.
     */
    uint16_t hw_entry_info_count;
} bcmptm_sbr_lt_str_pt_info_t;

typedef struct bcmptm_sbr_lt_str_pt_list_s {
    /*! List of logical strength profile tables - managed by sbr_mgr.
     */
    const bcmptm_sbr_lt_str_pt_info_t *lt_str_pt_info;

    /*! Number of entries in lt_str_pt_info array. */
    uint16_t lt_str_pt_info_count;
} bcmptm_sbr_lt_str_pt_list_t;


/*! \cond  Externs for variant attach. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
extern const bcmptm_sbr_lt_str_pt_list_t _bd##_vu##_va##_ptm_sbr_list;
#define BCMLTD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */

/*******************************************************************************
 * Function prototypes
 */

/*
 * \brief Initialize SPM CTH handler on this BCM unit.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Inidicates cold or warm start status.
 *
 * \retval SHR_E_NONE On success.
 * \retval SHR_E_INIT On invalid device variant information.
 * \retval SHR_E_MEMORY On memory allocation failure.
 */
extern int
bcmptm_spm_init(int unit, bool warm);

/*
 * \brief Free all runtime resources allocated by SPM CTH implementation.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_spm_cleanup(int unit, bool warm);

/*
 * \brief Free all runtime resources allocated by SPM CTH implementation.
 *
 * \param [in] unit Unit number.
 * \param [in] tile SBR tile enumeration.
 * \param [in] current_tile_mode SBR tile_mode presently configured.
 * \param [in] new_tile_mode SBR tile_mode target.
 * \param [out] blocked TRUE if any SP LT which maps to tile is not empty.
 *                      FALSE if all such SP LTs are empty.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_spm_tile_mode_change_blocked(int unit,
                                    uint32_t tile,
                                    uint32_t current_tile_mode,
                                    uint32_t new_tile_mode,
                                    bool *blocked);

#endif /* BCMPTM_SPM_INTERNAL_H */
