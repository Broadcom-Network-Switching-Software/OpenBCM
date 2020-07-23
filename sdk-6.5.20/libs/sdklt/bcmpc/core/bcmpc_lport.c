/*! \file bcmpc_lport.c
 *
 * BCMPC LPORT APIs
 *
 * The logical port library provides the functions to create switch logical
 * ports and associate the logical port number with a physical device port
 * number.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <sal/sal_libc.h>
#include <sal/sal_types.h>
#include <sal/sal_assert.h>
#include <sal/sal_rwlock.h>
#include <shr/shr_debug.h>
#include <shr/shr_bitop.h>
#include <shr/shr_util.h>
#include <bcmevm/bcmevm_api.h>

#include <bcmpc/bcmpc_lport.h>
#include <bcmpc/bcmpc_lport_internal.h>
#include <bcmpc/bcmpc_pm_internal.h>
#include <bcmpc/bcmpc_pmgr.h>
#include <bcmpc/bcmpc_pmgr_internal.h>
#include <bcmpc/bcmpc_pmgr_drv_internal.h>
#include <bcmpc/bcmpc_pm.h>
#include <bcmpc/bcmpc_tm_internal.h>
#include <bcmdrd/bcmdrd_dev.h>


/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_LPORT
#define PC_PORT_SCAN_INTERVAL 1000000

bcmpc_thread_ctrl_t *bcmpc_pm_thread_ctrl[BCMDRD_CONFIG_MAX_UNITS];


/*!
 * \brief AUTONEG profile entry.
 *
 * This structure is the context for the traverse function
 * port_autoneg_hw_update(), which will be invoked by
 * bcmpc_autoneg_profile_internal_update().
 */
typedef struct pc_autoneg_prof_entry_s {

    /*! Profile ID. */
    uint32_t prof_id;

    /*! PC_AUTONEG profile content. */
    bcmpc_autoneg_prof_t *prof;

} pc_autoneg_prof_entry_t;

/*!
 * \brief Physical port range.
 *
 * This stucture is the context for the traverse function pc_port_activate()
 * which will be invoked by bcmpc_ports_activate().
 */
typedef struct pc_pport_range_s {

    /*! The start physical port number. */
    bcmpc_pport_t start;

    /*! The end physical port number. */
    bcmpc_pport_t end;

} pc_pport_range_t;

/*
 * Physical to logical port mapping.
 *
 * This structure is used to quickly retrieve the logical port associated with a
 * physical port.
 *
 * The reason for caching this information is to make the conversion be more
 * efficient without having to traverse the PC_PORT table or validate the port
 * configuration every time when we want to do the P2L port conversion.
 *
 * The logical port number will be set in the array only when the port
 * configuration is valid.
 */
static bcmpc_lport_t
        p2l_map[BCMPC_NUM_UNITS_MAX][BCMPC_NUM_PPORTS_PER_CHIP_MAX];

/*
 * Read/Write lock to access Logical to Physical port mapping.
 */
static sal_rwlock_t l2p_lock[BCMPC_NUM_UNITS_MAX];

/*
 * Logical to Physical port mapping.
 *
 * This structure is used to quickly retrieve the physical port associated with
 * a logical port.
 *
 * The reason for caching this information is to make the conversion be more
 * efficient without having to lookup PC_PORT_PHYS_MAP or validate the port
 * configuration for every L2P conversion.
 *
 * The physical port number will be set in the array only when the port
 * configuration is valid.
 */
static bcmpc_lport_t *l2p_map[BCMPC_NUM_UNITS_MAX];

/* PBMP for physical ports in use */
static SHR_BITDCLNAME(occupied_ppbmp[BCMPC_NUM_UNITS_MAX], \
                      BCMPC_NUM_PPORTS_PER_CHIP_MAX);


/*******************************************************************************
 * Private functions
 */
/*!
 * This function updates the FDR_STATUS LT with the values retrieved from
 * the hardware.
 */
static int
bcmpc_lport_fdr_stats_lt_update(int unit, bcmpc_lport_t lport,
                                bcmpc_fdr_port_stats_t stats, bool fdr_intr)
{
    int rv = SHR_E_NONE;
    bcmpc_fdr_port_stats_t cur_stats;
    bcmpc_pport_t pport;
    bcmpc_port_cfg_t pcfg;

    SHR_FUNC_ENTER(unit);

    /* Update the operational state of the entry. */
    if (fdr_intr) {
        /*
         * If fdr_intr is set, it means this function is getting invoked
         * in the interrupt context. Theretically, the logical port
         * configuration should be fine. Set the state to valid.
         */
        stats.oper_state = BCMPC_VALID;
    } else {
        /*
         * If no logical to physical port map, set state to map unknown.
         * If no PC_PORT entry present, set it to port unknowa.
         */
        SHR_IF_ERR_EXIT_EXCEPT_IF
            (bcmpc_port_phys_map_get(unit, lport, &pport), SHR_E_NOT_FOUND);
        if (pport == BCMPC_INVALID_PPORT) {
            stats.oper_state = BCMPC_PORT_MAP_UNKNOWN;
        } else {
            rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport),
                                           P(&pcfg));
            if (rv == SHR_E_NOT_FOUND) {
                stats.oper_state = BCMPC_PORT_UNKNOWN;
            }
        }
    }

    /*
     * Check if PC_FDR_STATUS LT for this logical port
     * is already present in the DB.
     * If not present, insert the entry with the values.
     * If present, then Cupdate the entry with the values.
     */
    sal_memset(&cur_stats, 0, sizeof(bcmpc_fdr_port_stats_t));
    rv = bcmpc_db_imm_entry_lookup(unit, PC_FDR_STATSt,
                                   P(&lport), P(&cur_stats));

    if (rv == SHR_E_NOT_FOUND) {
        SHR_IF_ERR_EXIT
           (bcmpc_db_imm_entry_insert(unit,
                                      PC_FDR_STATSt,
                                      sizeof(bcmpc_lport_t), &lport,
                                      sizeof(bcmpc_fdr_port_stats_t),
                                      &stats));
    } else {
        /* Check the operational state of the entry and mark it as valid. */
        if (fdr_intr) {
            stats.oper_state = BCMPC_VALID;
        }
        SHR_IF_ERR_EXIT
            (bcmpc_db_imm_entry_update(unit,
                                       PC_FDR_STATSt,
                                       sizeof(bcmpc_lport_t), &lport,
                                       sizeof(bcmpc_fdr_port_stats_t),
                                       &stats));
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get physical port number from the the cached L2P map.
 *
 * PC_PORT_PHYS_MAPt could get the logical to physical port information.
 * This function is used to get the optimize the performance.
 * Need to take the rw_read lock before retrieving the l2p_map buffer.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \return The physical port number in the cached L2P map.
 */
static bcmpc_pport_t
bcmpc_l2p_pport_get(int unit, bcmpc_lport_t lport)
{
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;
    int rv = SHR_E_NONE;
    bcmpc_drv_t *pc_drv = NULL;
    uint32_t max_lports = 0, max_pports = 0;

    /* Validate the logical port. */
    rv = bcmpc_drv_get(unit, &pc_drv);
    if (SHR_FAILURE(rv)) {
        goto exit;
    }

    if (pc_drv && pc_drv->max_num_ports_get) {
        rv = pc_drv->max_num_ports_get(unit, &max_pports, &max_lports);
        if (SHR_FAILURE(rv)) {
            goto exit;
        }
    }

    if (lport > max_lports) {
        return BCMPC_INVALID_PPORT;
    }

    rv = sal_rwlock_rlock(l2p_lock[unit], SAL_RWLOCK_FOREVER);

    if (SHR_FAILURE(rv)) {
        goto exit;
    }

    pport = l2p_map[unit][lport];

    rv = sal_rwlock_give(l2p_lock[unit]);
    if (SHR_FAILURE(rv)) {
        LOG_WARN(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                 "sal_rwlock_give failed! - %d\n"), unit));
    }
exit:
    return pport;
}

/*!
 * \brief Set physical port number from the the cached L2P map.
 *
 * PC_PORT_PHYS_MAPt could set the logical to physical port information.
 * This function is used optimize the performance.
 * Need to take the rw_write lock before updating the l2p_map buffer.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \return The physical port number in the cached L2P map.
 */
static int
bcmpc_l2p_pport_set(int unit, bcmpc_lport_t lport, bcmpc_pport_t pport)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (sal_rwlock_wlock(l2p_lock[unit], SAL_RWLOCK_FOREVER));

    l2p_map[unit][lport] = pport;
    sal_rwlock_give(l2p_lock[unit]);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Get logical port number from the the cached P2L map.
 *
 * PC_PORT_PHYS_MAPt could get the logical to physical port information.
 * This function is used to get the inverse information, i.e. physical to
 * logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] pport Physical port number.
 *
 * \return The logical port number in the cached P2L map.
 */
static bcmpc_lport_t
bcmpc_p2l_lport_get(int unit, bcmpc_pport_t pport)
{
    return p2l_map[unit][pport];
}

/*!
 * \brief Initialize the PHY of the given logical port.
 *
 * This function is used to initialize the PHY of the given logical port.
 *
 * When \c do_core_init is set, this function will initialize the PHY of the
 * port, and also the core where the port belongs to.
 *
 * \param [in] unit Unit number.
 * \param [in] port Logical port number.
 * \param [in] pcfg Port configuration.
 * \param [in] enable True to enable port.
 * \param [in] do_core_init True to do core initialization.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcmpc_pm_port_attach(int unit, bcmpc_lport_t lport, bcmpc_port_cfg_t *pcfg,
                     int enable, bool do_core_init)
{
    int pm_id;
    bcmpc_pm_core_t pm_core;
    bcmpc_pm_core_cfg_t ccfg;
    pm_vco_setting_t vco_cfg;
    pm_speed_config_t *lport_speed_cfg = NULL;
    bcmpc_drv_t *pc_drv = NULL;
    bool is_internal = FALSE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_id_get(unit, pcfg->pport, &pm_id));
    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    /* Check if port is internal. */
    if (pc_drv && pc_drv->is_internal_port) {
        SHR_IF_ERR_EXIT
            (pc_drv->is_internal_port(unit, pcfg->pport, &is_internal));
    }
    if (is_internal) {
        return SHR_E_NONE;
    }

    bcmpc_pm_core_cfg_t_init(&ccfg);
    sal_memset(&vco_cfg, 0, sizeof(vco_cfg));

    pm_core.pm_id = pm_id;
    pm_core.core_idx = 0;
    SHR_IF_ERR_EXIT
        (bcmpc_pm_core_config_get(unit, pm_core, &ccfg));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_fw_load_config_get(unit, &ccfg.pm_fw_cfg));

    /* When manually inserting first port on the PM, do_core_init is true. */
    if (do_core_init) {
        SHR_ALLOC(lport_speed_cfg, sizeof(pm_speed_config_t) * 1,
                  "bcmpcTmpLportSpdCfg");
        SHR_NULL_CHECK(lport_speed_cfg, SHR_E_MEMORY);

        /* Only one speed configuration available when doing core_init. */
        vco_cfg.num_speeds = 1;
        lport_speed_cfg[0].speed = pcfg->speed;
        lport_speed_cfg[0].num_lanes = pcfg->num_lanes;
        lport_speed_cfg[0].fec = pcfg->fec_mode;
        vco_cfg.speed_config_list = lport_speed_cfg;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_pm_vco_rate_calculate(unit, pcfg->pport, &vco_cfg));
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_pm_core_attach(unit, pcfg->pport, &vco_cfg, NULL,
                                        &ccfg));
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_pm_port_attach(unit, lport, pcfg, &ccfg, enable));

exit:

    if (lport_speed_cfg != NULL) {
        SHR_FREE(lport_speed_cfg);
    }

    SHR_FUNC_EXIT();
}


/*!
 * \brief detach the PHY of the given logical port.
 *
 * This function is used to delete the PHY of the given logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] port Logical port number.
 *
 * \retval SHR_E_NONE No errors.
 */
static int
bcmpc_pm_port_detach(int unit, bcmpc_lport_t lport)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_pm_port_detach(unit, lport));

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Internal public functions
 */

/*!
  * \brief pm8x50 thread for RLM and 400G AN
  * \param [in] arg Unit number.
  *
  * \return Nothing.
  */
static void
bcmpc_thread(shr_thread_t tc, void *arg)
{
    int unit = (unsigned long)(arg);
    bcmpc_thread_ctrl_t *pm_thread = bcmpc_pm_thread_ctrl[unit];
    sal_usecs_t interval;
    bcmpc_pport_t pport;
    bcmpc_pm_info_t pm_info;

    int port , pm_id;
    bcmdrd_pbmp_t pbmp;
    int rv = SHR_E_NONE;

    interval = pm_thread->interval_us;

    while (1) {
        shr_thread_sleep(tc, interval);
        if (shr_thread_stopping(tc)) {
            break;
        }
        rv = bcmdrd_dev_logic_pbmp(unit, &pbmp);
        if (SHR_FAILURE(rv)) {
            return;
        }
        BCMDRD_PBMP_ITER(pbmp, port) {
            /*need to skip CPU and management port */
            if (port == 0) {
                continue;
            }
            pport = bcmpc_lport_to_pport(unit, port);
            if ((port > 0) && (pport != BCMPC_INVALID_PPORT) &&
                 (pport < BCMPC_NUM_PPORTS_PER_CHIP_MAX)) {
                rv = bcmpc_topo_id_get(unit, pport, &pm_id);
                if (SHR_FAILURE(rv)) {
                    return;
                }
                rv = bcmpc_pm_info_get(unit, pm_id, &pm_info);
                if (SHR_FAILURE(rv)) {
                    return;
                }

                if (SHR_BITGET(pm_info.prop.pm_feature,
                               BCMPC_FT_SW_STATE_UPDATE)) {
                    rv = bcmpc_pmgr_port_sw_state_update(unit, pport);
                    if (SHR_FAILURE(rv)) {
                        return;
                    }
                }
            }
        }
    }
}

/*!
 * \brief Function to config if to disable the pc thread.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmpc_thread_disable_config(int unit, bool disable)
{
    bcmpc_thread_ctrl_t *pm_thread = bcmpc_pm_thread_ctrl[unit];

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pm_thread, SHR_E_INIT);
    pm_thread->pc_thread_disable = disable;
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Function to initiate the port macro polling thread.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmpc_thread_enable(int unit, bool enable)
{
    bcmpc_thread_ctrl_t *pm_thread = bcmpc_pm_thread_ctrl[unit];

    SHR_FUNC_ENTER(unit);
    SHR_NULL_CHECK(pm_thread, SHR_E_INIT);

    /*
     * COVERITY
     * The following  Structurally-dead-code code was added intentionally for ease of debug.
     * It will be reverted soon.
     * coverity[Structurally-dead-code:FALSE]
     * coverity[UNREACHABLE:FALSE]
     */
    if (enable) {
        /* PMthread is already running. */
        if (pm_thread->pid) {
            SHR_EXIT();
        }

        pm_thread->pid = shr_thread_start("pm_thread",
                              SAL_THREAD_PRIO_DEFAULT,
                              bcmpc_thread,
                              ((void *)(unsigned long)(unit)));
        if (pm_thread->pid == NULL) {
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    } else {
        if (pm_thread->pid == NULL) {
            /* PM thread is already killed. */
            SHR_EXIT();
        }
        /* Wait for thread to exit. */
        SHR_IF_ERR_VERBOSE_EXIT(
            shr_thread_stop(pm_thread->pid, 10 * pm_thread->interval_us));
        pm_thread->pid = NULL;
    }
exit:
    SHR_FUNC_EXIT();
}



/*!
 * \brief Update hardware for the newly created logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pcfg Logical port config.
 * \param [in] do_core_init Do core init when set.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
port_insert_hw_update(int unit, bcmpc_lport_t lport, bcmpc_port_cfg_t *pcfg,
                      bool do_core_init)
{
    bcmpc_drv_t *pc_drv;
    bcmpc_thread_ctrl_t *pm_thread = bcmpc_pm_thread_ctrl[unit];
    bcmpc_pm_info_t pm_info;
    int pm_id;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    /* Bring the PortMacro TSC out of reset. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_pm_tsc_power_on(unit, pcfg->pport));

    /* Notify new logical port add to TM. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_tm_port_add(unit, lport, pcfg));

    /* Initialize the MAC for the logical port. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_pm_port_enable(unit, pcfg->pport, 1));

    /* Initialize the PHY for the logical port. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_port_attach(unit, lport, pcfg, 1, do_core_init));

    /* Force link down when update HW. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_port_link_change(unit, pcfg->pport, 0));

    /*
     * Logical port attach is success now update the
     * logical port to physical port association in
     * L2P cache. Map the logical port to the first
     * physical lane for a multi-lane port.
     */
     SHR_IF_ERR_VERBOSE_EXIT
         (bcmpc_l2p_pport_set(unit, lport, pcfg->pport));

    /*
     * Send out "bcmpcEvPortAdd" event now to notify the
     * registered components to take actions accordingly.
     */
    bcmevm_publish_event_notify(unit, "bcmpcEvPortAdd", (uint64_t)lport);

    /* Configure the logical port settings as provisioned. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_port_cfg_set(unit, pcfg->pport, lport, pcfg, true));

    /* only enable thread for Pm8x50 port */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_id_get(unit, pcfg->pport, &pm_id));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_info_get(unit, pm_id, &pm_info));

    if ((pm_thread != NULL) && (pm_thread->pid == NULL)) {
        if (pm_thread->pc_thread_disable == 0) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_thread_enable(unit, 1));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update hardware for the deleted logical port.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval !SHR_E_NONE Failure.
 */
static int
port_delete_hw_update(int unit, bcmpc_lport_t lport)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_port_detach(unit, lport));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the port configuration within a PM.
 *
 * This function would check if the port configure within a PM could be
 * supported, and update the PHY PLLs if required.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pport Physical port number.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pc_port_config_validate(int unit, bcmpc_lport_t lport,
                             bcmpc_port_cfg_t *pcfg)
{
    int pm_id;
    bool first_port = 0;
    bcmpc_pm_info_t pm_info;
    bcmpc_topo_type_t *pm_prop = &pm_info.prop;
    bcmpc_pport_t pidx, pend;
    bcmpc_lport_t logical_port;
    int rv = SHR_E_NONE;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_id_get(unit, pcfg->pport, &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_info_get(unit, pm_id, &pm_info));

    /*
     * Check if the port is the first created port within the PM.
     */
    pend = pm_info.base_pport + pm_prop->num_ports;
    for (pidx = pm_info.base_pport; pidx < pend; pidx++) {
        logical_port = bcmpc_pport_to_lport(unit, pidx);
        if (logical_port == BCMPC_INVALID_LPORT) {
            first_port = 1;
            continue;
        } else {
            first_port = 0;
            break;
        }
    }
    /*
     * Check if port configuration violates current PM settings.
     */
    rv = bcmpc_pmgr_pm_port_config_validate(unit, pcfg, first_port);
    if (SHR_FAILURE(rv)) {
        LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "The configured port VCO is "
                             "invalid on PM for logical port %"PRIu32".\n"),
                            lport));
        SHR_ERR_EXIT(rv);
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the RLM config on a given port.
 *
 * This function would check if the RLM initiator config could be supported, and
 * update the PHY PLLs if required.
 *
 * \param [in] unit Unit number.
 * \param [in] speed current port speed.
 * \param [in] port_index current index of the PM core.
 * \param [in] active_lane_map  RLM active lane map
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static
int pc_port_rlm_initiator_config_validate(int unit, int speed, int port_index,  uint32_t active_lane_map)
{
    int i, num_lane = 8, active_lane_num = 0;

    SHR_FUNC_ENTER(unit);

    for (i = 0; i < num_lane; i++) {
        if (active_lane_map &  1 << (port_index + i)) {
            active_lane_num++;
        }
    }

    /* first check 200G case */
    switch (speed) {
    case 150000:
        if ((active_lane_map != 0xf0) && (active_lane_map != 0xf)) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 " RLM active lane map is not valid \n")));
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
        break;
    case 200000:
        if (active_lane_num != 3) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 " RLM active lane map is not valid \n")));
            SHR_ERR_EXIT(SHR_E_FAIL);
        } else if ((port_index == 0) && (active_lane_map  & 0xf0)) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 " RLM active lane map is not valid \n")));
            SHR_ERR_EXIT(SHR_E_FAIL);
        } else if ((port_index == 4) && (active_lane_map  & 0x0f)) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 " RLM active lane map is not valid \n")));
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
        break;
    case 300000:
    case 350000:
        if (active_lane_map != 0xff) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 " RLM active lane map is not valid \n")));
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
        break;
    case 400000:
        if ((active_lane_num != 6) && (active_lane_num != 7)) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 " RLM active lane map is not valid \n")));
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
        break;
    default:
        break;
    }

exit:
    SHR_FUNC_EXIT();
}


/*!
 * \brief Validate the  RLM configurations.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pcfg Logical port config.
 *
 * \retval SHR_E_NONE The RLM configuration is ready to update to hardware.
 * \retval SHR_E_PORT The associated physical port number in \pcfg exceeds the
 *                    max physical port number.:w
 * \retval SHR_E_FAIL The RLM configuration is invalid.
 */
static int
pc_port_rlm_config_valid_check(int unit, bcmpc_lport_t lport,
                              bcmpc_port_cfg_t *pcfg)
{
    int pm_id;
    bcmpc_pm_info_t pm_info;
    bcmpc_pmgr_port_status_t pst;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_id_get(unit, pcfg->pport, &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_info_get(unit, pm_id, &pm_info));

    /*only PM8x50 can support RLM */
    if (!SHR_BITGET(pm_info.prop.pm_feature, BCMPC_FT_RLM)) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             " RLM feature is not supported "
                             "for logical port %"PRIu32".\n"), lport));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Get the status from hardware when the port is valid. */
    if (pcfg->pport != BCMPC_INVALID_PPORT && pcfg->speed != 0) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_port_status_get(unit, pcfg->pport, &pst));
        /* only 150G/300G/350G/200G/400G can enable RLM mode */
        if ((pst.speed == 150000) ||
            (pst.speed == 300000) ||
            (pst.speed == 350000) ||
            (pst.speed == 400000) ||
            ((pst.speed == 200000) && (pst.fec_mode != PM_PORT_FEC_NONE))) {
            /* need to verify RLM config valid or not */
            if (pcfg->initiator) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (pc_port_rlm_initiator_config_validate(unit, pst.speed,
                                                           pcfg->pport - pm_info.base_pport,
                                                           pcfg->active_lane_mask));
            }
        } else {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 " RLM can not be enabled with this speed "
                                 "for logical port %"PRIu32".\n"), lport));
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Validate the port configurations.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] pcfg Logical port config.
 * \param [in] do_insert Set when the request come from INSERT operation.
 *
 * \retval SHR_E_NONE The port configuration is ready to update to hardware.
 * \retval SHR_E_PORT The associated physical port number in \pcfg exceeds the
 *                    max physical port number.:w
 * \retval SHR_E_FAIL The port configuration is invalid.
 */
static int
pc_port_config_valid_check(int unit, bcmpc_lport_t lport,
                           bcmpc_port_cfg_t *pcfg, bool do_insert)
{
    int rv = 0, pm_id, num_phys_ports = 0, num_pmd_lanes = 0, idx = 0;
    bcmpc_pport_t pport;
    bcmpc_lport_t logical_port;
    bcmpc_pm_lport_info_t lp_info;
    bcmpc_pm_info_t pm_info;
    pm_oper_status_t op_st;
    bcmpc_drv_t *pc_drv;
    bcmpc_port_cfg_t cfg_old;

    SHR_FUNC_ENTER(unit);

    sal_memset(&op_st, 0, sizeof(pm_oper_status_t));
    pport = pcfg->pport;

    if (pport >= COUNTOF(p2l_map[unit])) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "The associated physical port number of logical "
                             "port %"PRIu32" exceeds the max physical port "
                             "number.\n"),
                  lport));
        SHR_ERR_EXIT(SHR_E_PORT);
    }

    /* Get the associated logical PM information. */
    SHR_IF_ERR_VERBOSE_EXIT
         (bcmpc_topo_id_get(unit, pport, &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
         (bcmpc_pm_info_get(unit, pm_id, &pm_info));

    /* Check if both Autoneg and RLM are enabled. */
    if ((sal_strcmp(pm_info.prop.name, "PC_PM_TYPE_PM8X50") == 0) ||
         (sal_strcmp(pm_info.prop.name, "PC_PM_TYPE_PM8X50_GEN2") == 0) ||
         (sal_strcmp(pm_info.prop.name, "PC_PM_TYPE_PM8X50_GEN3") == 0)) {
        if (pcfg->rlm_mode && pcfg->autoneg) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "The Autoneg and RLM cannot be"
                                 "supported for logical port %"PRIu32".\n"),
                      lport));
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    } else {
        if (pcfg->rlm_mode) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "The RLM is not supported"
                                 "for logical port %"PRIu32".\n"),
                      lport));
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

    /* Check if number of pmd lanes is configured. */
    num_pmd_lanes = pcfg->num_lanes;
    if (num_pmd_lanes == 0) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "The configured port is not specified"
                             "any lanes for logical port %"PRIu32".\n"),
                  lport));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

    /* Check if speed is configured. */
    if (!pcfg->speed) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "The configured port is not specified "
                              "non-zero speed for logical port %"PRIu32".\n"),
                  lport));
        SHR_ERR_EXIT(SHR_E_FAIL);
    }


    /* Check if number of pmd lanes is changed when updating PC_PORTt. */
    if (!do_insert) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pm_lport_info_get(unit, pport, NULL, &lp_info));
        if (num_pmd_lanes != shr_util_popcount(lp_info.lbmp)) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                             "The number of pmd lane on logical port "
                             " %"PRIu32" cannot be changed when updating port "
                             " configuration.\n"),
                      lport));
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    } else {
        /*
         * Check if the associated physical port is in used by other logical port
         * when inserting PC_PORTt.
         */
        num_phys_ports = pcfg->num_lanes;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_drv_get(unit, &pc_drv));
        if (pc_drv->num_phys_ports_get) {
            pc_drv->num_phys_ports_get(unit, pport, pcfg->num_lanes,
                                   &num_phys_ports);
        }
        for (idx = 0; idx < num_phys_ports; idx++) {
            logical_port = bcmpc_pport_to_lport(unit, pport+idx);
            /*
             * The physical port map should be INVALID or mapped to the
             * same logical port, which indicates the validation is being
             * invoked multiple times or update to the same physical
             * or logical port.
             */
            if ((logical_port != BCMPC_INVALID_LPORT) &&
                (logical_port != lport)) {
                LOG_INFO(BSL_LOG_MODULE,
                         (BSL_META_U(unit,
                                     "The associated physical port is "
                                      "unavailable for logical port "
                                      "%"PRIu32".\n"),
                          lport));
                SHR_ERR_EXIT(SHR_E_FAIL);
            }
        }
    }

    /*
     * The port config validate function below validates if the force speed
     * configuration of the port can be accomodated with the current PM
     * VCO settings.
     * The validation should only be done on a port if a port is in force
     * speed mode (auto-negotiation not enabled).
     * Skip the validation if
     * 1. autoneg is enabled on a port.
     * 2. there is no force speed or FEC type change.
     */
    bcmpc_port_cfg_t_init(&cfg_old);
    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport),
                                   P(&cfg_old)), SHR_E_NOT_FOUND);

    if ((!pcfg->autoneg) && ((cfg_old.speed != pcfg->speed) ||
        (cfg_old.fec_mode != pcfg->fec_mode))) {
        /* Validate the port configuration within the PM. */
        rv = pc_port_config_validate(unit, lport, pcfg);
        if (SHR_FAILURE(rv)) {
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "The port configure within the PM is "
                                 "invalid for logical port %"PRIu32".\n"),
                      lport));
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }

    if (pcfg->rlm_mode) {
        if (!do_insert) {
            /*
             * Verify RLM configuration when:
             * 1. There's any RLM related field value change.
             * 2. User update rlm_mode field to re-start RLM state machine.
             */
            if ((pcfg->rlm_mode != cfg_old.rlm_mode) ||
                (pcfg->initiator != cfg_old.initiator) ||
                (pcfg->active_lane_mask != cfg_old.active_lane_mask) ||
                SHR_BITGET(pcfg->fbmp, BCMPC_PORT_LT_FLD_RLM)) {
                rv = pc_port_rlm_config_valid_check(unit, lport, pcfg);
                if (SHR_FAILURE(rv) && rv != SHR_E_UNAVAIL) {
                    LOG_INFO(BSL_LOG_MODULE,
                             (BSL_META_U(unit,
                                         "The RLM config is invalid "
                                         "for logical port %"PRIu32".\n"),
                              lport));
                    SHR_ERR_EXIT(SHR_E_FAIL);
                }
            }
        } else {
            /* Insert operation. */
            LOG_INFO(BSL_LOG_MODULE,
                     (BSL_META_U(unit,
                                 "The RLM is not supported on a new port.\n")));
            SHR_ERR_EXIT(SHR_E_FAIL);
        }
    }
exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse function for reinitializing the non-HA cache data.
 *
 * This function is called for each logical port by traversing the logical port
 * table.
 *
 * \param [in] unit Unit number.
 * \param [in] key_size Key size, i.e. sizeof(bcmpc_lport_t).
 * \param [in] key The logical port number, bcmpc_lport_t.
 * \param [in] data_size Data size, i.e. sizeof(bcmpc_port_cfg_t).
 * \param [in] data The logical port configuration, bcmpc_port_cfg_t.
 * \param [in] cookie Not used in this function.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pc_cache_reinit(int unit, size_t key_size, void *key,
                  size_t data_size, void *data, void *cookie)
{
    bcmpc_lport_t lport = *(bcmpc_lport_t *)key;
    bcmpc_port_cfg_t *pcfg = data;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;
    bcmpc_drv_t *pc_drv;
    int num_phys_ports = pcfg->num_lanes;

    SHR_FUNC_ENTER(unit);

    /*
     * The operational state of the PC_PORT entry is not valid,
     * do not populdate the software DB as it is invalid.
     */
    if (pcfg->oper_state != BCMPC_VALID) {
       SHR_EXIT();
    }

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpc_port_phys_map_get(unit, lport, &pcfg->pport), SHR_E_NOT_FOUND);
    pport = pcfg->pport;

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_inst_info_update(unit, pcfg));

    /* Recover the used physical ports. */
    SHR_BITSET(occupied_ppbmp[unit], pport);

    /* Recover bitmap of port. */
    SHR_IF_ERR_VERBOSE_EXIT
    (bcmpc_drv_get(unit, &pc_drv));
    if (pc_drv->num_phys_ports_get) {
        pc_drv->num_phys_ports_get(unit, pport, pcfg->num_lanes,
                                   &num_phys_ports);
    }

    /* Populate the physical port to logical port association. */
    while (num_phys_ports--) {
        p2l_map[unit][pport++] = lport;
    }

    /* Populate the logical port to physical port association. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_l2p_pport_set(unit, lport, pcfg->pport));

    /* PM init. */
    SHR_IF_ERR_VERBOSE_EXIT
       (bcmpc_pmgr_pm_init(unit, pcfg->pport));

    /* Recover link state */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_forced_link_state_recover(unit, pcfg->pport, pcfg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Free the physical ports which are occupied by the given logical port.
 *
 * When doing port deletion, the caller need to pass \c do_remove = 1 to remove
 * the P2L mapping and free the physical port resource which is used by the
 * given logical port.
 *
 * When doing PM mode change, the caller must pass \c do_remove = 0 to free the
 * physical ports which are used by the logical port, but not the physical port
 * resource itself.
 *
 * \param [in] unit Unit number.
 * \param [in] lport Logical port number.
 * \param [in] do_remove True to remove the entry from the P2L table as well;
 *                       otherwise only free the occupied physical ports.
 *
 * \retval SHR_E_NONE Success.
 * \retval SHR_E_FAIL Failure.
 */
static int
pc_lport_free(int unit, bcmpc_lport_t lport, bool do_remove)
{
    int num_phys_ports = 0;
    bcmpc_pport_t pport;
    bcmpc_pm_lport_info_t lp_info;
    bcmpc_drv_t *pc_drv;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_port_phys_map_get(unit, lport, &pport));

    /* Free the associated physical port. */
    SHR_BITCLR(occupied_ppbmp[unit], pport);

    /*
     * Send out "bcmpcEvPortDelete" event first, to avoid the other module to
     * keep using the deleting port.
     */
    bcmevm_publish_event_notify(unit, "bcmpcEvPortDelete", (uint64_t)lport);

    /* Remove p2l_map and its associated lanes. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_lport_info_get(unit, pport, NULL, &lp_info));
    num_phys_ports = shr_util_popcount(lp_info.lbmp);
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_drv_get(unit, &pc_drv));
    if (pc_drv->num_phys_ports_get) {
        pc_drv->num_phys_ports_get(unit, pport,
                                   shr_util_popcount(lp_info.lbmp),
                                   &num_phys_ports);
    }
    if (do_remove) {
        /*
         * Remove the physical port to logical port
         * association in the p2l map.
         */
        while (num_phys_ports--) {
            p2l_map[unit][pport++] = BCMPC_INVALID_LPORT;
        }
        /*
         * Remove the logical port to physical port
         * association in the l2p map.
         */
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_l2p_pport_set(unit, lport, BCMPC_INVALID_PPORT));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pc_internal_port_insert(int unit, bcmpc_lport_t lport, bcmpc_port_cfg_t *pcfg)
{
    int pcnt, pm_id, num_phys_ports = 0;
    bool do_core_init;
    bcmpc_pm_info_t core;
    bcmpc_pport_t pport;
    bcmpc_drv_t *pc_drv;
    pm_info_t *pm_drv_info = NULL;

    SHR_FUNC_ENTER(unit);
    pport = pcfg->pport;
    num_phys_ports = pcfg->num_lanes;

    if (SHR_FAILURE(pc_port_config_valid_check(unit, lport, pcfg, true))) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Logical port %"PRIu32" configuration validation "
                             "failed. It does not update configuration into "
                             "database and hardware.\n"),
                  lport));
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Update bitmap of the port. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_drv_get(unit, &pc_drv));
    if (pc_drv->num_phys_ports_get) {
        pc_drv->num_phys_ports_get(unit, pport, pcfg->num_lanes,
                                   &num_phys_ports);
    }
    while (num_phys_ports--) {
        p2l_map[unit][pport++] = lport;
    }
    pport = pcfg->pport;

    /* Get the associated PM core information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_id_get(unit, pport, &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_info_get(unit, pm_id, &core));

    /* Get the associated PM core driver database information. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_drv_info_get(unit, pm_id, &pm_drv_info));

    if (pm_drv_info == NULL) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "PM driver DB NULL on Logical port %"PRIu32"."
                             "\n"), lport));
        /*
         * If this logical port is the first created port within a PM core,
         * we need to initialize the core as well.
         */
        SHR_BITCOUNT_RANGE(occupied_ppbmp[unit], pcnt,
                           core.base_pport, core.prop.num_ports);
        do_core_init = (pcnt == 0);
    } else {
        do_core_init = pm_drv_info->pm_data.pm_ha_info->is_pm_initialized? 0: 1;
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "PM(%d)-initialized(%d)-Lport%"PRIu32"." "\n"),
                             pm_id, do_core_init, lport));
    }

    /* Update port config information in PM instance structure. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_inst_info_update(unit, pcfg));

    /* Update the settings to hardware. */
    if (SHR_FAILURE(port_insert_hw_update(unit, lport, pcfg, do_core_init))) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Hardware update failed on Logical port %"PRIu32"."
                             "\n"),
                  lport));

        /* Remove p2l_map when hardware update failed. */
        num_phys_ports = pcfg->num_lanes;
        if (pc_drv->num_phys_ports_get) {
            pc_drv->num_phys_ports_get(unit, pport, pcfg->num_lanes,
                                        &num_phys_ports);
        }
        while (num_phys_ports--) {
            p2l_map[unit][pport++] = BCMPC_INVALID_LPORT;
        }

        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /* Update the used physical ports. */
    SHR_BITSET(occupied_ppbmp[unit], pport);

exit:
    SHR_FUNC_EXIT();
}

static int
pc_internal_port_delete(int unit, bcmpc_lport_t lport)
{
    bcmpc_pport_t pport;
    uint32_t no_more_active_port = 1;
    bcmpc_pm_info_t pm_info;
    int pm_id, rv;

    SHR_FUNC_ENTER(unit);

    /* first get the pport */
    pport = bcmpc_l2p_pport_get(unit, lport);
    if (pport == BCMPC_INVALID_PPORT) {
        return BCMPC_INVALID_PPORT;
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_id_get(unit, pport, &pm_id));
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_info_get(unit, pm_id, &pm_info));

    /* Need to check if management port, if so
     * port delete is NOT allowed */
    if (SHR_BITGET(pm_info.prop.pm_feature, BCMPC_FT_MGMT_PORT_MACRO)) {
        LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit,
                            "The management port can not be deleted."
                            "port %"PRIu32"  \n"),
                 lport));
        SHR_ERR_EXIT(SHR_E_UNAVAIL);
    }

    /* Disable the logical port first. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_port_enable_set(unit, pport, lport, 0));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_tm_port_delete(unit, lport));

    /* Remove hardware setting of the port. */
    SHR_IF_ERR_VERBOSE_EXIT
        (port_delete_hw_update(unit, lport));

    /* Free the associated physical port. */
    SHR_IF_ERR_VERBOSE_EXIT
        (pc_lport_free(unit, lport, true));

    /* next need to check if there is no more front panel port */
    for (pport = 1; pport < BCMPC_NUM_PPORTS_PER_CHIP_MAX; pport++) {
        /* need to check if Pm8x50 port */
        rv = bcmpc_topo_id_get(unit, pport, &pm_id);
        if (rv == SHR_E_NOT_FOUND) {
            continue;
        }
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pm_info_get(unit, pm_id, &pm_info));

        /* make sure only count PM8x50 front panel port */
        if ( (sal_strcmp(pm_info.prop.name, "PC_PM_TYPE_PM8X50") == 0) ||
             (sal_strcmp(pm_info.prop.name, "PC_PM_TYPE_PM8X50_GEN2") == 0) ||
             (sal_strcmp(pm_info.prop.name, "PC_PM_TYPE_PM8X50_GEN3") == 0)) {
            if (p2l_map[unit][pport] != BCMPC_INVALID_LPORT) {
                no_more_active_port = 0;
                break;
            }
        }
    }

    /* need to kill the thread if no more active port */
    if (no_more_active_port) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_thread_deinit(unit));
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pc_internal_port_update(int unit, bcmpc_lport_t lport, bcmpc_port_cfg_t *pcfg)
{
    int rv = SHR_E_NONE;
    bcmpc_pport_t old_pport;
    bcmpc_port_cfg_t cfg_old;

    SHR_FUNC_ENTER(unit);

    bcmpc_port_cfg_t_init(&cfg_old);

    /*
     * If the associated physical port is changed, the process will be like
     * we delete the old activated port and add it back with the new
     * configuration.
     */
    rv = bcmpc_port_phys_map_get(unit, lport, &old_pport);
    if (SHR_SUCCESS(rv) && (pcfg->pport != old_pport)) {
        if (bcmpc_p2l_lport_get(unit, old_pport) == lport) {
            SHR_IF_ERR_VERBOSE_EXIT
                (pc_internal_port_delete(unit, lport));
        }
    }

    if (pcfg->pport == BCMPC_INVALID_PPORT) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "The associated physical port number of logical "
                             "port %"PRIu32" is not specified.\n"),
                  lport));
        SHR_EXIT();
    }

    /*
     * If we try to update the configuration of a port which is currently in
     * invalid state, we need to do insert.
     */
    if (bcmpc_p2l_lport_get(unit, pcfg->pport) == BCMPC_INVALID_LPORT) {
        SHR_ERR_EXIT
            (pc_internal_port_insert(unit, lport, pcfg));
    }

    /*
     * For the other fields changes, we only need to update the port with the
     * given configuration.
     */
    if (SHR_FAILURE(pc_port_config_valid_check(unit, lport, pcfg, false))) {
        LOG_INFO(BSL_LOG_MODULE,
                 (BSL_META_U(unit,
                             "Logical port %"PRIu32" configuration validation "
                             "failed. It does not update configuration into "
                             "database and hardware.\n"),
                  lport));
        SHR_ERR_EXIT(SHR_E_PARAM);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&cfg_old)));

     cfg_old.pport = old_pport;
    /* Notify TM speed change and encapsulation change. */
    if ((cfg_old.speed != pcfg->speed) ||
        (cfg_old.encap_mode != pcfg->encap_mode)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_tm_port_update(unit, lport, &cfg_old, pcfg));
    }

    rv = bcmpc_pmgr_port_cfg_set(unit, pcfg->pport, lport, pcfg, false);

    /* Notify TM speed change and encapsulation change. */
    if ((SHR_FAILURE(rv)) && ((cfg_old.speed != pcfg->speed) ||
        (cfg_old.encap_mode != pcfg->encap_mode))) {
        cfg_old.pport = pcfg->pport;
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_tm_port_update(unit, lport, pcfg, &cfg_old));
    }

    SHR_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Re-activate the port due to PM mode change.
 *
 * This is the traverse callback function which is used in
 * \ref bcmpc_ports_activate.
 *
 * \param [in] unit Unit number.
 * \param [in] key_size Key size, i.e. sizeof(bcmpc_lport_t).
 * \param [in] key The logical port number, bcmpc_lport_t.
 * \param [in] data_size Data size, i.e. sizeof(bcmpc_port_cfg_t).
 * \param [in] data The logical port configuration, bcmpc_port_cfg_t.
 * \param [in] cookie The physical port range, pc_pport_range_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pc_port_activate(int unit, size_t key_size, void *key,
                 size_t data_size, void *data,
                 void *cookie)
{
    bcmpc_lport_t *lport = key;
    bcmpc_port_cfg_t *pcfg = data;
    bcmpc_pport_t pport;
    pc_pport_range_t *prange = cookie;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpc_port_phys_map_get(unit, *lport, &pport), SHR_E_NOT_FOUND);

    /* Do nothing if the port is not in the given range. */
    if (pport < prange->start || pport > prange->end) {
        SHR_EXIT();
    }

    pcfg->pport = pport;
    SHR_IF_ERR_VERBOSE_EXIT
        (pc_internal_port_insert(unit, *lport, pcfg));

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Update the AUTONEG configuration to hardware.
 *
 * This is the traverse callback function which is used in
 * \ref bcmpc_autoneg_prof_internal_update.
 *
 * \param [in] unit Unit number.
 * \param [in] key_size Key size, i.e. sizeof(bcmpc_lport_t).
 * \param [in] key The logical port number, bcmpc_lport_t.
 * \param [in] data_size Data size, i.e. sizeof(bcmpc_port_cfg_t).
 * \param [in] data The logical port configuration, bcmpc_port_cfg_t.
 * \param [in] cookie The new AUTONEG entry, pc_autoneg_prof_entry_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
port_autoneg_hw_update(int unit, size_t key_size, void *key,
                       size_t data_size, void *data, void *cookie)
{
    bcmpc_lport_t lport = *(bcmpc_lport_t *)key;
    bcmpc_port_cfg_t *pcfg = data;
    pc_autoneg_prof_entry_t *autoneg_prof_entry = cookie;
    bcmpc_autoneg_prof_t an_cfg[NUM_AUTONEG_PROFILES_MAX];
    int index = 0, do_hw_config = 0, rv = 0;

    SHR_FUNC_ENTER(unit);

    for (index = 0; index < pcfg->num_an_profs; index++) {
        if (pcfg->autoneg_profile_ids[index] == autoneg_prof_entry->prof_id) {
            do_hw_config = 1;
            break;
        }
    }
    if (do_hw_config) {
        /* Do nothing for an invalid port. */
        SHR_IF_ERR_EXIT_EXCEPT_IF
             (bcmpc_port_phys_map_get(unit,
                                      lport,
                                      &pcfg->pport),
                                      SHR_E_NOT_FOUND);
        for (index = 0; index < pcfg->num_an_profs; index++) {
            if (autoneg_prof_entry->prof_id ==
                pcfg->autoneg_profile_ids[index]) {
                sal_memcpy(&(an_cfg[index]),
                           autoneg_prof_entry->prof,
                           sizeof(bcmpc_autoneg_prof_t));
            } else {
                rv = bcmpc_db_imm_entry_lookup(unit,
                                           PC_AUTONEG_PROFILEt,
                                           P(&pcfg->autoneg_profile_ids[index]),
                                           P(&an_cfg[index]));
                if (rv == SHR_E_NOT_FOUND) {
                    return rv;
                }
            }
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_pm_port_autoneg_advert_set(unit,
                                                   lport,
                                                   an_cfg,
                                                   pcfg));
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Internal Public functions
 */

void
bcmpc_port_cfg_t_init(bcmpc_port_cfg_t *pcfg)
{
    sal_memset(pcfg, 0, sizeof(*pcfg));
    pcfg->lport = BCMPC_INVALID_LPORT;
    pcfg->pport = BCMPC_INVALID_PPORT;
}

int
bcmpc_cache_init(int unit, bool warm)
{
    bcmpc_drv_t *pc_drv = NULL;
    uint32_t max_lports = BCMPC_NUM_LPORTS_PER_CHIP_MAX, max_pports = 0;
    uint32_t j;
    int i;
    bcmpc_thread_ctrl_t *pm_thread = bcmpc_pm_thread_ctrl[unit];

    SHR_FUNC_ENTER(unit);

    SHR_BITCLR_RANGE(occupied_ppbmp[unit], 0, BCMPC_NUM_PPORTS_PER_CHIP_MAX);

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    if (pc_drv && pc_drv->max_num_ports_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_drv->max_num_ports_get(unit, &max_pports, &max_lports));
    }

    /* Initialize the physical port to logical port map buffer. */
    sal_memset(p2l_map[unit], 0, sizeof(p2l_map[unit]));
    for (i = 0; i < COUNTOF(p2l_map[unit]); i++) {
        p2l_map[unit][i] = BCMPC_INVALID_LPORT;
    }

    /* Initialize the logical port to physical port map buffer. */
    for (j = 0; j < max_lports; j++) {
        l2p_map[unit][j] = BCMPC_INVALID_PPORT;
    }

    if (warm) {
        /* Recover the PC non-HA cache by traversing PC_PORTt. */
        bcmpc_db_imm_entry_traverse(unit,
                                    PC_PORTt,
                                    sizeof(bcmpc_lport_t),
                                    sizeof(bcmpc_port_cfg_t),
                                    pc_cache_reinit,
                                    NULL);
    }
    /*
     * Create HA link state buffer and
     * recover port link state if in warmboot.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_ha_link_state_create(unit, warm));

    if (warm) {
        if ((pm_thread != NULL) && (pm_thread->pid == NULL)) {
            if (pm_thread->pc_thread_disable == 0) {
                SHR_IF_ERR_VERBOSE_EXIT
                    (bcmpc_thread_enable(unit, 1));
            }
        }
    }
exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pport_free_check(int unit, bcmpc_pport_t pport, size_t pcnt)
{
    bcmpc_pport_t p;

    for (p = pport; p < pport + pcnt; p++) {
        if (SHR_BITGET(occupied_ppbmp[unit], p)) {
            return SHR_E_FAIL;
        }
    }

    return SHR_E_NONE;
}

int
bcmpc_ports_deactivate(int unit, bcmpc_pport_t pstart, int pcnt)
{
    bcmpc_pport_t pport, pend;
    bcmpc_lport_t lport;

    SHR_FUNC_ENTER(unit);

    pend = pstart + pcnt;
    for (pport = pstart; pport < pend; pport++) {
        lport = bcmpc_pport_to_lport(unit, pport);
        if (lport == BCMPC_INVALID_LPORT) {
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (pc_lport_free(unit, lport, false));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_ports_activate(int unit, bcmpc_pport_t pstart, int pcnt)
{
    pc_pport_range_t prange;

    SHR_FUNC_ENTER(unit);

    prange.start = pstart;
    prange.end = pstart + pcnt - 1;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_db_imm_entry_traverse(unit,
                                     PC_PORTt,
                                     sizeof(bcmpc_lport_t),
                                     sizeof(bcmpc_port_cfg_t),
                                     pc_port_activate,
                                     &prange));
exit:
    SHR_FUNC_EXIT();
}

void
bcmpc_autoneg_prof_t_init(bcmpc_autoneg_prof_t *prof)
{
    sal_memset(prof, 0, sizeof(*prof));
}

int
bcmpc_autoneg_prof_internal_update(int unit, int prof_id,
                                   bcmpc_autoneg_prof_t *prof)
{
    pc_autoneg_prof_entry_t autoneg_prof_entry;

    SHR_FUNC_ENTER(unit);

    autoneg_prof_entry.prof_id = prof_id;
    autoneg_prof_entry.prof = prof;
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_db_imm_entry_traverse(unit,
                                     PC_PORTt,
                                     sizeof(bcmpc_lport_t),
                                     sizeof(bcmpc_port_cfg_t),
                                     port_autoneg_hw_update,
                                     &autoneg_prof_entry));
exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_internal_port_cfg_insert(int unit, bcmpc_lport_t lport,
                               bcmpc_port_cfg_t *pcfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpc_port_phys_map_get(unit, lport, &pcfg->pport), SHR_E_NOT_FOUND);

    /*
     * Would not expect user call port suspend operation during
     * a logical port insert.
     */
    if ((pcfg->suspend) && (pcfg->pport != BCMPC_INVALID_PPORT)) {
        bcmpc_pmgr_port_suspend_set(unit, pcfg->pport, 1);
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_internal_port_insert(unit, lport, pcfg));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_internal_port_cfg_delete(int unit, bcmpc_lport_t lport)
{
    int rv;
    bcmpc_pport_t pport;

    SHR_FUNC_ENTER(unit);

    rv = bcmpc_port_phys_map_get(unit, lport, &pport);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_internal_port_delete(unit, lport));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_internal_port_cfg_update(int unit, bcmpc_lport_t lport,
                               bcmpc_port_cfg_t *pcfg)
{
    bcmpc_port_oper_status_t op_st;
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpc_port_phys_map_get(unit, lport, &pcfg->pport), SHR_E_NOT_FOUND);

    /*
     * Following are valid operations for an update operation.
     * Normal operation -> Update LT fields.
     *     Perform updation.
     * Normal operation -> Suspend operation and update other LT fields.
     *     Perform updation, then perform suspend operation.
     * Normal operation -> Suspend operation.
     *     Perform suspend operation.
     * Suspended -> update other LT fields.
     *     Invalid operation, reject and update PC_PORT_INFO.
     * Suspended -> Resume operation.
     *     perform resume operation.
     * Suspended -> Resume operation, and update other LT fields.
     *     perform resume operation followed by updation.
     *
     * The caller sets the port configuration members
     * is_suspended, suspend and resume.
     *
     * is_suspended is 1, if the PC_PORTt.SUSPENDf is previously set.
     * that is port is in suspended state.
     *
     * suspend is 1, if the PC_PORTt.SUSPENDf is the current operation.
     *
     * resume is 1, if the PC_PORTt.SUSPENDf is the previously set and
     * is reset in the current operation.
     */

    /* Restore switching on the port. */
    if ((pcfg->resume) && (pcfg->pport != BCMPC_INVALID_PPORT)) {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_port_suspend_set(unit, pcfg->pport, 0));
        pcfg->is_suspended = FALSE;
    }

    if (!pcfg->is_suspended) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_internal_port_update(unit, lport, pcfg));

        if ((pcfg->suspend) && (pcfg->pport != BCMPC_INVALID_PPORT)) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_pmgr_port_suspend_set(unit, pcfg->pport, 1));
            pcfg->is_suspended = TRUE;
        }
    } else {
        /*
         * Cannot allow PC_PORT update for LT fields other than
         * SUSPEND, as it can lead to unpredictable behavior.
         * Update the PC_PORT_INFO with error code.
         */
        sal_memset(&op_st, 0, sizeof(bcmpc_port_oper_status_t));
        op_st.status = PM_PORT_SUSPENDED;
        bcmpc_pmgr_oper_status_table_update(unit, lport,
                                            pcfg->pport, &op_st);
        SHR_ERR_EXIT(SHR_E_FAIL);
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_internal_port_map_insert(int unit, bcmpc_lport_t lport,
                               bcmpc_port_cfg_t *l2p_cfg)
{
    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_internal_port_insert(unit, lport, l2p_cfg));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_internal_port_map_delete(int unit, bcmpc_lport_t lport)
{
    int rv;

    SHR_FUNC_ENTER(unit);

    rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), 0, NULL);
    if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    SHR_IF_ERR_VERBOSE_EXIT
        (pc_internal_port_delete(unit, lport));

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_internal_port_map_update(int unit, bcmpc_lport_t lport,
                               bcmpc_port_cfg_t *l2p_cfg)
{
    int rv;
    bcmpc_port_cfg_t pcfg;

    SHR_FUNC_ENTER(unit);

    rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg));
    if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    } else {
        SHR_IF_ERR_VERBOSE_EXIT(rv);
    }

    pcfg.pport = l2p_cfg->pport;
    SHR_IF_ERR_VERBOSE_EXIT
        (pc_internal_port_update(unit, lport, &pcfg));

exit:
    SHR_FUNC_EXIT();
}

void
bcmpc_port_timesync_t_init(bcmpc_port_timesync_t *config)
{
    sal_memset(config, 0, sizeof(*config));
}

int
bcmpc_internal_port_timesync_insert(int unit, bcmpc_lport_t lport,
                                    bcmpc_port_timesync_t *config)
{
    int rv;
    bcmpc_pport_t pport;
    bcmpc_port_cfg_t pcfg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpc_port_phys_map_get(unit, lport, &pport), SHR_E_NOT_FOUND);

    rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg));
    if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_timesync_set(unit, lport, pport, config));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_internal_port_timesync_update(int unit, bcmpc_lport_t lport,
                                    bcmpc_port_timesync_t *config)
{
    int rv;
    bcmpc_pport_t pport;
    bcmpc_port_cfg_t pcfg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpc_port_phys_map_get(unit, lport, &pport), SHR_E_NOT_FOUND);

    rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg));
    if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_timesync_set(unit, lport, pport, config));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_internal_port_timesync_delete(int unit, bcmpc_lport_t lport,
                                    bcmpc_port_timesync_t *config)
{
    int rv;
    bcmpc_pport_t pport;
    bcmpc_port_cfg_t pcfg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpc_port_phys_map_get(unit, lport, &pport), SHR_E_NOT_FOUND);

    rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg));
    if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    } else {
        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_pmgr_timesync_set(unit, lport, pport, config));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_internal_port_fdr_cfg_insert(int unit, bcmpc_lport_t lport,
                                   bcmpc_fdr_port_control_t *fdr_cfg)
{
    int rv;
    bcmpc_pport_t pport;
    bcmpc_port_cfg_t pcfg;
    bcmpc_fdr_port_stats_t fdr_stats;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpc_port_phys_map_get(unit, lport, &pport), SHR_E_NOT_FOUND);

    rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg));
    if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    } else {
        SHR_IF_ERR_EXIT
            (bcmpc_pmgr_fdr_control_set(unit, lport, pport, fdr_cfg));
        /*
         * Insert the FDR_STATS LT for this logical port to capture the
         * statistics.
         */
        sal_memset(&fdr_stats, 0, sizeof(fdr_stats));
        SHR_IF_ERR_EXIT
            (bcmpc_lport_fdr_stats_lt_update(unit, lport, fdr_stats, false));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_internal_port_fdr_cfg_update(int unit, bcmpc_lport_t lport,
                                   bcmpc_fdr_port_control_t *fdr_cfg)
{
    int rv;
    bcmpc_pport_t pport;
    bcmpc_port_cfg_t pcfg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpc_port_phys_map_get(unit, lport, &pport), SHR_E_NOT_FOUND);

    rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg));
    if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    } else {
        SHR_IF_ERR_EXIT
            (bcmpc_pmgr_fdr_control_set(unit, lport, pport, fdr_cfg));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_internal_port_fdr_cfg_delete(int unit, bcmpc_lport_t lport)
{
    int rv;
    bcmpc_pport_t pport;
    bcmpc_port_cfg_t pcfg;
    bcmpc_fdr_port_control_t fdr_cfg;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpc_port_phys_map_get(unit, lport, &pport), SHR_E_NOT_FOUND);

    rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg));
    if (rv == SHR_E_NOT_FOUND) {
        SHR_EXIT();
    } else {
        sal_memset(&fdr_cfg, 0, sizeof(fdr_cfg));
        SHR_IF_ERR_EXIT
            (bcmpc_pmgr_fdr_control_set(unit, lport, pport, &fdr_cfg));
    }

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_internal_port_fdr_stats_update(int unit, bcmpc_lport_t lport,
                                     bool fdr_intr)
{
    int rv;
    bcmpc_pport_t pport = BCMPC_INVALID_PPORT;
    bcmpc_port_cfg_t pcfg;
    bcmpc_fdr_port_stats_t fdr_stats;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT_EXCEPT_IF
        (bcmpc_port_phys_map_get(unit, lport, &pport), SHR_E_NOT_FOUND);

    rv = bcmpc_db_imm_entry_lookup(unit, PC_PORTt, P(&lport), P(&pcfg));
    if (rv == SHR_E_NOT_FOUND) {
        if (fdr_intr) {
            LOG_WARN(BSL_LOG_MODULE,
                    (BSL_META_U(unit,
                     "FDR interrupt on unconfigured logical port(%d)\n"),
                      lport));
            SHR_IF_ERR_EXIT(rv);
        } else {
            SHR_EXIT();
        }
    } else {
        sal_memset(&fdr_stats, 0, sizeof(fdr_stats));
        SHR_IF_ERR_EXIT
            (bcmpc_pmgr_fdr_stats_get(unit, lport, pport, &fdr_stats));
        SHR_IF_ERR_EXIT
            (bcmpc_lport_fdr_stats_lt_update(unit, lport, fdr_stats, fdr_intr));
    }

exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Public functions
 */

bcmpc_lport_t
bcmpc_pport_to_lport(int unit, bcmpc_pport_t pport)
{
    if (pport >= BCMPC_NUM_PPORTS_PER_CHIP_MAX) {
        return BCMPC_INVALID_LPORT;
    }

    return bcmpc_p2l_lport_get(unit, pport);
}

bcmpc_pport_t
bcmpc_lport_to_pport(int unit, bcmpc_lport_t lport)
{
    bcmpc_pport_t pport;

    pport = bcmpc_l2p_pport_get(unit, lport);
    if (pport == BCMPC_INVALID_PPORT) {
        return BCMPC_INVALID_PPORT;
    }

    if (bcmpc_p2l_lport_get(unit, pport) != lport) {
        return BCMPC_INVALID_PPORT;
    }

    return pport;
}

int
bcmpc_port_failover_loopback_remove(int unit, bcmpc_lport_t lport)
{
    bcmpc_pport_t pport;

    pport = bcmpc_lport_to_pport(unit, lport);
    if (pport == BCMPC_INVALID_PPORT) {
        return SHR_E_PORT;
    }

    return bcmpc_pmgr_failover_loopback_remove(unit, pport);
}

int
bcmpc_thread_init(int unit)
{
    bcmpc_thread_ctrl_t *pm_thread_ctrl = NULL;

    SHR_FUNC_ENTER(unit);


    SHR_ALLOC(pm_thread_ctrl, sizeof(bcmpc_thread_ctrl_t), "bcmpcThreadCtrl");

    SHR_NULL_CHECK(pm_thread_ctrl, SHR_E_MEMORY);

    sal_memset(pm_thread_ctrl, 0, sizeof(bcmpc_thread_ctrl_t));

    sal_snprintf(pm_thread_ctrl->name, sizeof(pm_thread_ctrl->name), "PM_THREAD.%d", unit);
    pm_thread_ctrl->interval_us = PC_PORT_SCAN_INTERVAL;
    pm_thread_ctrl->pc_thread_disable = 0;

    bcmpc_pm_thread_ctrl[unit] = pm_thread_ctrl;

exit:
    if (SHR_FUNC_ERR()) {
        bcmpc_pm_thread_ctrl[unit] = NULL;
    }
    SHR_FUNC_EXIT();
}

int
bcmpc_thread_deinit(int unit)
{
    bcmpc_thread_ctrl_t *pm_thread = bcmpc_pm_thread_ctrl[unit];
    SHR_FUNC_ENTER(unit);

    if (pm_thread) {
        if (pm_thread->pid != NULL) {
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_thread_enable(unit, 0));
        }
        SHR_FREE(pm_thread);
        bcmpc_pm_thread_ctrl[unit] = NULL;
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Create rw lock for accessing the l2p_map buffer.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmpc_l2p_lock_init(int unit)
{
    SHR_FUNC_ENTER(unit);

    l2p_lock[unit] = sal_rwlock_create("PC_L2P");
    SHR_NULL_CHECK(l2p_lock[unit], SHR_E_MEMORY);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy rw lock for l2p_map buffer.
 *
 * \param [in] unit Unit number.
 *
 * \retval Nothing.
 */
int
bcmpc_l2p_lock_cleanup(int unit)
{
    SHR_FUNC_ENTER(unit);

    if (!l2p_lock[unit]) {
        SHR_ERR_EXIT(SHR_E_INTERNAL);
    }

    sal_rwlock_destroy(l2p_lock[unit]);
    l2p_lock[unit] = NULL;

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Allocate memory for logical to physical port map.
 *
 * \param [in] unit Unit number.
 *
 * \retval SHR_E_NONE No errors.
 */
int
bcmpc_l2p_map_create(int unit)
{
    bcmpc_drv_t *pc_drv = NULL;
    uint32_t max_lports = BCMPC_NUM_LPORTS_PER_CHIP_MAX, max_pports = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    if (pc_drv && pc_drv->max_num_ports_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_drv->max_num_ports_get(unit, &max_pports, &max_lports));
    }

    SHR_ALLOC(l2p_map[unit], sizeof(bcmpc_lport_t) * max_lports, "bcmpcL2pMap");

    SHR_NULL_CHECK(l2p_map[unit], SHR_E_MEMORY);
    sal_memset(&l2p_map[unit][0], 0, sizeof(bcmpc_lport_t) * max_lports);

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Destroy l2p_map buffer.
 *
 * \param [in] unit Unit number.
 *
 * \retval Nothing.
 */
void
bcmpc_l2p_map_destroy(int unit)
{

    if (l2p_map[unit]) {
        SHR_FREE(l2p_map[unit]);
        l2p_map[unit] = NULL;
    }
    return;
}
