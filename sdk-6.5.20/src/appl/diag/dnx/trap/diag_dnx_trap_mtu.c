/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * diag_dnx_trap_mtu.c
 *
 *  Created on: Mar 27, 2018
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
#include <include/bcm_int/dnx/rx/rx_trap.h>
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <bcm_int/dnx/rx/rx_trap.h>
#include <bcm/switch.h>
#include <bcm_int/dnx/port/port.h>
#include "include/soc/sand/sand_signals.h"
#include "diag_dnx_trap.h"

#define DNX_RX_ERPP_PMF_COUNTER_NOF                 (0x2)
#define DNX_RX_MTU_PROFILE_SHIFT                    (0x3)
#define DNX_RX_MTU_PROFILE_MASK                     (0x7)
#define DNX_RX_COMPRESSED_LAYER_TYPE_MASK       (0x7)

#define DNX_RX_MTU_ENTRY_INDEX_SET(_mtu_profile, _cmp_layer_type) \
                                    (((_mtu_profile & DNX_RX_MTU_PROFILE_MASK) << DNX_RX_MTU_PROFILE_SHIFT)   \
                                    | (_cmp_layer_type & DNX_RX_COMPRESSED_LAYER_TYPE_MASK))

const sh_dnx_trap_mtu_layer_info_t Eedb_out_lif_tables[] = {
    {DBAL_TABLE_EEDB_ARP, {bcmFieldLayerTypeEth, bcmFieldLayerTypeArp}, 2, {"Ethernet", "ARP"}},
    {DBAL_TABLE_EEDB_IPV4_TUNNEL, {bcmFieldLayerTypeIp4}, 1, {"IPv4"}},
    {DBAL_TABLE_EEDB_IPV6_TUNNEL, {bcmFieldLayerTypeIp6}, 1, {"IPv6"}},
    {DBAL_TABLE_EEDB_MPLS_TUNNEL, {bcmFieldLayerTypeMpls}, 1, {"MPLS"}},
    {DBAL_TABLE_EEDB_PWE, {bcmFieldLayerTypeMpls}, 1, {"MPLS"}},
    {DBAL_TABLE_EEDB_EVPN, {bcmFieldLayerTypeMpls}, 1, {"MPLS"}},
    {DBAL_TABLE_EEDB_SRV6, {bcmFieldLayerTypeSrv6Endpoint, bcmFieldLayerTypeSrv6Beyond}, 2,
     {"SRV6Endpoint", "SRV6Beyond"}},
    {DBAL_TABLE_EEDB_PPPOE, {bcmFieldLayerTypePppoe}, 1, {"PPOE"}},
    {DBAL_TABLE_EEDB_RIF_BASIC, {bcmFieldLayerTypeEth}, 1, {"Ethernet"}}
};

/**
 * \brief
 *    Init callback function for "trap mtu" diag commands
 *    1) Create an ETPP user defined trap
 *    2) Create OutLIF
 *    3) Map fwd layer type to compressed fwd layer type
 *    4) Assign trap and MTU threshold to Outlif and compressed fwd layer type
 * \param [in] unit - unit ID
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
diag_dnx_trap_mtu_init_cb(
    int unit)
{
    bcm_switch_control_info_t info;
    bcm_rx_trap_config_t config;
    bcm_rx_mtu_config_t mtu_config;
    bcm_rx_mtu_profile_key_t mtu_key;
    bcm_rx_mtu_profile_value_t mtu_val;
    bcm_switch_control_key_t key;
    bcm_if_t rif;
    int trap_id;
    SHR_FUNC_INIT_VARS(unit);

    bcm_rx_trap_config_t_init(&config);
    bcm_rx_mtu_config_t_init(&mtu_config);

    SHR_IF_ERR_EXIT(diag_dnx_rx_trap_outrif_create_example(unit, &rif));

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapEgTxUserDefine, &trap_id));

    /*
     * Creating a compressed forwarding layer type 
     */
    key.type = bcmSwitchLinkLayerMtuFilter;
    key.index = bcmFieldLayerTypeIp4;

    info.value = 3;

    SHR_IF_ERR_EXIT(bcm_switch_control_indexed_set(unit, key, info));

    bcm_rx_mtu_profile_key_t_init(&mtu_key);
    bcm_rx_mtu_profile_value_t_init(&mtu_val);

    mtu_key.cmp_layer_type = info.value;
    mtu_key.mtu_profile = 5;

    mtu_val.mtu_val = 100;
    BCM_GPORT_TRAP_SET(mtu_val.trap_gport, BCM_RX_TRAP_EG_TX_TRAP_ID_DROP, 7, 0);

    SHR_IF_ERR_EXIT(bcm_rx_mtu_profile_set(unit, BCM_RX_MTU_RIF, &mtu_key, &mtu_val));

    mtu_config.flags = BCM_RX_MTU_RIF;
    mtu_config.intf = rif;
    mtu_config.mtu = 100;
    mtu_config.mtu_profile = 5;

    SHR_IF_ERR_EXIT(bcm_rx_mtu_set(unit, &mtu_config));

    bcm_rx_mtu_profile_key_t_init(&mtu_key);
    bcm_rx_mtu_profile_value_t_init(&mtu_val);

    mtu_key.cmp_layer_type = 7;
    mtu_key.mtu_profile = 6;

    mtu_val.mtu_val = 6500;
    BCM_GPORT_TRAP_SET(mtu_val.trap_gport, trap_id, 7, 0);

    SHR_IF_ERR_EXIT(bcm_rx_mtu_profile_set(unit, BCM_RX_MTU_PORT, &mtu_key, &mtu_val));

    bcm_rx_mtu_config_t_init(&mtu_config);

    mtu_config.flags = BCM_RX_MTU_PORT;
    mtu_config.gport = 202;
    mtu_config.mtu = 6500;
    mtu_config.mtu_profile = 6;

    SHR_IF_ERR_EXIT(bcm_rx_mtu_set(unit, &mtu_config));

exit:
    SHR_FUNC_EXIT;
}

sh_sand_man_t sh_dnx_trap_mtu_cmd_man = {
    .brief = "Shows set MTU traps information\n",
    .full = "Lists of all MTU traps configured and the ports/interfaces they are attached to \n",
    .init_cb = diag_dnx_trap_mtu_init_cb
};

sh_sand_man_t sh_dnx_trap_mtu_profile_cmd_man = {
    .brief = "Show MTU Profiles information\n",
    .full = "Lists of all MTU configured Profiles and their full details \n",
    .init_cb = diag_dnx_trap_mtu_init_cb
};

/**
 * \brief
 *    Print Function for LIFs/RIFs which have MTU enabled. ".
 * \param [in] unit - unit ID
 * \param [in] prt_ctr - print pointer from the main function.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
sh_dnx_trap_mtu_interfaces_print(
    int unit,
    prt_control_t * prt_ctr)
{
    bcm_gport_t gport;
    uint32 out_lif;
    uint32 mtu_profile[1];
    int nof_fwd_types = 0;
    int is_end;
    uint32 field_rule_val = 0;
    char trap_id_str[20] = "";
    bcm_switch_control_info_t info;
    bcm_switch_control_key_t key;
    bcm_rx_mtu_profile_key_t mtu_key;
    bcm_rx_mtu_profile_value_t mtu_val;
    int nof_tables, table_index = 0;
    uint32 entry_handle_id;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    nof_tables = sizeof(Eedb_out_lif_tables) / sizeof(sh_dnx_trap_mtu_layer_info_t);
    /*
     * Loop through all DBAL tables which hold LIFs/RIFs containing MTU_PROFILEs.
     */
    for (table_index = 0; table_index < nof_tables; table_index++)
    {

        /** Taking a handle */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, Eedb_out_lif_tables[table_index].dbal_table, &entry_handle_id));

        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
        /** Add KEY rule*/
        SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                        (unit, entry_handle_id, DBAL_FIELD_MTU_PROFILE, INST_SINGLE, DBAL_CONDITION_BIGGER_THAN,
                         &field_rule_val, NULL));
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_MTU_PROFILE, INST_SINGLE, mtu_profile));
            /*
             * DBAL_TABLE_EEDB_RIF_BASIC is the only table which has OUT_RIF key, all else are OUT_LIF.
             */
            if (Eedb_out_lif_tables[table_index].dbal_table == DBAL_TABLE_EEDB_RIF_BASIC)
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUT_RIF, &out_lif));
            }
            else
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_OUT_LIF, &out_lif));
            }

            /*
             * Loops through the number of FWD_LAYER_TYPES written in the EEDB outlif_tables array.
             * It retrieves every valid MTU configuration for the valid mtu_profile:cmp_fwd_layer_type combo.
             */
            for (nof_fwd_types = 0;
                 nof_fwd_types < Eedb_out_lif_tables[table_index].nof_fwd_layer_types; nof_fwd_types++)
            {
                sal_memset(&info, 0, sizeof(bcm_switch_control_info_t));
                bcm_rx_mtu_profile_key_t_init(&mtu_key);
                bcm_rx_mtu_profile_value_t_init(&mtu_val);

                key.type = bcmSwitchLinkLayerMtuFilter;
                key.index = Eedb_out_lif_tables[table_index].fwd_layer_type[nof_fwd_types];

                SHR_IF_ERR_EXIT(bcm_switch_control_indexed_get(unit, key, &info));
                mtu_key.cmp_layer_type = info.value;
                mtu_key.mtu_profile = mtu_profile[0];

                SHR_IF_ERR_EXIT(bcm_rx_mtu_profile_get(unit, BCM_RX_MTU_LIF, &mtu_key, &mtu_val));
                if (mtu_val.mtu_val > 0)
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                    if (Eedb_out_lif_tables[table_index].dbal_table == DBAL_TABLE_EEDB_RIF_BASIC)
                    {
                        PRT_CELL_SET("0x%x", out_lif);
                        PRT_CELL_SET("RIF");
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                                        (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, _SHR_CORE_ALL, out_lif,
                                         &gport));

                        PRT_CELL_SET("0x%x", gport);
                        PRT_CELL_SET("LIF");
                    }
                    PRT_CELL_SET("%s(%d)", Eedb_out_lif_tables[table_index].fwd_layer_name[nof_fwd_types], info.value);
                    PRT_CELL_SET("%d", mtu_val.mtu_val);
                    dnx_egress_trap_id_to_string(BCM_GPORT_TRAP_GET_ID(mtu_val.trap_gport), trap_id_str);
                    PRT_CELL_SET("Trap_ID: %s, Fwd_Str: %d, Snp_Str: %d", trap_id_str,
                                 BCM_GPORT_TRAP_GET_STRENGTH(mtu_val.trap_gport),
                                 BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(mtu_val.trap_gport));
                    PRT_CELL_SET("%d", mtu_profile[0]);
                }
            }

            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }

        DBAL_HANDLE_FREE(unit, entry_handle_id);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Print Function for Ports which have MTU enabled. ".
 * \param [in] unit - unit ID
 * \param [in] prt_ctr - print pointer from the main function.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
sh_dnx_trap_mtu_port_print(
    int unit,
    prt_control_t * prt_ctr)
{
    uint32 entry_handle_id;
    uint32 out_port;
    uint32 mtu_profile[1];
    int is_end;
    uint32 field_rule_val = 0;
    char trap_id_str[20] = "";
    bcm_rx_mtu_profile_key_t mtu_key;
    bcm_rx_mtu_profile_value_t mtu_val;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Taking a handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_PP_PORT, &entry_handle_id));

    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
     /** Add KEY rule */
    SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                    (unit, entry_handle_id, DBAL_FIELD_MTU_PROFILE, INST_SINGLE, DBAL_CONDITION_BIGGER_THAN,
                     &field_rule_val, NULL));
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                        (unit, entry_handle_id, DBAL_FIELD_MTU_PROFILE, INST_SINGLE, mtu_profile));
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_PP_PORT, &out_port));

        bcm_rx_mtu_profile_key_t_init(&mtu_key);
        bcm_rx_mtu_profile_value_t_init(&mtu_val);

        mtu_key.mtu_profile = mtu_profile[0];

        SHR_IF_ERR_EXIT(bcm_rx_mtu_profile_get(unit, BCM_RX_MTU_PORT, &mtu_key, &mtu_val));

        if (mtu_val.mtu_val > 0)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("%d", out_port);
            PRT_CELL_SET("PORT");
            PRT_CELL_SET("N/A");
            PRT_CELL_SET("%d", mtu_val.mtu_val);
            dnx_egress_trap_id_to_string(BCM_GPORT_TRAP_GET_ID(mtu_val.trap_gport), trap_id_str);
            PRT_CELL_SET("Trap_ID: %s, Fwd_Str: %d, Snp_Str: %d", trap_id_str,
                         BCM_GPORT_TRAP_GET_STRENGTH(mtu_val.trap_gport),
                         BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(mtu_val.trap_gport));
            PRT_CELL_SET("%d", mtu_profile[0]);
        }

        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Print Function for all MTU option for "trap mtu".
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
sh_dnx_trap_mtu_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("MTU Trap list");

    PRT_COLUMN_ADD("ID");
    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD("LIF Layer type(Compressed)");
    PRT_COLUMN_ADD("MTU Value");
    PRT_COLUMN_ADD("Action Profile");
    PRT_COLUMN_ADD("MTU Profile");

    SHR_IF_ERR_EXIT(sh_dnx_trap_mtu_interfaces_print(unit, prt_ctr));

    SHR_IF_ERR_EXIT(sh_dnx_trap_mtu_port_print(unit, prt_ctr));

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Print Function for all MTU option for "trap mtu".
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_mtu_profile_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_rx_mtu_profile_key_t mtu_key;
    bcm_rx_mtu_profile_value_t mtu_val;
    int mtu_prof_index = 0, cmp_layer_type = 0;
    char trap_id_str[20] = "";
    int flags = BCM_RX_MTU_LIF;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("MTU Profile info");

    PRT_COLUMN_ADD("ID");
    PRT_COLUMN_ADD("MTU Profile");
    PRT_COLUMN_ADD("Compressed Layer Type");
    PRT_COLUMN_ADD("MTU Value");
    PRT_COLUMN_ADD("Action Profile");

    for (mtu_prof_index = 0; mtu_prof_index < 8; mtu_prof_index++)
    {
        for (cmp_layer_type = 0; cmp_layer_type < 8; cmp_layer_type++)
        {
            flags = BCM_RX_MTU_LIF;
            if (cmp_layer_type == 7)
            {
                flags = BCM_RX_MTU_PORT;
            }
            bcm_rx_mtu_profile_key_t_init(&mtu_key);
            bcm_rx_mtu_profile_value_t_init(&mtu_val);

            mtu_key.mtu_profile = mtu_prof_index;
            mtu_key.cmp_layer_type = cmp_layer_type;

            SHR_IF_ERR_EXIT(bcm_rx_mtu_profile_get(unit, flags, &mtu_key, &mtu_val));

            if (mtu_val.mtu_val > 0)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                PRT_CELL_SET("%d", DNX_RX_MTU_ENTRY_INDEX_SET(mtu_prof_index, cmp_layer_type));
                PRT_CELL_SET("%d", mtu_prof_index);
                PRT_CELL_SET("%d", cmp_layer_type);
                PRT_CELL_SET("%d", mtu_val.mtu_val);
                dnx_egress_trap_id_to_string(BCM_GPORT_TRAP_GET_ID(mtu_val.trap_gport), trap_id_str);
                PRT_CELL_SET("Trap_ID: %s, Fwd_Str: %d, Snp_Str: %d", trap_id_str,
                             BCM_GPORT_TRAP_GET_STRENGTH(mtu_val.trap_gport),
                             BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(mtu_val.trap_gport));
            }
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

sh_sand_cmd_t sh_dnx_trap_mtu_cmds[] = {
    /*
     * keyword, action, command, options, man
     */
    {"list", sh_dnx_trap_mtu_cmd, NULL, NULL, &sh_dnx_trap_mtu_cmd_man}
    ,
    {"PROFile", sh_dnx_trap_mtu_profile_cmd, NULL, NULL, &sh_dnx_trap_mtu_profile_cmd_man}
    ,
    {NULL}
};
