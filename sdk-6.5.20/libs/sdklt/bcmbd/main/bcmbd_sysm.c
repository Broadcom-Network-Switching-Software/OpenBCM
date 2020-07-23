/*! \file bcmbd_sysm.c
 *
 * BCMBD interface to system manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_debug.h>
#include <shr/shr_sysm.h>
#include <bcmmgmt/bcmmgmt_sysm.h>

#include <bcmbd/bcmbd.h>
#include <bcmbd/bcmbd_sysm.h>

/*******************************************************************************
 * Defines
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMBD_INIT


/*******************************************************************************
 * SYSM callback functions
 */

static shr_sysm_rv_t
bd_sysm_init(shr_sysm_categories_t inst_cat, int unit, void *comp_data,
             bool warm, uint32_t *blocking_comp)
{
    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t
bd_sysm_comp_config(shr_sysm_categories_t inst_cat, int unit, void *comp_data,
                    bool warm, uint32_t *blocking_comp)
{
    int idx;
    uint32_t comp_dep[] = {
        BCMMGMT_CFG_COMP_ID
    };

    for (idx = 0; idx < COUNTOF(comp_dep); idx++) {
        if (!shr_sysm_is_comp_complete(comp_dep[idx], inst_cat, unit)) {
            *blocking_comp = comp_dep[idx];
            return SHR_SYSM_RV_BLOCKED;
        }
    }

    /* Attach base driver */
    if (!bcmbd_attached(unit)) {
        if (SHR_FAILURE(bcmbd_attach(unit))) {
            return SHR_SYSM_RV_ERROR;
        }
    }

    /* Reset and initialize if not warm-boot */
    if (!warm) {
        if (SHR_FAILURE(bcmbd_dev_reset(unit))) {
            return SHR_SYSM_RV_ERROR;
        }

        if (SHR_FAILURE(bcmbd_dev_init(unit))) {
            return SHR_SYSM_RV_ERROR;
        }
    }

    if (SHR_FAILURE(bcmbd_dev_start(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t
bd_sysm_pre_config(shr_sysm_categories_t inst_cat, int unit, void *comp_data,
                   bool warm, uint32_t *blocking_comp)
{
    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t
bd_sysm_stop(shr_sysm_categories_t inst_cat, int unit, void *comp_data,
             bool graceful, uint32_t *blocking_comp)
{
    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t
bd_sysm_shutdown(shr_sysm_categories_t inst_cat, int unit, void *comp_data,
                 bool graceful, uint32_t *blocking_comp)
{
    /*
     * BD cleanup must wait for LM cleanup since LM firmware linkscan
     * uses bcmbd_m0ssq APIs.
     */
    if (!shr_sysm_is_comp_complete(BCMMGMT_LM_COMP_ID, inst_cat, unit)) {
        *blocking_comp = BCMMGMT_LM_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (SHR_FAILURE(bcmbd_dev_stop(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (SHR_FAILURE(bcmbd_detach(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMBD System manager init struct.
 */
static shr_sysm_cb_func_set_t bd_sysm_handler = {
    .init               = bd_sysm_init,
    .comp_config        = bd_sysm_comp_config,
    .pre_config         = bd_sysm_pre_config,
    .stop               = bd_sysm_stop,
    .shutdown           = bd_sysm_shutdown
};


/*******************************************************************************
 * Public Functions
 */

int
bcmbd_start(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_BD_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &bd_sysm_handler,
                           NULL));
exit:
    SHR_FUNC_EXIT();
}
