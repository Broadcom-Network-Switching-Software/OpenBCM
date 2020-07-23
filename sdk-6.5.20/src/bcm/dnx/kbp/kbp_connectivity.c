/** \file kbp_connectivity.c
 *
 * Functions for handling Jericho2 KBP connectivity.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_ELKDNX

/*
 * {
 */

/*************
 * INCLUDES  *
 */
/*
 * {
 */
#include <shared/bsl.h>
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx_dispatch.h>

#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <soc/cmicx_miim.h>
#include <soc/util.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_elk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dbal/dbal.h>
#include <sal/core/boot.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port_pp.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stif.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_port_imb_access.h>
#include <soc/dnxc/dnxc_ha.h>

/** allow drv.h include explicitly */
#ifdef _SOC_DRV_H
#undef _SOC_DRV_H
#endif
#include <soc/drv.h> /** needed for soc property functions */

#if defined(INCLUDE_KBP)

#include <bcm_int/dnx/kbp/kbp_connectivity.h>
#include <bcm_int/dnx/kbp/kbp_mngr.h>
#include <soc/dnx/kbp/kbp_common.h>
#include <bcm_int/dnx/kbp/kbp_recover.h>

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/*
 * EGW data width
 */
#define EGW_TO_ILKN_DATA_WIDTH  (64)

extern generic_kbp_app_data_t *Kbp_app_data[SOC_MAX_NUM_DEVICES];

#define DNX_KBP_STAT_PORTS_MAX (4)
/*
 * }
 */

/*************
 * FUNCTIONS *
 */
/*
 * {
 */

/**
 * \brief
 *   Get the ILKN port parameters on Jericho2 side.
 * \param [in] unit - The unit number.
 * \param [in] core - The core where the ILKN port reside
 * \param [out] ilkn_port - The ilkn logical port number.
 * \param [out] ilkn_num_lanes - Numbers of lanes of each ILKN port.
 * \param [out] ilkn_rate - ILKN per lane speed.
 * \param [out] ilkn_metaframe - ILKN metaframe length.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_kbp_ilkn_interface_param_get(
    int unit,
    int core,
    bcm_port_t * ilkn_port,
    int *ilkn_num_lanes,
    int *ilkn_rate,
    int *ilkn_metaframe)
{
    bcm_pbmp_t port_pbmp;
    int ilkn_id;

    SHR_FUNC_INIT_VARS(unit);
    /*
     * Get all the ELK ports for given core.
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN_ELK, 0, &port_pbmp));
    /*
     * Find the ILKN ports.
     */
    if (BCM_PBMP_IS_NULL(port_pbmp))
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "No ELK port is configured on core %d.\n", core);
    }
    _SHR_PBMP_ITER(port_pbmp, *ilkn_port)
    {
        break;
    }
    if (ilkn_num_lanes != NULL)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_nof_get(unit, *ilkn_port, ilkn_num_lanes));
    }
    if (ilkn_rate != NULL)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_speed_get(unit, *ilkn_port, 0, ilkn_rate));
    }
    if (ilkn_metaframe != NULL)
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, *ilkn_port, &ilkn_id));
        *ilkn_metaframe = dnx_data_nif.ilkn.properties_get(unit, ilkn_id)->metaframe_period;
    }
exit:
    SHR_FUNC_EXIT;
}

int32_t
dnx_kbp_callback_usleep(
    void *handle,
    uint32_t usec)
{
    sal_usleep(usec);
    return KBP_OK;
}
int32_t
dnx_kbp_callback_mdio_read(
    void *handle,
    int32_t chip_no,
    uint8_t dev,
    uint16_t reg,
    uint16_t * value)
{
    uint32 kbp_reg_addr;
    dnx_kbp_user_data_t *user_data = handle;
    int rv;

    if (handle == NULL)
    {
        return KBP_INVALID_ARGUMENT;
    }

    kbp_reg_addr = reg + (dev << 16);

    rv = soc_cmicx_miim_operation(user_data->device, 0 /* is_write */ , DNX_KBP_MDIO_CLAUSE, user_data->kbp_mdio_id,
                                  kbp_reg_addr, value);

    if (rv != _SHR_E_NONE)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("%s(): soc_cmicx_miim_operation() Failed\n"), FUNCTION_NAME()));
        return KBP_INTERNAL_ERROR;
    }
    return KBP_OK;
}

int32_t
dnx_kbp_callback_mdio_write(
    void *handle,
    int32_t chip_no,
    uint8_t dev,
    uint16_t reg,
    uint16_t value)
{
    uint32 kbp_reg_addr;
    dnx_kbp_user_data_t *user_data = handle;
    int rv;

    if (handle == NULL)
    {
        return KBP_INVALID_ARGUMENT;
    }
    kbp_reg_addr = reg + (dev << 16);

    rv = soc_cmicx_miim_operation(user_data->device, 1 /* is_write */ , DNX_KBP_MDIO_CLAUSE, user_data->kbp_mdio_id,
                                  kbp_reg_addr, &value);

    if (rv != _SHR_E_NONE)
    {
        LOG_ERROR(BSL_LOG_MODULE, (BSL_META("%s(): dnx_kbp_callback_mdio_write() Failed\n"), FUNCTION_NAME()));
        return KBP_INTERNAL_ERROR;
    }
    return KBP_OK;
}

int32_t
dnx_kbp_callback_reset_device(
    void *handle,
    int32_t s_reset_low,
    int32_t c_reset_low)
{
    /** User can put reset codes here */
    return KBP_OK;
}

/**
 * \brief
 *   Configure KBP OPCODE mapping table.
 * \param [in] unit - The unit number.
 * \param [in] core - The core where the ILKN port reside
 * \param [in] opcode_map - Opcode mapping info.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_kbp_opcode_map_set(
    int unit,
    bcm_core_t core,
    dnx_kbp_opcode_map_t * opcode_map)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGW_RECORD_OPCODE_MAPPING_TABLE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE, opcode_map->opcode);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_RECORD_TYPE, INST_SINGLE, opcode_map->rx_type);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_RECORD_TYPE, INST_SINGLE, opcode_map->tx_type);
    /*
     * The size in opcode mapping table is 0-based.
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_RECORD_SIZE, INST_SINGLE,
                                 opcode_map->rx_size - 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_RECORD_SIZE, INST_SINGLE,
                                 opcode_map->tx_size - 1);

    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Get OPCODE mapping data from the mapping table.
 * \param [in] unit - The unit number.
 * \param [in] core - The core where the ILKN port reside
 * \param [out] opcode_map - Opcode mapping info.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_kbp_opcode_map_get(
    int unit,
    bcm_core_t core,
    dnx_kbp_opcode_map_t * opcode_map)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGW_RECORD_OPCODE_MAPPING_TABLE, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE, opcode_map->opcode);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RX_RECORD_TYPE, INST_SINGLE, &opcode_map->rx_type);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TX_RECORD_TYPE, INST_SINGLE, &opcode_map->tx_type);

    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_RX_RECORD_SIZE, INST_SINGLE, &opcode_map->rx_size);
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_TX_RECORD_SIZE, INST_SINGLE, &opcode_map->tx_size);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Change the record size to 1-based.
     */
    opcode_map->rx_size += 1;
    opcode_map->tx_size += 1;

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get KBP user data, including UNIT and MDIO ID
 *
 * \param [in] unit - The unit number.
 * \param [out] user_data - kbp user data.
 *
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_kbp_init_user_data_get(
    int unit,
    int kbp_inst,
    dnx_kbp_user_data_t * user_data)
{
    SHR_FUNC_INIT_VARS(unit);

    user_data->device = DNX_KBP_IS_SECOND_KBP_EXIST ? kbp_inst : unit;
    user_data->kbp_mdio_id = dnx_data_elk.connectivity.mdio_get(unit, kbp_inst)->mdio_id;

    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get KBP device type.
 *
 * \param [in] unit - The unit number.
 * \param [out] device_type - device type.
 *
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_kbp_init_device_type_get(
    int unit,
    enum kbp_device_type *device_type)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SAL_BOOT_PLISIM)
    {
        /*
         * For SIM, always return the KBP type OP, even though
         * device_type is not defined.
         */
        *device_type = KBP_DEVICE_OP2;
    }
    else if (dnx_data_elk.general.device_type_get(unit) == DNX_KBP_DEV_TYPE_BCM52311)
    {
        *device_type = KBP_DEVICE_OP;
    }
    else if (dnx_data_elk.general.device_type_get(unit) == DNX_KBP_DEV_TYPE_BCM52321)
    {
        *device_type = KBP_DEVICE_OP2;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "Unsupported KBP device type.\n");
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Get the KBP initialziation parameters.
 * \param [in] unit - The unit number.
 * \param [in] core - The core where the ILKN port reside
 * \param [out] flags - KBP device boot flags.
 * \param [out] kbp_type - KBP device type.
 * \param [out] kbp_config - kbp configuration parameters.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_kbp_init_config_param_get(
    int unit,
    int kbp_inst,
    bcm_core_t core,
    uint32 *flags,
    struct kbp_device_config *kbp_config)
{
    int ilkn_num_lanes, ilkn_metaframe;
    bcm_port_t ilkn_port;
    bcm_pbmp_t port_pbmp;
    int ilkn_id, temp_core_id, kbp_port_id;
    bcm_port_resource_t resource;
    int ilkn_rate, i;
    int max_lane_id = dnx_data_elk.general.max_lane_id_get(unit);

    SHR_FUNC_INIT_VARS(unit);

    if (DNX_KBP_OP_IS_DUAL_MODE(unit))
    {
        *flags |= KBP_DEVICE_QUAD_PORT;
    }
    else if (DNX_KBP_OP2_IS_SINGLE_PORT_MODE(unit))
    {
        *flags |= KBP_DEVICE_DEFAULT;
    }
    else
    {
        *flags |= KBP_DEVICE_DUAL_PORT;
    }

    kbp_config->mdio_read = dnx_kbp_callback_mdio_read;
    kbp_config->mdio_write = dnx_kbp_callback_mdio_write;
    kbp_config->handle = &Kbp_app_data[unit]->user_data[kbp_inst];
    kbp_config->assert_kbp_resets = dnx_kbp_callback_reset_device;

    if (dnx_kbp_interface_enabled(unit))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN_ELK, 0, &port_pbmp));

        BCM_PBMP_ITER(port_pbmp, ilkn_port)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, ilkn_port, &ilkn_id));
            SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, ilkn_port, &temp_core_id));

            if (DNX_KBP_IS_SECOND_KBP_EXIST)
            {
                /*
                 * Here is an assumption that the port0/1 must be connected to the
                 * first ilkn of egw. It means the port0/1 connects to
                 * ile0 or ile2,port2/3 connects to ile1 or ile3
                 * For J2, this rule is also applied
                 */
                if ((kbp_inst == 0 && ilkn_id % 2 == 0) || (kbp_inst == 1 && ilkn_id % 2 == 1))
                {
                    kbp_port_id = kbp_inst * 2;
                    kbp_port_id =
                        (dnx_data_elk.connectivity.topology_get(unit)->port_core_mapping[kbp_port_id] ==
                         temp_core_id) ? kbp_port_id : kbp_port_id + 1;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                kbp_port_id = 0;
                kbp_port_id =
                    (dnx_data_elk.connectivity.topology_get(unit)->port_core_mapping[kbp_port_id] ==
                     temp_core_id) ? kbp_port_id : kbp_port_id + 1;
            }

            kbp_config->reverse_lanes |=
                (dnx_data_elk.connectivity.ilkn_reverse_get(unit, ilkn_id)->reverse << (kbp_port_id % 2));
        }
        /*
         *  Take kbp_inst as core since all ile are equal in connectivity topology.
         */
        SHR_IF_ERR_EXIT(dnx_kbp_ilkn_interface_param_get
                        (unit, kbp_inst, &ilkn_port, &ilkn_num_lanes, &ilkn_rate, &ilkn_metaframe));

        switch (ilkn_rate)
        {
            case 10312:
                kbp_config->speed = KBP_INIT_LANE_10_3;
                break;
            case 12500:
                kbp_config->speed = KBP_INIT_LANE_12_5;
                break;
            case 25781:
                kbp_config->speed = KBP_INIT_LANE_25_78125;
                break;
            case 27343:
                kbp_config->speed = KBP_INIT_LANE_27_34375;
                break;
            case 28125:
                kbp_config->speed = KBP_INIT_LANE_28_125;
                break;
            case 53125:
                kbp_config->speed = KBP_INIT_LANE_53_125;
                break;
            case 56250:
                kbp_config->speed = KBP_INIT_LANE_56_25;
                break;
            default:
                SHR_ERR_EXIT(_SHR_E_CONFIG, "Unsupported ILKN lane speed for KBP.\n");
        }

        kbp_port_id = kbp_inst * 2;
        for (i = 0; i < DNX_KBP_USED_KBP_PORTS_NUM; i++)
        {
            kbp_config->port_map[i].start_lane =
                dnx_data_elk.connectivity.topology_get(unit)->start_lane[kbp_port_id + i] % max_lane_id;
            kbp_config->port_map[i].num_lanes = ilkn_num_lanes;
        }

        kbp_config->burst = KBP_INIT_BURST_SHORT_16_BYTES;
        /*
         * metframe length set
         */
        if (ilkn_metaframe == 64 || ilkn_metaframe == 2048)
        {
            kbp_config->meta_frame_len = ilkn_metaframe;
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_CONFIG, "Unsupported ilkn metadata length=%d.\n", ilkn_metaframe);
        }

        kbp_config->init_avs = 1;

        if (Kbp_app_data[unit]->device_type == KBP_DEVICE_OP2)
        {
            /*
             * keep link traing on KBP side aligned with JR2 side
             */
            if (sw_state_is_warm_boot(unit))
            {
               /** Allow writing to registers for port_resource_get during warmboot time */
                SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_enable(unit, UTILEX_SEQ_ALLOW_SCHAN));
            }
            SHR_IF_ERR_EXIT(bcm_dnx_port_resource_get(unit, ilkn_port, &resource));
            if (sw_state_is_warm_boot(unit))
            {
                /** Return to warmboot normal mode */
                SHR_IF_ERR_EXIT(dnxc_ha_tmp_allow_access_disable(unit, UTILEX_SEQ_ALLOW_SCHAN));
            }
            /*
             * Enable link traing on KBP side
             */
            if (resource.link_training != 0)
            {
                kbp_config->settings.link_training_enable = (resource.link_training != 0);
            }
            /*
             * Disable link traing on KBP side and set TX tap
             */
            else
            {
                kbp_config->settings.link_training_enable = 0;
                kbp_config->settings.user_tap_settings = 1;
                kbp_config->settings.tap_settings.enable_taps =
                    dnx_data_elk.connectivity.kbp_serdes_tx_taps_get(unit, kbp_inst)->txfir_tap_enable;
                kbp_config->settings.tap_settings.pre2 =
                    dnx_data_elk.connectivity.kbp_serdes_tx_taps_get(unit, kbp_inst)->pre2;
                kbp_config->settings.tap_settings.pre1 =
                    dnx_data_elk.connectivity.kbp_serdes_tx_taps_get(unit, kbp_inst)->pre;
                kbp_config->settings.tap_settings.mainx =
                    dnx_data_elk.connectivity.kbp_serdes_tx_taps_get(unit, kbp_inst)->main;
                kbp_config->settings.tap_settings.post1 =
                    dnx_data_elk.connectivity.kbp_serdes_tx_taps_get(unit, kbp_inst)->post;
                kbp_config->settings.tap_settings.post2 =
                    dnx_data_elk.connectivity.kbp_serdes_tx_taps_get(unit, kbp_inst)->post2;
                kbp_config->settings.tap_settings.post3 =
                    dnx_data_elk.connectivity.kbp_serdes_tx_taps_get(unit, kbp_inst)->post3;
            }

        }
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Configure the EGW default opcode.
 * \param [in] unit - The unit number.
 * \param [in] core - The core where the ILKN port reside
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_kbp_init_egw_default_opcode_set(
    int unit,
    bcm_core_t core)
{
    dnx_kbp_opcode_map_t opcode_map;

    SHR_FUNC_INIT_VARS(unit);

    opcode_map.rx_type = DNX_KBP_OPCODE_TYPE_REPLY;
    opcode_map.tx_type = DNX_KBP_OPCODE_TYPE_REQUEST;

    opcode_map.opcode = DNX_KBP_CPU_WR_LUT_OPCODE;
    opcode_map.rx_size = DNX_KBP_CPU_WR_LUT_REP_SIZE;
    opcode_map.tx_size = DNX_KBP_CPU_WR_LUT_REQ_SIZE;

    SHR_IF_ERR_EXIT(dnx_kbp_opcode_map_set(unit, core, &opcode_map));

    opcode_map.opcode = DNX_KBP_CPU_RD_LUT_OPCODE;
    opcode_map.rx_size = DNX_KBP_CPU_RD_LUT_REP_SIZE;
    opcode_map.tx_size = DNX_KBP_CPU_RD_LUT_REQ_SIZE;
    SHR_IF_ERR_EXIT(dnx_kbp_opcode_map_set(unit, core, &opcode_map));

    opcode_map.opcode = DNX_KBP_CPU_PIOWR_OPCODE;
    opcode_map.rx_size = DNX_KBP_CPU_PIOWR_REP_SIZE;
    opcode_map.tx_size = DNX_KBP_CPU_PIOWR_REQ_SIZE;
    SHR_IF_ERR_EXIT(dnx_kbp_opcode_map_set(unit, core, &opcode_map));

    opcode_map.opcode = DNX_KBP_CPU_PIORDX_OPCODE;
    opcode_map.rx_size = DNX_KBP_CPU_PIORDX_REP_SIZE;
    opcode_map.tx_size = DNX_KBP_CPU_PIORDX_REQ_SIZE;
    SHR_IF_ERR_EXIT(dnx_kbp_opcode_map_set(unit, core, &opcode_map));

    opcode_map.opcode = DNX_KBP_CPU_PIORDY_OPCODE;
    opcode_map.rx_size = DNX_KBP_CPU_PIORDY_REP_SIZE;
    opcode_map.tx_size = DNX_KBP_CPU_PIORDY_REQ_SIZE;
    SHR_IF_ERR_EXIT(dnx_kbp_opcode_map_set(unit, core, &opcode_map));

    opcode_map.opcode = DNX_KBP_CPU_BLK_COPY_OPCODE;
    opcode_map.rx_size = DNX_KBP_CPU_BLK_COPY_REP_SIZE;
    opcode_map.tx_size = DNX_KBP_CPU_BLK_COPY_REQ_SIZE;
    SHR_IF_ERR_EXIT(dnx_kbp_opcode_map_set(unit, core, &opcode_map));

    opcode_map.opcode = DNX_KBP_CPU_BLK_MOVE_OPCODE;
    opcode_map.rx_size = DNX_KBP_CPU_BLK_MOVE_REP_SIZE;
    opcode_map.tx_size = DNX_KBP_CPU_BLK_MOVE_REQ_SIZE;
    SHR_IF_ERR_EXIT(dnx_kbp_opcode_map_set(unit, core, &opcode_map));

    opcode_map.opcode = DNX_KBP_CPU_BLK_CLR_OPCODE;
    opcode_map.rx_size = DNX_KBP_CPU_BLK_CLR_REP_SIZE;
    opcode_map.tx_size = DNX_KBP_CPU_BLK_CLR_REQ_SIZE;
    SHR_IF_ERR_EXIT(dnx_kbp_opcode_map_set(unit, core, &opcode_map));

    opcode_map.opcode = DNX_KBP_CPU_BLK_EV_OPCODE;
    opcode_map.rx_size = DNX_KBP_CPU_BLK_EV_REP_SIZE;
    opcode_map.tx_size = DNX_KBP_CPU_BLK_EV_REQ_SIZE;
    SHR_IF_ERR_EXIT(dnx_kbp_opcode_map_set(unit, core, &opcode_map));
exit:
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Configure EGW lookup parameters.
 * \param [in] unit - The unit number.
 * \param [in] core - The core where the ILKN port reside
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_kbp_init_egw_lookup_config_set(
    int unit,
    bcm_core_t core)
{
    uint32 entry_handle_id;
    uint32 lookup_time_delay;
    int ilkn_id;
    uint32 nif_enable = 0;
    bcm_port_t ilkn_port;
    bcm_pbmp_t port_pbmp;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    lookup_time_delay = dnx_data_elk.general.lookup_timeout_delay_get(unit);

    if (dnx_data_elk.general.feature_get(unit, dnx_data_elk_general_is_egw_support_rr))
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                        (unit, core, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN_ELK, 0, &port_pbmp));

        BCM_PBMP_ITER(port_pbmp, ilkn_port)
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_interface_offset_get(unit, ilkn_port, &ilkn_id));
            nif_enable |= (1 << (ilkn_id % 2));
        }
    }
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGW_LOOKUP_CTRL, &entry_handle_id));

    /*
     * set key field
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ELK_LOOKUP_EN, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_LOOKUP_TIMEOUT_DLY, INST_SINGLE, lookup_time_delay);

    if (dnx_data_elk.general.feature_get(unit, dnx_data_elk_general_is_egw_support_rr))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_NIF_ENABLE, INST_SINGLE, nif_enable);
    }
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configure EGW transmission parameters.
 * \param [in] unit - The unit number.
 * \param [in] core - The core where the ILKN port reside
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_kbp_init_egw_transmit_config_set(
    int unit,
    bcm_core_t core)
{
    uint32 entry_handle_id;
    uint32 trans_timer_delay;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    trans_timer_delay = dnx_data_elk.general.trans_timer_delay_get(unit);

    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGW_TRANSMIT_ATTRIBUTES_CFG, &entry_handle_id));
    /*
     * set key field
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    /*
     * set value field
     */
    /*
     * Minimum/Maximum packet size in B, up to which packet not closed
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MIN_PKT_SIZE, INST_SINGLE, 64);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MAX_PKT_SIZE, INST_SINGLE, 256);
    /*
     * EGW will not transmit FLP records if FIFO exceed this threshold
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_FLP_TRANSMIT_THRESHOLD, INST_SINGLE, 0x190);
    /*
     * Congestion identified when FIFO exceed this threshold
     * (enables ROP style packing and/or MaxPktSize reaching)
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CONGESTION_THRESHOLD, INST_SINGLE, 0x14);
    /*
     * Enable ROP style packing
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ROP_PACKING_ENABLE, INST_SINGLE, 1);
    /*
     * set CPU priority over traffic lookups
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_CPU_RECORD_PRIO, INST_SINGLE, 1);
    /*
     * Enable transmit timer mode and configure packing delay,
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TIMER_MODE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TIMER_DELAY, INST_SINGLE, trans_timer_delay);
    /*
     * Enables Attaching application prefix in packet transmition/reciving
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_APP_PREFIX_EN, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_APP_PREFIX_EN, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_CHANNEL_NUM, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RX_CHANNEL_NUM, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_MODE, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_MIN_BURST, INST_SINGLE, 1);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ILKN_MAX_BURST, INST_SINGLE, 7);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Configure EGW shaper parameters.
 * \param [in] unit - The unit number.
 * \param [in] core - The core where the ILKN port reside
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_kbp_init_egw_shaper_config_set(
    int unit,
    bcm_core_t core)
{
    int ilkn_total_mbits_rate, num_lanes, ilkn_rate, meta_frame_len;
    uint32 entry_handle_id;
    uint32 ilkn_total_burst_rate, core_clk_ticks;
    uint32 spr_dly_mega, spr_dly_fld, spr_dly_fraction_fld;
    bcm_port_t ilkn_port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_kbp_ilkn_interface_param_get(unit, core, &ilkn_port, &num_lanes, &ilkn_rate, &meta_frame_len));
    /*
     * alloc DBAL table handle
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGW_SHAPER_CFG, &entry_handle_id));
    /*
     * set key field
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    ilkn_total_mbits_rate = num_lanes * ilkn_rate;
    ilkn_total_burst_rate = ilkn_total_mbits_rate / (EGW_TO_ILKN_DATA_WIDTH * 8);
    core_clk_ticks = dnx_data_device.general.core_clock_khz_get(unit) * 1000;
    /*
     * The actual ILKN rate should consider:
     *     overhead for metaframe(4B);
     *     64B/67B encoding;
     *     overhead for burst control(8B).
     *
     * in case of overflow, EGW_TO_ILKN_DATA_WIDTH=64;
     *  (64 * EGW_TO_ILKN_DATA_WIDTH) / (67 * (EGW_TO_ILKN_DATA_WIDTH + 8))
     *   = (64 * 64)/ (67 *72) = 85/100 = 17/20
     */
    spr_dly_mega = core_clk_ticks / ((ilkn_total_burst_rate * (meta_frame_len - 4) * 17) / (meta_frame_len * 20));
    /*
     * Need to find the fraction = ((64 * 1024 * dly_mega) / 1000000).
     * since the fraction is = x/64K. 64/1000000 = 1/15625
     */
    spr_dly_fld = spr_dly_mega / 1000000;
    spr_dly_fraction_fld = ((spr_dly_mega % 1000000) * 1024) / 15625;
    /*
     * set value field
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SHAPER_DLY, INST_SINGLE, spr_dly_fld);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SHAPER_DLY_FRACTION, INST_SINGLE,
                                 spr_dly_fraction_fld);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_SHAPER_MAX_BURST, INST_SINGLE, 0x1FF);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_WORD_ALIGNMENT, INST_SINGLE, 0X3);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_GAP, INST_SINGLE, 0);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PACKET_OVERHEAD, INST_SINGLE, 0X8);
    /*
     * commit the value
     */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;

}

/**
 * \brief
 *   Configure EGW parameters for KBP
 * \param [in] unit - The unit number.
 * \param [in] core - The core where the ILKN port reside
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_kbp_init_egw_config_set(
    int unit,
    bcm_core_t core)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_kbp_init_egw_lookup_config_set(unit, core));
    SHR_IF_ERR_EXIT(dnx_kbp_init_egw_transmit_config_set(unit, core));
    SHR_IF_ERR_EXIT(dnx_kbp_init_egw_shaper_config_set(unit, core));
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_kbp_init_kbp_stat_interface(
    int unit)
{
    uint32 port_speed = DNXC_PORT_INVALID_SPEED, final_speed = DNXC_PORT_INVALID_SPEED;
    uint32 port_bmp = 0, port_id;
    uint32 link_training = 0, final_link_training = 0, is_nrz = FALSE;
    int nof_lanes, final_nof_lanes = 4;
    struct kbp_serdes_tap_settings txfir_vals;
    int kbp_inst;
    SHR_FUNC_INIT_VARS(unit);

    DNX_KBP_INST_ITER(unit, kbp_inst)
    {
        /** read stat port soc properties */
        /** the below loop reads ports 0 and 2 for kbp_inst=0 and ports 1 and 3 for kbp_inst=1 */
        /** port_bmp should be set is such way that only 0 and 2 could be set */
        for (port_id = kbp_inst; port_id < DNX_KBP_STAT_PORTS_MAX; port_id += DNX_KBP_MAX_NOF_INST)
        {
            port_speed = dnx_data_port.static_add.ext_stat_speed_get(unit, port_id)->speed;
            if ((port_speed != 100000) && (port_speed != 200000) && (port_speed != DNXC_PORT_INVALID_SPEED))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "port_speed=%d. Unsupported ext stat port speed.\n", port_speed);
            }
            nof_lanes = dnx_data_port.static_add.ext_stat_speed_get(unit, port_id)->nof_lanes;
            if ((nof_lanes != 0) && (nof_lanes != 2) && (nof_lanes != 4))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "nof_lanes=%d. Unsupported ext stat port nof lanes.\n", nof_lanes);
            }
            if (port_speed != DNXC_PORT_INVALID_SPEED)
            {
                link_training = dnx_data_port.static_add.ext_stat_link_training_get(unit, port_id)->val;
                /** It's not possible to configure two ports with different speeds and link training. */
                /** All ports must be set to same speed and link training values.*/
                if ((port_speed != final_speed) && (port_bmp != 0))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "all ext stat ports speed must have the same value \n");
                }
                if ((link_training != final_link_training) && (port_bmp != 0))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "all ext stat ports link training configuration must be the same \n");
                }
                if ((nof_lanes != final_nof_lanes) && (port_bmp != 0))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "all ext stat ports nof lanes configuration must be the same \n");
                }

                SHR_IF_ERR_EXIT(utilex_bitstream_set(&port_bmp, (port_id - kbp_inst), 1));
                final_speed = port_speed;
                final_link_training = link_training;
                final_nof_lanes = nof_lanes;
            }
        }

        /*
         * Port Macro init should be done only once
         * during board reset time.
         */
        if (port_bmp != 0)
        {
            sal_msleep(1);
            DNX_KBP_TRY(kbp_pcie_reset(KBP_DEVICE_OP2, Kbp_app_data[unit]->xpt_p[kbp_inst], KBP_PCIE_PM0_SW_SRST));
            DNX_KBP_TRY(kbp_pcie_reset(KBP_DEVICE_OP2, Kbp_app_data[unit]->xpt_p[kbp_inst], KBP_PCIE_PM1_SW_SRST));

            is_nrz = ((final_speed == 100000) && (final_nof_lanes == 4)) ? TRUE : FALSE;

            /** if link training is on, no need to configure TXFIR (tx taps) strucutre */
            if (link_training == TRUE)
            {
                DNX_KBP_TRY(kbp_device_pm_interface_init
                            (unit, Kbp_app_data[unit]->xpt_p[kbp_inst], (final_speed / 1000), port_bmp, is_nrz,
                             link_training, NULL));
            }
            else
            {
                /** if we have global TXFIR configuration, use it */
                if (dnx_data_port.static_add.ext_stat_global_serdes_tx_taps_get(unit)->main !=
                    DNXC_PORT_TX_FIR_INVALID_MAIN_TAP)
                {
                    txfir_vals.enable_taps =
                        dnx_data_port.static_add.ext_stat_global_serdes_tx_taps_get(unit)->txfir_tap_enable;
                    txfir_vals.pre2 = dnx_data_port.static_add.ext_stat_global_serdes_tx_taps_get(unit)->pre2;
                    txfir_vals.pre1 = dnx_data_port.static_add.ext_stat_global_serdes_tx_taps_get(unit)->pre;
                    txfir_vals.mainx = dnx_data_port.static_add.ext_stat_global_serdes_tx_taps_get(unit)->main;
                    txfir_vals.post1 = dnx_data_port.static_add.ext_stat_global_serdes_tx_taps_get(unit)->post;
                    txfir_vals.post2 = dnx_data_port.static_add.ext_stat_global_serdes_tx_taps_get(unit)->post2;
                    txfir_vals.post3 = dnx_data_port.static_add.ext_stat_global_serdes_tx_taps_get(unit)->post3;
                    DNX_KBP_TRY(kbp_device_pm_interface_init
                                (unit, Kbp_app_data[unit]->xpt_p[kbp_inst], (final_speed / 1000), port_bmp, is_nrz,
                                 link_training, &txfir_vals));
                }
                else
                {
                    DNX_KBP_TRY(kbp_device_pm_interface_init
                                (unit, Kbp_app_data[unit]->xpt_p[kbp_inst], (final_speed / 1000), port_bmp, is_nrz,
                                 link_training, NULL));
                }
                    /** Configure TXFIR per lane, if exist. API gets uint32 bitmap lanes. Each call we will configure different lane */
                for (int lane_idx = 0; lane_idx < 32; lane_idx++)
                {
                    /** INVALID_MAIN_TAP means that there is no Soc property configuration for this lane */
                    if (dnx_data_port.static_add.ext_stat_lane_serdes_tx_taps_get(unit, lane_idx)->main !=
                        DNXC_PORT_TX_FIR_INVALID_MAIN_TAP)
                    {
                        txfir_vals.enable_taps =
                            dnx_data_port.static_add.ext_stat_lane_serdes_tx_taps_get(unit, lane_idx)->txfir_tap_enable;
                        txfir_vals.pre2 =
                            dnx_data_port.static_add.ext_stat_lane_serdes_tx_taps_get(unit, lane_idx)->pre2;
                        txfir_vals.pre1 =
                            dnx_data_port.static_add.ext_stat_lane_serdes_tx_taps_get(unit, lane_idx)->pre;
                        txfir_vals.mainx =
                            dnx_data_port.static_add.ext_stat_lane_serdes_tx_taps_get(unit, lane_idx)->main;
                        txfir_vals.post1 =
                            dnx_data_port.static_add.ext_stat_lane_serdes_tx_taps_get(unit, lane_idx)->post;
                        txfir_vals.post2 =
                            dnx_data_port.static_add.ext_stat_lane_serdes_tx_taps_get(unit, lane_idx)->post2;
                        txfir_vals.post3 =
                            dnx_data_port.static_add.ext_stat_lane_serdes_tx_taps_get(unit, lane_idx)->post3;
                        DNX_KBP_TRY(kbp_device_pm_interface_serdes_tap_settings
                                    (Kbp_app_data[unit]->xpt_p[kbp_inst], (1 << lane_idx), &txfir_vals));
                    }
                }
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   get if kbp stat interface enable.
 *   return true if stif is enabled and at least one kbp ext_stat port is configured.
 *   \param [in] unit - Relevant unit.
 *   \param [out] enable - True if kbp stat enabled.
 *  \return
 *    \retval errors if unexpected behavior. See \ref shr_error_e
 *  \remark
 *    None
 *  \see
 *    shr_error_e
 */
shr_error_e
dnx_kbp_stat_interface_enable_get(
    int unit,
    int *enable)
{
    uint32 port_id;
    uint32 port_speed;
    SHR_FUNC_INIT_VARS(unit);

    *enable = FALSE;
    if (dnx_data_stif.config.feature_get(unit, dnx_data_stif_config_stif_enable) == TRUE)
    {
        /** read stat port soc properties */
        for (port_id = 0; port_id < DNX_KBP_STAT_PORTS_MAX; port_id++)
        {
            port_speed = dnx_data_port.static_add.ext_stat_speed_get(unit, port_id)->speed;
            if (port_speed != DNXC_PORT_INVALID_SPEED)
            {
                *enable = TRUE;
                break;
            }
        }
    }

/*exit:*/
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize KBP interce on KBP side.
 * \param [in] unit - The unit number.
 * \param [in] core - The core where the ILKN port reside
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

shr_error_e
dnx_kbp_init_kbp_interface(
    int unit,
    int kbp_inst)
{

    SHR_FUNC_INIT_VARS(unit);

    DNX_KBP_TRY(kbp_device_interface_init
                (Kbp_app_data[unit]->device_type, Kbp_app_data[unit]->flags,
                 &Kbp_app_data[unit]->device_config[kbp_inst]));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize KBP interface on DNX side.
 *   Mainly refer to the EGW. The ILE module
 *   has been initialized during NIF init phase.
 * \param [in] unit - The unit number.
 * \param [in] core - The core where the ILKN port reside
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */

static shr_error_e
dnx_kbp_init_dnx_interface(
    int unit,
    bcm_core_t core)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_kbp_init_egw_config_set(unit, core));
    SHR_IF_ERR_EXIT(dnx_kbp_init_egw_default_opcode_set(unit, core));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Initialize the kbp interface on DNX and KBP side
 *   and KBP side.
 * \param [in] unit - The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_kbp_interface_init(
    int unit)
{
    bcm_pbmp_t elk_pbmp;
    bcm_port_t elk_port;
    bcm_core_t core;
    soc_timeout_t to;
    sal_usecs_t timeout = 1000000;
    bcm_port_link_state_t link_state;
    struct kbp_serdes_tap_settings txfir_vals;
    struct kbp_device_config *kbp_config;
    int max_lane_id = dnx_data_elk.general.max_lane_id_get(unit);
    int kbp_inst, start_lane_idx, end_lane_idx;
    int lane_offset;
    int kbp_mdio_id, kbp_port_id = 0;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ILKN_ELK, 0, &elk_pbmp));

    DNXCMN_CORES_ITER(unit, BCM_CORE_ALL, core)
    {
        SHR_IF_ERR_EXIT(dnx_kbp_init_dnx_interface(unit, core));
    }

    DNX_KBP_INST_ITER(unit, kbp_inst)
    {
        kbp_config = &Kbp_app_data[unit]->device_config[kbp_inst];
        SHR_IF_ERR_EXIT(dnx_kbp_init_kbp_interface(unit, kbp_inst));

        /** if link training is on, no need to configure TXFIR (tx taps) strucutre */
        if (Kbp_app_data[unit]->device_type == KBP_DEVICE_OP2 && kbp_config->settings.link_training_enable == 0)
        {
            /** Configure TXFIR per lane, if exist. API gets uint32 bitmap lanes. Each call we will configure different lane */
            start_lane_idx = kbp_inst * max_lane_id;
            end_lane_idx = start_lane_idx + max_lane_id;
            for (int lane_idx = start_lane_idx; lane_idx < end_lane_idx; lane_idx++)
            {
                /** INVALID_MAIN_TAP means that there is no Soc property configuration for this lane */
                if (dnx_data_elk.connectivity.kbp_lane_serdes_tx_taps_get(unit, lane_idx)->main !=
                    DNXC_PORT_TX_FIR_INVALID_MAIN_TAP)
                {
                    txfir_vals.enable_taps =
                        dnx_data_elk.connectivity.kbp_lane_serdes_tx_taps_get(unit, lane_idx)->txfir_tap_enable;
                    txfir_vals.pre2 = dnx_data_elk.connectivity.kbp_lane_serdes_tx_taps_get(unit, lane_idx)->pre2;
                    txfir_vals.pre1 = dnx_data_elk.connectivity.kbp_lane_serdes_tx_taps_get(unit, lane_idx)->pre;
                    txfir_vals.mainx = dnx_data_elk.connectivity.kbp_lane_serdes_tx_taps_get(unit, lane_idx)->main;
                    txfir_vals.post1 = dnx_data_elk.connectivity.kbp_lane_serdes_tx_taps_get(unit, lane_idx)->post;
                    txfir_vals.post2 = dnx_data_elk.connectivity.kbp_lane_serdes_tx_taps_get(unit, lane_idx)->post2;
                    txfir_vals.post3 = dnx_data_elk.connectivity.kbp_lane_serdes_tx_taps_get(unit, lane_idx)->post3;
                    lane_offset = lane_idx % max_lane_id;
                    DNX_KBP_TRY(kbp_device_interface_serdes_tap_settings
                                (KBP_DEVICE_OP2, kbp_config, (1 << lane_offset), &txfir_vals));
                }
            }
        }
    }

    if (Kbp_app_data[unit]->device_type == KBP_DEVICE_OP)
    {
        kbp_mdio_id = dnx_data_elk.connectivity.mdio_get(unit, 0)->mdio_id;
        BCM_PBMP_ITER(elk_pbmp, elk_port)
        {
            kbp_port_id = 0;
            kbp_port_id =
                (dnx_data_elk.connectivity.topology_get(unit)->port_core_mapping[kbp_port_id] ==
                 core) ? kbp_port_id : kbp_port_id + 1;

            SHR_IF_ERR_EXIT(dnx_kbp_phase_matching_fifo_reset(unit, kbp_mdio_id, kbp_port_id, 1));
            sal_usleep(200000);
            SHR_IF_ERR_EXIT(dnx_kbp_phase_matching_fifo_reset(unit, kbp_mdio_id, kbp_port_id, 0));
            sal_usleep(400000);
        }
    }

    BCM_PBMP_ITER(elk_pbmp, elk_port)
    {

        /*
         * Polling for link status up
         */
        soc_timeout_init(&to, timeout, 100);
        for (;;)
        {
            SHR_IF_ERR_EXIT(imb_port_link_state_get(unit, elk_port, 1, &link_state));
            if (link_state.status == 1)
            {
                break;
            }
            if (soc_timeout_check(&to))
            {
                SHR_ERR_EXIT(_SHR_E_UNAVAIL, "ILKN port is down %d\n", elk_port);
            }

            sal_usleep(10000);
        }

        if (dnx_data_nif.ilkn.feature_get(unit, dnx_data_nif_ilkn_elk_require_core_reset_after_link_training))
        {
            bcm_port_resource_t resource;
            uint32 pmd_lock_counter;

            SHR_IF_ERR_EXIT(bcm_dnx_port_resource_get(unit, elk_port, &resource));
            if (resource.link_training)
            {
                /** store PMD lock counter to know for what value ILKN core reset is done */
                SHR_IF_ERR_EXIT(portmod_port_pmd_lock_counter_get(unit, elk_port, &pmd_lock_counter));
                SHR_IF_ERR_EXIT(imbm.pmd_lock_counter.set(unit, elk_port, pmd_lock_counter));

                /** reset everything but the serdes to clear ILKN core state */
                SHR_IF_ERR_EXIT(imb_port_enable_set(unit, elk_port, IMB_PORT_ENABLE_F_SKIP_PORTMOD, 0));
                SHR_IF_ERR_EXIT(imb_port_enable_set(unit, elk_port, IMB_PORT_ENABLE_F_SKIP_PORTMOD, 1));
            }
        }

        /*
         * Enable RX data manually during init sequence
         */
        SHR_IF_ERR_EXIT(imb_port_rx_data_enable_set(unit, elk_port, 1));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Reset KBP before initializing
 *
 * \param [in] unit - The unit number.
 * \return
 *   See \ref shr_error_e
 * \remark
 *   None.
 * \see
 *   None.
 */
shr_error_e
dnx_kbp_init_reset(
    int unit)
{
    int kbp_inst;
    SHR_FUNC_INIT_VARS(unit);

    DNX_KBP_INST_ITER(unit, kbp_inst)
    {
        /*
         * KBP Reset
         */
        if (!SAL_BOOT_PLISIM)
        {
            DNX_KBP_TRY(kbp_device_interface_reset(Kbp_app_data[unit]->device_type,
                                                   &Kbp_app_data[unit]->device_config[kbp_inst]));
        }
    }
exit:
    SHR_FUNC_EXIT;
}
/*
 * }
 */

/*
 * }
 */
#endif /* defined(INCLUDE_KBP) */

#undef BSL_LOG_MODULE
