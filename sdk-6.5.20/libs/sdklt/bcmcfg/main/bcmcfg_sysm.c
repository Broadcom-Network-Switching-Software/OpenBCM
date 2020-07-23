/*! \file bcmcfg_sysm.c
 *
 * BCMCFG system manager handlers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_sysm.h>

#include <bcmdrd/bcmdrd_types.h>

#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmltd/bcmltd_types.h>

#include <bcmcfg/bcmcfg.h>
#include <bcmcfg/bcmcfg_reader.h>
#include <bcmcfg/bcmcfg_sysm.h>
#include <bcmcfg/bcmcfg_init.h>
#include <bcmcfg/bcmcfg_internal.h>
#include <bcmcfg/bcmcfg_ha_internal.h>

/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMCFG_INIT

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief BCMCFG initialization function.
 *
 * BCMCFG is requested to initialize.
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
bcmcfg_sysm_init(shr_sysm_categories_t instance_category,
                 int unit,
                 void *comp_data,
                 bool warm,
                 uint32_t *blocking_component)
{
    int rv;

    rv = bcmcfg_init(unit, warm);
    if (SHR_FAILURE(rv)) {
        return SHR_SYSM_RV_ERROR;
    }

    rv = bcmcfg_ha_variant_config_init(unit, warm);
    if (SHR_FAILURE(rv)) {
        return SHR_SYSM_RV_ERROR;
    }

    if (warm) {
        rv = bcmcfg_ha_variant_config_restore(unit, warm);
    } else {
        rv = bcmcfg_ha_variant_config_save(unit, warm);
    }
    if (SHR_FAILURE(rv)) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMCFG config function.
 *
 * BCMCFG is requested to configure.
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
bcmcfg_sysm_comp_config(shr_sysm_categories_t instance_category,
                        int unit,
                        void *comp_data,
                        bool warm,
                        uint32_t *blocking_component)
{
    int rv;

    if (!shr_sysm_is_comp_complete(BCMMGMT_ISSU_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_ISSU_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    rv = bcmcfg_ha_data_init(unit, warm);
    if (SHR_FAILURE(rv)) {
        return SHR_SYSM_RV_ERROR;
    }

    /*
     * Conditional map editing should be performed after
     * saving/restoring HA data.
     */
    if (warm) {
        /* Restore HA data. */
        rv = bcmcfg_ha_data_restore(unit, warm);
    } else {
        /* Save HA data. */
        rv = bcmcfg_ha_data_save(unit, warm);
    }
    if (SHR_FAILURE(rv)) {
        return SHR_SYSM_RV_ERROR;
    }

    /* Load configuration */
    rv = bcmcfg_config(unit, warm);
    if (SHR_FAILURE(rv)) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMCFG shutdown function.
 *
 * BCMCFG is requested to being shutdown completely.
 *
 * \param [in]  instance_category       System manager instance category.
 * \param [in]  unit                    The device unit number.
 * \param [in]  comp_data               Pointer to the component context.
 * \param [in]  cold                    Indicates cold or warm start.
 * \param [out] blocking_comp           Blocking component.
 *
 * \return SHR_SYSM_RV_DONE on success, SHR_SYSM_RV_ERROR on error and
 *         SHR_SYSM_RV_BLOCKED when waiting on other component.
 */
static shr_sysm_rv_t
bcmcfg_sysm_shutdown(shr_sysm_categories_t instance_category,
                     int unit,
                     void *comp_data,
                     bool graceful,
                     uint32_t *blocking_component)
{
    int rv;

    rv = bcmcfg_shutdown(unit);
    if (SHR_FAILURE(rv)) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMCFG generic initialization function.
 *
 * BCMCFG is requested to initialize.
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
bcmcfg_sysm_generic_init(shr_sysm_categories_t instance_category,
                         int unit,
                         void *comp_data,
                         bool warm,
                         uint32_t *blocking_component)
{
    int rv;
    int iunit;

    if (!shr_sysm_is_comp_complete(BCMMGMT_ISSU_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_ISSU_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    /* Clean up any early init to avoid leaks */
    for (iunit = 0; iunit < BCMDRD_CONFIG_MAX_UNITS; iunit++) {
        if (bcmcfg_ready(iunit)) {
            (void)bcmcfg_shutdown(iunit);
        }
    }

    rv = bcmcfg_read_gen_init();
    if (SHR_FAILURE(rv)) {
        return SHR_SYSM_RV_ERROR;
    }

    rv = bcmcfg_ha_read_data_init(warm);
    if (SHR_FAILURE(rv)) {
        return SHR_SYSM_RV_ERROR;
    }

    if (warm) {
        /* Restore HA data. */
        rv = bcmcfg_ha_read_data_restore(warm);
    } else {
        /* Save HA data. */
        rv = bcmcfg_ha_read_data_save(warm);
    }
    if (SHR_FAILURE(rv)) {
        return SHR_SYSM_RV_ERROR;
    }

    rv = bcmcfg_ha_generic_init(warm);
    if (SHR_FAILURE(rv)) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMCFG generic shutdown function.
 *
 * BCMCFG is requested to being shutdown completely.
 *
 * \param [in]  instance_category       System manager instance category.
 * \param [in]  unit                    The device unit number.
 * \param [in]  comp_data               Pointer to the component context.
 * \param [in]  cold                    Indicates cold or warm start.
 * \param [out] blocking_comp           Blocking component.
 *
 * \return SHR_SYSM_RV_DONE on success, SHR_SYSM_RV_ERROR on error and
 *         SHR_SYSM_RV_BLOCKED when waiting on other component.
 */
static shr_sysm_rv_t
bcmcfg_sysm_generic_shutdown(shr_sysm_categories_t instance_category,
                             int unit,
                             void *comp_data,
                             bool graceful,
                             uint32_t *blocking_component)
{
    int rv;
    int iunit;

    rv = bcmcfg_read_gen_cleanup();
    if (SHR_FAILURE(rv)) {
        return SHR_SYSM_RV_ERROR;
    }

    for (iunit = 0; iunit < BCMDRD_CONFIG_MAX_UNITS; iunit++) {
        rv = bcmcfg_config_table_init(iunit, 0);
        if (SHR_FAILURE(rv)) {
            return SHR_SYSM_RV_ERROR;
        }
    }

    bcmcfg_unit_conf_cleanup();

    rv = bcmcfg_ha_generic_deinit();
    if (SHR_FAILURE(rv)) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMCFG System manager generic init struct.
 */
static shr_sysm_cb_func_set_t bcmcfg_sysm_generic_handler = {
    .init        = bcmcfg_sysm_generic_init,
    .shutdown    = bcmcfg_sysm_generic_shutdown
};

/*!
 * \brief BCMCFG System manager unit init struct.
 */
static shr_sysm_cb_func_set_t bcmcfg_sysm_handler = {
    .init        = bcmcfg_sysm_init,
    .comp_config = bcmcfg_sysm_comp_config,
    .shutdown    = bcmcfg_sysm_shutdown
};

/*******************************************************************************
 * Public functions
 */

/*
 * Called by system manager to start BCMCFG.
 */
int
bcmcfg_start(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);
    SHR_IF_ERR_CONT
        (shr_sysm_register(BCMMGMT_CFG_COMP_ID,
                           SHR_SYSM_CAT_GENERIC,
                           &bcmcfg_sysm_generic_handler,
                           NULL));

    SHR_IF_ERR_CONT
        (shr_sysm_register(BCMMGMT_CFG_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &bcmcfg_sysm_handler,
                           NULL));
    SHR_FUNC_EXIT();
}
