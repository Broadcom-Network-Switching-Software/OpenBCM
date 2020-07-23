/*! \file bcmltm_sysm.c
 *
 * Logical Table Manager - System Manager Integration
 *
 * This file contains the LTM implementation for the integration
 * to the System Manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_types.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <shr/shr_sysm.h>

#include <bsl/bsl.h>

#include <bcmdrd_config.h>
#include <bcmmgmt/bcmmgmt_sysm.h>

#include <bcmlrd/bcmlrd_table.h>

#include <bcmptm/bcmptm.h>

#include <bcmltm/bcmltm.h>
#include <bcmltm/bcmltm_table_op_pt_info.h>
#include <bcmltm/bcmltm_state_internal.h>
#include <bcmltm/bcmltm_sysm.h>


/*******************************************************************************
 * Local definitions
 */

/* BSL Module */
#define BSL_LOG_MODULE BSL_LS_BCMLTM_INIT

static bool initialized[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

/*!
 * \brief LTM callbacks within PTM registration function.
 *
 * Register all of the LTM function callbacks supplied to the PTM
 * to permit CTH wrapper functions that protect LTM internal state.
 *
 * \param [in] unit Unit number.
 *
 * \return None
 */
static void
ltm_sysm_ptm_callbacks_register(int unit)
{
    bcmptm_lt_stat_increment_register(unit, bcmltm_stats_increment_lt);
    bcmlrd_table_inuse_count_get_register(unit, bcmltm_inuse_count_get);

    bcmptm_pt_status_mreq_keyed_lt_register(unit,
                                   bcmltm_pt_status_mreq_keyed_lt);

    bcmptm_pt_status_mreq_indexed_lt_register(unit,
                                     bcmltm_pt_status_mreq_indexed_lt);

    bcmptm_pt_status_ireq_hash_lt_register(unit,
                                  bcmltm_pt_status_ireq_hash_lt);

    bcmptm_pt_status_ireq_op_register(unit,
                                      bcmltm_pt_status_ireq_op);
}

/*!
 * \brief LTM callbacks within PTM unregistration function.
 *
 * Unregister all of the LTM function callbacks supplied to the PTM
 * to permit CTH wrapper functions that protect LTM internal state.
 * The LTM may cleanup its internal state safely after these callbacks
 * are been disabled.
 *
 * \param [in] unit Unit number.
 *
 * \return None
 */
static void
ltm_sysm_ptm_callbacks_unregister(int unit)
{
    bcmptm_lt_stat_increment_register(unit, NULL);

    bcmptm_pt_status_mreq_keyed_lt_register(unit, NULL);

    bcmptm_pt_status_mreq_indexed_lt_register(unit, NULL);

    bcmptm_pt_status_ireq_hash_lt_register(unit, NULL);

    bcmptm_pt_status_ireq_op_register(unit, NULL);
}

/*!
 * \brief LTM initialization function.
 *
 * LTM is requested to initialize.
 *
 * \param [in] instance_category System manager instance category.
 * \param [in] unit Unit number.
 * \param [in] comp_data Pointer to the component context.
 * \param [in] warm Indicates cold or warm start.
 * \param [out] blocking_component Blocking component.
 *
 * \return SHR_SYSM_RV_DONE on success, SHR_SYSM_RV_ERROR on error and
 *         SHR_SYSM_RV_BLOCKED when waiting on other component.
 */
static shr_sysm_rv_t
ltm_sysm_init(shr_sysm_categories_t instance_category,
              int unit,
              void *comp_data,
              bool warm,
              uint32_t *blocking_component)
{
    /* Nothing to do for LTM in this phase. Wait until component config. */

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief LTM config function.
 *
 * LTM is requested to configure.
 *
 * \param [in] instance_category System manager instance category.
 * \param [in] unit Unit number.
 * \param [in] comp_data Pointer to the component context.
 * \param [in] warm Indicates cold or warm start.
 * \param [out] blocking_component Blocking component.
 *
 * \return SHR_SYSM_RV_DONE on success, SHR_SYSM_RV_ERROR on error and
 *         SHR_SYSM_RV_BLOCKED when waiting on other component.
 */
static shr_sysm_rv_t
ltm_sysm_comp_config(shr_sysm_categories_t instance_category,
                     int unit,
                     void *comp_data,
                     bool warm,
                     uint32_t *blocking_component)
{
    /* Check dependencies */

    /*
     * Although LTM currently does not need to do anything in this state,
     * it checks that relevant dependent components have completed
     * this state in order to report the correct status to other components
     * that need to take actions.
     */
    if (!shr_sysm_is_comp_complete(BCMMGMT_PTM_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_PTM_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_RM_HASH_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_PTM_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_RM_TCAM_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_PTM_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_RM_ALPM_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_PTM_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!initialized[unit]) {
        if (SHR_FAILURE(bcmltm_init(unit, warm))) {
            return SHR_SYSM_RV_ERROR;
        }
        initialized[unit] = true;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_DI_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_DI_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_SER_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_SER_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief LTM pre_config function.
 *
 * LTM is requested to pre-configure.
 *
 * \param [in] instance_category System manager instance category.
 * \param [in] unit Unit number.
 * \param [in] comp_data Pointer to the component context.
 * \param [in] warm Indicates cold or warm start.
 * \param [out] blocking_component Blocking component.
 *
 * \return SHR_SYSM_RV_DONE on success, SHR_SYSM_RV_ERROR on error and
 *         SHR_SYSM_RV_BLOCKED when waiting on other component.
 */
static shr_sysm_rv_t
ltm_sysm_pre_config(shr_sysm_categories_t instance_category,
                    int unit,
                    void *comp_data,
                    bool warm,
                    uint32_t *blocking_component)
{
    /* Check dependencies */

    if (!shr_sysm_is_comp_complete(BCMMGMT_PTM_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_PTM_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_DI_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_DI_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_SER_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_SER_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    /* Turn on the PTM callbacks once the LTM init of LT state
     * is completed.
     */
    ltm_sysm_ptm_callbacks_register(unit);

    if (SHR_FAILURE(bcmltm_dev_ptstat_init(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }

    

    return SHR_SYSM_RV_DONE;
}


/*!
 * \brief LTM shutdown function.
 *
 * LTM is requested to being shutdown completely.
 *
 * \param [in] instance_category System manager instance category.
 * \param [in] unit Unit number.
 * \param [in] comp_data Pointer to the component context.
 * \param [in] warm Indicates cold or warm start.
 * \param [out] blocking_component Blocking component.
 *
 * \return SHR_SYSM_RV_DONE on success, SHR_SYSM_RV_ERROR on error and
 *         SHR_SYSM_RV_BLOCKED when waiting on other component.
 */
static shr_sysm_rv_t
ltm_sysm_shutdown(shr_sysm_categories_t instance_category,
                  int unit,
                  void *comp_data,
                  bool graceful,
                  uint32_t *blocking_component)
{
    /* Turn off the PTM callbacks before LT state is discarded. */
    ltm_sysm_ptm_callbacks_unregister(unit);

    if (SHR_FAILURE(bcmltm_cleanup(unit))) {
        return SHR_SYSM_RV_ERROR;
    }
    initialized[unit] = false;

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief LTM System manager init struct.
 */
static shr_sysm_cb_func_set_t ltm_sysm_handler = {
    /*! Initialization. */
    .init        = ltm_sysm_init,

    /*! Component configuration. */
    .comp_config = ltm_sysm_comp_config,

    /*! Pre-configuration. */
    .pre_config  = ltm_sysm_pre_config,

    /*! Run. */
    .run         = NULL,

    /*! Stop. */
    .stop        = NULL,

    /*! Shutdown. */
    .shutdown    = ltm_sysm_shutdown
};

/*******************************************************************************
 * Public functions
 */

/*
 * Called by System Manager to start LTM.
 */
int
bcmltm_start(void)
{
    int j;

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    for (j = 0; j < BCMDRD_CONFIG_MAX_UNITS; j++) {
        initialized[j] = false;
    }

    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_LTM_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &ltm_sysm_handler,
                           NULL));

 exit:
    SHR_FUNC_EXIT();
}
