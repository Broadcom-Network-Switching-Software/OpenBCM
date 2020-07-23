/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Chassis support
 */


#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <shared/bsl.h>
#if defined(MBZ)
extern int sysIsLM();
/* Chassis failover related stuff */

#include <soc/cm.h>
#include <soc/mem.h>
#include <soc/debug.h>

#include <bcm/link.h>

#define CFM1_PORT_LM   6
#define CFM2_PORT_LM   3
#define DRACO_UNIT1_PORT 8
#define DRACO_UNIT2_PORT 1
#define CFM1_PORT_CX4   5
#define CFM2_PORT_CX4  4
#define LYNX_UNIT1_PORT 6
#define LYNX_UNIT6_PORT 3
#define CFM1_PORT   ((cx4_flag) ? CFM1_PORT_CX4 : CFM1_PORT_LM)
#define CFM2_PORT   ((cx4_flag) ? CFM2_PORT_CX4 : CFM2_PORT_LM)
int cfm_hattached = 0;
int cx4_flag = 0;     


void
bcm_cx4_fabric_setup(int unit, bcm_port_t port)
{
    if (port == CFM1_PORT_CX4) {
        /* CFM in slot 1 */
        LOG_ERROR(BSL_LS_APPL_CHASSIS,
                  (BSL_META_U(unit,
                              "bcm_cx4_fabric_setup: port=%d CFM slot-1\n"),
                   port));
        /* soc_icm_egress_mask_set(int unit, int port, pbmp_t pbmp)*/
        WRITE_ING_EGRMSKBMAPr(unit, LYNX_UNIT6_PORT, 0x020);
        WRITE_ING_EGRMSKBMAPr(unit, LYNX_UNIT1_PORT, 0x020);
        WRITE_ING_EGRMSKBMAPr(unit, CFM2_PORT_CX4, 0x048);
        WRITE_ING_EGRMSKBMAPr(unit, CFM1_PORT_CX4, 0x048);
    } else if (port == CFM2_PORT_CX4) {
        /* CFM in slot 2 */
        LOG_ERROR(BSL_LS_APPL_CHASSIS,
                  (BSL_META_U(unit,
                              "bcm_cx4_fabric_setup: unit=%d port=%d CFM slot-2\n"),
                   unit, port));
        WRITE_ING_EGRMSKBMAPr(unit, LYNX_UNIT6_PORT, 0x010);
        WRITE_ING_EGRMSKBMAPr(unit, LYNX_UNIT1_PORT, 0x010);
        WRITE_ING_EGRMSKBMAPr(unit, CFM2_PORT_CX4, 0x048);
        WRITE_ING_EGRMSKBMAPr(unit, CFM1_PORT_CX4, 0x048);
    } else {
        /* Both CFMs active */
        LOG_ERROR(BSL_LS_APPL_CHASSIS,
                  (BSL_META_U(unit,
                              "bcm_cx4_fabric_setup: unit=%d port=%d CFM slot-1&2\n"),
                   unit, port));
        WRITE_ING_EGRMSKBMAPr(unit, LYNX_UNIT6_PORT, 0x010);
        WRITE_ING_EGRMSKBMAPr(unit, LYNX_UNIT1_PORT, 0x020);
        WRITE_ING_EGRMSKBMAPr(unit, CFM2_PORT_CX4, 0x008);
        WRITE_ING_EGRMSKBMAPr(unit, CFM1_PORT_CX4, 0x040);
    }
}

void
bcm_fabric_setup(int unit, bcm_port_t port)
{
    if (port == CFM1_PORT_LM) {
        /* CFM in slot 1 */
        LOG_ERROR(BSL_LS_APPL_CHASSIS,
                  (BSL_META_U(unit,
                              "bcm_fabric_setup: unit=%d port=%d CFM slot-1\n"),
                              unit, port));
        /* soc_icm_egress_mask_set(int unit, int port, pbmp_t pbmp)*/
        WRITE_ING_EGRMSKBMAPr(unit, DRACO_UNIT2_PORT, 0x142);
        WRITE_ING_EGRMSKBMAPr(unit, DRACO_UNIT1_PORT, 0x142);
    } else if (port == CFM2_PORT_LM) {
        /* CFM in slot 2 */
        LOG_ERROR(BSL_LS_APPL_CHASSIS,
                  (BSL_META_U(unit,
                              "bcm_fabric_setup: unit=%d port=%d CFM slot-2\n"),
                              unit, port));
        WRITE_ING_EGRMSKBMAPr(unit, DRACO_UNIT2_PORT, 0x10a);
        WRITE_ING_EGRMSKBMAPr(unit, DRACO_UNIT1_PORT, 0x10a);
    } else {
        /* Both CFMs active */
        LOG_ERROR(BSL_LS_APPL_CHASSIS,
                  (BSL_META_U(unit,
                              "bcm_fabric_setup: unit=%d port=%d CFM slot-1&2\n"),
                              unit, port));
        WRITE_ING_EGRMSKBMAPr(unit, DRACO_UNIT2_PORT, 0x10a);
        WRITE_ING_EGRMSKBMAPr(unit, DRACO_UNIT1_PORT, 0x142);
    }
}

void
bcm_cfm_fail_over(int unit, bcm_port_t port, bcm_port_info_t *info)
{
    int link_status;
    bcm_port_t cfm_port;
    char *ll_status[] = {"down", "up", "failed", "fault"};

    cfm_port = port;

    LOG_ERROR(BSL_LS_APPL_CHASSIS,
              (BSL_META_U(unit,
                          "bcm_cfm_fail_over: unit = %d port = %d Link = %s\n"),
                          unit, port,
                          ((info->linkstatus >= BCM_PORT_LINK_STATUS_DOWN &&
                            info->linkstatus <= BCM_PORT_LINK_STATUS_REMOTE_FAULT) ? 
                           ll_status[info->linkstatus] : "Invalid")));
    if ((port != CFM1_PORT) && (port != CFM2_PORT))  {
        return;
    }

    if (info->linkstatus == BCM_PORT_LINK_STATUS_UP) {
        /* Link Up */
        if (bcm_port_link_status_get(unit, (port == CFM1_PORT) ?  CFM2_PORT :
                                CFM1_PORT, &link_status) >= 0) {
            if (link_status) {
                cfm_port = -1; /* Both CFMs active */
            }
        }
    } else {
        /* Link down */
        cfm_port = (port == CFM1_PORT) ? CFM2_PORT : CFM1_PORT;
    }

    if (cx4_flag) {
        bcm_cx4_fabric_setup(unit, cfm_port);
    } else {
        bcm_fabric_setup(unit, cfm_port);
    }
}


void bcm_cfm_failover_attach(int unit)
{
    int link_status;
    bcm_port_t cfm_port;

    if (unit != 0) return;

    cfm_port = CFM1_PORT;

    if (sysIsLM()) {
        if (bcm_port_link_status_get(unit, CFM1_PORT, &link_status) >= 0) {
            if (link_status) {
                cfm_port = CFM1_PORT;
                if (bcm_port_link_status_get(
                        unit, CFM2_PORT, &link_status) >= 0) {
                    if (link_status) {
                        cfm_port = -1; /* Both CFMs active */
                    }
                }
            }
            else {
                cfm_port = CFM2_PORT;
            }
        }

        if (cx4_flag) {
            bcm_cx4_fabric_setup(unit, cfm_port);
        } else {
            bcm_fabric_setup(unit, cfm_port);
        }

        LOG_ERROR(BSL_LS_APPL_CHASSIS,
                  (BSL_META_U(unit,
                              "bcm_cfm_failover_attach: unit = %d %s\n"),
                              unit, "Registered Handler for CFM failover")); 
        bcm_linkscan_register(unit, bcm_cfm_fail_over);
    }
    cfm_hattached = 1;
}

void bcm_cfm_failover_detach(int unit)
{
    bcm_linkscan_unregister(unit, bcm_cfm_fail_over);
    cfm_hattached = 0;
}

/*
 * Function:
 *	if_cfm_failover
 * Purpose:
 *	Activate/Deactivate CFM failover
 * Parameters:
 *	unit - SOC unit #
 *	a - pointer to args
 * Returns:
 *	CMD_OK/CMD_FAIL
 */

char if_cfm_failover_usage[] =
    "Activate CFM failover.\n";

cmd_result_t
if_cfm_failover(int unit, args_t *a)
{
    char           *c;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if ((c = ARG_GET(a)) != NULL) {
        if (!sal_strcmp(c, "cx4")) {
            cx4_flag = 1; 
            c = ARG_GET(a);
        }
    }

    if (c  != NULL) {
        if (!sal_strcmp(c, "enable")) {
            if (cfm_hattached) bcm_cfm_failover_detach(unit);
            bcm_cfm_failover_attach(unit);
        } else if (!sal_strcmp(c, "disable")) {
            bcm_cfm_failover_detach(unit);
        } else {
            cli_out("cfmfailover [enable/disable] \n");
        }
    } else {
            cli_out("cfmfailover [enable/disable] \n");
    }

    return CMD_OK;
}

#endif

typedef int _bcm_diag_chassis_not_empty; /* Make ISO compilers happy. */
