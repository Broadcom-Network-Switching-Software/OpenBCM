/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Stacking CLI commands
 */

#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/dport.h>
#include <shared/bsl.h>
#include <bcm/vlan.h>
#include <bcm/stack.h>
#include <bcm/error.h>

STATIC void
_report_flags(int unit, int port, uint32 flags) {
    if (port > 0) {
        cli_out("Stack flags for unit %d, port %d: (0x%x)\n", unit,
                port, flags);
    } else {
        cli_out("Stack flags for unit %d: (0x%x)\n", unit, flags);
    }
    if (flags & (BCM_STK_CAPABLE | BCM_STK_ENABLE | BCM_STK_INACTIVE |
                 BCM_STK_SIMPLEX | BCM_STK_DUPLEX)) {
        cli_out("    %s%s%s%s%s\n",
                flags & BCM_STK_CAPABLE  ? "capable " : "",
                flags & BCM_STK_ENABLE   ? "enable " : "",
                flags & BCM_STK_INACTIVE ? "inactive " : "",
                flags & BCM_STK_SIMPLEX  ? "simplex " : "",
                flags & BCM_STK_DUPLEX   ? "duplex " : "");
    }
    if (flags & (BCM_STK_NONE | BCM_STK_SL | BCM_STK_HG |
                 BCM_STK_INTERNAL | BCM_STK_NO_LINK)) {
        cli_out("    %s%s%s%s%s\n",
                flags & BCM_STK_NONE  ? "none " : "",
                flags & BCM_STK_SL   ? "sl " : "",
                flags & BCM_STK_HG ? "hg " : "",
                flags & BCM_STK_INTERNAL ? "internal " : "",
                flags & BCM_STK_NO_LINK ? "no_link " : "");
    }
}

char cmd_stk_mode_usage[] =
    "\nSTACKMode [Unit=<n>] [SL=<T|F>] [HG=<T|F>] [NONE=<T|F>]\n"
    "    Set/get stack mode.  Without SL/HG/NONE, gets the current mode.\n"
    "    With SL or HG or NONE, sets the current mode.\n"
    "    Note:  Only SL=T is really supported currently\n";

cmd_result_t
cmd_stk_mode(int unit, args_t *args)
{
    parse_table_t	pt;
    int rv;
    int stk_unit = unit;
    int none = -1,
        sl = -1,
        hg = -1;
    uint32 flags = 0;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Unit", PQ_DFL|PQ_INT,
		    INT_TO_PTR(unit), &stk_unit, 0);
    parse_table_add(&pt, "SL", PQ_DFL|PQ_BOOL, 0, &sl, 0);
    parse_table_add(&pt, "HG", PQ_DFL|PQ_BOOL, 0, &hg, 0);
    parse_table_add(&pt, "NONE", PQ_DFL|PQ_BOOL, 0, &none, 0);
    if (parse_arg_eq(args, &pt) < 0) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    parse_arg_eq_done(&pt);
    if ((none < 0) && (sl < 0) && (hg < 0)) { /* Get */
        rv = bcm_stk_mode_get(stk_unit, &flags);
        if (rv < 0) {
            cli_out("ERROR: bcm_stk_mode_get returns %s\n", bcm_errmsg(rv));
            return CMD_FAIL;
        }
        _report_flags(stk_unit, -1, flags);
    } else {
        flags |= (none > 0) ? BCM_STK_NONE : 0;
        flags |= (sl > 0) ? BCM_STK_SL : 0;
        flags |= (hg > 0) ? BCM_STK_HG : 0;

        rv = bcm_stk_mode_set(stk_unit, flags);
        if (rv < 0) {
            cli_out("ERROR: bcm_stk_mode_set returns: %s\n", bcm_errmsg(rv));
            cli_out("   Unit %d. Flags: 0x%x\n", stk_unit, flags);
            return CMD_FAIL;
        }
    }

    return CMD_OK;
}

/* Expose bcm_stk_port_set/get */

char cmd_stk_port_set_usage[] =
    "\nStackPortSet Port=<n> [Unit=<n>] [Capable=<T|F>] [Enable=<T|F>]\n"
    "    [Inactive=<T|F>] [Simplex=<T|F>] [Duplex=<T|F>] [Internal=<T|F>]\n"
    "    Add a stack port to the system with the indicated flags\n";

cmd_result_t
cmd_stk_port_set(int unit, args_t *args)
{
    parse_table_t	pt;
    int rv;
    int stk_unit = unit;
    int stk_port = -1; /* This is intentionally a port */
    int capable = 0,
        enable = 1,
        inactive = 0,
        simplex = 0,
        internal = 0,
        duplex = 0;
    uint32 flags = 0;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Unit", PQ_DFL|PQ_INT,
		    INT_TO_PTR(unit), &stk_unit, 0);
    parse_table_add(&pt, "Port", PQ_DFL|PQ_PORT, 0, &stk_port, 0);
    parse_table_add(&pt, "Capable", PQ_DFL|PQ_BOOL, 0, &capable, 0);
    parse_table_add(&pt, "Enable", PQ_DFL|PQ_BOOL, INT_TO_PTR(1), &enable, 0);
    parse_table_add(&pt, "Inactive", PQ_DFL|PQ_BOOL, 0, &inactive, 0);
    parse_table_add(&pt, "Simplex", PQ_DFL|PQ_BOOL, 0, &simplex, 0);
    parse_table_add(&pt, "Duplex", PQ_DFL|PQ_BOOL, 0, &duplex, 0);
    parse_table_add(&pt, "Internal", PQ_DFL|PQ_BOOL, 0, &internal, 0);
    if (parse_arg_eq(args, &pt) < 0) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    parse_arg_eq_done(&pt);
    if (stk_port < 0) {
        cli_out("ERROR: Must specify port number > 0\n");
        return CMD_USAGE;
    }

    flags |= capable ?   BCM_STK_CAPABLE   : 0;
    flags |= enable ?    BCM_STK_ENABLE    : 0;
    flags |= inactive ?  BCM_STK_INACTIVE  : 0;
    flags |= simplex ?   BCM_STK_SIMPLEX   : 0;
    flags |= duplex ?    BCM_STK_DUPLEX    : 0;
    flags |= internal ?  BCM_STK_INTERNAL  : 0;

    rv = bcm_stk_port_set(stk_unit, stk_port, flags);
    if (rv < 0) {
        cli_out("ERROR: stk_port_set unit %d port %d flags 0x%x: %s\n",
                stk_unit, stk_port, flags, bcm_errmsg(rv));
        return CMD_FAIL;
    }

    return CMD_OK;
}

char cmd_stk_port_get_usage[] =
    "\nStackPortGet [Port=<n>] [Unit=<n>]\n"
    "    Get stack port information.  If no port is specified, get all\n"
    "    information for the current or specified device\n"
    "    If unit < 0 is specified, get all info for all local devices.\n";

cmd_result_t
cmd_stk_port_get(int unit, args_t *args)
{
    parse_table_t	pt;
    int rv;
    int stk_unit = unit;
    int stk_port = -1; /* This is intentionally a port */
    int dport;
    uint32 flags = 0;
    bcm_pbmp_t stk_pbmp, inactive_pbmp;
    int i, start_unit, end_unit;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Unit", PQ_DFL|PQ_INT,
		    INT_TO_PTR(unit), &stk_unit, 0);
    parse_table_add(&pt, "Port", PQ_DFL|PQ_PORT, 0, &stk_port, 0);
    if (parse_arg_eq(args, &pt) < 0) {
        parse_arg_eq_done(&pt);
        return CMD_USAGE;
    }
    parse_arg_eq_done(&pt);

    if (stk_port >= 0) {
        if (stk_unit < 0) {
            cli_out("ERROR: Can't specify port with unit < 0\n");
            return CMD_FAIL;
        }

        rv = bcm_stk_port_get(stk_unit, stk_port, &flags);
        if (rv < 0) {
            cli_out("ERROR: stk_port_get unit %d port %d flags 0x%x: %s\n",
                    stk_unit, stk_port, flags, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        _report_flags(stk_unit, stk_port, flags);
        return CMD_OK;
    }

    if (unit >= 0) {
        start_unit = unit;
        end_unit = unit + 1;
    } else {
        start_unit = 0;
        end_unit = soc_ndev;
    }

    for (i = start_unit; i < end_unit; i++) {
        rv = bcm_stk_pbmp_get(i, &stk_pbmp, &inactive_pbmp);
        if (rv < 0) {
            cli_out("ERROR: stk_pbmp_get unit %d: %s\n", i, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        /* coverity[overrun-local] */
        DPORT_BCM_PBMP_ITER(unit, stk_pbmp, dport, stk_port) {
            rv = bcm_stk_port_get(i, stk_port, &flags);
            if (rv < 0) {
                cli_out("ERROR: stk_port_get unit %d port %d flags 0x%x: %s\n",
                        i, stk_port, flags, bcm_errmsg(rv));
                return CMD_FAIL;
            }
            _report_flags(i, stk_port, flags);
        }
    }

    return CMD_OK;
}

#ifdef BCM_XGS_SUPPORT
char modmap_cmd_usage[] =
    "Usages:\n\t"
#ifdef COMPILER_STRING_CONST_LIMIT
    "modmap <option> [args...]\n"
#else
    "modmap enable [PortBitMap = <pbmp>]\n\t"
    "       - Enable MODID remapping\n\t"
    "modmap disable [PortBitMap = <pbmp>]\n\t"
    "       - Disable MODID remapping\n\t"
    "modmap lmod set [Port = <val>] [FMod = <val>] [LMod = <val>]\n\t"
    "       - Set mapping between fabric module id and local module id\n\t"
    "modmap lmod get [Port = <val>]\n\t"
    "       - Get mapping between fabric module id and local module id\n\t"
    "modmap smod set [PortBitMap = <pbmp>] [FMod = <val>] [SMod = <val>]\n\t"
    "                [SPort = <val>] [NPorts = <val>]\n\t"
    "       - Set mapping between fabric module id and switch module id\n\t"
    "modmap smod get [PortBitMap = <pbmp>] [FMod = <val>]\n\t"
    "       - Get mapping between fabric module id and switch module id\n\t"
    "modmap group set [PortBitMap = <pbmp>] [Group = <val>]\n\t"
    "       - Set the group membership of ports\n\t"
    "modmap group show\n\t"
    "       - Display the group membership of all ports\n\t"
    "modmap show\n\t"
    "       - Show MODID mapping states\n"
#endif
    ;

cmd_result_t
cmd_modmap(int unit, args_t *a)
{
    char *subcmd;
    parse_table_t  pt;
    cmd_result_t   retCode;
    int            r, mode;
    soc_port_t     port, dport;
    bcm_pbmp_t pbmp;
    bcm_port_config_t pcfg;
    int     arg_fmod = 0, arg_lmod = 0, arg_port = 0;
    int     arg_smod = 0, arg_sport = 0, arg_group = 0;
    uint32  arg_nports = 0;
    int            fmod, lmod, smod, sport, group;
    uint32	   nports;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    BCM_PBMP_CLEAR(pbmp);

    if (sal_strcasecmp(subcmd, "enable") == 0) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                        (void *)(0), &pbmp, NULL);
        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }

        BCM_PBMP_AND(pbmp, pcfg.stack_ext);
        /* coverity[overrun-local] */
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            r = bcm_stk_modmap_enable_set(unit, port, 1);

            if (r < 0) {
                cli_out("ERROR: %s %s port %s failed: %s\n",
                        ARG_CMD(a), subcmd, BCM_PORT_NAME(unit, port),
                        bcm_errmsg(r));
                return CMD_FAIL;
            }
            cli_out("port %s modid mapping enabled.\n",
                    BCM_PORT_NAME(unit, port));
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "disable") == 0) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                        (void *)(0), &pbmp, NULL);
        if (!parseEndOk(a, &pt, &retCode)) {
            return retCode;
        }

        BCM_PBMP_AND(pbmp, pcfg.stack_ext);
        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
            r = bcm_stk_modmap_enable_set(unit, port, 0);

            if (r < 0) {
                cli_out("ERROR: %s %s port %s failed: %s\n",
                        ARG_CMD(a), subcmd, BCM_PORT_NAME(unit, port),
                        bcm_errmsg(r));
                return CMD_FAIL;
            }
            cli_out("port %s modid mapping disabled.\n",
                    BCM_PORT_NAME(unit, port));
        }
        return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "lmod") == 0) {
        if ((subcmd = ARG_GET(a)) == NULL)
            return CMD_USAGE;

        if (sal_strcasecmp(subcmd, "set") == 0) {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Port", PQ_DFL|PQ_PORT|PQ_BCM,
                            0, (void *)&arg_port, 0);
            parse_table_add(&pt, "FModule", PQ_DFL|PQ_INT,
                            0, &arg_fmod,  NULL);
            parse_table_add(&pt, "LModule", PQ_DFL|PQ_INT,
                            0, &arg_lmod,  NULL);
            if (!parseEndOk(a, &pt, &retCode))
                return retCode;

            r = bcm_stk_fmod_lmod_mapping_set(unit, arg_port,
					      arg_fmod, arg_lmod);

            if (r < 0) {
                cli_out("ERROR: %s %s port %s failed: %s\n",
                        ARG_CMD(a), subcmd,
                        BCM_PORT_NAME(unit, arg_port), bcm_errmsg(r));
                return CMD_FAIL;
            }
            cli_out("port %s assigns fmod=%d and lmod=%d.\n",
                    BCM_PORT_NAME(unit, arg_port), arg_fmod, arg_lmod);
            return CMD_OK;
        }

        if (sal_strcasecmp(subcmd, "get") == 0) {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "Port", PQ_DFL|PQ_PORT|PQ_BCM,
                            0, (void *)&arg_port, 0);
            if (!parseEndOk(a, &pt, &retCode))
                return retCode;

            r = bcm_stk_fmod_lmod_mapping_get(unit, arg_port, &fmod, &lmod);

            if (r < 0) {
                cli_out("ERROR: %s %s port %s failed: %s\n",
                        ARG_CMD(a), subcmd,
                        BCM_PORT_NAME(unit, arg_port), bcm_errmsg(r));
                return CMD_FAIL;
            }
            cli_out("port %s has fmod=%d maps to lmod=%d.\n",
                    BCM_PORT_NAME(unit, arg_port), fmod, lmod);
            return CMD_OK;
        }
    }

    if (sal_strcasecmp(subcmd, "smod") == 0) {
        if ((subcmd = ARG_GET(a)) == NULL)
            return CMD_USAGE;

        if (sal_strcasecmp(subcmd, "set") == 0) {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                            (void *)(0), &pbmp, NULL);
            parse_table_add(&pt, "FModule", PQ_DFL|PQ_INT,
                            0, &arg_fmod,  NULL);
            parse_table_add(&pt, "SModule", PQ_DFL|PQ_INT,
                            0, &arg_smod,  NULL);
            parse_table_add(&pt, "SPort", PQ_DFL|PQ_INT,
                            0, &arg_sport,  NULL);
            parse_table_add(&pt, "NPorts", PQ_DFL|PQ_INT,
                            0, &arg_nports,  NULL);
            if (!parseEndOk(a, &pt, &retCode))
                return retCode;

            BCM_PBMP_AND(pbmp, pcfg.stack_ext);
            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
                r = bcm_stk_fmod_smod_mapping_set(unit, port,
						  arg_fmod, arg_smod,
						  arg_sport, arg_nports);
                if (r < 0) {
                    cli_out("ERROR: %s %s failed: %s\n",
                            ARG_CMD(a), subcmd, bcm_errmsg(r));
                    return CMD_FAIL;
                }
                if (arg_nports != 0) {
                    cli_out("port %s: fmod=%d maps smod=%d "
                            "with %d ports (base_port=%d)\n",
                            BCM_PORT_NAME(unit, port),
                            arg_fmod, arg_smod, arg_nports, arg_sport);
                } else {
                    cli_out("port %s: fmod=%d maps smod=%d deleted\n",
                            BCM_PORT_NAME(unit, port),
                            arg_fmod, arg_smod);
                }
            }
            return CMD_OK;
        }

        if (sal_strcasecmp(subcmd, "get") == 0) {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                            (void *)(0), &pbmp, NULL);
            parse_table_add(&pt, "FModule", PQ_DFL|PQ_INT,
                            0, &arg_fmod,  NULL);
            if (!parseEndOk(a, &pt, &retCode))
                return retCode;

            BCM_PBMP_AND(pbmp, pcfg.stack_ext);
            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
                r = bcm_stk_fmod_smod_mapping_get(unit, port, arg_fmod,
                                                  &smod, &sport, &nports);
                if (r < 0) {
                    cli_out("ERROR: %s %s failed: %s\n",
                            ARG_CMD(a), subcmd, bcm_errmsg(r));
                    return CMD_FAIL;
                }
                cli_out("port %s: fmod=%d maps smod=%d "
                        "with %d ports (base_port=%d)\n",
                        BCM_PORT_NAME(unit, port),
                        arg_fmod, smod, nports, sport);
            }
            return CMD_OK;
        }
    }

    if (sal_strcasecmp(subcmd, "group") == 0) {
        if ((subcmd = ARG_GET(a)) == NULL)
            return CMD_USAGE;

        if (sal_strcasecmp(subcmd, "set") == 0) {
            parse_table_init(unit, &pt);
            parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                            (void *)(0), &pbmp, NULL);
            parse_table_add(&pt, "Group", PQ_DFL|PQ_INT,
                            0, &arg_group,  NULL);
            if (!parseEndOk(a, &pt, &retCode))
                return retCode;

            /* Parameter limit check */
            r = bcm_stk_port_modmap_group_max_get(unit, &group);
            if (r < 0) {
                cli_out("ERROR: %s %s failed: %s\n",
                        ARG_CMD(a), subcmd, bcm_errmsg(r));
                return CMD_FAIL;
            }

            if (arg_group > group) {
                cli_out("ERROR: Modmap group %d exceeds maximum of %d\n",
                        arg_group, group);
                return CMD_FAIL;
            }

            BCM_PBMP_AND(pbmp, pcfg.stack_ext);
            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
                r = bcm_stk_port_modmap_group_set(unit, port, arg_group);
                if (r < 0) {
                    cli_out("ERROR: %s %s failed: %s\n",
                            ARG_CMD(a), subcmd, bcm_errmsg(r));
                    return CMD_FAIL;
                }
                cli_out("port %s set to modmap group %d\n",
                        BCM_PORT_NAME(unit, port), arg_group);
            }
            return CMD_OK;
        }

        if (sal_strcasecmp(subcmd, "show") == 0) {
            pbmp_t pbmp_g0, pbmp_g1;
            char   bufp[FORMAT_PBMP_MAX];

            BCM_PBMP_CLEAR(pbmp_g0);
            BCM_PBMP_CLEAR(pbmp_g1);

            BCM_PBMP_ASSIGN(pbmp, pcfg.stack_ext);
            DPORT_BCM_PBMP_ITER(unit, pbmp, dport, port) {
                r = bcm_stk_port_modmap_group_get(unit, port, &group);
                if (r < 0) {
                    cli_out("ERROR: %s %s failed: %s\n",
                            ARG_CMD(a), subcmd, bcm_errmsg(r));
                    return CMD_FAIL;
                }

                if (group == 1) {
                    /* coverity[overrun-local] */
                    BCM_PBMP_PORT_ADD(pbmp_g1, port);
                } else if (group == 0) {
                    /* coverity[overrun-local] */
                    BCM_PBMP_PORT_ADD(pbmp_g0, port);
                } else {
                    /* Shouldn't be here */
                    cli_out("ERROR: Inconsistent modmap group\n");
                    return CMD_FAIL;
                }
            }
            format_bcm_pbmp(unit, bufp, sizeof(bufp), pbmp_g0);
            cli_out("Modmap group 0: %s\n", bufp);
            format_bcm_pbmp(unit, bufp, sizeof(bufp), pbmp_g1);
            cli_out("Modmap group 1: %s\n", bufp);
            return CMD_OK;
        }
    }

    if (sal_strcasecmp(subcmd, "show") == 0) {
        BCM_PBMP_ITER(pcfg.stack_ext, port) {
            r = bcm_stk_modmap_enable_get(unit, port, &mode);

            if (r < 0) {
                cli_out("ERROR: %s %s port %s failed: %s\n",
                        ARG_CMD(a), subcmd, BCM_PORT_NAME(unit, port),
                        bcm_errmsg(r));
                return CMD_FAIL;
            }
            cli_out("port %s modid mapping %s.\n",
                    BCM_PORT_NAME(unit, port), (mode ? "enable" : "disable"));

            if (mode) {
                r = bcm_stk_fmod_lmod_mapping_get(unit, port, &fmod, &lmod);
                if (r < 0) {
                    cli_out("ERROR: %s %s port %s failed: %s\n",
                            ARG_CMD(a), subcmd, BCM_PORT_NAME(unit, port),
                            bcm_errmsg(r));
                    return CMD_FAIL;
                }
                cli_out("         fmod=%d maps lmod=%d\n", fmod, lmod);

                r = bcm_stk_fmod_smod_mapping_get(unit, port, fmod,
                                                  &smod, &sport, &nports);
                if (r < 0) {
                    cli_out("ERROR: %s %s failed: %s\n",
                            ARG_CMD(a), subcmd, bcm_errmsg(r));
                    return CMD_FAIL;
                }
                cli_out("         fmod=%d maps smod=%d "
                        "with %d ports (base_port=%d)\n",
                        fmod, smod, nports, sport);
            }
        }
        return CMD_OK;
    }

    return CMD_USAGE;
}
#endif /* BCM_XGS_SUPPORT */
