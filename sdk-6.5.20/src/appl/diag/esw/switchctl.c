/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Switch Control commands
 */

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/dport.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/switch.h>

STATIC char *switch_control_names[] = {
    BCM_SWITCH_CONTROL_STR
};

char cmd_switch_control_usage[] =
    "\n\tSwitchControl [PortBitMap=<pbmp>] [<name> | <name>=<value>]\n";

cmd_result_t
cmd_switch_control(int unit, args_t *a)
{
    parse_table_t		pt;
    int				r;
    bcm_pbmp_t			arg_pbmp;
    int				arg_pbmp_given;
    bcm_switch_control_t	type;
    int				param;
    int				retCode = CMD_OK;
    bcm_port_t			port, dport;
    char			*spec;
    char			name[128], *value;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    if (COUNTOF(switch_control_names) != bcmSwitch__Count) {
	cli_out("%s: Error: NAMES ARRAY OUT OF SYNC\n", ARG_CMD(a));
	return CMD_FAIL;
    }

    arg_pbmp = PBMP_ALL(unit);

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "PortBitMap", 	PQ_DFL|PQ_PBMP|PQ_BCM,
		    (void *)(0), &arg_pbmp, NULL);
    if (parse_arg_eq(a, &pt) < 0) {
	parse_arg_eq_done(&pt);
	cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
	return CMD_FAIL;
    }

    arg_pbmp_given = ((pt.pt_entries[0].pq_type & PQ_PARSED) != 0);

    parse_arg_eq_done(&pt);

    if ((spec = ARG_GET(a)) == NULL) {
	if (!arg_pbmp_given) {
	    for (type = 0; type < bcmSwitch__Count; type++) {
		r = bcm_switch_control_get(unit, type, &param);
                /* Supported only */
		if (r == BCM_E_NONE) {
                    cli_out("%-40s", switch_control_names[type]);
		    cli_out("0x%x\n", (uint32)param);
		}
	    }
	} else {
            /* coverity[overrun-local] */
            DPORT_BCM_PBMP_ITER(unit, arg_pbmp, dport, port) {
		cli_out("%s:\n", BCM_PORT_NAME(unit, port));
		for (type = 0; type < bcmSwitch__Count; type++) {
		    r = bcm_switch_control_port_get(unit, port, type, &param);
                    /* Supported only */
                    if (r == BCM_E_NONE) {
                        cli_out("    %-40s", switch_control_names[type]);
                        cli_out("0x%x\n", (uint32)param);
		    }
		}
	    }
	}
    } else {
	strncpy(name, spec, sizeof (name));
	name[sizeof (name) - 1] = 0;
	if ((value = strchr(name, '=')) != NULL) {
	    *value++ = 0;
	}

	for (type = 0; type < bcmSwitch__Count; type++) {
	    if (sal_strcasecmp(name, switch_control_names[type]) == 0) {
		break;
	    }
	}

	if (type == bcmSwitch__Count) {
        int first_hit = 0;
	    cli_out("Unknown switch control: %s\n", name);
        for (type = 0; type < bcmSwitch__Count; type++) {
            if (strcaseindex(switch_control_names[type], name) != NULL) {
                if (first_hit == 0) {
                    cli_out("Did you mean?\n");
                    first_hit = 1;
                }
                cli_out("\t%s\n", switch_control_names[type]);
            }
        }
	    return CMD_FAIL;
	}

	if (value == NULL) {
	    if (!arg_pbmp_given) {
		r = bcm_switch_control_get(unit, type, &param);
		if (r < 0) {
		    cli_out("bcm_switch_control_get ERROR: %s\n",
                            bcm_errmsg(r));
		    retCode = CMD_FAIL;
		} else {
		    cli_out("0x%x\n", (uint32)param);
		}
	    } else {
                DPORT_BCM_PBMP_ITER(unit, arg_pbmp, dport, port) {
		    cli_out("%s: ", BCM_PORT_NAME(unit, port));
		    r = bcm_switch_control_port_get(unit, port, type, &param);
		    if (r < 0) {
			cli_out("bcm_switch_control_port_get ERROR: %s\n",
                                bcm_errmsg(r));
		    } else {
			cli_out("0x%x\n", (uint32)param);
		    }
		}
	    }
	} else {
	    param = parse_integer(value);

	    if (!arg_pbmp_given) {
		r = bcm_switch_control_set(unit, type, param);
		if (r < 0) {
		    cli_out("bcm_switch_control_set ERROR: %s\n",
                            bcm_errmsg(r));
		    retCode = CMD_FAIL;
		}
	    } else {
                DPORT_BCM_PBMP_ITER(unit, arg_pbmp, dport, port) {
		    r = bcm_switch_control_port_set(unit, port, type, param);
		    if (r < 0) {
			cli_out("bcm_switch_control_port_set ERROR: "
                                "port=%d: %s\n",
                                port, bcm_errmsg(r));
			retCode = CMD_FAIL;
		    }
		}
	    }
	}
    }

    return retCode;
}
