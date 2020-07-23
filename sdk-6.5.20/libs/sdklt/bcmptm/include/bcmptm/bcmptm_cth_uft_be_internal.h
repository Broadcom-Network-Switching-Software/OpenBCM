/*! \file bcmptm_cth_uft_be_internal.h
 *
 * APIs, defines for PTM to interface with UFT Custom Handler backend part.
 * This file contains APIs, defines for CTH-UFT interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMPTM_CTH_UFT_BE_INTERNAL_H
#define BCMPTM_CTH_UFT_BE_INTERNAL_H

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmptm/bcmptm_uft.h>
#include <bcmptm/bcmptm_uft_internal.h>

/*******************************************************************************
 * Typedefs
 */

/* Max fields count for UFT LT.*/
#define UFT_FIELD_COUNT_MAX  (128)

/*! \brief Tile's hit index profile info. */
typedef struct uft_tile_hit_profile_info_s {
    /*! Tile name. */
    const char *tile_name;

    /*! PTSID of hit_index_profile memroy. */
    const bcmdrd_sid_t prof_ptsid;

    /*! HIT_CONTEXT field. */
    const bcmdrd_fid_t hc_fid;
} uft_tile_hit_profile_info_t;

/*!
 * \brief UFT group info
 */
typedef struct uft_be_dev_info_s {
    /*! Count of device em groups or banks. */
    int pt_cnt;

    /*! Widest phyiscal sid for groups in xgs device. */
    const bcmdrd_sid_t *ptsid;

    /*!
     * Stores primary group for each EM group.
     * When the primary group is not same as self, it is valid.
     * When primary group's bank count is non-zero, primary group
     * replace the EM group.
     * Array's size equals pt_cnt.
     */
    uint32_t *primary_grp;

    /*! Widest phyical sid for all banks in xfs device. */
    const bcmdrd_sid_t *bank_ptsid;

    /*! Widest phyical alpm sid for all banks in xfs device. */
    const bcmdrd_sid_t *alpm_ptsid;

    /*! UFT variant info. */
    const bcmptm_uft_var_drv_t *var;

    /*! Tile's HIT_INDEX_PROFILE info. */
    uft_tile_hit_profile_info_t *hit_prof;

    /*! Count of hit_prof. */
    uint32_t hit_prof_cnt;

} uft_be_dev_info_t;

/*! Function type for device info initialization */
typedef int (*uft_be_device_info_init_f) (int unit,
                                          const bcmptm_uft_var_drv_t *var_info,
                                          uft_be_dev_info_t *dev);

/*!
 * \brief UFT_BE device specific routines
 */
typedef struct uft_be_driver_s {
    /*! Callback for device info initialization */
    uft_be_device_info_init_f     device_info_init;
} uft_be_driver_t;

/*******************************************************************************
 * Function prototypes
 */


/*!
 * \brief Allocate LT fields buffer.
 *
 * \param [in] unit Unit number.
 * \param [in] in Lt fields buffer pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_uft_lt_fields_buff_alloc(int unit, bcmltd_fields_t *in);

/*!
 * \brief Free LT fields buffer.
 *
 * \param [in] unit Unit number.
 * \param [in] in Lt fields buffer pointer.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
extern int
bcmptm_uft_lt_fields_buff_free(int unit, bcmltd_fields_t *in);

/*!
 * \brief Get fixed banks for a given EM_GROUP.
 * Note that this function is used for all devices.
 *
 * \param [in] unit Device unit.
 * \param [in] grp_id Group ID.
 * \param [out] bank_arr Returned bank array.
 * \param [int] bank_size Size of bank array.
 * \param [out] r_bank_cnt Returned fixed bank count.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_grp_fixed_bank_get(int unit, uint32_t grp_id,
                              uint32_t *bank_arr, uint32_t bank_size,
                              uint32_t *r_bank_cnt);

/*!
 * \brief Get group information for a given EM_GROUP.
 * Note that this function is used for all devices.
 *
 * \param [in] unit Device unit.
 * \param [in] grp_id Group ID.
 * \param [out] grp_info Returned group info pointer.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_grp_info_get(int unit,
                        uint32_t grp_id,
                        const em_grp_info_t **grp_info);

/*!
 * \brief Get all group belonging to a specific tile mode.
 *
 * \param [in] unit Device unit.
 * \param [in] tile_mode Tile mode ID.
 * \param [out] grp_arr Returned group array.
 * \param [in] grp_size Size of group array.
 * \param [out] r_grp_cnt Actual returned count of groups.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_grp_get_per_tile_mode(int unit,
                                 uint32_t tile_mode,
                                 uint32_t *grp_arr,
                                 uint32_t  grp_size,
                                 uint32_t *r_grp_cnt);

/*!
 * \brief Validate if the tile mode is valid for a given tile.
 * \param [in] unit Unit number.
 * \param [in] tile_id Tile ID.
 * \param [in] tile_mode Tile mode.
 *
 * \retval SHR_E_NONE: valid.
 * \retval SHR_E_PARAM: invalid.
 */
extern int
bcmptm_uft_tile_mode_validate(int unit, uint32_t tile_id, uint32_t tile_mode);

/*!
 * \brief Get all EM groups of a given tile in current tile mode.
 * \param [in] unit Unit number.
 * \param [in] tile_id Tile ID.
 * \param [out] grp_arr Returned group array.
 * \param [in] grp_size Size of group array.
 * \param [out] r_grp_cnt Actual returned count of groups.
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_uft_tile_grp_get(int unit, uint32_t tile_id,
                        uint32_t *grp_arr,
                        uint32_t  grp_size,
                        uint32_t *r_grp_cnt);

/*!
 * \brief Get info of a given tile.
 * \param [in] unit Unit number.
 * \param [in] tile_id Tile ID.
 * \param [out] tile_info Returned tile info structure.
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_uft_tile_info_get(int unit,
                        uint32_t tile_id,
                        const em_tile_info_t **tile_info);

/*!
 * \brief Initialize the custom handler for UFT backend.
 * \param [in] unit Logical device id.
 * \param [in] warm Warm init.
 *
 * \retval SHR_E_NONE if success, otherwise failure.
 */
extern int
bcmptm_cth_uft_be_init(int unit, bool warm);

/*!
 * \brief De-initialize the custom handler for UFT backend.
 * \param [in] unit Logical device id
 * \param [in] warm Warm cleanup
 *
 * \retval NONE.
 */
extern int
bcmptm_cth_uft_be_cleanup(int unit, bool warm);

/*!
 * \brief Check if a bank was used.
 *
 * \param [in] unit Device unit.
 * \param [in] bank_id  Bank id.
 * \param [in] alpm TRUE if bank is used for ALPM.
 * \param [out] used TRUE if bank is used.
 *
 * \return SHR_E_XXX.
 */
int
bcmpmt_uft_bank_used(int unit, uint32_t bank_id, bool alpm, bool *used);

/*!
 * \brief Get bank sid and tile id for a specific bank.
 *
 * \param [in] unit Device unit.
 * \param [in] bank_id  Bank id.
 * \param [in] alpm  ALPM type bank.
 * \param [out] bank_sid Bank sid.
 * \param [out] tile_id Bank's tile id.
 *
 * \return SHR_E_XXX.
 */
int
bcmptm_uft_bank_info_get(int unit,
                         uint32_t bank_id,
                         bool alpm,
                         bcmdrd_sid_t *bank_sid,
                         uint32_t *tile_id);

/*!
 * \brief Get bank type.
 *
 * \param [in] unit Device unit.
 * \param [in] bank_id  Bank id.
 * \param [out] type Returned bank type.
 *
 * \return SHR_E_XXX.
 */
int
bcmptm_uft_bank_type_get(int unit, uint32_t bank_id,
                         em_bank_type_t *type);

/*!
 * \brief Return paired bank_id in given group.
 *
 * \param [in] unit Device unit.
 * \param [in] grp_id EM group id.
 * \param [in] bank_id  Bank id.
 * \param [out] pair_bank Paired bank id.
 *
 * \return SHR_E_XXX.
 */
int
bcmptm_uft_pair_bank_get(int unit,
                         uint32_t grp_id,
                         uint32_t bank_id,
                         uint32_t *pair_bank);

/*!
 * \brief Get the phyical table symbol ID.
 * \param [in] unit Logical device id.
 * \param [in] arr_size Size of pt_arr.
 * \param [out] pt_arr Pointer of returned ptsid array.
 *
 * \retval NONE.
 * Note: It is only used for uft unit test.
 */
extern int
bcmptm_uft_pts_get(int unit, int arr_size, bcmdrd_sid_t *pt_arr);

/*!
 * \brief Check if tile-mode based bank management is supported.
 * \param [in] unit Device unit.
 *
 * \retval NONE.
 * Note: It is only used for uft unit test.
 */
extern int
bcmptm_uft_tile_mode_enabled(int unit, bool *tile_mode_en);

/*!
 * \brief Transform from DEVICE_EM_BANK_PAIR_KEY_T to DEVICE_EM_BANK_ID_KEY_T.
 * \param [in] unit Device unit.
 * \param [in] pair Bank ID in DEVICE_EM_BANK_PAIR_KEY_T type.
 * \param [out] global Bank id in DEVICE_EM_BANK_ID_KEY_T type.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_bank_pair_to_global(int unit, uint32_t pair_bank, uint32_t *global);

/*!
 * \brief Transform from DEVICE_EM_BANK_ID_KEY_T to DEVICE_EM_BANK_PAIR_KEY_T.
 * \param [in] unit Device unit.
 * \param [in] pair Bank ID in DEVICE_EM_BANK_PAIR_KEY_T type.
 * \param [out] global Bank id in DEVICE_EM_BANK_ID_KEY_T type.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_bank_global_to_pair(int unit, uint32_t global, uint32_t *pair);

/*!
 * \brief Transform from DEVICE_HASH_BANK_ID_KEY_T to DEVICE_EM_BANK_ID_KEY_T.
 * \param [in] unit Device unit.
 * \param [in] hash Bank ID in DEVICE_HASH_BANK_ID_KEY_T type.
 * \param [out] global Bank id in DEVICE_EM_BANK_ID_KEY_T type.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_bank_hash_to_global(int unit, uint32_t pair, uint32_t *global);

/*!
 * \brief Transform from DEVICE_EM_BANK_ID_KEY_T to DEVICE_HASH_BANK_ID_KEY_T.
 * \param [in] unit Device unit.
 * \param [in] hash Bank ID in DEVICE_EM_BANK_PAIR_KEY_T type.
 * \param [out] global Bank id in DEVICE_EM_BANK_ID_KEY_T type.
 *
 * \return SHR_E_XXX.
 */
extern int
bcmptm_uft_bank_global_to_hash(int unit, uint32_t global, uint32_t *hash);

/*!
 * \brief Register device specfic driver for UFT_BE
 *
 * \param [in] unit Device unit.
 * \param [in] drv Device specific structure containing routine pointers.
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_uft_be_driver_register(int unit, uft_be_driver_t *drv);

/*!
 * \brief Dump lts_tcam ptsid for a LT.
 *
 * \param [in] unit Device unit.
 * \param [in] ltid Logical table ID.
 *
 * \return SHR_E_XXX
 */
extern int
bcmptm_uft_lt_lts_tcam_dump(int unit, bcmltd_sid_t ltid);

#endif /* BCMPTM_CTH_UFT_BE_INTERNAL_H */

