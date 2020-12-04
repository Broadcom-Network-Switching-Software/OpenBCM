/** \file src/appl/diag/dnx/tm/diag_dnx_snif.c
 *
 * DNX SNIF diagnostics
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_COSQ

/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <soc/sand/sand_signals.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_snif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stif.h>
#include <src/bcm/dnx/mirror/mirror_profile_alloc.h>
#include <bcm_int/dnx/cmn/dnxcmn.h>

#include "diag_dnx_snif.h"

/*
 * --------------------------------------------------------------------------
 * dnx tm snif
 * --------------------------------------------------------------------------
 */

/*
 * Define the short strings for Sinf dignostic options.
 */
/* *INDENT-OFF* }*/
static sh_sand_enum_t sand_diag_snif_type[] = {
    /**String         Value*/
    {"Snoop",         DNX_SNIF_TYPE_SNOOP,               "SNIF type Snoop"},
    {"Mirror",        DNX_SNIF_TYPE_MIRROR,              "SNIF type Mirror"},
    {"Stat_sample",   DNX_SNIF_TYPE_STATISTICAL_SAMPLE,  "SNIF type Stasistical Sample"},
    {"ALL",           DNX_SNIF_TYPE_COUNT,               "All SNIF types"},
    {NULL}
};

static sh_sand_enum_t sand_diag_snif[] = {
    /**String      Value*/
    {"ing",        DNX_SNIF_INGRESS,    "Ingress"},
    {"egr",        DNX_SNIF_EGRESS,     "Egress"},
    {"ALL",        DNX_SNIF_COUNT,      "All (Ingress and Egress)"},
    {NULL}
};
/* *INDENT-ON* }*/

sh_sand_option_t sh_dnx_snif_options[] = {
/* *INDENT-OFF* */
    /*Name           Type                  Description                        Default Extension*/
    {"Direction",    SAL_FIELD_TYPE_ENUM,  "Ingress or Egress",               "ALL",  (void *) sand_diag_snif, NULL, SH_SAND_ARG_FREE},
    {"Type",         SAL_FIELD_TYPE_ENUM,  "SNIF type",                       "ALL",  (void *) sand_diag_snif_type},
    {"Command_ID",   SAL_FIELD_TYPE_INT32, "Command ID",                      "-1",   NULL},
    {"Enabled_Only", SAL_FIELD_TYPE_BOOL,  "Show only enabled SNIF profiles", "0",    NULL},
    {NULL}
/* *INDENT-ON* */
};

sh_sand_man_t sh_dnx_snif_man = {
    "Present SNIF command attributes",
    "Present SNIF command attributes for a specified SNIF type and command ID.\n"
        "First parameter indicates which attributes to show and can be is ingress, egress or all. If it is not specified all will be shown"
        "Type can be Mirror, Snoop or Stat_sample. If no type is specified, the specified command ID will be shown for all possible types.\n"
        "Command ID can be in the range 0-7(8 profiles in total) for egress and range 0-31(32 profiles in total) for ingress attributes.\n"
        "You can specify a single command ID or a range in the following format: Command_ID=start-end. If no command ID is specified, all command IDs for the specified type will be shown.\n"
        "If no parameters at all are specified, all command IDs for all types, both ingress and egress attributes will be shown.\n"
        "If Enabled is set to 1 only attributes of enabled command IDs(profiles) will be shown.\n",
    "ing/egr/all Type=<SNIF Type> Command_ID=<Command(Profile) ID> Enabled_Only=<0/1>",
    "ing Type=Mirror Command_ID=5\n" "egr Type=Snoop Command_ID=7\n"
        "ing Type=Stat_sample Command_ID=10-25 Enabled_Only=1\n"
};

/**
 * See .h file
 */
shr_error_e
sh_dnx_snif_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 entry_handle_id_ing;
    uint32 entry_handle_id_egr;
    uint32 entry_handle_id_count;
    uint32 entry_handle_id_snoop_str;
    uint32 entry_handle_id_header;
    uint32 entry_handle_id_new_header;
    dnx_ing_egr_e ingr_egr;
    int cmd_id, cmd_id_start, cmd_id_end, cmd_id_max;
    uint32 type, type_start, type_end;
    uint8 is_allocated = TRUE, enabled_only;
    uint32 destination = 0;
    uint32 lag_lb_key_ow = 0, lag_lb_key = 0;
    uint32 tc_ow = 0, tc = 0;
    uint32 dp_ow = 0, dp = 0;
    uint32 crop_enable = 0;
    uint32 drop_sniff_if_fwd_dropped = 0;
    uint32 drop_fwd_if_sniff_dropped = 0;
    uint32 in_pp_port_ow = 0, in_pp_port = 0;
    uint32 st_vsq_ptr_ow = 0, st_vsq_ptr = 0;
    uint32 admit_profile_ow = 0, admit_profile = 0;
    uint32 snif_probability = 0;
    uint32 enable = 0, snoop_strength = 0;
    uint32 snif_egress_probability = 0;
    uint32 snif_rcy_priority = 0;
    uint32 snif_append_orig_headers = 0;
    uint32 snif_crop_enable = 0;
    uint32 snif_rcy_cmd = 0;
    uint32 ext_stat_data_valid_ow[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    uint32 ext_stat_data_valid[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    uint32 ext_stat_data_opcode_ow[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    uint32 ext_stat_data_type_id[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][4];
    uint32 ext_stat_data_ow[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    uint32 ext_stat_data[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES][4];
    uint32 int_stat_obj_valid_ow[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    uint32 int_stat_obj_valid[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    uint32 int_stat_obj_id_ow[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    uint32 int_stat_obj_id[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    uint32 int_stat_obj_type_ow[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    uint32 int_stat_obj_type[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    uint32 int_stat_obj_meta_data_ow[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    uint32 int_stat_obj_meta_data[DNX_DATA_MAX_DEVICE_GENERAL_NOF_CORES] = { 0 };
    uint32 dsp_ext_destination = 0, dsp_ext_destination_stamp_en = 0;
    uint32 remove_system_headers = 0, keep_incoming_system_headrs = 0;
    uint32 sniff_append_mode = 0;
    uint32 tm_action_type = 0, tm_action_type_mask = 0;
    uint32 is_mc = 0, is_mc_mask = 0;
    uint32 mc_outlif, mc_outlif_mask = 0;
    uint32 new_system_header[14] = { 0 };
    uint32 header_size = 0;
    uint32 ftmh_stacking_ext_offset = 0;
    uint32 application_ext_stamp_en = 0, application_ext_offset = 0;
    uint32 app_ext_type = 0;
    uint32 pph_fhei_stamp_en = 0;
    uint32 pph_fhei_ext_offset = 0;
    char string[DSIG_MAX_SIZE_STR];
    bcm_core_t core_id, core;
    int i = 0;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Get input parameters
     */
    SH_SAND_GET_ENUM("direction", ingr_egr);
    SH_SAND_GET_ENUM("Type", type);
    SH_SAND_GET_INT32_RANGE("Command_ID", cmd_id_start, cmd_id_end);
    SH_SAND_GET_BOOL("Enabled_Only", enabled_only);
    SH_SAND_GET_INT32("core", core);

    /*
     * If specified type is ALL, we need to display all types of SNIF
     */
    if (type == DNX_SNIF_TYPE_COUNT)
    {
        type_start = DNX_SNIF_TYPE_FIRST;
        type_end = DNX_SNIF_TYPE_COUNT - 1;
    }
    else
    {
        type_start = type;
        type_end = type;
    }

    /*
     * If specified command ID is ALL, we need to display all types of Snif
     */
    cmd_id_max = (ingr_egr == DNX_SNIF_EGRESS ? dnx_data_snif.egress.mirror_snoop_nof_profiles_get(unit) :
                  dnx_data_snif.ingress.nof_profiles_get(unit)) - 1;
    if ((cmd_id_start == -1) && (cmd_id_end == -1))
    {
        cmd_id_start = 0;
        cmd_id_end = cmd_id_max;
    }
    else
    {
        /*
         * Verify that the specified command ID are in range
         */
        SHR_RANGE_VERIFY(cmd_id_start, 0, cmd_id_max, _SHR_E_PARAM, "Minimum command ID out of range.\n");
        SHR_RANGE_VERIFY(cmd_id_end, cmd_id_start, cmd_id_max, _SHR_E_PARAM, "Maximum command ID out of range.\n");
    }

    /*
     * Allocate handles for the used DBAL tables
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_COMMAND_TABLE, &entry_handle_id_ing));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_EGRESS_COMMAND_TABLE, &entry_handle_id_egr));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_COUNTING_TABLE, &entry_handle_id_count));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_RCY_CMD_MAP, &entry_handle_id_snoop_str));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_SYSTEM_HEADER_TABLE, &entry_handle_id_header));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SNIF_NEW_SYSTEM_HEADER_TABLE, &entry_handle_id_new_header));

    /*
     * Iterate over the types
     */
    for (type = type_start; type <= type_end; type++)
    {
        /*
         * Iterate over the command IDs
         */
        for (cmd_id = cmd_id_start; cmd_id <= cmd_id_end; cmd_id++)
        {
            /*
             * If Enabled is set, check if the profile is allocated
             */
            if (enabled_only == 1)
            {
                is_allocated = TRUE;
                switch (type)
                {
                    case DNX_SNIF_TYPE_SNOOP:
                    {
                        if (!(cmd_id == 0))
                        {
                            SHR_IF_ERR_EXIT(dnx_mirror_db.snif_ingress_profiles_snoop.is_allocated
                                            (unit, cmd_id, &is_allocated));
                        }
                        break;
                    }
                    case DNX_SNIF_TYPE_MIRROR:
                    {
                        if (!(cmd_id == 0))
                        {
                            SHR_IF_ERR_EXIT(dnx_mirror_db.snif_ingress_profiles_mirror.is_allocated
                                            (unit, cmd_id, &is_allocated));
                        }
                        break;
                    }
                    case DNX_SNIF_TYPE_STATISTICAL_SAMPLE:
                    {
                        if (!(cmd_id == 0))
                        {
                            SHR_IF_ERR_EXIT(dnx_mirror_db.snif_ingress_profiles_stat_sampling.is_allocated
                                            (unit, cmd_id, &is_allocated));
                        }
                        break;
                    }
                    default:
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM, "Type is invalid.\n");
                    }
                }

                if (cmd_id == 0)
                {
                    /** Profile 0 is special, profile 0 is reserved as default in HW and means don't mirror */
                    is_allocated = FALSE;
                }

            }

            if (is_allocated == TRUE)
            {
                /*
                 * Iterate over the cores
                 */
                DNXCMN_CORES_ITER(unit, core, core_id)
                {
                    /*
                     * Print table header
                     */
                    PRT_TITLE_SET("Showing TM attributes of %s command(profile) %d", sand_diag_snif_type[type].string,
                                  cmd_id);

                    /*
                     * Print SNIF diagnostic
                     */
                    PRT_COLUMN_ADD("ATTRIBUTES");
                    PRT_COLUMN_ADD_FLEX(PRT_FLEX_BINARY, "CORE %d", core_id);

                    if ((ingr_egr == DNX_SNIF_INGRESS) || (ingr_egr == DNX_SNIF_COUNT))
                    {
                        /*
                         * Get the information for the ingress attributes to be presented from the table
                         */
                        dbal_entry_key_field32_set(unit, entry_handle_id_ing, DBAL_FIELD_SNIF_COMMAND_ID, cmd_id);
                        dbal_entry_key_field32_set(unit, entry_handle_id_ing, DBAL_FIELD_SNIF_TYPE, type);
                        dbal_value_field32_request(unit, entry_handle_id_ing, DBAL_FIELD_DESTINATION, INST_SINGLE,
                                                   &destination);
                        dbal_value_field32_request(unit, entry_handle_id_ing, DBAL_FIELD_LAG_LB_KEY_OW, INST_SINGLE,
                                                   &lag_lb_key_ow);
                        dbal_value_field32_request(unit, entry_handle_id_ing, DBAL_FIELD_LAG_LB_KEY, INST_SINGLE,
                                                   &lag_lb_key);
                        dbal_value_field32_request(unit, entry_handle_id_ing, DBAL_FIELD_TC_OW, INST_SINGLE, &tc_ow);
                        dbal_value_field32_request(unit, entry_handle_id_ing, DBAL_FIELD_TC, INST_SINGLE, &tc);
                        dbal_value_field32_request(unit, entry_handle_id_ing, DBAL_FIELD_DP_OW, INST_SINGLE, &dp_ow);
                        dbal_value_field32_request(unit, entry_handle_id_ing, DBAL_FIELD_DP, INST_SINGLE, &dp);
                        dbal_value_field32_request(unit, entry_handle_id_ing, DBAL_FIELD_CROP_ENABLE, INST_SINGLE,
                                                   &crop_enable);
                        dbal_value_field32_request(unit, entry_handle_id_ing, DBAL_FIELD_DROP_SNIFF_IF_FWD_DROPPED,
                                                   INST_SINGLE, &drop_sniff_if_fwd_dropped);
                        dbal_value_field32_request(unit, entry_handle_id_ing, DBAL_FIELD_DROP_FWD_IF_SNIFF_DROPPED,
                                                   INST_SINGLE, &drop_fwd_if_sniff_dropped);
                        dbal_value_field32_request(unit, entry_handle_id_ing, DBAL_FIELD_IN_PP_PORT_OW, INST_SINGLE,
                                                   &in_pp_port_ow);
                        dbal_value_field32_request(unit, entry_handle_id_ing, DBAL_FIELD_IN_PP_PORT, INST_SINGLE,
                                                   &in_pp_port);
                        dbal_value_field32_request(unit, entry_handle_id_ing, DBAL_FIELD_ST_VSQ_PTR_OW, INST_SINGLE,
                                                   &st_vsq_ptr_ow);
                        dbal_value_field32_request(unit, entry_handle_id_ing, DBAL_FIELD_ST_VSQ_PTR, INST_SINGLE,
                                                   &st_vsq_ptr);
                        dbal_value_field32_request(unit, entry_handle_id_ing, DBAL_FIELD_ADMIT_PROFILE_OW, INST_SINGLE,
                                                   &admit_profile_ow);
                        dbal_value_field32_request(unit, entry_handle_id_ing, DBAL_FIELD_ADMIT_PROFILE, INST_SINGLE,
                                                   &admit_profile);
                        dbal_value_field32_request(unit, entry_handle_id_ing, DBAL_FIELD_SNIF_PROBABILITY, INST_SINGLE,
                                                   &snif_probability);
                        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_ing, DBAL_COMMIT));

                        /*
                         * Get the information for the FTMH header construction to be presented from the table
                         */
                        dbal_entry_key_field32_set(unit, entry_handle_id_header, DBAL_FIELD_SNIF_COMMAND_ID, cmd_id);
                        dbal_entry_key_field32_set(unit, entry_handle_id_header, DBAL_FIELD_SNIF_TYPE, type);
                        dbal_value_field32_request(unit, entry_handle_id_header, DBAL_FIELD_DSP_EXT_DESTINATION,
                                                   INST_SINGLE, &dsp_ext_destination);
                        dbal_value_field32_request(unit, entry_handle_id_header,
                                                   DBAL_FIELD_DSP_EXT_DESTINATION_STAMP_EN, INST_SINGLE,
                                                   &dsp_ext_destination_stamp_en);
                        dbal_value_field32_request(unit, entry_handle_id_header, DBAL_FIELD_REMOVE_SYSTEM_HEADERS,
                                                   INST_SINGLE, &remove_system_headers);
                        dbal_value_field32_request(unit, entry_handle_id_header, DBAL_FIELD_KEEP_INCOMING_SYSTEM_HEADRS,
                                                   INST_SINGLE, &keep_incoming_system_headrs);
                        dbal_value_field32_request(unit, entry_handle_id_header, DBAL_FIELD_SNIFF_APPEND_MODE,
                                                   INST_SINGLE, &sniff_append_mode);
                        dbal_value_field32_request(unit, entry_handle_id_header, DBAL_FIELD_TM_ACTION_TYPE, INST_SINGLE,
                                                   &tm_action_type);
                        dbal_value_field32_request(unit, entry_handle_id_header, DBAL_FIELD_TM_ACTION_TYPE_MASK,
                                                   INST_SINGLE, &tm_action_type_mask);
                        dbal_value_field32_request(unit, entry_handle_id_header, DBAL_FIELD_IS_MC, INST_SINGLE, &is_mc);
                        dbal_value_field32_request(unit, entry_handle_id_header, DBAL_FIELD_IS_MC_MASK, INST_SINGLE,
                                                   &is_mc_mask);
                        dbal_value_field32_request(unit, entry_handle_id_header, DBAL_FIELD_MC_OUTLIF, INST_SINGLE,
                                                   &mc_outlif);
                        dbal_value_field32_request(unit, entry_handle_id_header, DBAL_FIELD_MC_OUTLIF_MASK, INST_SINGLE,
                                                   &mc_outlif_mask);
                        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_header, DBAL_COMMIT));
                        /*
                         * Get the information for the new header if the SNIF append mode is 1
                         */
                        dbal_entry_key_field32_set(unit, entry_handle_id_new_header, DBAL_FIELD_SNIF_COMMAND_ID,
                                                   cmd_id);
                        dbal_entry_key_field32_set(unit, entry_handle_id_new_header, DBAL_FIELD_SNIF_TYPE, type);
                        dbal_value_field_arr32_request(unit, entry_handle_id_new_header, DBAL_FIELD_NEW_SYSTEM_HEADER,
                                                       INST_SINGLE, new_system_header);
                        dbal_value_field32_request(unit, entry_handle_id_new_header, DBAL_FIELD_HEADER_SIZE,
                                                   INST_SINGLE, &header_size);
                        dbal_value_field32_request(unit, entry_handle_id_new_header,
                                                   DBAL_FIELD_FTMH_STACKING_EXT_OFFSET, INST_SINGLE,
                                                   &ftmh_stacking_ext_offset);
                        dbal_value_field32_request(unit, entry_handle_id_new_header,
                                                   DBAL_FIELD_APPLICATION_EXT_STAMP_EN, INST_SINGLE,
                                                   &application_ext_stamp_en);
                        dbal_value_field32_request(unit, entry_handle_id_new_header, DBAL_FIELD_APPLICATION_EXT_OFFSET,
                                                   INST_SINGLE, &application_ext_offset);
                        dbal_value_field32_request(unit, entry_handle_id_new_header, DBAL_FIELD_APP_EXT_TYPE,
                                                   INST_SINGLE, &app_ext_type);
                        dbal_value_field32_request(unit, entry_handle_id_new_header, DBAL_FIELD_PPH_FHEI_STAMP_EN,
                                                   INST_SINGLE, &pph_fhei_stamp_en);
                        dbal_value_field32_request(unit, entry_handle_id_new_header, DBAL_FIELD_PPH_FHEI_EXT_OFFSET,
                                                   INST_SINGLE, &pph_fhei_ext_offset);
                        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_new_header, DBAL_COMMIT));

                        /*
                         * Present the information
                         */
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("Ingress Attributes:");
                        PRT_CELL_SET("");

                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                        PRT_CELL_SET("%s", "Destination");
                        PRT_CELL_SET("0x%x", destination);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "LAG Load Balancing Key Overwrite");
                        PRT_CELL_SET("0x%x", lag_lb_key_ow);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "LAG Load Balancing Key");
                        PRT_CELL_SET("0x%x", lag_lb_key);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "Traffic Class Overwrite");
                        PRT_CELL_SET("0x%x", tc_ow);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "Traffic Class");
                        PRT_CELL_SET("0x%x", tc);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "Drop Precedence Overwrite");
                        PRT_CELL_SET("0x%x", dp_ow);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "Drop Precedence");
                        PRT_CELL_SET("0x%x", dp);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "Crop enabled");
                        PRT_CELL_SET("0x%x", crop_enable);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "Drop Snif if forward copy is dropped");
                        PRT_CELL_SET("0x%x", drop_sniff_if_fwd_dropped);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "Drop forward copy if Snif is dropped");
                        PRT_CELL_SET("0x%x", drop_fwd_if_sniff_dropped);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "In PP Port Overwrite");
                        PRT_CELL_SET("0x%x", in_pp_port_ow);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "In PP Port");
                        PRT_CELL_SET("0x%x", in_pp_port);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "ST VSQ pointer Overwrite");
                        PRT_CELL_SET("0x%x", st_vsq_ptr_ow);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "ST VSQ pointer");
                        PRT_CELL_SET("0x%x", st_vsq_ptr);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "Admit Profile Overwrite");
                        PRT_CELL_SET("0x%x", admit_profile_ow);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "Admit Profile");
                        PRT_CELL_SET("0x%x", admit_profile);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "Snif Probability");
                        PRT_CELL_SET("0x%x", snif_probability);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "DSP Ext Destination");
                        PRT_CELL_SET("0x%x", dsp_ext_destination);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "DSP Ext Destination Stamp Enable");
                        PRT_CELL_SET("0x%x", dsp_ext_destination_stamp_en);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "Remove System Headers");
                        PRT_CELL_SET("0x%x", remove_system_headers);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "Keerp Incoming System Headers");
                        PRT_CELL_SET("0x%x", keep_incoming_system_headrs);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "SNIF Append Mode");
                        PRT_CELL_SET("0x%x", sniff_append_mode);

                        if (sniff_append_mode == 1)
                        {
                            sand_signal_value_to_str(unit, SAL_FIELD_TYPE_ARRAY32, new_system_header, string, 448, 0);
                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                            PRT_CELL_SET("%s", "New System Header");
                            PRT_CELL_SET("%s", string);

                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                            PRT_CELL_SET("%s", "Header Size");
                            PRT_CELL_SET("0x%x", header_size);

                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                            PRT_CELL_SET("%s", "PPH FHEI Ext Stamp Enable");
                            PRT_CELL_SET("0x%x", pph_fhei_stamp_en);

                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                            PRT_CELL_SET("%s", "PPH FHEI Ext Offset");
                            PRT_CELL_SET("0x%x", pph_fhei_ext_offset);
                        }

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "TM Action Type");
                        PRT_CELL_SET("0x%x", tm_action_type);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "TM Action Type Mask");
                        PRT_CELL_SET("0x%x", tm_action_type_mask);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "Is Multicast");
                        PRT_CELL_SET("0x%x", is_mc);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "Is Multicast Mask");
                        PRT_CELL_SET("0x%x", is_mc_mask);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "Multicast Outlif");
                        PRT_CELL_SET("0x%x", mc_outlif);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "Multicast Outlif Mask");
                        PRT_CELL_SET("0x%x", mc_outlif_mask);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "FTMH Stacking Ext Offset");
                        PRT_CELL_SET("0x%x", ftmh_stacking_ext_offset);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "Application Ext Stamp Enable");
                        PRT_CELL_SET("0x%x", application_ext_stamp_en);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "Application Ext Offset");
                        PRT_CELL_SET("0x%x", application_ext_offset);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "Application Ext Type");
                        PRT_CELL_SET("0x%x", app_ext_type);
                    }

                    if (((ingr_egr == DNX_SNIF_EGRESS) || (ingr_egr == DNX_SNIF_COUNT))
                        && (type != DNX_SNIF_TYPE_STATISTICAL_SAMPLE)
                        && (cmd_id < (int) dnx_data_snif.egress.mirror_snoop_nof_profiles_get(unit)))
                    {
                        /*
                         * Get the information for the eggress attributes to be presented from the table
                         */
                        dbal_entry_key_field32_set(unit, entry_handle_id_egr, DBAL_FIELD_SNIF_COMMAND_ID, cmd_id);
                        dbal_entry_key_field32_set(unit, entry_handle_id_egr, DBAL_FIELD_SNIF_TYPE, type);
                        dbal_value_field32_request(unit, entry_handle_id_egr, DBAL_FIELD_ENABLE, INST_SINGLE, &enable);
                        dbal_value_field32_request(unit, entry_handle_id_egr, DBAL_FIELD_SNIF_EGRESS_PROBABILITY,
                                                   INST_SINGLE, &snif_egress_probability);
                        dbal_value_field32_request(unit, entry_handle_id_egr, DBAL_FIELD_SNIF_RCY_PRIORITY, INST_SINGLE,
                                                   &snif_rcy_priority);
                        dbal_value_field32_request(unit, entry_handle_id_egr, DBAL_FIELD_SNIF_APPEND_ORIG_HEADERS,
                                                   INST_SINGLE, &snif_append_orig_headers);
                        dbal_value_field32_request(unit, entry_handle_id_egr, DBAL_FIELD_SNIF_CROP_ENABLE, INST_SINGLE,
                                                   &snif_crop_enable);
                        dbal_value_field32_request(unit, entry_handle_id_egr, DBAL_FIELD_SNIF_RCY_CMD, INST_SINGLE,
                                                   &snif_rcy_cmd);
                        SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_egr, DBAL_COMMIT));

                        /*
                         * Present the information
                         */
                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                        PRT_CELL_SET("Egress Attributes:");
                        PRT_CELL_SET("");

                        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                        PRT_CELL_SET("%s", "Enable");
                        PRT_CELL_SET("0x%x", enable);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "Snif Egress Probability");
                        PRT_CELL_SET("0x%x", snif_egress_probability);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "Snif Recycle Priority");
                        PRT_CELL_SET("0x%x", snif_rcy_priority);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "Append original headers to Snif copy");
                        PRT_CELL_SET("0x%x", snif_append_orig_headers);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "Snif Crop Enabled");
                        PRT_CELL_SET("0x%x", snif_crop_enable);

                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s", "Snif Recycle command");
                        PRT_CELL_SET("0x%x", snif_rcy_cmd);

                        /*
                         * If the type is Snoop for egress we have one extra attribute to be presented.
                         */
                        if (type == DNX_SNIF_TYPE_SNOOP)
                        {
                            /*
                             * Get the information for the Snoop strength
                             */
                            dbal_entry_key_field32_set(unit, entry_handle_id_snoop_str, DBAL_FIELD_RECYCLE_COMMAND_ID,
                                                       cmd_id);
                            dbal_value_field32_request(unit, entry_handle_id_snoop_str, DBAL_FIELD_RCY_SNOOP_STRENGTH,
                                                       INST_SINGLE, &snoop_strength);
                            SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_snoop_str, DBAL_COMMIT));

                            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                            PRT_CELL_SET("%s", "Snoop Strength");
                            PRT_CELL_SET("0x%x", snoop_strength);

                        }
                    }

                    /*
                     * Get the information for the counting attributes to be presented from the table
                     */
                    dbal_entry_key_field32_set(unit, entry_handle_id_count, DBAL_FIELD_SNIF_COMMAND_ID, cmd_id);
                    dbal_entry_key_field32_set(unit, entry_handle_id_count, DBAL_FIELD_SNIF_TYPE, type);
                    /*
                     * Get the information for current core
                     */
                    dbal_entry_key_field32_set(unit, entry_handle_id_count, DBAL_FIELD_CORE_ID, core_id);
                    dbal_value_field32_request(unit, entry_handle_id_count, DBAL_FIELD_EXT_STAT_DATA_VALID_OW,
                                               INST_SINGLE, &ext_stat_data_valid_ow[core_id]);
                    dbal_value_field32_request(unit, entry_handle_id_count, DBAL_FIELD_EXT_STAT_DATA_VALID, INST_SINGLE,
                                               &ext_stat_data_valid[core_id]);
                    dbal_value_field32_request(unit, entry_handle_id_count, DBAL_FIELD_EXT_STAT_DATA_OPCODE_OW,
                                               INST_SINGLE, &ext_stat_data_opcode_ow[core_id]);
                    dbal_value_field32_request(unit, entry_handle_id_count, DBAL_FIELD_EXT_STAT_DATA_OW, INST_SINGLE,
                                               &ext_stat_data_ow[core_id]);
                    for (i = 0; i < dnx_data_stif.report.stat_id_max_count_get(unit); i++)
                    {
                        dbal_value_field32_request(unit, entry_handle_id_count, DBAL_FIELD_EXT_STAT_DATA_ID, i,
                                                   &ext_stat_data[core_id][i]);
                        dbal_value_field32_request(unit, entry_handle_id_count, DBAL_FIELD_EXT_STAT_DATA_OPCODE, i,
                                                   &ext_stat_data_type_id[core_id][i]);
                    }
                    dbal_value_field32_request(unit, entry_handle_id_count, DBAL_FIELD_INT_STAT_OBJ_VALID_OW,
                                               INST_SINGLE, &int_stat_obj_valid_ow[core_id]);
                    dbal_value_field32_request(unit, entry_handle_id_count, DBAL_FIELD_INT_STAT_OBJ_VALID, INST_SINGLE,
                                               &int_stat_obj_valid[core_id]);
                    dbal_value_field32_request(unit, entry_handle_id_count, DBAL_FIELD_INT_STAT_OBJ_ID_OW, INST_SINGLE,
                                               &int_stat_obj_id_ow[core_id]);
                    dbal_value_field32_request(unit, entry_handle_id_count, DBAL_FIELD_INT_STAT_OBJ_ID, INST_SINGLE,
                                               &int_stat_obj_id[core_id]);
                    dbal_value_field32_request(unit, entry_handle_id_count, DBAL_FIELD_INT_STAT_OBJ_TYPE_OW,
                                               INST_SINGLE, &int_stat_obj_type_ow[core_id]);
                    dbal_value_field32_request(unit, entry_handle_id_count, DBAL_FIELD_INT_STAT_OBJ_TYPE, INST_SINGLE,
                                               &int_stat_obj_type[core_id]);
                    dbal_value_field32_request(unit, entry_handle_id_count, DBAL_FIELD_INT_STAT_OBJ_META_DATA_OW,
                                               INST_SINGLE, &int_stat_obj_meta_data_ow[core_id]);
                    dbal_value_field32_request(unit, entry_handle_id_count, DBAL_FIELD_INT_STAT_OBJ_META_DATA,
                                               INST_SINGLE, &int_stat_obj_meta_data[core_id]);
                    SHR_IF_ERR_EXIT(dbal_entry_get(unit, entry_handle_id_count, DBAL_COMMIT));

                    /*
                     * Present the information
                     */
                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    PRT_CELL_SET("Counting Attributes:");
                    PRT_CELL_SET("");

                    PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
                    PRT_CELL_SET("%s", "External Stat Data Valid Overwrite");
                    PRT_CELL_SET("0x%x", ext_stat_data_valid_ow[core_id]);

                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "External Stat Data Valid");
                    PRT_CELL_SET("0x%x", ext_stat_data_valid[core_id]);

                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "External Stat Data Opcode Overwrite");
                    PRT_CELL_SET("0x%x", ext_stat_data_opcode_ow[core_id]);

                    for (i = 0; i < dnx_data_stif.report.stat_id_max_count_get(unit); i++)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s%d", "External Stat Data Opcode type id for stat id ", i);
                        PRT_CELL_SET("0x%x", ext_stat_data_type_id[core_id][i]);
                    }

                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "External Stat Data Overwrite");
                    PRT_CELL_SET("0x%x", ext_stat_data_ow[core_id]);

                    for (i = 0; i < dnx_data_stif.report.stat_id_max_count_get(unit); i++)
                    {
                        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                        PRT_CELL_SET("%s%d", "External Stat Data ID ", i);
                        PRT_CELL_SET("0x%x", ext_stat_data[core_id][i]);
                    }

                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "Internal Stat Object Valid Overwrite");
                    PRT_CELL_SET("0x%x", int_stat_obj_valid_ow[core_id]);

                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "Internal Stat Object Valid");
                    PRT_CELL_SET("0x%x", int_stat_obj_valid[core_id]);

                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "Internal Stat Object ID Overwrite");
                    PRT_CELL_SET("0x%x", int_stat_obj_id_ow[core_id]);

                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "Internal Stat Object ID");
                    PRT_CELL_SET("0x%x", int_stat_obj_id[core_id]);

                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "Internal Stat Object Type Overwrite");
                    PRT_CELL_SET("0x%x", int_stat_obj_type_ow[core_id]);

                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "Internal Stat Object Type");
                    PRT_CELL_SET("0x%x", int_stat_obj_type[core_id]);

                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "Internal Stat Object Meta Data Overwrite");
                    PRT_CELL_SET("0x%x", int_stat_obj_meta_data_ow[core_id]);

                    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                    PRT_CELL_SET("%s", "Internal Stat Object Meta Data");
                    PRT_CELL_SET("0x%x", int_stat_obj_meta_data[core_id]);

                    PRT_COMMITX;
                }
            }
        }
    }

exit:
    PRT_FREE;
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
