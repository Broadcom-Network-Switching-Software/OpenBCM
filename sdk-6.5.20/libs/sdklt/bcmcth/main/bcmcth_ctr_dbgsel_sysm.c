/*! \file bcmcth_ctr_dbgsel_sysm.c
 *
 * BCMCTH ING/EGR Debug Counter Trigger Selection interface to system manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_sysm.h>
#include <bcmmgmt/bcmmgmt_sysm.h>

#include <bcmcth/bcmcth_ctr_dbgsel.h>
#include <bcmcth/bcmcth_ctr_dbgsel_sysm.h>

/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTRDBGSEL

/*******************************************************************************
 * SYSM callback functions
 */
/*!
 * \brief BCMCTH CTR_DBGSEL shutdown function.
 *
 * BCMCTH CTR_DBGSEL is requested to being shutdown completely.
 *
 * \param [in] inst_cat System manager instance category.
 * \param [in] unit Unit number.
 * \param [in] comp_data Pointer to the component context.
 * \param [in] warm Indicates cold or warm start.
 * \param [out] blocking_component Blocking component.
 *
 * \retval SHR_SYSM_RV_DONE Success.
 * \retval SHR_SYSM_RV_ERROR On Failure.
 */
static shr_sysm_rv_t
cth_ctr_dbgsel_sysm_shutdown(shr_sysm_categories_t inst_cat,
                       int unit, void *comp_data, bool warm,
                       uint32_t *blocking_component)
{
    if (SHR_FAILURE(bcmcth_ctr_dbgsel_detach(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMCTH CTR_DBGSEL config function.
 *
 * BCMCTH CTR_DBGSEL is requested to configure.
 *
 * \param [in] inst_cat System manager instance category.
 * \param [in] unit Unit number.
 * \param [in] comp_data Pointer to the component context.
 * \param [in] warm Indicates cold or warm start.
 * \param [out] blocking_component Blocking component.
 *
 * \retval SHR_SYSM_RV_DONE Success.
 * \retval SHR_SYSM_RV_ERROR On Failure.
 */
static shr_sysm_rv_t
cth_ctr_dbgsel_sysm_comp_config(shr_sysm_categories_t inst_cat,
                                int unit,
                                void *comp_data,
                                bool warm,
                                uint32_t *blocking_component)
{
    int idx;
    uint32_t comp_dep[] = {
        BCMMGMT_CFG_COMP_ID,
        BCMMGMT_PTM_COMP_ID,
    };

    for (idx = 0; idx < COUNTOF(comp_dep); idx++) {
        if (!shr_sysm_is_comp_complete(comp_dep[idx], inst_cat, unit)) {
            *blocking_component = comp_dep[idx];
            return SHR_SYSM_RV_BLOCKED;
        }
    }

    if (SHR_FAILURE(bcmcth_ctr_dbgsel_attach(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMCTH CTR_DBGSEL System manager init struct.
 */
static shr_sysm_cb_func_set_t cth_ctr_dbgsel_sysm_handler = {

    /*! Component configuration. */
    .comp_config = cth_ctr_dbgsel_sysm_comp_config,

    /*! Shutdown. */
    .shutdown    = cth_ctr_dbgsel_sysm_shutdown
};

/*******************************************************************************
 * Public Functions
 */
int
bcmcth_ctr_dbgsel_start(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_CTR_DBGSEL_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &cth_ctr_dbgsel_sysm_handler,
                           NULL));
exit:
    SHR_FUNC_EXIT();
}

