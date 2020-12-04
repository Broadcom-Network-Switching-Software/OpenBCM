/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Auth (802.1x) CLI commands
 */

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/dport.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/auth.h>
#include <bcm/debug.h>


cmd_result_t
cmd_esw_auth(int unit, args_t *a)
{
    char           *subcmd;
    parse_table_t  pt;
    cmd_result_t   retCode;
    int            ingress;
    uint32	   mode;
    int            l2ena, ilink;
    int            port, dport, r;
    bcm_pbmp_t     pbmp;
    bcm_port_config_t pcfg;
    sal_mac_addr_t mac;
    char           *mac_str;

    UNSUPPORTED_COMMAND(unit, SOC_CHIP_BCM5670, a);
    UNSUPPORTED_COMMAND(unit, SOC_CHIP_BCM5675, a);

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    ingress = 0;
    mode = 0;
    l2ena = 0;
    ilink = 0;
    sal_memset(mac, 0, sizeof(sal_mac_addr_t));
    BCM_PBMP_CLEAR(pbmp);

    if (sal_strcasecmp(subcmd, "mac") == 0) {
        if ((subcmd = ARG_GET(a)) == NULL)
            return CMD_USAGE;

        if (sal_strcasecmp(subcmd, "init") == 0) {
            mac_str = soc_property_get_str(unit, spn_STATION_MAC_ADDRESS);
            if (!mac_str) {
                cli_out("station_mac_address not set\n");
                return CMD_FAIL;
            }

            if (parse_macaddr(mac_str, mac) < 0 ) {
                      cli_out("ERROR: %s %s failed: No valid MAC address configured\n",
                              ARG_CMD(a), subcmd);
                      return CMD_FAIL;                 
            }
            r = bcm_auth_mac_add(unit, -1, mac);
            if (r < 0) {
                cli_out("ERROR: %s %s failed: %s\n",
                        ARG_CMD(a), subcmd, bcm_errmsg(r));
                return CMD_FAIL;
            }
            cli_out("All ports set to MAC address "
                    "%02x:%02x:%02x:%02x:%02x:%02x\n",
                    mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            return CMD_OK;
        }

        if (sal_strcasecmp(subcmd, "add") == 0) {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                            (void *)(0), &pbmp, NULL);
            parse_table_add(&pt, "Mac", PQ_DFL | PQ_MAC, 0, 
                            (void *)mac, 0);
            if (!parseEndOk(a, &pt, &retCode))
                return retCode;

            BCM_PBMP_AND(pbmp, pcfg.e);
            /* coverity[overrun-local] */
            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
                r = bcm_auth_mac_add(unit, port, mac);

                if (r < 0) {
                    cli_out("ERROR: %s %s port %s failed: %s\n",
                            ARG_CMD(a), subcmd, BCM_PORT_NAME(unit, port),
                            bcm_errmsg(r));
                    return CMD_FAIL;
                }
                cli_out("port %s MAC address "
                        "%02x:%02x:%02x:%02x:%02x:%02x set.\n",
                        SOC_PORT_NAME(unit, port),mac[0],mac[1],mac[2],
                        mac[3],mac[4],mac[5]);
            }
            return CMD_OK;
        }

        if (sal_strcasecmp(subcmd, "del") == 0) {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                            (void *)(0), &pbmp, NULL);
            parse_table_add(&pt, "Mac", PQ_DFL | PQ_MAC, 0,
                            (void *)mac, 0);
            if (!parseEndOk(a, &pt, &retCode))
                return retCode;

            BCM_PBMP_AND(pbmp, pcfg.e);
            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
                r = bcm_auth_mac_delete(unit, port, mac);

                if (r < 0) {
                    cli_out("ERROR: %s %s port %s failed: %s\n",
                            ARG_CMD(a), subcmd, BCM_PORT_NAME(unit, port),
                            bcm_errmsg(r));
                    return CMD_FAIL;
                }
                cli_out("port %s MAC address "
                        "%02x:%02x:%02x:%02x:%02x:%02x deleted.\n",
                        BCM_PORT_NAME(unit, port),mac[0],mac[1],mac[2],
                        mac[3],mac[4],mac[5]);
            }
            return CMD_OK;
        }

        if (sal_strcasecmp(subcmd, "clear") == 0) {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                            (void *)(0), &pbmp, NULL);
            if (!parseEndOk(a, &pt, &retCode))
                return retCode;

            BCM_PBMP_AND(pbmp, pcfg.e);
            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
                r = bcm_auth_mac_delete_all(unit, port);

                if (r < 0) {
                    cli_out("ERROR: %s %s port %s failed: %s\n",
                            ARG_CMD(a), subcmd, BCM_PORT_NAME(unit, port),
                            bcm_errmsg(r));
                    return CMD_FAIL;
                }
                cli_out("port %s all MAC addresses deleted.\n",
                        BCM_PORT_NAME(unit, port));
            }
            return CMD_OK;
        }
    }

    if (sal_strcasecmp(subcmd, "block") == 0) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                        (void *)(0), &pbmp, NULL);
        parse_table_add(&pt, "IngressOnly", PQ_BOOL|PQ_DFL,
                        0, &ingress, 0);
        if (!parseEndOk(a, &pt, &retCode))
            return retCode;

        if (ingress) {
            mode = BCM_AUTH_MODE_UNAUTH | BCM_AUTH_BLOCK_IN;
        }
        else {
            mode = BCM_AUTH_MODE_UNAUTH | BCM_AUTH_BLOCK_INOUT;
        }

        BCM_PBMP_AND(pbmp, pcfg.e);
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            r = bcm_auth_mode_set(unit, port, mode);

            if (r < 0) {
                cli_out("ERROR: %s %s port %s failed: %s\n",
                        ARG_CMD(a), subcmd, BCM_PORT_NAME(unit, port),
                        bcm_errmsg(r));
                return CMD_FAIL;
            }
            cli_out("port %s blocked in %s direction(s).\n",
                    BCM_PORT_NAME(unit, port), ingress ? "ingress" : "all");
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "unblock") == 0) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                        (void *)(0), &pbmp, NULL);
        if (!parseEndOk(a, &pt, &retCode))
            return retCode;

        mode = BCM_AUTH_MODE_UNCONTROLLED;

        BCM_PBMP_AND(pbmp, pcfg.e);
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            r = bcm_auth_mode_set(unit, port, mode);

            if (r < 0) {
                cli_out("ERROR: %s %s port %s failed: %s\n",
                        ARG_CMD(a), subcmd, BCM_PORT_NAME(unit, port),
                        bcm_errmsg(r));
                return CMD_FAIL;
            }
            cli_out("port %s unblock - all traffic allowed now.\n",
                    BCM_PORT_NAME(unit, port));
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "enable") == 0) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                        (void *)(0), &pbmp, NULL);
        parse_table_add(&pt, "LearnEnable", PQ_BOOL|PQ_DFL,
                        0, &l2ena, 0);
        parse_table_add(&pt, "IgnorLink", PQ_BOOL|PQ_DFL,
                        0, &ilink, 0);
        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }

        mode = BCM_AUTH_MODE_AUTH;
        if (l2ena) {
            mode |= BCM_AUTH_LEARN;
        }
        if (ilink) {
            mode |= BCM_AUTH_IGNORE_LINK;
        }

        BCM_PBMP_AND(pbmp, pcfg.e);
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            r = bcm_auth_mode_set(unit, port, mode);

            if (r < 0) {
                cli_out("ERROR: %s %s port %s failed: %s\n",
                        ARG_CMD(a), subcmd, BCM_PORT_NAME(unit, port),
                        bcm_errmsg(r));
                return CMD_FAIL;
            }
            cli_out("port %s authorization enabled.\n",
                    BCM_PORT_NAME(unit, port));
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "disable") == 0) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                        (void *)(0), &pbmp, NULL);
        if (!parseEndOk( a, &pt, &retCode))
            return retCode;

        mode = BCM_AUTH_MODE_UNAUTH | BCM_AUTH_BLOCK_INOUT;

        BCM_PBMP_AND(pbmp, pcfg.e);
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            r = bcm_auth_mode_set(unit, port, mode);

            if (r < 0) {
                cli_out("ERROR: %s %s port %s failed: %s\n",
                        ARG_CMD(a), subcmd, BCM_PORT_NAME(unit, port),
                        bcm_errmsg(r));
                return CMD_FAIL;
            }
            cli_out("port %s authorization disable - no traffic allowed.\n",
                    BCM_PORT_NAME(unit, port));
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "init") == 0) {
        if ((r = bcm_auth_init(unit)) < 0) {
            cli_out("%s: error initializing: %s\n", ARG_CMD(a), bcm_errmsg(r));
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "detach") == 0) {
        r = bcm_auth_detach(unit);
        if (r < 0) {
            cli_out("ERROR: %s %s failed: %s\n",
                    ARG_CMD(a), subcmd, bcm_errmsg(r));
            return CMD_FAIL;
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "show") == 0) {
        /* coverity[overrun-local] */
        DPORT_BCM_PBMP_ITER(unit, pcfg.e, dport, port) {
            r = bcm_auth_mode_get(unit, port, &mode);

            if (r < 0) {
                cli_out("ERROR: %s %s port %s failed: %s\n",
                        ARG_CMD(a), subcmd, BCM_PORT_NAME(unit, port),
                        bcm_errmsg(r));
                return CMD_FAIL;
            }
            if (mode & BCM_AUTH_MODE_UNCONTROLLED) {
                cli_out("port %s in uncontrolled state.\n",
                        BCM_PORT_NAME(unit, port));
            } else if (mode & BCM_AUTH_MODE_UNAUTH) {
                cli_out("port %s is unauthorized for %s direction(s).\n",
                        BCM_PORT_NAME(unit, port),
                        (mode & BCM_AUTH_BLOCK_IN) ? "ingress" : "all");
            } else {
                if (!(mode & (BCM_AUTH_LEARN|BCM_AUTH_IGNORE_LINK
                            |BCM_AUTH_IGNORE_VIOLATION))) {
                    cli_out("port %s is authorized.\n",
                            BCM_PORT_NAME(unit, port));
                } else {
                   cli_out("port %s is authorized with condition(s) %s%s%s\n",
                           BCM_PORT_NAME(unit, port),
                           (mode & BCM_AUTH_LEARN) ?
                           "L2LEARN " : "",
                           (mode & BCM_AUTH_IGNORE_LINK) ?
                           "IGNORE_LINK " : "",
                           (mode & BCM_AUTH_IGNORE_VIOLATION) ?
                           "IGNORE_VIOLATION " : "");
                }
            }
        }
        return CMD_OK;
    }

    return CMD_USAGE;
}
