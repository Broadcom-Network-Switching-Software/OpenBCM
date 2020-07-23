/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/** \file algo_gpm.c
 *  Gport Managment procedures for DNX.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT
/*
 * Includes.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/dnx_dispatch.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include <bcm/types.h>
#include <shared/trunk.h>
#include <bcm/trunk.h>
#include <bcm_int/dnx/algo/algo_gpm.h>

#include <soc/feature.h>
#include <soc/types.h>
#include <soc/dnxc/drv_dnxc_utils.h>

#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/trunk/algo_trunk.h>
#include <bcm_int/dnx/stk/stk_sys.h>
#include <bcm_int/dnx/switch/switch.h>
#include <bcm_int/dnx/cosq/ingress/ipq.h>
#include <bcm_int/dnx/trunk/trunk.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/vlan/vlan.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_port.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_trunk.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_esem.h>
#include <bcm_int/dnx/port/port_prt_tcam.h>
/*
 * }
 */

/*
 * Defines.
 * {
 */
/*
 * Invalid destination, packet will be dropped
 */
#define DNX_ALGO_GPM_DESTINATION_INVALID 0x17FFFF
/**
 * Invalid DBAL result type
 */
#define DNX_ALGO_GPM_DBAL_RESULT_TYPE_INVALID (-1)

#define DNX_ALGO_GPM_GPORT_HW_RESOURCES_INGRESS_FLAGS \
        (DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS \
        | DNX_ALGO_GPM_GPORT_HW_RESOURCES_SBC_LOCAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS)

#define DNX_ALGO_GPM_GPORT_HW_RESOURCES_EGRESS_FLAGS \
        (DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS)

#define DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_FLAGS \
        (DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_SBC_LOCAL_LIF_INGRESS \
                        | DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS)

#define DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_FLAGS \
    (DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS)

#define DNX_ALGO_GPM_GPORT_HW_RESOURCE_GPORT_FROM_LIF_SUPPORTED_FLAGS \
    (DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_FLAGS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_FLAGS)

/*
 * }
 */

/*
 * Macros.
 * {
 */

/*
 * Update internal struct dnx_algo_gpm_gport_phy_pp_port_info_t with single pp port information
 */
#define DNX_ALGO_GPM_GPORT_PHY_PP_PORT_INFO_SET_SINGLE(_port_pp_info, _core_id, _pp_port)\
    { \
        _port_pp_info.nof_pp_ports = 1; \
        _port_pp_info.core_id[0] = _core_id; \
        _port_pp_info.pp_port[0] = _pp_port; \
    }
/*
 * }
 */

/*
 * See .h file
 * Note: PON is not supported.
 */
shr_error_e
dnx_algo_gpm_gport_phy_info_get(
    int unit,
    bcm_gport_t port,
    uint32 operations,
    dnx_algo_gpm_gport_phy_info_t * gport_info)
{

    shr_error_e rv = _SHR_E_NONE;
    bcm_gport_t gport;
    bcm_module_t modid = 0;
    bcm_port_t tm_port = SOC_MAX_NUM_PORTS;
    int queue_id = -1;
    int trunk_id;
    int core_id;
    int internal_port_pp;
    int is_local;
    int ftmh_pp_dsp;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify the pointer 'gport_info' is not NULL and exit with error if it is.
     */
    SHR_NULL_CHECK(gport_info, _SHR_E_PARAM, "gport_info");

    /*
     * Init Struct.
     */
    gport_info->sys_port = -1;
    gport_info->lane = -1;
    gport_info->flags = 0;
    gport_info->local_port = SOC_MAX_NUM_PORTS;
    gport_info->internal_port_pp_info.nof_pp_ports = 0;
    for (core_id = 0; core_id < DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES; core_id++)
    {
        gport_info->internal_port_pp_info.pp_port[core_id] = -1;
        gport_info->internal_port_pp_info.core_id[core_id] = -1;
    }
    BCM_PBMP_CLEAR(gport_info->local_port_bmp);

    /*
     * If GPORT is invalid EXIT
     */
    if (port == BCM_GPORT_INVALID)
    {
        SHR_EXIT();
    }

    /*
     * Check if gport is actually local port that is not encoded as GPORT
     */
    if (BCM_GPORT_IS_SET(port))
    {
        gport = port;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));
        BCM_GPORT_LOCAL_SET(gport, port);
    }

    if (BCM_GPORT_IS_LOCAL(gport) || BCM_GPORT_IS_LOCAL_INTERFACE(gport))
    {
        dnx_algo_port_info_s port_info;
        int port_in_lag;
        uint32 flags;

        /*
         * Local Port
         */
        if BCM_GPORT_IS_LOCAL
            (gport)
        {
            gport_info->local_port = BCM_GPORT_LOCAL_GET(gport);
        }
        else
        {
            gport_info->local_port = BCM_GPORT_LOCAL_INTERFACE_GET(gport);
        }
        gport_info->flags |= DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT;

        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, gport_info->local_port, &port_info));
        SHR_IF_ERR_EXIT(dnx_algo_port_in_lag(unit, gport_info->local_port, &port_in_lag));

        /**get the flag only when it's represents pp port */
        if (DNX_ALGO_PORT_TYPE_IS_PP(unit, port_info, 0))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_app_flags_get(unit, gport_info->local_port, &flags));
        }
        else
        {
            flags = 0;
        }

        if (DNX_ALGO_PORT_TYPE_IS_PP(unit, port_info, port_in_lag))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_get
                            (unit, gport_info->local_port, &core_id, (uint32 *) &internal_port_pp));
            DNX_ALGO_GPM_GPORT_PHY_PP_PORT_INFO_SET_SINGLE(gport_info->internal_port_pp_info, core_id,
                                                           internal_port_pp);
        }
        else if ((flags & DNX_ALGO_PORT_APP_FLAG_COE) && port_in_lag)
        {
            uint32 tcam_access_id;
            uint32 key_value;

            /*
             *  Allow to get pp_port when COE port in LAG
             */
            SHR_IF_ERR_EXIT(dnx_port_prt_tcam_virtual_port_tcam_coe_lag_sw_get
                            (unit, gport_info->local_port, (uint32 *) &core_id, (uint32 *) &internal_port_pp,
                             &tcam_access_id, &key_value));
            DNX_ALGO_GPM_GPORT_PHY_PP_PORT_INFO_SET_SINGLE(gport_info->internal_port_pp_info, core_id,
                                                           internal_port_pp);
        }

        /*
         * get physical system port, identify <mod,port>
         */
        if (operations & DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT)
        {
            SHR_IF_ERR_EXIT(dnx_stk_sys_logical_port_to_sysport_map_get
                            (unit, gport_info->local_port, &gport_info->sys_port));
        }
    }
    else if (BCM_GPORT_IS_MODPORT(gport))
    {
        /*
         * Mod Port
         */
        modid = BCM_GPORT_MODPORT_MODID_GET(gport);
        ftmh_pp_dsp = BCM_GPORT_MODPORT_PORT_GET(gport);

        SHR_IF_ERR_EXIT(dnx_stk_sys_modid_is_local_get(unit, modid, &is_local));
        if (is_local)
        {
            dnx_algo_port_info_s port_info;
            int port_in_lag;

            gport_info->flags |= DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT;
            /*
             * Get logical port
             */
            SHR_IF_ERR_EXIT(dnx_stk_sys_modport_to_tm_port_convert(unit, modid, ftmh_pp_dsp, &core_id, &tm_port));
            SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core_id, tm_port, &gport_info->local_port));
            /*
             * Get PP port
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, gport_info->local_port, &port_info));
            SHR_IF_ERR_EXIT(dnx_algo_port_in_lag(unit, gport_info->local_port, &port_in_lag));
            if (DNX_ALGO_PORT_TYPE_IS_PP(unit, port_info, port_in_lag))
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_get
                                (unit, gport_info->local_port, &core_id, (uint32 *) &internal_port_pp));
                DNX_ALGO_GPM_GPORT_PHY_PP_PORT_INFO_SET_SINGLE(gport_info->internal_port_pp_info, core_id,
                                                               internal_port_pp);
            }

        }
        /*
         * Throw an error in a case modport to system port is required.
         * Few system ports might be mapped to a modport.
         */
        if (operations & DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT)
        {
            SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                         "Setting / Getting system port attributes using modport gport is not supported.\n The configuration should use system port explicitly.\n");
        }
    }
    else if (BCM_GPORT_IS_DEVPORT(gport))
    {
        /*
         * Device Port
         */
        gport_info->local_port = BCM_GPORT_DEVPORT_PORT_GET(gport);
        if (unit == BCM_GPORT_DEVPORT_DEVID_GET(gport))
        {
            gport_info->flags |= DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT;
        }
    }
    else if (BCM_GPORT_IS_TRUNK(gport))
    {
        int nof_cores = dnx_data_device.general.nof_cores_get(unit);
        int pp_ports_counter = 0;

        trunk_id = BCM_GPORT_TRUNK_GET(gport);

        /*
         * Get system port.
         */
        SHR_IF_ERR_EXIT(dnx_trunk_gport_to_spa(unit, gport, 0, &gport_info->sys_port));

        /*
         * Iterate over cores, get local and pp port from each.
         * PP port on each core can be valid / invalid
         */
        for (int core = 0; core < nof_cores; ++core)
        {
            bcm_pbmp_t local_port_pbmp_per_core;
            BCM_PBMP_CLEAR(local_port_pbmp_per_core);
            SHR_IF_ERR_EXIT(dnx_trunk_local_port_bmp_get(unit, trunk_id, core, &local_port_pbmp_per_core));
            BCM_PBMP_OR(gport_info->local_port_bmp, local_port_pbmp_per_core);
            SHR_IF_ERR_EXIT(dnx_trunk_pp_port_get(unit, trunk_id, core, (uint32 *) &internal_port_pp));
            if (internal_port_pp != dnx_data_port.general.nof_pp_ports_get(unit))
            {
                gport_info->internal_port_pp_info.core_id[pp_ports_counter] = core;
                gport_info->internal_port_pp_info.pp_port[pp_ports_counter] = internal_port_pp;
                pp_ports_counter = pp_ports_counter + 1;
            }
        }
        gport_info->internal_port_pp_info.nof_pp_ports = pp_ports_counter;

        /** get flags */
        gport_info->flags |= DNX_ALGO_GPM_GPORT_INFO_F_IS_LAG;
        /** get first local port */
        _SHR_PBMP_FIRST(gport_info->local_port_bmp, gport_info->local_port);
        if (gport_info->local_port != _SHR_PORT_INVALID)
        {
            gport_info->flags |= DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT;
        }
    }
    else if (BCM_GPORT_IS_SYSTEM_PORT(gport))
    {
        /*
         * System Port
         */
        bcm_trunk_t trunk_id = 0;
        gport_info->sys_port = BCM_GPORT_SYSTEM_PORT_ID_GET(gport);

        /*
         * Get Modid and TM port.
         */
        SHR_IF_ERR_EXIT(dnx_stk_sys_sysport_modport_map_get
                        (unit, gport_info->sys_port, (uint32 *) &modid, (uint32 *) &ftmh_pp_dsp));

        /*
         * Modid to logical port (only if local port)
         */
        SHR_IF_ERR_EXIT(dnx_stk_sys_modid_is_local_get(unit, modid, &is_local));
        if (is_local)
        {
            gport_info->flags |= DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT;
            /*
             * Get logical port
             */
            SHR_IF_ERR_EXIT(dnx_stk_sys_modport_to_tm_port_convert(unit, modid, ftmh_pp_dsp, &core_id, &tm_port));
            SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core_id, tm_port, &gport_info->local_port));
            /*
             * Get PP port
             */
            rv = bcm_dnx_trunk_find(unit, 0, gport, &trunk_id);
            if (rv == _SHR_E_NOT_FOUND)
            {
                dnx_algo_port_info_s port_info;
                int port_in_lag;

                SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, gport_info->local_port, &port_info));
                SHR_IF_ERR_EXIT(dnx_algo_port_in_lag(unit, gport_info->local_port, &port_in_lag));
                if (DNX_ALGO_PORT_TYPE_IS_PP(unit, port_info, port_in_lag))
                {
                    /** get PP port only if port is not part of lag */

                    SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_get
                                    (unit, gport_info->local_port, &core_id, (uint32 *) &internal_port_pp));
                    DNX_ALGO_GPM_GPORT_PHY_PP_PORT_INFO_SET_SINGLE(gport_info->internal_port_pp_info, core_id,
                                                                   internal_port_pp);
                }
            }
            else
            {
                SHR_IF_ERR_EXIT(rv);
            }

        }

    }
    else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport))
    {   /* unicast queue */
        queue_id = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport);
        core_id = BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(gport);

        /*
         * System Port
         */
        SHR_IF_ERR_EXIT(dnx_ipq_queue_sysport_map_get(unit, core_id, queue_id, &(gport_info->sys_port)));

        /*
         * Get Modid and TM port.
         */
        SHR_IF_ERR_EXIT(dnx_stk_sys_sysport_modport_map_get
                        (unit, gport_info->sys_port, (uint32 *) &modid, (uint32 *) &ftmh_pp_dsp));

        /*
         * Modid to logical port (only if local port)
         */
        SHR_IF_ERR_EXIT(dnx_stk_sys_modid_is_local_get(unit, modid, &is_local));
        if (is_local)
        {
            dnx_algo_port_info_s port_info;
            int port_in_lag;

            gport_info->flags |= DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT;
            /*
             * Get logical port
             */
            SHR_IF_ERR_EXIT(dnx_stk_sys_modport_to_tm_port_convert(unit, modid, ftmh_pp_dsp, &core_id, &tm_port));
            SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core_id, tm_port, &gport_info->local_port));
            /*
             * Get PP port
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, gport_info->local_port, &port_info));
            SHR_IF_ERR_EXIT(dnx_algo_port_in_lag(unit, gport_info->local_port, &port_in_lag));
            if (DNX_ALGO_PORT_TYPE_IS_PP(unit, port_info, port_in_lag))
            {
                /** get PP port only if port is not part of lag */
                SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_get
                                (unit, gport_info->local_port, &core_id, (uint32 *) &internal_port_pp));
                DNX_ALGO_GPM_GPORT_PHY_PP_PORT_INFO_SET_SINGLE(gport_info->internal_port_pp_info, core_id,
                                                               internal_port_pp);
            }
        }
    }
    else if (BCM_GPORT_IS_LOCAL_CPU(gport))
    {   /* CPU-port */
        bcm_pbmp_t cpu_ports;
        int port_i, nof_cpu_ports;
        int header_type = BCM_SWITCH_PORT_HEADER_TYPE_NONE;

        gport_info->flags |= DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT;
        /*
         * Iterate over all CPU ports and find the first one with header type out = CPU.
         * If no such port return the firt CPU port
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get(unit, BCM_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_CPU, 0, &cpu_ports));
        BCM_PBMP_COUNT(cpu_ports, nof_cpu_ports);
        if (nof_cpu_ports < 1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "No CPU port was found in the system.\r\n");
        }
        BCM_PBMP_ITER(cpu_ports, port_i)
        {
            /*
             * Get first CPU port with header type out = CPU
             */
            SHR_IF_ERR_EXIT(dnx_switch_header_type_get
                            (unit, port_i, DNX_SWITCH_PORT_HEADER_TYPE_INDEX_OUT, &header_type));
            if (header_type == BCM_SWITCH_PORT_HEADER_TYPE_CPU)
            {
                gport_info->local_port = port_i;
                break;
            }
        }
        if (header_type != BCM_SWITCH_PORT_HEADER_TYPE_CPU)
        {
            /*
             * Get first CPU port regardless of header type out
             */
            BCM_PBMP_ITER(cpu_ports, port_i)
            {
                gport_info->local_port = port_i;
                break;
            }
        }

        /*
         * get physical system port
         */
        if (operations & DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT)
        {
            SHR_IF_ERR_EXIT(dnx_stk_sys_logical_port_to_sysport_map_get
                            (unit, gport_info->local_port, &gport_info->sys_port));
        }
        /*
         * get core and pp port
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_pp_port_get
                        (unit, gport_info->local_port, &core_id, (uint32 *) &internal_port_pp));
        DNX_ALGO_GPM_GPORT_PHY_PP_PORT_INFO_SET_SINGLE(gport_info->internal_port_pp_info, core_id, internal_port_pp);

    }
    else if (BCM_PHY_GPORT_IS_PHYN(port))
    {
        gport_info->local_port = BCM_PHY_GPORT_PHYN_PORT_PORT_GET(port);
        gport_info->flags |= DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT;
    }
    else if (BCM_PHY_GPORT_IS_PHYN_LANE(port))
    {
        gport_info->local_port = BCM_PHY_GPORT_PHYN_LANE_PORT_PORT_GET(port);
        gport_info->flags |= DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT;
    }
    else if (BCM_PHY_GPORT_IS_PHYN_SYS_SIDE(port))
    {
        gport_info->local_port = BCM_PHY_GPORT_PHYN_SYS_SIDE_PORT_PORT_GET(port);
        gport_info->flags |= DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT;
        gport_info->flags |= DNX_ALGO_GPM_GPORT_INFO_F_IS_SYS_SIDE;
    }
    else if (BCM_PHY_GPORT_IS_PHYN_SYS_SIDE_LANE(port))
    {
        gport_info->local_port = BCM_PHY_GPORT_PHYN_SYS_SIDE_LANE_PORT_PORT_GET(port);
        gport_info->flags |= DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT;
        gport_info->flags |= DNX_ALGO_GPM_GPORT_INFO_F_IS_SYS_SIDE;
    }
    else if (BCM_PHY_GPORT_IS_LANE(gport))
    {
        gport_info->local_port = BCM_PHY_GPORT_LANE_PORT_PORT_GET(port);
        gport_info->lane = BCM_PHY_GPORT_LANE_PORT_LANE_GET(port);
        gport_info->flags |= DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT;
    }
    else if (BCM_GPORT_IS_LOCAL_FABRIC(gport))
    {
        gport_info->local_port = BCM_GPORT_LOCAL_FABRIC_GET(gport) + dnx_data_port.general.fabric_port_base_get(unit);
        gport_info->flags |= DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT;
    }
    else if (BCM_GPORT_IS_BLACK_HOLE(gport))
    {
        gport_info->flags |= DNX_ALGO_GPM_GPORT_INFO_F_IS_BLACK_HOLE;
    }
    /*
     * Return Error, as port is not physical
     */
    else
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "GPORT 0x%x is not physical, and thus cannot be an input for the API.\r\n", gport);
    }
    /*
     * Return Error if local port was required but not retrieved
     */
    if (_SHR_IS_FLAG_SET(operations, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY)
        && !_SHR_IS_FLAG_SET(gport_info->flags, DNX_ALGO_GPM_GPORT_INFO_F_IS_LOCAL_PORT))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "given gport: 0x%x is not resolved to local port.\r\n", gport);
    }
    /*
     * Return Error if pp port was required but not retrieved
     */
    if (_SHR_IS_FLAG_SET(operations, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY) &&
        (gport_info->internal_port_pp_info.nof_pp_ports < 1))
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "given gport: 0x%x is not resolved to pp port.\r\n", gport);
    }

    /*
     * In case the GPORT is not LAG, set the port bitmap to with the only valid local port
     */
    if (!BCM_GPORT_IS_TRUNK(gport) && (gport_info->local_port != SOC_MAX_NUM_PORTS)
        && (gport_info->local_port != _SHR_PORT_INVALID))
    {
        BCM_PBMP_PORT_ADD(gport_info->local_port_bmp, gport_info->local_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h file
 * Note: PON is not supported.
 */
shr_error_e
dnx_algo_gpm_gport_is_physical(
    int unit,
    bcm_gport_t port,
    uint8 *is_physical_port)
{

    bcm_gport_t gport;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Verify the pointer 'gport_info' is not NULL and exit with error if it is.
     */
    SHR_NULL_CHECK(is_physical_port, _SHR_E_PARAM, "is_physical_port");

    /*
     * If GPORT is invalid EXIT
     */
    if (port == BCM_GPORT_INVALID)
    {
        SHR_EXIT();
    }

    /*
     * Check if gport is actually local port that is not encoded as GPORT
     */
    if (BCM_GPORT_IS_SET(port))
    {
        gport = port;
    }
    else if (dnx_algo_port_valid_verify(unit, port) == _SHR_E_NONE)
    {
        BCM_GPORT_LOCAL_SET(gport, port);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PORT, "PORT %d is not valid.\r\n", port);
    }

    /*
     * GPORT is physical in case it's not one of the following types:
     * VPAN PORT / MPLS PORT / MIM PORT / TRILL PORT / TUNNEL / L2GRE PORT / VXLAN PORT / FORWARD PORT / EXTENDER PORT
     */
    if (BCM_GPORT_IS_VLAN_PORT(gport) || BCM_GPORT_IS_MPLS_PORT(gport) || BCM_GPORT_IS_MIM_PORT(gport) ||
        BCM_GPORT_IS_TRILL_PORT(gport) || BCM_GPORT_IS_TUNNEL(gport) || BCM_GPORT_IS_L2GRE_PORT(gport) ||
        BCM_GPORT_IS_VXLAN_PORT(gport) || BCM_GPORT_IS_FORWARD_PORT(gport) || BCM_GPORT_IS_EXTENDER_PORT(gport))
    {

        *is_physical_port = 0;
    }
    else
    {
        *is_physical_port = 1;
    }

    SHR_EXIT();

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnx_algo_gpm_gport_from_system_port(
    int unit,
    int system_port,
    bcm_gport_t * gport)
{
    int is_spa;
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(dnx_trunk_system_port_is_spa(unit, system_port, &is_spa));

    if (is_spa)
    {
        int dummy;
        SHR_IF_ERR_EXIT(dnx_trunk_spa_to_gport(unit, system_port, &dummy, gport));
    }
    else
    {
        BCM_GPORT_SYSTEM_PORT_ID_SET(*gport, system_port);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Given a gport, returns related global HW resources -
 *   Global-LIF or FEC. Both ingress and egress gports are
 *   returned.
 *
 * \param [in] unit -
 *   Identifier of HW platform.
 * \param [in] gport -
 *   In GPORT given by user
 * \param [in] global_in_lif_id -
 *   Pointer to global_in_lif_id. This procedure loads pointed memory
 *   by Global In Lif, DNX_ALGO_GPM_LIF_INVALID if invalid
 * \param [in] global_out_lif_id -
 *   Pointer to global_out_lif_id. This procedure loads pointed memory
 *   by Global Out Lif, DNX_ALGO_GPM_LIF_INVALID if invalid
 * \param [in] fec_id -
 *   Pointer to fec_id. This procedure loads pointed memory
 *   by FEC id, DNX_ALGO_GPM_FEC_INVALID if invalid
 * \param [out] egr_pointed_p -
 *   Pointer to dnx_egr_pointed_t. This procedure loads pointed memory
 *   by the value of 'egr_pointed' id as derived from 'gport'.
 *   If gport is not encoded as 'EGRESS_POINTED', loaded value is
 *   DNX_ALGO_EGR_POINTED_INVALID.
 *   See BCM_L3_ITF_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED
 * \return
 *   \retval Zero - On success
 *   \retval Error - Identifier as per shr_error_e
 * \remark
 *   Current implementation assumes that only one of the two -
 *   global_in/out_lif_id or fec_id can be returned but not
 *   both. In case both are needed, it will require more
 *   implementation.
 */
static shr_error_e
dnx_algo_gpm_gport_to_global_resources(
    int unit,
    bcm_gport_t gport,
    int *global_in_lif_id,
    int *global_out_lif_id,
    int *fec_id,
    dnx_egr_pointed_t * egr_pointed_p)
{
    int gport_internal_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     *  Validity - if port is not valid or not set then nothing to do
     */
    if ((gport == BCM_GPORT_INVALID) || !BCM_GPORT_IS_SET(gport))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "gport given is invalid when trying to map gport to lif, gport (0x%x)\n", gport);
    }

    *global_in_lif_id = DNX_ALGO_GPM_LIF_INVALID;
    *global_out_lif_id = DNX_ALGO_GPM_LIF_INVALID;
    *fec_id = DNX_ALGO_GPM_FEC_INVALID;
    *egr_pointed_p = DNX_ALGO_EGR_POINTED_INVALID;
    /*
     *  1. In these cases we can extract the HW resources directly from 'gport' encoding
     */

    /*
     * Use gport_internal_id in case GPORT has GPORT SUBTYPE, for example GPORT of type VLAN_PORT or MPLS PORT.
     * In that case we need to check GPORT SUBTYPE to understand which resources are relevant
     */
    gport_internal_id = BCM_GPORT_INVALID;

    /*
     *  Go through all possible GPORT types
     */
    if (BCM_GPORT_IS_VLAN_PORT(gport))
    {
        /*
         * VLAN PORT - store internal id, check SUB_TYPE later on
         */
        gport_internal_id = BCM_GPORT_VLAN_PORT_ID_GET(gport);
    }
    else if (BCM_GPORT_IS_MPLS_PORT(gport))
    {
        /*
         * MPLS PORT - store internal id, check SUB_TYPE later on
         */
        gport_internal_id = BCM_GPORT_MPLS_PORT_ID_GET(gport);
    }
    else if (BCM_GPORT_IS_MIM_PORT(gport))
    {
        /*
         * MIM PORT - store FEC, no valid LIF
         */
        *fec_id = BCM_GPORT_MIM_PORT_ID_GET(gport);
    }
    else if (BCM_GPORT_IS_EXTENDER_PORT(gport))
    {
        /*
         * EXTENDER PORT -  store internal id, check SUB_TYPE later on
         */
        gport_internal_id = BCM_GPORT_EXTENDER_PORT_ID_GET(gport);
    }
    else if (BCM_GPORT_IS_TUNNEL(gport))
    {
        /*
         * IP-tunnel / MPLS tunnel - in/out-LIF ID, no valid FEC
         */
        if (!(BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(gport)))
        {
            *global_in_lif_id = *global_out_lif_id = BCM_GPORT_TUNNEL_ID_GET(gport);
            /** If the value is in RIF range than return invalid outlif */
            if (*global_out_lif_id < dnx_data_l3.rif.nof_rifs_get(unit))
            {
                *global_out_lif_id = DNX_ALGO_GPM_LIF_INVALID;
            }
        }
        else
        {
            *egr_pointed_p = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_GET(gport);
        }
    }
    else if (BCM_GPORT_IS_L2GRE_PORT(gport))
    {
        /*
         * L2 GRE PORT - in-LIF ID, no valid FEC or out lif
         */
        *global_in_lif_id = BCM_GPORT_L2GRE_PORT_ID_GET(gport);
    }
    else if (BCM_GPORT_IS_VXLAN_PORT(gport))
    {
        /*
         * VXLAN PORT - in-LIF ID, no valid FEC or out lif
         */
        *global_in_lif_id = BCM_GPORT_VXLAN_PORT_ID_GET(gport);
    }
    else if (BCM_GPORT_IS_FORWARD_PORT(gport))
    {
        /*
         * FORWARD PORT - only FEC is valid
         */
        *fec_id = BCM_GPORT_FORWARD_PORT_GET(gport);
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown GPORT type, gport (0x%x)\n", gport);
    }

    /*
     * Check subtype if relevant
     */
    if (gport_internal_id != BCM_GPORT_INVALID)
    {
        /*
         * For example in case of VLAN_PORT and MPLS_PORT
         */
        if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(gport))
        {
            *global_out_lif_id = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_GET(gport);
        }
        else if (BCM_GPORT_SUB_TYPE_IS_LIF(gport))
        {
            /*
             * LIF - can be either ingress only/egress only or both
             * Get from subtype encoding of the gport
             */
            if ((BCM_GPORT_SUB_TYPE_LIF_EXC_GET(gport_internal_id) == BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY))
            {
                *global_in_lif_id = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(gport_internal_id);
                *global_out_lif_id = DNX_ALGO_GPM_LIF_INVALID;
            }
            else if ((BCM_GPORT_SUB_TYPE_LIF_EXC_GET(gport_internal_id) == BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY))
            {
                *global_out_lif_id = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(gport_internal_id);
                *global_in_lif_id = DNX_ALGO_GPM_LIF_INVALID;
            }
            else
            {
                *global_in_lif_id = *global_out_lif_id = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(gport_internal_id);
            }
        }
        else if (BCM_GPORT_SUB_TYPE_IS_L3_VLAN_TRANSLATION(gport))
        {
            *global_out_lif_id = BCM_GPORT_SUB_TYPE_L3_VLAN_TRANSLATION_GET(gport_internal_id);
        }
        else if (!BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NATIVE(gport))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported GPORT SUBTYPE in gport (0x%x), "
                         "supported are BCM_GPORT_SUB_TYPE_VLAN_TRANSLATION, BCM_GPORT_SUB_TYPE_VIRTUAL "
                         "and BCM_GPORT_SUB_TYPE_LIF\n", gport);
        }
    }

    /*
     * 2. These resources we can get only from reading SW state
     * Currently nothing to do here - might be required in the future
     */

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Retrieve the information of an ingress Virtual Native AC from SW DBs
 * \param [in] unit - Unit #
 * \param [in] gport - ingress Virtual type GPort
 * \param [out] local_in_lif - retrieved local inlif
 * \return shr_error_e Standard error handeling
 */
static shr_error_e
dnx_vlan_port_ingress_virtual_native_ac_local_lif_get(
    int unit,
    bcm_gport_t gport,
    uint32 *local_in_lif)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get the local in-lif from the dedicated SW DB
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_GPORT_TO_LOCAL_IN_LIF_SW, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GPORT, gport);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_LOCAL_IN_LIF, INST_SINGLE, local_in_lif));
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function sets ESEM table handler with the access keys.
 * \param [in] unit -
 * \param [in] sw_table_handle_id - the sw table entry containing the match-info.
 * \param [in] entry_handle_id -ESEM table handler to be used to update the access keys.
 * \return shr_error_e Standard error handling
 */
static shr_error_e
algo_gpm_gport_egress_hw_key_set_virtual(
    int unit,
    uint32 sw_table_handle_id,
    uint32 entry_handle_id)
{
    uint32 port, vsi, vlan_domain, c_vid;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     *  Retrieve VSI and port from ESEM SW DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_FODO_VSI, INST_SINGLE, &vsi));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &port));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &c_vid));

    /*
     * Get port's vlan_domain
     */
    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));
    /*
     * Update the EGRESS ESEM handler with the retrieved keys:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, c_vid);
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief -
 * This function sets ESEM Namespace-vsi table handler with the access keys.
 * \param [in] unit -
 * \param [in] sw_table_handle_id - the sw table entry containing all info
 * \param [out] entry_handle_id -ESEM table handler updated with the access keys
 * \return shr_error_e Standard error handling
 */
static shr_error_e
algo_gpm_gport_egress_hw_key_set_virtual_namespace_vsi(
    int unit,
    uint32 sw_table_handle_id,
    uint32 entry_handle_id)
{
    uint32 tmp32;
    bcm_vlan_t vsi;
    uint32 match_class_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     *  Retrieve VSI and vlan_domain from ESEM SW DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_FODO_VSI, INST_SINGLE, &tmp32));
    vsi = (bcm_vlan_t) tmp32;
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_NETWORK_DOMAIN, INST_SINGLE, &match_class_id));
    /*
     * Update the EGRESS ESEM handler with the retrieved keys (if necessary):
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, match_class_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VSI, vsi);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_C_VID, 0);

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 * This function sets ESEM Namespace-port table handler with the access keys.
 * \param [in] unit -
 * \param [in] sw_table_handle_id - the sw table entry containing all info
 * \param [out] entry_handle_id -ESEM table handler updated with the access keys
 * \return shr_error_e Standard error handling
 */
static shr_error_e
algo_gpm_gport_egress_hw_key_set_virtual_namespace_port(
    int unit,
    uint32 sw_table_handle_id,
    uint32 entry_handle_id)
{
    uint32 port, vlan_domain;
    dnx_algo_gpm_gport_phy_info_t gport_phy_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    /*
     *  Retrieve system_port and vlan_domain from ESEM SW DB
     */
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &port));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, sw_table_handle_id, DBAL_FIELD_NAMESPACE, INST_SINGLE, &vlan_domain));

    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_phy_info));

    /*
     * Update the EGRESS ESEM handler with the retrieved keys:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, vlan_domain);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT_AGGREGATE, gport_phy_info.sys_port);
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief -
 * This function sets ESEM Namespace-port table handler with the access keys.
 * \param [in] unit -
 * \param [in] gport - the gport given by the user
 * \param [out] entry_handle_id -ESEM table handler updated with the access keys
 * \return shr_error_e Standard error handling
 */
static shr_error_e
algo_gpm_gport_egress_hw_key_set_virtual_egress_default(
    int unit,
    bcm_gport_t gport,
    uint32 entry_handle_id)
{
    int esem_default_result_profile;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Retrieve esem_default_result_profile from vlan_port_id
     */
    esem_default_result_profile = BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(gport);
    /*
     * Check esem_default_result_profile
 */
    if ((esem_default_result_profile < 0)
        || (esem_default_result_profile >= dnx_data_esem.default_result_profile.nof_profiles_get(unit)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "gport (0x%08X) is not a valid virtual gport for esem default entry!\n", gport);
    }

    /*
     * Configure ESEM default table key
     */
    dbal_entry_key_field16_set(unit, entry_handle_id, DBAL_FIELD_ESEM_DEFAULT_RESULT_PROFILE,
                               esem_default_result_profile);

exit:
    SHR_FUNC_EXIT;
}
/*
 * See .h file
 */
shr_error_e
algo_gpm_gport_egress_hw_key_set(
    int unit,
    uint32 entry_handle_id,
    bcm_gport_t gport,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources)
{
    uint8 is_virtual_gport = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(gport_hw_resources, _SHR_E_PARAM, "gport_hw_resources");

    if (gport_hw_resources->local_out_lif != DNX_ALGO_GPM_LIF_INVALID)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, gport_hw_resources->local_out_lif);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     gport_hw_resources->outlif_dbal_result_type);
    }
    else if (BCM_GPORT_IS_VLAN_PORT(gport)
             && (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(gport)
                 || BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(gport)))
    {
        uint32 res_type;
        uint32 sw_table_handle_id;
        /*
         * Set result type using gport hw resources
         */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     gport_hw_resources->outlif_dbal_result_type);
        /*
         * Retrieve outlif and VSI from sw state
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &sw_table_handle_id));
        /*
         * Access SW DB
         */
        dbal_entry_key_field32_set(unit, sw_table_handle_id, DBAL_FIELD_GPORT, gport);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, sw_table_handle_id, DBAL_GET_ALL_FIELDS));
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, sw_table_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &res_type));
        
        switch (res_type)
        {
            case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NATIVE_AC:
            {
                /*
                 * set ESEM keys for virtual native
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_egress_virtual_native_key_get
                                (unit, sw_table_handle_id, entry_handle_id, NULL));
                break;
            }
            case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_AC:
            {
                /*
                 * set ESEM keys for out AC
                 */
                SHR_IF_ERR_EXIT(algo_gpm_gport_egress_hw_key_set_virtual(unit, sw_table_handle_id, entry_handle_id));
                break;
            }
            case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_VSI:
            {
                /*
                 * set ESEM keys for namespace VSI
                 */
                SHR_IF_ERR_EXIT(algo_gpm_gport_egress_hw_key_set_virtual_namespace_vsi
                                (unit, sw_table_handle_id, entry_handle_id));

                break;
            }
            case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_PORT:
            {
                /*
                 * set ESEM keys for namespace port
                 */
                SHR_IF_ERR_EXIT(algo_gpm_gport_egress_hw_key_set_virtual_namespace_port
                                (unit, sw_table_handle_id, entry_handle_id));
                break;
            }
            case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_DEFAULT:
            {
                /*
                 * set ESEM keys for default
                 */
                SHR_IF_ERR_EXIT(algo_gpm_gport_egress_hw_key_set_virtual_egress_default(unit, gport, entry_handle_id));
                break;
            }
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error!, accessing ESM SW DB table (%d) with the key gport = 0x%08X but"
                             " resultType = %d is not supported.\n",
                             DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, gport, res_type);
        }
        is_virtual_gport = TRUE;
    }

    if ((gport_hw_resources->local_out_lif == DNX_ALGO_GPM_LIF_INVALID) && (is_virtual_gport == FALSE))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "hw resources do not hold enough information to set key for gport (0x%08X)\n",
                     gport);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Given a gport, returns related virtual egress HW resources - outlif_dbal_table_id.
*   In case of non virtual gport LIF, ignore.
*   Called by dnx_algo_gpm_gport_to_hw_resources in case gport is virtual
*
*  \param [in] unit -
*    Relevant unit.
*  \param [in] gport -
*    GPORT given by user
*  \param [in] flags -
*     flags of options what to retrieve. Currently not in use
*  \param [out] gport_hw_resources -
*    GPORT HW resources. only outlif_dbal_table_id will be filled
*  \return
*    shr_error_e -
*      Error return value
*  \remark
*    None
*  \see
*    dnx_algo_gpm_gport_to_hw_resources
 */
static shr_error_e
dnx_algo_gpm_gport_to_hw_virtual_resources(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources)
{

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(gport_hw_resources, _SHR_E_PARAM, "gport_hw_resources");

    if (BCM_GPORT_IS_VLAN_PORT(gport))
    {
        if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(gport))
        {
            uint32 sw_table_handle_id;
            uint32 entry_type;
            uint32 is_native_intf_name_space;

            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC
                            (unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &sw_table_handle_id));

            dbal_entry_key_field32_set(unit, sw_table_handle_id, DBAL_FIELD_GPORT, gport);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, sw_table_handle_id, DBAL_GET_ALL_FIELDS));

            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, sw_table_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &entry_type));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, sw_table_handle_id, DBAL_FIELD_HW_RESULT_TYPE, INST_SINGLE,
                             &gport_hw_resources->outlif_dbal_result_type));
            switch (entry_type)
            {
                case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NATIVE_AC:
                {
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, sw_table_handle_id, DBAL_FIELD_IS_LIF_SCOPE, INST_SINGLE,
                                     &is_native_intf_name_space));
                    if (is_native_intf_name_space)
                    {
                        gport_hw_resources->outlif_dbal_table_id =
                            DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB;
                    }
                    else
                    {
                        gport_hw_resources->outlif_dbal_table_id = DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB;
                    }
                    break;
                }
                case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_AC:
                {
                    gport_hw_resources->outlif_dbal_table_id = DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB;
                    break;
                }
                case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_VSI:
                {
                    gport_hw_resources->outlif_dbal_table_id = DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB;
                    break;
                }
                case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_PORT:
                {
                    gport_hw_resources->outlif_dbal_table_id = DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB;
                    break;
                }
                default:
                    SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                 "Error!, accessing ESM SW DB table (%d) with the key gport = 0x%08X but"
                                 " resultType = %d is not supported.\n",
                                 DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, gport, entry_type);
            }
        }
        else if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(gport))
        {
            gport_hw_resources->outlif_dbal_table_id = DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE;
            gport_hw_resources->outlif_dbal_result_type = DBAL_RESULT_TYPE_ESEM_DEFAULT_RESULT_TABLE_ETPS_AC;
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
*   Verify gport_to_hw_resources was set with valid values according to flags
*   called by dnx_algo_gpm_gport_to_hw_resources
*
*  \param [in] unit -
*    Relevant unit.
*  \param [in] gport -
*    GPORT given by user
*  \param [in] flags -
*     flags of options what to retrieve. Currently not in use
*  \param [in] gport_hw_resources -
*    GPORT HW resources. only outlif_dbal_table_id will be filled
*  \return
*    shr_error_e -
*      Error return value
*  \remark
*    None
*  \see
*    dnx_algo_gpm_gport_to_hw_resources
 */
static shr_error_e
dnx_algo_gpm_gport_to_hw_strict_verify(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources)
{

    SHR_FUNC_INIT_VARS(unit);

    /*
     * FEC
     */
    if ((_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_FEC))
        && (gport_hw_resources->fec_id == DNX_ALGO_GPM_FEC_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Requested FEC_ID but it is invalid, gport (0x%x)\n", gport);
    }
    /*
     * Ingress Global LIF
     */
    if ((_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS)) &&
        (gport_hw_resources->global_in_lif == DNX_ALGO_GPM_LIF_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Requested GLOBAL_LIF_INGRESS but it is invalid, gport (0x%x)\n", gport);
    }
    /*
     * Egress Global LIF
     */
    if ((_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS)) &&
        (gport_hw_resources->global_out_lif == DNX_ALGO_GPM_LIF_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Requested GLOBAL_LIF_EGRESS but it is invalid, gport (0x%x)\n", gport);
    }
    /*
     * Virtual Egress Pointed (egr_pointed)
     */
    if ((BCM_GPORT_IS_TUNNEL(gport) && BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(gport)) &&
        (gport_hw_resources->local_out_lif == DNX_ALGO_GPM_LIF_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Requested EGR_POINTED but it is invalid, gport (0x%08X)\n", gport);
    }
    /*
     * Ingress Local LIF
     */
    if ((_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS)) &&
        (gport_hw_resources->local_in_lif == DNX_ALGO_GPM_LIF_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Requested LOCAL_LIF_INGRESS but it is invalid, gport (0x%x)\n", gport);
    }
    /*
     * Egress Local LIF
     */
    if ((_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS)) &&
        !(_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_VIRTUAL_EGRESS)) &&
        (gport_hw_resources->local_out_lif == DNX_ALGO_GPM_LIF_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Requested LOCAL_LIF_EGRESS but it is invalid, gport (0x%x)\n", gport);
    }
    /*
     * Egress Virtual
     */
    if (!(_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS)) &&
        (_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_VIRTUAL_EGRESS)) &&
        (gport_hw_resources->outlif_dbal_table_id == DBAL_NOF_TABLES))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Requested VIRTUAL_EGRESS but it is invalid, gport (0x%x)\n", gport);
    }
    /*
     * Egress Virtual / LIF
     */
    if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_EGRESS_LOCAL_LIF_OR_VIRTUAL) &&
        (gport_hw_resources->outlif_dbal_table_id == DBAL_NOF_TABLES) &&
        (gport_hw_resources->local_out_lif == DNX_ALGO_GPM_LIF_INVALID))
    {
        SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Requested LOCAL_LIF_OR_VIRTUAL but it is invalid, gport (0x%x)\n", gport);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 *  See algo_gpm.h for function description.
 */
shr_error_e
dnx_algo_gpm_gport_to_hw_resources(
    int unit,
    bcm_gport_t gport,
    uint32 flags,
    dnx_algo_gpm_gport_hw_resources_t * gport_hw_resources)
{
    int *global_in_lif = NULL;
    int *global_out_lif = NULL;
    dnx_egr_pointed_t egr_pointed_id = DNX_ALGO_EGR_POINTED_INVALID;
    int *fec_id = NULL;
    int *local_in_lif = NULL, *local_in_lif_core = NULL;
    int *local_out_lif = NULL;
    int global_in_lif_dummy = DNX_ALGO_GPM_LIF_INVALID;
    int global_out_lif_dummy = DNX_ALGO_GPM_LIF_INVALID;
    int fec_dummy = DNX_ALGO_GPM_FEC_INVALID;
    lif_mapping_local_lif_info_t local_lif_info;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(gport_hw_resources, _SHR_E_PARAM, "gport_hw_resources");

    /*
     * Verify that at least one flag is set.
     */
    if ((flags & (DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF | DNX_ALGO_GPM_GPORT_HW_RESOURCES_FEC)) == 0)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "No flags requested, gport (0x%x)\n", gport);
    }
    /*
     * Verify that caller is not requesting both FEC and LIF.
     * This is not supported by current implementation.
     */
    if (((_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF)) ||
         (_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF))) &&
        (_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_FEC)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "FEC and LIF resources cannot be requested in the same time, gport (0x%x)\n", gport);
    }
    /*
     * Verify that caller is not requesting both EGR_POINTED and GLOBAL_LIF.
     * This is not legal on current implementation.
     */
    if ((_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS)) &&
        (BCM_GPORT_IS_TUNNEL(gport) && BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_POINTED(gport)))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "gport is EGR_POINTED, cannot return GLOBAL_LIF.\r\n"
                     "gport (0x%08X) flags (0x%08X)\r\n", gport, flags);
    }
    /*
     * Init values
     */
    gport_hw_resources->fec_id = DNX_ALGO_GPM_FEC_INVALID;
    gport_hw_resources->global_in_lif = DNX_ALGO_GPM_LIF_INVALID;
    gport_hw_resources->global_out_lif = DNX_ALGO_GPM_LIF_INVALID;
    gport_hw_resources->local_in_lif = DNX_ALGO_GPM_LIF_INVALID;
    gport_hw_resources->local_in_lif_core = _SHR_CORE_ALL;
    gport_hw_resources->local_out_lif = DNX_ALGO_GPM_LIF_INVALID;
    gport_hw_resources->outlif_dbal_table_id = DBAL_NOF_TABLES;
    gport_hw_resources->outlif_dbal_result_type = DNX_ALGO_GPM_DBAL_RESULT_TYPE_INVALID;
    gport_hw_resources->inlif_dbal_table_id = DBAL_NOF_TABLES;
    gport_hw_resources->inlif_dbal_result_type = DNX_ALGO_GPM_DBAL_RESULT_TYPE_INVALID;
    if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS))
    {
        global_in_lif = &(gport_hw_resources->global_in_lif);
    }
    if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS))
    {
        global_out_lif = &(gport_hw_resources->global_out_lif);
    }
    if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_INGRESS))
    {
        local_in_lif = &(gport_hw_resources->local_in_lif);
        local_in_lif_core = &(gport_hw_resources->local_in_lif_core);
    }
    if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS))
    {
        local_out_lif = &(gport_hw_resources->local_out_lif);
    }
    if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_FEC))
    {
        fec_id = &(gport_hw_resources->fec_id);
    }

    if (BCM_GPORT_IS_VLAN_PORT(gport)
        && (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(gport) || BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(gport)))
    {
        /*
         * Expecting virtual
         */
        if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_VIRTUAL_EGRESS))
        {
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_virtual_resources(unit, gport, flags, gport_hw_resources));
        }
    }
    else
    {
        /*
         * First, retrieve GLOBAL LIFs and FEC
         * Also, retrieve 'egr_pointed' object id
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_global_resources
                        (unit, gport, &global_in_lif_dummy, &global_out_lif_dummy, &fec_dummy, &egr_pointed_id));
        SHR_IF_NOT_NULL_FILL(global_in_lif, global_in_lif_dummy);
        SHR_IF_NOT_NULL_FILL(global_out_lif, global_out_lif_dummy);
        SHR_IF_NOT_NULL_FILL(fec_id, fec_dummy);
        /*
         * At this point, 'egr_pointed_dummy' has a valid value only if 'gport' is encoded
         * as 'egr_pointed' under 'tunnel'
         */
        /*
         * Second, if LOCAL LIFs required, get them
         */
        if (local_in_lif)
        {
            if (BCM_GPORT_IS_VLAN_PORT(gport) && BCM_GPORT_SUB_TYPE_IS_VIRTUAL_INGRESS_NATIVE(gport))
            {
                /*
                 * In case of VIRTUAL GPORT, there is no global lif but local lif can be retrieved from SW state.
                 */
                SHR_IF_ERR_EXIT(dnx_vlan_port_ingress_virtual_native_ac_local_lif_get
                                (unit, gport, (uint32 *) local_in_lif));
            /** Virtual native vlan lifs always use INLIF-2. */
                SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get
                                (unit, *local_in_lif, _SHR_CORE_ALL, DBAL_PHYSICAL_TABLE_INLIF_2,
                                 &gport_hw_resources->inlif_dbal_table_id,
                                 &gport_hw_resources->inlif_dbal_result_type, NULL));
            }
            else if (global_in_lif_dummy != DNX_ALGO_GPM_LIF_INVALID)
            {
                /*
                 * Get local lif. Error will be returned only in case of STRICT flag
                 */
                SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_algo_lif_mapping_global_to_local_get
                                          (unit, DNX_ALGO_LIF_INGRESS, global_in_lif_dummy, &local_lif_info),
                                          _SHR_E_NOT_FOUND);

                *local_in_lif = local_lif_info.local_lif;

                if (*local_in_lif != DNX_ALGO_GPM_LIF_INVALID)
                {
                    *local_in_lif_core = _SHR_CORE_ALL;
                    SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get
                                    (unit, *local_in_lif, *local_in_lif_core, local_lif_info.phy_table,
                                     &gport_hw_resources->inlif_dbal_table_id,
                                     &gport_hw_resources->inlif_dbal_result_type, NULL));
                }
            }
            else
            {
                *local_in_lif = DNX_ALGO_GPM_LIF_INVALID;
            }
        }

        if (local_out_lif)
        {
            /*
             * We are counting here on the fact that 'egr_pointed_dummy' and 'global_out_lif_dummy'
             * will not be valid at the same time.
             */
            if (egr_pointed_id != DNX_ALGO_EGR_POINTED_INVALID)
            {
                int egr_pointed_flags;
                /*
                 * call dnx_algo_l3_egr_pointed_to_local_lif_get()
                 * to get local_lif from SW-STATE table (map of 'egress_pointed' to 'local_out_lif').
                 */
                egr_pointed_flags = 0;
                SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_algo_l3_egr_pointed_to_local_lif_get
                                          (unit, egr_pointed_flags, egr_pointed_id, local_out_lif), _SHR_E_NOT_FOUND);
                if (*local_out_lif != DNX_ALGO_GPM_LIF_INVALID)
                {
                    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get(unit, *local_out_lif,
                                                                    &gport_hw_resources->outlif_dbal_table_id,
                                                                    &gport_hw_resources->outlif_dbal_result_type,
                                                                    &gport_hw_resources->outlif_phase, NULL, NULL,
                                                                    NULL));
                }
            }
            else if (global_out_lif_dummy != DNX_ALGO_GPM_LIF_INVALID)
            {
                /*
                 * Get local lif. Error will be returned only in case of STRICT flag
                 */
                SHR_IF_ERR_EXIT_EXCEPT_IF(dnx_algo_lif_mapping_global_to_local_get
                                          (unit, DNX_ALGO_LIF_EGRESS, global_out_lif_dummy, &local_lif_info),
                                          _SHR_E_NOT_FOUND);
                *local_out_lif = local_lif_info.local_lif;
                if (*local_out_lif != DNX_ALGO_GPM_LIF_INVALID)
                {
                    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get(unit, *local_out_lif,
                                                                    &gport_hw_resources->outlif_dbal_table_id,
                                                                    &gport_hw_resources->outlif_dbal_result_type,
                                                                    &gport_hw_resources->outlif_phase, NULL, NULL,
                                                                    NULL));
                }
            }
            else
            {
                *local_out_lif = DNX_ALGO_GPM_LIF_INVALID;
            }
        }
    }

    /*
     * Strict checks - return error in case one of the requested parameters wasn't found
     */
    if (!_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT))
    {
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_strict_verify(unit, gport, flags, gport_hw_resources));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
*  See algo_gpm.h for function description.
*/
shr_error_e
dnx_algo_gpm_rif_intf_to_hw_resources(
    int unit,
    bcm_if_t intf,
    dnx_algo_gpm_rif_hw_resources_t * rif_hw_resources)
{
    lif_mngr_outlif_phase_e outlif_phase_dummy;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(rif_hw_resources, _SHR_E_PARAM, "gport_hw_resources");

    if (!BCM_L3_ITF_TYPE_IS_RIF(intf))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "dnx_algo_gpm_rif_intf_to_hw_resources can be only used for RIFs.  (0x%x) in not a RIF\n", intf);
    }

    SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get(unit, intf,
                                                    &rif_hw_resources->outlif_dbal_table_id,
                                                    &rif_hw_resources->outlif_dbal_result_type,
                                                    &outlif_phase_dummy, NULL, NULL, NULL));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_algo_gpm_gport_from_lif_verify(
    int unit,
    uint32 flags,
    int core_id,
    int lif,
    bcm_gport_t * gport)
{
    uint32 lif_flags;
    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(gport, _SHR_E_PARAM, "gport");

    /*
     * Verify flags and lif id.
     */
    lif_flags = flags & DNX_ALGO_GPM_GPORT_HW_RESOURCE_GPORT_FROM_LIF_SUPPORTED_FLAGS;
    if (utilex_nof_on_bits_in_long(lif_flags) != 1)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "Exactly one flag of "
                     "DNX_ALGO_GPM_GPORT_HW_RESOURCES_[GLOBAL|LOCAL]_LIF_[EGRESS|[DPC_|SBC_]INGRESS] "
                     "must be set. Given: 0x%08x", flags);
    }

    /*
     * Verify lif id.
     */
    if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS))
    {
        LIF_MNGR_GLOBAL_IN_LIF_VERIFY(unit, lif);
    }
    else if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS))
    {
        LIF_MNGR_GLOBAL_OUT_LIF_VERIFY(unit, lif);
    }
    else if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_SBC_LOCAL_LIF_INGRESS))
    {
        LIF_MNGR_LOCAL_SBC_IN_LIF_VERIFY(unit, lif);
    }
    else if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS))
    {
        LIF_MNGR_LOCAL_DPC_IN_LIF_VERIFY(unit, lif);
    }
    else if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS))
    {
        LIF_MNGR_LOCAL_OUT_LIF_VERIFY(unit, lif);
    }

    /*
     * Verify core.
     * Must be a legal/all for DPC inlif, all for everything else.
     */
    if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS))
    {
        DNXCMN_CORE_VALIDATE(unit, core_id, TRUE);
    }
    else
    {
        if (core_id != _SHR_CORE_ALL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Core ID must be _SHR_CORE_ALL for this lif type. Given %d", core_id);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
*  See algo_gpm.h for function description.
*/
shr_error_e
dnx_algo_gpm_gport_from_lif(
    int unit,
    uint32 flags,
    int core_id,
    int lif,
    bcm_gport_t * gport)
{
    dbal_tables_e dbal_table_id;
    uint32 dbal_result_type;
    int global_lif, local_lif;
    int gport_id_dummy;
    uint8 is_symmetric;
    /*
     * The variable 'is_egress_pointed' is loaded by a non-zero value
     * if it is found that the required 'gport' should b encoded as
     * 'virtual_egress_pointed'. This happens when input 'local_lif'
     * is not found on 'local_lif->global_lif' table but on
     * 'local_lif->egr_pointed' table.
     */
    int is_egress_pointed;
    dnx_egr_pointed_t egr_pointed_id;
    int is_ingress, is_egress, is_local, is_global;
    lif_mapping_local_lif_info_t local_lif_info;
    dbal_physical_tables_e dbal_physical_table;
    shr_error_e shr_error = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);
    is_egress_pointed = FALSE;
    /*
     * Not required. Just silence compiler complaint.
     */
    egr_pointed_id = 0;
    SHR_INVOKE_VERIFY_DNX(dnx_algo_gpm_gport_from_lif_verify(unit, flags, core_id, lif, gport));

    sal_memset(&local_lif_info, 0, sizeof(local_lif_info));

    /*
     * Set flags.
     */
    is_ingress = _SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_INGRESS_FLAGS);

    is_egress = _SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_EGRESS_FLAGS);

    is_local = _SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_FLAGS);

    is_global = _SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_FLAGS);

    /*
     * If lif is global lif, then translate it to local lif.
     */
    if (is_global)
    {
        uint32 direction_flags = is_egress ? DNX_ALGO_LIF_EGRESS : DNX_ALGO_LIF_INGRESS;

        global_lif = lif;

        shr_error = dnx_algo_lif_mapping_global_to_local_get(unit, direction_flags, global_lif, &local_lif_info);
        SHR_IF_ERR_EXIT_EXCEPT_IF(shr_error, _SHR_E_NOT_FOUND);
        if ((shr_error == _SHR_E_NOT_FOUND) && (!_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT)))
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Global Lif (0x%x) couldn't be resolved to local lif\n", global_lif);
        }

        local_lif = local_lif_info.local_lif;
    }
    else
    {
        /*
         * Otherwise, the lif is a local lif.
         */
        local_lif = lif;
    }

    /*
     * Set dbal_physical_table and core_id
     */
    if (is_ingress)
    {
        if (_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS))
        {
            dbal_physical_table = DBAL_PHYSICAL_TABLE_INLIF_2;
        }
        else if ((_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_SBC_LOCAL_LIF_INGRESS)))
        {
            dbal_physical_table = DBAL_PHYSICAL_TABLE_INLIF_1;
        }
        else
        {
            /*
             * Lif is global, the physical table and core are in the lif info.
             */
            dbal_physical_table = local_lif_info.phy_table;
        }
    }
    else
    {
        dbal_physical_table = 0;        /* Don't care. */
    }

    /*
     * If lif is local, then get the global lif.
     */
    if (is_local)
    {
        uint32 mapping_flags = (is_ingress) ? DNX_ALGO_LIF_INGRESS : DNX_ALGO_LIF_EGRESS;
        local_lif_info.local_lif = local_lif;
        local_lif_info.phy_table = dbal_physical_table;
        if (is_ingress)
        {
            shr_error = dnx_algo_lif_mapping_local_to_global_get(unit, mapping_flags, &local_lif_info, &global_lif);
            SHR_IF_ERR_EXIT_EXCEPT_IF(shr_error, _SHR_E_NOT_FOUND);
            if ((shr_error == _SHR_E_NOT_FOUND)
                && (!_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT)))
            {
                SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Global Lif (0x%x) couldn't be resolved to local lif\n", global_lif);
            }
        }
        else
        {
            /*
             * On egress,
             * Look for local_out_lif in one of the two tables:
             *   Map of local_lif to global_lif
             *   Map of local_lif to 'egr_pointed'
             * It can be on one of them only.
             */
            uint32 egr_pointed_flags;
            dnx_egr_pointed_t egr_pointed_id_dummy[1];

            egr_pointed_flags = 0;
            /*
             * Since, somewhere along the line (dbal_entry_handle_value_field32_get()), the last parameter
             * (&egr_pointed_id) is treated as a table, coverity requires that it actually be a table.
             */
            shr_error =
                dnx_algo_l3_local_lif_to_egr_pointed_get(unit, egr_pointed_flags, local_lif, egr_pointed_id_dummy);
            egr_pointed_id = egr_pointed_id_dummy[0];
            SHR_IF_ERR_EXIT_EXCEPT_IF(shr_error, _SHR_E_NOT_FOUND);
            if (shr_error == _SHR_E_NONE)
            {
                /*
                 * 'Local_lif has been found on 'local_lif->egr_pointed' table.
                 * Mark it as 'egress_pointed'
                 */
                is_egress_pointed = TRUE;
            }
            else
            {
                /*
                 * If 'local_lif' is not found on 'local_lif->egr_pointed' table,
                 * try to find it on 'local_lif->egr_pointed' table.
                 * If it is not found there either then eject an error.
                 */
                shr_error = dnx_algo_lif_mapping_local_to_global_get(unit, mapping_flags, &local_lif_info, &global_lif);
                SHR_IF_ERR_EXIT_EXCEPT_IF(shr_error, _SHR_E_NOT_FOUND);
                if ((shr_error == _SHR_E_NOT_FOUND)
                    && (!_SHR_IS_FLAG_SET(flags, DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT)))
                {
                    SHR_ERR_EXIT(_SHR_E_NOT_FOUND, "Local Lif (0x%x) couldn't be resolved to global lif\n", global_lif);
                }
            }
        }
    }

    /*
     * Get the dbal table and result type from lif manager
     */
    if (is_ingress && (shr_error == _SHR_E_NONE))
    {
        SHR_IF_ERR_EXIT(dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get(unit, local_lif, core_id,
                                                                                dbal_physical_table, &dbal_table_id,
                                                                                &dbal_result_type, NULL));
    }
    else if (shr_error == _SHR_E_NONE)
    {
        SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get
                        (unit, local_lif, &dbal_table_id, &dbal_result_type, NULL, NULL, NULL, NULL));
    }

    /*
     * Now convert to gport according to dbal table and result type
     */
    /** Egress */
    if (is_egress && (shr_error == _SHR_E_NONE))
    {
        switch (dbal_table_id)
        {
            case DBAL_TABLE_EEDB_ARP:
            {
                /*
                 * ARP
                 */
                if (dbal_result_type == DBAL_RESULT_TYPE_EEDB_ARP_ETPS_ARP_AC)
                {
                    BCM_GPORT_SUB_TYPE_L3_VLAN_TRANSLATION_SET(gport_id_dummy, global_lif);
                    BCM_GPORT_VLAN_PORT_ID_SET(*gport, gport_id_dummy);
                }
                else
                {
                    if (is_egress_pointed == TRUE)
                    {
                        /*
                         * Encode output gport as 'egress_pointed'
                         */
                        int dummy_gport;

                        BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SET(dummy_gport, egr_pointed_id);
                        BCM_GPORT_TUNNEL_ID_SET(*gport, dummy_gport);
                    }
                    else
                    {
                        BCM_GPORT_TUNNEL_ID_SET(*gport, global_lif);
                    }
                }
                break;
            }
            case DBAL_TABLE_EEDB_OUT_AC:
            {
                if (dbal_result_type == DBAL_RESULT_TYPE_EEDB_OUT_AC_ETPS_AC_TRIPLE_TAG)
                {
                    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id_dummy, 0, global_lif);
                    BCM_GPORT_EXTENDER_PORT_ID_SET(*gport, gport_id_dummy);
                }
                else
                {
                    /*
                     * AC
                     */
                    SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_lif_id_is_symmetric
                                    (unit, global_lif, &is_symmetric));
                    if (!is_symmetric)
                    {
                        BCM_GPORT_SUB_TYPE_LIF_SET(gport_id_dummy, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, global_lif);
                    }
                    else
                    {
                        BCM_GPORT_SUB_TYPE_LIF_SET(gport_id_dummy, 0, global_lif);
                    }
                    BCM_GPORT_VLAN_PORT_ID_SET(*gport, gport_id_dummy);
                }
                break;
            }
            case DBAL_TABLE_EEDB_IPV4_TUNNEL:
            case DBAL_TABLE_EEDB_IPV6_TUNNEL:
            case DBAL_TABLE_EEDB_MPLS_TUNNEL:
            case DBAL_TABLE_EEDB_SRV6:
            case DBAL_TABLE_EEDB_DATA_ENTRY:
            case DBAL_TABLE_EEDB_IPFIX_PSAMP:
            case DBAL_TABLE_EEDB_SFLOW:
            case DBAL_TABLE_EEDB_EVPN:
            case DBAL_TABLE_EEDB_REFLECTOR:
            {
                /*
                 * Tunnel
                 */
                if (is_egress_pointed == TRUE)
                {
                    /*
                     * Encode output gport as 'egress_pointed'
                     */
                    int dummy_gport;

                    BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_POINTED_SET(dummy_gport, egr_pointed_id);
                    BCM_GPORT_TUNNEL_ID_SET(*gport, dummy_gport);
                }
                else
                {
                    BCM_GPORT_TUNNEL_ID_SET(*gport, global_lif);
                }
                break;
            }
            case DBAL_TABLE_EEDB_PWE:
            {
                /*
                 * MPLS PORT
                 */
                BCM_GPORT_SUB_TYPE_LIF_SET(gport_id_dummy, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, global_lif);
                BCM_GPORT_MPLS_PORT_ID_SET(*gport, gport_id_dummy);
                break;
            }
            case DBAL_TABLE_EEDB_ERSPAN:
            {
                /*
                 * ERSPAN Tunnel
                 */
                BCM_GPORT_TUNNEL_ID_SET(*gport, global_lif);
                break;
            }
            case DBAL_TABLE_EEDB_RCH:
            case DBAL_TABLE_EEDB_GTP:
                BCM_GPORT_TUNNEL_ID_SET(*gport, global_lif);
                break;
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown dbal_table_id for egress local lif (0x%x)\n", local_lif);
                break;
            }
        }
    }
    else if (shr_error == _SHR_E_NONE)
    {
        /*
         * Ingress
         */
        switch (dbal_table_id)
        {
            case DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION:
            case DBAL_TABLE_IN_AC_INFO_DB:
            {
                /*
                 * Vlan port
                 */
                SHR_IF_ERR_EXIT(dnx_algo_global_lif_allocation_lif_id_is_symmetric(unit, global_lif, &is_symmetric));
                if (!is_symmetric)
                {
                    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id_dummy, BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY, global_lif);
                }
                else
                {
                    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id_dummy, 0, global_lif);
                }
                BCM_GPORT_VLAN_PORT_ID_SET(*gport, gport_id_dummy);

                /*
                 * If the gport is ac and symmetric, there's a change that it's actually extender port.
                 * Get the outlif result type to know for sure.
                 */
                if (is_symmetric && dbal_table_id == DBAL_TABLE_IN_AC_INFO_DB)
                {
                    dnx_algo_gpm_gport_hw_resources_t hw_res;

                    sal_memset(&hw_res, 0, sizeof(dnx_algo_gpm_gport_hw_resources_t));

                    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, *gport,
                                                                       DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS,
                                                                       &hw_res));

                    if (hw_res.outlif_dbal_result_type == DBAL_RESULT_TYPE_EEDB_OUT_AC_ETPS_AC_TRIPLE_TAG)
                    {
                        BCM_GPORT_SUB_TYPE_LIF_SET(gport_id_dummy, 0, global_lif);
                        BCM_GPORT_EXTENDER_PORT_ID_SET(*gport, gport_id_dummy);
                    }
                }
                break;
            }
            case DBAL_TABLE_IN_LIF_FORMAT_LSP:
            case DBAL_TABLE_IN_LIF_IPvX_TUNNELS:
            case DBAL_TABLE_IN_LIF_FORMAT_EVPN:
            case DBAL_TABLE_IN_LIF_FORMAT_GTP:
            {
                /*
                 * Tunnel
                 */
                BCM_GPORT_TUNNEL_ID_SET(*gport, global_lif);
                break;
            }
            case DBAL_TABLE_IN_LIF_FORMAT_PWE:
            {
                /*
                 * MPLS PORT
                 */
                BCM_GPORT_SUB_TYPE_LIF_SET(gport_id_dummy, BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY, global_lif);
                BCM_GPORT_MPLS_PORT_ID_SET(*gport, gport_id_dummy);
                break;
            }
            case DBAL_TABLE_IN_LIF_FORMAT_SRV6_TERMINATED:
            {
                /*
                 * FIXME SDK-131927: SRv6
 */
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown dbal_table_id for ingress local lif (0x%x)\n", local_lif);
                break;
            }
            case DBAL_TABLE_IN_LIF_FORMAT_BIER:
            {
                
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown dbal_table_id for ingress local lif (0x%x)\n", local_lif);
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown dbal_table_id for ingress local lif (0x%x)\n", local_lif);
                break;
            }
        }

    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * called by algo_gpm_encode_destination_field_from_gport in case destination
 * is egress multicast (has special encoding)
 */
static shr_error_e
algo_gpm_encode_destination_field_from_gport_egress_mc_internal(
    int unit,
    bcm_gport_t port,
    uint32 *destination)
{
    uint32 tm_port, core_id;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    int egress_trunk_id;
    int profile_found;

    dbal_fields_e egress_destination_field[] = { DBAL_FIELD_EGRESS_DESTINATION, DBAL_FIELD_EGRESS_DESTINATION_EXT };
    dbal_fields_e dsp_field[] = { DBAL_FIELD_DSP_NOEXT, DBAL_FIELD_DSP_EXT };
    dbal_fields_e dsp_agg_field[] = { DBAL_FIELD_DSP_NOEXT_AGG, DBAL_FIELD_DSP_EXT_AGG };

    int ext_mode;

    SHR_FUNC_INIT_VARS(unit);

    ext_mode = (dnx_data_port.general.nof_tm_ports_get(unit) > 256 ? 1 : 0);

    if (BCM_GPORT_IS_TRUNK(port))
    {
        /*
         * Get egress trunk
         */
        SHR_IF_ERR_EXIT(dnx_algo_trunk_egress_get(unit, BCM_GPORT_TRUNK_GET(port), &egress_trunk_id, &profile_found));
        if (!profile_found)
        {
            SHR_ERR_EXIT(_SHR_E_NOT_FOUND, " gport (0x%x), is encoded as TRUNK but does not have egress profile\n",
                         port);
        }
        /*
         * Encode destination as aggregation port
         */
        SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                        (unit, egress_destination_field[ext_mode], dsp_agg_field[ext_mode], (uint32 *) &egress_trunk_id,
                         destination));
    }
    else
    {
        /*
         * Encode tm destination as dsp
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_LOCAL_IS_MANDATORY, &gport_info));
        SHR_IF_ERR_EXIT(dnx_algo_port_tm_port_get(unit, gport_info.local_port, (int *) &core_id, &tm_port));
        SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                        (unit, egress_destination_field[ext_mode], dsp_field[ext_mode], &tm_port, destination));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * Same interface as
 * algo_gpm_encode_destination_field_from_gport, but this
 * function also returns one more parameter:
 *
 *  gport_is_valid_destination -
 *      If GPORT cannot be translated to destination pointed
 *      memory is loaded by FALSE, otherwise TRUE
 */
static shr_error_e
algo_gpm_encode_destination_field_from_gport_internal(
    int unit,
    bcm_gport_t port,
    uint32 *destination,
    uint8 *gport_is_valid_destination)
{
    bcm_gport_t gport;
    dnx_algo_gpm_gport_phy_info_t gport_info;
    uint32 trap_destination[1];
    uint32 snoop_str;
    uint32 trap_str;
    uint32 trap_id;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Check if gport is actually local port that is not encoded as GPORT
     */
    if (BCM_GPORT_IS_SET(port))
    {
        gport = port;
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, port));
        BCM_GPORT_LOCAL_SET(gport, port);
    }

    *gport_is_valid_destination = TRUE;

    /*
     * Set DESTINATION according to GPORT TYPE
     */

    if (BCM_GPORT_IS_BLACK_HOLE(gport))
    {
        /*
         * GPORT type is BLACK HOLE and should be dropped
         */
        *destination = DNX_ALGO_GPM_DESTINATION_INVALID;
    }
    else if (BCM_GPORT_IS_SYSTEM_PORT(gport))
    {
        /*
         * GPORT type is SYSTEM PORT
         */
        /*
         * Get system port from gport
         */
        uint32 sys_port = BCM_GPORT_SYSTEM_PORT_ID_GET(gport);
        /*
         * checking that system port is valid
         */
        if (sys_port > dnx_data_device.general.max_nof_system_ports_get(unit) - 1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "gport SYSTEM port id is invalid. gport (0x%x) \n", gport);
        }
        /*
         * Encode destination as system port
         */
        SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                        (unit, DBAL_FIELD_DESTINATION, DBAL_FIELD_PORT_ID, &sys_port, destination));
    }
    else if (BCM_GPORT_IS_LOCAL(gport) || BCM_GPORT_IS_LOCAL_CPU(gport) || BCM_GPORT_IS_MODPORT(gport))
    {
        /*
         * GPORT type is LOCAL PORT
         */
        /*
         * Get system port from gport
         */
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                        (unit, gport, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_info));
        /*
         * Encode destination as system port
         */
        SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                        (unit, DBAL_FIELD_DESTINATION, DBAL_FIELD_PORT_ID, &gport_info.sys_port, destination));
    }
    else if (BCM_GPORT_IS_TRUNK(gport))
    {
        /*
         * GPORT is LAG, Encode destination as lag (System Port Aggregate)
         */
        uint32 spa, spa_id;
        SHR_IF_ERR_EXIT(dnx_trunk_gport_to_spa(unit, gport, 0, &spa));
        spa_id = spa & ((1 << dnx_data_trunk.parameters.spa_type_shift_get(unit)) - 1);
        SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                        (unit, DBAL_FIELD_DESTINATION, DBAL_FIELD_LAG_ID, &spa_id, destination));
    }
    else if (BCM_GPORT_IS_MCAST(gport))
    {
        /*
         * GPORT is MULTICAST, Encode destination as multicast
         */
        uint32 mc_id = _BCM_MULTICAST_ID_GET(BCM_GPORT_MCAST_GET(gport));
        SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set(unit, DBAL_FIELD_DESTINATION, DBAL_FIELD_MC_ID,
                                                             &mc_id, destination));
    }
    else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport))
    {
        /*
         * GPORT type is UNICAST GROUP (CosQ/Flow), Encode destination as flow id
         */
        uint32 flow_id = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport);
        SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set(unit, DBAL_FIELD_DESTINATION, DBAL_FIELD_FLOW_ID,
                                                             &flow_id, destination));
    }
    else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport))
    {
        /*
         * GPORT type is MULTICAST GROUP (CosQ/Flow), Encode destination as flow id
         */
        uint32 flow_id = BCM_GPORT_MCAST_QUEUE_GROUP_QID_GET(gport);
        SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set(unit, DBAL_FIELD_DESTINATION, DBAL_FIELD_FLOW_ID,
                                                             &flow_id, destination));
    }
    else if (BCM_GPORT_IS_TRAP(gport))
    {
        /*
         * GPORT type is TRAP
         */
        snoop_str = BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(gport);
        trap_str = BCM_GPORT_TRAP_GET_STRENGTH(gport);
        trap_id = BCM_GPORT_TRAP_GET_ID(gport);
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, DBAL_FIELD_INGRESS_TRAP_DEST, DBAL_FIELD_TRAP_SNP_STRENGTH, &snoop_str,
                         trap_destination));
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, DBAL_FIELD_INGRESS_TRAP_DEST, DBAL_FIELD_TRAP_FWD_STRENGTH, &trap_str,
                         trap_destination));
        SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                        (unit, DBAL_FIELD_INGRESS_TRAP_DEST, DBAL_FIELD_INGRESS_TRAP_ID, &trap_id, trap_destination));

        SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                        (unit, DBAL_FIELD_DESTINATION, DBAL_FIELD_INGRESS_TRAP_DEST, trap_destination, destination));
    }
    else if (BCM_GPORT_IS_FORWARD_PORT(gport))
    {
        /*
         * GPORT type is FEC (forward group), encode destination as FEC
         */
        uint32 fec = BCM_GPORT_FORWARD_PORT_GET(gport);
        SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                        (unit, DBAL_FIELD_DESTINATION, DBAL_FIELD_FEC, &fec, destination));
    }
    else if (((BCM_GPORT_IS_VLAN_PORT(gport)) || (BCM_GPORT_IS_EXTENDER_PORT(gport)) ||
              (BCM_GPORT_IS_MPLS_PORT(gport))) && ((BCM_GPORT_SUB_TYPE_IS_FORWARD_GROUP(gport)
                                                    || BCM_GPORT_SUB_TYPE_IS_PROTECTION(gport))))
    {
        /*
         * GPORT type is LIF, but subtype not supported
         */
        SHR_ERR_EXIT(_SHR_E_PARAM, "Unsupported GPORT SUBTYPE in gport (0x%x), "
                     "supported are BCM_GPORT_SUB_TYPE_VLAN_TRANSLATION, BCM_GPORT_SUB_TYPE_VIRTUAL_INGRESS_NATIVE and BCM_GPORT_SUB_TYPE_LIF\n",
                     gport);
    }
    else
    {
        /*
         * Unsupported GPORT type
         */
        *gport_is_valid_destination = FALSE;
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * See documentation in .h file
 */
shr_error_e
algo_gpm_encode_destination_field_from_gport(
    int unit,
    uint32 flags,
    bcm_gport_t port,
    uint32 *destination)
{
    uint8 gport_is_valid_destination;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(destination, _SHR_E_PARAM, "destination");

    /*
     * In case destination is egress multicast encoding is different than in regular case
     */
    if (_SHR_IS_FLAG_SET(flags, ALGO_GPM_ENCODE_DESTINATION_EGRESS_MULTICAST))
    {
        SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport_egress_mc_internal(unit, port, destination));
    }
    else
    {
        SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport_internal
                        (unit, port, destination, &gport_is_valid_destination));
        if (!gport_is_valid_destination)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "port is not a valid destination gport.\r\n");
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * See documentation in .h file
 */
shr_error_e
algo_gpm_gport_from_encoded_destination_field(
    int unit,
    uint32 flags,
    uint32 destination,
    bcm_gport_t * gport)
{
    dbal_fields_e dbal_dest_type;
    uint32 dest_val = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(gport, _SHR_E_PARAM, "gport");

    if (_SHR_IS_FLAG_SET(flags, ALGO_GPM_ENCODE_DESTINATION_EGRESS_MULTICAST))
    {
        SHR_ERR_EXIT(_SHR_E_INTERNAL, "gport for egress multicast get is not supported\n");
    }

    if (destination == DNX_ALGO_GPM_DESTINATION_INVALID)
    {
        /*
         *  Invalid destination means packets will be dropped
         */
        *gport = BCM_GPORT_BLACK_HOLE;
    }
    else
    {
        /*
         * Check if destination is FEC or physical port using destination DBAL encoding
         */
        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                        (unit, DBAL_FIELD_DESTINATION, destination, &dbal_dest_type, &dest_val));
        if (dbal_dest_type == DBAL_FIELD_FEC)
        {
            /*
             *  destination is FEC
             */
            BCM_GPORT_FORWARD_PORT_SET(*gport, dest_val);
        }
        else
        {
            /*
             * destination is phy-port
             */
            if (dest_val == DNX_ALGO_GPM_DESTINATION_INVALID)
            {
                /*
                 * GPORT type is BLACK HOLE packet will be dropped
                 */
                *gport = BCM_GPORT_BLACK_HOLE;
            }
            else if (dbal_dest_type == DBAL_FIELD_MC_ID)
            {
                /*
                 * multicast ID
                 */
                BCM_GPORT_MCAST_SET(*gport, dest_val);
            }
            else if (dbal_dest_type == DBAL_FIELD_PORT_ID)
            {
                /*
                 * system port ID - mapping to modport
                 */
                BCM_GPORT_SYSTEM_PORT_ID_SET(*gport, dest_val);
            }
            else if (dbal_dest_type == DBAL_FIELD_LAG_ID)
            {
                /*
                 * LAG ID
                 */
                int dummy;
                SHR_IF_ERR_EXIT(dnx_trunk_spa_id_to_gport(unit, dest_val, &dummy, gport));
            }
            else if (dbal_dest_type == DBAL_FIELD_FLOW_ID)
            {
                /*
                 * Flow ID
                 */
                BCM_GPORT_UNICAST_QUEUE_GROUP_SET(*gport, dest_val);
            }
            else if (dbal_dest_type == DBAL_FIELD_INGRESS_TRAP_DEST)
            {
                uint32 snoop_str;
                uint32 trap_str;
                uint32 trap_id;

                SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                                (unit, DBAL_FIELD_INGRESS_TRAP_DEST, DBAL_FIELD_TRAP_SNP_STRENGTH, &snoop_str,
                                 &destination));
                SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                                (unit, DBAL_FIELD_INGRESS_TRAP_DEST, DBAL_FIELD_TRAP_FWD_STRENGTH, &trap_str,
                                 &destination));
                SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                                (unit, DBAL_FIELD_INGRESS_TRAP_DEST, DBAL_FIELD_INGRESS_TRAP_ID, &trap_id,
                                 &destination));
                BCM_GPORT_TRAP_SET(*gport, trap_id, trap_str, snoop_str);
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Can not retrieve port type\n");
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

shr_error_e
algo_gpm_gport_and_encap_to_forward_information(
    int unit,
    bcm_gport_t gport,
    uint32 encap_id,
    dnx_algo_gpm_forward_info_t * forward_info)
{
    uint8 gport_is_valid_destination;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(forward_info, _SHR_E_PARAM, "forward_info");
    sal_memset(forward_info, 0, sizeof(dnx_algo_gpm_forward_info_t));

    /*
     * Destination - either from GPORT or from SW state
     */
    SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport_internal
                    (unit, gport, &forward_info->destination, &gport_is_valid_destination));
    /*
     * If GPORT is not valid destination - probably logical gport, try to get forward info from SW state
     */
    if (gport_is_valid_destination)
    {
        /*
         * If GPORT is valid destination - get additional info from encap id
         */
        if ((encap_id != BCM_FORWARD_ENCAP_ID_INVALID) && (encap_id != 0))
        {
            if (BCM_FORWARD_ENCAP_ID_IS_OUTLIF(encap_id))
            {
                /*
                 * In J2 mode there is no EEI, so no need to use this MACRO (encoding of OUTLIF type is 0).
                 * In Jericho mode we need to distinguish EEI from Outlif so this check is needed.
                 */
                forward_info->fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF;
                forward_info->outlif = BCM_FORWARD_ENCAP_ID_VAL_GET(encap_id);
            }
            else
            {
                /*
                 * EEI - JERICHO MODE ONLY
                 */
                if (dnx_data_headers.system_headers.system_headers_mode_get(unit) !=
                    DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "encap_id of type EEI is only supported under JR mode");
                }
                forward_info->fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_EEI;
                if (BCM_FORWARD_ENCAP_ID_EEI_USAGE_GET(encap_id) == BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT)
                {
                    /*
                     * VC + Push Profile
                     */
                    uint32 eei_val, vc_label, push_profile;
                    vc_label = BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_VC_GET(encap_id);
                    push_profile = BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_PUSH_PROFILE_GET(encap_id);

                    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                                    (unit, DBAL_FIELD_EEI_MPLS_PUSH_COMMAND, DBAL_FIELD_MPLS_LABEL, &vc_label,
                                     &eei_val));
                    SHR_IF_ERR_EXIT(dbal_fields_struct_field_encode
                                    (unit, DBAL_FIELD_EEI_MPLS_PUSH_COMMAND, DBAL_FIELD_PUSH_CMD_ID, &push_profile,
                                     &eei_val));

                    SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                                    (unit, DBAL_FIELD_EEI, DBAL_FIELD_EEI_MPLS_PUSH_COMMAND, &eei_val,
                                     &forward_info->eei));
                }
                else if (BCM_FORWARD_ENCAP_ID_EEI_USAGE_GET(encap_id) == BCM_FORWARD_ENCAP_ID_EEI_USAGE_ENCAP_POINTER)
                {
                    /*
                     * Outlif
                     */
                    uint32 eei_val = BCM_FORWARD_ENCAP_ID_VAL_GET(encap_id);
                    SHR_IF_ERR_EXIT(dbal_fields_parent_field32_value_set
                                    (unit, DBAL_FIELD_EEI, DBAL_FIELD_EEI_ENCAPSULATION_POINTER, &eei_val,
                                     &forward_info->eei));
                }
                else
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Unknown EEI type, gport (0x%x)\n", gport);
                }
            }
        }
        else
        {
            /*
             * If ecnap_id not valid, type is destination only
             */
            forward_info->fwd_info_result_type = DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY;
        }
    }
    else
    {
        /*
         * If GPORT is not valid destination - probably logical gport, try to get forward info from SW state
         */
        /*
         * Get from Gport to Forward Info table (SW state)
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L2_GPORT_TO_FORWARDING_SW_INFO, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_L2_LOGICAL_GPORT, gport);
        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

        /*
         * Get type from entry
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                                            &forward_info->fwd_info_result_type));
        /*
         * According to type - Set forward info result type and fill OutLif / EEI
         */
        switch (forward_info->fwd_info_result_type)
        {
            case DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY:
            {
                /*
                 * Destination only
                 */
                break;
            }
            case DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF:
            {
                /*
                 * Destination + Outlif
                 */
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, &forward_info->outlif));
                break;
            }
            case DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_EEI:
            {
                /*
                 * Destination + EEI
                 */
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                (unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, &forward_info->eei));
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unknown forward result type (0x%x)\n",
                             forward_info->fwd_info_result_type);
                break;
            }
        }
        /*
         * Destination - always exists
         */
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE,
                                                            &forward_info->destination));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file for documentation
 */
shr_error_e
algo_gpm_gport_l2_forward_info_add(
    int unit,
    uint8 is_replace,
    bcm_gport_t gport,
    dnx_algo_gpm_forward_info_t * forward_info)
{
    uint32 entry_handle_id;
    uint8 info_replace = FALSE;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(forward_info, _SHR_E_INTERNAL, "forward_info");

    /*
     * Verify:
     * If replacing an existing gport, only AC can have new info
     */
    if (is_replace)
    {
        /*
         * Getting existing forward information
         */
        dnx_algo_gpm_forward_info_t old_forward_info;
        SHR_IF_ERR_EXIT(algo_gpm_gport_l2_forward_info_get(unit, gport, &old_forward_info));

        /*
         * For VLAN PORT learn information type change or destination change is allowed
         */
        if (BCM_GPORT_IS_VLAN_PORT(gport))
        {
            if ((old_forward_info.fwd_info_result_type != forward_info->fwd_info_result_type) ||
                (old_forward_info.destination != forward_info->destination))
            {
                /*
                 * In this case, delete the existing entry before writing the new one
                 */
                info_replace = TRUE;
                SHR_IF_ERR_EXIT(algo_gpm_gport_l2_forward_info_delete(unit, gport));
            }
        }
        else
        {
            if (old_forward_info.fwd_info_result_type != forward_info->fwd_info_result_type)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Replacing type of forwarding information is not allowed. \n");
            }
            if (old_forward_info.outlif != forward_info->outlif)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Replacing outlif of forwarding information is not allowed. \n");
            }
            if (old_forward_info.eei != forward_info->eei)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Replacing eei of forwarding information is not allowed. \n");
            }
            if (old_forward_info.destination != forward_info->destination)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Replacing destination of forwarding information is not allowed. \n");
            }
        }
    }
    /*
     * If replacing existing gport no need to write again unless destination is replaced
     */
    if ((!is_replace) || info_replace)
    {
        /*
         * GPORT to forward information SW state
         */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L2_GPORT_TO_FORWARDING_SW_INFO, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_L2_LOGICAL_GPORT, gport);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                                     forward_info->fwd_info_result_type);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE,
                                     forward_info->destination);
        if (forward_info->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE,
                                         forward_info->outlif);
        }
        if (forward_info->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_EEI)
        {
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, forward_info->eei);
        }
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_L2_GPORT_TO_FORWARDING_SW_INFO, entry_handle_id));

        /*
         * forward information to GPORT SW state
         * In case result type is destination only, do not store in SW state
         */
        if (forward_info->fwd_info_result_type != DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L2_FORWARDING_SW_INFO_TO_GPORT, &entry_handle_id));
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DBAL_RESULT_TYPE,
                                       forward_info->fwd_info_result_type);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, forward_info->destination);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, forward_info->outlif);
            dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI, forward_info->eei);
            dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_L2_LOGICAL_GPORT, INST_SINGLE, gport);
            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * See .h file for documentation
 */
shr_error_e
algo_gpm_gport_l2_forward_info_get(
    int unit,
    bcm_gport_t gport,
    dnx_algo_gpm_forward_info_t * forward_info)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(forward_info, _SHR_E_INTERNAL, "forward_info");
    sal_memset(forward_info, 0, sizeof(dnx_algo_gpm_forward_info_t));

    /*
     * forward information SW state from GPORT
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L2_GPORT_TO_FORWARDING_SW_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_L2_LOGICAL_GPORT, gport);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE,
                     &(forward_info->fwd_info_result_type)));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &(forward_info->destination)));
    if (forward_info->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, &(forward_info->outlif)));
    }
    if (forward_info->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_EEI)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, &(forward_info->eei)));
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_L2_GPORT_TO_FORWARDING_SW_INFO, entry_handle_id));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/*
 * For documentation see .h file
 */
shr_error_e
algo_gpm_gport_l2_forward_info_delete(
    int unit,
    bcm_gport_t gport)
{
    uint32 entry_handle_id;
    dnx_algo_gpm_forward_info_t forward_info;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&forward_info, 0, sizeof(dnx_algo_gpm_forward_info_t));

    /*
     * GPORT to forward information SW state,
     * First get entry then delete it.
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L2_GPORT_TO_FORWARDING_SW_INFO, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_L2_LOGICAL_GPORT, gport);
    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &forward_info.fwd_info_result_type));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                    (unit, entry_handle_id, DBAL_FIELD_DESTINATION, INST_SINGLE, &forward_info.destination));
    if (forward_info.fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, INST_SINGLE, &forward_info.outlif));
    }
    if (forward_info.fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_EEI)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_EEI, INST_SINGLE, &forward_info.eei));
    }

    SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_L2_GPORT_TO_FORWARDING_SW_INFO, entry_handle_id));

    /*
     * forward information to GPORT SW state
     * In case result type is destination only, the entry is not stored in SW state
     */
    if (forward_info.fwd_info_result_type != DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_ONLY)
    {
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L2_FORWARDING_SW_INFO_TO_GPORT, &entry_handle_id));
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DBAL_RESULT_TYPE,
                                   forward_info.fwd_info_result_type);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, forward_info.destination);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, forward_info.outlif);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI, forward_info.eei);
        SHR_IF_ERR_EXIT(dbal_entry_clear(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
* \brief
* Get gport from forward information using SW state
* L2_FORWARDING_SW_INFO_TO_GPORT.
* \par DIRECT INPUT:
*   \param [in] unit       -  Relevant unit.
*   \param [in] forward_info        -  forwarding information.
*   \param [out] gport  -  GPORT value that was found in SW
*          state.
*   \param [out] is_gport_found   -  indication if SW state
*          entry was found.
* \par DIRECT OUTPUT:
*   shr_error_e - Non-zero in case of an error.
* \remark
*   * None
* \see
*   dnx_algo_gpm_forward_info_t
*/
shr_error_e
algo_gpm_gport_from_forward_information_find(
    int unit,
    dnx_algo_gpm_forward_info_t * forward_info,
    bcm_gport_t * gport,
    uint8 *is_gport_found)
{
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    uint32 entry_handle_id;
    shr_error_e rv;

    /*
     * Take table handle:
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_L2_FORWARDING_SW_INFO_TO_GPORT, &entry_handle_id));
    /*
     * Set keys:
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DBAL_RESULT_TYPE, forward_info->fwd_info_result_type);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_DESTINATION, forward_info->destination);
    if (forward_info->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_OUTLIF)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, forward_info->outlif);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_GLOB_OUT_LIF, 0);
    }
    if (forward_info->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_EEI)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI, forward_info->eei);
    }
    else
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_EEI, 0);
    }

    /*
     * Get values:
     */
    dbal_value_field32_request(unit, entry_handle_id, DBAL_FIELD_L2_LOGICAL_GPORT, INST_SINGLE, (uint32 *) gport);
    rv = dbal_entry_get(unit, entry_handle_id, DBAL_COMMIT);

    /*
     * Check if this FWD info was found in SW state
     */
    if (rv == _SHR_E_NONE)
    {
        /*
         *  Gport found in SW state
         */
        *is_gport_found = 1;
    }
    else if (rv == _SHR_E_NOT_FOUND)
    {
        /*
         *  Gport not found in SW state
         */
        *is_gport_found = 0;
    }
    else
    {
        /*
         *  Another error
         */
        SHR_SET_CURRENT_ERR(rv);
        SHR_EXIT();
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
*   See .h file
*/
shr_error_e
algo_gpm_forward_information_to_non_logical_port_and_encap(
    int unit,
    dnx_algo_gpm_forward_info_t * forward_info,
    bcm_gport_t * gport,
    uint32 *encap_id)
{
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Port
     */
    SHR_IF_ERR_EXIT(algo_gpm_gport_from_encoded_destination_field
                    (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, forward_info->destination, gport));

    /*
     * Encap id - in JR2 mode can be only outlif. in JR1 mode can be EEI
     */
    if (forward_info->fwd_info_result_type == DBAL_RESULT_TYPE_L2_GPORT_TO_FORWARDING_SW_INFO_DEST_EEI)
    {
        /*
         * JR1 Mode only start
         */
        dbal_fields_e dbal_eei_type;
        uint32 eei_val, eei_usage;

        if (dnx_data_headers.system_headers.system_headers_mode_get(unit) !=
            DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO_MODE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "encap_id of type EEI is only supported under JR mode");
        }

        SHR_IF_ERR_EXIT(dbal_fields_uint32_sub_field_info_get
                        (unit, DBAL_FIELD_EEI, forward_info->eei, &dbal_eei_type, &eei_val));
        switch (dbal_eei_type)
        {
            case DBAL_FIELD_EEI_MPLS_PUSH_COMMAND:
            {
                /*
                 * Overwrite eei_val with encoded VC label + Push profile
                 */
                uint32 vc_label, push_profile;

                SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                                (unit, DBAL_FIELD_EEI_MPLS_PUSH_COMMAND, DBAL_FIELD_PUSH_CMD_ID, &push_profile,
                                 &eei_val));
                SHR_IF_ERR_EXIT(dbal_fields_struct_field_decode
                                (unit, DBAL_FIELD_EEI_MPLS_PUSH_COMMAND, DBAL_FIELD_MPLS_LABEL, &vc_label, &eei_val));

                BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT_SET(eei_val, vc_label, push_profile);
                eei_usage = BCM_FORWARD_ENCAP_ID_EEI_USAGE_MPLS_PORT;
                break;
            }
            case DBAL_FIELD_EEI_ENCAPSULATION_POINTER:
            {
                eei_usage = BCM_FORWARD_ENCAP_ID_EEI_USAGE_ENCAP_POINTER;
                break;
            }
            default:
                SHR_ERR_EXIT(_SHR_E_INTERNAL, "dbal_eei_type (%d) is not supported \r\n", dbal_eei_type);
        }

        BCM_FORWARD_ENCAP_ID_VAL_SET(*encap_id, BCM_FORWARD_ENCAP_ID_TYPE_EEI, eei_usage, eei_val);
        /*
         * JR1 Mode only end
         */
    }
    else
    {
        *encap_id = forward_info->outlif;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
* See .h file for documentation
*/
shr_error_e
algo_gpm_gport_and_encap_from_forward_information(
    int unit,
    bcm_gport_t * gport,
    uint32 *encap_id,
    dnx_algo_gpm_forward_info_t * forward_info,
    uint8 force_port_and_encap)
{
    uint8 is_gport_found;

    SHR_FUNC_INIT_VARS(unit);

    SHR_NULL_CHECK(forward_info, _SHR_E_INTERNAL, "forward_info");
    SHR_NULL_CHECK(gport, _SHR_E_INTERNAL, "gport");

    if (force_port_and_encap && !(encap_id))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "encap_id must not be NULL when calling algo_gpm_gport_and_encap_from_forward_information with force_destination\n");
    }

    /*
     * Check if user requires to return port and encap as separate parameters
     */
    if (!force_port_and_encap)
    {
        /*
         * If we are not forced to return port+encap, then we try to look for gport in sw state
         */
        SHR_IF_ERR_EXIT(algo_gpm_gport_from_forward_information_find(unit, forward_info, gport, &is_gport_found));
    }

    /*
     * If we are forced return port+encap or we did not find in SW state,
     * then we will return FEC/physical port + encap_id
     */
    if (force_port_and_encap || (!force_port_and_encap && !is_gport_found))
    {
        SHR_IF_ERR_EXIT(algo_gpm_forward_information_to_non_logical_port_and_encap
                        (unit, forward_info, gport, encap_id));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
shr_error_e
algo_gpm_tm_gport_to_port_get(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_port_t * port)
{
    int modid, core;
    int tm_port;
    int is_local;
    int ftmh_pp_dsp;

    SHR_FUNC_INIT_VARS(unit);

    if (BCM_GPORT_IS_LOCAL(gport))
    {
        *port = BCM_GPORT_LOCAL_GET(gport);
    }
    else if (BCM_GPORT_IS_LOCAL_INTERFACE(gport))
    {
        *port = BCM_GPORT_LOCAL_INTERFACE_GET(gport);
    }
    else if (BCM_COSQ_GPORT_IS_E2E_PORT(gport))
    {
        *port = BCM_COSQ_GPORT_E2E_PORT_GET(gport);
    }
    else if (BCM_COSQ_GPORT_IS_E2E_INTERFACE(gport))
    {
        *port = BCM_COSQ_GPORT_E2E_INTERFACE_GET(gport);
    }
    else if (BCM_COSQ_GPORT_IS_PORT_TC(gport))
    {
        *port = BCM_COSQ_GPORT_PORT_TC_GET(gport);
    }
    else if (BCM_COSQ_GPORT_IS_E2E_PORT_TC(gport))
    {
        *port = BCM_COSQ_GPORT_E2E_PORT_TC_GET(gport);
    }
    else if (BCM_COSQ_GPORT_IS_PORT_TCG(gport))
    {
        *port = BCM_COSQ_GPORT_PORT_TCG_GET(gport);
    }
    else if (BCM_COSQ_GPORT_IS_E2E_PORT_TCG(gport))
    {
        *port = BCM_COSQ_GPORT_E2E_PORT_TCG_GET(gport);
    }
    else if (BCM_COSQ_GPORT_IS_UCAST_EGRESS_QUEUE(gport))
    {
        *port = BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_GET(gport);
    }
    else if (BCM_COSQ_GPORT_IS_MCAST_EGRESS_QUEUE(gport))
    {
        *port = BCM_COSQ_GPORT_MCAST_EGRESS_QUEUE_GET(gport);
    }
    else if (BCM_GPORT_IS_MODPORT(gport))
    {
        ftmh_pp_dsp = BCM_GPORT_MODPORT_PORT_GET(gport);
        modid = BCM_GPORT_MODPORT_MODID_GET(gport);

        SHR_IF_ERR_EXIT(dnx_stk_sys_modid_is_local_get(unit, modid, &is_local));
        if (is_local)
        {
            SHR_IF_ERR_EXIT(dnx_stk_sys_modport_to_tm_port_convert(unit, modid, ftmh_pp_dsp, &core, &tm_port));
            SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core, tm_port, port));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Modid %d is not for local device", modid);
        }
    }
    else
    {
        /*
         * assuming gport is logical port
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_valid_verify(unit, gport));
        *port = gport;
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
shr_error_e
algo_gpm_tm_gport_to_core_get(
    int unit,
    uint32 flags,
    bcm_gport_t gport,
    bcm_core_t * core)
{
    bcm_port_t port;
    bcm_module_t modid;
    int fap_id_index;

    SHR_FUNC_INIT_VARS(unit);

    if (dnx_data_device.general.nof_cores_get(unit) <= 1)
    {
        *core = 0;
    }
    else if (BCM_COSQ_GPORT_IS_CORE(gport))
    {
        *core = BCM_COSQ_GPORT_CORE_GET(gport);
    }
    else if (BCM_GPORT_IS_SCHEDULER(gport))
    {
        *core = BCM_GPORT_SCHEDULER_CORE_GET(gport);
    }
    else if (BCM_COSQ_GPORT_IS_VOQ_CONNECTOR(gport))
    {
        *core = BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_GET(gport);
    }
    else if (BCM_COSQ_GPORT_IS_SCHED_CIR(gport))
    {
        *core = BCM_COSQ_GPORT_SCHED_CIR_CORE_GET(gport);
    }
    else if (BCM_COSQ_GPORT_IS_SCHED_PIR(gport))
    {
        *core = BCM_COSQ_GPORT_SCHED_PIR_CORE_GET(gport);
    }
    else if (BCM_COSQ_GPORT_IS_SHARED_SHAPER_ELEM(gport))
    {
        *core = BCM_COSQ_GPORT_SHARED_SHAPER_ELEM_CORE_GET(gport);
    }
    else if (BCM_COSQ_GPORT_IS_COMPOSITE_SF2(gport))
    {
        *core = BCM_COSQ_GPORT_COMPOSITE_SF2_CORE_GET(gport);
    }
    else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport))
    {
        *core = BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_GET(gport);
    }
    else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport))
    {
        *core = BCM_GPORT_MCAST_QUEUE_GROUP_CORE_GET(gport);
    }
    else if (BCM_GPORT_IS_MODPORT(gport))
    {
        modid = BCM_GPORT_MODPORT_MODID_GET(gport);
        SHR_IF_ERR_EXIT(dnx_stk_sys_modid_local_core_get(unit, modid, core, &fap_id_index));
        if (*core == -1)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Cannot extract local core from a remote desitnation modport gport");
        }
    }
    else
    {
        /*
         * Extract logical port from the gport
         *
         * Supported gport conditions:
         *
         * - BCM_GPORT_IS_LOCAL
         * - BCM_COSQ_GPORT_IS_E2E_PORT
         * - BCM_COSQ_GPORT_IS_PORT_TC
         * - BCM_COSQ_GPORT_IS_E2E_PORT_TC
         * - BCM_COSQ_GPORT_IS_PORT_TCG
         * - BCM_COSQ_GPORT_IS_UCAST_EGRESS_QUEUE
         * - BCM_COSQ_GPORT_IS_MCAST_EGRESS_QUEUE
         * - local port (not gport)
         */
        SHR_IF_ERR_EXIT(algo_gpm_tm_gport_to_port_get(unit, 0, gport, &port));
        SHR_IF_ERR_EXIT(dnx_algo_port_core_get(unit, port, core));
    }

    if (flags & ALGO_GPM_GPORT_CORE_F_NO_CORE_ALL)
    {
        if (*core == BCM_CORE_ALL)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "BCM_CORE_ALL is not supported for operation.\n");
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
shr_error_e
algo_gpm_gport_verify(
    int unit,
    bcm_gport_t gport,
    uint32 nof_allowed_types,
    algo_gpm_gport_verify_type_e * allowed_types)
{
    algo_gpm_gport_verify_type_e gport_info = ALGO_GPM_GPORT_VERIFY_TYPE_NOF;
    int gport_info_index = 0, is_supported = 0;

    SHR_FUNC_INIT_VARS(unit);

    /** get gport type */
    if (!BCM_GPORT_IS_SET(gport))
    {
        gport_info = ALGO_GPM_GPORT_VERIFY_TYPE_PORT;
    }
    else if (BCM_GPORT_IS_LOCAL(gport))
    {
        gport_info = ALGO_GPM_GPORT_VERIFY_TYPE_LOCAL_PORT;
    }
    else if (BCM_GPORT_IS_MODPORT(gport))
    {
        gport_info = ALGO_GPM_GPORT_VERIFY_TYPE_MODPORT;
    }
    else if (BCM_GPORT_IS_DEVPORT(gport))
    {
        gport_info = ALGO_GPM_GPORT_VERIFY_TYPE_DEVPORT;
    }
    else if (BCM_GPORT_IS_TRUNK(gport))
    {
        gport_info = ALGO_GPM_GPORT_VERIFY_TYPE_TRUNK;
    }
    else if (BCM_GPORT_IS_SYSTEM_PORT(gport))
    {
        uint32 system_port;

        /*
         * Check gport validity
         */

        system_port = BCM_GPORT_SYSTEM_PORT_ID_GET(gport);
        if (system_port >= dnx_data_device.general.max_nof_system_ports_get(unit))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid system port received: 0x%x\n", system_port);
        }
        gport_info = ALGO_GPM_GPORT_VERIFY_TYPE_SYSTEM_PORT;
    }
    else if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport))
    {
        gport_info = ALGO_GPM_GPORT_VERIFY_TYPE_UCAST_QUEUE_GROUP;
    }
    else if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport))
    {
        gport_info = ALGO_GPM_GPORT_VERIFY_TYPE_MCAST_QUEUE_GROUP;
    }
    else if (BCM_GPORT_IS_LOCAL_CPU(gport))
    {
        gport_info = ALGO_GPM_GPORT_VERIFY_TYPE_LOCAL_CPU;
    }
    else if (BCM_PHY_GPORT_IS_PHYN(gport))
    {
        gport_info = ALGO_GPM_GPORT_VERIFY_TYPE_PHYN;
    }
    else if (BCM_PHY_GPORT_IS_PHYN_LANE(gport))
    {
        gport_info = ALGO_GPM_GPORT_VERIFY_TYPE_PHYN_LANE;
    }
    else if (BCM_PHY_GPORT_IS_PHYN_SYS_SIDE(gport))
    {
        gport_info = ALGO_GPM_GPORT_VERIFY_TYPE_PHYN_SYS_SIDE;
    }
    else if (BCM_PHY_GPORT_IS_PHYN_SYS_SIDE_LANE(gport))
    {
        gport_info = ALGO_GPM_GPORT_VERIFY_TYPE_PHYN_SYS_SIDE_LANE;
    }
    else if (BCM_PHY_GPORT_IS_LANE(gport))
    {
        gport_info = ALGO_GPM_GPORT_VERIFY_TYPE_LANE;
    }
    else if (BCM_GPORT_IS_LOCAL_FABRIC(gport))
    {
        gport_info = ALGO_GPM_GPORT_VERIFY_TYPE_LOCAL_FABRIC;
    }
    else if (BCM_GPORT_IS_BLACK_HOLE(gport))
    {
        gport_info = ALGO_GPM_GPORT_VERIFY_TYPE_BLACK_HOLE;
    }
    else if (BCM_GPORT_IS_MCAST(gport))
    {
        gport_info = ALGO_GPM_GPORT_VERIFY_TYPE_MCAST;
    }
    else
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "unknown gport type 0x%x", gport);
    }

    /** look for the actual type in allowed gports */
    for (gport_info_index = 0; gport_info_index < nof_allowed_types; gport_info_index++)
    {
        if (gport_info == allowed_types[gport_info_index])
        {
            is_supported = 1;
            break;
        }
    }

    /*
     * throw an error in case not supported
     */
    if (!is_supported)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "gport is not supported by API - 0x%x", gport);
    }

exit:
    SHR_FUNC_EXIT;
}
