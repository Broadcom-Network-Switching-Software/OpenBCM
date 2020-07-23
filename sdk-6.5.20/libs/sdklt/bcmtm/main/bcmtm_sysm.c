/*! \file bcmtm_sysm.c
 *
 * BCMTM interface to system manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_sysm.h>
#include <bcmmgmt/bcmmgmt_sysm.h>

#include <bcmtm/bcmtm_types.h>
#include <bcmtm/bcmtm_drv.h>
#include <bcmtm/bcmtm_sysm.h>
#include <bcmtm/imm/bcmtm_imm_defines_internal.h>

#define BSL_LOG_MODULE  BSL_LS_BCMTM_INIT

/*******************************************************************************
 * SYSM callback functions
 */

static bool warmboot[BCMDRD_CONFIG_MAX_UNITS];

static shr_sysm_rv_t
tm_sysm_init(shr_sysm_categories_t instance_category,
             int unit,
             void *comp_data,
             bool warm,
             uint32_t *blocking_component)
{
    warmboot[unit] = warm;
    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t
tm_sysm_comp_config(shr_sysm_categories_t instance_category,
                    int unit,
                    void *comp_data,
                    bool warm,
                    uint32_t *blocking_component)
{
    bcmtm_drv_t *tm_drv = NULL;
    bcmpc_tm_handler_t *pc_hdl;

    if (!shr_sysm_is_comp_complete(BCMMGMT_CFG_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_CFG_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_PC_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_PC_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_PTM_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_PTM_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_DI_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_DI_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_SER_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_SER_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    /* BCMTM imm component initialization. */
    if (!shr_sysm_is_comp_complete(BCMMGMT_IMM_FE_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_IMM_FE_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }
    if (SHR_FAILURE(bcmtm_attach(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (SHR_FAILURE(bcmtm_imm_reg(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (SHR_FAILURE(bcmtm_imm_cb_reg(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    /*
     * The component driver should be ready at or before comp-config phase,
     * because LTP will start to play the LT configuration at pre-config phase.
     */

    if (SHR_FAILURE(bcmtm_drv_get(unit, &tm_drv))) {
        return SHR_SYSM_RV_ERROR;
    }

    /* PFC deadlock init. */
    if (SHR_FAILURE(bcmtm_pfc_deadlock_monitor_init(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (tm_drv && tm_drv->dev_init) {
        if (SHR_FAILURE(tm_drv->dev_init(unit, warm))) {
            return SHR_SYSM_RV_ERROR;
        }
    }

    if (tm_drv && tm_drv->pc_hdl_get) {
        pc_hdl = tm_drv->pc_hdl_get(unit);
        if (SHR_FAILURE(bcmpc_tm_handler_register(unit, pc_hdl))) {
            return SHR_SYSM_RV_ERROR;
        }
    }


    if (SHR_FAILURE(bcmtm_imm_init(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t
tm_sysm_stop(shr_sysm_categories_t instance_category,
             int unit,
             void *comp_data,
             bool graceful,
             uint32_t *blocking_component)
{
    if (SHR_FAILURE(bcmtm_stop(unit, graceful))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t
tm_sysm_shutdown(shr_sysm_categories_t instance_category,
                 int unit,
                 void *comp_data,
                 bool graceful,
                 uint32_t *blocking_component)
{
    if (SHR_FAILURE(bcmtm_detach(unit, graceful))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

static void
tm_sysm_run(shr_sysm_categories_t instance_category,
            int unit,
            void *comp_data)
{
    int rv;

    rv = bcmtm_run(unit, warmboot[unit]);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "bcmtm_run failed (rv = %d).\n"),
                   rv));
    }
}

/*!
 * \brief BCMTM System manager init struct.
 */
static shr_sysm_cb_func_set_t tm_sysm_handler = {
    /*! Initialization. */
    .init        = tm_sysm_init,

    /*! Component configuration. */
    .comp_config = tm_sysm_comp_config,

    /*! Pre-configuration. */
    .pre_config  = NULL,

    /*! Run. */
    .run         = tm_sysm_run,

    /*! Stop. */
    .stop        = tm_sysm_stop,

    /*! Shutdown. */
    .shutdown    = tm_sysm_shutdown
};


/*******************************************************************************
 * Public Functions
 */

int
bcmtm_start(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_TM_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &tm_sysm_handler,
                           NULL));
exit:
    SHR_FUNC_EXIT();
}

