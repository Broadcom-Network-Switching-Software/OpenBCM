/** \file diag_dnx_stat_pp.c
 *
 * Main diagnostics for stat_pp commands are gathered in this file.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_SOCDNX_DIAGDNX

/*************
 * INCLUDES  *
 */
#include <bcm_int/dnx/cmn/dnxcmn.h>
#include "diag_dnx_stat_pp.h"
#include "diag_dnx_pp.h"

static char *dnx_pp_header_type_desc[bcmStatPpMetadataHeaderTypeCount + 1] = {
    "ETH",
    "IPv4",
    "IPv6",
    "MPLS",
    "ARP",
    "FCoE",
    "SRv6",
    "TCP",
    "UDP",
    "BFD",
    "ICMP",
    "Others",
    "Unknown",
};

/**
 * \brief
 *   Iterate over the dbal table received and print statistics configuration
 * \param [in] unit - Unit ID
 * \param [in] block - block name
 * \param [in] dbal_table_id - relevant dbal table
 * \param [in] sand_control - pointer to framework control structure
 * \return
 *      Standard error handling
 */
static shr_error_e
dnx_stat_pp_block_dump(
    int unit,
    char *block,
    dbal_tables_e dbal_table_id,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id;
    int is_end = 0;
    uint32 field_value[DBAL_FIELD_ARRAY_MAX_SIZE_IN_WORDS];
    int object_id;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("%s Statistics summary", block);

    PRT_COLUMN_ADD("Statistic profile");
    PRT_COLUMN_ADD("Object-ID");
    PRT_COLUMN_ADD("Statistic object type");
    PRT_COLUMN_ADD("Is-Meter");
    PRT_COLUMN_ADD("Metadata");
    PRT_COLUMN_ADD("External-Object-ID");
    PRT_COLUMN_ADD("DP-Meter-Command");
    PRT_COLUMN_ADD("Final_meter_index");

    /**Iterate over all block dbal table and get data from valid entries*/
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, dbal_table_id, &entry_handle_id));
    SHR_IF_ERR_EXIT(dbal_iterator_init(unit, entry_handle_id, DBAL_ITER_MODE_GET_ALL_EXCEPT_DEFAULT));

    /*
     * Receive first entry in table.
     */
    SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    while (!is_end)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        /**Statistic interface*/
        SHR_IF_ERR_EXIT(dbal_entry_handle_key_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_STAT_CMD, field_value));
        PRT_CELL_SET("%d", field_value[0]);

        /**Object-ID*/
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_STAT_IF_OBJECT_ID,
                                                                INST_SINGLE, field_value));
        object_id = field_value[0];
        PRT_CELL_SET("%d", object_id);

        /**Type*/
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_STAT_IF_TYPE_ID,
                                                                INST_SINGLE, field_value));
        PRT_CELL_SET("%d", field_value[0]);

        /**Is-Meter*/
        SHR_IF_ERR_EXIT(dbal_entry_handle_value_field_arr32_get(unit, entry_handle_id, DBAL_FIELD_IS_METER,
                                                                INST_SINGLE, field_value));
        PRT_CELL_SET("%d", field_value[0]);

        
        /**Metadata*/
        PRT_CELL_SET("Unsupported");

        /**External-Object-ID*/
        PRT_CELL_SET("Unsupported");

        /**DP-Meter-Command*/
        PRT_CELL_SET("Unsupported");

        /**Final_meter_index*/
        PRT_CELL_SET("Unsupported");

        /*
         * Receive next entry in table.
         */
        SHR_IF_ERR_EXIT(dbal_iterator_get_next(unit, entry_handle_id, &is_end));
    }

    PRT_COMMITX;
exit:
    DBAL_FUNC_FREE_VARS;
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   print statistic object type, id and profile in table
 * \param [in] unit - Unit ID
 * \param [in] object_type - statistic object type
 * \param [in] stat_id - statistic object ID
 * \param [in] stat_profile - statistic object profile
 * \param [in] stat_metadata - statistic metadata value
 * \param [in] prt_ctr - print control pointer
 * \return
 *      Standard error handling
 */
static shr_error_e
dnx_stat_pp_print_vis_stat(
    int unit,
    char *object_type,
    int stat_id,
    int stat_profile,
    int stat_metadata,
    prt_control_t * prt_ctr)
{
    SHR_FUNC_INIT_VARS(unit);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    /**type*/
    PRT_CELL_SET("%s", object_type);

    /**Stat id i*/
    if (stat_id != 0)
    {
        PRT_CELL_SET("0x%x", stat_id);
    }
    else
    {
        PRT_CELL_SET("");
    }

    /**Stat pp profile i*/
    if (stat_profile != 0)
    {
        PRT_CELL_SET("0x%x", stat_profile);
    }
    else
    {
        PRT_CELL_SET("");
    }

    /**Stat metadata */
    if (stat_metadata != 0)
        PRT_CELL_SET("0x%x", stat_metadata);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   print stat pp port command
 * \param [in] unit - Unit ID
 * \param [in] core - Core ID
 * \param [in] prt_ctr - print control pointer
 * \return
 *      Standard error handling
 */
static shr_error_e
dnx_stat_pp_irpp_vis_gen_pp_port_stats(
    int unit,
    int core,
    prt_control_t * prt_ctr)
{
    uint32 entry_handle_id;
    uint32 stat_id;
    uint32 stat_pp_profile;
    uint32 in_pp_port;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get pp port */
    SHR_IF_ERR_EXIT_NO_MSG(dpp_dsig_read(unit, core, "IRPP", "IPMF1", "FER", "In_Port", &in_pp_port, 1));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, in_pp_port);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                        entry_handle_id,
                                                        DBAL_FIELD_STAT_OBJECT_ID, INST_SINGLE, &stat_id));
    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                        entry_handle_id,
                                                        DBAL_FIELD_STAT_OBJECT_CMD, INST_SINGLE, &stat_pp_profile));

    if (stat_id != 0)
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_print_vis_stat(unit, "iPMF in pp port", stat_id, stat_pp_profile, 0, prt_ctr));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   print stat fwd action profile command
 * \param [in] unit - Unit ID
 * \param [in] core - Core ID
 * \param [in] prt_ctr - print control pointer
 * \return
 *      Standard error handling
 */
static shr_error_e
dnx_stat_pp_irpp_vis_gen_fwd_act_profile_stats(
    int unit,
    int core,
    prt_control_t * prt_ctr)
{
    uint32 entry_handle_id;
    uint32 stat_id;
    uint32 trap_id;
    uint32 stat_metadata_prev = 0;
    uint32 stat_metadata_cur = 0;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get trap ID */
    SHR_IF_ERR_EXIT_NO_MSG(dpp_dsig_read(unit, core, "IRPP", "IPMF3", "LBP", "Fwd_Action_CPU_Trap_Code", &trap_id, 1));

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_TRAP_INGRESS_FWD_ACTION_TABLE, &entry_handle_id));
    /** key construction*/
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_TRAP_ID, trap_id);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, core);

    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id, DBAL_GET_ALL_FIELDS));

    SHR_IF_ERR_EXIT(dbal_entry_handle_value_field32_get(unit,
                                                        entry_handle_id,
                                                        DBAL_FIELD_STAT_OBJ_VALUE_0, INST_SINGLE, &stat_id));

    /** Get stat_metadata value before and after iPMF3 */
    SHR_IF_ERR_EXIT_NO_MSG(dpp_dsig_read(unit, core, "IRPP", "FER", "IPMF3", "Stats_Meta_Data", &stat_metadata_prev, 1));
    SHR_IF_ERR_EXIT_NO_MSG(dpp_dsig_read(unit, core, "IRPP", "IPMF3", "LBP", "Stats_Meta_Data", &stat_metadata_cur, 1));

    if (stat_id != 0)
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_print_vis_stat(unit, "trap fwd action profile", stat_id, 0, stat_metadata_cur, prt_ctr));
    }
    else if (stat_metadata_prev != stat_metadata_cur)
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_print_vis_stat(unit, "iPMF3 to LBP", 0, 0, stat_metadata_cur, prt_ctr));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   print stat fer commands
 * \param [in] unit - Unit ID
 * \param [in] core - Core ID
 * \param [in] prt_ctr - print control pointer
 * \return
 *      Standard error handling
 */
static shr_error_e
dnx_stat_pp_irpp_vis_gen_fer_stats(
    int unit,
    int core,
    prt_control_t * prt_ctr)
{
    char fec_object_type[32];
    int i;
    uint32 stat_id;
    uint32 stat_pp_profile;
    signal_output_t *stats_ids = NULL;
    signal_output_t *stats_profiles = NULL;
    uint32 stat_metadata_prev = 0;
    uint32 stat_metadata_cur = 0;
    int stat_metadata_printed = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT_NO_MSG(sand_signal_output_find(unit, core, 0, NULL, "FER", "IPMF3", "Fer_Stats_Obj", &stats_ids));
    SHR_IF_ERR_EXIT_NO_MSG(sand_signal_output_find
                           (unit, core, 0, NULL, "FER", "IPMF3", "Fer_Stats_Obj_Atr", &stats_profiles));

    /** Get stat_metadata value before and after iPMF1 */
    SHR_IF_ERR_EXIT_NO_MSG(dpp_dsig_read(unit, core, "IRPP", "FWD2", "IPMF1", "Stats_Meta_Data", &stat_metadata_prev, 1));
    SHR_IF_ERR_EXIT_NO_MSG(dpp_dsig_read(unit, core, "IRPP", "FER", "IPMF3", "Stats_Meta_Data", &stat_metadata_cur, 1));

    for (i = 0; i <= 2; i++)
    {
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(stats_ids->value, i * 24, 24, &stat_id));

        if (stat_id != 0)
        {
            SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field(stats_profiles->value, i * 5, 5, &stat_pp_profile));
            sal_snprintf(fec_object_type, RHNAME_MAX_SIZE, "Destination fec %d", (i + 1));
            SHR_IF_ERR_EXIT(dnx_stat_pp_print_vis_stat(unit, fec_object_type, stat_id, stat_pp_profile, stat_metadata_cur, prt_ctr));

            stat_metadata_printed = 1;
        }
    }

    if (!stat_metadata_printed && (stat_metadata_prev != stat_metadata_cur))
    {
        /** print if all stat_id are zero but stat_metadata has been changed by iPMF1, print it here */
        sal_snprintf(fec_object_type, RHNAME_MAX_SIZE, "FER to IPMF3");
        SHR_IF_ERR_EXIT(dnx_stat_pp_print_vis_stat(unit, fec_object_type, 0, 0, stat_metadata_cur, prt_ctr));
    }

exit:
    sand_signal_output_free(stats_ids);
    sand_signal_output_free(stats_profiles);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   print irpp stat commands
 * \param [in] unit - Unit ID
 * \param [in] stat_type - statistic object type
 * \param [in] idx - object idx inside stat ids, stat profiles signals
 * \param [in] stats_ids - stats ids signal
 * \param [in] stats_profiles - stats profiles signal
 * \param [in] stats_metadata_prev - stats metadata signal from previous stage
 * \param [in] stats_metadata_cur - stats metadata signal for current stage
 * \param [in] prt_ctr - print control pointer
 * \return
 *      Standard error handling
 */
static shr_error_e
dnx_stat_pp_irpp_vis_gen_stats(
    int unit,
    char *stat_type,
    int idx,
    signal_output_t * stats_ids,
    signal_output_t * stats_profiles,
    signal_output_t * stats_metadata_prev,
    signal_output_t * stats_metadata_cur,
    prt_control_t * prt_ctr)
{
    uint32 stat_id;
    uint32 stat_profile;
    uint32 stat_metadata_prev = 0;
    uint32 stat_metadata_cur = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                    (stats_ids->value, idx * DIAG_DNX_STAT_PP_STAT_ID_SIZE, DIAG_DNX_STAT_PP_STAT_ID_SIZE, &stat_id));

    if (stats_metadata_prev != NULL)
    {
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (stats_metadata_prev->value, 0, DIAG_DNX_STAT_PP_IRPP_STAT_METADATA_SIZE, &stat_metadata_prev));
    }

    if (stats_metadata_cur != NULL)
    {
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (stats_metadata_cur->value, 0, DIAG_DNX_STAT_PP_IRPP_STAT_METADATA_SIZE, &stat_metadata_cur));
    }

    if (stat_id != 0)
    {
        SHR_IF_ERR_EXIT(utilex_bitstream_get_any_field
                        (stats_profiles->value, idx * DIAG_DNX_STAT_PP_STAT_PROFILE_SIZE,
                         DIAG_DNX_STAT_PP_STAT_PROFILE_SIZE, &stat_profile));
        SHR_IF_ERR_EXIT(dnx_stat_pp_print_vis_stat(unit, stat_type, stat_id, stat_profile, stat_metadata_cur, prt_ctr));
    }
    else if (stat_metadata_cur != stat_metadata_prev)
    {
        /** stat_metadata changed from previous stage */
        SHR_IF_ERR_EXIT(dnx_stat_pp_print_vis_stat(unit, stat_type, 0, 0, stat_metadata_cur, prt_ctr));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   print irpp stat commands
 * \param [in] unit - Unit ID
 * \param [in] core - Core ID
 * \param [in] sand_control - pointer to framework control structure
 * \return
 *      Standard error handling
 */
static shr_error_e
dnx_stat_pp_irpp_vis(
    int unit,
    int core,
    sh_sand_control_t * sand_control)
{
    signal_output_t *stats_ids = NULL;
    signal_output_t *stats_profiles = NULL;
    signal_output_t *stats_metadatas[STAT_PP_IRPP_OBJ_IDX_VTA_LIF+1] = {NULL};
    int ii;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("IRPP Statistics summary");

    PRT_COLUMN_ADD("Statistic stage and type");
    PRT_COLUMN_ADD("statistic object ID");
    PRT_COLUMN_ADD("statistic profile ID");
    PRT_COLUMN_ADD("Metadata");

    SHR_IF_ERR_EXIT_NO_MSG(sand_signal_output_find(unit, core, 0, NULL, "FWD2", "IPMF1", "Stats_Objects", &stats_ids));
    SHR_IF_ERR_EXIT_NO_MSG(sand_signal_output_find(unit, core, 0, NULL, "FWD2", "IPMF1", "Stats_Objects_Cmd",
                                                   &stats_profiles));
    SHR_IF_ERR_EXIT_NO_MSG(sand_signal_output_find(unit, core, 0, NULL, "VTT1", "VTT2", "Stats_Meta_Data", &stats_metadatas[STAT_PP_IRPP_OBJ_IDX_VTA_LIF]));
    SHR_IF_ERR_EXIT_NO_MSG(sand_signal_output_find(unit, core, 0, NULL, "VTT2", "VTT3", "Stats_Meta_Data", &stats_metadatas[STAT_PP_IRPP_OBJ_IDX_VTB_LIF]));
    SHR_IF_ERR_EXIT_NO_MSG(sand_signal_output_find(unit, core, 0, NULL, "VTT3", "VTT4", "Stats_Meta_Data", &stats_metadatas[STAT_PP_IRPP_OBJ_IDX_VTC_LIF]));
    SHR_IF_ERR_EXIT_NO_MSG(sand_signal_output_find(unit, core, 0, NULL, "VTT4", "VTT5", "Stats_Meta_Data", &stats_metadatas[STAT_PP_IRPP_OBJ_IDX_VTD_LIF]));
    SHR_IF_ERR_EXIT_NO_MSG(sand_signal_output_find(unit, core, 0, NULL, "VTT5", "FWD1", "Stats_Meta_Data", &stats_metadatas[STAT_PP_IRPP_OBJ_IDX_VTE_LIF]));
    SHR_IF_ERR_EXIT_NO_MSG(sand_signal_output_find(unit, core, 0, NULL, "FWD1", "FWD2", "Stats_Meta_Data", &stats_metadatas[STAT_PP_IRPP_OBJ_IDX_FWD1_FWD_FODO]));
    SHR_IF_ERR_EXIT_NO_MSG(sand_signal_output_find(unit, core, 0, NULL, "FWD2", "IPMF1", "Stats_Meta_Data", &stats_metadatas[STAT_PP_IRPP_OBJ_IDX_FWD2_FWD_FODO]));

    SHR_IF_ERR_EXIT(dnx_stat_pp_irpp_vis_gen_stats
                    (unit, "VTA in-lif", STAT_PP_IRPP_OBJ_IDX_VTA_LIF, stats_ids, stats_profiles,
                    NULL, stats_metadatas[STAT_PP_IRPP_OBJ_IDX_VTA_LIF], prt_ctr));
    SHR_IF_ERR_EXIT(dnx_stat_pp_irpp_vis_gen_stats
                    (unit, "VTB in-lif", STAT_PP_IRPP_OBJ_IDX_VTB_LIF, stats_ids, stats_profiles,
                    stats_metadatas[STAT_PP_IRPP_OBJ_IDX_VTA_LIF], stats_metadatas[STAT_PP_IRPP_OBJ_IDX_VTB_LIF], prt_ctr));
    SHR_IF_ERR_EXIT(dnx_stat_pp_irpp_vis_gen_stats
                    (unit, "VTC in-lif", STAT_PP_IRPP_OBJ_IDX_VTC_LIF, stats_ids, stats_profiles,
                    stats_metadatas[STAT_PP_IRPP_OBJ_IDX_VTB_LIF], stats_metadatas[STAT_PP_IRPP_OBJ_IDX_VTC_LIF], prt_ctr));
    SHR_IF_ERR_EXIT(dnx_stat_pp_irpp_vis_gen_stats
                    (unit, "VTD in-lif", STAT_PP_IRPP_OBJ_IDX_VTD_LIF, stats_ids, stats_profiles,
                    stats_metadatas[STAT_PP_IRPP_OBJ_IDX_VTC_LIF], stats_metadatas[STAT_PP_IRPP_OBJ_IDX_VTD_LIF], prt_ctr));
    SHR_IF_ERR_EXIT(dnx_stat_pp_irpp_vis_gen_stats
                    (unit, "VTE in-lif", STAT_PP_IRPP_OBJ_IDX_VTE_LIF, stats_ids, stats_profiles,
                    stats_metadatas[STAT_PP_IRPP_OBJ_IDX_VTD_LIF], stats_metadatas[STAT_PP_IRPP_OBJ_IDX_VTE_LIF], prt_ctr));
    SHR_IF_ERR_EXIT(dnx_stat_pp_irpp_vis_gen_stats
                    (unit, "VTA terminated FODO", STAT_PP_IRPP_OBJ_IDX_VTA_TERM_FODO, stats_ids, stats_profiles,
                     NULL, stats_metadatas[STAT_PP_IRPP_OBJ_IDX_VTA_LIF], prt_ctr));
    SHR_IF_ERR_EXIT(dnx_stat_pp_irpp_vis_gen_stats
                    (unit, "VTE terminated FODO", STAT_PP_IRPP_OBJ_IDX_VTE_TERM_FODO, stats_ids, stats_profiles,
                     stats_metadatas[STAT_PP_IRPP_OBJ_IDX_VTE_LIF], stats_metadatas[STAT_PP_IRPP_OBJ_IDX_VTE_LIF], prt_ctr));
    SHR_IF_ERR_EXIT(dnx_stat_pp_irpp_vis_gen_stats
                    (unit, "FWD1 forward FODO", STAT_PP_IRPP_OBJ_IDX_FWD1_FWD_FODO, stats_ids, stats_profiles,
                     stats_metadatas[STAT_PP_IRPP_OBJ_IDX_VTE_LIF], stats_metadatas[STAT_PP_IRPP_OBJ_IDX_FWD1_FWD_FODO], prt_ctr));
    SHR_IF_ERR_EXIT(dnx_stat_pp_irpp_vis_gen_stats
                    (unit, "FWD2 forward FODO", STAT_PP_IRPP_OBJ_IDX_FWD2_FWD_FODO, stats_ids, stats_profiles,
                     stats_metadatas[STAT_PP_IRPP_OBJ_IDX_FWD1_FWD_FODO], stats_metadatas[STAT_PP_IRPP_OBJ_IDX_FWD2_FWD_FODO], prt_ctr));
    SHR_IF_ERR_EXIT(dnx_stat_pp_irpp_vis_gen_stats
                    (unit, "FWD1 source lookup", STAT_PP_IRPP_OBJ_IDX_FWD1_LOOKUP_SRC, stats_ids, stats_profiles,
                     stats_metadatas[STAT_PP_IRPP_OBJ_IDX_FWD1_FWD_FODO], stats_metadatas[STAT_PP_IRPP_OBJ_IDX_FWD1_FWD_FODO], prt_ctr));
    SHR_IF_ERR_EXIT(dnx_stat_pp_irpp_vis_gen_stats
                    (unit, "FWD2 source lookup", STAT_PP_IRPP_OBJ_IDX_FWD2_LOOKUP_SRC, stats_ids, stats_profiles,
                     stats_metadatas[STAT_PP_IRPP_OBJ_IDX_FWD2_FWD_FODO], stats_metadatas[STAT_PP_IRPP_OBJ_IDX_FWD2_FWD_FODO], prt_ctr));
    SHR_IF_ERR_EXIT(dnx_stat_pp_irpp_vis_gen_stats
                    (unit, "FWD1 destination lookup", STAT_PP_IRPP_OBJ_IDX_FWD1_LOOKUP_DST, stats_ids, stats_profiles,
                     stats_metadatas[STAT_PP_IRPP_OBJ_IDX_FWD1_FWD_FODO], stats_metadatas[STAT_PP_IRPP_OBJ_IDX_FWD1_FWD_FODO], prt_ctr));
    SHR_IF_ERR_EXIT(dnx_stat_pp_irpp_vis_gen_stats
                    (unit, "FWD2 destination lookup", STAT_PP_IRPP_OBJ_IDX_FWD2_LOOKUP_DST, stats_ids, stats_profiles,
                     stats_metadatas[STAT_PP_IRPP_OBJ_IDX_FWD2_FWD_FODO], stats_metadatas[STAT_PP_IRPP_OBJ_IDX_FWD2_FWD_FODO], prt_ctr));

    SHR_IF_ERR_EXIT(dnx_stat_pp_irpp_vis_gen_pp_port_stats(unit, core, prt_ctr));

    /**fer statistics*/
    SHR_IF_ERR_EXIT(dnx_stat_pp_irpp_vis_gen_fer_stats(unit, core, prt_ctr));

    /**Forward action profile statistics*/
    SHR_IF_ERR_EXIT(dnx_stat_pp_irpp_vis_gen_fwd_act_profile_stats(unit, core, prt_ctr));

    PRT_COMMITX;

exit:
    sand_signal_output_free(stats_ids);
    sand_signal_output_free(stats_profiles);

    for (ii=0; ii<sizeof(stats_metadatas)/sizeof(stats_metadatas[0]); ++ii)
    {
        if (stats_metadatas[ii] != NULL)
            sand_signal_output_free(stats_metadatas[ii]);
    }

    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   print stat commands in trap
 * \param [in] unit - Unit ID
 * \param [in] core - Core ID
 * \param [in] object_valid - object valid string
 * \param [in] object_id - object ID string
 * \param [in] prt_ctr - print control pointer
 * \return
 *      Standard error handling
 */
static shr_error_e
dnx_stat_pp_etpp_vis_gen_stat(
    int unit,
    int core,
    char *object_valid,
    char *object_id,
    prt_control_t * prt_ctr)
{
    signal_output_t *stats = NULL;

    SHR_FUNC_INIT_VARS(unit);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    /**Stat id i*/
    SHR_IF_ERR_EXIT_NO_MSG(sand_signal_output_find(unit, core, 0, NULL, "TRAP", "CRPS", object_id, &stats));
    PRT_CELL_SET("%s", stats->print_value);
    /**valid indication*/
    SHR_IF_ERR_EXIT_NO_MSG(sand_signal_output_find(unit, core, 0, NULL, "TRAP", "CRPS", object_valid, &stats));
    PRT_CELL_SET("%s", stats->print_value);

exit:
    sand_signal_output_free(stats);
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   print last packet hits in each etpp statistic command generation point
 * \param [in] unit - Unit ID
 * \param [in] core - Core ID
 * \param [in] sand_control - pointer to framework control structure
 * \return
 *      Standard error handling
 */
static shr_error_e
dnx_stat_pp_etpp_vis(
    int unit,
    int core,
    sh_sand_control_t * sand_control)
{
    uint32 metadata = 0;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("ETPP Statistics summary");
    PRT_COLUMN_ADD("statistic object ID");
    PRT_COLUMN_ADD("statistic object ID valid");
    PRT_COLUMN_ADD("Metadata");

    SHR_IF_ERR_EXIT(dnx_stat_pp_etpp_vis_gen_stat
                    (unit, core, "Internal_Stats_Obj_0_Valid", "Internal_Stats_Obj_0_ID", prt_ctr));

    /** Print metadata. Only single instance */
    SHR_IF_ERR_EXIT_NO_MSG(dpp_dsig_read(unit, core, "ETPP", "Trap", "CRPS", "Internal_Metadata", &metadata, 1));
    PRT_CELL_SET("0x%x", metadata);

    SHR_IF_ERR_EXIT(dnx_stat_pp_etpp_vis_gen_stat
                    (unit, core, "Internal_Stats_Obj_1_Valid", "Internal_Stats_Obj_1_ID", prt_ctr));
    SHR_IF_ERR_EXIT(dnx_stat_pp_etpp_vis_gen_stat
                    (unit, core, "Internal_Stats_Obj_2_Valid", "Internal_Stats_Obj_2_ID", prt_ctr));
    SHR_IF_ERR_EXIT(dnx_stat_pp_etpp_vis_gen_stat
                    (unit, core, "Internal_Stats_Obj_3_Valid", "Internal_Stats_Obj_3_ID", prt_ctr));
    SHR_IF_ERR_EXIT(dnx_stat_pp_etpp_vis_gen_stat
                    (unit, core, "Internal_Stats_Obj_4_Valid", "Internal_Stats_Obj_4_ID", prt_ctr));
    SHR_IF_ERR_EXIT(dnx_stat_pp_etpp_vis_gen_stat
                    (unit, core, "Internal_Stats_Obj_5_Valid", "Internal_Stats_Obj_5_ID", prt_ctr));

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Vis command. Print last hits in each pp pipe statistics generation point
 */
shr_error_e
stat_pp_vis_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *block;
    int core_in, core;

    SHR_FUNC_INIT_VARS(unit);

#ifdef ADAPTER_SERVER_MODE
    SHR_ERR_EXIT(_SHR_E_NONE, "pp vis stats command in not supported on adapter mode\n");
#endif

    SH_SAND_GET_STR("block", block);
    SH_SAND_GET_INT32("core", core_in);

    DNXCMN_CORES_ITER(unit, core_in, core)
    {
        if (!ISEMPTY(block))
        {
            if (sal_strcasecmp("irpp", block) == 0)
            {
                SHR_IF_ERR_EXIT(dnx_stat_pp_irpp_vis(unit, core, sand_control));
            }
            else if (sal_strcasecmp("itpp", block) == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "ITPP is not yet supported in SDK");
            }
            else if (sal_strcasecmp("erpp", block) == 0)
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "ERPP is not yet supported in SDK");
            }
            else if (sal_strcasecmp("etpp", block) == 0)
            {
                SHR_IF_ERR_EXIT(dnx_stat_pp_etpp_vis(unit, core, sand_control));
            }
            else if (sal_strcasecmp("all", block) == 0)
            {
                SHR_IF_ERR_EXIT(dnx_stat_pp_irpp_vis(unit, core, sand_control));
                SHR_IF_ERR_EXIT(dnx_stat_pp_etpp_vis(unit, core, sand_control));
            }
            else
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "invalid input %s, valid inputs: pp vis stats block=irpp/itpp/erpp/etpp/all\n", block);
            }
        }
        /**If no block is specified - run on all blocks*/
        else
        {
            LOG_CLI((BSL_META("\nVis on all stages (currently supported: IRPP, ETPP\n")));
            SHR_IF_ERR_EXIT(dnx_stat_pp_irpp_vis(unit, core, sand_control));
            SHR_IF_ERR_EXIT(dnx_stat_pp_etpp_vis(unit, core, sand_control));
        }
    }
exit:
    SHR_FUNC_EXIT;
}

static char *
dnx_stat_pp_header_type_to_str(
    bcm_stat_pp_metadata_header_type_t header_type)
{
    if (header_type < bcmStatPpMetadataHeaderTypeCount)
    {
        return dnx_pp_header_type_desc[header_type];
    }

    return dnx_pp_header_type_desc[bcmStatPpMetadataHeaderTypeCount];
}

static shr_error_e
dnx_stat_pp_metadata_irpp_dump(
    int unit,
    sh_sand_control_t * sand_control)
{
    bcm_stat_pp_metadata_info_t metadata_info;
    bcm_stat_pp_metadata_header_type_t header_type;
    int is_unknown;
    int is_mc;
    int is_bc;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("IRPP VTT metadata summary");
    PRT_COLUMN_ADD("Header");
    PRT_COLUMN_ADD("Is_MC");
    PRT_COLUMN_ADD("Metadata");

    bcm_stat_pp_metadata_info_t_init(&metadata_info);

    metadata_info.flags = BCM_STAT_PP_METADATA_INGRESS;
    metadata_info.flags |= BCM_STAT_PP_METADATA_TUNNEL;

    for (is_mc = 0; is_mc < 2; ++is_mc)
    {
        for (header_type = bcmStatPpMetadataHeaderTypeEth;
             header_type < (is_mc ? bcmStatPpMetadataHeaderTypeIpv6 + 1 : bcmStatPpMetadataHeaderTypeCount);
             ++header_type)
        {
            metadata_info.header_type = header_type;

            /*
             * IS_MC supported for header type ETH/IPv4/IPv6 only
             */
            if (is_mc && ((header_type == bcmStatPpMetadataHeaderTypeEth) ||
                          (header_type == bcmStatPpMetadataHeaderTypeIpv4) ||
                          (header_type == bcmStatPpMetadataHeaderTypeIpv6)))
            {
                metadata_info.address_type_flags = BCM_STAT_PP_METADATA_ADDRESS_TYPE_MCAST;
            }
            else
            {
                metadata_info.address_type_flags = 0;
            }

            SHR_IF_ERR_EXIT(bcm_stat_pp_metadata_get(unit, &metadata_info));

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            PRT_CELL_SET("%s", dnx_stat_pp_header_type_to_str(header_type));
            PRT_CELL_SET("%d", is_mc);
            PRT_CELL_SET("%d", metadata_info.metadata);
        }
    }

    PRT_COMMITX;

    cli_out("\n");

    PRT_TITLE_SET("IRPP Forward metadata summary");
    PRT_COLUMN_ADD("Unknown");
    PRT_COLUMN_ADD("Broadcast");
    PRT_COLUMN_ADD("Multicast");
    PRT_COLUMN_ADD("Metadata");

    bcm_stat_pp_metadata_info_t_init(&metadata_info);

    metadata_info.flags = BCM_STAT_PP_METADATA_INGRESS;
    metadata_info.flags |= BCM_STAT_PP_METADATA_FORWARD;

    for (is_unknown = 0; is_unknown < 2; ++is_unknown)
        for (is_mc = 0; is_mc < 2; ++is_mc)
            for (is_bc = 0; is_bc < (is_mc ? 1 : 2); ++is_bc)
            {
                metadata_info.address_type_flags |= is_unknown ? 0 : BCM_STAT_PP_METADATA_ADDRESS_TYPE_UNKNOWN;
                metadata_info.address_type_flags |= is_mc ? 0 : BCM_STAT_PP_METADATA_ADDRESS_TYPE_MCAST;
                metadata_info.address_type_flags |= is_bc ? 0 : BCM_STAT_PP_METADATA_ADDRESS_TYPE_BCAST;
                SHR_IF_ERR_EXIT(bcm_stat_pp_metadata_get(unit, &metadata_info));

                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                PRT_CELL_SET("%d", is_unknown);
                PRT_CELL_SET("%d", is_mc);
                PRT_CELL_SET("%d", is_bc);
                PRT_CELL_SET("%d", metadata_info.metadata);
            }

    PRT_COMMITX;

    cli_out("\n");

    PRT_TITLE_SET("IRPP Forward_plus_one metadata summary");
    PRT_COLUMN_ADD("Header");
    PRT_COLUMN_ADD("Is_MC");
    PRT_COLUMN_ADD("Metadata");

    bcm_stat_pp_metadata_info_t_init(&metadata_info);

    metadata_info.flags = BCM_STAT_PP_METADATA_INGRESS;
    metadata_info.flags |= BCM_STAT_PP_METADATA_FORWARD_PLUS_ONE;

    for (is_mc = 0; is_mc < 2; ++is_mc)
    {
        for (header_type = bcmStatPpMetadataHeaderTypeEth;
             header_type < (is_mc ? bcmStatPpMetadataHeaderTypeIpv6 + 1 : bcmStatPpMetadataHeaderTypeCount);
             ++header_type)
        {
            metadata_info.header_type = header_type;

            /*
             * IS_MC supported for header type ETH/IPv4/IPv6 only
             */
            if (is_mc && ((header_type == bcmStatPpMetadataHeaderTypeEth) ||
                          (header_type == bcmStatPpMetadataHeaderTypeIpv4) ||
                          (header_type == bcmStatPpMetadataHeaderTypeIpv6)))
            {
                metadata_info.address_type_flags = BCM_STAT_PP_METADATA_ADDRESS_TYPE_MCAST;
            }
            else
            {
                metadata_info.address_type_flags = 0;
            }

            SHR_IF_ERR_EXIT(bcm_stat_pp_metadata_get(unit, &metadata_info));

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            PRT_CELL_SET("%s", dnx_stat_pp_header_type_to_str(header_type));
            PRT_CELL_SET("%d", is_mc);
            PRT_CELL_SET("%d", metadata_info.metadata);
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
dnx_stat_pp_metadata_etpp_dump(
    int unit,
    sh_sand_control_t * sand_control)
{
    bcm_stat_pp_metadata_info_t metadata_info;
    bcm_stat_pp_metadata_header_type_t header_type;
    int supported_fwd_code[] = {
        bcmStatPpMetadataHeaderTypeEth,
        bcmStatPpMetadataHeaderTypeIpv4,
        bcmStatPpMetadataHeaderTypeIpv6,
        bcmStatPpMetadataHeaderTypeMpls,
        bcmStatPpMetadataHeaderTypeSrv6,
        bcmStatPpMetadataHeaderTypeOthers
    };
    int is_ip;
    int is_mc;
    int ii;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("ETPP Forward metadata summary");
    PRT_COLUMN_ADD("Fwd Code");
    PRT_COLUMN_ADD("Cast");
    PRT_COLUMN_ADD("Metadata");

    bcm_stat_pp_metadata_info_t_init(&metadata_info);

    metadata_info.flags = BCM_STAT_PP_METADATA_EGRESS;
    metadata_info.flags |= BCM_STAT_PP_METADATA_FORWARD;

    for (ii = 0; ii < sizeof(supported_fwd_code) / sizeof(supported_fwd_code[0]); ++ii)
    {
        is_ip = (supported_fwd_code[ii] == bcmStatPpMetadataHeaderTypeIpv4) ||
            (supported_fwd_code[ii] == bcmStatPpMetadataHeaderTypeIpv6);

        for (is_mc = 0; is_mc < (1 + is_ip); ++is_mc)
        {
            metadata_info.header_type = supported_fwd_code[ii];
            metadata_info.address_type_flags = is_ip ? (BCM_STAT_PP_METADATA_ADDRESS_TYPE_UCAST + is_mc) : 0;
            SHR_IF_ERR_EXIT(bcm_stat_pp_metadata_get(unit, &metadata_info));

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            PRT_CELL_SET("%s", dnx_stat_pp_header_type_to_str(supported_fwd_code[ii]));
            PRT_CELL_SET("%s", is_ip ? (is_mc ? "MC" : "UC") : "-");
            PRT_CELL_SET("%d", metadata_info.metadata);
        }
    }

    PRT_COMMITX;

    cli_out("\n");

    PRT_TITLE_SET("ETPP Tunnel metadata summary");
    PRT_COLUMN_ADD("Layer Type");
    PRT_COLUMN_ADD("Metadata");

    bcm_stat_pp_metadata_info_t_init(&metadata_info);

    metadata_info.flags = BCM_STAT_PP_METADATA_EGRESS;
    metadata_info.flags |= BCM_STAT_PP_METADATA_TUNNEL;

    for (header_type = bcmStatPpMetadataHeaderTypeEth; header_type < bcmStatPpMetadataHeaderTypeCount; ++header_type)
    {
        metadata_info.header_type = header_type;
        SHR_IF_ERR_EXIT(bcm_stat_pp_metadata_get(unit, &metadata_info));

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
        PRT_CELL_SET("%s", dnx_stat_pp_header_type_to_str(header_type));
        PRT_CELL_SET("%d", metadata_info.metadata);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   Dump command. Get block name (IRPP/ITPP/ERPP/ETPP)
 *   From the user. Validate input and call dnx_stat_pp_block_dump to print stat_pp configuration
 */
shr_error_e
stat_pp_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    char *block;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_STR("block", block);

    if (!ISEMPTY(block))
    {
        if (sal_strcasecmp("irpp", block) == 0)
        {
            SHR_IF_ERR_EXIT(dnx_stat_pp_block_dump(unit, "IRPP", DBAL_TABLE_STAT_PP_IRPP_PROFILE_INFO, sand_control));
            SHR_IF_ERR_EXIT(dnx_stat_pp_metadata_irpp_dump(unit, sand_control));
        }
        else if (sal_strcasecmp("itpp", block) == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "ITPP is not yet supported in SDK");
        }
        else if (sal_strcasecmp("erpp", block) == 0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "ERPP is not yet supported in SDK");
        }
        else if (sal_strcasecmp("etpp", block) == 0)
        {
            SHR_IF_ERR_EXIT(dnx_stat_pp_block_dump(unit, "ETPP", DBAL_TABLE_STAT_PP_ETPP_PROFILE_INFO, sand_control));
            SHR_IF_ERR_EXIT(dnx_stat_pp_metadata_etpp_dump(unit, sand_control));
        }
        else if (sal_strcasecmp("all", block) == 0)
        {
            SHR_IF_ERR_EXIT(dnx_stat_pp_block_dump(unit, "IRPP", DBAL_TABLE_STAT_PP_IRPP_PROFILE_INFO, sand_control));
            SHR_IF_ERR_EXIT(dnx_stat_pp_block_dump(unit, "ETPP", DBAL_TABLE_STAT_PP_ETPP_PROFILE_INFO, sand_control));
            SHR_IF_ERR_EXIT(dnx_stat_pp_metadata_irpp_dump(unit, sand_control));
            SHR_IF_ERR_EXIT(dnx_stat_pp_metadata_etpp_dump(unit, sand_control));
        }
        else
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "invalid input %s, valid inputs: stat pp dump block=irpp/itpp/erpp/etpp/all\n", block);
        }
    }
    /**If no block is specified - run on all blocks*/
    else
    {
        SHR_IF_ERR_EXIT(dnx_stat_pp_block_dump(unit, "IRPP", DBAL_TABLE_STAT_PP_IRPP_PROFILE_INFO, sand_control));
        SHR_IF_ERR_EXIT(dnx_stat_pp_block_dump(unit, "ETPP", DBAL_TABLE_STAT_PP_ETPP_PROFILE_INFO, sand_control));
        SHR_IF_ERR_EXIT(dnx_stat_pp_metadata_irpp_dump(unit, sand_control));
        SHR_IF_ERR_EXIT(dnx_stat_pp_metadata_etpp_dump(unit, sand_control));
    }

exit:
    SHR_FUNC_EXIT;
}

sh_sand_option_t dnx_stat_pp_options[] = {
    {"block", SAL_FIELD_TYPE_STR, "block statistic info", ""}
    ,
    {NULL}
};

/**
 * \brief
 *   Manual for the main stat_pp command
 */
sh_sand_man_t sh_dnx_stat_pp_man = {
    .brief = "PP statistics command generation summary",
    .full =
        "Show per stage statistic Id, metadata, stat profile generated by it, and the final statistic command block output (command id + profile results). In order to see counters values, use crps get diagnostics: CRPS Get  [engine=<...>]  [database=<...>]  set=<...>  type=<...>.",
    .synopsis = "Block=<Block name>",
    .init_cb = diag_sand_packet_send_simple,
    .examples = "block=IRPP\n",
    .compatibility = DIAG_DNX_PP_BARE_METAL_NOT_AVAILABLE_COMMAND
};
