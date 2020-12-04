/*! \file bcmcth_vlan_sysm.c
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
#include <shr/shr_debug.h>
#include <shr/shr_sysm.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmltd/chip/bcmltd_id.h>
#include <bcmcth/bcmcth_vlan_sysm.h>
#include <bcmcth/bcmcth_vlan.h>


/*******************************************************************************
 * Defines
 */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_VLAN

/*******************************************************************************
 * SYSM callback functions
 */
/*!
 * \brief BCMCTH VLAN shutdown function.
 *
 * BCMCTH VLAN is requested to being shutdown completely.
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
cth_vlan_sysm_shutdown(shr_sysm_categories_t inst_cat,
                       int unit, void *comp_data, bool graceful,
                       uint32_t *blocking_component)
{
    if (SHR_FAILURE(bcmcth_vlan_detach(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMCTH VLAN config function.
 *
 * BCMCTH VLAN is requested to configure.
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
cth_vlan_sysm_comp_config(shr_sysm_categories_t inst_cat,
                          int unit,
                          void *comp_data,
                          bool warm,
                          uint32_t *blocking_component)
{
    int idx;
    uint32_t comp_dep[] = {
        BCMMGMT_CFG_COMP_ID,
        BCMMGMT_PTM_COMP_ID,
        BCMMGMT_DI_COMP_ID,
        BCMMGMT_SER_COMP_ID
    };

    for (idx = 0; idx < COUNTOF(comp_dep); idx++) {
        if (!shr_sysm_is_comp_complete(comp_dep[idx], inst_cat, unit)) {
            *blocking_component = comp_dep[idx];
            return SHR_SYSM_RV_BLOCKED;
        }
    }

    if (SHR_FAILURE(bcmcth_vlan_attach(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMCTH VLAN System manager init struct.
 */
static shr_sysm_cb_func_set_t cth_vlan_sysm_handler = {

    /*! Component configuration. */
    .comp_config = cth_vlan_sysm_comp_config,

    /*! Shutdown. */
    .shutdown    = cth_vlan_sysm_shutdown
};
/*******************************************************************************
 * Public Functions
 */
int
bcmcth_vlan_start(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_VLAN_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &cth_vlan_sysm_handler,
                           NULL));
exit:
    SHR_FUNC_EXIT();
}
