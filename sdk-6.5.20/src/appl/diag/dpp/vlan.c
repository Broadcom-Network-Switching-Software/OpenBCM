/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * VLAN CLI commands
 */



#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/dport.h>

#include <bcm/error.h>
#include <bcm/vlan.h>
#include <bcm/l3.h>
#include <bcm/debug.h>


char cmd_dpp_vlan_usage[] =
    "Usages:\n\t"
    "  vlan create <id> [PortBitMap=<pbmp> UntagBitMap=<pbmp>]\n\t"
    "                                       - Create a VLAN\n\t"
    "  vlan destroy <id>                    - Destroy a VLAN\n\t"
    "  vlan clear                           - Destroy all VLANs\n\t"
    "  vlan add <id> [PortBitMap=<pbmp> UntagBitMap=<pbmp>\n\t"
    "                                       - Add port(s) to a VLAN\n\t"
    "  vlan remove <id> [PortBitMap=<pbmp>] - Remove ports from a VLAN\n\t"
    "  vlan gport add <id> [GportPortID=<port_id>\n\t"
    "                                       - Add gport to a VLAN\n\t"
    "  vlan gport delete <id> [GportPortID=<port_id>\n\t"
    "                                       - Delete gport from a VLAN\n\t"
    "  vlan gport get <id> [GportPortID=<port_id>\n\t"
    "                                       - Check if gport belongs to a VLAN\n\t"
    "  vlan gport clear <id>\n\t"
    "                                       - Delete all ports from a VLAN\n\t"
    "  vlan gport show <id>\n\t"
    "                                       - Show all ports in a VLAN\n\t"
    "  vlan show                            - Display all VLANs\n\t"
    "  vlan default [<id>]                  - Show or set the default VLAN\n\t"
    "  vlan mac add MACaddress=<address> VLan=<vlanid> Prio=<prio>\n\t"
    "        Cng=<cng>\n\t"
    "  vlan mac delete MACaddress=<address>\n\t"
    "  vlan mac clear\n"
    ;

cmd_result_t
cmd_dpp_vlan(int unit, args_t *a)
{
    char                *subcmd, *c;
    int                 is_untagged = 0;
    int                 r = 0;
    vlan_id_t           id = VLAN_ID_INVALID;
    bcm_port_config_t   pcfg;
    bcm_pbmp_t          arg_ubmp;
    bcm_pbmp_t          arg_pbmp;
    parse_table_t       pt;
    cmd_result_t        retCode;
    bcm_gport_t         gport = BCM_GPORT_INVALID;
    char                *bcm_vlan_mcast_flood_str[] = BCM_VLAN_MCAST_FLOOD_STR;
    uint8               is_allocated = FALSE;

    BCM_PBMP_CLEAR(arg_ubmp);
    BCM_PBMP_CLEAR(arg_pbmp);

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if (sw_state_access[unit].dpp.bcm.vlan.is_allocated(unit, &is_allocated) != 0) {
        cli_out("%s: ERROR: sw_state_access[unit].dpp.bcm.vlan.is_allocated failed\n",
                ARG_CMD(a));
        return CMD_FAIL;
    }
    if(is_allocated == FALSE) {
        cli_out("VLAN module is not initialized.\n");
        return CMD_NOTIMPL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (sal_strcasecmp(subcmd, "create") == 0) {
        if ((c = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }

        id = parse_integer(c);

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "PortBitMap",      PQ_DFL|PQ_PBMP|PQ_BCM,
                        (void *)(0), &arg_pbmp, NULL);
        parse_table_add(&pt, "UntagBitMap",     PQ_DFL|PQ_PBMP|PQ_BCM,
                        (void *)(0), &arg_ubmp, NULL);

        if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
            cli_out("%s: ERROR: Unknown option: %s\n",
                    ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
        parse_arg_eq_done(&pt);

        if ((r = bcm_vlan_create(unit, id)) < 0) {
            goto bcm_err;
        }

        if ((r = bcm_vlan_port_add(unit, id, arg_pbmp, arg_ubmp)) < 0) {
            goto bcm_err;
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "destroy") == 0) {
        if ((c = ARG_GET(a)) == NULL)
            return CMD_USAGE;

        id = parse_integer(c);

        if ((r = bcm_vlan_destroy(unit, id)) < 0)
            goto bcm_err;

        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "clear") == 0) {
        if ((r = bcm_vlan_destroy_all(unit)) < 0) {
            goto bcm_err;
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "add") == 0 ||
        sal_strcasecmp(subcmd, "remove") == 0) {
        if ((c = ARG_GET(a)) == NULL) {
            return CMD_USAGE;
        }

        id = parse_integer(c);

        parse_table_init(unit, &pt);
        parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                        (void *)(0), &arg_pbmp, NULL);
        if (sal_strcasecmp(subcmd, "add") == 0) {
            parse_table_add(&pt, "UntagBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                            (void *)(0), &arg_ubmp, NULL);
        }

        if (!parseEndOk( a, &pt, &retCode)) {   
            return retCode;
        }

        if (sal_strcasecmp(subcmd, "remove") == 0) {
            if ((r = bcm_vlan_port_remove(unit, id, arg_pbmp)) < 0) {
                goto bcm_err;
            }
        } else {
            if ((r = bcm_vlan_port_add(unit, id, arg_pbmp, arg_ubmp)) < 0) {
                goto bcm_err;
            }
        }

        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "gport") == 0) {
        subcmd = ARG_GET(a);
        if (subcmd == NULL) {
            cli_out("%s: ERROR: Missing gport subcommand\n", ARG_CMD(a));
            return CMD_FAIL;
        }
        /* Check against all valid "vlan gport" subcommands. */
        if ((sal_strcasecmp(subcmd, "delete") != 0) &&
            (sal_strcasecmp(subcmd, "add") != 0) &&
            (sal_strcasecmp(subcmd, "get") != 0) &&
            (sal_strcasecmp(subcmd, "clear") != 0) &&
            (sal_strcasecmp(subcmd, "show") != 0)) {
            cli_out("%s: ERROR: Invalid gport subcommand\n", ARG_CMD(a));
            return CMD_FAIL;
        }
    
        /* Fetch next argument, the VLAN id */
        if ((c = ARG_GET(a)) == NULL) { 
            return CMD_USAGE;
        }

        /* "id" gets the vlan ID */
        id = parse_integer(c);
    
        if ((sal_strcasecmp(subcmd, "clear") != 0) &&
            (sal_strcasecmp(subcmd, "show") != 0)) {
            /* "clear" and "show" do not need a gport parameter, the others do */
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "PortID", PQ_DFL | PQ_PORT,
                            (void *) (0), &gport, NULL);

            if (!parseEndOk(a, &pt, &retCode)) {
                return retCode;
            }
        }

        if (sal_strcasecmp(subcmd, "delete") == 0) {
            if ((r = bcm_vlan_gport_delete(unit, id, gport)) < 0) {
                goto bcm_err;
            }
        } else if (sal_strcasecmp(subcmd, "add") == 0) {
            if ((r = bcm_vlan_gport_add(unit, id, gport, 0)) < 0) {
                goto bcm_err;
            }
        } else if (sal_strcasecmp(subcmd, "get") == 0) {
            if ((r = bcm_vlan_gport_get(unit, id, gport, &is_untagged)) < 0) {
                goto bcm_err;
            }
        } else if (sal_strcasecmp(subcmd, "clear") == 0) {
            if ((r = bcm_vlan_gport_delete_all(unit, id)) < 0) {
                goto bcm_err;
            }
        } else if (sal_strcasecmp(subcmd, "show") == 0) {
            const int max = BCM_PBMP_PORT_MAX;
            int i, count;
            bcm_gport_t *port_array = NULL;
            int *is_untagged_array = NULL;

            port_array = sal_alloc(sizeof(bcm_gport_t)*BCM_PBMP_PORT_MAX, "cmd_dpp_vlan.port_array");
            if(port_array == NULL) {
                cli_out("Memory allocation failure\n");
                return CMD_FAIL;
            }
            is_untagged_array = sal_alloc(sizeof(int)*BCM_PBMP_PORT_MAX, "cmd_dpp_vlan.port_array");
            if(is_untagged_array == NULL) {
                cli_out("Memory allocation failure\n");
                sal_free(port_array);
                return CMD_FAIL;
            }
            /* Get all virtual and physical ports from the specified VLAN */
            if ((r = bcm_vlan_gport_get_all(unit, id, max, port_array,
                                            is_untagged_array, &count)) < 0) {
                sal_free(port_array);
                sal_free(is_untagged_array);
                goto bcm_err;
            }
            /*
             * Count will always be non-zero (BCM_E_NOT_FOUND returned when
             * VLAN is valid but has no associated ports.
             */
            cli_out("Virtual and physical ports defined for VLAN: %d\n", id);
            for (i = 0; i < count; i++) {
                /* port name is empty string if port is not local */
                const char *port_name = mod_port_name(unit, -1, port_array[i]);

                if (*port_name) {
                    cli_out("  %4s (%stagged)\n", port_name,
                            is_untagged_array[i] ? "un" : "");
                }
            }
            sal_free(port_array);
            sal_free(is_untagged_array);
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "show") == 0) {
        bcm_vlan_data_t *list;
        int             count, i;
        char            bufp[FORMAT_PBMP_MAX], bufu[FORMAT_PBMP_MAX];
        char            pfmtp[SOC_PBMP_FMT_LEN];
        char            pfmtu[SOC_PBMP_FMT_LEN];

        if ((c = ARG_GET(a)) != NULL) {
            id = parse_integer(c);
        }

        if ((r = bcm_vlan_list(unit, &list, &count)) < 0) {
            goto bcm_err;
        }

        for (i = 0; i < count; i++) {
            if (id == VLAN_ID_INVALID || list[i].vlan_tag == id) {
                bcm_vlan_mcast_flood_t  mode = BCM_VLAN_MCAST_FLOOD_COUNT;

                format_bcm_pbmp(unit, bufp, sizeof(bufp), list[i].port_bitmap);
                format_bcm_pbmp(unit, bufu, sizeof(bufu), list[i].ut_port_bitmap);
                cli_out("vlan %d\tports %s (%s), untagged %s (%s) %s\n",
                        list[i].vlan_tag,
                        bufp, SOC_PBMP_FMT(list[i].port_bitmap, pfmtp),
                        bufu, SOC_PBMP_FMT(list[i].ut_port_bitmap, pfmtu),
                        bcm_vlan_mcast_flood_str[mode]
                        );
            }
        }

        bcm_vlan_list_destroy(unit, list, count);

        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "default") == 0) {
        if ((c = ARG_GET(a)) != NULL) {
            id = parse_integer(c);
        }

        if (id == VLAN_ID_INVALID) {
            if ((r = bcm_vlan_default_get(unit, &id)) < 0) {
                goto bcm_err;
            }

            cli_out("Default VLAN ID is %d\n", id);
        } else {
            if ((r = bcm_vlan_default_set(unit, id)) < 0) {
                goto bcm_err;
            }

            cli_out("Default VLAN ID set to %d\n", id);
        }

        return CMD_OK;
    }
    return CMD_USAGE;

 bcm_err:

    cli_out("%s: ERROR: %s\n", ARG_CMD(a), bcm_errmsg(r));

    return CMD_FAIL;
}
