/*! \file bcmcth_ctr_evict_sysm.c
 *
 * BCMCTH EVICT interface to system manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <shr/shr_debug.h>
#include <shr/shr_sysm.h>
#include <bcmmgmt/bcmmgmt_sysm.h>

#include <bcmcth/bcmcth_ctr_evict.h>
#include <bcmcth/bcmcth_ctr_evict_sysm.h>
#include <bcmcth/bcmcth_ctr_evict_drv.h>

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMCTH_CTREVICT

static bcmmgmt_sysm_comp_id_t ctr_evict_init_comp_list[] = {
    BCMMGMT_LRD_COMP_ID,
    BCMMGMT_PTM_COMP_ID,
    BCMMGMT_IMM_FE_COMP_ID,
    BCMMGMT_MAX_COMP_ID  /* end of list indicator */
};

static bcmmgmt_sysm_comp_id_t ctr_evict_cleanup_comp_list[] = {
    BCMMGMT_LTM_COMP_ID,
    BCMMGMT_MAX_COMP_ID  /* end of list indicator */
};

static bool
bcmcth_ctr_evict_is_blocked(shr_sysm_categories_t instance_category,
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
 * \brief BCMCTH config function.
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
bcmcth_ctr_evict_unit_comp_config(shr_sysm_categories_t instance_category,
                                  int unit,
                                  void *comp_data, bool warm,
                                  uint32_t *blocking_component)
{
    if (bcmcth_ctr_evict_is_blocked(instance_category,
                                    unit,
                                    ctr_evict_init_comp_list,
                                    blocking_component)) {
        return SHR_SYSM_RV_BLOCKED;
    }

    if (SHR_FAILURE(bcmcth_ctr_evict_attach(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMCTH shutdown function.
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
bcmcth_ctr_evict_unit_shutdown(shr_sysm_categories_t instance_category,
                             int unit,
                             void *comp_data, bool warm,
                             uint32_t *blocking_component)
{
    if (bcmcth_ctr_evict_is_blocked(instance_category,
                                    unit,
                                    ctr_evict_cleanup_comp_list,
                                    blocking_component)) {
        return SHR_SYSM_RV_BLOCKED;
    }

    if (SHR_FAILURE(bcmcth_ctr_evict_detach(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief CTR EVICT System manager init struct.
 */
static shr_sysm_cb_func_set_t ctr_evict_sysm_handler = {
    /*! Initialization. */
    .init        = NULL,

    /*! Component configuration. */
    .comp_config = bcmcth_ctr_evict_unit_comp_config,

    /*! Pre-configuration. */
    .pre_config  = NULL,

    /*! Run. */
    .run         = NULL,

    /*! Stop. */
    .stop        = NULL,

    /*! Shutdown. */
    .shutdown    = bcmcth_ctr_evict_unit_shutdown
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
bcmcth_ctr_evict_start(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_CTR_EVICT_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &ctr_evict_sysm_handler,
                           NULL));

exit:
    SHR_FUNC_EXIT();
}
