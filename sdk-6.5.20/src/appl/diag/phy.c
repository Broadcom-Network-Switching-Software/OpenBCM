/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        phy.c
 * Purpose:     Functions to support CLI port commands
 * Requires:
 */
#include <sal/core/libc.h>
#include <sal/types.h>
#include <sal/appl/sal.h>
#include <sal/appl/io.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>

#include <soc/phy.h>
#include <soc/phy/phymod_sim.h>
#include <soc/eyescan.h>
#include <soc/phy/phyctrl.h>

#include <appl/diag/shell.h>
#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/diag.h>
#include <appl/diag/dport.h>

#if defined(BCM_ESW_SUPPORT)
#include <bcm_int/esw/port.h>
#endif

#if defined(PHYMOD_SUPPORT)
#include <appl/diag/phymod/phymod_symop.h>
#include <soc/phyreg.h>
#include <soc/phy/phymod_ctrl.h>
#include <phymod/phymod_reg.h>
#include <phymod/phymod_debug.h>
#include <phymod/phymod_diag.h>
#if defined(PORTMOD_SUPPORT)
#include <soc/portmod/portmod.h>
#include <soc/portmod/portmod_internal.h>
#endif
#endif
#if defined(PORTMOD_SUPPORT)
#include <soc/portmod/portmod_common.h>
#include <soc/portmod/portmod_chain.h>
#endif

#ifdef SW_AUTONEG_SUPPORT
#include <bcm_int/common/sw_an.h>
#endif

#ifdef INCLUDE_PHY_SYM_DBG
/* For timebeing adding the headers for socket calls directly */
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<sys/select.h>
#endif

#if defined(INCLUDE_PHY_8806X)
#include <phy8806x_funcs.h>
#include <phy8806x_ctr.h>
extern int phy_is_8806x(phy_ctrl_t *pc);
#endif

#if defined(INCLUDE_FCMAP)
/* BFCMAP TEST */
#include <bcm/fcmap.h>
extern int bfcmap_pcfg_set_get(int port);
extern int bfcmap88060_xmod_debug_cmd(int unit, int port, int cmd);
int bfcmap88060_show_fc_config(int unit, int port);
#endif
extern int phy8806x_xmod_debug_cmd(int unit, int port);

#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm_int/control.h>

#if defined(BCM_ESW_SUPPORT)
#include <soc/xaui.h>
#endif

#define DUMP_PHY_COLS    4
#define PHY_UPDATE(_flags, _control) ((_flags) & (1 << (_control)))
#define IS_LONGREACH(_type) (((_type)>=BCM_PORT_PHY_CONTROL_LONGREACH_SPEED) && \
                            ((_type)<=BCM_PORT_PHY_CONTROL_LONGREACH_ENABLE))

/* Note: See port.h, bcm_port_phy_control_t */
char *phy_control[] = {
    "WAN                     ",
    "Preemphasis             ",
    "DriverCurrent           ",
    "PreDriverCurrent        ",
    "EqualizerBoost          ",
    "Interface               ",
    "InterfaceMAX            ",
    "MacsecSwitchFixed       ",
    "MacsecSwitchFixedSpeed  ",
    "MacsecSwitchFixedDuplex ",
    "MacsecSwitchFixedPause  ",
    "MacsecPauseRxForward    ",
    "MacsecPauseTxForward    ",
    "MacsecLineIPG           ",
    "MacsecSwitchIPG         ",
    "SPeed                   ",
    "PAirs                   ",
    "GAin                    ",
    "AutoNeg                 ",
    "LocalAbility            ",
    "RemoteAbility      (RO) ",
    "CurrentAbility     (RO) ",
    "MAster                  ",
    "Active             (RO) ",
    "ENable                  ",
    "PrePremphasis           ",
    "Encoding                ",
    "Scrambler               ",
    "PrbsPolynomial          ",
    "PrbsTxInvertData        ",
    "PrbsTxEnable            ",
    "PrbsRxEnable            ",
    "PrbsRxStatus            ",
    "SerdesDriverTune        ",
    "SerdesDriverEqualTuneStatus",
    "8b10b                   ",
    NULL
};

/* Note:  See eyescan.h soc_stat_eyscan_counter_e */
char *eyescan_counter[] = {
    "RelativePhy", "PrbsPhy", "PrbsMac", "CrcMac", "BerMac", "Custom", NULL
};


cmd_result_t
port_phy_control_update(int u, bcm_port_t p, bcm_port_phy_control_t type,
                        uint32 val, uint32 flags, int *print_header)
{
    int rv;
    uint32 oval, flag_bit;
    char buffer[100];

    oval = 0;
    rv = bcm_port_phy_control_get(u, p, type, &oval);
    if (BCM_FAILURE(rv) && BCM_E_UNAVAIL != rv) {
        cli_out("%s\n", bcm_errmsg(rv));
        return CMD_FAIL;
    } else if (BCM_SUCCESS(rv)) {
        switch ( type ) {  /* for "phy clock" command */
            case BCM_PORT_PHY_CONTROL_CLOCK_ENABLE:
                flag_bit = 0;
                break;
            case BCM_PORT_PHY_CONTROL_CLOCK_SECONDARY_ENABLE:
                flag_bit = 1;
                break;
            case BCM_PORT_PHY_CONTROL_CLOCK_MODE_AUTO:
                flag_bit = 3;
                break;
            case BCM_PORT_PHY_CONTROL_CLOCK_AUTO_SECONDARY:
                flag_bit = 4;
                break;
            case BCM_PORT_PHY_CONTROL_FIRMWARE_DFE_ENABLE:
                flag_bit = 8;
                break;
            case BCM_PORT_PHY_CONTROL_FIRMWARE_LP_DFE_ENABLE:
                flag_bit = 9;
                break;
            case BCM_PORT_PHY_CONTROL_FIRMWARE_BR_DFE_ENABLE:
                flag_bit = 10;
                break;
            case BCM_PORT_PHY_CONTROL_CL72:
                flag_bit = 11;
                break;
            case BCM_PORT_PHY_CONTROL_FORWARD_ERROR_CORRECTION:
                flag_bit = 12;
                break;
            case BCM_PORT_PHY_CONTROL_FORWARD_ERROR_CORRECTION_CL91:
                flag_bit = 13;
                break;
            case BCM_PORT_PHY_CONTROL_FORWARD_ERROR_CORRECTION_CL108:
                flag_bit = 14;
                break;
            default:
                flag_bit = type;
        }
        if ((val != oval) && PHY_UPDATE(flags, flag_bit)) {
            if (BCM_FAILURE(bcm_port_phy_control_set(u, p, type, val))) {
                cli_out("%s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
            oval = val;
        }
        if (*print_header) {
            cli_out("Current PHY control settings of %s ->\n",
                    BCM_PORT_NAME(u, p));
            *print_header = FALSE;
        }
        if (IS_LONGREACH(type)) {
            switch (type) {
                case BCM_PORT_PHY_CONTROL_LONGREACH_SPEED:
                case BCM_PORT_PHY_CONTROL_LONGREACH_PAIRS:
                case BCM_PORT_PHY_CONTROL_LONGREACH_GAIN:
                    sal_sprintf(buffer, "%d", oval);
                    break;
                case BCM_PORT_PHY_CONTROL_LONGREACH_LOCAL_ABILITY:
                case BCM_PORT_PHY_CONTROL_LONGREACH_REMOTE_ABILITY:
                case BCM_PORT_PHY_CONTROL_LONGREACH_CURRENT_ABILITY:
                    format_phy_control_longreach_ability(buffer, sizeof(buffer),
                                                         (soc_phy_control_longreach_ability_t)
                                                         oval);
                    break;
                case BCM_PORT_PHY_CONTROL_LONGREACH_AUTONEG:
                case BCM_PORT_PHY_CONTROL_LONGREACH_MASTER:
                case BCM_PORT_PHY_CONTROL_LONGREACH_ACTIVE:
                case BCM_PORT_PHY_CONTROL_LONGREACH_ENABLE:
                    sal_sprintf(buffer, "%s", (oval == 1) ? "True" : "False");
                    break;

                /* coverity[dead_error_begin] */
                default:
                    buffer[0] = 0;
                    break;

            }
            cli_out("%s = %s\n", phy_control[type], buffer);
        } else if (type == BCM_PORT_PHY_CONTROL_LOOPBACK_EXTERNAL) {
            cli_out("        ENable = %s\n", (oval == 1) ? "True" : "False");
        } else if (type == BCM_PORT_PHY_CONTROL_CLOCK_ENABLE) {
            cli_out("Extraction to clock out (PRImary)          = %s\n",
                    (oval == 1) ? "Enabled" : "Disabled");
        } else if (type == BCM_PORT_PHY_CONTROL_CLOCK_SECONDARY_ENABLE) {
            cli_out("Extraction to clock out (SECondary)        = %s\n",
                    (oval == 1) ? "Enabled" : "Disabled");
        } else if (type == BCM_PORT_PHY_CONTROL_CLOCK_MODE_AUTO) {
            cli_out("Recovered clock auto Disable (AutoDisable) = %s\n",
                    (oval == 1) ? "Yes" : "No");
        } else if (type == BCM_PORT_PHY_CONTROL_CLOCK_AUTO_SECONDARY) {
            cli_out("Auto switch to Secondary   (AutoSECondary) = %s\n",
                    (oval == 1) ? "Yes" : "No");
        } else if (type == BCM_PORT_PHY_CONTROL_CLOCK_SOURCE) {
            cli_out("Recovery clock is being derived from       = %s\n",
                    (oval == bcmPortPhyClockSourcePrimary) ? "PRImary" : "SECondary");
        } else if (type == BCM_PORT_PHY_CONTROL_CLOCK_FREQUENCY) {
            cli_out("Extraction / Input (FR)equency             = %d KHz\n", oval);
        } else if (type == BCM_PORT_PHY_CONTROL_PORT_PRIMARY) {
            cli_out("(BA)se port of chip                        = %d\n", oval);
        } else if (type == BCM_PORT_PHY_CONTROL_PORT_OFFSET) {
            cli_out("Port (OF)fset within the chip              = %d\n", oval);
        } else if (type == BCM_PORT_PHY_CONTROL_FIRMWARE_DFE_ENABLE) {
            cli_out("DFE ENable               = %s\n", (oval == 1) ? "True" : "False");
        } else if (type == BCM_PORT_PHY_CONTROL_FIRMWARE_LP_DFE_ENABLE) {
            cli_out("LP DFE ENable            = %s\n", (oval == 1) ? "True" : "False");
        } else if (type == BCM_PORT_PHY_CONTROL_FIRMWARE_BR_DFE_ENABLE) {
            cli_out("BR DFE ENable            = %s\n", (oval == 1) ? "True" : "False");
        } else if (type == BCM_PORT_PHY_CONTROL_CL72) {
            cli_out("LinkTraining Enable      = %s\n", (oval == 1) ? "True" : "False");
        } else if (type == BCM_PORT_PHY_CONTROL_FORWARD_ERROR_CORRECTION) {
            cli_out("CL74                     = %s\n", oval ? "True" : "False");
        } else if (type == BCM_PORT_PHY_CONTROL_FORWARD_ERROR_CORRECTION_CL91) {
            cli_out("CL91                     = %s\n", oval ? "True" : "False");
        } else if (type == BCM_PORT_PHY_CONTROL_FORWARD_ERROR_CORRECTION_CL108) {
            cli_out("CL108                    = %s\n", oval ? "True" : "False");
        }else {
            cli_out("%s = 0x%0x\n", phy_control[type], oval);
        }
    }
    return CMD_OK;
}

int
port_diag_ctrl(int unit, soc_port_t port,
               uint32 inst, int op_type, int op_cmd, void *arg)
{
    int rv;
#if defined(BCM_ESW_SUPPORT)
    if(SOC_IS_ESW(unit)) {
        PORT_LOCK(unit);
    }
#endif
    {
#ifdef PORTMOD_SUPPORT
        int is_legacy_phy = 0;
        int dev, is_internal = 0;
        if (SOC_PORT_USE_PORTCTRL(unit, port)) {
            rv = portmod_port_is_legacy_ext_phy_present(unit, port, &is_legacy_phy);
            if (rv < 0) {
#if defined(BCM_ESW_SUPPORT)
                if(SOC_IS_ESW(unit)) {
                    PORT_UNLOCK(unit);
                }
#endif
                return rv;
            }
            dev  = PHY_DIAG_INST_DEV(inst);
              if (dev == PHY_DIAG_DEV_DFLT) {
              is_internal = is_legacy_phy ? 0 : 1;
              } else if (dev == PHY_DIAG_DEV_INT) {
                  is_internal = 1;
              } else {
                  is_internal = 0;
              }

            if (is_internal) {
            rv = portmod_port_diag_ctrl(unit, port, inst, op_type, op_cmd, arg);
            } else {
                if (!is_legacy_phy) {
                    rv = portmod_port_diag_ctrl(unit, port, inst, op_type, op_cmd, arg);
                } else {
                    rv = soc_phyctrl_diag_ctrl(unit, port, inst, op_type, op_cmd, arg);
                }
            }
        } else
#endif
        {
            rv = soc_phyctrl_diag_ctrl(unit, port, inst, op_type, op_cmd, arg);
        }
    }
#if defined(BCM_ESW_SUPPORT)
    if(SOC_IS_ESW(unit)) {
        PORT_UNLOCK(unit);
    }
#endif
    return rv;
}

/* definition for phy low power control command */
typedef struct {
    bcm_pbmp_t pbm;
    int registered;
} phy_power_ctrl_t;

static phy_power_ctrl_t phy_pctrl_desc[SOC_MAX_NUM_DEVICES];

STATIC void
_phy_power_linkscan_cb(int unit, soc_port_t port, bcm_port_info_t *info)
{
    int found = FALSE;
    soc_port_t p, dport;
    phy_power_ctrl_t *pDesc;
    bcm_port_cable_diag_t cds;
    int rv;

    pDesc = &phy_pctrl_desc[unit];

    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pDesc->pbm, dport, p) {
        if (p == port) {
            found = TRUE;
            break;
        }
    }

    if (found == TRUE) {
        if (info->linkstatus == BCM_PORT_LINK_STATUS_UP) {
            /* link down->up transition */

            /* Check if giga link */
            if (info->speed != 1000) {
                return;
            }

            /* Run cable diag if giga link */
            sal_memset(&cds, 0, sizeof(bcm_port_cable_diag_t));
            rv = bcm_port_cable_diag(unit, p, &cds);
            if (SOC_FAILURE(rv)) {
                return;
            }

            if (cds.pair_len[0] >= 0 && cds.pair_len[0] < 10) {
                /* Enable low power mode */
                (void)bcm_port_phy_control_set(unit, p,
                                               BCM_PORT_PHY_CONTROL_POWER,
                                               BCM_PORT_PHY_CONTROL_POWER_LOW);
            }
        } else {
            /* link up->down transition */
            /* disable low-power mode */
            (void)bcm_port_phy_control_set(unit, p,
                                           BCM_PORT_PHY_CONTROL_POWER,
                                           BCM_PORT_PHY_CONTROL_POWER_FULL);

        }
    }
}

STATIC int
_phy_auto_low_start(int unit, bcm_pbmp_t pbm, int enable)
{
    soc_port_t p, dport;
    phy_power_ctrl_t *pDesc;

    pDesc = &phy_pctrl_desc[unit];

    if (enable) {
        if (!pDesc->registered) {
            pDesc->pbm = pbm;
            (void)bcm_linkscan_register(unit, _phy_power_linkscan_cb);
            pDesc->registered = TRUE;
        }
    } else {
        if (pDesc->registered) {
            (void)bcm_linkscan_unregister(unit, _phy_power_linkscan_cb);
            pDesc->registered = FALSE;
            /* coverity[overrun-local] */
            DPORT_BCM_PBMP_ITER(unit, pbm, dport, p) {
                (void)bcm_port_phy_control_set(unit, p,
                                               BCM_PORT_PHY_CONTROL_POWER,
                                               BCM_PORT_PHY_CONTROL_POWER_FULL);
            }
        }
    }
    return SOC_E_NONE;
}

STATIC cmd_result_t
_phy_diag_phy_unit_get(int unit_value, int *phy_dev)
{
    *phy_dev = PHY_DIAG_DEV_DFLT;
    if (unit_value == 0) {      /* internal PHY */
        *phy_dev = PHY_DIAG_DEV_INT;
    } else if ((unit_value > 0) && (unit_value < 4)) {
        *phy_dev = PHY_DIAG_DEV_EXT;
    } else if (unit_value != -1) {
        cli_out("unit is numeric value: 0,1,2, ...\n");
        return CMD_FAIL;
    }
    return CMD_OK;
}

STATIC cmd_result_t
_phy_diag_phy_if_get(char *if_str, int *dev_if)
{
    *dev_if = PHY_DIAG_INTF_DFLT;
    if (if_str) {
        if (sal_strcasecmp(if_str, "sys") == 0) {
            *dev_if = PHY_DIAG_INTF_SYS;
        } else if (sal_strcasecmp(if_str, "line") == 0) {
            *dev_if = PHY_DIAG_INTF_LINE;
        } else if (if_str[0] != 0) {
            cli_out("InterFace must be sys or line.\n");
            return CMD_FAIL;
        }
    } else {
        cli_out("Invalid Interface string\n");
        return CMD_FAIL;
    }
    return CMD_OK;
}

STATIC cmd_result_t
_phy_diag_loopback(int unit, bcm_pbmp_t pbmp, args_t *args)
{
    parse_table_t pt;
    bcm_port_t port, dport;
    int rv=0;
    bcm_error_t bcm_res = BCM_E_NONE;
    cmd_result_t cmd_res = CMD_OK;
    char *if_str = NULL, *mode_str = NULL;
    int phy_unit = 0, phy_unit_value = -1, phy_unit_if;
    int pmd_status = 0, remote_status = 0, internal_status = 0;
    int mode = 0;
    uint32 inst;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "unit", PQ_DFL | PQ_INT, (void *)(0),
                    &phy_unit_value, NULL);
    parse_table_add(&pt, "InterFace", PQ_STRING, 0, &if_str, NULL);
    parse_table_add(&pt, "mode", PQ_STRING, 0, &mode_str, NULL);

    if (parse_arg_eq(args, &pt) < 0) {
        cli_out("Error: invalid option: %s\n", ARG_CUR(args));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    cmd_res = _phy_diag_phy_if_get(if_str, &phy_unit_if);
    if (cmd_res == CMD_OK) {
        cmd_res = _phy_diag_phy_unit_get(phy_unit_value, &phy_unit);
    }

    if (mode_str && sal_strlen(mode_str)) {
        if (sal_strcasecmp(mode_str, "remote") == 0) {
            mode = 1;           /* remote loopback */
        } else if (sal_strcasecmp(mode_str, "local") == 0) {
            mode = 2;           /* internal loopback */
        } else if (sal_strcasecmp(mode_str, "global") == 0) {
            mode = 3;           /* global loopback */
        } else if (sal_strcasecmp(mode_str, "none") == 0) {
            mode = 0;           /* no loopback */
        } else {
            cli_out("valid modes: remote,local,global and none\n");
            cmd_res = CMD_FAIL;
        }
    } else {
            mode = -1;
    }
    /* Now free allocated strings */
    parse_arg_eq_done(&pt);

    if (cmd_res != CMD_OK) {
        return cmd_res;
    }

    inst = PHY_DIAG_INSTANCE(phy_unit, phy_unit_if, PHY_DIAG_LN_DFLT);

    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {

        if (mode > 0) {
            bcm_res = bcm_port_loopback_set(unit, port, 2); /* PHY */

            if (bcm_res != BCM_E_NONE) break;

            bcm_res = port_diag_ctrl(unit, port, PHY_DIAG_INSTANCE(phy_unit, PHY_DIAG_INTF_LINE, PHY_DIAG_LN_DFLT) ,
                                   PHY_DIAG_CTRL_SET,
                                   SOC_PHY_CONTROL_LOOPBACK_INTERNAL,
                                   (void *)FALSE);
            if (bcm_res != BCM_E_NONE) break;
        }

        switch (mode) {
        case 3:
            bcm_res = port_diag_ctrl(unit, port, inst,
                                   PHY_DIAG_CTRL_SET,
                                   SOC_PHY_CONTROL_LOOPBACK_INTERNAL,
                                   (void *)TRUE);
            break;
        case 2:
            bcm_res = port_diag_ctrl(unit, port, inst,
                                   PHY_DIAG_CTRL_SET,
                                   SOC_PHY_CONTROL_LOOPBACK_PMD,
                                   (void *)TRUE);
            break;
        case 1:
            bcm_res = port_diag_ctrl(unit, port, inst,
                                   PHY_DIAG_CTRL_SET,
                                   SOC_PHY_CONTROL_LOOPBACK_REMOTE,
                                   (void *)TRUE);
            break;

        case 0:
            bcm_res = bcm_port_loopback_set(unit, port, 0);

            if (bcm_res != BCM_E_NONE) break;

            bcm_res = port_diag_ctrl(unit, port, PHY_DIAG_INSTANCE(phy_unit, PHY_DIAG_INTF_LINE, PHY_DIAG_LN_DFLT),
                                   PHY_DIAG_CTRL_SET,
                                   SOC_PHY_CONTROL_LOOPBACK_PMD,
                                   (void *)FALSE);
            if (bcm_res!= BCM_E_NONE) break;

            bcm_res = port_diag_ctrl(unit, port, PHY_DIAG_INSTANCE(phy_unit, PHY_DIAG_INTF_LINE, PHY_DIAG_LN_DFLT),
                                   PHY_DIAG_CTRL_SET,
                                   SOC_PHY_CONTROL_LOOPBACK_REMOTE,
                                   (void *)FALSE);
            if (bcm_res != BCM_E_NONE) break;


            bcm_res = port_diag_ctrl(unit, port, PHY_DIAG_INSTANCE(phy_unit, PHY_DIAG_INTF_LINE, PHY_DIAG_LN_DFLT),
                                   PHY_DIAG_CTRL_SET,
                                   SOC_PHY_CONTROL_LOOPBACK_INTERNAL,
                                   (void *)FALSE);
            if (bcm_res != BCM_E_NONE) break;

            bcm_res = port_diag_ctrl(unit, port, PHY_DIAG_INSTANCE(phy_unit, PHY_DIAG_INTF_SYS, PHY_DIAG_LN_DFLT),
                                   PHY_DIAG_CTRL_SET,
                                   SOC_PHY_CONTROL_LOOPBACK_PMD,
                                   (void *)FALSE);
            if (bcm_res != BCM_E_NONE) break;

            bcm_res = port_diag_ctrl(unit, port, PHY_DIAG_INSTANCE(phy_unit, PHY_DIAG_INTF_SYS, PHY_DIAG_LN_DFLT),
                                   PHY_DIAG_CTRL_SET,
                                   SOC_PHY_CONTROL_LOOPBACK_REMOTE,
                                   (void *)FALSE);
            if (bcm_res != BCM_E_NONE) break;

            bcm_res = port_diag_ctrl(unit, port, PHY_DIAG_INSTANCE(phy_unit, PHY_DIAG_INTF_SYS, PHY_DIAG_LN_DFLT),
                                   PHY_DIAG_CTRL_SET,
                                   SOC_PHY_CONTROL_LOOPBACK_INTERNAL,
                                   (void *)FALSE);
            break;


        case -1:
        default:

            bcm_res = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_GET,
                                       SOC_PHY_CONTROL_LOOPBACK_PMD,
                                       (void *)&pmd_status);
            if (bcm_res != BCM_E_NONE) break;

            bcm_res = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_GET,
                                       SOC_PHY_CONTROL_LOOPBACK_REMOTE,
                                       (void *)&remote_status);
            if (bcm_res != (int)BCM_E_NONE) break;

            bcm_res = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_GET,
                                       SOC_PHY_CONTROL_LOOPBACK_INTERNAL,
                                       (void *)&internal_status);
            if (bcm_res != (int)BCM_E_NONE) break;
            cli_out("Loopback Status: u=%d p=%d if=%s PMD=%d, PMD_REMOTE=%d, MAC=%d\n", unit, port,
                   (phy_unit_if & 0x1) ? "L" : "S",  pmd_status, remote_status, internal_status);
        }

    }

    if (bcm_res != BCM_E_NONE) {
        cli_out("Setting loopback failed: %s\n", bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

STATIC cmd_result_t
_phy_diag_dsc(int unit, bcm_pbmp_t pbmp, args_t *args)
{
    parse_table_t pt;
    bcm_port_t port, dport;
    bcm_error_t rv = BCM_E_NONE;
    cmd_result_t cmd_result;

    char *if_str;
    int phy_unit, phy_unit_value = 0, phy_unit_if;
    uint32 inst;
    char *cmd_str = NULL;

    parse_table_init(unit, &pt);

    /*
     * unit:  phy_unit_value: phy chain number
     * if  :  if_str="sys"|"line"
     *
     * in cases like phy diag xe0 dsc u=1 the string is not specified
     * Need to handle this case too.
     */

    if (args->a_argc > 4) {
        if (args->a_argv[4] && args->a_argv[4][0] != 'u') {
            cmd_str = ARG_GET(args);
        }
    }

    parse_table_add(&pt, "unit", PQ_DFL | PQ_INT, (void *)(0),
                    &phy_unit_value, NULL);
    parse_table_add(&pt, "if", PQ_STRING, 0, &if_str, NULL);
    if (parse_arg_eq(args, &pt) < 0) {
        cli_out("Error: invalid option: %s\n", ARG_CUR(args));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    cmd_result = _phy_diag_phy_if_get(if_str, &phy_unit_if);
    if (cmd_result == CMD_OK) {
        cmd_result = _phy_diag_phy_unit_get(phy_unit_value, &phy_unit);
    }

    /* Now free allocated strings */
    parse_arg_eq_done(&pt);

    if (cmd_result != CMD_OK) {
        return cmd_result;
    }

    inst = PHY_DIAG_INSTANCE(phy_unit, phy_unit_if, PHY_DIAG_LN_DFLT);

    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
        rv = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_CMD,
                                   PHY_DIAG_CTRL_DSC, (void *)cmd_str);
        if (rv != BCM_E_NONE) {
            return CMD_FAIL;
        }
    }
    return CMD_OK;
}

cmd_result_t
_phy_diag_pcs(int unit, bcm_pbmp_t pbmp, args_t *args)
{
    parse_table_t pt;
    bcm_port_t port, dport;
    char *if_str;
    int phy_unit, phy_unit_value = 0, phy_unit_if;
    uint32 inst;
    char *cmd_str;
    bcm_error_t rv = BCM_E_NONE;
    cmd_result_t cmd_result;

    cmd_str = ARG_GET(args);

    parse_table_init(unit, &pt);

    /*
     * unit:  phy_unit_value: phy chain number
     * if  :  if_str="sys"|"line"
     */
    parse_table_add(&pt, "unit", PQ_DFL | PQ_INT, (void *)(0),
                    &phy_unit_value, NULL);
    parse_table_add(&pt, "if", PQ_STRING, 0, &if_str, NULL);
    if (parse_arg_eq(args, &pt) < 0) {
        cli_out("Error: invalid option: %s\n", ARG_CUR(args));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    cmd_result = _phy_diag_phy_if_get(if_str, &phy_unit_if);
    if (cmd_result == CMD_OK) {
        cmd_result = _phy_diag_phy_unit_get(phy_unit_value, &phy_unit);
    }


    /* Now free allocated strings */
    parse_arg_eq_done(&pt);

    if (cmd_result != CMD_OK) {
        return cmd_result;
    }

    inst = PHY_DIAG_INSTANCE(phy_unit, phy_unit_if, PHY_DIAG_LN_DFLT);

    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
        rv = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_CMD,
                                   PHY_DIAG_CTRL_PCS, (void *)cmd_str);
        /* coverity[mixed_enums] */
        if (rv != BCM_E_NONE) {
            return CMD_FAIL;
        }
    }
    return CMD_OK;
}

#ifdef PHYMOD_LINKCAT_BLACKHAWK_SUPPORT
STATIC cmd_result_t
_phy_diag_linkcat(int unit, bcm_pbmp_t pbmp, args_t *args)
{
    parse_table_t pt;
    bcm_port_t port, dport;
    int lane; /* lane number specified on command line */
    char *mode = NULL;
    int mode_val = phymodlinkCAT_LPBK_MODE; /* mode specified on command line. Default to lpbk */
    phymod_phy_access_t access;
    int num_lanes;
    int first_bit, mask;
    int rv;

    if ( args->a_argc < 5) {
        return CMD_USAGE;
    }
    parse_table_init(unit, &pt);
    parse_table_add(&pt, "lane",  PQ_INT,
                    (void *)(0), &lane, NULL);
    parse_table_add(&pt, "mode", PQ_DFL | PQ_STRING,
               (void *)(0), &mode, NULL);
    if ( parse_arg_eq(args, &pt) < 0) {
        cli_out("Error: invalid option: %s\n", ARG_CUR(args));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    if ( mode != NULL ) {
        if ( !sal_strcasecmp(mode, "tx") ) {
            mode_val = phymodlinkCAT_TX_MODE;
        } else if ( !sal_strcasecmp(mode, "rx") ) {
            mode_val = phymodlinkCAT_RX_MODE;
        } else if ( !sal_strcasecmp(mode, "lpbk") ) {
            mode_val = phymodlinkCAT_LPBK_MODE;
        } else {
            cli_out("Invalid mode parameter %s\n", mode);
            return CMD_OK;
        }
    }

    parse_arg_eq_done(&pt);

    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
        num_lanes =  SOC_INFO(unit).port_num_lanes[port];

#ifdef PORTMOD_SUPPORT
        portmod_access_get_params_t params;
        phymod_core_access_t internal_core;
        int nof_phys, is_most_ext, max_phys = 1, nof_cores = 0;

        phymod_core_access_t_init(&internal_core);
        portmod_port_main_core_access_get(unit, port, 0, &internal_core, &nof_cores);
        if(nof_cores == 0) {
            continue;
        }

        params.phyn = 0; /* internal PHY */
        params.sys_side = PORTMOD_SIDE_LINE;
        params.lane = -1; /* request all lanes */
        params.apply_lane_mask = 0;

        portmod_port_phy_lane_access_get(unit, port, &params, max_phys, &access, &nof_phys, &is_most_ext);
#endif /* PORTMOD_SUPPORT */

        if ( access.access.bus == NULL || !access.access.addr || !access.access.lane_mask ) {
            cli_out("\nERROR: Unable to obtain PM core information for port %d\n. Command terminated", port);
            return CMD_OK;
        }

        if ( lane >= num_lanes ) {
            cli_out("\nInvalid lane number %d. Port %d has only %d lanes (0x%x)\n", lane, port, num_lanes, access.access.lane_mask);
            return CMD_OK;
        }

        mask = 0x1;
        for(first_bit = 0; first_bit < 32; first_bit++) {
           if ( (access.access.lane_mask & mask) ) {
                access.access.lane_mask = 1 << ( first_bit + lane );
                break;
            }
            mask = mask << 1;
        }

        cli_out("Running linkCAT on addr=0x%x lane_mask=0x%x mode=%d\n",
                access.access.addr, access.access.lane_mask, mode_val);
        rv = phymod_phy_linkcat(&access, mode_val);
        if (rv == BCM_E_UNAVAIL) {
            cli_out("This utility supports only PM8x50\n");
            return CMD_OK;
        } else if (rv != BCM_E_NONE)  {
            printf("LinkCAT returns with error code %d\n", rv);
            return (rv);
        }
    }
    return CMD_OK;
}
#endif /* PHYMOD_LINKCAT_BLACKHAWK_SUPPORT */

STATIC cmd_result_t
_phy_diag_prbs(int unit, bcm_pbmp_t pbmp, args_t *args)
{
    parse_table_t pt;
    bcm_port_t port, dport;
    int rv, cmd, enable;
    char *cmd_str, *if_str, *poly_str=NULL;
    int poly = 0, invert = 0, max_poly_index = 0;
    int phy_unit, phy_unit_value = -1, phy_unit_if;
    uint32 inst;

    enum { _PHY_PRBS_SET_CMD, _PHY_PRBS_GET_CMD, _PHY_PRBS_CLEAR_CMD };
    enum { _PHY_PRBS_SI_MODE, _PHY_PRBS_HC_MODE };

    if ((cmd_str = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    if (sal_strcasecmp(cmd_str, "set") == 0) {
        cmd = _PHY_PRBS_SET_CMD;
        enable = 1;
    } else if (sal_strcasecmp(cmd_str, "get") == 0) {
        cmd = _PHY_PRBS_GET_CMD;
        enable = 0;
    } else if (sal_strcasecmp(cmd_str, "clear") == 0) {
        cmd = _PHY_PRBS_CLEAR_CMD;
        enable = 0;
    } else
        return CMD_USAGE;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "unit", PQ_DFL | PQ_INT, (void *)(0),
                    &phy_unit_value, NULL);
    parse_table_add(&pt, "if", PQ_STRING, 0, &if_str, NULL);
    if (cmd == _PHY_PRBS_SET_CMD) {
        parse_table_add(&pt, "Polynomial", PQ_DFL | PQ_STRING,
                   (void *)(0), &poly_str, NULL);
        parse_table_add(&pt, "Invert", PQ_DFL | PQ_INT,
                        (void *)(0), &invert, NULL);
    }
    if (parse_arg_eq(args, &pt) < 0) {
        cli_out("Error: invalid option: %s\n", ARG_CUR(args));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    if ( poly_str ) {
        if ( !sal_strcasecmp(poly_str, "P7") || !sal_strcasecmp(poly_str, "0")) {
            poly = 0;
        } else if (!sal_strcasecmp(poly_str, "P15") || !sal_strcasecmp(poly_str, "1")  ) {
            poly = 1;
        } else if (!sal_strcasecmp(poly_str, "P23") || !sal_strcasecmp(poly_str, "2") ) {
            poly = 2;
        } else if (!sal_strcasecmp(poly_str, "P31") || !sal_strcasecmp(poly_str, "3") ) {
            poly = 3;
        } else if (!sal_strcasecmp(poly_str, "P9") || !sal_strcasecmp(poly_str, "4")) {
            poly = 4;
        } else if (!sal_strcasecmp(poly_str, "P11") || !sal_strcasecmp(poly_str, "5") ) {
            poly = 5;
        } else if (!sal_strcasecmp(poly_str, "P58") || !sal_strcasecmp(poly_str, "6")) {
            poly = 6;
#ifdef BCM_TOMAHAWK3_SUPPORT
        } else if (SOC_IS_TOMAHAWK3(unit)) {
            if (!sal_strcasecmp(poly_str, "P49") || !sal_strcasecmp(poly_str, "7")) {
                poly = 7;
            } else if (!sal_strcasecmp(poly_str, "P20") || !sal_strcasecmp(poly_str, "8")) {
                poly = 8;
            } else if (!sal_strcasecmp(poly_str, "P13") || !sal_strcasecmp(poly_str, "9")) {
                poly = 9;
            } else if (!sal_strcasecmp(poly_str, "P10") || !sal_strcasecmp(poly_str, "10")) {
                poly = 10;
            } else {
                cli_out("Prbs p must be P7(0), P15(1), P23(2), P31(3), P9(4), P11(5), P58(6), P49(7), P20(8), P13(9), or P10(10)\n");
                return CMD_FAIL;
            }
#endif /* BCM_TOMAHAWK3_SUPPORT */
        } else {
            cli_out("Prbs p must be P7(0), P15(1), P23(2), P31(3), P9(4), P11(5), or P58(6).\n");
            return CMD_FAIL;
        }
    }

    rv = (int)_phy_diag_phy_if_get(if_str, &phy_unit_if);
    if (rv == ((int)CMD_OK)) {
        rv = (int)_phy_diag_phy_unit_get(phy_unit_value, &phy_unit);
    }

    /* Now free allocated strings */
    parse_arg_eq_done(&pt);

    if (rv != CMD_OK) {
        return rv;
    }

    inst = PHY_DIAG_INSTANCE(phy_unit, phy_unit_if, PHY_DIAG_LN_DFLT);

    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
        if (cmd == _PHY_PRBS_SET_CMD || cmd == _PHY_PRBS_CLEAR_CMD) {
            max_poly_index = 6;
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                max_poly_index = 10;
            }
#endif /* BCM_TOMAHAWK3_SUPPORT */
            if (poly >= 0 && poly <= max_poly_index) {
                /* Set polynomial */
                rv = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_SET,
                                           SOC_PHY_CONTROL_PRBS_POLYNOMIAL,
                                           INT_TO_PTR(poly));
                if (rv != ((int)BCM_E_NONE)) {
                    cli_out("Setting prbs polynomial failed: %s\n",
                            bcm_errmsg(rv));
                    return CMD_FAIL;
                }
            } else {
                cli_out("Polynomial must be 0..%d.\n", max_poly_index);
                return CMD_FAIL;
            }

            /* Set invert */
            rv = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_SET,
                                       SOC_PHY_CONTROL_PRBS_TX_INVERT_DATA,
                                       INT_TO_PTR(invert));
            if (rv != ((int)BCM_E_NONE)) {
                cli_out("Setting prbs invertion failed: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }

            rv = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_SET,
                                       SOC_PHY_CONTROL_PRBS_TX_ENABLE,
                                       INT_TO_PTR(enable));
            if (rv != ((int)BCM_E_NONE)) {
                cli_out("Setting prbs tx enable failed: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }

            rv = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_SET,
                                       SOC_PHY_CONTROL_PRBS_RX_ENABLE,
                                       INT_TO_PTR(enable));
            if (rv != ((int)BCM_E_NONE)) {
                cli_out("Setting prbs rx enable failed: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
        } else {                /* _PHY_PRBS_GET_CMD */
            int status;

            rv = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_GET,
                                       SOC_PHY_CONTROL_PRBS_RX_STATUS,
                                       (void *)&status);
            if (rv != ((int)BCM_E_NONE)) {
                cli_out("Getting prbs rx status failed: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }

            switch (status) {
                case 0:
                    cli_out("%s (%2d):  PRBS OK!\n", BCM_PORT_NAME(unit, port),
                            port);
                    break;
                case -1:
                    cli_out("%s (%2d):  PRBS Failed!\n",
                            BCM_PORT_NAME(unit, port), port);
                    break;
                default:
                    cli_out("%s (%2d):  PRBS has %d errors!\n",
                            BCM_PORT_NAME(unit, port), port, status);
                    break;
            }
        }
    }
    return CMD_OK;
}

STATIC cmd_result_t
_phy_diag_mfg(int unit, bcm_pbmp_t pbmp, args_t *args)
{
#ifndef  NO_FILEIO
    parse_table_t pt;
    bcm_port_t port, dport;
    int rv, i, data_len = 0;
    char *file_name = NULL, *buffer = NULL, *buf_ptr = NULL, *dptr;
    int test, test_cmd = 0, num_ports, data;
    FILE *ofp = NULL;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Test", PQ_INT | PQ_DFL | PQ_NO_EQ_OPT, 0, &test, 0);
    parse_table_add(&pt, "Data", PQ_INT | PQ_DFL | PQ_NO_EQ_OPT, 0, &data, 0);
    parse_table_add(&pt, "File", PQ_STRING, 0, &file_name, NULL);

    if (parse_arg_eq(args, &pt) < 0) {
        cli_out("Error: invalid option: %s\n", ARG_CUR(args));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    switch (test) {
        case 1:
            test_cmd = PHY_DIAG_CTRL_MFG_HYB_CANC;
            data_len = 770 * 4;
            break;
        case 2:
            test_cmd = PHY_DIAG_CTRL_MFG_DENC;
            data_len = 44 * 4;
            break;
        case 3:
            test_cmd = PHY_DIAG_CTRL_MFG_TX_ON;
            break;
        case 0:
            test_cmd = PHY_DIAG_CTRL_MFG_EXIT;
            break;
        default:
            cli_out("Test should be : 1 (HYB_CANC), 2 (DENC), "
                    "3 (TX_ON) or 0 (EXIT)\n");
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
            break;
    }

    if (data_len) {
        if ((ofp = sal_fopen(file_name, "w")) == NULL) {
            cli_out("ERROR: Can't open the file : %s (for write)\n", file_name);
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        switch (test) {
            case 1:
                sal_fprintf(ofp, "PHY_DIAG_CTRL_MFG_HYB_CANC\n");
                break;
            case 2:
                sal_fprintf(ofp, "PHY_DIAG_CTRL_MFG_DENC\n");
                break;
        }
    }

    /* Now free allocated strings */
    parse_arg_eq_done(&pt);

    num_ports = 0;
    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
        rv = port_diag_ctrl(unit, port, 0,
                                   PHY_DIAG_CTRL_SET, test_cmd,
                                   INT_TO_PTR(data));
        if (rv != SOC_E_NONE) {
            cli_out("Error: PHY_DIAG_CTRL_SET u=%d p=%d test_cmd=%d\n", unit,
                    port, test_cmd);
        }
        num_ports++;
    }

    /* every port has an extra 32 bit scratch pad */
    buffer = sal_alloc(num_ports * (data_len + 32), "mfg_test_results");
    if (buffer == NULL) {
        cli_out("Insufficient memory.\n");
        if (ofp) {
            sal_fclose(ofp);
        }
        return CMD_FAIL;
    }
    buf_ptr = buffer;

    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
        buf_ptr[0] = 0;
        rv = port_diag_ctrl(unit, port, 0,
                                   PHY_DIAG_CTRL_GET, test_cmd,
                                   (void *)(buf_ptr + 32));
        if (rv != SOC_E_NONE) {
            cli_out("Error: PHY_DIAG_CTRL_GET u=%d p=%d test_cmd=%d\n", unit,
                    port, test_cmd);
        } else {
            buf_ptr[0] = -1;    /* indicates that the test result is valid */
        }
        buf_ptr += (data_len + 32);
    }

    if (data_len) {
        buf_ptr = buffer;
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            i = 0;
            dptr = buf_ptr + 32;
            if (buf_ptr[0]) {
                sal_fprintf(ofp, "\n\nOutput data for port %s\n",
                            BCM_PORT_NAME(unit, port));
                while (i < data_len) {
                    if ((i & 0x1f) == 0) {      /* i % 32 */
                        sal_fprintf(ofp, "\n");
                    }
                    /* data in ARM memory is big endian (network byte order) */
                    sal_fprintf(ofp, "0x%08x", soc_ntohl_load(dptr));
                    dptr += 4;
                    i += 4;
                    if (i >= data_len) {
                        sal_fprintf(ofp, "\n");
                        break;
                    } else {
                        sal_fprintf(ofp, ", ");
                    }
                }
            } else {
                sal_fprintf(ofp, "\n\nTest failed for port %s\n",
                            BCM_PORT_NAME(unit, port));
            }
            buf_ptr += (data_len + 32);
        }
    }

    if (ofp) {
        sal_fclose(ofp);
    }
    sal_free(buffer);

    return CMD_OK;
#else
    cli_out("This command is not supported without file I/O\n");
    return CMD_FAIL;
#endif
}

STATIC cmd_result_t
_phy_diag_state(int unit, bcm_pbmp_t pbmp, args_t *args)
{
#ifndef  NO_FILEIO
    parse_table_t pt;
    bcm_port_t port, dport;
    int rv, i, data_len = 0;
    char *file_name = NULL, *buffer = NULL, *buf_ptr = NULL, *dptr;
    XGPHY_DIAG_DATA_t *diag_dptr;
    int test, test_cmd = 0, num_ports, data;
    FILE *ofp = NULL;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Test", PQ_INT | PQ_DFL | PQ_NO_EQ_OPT, 0, &test, 0);
    parse_table_add(&pt, "Data", PQ_INT | PQ_DFL | PQ_NO_EQ_OPT, 0, &data, 0);
    parse_table_add(&pt, "File", PQ_STRING, 0, &file_name, NULL);

    if (parse_arg_eq(args, &pt) < 0) {
        cli_out("Error: invalid option: %s\n", ARG_CUR(args));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    switch (test) {
        case 1:
            test_cmd = PHY_DIAG_CTRL_STATE_TRACE1;
            data_len = 2048 * 4;
            break;
        case 2:
            test_cmd = PHY_DIAG_CTRL_STATE_TRACE2;
            data_len = 1024 * 4;
            break;
        case 3:
            test_cmd = PHY_DIAG_CTRL_STATE_WHEREAMI;
            data_len = sizeof(XGPHY_DIAG_DATA_t);
            break;
        case 4:
            test_cmd = PHY_DIAG_CTRL_STATE_TEMP;
            data_len = sizeof(XGPHY_DIAG_DATA_t);
            break;
        case 5:
            test_cmd = PHY_DIAG_CTRL_STATE_GENERIC;
            break;
        default:
            cli_out("Test should be : 1 (STATE_TRACE1), 2 (STATE_TRACE2), "
                    "3 (WHERE_AM_I), 4 (TEMP)\n");
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
            break;
    }

    if (data_len) {
        if ((ofp = sal_fopen(file_name, "a+")) == NULL) {
            cli_out("ERROR: Can't open the file : %s\n", file_name);
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        sal_fprintf(ofp,
                   "\n-------------------------------------------------------"
                   "------------------------------------------------------\n");
        switch (test) {
            case 1:
                sal_fprintf(ofp, "PHY_DIAG_CTRL_STATE_TRACE1\n");
                break;
            case 2:
                sal_fprintf(ofp, "PHY_DIAG_CTRL_STATE_TRACE2\n");
                break;
            case 3:
                sal_fprintf(ofp, "PHY_DIAG_CTRL_STATE_WHERE_AM_I\n");
                break;
            case 4:
                sal_fprintf(ofp, "PHY_DIAG_CTRL_STATE_TEMP\n");
                break;
        }
    }

    /* Now free allocated strings */
    parse_arg_eq_done(&pt);

    num_ports = 0;
    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
        rv = port_diag_ctrl(unit, port, 0,
                            PHY_DIAG_CTRL_SET, test_cmd,
                            INT_TO_PTR(data));
        if (rv != SOC_E_NONE) {
            cli_out("Error: PHY_DIAG_CTRL_SET u=%d p=%d test_cmd=%d\n", unit,
                    port, test_cmd);
        }
        num_ports++;
    }

    /* every port has an extra 32 bit scratch pad */
    buffer = sal_alloc(num_ports * (data_len + 32), "state_test_results");
    if (buffer == NULL) {
        cli_out("Insufficient memory.\n");
        if (ofp) {
            sal_fclose(ofp);
        }
        return CMD_FAIL;
    }
    buf_ptr = buffer;

    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
        buf_ptr[0] = 0;
        rv = port_diag_ctrl(unit, port, 0,
                            PHY_DIAG_CTRL_GET, test_cmd,
                            (void *)(buf_ptr + 32));
        if (rv != SOC_E_NONE) {
            cli_out("Error: PHY_DIAG_CTRL_GET u=%d p=%d test_cmd=%d\n", unit,
                    port, test_cmd);
        } else {
            buf_ptr[0] = -1;    /* indicates that the test result is valid */
        }
        buf_ptr += (data_len + 32);
    }

    if (data_len) {
        buf_ptr = buffer;
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            i = 0;
            dptr = buf_ptr + 32;
            diag_dptr = (XGPHY_DIAG_DATA_t *) dptr;

            if (buf_ptr[0]) {
                sal_fprintf(ofp, "\n\nOutput data for port %s\n",
                            BCM_PORT_NAME(unit, port));
                if (test_cmd == PHY_DIAG_CTRL_STATE_WHEREAMI) {
                    int32 valA, valB, valC, valD, statA, statB, statC, statD;

                    if (diag_dptr->flags & PHY_DIAG_FULL_SNR_SUPPORT) {
                        valA =
                            soc_letohl_load(&diag_dptr->snr_block[16]) - 32768;
                        valB =
                            soc_letohl_load(&diag_dptr->snr_block[20]) - 32768;
                        valC =
                            soc_letohl_load(&diag_dptr->snr_block[24]) - 32768;
                        valD =
                            soc_letohl_load(&diag_dptr->snr_block[28]) - 32768;

                        statA = soc_letohl_load(&diag_dptr->snr_block[0]);
                        statB = soc_letohl_load(&diag_dptr->snr_block[4]);
                        statC = soc_letohl_load(&diag_dptr->snr_block[8]);
                        statD = soc_letohl_load(&diag_dptr->snr_block[12]);

                        sal_fprintf(ofp,
                                    "\nsnrA = %d.%d (%s) snrB = %d.%d (%s) "
                                    "snrC = %d.%d (%s) snrD = %d.%d (%s) "
                                    "serr = %d cerr = %d block_lock = %d "
                                    "block_point_id = %d\n",
                                    valA / 10, valA % 10,
                                    statA ? "OK" : "Not OK", valB / 10,
                                    valB % 10, statB ? "OK" : "Not OK",
                                    valC / 10, valC % 10,
                                    statC ? "OK" : "Not OK", valD / 10,
                                    valD % 10, statD ? "OK" : "Not OK",
                                    diag_dptr->serr, diag_dptr->cerr,
                                    diag_dptr->block_lock,
                                    diag_dptr->block_point_id);

                    } else {
                        valA = soc_letohl_load(&diag_dptr->snr_block[0]);
                        valB = soc_letohl_load(&diag_dptr->snr_block[4]);
                        valC = soc_letohl_load(&diag_dptr->snr_block[8]);
                        valD = soc_letohl_load(&diag_dptr->snr_block[12]);

                        sal_fprintf(ofp,
                                    "\nmseA = %d mseB = %d mseC = %d mseD = %d "
                                    "serr = %d cerr = %d block_lock = 0x%x "
                                    "block_point_id = 0x%x\n",
                                    valA, valB, valC, valD, diag_dptr->serr,
                                    diag_dptr->cerr, diag_dptr->block_lock,
                                    diag_dptr->block_point_id);
                    }
                } else if (test_cmd == PHY_DIAG_CTRL_STATE_TEMP) {
                    sal_fprintf(ofp, "\nTemperature = %d C,  %d F\n",
                                diag_dptr->digital_temp,
                                (diag_dptr->digital_temp * 9) / 5 + 32);
                } else {
                    while (i < data_len) {
                        if ((i & 0x1f) == 0) {  /* i % 32 */
                            sal_fprintf(ofp, "\n");
                        }
                        sal_fprintf(ofp, "0x%08x", soc_letohl_load(&dptr[0]));
                        dptr += 4;
                        i += 4;
                        if (i >= data_len) {
                            sal_fprintf(ofp, "\n");
                            break;
                        } else {
                            sal_fprintf(ofp, ", ");
                        }
                    }
                }
            } else {
                sal_fprintf(ofp, "\n\nTest failed for port %s\n",
                            BCM_PORT_NAME(unit, port));
            }
            buf_ptr += (data_len + 32);
        }
    }

    if (ofp) {
        sal_fclose(ofp);
    }
    sal_free(buffer);

    return CMD_OK;
#else
    cli_out("This command is not supported without file I/O\n");
    return CMD_FAIL;
#endif
}

STATIC cmd_result_t
_phy_diag_fast_eyescan(int unit, bcm_pbmp_t pbmp, args_t *args)
{
    parse_table_t pt;
    bcm_port_t port, dport;
    int rv=0,res=0;
    char *if_str;
    int phy_unit = 0, phy_unit_value = 0, phy_unit_if = 0;
    int lane_num = 0;
    uint32 inst;

    parse_table_init(unit, &pt);

    /*
     * unit:  phy_unit_value: phy chain number
     * if  :  if_str="sys"|"line"
     */
    parse_table_add(&pt, "unit", PQ_DFL | PQ_INT, (void *)(0),
                    &phy_unit_value, NULL);
    parse_table_add(&pt, "if", PQ_STRING, 0, &if_str, NULL);
    parse_table_add(&pt, "lane", PQ_DFL | PQ_INT, (void *)(0), &lane_num, NULL);

    if (parse_arg_eq(args, &pt) < 0) {
        cli_out("Error: invalid option: %s\n", ARG_CUR(args));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    rv = _phy_diag_phy_if_get(if_str, &phy_unit_if);
    if (rv == CMD_OK) {
        rv = _phy_diag_phy_unit_get(phy_unit_value, &phy_unit);
    }

    /* Now free allocated strings */
    parse_arg_eq_done(&pt);

    if (rv != CMD_OK) {
        return rv;
    }

    inst =
        PHY_DIAG_INSTANCE(phy_unit, phy_unit_if,
                          (lane_num == 0xff) ? PHY_DIAG_LN_DFLT : lane_num);

    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
        res = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_CMD,
                                   PHY_DIAG_CTRL_START_FAST_EYESCAN, (void *)0);
        if (res != BCM_E_NONE) {
            return CMD_FAIL;
        }
    }
    return CMD_OK;
}

STATIC cmd_result_t
_phy_diag_link_mon(int unit, bcm_pbmp_t pbmp, args_t *args)
{
    parse_table_t pt;
    bcm_port_t port, dport;
    int rv;
    char *cmd_str, *if_str;
    int phy_unit = 0, phy_unit_value = 0, phy_unit_if = 0;
    int lane_num = 0;
    uint32 link_mon_mode = 0, cmd;
    uint32 inst;

    enum { _PHY_LINKMON_SET_CMD, _PHY_LINKMON_GET_CMD};
    parse_table_init(unit, &pt);

    if ((cmd_str = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    if (sal_strcasecmp(cmd_str, "set") == 0) {
        cmd = _PHY_LINKMON_SET_CMD;
    } else if (sal_strcasecmp(cmd_str, "get") == 0) {
        cmd = _PHY_LINKMON_GET_CMD;
    } else {
        return CMD_USAGE;
    }

    /*
     * unit:  phy_unit_value: phy chain number
     * if  :  if_str="sys"|"line"
     */
    parse_table_add(&pt, "unit", PQ_DFL | PQ_INT, (void *)(0),
                    &phy_unit_value, NULL);
    parse_table_add(&pt, "if", PQ_STRING, 0, &if_str, NULL);
    parse_table_add(&pt, "lane", PQ_DFL | PQ_INT, (void *)(0), &lane_num, NULL);
    if (cmd == _PHY_LINKMON_SET_CMD) {
        parse_table_add(&pt, "mode", PQ_DFL | PQ_INT, (void *)(0), &link_mon_mode, NULL);
    }

    if (parse_arg_eq(args, &pt) < 0) {
        cli_out("Error: invalid option: %s\n", ARG_CUR(args));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    rv = (int)_phy_diag_phy_if_get(if_str, &phy_unit_if);
    if (rv == ((int)CMD_OK)) {
        rv = (int)_phy_diag_phy_unit_get(phy_unit_value, &phy_unit);
    }

    /* Now free allocated strings */
    parse_arg_eq_done(&pt);

    if (rv != ((int)CMD_OK)) {
        return rv;
    }

    inst = PHY_DIAG_INSTANCE(phy_unit, phy_unit_if, lane_num);

    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {

#ifdef PORTMOD_SUPPORT
        if (SOC_PORT_USE_PORTCTRL(unit, port)) {
            if (cmd == _PHY_LINKMON_SET_CMD) {
                rv = portmod_port_diag_ctrl( unit, port, inst, PHY_DIAG_CTRL_CMD,
                                            PHY_DIAG_CTRL_LINKMON_MODE, INT_TO_PTR(link_mon_mode));
            } else {
                rv = portmod_port_diag_ctrl( unit, port, inst, PHY_DIAG_CTRL_CMD,
                                            PHY_DIAG_CTRL_LINKMON_STATUS,(void *)0);
            }
        } else
#endif
        {

            if (cmd == _PHY_LINKMON_SET_CMD) {
                rv = soc_phyctrl_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_CMD,
                                           PHY_DIAG_CTRL_LINKMON_MODE, INT_TO_PTR(link_mon_mode));
            } else {
                rv = soc_phyctrl_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_CMD,
                                           PHY_DIAG_CTRL_LINKMON_STATUS, (void *)0);
            }
        }

        if (rv != ((int)BCM_E_NONE)) {
            return CMD_FAIL;
        }
    }
    return CMD_OK;
}

STATIC int
_phy_diag_eyescan_res_print(int unit,
                            int speed,
                            soc_port_t port,
                            int sample_res,
                            soc_port_phy_eye_bounds_t *bounds,
                            soc_port_phy_eyescan_results_t *res)
{

    int vt_i, hz_i, legend_ndx = 0, legend_ndx_offset = 0;
    int hz_min, hz_max, vt_min, vt_max;
    const char legend[] = { ' ', '.', 'o', 'b', 'P', 'D', 'B', 'M', 'W', 'X' };
    const char legend_low_ber[] =
        { ' ', 'X', 'W', 'M', 'B', 'D', 'P', 'b', 'o', '.', 'Y', 'Z', 'A' };
    const char error_char = '!';
    const char invalid_char = '?';
    int desc_idx = 0;
    unsigned int error_cnt;
    unsigned int time_tot;
    cmd_result_t ret = CMD_OK;
    phy_ctrl_t *pc;

#if defined(BROADCOM_DEBUG)     /*required for compilation */
    char *description[] = {
        "   Description:",
        "   =============",
        "   '+'  Latch",
        "   '?'  No data"
    };
#endif                          /*BROADCOM_DEBUG */
    int desc_size = 4;

#if defined(BROADCOM_DEBUG)     /*required for compilation */
    char *desc_ber[] = {
        "   '%c' BER <10-9",
        "   '%c' BER ~10-9",
        "   '%c' BER ~10-8",
        "   '%c' BER ~10-7",
        "   '%c' BER ~10-6",
        "   '%c' BER ~10-5",
        "   '%c' BER ~10-4",
        "   '%c' BER ~10-3",
        "   '%c' BER ~10-2",
        "   '%c' BER >10-2"
    };
    char *desc_low_ber[] = {
        "   '%c' BER <10-12",
        "   '%c' BER >10-2",
        "   '%c' BER ~10-2",
        "   '%c' BER ~10-3",
        "   '%c' BER ~10-4",
        "   '%c' BER ~10-5",
        "   '%c' BER ~10-6",
        "   '%c' BER ~10-7",
        "   '%c' BER ~10-8",
        "   '%c' BER ~10-9",
        "   '%c' BER ~10-10",
        "   '%c' BER ~10-11",
        "   '%c' BER ~10-12"
    };
#endif                          /*BROADCOM_DEBUG */
    int desc_ber_size = 10;
    int desc_low_ber_size = 13;


    const unsigned int nof_legend_chars = sizeof(legend) / sizeof(char);
    const unsigned int nof_legend_low_ber_chars =
        sizeof(legend_low_ber) / sizeof(char);
    char print_char;
    unsigned int ber;
    char buf[5];
    unsigned int rev_ber = 0;
    unsigned int bits_xferred = 0;
    unsigned int gspeed = 0;    /* Speed in Gbps */
    unsigned int speed_n = 0;   /* Normalized speed */
    unsigned int time_n = 0;    /* Normalized time */

    pc = EXT_PHY_SW_STATE(unit, port);

    if (!BCM_UNIT_VALID(unit)) {
        ret = CMD_FAIL;
        goto exit;
    }

    /*psrsmeters verification */
    if (0 == bounds) {
        ret = CMD_FAIL;
        goto exit;
    }

    hz_min = bounds->horizontal_min;
    hz_max = bounds->horizontal_max;
    vt_min = bounds->vertical_min;
    vt_max = bounds->vertical_max;

    if (hz_max < hz_min) {
        ret = CMD_FAIL;
        goto exit;
    }
    if (vt_max < vt_min) {
        ret = CMD_FAIL;
        goto exit;
    }

    if (0 == res) {
        ret = CMD_FAIL;
        goto exit;
    }

    /*Print results log */
    for (vt_i = vt_min; vt_i <= vt_max; vt_i++) {
        if (vt_i % sample_res != 0) {
            continue;
        }
        for (hz_i = hz_min; hz_i <= hz_max; hz_i++) {
            if (hz_i % sample_res != 0) {
                continue;
            }

            cli_out("[H=%d, V=%d] %u errors in %d milisec \r\n", hz_i, vt_i,
                    res->error_count[hz_i + SOC_PORT_PHY_EYESCAN_H_INDEX]
                    [vt_i + SOC_PORT_PHY_EYESCAN_V_INDEX],
                    res->run_time[hz_i + SOC_PORT_PHY_EYESCAN_H_INDEX]
                    [vt_i + SOC_PORT_PHY_EYESCAN_V_INDEX]);
        }
    }

    if (res->ext_done) {
        switch (res->ext_better) {
            case -1:
                cli_out("BER(extrapolated) is *worse* than 1e-%d.%d\n",
                        res->ext_results_int, res->ext_results_remainder);
                break;
            case 1:
                cli_out("BER(extrapolated) is *better* than 1e-%d.%d\n",
                        res->ext_results_int, res->ext_results_remainder);
                break;
            default:
                cli_out("BER(extrapolated) = 1e-%d.%d\n", res->ext_results_int,
                        res->ext_results_remainder);
                break;
        }
    }

    if (hz_max != hz_min) {
        cli_out("\r\n[VERTICAL]-------------------------------"
                "------------------------\r\n");
        for (vt_i = vt_max; vt_i >= vt_min; vt_i--) {
            if (vt_i % sample_res != 0) {
                continue;
            }

            /* Print Y axis and values */
            if ((((vt_i - vt_min) / sample_res) % 5 == 0)) {
                cli_out("%3d-+", vt_i);
            } else {
                cli_out("    |");
            }

            for (hz_i = hz_min; hz_i <= hz_max; hz_i++) {

                if (hz_i % sample_res != 0) {
                    continue;
                }

                print_char = error_char;
                error_cnt =
                    res->error_count[hz_i + SOC_PORT_PHY_EYESCAN_H_INDEX]
                                    [vt_i + SOC_PORT_PHY_EYESCAN_V_INDEX];
                time_tot =
                    res->run_time[hz_i + SOC_PORT_PHY_EYESCAN_H_INDEX]
                                 [vt_i + SOC_PORT_PHY_EYESCAN_V_INDEX];

                if (time_tot == 0 && error_cnt == 0) {
                    print_char = invalid_char;
                } else {
                    /*
                     * Eyescan calculation for Low BER
                     * -------------------------------
                     *   Standard/Existing eyescan plot function was supporting
                     * upto BER 10^-9 below logic is implemented for plotting
                     * eyescan for BER 10^-10, 10^-11, 10^-12 for satisfying
                     * low BER requirements. Currently this logic is implemented
                     * for Gallardo40 to fix the JIRA PHY-1156.
                     *      BER = #bit errors/#bit transferred
                     * For testing low BER we need to run eyescan for long
                     * sample time. For Ex: We may have to run eyescan for 10E5
                     * milli sec with 10G traffic to see BER 10^-12
                     * i.e., #bits transferred in 10^5 millisec = 10^12
                     * if #bit errors = 1, BER = 1 / 10^12 = 10^-12
                     * The below logic is implemented using uint32 so as to
                     * avoid the usage of uint64, Float, math library functions
                     * such as log, abs, ceil, floor these have performance
                     * issues and prone to introduce build issues in our SDK
                     * environment. uint32 can store max 2^32
                     * (cannot store 10^12), and actual BER requires
                     * Float (#biterrors/#bits transferred, We calculate
                     * Reverse BER, calculating index of legend_low_ber by
                     * calculating no of digits present in reverse BER to print
                     * the char for plotting eyescan
                     *          rev_ber = bits_xferred / error_cnt;
                     *          legend_ndx = #digits(rev_ber);
                     *          print_char = legend_low_ber[legend_ndx];
                     * Below is the map between print_char and BER
                     * =============
                     * '+'  Latch
                     * '?'  No data
                     * ' ' BER <10-12
                     * 'X' BER >10-2
                     * 'W' BER ~10-2
                     * 'M' BER ~10-3
                     * 'B' BER ~10-4
                     * 'D' BER ~10-5
                     * 'P' BER ~10-6
                     * 'b' BER ~10-7
                     * 'o' BER ~10-8
                     * '.' BER ~10-9
                     * 'Y' BER ~10-10
                     * 'Z' BER ~10-11
                     * 'A' BER ~10-12
                     */
                    if (pc && pc->phy_id0 == 0x600d && pc->phy_id1 == 0x8500) {
                        if (time_tot == 0) {
                            legend_ndx = 0;
                        } else {
                            time_n = 1;
                            while (time_n <= 1000000) {
                                if (time_tot <= time_n) {
                                    break;
                                }
                                time_n *= 10;
                            }

                            gspeed = speed / 1000;
                            speed_n = 1;
                            while (speed_n <= 100) {
                                if (gspeed <= speed_n) {
                                    break;
                                }
                                speed_n *= 10;
                            }

                            switch (time_n * speed_n) {
                                case 1:
                                case 10:
                                case 100:
                                case 1000:
                                    bits_xferred = gspeed * 1000000 * time_tot;
                                    legend_ndx_offset = 0;
                                    break;
                                case 10000:
                                    bits_xferred = gspeed * 100000 * time_tot;
                                    legend_ndx_offset = 1;
                                    break;
                                case 100000:
                                    bits_xferred = gspeed * 10000 * time_tot;
                                    legend_ndx_offset = 2;
                                    break;
                                case 1000000:
                                    bits_xferred = gspeed * 1000 * time_tot;
                                    legend_ndx_offset = 3;
                                    break;
                                default:
                                    bits_xferred = gspeed * 100 * time_tot;
                                    legend_ndx_offset = 4;
                                break;
                            }

                            if (error_cnt == 0) {
                                legend_ndx = 0;
                            } else {
                                rev_ber = bits_xferred / error_cnt;
                                if (rev_ber == 0) {
                                    legend_ndx = 1;
                                } else {
                                    legend_ndx = 0;
                                    while (rev_ber > 0) {
                                        rev_ber /= 10;
                                        legend_ndx++;
                                    }
                                    if (legend_ndx > 1) {
                                        legend_ndx =
                                            legend_ndx + legend_ndx_offset - 1;
                                    }
                                }
                            }
                        }

                        /* If exceeds legend size, round legend_ndx down */
                        if (legend_ndx >= nof_legend_low_ber_chars) {
                            legend_ndx = 0;
                        }
                        print_char = legend_low_ber[legend_ndx];
                    } else {
                        if (time_tot == 0) {
                            ber = 0;
                        } else if (error_cnt < (0xffffffff / 1000)) {
                            /* multiplying by 1000 won't cause an overflow */
                            ber = (error_cnt * 1000 / time_tot);
                        } else {
                            /* number is big enough,
                               so can first divide by time and than multiply */
                            ber = (error_cnt / time_tot * 1000);
                        }

                        /* Print matrix entries */
                        if (ber == SRD_EYESCAN_INVALID) {
                           /* print_char = invalid_char; */
                        } else {

                            legend_ndx = 0;
                            while (ber > 0) {
                                ber /= 10;
                                legend_ndx++;
                            }

                            /* If exceeds legend size, round legend_ndx down */
                            if (legend_ndx >= nof_legend_chars) {
                                legend_ndx = (nof_legend_chars - 1);
                            }
                        }

                        print_char = legend[legend_ndx];
                    }
                }

                cli_out("%c", print_char);
            }

            cli_out("|");

            /* print description on relevant lines */
            if (pc && pc->phy_id0 == 0x600d && pc->phy_id1 == 0x8500) {
                if (desc_idx < desc_low_ber_size + desc_size) {
#if defined(BROADCOM_DEBUG)
                    if (desc_idx < desc_size) {
                        cli_out("%s", description[desc_idx]);
                    } else {
                        cli_out(desc_low_ber[desc_idx - desc_size],
                                legend_low_ber[desc_idx - desc_size]);
                    }
#endif
                    desc_idx++;
                }
            } else {
                if (desc_idx < desc_ber_size + desc_size) {
#if defined(BROADCOM_DEBUG)
                    if (desc_idx < desc_size) {
                        cli_out("%s", description[desc_idx]);
                    } else {
                        cli_out(desc_ber[desc_idx - desc_size],
                                legend[desc_idx - desc_size]);
                    }
#endif
                    desc_idx++;
                }
            }
            cli_out("\r\n");
        }

        /* print x axis */
        cli_out("    -");

        for (hz_i = hz_min; hz_i < hz_max - 9 * sample_res; hz_i++) {
            if (hz_i % sample_res != 0) {
                continue;
            }
            cli_out("%c",
                    ((((hz_i - hz_min) / sample_res) % 5) ? '-' : '+'));
        }

        cli_out("[HORIZONTAL]\r\n");
        cli_out("     ");

        for (hz_i = hz_min; hz_i <= hz_max; hz_i++) {
            if (hz_i % sample_res != 0) {
                continue;
            }
            cli_out("%c",
                    ((((hz_i - hz_min) / sample_res) % 5) ? ' ' : '|'));
        }

        cli_out("\r\n");
        cli_out("     ");
        for (hz_i = hz_min; hz_i <= hz_max; hz_i++) {
            if (hz_i % sample_res != 0) {
                continue;
            }
            if (((hz_i - hz_min) / sample_res) % 5) {
                cli_out(" ");
            } else {
                cli_out("%2d", hz_i);
                sal_sprintf(buf, "%2d", hz_i);
                hz_i += ((sal_strlen(buf) - 1) * sample_res);
            }
        }
        cli_out("\r\n");
    }
 exit:
    return ret;
}

cmd_result_t
_phy_diag_reg(int unit, bcm_pbmp_t pbmp, args_t *args)
{
    bcm_port_t port, dport;
    int cmd;
    char *cmd_str/*, *core_str*/;
    uint32 *pData, par[5];

    par[0] = 0;
    par[1] = 0;
    par[2] = 0;
    par[3] = 0;
    par[4] = 0;
    pData = &par[0];

    cmd = PHY_DIAG_CTRL_REG_READ;

    if ((cmd_str = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    if (cmd_str) {
        if (sal_strcasecmp(cmd_str, "core0") == 0) {
            *pData++ = 0;
        } else if (sal_strcasecmp(cmd_str, "core1") == 0) {
            *pData++ = 1;
        } else if (sal_strcasecmp(cmd_str, "core2") == 0) {
            *pData++ = 2;
        } else if (sal_strcasecmp(cmd_str, "mld") == 0) {
            *pData++ = 3;
        }
    }
    if ((cmd_str = ARG_GET(args)) == NULL) { /* Get aer number */
        return CMD_USAGE;
    }
    *pData++ = sal_strtoul(cmd_str, NULL, 0);

    if ((cmd_str = ARG_GET(args)) == NULL) { /* Get reg number */
        return CMD_USAGE;
    }
    *pData++ = sal_strtoul(cmd_str, NULL, 0);

    if ((cmd_str = ARG_GET(args)) != NULL) { /* Get write data */
        cmd = PHY_DIAG_CTRL_REG_WRITE;
        *pData++ = sal_strtoul(cmd_str, NULL, 0);
    }

    /* command targets to Internal serdes device for now */
    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
        if (port_diag_ctrl(unit, port, PHY_DIAG_INT,
                                  PHY_DIAG_CTRL_CMD, cmd,
                                  &par) != SOC_E_NONE) {
            return CMD_FAIL;
        }
    }
    return CMD_OK;
}

cmd_result_t
_phy_diag_eyescan(int unit, bcm_pbmp_t pbmp, args_t *args)
{
    int i, speed, port_count;
    cmd_result_t ret = CMD_OK;
    soc_port_phy_eyescan_params_t params;
    soc_port_phy_eyescan_results_t *results;
    int phy_unit, phy_unit_if, phy_unit_value = 0;
    parse_table_t pt;
    int rv, flags = 0;
    uint32 nof_ports;
    uint32 inst;
    char *if_str;
    soc_port_t *ports, port, dport;
    int *local_lane_num, lane_num = 0xff;
    char *eyescan_counters_strs[socPortPhyEyescanNofCounters * 2 + 1];
    char numbers_strs[socPortPhyEyescanNofCounters][3];

    for (i = 0; i < socPortPhyEyescanNofCounters; i++) {
        sal_sprintf(numbers_strs[i], "%d", i);
        eyescan_counters_strs[i] = eyescan_counter[i];
        eyescan_counters_strs[socPortPhyEyescanNofCounters + i] =
            numbers_strs[i];
    }
    eyescan_counters_strs[socPortPhyEyescanNofCounters * 2] = NULL;

    BCM_PBMP_COUNT(pbmp, port_count);
    if (port_count == 0) {
        return CMD_OK;
    }

    results = sal_alloc((port_count) * sizeof(soc_port_phy_eyescan_results_t),
                        "eyescan results array");
    /*array of result per port */
    if (results == NULL) {
        cli_out("ERROR, in phy_diag_eyescan: failed to allocate results");
        return CMD_FAIL;
    }
    ports = sal_alloc((port_count) * sizeof(soc_port_t), "eyescan ports");
    if (ports == NULL) {
        cli_out("ERROR, in phy_diag_eyescan: failed to allocate ports");
        sal_free(results);
        return CMD_FAIL;
    }
    local_lane_num =
        sal_alloc((port_count) * sizeof(int), "eyescan lane_num array");
    if (local_lane_num == NULL) {
        cli_out("ERROR, in phy_diag_eyescan: failed to allocate local_lane_num");
        sal_free(results);
        sal_free(ports);
        return CMD_FAIL;
    }

    /*default values */
    params.sample_time = 1000;
    params.sample_resolution = 1;
    params.bounds.horizontal_max = 31;
    params.bounds.horizontal_min = -31;
    params.bounds.vertical_max = 31;
    params.bounds.vertical_min = -31;
    params.counter = socPortPhyEyescanCounterRelativePhy;
    params.error_threshold = 20;
    params.time_upper_bound = 256000;
    params.type = -1;
    /* defaults for BER projection */
    params.ber_proj_scan_mode = 0;  /* DIAG_BER_VERT DIAG_BER_POS */
    params.ber_proj_timer_cnt = 96;
    params.ber_proj_err_cnt = 8;

    sal_memset(results, 0, sizeof(soc_port_phy_eyescan_results_t) * port_count);

    nof_ports = 0;
    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
        ports[nof_ports] = port;
        nof_ports++;
    }

    params.nof_threshold_links = nof_ports / 2;
    if (params.nof_threshold_links < 1) {
        params.nof_threshold_links = 1;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "type", PQ_DFL | PQ_INT, (void *)(0),
                    &(params.type), NULL);
    parse_table_add(&pt, "vertical_max", PQ_DFL | PQ_INT, (void *)(0),
                    &(params.bounds.vertical_max), NULL);
    parse_table_add(&pt, "vertical_min", PQ_DFL | PQ_INT, (void *)(0),
                    &(params.bounds.vertical_min), NULL);
    parse_table_add(&pt, "horizontal_max", PQ_DFL | PQ_INT, (void *)(0),
                    &(params.bounds.horizontal_max), NULL);
    parse_table_add(&pt, "horizontal_min", PQ_DFL | PQ_INT, (void *)(0),
                    &(params.bounds.horizontal_min), NULL);
    parse_table_add(&pt, "sample_resolution", PQ_DFL | PQ_INT, (void *)(0),
                    &(params.sample_resolution), NULL);
    parse_table_add(&pt, "sample_time", PQ_DFL | PQ_INT, (void *)(0),
                    &(params.sample_time), NULL);
    parse_table_add(&pt, "counter", PQ_MULTI | PQ_DFL | PQ_NO_EQ_OPT,
                    (void *)(0), &(params.counter), &eyescan_counters_strs);
    parse_table_add(&pt, "flags", PQ_DFL | PQ_INT, (void *)(0), &flags, NULL);
    parse_table_add(&pt, "lane", PQ_DFL | PQ_INT, (void *)(0), &lane_num, NULL);
    parse_table_add(&pt, "error_threshold", PQ_DFL | PQ_INT, (void *)(0),
                    &(params.error_threshold), NULL);
    parse_table_add(&pt, "time_upper_bound", PQ_DFL | PQ_INT, (void *)(0),
                    &(params.time_upper_bound), NULL);
    parse_table_add(&pt, "nof_threshold_links", PQ_DFL | PQ_INT, (void *)(0),
                    &(params.nof_threshold_links), NULL);
    parse_table_add(&pt, "unit", PQ_DFL | PQ_INT, (void *)(0),
                    &phy_unit_value, NULL);
    parse_table_add(&pt, "if", PQ_STRING, 0, &if_str, NULL);
    /* BER projection related parameter */
    parse_table_add(&pt, "ber_scan_mode", PQ_DFL | PQ_INT, (void *)(0),
                    &(params.ber_proj_scan_mode), NULL);
    parse_table_add(&pt, "timer_control", PQ_DFL | PQ_INT, (void *)(0),
                    &(params.ber_proj_timer_cnt), NULL);
    parse_table_add(&pt, "max_err_control", PQ_DFL | PQ_INT, (void *)(0),
                    &(params.ber_proj_err_cnt), NULL);

    if (parse_arg_eq(args, &pt) < 0) {
        cli_out("ERROR: could not parse parameters\n");
        parse_arg_eq_done(&pt);
        ret = CMD_FAIL;
        goto exit;
    }

    if (ARG_CNT(args) > 0) {
        cli_out("%s: Unknown argument %s\n", ARG_CMD(args), ARG_CUR(args));
        parse_arg_eq_done(&pt);
        ret = CMD_FAIL;
        goto exit;
    }
    params.counter = params.counter % socPortPhyEyescanNofCounters;
    if(params.type == -1) {  /* -1 is un-specified, so set to 1 as default */
        params.type = 1 ;
    }                        /*  params.type !=0 means phymod eyescan      */

    phy_unit = 0;
    rv = _phy_diag_phy_if_get(if_str, &phy_unit_if);
    if (rv != ((int)CMD_OK)) {
        parse_arg_eq_done(&pt);
        ret = CMD_FAIL;
        goto exit;
    }
    rv = _phy_diag_phy_unit_get(phy_unit_value, &phy_unit);
    if (rv != ((int)CMD_OK)) {
        parse_arg_eq_done(&pt);
        ret = CMD_FAIL;
        goto exit;
    }

    /* inst has unit, intf, lane */
    inst = PHY_DIAG_INSTANCE(phy_unit, phy_unit_if,
                             lane_num == 0xff ? PHY_DIAG_LN_DFLT : lane_num);

    for (i = 0; i < nof_ports; i++) {
        results[i].ext_done = 0;
    }

    if (lane_num == 0xff) {
        if (local_lane_num != NULL) {
            sal_free(local_lane_num);
        }
        local_lane_num = NULL;
    } else {
            /*now check the correct lane num input for each port */
#ifndef PORTMOD_SUPPORT
        for (i = 0; i < nof_ports; i++) {
            if (lane_num > SOC_INFO(unit).port_num_lanes[ports[i]]) {
                cli_out("ERROR, in phy_diag_eyescan: lane num is wrong \n");
                return CMD_FAIL;
            }
        }
#endif

        for(i = 0; i < port_count; i++) {
            sal_memcpy(&local_lane_num[i], &lane_num, sizeof(int));
        }
    }

    if ((params.sample_time < 0) || (params.sample_time > params.time_upper_bound)) {
        cli_out("ERROR, in phy_diag_eyescan: sample time %d"
                " out of range ( 0 ~ %d )\n",
                params.sample_time, params.time_upper_bound);
        ret = CMD_FAIL;
        goto exit;
    }

    rv = soc_port_phy_eyescan_run(unit, inst, flags, &params, nof_ports, ports,
                                  local_lane_num, results);
#ifdef PHYMOD_SUPPORT
    if (!(is_eyescan_algorithm_legacy_mode(unit, nof_ports, ports, inst)) || rv){
        ret = (rv == SOC_E_NONE)? CMD_OK : CMD_FAIL;
        goto exit;
    }
#endif

#ifdef PHYMOD_SUPPORT
    if(is_eyescan_algorithm_legacy_rpt_mode(unit, nof_ports, ports, inst))
#endif
    {
        rv = soc_port_phy_eyescan_extrapolate(unit, flags, &params, nof_ports,
                                              ports, results);

        if (rv != SOC_E_NONE) {
            ret = CMD_FAIL;
            goto exit;
        }

        for (i = 0; i < nof_ports; i++) {

            rv = bcm_port_speed_get(unit, ports[i], &speed);
            if (rv != SOC_E_NONE) {
                ret = CMD_FAIL;
                goto exit;
            }

            cli_out("Eye\\Cross-section Results For Port %d (with rate %d):\n",
                    ports[i], speed);

            rv = _phy_diag_eyescan_res_print(unit, speed, ports[i],
                                             params.sample_resolution,
                                             &(params.bounds), &(results[i]));

            if (rv != SOC_E_NONE) {
                ret = CMD_FAIL;
                goto exit;
            }
        }
    }
    ret = CMD_OK;

 exit:
    if (results != NULL) {
        sal_free(results);
    }
    if (ports != NULL) {
        sal_free(ports);
    }
    if (local_lane_num != NULL) {
        sal_free(local_lane_num);
    }
    return ret;
}

cmd_result_t
_phy_diag_berproj(int unit, bcm_pbmp_t pbmp, args_t *args)
{
    parse_table_t pt;
    bcm_port_t port, dport;
    int num_ports, i, num_lanes;
    cmd_result_t res = CMD_OK;
    bcm_error_t rv = BCM_E_NONE;
    char *if_str;
    int phy_unit, phy_unit_value = 0, phy_unit_if;
    uint32 inst;
    soc_port_phy_ber_proj_params_t params;
    soc_phy_prbs_errcnt_t** prbs_errcnt = NULL;
    uint32 time_remaining;

    params.ber_proj_fec_type = _SHR_PORT_PHY_FEC_RS_544;
    params.ber_proj_hist_errcnt_thresh = 0;
    params.ber_proj_timeout_s = 60;

    /*
     * unit:  phy_unit_value: phy chain number
     * if  :  if_str="sys"|"line"
     */
    parse_table_init(unit, &pt);

    parse_table_add(&pt, "unit", PQ_DFL | PQ_INT, (void *)(0),
                    &phy_unit_value, NULL);
    parse_table_add(&pt, "if", PQ_STRING, 0, &if_str, NULL);
    parse_table_add(&pt, "hist_errcnt_threshold", PQ_DFL | PQ_INT, (void *)(0),
                    &params.ber_proj_hist_errcnt_thresh, NULL);
    parse_table_add(&pt, "sample_time", PQ_DFL | PQ_INT, (void *)(0),
                    &params.ber_proj_timeout_s, NULL);

    if (parse_arg_eq(args, &pt) < 0) {
        cli_out("Error: invalid option: %s\n", ARG_CUR(args));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    res = _phy_diag_phy_if_get(if_str, &phy_unit_if);
    if (res == CMD_OK) {
        res = _phy_diag_phy_unit_get(phy_unit_value, &phy_unit);
    }

    /* Now free allocated strings */
    parse_arg_eq_done(&pt);

    if (res != CMD_OK) {
        return res;
    }

    inst = PHY_DIAG_INSTANCE(phy_unit, phy_unit_if, PHY_DIAG_LN_DFLT);

    /* Here we parallelize the ber proj for all the ports
     * Here're the steps for ber proj:
     * 1. Pre: allocate memory to store err count.
     * 2. Pre config analyzer. (Only needed when hist_errcnt_threshold == 0.)
     * 3. Config: Configure BER analyzer.
     * 4. Start: Start accumulating errors.
     * 5. Collect: Collect error count.
     * 6. Proj: Project BER.
     * 7. Print output.
     */

    /* Verify input */
    if (params.ber_proj_timeout_s <= 0) {
        cli_out("Error: invalid timeout value: %d\n", params.ber_proj_timeout_s);
        return CMD_USAGE;
    }

    num_ports = 0;
    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
        num_ports++;
    }

    /* Allocate memories to collect error count */
    prbs_errcnt = sal_alloc(sizeof(soc_phy_prbs_errcnt_t*)*num_ports, "BER error cnt array");
    if (prbs_errcnt == NULL) {
        cli_out("Insufficient memory.\n");
        res = CMD_FAIL;
        goto exit;
    }
    /* Initialize memory space */
    for (i = 0; i < num_ports; i++) {
        prbs_errcnt[i] = NULL;
    }

    i = 0;
    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
        num_lanes = SOC_INFO(unit).port_num_lanes[port];
        prbs_errcnt[i] = sal_alloc(sizeof(soc_phy_prbs_errcnt_t)*num_lanes, "BER error cnt");
        if (prbs_errcnt[i] == NULL) {
            cli_out("Insufficient memory.\n");
            res = CMD_FAIL;
            goto exit;
        }
        sal_memset(prbs_errcnt[i], 0, sizeof(soc_phy_prbs_errcnt_t)*num_lanes);
        i++;
    }

    if (params.ber_proj_hist_errcnt_thresh == 0) {
        /* Pre-stage, only needed when using optimized errcnt threshold. */
        cli_out("Getting optimized threshold for all the lanes...\n");
        i = 0;
        /* coverity[overrun-local] */
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            params.ber_proj_phase = SOC_PORT_PHY_BER_PROJ_P_PRE;
            params.ber_proj_prbs_errcnt = prbs_errcnt[i];
            rv = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_CMD,
                                 PHY_DIAG_CTRL_BER_PROJ, (void *)&params);
            if (rv != BCM_E_NONE) {
                res = CMD_FAIL;
                goto exit;
            }
            i++;
        }
        /* 99 is used to generate CEIL function of 5% of timeout_s */
        time_remaining = (params.ber_proj_timeout_s * 5 + 99)/ 100;
        sal_sleep(time_remaining);
    }

    i = 0;
    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
        /* Stage 1: Config PRBS checker */
        params.ber_proj_phase = SOC_PORT_PHY_BER_PROJ_P_CONFIG;
        params.ber_proj_prbs_errcnt = prbs_errcnt[i];
        rv = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_CMD,
                             PHY_DIAG_CTRL_BER_PROJ, (void *)&params);
        if (rv != BCM_E_NONE) {
            res = CMD_FAIL;
            goto exit;
        }
        i++;
    }

    i = 0;
    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
        /* Stage 2: Start Error accumulation */
        params.ber_proj_phase = SOC_PORT_PHY_BER_PROJ_P_START;
        params.ber_proj_prbs_errcnt = prbs_errcnt[i];
        rv = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_CMD,
                             PHY_DIAG_CTRL_BER_PROJ, (void *)&params);
        if (rv != BCM_E_NONE) {
            res = CMD_FAIL;
            goto exit;
        }
        i++;
    }

    /* Stage 3: Collect PRBS error count */
    time_remaining = params.ber_proj_timeout_s;
    while (time_remaining > 0) {
        if (time_remaining > 5) {
            sal_sleep(5);
            time_remaining = time_remaining - 5;
            cli_out(".");
        } else {
            sal_sleep(time_remaining);
            time_remaining = 0;
            cli_out(".\n");
        }
        i = 0;
        /* coverity[overrun-local] */
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            params.ber_proj_phase = SOC_PORT_PHY_BER_PROJ_P_COLLECT;
            params.ber_proj_prbs_errcnt = prbs_errcnt[i];
            rv = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_CMD,
                                 PHY_DIAG_CTRL_BER_PROJ, (void *)&params);
            if (rv != BCM_E_NONE) {
                res = CMD_FAIL;
                goto exit;
            }
            i++;
        }
    }

    /* Stage 4: BER Calculate */
    i = 0;
    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
        params.ber_proj_phase = SOC_PORT_PHY_BER_PROJ_P_CALC;
        params.ber_proj_prbs_errcnt = prbs_errcnt[i];
        rv = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_CMD,
                             PHY_DIAG_CTRL_BER_PROJ, (void *)&params);
        if (rv != BCM_E_NONE) {
            res = CMD_FAIL;
            goto exit;
        }
        i++;
    }

exit:
    if (prbs_errcnt != NULL) {
        i = 0;
        /* coverity[overrun-local] */
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            if (prbs_errcnt[i] != NULL) {
                sal_free(prbs_errcnt[i]);
            }
            i++;
        }
        sal_free(prbs_errcnt);
    }

    return res;
}

#ifdef PORTMOD_SUPPORT

int portmod_port_num_phys_get(int unit, int port, int *nof_phys)
{
int is_most_ext = 0;
int phyn,  nof_cores;
phymod_core_access_t core_access[7];
int rv;
    phyn = 0;

    while (!is_most_ext) {
        rv = portmod_port_core_access_get(unit, port, phyn, MAX_PHYN, core_access, &nof_cores, &is_most_ext);
        if (BCM_FAILURE(rv)){
            return CMD_FAIL;
        }
        phyn++;
    }
    *nof_phys = phyn;
    return CMD_OK;

}
#endif

int
port_to_phyaddr(int unit, int port)
{
    int phy_id;
#ifdef PORTMOD_SUPPORT
    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        phy_id = portmod_port_to_phyaddr(unit, port);
    } else
#endif
    {
        phy_id = PORT_TO_PHY_ADDR(unit, port);
    }
    return phy_id;
}

int
port_to_phyaddr_int(int unit, int port)
{
    int phy_id;
#ifdef PORTMOD_SUPPORT
    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        phy_id = portmod_port_to_phyaddr_int(unit, port);
    } else
#endif
    {
        phy_id = PORT_TO_PHY_ADDR_INT(unit, port);
    }
    return phy_id;
}

void
_print_timesync_egress_message_mode(char *message,
                                         bcm_port_phy_timesync_event_message_egress_mode_t
                                         mode)
{

    cli_out("%s (no,uc,rc,ct) - ", message);

    switch (mode) {
        case bcmPortPhyTimesyncEventMessageEgressModeNone:
            cli_out("NOne\n");
            break;
        case bcmPortPhyTimesyncEventMessageEgressModeUpdateCorrectionField:
            cli_out("Update_Correctionfield\n");
            break;
        case bcmPortPhyTimesyncEventMessageEgressModeReplaceCorrectionFieldOrigin:
            cli_out("Replace_Correctionfield_origin\n");
            break;
        case bcmPortPhyTimesyncEventMessageEgressModeCaptureTimestamp:
            cli_out("Capture_Timestamp\n");
            break;
        default:
            cli_out("\n");
            break;
    }

}

void
_print_inband_timesync_matching_criterion(uint32 flags)
{
    uint8 delimit = 0;
    cli_out("InBand timesync MATch (none, ip, mac, pnum, vlid) - ");

    if (flags & BCM_PORT_PHY_TIMESYNC_INBAND_MATCH_IP_ADDR) {
        cli_out("ip");
        delimit = 1;
    }
    if (flags & BCM_PORT_PHY_TIMESYNC_INBAND_MATCH_MAC_ADDR) {
        cli_out("%s", delimit ? ", mac" : "mac");
        delimit = 1;
    }
    if (flags & BCM_PORT_PHY_TIMESYNC_INBAND_MATCH_SRC_PORT_NUM) {
        cli_out("%s", delimit ? ", pnum" : "pnum");
        delimit = 1;
    }
    if (flags & BCM_PORT_PHY_TIMESYNC_INBAND_MATCH_VLAN_ID) {
        cli_out("%s", delimit ? ", vlid" : "vlid");
    }
    cli_out("\n");
}

bcm_port_phy_timesync_event_message_egress_mode_t
_convert_timesync_egress_message_str(char *str,
                                     bcm_port_phy_timesync_event_message_egress_mode_t
                                     def)
{
    int i;
    struct s_array {
        char *s;
        bcm_port_phy_timesync_event_message_egress_mode_t value;
    } data[] = {
        {
        "no", bcmPortPhyTimesyncEventMessageEgressModeNone}, {
        "uc", bcmPortPhyTimesyncEventMessageEgressModeUpdateCorrectionField}, {
        "rc",
                bcmPortPhyTimesyncEventMessageEgressModeReplaceCorrectionFieldOrigin},
        {
    "ct", bcmPortPhyTimesyncEventMessageEgressModeCaptureTimestamp}};

    for (i = 0; i < (sizeof(data) / sizeof(data[0])); i++) {
        if (!sal_strncmp(str, data[i].s, 2)) {
            return data[i].value;
        }
    }
    return def;
}

void
_print_timesync_ingress_message_mode(char *message,
                                          bcm_port_phy_timesync_event_message_ingress_mode_t
                                          mode)
{

    cli_out("%s (no,uc,it,id) - ", message);

    switch (mode) {
        case bcmPortPhyTimesyncEventMessageIngressModeNone:
            cli_out("NOne\n");
            break;
        case bcmPortPhyTimesyncEventMessageIngressModeUpdateCorrectionField:
            cli_out("Update_Correctionfield\n");
            break;
        case bcmPortPhyTimesyncEventMessageIngressModeInsertTimestamp:
            cli_out("Insert_Timestamp\n");
            break;
        case bcmPortPhyTimesyncEventMessageIngressModeInsertDelaytime:
            cli_out("Insert_Delaytime\n");
            break;
        default:
            cli_out("\n");
            break;
    }

}

bcm_port_phy_timesync_event_message_ingress_mode_t
_convert_timesync_ingress_message_str(char *str,
                                      bcm_port_phy_timesync_event_message_ingress_mode_t
                                      def)
{
    int i;
    struct s_array {
        char *s;
        bcm_port_phy_timesync_event_message_ingress_mode_t value;
    } data[] = {
        {
        "no", bcmPortPhyTimesyncEventMessageIngressModeNone}, {
        "uc", bcmPortPhyTimesyncEventMessageIngressModeUpdateCorrectionField}, {
        "it", bcmPortPhyTimesyncEventMessageIngressModeInsertTimestamp}, {
    "id", bcmPortPhyTimesyncEventMessageIngressModeInsertDelaytime}};

    for (i = 0; i < (sizeof(data) / sizeof(data[0])); i++) {
        if (!sal_strncmp(str, data[i].s, 2)) {
            return data[i].value;
        }
    }
    return def;
}

void
_print_timesync_gmode(char *message,
                           bcm_port_phy_timesync_global_mode_t mode)
{

    cli_out("%s (fr,si,cp) - ", message);

    switch (mode) {
        case bcmPortPhyTimesyncModeFree:
            cli_out("FRee\n");
            break;
        case bcmPortPhyTimesyncModeSyncin:
            cli_out("SyncIn\n");
            break;
        case bcmPortPhyTimesyncModeCpu:
            cli_out("CPu\n");
            break;
        default:
            cli_out("\n");
            break;
    }

}

bcm_port_phy_timesync_global_mode_t
_convert_timesync_gmode_str(char *str,
                                 bcm_port_phy_timesync_global_mode_t def)
{
    int i;
    struct s_array {
        char *s;
        bcm_port_phy_timesync_global_mode_t value;
    } data[] = {
        {
        "fr", bcmPortPhyTimesyncModeFree}, {
        "si", bcmPortPhyTimesyncModeSyncin}, {
    "cp", bcmPortPhyTimesyncModeCpu}};

    for (i = 0; i < (sizeof(data) / sizeof(data[0])); i++) {
        if (!sal_strncmp(str, data[i].s, 2)) {
            return data[i].value;
        }
    }
    return def;
}

void
_print_framesync_mode(char *message,
                           bcm_port_phy_timesync_framesync_mode_t mode)
{

    cli_out("%s (fno,fs0,fs1,fss,fsc) - ", message);

    switch (mode) {
        case bcmPortPhyTimesyncFramesyncNone:
            cli_out("FramesyncNOne\n");
            break;
        case bcmPortPhyTimesyncFramesyncSyncin0:
            cli_out("FramesyncSyncIn0\n");
            break;
        case bcmPortPhyTimesyncFramesyncSyncin1:
            cli_out("FramesyncSyncIn1\n");
            break;
        case bcmPortPhyTimesyncFramesyncSyncout:
            cli_out("FrameSyncSyncout\n");
            break;
        case bcmPortPhyTimesyncFramesyncCpu:
            cli_out("FrameSyncCpu\n");
            break;
        default:
            cli_out("\n");
            break;
    }

}

bcm_port_phy_timesync_framesync_mode_t
_convert_framesync_mode_str(char *str,
                                 bcm_port_phy_timesync_framesync_mode_t def)
{
    int i;
    struct s_array {
        char *s;
        bcm_port_phy_timesync_framesync_mode_t value;
    } data[] = {
        {
        "fno", bcmPortPhyTimesyncFramesyncNone}, {
        "fs0", bcmPortPhyTimesyncFramesyncSyncin0}, {
        "fs1", bcmPortPhyTimesyncFramesyncSyncin1}, {
        "fss", bcmPortPhyTimesyncFramesyncSyncout}, {
    "fsc", bcmPortPhyTimesyncFramesyncCpu}};

    for (i = 0; i < (sizeof(data) / sizeof(data[0])); i++) {
        if (!sal_strncmp(str, data[i].s, 3)) {
            return data[i].value;
        }
    }
    return def;
}

void
_print_syncout_mode(char *message,
                         bcm_port_phy_timesync_syncout_mode_t mode)
{

    cli_out("%s (sod,sot,spt,sps) - ", message);

    switch (mode) {
        case bcmPortPhyTimesyncSyncoutDisable:
            cli_out("SyncOutDisable\n");
            break;
        case bcmPortPhyTimesyncSyncoutOneTime:
            cli_out("SyncoutOneTime\n");
            break;
        case bcmPortPhyTimesyncSyncoutPulseTrain:
            cli_out("SyncoutPulseTrain\n");
            break;
        case bcmPortPhyTimesyncSyncoutPulseTrainWithSync:
            cli_out("SyncoutPulsetrainSync\n");
            break;
        default:
            cli_out("\n");
            break;
    }

}

bcm_port_phy_timesync_syncout_mode_t
_convert_syncout_mode_str(char *str,
                               bcm_port_phy_timesync_syncout_mode_t def)
{
    int i;
    struct s_array {
        char *s;
        bcm_port_phy_timesync_syncout_mode_t value;
    } data[] = {
        {
        "sod", bcmPortPhyTimesyncSyncoutDisable}, {
        "sot", bcmPortPhyTimesyncSyncoutOneTime}, {
        "spt", bcmPortPhyTimesyncSyncoutPulseTrain}, {
    "sps", bcmPortPhyTimesyncSyncoutPulseTrainWithSync}};

    for (i = 0; i < (sizeof(data) / sizeof(data[0])); i++) {
        if (!sal_strncmp(str, data[i].s, 3)) {
            return data[i].value;
        }
    }
    return def;
}

void
_set_inband_timesync_matching_criterion(char *str,
                                        uint32 * inband_ctrl_flags)
{
    int i;
    struct s_array {
        char *s;
        uint32 flag;
    } data[] = {
                  {"ip"  , BCM_PORT_PHY_TIMESYNC_INBAND_MATCH_IP_ADDR},
                  {"mac" , BCM_PORT_PHY_TIMESYNC_INBAND_MATCH_MAC_ADDR},
                  {"pnum", BCM_PORT_PHY_TIMESYNC_INBAND_MATCH_SRC_PORT_NUM},
                  {"vlid", BCM_PORT_PHY_TIMESYNC_INBAND_MATCH_VLAN_ID},
                  {"none", 0}
               };

    *inband_ctrl_flags &=
                ~(BCM_PORT_PHY_TIMESYNC_INBAND_MATCH_IP_ADDR |
                  BCM_PORT_PHY_TIMESYNC_INBAND_MATCH_MAC_ADDR |
                  BCM_PORT_PHY_TIMESYNC_INBAND_MATCH_SRC_PORT_NUM |
                  BCM_PORT_PHY_TIMESYNC_INBAND_MATCH_VLAN_ID);
    for ( i = 0; i < (sizeof(data) / sizeof(data[0])); i++ ) {
        if ( ! sal_strcmp(str, data[i].s) ) {
            *inband_ctrl_flags |= data[i].flag;
        }
    }

}

void
_print_timesync_config(bcm_port_phy_timesync_config_t *conf)
{

    cli_out("ENable (Y or N) - %s\n",
            conf->flags & BCM_PORT_PHY_TIMESYNC_ENABLE ? "Yes" : "No");

    cli_out("CaptureTS (Y or N) - %s\n",
            conf->
            flags & BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE ? "Yes" : "No");

    cli_out("HeartbeatTS (Y or N) - %s\n",
            conf->
            flags & BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE ? "Yes" : "No");

    cli_out("RxCrc (Y or N) - %s\n",
            conf->flags & BCM_PORT_PHY_TIMESYNC_RX_CRC_ENABLE ? "Yes" : "No");

    cli_out("AS (Y or N) - %s\n",
            conf->flags & BCM_PORT_PHY_TIMESYNC_8021AS_ENABLE ? "Yes" : "No");

    cli_out("L2 (Y or N) - %s\n",
            conf->flags & BCM_PORT_PHY_TIMESYNC_L2_ENABLE ? "Yes" : "No");

    cli_out("IP4 (Y or N) - %s\n",
            conf->flags & BCM_PORT_PHY_TIMESYNC_IP4_ENABLE ? "Yes" : "No");

    cli_out("IP6 (Y or N) - %s\n",
            conf->flags & BCM_PORT_PHY_TIMESYNC_IP6_ENABLE ? "Yes" : "No");

    cli_out("ExtClock (Y or N) - %s\n",
            conf->flags & BCM_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT ? "Yes" : "No");

    cli_out("ITpid = 0x%04x\n", conf->itpid);

    cli_out("OTpid = 0x%04x\n", conf->otpid);

    cli_out("OriginalTimecodeSeconds = 0x%08x%08x\n",
            COMPILER_64_HI(conf->original_timecode.seconds),
            COMPILER_64_LO(conf->original_timecode.seconds));

    cli_out("OriginalTimecodeNanoseconds = 0x%08x\n",
            conf->original_timecode.nanoseconds);

    _print_timesync_gmode("GMode", conf->gmode);

    _print_framesync_mode("FramesyncMode", conf->framesync.mode);

    _print_syncout_mode("SyncoutMode", conf->syncout.mode);

    cli_out("TxOffset = %d\n", conf->tx_timestamp_offset);

    cli_out("RxOffset = %d\n", conf->rx_timestamp_offset);

    _print_timesync_egress_message_mode("TxSync", conf->tx_sync_mode);
    _print_timesync_egress_message_mode("TxDelayReq",
                                        conf->tx_delay_request_mode);
    _print_timesync_egress_message_mode("TxPdelayReq",
                                        conf->tx_pdelay_request_mode);
    _print_timesync_egress_message_mode("TxPdelayreS",
                                        conf->tx_pdelay_response_mode);

    _print_timesync_ingress_message_mode("RxSync", conf->rx_sync_mode);
    _print_timesync_ingress_message_mode("RxDelayReq",
                                         conf->rx_delay_request_mode);
    _print_timesync_ingress_message_mode("RxPdelayReq",
                                         conf->rx_pdelay_request_mode);
    _print_timesync_ingress_message_mode("RxPdelayreS",
                                         conf->rx_pdelay_response_mode);

}

void
_print_inband_timesync_config(bcm_port_phy_timesync_config_t *conf)
{
    cli_out("InBand timesync message Sync (Y or N)    - %s\n",
            ( conf->inband_control.flags
                   &  BCM_PORT_PHY_TIMESYNC_INBAND_SYNC_ENABLE )
                   ? "Yes" : "No");
    cli_out("InBand timesync Delay Request (Y or N)   - %s\n",
            ( conf->inband_control.flags
                   &  BCM_PORT_PHY_TIMESYNC_INBAND_DELAY_RQ_ENABLE )
                   ? "Yes" : "No");
    cli_out("InBand timesync Pdelay Request (Y or N)  - %s\n",
            ( conf->inband_control.flags
                   &  BCM_PORT_PHY_TIMESYNC_INBAND_PDELAY_RQ_ENABLE )
                   ? "Yes" : "No");
    cli_out("InBand timesync Pdelay reSponse (Y or N) - %s\n",
            ( conf->inband_control.flags
                   &  BCM_PORT_PHY_TIMESYNC_INBAND_PDELAY_RESP_ENABLE )
                   ? "Yes" : "No");
    cli_out("InBand timesync ID (Resv_0) - 0x%X\n",
              conf->inband_control.resv0_id);
    cli_out("InBand timesync ID ChecK (Y or N)  - %s\n",
            ( conf->inband_control.flags
                   &  BCM_PORT_PHY_TIMESYNC_INBAND_RESV0_ID_CHECK )
                   ? "Yes" : "No");
    cli_out("InBand timesync ID Update (Y or N) - %s\n",
            ( conf->inband_control.flags
                   &  BCM_PORT_PHY_TIMESYNC_INBAND_RESV0_ID_UPDATE )
                   ? "Yes" : "No");
    _print_inband_timesync_matching_criterion(conf->inband_control.flags);

    cli_out("InBand timesync Follow Up Assist (Y or N) - %s\n",
            ( conf->inband_control.flags
                   &  BCM_PORT_PHY_TIMESYNC_INBAND_FOLLOW_UP_ASSIST )
                   ? "Yes" : "No");
    cli_out("InBand timesync Delay Response Assist (Y or N) - %s\n",
            ( conf->inband_control.flags
                   &  BCM_PORT_PHY_TIMESYNC_INBAND_DELAY_RESP_ASSIST )
                   ? "Yes" : "No");
    cli_out("InBand timesync Long TimeStamp (Y or N) - %s\n",
            ( conf->inband_control.timer_mode
                   &  bcmPortPhyTimesyncTimerMode80bit )
                   ? "Yes" : "No");
}

void
_print_heartbeat_ts(int unit, bcm_port_t port)
{
    int rv;
    uint64 time;

    rv = bcm_port_control_phy_timesync_get(unit, port,
                                           bcmPortControlPhyTimesyncHeartbeatTimestamp,
                                           &time);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_get "
                "failed with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    cli_out("Heartbeat TS = %08x%08x\n",
            COMPILER_64_HI(time), COMPILER_64_LO(time));
}

void
_print_capture_ts(int unit, bcm_port_t port)
{
    int rv;
    uint64 time;

    rv = bcm_port_control_phy_timesync_get(unit, port,
                                           bcmPortControlPhyTimesyncCaptureTimestamp,
                                           &time);

    if (rv != BCM_E_NONE) {
        cli_out("bcm_port_control_phy_timesync_get "
                "failed with error  u=%d p=%d %s\n",
                unit, port, bcm_errmsg(rv));
    }

    cli_out("Capture   TS = %08x%08x\n",
            COMPILER_64_HI(time), COMPILER_64_LO(time));
}






#if defined(PHYMOD_SUPPORT)

int
phymod_sym_access(int u, args_t *a, int intermediate, soc_pbmp_t *pbm)
{
    phymod_symbols_iter_t iter;
    phymod_symbols_t *symbols;
    phymod_phy_access_t pm_acc;
    int rv, p, dport;
    char hdr[32];

    rv = phymod_symop_init(&iter, a);
    if (rv != CMD_OK) {
        return rv;
    }

    /* coverity[overrun-local] */
    DPORT_SOC_PBMP_ITER(u, *pbm, dport, p) {
        if (phymod_sym_info(u, p, intermediate, &iter, &pm_acc) < 0) {
            continue;
        }
        /* coverity[overrun-local] */
        if (IS_ROE_PORT(u, p)) {
#ifdef PHYMOD_TSCF_GEN3_SUPPORT
            pm_acc.type = phymodDispatchTypeTscf_gen3;
#else
            pm_acc.type = phymodDispatchTypeCount;
#endif
        }
        if (phymod_diag_symbols_table_get(&pm_acc, &symbols) < 0) {
            continue;
        }
        /* coverity[illegal_address] */
        rv = sal_snprintf(hdr, sizeof(hdr), "Port %s%s:\n",
                          SOC_PORT_NAME(u, p),
                          intermediate ? " (int)" : "");
        if (rv >= sizeof(hdr)) {
            continue;
        }
        rv = phymod_symop_exec(&iter, symbols, &pm_acc, hdr);
        if (rv != CMD_OK) {
            return rv;
        }
    }

    return phymod_symop_cleanup(&iter);
}

#endif /* defined(PHYMOD_SUPPORT) */

/* #define INCLUDE_TIMESYNC_DVT_TESTS */
#ifdef  INCLUDE_TIMESYNC_DVT_TESTS
cmd_result_t phy_test1588(int unit, args_t *args, soc_pbmp_t *pbm);
#endif

STATIC cmd_result_t
_if_esw_phy_info(int u, args_t *a)
{
    soc_pbmp_t pbm;
    soc_port_t p, p1, dport;
    char remark[40];
#if defined(BCM_KATANA2_SUPPORT)
    soc_field_t wc_xfi_mode_sel_fld[] =
        { WC0_8_XFI_MODE_SELf, WC1_8_XFI_MODE_SELf };
    uint32 wc_xfi_mode_sel_val[2] = { 0 };
    uint32 top_misc_control_1_val = 0;
#endif

    remark[0] = '\0';
#if defined(BCM_KATANA2_SUPPORT)
    if (SOC_IS_KATANA2(u) && !SOC_IS_SABER2(u)) {
        SOC_IF_ERROR_RETURN(READ_TOP_MISC_CONTROL_1r
                            (u, &top_misc_control_1_val));
        wc_xfi_mode_sel_val[0] =
            soc_reg_field_get(u, TOP_MISC_CONTROL_1r,
                              top_misc_control_1_val,
                              wc_xfi_mode_sel_fld[0]);
        wc_xfi_mode_sel_val[1] =
            soc_reg_field_get(u, TOP_MISC_CONTROL_1r,
                              top_misc_control_1_val,
                              wc_xfi_mode_sel_fld[1]);
    }
#endif
    SOC_PBMP_ASSIGN(pbm, PBMP_PORT_ALL(u));
    SOC_PBMP_REMOVE(pbm, PBMP_HG_SUBPORT_ALL(u));
    SOC_PBMP_REMOVE(pbm, PBMP_REQ_ALL(u));
    cli_out("Phy mapping dump:\n");
    cli_out("%10s %5s %5s %5s %5s %23s %17s\n",
            "port", "id0", "id1", "addr", "iaddr", "name", "timeout");
    /* Coverity
     * DPORT_SOC_PBMP_ITER checks that dport is valid.
     */
    /* coverity[overrun-local] */
    DPORT_SOC_PBMP_ITER(u, pbm, dport, p) {
        if (phy_port_info[u] == NULL) {
            continue;
        }
        p1 = p;
#if defined(BCM_KATANA2_SUPPORT)
        if (SOC_IS_KATANA2(u) && !SOC_IS_SABER2(u)) {
            switch (p) {
                case 25:
                case 36:
                    if (wc_xfi_mode_sel_val[0]) {
                        p1 = (p == 25) ? 32 : 34;
                        sal_sprintf(remark, "XFI#Int:%d:==>Ext:%d:", p, p1);
                    }
                    break;
                case 26:
                case 39:
                    if (wc_xfi_mode_sel_val[1]) {
                        p1 = (p == 26) ? 29 : 31;
                        sal_sprintf(remark, "XFI#Int:%d:==>Ext:%d:", p, p1);
                    }
                    break;
                default:
                    remark[0] = '\0';
                    break;
            }
        }
#endif

#ifdef PORTMOD_SUPPORT
        if(SOC_PORT_USE_PORTCTRL(u, p)) {
            phymod_core_access_t core_acc, internal_core;
            phymod_core_info_t   core_info;
            int nof_cores = 0 , an_timeout = -1;
            int phy = 0, core_num = 0, is_legacy =0;
            int range_start = 0;
            int is_first_range;
            portmod_port_diag_info_t diag_info;
            int diag_rv;
            uint8   pcount=0;
            char    lnstr[32], *pname, namelen;
            pm_info_t pm_info;
            int mod = 4;

            phymod_core_access_t_init(&core_acc);
            phymod_core_access_t_init(&internal_core);
            phymod_core_info_t_init(&core_info);
            sal_memset(&diag_info, 0, sizeof(portmod_port_diag_info_t));

            portmod_port_main_core_access_get(u, p, -1, &core_acc, &nof_cores);
            if(nof_cores == 0) {
                continue;
            }
            /* check if the external phy is a legacy phy */
            diag_rv = portmod_port_check_legacy_phy(u, p, &is_legacy);
            if (diag_rv) {
                continue;
            }

            portmod_port_main_core_access_get(u, p, 0, &internal_core, &nof_cores);
            if(nof_cores == 0) {
                continue;
            }

            diag_rv = portmod_port_diag_info_get(u, p, &diag_info);
            if(diag_rv){
                continue;
            }

            diag_rv = portmod_port_core_num_get(u, p, &core_num);
            if (diag_rv){
                continue;
            }

            diag_rv = portmod_pm_info_get(u, p, &pm_info);
            if (diag_rv) {
                continue;
            }

#ifdef PORTMOD_PM8X50_SUPPORT
            if (pm_info->type == portmodDispatchTypePm8x50) {
               mod = 8;
            }
#endif
            is_first_range = TRUE;
            PORTMOD_PBMP_ITER(diag_info.phys, phy){
                if( is_first_range ){
                    range_start = phy ;
                    is_first_range = FALSE;
                }
            }

            if (IS_QSGMII_PORT(u, p)) {
#ifdef BCM_HURRICANE4_SUPPORT
                if (SOC_IS_HURRICANE4(u)) {
                    range_start = (range_start - 25) / 4 + 1;
                } else
#endif
#ifdef BCM_FIRELIGHT_SUPPORT
                if (SOC_IS_FIRELIGHT(u)) {
                    range_start = (range_start - 2) / 4 + 2;
                } else
#endif
                {
                    range_start = (range_start - 1)/4 + 1;
                }
            }

            an_timeout = soc_property_port_get(u, p,
                              spn_PHY_AUTONEG_TIMEOUT, 250000);
#ifdef PORTMOD_CPM4X25_SUPPORT
            if (pm_info->type == portmodDispatchTypeCpm4x25) {
                pname = "CPRIFA0";
                if (IS_E_PORT(u, p)) {
                    SOC_IF_ERROR_RETURN
                        (phymod_core_info_get(&core_acc, &core_info));
                }
            } else
#endif
            {
                if ( !is_legacy ) {
                    SOC_IF_ERROR_RETURN
                        (phymod_core_info_get(&core_acc, &core_info));
                }

                PORTMOD_PBMP_COUNT(diag_info.phys, pcount);

                pname = phymod_core_version_t_mapping[core_info.core_version].key;
            }
            namelen = strlen(pname);

            sal_snprintf(lnstr, sizeof(lnstr), "%s", pname);
            sal_snprintf(lnstr+namelen-2, sizeof(lnstr)-(namelen-2), "-%s/%02d/", pname+namelen-2, core_num);

            pname = lnstr;
            while (*pname != '-') {
                *pname = sal_toupper(*pname);
                 pname++;
            }

            pname = lnstr+strlen(lnstr);
            if (pcount == 8) {
               sal_snprintf(pname,sizeof(lnstr), "%d", pcount);
            } else if (pcount == 4) {
               if (mod == 8) {
                   sal_snprintf(pname,sizeof(lnstr), "%d-%d", (range_start-1)%mod, ((range_start-1)%mod)+3);
               } else {
                   sal_snprintf(pname,sizeof(lnstr), "%d", pcount);
               }
            } else if (pcount == 2) {
#ifdef BCM_FIRELIGHT_SUPPORT
                /* The physical port number for SerDes starts from 2 on FL */
               if (SOC_IS_FIRELIGHT(u)) {
                    sal_snprintf(pname,sizeof(lnstr), "%d-%d", (range_start-2)%mod, ((range_start-2)%mod)+1);
               } else
#endif
               sal_snprintf(pname,sizeof(lnstr), "%d-%d", (range_start-1)%mod, ((range_start-1)%mod)+1);
            } else {
#ifdef BCM_FIRELIGHT_SUPPORT
               if (SOC_IS_FIRELIGHT(u)) {
                    sal_snprintf(pname,sizeof(lnstr), "%d", (range_start-2)%mod);
               } else
#endif
               sal_snprintf(pname,sizeof(lnstr), "%d", (range_start-1)%mod);
            }

            if ( !is_legacy ) {
                cli_out("%5s(%3d) %5x %5x %5x %5x %23s %10d %s \n",
                        SOC_PORT_NAME(u, p), p1,
                        core_info.phy_id0,
                        core_info.phy_id1,
                        core_acc.access.addr,
                        internal_core.access.addr,
                        lnstr, an_timeout,
                        /*soc_phy_an_timeout_get(u, p), */
                        remark);
            } else {
                cli_out("%5s(%3d) %5x %5x %5x %5x %23s %10d %s \n",
                        SOC_PORT_NAME(u, p), p1,
                        soc_phy_id0reg_get(u, p),
                        soc_phy_id1reg_get(u, p),
                        soc_phy_addr_of_port(u, p),
                        internal_core.access.addr,
                        soc_phy_name_get(u, p),
                        soc_phy_an_timeout_get(u, p), remark);
            }
       } else
#endif
       {
        cli_out("%5s(%3d) %5x %5x %5x %5x %23s %10d %s \n",
                SOC_PORT_NAME(u, p), p1,
                soc_phy_id0reg_get(u, p),
                soc_phy_id1reg_get(u, p),
                soc_phy_addr_of_port(u, p),
                soc_phy_addr_int_of_port(u, p),
                soc_phy_name_get(u, p),
                soc_phy_an_timeout_get(u, p), remark);
       }
    }

    return CMD_OK;
}

#if defined(PHYMOD_SUPPORT)
STATIC cmd_result_t
_if_esw_phy_phymod(int u, args_t *a)
{
    soc_pbmp_t pbm;
    soc_port_t p, dport;
    char *c;
    int rv = 0;

    if ((c = ARG_GET(a)) != NULL) {
        soc_phymod_ctrl_t *pmc;
        soc_phymod_phy_t *phy;
        phy_ctrl_t *pc;
        int phy_id = sal_ctoi(c, NULL);
#ifdef PORTMOD_SUPPORT
        if( sal_strcasecmp(c, "addr") == 0 ) {
            if (SOC_USE_PORTCTRL(u)) {
                if ((c = ARG_GET(a)) != NULL) {
                    phymod_dbg_addr = sal_ctoi(c, NULL);
                    if ((c = ARG_GET(a)) != NULL) {
                        phymod_dbg_mask = sal_ctoi(c, NULL);
                        if ((c = ARG_GET(a)) != NULL) {
                            phymod_dbg_lane = sal_ctoi(c, NULL);
                        } else {
                            phymod_dbg_lane = 0;
                        }
                    }
                }
                cli_out("addr=0x%0x mask=0x%08x lane=%0x",
                        phymod_dbg_addr, phymod_dbg_mask, phymod_dbg_lane) ;
                cli_out("\n");
            }
            return CMD_OK;
        }
#endif
        if (parse_bcm_pbmp(u, c, &pbm) == 0) {
            DPORT_SOC_PBMP_ITER(u, pbm, dport, p) {
                if (phy_port_info[u] == NULL) {
                    continue;
                }
#ifdef PORTMOD_SUPPORT
                if (SOC_PORT_USE_PORTCTRL(u, p)) {
                    if ((c = ARG_GET(a)) != NULL) {
                        uint16 phyad = 0;
                        phyad = portmod_port_to_phyaddr(u, p);
                        phymod_dbg_addr = phyad;
                        phymod_dbg_mask = sal_ctoi(c, NULL);
                        if ((c = ARG_GET(a)) != NULL) {
                            phymod_dbg_lane = sal_ctoi(c, NULL);
                        } else {
                            phymod_dbg_lane = 0;
                        }
                    }
                    cli_out("%5s(%3d) %d  ",
                            SOC_PORT_NAME(u, p), p,
                            SOC_PORT_BINDEX(u, p));
                    cli_out("addr=0x%0x mask=0x%08x lane=0x%0x",
                            phymod_dbg_addr, phymod_dbg_mask, phymod_dbg_lane) ;
                } else
#endif
                {
                pc = INT_PHY_SW_STATE(u, p);
                if (pc == NULL) {
                    continue;
                }
                if ((c = ARG_GET(a)) != NULL) {
                    uint16 phyad = 0;
                    soc_phy_cfg_addr_get(u, p, SOC_PHY_INTERNAL, &phyad);
                    phymod_dbg_addr = phyad;
                    phymod_dbg_mask = sal_ctoi(c, NULL);
                    if ((c = ARG_GET(a)) != NULL) {
                        phymod_dbg_lane = sal_ctoi(c, NULL);
                    } else {
                        phymod_dbg_lane = 0;
                    }
                    continue;
                }
                pmc = &pc->phymod_ctrl;
                cli_out("%5s(%3d) %d  ",
                        SOC_PORT_NAME(u, p), p,
                        SOC_PORT_BINDEX(u, p));
                phy = pmc->phy[0];
                if (phy) {
                    cli_out("phy(0x%08x)->core(0x%08x)  ",
                            phy->obj.obj_id,
                            phy->core->obj.obj_id);
                }
                }
                cli_out("\n");
            }
        } else {
            rv = soc_phymod_phy_create(u, -1, &phy);
            if (SOC_SUCCESS(rv)) {
                cli_out("phymod ID %d created\n", phy->obj.obj_id);
            }
            rv = soc_phymod_phy_find_by_id(u, phy_id, &phy);
            cli_out("phymod ID %d%s found\n", phy_id, (rv < 0) ? " not" : "");
        }
    }
    return CMD_OK;
}
#endif /* defined(PHYMOD_SUPPORT) */

STATIC cmd_result_t
_if_esw_phy_eee(int u, args_t *a)
{
    soc_pbmp_t pbm;
    soc_port_t p, dport;
    char *c;
    int rv = 0;
    char *str, *latency_str;
    uint32 eee_mode_value, eee_auto_mode_value;
    uint32 latency, idle = 0, tx_events, tx_duration, rx_events, rx_duration;
    uint32 flags;
    int i;
    parse_table_t pt;
    char *mode_type = NULL, *lstr = NULL, *stats = NULL;
    int idle_th = -1;

    if (((c = ARG_GET(a)) == NULL) || (parse_bcm_pbmp(u, c, &pbm) < 0)) {
        cli_out("%s: ERROR: unrecognized port bitmap: %s\n", ARG_CMD(a), c);
        return CMD_FAIL;
    }

    if ((c = ARG_CUR(a)) != NULL) {

        parse_table_init(u, &pt);
        parse_table_add(&pt, "MOde", PQ_DFL | PQ_STRING, 0, &mode_type, 0);

        parse_table_add(&pt, "LAtency", PQ_DFL | PQ_STRING, 0, &lstr, 0);

        parse_table_add(&pt, "IDle_th", PQ_DFL | PQ_INT, 0, &idle_th, 0);

        parse_table_add(&pt, "STats", PQ_DFL | PQ_STRING, 0, &stats, 0);

        if (parse_arg_eq(a, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        if (ARG_CNT(a) > 0) {
            cli_out("%s: Unknown argument %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }

        flags = 0;

        for (i = 0; i < pt.pt_cnt; i++) {
            if (pt.pt_entries[i].pq_type & PQ_PARSED) {
                flags |= (1 << i);
            }
        }

        DPORT_SOC_PBMP_ITER(u, pbm, dport, p) {

            if (flags & 0x1) {

                if (sal_strcasecmp(mode_type, "native") == 0) {
                    rv = bcm_port_phy_control_set(u, p,
                                                  BCM_PORT_PHY_CONTROL_EEE,
                                                  1);
                }
                if (sal_strcasecmp(mode_type, "auto") == 0) {
                    rv = bcm_port_phy_control_set(u, p,
                                                  BCM_PORT_PHY_CONTROL_EEE_AUTO,
                                                  1);
                }
                if (sal_strcasecmp(mode_type, "none") == 0) {
                    rv = bcm_port_phy_control_set(u, p,
                                                  BCM_PORT_PHY_CONTROL_EEE_AUTO,
                                                  0);
                    rv = bcm_port_phy_control_set(u, p,
                                                  BCM_PORT_PHY_CONTROL_EEE,
                                                  0);
                }

                if (rv == BCM_E_NONE) {
                    cli_out("Port %s EEE mode set to %s EEE mode\n",
                            SOC_PORT_NAME(u, p), mode_type);
                } else {
                    if (rv == BCM_E_UNAVAIL) {
                        cli_out("Port %s EEE %s mode not available\n",
                                SOC_PORT_NAME(u, p), mode_type);
                    } else {
                        cli_out("Port %s EEE %s mode set unsuccessful\n",
                                SOC_PORT_NAME(u, p), mode_type);
                    }
                }
                rv = bcm_port_phy_control_get(u, p,
                                              BCM_PORT_PHY_CONTROL_EEE_AUTO,
                                              &eee_auto_mode_value);
                if ((rv == BCM_E_NONE) && (eee_auto_mode_value == 1)) {
                    str = "auto";
                } else {
                    rv = bcm_port_phy_control_get(u, p,
                                                  BCM_PORT_PHY_CONTROL_EEE,
                                                  &eee_mode_value);
                    if (rv == BCM_E_NONE) {
                        if (eee_mode_value == 0 && eee_auto_mode_value == 0) {
                            str = "none";
                        } else {
                            str = "native";
                        }
                    } else {
                        str = "NA";
                    }
                }
                cli_out("Port %s EEE mode = %s\n", SOC_PORT_NAME(u, p), str);
            }

            if (flags & 0x2) {
                if (sal_strcasecmp(lstr, "fixed") == 0) {
                    rv = bcm_port_phy_control_set(u, p,
                                                  BCM_PORT_PHY_CONTROL_EEE_AUTO_FIXED_LATENCY,
                                                  1);
    		if (rv != BCM_E_NONE) {
    			return CMD_FAIL;
    		}
    	    }
    	    if (sal_strcasecmp(lstr, "variable") == 0) {
    		    rv = bcm_port_phy_control_set(u, p,
    				    BCM_PORT_PHY_CONTROL_EEE_AUTO_FIXED_LATENCY,
    				    0);
                	if (rv != BCM_E_NONE) {
    			return CMD_FAIL;
    		}
                }
                rv = bcm_port_phy_control_get(u, p,
                                              BCM_PORT_PHY_CONTROL_EEE_AUTO_FIXED_LATENCY,
                                              &latency);
                if (rv == BCM_E_NONE) {
                    if (latency == 1) {     /* AutogrEEEn Ctrl Reg. */
                        str = "fixed";      /*  bit_2 == 0  */
                    } else {
                        str = "variable";   /*  bit_2 == 1  */
                    }
                    cli_out("Port %s EEE Auto mode Latency = %s\n",
                            SOC_PORT_NAME(u, p), str);
                }
            }

            if (flags & 0x4) {
                if ((rv =
                     bcm_port_phy_control_set(u, p,
                                              BCM_PORT_PHY_CONTROL_EEE_AUTO_IDLE_THRESHOLD,
                                              idle_th)) != BCM_E_NONE) {
                    return CMD_FAIL;
                }
                if ((rv = bcm_port_phy_control_get(u, p,
                                                   BCM_PORT_PHY_CONTROL_EEE_AUTO_IDLE_THRESHOLD,
                                                   &idle)) != BCM_E_NONE) {
                    return CMD_FAIL;
                }
                cli_out("Port %s EEE Auto mode IDLE Threshold = %d\n",
                        SOC_PORT_NAME(u, p), idle);
            }

            if (flags & 0x8) {
                if (sal_strcasecmp(stats, "get") == 0) {
                    (void)bcm_port_phy_control_get(u, p,
                                                   BCM_PORT_PHY_CONTROL_EEE_TRANSMIT_EVENTS,
                                                   &tx_events);
                    (void)bcm_port_phy_control_get(u, p,
                                                   BCM_PORT_PHY_CONTROL_EEE_TRANSMIT_DURATION,
                                                   &tx_duration);
                    (void)bcm_port_phy_control_get(u, p,
                                                   BCM_PORT_PHY_CONTROL_EEE_RECEIVE_EVENTS,
                                                   &rx_events);
                    (void)bcm_port_phy_control_get(u, p,
                                                   BCM_PORT_PHY_CONTROL_EEE_RECEIVE_DURATION,
                                                   &rx_duration);
                    cli_out("Port %s Tx events = %u TX Duration = %u  "
                            "RX events = %u RX Duration = %u\n",
                            SOC_PORT_NAME(u, p), tx_events,
                            tx_duration, rx_events, rx_duration);
                }
                if (sal_strcasecmp(stats, "clear") == 0) {
                    (void)bcm_port_phy_control_set(u, p,
                                                   BCM_PORT_PHY_CONTROL_EEE_STATISTICS_CLEAR,
                                                   1);
                    cli_out("Port %s Statistics Cleared \n",
                            SOC_PORT_NAME(u, p));
                }

                if (sal_strcasecmp(stats, "all") == 0) {
                    (void)bcm_port_phy_control_get(u, p,
                                                   BCM_PORT_PHY_CONTROL_EEE_TRANSMIT_EVENTS,
                                                   &tx_events);
                    (void)bcm_port_phy_control_get(u, p,
                                                   BCM_PORT_PHY_CONTROL_EEE_TRANSMIT_DURATION,
                                                   &tx_duration);
                    (void)bcm_port_phy_control_get(u, p,
                                                   BCM_PORT_PHY_CONTROL_EEE_RECEIVE_EVENTS,
                                                   &rx_events);
                    rv = bcm_port_phy_control_get(u, p,
                                                  BCM_PORT_PHY_CONTROL_EEE_RECEIVE_DURATION,
                                                  &rx_duration);
                    if (rv == BCM_E_NONE) {
                        cli_out("Port %s EEE Statistics\n",
                                SOC_PORT_NAME(u, p));
                        cli_out("\tEEE Transmit Events\t%u\n"  , tx_events);
                        cli_out("\tEEE Transmit Duration\t%u\n", tx_duration);
                        cli_out("\tEEE Receive  Events\t%u\n"  , rx_events);
                        cli_out("\tEEE Receive  Duration\t%u\n", rx_duration);
                    }
                }
            }
        }
        /* free allocated memory from arg parsing */
        parse_arg_eq_done(&pt);
    } else {

        cli_out("EEE Details:\n");
        cli_out("%10s %16s %16s %14s %14s\n",
                "port", "name", "eee mode", "latency mode",
                "Idle Threshold(ms)");
        /* coverity[overrun-local] */
        DPORT_SOC_PBMP_ITER(u, pbm, dport, p) {
            latency_str = "NA";
            idle = 0;

            if ((rv =
                 bcm_port_phy_control_get(u, p,
                                          BCM_PORT_PHY_CONTROL_EEE_AUTO,
                                          &eee_auto_mode_value)) ==
                BCM_E_FAIL) {
                cli_out("Phy control get: "
                        "BCM_PORT_PHY_CONTROL_EEE_AUTO failed\n");
                return BCM_E_FAIL;
            }
            if ((rv == BCM_E_NONE) && (eee_auto_mode_value == 1)) {
                str = "auto";
                rv = bcm_port_phy_control_get(u, p,
                                              BCM_PORT_PHY_CONTROL_EEE_AUTO_FIXED_LATENCY,
                                              &latency);
                if (rv == BCM_E_NONE) {
                    if (latency == 1) {     /* AutogrEEEn Ctrl Reg. */
                        latency_str = "fixed";      /*  bit_2 == 0  */
                    } else {
                        latency_str = "variable";   /*  bit_2 == 1  */
                    }
                } else {
                    latency_str = "NA";
                }
                rv = bcm_port_phy_control_get(u, p,
                                              BCM_PORT_PHY_CONTROL_EEE_AUTO_IDLE_THRESHOLD,
                                              &idle);
                if (rv != BCM_E_NONE) {
                    idle = 0;
                }
            } else {
                if ((rv =
                     bcm_port_phy_control_get(u, p,
                                              BCM_PORT_PHY_CONTROL_EEE,
                                              &eee_mode_value)) ==
                    BCM_E_FAIL) {
                    cli_out("Phy control get: "
                            "BCM_PORT_PHY_CONTROL_EEE failed\n");
                    return BCM_E_FAIL;
                }
                if (eee_mode_value == 1) {
                    str = "native";
                } else {
                    str = "none";
                }
            }
            /* coverity[illegal_address] */
            cli_out("%5s(%3d) %16s %14s %14s %10d\n",
                    SOC_PORT_NAME(u, p), p,
                    soc_phy_name_get(u, p), str, latency_str, idle);
        }
    }
    return CMD_OK;
}

STATIC cmd_result_t
_if_esw_phy_timesync(int u, args_t *a)
{
    soc_pbmp_t pbm;
    soc_port_t p, dport;
    char *c;
    int i, rv = 0;
    bcm_port_phy_timesync_config_t conf;
    uint64 val64;

    sal_memset(&conf, 0, sizeof(conf));

    if (((c = ARG_GET(a)) == NULL) || (parse_bcm_pbmp(u, c, &pbm) < 0)) {
        cli_out("%s: ERROR: unrecognized port bitmap: %s\n", ARG_CMD(a), c);
        return CMD_FAIL;
    }
    if ((c = ARG_CUR(a)) != NULL) {
        parse_table_t pt;
        uint32 enable, capture_ts, heartbeat_ts, rx_crc, as, l2, ip4, ip6, ec,      /* bool */
            itpid, otpid, tx_offset, rx_offset, original_timecode_seconds,
            original_timecode_nanoseconds, load_all, frame_sync, flags,
            flags_2, ibts_sync = 0;
        uint32 ibts_dreq = 0, ibts_pdreq = 0, ibts_pdrsp = 0,
               ibts_resv_id_chk = 0, ibts_resv_id_upd = 0, ibts_resv_id = 0,
               ibts_upd_fu = 0, ibts_upd_drsp = 0, ibts_longts = 0;
        char *gmode_str = NULL, *tx_sync_mode_str = NULL,
             *tx_delay_request_mode_str = NULL,
             *tx_pdelay_request_mode_str = NULL,
             *tx_pdelay_response_mode_str = NULL, *rx_sync_mode_str = NULL,
             *rx_delay_request_mode_str = NULL,
             *rx_pdelay_request_mode_str = NULL,
             *rx_pdelay_response_mode_str = NULL, *framesync_mode_str = NULL,
             *syncout_mode_str = NULL, *ibts_match = NULL;

        parse_table_init(u, &pt);
        parse_table_add(&pt, "ENable",
                        PQ_BOOL | PQ_DFL |
                        PQ_NO_EQ_OPT, 0,
                        &enable, 0);                      /* index 0 */
        parse_table_add(&pt, "CaptureTS",
                        PQ_BOOL | PQ_DFL |
                        PQ_NO_EQ_OPT, 0,
                        &capture_ts, 0);                  /* index 1 */
        parse_table_add(&pt, "HeartbeatTS",
                        PQ_BOOL | PQ_DFL |
                        PQ_NO_EQ_OPT, 0,
                        &heartbeat_ts, 0);                /* index 2 */
        parse_table_add(&pt, "RxCrc",
                        PQ_BOOL | PQ_DFL |
                        PQ_NO_EQ_OPT, 0,
                        &rx_crc, 0);                      /* index 3 */
        parse_table_add(&pt, "AS",
                        PQ_BOOL | PQ_DFL |
                        PQ_NO_EQ_OPT, 0,
                        &as, 0);                          /* index 4 */
        parse_table_add(&pt, "L2",
                        PQ_BOOL | PQ_DFL |
                        PQ_NO_EQ_OPT, 0,
                        &l2, 0);                          /* index 5 */
        parse_table_add(&pt, "IP4",
                        PQ_BOOL | PQ_DFL |
                        PQ_NO_EQ_OPT, 0,
                        &ip4, 0);                         /* index 6 */
        parse_table_add(&pt, "IP6",
                        PQ_BOOL | PQ_DFL |
                        PQ_NO_EQ_OPT, 0,
                        &ip6, 0);                         /* index 7 */
        parse_table_add(&pt, "ExtClock",
                        PQ_BOOL | PQ_DFL |
                        PQ_NO_EQ_OPT, 0,
                        &ec, 0);                         /* index 8 */
        parse_table_add(&pt, "ITpid",
                        PQ_DFL | PQ_INT, 0,
                        &itpid, 0);                      /* index 9 */
        parse_table_add(&pt, "OTpid",
                        PQ_DFL | PQ_INT,
                        0, &otpid, 0);                   /* index 10 */
        parse_table_add(&pt, "GMode",
                        PQ_DFL | PQ_STRING,
                        0, &gmode_str, 0);               /* index 11 */
        parse_table_add(&pt, "TxOffset",
                        PQ_DFL | PQ_INT,
                        0, &tx_offset, 0);               /* index 12 */
        parse_table_add(&pt, "RxOffset",
                        PQ_DFL | PQ_INT,
                        0, &rx_offset, 0);               /* index 13 */
        parse_table_add(&pt, "TxSync",
                        PQ_DFL | PQ_STRING,
                        0, &tx_sync_mode_str, 0);       /* index 14 */
        parse_table_add(&pt, "TxDelayReq",
                        PQ_DFL | PQ_STRING,
                        0, &tx_delay_request_mode_str,
                        0);                            /* index 15 */
        parse_table_add(&pt, "TxPdelayReq",
                        PQ_DFL | PQ_STRING,
                        0, &tx_pdelay_request_mode_str,
                        0); /* index 16 */
        parse_table_add(&pt, "TxPdelayreS",
                        PQ_DFL | PQ_STRING,
                        0, &tx_pdelay_response_mode_str,
                        0);                               /* index 17 */
        parse_table_add(&pt, "RxSync",
                        PQ_DFL | PQ_STRING,
                        0, &rx_sync_mode_str, 0);        /* index 18 */
        parse_table_add(&pt, "RxDelayReq",
                        PQ_DFL | PQ_STRING,
                        0, &rx_delay_request_mode_str,
                        0);                               /* index 19 */
        parse_table_add(&pt, "RxPdelayReq",
                        PQ_DFL | PQ_STRING,
                        0, &rx_pdelay_request_mode_str,
                        0);                              /* index 20 */
        parse_table_add(&pt, "RxPdelayreS",
                        PQ_DFL | PQ_STRING,
                        0, &rx_pdelay_response_mode_str,
                        0);                              /* index 21 */
        parse_table_add(&pt, "OriginalTimecodeSeconds",
                        PQ_DFL | PQ_INT,
                        0, &original_timecode_seconds,
                        0);                              /* index 22 */
        parse_table_add(&pt, "OriginalTimecodeNanoseconds",
                        PQ_DFL | PQ_INT,
                        0, &original_timecode_nanoseconds,
                        0);                              /* index 23 */
        parse_table_add(&pt, "FramesyncMode",
                        PQ_DFL | PQ_STRING,
                        0, &framesync_mode_str, 0);     /* index 24 */
        parse_table_add(&pt, "SyncoutMode",
                        PQ_DFL | PQ_STRING,
                        0, &syncout_mode_str, 0);       /* index 25 */
        parse_table_add(&pt, "LoadAll",
                        PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                        0, &load_all, 0);                /* index 26 */
        parse_table_add(&pt, "FrameSync",
                        PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                        0, &frame_sync, 0);             /* index 27 */
        parse_table_add(&pt, "InBandSync",
                        PQ_DFL | PQ_BOOL | PQ_NO_EQ_OPT,
                        0, &ibts_sync, 0);              /* index 28 */
        parse_table_add(&pt, "InBandDelayRequest",
                        PQ_DFL | PQ_BOOL | PQ_NO_EQ_OPT,
                        0, &ibts_dreq, 0);              /* index 29 */
        parse_table_add(&pt, "InBandPdelayRequest",
                        PQ_DFL | PQ_BOOL | PQ_NO_EQ_OPT,
                        0, &ibts_pdreq, 0);               /* index 30 */
        parse_table_add(&pt, "InBandPdelayreSponse",
                        PQ_DFL | PQ_BOOL | PQ_NO_EQ_OPT,
                        0, &ibts_pdrsp, 0);              /* index 31 */
        parse_table_add(&pt, "InBandID",
                        PQ_DFL | PQ_INT, 0, &ibts_resv_id,
                        0);                           /* flag_2 index 0 */
        parse_table_add(&pt, "InBandIDCheck",
                        PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                        0, &ibts_resv_id_chk, 0);   /* flag_2 index 1 */
        parse_table_add(&pt, "InBandIDUpdate",
                        PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                        0, &ibts_resv_id_upd, 0);  /* flag_2 index 2 */
        parse_table_add(&pt, "InBandMATch",
                        PQ_DFL | PQ_STRING, "none",
                        &ibts_match, 0);   /* flag_2 index 3 */
        parse_table_add(&pt, "InBandFollowUpAssist",
                        PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                        0, &ibts_upd_fu, 0);
        parse_table_add(&pt, "InBandDelayRespAssist",
                        PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                        0, &ibts_upd_drsp, 0);
        parse_table_add(&pt, "InBandLongTimeStamp",
                        PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                        0, &ibts_longts, 0);

        if (parse_arg_eq(a, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        if (ARG_CNT(a) > 0) {
            cli_out("%s: Unknown argument %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }

        flags = 0;
        flags_2 = 0;

        for (i = 0; i < pt.pt_cnt; i++) {
            if (pt.pt_entries[i].pq_type & PQ_PARSED) {
                if (i >= (sizeof(flags) * 8)) {
                    flags_2 |= (1 << (i - (sizeof(flags) * 8)));
                } else {
                    flags   |= (1 << i);
                }

            }
        }

        DPORT_SOC_PBMP_ITER(u, pbm, dport, p) {
            conf.validity_mask =
                0xffffffff & (~(BCM_PORT_PHY_TIMESYNC_VALID_MPLS_CONTROL));
            if ((rv =
                 bcm_port_phy_timesync_config_get(u, p,
                                                  &conf)) == BCM_E_FAIL) {
                cli_out("bcm_port_phy_timesync_config_get() "
                        "failed, u=%d, p=%d\n", u, p);
                parse_arg_eq_done(&pt);
                return BCM_E_FAIL;
            }

            conf.validity_mask &=
                ~BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_INBAND_CONTROL;

            if (flags & (1U << 0)) {
                conf.flags &= ~BCM_PORT_PHY_TIMESYNC_ENABLE;
                conf.flags |= enable ? BCM_PORT_PHY_TIMESYNC_ENABLE : 0;
            }

            if (flags & (1U << 1)) {
                conf.flags &= ~BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE;
                conf.flags |=
                    capture_ts ? BCM_PORT_PHY_TIMESYNC_CAPTURE_TS_ENABLE :
                    0;
            }

            if (flags & (1U << 2)) {
                conf.flags &= ~BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE;
                conf.flags |=
                    heartbeat_ts ? BCM_PORT_PHY_TIMESYNC_HEARTBEAT_TS_ENABLE
                    : 0;
            }

            if (flags & (1U << 3)) {
                conf.flags &= ~BCM_PORT_PHY_TIMESYNC_RX_CRC_ENABLE;
                conf.flags |=
                    rx_crc ? BCM_PORT_PHY_TIMESYNC_RX_CRC_ENABLE : 0;
            }

            if (flags & (1U << 4)) {
                conf.flags &= ~BCM_PORT_PHY_TIMESYNC_8021AS_ENABLE;
                conf.flags |= as ? BCM_PORT_PHY_TIMESYNC_8021AS_ENABLE : 0;
            }

            if (flags & (1U << 5)) {
                conf.flags &= ~BCM_PORT_PHY_TIMESYNC_L2_ENABLE;
                conf.flags |= l2 ? BCM_PORT_PHY_TIMESYNC_L2_ENABLE : 0;
            }

            if (flags & (1U << 6)) {
                conf.flags &= ~BCM_PORT_PHY_TIMESYNC_IP4_ENABLE;
                conf.flags |= ip4 ? BCM_PORT_PHY_TIMESYNC_IP4_ENABLE : 0;
            }

            if (flags & (1U << 7)) {
                conf.flags &= ~BCM_PORT_PHY_TIMESYNC_IP6_ENABLE;
                conf.flags |= ip6 ? BCM_PORT_PHY_TIMESYNC_IP6_ENABLE : 0;
            }

            if (flags & (1U << 8)) {
                conf.flags &= ~BCM_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT;
                conf.flags |= ec ? BCM_PORT_PHY_TIMESYNC_CLOCK_SRC_EXT : 0;
            }

            if (flags & (1U << 9)) {
                conf.itpid = itpid;
            }

            if (flags & (1U << 10)) {
                conf.otpid = otpid;
            }

            if (flags & (1U << 11)) {
                conf.gmode =
                    _convert_timesync_gmode_str(gmode_str, conf.gmode);
            }

            if (flags & (1U << 12)) {
                conf.tx_timestamp_offset = tx_offset;
            }

            if (flags & (1U << 13)) {
                conf.rx_timestamp_offset = rx_offset;
            }

            if (flags & (1U << 14)) {
                conf.tx_sync_mode =
                    _convert_timesync_egress_message_str(tx_sync_mode_str,
                                                         conf.tx_sync_mode);
            }

            if (flags & (1U << 15)) {
                conf.tx_delay_request_mode =
                    _convert_timesync_egress_message_str
                    (tx_delay_request_mode_str, conf.tx_delay_request_mode);
            }

            if (flags & (1U << 16)) {
                conf.tx_pdelay_request_mode =
                    _convert_timesync_egress_message_str
                    (tx_pdelay_request_mode_str,
                     conf.tx_pdelay_request_mode);
            }

            if (flags & (1U << 17)) {
                conf.tx_pdelay_response_mode =
                    _convert_timesync_egress_message_str
                    (tx_pdelay_response_mode_str,
                     conf.tx_pdelay_response_mode);
            }

            if (flags & (1U << 18)) {
                conf.rx_sync_mode =
                    _convert_timesync_ingress_message_str(rx_sync_mode_str,
                                                          conf.
                                                          rx_sync_mode);
            }

            if (flags & (1U << 19)) {
                conf.rx_delay_request_mode =
                    _convert_timesync_ingress_message_str
                    (rx_delay_request_mode_str, conf.rx_delay_request_mode);
            }

            if (flags & (1U << 20)) {
                conf.rx_pdelay_request_mode =
                    _convert_timesync_ingress_message_str
                    (rx_pdelay_request_mode_str,
                     conf.rx_pdelay_request_mode);
            }

            if (flags & (1U << 21)) {
                conf.rx_pdelay_response_mode =
                    _convert_timesync_ingress_message_str
                    (rx_pdelay_response_mode_str,
                     conf.rx_pdelay_response_mode);
            }

            if (flags & (1U << 22)) {

                COMPILER_64_SET(conf.original_timecode.seconds, 0,
                                original_timecode_seconds);
            }

            if (flags & (1U << 23)) {
                conf.original_timecode.nanoseconds =
                    original_timecode_nanoseconds;
            }

            if (flags & (1U << 24)) {
                conf.framesync.mode =
                    _convert_framesync_mode_str(framesync_mode_str,
                                                conf.framesync.mode);
            }

            if (flags & (1U << 25)) {
                conf.syncout.mode =
                    _convert_syncout_mode_str(syncout_mode_str,
                                              conf.syncout.mode);
            }
            if (flags & (1U << 28)) {
                conf.validity_mask |=
                    BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_INBAND_CONTROL;
                conf.inband_control.flags &=
                    ~BCM_PORT_PHY_TIMESYNC_INBAND_SYNC_ENABLE;
                conf.inband_control.flags |=
                    ibts_sync ? BCM_PORT_PHY_TIMESYNC_INBAND_SYNC_ENABLE
                    : 0;
            }
            if (flags & (1U << 29)) {
                conf.validity_mask |=
                    BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_INBAND_CONTROL;
                conf.inband_control.flags &=
                    ~BCM_PORT_PHY_TIMESYNC_INBAND_DELAY_RQ_ENABLE;
                conf.inband_control.flags |=
                    ibts_dreq ?
                    BCM_PORT_PHY_TIMESYNC_INBAND_DELAY_RQ_ENABLE : 0;
            }
            if (flags & (1U << 30)) {
                conf.validity_mask |=
                    BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_INBAND_CONTROL;
                conf.inband_control.flags &=
                    ~BCM_PORT_PHY_TIMESYNC_INBAND_PDELAY_RQ_ENABLE;
                conf.inband_control.flags |=
                    ibts_pdreq ?
                    BCM_PORT_PHY_TIMESYNC_INBAND_PDELAY_RQ_ENABLE : 0;
            }
            if (flags & (1U << 31)) {
                conf.validity_mask |=
                    BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_INBAND_CONTROL;
                conf.inband_control.flags &=
                    ~BCM_PORT_PHY_TIMESYNC_INBAND_PDELAY_RESP_ENABLE;
                conf.inband_control.flags |=
                    ibts_pdrsp ?
                    BCM_PORT_PHY_TIMESYNC_INBAND_PDELAY_RESP_ENABLE : 0;
            }
            if (flags_2 & (1U << 0)) {
                conf.validity_mask |=
                    BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_INBAND_CONTROL;
                conf.inband_control.resv0_id = ibts_resv_id;
            }
            if (flags_2 & (1U << 1)) {
                conf.validity_mask |=
                    BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_INBAND_CONTROL;
                conf.inband_control.flags &=
                    ~BCM_PORT_PHY_TIMESYNC_INBAND_RESV0_ID_CHECK;
                conf.inband_control.flags |=
                    ibts_resv_id_chk ?
                    BCM_PORT_PHY_TIMESYNC_INBAND_RESV0_ID_CHECK : 0;
            }
            if (flags_2 & (1U << 2)) {
                conf.validity_mask |=
                    BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_INBAND_CONTROL;
                conf.inband_control.flags &=
                    ~BCM_PORT_PHY_TIMESYNC_INBAND_RESV0_ID_UPDATE;
                conf.inband_control.flags |=
                    ibts_resv_id_upd ?
                    BCM_PORT_PHY_TIMESYNC_INBAND_RESV0_ID_UPDATE : 0;
            }
            if (flags_2 & (1U << 3)) {
                conf.validity_mask |=
                    BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_INBAND_CONTROL;
                _set_inband_timesync_matching_criterion(ibts_match,
                                &conf.inband_control.flags);
            }
            if (flags_2 & (1U << 4)) {
                conf.validity_mask |=
                    BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_INBAND_CONTROL;
                conf.inband_control.flags &=
                     (~BCM_PORT_PHY_TIMESYNC_INBAND_FOLLOW_UP_ASSIST);
                conf.inband_control.flags |= ( ibts_upd_fu ) ?
                       BCM_PORT_PHY_TIMESYNC_INBAND_FOLLOW_UP_ASSIST : 0;
            }
            if (flags_2 & (1U << 5)) {
                conf.validity_mask |=
                    BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_INBAND_CONTROL;
                conf.inband_control.flags &=
                     (~BCM_PORT_PHY_TIMESYNC_INBAND_DELAY_RESP_ASSIST);
                conf.inband_control.flags |= ( ibts_upd_drsp ) ?
                       BCM_PORT_PHY_TIMESYNC_INBAND_DELAY_RESP_ASSIST : 0;
            }
            if (flags_2 & (1U << 6)) {
                conf.validity_mask |=
                    BCM_PORT_PHY_TIMESYNC_VALID_PHY_1588_INBAND_CONTROL;
                conf.inband_control.timer_mode &=
                     (~bcmPortPhyTimesyncTimerMode80bit);
                conf.inband_control.timer_mode |= ( ibts_longts ) ?
                       bcmPortPhyTimesyncTimerMode80bit : 0;
            }

            conf.validity_mask =
                0xffffffff & ~BCM_PORT_PHY_TIMESYNC_VALID_MPLS_CONTROL;

            if ((rv =
                 bcm_port_phy_timesync_config_set(u, p,
                                                  &conf)) == BCM_E_FAIL) {
                cli_out("bcm_port_phy_timesync_config_set() "
                        "failed, u=%d, p=%d\n", u, p);
                parse_arg_eq_done(&pt);
                return BCM_E_FAIL;
            }
            if (flags & (1U << 26)) {
                COMPILER_64_SET(val64, 0, 0xaaaaaaaa);
                rv = bcm_port_control_phy_timesync_set(u, p,
                                                       bcmPortControlPhyTimesyncLoadControl,
                                                       val64);
                if (rv != BCM_E_NONE) {
                    cli_out("bcm_port_control_phy_timesync_set failed "
                            "with error  u=%d p=%d %s\n",
                            u, p, bcm_errmsg(rv));
                }
            }
            if (flags & (1U << 27)) {
                COMPILER_64_SET(val64, 0, 1);
                rv = bcm_port_control_phy_timesync_set(u, p,
                                                       bcmPortControlPhyTimesyncFrameSync,
                                                       val64);
                if (rv != BCM_E_NONE) {
                    cli_out("bcm_port_control_phy_timesync_set failed "
                            "with error  u=%d p=%d %s\n",
                            u, p, bcm_errmsg(rv));
                }
            }

        }

        /* free allocated memory from arg parsing */
        parse_arg_eq_done(&pt);

    } else {

        /* coverity[overrun-local] */
        DPORT_SOC_PBMP_ITER(u, pbm, dport, p) {
            int offset;

            /* coverity[unchecked_value] */
            soc_phyctrl_offset_get(u, p, &offset);  /* return value not checked on purpose */

            cli_out("IEEE-1588 settings for %s(%03d) %s, offset=%d\n",
                    SOC_PORT_NAME(u, p), p, soc_phy_name_get(u, p), offset);

            conf.validity_mask =
                0xffffffff & ~BCM_PORT_PHY_TIMESYNC_VALID_MPLS_CONTROL;
            if ((rv =
                 bcm_port_phy_timesync_config_get(u, p,
                                                  &conf)) == BCM_E_FAIL) {
                cli_out("bcm_port_phy_timesync_config_get() "
                        "failed, u=%d, p=%d\n", u, p);
                return BCM_E_FAIL;
            }
            _print_timesync_config(&conf);
            _print_inband_timesync_config(&conf);
            _print_heartbeat_ts(u, p);
            _print_capture_ts(u, p);
            /*_print_enhanced_capture(u, p);*/
        }

    }
    return CMD_OK;
}

#ifdef INCLUDE_PHY_SYM_DBG
/*
 * Interface to PHY GUI for debugging
 */
/*
 * MDIO Message format
 */
typedef struct phy_sym_dbg_mdio_msg_s {
    uint8 op;
    uint8 port_id;
    uint8 device;
    uint8 c45;
    uint16 reg_addr;
    uint16 data;
} phy_sym_dbg_mdio_msg_t;

enum phy_sum_dbg_mdio_op {
    MDIO_HANDSHAKE,
    MDIO_WRITE,
    MDIO_READ,
    MDIO_RESP,
    MDIO_ERROR,
    MDIO_CLOSE
};
#define PHY_SYM_DBG_MDIO_MSG_LEN sizeof(phy_sym_dbg_mdio_msg_t)

typedef struct sym_dbg_s {
    int unit;
    int tcpportnum;
    sal_mutex_t sym_dbg_lock;
    int flags;
} sym_dbg_t;

sym_dbg_t sym_dbg_arg;

#define SYM_DBG_LOCK(sym_dbg_arg) sal_mutex_take(sym_dbg_arg.sym_dbg_lock, \
                                                         sal_mutex_FOREVER)
#define SYM_DBG_UNLOCK(sym_dbg_arg) sal_mutex_give(sym_dbg_arg.sym_dbg_lock)

#define SYM_DBG_THREAD_START 1
#define SYM_DBG_THREAD_STOP  2

#define PHY_SYM_DBG_DEBUG_PRINT  0

static void
_phy_sym_debug_server(void *unit)
{
    int sockfd, newsockfd, clilen;
    struct sockaddr_in serv_addr, client_addr;
    phy_sym_dbg_mdio_msg_t buffer, mdio_rsp;
    uint16 phy_data, reg_addr, data;
    int n, rv = 0;
    int u, tcpportno;
    int flags = 0;
    struct timeval tout;
    fd_set sock_fdset;
    int i;

    /* Not yet initialized */
    SYM_DBG_LOCK(sym_dbg_arg);
    flags = sym_dbg_arg.flags;
    SYM_DBG_UNLOCK(sym_dbg_arg);

    if (!(flags & SYM_DBG_THREAD_START)) {
        sal_thread_exit(-1);
    }

    u = sym_dbg_arg.unit;
    tcpportno = sym_dbg_arg.tcpportnum;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        cli_out("Error opeing a socket\n");
        sal_thread_exit(-1);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(tcpportno);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        cli_out("Error: socket bind falied\n");
        close(sockfd);
        sal_thread_exit(-1);
    }
    cli_out("Listening on the port %d for client connection\n", tcpportno);

    listen(sockfd, 1);
    clilen = sizeof(client_addr);
    cli_out("accepting on the socket %d for client connection\n", sockfd);

    while (1) {
        FD_ZERO(&sock_fdset);
        FD_SET(sockfd, &sock_fdset);
        newsockfd = -1;
        do {
#if PHY_SYM_DBG_DEBUG_PRINT
            cli_out("Calling select\n");
#endif

            tout.tv_sec = 20;   /* 5 seconds */
            tout.tv_usec = 0;
            newsockfd = select(FD_SETSIZE,
                               &sock_fdset, &sock_fdset, NULL, &tout);

#if PHY_SYM_DBG_DEBUG_PRINT
            cli_out("Returned from select %d\n", newsockfd);
#endif
            SYM_DBG_LOCK(sym_dbg_arg);
            if (sym_dbg_arg.flags & SYM_DBG_THREAD_STOP) {
                cli_out("Closing Socket, user aborted\n");
                close(sockfd);
                sal_thread_exit(-1);
            }
            SYM_DBG_UNLOCK(sym_dbg_arg);
        } while (newsockfd <= 0);

#if PHY_SYM_DBG_DEBUG_PRINT
        cli_out("Select returned \n");
#endif
        newsockfd = accept(sockfd, (struct sockaddr *)&client_addr,
                           (socklen_t *) & clilen);
        if (newsockfd < 0) {
            cli_out("Error in socket accept \n");
            close(sockfd);
            sal_thread_exit(-1);
        }
#if PHY_SYM_DBG_DEBUG_PRINT
        cli_out("Accepted Client connection %d\n", newsockfd);
#endif

#if 0
        bzero(&buffer, PHY_SYM_DBG_MDIO_MSG_LEN);
        n = recv(newsockfd, &buffer, PHY_SYM_DBG_MDIO_MSG_LEN, 0);
        if ((n < 0) || (n < PHY_SYM_DBG_MDIO_MSG_LEN)) {
            cli_out("Error reading from socket \n");
            rv = -1;
            goto sock_close;
        }
        if (buffer.op != MDIO_HANDSHAKE) {
            cli_out("Unknown connection, Handshake not "
                    "received: Closing Socket\n");
            rv = -1;
            goto sock_close;
        }

        /* Sending back the handshake */
        bzero(&mdio_rsp, PHY_SYM_DBG_MDIO_MSG_LEN);
        mdio_rsp.op = MDIO_HANDSHAKE;
        n = send(newsockfd, &mdio_rsp, PHY_SYM_DBG_MDIO_MSG_LEN, 0);
        if (n < 0) {
            cli_out("Handshake:Error writing to socket\n");
            rv = -1;
            goto sock_close;
        }
        cli_out("Handshake with client completed\n");
#endif
        while (1) {
            /* Check if the thread needs to be aborted */
            SYM_DBG_LOCK(sym_dbg_arg);
            if (sym_dbg_arg.flags & SYM_DBG_THREAD_STOP) {
                break;
            }
            SYM_DBG_UNLOCK(sym_dbg_arg);
            bzero((char *)&buffer, PHY_SYM_DBG_MDIO_MSG_LEN);
            n = recv(newsockfd, (void *)&buffer, PHY_SYM_DBG_MDIO_MSG_LEN, 0);
#if PHY_SYM_DBG_DEBUG_PRINT
            cli_out("Size read %d %d\n", n, PHY_SYM_DBG_MDIO_MSG_LEN);
#endif
            if (n > 0) {
                for (i = 0; i < n; i++) {
#if PHY_SYM_DBG_DEBUG_PRINT
                    cli_out("%x ", *(((char *)&buffer) + i));
#endif
                }
            }
            if ((n < 0) || (n < PHY_SYM_DBG_MDIO_MSG_LEN)) {
                cli_out("MDIO message corrupted or garbage read"
                        " from socket: Closing socket\n");
                break;
            }
            reg_addr = ntohs(buffer.reg_addr);
            data = ntohs(buffer.data);

            bzero((char *)&mdio_rsp, PHY_SYM_DBG_MDIO_MSG_LEN);
            if (buffer.op == MDIO_WRITE) {
#if PHY_SYM_DBG_DEBUG_PRINT
                cli_out("MDIO WRITE operation\n");
                cli_out("%x %x %x %x %x\n", buffer.port_id, buffer.device,
                        reg_addr, buffer.c45, data);
#endif
                if (buffer.c45) {
                    rv = soc_miimc45_write(u, buffer.port_id, buffer.device,
                                           reg_addr, data);
                    /* Read the register */
                    if (rv >= 0) {
                        rv = soc_miimc45_read(u, buffer.port_id, buffer.device,
                                              reg_addr, &phy_data);
                    }
                } else {
                    rv = soc_miim_write(u, buffer.port_id, reg_addr, data);
                    /* Read the register */
                    if (rv >= 0) {
                        rv = soc_miim_read(u, buffer.port_id, reg_addr,
                                           &phy_data);
#if PHY_SYM_DBG_DEBUG_PRINT
                        cli_out("read data = %x\n", phy_data);
#endif
                    }
                }
                if (rv < 0) {
                    cli_out("ERROR: MII Addr %d: soc_miim_write failed: %s\n",
                            buffer.port_id, soc_errmsg(rv));
                    rv = -1;
                    break;
                }
                mdio_rsp.data = htons(phy_data);
                mdio_rsp.op = MDIO_RESP;
            } else {
                if (buffer.op == MDIO_READ) {
#if PHY_SYM_DBG_DEBUG_PRINT
                    cli_out("MDIO READ operation\n");
                    cli_out("%x %x %x %x %x\n", buffer.port_id, buffer.device,
                            reg_addr, buffer.c45, data);
#endif

                    if (buffer.c45) {
                        rv = soc_miimc45_read(u, buffer.port_id, buffer.device,
                                              reg_addr, &phy_data);
                    } else {
                        rv = soc_miim_read(u, buffer.port_id, reg_addr,
                                           &phy_data);
                    }
                    if (rv < 0) {
                        cli_out("ERROR: MII Addr %d: soc_miim_read failed: %s\n",
                                buffer.port_id, soc_errmsg(rv));
                        rv = -1;
                        break;
                    }
                    mdio_rsp.data = htons(phy_data);
                    mdio_rsp.op = MDIO_RESP;
#if PHY_SYM_DBG_DEBUG_PRINT
                    cli_out("READ data = %x %x\n", mdio_rsp.data, phy_data);
#endif
                } else {
                    if (buffer.op == MDIO_CLOSE) {
                        cli_out("Closing connection\n");
                        break;
                    } else {
                        cli_out("Unknown MDIO operation\n");
                        mdio_rsp.op = MDIO_ERROR;
                    }
                }
            }
#if PHY_SYM_DBG_DEBUG_PRINT
            cli_out("sending response:\n");
            for (i = 0; i < n; i++) {
                cli_out("%x ", *(((char *)&mdio_rsp) + i));
            }
#endif
            n = send(newsockfd, (void *)&mdio_rsp, PHY_SYM_DBG_MDIO_MSG_LEN, 0);
            if (n < 0) {
#if PHY_SYM_DBG_DEBUG_PRINT
                cli_out("Error writing to socket\n");
#endif
                rv = -1;
                break;
            }
        }
    }
/*sock_close:*/
    close(newsockfd);
    close(sockfd);
    sal_thread_exit(rv);
}

STATIC cmd_result_t
_if_esw_phy_symdebug(int u, args_t *a)
{
    int portnum;
    sal_thread_t sym_dbg_thread;
    char *c;

    if (sym_dbg_arg.flags == SYM_DBG_THREAD_START) {
        cli_out("Thread already running\n");
        return CMD_OK;
    }

    /* Get TCP port number to listen on */
    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    portnum = sal_ctoi(c, 0);

    cli_out("Entering PHY Symbolic Debug mode. In this mode the link scan "
            "is DISABLED\n");
    cli_out("Listening on TCP port %d\n", portnum);

    sym_dbg_arg.unit = u;
    sym_dbg_arg.tcpportnum = portnum;
    sym_dbg_arg.flags = SYM_DBG_THREAD_START;
    sym_dbg_arg.sym_dbg_lock = sal_mutex_create("sym_dbg_lock");

    /* Create a thread to execute the sock functions */
    sym_dbg_thread = sal_thread_create("phy_sym_dbg", 0, SAL_THREAD_STKSZ,
                                       _phy_sym_debug_server, (void *)&u);
    if (sym_dbg_thread == NULL) {
        cli_out("Unable to create thread\n");
        return CMD_FAIL;
    }
    return CMD_OK;
}

STATIC cmd_result_t
_if_esw_phy_symdebug_off(int u, args_t *a)
{
    cli_out("Stopping server thread\n");
    SYM_DBG_LOCK(sym_dbg_arg);
    sym_dbg_arg.flags = SYM_DBG_THREAD_STOP;
    SYM_DBG_UNLOCK(sym_dbg_arg);
    return CMD_OK;
}
#endif

STATIC cmd_result_t
_if_esw_phy_firmware(int u, args_t *a)
{
#ifdef  NO_FILEIO
    cli_out("This command is not supported without file I/O\n");
    return CMD_FAIL;
#else
    soc_pbmp_t pbm;
    soc_port_t p, dport;
    char *c;
    int rv = 0;
    parse_table_t pt;
    int count;
    FILE *fp = NULL;
    uint8 *buf;
    int buf_len;
    int len;
    uint32 flags;
    char input[32];
    int no_confirm = FALSE;
    int internal = 0;
    int raw = 0;
    char *filename = NULL;

    if (((c = ARG_GET(a)) == NULL) || (parse_bcm_pbmp(u, c, &pbm) < 0)) {
        cli_out("%s: ERROR: unrecognized port bitmap: %s\n",
                ARG_CMD(a), c ? c : "");
        return CMD_FAIL;
    }

    SOC_PBMP_COUNT(pbm, count);

    if (count > 1) {
        cli_out("ERROR: too many ports specified : %d\n",
                count);
        return CMD_FAIL;
    }
    parse_table_init(u, &pt);
    if ((c = ARG_CUR(a)) != NULL) {

        if (c[0] == '=') {
            return CMD_USAGE;       /* '=' unsupported */
        }

        parse_table_add(&pt, "set", PQ_DFL | PQ_STRING, 0,
                        &filename, NULL);
        parse_table_add(&pt, "-y", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT, 0,
                        &no_confirm, NULL);
        parse_table_add(&pt, "int", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT, 0,
                        &internal, NULL);
        parse_table_add(&pt, "raw", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT, 0,
                        &raw, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        if (ARG_CNT(a) > 0) {
            cli_out("%s: Unknown argument %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
    }

    if (!filename) {
        cli_out("ERROR: file name missing\n");
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    if (raw && !internal) {
        cli_out("ERROR: raw mode supported for internal PHYs only\n");
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    if ((fp = sal_fopen(filename, "rb")) == NULL) {
        cli_out("ERROR: Can't open the file : %s\n",
                filename);
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }

    ;
    if ((buf_len = sal_fsize(fp)) <= 0) {
        cli_out("ERROR: Could not determine file size\n");
        parse_arg_eq_done(&pt);
        sal_fclose(fp);
        return CMD_FAIL;
    }

    /*
     * filename points to memory allocated by arg parsing.
     * Calling parse_arg_eq_done frees this memory.
     */
    parse_arg_eq_done(&pt);

    if (!raw && !no_confirm) {
        /* prompt user for confirmation */
        cli_out("Warning!!!\n"
                "The PHY device may become un-usable if the power is off\n"
                "during this process or a wrong file is given. The file must\n"
                "be in BINARY format. The only way to recover is to program\n"
                "the non-volatile storage device with a rom burner\n");

        if ((NULL ==
             sal_readline("Are you sure you want to continue (yes/[no])?",
                          input, sizeof(input), "no")) ||
            (sal_strlen(input) != sal_strlen("yes")) ||
            (sal_strncasecmp("yes", input, sal_strlen(input)))) {
            sal_fclose(fp);
            cli_out("Firmware update aborted. No writes to the "
                    "PHY device's non-volatile storage\n");
            return CMD_FAIL;
        }
    }

    buf = sal_alloc(buf_len, "temp_buf");
    if (buf == NULL) {
        sal_fclose(fp);
        cli_out("ERROR: Cannot allocate enough buffer space: %d\n",
                buf_len);
        return CMD_FAIL;
    }

    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(u, pbm, dport, p) {
        /*    coverity[tainted_data_argument]    */
        len = sal_fread(buf, 1, buf_len, fp);
        if (len != buf_len) {
            sal_fclose(fp);
            /* coverity[tainted_data] */
            sal_free(buf);
            cli_out("ERROR: Could only read %d bytes (out of %d)\n",
                    len, buf_len);
            return CMD_FAIL;
        }
        cli_out("Downloading %d bytes...\n", len);
        flags = internal ? BCM_PORT_PHY_INTERNAL : 0;
        if (raw) {
            rv = soc_phy_firmware_load(u, p, buf, len);
        } else {
            rv = bcm_port_phy_firmware_set(u, p, flags, 0, buf, len);
        }
        break;
    }
    sal_fclose(fp);
    /*    coverity[tainted_data]    */
    sal_free(buf);
    if (rv == SOC_E_NONE) {
        cli_out("Successfully done!!!\n");
    } else if (rv == SOC_E_UNAVAIL) {
        cli_out("Aborted.\n"
                "Feature is not available for this PHY device.\n");
    } else {
        cli_out("Failed (%s).\n",
                bcm_errmsg(rv));
        if (!raw) {
            cli_out("PHY device may not be usable.\n");
        }
    }
    return CMD_OK;
#endif
}

STATIC cmd_result_t
_if_esw_phy_oam(int u, args_t *a)
{
    soc_pbmp_t pbm;
    soc_port_t p, dport;
    char *c;
    int i, rv = 0;
    bcm_port_config_phy_oam_t conf;
    uint64 val64;

    sal_memset(&conf, 0, sizeof(bcm_port_config_phy_oam_t));
    COMPILER_64_SET(val64, 0, 0);

    if (((c = ARG_GET(a)) == NULL) || (parse_bcm_pbmp(u, c, &pbm) < 0)) {
        cli_out("%s: ERROR: unrecognized port bitmap: %s\n", ARG_CMD(a), c);
        return CMD_FAIL;
    }
    if ((c = ARG_CUR(a)) != NULL) {
        parse_table_t pt;
        uint32 mac_addr_hi = 0, mac_addr_low = 0, flags;
        uint32 mac_addr_hi_1 = 0, mac_addr_low_1 = 0;
        uint32 mac_addr_hi_2 = 0, mac_addr_low_2 = 0;
        uint32 mac_addr_hi_3 = 0, mac_addr_low_3 = 0;
        uint32 eth_type = 0;
        uint32 mac_check_en = 0, cw_en = 0, entropy_en = 0,
            mac_index = 0, timestamp = 0;
        char *mode, *dir, *ts_format;
        uint8 tx = 0, rx = 0, oam_mode;
        uint32 type;

        parse_table_init(u, &pt);
        parse_table_add(&pt, "MODE", PQ_DFL | PQ_STRING, 0, &mode, 0);
        parse_table_add(&pt, "DIR", PQ_DFL | PQ_STRING, 0, &dir, 0);
        parse_table_add(&pt, "MacCheck", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                        0, &mac_check_en, 0);
        parse_table_add(&pt, "ControlWord", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                        0, &cw_en, 0);
        parse_table_add(&pt, "ENTROPY", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                        0, &entropy_en, 0);
        parse_table_add(&pt, "TimeStampFormat", PQ_DFL | PQ_STRING,
                        0, &ts_format, 0);
        parse_table_add(&pt, "ETHerType", PQ_INT | PQ_DFL, 0, &eth_type, 0);
        parse_table_add(&pt, "MACIndex", PQ_INT | PQ_DFL, 0, &mac_index, 0);
        parse_table_add(&pt, "MACAddrHi", PQ_INT | PQ_DFL,
                        0, &mac_addr_hi, 0);
        parse_table_add(&pt, "MACAddrLow", PQ_INT | PQ_DFL,
                        0, &mac_addr_low, 0);
        parse_table_add(&pt, "MACAddrHi1", PQ_INT | PQ_DFL,
                        0, &mac_addr_hi_1, 0);
        parse_table_add(&pt, "MACAddrLow1", PQ_INT | PQ_DFL,
                        0, &mac_addr_low_1, 0);
        parse_table_add(&pt, "MACAddrHi2", PQ_INT | PQ_DFL,
                        0, &mac_addr_hi_2, 0);
        parse_table_add(&pt, "MACAddrLow2", PQ_INT | PQ_DFL,
                        0, &mac_addr_low_2, 0);
        parse_table_add(&pt, "MACAddrHi3", PQ_INT | PQ_DFL,
                        0, &mac_addr_hi_3, 0);
        parse_table_add(&pt, "MACAddrLow3", PQ_INT | PQ_DFL,
                        0, &mac_addr_low_3, 0);

        if (parse_arg_eq(a, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        if (ARG_CNT(a) > 0) {
            cli_out("%s: Unknown argument %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }

        flags = 0;

        for (i = 0; i < pt.pt_cnt; i++) {
            if (pt.pt_entries[i].pq_type & PQ_PARSED) {
                flags |= (1 << i);
            }
        }

        DPORT_SOC_PBMP_ITER(u, pbm, dport, p) {
            if ((rv =
                 bcm_port_config_phy_oam_get(u, p, &conf)) == BCM_E_FAIL) {
                cli_out("bcm_port_config_phy_oam_get() failed, u=%d, p=%d\n",
                        u, p);
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }

            if (flags & (1U << 1)) {
                if (!sal_strcmp(dir, "tx")) {
                    tx = 1;
                } else if (!sal_strcmp(dir, "rx")) {
                    rx = 1;
                } else {
                    parse_arg_eq_done(&pt);
                    return CMD_USAGE;
                }
            } else {
                tx = 1;
                rx = 1;
            }

            if (flags & (1U << 0)) {
                if (!sal_strcmp(mode, "y1731")) {
                    oam_mode = bcmPortConfigPhyOamDmModeY1731;
                } else if (!sal_strcmp(mode, "bhh")) {
                    oam_mode = bcmPortConfigPhyOamDmModeBhh;
                } else if (!sal_strcmp(mode, "ietf")) {
                    oam_mode = bcmPortConfigPhyOamDmModeIetf;
                } else {
                    parse_arg_eq_done(&pt);
                    return CMD_USAGE;
                }

                if (tx) {
                    conf.tx_dm_config.mode = oam_mode;
                }
                if (rx) {
                    conf.rx_dm_config.mode = oam_mode;
                }
            }

            if (flags & (1U << 2)) {
                if (tx) {
                    conf.tx_dm_config.flags &=
                        ~BCM_PORT_PHY_OAM_DM_MAC_CHECK_ENABLE;
                    conf.tx_dm_config.flags |=
                        mac_check_en ? BCM_PORT_PHY_OAM_DM_MAC_CHECK_ENABLE
                        : 0;
                }
                if (rx) {
                    conf.rx_dm_config.flags &=
                        ~BCM_PORT_PHY_OAM_DM_MAC_CHECK_ENABLE;
                    conf.rx_dm_config.flags |=
                        mac_check_en ? BCM_PORT_PHY_OAM_DM_MAC_CHECK_ENABLE
                        : 0;
                }
            }
            if (flags & (1U << 3)) {
                if (tx) {
                    conf.tx_dm_config.flags &=
                        ~BCM_PORT_PHY_OAM_DM_CONTROL_WORD_ENABLE;
                    conf.tx_dm_config.flags |=
                        cw_en ? BCM_PORT_PHY_OAM_DM_CONTROL_WORD_ENABLE : 0;
                }
                if (rx) {
                    conf.rx_dm_config.flags &=
                        ~BCM_PORT_PHY_OAM_DM_CONTROL_WORD_ENABLE;
                    conf.rx_dm_config.flags |=
                        cw_en ? BCM_PORT_PHY_OAM_DM_CONTROL_WORD_ENABLE : 0;
                }
            }
            if (flags & (1U << 4)) {
                if (tx) {
                    conf.tx_dm_config.flags &=
                        ~BCM_PORT_PHY_OAM_DM_ENTROPY_ENABLE;
                    conf.tx_dm_config.flags |=
                        entropy_en ? BCM_PORT_PHY_OAM_DM_ENTROPY_ENABLE : 0;
                }
                if (rx) {
                    conf.rx_dm_config.flags &=
                        ~BCM_PORT_PHY_OAM_DM_ENTROPY_ENABLE;
                    conf.rx_dm_config.flags |=
                        entropy_en ? BCM_PORT_PHY_OAM_DM_ENTROPY_ENABLE : 0;
                }
            }
            if (flags & (1U << 5)) {
                if (!sal_strcmp(ts_format, "ntp")) {
                    timestamp = 1;
                } else if (!sal_strcmp(ts_format, "ptp")) {
                    timestamp = 0;
                } else {
                    parse_arg_eq_done(&pt);
                    return CMD_USAGE;
                }

                if (tx) {
                    conf.tx_dm_config.flags &=
                        ~BCM_PORT_PHY_OAM_DM_TS_FORMAT;
                    conf.tx_dm_config.flags |=
                        timestamp ? BCM_PORT_PHY_OAM_DM_TS_FORMAT : 0;
                }
                if (rx) {
                    conf.rx_dm_config.flags &=
                        ~BCM_PORT_PHY_OAM_DM_TS_FORMAT;
                    conf.rx_dm_config.flags |=
                        timestamp ? BCM_PORT_PHY_OAM_DM_TS_FORMAT : 0;
                }
            }

            /* CONFIG SET */
            if ((rv =
                 bcm_port_config_phy_oam_set(u, p, &conf)) == BCM_E_FAIL) {
                cli_out("bcm_port_config_phy_oam_set() failed, u=%d, p=%d\n",
                        u, p);
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }

            if (flags & (1U << 6)) {
                COMPILER_64_SET(val64, 0, eth_type);
                if (tx) {
                    rv = bcm_port_control_phy_oam_set(u, p,
                                                      bcmPortControlPhyOamDmTxEthertype,
                                                      val64);
                    if (rv != BCM_E_NONE) {
                        cli_out("bcm_port_control_phy_oam_set failed with error \
                                u=%d p=%d %s\n", u, p,
                                bcm_errmsg(rv));
                        parse_arg_eq_done(&pt);
                        return CMD_FAIL;
                    }
                }
                if (rx) {
                    rv = bcm_port_control_phy_oam_set(u, p,
                                                      bcmPortControlPhyOamDmRxEthertype,
                                                      val64);
                    if (rv != BCM_E_NONE) {
                        cli_out("bcm_port_control_phy_oam_set failed with error  \
                                u=%d p=%d %s\n", u, p,
                                bcm_errmsg(rv));
                        parse_arg_eq_done(&pt);
                        return CMD_FAIL;
                    }
                }
            }

            if (flags & (1U << 7)) {
                if (mac_index == 0) {
                    parse_arg_eq_done(&pt);
                    return CMD_USAGE;
                }
                COMPILER_64_SET(val64, 0, mac_index);
                if (tx) {
                    rv = bcm_port_control_phy_oam_set(u, p,
                                                      bcmPortControlPhyOamDmTxPortMacAddressIndex,
                                                      val64);
                    if (rv != BCM_E_NONE) {
                        cli_out("bcm_port_control_phy_oam_set failed with error  \
                                u=%d p=%d %s\n", u, p,
                                bcm_errmsg(rv));
                        parse_arg_eq_done(&pt);
                        return CMD_FAIL;
                    }
                }
                if (rx) {
                    rv = bcm_port_control_phy_oam_set(u, p,
                                                      bcmPortControlPhyOamDmRxPortMacAddressIndex,
                                                      val64);
                    if (rv != BCM_E_NONE) {
                        cli_out("bcm_port_control_phy_oam_set failed with error  \
                                u=%d p=%d %s\n", u, p,
                                bcm_errmsg(rv));
                        parse_arg_eq_done(&pt);
                        return CMD_FAIL;
                    }
                }
            }
            /* If mac_index is not provided and mac_addr needs to be set
               use mac_addr_1/2/3 instead.
             */
            if (!(flags & (1U << 7)) &&
                ((flags & (1U << 8)) || (flags & (1U << 9)))) {
                cli_out(" MACAddrHi/MACAddrLow cannot be used without 'MACIndex. "
                        "Please use MACAddrHi1/2/3 for updating MAC address\n");
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
            }
            if ((flags & (1U << 8)) || (flags & (1U << 9))) {
                switch (mac_index) {
                    case 1:
                        type = bcmPortControlPhyOamDmMacAddress1;
                        break;
                    case 2:
                        type = bcmPortControlPhyOamDmMacAddress2;
                        break;
                    case 3:
                        type = bcmPortControlPhyOamDmMacAddress3;
                        break;
                    default:
                        parse_arg_eq_done(&pt);
                        return CMD_FAIL;
                }

                COMPILER_64_SET(val64, mac_addr_hi, mac_addr_low);

                rv = bcm_port_control_phy_oam_set(u, p, type, val64);
                if (rv != BCM_E_NONE) {
                    cli_out("bcm_port_control_phy_oam_set failed with error  \
                            u=%d p=%d %s\n", u, p,
                            bcm_errmsg(rv));
                    parse_arg_eq_done(&pt);
                    return CMD_FAIL;
                }
            }
            if ((flags & (1U << 10)) || (flags & (1U << 11))) {
                COMPILER_64_SET(val64, mac_addr_hi_1, mac_addr_low_1);

                rv = bcm_port_control_phy_oam_set(u, p,
                                                  bcmPortControlPhyOamDmMacAddress1,
                                                  val64);
                if (rv != BCM_E_NONE) {
                    cli_out("bcm_port_control_phy_oam_set failed with error  \
                            u=%d p=%d %s\n", u, p,
                            bcm_errmsg(rv));
                    parse_arg_eq_done(&pt);
                    return CMD_FAIL;
                }
            }
            if ((flags & (1U << 12)) || (flags & (1U << 13))) {
                COMPILER_64_SET(val64, mac_addr_hi_2, mac_addr_low_2);

                rv = bcm_port_control_phy_oam_set(u, p,
                                                  bcmPortControlPhyOamDmMacAddress2,
                                                  val64);
                if (rv != BCM_E_NONE) {
                    cli_out("bcm_port_control_phy_oam_set failed with error  \
                            u=%d p=%d %s\n", u, p,
                            bcm_errmsg(rv));
                    parse_arg_eq_done(&pt);
                    return CMD_FAIL;
                }
            }
            if ((flags & (1U << 14)) || (flags & (1U << 15))) {
                COMPILER_64_SET(val64, mac_addr_hi_3, mac_addr_low_3);

                rv = bcm_port_control_phy_oam_set(u, p,
                                                  bcmPortControlPhyOamDmMacAddress3,
                                                  val64);
                if (rv != BCM_E_NONE) {
                    cli_out("bcm_port_control_phy_oam_set failed with error  \
                            u=%d p=%d %s\n", u, p,
                            bcm_errmsg(rv));
                    parse_arg_eq_done(&pt);
                    return CMD_FAIL;
                }
            }

        }
        /* free allocated memory from arg parsing */
        parse_arg_eq_done(&pt);

    } else {

        /* coverity[overrun-local] */
        DPORT_SOC_PBMP_ITER(u, pbm, dport, p) {
            int offset;
            uint64 tx_ethtype, rx_ethtype;
            uint64 mac1, mac2, mac3;
            uint64 tx_mac_index, rx_mac_index;

            COMPILER_64_SET(tx_ethtype, 0, 0);
            COMPILER_64_SET(rx_ethtype, 0, 0);
            COMPILER_64_SET(mac1, 0, 0);
            COMPILER_64_SET(mac2, 0, 0);
            COMPILER_64_SET(mac3, 0, 0);
            COMPILER_64_SET(tx_mac_index, 0, 0);
            COMPILER_64_SET(rx_mac_index, 0, 0);
            /* coverity[unchecked_value] */
            soc_phyctrl_offset_get(u, p, &offset);  /* return value not checked on purpose */

            cli_out("\n\nOAM settings for %s(%3d) %s, offset = %d\n\n",
                    SOC_PORT_NAME(u, p), p, soc_phy_name_get(u, p), offset);

            if ((rv =
                 bcm_port_config_phy_oam_get(u, p, &conf)) == BCM_E_FAIL) {
                cli_out("bcm_port_config_phy_oam_get() failed, u=%d, p=%d\n",
                        u, p);
                return CMD_FAIL;
            }

            rv = bcm_port_control_phy_oam_get(u, p,
                                              bcmPortControlPhyOamDmTxEthertype,
                                              &tx_ethtype);
            if (rv != BCM_E_NONE) {
                cli_out("bcm_port_control_phy_oam_get (TxEthertype) failed with error \
                        u=%d p=%d %s\n", u, p,
                        bcm_errmsg(rv));
                return CMD_FAIL;
            }
            rv = bcm_port_control_phy_oam_get(u, p,
                                              bcmPortControlPhyOamDmRxEthertype,
                                              &rx_ethtype);
            if (rv != BCM_E_NONE) {
                cli_out("bcm_port_control_phy_oam_get (RxEthertype) failed with error  \
                        u=%d p=%d %s\n", u, p,
                        bcm_errmsg(rv));
                return CMD_FAIL;
            }
            rv = bcm_port_control_phy_oam_get(u, p,
                                              bcmPortControlPhyOamDmTxPortMacAddressIndex,
                                              &tx_mac_index);
            if (rv != BCM_E_NONE) {
                cli_out("bcm_port_control_phy_oam_get (TxMACIndex)failed with error  \
                        u=%d p=%d %s\n", u, p,
                        bcm_errmsg(rv));
                return CMD_FAIL;
            }
            rv = bcm_port_control_phy_oam_get(u, p,
                                              bcmPortControlPhyOamDmRxPortMacAddressIndex,
                                              &rx_mac_index);
            if (rv != BCM_E_NONE) {
                cli_out("bcm_port_control_phy_oam_get (RxMACIndex)failed with error  \
                        u=%d p=%d %s\n", u, p,
                        bcm_errmsg(rv));
                return CMD_FAIL;
            }

            rv = bcm_port_control_phy_oam_get(u, p,
                                              bcmPortControlPhyOamDmMacAddress1,
                                              &mac1);
            if (rv != BCM_E_NONE) {
                cli_out("bcm_port_control_phy_oam_get (MacAddr1)failed with error  \
                        u=%d p=%d %s\n", u, p,
                        bcm_errmsg(rv));
                return CMD_FAIL;
            }
            rv = bcm_port_control_phy_oam_get(u, p,
                                              bcmPortControlPhyOamDmMacAddress2,
                                              &mac2);
            if (rv != BCM_E_NONE) {
                cli_out("bcm_port_control_phy_oam_get (MacAddr2)failed with error  \
                        u=%d p=%d %s\n", u, p,
                        bcm_errmsg(rv));
                return CMD_FAIL;
            }
            rv = bcm_port_control_phy_oam_get(u, p,
                                              bcmPortControlPhyOamDmMacAddress3,
                                              &mac3);
            if (rv != BCM_E_NONE) {
                cli_out("bcm_port_control_phy_oam_get (MacAddr3)failed with error  \
                        u=%d p=%d %s\n", u, p,
                        bcm_errmsg(rv));
                return CMD_FAIL;
            }

            cli_out("\nPHY OAM TX Config Settings:\n");
            cli_out("=============================\n");
            cli_out("MODE (Y1731, BHH or IETF) - %s\n",
                    (conf.tx_dm_config.mode ==
                    bcmPortConfigPhyOamDmModeY1731) ? "Y.1731" : (conf.
                    tx_dm_config.
                    mode ==
                    bcmPortConfigPhyOamDmModeBhh)
                    ? "BHH" : (conf.tx_dm_config.mode ==
                    bcmPortConfigPhyOamDmModeIetf) ? "IETF" :
                    "NONE");
            cli_out("MacCheck (Y or N) - %s\n",
                    conf.tx_dm_config.
                    flags & BCM_PORT_PHY_OAM_DM_MAC_CHECK_ENABLE ? "Y" :
                    "N");
            cli_out("ControlWord (Y or N) - %s\n",
                    conf.tx_dm_config.
                    flags & BCM_PORT_PHY_OAM_DM_CONTROL_WORD_ENABLE ? "Y" :
                    "N");
            cli_out("ENTROPY (Y or N) - %s\n",
                    conf.tx_dm_config.
                    flags & BCM_PORT_PHY_OAM_DM_ENTROPY_ENABLE ? "Y" : "N");
            cli_out("TimeStampFormat (PTP or NTP) - %s\n",
                    conf.tx_dm_config.
                    flags & BCM_PORT_PHY_OAM_DM_TS_FORMAT ? "NTP" : "PTP");
            cli_out("EtherType- 0x%x\n", COMPILER_64_LO(tx_ethtype));
            cli_out("MacIndex- %d\n", COMPILER_64_LO(tx_mac_index));

            cli_out("\nPHY OAM RX Config Settings:\n");
            cli_out("=============================\n");
            cli_out("MODE (Y1731, BHH or IETF) - %s\n",
                    (conf.rx_dm_config.mode ==
                    bcmPortConfigPhyOamDmModeY1731) ? "Y.1731" : (conf.
                    rx_dm_config.
                    mode ==
                    bcmPortConfigPhyOamDmModeBhh)
                    ? "BHH" : (conf.rx_dm_config.mode ==
                    bcmPortConfigPhyOamDmModeIetf) ? "IETF" :
                    "NONE");
            cli_out("MacCheck (Y or N) - %s\n",
                    conf.rx_dm_config.
                    flags & BCM_PORT_PHY_OAM_DM_MAC_CHECK_ENABLE ? "Y" :
                    "N");
            cli_out("ControlWord (Y or N) - %s\n",
                    conf.rx_dm_config.
                    flags & BCM_PORT_PHY_OAM_DM_CONTROL_WORD_ENABLE ? "Y" :
                    "N");
            cli_out("ENTROPY (Y or N) - %s\n",
                    conf.rx_dm_config.
                    flags & BCM_PORT_PHY_OAM_DM_ENTROPY_ENABLE ? "Y" : "N");
            cli_out("TimeStampFormat (PTP or NTP) - %s\n",
                    conf.rx_dm_config.
                    flags & BCM_PORT_PHY_OAM_DM_TS_FORMAT ? "NTP" : "PTP");
            cli_out("EtherType- 0x%x\n", COMPILER_64_LO(rx_ethtype));
            cli_out("MacIndex- %d\n", COMPILER_64_LO(rx_mac_index));

            cli_out("\nOther Settings:\n");
            cli_out("=============================\n");
            cli_out("MAC Address 1 - 0x%08x%08x\n", COMPILER_64_HI(mac1),
                    COMPILER_64_LO(mac1));
            cli_out("MAC Address 2 - 0x%08x%08x\n", COMPILER_64_HI(mac2),
                    COMPILER_64_LO(mac2));
            cli_out("MAC Address 3 - 0x%08x%08x\n", COMPILER_64_HI(mac3),
                    COMPILER_64_LO(mac3));

        }

    }
    return CMD_OK;
}

STATIC cmd_result_t
_if_esw_phy_power(int u, args_t *a)
{
    soc_pbmp_t pbm;
    soc_port_t p, dport;
    char *c;
    int rv = 0;
    parse_table_t pt;
    char *mode_type = 0;
    uint32 mode_value;
    int sleep_time = -1;
    int wake_time = -1;

    if (((c = ARG_GET(a)) == NULL) || (parse_bcm_pbmp(u, c, &pbm) < 0)) {
        cli_out("%s: ERROR: unrecognized port bitmap: %s\n", ARG_CMD(a), c);
        return CMD_FAIL;
    }

    if ((c = ARG_CUR(a)) != NULL) {

        if (c[0] == '=') {
            return CMD_USAGE;       /* '=' unsupported */
        }

        parse_table_init(u, &pt);
        parse_table_add(&pt, "mode", PQ_DFL | PQ_STRING, 0,
                        &mode_type, NULL);

        parse_table_add(&pt, "Sleep_Time", PQ_DFL | PQ_INT,
                        0, &sleep_time, NULL);

        parse_table_add(&pt, "Wake_Time", PQ_DFL | PQ_INT,
                        0, &wake_time, NULL);

        if (parse_arg_eq(a, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        if (ARG_CNT(a) > 0) {
            cli_out("%s: Unknown argument %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
    } else {
        char *str;
        cli_out("Phy Power Mode dump:\n");
        cli_out("%10s %16s %14s %14s %14s\n",
                "port", "name", "power_mode", "sleep_time(ms)",
                "wake_time(ms)");
        /* coverity[overrun-local] */
        DPORT_SOC_PBMP_ITER(u, pbm, dport, p) {
            mode_value = 0;
            sleep_time = 0;
            wake_time = 0;
            rv = bcm_port_phy_control_get(u, p, BCM_PORT_PHY_CONTROL_POWER,
                                          &mode_value);
            if (rv == SOC_E_NONE) {
                if (mode_value == BCM_PORT_PHY_CONTROL_POWER_AUTO) {
                    str = "auto_down";
                    if ((rv = bcm_port_phy_control_get(u, p,
                                                       BCM_PORT_PHY_CONTROL_POWER_AUTO_SLEEP_TIME,
                                                       (uint32 *) &
                                                       sleep_time)) !=
                        SOC_E_NONE) {
                        sleep_time = 0;
                    }
                    if ((rv = bcm_port_phy_control_get(u, p,
                                                       BCM_PORT_PHY_CONTROL_POWER_AUTO_WAKE_TIME,
                                                       (uint32 *) &
                                                       wake_time)) !=
                        SOC_E_NONE) {
                        wake_time = 0;
                    }

                } else if (mode_value == BCM_PORT_PHY_CONTROL_POWER_LOW) {
                    str = "low";
                } else {
                    str = "full";
                }
            } else {
                str = "unavail";
            }
            /* coverity[illegal_address] */
            cli_out("%5s(%3d) %16s %14s ",
                    SOC_PORT_NAME(u, p), p, soc_phy_name_get(u, p), str);
            if (sleep_time && wake_time) {
                cli_out("%10d %14d\n", sleep_time, wake_time);
            } else {
                cli_out("%10s %14s\n", "N/A", "N/A");
            }
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(mode_type, "auto_low") == 0) {
        (void)_phy_auto_low_start(u, pbm, 1);
    } else if (sal_strcasecmp(mode_type, "auto_off") == 0) {
        (void)_phy_auto_low_start(u, pbm, 0);
    } else if (sal_strcasecmp(mode_type, "low") == 0) {
        DPORT_SOC_PBMP_ITER(u, pbm, dport, p) {
            (void)bcm_port_phy_control_set(u, p, BCM_PORT_PHY_CONTROL_POWER,
                                           BCM_PORT_PHY_CONTROL_POWER_LOW);
        }
    } else if (sal_strcasecmp(mode_type, "full") == 0) {
        DPORT_SOC_PBMP_ITER(u, pbm, dport, p) {
            (void)bcm_port_phy_control_set(u, p, BCM_PORT_PHY_CONTROL_POWER,
                                           BCM_PORT_PHY_CONTROL_POWER_FULL);
        }
    } else if (sal_strcasecmp(mode_type, "auto_down") == 0) {
        DPORT_SOC_PBMP_ITER(u, pbm, dport, p) {
            (void)bcm_port_phy_control_set(u, p, BCM_PORT_PHY_CONTROL_POWER,
                                           BCM_PORT_PHY_CONTROL_POWER_AUTO);
            if (sleep_time >= 0) {
                (void)bcm_port_phy_control_set(u, p,
                                               BCM_PORT_PHY_CONTROL_POWER_AUTO_SLEEP_TIME,
                                               sleep_time);
            }
            if (wake_time >= 0) {
                (void)bcm_port_phy_control_set(u, p,
                                               BCM_PORT_PHY_CONTROL_POWER_AUTO_WAKE_TIME,
                                               wake_time);
            }
        }
    }

    /* free allocated memory from arg parsing */
    parse_arg_eq_done(&pt);
    return CMD_OK;
}

STATIC cmd_result_t
_if_esw_phy_margin(int unit, args_t *args)
{
    parse_table_t pt;
    bcm_port_t port, dport;
    bcm_pbmp_t pbmp;
    int rv, cmd, enable;
    char *cmd_str, *port_str;
    int marginval = 0;

    enum { _PHY_MARGIN_MAX_GET_CMD,
        _PHY_MARGIN_SET_CMD,
        _PHY_MARGIN_VALUE_SET_CMD,
        _PHY_MARGIN_VALUE_GET_CMD,
        _PHY_MARGIN_CLEAR_CMD
    };

    if ((port_str = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    BCM_PBMP_CLEAR(pbmp);
    if (parse_bcm_pbmp(unit, port_str, &pbmp) < 0) {
        cli_out("Error: unrecognized port bitmap: %s\n", port_str);
        return CMD_FAIL;
    }

    if ((cmd_str = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    if (sal_strcasecmp(cmd_str, "maxget") == 0) {
        cmd = _PHY_MARGIN_MAX_GET_CMD;
        enable = 0;
    } else if (sal_strcasecmp(cmd_str, "set") == 0) {
        cmd = _PHY_MARGIN_SET_CMD;
        enable = 1;
    } else if (sal_strcasecmp(cmd_str, "valueset") == 0) {
        cmd = _PHY_MARGIN_VALUE_SET_CMD;
        enable = 0;
    } else if (sal_strcasecmp(cmd_str, "valueget") == 0) {
        cmd = _PHY_MARGIN_VALUE_GET_CMD;
        enable = 0;
    } else if (sal_strcasecmp(cmd_str, "clear") == 0) {
        cmd = _PHY_MARGIN_CLEAR_CMD;
        enable = 0;
    } else
        return CMD_USAGE;

    parse_table_init(unit, &pt);
    if (cmd == _PHY_MARGIN_VALUE_SET_CMD) {
        parse_table_add(&pt, "marginval", PQ_DFL | PQ_INT,
                        (void *)(0), &marginval, NULL);
    }
    if (parse_arg_eq(args, &pt) < 0) {
        cli_out("Error: invalid option: %s\n", ARG_CUR(args));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    /* Now free allocated strings */
    parse_arg_eq_done(&pt);

    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {

        switch (cmd) {

            case _PHY_MARGIN_SET_CMD:
            case _PHY_MARGIN_CLEAR_CMD:

                rv = bcm_port_control_set(unit, port,
                                          bcmPortControlSerdesTuneMarginMode,
                                          enable);
                if (rv != BCM_E_NONE) {
                    cli_out("Setting margin enable failed: %s\n",
                            bcm_errmsg(rv));
                    return CMD_FAIL;
                }

                break;

            case _PHY_MARGIN_VALUE_SET_CMD:

                rv = bcm_port_control_set(unit, port,
                                          bcmPortControlSerdesTuneMarginValue,
                                          marginval);
                if (rv != BCM_E_NONE) {
                    cli_out("Getting margin value failed: %s\n", bcm_errmsg(rv));
                    return CMD_FAIL;
                }
                cli_out("margin value(%d)\n", marginval);
                break;
            case _PHY_MARGIN_MAX_GET_CMD:

                rv = bcm_port_control_get(unit, port,
                                          bcmPortControlSerdesTuneMarginMax,
                                          &marginval);
                if (rv != BCM_E_NONE) {
                    cli_out("Getting margin max value failed: %s\n",
                            bcm_errmsg(rv));
                    return CMD_FAIL;
                }
                cli_out("margin max value(%d)\n", marginval);
                break;

            case _PHY_MARGIN_VALUE_GET_CMD:

                rv = bcm_port_control_get(unit, port,
                                          bcmPortControlSerdesTuneMarginValue,
                                          &marginval);
                if (rv != BCM_E_NONE) {
                    cli_out("Getting margin value failed: %s\n", bcm_errmsg(rv));
                    return CMD_FAIL;
                }
                cli_out("margin value(%d)\n", marginval);
                break;

            default:
                break;
        }
    }

    return CMD_OK;
}

STATIC cmd_result_t
_if_esw_phy_prbs(int unit, args_t *args)
{
    parse_table_t pt;
    bcm_port_t port, dport;
    bcm_pbmp_t pbmp;
    int rv, cmd, enable, mode = 0;
    char *cmd_str, *port_str, *mode_str, *poly_str = NULL;
    int poly = 0, lb = 0;

    enum { _PHY_PRBS_SET_CMD, _PHY_PRBS_GET_CMD, _PHY_PRBS_CLEAR_CMD };
    enum { _PHY_PRBS_SI_MODE, _PHY_PRBS_HC_MODE };

    if ((port_str = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    BCM_PBMP_CLEAR(pbmp);
    if (parse_bcm_pbmp(unit, port_str, &pbmp) < 0) {
        cli_out("Error: unrecognized port bitmap: %s\n", port_str);
        return CMD_FAIL;
    }

    if ((cmd_str = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }
    if (sal_strcasecmp(cmd_str, "set") == 0) {
        cmd = _PHY_PRBS_SET_CMD;
        enable = 1;
    } else if (sal_strcasecmp(cmd_str, "get") == 0) {
        cmd = _PHY_PRBS_GET_CMD;
        enable = 0;
    } else if (sal_strcasecmp(cmd_str, "clear") == 0) {
        cmd = _PHY_PRBS_CLEAR_CMD;
        enable = 0;
    } else
        return CMD_USAGE;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Mode", PQ_STRING, 0, &mode_str, NULL);
    if (cmd == _PHY_PRBS_SET_CMD) {
            parse_table_add(&pt, "Polynomial", PQ_DFL | PQ_STRING,
                        (void *)(0), &poly_str, NULL);
        parse_table_add(&pt, "LoopBack", PQ_DFL | PQ_BOOL,
                        (void *)(0), &lb, NULL);
    }
    if (parse_arg_eq(args, &pt) < 0) {
        cli_out("Error: invalid option: %s\n", ARG_CUR(args));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }

    if ( poly_str ) {
        if ( !sal_strcasecmp(poly_str, "P7") || !sal_strcasecmp(poly_str, "0")) {
            poly = 0;
        } else if (!sal_strcasecmp(poly_str, "P15") || !sal_strcasecmp(poly_str, "1")  ) {
            poly = 1;
        } else if (!sal_strcasecmp(poly_str, "P23") || !sal_strcasecmp(poly_str, "2") ) {
            poly = 2;
        } else if (!sal_strcasecmp(poly_str, "P31") || !sal_strcasecmp(poly_str, "3") ) {
            poly = 3;
        } else if (!sal_strcasecmp(poly_str, "P9") || !sal_strcasecmp(poly_str, "4")) {
            poly = 4;
        } else if (!sal_strcasecmp(poly_str, "P11") || !sal_strcasecmp(poly_str, "5") ) {
            poly = 5;
        } else if (!sal_strcasecmp(poly_str, "P58") || !sal_strcasecmp(poly_str, "6")) {
            poly = 6;
        }else {
            cli_out("Prbs p must be P7(0), P15(1), P23(2), P31(3), P9(4), P11(5), or P58(6).\n");
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
    }

    if (mode_str) {
        if (sal_strcasecmp(mode_str, "si") == 0) {
            mode = 1;
        } else if (sal_strcasecmp(mode_str, "hc") == 0) {
            mode = 0;
        } else {
                cli_out("Prbs mode must be si, mac, phy or hc.\n");
                parse_arg_eq_done(&pt);
                return CMD_FAIL;
        }
    }

    /* Now free allocated strings */
    parse_arg_eq_done(&pt);

    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {

            /* First set prbs mode */
            rv = bcm_port_control_set(unit, port, bcmPortControlPrbsMode, mode);
            if (rv != BCM_E_NONE) {
                cli_out("Setting prbs mode failed: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
        if (cmd == _PHY_PRBS_SET_CMD || cmd == _PHY_PRBS_CLEAR_CMD) {
            if (poly >= 0 && poly <= 6) {
                /* Set polynomial */
                rv = bcm_port_control_set(unit, port,
                                          bcmPortControlPrbsPolynomial, poly);
                if (rv != BCM_E_NONE) {
                    cli_out("Setting prbs polynomial failed: %s\n",
                            bcm_errmsg(rv));
                    return CMD_FAIL;
                }
            } else {
                cli_out("Polynomial must be 0..6.\n");
                return CMD_FAIL;
            }

            /*
             * Set tx/rx enable. If clear, enable == 0
             * Note that the order of enabling is important. The following
             * steps are listed in the SI Block description in the uArch Spec.
             * - Disable the normal receive path at the receive end. This is done
             *     via SI_CONFIG0.enable;
             * - Enable the PRBS generator at the transmit end,
             *     SI_CONFIG0.prbs_generator_en;
             * - Allow enough time for the PRBS stream to be received at the
             *     monitor end
             * - Enable the PRBS monitor, SI_CONFIG0.prbs_monitor_en
             * - Read the PRBS error register to clear the error counter
             */
            if (lb) {
                enable |= 0x8000;
            }
            rv = bcm_port_control_set(unit, port,
                                      bcmPortControlPrbsTxEnable, enable);
            if (rv != BCM_E_NONE) {
                cli_out("Setting prbs tx enable failed: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }

            rv = bcm_port_control_set(unit, port,
                                      bcmPortControlPrbsRxEnable, enable);
            if (rv != BCM_E_NONE) {
                cli_out("Setting prbs rx enable failed: %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
        } else {                /* _PHY_PRBS_GET_CMD */
            int status;

            /*If reading twice the error count will be missed.*/
            rv = bcm_port_control_get(unit, port,
                                      bcmPortControlPrbsRxStatus, &status);
            if (rv != BCM_E_NONE) {
                cli_out("Getting prbs rx status failed: %s\n",
                        bcm_errmsg(rv));
                return CMD_FAIL;
            }

            switch (status) {
                case 0:
                    cli_out("%s (%2d):  PRBS OK!\n", BCM_PORT_NAME(unit, port),
                            port);
                    break;
                case -1:
                    cli_out("%s (%2d):  PRBS Failed!\n",
                            BCM_PORT_NAME(unit, port), port);
                    break;
                default:
                    cli_out("%s (%2d):  PRBS has %d errors!\n",
                            BCM_PORT_NAME(unit, port), port, status);
                    break;
            }
        }
    }
    return CMD_OK;
}

#ifdef BCM_TOMAHAWK3_SUPPORT
/* PRBSStat is a command to periodically collect PRBS error counters and
 * compute BER based on the port configuration and the observed errorr
 * counters. The command processes and displays counters and BER calculation
 * for all lanes on a given port.
*/

#define PRBS_STAT_F_INIT        (1 << 0)
#define PRBS_STAT_F_RUNNING     (1 << 1)

#define PRBS_STAT_INIT(f)       (f & PRBS_STAT_F_INIT)
#define PRBS_STAT_RUNNING(f)    (f & PRBS_STAT_F_RUNNING)

#define PM_MAX_LANES          8

#define PRBS_STAT_LOCK(lock)      \
    if (lock) { \
        sal_mutex_take(lock, sal_mutex_FOREVER);   \
    }

#define PRBS_STAT_UNLOCK(lock) \
    if (lock) { \
        sal_mutex_give(lock); \
    }

typedef struct prbs_stat_subcounter_s {
    uint64 errors;      /* PRBS errors */
    uint64 losslock;    /* Loss of lock */
} prbs_stat_subcounter_t;

/*
 * Callback function -
 */
typedef void (*prbs_stat_handler_func_t)(int unit, bcm_port_t port,
                                         int lane, uint32 errors,
                                         int losslock);
int
prbs_stat_ber_get(int unit, bcm_port_t port, int lane, double *ber,
                  int *interval);
int
prbs_stat_handler_register(int unit, prbs_stat_handler_func_t func);

int
prbs_stat_handler_unregister(int unit, prbs_stat_handler_func_t func);

/*
 * Acc: accummulated from beginning/last clear
 * Cur: count since last time shown
 */
typedef struct prbs_stat_counter_s {
    prbs_stat_subcounter_t acc;
    prbs_stat_subcounter_t cur;
} prbs_stat_counter_t;

/*
 * Per port info
 */
typedef struct prbs_stat_pinfo_s {
    int speed;
    int lanes;
    bcm_port_phy_fec_t fec_type;
    int intervals[PM_MAX_LANES];   /* Intervals expired since last show */
    int prbs_lock[PM_MAX_LANES];
    prbs_stat_counter_t counters[PM_MAX_LANES]; /* Counters per port/lane */
    double ber[PM_MAX_LANES]; /* BER updated every interval */
} prbs_stat_pinfo_t;

typedef struct prbs_stat_handler_s {
    struct prbs_stat_handler_s *next;
    prbs_stat_handler_func_t func;
} prbs_stat_handler_t;


/*
 * The main control block
 *
 * The hardware polling interval is stored in secs. Due to the latched
 * clear-on-read hardware register behavior, the interval determines
 * how fast new data is available to be examined with show counters / show
 * ber. If show commands are called before an interval has transpired,
 * there will be no new errors or ber computation available.
 *
 * Loss of lock events are recorded. For counters, there are lossoflock
 * counters that get displayed. For BER computation, once there is a
 * loss of lock, the BER computation will not be updated until the
 * next show ber is called. This ensures loss of lock events are
 * noticed in the show ber output.
 */
typedef struct prbs_stat_cb_ {
    uint32 flags;
    int secs;
    bcm_pbmp_t pbmp;
    prbs_stat_pinfo_t pinfo[BCM_PBMP_PORT_MAX];
    sal_sem_t sem;
    sal_thread_t thread_id;
    sal_mutex_t lock;
    sal_mutex_t handler_lock;
    prbs_stat_handler_t *handlers;      /* Registered callback list */
} prbs_stat_cb_t;

/* Lookup table to determine VCO, base rate */
typedef struct stat_speed_entry_ {
    uint32 speed;               /* port speed in Mbps */
    uint32 num_lanes;           /* number of lanes */
    bcm_port_phy_fec_t fec_type; /* FEC type */
    int vco;                    /* associated VCO rate of the ability */
    double rate;                /* base rate for speed */
} stat_speed_entry_t;

/* List of speed, lane, fec, VCO, and rate */
stat_speed_entry_t stat_speed_table[] =
{
    /* speed  lane fec_type             vco base rate(Gbps) */
    {10000,     1, bcmPortPhyFecNone,   20, 10.3125},
    {10000,     1, bcmPortPhyFecBaseR,  20, 10.3125},
    {20000,     1, bcmPortPhyFecNone,   20, 10.3125},
    {20000,     1, bcmPortPhyFecBaseR,  20, 10.3125},
    {40000,     4, bcmPortPhyFecNone,   20, 10.3125},
    {40000,     4, bcmPortPhyFecBaseR,  20, 10.3125},
    {40000,     2, bcmPortPhyFecNone,   20, 10.3125},
    {25000,     1, bcmPortPhyFecNone,   25, 25.78125},
    {25000,     1, bcmPortPhyFecBaseR,  25, 25.78125},
    {25000,     1, bcmPortPhyFecRsFec,  25, 25.7812},
    {50000,     1, bcmPortPhyFecNone,   25, 51.5625},
    {50000,     1, bcmPortPhyFecRsFec,  25, 51.5625},
    {50000,     1, bcmPortPhyFecRs544,  26, 53.125},
    {50000,     1, bcmPortPhyFecRs272,  26, 53.125},
    {50000,     2, bcmPortPhyFecNone,   25, 25.78125},
    {50000,     2, bcmPortPhyFecRsFec,  25, 25.78125},
    {50000,     2, bcmPortPhyFecRs544,  26, 26.5625},
    {100000,    2, bcmPortPhyFecNone,   25, 51.5625},
    {100000,    2, bcmPortPhyFecRsFec,  25, 51.5625},
    {100000,    2, bcmPortPhyFecRs544,  26, 53.125},
    {100000,    2, bcmPortPhyFecRs272,  26, 53.125},
    {100000,    4, bcmPortPhyFecNone,   25, 25.78125},
    {100000,    4, bcmPortPhyFecRsFec,  25, 25.78125},
    {100000,    4, bcmPortPhyFecRs544,  26, 26.5625},
    {200000,    4, bcmPortPhyFecNone,   25, 51.5625},
    {200000,    4, bcmPortPhyFecRs272,  26, 53.125},
    {200000,    4, bcmPortPhyFecRs544,  26, 53.125},
    {200000,    4, bcmPortPhyFecRs544_2xN,  26, 53.125},
    {400000,    8, bcmPortPhyFecRs544_2xN,  26, 53.125}
};

prbs_stat_cb_t prbs_stat_cb[BCM_MAX_NUM_UNITS];

/*
 * Get base rate in Gbps for port based on port speed, lanes, and FEC
 */
STATIC int
stat_speed_rate_get(int speed, int num_lanes, bcm_port_phy_fec_t fec_type,
                    double *rate)
{
    int i;
    int entries;

    *rate = 0.;
    entries = sizeof(stat_speed_table) / sizeof(stat_speed_entry_t);
    for (i = 0; i < entries; i++) {
        if (stat_speed_table[i].speed == speed &&
            stat_speed_table[i].num_lanes == num_lanes &&
            stat_speed_table[i].fec_type == fec_type)
        {
            *rate = stat_speed_table[i].rate;
            break;
        }
    }
    return BCM_E_NONE;
}

STATIC int
prbs_stat_ber_compute(int unit, bcm_port_t port, int lanes,
                      uint32 delta, sal_time_t secs,
                      double *ber)
{
    double rate;
    double nbits;
    prbs_stat_pinfo_t *psp;

    if (secs < 1) {
        return 0;
    }

    /* Make sure BER is computed at max if no errors */
    if ( delta == 0){
        delta = 1;
    }
    psp = &(prbs_stat_cb[unit].pinfo[port]);
    stat_speed_rate_get(psp->speed, psp->lanes, psp->fec_type, &rate);

    /* Rate needs to be adjusted from Gbps to bps */
    rate = rate * 1024. * 1024. * 1024.;
    nbits = rate * lanes;
    *ber = delta / (nbits * secs);

    return 1;
}

STATIC void
prbs_stat_handler_notify(int unit, bcm_port_t port, int lane, uint32 errors,
                         int losslock)
{
    prbs_stat_cb_t *pscb;
    prbs_stat_handler_t *h, *next;

    pscb = &(prbs_stat_cb[unit]);
    /* Notify all registered callbacks */
    PRBS_STAT_LOCK(pscb->handler_lock);
    for (h = pscb->handlers; h != NULL; h = next) {
        next = h->next;
        h->func(unit, port, lane, errors, losslock);
    }
    PRBS_STAT_UNLOCK(pscb->handler_lock);
}

STATIC void
prbs_stat_counter_show(char *cname, char *pname, int lane,
                       uint64 acc_counter, uint64 cur_counter,
                       sal_time_t secs, int show_rate)
{
    char chdr[32];
    char buf[64];
    uint64 delta;

    sal_sprintf(chdr, "%s.%s[%d]", cname, pname, lane);
    COMPILER_64_COPY(delta, acc_counter);
    COMPILER_64_SUB_64(delta, cur_counter);
    if (!COMPILER_64_IS_ZERO(delta)) {
        /* Show couters since beginning */
        format_uint64_decimal(buf, acc_counter, ',');
        LOG_CLI(("%-20s: %20s", chdr, buf));

        /* Show counters since last show */
        format_uint64_decimal(buf, delta, ',');
        LOG_CLI(("  %20s",buf));

        if (show_rate) {
            /* Show counters per second */
            COMPILER_64_UDIV_32(delta, (uint32)secs);
            format_uint64_decimal(buf, delta, ',');
            LOG_CLI(("  %12s/s",buf));
        }
        LOG_CLI(("\n"));
    }
}


STATIC int
prbs_stat_port_counter(int unit, bcm_port_t port)
{
    int lane;
    int unlocked = 0;
    prbs_stat_cb_t *pscb;
    prbs_stat_pinfo_t *pspi;
    prbs_stat_counter_t *psco;

    pscb = &(prbs_stat_cb[unit]);
    pspi = &(pscb->pinfo[port]);
    psco = pspi->counters;

    for (lane = 0; lane < pspi->lanes; lane++) {
        if (!pspi->prbs_lock[lane]) {
            unlocked++;
        }
    }
    if (unlocked == pspi->lanes) {
        LOG_CLI(("%s: no PRBS lock\n", BCM_PORT_NAME(unit, port)));
    }

    for (lane = 0; lane < pspi->lanes; lane++) {
        prbs_stat_counter_show("ERRORS", BCM_PORT_NAME(unit, port),
                               lane, psco[lane].acc.errors,
                               psco[lane].cur.errors,
                               pscb->secs * pspi->intervals[lane], TRUE);
        pspi->intervals[lane] = 0;
        psco[lane].cur.errors = psco[lane].acc.errors;
    }

    for (lane = 0; lane < pspi->lanes; lane++) {
        prbs_stat_counter_show("LOSSLOCK", BCM_PORT_NAME(unit, port),
                               lane, psco[lane].acc.losslock,
                               psco[lane].cur.losslock, 0, FALSE);
        psco[lane].cur.losslock = psco[lane].acc.losslock;
    }


    return 1;
}

STATIC void
prbs_stat_ber_update(int unit, bcm_port_t port, int lane, int delta)
{
    double ber;
    prbs_stat_cb_t *pscb;
    prbs_stat_pinfo_t *pspi;

    pscb = &(prbs_stat_cb[unit]);
    pspi = &(pscb->pinfo[port]);

    if (delta >= 0) {
        if (!prbs_stat_ber_compute(unit, port, 1, delta, pscb->secs, &ber)) {
            LOG_ERROR(BSL_LS_APPL_PHY,
                      (BSL_META_U(unit,
                                  "%s[%d]: could not compute BER\n"),
                                  BCM_PORT_NAME(unit, port), lane));
            return;
        }
        pspi->ber[lane] = ber;
        LOG_DEBUG(BSL_LS_APPL_PHY,
                  (BSL_META_U(unit, "Updated BER for port %d: %8.2e (delta=%d)\n"),
                   port, ber, delta));
    } else {
        pspi->ber[lane] = 0;
    }
}

/*
 * If there is a port configuration change, update the new configuration
 * and clear counters, BER
 */
STATIC int
prbs_stat_pinfo_update(int unit, bcm_port_t port)
{
    prbs_stat_pinfo_t *pspi;
    prbs_stat_counter_t *psco;
    bcm_port_resource_t rsrc;

    pspi = &(prbs_stat_cb[unit].pinfo[port]);

    BCM_IF_ERROR_RETURN(bcm_port_resource_speed_get(unit, port, &rsrc));
    if ((pspi->speed == rsrc.speed) &&
        (pspi->lanes == rsrc.lanes) &&
        (pspi->fec_type == rsrc.fec_type)) {
        return 1;
    }

    LOG_DEBUG(BSL_LS_APPL_PHY,
              (BSL_META_U(unit, "Updating %s config\n"),
               BCM_PORT_NAME(unit, port)));

    PRBS_STAT_LOCK(prbs_stat_cb[unit].lock);
    pspi->speed = rsrc.speed;
    pspi->lanes = rsrc.lanes;
    pspi->fec_type = rsrc.fec_type;

    psco = pspi->counters;
    sal_memset(psco, 0, sizeof(prbs_stat_counter_t) * PM_MAX_LANES);
    sal_memset(&(pspi->ber), 0, sizeof(double) * PM_MAX_LANES);
    PRBS_STAT_UNLOCK(prbs_stat_cb[unit].lock);

    return 1;
}


STATIC int
prbs_stat_collect(int unit, bcm_port_t port)
{
    int rv;
    int lane;
    uint32 status;
    uint64 lcount;
    bcm_gport_t gport;
    prbs_stat_pinfo_t *pspi;
    prbs_stat_counter_t *psco;

    prbs_stat_pinfo_update(unit, port);
    pspi = &(prbs_stat_cb[unit].pinfo[port]);
    psco = pspi->counters;

    PRBS_STAT_LOCK(prbs_stat_cb[unit].lock);
    for (lane = 0; lane < pspi->lanes; lane++) {
        COMPILER_64_SET(lcount, 0, 0);
        BCM_PHY_GPORT_LANE_PORT_SET(gport, lane, port);
        rv = bcm_port_phy_control_get(0, gport,
                                      BCM_PORT_PHY_CONTROL_PRBS_RX_STATUS,
                                      &status);
        if (BCM_FAILURE(rv)) {
            PRBS_STAT_UNLOCK(prbs_stat_cb[unit].lock);
            return 0;
        }
        LOG_DEBUG(BSL_LS_APPL_PHY,
                  (BSL_META_U(unit, "%s: collecting status %d\n"),
                   BCM_PORT_NAME(unit, port), status));
        switch (status) {
        case -1:
            pspi->prbs_lock[lane] = 0;
            pspi->ber[lane] = 0;
            break;
        case -2:
            pspi->prbs_lock[lane] = 1;
            COMPILER_64_ADD_32(psco[lane].acc.losslock, 1);
            pspi->ber[lane] = -2;
            prbs_stat_handler_notify(unit, port, lane, 0, 1);
            break;
        default:
            pspi->prbs_lock[lane] = 1;
            COMPILER_64_SET(lcount, 0, status);
            COMPILER_64_ADD_64(psco[lane].acc.errors, lcount);
            pspi->intervals[lane]++;
            if (pspi->ber[lane] != -2) {
                /*
                 * Do not update BER to preserve the loss of lock event.
                 * The next show BER will 'unlatch' the loss of lock event
                 * and restart BER calculation. Without this, the loss
                 * of lock event will be lost if multiple intervals
                 * transpire between show ber calls.
                 */
                prbs_stat_ber_update(unit, port, lane, (int)status);
            }
            prbs_stat_handler_notify(unit, port, lane, status, 0);
        }
    }
    PRBS_STAT_UNLOCK(prbs_stat_cb[unit].lock);

    return 1;
}

STATIC void
prbs_stat_thread(int unit)
{
    int u_interval;
    bcm_port_t port;
    prbs_stat_cb_t *pscb;

    pscb = &(prbs_stat_cb[unit]);
    u_interval = pscb->secs * 1000000; /* Adjust to usec */

    while (pscb->secs) {
        BCM_PBMP_ITER(pscb->pbmp, port) {
            LOG_DEBUG(BSL_LS_APPL_PHY,
                      (BSL_META_U(unit, "Collecting PRBS for port %d\n"), port));
            if (!prbs_stat_collect(unit, port)) {
                LOG_ERROR(BSL_LS_APPL_PHY,
                          (BSL_META_U(
                              unit,
                              "Failed collecting PRBS stats for port %d\n"), port));
            }
        }
        sal_sem_take(pscb->sem, u_interval);
    }

    PRBS_STAT_LOCK(pscb->lock);
    BCM_PBMP_ITER(pscb->pbmp, port) {
        sal_memset(&(prbs_stat_cb[unit].pinfo[port].counters),
                   0, sizeof(prbs_stat_counter_t) * PM_MAX_LANES);
    }
    pscb->flags &= ~PRBS_STAT_F_RUNNING;
    PRBS_STAT_UNLOCK(pscb->lock);

    pscb->thread_id = NULL;
    LOG_DEBUG(BSL_LS_APPL_PHY, ("PRBS stat thread exiting...\n"));
    sal_thread_exit(0);
}


STATIC int
prbs_stat_init(int unit)
{
    prbs_stat_cb_t *pscb;

    pscb = &(prbs_stat_cb[unit]);
    if (PRBS_STAT_INIT(pscb->flags)) {
        return 1;
    }
    sal_memset(pscb, 0, sizeof(prbs_stat_cb_t));
    pscb->flags = PRBS_STAT_F_INIT;
    pscb->handlers = NULL;
    if ((pscb->lock = sal_mutex_create("PRBSStat lock")) == NULL)
        return 0;
    if ((pscb->handler_lock = sal_mutex_create("PRBSStat handler lock")) == NULL)
        return 0;
    if ((pscb->sem = sal_sem_create("PRBSStat sleep", sal_sem_BINARY, 0)) == NULL)
        return 0;

    return 1;
}

STATIC int
prbs_stat_clear(int unit, bcm_pbmp_t pbmp, args_t *a)
{
    bcm_port_t port;
/*
    bcm_pbmp_t pbmp;

    if (!prbs_stat_pbmp_parse(unit, a, &pbmp)) {
        return CMD_USAGE;
    }
*/
    PRBS_STAT_LOCK(prbs_stat_cb[unit].lock);
    BCM_PBMP_ITER(pbmp, port) {
        sal_memset(&(prbs_stat_cb[unit].pinfo[port].counters),
                   0, sizeof(prbs_stat_counter_t) * PM_MAX_LANES);
    }
    PRBS_STAT_UNLOCK(prbs_stat_cb[unit].lock);
    return CMD_OK;
}


STATIC int
prbs_stat_counter(int unit, bcm_pbmp_t pbmp, args_t *a)
{
    int rv = 1;
    bcm_port_t port;
/*
    bcm_pbmp_t pbmp;

    if (!prbs_stat_pbmp_parse(unit, a, &pbmp)) {
        return CMD_USAGE;
    }
*/
    if (!PRBS_STAT_RUNNING(prbs_stat_cb[unit].flags)) {
        LOG_CLI(("PRBSStat not running\n"));
        return CMD_FAIL;
    }

    PRBS_STAT_LOCK(prbs_stat_cb[unit].lock);
    LOG_CLI(("%-20s  %20s  %20s  %12s",
        "port", "accumulated count", "last count", "count per sec\n"));
    BCM_PBMP_ITER(pbmp, port) {
        rv = prbs_stat_port_counter(unit, port);
        if (!rv) {
            break;
        }
    }

    PRBS_STAT_UNLOCK(prbs_stat_cb[unit].lock);
    return rv ? CMD_OK : CMD_FAIL;
}


STATIC int
prbs_stat_ber(int unit,  bcm_pbmp_t pbmp, args_t *a)
{
    int lane;
    bcm_port_t port;
/*
    bcm_pbmp_t pbmp;
*/
    prbs_stat_cb_t *pscb;
    prbs_stat_pinfo_t *pspi;

/*
    if (!prbs_stat_pbmp_parse(unit, a, &pbmp)) {
        return CMD_USAGE;
    }
*/

    pscb = &(prbs_stat_cb[unit]);
    if (!PRBS_STAT_RUNNING(pscb->flags)) {
        LOG_CLI(("PRBSStat not running\n"));
        return CMD_FAIL;
    }

    LOG_CLI(("%-6s   %s", "port", "BER\n"));
    LOG_CLI(("====\n"));
    PRBS_STAT_LOCK(pscb->lock);
    BCM_PBMP_ITER(pbmp, port) {
        pspi = &(pscb->pinfo[port]);
        for (lane = 0; lane < pspi->lanes; lane++) {
            if (pspi->ber[lane] == -2) {
                LOG_CLI(("%s[%d] : LossOfLock\n", BCM_PORT_NAME(unit, port), lane));
                pspi->ber[lane] = 0;    /* Clear it here now that has been displayed */
            } else if (pspi->ber[lane]) {
                LOG_CLI(("%s[%d] : %4.2e\n", BCM_PORT_NAME(unit, port),
                         lane, pspi->ber[lane]));
            } else {
                LOG_CLI(("%s[%d] : NoLock\n", BCM_PORT_NAME(unit, port), lane));
            }
        }
        LOG_CLI(("====\n"));
    }
    PRBS_STAT_UNLOCK(pscb->lock);
    return CMD_OK;
}


STATIC int
prbs_stat_stop(int unit)
{
    prbs_stat_cb_t *pscb;

    pscb = &(prbs_stat_cb[unit]);
    if (PRBS_STAT_RUNNING(pscb->flags)) {
        /* Signal thread to stop running */
        pscb->secs = 0;
        LOG_CLI(("Stopping PRBSStat thread\n"));
        sal_sem_give(pscb->sem);
        /* Cleared when thread exit -- pscb->flags &= ~PRBS_STAT_F_RUNNING; */
    } else {
        LOG_CLI(("PRBSStat already stopped\n"));
    }

    return CMD_OK;
}


STATIC int
prbs_stat_counter_init(int unit)/*, bcm_pbmp_t pbmp)*/
{
    int lane;
    uint32 status;
    bcm_port_t port;
    bcm_gport_t gport;
    prbs_stat_cb_t *pscb;
    prbs_stat_pinfo_t *pspi;
    prbs_stat_counter_t *psco;

    pscb = &(prbs_stat_cb[unit]);

    BCM_PBMP_ITER(pscb->pbmp, port) {
        pspi = &(pscb->pinfo[port]);
        psco = pspi->counters;

        sal_memset(psco, 0,sizeof(prbs_stat_counter_t) * PM_MAX_LANES);
        sal_memset(&(pspi->ber), 0, sizeof(double) * PM_MAX_LANES);

        /* Prime PRBS once to clear hardware counters */
        for (lane = 0; lane < pspi->lanes; lane++) {
            BCM_PHY_GPORT_LANE_PORT_SET(gport, lane, port);
            BCM_IF_ERROR_RETURN(bcm_port_phy_control_get(unit, gport,
                                     BCM_PORT_PHY_CONTROL_PRBS_RX_STATUS,
                                     &status));
            pspi->intervals[lane] = 0;
        }
    }

    return BCM_E_NONE;
}

STATIC int
prbs_stat_start(int unit, bcm_pbmp_t pbmp, args_t *a)
{
    int secs;
    /*bcm_pbmp_t pbmp;*/
    parse_table_t   pt;
    prbs_stat_cb_t *pscb;

    pscb = &(prbs_stat_cb[unit]);

    if (ARG_CNT(a) == 0) {
        if (!(pscb->flags & PRBS_STAT_F_RUNNING)) {
            LOG_CLI(("PRBSStat: not running\n"));
        } else {
            char pbmp_str[SOC_PBMP_FMT_LEN];
            SOC_PBMP_FMT(pscb->pbmp, pbmp_str);
            LOG_CLI(("PRBSStat: interval=%ds\n", pscb->secs));
            LOG_CLI(("PRBSStat: pbmp=%s\n", pbmp_str));
        }
        return CMD_OK;
    }

    /*BCM_PBMP_ASSIGN(pbmp, pscb->pbmp);*/
    secs = pscb->secs;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Interval", PQ_DFL|PQ_INT, 0, &secs, NULL);
    /*parse_table_add(&pt, "PortBitMap",PQ_DFL|PQ_PBMP, 0, &pbmp, NULL);*/

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    parse_arg_eq_done(&pt);

    if (secs == 0 || secs > 180) {
        LOG_CLI(("Interval must be between 1 and 180 seconds\n"));
        return CMD_USAGE;
    }

    /*
     * Allow start to be called while already running to update
     * interval and pbmp
     */
    if (pscb->flags & PRBS_STAT_F_RUNNING) {
        PRBS_STAT_LOCK(pscb->lock);
    }

    pscb->secs = secs;
    BCM_PBMP_ASSIGN(pscb->pbmp, pbmp);

    if (prbs_stat_counter_init(unit) != BCM_E_NONE) {
        LOG_CLI(("Failed to initialize prbs_state_counter\n"));
        return CMD_FAIL;
    }

    if (pscb->flags & PRBS_STAT_F_RUNNING) {
        PRBS_STAT_UNLOCK(pscb->lock);
        /* Must return without starting new thread */
        return CMD_OK;
    }

    /* Thread should not be running */
    if (pscb->thread_id != NULL) {
        LOG_CLI(("PRBSStat thread already running\n"));
        return CMD_FAIL;
    }

    pscb->thread_id = sal_thread_create("PRBSstat",
                                      SAL_THREAD_STKSZ,
                                      100,   /* thread priority */
                                      (void (*)(void*))prbs_stat_thread,
                                      INT_TO_PTR(unit));
    if (pscb->thread_id == SAL_THREAD_ERROR) {
        LOG_ERROR(BSL_LS_APPL_PHY,
            (BSL_META_U(unit, "Could not create PRBSstat thread\n")));
        pscb->flags &= ~PRBS_STAT_F_RUNNING;
        return CMD_FAIL;
    }
    LOG_CLI(("PRBSStat thread started...\n"));
    pscb->flags |= PRBS_STAT_F_RUNNING;

    return CMD_OK;
}


/*
 * Callback testing
 */

STATIC void
prbs_stat_test_callback(int unit, bcm_port_t port, int lane, uint32 errors, int losslock)
{
    if (losslock) {
        LOG_CLI(("%s[%d]: locklock=%d\n", BCM_PORT_NAME(unit, port),
                 lane, losslock));
    } else if (errors) {
        LOG_CLI(("%s[%d]: errors=%-8d\n", BCM_PORT_NAME(unit, port),
                 lane, errors));
    }
}


/*
 * Match input with key. Allows abbreviated matches based on
 * the minimum initial capital letters in key. For example,
 * key = ABCdef will be matched with the input
 * abc, abcd, abcde, abcdef. Any other input is a mismatch.
 */
STATIC int
prbs_stat_cmd_match(char *inp, char *key)
{
    while (*key && isupper(*key)) {
        if (!*inp || (toupper(*inp) != *key)) {
            return 0;
        }
        key++;
        inp++;
    }
    while (*inp) {
        if (toupper(*inp) != toupper(*key)) {
            return 0;
        }
        key++;
        inp++;
    }
    return 1;
}


STATIC int
prbs_stat_callback_test(int unit, args_t *a)
{
    int rv;
    char *option;

    if (ARG_CNT(a) == 0) {
        LOG_ERROR(BSL_LS_APPL_PHY,
                  (BSL_META_U(unit, "Must specify REGister or UNREGister\n")));
        return CMD_USAGE;
    }
    option = ARG_GET(a);
    if (option == NULL) {
        return CMD_USAGE;
    }
    if (prbs_stat_cmd_match(option, "REGister")) {
        rv =  prbs_stat_handler_register(unit, prbs_stat_test_callback);
    } else if (prbs_stat_cmd_match(option, "UNREGister")) {
        rv =  prbs_stat_handler_unregister(unit, prbs_stat_test_callback);
    } else {
        LOG_ERROR(BSL_LS_APPL_PHY,
                  (BSL_META_U(unit, "Must specify REGister or UNREGister\n")));
        return CMD_USAGE;
    }
    return (rv == BCM_E_NONE) ? CMD_OK : CMD_FAIL;
}

STATIC cmd_result_t
prbs_stat_cfg(int unit)
{
    prbs_stat_cb_t *pscb;

    pscb = &(prbs_stat_cb[unit]);

    if (!PRBS_STAT_RUNNING(prbs_stat_cb[unit].flags)) {
        LOG_CLI(("PRBSStat not started\n"));
    } else {
        char buf[512];
        char pfmt[SOC_PBMP_FMT_LEN];

        format_bcm_pbmp(unit, buf, sizeof(buf), pscb->pbmp);
        LOG_CLI(("PRBSStat: Polling interval: %d sec\n", pscb->secs));
        LOG_CLI(("PRBSStat: Port bitmap: %s: %s\n",
                 SOC_PBMP_FMT(pscb->pbmp, pfmt), buf));
    }
    return CMD_OK;
}

/*** APIs ***/
/*
 * Get computed BER for the last interval
 *
 * If the port is not configured with FEC or the BER cannot be computed,
 * the BER is 0
 *
 * If successful, return 1; otherwise 0
 */
int
prbs_stat_ber_get(int unit, bcm_port_t port, int lane, double *ber,
                  int *interval)
{
    prbs_stat_cb_t *pscb = &(prbs_stat_cb[unit]);
    prbs_stat_pinfo_t *pspi = &(pscb->pinfo[port]);

    PRBS_STAT_LOCK(pscb->lock);
    *ber = pspi->ber[lane];
    *interval = pscb->secs;
    PRBS_STAT_UNLOCK(pscb->lock);

    return 1;
}


/*
 * Register handler function from the caller
 */
int
prbs_stat_handler_register(int unit, prbs_stat_handler_func_t func)
{
    int found = 0;
    prbs_stat_cb_t *pscb = &(prbs_stat_cb[unit]);
    prbs_stat_handler_t *h;

    if (!PRBS_STAT_INIT(pscb->flags)) {
        LOG_CLI(("PRBSStat not initialized\n"));
        return BCM_E_DISABLED;
    }

    PRBS_STAT_LOCK(pscb->handler_lock);
    for (h = pscb->handlers; h != NULL; h = h->next) {
        if (h->func == func) {
            found = TRUE;
        }
    }
    /* Already registered */
    if (found) {
        PRBS_STAT_UNLOCK(pscb->handler_lock);
        return BCM_E_NONE;
    }

    h = sal_alloc(sizeof(prbs_stat_handler_t), "PRBSStat handler");
    if (h == NULL) {
        PRBS_STAT_UNLOCK(pscb->handler_lock);
        return BCM_E_MEMORY;
    }
    /* Move current first node to second node */
    h->next = pscb->handlers;
    h->func = func;
    /* Put the newly created node in the front of the list */
    pscb->handlers = h;
    PRBS_STAT_UNLOCK(pscb->handler_lock);

    return BCM_E_NONE;
}

/*
 * Unregister handler function from the caller
 */
int
prbs_stat_handler_unregister(int unit, prbs_stat_handler_func_t func)
{
    prbs_stat_cb_t *pscb = &(prbs_stat_cb[unit]);
    prbs_stat_handler_t *h, *prev;

    if (!PRBS_STAT_INIT(pscb->flags)) {
        LOG_CLI(("PRBSStat not initialized\n"));
        return BCM_E_DISABLED;
    }

    PRBS_STAT_LOCK(pscb->handler_lock);
    for (prev = NULL, h = pscb->handlers; h != NULL; prev = h, h = h->next) {
        if (h->func == func) {
            if (prev == NULL) {
                pscb->handlers = h->next;
            } else {
                prev->next = h->next;
            }
            break;
        }
    }
    PRBS_STAT_UNLOCK(pscb->handler_lock);

    if (h != NULL) {
        sal_free(h);
        return BCM_E_NONE;
    }
    return BCM_E_NOT_FOUND;
}

static char prbs_stat_usage[] =
    "\nParameters: [STArt [Interval=<secs>]]\n"
        "    [STOp] [Counters] [Ber] [CLear]\n";

STATIC cmd_result_t
_phy_diag_prbsstat(int unit, bcm_pbmp_t pbmp, args_t *a)
{
    char *sc;

    /* Check if a unit is attached */
    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (!SOC_IS_TOMAHAWK3(unit)) {
        return CMD_NOTIMPL;
    }

    sc = ARG_GET(a);
    /* check and display current configuration */
    if (sc == NULL) {
        return prbs_stat_cfg(unit);
    }

    /* initialize */
    if (!prbs_stat_init(unit)) {
        return CMD_FAIL;
    }

    if (prbs_stat_cmd_match(sc, "?")) {
         LOG_CLI(("%s\n", prbs_stat_usage));
    } else if (prbs_stat_cmd_match(sc, "STArt")) {
        return prbs_stat_start(unit, pbmp, a);
    } else if (prbs_stat_cmd_match(sc, "STOp")) {
        return prbs_stat_stop(unit);
    } else if (prbs_stat_cmd_match(sc, "Counters")) {
        return prbs_stat_counter(unit, pbmp, a);
    } else if (prbs_stat_cmd_match(sc, "Ber")) {
        return prbs_stat_ber(unit, pbmp, a);
    } else if (prbs_stat_cmd_match(sc, "CLear")) {
        return prbs_stat_clear(unit, pbmp, a);
    } else if (prbs_stat_cmd_match(sc, "Testcb")) {
        return prbs_stat_callback_test(unit, a);
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
}

/* end of prbs_stat */

/* start of fec_stat */
/* FECStat is a command that periodically collects FEC status counters such as
 * corrected and uncorrected FEC codewords for a given port. The command then
 * uses the corrected codewords to compute the pre-FEC bit error rate for the
 * channel. FECStat can be used to collect FEC status counters anytime there is
 * FEC traffic so there is no need to stop real traffic and dedicate the channel
 * for diagnostics purposes.
 */
#define FEC_STAT_F_INIT         (1 << 0)
#define FEC_STAT_F_RUNNING      (1 << 1)

#define FEC_STAT_INIT(f)        (f & FEC_STAT_F_INIT)
#define FEC_STAT_RUNNING(f)     (f & FEC_STAT_F_RUNNING)

#define FEC_STAT_LOCK(unit) \
    if (fec_stat_cb[unit].lock) { \
        sal_mutex_take(fec_stat_cb[unit].lock, sal_mutex_FOREVER);   \
    }

#define FEC_STAT_UNLOCK(unit) \
    if (fec_stat_cb[unit].lock) { \
        sal_mutex_give(fec_stat_cb[unit].lock); \
    }

typedef struct fec_stat_subcounter_s {
    uint64 corrected;
    uint64 uncorrected;
    uint64 symbol_error[PM_MAX_LANES]; /* Per-lane based symbol error count. RS FEC only.*/
} fec_stat_subcounter_t;

/*
 * Acc: accummulated from beginning/last clear
 * Cur: count since last time shown
 */
typedef struct fec_stat_counter_s {
    fec_stat_subcounter_t acc;
    fec_stat_subcounter_t cur;
} fec_stat_counter_t;

/*
 * Per port info
 */
typedef struct fec_stat_pinfo_s {
    int speed;
    int lanes;
    bcm_port_phy_fec_t fec_type;
    int intervals;
    fec_stat_counter_t counters;/* Counters per port for this FEC */
    double ber[PM_MAX_LANES];   /*  updated every interval */
} fec_stat_pinfo_t;

typedef struct fec_stat_cb_ {
    uint32 flags;
    int secs;
    int cfg_secs;
    bcm_pbmp_t pbmp;
    fec_stat_pinfo_t pinfo[BCM_PBMP_PORT_MAX];
    sal_thread_t thread_id;
    sal_mutex_t lock;
    sal_sem_t sem;
} fec_stat_cb_t;

fec_stat_cb_t fec_stat_cb[BCM_MAX_NUM_UNITS];

/*
 * Get FEC that is then used to key off which port phy control to issue
 * to get FEC corrected/uncorrected counters
 *
 * Note that the counters are based on either BASE_R or RS FEC. RS FEC
 * includes RS528, RS544, RS272 FEC
 */
STATIC void
fec_stat_fec_type_get(int unit, bcm_port_t port, bcm_port_phy_fec_t *fec_type)
{
    fec_stat_pinfo_t *fspi;

    fspi = &(fec_stat_cb[unit].pinfo[port]);

    *fec_type = fspi->fec_type;

    if ((fspi->fec_type == bcmPortPhyFecRs544) ||
        (fspi->fec_type == bcmPortPhyFecRs272) ||
        (fspi->fec_type == bcmPortPhyFecRs544_2xN) ||
        (fspi->fec_type == bcmPortPhyFecRs272_2xN)) {
        *fec_type = bcmPortPhyFecRsFec;
    }
}

/* This function will compute per-lane based BER for 1xN RS FEC and
 * per-port based BER for BaseR FEC and 2xN RS FEC.
 */
STATIC int
fec_stat_ber_compute(int unit, bcm_port_t port, uint32 delta, uint32* symb_delta,
                     sal_time_t secs, double *ber)
{
    int i;
    double rate;
    double nbits;
    uint32 sum_symb_delta = 0;
    fec_stat_pinfo_t *fspi;
    bcm_port_phy_fec_t fec_type;

    if (secs < 1) {
        return BCM_E_PARAM;
    }

    fspi = &(fec_stat_cb[unit].pinfo[port]);
    BCM_IF_ERROR_RETURN
        (stat_speed_rate_get(fspi->speed, fspi->lanes, fspi->fec_type, &rate));
    if (rate == 0) {
        cli_out("Error: Unsupported speed mode: %d\n", port);
        return BCM_E_UNAVAIL;
    }
    fec_stat_fec_type_get(unit, port, &fec_type);

    /* Rate needs to be adjusted from Gbps to bps */
    rate = rate * 1024. * 1024. * 1024.;
    /* Calculate per port BER for 2xN RS FEC based on sum of symbol error delta.
     * Calculate per lane BER for other RS FEC based on symbol error delta.
     * Calculate per port BER for other FEC types based on block error delta.
     */
    if ((fspi->fec_type == bcmPortPhyFecRs544_2xN) ||
        (fspi->fec_type == bcmPortPhyFecRs272_2xN)) {
        nbits = rate * fspi->lanes;
        for (i = 0; i < fspi->lanes; i++) {
            sum_symb_delta += symb_delta[i];
        }
        /* Per port BER stored in ber[0]. */
        ber[0] = sum_symb_delta / (nbits * secs);
        LOG_DEBUG(BSL_LS_APPL_PHY,
                  (BSL_META_U(unit, "Updated BER for port %d: %8.2e\n"),
                   port, ber[0]));
    } else if (fec_type == bcmPortPhyFecRsFec) {
        nbits = rate;
        for (i = 0; i < fspi->lanes; i++) {
            ber[i] = symb_delta[i] / (nbits * secs);
            LOG_DEBUG(BSL_LS_APPL_PHY,
                      (BSL_META_U(unit, "Updated BER for port %d lane %d: %8.2e\n"),
                       port, i, ber[i]));

        }
    } else {
        /* Per port BER stored in ber[0]. */
        nbits = rate * fspi->lanes;
        ber[0] = delta / (nbits * secs);
        LOG_DEBUG(BSL_LS_APPL_PHY,
                  (BSL_META_U(unit, "Updated BER for port %d: %8.2e\n"),
                   port, ber[0]));

    }

    return BCM_E_NONE;
}

STATIC void
fec_stat_counter_show(char *cname, char *pname, int* lane,
                      uint64 acc_counter, uint64 cur_counter, sal_time_t secs)
{
    char chdr[32];
    char buf[64];
    uint64 delta;

    if (lane != NULL) {
        sal_sprintf(chdr, "%s.%s[%d]", cname, pname, *lane);
    } else {
        sal_sprintf(chdr, "%s.%s", cname, pname);
    }

    delta = acc_counter;
    COMPILER_64_SUB_64(delta, cur_counter);
    if (!COMPILER_64_IS_ZERO(delta)) {
        format_uint64_decimal(buf, acc_counter, ',');
        LOG_CLI(("%-25s: %20s", chdr, buf));

        /* Show counters since last show */
        format_uint64_decimal(buf, delta, ',');
        LOG_CLI(("  %20s",buf));

        /* Show counters per second */
        COMPILER_64_UDIV_32(delta, (uint32)secs);
        format_uint64_decimal(buf, delta, ',');
        LOG_CLI(("  %12s/s\n",buf));
    }
}

STATIC int
fec_stat_port_counter(int unit, bcm_port_t port)
{
    int secs, lane, symb_err_valid = 0;
    bcm_port_phy_fec_t fec_type;
    fec_stat_cb_t *fscb;
    fec_stat_pinfo_t *fspi;
    fec_stat_counter_t *fsco;
    char *corrected_name;
    char *uncorrected_name;
    char *symbol_error_name = "RS_SYMBOL_ERR";
    fscb = &(fec_stat_cb[unit]);
    fspi = &(fscb->pinfo[port]);
    fsco = &(fspi->counters);

    fec_stat_fec_type_get(unit, port, &fec_type);
    if (fec_type == bcmPortPhyFecRsFec) {
        corrected_name = "CORREC_RS_CW";
        uncorrected_name = "UNCORREC_RS_CW";
        if ((fspi->fec_type != bcmPortPhyFecRs544_2xN) &&
            (fspi->fec_type != bcmPortPhyFecRs272_2xN)) {
            symb_err_valid = 1;
        }
    } else {
        corrected_name = "CORREC_BASER";
        uncorrected_name = "UNCORREC_BASER";
    }

    secs = fscb->secs * fspi->intervals;
    fec_stat_counter_show(corrected_name, BCM_PORT_NAME(unit, port), NULL,
                          fsco->acc.corrected, fsco->cur.corrected,
                          secs);
    fec_stat_counter_show(uncorrected_name, BCM_PORT_NAME(unit, port), NULL,
                          fsco->acc.uncorrected, fsco->cur.uncorrected,
                          secs);
    if (symb_err_valid) {
        for (lane = 0; lane < fspi->lanes; lane++) {
            fec_stat_counter_show(symbol_error_name, BCM_PORT_NAME(unit, port), &lane,
                                  fsco->acc.symbol_error[lane], fsco->cur.symbol_error[lane],
                                  secs);
            fsco->cur.symbol_error[lane] = fsco->acc.symbol_error[lane];
        }
    }
    fspi->intervals = 0;
    fsco->cur.corrected = fsco->acc.corrected;
    fsco->cur.uncorrected = fsco->acc.uncorrected;

    return BCM_E_NONE;
}


STATIC void
fec_stat_accumulate(int unit, bcm_port_t port, bcm_port_phy_control_t type,
                    uint64 *subc, uint32 *out_count)
{
    int rv;
    uint32 count;

    rv = bcm_port_phy_control_get(unit, port, type, &count);
    if (BCM_FAILURE(rv)) {
        count = 0;
    }
    if (count) {
        COMPILER_64_ADD_32(*subc, count);
    }
    *out_count = count;
}


STATIC int
fec_stat_ber_update(int unit, bcm_port_t port, uint32 delta, uint32* symb_delta)
{
    int rv = 0;
    fec_stat_cb_t *fscb;
    fec_stat_pinfo_t *fspi;

    fscb = &(fec_stat_cb[unit]);
    fspi = &(fscb->pinfo[port]);

    rv = fec_stat_ber_compute(unit, port, delta, symb_delta, fscb->secs, fspi->ber);

    return rv;
}

/*
 * If there is a port configuration change, update the new configuration
 * and clear counters, BER
 */
STATIC int
fec_stat_pinfo_update(int unit, bcm_port_t port)
{
    fec_stat_pinfo_t *fspi;
    fec_stat_counter_t *fsco;
    bcm_port_resource_t rsrc;

    fspi = &(fec_stat_cb[unit].pinfo[port]);

    BCM_IF_ERROR_RETURN(bcm_port_resource_speed_get(unit, port, &rsrc));
    if ((fspi->speed == rsrc.speed) &&
        (fspi->lanes == rsrc.lanes) &&
        (fspi->fec_type == rsrc.fec_type)) {
        return BCM_E_NONE;
    }

    LOG_DEBUG(BSL_LS_APPL_PHY,
              (BSL_META_U(unit, "Updating %s config\n"),
               BCM_PORT_NAME(unit, port)));

    fspi->speed = rsrc.speed;
    fspi->lanes = rsrc.lanes;
    fspi->fec_type = rsrc.fec_type;

    sal_memset(fspi->ber, 0, PM_MAX_LANES * sizeof(double));

    fsco = &(fspi->counters);
    sal_memset(&(fsco->acc), 0, sizeof(fec_stat_subcounter_t));
    sal_memset(&(fsco->cur), 0, sizeof(fec_stat_subcounter_t));

    return BCM_E_NONE;
}


STATIC cmd_result_t
fec_stat_collect(int unit, bcm_port_t port)
{
    bcm_error_t rv = BCM_E_NONE;
    uint32 count, corrected_count, error_count[PM_MAX_LANES];
    soc_port_phy_rsfec_symb_errcnt_t symbol_error_count;
    uint32 inst;
    int i, corrected_ctrl, uncorrected_ctrl, symbol_err_valid = 0;
    bcm_port_phy_fec_t fec_type;
    fec_stat_pinfo_t *fspi;
    fec_stat_counter_t *fsco;

    FEC_STAT_LOCK(unit);

    rv = fec_stat_pinfo_update(unit, port);
    if (rv != BCM_E_NONE) {
        FEC_STAT_UNLOCK(unit);
        return CMD_FAIL;
    }

    fspi = &(fec_stat_cb[unit].pinfo[port]);
    fsco = &(fspi->counters);

    fec_stat_fec_type_get(unit, port, &fec_type);

    if (fec_type == bcmPortPhyFecRsFec) {
        corrected_ctrl = BCM_PORT_PHY_CONTROL_FEC_CORRECTED_CODEWORD_COUNT;
        uncorrected_ctrl = BCM_PORT_PHY_CONTROL_FEC_UNCORRECTED_CODEWORD_COUNT;
        symbol_err_valid = 1;
    } else {   /* Assume other FEC is BaseR */
        corrected_ctrl = BCM_PORT_PHY_CONTROL_FEC_CORRECTED_BLOCK_COUNT;
        uncorrected_ctrl = BCM_PORT_PHY_CONTROL_FEC_UNCORRECTED_BLOCK_COUNT;
    }

    fec_stat_accumulate(unit, port, uncorrected_ctrl, &(fsco->acc.uncorrected),
                        &count);
    fec_stat_accumulate(unit, port, corrected_ctrl, &(fsco->acc.corrected),
                        &corrected_count);
    if (symbol_err_valid) {
        symbol_error_count.max_count = PM_MAX_LANES;
        symbol_error_count.symbol_errcnt = error_count;
        inst = PHY_DIAG_INSTANCE(PHY_DIAG_DEV_INT , PHY_DIAG_INTF_DFLT, PHY_DIAG_LN_DFLT);
        rv = port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_CMD,
                             PHY_DIAG_CTRL_RSFEC_SYMB_ERR, (void *)&symbol_error_count);
        if (rv != BCM_E_NONE) {
            FEC_STAT_UNLOCK(unit);
            return CMD_FAIL;
        }
        if (symbol_error_count.actual_count != fspi->lanes) {
            /* If actual_count returned by the API does not equal to
             * number of lanes stored in DB, it means the speed configs in
             * DB is not up-to-date.
             */
            FEC_STAT_UNLOCK(unit);
            return CMD_FAIL;
        }
        for (i = 0; i < fspi->lanes; i++) {
            COMPILER_64_ADD_32(fsco->acc.symbol_error[i], error_count[i]);
        }
    }

    rv = fec_stat_ber_update(unit, port, corrected_count, error_count);

    fspi->intervals++;

    FEC_STAT_UNLOCK(unit);
    return CMD_OK;
}

STATIC void
fec_stat_thread(int unit)
{
    int u_interval;
    bcm_port_t port;
    fec_stat_cb_t *fscb;

    fscb = &(fec_stat_cb[unit]);
    u_interval = fscb->secs * 1000000;

    LOG_CLI(("FEC stat thread started...\n"));
    while (fscb->secs) {
        BCM_PBMP_ITER(fscb->pbmp, port) {
            LOG_DEBUG(BSL_LS_APPL_PHY,
                      (BSL_META_U(unit, "Collect FEC for port %d\n"), port));
            if (fec_stat_collect(unit, port)) {
                LOG_ERROR(BSL_LS_APPL_PHY,
                          (BSL_META_U(
                              unit,
                              "Failed collect FEC stats for port %d\n"), port));
            }
        }
        sal_sem_take(fscb->sem, u_interval);
    }

    LOG_DEBUG(BSL_LS_APPL_PHY, ("FEC stat thread exiting...\n"));
    sal_memset(fscb, 0, sizeof(fec_stat_cb_t));
    fscb->thread_id = NULL;
    sal_thread_exit(0);
}


STATIC int
fec_stat_init(int unit)
{
    fec_stat_cb_t *fscb;

    fscb = &(fec_stat_cb[unit]);
    if (FEC_STAT_INIT(fscb->flags)) {
        return BCM_E_NONE;
    }
    sal_memset(fscb, 0, sizeof(fec_stat_cb_t));
    fscb->flags = FEC_STAT_F_INIT;

    if ((fscb->lock = sal_mutex_create("FECStat lock")) == NULL)
        return BCM_E_INTERNAL;

    if ((fscb->sem = sal_sem_create("FECStat sleep", sal_sem_BINARY, 0)) == NULL)
        return BCM_E_INTERNAL;

    return BCM_E_NONE;
}

STATIC int
fec_stat_clear(int unit, bcm_pbmp_t pbmp, args_t *a)
{
    bcm_port_t port;

    FEC_STAT_LOCK(unit);
    BCM_PBMP_ITER(pbmp, port) {
        sal_memset(&(fec_stat_cb[unit].pinfo[port].counters),
                   0, sizeof(fec_stat_counter_t));
    }
    FEC_STAT_UNLOCK(unit);
    return CMD_OK;
}

STATIC int
fec_stat_counters(int unit, bcm_pbmp_t pbmp, args_t *a)
{
    int rv = 0;
    bcm_port_t port;

    FEC_STAT_LOCK(unit);
    LOG_CLI(("%-20s\t\t%20s\t%20s\t%12s",
        "port", "accumulated count", "last count", "count per sec\n"));
    BCM_PBMP_ITER(pbmp, port) {
        rv = fec_stat_port_counter(unit, port);
        if (rv) {
            break;
        }
    }
    FEC_STAT_UNLOCK(unit);

    return rv ? CMD_FAIL : CMD_OK;
}

STATIC int
fec_stat_ber(int unit, bcm_pbmp_t pbmp, args_t *a)
{
    bcm_port_t port;
    fec_stat_pinfo_t *fspi;
    bcm_port_phy_fec_t fec_type;
    int lane;

    FEC_STAT_LOCK(unit);
    /* Show BER computed during last interval */
    LOG_CLI(("%-10s   %s", "port", "BER\n"));
    LOG_CLI(("====\n"));
    BCM_PBMP_ITER(pbmp, port) {
        fspi = &(fec_stat_cb[unit].pinfo[port]);
        fec_stat_fec_type_get(unit, port, &fec_type);
        if ((fspi->fec_type == bcmPortPhyFecRs544_2xN) ||
            (fspi->fec_type == bcmPortPhyFecRs272_2xN)) {
            if (fspi->ber[0]) {
                LOG_CLI(("%-10s:  %4.2e\n", BCM_PORT_NAME(unit, port), fspi->ber[0]));
            } else {
                LOG_CLI(("%-10s:  N/A\n", BCM_PORT_NAME(unit, port)));
            }
        } else if (fec_type == bcmPortPhyFecRsFec) {
            for (lane = 0; lane < fspi->lanes; lane++) {
                if (fspi->ber[lane]) {
                    LOG_CLI(("%-s[%d]:  %4.2e\n", BCM_PORT_NAME(unit, port), lane, fspi->ber[lane]));
                } else {
                    LOG_CLI(("%-s[%d]:  N/A\n", BCM_PORT_NAME(unit, port), lane));
                }
            }
        } else {
            if (fspi->ber[0]) {
                LOG_CLI(("%-10s:  %4.2e\n", BCM_PORT_NAME(unit, port), fspi->ber[0]));
            } else {
                LOG_CLI(("%-10s:  N/A\n", BCM_PORT_NAME(unit, port)));
            }
        }
    }
    FEC_STAT_UNLOCK(unit);
    return CMD_OK;
}


STATIC int
fec_stat_stop(int unit)
{
    fec_stat_cb_t *fscb;

    fscb = &(fec_stat_cb[unit]);
    if (FEC_STAT_RUNNING(fscb->flags)) {
        fscb->secs = 0;
        fscb->flags &= ~FEC_STAT_F_RUNNING;
        LOG_CLI(("Stopping FECStat thread\n"));
        sal_sem_give(fscb->sem);
    }
    return CMD_OK;
}

STATIC int
fec_stat_counter_init(int unit)
{
    uint32 inst, value, error_count[PM_MAX_LANES];
    soc_port_phy_rsfec_symb_errcnt_t symbol_error_count;
    bcm_port_phy_fec_t fec_type;
    bcm_port_t port;
    fec_stat_pinfo_t *fspi;
    fec_stat_cb_t *fscb;

    fscb = &(fec_stat_cb[unit]);

    BCM_PBMP_ITER(fscb->pbmp, port) {
        fspi = &(fec_stat_cb[unit].pinfo[port]);
        sal_memset(&(fspi->counters), 0, sizeof(fec_stat_counter_t));
        BCM_IF_ERROR_RETURN(fec_stat_pinfo_update(unit, port));

        /* Prime counters to avoid first read to get all values since reboot */
        fec_stat_fec_type_get(unit, port, &fec_type);
        if (fec_type == bcmPortPhyFecRsFec) {
            symbol_error_count.max_count = PM_MAX_LANES;
            symbol_error_count.symbol_errcnt = error_count;
            inst = PHY_DIAG_INSTANCE(PHY_DIAG_DEV_INT , PHY_DIAG_INTF_DFLT, PHY_DIAG_LN_DFLT);
            BCM_IF_ERROR_RETURN(port_diag_ctrl(unit, port, inst, PHY_DIAG_CTRL_CMD,
                                PHY_DIAG_CTRL_RSFEC_SYMB_ERR, (void *)&symbol_error_count));

            BCM_IF_ERROR_RETURN(bcm_port_phy_control_get(unit, port,
                                BCM_PORT_PHY_CONTROL_FEC_CORRECTED_CODEWORD_COUNT,
                                &value));
            BCM_IF_ERROR_RETURN(bcm_port_phy_control_get(unit, port,
                                BCM_PORT_PHY_CONTROL_FEC_UNCORRECTED_CODEWORD_COUNT,
                                &value));
        } else {
            BCM_IF_ERROR_RETURN(bcm_port_phy_control_get(unit, port,
                                BCM_PORT_PHY_CONTROL_FEC_CORRECTED_BLOCK_COUNT,
                                &value));
            BCM_IF_ERROR_RETURN(bcm_port_phy_control_get(unit, port,
                                BCM_PORT_PHY_CONTROL_FEC_UNCORRECTED_BLOCK_COUNT,
                                &value));
        }
    }
    return BCM_E_NONE;
}

STATIC int
fec_stat_start(int unit, bcm_pbmp_t pbmp, args_t *a)
{
    int secs, rv;
    parse_table_t   pt;
    fec_stat_cb_t *fscb;

    fscb = &(fec_stat_cb[unit]);

    if (ARG_CNT(a) == 0) {
        if (!(fscb->flags & FEC_STAT_F_RUNNING)) {
            LOG_CLI(("FECStat: not running\n"));
        } else {
            char pbmp_str[SOC_PBMP_FMT_LEN];
            SOC_PBMP_FMT(fscb->pbmp, pbmp_str);
            LOG_CLI(("FECStat: interval=%ds\n", fscb->secs));
            LOG_CLI(("FECStat: pbmp=%s\n", pbmp_str));
        }
        return CMD_OK;
    }

    /*SOC_PBMP_ASSIGN(pbmp, fscb->pbmp);*/
    secs = fscb->secs;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Interval", PQ_DFL|PQ_INT, 0, &secs, NULL);
    /*parse_table_add(&pt, "PortBitMap",PQ_DFL|PQ_PBMP, 0, &pbmp, NULL);*/

    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);

    if (secs == 0 || secs > 180) {
        LOG_CLI(("Interval must be between 1 and 180 seconds\n"));
        return CMD_FAIL;
    }

    /*
     * Allow start to be called while already running to update
     * interval and pbmp
     */
    if (fscb->flags & FEC_STAT_F_RUNNING) {
        FEC_STAT_LOCK(unit);
    }

    fscb->secs = secs;
    SOC_PBMP_ASSIGN(fscb->pbmp, pbmp);

    rv = fec_stat_counter_init(unit);

    if (fscb->flags & FEC_STAT_F_RUNNING) {
        /*
         * If fec_stat is already running,
         * need to return without starting new thread.
         */
        FEC_STAT_UNLOCK(unit);
        return (rv == BCM_E_NONE) ? CMD_OK : CMD_FAIL;
    }

    if (rv != BCM_E_NONE) {
        return CMD_FAIL;
    }

    fscb->thread_id = sal_thread_create("FECstat",
                                      SAL_THREAD_STKSZ,
                                      100,   /* thread priority */
                                      (void (*)(void*))fec_stat_thread,
                                      INT_TO_PTR(unit));
    if (fscb->thread_id == SAL_THREAD_ERROR) {
        LOG_ERROR(
            BSL_LS_APPL_PHY,
            (BSL_META_U(unit, "Could not create FECstat thread\n")));
        fscb->flags &= ~FEC_STAT_F_RUNNING;
        return CMD_FAIL;
    }
    fscb->flags |= FEC_STAT_F_RUNNING;

    return CMD_OK;
}

/*
 * Match input with key. Allows abbreviated matches based on
 * the minimum initial capital letters in key. For example,
 * key = ABCdef will be matched with the input
 * abc, abcd, abcde, abcdef. Any other input is a mismatch.
 */
STATIC int
fec_stat_cmd_match(char *inp, char *key)
{
    while (*key && isupper(*key)) {
        if (!*inp || (toupper(*inp) != *key)) {
            return 0;
        }
        key++;
        inp++;
    }
    while (*inp) {
        if (toupper(*inp) != toupper(*key)) {
            return 0;
        }
        key++;
        inp++;
    }
    return 1;
}

STATIC cmd_result_t
fec_stat_cfg(int unit)
{
    fec_stat_cb_t *fscb;

    fscb = &(fec_stat_cb[unit]);

    if (!FEC_STAT_RUNNING(fec_stat_cb[unit].flags)) {
        LOG_CLI(("FECStat not started\n"));
    } else {
        char buf[512];
        char pfmt[SOC_PBMP_FMT_LEN];

        format_bcm_pbmp(unit, buf, sizeof(buf), fscb->pbmp);
        LOG_CLI(("FECStat: Polling interval: %d sec\n", fscb->secs));
        LOG_CLI(("FECStat: Port bitmap: %s: %s\n",
                 SOC_PBMP_FMT(fscb->pbmp, pfmt), buf));
    }
    return CMD_OK;
}

/*** API ***/
/*
 * Get computed BER for the last <secs>
 *
 * If the port is not configured with FEC or the BER cannot be computed,
 * the BER is 0
 *
 * If successful, return 1; otherwise 0
 */
int
fec_stat_ber_get(int unit, bcm_port_t port, int *lane, double *ber)
{
    fec_stat_pinfo_t *fscb = &(fec_stat_cb[unit].pinfo[port]);

    FEC_STAT_LOCK(unit);
    if (lane == NULL) {
        *ber = fscb->ber[0];
    } else if ((*lane >= 0) && (*lane < PM_MAX_LANES)) {
        *ber = fscb->ber[*lane];
    } else {
        return 0;
    }

    FEC_STAT_UNLOCK(unit);

    return 1;

}

static char fec_stat_usage[] =
    "\nParameters: [STArt [Interval=<secs>]]\n"
        "    [STOp] [Counters] [Ber] [CLear]\n";

STATIC cmd_result_t
_phy_diag_fecstat(int unit, bcm_pbmp_t pbmp, args_t *a)
{
    char *sc;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (!SOC_IS_TOMAHAWK3(unit)) {
        return CMD_NOTIMPL;
    }

    sc = ARG_GET(a);
    if (sc == NULL) {
        return fec_stat_cfg(unit);
    }

    if (fec_stat_init(unit)) {
        return CMD_FAIL;
    }
    if (fec_stat_cmd_match(sc, "?")) {
         LOG_CLI(("%s\n", fec_stat_usage));
    }
    else if (fec_stat_cmd_match(sc, "STArt")) {
        return fec_stat_start(unit, pbmp, a);
    } else if (fec_stat_cmd_match(sc, "STOp")) {
        return fec_stat_stop(unit);
    } else if (fec_stat_cmd_match(sc, "Counters")) {
        return fec_stat_counters(unit, pbmp, a);
    } else if (fec_stat_cmd_match(sc, "Ber")) {
        return fec_stat_ber(unit, pbmp, a);
    } else if (fec_stat_cmd_match(sc, "CLear")) {
        return fec_stat_clear(unit, pbmp, a);
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
}

/* end of fec_stat */
#endif /* BCM_TOMAHAWK3_SUPPORT */

/*
 * Diagnostic utilities for serdes and PHY devices.
 *
 * Command format used in BCM diag shell:
 * phy diag <pbm> <sub_cmd> [sub cmd parameters]
 * All sub commands take two general parameters: unit and if. This identifies
 * the instance the command targets to.
 * unit = 0,1, ....
 *   unit takes numeric values identifying the instance of the PHY devices
 *   associated with the given port. A value 0 indicates the internal
 *   PHY(serdes) the one directly connected to the MAC. A value 1 indicates
 *   the first external PHY.
 * if(interface) = [sys | line]
 *   interface identifies the system side interface or line side interface of
 *   PHY device.
 * The list of sub commands:
 *   dsc - display tx/rx equalization information. Warpcore(WC) only.
 *   veye - vertical eye margin mesurement. WC only. All eye margin functions
 *          are used in conjunction with PRBS utility in the configed speed mode
 *   heye_r - right horizontal eye margin mesurement. WC only
 *   heye_l - right horizontal eye margin mesurement. WC only
 *   for the WarpLite, there are three additional parameters, live link or not, par1 for the target BER value for example: -18 stands for 10^(-18)
 *   par2 will be percentage range for example, 5 means checxk for +-%5 of the target BER
 *   loopback - put the device in the given loopback mode
 *              parameter: mode=[remote | local | none]
 *   prbs - perform various PRBS functions. Takes all parameters of the
 *          "phy prbs" command except the mode parameter.
 *          Example:
 *            A port has a WC serdes and 84740 PHY connected. A typical usage
 *            is to use the PRBS to check the link between WC and the system
 *            side of the 84740. Use port xe0 as an example:
 *            BCM.0> phy diag xe0 prbs set unit=0 p=3
 *            BCM.0> phy diag xe0 prbs set unit=1 if=sys p=3
 *            BCM.0> phy diag xe0 prbs get unit=1 if=sys
 *            BCM.0> phy diag xe0 prbs get unit=0
 *
 *   prbsstat - periodically collect PRBS error counters an
 *           compute BER based on the port configuration an
 *           the observed error counters.
 *           Parameters:
 *             [STArt [Interval=<secs>]]
 *             [STOp]
 *             [Counters]
 *             [Ber]
 *             [Clear]
 *          Example:
 *            BCM.0> phy diag cd0-cd3 prbsstat
 *            BCM.0> phy diag cd0-cd3 prbsstat start i=60
 *            BCM.0> phy diag cd0-cd3 prbsstat counters
 *            BCM.0> phy diag cd0-cd3 prbsstat ber
 *            BCM.0> phy diag cd0-cd3 prbsstat clear
 *   pcs - display Diag information
 *            BCM.0> phy diag xe0 pcs
 *            BCM.0> phy diag xe0 pcs topology
 *            BCM.0> phy diag xe0 pcs link
 *            BCM.0> phy diag xe0 pcs aneg
 *            BCM.0> phy diag xe0 pcs state
 *            BCM.0> phy diag xe0 pcs tfc
 *            BCM.0> phy diag xe0 pcs antimers
 *
 *   dsc - display Diag information
 *            BCM.0> phy diag xe0 dsc
 *            BCM.0> phy diag xe0 dsc config
 *            BCM.0> phy diag xe0 dsc cl72
 *            BCM.0> phy diag xe0 dsc debug
 *
 *   mfg - run manufacturing test on PHY BCM8483X
 *              parameter: (t)est=num (d)ata=<val> (f)ile=filename
 *              Test should be :
 *              1 (HYB_CANC), needs (f)ile=filename
 *              2 (DENC),     needs (f)ile=filename
 *              3 (TX_ON)     needs (d)ata = bit map for turning TX off on pairs DCBA
 *              0 (EXIT)
 */
STATIC cmd_result_t
_if_esw_phy_diag(int unit, args_t *args)
{
    bcm_port_t port, dport;
    bcm_pbmp_t pbmp;
    int rv, cmd;
    int par[5];
    int *pData;
    int lscan_time;
    char *cmd_str, *port_str;
    parse_table_t pt;
    bcm_port_t p;
    int lnk_mode[BCM_PBMP_PORT_MAX];

    rv = CMD_OK;
    par[0] = 0;
    par[1] = 0;
    par[2] = 0;
    par[3] = 0;
    par[4] = 0;
    pData = &par[0];

    sal_memset(&lnk_mode, -1, sizeof(lnk_mode));

    if ((port_str = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    BCM_PBMP_CLEAR(pbmp);
    if (parse_bcm_pbmp(unit, port_str, &pbmp) < 0) {
        cli_out("Error: unrecognized port bitmap: %s\n", port_str);
        return CMD_FAIL;
    }

    if ((cmd_str = ARG_GET(args)) == NULL) {
        return CMD_USAGE;
    }

    /* linkscan should be disabled. soc_phyctrl_diag_ctrl() doesn't
     * assume exclusive access to the device.
     */
    BCM_IF_ERROR_RETURN(bcm_linkscan_enable_get(unit, &lscan_time));
    if (lscan_time != 0) {
        BCM_PBMP_ITER(pbmp, p) {
            BCM_IF_ERROR_RETURN(bcm_linkscan_mode_get(unit, p, &lnk_mode[p]));
            if (lnk_mode[p] != BCM_LINKSCAN_MODE_NONE) {
                BCM_IF_ERROR_RETURN(bcm_linkscan_mode_set(unit, p, BCM_LINKSCAN_MODE_NONE));
            }
        }
    }
    if (sal_strcasecmp(cmd_str, "peek") == 0) {
        cmd = PHY_DIAG_CTRL_PEEK;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "fb", PQ_DFL | PQ_INT, (void *)(0), pData++, NULL);
        parse_table_add(&pt, "lane", PQ_DFL | PQ_INT, (void *)(0),
                        pData++, NULL);
        if (parse_arg_eq(args, &pt) < 0) {
            cli_out("Error: invalid option: %s\n", ARG_CUR(args));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        /* command targets to Internal serdes device for now */
        /* coverity[overrun-local] */
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            if (port_diag_ctrl(unit, port, PHY_DIAG_INT,
                                      PHY_DIAG_CTRL_CMD, cmd,
                                      &par) != SOC_E_NONE) {
                rv = CMD_FAIL;
            }
        }
    } else if (sal_strcasecmp(cmd_str, "poke") == 0) {
        cmd = PHY_DIAG_CTRL_POKE;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "fb", PQ_DFL | PQ_INT, (void *)(0), pData++, NULL);
        parse_table_add(&pt, "val", PQ_DFL | PQ_INT, (void *)(0),
                        pData++, NULL);
        if (parse_arg_eq(args, &pt) < 0) {
            cli_out("Error: invalid option: %s\n", ARG_CUR(args));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        /* command targets to Internal serdes device for now */
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            if (port_diag_ctrl(unit, port, PHY_DIAG_INT,
                                      PHY_DIAG_CTRL_CMD, cmd,
                                      &par) != SOC_E_NONE) {
                rv = CMD_FAIL;
            }
        }
    } else if (sal_strcasecmp(cmd_str, "load_uc") == 0) {
        cmd = PHY_DIAG_CTRL_LOAD_UC;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "crc", PQ_DFL | PQ_INT, (void *)(0),
                        pData++, NULL);
        parse_table_add(&pt, "debug", PQ_DFL | PQ_INT, (void *)(0),
                        pData++, NULL);
        if (parse_arg_eq(args, &pt) < 0) {
            cli_out("Error: invalid option: %s\n", ARG_CUR(args));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        /* command targets to Internal serdes device for now */
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            if (port_diag_ctrl(unit, port, PHY_DIAG_INT,
                                      PHY_DIAG_CTRL_CMD, cmd,
                                      &par) != SOC_E_NONE) {
                rv = CMD_FAIL;
            }
        }
    } else if (sal_strcasecmp(cmd_str, "veye") == 0) {
        cmd = PHY_DIAG_CTRL_EYE_MARGIN_VEYE;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "live", PQ_DFL | PQ_INT, (void *)(0),
                        pData++, NULL);
        parse_table_add(&pt, "BER", PQ_DFL | PQ_INT, (void *)(0),
                        pData++, NULL);
        parse_table_add(&pt, "range", PQ_DFL | PQ_INT, (void *)(0),
                        pData++, NULL);
        parse_table_add(&pt, "lane", PQ_DFL | PQ_INT, (void *)(0),
                        pData++, NULL);
        parse_table_add(&pt, "time_upper_bound", PQ_DFL | PQ_INT, (void *)(0),
                        pData, NULL);
        if (parse_arg_eq(args, &pt) < 0) {
            cli_out("Error: invalid option: %s\n", ARG_CUR(args));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        /* command targets to Internal serdes device for now */
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            if (port_diag_ctrl(unit, port, PHY_DIAG_INT,
                                      PHY_DIAG_CTRL_CMD, cmd,
                                      &par) != SOC_E_NONE) {
                rv = CMD_FAIL;
            }
        }
    } else if (sal_strcasecmp(cmd_str, "veye_u") == 0) {
        cmd = PHY_DIAG_CTRL_EYE_MARGIN_VEYE_UP;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "live", PQ_DFL | PQ_INT, (void *)(0),
                        pData++, NULL);
        parse_table_add(&pt, "BER", PQ_DFL | PQ_INT, (void *)(0),
                        pData++, NULL);
        parse_table_add(&pt, "range", PQ_DFL | PQ_INT, (void *)(0),
                        pData++, NULL);
        parse_table_add(&pt, "lane", PQ_DFL | PQ_INT, (void *)(0),
                        pData++, NULL);
        parse_table_add(&pt, "time_upper_bound", PQ_DFL | PQ_INT, (void *)(0),
                        pData, NULL);
        if (parse_arg_eq(args, &pt) < 0) {
            cli_out("Error: invalid option: %s\n", ARG_CUR(args));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        /* command targets to Internal serdes device for now */
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            if (port_diag_ctrl(unit, port, PHY_DIAG_INT,
                                      PHY_DIAG_CTRL_CMD, cmd,
                                      &par) != SOC_E_NONE) {
                rv = CMD_FAIL;
            }
        }
    } else if (sal_strcasecmp(cmd_str, "veye_d") == 0) {
        cmd = PHY_DIAG_CTRL_EYE_MARGIN_VEYE_DOWN;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "live", PQ_DFL | PQ_INT, (void *)(0),
                        pData++, NULL);
        parse_table_add(&pt, "BER", PQ_DFL | PQ_INT, (void *)(0),
                        pData++, NULL);
        parse_table_add(&pt, "range", PQ_DFL | PQ_INT, (void *)(0),
                        pData, NULL);
        if (parse_arg_eq(args, &pt) < 0) {
            cli_out("Error: invalid option: %s\n", ARG_CUR(args));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        /* command targets to Internal serdes device for now */
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            if (port_diag_ctrl(unit, port, PHY_DIAG_INT,
                                      PHY_DIAG_CTRL_CMD, cmd,
                                      &par) != SOC_E_NONE) {
                rv = CMD_FAIL;
            }
        }
    } else if (sal_strcasecmp(cmd_str, "heye_r") == 0) {
        cmd = PHY_DIAG_CTRL_EYE_MARGIN_HEYE_RIGHT;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "live", PQ_DFL | PQ_INT, (void *)(0),
                        pData++, NULL);
        parse_table_add(&pt, "BER", PQ_DFL | PQ_INT, (void *)(0),
                        pData++, NULL);
        parse_table_add(&pt, "range", PQ_DFL | PQ_INT, (void *)(0),
                        pData++, NULL);
        parse_table_add(&pt, "lane", PQ_DFL | PQ_INT, (void *)(0),
                        pData++, NULL);
        parse_table_add(&pt, "time_upper_bound", PQ_DFL | PQ_INT, (void *)(0),
                        pData, NULL);
        if (parse_arg_eq(args, &pt) < 0) {
            cli_out("Error: invalid option: %s\n", ARG_CUR(args));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        /* command targets to Internal serdes device for now */
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            if (port_diag_ctrl(unit, port, PHY_DIAG_INT,
                                      PHY_DIAG_CTRL_CMD, cmd,
                                      &par) != SOC_E_NONE) {
                rv = CMD_FAIL;
            }
        }
    } else if (sal_strcasecmp(cmd_str, "heye_l") == 0) {
        cmd = PHY_DIAG_CTRL_EYE_MARGIN_HEYE_LEFT;
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "live", PQ_DFL | PQ_INT, (void *)(0),
                        pData++, NULL);
        parse_table_add(&pt, "BER", PQ_DFL | PQ_INT, (void *)(0),
                        pData++, NULL);
        parse_table_add(&pt, "range", PQ_DFL | PQ_INT, (void *)(0),
                        pData++, NULL);
        parse_table_add(&pt, "lane", PQ_DFL | PQ_INT, (void *)(0),
                        pData++, NULL);
        parse_table_add(&pt, "time_upper_bound", PQ_DFL | PQ_INT, (void *)(0),
                        pData, NULL);
        if (parse_arg_eq(args, &pt) < 0) {
            cli_out("Error: invalid option: %s\n", ARG_CUR(args));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        /* command targets to Internal serdes device for now */
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            if (port_diag_ctrl(unit, port, PHY_DIAG_INT,
                                      PHY_DIAG_CTRL_CMD, cmd,
                                      &par) != SOC_E_NONE) {
                rv = CMD_FAIL;
            }
        }

    } else if (sal_strcasecmp(cmd_str, "dsc") == 0) {
        rv = _phy_diag_dsc(unit, pbmp, args);
    } else if (sal_strcasecmp(cmd_str, "pcs") == 0) {
        rv = _phy_diag_pcs(unit, pbmp, args);
    } else if (sal_strcasecmp(cmd_str, "reg") == 0) {
        rv = _phy_diag_reg(unit, pbmp, args);
    } else if (sal_strcasecmp(cmd_str, "eyescan") == 0) {
        rv = _phy_diag_eyescan(unit, pbmp, args);
    } else if (sal_strcasecmp(cmd_str, "LoopBack") == 0) {
        rv = _phy_diag_loopback(unit, pbmp, args);
#ifdef BCM_TOMAHAWK3_SUPPORT
    } else if (sal_strcasecmp(cmd_str, "prbsstat") == 0) {
        rv = _phy_diag_prbsstat(unit, pbmp, args);
    } else if (sal_strcasecmp(cmd_str, "fecstat") == 0) {
        rv = _phy_diag_fecstat(unit, pbmp, args);
#endif /* BCM_TOMAHAWK3_SUPPORT */
    } else if (sal_strcasecmp(cmd_str, "prbs") == 0) {
        rv = _phy_diag_prbs(unit, pbmp, args);
    } else if (sal_strcasecmp(cmd_str, "mfg") == 0) {
        rv = _phy_diag_mfg(unit, pbmp, args);
    } else if (sal_strcasecmp(cmd_str, "state") == 0) {
        rv = _phy_diag_state(unit, pbmp, args);
    } else if (sal_strcasecmp(cmd_str, "feyescan") == 0) {
        rv = _phy_diag_fast_eyescan(unit, pbmp, args);
    } else if (sal_strcasecmp(cmd_str, "linkmon") == 0) {
        rv = _phy_diag_link_mon(unit, pbmp, args);
    } else if (sal_strcasecmp(cmd_str, "berproj") == 0) {
        rv = _phy_diag_berproj(unit, pbmp, args);
#ifdef PHYMOD_LINKCAT_BLACKHAWK_SUPPORT
    } else if (sal_strcasecmp(cmd_str, "linkcat") == 0) {
        rv = _phy_diag_linkcat(unit, pbmp, args);
#endif /* PHYMOD_LINKCAT_BLACKHAWK_SUPPORT */
    }

#ifdef SW_AUTONEG_SUPPORT
    else if (sal_strcasecmp(cmd_str, "sw_an") == 0) {

        if (soc_feature(unit, soc_feature_sw_autoneg)) {

            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
                if (bcm_sw_an_port_diag(unit, port)) {
                    rv = CMD_FAIL;
                }
            }
        } else {
            cli_out("Error: SW AUTONEG Not supported on this platform: \n");
            rv = CMD_FAIL;
        }
    }
#endif
    else {
        cli_out("Error: unrecognized PHY DIAG command: %s\n", cmd_str);
        rv = CMD_FAIL;
    }

    if (lscan_time != 0) {
        BCM_PBMP_ITER(pbmp, p) {
            BCM_IF_ERROR_RETURN(bcm_linkscan_mode_set(unit, p, lnk_mode[p]));
        }
    }

    return rv;
}


STATIC cmd_result_t
_if_esw_phy_longreach(int u, args_t *a)
{
    soc_pbmp_t pbm;
    soc_port_t p, dport;
    char *c;
    int i;
    cmd_result_t cmd_rv;
    parse_table_t pt;
    int print_header;
    uint32 flags;
    uint32 longreach_speed, longreach_pairs;
    uint32 longreach_gain, longreach_autoneg;
    uint32 longreach_local_ability, longreach_remote_ability;
    uint32 longreach_current_ability, longreach_master;
    uint32 longreach_active, longreach_enable;

    if (((c = ARG_GET(a)) == NULL) || (parse_bcm_pbmp(u, c, &pbm) < 0)) {
        cli_out("%s: ERROR: unrecognized port bitmap: %s\n", ARG_CMD(a), c);
        return CMD_FAIL;
    }

    {

        if (c[0] == '=') {
            return CMD_USAGE;       /* '=' unsupported */
        }

        parse_table_init(u, &pt);

        parse_table_add(&pt, "SPeed", PQ_INT | PQ_DFL | PQ_NO_EQ_OPT,
                        0, &longreach_speed, 0);
        parse_table_add(&pt, "PAirs", PQ_INT | PQ_DFL | PQ_NO_EQ_OPT,
                        0, &longreach_pairs, 0);
        parse_table_add(&pt, "GAin", PQ_INT | PQ_DFL | PQ_NO_EQ_OPT,
                        0, &longreach_gain, 0);
        parse_table_add(&pt, "AutoNeg", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                        0, &longreach_autoneg, 0);
        parse_table_add(&pt, "LocalAbility",
                        PQ_LR_PHYAB | PQ_DFL | PQ_NO_EQ_OPT, 0,
                        &longreach_local_ability, 0);
        parse_table_add(&pt, "RemoteAbility",
                        PQ_LR_PHYAB | PQ_DFL | PQ_NO_EQ_OPT, 0,
                        &longreach_remote_ability, 0);
        parse_table_add(&pt, "CurrentAbility",
                        PQ_LR_PHYAB | PQ_DFL | PQ_NO_EQ_OPT, 0,
                        &longreach_current_ability, 0);
        parse_table_add(&pt, "MAster", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT, 0,
                        &longreach_master, 0);
        parse_table_add(&pt, "Active", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT, 0,
                        &longreach_active, 0);
        parse_table_add(&pt, "ENable", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT, 0,
                        &longreach_enable, 0);

        if (parse_arg_eq(a, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }
        if (ARG_CNT(a) > 0) {
            cli_out("%s: Unknown argument %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }

        flags = 0;

        for (i = 0; i < pt.pt_cnt; i++) {
            if (pt.pt_entries[i].pq_type & PQ_PARSED) {
                flags |= (1 << (SOC_PHY_CONTROL_LONGREACH_SPEED + i));
            }
        }
        /* free allocated memory from arg parsing */
        parse_arg_eq_done(&pt);

        /* coverity[overrun-local] */
        DPORT_BCM_PBMP_ITER(u, pbm, dport, p) {
            print_header = FALSE;

            cli_out("\nCurrent Longreach settings of %s ->\n",
                    BCM_PORT_NAME(u, p));

            /* Read and set the longreach speed */
            cmd_rv = port_phy_control_update(u, p,
                                             BCM_PORT_PHY_CONTROL_LONGREACH_SPEED,
                                             longreach_speed,
                                             flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

            /* Read and set the longreach pairs */
            cmd_rv = port_phy_control_update(u, p,
                                             BCM_PORT_PHY_CONTROL_LONGREACH_PAIRS,
                                             longreach_pairs,
                                             flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

            /* Read and set the longreach gain */
            cmd_rv = port_phy_control_update(u, p,
                                             BCM_PORT_PHY_CONTROL_LONGREACH_GAIN,
                                             longreach_gain,
                                             flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

            /* Read and set the longreach autoneg (LDS) */
            cmd_rv = port_phy_control_update(u, p,
                                             BCM_PORT_PHY_CONTROL_LONGREACH_AUTONEG,
                                             longreach_autoneg,
                                             flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

            /* Read and set the longreach local ability */
            cmd_rv = port_phy_control_update(u, p,
                                             BCM_PORT_PHY_CONTROL_LONGREACH_LOCAL_ABILITY,
                                             longreach_local_ability,
                                             flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }
            /* Read the longreach remote ability */
            cmd_rv = port_phy_control_update(u, p,
                                             BCM_PORT_PHY_CONTROL_LONGREACH_REMOTE_ABILITY,
                                             longreach_remote_ability,
                                             flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

            /* Read the longreach current ability (GCD - read only) */
            cmd_rv = port_phy_control_update(u, p,
                                             BCM_PORT_PHY_CONTROL_LONGREACH_CURRENT_ABILITY,
                                             longreach_current_ability,
                                             flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

            /* Read and set the longreach master (when no LDS) */
            cmd_rv = port_phy_control_update(u, p,
                                             BCM_PORT_PHY_CONTROL_LONGREACH_MASTER,
                                             longreach_master,
                                             flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }
            /* Read and set the longreach active (LR is active - read only) */
            cmd_rv = port_phy_control_update(u, p,
                                             BCM_PORT_PHY_CONTROL_LONGREACH_ACTIVE,
                                             longreach_active,
                                             flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }
            /* Read and set the longreach active (Enable LR) */
            cmd_rv = port_phy_control_update(u, p,
                                             BCM_PORT_PHY_CONTROL_LONGREACH_ENABLE,
                                             longreach_enable,
                                             flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

        }

    }

    return CMD_OK;
}

STATIC cmd_result_t
_if_esw_phy_extlb(int u, args_t *a)
{
    soc_pbmp_t pbm;
    soc_port_t p, dport;
    char *c;
    int i;
    cmd_result_t cmd_rv;
    parse_table_t pt;
    int print_header;
    uint32 flags;
    uint32 enable;

    if (((c = ARG_GET(a)) == NULL) || (parse_bcm_pbmp(u, c, &pbm) < 0)) {
        cli_out("%s: ERROR: unrecognized port bitmap: %s\n", ARG_CMD(a), c);
        return CMD_FAIL;
    }

    {

        if (c[0] == '=') {
            return CMD_USAGE;       /* '=' unsupported */
        }

        parse_table_init(u, &pt);

        parse_table_add(&pt, "ENable", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                        0, &enable, 0);

        if (parse_arg_eq(a, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }

        if (ARG_CNT(a) > 0) {
            cli_out("%s: Unknown argument %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }

        flags = 0;

        for (i = 0; i < pt.pt_cnt; i++) {
            if (pt.pt_entries[i].pq_type & PQ_PARSED) {
                flags |= (1 << (SOC_PHY_CONTROL_LOOPBACK_EXTERNAL + i));
            }
        }
        /* free allocated memory from arg parsing */
        parse_arg_eq_done(&pt);

        /* coverity[overrun-local] */
        DPORT_BCM_PBMP_ITER(u, pbm, dport, p) {
            print_header = FALSE;

            cli_out("\nExternal loopback plug mode setting of %s ->\n",
                    BCM_PORT_NAME(u, p));

            /* Get and set the external loopback plug mode */
            cmd_rv = port_phy_control_update(u, p,
                                             BCM_PORT_PHY_CONTROL_LOOPBACK_EXTERNAL,
                                             enable, flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }

        }

    }

    return CMD_OK;
}

STATIC cmd_result_t
_if_esw_phy_clock(int u, args_t *a)
{
    soc_pbmp_t pbm;
    soc_port_t p, dport;
    char *c;
    int i;
    cmd_result_t cmd_rv;
    parse_table_t pt;
    int print_header;
    uint32 flags;
    uint32 pri_enable, sec_enable, frequency, mode_auto, auto_sec, source, base, offset;

    if (((c = ARG_GET(a)) == NULL) || (parse_bcm_pbmp(u, c, &pbm) < 0)) {
        cli_out("%s: ERROR: unrecognized port bitmap: %s\n", ARG_CMD(a), c);
        return CMD_FAIL;
    }

    {

        if (c[0] == '=') {
            return CMD_USAGE;       /* '=' unsupported */
        }

        parse_table_init(u, &pt);

        parse_table_add(&pt, "PRImary", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                        0, &pri_enable, 0);
        parse_table_add(&pt, "SECondary", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                        0, &sec_enable, 0);
        parse_table_add(&pt, "FRequency", PQ_INT | PQ_DFL | PQ_NO_EQ_OPT,
                        0, &frequency, 0);
        parse_table_add(&pt, "AutoDisable", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                        0, &mode_auto, 0);
        parse_table_add(&pt, "AutoSECondary", PQ_BOOL | PQ_DFL | PQ_NO_EQ_OPT,
                        0, &auto_sec, 0);
        parse_table_add(&pt, "ClockSource", PQ_INT | PQ_DFL | PQ_NO_EQ_OPT,
                        0, &source, 0);

        parse_table_add(&pt, "BAse", PQ_INT | PQ_DFL | PQ_NO_EQ_OPT,
                        0, &base, 0);
        parse_table_add(&pt, "OFfset", PQ_INT | PQ_DFL | PQ_NO_EQ_OPT,
                        0, &offset, 0);

        if (parse_arg_eq(a, &pt) < 0) {
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }

        if (ARG_CNT(a) > 0) {
            cli_out("%s: Unknown argument %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_USAGE;
        }

        flags = 0;

        for (i = 0; i < pt.pt_cnt; i++) {
            if (pt.pt_entries[i].pq_type & PQ_PARSED) {
                /* flags |= (1 << (SOC_PHY_CONTROL_CLOCK_ENABLE + i)); */
                flags |= (1 << i);
            }
        }
        /* free allocated memory from arg parsing */
        parse_arg_eq_done(&pt);

        /* coverity[overrun-local] */
        DPORT_BCM_PBMP_ITER(u, pbm, dport, p) {
            print_header = FALSE;

            cli_out("Clock extraction setting of %s ->\n",
                    BCM_PORT_NAME(u, p));

            cmd_rv = port_phy_control_update(u, p,
                                             BCM_PORT_PHY_CONTROL_CLOCK_ENABLE,
                                             pri_enable,
                                             flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }
            cmd_rv = port_phy_control_update(u, p,
                                             BCM_PORT_PHY_CONTROL_CLOCK_SECONDARY_ENABLE,
                                             sec_enable,
                                             flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }
            cmd_rv = port_phy_control_update(u, p,
                                             BCM_PORT_PHY_CONTROL_CLOCK_FREQUENCY,
                                             frequency,
                                             flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }
            cmd_rv = port_phy_control_update(u, p,
                                             BCM_PORT_PHY_CONTROL_CLOCK_MODE_AUTO,
                                             mode_auto,
                                             flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }
            cmd_rv = port_phy_control_update(u, p,
                                             BCM_PORT_PHY_CONTROL_CLOCK_AUTO_SECONDARY,
                                             auto_sec,
                                             flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }
            cmd_rv = port_phy_control_update(u, p,
                                             BCM_PORT_PHY_CONTROL_CLOCK_SOURCE,
                                             source,
                                             flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }
            cmd_rv = port_phy_control_update(u, p,
                                             BCM_PORT_PHY_CONTROL_PORT_PRIMARY,
                                             base, flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }
            cmd_rv = port_phy_control_update(u, p,
                                             BCM_PORT_PHY_CONTROL_PORT_OFFSET,
                                             offset, flags, &print_header);
            if (cmd_rv != CMD_OK) {
                return cmd_rv;
            }
        } /* DPORT_BCM_PBMP_ITER( ) */
    }
    return CMD_OK;
}

STATIC cmd_result_t
_if_esw_phy_wr(int u, args_t *a)
{
    char *c;
    cmd_result_t cmd_rv;
    bcm_port_t port;
    uint32 block;
    uint32 address;
    uint32 value;

    /* Get port */
    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    port = sal_ctoi(c, 0);
    if (!SOC_PORT_VALID(u, port)) {
        cli_out("%s: Invalid port\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    /* Get block */
    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    block = sal_ctoi(c, 0);

    /* Get address */
    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    address = sal_ctoi(c, 0);

    /* Get value */
    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    value = sal_ctoi(c, 0);

    /* Write the phy register */
    cmd_rv = bcm_port_phy_set(u, port, BCM_PORT_PHY_INTERNAL,
                              BCM_PORT_PHY_REG_INDIRECT_ADDR
                              (0, block, address), value);
    return cmd_rv;
}

STATIC cmd_result_t
_if_esw_phy_rd_cp(int u, args_t *a)
{
    char *c;
    cmd_result_t cmd_rv;
    bcm_port_t port;
    uint32 block;
    uint32 address;
    uint32 value;
    uint32 rval;

    /* Get port */
    c = ARG_GET(a);
    port = sal_ctoi(c, 0);
    if (!SOC_PORT_VALID(u, port)) {
        cli_out("%s: Invalid port\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    /* Get block */
    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    block = sal_ctoi(c, 0);

    /* Get address */
    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    address = sal_ctoi(c, 0);

    /* Get compare value */
    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    value = sal_ctoi(c, 0);

    /* Read the phy register */
    cmd_rv = bcm_port_phy_get(u, port, BCM_PORT_PHY_INTERNAL,
                              BCM_PORT_PHY_REG_INDIRECT_ADDR
                              (0, block, address), &rval);

    if (value != rval) {
        cli_out("Error: block %x, register %x expected %x, got %x\n",
                block, address, value, rval);
    } else {
        cli_out("Pass\n");
    }
    return cmd_rv;
}

STATIC cmd_result_t
_if_esw_phy_rd_cp2(int u, args_t *a)
{
    char *c;
    cmd_result_t cmd_rv;
    bcm_port_t port;
    uint32 block;
    uint32 address;
    uint32 value;
    uint32 mask;
    uint32 rval;

    /* Get port */
    c = ARG_GET(a);
    port = sal_ctoi(c, 0);
    if (!SOC_PORT_VALID(u, port)) {
        cli_out("%s: Invalid port\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    /* Get block */
    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    block = sal_ctoi(c, 0);

    /* Get address */
    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    address = sal_ctoi(c, 0);

    /* Get compare value */
    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    value = sal_ctoi(c, 0);

    /* Get mask */
    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    mask = sal_ctoi(c, 0);

    /* Read the phy register */
    cmd_rv = bcm_port_phy_get(u, port, BCM_PORT_PHY_INTERNAL,
                              BCM_PORT_PHY_REG_INDIRECT_ADDR
                              (0, block, address), &rval);

    if ((value & mask) != (rval & mask)) {
        cli_out("Error: block %x, register %x expected %x, got %x\n",
                block, address, (value & mask), (rval & mask));
    } else {
        cli_out("Pass\n");
    }
    return cmd_rv;
}

STATIC cmd_result_t
_if_esw_phy_mod(int u, args_t *a)
{
    char *c;
    cmd_result_t cmd_rv;
    bcm_port_t port;
    int block;
    uint32 flags;
    uint32 address;
    uint32 value;
    uint32 mask;

    /* Get port */
    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    port = sal_ctoi(c, 0);
    if (!SOC_PORT_VALID(u, port)) {
        cli_out("%s: Invalid port\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    /* Get block */
    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    block = sal_ctoi(c, 0);

    /* Get address */
    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    address = sal_ctoi(c, 0);

    /* Get value */
    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    value = sal_ctoi(c, 0);

    /* Get mask */
    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }
    mask = sal_ctoi(c, 0);

    /* Modify the phy register */
    flags = 0;
    if (block >= 0) {
        flags = BCM_PORT_PHY_INTERNAL;
        address = BCM_PORT_PHY_REG_INDIRECT_ADDR(0, block, address);
    }
    cmd_rv = bcm_port_phy_modify(u, port, flags, address, value, mask);

    return cmd_rv;
}

STATIC cmd_result_t
_if_esw_phy_control(int u, args_t *a)
{
    soc_pbmp_t pbm;
    soc_port_t p, dport;
    char *c;
    uint32 wan_mode, preemphasis, predriver_current, driver_current, dfe, lp_dfe, br_dfe, linkTraining;
    uint32 sw_rx_los_nval = 0, sw_rx_los_oval;
    uint32 eq_boost;
    uint32 interface;
    uint32 interfacemax;
    uint32 flags;
    int print_header;
    cmd_result_t cmd_rv;
    bcm_error_t bcm_rv;
    bcm_port_config_t pcfg;
    int eq_tune = FALSE;
    int eq_status = FALSE;
    int dump = FALSE;
    int farEndEqValue = 0;
    uint32 fec_cl74 = 0;
    uint32 fec_cl91 = 0;
    uint32 fec_cl108 = 0;
#ifdef INCLUDE_MACSEC
    uint32 macsec_switch_fixed, macsec_switch_fixed_speed;
    uint32 macsec_switch_fixed_duplex, macsec_switch_fixed_pause;
    uint32 macsec_pause_rx_fwd, macsec_pause_tx_fwd;
    uint32 macsec_line_ipg, macsec_switch_ipg;

    macsec_switch_fixed = 0;
    macsec_switch_fixed_speed = 0;
    macsec_switch_fixed_duplex = 0;
    macsec_switch_fixed_pause = 0;
    macsec_pause_rx_fwd = 0;
    macsec_pause_tx_fwd = 0;
    macsec_line_ipg = 0;
    macsec_switch_ipg = 0;
#endif

    if (bcm_port_config_get(u, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    wan_mode = 0;
    preemphasis = 0;
    predriver_current = 0;
    driver_current = 0;
    interface = 0;
    interfacemax = 0;
    eq_boost = 0;

    dfe = 0;
    lp_dfe = 0;
    br_dfe = 0;
    linkTraining = 0;

    if ((c = ARG_GET(a)) == NULL) {
        SOC_PBMP_ASSIGN(pbm, pcfg.port);
    } else if (parse_bcm_pbmp(u, c, &pbm) < 0) {
        cli_out("%s: ERROR: unrecognized port bitmap: %s\n", ARG_CMD(a), c);
        return CMD_FAIL;
    }

    BCM_PBMP_AND(pbm, pcfg.port);

    flags = 0;
    if ((c = ARG_CUR(a)) != NULL) {
        parse_table_t pt;
        int i;

        if (c[0] == '=') {
            return CMD_USAGE;       /* '=' unsupported */
        }
        if (sal_strcasecmp(c, "RxTune") == 0) {
            if (ARG_CNT(a) <= 1) { /*Current counter is at RxTune */
                ARG_NEXT(a);
                farEndEqValue = 0;
                cli_out("far end equalization value not input, using 0\n");
            } else {
                /* go to next argument */
                ARG_NEXT(a);
                /* Get far end equalization value  */
                if ((c = ARG_GET(a)) != NULL) {
                    farEndEqValue = sal_ctoi(c, 0);
                    cli_out("far end equalization value input (%d)\n",
                              farEndEqValue);
                }
            }
            eq_tune = TRUE;
        }
        if (sal_strcasecmp(c, "Dump") == 0) {
            c = ARG_GET(a);
            dump = TRUE;
        }

        if ((eq_tune == FALSE) && (dump == FALSE)) {
            parse_table_init(u, &pt);
            parse_table_add(&pt, "WanMode", PQ_DFL | PQ_BOOL,
                            0, &wan_mode, 0);
            parse_table_add(&pt, "Preemphasis", PQ_DFL | PQ_INT,
                            0, &preemphasis, 0);
            parse_table_add(&pt, "DriverCurrent", PQ_DFL | PQ_INT,
                            0, &driver_current, 0);
            parse_table_add(&pt, "PreDriverCurrent", PQ_DFL | PQ_INT,
                            0, &predriver_current, 0);
            parse_table_add(&pt, "EqualizerBoost", PQ_DFL | PQ_INT,
                            0, &eq_boost, 0);
            parse_table_add(&pt, "Interface", PQ_DFL | PQ_INT,
                            0, &interface, 0);
            parse_table_add(&pt, "InterfaceMax", PQ_DFL | PQ_INT,
                            0, &interfacemax, 0);
            parse_table_add(&pt, "SwRxLos", PQ_DFL | PQ_INT,
                            0, &sw_rx_los_nval, 0);
            parse_table_add(&pt, "Dfe", PQ_DFL | PQ_INT,
                            0, &dfe, 0);
            parse_table_add(&pt, "LpDfe", PQ_DFL | PQ_INT,
                            0, &lp_dfe, 0);
            parse_table_add(&pt, "BrDfe", PQ_DFL | PQ_INT,
                            0, &br_dfe, 0);
            parse_table_add(&pt, "LT", PQ_DFL | PQ_INT,
                            0, &linkTraining, 0);
            parse_table_add(&pt, "Cl74", PQ_DFL | PQ_BOOL,
                            0, &fec_cl74, 0);
            parse_table_add(&pt, "Cl91", PQ_DFL | PQ_BOOL,
                            0, &fec_cl91, 0);
            parse_table_add(&pt, "Cl108", PQ_DFL | PQ_BOOL,
                            0, &fec_cl108, 0);

#ifdef INCLUDE_MACSEC
            parse_table_add(&pt, "MacsecSwitchFixed", PQ_DFL | PQ_BOOL,
                            0, &macsec_switch_fixed, 0);
            parse_table_add(&pt, "MacsecSwitchFixedSpeed", PQ_DFL | PQ_INT,
                            0, &macsec_switch_fixed_speed, 0);
            parse_table_add(&pt, "MacsecSwitchFixedDuplex",
                            PQ_DFL | PQ_BOOL, 0,
                            &macsec_switch_fixed_duplex, 0);
            parse_table_add(&pt, "MacsecSwitchFixedPause", PQ_DFL | PQ_BOOL,
                            0, &macsec_switch_fixed_pause, 0);
            parse_table_add(&pt, "MacsecPauseRXForward", PQ_DFL | PQ_BOOL,
                            0, &macsec_pause_rx_fwd, 0);
            parse_table_add(&pt, "MacsecPauseTXForward", PQ_DFL | PQ_BOOL,
                            0, &macsec_pause_tx_fwd, 0);
            parse_table_add(&pt, "MacsecLineIPG", PQ_DFL | PQ_INT, 0,
                            &macsec_line_ipg, 0);
            parse_table_add(&pt, "MacsecSwitchIPG", PQ_DFL | PQ_INT, 0,
                            &macsec_switch_ipg, 0);
#endif

            if (parse_arg_eq(a, &pt) < 0) {
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }
            if (ARG_CNT(a) > 0) {
                cli_out("%s: Unknown argument %s\n", ARG_CMD(a), ARG_CUR(a));
                parse_arg_eq_done(&pt);
                return CMD_USAGE;
            }

            for (i = 0; i < pt.pt_cnt; i++) {
                if (pt.pt_entries[i].pq_type & PQ_PARSED) {
                    flags |= (1 << i);
                }
            }
            parse_arg_eq_done(&pt);
        }
    }
    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(u, pbm, dport, p) {
        print_header = TRUE;

        if (eq_tune == TRUE) {

            cmd_rv =
                bcm_port_control_set(u, p,
                                     bcmPortControlSerdesDriverEqualizationFarEnd,
                                     farEndEqValue);

            cmd_rv = bcm_port_control_set(u, p,
                                          bcmPortControlSerdesDriverTune,
                                          1);
            if (cmd_rv != CMD_OK) {
                cli_out("unit %d port %d Tuning function not available\n",
                        u, p);
                continue;
            }
            cli_out("Rx Equalization Tuning start\n");
            sal_usleep(1000000);
            cmd_rv = bcm_port_control_get(u, p,
                                          bcmPortControlSerdesDriverEqualizationTuneStatusFarEnd,
                                          &eq_status);

            cli_out("unit %d port %d Tuning done, Status: %s\n",
                    u, p, ((cmd_rv == CMD_OK) && eq_status) ? "OK" : "FAIL");
            continue;
        }
        if (dump == TRUE) {
            /* coverity[returned_value] */
            cmd_rv =
                bcm_port_phy_control_set(u, p, BCM_PORT_PHY_CONTROL_DUMP,
                                         1);
            continue;
        }

        cmd_rv = port_phy_control_update(u, p, BCM_PORT_PHY_CONTROL_WAN,
                                         wan_mode, flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set WAN mode */
        cmd_rv = port_phy_control_update(u, p, BCM_PORT_PHY_CONTROL_WAN,
                                         wan_mode, flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set Preemphasis */
        cmd_rv = port_phy_control_update(u, p,
                                         BCM_PORT_PHY_CONTROL_PREEMPHASIS,
                                         preemphasis, flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set Driver Current */
        cmd_rv = port_phy_control_update(u, p,
                                         BCM_PORT_PHY_CONTROL_DRIVER_CURRENT,
                                         driver_current, flags,
                                         &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set Pre-driver Current */
        cmd_rv = port_phy_control_update(u, p,
                                         BCM_PORT_PHY_CONTROL_PRE_DRIVER_CURRENT,
                                         predriver_current, flags,
                                         &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set DFE */
        cmd_rv = port_phy_control_update(u, p,
                                         BCM_PORT_PHY_CONTROL_FIRMWARE_DFE_ENABLE,
                                         dfe, flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set LP_DFE */
        cmd_rv = port_phy_control_update(u, p,
                                         BCM_PORT_PHY_CONTROL_FIRMWARE_LP_DFE_ENABLE,
                                         lp_dfe, flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set  BR_DFE */
        cmd_rv = port_phy_control_update(u, p,
                                         BCM_PORT_PHY_CONTROL_FIRMWARE_BR_DFE_ENABLE,
                                         br_dfe, flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set forced cl72/93 */
        cmd_rv = port_phy_control_update(u, p,
                                         BCM_PORT_PHY_CONTROL_CL72,
                                         linkTraining, flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set Equalizer Boost */
        cmd_rv = port_phy_control_update(u, p,
                                         BCM_PORT_PHY_CONTROL_EQUALIZER_BOOST,
                                         eq_boost, flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set the interface */
        cmd_rv = port_phy_control_update(u, p,
                                         BCM_PORT_PHY_CONTROL_INTERFACE,
                                         interface, flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }
        /* Read and set(is noop) the interface */
        cmd_rv = port_phy_control_update(u, p,
                                         SOC_PHY_CONTROL_INTERFACE_MAX,
                                         interfacemax, flags,
                                         &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set(is noop) the interface */
        bcm_rv = bcm_port_phy_control_get(u, p,
                                          BCM_PORT_PHY_CONTROL_SOFTWARE_RX_LOS,
                                          &sw_rx_los_oval);
        if (BCM_FAILURE(bcm_rv) && BCM_E_UNAVAIL != bcm_rv) {
            cli_out("%s\n", bcm_errmsg(bcm_rv));
            return CMD_FAIL;
        } else if (BCM_SUCCESS(bcm_rv)) {
            if ((sw_rx_los_nval != sw_rx_los_oval) && (flags & (1 << 7))) {
                bcm_rv = bcm_port_phy_control_set(u, p,
                                                  BCM_PORT_PHY_CONTROL_SOFTWARE_RX_LOS,
                                                  sw_rx_los_nval);
                if (BCM_FAILURE(bcm_rv)) {
                    cli_out("%s\n", bcm_errmsg(bcm_rv));
                    return CMD_FAIL;
                }
                sw_rx_los_oval = sw_rx_los_nval;
            }
            cmd_rv = CMD_OK;
            cli_out("Rx LOS (s/w) enable         - %d\n", sw_rx_los_oval);
        }

        /* Read and set(is noop) the cl74 */
        cmd_rv = port_phy_control_update(u, p,
                                         BCM_PORT_PHY_CONTROL_FORWARD_ERROR_CORRECTION,
                                         fec_cl74, flags,
                                         &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set(is noop) the cl91 */
        cmd_rv = port_phy_control_update(u, p,
                                         BCM_PORT_PHY_CONTROL_FORWARD_ERROR_CORRECTION_CL91,
                                         fec_cl91, flags,
                                         &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set(is noop) the cl108 */
        cmd_rv = port_phy_control_update(u, p,
                                         BCM_PORT_PHY_CONTROL_FORWARD_ERROR_CORRECTION_CL108,
                                         fec_cl108, flags,
                                         &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

#ifdef INCLUDE_MACSEC

        /* Read and set the Switch fixed */
        cmd_rv = port_phy_control_update(u, p,
                                         BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED,
                                         macsec_switch_fixed,
                                         flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set the Switch fixed Speed */
        cmd_rv = port_phy_control_update(u, p,
                                         BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED_SPEED,
                                         macsec_switch_fixed_speed,
                                         flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set the Switch fixed Duplex */
        cmd_rv = port_phy_control_update(u, p,
                                         BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED_DUPLEX,
                                         macsec_switch_fixed_duplex,
                                         flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set the Switch fixed Pause */
        cmd_rv = port_phy_control_update(u, p,
                                         BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_FIXED_PAUSE,
                                         macsec_switch_fixed_pause,
                                         flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set Pause Receive Forward */
        cmd_rv = port_phy_control_update(u, p,
                                         BCM_PORT_PHY_CONTROL_MACSEC_PAUSE_RX_FORWARD,
                                         macsec_pause_rx_fwd,
                                         flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set Pause transmit Forward */
        cmd_rv = port_phy_control_update(u, p,
                                         BCM_PORT_PHY_CONTROL_MACSEC_PAUSE_TX_FORWARD,
                                         macsec_pause_tx_fwd,
                                         flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set Line Side IPG */
        cmd_rv = port_phy_control_update(u, p,
                                         BCM_PORT_PHY_CONTROL_MACSEC_LINE_IPG,
                                         macsec_line_ipg,
                                         flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }

        /* Read and set Switch Side IPG */
        cmd_rv = port_phy_control_update(u, p,
                                         BCM_PORT_PHY_CONTROL_MACSEC_SWITCH_IPG,
                                         macsec_switch_ipg,
                                         flags, &print_header);
        if (cmd_rv != CMD_OK) {
            return cmd_rv;
        }
#endif
    }
    return CMD_OK;
}

STATIC cmd_result_t
_if_esw_phy_dumpall(int u, args_t *a)
{
    char *c;
    uint16 phy_data, phy_devad = 0;
    uint16 phy_addr;
    uint32 phy_reg;
    uint16 phy_addr_start = 0;
    uint16 phy_addr_end = 0xFF;
    int is_c45 = 0;
    int rv = 0;

    if ((c = ARG_GET(a)) == NULL) {
        cli_out("%s: Error: expecting \"c45\" or \"c22\"\n", ARG_CMD(a));
        return CMD_USAGE;
    }
    is_c45 = 0;
    if (sal_strcasecmp(c, "c45") == 0) {
        is_c45 = 1;
        if (!soc_feature(u, soc_feature_phy_cl45)) {
            cli_out("%s: Error: Device does not support clause 45\n",
                    ARG_CMD(a));
            return CMD_USAGE;
        }
    } else if (sal_strcasecmp(c, "c22") != 0) {
        cli_out("%s: Error: expecting \"c45\" or \"c22\"\n", ARG_CMD(a));
        return CMD_USAGE;
    }
    if ((c = ARG_GET(a)) != NULL) {
        char *end;

        phy_addr_start = sal_strtoul(c, &end, 0);
        if (*end) {
            cli_out("%s: Error: Expecting PHY start address [%s]\n",
                    ARG_CMD(a), c);
            return CMD_USAGE;
        }
        if ((c = ARG_GET(a)) != NULL) {
            phy_addr_end = sal_strtoul(c, &end, 0);
            if (*end) {
                cli_out("%s: Error: Expecting PHY end address [%s]\n",
                        ARG_CMD(a), c);
                return CMD_USAGE;
            }
        } else {
            /* If start specified but no end, just print one phy address */
            phy_addr_end = phy_addr_start;
        }
    } else {
        /* phy addr start and end are not specified */
        if (SOC_IS_TRIDENT3(u) || SOC_IS_TOMAHAWK(u)) {
            phy_addr_end = 0x1FF;
        } else if (SOC_IS_TOMAHAWK3(u)) {
            phy_addr_end = 0x2FF;
        }
    }

    if (is_c45) {
        cli_out("%4s%5s %5s %3s: %-6s\n", "", "PRTAD", "DEVAD", "REG",
                "VALUE");
        for (phy_addr = phy_addr_start; phy_addr <= phy_addr_end;
             phy_addr++) {
            /* Clause 45 supports 32 devices per phy. */
            for (phy_devad = 0; phy_devad <= 0x1f; phy_devad++) {
                /* Device ID is in registers 2 and 3 */
                for (phy_reg = 2; phy_reg <= 3; phy_reg++) {
                    rv = soc_miimc45_read(u, phy_addr, phy_devad,
                                          phy_reg, &phy_data);
                    if (rv < 0) {
                        cli_out("ERROR: MII Addr %d: "
                                "soc_miim_read failed: %s\n",
                                phy_addr, soc_errmsg(rv));
                        return CMD_FAIL;
                    }
                    /* Assume device doesn't exist in phy if
                       read back is all 0/1 */
                    if ((phy_data != 0xFFFF) && (phy_data != 0x0000) &&
                        (phy_data != 0x7FFF)&& (phy_data != 0x3FFF)) {
                        cli_out("%4s 0x%02X 0x%02X 0x%02X: 0x%04X\n",
                                "", phy_addr, phy_devad, phy_reg, phy_data);
                    }
                }
            }
        }
    } else {
        cli_out("%4s%5s %3s: %-6s\n", "", "PRTAD", "REG", "VALUE");
        for (phy_addr = phy_addr_start; phy_addr <= phy_addr_end;
             phy_addr++) {
            /* Device ID is in registers 2 and 3 */
            for (phy_reg = 2; phy_reg <= 3; phy_reg++) {
                rv = soc_miim_read(u, phy_addr, phy_reg, &phy_data);
                if (rv < 0) {
                    cli_out("ERROR: MII Addr %d: soc_miim_read failed: %s\n",
                            phy_addr, soc_errmsg(rv));
                    return CMD_FAIL;
                }
                if ((phy_data != 0xFFFF) && (phy_data != 0x0000)) {
                    cli_out("%4s0x%02X 0x%02x: 0x%04x\n",
                            "", phy_addr, phy_reg, phy_data);
                }
            }
        }
    }
    return CMD_OK;
}

STATIC cmd_result_t
_if_esw_phy_raw(int u, args_t *a)
{
    char *c, *bus_name;
    int is_c45 = 0;
    int is_sbus = 0;
    int is_sim = 0;
    int is_mpp1 = 0;
    uint16 phy_data, phy_devad = 0;
    uint16 phy_addr, phy_lane = 0, phy_pindex = 0;
    uint32 phy_reg, phy_wrmask;
    uint32 phy_data32, phy_aer = 0;
    int rv = 0;

    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    bus_name = "miim";
    if (sal_strcasecmp(c, "sbus") == 0) {
        is_sbus = 1;
        bus_name = "sbus_mdio";
        if ((c = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
    } else if (sal_strcasecmp(c, "sim") == 0) {
        is_sim = 1;
        bus_name = "physim";
        if ((c = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }
    } else if (soc_feature(u, soc_feature_phy_cl45)) {
        if (sal_strcasecmp(c, "c45") == 0) {
            bus_name = "miimc45";
            is_c45 = 1;
            if ((c = ARG_GET(a)) == NULL) {
                return CMD_USAGE;
            }
        }
    }
    phy_addr = sal_strtoul(c, NULL, 0);
    if ((c = ARG_GET(a)) == NULL) { /* Get register number */
        return CMD_USAGE;
    }
    if (is_sbus || is_sim) {
        phy_devad = sal_strtoul(c, NULL, 0);
        if (phy_devad > 0x1f) {
            cli_out("ERROR: Invalid devad 0x%x, max=0x%x\n",
                    phy_devad, 0x1f);
            return CMD_FAIL;
        }
        /*
         * If DEVAD is specified as X.Y then X is the DEVAD and Y
         * is the lane number. For example DEVAD=1 and lane=2 is
         * specified as 1.2 and encoded in phy_aer as 0x0802.
         */
        if ((c = sal_strchr(c, '.')) != NULL) {
            c++;
            phy_lane = sal_strtoul(c, NULL, 0);
            /*next check lane number is greater than 7 for TSCBH */
            if (phy_lane > 7) {
                cli_out("ERROR: Invalid phy_lane 0x%x, max=0x%x\n",
                        phy_lane, 0x7);
                return CMD_FAIL;
            } else {
                if (phy_lane > 3) {
                    is_mpp1 = 1;
                }
            }
            if((c = sal_strchr(c, '.')) != NULL) {
                c++;
                phy_pindex = sal_strtoul(c, NULL, 0);
            }
        }

        /* need to check if TSCBH 8 lane, for pcs register lane 0-lane3 using MPP0
        and lane 4-7 using MPP1, the bit location for MPP0 is 24 and MPP1 is 25 */
        if (phy_devad == 0) {
            if (is_mpp1) {
                phy_aer = 1 << 9;
            } else {
                phy_aer = 1 << 8;
            }
            phy_aer |= (phy_devad << 11) | (phy_lane % 4);
        } else {
            phy_aer = (phy_devad << 11) | phy_lane | (phy_pindex << 8);
        }
        if ((c = ARG_GET(a)) == NULL) {     /* Get register number */
            return CMD_USAGE;
        }
    } else  if (is_c45) {
        phy_devad = sal_strtoul(c, NULL, 0);
        if (phy_devad > 0x1f) {
            cli_out("ERROR: Invalid devad 0x%x, max=0x%x\n",
                    phy_devad, 0x1f);
            return CMD_FAIL;
        }
        if ((c = ARG_GET(a)) == NULL) {     /* Get register number */
            return CMD_USAGE;
        }
    }
    phy_reg = sal_strtoul(c, NULL, 0);
    if ((c = ARG_GET(a)) == NULL) { /* Read register */
        if (is_sbus) {
            phy_reg = (phy_aer << 16) | phy_reg;
            rv = soc_sbus_mdio_read(u, phy_addr, phy_reg, &phy_data32);
            phy_data = (uint16)phy_data32;
        } else if (is_sim) {
            phy_reg = (phy_aer << 16) | phy_reg;
            rv = soc_physim_read(u, phy_addr, phy_reg, &phy_data);
        } else if (is_c45) {
            rv = soc_miimc45_read(u, phy_addr, phy_devad,
                                  phy_reg, &phy_data);
        } else {
            rv = soc_miim_read(u, phy_addr, phy_reg, &phy_data);
        }
        if (rv < 0) {
            cli_out("ERROR: MII Addr %d: soc_%s_read failed: %s\n",
                    phy_addr, bus_name, soc_errmsg(rv));
            return CMD_FAIL;
        }
        var_set_hex("phy_reg_data", phy_data, TRUE, FALSE);
        cli_out("%s\t0x%02x: 0x%04x\n", "", phy_reg, phy_data);
    } else {                /* write */
        phy_data = sal_strtoul(c, NULL, 0);
        if (is_sbus || is_sim) {
            /*
             * If register data is specified as X/Y then X is the
             * data value and Y is the data mask. For example
             * data=0x50 and mask=0xf0 is specified as 0x50/0xf0
             * and encoded in phy_data32 as 0x0f000500.  Note that
             * if the mask is not specified (or zero) then it is
             * implicitly assumed to be 0xffff (all bits valid).
             */
            phy_data32 = phy_data;
            phy_wrmask = 0;
            if ((c = sal_strchr(c, '/')) != NULL) {
                c++;
                phy_wrmask = sal_strtoul(c, NULL, 0);
                phy_data32 |= (phy_wrmask << 16);
            }
            phy_reg = (phy_aer << 16) | phy_reg;
            if (is_sbus) {
                rv = soc_sbus_mdio_write(u, phy_addr, phy_reg, phy_data32);
            } else { /* is_sim */
                rv = soc_physim_wrmask(u, phy_addr,
                                       phy_reg, phy_data, phy_wrmask);
            }
        } else if (is_c45) {
            rv = soc_miimc45_write(u, phy_addr, phy_devad, phy_reg,
                                   phy_data);
        } else {
            rv = soc_miim_write(u, phy_addr, phy_reg, phy_data);
        }
        if (rv < 0) {
            cli_out("ERROR: MII Addr %d: soc_%s_write failed: %s\n",
                    phy_addr, bus_name, soc_errmsg(rv));
            return CMD_FAIL;
        }
    }
    return CMD_OK;
}

#if defined(BCM_ESW_SUPPORT) && defined(INCLUDE_PHY_8806X)
STATIC cmd_result_t
_phy_mt2(int unit, args_t *args)
{
    mt2_sym_t *phy8806xsymbs = NULL;
    parse_table_t pt;
    bcm_pbmp_t pbm;
    soc_port_t p, dport;
    phy_ctrl_t *pc;
    uint16 phy_addr;
    char *port_str, *cmd_str, *regname, *regval, *fldname, *fldval;
 #if defined(INCLUDE_FCMAP)
    char *fc_oper;
    uint8 cmd = 0, show = 0;
#endif
    char temp_char[2];
    uint32 testno;
    uint32 val1 = 0;
    uint32 mt2_addr;
    uint32 mt2_data[4];
    uint32 mt2_field;
    uint32 mt2_field_data[4];
    uint32 flags = 0;
    int i, j, tem, sbit, ebit, cnt, blkno = -1, min_idx = -1, max_idx = -1;
    parse_pm_t ctr_options[] = {
        {"All",         MT2_CTR_ALL},
        {"SYs",         MT2_CTR_SYS},
        {"LiNe",        MT2_CTR_LINE},
        {"CHanged",     MT2_CTR_CHANGED},
        {"Same",        MT2_CTR_SAME},
        {"Zero",        MT2_CTR_ZERO},
        {"NonZero",     MT2_CTR_NONZERO},
        {"heX",         MT2_CTR_HEX},
        {NULL,          0}
    };

    BCM_PBMP_CLEAR(pbm);
    port_str = NULL;

    if ((cmd_str = ARG_GET(args)) == NULL) {
        cli_out ("Specify Operation.. getreg, setreg, getmem, setmem, gettotsc, settotsc, counter, dump or testrun\n");
        return CMD_FAIL;
    }

    if ((sal_strcasecmp(cmd_str, "counter") == 0) || (sal_strcasecmp(cmd_str, "ctr") == 0)) {
        if ((cmd_str = ARG_GET(args)) == NULL) {
            cli_out ("Specify subcommand.. start, stop, show , interval\n");
            return CMD_FAIL;
        }
        if (sal_strcasecmp(cmd_str, "start") == 0) {
            phy8806x_ctr_start(unit);
            return CMD_OK;
        } else if (sal_strcasecmp(cmd_str, "stop") == 0) {
            phy8806x_ctr_stop(unit);
            return CMD_OK;
        } else if ((sal_strcasecmp(cmd_str, "show") == 0) || (sal_strcasecmp(cmd_str, "interval") == 0) ||
                   (sal_strcasecmp(cmd_str, "fcevt") == 0)) {
            if ((port_str = ARG_GET(args)) == NULL) {
                cli_out ("Specify port bitmap\n");
                return CMD_FAIL;
            }
            if (parse_bcm_pbmp(unit, port_str, &pbm) < 0) {
                cli_out("ERROR: unrecognized port bitmap: %s\n", port_str);
                return CMD_FAIL;
            }
        } else {
            cli_out ("Invalid subcommand.. start, stop, show, interval\n");
            return CMD_FAIL;
        }
        if (sal_strcasecmp(cmd_str, "interval") == 0) {
            if ((regval = ARG_GET(args)) == NULL) {
                cli_out ("Specify the ctr task interval\n");
                return CMD_FAIL;
            }
            val1 = sal_ctoi(regval, 0);
            /* coverity[overrun-local] */
            DPORT_SOC_PBMP_ITER(unit, pbm, dport, p) {
                pc = EXT_PHY_SW_STATE(unit, p);
                if ((pc == NULL) || (phy_is_8806x(pc) != SOC_E_NONE)) {
                    continue;
                }
                phy8806x_ctr_interval_set(pc, val1);
            }
 #if defined(INCLUDE_FCMAP)
        } else if (sal_strcasecmp(cmd_str, "fcevt") == 0) {
            if ((regval = ARG_GET(args)) == NULL) {
                cli_out ("Specify FC event 0 - disable; 1 - enable\n");
                return CMD_FAIL;
            }

            val1 = sal_ctoi(regval, 0);
            if (val1 == 0) {
                cmd = 0xc;
            } else if (val1 == 1) {
                cmd = 0xd;
            }

            DPORT_SOC_PBMP_ITER(unit, pbm, dport, p) {
                cli_out("FC Evt: u 0x%x, p 0x%x, cmd 0x%x\n", unit, p, cmd);
                bfcmap88060_xmod_debug_cmd(unit, p, cmd);
            }
#endif
        } else if (sal_strcasecmp(cmd_str, "show") == 0) {
            while ((cmd_str = ARG_GET(args)) != NULL) {
                if (parse_mask(cmd_str, ctr_options, &flags)) {
                    cli_out("Error: invalid option ignored: %s\n", cmd_str);
                }
            }

            DPORT_SOC_PBMP_ITER(unit, pbm, dport, p) {
                pc = EXT_PHY_SW_STATE(unit, p);
                if ((pc == NULL) || (phy_is_8806x(pc) != SOC_E_NONE)) {
                    continue;
                }
                phy8806x_ctr_show(pc, flags);
            }
        }
        return CMD_OK;
    }

    if (!(sal_strcasecmp(cmd_str, "listreg") == 0)) {
        if ((port_str = ARG_GET(args)) == NULL) {
            cli_out("%s: ERROR: missing port bitmap\n", ARG_CMD(args));
            return CMD_FAIL;
        }
        if (parse_bcm_pbmp(unit, port_str, &pbm) < 0) {
           cli_out("ERROR: unrecognized port bitmap: %s\n", port_str);
           return CMD_FAIL;
        }
    }

    if (sal_strcasecmp(cmd_str, "listreg") == 0) {
        if ((regname = ARG_GET(args)) == NULL) {
            cli_out ("Specify Partial register Name\n");
            return CMD_FAIL;
        }
        i = 0;
        while (i >= 0) {
            i = mt2_syms_find_next_name(regname, &phy8806xsymbs, i);
            if (i >= 0)
            {
                cli_out("%s\n", phy8806xsymbs->name);
            }
        }
        return CMD_OK;
    } else if (sal_strcasecmp(cmd_str, "dump") == 0) {
        if ((regname = ARG_GET(args)) == NULL) {
            cli_out ("Specify AXI Address\n");
            return CMD_FAIL;
        }
        mt2_addr = sal_ctoi(regname, 0);

        if ((regval = ARG_GET(args)) == NULL) {
            val1 = 1;
        } else {
            val1 = sal_ctoi(regval, 0);
        }

        DPORT_SOC_PBMP_ITER(unit, pbm, dport, p) {
            phy_addr = port_to_phyaddr(unit,p);
            for (i = 0; i < val1; i++) {
                mt2_axi_read(unit, phy_addr, (mt2_addr + (i * 4)), mt2_data);
                /* To Do: Format Dump printout */
                cli_out("Reading at 0x%08x at phy addr 0x%02x = 0x%08x\n", (mt2_addr + (i * 4)), phy_addr, mt2_data[0]);
            }
        }
    } else if ((sal_strcasecmp(cmd_str, "getreg") == 0) || (sal_strcasecmp(cmd_str, "setreg") == 0) || (sal_strcasecmp(cmd_str, "modreg") == 0)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "BLocK", PQ_DFL | PQ_INT, (void *)(-1), &blkno, 0);
        if (parse_arg_eq(args, &pt) < 0) {
            /* No Arguments. Don't have to do anything */
        }
        parse_arg_eq_done(&pt);

        if ((regname = ARG_GET(args)) == NULL) {
            /* Display All Registers */
            regname = &temp_char[0]; /* use a local char array to avoid memory leak */
            sal_strcpy(regname, "r");
        }
        if (isint(regname)) {
            cli_out("Reg operation by Address Not Implemented");
            /* Display All Registers */
            regname = &temp_char[0];
            sal_strcpy(regname, "r");
        }

        if ((phy8806xsymbs = mt2_syms_find_name(regname)) != NULL)
        {
            if (sal_strcasecmp(cmd_str, "setreg") == 0) {   /* Set Reg */
                if ((regval = ARG_GET(args)) == NULL) {
                    cli_out ("Specify Value to Write\n");
                    return CMD_FAIL;
                }
                if (sal_strcmp(regval, "POR") == 0) {
                    mt2_data[0] = phy8806xsymbs->rstval;
                    mt2_data[1] = phy8806xsymbs->rstval_hi;
                } else {
                    if (phy8806xsymbs->flags & MT2_SYMBOL_FLAG_R64) {
                        mt2_data[1] = sal_ctoi(regval, 0);
                        if ((regval = ARG_GET(args)) == NULL) {
                            cli_out ("Value Size Mismatch(64 bits) - Provide 2 words\n");
                            return CMD_FAIL;
                        }
                    } else {
                        mt2_data[1] = 0;
                    }
                    mt2_data[0] = sal_ctoi(regval, 0);
               }

                while (((fldname = ARG_GET(args)) != NULL) && ((fldval = ARG_GET(args)) != NULL)) {
                    /* Search Fields */
                    i = 0;
                    while (1) {
                        mt2_field = phy8806xsymbs->fields[i];
                        j = (mt2_field >> 16) & 0x7fff;
                        if (sal_strcasecmp(fldname, phy8806x_fields[j]) == 0) {
                            sbit = (mt2_field & 0xff);
                            ebit = ((mt2_field >> 8) & 0xff);
                            if ((ebit - sbit) <= 31) {
                                j = sal_ctoi(fldval, 0);
                                mt2_field32_set(mt2_data, sbit, ebit, j);
                                cli_out ("Setting Field %s at %d - %d to %d\n", fldname, sbit, ebit, j);
                            }
                            break;
                        }
                        if (mt2_field & CDK_SYMBOL_FIELD_FLAG_LAST) {
                            break;
                        }
                        i++;
                    }
                }

                DPORT_SOC_PBMP_ITER(unit, pbm, dport, p) {
                     phy_addr = port_to_phyaddr(unit,p);
                     mt2_sbus_reg_write(unit, phy_addr, blkno, phy8806xsymbs, mt2_data);
                     if (phy8806xsymbs->flags & MT2_SYMBOL_FLAG_R64) {
                         cli_out("Writing 0x%08x_%08x to reg %s at phy addr 0x%02x\n", mt2_data[1], mt2_data[0], regname, phy_addr);
                     } else {
                         cli_out("Writing 0x%08x to reg %s at phy addr 0x%02x\n", mt2_data[0], regname, phy_addr);
                     }
                }
            } else if (sal_strcasecmp(cmd_str, "modreg") == 0) {   /* Modify Reg */
                DPORT_SOC_PBMP_ITER(unit, pbm, dport, p) {
                    phy_addr = port_to_phyaddr(unit,p);
                    cli_out("Modify Register %s at phy addr 0x%02x\n", regname, phy_addr);
                    mt2_sbus_reg_read(unit, phy_addr, blkno, phy8806xsymbs, mt2_data);
                    tem = 0;
                    while (((fldname = ARG_GET(args)) != NULL) && ((fldval = ARG_GET(args)) != NULL)) {
                        /* Search Fields */
                        i = 0;
                        while (1) {
                            mt2_field = phy8806xsymbs->fields[i];
                            j = (mt2_field >> 16) & 0x7fff;
                            if (sal_strcasecmp(fldname, phy8806x_fields[j]) == 0) {
                                sbit = (mt2_field & 0xff);
                                ebit = ((mt2_field >> 8) & 0xff);
                                if ((ebit - sbit) > 31) {
                                    cli_out("Skipping 64-bit field %s\n", fldname);
                                } else {
                                    j = sal_ctoi(fldval, 0);
                                    mt2_field32_set(mt2_data, sbit, ebit, j);
                                    tem = 1;
                                    cli_out ("Setting Field %s at %d - %d to %d\n", fldname, sbit, ebit, j);
                                }
                                break;
                            }

                            if (mt2_field & CDK_SYMBOL_FIELD_FLAG_LAST) {
                                cli_out ("Not Found Field %s\n", fldname);
                                break;
                            }
                            i++;
                        }
                    }
                    if (tem) { /* At least one field had changed.. */
                        mt2_sbus_reg_write(unit, phy_addr, blkno, phy8806xsymbs, mt2_data);
                    }
                }
            } else {                                        /* Get Reg */
                DPORT_SOC_PBMP_ITER(unit, pbm, dport, p) {
                    phy_addr = port_to_phyaddr(unit,p);
                    mt2_sbus_reg_read(unit, phy_addr, blkno, phy8806xsymbs, mt2_data);
                    if (phy8806xsymbs->flags & MT2_SYMBOL_FLAG_R64) {
                        cli_out("%s.%s[0x%08x]= 0x%08x_%08x: <", phy8806xsymbs->name, port_str, phy8806xsymbs->addr, mt2_data[1], mt2_data[0]);
                    } else {
                        cli_out("%s.%s[0x%08x]= 0x%08x: <", phy8806xsymbs->name, port_str, phy8806xsymbs->addr, mt2_data[0]);
                    }

                    /* Print Fields */
                    i = 0;
                    while (1) {
                        if (!phy8806xsymbs->fields) {
                            cli_out(" field info unavailable >\n");
                            break;
                        }
                        mt2_field = phy8806xsymbs->fields[i];
                        j = (mt2_field >> 16) & 0x7fff;
                        sbit = (mt2_field & 0xff);
                        ebit = ((mt2_field >> 8) & 0xff);
                        mt2_field_get(mt2_data, sbit, ebit, mt2_field_data);
                        if ((ebit - sbit) > 31) {
                            cli_out("%s=0x%08x_%08x", phy8806x_fields[j], mt2_field_data[1], mt2_field_data[0]);
                        } else {
                            cli_out("%s=%d", phy8806x_fields[j], mt2_field_data[0]);
                        }
                        if (mt2_field & CDK_SYMBOL_FIELD_FLAG_LAST) {
                            cli_out(">\n");
                            break;
                        } else {
                            cli_out(", ");
                        }
                        i++;
                    }
                }
            }
        } else {
            cli_out("Reg Not Found. Possible registers are:\n");
            i = 0;
            while (i >= 0) {
                i = mt2_syms_find_next_name(regname, &phy8806xsymbs, i);
                if (i >= 0)
                {
                    if (phy8806xsymbs->flags & MT2_SYMBOL_FLAG_REGISTER) {
                        cli_out("%s\n", phy8806xsymbs->name);
                    }
                }
            }
        }
    } else if ((sal_strcasecmp(cmd_str, "getmem") == 0) || (sal_strcasecmp(cmd_str, "setmem") == 0)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "MINIdx", PQ_DFL | PQ_INT, (void *)(-1), &min_idx, 0);
        parse_table_add(&pt, "MAXIdx", PQ_DFL | PQ_INT, (void *)(-1), &max_idx, 0);
        parse_table_add(&pt, "BLocK", PQ_DFL | PQ_INT, (void *)(-1), &blkno, 0);

        if (parse_arg_eq(args, &pt) < 0) {
            /* No Arguments. Don't have to do anything */
        }
        parse_arg_eq_done(&pt);

        if ((regname = ARG_GET(args)) == NULL) {
            /* Display All Memories */
            regname = &temp_char[0]; /* use local char array to avoid memory leak */
            sal_strcpy(regname, "m");
        }

        if (isint(regname)) {
            cli_out("Mem operation by Address Not Implemented\n");
            /* Display All Memories */
            regname = &temp_char[0];
            sal_strcpy(regname, "m");
        }

        if ((phy8806xsymbs = mt2_syms_find_name(regname)) != NULL)
        {
            cnt = (phy8806xsymbs->index & 0xff) / 4;    /* Number of 32-bit words */
            if (min_idx == -1) {
                min_idx = MT2_SYMBOL_INDEX_MIN_DECODE(phy8806xsymbs->index);
            }
            if (max_idx == -1) {
                max_idx = MT2_SYMBOL_INDEX_MAX_DECODE(phy8806xsymbs->index);
            }

            if (sal_strcasecmp(cmd_str, "setmem") == 0) {   /* Set Mem */
                for (i = 1; i <= cnt; i++) {
                    if ((regval = ARG_GET(args)) == NULL) {
                        cli_out ("Value Size Mismatch(%d bits) - Provide %d words\n", (cnt*32), cnt);
                        return CMD_FAIL;
                    }
                    mt2_data[cnt-i] = sal_ctoi(regval, 0);
                }
                DPORT_SOC_PBMP_ITER(unit, pbm, dport, p) {
                    phy_addr = port_to_phyaddr(unit,p);
                    for (i = min_idx; i <= max_idx; i++) {
                        mt2_sbus_mem_write(unit, phy_addr, blkno, phy8806xsymbs, i, mt2_data);
                        cli_out("Writing %d words to mem %s[%d] at phy addr 0x%02x\n", cnt, regname, i, phy_addr);
                    }
                }
            } else {                                        /* Get Mem */
                DPORT_SOC_PBMP_ITER(unit, pbm, dport, p) {
                    phy_addr = port_to_phyaddr(unit,p);
                    for (i = min_idx; i <= max_idx; i++) {
                        mt2_sbus_mem_read(unit, phy_addr, blkno, phy8806xsymbs, i, mt2_data);
                        cli_out("Reading mem %s[%d] at phy addr 0x%02x = 0x", regname, i, phy_addr);
                        for (j = 1; j <= cnt; j++) {
                            cli_out("%08x%s", mt2_data[cnt-j], (j == cnt) ? "\n" : "_");
                        }
                    }
                }
            }
        } else {
            cli_out("memory Not Found. Possible memories are:\n");
            i = 0;
            while (i >= 0) {
                i = mt2_syms_find_next_name(regname, &phy8806xsymbs, i);
                if (i >= 0)
                {
                    if (phy8806xsymbs->flags & MT2_SYMBOL_FLAG_MEMORY) {
                        cli_out("%s\n", phy8806xsymbs->name);
                    }
                }
            }
        }
        return CMD_OK;
    } else if ((sal_strcasecmp(cmd_str, "gettotsc") == 0) || (sal_strcasecmp(cmd_str, "settotsc") == 0)) {
        if ((regname = ARG_GET(args)) == NULL) {
            cli_out("Specify Address\n");
            return CMD_FAIL;
        }
        if (!isint(regname)) {
            cli_out("To-TSC Symbolic Name Not Implemented. Give Address\n");
            return CMD_FAIL;
        }
        mt2_addr = sal_ctoi(regname, 0);
        if (sal_strcasecmp(cmd_str, "settotsc") == 0) {   /* Set To-Tsc Reg */
            if ((regval = ARG_GET(args)) == NULL) {
                cli_out ("Specify Value to Write\n");
                return CMD_FAIL;
            }
            tem = 0;
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Mask", PQ_DFL | PQ_INT, 0, &tem, 0);
            if (parse_arg_eq(args, &pt) < 0) {
                /* No Arguments. Mask is 0 */
            }
            parse_arg_eq_done(&pt);
            mt2_data[0] = sal_ctoi(regval, 0) << 16;
            mt2_data[0] |= (tem & 0xffff);

            DPORT_SOC_PBMP_ITER(unit, pbm, dport, p) {
                 phy_addr = port_to_phyaddr(unit,p);
                 mt2_sbus_to_tsc_write(unit, phy_addr, mt2_addr, mt2_data);
                 cli_out("Writing 0x%08x to to_tsc reg 0x%08x at phy addr 0x%02x\n", mt2_data[0], mt2_addr, phy_addr);
            }
        } else {                                           /* Set To-Tsc Reg */
            DPORT_SOC_PBMP_ITER(unit, pbm, dport, p) {
                 phy_addr = port_to_phyaddr(unit,p);
                 mt2_sbus_to_tsc_read(unit, phy_addr, mt2_addr, mt2_data);
                 cli_out("Reading to_tsc reg 0x%08x at phy addr 0x%02x = 0x%08x\n", mt2_addr, phy_addr, mt2_data[0]);
            }
        }

        return CMD_OK;
    } else if ((sal_strcasecmp(cmd_str, "testrun") == 0) || (sal_strcasecmp(cmd_str, "tr") == 0)) {
        if ((regname = ARG_GET(args)) == NULL) {
            cli_out ("Specify Test Number\n");
            return CMD_FAIL;
        }
        i = 0;
        if (sal_strcasecmp(regname, "-s") == 0) {
            i = 1;   /* Silent / Summary Only */
            if ((regname = ARG_GET(args)) == NULL) {
                cli_out ("Specify Test Number\n");
                return CMD_FAIL;
            }
        }
        testno = sal_ctoi(regname, 0);

        /* PORT_LOCK(unit); */
        DPORT_SOC_PBMP_ITER(unit, pbm, dport, p) {
             phy_addr = port_to_phyaddr(unit,p);
             mt2_test_run(unit, phy_addr, testno, i);
        }
    } else if (sal_strcasecmp(cmd_str, "ether") == 0) {
        DPORT_SOC_PBMP_ITER(unit, pbm, dport, p) {
            if (phy8806x_xmod_debug_cmd(unit, p) == SOC_E_UNAVAIL)
                bsl_printf("Command not supported in FC mode \n");
        }
        /* PORT_UNLOCK(unit); */
#if defined(INCLUDE_FCMAP)
    } else if (sal_strcasecmp(cmd_str, "fc") == 0) {
        if ((fc_oper = ARG_GET(args)) == NULL) {
            cli_out ("Specify FC operation..\n");
            return CMD_FAIL;
        }

        if (sal_strcasecmp(fc_oper, "dmpstat") == 0) {
            cmd = 0x5;
        }

        if (sal_strcasecmp(fc_oper, "clrstat") == 0) {
            cmd = 0x6;
        }

        if (sal_strcasecmp(fc_oper, "dmpdbg") == 0) {
            cmd = 0x7;
        }

        if (sal_strcasecmp(fc_oper, "disable") == 0) {
            cmd = 0x8;
        }

        if (sal_strcasecmp(fc_oper, "syslnk") == 0) {
            cmd = 0x9;
        }
        if (sal_strcasecmp(fc_oper, "admn") == 0) {
            cmd = 0xa;
        }
        if (sal_strcasecmp(fc_oper, "clkvld") == 0) {
            cmd = 0xb;
        }
        if (sal_strcasecmp(fc_oper, "speed") == 0) {
            cmd = 0x4;
        }

        if (sal_strcasecmp(fc_oper, "showcfg") == 0) {
            show = 0x1;
        }

        DPORT_SOC_PBMP_ITER(unit, pbm, dport, p) {
            if (show == 0x1) {
                cli_out("FC CFG for port %d\n", p);
                bfcmap88060_show_fc_config(unit, p);
            } else {
                cli_out("FC OPER: u 0x%x, p 0x%x, oper %s cmd 0x%x\n", unit, p, fc_oper, cmd);
                bfcmap88060_xmod_debug_cmd(unit, p, cmd);
            }
        }
#endif
    } else if (sal_strcasecmp(cmd_str, "diag") == 0) {
        if ((cmd_str = ARG_GET(args)) == NULL) {
            cli_out ("Specify Diag Command\n");
            return CMD_FAIL;
        }
        regname = ARG_GET(args); /* recycle regname for Optional Parameter */
        DPORT_SOC_PBMP_ITER(unit, pbm, dport, p) {
            pc = EXT_PHY_SW_STATE(unit, p);
            if ((pc == NULL) || (phy_is_8806x(pc) != SOC_E_NONE)) {
                continue;
            }
            phy_addr = port_to_phyaddr(unit,p);
            mt2_diag_run(unit, phy_addr, cmd_str, regname);
        }
    } else if (sal_strcasecmp(cmd_str, "uart") == 0) {
        void *buffer, *buf;
        char *fn = NULL, file_name[81];
        unsigned char *cbuf;
        parse_table_t pt;
        int lscan_time;
        unsigned int buf_size = 4096; /* NULL termination taken care of internally */

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "File", PQ_STRING, 0, &fn, NULL);
        parse_table_add(&pt, "BufSize", PQ_DFL | PQ_INT, 0, &buf_size, 0);

        if (parse_arg_eq(args, &pt) < 0) {
            cli_out("Dumping to the screen: %s\n", ARG_CUR(args));
            file_name[0] = 0;
        } else {
            sal_strncpy(file_name, fn, 80);
        }
        parse_arg_eq_done(&pt);

        buffer = (void*) sal_alloc(buf_size, "mt2 uart buffer");

        if (buffer == NULL) {
            cli_out("Insufficient memory.\n");
            return CMD_FAIL;
        }
        /* rounding up to the next 4 byte boundary */
        buf = UINTPTR_TO_PTR(((PTR_TO_UINTPTR(buffer) + 0x3) & ~0x3));

        cbuf = (unsigned char *) buf;
        buf_size -= (PTR_TO_UINTPTR(buffer) & 0x3) + 4;
        cbuf[0] = buf_size & 0xff;
        cbuf[1] = (buf_size >> 8) & 0xff;
        cbuf[2] = (buf_size >> 16) & 0xff;
        cbuf[3] = (buf_size >> 24) & 0xff;
        buf = UINTPTR_TO_PTR(PTR_TO_UINTPTR(buf) + 0x4);

        BCM_IF_ERROR_RETURN(bcm_linkscan_enable_get(unit, &lscan_time));
        if (lscan_time != 0) {
            cli_out("Stopping Linkscan.\n");
            BCM_IF_ERROR_RETURN(bcm_linkscan_enable_set(unit, 0));
            /* Give enough time for linkscan task to exit. */
            sal_usleep(lscan_time * 2);
        }

        {
#ifndef  NO_FILEIO
            FILE *ofp = NULL;
            if ((ofp = sal_fopen(file_name, "w")) != NULL) {
                DPORT_SOC_PBMP_ITER(unit, pbm, dport, p) {
                        sal_fprintf(ofp, "-------------- Start Port %s --------------\n", SOC_PORT_NAME(unit, p));
                        do {
                            if (port_diag_ctrl(unit, p, 0,
                                   PHY_DIAG_CTRL_GET, PHY_DIAG_CTRL_STATE_GENERIC, buf) == BCM_E_NONE) {
                                sal_fprintf(ofp, "%s", (char*) buf);
                            } else {
                                sal_fprintf(ofp, "PHY_DIAG_CTRL_GET (PHY_DIAG_CTRL_STATE_GENERIC) failed for %s !.\n", SOC_PORT_NAME(unit, p));
                                cli_out("PHY_DIAG_CTRL_GET (PHY_DIAG_CTRL_STATE_GENERIC) failed for %s !.\n", SOC_PORT_NAME(unit, p));
                                break;
                            }
                            /* cli_out("available_size =%d sal_strlen = %d\n", buf_size, sal_strlen((char*) buf)); */
                        } while (sal_strlen((char*) buf) >= (buf_size - 8)); /* full buffer returned */
                        sal_fprintf(ofp, "\n-------------- End Port %s ----------------\n", SOC_PORT_NAME(unit, p));
                }
                sal_fclose(ofp);
            } else
#endif
            {
                DPORT_SOC_PBMP_ITER(unit, pbm, dport, p) {
                        cli_out("-------------- Start Port %s --------------\n", SOC_PORT_NAME(unit, p));
                        do {
                            if (port_diag_ctrl(unit, p, 0,
                                   PHY_DIAG_CTRL_GET, PHY_DIAG_CTRL_STATE_GENERIC, buf) == BCM_E_NONE) {
                                cli_out("%s", (char*) buf);
                            } else {
                                cli_out("PHY_DIAG_CTRL_GET (PHY_DIAG_CTRL_STATE_GENERIC) failed for %s !.\n", SOC_PORT_NAME(unit, p));
                                break;
                            }
                            /* cli_out("available_size =%d sal_strlen = %d\n", buf_size, sal_strlen((char*) buf)); */
                        } while (sal_strlen((char*) buf) >= (buf_size - 8)); /* full buffer returned */
                        cli_out("\n-------------- End Port %s ----------------\n", SOC_PORT_NAME(unit, p));
                }
            }
        }
        if (lscan_time != 0) {
            cli_out("Restarting Linkscan.\n");
            BCM_IF_ERROR_RETURN(bcm_linkscan_enable_set(unit, lscan_time));
        }
        sal_free(buffer);
    } else {
        cli_out ("Unknown Operation : %s\n", cmd_str);
        return CMD_FAIL;
    }

    return CMD_OK;
}
#endif /* defined(BCM_ESW_SUPPORT) && defined(INCLUDE_PHY_8806X) */

/*
 * Function:    if_phy
 * Purpose:     Show/configure phy registers.
 * Parameters:  u - SOC unit #
 *              a - pointer to args
 * Returns:     CMD_OK/CMD_FAIL/
 */
cmd_result_t if_esw_phy(int u, args_t *a)
{
    soc_pbmp_t pbm, pbm_phys, pbm_temp;
    soc_port_t p, dport;
    char *c, drv_name[64];
    uint16 phy_data, phy_devad = 0;
    uint16 phy_addr;
    uint32 phy_reg;
    uint32 phy_data32;
    int int_ovrride; /* Set if no external PHY is present */
    int intermediate = 0;
    int is_c45 = 0;
#if defined(BCM_RCPU_SUPPORT) && defined(BCM_CMICM_SUPPORT)
    int is_rcpu = 0;
#endif                          /* defined(BCM_RCPU_SUPPORT) && defined(BCM_CMICM_SUPPORT) */
    int rv = 0;
    char pfmt[SOC_PBMP_FMT_LEN];
    int i;
    int p_devad[] = { PHY_C45_DEV_PMA_PMD,
        PHY_C45_DEV_WIS,
        PHY_C45_DEV_PCS,
        PHY_C45_DEV_PHYXS,
        PHY_C45_DEV_DTEXS,
        PHY_C45_DEV_AN,
        PHY_C45_DEV_USER
    };
    char *p_devad_str[] = { "DEV_PMA_PMD",
        "DEV_WIS",
        "DEV_PCS",
        "DEV_PHYXS",
        "DEV_DTEXS",
        "DEV_AN",
        "DEV_USER"
    };
#ifdef PORTMOD_SUPPORT
    int nof_phys =0;
#endif

    if (!sh_check_attached(ARG_CMD(a), u)) {
        return CMD_FAIL;
    }

    c = ARG_GET(a);

    if (c != NULL && sal_strcasecmp(c, "info") == 0) {
        return _if_esw_phy_info(u, a);
    }

#if defined(PHYMOD_SUPPORT)
    if (c != NULL && sal_strcasecmp(c, "phymod") == 0) {
        return _if_esw_phy_phymod(u, a);
    }
#endif /* defined(PHYMOD_SUPPORT) */

    if (c != NULL && sal_strcasecmp(c, "eee") == 0) {
        return _if_esw_phy_eee(u, a);
    }

#ifdef   INCLUDE_TIMESYNC_DVT_TESTS    /* test IEEE1588 TimeSync */
    if (c != NULL && sal_strncasecmp(c, "tst", 3) == 0) {
        return phy_test1588(u, a, &pbm);
    }
#endif

    if ( c != NULL &&
         ((sal_strcasecmp(c, "TS")       == 0) ||
          (sal_strcasecmp(c, "TimeSync") == 0)) ) {
        return _if_esw_phy_timesync(u, a);
    }

#ifdef INCLUDE_PHY_SYM_DBG
    if (c != NULL && sal_strcasecmp(c, "SymDebug") == 0) {
        return _if_esw_phy_symdebug(u, a);
    }

    if (c != NULL && sal_strcasecmp(c, "SymDebugOff") == 0) {
        return _if_esw_phy_symdebug_off(u, a);
    }
#endif

    if (c != NULL && sal_strcasecmp(c, "firmware") == 0) {
        return _if_esw_phy_firmware(u, a);
    }

    if (c != NULL && sal_strcasecmp(c, "oam") == 0) {
        return _if_esw_phy_oam(u, a);
    }

    if (c != NULL && sal_strcasecmp(c, "power") == 0) {
        return _if_esw_phy_power(u, a);
    }

    if (c != NULL && sal_strcasecmp(c, "margin") == 0) {
        return _if_esw_phy_margin(u, a);
    }

    if (c != NULL && sal_strcasecmp(c, "prbs") == 0) {
        return _if_esw_phy_prbs(u, a);
    }

    if (c != NULL && sal_strcasecmp(c, "diag") == 0) {
        return _if_esw_phy_diag(u, a);
    }

    if (c != NULL && sal_strcasecmp(c, "longreach") == 0) {
        return _if_esw_phy_longreach(u, a);
    }

    if (c != NULL && sal_strcasecmp(c, "extlb") == 0) {
        return _if_esw_phy_extlb(u, a);
    }

    if (c != NULL && sal_strcasecmp(c, "clock") == 0) {
        return _if_esw_phy_clock(u, a);
    }

    if (c != NULL && sal_strcasecmp(c, "wr") == 0) {
        return _if_esw_phy_wr(u, a);
    }

    if (c != NULL && sal_strcasecmp(c, "rd_cp") == 0) {
        return _if_esw_phy_rd_cp(u, a);
    }

    if (c != NULL && sal_strcasecmp(c, "rd_cp2") == 0) {
        return _if_esw_phy_rd_cp2(u, a);
    }

    if (c != NULL && sal_strcasecmp(c, "mod") == 0) {
        return _if_esw_phy_mod(u, a);
    }

    if (c != NULL && sal_strcasecmp(c, "control") == 0) {
        return _if_esw_phy_control(u, a);
    }

    /* All access to an MII register */
    if (c != NULL && sal_strcasecmp(c, "dumpall") == 0) {
        return _if_esw_phy_dumpall(u, a);
    }

    /* Raw access to an MII register */
    if (c != NULL && sal_strcasecmp(c, "raw") == 0) {
        return _if_esw_phy_raw(u, a);
    }

#if defined(BCM_RCPU_SUPPORT) && defined(BCM_CMICM_SUPPORT)
    if (c != NULL && sal_strcasecmp(c, "rcpu") == 0) {
        is_rcpu = 1;
        c = ARG_GET(a);
    }
#endif                          /* defined(BCM_RCPU_SUPPORT) && defined(BCM_CMICM_SUPPORT) */

    if (c != NULL && sal_strcasecmp(c, "int") == 0) {
        intermediate = 1;
        c = ARG_GET(a);
    }

#if defined(BCM_ESW_SUPPORT) && defined(INCLUDE_PHY_8806X)
    if (c != NULL && sal_strcasecmp(c, "mt2") == 0) {
        return _phy_mt2(u, a);
    }
#endif /* defined(BCM_ESW_SUPPORT) && defined(INCLUDE_PHY_8806X) */


    if (c == NULL) {
        return (CMD_USAGE);
    }

    /* Parse the bitmap. */
    if (parse_pbmp(u, c, &pbm) < 0) {
        cli_out("%s: ERROR: unrecognized port bitmap: %s\n", ARG_CMD(a), c);
        return CMD_FAIL;
    }

    SOC_PBMP_ASSIGN(pbm_phys, pbm);
    SOC_PBMP_AND(pbm_phys, PBMP_PORT_ALL(u));
    if (SOC_PBMP_IS_NULL(pbm_phys)) {
        cli_out("Ports specified do not have PHY drivers.\n");
    } else {
        SOC_PBMP_ASSIGN(pbm_temp, pbm);
        SOC_PBMP_REMOVE(pbm_temp, PBMP_PORT_ALL(u));
        if (SOC_PBMP_NOT_NULL(pbm_temp)) {
            cli_out("Not all ports given have PHY drivers.  Using %s\n",
                    SOC_PBMP_FMT(pbm_phys, pfmt));
        }
    }
    SOC_PBMP_ASSIGN(pbm, pbm_phys);

    if (ARG_CNT(a) == 0) {      /*  show information for all registers */
#if defined(BCM_TOMAHAWK3_SUPPORT)
        if (SOC_IS_TOMAHAWK3(u)) {
            cli_out("Tomahawk 3 does not support MDIO. Please use phy <pbm> *\n\n\n");
            return (CMD_USAGE);
        }

#endif
        DPORT_SOC_PBMP_ITER(u, pbm, dport, p) {
            phy_addr = (intermediate ?
                        port_to_phyaddr_int(u, p) : port_to_phyaddr(u, p));
            int_ovrride = (EXT_PHY_SW_STATE(u, p) == NULL) ? 1 : 0;
            if (phy_addr == 0xff) {
                cli_out("Port %s: No %sPHY address assigned\n",
                        SOC_PORT_NAME(u, p),
                        intermediate ? "intermediate " : "");
                continue;
            }
#ifdef PORTMOD_SUPPORT
            if (SOC_PORT_USE_PORTCTRL(u, p)) {
                portmod_port_num_phys_get(u, p, &nof_phys);
                int_ovrride = 0;
                if (nof_phys == 1) {
                    is_c45 = 0;
                } else {
                    is_c45 = portmod_port_flags_test(u, p, PHY_FLAGS_C45);
                }
            } else
#endif
            {
                is_c45 = soc_phy_is_c45_miim(u, p);
            }

            if (intermediate) {
                cli_out("Port %s (intermediate PHY addr 0x%02x):",
                        SOC_PORT_NAME(u, p), phy_addr);
            } else {
                int ap = p;
                char lnstr[32];

#ifdef PORTMOD_SUPPORT
                if (SOC_PORT_USE_PORTCTRL(u, p)) {
                    phymod_core_access_t core_acc;
                    phymod_core_info_t   core_info;
                    int nof_cores = 0;
                    int phy = 0, core_num = 0, is_legacy =0;
                    int range_start = 0;
                    int is_first_range;
                    portmod_port_diag_info_t diag_info;
                    int diag_rv;
                    uint8 pcount=0;
                    char *pname, namelen;

                    phymod_core_access_t_init(&core_acc);
                    phymod_core_info_t_init(&core_info);
                    sal_memset(&diag_info, 0, sizeof(portmod_port_diag_info_t));

                    portmod_port_main_core_access_get(u, p, -1, &core_acc, &nof_cores);
                    if(nof_cores == 0) {
                        continue;
                    }
                    /* check if the phy is a legacy phy */
                    diag_rv = portmod_port_check_legacy_phy(u, p, &is_legacy);
                    if (diag_rv) {
                        continue;
                    }
                    if ( !is_legacy ) {
                        diag_rv = portmod_port_diag_info_get(u, p, &diag_info);
                        if(diag_rv){
                            continue;
                        }

                        diag_rv = portmod_port_core_num_get(u, p, &core_num);
                        if (diag_rv){
                            continue;
                        }

                        is_first_range = TRUE;
                        BCM_PBMP_ITER(diag_info.phys, phy){
                            if( is_first_range ){
                                range_start = phy ;
                                is_first_range = FALSE;
                            }
                        }

                        SOC_IF_ERROR_RETURN
                            (phymod_core_info_get(&core_acc, &core_info));

                        SOC_PBMP_COUNT(diag_info.phys, pcount);

                        pname = phymod_core_version_t_mapping[core_info.core_version].key;
                        namelen = strlen(pname);

                        sal_snprintf(lnstr, sizeof(lnstr), "%s", pname);
                        sal_snprintf(lnstr+namelen-2, sizeof(lnstr)-(namelen-2), "-%s/%02d/", pname+namelen-2, core_num);

                        pname = lnstr;
                        while (*pname != '-') {
                            *pname = sal_toupper(*pname);
                            pname++;
                        }

                        pname = lnstr+strlen(lnstr);
                        if (pcount == 4)
                            sal_snprintf(pname,sizeof(lnstr), "%d", pcount);
                        else if (pcount == 2)
                            sal_snprintf(pname,sizeof(lnstr), "%d-%d", (range_start-1)%4, ((range_start-1)%4)+1);
                        else
                            sal_snprintf(pname,sizeof(lnstr), "%d", (range_start-1)%4);
                    }else {
                        sal_snprintf(lnstr, sizeof(lnstr), "%s", soc_phy_name_get(u, p));
                    }
                }else
#endif
                {
                    sal_snprintf(lnstr, sizeof(lnstr), "%s", soc_phy_name_get(u, p));
                }

                BCM_API_XLATE_PORT_P2A(u, &ap); /* Use BCM API port */
                BCM_IF_ERROR_RETURN(bcm_port_phy_drv_name_get
                                    (u, ap, drv_name, 64));
                cli_out("Port %s (PHY addr 0x%02x): %s (%s)",
                        (SOC_PORT_VALID_RANGE(u, p) ? SOC_PORT_NAME(u, p):"unknown"),
                        phy_addr, lnstr,
                        drv_name);
            }
            if (is_c45 && !intermediate && !int_ovrride) {
                for (i = 0; i < COUNTOF(p_devad); i++) {
                    phy_devad = p_devad[i];
                    cli_out("\nDevAd = %d(%s)", phy_devad, p_devad_str[i]);
                    for (phy_reg = PHY_MIN_REG; phy_reg <= PHY_MAX_REG;
                         phy_reg++) {
#if defined(BCM_RCPU_SUPPORT) && defined(BCM_CMICM_SUPPORT)
                        if (1 == is_rcpu) {
                            rv = soc_rcpu_miimc45_read(u, phy_addr,
                                                       phy_devad, phy_reg,
                                                       &phy_data);
                        } else
#endif                          /* defined(BCM_RCPU_SUPPORT) && defined(BCM_CMICM_SUPPORT) */
                        if (EXT_PHY_SW_STATE(u, p) &&
                                (EXT_PHY_SW_STATE(u, p)->read)) {
                            rv = EXT_PHY_SW_STATE(u, p)->read(u, phy_addr,
                                                              BCM_PORT_PHY_CLAUSE45_ADDR
                                                              (phy_devad,
                                                               phy_reg),
                                                              &phy_data);
                        } else if (SOC_USE_PORTCTRL(u)) {
                            rv = soc_miimc45_read(u, phy_addr,
                                                  phy_devad, phy_reg,
                                                  &phy_data);
                        } else {
                            rv = soc_miimc45_read(u, phy_addr,
                                                  phy_devad, phy_reg,
                                                  &phy_data);
                        }

                        if (rv < 0) {
                            cli_out("\nERROR: Port %s: "
                                    "soc_miim_read failed: %s\n",
                                    SOC_PORT_NAME(u, p), soc_errmsg(rv));
			    return CMD_FAIL;
                        }
                        cli_out("%s\t0x%04x: 0x%04x",
                                ((phy_reg % DUMP_PHY_COLS) == 0) ? "\n" : "",
                                phy_reg, phy_data);
                    }
                }
            } else {
                for (phy_reg = PHY_MIN_REG; phy_reg <= PHY_MAX_REG; phy_reg++) {
#if defined(BCM_RCPU_SUPPORT) && defined(BCM_CMICM_SUPPORT)
                    if (1 == is_rcpu) {
                        rv = soc_rcpu_miim_read(u, phy_addr, phy_reg,
                                                &phy_data);
                    } else
#endif                          /* defined(BCM_RCPU_SUPPORT) && defined(BCM_CMICM_SUPPORT) */
                    if (!intermediate && !int_ovrride) {
                        if (EXT_PHY_SW_STATE(u, p) &&
                            (EXT_PHY_SW_STATE(u, p)->read)) {
                            rv = EXT_PHY_SW_STATE(u, p)->read(u, phy_addr,
                                                              phy_reg,
                                                              &phy_data);
                        } else if (SOC_USE_PORTCTRL(u)) {
                            rv = soc_sbus_mdio_read(u, phy_addr, phy_reg, &phy_data32);
                            phy_data = (uint16)phy_data32;
                        } else {
                            rv = soc_miim_read(u, phy_addr, phy_reg, &phy_data);
                        }
                    } else {
                        if (INT_PHY_SW_STATE(u, p) &&
                            (INT_PHY_SW_STATE(u, p)->read)) {
                            rv = INT_PHY_SW_STATE(u, p)->read(u, phy_addr,
                                                              phy_reg,
                                                              &phy_data);
                        } else if (SOC_USE_PORTCTRL(u)) {
                            rv = soc_sbus_mdio_read(u, phy_addr, phy_reg, &phy_data32);
                            phy_data = (uint16)phy_data32;
                        } else {
                            rv = soc_miim_read(u, phy_addr, phy_reg, &phy_data);
                        }
                    }
                    if (rv < 0) {
                        cli_out("\nERROR: Port %s: soc_miim_read failed: %s\n",
                                SOC_PORT_NAME(u, p), soc_errmsg(rv));
			return CMD_FAIL;
                    }
                    cli_out("%s\t0x%02x: 0x%04x",
                            ((phy_reg % DUMP_PHY_COLS) == 0) ? "\n" : "",
                            phy_reg, phy_data);
                }
            }
            cli_out("\n");
        }
    } else {                    /* get register argument */

#if defined(PHYMOD_SUPPORT)
        if (!isint(ARG_CUR(a))) {
            return phymod_sym_access(u, a, intermediate, &pbm);
        }
#endif /* defined(PHYMOD_SUPPORT) */

        c = ARG_GET(a);
        phy_reg = sal_ctoi(c, 0);

        if (ARG_CNT(a) == 0) {  /* no more args; show this register */
            DPORT_SOC_PBMP_ITER(u, pbm, dport, p) {
                phy_addr = (intermediate ?
                            port_to_phyaddr_int(u, p) : port_to_phyaddr(u, p));
                int_ovrride = (EXT_PHY_SW_STATE(u, p) == NULL) ? 1 : 0;
                if (phy_addr == 0xff) {
                    cli_out("Port %s: No %sPHY address assigned\n",
                            SOC_PORT_NAME(u, p),
                            intermediate ? "intermediate " : "");
                    continue;
                }
#ifdef PORTMOD_SUPPORT
            if (SOC_PORT_USE_PORTCTRL(u, p)) {
                portmod_port_num_phys_get(u, p, &nof_phys);
                int_ovrride = 0;
                if (nof_phys == 1) {
                    is_c45 = 0;
                } else {
                    is_c45 = portmod_port_flags_test(u, p, PHY_FLAGS_C45);
                }
            } else
#endif
            {
                is_c45 = soc_phy_is_c45_miim(u, p);
            }
                if ( is_c45 && !intermediate && !int_ovrride) {
                    for (i = 0; i < COUNTOF(p_devad); i++) {
                        phy_devad = p_devad[i];
                        cli_out("Port %s (PHY addr 0x%02x) "
                                "DevAd %d(%s) Reg 0x%04x: ",
                                SOC_PORT_NAME(u, p), phy_addr, phy_devad,
                                p_devad_str[i], phy_reg);
#if defined(BCM_RCPU_SUPPORT) && defined(BCM_CMICM_SUPPORT)
                        if (1 == is_rcpu) {
                            rv = soc_rcpu_miimc45_read(u, phy_addr,
                                                       phy_devad, phy_reg,
                                                       &phy_data);
                        } else
#endif                          /* defined(BCM_RCPU_SUPPORT) &&
                                   defined(BCM_CMICM_SUPPORT) */
                        if (EXT_PHY_SW_STATE(u, p) &&
                                (EXT_PHY_SW_STATE(u, p)->read)) {
                            rv = EXT_PHY_SW_STATE(u, p)->read(u, phy_addr,
                                                              BCM_PORT_PHY_CLAUSE45_ADDR
                                                              (phy_devad,
                                                               phy_reg),
                                                              &phy_data);
                        } else if (SOC_USE_PORTCTRL(u)) {
                            rv = soc_miimc45_read(u, phy_addr,phy_devad, phy_reg, &phy_data);
                        } else {
                            rv = soc_miimc45_read(u, phy_addr,
                                                  phy_devad, phy_reg,
                                                  &phy_data);
                        }

                        if (rv < 0) {
                            cli_out("\nERROR: Port %s: "
                                    "soc_miim_read failed: %s\n",
                                    SOC_PORT_NAME(u, p), soc_errmsg(rv));
			    return CMD_FAIL;
                        }
                        var_set_hex("phy_reg_data", phy_data, TRUE, FALSE);
                        cli_out("0x%04x\n", phy_data);
                    }
                } else {
                    cli_out("Port %s (PHY addr 0x%02x) Reg 0x%04x: ",
                            SOC_PORT_NAME(u, p), phy_addr, phy_reg);
#if defined(BCM_RCPU_SUPPORT) && defined(BCM_CMICM_SUPPORT)
                    if (1 == is_rcpu) {
                        rv = soc_rcpu_miim_read(u, phy_addr, phy_reg,
                                                &phy_data);
                    } else
#endif                          /* defined(BCM_RCPU_SUPPORT) && defined(BCM_CMICM_SUPPORT) */
                    if (!intermediate && !int_ovrride) {
                        if (EXT_PHY_SW_STATE(u, p) &&
                            (EXT_PHY_SW_STATE(u, p)->read)) {
                            rv = EXT_PHY_SW_STATE(u, p)->read(u, phy_addr,
                                                              phy_reg,
                                                              &phy_data);
                        } else if (SOC_PORT_USE_PORTCTRL(u, p)) {
#ifdef PORTMOD_SUPPORT
                            rv = portmod_port_phy_reg_read(u,p, -1, 0,phy_reg,&phy_data32);
                            phy_data = (uint16)phy_data32;
#endif
                        } else {
                            rv = soc_miim_read(u, phy_addr, phy_reg, &phy_data);
                        }
                    } else {
                        if (INT_PHY_SW_STATE(u, p) &&
                            (INT_PHY_SW_STATE(u, p)->read)) {
                            rv = INT_PHY_SW_STATE(u, p)->read(u, phy_addr,
                                                              phy_reg,
                                                              &phy_data);
                        } else if (SOC_PORT_USE_PORTCTRL(u, p)) {
#ifdef PORTMOD_SUPPORT
                            rv = portmod_port_phy_reg_read(u,p, -1, 0,phy_reg,&phy_data32);
                            phy_data = (uint16)phy_data32;
#endif
                        } else {
                            rv = soc_miim_read(u, phy_addr, phy_reg, &phy_data);
                        }
                    }

                    if (rv < 0) {
                        cli_out("\nERROR: Port %s: soc_miim_read failed: %s\n",
                                SOC_PORT_NAME(u, p), soc_errmsg(rv));
			return CMD_FAIL;
                    }
                    var_set_hex("phy_reg_data", phy_data, TRUE, FALSE);
                    cli_out("0x%04x\n", phy_data);
                }
            }
        } else {                /* set the reg to given value for the indicated phys */
            c = ARG_GET(a);
            phy_data = phy_devad = sal_ctoi(c, 0);

            DPORT_SOC_PBMP_ITER(u, pbm, dport, p) {
                phy_addr = (intermediate ?
                            port_to_phyaddr_int(u, p) : port_to_phyaddr(u, p));
                int_ovrride = (EXT_PHY_SW_STATE(u, p) == NULL) ? 1 : 0;
                if (phy_addr == 0xff) {
                    cli_out("Port %s: No %sPHY address assigned\n",
                            SOC_PORT_NAME(u, p),
                            intermediate ? "intermediate " : "");
		    return CMD_FAIL;
                }
#ifdef PORTMOD_SUPPORT
                if (SOC_PORT_USE_PORTCTRL(u, p)) {
                    portmod_port_num_phys_get(u, p, &nof_phys);
                    int_ovrride = 0;
                    if (nof_phys == 1) {
                        is_c45 = 0;
                    } else {
                        is_c45 = portmod_port_flags_test(u, p, PHY_FLAGS_C45);
                    }
                } else
#endif
                {
                    is_c45 = soc_phy_is_c45_miim(u, p);
                }
                if ( is_c45 && !intermediate && !int_ovrride) {
                    for (i = 0; i < COUNTOF(p_devad); i++) {
                        if (phy_devad == p_devad[i]) {
                            break;
                        }
                    }
                    if (i >= COUNTOF(p_devad)) {
                        cli_out("\nERROR: Port %s: Invalid DevAd %d\n",
                                SOC_PORT_NAME(u, p), phy_devad);
			return CMD_FAIL;
                    }
                    if (ARG_CNT(a) == 0) {/* no more args; show this register */
                        cli_out("Port %s (PHY addr 0x%02x) "
                                "DevAd %d(%s) Reg 0x%04x: ",
                                SOC_PORT_NAME(u, p), phy_addr, phy_devad,
                                p_devad_str[i], phy_reg);
#if defined(BCM_RCPU_SUPPORT) && defined(BCM_CMICM_SUPPORT)
                        if (1 == is_rcpu) {
                            rv = soc_rcpu_miimc45_read(u, phy_addr,
                                                       phy_devad, phy_reg,
                                                       &phy_data);
                        } else
#endif                          /* defined(BCM_RCPU_SUPPORT) &&
                                   defined(BCM_CMICM_SUPPORT) */
                        if (EXT_PHY_SW_STATE(u, p) &&
                                (EXT_PHY_SW_STATE(u, p)->read)) {
                            rv = EXT_PHY_SW_STATE(u, p)->read(u, phy_addr,
                                                              BCM_PORT_PHY_CLAUSE45_ADDR
                                                              (phy_devad,
                                                               phy_reg),
                                                              &phy_data);
                        } else if (SOC_USE_PORTCTRL(u)) {
                            rv = soc_miimc45_read(u, phy_addr,
                                                  phy_devad, phy_reg,
                                                  &phy_data);

                        } else {
                            rv = soc_miimc45_read(u, phy_addr,
                                                  phy_devad, phy_reg,
                                                  &phy_data);
                        }

                        if (rv < 0) {
                            cli_out("\nERROR: Port %s: "
                                    "soc_miim_read failed: %s\n",
                                    SOC_PORT_NAME(u, p), soc_errmsg(rv));
			    return CMD_FAIL;
                        }
                        var_set_hex("phy_reg_data", phy_data, TRUE, FALSE);
                        cli_out("0x%04x\n", phy_data);
                    } else {    /* write */
                        c = ARG_GET(a);
                        phy_data = sal_ctoi(c, 0);
#if defined(BCM_RCPU_SUPPORT) && defined(BCM_CMICM_SUPPORT)
                        if (1 == is_rcpu) {
                            rv = soc_rcpu_miimc45_write(u, phy_addr,
                                                        phy_devad, phy_reg,
                                                        phy_data);
                        } else
#endif                          /* defined(BCM_RCPU_SUPPORT) && defined(BCM_CMICM_SUPPORT) */
                        if (EXT_PHY_SW_STATE(u, p) &&
                                (EXT_PHY_SW_STATE(u, p)->write)) {
                            rv = EXT_PHY_SW_STATE(u, p)->write(u, phy_addr,
                                                               BCM_PORT_PHY_CLAUSE45_ADDR
                                                               (phy_devad,
                                                                phy_reg),
                                                               phy_data);
                        } else if (SOC_PORT_USE_PORTCTRL(u, p)) {
#ifdef PORTMOD_SUPPORT
                            portmod_port_num_phys_get(u, p, &nof_phys);
                            if(nof_phys > 1 ){
                                rv = soc_miimc45_write(u, phy_addr,
                                                       phy_devad, phy_reg,
                                                       phy_data);
                            } else
#endif
                            {
                                rv = soc_sbus_mdio_write(u, phy_addr, phy_reg, phy_data);
                            }
                        } else {
                            rv = soc_miimc45_write(u, phy_addr,
                                                   phy_devad, phy_reg,
                                                   phy_data);
                        }

                        if (rv < 0) {
                            cli_out("ERROR: Port %s: "
                                    "soc_miim_write failed: %s\n",
                                    SOC_PORT_NAME(u, p), soc_errmsg(rv));
			    return CMD_FAIL;
                        }
                    }
                } else {
#if defined(BCM_RCPU_SUPPORT) && defined(BCM_CMICM_SUPPORT)
                    if (1 == is_rcpu) {
                        rv = soc_rcpu_miim_write(u, phy_addr, phy_reg,
                                                 phy_data);
                    } else
#endif                          /* defined(BCM_RCPU_SUPPORT) && defined(BCM_CMICM_SUPPORT) */
                    if (!intermediate && !int_ovrride) {
                        if (EXT_PHY_SW_STATE(u, p) &&
                            (EXT_PHY_SW_STATE(u, p)->write)) {
                            rv = EXT_PHY_SW_STATE(u, p)->write(u, phy_addr,
                                                               phy_reg,
                                                               phy_data);
                        } else if (SOC_PORT_USE_PORTCTRL(u, p)) {
#ifdef PORTMOD_SUPPORT
                            portmod_port_num_phys_get(u, p, &nof_phys);
                            if(nof_phys > 1 ){
                                rv = soc_miimc45_write(u, phy_addr,
                                                       phy_devad, phy_reg,
                                                       phy_data);
                            } else {
                                rv = portmod_port_phy_reg_write(u, p, -1, 0, phy_reg, phy_data);
                            }
#endif
                        } else {
                            rv = soc_miim_write(u, phy_addr, phy_reg, phy_data);
                        }
                    } else {
                        if (INT_PHY_SW_STATE(u, p) &&
                            (INT_PHY_SW_STATE(u, p)->write)) {
                            rv = INT_PHY_SW_STATE(u, p)->write(u, phy_addr,
                                                               phy_reg,
                                                               phy_data);
                        } else if (SOC_PORT_USE_PORTCTRL(u, p)) {
#ifdef PORTMOD_SUPPORT
                            rv = portmod_port_phy_reg_write(u, p, -1, 0, phy_reg, phy_data);
#endif
                        } else {
                            rv = soc_miim_write(u, phy_addr, phy_reg, phy_data);
                        }
                    }
                    if (rv < 0) {
                        cli_out("ERROR: Port %s: soc_miim_write failed: %s\n",
                                SOC_PORT_NAME(u, p), soc_errmsg(rv));
			return CMD_FAIL;
                    }
                }
            }
        }
    }

    if (BCM_FAILURE(rv)) {
	    cli_out("%s\n", bcm_errmsg(rv));
	    return CMD_FAIL;
    }
    return CMD_OK;
}

/***********************************************************************
 *
 * Combo port support
 */

/*
 * Function:    if_combo_dump
 * Purpose:     Dump the contents of a bcm_phy_config_t
 */

STATIC int
if_combo_dump(args_t *a, int u, int p, int medium)
{
    char pm_str[80];
    bcm_port_medium_t active_medium;
    int r;
    bcm_phy_config_t cfg;

    /*
     * Get active medium so we can put an asterisk next to the status if
     * it is active.
     */

    if ((r = bcm_port_medium_get(u, p, &active_medium)) < 0) {
        return r;
    }
    if ((r = bcm_port_medium_config_get(u, p, medium, &cfg)) < 0) {
        return r;
    }

    cli_out("%s:\t%s medium%s\n",
            BCM_PORT_NAME(u, p),
            MEDIUM_STATUS(medium), (medium == active_medium) ? " (active)" : "");

    format_port_mode(pm_str, sizeof(pm_str), cfg.autoneg_advert, TRUE);

    cli_out("\tenable=%d preferred=%d "
            "force_speed=%d force_duplex=%d master=%s\n",
            cfg.enable, cfg.preferred,
            cfg.force_speed, cfg.force_duplex, PHYMASTER_MODE(cfg.master));
    cli_out("\tautoneg_enable=%d autoneg_advert=%s(0x%x)\n",
            cfg.autoneg_enable, pm_str, cfg.autoneg_advert);
    cli_out("\tMDIX=%s\n", MDIX_MODE(cfg.mdix));

    return BCM_E_NONE;
}

static int combo_watch[SOC_MAX_NUM_DEVICES][SOC_MAX_NUM_PORTS];

static void
if_combo_watch(int unit, bcm_port_t port, bcm_port_medium_t medium, void *arg)
{
    cli_out("Unit %d: %s: Active medium switched to %s\n",
            unit, BCM_PORT_NAME(unit, port), MEDIUM_STATUS(medium));

    /*
     * Increment the number of medium changes. Remember, that we pass the
     * address of combo_watch[unit][port] when we register this callback
     */
    (*((int *)arg))++;
}

/*
 * Function:    if_combo
 * Purpose:     Control combo ports
 * Parameters:  u - SOC unit #
 *              a - pointer to args
 * Returns:     CMD_OK/CMD_FAIL/
 */
cmd_result_t
if_esw_combo(int u, args_t *a)
{
    pbmp_t pbm;
    soc_port_t p, dport;
    int specified_medium = BCM_PORT_MEDIUM_COUNT;
    int r, rc, rf;
    char *c;
    parse_table_t pt;
    bcm_phy_config_t cfg, cfg_opt;

    enum if_combo_cmd_e {
        COMBO_CMD_DUMP,
        COMBO_CMD_SET,
        COMBO_CMD_WATCH,
        CONBO_CMD_COUNT
    } cmd;

    enum if_combo_watch_arg_e {
        COMBO_CMD_WATCH_SHOW,
        COMBO_CMD_WATCH_ON,
        COMBO_CMD_WATCH_OFF,
        COMBO_CMD_WATCH_COUNT
    } watch_arg = COMBO_CMD_WATCH_SHOW;

    cfg_opt.enable = -1;
    cfg_opt.preferred = -1;
    cfg_opt.autoneg_enable = -1;
    cfg_opt.autoneg_advert = 0xffffffff;
    cfg_opt.force_speed = -1;
    cfg_opt.force_duplex = -1;
    cfg_opt.master = -1;
    cfg_opt.mdix = -1;

    if (!sh_check_attached(ARG_CMD(a), u)) {
        return CMD_FAIL;
    }

    if ((c = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (parse_bcm_pbmp(u, c, &pbm) < 0) {
        cli_out("%s: ERROR: unrecognized port bitmap: %s\n", ARG_CMD(a), c);
        return CMD_FAIL;
    }

    SOC_PBMP_AND(pbm, PBMP_PORT_ALL(u));

    c = ARG_GET(a);             /* NULL or media type or command */

    if (c == NULL) {
        cmd = COMBO_CMD_DUMP;
        specified_medium = BCM_PORT_MEDIUM_COUNT;
    } else if (sal_strcasecmp(c, "copper") == 0 || sal_strcasecmp(c, "c") == 0) {
        cmd = COMBO_CMD_SET;
        specified_medium = BCM_PORT_MEDIUM_COPPER;
    } else if (sal_strcasecmp(c, "fiber") == 0 || sal_strcasecmp(c, "f") == 0) {
        cmd = COMBO_CMD_SET;
        specified_medium = BCM_PORT_MEDIUM_FIBER;
    } else if (sal_strcasecmp(c, "watch") == 0 || sal_strcasecmp(c, "w") == 0) {
        cmd = COMBO_CMD_WATCH;
    } else {
        return CMD_USAGE;
    }

    switch (cmd) {
        case COMBO_CMD_SET:
            if ((c = ARG_CUR(a)) != NULL) {
                if (c[0] == '=') {
                    return CMD_USAGE;   /* '=' unsupported */
                }

                parse_table_init(u, &pt);
                parse_table_add(&pt, "Enable", PQ_DFL | PQ_BOOL, 0,
                                &cfg_opt.enable, 0);
                parse_table_add(&pt, "PREFerred", PQ_DFL | PQ_BOOL, 0,
                                &cfg_opt.preferred, 0);
                parse_table_add(&pt, "Autoneg_Enable", PQ_DFL | PQ_BOOL, 0,
                                &cfg_opt.autoneg_enable, 0);
                parse_table_add(&pt, "Autoneg_Advert", PQ_DFL | PQ_PORTMODE, 0,
                                &cfg_opt.autoneg_advert, 0);
                parse_table_add(&pt, "Force_Speed", PQ_DFL | PQ_INT, 0,
                                &cfg_opt.force_speed, 0);
                parse_table_add(&pt, "Force_Duplex", PQ_DFL | PQ_BOOL, 0,
                                &cfg_opt.force_duplex, 0);
                parse_table_add(&pt, "MAster", PQ_DFL | PQ_BOOL, 0,
                                &cfg_opt.master, 0);
                parse_table_add(&pt, "MDIX", PQ_DFL | PQ_MULTI, 0,
                                &cfg_opt.mdix, mdix_mode);

                if (parse_arg_eq(a, &pt) < 0) {
                    parse_arg_eq_done(&pt);
                    return CMD_USAGE;
                }
                parse_arg_eq_done(&pt);

                if (ARG_CUR(a) != NULL) {
                    return CMD_USAGE;
                }
            } else {
                cmd = COMBO_CMD_DUMP;
            }

            break;

        case COMBO_CMD_WATCH:
            c = ARG_GET(a);

            if (c == NULL) {
                watch_arg = COMBO_CMD_WATCH_SHOW;
            } else if (sal_strcasecmp(c, "on") == 0) {
                watch_arg = COMBO_CMD_WATCH_ON;
            } else if (sal_strcasecmp(c, "off") == 0) {
                watch_arg = COMBO_CMD_WATCH_OFF;
            } else {
                return CMD_USAGE;
            }
            break;

        default:
            break;
    }

    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(u, pbm, dport, p) {
        switch (cmd) {
            case COMBO_CMD_DUMP:
                cli_out("Port %s:\n", BCM_PORT_NAME(u, p));

                rc = rf = BCM_E_UNAVAIL;
                if (specified_medium == BCM_PORT_MEDIUM_COPPER ||
                    specified_medium == BCM_PORT_MEDIUM_COUNT) {
                    rc = if_combo_dump(a, u, p, BCM_PORT_MEDIUM_COPPER);
                    if (rc != BCM_E_NONE && rc != BCM_E_UNAVAIL) {
                        cli_out("%s:\tERROR(copper): %s\n",
                                BCM_PORT_NAME(u, p), bcm_errmsg(rc));
                    }
                }

                if (specified_medium == BCM_PORT_MEDIUM_FIBER ||
                    specified_medium == BCM_PORT_MEDIUM_COUNT) {
                    rf = if_combo_dump(a, u, p, BCM_PORT_MEDIUM_FIBER);
                    if (rf != BCM_E_NONE && rf != BCM_E_UNAVAIL) {
                        cli_out("%s:\tERROR(fiber): %s\n",
                                BCM_PORT_NAME(u, p), bcm_errmsg(rf));
                    }
                }

                /*
                 * If there were problems getting medium-specific info on
                 * individual mediums, then they will be printed above. However,
                 * if BCM_E_UNAVAIL is returned for both copper and fiber mediums
                 * we'll print only one error message
                 */
                if (rc == BCM_E_UNAVAIL && rf == BCM_E_UNAVAIL) {
                    cli_out("%s:\tmedium info unavailable\n",
                            BCM_PORT_NAME(u, p));
                }
                break;

            case COMBO_CMD_SET:
                /*
                 * Update the medium operating mode.
                 */
                r = bcm_port_medium_config_get(u, p, specified_medium, &cfg);

                if (r < 0) {
                    cli_out("%s: port %s: Error getting medium config: %s\n",
                            ARG_CMD(a), BCM_PORT_NAME(u, p), bcm_errmsg(r));
                    return CMD_FAIL;
                }

                if (cfg_opt.enable != -1) {
                    cfg.enable = cfg_opt.enable;
                }

                if (cfg_opt.preferred != -1) {
                    cfg.preferred = cfg_opt.preferred;
                }

                if (cfg_opt.autoneg_enable != -1) {
                    cfg.autoneg_enable = cfg_opt.autoneg_enable;
                }

                if (cfg_opt.autoneg_advert != 0xffffffff) {
                    cfg.autoneg_advert = cfg_opt.autoneg_advert;
                }

                if (cfg_opt.force_speed != -1) {
                    cfg.force_speed = cfg_opt.force_speed;
                }

                if (cfg_opt.force_duplex != -1) {
                    cfg.force_duplex = cfg_opt.force_duplex;
                }

                if (cfg_opt.master != -1) {
                    cfg.master = cfg_opt.master;
                }

                if (cfg_opt.mdix != -1) {
                    cfg.mdix = cfg_opt.mdix;
                }

                r = bcm_port_medium_config_set(u, p, specified_medium, &cfg);

                if (r < 0) {
                    cli_out("%s: port %s: Error setting medium config: %s\n",
                            ARG_CMD(a), BCM_PORT_NAME(u, p), bcm_errmsg(r));
                    return CMD_FAIL;
                }

                break;

            case COMBO_CMD_WATCH:
                switch (watch_arg) {
                    case COMBO_CMD_WATCH_SHOW:
                        if (combo_watch[u][p]) {
                            cli_out("Port %s: Medium status "
                                    "change watch is  ON. "
                                    "Medim changed %d times\n",
                                    BCM_PORT_NAME(u, p),
                                    combo_watch[u][p] - 1);
                        } else {
                            cli_out("Port %s: Medium status "
                                    "change watch is OFF.\n",
                                    BCM_PORT_NAME(u, p));
                        }
                        break;

                    case COMBO_CMD_WATCH_ON:
                        if (!combo_watch[u][p]) {
                            r = bcm_port_medium_status_register(u, p,
                                                                if_combo_watch,
                                                                &combo_watch[u]
                                                                [p]);
                            if (r < 0) {
                                cli_out("Error registerinig medium "
                                        "status change callback for %s: %s\n",
                                        BCM_PORT_NAME(u, p),
                                        soc_errmsg(r));
                                return (CMD_FAIL);
                            }

                            combo_watch[u][p] = 1;
                        }

                        cli_out("Port %s: Medium change watch is ON\n",
                                BCM_PORT_NAME(u, p));

                        break;

                    case COMBO_CMD_WATCH_OFF:
                        if (combo_watch[u][p]) {
                            r = bcm_port_medium_status_unregister(u, p,
                                                                  if_combo_watch,
                                                                  &combo_watch
                                                                  [u][p]);
                            if (r < 0) {
                                cli_out("Error unregisterinig medium "
                                        "status change callback for %s: %s\n",
                                        BCM_PORT_NAME(u, p),
                                        soc_errmsg(r));
                                return (CMD_FAIL);
                            }

                            combo_watch[u][p] = 0;
                        }

                        cli_out("Port %s: Medium change watch is OFF\n",
                                BCM_PORT_NAME(u, p));

                        break;

                    default:
                        return CMD_FAIL;
                } /* switch ( watch_arg ) */

                break;

            /* must default. for covering all the cases of 'enum if_combo_cmd_e' */
            /* coverity[dead_error_begin : FALSE] */
            default:
                return CMD_FAIL;
        }  /* switch ( cmd ) */
    }

    return CMD_OK;
}

/*
 * Function:
 *    cmd_cablediag
 * Purpose:
 *    Run cable diagnostics (if available)
 */

cmd_result_t
cmd_esw_cablediag(int unit, args_t *a)
{
    char *s;
    bcm_pbmp_t pbm;
    bcm_port_t port, dport;
    int rv, i;
    bcm_port_cable_diag_t cd;
    char *statename[] = _SHR_PORT_CABLE_STATE_NAMES_INITIALIZER;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if ((s = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (parse_bcm_pbmp(unit, s, &pbm) < 0) {
        cli_out("%s: ERROR: unrecognized port bitmap: %s\n", ARG_CMD(a), s);
        return CMD_FAIL;
    }

    sal_memset(&cd, 0, sizeof(bcm_port_cable_diag_t));

    /* Coverity
     * DPORT_BCM_PBMP_ITER checks that dport is valid.
     */
    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pbm, dport, port) {
        rv = bcm_port_cable_diag(unit, port, &cd);
        if (rv < 0) {
            cli_out("%s: ERROR: port %s: %s\n",
                    ARG_CMD(a), BCM_PORT_NAME(unit, port), bcm_errmsg(rv));
            continue;
        }
        if (cd.fuzz_len == 0) {
            cli_out("port %s: cable (%d pairs)\n",
                    BCM_PORT_NAME(unit, port), cd.npairs);
        } else {
            cli_out("port %s: cable (%d pairs, length +/- %d meters)\n",
                    BCM_PORT_NAME(unit, port), cd.npairs, cd.fuzz_len);
        }
        for (i = 0; i < cd.npairs; i++) {
            cli_out("\tpair %c %s, length %d meters\n",
                    'A' + i, statename[cd.pair_state[i]], cd.pair_len[i]);
        }
    }

    return CMD_OK;
}

#ifdef BCM_XGS3_SWITCH_SUPPORT
/* XAUI BERT Test */

char cmd_xaui_usage[] =
    "Run XAUI BERT.\n"
    "Usages:\n\t"
    "  xaui bert SrcPort=<port> DestPort=<port> Duration=<usec> Verb=0/1\n";

#define XAUI_PREEMPHASIS_MIN  (0)
#define XAUI_PREEMPHASIS_MAX  (15)
#define XAUI_IDRIVER_MIN      (0)
#define XAUI_IDRIVER_MAX      (15)
#define XAUI_EQUALIZER_MIN    (0)
#define XAUI_EQUALIZER_MAX    (7)

typedef struct _xaui_bert_info_s {
    bcm_port_t src_port;
    bcm_port_t dst_port;
    soc_xaui_config_t src_config;
    soc_xaui_config_t dst_config;
    soc_xaui_config_t test_config;
    bcm_port_info_t src_info;
    bcm_port_info_t dst_info;
    int duration;
    int linkscan_us;
    int verbose;
} _xaui_bert_info_t;

/*
 * Function:
 *      _xaui_bert_counter_check
 * Purpose:
 *      Check BERT counters after the test.
 * Parameters:
 *      (IN) unit       - BCM unit number
 *      (IN) test_info - Test configuration
 * Returns:
 *      BCM_E_NONE - success
 *      BCM_E_XXXX - failed.
 * Notes:
 */
static int
_xaui_bert_counter_check(int unit, _xaui_bert_info_t *test_info)
{
    bcm_port_t src_port, dst_port;
    uint32 tx_pkt, tx_byte, rx_pkt, rx_byte, bit_err, byte_err, pkt_err;
    int prbs_lock, lock;

    src_port = test_info->src_port;
    dst_port = test_info->dst_port;

    /* Read Tx counters */
    SOC_IF_ERROR_RETURN(soc_xaui_txbert_pkt_count_get(unit, src_port, &tx_pkt));
    SOC_IF_ERROR_RETURN
        (soc_xaui_txbert_byte_count_get(unit, src_port, &tx_byte));

    lock = 1;
    /* Read Rx counters */
    SOC_IF_ERROR_RETURN
        (soc_xaui_rxbert_pkt_count_get(unit, dst_port, &rx_pkt, &prbs_lock));
    lock &= prbs_lock;

    SOC_IF_ERROR_RETURN
        (soc_xaui_rxbert_byte_count_get(unit, dst_port, &rx_byte, &prbs_lock));
    lock &= prbs_lock;

    SOC_IF_ERROR_RETURN
        (soc_xaui_rxbert_bit_err_count_get(unit, dst_port,
                                           &bit_err, &prbs_lock));
    lock &= prbs_lock;

    SOC_IF_ERROR_RETURN
        (soc_xaui_rxbert_byte_err_count_get(unit, dst_port,
                                            &byte_err, &prbs_lock));
    lock &= prbs_lock;

    SOC_IF_ERROR_RETURN
        (soc_xaui_rxbert_pkt_err_count_get(unit, dst_port,
                                           &pkt_err, &prbs_lock));
    lock &= prbs_lock;

    if (test_info->verbose) {
        cli_out(" %4s->%4s, 0x%08x, 0x%08x, 0x%08x, %s, ",
                BCM_PORT_NAME(unit, src_port), BCM_PORT_NAME(unit, dst_port),
                tx_byte, rx_byte, bit_err, lock ? "       OK" : "      !OK");
    }

    /* Check TX/RX counters */
    if ((tx_byte == 0) || (tx_pkt == 0) ||
        (tx_byte != rx_byte) || (tx_pkt != rx_pkt) || !lock) {
        return BCM_E_FAIL;
    }

    /* Check error counters */
    if ((bit_err != 0) || (byte_err != 0) || (pkt_err != 0)) {
        return BCM_E_FAIL;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _xaui_bert_test
 * Purpose:
 *      Run BERT test with requested port configuration.
 * Parameters:
 *      (IN) unit       - BCM unit number
 *      (IN) test_info - Test configuration
 * Returns:
 *      BCM_E_NONE - success
 *      BCM_E_XXXX - failed.
 * Notes:
 */
static int
_xaui_bert_test(int unit, _xaui_bert_info_t *test_info)
{
    int result1, result2;
    bcm_port_t src_port, dst_port;

    src_port = test_info->src_port;
    dst_port = test_info->dst_port;

    BCM_IF_ERROR_RETURN(bcm_port_speed_set(unit, src_port, 10000));
    BCM_IF_ERROR_RETURN(bcm_port_speed_set(unit, dst_port, 10000));

    BCM_IF_ERROR_RETURN
        (soc_xaui_config_set(unit, src_port, &test_info->test_config));
    BCM_IF_ERROR_RETURN
        (soc_xaui_config_set(unit, dst_port, &test_info->test_config));

    /* Wait up to 0.1 sec for TX PLL lock */
    sal_usleep(100000);

    /* Enable RX BERT on both ports first */
    BCM_IF_ERROR_RETURN(soc_xaui_rxbert_enable(unit, dst_port, TRUE));
    if (src_port != dst_port) {
        BCM_IF_ERROR_RETURN(soc_xaui_rxbert_enable(unit, src_port, TRUE));
    }

    /* Enable TX BERT on both ports */
    BCM_IF_ERROR_RETURN(soc_xaui_txbert_enable(unit, src_port, TRUE));
    if (src_port != dst_port) {
        BCM_IF_ERROR_RETURN(soc_xaui_txbert_enable(unit, dst_port, TRUE));
    }

    /* Run test for requested duration */
    sal_usleep(test_info->duration);

    /* Disable TX BERT */
    BCM_IF_ERROR_RETURN(soc_xaui_txbert_enable(unit, src_port, FALSE));
    if (src_port != dst_port) {
        BCM_IF_ERROR_RETURN(soc_xaui_txbert_enable(unit, dst_port, FALSE));
    }

    /* Give enough time to complete tx/rx */
    sal_usleep(500);
    result1 = _xaui_bert_counter_check(unit, test_info);
    result2 = _xaui_bert_counter_check(unit, test_info);
    if (BCM_SUCCESS(result1) && BCM_SUCCESS(result2)) {
        cli_out(" ( P ) ");
    } else {
        cli_out(" ( F ) ");
    }

    if (test_info->verbose) {
        cli_out("\n");
    }

    /* Disable RX BERT only after reading the counter.
     * Otherwise, counters always read zero.
     */
    BCM_IF_ERROR_RETURN(soc_xaui_rxbert_enable(unit, src_port, FALSE));
    if (src_port != dst_port) {
        BCM_IF_ERROR_RETURN(soc_xaui_rxbert_enable(unit, dst_port, FALSE));
    }

    if ((BCM_E_NONE != result1) && (BCM_E_FAIL != result1)) {
        return result1;
    }
    if ((BCM_E_NONE != result2) && (BCM_E_FAIL != result2)) {
        return result2;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _xaui_bert_save_config
 * Purpose:
 *      Disable linkscan and save current port configuration.
 * Parameters:
 *      (IN) unit       - BCM unit number
 *      (OUT) test_info - Current port configuration
 * Returns:
 *      BCM_E_NONE - success
 *      BCM_E_XXXX - failed.
 * Notes:
 */
static int
_xaui_bert_save_config(int unit, _xaui_bert_info_t *test_info)
{

    /* If linkscan is enabled, disable linkscan */
    BCM_IF_ERROR_RETURN(bcm_linkscan_enable_get(unit, &test_info->linkscan_us));
    if (test_info->linkscan_us != 0) {
        BCM_IF_ERROR_RETURN(bcm_linkscan_enable_set(unit, 0));
        /* Give enough time for linkscan task to exit. */
        sal_usleep(test_info->linkscan_us * 5);
    }

    /* Save current settings */
    BCM_IF_ERROR_RETURN
        (soc_xaui_config_get(unit, test_info->src_port,
                             &test_info->src_config));
    BCM_IF_ERROR_RETURN
        (soc_xaui_config_get(unit, test_info->dst_port,
                             &test_info->dst_config));

    /* Save original speed settings */
    BCM_IF_ERROR_RETURN
        (bcm_port_info_save(unit, test_info->src_port, &test_info->src_info));
    BCM_IF_ERROR_RETURN
        (bcm_port_info_save(unit, test_info->dst_port, &test_info->dst_info));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _xaui_bert_restore_config
 * Purpose:
 *      Restore original port configuration.
 * Parameters:
 *      (IN) unit      - BCM unit number
 *      (IN) test_info - Port configuration to be restored
 * Returns:
 *      BCM_E_NONE - success
 *      BCM_E_XXXX - failed.
 * Notes:
 */
static int
_xaui_bert_restore_config(int unit, _xaui_bert_info_t *test_info)
{

    BCM_IF_ERROR_RETURN
        (bcm_port_info_restore(unit, test_info->src_port,
                               &test_info->src_info));
    BCM_IF_ERROR_RETURN
        (bcm_port_info_restore(unit, test_info->dst_port,
                               &test_info->dst_info));

#if 0
    /* Restore original configuration */
    BCM_IF_ERROR_RETURN
        (soc_xaui_config_set(unit, test_info->src_port,
                             &test_info->src_config));
    BCM_IF_ERROR_RETURN
        (soc_xaui_config_set(unit, test_info->dst_port,
                             &test_info->dst_config));
#endif

    if (test_info->linkscan_us != 0) {
        BCM_IF_ERROR_RETURN
            (bcm_linkscan_enable_set(unit, test_info->linkscan_us));
    }

    return BCM_E_NONE;
}

char bert_header[] =
    "                                   Equalizer\n"
    "I_Driver     0      1      2      3      4      5      6      7\n";

char bert_header_v[] =
    "\n Preemph, I_Driver, Equalizer,   Src->Des,"
    "    tx_byte,    rx_byte,    bit_err, PRBS Lock,"
    "    Des->Src,    tx_byte,    rx_byte,    bit_err," " PRBS Lock\n";

/*
 * Function:
 *      cmd_xaui
 * Purpose:
 *      Entry point to XAUI related CLI commands.
 * Parameters:
 *      (IN) unit - BCM unit number
 *      (IN) a    - Arguments for the command
 * Returns:
 *      BCM_E_NONE - success
 *      BCM_E_XXXX - failed.
 * Notes:
 */
cmd_result_t
cmd_xaui(int unit, args_t *a)
{
    char *subcmd;
    parse_table_t pt;
    int rv;
    cmd_result_t retCode;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    rv = BCM_E_NONE;
    if (sal_strcasecmp(subcmd, "bert") == 0) {
        uint32 preemphasis, idriver, equalizer;
        _xaui_bert_info_t test_info;

        sal_memset(&test_info, 0, sizeof(test_info));
        test_info.duration = 10;

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "SrcPort", PQ_DFL | PQ_PORT, 0,
                        &test_info.src_port, NULL);
        parse_table_add(&pt, "DestPort", PQ_DFL | PQ_PORT, 0,
                        &test_info.dst_port, NULL);
        parse_table_add(&pt, "Duration", PQ_INT | PQ_DFL, 0,
                        &test_info.duration, NULL);
        parse_table_add(&pt, "Verbose", PQ_BOOL | PQ_DFL, 0,
                        &test_info.verbose, NULL);

        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }

        /* Run only on HG and XE port */
        if ((!IS_HG_PORT(unit, test_info.src_port) &&
             !IS_XE_PORT(unit, test_info.src_port)) ||
            (!IS_HG_PORT(unit, test_info.dst_port) &&
             !IS_XE_PORT(unit, test_info.dst_port))) {
            cli_out("%s: ERROR: Invalid port selection %d, %d\n",
                    ARG_CMD(a), test_info.src_port, test_info.dst_port);
            return CMD_FAIL;
        }

        if ((rv = _xaui_bert_save_config(unit, &test_info)) < 0) {
            goto cmd_xaui_err;
        }

        test_info.test_config = test_info.src_config;
        for (preemphasis = XAUI_PREEMPHASIS_MIN;
             preemphasis <= XAUI_PREEMPHASIS_MAX; preemphasis++) {
            test_info.test_config.preemphasis = preemphasis;

            if (!test_info.verbose) {
                cli_out("\nPreemphasis = %d\n", preemphasis);
            }

            cli_out("%s", test_info.verbose ? bert_header_v : bert_header);

            for (idriver = XAUI_IDRIVER_MIN;
                 idriver <= XAUI_IDRIVER_MAX; idriver++) {
                test_info.test_config.idriver = idriver;
                if (!test_info.verbose) {
                    cli_out("%8d  ", idriver);
                }
                for (equalizer = XAUI_EQUALIZER_MIN;
                     equalizer <= XAUI_EQUALIZER_MAX; equalizer++) {

                    if (test_info.verbose) {
                        cli_out("%8d, %8d, %9d,", preemphasis, idriver,
                                equalizer);
                    }

                    test_info.test_config.equalizer_ctrl = equalizer;
                    if ((rv = _xaui_bert_test(unit, &test_info)) < 0) {
                        _xaui_bert_restore_config(unit, &test_info);
                        goto cmd_xaui_err;
                    }
                }
                cli_out("\n");
            }
        }

        if ((rv = _xaui_bert_restore_config(unit, &test_info)) < 0) {
            goto cmd_xaui_err;
        }
        return CMD_OK;
    }
    cli_out("%s: ERROR: Unknown xaui subcommand: %s\n", ARG_CMD(a), subcmd);
    return CMD_USAGE;

 cmd_xaui_err:
    cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(rv));
    return CMD_FAIL;
}
#endif                          /* BCM_XGS3_SWITCH_SUPPORT */

#if defined(INCLUDE_FCMAP)
/* BFCMAP TEST */
int
diag_bfcmap_pcfg_set_get(int port)
{
    int rc = CMD_OK;

    rc = bfcmap_pcfg_set_get(port);

    return (rc == BFCMAP_E_NONE) ? CMD_OK : CMD_FAIL;
}

int bfcmap88060_show_fc_config(int unit, int port)
{
    int rv = CMD_OK;
    bcm_fcmap_port_config_t pCfg;

    /* Setting cos to invalid COS value to
     * get current configured cos and priority value
     */
    pCfg.cos_to_pri.cos = 0xFFFF;

    rv = bcm_fcmap_port_config_get(unit, port, &pCfg);

    if (rv != BCM_E_NONE) {
        cli_out("Error in getting FC port config!!\n");
        return CMD_FAIL;
    }

    cli_out("    XMOD_FCMAP_ATTR_xxx.                                                     | action_mask                         : %#x\n",    pCfg.action_mask);
    cli_out("    XMOD_FCMAP_ATTR2_xxx.                                                    | action_mask2                        : %#x\n",    pCfg.action_mask2);
    cli_out("    FC port Mode, FCoE mode                                                  | port_mode                           : %#x\n",    pCfg.port_mode);
    cli_out("    Speed, AN/2/4/8/16/32/AN2/AN4/AN8/AN16/AN3                               | speed                               : %#x\n",    pCfg.speed);
    cli_out("    Transit B2B credits                                                      | tx_buffer_to_buffer_credits         : %#x\n",    pCfg.tx_buffer_to_buffer_credits);
    cli_out("    Receive B2B Credits (read only), computed based on max_frame_length      | rx_buffer_to_buffer_credits         : %#x\n",    pCfg.rx_buffer_to_buffer_credits);
    cli_out("    maximum FC frame length for ingress and egress (in unit of 32-bit word)  | max_frame_length                    : %#x\n",    pCfg.max_frame_length);
    cli_out("    bb credit recovery parameter                                             | bb_sc_n                             : %#x\n",    pCfg.bb_sc_n);
    cli_out("    Port state, INIT, Reset, Link Up, Link down, disabled (read only)        | port_state                          : %#x\n",    pCfg.port_state);
    cli_out("    Receive, transmit Timeout, default 100ms                                 | r_t_tov                             : %#x\n",    pCfg.r_t_tov);
    cli_out("    Interrupt enable                                                         | interrupt_enable                    : %#x\n",    pCfg.interrupt_enable);
    cli_out("    fillword on 8G active state                                              | fw_on_active_8g                     : %#x\n",    pCfg.fw_on_active_8g);
    cli_out("    Encap Source MAC construct mode                                          | src_mac_construct                   : %#x\n",    pCfg.src_mac_construct);
    cli_out("    Src MAC Addr for encapsulated frame                                      | src_mac_addr                        : %02x:%02x:%02x:%02x:%02x:%02x\n",
                                                                                                                                pCfg.src_mac_addr[0], pCfg.src_mac_addr[1], pCfg.src_mac_addr[2],
                                                                                                                                pCfg.src_mac_addr[3], pCfg.src_mac_addr[4], pCfg.src_mac_addr[5]);
    cli_out("    Source FCMAP prefix for the FPMA                                         | src_fcmap_prefix                    : %#x\n",    pCfg.src_fcmap_prefix);
    cli_out("    Encap Dest MAC construct mode                                            | dst_mac_construct                   : %#x\n",    pCfg.dst_mac_construct);
    cli_out("    Dest MAC Addr for encapsulated frame                                     | dst_mac_addr                        : %02x:%02x:%02x:%02x:%02x:%02x\n",
                                                                                                                                pCfg.dst_mac_addr[0], pCfg.dst_mac_addr[1], pCfg.dst_mac_addr[2],
                                                                                                                                pCfg.dst_mac_addr[3], pCfg.dst_mac_addr[4], pCfg.dst_mac_addr[5]);
    cli_out("    Destination FCMAP prefix for the FPMA                                    | dst_fcmap_prefix                    : %#x\n",    pCfg.dst_fcmap_prefix);
    cli_out("    default vlan for the mapper                                              | vlan_tag                            : %#x\n",    pCfg.vlan_tag);
    cli_out("    default VFT tag for the mapper                                           | vft_tag                             : %#x\n",    pCfg.vft_tag);
    cli_out("    Size of VLAN-VSAN Mapper table (read only)                               | mapper_len                          : %#x\n",    pCfg.mapper_len);
    cli_out("    Bypass Mapper                                                            | ingress_mapper_bypass               : %#x\n",    pCfg.ingress_mapper_bypass);
    cli_out("    Bypass Mapper                                                            | egress_mapper_bypass                : %#x\n",    pCfg.egress_mapper_bypass);
    cli_out("    VID or VFID                                                              | ingress_map_table_input             : %#x\n",    pCfg.ingress_map_table_input);
    cli_out("    VID or VFID                                                              | egress_map_table_input              : %#x\n",    pCfg.egress_map_table_input);
    cli_out("    Mapper FC-CRC handling                                                   | ingress_fc_crc_mode                 : %#x\n",    pCfg.ingress_fc_crc_mode);
    cli_out("    Mapper FC-CRC handling                                                   | egress_fc_crc_mode                  : %#x\n",    pCfg.egress_fc_crc_mode);
    cli_out("    VFT header processing mode                                               | ingress_vfthdr_proc_mode            : %#x\n",    pCfg.ingress_vfthdr_proc_mode);
    cli_out("    VFT header processing mode                                               | egress_vfthdr_proc_mode             : %#x\n",    pCfg.egress_vfthdr_proc_mode);
    cli_out("    VLAN header processing mode                                              | ingress_vlantag_proc_mode           : %#x\n",    pCfg.ingress_vlantag_proc_mode);
    cli_out("    VLAN header processing mode                                              | egress_vlantag_proc_mode            : %#x\n",    pCfg.egress_vlantag_proc_mode);
    cli_out("    source of VFID                                                           | ingress_vfid_mapsrc                 : %#x\n",    pCfg.ingress_vfid_mapsrc);
    cli_out("    source of VFID                                                           | egress_vfid_mapsrc                  : %#x\n",    pCfg.egress_vfid_mapsrc);
    cli_out("    source of VID                                                            | ingress_vid_mapsrc                  : %#x\n",    pCfg.ingress_vid_mapsrc);
    cli_out("    processing mode for priority field of vlan tag                           | ingress_vlan_pri_map_mode           : %#x\n",    pCfg.ingress_vlan_pri_map_mode);
    cli_out("    processing mode for priority field of vlan tag                           | egress_vlan_pri_map_mode            : %#x\n",    pCfg.egress_vlan_pri_map_mode);
    cli_out("    mode of VFT hopCnt check on ingress                                      | ingress_hopCnt_check_mode           : %#x\n",    pCfg.ingress_hopCnt_check_mode);
    cli_out("    enable VFT hopCnt decrement on egress                                    | egress_hopCnt_dec_enable            : %#x\n",    pCfg.egress_hopCnt_dec_enable);
    cli_out("    during 16G speed negotiation, 0 -- Use TTS, 1 -- Use PCS                 | use_tts_pcs_16G                     : %#x\n",    pCfg.use_tts_pcs_16G);
    cli_out("    during 32G speed negotiation, 0 -- Use TTS, 1 -- Use PCS                 | use_tts_pcs_32G                     : %#x\n",    pCfg.use_tts_pcs_32G);
    cli_out("    enable/disable link training for 16G                                     | training_enable_16G                 : %#x\n",    pCfg.training_enable_16G);
    cli_out("    enable/disable link training for 32G                                     | training_enable_32G                 : %#x\n",    pCfg.training_enable_32G);
    cli_out("    enable/disable FEC for 16G                                               | fec_enable_16G                      : %#x\n",    pCfg.fec_enable_16G);
    cli_out("    enable/disable FEC for 32G                                               | fec_enable_32G                      : %#x\n",    pCfg.fec_enable_32G);
    cli_out("    Enable FCS corruption on if FCoE pkt has EOFni or EOFa on ingress        | ingress_fcs_crrpt_eof_enable        : %#x\n",    pCfg.ingress_fcs_crrpt_eof_enable);
    cli_out("    VLAN header insert enable                                                | ingress_vlantag_presence_enable     : %#x\n",    pCfg.ingress_vlantag_presence_enable);
    cli_out("    mode of VFT hopCnt check on egress                                       | egress_hopcnt_check_mode            : %#x\n",    pCfg.egress_hopcnt_check_mode);
    cli_out("    enable VFT hopCnt decrement on ingress                                   | ingress_hopcnt_dec_enable           : %#x\n",    pCfg.ingress_hopcnt_dec_enable);
    cli_out("    enable passing control frame on egress when mapper is in bypass mode     | egress_pass_ctrl_frame_enable       : %#x\n",    pCfg.egress_pass_ctrl_frame_enable);
    cli_out("    enable passing pfc frame on egress when mapper is in bypass mode         | egress_pass_pfc_frame_enable        : %#x\n",    pCfg.egress_pass_pfc_frame_enable);
    cli_out("    enable passing pause frame on egress when mapper is in bypass mode       | egress_pass_pause_frame_enable      : %#x\n",    pCfg.egress_pass_pause_frame_enable);
    cli_out("    disable FCOE header version field check on egress                        | egress_fcoe_version_chk_disable     : %#x\n",    pCfg.egress_fcoe_version_chk_disable);
    cli_out("    specify default CoS value to use on egress                               | egress_default_cos_value            : %#x\n",    pCfg.egress_default_cos_value);
    cli_out("    use IP CoS map values on egress                                          | egress_use_ip_cos_map               : %#x\n",    pCfg.egress_use_ip_cos_map);
    cli_out("    bitmap to enable/disable scrambling for 2/4/8/16/32G FC speed            | scrambling_enable_mask              : %#x\n",    pCfg.scrambling_enable_mask);
    cli_out("    0 - disable, 1 - enable: bitmap value for corresponding speed            | scrambling_enable_value             : %#x\n",    pCfg.scrambling_enable_value);
    cli_out("    0 - disable, 1 - enable); legacy pause on egress                         | egress_pause_enable                 : %#x\n",    pCfg.egress_pause_enable);
    cli_out("    0 - disable, 1 - enable); PFC on egress                                  | egress_pfc_enable                   : %#x\n",    pCfg.egress_pfc_enable);
    cli_out("    stats collection interval in milliseconds                                | stat_interval                       : %#x\n",    pCfg.stat_interval);
    cli_out("    current COS value                                                        | cos_to_pri.cos                      : %#x\n",    pCfg.cos_to_pri.cos);
    cli_out("    current priority value                                                   | cos_to_pri.pri                      : %#x\n",    pCfg.cos_to_pri.pri);

    return CMD_OK;
}

#endif
