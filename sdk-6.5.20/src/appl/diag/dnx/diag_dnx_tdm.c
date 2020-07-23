/** \file diag_dnx_tdm.c
 * 
 * main file for tdm diagnostics
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#define BSL_LOG_MODULE BSL_LS_APPLDNX_DIAGDNX

/*
 * INCLUDE FILES:
 * {
 */
/** appl */
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>

/** bcm/bcm_int */
#include <bcm/tdm.h>
#include <bcm_int/dnx/tdm/tdm.h>
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/algo/swstate/auto_generated/access/algo_tdm_access.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_utils.h>

/** soc */
#include <soc/dnx/dnx_data/auto_generated/dnx_data_tdm.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <soc/dnx/dbal/auto_generated/dbal_defines_fields.h>

/*
 * }
 */

shr_error_e
sh_dnx_is_tdm_available(
    int unit,
    rhlist_t * test_list)
{
    SHR_FUNC_INIT_VARS(unit);

    /**
     * if init is not done, return that tdm is available.
     * this is done to prevent access to DNX-Data before it is init
     */
    if (!dnx_init_is_init_done_get(unit))
    {
        SHR_EXIT();
    }

    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported) != TRUE)
    {
        SHR_SET_CURRENT_ERR(_SHR_E_UNIT);
    }

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
sh_dnx_tdm_global_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int tdm_mode, system_headers_mode, out_header_type;
    int min_packet_size, max_packet_size, mc_prefix;
    char *mode_str = "N/A", *ftmh_header = "N/A";

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("Device TDM Configuration");

    PRT_COLUMN_ADD("Parameter");
    PRT_COLUMN_ADD("Value");

    tdm_mode = dnx_data_tdm.params.mode_get(unit);
    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    switch (tdm_mode)
    {
        case TDM_MODE_NONE:
            mode_str = "NONE";
            break;
        case TDM_MODE_OPTIMIZED:
            mode_str = "BYPASS";
            ftmh_header = "Optimized";
            break;
        case TDM_MODE_STANDARD:
            mode_str = "BYPASS";
            ftmh_header = (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE) ?
                "Standard" : "Standard J1";
            break;
        case TDM_MODE_PACKET:
            mode_str = "PACKET";
            ftmh_header = (system_headers_mode == DBAL_ENUM_FVAL_SYSTEM_HEADERS_MODE_JERICHO2_MODE) ?
                "Standard" : "Standard J1";
            break;
        default:
            SHR_ERR_EXIT(_SHR_E_INTERNAL, "Unsupported TDM mode:%d\n", tdm_mode);
    }

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("Mode");
    PRT_CELL_SET("%s", mode_str);

    if (tdm_mode != TDM_MODE_NONE)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("FTMH Type");
        PRT_CELL_SET("%s", ftmh_header);
        /*
         * Out header type is verified in bcm_tdm_control_get
         */
        SHR_IF_ERR_EXIT(bcm_tdm_control_get(unit, bcmTdmOutHeaderType, &out_header_type));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Out Header Type");
        PRT_CELL_SET("%s", out_header_type == BCM_TDM_CONTROL_OUT_HEADER_OTMH ? "OTMH" : "FTMH");

        SHR_IF_ERR_EXIT(bcm_tdm_control_get(unit, bcmTdmBypassMinPacketSize, &min_packet_size));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Min Packet Size");
        PRT_CELL_SET("%d", min_packet_size);

        SHR_IF_ERR_EXIT(bcm_tdm_control_get(unit, bcmTdmBypassMaxPacketSize, &max_packet_size));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("Max Packet Size");
        PRT_CELL_SET("%d", max_packet_size);

        SHR_IF_ERR_EXIT(bcm_tdm_control_get(unit, bcmTdmStreamMulticastPrefix, &mc_prefix));
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("MC Prefix");
        PRT_CELL_SET("0x%x", mc_prefix);
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static sh_sand_man_t sh_dnx_tdm_global_man = {
    .brief = "Present device TDM Configuration",
};

static shr_error_e
sh_dnx_tdm_context_cmd(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int id_start, id_end;
    int ingress_context_id;
    int index;
    int modid;
    bcm_port_t port;
    uint8 is_allocated = FALSE;
    uint8 id_is_present = FALSE;
    bcm_tdm_ingress_context_t ingress_context;
    bcm_pbmp_t fabric_ports;
    char fabric_ports_str[_SHR_PBMP_FMT_LEN] = { 0 };
    char temp_str_buff[10] = { 0 };
    char user_data_str[30] = { 0 };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /**
     * Check if TDM is supported
     */
    if ((dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported) != TRUE) ||
        (dnx_data_tdm.params.mode_get(unit) == TDM_MODE_NONE))
    {
        /**
         * Give message and exit
         */
        SHR_EXIT_WITH_LOG(_SHR_E_UNIT, "TDM functionality not supported or disabled!%s%s%s\n", EMPTY, EMPTY, EMPTY);
    }

    /**
     * Check if user has specified context ID
     */
    SH_SAND_IS_PRESENT("id", id_is_present);
    if (id_is_present == FALSE)
    {
        /**
         * Iterrate over all IDs
         */
        id_start = 0;
        id_end = dnx_data_tdm.params.nof_contexts_get(unit) - 1;
    }
    else
    {
        /**
         * Get user input
         */
        SH_SAND_GET_INT32_RANGE("id", id_start, id_end);
    }

    for (ingress_context_id = id_start; ingress_context_id <= id_end; ingress_context_id++)
    {
        /**
         * Check if context with current ID has been allocated
         */
        SHR_IF_ERR_EXIT(algo_tdm_db.ingress_context.is_allocated(unit, ingress_context_id, &is_allocated));
        if (is_allocated == TRUE)
        {
            /**
             * Get the current context
             */
            sal_memset(&ingress_context, 0, sizeof(bcm_tdm_ingress_context_t));
            SHR_IF_ERR_EXIT(bcm_tdm_ingress_context_get(unit, 0, ingress_context_id, &ingress_context));

            /**
             * Print table header
             */
            PRT_TITLE_SET("TDM Ingress Context ID:%d", ingress_context_id);
            PRT_COLUMN_ADD("Parameter");
            PRT_COLUMN_ADD("Value");
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("Type");
            if (ingress_context.type == bcmTdmIngressEditingNoChange)
            {
                PRT_CELL_SET("No Change");
            }
            else
            {
                PRT_CELL_SET("Prepend");
            }

            /**
             * If destination port exists, display it.
             * API returns a Modport Gport, so display the
             * destination in the following format modid: port
             */
            if (ingress_context.destination_port != 0)
            {
                port = BCM_GPORT_MODPORT_PORT_GET(ingress_context.destination_port);
                modid = BCM_GPORT_MODPORT_MODID_GET(ingress_context.destination_port);
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("Destination ModPort");
                PRT_CELL_SET("%d: %d", modid, port);
            }
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("Is Multicast");
            if (ingress_context.is_mc == TRUE)
            {
                /**
                 * In case of multicast display relevant fields:
                 * Multicast ID or Stamp MCID with SID
                 */
                PRT_CELL_SET("TRUE");
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("Multicast ID");
                PRT_CELL_SET("%d", ingress_context.multicast_id);
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("Stamp MCID with SID");
                PRT_CELL_SET("%d", ingress_context.stamp_mcid_with_sid);
            }
            else
            {
                PRT_CELL_SET("FALSE");
            }
            if (ingress_context.fabric_ports_count != 0)
            {
                /**
                 * In case of fabric ports used for direct routing
                 * display them
                 */
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("Fabric Ports Count");
                PRT_CELL_SET("%d", ingress_context.fabric_ports_count);
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("Fabric Ports List");
                BCM_PBMP_CLEAR(fabric_ports);
                for (index = 0; index < ingress_context.fabric_ports_count; index++)
                {
                    BCM_PBMP_PORT_ADD(fabric_ports, ingress_context.fabric_ports[index]);
                }
                _SHR_PBMP_FMT(fabric_ports, fabric_ports_str);
                PRT_CELL_SET("%s", fabric_ports_str);
            }
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
            PRT_CELL_SET("CPU traffic is TDM");
            if (ingress_context.is_not_tdm == TRUE)
            {
                PRT_CELL_SET("FALSE");
            }
            else
            {
                PRT_CELL_SET("TRUE");
            }
            if (ingress_context.user_data_count != 0)
            {
                /**
                 * In case user data to be set in FTMH header exists,
                 * display it. User data is not relevant for optimized FTMH
                 */
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("User Data Bits Count");
                PRT_CELL_SET("%d", ingress_context.user_data_count);
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("User Data");
                for (index = 0; index < BCM_TDM_USER_DATA_MAX_SIZE; index++)
                {
                    sal_snprintf(temp_str_buff, sizeof(temp_str_buff), "0x%02x", ingress_context.user_data[index]);
                    sal_strncat_s(user_data_str, temp_str_buff, sizeof(temp_str_buff));
                }
                PRT_CELL_SET("%s", user_data_str);
            }
            PRT_COMMITX;
        }
    }

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

#ifdef INCLUDE_CTEST
/** Used in TDM context diagnostic test callbacks to store soc properties to be restored */
static rhhandle_t diag_dnx_tdm_context_soc_h[SOC_MAX_NUM_DEVICES] = { NULL };

static shr_error_e
cmd_dnx_tdm_context_init_cb(
    int unit)
{
    int context_id;
    bcm_port_t logical_port;
    bcm_pbmp_t logical_ports;
    bcm_tdm_ingress_context_t ingress_context;
    ctest_soc_property_t ctest_soc_property[] = {
        {"tdm_mode.0", "TDM_OPTIMIZED"},
        {NULL}
    };

    SHR_FUNC_INIT_VARS(unit);

    /**
     * Check if TDM is supported. If it is not, exit.
     * Error for "no support" will be returned in the command main function
     */
    if (dnx_data_tdm.params.feature_get(unit, dnx_data_tdm_params_is_supported) != TRUE)
    {
        /**
         * Exit with no error
         */
        SHR_EXIT();
    }

    /**
     * Set SOC properties
     */
    SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &diag_dnx_tdm_context_soc_h[unit]));

    /**
     * Get logical TM ports in order to use one of them for
     * creating an ingress context
     */
    SHR_IF_ERR_EXIT(dnx_algo_port_logicals_get
                    (unit, _SHR_CORE_ALL, DNX_ALGO_PORT_LOGICALS_TYPE_NIF_ETH, 0, &logical_ports));
    _SHR_PBMP_FIRST(logical_ports, logical_port);

    /** Create first dummy Ingress Context - Prepend UC with destination port */
    bcm_tdm_ingress_editing_context_t_init(&ingress_context);
    BCM_GPORT_MODPORT_SET(ingress_context.destination_port, unit, logical_port);
    ingress_context.is_mc = FALSE;
    ingress_context.type = bcmTdmIngressEditingPrepend;
    ingress_context.stamp_mcid_with_sid = 0;
    SHR_IF_ERR_EXIT(bcm_tdm_ingress_context_create(unit, 0, &context_id, &ingress_context));

    /** Create first dummy Ingress Context - No change MC with ID 5 */
    bcm_tdm_ingress_editing_context_t_init(&ingress_context);
    BCM_GPORT_MODPORT_SET(ingress_context.destination_port, unit, logical_port);
    ingress_context.is_mc = TRUE;
    ingress_context.type = bcmTdmIngressEditingNoChange;
    ingress_context.stamp_mcid_with_sid = 0;
    ingress_context.multicast_id = 5;
    SHR_IF_ERR_EXIT(bcm_tdm_ingress_context_create(unit, 0, &context_id, &ingress_context));

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_dnx_tdm_context_deinit_cb(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    ctest_dnxc_restore_soc_properties(unit, diag_dnx_tdm_context_soc_h[unit]);

    SHR_FUNC_EXIT;
}
#endif

static sh_sand_man_t sh_dnx_tdm_context_man = {
    .brief = "Present TDM Ingress Context content",
    .examples = "id=0-10"
#ifdef INCLUDE_CTEST
        ,
    .init_cb = cmd_dnx_tdm_context_init_cb,
    .deinit_cb = cmd_dnx_tdm_context_deinit_cb
#endif
};

sh_sand_enum_t tdm_context_id_table[] = {
    { /** No need to define value if plugin_str was defined */
     .string = "max",
     .desc = "Number of Context IDs supported",
     .value = 0,
     .plugin_str = "DNX_DATA.tdm.params.nof_contexts-1"}
    ,
    {NULL}
};

/**
 * \brief - Options list for 'visibility mode' shell command
 */
static sh_sand_option_t sh_dnx_tdm_context_arguments[] = {
    /*
     * Name | Type | Description | Default 
     */
    {"id", SAL_FIELD_TYPE_INT32, "Context ID", "0-max", tdm_context_id_table, "0-max"},
    {NULL}      /* End of options list - must be last. */
};

/**
 * \brief DNX TDM diagnostics
 * List of the supported commands, pointer to command function and command usage function. 
 * This is the entry point for TDM diagnostic commands
 */
sh_sand_cmd_t sh_dnx_tdm_cmds[] = {
    /*
     * keyword, action, Array, arguments, man 
     */
    {"global", sh_dnx_tdm_global_cmd, NULL, NULL, &sh_dnx_tdm_global_man},
    {"context", sh_dnx_tdm_context_cmd, NULL, sh_dnx_tdm_context_arguments, &sh_dnx_tdm_context_man},
    {NULL}
};

sh_sand_man_t sh_dnx_tdm_man = {
    .brief = "TDM commands"
};

/* *INDENT-ON* */
