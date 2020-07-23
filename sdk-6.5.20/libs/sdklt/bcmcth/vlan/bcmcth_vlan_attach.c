/*! \file bcmcth_vlan_attach.c
 *
 * BCMCTH VLAN driver attach/detach
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmcth/bcmcth_vlan_drv.h>
#include <bcmcth/bcmcth_vlan.h>

#define BSL_LOG_MODULE BSL_LS_BCMCTH_VLAN

/*******************************************************************************
 * Public Functions
 */
int
bcmcth_vlan_attach(int unit, bool warm)
{
    bcmdrd_dev_type_t dev_type;
    const bcmlrd_map_t *map = NULL;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    /* Initialize device-specific driver */
    SHR_IF_ERR_EXIT
        (bcmcth_vlan_drv_init(unit));

    if (SHR_SUCCESS(bcmlrd_map_get(unit, VLAN_XLATE_CONFIGt, &map))) {
        /* Set the hardware according to VLAN_XLATE_CONFIGt */
        SHR_IF_ERR_EXIT
            (bcmcth_vlan_xlate_port_grp_set(unit));
    }

    /* Initialize IMM drivers */
    SHR_IF_ERR_EXIT
        (bcmcth_vlan_imm_init(unit));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_vlan_detach(int unit)
{
    SHR_FUNC_ENTER(unit);
    SHR_FUNC_EXIT();
}
