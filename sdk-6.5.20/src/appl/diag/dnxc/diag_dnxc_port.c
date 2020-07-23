/** \file diag_dnxc_port.c
 *
 * diagnostics for port and phy
 */
/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
#ifdef BSL_LOG_MODULE
#error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_APPL_SHELL

/*
 * Include files.
 * {
 */
#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/util.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/diag.h>
#include <appl/diag/sand/diag_sand_framework.h>
#include <appl/diag/sand/diag_sand_prt.h>
#include <appl/ctest/dnxc/ctest_dnxc_utils.h>

#include <appl/diag/phymod/phymod_sdk_diag.h>
#include <appl/diag/phymod/phymod_symop.h>
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_chain.h>

#include <bcm/error.h>
#include <bcm/debug.h>
#include <bcm/port.h>
#include <bcm/stat.h>

#include <soc/phy.h>
#include <soc/eyescan.h>
#include <soc/phyreg.h>
#include <soc/counter.h>
#include <soc/dnxc/dnxc_port.h>
#include <soc/dnxc/dnxc_defs.h>

#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/cmn/dnxf_drv.h>
#include <bcm_int/dnxf/port.h>
#include <soc/dnxf/cmn/mbcm.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_port.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_fabric.h>
#include <soc/dnxf/dnxf_data/auto_generated/dnxf_data_device.h>
#endif

#ifdef BCM_DNX_SUPPORT
#include <bcm_int/dnx/init/init.h>
#include <bcm_int/dnx/port/port.h>
#include <bcm_int/dnx/port/imb/imb.h>
#include <bcm_int/dnx/stat/mib/mib_stat.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <appl/diag/dnx/diag_dnx_fabric.h>
#include <soc/dnx/drv.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_nif.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_fabric.h>
#endif

#ifdef PHYMOD_LINKCAT_SUPPORT

#if defined(PHYMOD_LINKCAT_BLACKHAWK7_V1L8P1_SUPPORT) || defined(PHYMOD_LINKCAT_BLACKHAWK7_V1L8P2_SUPPORT)
#include "include/LinkCAT_lib.h"
#else
#include "include/Blackhawk_LinkCAT_lib.h"
#endif

#endif

#include "diag_dnxc_phy_prbsstat.h"

/*
 * }
 */

/*
 * Macros
 * {
 */
#define DIAG_DNXC_PORT_MAX_STR_LEN                32
#define DIAG_DNXC_PORT_NEW_CMD_LEN                100
#define DIAG_DNXC_PORT_BER_MAX_POW                18
#define DIAG_DNXC_PORT_STATUS_STRING_CHECK_PRINT(flags, mask, str, val)  \
    do {                                                                \
        if ((flags) & (mask)) {                                         \
            PRT_CELL_SET(str, val);                                     \
        } else {                                                        \
            PRT_CELL_SET("%s", "");                                     \
        }                                                               \
    } while (0)

#define DIAG_DNXC_MAX_CORE_ACCESS_STRUCTURES_PER_PORT 6
#define DIAG_DNXC_MAX_PORT_CORES                      6

#define DIAG_DNXC_PORT_STATUS_ACTION_MASK        (BCM_PORT_ATTR_ENABLE_MASK    | \
                                                  BCM_PORT_ATTR_LINKSTAT_MASK  | \
                                                  BCM_PORT_ATTR_AUTONEG_MASK   | \
                                                  BCM_PORT_ATTR_SPEED_MASK     | \
                                                  BCM_PORT_ATTR_LINKSCAN_MASK  | \
                                                  BCM_PORT_ATTR_LOOPBACK_MASK)
#define DIAG_DNXC_PHY_FECSTAT_NOF_COUNTERS       0x2
#define DIAG_DNXC_MAX_COUNTERS_PER_COUNTER_TYPE  0x4

#define DIAG_DNXC_PHY_DSC_BRIEF_MAN "Diagnostic for PHY dsc info"
#define DIAG_DNXC_PHY_DSC_FULL_MAN "Dump PHY DSC Info, only <pbmp> is mandatory. \n" \
                "For option \"state\", dump detailed DSC info, including"  \
                "lane info, core info and lane event info." \
                "For option \"config\", dump serdes lane and core configuration info" \
                "If no option is specified, dump the lane info and core info."
#define DIAG_DNXC_PHY_DSC_SYNOPSIS "<pbmp> [<state|config|NULL string>]"

#define DIAG_DNXC_PHY_RAW_SBUS_BRIEF_MAN    "Diagnostic for PHY raw sbus command"
#define DIAG_DNXC_PHY_RAW_SBUS_FULL_MAN     "Set or Get phy registers through sbus MDIO. " \
                                            "If option \"data\" is not present, it a read register operation. " \
                                            "This command is only available for DNX NIF PHY. \n" \
                                            "\"lane\" and \"pll_index\" are used for accessing registers according to " \
                                            "lane and PLL info."
#define DIAG_DNXC_PHY_RAW_SBUS_SYNOPSIS     "pm_id=<val> devad=<val> regad=<val> [lane=<val>] [pll_index=<val>] [data=<val>]"

/*
 * }
 */

/*
 * Enums
 * {
 */

typedef enum
{
    DIAG_DNXC_PHY_PRBS_MODE_PHY,
    DIAG_DNXC_PHY_PRBS_MODE_MAC
} diag_dnxc_phy_prbs_mode_e;

typedef enum
{
    DIAG_DNXC_PHY_PRBS_CURR_LOCK = 0,
    DIAG_DNXC_PHY_PRBS_CURR_LOCK_LOST = -1,
    DIAG_DNXC_PHY_PRBS_PREV_LOCK_LOST = -2,
    DIAG_DNXC_PHY_PRBS_COUNTER_OVF = 0x7FFFFFFF
} diag_dnxc_phy_prbs_status_e;

typedef enum
{
    DIAG_DNXC_PHY_CL72_OPTION_OFF,
    DIAG_DNXC_PHY_CL72_OPTION_ON,
    DIAG_DNXC_PHY_CL72_OPTION_STATE
} diag_dnxc_phy_cl72_option_e;

typedef enum
{
    DIAG_DNXC_PHY_MEASURE_OPTION_RX,
    DIAG_DNXC_PHY_MEASURE_OPTION_TX
} diag_dnxc_phy_measure_option_e;

typedef enum
{
    DIAG_DNXC_PHY_EYESCAN_TYPE1 = 1,
    DIAG_DNXC_PHY_EYESCAN_TYPE4 = 4
} diag_dnxc_phy_eyescan_type_e;

/*
 * }
 */

static uint64 phy_fecstat_val[SOC_MAX_NUM_DEVICES][SOC_MAX_NUM_PORTS][DIAG_DNXC_PHY_FECSTAT_NOF_COUNTERS] =
    { {{COMPILER_64_INIT(0, 0)}} };

#ifdef BCM_DNX_SUPPORT
#ifdef INCLUDE_CTEST
/** used in diag test  callbacks to store soc properties to be restored */
static rhhandle_t diag_dnxc_phy_soc_set_h[SOC_MAX_NUM_DEVICES] = { NULL };
#endif
#endif

/**
 * \brief - Check if given option should be present
 *
 * \param [in] unit - chip unit id
 * \param [in] sand_control - sand control
 * \param [in] option_str - option
 * \param [in] log_str - the log string to print
 * \param [in] should_present - if the option should be present
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnxc_phy_diag_option_present_check(
    int unit,
    sh_sand_control_t * sand_control,
    char *option_str,
    char *log_str,
    int should_present)
{
    int is_present;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_IS_PRESENT(option_str, is_present);
    if (!is_present && should_present)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Option '%s' is a must!\n", log_str);
    }
    else if (is_present && !should_present)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Option '%s' should not be present!\n", log_str);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Turn off linkscan
 *
 * \param [in] unit - chip unit id
 * \param [out] interval - linkscan interval
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnxc_phy_diag_linkscan_off(
    int unit,
    int *interval)
{
    SHR_FUNC_INIT_VARS(unit);

    *interval = 0;
    SHR_IF_ERR_EXIT(bcm_linkscan_enable_get(unit, interval));
    if (*interval != 0)
    {
        SHR_IF_ERR_EXIT(bcm_linkscan_enable_set(unit, 0));
    }

exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - Recover linkscan
 *
 * \param [in] unit - chip unit id
 * \param [in] interval - linkscan interval
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnxc_phy_diag_linkscan_recover(
    int unit,
    int interval)
{
    SHR_FUNC_INIT_VARS(unit);

    if (interval != 0)
    {
        SHR_IF_ERR_EXIT(bcm_linkscan_enable_set(unit, interval));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Format pbmp to str buff
 *
 * \param [in] unit - chip unit id
 * \param [in] bufsize - buff size
 * \param [in] pbmp - port pbmp
 * \param [out] buf - port format buff
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnxc_phy_diag_format_pbmp(
    int unit,
    char *buf,
    int bufsize,
    bcm_pbmp_t * pbmp)
{
    int port, offset;
    int count = 0;
    SHR_FUNC_INIT_VARS(unit);

    BCM_PBMP_ITER(*pbmp, port)
    {
        count++;
        offset = sal_snprintf(buf, bufsize, "%s%d", count == 1 ? "" : ",", port);
        buf += offset;
        bufsize -= offset;
    }
    SHR_EXIT();
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - Check if given pbmp is valid
 *
 * \param [in] unit - chip unit id
 * \param [in] ppbmp - port pbmp pointer
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnxc_phy_diag_pbmp_check(
    int unit,
    bcm_pbmp_t * ppbmp)
{
    bcm_port_config_t *port_config;
    char pbmp_str[FORMAT_PBMP_MAX];
    bcm_pbmp_t pbmp;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Coverity 2028067, large stack use
     */
    port_config = sal_alloc(sizeof(bcm_port_config_t), "port_config");
    if (NULL == port_config)
    {
    SHR_ERR_EXIT(_SHR_E_MEMORY, "Not enough memory to allocate 'port_config'.\n")}

    sal_memset(pbmp_str, 0, (sizeof(char) * FORMAT_PBMP_MAX));
    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, port_config));
    BCM_PBMP_ASSIGN(pbmp, *ppbmp);
    BCM_PBMP_REMOVE(pbmp, port_config->port);
    if (BCM_PBMP_NOT_NULL(pbmp))
    {
        SHR_IF_ERR_EXIT(dnxc_phy_diag_format_pbmp(unit, pbmp_str, sizeof(pbmp_str), &pbmp));
        SHR_CLI_EXIT(_SHR_E_PARAM, "Provided port %s either doesn't exists or is not enabled.\n", pbmp_str);
    }

exit:
    if (NULL != port_config)
    {
        sal_free(port_config);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - Function to access the phymod register symbol table
 *
 * \param [in] unit - chip unit id
 * \param [in] a - diag command argvs
 * \param [in] pbm - port bitmap
 *
 * \return
 *   int - see _SHR_E_*
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static shr_error_e
dnxc_phy_portmod_sym_access(
    int unit,
    args_t * a,
    bcm_pbmp_t * pbm)
{
    int port, i = 0;
    int nof_phys_structs;
    char hdr[DIAG_DNXC_PORT_MAX_STR_LEN];
    portmod_access_get_params_t params;
    phymod_symbols_iter_t iter;
    phymod_symbols_t *symbols_table = NULL;
    phymod_phy_access_t phy_access[DIAG_DNXC_MAX_CORE_ACCESS_STRUCTURES_PER_PORT];

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));

    if (SHR_FAILURE(phymod_symop_init(&iter, a)))
    {
        /*
         * phymod_symop_init returns CMD_OK/CMD_FAIL, convert the return value here
         */
         /* coverity[copy_paste_error : FALSE]  */
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Failed to parse command for PHY symbolic operation! \n");
    }

    BCM_PBMP_ITER(*pbm, port)
    {
        SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get
                        (unit, port, &params, DIAG_DNXC_MAX_CORE_ACCESS_STRUCTURES_PER_PORT, phy_access,
                         &nof_phys_structs, NULL));
        for (i = 0; i < nof_phys_structs; i++)
        {
            sal_snprintf(hdr, sizeof(hdr), "Port %s(0x%02x):\n", SOC_PORT_NAME(unit, port), phy_access[i].access.addr);
            SHR_IF_ERR_EXIT(phymod_diag_symbols_table_get(&phy_access[i], &symbols_table));
            SHR_IF_ERR_EXIT(phymod_symop_exec(&iter, symbols_table, &phy_access[i], hdr));
        }
    }
exit:
    /*
     * Coverity reports iter.vptr is not initialized, however iter.vptr has been initialized
     * in phymod_symop_init. Therefore ignore this issue.
     */
     /* coverity[uninit_use_in_call : FALSE]  */
    phymod_symop_cleanup(&iter);
    SHR_FUNC_EXIT;
}

/**
 * \brief - display PHY INFO
 */
static shr_error_e
cmd_dnxc_phy_info(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_port_t port;
    bcm_pbmp_t pbm;
    phymod_core_access_t core_acc[DIAG_DNXC_MAX_PORT_CORES];
    phymod_core_info_t core_info;
    int nof_cores = 0;
    int first_phy = 0;
    portmod_port_diag_info_t diag_info;
    int i;
    char lnstr[DIAG_DNXC_PORT_MAX_STR_LEN], *pname;
    bcm_port_config_t *port_config = NULL;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    PRT_TITLE_SET("PHY INFO Dump:");

    PRT_COLUMN_ADD("Port");
    PRT_COLUMN_ADD("ID0");
    PRT_COLUMN_ADD("ID1");
    PRT_COLUMN_ADD("ADDR");
    PRT_COLUMN_ADD("Name");

    /*
     * Get NIF and SFI ports
     */
    SHR_ALLOC_SET_ZERO(port_config, sizeof(bcm_port_config_t), "port config", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_IF_ERR_EXIT(bcm_port_config_get(unit, port_config));
    SOC_PBMP_ASSIGN(pbm, port_config->port);

    BCM_PBMP_ITER(pbm, port)
    {
        for (i = 0; i < DIAG_DNXC_MAX_PORT_CORES; i++)
        {
            SHR_IF_ERR_EXIT(phymod_core_access_t_init(&core_acc[i]));
        }

        SHR_IF_ERR_EXIT(phymod_core_info_t_init(&core_info));

        sal_memset(&diag_info, 0, sizeof(portmod_port_diag_info_t));

        SHR_IF_ERR_EXIT(portmod_port_core_access_get
                        (unit, port, -1, DIAG_DNXC_MAX_PORT_CORES, core_acc, &nof_cores, NULL));

        if (nof_cores == 0)
        {
            continue;
        }

        SHR_IF_ERR_EXIT(portmod_port_diag_info_get(unit, port, &diag_info));

        PORTMOD_PBMP_ITER(diag_info.phys, first_phy)
        {
            break;
        }

        SHR_IF_ERR_EXIT(phymod_core_info_get(core_acc, &core_info));

        pname = phymod_core_version_t_mapping[core_info.core_version].key;

        sal_snprintf(lnstr, sizeof(lnstr), "%s", pname);

        pname = lnstr;
        while (*pname != '\0')
        {
            /*
             * Change the lower case to upper case
             */
            *pname = sal_toupper(*pname);
            pname++;
        }

        pname = lnstr + sal_strlen(lnstr);
        /*
         * Append first phy ID
         */
        sal_snprintf(pname, sizeof(lnstr) - sal_strlen(lnstr) + 1, "/%d", first_phy);

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        PRT_CELL_SET("%s(%d)", SOC_PORT_NAME(unit, port), port);
        PRT_CELL_SET("0x%x", core_info.phy_id0);
        PRT_CELL_SET("0x%x", core_info.phy_id1);
        PRT_CELL_SET("0x%x", core_acc[0].access.addr);
        PRT_CELL_SET("%s", lnstr);
    }
    PRT_COMMITX;
exit:
    SHR_FREE(port_config);
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - CMD for setting prbs
 */
static shr_error_e
cmd_dnxc_phy_prbs_set(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int mode, poly, invert;
    int ls_interval = 0, status;
    bcm_port_t port;
    bcm_pbmp_t pbmp;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("pbmp", pbmp);
    SH_SAND_GET_BOOL("invert", invert);
    SH_SAND_GET_ENUM("polynomial", poly);
    SH_SAND_GET_ENUM("mode", mode);

    SHR_IF_ERR_EXIT(dnxc_phy_diag_pbmp_check(unit, &pbmp));
    /*
     * Close linkscan
     */
    SHR_IF_ERR_EXIT(dnxc_phy_diag_linkscan_off(unit, &ls_interval));

    BCM_PBMP_ITER(pbmp, port)
    {
        SHR_IF_ERR_EXIT(bcm_port_control_set(unit, port, bcmPortControlPrbsMode, mode));
        /*
         * Set polynomial and invert
         */
        if (mode == DIAG_DNXC_PHY_PRBS_MODE_MAC)
        {
            if (poly != BCM_PORT_PRBS_POLYNOMIAL_X31_X28_1)
            {
                SHR_CLI_EXIT(_SHR_E_PARAM,
                             "ERROR: Setting prbs polynomial %d is not allowed when prbs injection mode is MAC. Only supported value for MAC PRBS is %d \n",
                             poly, BCM_PORT_PRBS_POLYNOMIAL_X31_X28_1);
            }
            if (invert)
            {
                 /* coverity[copy_paste_error : FALSE]  */
                SHR_CLI_EXIT(_SHR_E_PARAM, "ERROR: Invert is not supported for MAC PRBS! \n");
            }
        }
        else
        {
            SHR_IF_ERR_EXIT(bcm_port_control_set(unit, port, bcmPortControlPrbsPolynomial, poly));
            SHR_IF_ERR_EXIT(bcm_port_control_set(unit, port, bcmPortControlPrbsTxInvertData, invert));
        }
        /*
         * PRBS enable order: Rx -> Tx
         */
        SHR_IF_ERR_EXIT(bcm_port_control_set(unit, port, bcmPortControlPrbsRxEnable, 1));
        SHR_IF_ERR_EXIT(bcm_port_control_set(unit, port, bcmPortControlPrbsTxEnable, 1));
        /*
         * clear errors
         */
        SHR_IF_ERR_EXIT(bcm_port_control_get(unit, port, bcmPortControlPrbsRxStatus, &status));
    }
exit:
    if ((_func_rv = dnxc_phy_diag_linkscan_recover(unit, ls_interval)) < 0)
    {
        cli_out("Failed to recover linkscan, orginial linkscan interval is %d \n", ls_interval);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - Get the PAM4 rate according to the lane speed and fec_type
 *
 * \param [in] unit - chip unit id
 * \param [in] port - logical port
 * \param [out] serdes_rate - serdes rate for single lane in [Mb/s]
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
dnxc_phy_diag_serdes_rate_get(
    int unit,
    bcm_port_t port,
    uint32 *serdes_rate)
{
    int lane_speed;
    portmod_speed_config_t speed_config;

    SHR_FUNC_INIT_VARS(unit);

    /*
     * Get the lane speed
     */
    SHR_IF_ERR_EXIT(portmod_port_speed_config_get(unit, port, &speed_config));

     /* coverity[negative_shift:FALSE]  */
    if (IS_IL_PORT(unit, port) || IS_SFI_PORT(unit, port))
    {
        lane_speed = speed_config.speed;
    }
    else
    {
        lane_speed = speed_config.speed / speed_config.num_lane;
    }

    *serdes_rate = 0;
    switch (lane_speed)
    {
        case 53125:
        case 56250:
        case 28125:
        case 27343:
        case 25781:
        case 12500:
        case 10312:
            *serdes_rate = lane_speed;
            break;
        case 50000:
            if (IS_SFI_PORT(unit, port) || IS_IL_PORT(unit, port))
            {
                *serdes_rate = 50000;
            }
            else
            {
                if ((speed_config.fec == PORTMOD_PORT_PHY_FEC_NONE)
                    || (speed_config.fec == PORTMOD_PORT_PHY_FEC_RS_FEC))
                {
                    *serdes_rate = 51562;
                }
                else if ((speed_config.fec == PORTMOD_PORT_PHY_FEC_RS_544)
                         || (speed_config.fec == PORTMOD_PORT_PHY_FEC_RS_544_2XN)
                         || (speed_config.fec == PORTMOD_PORT_PHY_FEC_RS_272)
                         || (speed_config.fec == PORTMOD_PORT_PHY_FEC_RS_272_2XN))
                {
                    *serdes_rate = 53125;
                }
                else
                {
                    SHR_CLI_EXIT(_SHR_E_PARAM, "PAM4 does not support fec type %d\n", speed_config.fec);
                }
            }
            break;
        case 25000:
            if (IS_SFI_PORT(unit, port) || IS_IL_PORT(unit, port))
            {
                *serdes_rate = 25000;
            }
            else
            {
                if (speed_config.fec == PORTMOD_PORT_PHY_FEC_RS_544)
                {
                    *serdes_rate = 26562;
                }
                else
                {
                    *serdes_rate = 25781;
                }
            }
            break;
        case 20000:
            *serdes_rate = 20625;
            break;
        case 10000:
            *serdes_rate = 10312;
            break;
        default:
            SHR_CLI_EXIT(_SHR_E_PARAM, "lane speed %d is not supported!\n", lane_speed);
    }
exit:
    SHR_FUNC_EXIT;
}

shr_error_e
dnxc_diag_format_big_integer_div(
    int unit,
    uint64 divisor,
    int dividend,
    uint32 *int32,
    uint32 *remainder32,
    int *power)
{
    int pow = 0, temp_int32 = 0, temp_remainder32 = 0;
    uint64 base = COMPILER_64_INIT(0, 0);
    uint64 temp_delta, int64, remainder64;
    uint64 tmp_64;

    SHR_FUNC_INIT_VARS(unit);
    COMPILER_64_ZERO(tmp_64);
    if (COMPILER_64_EQ(divisor, tmp_64) || int32 == NULL || remainder32 == NULL || power == NULL)
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Invalid parameters!\n");
    }

    for (pow = 0; pow <= DIAG_DNXC_PORT_BER_MAX_POW; pow++)
    {
        if (pow == 0)
        {
            COMPILER_64_SET(base, 0, 1);
        }
        else
        {
            COMPILER_64_UMUL_32(base, 10);
        }

        COMPILER_64_COPY(temp_delta, base);
        COMPILER_64_UMUL_32(temp_delta, dividend);

        COMPILER_64_COPY(int64,
                         temp_delta);
        COMPILER_64_UDIV_64(int64,
                            divisor);

        COMPILER_64_TO_32_LO(temp_int32, int64);
        if (temp_int32 >= 1)
        {
            break;
        }
        else if (pow == DIAG_DNXC_PORT_BER_MAX_POW)
        {
             /* coverity[copy_paste_error:FALSE]  */
            SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error happened while formatting big integer!\n");
        }
    }

    COMPILER_64_UMUL_32(temp_delta, 100);
    COMPILER_64_COPY(remainder64, temp_delta);
    COMPILER_64_UDIV_64(remainder64, divisor);
    COMPILER_64_TO_32_LO(temp_remainder32, remainder64);
    *int32 = temp_int32;
    *remainder32 = temp_remainder32 % 100;
    *power = pow;

exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
_phy_prbs_status_get(
    int unit,
    bcm_port_t port,
    int is_lane_port,
    sal_usecs_t start_time)
{
    uint32 serdes_rate = 0, ber_int = 0, ber_remainder = 0;
    int status, pow = 0;
    uint64 total_bits;
    sal_usecs_t real_interval;
    int logical_port, lane = 0;

    SHR_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(bcm_port_control_get(unit, port, bcmPortControlPrbsRxStatus, &status));

    if (is_lane_port)
    {
        logical_port = BCM_PHY_GPORT_LANE_PORT_PORT_GET(port);
        lane = BCM_PHY_GPORT_LANE_PORT_LANE_GET(port);
    }
    else
    {
        logical_port = port;
    }

    switch (status)
    {
        case DIAG_DNXC_PHY_PRBS_CURR_LOCK:
            if (is_lane_port)
            {
                cli_out("%s (lane %1d):  PRBS PASSED (LOCKED and returns no error). BER=0!\n",
                        BCM_PORT_NAME(unit, logical_port), lane);
            }
            else
            {
                cli_out("%s :  PRBS PASSED (LOCKED and returns no error). BER=0!\n", BCM_PORT_NAME(unit, logical_port));
            }
            break;
        case DIAG_DNXC_PHY_PRBS_CURR_LOCK_LOST:
            if (is_lane_port)
            {
                cli_out("%s (lane %1d):  PRBS FAILED (not LOCKED)!\n", BCM_PORT_NAME(unit, logical_port), lane);
            }
            else
            {
                cli_out("%s :  PRBS FAILED (not LOCKED)!\n", BCM_PORT_NAME(unit, logical_port));
            }
            break;
        case DIAG_DNXC_PHY_PRBS_PREV_LOCK_LOST:
            if (is_lane_port)
            {
                cli_out("%s (lane %1d):  PRBS FAILED (currently LOCKED but was not LOCKED since last read)!\n",
                        BCM_PORT_NAME(unit, logical_port), lane);
            }
            else
            {
                cli_out("%s :  PRBS FAILED (currently LOCKED but was not LOCKED since last read)!\n",
                        BCM_PORT_NAME(unit, logical_port));
            }
            break;
        case DIAG_DNXC_PHY_PRBS_COUNTER_OVF:
            if (is_lane_port)
            {
                cli_out("%s (lane %1d):  PRBS FAILED with %d errors(0x7fffffff => saturated). BER is not reliable!\n",
                        BCM_PORT_NAME(unit, logical_port), lane, status);
            }
            else
            {
                cli_out("%s :  PRBS FAILED with %d errors(0x7fffffff => saturated). BER is not reliable!\n",
                        BCM_PORT_NAME(unit, logical_port), status);
            }
            break;
        default:
            /*
             * Get the real interval
             */
            real_interval = SAL_USECS_SUB(sal_time_usecs(), start_time);
            /*
             * Get PAM4 rate
             */
            SHR_IF_ERR_EXIT(dnxc_phy_diag_serdes_rate_get(unit, logical_port, &serdes_rate));
            /*
             * Get the total bits
             */
            COMPILER_64_SET(total_bits, 0, real_interval);
            COMPILER_64_UMUL_32(total_bits, serdes_rate);
            SHR_IF_ERR_EXIT(dnxc_diag_format_big_integer_div(unit, total_bits, status, &ber_int, &ber_remainder, &pow));

            if (is_lane_port)
            {
                cli_out("%s (lane %1d):  PRBS FAILED with %d errors. BER=%d.%02de-%02d!\n",
                        BCM_PORT_NAME(unit, logical_port), lane, status, ber_int, ber_remainder, pow);
            }
            else
            {
                cli_out("%s :  PRBS FAILED with %d errors. BER=%d.%02de-%02d!\n", BCM_PORT_NAME(unit, logical_port),
                        status, ber_int, ber_remainder, pow);
            }

            break;
    }
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - CMD for getting prbs status
 */
static shr_error_e
cmd_dnxc_phy_prbs_get(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int ls_interval = 0, interval, status;
    bcm_port_t port;
    bcm_pbmp_t pbmp;
    sal_usecs_t *start_time = NULL;

#ifdef BCM_DNX_SUPPORT
    bcm_pbmp_t nif_phys;
    int lane, lane_count;
    bcm_port_t lane_port;
#endif

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("pbmp", pbmp);
    SH_SAND_GET_INT32("interval", interval);

    SHR_IF_ERR_EXIT(dnxc_phy_diag_pbmp_check(unit, &pbmp));
    /*
     * Close linkscan
     */
    SHR_IF_ERR_EXIT(dnxc_phy_diag_linkscan_off(unit, &ls_interval));

    /*
     * Coverity 2028067, large stack use
     */
    start_time = sal_alloc(SOC_MAX_NUM_PORTS * sizeof(sal_usecs_t), "start_time");
    if (NULL == start_time)
    {
    SHR_ERR_EXIT(_SHR_E_MEMORY, "Not enough memory to allocate 'start_time'.\n")}
    else
    {
        sal_memset(start_time, 0, SOC_MAX_NUM_PORTS * sizeof(sal_usecs_t));
    }
    /*
     * Clear the PRBS status for all the given ports and record the start time.
     * If there are errors during PRBS test, we need the real time to
     * calculate the BER.
     */
    BCM_PBMP_ITER(pbmp, port)
    {
        SHR_IF_ERR_EXIT(bcm_port_control_get(unit, port, bcmPortControlPrbsRxStatus, &status));
        start_time[port] = sal_time_usecs();
    }
    sal_sleep(interval);

    BCM_PBMP_ITER(pbmp, port)
    {
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit) && !IS_SFI_PORT(unit, port))
        {
            /*
             * Get NIF phys
             */
            SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &nif_phys));
            lane_count = 0;
            BCM_PBMP_ITER(nif_phys, lane)
            {
                /*
                 * Get per lane gports status.
                 */
                BCM_PHY_GPORT_LANE_PORT_SET(lane_port, lane_count, port);
                SHR_IF_ERR_EXIT(_phy_prbs_status_get(unit, lane_port, TRUE, start_time[port]));
                ++lane_count;
            }
        }
        else
#endif
        {
            /*
             * check prbs results
             */
            SHR_IF_ERR_EXIT(_phy_prbs_status_get(unit, port, FALSE, start_time[port]));
        }
    }
exit:
    if ((_func_rv = dnxc_phy_diag_linkscan_recover(unit, ls_interval)) < 0)
    {
        cli_out("Failed to recover linkscan, orginial linkscan interval is %d \n", ls_interval);
    }
    if (NULL != start_time)
    {
        sal_free(start_time);
    }
    SHR_FUNC_EXIT;
}
/**
 * \brief - CMD for stopping PRBS generator and checker
 */
static shr_error_e
cmd_dnxc_phy_prbs_clear(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int mode = 0;
    int def_poly = BCM_PORT_PRBS_POLYNOMIAL_X31_X28_1;
    int def_invert = 0;
    int def_prbs_mode = DIAG_DNXC_PHY_PRBS_MODE_PHY;
    int ls_interval = 0;
    bcm_port_t port;
    bcm_pbmp_t pbmp;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("pbmp", pbmp);

    SHR_IF_ERR_EXIT(dnxc_phy_diag_pbmp_check(unit, &pbmp));
    /*
     * Close linkscan
     */
    SHR_IF_ERR_EXIT(dnxc_phy_diag_linkscan_off(unit, &ls_interval));

    BCM_PBMP_ITER(pbmp, port)
    {
        /*
         * Get the PRBS mode
         */
        SHR_IF_ERR_EXIT(bcm_port_control_get(unit, port, bcmPortControlPrbsMode, &mode));
        /*
         * Restore the default poly and invert value
         */
        if (mode == DIAG_DNXC_PHY_PRBS_MODE_PHY)
        {
            SHR_IF_ERR_EXIT(bcm_port_control_set(unit, port, bcmPortControlPrbsPolynomial, def_poly));
            SHR_IF_ERR_EXIT(bcm_port_control_set(unit, port, bcmPortControlPrbsTxInvertData, def_invert));
        }
        /*
         * Disale PRBS, order: Tx -> Rx
         */
        SHR_IF_ERR_EXIT(bcm_port_control_set(unit, port, bcmPortControlPrbsTxEnable, 0));
        sal_usleep(1000);
        SHR_IF_ERR_EXIT(bcm_port_control_set(unit, port, bcmPortControlPrbsRxEnable, 0));
        /*
         * Restore the default PRBS mode
         */
        if (mode != def_prbs_mode)
        {
            SHR_IF_ERR_EXIT(bcm_port_control_set(unit, port, bcmPortControlPrbsMode, def_prbs_mode));
        }
    }
exit:
    if ((_func_rv = dnxc_phy_diag_linkscan_recover(unit, ls_interval)) < 0)
    {
        cli_out("Failed to recover linkscan, orginial linkscan interval is %d \n", ls_interval);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - CMD for PHY measure
 */
static shr_error_e
cmd_dnxc_phy_measure(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int is_rx;
    bcm_port_t port;
    bcm_pbmp_t pbmp;
    diag_dnxc_phy_measure_option_e option;
    soc_dnxc_port_phy_measure_t phy_measure;
    char pll_str[DIAG_DNXC_PORT_MAX_STR_LEN];

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("pbmp", pbmp);
    SH_SAND_GET_ENUM("direction", option);

    is_rx = (option == DIAG_DNXC_PHY_MEASURE_OPTION_RX) ? 1 : 0;

    PRT_TITLE_SET("PHY MEASURE:");

    PRT_COLUMN_ADD("Port");
    PRT_COLUMN_ADD("Measured Lane");
    PRT_COLUMN_ADD("Serdes Rate [Gbps]");
    PRT_COLUMN_ADD("PLL [Mhz]");

    SHR_IF_ERR_EXIT(dnxc_phy_diag_pbmp_check(unit, &pbmp));
    BCM_PBMP_ITER(pbmp, port)
    {
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            SHR_IF_ERR_EXIT(dnx_port_phy_measure_get(unit, port, is_rx, &phy_measure));
            /*
             * If the measure value is not valid, continue.
             */
            if (!phy_measure.valid)
            {
                continue;
            }
            sal_snprintf(pll_str, sizeof(pll_str), "%d.%02d", phy_measure.ref_clk_int, phy_measure.ref_clk_remainder);
        }
        else
#endif
        {
#ifdef BCM_DNXF_SUPPORT
            SHR_IF_ERR_EXIT(dnxf_port_phy_measure_get(unit, port, is_rx, &phy_measure));
            sal_snprintf(pll_str, sizeof(pll_str), "-");
#endif
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        PRT_CELL_SET("%s(%d)", SOC_PORT_NAME(unit, port), port);
        PRT_CELL_SET("%d", phy_measure.lane);
        PRT_CELL_SET("%d.%03d", phy_measure.serdes_freq_int, phy_measure.serdes_freq_remainder);
        PRT_CELL_SET("%s", pll_str);
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Sequence used to force the link training on the port
 * assuming that if the user used the command he is aware that the
 * phy lane config port resource might be changed.
 */
static shr_error_e
diag_dnxc_phy_cl72_force_link_training(
    int unit,
    bcm_port_t port,
    bcm_port_resource_t * resource,
    int enable)
{
    int lane_rate = 0;
#ifdef BCM_DNX_SUPPORT
    int num_lane = 0;
    bcm_pbmp_t phys;
    dnx_algo_port_info_s port_info;
#endif

    SHR_FUNC_INIT_VARS(unit);

    if (!SAL_BOOT_PLISIM)
    {
        /*
         *  calculate lane rate
         */
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, resource->port, &port_info));
            if (DNX_ALGO_PORT_TYPE_IS_NIF_ILKN(unit, port_info, 1, 1) || DNX_ALGO_PORT_TYPE_IS_FABRIC(unit, port_info))
            {
                lane_rate = resource->speed;
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_nif_phys_get(unit, port, 0, &phys));
                BCM_PBMP_COUNT(phys, num_lane);
                lane_rate = (resource->speed / num_lane);
            }
        }
#endif
#ifdef BCM_DNXF_SUPPORT
        if (SOC_IS_DNXF(unit))
        {
            lane_rate = resource->speed;
        }
#endif

        resource->link_training = enable;
        if (enable)
        {
            /**
             * Enable LinkTraining
             */
            if (!(BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_GET(resource->phy_lane_config)))
            {
                BCM_PORT_RESOURCE_PHY_LANE_CONFIG_DFE_SET(resource->phy_lane_config);
                cli_out("WARNING! DFE was enabled on port %d so we can enable CL72 \n", port);
            }
        }
        else
        {
            /**
             * Disable LinkTraining on PAM4 port
             */
            if (DNXC_PORT_PHY_SPEED_IS_PAM4(lane_rate))
            {
                if (!(BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_GET(resource->phy_lane_config))
                    && !(BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_ES_GET(resource->phy_lane_config)))
                {
                    BCM_PORT_RESOURCE_PHY_LANE_CONFIG_FORCE_NS_SET(resource->phy_lane_config);
                    cli_out("WARNING! NS was enabled on port %d with PAM4 modulation so we can disable CL72 \n", port);
                }
            }
        }

        SHR_IF_ERR_EXIT(bcm_port_resource_set(unit, port, resource));
    }
exit:
    SHR_FUNC_EXIT;
}
/**
 * \brief - CMD for enabling or disabling CL72 or dumping CL72 status
 */
static shr_error_e
cmd_dnxc_phy_cl72(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t pbmp;
    bcm_port_t port;
    diag_dnxc_phy_cl72_option_e option;
    bcm_port_resource_t resource;
    uint32 cl72_status;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("pbmp", pbmp);
    SH_SAND_GET_ENUM("mode", option);

    /*
     * Dump CL72 status
     */
    if (option == DIAG_DNXC_PHY_CL72_OPTION_STATE)
    {
        PRT_TITLE_SET("CL72 INFO:");

        PRT_COLUMN_ADD("Port");
        PRT_COLUMN_ADD("CL72 Enable");
        PRT_COLUMN_ADD("CL72 Status");
    }

    SHR_IF_ERR_EXIT(dnxc_phy_diag_pbmp_check(unit, &pbmp));
    BCM_PBMP_ITER(pbmp, port)
    {
        SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, port, &resource));

        if (option == DIAG_DNXC_PHY_CL72_OPTION_STATE)
        {
            SHR_IF_ERR_EXIT(bcm_port_phy_control_get(unit, port, BCM_PORT_PHY_CONTROL_CL72_STATUS, &cl72_status));

            PRT_ROW_ADD(PRT_ROW_SEP_NONE);

            PRT_CELL_SET("%s(%d)", SOC_PORT_NAME(unit, port), port);
            PRT_CELL_SET("%s", resource.link_training ? "Enabled" : "Disabled");
            PRT_CELL_SET("%s", cl72_status ? "Locked" : "Unlocked");
        }
        else if (option == DIAG_DNXC_PHY_CL72_OPTION_ON)
        {
            /*
             * turn on cl72
             */
            if (resource.link_training == 0)
            {
                SHR_IF_ERR_EXIT(diag_dnxc_phy_cl72_force_link_training(unit, port, &resource, 1));
            }
        }
        else
        {
            /*
             * turn off cl72
             */
            if (resource.link_training == 1)
            {
                SHR_IF_ERR_EXIT(diag_dnxc_phy_cl72_force_link_training(unit, port, &resource, 0));
            }
        }
    }
    if (option == DIAG_DNXC_PHY_CL72_OPTION_STATE)
    {
        PRT_COMMITX;
    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_dnxc_phy_raw_c45(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 phyid, regad, devad;
    uint16 rd_data, wr_data;
    int is_write = 0, clause = 45;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("phy_id", phyid);
    SH_SAND_GET_UINT32("dev_addr", devad);
    SH_SAND_GET_UINT32("reg_addr", regad);
    SH_SAND_GET_UINT32("data", wr_data);
    SH_SAND_IS_PRESENT("data", is_write);

    /*
     * On adapter, CMICx operation will return TIMEOUT
     */
    if (!SAL_BOOT_PLISIM)
    {
        if (is_write)
        {
            SHR_IF_ERR_EXIT(soc_dcmn_miim_write(unit, clause, phyid, SOC_PHY_CLAUSE45_ADDR(devad, regad), wr_data));
        }
        else
        {
            SHR_IF_ERR_EXIT(soc_dcmn_miim_read(unit, clause, phyid, SOC_PHY_CLAUSE45_ADDR(devad, regad), &rd_data));
        }
        if (!is_write)
        {
            PRT_TITLE_SET("PHY RAW READ");

            PRT_COLUMN_ADD("PHY ID");
            PRT_COLUMN_ADD("Value");

            PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

            PRT_CELL_SET("0x%x", phyid);
            PRT_CELL_SET("0x%x", rd_data);

            PRT_COMMITX;
        }
    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

#ifdef BCM_DNX_SUPPORT
static shr_error_e
cmd_dnxc_phy_raw_sbus(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    uint32 pm_id, regad, lane, pll_index;
    uint32 devad, rd_data, wr_data;
    int is_write = 0, nof_phys;
    portmod_access_get_params_t params;
    phymod_phy_access_t phy_access;
    portmod_dispatch_type_t pm_type;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_UINT32("pm_id", pm_id);
    SH_SAND_GET_UINT32("dev_addr", devad);
    SH_SAND_GET_UINT32("reg_addr", regad);
    SH_SAND_GET_INT32("lane", lane);
    SH_SAND_GET_UINT32("pll_index", pll_index);
    SH_SAND_GET_UINT32("data", wr_data);
    SH_SAND_IS_PRESENT("data", is_write);

    SHR_IF_ERR_EXIT(portmod_pm_id_pm_type_get(unit, pm_id, &pm_type));
    if (pm_type != portmodDispatchTypePm8x50)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL, "PM Type is not supported! \n");
    }

    SHR_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
    params.lane = lane;

    SHR_IF_ERR_EXIT(portmod_pm_phy_lane_access_get(unit, pm_id, &params, 1, &phy_access, &nof_phys, NULL));
    phy_access.access.pll_idx = pll_index;

    if (is_write)
    {
        SHR_IF_ERR_EXIT(phymod_phy_reg_write(&phy_access, SOC_PHY_CLAUSE45_ADDR(devad, regad), wr_data));
    }
    else
    {
        SHR_IF_ERR_EXIT(phymod_phy_reg_read(&phy_access, SOC_PHY_CLAUSE45_ADDR(devad, regad), &rd_data));
    }
    if (!is_write)
    {
        PRT_TITLE_SET("PHY RAW READ");

        PRT_COLUMN_ADD("PM ID");
        PRT_COLUMN_ADD("Value");

        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);

        PRT_CELL_SET("0x%x", pm_id);
        PRT_CELL_SET("0x%x", rd_data);

        PRT_COMMITX;
    }
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;

}
#endif

/**
 * \brief - CMD for dumping the PHY DSC
 */
static shr_error_e
cmd_dnxc_phy_dsc(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t pbmp;
    bcm_port_t port;
    int is_state = 0, is_config = 0;
    char *option_str;
    int interval = 0;
    int sim_skip = 0;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("pbmp", pbmp);
    SH_SAND_GET_BOOL("state", is_state);
    SH_SAND_GET_BOOL("config", is_config);

    if (is_state && is_config)
    {
         /* coverity[copy_paste_error : FALSE]  */
        SHR_CLI_EXIT(_SHR_E_PARAM, "Option \"state\" and \"config\" cannot be specified simultaneously! \n");
    }
    else if (is_state)
    {
        /**
        * By default print dsc state together with an eyescan
        */
        option_str = "state_eye";
        sim_skip = 1;
    }
    else if (is_config)
    {
        option_str = "config";
    }
    else
    {
        option_str = NULL;
    }

    SHR_IF_ERR_EXIT(dnxc_phy_diag_pbmp_check(unit, &pbmp));
    /*
     * Close linkscan
     */
    SHR_IF_ERR_EXIT(dnxc_phy_diag_linkscan_off(unit, &interval));

    /*
     * On adapter, don't run 'phy dsc xxx state_eye' 
     */
    if (!SAL_BOOT_PLISIM || !sim_skip)
    {
        BCM_PBMP_ITER(pbmp, port)
        {
            SHR_IF_ERR_EXIT(portmod_port_diag_ctrl
                            (unit, port, PHY_DIAG_INT, PHY_DIAG_CTRL_CMD, PHY_DIAG_CTRL_DSC, option_str));
            /*
             * Since this diag is not informative when running on simulation,
             * and it takes a long time to print, then print information only
             * for 1 port (mainly to verify that this diag is not crashing).
             */
            if (SAL_BOOT_PLISIM)
            {
                SHR_CLI_EXIT(_SHR_E_NONE, "Running on simulation - printing only 1 port info (port %d)\n", port);
            }
        }
    }

exit:
    if ((_func_rv = dnxc_phy_diag_linkscan_recover(unit, interval)) < 0)
    {
        cli_out("Failed to recover linkscan, orginial linkscan interval is %d \n", interval);
    }
    SHR_FUNC_EXIT;
}

/**
 * \brief - CMD for printing register matching the
 *    specified pattern/address
 */
static shr_error_e
cmd_dnxc_phy_list(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t pbmp;
    char *patter_str = NULL;
    char new_cmd[DIAG_DNXC_PORT_NEW_CMD_LEN];
    args_t *new_args = NULL;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("pbmp", pbmp);
    SH_SAND_GET_STR("pattern", patter_str);

    SHR_IF_ERR_EXIT(dnxc_phy_diag_pbmp_check(unit, &pbmp));
    /*
     * Format New command
     */
    sal_snprintf(new_cmd, DIAG_DNXC_PORT_NEW_CMD_LEN, "%s %s", "list", patter_str);

    SHR_ALLOC(new_args, sizeof(args_t), "new args", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_IF_ERR_EXIT(diag_parse_args(new_cmd, NULL, new_args));
    SHR_IF_ERR_EXIT(dnxc_phy_portmod_sym_access(unit, new_args, &pbmp));
exit:SHR_FREE(new_args);
    SHR_FUNC_EXIT;
}

/**
 * \brief - CMD for setting PHY registers according to
 *    the given PBMP and register info
 */
static shr_error_e
cmd_dnxc_phy_set(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int fld_is_present, lane, pll_index;
    uint32 data;
    bcm_pbmp_t pbmp;
    args_t *new_args = NULL;
    char *reg_str = NULL, *fld_str = NULL;
    char new_cmd[DIAG_DNXC_PORT_NEW_CMD_LEN];

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("pbmp", pbmp);
    SHR_IF_ERR_EXIT(dnxc_phy_diag_pbmp_check(unit, &pbmp));

    SH_SAND_GET_STR("register", reg_str);
    SH_SAND_GET_INT32("lane", lane);
    SH_SAND_GET_INT32("pll_index", pll_index);
    SH_SAND_GET_UINT32("data", data);

    SH_SAND_IS_PRESENT("field", fld_is_present);
    if (fld_is_present)
    {
        SH_SAND_GET_STR("field", fld_str);
    }
    /*
     * Format New command
     */

    if (fld_is_present)
    {
        sal_snprintf(new_cmd, DIAG_DNXC_PORT_NEW_CMD_LEN, "%s.%d.%d %s=%d", reg_str, lane, pll_index, fld_str, data);
    }
    else
    {
        sal_snprintf(new_cmd, DIAG_DNXC_PORT_NEW_CMD_LEN, "%s.%d.%d %d", reg_str, lane, pll_index, data);
    }
    SHR_ALLOC(new_args, sizeof(args_t), "new args", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
    SHR_IF_ERR_EXIT(diag_parse_args(new_cmd, NULL, new_args));
    /*
     * The portmod layer will handle the command
     */
    SHR_IF_ERR_EXIT(dnxc_phy_portmod_sym_access(unit, new_args, &pbmp));

exit:
    SHR_FREE(new_args);
    SHR_FUNC_EXIT;
}

/**
 * \brief - CMD fro getting the PHY register value
 *    accroding to the given PBMP and register info
 */
static shr_error_e
cmd_dnxc_phy_get(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int nz, lane, pll_index;
    bcm_pbmp_t pbmp;
    args_t *new_args = NULL;
    char *pattern_str = NULL;
    char new_cmd[DIAG_DNXC_PORT_NEW_CMD_LEN];

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("pbmp", pbmp);
    SH_SAND_GET_STR("pattern", pattern_str);
    SH_SAND_GET_BOOL("nonzero", nz);
    SH_SAND_GET_INT32("lane", lane);
    SH_SAND_GET_INT32("pll_index", pll_index);
    SHR_IF_ERR_EXIT(dnxc_phy_diag_pbmp_check(unit, &pbmp));

    if (nz)
    {
        ARG_NEXT(args);
        SHR_IF_ERR_EXIT(dnxc_phy_portmod_sym_access(unit, args, &pbmp));
    }
    else
    {
        SHR_ALLOC(new_args, sizeof(args_t), "new args", "%s%s%s\r\n", EMPTY, EMPTY, EMPTY);
        sal_snprintf(new_cmd, DIAG_DNXC_PORT_NEW_CMD_LEN, "%s.%d.%d", pattern_str, lane, pll_index);
        SHR_IF_ERR_EXIT(diag_parse_args(new_cmd, NULL, new_args));
        /*
         * The portmod layer will handle the command
         */
        SHR_IF_ERR_EXIT(dnxc_phy_portmod_sym_access(unit, new_args, &pbmp));
    }
exit:
    SHR_FREE(new_args);
    SHR_FUNC_EXIT;
}

/**
 * \brief - CMD for dumping PHY eyescan
 */
static shr_error_e
cmd_dnxc_phy_eyescan(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t pbmp;
    bcm_port_t port;
    int type, interval = 0;
    int lane_id, lane_is_present, lane_num;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("pbmp", pbmp);
    SH_SAND_GET_INT32("type", type);
    SHR_IF_ERR_EXIT(dnxc_phy_diag_pbmp_check(unit, &pbmp));

    if (!(type == DIAG_DNXC_PHY_EYESCAN_TYPE1 || type == DIAG_DNXC_PHY_EYESCAN_TYPE4))
    {
        SHR_CLI_EXIT(_SHR_E_PARAM, "Only type 1 and type 4 is supported! \n");
    }
    /*
     * Verify if the lane ID is legal
     */
    SH_SAND_IS_PRESENT("lane", lane_is_present);
    if (lane_is_present)
    {
        SH_SAND_GET_INT32("lane", lane_id);
        BCM_PBMP_ITER(pbmp, port)
        {
            lane_num = SOC_INFO(unit).port_num_lanes[port];
            /** Verify lane boundary */
            SHR_RANGE_VERIFY(lane_id, 0, lane_num - 1, _SHR_E_PARAM,
                             "Lane ID is illegal, port %d only has %d lanes! \n", port, lane_num);
        }
    }

    if (type == DIAG_DNXC_PHY_EYESCAN_TYPE1)
    {
        SHR_IF_ERR_EXIT(dnxc_phy_diag_option_present_check(unit, sand_control, "ber_scan_mode", "ber_scan_mode", 0));
        SHR_IF_ERR_EXIT(dnxc_phy_diag_option_present_check
                        (unit, sand_control, "max_err_control", "max_err_control", 0));
        SHR_IF_ERR_EXIT(dnxc_phy_diag_option_present_check(unit, sand_control, "timer_control", "timer_control", 0));
    }
    SHR_IF_ERR_EXIT(dnxc_phy_diag_linkscan_off(unit, &interval));
    /*
     * Skip the <pbmp> option
     */
    ARG_NEXT(args);
    if (SHR_FAILURE(_phy_diag_eyescan(unit, pbmp, args)))
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error in _phy_diag_eyescan! \n");
    }
exit:
    if ((_func_rv = dnxc_phy_diag_linkscan_recover(unit, interval)) < 0)
    {
        cli_out("Failed to recover linkscan, orginial linkscan interval is %d \n", interval);
    }
    SHR_FUNC_EXIT;
}

#ifdef SERDES_API_FLOATING_POINT
/**
 * \brief - CMD for dumping PHY BER projection
 */
static shr_error_e
cmd_dnxc_phy_ber_proj(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t pbmp;
    int interval = 0;
    int rv;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("pbmp", pbmp);

    SHR_IF_ERR_EXIT(dnxc_phy_diag_pbmp_check(unit, &pbmp));
    SHR_IF_ERR_EXIT(dnxc_phy_diag_linkscan_off(unit, &interval));
    /*
     * Skip the <pbmp> option
     */
    ARG_NEXT(args);

    rv = _phy_diag_berproj(unit, pbmp, args);
    if (rv == CMD_FAIL)
    {
        /*
         * PHY berproj is not supported for PM4x25.
         * When _phy_diag_berproj is called for a port on PM4x25
         * the following errors are issued:
         *
         *   src/soc/portmod/pms/portmod_phy_control.c[1845]portmod_pm_phy_ber_proj unit <nounit>:Feature unavailable
         *   src/soc/portmod/portmod_dispatch.c[6489]portmod_port_diag_ctrl unit <nounit>:Feature unavailable
         *
         * Catch this error type to print clear message
         */

        SHR_CLI_EXIT(_SHR_E_UNAVAIL, "phy berproj is not supported for PM used by the port\n");
    }
    else if (SHR_FAILURE(rv))
    {
        SHR_CLI_EXIT(_SHR_E_INTERNAL, "Error in _phy_diag_berproj! \n");
    }
exit:
    if ((_func_rv = dnxc_phy_diag_linkscan_recover(unit, interval)) < 0)
    {
        cli_out("Failed to recover linkscan, orginial linkscan interval is %d \n", interval);
    }
    SHR_FUNC_EXIT;
}
#endif

#ifdef PHYMOD_LINKCAT_SUPPORT
/**
 * \brief - CMD for PHY LinkCAT
 */
static shr_error_e
cmd_dnxc_phy_linkcat(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int lane_id, lane_is_present, nof_lanes;
    int nof_cores, interval = 0;
    bcm_pbmp_t logical_ports;
    bcm_port_t port;
    portmod_access_get_params_t params;
    enum srds_linkcat_mode_enum linkcat_mode;
    phymod_phy_access_t phy_access[DIAG_DNXC_MAX_CORE_ACCESS_STRUCTURES_PER_PORT];
#ifdef PHYMOD_NEW_LINKCAT
    char *prefix_str = NULL;
    linkcat_config linkcat_config;
    linkcat_returns linkcat_status;
#endif

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("pbmp", logical_ports);
    SHR_IF_ERR_EXIT(dnxc_phy_diag_pbmp_check(unit, &logical_ports));

    SH_SAND_GET_INT32("lane", lane_id);
    SH_SAND_IS_PRESENT("lane", lane_is_present);

    SH_SAND_GET_ENUM("mode", linkcat_mode);

#ifdef PHYMOD_NEW_LINKCAT
    SH_SAND_GET_STR("prefix", prefix_str);
#endif

    SHR_IF_ERR_EXIT(dnxc_phy_diag_linkscan_off(unit, &interval));

    BCM_PBMP_ITER(logical_ports, port)
    {
        nof_lanes = SOC_INFO(unit).port_num_lanes[port];
        if (lane_is_present)
        {
            /** Verify lane boundary */
            SHR_RANGE_VERIFY(lane_id, 0, nof_lanes - 1, _SHR_E_PARAM,
                             "Lane ID is illegal, port %d only has %d lanes! \n", port, nof_lanes);
        }
        else if (!lane_is_present && (nof_lanes > 1))
        {
            SHR_CLI_EXIT(_SHR_E_PARAM, "Option lane is mandatory for multi-lane port %d! \n", port);
        }
        /*
         * Get PHY access
         */
        SHR_IF_ERR_EXIT(portmod_access_get_params_t_init(unit, &params));
        params.phyn = 0;
        params.lane = lane_id;
        SHR_IF_ERR_EXIT(portmod_port_phy_lane_access_get
                        (unit, port, &params, DIAG_DNXC_MAX_CORE_ACCESS_STRUCTURES_PER_PORT, phy_access, &nof_cores,
                         NULL));

#ifdef PHYMOD_NEW_LINKCAT
        linkcat_status.insertion_loss = 0.0;
        linkcat_status.fit_factor = 0.0;
        linkcat_config.mode = linkcat_mode;
        linkcat_config.portaddress = phy_access[0].access.addr;
        linkcat_config.dir_name = NULL;
        linkcat_config.file_prefix = prefix_str;
#endif /* PHYMOD_NEW_LINKCAT */

        cli_out("Running linkCAT on port=%d addr=0x%x lane_mask=0x%x mode=%d\n", port,
                phy_access[0].access.addr, phy_access[0].access.lane_mask, linkcat_mode);

        switch (phy_access[0].type)
        {
            case phymodDispatchTypeBlackhawk:
            case phymodDispatchTypeTscbh:
#if !defined(PHYMOD_LINKCAT_BLACKHAWK7_V1L8P1_SUPPORT) && !defined(PHYMOD_LINKCAT_BLACKHAWK7_V1L8P2_SUPPORT)
#ifdef PHYMOD_NEW_LINKCAT
                SHR_IF_ERR_EXIT(blackhawk_tsc_LinkCat(&phy_access[0].access, &linkcat_config, &linkcat_status));
#else
                SHR_IF_ERR_EXIT(blackhawk_tsc_LinkCat(&phy_access[0].access, linkcat_mode, phy_access[0].access.addr));
#endif /* PHYMOD_NEW_LINKCAT */
#endif
                break;
            case phymodDispatchTypeBlackhawk7_v1l8p1:
#if defined(PHYMOD_LINKCAT_BLACKHAWK7_V1L8P1_SUPPORT)
                SHR_IF_ERR_EXIT(blackhawk7_v1l8p1_LinkCat(&phy_access[0].access, &linkcat_config, &linkcat_status));
#else
                SHR_CLI_EXIT(_SHR_E_UNAVAIL, "LinkCAT lib is not compiled for port %d! \n", port);
#endif
                break;
            case phymodDispatchTypeTscbh_gen2:
#if defined(PHYMOD_LINKCAT_BLACKHAWK7_V1L8P2_SUPPORT)
                SHR_IF_ERR_EXIT(blackhawk7_v1l8p2_LinkCat(&phy_access[0].access, &linkcat_config, &linkcat_status));
#else
                SHR_CLI_EXIT(_SHR_E_UNAVAIL, "LinkCAT lib is not compiled for port %d! \n", port);
#endif
                break;
            default:
                SHR_CLI_EXIT(_SHR_E_UNAVAIL, "The PM type of port %d does not support LinkCAT! \n", port);
        }
    }
exit:
    if ((_func_rv = dnxc_phy_diag_linkscan_recover(unit, interval)) < 0)
    {
        cli_out("Failed to recover linkscan, orginial linkscan interval is %d \n", interval);
    }
    SHR_FUNC_EXIT;
}
#endif

/**
 * \brief - Get fecstat counter rate
 */
static shr_error_e
dnxc_phy_diag_fecstat_counter_rate_get(
    int unit,
    bcm_port_t port,
    bcm_stat_val_t counter_type,
    uint32 *rate)
{
    uint32 counters[DIAG_DNXC_MAX_COUNTERS_PER_COUNTER_TYPE] = { 0 };
    int array_size;
#ifdef BCM_DNX_SUPPORT
    int sub_array_size;
    uint32 sub_counters[DIAG_DNXC_MAX_COUNTERS_PER_COUNTER_TYPE];
#endif
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX(unit))
    {
#ifdef BCM_DNX_SUPPORT

        SHR_IF_ERR_EXIT(dnx_mib_mapping_stat_get(unit, port, counter_type, DIAG_DNXC_MAX_COUNTERS_PER_COUNTER_TYPE,
                                                 counters, &array_size, sub_counters, &sub_array_size));
#endif
    }
    else
    {
#ifdef BCM_DNXF_SUPPORT
        SHR_IF_ERR_EXIT(MBCM_DNXF_DRIVER_CALL
                        (unit, mbcm_dnxf_mapping_stat_get,
                         (unit, port, (int *) counters, &array_size, counter_type, 1)));
#endif
    }
    SHR_IF_ERR_EXIT(soc_counter_get32_rate(unit, port, counters[0], 0, rate));
exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - CMD to dump PHY fecstat counter
 */
static shr_error_e
cmd_dnxc_phy_fecstat_counter(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int i;
    bcm_pbmp_t logical_ports;
    bcm_port_t port;
    uint32 rate;
    uint64 counter_val, counter_diff, rate64;
    char buf[DIAG_DNXC_PORT_MAX_STR_LEN];
    bcm_stat_val_t counter_type[DIAG_DNXC_PHY_FECSTAT_NOF_COUNTERS] =
        { snmpBcmRxFecCorrectable, snmpBcmRxFecUncorrectable };
    char *counter_name[DIAG_DNXC_PHY_FECSTAT_NOF_COUNTERS] = { "FEC Correctable Counter", "FEC Uncorrectable Counter" };

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_CONTROL(unit)->counter_interval == 0)
    {
        SHR_CLI_EXIT(_SHR_E_NONE, "This command is only available when counter thread is enabled! \n");
    }

    PRT_TITLE_SET("FEC stat counters");

    PRT_COLUMN_ADD("Port");
    PRT_COLUMN_ADD("Counter Name");
    PRT_COLUMN_ADD("Accumulated Count ");
    PRT_COLUMN_ADD("Last Count ");
    PRT_COLUMN_ADD("Count Per Sec");

    SH_SAND_GET_PORT("pbmp", logical_ports);
    /*
     * Filter illegal ports
     */
    BCM_PBMP_AND(logical_ports, PBMP_PORT_ALL(unit));
    BCM_PBMP_ITER(logical_ports, port)
    {
        for (i = 0; i < DIAG_DNXC_PHY_FECSTAT_NOF_COUNTERS; ++i)
        {
            uint64 tmp_64;

            if (i % DIAG_DNXC_PHY_FECSTAT_NOF_COUNTERS != 0)
            {
                PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
                PRT_CELL_SET("%s", "");
            }
            else
            {
                PRT_ROW_ADD(PRT_ROW_SEP_NONE);
                PRT_CELL_SET("%s", BCM_PORT_NAME(unit, port));
            }
            PRT_CELL_SET("%s", counter_name[i]);

            SHR_IF_ERR_EXIT(bcm_stat_get(unit, port, counter_type[i], &counter_val));
            COMPILER_64_ZERO(tmp_64);
            if (COMPILER_64_EQ(counter_val, tmp_64))
            {
                PRT_CELL_SET("%s", "N/A");
                PRT_CELL_SET("%s", "N/A");
                PRT_CELL_SET("%s", "N/A");
            }
            else
            {
                COMPILER_64_COPY(counter_diff, counter_val);
                COMPILER_64_SUB_64(counter_diff, phy_fecstat_val[unit][port][i]);

                SHR_IF_ERR_EXIT(dnxc_phy_diag_fecstat_counter_rate_get(unit, port, counter_type[i], &rate));

                sal_memset(buf, 0, sizeof(buf));
                format_uint64_decimal(buf, counter_val, ',');
                PRT_CELL_SET("%s", buf);

                sal_memset(buf, 0, sizeof(buf));
                format_uint64_decimal(buf, counter_diff, ',');
                PRT_CELL_SET("%s", buf);

                sal_memset(buf, 0, sizeof(buf));
                COMPILER_64_SET(rate64, 0, rate);
                format_uint64_decimal(buf, rate64, ',');
                PRT_CELL_SET("%s%s", buf, "/s");

                COMPILER_64_COPY(phy_fecstat_val[unit][port][i], counter_val);
            }
        }
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - CMD to dump pre-FEC BER
 */
static shr_error_e
cmd_dnxc_phy_fecstat_ber(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t logical_ports;
    bcm_port_t port;
    int pow = 0;
    uint32 ber_int = 0, ber_remainder = 0;
    uint32 serdes_rate, rate;
    uint64 total_serdes_rate;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_CONTROL(unit)->counter_interval == 0)
    {
        SHR_CLI_EXIT(_SHR_E_NONE, "This command is only available when counter thread is enabled! \n");
    }

    PRT_TITLE_SET("FEC stat BER");

    PRT_COLUMN_ADD("Port");
    PRT_COLUMN_ADD("BER");

    SH_SAND_GET_PORT("pbmp", logical_ports);
    /*
     * Filter illegal ports
     */
    BCM_PBMP_AND(logical_ports, PBMP_PORT_ALL(unit));
    BCM_PBMP_ITER(logical_ports, port)
    {
        PRT_ROW_ADD(PRT_ROW_SEP_UNDERSCORE);
        PRT_CELL_SET("%s", BCM_PORT_NAME(unit, port));

        SHR_IF_ERR_EXIT(dnxc_phy_diag_fecstat_counter_rate_get(unit, port, snmpBcmRxFecCorrectable, &rate));
        if (rate == 0)
        {
            PRT_CELL_SET("%s", "N/A");
        }
        else
        {
            SHR_IF_ERR_EXIT(dnxc_phy_diag_serdes_rate_get(unit, port, &serdes_rate));

            COMPILER_64_SET(total_serdes_rate, 0, serdes_rate * SOC_INFO(unit).port_num_lanes[port]);
            COMPILER_64_UMUL_32(total_serdes_rate, 1000000);
            SHR_IF_ERR_EXIT(dnxc_diag_format_big_integer_div
                            (unit, total_serdes_rate, (int) rate, &ber_int, &ber_remainder, &pow));

            PRT_CELL_SET("%d.%02de-%02d", ber_int, ber_remainder, pow);
        }
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Convert fec type to string
 *
 * \param [in] unit - chip unit id
 * \param [in] fec_type - fec type
 *
 * \return
 *   string
 *
 * \remarks
 *   * None
 * \see
 *   * None
 */
static const char *
dnxc_diag_fec_type_to_string(
    int unit,
    bcm_port_phy_fec_t fec_type)
{
    const char *str = "";

    switch (fec_type)
    {
        case (bcmPortPhyFecNone):
            str = "NONE";
            break;
        case (bcmPortPhyFecBaseR):
            str = "BASE-R";
            break;
        case (bcmPortPhyFecRsFec):
            str = "RS-528";
            break;
        case (bcmPortPhyFecRs544):
            str = "RS-544-1xN";
            break;
        case (bcmPortPhyFecRs272):
            str = "RS-272-1xN";
            break;
        case (bcmPortPhyFecRs206):
            str = "RS-206";
            break;
        case (bcmPortPhyFecRs108):
            str = "RS-108";
            break;
        case (bcmPortPhyFecRs545):
            str = "RS-545";
            break;
        case (bcmPortPhyFecRs304):
            str = "RS-304";
            break;
        case (bcmPortPhyFecRs544_2xN):
            str = "RS-544-2xN";
            break;
        case (bcmPortPhyFecRs272_2xN):
            str = "RS-272-2xN";
            break;
        default:
            str = "NONE";
            break;
    }

    return str;
}

/**
 * \brief - Dump the port status info
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
cmd_dnxc_port_status(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t logical_ports;
    bcm_port_t port;
    bcm_port_info_t port_info;
    char speed_str[DIAG_DNXC_PORT_MAX_STR_LEN];
#ifdef BCM_DNX_SUPPORT
    dnx_algo_port_info_s port_mgm_info;
#endif
    bcm_port_resource_t resource;

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("pbmp", logical_ports);

    /*
     * Print table header
     */
    PRT_TITLE_SET("Port status");

    PRT_COLUMN_ADD("Port");
    PRT_COLUMN_ADD("Ena/Link");
    PRT_COLUMN_ADD("Speed");
    PRT_COLUMN_ADD("Linkscan");
    PRT_COLUMN_ADD("Autoneg");
    PRT_COLUMN_ADD("FEC");
    PRT_COLUMN_ADD("Loopback");

    BCM_PBMP_ITER(logical_ports, port)
    {
#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            SHR_IF_ERR_EXIT(dnx_algo_port_info_get(unit, port, &port_mgm_info));
            if (!DNX_ALGO_PORT_TYPE_IS_IMB(unit, port_mgm_info, FALSE))
            {
                /*
                 * Not relevant for the other port types.
                 */
                continue;
            }
        }
#endif
        port_info_init(unit, port, &port_info, DIAG_DNXC_PORT_STATUS_ACTION_MASK);

#ifdef BCM_DNX_SUPPORT
        if (SOC_IS_DNX(unit))
        {
            SHR_IF_ERR_EXIT(dnx_port_selective_get(unit, port, &port_info));
        }
        else
#endif
        {
            SHR_IF_ERR_EXIT(bcm_port_selective_get(unit, port, &port_info));
        }

        bcm_port_resource_t_init(&resource);
        SHR_IF_ERR_EXIT(bcm_port_resource_get(unit, port, &resource));

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);

        /*
         * Print port name and status
         */
        PRT_CELL_SET("%s(%d)", BCM_PORT_NAME(unit, port), port);
        PRT_CELL_SET("%s",
                     !port_info.enable ? "!ena" : (port_info.linkstatus ==
                                                   BCM_PORT_LINK_STATUS_FAILED) ? "fail" : (port_info.linkstatus ==
                                                                                            BCM_PORT_LINK_STATUS_UP ?
                                                                                            "up" : "down"));

        /*
         * Format speed string and print
         */
        sal_memset(speed_str, 0, sizeof(speed_str));

        PRT_CELL_SET("%4s",
                     (port_info.action_mask & BCM_PORT_ATTR_SPEED_MASK) ? if_fmt_speed(speed_str,
                                                                                       port_info.speed) : "");
        /*
         * Print linkscan, AN
         */
        DIAG_DNXC_PORT_STATUS_STRING_CHECK_PRINT(BCM_PORT_ATTR_LINKSCAN_MASK, port_info.action_mask, "%s",
                                                 LINKSCAN_MODE(port_info.linkscan));
        DIAG_DNXC_PORT_STATUS_STRING_CHECK_PRINT(BCM_PORT_ATTR_AUTONEG_MASK, port_info.action_mask, "%s",
                                                 port_info.autoneg ? "Yes" : "No");
        /*
         * Print FEC string
         */
        PRT_CELL_SET("%s", dnxc_diag_fec_type_to_string(unit, resource.fec_type));
        /*
         * Print Loopback string
         */
        DIAG_DNXC_PORT_STATUS_STRING_CHECK_PRINT(BCM_PORT_ATTR_LOOPBACK_MASK, port_info.action_mask, "%s",
                                                 LOOPBACK_MODE(port_info.loopback));

    }
    PRT_COMMITX;

exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

/**
 * \brief - Set port loopback
 */
shr_error_e
cmd_dnxc_port_loopback(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    bcm_pbmp_t pbmp;
    bcm_port_t port;
    int mode;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("pbmp", pbmp);
    SH_SAND_GET_ENUM("mode", mode);
    if (!SAL_BOOT_PLISIM)
    {
        BCM_PBMP_ITER(pbmp, port)
        {
            SHR_IF_ERR_EXIT(bcm_port_loopback_set(unit, port, mode));
        }
    }

exit:SHR_FUNC_EXIT;
}

/**
 * \brief - CMD to enable/disable port
 */
shr_error_e
cmd_dnxc_port_enable(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int enable;
    bcm_port_t port;
    bcm_pbmp_t pbmp;

    SHR_FUNC_INIT_VARS(unit);

    SH_SAND_GET_PORT("pbmp", pbmp);
    SH_SAND_GET_BOOL("enable", enable);

    BCM_PBMP_ITER(pbmp, port)
    {
        SHR_IF_ERR_EXIT(bcm_port_enable_set(unit, port, enable));
    }
exit:SHR_FUNC_EXIT;
}

/**
 * \brief - Change the PM type enum to string.
 *    For example:
 *        INPUT: portmodDispatchTypePm8x50
 *        OUTPUT: Pm8x50
 */
static char *
diag_dnxc_pm_enum_value_to_str(
    enum_mapping_t * mapping,
    int enum_val,
    char *type_str)
{
    char *key;
    int i = 0;

    key = mapping[0].key;
    while (key != NULL)
    {
        if (enum_val == mapping[i].enum_value)
        {
            /*
             * Found the PM type string
             */
            type_str = mapping[i].key;
            return type_str;
        }
        i++;
        key = mapping[i].key;
    }
    type_str = "Unknown";
    return type_str;
}

/**
 * \brief - Dump the port portmacro info
 */

shr_error_e
cmd_dnxc_port_pm_info(
    int unit,
    args_t * args,
    sh_sand_control_t * sand_control)
{
    int pm_id, pm_id_present = 0, phy;
    int pm_id_start, pm_id_end, pm, max_nof_pms = 0;
    char *pm_type_str = NULL;
    char *ports_str = NULL;
    char buf[FORMAT_PBMP_MAX];
    portmod_pm_diag_info_t diag_info;
    bcm_pbmp_t phy_pbmp;
#ifdef BCM_DNX_SUPPORT
    int nof_ethu_pms, nof_ilkn_units, nof_fabric_pms;
#endif

    PRT_INIT_VARS;
    SHR_FUNC_INIT_VARS(unit);

    /*
     * Print table header
     */
    PRT_TITLE_SET("PortMacro INFO");

    PRT_COLUMN_ADD("PM ID");
    PRT_COLUMN_ADD("PM Type");
    PRT_COLUMN_ADD("PM PHYS");
    PRT_COLUMN_ADD("Attached Ports");

#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit))
    {
        nof_ethu_pms = dnx_data_nif.eth.total_nof_ethu_pms_in_device_get(unit);
        nof_ilkn_units = dnx_data_nif.ilkn.ilkn_unit_nof_get(unit);
        nof_fabric_pms = dnx_data_fabric.blocks.nof_pms_get(unit);
        max_nof_pms = nof_ethu_pms + nof_ilkn_units + nof_fabric_pms;
    }
    else
#endif
    {
#ifdef BCM_DNXF_SUPPORT
        max_nof_pms = dnxf_data_device.blocks.nof_instances_fsrd_get(unit);
#endif
    }

    SH_SAND_IS_PRESENT("pm_id", pm_id_present);

    if (pm_id_present)
    {
        SH_SAND_GET_INT32("pm_id", pm_id);

        SHR_RANGE_VERIFY(pm_id, 0, max_nof_pms - 1, _SHR_E_PARAM, "PM ID is out of bound.\n");
        pm_id_start = pm_id;
        pm_id_end = pm_id;
    }
    else
    {
        pm_id_start = 0;
        pm_id_end = max_nof_pms - 1;
    }

    for (pm = pm_id_start; pm <= pm_id_end; pm++)
    {
        SHR_IF_ERR_EXIT(portmod_pm_diag_info_get(unit, pm, &diag_info));
        /*
         * PM is not active.
         */
        if (diag_info.type == portmodDispatchTypeCount)
        {
            continue;
        }

        PRT_ROW_ADD(PRT_ROW_SEP_NONE);
        /*
         * Print PM ID
         */
        PRT_CELL_SET("%02d", pm);
        /*
         * Print PM Type String
         */
        PRT_CELL_SET("%s",
                     diag_dnxc_pm_enum_value_to_str(portmod_dispatch_type_t_mapping, diag_info.type, pm_type_str));
        /*
         * Format phys range string and print
         */
        BCM_PBMP_CLEAR(phy_pbmp);
        PORTMOD_PBMP_ITER(diag_info.phys, phy)
        {
            BCM_PBMP_PORT_ADD(phy_pbmp, phy);
        }
        sal_memset(buf, 0, sizeof(buf));
        PRT_CELL_SET("%s", shr_pbmp_range_format(phy_pbmp, buf, sizeof(buf)));
        /*
         * Format attached ports string and print
         */
        if (BCM_PBMP_IS_NULL(diag_info.ports))
        {
            ports_str = "No Port Attached";
        }
        else
        {
            sal_memset(buf, 0, sizeof(buf));
            ports_str = shr_pbmp_range_format(diag_info.ports, buf, sizeof(buf));
        }
        PRT_CELL_SET("%s", ports_str);
    }
    PRT_COMMITX;
exit:
    PRT_FREE;
    SHR_FUNC_EXIT;
}

#ifdef BCM_DNX_SUPPORT
#ifdef INCLUDE_CTEST

static shr_error_e
cmd_dnxc_phy_init_cb(
    int unit)
{
    ctest_soc_property_t ctest_soc_property[] = {
        {"fabric_connect_mode.0", "FE"},
        {NULL}
    };

    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX(unit) && dnx_data_fabric.links.nof_links_get(unit) > 0)
    {
        SHR_IF_ERR_EXIT(ctest_dnxc_set_soc_properties(unit, ctest_soc_property, &diag_dnxc_phy_soc_set_h[unit]));
    }
exit:
    SHR_FUNC_EXIT;
}

static shr_error_e
cmd_dnxc_phy_deinit_cb(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    if (SOC_IS_DNX(unit) && dnx_data_fabric.links.nof_links_get(unit) > 0)
    {
        ctest_dnxc_restore_soc_properties(unit, diag_dnxc_phy_soc_set_h[unit]);
    }
    SHR_FUNC_EXIT;
}
#endif
#endif

/**
 * List of the supported commands, pointer to command function and command usage function.
 */
/* *INDENT-OFF* */

/** phy info */
static sh_sand_man_t dnxc_phy_info_man = {
    .brief    = "Dump PHY info",
    .full     = "Dump PHY info",
};

/** phy prbs */
static sh_sand_enum_t phy_prbs_poly_enum_table[] = {
    {"X7_X6_1", BCM_PORT_PRBS_POLYNOMIAL_X7_X6_1, "PRBs polynomial X7_X6_1"},
    {"X15_X14_1", BCM_PORT_PRBS_POLYNOMIAL_X15_X14_1, "PRBs polynomial X15_X14_1"},
    {"X23_X18_1", BCM_PORT_PRBS_POLYNOMIAL_X23_X18_1, "PRBs polynomial X23_X18_1"},
    {"X31_X28_1", BCM_PORT_PRBS_POLYNOMIAL_X31_X28_1, "PRBs polynomial X31_X28_1"},
    {"X9_X5_1", BCM_PORT_PRBS_POLYNOMIAL_X9_X5_1, "PRBs polynomial X9_X5_1"},
    {"X11_X9_1", BCM_PORT_PRBS_POLYNOMIAL_X11_X9_1, "PRBs polynomial X11_X9_1"},
    {"X58_X31_1", BCM_PORT_PRBS_POLYNOMIAL_X58_X31_1, "PRBs polynomial X58_X31_1"},
    {"X49_X40_1", BCM_PORT_PRBS_POLYNOMIAL_X49_X40_1, "PRBs polynomial X49_X40_1"},
    {"X20_X3_1", BCM_PORT_PRBS_POLYNOMIAL_X20_X3_1, "PRBs polynomial X20_X3_1"},
    {"X13_X12_X2_1", BCM_PORT_PRBS_POLYNOMIAL_X13_X12_X2_1, "PRBs polynomial X13_X12_X2_1"},
    {"X10_X7_1", BCM_PORT_PRBS_POLYNOMIAL_X10_X7_1, "PRBs polynomial X10_X7_1"},
    {"0", BCM_PORT_PRBS_POLYNOMIAL_X7_X6_1, "PRBs polynomial X7_X6_1"},
    {"1", BCM_PORT_PRBS_POLYNOMIAL_X15_X14_1, "PRBs polynomial X15_X14_1"},
    {"2", BCM_PORT_PRBS_POLYNOMIAL_X23_X18_1, "PRBs polynomial X23_X18_1"},
    {"3", BCM_PORT_PRBS_POLYNOMIAL_X31_X28_1, "PRBs polynomial X31_X28_1"},
    {"4", BCM_PORT_PRBS_POLYNOMIAL_X9_X5_1, "PRBs polynomial X9_X5_1"},
    {"5", BCM_PORT_PRBS_POLYNOMIAL_X11_X9_1, "PRBs polynomial X11_X9_1"},
    {"6", BCM_PORT_PRBS_POLYNOMIAL_X58_X31_1, "PRBs polynomial X58_X31_1"},
    {"7", BCM_PORT_PRBS_POLYNOMIAL_X49_X40_1, "PRBs polynomial X49_X40_1"},
    {"8", BCM_PORT_PRBS_POLYNOMIAL_X20_X3_1, "PRBs polynomial X20_X3_1"},
    {"9", BCM_PORT_PRBS_POLYNOMIAL_X13_X12_X2_1, "PRBs polynomial X13_X12_X2_1"},
    {"10", BCM_PORT_PRBS_POLYNOMIAL_X10_X7_1, "PRBs polynomial X10_X7_1"},
    {NULL}
};

static sh_sand_enum_t phy_prbs_mode_enum_table[] = {
    {"phy", DIAG_DNXC_PHY_PRBS_MODE_PHY, "PRBs mode: PHY"},
    {"mac", DIAG_DNXC_PHY_PRBS_MODE_MAC, "PRBs mode: MAC"},
    {NULL}
};

static sh_sand_man_t dnxc_phy_prbs_set_man = {
    .brief    = "Diagnostic for PHY Prbs set",
    .full     = "Set the PHY prbs parameters.",
    .synopsis = "<pbmp>[mode=<phy|mac>][polynomial=<poly>][invert=<val>]",
    .examples = "sfi mode=mac poly=3 \n"
                "1 poly=X31_X28_1 invert=1"
};

static sh_sand_man_t dnxc_phy_prbs_get_man = {
    .brief    = "Diagnostic for PHY Prbs get",
    .full     = "Get the prbs status. Results might be one of the below: \n"
                "\t 1. Unlocked.  \n"
                "\t 2. Locked but lock was lost since last \'get\' diagnostic. This result is valid only for phy mode. \n"
                "\t 3. Locked. In this case number of errors and BER results are displayed. \n",
    .synopsis = "<pbmp>[interval=<val>]",
    .examples = "1 interval=1"
};

static sh_sand_man_t dnxc_phy_prbs_man = {
    .brief    = "Diagnostic for Prbs",
    .full     = "Diagnostic for Prbs",
};

static sh_sand_man_t dnxc_phy_prbs_clear_man = {
    .brief    = "Diagnostic for PHY Prbs clear",
    .full     = "Stop the PRBS generator and checker",
    .synopsis = "<pbmp>",
    .examples = "sfi \n"
                "13"
};

static sh_sand_option_t dnxc_phy_prbs_set_options[] = {
    /*name           type                  desc          default    ext*/
    {"pbmp",       SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", NULL,   NULL, NULL, SH_SAND_ARG_FREE},
    {"mode",       SAL_FIELD_TYPE_ENUM, "Prbs mode",     "phy", (void *)phy_prbs_mode_enum_table},
    {"invert",     SAL_FIELD_TYPE_BOOL, "invert",     "false", NULL},
    {"polynomial", SAL_FIELD_TYPE_ENUM, "prbs polynomial",     "3", (void *)phy_prbs_poly_enum_table},
    {NULL}
};

static sh_sand_option_t dnxc_phy_prbs_get_options[] = {
    /*name           type                  desc          default    ext*/
    {"pbmp",     SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", NULL,   NULL, NULL, SH_SAND_ARG_FREE},
    {"interval", SAL_FIELD_TYPE_INT32, "interval [seconds]",     "1",   NULL},
    {NULL}
};

static sh_sand_option_t dnxc_phy_prbs_clear_options[] = {
    /*name           type                  desc          default    ext*/
    {"pbmp",    SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", NULL,   NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

static sh_sand_cmd_t sh_dnxc_phy_prbs_cmds[] = {
    /*keyword,   action,                            command,    options,                            man*/
    {"set",     cmd_dnxc_phy_prbs_set,         NULL,       dnxc_phy_prbs_set_options,      &dnxc_phy_prbs_set_man,    NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"get",     cmd_dnxc_phy_prbs_get,         NULL,       dnxc_phy_prbs_get_options,      &dnxc_phy_prbs_get_man,    NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"clear",   cmd_dnxc_phy_prbs_clear,       NULL,       dnxc_phy_prbs_clear_options,    &dnxc_phy_prbs_clear_man,  NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {NULL}
};

/** phy measure */
static sh_sand_enum_t phy_measure_option_enum_table[] = {
    {"rx", DIAG_DNXC_PHY_MEASURE_OPTION_RX, "Use Rx rate FIFO to do phy measure."},
    {"tx", DIAG_DNXC_PHY_MEASURE_OPTION_TX, "Use Tx rate FIFO to do phy measure."},
    {NULL}
};

static sh_sand_man_t dnxc_phy_measure_man = {
    .brief    = "Diagnostic for PHY measure",
    .full     = "Measure the Serdes Rate. Only <pbmp> is mandatory.",
    .synopsis = "<pbmp> [<tx|rx>]",
    .examples = "sfi \n"
                "1 rx"
};
static sh_sand_option_t dnxc_phy_measure_options[] = {
    /*name           type                  desc          default                         ext        valid flags*/
    {"pbmp",      SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", NULL,   NULL,      NULL, SH_SAND_ARG_FREE},
    {"direction", SAL_FIELD_TYPE_ENUM, "Tx or Rx",     "rx", (void *)phy_measure_option_enum_table, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

/** phy cl72 */

static sh_sand_enum_t phy_cl72_option_enum_table[] = {
    {"on", DIAG_DNXC_PHY_CL72_OPTION_ON, "Turn on link training."},
    {"off", DIAG_DNXC_PHY_CL72_OPTION_OFF, "Turn off link training."},
    {"state", DIAG_DNXC_PHY_CL72_OPTION_STATE, "Show link traning state."},
    {NULL}
};

static sh_sand_man_t dnxc_phy_cl72_man = {
    .brief    = "Diagnostic for PHY cl72",
    .full     = "Turn on or Turn off PHY CL72 for given ports, "
                "if 'on' or 'off' is not present, dump the current"
                "CL72 status",
#ifdef BCM_DNX_SUPPORT
#ifdef INCLUDE_CTEST
    .init_cb = cmd_dnxc_phy_init_cb,
    .deinit_cb = cmd_dnxc_phy_deinit_cb,
#endif
#endif
    .synopsis = "<pbmp>[<on|off|NULL string>]",
    .examples = "sfi \n"
                "sfi off \n"
                "sfi on \n"
                "1 on"
};
static sh_sand_option_t dnxc_phy_cl72_options[] = {
    /*name           type                  desc          default    ext*/
    {"pbmp",    SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", NULL,   NULL, NULL, SH_SAND_ARG_FREE},
    {"mode", SAL_FIELD_TYPE_ENUM, "on or off",     "state", (void *)phy_cl72_option_enum_table, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

/** phy raw */
static sh_sand_man_t dnxc_phy_raw_man = {
    .brief    = "Diagnostic for PHY raw command",
    .full     = "Diagnostic for PHY raw command",
};

static sh_sand_man_t dnxc_phy_raw_c45_man = {
    .brief    = "Diagnostic for PHY raw c45 command",
    .full     = "Set or Get phy registers via clause 45 MDIO. "
                "If option \"data\" is not present, it a read register operation.",
    .synopsis = "phy_id=<val> devad=<val> regad=<val> [data=<val>]",
    .examples = "phy_id=0x1a0 devad=0x1 regad=0x96 \n"
                "phy_id=0x1a0 devad=0x1 regad=0x96 data=0x1"
};

#ifdef BCM_DNX_SUPPORT
static sh_sand_man_t dnxc_phy_raw_sbus_man = {
    .brief    = DIAG_DNXC_PHY_RAW_SBUS_BRIEF_MAN,
    .full     = DIAG_DNXC_PHY_RAW_SBUS_FULL_MAN,
    .synopsis = DIAG_DNXC_PHY_RAW_SBUS_SYNOPSIS,
    .examples = "pm_id=1 devad=1 regad=0x96 lane=0x1 pindx=0x1 \n"
                "pm_id=1 devad=1 regad=0x96 lane=0x1 pindx=0x1 data=0x1"
};

#endif

static sh_sand_option_t dnxc_phy_raw_c45_options[] = {
    /*name           type                  desc          default    ext*/
    {"phy_id",    SAL_FIELD_TYPE_UINT32, "PHY ID", NULL, NULL},
    {"dev_addr",  SAL_FIELD_TYPE_UINT32, "Device Address", NULL, NULL},
    {"reg_addr",  SAL_FIELD_TYPE_UINT32, "Register Address", NULL, NULL},
    {"data",      SAL_FIELD_TYPE_UINT32, "Set value", "0", NULL},
    {NULL}
};

#ifdef BCM_DNX_SUPPORT
static sh_sand_option_t dnxc_phy_raw_sbus_options[] = {
    /*name           type                  desc          default    ext*/
    {"pm_id",     SAL_FIELD_TYPE_UINT32, "PM ID", NULL, NULL},
    {"dev_addr",  SAL_FIELD_TYPE_UINT32, "Device Address", NULL, NULL},
    {"reg_addr",  SAL_FIELD_TYPE_UINT32, "Register Address", NULL, NULL},
    {"lane",      SAL_FIELD_TYPE_INT32, "Lane ID", "-1", NULL},
    {"pll_index", SAL_FIELD_TYPE_UINT32, "PLL index", "0", NULL},
    {"data",      SAL_FIELD_TYPE_UINT32, "Set value", "0", NULL},
    {NULL}
};
#endif
static sh_sand_cmd_t sh_dnxc_phy_raw_cmds[] = {
    /*keyword,   action,                            command,    options,                            man*/
    {"c45",     cmd_dnxc_phy_raw_c45,         NULL,       dnxc_phy_raw_c45_options,      &dnxc_phy_raw_c45_man},
#ifdef BCM_DNX_SUPPORT
    {"sbus",    cmd_dnxc_phy_raw_sbus,        NULL,       dnxc_phy_raw_sbus_options,     &dnxc_phy_raw_sbus_man,  NULL, NULL, SH_CMD_CONDITIONAL, sh_cmd_is_dnx},
#endif
    {NULL}
};

/** phy dsc */
#ifdef BCM_DNX_SUPPORT
static sh_sand_man_t dnx_phy_dsc_fabric_available_man = {
    .brief    = DIAG_DNXC_PHY_DSC_BRIEF_MAN,
    .full     = DIAG_DNXC_PHY_DSC_FULL_MAN,
#ifdef INCLUDE_CTEST
    .init_cb = cmd_dnxc_phy_init_cb,
    .deinit_cb = cmd_dnxc_phy_deinit_cb,
#endif
    .synopsis = DIAG_DNXC_PHY_DSC_SYNOPSIS,
    .examples = "fabric1 state \n"
                "1 config \n"
                "13"
};

static sh_sand_man_t dnx_phy_dsc_fabric_not_available_man = {
    .brief    = DIAG_DNXC_PHY_DSC_BRIEF_MAN,
    .full     = DIAG_DNXC_PHY_DSC_FULL_MAN,
    .synopsis = DIAG_DNXC_PHY_DSC_SYNOPSIS,
    .examples = "13 state \n"
                "1 config \n"
                "13"
};
#endif

#ifdef BCM_DNXF_SUPPORT
static sh_sand_man_t dnxf_phy_dsc_man = {
    .brief    = DIAG_DNXC_PHY_DSC_BRIEF_MAN,
    .full     = DIAG_DNXC_PHY_DSC_FULL_MAN,
    .synopsis = DIAG_DNXC_PHY_DSC_SYNOPSIS,
    .examples = "fabric1 state \n"
                "1 config \n"
                "13"
};
#endif

static sh_sand_option_t dnxc_phy_dsc_options[] = {
    /*name           type                  desc          default    ext*/
    {"pbmp",    SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", NULL,   NULL, NULL, SH_SAND_ARG_FREE},
    {"state", SAL_FIELD_TYPE_BOOL,  "Detailed DSC dump, including lane info, core info and lane event info and EYE Scan", "false", NULL},
    {"config", SAL_FIELD_TYPE_BOOL, "Dump Serdes lane and core configuration", "false", NULL},
    {NULL}
};

/** phy list */

static sh_sand_man_t dnxc_phy_list_man = {
    .brief    = "Diagnostic for PHY list",
    .full     = "Print register matching the given pattern/address",
    .synopsis = "<pbmp> [pattern]",
    .examples = "sfi pmd \n"
                "13 ctrl"
};
static sh_sand_option_t dnxc_phy_list_options[] = {
    /*name           type                  desc                                default ext   valid flags*/
    {"pbmp",    SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", NULL,   NULL, NULL, SH_SAND_ARG_FREE},
    {"pattern", SAL_FIELD_TYPE_STR,  "pattern",                                NULL,   NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

/** phy set */

static sh_sand_man_t dnxc_phy_set_man = {
    .brief    = "Diagnostic for PHY Set",
    .full     = "Configure the PHY reigster according the given ports and register info. "
                "If \"field\" option is not present, it will set value for the register; "
                "Otherwise, it will set value for the given field. "
                "\"lane\" and \"pll_index\" are used for configuring registers for "
                "each lane and each PLL.",
    .synopsis = "<pbmp> reg=<reg_name|reg_addr> [field=<val>] [lane=<val>] [pll_index=<val>] data=<val>",
    .examples = "sfi reg=DIG_RST_CTL_PMDr data=0x1 \n"
                "13 reg=DIG_RST_CTL_PMDr fld=CORE_S_RSTB dt=0x1 \n"
                "14 reg=DIG_RST_CTL_PMDr fld=CORE_S_RSTB lane=0 pindx=0 dt=0x1 \n"
                "1 reg=0xd101 data=0x1"
};
static sh_sand_option_t dnxc_phy_set_options[] = {
    /*name           type                  desc                                default    ext*/
    {"pbmp",    SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", NULL,   NULL, NULL, SH_SAND_ARG_FREE},
    {"register",  SAL_FIELD_TYPE_STR, "Register name or addr",                 NULL},
    {"field",     SAL_FIELD_TYPE_STR, "Register field name",                   ""},
    {"lane",      SAL_FIELD_TYPE_INT32, "Lane ID",                             "-1"},
    {"pll_index", SAL_FIELD_TYPE_INT32, "PLL index",                           "-1"},
    {"data",      SAL_FIELD_TYPE_UINT32, "Set value",                          NULL},
    {NULL}
};

/** phy get */
static sh_sand_man_t dnxc_phy_get_man = {
    .brief    = "Diagnostic for PHY Get",
    .full     = "Get the PHY reigster according the given ports, register info and pattern.\n"
                "option \"nz\" is used to dump the value which is not zero.\n"
                "option \"pattern\" can be one of the following formats: \n"
                "    1. Register ADDR \n"
                "    2. Register name \n"
                "    3. Part of register name with wildcard(only valid when option \"nz\" is present)\n"
                "\"lane\" and \"pll_index\" are used for getting register values for "
                "each lane and each PLL. They are only available when option \"nz\" is not present.",
    .synopsis = "<pbmp>[nz][pattern][lane=<val>][pll_index=<val>]",
    .examples = "sfi nz *pmd \n"
                "1 nz * \n"
                "13 DIG_RST_CTL_PMDr \n"
                "13 DIG_RST_CTL_PMDr lane=0 pindx=0 \n"
                "14 nz DIG_RST_CTL_PMDr"
};
static sh_sand_option_t dnxc_phy_get_options[] = {
    /*name           type                  desc          default    ext*/
    {"pbmp",    SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", NULL,   NULL, NULL, SH_SAND_ARG_FREE},
    {"nonzero",  SAL_FIELD_TYPE_BOOL, "list registers matching the pattern and the value is not zero", "false", NULL},
    {"pattern",  SAL_FIELD_TYPE_STR, "pattern", "", NULL, NULL, SH_SAND_ARG_FREE},
    {"lane",      SAL_FIELD_TYPE_INT32, "Lane ID", "-1", NULL},
    {"pll_index", SAL_FIELD_TYPE_INT32, "PLL index", "-1", NULL},
    {NULL}
};

/** phy get */

static sh_sand_man_t dnxc_phy_eyescan_man = {
    .brief    = "Diagnostic for PHY eyescan",
    .full     = "Option \"pbmp\" is mandatory. "
                "Supported type is 1 and 4. "
                "Type 4 is only supported with a special compilation. "
                "Options \"ber_scan_mode\", \"max_err_control\" and \"timer_control\" are only relevant for type 4. ",
    .synopsis = "<pbmp>[lane=<val>][type=<val>][ber_scan_mode=<val>][max_err_control=<val>] [timer_control=<val>]",
    .examples = "1 lane=0 \n"
                "13 type=1 \n"
                "13 type=4 ber_scan_mode=1 timer_control=100 max_err_control=51"
};
static sh_sand_option_t dnxc_phy_eyescan_options[] = {
    /*name           type                  desc          default    ext*/
    {"pbmp",    SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", NULL,   NULL, NULL, SH_SAND_ARG_FREE},
    {"lane",           SAL_FIELD_TYPE_INT32, "lane", "0", NULL},
    {"type",           SAL_FIELD_TYPE_INT32, "type", "1", NULL},
    {"ber_scan_mode",  SAL_FIELD_TYPE_INT32, "ber_scan_mode", "0", NULL},
    {"max_err_control",SAL_FIELD_TYPE_INT32, "max_err_control", "0", NULL},
    {"timer_control",  SAL_FIELD_TYPE_INT32, "timer_control", "0", NULL},
    {NULL}
};

#ifdef SERDES_API_FLOATING_POINT
/** phy BER projection */
static sh_sand_man_t dnxc_phy_ber_proj_man = {
    .brief    = "Diagnostic for PHY BER projection",
    .full     = "This diag is used to calculate Post FEC BER projection. "
                "Option \'hist_errcnt_threshold\' is set to 0 by default, which means BER projection "
                "tool will choose an optimized threshold automatically. The other acceptable values "
                "for this threshold are [1~7]. This command supports Post FEC BER projection calculation across "
                "multiple lanes and multiple ports simultaneously. \n"
                "Please NOTE: \n"
                "1. BER projection is only available with SERDES_API_FLOATING_POINT compilation flag. \n"
                "2. BER projection checker is working only after setting PRBs manually. ",
    .synopsis = "<pbmp>[hist_errcnt_threshold=<val>] [sample_time=<val>]",
    .examples = "1 \n"
                "13 hist_errcnt_threshold=4 sample_time=120"
};
static sh_sand_option_t dnxc_phy_ber_proj_options[] = {
    /*name                    type                  desc          default    ext*/
    {"pbmp",                  SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", NULL,   NULL, NULL, SH_SAND_ARG_FREE},
    {"hist_errcnt_threshold", SAL_FIELD_TYPE_INT32, "Histogram Error Threshold", "0", NULL},
    {"sample_time",           SAL_FIELD_TYPE_INT32, "Duration in second", "60", NULL},
    {NULL}
};
#endif

#ifdef PHYMOD_LINKCAT_SUPPORT
/** phy Linkcat */
static sh_sand_enum_t phy_linkcat_mode_option_enum_table[] = {
    {"rx",   LINKCAT_RX_MODE, "LinkCAT mode: RX."},
    {"lpbk", LINKCAT_LPBK_MODE, "LinkCAT mode: lPBK."},
#ifdef PHYMOD_NEW_LINKCAT
    {"tx",   LINKCAT_LPTX_MODE, "LinkCAT mode: TX."},
#else
    {"tx",   LINKCAT_TX_MODE, "LinkCAT mode: TX."},
#endif
    {NULL}
};

static sh_sand_man_t dnxc_phy_linkcat_man = {
    .brief    = "Diagnostic for PHY linkCAT",
    .full     = "This diag is used to gather data from SerDes RX and "
                "provide information about the channel, such as Step, Pulse and "
                "Frequency Response. The linkCAT tool outputs three files "
                "Frequency Response(fr), Pulse Response(pr), and Step Response(sr) in CSV format. "
                "Files will be written to path specified by environment variable LINKCAT_PATH or "
                "the execution directory of binary bcm.user. \n"
                "Option \"lane\" is mandatory for multi-lane ports. \n"
                "Option \"prefix\" is only available when compilation flag PHYMOD_NEW_LINKCAT is present.",
    .synopsis = "<pbmp> [lane=<lane_id>] [mode=<mode>] [prefix=<string>]",
    .examples = "1 lane=0\n"
#ifdef PHYMOD_NEW_LINKCAT
                "13 prefix=test mode=lpbk"
#else
                "13 mode=lpbk"
#endif
};
static sh_sand_option_t dnxc_phy_linkcat_options[] = {
    /*name                    type                  desc          default    ext*/
    {"pbmp",      SAL_FIELD_TYPE_PORT,  "port # / logical port type / port name", NULL,   NULL, NULL, SH_SAND_ARG_FREE},
    {"lane",      SAL_FIELD_TYPE_INT32, "Lane ID", "0", NULL},
    {"mode",      SAL_FIELD_TYPE_ENUM,  "LinkCAT mode", "lpbk", (void *)phy_linkcat_mode_option_enum_table},
#ifdef PHYMOD_NEW_LINKCAT
    {"prefix",    SAL_FIELD_TYPE_STR,   "Prefix string for LinkCAT output file", "", NULL},
#endif
    {NULL}
};
#endif

/** phy fecstat */
static sh_sand_man_t dnxc_phy_fecstat_man = {
    .brief    = "Diagnostic for PHY FECSTAT command",
    .full     = "Diagnostic commands for displaying FEC statistics",
};

static sh_sand_man_t dnxc_phy_fecstat_counter_man = {
    .brief    = "Diagnostic to show PHY fecstat counters",
    .full     = "Dump PHY FECSTAT counters, including correctable counter "
                "and uncorrectable counter. Counter thread needs to be enabled "
                "when using this command",
    .synopsis = "<pbmp>",
    .examples = "sfi \n"
                "1"
};

static sh_sand_man_t dnxc_phy_fecstat_ber_man = {
    .brief    = "Diagnostic to show PHY pre-FEC BER",
    .full     = "The command is used to calculate pre-FEC ber value according to "
                "the FEC correctable counter rate. Counter thread needs to be enabled "
                "when using this command",
    .synopsis = "<pbmp>",
    .examples = "sfi \n"
                "1"
};

static sh_sand_option_t dnxc_phy_fecstat_options[] = {
    /*name           type                  desc          default    ext*/
    {"pbmp",   SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", "all", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

static sh_sand_cmd_t sh_dnxc_phy_fecstat_cmds[] = {
    /*keyword,   action,                            command,    options,                            man*/
    {"counters", cmd_dnxc_phy_fecstat_counter,    NULL,       dnxc_phy_fecstat_options,     &dnxc_phy_fecstat_counter_man,  NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"ber",      cmd_dnxc_phy_fecstat_ber,        NULL,       dnxc_phy_fecstat_options,     &dnxc_phy_fecstat_ber_man,      NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {NULL}
};


sh_sand_cmd_t sh_dnxc_phy_cmds[] = {
    /*keyword,  action,   command,  options,  man */
    {"info",      cmd_dnxc_phy_info,      NULL,                   NULL,                      &dnxc_phy_info_man},
    {"prbs",      NULL,                   sh_dnxc_phy_prbs_cmds,  NULL,                      &dnxc_phy_prbs_man},
    {"measure",   cmd_dnxc_phy_measure,   NULL,                   dnxc_phy_measure_options,  &dnxc_phy_measure_man,  NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"cl72",      cmd_dnxc_phy_cl72,      NULL,                   dnxc_phy_cl72_options,     &dnxc_phy_cl72_man,    NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"raw",       NULL,                   sh_dnxc_phy_raw_cmds,   NULL,                      &dnxc_phy_raw_man},
#ifdef BCM_DNX_SUPPORT
    {"dsc",       cmd_dnxc_phy_dsc,       NULL,                   dnxc_phy_dsc_options,      &dnx_phy_dsc_fabric_available_man,        NULL, NULL, SH_CMD_CONDITIONAL | SH_CMD_NO_XML_VERIFY, sh_cmd_is_dnx_with_fabric_available},
    {"dsc",       cmd_dnxc_phy_dsc,       NULL,                   dnxc_phy_dsc_options,      &dnx_phy_dsc_fabric_not_available_man,    NULL, NULL, SH_CMD_CONDITIONAL | SH_CMD_NO_XML_VERIFY, sh_cmd_is_dnx_with_fabric_unvailable},
#endif
#ifdef BCM_DNXF_SUPPORT
    {"dsc",       cmd_dnxc_phy_dsc,       NULL,                   dnxc_phy_dsc_options,      &dnxf_phy_dsc_man,     NULL, NULL, SH_CMD_CONDITIONAL | SH_CMD_NO_XML_VERIFY, sh_cmd_is_dnxf},
#endif
    {"list",      cmd_dnxc_phy_list,      NULL,                   dnxc_phy_list_options,     &dnxc_phy_list_man,     NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"set",       cmd_dnxc_phy_set,       NULL,                   dnxc_phy_set_options,      &dnxc_phy_set_man,      NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"get",       cmd_dnxc_phy_get,       NULL,                   dnxc_phy_get_options,      &dnxc_phy_get_man,      NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"eyescan",   cmd_dnxc_phy_eyescan,   NULL,                   dnxc_phy_eyescan_options,  &dnxc_phy_eyescan_man,  NULL, NULL, SH_CMD_NO_XML_VERIFY},
#ifdef SERDES_API_FLOATING_POINT
    {"berproj",   cmd_dnxc_phy_ber_proj,  NULL,                   dnxc_phy_ber_proj_options, &dnxc_phy_ber_proj_man, NULL, NULL, SH_CMD_NO_XML_VERIFY},
#endif
#ifdef PHYMOD_LINKCAT_SUPPORT
    {"linkcat",   cmd_dnxc_phy_linkcat,   NULL,                   dnxc_phy_linkcat_options,  &dnxc_phy_linkcat_man},
#endif
    {"fecstat",   NULL,                   sh_dnxc_phy_fecstat_cmds,   NULL,                  &dnxc_phy_fecstat_man},
    {"prbsstat",  NULL,                   sh_dnxc_phy_prbsst_cmds,NULL,                      &dnxc_phy_prbsstat_man},
    {NULL}
};

sh_sand_man_t sh_dnxc_phy_man = {
    .brief    = "Misc facilities for displaying phy diagnostics",
    .full     = "PHY diagnostics for dumping PHY status and configuration, configuring PHY, etc",
    .synopsis = NULL,
    .examples = NULL
};

/**
 * \brief Port diagnostics for DNX and DNXF
 */

/** port status */

sh_sand_man_t sh_dnxc_port_status_man = {
    .brief    = "Diagnostic for printing the port status for all the logical ports.",
    .full     = "Dump the port status for all the Eth and ILKN ports, including link status, "
                "speed, Loopback info and so on. ",
    .synopsis = "<pbmp>",
    .examples = "sfi \n"
                "1"
};

sh_sand_option_t sh_dnxc_port_status_options[] = {
    {"pbmp",   SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", "all", NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

/** port loopback */
static sh_sand_enum_t dnxc_port_loopback_mode_enum_table[] = {
    {"none",     BCM_PORT_LOOPBACK_NONE, "Loopback mode: None"},
    {"phy",      BCM_PORT_LOOPBACK_PHY,  "Loopback mode: PHY"},
    {"mac",      BCM_PORT_LOOPBACK_MAC,  "Loopback mode: MAC"},
    {"rmt",      BCM_PORT_LOOPBACK_PHY_REMOTE, "Loopback mode: PHY REMOTE"},
    {"mac_rmt",  BCM_PORT_LOOPBACK_MAC_REMOTE, "Loopback mode: MAC REMOTE"},
    {NULL}
};

sh_sand_man_t sh_dnxc_port_loopback_man = {
    .brief    = "Diagnostic for loopback",
    .full     = "Set PHY, MAC, PHY, REMOTE loopback \n"
                "NOTE! Option mac_rmt not supported for NIF ports.",
    .synopsis = "<pbmp> [mode=<none|mac|phy|rmt|mac_rmt>]",
    .examples = "1 mode=mac \n"
                "1 mode=none"
};

sh_sand_option_t sh_dnxc_port_loopback_options[] = {
    {"pbmp",   SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", NULL, NULL, NULL, SH_SAND_ARG_FREE},
    {"mode",      SAL_FIELD_TYPE_ENUM, "Loopback Mode", NULL, (void *)dnxc_port_loopback_mode_enum_table},
    {NULL}
};

/** port enable */
sh_sand_man_t sh_dnxc_port_enable_man = {
    .brief    = "Diagnostic for port enable",
    .full     = "Enable or Disable port",
    .synopsis = "<pbmp> [<Yes|No|True|False>]",
    .examples = "sfi no \n"
                "sfi yes \n"
                "1 false \n"
                "1 yes \n"
                "13 true"
};

sh_sand_option_t sh_dnxc_port_enable_options[] = {
    {"pbmp",   SAL_FIELD_TYPE_PORT, "port # / logical port type / port name", "all", NULL, NULL, SH_SAND_ARG_FREE},
    {"enable", SAL_FIELD_TYPE_BOOL, "Enable or Disable port",                 NULL, NULL, NULL, SH_SAND_ARG_FREE},
    {NULL}
};

/** port pm */
sh_sand_man_t sh_dnxc_port_pm_man = {
    .brief    = "Diagnostic for port portmacro",
    .full     = "Diagnostic for port portmacro",
    .synopsis = NULL,
    .examples = NULL
};

static sh_sand_man_t sh_dnxc_port_pm_info_man = {
    .brief    = "Diagnostic for port portmacro info",
    .full     = "Dump port portmacro info",
    .synopsis = "[pm_id=<pm_id>]",
    .examples = "pm_id=0 \n"
};

static sh_sand_option_t sh_dnxc_port_pm_info_options[] = {
    {"pm_id",  SAL_FIELD_TYPE_INT32, "PortMacro ID", "-1", NULL},
    {NULL}
};


sh_sand_cmd_t sh_dnxc_port_pm_cmds[] = {
    /*
     * keyword, action, command, options, man callback legacy
     */
    {"info", cmd_dnxc_port_pm_info, NULL,   sh_dnxc_port_pm_info_options, &sh_dnxc_port_pm_info_man},
    {NULL}
};

sh_sand_cmd_t sh_dnxc_port_cmds[] = {
    /*
     * keyword, action, command, options, man callback legacy
     */
    {"status",   cmd_dnxc_port_status,   NULL,   sh_dnxc_port_status_options,   &sh_dnxc_port_status_man, NULL, NULL, SH_CMD_NO_XML_VERIFY},
    {"loopback", cmd_dnxc_port_loopback, NULL,   sh_dnxc_port_loopback_options, &sh_dnxc_port_loopback_man},
    {"enable",   cmd_dnxc_port_enable,   NULL,   sh_dnxc_port_enable_options,   &sh_dnxc_port_enable_man},
    {"portmod",  NULL,   sh_dnxc_port_pm_cmds,   NULL,                          &sh_dnxc_port_pm_man},
    {NULL}
};

sh_sand_man_t sh_dnxc_port_man = {
    .brief = "Diagnostic pack for port module",
    .full = NULL
};
/* *INDENT-ON* */
/*
 * }
 */
#undef _ERR_MSG_MODULE_NAME
