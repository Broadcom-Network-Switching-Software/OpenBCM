/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*
 * diag_dnx_trap_lif.c
 *
 *  Created on: APr 11, 2018
 *      Author: sm888907
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAG

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <include/bcm_int/dnx/algo/rx/algo_rx.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_rx_access.h>
#include <include/bcm_int/dnx/rx/rx_trap.h>
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "diag_dnx_trap.h"

/**
* \brief
*    Init callback function for "trap lif" diag commands
*    1) Create an ingress user defined trap
*    2) Create InLIF
*    3) Assign a trap to InLIF created
* \param [in] unit - unit ID
* \return
*   \retval Zero if no error was detected
*   \retval Negative if error was detected. See \ref shr_error_e
*/
static shr_error_e
diag_dnx_trap_lif_init_cb(
    int unit)
{
    bcm_rx_trap_config_t config = { 0 };
    int fwd_strength = 15, snp_strength = 0, trap_id = -1;
    bcm_rx_trap_lif_config_t lif_config;
    bcm_gport_t lif_gport = BCM_GPORT_INVALID;
    bcm_if_t outrif;
    SHR_FUNC_INIT_VARS(unit);

    bcm_rx_trap_config_t_init(&config);

    config.flags = BCM_RX_TRAP_UPDATE_DEST;
    config.dest_port = BCM_GPORT_BLACK_HOLE;

    SHR_IF_ERR_EXIT(bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_id));

    SHR_IF_ERR_EXIT(bcm_rx_trap_set(unit, trap_id, &config));

    SHR_IF_ERR_EXIT(diag_dnx_rx_trap_inlif_create_example(unit, &lif_gport));

    lif_config.lif_type = bcmRxTrapLifTypeInLif;
    lif_config.lif_gport = lif_gport;
    BCM_GPORT_TRAP_SET(lif_config.action_gport, trap_id, fwd_strength, snp_strength);

    SHR_IF_ERR_EXIT(bcm_rx_trap_lif_set(unit, 0, &lif_config));

    SHR_IF_ERR_EXIT(diag_dnx_rx_trap_outrif_create_example(unit, &outrif));

    lif_config.lif_type = bcmRxTrapLifTypeOutRif;
    lif_config.rif_intf = outrif;
    BCM_GPORT_TRAP_SET(lif_config.action_gport, BCM_RX_TRAP_EG_TX_TRAP_ID_DROP, fwd_strength, snp_strength);

    SHR_IF_ERR_EXIT(bcm_rx_trap_lif_set(unit, 0, &lif_config));

exit:
    SHR_FUNC_EXIT;
}

sh_sand_man_t sh_dnx_trap_lif_cmd_man = {
    .brief = "Show LIF traps information\n",
    .full = "Lists of all LIF traps configured \n",
    .init_cb = diag_dnx_trap_lif_init_cb
};

const sh_sand_enum_t Lif_trap_type_enum_table[] = {
    /**String      Value                                            Description*/
    {"InLIF", bcmRxTrapLifTypeInLif, "LIF type InLIF"}
    ,
    {"InRIF", bcmRxTrapLifTypeInRif, "LIF type InRIF"}
    ,
    {"OutLIF", bcmRxTrapLifTypeOutLif, "LIF type OutLIF"}
    ,
    {"OutRIF", bcmRxTrapLifTypeOutRif, "LIF type OutRIF"}
    ,
    {NULL}
};

const sh_dnx_trap_lif_table_info_t In_lif_tables[] = {
    {DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION, DBAL_FIELD_IN_LIF},
    {DBAL_TABLE_IN_LIF_FORMAT_LSP, DBAL_FIELD_IN_LIF},
    {DBAL_TABLE_IN_LIF_FORMAT_PWE, DBAL_FIELD_IN_LIF},
    {DBAL_TABLE_IN_LIF_FORMAT_EVPN, DBAL_FIELD_IN_LIF},
    {DBAL_TABLE_IN_LIF_IPvX_TUNNELS, DBAL_FIELD_IN_LIF},
    {DBAL_TABLE_IN_AC_INFO_DB, DBAL_FIELD_IN_LIF},
};

const sh_dnx_trap_lif_table_info_t Out_lif_tables[] = {
    {DBAL_TABLE_EEDB_ARP, DBAL_FIELD_OUT_LIF},
    {DBAL_TABLE_EEDB_IPV4_TUNNEL, DBAL_FIELD_OUT_LIF},
    {DBAL_TABLE_EEDB_IPV6_TUNNEL, DBAL_FIELD_OUT_LIF},
    {DBAL_TABLE_EEDB_MPLS_TUNNEL, DBAL_FIELD_OUT_LIF},
    {DBAL_TABLE_EEDB_PWE, DBAL_FIELD_OUT_LIF},
    {DBAL_TABLE_EEDB_EVPN, DBAL_FIELD_OUT_LIF},
    {DBAL_TABLE_EEDB_OUT_AC, DBAL_FIELD_OUT_LIF},
    {DBAL_TABLE_ESEM_DEFAULT_RESULT_TABLE, DBAL_FIELD_ESEM_DEFAULT_RESULT_PROFILE},
    {DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB,
     DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB_ETPS_AC_STAT},
    {DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB, DBAL_RESULT_TYPE_ESEM_FORWARD_DOMAIN_MAPPING_DB_ETPS_AC_STAT},
    {DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB, DBAL_RESULT_TYPE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB_ETPS_AC_STAT},
};

const sh_dnx_trap_lif_table_info_t Out_rif_tables[] = {
    {DBAL_TABLE_EEDB_ARP, DBAL_FIELD_OUT_LIF},
    {DBAL_TABLE_EEDB_IPV4_TUNNEL, DBAL_FIELD_OUT_LIF},
    {DBAL_TABLE_EEDB_IPV6_TUNNEL, DBAL_FIELD_OUT_LIF},
    {DBAL_TABLE_EEDB_MPLS_TUNNEL, DBAL_FIELD_OUT_LIF},
    {DBAL_TABLE_EEDB_PWE, DBAL_FIELD_OUT_LIF},
    {DBAL_TABLE_EEDB_EVPN, DBAL_FIELD_OUT_LIF},
    {DBAL_TABLE_EEDB_OUT_AC, DBAL_FIELD_OUT_LIF},
    {DBAL_TABLE_EEDB_RIF_BASIC, DBAL_FIELD_OUT_RIF},
};

/**
 * \brief
 *    Info for configuration according to LIF type  
 * \param [in] unit - unit ID
 * \param [in] lif_type - lif type
 * \param [out] dbal_tables_arr_p - array of dbal tables to check
 * \param [out] nof_tables_p - number of lif tables 
 * \param [out] lif_profile_field_type_p - lif profile dbal field: ACTION_PROFILE_IDX(ingress) or ACTION_PROFILE(egress)
 * \param [out] algo_gpm_flag_p - algo gpm flag: LOCAL_LIF_INGRESS or LOCAL_LIF_EGRESS 
 * \param [out] is_rif_p - indication if LIF type is RIF 
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnx_trap_lif_type_to_info(
    int unit,
    bcm_rx_trap_lif_type_t lif_type,
    sh_dnx_trap_lif_table_info_t ** dbal_tables_arr_p,
    uint32 *nof_tables_p,
    dbal_fields_e * lif_profile_field_type_p,
    int *algo_gpm_flag_p,
    uint8 *is_rif_p)
{
    uint32 size_of_entry = sizeof(sh_dnx_trap_lif_table_info_t);
    SHR_FUNC_INIT_VARS(unit);

    switch (lif_type)
    {
        case bcmRxTrapLifTypeInLif:
            *dbal_tables_arr_p = (sh_dnx_trap_lif_table_info_t *) & In_lif_tables;
            *nof_tables_p = sizeof(In_lif_tables) / size_of_entry;
            *lif_profile_field_type_p = DBAL_FIELD_ACTION_PROFILE_IDX;
            *algo_gpm_flag_p = DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS;
            *is_rif_p = FALSE;
            break;
        case bcmRxTrapLifTypeInRif:
            *dbal_tables_arr_p = (sh_dnx_trap_lif_table_info_t *) & In_lif_tables;
            *nof_tables_p = sizeof(In_lif_tables) / size_of_entry;
            *lif_profile_field_type_p = DBAL_FIELD_ACTION_PROFILE_IDX;
            *algo_gpm_flag_p = DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS;
            *is_rif_p = TRUE;
            break;
        case bcmRxTrapLifTypeOutLif:
            *dbal_tables_arr_p = (sh_dnx_trap_lif_table_info_t *) & Out_lif_tables;
            *nof_tables_p = sizeof(Out_lif_tables) / size_of_entry;
            *lif_profile_field_type_p = DBAL_FIELD_ACTION_PROFILE;
            *algo_gpm_flag_p = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
            *is_rif_p = FALSE;
            break;
        case bcmRxTrapLifTypeOutRif:
            *dbal_tables_arr_p = (sh_dnx_trap_lif_table_info_t *) & Out_rif_tables;
            *nof_tables_p = sizeof(Out_rif_tables) / size_of_entry;
            *lif_profile_field_type_p = DBAL_FIELD_ACTION_PROFILE;
            *algo_gpm_flag_p = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
            *is_rif_p = TRUE;
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid type %d given as input\n", lif_type);
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    A diagnostics function that prints a list of LIFs/RIFs for a given lif_type
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
shr_error_e
sh_dnx_trap_lif_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_gport_t gport = 0;
    bcm_if_t rif = 0;
    uint8 is_rif = 0;
    uint32 entry_handle_id = 0, local_lif = 0, lif_profile = 0, nof_tables = 0, field_rule_val = 0;
    int table_index = 0, is_end = 0, algo_gpm_flag = 0, trap_id = -1;
    char *lif_name = NULL;
    uint32 vsi = 0, vlan_domain = 0;
    uint32 local_out_lif = 0, port;
    sh_dnx_trap_lif_table_info_t *dbal_tables_arr = NULL;
    dbal_fields_e lif_profile_field_type = DBAL_FIELD_EMPTY;
    bcm_rx_trap_lif_type_t lif_type;
    bcm_rx_trap_lif_config_t lif_config;
    char trap_id_str[20] = "";
    bcm_vlan_port_t vlan_port;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("List of LIFs configured");
    PRT_COLUMN_ADD("LIF type");
    PRT_COLUMN_ADD("LIF ID (gport/intf)");
    PRT_COLUMN_ADD("Mirror cmd");
    PRT_COLUMN_ADD("Trap ID");
    PRT_COLUMN_ADD("Fwd Strength");
    PRT_COLUMN_ADD("Snp Strength");

    for (lif_type = bcmRxTrapLifTypeInLif; lif_type < bcmRxTrapLifTypeCount; lif_type++)
    {
        SHR_IF_ERR_EXIT(sh_dnx_trap_lif_type_to_info(unit, lif_type, &dbal_tables_arr, &nof_tables,
                                                     &lif_profile_field_type, &algo_gpm_flag, &is_rif));

        lif_name = Lif_trap_type_enum_table[lif_type].string;
        bcm_rx_trap_lif_config_t_init(&lif_config);
        lif_config.lif_type = lif_type;

        for (table_index = 0; table_index < nof_tables; table_index++)
        {
            /** Taking a handle */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_tables_arr[table_index].dbal_table, &entry_handle_id));
            SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));
            /** Add KEY rule */
            SHR_IF_ERR_EXIT(dbal_iterator_value_field_arr32_rule_add
                            (unit, entry_handle_id, lif_profile_field_type, INST_SINGLE, DBAL_CONDITION_BIGGER_THAN,
                             &field_rule_val, NULL));
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
            while (!is_end)
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                (unit, entry_handle_id, lif_profile_field_type, INST_SINGLE, &lif_profile));
                if (dbal_tables_arr[table_index].dbal_table ==
                    DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_PER_INTERFACE_NAMESPACE_DB)
                {
                     /** Set keys: */
                    dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_VSI, &vsi);
                    dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_OUT_LIF, &local_out_lif);

                    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                                    (unit, DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS, _SHR_CORE_ALL,
                                     local_out_lif, &gport));
                }
                else if (dbal_tables_arr[table_index].dbal_table == DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB)
                {
                    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                    (unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, &vlan_domain));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_VSI, &vsi));

                    bcm_vlan_port_t_init(&vlan_port);

                    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
                    vlan_port.vsi = vsi;
                    vlan_port.port = vlan_domain;
                    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;

                    SHR_IF_ERR_EXIT(bcm_vlan_port_find(unit, &vlan_port));

                    gport = vlan_port.vlan_port_id;
                }
                else if (dbal_tables_arr[table_index].dbal_table == DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB)
                {
                    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                    (unit, entry_handle_id, DBAL_FIELD_VLAN_DOMAIN, &vlan_domain));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                    (unit, entry_handle_id, DBAL_FIELD_SYSTEM_PORT_AGGREGATE, &port));

                    bcm_vlan_port_t_init(&vlan_port);
                    BCM_GPORT_SYSTEM_PORT_ID_SET(vlan_port.port, port);
                    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_PORT;
                    vlan_port.match_class_id = vlan_domain;
                    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;

                    SHR_IF_ERR_EXIT(bcm_vlan_port_find(unit, &vlan_port));

                    gport = vlan_port.vlan_port_id;
                }
                else
                {
                    SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                                    (unit, entry_handle_id, dbal_tables_arr[table_index].lif_field_type, &local_lif));
                }

                if (is_rif && (dbal_tables_arr[table_index].lif_field_type == DBAL_FIELD_OUT_RIF))
                {
                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                    PRT_CELL_SET("%s", lif_name);
                    PRT_CELL_SET("0x%x", local_lif);
                    lif_config.rif_intf = local_lif;
                }
                else
                {
                    if (dbal_tables_arr[table_index].lif_field_type != DBAL_FIELD_ESEM_DEFAULT_RESULT_PROFILE &&
                        dbal_tables_arr[table_index].dbal_table != DBAL_TABLE_ESEM_FORWARD_DOMAIN_MAPPING_DB &&
                        dbal_tables_arr[table_index].dbal_table != DBAL_TABLE_ESEM_PORT_BASED_SOURCE_ADDRESS_DB)
                    {
                        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_from_lif
                                        (unit, algo_gpm_flag, _SHR_CORE_ALL, local_lif, &gport));
                    }

                    if (is_rif && BCM_GPORT_IS_TUNNEL(gport))
                    {
                        BCM_GPORT_TUNNEL_TO_L3_ITF_LIF(rif, gport);
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                        PRT_CELL_SET("%s", lif_name);
                        PRT_CELL_SET("0x%x", rif);
                        lif_config.rif_intf = rif;
                    }
                    else if (!is_rif)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                        PRT_CELL_SET("%s", lif_name);
                        PRT_CELL_SET("0x%x", gport);
                        lif_config.lif_gport = gport;
                    }
                    else
                    {
                        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
                        continue;
                    }
                }

                SHR_IF_ERR_EXIT(bcm_rx_trap_lif_get(unit, &lif_config));

                trap_id = BCM_GPORT_TRAP_GET_ID(lif_config.action_gport);

                if (BCM_GPORT_IS_TRAP(lif_config.action_gport))
                {
                    dnx_egress_trap_id_to_string(trap_id, trap_id_str);
                    PRT_CELL_SET("N/A");
                    PRT_CELL_SET("%s", trap_id_str);
                    PRT_CELL_SET("%d", BCM_GPORT_TRAP_GET_STRENGTH(lif_config.action_gport));
                    PRT_CELL_SET("%d", BCM_GPORT_TRAP_GET_SNOOP_STRENGTH(lif_config.action_gport));
                }
                else
                {
                    PRT_CELL_SET("%d", BCM_GPORT_MIRROR_GET(lif_config.action_gport));
                    PRT_CELL_SET("N/A");
                    PRT_CELL_SET("N/A");
                    PRT_CELL_SET("N/A");
                }

                SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
            }

            SHR_IF_ERR_EXIT(DBAL_HANDLE_FREE(unit, entry_handle_id));
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
