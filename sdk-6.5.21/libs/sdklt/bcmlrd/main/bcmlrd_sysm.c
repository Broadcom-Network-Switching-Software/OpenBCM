/*! \file bcmlrd_sysm.c
 *
 * \brief Logical Table Resource Database system manager interface.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_types.h>
#include <sal/sal_libc.h>
#include <sal/sal_assert.h>
#include <sal/sal_types.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <shr/shr_sysm.h>
#include <bcmlrd/bcmlrd_internal.h>
#include <bcmlrd/bcmlrd_init.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmlrd/bcmlrd_sysm.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLRD_SYSM

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief BCMLRD generic initialization function.
 *
 * Genearal initialization of BCMLRD module.
 *
 * \param [in] instance_category System manager instance category.
 * \param [in] unit The device unit number.
 * \param [in] comp_data Pointer to the component context.
 * \param [in] warm Indicates cold or warm start.
 * \param [out] blocking_comp Blocking component.
 *
 * \return SHR_SYSM_RV_DONE on success, SHR_SYSM_RV_ERROR on error.
 */
static shr_sysm_rv_t
bcmlrd_generic_init(shr_sysm_categories_t instance_category,
                 int unit,
                 void *comp_data,
                 bool warm,
                 uint32_t *blocking_component)
{
    shr_sysm_rv_t sysm_rv = SHR_SYSM_RV_ERROR;
    int rv;

    /* Check dependency on DRD. */
    if (!shr_sysm_is_comp_complete(BCMMGMT_DRD_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_DRD_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    sysm_rv = SHR_SYSM_RV_ERROR;
    rv = bcmlrd_init();
    if (SHR_SUCCESS(rv)) {
        sysm_rv = SHR_SYSM_RV_DONE;
    }
    return sysm_rv;
}

/*!
 * \brief BCMLRD config function.
 *
 * General configuration of BCMLRD.
 *
 * \param [in] instance_category System manager instance category.
 * \param [in] unit The device unit number.
 * \param [in] comp_data Pointer to the component context.
 * \param [in] warm Indicates cold or warm start.
 * \param [out] blocking_comp Blocking component.
 *
 * \return SHR_SYSM_RV_DONE on success.
 */
static shr_sysm_rv_t
bcmlrd_generic_comp_config(shr_sysm_categories_t instance_category,
                 int unit,
                 void *comp_data,
                 bool warm,
                 uint32_t *blocking_component)
{
    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMLRG generic pre_config function.
 *
 * General pre-configure for BCMLRD.
 *
 * \param [in] instance_category System manager instance category.
 * \param [in] unit The device unit number.
 * \param [in] comp_data Pointer to the component context.
 * \param [in] warm Indicates cold or warm start.
 * \param [out] blocking_comp Blocking component.
 *
 * This function signature is used for initialization and configuration
 * callback events by the system manager. A component should provide these
 * callback functions if it chooses to be called for the relevant event.
 *
 *
 * \return SHR_SYSM_RV_DONE on success.
 */
static shr_sysm_rv_t
bcmlrd_generic_pre_config(shr_sysm_categories_t instance_category,
                       int unit,
                       void *comp_data,
                       bool warm,
                       uint32_t *blocking_component)
{
    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMLRD run function.
 *
 * Execute BCMLRD.
 *
 * \param [in] instance_category System manager instance category.
 * \param [in] unit The device unit number.
 * \param [in] comp_data Pointer to the component context.
 *
 * \return none.
 */
static void
bcmlrd_generic_run(shr_sysm_categories_t instance_category,
                int unit,
                void *comp_data)
{
}

/*!
 * \brief BCMLRD generic stop function.
 *
 * Stop execution of BCMLRD module.
 * This function signature is used for initialization and configuration
 * callback events by the system manager. A component should provide these
 * callback functions if it chooses to be called for the relevant event.
 *
 * \param [in] instance_category System manager instance category.
 * \param [in] unit The device unit number.
 * \param [in] comp_data Pointer to the component context.
 * \param [in] graceful Indicates abrupt or graceful stop.
 * \param [out] blocking_comp Blocking component.
 *
 * \return SHR_SYSM_RV_DONE on success.
 */
static shr_sysm_rv_t
bcmlrd_generic_stop(shr_sysm_categories_t instance_category,
                 int unit,
                 void *comp_data,
                 bool graceful,
                 uint32_t *blocking_component)
{
    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMLRD shutdown function.
 *
 * Shutdown of generic BCMLRD.
 *
 * \param [in] instance_category System manager instance category.
 * \param [in] unit The device unit number.
 * \param [in] comp_data Pointer to the component context.
 * \param [in] warm Indicates cold or warm start.
 * \param [out] blocking_comp Blocking component.
 *
 * \return SHR_SYSM_RV_DONE on success, SHR_SYSM_RV_ERROR on error and
 *         SHR_SYSM_RV_BLOCKED when waiting on other component.
 */
static shr_sysm_rv_t
bcmlrd_generic_shutdown(shr_sysm_categories_t instance_category,
                     int unit,
                     void *comp_data,
                     bool graceful,
                     uint32_t *blocking_component)
{
    shr_sysm_rv_t sysm_rv;
    int rv;

    sysm_rv = SHR_SYSM_RV_ERROR;
    rv = bcmlrd_cleanup();
    if (SHR_SUCCESS(rv)) {
        sysm_rv = SHR_SYSM_RV_DONE;
    }
    return sysm_rv;
}

/*!
 * \brief BCMLRD generic system manager initialization.
 */
static shr_sysm_cb_func_set_t bcmlrd_sysm_handler = {
    /*! Initialization. */
    .init        = bcmlrd_generic_init,

    /*! Component configuration. */
    .comp_config = bcmlrd_generic_comp_config,

    /*! Pre-configuration. */
    .pre_config  = bcmlrd_generic_pre_config,

    /*! Run. */
    .run         = bcmlrd_generic_run,

    /*! Stop. */
    .stop        = bcmlrd_generic_stop,

    /*! Shutdown. */
    .shutdown    = bcmlrd_generic_shutdown
};

/*******************************************************************************
 * Public functions
 */

int
bcmlrd_start(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_LRD_COMP_ID,
                           SHR_SYSM_CAT_GENERIC,
                           &bcmlrd_sysm_handler,
                           NULL));
exit:
    SHR_FUNC_EXIT();
}

