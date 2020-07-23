/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * diag_dnx_trap_protocol.c
 *
 *  Created on: Mar 22, 2018
 *      Author: dp889757
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAG

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <include/bcm_int/dnx/algo/rx/algo_rx.h>
#include <include/bcm_int/dnx/rx/rx_trap.h>
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <soc/dnx/mdb.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <src/soc/dnx/dbal/dbal_internal.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include "diag_dnx_trap.h"

const sh_sand_enum_t Protocol_trap_type_enum_table[] = {
    /**String      Value                                            Description*/
    {"L2CP", bcmRxTrapL2cpPeer, "L2 Control Protocol trap"}
    ,
    {"ICMP", bcmRxTrapIcmpRedirect, "ICMP trap"}
    ,
    {"IGMP", bcmRxTrapIgmpMembershipQuery, "IGMP trap"}
    ,
    {"8021x", bcmRxTrap8021xFail, "Non authorized 802.1x trap"}
    ,
    {"DHCP6S", bcmRxTrapDhcpv6Server, "DHCP Ipv6 server trap"}
    ,
    {"DHCP6C", bcmRxTrapDhcpv6Client, "DHCP Ipv6 client trap"}
    ,
    {"DHCP4S", bcmRxTrapDhcpv4Server, "DHCP Ipv4 server trap"}
    ,
    {"DHCP4C", bcmRxTrapDhcpv4Client, "DHCP Ipv4 client trap"}
    ,
    {"ARP", bcmRxTrapArp, "ARP trap"}
    ,
    {"MYARP", bcmRxTrapArpMyIp, "MyARP trap"}
    ,
    {"NDP", bcmRxTrapNdp, "NDP trap"}
    ,
    {"MYNDP", bcmRxTrapNdpMyIp, "MyNDP trap"}
    ,
    {"NULL", bcmRxTrapCount, "NULL"}
    ,
    {NULL}
};

/**
* \brief
*    Init callback function for "trap protocol" diag commands
*    1) Create an ingress user defined trap
*    2) Create InLIF
*    3) Assign a protocol profile to InLIF created
*    4) Assign a trap to protocol indexed by key
* \param [in] unit - unit ID
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
*/
static shr_error_e
diag_dnx_trap_protocol_init_cb(
    int unit)
{
    bcm_rx_trap_config_t config = { 0 };
    int fwd_strength = 15, snp_strength = 0;
    bcm_rx_trap_protocol_profiles_t protocol_profiles = { 0 };
    bcm_rx_trap_protocol_key_t key;
    bcm_gport_t trap_gport, lif_gport;
    int trap_id = -1;

    SHR_FUNC_INIT_VARS(unit);

    /**
     * ICMP Redirect full trap config.
     */
    bcm_rx_trap_config_t_init(&config);

    config.flags = BCM_RX_TRAP_UPDATE_DEST;
    config.dest_port = BCM_GPORT_BLACK_HOLE;

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id));

    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id, &config));

    SHR_IF_ERR_EXIT(diag_dnx_rx_trap_inlif_create_example(unit, &lif_gport));

    protocol_profiles.icmpv4_trap_profile = 2;

    SHR_IF_ERR_EXIT(bcm_rx_trap_protocol_profiles_set(unit, lif_gport, &protocol_profiles));

    key.protocol_trap_profile = protocol_profiles.icmpv4_trap_profile;
    key.protocol_type = bcmRxTrapIcmpRedirect;
    key.trap_args = 8;

    BCM_GPORT_TRAP_SET(trap_gport, trap_id, fwd_strength, snp_strength);

    SHR_IF_ERR_EXIT(bcm_rx_trap_protocol_set(unit, &key, trap_gport));

    /**
     * L2CP Redirect full trap config.
     */
    bcm_rx_trap_config_t_init(&config);

    config.flags = BCM_RX_TRAP_UPDATE_DEST;
    config.dest_port = BCM_GPORT_BLACK_HOLE;

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id));

    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id, &config));

    protocol_profiles.l2cp_trap_profile = 3;

    SHR_IF_ERR_EXIT(bcm_rx_trap_protocol_profiles_set(unit, lif_gport, &protocol_profiles));

    key.protocol_trap_profile = protocol_profiles.l2cp_trap_profile;
    key.protocol_type = bcmRxTrapL2cpPeer;
    key.trap_args = 34;

    BCM_GPORT_TRAP_SET(trap_gport, trap_id, fwd_strength, snp_strength);

    SHR_IF_ERR_EXIT(bcm_rx_trap_protocol_set(unit, &key, trap_gport));

    /**
     * DHCP4 Redirect full trap config.
     */
    bcm_rx_trap_config_t_init(&config);

    config.flags = BCM_RX_TRAP_UPDATE_DEST;
    config.dest_port = BCM_GPORT_BLACK_HOLE;

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id));

    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id, &config));

    protocol_profiles.dhcp_trap_profile = 1;

    SHR_IF_ERR_EXIT(bcm_rx_trap_protocol_profiles_set(unit, lif_gport, &protocol_profiles));

    key.protocol_trap_profile = protocol_profiles.dhcp_trap_profile;
    key.protocol_type = bcmRxTrapDhcpv4Server;
    key.trap_args = 0;

    BCM_GPORT_TRAP_SET(trap_gport, trap_id, fwd_strength, snp_strength);

    SHR_IF_ERR_EXIT(bcm_rx_trap_protocol_set(unit, &key, trap_gport));
exit:
    SHR_FUNC_EXIT;

}

static sh_sand_man_t dnx_trap_protocol_list_man = {
    .brief = "Show configured protocol profiles per interface\n",
    .full = "Lists of Protocol traps profiles per IN_PORT/IN-LIF\n",
    .synopsis = "[port=200-203] or [port=LIF-Value]",
    .examples = "port=0x1890270f\n",
    .init_cb = diag_dnx_trap_protocol_init_cb
};

static sh_sand_man_t dnx_trap_protocol_profiles_man = {
    .brief = "Show protocol traps profiles and arguments per Protocol\n",
    .full = "Lists of Protocol traps info(Arguments + profiles) for the requested Protocol type\n",
    .synopsis = "[type=L2CP|ICMP|IGMP|etc...]",
    .examples = "type=ICMP\n" "type=L2CP\n" "type=DHCP4S\n",
    .init_cb = diag_dnx_trap_protocol_init_cb
};

static sh_sand_option_t dnx_trap_protocol_list_options[] = {
    {"port", SAL_FIELD_TYPE_INT32, "In PP-Port or LIF", "0xFFFFFF"},
    {NULL}
};

static sh_sand_option_t dnx_trap_protocol_profiles_options[] = {
    {"type", SAL_FIELD_TYPE_ENUM, "Protocol Type", "NULL", (void *) Protocol_trap_type_enum_table},
    {NULL}
};

/**
 * \brief
 *  Gather the protocol trap info for the supplied port/LIF .
 * \param [in] unit                 - unit ID
 * \param [in] port                  - LIF/PORT
 * \param [in] protocol_profiles    - Protocol profiles
 * \param [in] add_new_inlif        - Is new inlif print required
 * \param [in] prt_ctr              - Print pointer
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_protocol_lines_print(
    int unit,
    bcm_gport_t port,
    bcm_rx_trap_protocol_profiles_t protocol_profiles,
    uint8 add_new_inlif,
    prt_control_t * prt_ctr)
{
    uint8 add_new_protocol;
    bcm_rx_trap_t protocol_types[] = { bcmRxTrapL2cpPeer, bcmRxTrapIcmpRedirect, bcmRxTrap8021xFail,
        bcmRxTrapIgmpMembershipQuery, bcmRxTrapDhcpv4Client, bcmRxTrapDhcpv4Server, bcmRxTrapDhcpv6Client,
        bcmRxTrapDhcpv6Server, bcmRxTrapArp, bcmRxTrapArpMyIp, bcmRxTrapCount
    };
    uint8 nof_trap_args[] = { 64, 32, 1, 1, 1, 1, 1, 1, 1, 1 };
    char *protocol_name = "";
    bcm_rx_trap_protocol_key_t key;
    bcm_gport_t trap_gport;
    int protocol_index = 0, trap_arg_index = 0;
    SHR_FUNC_INIT_VARS(unit);

    for (protocol_index = 0; protocol_types[protocol_index] != bcmRxTrapCount; protocol_index++)
    {
        add_new_protocol = TRUE;
        for (trap_arg_index = 0; trap_arg_index < nof_trap_args[protocol_index]; trap_arg_index++)
        {
            bcm_rx_trap_protocol_key_t_init(&key);
            trap_gport = 0;

            switch (protocol_types[protocol_index])
            {
                case bcmRxTrapL2cpPeer:
                {
                    protocol_name = "L2CP";
                    key.protocol_trap_profile = protocol_profiles.l2cp_trap_profile;
                    break;
                }
                case bcmRxTrapIcmpRedirect:
                {
                    
                    protocol_name = "ICMP";
                    key.protocol_trap_profile = protocol_profiles.icmpv4_trap_profile;
                    break;
                }
                case bcmRxTrapIgmpMembershipQuery:
                case bcmRxTrapIgmpReportLeaveMsg:
                case bcmRxTrapIgmpUndefined:
                {
                    protocol_name = "IGMP";
                    key.protocol_trap_profile = protocol_profiles.igmp_trap_profile;
                    break;
                }
                case bcmRxTrapDhcpv4Client:
                case bcmRxTrapDhcpv4Server:
                case bcmRxTrapDhcpv6Client:
                case bcmRxTrapDhcpv6Server:
                {
                    protocol_name = "DHCP";
                    key.protocol_trap_profile = protocol_profiles.dhcp_trap_profile;
                    break;
                }
                case bcmRxTrapArp:
                case bcmRxTrapArpMyIp:
                {
                    protocol_name = "ARP";
                    key.protocol_trap_profile = protocol_profiles.arp_trap_profile;
                    break;
                }
                case bcmRxTrap8021xFail:
                {
                    protocol_name = "Non-auth 8021x";
                    key.protocol_trap_profile = protocol_profiles.non_auth_8021x_trap_profile;
                    break;
                }
                case bcmRxTrapCount:
                {
                    break;
                }
                default:
                {
                    break;
                }
            }

            key.protocol_type = protocol_types[protocol_index];
            key.trap_args = trap_arg_index;
            SHR_IF_ERR_EXIT(bcm_rx_trap_protocol_get(unit, &key, &trap_gport));

            if (BCM_GPORT_TRAP_GET_STRENGTH(trap_gport) != 0 || BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(trap_gport) != 0)
            {
                if (add_new_inlif)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    if (BCM_GPORT_IS_SET(port))
                    {
                        PRT_CELL_SET("0x%08X", port);
                    }
                    else
                    {
                        PRT_CELL_SET("%d", port);
                    }
                    add_new_inlif = FALSE;
                }
                else
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SKIP(1);
                }

                if (add_new_protocol)
                {
                    PRT_CELL_SET("%s(%d)", protocol_name, key.protocol_trap_profile);
                    add_new_protocol = FALSE;
                }
                else
                {
                    PRT_CELL_SKIP(1);
                }

                if (nof_trap_args[protocol_index] == 1)
                {
                    PRT_CELL_SET("N/A");
                }
                else
                {
                    PRT_CELL_SET("%d", trap_arg_index);
                }
                PRT_CELL_SET("%d", BCM_GPORT_TRAP_GET_ID(trap_gport));
                PRT_CELL_SET("%d", BCM_GPORT_TRAP_GET_STRENGTH(trap_gport));
                PRT_CELL_SET("%d", BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(trap_gport));
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  Gather the protocol trap info for the supplied port/LIF .
 * \param [in] unit           - unit ID
 * \param [in] port            - LIF/PORT to be printed
 * \param [in] sand_control   - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_protocol_specific_print(
    int unit,
    bcm_gport_t port,
    sh_sand_control_t * sand_control)
{
    bcm_rx_trap_protocol_profiles_t protocol_profiles;
    uint8 protocol_trap_mode;
    char *type;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    protocol_trap_mode = dnx_data_trap.ingress.protocol_traps_init_mode_get(unit);

    if (protocol_trap_mode == DNX_RX_TRAP_PROTOCOL_MODE_IN_LIF)
    {
        if (!BCM_GPORT_IS_SET(port) && BCM_L3_ITF_TYPE_IS_RIF(port))
        {
            type = "ETH RIF";
        }
        else
        {
            type = "LIF";
        }
    }
    else
    {
        type = "PORT";
    }

    PRT_TITLE_SET("Protocol Trap Info per %s", type);
    PRT_COLUMN_ADD("%s", type);
    PRT_COLUMN_ADD("Protocol");
    PRT_COLUMN_ADD("Arg ID");
    PRT_COLUMN_ADD("Trap ID");
    PRT_COLUMN_ADD("Forward Strength");
    PRT_COLUMN_ADD("Snoop Strength");

    if (!BCM_GPORT_IS_SET(port) && BCM_L3_ITF_TYPE_IS_RIF(port))
    {
        SHR_IF_ERR_EXIT(bcm_rx_trap_protocol_interface_profiles_get(unit, port, &protocol_profiles));
    }
    else
    {
        SHR_IF_ERR_EXIT(bcm_rx_trap_protocol_profiles_get(unit, port, &protocol_profiles));
    }

    SHR_IF_ERR_EXIT(sh_dnx_trap_protocol_lines_print(unit, port, protocol_profiles, 1, prt_ctr));

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Iterate over all port and gather the info for all port that are configured.
 * \param [in] unit           - unit ID
 * \param [in] sand_control   - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_protocol_port_cmd(
    int unit,
    sh_sand_control_t * sand_control)
{
    bcm_rx_trap_protocol_profiles_t protocol_profiles;
    uint32 entry_handle_id = 0;
    uint32 dbal_tables[] = { DBAL_TABLE_PROTOCOL_TRAP_HANDLING_PROFILE };
    uint32 nof_tables = sizeof(dbal_tables) / sizeof(dbal_tables[0]);
    uint32 table;
    uint32 in_port_field, field_val = 0;
    int is_end = 0, rv = 0;
    uint8 add_new_port;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Protocol Trap Info per Port");
    PRT_COLUMN_ADD("PORT");
    PRT_COLUMN_ADD("Protocol");
    PRT_COLUMN_ADD("Arg ID");
    PRT_COLUMN_ADD("Trap ID");
    PRT_COLUMN_ADD("Forward Strength");
    PRT_COLUMN_ADD("Snoop Strength");

    /** Iterate over all tables and all their entries. */
    for (table = 0; table < nof_tables; table++)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_tables[table], &entry_handle_id));

        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        SHR_IF_ERR_EXIT(dbal_iterator_key_field32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_IN_PP_PORT, DBAL_CONDITION_BIGGER_THAN, field_val, 0));
        /** Receive first entry in table. */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {

            /** Get the IN_LIF, which is the KEY of the dbal tables. */
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_IN_PP_PORT, &in_port_field));

            add_new_port = TRUE;
            bcm_rx_trap_protocol_profiles_t_init(&protocol_profiles);

            rv = bcm_rx_trap_protocol_profiles_get(unit, in_port_field, &protocol_profiles);
            if (rv != BCM_E_NONE)
            {
                SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
            }

            SHR_IF_ERR_EXIT(sh_dnx_trap_protocol_lines_print
                            (unit, in_port_field, protocol_profiles, add_new_port, prt_ctr));

            /** Receive next entry in table. */
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Iterate over all LIFs and gather the info for all LIFs that are configured.
 * \param [in] unit           - unit ID
 * \param [in] sand_control   - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_protocol_lif_cmd(
    int unit,
    sh_sand_control_t * sand_control)
{
    bcm_gport_t gport;
    bcm_rx_trap_protocol_profiles_t protocol_profiles;
    uint32 entry_handle_id = 0;
    uint32 dbal_tables[] = { DBAL_TABLE_IN_AC_INFO_DB,
        DBAL_TABLE_IN_LIF_FORMAT_PWE, DBAL_TABLE_IN_LIF_FORMAT_EVPN,
        DBAL_TABLE_IN_LIF_FORMAT_LSP, DBAL_TABLE_IN_LIF_IPvX_TUNNELS
    };
    uint32 nof_tables = sizeof(dbal_tables) / sizeof(dbal_tables[0]);
    uint32 table;
    uint32 in_lif_field, field_val = 1;
    int is_end = 0;
    uint8 add_new_inlif;
    dbal_entry_handle_t *entry_handle;
    int global_lif = -1;
    lif_mapping_local_lif_info_t local_lif_info;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Protocol Trap list per LIF");
    PRT_COLUMN_ADD("LIF");
    PRT_COLUMN_ADD("Protocol");
    PRT_COLUMN_ADD("Arg ID");
    PRT_COLUMN_ADD("Trap ID");
    PRT_COLUMN_ADD("Forward Strength");
    PRT_COLUMN_ADD("Snoop Strength");

    /** Iterate over all tables and all their entries. */
    for (table = 0; table < nof_tables; table++)
    {
        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_tables[table], &entry_handle_id));

        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        SHR_IF_ERR_EXIT(dbal_iterator_key_field32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_IN_LIF, DBAL_CONDITION_BIGGER_THAN, field_val, 0));
        /** Receive first entry in table. */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {

            /** Get the IN_LIF, which is the KEY of the dbal tables. */
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_IN_LIF, &in_lif_field));

            add_new_inlif = TRUE;
            bcm_rx_trap_protocol_profiles_t_init(&protocol_profiles);

            SHR_IF_ERR_EXIT(dbal_entry_handle_get_internal(unit, entry_handle_id, &entry_handle));

            local_lif_info.local_lif = in_lif_field;
            local_lif_info.phy_table = entry_handle->table->physical_db_id[0];
            dnx_algo_lif_mapping_local_to_global_get(unit, DNX_ALGO_LIF_INGRESS, &local_lif_info, &global_lif);
            if (global_lif != -1)
            {
                if (dbal_tables[table] == DBAL_TABLE_IN_LIF_FORMAT_PWE ||
                    dbal_tables[table] == DBAL_TABLE_IN_LIF_FORMAT_LSP)
                {
                    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                                    (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS, _SHR_CORE_ALL,
                                     in_lif_field, &gport));
                }
                else
                {
                    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                                    (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS, _SHR_CORE_ALL,
                                     global_lif, &gport));
                }

                SHR_IF_ERR_EXIT(bcm_rx_trap_protocol_profiles_get(unit, gport, &protocol_profiles));
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
                continue;
            }

            SHR_IF_ERR_EXIT(sh_dnx_trap_protocol_lines_print(unit, gport, protocol_profiles, add_new_inlif, prt_ctr));

            /** Receive next entry in table. */
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
        SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Iterate over all RIFs and gather the info for all RIFs that are configured.
 * \param [in] unit           - unit ID
 * \param [in] sand_control   - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_protocol_rif_cmd(
    int unit,
    sh_sand_control_t * sand_control)
{
    bcm_rx_trap_protocol_profiles_t protocol_profiles;
    uint32 entry_handle_id = 0;
    uint32 vsi_field, field_val = 1;
    int is_end = 0;
    uint8 add_new_rif;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Protocol Trap list per RIF");
    PRT_COLUMN_ADD("RIF");
    PRT_COLUMN_ADD("Protocol");
    PRT_COLUMN_ADD("Arg ID");
    PRT_COLUMN_ADD("Trap ID");
    PRT_COLUMN_ADD("Forward Strength");
    PRT_COLUMN_ADD("Snoop Strength");

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_ING_VSI_INFO_DB, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
    SHR_IF_ERR_EXIT(dbal_iterator_key_field32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_VSI, DBAL_CONDITION_BIGGER_THAN, field_val, 0));
    /** Receive first entry in table. */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        uint32 my_mac_prefix_profile = 0;
        /** Get the VSI, which is the KEY of the dbal tables. */
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_VSI, &vsi_field));

        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                        (unit, entry_handle_id, DBAL_FIELD_MY_MAC_PREFIX, INST_SINGLE, &my_mac_prefix_profile));

        /** In case current VSI is L2 VSI skip it. */
        if (my_mac_prefix_profile == 0)
        {
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
            continue;
        }

        add_new_rif = TRUE;
        bcm_rx_trap_protocol_profiles_t_init(&protocol_profiles);
        SHR_IF_ERR_EXIT(bcm_rx_trap_protocol_interface_profiles_get(unit, vsi_field, &protocol_profiles));

        SHR_IF_ERR_EXIT(sh_dnx_trap_protocol_lines_print(unit, vsi_field, protocol_profiles, add_new_rif, prt_ctr));

        /** Receive next entry in table. */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }
    SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));

    PRT_COMMITX;

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints all protocol trap profiles
 *    for the supplied IN-PP-Port or IN-LIF.
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_protocol_list_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_gport_t port;
    uint8 protocol_trap_mode;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_INT32("port", port);

    protocol_trap_mode = dnx_data_trap.ingress.protocol_traps_init_mode_get(unit);

    if (port == DIAG_DNX_TRAP_PROTOCOL_LIST_ALL)
    {
        if (protocol_trap_mode == DNX_RX_TRAP_PROTOCOL_MODE_IN_LIF)
        {
            if (!BCM_GPORT_IS_SET(port) && BCM_L3_ITF_TYPE_IS_RIF(port))
            {
                SHR_IF_ERR_EXIT(sh_dnx_trap_protocol_rif_cmd(unit, sand_control));
            }
            else
            {
                SHR_IF_ERR_EXIT(sh_dnx_trap_protocol_lif_cmd(unit, sand_control));
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(sh_dnx_trap_protocol_port_cmd(unit, sand_control));
        }
    }
    else
    {
        if (protocol_trap_mode == DNX_RX_TRAP_PROTOCOL_MODE_IN_LIF)
        {
            if (!BCM_GPORT_IS_SET(port))
            {
                if (!BCM_L3_ITF_TYPE_IS_RIF(port))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "Protocol trap mode is IN-LIF, please use a valid In-LIF.");
                }
            }
        }
        else
        {
            if (port >= 256)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "Protocol trap mode is IN-PORT, please use a valid In-Port.");
            }
        }

        SHR_IF_ERR_EXIT(sh_dnx_trap_protocol_specific_print(unit, port, sand_control));
    }

exit:
    SHR_FUNC_EXIT;

}
/**
 * \brief
 *    A diagnostics function that prints all protocol trap profiles
 * \param [in] unit           - unit ID
 * \param [in] protocol_type  - trap type from user
 * \param [in] protocol_name  - trap name from user input
 * \param [in] sand_control   - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_protocol_profiles_print(
    int unit,
    bcm_rx_trap_t protocol_type,
    char *protocol_name,
    sh_sand_control_t * sand_control)
{

    uint8 add_new_protocol, add_new_profile;
    bcm_rx_trap_t protocol_types[] = { bcmRxTrapL2cpPeer, bcmRxTrapIcmpRedirect, bcmRxTrap8021xFail,
        bcmRxTrapIgmpMembershipQuery, bcmRxTrapDhcpv4Client, bcmRxTrapDhcpv4Server, bcmRxTrapDhcpv6Client,
        bcmRxTrapDhcpv6Server, bcmRxTrapArp, bcmRxTrapArpMyIp, bcmRxTrapCount
    };
    uint8 nof_trap_args[] = { 64, 32, 1, 1, 1, 1, 1, 1, 1, 1 };
    bcm_rx_trap_protocol_key_t key;
    bcm_gport_t trap_gport;
    char *protocol_name_inner;
    int protocol_index = 0, trap_arg_index = 0, protocol_trap_profile = 0;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Protocol Trap profiles info");
    PRT_COLUMN_ADD("Protocol");
    PRT_COLUMN_ADD("Profile");
    PRT_COLUMN_ADD("Arg ID");
    PRT_COLUMN_ADD("Trap ID");
    PRT_COLUMN_ADD("Forward Strength");
    PRT_COLUMN_ADD("Snoop Strength");

    for (protocol_index = 0; protocol_types[protocol_index] != bcmRxTrapCount; protocol_index++)
    {
        add_new_protocol = TRUE;
        protocol_name_inner = SH_SAND_GET_ENUM_STR("type", protocol_types[protocol_index]);

        if (protocol_type != bcmRxTrapCount)
        {
            protocol_name_inner = protocol_name;
            if (protocol_types[protocol_index] != protocol_type)
            {
                continue;
            }
        }

        for (protocol_trap_profile = 0; protocol_trap_profile < 4; protocol_trap_profile++)
        {
            add_new_profile = TRUE;
            for (trap_arg_index = 0; trap_arg_index < nof_trap_args[protocol_index]; trap_arg_index++)
            {
                bcm_rx_trap_protocol_key_t_init(&key);
                trap_gport = 0;

                key.protocol_type = protocol_types[protocol_index];
                key.trap_args = trap_arg_index;
                key.protocol_trap_profile = protocol_trap_profile;
                SHR_IF_ERR_EXIT(bcm_rx_trap_protocol_get(unit, &key, &trap_gport));

                if (BCM_GPORT_TRAP_GET_STRENGTH(trap_gport) != 0 || BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(trap_gport) != 0)
                {

                    if (add_new_protocol)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", protocol_name_inner);
                        add_new_protocol = FALSE;
                    }
                    else
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SKIP(1);
                    }

                    if (add_new_profile)
                    {
                        PRT_CELL_SET("%d", protocol_trap_profile);
                        add_new_profile = FALSE;
                    }
                    else
                    {
                        PRT_CELL_SKIP(1);
                    }

                    if (nof_trap_args[protocol_index] == 1)
                    {
                        PRT_CELL_SET("N/A");
                    }
                    else
                    {
                        PRT_CELL_SET("%d", trap_arg_index);
                    }

                    PRT_CELL_SET("%d", BCM_GPORT_TRAP_GET_ID(trap_gport));
                    PRT_CELL_SET("%d", BCM_GPORT_TRAP_GET_STRENGTH(trap_gport));
                    PRT_CELL_SET("%d", BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(trap_gport));
                }
            }
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints all protocol trap profiles
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_protocol_profiles_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_rx_trap_t trap_type;
    char *protocol_name = "";
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_ENUM("type", trap_type);
    protocol_name = SH_SAND_GET_ENUM_STR("type", trap_type);
    SHR_IF_ERR_EXIT(sh_dnx_trap_protocol_profiles_print(unit, trap_type, protocol_name, sand_control));

exit:
    SHR_FUNC_EXIT;

}

sh_sand_cmd_t sh_dnx_trap_protocol_cmds[] = {
    /*
     * keyword, action, command, options, man
     */
    {"PROFiles", sh_dnx_trap_protocol_profiles_cmd, NULL, dnx_trap_protocol_profiles_options,
     &dnx_trap_protocol_profiles_man}
    ,
    {"List", sh_dnx_trap_protocol_list_cmd, NULL, dnx_trap_protocol_list_options, &dnx_trap_protocol_list_man}
    ,
    {NULL}
};
