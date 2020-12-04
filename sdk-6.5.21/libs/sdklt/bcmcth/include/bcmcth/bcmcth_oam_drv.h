/*! \file bcmcth_oam_drv.h
 *
 * BCMCTH OAM driver object.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMCTH_OAM_DRV_H
#define BCMCTH_OAM_DRV_H

#include <sal/sal_types.h>
#include <shr/shr_bitop.h>
#include <bcmbd/mcs_shared/mcs_bfd_common.h>
/*!
 * \brief Initialize the OAM.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_oam_init_f)(int unit, bool warm);

/*!
 * \brief De-initialize the OAM.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
typedef int (*bcmcth_oam_deinit_f)(int unit);

/*!
 * \brief Get the flex fields that need to be extracted from Rx PMD.
 *
 * Get the flex fields in the Rx PMD that is required by the EApp.
 *
 * \param [in] unit Unit number.
 * \param [in] max_fids Indicates the size of the \c fids array
 * \param [out] num_fids Number of fields that need to be extracted from
 *                         Rx PMD.
 * \param [out] fids Array of flex field ids.
*/
typedef int (*bcmcth_oam_bfd_rxpmd_flex_fids_get_f) (int unit, int max_fids,
                                                     int *num_fids, int *fids);

/*!
 * \brief Get the string corresponding to the match id.
 *
 * Get the string corresponding to the match id that is required by the EApp.
 *
 * \param [in] unit Unit number.
 * \param [in] match_id Match id required by EApp
 *
 * \param [out] str Match id string.
 */
typedef const char* (*bcmcth_oam_bfd_match_id_string_get_f) (int unit,
    mcs_bfd_match_id_fields_t match_id);

/*!
 * \brief BFD driver object.
 *
 * Device specific BFD functions are implemented using this driver.
 */
typedef struct bcmcth_oam_bfd_drv_s {
    /*! Maximum number of MPLS labels. */
    uint8_t max_mpls_labels;

    /*! Get number of vlans from VLAN tag type. */
    int (*num_vlans_get) (uint8_t vlan_tag_type, uint8_t *num_vlans);

    /*! Get the Rx PMD flex field ids. */
    bcmcth_oam_bfd_rxpmd_flex_fids_get_f rxpmd_flex_fids_get;

    /*! Get the string corresponding to the match id. */
    bcmcth_oam_bfd_match_id_string_get_f match_id_string_get;

    /*! Maximum number of BFD endpoints. */
    uint32_t max_endpoints;

    /*! Maximum number of BFD Simple Password authentication keys. */
    uint32_t max_auth_sp_keys;

    /*! Maximum number of BFD SHA1 authentication keys. */
    uint32_t max_auth_sha1_keys;
} bcmcth_oam_bfd_drv_t;

/*!
 * \brief OAM driver object
 *
 * OAM driver is used to provide the chip specific information.
 *
 * Each device should provide its own instance to BCMCTH OAM module by
 * \ref bcmcth_oam_drv_init() from the top layer. BCMCTH OAM internally
 * will use this interface to get the corresponding information.
 */
typedef struct bcmcth_oam_drv_s {
    /*! Init the OAM feature. */
    bcmcth_oam_init_f oam_init;

    /*! De-init the OAM feature. */
    bcmcth_oam_deinit_f oam_deinit;

    /*! BFD driver functions. */
    bcmcth_oam_bfd_drv_t *bfd_drv;
} bcmcth_oam_drv_t;

/*! \cond  Externs for the required functions. */
#define BCMDRD_DEVLIST_ENTRY(_nm,_vn,_dv,_rv,_md,_pi,_bd,_bc,_fn,_cn,_pf,_pd,_r0,_r1) \
extern bcmcth_oam_drv_t *_bc##_cth_oam_drv_get(int unit);
#define BCMDRD_DEVLIST_OVERRIDE
#include <bcmdrd/bcmdrd_devlist.h>
/*! \endcond */

/*!
 * \brief Get the OAM driver of the device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern bcmcth_oam_drv_t *
bcmcth_oam_drv_get(int unit);

/*!
 * \brief Initialize the OAM driver of the device.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_oam_drv_init(int unit);

/*!
 * \brief Initialize the OAM event.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warmboot indicator.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_oam_init(int unit, bool warm);

/*!
 * \brief De-initialize the OAM event.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmcth_oam_deinit(int unit);

/*!
 * \brief Get the BFD device specific APIs and parameters.
 *
 *
 * \param [in] unit Unit number.
 *
 * \return BFD driver API structure
 */
bcmcth_oam_bfd_drv_t *
bcmcth_oam_bfd_drv_get(int unit);

#endif /* BCMCTH_OAM_DRV_H */
