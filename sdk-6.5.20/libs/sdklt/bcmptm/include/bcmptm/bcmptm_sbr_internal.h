/*! \file bcmptm_sbr_internal.h
 *
 * SBR-RM (sub-component) interface functions and definitions to UFT-MGR.
 *
 * This file contains APIs and definitions of SBR-RM sub-component.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_SBR_INTERNAL_H
#define BCMPTM_SBR_INTERNAL_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/chip/bcmltd_device_constants.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_spm_internal.h>
#include <bcmptm/generated/bcmptm_sbr_ha.h>

/*******************************************************************************
 * Defines
 */
/*! Invalid 32-bit index value.  */
#define SBR_UINT32_INVALID ((uint32_t)(-1))

/*! Invalid 16-bit index value.  */
#define SBR_UINT16_INVALID ((uint16_t)(-1))
/*******************************************************************************
 * Typedef
 */
/*!
 * \brief Usage of SBR entry after boot-up.
 */
typedef enum bcmptm_sbr_entry_usage_e {
    /*! SBR profile entry is free to use */
    BCMPTM_SBR_ENTRY_FREE = 0,

    /*! SBR profile entry is reserved for MISS */
    BCMPTM_SBR_ENTRY_RSVD_MISS,

    /*! SBR profile entry is reserved for HIT */
    BCMPTM_SBR_ENTRY_RSVD_HIT,

    /*! SBR profile entry is reserved */
    BCMPTM_SBR_ENTRY_RSVD_DEFAULT,

    /*! SBR profile entry is currently to used */
    BCMPTM_SBR_ENTRY_IN_USE

} bcmptm_sbr_entry_usage_t;

/*!
 * \brief Logical table lookup instance type definition.
 */
typedef enum bcmptm_sbr_lt_lookup_e {
    /*! Invalid lookup instance. */
    BCMPTM_SBR_LOOKUP_INVALID   = 0,
    /*! LT lookup instance 0. */
    BCMPTM_SBR_LOOKUP0          = 1,
    /*! LT lookup instance 1. */
    BCMPTM_SBR_LOOKUP1          = 2,
    /*! LT lookup reserved instance for use by compiler. */
    BCMPTM_SBR_LOOKUP_RESERVED
} bcmptm_sbr_lt_lookup_t;

/*!
 * \brief Tile enum for SBR, N means TILE_N.
 */
typedef uint32_t bcmptm_sbr_tile_t;

/*!
 * \brief Tile mode enum for SBR, N means TILE_N, while N+1 means
 *        TILE_MODE_DISABLE.
 */
typedef uint32_t bcmptm_sbr_tile_mode_t;

/*!
 * \brief SBR bit position of SBR_PROFILE_INDEXf per profile entry in
 *        LTS_TCAM_DATA_ONLY view.
 */
typedef struct bcmptm_sbr_bitp_s {
    /*! Minimum bit position of SBR_PROFILE_INDEXf. */
    uint16_t minbit;

    /*! Maximum bit position of SBR_PROFILE_INDEXf. */
    uint16_t maxbit;

} bcmptm_sbr_bitp_t;

/*!
 * \brief SBR information per SBR_PROFILE_TABLEm.
 */
typedef struct bcmptm_sbr_entry_data_s {
    /*! SID for associated SBR_PROFILE_TABLE/_EXTm...etc */
    bcmdrd_sid_t sid;

    /*! SBR entry value for this SBR_PROFILE_TABLEm. */
    const uint32_t *dw;

    /*! Number of data words for SBR entry values. */
    uint16_t dw_count;

} bcmptm_sbr_entry_data_t;

/*!
 * \brief SBR_information for a LT.LOOKUP, per tile (assuming same among modes
 *        in a tile).
 */
typedef struct bcmptm_sbr_lt_lookup_info_tile_s {
    /*! Tile ID (optional) */
    bcmptm_sbr_tile_t tile;

    /*! lookup_type portion of this LT.LOOKUP. */
    bcmptm_sbr_lt_lookup_t lookup_type;

    /*! LT portion of this LT.LOOKUP. */
    bcmltd_sid_t ltid;

    /*! TRUE => this LT.LOOKUP is default (part of TILE_MODE_0) */
    bool lt_lookup_is_default;

    /*! LT for associated strength profile table. */
    bcmltd_sid_t ltid_strength_profile;

    /*! SBR profile data for this LT.LOOKUP */
    const bcmptm_sbr_entry_data_t *sbr_profile_data;

    /*! Number of SBR profile data entries */
    uint16_t sbr_profile_data_count;

} bcmptm_sbr_lt_lookup_info_tile_t;

/*!
 * \brief Field enums for LOOKUP_EN_VALUEf, LOOKUP_EN_MASKf in
 *        LTS_TCAM_DATA_ONLY view. Meaningful only for itiles.
 */
typedef struct bcmptm_sbr_lookup_en_info_s {
    /*! Bit position for LOOKUP_EN_VALUEf */
    const bcmptm_sbr_bitp_t *bitp_f_lookup_en_value;

    /*! Bit position for LOOKUP_EN_MASKf */
    const bcmptm_sbr_bitp_t *bitp_f_lookup_en_mask;
} bcmptm_sbr_lookup_en_info_t;

/*!
 * \brief SBR_information per LT.LOOKUP, per tile mode.
 */
typedef struct bcmptm_sbr_lt_lookup_info_tile_mode_s {
    /*! Tile ID (optional) */
    bcmptm_sbr_tile_t tile;

    /*! lookup_type portion of this LT.LOOKUP. */
    bcmptm_sbr_lt_lookup_t lookup_type;

    /*! LT portion of this LT.LOOKUP. */
    bcmltd_sid_t ltid;

    /*! SID for LTS_TCAM_DATA_ONLY view. */
    bcmdrd_sid_t sid_lts_tcam_data_only;

    /*! Indexes for rules in LTS_TCAM_DATA_ONLY view. */
    const uint16_t *rule_index;

    /*! Number of entries in LTS_TCAM_DATA_ONLY view. */
    uint16_t rule_index_count;

    /*! Bit position for SBR_PROFILE_INDEXf in LTS_TCAM_DATA_ONLY view. */
    const bcmptm_sbr_bitp_t *bitp_f_sbr_profile_index;

    /*! Lookup enable information. */
    const bcmptm_sbr_lookup_en_info_t *lookup_en_info;

} bcmptm_sbr_lt_lookup_info_tile_mode_t;

/*!
 * \brief SBR information per tile mode.
 */
typedef struct bcmptm_sbr_tile_mode_info_s {
    /*! List of TILE_MODE_LT_LOOKUP_INFO for LOOKUP0. */
    const bcmptm_sbr_lt_lookup_info_tile_mode_t *lt_lookup0_info;

    /*! Number of elements in TILE_MODE_LT_LOOKUP0 arrays. */
    uint16_t lt_lookup0_info_count;

    /*! List of TILE_MODE_LT_LOOKUP_INFO for LOOKUP1. */
    const bcmptm_sbr_lt_lookup_info_tile_mode_t *lt_lookup1_info;

    /*! Number of elements in TILE_MODE_LT_LOOKUP1 arrays. */
    uint16_t lt_lookup1_info_count;

    /*! If this tile mode is the default mode (mode 0). */
    bool is_default_tile_mode;

} bcmptm_sbr_tile_mode_info_t;

/*!
 * \brief SBR information per tile.
 */
typedef struct bcmptm_sbr_tile_info_s {
    /*! Tile id. */
    uint32_t tile_id;

    /*! ORDERED list of enums for supported tile modes. */
    const bcmptm_sbr_tile_mode_t *tile_mode;

    /*! tile_mode_info for every tile_mode (same ORDER). */
    const bcmptm_sbr_tile_mode_info_t *tile_mode_info;

    /*! Number of elements in tile modes, tile_mode_info arrays. */
    uint32_t tile_mode_count;

    /*!
     * List of Tile specific sbr_info for LT.LOOKUP0, sorted by LTID. This
     * info is common across all Tile_Modes of a Tile.
     */
    const bcmptm_sbr_lt_lookup_info_tile_t *sbr_lt_lookup0_info;

    /*! Number of elements in sbr_lt_lookup0_info. */
    uint32_t sbr_lt_lookup0_info_count;

    /*!
     * List of Tile specific sbr_info for LT.LOOKUP1, sorted by LTID. This
     * info is common across all Tile_Modes of a Tile.
     */
    const bcmptm_sbr_lt_lookup_info_tile_t *sbr_lt_lookup1_info;

    /*! Number of elements in sbr_lt_lookup1_info. */
    uint32_t sbr_lt_lookup1_info_count;

    /*! Initial tile mode for this tile */
    bcmptm_sbr_tile_mode_t initial_tile_mode;

} bcmptm_sbr_tile_info_t;

/*!
 * \brief SBR profile entry per LT.LOOKUP.
 */
typedef struct bcmptm_sbr_entry_profile_map_s {
    /*! SID for SBR_PROFILE_TABLEm. */
    bcmdrd_sid_t sid;

    /*! Entry index which is in_use. */
    uint16_t entry_index;

    /*! Usage of SBR entry after boot-up. */
    bcmptm_sbr_entry_usage_t usage;

    /*!
     * LTID portion of LT.LOOKUP. Meaningful only when usage ==
     * BCMPTM_SBR_ENTRY_IN_USE.
     */
    bcmltd_sid_t ltid;

    /*!
     * Lookup portion of LT.LOOKUP. Meaningful only when usage ==
     * BCMPTM_SBR_ENTRY_IN_USE.
     */
    bcmptm_sbr_lt_lookup_t lookup_type;

} bcmptm_sbr_entry_profile_map_t;

/*!
 * \brief Structure for extension SBR profile tables info.
 */
typedef struct bcmptm_sbr_ext_to_main_s {
   /*! Sorted array of SBR extension profile tables. */
    const bcmdrd_sid_t *mem_ext;

   /*! Corresponding array of main SBR profile tables (same order). */
    const bcmdrd_sid_t *mem;

   /*! Number of elements in mem_ext, mem arrays. */
    uint16_t count;

} bcmptm_sbr_ext_to_main_t;

/*!
 * \brief Structure for SBR variant driver.
 */
typedef struct bcmptm_sbr_var_drv_s {
    /*! SBR per tile information. */
    const bcmptm_sbr_tile_info_t *sbr_tile_info;

    /*! Number of entries for SBR per tile information. */
    uint16_t sbr_tile_info_count;

    /*! SBR per SBR_PROFILE_TABLE information. */
    const bcmptm_sbr_entry_profile_map_t *sbr_entry_profile_map;

    /*! Number of entries for SBR per SBR_PROFILE_TABLE information. */
    uint16_t sbr_entry_profile_map_count;

    /*! SBR_PROFILE_TABLE SID list. */
    const bcmdrd_sid_t *sbr_profile_table;

    /*! Number of entries in SBR_PROFILE_TABLE SID list. */
    uint16_t sbr_profile_table_count;

    /*! Tile number for SBR to reference. */
    uint16_t tile_count;

    /*! The enumeration value for TILE_MODE_DISABLE. */
    bcmptm_sbr_tile_mode_t tile_mode_disable;

    /*! Pointer to SBR List. */
    const bcmptm_sbr_lt_str_pt_list_t *sbr_list;

    /*!
     * Pointer to the list of extension SBR profile tables and their main
     * tables SID info.
     */
    const bcmptm_sbr_ext_to_main_t *sbr_ext_to_main;
} bcmptm_sbr_var_drv_t;

/*!
 * \brief SBR variant driver attach function prototype declaration.
 */
typedef int (*sbr_var_drv_attach_f)(bcmptm_sbr_var_drv_t *drv);

/*!
 * \brief List of operations supported on a SBR table.
 */
typedef enum sbr_pt_ent_oper_s {
    /*! No operation on the SBR PT entry. */
    SBR_PT_ENT_NOOP             = 0,
    /*! Allocate and install new SBR PT entry in HW. */
    SBR_PT_ENT_ALLOC_HW_INSTALL = 1,
    /*! Free and clear existing SBR PT entry in HW. */
    SBR_PT_ENT_FREE_HW_CLEAR    = 2,
    /*! Increment reference count of an existing in-use SBR PT entry. */
    SBR_PT_ENT_REF_INCR         = 3,
    /*! Allocate new SBR PT entry or entries pair. */
    SBR_PT_ENT_ALLOC            = 4,
    /*! Free previously allocated SBR PT entry or entries pair. */
    SBR_PT_ENT_FREE             = 5,
    /*! Must be last one. */
    SBR_PT_OPER_COUNT           = 6
} sbr_pt_ent_oper_t;

/*!
 * \brief List of LT lookup operations related to a tile mode change.
 */
typedef enum sbr_tm_lt_lookup_oper_s {
    /*! No operation for this LT lookup on this tile. */
    SBR_TM_LT_LOOKUP_NOOP               = 0,
    /*! Add this new LT lookup for this tile. */
    SBR_TM_LT_LOOKUP_ADD                = 1,
    /*! Delete this LT lookup from this tile. */
    SBR_TM_LT_LOOKUP_DELETE             = 2,
    /*!
     * Update existing SBR entry index linked to this LT lookup in the new rule
     * entry (index) in the LTS TCAM programmed by UFT-MGR due to tile mode
     * change.
     */
    SBR_TM_LT_LOOKUP_LTS_TCAM_UPDATE    = 3,
    /*! Must be last one. */
    SBR_TM_LT_LOOKUP_OPER_COUNT         = 4
} sbr_tm_lt_lookup_oper_t;

/*!
 * \brief SBR device specific info.
 */
typedef struct sbr_dev_info_s {
    /*! Invalid LT ID value for this variant. */
    uint32_t invalid_ltid;

    /*! Invalid physical table ID value for this device variant. */
    uint32_t invalid_ptid;

    /*!
     * Size of the widest SBR profile table entry in words, among the SBR
     * profile tables managed by this RM on this device.
     */
    uint32_t spt_entry_max_wsize;

    /*! Device SBR profile tables HA entries state array. */
    bcmptm_sbr_entry_state_t *spt_estate_ha;

    /*! Device SBR profile tables current entries state array. */
    bcmptm_sbr_entry_state_t *spt_estate;

    /*!
     * SBR profile tables entries state total entries count in the older
     * version.
     */
    uint32_t issu_old_spt_ecount;

    /*! Total number of SBR profile table entries on this device. */
    uint32_t spt_entries_count;

    /*! Device SBR profile tables current entries state block size. */
    uint32_t spt_estate_blk_sz;

    /*! ISSU SBR profile tables deleted. */
    bool issu_spt_del;

    /*! ISSU SBR profile tables added. */
    bool issu_spt_add;

    /*! Device SBR profile tables HA state array. */
    bcmptm_sbr_pt_state_t *spt_state_ha;

    /*! Device SBR profile tables current state array. */
    bcmptm_sbr_pt_state_t *spt_state;

    /*! SBR profile tables count in the older version. */
    uint16_t issu_old_spt_count;

    /*! Number of SBR profile tables on this device. */
    uint16_t spt_count;

    /*! Device SBR profile tables current state block size. */
    uint32_t spt_state_blk_sz;

    /*! ISSU LT.LOOKUP deleted. */
    bool issu_tile_lkup_del;

    /*! ISSU LT.LOOKUP added. */
    bool issu_tile_lkup_add;

    /*! Tile LT lookup HA state. */
    bcmptm_sbr_lt_lookup_state_tile_t *tile_lkup_state_ha;

    /*! Tile LT lookup state. */
    bcmptm_sbr_lt_lookup_state_tile_t *tile_lkup_state;

    /*! Total tile LT lookup entries in the older version.  */
    uint16_t issu_old_lt_lkup_count;

    /*! Total tile LT lookup entries on this device.  */
    uint16_t lt_lkup_count;

    /*! Tile LT lookup current state block size. */
    uint32_t tile_lkup_state_blk_sz;

    /*! ISSU tile deleted. */
    bool issu_tile_del;

    /*! ISSU tile added. */
    bool issu_tile_add;

    /*! Device tiles HA state array. */
    bcmptm_sbr_tile_state_t *tile_state_ha;

    /*! Device tiles current state array. */
    bcmptm_sbr_tile_state_t *tile_state;

    /*! Tiles count in the older version. */
    uint16_t issu_old_tile_count;

    /*! Number of tiles on this device. */
    uint16_t tile_count;

    /*! Device tiles current state block size. */
    uint32_t tile_state_blk_sz;

    /*! Variant SBR profile tables HA state. */
    bcmptm_sbr_var_spt_state_t *v_spt_state_ha;

    /*! Variant Tile ID list HA state. */
    bcmptm_sbr_var_tile_id_state_t *v_tileid_state_ha;

    /*! Variant Tile LT.LOOKUP LTID list HA state. */
    bcmptm_sbr_var_tile_ltlkup_state_t *v_tltlkup_state_ha;

    /*! Pointer to device variant SBR information structure. */
    const bcmptm_sbr_var_drv_t *var;
} sbr_dev_info_t;

/*!
 * \brief Function type for device info initialization.
 */
typedef int (*sbr_device_info_init_f)(int unit,
                                      const bcmptm_sbr_var_drv_t *var_info,
                                      sbr_dev_info_t *dev);
/*!
 * \brief Function type for device info deinit.
 */
typedef int (*sbr_device_deinit_f)(int unit);

/*!
 * \brief Function type for device default SBR profiles init.
 */
typedef int (*sbr_default_profiles_init_f)(int unit,
                                           const bcmptm_sbr_var_drv_t *var_info,
                                           sbr_dev_info_t *dev);
/*!
 * \brief SBR device specific routines.
 */
typedef struct sbr_driver_s {
    sbr_device_info_init_f      device_info_init;
    sbr_device_deinit_f         device_deinit;
    sbr_default_profiles_init_f default_profiles_init;
} sbr_driver_t;

/*******************************************************************************
 * Function prototypes
 */
/*
 * \brief Initialize SBR resource manager state on this BCM unit.
 * \param [in] unit Unit number.
 * \param [in] warm Inidicates cold or warm start status.
 *
 * \retval SHR_E_NONE On success.
 * \retval SHR_E_INIT On invalid device variant information.
 * \retval SHR_E_MEMORY On memory allocation failure.
 */
extern int
bcmptm_sbr_init(int unit, bool warm);

/*
 * \brief Free all runtime resources allocated by SBR resource manager.
 * \param [in] unit Unit number.
 * \param [in] warm Indicates cold or warm start status.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_sbr_cleanup(int unit, bool warm);

/*!
 * \brief Register device specfic driver for SBR
 *
 * \param [in] unit Device unit.
 * \param [in] drv Device specific structure containing routine pointers.
 *
 * \retval SHR_E_NONE On success.
 * \retval SHR_E_UNAVAIL Device driver registration function is not available.
 */
extern int
bcmptm_sbr_driver_register(int unit, sbr_driver_t *drv);

/*!
 * \brief Get SBR variant info.
 *
 * \param [in] unit unit.
 * \param [in] info Variant specific structure containing routine pointers.
 *
 * \retval SHR_E_NONE On success.
 * \retval SHR_E_INIT Device variant driver has not been initialized.
 */
extern int
bcmptm_sbr_var_info_get(int unit, const bcmptm_sbr_var_drv_t **info);

/*! \cond  Externs for variant attach. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
extern int _bd##_vu##_va##_ptm_sbr_var_drv_attach(bcmptm_sbr_var_drv_t *drv);
#define BCMLTD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */

#endif /* BCMPTM_SBR_INTERNAL_H */
