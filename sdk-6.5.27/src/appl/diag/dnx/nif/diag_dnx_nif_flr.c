/** \file diag_dnx_nif_flr.c
 *
 * diagnostics for NIF FLR info
 *
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/*
 * Include files.
 * {
 */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/diag.h>
#include <sal/core/boot.h>

#include <shared/util.h>
#include <soc/sand/shrextend/shrextend_debug.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>

#include <sal/appl/sal.h>

#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/port/port.h>
#include <bcm_int/dnx/algo/lane_map/algo_lane_map.h>
/*
 * }
 */

/*
 * Macros
 * {
 */
#define  DNX_DIAG_NIF_STATUS_MAX_STRING_LEN    40
 /*
  * }
  */

/**
 * \brief - Get FLR timeout ms from timeout enum
 *
 */
static shr_error_e
sh_dnx_flr_timeout_ms_get(
    int unit,
    bcm_port_flr_timeout_t timeout,
    int *timeout_ms)
{
    SHR_FUNC_INIT_VARS(unit);
    switch (timeout)
    {
        case bcmPortFlrTimeout50ms:
        {
            *timeout_ms = 50;
            break;
        }
        case bcmPortFlrTimeout75ms:
        {
            *timeout_ms = 75;
            break;
        }
        case bcmPortFlrTimeout100ms:
        {
            *timeout_ms = 100;
            break;
        }
        case bcmPortFlrTimeout150ms:
        {
            *timeout_ms = 150;
            break;
        }
        case bcmPortFlrTimeout250ms:
        {
            *timeout_ms = 250;
            break;
        }
        case bcmPortFlrTimeout500ms:
        {
            *timeout_ms = 500;
            break;
        }
        default:
        {
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Unsupported timeout enum! \n");
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Dump the NIF FLR info
 *
 * \param [in] unit - chip unit id
 * \param [in] args - diag command arguments
 * \param [in] sand_control - diag comman control
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
shr_error_e
sh_dnx_nif_flr_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_port_t logical_port;
    bcm_pbmp_t logical_ports;
    dnx_algo_port_info_s port_info;
    char port_info_str[DNX_ALGO_PORT_TYPE_STR_LENGTH];
    char timeout_str[DNX_DIAG_NIF_STATUS_MAX_STRING_LEN];
    char lanes_str[DNX_DIAG_NIF_STATUS_MAX_STRING_LEN];
    char lane_id_str[DNX_DIAG_NIF_STATUS_MAX_STRING_LEN];
    int is_flr_fw_support, flr_enable, actual_size;
    int timeout_ms, lane_index;
    bcm_port_flr_timeout_t timeout;
    bcm_port_flr_link_config_t link_config;
    int lane_order[DNX_DATA_MAX_NIF_OTN_NOF_LANES];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("port", logical_ports);
    /*
     * Print table header
     */
    PRT_TITLE_SET("FLR Info");

    PRT_COLUMN_ADD("Port");
    PRT_COLUMN_ADD("Port Type");
    PRT_COLUMN_ADD("FLR Enable");
    PRT_COLUMN_ADD("FLR Timeout");

    if (dnx_data_nif.flr.feature_get(unit, dnx_data_nif_flr_is_advanced_flr_supported))
    {
        PRT_COLUMN_ADD("CDR Unlock");
        PRT_COLUMN_ADD("CDR OOL");
        PRT_COLUMN_ADD("LOS/SF");
        PRT_COLUMN_ADD("Link Stable Timer");
        PRT_COLUMN_ADD("FLR logical lane");
    }

    BCM_PBMP_ITER(logical_ports, logical_port)
    {
        /*
         * Get the port speed and type
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, logical_port, &port_info));
        if (!DNX_ALGO_PORT_TYPE_IS_NIF_ETH(unit, port_info, DNX_ALGO_PORT_TYPE_INCLUDE(STIF, L1, MGMT)) &&
            !DNX_ALGO_PORT_TYPE_IS_FLEXE_PHY(unit, port_info, TRUE) && !DNX_ALGO_PORT_TYPE_IS_OTN_PHY(unit, port_info))
        {
            continue;
        }
        /*
         * Check if FLR firmware is enabled
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_nif_flr_fw_support_get(unit, logical_port, &is_flr_fw_support));
        if (!is_flr_fw_support)
        {
            continue;
        }
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        /*
         * Get Port type str
         */
        SHR_IF_ERR_EXIT(dnx_algo_port_type_str_get(unit, port_info.port_type, port_info_str));
        /*
         * Get FLR enable and timeout
         */
        SHR_IF_ERR_EXIT(bcm_port_fast_link_recovery_enable_get(unit, logical_port, &flr_enable, &timeout));
        if (flr_enable)
        {
            SHR_IF_ERR_EXIT(sh_dnx_flr_timeout_ms_get(unit, timeout, &timeout_ms));
            sal_snprintf(timeout_str, sizeof(timeout_str), "%dms", timeout_ms);
        }
        else
        {
            sal_snprintf(timeout_str, sizeof(timeout_str), "-");
        }
        /*
         * Get specific info for advanced FLR
         */
        if (dnx_data_nif.flr.feature_get(unit, dnx_data_nif_flr_is_advanced_flr_supported))
        {
            /*
             * Get link config
             */
            SHR_IF_ERR_EXIT(bcm_port_fast_link_recovery_link_config_get(unit, logical_port, 0, &link_config));
            /*
             * Get FLR logical lanes
             */
            SHR_IF_ERR_EXIT(imb_port_lane_map_get
                            (unit, logical_port, 0, DNX_DATA_MAX_NIF_OTN_NOF_LANES, lane_order, &actual_size));

            sal_memset(lanes_str, 0, sizeof(lanes_str));
            for (lane_index = 0; lane_index < actual_size; ++lane_index)
            {
                sal_memset(lane_id_str, 0, sizeof(lane_id_str));
                sal_snprintf(lane_id_str, sizeof(lane_id_str), "%d", lane_order[lane_index]);
                utilex_str_append(lanes_str, lane_id_str);
            }
            utilex_str_escape(lanes_str, ',');
        }

        PRT_CELL_SET("%d", logical_port);
        PRT_CELL_SET("%s", port_info_str);
        PRT_CELL_SET("%d", flr_enable);
        PRT_CELL_SET("%s", timeout_str);
        if (dnx_data_nif.flr.feature_get(unit, dnx_data_nif_flr_is_advanced_flr_supported))
        {
            PRT_CELL_SET("%s", (link_config.link_mask & BCM_PORT_FLR_CDR_LOCK_DOWN_MASK) ? "Masked" : "Unmasked");
            PRT_CELL_SET("%s", (link_config.link_mask & BCM_PORT_FLR_CDR_OOL_DOWN_MASK) ? "Masked" : "Unmasked");
            PRT_CELL_SET("%s", (link_config.link_mask & BCM_PORT_FLR_LOS_DOWN_MASK) ? "Masked" : "Unmasked");
            PRT_CELL_SET("%dus", link_config.link_stable_timer);
            PRT_CELL_SET("%s", lanes_str);
        }
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief DNX NIF FLR info
 * List of the supported commands, pointer to command function and command usage function.
 * This is the entry point for NIF lane map diagnostic commands
 */

/* *INDENT-OFF* */

sh_sand_man_t sh_dnx_nif_flr_man = {
    .brief    = "Diagnostic for printing the NIF FLR info.",
    .full     = "Dump the NIF FLR info for all the Eth, flexe PHY and OTN ports "
                "when FLR is enabled ",
    .synopsis = "[port=<integer|port_name>]",
    .examples = "\n"
                "port=1\n"
                "port=eth"
};

sh_sand_option_t sh_dnx_nif_flr_options[] = {
    {"port",      SAL_FIELD_TYPE_PORT, "port # / logical port type / port name",    "nif",      NULL},
    {NULL}
};

/* *INDENT-ON* */
/*
 * }
 */
#undef _ERR_MSG_MODULE_NAME
