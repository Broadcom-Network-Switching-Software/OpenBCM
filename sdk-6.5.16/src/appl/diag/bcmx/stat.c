/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        stat.c
 * Purpose:	BCMX diagnostic stat command
 */

#ifdef	INCLUDE_BCMX

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/stat.h>
#include <bcmx/stat.h>
#include <bcmx/lplist.h>

#include <appl/diag/parse.h>
#include <appl/diag/shell.h>
#include <appl/diag/system.h>
#include "bcmx.h"

char bcmx_cmd_stat_usage[] =
	"\n"
	"\tstat init\n"
	"\tstat show [all] <user-port-list>\n"
	"\tstat clear [<user-port-list>]\n";

cmd_result_t
bcmx_cmd_stat(int unit, args_t *args)
{
    char		*cmd, *subcmd, *s;
    int			rv, all, count;
    bcmx_lport_t	lport;
    bcmx_lplist_t	lplist;
    bcm_stat_val_t	stype;
    uint64		sval;
    char		*sname;
    char        *_stat_names[] = BCM_STAT_NAME_INITIALIZER;

    cmd = ARG_CMD(args);
    subcmd = ARG_GET(args);
    if (subcmd == NULL) {
	subcmd = "show";
    }

    if (sal_strcasecmp(subcmd, "init") == 0) {
	rv = bcmx_stat_init();
	if (rv < 0) {
	    sal_printf("%s: ERROR: %s failed: %s\n",
		       cmd, subcmd, bcm_errmsg(rv));
	    return CMD_FAIL;
	}
	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "show") == 0) {
	all = 0;
	s = ARG_GET(args);
	if (s != NULL && sal_strcasecmp(s, "all") == 0) {
	    all = 1;
	    s = ARG_GET(args);
	}
	if (s == NULL) {
	    s = "*";
	}
	if (bcmx_lplist_init(&lplist, 0, 0) < 0) {
	    sal_printf("%s: ERROR: could not init port list\n", cmd);
	    return CMD_FAIL;
	}
        if (bcmx_lplist_parse(&lplist, s) < 0) {
            sal_printf("%s: ERROR: could not parse port list: %s\n",
                         cmd, s);
            return CMD_FAIL;
        }

	BCMX_LPLIST_ITER(lplist, lport, count) {
        sal_printf("%s: Statistics for port %s\n",
                   cmd, bcmx_lport_to_uport_str(lport));

	    for (stype = (bcm_stat_val_t)0; stype < snmpValCount; stype++) {
		sname = _stat_names[stype];
		rv = bcmx_stat_get(lport, stype, &sval);
		if (rv < 0) {
		    sal_printf("\t%18s\t%s (stat %d): %s\n",
			       "-", sname, stype, bcm_errmsg(rv));
		    continue;
		}
		if (all == 0 && COMPILER_64_IS_ZERO(sval)) {
		    continue;
		}
		if (COMPILER_64_HI(sval) == 0) {
		    sal_printf("\t%18u\t%s (stat %d)\n",
			       COMPILER_64_LO(sval), sname, stype);
		} else {
		    sal_printf("\t0x%08x%08x\t%s (stat %d)\n",
			       COMPILER_64_HI(sval),
			       COMPILER_64_LO(sval),
			       sname, stype);
		}

	    }
	}
        bcmx_lplist_free(&lplist);
	return CMD_OK;
    }

    if (sal_strcasecmp(subcmd, "clear") == 0) {
	s = ARG_GET(args);
	if (s == NULL) {
	    s = "*";
	}
	if (bcmx_lplist_init(&lplist, 0, 0) < 0) {
	    sal_printf("%s: ERROR: could not init port list\n", cmd);
	    return CMD_FAIL;
	}
        if (bcmx_lplist_parse(&lplist, s) < 0) {
            sal_printf("%s: ERROR: could not parse port list: %s\n",
                         cmd, s);
            return CMD_FAIL;
        }

	BCMX_LPLIST_ITER(lplist, lport, count) {
	    rv = bcmx_stat_clear(lport);
	    if (rv < 0) {
            sal_printf("%s: ERROR: %s on port %s failed: %s\n",
                       cmd, subcmd,
                       bcmx_lport_to_uport_str(lport), bcm_errmsg(rv));
            return CMD_FAIL;
	    }
	}
        bcmx_lplist_free(&lplist);
	return CMD_OK;
    }

    sal_printf("%s: unknown subcommand '%s'\n", ARG_CMD(args), subcmd);
    return CMD_USAGE;
}

#endif	/* INCLUDE_BCMX */
