/*! \file bcmcth_port_sysm.c
 *
 * Interface functions to the system manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <sal/sal_assert.h>
#include <sal/sal_libc.h>
#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_sysm.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmcth/bcmcth_port_sysm.h>
#include <bcmcth/bcmcth_port.h>

/*******************************************************************************
 * Defines
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_PORT

/*******************************************************************************
 * Typedefs
 */


/*******************************************************************************
 * Private variables
 */

/*******************************************************************************
 * Private Functions
 */
/* Phase: comp_config */
static shr_sysm_rv_t
cth_port_sysm_comp_config(shr_sysm_categories_t inst_cat,
                          int unit,
                          void *comp_data,
                          bool warm,
                          uint32_t *blocking_component)
{
    int idx;
    uint32_t comp_dep[] = {
        BCMMGMT_CFG_COMP_ID,
        BCMMGMT_IMM_FE_COMP_ID,
        BCMMGMT_BD_COMP_ID,
        BCMMGMT_EVM_COMP_ID,
        BCMMGMT_PTM_COMP_ID,
        BCMMGMT_DI_COMP_ID,
        BCMMGMT_SER_COMP_ID,
        BCMMGMT_DRD_COMP_ID
    };

    for (idx = 0; idx < COUNTOF(comp_dep); idx++) {
        if (!shr_sysm_is_comp_complete(comp_dep[idx], inst_cat, unit)) {
            *blocking_component = comp_dep[idx];
            return SHR_SYSM_RV_BLOCKED;
        }
    }

    if (SHR_FAILURE(bcmcth_port_attach(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/* Phase: shutdown */
static shr_sysm_rv_t
cth_port_sysm_shutdown(shr_sysm_categories_t inst_cat,
                       int unit,
                       void *comp_data,
                       bool graceful,
                       uint32_t *blocking_component)
{
    if (SHR_FAILURE(bcmcth_port_detach(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMCTH PORT System manager init struct.
 */
static shr_sysm_cb_func_set_t cth_port_sysm_handler = {

    /*! Component configuration. */
    .comp_config = cth_port_sysm_comp_config,

    /*! Shutdown. */
    .shutdown    = cth_port_sysm_shutdown
};
/*******************************************************************************
 * Public Functions
 */
int
bcmcth_port_start(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_PORT_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &cth_port_sysm_handler,
                           NULL));
exit:
    SHR_FUNC_EXIT();
}
