/** \file diag_dnx_port_mgmt.c
 * 
 * DNX PORT MGMT DIAG PACK - diagnostic pack for port mgmt module (algo port module)
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_PORT

/*
 * INCLUDE FILES:
 * {
 */
/** appl */
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
/** bcm */
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
/** sal */
#include <sal/appl/sal.h>
/** local */
#include "diag_dnx_port_mgmt.h"
/*
 * }
 */

#define DIAG_DNX_PORT_MGMT_DUMP_FULL_TYPE(str)  \
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);\
        PRT_CELL_SKIP(2);\
        PRT_CELL_SET(str)

#define DIAG_DNX_PORT_MGMT_PHYS_STR_LENGTH      (SOC_MAX_NUM_PORTS + 3)
/*
 * LOCAL FUNCTIONs:
 * {
 */
/**
 * \brief - Dump summary of port mgmt info (algo_port_*)
 *
 * \param [in] unit -  Unit #
 * \param [in] logical_ports - bitmap of required logical ports
 * \param [in] sand_control - pointer to framework control structure
 */
static shr_error_e
sh_dnx_port_mgmt_dump(
    int unit,
    bcm_pbmp_t logical_ports,
    sh_sand_control_t * sand_control)
{
    dnx_algo_port_diag_info_t diag_info;
    bcm_port_t logical_port;
    char port_info_str[DNX_ALGO_PORT_TYPE_STR_LENGTH];
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Print table header
     */
    PRT_TITLE_SET("PORT MGMT");
    PRT_COLUMN_ADD("Port");
    PRT_COLUMN_ADD("Type");
    PRT_COLUMN_ADD("Core");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "First PHY");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "TM Port");
    PRT_COLUMN_ADD("Channel");

    /*
     * Dump table
     */
    BCM_PBMP_ITER(logical_ports, logical_port)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        /*
         * Get data
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_diag_info_get(unit, logical_port, &diag_info));
        SHR_IF_ERR_EXIT(dnx_algo_port_type_str_get(unit, diag_info.port_info.port_type, port_info_str));

        /*
         * Dump table line
         */
        PRT_CELL_SET("%d", diag_info.logical_port);
        PRT_CELL_SET("%s", port_info_str);
        PRT_CELL_SET("%d", diag_info.core_id);
        PRT_CELL_SET("%d", diag_info.phy_id);
        PRT_CELL_SET("%d", diag_info.tm_port);
        PRT_CELL_SET("%d", diag_info.channel_id);
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Dump port mgmt info (algo_port_*)
 *
 * \param [in] unit -  Unit #
 * \param [in] logical_ports - bitmap of required logical ports
 * \param [in] sand_control - pointer to framework control structure
 */
static shr_error_e
sh_dnx_port_mgmt_full_dump(
    int unit,
    bcm_pbmp_t logical_ports,
    sh_sand_control_t * sand_control)
{
    dnx_algo_port_diag_info_all_t diag_info;
    bcm_port_t logical_port;
    char port_info_str[DNX_ALGO_PORT_TYPE_STR_LENGTH];
    char tdm_mode_str[DNX_ALGO_PORT_TDM_MODE_STR_LENGTH];
    char if_tdm_mode_str[DNX_ALGO_PORT_IF_TDM_MODE_STR_LENGTH];
    char phys_buf[DIAG_DNX_PORT_MGMT_PHYS_STR_LENGTH];
    char channels_buf[DIAG_DNX_PORT_MGMT_PHYS_STR_LENGTH];
    char logical_fifo_buf[DIAG_DNX_PORT_MGMT_PHYS_STR_LENGTH];
    int is_lag;
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Print table header
     */
    PRT_TITLE_SET("PORT MGMT FULL");
    PRT_COLUMN_ADD("Port");
    PRT_COLUMN_ADD("Module");
    PRT_COLUMN_ADD("Property");
    PRT_COLUMN_ADD("Value");

    /*
     * Dump table
     */
    BCM_PBMP_ITER(logical_ports, logical_port)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_EQUAL_BEFORE);
        /*
         * Get data
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_diag_info_all_get(unit, logical_port, &diag_info));
        SHR_IF_ERR_EXIT(dnx_algo_port_type_str_get
                        (unit, diag_info.logical_port_info.port_info.port_type, port_info_str));

        /*
         * Dump table line
         */
        /** Logicals */
        PRT_CELL_SET("%d", diag_info.logical_port_info.logical_port);
        PRT_CELL_SET("%s", "LOGICAL");

        PRT_CELL_SET("type");
        PRT_CELL_SET("%s", port_info_str);
        DIAG_DNX_PORT_MGMT_DUMP_FULL_TYPE("if offset");
        PRT_CELL_SET("%d", diag_info.logical_port_info.interface_offset);
        DIAG_DNX_PORT_MGMT_DUMP_FULL_TYPE("master");
        PRT_CELL_SET("%d", diag_info.logical_port_info.master_port);

        /** Interface */
        if (diag_info.interface_info.valid)
        {
            DIAG_DNX_PORT_MGMT_DUMP_FULL_TYPE("core");
            PRT_CELL_SET("%d", diag_info.interface_info.core_id);
            DIAG_DNX_PORT_MGMT_DUMP_FULL_TYPE("interface speed");
            PRT_CELL_SET("%s:%d", "Rx", diag_info.interface_info.interface_rx_speed);
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SKIP(3);
            PRT_CELL_SET("%s:%d", "Tx", diag_info.interface_info.interface_tx_speed);
        }

        /** NIF */
        if (diag_info.nif_info.valid)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            PRT_CELL_SKIP(1);
            PRT_CELL_SET("%s", "NIF");

            PRT_CELL_SET("first phy");
            PRT_CELL_SET("%d", diag_info.nif_info.first_phy);
            shr_pbmp_range_format(diag_info.nif_info.phy_ports, phys_buf, DIAG_DNX_PORT_MGMT_PHYS_STR_LENGTH);
            DIAG_DNX_PORT_MGMT_DUMP_FULL_TYPE("phys");
            PRT_CELL_SET("%s", phys_buf);
            DIAG_DNX_PORT_MGMT_DUMP_FULL_TYPE("logical fifos");
            shr_pbmp_range_format(diag_info.nif_info.logical_fifos, logical_fifo_buf,
                                  DIAG_DNX_PORT_MGMT_PHYS_STR_LENGTH);
            PRT_CELL_SET("%s", logical_fifo_buf);

            /** ILKN info  */
            if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, diag_info.logical_port_info.port_info, TRUE, TRUE))
            {
                DIAG_DNX_PORT_MGMT_DUMP_FULL_TYPE("nof segments");

                PRT_CELL_SET("%d", diag_info.ilkn_info.nof_segments);

                if (DNX_ALGO_PORT_TYPE_IS_ING_TM(unit, diag_info.logical_port_info.port_info))
                {
                    DIAG_DNX_PORT_MGMT_DUMP_FULL_TYPE("interleaved");
                    PRT_CELL_SET("%d", diag_info.ilkn_info.is_interleaved);
                }
            }
        }

        /** TM AND PP */
        if (diag_info.tm_pp_info.valid || diag_info.tm_pp_info.sch_valid)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            PRT_CELL_SKIP(1);
            PRT_CELL_SET("%s", "TM AND PP");
        }

        if (diag_info.tm_pp_info.valid)
        {
            /** General TM port */
            PRT_CELL_SET("tm port");
            PRT_CELL_SET("%d", diag_info.tm_pp_info.tm_port);
            SHR_IF_ERR_EXIT(dnx_algo_port_tdm_mode_str_get(unit, diag_info.tm_pp_info.tdm_mode, tdm_mode_str));
            DIAG_DNX_PORT_MGMT_DUMP_FULL_TYPE("tdm mode");
            PRT_CELL_SET("%s", tdm_mode_str);
            SHR_IF_ERR_EXIT(dnx_algo_port_if_tdm_mode_str_get(unit, diag_info.tm_pp_info.if_tdm_mode, if_tdm_mode_str));
            DIAG_DNX_PORT_MGMT_DUMP_FULL_TYPE("if_tdm mode");
            PRT_CELL_SET("%s", if_tdm_mode_str);

            DIAG_DNX_PORT_MGMT_DUMP_FULL_TYPE("is channelized");
            PRT_CELL_SET("%d", diag_info.tm_pp_info.is_channelized);
            DIAG_DNX_PORT_MGMT_DUMP_FULL_TYPE("channel");
            PRT_CELL_SET("%d", diag_info.tm_pp_info.channel_id);
            shr_pbmp_range_format(diag_info.tm_pp_info.channels_ports, channels_buf,
                                  DIAG_DNX_PORT_MGMT_PHYS_STR_LENGTH);
            DIAG_DNX_PORT_MGMT_DUMP_FULL_TYPE("channels");
            PRT_CELL_SET("%s", channels_buf);
            DIAG_DNX_PORT_MGMT_DUMP_FULL_TYPE("tdm-bypass master");
            PRT_CELL_SET("%d", diag_info.tm_pp_info.tdm_bypass_master);
            DIAG_DNX_PORT_MGMT_DUMP_FULL_TYPE("non tdm-bypass master");
            PRT_CELL_SET("%d", diag_info.tm_pp_info.non_tdm_bypass_master);

            /**  Egress TM ports only */
            if (DNX_ALGO_PORT_TYPE_IS_EGR_TM(unit, diag_info.logical_port_info.port_info))
            {
                DIAG_DNX_PORT_MGMT_DUMP_FULL_TYPE("egress interface");
                PRT_CELL_SET("%d", diag_info.tm_pp_info.egr_if_id);
                DIAG_DNX_PORT_MGMT_DUMP_FULL_TYPE("egress interface interleaved");
                PRT_CELL_SET("%d", diag_info.tm_pp_info.egr_if_interleaved);
                DIAG_DNX_PORT_MGMT_DUMP_FULL_TYPE("base_q_pair");
                PRT_CELL_SET("%d", diag_info.tm_pp_info.base_q_pair);
                DIAG_DNX_PORT_MGMT_DUMP_FULL_TYPE("nof priorities");
                PRT_CELL_SET("%d", diag_info.tm_pp_info.num_priorities);
            }
        }

        /**  Egress SCH ports only */
        if (DNX_ALGO_PORT_TYPE_IS_E2E_SCH(unit, diag_info.logical_port_info.port_info))
        {
            DIAG_DNX_PORT_MGMT_DUMP_FULL_TYPE("scheduler interface");
            PRT_CELL_SET("%d", diag_info.tm_pp_info.sch_if_id);
            DIAG_DNX_PORT_MGMT_DUMP_FULL_TYPE("base_hr");
            PRT_CELL_SET("%d", diag_info.tm_pp_info.base_hr);
            DIAG_DNX_PORT_MGMT_DUMP_FULL_TYPE("sch priorities");
            PRT_CELL_SET("%d", diag_info.tm_pp_info.sch_priorities);
        }

        /**  PP ports only */
        SHR_IF_ERR_EXIT(dnx_algo_port_in_lag(unit, logical_port, &is_lag));
        if (DNX_ALGO_PORT_TYPE_IS_PP(unit, diag_info.logical_port_info.port_info, is_lag))
        {
            DIAG_DNX_PORT_MGMT_DUMP_FULL_TYPE("pp port");
            PRT_CELL_SET("%d", diag_info.tm_pp_info.pp_port);
        }

        /** FABRIC */
        if (diag_info.fabric_info.valid)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE_BEFORE);
            PRT_CELL_SKIP(1);
            PRT_CELL_SET("%s", "FABRIC");

            PRT_CELL_SET("link");
            PRT_CELL_SET("%d", diag_info.fabric_info.link_id);
        }
    }

    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - parse args and call to diag_dnx_port_mgmt_dump() or diag_dnx_port_mgmt_full_dump with the requested data params
 */
static shr_error_e
sh_dnx_port_mgmt_dump_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int is_full;
    bcm_pbmp_t logical_ports;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("port", logical_ports);
    SH_SAND_GET_BOOL("full", is_full);

    /*
     * Call to the relevant dump function
     */
    if (is_full)
    {
        SHR_IF_ERR_EXIT(sh_dnx_port_mgmt_full_dump(unit, logical_ports, sand_control));
    }
    else
    {
        SHR_IF_ERR_EXIT(sh_dnx_port_mgmt_dump(unit, logical_ports, sand_control));
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * }
 */
/* *INDENT-OFF* */
static sh_sand_option_t sh_dnx_port_mgmt_dump_options[] = {
    /*name        type                  desc                                        default     ext*/
    {"port",      SAL_FIELD_TYPE_PORT, "port # / logical port type / port name",    "all",      NULL},
    {"full",      SAL_FIELD_TYPE_BOOL,  "show full info or summary",                "false",    NULL},
    {NULL}
};

static sh_sand_man_t sh_dnx_port_mgmt_dump_man = {
    .brief    = "Diagnostic pack for port mgmt (algo port module)",
    .full     = "Diagnostic pack for port mgmt (algo port module)",
    .synopsis = "[full] [port=<integer|port_name|'tm'|'nif'|'fabric'|'all'> ]",
    .examples = "\n"
                "port=fabric\n"
                "full\n"
                "full port=xe\n"
                "full port=1"
};

sh_sand_cmd_t sh_dnx_port_mgmt_cmds[] = {
    /*keyword,   action,                    command, options,                            man*/
    {"dump",     sh_dnx_port_mgmt_dump_cmd, NULL,    sh_dnx_port_mgmt_dump_options,      &sh_dnx_port_mgmt_dump_man},
    {NULL}
};

sh_sand_man_t sh_dnx_port_mgmt_man = {
    .brief    = "Diagnostic pack for port mgmt module (algo port module)"
};

/* *INDENT-ON* */
/*
 * }
 */
