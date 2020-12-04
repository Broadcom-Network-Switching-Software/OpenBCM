/*! \file bcmcth_port_attach.c
 *
 * BCMCTH PORT driver attach/detach
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
#include <bcmcth/bcmcth_port_drv.h>
#include <bcmcth/bcmcth_port.h>
#include <bcmcth/bcmcth_port_system.h>

#define BSL_LOG_MODULE BSL_LS_BCMCTH_PORT

/*******************************************************************************
 * Public Functions
 */
int
bcmcth_port_attach(int unit, bool warm)
{
    bcmdrd_dev_type_t dev_type;
    int                 rv = SHR_E_NONE;
    const bcmlrd_map_t  *map = NULL;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_ERR_EXIT(SHR_E_UNIT);
    }

    /* Initialize device-specific driver */
    SHR_IF_ERR_EXIT
        (bcmcth_port_drv_init(unit, warm));

    /* Initialize the BCMPORT component. */
    SHR_IF_ERR_EXIT
        (bcmcth_port_init(unit, warm));

    rv = bcmlrd_map_get(unit, PORT_SYSTEMt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(bcmcth_port_system_sw_state_init(unit, warm));
    }

    /* Initialize IMM drivers */
    SHR_IF_ERR_EXIT
        (bcmcth_port_imm_init(unit));
exit:
    SHR_FUNC_EXIT();
}

int
bcmcth_port_detach(int unit)
{
    int                 rv = SHR_E_NONE;
    const bcmlrd_map_t  *map = NULL;

    SHR_FUNC_ENTER(unit);
    rv = bcmlrd_map_get(unit, PORT_SYSTEMt, &map);
    if (SHR_SUCCESS(rv) && map) {
        SHR_IF_ERR_EXIT(bcmcth_port_system_sw_state_cleanup(unit));
    }
    /* De-initialize the BCMPORT component. */
    SHR_IF_ERR_EXIT
        (bcmcth_port_deinit(unit));
    bcmcth_port_imm_db_cleanup(unit);
exit:
    SHR_FUNC_EXIT();
}
