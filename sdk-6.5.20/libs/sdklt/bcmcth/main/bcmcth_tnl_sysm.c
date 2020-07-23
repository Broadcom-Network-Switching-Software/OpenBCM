/*! \file bcmcth_tnl_sysm.c
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
#include <bcmdrd_config.h>
#include <bcmdrd/bcmdrd_types.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmlrd/bcmlrd_map.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcth/bcmcth_tnl.h>

#include <bcmcth/bcmcth_tnl_sysm.h>
/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE  BSL_LS_BCMCTH_TNL

/*******************************************************************************
 * Public functions
 */
/*!
 * \brief Initializes tunnel feature on this BCM unit.
 *
 * \param [in] inst_cat System manager instance category.
 * \param [in] unit Unit number.
 *
 * \retval SHR_SYSM_RV_DONE Success.
 * \retval SHR_SYSM_RV_ERROR On Failure.
 */
static shr_sysm_rv_t
cth_tnl_sysm_comp_config(shr_sysm_categories_t inst_cat,
                         int unit,
                         void *comp_data,
                         bool warm,
                         uint32_t *blocking_comp)
{
    int idx;
    uint32_t comp_dep[] = {
        BCMMGMT_DRD_COMP_ID,
        BCMMGMT_LTD_COMP_ID,
        BCMMGMT_BD_COMP_ID,
        BCMMGMT_PTM_COMP_ID,
        BCMMGMT_DI_COMP_ID,
        BCMMGMT_SER_COMP_ID,
        BCMMGMT_IMM_FE_COMP_ID
    };

    for (idx = 0; idx < COUNTOF(comp_dep); idx++) {
        if (!shr_sysm_is_comp_complete(comp_dep[idx], inst_cat, unit)) {
            *blocking_comp = comp_dep[idx];
            return SHR_SYSM_RV_BLOCKED;
        }
    }

    if (SHR_FAILURE(bcmcth_tnl_init(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief Shutsdown tunnel feature on this BCM unit.
 *
 * \param [in] instance_category System manager instance category.
 * \param [in] unit Unit number.
 *
 * \retval SHR_SYSM_RV_DONE Success.
 * \retval SHR_SYSM_RV_ERROR On Failure.
 */
static shr_sysm_rv_t
cth_tnl_sysm_shutdown(shr_sysm_categories_t inst_cat,
                      int unit,
                      void *comp_data,
                      bool graceful,
                      uint32_t *blocking_comp)
{
    if (SHR_FAILURE(bcmcth_tnl_cleanup(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMCTH Tunnel System manager init struct.
 */
static shr_sysm_cb_func_set_t cth_tnl_sysm_handler = {

    /*! Component configuration. */
    .comp_config = cth_tnl_sysm_comp_config,

    /*! Shutdown. */
    .shutdown    = cth_tnl_sysm_shutdown
};

/*******************************************************************************
 * Public functions
 */
/*!
 * \brief Tunnel component init.
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int
bcmcth_tnl_start(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_TNL_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &cth_tnl_sysm_handler,
                           NULL));
exit:
    SHR_FUNC_EXIT();
}
