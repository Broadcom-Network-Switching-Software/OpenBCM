/*! \file bcmcth_tnl_attach.c
 *
 * Tunnel component interface to system manager.
 * This file implements the Tunnel interface to system manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <sal/sal_alloc.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <shr/shr_sysm.h>
#include <bsl/bsl.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmdrd_config.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcth/bcmcth_tnl_l3_intf.h>
#include <bcmcth/bcmcth_tnl_mpls_drv.h>
#include <bcmcth/bcmcth_tnl_drv.h>
#include <bcmcth/bcmcth_tnl_sysm.h>
#include <bcmcth/bcmcth_tnl.h>
#include <bcmcth/bcmcth_tnl_mpls_prot.h>

/*******************************************************************************
 * Defines
 */

#define BSL_LOG_MODULE  BSL_LS_BCMCTH_TNL

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Initialize device driver.
 *
 * Initialize device features and install base driver functions.
 *
 * \param [in] unit Unit number.
 */
static int
bcmcth_tnl_drv_attach (int unit)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    /* Initialize device-specific driver. */
    SHR_IF_ERR_EXIT
        (bcmcth_tnl_encap_drv_init(unit));

    SHR_IF_ERR_EXIT
        (bcmcth_tnl_decap_drv_init(unit));

    SHR_IF_ERR_EXIT
        (bcmcth_tnl_mpls_drv_init(unit));

    SHR_IF_ERR_EXIT
        (bcmcth_tnl_mpls_prot_drv_init(unit));
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Clean up device driver.
 *
 * Release any resources allocated during attach.
 *
 * \param [in] unit Unit number.
 */
static int
bcmcth_tnl_drv_detach (int unit)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

/*!
 * \brief Initialize tunnel handler function software data structures.
 *
 * \param [in] unit Unit number.
 * \param [in] cold Cold/Warmboot flag.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_tnl_init(int unit, bool warm)
{
    int                 rv = SHR_E_NONE;
    const bcmlrd_map_t  *map = NULL;
    bcmcth_tnl_encap_drv_t  *drv = NULL;

    SHR_FUNC_ENTER(unit);

    if (bcmcth_tnl_drv_attach(unit) != SHR_E_NONE) {
        return SHR_SYSM_RV_ERROR;
    }

    rv = bcmcth_tnl_encap_drv_get(unit, &drv);
    if (SHR_SUCCESS(rv) && (drv != NULL)) {
        SHR_IF_ERR_EXIT(bcmcth_tnl_sw_state_init(unit, warm));
    }

    rv = bcmlrd_map_get(unit, L3_EIFt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(bcmcth_tnl_l3_intf_sw_state_init(unit, warm));
    }

    (void) bcmcth_tnl_imm_register(unit);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Cleanup tunnel handler function software data structures.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_tnl_cleanup(int unit)
{
    int                 rv = SHR_E_NONE;
    const bcmlrd_map_t  *map = NULL;
    bcmcth_tnl_encap_drv_t  *drv = NULL;

    SHR_FUNC_ENTER(unit);

    rv = bcmcth_tnl_encap_drv_get(unit, &drv);
    if (SHR_SUCCESS(rv) && (drv != NULL)) {
        SHR_IF_ERR_EXIT(bcmcth_tnl_sw_state_cleanup(unit));
    }

    rv = bcmlrd_map_get(unit, L3_EIFt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(bcmcth_tnl_l3_intf_sw_state_cleanup(unit));
    }

    if (bcmcth_tnl_drv_detach(unit) != SHR_E_NONE) {
        return SHR_SYSM_RV_ERROR;
    }

exit:
    SHR_FUNC_EXIT();
}
