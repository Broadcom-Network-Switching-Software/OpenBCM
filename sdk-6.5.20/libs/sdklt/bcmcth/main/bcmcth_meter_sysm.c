/*! \file bcmcth_meter_sysm.c
 *
 * Tunnel component interface to system manager.
 * This file implements the Tunnel interface to system manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <sal/sal_alloc.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <shr/shr_sysm.h>
#include <bsl/bsl.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmdrd_config.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcth/bcmcth_meter.h>
#include <bcmcth/bcmcth_meter_internal.h>
#include <bcmcth/bcmcth_meter_fp_ing_imm.h>
#include <bcmcth/bcmcth_meter_fp_egr_imm.h>

#include <bcmcth/bcmcth_meter_sysm.h>
/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE  BSL_LS_BCMCTH_METER

/*******************************************************************************
 * Private functions
 */
/*!
 * \brief Initializes METER feature on this BCM unit.
 *
 * \param [in] inst_cat System manager instance category.
 * \param [in] unit Unit number.
 *
 * \retval SHR_SYSM_RV_DONE Success.
 * \retval SHR_SYSM_RV_ERROR On Failure.
 */
static shr_sysm_rv_t
cth_meter_sysm_comp_config(shr_sysm_categories_t inst_cat,
                           int unit,
                           void *comp_data,
                           bool warm,
                           uint32_t *blocking_comp)
{
    int idx;
    uint32_t comp_dep[] = {
        BCMMGMT_CFG_COMP_ID,
        BCMMGMT_DRD_COMP_ID,
        BCMMGMT_LTD_COMP_ID,
        BCMMGMT_BD_COMP_ID,
        BCMMGMT_PTM_COMP_ID,
        BCMMGMT_DI_COMP_ID,
        BCMMGMT_SER_COMP_ID,
        BCMMGMT_EVM_COMP_ID,
        BCMMGMT_ISSU_COMP_ID,
    };

    for (idx = 0; idx < COUNTOF(comp_dep); idx++) {
        if (!shr_sysm_is_comp_complete(comp_dep[idx], inst_cat, unit)) {
            *blocking_comp = comp_dep[idx];
            return SHR_SYSM_RV_BLOCKED;
        }
    }

    if (SHR_FAILURE(bcmcth_meter_attach(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }
    return SHR_SYSM_RV_DONE;
}

/* Phase: stop */
static shr_sysm_rv_t
cth_meter_sysm_stop(shr_sysm_categories_t instance_category,
                    int unit,
                    void *comp_data,
                    bool graceful,
                    uint32_t *blocking_component)
{
    const bcmlrd_map_t  *map = NULL;
    int                 rv = SHR_E_NONE;

    rv = bcmlrd_map_get(unit, METER_ING_FP_TEMPLATEt, &map);
    if (SHR_SUCCESS(rv) && map) {
        if (bcmevm_unregister_published_event(unit,
                                              "FP_ING_COMMIT",
                                              bcmcth_meter_fp_event_cb)) {
            return SHR_SYSM_RV_ERROR;
        }

        if (bcmevm_unregister_published_event(unit,
                                              "FP_ING_ABORT",
                                              bcmcth_meter_fp_event_cb)) {
            return SHR_SYSM_RV_ERROR;
        }
    }

    rv = bcmlrd_map_get(unit, METER_EGR_FP_TEMPLATEt, &map);
    if (SHR_SUCCESS(rv) && map) {
        if (bcmevm_unregister_published_event(unit,
                                              "FP_EGR_COMMIT",
                                              bcmcth_meter_fp_event_cb)) {
            return SHR_SYSM_RV_ERROR;
        }

        if (bcmevm_unregister_published_event(unit,
                                              "FP_EGR_ABORT",
                                              bcmcth_meter_fp_event_cb)) {
            return SHR_SYSM_RV_ERROR;
        }
    }
    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief Shutdown METER feature on this BCM unit.
 *
 * \param [in] inst_cat System manager instance category.
 * \param [in] unit Unit number.
 *
 * \retval SHR_SYSM_RV_DONE Success.
 * \retval SHR_SYSM_RV_ERROR On Failure.
 */
static shr_sysm_rv_t
cth_meter_sysm_shutdown(shr_sysm_categories_t inst_cat,
                        int unit,
                        void *comp_data,
                        bool graceful,
                        uint32_t *blocking_component)
{
    if (SHR_FAILURE(bcmcth_meter_detach(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMCTH Meter System manager init struct.
 */
static shr_sysm_cb_func_set_t cth_meter_sysm_handler = {

    /*! Component configuration. */
    .comp_config = cth_meter_sysm_comp_config,

    /*! Stop. */
    .stop = cth_meter_sysm_stop,

    /*! Shutdown. */
    .shutdown    = cth_meter_sysm_shutdown
};

/*******************************************************************************
 * Public functions
 */
/*!
 * \brief Meter component init.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_meter_start(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_METER_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &cth_meter_sysm_handler,
                           NULL));
exit:
    SHR_FUNC_EXIT();
}
