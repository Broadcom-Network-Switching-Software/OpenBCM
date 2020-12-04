/*! \file bcm56880_a0_lm_drv_attach.c
 *
 * Linkscan chip-specific driver for BCM56880.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>

#include <bcmdrd/bcmdrd_port.h>
#include <bcmlm/bcmlm_drv_internal.h>
#include <bcmlm/bcmlm_fw_linkscan.h>
#include <bcmlm/bcmlm_chip_internal.h>

/* Debug log target definition */
#define BSL_LOG_MODULE  BSL_LS_BCMLM_DRV

static int
bcm56880_a0_fault_check_enabled(int unit, int *enabled)
{
    *enabled = TRUE;

    return SHR_E_NONE;
}

static int
bcm56880_a0_lm_port_validate(int unit, shr_port_t port)
{
    uint32_t port_type = bcmdrd_lport_type_get(unit, port);

    SHR_FUNC_ENTER(unit);

    /* Link scan on front ports and management ports. */
    if (port_type & ~(BCMDRD_PORT_TYPE_FPAN | BCMDRD_PORT_TYPE_MGMT)) {
        SHR_ERR_EXIT(SHR_E_PORT);
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

/*!
 * \brief Allocate resources and attach specific operations.
 *
 * \param [in] drv Pointer to driver structure.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm56880_a0_lm_drv_attach(bcmlm_drv_t *drv)
{
    SHR_FUNC_ENTER(drv->unit);

    SHR_IF_ERR_EXIT
        (bcmlm_cmicx_fw_linkscan_drv_attach(drv->unit, drv));

    drv->fault_check_enabled = bcm56880_a0_fault_check_enabled;
    drv->port_validate = bcm56880_a0_lm_port_validate;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Release resources.
 *
 * \param [in] drv Pointer to driver structure.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcm56880_a0_lm_drv_detach(bcmlm_drv_t *drv)
{
    return bcmlm_cmicx_fw_linkscan_drv_detach(drv->unit, drv);
}
