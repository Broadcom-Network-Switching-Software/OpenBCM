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

#ifdef BCM_DFE_SUPPORT
#include <soc/dfe/cmn/dfe_drv.h>
#endif

#ifdef BCM_DNXF_SUPPORT
#include <soc/dnxf/cmn/dnxf_drv.h>
#endif

char cmd_stkmode_usage[] =
"Parameters:\n"
"	Modid=<m>		Set the module identifier\n"
"	ModPortClear=[T|F]	Clear modport table\n"
"	SLCount=<n>		Set SL Simplex hop count\n"
"	Cascade=[T|F]		Enable/disable Higig/SL Cascade\n"
"	SimplexPorts=<pbmp>	SL Simplex mode stack ports\n"
"	DuplexPorts=<pbmp>	SL Duplex mode stack ports\n"
"If no parameters, show current state\n";

cmd_result_t
cmd_stkmode(int unit, args_t *a)
{
    parse_table_t	pt;
    int			rv, modcount;
    int			modid, modportclear, slmode, slcount;
    int enable=-1;
    bcm_pbmp_t		stkp_simplex, stkp_duplex;
    uint32              flags = 0;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    if (ARG_CNT(a) == 0) {	/* show current state */
    	rv = bcm_stk_modid_get(unit, &modid);
    	if (rv < 0) {
    	    cli_out("%s: ERROR: bcm_stk_modid_get: %s\n",
                    ARG_CMD(a), bcm_errmsg(rv));
    	    return CMD_FAIL;
    	}
#ifdef BCM_DFE_SUPPORT
        if(SOC_IS_DFE(unit)) {
             cli_out("unit %d: module id %d\n", unit, modid);
             return CMD_OK;
        }
#endif
#ifdef BCM_DNXF_SUPPORT
        if(SOC_IS_DNXF(unit)) {
             cli_out("unit %d: module id %d\n", unit, modid);
             return CMD_OK;
        }
#endif
    	if (bcm_stk_modid_count(unit, &modcount) < 0) {
    	    modcount = 1;
    	}
            rv = bcm_stk_mode_get(unit, &flags);
            if (rv < 0) {
                cli_out("ERROR: bcm_stk_mode_get returns %s\n", bcm_errmsg(rv));
                return CMD_FAIL;
            }
    	slmode = (flags & BCM_STK_SL) ? 1 : 0;
    	slcount = 0;
    	if (modcount != 1) {
    	    cli_out("%s: unit %d: module id %d (uses %d module ids)\n",
                    ARG_CMD(a), unit, modid, modcount);
    	} else {
    	    cli_out("%s: unit %d: module id %d\n",
                    ARG_CMD(a), unit, modid);
    	}
    	if (slmode || slcount) {
            /* coverity[dead_error_line] */
    	    cli_out("%s: unit %d: SL mode %s, simplex hop count %d, "
                    "Higig/SL\n",
                    ARG_CMD(a), unit,
                    slmode ? "on" : "off",
                    slcount);
    	}
    #if 0 
        char                pfmt[SOC_PBMP_FMT_LEN];
        char                pstr[FORMAT_PBMP_MAX];
    
    	if (bcm_stk_pbmp_get(unit, &stkp_simplex, &stkp_duplex) >= 0) {
    	    if (BCM_PBMP_NOT_NULL(stkp_simplex)) {
    		format_pbmp(unit, pstr, sizeof(pstr), stkp_simplex);
    		cli_out("%s: unit %d: SL simplex stack ports %s %s\n",
                        ARG_CMD(a), unit,
                        SOC_PBMP_FMT(stkp_simplex, pfmt), pstr);
    	    }
    	    if (BCM_PBMP_NOT_NULL(stkp_duplex)) {
    		format_pbmp(unit, pstr, sizeof(pstr), stkp_duplex);
    		cli_out("%s: unit %d: SL duplex  stack ports %s %s\n",
                        ARG_CMD(a), unit,
                        SOC_PBMP_FMT(stkp_duplex, pfmt), pstr);
    	    }
    	}
    #endif
    	return CMD_OK;
    }

    modid = slcount = -1;
    modportclear = -1;
    BCM_PBMP_CLEAR(stkp_simplex);
    BCM_PBMP_CLEAR(stkp_duplex);

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Modid", PQ_INT|PQ_DFL,
		    0, &modid, NULL);
#ifdef BCM_DFE_SUPPORT
    if(SOC_IS_DFE(unit)) {
        parse_table_add(&pt, "ENable", PQ_BOOL|PQ_DFL,
        0, &enable, NULL);
    } 
    else
#endif
#ifdef BCM_DNXF_SUPPORT
    if(SOC_IS_DNXF(unit)) {
        parse_table_add(&pt, "ENable", PQ_BOOL|PQ_DFL,
        0, &enable, NULL);
    }
    else
#endif
{
    parse_table_add(&pt, "ModPortClear", PQ_BOOL|PQ_DFL,
		    0, &modportclear, NULL);
    parse_table_add(&pt, "SLCount", PQ_INT|PQ_DFL,
		    0, &slcount, NULL);
    parse_table_add(&pt, "SimplexPorts", PQ_PBMP|PQ_BCM|PQ_DFL,
		    0, &stkp_simplex, NULL);
    parse_table_add(&pt, "DuplexPorts", PQ_PBMP|PQ_BCM|PQ_DFL,
		    0, &stkp_duplex, NULL);
}
    if (parse_arg_eq(a, &pt) < 0) {
    	cli_out("%s: Invalid argument: %s\n", ARG_CMD(a), ARG_CUR(a));
    	parse_arg_eq_done(&pt);
    	return CMD_FAIL;
    }
    parse_arg_eq_done(&pt);

    if (modid >= 0) {
    	rv = bcm_stk_modid_set(unit, modid);
    	if (rv < 0) {
    	    cli_out("%s: ERROR: bcm_stk_modid_set: %s\n",
                    ARG_CMD(a), bcm_errmsg(rv));
    	    return CMD_FAIL;
    	}
    }

    /* coverity[dead_error_begin] */
    if (enable >= 0) {
    	rv = bcm_stk_module_enable(unit, modid, -1, enable);
    	if (rv < 0) {
    	    cli_out("%s: ERROR: bcm_stk_module_enable: %s\n",
                    ARG_CMD(a), bcm_errmsg(rv));
    	    return CMD_FAIL;
    	}
    }
    /* coverity[dead_error_begin] */

    if (modportclear > 0) {
	rv = bcm_stk_modport_clear_all(unit);
	if (rv < 0) {
	    cli_out("%s: ERROR: bcm_stk_modport_clear_all: %s\n",
                    ARG_CMD(a), bcm_errmsg(rv));
	    return CMD_FAIL;
	}
    }
#if 0 
    if (BCM_PBMP_NOT_NULL(stkp_simplex) || BCM_PBMP_NOT_NULL(stkp_duplex)) {
	rv = bcm_stk_pbmp_get(unit, stkp_simplex, stkp_duplex);
	if (rv < 0) {
	    cli_out("%s: ERROR: bcm_stk_port_add: %s\n",
                    ARG_CMD(a), bcm_errmsg(rv));
	    return CMD_FAIL;
	}
    }
#endif
    if (slcount >= 0) {
	/* bcm_stk_sl_simplex_count_set is deprecated */
	cli_out("%s: ERROR: bcm_stk_sl_simplex_count_set: %s\n",
                ARG_CMD(a), bcm_errmsg(BCM_E_UNAVAIL));
	return CMD_FAIL;
    }
    return CMD_OK;
}


