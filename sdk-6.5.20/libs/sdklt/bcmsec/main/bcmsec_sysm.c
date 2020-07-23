/*! \file bcmsec_sysm.c
 *
 * SEC interface to system manager.
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

#include <bcmsec/bcmsec_types.h>
#include <bcmsec/generated/bcmsec_ha.h>
#include <bcmsec/bcmsec_drv.h>
#include <bcmsec/bcmsec_sysm.h>
#include <bcmsec/bcmsec_event.h>
#include <bcmpc/bcmpc_sec.h>
#include <bcmsec/imm/bcmsec_imm_defines_internal.h>

#define BSL_LOG_MODULE  BSL_LS_BCMSEC_INIT

/*******************************************************************************
 * SYSM callback functions
 */

static bool warmboot[BCMDRD_CONFIG_MAX_UNITS];

static shr_sysm_rv_t
sec_sysm_init(shr_sysm_categories_t instance_category,
             int unit,
             void *comp_data,
             bool warm,
             uint32_t *blocking_component)
{
    /*
     * Since bcmsec_pc_drv_tm_config_set will be called from bcmsec_attach, we
     * need to make sure that PC is already initialized
     */
    if (!shr_sysm_is_comp_complete(BCMMGMT_PC_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_PC_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (SHR_FAILURE(bcmsec_attach(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    warmboot[unit] = warm;
    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t
sec_sysm_comp_config(shr_sysm_categories_t instance_category,
                     int unit,
                     void *comp_data,
                     bool warm,
                     uint32_t *blocking_component)
{
    bcmsec_drv_t *sec_drv = NULL;
    bcmpc_sec_handler_t *pc_hdl;

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

    /* bcmsec imm component initialization. */
    if (!shr_sysm_is_comp_complete(BCMMGMT_IMM_FE_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_IMM_FE_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (SHR_FAILURE(bcmsec_imm_reg(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (SHR_FAILURE(bcmsec_imm_cb_reg(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    /*
     * The component driver should be ready at or before comp-config phase,
     * because LTP will start to play the LT configuration at pre-config phase.
     */

    if (SHR_FAILURE(bcmsec_drv_get(unit, &sec_drv))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (sec_drv && sec_drv->dev_init) {
        if (SHR_FAILURE(sec_drv->dev_init(unit, warm))) {
            return SHR_SYSM_RV_ERROR;
        }
    }

    if (sec_drv && sec_drv->sec_pc_hdl_get) {
        pc_hdl = sec_drv->sec_pc_hdl_get(unit);
        if (SHR_FAILURE(bcmpc_sec_handler_register(unit, pc_hdl))) {
            return SHR_SYSM_RV_ERROR;
        }
    }


    if (SHR_FAILURE(bcmsec_imm_init(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (SHR_FAILURE(bcmsec_event_init(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t
sec_sysm_stop(shr_sysm_categories_t instance_category,
             int unit,
             void *comp_data,
             bool graceful,
             uint32_t *blocking_component)
{
    if (SHR_FAILURE(bcmsec_stop(unit, graceful))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t
sec_sysm_shutdown(shr_sysm_categories_t instance_category,
                 int unit,
                 void *comp_data,
                 bool graceful,
                 uint32_t *blocking_component)
{
    if (SHR_FAILURE(bcmsec_detach(unit, graceful))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

static void
sec_sysm_run(shr_sysm_categories_t instance_category,
            int unit,
            void *comp_data)
{
    int rv;

    rv = bcmsec_run(unit, warmboot[unit]);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "bcmsec_run failed (rv = %d).\n"),
                   rv));
    }
}

/*!
 * \brief bcmsec System manager init struct.
 */
static shr_sysm_cb_func_set_t tm_sysm_handler = {
    /*! Initialization. */
    .init        = sec_sysm_init,

    /*! Component configuration. */
    .comp_config = sec_sysm_comp_config,

    /*! Pre-configuration. */
    .pre_config  = NULL,

    /*! Run. */
    .run         = sec_sysm_run,

    /*! Stop. */
    .stop        = sec_sysm_stop,

    /*! Shutdown. */
    .shutdown    = sec_sysm_shutdown
};


/*******************************************************************************
 * Public Functions
 */

int
bcmsec_start(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_SEC_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &tm_sysm_handler,
                           NULL));
exit:
    SHR_FUNC_EXIT();
}
