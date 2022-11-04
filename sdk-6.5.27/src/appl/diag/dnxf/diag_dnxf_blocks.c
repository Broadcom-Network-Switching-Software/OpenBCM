/** \file diag_dnxf_blocks.c
 *
 * Soecial diagnostic and printout functions for DNXF devices
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPLDNX_FABRIC

/*
 * INCLUDE FILES:
 * {
 */
 /*
  * shared
  */
#include <shared/bsl.h>
#include <shared/pbmp.h>
#include <shared/utilex/utilex_integer_arithmetic.h>
/*appl*/
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/dnxf/diag_dnxf_fabric.h>
#include <appl/diag/dnxc/diag_dnxc_fabric.h>
#include <appl/diag/cmdlist.h>

/*bcm*/
#include <bcm/fabric.h>
#include <bcm/types.h>
/*soc*/
#include <soc/drv.h>
#include <soc/dnxf/cmn/dnxf_diag.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
/* Put your new common defines in this file*/
#include <soc/dnxc/dnxc_defs.h>
#include <soc/dnxc/fabric.h>
/*sal*/
#include <sal/appl/sal.h>
/*
 * }
 */

#define DIAG_DNXF_BLOCK_OFFSET_STR_LEN        20
#define DIAG_DNXF_BLOCK_INDEX_STR_LEN         20

#define DIAG_DNXF_BLOCKS_MAP_OPTION_LINK            "link"
#define DIAG_DNXF_BLOCKS_MAP_OPTION_BLOCK_TYPE      "type"
#define DIAG_DNXF_BLOCKS_MAP_OPTION_BLOCK_INDEX     "index"
#define DIAG_DNXF_BLOCKS_MAP_OPTION_BLOCK_OFFSET    "offset"

typedef enum diag_dnxf_block_mapping_result_filter_e
{
    DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_INVALID = -1,
    DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_FSRD = 0,
    DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_FMAC,
    DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_DCH,
    DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_RTP,
    DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_NOF
} diag_dnxf_block_mapping_result_filter_t;

typedef struct diag_dnxf_link_mapping_info_s
{
    int instance;
    int instance_link;
} diag_dnxf_link_mapping_info_t;

typedef struct diag_dnxf_blocks_mapping_s
{
    diag_dnxf_link_mapping_info_t info[DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_NOF];
} diag_dnxf_blocks_mapping_t;

/** Link the values in the two enums */
static sh_sand_enum_t dnxf_fabric_blocks_enum_table[] = {
    {"FSRD", DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_FSRD, "FSRD block"},
    {"FMAC", DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_FMAC, "FMAC block"},
    {"DCH", DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_DCH, "DCH block"},
    {"RTP", DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_RTP, "RTP block"},
    {"ALL", DIAG_DNXC_OPTION_ALL, "ALL blocks"},
    {NULL}
};

static shr_error_e
diag_dnxf_block_mapping_get_all(
    int unit,
    bcm_port_t link,
    diag_dnxf_blocks_mapping_t * mapping_info)
{
    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(soc_dnxf_drv_link_to_block_mapping(unit, link,
                                                       &(mapping_info->
                                                         info[DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_FSRD].instance),
                                                       &(mapping_info->
                                                         info
                                                         [DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_FSRD].instance_link),
                                                       SOC_DNXF_DRV_BLOCK_TYPE_FSRD));

    SHR_IF_ERR_EXIT(soc_dnxf_drv_link_to_block_mapping(unit, link,
                                                       &(mapping_info->
                                                         info[DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_FMAC].instance),
                                                       &(mapping_info->
                                                         info
                                                         [DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_FMAC].instance_link),
                                                       SOC_DNXF_DRV_BLOCK_TYPE_FMAC));

    SHR_IF_ERR_EXIT(soc_dnxf_drv_link_to_block_mapping(unit, link,
                                                       &(mapping_info->
                                                         info[DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_DCH].instance),
                                                       &(mapping_info->
                                                         info[DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_DCH].instance_link),
                                                       SOC_DNXF_DRV_BLOCK_TYPE_DCH));

    SHR_IF_ERR_EXIT(soc_dnxf_drv_link_to_block_mapping(unit, link,
                                                       &(mapping_info->
                                                         info[DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_RTP].instance),
                                                       &(mapping_info->
                                                         info[DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_RTP].instance_link),
                                                       SOC_DNXF_DRV_BLOCK_TYPE_RTP));

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief
 *    Print out all the mapping of a certain block/link
 * \param [in] unit - unit ID
 * \param [in] args - arguments passed to the function according to the diag mechanism
 * \param [in] sand_control - passed according to the diag mechanism
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected. See \ref shr_error_e
 */
static shr_error_e
sh_dnxf_block_map_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    diag_dnxf_block_mapping_result_filter_t block_type = -1;
    uint32 block_offset = -1, block_index = -1;
    uint8 is_link_provided = 0, is_type_provided = 0;
    uint8 is_block_index_provided = 0, is_block_offset_provided = 0;
    bcm_pbmp_t links;
    bcm_port_t link;
    char *type_str;
    char block_offset_str[DIAG_DNXF_BLOCK_OFFSET_STR_LEN + 1];
    char block_index_str[DIAG_DNXF_BLOCK_INDEX_STR_LEN + 1];
    diag_dnxf_blocks_mapping_t mapping_info;
    int should_skip;
    int famc_lane;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get parameters
     */
    SH_SAND_IS_PRESENT(DIAG_DNXF_BLOCKS_MAP_OPTION_LINK, is_link_provided);

    if (!is_link_provided)
    {
        /** if link is explicitly provided, ignore other arguments */
        SH_SAND_IS_PRESENT(DIAG_DNXF_BLOCKS_MAP_OPTION_BLOCK_TYPE, is_type_provided);
        if (is_type_provided)
        {
            SH_SAND_GET_ENUM(DIAG_DNXF_BLOCKS_MAP_OPTION_BLOCK_TYPE, block_type);

            /*
             * index and offset can and may be independent from each other as well as provided together.
             *
             * These are the options once type is provided:
             *  - index=x && offset=y
             *  - index=x
             *  - offset=y
             */
            SH_SAND_IS_PRESENT(DIAG_DNXF_BLOCKS_MAP_OPTION_BLOCK_INDEX, is_block_index_provided);
            if (is_block_index_provided)
            {
                SH_SAND_GET_UINT32(DIAG_DNXF_BLOCKS_MAP_OPTION_BLOCK_INDEX, block_index);
            }

            SH_SAND_IS_PRESENT(DIAG_DNXF_BLOCKS_MAP_OPTION_BLOCK_OFFSET, is_block_offset_provided);
            if (is_block_offset_provided)
            {
                SH_SAND_GET_UINT32(DIAG_DNXF_BLOCKS_MAP_OPTION_BLOCK_OFFSET, block_offset);
            }
        }

        /** create the link bitmap */
        _SHR_PBMP_CLEAR(links);
        _SHR_PBMP_PORTS_RANGE_ADD(links, 0, dnxf_data_port.general.nof_links_get(unit));
    }
    else
    {
        SH_SAND_GET_PORT(DIAG_DNXF_BLOCKS_MAP_OPTION_LINK, links);
    }

    /** print title */
    if (is_block_index_provided)
    {
        sal_snprintf(block_index_str, DIAG_DNXF_BLOCK_OFFSET_STR_LEN, "%d", block_index);
    }
    else
    {
        sal_snprintf(block_index_str, DIAG_DNXF_BLOCK_OFFSET_STR_LEN, "N/A");
    }

    if (is_block_offset_provided)
    {
        sal_snprintf(block_offset_str, DIAG_DNXF_BLOCK_INDEX_STR_LEN, "%d", block_offset);
    }
    else
    {
        sal_snprintf(block_offset_str, DIAG_DNXF_BLOCK_INDEX_STR_LEN, "N/A");
    }

    type_str = sh_sand_enum_str(dnxf_fabric_blocks_enum_table, block_type);
    PRT_TITLE_SET("Block mapping block_type %s, block index %s, block offset %s",
                  type_str, block_index_str, block_offset_str);

    /** print columns */
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Link #");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FSRD id");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FSRD offset");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FMAC id");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "FMAC offset");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "DCH id");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "DCH offset");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "RTP id");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "RTP offset");

    /** print only relevant entries */
    _SHR_PBMP_ITER(links, link)
    {
        should_skip = 0;

        SHR_IF_ERR_EXIT(soc_dnxf_lane_map_db_link_to_fmac_lane_get(unit, link, &famc_lane));

        /** check if link is mapped */
        if (famc_lane == -1)
        {
            should_skip = 1;
        }

        /** check if link and block mapping match user arguments */
        if (should_skip == 0)
        {
            /** get mapping info all all relevant blocks */
            SHR_IF_ERR_EXIT(diag_dnxf_block_mapping_get_all(unit, link, &mapping_info));

            if (is_type_provided && (block_type != DIAG_DNXC_OPTION_ALL))
            {
                /** if the user provided a block type, need to filter all links that do not match the index */
                if ((should_skip == 0) && is_block_index_provided)
                {
                    if (mapping_info.info[block_type].instance != block_index)
                    {
                        should_skip = 1;
                    }
                }

                /** if the user provided a block type, need to filter all links that do not match the offset */
                if ((should_skip == 0) && is_block_offset_provided)
                {
                    if (mapping_info.info[block_type].instance_link != block_offset)
                    {
                        should_skip = 1;
                    }
                }
            }
        }

        /** skip */
        if (should_skip)
        {
            continue;
        }

        /** print link block mapping */
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%d", link);
        PRT_CELL_SET("%d", mapping_info.info[DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_FSRD].instance);
        PRT_CELL_SET("%d", mapping_info.info[DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_FSRD].instance_link);
        PRT_CELL_SET("%d", mapping_info.info[DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_FMAC].instance);
        PRT_CELL_SET("%d", mapping_info.info[DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_FMAC].instance_link);
        PRT_CELL_SET("%d", mapping_info.info[DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_DCH].instance);
        PRT_CELL_SET("%d", mapping_info.info[DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_DCH].instance_link);
        PRT_CELL_SET("%d", mapping_info.info[DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_RTP].instance);
        PRT_CELL_SET("%d", mapping_info.info[DIAG_DNXF_BLOCK_MAPPING_RESULT_FILTER_RTP].instance_link);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
sh_sand_man_t sh_dnxf_blocks_man = {
    .brief = "block commands."
};

sh_sand_man_t sh_dnxf_blocks_map_man = {
    .brief = "Block mapping commands.",
    .full = "Helps map various block types to their corresponding index and offset",
    .synopsis = "[link=<link>][type=<block_type>][index=<block_index>][offset=<block_offset>]",
    .examples = "\n"
                "0-31 \n"
                "link=127 \n"
                "type=FSRD index=1 offset=2 \n"
                "type=FSRD index=1 \n"
                "type=FMAC offset=2"
};


sh_sand_option_t sh_dnxf_block_map_options[] = {
    {DIAG_DNXF_BLOCKS_MAP_OPTION_LINK,            SAL_FIELD_TYPE_PORT,    "Logical link number ",         "all",  NULL, NULL, SH_SAND_ARG_FREE},
    {DIAG_DNXF_BLOCKS_MAP_OPTION_BLOCK_TYPE,      SAL_FIELD_TYPE_ENUM,    "RTP/DCH/FMAC/FSRD/ALL",        "all",  (void *) dnxf_fabric_blocks_enum_table},
    {DIAG_DNXF_BLOCKS_MAP_OPTION_BLOCK_INDEX,     SAL_FIELD_TYPE_UINT32,  "Index of the block to map",    "-1"},
    {DIAG_DNXF_BLOCKS_MAP_OPTION_BLOCK_OFFSET,    SAL_FIELD_TYPE_UINT32,  "Offset in the block to map",   "-1"},
    {NULL}
};

sh_sand_cmd_t sh_dnxf_blocks_cmds[] = {
    {"MAPping",         sh_dnxf_block_map_cmd,        NULL,   sh_dnxf_block_map_options,         &sh_dnxf_blocks_map_man,  NULL, NULL, SH_CMD_NOT_RELEASE * SH_CMD_NO_XML_VERIFY},
    {NULL}
};

sh_sand_option_t sh_dnxf_device_reset_options[] = {
    {"SoFT",          SAL_FIELD_TYPE_BOOL,  "Perform Soft reset",                "No", NULL},
    {"hard",          SAL_FIELD_TYPE_BOOL,  "Perform hard reset",                "No", NULL},
    {"ACCessOnly",    SAL_FIELD_TYPE_BOOL,  "Init in Access-only mode",          "No", NULL, NULL, SH_SAND_ARG_QUIET},
    {"mode",          SAL_FIELD_TYPE_INT32, "Legacy command mode",               "0",  NULL, NULL, SH_SAND_ARG_FREE | SH_SAND_ARG_QUIET},
    {"action",        SAL_FIELD_TYPE_INT32, "Legacy command action",             "0",  NULL, NULL, SH_SAND_ARG_FREE | SH_SAND_ARG_QUIET},
    {"DURation",      SAL_FIELD_TYPE_BOOL,  "Export the duration of the reset",  "No", NULL},
    {NULL}
};

sh_sand_man_t sh_dnxf_device_reset_man = {
    .brief = "Reset device",
    .full = "Reset device according to provided mode (soft, Accessonly, etc..).\n"
        "This command also supports legacy der command calls",
    .examples = "Soft\n" "hard\n" "Soft DURation\n" "ACCessOnly\n",
};


shr_error_e
sh_dnxf_device_reset_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int soft_reset, access_only, hard_reset, duration;
    sal_usecs_t t1 = 0, t2 = 0;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_BOOL("soft", soft_reset);
    SH_SAND_GET_BOOL("hard", hard_reset);
    SH_SAND_GET_BOOL("ACCessOnly", access_only);
    SH_SAND_GET_BOOL("DURation", duration);

    if (duration)
    {
        t1 = sal_time_usecs();
    }

    if (soft_reset)
    {
        SHR_IF_ERR_EXIT(soc_dnxf_device_reset(unit, SOC_DNXC_RESET_MODE_BLOCKS_SOFT_RESET, 0));
    }
    else if (hard_reset)
    {
        SHR_IF_ERR_EXIT(soc_dnxf_device_reset(unit, SOC_DNXC_RESET_MODE_BLOCKS_RESET, 0));
    }
    else if (access_only)
    {
        SHR_IF_ERR_EXIT(soc_dnxf_device_reset(unit, SOC_DNXC_RESET_MODE_REG_ACCESS, 0));
    }
    else if (ARG_CUR(args))
    {
        int rv;

        /** invoke legacy command */
        rv = sh_device_reset(unit, args);
        if (rv != CMD_OK)
        {
            SHR_CLI_EXIT(_SHR_E_FAIL, "Legacy DeviceRest command failed\n");
        }
    }

    if (duration)
    {
        t2 = sal_time_usecs();
        cli_out("The duration of reset is: %d\n", t2 - t1);
    }

exit:
    SHR_FUNC_EXIT;
}


/* *INDENT-ON* */
