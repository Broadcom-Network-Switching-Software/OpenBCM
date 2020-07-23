/** \file diag_dnx_field_port.c
 *
 * Diagnostics procedures, for DNX, for 'port' operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_FLDDIAGSDNX
/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm_int/dnx/field/field.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include "diag_dnx_field_port.h"
#include <bcm_int/dnx/field/field_group.h>
#include <appl/diag/dnx/diag_dnx_field.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_access.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_field_context_access.h>
#include <bcm_int/dnx/algo/field/algo_field.h>
#include <bcm_int/dnx/field/field_entry.h>
#include <bcm_int/dnx/field/tcam/tcam_handler.h>
#include "diag_dnx_field_utils.h"
#include <bcm_int/dnx/field/field_port.h>
#include <soc/dnx/mdb.h>
#include <bcm_int/dnx/algo/lif_mngr/lif_mngr.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>

extern shr_error_e dbal_tables_physical_table_get(
    int unit,
    dbal_tables_e table_id,
    int physical_tbl_index,
    dbal_physical_tables_e * physical_table_id);

/*
 * }
 */

/*
 * DEFINEs
 * {
 */
/*
 * }
 */
/*
 * TYPEDEFs
 * {
 */
/*
 * }
 */
/*
 * MACROs
 * {
 */
/*
 * Options
 * {
 */
/*
 * }
 */
/* *INDENT-OFF* */
/*
 * Global and Static
 * {
 */
/*
 * }
 */
/* *INDENT-ON* */

/**
 * \brief
 *   This function allocates some resources to be used
 *   for testing during the 'field port list exec' command.
 * \param [in] unit - The unit number.
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_port_init(
    int unit)
{
    bcm_gport_t gport_pp;
    bcm_gport_t gport_tm;

    SHR_FUNC_INIT_VARS(unit);

    BCM_GPORT_LOCAL_SET(gport_pp, 200);
    BCM_GPORT_LOCAL_SET(gport_tm, 201);

    SHR_IF_ERR_EXIT(bcm_port_class_set(unit, gport_pp, bcmPortClassFieldIngressPMF1PacketProcessingPort, 0x5ff));
    SHR_IF_ERR_EXIT(bcm_port_class_set(unit, gport_pp, bcmPortClassFieldIngressPMF3PacketProcessingPortCs, 1));
    SHR_IF_ERR_EXIT(bcm_port_class_set(unit, gport_pp, bcmPortClassFieldEgressPacketProcessingPort, 0x7ff));

    SHR_IF_ERR_EXIT(bcm_port_class_set(unit, gport_tm, bcmPortClassFieldIngressPMF1TrafficManagementPort, 0x6ff));
    SHR_IF_ERR_EXIT(bcm_port_class_set(unit, gport_tm, bcmPortClassFieldIngressPMF3TrafficManagementPortCs, 2));
    SHR_IF_ERR_EXIT(bcm_port_class_set(unit, gport_tm, bcmPortClassFieldEgressTrafficManagementPort, 0x8ff));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function gives all pclasses per port_type.
 *
 *  \param [in] unit - The unit number.
 *  \param [in] port_type - The port_type.
 *  \param [out] port_type_pclass_arr_p - An array with all pclasses for given port_type.
 *  \param [out] nof_pclass_p - Number of pclasses for given port_type.
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_port_pclass_get_per_type(
    int unit,
    port_type_e port_type,
    bcm_port_class_t * port_type_pclass_arr_p,
    int *nof_pclass_p)
{
    SHR_FUNC_INIT_VARS(unit);
   /** Switch between all port types and get the relevant pclasses. */
    switch (port_type)
    {
        case PORT_TYPE_LIF:
        {
            port_type_pclass_arr_p[0] = bcmPortClassFieldIngressVport;
            port_type_pclass_arr_p[1] = bcmPortClassFieldEgressVport;
            *nof_pclass_p = 2;
            break;
        }
        case PORT_TYPE_PP:
        {
            port_type_pclass_arr_p[0] = bcmPortClassFieldIngressPMF1PacketProcessingPort;
            port_type_pclass_arr_p[1] = bcmPortClassFieldIngressPMF1PacketProcessingPortCs;
            port_type_pclass_arr_p[2] = bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralData;
            port_type_pclass_arr_p[3] = bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralDataHigh;
            port_type_pclass_arr_p[4] = bcmPortClassFieldIngressPMF3PacketProcessingPort;
            port_type_pclass_arr_p[5] = bcmPortClassFieldIngressPMF3PacketProcessingPortCs;
            port_type_pclass_arr_p[6] = bcmPortClassFieldIngressPMF3PacketProcessingPortGeneralData;
            port_type_pclass_arr_p[7] = bcmPortClassFieldIngressPMF3PacketProcessingPortGeneralDataHigh;
            port_type_pclass_arr_p[8] = bcmPortClassFieldEgressPacketProcessingPort;
            port_type_pclass_arr_p[9] = bcmPortClassFieldEgressPacketProcessingPortCs;
            port_type_pclass_arr_p[10] = bcmPortClassFieldExternalPacketProcessingPortCs;
            *nof_pclass_p = 11;
            break;
        }
        case PORT_TYPE_TM:
        {
            port_type_pclass_arr_p[0] = bcmPortClassFieldIngressPMF1TrafficManagementPort;
            port_type_pclass_arr_p[1] = bcmPortClassFieldIngressPMF1TrafficManagementPortCs;
            port_type_pclass_arr_p[2] = bcmPortClassFieldIngressPMF3TrafficManagementPort;
            port_type_pclass_arr_p[3] = bcmPortClassFieldIngressPMF3TrafficManagementPortCs;
            port_type_pclass_arr_p[4] = bcmPortClassFieldEgressTrafficManagementPort;
            port_type_pclass_arr_p[5] = bcmPortClassFieldExternalTrafficManagementPortCs;
            *nof_pclass_p = 6;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid port type");
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function gives all pclasses per stage.
 *
 *  \param [in] unit              - The unit number.
 *  \param [in] field_stage       - The field stage.
 *  \param [out] stage_pclass_arr_p -  An array with all pclasses for given stage.
 *  \param [out] nof_pclass_p - Number of pclasses for given stage.
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_port_pclass_get_per_stage(
    int unit,
    dnx_field_stage_e field_stage,
    bcm_port_class_t * stage_pclass_arr_p,
    int *nof_pclass_p)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Switch between all stages types and get the relevant pclasses. */
    switch (field_stage)
    {
        case DNX_FIELD_STAGE_IPMF1:
        case DNX_FIELD_STAGE_IPMF2:
        {
            stage_pclass_arr_p[0] = bcmPortClassFieldIngressVport;
            stage_pclass_arr_p[1] = bcmPortClassFieldIngressPMF1PacketProcessingPort;
            stage_pclass_arr_p[2] = bcmPortClassFieldIngressPMF1PacketProcessingPortCs;
            stage_pclass_arr_p[3] = bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralData;
            stage_pclass_arr_p[4] = bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralDataHigh;
            stage_pclass_arr_p[5] = bcmPortClassFieldIngressPMF1TrafficManagementPort;
            stage_pclass_arr_p[6] = bcmPortClassFieldIngressPMF1TrafficManagementPortCs;
            *nof_pclass_p = 7;
            break;
        }
        case DNX_FIELD_STAGE_IPMF3:
        {
            stage_pclass_arr_p[0] = bcmPortClassFieldIngressVport;
            stage_pclass_arr_p[1] = bcmPortClassFieldIngressPMF3PacketProcessingPort;
            stage_pclass_arr_p[2] = bcmPortClassFieldIngressPMF3PacketProcessingPortCs;
            stage_pclass_arr_p[3] = bcmPortClassFieldIngressPMF3PacketProcessingPortGeneralData;
            stage_pclass_arr_p[4] = bcmPortClassFieldIngressPMF3PacketProcessingPortGeneralDataHigh;
            stage_pclass_arr_p[5] = bcmPortClassFieldIngressPMF3TrafficManagementPort;
            stage_pclass_arr_p[6] = bcmPortClassFieldIngressPMF3TrafficManagementPortCs;
            *nof_pclass_p = 7;
            break;
        }
        case DNX_FIELD_STAGE_EPMF:
        {
            stage_pclass_arr_p[0] = bcmPortClassFieldEgressVport;
            stage_pclass_arr_p[1] = bcmPortClassFieldEgressPacketProcessingPort;
            stage_pclass_arr_p[2] = bcmPortClassFieldEgressPacketProcessingPortCs;
            stage_pclass_arr_p[3] = bcmPortClassFieldEgressTrafficManagementPort;
            *nof_pclass_p = 5;
            break;
        }
        case DNX_FIELD_STAGE_EXTERNAL:
        {
            stage_pclass_arr_p[0] = bcmPortClassFieldExternalPacketProcessingPortCs;
            stage_pclass_arr_p[1] = bcmPortClassFieldExternalTrafficManagementPortCs;
            *nof_pclass_p = 2;
            break;
        }
        default:
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid PMF Stage");
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 * This function gives all pclasses per stage.
 *
 *  \param [in] unit              - The unit number.
 *  \param [in] field_stage       - The field stage.
 *  \param [in] port_type - The port_type.
 *  \param [out] pclass_arr_p - An array with all pclasses for given port_type.
 *  \param [out] nof_pclasses_p - The number of all pclasses for given port_type.
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static void
diag_dnx_field_port_pclass_get(
    int unit,
    dnx_field_stage_e field_stage,
    int port_type,
    bcm_port_class_t * pclass_arr_p,
    int *nof_pclasses_p)
{
    bcm_port_class_t stage_pclass_arr[8], port_type_pclass_arr[11];
    int stage_iter, port_iter, pclass_iter;
    int nof_pclass_per_stage = 0;
    int nof_pclass_per_port_type = 0;
    /** Get all port classes for the given port_type. */
    diag_dnx_field_port_pclass_get_per_type(unit, port_type, port_type_pclass_arr, &nof_pclass_per_port_type);
    /** Get all port classes for the given stage. */
    diag_dnx_field_port_pclass_get_per_stage(unit, field_stage, stage_pclass_arr, &nof_pclass_per_stage);
    pclass_iter = 0;
    /** Iterate over all pclasses per port. */
    for (port_iter = 0; port_iter < nof_pclass_per_port_type; port_iter++)
    {
        /** Iterate over all pclasses per stage. */
        for (stage_iter = 0; stage_iter < nof_pclass_per_stage; stage_iter++)
        {
            /** If we have a match. Save the pclass into the array. */
            if (port_type_pclass_arr[port_iter] == stage_pclass_arr[stage_iter])
            {
                pclass_arr_p[pclass_iter] = stage_pclass_arr[stage_iter];
                pclass_iter++;
            }
        }
    }
    *nof_pclasses_p = pclass_iter;
}

/**
 * \brief
 *   This function displays all class ID's for port_type LIF.
 *
 * \param [in] unit - The unit number.
 * \param [in] prt_ctr - Pointer to control structure used by pretty printing,
 *  declared by PRT_INIT_VARS().
 * \param [out] do_display_p       - Pointer to show if there is something to display.
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_port_list_lif_class_id_print(
    int unit,
    prt_control_t * prt_ctr,
    uint8 *do_display_p)
{
    uint32 entry_handle_id = 0;
    uint32 dbal_tables[] = { DBAL_TABLE_IN_AC_INFO_DB,
        DBAL_TABLE_IN_LIF_FORMAT_PWE, DBAL_TABLE_IN_LIF_FORMAT_EVPN,
        DBAL_TABLE_IN_LIF_FORMAT_LSP, DBAL_TABLE_IN_LIF_IPvX_TUNNELS,
        DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION,
        DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT
    };
    uint32 nof_tables = sizeof(dbal_tables) / sizeof(dbal_tables[0]);
    uint32 table;
    uint32 in_lif_profile_field[1], result_type[1], in_lif_field;
    int is_end = 0;
    unsigned int start_bit = 0;
    unsigned int nof_bits = 0;
    int mask = 0;
    uint32 class_id = 0;
    int nof_unique_class_id, class_id_iter, unique_class_id_arr[5];

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);
    nof_unique_class_id = 0;
    /** Get the start bit and the nof_bits of the PMF in_lif_profile bits. */
    SHR_IF_ERR_EXIT(dnx_field_port_profile_bits_get
                    (unit, DNX_FIELD_PORT_PROFILE_TYPE_IN_LIF_INGRESS, &start_bit, &nof_bits));
    /** Iterate over all tables and all their entries. */
    for (table = 0; table < nof_tables; table++)
    {
        /**
         * Allocate handle to the table of the iteration and initialize an iterator entity.
         * The iterator is in mode ALL, which means that it will consider all entries regardless
         * of them being default entries or not.
         */
        if (table == 0)
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_tables[table], &entry_handle_id));
        }
        else
        {
            SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, dbal_tables[table], entry_handle_id));
        }
        SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_ALL));
        /** Receive first entry in table. */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        while (!is_end)
        {
            /**
             * IF the table is DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT check the result_types
             * because not all result types has a field DBAL_FIELD_IN_LIF_PROFILE.
             */
            if (dbal_tables[table] == DBAL_TABLE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT)
            {
                SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                                (unit, entry_handle_id, DBAL_FIELD_RESULT_TYPE, INST_SINGLE, result_type));

                /** If there is no DBAL_FIELD_IN_LIF_PROFILE in the result type continue. */
                if (result_type[0] ==
                    DBAL_RESULT_TYPE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT_IN_ETH_VLAN_EDIT_VSI_MP
                    || result_type[0] ==
                    DBAL_RESULT_TYPE_INNER_ETH_VLAN_EDIT_CLASSIFICATION_VLAN_EDIT_IN_ETH_VLAN_EDIT_ONLY)
                {
                    /** Receive next entry in table. */
                    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
                    continue;
                }
            }
            /** Get the IN_LIF, which is the KEY of the dbal tables. */
            SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_IN_LIF, &in_lif_field));
            /** Get the IN_LIF_PROFILE. */
            SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get
                            (unit, entry_handle_id, DBAL_FIELD_IN_LIF_PROFILE, INST_SINGLE, in_lif_profile_field));
            /** Mask the PMF bits of the in_lif_profile. */
            mask = (((1 << nof_bits) - 1) << start_bit);
            /** If the PMF IN_LIF_PROFILE bits are different than 0. Get the class_id. */
            if ((in_lif_profile_field[0] & mask) != 0)
            {
                /** Calculate the class_id. */
                class_id = (in_lif_profile_field[0] >> start_bit);
                /** Iterate over all unique class IDs.(To remove the duplication). */
                for (class_id_iter = 0; class_id_iter < nof_unique_class_id; class_id_iter++)
                {
                    /** Check if the class_id is already in the array with unique IDs. */
                    if (class_id == unique_class_id_arr[class_id_iter])
                    {
                        break;
                    }
                }
                /** If the nof_unique_class_id is reached. Save the class_id into the array with unique IDs. */
                if (class_id_iter == nof_unique_class_id)
                {
                    int global_lif;
                    lif_mapping_local_lif_info_t local_lif_info;

                    unique_class_id_arr[class_id_iter] = class_id;
                    nof_unique_class_id++;

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                    local_lif_info.local_lif = in_lif_field;
                    SHR_IF_ERR_EXIT(dbal_tables_physical_table_get
                                    (unit, dbal_tables[table], 0, &local_lif_info.phy_table));

                    dnx_algo_lif_mapping_local_to_global_get(unit, DNX_ALGO_LIF_INGRESS, &local_lif_info, &global_lif);
                    PRT_CELL_SET("0x%x", global_lif);
                    PRT_CELL_SET("0x%x", class_id);

                    *do_display_p = TRUE;
                }
            }

            /** Receive next entry in table. */
            SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
        }
    }
exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   This function displays Field ports per port ID or port of IDs  (optional), specified by the caller.
 * \param [in] unit - The unit number.
 * \param [in] args -
 *   Null terminated string. Contains 'Options': List of arguments as shown on screen (typed by caller/user)
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 *
 * \return
 *   Error code (as per 'shr_error_e').
 * \see
 *   shr_error_e
 */
static shr_error_e
diag_dnx_field_port_list_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint8 do_display, print_new_port;
    int port_type_lower, port_type_upper;
    dnx_field_stage_e stage_lower, stage_upper;
    dnx_field_stage_e stage_index;
    int port_type, pclass_indx, nof_pclasses;
    bcm_port_t port = 0;
    bcm_port_class_t pclass_arr[11];
    bcm_port_config_t port_config;
    uint32 class_id = 0;
    int been_there;
    char title[4][4] = { "LIF", "PP", "TM" };
    /** This variables stores the severity of the Field processor dnx. */
    bsl_severity_t original_severity_fldprocdnx;
    /** This variables stores the severity of the Port dnx. */
    bsl_severity_t original_severity_portdnx;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    port_type = 0;

    /**
     * For quiet mode (To avoid seeing error messages), we store the original severity here and,
     * increase the severity to 'fatal' so that standard errors will not show on the screen.
     * Upon exit, we restore the original severity.
     */
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_GET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_RESMNGR, original_severity_portdnx);

    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_TYPE, port_type_lower, port_type_upper);
    SH_SAND_GET_UINT32_RANGE(DIAG_DNX_FIELD_OPTION_STAGE, stage_lower, stage_upper);

    print_new_port = FALSE;
    for (port_type = port_type_lower; port_type <= port_type_upper; port_type++)
    {
        do_display = FALSE;

        /**
         * If type is LIF then print LIF related info, otherwise
         * present tables with relevant columns for PP and TM types.
         */
        if (port_type == 0)
        {
            PRT_TITLE_SET("LIF Info");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "LIF");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Profile");
        }
        else
        {
            PRT_TITLE_SET("%s Port Info", title[port_type]);
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Port");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Stage");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "CS Profile");
            PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Profile");
            /** In case of PP type we have to present General Data. */
            if (port_type == 1)
            {
                PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "General Data");
            }
        }
        been_there = FALSE;
        /** Iterate over all given stages. */
        for (stage_index = stage_lower; stage_index <= stage_upper; stage_index++)
        {
            dnx_field_stage_e field_stage;
            SHR_IF_ERR_EXIT(diag_dnx_field_utils_stage_str_to_enum_val_conversion
                            (unit, Field_port_diag_stage_enum_table[stage_index].string, &field_stage));

            /** Fill the array for all pclasses for the given port_type and stage. */
            nof_pclasses = 0;
            diag_dnx_field_port_pclass_get(unit, field_stage, port_type, pclass_arr, &nof_pclasses);
            /** If the port type is 0 return all LIF class_ids. */
            if (port_type == 0)
            {
                /** The diag_dnx_field_port_list_lif_class_id_print is only for INGRESS LIF_PROFILES. */
                if (field_stage == DNX_FIELD_STAGE_IPMF1 ||
                    field_stage == DNX_FIELD_STAGE_IPMF2 || field_stage == DNX_FIELD_STAGE_IPMF3)
                {
                    /** Go only once in this function since the LIF_PROFILES are the same for all INGRESS stages. */
                    if (been_there == FALSE)
                    {
                        SHR_IF_ERR_EXIT(diag_dnx_field_port_list_lif_class_id_print(unit, prt_ctr, &do_display));
                        been_there = TRUE;
                    }
                }
                
                else
                {
                    continue;
                }
            }
            /** Else iterate over all physical ports and get the class_ids. */
            else
            {
                /** Get the port iterator. */
                SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));
                BCM_PBMP_ITER(port_config.all, port)
                {

                    uint32 profile_value = 0;
                    uint32 cs_profile_value = 0;
                    uint32 general_data_value = 0;
                    bcm_gport_t port_gport;
                    dnx_algo_port_info_s port_info;
                    int port_in_lag = 0;
                    SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_info));
                    SHR_IF_ERR_EXIT(dnx_algo_port_in_lag(unit, port, &port_in_lag));
                    /*
                     * Skip ports that do not have TM or PP port
                     */
                    if (DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, port_info) || DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, port_info)
                        || DNX_ALGO_PORT_TYPE_IS_PP(unit, port_info, port_in_lag))
                    {
                        BCM_GPORT_LOCAL_SET(port_gport, port);
                        /** Iterate over all pclass_arr for this stage and port_type. */
                        for (pclass_indx = 0; pclass_indx < nof_pclasses; pclass_indx++)
                        {
                            shr_error_e rv = _SHR_E_NONE;
                            /** Increase the severity to 'fatal' to avoid seeing errors messages on the screen. */
                            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, bslSeverityFatal);
                            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_PORT, bslSeverityFatal);

                            /** Get the class_id. If an error because of the wrong port occurred continue. */
                            rv = bcm_port_class_get(unit, port_gport, pclass_arr[pclass_indx], &class_id);

                            /** Restore the original severity. */
                            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX,
                                                                 original_severity_fldprocdnx);
                            SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_PORT, original_severity_portdnx);

                            if (rv == _SHR_E_PORT)
                            {
                                continue;
                            }
                            else if (rv != _SHR_E_NONE)
                            {
                                SHR_IF_ERR_EXIT(bcm_port_class_get
                                                (unit, port_gport, pclass_arr[pclass_indx], &class_id));
                                SHR_ERR_EXIT(_SHR_E_INTERNAL,
                                             "bcm_port_class_get for GPORT 0x%x port class 0x%x once failed (%d) and once succeeded for the same input.\n",
                                             port_gport, pclass_arr[pclass_indx], rv);
                            }

                            if (class_id)
                            {
                                switch (pclass_arr[pclass_indx])
                                {
                                    case bcmPortClassFieldIngressPMF1PacketProcessingPort:
                                    case bcmPortClassFieldIngressPMF1TrafficManagementPort:
                                    case bcmPortClassFieldIngressPMF3PacketProcessingPort:
                                    case bcmPortClassFieldIngressPMF3TrafficManagementPort:
                                    case bcmPortClassFieldEgressPacketProcessingPort:
                                    case bcmPortClassFieldEgressTrafficManagementPort:
                                    {
                                        profile_value = class_id;
                                        break;
                                    }
                                    case bcmPortClassFieldIngressPMF1PacketProcessingPortCs:
                                    case bcmPortClassFieldIngressPMF1TrafficManagementPortCs:
                                    case bcmPortClassFieldIngressPMF3PacketProcessingPortCs:
                                    case bcmPortClassFieldIngressPMF3TrafficManagementPortCs:
                                    case bcmPortClassFieldEgressPacketProcessingPortCs:
                                    case bcmPortClassFieldExternalPacketProcessingPortCs:
                                    case bcmPortClassFieldExternalTrafficManagementPortCs:
                                    {
                                        cs_profile_value = class_id;
                                        break;
                                    }
                                    case bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralData:
                                    case bcmPortClassFieldIngressPMF1PacketProcessingPortGeneralDataHigh:
                                    case bcmPortClassFieldIngressPMF3PacketProcessingPortGeneralData:
                                    case bcmPortClassFieldIngressPMF3PacketProcessingPortGeneralDataHigh:
                                    {
                                        general_data_value = class_id;
                                        break;
                                    }
                                    default:
                                    {
                                        SHR_ERR_EXIT(_SHR_E_PARAM, "Invalid Class Field Type.");
                                        break;
                                    }
                                }

                                print_new_port = TRUE;
                            }
                        }

                        if (print_new_port && (cs_profile_value || profile_value || general_data_value))
                        {
                            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                            PRT_CELL_SET("%d", port);
                            PRT_CELL_SET("%s", dnx_field_stage_text(unit, field_stage));
                            PRT_CELL_SET("0x%x", cs_profile_value);
                            if (field_stage != DNX_FIELD_STAGE_EXTERNAL)
                            {
                                PRT_CELL_SET("0x%x", profile_value);
                            }
                            else
                            {
                                PRT_CELL_SET("%s", "N/A");
                            }

                          /** In case of PP type and field stage is one of IPMF1/2/3 we have to present General Data. */
                            if (port_type == 1)
                            {
                                if (field_stage == DNX_FIELD_STAGE_IPMF1 || field_stage == DNX_FIELD_STAGE_IPMF2 ||
                                    field_stage == DNX_FIELD_STAGE_IPMF3)
                                {
                                    PRT_CELL_SET("0x%x", general_data_value);
                                }
                                else
                                {
                                    PRT_CELL_SET("%s", "N/A");
                                }
                            }

                            do_display = TRUE;
                        }
                    }
                }
            }
        }

        if (do_display)
        {
            PRT_COMMITX;
        }
        else
        {
            /**
             * If nothing was printed then just clear loaded PRT memory.
             * This could be left to 'exit' but it is clearer here.
             */
            PRT_FREE;
            LOG_CLI_EX("\r\n" "NO %s ports configured!! %s%s%s\r\n\n", title[port_type], EMPTY, EMPTY, EMPTY);
        }
    }

exit:
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_FLDPROCDNX, original_severity_fldprocdnx);
    SHR_SET_SEVERITY_FOR_MODULE_SPECIFIC(BSL_LS_BCMDNX_PORT, original_severity_portdnx);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
/*
 * Global and Static
 * {
 */

/**
 *  'Help' description for Port List (shell commands).
 */
static sh_sand_man_t Field_port_list_man = {
    .brief = "'type'- displays the basic info about a specific port type"
             "on a certain stage (filter by field stage)",
    .full = "'type' display for a port of field ports. Field port is specified via 'port'.\r\n"
            "If no 'type' is specified then '0'-'nof_ports_types-1' is assumed (0-255).\r\n"
            "If no stage is specified then 'IPMF1'-'EPMF' is assumed (0-3).\r\n",
    .synopsis = "[type=<<Lowest port type>-<Highest port type>>]"
            "[stage=<<IPMF1 | IPMF2 | ...>-<<IPMF1 | IPMF2 | ...>>]",
    .examples = "type=LIF-TM stage=IPMF1-EXTERNAL",
    .init_cb = diag_dnx_field_port_init,
};

static sh_sand_option_t Field_port_list_options[] = {
    {DIAG_DNX_FIELD_OPTION_TYPE,  SAL_FIELD_TYPE_UINT32, "Lowest-highest port type to get its info",    "LIF-TM",       (void *)Field_port_type_enum_table, "LIF-TM"},
    {DIAG_DNX_FIELD_OPTION_STAGE,  SAL_FIELD_TYPE_UINT32, "Lowest-highest field stage to context key info for",   "STAGE_LOWEST-STAGE_HIGHEST",    (void *)Field_port_diag_stage_enum_table, "STAGE_LOWEST-STAGE_HIGHEST"},
    {NULL}
};

/*
 * }
 */
/* *INDENT-ON* */

/*
 * Globals required at end because they reference static procedures/tables above.
 * {
 */
/**
 * \brief
 *   List of shell options for 'port' shell commands (list, info)
 */
sh_sand_cmd_t Sh_dnx_field_port_cmds[] = {
    {"list", diag_dnx_field_port_list_cb, NULL, Field_port_list_options, &Field_port_list_man}
    ,
    {NULL}
};

/*
 * }
 */
