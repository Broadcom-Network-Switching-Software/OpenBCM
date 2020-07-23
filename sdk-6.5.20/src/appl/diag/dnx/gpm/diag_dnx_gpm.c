/** \file diag_dnx_gpm.c
 *
 * Diagnostics for the allocation managers.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_PORT

/*************
 * INCLUDES  *
 */
/*shared*/
#include <shared/bsl.h>
/*appl*/
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/shell.h>
/*bcm*/
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/algo/l3/algo_l3.h>
#include <bcm_int/dnx/l3/l3_fec.h>
#include <bcm_int/dnx/port/port_pp.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
/*soc*/
#include <soc/dnx/dbal/dbal_string_mgmt.h>
/*sal*/
#include <sal/appl/sal.h>

/*************
 *  DEFINES  *
 */

#define CHAR_BUFF_SIZE 500

/**External function declaration */
extern const char *strcaseindex(
    const char *s,
    const char *sub);

/*************
 *  MACROES  *
 */

/*************
 * FUNCTIONS *
 */

/**
 * \brief -
 *  Get the HW resource information accroding to the given gport or local_lif.
 *
 * \param [in] unit - Identifier of the device to access.
 * \param [in] gport - gport ID;
 * \param [in] local_lif - Local in/out lif ID, making sense only when gport is invalid;
 * \param [in] lif_flags - Flags to indicate the lif properties.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
static shr_error_e
sh_dnx_gport_to_hw_resources_cmd_internal(
    int unit,
    int gport,
    int local_lif,
    uint32 *lif_flags)
{
    dnx_algo_gpm_gport_hw_resources_t fec_hw_resources, lif_hw_resources;
    uint32 flags;
    uint8 is_ingress, is_egress, is_egress_virtual;
    char shell_commands_buffer[CHAR_BUFF_SIZE];
    char *dbal_entry_get_command = "dbal eNTry get table=";

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    sal_memset(&fec_hw_resources, 0, sizeof(fec_hw_resources));
    sal_memset(&lif_hw_resources, 0, sizeof(lif_hw_resources));

    is_ingress = _SHR_IS_FLAG_SET(lif_flags[0], DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS |
                                  DNX_ALGO_GPM_GPORT_HW_RESOURCES_SBC_LOCAL_LIF_INGRESS |
                                  DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS);
    is_egress = _SHR_IS_FLAG_SET(lif_flags[0], DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS |
                                 DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS);
    is_egress_virtual = FALSE;

    /*
     * First, find the fec / lif resources for this gport.
     * Use NON_STRICT because we don't know which resources will be valid.
     */
    if (gport != BCM_GPORT_INVALID)
    {
        /** Retrieve the info per gport */
        flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_FEC | DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT;
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, flags, &fec_hw_resources));

        flags = DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_AND_GLOBAL_LIF |
                DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT |
                DNX_ALGO_GPM_GPORT_HW_RESOURCES_VIRTUAL_EGRESS;
        SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_to_hw_resources(unit, gport, flags, &lif_hw_resources));

        is_egress_virtual = BCM_GPORT_IS_VLAN_PORT(gport) && 
                            (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(gport) ||
                             BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_DEFAULT(gport));
    }
    else
    {
        /** Retrieve the information per local LIF */
        dbal_physical_tables_e dbal_physical_table;
        int rv;

        fec_hw_resources.fec_id = DNX_ALGO_GPM_FEC_INVALID;
        lif_hw_resources.local_in_lif = DNX_ALGO_GPM_LIF_INVALID;
        lif_hw_resources.local_out_lif = DNX_ALGO_GPM_LIF_INVALID;

        if (is_ingress)
        {
            lif_hw_resources.local_in_lif = local_lif;
            lif_hw_resources.local_in_lif_core = _SHR_CORE_ALL;
            if (_SHR_IS_FLAG_SET(lif_flags[0], DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS))
            {
                dbal_physical_table = DBAL_PHYSICAL_TABLE_INLIF_2;
            }
            else
            {
                dbal_physical_table = DBAL_PHYSICAL_TABLE_INLIF_1;
            }

            rv = dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get(unit, local_lif, _SHR_CORE_ALL,
                                                                         dbal_physical_table,
                                                                         &(lif_hw_resources.inlif_dbal_table_id),
                                                                         &(lif_hw_resources.inlif_dbal_result_type),
                                                                         NULL);
            if ((rv == _SHR_E_NOT_FOUND)
                && (_SHR_IS_FLAG_SET(lif_flags[1], DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS)))
            {
                rv = dnx_lif_mngr_inlif_sw_info_table_id_and_result_type_get(unit, local_lif, _SHR_CORE_ALL,
                                                                             DBAL_PHYSICAL_TABLE_INLIF_2,
                                                                             &(lif_hw_resources.inlif_dbal_table_id),
                                                                             &(lif_hw_resources.inlif_dbal_result_type),
                                                                             NULL);
            }
            SHR_IF_ERR_EXIT(rv);
        }

        if (is_egress)
        {
            lif_hw_resources.local_out_lif = local_lif;
            SHR_IF_ERR_EXIT(dnx_lif_mngr_outlif_sw_info_get(unit, local_lif, &(lif_hw_resources.outlif_dbal_table_id),
                                                            &(lif_hw_resources.outlif_dbal_result_type),
                                                            &(lif_hw_resources.outlif_phase), NULL, NULL, NULL));
        }
    }

    /*
     * If none are found, return error.
     * There are currently no gports that have a global lif and no local lif, but there are gports that only have local lif,
     * so we don't check the global lif's existence.
     */
    if (fec_hw_resources.fec_id == DNX_ALGO_GPM_FEC_INVALID
        && lif_hw_resources.local_in_lif == DNX_ALGO_GPM_LIF_INVALID
        && lif_hw_resources.local_out_lif == DNX_ALGO_GPM_LIF_INVALID
        && !is_egress_virtual)
    {
        SHR_CLI_EXIT(_SHR_E_NONE, "Given gport 0x%08x has no hardware resources or doesn't exist.\n", gport);
    }

    /*
     * Print fec information, if it's valid.
     */
    if (fec_hw_resources.fec_id != DNX_ALGO_GPM_FEC_INVALID)
    {
        uint32 fec_hierarchy;
        dbal_tables_e fec_dbal_table;
        LOG_CLI((BSL_META("FEC info:\n")));

        SHR_IF_ERR_EXIT(algo_l3_fec_hierarchy_stage_map_get(unit, fec_hw_resources.fec_id, &fec_hierarchy));

        LOG_CLI((BSL_META("gport:0x%08X -> FEC ID:%d -> hierarchy: %d\n"), gport, fec_hw_resources.fec_id,
                 fec_hierarchy + 1));

        /*
         * Print fec dbal information, using the dbal entry get shell command.
         */
        fec_dbal_table = L3_FEC_TABLE_FROM_HIERARCHY_GET(fec_hierarchy);

        sal_memset(shell_commands_buffer, 0, CHAR_BUFF_SIZE);

        sal_snprintf(shell_commands_buffer, CHAR_BUFF_SIZE, "%s%s super_fec_id=%d",
                     dbal_entry_get_command,
                     dbal_logical_table_to_string(unit, fec_dbal_table),
                     DNX_ALGO_L3_FEC_ID_TO_SUPER_FEC_ID(fec_hw_resources.fec_id));
        SHR_IF_ERR_EXIT(sh_process_command(unit, shell_commands_buffer));

        LOG_CLI((BSL_META("\n\n")));
    }

    /*
     * Print inlif information, if it's valid.
     */
    if (is_ingress)
    {
        if (lif_hw_resources.local_in_lif != DNX_ALGO_GPM_LIF_INVALID)
        {
            const char *logical_table_str = dbal_logical_table_to_string(unit, lif_hw_resources.inlif_dbal_table_id);
            const char *result_type_str = dbal_result_type_to_string(unit, lif_hw_resources.inlif_dbal_table_id,
                                                                     lif_hw_resources.inlif_dbal_result_type);
            LOG_CLI((BSL_META("INLIF info:\n")));

            if (gport != BCM_GPORT_INVALID)
            {
                LOG_CLI((BSL_META("gport:0x%08X -> "), gport));
            }
            else
            {
                LOG_CLI((BSL_META("gport: N/A -> ")));
            }

            if (lif_hw_resources.global_in_lif != DNX_ALGO_GPM_LIF_INVALID)
            {
                LOG_CLI((BSL_META("Global inlif:0x%08X -> "), lif_hw_resources.global_in_lif));
            }
            else
            {
                LOG_CLI((BSL_META("Global inlif: N/A -> ")));
            }

            LOG_CLI((BSL_META("Local inlif:0x%08X\n"), lif_hw_resources.local_in_lif));

            LOG_CLI((BSL_META("Table: %s"), logical_table_str));

            /*
             * Print inlif dbal information, using the dbal entry get shell command.
             */
            sal_memset(shell_commands_buffer, 0, CHAR_BUFF_SIZE);

            sal_snprintf(shell_commands_buffer, CHAR_BUFF_SIZE, "%s%s core_id=%d in_lif=%d result_type=%s",
                         dbal_entry_get_command,
                         logical_table_str,
                         (lif_hw_resources.local_in_lif_core == _SHR_CORE_ALL) ? 0 : lif_hw_resources.local_in_lif_core,
                         lif_hw_resources.local_in_lif, result_type_str);
            SHR_IF_ERR_EXIT(sh_process_command(unit, shell_commands_buffer));
        }
        else
        {
            LOG_CLI((BSL_META("No INLIF information for gport 0x%08X.\n"), gport));
        }

        LOG_CLI((BSL_META("\n\n")));
    }

    if (is_egress)
    {
        /*
         * Print outlif information, if it's valid.
         */
        if (lif_hw_resources.local_out_lif != DNX_ALGO_GPM_LIF_INVALID)
        {
            const char *logical_table_str = dbal_logical_table_to_string(unit, lif_hw_resources.outlif_dbal_table_id);
            const char *result_type_str = dbal_result_type_to_string(unit, lif_hw_resources.outlif_dbal_table_id,
                                                                     lif_hw_resources.outlif_dbal_result_type);
            LOG_CLI((BSL_META("OUTLIF info:\n")));

            if (gport != BCM_GPORT_INVALID)
            {
                LOG_CLI((BSL_META("gport:0x%08X -> "), gport));
            }
            else
            {
                LOG_CLI((BSL_META("gport: N/A -> ")));
            }

            if (lif_hw_resources.global_out_lif != DNX_ALGO_GPM_LIF_INVALID)
            {
                LOG_CLI((BSL_META("Global outlif:0x%08X -> "), lif_hw_resources.global_out_lif));
            }
            else
            {
                LOG_CLI((BSL_META("Global outlif: N/A -> ")));
            }

            LOG_CLI((BSL_META("Local outlif:0x%08X -> phase:%d\n"), lif_hw_resources.local_out_lif,
                     lif_hw_resources.outlif_phase + 1));
            LOG_CLI((BSL_META("Table: %s"), logical_table_str));
            /*
             * Print outlif dbal information, using the dbal entry get shell command.
             */
            sal_memset(shell_commands_buffer, 0, CHAR_BUFF_SIZE);

            sal_snprintf(shell_commands_buffer, CHAR_BUFF_SIZE, "%s%s out_lif=%d result_type=%s",
                         dbal_entry_get_command, logical_table_str, lif_hw_resources.local_out_lif, result_type_str);
            SHR_IF_ERR_EXIT(sh_process_command(unit, shell_commands_buffer));
        }
        /*
         * Print ESEM information, if it's valid.
         */
        else if (is_egress_virtual)
        {
            uint32 sw_table_handle_id, res_type;
            const char *logical_table_str = dbal_logical_table_to_string(unit, lif_hw_resources.outlif_dbal_table_id);

            LOG_CLI((BSL_META("ESEM info:\n")));
            LOG_CLI((BSL_META("gport:0x%08X -> "), gport));
            if (BCM_GPORT_SUB_TYPE_IS_VIRTUAL_EGRESS_MATCH(gport))
            {
                LOG_CLI((BSL_META("Virtual Handle:0x%08X \n"), BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_MATCH_GET(gport)));
            }
            else
            {
                LOG_CLI((BSL_META("Virtual Handle:0x%08X \n"), BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(gport)));
            }
            LOG_CLI((BSL_META("Table: %s"), logical_table_str));
            sal_memset(shell_commands_buffer, 0, CHAR_BUFF_SIZE);

            /** Retrieve outlif and VSI from sw state */
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, &sw_table_handle_id));

            dbal_entry_key_field32_set(unit, sw_table_handle_id, DBAL_FIELD_GPORT, gport);
            SHR_IF_ERR_EXIT(dbal_entry_get(unit, sw_table_handle_id, DBAL_GET_ALL_FIELDS));
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                            (unit, sw_table_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, &res_type));

            switch (res_type)
            {
                case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NATIVE_AC:
                {
                    uint32 vsi, local_out_lif, vlan_domain;
                    uint32 is_native_intf_name_space;

                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, sw_table_handle_id, DBAL_FIELD_LOCAL_OUT_LIF, INST_SINGLE, &local_out_lif));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, sw_table_handle_id, DBAL_FIELD_FODO_VSI, INST_SINGLE, &vsi));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, sw_table_handle_id, DBAL_FIELD_IS_LIF_SCOPE, INST_SINGLE, &is_native_intf_name_space));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, sw_table_handle_id, DBAL_FIELD_VLAN_DOMAIN, INST_SINGLE, &vlan_domain));

                    if (is_native_intf_name_space)
                    {
                        sal_snprintf(shell_commands_buffer, CHAR_BUFF_SIZE, "%s%s FODO=%d OUT_LIF=%d",
                                     dbal_entry_get_command, logical_table_str, vsi, local_out_lif);
                    }
                    else
                    {

                        sal_snprintf(shell_commands_buffer, CHAR_BUFF_SIZE, "%s%s FODO=%d VLAN_DOMAIN=%d C_VID=%d",
                                     dbal_entry_get_command, logical_table_str, vsi, vlan_domain, 0);
                    }
                    break;
                }
                case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_AC:
                {
                    uint32 vsi, port, vlan_domain, c_vid;
                
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, sw_table_handle_id, DBAL_FIELD_FODO_VSI, INST_SINGLE, &vsi));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, sw_table_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &port));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, sw_table_handle_id, DBAL_FIELD_OUTER_VLAN_ID, INST_SINGLE, &c_vid));

                    /** Get port's vlan_domain */
                    SHR_IF_ERR_EXIT(dnx_port_pp_vlan_domain_get(unit, port, &vlan_domain));

                    sal_snprintf(shell_commands_buffer, CHAR_BUFF_SIZE, "%s%s FODO=%d VLAN_DOMAIN=%d C_VID=%d",
                                 dbal_entry_get_command, logical_table_str, vsi, vlan_domain, c_vid);
  
                    break;
                }
                case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_VSI:
                {
                    uint32 vsi, match_class_id;
                
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, sw_table_handle_id, DBAL_FIELD_FODO_VSI, INST_SINGLE, &vsi));

                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, sw_table_handle_id, DBAL_FIELD_NETWORK_DOMAIN, INST_SINGLE, &match_class_id));

                    sal_snprintf(shell_commands_buffer, CHAR_BUFF_SIZE, "%s%s FODO=%d VLAN_DOMAIN=%d C_VID=%d",
                                 dbal_entry_get_command, logical_table_str, vsi, match_class_id, 0);

                    break;
                }
                case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_NAMESPACE_PORT:
                {
                    uint32 port, vlan_domain;
                    dnx_algo_gpm_gport_phy_info_t gport_phy_info;

                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, sw_table_handle_id, DBAL_FIELD_GPORT, INST_SINGLE, &port));
                    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get
                                    (unit, sw_table_handle_id, DBAL_FIELD_NAMESPACE, INST_SINGLE, &vlan_domain));

                    SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                                    (unit, port, DNX_ALGO_GPM_GPORT_TO_PHY_OP_RETRIVE_SYS_PORT, &gport_phy_info));

                    sal_snprintf(shell_commands_buffer, CHAR_BUFF_SIZE, "%s%s SYSTEM_PORT_AGGREGATE=%d VLAN_DOMAIN=%d",
                                dbal_entry_get_command, logical_table_str, gport_phy_info.sys_port, vlan_domain);

                    break;
                }
                case DBAL_RESULT_TYPE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW_ESEM_DEFAULT:
                {
                    sal_snprintf(shell_commands_buffer, CHAR_BUFF_SIZE, "%s%s ESEM_DEFAULT_RESULT_PROFILE=%d",
                             dbal_entry_get_command, logical_table_str, BCM_GPORT_SUB_TYPE_VIRTUAL_EGRESS_DEFAULT_GET(gport));
                    break;
                }
                default:
                    SHR_ERR_EXIT(_SHR_E_INTERNAL, "Error!, accessing ESM SW DB table (%d) with the key gport = 0x%08X but"
                                 " resultType = %d is not supported.\n",
                                 DBAL_TABLE_EGRESS_VIRTUAL_GPORT_TO_MATCH_INFO_SW, gport, res_type);
            }
            SHR_IF_ERR_EXIT(sh_process_command(unit, shell_commands_buffer));
        }
        else
        {
            LOG_CLI((BSL_META("No OUTLIF information for gport 0x%08X.\n"), gport));
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_gport_to_hw_resources_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int gport;
    uint32 flags[2] = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("gport", gport);

    flags[0] = DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS | DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
    SHR_IF_ERR_EXIT(sh_dnx_gport_to_hw_resources_cmd_internal(unit, gport, DNX_ALGO_GPM_LIF_INVALID, flags));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_lif_to_hw_resources_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int rv;
    int gport, lif, is_global_lif;
    int is_ingress, is_egress, is_dpc;
    char *direction;
    uint32 lif_flags[2] = { 0 };
    uint8 iter_for_lif = 1, iter_id;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("id", lif);
    SH_SAND_GET_BOOL("global", is_global_lif);
    SH_SAND_GET_STR("type", direction);
    SH_SAND_GET_INT32("dpc", is_dpc);

    is_ingress = strcaseindex(direction, "i") || strcaseindex(direction, "b");
    is_egress = strcaseindex(direction, "o") || strcaseindex(direction, "b");

    if ((is_global_lif || is_egress) && (is_dpc != -1))
    {
        LOG_CLI((BSL_META("DPC input (%d) is ignored for non ingress local LIF case!\n"), is_dpc));
    }

    gport = BCM_GPORT_INVALID;
    lif_flags[0] = lif_flags[1] = DNX_ALGO_GPM_GPORT_HW_RESOURCES_NON_STRICT;
    if (is_ingress)
    {
        if (is_global_lif)
        {
            lif_flags[0] |= DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_INGRESS;
        }
        else if (is_dpc == -1)
        {
            iter_for_lif = 2;
            lif_flags[0] |= DNX_ALGO_GPM_GPORT_HW_RESOURCES_SBC_LOCAL_LIF_INGRESS;
            lif_flags[1] |= DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS;
        }
        else if (is_dpc == 0)
        {
            lif_flags[0] |= DNX_ALGO_GPM_GPORT_HW_RESOURCES_SBC_LOCAL_LIF_INGRESS;
        }
        else
        {
            lif_flags[0] |= DNX_ALGO_GPM_GPORT_HW_RESOURCES_DPC_LOCAL_LIF_INGRESS;
        }
    }
    else if (is_egress)
    {
        if (is_global_lif)
        {
            lif_flags[0] |= DNX_ALGO_GPM_GPORT_HW_RESOURCES_GLOBAL_LIF_EGRESS;
        }
        else
        {
            lif_flags[0] |= DNX_ALGO_GPM_GPORT_HW_RESOURCES_LOCAL_LIF_EGRESS;
        }
    }
    else
    {
        SHR_CLI_EXIT(_SHR_E_NONE, "Legal options for lif direction TYpe: in,out,both.\n");
    }

    for (iter_id = 0; iter_id < iter_for_lif; iter_id++)
    {
        rv = dnx_algo_gpm_gport_from_lif(unit, lif_flags[iter_id], _SHR_CORE_ALL, lif, &gport);
        if ((rv == _SHR_E_NONE) && (gport != BCM_GPORT_INVALID))
        {
            break;
        }
        else
        {
            SHR_IF_ERR_EXIT_EXCEPT_IF(rv, _SHR_E_NOT_FOUND);
        }
    }

    if (gport == BCM_GPORT_INVALID)
    {
        if (is_global_lif)
        {
            SHR_CLI_EXIT(_SHR_E_NONE, "Failed to translate %sGlobal LIF to gport, given LIF(0x%08X) may not exist.\n",
                         (is_ingress && !is_egress) ? "ingress " : ((!is_ingress && is_egress) ? "egress " : ""), lif);
        }
        else
        {
            LOG_CLI((BSL_META("Failed to translate %sLocal LIF to gport, given LIF(0x%08X) is used for table info.\n"),
                     (is_ingress && !is_egress) ? "ingress " : ((!is_ingress && is_egress) ? "egress " : ""), lif));
        }
    }

    SHR_IF_ERR_EXIT(sh_dnx_gport_to_hw_resources_cmd_internal(unit, gport, lif, lif_flags));

exit:
    SHR_FUNC_EXIT;
}

#define DIAG_DNX_VLAN_PORT_ID 0x44804572

/*
 * Create a simple vlan port for the diagnostic.
 */
shr_error_e
sh_dnx_gport_diagnostics_init(
    int unit)
{
    bcm_vlan_port_t vp;
    SHR_FUNC_INIT_VARS(unit);

    bcm_vlan_port_t_init(&vp);
    vp.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vp.flags = BCM_VLAN_PORT_WITH_ID;
    vp.vlan_port_id = DIAG_DNX_VLAN_PORT_ID;

    SHR_IF_ERR_EXIT(bcm_vlan_port_create(unit, &vp));

exit:
    SHR_FUNC_EXIT;
}

/*
 * Destroy the simple vlan port for the diagnostic.
 */
shr_error_e
sh_dnx_gport_diagnostics_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_vlan_port_destroy(unit, DIAG_DNX_VLAN_PORT_ID));

exit:
    SHR_FUNC_EXIT;
}

/**
 * List of the supported commands, pointer to command function and command usage function.
 */

static sh_sand_man_t sh_dnx_gport_to_hw_resources_man = {
    .brief = "Given a gport, display the hw resources related to it: Global and local lif, and fec.",
    .full =
        "All hw resources connected to this gport will be displayed. Non existing resources will not be displayed.\n"
        "A full dbal dump of each resource will also be displayed.",
    .init_cb = sh_dnx_gport_diagnostics_init,
    .deinit_cb = sh_dnx_gport_diagnostics_deinit,
    .examples = "gport=0x44804572"
};

static sh_sand_option_t sh_dnx_gport_to_hw_resources_options[] = {
    {"gport", SAL_FIELD_TYPE_INT32, "gport.", NULL, NULL, NULL, SH_SAND_ARG_QUIET},
    {NULL}
};

static sh_sand_man_t sh_dnx_lif_to_hw_resources_man = {
    .brief = "Given a lif, display its gport and the hw resources related to it: Global and local lif.",
    .full =
        "All hw resources connected to this lif will be displayed. Non existing resources will not be displayed.\n"
        "A full dbal dump of each resource will also be displayed.",
    .init_cb = sh_dnx_gport_diagnostics_init,
    .deinit_cb = sh_dnx_gport_diagnostics_deinit,
    .examples = "id=0x1000 type=out global=true"
};

static sh_sand_option_t sh_dnx_lif_to_hw_resources_options[] = {
    {"ID", SAL_FIELD_TYPE_INT32, "lif id.", NULL, NULL, NULL, 0},
    {"GLOBal", SAL_FIELD_TYPE_BOOL, "global lif indication.", "1", NULL, NULL, 0},
    {"TYpe", SAL_FIELD_TYPE_STR, "lif direction: in, out or both.", "out", NULL, NULL, 0},
    {"DPC", SAL_FIELD_TYPE_INT32, "DPC indication(0: SBC; 1: DPC; -1: Ignore), valid for ingress local LIF only. "
     "If DPC, core id is expected, or leave empty for all cores. "
     "If Ignore, SDK looks for the inLIF with SBC first and DPC if fails.",
     "-1", NULL, NULL, SH_SAND_ARG_QUIET},
    {NULL}
};

sh_sand_cmd_t sh_dnx_gpm_cmds[] = {
    /*
     * keyword, action, command, options, man
     */
    {"HW", sh_dnx_gport_to_hw_resources_cmd, NULL, sh_dnx_gport_to_hw_resources_options,
     &sh_dnx_gport_to_hw_resources_man},
    {"LIF", sh_dnx_lif_to_hw_resources_cmd, NULL, sh_dnx_lif_to_hw_resources_options,
     &sh_dnx_lif_to_hw_resources_man},
    {NULL}
};

sh_sand_man_t sh_dnx_gpm_man = {
    "Display gport information.",
    NULL,
};
