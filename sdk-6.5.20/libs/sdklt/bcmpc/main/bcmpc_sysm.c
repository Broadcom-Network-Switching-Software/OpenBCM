/*! \file bcmpc_sysm.c
 *
 * BCMPC interface to system manager.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*******************************************************************************
 * Includes
 */
#include <sal/sal_types.h>
#include <shr/shr_error.h>
#include <shr/shr_debug.h>
#include <shr/shr_sysm.h>
#include <bcmmgmt/bcmmgmt_sysm.h>

#include <bcmpc/bcmpc_drv_internal.h>
#include <bcmpc/bcmpc_sysm.h>
#include <bcmpc/bcmpc_db_internal.h>
#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_imm_internal.h>
#include <bcmpc/bcmpc_lport_internal.h>
#include <bcmpc/bcmpc_tm.h>
#include <bcmpc/bcmpc_tm_internal.h>
#include <bcmpc/bcmpc_sec_internal.h>
#include <bcmpc/bcmpc_cfg_internal.h>
#include <bcmpc/bcmpc_pmgr_internal.h>

/*******************************************************************************
 * Defines
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_INIT

/* Warmboot indication. */
static bool warmboot;


/*******************************************************************************
 * SYSM callback functions
 */

static shr_sysm_rv_t
pc_sysm_init(shr_sysm_categories_t instance_category,
             int unit,
             void *comp_data,
             bool warm,
             uint32_t *blocking_component)
{
    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t
pc_sysm_comp_config(shr_sysm_categories_t instance_category,
                    int unit,
                    void *comp_data,
                    bool warm,
                    uint32_t *blocking_component)
{
    bcmpc_drv_t *pc_drv = NULL;

    /*
     * Component Dependencies.
     */
    if (!shr_sysm_is_comp_complete(BCMMGMT_IMM_FE_COMP_ID,
                                   instance_category,
                                   unit)) {
        *blocking_component = BCMMGMT_IMM_FE_COMP_ID;
        return SHR_SYSM_RV_BLOCKED;
    }

    /*
     * PC initialization.
     */
    if (SHR_FAILURE(bcmpc_db_imm_init(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (SHR_FAILURE(bcmpc_imm_init(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (SHR_FAILURE(bcmpc_attach(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (SHR_FAILURE(bcmpc_drv_get(unit, &pc_drv))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (pc_drv && pc_drv->imm_init) {
        /* Device-specific imm initialization. */
        if (SHR_FAILURE(pc_drv->imm_init(unit))) {
            return SHR_SYSM_RV_ERROR;
        }
    }

    if (pc_drv && pc_drv->topology_init) {
        if (SHR_FAILURE(pc_drv->topology_init(unit, warm))) {
            return SHR_SYSM_RV_ERROR;
        }
    }

    if (pc_drv && pc_drv->dev_init) {
        /* By default, download firmware before port being configured. */
        if (SHR_FAILURE(pc_drv->dev_init(unit, BCMPC_PRE_FW_LOAD, warm))) {
            return SHR_SYSM_RV_ERROR;
        }
    }
    if (pc_drv && pc_drv->aux_func_init) {
        if (SHR_FAILURE(pc_drv->aux_func_init(unit, warm))) {
            return SHR_SYSM_RV_ERROR;
        }
    }
    if (SHR_FAILURE(bcmpc_pc_tm_mutex_init(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (SHR_FAILURE(bcmpc_pc_sec_mutex_init(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (SHR_FAILURE(bcmpc_l2p_map_create(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (SHR_FAILURE(bcmpc_l2p_lock_init(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (SHR_FAILURE(bcmpc_thread_init(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (SHR_FAILURE(bcmpc_cache_init(unit, warm))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t
pc_sysm_pre_config(shr_sysm_categories_t instance_category,
                   int unit,
                   void *comp_data,
                   bool warm,
                   uint32_t *blocking_component)
{
    bcmpc_drv_t *pc_drv = NULL;

    warmboot = warm;

    /* Check if this is running on hardware platform supported. */
    if (SHR_FAILURE(bcmpc_drv_get(unit, &pc_drv))) {
        return SHR_SYSM_RV_ERROR;
    }

    /*
     * Preopulate all the update only Logical Tables
     * with null entries.
     * Tables that are populated,
     * PC_TX_TAPS
     * PC_MAC_CONTROL
     * PC_PMD_FIRMWARE
     * PC_PORT_TIMESYNC
     * PC_PHY_CONTROL
     * PC_PFC
     */
    if (!warm && pc_drv) {
        if (SHR_FAILURE(bcmpc_pm_core_prepopulate_all(unit))) {
            return SHR_SYSM_RV_ERROR;
        }

        if (SHR_FAILURE(bcmpc_mac_control_prepopulate_all(unit))) {
            return SHR_SYSM_RV_ERROR;
        }

        if (SHR_FAILURE(bcmpc_pfc_prepopulate_all(unit))) {
            return SHR_SYSM_RV_ERROR;
        }

        if (SHR_FAILURE(bcmpc_port_timesync_prepopulate_all(unit))) {
            return SHR_SYSM_RV_ERROR;
        }

        if (SHR_FAILURE(bcmpc_pmd_firmware_prepopulate_all(unit))) {
            return SHR_SYSM_RV_ERROR;
        }

        if (SHR_FAILURE(bcmpc_tx_taps_prepopulate_all(unit))) {
            return SHR_SYSM_RV_ERROR;
        }

        if (SHR_FAILURE(bcmpc_phy_control_prepopulate_all(unit))) {
            return SHR_SYSM_RV_ERROR;
        }

        if (SHR_FAILURE(bcmpc_port_monitor_prepopulate(unit))) {
            return SHR_SYSM_RV_ERROR;
        }
    }

    return SHR_SYSM_RV_DONE;
}

static void
pc_sysm_run(shr_sysm_categories_t instance_category,
            int unit,
            void *comp_data)
{
    int rv;
    bcmpc_drv_t *pc_drv = NULL;

    if (!warmboot) {
        rv = bcmpc_drv_get(unit, &pc_drv);
        if (SHR_FAILURE(rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit, "Failed to get Device Driver "
                                 "(%s)\n"), shr_errmsg(rv)));
        }
        if (pc_drv && pc_drv->port_config_apply) {
            rv = pc_drv->port_config_apply(unit);
            if (SHR_FAILURE(rv) || (rv == SHR_E_CONFIG)) {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "Port configuration error. (%s)\n"),
                                     shr_errmsg(rv)));
                return;
            }
         } else {
            rv = bcmpc_config_apply(unit);
            if (SHR_FAILURE(rv) || (rv == SHR_E_CONFIG)) {
                LOG_WARN(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "Port configuration error. (%s)\n"),
                                     shr_errmsg(rv)));
                return;
            }
        }
        /* Do TM update only once for all ports in the config file. */
        rv = bcmpc_tm_update_now(unit);
        if (SHR_FAILURE(rv)) {
            LOG_WARN(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "Failed to update port configurations to"
                                  " TM (%s)\n"),
                                  shr_errmsg(rv)));
        }
    }

    /* Enable PC component per-port based configuration mode. */
    rv = bcmpc_pc_port_based_config_set(unit, TRUE);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Failed to set PC config mode. (%s)\n"),
                  shr_errmsg(rv)));
        return;
    }

    /* Enable TM update for each port addition/deletion. */
    rv = bcmpc_tm_port_based_update_set(unit, true);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Failed to set TM update mode. (%s)\n"),
                  shr_errmsg(rv)));
    }
}

static shr_sysm_rv_t
pc_sysm_stop(shr_sysm_categories_t instance_category,
             int unit,
             void *comp_data,
             bool graceful,
             uint32_t *blocking_component)
{

    /* Set HA link state */
    if (SHR_FAILURE(bcmpc_pmgr_ha_link_state_set(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    if (SHR_FAILURE(bcmpc_thread_deinit(unit))) {
        return SHR_SYSM_RV_ERROR;
    }
    return SHR_SYSM_RV_DONE;
}

static shr_sysm_rv_t
pc_sysm_shutdown(shr_sysm_categories_t instance_category,
                 int unit,
                 void *comp_data,
                 bool graceful,
                 uint32_t *blocking_component)
{
    int rv;
    bcmpc_drv_t *pc_drv = NULL;

    rv = bcmpc_drv_get(unit, &pc_drv);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                 (BSL_META_U(unit, "Failed to get Device Driver "
                             "(%s)\n"), shr_errmsg(rv)));
    }

    if (pc_drv && pc_drv->aux_func_cleanup) {
        if (SHR_FAILURE(pc_drv->aux_func_cleanup(unit))) {
            return SHR_SYSM_RV_ERROR;
        }
    }
    if (SHR_FAILURE(bcmpc_detach(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    bcmpc_pc_tm_mutex_cleanup(unit);

    bcmpc_pc_sec_mutex_cleanup(unit);

    bcmpc_l2p_map_destroy(unit);

    bcmpc_l2p_lock_cleanup(unit);

    if (SHR_FAILURE(bcmpc_db_imm_cleanup(unit))) {
        return SHR_SYSM_RV_ERROR;
    }

    /* Disable PC component per-port based configuration mode. */
    if (SHR_FAILURE(bcmpc_pc_port_based_config_set(unit, false))) {
        return SHR_SYSM_RV_ERROR;
    }

    /* Disable TM update for each port addition/deletion. */
    if (SHR_FAILURE(bcmpc_tm_port_based_update_set(unit, false))) {
        return SHR_SYSM_RV_ERROR;
    }

    return SHR_SYSM_RV_DONE;
}

/*!
 * \brief BCMPC System manager init struct.
 */
static shr_sysm_cb_func_set_t pc_sysm_handler = {
    /*! Initialization. */
    .init        = pc_sysm_init,

    /*! Component configuration. */
    .comp_config = pc_sysm_comp_config,

    /*! Pre-configuration. */
    .pre_config  = pc_sysm_pre_config,

    /*! Run. */
    .run         = pc_sysm_run,

    /*! Stop. */
    .stop        = pc_sysm_stop,

    /*! Shutdown. */
    .shutdown    = pc_sysm_shutdown
};


/*******************************************************************************
 * Public Functions
 */

int
bcmpc_start(void)
{
    SHR_FUNC_ENTER(BSL_UNIT_UNKNOWN);

    SHR_IF_ERR_EXIT
        (shr_sysm_register(BCMMGMT_PC_COMP_ID,
                           SHR_SYSM_CAT_UNIT,
                           &pc_sysm_handler,
                           NULL));
exit:
    SHR_FUNC_EXIT();
}

