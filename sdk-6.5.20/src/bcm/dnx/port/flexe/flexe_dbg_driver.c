/** \file flexe_dbg_driver.c
 *
 * Functions for FlexE adapter layer
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_FLEXE

/*************
 * INCLUDES  *
 */
/* { */

#include <shared/shrextend/shrextend_debug.h>
#include <shared/util.h>

#include <bcm/port.h>
#ifdef INCLUDE_FLEXE_DBG
#include <bcm_int/dnx/port/flexe/flexe_core.h>
#include <bcm_int/dnx/port/flexe/flexe_dbg.h>

/** FlexE core includes*/
#include <module_sar.h>
#include <module_demux.h>
#include <module_mux.h>
#include <module_cpb.h>
#include <module_66bswitch.h>
#include <oh_rx.h>
#include <oh_tx.h>
#include <flexe_multi_deskew.h>
#include <top.h>
#include <channelize.h>
#include <flexe_mux.h>
#include <flexe_env.h>
#include <datatype.h>
#include <hal.h>
#include <flexe_oam_rx.h>
#include <flexe_oam_tx.h>
#include <oam_rx.h>
#include <mcmactx.h>
#include <mcmacrx.h>
#include <b66sar_rx_tx.h>
#include <reg_operate.h>
#include <req_gen.h>
#include <ccu_cfg.h>
#include <b66switch.h>
#include <inf_ch_adp_tx.h>
#include <inf_ch_adp_rx.h>
#include <ohoam_ef.h>
#include <flexe_demux.h>
#include <oam_tx.h>
#include <module_flexe_oh.h>
#include <module_oam.h>
#include <module_init.h>
#include <module_rateadpt.h>
#include <flexe_macrx.h>

/* } */

/*************
 * DEFINES   *
 */
/* { */

#define DNX_FLEXE_MAX_NOF_PHYS           (8)
#define DNX_FLEXE_MAP_PHY_MAP_OCTETS     (32)
#define DNX_FLEXE_MAX_SLOTS_PER_INS      (20)

/* } */

/*************
 *  MACROS   *
 */
/* { */

#define DNX_FLEXE_IF_ERR_EXIT(expr)                        \
    do                                                     \
    {                                                      \
        int rv = expr;                                     \
        int err_translate[] = {_SHR_E_NONE,                \
                               _SHR_E_FAIL,                \
                               _SHR_E_PARAM,               \
                               _SHR_E_INIT,                \
                               _SHR_E_INIT,                \
                               _SHR_E_MEMORY,              \
                               _SHR_E_MEMORY,              \
                               _SHR_E_PARAM,               \
                               _SHR_E_UNAVAIL,             \
                               _SHR_E_UNAVAIL};            \
        SHR_IF_ERR_EXIT(err_translate[rv]);                \
    }                                                      \
    while(0)

/* } */

/*************
 * TYPE DEFS *
 */
/* { */

typedef struct dnx_flexe_core_port_db_s
{
    int speed;
    int logical_phy_id;
    int flexe_local_port;
    soc_pbmp_t flexe_phys;
    flexe_phy_mode phy_mode;
    int group_index;
} dnx_flexe_core_port_db_t;

/* } */

/*************
 * GLOBALS   *
 */
/* { */

/*
 * Global struct to store FlexE core port info
 */
static dnx_flexe_core_port_db_t port_mapping[SOC_MAX_NUM_DEVICES][DNX_FLEXE_MAX_NOF_PHYS];

/* } */

/*************
 * FUNCTIONS *
 */
/* { */

static shr_error_e flexe_dbg_oh_client_id_clear(
    int unit,
    int cal_id,
    const int *flexe_core_port_array,
    bcm_port_flexe_time_slot_t ts_mask,
    int nof_flexe_core_ports);

/**
 * \brief - FlexE init function
 */
shr_error_e
flexe_dbg_init(
    int unit)
{
    int phy_index;

    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(module_init(unit));

    /** soh to mac chan id */
    SHR_IF_ERR_EXIT(b66switch_chid_cfg_set(unit, 0, 80));
    /** 1588 to mac chan id */
    SHR_IF_ERR_EXIT(b66switch_chid_cfg_set(unit, 1, 81));
    /** mac to soh chan id */
    SHR_IF_ERR_EXIT(b66switch_chid_cfg_set(unit, 2, 80));
    /** mac to 1588 chan id */
    SHR_IF_ERR_EXIT(b66switch_chid_cfg_set(unit, 3, 81));

    for (phy_index = 0; phy_index < DNX_FLEXE_MAX_NOF_PHYS; ++phy_index)
    {
        port_mapping[unit][phy_index].group_index = -1;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - FlexE deinit function
 */
shr_error_e
flexe_dbg_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(module_uninit(unit));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure FlexE env
 */
static shr_error_e
flexe_dbg_env_set(
    int unit,
    int flexe_local_port,
    flexe_core_port_speed_mode_t speed_mode,
    int is_bypass)
{
    flexe_env_mode mode;

    SHR_FUNC_INIT_VARS(unit);

    switch (speed_mode)
    {
        case flexe_core_port_speed_mode_50G:
            if (is_bypass)
            {
                mode = _50GE_50GE;
            }
            else
            {
                mode = _50GE_50INSTANCE;
            }
            break;
        case flexe_core_port_speed_mode_100G:
            if (is_bypass)
            {
                mode = _100GE_100GE;
            }
            else
            {
                mode = _100GE_50INSTANCE;
            }
            break;
        case flexe_core_port_speed_mode_200G:
            if (is_bypass)
            {
                mode = _200GE_200GE;
            }
            else
            {
                mode = _200GE_50INSTANCE;
            }
            break;
        case flexe_core_port_speed_mode_400G:
            if (is_bypass)
            {
                mode = _400GE_400GE;
            }
            else
            {
                mode = _400GE_50INSTANCE;
            }
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported speed.\n");
    }

    DNX_FLEXE_IF_ERR_EXIT(flexe_env_cfg(unit, 1, flexe_local_port, mode));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get FlexE SPEED mode
 */
static shr_error_e
flexe_dbg_speed_mode_get(
    int unit,
    int speed,
    flexe_core_port_speed_mode_t * flexe_core_port_speed_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    switch (speed)
    {
        case 50000:
            *flexe_core_port_speed_mode = flexe_core_port_speed_mode_50G;
            break;
        case 100000:
            *flexe_core_port_speed_mode = flexe_core_port_speed_mode_100G;
            break;
        case 200000:
            *flexe_core_port_speed_mode = flexe_core_port_speed_mode_200G;
            break;
        case 400000:
            *flexe_core_port_speed_mode = flexe_core_port_speed_mode_400G;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported speed.\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create FlexE core port mapping
 */
static shr_error_e
flexe_dbg_port_mapping_create(
    int unit,
    int flexe_core_port,
    int speed,
    int logical_phy_id)
{
    int i, nof_instance, phy_index;

    SHR_FUNC_INIT_VARS(unit);

    port_mapping[unit][flexe_core_port].flexe_local_port = flexe_core_port;

    if ((speed == 50000) || (speed == 100000))
    {
        nof_instance = 1;
    }
    else if (speed == 200000)
    {
        nof_instance = 2;
    }
    else
    {
        nof_instance = 4;
    }
    port_mapping[unit][flexe_core_port].speed = speed;

    phy_index = flexe_core_port;
    _SHR_PBMP_CLEAR(port_mapping[unit][flexe_core_port].flexe_phys);
    for (i = 0; i < nof_instance; ++i)
    {
        _SHR_PBMP_PORT_ADD(port_mapping[unit][flexe_core_port].flexe_phys, phy_index);
        phy_index++;
    }
    port_mapping[unit][flexe_core_port].logical_phy_id = logical_phy_id;

    SHR_FUNC_EXIT;
}

/**
 * \brief - Get FlexE phy mode
 */
static shr_error_e
flexe_dbg_phy_mode_get(
    int unit,
    int speed,
    int is_bypass,
    flexe_phy_mode * phy_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    if (!is_bypass)
    {
        if (speed == 50000)
        {
            *phy_mode = FLEXE_PHY_50G;
        }
        else
        {
            *phy_mode = FLEXE_PHY_100G;
        }
    }
    else
    {
        if (speed == 50000)
        {
            *phy_mode = PCS_PHY_50G;
        }
        else if (speed == 100000)
        {
            *phy_mode = PCS_PHY_100G;
        }
        else if (speed == 200000)
        {
            *phy_mode = PCS_PHY_200G;
        }
        else
        {
            *phy_mode = PCS_PHY_400G;
        }
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - Get FlexE instance ID
 */
static shr_error_e
flexe_dbg_logical_phy_id_to_instance_id_get(
    int unit,
    int speed,
    uint8 logical_phy_id,
    uint8 instance_index,
    uint8 *instance_id)
{
    SHR_FUNC_INIT_VARS(unit);

    *instance_id = 0;
    if ((speed == 50000) || (speed == 100000))
    {
        *instance_id = logical_phy_id;
    }
    else if (speed == 200000)
    {
        *instance_id = (logical_phy_id << 1) + instance_index;
    }
    else
    {
        *instance_id = (logical_phy_id << 2) + instance_index;
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - Create time slots for each FlexE PHY instance
 */
static shr_error_e
flexe_dbg_phy_instance_ts_get(
    int unit,
    const int *flexe_core_port_array,
    bcm_port_flexe_time_slot_t ts_mask,
    int nof_flexe_core_ports,
    uint8 *phy_array,
    uint32 *phy_ts_mask,
    int *total_nof_phys)
{
    int i, max_slots_per_instance, phy_index, nof_phys;
    bcm_port_flexe_time_slot_t tmp_ts_mask;

    SHR_FUNC_INIT_VARS(unit);

    *total_nof_phys = 0;
    nof_phys = 0;
    max_slots_per_instance = (port_mapping[unit][flexe_core_port_array[0]].phy_mode == FLEXE_PHY_100G) ? 20 : 10;

    for (i = 0; i < nof_flexe_core_ports; ++i)
    {
        _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port_array[i]].flexe_phys, phy_index)
        {
            SHR_BITCLR_RANGE(tmp_ts_mask, 0, BCM_PORT_FLEXE_MAX_NOF_SLOTS);
            /*
             * Get the time slots
             */
            SHR_BITCOPY_RANGE(tmp_ts_mask, 0, ts_mask, nof_phys * max_slots_per_instance, max_slots_per_instance);
            /*
             * Update only when there is valid TS
             */
            if (!SHR_BITNULL_RANGE(tmp_ts_mask, 0, BCM_PORT_FLEXE_MAX_NOF_SLOTS))
            {
                SHR_BITCOPY_RANGE(&phy_ts_mask[*total_nof_phys], 0, tmp_ts_mask, 0, max_slots_per_instance);

                phy_array[*total_nof_phys] = phy_index;
                (*total_nof_phys)++;
            }
            nof_phys++;
        }
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - Get FlexE PHY ID
 */
static shr_error_e
flexe_dbg_instance_id_to_logical_phy_id_get(
    int unit,
    int speed,
    int instance_id,
    int *logical_phy_id)
{
    SHR_FUNC_INIT_VARS(unit);

    *logical_phy_id = 0;
    if ((speed == 50000) || (speed == 100000))
    {
        *logical_phy_id = instance_id;
    }
    else if (speed == 200000)
    {
        *logical_phy_id = (instance_id >> 1);
    }
    else
    {
        *logical_phy_id = (instance_id >> 2);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get all the phy instances in one group
 */
static shr_error_e
flexe_dbg_group_instances_get(
    int unit,
    int group_index,
    uint8 *instances)
{
    int i, phy_index;

    SHR_FUNC_INIT_VARS(unit);

    *instances = 0;

    for (i = 0; i < DNX_FLEXE_MAX_NOF_PHYS; ++i)
    {
        if (port_mapping[unit][i].group_index == group_index)
        {
            _SHR_PBMP_ITER(port_mapping[unit][i].flexe_phys, phy_index)
            {
                SHR_BITSET(instances, phy_index);
            }
        }
    }

    SHR_FUNC_EXIT;
}

/**
 * \brief - Add phys to FlexE group
 */
shr_error_e
flexe_dbg_group_phy_add(
    int unit,
    uint32 flags,
    int group_index,
    soc_pbmp_t flexe_core_port_bmp,
    const int *logical_phy_id,
    const flexe_core_port_info_t * port_info)
{
    int flexe_core_port = 0, phy_index, first_phy, is_flexe_200g = 0, instance_idx;
    int inst[DNX_FLEXE_MAX_NOF_PHYS][2] = { {0} }, nof_phys;
    uint8 phy_bmp[1] = { 0 }, phy_id[DNX_FLEXE_MAX_NOF_PHYS] =
    {
    0}, all_phys;
    flexe_phy_mode phy_mode = 0;
    flexe_core_port_speed_mode_t speed_mode;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Create PORT mapping
     */
    is_flexe_200g = ((port_info->speed == 200000) && !(port_info->is_bypass));

    nof_phys = 0;
    _SHR_PBMP_ITER(flexe_core_port_bmp, flexe_core_port)
    {
        if (flexe_core_port >= DNX_FLEXE_MAX_NOF_PHYS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "FlexE core port %d is out of range.\n", flexe_core_port);
        }
        SHR_IF_ERR_EXIT(flexe_dbg_port_mapping_create
                        (unit, flexe_core_port, port_info->speed, logical_phy_id[flexe_core_port]));
        /*
         * Prepare data for demux/mux APIs
         */
        instance_idx = 0;
        _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port].flexe_phys, phy_index)
        {
             /* coverity[overrun-local : FALSE]  */
            SHR_BITSET(phy_bmp, phy_index);
            SHR_IF_ERR_EXIT(flexe_dbg_logical_phy_id_to_instance_id_get
                            (unit, port_info->speed, logical_phy_id[flexe_core_port], instance_idx, &phy_id[nof_phys]));
            if (is_flexe_200g)
            {
                inst[flexe_core_port][instance_idx] = phy_index;
            }
            ++instance_idx;
            ++nof_phys;
        }
        /*
         * Get PHY mode
         */
        SHR_IF_ERR_EXIT(flexe_dbg_phy_mode_get(unit, port_info->speed, port_info->is_bypass, &phy_mode));
        port_mapping[unit][flexe_core_port].phy_mode = phy_mode;
        port_mapping[unit][flexe_core_port].group_index = group_index;
    }
    /*
     * Get speed mode
     */
    SHR_IF_ERR_EXIT(flexe_dbg_speed_mode_get(unit, port_info->speed, &speed_mode));

    if (flags & BCM_PORT_FLEXE_RX)
    {
        /*
         * Configure Rx PHY
         */
        if (!port_info->is_bypass)
        {
            DNX_FLEXE_IF_ERR_EXIT(demux_phy_group_cfg(unit, group_index, phy_bmp[0], phy_id, phy_mode, 1));
            DNX_FLEXE_IF_ERR_EXIT(flexe_multi_deskew_grp_mode_set(unit, group_index, phy_mode));
            /*
             * Get all the PHY instances in the group
             */
             /* coverity[callee_ptr_arith : FALSE]  */
            SHR_IF_ERR_EXIT(flexe_dbg_group_instances_get(unit, group_index, &all_phys));
            DNX_FLEXE_IF_ERR_EXIT(flexe_multi_deskew_grp_cfg_set(unit, group_index, all_phys));
        }
        _SHR_PBMP_ITER(flexe_core_port_bmp, flexe_core_port)
        {
            if (!port_info->is_bypass)
            {
                _SHR_PBMP_FIRST(port_mapping[unit][flexe_core_port].flexe_phys, first_phy);
                if (is_flexe_200g)
                {
                     /* coverity[overrun-local : FALSE]  */
                    DNX_FLEXE_IF_ERR_EXIT(inf_ch_adp_rx_cfg
                                          (unit, 1, flexe_core_port, first_phy, inst[flexe_core_port][0],
                                           inst[flexe_core_port][1], speed_mode, port_info->serdes_rate_mode));
                }
                else
                {
                    DNX_FLEXE_IF_ERR_EXIT(inf_ch_adp_rx_cfg
                                          (unit, 1, flexe_core_port, first_phy, 0, 0, speed_mode,
                                           port_info->serdes_rate_mode));
                }
                _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port].flexe_phys, phy_index)
                {
                    DNX_FLEXE_IF_ERR_EXIT(flexe_oh_port_mode_set(unit, phy_index, speed_mode));
                }
            }
            else
            {
                DNX_FLEXE_IF_ERR_EXIT(channelize_bypass_en_set(unit, flexe_core_port, 1));
            }
        }
    }

    if (flags & BCM_PORT_FLEXE_TX)
    {
        if (phy_mode != PCS_PHY_400G)
        {
            DNX_FLEXE_IF_ERR_EXIT(mux_phy_group_cfg(unit, group_index, phy_bmp[0], phy_mode, 1));
        }
        nof_phys = 0;
        _SHR_PBMP_ITER(flexe_core_port_bmp, flexe_core_port)
        {
            _SHR_PBMP_FIRST(port_mapping[unit][flexe_core_port].flexe_phys, first_phy);

            if (port_info->is_bypass)
            {
                SHR_IF_ERR_EXIT(flexe_dbg_env_set(unit, flexe_core_port, speed_mode, 1));
            }
            else
            {
                if (is_flexe_200g)
                {
                    DNX_FLEXE_IF_ERR_EXIT(inf_ch_adp_tx_cfg
                                          (unit, 1, flexe_core_port, first_phy, inst[flexe_core_port][0],
                                           inst[flexe_core_port][1], speed_mode, 0, -1));
                }
                else
                {
                    DNX_FLEXE_IF_ERR_EXIT(inf_ch_adp_tx_cfg
                                          (unit, 1, flexe_core_port, first_phy, 0, 0, speed_mode, 0, -1));
                }
                _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port].flexe_phys, phy_index)
                {
                    DNX_FLEXE_IF_ERR_EXIT(flexe_oh_port_mode_set(unit, phy_index, speed_mode));
                }

                SHR_IF_ERR_EXIT(flexe_dbg_env_set(unit, first_phy, speed_mode, 0));
                if (is_flexe_200g)
                {
                   /** DNX_FLEXE_IF_ERR_EXIT(flexe_mux_en_sel_cfg_set(unit, inst[0], first_phy)); */
                   /** DNX_FLEXE_IF_ERR_EXIT(flexe_mux_en_sel_cfg_set(unit, inst[1], first_phy)); */
                }
                /*
                 * Update overhead
                 */
                _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port].flexe_phys, phy_index)
                {
                    DNX_FLEXE_IF_ERR_EXIT(flexe_oh_tx_pid_set(unit, phy_index, phy_id[nof_phys]));
                    DNX_FLEXE_IF_ERR_EXIT(flexe_oh_exp_pid_set(unit, phy_index, phy_id[nof_phys]));
                    nof_phys++;
                }
                SHR_IF_ERR_EXIT(flexe_dbg_oh_phymap_set(unit, flexe_core_port, logical_phy_id[flexe_core_port], 1));
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Remove PHY from FlexE group
 */
shr_error_e
flexe_dbg_group_phy_remove(
    int unit,
    uint32 flags,
    int group_index,
    soc_pbmp_t flexe_core_port_bmp)
{
    int flexe_core_port, phy_index, first_phy, is_flexe_200g = 0, instance_idx;
    int inst[DNX_FLEXE_MAX_NOF_PHYS][2] = { {0} }, nof_phys;
    int is_bypass = 0;
    uint8 phy_bmp[1] = { 0 }, phy_id[DNX_FLEXE_MAX_NOF_PHYS], all_phys[1] =
    {
    0};
    flexe_core_port_speed_mode_t speed_mode;
    flexe_phy_mode phy_mode;

    SHR_FUNC_INIT_VARS(unit);

    if (_SHR_PBMP_IS_NULL(flexe_core_port_bmp))
    {
        /*
         * Exit directly if there is no flexe core port
         */
        SHR_EXIT();
    }
    /*
     * Create PORT mapping
     */
    nof_phys = 0;
    _SHR_PBMP_ITER(flexe_core_port_bmp, flexe_core_port)
    {
        if (flexe_core_port >= DNX_FLEXE_MAX_NOF_PHYS)
        {
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "FlexE core port %d is out of range.\n", flexe_core_port);
        }

        is_bypass = ((port_mapping[unit][flexe_core_port].phy_mode == PCS_PHY_50G) ||
                     (port_mapping[unit][flexe_core_port].phy_mode == PCS_PHY_100G) ||
                     (port_mapping[unit][flexe_core_port].phy_mode == PCS_PHY_200G) ||
                     (port_mapping[unit][flexe_core_port].phy_mode == PCS_PHY_400G));
        is_flexe_200g = ((port_mapping[unit][flexe_core_port].speed == 200000) && !is_bypass);
        /*
         * Prepare data
         */
        instance_idx = 0;
        _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port].flexe_phys, phy_index)
        {
             /* coverity[overrun-local : FALSE]  */
            SHR_BITSET(phy_bmp, phy_index);
            SHR_IF_ERR_EXIT(flexe_dbg_logical_phy_id_to_instance_id_get
                            (unit, port_mapping[unit][flexe_core_port].speed,
                             port_mapping[unit][flexe_core_port].logical_phy_id, instance_idx, &phy_id[nof_phys]));
            if (is_flexe_200g)
            {
                inst[flexe_core_port][instance_idx] = phy_index;
            }
            ++instance_idx;
            ++nof_phys;
        }
    }
    /*
     * Get speed mode
     */
    _SHR_PBMP_FIRST(flexe_core_port_bmp, flexe_core_port);
    SHR_IF_ERR_EXIT(flexe_dbg_speed_mode_get(unit, port_mapping[unit][flexe_core_port].speed, &speed_mode));
    phy_mode = port_mapping[unit][flexe_core_port].phy_mode;

    if (flags & BCM_PORT_FLEXE_RX)
    {
        /*
         * Delete Rx PHY
         */
        if (!is_bypass)
        {
            _SHR_PBMP_ITER(flexe_core_port_bmp, flexe_core_port)
            {
                _SHR_PBMP_FIRST(port_mapping[unit][flexe_core_port].flexe_phys, first_phy);
                if (is_flexe_200g)
                {
                    /*
                     * For remove API, we don't care the serdes_rate_mode, set it to "0"
                     */
                    DNX_FLEXE_IF_ERR_EXIT(inf_ch_adp_rx_cfg
                                          (unit, 0, flexe_core_port, first_phy, inst[flexe_core_port][0],
                                           inst[flexe_core_port][1], speed_mode, 0));
                }
                else
                {
                    DNX_FLEXE_IF_ERR_EXIT(inf_ch_adp_rx_cfg(unit, 0, flexe_core_port, first_phy, 0, 0, speed_mode, 0));
                }
            }
            DNX_FLEXE_IF_ERR_EXIT(demux_phy_group_cfg(unit, group_index, phy_bmp[0], phy_id, phy_mode, 0));
            /*
             * Get all the PHY instances in the group
             */
            SHR_IF_ERR_EXIT(flexe_dbg_group_instances_get(unit, group_index, all_phys));
            SHR_BIT_ITER(phy_bmp, DNX_FLEXE_MAX_NOF_PHYS, phy_index)
            {
                SHR_BITCLR(all_phys, phy_index);
            }
            DNX_FLEXE_IF_ERR_EXIT(flexe_multi_deskew_grp_cfg_set(unit, group_index, all_phys[0]));
            if (all_phys[0] == 0)
            {
                DNX_FLEXE_IF_ERR_EXIT(flexe_multi_deskew_grp_mode_set(unit, group_index, 0));
            }
        }
        else
        {
            DNX_FLEXE_IF_ERR_EXIT(channelize_bypass_en_set(unit, flexe_core_port, 0));
        }
    }

    if (flags & BCM_PORT_FLEXE_TX)
    {
        if (phy_mode != PCS_PHY_400G)
        {
            DNX_FLEXE_IF_ERR_EXIT(mux_phy_group_cfg(unit, group_index, phy_bmp[0], phy_mode, 0));
        }
        if (!is_bypass)
        {
            /*
             * Delete TX PHY
             */
            _SHR_PBMP_ITER(flexe_core_port_bmp, flexe_core_port)
            {
                _SHR_PBMP_FIRST(port_mapping[unit][flexe_core_port].flexe_phys, first_phy);
                if (is_flexe_200g)
                {
                    /*
                     * Need to distinguish bypass mode or non-bypass mode
                     */
                    DNX_FLEXE_IF_ERR_EXIT(inf_ch_adp_tx_cfg
                                          (unit, 0, flexe_core_port, first_phy, inst[flexe_core_port][0],
                                           inst[flexe_core_port][1], speed_mode, 0, -1));
                }
                else
                {
                    /*
                     * Need to distinguish bypass mode or non-bypass mode
                     */
                    DNX_FLEXE_IF_ERR_EXIT(inf_ch_adp_tx_cfg
                                          (unit, 0, flexe_core_port, first_phy, 0, 0, speed_mode, 0, -1));
                }
            }

            /*
             * Update overhead and DB
             */
            _SHR_PBMP_ITER(flexe_core_port_bmp, flexe_core_port)
            {
                _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port].flexe_phys, phy_index)
                {
                    DNX_FLEXE_IF_ERR_EXIT(flexe_oh_tx_pid_set(unit, phy_index, 1));
                    DNX_FLEXE_IF_ERR_EXIT(flexe_oh_exp_pid_set(unit, phy_index, 1));
                }
                SHR_IF_ERR_EXIT(flexe_dbg_oh_phymap_set
                                (unit, flexe_core_port, port_mapping[unit][flexe_core_port].logical_phy_id, 0));
                SHR_IF_ERR_EXIT(flexe_dbg_oh_group_id_set(unit, flexe_core_port, 1));
            }
        }
        _SHR_PBMP_ITER(flexe_core_port_bmp, flexe_core_port)
        {
            port_mapping[unit][flexe_core_port].group_index = -1;
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add FlexE BusA client
 */
shr_error_e
flexe_dbg_client_add(
    int unit,
    int client_channel,
    uint32 flags,
    int cal_id,
    const int *flexe_core_port_array,
    bcm_port_flexe_time_slot_t ts_mask,
    int nof_flexe_core_ports)
{
    int nof_phys, is_bypass;
    uint8 phy_array[DNX_FLEXE_MAX_NOF_PHYS];
    uint32 phy_ts_mask[DNX_FLEXE_MAX_NOF_PHYS] = { 0 };
    flexe_core_port_speed_mode_t speed_mode;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Assign the time slots to phy instance
     */
    is_bypass = ((port_mapping[unit][flexe_core_port_array[0]].phy_mode == PCS_PHY_50G) ||
                 (port_mapping[unit][flexe_core_port_array[0]].phy_mode == PCS_PHY_100G) ||
                 (port_mapping[unit][flexe_core_port_array[0]].phy_mode == PCS_PHY_200G) ||
                 (port_mapping[unit][flexe_core_port_array[0]].phy_mode == PCS_PHY_400G));
    if (!is_bypass)
    {
        SHR_IF_ERR_EXIT(flexe_dbg_phy_instance_ts_get(unit, flexe_core_port_array, ts_mask,
                                                      nof_flexe_core_ports, phy_array, phy_ts_mask, &nof_phys));

        if (flags & BCM_PORT_FLEXE_RX)
        {
            if (cal_id == 0)
            {
                DNX_FLEXE_IF_ERR_EXIT(demux_traffic_flexe_cfg
                                      (unit, client_channel, phy_array, nof_phys, (UINT_32 *) phy_ts_mask, 1));
            }
            else
            {
                DNX_FLEXE_IF_ERR_EXIT(demux_traffic_flexe_b_cfg
                                      (unit, client_channel, phy_array, nof_phys, (UINT_32 *) phy_ts_mask, 1));
            }
            /*
             * The following API only updates SW, check with Jingxuan if we need it.
             * Or we need to call
             * If we need it, can we move it to OAM relate APIs
             */
            /** will put the following API into demux_traffic_flexe_cfg */
            /** DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_ts_sw_add(unit, client_channel, nof_slots * 5)); */
        }
        if (flags & BCM_PORT_FLEXE_TX)
        {
            if (cal_id == 0)
            {
                DNX_FLEXE_IF_ERR_EXIT(mux_traffic_flexe_cfg
                                      (unit, client_channel, phy_array, nof_phys, (UINT_32 *) phy_ts_mask, 1));
            }
            else
            {
                DNX_FLEXE_IF_ERR_EXIT(mux_traffic_flexe_b_cfg
                                      (unit, client_channel, phy_array, nof_phys, (UINT_32 *) phy_ts_mask, 1));
            }
        }
    }
    else
    {
        if (flags & BCM_PORT_FLEXE_RX)
        {
            /*
             * Configure it when adding client.
             */
            DNX_FLEXE_IF_ERR_EXIT(channelize_pcschid_cfg_set(unit, flexe_core_port_array[0], client_channel));
        }
        if (flags & BCM_PORT_FLEXE_TX)
        {
            SHR_IF_ERR_EXIT(flexe_dbg_speed_mode_get
                            (unit, port_mapping[unit][flexe_core_port_array[0]].speed, &speed_mode));
            DNX_FLEXE_IF_ERR_EXIT(inf_ch_adp_tx_cfg
                                  (unit, 1, flexe_core_port_array[0], flexe_core_port_array[0], 0, 0, speed_mode, 1,
                                   client_channel));
            /*
             * Configure PCS
             */
            if (port_mapping[unit][flexe_core_port_array[0]].phy_mode != PCS_PHY_400G)
            {
                DNX_FLEXE_IF_ERR_EXIT(mux_traffic_pcs_cfg
                                      (unit, client_channel, flexe_core_port_array[0],
                                       port_mapping[unit][flexe_core_port_array[0]].phy_mode, 1));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add FlexE BusA client only in calendar, meaning
 *    only update Mux and demux
 */
shr_error_e
flexe_dbg_calendar_add(
    int unit,
    int client_channel,
    uint32 flags,
    int cal_id,
    int *flexe_core_port_array,
    bcm_port_flexe_time_slot_t * ts_mask,
    int nof_flexe_core_ports)
{
    int nof_phys, is_bypass;
    uint8 phy_array[DNX_FLEXE_MAX_NOF_PHYS];
    uint32 phy_ts_mask[DNX_FLEXE_MAX_NOF_PHYS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Assign the time slots to phy instance
     */
    is_bypass = ((port_mapping[unit][flexe_core_port_array[0]].phy_mode == PCS_PHY_50G) ||
                 (port_mapping[unit][flexe_core_port_array[0]].phy_mode == PCS_PHY_100G) ||
                 (port_mapping[unit][flexe_core_port_array[0]].phy_mode == PCS_PHY_200G));
    if (!is_bypass)
    {
        SHR_IF_ERR_EXIT(flexe_dbg_phy_instance_ts_get(unit, flexe_core_port_array, *ts_mask,
                                                      nof_flexe_core_ports, phy_array, phy_ts_mask, &nof_phys));
        if (flags & BCM_PORT_FLEXE_RX)
        {
            DNX_FLEXE_IF_ERR_EXIT(demux_traffic_flexe_cfg
                                  (unit, client_channel, phy_array, nof_phys, (UINT_32 *) phy_ts_mask, 1));
        }
        if (flags & BCM_PORT_FLEXE_TX)
        {
            DNX_FLEXE_IF_ERR_EXIT(mux_traffic_flexe_cfg
                                  (unit, client_channel, phy_array, nof_phys, (UINT_32 *) phy_ts_mask, 1));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete FlexE busA client
 */
shr_error_e
flexe_dbg_client_delete(
    int unit,
    int client_channel,
    uint32 flags,
    int cal_id,
    const int *flexe_core_port_array,
    bcm_port_flexe_time_slot_t ts_mask,
    int nof_flexe_core_ports)
{
    int nof_phys, is_bypass;
    uint8 phy_array[DNX_FLEXE_MAX_NOF_PHYS];
    uint32 phy_ts_mask[DNX_FLEXE_MAX_NOF_PHYS] = { 0 };
    flexe_core_port_speed_mode_t speed_mode;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Assign the time slots to phy instance
     */
    is_bypass = ((port_mapping[unit][flexe_core_port_array[0]].phy_mode == PCS_PHY_50G) ||
                 (port_mapping[unit][flexe_core_port_array[0]].phy_mode == PCS_PHY_100G) ||
                 (port_mapping[unit][flexe_core_port_array[0]].phy_mode == PCS_PHY_200G) ||
                 (port_mapping[unit][flexe_core_port_array[0]].phy_mode == PCS_PHY_400G));
    if (!is_bypass)
    {
        SHR_IF_ERR_EXIT(flexe_dbg_phy_instance_ts_get(unit, flexe_core_port_array, ts_mask,
                                                      nof_flexe_core_ports, phy_array, phy_ts_mask, &nof_phys));

        if (flags & BCM_PORT_FLEXE_RX)
        {
            if (cal_id == 0)
            {
                DNX_FLEXE_IF_ERR_EXIT(demux_traffic_flexe_cfg
                                      (unit, client_channel, phy_array, nof_phys, (UINT_32 *) phy_ts_mask, 0));
            }
            else
            {
                DNX_FLEXE_IF_ERR_EXIT(demux_traffic_flexe_b_cfg
                                      (unit, client_channel, phy_array, nof_phys, (UINT_32 *) phy_ts_mask, 0));
            }
        }
        if (flags & BCM_PORT_FLEXE_TX)
        {
            if (cal_id == 0)
            {
                DNX_FLEXE_IF_ERR_EXIT(mux_traffic_flexe_cfg
                                      (unit, client_channel, phy_array, nof_phys, (UINT_32 *) phy_ts_mask, 0));
            }
            else
            {
                DNX_FLEXE_IF_ERR_EXIT(mux_traffic_flexe_b_cfg
                                      (unit, client_channel, phy_array, nof_phys, (UINT_32 *) phy_ts_mask, 0));
            }
            /*
             * Clear the overhead configuration
             */
            SHR_IF_ERR_EXIT(flexe_dbg_oh_client_id_clear
                            (unit, cal_id, flexe_core_port_array, ts_mask, nof_flexe_core_ports));
        }
    }
    else
    {
        if (flags & BCM_PORT_FLEXE_RX)
        {
            DNX_FLEXE_IF_ERR_EXIT(channelize_pcschid_cfg_set(unit, flexe_core_port_array[0], 0xff));
        }
        if (flags & BCM_PORT_FLEXE_TX)
        {
            SHR_IF_ERR_EXIT(flexe_dbg_speed_mode_get
                            (unit, port_mapping[unit][flexe_core_port_array[0]].speed, &speed_mode));
            DNX_FLEXE_IF_ERR_EXIT(inf_ch_adp_tx_cfg
                                  (unit, 0, flexe_core_port_array[0], flexe_core_port_array[0], 0, 0, speed_mode, 1,
                                   client_channel));
            if (port_mapping[unit][flexe_core_port_array[0]].phy_mode != PCS_PHY_400G)
            {
                DNX_FLEXE_IF_ERR_EXIT(mux_traffic_pcs_cfg
                                      (unit, client_channel, flexe_core_port_array[0],
                                       port_mapping[unit][flexe_core_port_array[0]].phy_mode, 0));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete FlexE busA client only in calendar
 */
shr_error_e
flexe_dbg_calendar_delete(
    int unit,
    int client_channel,
    uint32 flags,
    int cal_id,
    int *flexe_core_port_array,
    bcm_port_flexe_time_slot_t * ts_mask,
    int nof_flexe_core_ports)
{
    int nof_phys, is_bypass;
    uint8 phy_array[DNX_FLEXE_MAX_NOF_PHYS];
    uint32 phy_ts_mask[DNX_FLEXE_MAX_NOF_PHYS] = { 0 };

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Assign the time slots to phy instance
     */
    is_bypass = ((port_mapping[unit][flexe_core_port_array[0]].phy_mode == PCS_PHY_50G) ||
                 (port_mapping[unit][flexe_core_port_array[0]].phy_mode == PCS_PHY_100G) ||
                 (port_mapping[unit][flexe_core_port_array[0]].phy_mode == PCS_PHY_200G));

    if (!is_bypass)
    {
        SHR_IF_ERR_EXIT(flexe_dbg_phy_instance_ts_get(unit, flexe_core_port_array, *ts_mask,
                                                      nof_flexe_core_ports, phy_array, phy_ts_mask, &nof_phys));
        if (flags & BCM_PORT_FLEXE_RX)
        {
            DNX_FLEXE_IF_ERR_EXIT(demux_traffic_flexe_cfg
                                  (unit, client_channel, phy_array, nof_phys, (UINT_32 *) phy_ts_mask, 0));
        }
        if (flags & BCM_PORT_FLEXE_TX)
        {
            DNX_FLEXE_IF_ERR_EXIT(mux_traffic_flexe_cfg
                                  (unit, client_channel, phy_array, nof_phys, (UINT_32 *) phy_ts_mask, 0));
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add FlexE BusB client
 */
shr_error_e
flexe_dbg_mac_client_add(
    int unit,
    int client_channel,
    int nof_slots)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(client_env_ts_cfg(unit, 1, client_channel, nof_slots));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete FlexE BusB client
 */
shr_error_e
flexe_dbg_mac_client_delete(
    int unit,
    int client_channel,
    int nof_slots)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(client_env_ts_cfg(unit, 0, client_channel, nof_slots));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Add FlexE BusC client
 */
shr_error_e
flexe_dbg_sar_client_add(
    int unit,
    int client_channel,
    uint32 flags,
    int ts_num)
{
    int cell_mode;

    SHR_FUNC_INIT_VARS(unit);

    /** Get the original SAR cell mode */
    SHR_IF_ERR_EXIT(flexe_dbg_sar_cell_mode_get(unit, &cell_mode));

    if (flags & BCM_PORT_FLEXE_RX)
    {
        DNX_FLEXE_IF_ERR_EXIT(sar_rx_ts_cfg_asic(unit, 1, client_channel, cell_mode, ts_num));
    }

    if (flags & BCM_PORT_FLEXE_TX)
    {
        DNX_FLEXE_IF_ERR_EXIT(sar_tx_ts_cfg_asic(unit, 1, client_channel, cell_mode, ts_num));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Delete FlexE BusC client
 */
shr_error_e
flexe_dbg_sar_client_delete(
    int unit,
    int client_channel,
    uint32 flags,
    int ts_num)
{
    int cell_mode;

    SHR_FUNC_INIT_VARS(unit);

    /** Get the original SAR cell mode */
    SHR_IF_ERR_EXIT(flexe_dbg_sar_cell_mode_get(unit, &cell_mode));

    if (flags & BCM_PORT_FLEXE_RX)
    {
        DNX_FLEXE_IF_ERR_EXIT(sar_rx_ts_cfg_asic(unit, 0, client_channel, cell_mode, ts_num));
    }

    if (flags & BCM_PORT_FLEXE_TX)
    {
        DNX_FLEXE_IF_ERR_EXIT(sar_tx_ts_cfg_asic(unit, 0, client_channel, cell_mode, ts_num));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure FlexE sar channel map
 */
shr_error_e
flexe_dbg_sar_channel_map_set(
    int unit,
    uint32 flags,
    int sar_channel,
    int ilkn_channel,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_PORT_FLEXE_RX)
    {
        DNX_FLEXE_IF_ERR_EXIT(sar_rx_chan_map_cfg(unit, enable, sar_channel, ilkn_channel));
    }
    if (flags & BCM_PORT_FLEXE_TX)
    {
        DNX_FLEXE_IF_ERR_EXIT(sar_tx_chan_map_cfg(unit, enable, sar_channel, ilkn_channel));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure 66B switch
 */
shr_error_e
flexe_dbg_66b_switch_set(
    int unit,
    int src_channel,
    int dest_channel,
    int index,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(b66switch_cfg(unit, enable, index, src_channel, dest_channel));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure PHY instance number
 */
shr_error_e
flexe_dbg_oh_logical_phy_id_set(
    int unit,
    int flexe_core_port,
    int logical_phy_id)
{
    int phy_index, instance_idx = 0;
    uint8 instance_id;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port].flexe_phys, phy_index)
    {
        SHR_IF_ERR_EXIT(flexe_dbg_logical_phy_id_to_instance_id_get(unit,
                                                                    port_mapping[unit][flexe_core_port].speed,
                                                                    logical_phy_id, instance_idx, &instance_id));

        DNX_FLEXE_IF_ERR_EXIT(flexe_oh_tx_pid_set(unit, phy_index, instance_id));
        DNX_FLEXE_IF_ERR_EXIT(flexe_oh_exp_pid_set(unit, phy_index, instance_id));
        instance_idx++;
    }
    /*
     * Clear the original PHY ID in the phymap
     */
    SHR_IF_ERR_EXIT(flexe_dbg_oh_phymap_set
                    (unit, flexe_core_port, port_mapping[unit][flexe_core_port].logical_phy_id, 0));
    /*
     * Update the new PHY ID in the phymap
     */
    SHR_IF_ERR_EXIT(flexe_dbg_oh_phymap_set(unit, flexe_core_port, logical_phy_id, 1));
    /*
     * Update DB
     */
    port_mapping[unit][flexe_core_port].logical_phy_id = logical_phy_id;
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure PHY instance number
 */
shr_error_e
flexe_dbg_oh_logical_phy_id_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *logical_phy_id)
{
    int phy_index;
    uint32 instance_id;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port].flexe_phys, phy_index)
    {
        if (flags == BCM_PORT_FLEXE_TX)
        {
            DNX_FLEXE_IF_ERR_EXIT(oh_tx_oh_ins_cfg_get(unit, phy_index, PID_INS, (UINT_32 *) & instance_id));
        }
        else if (flags == BCM_PORT_FLEXE_RX)
        {
            DNX_FLEXE_IF_ERR_EXIT(oh_rx_id_rx_value_get(unit, phy_index, ACPID, (UINT_32 *) & instance_id));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags.\r\n");
        }
        SHR_IF_ERR_EXIT(flexe_dbg_instance_id_to_logical_phy_id_get(unit,
                                                                    port_mapping[unit][flexe_core_port].speed,
                                                                    instance_id, logical_phy_id));
        break;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure group ID
 */
shr_error_e
flexe_dbg_oh_group_id_set(
    int unit,
    int flexe_core_port,
    int group_id)
{
    int phy_index;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port].flexe_phys, phy_index)
    {
        DNX_FLEXE_IF_ERR_EXIT(flexe_oh_tx_gid_set(unit, phy_index, group_id));
        DNX_FLEXE_IF_ERR_EXIT(flexe_oh_exp_gid_set(unit, phy_index, group_id));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get group ID
 */
shr_error_e
flexe_dbg_oh_group_id_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *group_id)
{
    int phy_index;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port].flexe_phys, phy_index)
    {
        if (flags == BCM_PORT_FLEXE_TX)
        {
            DNX_FLEXE_IF_ERR_EXIT(oh_tx_oh_ins_cfg_get(unit, phy_index, GID_INS, (UINT_32 *) group_id));
        }
        else if (flags == BCM_PORT_FLEXE_RX)
        {
            DNX_FLEXE_IF_ERR_EXIT(flexe_oh_rx_gid_get(unit, phy_index, (UINT_32 *) group_id));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags, Rx and Tx flags cannot be used together.\r\n");
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure CLIENT id
 */
shr_error_e
flexe_dbg_oh_client_id_set(
    int unit,
    int cal_id,
    int nof_ports,
    const int *flexe_core_port_array,
    int nof_slots,
    const int *calendar_slots)
{
    int nof_phys, max_slots_per_instance, i, phy_index;
    int start_ts_index;

    SHR_FUNC_INIT_VARS(unit);

    nof_phys = 0;
    max_slots_per_instance = (port_mapping[unit][flexe_core_port_array[0]].phy_mode == FLEXE_PHY_100G) ? 20 : 10;

    for (i = 0; i < nof_ports; ++i)
    {
        _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port_array[i]].flexe_phys, phy_index)
        {
            start_ts_index = nof_phys * max_slots_per_instance;
            DNX_FLEXE_IF_ERR_EXIT(flexe_oh_tx_ccab_set
                                  (unit, cal_id, phy_index, (UINT_32 *) & calendar_slots[start_ts_index]));
            DNX_FLEXE_IF_ERR_EXIT(flexe_oh_exp_ccab_set
                                  (unit, cal_id, phy_index, (UINT_32 *) & calendar_slots[start_ts_index]));
            ++nof_phys;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get CLIENT id
 */
shr_error_e
flexe_dbg_oh_client_id_get(
    int unit,
    int cal_id,
    uint32 flags,
    int nof_ports,
    const int *flexe_core_port_array,
    int nof_slots,
    int *calendar_slots)
{
    int max_slots_per_instance, i, phy_index, field_id;
    int ts_index, inner_ts_index;
    uint32 flag_mask = BCM_PORT_FLEXE_TX | BCM_PORT_FLEXE_RX;

    SHR_FUNC_INIT_VARS(unit);

    max_slots_per_instance = (port_mapping[unit][flexe_core_port_array[0]].phy_mode == FLEXE_PHY_100G) ? 20 : 10;
    /*
     * Mask flags
     */
    flags = flags & flag_mask;

    ts_index = 0;
    for (i = 0; i < nof_ports; ++i)
    {
        _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port_array[i]].flexe_phys, phy_index)
        {
            for (inner_ts_index = 0; inner_ts_index < max_slots_per_instance; ++inner_ts_index)
            {
                if (flags == BCM_PORT_FLEXE_TX)
                {
                    field_id = (cal_id == 0) ? 1 : 2;
                    DNX_FLEXE_IF_ERR_EXIT(oh_tx_phymap_cca_ccb_ins_cfg_get
                                          (unit, phy_index, field_id, inner_ts_index,
                                           (UINT_32 *) & calendar_slots[ts_index]));
                }
                else if (flags == BCM_PORT_FLEXE_RX)
                {
                    field_id = (cal_id == 0) ? 0 : 1;
                    DNX_FLEXE_IF_ERR_EXIT(oh_rx_calendar_rx_get
                                          (unit, phy_index, field_id, inner_ts_index,
                                           (UINT_32 *) & calendar_slots[ts_index]));
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags, Rx and Tx flags cannot be used together.\r\n");
                }
                ++ts_index;
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - clear the client id in overhead
 */
static shr_error_e
flexe_dbg_oh_client_id_clear(
    int unit,
    int cal_id,
    const int *flexe_core_port_array,
    bcm_port_flexe_time_slot_t ts_mask,
    int nof_flexe_core_ports)
{
    int nof_phys, max_slots_per_instance, i, phy_index;
    int inner_ts_index, field_id;
    uint32 zero = 0;

    SHR_FUNC_INIT_VARS(unit);

    nof_phys = 0;
    max_slots_per_instance = (port_mapping[unit][flexe_core_port_array[0]].phy_mode == FLEXE_PHY_100G) ? 20 : 10;

    for (i = 0; i < nof_flexe_core_ports; ++i)
    {
        _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port_array[i]].flexe_phys, phy_index)
        {
            for (inner_ts_index = 0; inner_ts_index < max_slots_per_instance; ++inner_ts_index)
            {
                if (SHR_BITGET(ts_mask, (inner_ts_index + nof_phys * max_slots_per_instance)))
                {
                    field_id = (cal_id == 0) ? 1 : 2;
                    DNX_FLEXE_IF_ERR_EXIT(oh_tx_phymap_cca_ccb_ins_cfg_set
                                          (unit, phy_index, field_id, inner_ts_index, (UINT_32 *) & zero));
                    field_id = (cal_id == 0) ? 0 : 1;
                    DNX_FLEXE_IF_ERR_EXIT(oh_rx_ccab_rx_exp_set
                                          (unit, phy_index, field_id, inner_ts_index, (UINT_32 *) & zero));
                }
            }
            ++nof_phys;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure phymap
 */
shr_error_e
flexe_dbg_oh_phymap_set(
    int unit,
    int flexe_core_port,
    int logical_phy_id,
    int enable)
{
    int phy_index, instance_idx = 0, phy_map_offset, i, first_phy;
    int flexe_core_port_tmp, port_i, nof_ports, clear_phymap;
    uint8 instance_id_array[DNX_FLEXE_MAX_NOF_PHYS];
    uint32 zero = 0, phy_map[DNX_FLEXE_MAP_PHY_MAP_OCTETS] = { 0 };
    soc_pbmp_t flexe_core_ports;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port].flexe_phys, phy_index)
    {
        SHR_IF_ERR_EXIT(flexe_dbg_logical_phy_id_to_instance_id_get(unit,
                                                                    port_mapping[unit][flexe_core_port].speed,
                                                                    logical_phy_id,
                                                                    instance_idx, &instance_id_array[instance_idx]));
        instance_idx++;
    }
    /*
     * Get all the flexe core port in the group
     */
    _SHR_PBMP_CLEAR(flexe_core_ports);
    for (i = 0; i < DNX_FLEXE_MAX_NOF_PHYS; ++i)
    {
        if (port_mapping[unit][flexe_core_port].group_index == port_mapping[unit][i].group_index)
        {
            _SHR_PBMP_PORT_ADD(flexe_core_ports, i);
        }
    }
    _SHR_PBMP_COUNT(flexe_core_ports, nof_ports);
    if (nof_ports > 1)
    {
        /*
         * If there is more than 1 FlexE PHYs in the group,
         * need to get the existing FlexE core port. We need to update
         * the phymap based on the exsiting phymap
         */
        _SHR_PBMP_ITER(flexe_core_ports, flexe_core_port_tmp)
        {
            if (flexe_core_port_tmp != flexe_core_port)
            {
                break;
            }
        }
    }
    else
    {
        flexe_core_port_tmp = flexe_core_port;
    }
    /*
     * Get original PHY MAP
     */
    _SHR_PBMP_FIRST(port_mapping[unit][flexe_core_port_tmp].flexe_phys, first_phy);
    for (phy_map_offset = 0; phy_map_offset < DNX_FLEXE_MAP_PHY_MAP_OCTETS; ++phy_map_offset)
    {
        DNX_FLEXE_IF_ERR_EXIT(oh_tx_phymap_cca_ccb_ins_cfg_get
                              (unit, first_phy, 0, phy_map_offset, (UINT_32 *) & phy_map[phy_map_offset]));
    }
    /*
     * Enable or disable phy map
     */
    for (i = 0; i < instance_idx; ++i)
    {
        phy_map_offset = instance_id_array[i] / 8;
        SHR_BITWRITE(&phy_map[phy_map_offset], (instance_id_array[i] % 8), enable);
    }
    /*
     * Update HW
     */
    for (phy_map_offset = 0; phy_map_offset < DNX_FLEXE_MAP_PHY_MAP_OCTETS; ++phy_map_offset)
    {
        _SHR_PBMP_ITER(flexe_core_ports, port_i)
        {
            /*
             * If the FlexE core port is removed, need to clear the phymap
             */
            clear_phymap = ((enable == 0) && (port_i == flexe_core_port)) ? 1 : 0;
            _SHR_PBMP_ITER(port_mapping[unit][port_i].flexe_phys, phy_index)
            {
                DNX_FLEXE_IF_ERR_EXIT(oh_tx_phymap_cca_ccb_ins_cfg_set
                                      (unit, phy_index, 0, phy_map_offset,
                                       clear_phymap ? (UINT_32 *) & zero : (UINT_32 *) & phy_map[phy_map_offset]));
                DNX_FLEXE_IF_ERR_EXIT(oh_rx_phymap_rx_exp_set
                                      (unit, phy_index, phy_map_offset,
                                       clear_phymap ? (UINT_32 *) & zero : (UINT_32 *) & phy_map[phy_map_offset]));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure CR bit
 */
shr_error_e
flexe_dbg_oh_cr_bit_set(
    int unit,
    int flexe_core_port,
    int cal_id)
{
    int phy_index;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port].flexe_phys, phy_index)
    {
        DNX_FLEXE_IF_ERR_EXIT(oh_tx_oh_ins_cfg_set(unit, phy_index, CR_INS, cal_id));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get CR bit
 */
shr_error_e
flexe_dbg_oh_cr_bit_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *cal_id)
{
    int phy_index;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port].flexe_phys, phy_index)
    {
        if (flags == BCM_PORT_FLEXE_RX)
        {
            DNX_FLEXE_IF_ERR_EXIT(oh_rx_cc_rx_value_get(unit, phy_index, ACCR, (UINT_32 *) cal_id));
        }
        else if (flags == BCM_PORT_FLEXE_TX)
        {
            DNX_FLEXE_IF_ERR_EXIT(oh_tx_oh_ins_cfg_get(unit, phy_index, CR_INS, (UINT_32 *) cal_id));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags, Rx and Tx flags cannot be used together.\r\n");
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure CA bit
 */
shr_error_e
flexe_dbg_oh_ca_bit_set(
    int unit,
    int flexe_core_port,
    int cal_id)
{
    int phy_index;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port].flexe_phys, phy_index)
    {
        DNX_FLEXE_IF_ERR_EXIT(oh_tx_oh_ins_cfg_set(unit, phy_index, CA_INS, cal_id));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get CA bit
 */
shr_error_e
flexe_dbg_oh_ca_bit_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *cal_id)
{
    int phy_index;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port].flexe_phys, phy_index)
    {
        if (flags == BCM_PORT_FLEXE_RX)
        {
            DNX_FLEXE_IF_ERR_EXIT(oh_rx_cc_rx_value_get(unit, phy_index, ACCA, (UINT_32 *) cal_id));
        }
        else if (flags == BCM_PORT_FLEXE_TX)
        {
            DNX_FLEXE_IF_ERR_EXIT(oh_tx_oh_ins_cfg_get(unit, phy_index, CA_INS, (UINT_32 *) cal_id));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags, Rx and Tx flags cannot be used together.\r\n");
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure C bit
 */
shr_error_e
flexe_dbg_oh_c_bit_set(
    int unit,
    int flexe_core_port,
    int cal_id)
{
    int phy_index;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port].flexe_phys, phy_index)
    {
        DNX_FLEXE_IF_ERR_EXIT(oh_tx_oh_ins_cfg_set(unit, phy_index, CCC_INS, cal_id));
        DNX_FLEXE_IF_ERR_EXIT(oh_rx_exccc_exsc_set(unit, phy_index, EXCCC, cal_id));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - get C bit
 */
shr_error_e
flexe_dbg_oh_c_bit_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *cal_id)
{
    int phy_index;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port].flexe_phys, phy_index)
    {
        if (flags == BCM_PORT_FLEXE_RX)
        {
            DNX_FLEXE_IF_ERR_EXIT(oh_rx_cc_rx_value_get(unit, phy_index, ACCCC, (UINT_32 *) cal_id));
        }
        else if (flags == BCM_PORT_FLEXE_TX)
        {
            DNX_FLEXE_IF_ERR_EXIT(oh_tx_oh_ins_cfg_get(unit, phy_index, CCC_INS, (UINT_32 *) cal_id));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags, Rx and Tx flags cannot be used together.\r\n");
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Configure SC bit
 */
shr_error_e
flexe_dbg_oh_sc_bit_set(
    int unit,
    int flexe_core_port,
    int sync_config)
{
    int phy_index;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port].flexe_phys, phy_index)
    {
        DNX_FLEXE_IF_ERR_EXIT(oh_tx_res_ins_cfg_set(unit, phy_index, SC_INS, sync_config));
        DNX_FLEXE_IF_ERR_EXIT(oh_rx_exccc_exsc_set(unit, phy_index, EXSC, sync_config));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Get SC bit
 */
shr_error_e
flexe_dbg_oh_sc_bit_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *sync_config)
{
    int phy_index;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port].flexe_phys, phy_index)
    {
        if (flags == BCM_PORT_FLEXE_RX)
        {
            DNX_FLEXE_IF_ERR_EXIT(oh_rx_cc_rx_value_get(unit, phy_index, ACSC, (UINT_32 *) sync_config));
        }
        else if (flags == BCM_PORT_FLEXE_TX)
        {
            DNX_FLEXE_IF_ERR_EXIT(oh_tx_res_ins_cfg_get(unit, phy_index, SC_INS, (UINT_32 *) sync_config));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags, Rx and Tx flags cannot be used together.\r\n");
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure active cal in MUX
 */
shr_error_e
flexe_dbg_active_calendar_set(
    int unit,
    int flexe_core_port,
    int cal_id)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(mux_phy_group_c_cfg(unit, port_mapping[unit][flexe_core_port].group_index, cal_id));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get active cal
 */
shr_error_e
flexe_dbg_active_calendar_get(
    int unit,
    int flexe_core_port,
    uint32 flags,
    int *cal_id)
{
    int phy_index;

    SHR_FUNC_INIT_VARS(unit);

    _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port].flexe_phys, phy_index)
    {
        if (flags == BCM_PORT_FLEXE_TX)
        {
            DNX_FLEXE_IF_ERR_EXIT(flexe_mux_c_cfg_get(unit, phy_index, (UINT_32 *) cal_id));
        }
        else if (flags == BCM_PORT_FLEXE_RX)
        {
            DNX_FLEXE_IF_ERR_EXIT(oh_rx_cc_rx_value_get(unit, phy_index, ACCCC, (UINT_32 *) cal_id));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported flags, Rx and Tx flags cannot be used together.\r\n");
        }
        break;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Group Alarms
 *    Bit 0:  GIDM_ALM
 *    Bit 1:  phy_num mismatch
 *    Bit 2:  LOF
 *    Bit 3:  LOM
 *    Bit 4:  RPF
 *    Bit 5:  OH1_BLOCK_ALM
 *    Bit 6:  C_BIT_ALM
 *    Bit 7:  PMM
 *    Bit 8:  CCAM
 *    Bit 9:  CCBM
 *    Bit 10: FlexE_LOGA (deskew alram)
 *    Bit 11: CRC
 */
shr_error_e
flexe_dbg_oh_alarm_status_get(
    int unit,
    int flexe_core_port,
    uint16 *alarm_status)
{
    int phy_index;
    uint16 tmp_status;

    SHR_FUNC_INIT_VARS(unit);

    *alarm_status = 0;

    if (port_mapping[unit][flexe_core_port].group_index == -1)
    {
        /*
         * If the FlexE core port is not belong to any group, get OH alarm from the instance directly
         */
        DNX_FLEXE_IF_ERR_EXIT(flexe_oh_instance_alm_get(unit, flexe_core_port, (UINT_16 *) & tmp_status));
        *alarm_status |= tmp_status;
    }
    else
    {
        _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port].flexe_phys, phy_index)
        {
            DNX_FLEXE_IF_ERR_EXIT(flexe_oh_instance_alm_get(unit, phy_index, (UINT_16 *) & tmp_status));
            *alarm_status |= tmp_status;
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the time slots bitmap if there is
 *     mismatch happens.
 */
shr_error_e
flexe_dbg_oh_cal_mismatch_ts_get(
    int unit,
    int flexe_core_port,
    bcm_port_flexe_time_slot_t * time_slots)
{
    int phy_index, max_slots_per_instance, nof_phys;
    uint32 tmp_slots[1];
    uint16 client_id[DNX_FLEXE_MAX_SLOTS_PER_INS];

    SHR_FUNC_INIT_VARS(unit);

    SHR_BITCLR_RANGE(*time_slots, 0, BCM_PORT_FLEXE_MAX_NOF_SLOTS);
    max_slots_per_instance = (port_mapping[unit][flexe_core_port].phy_mode == FLEXE_PHY_100G) ? 20 : 10;

    nof_phys = 0;
    _SHR_PBMP_ITER(port_mapping[unit][flexe_core_port].flexe_phys, phy_index)
    {
        DNX_FLEXE_IF_ERR_EXIT(flexe_oh_ccm_alm_get(unit, phy_index, (UINT_32 *) & tmp_slots[0], (UINT_16 *) client_id));
        SHR_BITCOPY_RANGE(*time_slots, nof_phys * max_slots_per_instance, tmp_slots, 0, max_slots_per_instance);
        nof_phys++;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get OAM Alarms
 *    Bit 0: RX_LPI
 *    Bit 1: RX_CS_LF
 *    Bit 2: RX_CS_RF
 *    Bit 3: BASE_OAM_LOS
 *    Bit 4: RX_SDBIP
 *    Bit 5: RX_CRC
 *    Bit 6: RX_RDI
 *    Bit 7: RX_PERIOD_MISMATCH
 *    Bit 8: SDREI
 *    Bit 9: SFBIP
 *    Bit 10: SFREI
 *    Bit 11: LOCAL FAULT
 *    Bit 12: REMOTE FAULT
 */
shr_error_e
flexe_dbg_oam_alarm_status_get(
    int unit,
    int client_channel,
    uint16 *alarms_status)
{
    uint16 tmp_alarm_status = 0;

    SHR_FUNC_INIT_VARS(unit);

    *alarms_status = 0;

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_channel_alm_get(unit, client_channel, (UINT_16 *) & tmp_alarm_status));
    *alarms_status |= tmp_alarm_status;

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_lf_rf_alm_get(unit, client_channel, (UINT_8 *) & tmp_alarm_status));
    if (tmp_alarm_status & 0x1)
    {
        *alarms_status |= FLEXE_CORE_OAM_ALARM_RX_LF;
    }
    if (tmp_alarm_status & (0x1 << 1))
    {
        *alarms_status |= FLEXE_CORE_OAM_ALARM_RX_RF;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable FlexE OAM alarms
 */
shr_error_e
flexe_dbg_oam_alarm_enable_set(
    int unit,
    int client_channel,
    int alarm_type,
    int enable)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_alm_en_set(unit, alarm_type, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure OAM base period for Rx and Tx
 */
shr_error_e
flexe_dbg_oam_base_period_set(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 base_period)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_PORT_FLEXE_TX)
    {
        DNX_FLEXE_IF_ERR_EXIT(flexe_oam_tx_bas_period_cfg_set(unit, client_channel, base_period));
    }
    else
    {
        DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_period_cfg_set(unit, client_channel, base_period));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get OAM base period for Rx and Tx
 */
shr_error_e
flexe_dbg_oam_base_period_get(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 *base_period)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_PORT_FLEXE_TX)
    {
        DNX_FLEXE_IF_ERR_EXIT(flexe_oam_tx_bas_period_cfg_get(unit, client_channel, (UINT_32 *) base_period));
    }
    else
    {
        DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_period_cfg_get(unit, client_channel, (UINT_32 *) base_period));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable/Disable Base OAM insert
 */
shr_error_e
flexe_dbg_oam_base_insert_enable_set(
    int unit,
    int client_channel,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_tx_bas_en_cfg_asic_set(unit, client_channel, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get Base OAM insert Enable/Disable status
 */
shr_error_e
flexe_dbg_oam_base_insert_enable_get(
    int unit,
    int client_channel,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_tx_bas_en_cfg_asic_get(unit, client_channel, (UINT_32 *) enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable/Disable OAM bypass functionality
 */
shr_error_e
flexe_dbg_oam_bypass_enable_set(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 enable)
{
    int direction;

    SHR_FUNC_INIT_VARS(unit);

    direction = (flags & BCM_PORT_FLEXE_TX) ? 1 : 0;
    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_bypass_cfg(unit, client_channel, direction, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get OAM bypass Enable/Disable status
 */
shr_error_e
flexe_dbg_oam_bypass_enable_get(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_PORT_FLEXE_TX)
    {
        DNX_FLEXE_IF_ERR_EXIT(flexe_oam_tx_bypass_client_get(unit, client_channel, (UINT_32 *) enable));
    }
    else
    {
        DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_bypass_chnl_get(unit, client_channel, (UINT_32 *) enable));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable/Disable OAM bypass functionality for SAR client
 */
shr_error_e
flexe_dbg_sar_oam_bypass_enable_set(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 enable)
{
    int direction;

    SHR_FUNC_INIT_VARS(unit);

    direction = (flags & BCM_PORT_FLEXE_TX) ? 1 : 0;
    DNX_FLEXE_IF_ERR_EXIT(sar_oam_bypass_cfg(unit, client_channel, direction, enable));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - GET OAM bypass for SAR client enable/disable status
 */
shr_error_e
flexe_dbg_sar_oam_bypass_enable_get(
    int unit,
    int client_channel,
    uint32 flags,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_PORT_FLEXE_TX)
    {
        DNX_FLEXE_IF_ERR_EXIT(oam_tx_bypass_client_get(unit, client_channel, (UINT_32 *) enable));
    }
    else
    {
        DNX_FLEXE_IF_ERR_EXIT(oam_rx_bypass_chnl_get(unit, client_channel, (UINT_32 *) enable));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable/Disable OAM local fault insert
 */
shr_error_e
flexe_dbg_oam_local_fault_insert_set(
    int unit,
    int client_channel,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_tx_manual_insert_csf_lf_set(unit, client_channel, enable));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get OAM local fault insert Enable/disable status
 */
shr_error_e
flexe_dbg_oam_local_fault_insert_get(
    int unit,
    int client_channel,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_tx_manual_insert_csf_lf_get(unit, client_channel, (UINT_32 *) enable));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Enable/Disable OAM local fault insert
 */
shr_error_e
flexe_dbg_oam_remote_fault_insert_set(
    int unit,
    int client_channel,
    uint32 enable)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_tx_manual_insert_csf_rf_set(unit, client_channel, enable));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get OAM local fault insert Enable/Disable status
 */
shr_error_e
flexe_dbg_oam_remote_fault_insert_get(
    int unit,
    int client_channel,
    uint32 *enable)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_tx_manual_insert_csf_rf_get(unit, client_channel, (UINT_32 *) enable));
exit:
    SHR_FUNC_EXIT;
}

/*
 * The SD configuration APIs in Core driver needs to
 * be split to three.
 */
/**
 * \brief - Configure SD BIP8 alarm block number
 */
shr_error_e
flexe_dbg_oam_sd_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_sd_block_num_set(unit, client_channel, block_num));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get SD BIP8 alarm block number
 */
shr_error_e
flexe_dbg_oam_sd_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_sd_block_num_get(unit, client_channel, (UINT_32 *) block_num));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SD BIP8 error set threshold
 */
shr_error_e
flexe_dbg_oam_sd_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_sd_bip8_set_thresh_set(unit, client_channel, threshold));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get SD BIP8 error set threshold
 */
shr_error_e
flexe_dbg_oam_sd_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_sd_bip8_set_thresh_get(unit, client_channel, (UINT_32 *) threshold));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SD BIP8 error clear threshold
 */
shr_error_e
flexe_dbg_oam_sd_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_sd_bip8_clr_thresh_set(unit, client_channel, threshold));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get SD BIP8 error clear threshold
 */
shr_error_e
flexe_dbg_oam_sd_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_sd_bip8_clr_thresh_get(unit, client_channel, (UINT_32 *) threshold));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SD BEI alarm block number
 */
shr_error_e
flexe_dbg_oam_sd_bei_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_sd_bei_block_num_set(unit, client_channel, block_num));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get SD BEI alarm block number
 */
shr_error_e
flexe_dbg_oam_sd_bei_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_sd_bei_block_num_get(unit, client_channel, (UINT_32 *) block_num));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SD BEI error set threshold
 */
shr_error_e
flexe_dbg_oam_sd_bei_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_sd_bei_set_thresh_set(unit, client_channel, threshold));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get SD BEI error set threshold
 */
shr_error_e
flexe_dbg_oam_sd_bei_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_sd_bei_set_thresh_get(unit, client_channel, (UINT_32 *) threshold));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SD BEI error clear threshold
 */
shr_error_e
flexe_dbg_oam_sd_bei_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_sd_bei_clr_thresh_set(unit, client_channel, threshold));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get SD BEI error clear threshold
 */
shr_error_e
flexe_dbg_oam_sd_bei_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_sd_bei_clr_thresh_get(unit, client_channel, (UINT_32 *) threshold));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SF BIP8 alarm block number
 */
shr_error_e
flexe_dbg_oam_sf_bip8_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_sf_bip8_block_num_set(unit, client_channel, block_num));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get SF BIP8 alarm block number
 */
shr_error_e
flexe_dbg_oam_sf_bip8_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_sf_bip8_block_num_get(unit, client_channel, (UINT_32 *) block_num));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SF BIP8 error set threshold
 */
shr_error_e
flexe_dbg_oam_sf_bip8_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_sf_bip8_set_thresh_set(unit, client_channel, threshold));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get SF BIP8 error set threshold
 */
shr_error_e
flexe_dbg_oam_sf_bip8_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_sf_bip8_set_thresh_get(unit, client_channel, (UINT_32 *) threshold));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SF BIP8 error clear threshold
 */
shr_error_e
flexe_dbg_oam_sf_bip8_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_sf_bip8_clr_thresh_set(unit, client_channel, threshold));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get SF BIP8 error clear threshold
 */
shr_error_e
flexe_dbg_oam_sf_bip8_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_sf_bip8_clr_thresh_get(unit, client_channel, (UINT_32 *) threshold));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SF BEI alarm block number
 */
shr_error_e
flexe_dbg_oam_sf_bei_alm_block_num_set(
    int unit,
    int client_channel,
    uint32 block_num)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_sf_bei_block_num_set(unit, client_channel, block_num));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get SF BEI alarm block number
 */
shr_error_e
flexe_dbg_oam_sf_bei_alm_block_num_get(
    int unit,
    int client_channel,
    uint32 *block_num)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_sf_bei_block_num_get(unit, client_channel, (UINT_32 *) block_num));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SF BEI error set threshold
 */
shr_error_e
flexe_dbg_oam_sf_bei_alm_trigger_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_sf_bei_set_thresh_set(unit, client_channel, threshold));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get SF BEI error set threshold
 */
shr_error_e
flexe_dbg_oam_sf_bei_alm_trigger_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_sf_bei_set_thresh_get(unit, client_channel, (UINT_32 *) threshold));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Configure SF BEI error clear threshold
 */
shr_error_e
flexe_dbg_oam_sf_bei_alm_clear_thr_set(
    int unit,
    int client_channel,
    uint32 threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_sf_bei_clr_thresh_set(unit, client_channel, threshold));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get SF BEI error clear threshold
 */
shr_error_e
flexe_dbg_oam_sf_bei_alm_clear_thr_get(
    int unit,
    int client_channel,
    uint32 *threshold)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_rx_sf_bei_clr_thresh_get(unit, client_channel, (UINT_32 *) threshold));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get OAM BIP8 counter
 */
shr_error_e
flexe_dbg_oam_bip8_counter_get(
    int unit,
    int client_channel,
    uint64 *val)
{
    uint32 bip8_val;

    SHR_FUNC_INIT_VARS(unit);

     /* coverity[callee_ptr_arith : FALSE]  */
    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_sdbip_cnt_get(unit, client_channel, (UINT_32 *) & bip8_val));
    COMPILER_64_SET(*val, 0, bip8_val);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get OAM BEI counter
 */
shr_error_e
flexe_dbg_oam_bei_counter_get(
    int unit,
    int client_channel,
    uint64 *val)
{
    uint32 bei_val;

    SHR_FUNC_INIT_VARS(unit);

     /* coverity[callee_ptr_arith : FALSE]  */
    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_sdbei_cnt_get(unit, client_channel, (UINT_32 *) & bei_val));
    COMPILER_64_SET(*val, 0, bei_val);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get OAM packet
 */
shr_error_e
flexe_dbg_oam_pkt_count_get(
    int unit,
    int client_channel,
    uint64 *val)
{
    uint32 counter_val;

    SHR_FUNC_INIT_VARS(unit);

     /* coverity[callee_ptr_arith : FALSE]  */
    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_cnt_get(unit, client_channel, (UINT_32 *) & counter_val));
    COMPILER_64_SET(*val, 0, counter_val);
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get OAM base packet
 */
shr_error_e
flexe_dbg_oam_base_pkt_count_get(
    int unit,
    int client_channel,
    uint64 *val)
{
    uint32 counter_val;

    SHR_FUNC_INIT_VARS(unit);

     /* coverity[callee_ptr_arith : FALSE]  */
    DNX_FLEXE_IF_ERR_EXIT(flexe_oam_bas_cnt_get(unit, client_channel, (UINT_32 *) & counter_val));
    COMPILER_64_SET(*val, 0, counter_val);
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Configure default FlexE phy settings
 */
shr_error_e
flexe_dbg_phy_default_config_set(
    int unit,
    int flexe_core_port,
    int speed)
{
    flexe_core_port_speed_mode_t speed_mode;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(flexe_dbg_speed_mode_get(unit, speed, &speed_mode));
    DNX_FLEXE_IF_ERR_EXIT(flexe_default_config(unit, flexe_core_port, speed_mode));
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Allocated time slots in rate adapter
 */
shr_error_e
flexe_dbg_rateadpt_slots_add(
    int unit,
    int client_channel,
    int ts_num)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(rateadp_traffic_cfg(unit, 1, client_channel, ts_num));
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Delete time slots in rate adapter
 */
shr_error_e
flexe_dbg_rateadpt_slots_delete(
    int unit,
    int client_channel)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(rateadp_traffic_cfg(unit, 0, client_channel, 0));
exit:
    SHR_FUNC_EXIT;
}

/*
 * \brief - Configure the OAM alarm collection time step
 */
shr_error_e
flexe_dbg_oam_alarm_collection_timer_step_set(
    int unit,
    int step)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(ohoam_ef_soam_alm_colection_set(unit, 2, step));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get the OAM alarm collection time step
 */
shr_error_e
flexe_dbg_oam_alarm_collection_timer_step_get(
    int unit,
    int *step)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(ohoam_ef_soam_alm_colection_get(unit, 2, (UINT_32 *) step));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Configure number of steps for oam alarm collection period
 */
shr_error_e
flexe_dbg_oam_alarm_collection_step_count_set(
    int unit,
    int step_count)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(ohoam_ef_soam_alm_colection_set(unit, 0, step_count));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get number of steps for oam alarm collection period
 */
shr_error_e
flexe_dbg_oam_alarm_collection_step_count_get(
    int unit,
    int *step_count)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(ohoam_ef_soam_alm_colection_get(unit, 0, (UINT_32 *) step_count));
exit:
    SHR_FUNC_EXIT;

}
/*
 * \brief - Configure SAR cell mode in FlexE core
 */
shr_error_e
flexe_dbg_sar_cell_mode_set(
    int unit,
    int cell_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Configure SAR cell mode in Tx*/
    DNX_FLEXE_IF_ERR_EXIT(b66sar_rx_tx_b66size_29b_set(unit, 0, cell_mode));

    /** Configure SAR cell mode in Rx*/
    DNX_FLEXE_IF_ERR_EXIT(b66sar_rx_tx_b66size_29b_set(unit, 1, cell_mode));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Get SAR cell mode in FlexE core
 */
shr_error_e
flexe_dbg_sar_cell_mode_get(
    int unit,
    int *cell_mode)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Get SAR cell mode in Tx*/
    DNX_FLEXE_IF_ERR_EXIT(b66sar_rx_tx_b66size_29b_get(unit, 0, (UINT_32 *) cell_mode));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - Set channel map in FlexE core
 */
shr_error_e
flexe_dbg_channel_map_set(
    int unit,
    uint32 flags,
    int flexe_core_port,
    uint32 local_port)
{
    SHR_FUNC_INIT_VARS(unit);

    if (flags & BCM_PORT_FLEXE_TX)
    {
        DNX_FLEXE_IF_ERR_EXIT(inf_ch_adp_tx_map_table_set(unit, flexe_core_port, local_port));
    }
    if (flags & BCM_PORT_FLEXE_RX)
    {
        DNX_FLEXE_IF_ERR_EXIT(inf_ch_adp_rx_map_table_set(unit, flexe_core_port, local_port));
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - The local fault status for FlexE client
 */
shr_error_e
flexe_dbg_client_local_fault_get(
    int unit,
    int client_channel,
    int *local_fault_status)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_macrx_arm_lf_alm_get(unit, client_channel, (UINT_32 *) local_fault_status));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - The Remote fault status for FlexE client
 */
shr_error_e
flexe_dbg_client_remote_fault_get(
    int unit,
    int client_channel,
    int *remote_fault_status)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(flexe_macrx_arm_rf_alm_get(unit, client_channel, (UINT_32 *) remote_fault_status));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - The local fault status for FlexE SAR client
 */
shr_error_e
flexe_dbg_sar_client_local_fault_get(
    int unit,
    int client_channel,
    int *local_fault_status)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(mcmacrx_arm_lf_alm_get(unit, client_channel, (UINT_32 *) local_fault_status));
exit:
    SHR_FUNC_EXIT;
}
/*
 * \brief - The Remote fault status for FlexE SAR client
 */
shr_error_e
flexe_dbg_sar_client_remote_fault_get(
    int unit,
    int client_channel,
    int *remote_fault_status)
{
    SHR_FUNC_INIT_VARS(unit);

    DNX_FLEXE_IF_ERR_EXIT(mcmacrx_arm_rf_alm_get(unit, client_channel, (UINT_32 *) remote_fault_status));
exit:
    SHR_FUNC_EXIT;
}

/* } */
#endif /* INCLUDE_FLEXE */
