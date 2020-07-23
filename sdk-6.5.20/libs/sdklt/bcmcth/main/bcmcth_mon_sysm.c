/*! \file bcmcth_mon_sysm.c
 *
 * BCMCTH interface to system manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_sysm.h>
#include <bcmmgmt/bcmmgmt_sysm.h>

#include <bcmcth/bcmcth_mon.h>
#include <bcmcth/bcmcth_mon_sysm.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_MIRROR

/*******************************************************************************
 * SYSM callback functions
 */

/*!
 * \brief BCMCTH config function.
 *
 * BCMCTH is requested to configure.
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
cth_mon_sysm_comp_config(shr_sysm_categories_t inst_cat,
                         int unit, void *comp_data, bool warm,
                         uint32_t *blocking_component)
{
    if (!shr_sysm_is_comp_complete(BCMMGMT_IMM_FE_COMP_ID,
                                   inst_cat,
                                   unit)) {
        *blocking_component = BCMMGMT_IMM_FE_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (SHR_FAILURE(bcmcth_mon_attach(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMCTH shutdown function.
 *
 * BCMCTH is requested to being shutdown completely.
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
cth_mon_sysm_shutdown(shr_sysm_categories_t inst_cat,
                      int unit, void *comp_data, bool graceful,
                      uint32_t *blocking_component)
{
    if (SHR_FAILURE(bcmcth_mon_detach(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMCTH System manager init struct.
 */
static shr_sysm_cb_func_set_t cth_mon_sysm_handler = {

    /*! Component configuration. */
    .comp_config = cth_mon_sysm_comp_config,

    /*! Shutdown. */
    .shutdown    = cth_mon_sysm_shutdown
};


/*******************************************************************************
 * Public Functions
 */
/*!
 * \brief Start CTH component.
 *
 * Register CTH component with System Manager.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_PARAM  Component not recognized.
 * \retval SHR_E_EXISTS Component already registered.
 */
int
bcmcth_mon_start(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_MON_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &cth_mon_sysm_handler,
                           NULL));
exit:
    SHR_FUNC_EXIT();
}
