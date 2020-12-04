/*! \file bcmcth_vlan_drv.h
 *
 * BCMCTH VLAN driver objects.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_VLAN_DRV_H
#define BCMCTH_VLAN_DRV_H

#include <sal/sal_types.h>
#include <bcmcth/bcmcth_vlan_util.h>
#include <shr/shr_bitop.h>
#include <sal/sal_types.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmltd/bcmltd_types.h>

/*!
 * \brief This macro is used to set a Field present indicator bit status.
 *
 * Sets the Field \c _f present bit indicator in the Field Bitmap \c _m.
 *
 * \param [in] _m Field bitmap.
 * \param [in] _f Field position in the bitmap.
 *
 * \returns Nothing.
 */
#define BCMCTH_VLAN_LT_FIELD_SET(_m, _f)    \
            do {                        \
                SHR_BITSET(_m, _f);;      \
            } while (0)

/*!
 * \brief This macro is used to get the Field present indicator bit status.
 *
 * Returns the Field \c _f present bit indicator status from the Field
 * Bitmap \c _m.
 *
 * \param [in] _m Field bitmap.
 * \param [in] _f Field position in the bitmap.
 *
 * \returns Field present indicator bit status from the Field bitmap.
 */
#define BCMCTH_VLAN_LT_FIELD_GET(_m, _f)  (SHR_BITGET(_m, _f))

/*!
 * \brief VLAN IMM related Logical Table Fields information.
 */
typedef enum bcmcth_vlan_lt_fields_e {
    STG_PROFILE_ID = 0,
    STG_STATE,
    STG_STATE_MASK,
    STG_COMPARED_VALUE,
    STG_MASK_TARGET,
    STG_MAKS_ACTION,
    VLAN_FILEDS_COUNT
} bcmcth_vlan_lt_fields_t;

/*! The maximum num of PORT. */
#define MAX_PORTS 256

/*!
 * \brief Structure for VLAN_ING_STG_PROFILE Logical Table entry information.
 */
typedef struct vlan_ing_stg_profile_s {
    /*! Bitmap of Fields to be operated */
    SHR_BITDCLNAME(fbmp, VLAN_FILEDS_COUNT);

    /*! Bitmap of Fields to be operated */
    SHR_BITDCLNAME(state_array_bmp, MAX_PORTS);

    /*! Store LT entry STG Identifier key field value. */
    uint8_t           stg_id;

    /*! Store LT entry Spanning tree group forwarding state per-port. */
    uint8_t           state[MAX_PORTS];
} vlan_ing_stg_profile_t;

/*!
 * \brief Structure for VLAN_ING_STG_FLOOD_BLOCK_PROFILE Logical Table entry information.
 */
typedef struct vlan_ing_egr_stg_member_profile_s {
    /*! Bitmap of Fields to be operated */
    SHR_BITDCLNAME(fbmp, VLAN_FILEDS_COUNT);

    /*! Bitmap of Fields to be operated */
    SHR_BITDCLNAME(state_array_bmp, MAX_PORTS);

    /*! Store LT entry STG Identifier key field value. */
    uint8_t           stg_id;

    /*! Store LT entry Spanning tree group forwarding state for generic mask. */
    uint8_t           state[MAX_PORTS];

    /*!
     * Store LT entry Status used to compare against each members state
     * to generate mask bitmap
     */
    uint8_t           compared_state;

    /*! Store LT entry Block mask target applied for generic mask. */
    uint8_t           mask_target;

    /*! Store LT entry Block mask action applied for generic mask. */
    uint8_t           mask_action;
} vlan_ing_egr_stg_member_profile_t;


/*!
 * \brief Structure for VLAN_EGR_STG_PROFILE Logical Table entry information.
 */
typedef struct vlan_egr_stg_profile_s {
    /*! Bitmap of Fields to be operated */
    SHR_BITDCLNAME(fbmp, VLAN_FILEDS_COUNT);

    /*! Bitmap of Fields to be operated */
    SHR_BITDCLNAME(state_array_bmp, MAX_PORTS);

    /*! Store LT entry STG Identifier key field value. */
    uint8_t           stg_id;

    /*! Store LT entry Spanning tree group forwarding state per-port. */
    uint8_t           state[MAX_PORTS];
} vlan_egr_stg_profile_t;

/*!
 * \brief Set the hardware for VLAN_XLATE_CONFIGt_EGR_XLATE_PORT_GRPf.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */

typedef int (*bcmcth_vlan_xlate_port_grp_set_f)(int unit);

/*!
 * \brief Set the hardware for VLAN_ING_STG_PROFILEt.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] lt_id Logical table ID.
 * \param [in] lt_info LT Buffer.
 *
 * \retval SHR_E_NONE No errors.
 */

typedef int (*bcmcth_vlan_ing_stg_profile_set_f)(int unit,
                                                 uint32_t trans_id,
                                                 bcmltd_sid_t lt_id,
                                                 vlan_ing_stg_profile_t *lt_info);

/*!
 * \brief Set the hardware for VLAN_ING_EGR_STG_MEMBER_PROFILEt.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] lt_id Logical table ID.
 * \param [in] lt_info LT Buffer.
 *
 * \retval SHR_E_NONE No errors.
 */

typedef int (*bcmcth_vlan_ing_egr_stg_member_profile_set_f)(int unit,
                                                            uint32_t trans_id,
                                                            bcmltd_sid_t lt_id,
                                                            vlan_ing_egr_stg_member_profile_t *lt_info);

/*!
 * \brief Set the hardware for VLAN_EGR_STG_PROFILEt.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] lt_id Logical table ID.
 * \param [in] lt_info LT Buffer.
 *
 * \retval SHR_E_NONE No errors.
 */

typedef int (*bcmcth_vlan_egr_stg_profile_set_f)(int unit,
                                                 uint32_t trans_id,
                                                 bcmltd_sid_t lt_id,
                                                 vlan_egr_stg_profile_t *lt_info);

/*!
 * \brief VLAN driver object
 *
 * VLAN driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH VLAN module by
 * \ref bcmcth_vlan_drv_init from the top layer. BCMCTH VLAN internally
 * will use this interface to get the corresponding information.
 */
typedef struct bcmcth_vlan_drv_s {
    /*! Set the hardware for VLAN_XLATE_CONFIGt_EGR_XLATE_PORT_GRPf driver. */
    bcmcth_vlan_xlate_port_grp_set_f xlate_port_grp_set;

    /*! Set the hardware for VLAN_ING_STG_PROFILEt driver. */
    bcmcth_vlan_ing_stg_profile_set_f ing_stg_profile_set;

    /*! Set the hardware for VLAN_ING_STG_FLOOD_BLOCK_PROFILEt driver. */
    bcmcth_vlan_ing_egr_stg_member_profile_set_f ing_egr_stg_member_profile_set;

    /*! Set the hardware for VLAN_EGR_STG_PROFILEt driver. */
    bcmcth_vlan_egr_stg_profile_set_f egr_stg_profile_set;

} bcmcth_vlan_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
    extern bcmcth_vlan_drv_t *_bc##_cth_vlan_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Install the device-specific VLAN driver.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_vlan_drv_init(int unit);

/*!
 * \brief Configure Vlan Xlate use Port group or modport as key
 *
 * \param [in] unit Logical device id
 *
 * \retval            - SHR_E_NONE Success
 * \retval Else       - SHR_E_UNAVAIL Not supported
 */
extern int
bcmcth_vlan_xlate_port_grp_set(int unit);

/*!
 * \brief Initialize IMM callbacks.
 *
 * The VLAN custom table handling is done via the IMM component,
 * which needs a callback interface to commit changes to hardware.
 *
 * \param [in] unit
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_vlan_imm_init(int unit);

/*!
 * \brief Set the hardware for VLAN_ING_STG_PROFILEt.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] lt_id Logical table ID.
 * \param [in] lt_info LT Buffer.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_vlan_ing_stg_profile_set(int unit,
                                uint32_t trans_id,
                                bcmltd_sid_t lt_id,
                                vlan_ing_stg_profile_t *lt_info);

/*!
 * \brief Set the hardware for VLAN_ING_EGR_STG_MEMBER_PROFILEt.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] lt_id Logical table ID.
 * \param [in] lt_info LT Buffer.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_vlan_ing_egr_stg_member_profile_set(int unit,
                                           uint32_t trans_id,
                                           bcmltd_sid_t lt_id,
                                           vlan_ing_egr_stg_member_profile_t *lt_info);
/*!
 * \brief Set the hardware for VLAN_EGR_STG_PROFILEt.
 *
 * \param [in] unit Unit number.
 * \param [in] trans_id Transaction ID.
 * \param [in] lt_id Logical table ID.
 * \param [in] lt_info LT Buffer.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_vlan_egr_stg_profile_set(int unit,
                                uint32_t trans_id,
                                bcmltd_sid_t lt_id,
                                vlan_egr_stg_profile_t *lt_info);

#endif /* BCMCTH_VLAN_DRV_H */
