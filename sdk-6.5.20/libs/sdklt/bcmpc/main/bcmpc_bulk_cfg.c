
/*! \file bcmpc_bulk_cfg.c
 *
 * BCMPC bulk configuration interfaces.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#include <bsl/bsl.h>
#include <shr/shr_debug.h>
#include <shr/shr_util.h>
#include <bcmpc/bcmpc_pmgr_internal.h>
#include <bcmpc/bcmpc_cfg_internal.h>
#include <bcmpc/bcmpc_util_internal.h>
#include <bcmpc/bcmpc_drv_internal.h>
#include <bcmdrd/bcmdrd_dev.h>

/*******************************************************************************
 * Local definitions
 */

/* Debug log target definition */
#define BSL_LOG_MODULE BSL_LS_BCMPC_PM

/*! PC configuration mode - per-port based or bulk configuration mode. */
static bool pc_port_based_cfg[BCMDRD_CONFIG_MAX_UNITS];

/*******************************************************************************
 * Private functions
 */

/*!
 * Insert the PC_PORT_INFOt LT with the operation status errors
 * for logical port configuration errors.
 *
 * \param [in] unit Unit number.
 * \param [in] bcfg Bulk logcial port configuration data structure,
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
bcmpc_port_oper_status_update(int unit, bcmpc_bulk_cfg_info_t *bcfg)
{
    bcmpc_port_oper_status_t op_st;
    bcmpc_drv_t *pc_drv = NULL;
    uint32_t max_lports = 0, max_pports = 0;
    uint32_t lport_iter = 0;
    int rv1, rv;
    bool en_debug = FALSE;

    SHR_FUNC_ENTER(unit);

    rv = SHR_E_NONE;
    rv1 = SHR_E_NONE;

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    if (pc_drv && pc_drv->max_num_ports_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_drv->max_num_ports_get(unit, &max_pports, &max_lports));
    } else {
        /*
         * TBD: If there is no implementation to get the maximum number
         * of logical and physical ports, then iterate through the
         * PC_PHYS_PORT LT to get the number of physical ports and
         * iterate through the PC_PORT_PHYS_MAP LT to get the number of
         * logical ports.
         */
        max_lports = max_pports = 260;
    }

    for (lport_iter = 0; lport_iter < max_lports; lport_iter++) {
        /* Check if logical port is marked as invalid. */
        if ((bcfg->lport_valid[lport_iter]) ||
            (bcfg->l2p_buf[lport_iter] == BCMPC_INVALID_PPORT)) {
            continue;
        }

        if (en_debug) {
            LOG_CLI((BSL_META("Updating PC_PORT_INFO [%d]\n"), lport_iter));
        }
        sal_memset(&op_st, 0, sizeof(bcmpc_port_oper_status_t));

        /*
         * Get the logical port from physical port for which the
         * operation failed.
         */
         rv = (bcmpc_db_imm_entry_lookup(unit, PC_PORT_INFOt,
                                         sizeof(bcmpc_lport_t), &lport_iter,
                                         sizeof(bcmpc_port_oper_status_t),
                                         &op_st));

         op_st.status = PM_PORT_INVALID;
         if (rv == SHR_E_NOT_FOUND) {
                rv1 = bcmpc_db_imm_entry_insert(unit, PC_PORT_INFOt,
                                             sizeof(bcmpc_lport_t), &lport_iter,
                                             sizeof(bcmpc_port_oper_status_t),
                                             &op_st);
         } else {
                rv1 = bcmpc_db_imm_entry_update(unit, PC_PORT_INFOt,
                                             sizeof(bcmpc_lport_t), &lport_iter,
                                             sizeof(bcmpc_port_oper_status_t),
                                             &op_st);
         }
    }

    if ( rv1 != SHR_E_NONE ) {
       SHR_ERR_EXIT(rv1);
    }

exit:
    SHR_FUNC_EXIT();
}

static int
pc_print_pm_cfg_info(int unit, bcmpc_topo_t *topo_info,
                     bcmpc_topo_type_t *type_info, bool en_debug)
{

    if (en_debug == FALSE) {
        return SHR_E_NONE;
    }

    LOG_CLI(("Topo Configuration.\n"));

    LOG_CLI((BSL_META("id[%d]-bpp[%d]\n"), topo_info->tid,
                                           topo_info->base_pport));
    LOG_CLI((BSL_META("nm[%s]-nl[%d]-np[%d]-npll[%d]-pllidx[%d]\n"),
                       type_info->name, type_info->num_lanes,
                       type_info->num_ports, type_info->num_plls,
                       type_info->tvco_pll_index));

    return SHR_E_NONE;
}

static int
bcmpc_port_config_apply(int unit, bcmpc_bulk_cfg_info_t *bcfg)
{
    bcmpc_drv_t *pc_drv = NULL;
    uint32_t max_lports = 0, max_pports = 0;
    uint32_t lport_iter = 0;
    bool is_internal = FALSE;
    bool en_debug = FALSE;

    SHR_FUNC_ENTER(unit);


    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    if (pc_drv && pc_drv->max_num_ports_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_drv->max_num_ports_get(unit, &max_pports, &max_lports));
    } else {
        /*
         * TBD: If there is no implementation to get the maximum number
         * of logical and physical ports, then iterate through the
         * PC_PHYS_PORT LT to get the number of physical ports and
         * iterate through the PC_PORT_PHYS_MAP LT to get the number of
         * logical ports.
         */
        max_lports = max_pports = 260;
    }

    for (lport_iter = 0; lport_iter < max_lports; lport_iter++) {
        /* Check if logical port is marked as invalid. */
        if (!bcfg->lport_valid[lport_iter] ||
            (bcfg->l2p_buf[lport_iter] == BCMPC_INVALID_PPORT)) {

            if (en_debug) {
                LOG_CLI((BSL_META("Skip lport->pport map [%d]\n"), lport_iter));
            }
            continue;
        } else {
            if (en_debug) {
                LOG_CLI((BSL_META("Configure lport->pport map [%d]\n"),
                                 lport_iter));
            }
        }

        /* Check if Logical port configuration is present, PC_PORTt entry. */
        if (bcfg->pcfg[lport_iter] == NULL) {
            if (en_debug) {
                LOG_CLI((BSL_META("Skip lport[%d], PC_PORTt entry missing\n"),
                                 lport_iter));
            }
            continue;
        }

        SHR_IF_ERR_VERBOSE_EXIT
            (bcmpc_internal_port_cfg_insert(unit, lport_iter,
                                            bcfg->pcfg[lport_iter]));
        if (en_debug) {
            LOG_CLI((BSL_META("Configured lport [%d]\n"), lport_iter));
        }

        /* Chip driver cannot be NULL, just a protective check. */
        if (pc_drv && pc_drv->is_internal_port) {
            SHR_IF_ERR_EXIT(pc_drv->is_internal_port(unit,
                                                     bcfg->l2p_buf[lport_iter],
                                                     &is_internal));
        }

        if (is_internal != true) {
            /* Prepopulate the PC_PMD_FIRMWARE LT. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_pmd_firmware_config_prepopulate(unit, lport_iter,
                                               bcfg->pcfg[lport_iter], FALSE));

            /* Prepopulate the PC_MAC_CONTROL LT. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_mac_control_config_prepopulate(unit, lport_iter,
                                               bcfg->pcfg[lport_iter], FALSE));

            /* Prepopulate the PC_PHY_CONTROL LT. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_phy_control_config_prepopulate(unit, lport_iter,
                                               bcfg->pcfg[lport_iter], FALSE));

            /* Prepopulate the PC_PFC LT. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_pfc_config_prepopulate(unit, lport_iter,
                                               bcfg->pcfg[lport_iter], FALSE));

            /* Prepopulate the PC_PORT_TIMESYNC LT. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_timesync_config_prepopulate(unit, lport_iter,
                                               bcfg->pcfg[lport_iter], FALSE));
        } else {
            /* Delete the LT entries created for the special ports. */
            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_pmd_firmware_config_delete(unit, lport_iter));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_mac_control_config_delete(unit, lport_iter));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_phy_control_config_delete(unit, lport_iter));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_pfc_config_delete(unit, lport_iter));

            SHR_IF_ERR_VERBOSE_EXIT
                (bcmpc_timesync_config_delete(unit, lport_iter));
        }
    }

exit:
    SHR_FUNC_EXIT();
}

static int
bcmpc_core_config_apply(int unit, bcmpc_pport_t pport, int pm_id,
                        pm_vco_setting_t *vco_cfg)
{
    bcmpc_pm_core_t pm_core;
    bcmpc_pm_core_cfg_t ccfg;

    SHR_FUNC_ENTER(unit);

    bcmpc_pm_core_cfg_t_init(&ccfg);

    pm_core.pm_id = pm_id;
    pm_core.core_idx = 0;

    SHR_IF_ERR_EXIT
        (bcmpc_pm_core_config_get(unit, pm_core, &ccfg));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pm_fw_load_config_get(unit, &ccfg.pm_fw_cfg));

    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pmgr_pm_core_attach(unit, pport, vco_cfg, NULL, &ccfg));

exit:
    SHR_FUNC_EXIT();
}

static int
bcmpc_pm_vcos_get(int unit, int pm_id, bcmpc_topo_t *topo_info,
                  void *cookie)
{
    bcmpc_drv_t *pc_drv = NULL;
    bcmpc_bulk_cfg_info_t *cfg = cookie;
    pm_vco_setting_t vco_cfg;
    pm_speed_config_t *lports_speed_cfg = NULL;
    int *lport_start_lane = NULL;
    bcmpc_topo_type_t topo_type_info;
    bcmpc_pport_t pport;
    bcmpc_lport_t lport = BCMPC_INVALID_LPORT;
    bcmpc_lport_t prev_lport = BCMPC_INVALID_LPORT;
    int num_pm_ports = 0, pport_iter = 0;
    int spd_idx = 0;
    bool is_internal = FALSE;
    bool en_debug = FALSE;

    SHR_FUNC_ENTER(unit);

    if (en_debug) {
        LOG_CLI((BSL_META("PM_ID[%d]\n"), pm_id));
    }

    sal_memset(&vco_cfg, 0, sizeof(vco_cfg));
    /* Do not process CPU and LOOPBACK Port Macro types. */

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    if (pc_drv && pc_drv->is_internal_pm) {
        SHR_IF_ERR_EXIT
            (pc_drv->is_internal_pm(unit, pm_id, &is_internal));
    }

    /* Return for internal CPU and LB Port Macro types. */
    if (is_internal) {
        if (en_debug) {
            LOG_CLI((BSL_META("Internal PM_ID[%d]\n"), pm_id));
        }
        SHR_EXIT();
    }

    /* Get Port Macro properties. */
    SHR_IF_ERR_EXIT
        (bcmpc_topo_type_get(unit, topo_info->tid, &topo_type_info));

    SHR_IF_ERR_EXIT
        (pc_print_pm_cfg_info(unit, topo_info, &topo_type_info, FALSE));

    /*
     * Use base_pport and number of physical ports from the topology
     * information to derive the logical ports configured on the Port Macro.
     * Get the logical port configuration from the bulk configuration
     * structure using logical port number as index.
     * If the number of PLLs supported by the Port Macro is greater than 1
     * derive the primary and secondary VCO's rates to be configured on
     * SERDES core, using the configuration of all the logical ports
     * configured on that Port Macro.
     */
    num_pm_ports = topo_type_info. num_ports;

    /* Allocate memory for storing the logical port speed configuration. */
    SHR_ALLOC(lports_speed_cfg, sizeof(pm_speed_config_t) * num_pm_ports,
              "bcmpcTmpLportSpdCfg");
    SHR_NULL_CHECK(lports_speed_cfg, SHR_E_MEMORY);

    SHR_ALLOC(lport_start_lane, sizeof(int) * num_pm_ports,
              "bcmpcTmpLportStartLane");
    SHR_NULL_CHECK(lport_start_lane, SHR_E_MEMORY);

    /*
     * Following loop iterates through the logical ports configured on PM
     * retrieves the speed, num lanes, fec, starting physical lane for
     * each logical port to construct the list of speeds that need to be
     * supported by the Port Macro.
     */
    for (pport_iter = 0; pport_iter < num_pm_ports; pport_iter++) {
        /*
         * Get the associated logical port for the base_pport.
         * Skip if not configured or marked as not valid (configuration
         * error).
         */
        pport = topo_info->base_pport + pport_iter;
        lport = cfg->p2l_buf[pport];

        /*
         * Skip if
         * logical port not configured (invalid) for the physical port.
         * logical port configuration NULL, PC_PORT entry null.
         * logical port is marked as invalid.
         */
        if ((lport == BCMPC_INVALID_PPORT) || (cfg->pcfg[lport] == NULL) ||
            (cfg->lport_valid[lport] == FALSE)) {
            if (en_debug)  {
                LOG_CLI((BSL_META("Skip PM_ID[%d]-Bpp[%d]\n"), pm_id, pport));
            }
            continue;
        }

        /* For multi-lane logical port skip this physical lane. */
        if (prev_lport == lport) {
            if (en_debug) {
                LOG_CLI((BSL_META("Skip MLP PM_ID[%d]-lp[%d]-pp[%d]\n"),
                                  pm_id, lport, pport));
            }
            continue;
        }


        /*
         * Populate the speed configuration structure for the logical ports
         * that belong to this Port Macro.
         */
        lports_speed_cfg[spd_idx].speed = cfg->pcfg[lport]->speed;
        lports_speed_cfg[spd_idx].num_lanes = cfg->pcfg[lport]->num_lanes;
        lports_speed_cfg[spd_idx].fec = cfg->pcfg[lport]->fec_mode;

        /* Get the first physical lane of the logcial port. */
        lport_start_lane[spd_idx] = (cfg->l2p_buf[lport] - 1) % num_pm_ports;

        if (en_debug) {
            LOG_CLI((BSL_META("spd_idx[%d]-pp[%d]-"
                              "lp[%d]-sl[%d]-sp[%d]-nl[%d]-f[%d]\n"),
                           spd_idx, pport, lport, lport_start_lane[spd_idx],
                           lports_speed_cfg[spd_idx].speed,
                           lports_speed_cfg[spd_idx].num_lanes,
                           lports_speed_cfg[spd_idx].fec));
        }
        spd_idx++;
        prev_lport = lport;
    }

    /*
     * If spd_idx is 0, means no active logical ports configured on this
     * Port macro, skip the PM core init.
     */

    if (!spd_idx) {
        if (en_debug) {
            LOG_CLI((BSL_META("No logical ports configured, ")));
            LOG_CLI((BSL_META("Skip PM_ID[%d]-Bpp[%d]\n"), pm_id, pport));
        }
        SHR_EXIT();
    }

    /*
     * Now call the Port Macro driver code with the above speed list database
     * to calculate the VCO rate required to initialize the SERDES core.
     */
    vco_cfg.num_speeds = spd_idx;
    vco_cfg.speed_config_list = lports_speed_cfg;
    vco_cfg.port_starting_lane_list = lport_start_lane;

    SHR_IF_ERR_EXIT
        (bcmpc_pmgr_pm_vco_rate_calculate(unit, pport, &vco_cfg));
    if (en_debug) {
        LOG_CLI((BSL_META("PM_ID[%d]-tvco[%d]-ovco[%d]\n"), pm_id,
                          vco_cfg.tvco, vco_cfg.ovco));
    }

    /*
     * Now we have Core PLL information, Logical port configuration,
     * Logical to physical port map.
     * Start applying the configuration to hardware.
     */

    SHR_IF_ERR_EXIT
        (bcmpc_core_config_apply(unit, pport, pm_id, &vco_cfg));

exit:

    if (lports_speed_cfg != NULL) {
        SHR_FREE(lports_speed_cfg);
    }

    if (lport_start_lane != NULL) {
        SHR_FREE(lport_start_lane);
    }
    SHR_FUNC_EXIT();

}

/*!
 * \brief Bulk configuration populate function.
 *
 * This function is called for each logical port to physical port mapping
 * by traversing the PC_PORT_PHYS_MAP lt. Using the logical port as index
 * the PC_PORT LT is looked up to get the port configuration. This information
 * is stored in the bulk_config structure.
 *
 * \param [in] unit Unit number.
 * \param [in] key_size Key size, i.e. sizeof(bcmpc_lport_t).
 * \param [in] key The logical port number, bcmpc_lport_t.
 * \param [in] data_size Data size, i.e. sizeof(bcmpc_port_cfg_t).
 * \param [in] data The logical port configuration, bcmpc_port_cfg_t.
 * \param [in] cookie pc_bulk_cfg_info_t structure.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pc_bulk_port_config_fill(int unit, size_t key_size, void *key,
                         size_t data_size, void *data,
                         void *cookie)
{
    bcmpc_lport_t lport = *(bcmpc_lport_t *)key;
    bcmpc_pport_t base_pport = BCMPC_INVALID_PPORT;
    bcmpc_port_cfg_t *pcfg = data;
    bcmpc_bulk_cfg_info_t *cfg = cookie;
    bcmpc_drv_t *pc_drv = NULL;
    int iter = 0;
    bool en_debug = FALSE;
    int num_phys_ports = 0;

    SHR_FUNC_ENTER(unit);

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));

    /* First check if this logical port is valid. */
    if (!cfg->lport_valid[lport]) {
        if (en_debug) {
            LOG_CLI((BSL_META("Logical port[%d] marked as invalid\n"), lport));
        }
        SHR_EXIT();
    }

    /* Logical port is valid, get the port configuration. */

    /* Allocate memory for logical port configuration. */
    SHR_ALLOC(cfg->pcfg[lport], sizeof(bcmpc_port_cfg_t),
              "bcmpcTmpPortCfg");
    SHR_NULL_CHECK(cfg->pcfg[lport], SHR_E_MEMORY);

    sal_memcpy(cfg->pcfg[lport], pcfg, sizeof(bcmpc_port_cfg_t));
    cfg->pcfg[lport]->lport = lport;
    cfg->pcfg[lport]->pport = cfg->l2p_buf[lport];
    cfg->pcfg[lport]->is_new = TRUE;
    base_pport = cfg->l2p_buf[lport];

    /*
     * Update the physical to logical port map for multi-lane logical ports.
     * Update only if physical to logical port map is currenlty invalid.
     * else mark both, the exisiting logical port in physical to logical port
     * map and the current logical port to be assigned.
     */
    if (cfg->pcfg[lport]->num_lanes > 1) {
        if (pc_drv && pc_drv->num_phys_ports_get) {
            /*
             * For some devices, number of physical ports is different from
             * number of physical lanes. Hence, call device specific function
             * to get the correct number of physical port for the given
             * logical port.
             */
            pc_drv->num_phys_ports_get(unit, cfg->pcfg[lport]->pport,
                                  cfg->pcfg[lport]->num_lanes, &num_phys_ports);
        } else {
            num_phys_ports = cfg->pcfg[lport]->num_lanes;
        }
        for (iter = 0; iter < num_phys_ports; iter++) {
            if ((cfg->p2l_buf[base_pport + iter] == BCMPC_INVALID_LPORT) ||
                (cfg->p2l_buf[base_pport + iter] == lport)) {
                cfg->p2l_buf[base_pport + iter] = lport;
            } else {
                /*
                 * First mark the logical port existing in the p2l_map
                 * as invalid. Then mark the current logical port as
                 * invalid.
                 */
                cfg->lport_valid[cfg->p2l_buf[base_pport + iter]] = FALSE;
                cfg->lport_valid[lport] = FALSE;
                cfg->config_error = TRUE;
                if (en_debug) {
                    LOG_CLI((BSL_META("Lports[%d][%d] marked as invalid\n"),
                                 cfg->p2l_buf[base_pport + iter], lport));
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT();
}

/*!
 * \brief Traverse function for populating logical to physical port
 *        physical port to logical buffer.
 *
 * This function is called for each logical port to physical port mapping
 * by traversing the logical port map table.
 *
 * \param [in] unit Unit number.
 * \param [in] key_size Key size, i.e. sizeof(bcmpc_lport_t).
 * \param [in] key The logical port number, bcmpc_lport_t.
 * \param [in] data_size Data size, i.e. sizeof(bcmpc_port_cfg_t).
 * \param [in] data The logical port configuration, bcmpc_port_cfg_t.
 * \param [in] cookie Structure for bcmpc_bulk_cfg_info_t.
 *
 * \retval SHR_E_NONE No errors.
 * \retval SHR_E_FAIL Failure.
 */
static int
pc_populate_port_map_buffer(int unit, size_t key_size, void *key,
                            size_t data_size, void *data,
                            void *cookie)
{
    bcmpc_lport_t lport = *(bcmpc_lport_t *)key;
    bcmpc_pport_t pport = *(bcmpc_pport_t *)data;
    bcmpc_bulk_cfg_info_t *cfg = cookie;

    SHR_FUNC_ENTER(unit);

    cfg->l2p_buf[lport] = pport;

    if (cfg->p2l_buf[pport] != BCMPC_INVALID_LPORT) {
        /*
         * A physical port can be mapped to only one logical port.
         * Multiple physical ports can map to the same logical port
         */
        LOG_ERROR(BSL_LOG_MODULE,
                (BSL_META_U(unit, "Pport[%d] already mapped "
                 "to Lport[%d] - new Lport[%d]!!\n"),
                  cfg->p2l_buf[pport], pport, lport));
        /*
         * Mark the logical ports as invalid in PC_PORT_INFO LT.
         */
        cfg->lport_valid[cfg->p2l_buf[pport]] = FALSE;
        cfg->lport_valid[lport] = FALSE;
        cfg->config_error = TRUE;

    } else {
        cfg->p2l_buf[pport] = lport;
    }

    cfg->lport_valid[lport] = 1;
    SHR_FUNC_EXIT();
}

static int
pc_print_cfg_info(int unit, bcmpc_bulk_cfg_info_t *cfg, bool en_debug)
{
    bcmpc_drv_t *pc_drv = NULL;
    uint32_t max_lports = 0, max_pports = 0;
    uint32_t iter = 0, max_iter = 0, i = 0;

    SHR_FUNC_ENTER(unit);

    if (en_debug != TRUE) {
        return SHR_E_NONE;
    }

    LOG_CLI(("Bulk Configuration.\n"));

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));


    if (pc_drv && pc_drv->max_num_ports_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_drv->max_num_ports_get(unit, &max_pports, &max_lports));
    } else {
        /*
         * TBD: If there is no implementation to get the maximum number
         * of logical and physical ports, then iterate through the
         * PC_PHYS_PORT LT to get the number of physical ports and
         * iterate through the PC_PORT_PHYS_MAP LT to get the number of
         * logical ports.
         */
        max_lports = max_pports = 260;
    }

    max_iter = (max_pports > max_lports)? max_pports: max_lports;

    for (iter = 0; iter < max_iter; iter++) {
        if (iter < max_lports) {
            if (cfg->l2p_buf[iter] != BCMPC_INVALID_LPORT) {
                LOG_CLI((BSL_META("\nl2p: %d -> %d"), iter, cfg->l2p_buf[iter]));
                i = cfg->l2p_buf[iter] + 1;
                while (cfg->p2l_buf[i] == iter) {
                    LOG_CLI((BSL_META(" %d"), cfg->p2l_buf[i]));
                    i++;
                }
            }
        }

        if (!cfg->lport_valid[iter] || (iter > max_lports)) {
            continue;
        }

        LOG_CLI((BSL_META("pcfg[%d] -> %p\n"), iter, (void *)cfg->pcfg[iter]));


        LOG_CLI((BSL_META("lp[%d]-sl[%d]-s[%d]-l[%d]-f[%x]-En[%d]-lt[%d]-lb[%d]\n"),
                 cfg->pcfg[iter]->lport, cfg->pcfg[iter]->pport,
                 cfg->pcfg[iter]->speed, cfg->pcfg[iter]->num_lanes,
                 cfg->pcfg[iter]->fec_mode, cfg->pcfg[iter]->encap_mode,
                 cfg->pcfg[iter]->link_training, cfg->pcfg[iter]->lpbk_mode));

    }
exit:
    SHR_FUNC_EXIT();
}

/*******************************************************************************
 * Internal Public functions
 */

/*******************************************************************************
 * Public functions
 */

int
bcmpc_pc_port_based_config_set(int unit, bool enable)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    pc_port_based_cfg[unit] = enable;

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_pc_port_based_config_get(int unit, bool *enable)
{
    bcmdrd_dev_type_t dev_type;

    SHR_FUNC_ENTER(unit);

    dev_type = bcmdrd_dev_type(unit);
    if (dev_type == BCMDRD_DEV_T_NONE) {
        SHR_IF_ERR_EXIT(SHR_E_UNIT);
    }

    *enable = pc_port_based_cfg[unit];

exit:
    SHR_FUNC_EXIT();
}

int
bcmpc_config_apply(int unit)
{
    bcmpc_drv_t *pc_drv = NULL;
    bcmpc_bulk_cfg_info_t bcfg;
    bool port_based_cfg_mode = FALSE;
    uint32_t max_lports = 0, max_pports = 0;
    uint32_t iter = 0;

    SHR_FUNC_ENTER(unit);

    sal_memset(&bcfg, 0, sizeof(bcfg));

    /* Check if PC component is in per-port or bulk configuration mode. */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_pc_port_based_config_get(unit, &port_based_cfg_mode));

    if (port_based_cfg_mode) {
        LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit, "PC per-port configuration mode!!\n")));
        return (SHR_E_NONE);
    } else {
        LOG_INFO(BSL_LOG_MODULE,
                (BSL_META_U(unit, "PC Bulk configuration mode!!\n")));
    }

    /* Start PC component bulk configration. */

    /*
     * Step 1: Build temporary database for all the logical
     * ports configured in the config file.
     * 1. Get the maximum number logical ports supported on the switch.
     * 2. Allocate memory for the database, based on the number of logical
     *    ports supported.
     * 3. Populate the memory allocated with the information from PC LT's.
     */

    SHR_IF_ERR_EXIT
        (bcmpc_drv_get(unit, &pc_drv));


    if (pc_drv && pc_drv->max_num_ports_get) {
        SHR_IF_ERR_VERBOSE_EXIT
            (pc_drv->max_num_ports_get(unit, &max_pports, &max_lports));
    } else {
        /*
         * TBD: If there is no implementation to get the maximum number
         * of logical and physical ports, then iterate through the
         * PC_PHYS_PORT LT to get the number of physical ports and
         * iterate through the PC_PORT_PHYS_MAP LT to get the number of
         * logical ports.
         */
        max_lports = max_pports = 260;
    }

    /* Allocate memory for the database. */
    SHR_ALLOC((bcfg.l2p_buf), sizeof(bcmpc_lport_t) * max_lports,
              "bcmpcTmpL2pBuf");
    SHR_NULL_CHECK(bcfg.l2p_buf, SHR_E_MEMORY);

    for (iter = 0; iter < max_lports; iter++) {
        bcfg.l2p_buf[iter] = BCMPC_INVALID_LPORT;
    }

    SHR_ALLOC(bcfg.lport_valid, sizeof(char) * max_lports,
              "bcmpcLportValidBuf");
    SHR_NULL_CHECK(bcfg.lport_valid, SHR_E_MEMORY);

    for (iter = 0; iter < max_lports; iter++) {
        bcfg.lport_valid[iter] = 0;
    }

    SHR_ALLOC(bcfg.p2l_buf, sizeof(bcmpc_pport_t) * max_pports,
              "bcmpcTmpL2pBuf");
    SHR_NULL_CHECK(bcfg.p2l_buf, SHR_E_MEMORY);

    for (iter = 0; iter < max_pports; iter++) {
        bcfg.p2l_buf[iter] = BCMPC_INVALID_LPORT;
    }

    /*
     * Populate the logical to physical map buffer and physical to logical
     * buffer.
     * Traverse the existing logical ports to physical port map.
     */
     SHR_IF_ERR_EXIT
         (bcmpc_db_imm_entry_traverse(unit,
                                     PC_PORT_PHYS_MAPt,
                                     sizeof(bcmpc_lport_t),
                                     sizeof(bcmpc_pport_t),
                                     pc_populate_port_map_buffer,
                                     &bcfg));

    /* Allocate memory for logical port configuration. */
    SHR_ALLOC(bcfg.pcfg, sizeof(bcmpc_port_cfg_t *) * max_lports,
              "bcmpcTmpPortCfgPtr");
    SHR_NULL_CHECK(bcfg.pcfg, SHR_E_MEMORY);

    for (iter = 0; iter < max_lports; iter++) {
        bcfg.pcfg[iter] = NULL;
    }

    /* Read and store port configuration for the valid logical ports only. */
     SHR_IF_ERR_EXIT
         (bcmpc_db_imm_entry_traverse(unit,
                                     PC_PORTt,
                                     sizeof(bcmpc_lport_t),
                                     sizeof(bcmpc_port_cfg_t),
                                     pc_bulk_port_config_fill,
                                     &bcfg));

    if (bcfg.config_error) {
        /*
         * Populate the PC_PORT_INFO LT for the logical ports
         * marked as invalid.
         */
        bcmpc_port_oper_status_update(unit, &bcfg);
        SHR_ERR_EXIT(SHR_E_CONFIG);
    }

    /*
     * Get VCO's per Port Macro required for the current logical port
     * configuration.
     */
    SHR_IF_ERR_VERBOSE_EXIT
        (bcmpc_topo_traverse(unit, bcmpc_pm_vcos_get, &bcfg));

    SHR_IF_ERR_EXIT
        (pc_print_cfg_info(unit, &bcfg, FALSE));
    SHR_IF_ERR_EXIT
        (bcmpc_port_config_apply(unit, &bcfg));
exit:
    /* Free the allocated memory. */
    if (bcfg.l2p_buf != NULL) {
        SHR_FREE(bcfg.l2p_buf);
    }

    if (bcfg.lport_valid != NULL) {
        SHR_FREE(bcfg.lport_valid);
    }

    if (bcfg.p2l_buf != NULL) {
        SHR_FREE(bcfg.p2l_buf);
    }

    for (iter = 0; iter < max_lports; iter++) {
         if (bcfg.pcfg[iter] != NULL) {
             SHR_FREE(bcfg.pcfg[iter]);
         }
    }

    if (bcfg.pcfg != NULL) {
        SHR_FREE(bcfg.pcfg);
    }

    SHR_FUNC_EXIT();
}

