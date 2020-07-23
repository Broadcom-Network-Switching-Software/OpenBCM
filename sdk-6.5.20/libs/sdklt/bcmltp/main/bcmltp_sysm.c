/*! \file bcmltp_sysm.c
 *
 * BCMLTP System Manager interface.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_assert.h>
#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_sysm.h>
#include <bcmdrd/bcmdrd_dev.h>
#include <bcmltp/bcmltp_player.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmltp/bcmltp_sysm.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module - use BCMCFG */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_INIT

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief Playback BCMCFG data.
 *
 * Playback BCMCFG data for given unit.
 *
 * \param [in]  unit                    The device unit number.
 *
 * \return SHR_SYSM_RV_DONE on success, SHR_SYSM_RV_ERROR on error.
 */
static shr_sysm_rv_t
bcmltp_sysm_play(int unit)
{
    shr_sysm_rv_t sysm_rv = SHR_SYSM_RV_DONE;
    int rv;

    /* Play back configuration for given unit. */
    rv = bcmltp_play(unit);
    if (SHR_FAILURE(rv) && rv != SHR_E_UNAVAIL) {
        sysm_rv = SHR_SYSM_RV_ERROR;
    }

    return sysm_rv;
}

/*!
 * \brief BCMLTP initialization function.
 *
 * BCMLTP is requested to initialize.
 *
 * \param [in]  instance_category       System manager instance category.
 * \param [in]  unit                    The device unit number.
 * \param [in]  comp_data               Pointer to the component context.
 * \param [in]  warm                    Indicates cold or warm start.
 * \param [out] blocking_comp           Blocking component.
 *
 * \return SHR_SYSM_RV_DONE on success, SHR_SYSM_RV_ERROR on error and
 *         SHR_SYSM_RV_BLOCKED when waiting on other component.
 */
static shr_sysm_rv_t
bcmltp_sysm_init(shr_sysm_categories_t instance_category,
                 int unit,
                 void *comp_data,
                 bool warm,
                 uint32_t *blocking_component)
{
    shr_sysm_rv_t sysm_rv = SHR_SYSM_RV_DONE;

    if (instance_category == SHR_SYSM_CAT_UNIT) {
        if (!shr_sysm_is_comp_complete(BCMMGMT_TRM_COMP_ID,
                                       instance_category,
                                       unit)) {
            *blocking_component = BCMMGMT_TRM_COMP_ID;
            sysm_rv = SHR_SYSM_RV_BLOCKED;
        }
    }

    return sysm_rv;
}

/*!
 * \brief BCMLTP config function.
 *
 * BCMLTP is requested to configure.
 *
 * \param [in]  instance_category       System manager instance category.
 * \param [in]  unit                    The device unit number.
 * \param [in]  comp_data               Pointer to the component context.
 * \param [in]  warm                    Indicates cold or warm start.
 * \param [out] blocking_comp           Blocking component.
 *
 * \return SHR_SYSM_RV_DONE on success, SHR_SYSM_RV_ERROR on error and
 *         SHR_SYSM_RV_BLOCKED when waiting on other component.
 */
static shr_sysm_rv_t
bcmltp_sysm_comp_config(shr_sysm_categories_t instance_category,
                 int unit,
                 void *comp_data,
                 bool warm,
                 uint32_t *blocking_component)
{
    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMLTP pre_config function.
 *
 * BCMLTP is requested to pre-configure.
 *
 * \param [in]  instance_category       System manager instance category.
 * \param [in]  unit                    The device unit number.
 * \param [in]  comp_data               Pointer to the component context.
 * \param [in]  warm                    Indicates cold or warm start.
 * \param [out] blocking_comp           Blocking component.
 *
 * This function signature is used for initialization and configuration
 * callback events by the system manager. A component should provide these
 * callback functions if it chooses to be called for the relevant event.
 *
 *
 * \return SHR_SYSM_RV_DONE on success, SHR_SYSM_RV_ERROR on error and
 *         SHR_SYSM_RV_BLOCKED when waiting on other component.
 */
static shr_sysm_rv_t
bcmltp_sysm_pre_config(shr_sysm_categories_t instance_category,
                       int unit,
                       void *comp_data,
                       bool warm,
                       uint32_t *blocking_component)
{
    shr_sysm_rv_t sysm_rv;

    if (instance_category == SHR_SYSM_CAT_UNIT &&
        !warm) {
        sysm_rv = bcmltp_sysm_play(unit);
    } else {
        sysm_rv = SHR_SYSM_RV_DONE;
    }

    return sysm_rv;
}

/*!
 * \brief BCMLTP run function
 *
 * BCMLTP is requested to run.
 *
 * \param [in]  instance_category       System manager instance category.
 * \param [in]  unit                    The device unit number.
 * \param [in]  comp_data               Pointer to the component context.
 *
 * \return none.
 */
static void
bcmltp_sysm_run(shr_sysm_categories_t instance_category,
                int unit,
                void *comp_data)
{
}

/*!
 * \brief BCMLTP stop function.
 *
 * BCMLTP is requested to stopped.
 *
 * This function signature is used for initialization and configuration
 * callback events by the system manager. A component should provide these
 * callback functions if it chooses to be called for the relevant event.
 *
 * \param [in]  instance_category       System manager instance category.
 * \param [in]  unit                    The device unit number.
 * \param [in]  comp_data               Pointer to the component context.
 * \param [in]  graceful                Indicates abrupt or graceful stop.
 * \param [out] blocking_comp           Blocking component.
 *
 * \return SHR_SYSM_RV_DONE on success, SHR_SYSM_RV_ERROR on error and
 *         SHR_SYSM_RV_BLOCKED when waiting on other component.
 */
static shr_sysm_rv_t
bcmltp_sysm_stop(shr_sysm_categories_t instance_category,
                 int unit,
                 void *comp_data,
                 bool graceful,
                 uint32_t *blocking_component)
{
    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMLTP shutdown function.
 *
 * BCMLTP is requested to being shutdown completely.
 *
 * \param [in]  instance_category       System manager instance category.
 * \param [in]  unit                    The device unit number.
 * \param [in]  comp_data               Pointer to the component context.
 * \param [in]  warm                    Indicates cold or warm start.
 * \param [out] blocking_comp           Blocking component.
 *
 * \return SHR_SYSM_RV_DONE on success, SHR_SYSM_RV_ERROR on error and
 *         SHR_SYSM_RV_BLOCKED when waiting on other component.
 */
static shr_sysm_rv_t
bcmltp_sysm_shutdown(shr_sysm_categories_t instance_category,
                     int unit,
                     void *comp_data,
                     bool graceful,
                     uint32_t *blocking_component)
{
    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMLTP System manager init struct.
 */
static shr_sysm_cb_func_set_t bcmltp_sysm_handler = {
    /*! Initialization. */
    .init        = bcmltp_sysm_init,

    /*! Component configuration. */
    .comp_config = bcmltp_sysm_comp_config,

    /*! Pre-configuration. */
    .pre_config  = bcmltp_sysm_pre_config,

    /*! Run. */
    .run         = bcmltp_sysm_run,

    /*! Stop. */
    .stop        = bcmltp_sysm_stop,

    /*! Shutdown. */
    .shutdown    = bcmltp_sysm_shutdown
};

/*******************************************************************************
 * Public functions
 */

/*
 * Called by system manager to start BCMLTP.
 *
 * Both generic events and unit events are used.
 */
int
bcmltp_start(void)
{
    int rv;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    do {

        rv = shr_sysm_register(BCMMGMT_LTP_COMP_ID,
                               SHR_SYSM_CAT_GENERIC,
                               &bcmltp_sysm_handler,
                               NULL);

        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_CONT(rv);
            break;
        }

        rv = shr_sysm_register(BCMMGMT_LTP_COMP_ID,
                               SHR_SYSM_CAT_UNIT,
                               &bcmltp_sysm_handler,
                               NULL);
        if (SHR_FAILURE(rv)) {
            SHR_IF_ERR_CONT(rv);
            break;
        }

    } while (0);

    SHR_FUNC_EXIT();
}
