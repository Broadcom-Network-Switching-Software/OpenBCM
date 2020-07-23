/** \file fabric/fabric.c
 * 
 *
 * Fabric APIs functions for DNX. \n
 * Note - most of the code is currently ported and is located under legacy folder
 */

/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_FABRIC

#include <bcm_int/dnx_dispatch.h>

#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm_int/dnx/cosq/cosq.h>
#include <bcm_int/dnx/cosq/ingress/iqs.h>
#include <bcm_int/dnx/cosq/ingress/ipq.h>
#include <bcm_int/dnx/cosq/ingress/fmq.h>
#include <bcm_int/dnx/cosq/ingress/system_red.h>
#include <bcm_int/dnx/port/port.h>
#include <bcm_int/dnx/port/port_dyn.h>
#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx/tdm/tdm.h>
#include <bcm_int/dnx/fabric/fabric.h>
#include <bcm_int/dnx/fabric/fabric_cgm.h>
#include <bcm_int/dnx/fabric/fabric_transmit.h>
#include <bcm_int/dnx/fabric/fabric_control_cells.h>
#include <bcm_int/dnx/fabric/fabric_rci_throttling.h>
#include <bcm_int/dnx/fabric/fabric_mesh.h>
#include <bcm_int/dnx/fabric/fabric_if.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>

#include <bcm/fabric.h>

#include <soc/dnx/dbal/dbal.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_ipq.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_lane_map.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_sch.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_fabric_access.h>
#include <soc/dnx/dnx_err_recovery_manager.h>
#include <soc/dnxc/dnxc_port.h>

#include <shared/shrextend/shrextend_debug.h>
#include <soc/sand/sand_aux_access.h>

#include <shared/fabric.h>

/** See implementation */
static shr_error_e dnx_fabric_minimal_links_to_dest_init(
    int unit);

/** See implementation */
static shr_error_e dnx_fabric_delay_single_cell_in_fabric_rx_enable_set(
    int unit,
    int enable);

/** See implementation */
static shr_error_e dnx_fabric_delay_single_cell_in_fabric_rx_enable_get(
    int unit,
    int *enable);

/**
 * ALDWP offset as a result of FEs delays along the way.
 * Fap->FE1->FE2->FE3->Fap - each FE has delay of 1.
 */
#define DNX_FABRIC_ALDWP_FE_DELAY_OFFSET   (3)
/**
 * Min value the ALDWP can have (value inherited from Arad)
 */
#define DNX_FABRIC_ALDWP_MIN               (0x2)
/**
 * Max value the ALDWP can have (register's field restriction)
 */
#define DNX_FABRIC_ALDWP_MAX               (0x3f)
/**
 * The ALDWP is written to HW in units of 64 tiks
 */
#define DNX_FABRIC_ALDWP_RESOLUTION        (64)

/**
 * \brief
 *   Initialize sw_state database.
 */
static shr_error_e
dnx_fabric_sw_state_db_init(
    int unit)
{
    uint32 dest = 0;
    uint32 nof_dest = 0;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init dnx_fabric_db
     */
    SHR_IF_ERR_EXIT(dnx_fabric_db.init(unit));

    /*
     * Alloc groups
     */
    SHR_IF_ERR_EXIT(dnx_fabric_db.modids_to_group_map.alloc(unit));

    /*
     * Alloc modids for each group (destination)
     */
    nof_dest = dnx_data_fabric.mesh.nof_dest_get(unit);
    for (dest = 0; dest < nof_dest; ++dest)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_db.modids_to_group_map.modids.alloc(unit, dest));
    }

    /*
     * Alloc links
     */
    SHR_IF_ERR_EXIT(dnx_fabric_db.links.alloc(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize fabric ALUWP (Active-Link-Up-Watchdog-Period).
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Called as part of the initialization sequence.
 * \see
 *   * None.
 */
static shr_error_e
dnx_fabric_aluwp_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * ALUWP - Active-Link-Up-Watchdog-Period
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_LINK_WATCHDOG, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ALUWP, INST_SINGLE, 0xfd);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize fabric multicast.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Called as part of the initialization sequence.
 * \see
 *   * None.
 */
static shr_error_e
dnx_fabric_multicast_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MULTICAST_CONFIG, &entry_handle_id));

    /*
     * Enable Fabric Multicast
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, 1);

    /*
     * Set all-reachable vector calculation rate
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RATE, INST_SINGLE, 0x1800);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_aldwp_config(
    int unit)
{
    uint32 entry_handle_id;
    uint32 max_cell_size = 0;
    uint32 aldwp = 0;
    uint32 highest_aldwp = 0;
    uint32 core_clock_speed_khz = 0;
    bcm_port_t logical_port = 0;
    int enable = 0, speed, has_speed;
    bcm_pbmp_t fabric_ports_bitmap;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Calc the max number of clock ticks to receive 3 cells:
     *
     *          3 *  max cell size * core clock
     *  aldwp =   ------------------------------
     *                   link rate
     */

    /*
     * Reterive global configuration
     */
    max_cell_size = dnx_data_fabric.cell.vsc256_max_size_get(unit);
    core_clock_speed_khz = DNXCMN_CORE_CLOCK_KHZ_GET(unit);

    /*
     * Find the highest aldwp
     */
    highest_aldwp = 0;
    BCM_PBMP_CLEAR(fabric_ports_bitmap);
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_FABRIC, 0, &fabric_ports_bitmap));
    BCM_PBMP_ITER(fabric_ports_bitmap, logical_port)
    {
        /*
         * Skip disabled ports
         */
        SHR_IF_ERR_EXIT(bcm_dnx_port_enable_get(unit, logical_port, &enable));
        if (!enable)
        {
            continue;
        }

        /*
         * Skip disabled ports
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_has_speed(unit, logical_port, 0, &has_speed));
        if (has_speed)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, logical_port, 0, &speed));
        }
        else
        {
            continue;
        }

        /*
         * Calc the max number of tiks to receive 3 cells.
         * Units calculation: [Bytes] * [KClocks/Sec] / [MBytes/Sec] = [miliClocks]
         */
        aldwp = (3 * max_cell_size * core_clock_speed_khz) / (speed / 8 /* bits to bytes */ );  /* mili tiks */
        /** The devision with 1024 is to convert militiks->tiks units */
        aldwp = aldwp / (1024 * DNX_FABRIC_ALDWP_RESOLUTION) + ((aldwp % (1024 * DNX_FABRIC_ALDWP_RESOLUTION) != 0) ? 1 : 0)    /* round
                                                                                                                                 * up */ ;
        /** Update MAX */
        highest_aldwp = (aldwp > highest_aldwp ? aldwp : highest_aldwp);
    }

    /** +3 to be on the safe side with possible delays from fe2/fe3 */
    if (highest_aldwp != 0 /** at least one port is enabled */ )
    {
        highest_aldwp += DNX_FABRIC_ALDWP_FE_DELAY_OFFSET;
    }

    /*
     * Check highest_aldwp values
     */
    if (highest_aldwp == 0)
    {
        /** All port are disabled or powered down */
        SHR_EXIT();
    }
    else if (highest_aldwp < DNX_FABRIC_ALDWP_MIN)
    {
        /** hightest_aldwp may be lower than DNX_FABRIC_ALDWP_MIN */
         /* coverity[dead_error_line:FALSE]  */
        highest_aldwp = DNX_FABRIC_ALDWP_MIN;
    }
    else if (highest_aldwp > DNX_FABRIC_ALDWP_MAX)
    {
        highest_aldwp = DNX_FABRIC_ALDWP_MAX;
    }

    /** Configure ALDWP */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_LINK_WATCHDOG, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ALDWP, INST_SINGLE, highest_aldwp);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set number of pipes configurations
 */
static shr_error_e
dnx_fabric_pipes_nof_init(
    int unit)
{
    uint32 max_pipe_id = dnx_data_fabric.pipes.nof_pipes_get(unit) - 1;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_PIPES, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_PIPE_ID, INST_SINGLE, max_pipe_id);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set pipes mapping configurations
 */
static shr_error_e
dnx_fabric_pipes_mapping_init(
    int unit)
{
    uint32 entry_handle_id;
    const dnx_data_fabric_pipes_map_t *fabric_pipe_map = dnx_data_fabric.pipes.map_get(unit);
    int fabric_priority = 0;
    int nof_fabric_priorities = dnx_data_fabric.cell.nof_priorities_get(unit);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Configure pipe mapping
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_PIPES_MAPPING, &entry_handle_id));
    for (fabric_priority = 0; fabric_priority < nof_fabric_priorities; ++fabric_priority)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_PIPES_MAPPING, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAST, DBAL_ENUM_FVAL_TRAFFIC_CAST_UC);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_PRIORITY, fabric_priority);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PIPE_ID, INST_SINGLE,
                                     fabric_pipe_map->uc[fabric_priority]);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_PIPES_MAPPING, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CAST, DBAL_ENUM_FVAL_TRAFFIC_CAST_MC);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_PRIORITY, fabric_priority);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PIPE_ID, INST_SINGLE,
                                     fabric_pipe_map->mc[fabric_priority]);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set DTQ mode according to pipes configuration
 */
static shr_error_e
dnx_fabric_pipes_dtq_mode_init(
    int unit)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_fabric_dtq_mode_e dtq_mode = DBAL_NOF_ENUM_FABRIC_DTQ_MODE_VALUES;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Configure DTQ mode.
     * Note that TDM traffic goes straight to FDT (bypass) without
     * entering DTQs, so DTQ_MODE ignores TDM.
     */
    switch (dnx_data_fabric.dtqs.nof_active_dtqs_get(unit))
    {
        case 3:
        {
            dtq_mode = DBAL_ENUM_FVAL_FABRIC_DTQ_MODE_UC_HPMC_LPMC;
            break;
        }
        case 2:
        {
            dtq_mode = DBAL_ENUM_FVAL_FABRIC_DTQ_MODE_UC_MC;
            break;
        }
        case 1:
        {
            dtq_mode = DBAL_ENUM_FVAL_FABRIC_DTQ_MODE_SINGLE_QUEUE;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Number of DTQs is not supported");
            break;
        }
    }

    /*
     * Set DTQ mode
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_PIPES_DTQ_MODE, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DTQ_MODE, INST_SINGLE, dtq_mode);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set multi-pipe system configurations
 */
static shr_error_e
dnx_fabric_pipes_multi_pipe_system_init(
    int unit)
{
    uint32 o_size = 0;
    uint32 g_size = 0;
    int nof_pipes = dnx_data_fabric.pipes.nof_pipes_get(unit);
    int multi_pipe_system = dnx_data_fabric.pipes.multi_pipe_system_get(unit);
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_PIPES_MULTI_PIPES_SYSTEM, &entry_handle_id));

    o_size = ((nof_pipes > 1) || (multi_pipe_system == TRUE)) ? 0x2 : 0x0;
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_O_SIZE, INST_SINGLE, o_size);

    g_size = dnx_data_fabric.debug.mesh_topology_size_get(unit);
    if (g_size == -1)
    {
        g_size = o_size;
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_G_SIZE, INST_SINGLE, g_size);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set TDM pipes configurations
 */
static shr_error_e
dnx_fabric_pipes_tdm_init(
    int unit)
{
    uint32 entry_handle_id;
    int tdm_pipe;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    switch (dnx_data_fabric.pipes.map_get(unit)->type)
    {
        case soc_dnxc_fabric_pipe_map_dual_tdm_non_tdm:
        {
            tdm_pipe = 1;
            break;
        }
        case soc_dnxc_fabric_pipe_map_triple_uc_mc_tdm:
        {
            tdm_pipe = 2;
            break;
        }
        default:
        {
            tdm_pipe = -1;
            break;
        }
    }

    if (tdm_pipe != -1)
    {
        /*
         * set TDM pipe
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_PIPES_TDM, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PIPE_ID, tdm_pipe);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * set strict priority to TDM pipe
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_PIPES_STRICT_PRIORITY, entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PIPE_ID, INST_SINGLE, tdm_pipe);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialization of fabric pipes
 */
static shr_error_e
dnx_fabric_pipes_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Configure nof pipes
     */
    SHR_IF_ERR_EXIT(dnx_fabric_pipes_nof_init(unit));

    /*
     * Configure pipe mapping
     */
    SHR_IF_ERR_EXIT(dnx_fabric_pipes_mapping_init(unit));

    /*
     * Configure DTQ mode
     */
    SHR_IF_ERR_EXIT(dnx_fabric_pipes_dtq_mode_init(unit));

    /*
     * Set multi-pipe system configurations
     */
    SHR_IF_ERR_EXIT(dnx_fabric_pipes_multi_pipe_system_init(unit));

    /*
     * Set pipes TDM configurations
     */
    SHR_IF_ERR_EXIT(dnx_fabric_pipes_tdm_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize fabric priority mapping
 */
static shr_error_e
dnx_fabric_priority_map_init(
    int unit)
{
    uint32 entry_handle_id;
    int is_hp = 0;
    int is_mc = 0;
    int dp = 0;
    int tc = 0;
    int is_ocb_only = 0;
    int min_hp_mc = dnx_data_fabric.pipes.map_get(unit)->min_hp_mc;
    uint32 fabric_priority = 0;
    uint32 is_tdm = 0;
    soc_dnxc_fabric_pipe_map_type_t map_type = dnx_data_fabric.pipes.map_get(unit)->type;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CELL_PRIORITY, &entry_handle_id));

    /*
     * Loop over all table indices
     */
    for (is_hp = 0; is_hp <= 1; ++is_hp)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_CELL_PRIORITY, entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_HP, is_hp);

        for (is_mc = 0; is_mc <= 1; ++is_mc)
        {
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, is_mc);

            for (dp = 0; dp < DNX_COSQ_NOF_DP; ++dp)
            {
                dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);

                for (tc = 0; tc < DNX_COSQ_NOF_TC; ++tc)
                {
                    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);

                    for (is_ocb_only = 0; is_ocb_only <= 1; ++is_ocb_only)
                    {
                        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_OCB_ONLY, is_ocb_only);

                        if ((dnx_data_tdm.params.mode_get(unit) == TDM_MODE_PACKET) && is_ocb_only)
                        {
                            /*
                             * Packet-TDM traffic use OCB-Only queues
                             */
                            fabric_priority = dnx_data_fabric.tdm.priority_get(unit);
                            is_tdm = 1;
                        }
                        else if (map_type == soc_dnxc_fabric_pipe_map_triple_uc_hp_mc_lp_mc)
                        {
                            /*
                             * fabric_priority must be fully aligned with hp_bit since dtq uses hp_bit to decide queue id while FDR
                             * uses fabric_priority to decide pipe id.  Therefore, in "uc / hp mc / lp mc" configuration => LP bit ==>
                             * priority 0 (= lowest priority of lp mc) HP bit ==> lowest priority of hp mc
                             */
                            fabric_priority = is_hp ? min_hp_mc : 0;
                            is_tdm = 0;
                        }
                        else
                        {
                            /*
                             * tc=0,1,2 ==> prio=0
                             * tc=3,4,5 ==> prio=1
                             * tc=6,7   ==> prio=2
                             */
                            fabric_priority = tc / 3;
                            is_tdm = 0;
                        }

                        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_PRIORITY, INST_SINGLE,
                                                     fabric_priority);
                        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm);
                        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
                    }
                }
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize fabric TDM priority
 */
static shr_error_e
dnx_fabric_priority_tdm_init(
    int unit)
{
    int tdm_priority = dnx_data_fabric.tdm.priority_get(unit);
    int tdm_priority_disabled = dnx_data_fabric.cell.nof_priorities_get(unit);
    int tdm_header_priority;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    tdm_header_priority = (tdm_priority == -1) ? tdm_priority_disabled : tdm_priority;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_TDM_PRIORITY, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_TDM_PRIORITY, INST_SINGLE, tdm_header_priority);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize fabric priority configurations
 */
static shr_error_e
dnx_fabric_priority_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Configure priority mapping
     */
    SHR_IF_ERR_EXIT(dnx_fabric_priority_map_init(unit));

    /*
     * Configure TDM priority
     */
    SHR_IF_ERR_EXIT(dnx_fabric_priority_tdm_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize Fabric PCP (Packet Cell Packing)
 */
static shr_error_e
dnx_fabric_pcp_init(
    int unit)
{
    uint32 entry_handle_id;
    int pcp_enable;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * enable/disable fabric_pcp
     */
    pcp_enable = dnx_data_fabric.cell.pcp_enable_get(unit);
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_PCP_ENABLE, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, pcp_enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize fabric contexts interleaving.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Called as part of the initialization sequence.
 * \see
 *   * None.
 */
static shr_error_e
dnx_fabric_interleaving_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 max_nof_contexts = dnx_data_fabric.pipes.max_nof_contexts_get(unit);
    uint32 max_nof_subcontexts = dnx_data_fabric.pipes.max_nof_subcontexts_get(unit);
    uint32 nof_subcontexts_per_context = max_nof_subcontexts / max_nof_contexts;
    uint32 src_context_id = 0;
    uint32 dest_context_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Configure context interleaving:
     * Clos mode: When DTQ-context==PIPE interleaving is allowed with no restriction.
     *            When DTQ-context!=PIPE some of the interleaving should be blocked
     *            according to the configuration.
     * Mesh-UC mode: interleaving is allowed with no restriction.
     * Mesh-MC mode: Since device works is separate UC/MC pipes configuration (UC/MC
     *               or UC/MC/TDM) interleaving is allowed with no restriction.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_INTERLEAVING_ENABLE, &entry_handle_id));
    for (dest_context_id = 0; dest_context_id < max_nof_subcontexts; ++dest_context_id)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_SUBCONTEXT_ID, dest_context_id);
        for (src_context_id = 0; src_context_id < max_nof_subcontexts; ++src_context_id)
        {
            /**
             * Interleaving between contexts should be enabled, except
             * for the cases depicted below.
             */
            uint32 context_interleaving_enable = 1;

            /*
             * In case we can configure interleaving between subcontexts,
             * there are some case we need to disable the interleaving.
             */
            if (max_nof_subcontexts > max_nof_contexts)
            {
                if (dnx_data_tdm.params.mode_get(unit) == TDM_MODE_PACKET)
                {
                    uint32 dtq0_ocb_context = dnx_data_fabric.pipes.dtq_subcontexts_get(unit, 0, 1)->index;
                    uint32 dtq1_ocb_context = dnx_data_fabric.pipes.dtq_subcontexts_get(unit, 1, 1)->index;

                    /**
                     * In Packet-TDM mode interleaving should be disabled
                     * between OCB contexts of DTQ0 and DTQ1, since both
                     * are destined to the TDM pipe.
                     */
                    if (((dest_context_id == dtq0_ocb_context) && (src_context_id == dtq1_ocb_context))
                        || ((dest_context_id == dtq1_ocb_context) && (src_context_id == dtq0_ocb_context)))
                    {
                        context_interleaving_enable = 0;
                    }
                }
                else
                {
                    uint32 dest_dtq_id = dest_context_id / nof_subcontexts_per_context;
                    uint32 mix_context = dnx_data_fabric.pipes.dtq_subcontexts_get(unit, dest_dtq_id, 0)->index;
                    uint32 ocb_context = dnx_data_fabric.pipes.dtq_subcontexts_get(unit, dest_dtq_id, 1)->index;

                    /**
                     * Interleaving between Mix and OCB subcontexts should be
                     * disabled unless working with TDM packet mode.
                     */
                    if ((dest_context_id == mix_context && src_context_id == ocb_context)
                        || (dest_context_id == ocb_context && src_context_id == mix_context))
                    {
                        context_interleaving_enable = 0;
                    }
                }
            }

            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SUBCONTEXT_ENABLE, src_context_id,
                                         context_interleaving_enable);
        }

        if (dnx_data_tdm.params.mode_get(unit) != TDM_MODE_NONE)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SUBCONTEXT_TDM_ENABLE, INST_SINGLE, 1);
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * set RCM (Reassembly Contexts Manager) to allocate contexts dynamically
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_REASSEMBLY_CONTEXT, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_DYNAMIC, INST_SINGLE, 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize fabric tdm contexts mapping.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Called as part of the initialization sequence.
 *   * Relevant only when TDM is supported.
 * \see
 *   * None.
 */
static shr_error_e
dnx_fabric_tdm_contexts_mapping_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_tdm.params.mode_get(unit) != TDM_MODE_NONE)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_TDM_CONTEXTS_MAPPING, &entry_handle_id));

        if (dnx_data_tdm.params.mode_get(unit) == TDM_MODE_PACKET)
        {
            uint32 dtq_0_ocb_context_id = dnx_data_fabric.pipes.dtq_subcontexts_get(unit, 0, 1)->index;
            uint32 dtq_1_ocb_context_id = dnx_data_fabric.pipes.dtq_subcontexts_get(unit, 1, 1)->index;
            uint32 dtq_2_ocb_context_id = dnx_data_fabric.pipes.dtq_subcontexts_get(unit, 2, 1)->index;

            /**
             * Packet TDM mode.
             * TDM packets will run through the OCB-Only subcontexts.
             */
            switch (dnx_data_fabric.dtqs.nof_active_dtqs_get(unit))
            {
                case 1:
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, dtq_0_ocb_context_id, 1);
                    break;
                }
                case 2:
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, dtq_0_ocb_context_id, 1);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, dtq_1_ocb_context_id, 1);
                    break;
                }
                case 3:
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, dtq_0_ocb_context_id, 1);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, dtq_1_ocb_context_id, 1);
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, dtq_2_ocb_context_id, 1);
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "For TDM, need to configure TDM pipes mapping");
                    break;
                }
            }
        }
        else
        {
            /** TDM Bypass mode */
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM_BYPASS, INST_SINGLE, 1);
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Every second, change the load balancing to count clocks instead
 *   of transactions for 1msec, and then set it back to count transactions.
 *   The seed for the LFSR is the same for the 2 clocks, which might cause
 *   the cores to be synchronized with their links choosing. In an attempt
 *   to handle this issue, we will set the counting mode of the LFSR
 *   to be by clocks for 1msec (so the links will be taken out of sync
 *   in higher probability), and then we will set back the counting mode
 *   to be by transactions.
 * \param [in] unit -
 *   The unit number.
 * \param [in] user_data -
 *   Not used.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Called as a callback to a periodic event.
 * \see
 *   * None.
 */
static shr_error_e
dnx_fabric_load_balance_periodic_event(
    int unit,
    void *user_data)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_LOAD_BALANCING, &entry_handle_id));

    /*
     * Set the LFSR to count according to clocks for 1ms.
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNT_MODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_FABRIC_LOAD_BALANCING_COUNT_MODE_CLOCKS);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    sal_usleep(1000);

    /*
     * Set back the LFSR to count according to transactions.
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNT_MODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_FABRIC_LOAD_BALANCING_COUNT_MODE_TRANSACTIONS);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See fabric.h
 */
shr_error_e
dnx_fabric_load_balance_thread_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_FE)
    {
        if (dnx_data_fabric.links.feature_get(unit, dnx_data_fabric_links_load_balancing_periodic_event_enabled))
        {
            periodic_event_config_t load_balancing_config;
            periodic_event_handler_t load_balancing_handler;

            periodic_event_config_t_init(&load_balancing_config);
            load_balancing_config.name = "fabric load balancing";
            load_balancing_config.bsl_module = BSL_LOG_MODULE;
            load_balancing_config.interval = dnx_data_fabric.links.usec_between_load_balancing_events_get(unit);
            load_balancing_config.callback = dnx_fabric_load_balance_periodic_event;
            SHR_IF_ERR_EXIT(periodic_event_create(unit, &load_balancing_config, &load_balancing_handler));
            SHR_IF_ERR_EXIT(dnx_fabric_db.load_balancing_periodic_event_handler.set(unit, load_balancing_handler));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See fabric.h
 */
shr_error_e
dnx_fabric_load_balance_thread_deinit(
    int unit)
{
    periodic_event_handler_t load_balancing_handler;
    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_FE)
    {
        if (dnx_data_fabric.links.feature_get(unit, dnx_data_fabric_links_load_balancing_periodic_event_enabled))
        {
            /*
             * Destroy the load balancing periodic event
             */
            SHR_IF_ERR_CONT(dnx_fabric_db.load_balancing_periodic_event_handler.get(unit, &load_balancing_handler));
            if (load_balancing_handler)
            {
                SHR_IF_ERR_CONT(periodic_event_destroy(&load_balancing_handler));
                SHR_IF_ERR_CONT(dnx_fabric_db.load_balancing_periodic_event_handler.set(unit, NULL));
            }
        }
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize load-balancing related configurations.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Called as part of the initialization sequence.
 * \see
 *   * None.
 */
static shr_error_e
dnx_fabric_load_balance_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_FE)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_LOAD_BALANCING, &entry_handle_id));

        /*
         * Enable switch network, the mechanism that responsible to scramble
         * the output links.
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, 1);

        /*
         * Set LFSR values, needed for switch network.
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SEED, INST_SINGLE, sal_time_usecs());
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESOLUTION, INST_SINGLE, 1000);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_COUNT_MODE, INST_SINGLE,
                                     DBAL_ENUM_FVAL_FABRIC_LOAD_BALANCING_COUNT_MODE_TRANSACTIONS);

        /*
         * Note that on JR2 there is a periodic event thread that adds more configurations.
         * This can be found in "threads" init sequence.
         */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        /*
         * Set Round-Robin pointer configurations
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_LOAD_BALANCING_RR_PTR, entry_handle_id));

        /** Enable RR pointer for each pipe */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, 1);

        if (dnx_data_tdm.params.mode_get(unit) != TDM_MODE_NONE)
        {
            /** Set RR pointer for TDM pipe */
            switch (dnx_data_fabric.pipes.map_get(unit)->type)
            {
                case soc_dnxc_fabric_pipe_map_single:
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_PIPE_ID, INST_SINGLE, 0);
                    break;
                }
                case soc_dnxc_fabric_pipe_map_dual_tdm_non_tdm:
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_PIPE_ID, INST_SINGLE, 1);
                    break;
                }
                case soc_dnxc_fabric_pipe_map_triple_uc_mc_tdm:
                {
                    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TDM_PIPE_ID, INST_SINGLE, 2);
                    break;
                }
                default:
                {
                    SHR_ERR_EXIT(_SHR_E_CONFIG, "For TDM, need to configure TDM pipes mapping");
                    break;
                }
            }
        }

        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialization of scheduler fabric links adaptation.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Called as part of the initialization sequence.
 * \see
 *   * None.
 */
static shr_error_e
dnx_fabric_scheduler_adaptation_init(
    int unit)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_SCHEDULER_ADAPTATION, &entry_handle_id));

    if (dnx_data_device.general.feature_get(unit, dnx_data_device_general_multi_core))
    {
        /*
         * Enable credit share
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_CREDITS_SHARE, INST_SINGLE, 1);
    }

    /*
     * Configure link state for SCH and FDT.
     * Link state will be a result of link integrity vector masked with
     * accessible links (links that have reachability cells from at least
     * a single device) according to the unicast table.
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_MC_LINKS_MASK, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE_ACTIVE_LINKS_MASK, INST_SINGLE, 1);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize Fabric WFQs.
 */
static shr_error_e
dnx_fabric_wfq_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 max_nof_pipes = dnx_data_fabric.pipes.max_nof_pipes_get(unit);
    uint32 nof_pipes = dnx_data_fabric.pipes.nof_pipes_get(unit);
    uint32 pipe_id = 0;
    uint32 max_nof_contexts = max_nof_pipes;
    uint32 nof_contexts = nof_pipes;
    uint32 context_id = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_pipes_ocb_wfq))
    {
        /*
         * WFQ will be affected by Mixed traffic only
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_WFQ_INIT, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_WFQ_AFFECTED_BY_OCB_TRAFFIC, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
    }

    /*
     * Enable WFQ between pipes (only between active pipes)
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_WFQ_PIPE_ENABLE, &entry_handle_id));
    for (pipe_id = 0; pipe_id < max_nof_pipes; ++pipe_id)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PIPE_ID, pipe_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE,
                                     (pipe_id < nof_pipes) ? 1 : 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /*
     * Enable WFQ between contexts (only between active contexts).
     * Note: currently contexts are equal to pipes, but in future implementation
     * we plan to separate them, so nof_contexts won't necessarily equal nof_pipes.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_FABRIC_WFQ_CONTEXT_ENABLE, entry_handle_id));
    for (context_id = 0; context_id < max_nof_contexts; ++context_id)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_ID, context_id);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE,
                                     (context_id < nof_contexts) ? 1 : 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize fabric features.
 */
static shr_error_e
dnx_fabric_features_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Init ALUWP
     */
    SHR_IF_ERR_EXIT(dnx_fabric_aluwp_init(unit));

    /*
     * Init Multicast
     */
    SHR_IF_ERR_EXIT(dnx_fabric_multicast_init(unit));

    /*
     * Init Mesh Topology
     */
    SHR_IF_ERR_EXIT(dnx_fabric_mesh_topology_init(unit));

    /*
     * Configure Fabric Pipes mapping
     */
    SHR_IF_ERR_EXIT(dnx_fabric_pipes_init(unit));

    /*
     * Configure Fabric Priority
     */
    SHR_IF_ERR_EXIT(dnx_fabric_priority_init(unit));

    /*
     * Configure packet cell packing
     */
    SHR_IF_ERR_EXIT(dnx_fabric_pcp_init(unit));

    /*
     * Configure MESH settings
     */
    SHR_IF_ERR_EXIT(dnx_fabric_mesh_init(unit));

    /*
     * Configure segmentation and interleaving
     */
    SHR_IF_ERR_EXIT(dnx_fabric_interleaving_init(unit));

    /*
     * Configure TDM contexts mapping
     */
    SHR_IF_ERR_EXIT(dnx_fabric_tdm_contexts_mapping_init(unit));

    /*
     * Configure load balance mode
     */
    SHR_IF_ERR_EXIT(dnx_fabric_load_balance_init(unit));

    /*
     * Configure Fabric WFQs
     */
    SHR_IF_ERR_EXIT(dnx_fabric_wfq_init(unit));

    /*
     * Configure flow control
     */
    SHR_IF_ERR_EXIT(dnx_fabric_cgm_init(unit));

    /*
     * Enable minimal links to destination device.
     */
    SHR_IF_ERR_EXIT(dnx_fabric_minimal_links_to_dest_init(unit));

    /*
     * Configure fabric link to core mapping.
     */
    SHR_IF_ERR_EXIT(dnx_fabric_scheduler_adaptation_init(unit));

    /*
     * Force TDM traffic
     */
    SHR_IF_ERR_EXIT(dnx_fabric_force_tdm_init(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize fabric ports.
 */
static shr_error_e
dnx_fabric_ports_init(
    int unit)
{
    int pm_id = 0;
    int quad_id = 0;
    int link_id = 0;
    int fmac_index = 0;
    dnx_algo_port_fabric_add_t info;
    int fabric_port_base = 0;
    int nof_fabric_links = 0;
    int nof_links_in_fmac = 0;
    int nof_links_in_pm = 0;
    int nof_fabric_pms = 0;
    int nof_fmacs_in_pm = 0;
    int quad_enable = 0;
    int i_link = 0;
    bcm_pbmp_t supported_links;
    bcm_pbmp_t pbmp, okay_pbmp;
    int port_enable[_SHR_PBMP_PORT_MAX] = { 0 };
    bcm_port_t logical_port = 0;
    bcm_port_t used_logical_port[_SHR_PBMP_PORT_MAX] = { 0 };
    bcm_port_resource_t *port_resource = NULL;
    uint32 num_port_resource = 0;
    int init_speed = 0;
    int tx_pam4_precoder;
    int lp_tx_precoder;
    uint32 pm_bypassed_links[1] = { 0 };
    uint32 is_bypassed = 0;
    int port_index;
    dnx_fabric_connect_mode_e fabric_connect_mode;

    soc_dnxc_lane_map_db_map_t lane2serdes[DNX_DATA_MAX_FABRIC_LINKS_NOF_LINKS];

    SHR_FUNC_INIT_VARS(unit);

    SHR_ALLOC_SET_ZERO(port_resource, (_SHR_PBMP_PORT_MAX * sizeof(bcm_port_resource_t)),
                       "PORT resource allocation", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);

    sal_memset(&info, 0, sizeof(dnx_algo_port_fabric_add_t));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(okay_pbmp);

    /*
     * Init local Fabric ports
     */
    fabric_port_base = dnx_data_port.general.fabric_port_base_get(unit);
    nof_fabric_links = dnx_data_fabric.links.nof_links_get(unit);
    nof_links_in_fmac = dnx_data_fabric.blocks.nof_links_in_fmac_get(unit);
    nof_fabric_pms = dnx_data_fabric.blocks.nof_pms_get(unit);
    nof_links_in_pm = dnx_data_fabric.blocks.nof_links_in_pm_get(unit);
    nof_fmacs_in_pm = dnx_data_fabric.blocks.nof_fmacs_in_pm_get(unit);

    supported_links = dnx_data_fabric.links.general_get(unit)->supported_links;
    fabric_connect_mode = dnx_data_fabric.general.connect_mode_get(unit);

    /** Create fabric ports bitmap */
    for (pm_id = 0; pm_id < nof_fabric_pms; ++pm_id)
    {
        /** Get PM links (octet) bypass mode */
        pm_bypassed_links[0] = dnx_data_fabric.ilkn.bypass_info_get(unit, pm_id)->links;

        for (fmac_index = 0; fmac_index < nof_fmacs_in_pm; fmac_index++)
        {
            /** Check quad enable status */
            quad_id = (pm_id * nof_fmacs_in_pm) + fmac_index;
            quad_enable = dnx_data_port.static_add.fabric_quad_info_get(unit, quad_id)->quad_enable;

            for (i_link = 0; i_link < nof_links_in_fmac; ++i_link)
            {
                /** get link bypass mode */
                is_bypassed = SHR_BITGET(pm_bypassed_links, fmac_index * nof_links_in_fmac + i_link) ? 1 : 0;

                /** calculate link-id */
                link_id = pm_id * nof_links_in_pm + fmac_index * nof_links_in_fmac + i_link;

                /** Add logical port if not in bypass and quad is enabled*/
                if (quad_enable == FALSE || !_SHR_PBMP_MEMBER(supported_links, link_id) ||
                    is_bypassed || fabric_connect_mode == DNX_FABRIC_CONNECT_MODE_SINGLE_FAP)
                {
                    SHR_IF_ERR_EXIT(dnx_fabric_if_link_force_signal_set(unit, link_id, TRUE));
                }
                else
                {
                    BCM_PBMP_PORT_ADD(pbmp, fabric_port_base + link_id);
                }
            }
        }
    }

    /** Set Lane Map DB according to dnx-data */
    for (link_id = 0; link_id < nof_fabric_links; link_id++)
    {
        lane2serdes[link_id].rx_id = dnx_data_lane_map.fabric.mapping_get(unit, link_id)->serdes_rx_id;
        lane2serdes[link_id].tx_id = dnx_data_lane_map.fabric.mapping_get(unit, link_id)->serdes_tx_id;
    }
    /** Store lane mapping info to swstate */
    SHR_IF_ERR_EXIT(dnx_algo_lane_map_set(unit, DNX_ALGO_LANE_MAP_FABRIC_SIDE, nof_fabric_links, lane2serdes));

    /*
     * Init power of all supported FSRDs. Power down unsupported FSRDs.
     */
    SHR_IF_ERR_EXIT(dnx_fabric_if_fsrd_blocks_power_init(unit));

    /*
     * Probe fabric ports
     */
    if (!BCM_PBMP_IS_NULL(pbmp))
    {
        SHR_IF_ERR_EXIT(bcm_dnx_port_probe(unit, pbmp, &okay_pbmp));
    }

    /*
     * Post-probe operations
     */
    PBMP_ITER(okay_pbmp, logical_port)
    {
#ifdef BCM_PORT_DEFAULT_DISABLE
        port_enable[logical_port] = FALSE;
#else
        port_enable[logical_port] = TRUE;
#endif /* BCM_PORT_DEFAULT_DISABLE */

        init_speed = dnx_data_port.static_add.speed_get(unit, logical_port)->val;
        if (DNXC_PORT_INVALID_SPEED == init_speed)
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "Port init speed must be configured for port %d. To disable port, set its speed to -1.",
                         logical_port);
        }
        else if (BCM_PORT_RESOURCE_DEFAULT_REQUEST == init_speed)
        {
            port_enable[logical_port] = FALSE;
        }
        else
        {
            bcm_port_resource_t *port_resource_ptr = &port_resource[num_port_resource];

            bcm_port_resource_t_init(port_resource_ptr);
            used_logical_port[num_port_resource] = logical_port;
            /*
             * get SoC properties values - speed, encoding, CL72
             * Speed must be overriden with SoC property value.
             * if fec_type and link_training soc propeties are not defined, BCM_PORT_RESOURCE_DEFAULT_REQUEST
             * will be set as their default value.
             */
            port_resource_ptr->speed = init_speed;
            port_resource_ptr->fec_type = dnx_data_port.static_add.fec_type_get(unit, logical_port)->val;
            port_resource_ptr->link_training = dnx_data_port.static_add.link_training_get(unit, logical_port)->val;
            port_resource_ptr->phy_lane_config = BCM_PORT_RESOURCE_DEFAULT_REQUEST;

            /*
             * get default recommended encoding, CL72, lane_config based on what we got from SoC properties above
             * (fill values that were not set by SoC properties)
             */
            SHR_IF_ERR_EXIT(bcm_dnx_port_resource_default_get(unit, logical_port, 0, port_resource_ptr));

            /** override phy lane config bits with values from SoC property */
            SHR_IF_ERR_EXIT(dnx_port_resource_lane_config_soc_properties_get(unit, logical_port, port_resource_ptr));

            num_port_resource++;
        }
    }

    SHR_IF_ERR_EXIT(bcm_dnx_port_resource_multi_set(unit, num_port_resource, port_resource));

    for (port_index = 0; port_index < num_port_resource; port_index++)
    {
        bcm_port_resource_t *port_resource_ptr = &port_resource[port_index];
        logical_port = used_logical_port[port_index];

        /** set precoder according to SoC property */
        tx_pam4_precoder = dnx_data_port.static_add.tx_pam4_precoder_get(unit, logical_port)->val;

        if (port_resource_ptr->link_training == 0)
        {
            SHR_IF_ERR_EXIT(bcm_dnx_port_phy_control_set
                            (unit, logical_port, BCM_PORT_PHY_CONTROL_TX_PAM4_PRECODER_ENABLE, tx_pam4_precoder));
        }
        else if ((port_resource_ptr->link_training == 1) && (tx_pam4_precoder == 1))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "port %d: if link training is enabled, enabling tx_pam4_precoder is not allowed.",
                         logical_port);
        }

        /** set lp precoder according to SoC property */
        lp_tx_precoder = dnx_data_port.static_add.lp_tx_precoder_get(unit, logical_port)->val;

        if (port_resource_ptr->link_training == 0)
        {
            SHR_IF_ERR_EXIT(bcm_dnx_port_phy_control_set
                            (unit, logical_port, BCM_PORT_PHY_CONTROL_LP_TX_PRECODER_ENABLE, lp_tx_precoder));
        }
        else if ((port_resource_ptr->link_training == 1) && (lp_tx_precoder == 1))
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG,
                         "port %d: if link training is enabled, enabling lp_tx_precoder is not allowed.", logical_port);
        }

        /*
         * if link training is disabled - set TX FIR params according to SoC property value
         */
        if (port_resource_ptr->link_training == 0)
        {
            SHR_IF_ERR_EXIT(dnx_port_init_serdes_tx_taps_config(unit, logical_port));
        }
    }

    PBMP_ITER(okay_pbmp, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_if_port_enable_set_phase1(unit, logical_port, port_enable[logical_port]));
    }

    sal_usleep(1000);

    PBMP_ITER(okay_pbmp, logical_port)
    {
        SHR_IF_ERR_EXIT(dnx_fabric_if_port_enable_set_phase2(unit, logical_port, port_enable[logical_port]));
    }

    /*
     * Configure ALDWP
     */
    SHR_IF_ERR_EXIT(dnx_fabric_aldwp_config(unit));

exit:
    SHR_FREE(port_resource);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set fabric blocks to power down
 */
static shr_error_e
dnx_fabric_power_down_init(
    int unit)
{
    dnx_fabric_connect_mode_e fabric_connect_mode;
    bcm_pbmp_t supported_links;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_clock_power_down))
    {

        supported_links = dnx_data_fabric.links.general_get(unit)->supported_links;
        fabric_connect_mode = dnx_data_fabric.general.connect_mode_get(unit);

        if (fabric_connect_mode == DNX_FABRIC_CONNECT_MODE_SINGLE_FAP || _SHR_PBMP_IS_NULL(supported_links))
        {
            /** Power down fabric blocks */
            SHR_IF_ERR_EXIT(dnx_fabric_blocks_power_down_set(unit, TRUE));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_init(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * init sw state db
     */
    SHR_IF_ERR_EXIT(dnx_fabric_sw_state_db_init(unit));

    /*
     * Init fabric features
     */
    SHR_IF_ERR_EXIT(dnx_fabric_features_init(unit));

    /*
     * Init fabric ports
     */
    SHR_IF_ERR_EXIT(dnx_fabric_ports_init(unit));

    /*
     * Set fabric blocks to power down
     */
    SHR_IF_ERR_EXIT(dnx_fabric_power_down_init(unit));
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Deinitialize sw_state database.
 */
static shr_error_e
dnx_fabric_sw_state_db_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * sw state module deinit is done automatically at device deinit
     */

    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Deinit sw state db
     */
    SHR_IF_ERR_CONT(dnx_fabric_sw_state_db_deinit(unit));

    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * Verify Input parameters of bcm_dnx_fabric_module_control_get().
 * Refer to bcm_dnx_fabric_module_control_get() for additional details
 */
static int
dnx_fabric_module_control_set_verify(
    int unit,
    uint32 flags,
    bcm_module_t modid,
    bcm_fabric_module_control_t control,
    int value)
{

    SHR_FUNC_INIT_VARS(unit);

    /** High level verify */
    SHR_RANGE_VERIFY(modid, 0, dnx_data_device.general.nof_faps_get(unit) - 1, _SHR_E_PARAM, "modid out of bound.\n");

    /** Verify Fabric is supported on this device */
    if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Fabric Module Controls are not supported on this device");
    }

    /** Verify Fabric Status Message modes configuration is supported on this device */
    if ((control == bcmFabricModuleControlFSMSeqNumberEnable)
        && !dnx_data_sch.general.feature_get(unit, dnx_data_sch_general_fsm_modes_config_supported))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Fabric Status Message modes configuration not supported on this device");
    }

    /** Value will be verified per control */
exit:
    SHR_FUNC_EXIT;

}
/**
 * \brief -
 * General function for simple configuration per module ID.
 *
 * Use cases:
 * ** Use case: "FSM Mode"
 * Configure FSM mode.
 * There are two mechanisms which handle FSM:
 * - Delayed FSM - wait before sending a new FSM.
 * - Sequential FSM - add sequence number to FSM control cell.
 * Default is sequential FSM (recommended)
 * Delayed FSM should be set when the remote device is legacy generation (done per destination FAP-ID).
 * Parameters:
 * - flags - not used, set to 0
 * - modid - represent FAP-ID
 * - control - set to bcmFabricModuleControlFSMSeqNumberEnable
 * - value - 0 - Delayed FSM
 *           1 - Sequential FSM
 *
 *  ** Use case: ""
 *  TBD
 *
 * \param [in] unit -  Unit-ID
 * \param [in] flags - description per use case
 * \param [in] modid - module ID (FAP-ID)
 * \param [in] control - action to preform (see use cases for the supported controls)
 * \param [in] value - description per use case
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_fabric_module_control_set(
    int unit,
    uint32 flags,
    bcm_module_t modid,
    bcm_fabric_module_control_t control,
    int value)
{
    dnx_iqs_fsm_mode_e fsm_mode;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);
    /** High level verify */
    SHR_INVOKE_VERIFY_DNX(dnx_fabric_module_control_set_verify(unit, flags, modid, control, value));
    /*
     * Use case per control
     */
    switch (control)
    {
            /*
             * Use case: "FSM Mode"
             */
        case bcmFabricModuleControlFSMSeqNumberEnable:
        {
            fsm_mode = (value ? dnx_iqs_fsm_mode_sequential : dnx_iqs_fsm_mode_delayed);
            SHR_IF_ERR_EXIT(dnx_iqs_fsm_mode_set(unit, modid, fsm_mode));
            break;
        }

        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Control not supported: %d", control);
            break;
        }
    }
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 * Verify Input parameters of bcm_dnx_fabric_module_control_get().
 * Refer to bcm_dnx_fabric_module_control_get() for additional details
 */
int
dnx_fabric_module_control_get_verify(
    int unit,
    uint32 flags,
    bcm_module_t modid,
    bcm_fabric_module_control_t control,
    int *value)
{

    SHR_FUNC_INIT_VARS(unit);

    SHR_RANGE_VERIFY(modid, 0, dnx_data_device.general.nof_faps_get(unit) - 1, _SHR_E_PARAM, "modid out of bound.\n");

    SHR_NULL_CHECK(value, _SHR_E_PARAM, "value");

    if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Fabric Module Controls are not supported on this device");
    }

    /** Verify Fabric Status Message modes configuration is supported on this device */
    if ((control == bcmFabricModuleControlFSMSeqNumberEnable)
        && !dnx_data_sch.general.feature_get(unit, dnx_data_sch_general_fsm_modes_config_supported))
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Fabric Status Message modes configuration not supported on this device");
    }

exit:
    SHR_FUNC_EXIT;

}
/**
 * \brief -
 * General function for simple configuration per module ID.
 * See description in bcm_dnx_fabric_module_control_set()
 *
 * \param [in] unit -  Unit-ID
 * \param [in] flags - description per use case
 * \param [in] modid - module ID (FAP-ID)
 * \param [in] control - action to preform (see use cases for the supported controls)
 * \param [out] value - description per use case
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_fabric_module_control_get(
    int unit,
    uint32 flags,
    bcm_module_t modid,
    bcm_fabric_module_control_t control,
    int *value)
{
    dnx_iqs_fsm_mode_e fsm_mode;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /** High level verify */
    SHR_INVOKE_VERIFY_DNX(dnx_fabric_module_control_get_verify(unit, flags, modid, control, value));
    /*
     * Use case per control
     */
    switch (control)
    {
            /*
             * Use case: "FSM Mode"
             */
        case bcmFabricModuleControlFSMSeqNumberEnable:
        {
            SHR_IF_ERR_EXIT(dnx_iqs_fsm_mode_get(unit, modid, &fsm_mode));
            *value = (fsm_mode == dnx_iqs_fsm_mode_sequential);
            break;
        }

        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Control not supported: %d", control);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * See fabric.h
 */
shr_error_e
dnx_fabric_link_isolation_set(
    int unit,
    int link,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_LINKS_ALLOWED, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ALLOWED_REACHABILITY, INST_SINGLE,
                                 enable ? FALSE : TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fabric_link_tx_traffic_disable_set(
    int unit,
    int link,
    int disable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Disable/Enable cells
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_LINKS_ALLOWED, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_LINK, link);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_ALLOWED, INST_SINGLE, disable ? FALSE : TRUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * General function for simple fabric configuration.
 *
 * Use cases:
 *
 * ** Use case: "watchdog queue range"
 * Range of queues supporting credit watchdog mechanism.
 * Parameters:
 * - type - bcmFabricWatchdogQueueMin or bcmFabricWatchdogQueueMax
 * - arg - Minimum queue / maximum queue in queue range
 *
 * ** Use case: "Enable watchdog"
 * Enable / Disable credit watchdog mechanism
 * Parameters:
 * - type - bcmFabricWatchdogQueueEnable
 * - arg - Enable / Disable
 *
 * ** use case: "multicast scheduling mode"
 * Set multicast scheduling mode.
 * Enhance scheduling of fabric multicast queues allows to create enhance scheduling scheme similarly to egress credit scheduling schemes.
 * In this mode, the local credit generate credits to be distributed be the local scheduler.
 * Notes:
 * - Must be called before creating egress scheduling scheme
 * - Allocating HRs 0-3 (HRs 0-3 could not be used for egress scheduling scheme)
 * Parameters:
 * - type - bcmFabricMulticastSchedulerMode
 * - arg - 1:enable enhance mode, 0:disable enhance mode (0 is the default).
 *
 * ** use case: "isolate"
 * isolate / unisolate device from the system.
 * Sequence which safely removes the device from the system (by stopping sending control cells)
 * Withoout any traffic loss.
 * Parameters:
 * - type - bcmFabricIsolate or bcmFabricControlCellsEnable (does the same operation)
 * - arg -  For bcmFabricIsolate: 0 - unisolate, 1 - isolate.
 *          For bcmFabricControlCellsEnable: 1 - unisolate, 0 - isolate.
 * ** use case: "FMQs range"
 * Set range for multicast queues (up to max_fmq_id)
 * By default, FMQ range is 0-3.
 * FMQ range can be expanded using this API. The remaining queues can be used for non-FMQ traffic.
 * Parameters:
 * - type - bcmFabricMulticastQueueMax
 * - arg - Maximal FMQ id. Must be in 2K resolution
 *
 * ** use case: "minimum links to destination"
 * Set minimum links to destination FAP.
 * Will set the min links to ALL the FAPs.
 * Notes:
 * - Setting min links through this function is deprecated, and is maintained
 *   here only for backward compatability reasons.
 * - User is advised to use bcm_fabric_destination_link_min_set instead.
 * Parameters:
 * - type - bcmFabricMinimalLinksToDestination
 * - arg - minimum links, must be between 0-nof_fabric_links.
 *
 * ** use case: "single cell delay"
 * Enable/Disable delay of single cell in Fabric RX.
 * Parameters:
 * - type - bcmFabricDelaySingleCellInFabricRx
 * - arg - 1: enable delay. 0: disable delay.
 *
 * ** use case: "GCI Leaky Bucket Enable"
 * Enable/Disable GCI Leaky Bucket mechanism.
 * LEGACY - this use case is deprecated and used only for backward compatibility.
 * Calls the new implementation.
 * - type - bcmFabricCgmGciLeakyBucketEn
 * - arg - 1: enable leaky bucket. 0: disable leaky bucket.
 *
 * ** use case: "GCI Random-Backoff Enable"
 * Enable/Disable GCI Random-Backoff mechanism.
 * LEGACY - this use case is deprecated and used only for backward compatibility.
 * Calls the new implementation.
 * - type - bcmFabricCgmGciBackoffEn
 * - arg - 1: enable random-backoff. 0: disable random-backoff.
 *
 * ** Use case: "RCI throttling mode"
 * Set rci throttling mode
 * Parameters:
 * - type - bcmFabricControlRciThrottlingMode
 * - arg - bcmFabricControlRciThrottlingModeGlobal -- Basic mode (Default mode).
 * -     - bcmFabricControlRciThrottlingModeDeviceBiasing -- Device biasing
 * -     - bcmFabricControlRciThrottlingModeFlowBiasing -- Flow biasing
 *
 * \param [in] unit -  Unit-ID
 * \param [in] type - action to preform (see use cases for the supported controls)
 * \param [in] arg - description per use case
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_fabric_control_set(
    int unit,
    bcm_fabric_control_t type,
    int arg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /*
     * Use case per type
     */
    switch (type)
    {
            /*
             * Use case: "watchdog queue range"
             */
        case bcmFabricWatchdogQueueMin:
        {
            /** Verify input paramter */
            SHR_RANGE_VERIFY(arg, 0, dnx_data_ipq.queues.nof_queues_get(unit) - 1, _SHR_E_PARAM,
                             "queue_min out of bound");
            /** update data base */
            SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_queue_min_sw_set(unit, /* queue-id */ arg));
            /** verify all watchdog parameters*/
            SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_verify(unit));
            /** set hw accordingly */
            SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_hw_set(unit));
            break;
        }
            /*
             * Use case: "watchdog queue range"
             */
        case bcmFabricWatchdogQueueMax:
        {
            /** verify input paramter */
            SHR_RANGE_VERIFY(arg, 0, dnx_data_ipq.queues.nof_queues_get(unit) - 1, _SHR_E_PARAM,
                             "queue_max out of bound");
            /** update data base */
            SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_queue_max_sw_set(unit, /* queue-id */ arg));
            /** verify all watchdog parameters */
            SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_verify(unit));
            /** set hw accordingly */
            SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_hw_set(unit));
            break;
        }
            /*
             * Use case: "Enable watchdog"
             */
        case bcmFabricWatchdogQueueEnable:
        {
            /** verify input parameter */
            if (arg != BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE
                && arg != BCM_FABRIC_WATCHDOG_QUEUE_DISABLE)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "unexpected arg: expecting BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE or BCM_FABRIC_WATCHDOG_QUEUE_DISABLE.\n");
            }
            /** update data base */
            SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_enable_sw_set(unit, /* enable/disable */ arg));
            /** verify */
            SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_verify(unit));
            /** set hw accordingly */
            SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_hw_set(unit));
            break;
        }
            /*
             * use case: "multicast scheduling mode"
             */
        case bcmFabricMulticastSchedulerMode:
        {
            /** update data base */
            SHR_IF_ERR_EXIT(dnx_cosq_fmq_scheduler_mode_set(unit, /* scheduler mode - is enahanced */ arg));
            break;
        }
            /*
             * use case: "isolate"
             */
        case bcmFabricIsolate:
        {
            if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
            {
                SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_enable_set(unit, arg ? FALSE : TRUE, 0));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcmFabricIsolate is not supported on this device");
            }
            break;
        }
            /*
             * use case: "isolate"
             */
        case bcmFabricControlCellsEnable:
        {
            if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
            {
                SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_enable_set(unit, arg ? TRUE : FALSE, 0));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcmFabricControlCellsEnable is not supported on this device");
            }
            break;
        }
            /*
             * use case: "FMQs range"
             */
        case bcmFabricMulticastQueueMax:
        {
            SHR_IF_ERR_EXIT(dnx_cosq_ipq_fabric_multicast_queue_range_set(unit, arg));
            break;
        }

            /*
             * Throw error for controls not that are not supported in DNX
             */
        case bcmFabricShaperQueueMin:
        case bcmFabricShaperQueueMax:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "ISQs are not supported.");
            break;
        }
        case bcmFabricQueueMin:
        case bcmFabricQueueMax:
        case bcmFabricMulticastQueueMin:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Use bcmFabricMulticastQueueMax to get max FMQ range (min FMQ range is 0).");
            break;
        }
            /*
             * use case: "minimum links to destination"
             */
        case bcmFabricMinimalLinksToDestination:
        {
            SHR_IF_ERR_EXIT(bcm_dnx_fabric_destination_link_min_set(unit, 0, BCM_MODID_ALL, arg));
            break;
        }
            /*
             * use case: "single cell delay"
             */
        case bcmFabricDelaySingleCellInFabricRx:
        {
            if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_is_jr1_in_system_supported))
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcmFabricDelaySingleCellInFabricRx is not supported for this device.");
            }

            /** Verify input paramter */
            SHR_RANGE_VERIFY(arg, 0, 1, _SHR_E_PARAM, "arg must be either 0 or 1.");

            SHR_IF_ERR_EXIT(dnx_fabric_delay_single_cell_in_fabric_rx_enable_set(unit, arg));
            break;
        }
            /*
             * use case: "GCI Leaky Bucket Enable"
             */
        case bcmFabricGciLeakyBucketEnable:
        {
            SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set(unit, 0, bcmFabricCgmGciLeakyBucketEn, -1, arg));
            break;
        }
            /*
             * use case: "GCI Random-Backoff Enable"
             */
        case bcmFabricGciBackoffEnable:
        {
            SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set(unit, 0, bcmFabricCgmGciBackoffEn, -1, arg));
            break;
        }
            /*
             * use case: "RCI throttling mode"
             */
        case bcmFabricControlRciThrottlingMode:
        {
            SHR_IF_ERR_EXIT(dnx_fabric_rci_throttling_mode_set(unit, (bcm_fabric_control_rci_throttling_mode_t) arg));
            break;
        }
            /*
             * Throw error for controls not that are not supported in DNX
             */
        case bcmFabricRCIControlSource:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "Type %d is not supported. Please use bcm_fabric_control_set(bcmFabricControlRciThrottlingMode).\n",
                         type);
            break;
        }
            /*
             * Throw error for controls not that are not supported on this DNX API
             */
        case bcmFabricCellSizeMax:
        case bcmFabricCellSizeMin:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "\r\n"
                         "These TDM controls (bcmFabricCellSizeMax/bcmFabricCellSizeMin) are supported on another API: bcm_tdm_control_set()\n");
            break;
        }
            /*
             * Throw error for controls not that are not supported for TDM on this device
             */
        case bcmFabricCellSizeFixed:
        case bcmFabricForceTdmBypassTrafficToFabric:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "\r\n"
                         "These TDM controls (bcmFabricCellSizeFixed/bcmFabricForceTdmBypassTrafficToFabric) are not supported for TDM on this device.\n");
            break;
        }
        case bcmFabricRCIIncrementValue:
        case bcmFabricVsqCategory:
        case bcmFabricRecycleQueueMin:
        case bcmFabricRecycleQueueMax:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "type %d is not supported on this device.\n", type);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Type not supported: %d", type);
            break;
        }
    }
exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * General function for simple fabric configuration.
 * See bcm_dnx_fabric_control_set() for additional details.
 *
 * \param [in] unit -  Unit-ID
 * \param [in] type - action to preform (see use cases for the supported controls)
 * \param [in] arg - description per use case
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_fabric_control_get(
    int unit,
    bcm_fabric_control_t type,
    int *arg)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);
    /*
     * Global Verification
     */
    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");

    /*
     * Use case per type
     */
    switch (type)
    {
            /*
             * Use case: "watchdog queue range"
             */
        case bcmFabricWatchdogQueueMin:
        {
            /** update data base */
            SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_queue_min_sw_get(unit, /* queue-id */ arg));
            break;
        }
            /*
             * Use case: "watchdog queue range"
             */
        case bcmFabricWatchdogQueueMax:
        {
            /** get from sw data base */
            SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_queue_max_sw_get(unit, /* queue-id */ arg));
            break;
        }
            /*
             * Use case: "Enable watchdog"
             */
        case bcmFabricWatchdogQueueEnable:
        {
            /** get from sw data base */
            SHR_IF_ERR_EXIT(dnx_iqs_credit_watchdog_enable_sw_get(unit, /* enable/disable */ arg));
            *arg = *arg ? BCM_FABRIC_WATCHDOG_QUEUE_ENABLE_COMMON_STATUS_MESSAGE : BCM_FABRIC_WATCHDOG_QUEUE_DISABLE;
            break;
        }
            /*
             * use case: "multicast scheduling mode"
             */
        case bcmFabricMulticastSchedulerMode:
        {
            /** get from sw data base */
            SHR_IF_ERR_EXIT(dnx_cosq_fmq_scheduler_mode_get(unit, /* scheduler mode - is enahanced */ arg));
            break;
        }
            /*
             * use case: "isolate"
             */
        case bcmFabricIsolate:
        {
            if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
            {
                int is_control_cells_enabled = 0;

                SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_enable_get(unit, &is_control_cells_enabled));
                *arg = (is_control_cells_enabled == FALSE) ? 1 : 0;
            }
            else
            {
                *arg = 1;
            }
            break;
        }
            /*
             * use case: "isolate"
             */
        case bcmFabricControlCellsEnable:
        {
            if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
            {
                SHR_IF_ERR_EXIT(dnx_fabric_ctrl_cells_enable_get(unit, arg));
            }
            else
            {
                *arg = 0;
            }
            break;
        }
            /*
             * use case: "FMQs range"
             */
        case bcmFabricMulticastQueueMax:
        {
            SHR_IF_ERR_EXIT(dnx_cosq_ipq_fabric_multicast_queue_range_get(unit, arg));
            break;
        }

            /*
             * Throw error for controls not that are not supported in DNX
             */
        case bcmFabricShaperQueueMin:
        case bcmFabricShaperQueueMax:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "ISQs are not supported.");
            break;
        }
        case bcmFabricQueueMin:
        case bcmFabricQueueMax:
        case bcmFabricMulticastQueueMin:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Use bcmFabricMulticastQueueMax to get max FMQ range (min FMQ range is 0).");
            break;
        }
            /*
             * use case: "minimum links to destination"
             */
        case bcmFabricMinimalLinksToDestination:
        {
            SHR_IF_ERR_EXIT(bcm_dnx_fabric_destination_link_min_get(unit, 0, BCM_MODID_ALL, arg));
            break;
        }
            /*
             * use case: "single cell delay"
             */
        case bcmFabricDelaySingleCellInFabricRx:
        {
            if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_is_jr1_in_system_supported))
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "bcmFabricDelaySingleCellInFabricRx is not supported for this device.");
            }

            SHR_IF_ERR_EXIT(dnx_fabric_delay_single_cell_in_fabric_rx_enable_get(unit, arg));
            break;
        }
            /*
             * use case: "GCI Leaky Bucket Enable"
             */
        case bcmFabricGciLeakyBucketEnable:
        {
            SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get(unit, 0, bcmFabricCgmGciLeakyBucketEn, -1, arg));
            break;
        }
            /*
             * use case: "GCI Random-Backoff Enable"
             */
        case bcmFabricGciBackoffEnable:
        {
            SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get(unit, 0, bcmFabricCgmGciBackoffEn, -1, arg));
            break;
        }
            /*
             * use case: "RCI throttling mode"
             */
        case bcmFabricControlRciThrottlingMode:
        {
            SHR_IF_ERR_EXIT(dnx_fabric_rci_throttling_mode_get(unit, (bcm_fabric_control_rci_throttling_mode_t *) arg));
            break;
        }
            /*
             * Throw error for controls not that are not supported in DNX
             */
        case bcmFabricRCIControlSource:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "Type %d is not supported. Please use bcm_fabric_control_get(bcmFabricControlRciThrottlingMode).\n",
                         type);
            break;
        }
            /*
             * Throw error for controls not that are not supported on this DNX API
             */
        case bcmFabricCellSizeMax:
        case bcmFabricCellSizeMin:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "\r\n"
                         "These TDM controls (bcmFabricCellSizeMax/bcmFabricCellSizeMin) are supported on another API: bcm_tdm_control_set()\n");
            break;
        }
            /*
             * Throw error for controls not that are not supported for TDM on this device
             */
        case bcmFabricCellSizeFixed:
        case bcmFabricForceTdmBypassTrafficToFabric:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "\r\n"
                         "These TDM controls (bcmFabricCellSizeFixed/bcmFabricForceTdmBypassTrafficToFabric) are not supported for TDM on this device.\n");
            break;
        }
        case bcmFabricRCIIncrementValue:
        case bcmFabricVsqCategory:
        case bcmFabricRecycleQueueMin:
        case bcmFabricRecycleQueueMax:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "type %d is not supported on this device.\n", type);
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Type not supported: %d", type);
            break;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * General function for simple fabric link configuration.
 *
 * Use cases:
 *
 * ** Use case: "LLFC Enable"
 * Enable/Disable link's LLFC.
 * Parameters:
 * - type - bcmFabricLLFControlSource
 * - arg - 1 - enable LLFC. 0 - disable LLFC.
 *
 * ** Use case: "Link isolation"
 * Isolate/Unisolate link.
 * Parameters:
 * - type - bcmFabricLinkIsolate
 * - arg - 1 - isolate link. 0 - unisolate link.
 *
 * ** Use case: "Disable Tx Traffic"
 * Disable/Enable Tx traffic.
 * Parameters:
 * - type - bcmFabricLinkTxTrafficDisable
 * - arg - 1 - disable Tx traffic. 0 - enable Tx traffic.
 *
 * ** Use case: "Connect link to a repeater"
 * Connecting the device to a repeater is not supported, and this use case
 * is here only to print error for user.
 * Parameters:
 * - type - bcmFabricLinkRepeaterEnable
 *
 * \param [in] unit - Unit-ID
 * \param [in] link - port number
 * \param [in] type - action to perform (see use cases for the supported controls)
 * \param [in] arg - description per use case
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_fabric_link_control_set(
    int unit,
    bcm_port_t link,
    bcm_fabric_link_control_t type,
    int arg)
{
    bcm_port_t logical_port = 0;
    int link_i = 0;
    uint32 nof_links = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /*
     * General validation
     */
    /*
     * Get local port from port.
     * Note that param 'link' is actually refering to 'port'.
     */
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, link));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, link, &logical_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &link_i));
    nof_links = dnx_data_fabric.links.nof_links_get(unit);
    SHR_RANGE_VERIFY(link_i, 0, nof_links - 1, _SHR_E_PARAM, "Link %d is out-of-range", link_i);

    SHR_RANGE_VERIFY(arg, 0, 1, _SHR_E_PARAM, "Unsupported arg %d", arg);

    switch (type)
    {
            /*
             * use case: "LLFC Enable"
             */
        case bcmFabricLLFControlSource:
        {
            SHR_IF_ERR_EXIT(dnx_fabric_cgm_llfc_enable_hw_set(unit, link_i, arg));
            break;
        }
            /*
             * use case: "Link isolation"
             */
        case bcmFabricLinkIsolate:
        {
            int is_port_enabled = 0;

            /** Need to stop traffic before isolation */
            if (arg == 1)
            {
                /** Stop tx traffic on the link */
                SHR_IF_ERR_EXIT(dnx_fabric_link_tx_traffic_disable_set(unit, link_i, arg));
            }

            /*
             * Update HW only if port is enabled. If port is currently
             * disabled, HW will be updated according to sw_state when
             * port will be enabled again.
             */
            DNX_ERR_RECOVERY_SUPPRESS(unit);
            SHR_IF_ERR_EXIT(bcm_dnx_port_enable_get(unit, link, &is_port_enabled));
            DNX_ERR_RECOVERY_UNSUPPRESS(unit);

            if (is_port_enabled)
            {
                /** Stop transmitting reachability cells on the link */
                SHR_IF_ERR_EXIT(dnx_fabric_link_isolation_set(unit, link_i, arg));
            }

            /** Update sw_state */
            SHR_IF_ERR_EXIT(dnx_fabric_db.links.is_link_isolated.set(unit, link_i, arg ? 1 : 0));

            /** Need to enable traffic after un-isolation */
            if (arg == 0)
            {
                int is_link_allowed = 0;

                /*
                 * Enable link in RTP according to sw_state
                 */
                SHR_IF_ERR_EXIT(dnx_fabric_db.links.is_link_allowed.get(unit, link_i, &is_link_allowed));
                if (is_link_allowed)
                {
                    /** Enable tx traffic on the link */
                    SHR_IF_ERR_EXIT(dnx_fabric_link_tx_traffic_disable_set(unit, link_i, arg));
                }
            }
            break;
        }
            /*
             * use case: "Disable Tx Traffic"
             */
        case bcmFabricLinkTxTrafficDisable:
        {
            int link_traffic_disable = arg ? 1 : 0;
            int is_port_enabled = 0;

            /*
             * Update HW only if port is enabled. If port is currently
             * disabled, HW will be updated according to sw_state when
             * port will be enabled again.
             */
            DNX_ERR_RECOVERY_SUPPRESS(unit);
            SHR_IF_ERR_EXIT(bcm_dnx_port_enable_get(unit, link, &is_port_enabled));
            DNX_ERR_RECOVERY_UNSUPPRESS(unit);

            if (is_port_enabled)
            {
                SHR_IF_ERR_EXIT(dnx_fabric_link_tx_traffic_disable_set(unit, link_i, link_traffic_disable));
            }

            /*
             * Update sw_state
             */
            SHR_IF_ERR_EXIT(dnx_fabric_db.links.is_link_allowed.set(unit, link_i, link_traffic_disable ? 0 : 1));

            break;
        }
            /*
             * use case: "Connect link to a repeater"
             */
        case bcmFabricLinkRepeaterEnable:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Connecting this device to a repeater is not supported");
            break;
        }

        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unsupported Type %d", type);
        }
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * General function for simple fabric link configuration.
 * See bcm_dnx_fabric_link_control_set() for additional details.
 *
 * \param [in] unit - Unit-ID
 * \param [in] link - link number
 * \param [in] type - action to preform (see use cases for the supported controls)
 * \param [in] arg - description per use case
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_fabric_link_control_get(
    int unit,
    bcm_port_t link,
    bcm_fabric_link_control_t type,
    int *arg)
{
    bcm_port_t logical_port = 0;
    bcm_port_t link_i;
    uint32 nof_links = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * General validation
     */
    SHR_NULL_CHECK(arg, _SHR_E_PARAM, "arg");

    /*
     * Get local port from port.
     * Note that param 'link' is actually refering to 'port'.
     */
    SHR_IF_ERR_EXIT(dnx_port_logical_verify(unit, link));
    SHR_IF_ERR_EXIT(dnx_port_logical_get(unit, link, &logical_port));

    SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &link_i));
    nof_links = dnx_data_fabric.links.nof_links_get(unit);
    SHR_RANGE_VERIFY(link_i, 0, nof_links - 1, _SHR_E_PARAM, "Link %d is out-of-range", link_i);

    switch (type)
    {
            /*
             * use case: "LLFC Enable"
             */
        case bcmFabricLLFControlSource:
        {
            SHR_IF_ERR_EXIT(dnx_fabric_cgm_llfc_enable_hw_get(unit, link_i, arg));
            break;
        }
            /*
             * use case: "Link isolation"
             */
        case bcmFabricLinkIsolate:
        {
            /*
             * The status is taken from sw_state and not from HW since
             * the fabric link can be conceptualy not isolated even
             * though the link is currently disabled.
             */
            SHR_IF_ERR_EXIT(dnx_fabric_db.links.is_link_isolated.get(unit, link_i, arg));
            break;
        }
            /*
             * use case: "Disable Tx Traffic"
             */
        case bcmFabricLinkTxTrafficDisable:
        {
            int is_fabric_link_allowed = 0;

            /*
             * The status is taken from sw_state and not from HW since
             * traffic can be conceptualy enabled even though the port
             * is currently disabled.
             */
            SHR_IF_ERR_EXIT(dnx_fabric_db.links.is_link_allowed.get(unit, link_i, &is_fabric_link_allowed));
            *arg = !is_fabric_link_allowed;
            break;
        }
            /*
             * use case: "Connect link to a repeater"
             */
        case bcmFabricLinkRepeaterEnable:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Connecting this device to a repeater is not supported");
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Unsupported Type %d", type);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/** See .h file */
shr_error_e
dnx_fabric_control_cells_disable(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Disable fabric control cells */
    SHR_IF_ERR_EXIT(bcm_dnx_fabric_control_set(unit, bcmFabricControlCellsEnable, FALSE));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set Fabric CGM GCI thresholds.
 *   This function is deprecated and used only for backward compatibility.
 *   Use bcm_fabric_cgm_control_set instead.
 *
 * \param [in] unit -
 *   Unit-ID
 * \param [in] fifo_type -
 *   should be set to -1.
 * \param [in] count -
 *   number of threshold types in 'type' array.
 * \param [in] type -
 *   array of threshold types.
 * \param [in] value -
 *   array of threshold values that match the threshold types array.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_fabric_link_thresholds_set(
    int unit,
    int fifo_type,
    uint32 count,
    bcm_fabric_link_threshold_type_t * type,
    int *value)
{
    int index = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    if (fifo_type == -1)
    {
        /*
         * special functionality - no fifo_type required
         */
        for (index = 0; index < count; index++)
        {
            switch (type[index])
            {
                    /*
                     * GCI Backoff related thresholds configuration
                     */
                case bcmFabricLinkTxGciLvl1FC:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set
                                    (unit, 0, bcmFabricCgmGciBackoffLevelTh, bcmFabricLevel0, value[index]));
                    break;
                }
                case bcmFabricLinkTxGciLvl2FC:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set
                                    (unit, 0, bcmFabricCgmGciBackoffLevelTh, bcmFabricLevel1, value[index]));
                    break;
                }
                case bcmFabricLinkTxGciLvl3FC:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set
                                    (unit, 0, bcmFabricCgmGciBackoffLevelTh, bcmFabricLevel2, value[index]));
                    break;
                }

                    /*
                     * GCI Leaky bucket flow control thresholds
                     */
                case bcmFabricLinkGciLeakyBucket1Congestion:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set
                                    (unit, 0, bcmFabricCgmGciLeakyBucketFCLeakyBucketTh, bcmFabricLeakyBucket0,
                                     value[index]));
                    break;
                }
                case bcmFabricLinkGciLeakyBucket2Congestion:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set
                                    (unit, 0, bcmFabricCgmGciLeakyBucketFCLeakyBucketTh, bcmFabricLeakyBucket1,
                                     value[index]));
                    break;
                }
                case bcmFabricLinkGciLeakyBucket3Congestion:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set
                                    (unit, 0, bcmFabricCgmGciLeakyBucketFCLeakyBucketTh, bcmFabricLeakyBucket2,
                                     value[index]));
                    break;
                }
                case bcmFabricLinkGciLeakyBucket4Congestion:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set
                                    (unit, 0, bcmFabricCgmGciLeakyBucketFCLeakyBucketTh, bcmFabricLeakyBucket3,
                                     value[index]));
                    break;
                }

                    /*
                     * GCI Leaky bucket full thresholds
                     */
                case bcmFabricLinkGciLeakyBucket1Full:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set
                                    (unit, 0, bcmFabricCgmGciLeakyBucketSizeLeakyBucketTh, bcmFabricLeakyBucket0,
                                     value[index]));
                    break;
                }
                case bcmFabricLinkGciLeakyBucket2Full:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set
                                    (unit, 0, bcmFabricCgmGciLeakyBucketSizeLeakyBucketTh, bcmFabricLeakyBucket1,
                                     value[index]));
                    break;
                }
                case bcmFabricLinkGciLeakyBucket3Full:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set
                                    (unit, 0, bcmFabricCgmGciLeakyBucketSizeLeakyBucketTh, bcmFabricLeakyBucket2,
                                     value[index]));
                    break;
                }
                case bcmFabricLinkGciLeakyBucket4Full:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_set
                                    (unit, 0, bcmFabricCgmGciLeakyBucketSizeLeakyBucketTh, bcmFabricLeakyBucket3,
                                     value[index]));
                    break;
                }

                default:
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown type %d", type[index]);
                    break;
                }
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Fifo type not supported: %d", fifo_type);
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get Fabric CGM GCI thresholds.
 *   This function is deprecated and used only for backward compatibility.
 *   Use bcm_fabric_cgm_control_get instead.
 *
 * \param [in] unit -
 *   Unit-ID
 * \param [in] fifo_type -
 *   should be set to -1.
 * \param [in] count -
 *   number of threshold types in 'type' array.
 * \param [in] type -
 *   array of threshold types.
 * \param [out] value -
 *   array of threshold values that match the threshold types array.
 *
 * \return
 *   See shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_fabric_link_thresholds_get(
    int unit,
    int fifo_type,
    uint32 count,
    bcm_fabric_link_threshold_type_t * type,
    int *value)
{
    int index = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    if (fifo_type == -1)
    {
        /*
         * special functionality - no fifo_type required
         */
        for (index = 0; index < count; index++)
        {
            switch (type[index])
            {
                    /*
                     * GCI Backoff related thresholds configuraion
                     */
                case bcmFabricLinkTxGciLvl1FC:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get
                                    (unit, 0, bcmFabricCgmGciBackoffLevelTh, bcmFabricLevel0, &(value[index])));
                    break;
                }
                case bcmFabricLinkTxGciLvl2FC:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get
                                    (unit, 0, bcmFabricCgmGciBackoffLevelTh, bcmFabricLevel1, &(value[index])));
                    break;
                }
                case bcmFabricLinkTxGciLvl3FC:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get
                                    (unit, 0, bcmFabricCgmGciBackoffLevelTh, bcmFabricLevel2, &(value[index])));
                    break;
                }

                    /*
                     * GCI Leaky bucket flow control thresholds
                     */
                case bcmFabricLinkGciLeakyBucket1Congestion:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get
                                    (unit, 0, bcmFabricCgmGciLeakyBucketFCLeakyBucketTh, bcmFabricLeakyBucket0,
                                     &(value[index])));
                    break;
                }
                case bcmFabricLinkGciLeakyBucket2Congestion:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get
                                    (unit, 0, bcmFabricCgmGciLeakyBucketFCLeakyBucketTh, bcmFabricLeakyBucket1,
                                     &(value[index])));
                    break;
                }
                case bcmFabricLinkGciLeakyBucket3Congestion:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get
                                    (unit, 0, bcmFabricCgmGciLeakyBucketFCLeakyBucketTh, bcmFabricLeakyBucket2,
                                     &(value[index])));
                    break;
                }
                case bcmFabricLinkGciLeakyBucket4Congestion:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get
                                    (unit, 0, bcmFabricCgmGciLeakyBucketFCLeakyBucketTh, bcmFabricLeakyBucket3,
                                     &(value[index])));
                    break;
                }

                    /*
                     * GCI Leaky bucket full thresholds
                     */
                case bcmFabricLinkGciLeakyBucket1Full:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get
                                    (unit, 0, bcmFabricCgmGciLeakyBucketSizeLeakyBucketTh, bcmFabricLeakyBucket0,
                                     &(value[index])));
                    break;
                }
                case bcmFabricLinkGciLeakyBucket2Full:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get
                                    (unit, 0, bcmFabricCgmGciLeakyBucketSizeLeakyBucketTh, bcmFabricLeakyBucket1,
                                     &(value[index])));
                    break;
                }
                case bcmFabricLinkGciLeakyBucket3Full:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get
                                    (unit, 0, bcmFabricCgmGciLeakyBucketSizeLeakyBucketTh, bcmFabricLeakyBucket2,
                                     &(value[index])));
                    break;
                }
                case bcmFabricLinkGciLeakyBucket4Full:
                {
                    SHR_IF_ERR_EXIT(bcm_dnx_fabric_cgm_control_get
                                    (unit, 0, bcmFabricCgmGciLeakyBucketSizeLeakyBucketTh, bcmFabricLeakyBucket3,
                                     &(value[index])));
                    break;
                }

                default:
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown type %d", type[index]);
                    break;
                }
            }
        }
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "Fifo type not supported: %d", fifo_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set fabric priority according to several parameters
 * \param [in] unit -
 *   Unit id.
 * \param [in] tc -
 *   Traffic Class.
 * \param [in] dp -
 *   Color.
 * \param [in] flags -
 *   Additional parametes. Can be one of the following:
 *   BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY
 *   BCM_FABRIC_PRIORITY_MULTICAST
 *   BCM_FABRIC_PRIORITY_OCB_ONLY_ONLY
 * \param [in] fabric_priority -
 *   The fabric priority to set for the above parameters.
 */
static shr_error_e
dnx_fabric_priority_set(
    int unit,
    uint32 tc,
    uint32 dp,
    uint32 flags,
    int fabric_priority)
{
    uint32 entry_handle_id;
    int is_hp = 0;
    int is_mc = 0;
    int is_ocb_only = 0;
    int is_tdm = 0;
    uint32 is_tdm_priority = (dnx_data_fabric.tdm.priority_get(unit) == fabric_priority);
    uint32 is_packet_tdm = (dnx_data_tdm.params.mode_get(unit) == TDM_MODE_PACKET);
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get whether the configured packet is TDM packet
     */
    is_tdm = (is_tdm_priority && is_packet_tdm) ? 1 : 0;

    /*
     * 'tc' and 'dp' keys are given as params.
     * Get additional keys from 'flags' param.
     */
    is_hp = (flags & BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY) ? 1 : 0;
    is_mc = (flags & BCM_FABRIC_PRIORITY_MULTICAST) ? 1 : 0;
    is_ocb_only = (flags & BCM_FABRIC_PRIORITY_OCB_ONLY_ONLY) ? 1 : 0;

    /*
     * Fill table with fabric priority in the index found
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CELL_PRIORITY, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_HP, is_hp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, is_mc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_OCB_ONLY, is_ocb_only);

    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_PRIORITY, INST_SINGLE, fabric_priority);
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, is_tdm);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get fabric priority according to several parameters
 * \param [in] unit -
 *   Unit id.
 * \param [in] tc -
 *   Traffic Class.
 * \param [in] dp -
 *   Color.
 * \param [in] flags -
 *   Additional parametes. Can be one of the following:
 *   BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY
 *   BCM_FABRIC_PRIORITY_MULTICAST
 *   BCM_FABRIC_PRIORITY_OCB_ONLY_ONLY
 * \param [out] fabric_priority -
 *   The fabric priority to set for the above parameters.
 */
static shr_error_e
dnx_fabric_priority_get(
    int unit,
    uint32 tc,
    uint32 dp,
    uint32 flags,
    int *fabric_priority)
{
    uint32 entry_handle_id;
    int is_hp = 0;
    int is_mc = 0;
    int is_ocb_only = 0;
    uint32 is_tdm = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * The keys 'tc' and 'dp' are given as params.
     * Get additional keys from 'flags' param.
     */
    is_hp = (flags & BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY) ? 1 : 0;
    is_mc = (flags & BCM_FABRIC_PRIORITY_MULTICAST) ? 1 : 0;
    is_ocb_only = (flags & BCM_FABRIC_PRIORITY_OCB_ONLY_ONLY) ? 1 : 0;

    /*
     * Retrieve table entry according to keys
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_CELL_PRIORITY, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_HP, is_hp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_MC, is_mc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DP, dp);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_TC, tc);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_OCB_ONLY, is_ocb_only);

    if (dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FABRIC_PRIORITY, INST_SINGLE,
                                   (uint32 *) fabric_priority);
    }
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_IS_TDM, INST_SINGLE, &is_tdm);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    if (!dnx_data_fabric.general.feature_get(unit, dnx_data_fabric_general_blocks_exist))
    {
        /*
         * In case there is no fabric, use 'fabric_priority' as TDM indication:
         * If packet TDM - fabric_priority will be TDM priority.
         * If not - fabric priority will be 0.
         */
        *fabric_priority = (is_tdm) ? dnx_data_fabric.tdm.priority_get(unit) : 0;
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * \brief
 *   Validate params of bcm_dnx_fabric_priority_set/get()
 */
static int
dnx_fabric_priority_verify(
    int unit,
    uint32 flags,
    bcm_cos_t ingress_pri,
    bcm_color_t color,
    int *fabric_priority)
{
    uint32 mask =
        BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY | BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY | BCM_FABRIC_PRIORITY_MULTICAST |
        BCM_FABRIC_PRIORITY_TDM | BCM_FABRIC_PRIORITY_OCB_MIX_ONLY | BCM_FABRIC_PRIORITY_OCB_ONLY_ONLY;
    SHR_FUNC_INIT_VARS(unit);

    SHR_RANGE_VERIFY(ingress_pri, 0, DNX_COSQ_NOF_TC - 1, _SHR_E_PARAM, "Invalid ingress_pri parameter %d",
                     ingress_pri);

    SHR_RANGE_VERIFY(color, 0, DNX_COSQ_NOF_DP - 1, _SHR_E_PARAM, "Invalid ingress_dp parameter %d", color);

    SHR_MASK_VERIFY(flags, mask, _SHR_E_PARAM, "Unrecognized flags");

    if (dnx_data_tdm.params.mode_get(unit) == TDM_MODE_NONE)
    {
        if (flags & BCM_FABRIC_PRIORITY_TDM)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "flag BCM_FABRIC_PRIORITY_TDM is not supported");
        }
    }

    if ((flags & BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY) && (flags & BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "flags includes both BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY and BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY");
    }

    if ((flags & BCM_FABRIC_PRIORITY_OCB_MIX_ONLY) && (flags & BCM_FABRIC_PRIORITY_OCB_ONLY_ONLY))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "flags includes both BCM_FABRIC_PRIORITY_OCB_MIX_ONLY and BCM_FABRIC_PRIORITY_OCB_ONLY_ONLY");
    }

    if (fabric_priority != NULL)
    {
        uint32 is_tdm_priority = (dnx_data_fabric.tdm.priority_get(unit) == *fabric_priority);
        uint32 is_packet_tdm = (dnx_data_tdm.params.mode_get(unit) == TDM_MODE_PACKET);

        /** nof_priorities is 1 based, whereas fabric_priority is 0 based */
        SHR_RANGE_VERIFY(*fabric_priority, 0, dnx_data_fabric.cell.nof_priorities_get(unit) - 1, _SHR_E_PARAM,
                         "fabric PKT priority is %d, but it must be between 0 and %d", *fabric_priority,
                         dnx_data_fabric.cell.nof_priorities_get(unit) - 1);

        if (is_tdm_priority && !is_packet_tdm)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "TDM priority can be set only on systems which support packet-TDM");
        }

        if (is_packet_tdm)
        {
            if (((flags & BCM_FABRIC_PRIORITY_OCB_ONLY_ONLY) && !is_tdm_priority) ||
                (is_tdm_priority && !(flags & BCM_FABRIC_PRIORITY_OCB_ONLY_ONLY)))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "In Packet-TDM mode, tdm priority should be set along with BCM_FABRIC_PRIORITY_OCB_ONLY_ONLY flag");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_priority_set(
    int unit,
    uint32 flags,
    bcm_cos_t ingress_pri,
    bcm_color_t color,
    int fabric_priority)
{
    int traffic_enabled = 0;
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    /*
     * Input validation
     */
    SHR_IF_ERR_EXIT(dnx_fabric_priority_verify(unit, flags, ingress_pri, color, &fabric_priority));

    /*
     * Validate traffic is disabled
     */
    SHR_IF_ERR_EXIT(dnx_stk_sys_traffic_enable_get(unit, &traffic_enabled));
    if (traffic_enabled)
    {
        SHR_ERR_EXIT(_SHR_E_FAIL, "Traffic must be disabled is order to run this API");
    }

    if ((flags & BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY) == 0 && (flags & BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY) == 0)
    {
        /*
         * set both hp and lp
         */
        uint32 flags_high, flags_low;

        flags_high = flags | BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY;
        SHR_IF_ERR_EXIT(dnx_fabric_priority_set(unit, ingress_pri, color, flags_high, fabric_priority));

        flags_low = flags | BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY;
        SHR_IF_ERR_EXIT(dnx_fabric_priority_set(unit, ingress_pri, color, flags_low, fabric_priority));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_fabric_priority_set(unit, ingress_pri, color, flags, fabric_priority));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_priority_get(
    int unit,
    uint32 flags,
    bcm_cos_t ingress_pri,
    bcm_color_t color,
    int *fabric_priority)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    /*
     * Input validation
     */
    SHR_NULL_CHECK(fabric_priority, _SHR_E_PARAM, "fabric_priority");
    SHR_IF_ERR_EXIT(dnx_fabric_priority_verify(unit, flags, ingress_pri, color, NULL));

    if ((flags & BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY) == 0 && (flags & BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY) == 0)
    {
        /*
         * get both hp and lp
         */
        uint32 flags_high, flags_low;
        int fabric_priority_low = 0, fabric_priority_high = 0;

        flags_high = flags | BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY;
        SHR_IF_ERR_EXIT(dnx_fabric_priority_get(unit, ingress_pri, color, flags_high, &fabric_priority_high));

        flags_low = flags | BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY;
        SHR_IF_ERR_EXIT(dnx_fabric_priority_get(unit, ingress_pri, color, flags_low, &fabric_priority_low));

        SHR_VAL_VERIFY(fabric_priority_low, fabric_priority_high, _SHR_E_FAIL,
                       "fabric priority is different for QUEUE_PRIORITY_LOW and QUEUE_PRIORITY_HIGH. use flags BCM_FABRIC_QUEUE_PRIORITY_HIGH_ONLY or BCM_FABRIC_QUEUE_PRIORITY_LOW_ONLY.");

        *fabric_priority = fabric_priority_high;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_fabric_priority_get(unit, ingress_pri, color, flags, fabric_priority));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set Fabric PCP (Packet Cell Packing) mode per destination device.
 *   There are 3 supported PCP modes:
 *     0 - No packing.
 *     1 - Simple packing.
 *     2 - Continuous packing.
 */
static shr_error_e
dnx_fabric_pcp_dest_mode_config_set(
    int unit,
    bcm_module_t modid,
    bcm_fabric_pcp_mode_t pcp_mode)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_fabric_pcp_mode_e fabric_pcp_mode;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Convert bcm enum to corresponding dbal enum
     */
    switch (pcp_mode)
    {
        case bcmFabricPcpModeNoPacking:
        {
            fabric_pcp_mode = DBAL_ENUM_FVAL_FABRIC_PCP_MODE_NO_PACKING;
            break;
        }
        case bcmFabricPcpModeSimple:
        {
            fabric_pcp_mode = DBAL_ENUM_FVAL_FABRIC_PCP_MODE_SIMPLE;
            break;
        }
        case bcmFabricPcpModeContinuous:
        {
            fabric_pcp_mode = DBAL_ENUM_FVAL_FABRIC_PCP_MODE_CONTINUOUS;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown PCP mode\n");
        }
    }

    /*
     * Set pcp mode for modid
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_PCP_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FAP_ID, modid);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_PCP_MODE, INST_SINGLE, fabric_pcp_mode);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get Fabric PCP (Packet Cell Packing) mode per destination device.
 *   There are 3 supported PCP modes:
 *     0 - No packing.
 *     1 - Simple packing.
 *     2 - Continuous packing.
 */
static shr_error_e
dnx_fabric_pcp_dest_mode_config_get(
    int unit,
    bcm_module_t modid,
    bcm_fabric_pcp_mode_t * pcp_mode)
{
    uint32 entry_handle_id;
    dbal_enum_value_field_fabric_pcp_mode_e fabric_pcp_mode;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get pcp mode for modid
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_PCP_CONFIG, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FAP_ID, modid);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_FABRIC_PCP_MODE, INST_SINGLE, &fabric_pcp_mode);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Convert dbal enum to corresponding bcm enum
     */
    switch (fabric_pcp_mode)
    {
        case DBAL_ENUM_FVAL_FABRIC_PCP_MODE_NO_PACKING:
        {
            *pcp_mode = bcmFabricPcpModeNoPacking;
            break;
        }
        case DBAL_ENUM_FVAL_FABRIC_PCP_MODE_SIMPLE:
        {
            *pcp_mode = bcmFabricPcpModeSimple;
            break;
        }
        case DBAL_ENUM_FVAL_FABRIC_PCP_MODE_CONTINUOUS:
        {
            *pcp_mode = bcmFabricPcpModeContinuous;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown fabric PCP mode\n");
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Verify params of bcm_fabric_pcp_dest_mode_config_set/get
 */
static shr_error_e
dnx_fabric_pcp_dest_mode_config_verify(
    int unit,
    uint32 flags,
    bcm_module_t modid,
    bcm_fabric_pcp_mode_config_t * pcp_config)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_MASK_VERIFY(flags, 0, _SHR_E_PARAM, "Unrecognized flags");

    SHR_RANGE_VERIFY(modid, 0, dnx_data_device.general.nof_faps_get(unit) - 1, _SHR_E_PARAM, "modid out of bound.\n");

    SHR_NULL_CHECK(pcp_config, _SHR_E_PARAM, "pcp_config");

exit:
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_pcp_dest_mode_config_set(
    int unit,
    uint32 flags,
    bcm_module_t modid,
    bcm_fabric_pcp_mode_config_t * pcp_config)
{
    int is_local;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    if (dnx_data_fabric.cell.pcp_enable_get(unit) == 1)
    {
        SHR_INVOKE_VERIFY_DNX(dnx_fabric_pcp_dest_mode_config_verify(unit, flags, modid, pcp_config));

        switch (pcp_config->pcp_mode)
        {
            case bcmFabricPcpModeNoPacking:
            case bcmFabricPcpModeSimple:
            case bcmFabricPcpModeContinuous:
            {
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "pcp_mode is invalid\n");
            }
        }

        /*
         * Local modids must be configured with continuous pcp mode
         */
        SHR_IF_ERR_EXIT(dnx_stk_sys_modid_is_local_get(unit, modid, &is_local));
        if (is_local)
        {
            if (pcp_config->pcp_mode != bcmFabricPcpModeContinuous)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "pcp_mode must be continuous for local modids\n");
            }
        }

        SHR_IF_ERR_EXIT(dnx_fabric_pcp_dest_mode_config_set(unit, modid, pcp_config->pcp_mode));
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "fabric_pcp is disabled");
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_pcp_dest_mode_config_get(
    int unit,
    uint32 flags,
    bcm_module_t modid,
    bcm_fabric_pcp_mode_config_t * pcp_config)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    if (dnx_data_fabric.cell.pcp_enable_get(unit) == 1)
    {
        SHR_INVOKE_VERIFY_DNX(dnx_fabric_pcp_dest_mode_config_verify(unit, flags, modid, pcp_config));

        SHR_IF_ERR_EXIT(dnx_fabric_pcp_dest_mode_config_get(unit, modid, &pcp_config->pcp_mode));

    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "fabric_pcp is disabled");
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fabric_wfq_set(
    int unit,
    int pipe,
    int weight,
    dnx_fabric_wfq_type_e fabric_wfq_type)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * configure ingress part (FDT)
     */
    if (fabric_wfq_type != DNX_FABRIC_WFQ_TYPE_PIPE_EGRESS)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_WFQ_CONTEXTS_WEIGHTS, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_ID, pipe);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WEIGHT, INST_SINGLE, weight);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
    }

    /*
     * configure Egress part (FDR)
     */
    if (fabric_wfq_type != DNX_FABRIC_WFQ_TYPE_CONTEXT_INGRESS)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_WFQ_PIPES_WEIGHTS, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PIPE_ID, pipe);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WEIGHT, INST_SINGLE, weight);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fabric_wfq_get(
    int unit,
    int pipe,
    int *weight,
    dnx_fabric_wfq_type_e fabric_wfq_type)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get Egress WFQ (FDR)
     */
    if (fabric_wfq_type == DNX_FABRIC_WFQ_TYPE_PIPE_EGRESS)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_WFQ_PIPES_WEIGHTS, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PIPE_ID, pipe);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_WEIGHT, INST_SINGLE, (uint32 *) weight);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }
    /*
     * Get Ingress WFQ (FDT)
     * (also when calling with fabric_wfq_type=DNX_FABRIC_WFQ_TYPE_ALL)
     */
    else
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_WFQ_CONTEXTS_WEIGHTS, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CONTEXT_ID, pipe);
        dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_WEIGHT, INST_SINGLE, (uint32 *) weight);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fabric_wfq_dynamic_set(
    int unit,
    dbal_enum_value_field_fabric_wfq_fifo_e fabric_wfq_fifo,
    int is_congested,
    int weight)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_WFQ_DYNAMIC_WEIGHTS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_CONGESTED, is_congested);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_WFQ_FIFO, fabric_wfq_fifo);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WEIGHT, INST_SINGLE, weight);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See .h file
 */
shr_error_e
dnx_fabric_wfq_dynamic_get(
    int unit,
    dbal_enum_value_field_fabric_wfq_fifo_e fabric_wfq_fifo,
    int is_congested,
    int *weight)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_WFQ_DYNAMIC_WEIGHTS, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_IS_CONGESTED, is_congested);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FABRIC_WFQ_FIFO, fabric_wfq_fifo);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_WEIGHT, INST_SINGLE, (uint32 *) weight);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * System RED
 * set discard configuration. Configuration to enable/ disable aging and
 * its settings
 */
shr_error_e
bcm_dnx_fabric_config_discard_set(
    int unit,
    bcm_fabric_config_discard_t * discard)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    SHR_IF_ERR_EXIT(dnx_system_red_aging_config_set(unit, discard));

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

/* get system RED discard configuration. */
shr_error_e
bcm_dnx_fabric_config_discard_get(
    int unit,
    bcm_fabric_config_discard_t * discard)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_IF_ERR_EXIT(dnx_system_red_aging_config_get(unit, discard));

exit:
    SHR_FUNC_EXIT;
}
/*
 * Number of bits needed for setting minimum number of links per FAP.
 * 112 links requires 7 bits.
 */
#define SOC_JR2_FABRIC_MIN_NOF_LINKS_BITS_NOF  (7)

/**
 * \brief
 *   Initialize minimal links per destination device feature.
 * \param [in] unit -
 *   The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * Called as part of the initialization sequence.
 * \see
 *   * None.
 */
static shr_error_e
dnx_fabric_minimal_links_to_dest_init(
    int unit)
{
    uint32 entry_handle_id;
    uint32 enable = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Minimum number of links feature is not supported in Mesh mode.
     */
    enable = (dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_FE) ? 1 : 0;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MIN_LINKS_TO_DEST_INIT, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set minimal links per destination device.
 *   Once the number of active links of a destination device is lower than
 *   the configurable value, all of its cells will be dropped.
 * \param [in] unit -
 *   The unit number.
 * \param [in] module_id -
 *   Module id of the device to configure.
 *   SOC_MODID_ALL will configure all devices.
 * \param [in] min_links -
 *   The number of links to configure as minimum.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
static shr_error_e
dnx_fabric_minimal_links_to_dest_set(
    int unit,
    bcm_module_t module_id,
    int min_links)
{
    uint32 entry_handle_id;
    int modid = 0;
    int modid_min = 0;
    int modid_max = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Set minimum number of links for all modids
     */
    if (module_id == BCM_MODID_ALL)
    {
        modid_min = 0;
        modid_max = dnx_data_device.general.nof_faps_get(unit) - 1;
    }
    /*
     * Set minimum number of links for a specific modid
     */
    else
    {
        modid_min = module_id;
        modid_max = module_id;
    }

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MIN_LINKS_TO_DEST, &entry_handle_id));
    for (modid = modid_min; modid <= modid_max; ++modid)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FAP_ID, modid);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_NOF_LINKS, INST_SINGLE, min_links);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get minimal links per destination device.
 *   Once the number of active links of a destination device is lower than
 *   the configurable value, all of its cells will be dropped.
 * \param [in] unit -
 *   The unit number.
 * \param [in] module_id -
 *   Module id of the device to get configuration.
 *   SOC_MODID_ALL will get configuration of device 0 (all devices was
 *   configured the same).
 * \param [out] min_links -
 *   The number of links that was configured as minimum.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
static shr_error_e
dnx_fabric_minimal_links_to_dest_get(
    int unit,
    bcm_module_t module_id,
    int *min_links)
{
    uint32 entry_handle_id;
    int modid = 0;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    modid = (module_id == BCM_MODID_ALL) ? 0 : module_id;

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MIN_LINKS_TO_DEST, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_FAP_ID, modid);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MIN_NOF_LINKS, INST_SINGLE, (uint32 *) min_links);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set minimal links for all-reachable vactor.
 *   In calculation of multicast distribution links,
 *   ignore FAPs with number of links below this number.
 * \param [in] unit -
 *   The unit number.
 * \param [in] min_links -
 *   The number of links to configure as minimum.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e
dnx_fabric_minimal_links_all_reachable_set(
    int unit,
    int min_links)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MIN_LINKS_MULTICAST, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_NOF_LINKS, INST_SINGLE, min_links);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get minimal links for all-reachable vactor.
 *   In calculation of multicast distribution links,
 *   ignore FAPs with number of links below this number.
 * \param [in] unit -
 *   The unit number.
 * \param [out] min_links -
 *   The number of links that was configured as minimum.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
shr_error_e
dnx_fabric_minimal_links_all_reachable_get(
    int unit,
    int *min_links)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_MIN_LINKS_MULTICAST, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_MIN_NOF_LINKS, INST_SINGLE, (uint32 *) min_links);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_destination_link_min_set(
    int unit,
    uint32 flags,
    bcm_module_t module_id,
    int num_of_links)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    if (!dnx_data_fabric.links.nof_links_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "bcm_fabric_destination_link_min_set is not supported for devices without fabric links\n");
    }

    if (dnx_data_fabric.general.connect_mode_get(unit) != DNX_FABRIC_CONNECT_MODE_FE)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "minimum number of links configuration not supported in mesh");
    }

    SHR_MASK_VERIFY(flags, BCM_FABRIC_DESTINATION_LINK_MIN_ALL_REACHABLE, _SHR_E_PARAM, "Unrecognized flags");

    SHR_RANGE_VERIFY(num_of_links, 0, dnx_data_fabric.links.nof_links_get(unit), _SHR_E_PARAM,
                     "number of links is invalid");

    if (flags & BCM_FABRIC_DESTINATION_LINK_MIN_ALL_REACHABLE)
    {
        if (module_id != BCM_MODID_ALL && module_id != -1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Please set module_id to be -1 or BCM_MODID_ALL, "
                         "since the configuration is done for all module ids when configuring the all-reachable-vector.");
        }

        SHR_IF_ERR_EXIT(dnx_fabric_minimal_links_all_reachable_set(unit, num_of_links));
    }
    else
    {
        if (module_id != BCM_MODID_ALL)
        {
            SHR_RANGE_VERIFY(module_id, 0, dnx_data_device.general.nof_faps_get(unit) - 1, _SHR_E_PARAM,
                             "module id %d is invalid", module_id);
        }

        SHR_IF_ERR_EXIT(dnx_fabric_minimal_links_to_dest_set(unit, module_id, num_of_links));
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    SHR_FUNC_EXIT;
}

int
bcm_dnx_fabric_destination_link_min_get(
    int unit,
    uint32 flags,
    bcm_module_t module_id,
    int *num_of_links)
{
    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    if (!dnx_data_fabric.links.nof_links_get(unit))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "bcm_fabric_destination_link_min_get is not supported for devices without fabric links\n");
    }

    SHR_NULL_CHECK(num_of_links, _SHR_E_PARAM, "num_of_links");

    if (dnx_data_fabric.general.connect_mode_get(unit) != DNX_FABRIC_CONNECT_MODE_FE)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "minimum number of links configuration not supported in mesh");
    }

    SHR_MASK_VERIFY(flags, BCM_FABRIC_DESTINATION_LINK_MIN_ALL_REACHABLE, _SHR_E_PARAM, "Unrecognized flags");

    if (flags & BCM_FABRIC_DESTINATION_LINK_MIN_ALL_REACHABLE)
    {
        if (module_id != BCM_MODID_ALL && module_id != -1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Please set module_id to be -1 or BCM_MODID_ALL, "
                         "since the configuration is done for all module ids when configuring the all-reachable-vector.");
        }

        SHR_IF_ERR_EXIT(dnx_fabric_minimal_links_all_reachable_get(unit, num_of_links));
    }
    else
    {
        if (module_id != BCM_MODID_ALL)
        {
            SHR_RANGE_VERIFY(module_id, 0, dnx_data_device.general.nof_faps_get(unit) - 1, _SHR_E_PARAM,
                             "module id %d is invalid", module_id);
        }

        SHR_IF_ERR_EXIT(dnx_fabric_minimal_links_to_dest_get(unit, module_id, num_of_links));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Set whether a single cell should be delayed in Fabric RX
 *   until a second cell will arrive (or timeout).
 *   This feature should be enabled when there is a JR1 device
 *   in the system.
 *   The amount of time to wait for a second cell is configured
 *   in the init sequence.
 * \param [in] unit -
 *   The unit number.
 * \param [in] enable -
 *   1 - enable delay.
 *   0 - disable delay.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
static shr_error_e
dnx_fabric_delay_single_cell_in_fabric_rx_enable_set(
    int unit,
    int enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_DELAY_SINGLE_CELL_IN_FABRIC_RX, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, enable ? 1 : 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get whether a single cell should be delayed in Fabric RX
 *   until a second cell will arrive (or timeout).
 * \param [in] unit -
 *   The unit number.
 * \param [out] enable -
 *   1 - enable delay.
 *   0 - disable delay.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   * None.
 * \see
 *   * None.
 */
static shr_error_e
dnx_fabric_delay_single_cell_in_fabric_rx_enable_get(
    int unit,
    int *enable)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_DELAY_SINGLE_CELL_IN_FABRIC_RX, &entry_handle_id));
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_ENABLE, INST_SINGLE, (uint32 *) enable);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * See fabric.h
 */
shr_error_e
dnx_fabric_blocks_power_down_set(
    int unit,
    int power_down)
{
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_POWER_DOWN, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_POWER_DOWN, INST_SINGLE, power_down);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get interface gport
 *
 * \param [in] unit - Unit #
 * \param [in] child_port - port or port gport
 * \param [flags] flags - unused
 * \param [out] parent_port - interface gport
 *
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_fabric_port_get(
    int unit,
    bcm_gport_t child_port,
    uint32 flags,
    bcm_gport_t * parent_port)
{
    bcm_port_t port = 0, tm_port;
    int core;

    SHR_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_NOT_NEEDED(unit);

    SHR_NULL_CHECK(parent_port, _SHR_E_PARAM, "parent_port");

    if (BCM_GPORT_IS_LOCAL(child_port))
    {
        /** IN Local gport -> OUT Local Interface gport  */
        port = BCM_GPORT_LOCAL_GET(child_port);
        BCM_GPORT_LOCAL_INTERFACE_SET(*parent_port, port);

    }
    else if (BCM_GPORT_IS_MODPORT(child_port))
    {

        SHR_IF_ERR_EXIT(dnx_stk_sys_modport_gport_to_tm_port_convert(unit, child_port, &core, &tm_port));

        SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, &port));

        BCM_GPORT_LOCAL_INTERFACE_SET(*parent_port, port);

    }
    else if (BCM_COSQ_GPORT_IS_E2E_PORT(child_port))
    {
        /** IN E2E gport -> OUT E2E Interface gport  */
        port = BCM_COSQ_GPORT_E2E_PORT_GET(child_port);
        BCM_COSQ_GPORT_E2E_INTERFACE_SET(*parent_port, port);
    }
    else
    {
        port = child_port;
        SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));
        BCM_GPORT_LOCAL_INTERFACE_SET(*parent_port, port);
    }

exit:
    SHR_FUNC_EXIT;
}

static int
bcm_dnx_fabric_stat_get_verify(
    int unit,
    uint32 flags,
    bcm_fabric_stat_index_t index,
    bcm_fabric_stat_t stat,
    uint64 *value)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check if index type is device 
     */
    if (!_SHR_FABRIC_STAT_INDEX_IS_DEVICE(index))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "bcm_fabric_stat_index_t type you passed is not supported.");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Get Fabric statistics
 *
 * \param [in] unit - Unit #
 * \param [flags] flags - unused
 * \param [in] index - Conains encoded counter type
 * \param [in] stat - Counter enum
 * \param [out] value - value
 *
 * \return
 *     see shr_error_e
 * \remark
 *   * None
 * \see
 *   * None
 */
int
bcm_dnx_fabric_stat_get(
    int unit,
    uint32 flags,
    bcm_fabric_stat_index_t index,
    bcm_fabric_stat_t stat,
    uint64 *value)
{
    uint32 entry_handle_id;
    uint16 count;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    DNX_ERR_RECOVERY_START(unit);

    COMPILER_64_ZERO(*value);

    SHR_INVOKE_VERIFY_DNX(bcm_dnx_fabric_stat_get_verify(unit, flags, index, stat, value));

    switch (stat)
    {
        case bcmFabricStatDeviceReachDrop:
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, DBAL_TABLE_FABRIC_STAT_UNREACH_DEST_DISCARD_CELLS_CNT, &entry_handle_id));

            dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_FABRIC_STAT_UNREACH_DEST_CNT,
                                       INST_SINGLE, &count);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            COMPILER_64_ADD_32(*value, count);
            break;
        case bcmFabricStatDeviceRciWmkLvl:
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_FABRIC_STAT_RCI_SCORES_INFO, &entry_handle_id));
            dbal_value_field16_request(unit, entry_handle_id, DBAL_FIELD_FABRIC_STAT_RCI_HIGH_SEV_MAX_SCORE_ALL,
                                       INST_SINGLE, &count);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));
            COMPILER_64_ADD_32(*value, count);
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Enum %d you passed with stat is not supported with LINK index type.", stat);
    }

exit:
    DNX_ERR_RECOVERY_END(unit);
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
