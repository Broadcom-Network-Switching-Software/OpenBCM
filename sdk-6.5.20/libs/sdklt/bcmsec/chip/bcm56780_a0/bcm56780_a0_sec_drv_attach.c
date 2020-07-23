/*! \file bcm56780_a0_sec_drv_attach.c
 *
 * Traffic manager chip driver
 *
 * The library functions definitions for bcm56780_a0 device.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_feature.h>
#include <bcmbd/bcmbd_cmicd_mem.h>
#include <bcmbd/chip/bcm56780_a0_acc.h>
#include <bcmsec/bcmsec_types.h>
#include <bcmsec/generated/bcmsec_ha.h>
#include <bcmsec/bcmsec_drv.h>
#include <bcmsec/bcmsec_sysm.h>
#include <bcmsec/bcmsec_utils.h>
#include <bcmsec/chip/bcm56780_a0_sec.h>
#include <bcmsec/chip/bcm56780_a0_sec_port.h>
#include <bcmsec/chip/bcm56780_a0_sec_sa_policy.h>
#include <bcmsec/bcmsec_utils_internal.h>
#include "bcm56780_a0_sec_mgmt.h"

#define BSL_LOG_MODULE  BSL_LS_BCMSEC_INIT

/*!
 * \brief BCMSEC device clean up for TD4.
 *
 * \param [in] unit Logical unit number.
 * \param [in] warm Warm boot.
 *
 * \retval SHR_E_NONE No error.
 * \retval !SHR_E_NONE Error code.
 */
static int
bcm56780_a0_bcmsec_dev_cleanup(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);
    SHR_FUNC_EXIT();
}

/*!
 * \brief BCMSEC device stop for TD4.
 *
 * \param [in] unit Logical unit number.
 * \param [in] warm Warm boot.
 *
 * \return SHR_E_NONE No error.
 * \return !SHR_E_NONE Error code.
 */
static int
bcm56780_a0_bcmsec_dev_stop(int unit, bool warm)
{
    SHR_FUNC_ENTER(unit);

    SHR_FUNC_EXIT();
}

/*!
 * \brief Is SEC valid on PM
 *
 * \param [in] unit Logical unit number.
 * \param [in] pm_id Port Macro
 *
 * \return Return 1 if PM is SEC enabled
 */
static int
bcm56780_a0_is_pm_sec_enabled(int unit, int pm_id)
{
    int rv;
    bcmsec_pm_info_t *pm_info;
    bcmsec_dev_info_t *dev_info;

    if ((pm_id < 1) || (pm_id > 20)) {
        return 0;
    }

    if (pm_id) {
        pm_id--;
    }
    rv = bcmsec_dev_info_get(unit, &dev_info);
    if (rv == SHR_E_NONE) {
        pm_info = &(dev_info->pm_map_info[pm_id]);
        if (pm_info && pm_info->enable) {
            return 1;
        }
    }
    return 0;
}

/*!
 * \brief BCMSEC get SEC port number for a physical port.
 *
 * \param [in] unit Logical unit number.
 *
 * \return Return SHR_E_NONE if valid. Else return SHR_E_PARAM.
 */
static int
bcm56780_a0_bcmsec_sec_port_get(int unit, int pport, int *sec_port)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcm56780_a0_sec_pport_to_secport(unit, (bcmsec_pport_t)pport, NULL,
                                          sec_port, NULL));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief BCMSEC enable check.
 *
 * \param [in] unit Logical unit number.
 *
 * \return Return 1 if sec enabled
 */
static int
bcm56780_a0_bcmsec_sec_valid_get(int unit)
{
    if (bcmdrd_dev_flags_get(unit) & CHIP_FLAG_MACSEC) {
        return 1;
    }
    return 0;
}

/*!
 * \brief BCMSEC BCMPC handler for TD4.
 *
 * \param [in] unit Logical unit number.
 *
 * \return Return the handler function for BCMSEC and BCMPC interaction.
 */
static bcmpc_sec_handler_t *
bcm56780_a0_bcmsec_pc_hdl_get(int unit)
{
    static bcmpc_sec_handler_t hdl;

    hdl.sec_configure = bcm56780_a0_sec_pc_configure;
    hdl.sec_validate = bcm56780_a0_sec_validate;
    hdl.port_op_exec = bcmsec_port_op_exec;
    hdl.sec_valid_get = bcm56780_a0_bcmsec_sec_valid_get;
    hdl.is_pm_sec_enabled = bcm56780_a0_is_pm_sec_enabled;
    hdl.sec_port_get = bcm56780_a0_bcmsec_sec_port_get;

    return &hdl;
}

/*!
 * \brief Verify if blktype is SEC.
 *
 * \param [in] unit Logical unit number.
 * \param [in] blktype Blocktype
 *
 * \return Return TRUE if blktype is SEC,
 *                FALSE otherwise.
 */
static bool
bcm56780_a0_bcmsec_blktype_is_sec(int unit, int blktype)
{
    return (blktype == BLKTYPE_MACSEC);
}

static
bcmsec_drv_t bcm56780_a0_sec_drv = {
    .dev_init = bcm56780_a0_sec_chip_init,
    .dev_stop = bcm56780_a0_bcmsec_dev_stop,
    .dev_cleanup = bcm56780_a0_bcmsec_dev_cleanup,
    .imm_init = bcm56780_a0_sec_imm_init,
    .sec_pc_hdl_get = bcm56780_a0_bcmsec_pc_hdl_get,
    .port_ing_down = bcm56780_a0_sec_port_rx_down,
    .port_egr_down = bcm56780_a0_sec_port_tx_down,
    .port_egr_up = bcm56780_a0_sec_port_tx_up,
    .pm_enable = bcm56780_a0_sec_pm_enable,
    .decrypt_mgmt_set = bcm56780_a0_sec_decrypt_mgmt,
    .port_info_get = bcm56780_a0_sec_port_info_get,
    .port_ctrl_set = bcm56780_a0_sec_egr_port_control_set,
    .decrypt_port_ctrl_set = bcm56780_a0_decrypt_port_control_set,
    .sa_policy_set = bcm56780_a0_sa_policy_set,
    .sa_policy_get = bcm56780_a0_sa_policy_get,
    .sa_expire_handler = bcm56780_a0_sec_sa_expire_handler,
    .blktype_is_sec = bcm56780_a0_bcmsec_blktype_is_sec,
    .is_sec_supported = bcm56780_a0_bcmsec_sec_valid_get,
};

/*******************************************************************************
* Public functions
 */
int
bcmsec_bcm56780_a0_drv_attach(int unit)
{
    SHR_FUNC_ENTER(unit);
    SHR_IF_ERR_EXIT
        (bcmsec_drv_set(unit, &bcm56780_a0_sec_drv));
exit:
    SHR_FUNC_EXIT();
}
