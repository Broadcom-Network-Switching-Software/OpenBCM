/*! \file bcmfp_sysm.c
 *
 * BCM Field Processor - System Manager Integration
 *
 * This file contains the BCMFP implementation for the integration
 * to the System Manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_types.h>
#include <shr/shr_debug.h>
#include <shr/shr_error.h>
#include <shr/shr_sysm.h>
#include <bcmmgmt/bcmmgmt_sysm.h>
#include <bcmptm/bcmptm_internal.h>
#include <bcmevm/bcmevm_api.h>
#include <bcmfp/bcmfp_sysm.h>
#include <bcmfp/bcmfp_internal.h>
#include <bcmfp/bcmfp_cth_meter.h>
#include <bcmfp/bcmfp_cth_info.h>

#define BSL_LOG_MODULE BSL_LS_BCMFP_INIT

/*!
 * \brief BCMFP initialization function.
 * BCMFP is requested to initialize.
 * Expect Devices will be attached, BD, DRD, LRD will
 * happen here. We can do config-independent init here
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
bcmfp_sysm_init(shr_sysm_categories_t instance_category,
                int unit,
                void *comp_data,
                bool warm,
                uint32_t *blocking_component)
{
    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMFP shutdown function.
 *
 * BCMFP is requested to being shutdown completely.
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
bcmfp_sysm_shutdown(shr_sysm_categories_t instance_category,
                    int unit,
                    void *comp_data,
                    bool graceful,
                    uint32_t *blocking_component)
{
    if (bcmfp_cleanup(unit, graceful) != SHR_E_NONE) {
        return SHR_SYSM_RV_ERROR;
    }
    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMFP config function.
 *
 * BCMFP is requested to configure.
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
bcmfp_sysm_comp_config(shr_sysm_categories_t instance_category,
                       int unit,
                       void *comp_data,
                       bool warm,
                       uint32_t *blocking_component)
{
    if (!shr_sysm_is_comp_complete(BCMMGMT_IMM_FE_COMP_ID,
                                   instance_category, unit)) {
        *blocking_component = BCMMGMT_IMM_FE_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_ISSU_COMP_ID,
                                   instance_category, unit)) {
        *blocking_component = BCMMGMT_ISSU_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_CFG_COMP_ID,
                                   instance_category, unit)) {
        *blocking_component = BCMMGMT_CFG_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_EVM_COMP_ID,
                                   instance_category, unit)) {
        *blocking_component = BCMMGMT_EVM_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_UFT_BE_COMP_ID,
                                   instance_category, unit)) {
        *blocking_component = BCMMGMT_UFT_BE_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (!shr_sysm_is_comp_complete(BCMMGMT_UFT_COMP_ID,
                                   instance_category, unit)) {
        *blocking_component = BCMMGMT_UFT_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    if (bcmfp_init(unit, warm) != SHR_E_NONE) {
        return SHR_SYSM_RV_ERROR;
    }
    if (bcmfp_imm_reg(unit) != SHR_E_NONE) {
        return SHR_SYSM_RV_ERROR;
    }

    
    if (bcmevm_register_published_event(unit,
                                        "METER_ING_INSERT",
                                        bcmfp_meter_event_cb)) {
        return SHR_SYSM_RV_ERROR;
    }

    if (bcmevm_register_published_event(unit,
                                        "METER_ING_UPDATE",
                                        bcmfp_meter_event_cb)) {
        return SHR_SYSM_RV_ERROR;
    }

    if (bcmevm_register_published_event(unit,
                                        "METER_ING_DELETE",
                                        bcmfp_meter_event_cb)) {
        return SHR_SYSM_RV_ERROR;
    }

    if (bcmevm_register_published_event(unit,
                                        "METER_EGR_INSERT",
                                        bcmfp_meter_event_cb)) {
        return SHR_SYSM_RV_ERROR;
    }

    if (bcmevm_register_published_event(unit,
                                        "METER_EGR_UPDATE",
                                        bcmfp_meter_event_cb)) {
        return SHR_SYSM_RV_ERROR;
    }

    if (bcmevm_register_published_event(unit,
                                        "METER_EGR_DELETE",
                                        bcmfp_meter_event_cb)) {
        return SHR_SYSM_RV_ERROR;
    }

    if (bcmevm_register_published_event(unit,
                                        "FP_INFO_LOOKUP",
                                        bcmfp_info_event_cb)) {
        return SHR_SYSM_RV_ERROR;
    }

    /*
     * Register handler to handle bank count, bank offset and
     * robust configuration change.
     */
    if (bcmevm_register_published_event(unit,
                                        BCMPTM_EVENT_UFT_GROUP_CHANGE_COMMIT,
                                        bcmfp_em_grp_change_event_cb)) {
        return SHR_SYSM_RV_ERROR;
    }

    if (bcmevm_register_published_event(unit,
                                        BCMPTM_EVENT_UFT_GROUP_CHANGE_COMMIT,
                                        bcmfp_em_ft_grp_change_event_cb)) {
        return SHR_SYSM_RV_ERROR;
    }

    if (bcmevm_register_published_event(unit,
                                        BCMPTM_EVENT_UFT_GROUP_CHANGE_COMMIT,
                                        bcmfp_vlan_grp_change_event_cb)) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMFP pre_config function.
 *
 * BCMFP is requested to pre-configure.Expect Cfg writes.
 * Unit-dependent config will happen with LT writes
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
bcmfp_sysm_pre_config(shr_sysm_categories_t instance_category,
                      int unit,
                      void *comp_data,
                      bool warm,
                      uint32_t *blocking_component)
{
    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMFP run function.
 *
 * BCMFP is requested to start all threads as configured.
 *
 * \param [in] instance_category System manager instance category.
 * \param [in] unit Unit number.
 * \param [in] comp_data Pointer to the component context.
 *
 * \return SHR_SYSM_RV_DONE on success, SHR_SYSM_RV_ERROR on error and
 *         SHR_SYSM_RV_BLOCKED when waiting on other component.
 */
static void
bcmfp_sysm_run(shr_sysm_categories_t instance_category,
               int unit,
               void *comp_data)
{
}

/* Phase: stop
 */
/*!
 * \brief BCMFP stop function.
 *
 * BCMFP is requested to stop all threads as configured.
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
bcmfp_sysm_stop(shr_sysm_categories_t instance_category,
                int unit,
                void *comp_data,
                bool graceful,
                uint32_t *blocking_component)
{
    
    if (bcmevm_unregister_published_event(unit,
                                          "METER_ING_INSERT",
                                          bcmfp_meter_event_cb)) {
        return SHR_SYSM_RV_ERROR;
    }

    if (bcmevm_unregister_published_event(unit,
                                          "METER_ING_UPDATE",
                                          bcmfp_meter_event_cb)) {
        return SHR_SYSM_RV_ERROR;
    }

    if (bcmevm_unregister_published_event(unit,
                                          "METER_ING_DELETE",
                                          bcmfp_meter_event_cb)) {
        return SHR_SYSM_RV_ERROR;
    }

    if (bcmevm_unregister_published_event(unit,
                                          "METER_EGR_INSERT",
                                          bcmfp_meter_event_cb)) {
        return SHR_SYSM_RV_ERROR;
    }

    if (bcmevm_unregister_published_event(unit,
                                          "METER_EGR_UPDATE",
                                          bcmfp_meter_event_cb)) {
        return SHR_SYSM_RV_ERROR;
    }

    if (bcmevm_unregister_published_event(unit,
                                          "METER_EGR_DELETE",
                                          bcmfp_meter_event_cb)) {
        return SHR_SYSM_RV_ERROR;
    }

    if (bcmevm_unregister_published_event(unit,
                                          BCMPTM_EVENT_UFT_GROUP_CHANGE_COMMIT,
                                          bcmfp_em_grp_change_event_cb)) {
        return SHR_SYSM_RV_ERROR;
    }

    if (bcmevm_unregister_published_event(unit,
                                          BCMPTM_EVENT_UFT_GROUP_CHANGE_COMMIT,
                                          bcmfp_em_ft_grp_change_event_cb)) {
        return SHR_SYSM_RV_ERROR;
    }

    if (bcmevm_unregister_published_event(unit,
                                          BCMPTM_EVENT_UFT_GROUP_CHANGE_COMMIT,
                                          bcmfp_vlan_grp_change_event_cb)) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMFP System manager init struct.
 */
static shr_sysm_cb_func_set_t bcmfp_sysm_handler = {
    /*! Initialization. */
    .init        = bcmfp_sysm_init,

    /*! Component configuration. */
    .comp_config = bcmfp_sysm_comp_config,

    /*! Pre-configuration. */
    .pre_config  = bcmfp_sysm_pre_config,

    /*! Run. */
    .run         = bcmfp_sysm_run,

    /*! Stop. */
    .stop        = bcmfp_sysm_stop,

    /*! Shutdown. */
    .shutdown    = bcmfp_sysm_shutdown
};

int
bcmfp_start(void)
{

    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    /* Register unit-independent functions */
    /* None */

    /* Register unit-specific functions */
    SHR_IF_ERR_EXIT(shr_sysm_register(BCMMGMT_FP_COMP_ID,
                                      SHR_SYSM_CAT_UNIT,
                                      &bcmfp_sysm_handler,
                                      NULL));
exit:
    SHR_FUNC_EXIT();
}
