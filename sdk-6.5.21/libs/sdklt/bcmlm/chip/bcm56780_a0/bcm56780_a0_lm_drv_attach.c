/*! \file bcm56780_a0_lm_drv_attach.c
 *
 * Linkscan chip-specific driver for BCM56780.
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

/*******************************************************************************
 * Local data
 */

/* Chip specific driver. */
static bcmlm_drv_t bcm56780_a0_lm_drv;

/* Exceptions of linkscan register bits mapping. */
static bcmlm_fw_pport_to_lsbit_t bcm56780_a0_pport_to_lsbit[] = {
    /* pport   lsbit */
    {  162,    163 } /* Management port (xe1). */
};

static int
bcm56780_a0_fault_check_enabled(int unit, int *enabled)
{
    *enabled = TRUE;

    return SHR_E_NONE;
}

static int
bcm56780_a0_lm_port_validate(int unit, shr_port_t port)
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

static int
bcm56780_a0_fw_linkscan_config(int unit, bcmdrd_pbmp_t pbm)
{
    int i;
    bcmlm_fw_pport_to_lsbit_t *map;

    /* Normally, the link bit position for physical port is at (pport - 1).
     * But there are some exceptions required to be handled.
     */
    map = bcm56780_a0_pport_to_lsbit;
    for (i = 0; i < COUNTOF(bcm56780_a0_pport_to_lsbit); i++) {
        if (BCMDRD_PBMP_MEMBER(pbm, map[i].pport)) {
            BCMDRD_PBMP_PORT_REMOVE(pbm, map[i].pport);
            BCMDRD_PBMP_PORT_ADD(pbm, map[i].lsbit);
        }
    }

    return bcm56780_a0_lm_drv.hw_config(unit, pbm);
}

static int
bcm56780_a0_fw_linkscan_link_get(int unit, bcmdrd_pbmp_t *pbm)
{
    int i;
    bcmlm_fw_pport_to_lsbit_t *map;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcm56780_a0_lm_drv.hw_link_get(unit, pbm));

    /* Normally, the link bit position for physical port is at (pport - 1).
     * But there are some exceptions required to be handled.
     */
    map = bcm56780_a0_pport_to_lsbit;
    for (i = 0; i < COUNTOF(bcm56780_a0_pport_to_lsbit); i++) {
        if (BCMDRD_PBMP_MEMBER(*pbm, map[i].lsbit)) {
            BCMDRD_PBMP_PORT_REMOVE(*pbm, map[i].lsbit);
            BCMDRD_PBMP_PORT_ADD(*pbm, map[i].pport);
        }
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
bcm56780_a0_lm_drv_attach(bcmlm_drv_t *drv)
{
    SHR_FUNC_ENTER(drv->unit);

    SHR_IF_ERR_EXIT
        (bcmlm_cmicx_fw_linkscan_drv_attach(drv->unit, drv));

    sal_memcpy(&bcm56780_a0_lm_drv, drv, sizeof(bcmlm_drv_t));

    /* Install chip specific callback. */
    drv->hw_config = bcm56780_a0_fw_linkscan_config;
    drv->hw_link_get = bcm56780_a0_fw_linkscan_link_get;
    drv->fault_check_enabled = bcm56780_a0_fault_check_enabled;
    drv->port_validate = bcm56780_a0_lm_port_validate;

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
bcm56780_a0_lm_drv_detach(bcmlm_drv_t *drv)
{
    return bcmlm_cmicx_fw_linkscan_drv_detach(drv->unit, drv);
}
