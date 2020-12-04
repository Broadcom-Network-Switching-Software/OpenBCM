/**
 * \file diag_dnx_pp_load_balancing.c
 *
 * Diagnostics procedures, for DNX, for 'pp load_balancing' operations.
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_BCMDNX_SWITCHDIAGSDNX
/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
/** bcm */
#include <bcm_int/dnx/switch/switch_load_balancing.h>
/** soc */
#include <soc/sand/sand_signals.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_switch.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_max_switch.h>
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/sand/diag_sand_signals.h>

#include "diag_dnx_pp.h"
#include "diag_dnx_pp_load_balancing.h"
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
 * }
 */
/* *INDENT-OFF* */
/*
 * Global and Static
 * {
 */
/**
 * \brief
 *   Level of detail of display level on 'switch' menus.
 */
typedef enum
{
    FIRST_DISPLAY_LEVEL = 0,
  /**
   * Display most details but drop legends.
   */
    DISPLAY_LEVEL_MEDIUM = FIRST_DISPLAY_LEVEL,
  /**
   * Display maximal detail level. Nothing is missing.
   */
    DISPLAY_LEVEL_HIGH,
    NUM_DIAPLY_LEVELS
} display_level_e;

/**
 * Enum for options display level
 */
static sh_sand_enum_t pp_load_balancing_enum_table_for_display_level[] = {
    {"HIGH", DISPLAY_LEVEL_HIGH, "Detailed level of display"},
    {"MEDIUM", DISPLAY_LEVEL_MEDIUM, "Concise level of display"},
    {NULL}
};

/**
 *  'Help' description for action display (shell commands).
 */
static
sh_sand_man_t pp_load_balancing_display_man = {
    .brief = "Display load balancing info in various levels of details (user selected)",
    .full = "Display load balancing info as specified by the user\r\n"
            "'level' indicates the level of detail.\r\n"
            "If 'level' is 'high' then display the highest detail level\r\n"
            "If 'level' is 'medium' then display all details but no legends\r\n"
            "If 'level' is not specified, display at the highest detail level\r\n",
    .synopsis = "[level=<detail level>]",
    .examples = "level=high",
};

static
sh_sand_option_t pp_load_balancing_display_options[] = {
    {DNX_DIAG_PP_LB_OPTION_DISPLAY_LEVEL,  SAL_FIELD_TYPE_UINT32, "Lowest-highest user defined action ids to get its info",       "HIGH",       (void *)pp_load_balancing_enum_table_for_display_level},
    {NULL}
};

sh_sand_man_t sh_dnx_diag_pp_lb_vis_man = {
    .brief = "Presenting 'pp vis load_balancing' diagnostics for BCM level and for DNX level",
    .init_cb = diag_sand_packet_send_simple,
    .compatibility = DIAG_DNX_PP_BARE_METAL_NOT_AVAILABLE_COMMAND
};
sh_sand_man_t sh_dnx_diag_pp_lb_info_man = {
    .brief = "Presenting 'pp info load_balancing' diagnostics for BCM level and for DNX level",
    .compatibility = DIAG_DNX_PP_BARE_METAL_NOT_AVAILABLE_COMMAND
};

/*
 * }
 */
/*
 * Diagnostics procedures to be used by, say, diag_dnx_switch_* and ctest_dnx_switch_*
 * {
 */
/**
 * \brief
 *  This diagnostics function will display information on reserved labels,
 *  as a PRT table.
 * \param [in] unit               -
 *   Device ID
 * \param [in] detail_level        -
 *   Enum of type display_level_e. Indicates how detailed the display will be.
 *   See documentation of enum.
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * This procedure executes the full PRT cycle (PRT_INIT_VARS,PRT_TITLE_SET, ...,PRT_FREE).
 * \see
 *   * diag_dnx_switch_load_balancing_display_cb()
 */
static shr_error_e
diag_dnx_pp_lb_display_reserved_labels(
    int unit,
    display_level_e detail_level,
    sh_sand_control_t * sand_control)
{
    dnx_switch_lb_mpls_reserved_t mpls_reserved;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    if (detail_level > DISPLAY_LEVEL_MEDIUM)
    {
        /*
         * Print legend for 'reserved_labels' table below.
         */
        PRT_TITLE_SET("Legend - For table of 'reserved labels', below.");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "column name");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "description");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Label indication");
        PRT_CELL_SET("%s",
                     "Bit map of 16 bits. Each bit corresponds to a reserved label. If set "
                     "then this label is included in the search.");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Force layer");
        PRT_CELL_SET("%s",
                     "Bit map of 16 bits. Each bit corresponds to a reserved label. If set then this label "
                     "is included in the search even if terminated.");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Next label indication");
        PRT_CELL_SET("%s",
                     "Bit map of 16 bits. Each bit corresponds to a reserved label. If set then the label "
                     "following this label is included in the search even if terminated.");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Next label valid");
        PRT_CELL_SET("%s", "Currently, this bitmaps should just be equal to 'Next label indication'");
        PRT_COMMITX;
    }

    PRT_TITLE_SET("Setup for handling of MPLS reserved labels - LOAD_BALANCING_RESERVED_LABELS");

    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Label indication");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Force layer");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Next label indication");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Next label valid");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    SHR_IF_ERR_EXIT(dnx_switch_load_balancing_mpls_reserved_label_get(unit, &mpls_reserved));

    PRT_CELL_SET("0x%04X", mpls_reserved.reserved_current_label_indication);
    PRT_CELL_SET("0x%04X", mpls_reserved.reserved_current_label_force_layer);
    PRT_CELL_SET("0x%04X", mpls_reserved.reserved_next_label_indication);
    PRT_CELL_SET("0x%04X", mpls_reserved.reserved_next_label_valid);
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This diagnostics function will display information on MPLS protocol identification,
 *  as a PRT table.
 * \param [in] unit               -
 *   Device ID
 * \param [in] detail_level        -
 *   Enum of type display_level_e. Indicates how detailed the display will be.
 *   See documentation of enum.
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * This procedure executes the full PRT cycle (PRT_INIT_VARS,PRT_TITLE_SET, ...,PRT_FREE).
 * \see
 *   * diag_dnx_switch_load_balancing_display_cb()
 */
static shr_error_e
diag_dnx_pp_lb_display_mpls_identification(
    int unit,
    display_level_e detail_level,
    sh_sand_control_t * sand_control)
{
    dnx_switch_lb_mpls_identification_t mpls_identification;
    uint32 mpls_identifier, num_mpls_identifiers;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    num_mpls_identifiers = sizeof(mpls_identification.mpls_identification) * SAL_UINT8_NOF_BITS;
    if (detail_level > DISPLAY_LEVEL_MEDIUM)
    {
        /*
         * Print legend for 'mpls_identification' table below.
         */
        PRT_TITLE_SET("Legend - For table of 'mpls identification', below.");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "column name");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "description");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Identification bitmap");
        PRT_CELL_SET("%s",
                     "Bit map of 32 bits. Each bit corresponds to identification of MPLS protocol "
                     "within the 5-bits protocol identifiers list.");
        PRT_COMMITX;
    }

    PRT_TITLE_SET("Bitmap of MPLS protocol identifiers - MPLS_LAYER_IDENTIFICATION_BITMAP_CFG");

    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Identification bit");
    for (mpls_identifier = 0; mpls_identifier < num_mpls_identifiers; mpls_identifier++)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "%d", mpls_identifier);
    }
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);

    SHR_IF_ERR_EXIT(dnx_switch_load_balancing_mpls_identification_get(unit, &mpls_identification));
    PRT_CELL_SET("%s", "Value");
    for (mpls_identifier = 0; mpls_identifier < num_mpls_identifiers; mpls_identifier++)
    {
        if (mpls_identification.mpls_identification & SAL_BIT(mpls_identifier))
        {
            PRT_CELL_SET("%d", 1);
        }
        else
        {
            PRT_CELL_SET("%d", 0);
        }
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This diagnostics function will display information on load balancing clients and the corresponding
 *  crc functions, as a PRT table.
 * \param [in] unit               -
 *   Device ID
 * \param [in] detail_level        -
 *   Enum of type display_level_e. Indicates how detailed the display will be.
 *   See documentation of enum.
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * This procedure executes the full PRT cycle (PRT_INIT_VARS,PRT_TITLE_SET, ...,PRT_FREE).
 * \see
 *   * diag_dnx_switch_load_balancing_display_cb()
 */
static shr_error_e
diag_dnx_pp_lb_display_clients_and_crc_functions(
    int unit,
    display_level_e detail_level,
    sh_sand_control_t * sand_control)
{
    int index, ind;
    dnx_switch_lb_tcam_key_t tcam_key;
    dnx_switch_lb_tcam_result_t tcam_result;
    dnx_switch_lb_valid_t tcam_valid;
    uint32 physical_client, max_physical_client;
    bcm_switch_control_t logical_client[DNX_DATA_MAX_SWITCH_LOAD_BALANCING_NOF_LB_CLIENTS];
    char *logical_client_name[DNX_DATA_MAX_SWITCH_LOAD_BALANCING_NOF_LB_CLIENTS];
    char *crc_enum_value[DNX_DATA_MAX_SWITCH_LOAD_BALANCING_NOF_CRC_FUNCTIONS] = {"0x1015d", "0x100d7", "0x10039", "0x10ac5", "0x109e7", "0x10939", "0x12105", "0x1203d"};
    char *logical_client_description;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    {
        /*
         * Prepare arrays for display.
         */
        max_physical_client = dnx_data_switch.load_balancing.nof_lb_clients_get(unit);
        for (physical_client = 0; physical_client < max_physical_client; physical_client++)
        {
            char *client_name;
            SHR_IF_ERR_EXIT(dnx_switch_lb_physical_client_to_logical
                            (unit, physical_client, &(logical_client[physical_client])));
            SHR_IF_ERR_EXIT(dnx_switch_control_t_name(unit, logical_client[physical_client], &client_name));
            logical_client_name[physical_client] = client_name;
        }
    }
    if (detail_level > DISPLAY_LEVEL_MEDIUM)
    {
        /*
         * Print legend for 'Load balancing clients and corresponding crc functions' table below.
         */
        PRT_TITLE_SET("Legend - For table of clients and corresponding crc functions, below.");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "column name");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "description");
        for (physical_client = 0; physical_client < max_physical_client; physical_client++)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            if (logical_client_name[physical_client] == (char *) 0)
            {
                /*
                 * This should never happen: All physical clients should be translatable to logical.
                 */
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "physical_client (%d) could not be translated to a logical string. Probably internal error.\r\n",
                             physical_client);
            }
            PRT_CELL_SET("%s", logical_client_name[physical_client]);
            switch (logical_client[physical_client])
            {
                case bcmSwitchECMPHashConfig:
                {
                    logical_client_description = "CRC function for first ECMP hierarchy";
                    break;
                }
                case bcmSwitchECMPSecondHierHashConfig:
                {
                    logical_client_description = "CRC function for second ECMP hierarchy";
                    break;
                }
                case bcmSwitchECMPThirdHierHashConfig:
                {
                    logical_client_description = "CRC function for Third ECMP hierarchy";
                    break;
                }
                case bcmSwitchTrunkHashConfig:
                {
                    logical_client_description = "CRC function for LAG";
                    break;
                }
                case bcmSwitchNwkHashConfig:
                {
                    logical_client_description = "CRC function for network load balancing";
                    break;
                }
                default:
                {
                    /*
                     * This should never happen: All logical clients should be handled above.
                     */
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "logical_client (%d) is is not one that is supported for load balancing. Probably internal error.\r\n",
                                 logical_client[physical_client]);
                    break;
                }
            }
            PRT_CELL_SET("%s", logical_client_description);
        }
        PRT_COMMITX;
    }
    PRT_TITLE_SET("Load balancing clients and corresponding crc functions.");
    PRT_INFO_ADD("%s", "  plus selection of LS or MS 16 bits out of 32 assigned per layer.");
    PRT_INFO_SET_MODE(PRT_ROW_SEP_NONE);
    PRT_INFO_ADD("%s", "Handling Tables LOAD_BALANCING_CRC_SELECTION_HL_TCAM and");
    PRT_INFO_ADD("%s", "LOAD_BALANCING_CLIET_FWD_BIT_SEL.");
    PRT_INFO_SET_MODE(PRT_ROW_SEP_NONE);
    PRT_INFO_ADD("%s", "The last row displays an 8-bits bitmap. It indicates whether specified");
    PRT_INFO_ADD("%s", "client would use the LS 16 bits (0) or the MS 16 bits of each of the");
    PRT_INFO_SET_MODE(PRT_ROW_SEP_NONE);
    PRT_INFO_ADD("%s", "8 record layers. There are 32 bits per record layer ");
    PRT_INFO_SET_MODE(PRT_ROW_SEP_NONE);
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Logical client name");
    for (physical_client = 0; physical_client < max_physical_client; physical_client++)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "%s", logical_client_name[physical_client]);
    }
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "Physical client id");
    for (physical_client = 0; physical_client < max_physical_client; physical_client++)
    {
        PRT_CELL_SET("%d", physical_client);
    }
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "crc function id");
    index = 0;
    SHR_IF_ERR_EXIT(dnx_switch_load_balancing_tcam_info_get(unit, index, &tcam_key, &tcam_result, &tcam_valid));
    for (physical_client = 0; physical_client < max_physical_client; physical_client++)
    {
        ind = tcam_result.client_x_crc_16_sel[physical_client];
        PRT_CELL_SET("%s", crc_enum_value[ind]);
    }
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "Select LS/MS 16-bits");
    {
        dnx_switch_lb_lsms_crc_select_t lsms_crc_select;

        for (physical_client = 0; physical_client < max_physical_client; physical_client++)
        {
            lsms_crc_select.lb_client = (uint8) physical_client;
            SHR_IF_ERR_EXIT(dnx_switch_load_balancing_lsms_crc_select_get(unit, &lsms_crc_select));
            PRT_CELL_SET("0x%04X", lsms_crc_select.lb_selection_bit_map);
        }
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This diagnostics function will display information on crc functions (8) and the corresponding
 *  '16-bits seed', that is used for their calculations, as a PRT table.
 * \param [in] unit               -
 *   Device ID
 * \param [in] detail_level        -
 *   Enum of type display_level_e. Indicates how detailed the display will be.
 *   See documentation of enum.
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * This procedure executes the full PRT cycle (PRT_INIT_VARS,PRT_TITLE_SET, ...,PRT_FREE).
 * \see
 *   * diag_dnx_switch_load_balancing_display_cb()
 */
static shr_error_e
diag_dnx_pp_lb_display_crc_seed_per_function(
    int unit,
    display_level_e detail_level,
    sh_sand_control_t * sand_control)
{
    uint16 crc_index, max_crc_index;
    uint16 crc_seeds[DNX_DATA_MAX_SWITCH_LOAD_BALANCING_NOF_CRC_FUNCTIONS];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    max_crc_index = dnx_data_switch.load_balancing.nof_crc_functions_get(unit);
    if (detail_level > DISPLAY_LEVEL_MEDIUM)
    {
        /*
         * Print legend for 'crc_seeds' table below.
         */
        PRT_TITLE_SET("Legend - For table of crc_seed_per_function, below.");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "column name");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "description");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "Identifier of crc function");
        PRT_CELL_SET("%s", "Seed for crc calculation. variable of 16 bits");
        PRT_COMMITX;
    }
    for (crc_index = 0; crc_index < max_crc_index; crc_index++)
    {
        SHR_IF_ERR_EXIT(dnx_switch_load_balancing_crc_seed_get(unit,crc_index, &crc_seeds[crc_index]));
    }
    PRT_TITLE_SET("Load balancing crc functions and corresponding seeds - LOAD_BALANCING_CRC_SEEDS");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "crc function id");
    for (crc_index = 0; crc_index < max_crc_index; crc_index++)
    {
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "%d", crc_index);
    }
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "16-bits seed");
    for (crc_index = 0; crc_index < max_crc_index; crc_index++)
    {
        PRT_CELL_SET("0x%04X", crc_seeds[crc_index]);
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *  This diagnostics function will display information on general crc seeds used for data collection
 *  and compression for MPLS stacks and for parser, as a PRT table.
 * \param [in] unit               -
 *   Device ID
 * \param [in] detail_level        -
 *   Enum of type display_level_e. Indicates how detailed the display will be.
 *   See documentation of enum.
 * \param [in] sand_control -
 *   Control information related to each of the 'options' entered by the caller (and contained in
 *   'args')
 * \return
 *   shr_error_e - Error Type
 * \remark
 *   * This procedure executes the full PRT cycle (PRT_INIT_VARS,PRT_TITLE_SET, ...,PRT_FREE).
 * \see
 *   * diag_dnx_switch_load_balancing_display_cb()
 */
static shr_error_e
diag_dnx_pp_lb_display_crc_general_seeds(
    int unit,
    display_level_e detail_level,
    sh_sand_control_t * sand_control)
{
    dnx_switch_lb_general_seeds_t general_seeds;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    if (detail_level > DISPLAY_LEVEL_MEDIUM)
    {
        /*
         * Print legend for 'general_seeds' table below.
         */
        PRT_TITLE_SET("Legend - For table of general crc seeds for MPLS label collection and for parser, below.");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "column name");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "description");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "MPLS_STACK_0");
        PRT_CELL_SET("%s", "Seed for the CRC32 function on label collection and compression for first MPLS stack");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "MPLS_STACK_1");
        PRT_CELL_SET("%s", "Seed for the CRC32 function on label collection and compression for second MPLS stack");
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", "PARSER");
        PRT_CELL_SET("%s", "Seed for the CRC32 function on data collection and compression for parser");
        PRT_COMMITX;
    }
    SHR_IF_ERR_EXIT(dnx_switch_load_balancing_general_seeds_get(unit, &general_seeds));
    PRT_TITLE_SET("Load balancing general crc seeds - LOAD_BALANCING_GENERAL_SEEDS");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "MPLS_STACK_0");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "MPLS_STACK_1");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "PARSER");
    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("%s", "32-bits seed");
    PRT_CELL_SET("0x%08X", general_seeds.mpls_stack_0);
    PRT_CELL_SET("0x%08X", general_seeds.mpls_stack_1);
    PRT_CELL_SET("0x%08X", general_seeds.parser);
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *   This function displays load-balancing info as per detail level, specified by the caller.
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
diag_dnx_pp_load_balancing_info_display_cb(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 detail_level;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);
    SH_SAND_GET_UINT32(DNX_DIAG_PP_LB_OPTION_DISPLAY_LEVEL, detail_level);
    SHR_IF_ERR_EXIT(diag_dnx_pp_lb_display_reserved_labels(unit, detail_level, sand_control));
    SHR_IF_ERR_EXIT(diag_dnx_pp_lb_display_mpls_identification(unit, detail_level, sand_control));
    SHR_IF_ERR_EXIT(diag_dnx_pp_lb_display_clients_and_crc_functions(unit, detail_level, sand_control));
    SHR_IF_ERR_EXIT(diag_dnx_pp_lb_display_crc_seed_per_function(unit, detail_level, sand_control));
    SHR_IF_ERR_EXIT(diag_dnx_pp_lb_display_crc_general_seeds(unit, detail_level, sand_control));

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

shr_error_e
sh_dnx_pp_lb_vis_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int idx;
    int core_in, core_id;
    int hier_id;
    int nof_bits_in_hier_hash = 16;
    int ecmp_hier_enum[3] =
            { bcmSwitchECMPHashConfig, bcmSwitchECMPSecondHierHashConfig, bcmSwitchECMPThirdHierHashConfig };
    const int max_layer_id = 8;
    const int nof_hierarchies = 3;
    uint32 is_fragmented = FALSE;
    uint32 forwarding_layer;
    uint32 crc_hash_selection, layer_header_type;
    uint32 crc_value;
    dbal_enum_value_field_layer_types_e layer_type;
    char buffer[DBAL_MAX_PRINTABLE_BUFFER_SIZE];
    bcm_switch_control_key_t control_key;
    bcm_switch_control_info_t control_info;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("core", core_in);

    SH_DNX_CORES_ITER(unit, core_in, core_id)
    {
        /** Print message and continue to next core if not ready */
        DNX_PP_BLOCK_IS_READY_CONT(core_id, DNX_PP_BLOCK_IRPP, TRUE);

        SHR_CLI_EXIT_IF_ERR(dpp_dsig_read(unit, core_id, "IRPP",  "VTT5", "", "fwd_layer_index",
                            &forwarding_layer, 1), "no signal record for fwd_layer_index\n");
        /*
         * TABLE with CRC information
         */
        PRT_TITLE_SET("ECMP CRC information for core:%d", core_id);
        PRT_COLUMN_ADD("Layer ID");
        PRT_COLUMN_ADD("Layer Type");
        PRT_COLUMN_ADD("CRC value");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Hierarchy 1 - 16 bit selection");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Hierarchy 2 - 16 bit selection");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Hierarchy 3 - 16 bit selection");
        PRT_COLUMN_ADD("Forwarding");
        PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Participate in hash");

        /** Iterate over all valid layers of the last packet */
        for (idx = 0; idx < max_layer_id; idx++)
        {
            char signal_name[RHNAME_MAX_SIZE];

            sal_snprintf(signal_name, RHNAME_MAX_SIZE - 1, "Layer_Records_CRC32.%d", idx);
            SHR_CLI_EXIT_IF_ERR(dpp_dsig_read(unit, core_id, "IRPP",  "VTT5", "", signal_name,
                                        &crc_value, 1), "Signal:%s is not available\n", signal_name);
            if (crc_value == 0)
                break;

            sal_snprintf(signal_name, RHNAME_MAX_SIZE - 1, "Layer_Protocols.%d", idx);
            SHR_CLI_EXIT_IF_ERR(dpp_dsig_read(unit, core_id, "IRPP",  "VTT5", "", signal_name,
                                        (uint32 *)&layer_type, 1), "Signal:%s is not available\n", signal_name);

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            SHR_IF_ERR_EXIT(dbal_table_field_printable_string_get(unit, DBAL_TABLE_IN_AC_TCAM_DB, DBAL_FIELD_LAYER_TYPE,
                                                                      (uint32 *) &layer_type, NULL, 0, TRUE, buffer));
            PRT_CELL_SET("%d", idx + 1);
            PRT_CELL_SET("%s", buffer);
            PRT_CELL_SET("0x%04x", crc_value);

            /** Fill in cells for selected 16 bits from the CRC per hierarchy */
            for (hier_id = 0; hier_id < nof_hierarchies; hier_id++)
            {
                control_key.type = bcmSwitchLayerRecordModeSelection;
                control_key.index = ecmp_hier_enum[hier_id];
                SHR_IF_ERR_EXIT(bcm_switch_control_indexed_get(unit, control_key, &control_info));
                crc_hash_selection = control_info.value;
                crc_hash_selection = (crc_hash_selection >> idx) & 0x1;
                if (crc_hash_selection == 1)
                {
                    PRT_CELL_SET("upper - 0x%04x", crc_value >> nof_bits_in_hier_hash);
                }
                else
                {
                    PRT_CELL_SET("lower - 0x%04x", crc_value & UTILEX_BITS_MASK(nof_bits_in_hier_hash - 1, 0));
                }
            }

            /** Forwarding layer - true or false */
            if (idx == forwarding_layer)
            {
                PRT_CELL_SET("TRUE");
            }
            else
            {
                PRT_CELL_SET("FALSE");
            }

            /*
             * Participate in hash - true or false
             * All non-terminated headers participate in hashing.
             * MPLS always participates in hash.
             * If the header is fragmented, no layers below the IP are used in hashing.
             */
            SHR_IF_ERR_EXIT(dbal_fields_enum_value_get(unit, DBAL_FIELD_LAYER_TYPES, layer_type, &layer_header_type));
            if ((idx >= forwarding_layer || layer_header_type == DBAL_ENUM_FVAL_LAYER_TYPES_MPLS) && is_fragmented == FALSE)
            {
                PRT_CELL_SET("TRUE");
            }
            else
            {
                PRT_CELL_SET("FALSE");
            }

            /** If current layer is IP, then check if it is fragmented in order to know whether following headers will be used in hashing. */
            if (layer_header_type == DBAL_ENUM_FVAL_LAYER_TYPES_IPV4 || layer_header_type == DBAL_ENUM_FVAL_LAYER_TYPES_IPV6)
            {
                SHR_CLI_EXIT_IF_ERR(dpp_dsig_read(unit, core_id, "IRPP", "VTT5", NULL, "is_fragmented",
                                                    &is_fragmented, 1), "no signal record for is_fragmented\n");
            }
        }
        PRT_COMMITX;
    }
exit:

    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * Globals required at end because they reference static procedures/tables above.
 * {
 */
/**
 * \brief
 *   List of shell options for 'load_balancing' shell commands (display, ...)
 */
sh_sand_cmd_t sh_dnx_diag_pp_lb_info_cmds[] = {
    {"display", diag_dnx_pp_load_balancing_info_display_cb, NULL, pp_load_balancing_display_options, &pp_load_balancing_display_man}
    ,
    {NULL}
};

/*
 * }
 */
