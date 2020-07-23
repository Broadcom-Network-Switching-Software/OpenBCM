/*
 * 
 * \file diag_dnxc_fabric.c
 *
 * diagnostic pack for fabric
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
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
#include <shared/utilex/utilex_integer_arithmetic.h>
/*appl*/
#include <appl/diag/diag.h>
#include <appl/diag/system.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/diag/dnxc/diag_dnxc_fabric.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>

/*bcm*/
#include <bcm/fabric.h>
#ifdef BCM_DNX_SUPPORT
#include <bcm_int/dnx/fabric/fabric.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/port/algo_port_soc.h>
#include <soc/dnx/pll/pll.h>
#include <soc/dnx/drv.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_pll.h>
#include <appl/diag/dnx/diag_dnx_fabric.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#endif
/*soc*/
#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/cmn/dnxf_diag.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <soc/dnxf/ramon/ramon_cgm.h>
/*dnxf data*/
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <appl/diag/dnxf/diag_dnxf_fabric.h>
#endif /* BCM_DNXF_SUPPORT */

/* Put your new common defines in this file*/
#include <soc/dnxc/dnxc_defs.h>
#include <soc/dnxc/fabric.h>
/*sal*/
#include <sal/appl/sal.h>
/*internal*/
#include "diag_dnxc.h"
/*
 * }
 */

 /*
  * LOCAL DEFINEs:
  * {
  */
#define DIAG_DNXC_MAX_NOF_REACHABILITY_CLMNS        24
#define DIAG_DNXC_MAX_NOF_MESH_TOPOLOGY_CLMNS        7

#define DIAG_DNXC_LINK_RANGE_STR_LENGTH             (3*SOC_DNXC_MAX_NOF_FABRIC_LINKS)
#define DIAG_DNXC_LINK_CONFIG_FULL_MANUAL \
                "Displays the most important Fabric configuration information for all links or for specific links. \n" \
                "By default prints the configuration information of all links. \n" \
                "Filters: \n" \
                "\t <pbmp> - Free parameter for displaying the configuration information only for specific fabric ports."

#define DIAG_DNXC_LINK_STATUS_FULL_MANUAL \
                "Displays the fabric link status for all links or for specific links. \n" \
                "By default prints the status of all links. \n" \
                "Filters: \n" \
                "\t <pbmp> - Free parameter for displaying the status only for specific fabric ports."

 /*
  * LOCAL DEFINEs:
  * }
  */

 /*
  * LOCAL TYPEs:
  * }
  */

 /*
  * LOCAL TYPEs:
  * }
  */

#ifdef INCLUDE_CTEST
/** used in diag test  callbacks to store soc properties to be restored */
static rhhandle_t diag_dnxc_fabric_soc_set_h[SOC_MAX_NUM_DEVICES] = { NULL };
#endif

/*
 * LOCAL FUNCTIONs:
 * {
 */

/*
 * LOCAL FUNCTIONs:
 * }
 */

/*
 * LOCAL DIAG PACK:
 * {
 */

/*
 * Reachability
 */

/* *INDENT-OFF* */
static sh_sand_option_t dnxc_fabric_reachability_options[] = {
    {"id",   SAL_FIELD_TYPE_INT32,  "Module ID",    NULL, NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};
/* *INDENT-ON* */

/* *INDENT-OFF* */
static sh_sand_man_t dnxc_fabric_reachability_man = {
    .brief =    "Displays reachable links to modid",
    .full =     "Diagnostic to display all reachable links to a given module",
    .synopsis = "<modid>",
    .examples = "3\n" "2\n" "id=3"
};
/* *INDENT-ON* */

/**
 * \brief - display reachable links to modid
 */
static shr_error_e
cmd_dnxc_fabric_reachability(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int modid;
    uint32 links_array[SOC_DNXC_MAX_NOF_FABRIC_LINKS];
    int array_size;
    int ii;
    int minimum_links = -1;
    int isolate = 0;
    bcm_pbmp_t link_bmp;
    char range_format_buf[DIAG_DNXC_LINK_RANGE_STR_LENGTH];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("id", modid);

    /*
     * Retrieve reachability information for current device
     */
    SHR_IF_ERR_EXIT(bcm_fabric_control_get(unit, bcmFabricIsolate, &isolate));
    SHR_IF_ERR_EXIT(bcm_fabric_reachability_status_get
                    (unit, modid, SOC_DNXC_MAX_NOF_FABRIC_LINKS, links_array, &array_size));

    if (array_size == 0)
    {
        SHR_CLI_EXIT(_SHR_E_NONE, "Module %d isn't reachable \n", modid);
    }

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit) && (dnx_data_fabric.general.connect_mode_get(unit) == DNX_FABRIC_CONNECT_MODE_FE))
    {
        SHR_IF_ERR_EXIT(bcm_fabric_destination_link_min_get(unit, 0, modid, &minimum_links));
    }
#endif

    PRT_TITLE_SET("Reachability table");

    /*
     * Displays reachability status information per module
     */
    if (isolate)
    {
        PRT_INFO_ADD("This device is isolated");
    }

    BCM_PBMP_CLEAR(link_bmp);
    for (ii = 0; ii < array_size; ii++)
    {
        BCM_PBMP_PORT_ADD(link_bmp, links_array[ii]);
    }

    shr_pbmp_range_format(link_bmp, range_format_buf, DIAG_DNXC_LINK_RANGE_STR_LENGTH);

    PRT_INFO_ADD("Found %d links to module %d:%s", array_size, modid, range_format_buf);
    PRT_COLUMN_ADD("#");

    if (minimum_links != -1)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("The minimum number of links to destination is %d", minimum_links);
        if (minimum_links > array_size && minimum_links)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
            PRT_CELL_SET("Current number of links is smaller than minimal number of links");
        }
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

#ifdef INCLUDE_CTEST
static shr_error_e
cmd_dnxc_fabric_init_cb(
    int unit)
{
    ctest_soc_property_t ctest_soc_property[] = {
        {"fabric_connect_mode.0", "FE"},
        {NULL}
    };

    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX(unit))
    {
        SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &diag_dnxc_fabric_soc_set_h[unit]));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_dnxc_fabric_deinit_cb(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX(unit))
    {
        ctest_dnxc_restore_soc_properties(unit, diag_dnxc_fabric_soc_set_h[unit]);
    }
    SHR_FUNC_EXIT;
}
#endif

/*
 * Connectivity
 */

/* *INDENT-OFF* */
static sh_sand_man_t dnxc_fabric_connectivity_man = {
    .brief =    "Display fabric interface connectivity information",
    .full =     "Diagnostic displaying all connectivity information for current module"
#ifdef INCLUDE_CTEST
    ,
    .init_cb = cmd_dnxc_fabric_init_cb,
    .deinit_cb = cmd_dnxc_fabric_deinit_cb
#endif
 };
/* *INDENT-ON* */

/**
 * \brief - display fabric interface connectivity information
 */
static shr_error_e
cmd_dnxc_fabric_connectivity(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_fabric_link_connectivity_t links_connectivity_array[SOC_DNXC_MAX_NOF_FABRIC_LINKS];
    int array_size;
    int link, links_count = 0;
    bcm_port_config_t config;
    bcm_port_t port;
    char *dev_type;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get an array with all ports associated to the passed number of links
     */
    SHR_IF_ERR_EXIT(bcm_fabric_link_connectivity_status_get
                    (unit, SOC_DNXC_MAX_NOF_FABRIC_LINKS, links_connectivity_array, &array_size));

    if (array_size == 0)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "No links were found for this device \n");
    }

    PRT_TITLE_SET("Connectivity status");
    /*
     * Displays device connectivity information
     */
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Link");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Logical Port");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Remote Module");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Remote Link");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Remote Device Type");

    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &config));
    BCM_PBMP_ITER(config.sfi, port)
    {
        /*
         * Get the device type of the connected module on the associated link
         */
        if (BCM_FABRIC_LINK_NO_CONNECTIVITY != links_connectivity_array[links_count].link_id)
        {

            switch (links_connectivity_array[links_count].device_type)
            {
                case bcmFabricDeviceTypeFE1:
                case bcmFabricDeviceTypeFE3:
                case bcmFabricDeviceTypeFE13:
                    dev_type = "FE13";
                    break;
                case bcmFabricDeviceTypeFE2:
                    dev_type = "FE2";
                    break;
                case bcmFabricDeviceTypeFAP:
                case bcmFabricDeviceTypeFIP:
                case bcmFabricDeviceTypeFOP:
                    dev_type = "FAP";
                    break;
                case bcmFabricDeviceTypeUnknown:
                default:
                    dev_type = "Unknown";
                    break;
            }
#ifdef BCM_DNX_SUPPORT
            if (SOC_IS_DNX(unit))
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, port, &link));
            }
            else
#endif
            {
                /*
                 * For FE devices link is always equal to port
                 */
                link = port;
            }

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);

            PRT_CELL_SET("%d", link);
            PRT_CELL_SET("%d", port);
            PRT_CELL_SET("%d", links_connectivity_array[links_count].module_id);
            PRT_CELL_SET("%d", links_connectivity_array[links_count].link_id);
            PRT_CELL_SET("%s", dev_type);
        }

        links_count++;
    }

    if (array_size != links_count)
    {
        SHR_CLI_EXIT(_SHR_E_CONFIG,
                     "Number of enabled links doesn't match returned value from the 'bcm_fabric_link_connectivity_status_get' API \n");
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/*
 * Thresholds diagnostic
 */

/*
 * Link Status/Config
 */

/* *INDENT-OFF* */
static sh_sand_option_t dnxc_fabric_link_status_options[] = {
    {"pbmp",         SAL_FIELD_TYPE_PORT,      "port # / logical port type / port name", "sfi", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};
/* *INDENT-ON* */

/* *INDENT-OFF* */
static sh_sand_man_t dnx_fabric_link_status_man = {
    .brief =    "Display fabric link status. \n",
    .full =     DIAG_DNXC_LINK_STATUS_FULL_MANUAL,
#ifdef INCLUDE_CTEST
    .init_cb = cmd_dnxc_fabric_init_cb,
    .deinit_cb = cmd_dnxc_fabric_deinit_cb,
#endif
    .synopsis = "<pbmp>",
    .examples = "\n"
                "fabric0",
};

static sh_sand_man_t dnxf_fabric_link_status_man = {
    .brief =    "Display fabric link status. \n",
    .full =     DIAG_DNXC_LINK_STATUS_FULL_MANUAL,
    .synopsis = "<pbmp>",
    .examples = "\n"
                "fabric0",
};

/* *INDENT-ON* */

/**
 * \brief - display fabric link status
 */
static shr_error_e
cmd_dnxc_fabric_link_status(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t pbmp;
    bcm_port_t logical_port;
    uint32 links_status, links_errored_token_count;
    bcm_pbmp_t diag_info_bmp;
    bcm_port_config_t port_config;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Retrieve the command inputs
     */
    SH_SAND_GET_PORT("pbmp", pbmp);

    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));

    PRT_TITLE_SET("Links status");

    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Logical Port");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "CRC Error");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Size Error");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Code Group Error");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Misalign");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "No Signal Lock");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "No signal accept");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Errored tokens");
    PRT_COLUMN_ADD_FLEX(PRT_FLEX_ASCII, "Errored tokens count");

    BCM_PBMP_CLEAR(diag_info_bmp);

    /*
     * Show diagnostic only for specified link
     */
    BCM_PBMP_ITER(pbmp, logical_port)
    {
        if (!BCM_PBMP_MEMBER(port_config.sfi, logical_port))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM,
                         "Provided link %d either doesn't exists or is not enabled. Please provide valid link! \n",
                         logical_port);
        }

        BCM_PBMP_PORT_ADD(diag_info_bmp, logical_port);
    }

    BCM_PBMP_ITER(diag_info_bmp, logical_port)
    {

        SHR_IF_ERR_EXIT(bcm_fabric_link_status_get(unit, logical_port, &links_status, &links_errored_token_count));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        PRT_CELL_SET("%d", logical_port);       /* Link number */

        PRT_CELL_SET("%s", links_status & BCM_FABRIC_LINK_STATUS_CRC_ERROR ? "***" : " - ");
        PRT_CELL_SET("%s", links_status & BCM_FABRIC_LINK_STATUS_SIZE_ERROR ? "***" : " - ");
        PRT_CELL_SET("%s", links_status & BCM_FABRIC_LINK_STATUS_CODE_GROUP_ERROR ? "***" : " - ");
        PRT_CELL_SET("%s", links_status & BCM_FABRIC_LINK_STATUS_MISALIGN ? "***" : " - ");
        PRT_CELL_SET("%s", links_status & BCM_FABRIC_LINK_STATUS_NO_SIG_LOCK ? "***" : " - ");
        PRT_CELL_SET("%s", links_status & BCM_FABRIC_LINK_STATUS_NO_SIG_ACCEP ? "***" : " - ");
        PRT_CELL_SET("%s", links_status & BCM_FABRIC_LINK_STATUS_ERRORED_TOKENS ? "***" : " - ");
        PRT_CELL_SET("%d", links_errored_token_count);
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - display fabric link configuration
 */

/* *INDENT-OFF* */
static sh_sand_man_t dnx_fabric_link_config_man = {
    .brief =    "Display fabric link config \n",
    .full =     DIAG_DNXC_LINK_CONFIG_FULL_MANUAL,
#ifdef INCLUDE_CTEST
    .init_cb = cmd_dnxc_fabric_init_cb,
    .deinit_cb = cmd_dnxc_fabric_deinit_cb,
#endif
    .synopsis = "<pbmp>",
    .examples = "\n"
                "fabric0",
};

static sh_sand_man_t dnxf_fabric_link_config_man = {
    .brief =    "Display fabric link config \n",
    .full =     DIAG_DNXC_LINK_CONFIG_FULL_MANUAL,
    .synopsis = "<pbmp>",
    .examples = "\n"
                "fabric0",
};
/* *INDENT-ON* */

/* *INDENT-OFF* */
static sh_sand_option_t dnxc_fabric_link_config_options[] = {
    {"pbmp",         SAL_FIELD_TYPE_PORT,    "port # / logical port type / port name", "sfi", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};
/* *INDENT-ON* */

typedef struct diag_dnxc_fabric_link_config_s
{
    int enable;
    int speed;
    bcm_port_phy_fec_t fec_type;
    int fec_llfc_extract_cig;
    int fec_error_detect;
    int fec_llfc_after_fec;
    int fec_llfc_low_latency;
    int fec_control_cells_fec_bypass;
    uint32 cl72;
    int pcp;
    bcm_fabric_link_remote_pipe_mapping_t pipe_mapping;
    uint32 rx_polarity;
    uint32 tx_polarity;
    uint32 lane_swap;
    int clk_freq;
} diag_dnxc_fabric_link_config_t;

static shr_error_e
cmd_dnxc_fabric_link_config(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t pbmp;
    diag_dnxc_fabric_link_config_t link_config;
    bcm_port_config_t port_config;
    bcm_pbmp_t diag_info_bmp;
    char fec_str[80], pipe_mapping_buf[20];
    char *fec_type, *pipe_mapping_str;
    bcm_port_t logical_port;
    int link_id, object_col_id = 0;
    bcm_port_lane_to_serdes_map_t serdes_map[SOC_DNXC_MAX_NOF_FABRIC_LINKS];
    bcm_fabric_pipe_t remote_pipe_map[SOC_DNXC_MAX_NOF_PIPES];
    bcm_port_resource_t resource;
    int nof_links = 0;
#ifdef BCM_DNX_SUPPORT
    int ref_clk_dnx = -1;
#endif
    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Retrieve the command inputs
     */
    SH_SAND_GET_PORT("pbmp", pbmp);

    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, &port_config));

    BCM_PBMP_CLEAR(diag_info_bmp);

    BCM_PBMP_ITER(pbmp, logical_port)
    {
        if (!BCM_PBMP_MEMBER(port_config.sfi, logical_port))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM,
                         "Provided link %d either doesn't exists or is not enabled. Please provide valid link! \n",
                         logical_port);
        }

        BCM_PBMP_PORT_ADD(diag_info_bmp, logical_port);
    }

    PRT_TITLE_SET("Link Config");

    PRT_COLUMN_ADDX_FLEX(PRT_FLEX_ASCII, PRT_XML_CHILD, PRT_TITLE_ID, &object_col_id, "Logical Port");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Enable");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Speed");
    PRT_COLUMN_ADDX_FLEX(PRT_FLEX_ASCII, PRT_XML_ATTRIBUTE, object_col_id, NULL, "Ref clock");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "CL72");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "FEC");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "PCP");
    PRT_COLUMN_ADDX_FLEX(PRT_FLEX_ASCII, PRT_XML_ATTRIBUTE, object_col_id, NULL, "Pipe mapping");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "Polarity");
    PRT_COLUMN_ADDX(PRT_XML_ATTRIBUTE, object_col_id, NULL, "SerDes");

    /*
     * Get Serdes Mapping
     */
#ifdef BCM_DNXF_SUPPORT
    if (SOC_IS_DNXF(unit))
    {
        nof_links = dnxf_data_port.general.nof_links_get(unit);
    }
#endif
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        nof_links = dnx_data_fabric.links.nof_links_get(unit);
    }
#endif
    SHR_IF_ERR_EXIT(bcm_port_lane_to_serdes_map_get(unit, BCM_PORT_LANE_TO_SERDES_FABRIC_SIDE, nof_links, serdes_map));

    BCM_PBMP_ITER(diag_info_bmp, logical_port)
    {
        sal_memset(&link_config, 0, sizeof(diag_dnxc_fabric_link_config_t));
        /*
         * Get link enable status
         */
        SHR_IF_ERR_EXIT(bcm_port_enable_get(unit, logical_port, &link_config.enable));

        /*
         * Get link resource
         */
        SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, logical_port, &resource));

        /*
         * Get link speed
         */
        link_config.speed = resource.speed;

        /*
         * Get Link CL72 status
         */
        link_config.cl72 = resource.link_training;

        /*
         * Get link FEC status
         */
        link_config.fec_type = resource.fec_type;

        if (link_config.fec_type == bcmPortPhyFecBaseR || link_config.fec_type == bcmPortPhyFecRs206
            || link_config.fec_type == bcmPortPhyFecRs108 || link_config.fec_type == bcmPortPhyFecRs545
            || link_config.fec_type == bcmPortPhyFecRs304)
        {
            SHR_IF_ERR_EXIT(bcm_port_control_get
                            (unit, logical_port, bcmPortControlFecErrorDetectEnable, &link_config.fec_error_detect));
        }

        if (link_config.fec_type == bcmPortPhyFecBaseR)
        {
            SHR_IF_ERR_EXIT(bcm_port_control_get
                            (unit, logical_port, bcmPortControlLlfcCellsCongestionIndEnable,
                             &link_config.fec_llfc_extract_cig));
            SHR_IF_ERR_EXIT(bcm_port_control_get
                            (unit, logical_port, bcmPortControlLowLatencyLLFCEnable,
                             &link_config.fec_llfc_low_latency));
        }

        if (link_config.fec_type == bcmPortPhyFecRs206 || link_config.fec_type == bcmPortPhyFecRs108
            || link_config.fec_type == bcmPortPhyFecRs545 || link_config.fec_type == bcmPortPhyFecRs304)
        {
            SHR_IF_ERR_EXIT(bcm_port_control_get
                            (unit, logical_port, bcmPortControlLLFCAfterFecEnable, &link_config.fec_llfc_after_fec));
            SHR_IF_ERR_EXIT(bcm_port_control_get
                            (unit, logical_port, bcmPortControlControlCellsFecBypassEnable,
                             &link_config.fec_control_cells_fec_bypass));
        }

        /*
         * Get Link TX Polarity status
         */
        SHR_IF_ERR_EXIT(bcm_port_phy_control_get
                        (unit, logical_port, BCM_PORT_PHY_CONTROL_TX_POLARITY, &link_config.tx_polarity));

        /*
         * Get Link RX Polarity status
         */
        SHR_IF_ERR_EXIT(bcm_port_phy_control_get
                        (unit, logical_port, BCM_PORT_PHY_CONTROL_RX_POLARITY, &link_config.rx_polarity));

        /*
         * Get link pipe mapping
         */
        bcm_fabric_link_remote_pipe_mapping_t_init(&link_config.pipe_mapping);
        link_config.pipe_mapping.remote_pipe_mapping = remote_pipe_map;
        link_config.pipe_mapping.remote_pipe_mapping_max_size = SOC_DNXC_MAX_NOF_PIPES;
        link_config.pcp = 0;

        if (SOC_IS_DNX(unit))
        {
#ifdef BCM_DNX_SUPPORT
            link_config.pcp = dnx_data_fabric.cell.pcp_enable_get(unit);
#endif
        }
        else
        {
#ifdef BCM_DNXF_SUPPORT
            SHR_IF_ERR_EXIT(bcm_fabric_link_remote_pipe_mapping_get(unit, logical_port, &link_config.pipe_mapping));
            SHR_IF_ERR_EXIT(bcm_fabric_link_control_get(unit, logical_port, bcmFabricLinkPcpEnable, &link_config.pcp));
#endif
        }
        /*
         * Get link's ref clk value
         */
        if (SOC_IS_DNX(unit))
        {
#ifdef BCM_DNX_SUPPORT
            SHR_IF_ERR_EXIT(dnx_algo_port_soc_ref_clk_get(unit, logical_port, &ref_clk_dnx));

            switch (ref_clk_dnx)
            {
                case DNX_SERDES_REF_CLOCK_125:
                {
                    link_config.clk_freq = 125;
                    break;
                }
                case DNX_SERDES_REF_CLOCK_156_25:
                {
                    link_config.clk_freq = 156;
                    break;
                }
                case DNX_SERDES_REF_CLOCK_312_5:
                {
                    link_config.clk_freq = 312;
                    break;
                }
                default:
                {
                    link_config.clk_freq = -1;
                }
            }
#endif
        }
        else
        {
#ifdef BCM_DNXF_SUPPORT
            link_config.clk_freq = SOC_INFO(unit).port_refclk_int[logical_port];
#endif
        }

        /*
         * Convert link FEC type to string
         */
        /*
         * { FEC info string create
         */
        switch (link_config.fec_type)
        {
            case bcmPortPhyFecNone:
                fec_type = "None";
                break;
            case bcmPortPhyFecBaseR:
                fec_type = "BaseR";
                break;
            case bcmPortPhyFecRs206:
                fec_type = "Rs206";
                break;
            case bcmPortPhyFecRs108:
                fec_type = "Rs108";
                break;
            case bcmPortPhyFecRs545:
                fec_type = "Rs545";
                break;
            case bcmPortPhyFecRs304:
                fec_type = "Rs304";
                break;
            default:
                fec_type = "unknown";
                break;
        }
        sal_sprintf(fec_str, "%s%s%s%s%s%s",
                    fec_type,
                    link_config.fec_llfc_extract_cig ? " (+LlfcCig)" : "",
                    link_config.fec_error_detect ? " (+ErrInd)" : "",
                    link_config.fec_llfc_low_latency ? " (+LlfcLat)" : "",
                    link_config.fec_llfc_after_fec ? " (+LlfcAfterFec)" : "",
                    link_config.fec_control_cells_fec_bypass ? " (+CtrlFecByp)" : "");

        /*
         * } FEC info string create
         */

        /*
         * Format pipe mapping strings
         */
        if (link_config.pipe_mapping.num_of_remote_pipes == 0)
        {
            /*
             * no mapping
             */
            pipe_mapping_str = "None";
        }
        else if (link_config.pipe_mapping.num_of_remote_pipes == 1)
        {
            sal_sprintf(pipe_mapping_buf, "   0->%d   ", link_config.pipe_mapping.remote_pipe_mapping[0]);
            pipe_mapping_str = pipe_mapping_buf;
        }
        else if (link_config.pipe_mapping.num_of_remote_pipes == 2)
        {
            sal_sprintf(pipe_mapping_buf, "0->%d, 1->%d", link_config.pipe_mapping.remote_pipe_mapping[0],
                        link_config.pipe_mapping.remote_pipe_mapping[1]);
            pipe_mapping_str = pipe_mapping_buf;
        }
        else
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "ERROR: convert pipe mapping to string");
        }

#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_fabric_link_get(unit, logical_port, &link_id));
        }
        else
#endif
        {
            link_id = logical_port;
        }
        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        PRT_CELL_SET("%d", logical_port);
        PRT_CELL_SET("%d", link_config.enable);
        PRT_CELL_SET("%d", link_config.speed);
        PRT_CELL_SET("%d ", link_config.clk_freq);
        PRT_CELL_SET("%d", link_config.cl72 ? 1 : 0);
        PRT_CELL_SET("%s", fec_str);
        PRT_CELL_SET("%d", link_config.pcp);
        PRT_CELL_SET("%s", pipe_mapping_str);
        PRT_CELL_SET("rx:%d, tx:%d", link_config.rx_polarity, link_config.tx_polarity);
        PRT_CELL_SET("rx:%d, tx:%d", serdes_map[link_id].serdes_rx_id, serdes_map[link_id].serdes_tx_id);
    }

    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
static sh_sand_man_t dnxc_fabric_link_man = {
    .brief = "display fabric link status and configuration",
};
/* *INDENT-ON* */

/* *INDENT-OFF* */
sh_sand_cmd_t sh_dnxc_fabric_link_cmds[] = {
    /*keyword, action,                      command, options,                         man                                        Flags               Presence Callback*/
    {"status", cmd_dnxc_fabric_link_status, NULL,    dnxc_fabric_link_status_options, &dnx_fabric_link_status_man,   NULL, NULL, SH_CMD_CONDITIONAL | SH_CMD_NO_XML_VERIFY, sh_cmd_is_dnx},
    {"status", cmd_dnxc_fabric_link_status, NULL,    dnxc_fabric_link_status_options, &dnxf_fabric_link_status_man,  NULL, NULL, SH_CMD_CONDITIONAL | SH_CMD_NO_XML_VERIFY, sh_cmd_is_dnxf},
    {"config", cmd_dnxc_fabric_link_config, NULL,    dnxc_fabric_link_config_options, &dnx_fabric_link_config_man,   NULL, NULL, SH_CMD_CONDITIONAL, sh_cmd_is_dnx},
    {"config", cmd_dnxc_fabric_link_config, NULL,    dnxc_fabric_link_config_options, &dnxf_fabric_link_config_man,  NULL, NULL, SH_CMD_CONDITIONAL, sh_cmd_is_dnxf},
    {NULL}
};

/*
 * Mesh Topology
 */

/* *INDENT-OFF* */
static sh_sand_man_t dnxc_fabric_mesh_topology_man = {
    .brief =    "Diagnostic for mesh_topology block",
};
/* *INDENT-ON* */

/**
 * \brief - display fabric mesh_topology configuration
 */
static shr_error_e
cmd_dnxc_fabric_mesh_topology(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int link_idx, col_idx;
    soc_dnxc_fabric_mesh_topology_diag_t mesh_topology_diag;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get traffic info
     */
    SHR_IF_ERR_EXIT(soc_dnxc_fabric_mesh_topology_diag_get(unit, &mesh_topology_diag));

    /*
     * Print mesh topology diag topic
     */
    LOG_CLI(("MESH TOPOLOGY DIAG \n"));

    /*
     * Print mesh topology indications table {
     */
    PRT_TITLE_SET("Indications:");
    PRT_COLUMN_ADD("Indication");
    PRT_COLUMN_ADD("Value");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("1");
    PRT_CELL_SET("%u", mesh_topology_diag.indication_1);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("2");
    PRT_CELL_SET("%u", mesh_topology_diag.indication_2);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("3");
    PRT_CELL_SET("%u", mesh_topology_diag.indication_3);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("4");
    PRT_CELL_SET("%u", mesh_topology_diag.indication_4);

    PRT_COMMITX;

    /*
     * Print mesh topology indications table * }
     */

    /*
     * Print mesh topology counters table {
     */
    PRT_TITLE_SET("Counters:");
    PRT_COLUMN_ADD("Control cell type");
    PRT_COLUMN_ADD("Count");

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("rx_control_cells_type1");
    PRT_CELL_SET("%u", mesh_topology_diag.rx_control_cells_type1);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("rx_control_cells_type2");
    PRT_CELL_SET("%u", mesh_topology_diag.rx_control_cells_type2);

    PRT_ROW_ADD(PRT_ROW_SEP_NONE);
    PRT_CELL_SET("control_cells_type3");
    PRT_CELL_SET("%u", mesh_topology_diag.control_cells_type3);

    PRT_COMMITX;
    /*
     * Print mesh topology counters table }
     */

    /*
     * Print mesh topology link status table {
     */
    PRT_TITLE_SET("type2_list:");

    for (col_idx = 0; col_idx < DIAG_DNXC_MAX_NOF_MESH_TOPOLOGY_CLMNS; col_idx++)
    {
        PRT_COLUMN_ADD("#%d", col_idx + 1);
        PRT_COLUMN_ADD("Values%d", col_idx + 1);
        /*
         * Empty column to separete each link with its status
         */
        PRT_COLUMN_ADD("");
    }

    for (link_idx = 0; link_idx < mesh_topology_diag.link_list_count; link_idx++)
    {

        if ((link_idx % DIAG_DNXC_MAX_NOF_MESH_TOPOLOGY_CLMNS) == 0)
        {
            PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        }

        PRT_CELL_SET("%d", link_idx);
        if (mesh_topology_diag.type2_list[link_idx] != -1)
        {
            PRT_CELL_SET("0x%x", mesh_topology_diag.type2_list[link_idx]);
        }
        else
        {
            PRT_CELL_SET("N/A");
        }
        /*
         * Empty column to separete each link with its status
         */
        PRT_CELL_SET_SHIFT(0, "");
    }

    PRT_COMMITX;
    /*
     * Print mesh topology link status table }
     */

    /*
     * Print mesh topology Status table 1 {
     */
    LOG_CLI(("status 1:\n"));
    LOG_CLI(("status_1 %u \n", mesh_topology_diag.status_1));
    LOG_CLI(("status_1_id1 %u \n", mesh_topology_diag.status_1_id1));
    LOG_CLI(("status_1_id2 %u \n", mesh_topology_diag.status_1_id2));
    /*
     * Print mesh topology Status table 1 }
     */

    /*
     * Print mesh topology Status table 2 {
     */
    LOG_CLI(("status 2:\n"));
    LOG_CLI(("status_2 %u \n", mesh_topology_diag.status_2));
    LOG_CLI(("status_2_id1 %u \n", mesh_topology_diag.status_2_id1));
    LOG_CLI(("status_2_id2 %u \n", mesh_topology_diag.status_2_id2));
    /*
     * Print mesh topology Status table 2 }
     */
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/* *INDENT-OFF* */
static sh_sand_man_t dnxc_fabric_lane_map_man = {
    .brief    = "Diagnostic for Fabric Lane Map, only dump the MAPPED lanes",
    .full     = "Include the attached ports info for each lane",
    .synopsis = "[lane=<lane_id>]",
    .examples = "lane=10\n",
};
static sh_sand_option_t dnxc_fabric_lane_map_options[] = {
    /*name           type                  desc          default    ext*/
    {"lane",         SAL_FIELD_TYPE_INT32, "Lane ID",    "-1",      NULL},
    {NULL}
};

/* *INDENT-ON* */

/**
 * \brief - display fabric lane map info
 */
static shr_error_e
cmd_dnxc_fabric_lane_map(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int print_lane;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_INT32("lane", print_lane);

    SHR_IF_ERR_EXIT(cmd_dnxc_lane_map_dump(unit, DIAG_DNXC_LANE_TO_SERDES_FABRIC_SIDE, print_lane, sand_control));
exit:
    SHR_FUNC_EXIT;

}

/**
 * List of the supported commands, pointer to command function and command usage function.
 */
/* *INDENT-OFF* */

sh_sand_cmd_t sh_dnxc_fabric_cmds[] = {
    /*keyword,          action,                        command,                  options,                           man                               */
    {"reachability",    cmd_dnxc_fabric_reachability,  NULL,                     dnxc_fabric_reachability_options,  &dnxc_fabric_reachability_man},
    {"connectivity",    cmd_dnxc_fabric_connectivity,  NULL,                     NULL,                              &dnxc_fabric_connectivity_man, NULL, NULL, SH_CMD_NO_XML_VERIFY},
#ifdef BCM_DNXF_SUPPORT
    {"queues",          cmd_dnxf_fabric_queues,        NULL,                     dnxf_fabric_queues_options,        &dnxf_fabric_queues_man, NULL, NULL, SH_CMD_CONDITIONAL, sh_cmd_is_dnxf},
    {"traffic_profile", cmd_dnxf_fabric_traffic,       NULL,                     dnxf_fabric_traffic_options,       &dnxf_fabric_traffic_man, NULL, NULL, SH_CMD_CONDITIONAL, sh_cmd_is_dnxf},
    {"thresholds",      cmd_dnxf_fabric_thresholds,    NULL,                     dnxf_fabric_thresholds_options,    &dnxf_fabric_thresholds_man, NULL, NULL, SH_CMD_CONDITIONAL, sh_cmd_is_dnxf},
#endif
    {"link",            NULL,                          sh_dnxc_fabric_link_cmds, NULL,                              &dnxc_fabric_link_man},
    {"mesh_topology",   cmd_dnxc_fabric_mesh_topology, NULL,                     NULL,                              &dnxc_fabric_mesh_topology_man},
    {"lane_map",        cmd_dnxc_fabric_lane_map,      NULL,                     dnxc_fabric_lane_map_options,      &dnxc_fabric_lane_map_man},
#ifdef BCM_DNX_SUPPORT
    {"mesh",            cmd_dnx_fabric_mesh,           NULL,                     NULL,                              &dnx_fabric_mesh_man, NULL, NULL, SH_CMD_CONDITIONAL, sh_cmd_is_dnx_mesh},
    {"force",           cmd_dnx_fabric_force,          NULL,                     dnx_fabric_force_options,          &dnx_fabric_force_man, NULL, NULL, SH_CMD_CONDITIONAL, sh_cmd_is_dnx_clos},
#endif /* BCM_DNX_SUPPORT */
    {NULL}
};

sh_sand_man_t sh_dnxc_fabric_man = {
    "Miscellaneous facilities for displaying fabric information"
};

/* *INDENT-ON* */

/*
 * }
 */
