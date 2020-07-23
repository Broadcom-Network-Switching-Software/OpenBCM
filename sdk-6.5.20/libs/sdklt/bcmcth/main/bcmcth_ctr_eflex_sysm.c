/*! \file bcmcth_ctr_eflex_sysm.c
 *
 * BCMCTH EFLEX interface to system manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_sysm.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmevm/bcmevm_api.h>

#include <bcmcth/bcmcth_ctr_eflex.h>
#include <bcmcth/bcmcth_ctr_eflex_sysm.h>
#include <bcmcth/bcmcth_ctr_eflex_drv.h>
#include <bcmcth/bcmcth_state_eflex_drv.h>
#include <bcmcth/bcmcth_ctr_eflex_internal.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREFLEX

static bcmmgmt_sysm_comp_id_t ctr_eflex_init_comp_list[] = {
    BCMMGMT_LRD_COMP_ID,
    BCMMGMT_PTM_COMP_ID,
    BCMMGMT_IMM_FE_COMP_ID,
    BCMMGMT_RM_HASH_COMP_ID,
    BCMMGMT_RM_TCAM_COMP_ID,
    BCMMGMT_RM_ALPM_COMP_ID,
    BCMMGMT_MAX_COMP_ID  /* end of list indicator */
};

static bcmmgmt_sysm_comp_id_t ctr_eflex_cleanup_comp_list[] = {
    BCMMGMT_LTM_COMP_ID,
    BCMMGMT_MAX_COMP_ID  /* end of list indicator */
};

static bool
bcmcth_ctr_eflex_is_blocked(shr_sysm_categories_t instance_category,
                            int unit,
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

/*******************************************************************************
 * SYSM callback functions
 */

/*!
 * \brief CTR EFLEX BCMCTH config function.
 *
 * BCMCTH is requested to configure.
 *
 * \param [in] instance_category System manager instance category.
 * \param [in] unit Unit number.
 * \param [in] comp_data Pointer to the component context.
 * \param [in] warm Indicates cold or warm start.
 * \param [out] blocking_component Blocking component.
 *
 * \retval SHR_SYSM_RV_DONE Success.
 * \retval SHR_SYSM_RV_ERROR On Failure.
 */
static shr_sysm_rv_t
bcmcth_ctr_eflex_unit_comp_config(shr_sysm_categories_t instance_category,
                                  int unit,
                                  void *comp_data, bool warm,
                                  uint32_t *blocking_component)
{
    if (bcmcth_ctr_eflex_is_blocked(instance_category,
                                    unit,
                                    ctr_eflex_init_comp_list,
                                    blocking_component)) {
        return SHR_SYSM_RV_BLOCKED;
    }

    if (SHR_FAILURE(bcmcth_ctr_eflex_attach(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (!warm) {
        if (SHR_FAILURE(bcmcth_ctr_eflex_bitp_profile_set(unit))) {
            return SHR_SYSM_RV_ERROR;
        }

        if (SHR_FAILURE(bcmcth_ctr_eflex_timestamp_init(unit))) {
            return SHR_SYSM_RV_ERROR;
        }
    }

    /* Register callback function to handle RM hash entry move. */
    if (SHR_FAILURE(bcmevm_register_published_event(
                        unit,
                        BCMPTM_LT_ENTRY_MOVE,
                        bcmcth_ctr_eflex_hash_move_event_cb))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief CTR EFLEX BCMCTH stop function.
 *
 * BCMCTH is requested to stop all threads as configured.
 *
 * \param [in] instance_category System manager instance category.
 * \param [in] unit Unit number.
 * \param [in] comp_data Pointer to the component context.
 * \param [in] graceful Stop should be performed gracefully or not.
 * \param [out] blocking_component Blocking component.
 *
 * \return SHR_SYSM_RV_DONE on success, SHR_SYSM_RV_ERROR on error and
 *         SHR_SYSM_RV_BLOCKED when waiting on other component.
 */
static shr_sysm_rv_t
bcmcth_ctr_eflex_unit_stop(shr_sysm_categories_t instance_category,
                           int unit,
                           void *comp_data,
                           bool graceful,
                           uint32_t *blocking_component)
{
    /* Unregister callback function to handle RM hash entry move. */
    if (SHR_FAILURE(bcmevm_unregister_published_event(
                        unit,
                        BCMPTM_LT_ENTRY_MOVE,
                        bcmcth_ctr_eflex_hash_move_event_cb))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief CTR EFLEX BCMCTH shutdown function.
 *
 * BCMCTH is requested to being shutdown completely.
 *
 * \param [in] instance_category System manager instance category.
 * \param [in] unit Unit number.
 * \param [in] comp_data Pointer to the component context.
 * \param [in] warm Indicates cold or warm start.
 * \param [out] blocking_component Blocking component.
 *
 * \retval SHR_SYSM_RV_DONE Success.
 * \retval SHR_SYSM_RV_ERROR On Failure.
 */
static shr_sysm_rv_t
bcmcth_ctr_eflex_unit_shutdown(shr_sysm_categories_t instance_category,
                             int unit,
                             void *comp_data, bool warm,
                             uint32_t *blocking_component)
{
    if (bcmcth_ctr_eflex_is_blocked(instance_category,
                                    unit,
                                    ctr_eflex_cleanup_comp_list,
                                    blocking_component)) {
        return SHR_SYSM_RV_BLOCKED;
    }

    if (SHR_FAILURE(bcmcth_ctr_eflex_detach(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief STATE EFLEX BCMCTH config function.
 *
 * BCMCTH is requested to configure.
 *
 * \param [in] instance_category System manager instance category.
 * \param [in] unit Unit number.
 * \param [in] comp_data Pointer to the component context.
 * \param [in] warm Indicates cold or warm start.
 * \param [out] blocking_component Blocking component.
 *
 * \retval SHR_SYSM_RV_DONE Success.
 * \retval SHR_SYSM_RV_ERROR On Failure.
 */
static shr_sysm_rv_t
bcmcth_state_eflex_unit_comp_config(shr_sysm_categories_t instance_category,
                                    int unit,
                                    void *comp_data, bool warm,
                                    uint32_t *blocking_component)
{
    if (bcmcth_ctr_eflex_is_blocked(instance_category,
                                    unit,
                                    ctr_eflex_init_comp_list,
                                    blocking_component)) {
        return SHR_SYSM_RV_BLOCKED;
    }

    if (SHR_FAILURE(bcmcth_state_eflex_attach(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (!warm) {
        if (SHR_FAILURE(bcmcth_state_eflex_bitp_profile_set(unit))) {
            return SHR_SYSM_RV_ERROR;
        }

        if (SHR_FAILURE(bcmcth_state_eflex_timestamp_init(unit))) {
            return SHR_SYSM_RV_ERROR;
        }
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief STATE EFLEX BCMCTH shutdown function.
 *
 * BCMCTH is requested to being shutdown completely.
 *
 * \param [in] instance_category System manager instance category.
 * \param [in] unit Unit number.
 * \param [in] comp_data Pointer to the component context.
 * \param [in] warm Indicates cold or warm start.
 * \param [out] blocking_component Blocking component.
 *
 * \retval SHR_SYSM_RV_DONE Success.
 * \retval SHR_SYSM_RV_ERROR On Failure.
 */
static shr_sysm_rv_t
bcmcth_state_eflex_unit_shutdown(shr_sysm_categories_t instance_category,
                                 int unit,
                                 void *comp_data, bool warm,
                                 uint32_t *blocking_component)
{
    if (bcmcth_ctr_eflex_is_blocked(instance_category,
                                    unit,
                                    ctr_eflex_cleanup_comp_list,
                                    blocking_component)) {
        return SHR_SYSM_RV_BLOCKED;
    }

    if (SHR_FAILURE(bcmcth_state_eflex_detach(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief CTR EFLEX System manager init struct.
 */
static shr_sysm_cb_func_set_t ctr_eflex_sysm_handler = {
    /*! Initialization. */
    .init        = NULL,

    /*! Component configuration. */
    .comp_config = bcmcth_ctr_eflex_unit_comp_config,

    /*! Pre-configuration. */
    .pre_config  = NULL,

    /*! Run. */
    .run         = NULL,

    /*! Stop. */
    .stop        = bcmcth_ctr_eflex_unit_stop,

    /*! Shutdown. */
    .shutdown    = bcmcth_ctr_eflex_unit_shutdown
};

/*!
 * \brief STATE EFLEX System manager init struct.
 */
static shr_sysm_cb_func_set_t state_eflex_sysm_handler = {
    /*! Initialization. */
    .init        = NULL,

    /*! Component configuration. */
    .comp_config = bcmcth_state_eflex_unit_comp_config,

    /*! Pre-configuration. */
    .pre_config  = NULL,

    /*! Run. */
    .run         = NULL,

    /*! Stop. */
    .stop        = NULL,

    /*! Shutdown. */
    .shutdown    = bcmcth_state_eflex_unit_shutdown
};

/*******************************************************************************
 * Public Functions
 */
/*!
 * \brief Start CTR EFLEX CTH component.
 *
 * Register CTR EFLEX CTH component with System Manager.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_PARAM  Component not recognized.
 * \retval SHR_E_EXISTS Component already registered.
 */
int
bcmcth_ctr_eflex_start(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_CTR_EFLEX_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &ctr_eflex_sysm_handler,
                           NULL));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Start STATE EFLEX CTH component.
 *
 * Register STATE EFLEX CTH component with System Manager.
 *
 * \retval SHR_E_NONE   No errors.
 * \retval SHR_E_PARAM  Component not recognized.
 * \retval SHR_E_EXISTS Component already registered.
 */
int
bcmcth_state_eflex_start(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_STATE_EFLEX_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &state_eflex_sysm_handler,
                           NULL));

exit:
    SHR_FUNC_EXIT();
}
