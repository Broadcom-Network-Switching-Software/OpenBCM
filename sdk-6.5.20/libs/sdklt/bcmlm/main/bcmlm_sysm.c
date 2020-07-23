/*! \file bcmlm_sysm.c
 *
 * Interface functions to the system manager
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

#include <bcmlm/bcmlm_sysm.h>
#include <bcmlm/bcmlm_drv_internal.h>
#include "bcmlm_internal.h"

/*******************************************************************************
 * Defines
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMLM_SYSM

/*******************************************************************************
 * Typedefs
 */

/*******************************************************************************
 * Private variables
 */

static int warmboot;

/*******************************************************************************
 * Private Functions
 */

/* Phase: init */
static shr_sysm_rv_t
unit_init(shr_sysm_categories_t instance_category,
          int unit,
          void *comp_data,
          bool warm,
          uint32_t *blocking_component)
{
    /* Dependencies */
    if (!shr_sysm_is_comp_complete(BCMMGMT_CFG_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_CFG_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_IMM_FE_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_IMM_FE_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (SHR_FAILURE(bcmlm_ctrl_init(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/* Phase: comp_config */
static shr_sysm_rv_t
unit_comp_config(shr_sysm_categories_t instance_category,
                 int unit,
                 void *comp_data,
                 bool warm,
                 uint32_t *blocking_component)
{
    /* Dependencies */
    if (!shr_sysm_is_comp_complete(BCMMGMT_CFG_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_CFG_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_IMM_FE_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_IMM_FE_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_BD_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_BD_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_DRD_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_DRD_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_EVM_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_DRD_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_PC_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_PC_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (SHR_FAILURE(bcmlm_drv_attach(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (SHR_FAILURE(bcmlm_imm_register(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (SHR_FAILURE(bcmlm_dev_init(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (SHR_FAILURE(bcmlm_port_event_register(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/* Phase: pre_config */
static shr_sysm_rv_t
unit_pre_config(shr_sysm_categories_t instance_category,
                int unit,
                void *comp_data,
                bool warm,
                uint32_t *blocking_component)
{
    warmboot = warm;

    return SHR_SYSM_RV_DONE;
}

/* Phase: run */
static void
unit_run(shr_sysm_categories_t instance_category,
         int unit,
         void *comp_data)
{
    int rv;

    if (warmboot) {
        rv = bcmlm_ctrl_link_reload(unit);
        if (SHR_FAILURE(rv)) {
            LOG_ERROR(BSL_LOG_MODULE,
                      (BSL_META_U(unit, "bcmlm_ctrl_reload failed (rv = %d).\n"),
                       rv));
            return;
        }
    }

    rv = bcmlm_ctrl_ready(unit);
    if (SHR_FAILURE(rv)) {
        LOG_ERROR(BSL_LOG_MODULE,
                  (BSL_META_U(unit, "bcmlm_ctrl_run failed (rv = %d).\n"),
                   rv));
    }
}

/* Phase: stop */
static shr_sysm_rv_t
unit_stop(shr_sysm_categories_t instance_category,
          int unit,
          void *comp_data,
          bool graceful,
          uint32_t *blocking_component)
{
    if (SHR_FAILURE(bcmlm_port_event_unregister(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (SHR_FAILURE(bcmlm_ctrl_stop(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/* Phase: shutdown */
static shr_sysm_rv_t
unit_shutdown(shr_sysm_categories_t instance_category,
              int unit,
              void *comp_data,
              bool graceful,
              uint32_t *blocking_component)
{
    if (SHR_FAILURE(bcmlm_dev_cleanup(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (SHR_FAILURE(bcmlm_drv_detach(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (SHR_FAILURE(bcmlm_ctrl_cleanup(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t sys_init(shr_sysm_categories_t instance_category,
                              int unit,
                              void *comp_data,
                              bool warm,
                              uint32_t *blocking_component)
{
    if (bcmlm_imm_db_init() != SHR_E_NONE) {
        return SHR_SYSM_RV_ERROR;
    }
    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t sys_shutdown(shr_sysm_categories_t instance_category,
                                  int unit,
                                  void *comp_data,
                                  bool graceful,
                                  uint32_t *blocking_component)
{
    bcmlm_imm_db_cleanup();

    return SHR_SYSM_RV_DONE;
}


/*******************************************************************************
 * Public Functions
 */
int
bcmlm_start(void)
{
    shr_sysm_cb_func_set_t funcs;
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Register unit-independent functions */
    sal_memset(&funcs, 0, sizeof(funcs));
    funcs.init = sys_init;
    funcs.shutdown = sys_shutdown;
    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_LM_COMP_ID,
                           SHR_SYSM_CAT_GENERIC,
                           &funcs,
                           NULL));


    /* Register unit-specific functions */
    sal_memset(&funcs, 0, sizeof(funcs));
    funcs.init = unit_init;
    funcs.comp_config = unit_comp_config;
    funcs.pre_config = unit_pre_config;
    funcs.run = unit_run;
    funcs.stop = unit_stop;
    funcs.shutdown = unit_shutdown;
    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_LM_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &funcs,
                           NULL));
exit:
    SHR_FUNC_EXIT();
}
