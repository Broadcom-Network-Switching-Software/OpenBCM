/*! \file bcmsec_drv.h
 *
 * BCMSEC Driver Object
 *
 * Declaration of the public structures, enumerations, and functions
 * which interface into the security (SEC) component.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifndef BCMSEC_DRV_H
#define BCMSEC_DRV_H

#include <sal/sal_types.h>
#include <bcmltd/bcmltd_types.h>
#include <bcmltd/bcmltd_handler.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmpc/bcmpc_sec.h>
#include <bcmsec/bcmsec_types.h>
#include <bcmsec/bcmsec_port_info.h>
#include <bcmsec/bcmsec_pt_internal.h>
#include <bcmsec/imm/bcmsec_imm_defines_internal.h>

/*!
 * \name SEC driver functions
 *
 * The function prototypes for \ref bcmsec_drv_t.
 */

typedef bcmdrd_sid_t
(*bcmsec_drv_pt_sid_uniq_acc_f)(int unit,
                              bcmdrd_sid_t in_sid,
                              int xpe,
                              int pipe);


typedef int
(*bcmsec_drv_chip_func_f)(int unit, bool warm);


typedef int
(*bcmsec_drv_port_op_f)(int unit, bcmsec_pport_t port);

/*!
 * Function pointer for imm initialization.
 */
typedef int
(*bcmsec_drv_imm_init_f)(int unit);

/*!
 * PM chip set
 */
typedef int
(*bcmsec_drv_pm_enable_f)(int unit, bcmltd_sid_t ltid,
                       uint32_t pm_id, uint32_t enable,
                       uint8_t *opcode);

/*!
 * Decrypt management chip level configuration.
 */
typedef int
(*bcmsec_drv_decrypt_mgmt_f)(int unit, bcmltd_sid_t ltid,
                       bcmsec_decrypt_mgmt_cfg_t *decrypt_mgmt_cfg);

/*!
 * Get port info.
 */
typedef int
(*bcmsec_port_info_get_f)(int unit, int pport,
                          sec_pt_port_info_t *info);

/*!
 * Set port info.
 */
typedef int
(*bcmsec_port_ctrl_set_f)(int unit, bcmltd_sid_t ltid,
                          bcmsec_encrypt_port_control_t *port_control,
                          uint32_t *opcode);
/*!
 * Decrypt port ctrl configuration.
 */
typedef int
(*bcmsec_decrypt_port_ctrl_set_f)(int unit, bcmltd_sid_t ltid,
                       int pport, bcmltd_field_t *in, uint32_t *opcode);

/*!
 * SA policy config set
 */
typedef int
(*bcmsec_sa_policy_set_f)(int unit, bcmltd_sid_t ltid,
                       bcmsec_pt_info_t *pt_dyn_info, bcmltd_field_t *in,
                       int encrypt);

/*!
 * SA policy config get
 */
typedef int
(*bcmsec_sa_policy_get_f)(int unit, bcmltd_sid_t ltid,
                       bcmsec_pt_info_t *pt_dyn_info,
                       sa_policy_info_t *sa_policy,
                       int encrypt);

/*!
 * SA Expiry handler
 */
typedef int
(*bcmsec_sa_expire_handler_f)(int unit, int blk_id, int event_num);

/*!
 * Check blktype is SEC
 */
typedef bool
(*bcmsec_blktype_is_sec_f)(int unit, int blktype);

/*!
 * Check if SEC is supported on device.
 */
typedef int
(*bcmsec_is_sec_supported_f)(int unit);

/*!
 * \brief Chip driver get for the device.
 *
 * \param [in] unit Logical unit number.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
typedef int (*bcmsec_drv_get_f) (int unit, void *drv);

/*!
 * \brief PC handler
 *
 * \param [in] unit Logical unit number.
 *
 * \retval bcmpc_sec_handler_t*
 */
typedef bcmpc_sec_handler_t* (*bcmsec_pc_hdl_get_f) (int unit);

/*******************************************************************************
* SEC driver object
 */

/*!
 * \brief SEC driver object
 *
 * SEC driver is used to provide the chip specific information.
 *
 * The chip specific information are included,
 * 1. The port block id and PHY address.
 * 2. The driver and access bus of PHY/PMAC.
 * 3. The programming sequence for complex port configuration e.g. port up/down.
 * 4. The linkscan mode.
 *
 * Each device should provide its own instance to BCMSEC module by \ref
 * bcmsec_drv_set() from the top layer. BCMSEC internally will use this interface
 * to get the corresponding information.
 */
typedef struct bcmsec_drv_s {
    /*! Device init. */
    bcmsec_drv_chip_func_f      dev_init;

    /*! Device stop. */
    bcmsec_drv_chip_func_f      dev_stop;

    /*! Device cleanup. */
    bcmsec_drv_chip_func_f      dev_cleanup;

    /*! Ingress port down. */
    bcmsec_drv_port_op_f        port_ing_down;

    /*! Egress port down. */
    bcmsec_drv_port_op_f        port_egr_down;

    /*! Egress port up. */
    bcmsec_drv_port_op_f        port_egr_up;

    /*! IMM initialization. */
    bcmsec_drv_imm_init_f       imm_init;

    /*! Port Macro enable sequence. */
    bcmsec_drv_pm_enable_f      pm_enable;

    /*! PC SEC handler get. */
    bcmsec_pc_hdl_get_f         sec_pc_hdl_get;

    /*! Decrypt management set. */
    bcmsec_drv_decrypt_mgmt_f   decrypt_mgmt_set;

    /*! SEC Port information get. */
    bcmsec_port_info_get_f      port_info_get;

    /*! SEC port control set. */
    bcmsec_port_ctrl_set_f      port_ctrl_set;

    /*! Decrypt port control set. */
    bcmsec_decrypt_port_ctrl_set_f
                                decrypt_port_ctrl_set;

    /*! SA policy set. */
    bcmsec_sa_policy_set_f      sa_policy_set;

    /*! SA policy set. */
    bcmsec_sa_policy_get_f      sa_policy_get;

    /*! SA expired update. */
    bcmsec_sa_expire_handler_f  sa_expire_handler;

    /*! Check if blktype is BLKTYPE_MACSEC. */
    bcmsec_blktype_is_sec_f     blktype_is_sec;

    /*! Check if SEC is supported on device. */
    bcmsec_is_sec_supported_f   is_sec_supported;
} bcmsec_drv_t;

/*!
 * \brief Set the SEC driver of the device.
 *
 * \param [in] unit Unit number.
 * \param [in] drv SEC driver to set.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmsec_drv_set(int unit, bcmsec_drv_t *drv);

/*!
 * \brief Get the SEC driver of the device.
 *
 * \param [in] unit Unit number.
 * \param [in] drv SEC driver to get.
 *
 * \retval SHR_E_NONE No errors.
 */
extern int
bcmsec_drv_get(int unit, bcmsec_drv_t **drv);

/*!
 * \brief Allocates HA memory for device specific driver info.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warm boot.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Error in allocating driver info.
 */
extern int
bcmsec_dev_info_alloc(int unit, bool warm);

/*!
 * \brief Allocates HA memory for device specific driver info.
 *
 * \param [in] unit Unit number.
 * \param [in] warm Warm boot.
 * \param [out] port_cfg Port configuration.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_PARAM Error in allocating driver info.
 */
extern int
bcmsec_port_info_alloc(int unit, bool warm,
                       sec_port_info_alloc_cfg_t *port_cfg);

/*!
 * \brief SEC modules regestering for IMM.
 *
 * \param [in] unit   Unit Number.
 */
extern int
bcmsec_imm_reg(int unit);

/*!
 * \brief Determine if the blktype is SEC.
 *
 * \param [in] unit   Unit Number.
 * \param [in] blktype Block type.
 *
 * \return Return TRUE if blktype is SEC,
 *                FALSE otherwise.
 */
extern int
bcmsec_is_blktype_sec(int unit, int blktype);

/*!
 * \brief Determine if SEC is supported on the device.
 *
 * \param [in] unit   Unit Number.
 *
 * \return Return TRUE if SEC is supported,
 *                FALSE otherwise.
 */
extern int
bcmsec_is_sec_supported(int unit);

#endif /* BCMSEC_DRV_H */
