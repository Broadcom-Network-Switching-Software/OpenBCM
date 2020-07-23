/*! \file bcmdi_sysm.c
 *
 * Flex Code Loader - System Manager Integration
 *
 * This file contains the DI implementation for the integration
 * to the System Manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_sysm.h>
#include <bsl/bsl.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmdi/bcmdi.h>
#include <bcmdi/bcmdi_sysm.h>

#define BSL_LOG_MODULE BSL_LS_BCMDI_INIT

static bcmmgmt_sysm_comp_id_t di_init_comp_list[] =
    {BCMMGMT_BD_COMP_ID,
     BCMMGMT_DRD_COMP_ID,
     BCMMGMT_PTM_COMP_ID,
     BCMMGMT_SER_COMP_ID,
     BCMMGMT_MAX_COMP_ID}; /* end of list indicator */

static bool
bcmdi_is_blocked(shr_sysm_categories_t instance_category, int unit,
               bcmmgmt_sysm_comp_id_t *comp_list,
               uint32_t *blocking_component)
{
    int i;
    for (i = 0; ; i++) {
        if (comp_list[i] == BCMMGMT_MAX_COMP_ID) {
            break;
        }
        if (!shr_sysm_is_comp_complete(comp_list[i], instance_category, unit)) {
            *blocking_component = comp_list[i];
            return TRUE;
        }
    }
    return FALSE;
}

static shr_sysm_rv_t
bcmdi_unit_comp_config(shr_sysm_categories_t instance_category,
                 int unit,
                 void *comp_data,
                 bool warm,
                 uint32_t *blocking_component)
{
    if (bcmdi_is_blocked(instance_category, unit, di_init_comp_list,
                       blocking_component)) {
        return SHR_SYSM_RV_BLOCKED;
    }

    if (SHR_FAILURE(bcmdi_devicecode_drv_init(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }
    if (SHR_FAILURE(bcmdi_devicecode_load(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }
    if (SHR_FAILURE(bcmdi_devicecode_extra(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (SHR_FAILURE(bcmdi_flexcode_drv_init(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }
    if (bcmdrd_feature_enabled(unit, BCMDRD_FT_FLEXCODE) &&
        bcmdi_flexcode_is_supported(unit)) {
        if (SHR_FAILURE(bcmdi_flexcode_load(unit, "DEFAULT", BCMPTM_DIRECT_TRANS_ID, warm))) {
            return SHR_SYSM_RV_ERROR;
        }
    } else {
        LOG_VERBOSE(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                                "Skip Flexcode load (not supported)\n")));
    }

    if (SHR_FAILURE(bcmdi_devicecode_extra_post(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t
bcmdi_unit_shutdown(shr_sysm_categories_t instance_category,
                 int unit,
                 void *comp_data,
                 bool graceful,
                 uint32_t *blocking_component)
{
    LOG_DEBUG
        (BSL_LOG_MODULE, (BSL_META_U(unit, "bcmdi_unit_shutdown.\n")));

    if (SHR_FAILURE(bcmdi_devicecode_drv_cleanup(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (SHR_FAILURE(bcmdi_flexcode_drv_cleanup(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}


/*!
 * \brief DI System manager init struct.
 */
static shr_sysm_cb_func_set_t di_sysm_handler = {
    /*! Initialization. */
    .init        = NULL,

    /*! Component configuration. */
    .comp_config = bcmdi_unit_comp_config,

    /*! Pre-configuration. */
    .pre_config  = NULL,

    /*! Run. */
    .run         = NULL,

    /*! Stop. */
    .stop        = NULL,

    /*! Shutdown. */
    .shutdown    = bcmdi_unit_shutdown
};

/*
 * Called by System Manager to start BCMDI.
 */
int
bcmdi_start(void) {
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_DI_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &di_sysm_handler,
                           NULL));
exit:
    SHR_FUNC_EXIT();
}
