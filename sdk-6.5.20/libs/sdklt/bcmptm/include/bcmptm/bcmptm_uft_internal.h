/*! \file bcmptm_uft_internal.h
 *
 * APIs, defines for PTM to interface with UFT LT sub-module.
 *
 * This file contains APIs, defines for PTM-UFT interfaces
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_UFT_INTERNAL_H
#define BCMPTM_UFT_INTERNAL_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <bcmbd/bcmbd.h>
#include <bcmptm/bcmptm.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/chip/bcmltd_device_constants.h>
#include <bcmltd/chip/bcmltd_table_constants.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmptm/bcmptm_uft.h>
#include <bcmltd/chip/bcmltd_id.h>

/*******************************************************************************
 * Defines
 */

/*! Size of buffer allocated to store entry data words for any PT. */
#define UFT_MAX_PT_ENTRY_WORDS  40  /* 1280 bit wide mem */
/*! Maxium fields of a PT. */
#define UFT_PT_FIELDS_MAX       16
/*! Maxium number of PT tables. */
#define UFT_PT_CNT_MAX          128
/*! Maxium physical banks of a device. */
#define UFT_PHYS_BANKS_MAX      128

/*******************************************************************************
 * Typedef
 */

/*! EM group attribute enum. */
typedef enum em_grp_attri_e {
    EM_GROUP_ATTRI_EM,
    EM_GROUP_ATTRI_ALPM_LV1 = 1,
    EM_GROUP_ATTRI_ALPM_LV2 = 2,
    EM_GROUP_ATTRI_ALPM_LV3 = 3,
    EM_GROUP_ATTRI_TCAM,
    EM_GROUP_ATTRI_FP,
    EM_GROUP_ATTRI_FT,
    EM_GROUP_ATTRI_INDEX,
    EM_GROUP_ATTRI_L3UC_DST,
    EM_GROUP_ATTRI_L3UC_SRC,
    EM_GROUP_ATTRI_COMP_DST,
    EM_GROUP_ATTRI_COMP_SRC,
    EM_GROUP_ATTRI_FT_FP,
    EM_GROUP_ATTRI_L3UC_DST_MTOP,
    EM_GROUP_ATTRI_L3UC_SRC_MTOP,
    EM_GROUP_ATTRI_COMP_DST_MTOP,
    EM_GROUP_ATTRI_COMP_SRC_MTOP,
} em_grp_attri_t;

/*! EM tile's ALPM PDD type. */
typedef enum em_tile_alpm_pdd_type_e {
    EM_TILE_ALPM_PDD_TYPE_NONE = 0,
    EM_TILE_ALPM_PDD_TYPE_L3UC_DST,
    EM_TILE_ALPM_PDD_TYPE_L3UC_SRC,
    EM_TILE_ALPM_PDD_TYPE_COMP_DST,
    EM_TILE_ALPM_PDD_TYPE_COMP_SRC,
    EM_TILE_ALPM_PDD_TYPE_L3UC_DST_MTOP,
    EM_TILE_ALPM_PDD_TYPE_L3UC_SRC_MTOP,
    EM_TILE_ALPM_PDD_TYPE_COMP_DST_MTOP,
    EM_TILE_ALPM_PDD_TYPE_COMP_SRC_MTOP,

    EM_TILE_ALPM_PDD_TYPE_NUM
} em_tile_alpm_pdd_type_t;

/*! EM tile's ALPM key mux type. */
typedef enum em_tile_alpm_key_mux_type_e {
    EM_TILE_ALPM_KEY_MUX_TYPE_NONE = 0,
    EM_TILE_ALPM_KEY_MUX_TYPE_L3UC_DST,
    EM_TILE_ALPM_KEY_MUX_TYPE_L3UC_SRC,
    EM_TILE_ALPM_KEY_MUX_TYPE_COMP_DST,
    EM_TILE_ALPM_KEY_MUX_TYPE_COMP_SRC,
    EM_TILE_ALPM_KEY_MUX_TYPE_L3UC_DST_MTOP,
    EM_TILE_ALPM_KEY_MUX_TYPE_L3UC_SRC_MTOP,
    EM_TILE_ALPM_KEY_MUX_TYPE_COMP_DST_MTOP,
    EM_TILE_ALPM_KEY_MUX_TYPE_COMP_SRC_MTOP,

    EM_TILE_ALPM_KEY_MUX_TYPE_NUM
} em_tile_alpm_key_mux_type_t;

/*! EM group info structure .*/
typedef struct em_grp_info_s {
    /*! Group id. */
    uint32_t grp_id;

    /*! Hash group or alpm group. */
    bool is_alpm;

    /*! Group attributes. */
    uint32_t attri;

    /*! Number of fixed banks assigned to the group. */
    uint32_t fixed_cnt;

    /*! Fixed banks array. */
    const uint32_t *fixed_banks;

    /*! Number of capable banks can be assigned to the group. */
    uint32_t cap_cnt;

    /*! All capable banks array. */
    const uint32_t *cap_banks;

    /*! Paired banks array. */
    const uint32_t *pair_banks;

    /*! The group is available only in this specific tile mode. */
    uint32_t tile_mode;

    /*! ALPM level. */
    uint32_t alpm_level;

    /*! The group belongs to this tile. */
    uint32_t tile;

    /*! Logical table array whose lookup0 are mapped to this group. */
    const bcmltd_sid_t *lookup0_lt;

    /*! Number of logical tables whose lookup0 are mapped to this group. */
    uint32_t lookup0_lt_count;

    /*! Logical table array whose lookup1 are mapped to this group. */
    const bcmltd_sid_t *lookup1_lt;

    /*! Number of logical tables whose lookup1 are mapped to this group. */
    uint32_t lookup1_lt_count;

    /*! ALPM PDD type.*/
    em_tile_alpm_pdd_type_t pdd_type;

} em_grp_info_t;

/*! EM group's mapped PT info structure. It is only used for legacy device. */
typedef struct em_grp_pt_info_s {
    /*! Fixed mapped physical memory table SID for this group. */
    bcmdrd_sid_t pt_sid;

    /*! Hash control PTSID for this group. */
    bcmdrd_sid_t hash_ctrl_sid;
} em_grp_pt_info_t;

/*! EM logical table info structure. */
typedef struct em_lt_info_s {
    /*! Logical table id. */
    bcmltd_sid_t ltid;

    /*! Maximum lookups supported by the logical table. */
    uint32_t lookup_cnt;
} em_lt_info_t;

/*! EM tile informatin */
typedef struct em_tile_info_s {
    /*! Tile id. */
    uint32_t tile_id;

    /*! Tile's control memory id. */
    bcmdrd_sid_t ctrl_ptid;

    /*! Default tile mode. */
    uint32_t def_mode;

    /*! Counf of available modes for this tile. */
    uint32_t ava_mode_count;

    /*! All available modes for this tile. */
    const uint32_t *ava_mode;

    /*! TRUE if ALPM PDD type is available for this tile. */
    bool alpm_pdd_type_available;

    /*! TRUE if ALPM key mux type is available for this tile. */
    bool alpm_key_mux_type_available;

    /*! TRUE if the tile is a MTOP tile. */
    bool mtop;

} em_tile_info_t;

/*! EM bank type. */
typedef enum em_bank_type_e {
    EM_BANK_TYPE_HASH,
    EM_BANK_TYPE_TCAM      = 1,
    EM_BANK_TYPE_INDEX     = 2,
    EM_BANK_TYPE_MINI_BANK = 3
} em_bank_type_t;

/*! Tile mode specific. */
typedef struct em_mode_bank_info_s {
    /*! Array of w_ptid. */
    const bcmdrd_sid_t *w_ptid_array;

    /*! W_ptid count. */
    uint32_t w_ptid_array_count;

    /*! Array of tile mode. */
    const uint32_t *tile_mode;

    /*! Tile mode count. */
    uint32_t tile_mode_count;

} em_mode_bank_info_t;

/*! EM bank info structure. */
typedef struct em_bank_info_s {
    /*! RM hash bank id */
    uint32_t bank_id;

    /*! Hash control pt id for each bank. */
    bcmdrd_sid_t ctrl_ptid;

    /*! Widest view pt id for each bank. */
    bcmdrd_sid_t w_ptid;

    /*! To indicate whether this tile is paired to support 2 lookups. */
    bool paired;

    /*! Indicate it is bank0 or bank 1. */
    uint32_t bank_idx;

    /*! Bank offset field. */
    bcmdrd_fid_t offset_fid;

    /*! Number of lookup supported by this bank. */
    uint32_t lookup_cnt;

    /*! Tile-mode specific bank information */
    const em_mode_bank_info_t *mode_bank_info;

} em_bank_info_t;

/*! EM Lookup types */
typedef enum bcmptm_em_lt_lookup_s {
    BCMPTM_EM_LOOKUP0          = 0,
    BCMPTM_EM_LOOKUP1          = 1,
    BCMPTM_EM_LOOKUP_RESERVED
} bcmptm_em_lt_lookup_t;

/*! EM bank info for a LT.LOOKUP under a specific tile mode. */
typedef struct em_lt_lookup_info_tile_mode_s {
    /*! LT portion of this LT.LOOKUP. */
    bcmltd_sid_t ltid;

    /*! lookup type of LT(LOOKUP 0 or LOOKUP1). */
    bcmptm_em_lt_lookup_t lookup_type;

    /*! Array of banks associated with this LT.LOOKUP. */
    const uint32_t *bank;

    /*! Corresponding array of hw_ltid for each bank in bank_id list. */
    const int32_t *hw_ltid;

    /*! Number of elements in bank_id, hw_ltid arrays. */
    uint16_t bank_count;

} em_lt_lookup_info_tile_mode_t;

/*! EM tile mode attribute. */
typedef enum em_tile_mode_attri_e {
    EM_TILE_MODE_ATTRI_NONE = 0,
    EM_TILE_MODE_ATTRI_ALPM_LEVEL1_LEVEL2_ONLY,
    EM_TILE_MODE_ATTRI_ALPM_COMPRESSION_LEVEL1_LEVEL2_ONLY,

    EM_TILE_MODE_NUM
} em_tile_mode_attri_t;

/*! LTS_TCAM - LT mapping info. */
typedef struct em_tile_lt_lts_tcam_s {
    /*! LT ID. */
    bcmltd_sid_t lt_id;

    /*! LTS_TCAM ID. */
    bcmdrd_sid_t sid_lts_tcam;

} em_tile_lt_lts_tcam_t;

/*!
 * Tile_Mode info.
 * Tooling provides flat list of tile mode info which UFT_mgr can
 * index based on tile_mode enum.
 */
typedef struct em_tile_mode_info_s {
    /*!
     * List of LTS_TCAM_ONLY views that needs to be cleared
     * before loading new Flexcode for this tile_mode.
     */
    const bcmdrd_sid_t *sid_lts_tcam_to_clear;

    /*! Number of elements in sid_lts_tcam_to_clear list. */
    uint16_t sid_lts_tcam_to_clear_count;

    /*! List of TILE_MODE_LT_LOOKUP_INFO for LOOKUP0 - sorted by LTID. */
    const em_lt_lookup_info_tile_mode_t *lt_lookup0;

    /*! Number of elements in lt_lookup0_info arrays. */
    uint16_t lt_lookup0_count;

    /*! List of TILE_MODE_LT_LOOKUP_INFO for LOOKUP1.- sorted by LTID. */
    const em_lt_lookup_info_tile_mode_t *lt_lookup1;

    /*! Number of elements in lt_lookup1_info arrays. */
    uint16_t lt_lookup1_count;

    /*! Flexcode for this tile mode. */
    const char *flexcode;

    /*! Tile mode attribute. */
    uint8_t attri;

    /*! Count of lts_tcam sid used only for EM_FT_1 and EM_FT_2. */
    uint16_t em_ft_lts_tcam_count;

    /*! Lts_tcam sid used only for EM_FT_1 and EM_FT_2. */
    const em_tile_lt_lts_tcam_t *em_ft_lts_tcam;

    /*! Count of ALPM PDD type. */
    uint16_t alpm_pdd_type_count;

    /*! ALPM PDD type array per mode. */
    const em_tile_alpm_pdd_type_t *alpm_pdd_type;

    /*! Count of ALPM key mux type. */
    uint16_t alpm_key_mux_type_count;

    /*! ALPM key mux type array per mode. */
    const em_tile_alpm_key_mux_type_t *alpm_key_mux_type;

} em_tile_mode_info_t;

/*!
 * \brief Bitmap of device physical banks.
 */
typedef struct bcmptm_uft_bank_map_s{
    /*! Bank map. */
    SHR_BITDCLNAME(bm, UFT_PHYS_BANKS_MAX);
} bcmptm_uft_bank_map_t;

/*! \brief UFT initial hw resource hash table group info. */
typedef struct uft_grp_info_s {
    /*! Hw resourse group id. */
    uint32_t grp_id;

    /*! Related overlay phyiscal table id. */
    bcmdrd_sid_t ovly_ptsid;

    /*! Default vector type. */
    uint32_t vector_type;

    /*! Count of dedicated banks. */
    uint32_t fixed_cnt;

    /*! Dedicated banks. */
    uint32_t fixed_banks[DEVICE_EM_BANK_CNT_MAX];

    /*! Count of variable banks. */
    uint32_t var_cnt;

    /*! Variable banks */
    uint32_t var_banks[DEVICE_EM_BANK_CNT_MAX];

    /*! Count of LTs map. */
    uint32_t lt_cnt;

    /*! LTs map to this group. */
    uint32_t lt_sids[MAX_MAPPED_LT];

    /*! Group type. */
    int is_alpm;

    /*! Group attribute. */
    uint32_t attri;
} uft_grp_info_t;

/*! \brief UFT physical bank info. */
typedef struct uft_bank_info_s {
    /*! Phyical bank id. */
    uint32_t bank_id;

    /*! Number of lookups supported by this bank. */
    uint32_t lookup_cnt;

    /*! Count of groups. */
    uint32_t grp_cnt;

    /*! List of HW Resource Group IDs which may contain this bank. */
    uint32_t grps[DEVICE_EM_GROUP_CNT_MAX];

    /*! Width of base entry in bits. */
    uint32_t base_entry_width;

    /*! Width of base bucket in base entries.*/
    uint32_t base_buket_width;

    /*! Number of base buckets. */
    uint32_t num_base_buckets;

    /*! Number of base entries. */
    uint32_t num_base_entries;
} uft_bank_info_t;

/*! Structure for uft variant driver. */
typedef struct bcmptm_uft_var_drv_s {
    /*! First UFT bank(IFTA90) of a device. */
    uint32_t first_uft_bank;

    /*! Count of UFT banks(IFTA90) of a device. */
    uint32_t uft_bank_cnt;

    /*! Count of EM groups. */
    uint32_t em_group_cnt;

    /*! EM Groups info. */
    const em_grp_info_t **em_group;

    /*! EM bank count of a device. */
    uint32_t em_bank_cnt;

    /*! EM banks info. */
    const em_bank_info_t *em_bank;

    /*! EM tile count of a device. */
    uint32_t em_tile_cnt;

    /*! EM tiles info.*/
    const em_tile_info_t *em_tile;

    /*! Count of lt_info. */
    uint32_t lt_info_cnt;

    /*! EM LT info. */
    const em_lt_info_t *lt_info;

    /*! Widest phyical sid for all banks in xfs device. */
    const bcmdrd_sid_t *bank_ptsid;

    /*! Widest phyical alpm sid for all banks in xfs device. */
    const bcmdrd_sid_t *alpm_ptsid;

    /*! Tile id for all banks in xfs device. */
    const uint32_t *tile_id;

    /*! Tile mode info. */
    const em_tile_mode_info_t **tile_mode;

    /*! Number of tile mode. */
    uint32_t tile_mode_cnt;

    /*! PDD type count per tile. */
    uint8_t alpm_pdd_type_cnt_per_tile;

    /*! Key mux type count per tile. */
    uint8_t alpm_key_mux_type_cnt_per_tile;

    /*! Data below is for legacy device. */
    bool legacy;

    /*! UFT groups. */
    uft_grp_info_t *groups_info;

    /*! UFT groups. */
    uft_bank_info_t *banks_info;

    /*! Bitmap of available banks for em grp. */
    bcmptm_uft_bank_map_t *grp_a_bmp_arr;

    /*! EM group's PT info. */
    const em_grp_pt_info_t *pt_info;

    /*! Group's primary group. */
    uint32_t *primary_grp;
} bcmptm_uft_var_drv_t;

/*! \brief register affected fields structure. */
typedef struct uft_hw_map_s {
    /*! PT SID. */
    bcmdrd_sid_t  sid;

    /*! pt index. */
    int index;

    /*! Count of field id. */
    int f_cnt;

    /*! Field ID array. */
    bcmdrd_fid_t f_ids[UFT_PT_FIELDS_MAX];

    /*! Data value of fields. */
    uint32_t f_data[UFT_PT_FIELDS_MAX];
} uft_hw_map_t;

/*! \brief UFT register/mem info to be updated. */
typedef struct uft_regs_info_s {
    /*! Number of PTs */
    int cnt;

    /*! Hw map info for PTs */
    uft_hw_map_t reg_map[UFT_PT_CNT_MAX];
} uft_regs_info_t;

/*! \brief Data structure to save the info of TABLE_EM_CONTROLt entry. */
typedef struct uft_tbl_em_ctrl_s {
    /*! Logical table id. */
    uint32_t lt_id;

    /*! Bitmap of fields to be operated. */
    SHR_BITDCLNAME(fbmp, TABLE_EM_CONTROLt_FIELD_COUNT);

    /*! Move depth. */
    uint8_t move_depth;
} uft_tbl_em_ctrl_t;

/*! Data structure to save the info of DEVICE_EM_GROUPt entry. */
typedef struct uft_dev_em_grp_s {
    /*! EM group id. */
    uint32_t grp_id;

    /*! Bitmap of fields to be operated. */
    SHR_BITDCLNAME(fbmp, DEVICE_EM_GROUPt_FIELD_COUNT);

    /*! Bitmap of bank array's member to be operated. */
    SHR_BITDCLNAME(bankbmp, DEVICE_EM_BANK_CNT_MAX);

    /*! Whether enable robust hash. */
    bool robust;

    /*! Hash vector type. */
    uint8_t vector_type;

    /*! Number of hash banks in this group. */
    uint8_t bank_cnt;

    /*! Hash vector type. */
    uint8_t bank[DEVICE_EM_BANK_CNT_MAX];
} uft_dev_em_grp_t;

/*! Data structure to save the info of DEVICE_EM_BANKt entry. */
typedef struct uft_dev_em_bank_s {

    /*! Bitmap of fields to be operated. */
    SHR_BITDCLNAME(fbmp, DEVICE_EM_BANKt_FIELD_COUNT);

    /*! EM bank id. */
    uint32_t bank_id;

    /*! Hash offset of this bank when it is used as hast table. */
    uint8_t offset;
} uft_dev_em_bank_t;

/*! Data structure to save the info of DEVICE_EM_TILEt entry. */
typedef struct uft_dev_em_tile_s {
    /*! EM tile id. */
    uint32_t tile_id;

    /*! Bitmap of fields to be operated. */
    SHR_BITDCLNAME(fbmp, DEVICE_EM_TILEt_FIELD_COUNT);

    /*! Tile working mode. */
    uint32_t mode;

    /*! Whether enable robust hash. */
    bool robust;

    /*! Whether enable robust hash. */
    uint32_t seed;
} uft_dev_em_tile_t;

/*!
 * \brief UFT device specific info.
 */
typedef struct uft_dev_info_s {
    /*! Max em groups. */
    uint32_t max_grps;

    /*! Max em tiles. */
    uint32_t max_tiles;

    /*! Max phyiscal banks. */
    uint32_t max_banks;

    /*! Points to first uft banks. */
    uint32_t first_uft_bank;

    /*! Hw resource EM table group info. */
    uft_grp_info_t *grp_info;

    /*! Physical bank info, bank count = max_banks.*/
    uft_bank_info_t *bank_info;

    /*! Bitmap of available banks for em grp. */
    bcmptm_uft_bank_map_t *grp_a_bmp_arr;

    /*! Bitmap of current banks for em grp. */
    bcmptm_uft_bank_map_t *grp_c_bmp_arr;

    /*! Array of physical bank offset. */
    uint8_t bank_offset[UFT_PHYS_BANKS_MAX];

    /*! UFT variant info. */
    const bcmptm_uft_var_drv_t *var;
} uft_dev_info_t;

/*! Function type for registers initialization */
typedef int (*uft_regs_init_f) (int unit,
                                uft_dev_info_t *dev,
                                uft_regs_info_t *regs);

/*! Function type for device info initialization */
typedef int (*uft_device_info_init_f) (int unit,
                                       const bcmptm_uft_var_drv_t *var_info,
                                       uft_dev_info_t *dev);

/*! Function type for TABLE_EM_CONTROL LT validate. */
typedef int (*uft_tbl_em_ctrl_validate_f) (int unit,
                                           uft_tbl_em_ctrl_t *ctrl);
/*! Function type for DEVICE_EM_GROUP LT validate. */
typedef int (*uft_dev_em_grp_validate_f) (int unit,
                                          uft_dev_em_grp_t *grp,
                                          uft_dev_info_t *dev);
/*! Function type for TABLE_EM_CONTROL LT set. */
typedef int (*uft_dev_em_grp_set_f) (int unit,
                                     uft_dev_em_grp_t *grp,
                                     uft_dev_info_t *dev,
                                     uft_regs_info_t *regs);
/*! Function type for TABLE_EM_BANK LT validate. */
typedef int (*uft_dev_em_bank_validate_f) (int unit,
                                           uft_dev_em_bank_t *bank);
/*! Function type for TABLE_EM_BANK LT set. */
typedef int (*uft_dev_em_bank_set_f) (int unit,
                                      uft_dev_em_bank_t *bank,
                                      uft_dev_info_t *dev,
                                      uft_regs_info_t *regs);
/*! Function type for DEVICE_EM_TILE LT set. */
typedef int (*uft_dev_em_tile_set_f) (int unit,
                                      const bcmltd_op_arg_t *op_arg,
                                      uft_dev_em_tile_t *tile,
                                      uft_dev_info_t *dev,
                                      uft_regs_info_t *regs);
/*! Function type for device info deinit */
typedef int (*uft_device_deinit_f) (int unit);

/*!
 * \brief UFT device specific routines
 */
typedef struct uft_driver_s {
    /*! UFT register init function. */
    uft_regs_init_f            regs_init;

    /*! UFT device init function. */
    uft_device_info_init_f     device_info_init;

    /*! UFT device de-init function. */
    uft_device_deinit_f        device_deinit;

    /*! TABLE_EM_CONTROL validate function. */
    uft_tbl_em_ctrl_validate_f tbl_em_ctrl_validate;

    /*! DEVICE_EM_GROUP validate function. */
    uft_dev_em_grp_validate_f  dev_em_grp_validate;

    /*! DEVICE_EM_GROUP set function. */
    uft_dev_em_grp_set_f       dev_em_grp_set;

    /*! DEVICE_EM_BANK validate function. */
    uft_dev_em_bank_validate_f dev_em_bank_validate;

    /*! DEVICE_EM_BANK set function. */
    uft_dev_em_bank_set_f      dev_em_bank_set;

    /*! DEVICE_EM_TILE set function. */
    uft_dev_em_tile_set_f      dev_em_tile_set;

} uft_driver_t;

/*! Signature of UFT variant driver attach function. */
typedef int (*uft_var_drv_attach_f)(bcmptm_uft_var_drv_t *drv);

/*******************************************************************************
 * Function prototypes
 */

/*!
 * \brief Initialize the Resource manager for UFT.
 * \param [in] unit Logical device id
 * \param [in] warm Warm init.
 *
 * \retval SHR_E_NONE Success
 */
extern int
bcmptm_uft_init(int unit, bool warm);

/*!
 * \brief De-initialize the Resource manager for UFT.
 * \param [in] unit Logical device id
 * \param [in] warm Warm init.
 *
 * \retval SHR_E_NONE Success
 */
int
bcmptm_uft_cleanup(int unit, bool warm);

/*!
 * \brief Field handler initialization.
 *
 * Initialize field array local memory manager handler.
 *
 *
 * \retval SHR_E_NONE No errors. Error code otherwise.
 */
extern int
bcmptm_uft_fld_hdl_init(void);

/*!
 * \brief Field handler cleanup.
 *
 * Delete field array local memory manager handler.
 *
 *
 * \return none.
 */
extern void
bcmptm_uft_fld_hdl_cleanup(void);

/*!
 * \brief Register device specfic driver for UFT
 *
 * \param [in] unit Device unit.
 * \param [in] drv Device specific structure containing routine pointers.
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_uft_driver_register(int unit, uft_driver_t *drv);

/*!
 * \brief Initialize the custom handler for HASH.
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE if success.
 */
extern int
bcmptm_uft_em_tile_event_hdl_init(int unit);

/*!
 * \brief Stop the custom handler for HASH.
 * \param [in] unit Logical device id
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_uft_em_tile_event_hdl_stop(int unit);

/*!
 * \brief Get uft variant info.
 *
 * \param [in] unit unit.
 * \param [in] info Variant specific structure containing routine pointers.
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_uft_var_info_get(int unit, const bcmptm_uft_var_drv_t **info);

/*! \cond  Externs for variant attach. */
#define BCMLRD_VARIANT_ENTRY(_bd,_bu,_va,_ve,_vu,_vv,_vo,_vd,_r0,_r1) \
extern int _bd##_vu##_va##_ptm_uft_var_drv_attach(bcmptm_uft_var_drv_t *drv);
#define BCMLTD_VARIANT_OVERRIDE
#include <bcmlrd/chip/bcmlrd_variant.h>
/*! \endcond */

#endif /* BCMPTM_UFT_INTERNAL_H */
