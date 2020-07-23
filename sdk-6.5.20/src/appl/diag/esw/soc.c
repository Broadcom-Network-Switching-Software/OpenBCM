/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * SOC-related CLI commands
 */

#include <shared/bsl.h>

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/dport.h>
#include <shared/bsl.h>
#include <bcm/error.h>
#include <bcm/init.h>

#include <soc/mem.h>
#include <soc/drv.h>

#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif


#ifdef BCM_XGS12_FABRIC_SUPPORT
#include <appl/diag/pp.h>
#include <soc/hercules.h>
#endif

#ifdef BCM_XGS_SUPPORT
#include <soc/hash.h>
#endif

/*
 * Raw SOC reset
 */

char socres_usage[] =
    "Parameters: none\n\t"
    "Performs an SCP reset -- NOT recommended\n";

cmd_result_t
cmd_socres(int unit, args_t *a)
{
    if (!sh_check_attached(ARG_CMD(a), unit))
	return CMD_FAIL;

    if (soc_reset(unit) < 0)
	return CMD_FAIL;

    return CMD_OK;
}

/*
 * Set number of microseconds before timing out on the S-Channel DONE bit
 * during S-Channel operations.
 */

char stimeout_usage[] =
    "Parameters: [usec]\n\t"
    "Sets number of microseconds before S-Channel operations time out.\n\t"
    "If argument is not given, displays the current setting.\n";

cmd_result_t
cmd_stimeout(int unit, args_t *a)
{
    char *usec_str = ARG_GET(a);
    int usec;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    if (usec_str) {
	usec = parse_integer(usec_str);
	SOC_CONTROL(unit)->schanTimeout = usec;
    } else {
	cli_out("S-Channel timeout is %d usec\n",
                SOC_CONTROL(unit)->schanTimeout);
    }

    return CMD_OK;
}

/*
 * Set number of microseconds before timing out on the MIIM DONE bit
 * during PHY register operations.
 */

char mtimeout_usage[] =
    "Parameters: [usec]\n\t"
    "Sets number of microseconds before MIIM operations time out.\n\t"
    "If argument is not given, displays the current setting.\n";

cmd_result_t
cmd_mtimeout(int unit, args_t *a)
{
    char *usec_str = ARG_GET(a);
    int usec;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    if (usec_str) {
	usec = parse_integer(usec_str);
	SOC_CONTROL(unit)->miimTimeout = usec;
    } else {
	cli_out("MIIM timeout is %d usec\n",
                SOC_CONTROL(unit)->miimTimeout);
    }

    return CMD_OK;
}

/*
 * Set number of milliseconds before timing out on BIST operations.
 */

char btimeout_usage[] =
    "Parameters: [msec]\n\t"
    "Sets number of milliseconds before BIST operations time out.\n\t"
    "If argument is not given, displays the current setting.\n";

cmd_result_t
cmd_btimeout(int unit, args_t *a)
{
    char *msec_str = ARG_GET(a);
    int msec;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    if (msec_str) {
	msec = parse_integer(msec_str);
	SOC_CONTROL(unit)->bistTimeout = msec;
    } else {
	cli_out("BIST timeout is %d msec\n",
                SOC_CONTROL(unit)->bistTimeout);
    }

    return CMD_OK;
}

/*
 * Perform generic S-channel operation given raw S-channel buffer words.
 */

char schan_usage[] =
    "Parameters: DW0 [... DWn]\n\t"
    "Sends the specified raw data as an S-channel message, waits for a\n\t"
    "response, then displays the S-channel message buffer (response).\n";

cmd_result_t
cmd_schan(int unit, args_t *a)
{
    schan_msg_t msg;
    int i;
    char *datastr = ARG_GET(a);
    int rv = CMD_FAIL;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	goto done;
    }

    if (datastr == NULL) {
	return CMD_USAGE;
    }

    sal_memset(&msg, 0, sizeof (msg));	/* Clear all words, not just header */

    for (i = 0; datastr; i++) {
	msg.dwords[i] = parse_integer(datastr);
	datastr = ARG_GET(a);
    }

    if ((i = soc_schan_op(unit, &msg,
			  CMIC_SCHAN_WORDS(unit),
			  CMIC_SCHAN_WORDS(unit), 1)) < 0) {
	cli_out("S-Channel operation failed: %s\n", soc_errmsg(i));
	goto done;
    }

    for (i = 0; i < CMIC_SCHAN_WORDS(unit); i++) {
	cli_out("0x%x ", msg.dwords[i]);
    }

    cli_out("\n");

    rv = CMD_OK;

 done:
    return rv;
}

cmd_result_t
cmd_esw_soc(int u, args_t *a)
/*
 * Function: 	sh_soc
 * Purpose:	Print soc control information.
 * Parameters:	u - unit #
 *		a - pointer to args, expects <unit> ...., if none passed,
 *			default unit # used.
 * Returns:	CMD_OK/CMD_FAIL
 */
{
    char 	*c;
    cmd_result_t rv = CMD_OK;

    if (!sh_check_attached("soc", u)) {
        return(CMD_FAIL);
    }

    if (!ARG_CNT(a)) {
        rv = soc_dump(u, "  ") ? CMD_FAIL : CMD_OK;
    } else {
        while ((CMD_OK == rv) && (c = ARG_GET(a))) {
            if (!isint(c)) {
                cli_out("%s: Invalid unit identifier: %s\n", ARG_CMD(a), c);
                rv = CMD_FAIL;
            } else {
                rv = soc_dump(parse_integer(c), "  ") ?
                CMD_FAIL : CMD_OK;
            }
        }
    }
    return(rv);
}

/*
 * Utility routine for BIST command as well as BIST diagnostic
 */

static void
bist_add_mem(soc_mem_t *mems, int *num_mems, soc_mem_t mem)
{
    int 		i;

    for (i = 0; i < *num_mems; i++) {
	if (mems[i] == mem) {
	    return;		/* Already there */
	}
    }

    mems[(*num_mems)++] = mem;
}

int
bist_args_to_mems(int unit, args_t *a, soc_mem_t *mems, int *num_mems)
{
    char		*parm;
    soc_mem_t		mem;
    int			rv = 0;

    *num_mems = 0;

    while ((parm = ARG_GET(a)) != 0) {
	if (sal_strcasecmp(parm, "all") == 0) {
	    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
		if (soc_mem_is_valid(unit, mem) &&
		    (soc_mem_is_bistepic(unit, mem) ||
		     soc_mem_is_bistcbp(unit, mem) ||
		     soc_mem_is_bistffp(unit, mem))) {
		    bist_add_mem(mems, num_mems, mem);
		}
	    }
	} else if (sal_strcasecmp(parm, "cbp") == 0) {
	    for (mem = 0; mem < NUM_SOC_MEM; mem++) {
		if (soc_mem_is_valid(unit, mem) &&
		    soc_mem_is_cbp(unit, mem)) {
		    bist_add_mem(mems, num_mems, mem);
		}
	    }
	} else if (parse_memory_name(unit, &mem, parm, 0, 0) < 0) {
	    cli_out("BIST ERROR: Unknown memory name: %s\n", parm);
	    rv = -1;
	} else {
	    bist_add_mem(mems, num_mems, mem);
	}
    }

    if (*num_mems == 0) {
	cli_out("BIST ERROR: No memories specified\n");
	rv = -1;
    }

    return rv;
}

/*
 * BIST (non-diagnostic version)
 */

char bist_usage[] =
    "Parameters: [table] ...\n\t"
    "Runs raw BIST (built-in self-test) on list of BISTable memories.\n\t"
    "Use \"listmem\" to get a list of BISTable memories.\n\t"
    "The special name CBP can be used to refer to all CBP memories.\n";

cmd_result_t
cmd_bist(int unit, args_t *a)
{
    soc_mem_t		*mems;
    int 		num_mems;
    int			rv;

    /*BIST is not supported on XGSIII devices*/
    UNSUPPORTED_COMMAND(unit, SOC_CHIP_BCM56601, a);
    UNSUPPORTED_COMMAND(unit, SOC_CHIP_BCM56602, a);
    UNSUPPORTED_COMMAND(unit, SOC_CHIP_BCM56504, a);
    UNSUPPORTED_COMMAND(unit, SOC_CHIP_BCM56102, a);
    UNSUPPORTED_COMMAND(unit, SOC_CHIP_BCM56304, a);
    UNSUPPORTED_COMMAND(unit, SOC_CHIP_BCM56112, a);
    UNSUPPORTED_COMMAND(unit, SOC_CHIP_BCM56314, a);
    UNSUPPORTED_COMMAND(unit, SOC_CHIP_BCM56514, a);
    UNSUPPORTED_COMMAND(unit, SOC_CHIP_BCM5650, a);

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    mems = sal_alloc(sizeof(soc_mem_t) * NUM_SOC_MEM,
                                  "BIST mem list");
    if (NULL == mems) {
	cli_out("Insufficient memory for BIST\n");
        return BCM_E_MEMORY;
    }

    if (bist_args_to_mems(unit, a, mems, &num_mems) < 0) {
        sal_free(mems);
	return CMD_FAIL;
    }

    if ((rv = soc_bist(unit,
		       mems, num_mems,
		       SOC_CONTROL(unit)->bistTimeout)) < 0) {
	cli_out("BIST failed: %s\n", soc_errmsg(rv));
        sal_free(mems);
	return CMD_FAIL;
    }

    sal_free(mems);
    return CMD_OK;
}

/*
 * PBMP
 */
cmd_result_t
cmd_esw_bcm_pbmp(int unit, args_t *a)
{
    pbmp_t		pbmp;
    pbmp_t		st;
    char		*c;
    bcm_port_t		port;
    bcm_port_config_t   *pcfg;
    char        pbmp_str[FORMAT_PBMP_MAX];
    char        pfmt[SOC_PBMP_FMT_LEN];
    int         res = CMD_OK;

    COMPILER_REFERENCE(unit);

    pcfg = sal_alloc(sizeof(bcm_port_config_t), "bcm port config");
    if (NULL == pcfg) {
        cli_out("ERROR: %s\n", bcm_errmsg(BCM_E_MEMORY));
        return CMD_FAIL;
    }
    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, pcfg));

    c = ARG_GET(a);

    if (!c) {
        BCM_PBMP_ASSIGN(st, pcfg->stack_ext);
        BCM_PBMP_OR(st, pcfg->stack_int);

        cli_out("Current BCM bitmaps:\n");
        cli_out("     FE   ==> %s\n",
                SOC_PBMP_FMT(pcfg->fe, pfmt));
        cli_out("     GE   ==> %s\n",
                SOC_PBMP_FMT(pcfg->ge, pfmt));
        cli_out("     XE   ==> %s\n",
                SOC_PBMP_FMT(pcfg->xe, pfmt));
        cli_out("     E    ==> %s\n",
                SOC_PBMP_FMT(pcfg->e, pfmt));
        cli_out("     HG   ==> %s\n",
                SOC_PBMP_FMT(pcfg->hg, pfmt));
        cli_out("     HL   ==> %s\n",
                SOC_PBMP_FMT(PBMP_HL_ALL(unit), pfmt));
        cli_out("     ST   ==> %s\n",
                SOC_PBMP_FMT(st, pfmt));
        cli_out("     PORT ==> %s\n",
                SOC_PBMP_FMT(pcfg->port, pfmt));
        cli_out("     CMIC ==> %s\n",
                SOC_PBMP_FMT(pcfg->cpu, pfmt));
        cli_out("     ALL  ==> %s\n",
                SOC_PBMP_FMT(pcfg->all, pfmt));
        goto exit;
    }

    if (sal_strcasecmp(c, "port") == 0) {
        if ((c = ARG_GET(a)) == NULL) {
            cli_out("ERROR: missing port string\n");
            res = CMD_FAIL;
            goto exit;
        }
        if (parse_bcm_port(unit, c, &port) < 0) {
            cli_out("%s: Invalid port string: %s\n", ARG_CMD(a), c);
            res = CMD_FAIL;
            goto exit;
        }
        cli_out("    port %s ==> %s (%d)\n",
                c, BCM_PORT_NAME(unit, port), port);
        goto exit;
    }

    if (parse_bcm_pbmp(unit, c, &pbmp) < 0) {
        cli_out("%s: Invalid pbmp string (%s); use 'pbmp ?' for more info.\n",
                ARG_CMD(a), c);
        res = CMD_FAIL;
        goto exit;
    }

    format_bcm_pbmp(unit, pbmp_str, sizeof (pbmp_str), pbmp);

    cli_out("    %s ==> %s\n", SOC_PBMP_FMT(pbmp, pfmt), pbmp_str);

exit:
    if (pcfg != NULL) {
        sal_free(pcfg);
    }
    return res;
}

/*
 * Disabled PBMP
 */
cmd_result_t
cmd_esw_disabled_pbmp(int unit, args_t *a)
{
    char *c;
    char pfmt[SOC_PBMP_FMT_LEN];

    COMPILER_REFERENCE(unit);

    c = ARG_GET(a);

    if (!c) {
        cli_out("Current disabled bitmaps:\n");
        cli_out("     FE   ==> %s\n",
                SOC_PBMP_FMT(SOC_PORT_DISABLED_BITMAP(unit, fe), pfmt));
        cli_out("     GE   ==> %s\n",
                SOC_PBMP_FMT(SOC_PORT_DISABLED_BITMAP(unit, ge), pfmt));
        cli_out("     XE   ==> %s\n",
                SOC_PBMP_FMT(SOC_PORT_DISABLED_BITMAP(unit, xe), pfmt));
#ifdef BCM_CMAC_SUPPORT
        cli_out("     CE   ==> %s\n",
                SOC_PBMP_FMT(SOC_PORT_DISABLED_BITMAP(unit, ce), pfmt));
#endif
        cli_out("     E    ==> %s\n",
                SOC_PBMP_FMT(SOC_PORT_DISABLED_BITMAP(unit, ether), pfmt));
        cli_out("     HG   ==> %s\n",
                SOC_PBMP_FMT(SOC_PORT_DISABLED_BITMAP(unit, hg), pfmt));
        cli_out("     LP   ==> %s\n",
                SOC_PBMP_FMT(SOC_PORT_DISABLED_BITMAP(unit, lp), pfmt));
        cli_out("     IL   ==> %s\n",
                SOC_PBMP_FMT(SOC_PORT_DISABLED_BITMAP(unit, il), pfmt));
        cli_out("     SCH  ==> %s\n",
                SOC_PBMP_FMT(SOC_PORT_DISABLED_BITMAP(unit, sch), pfmt));
        cli_out("     HL   ==> %s\n",
                SOC_PBMP_FMT(SOC_PORT_DISABLED_BITMAP(unit, hl), pfmt));
        cli_out("     ST   ==> %s\n",
                SOC_PBMP_FMT(SOC_PORT_DISABLED_BITMAP(unit, st), pfmt));
        cli_out("     GX   ==> %s\n",
                SOC_PBMP_FMT(SOC_PORT_DISABLED_BITMAP(unit, gx), pfmt));
        cli_out("     XL   ==> %s\n",
                SOC_PBMP_FMT(SOC_PORT_DISABLED_BITMAP(unit, xl), pfmt));
        cli_out("     MXQ  ==> %s\n",
                SOC_PBMP_FMT(SOC_PORT_DISABLED_BITMAP(unit, mxq), pfmt));
        cli_out("     XG   ==> %s\n",
                SOC_PBMP_FMT(SOC_PORT_DISABLED_BITMAP(unit, xg), pfmt));
        cli_out("     XQ   ==> %s\n",
                SOC_PBMP_FMT(SOC_PORT_DISABLED_BITMAP(unit, xq), pfmt));
        cli_out("     XT   ==> %s\n",
                SOC_PBMP_FMT(SOC_PORT_DISABLED_BITMAP(unit, xt), pfmt));
        cli_out("     XW   ==> %s\n",
                SOC_PBMP_FMT(SOC_PORT_DISABLED_BITMAP(unit, xw), pfmt));
        cli_out("     CL   ==> %s\n",
                SOC_PBMP_FMT(SOC_PORT_DISABLED_BITMAP(unit, cl), pfmt));
        cli_out("     C    ==> %s\n",
                SOC_PBMP_FMT(SOC_PORT_DISABLED_BITMAP(unit, c), pfmt));
        cli_out("     AXP  ==> %s\n",
                SOC_PBMP_FMT(SOC_PORT_DISABLED_BITMAP(unit, axp), pfmt));
        cli_out("     HPLT ==> %s\n",
                SOC_PBMP_FMT(SOC_PORT_DISABLED_BITMAP(unit, hyplite), pfmt));
        cli_out("     PORT ==> %s\n",
                SOC_PBMP_FMT(SOC_PORT_DISABLED_BITMAP(unit, port), pfmt));
        cli_out("     ALL  ==> %s\n",
                SOC_PBMP_FMT(SOC_PORT_DISABLED_BITMAP(unit, all), pfmt));

        return CMD_OK;
    }

    return CMD_OK;
}

cmd_result_t
cmd_esw_pbmp(int unit, args_t *a)
{
    pbmp_t		pbmp;
    char		*c;
    soc_port_t		port;
    char		pbmp_str[FORMAT_PBMP_MAX];
    char		pfmt[SOC_PBMP_FMT_LEN];

    COMPILER_REFERENCE(unit);

    c = ARG_GET(a);

    if (!c) {
        cli_out("Current bitmaps:\n");
        cli_out("     FE   ==> %s\n",
                SOC_PBMP_FMT(PBMP_FE_ALL(unit), pfmt));
        cli_out("     GE   ==> %s\n",
                SOC_PBMP_FMT(PBMP_GE_ALL(unit), pfmt));
        cli_out("     XE   ==> %s\n",
                SOC_PBMP_FMT(PBMP_XE_ALL(unit), pfmt));
#ifdef BCM_CMAC_SUPPORT
        cli_out("     CE   ==> %s\n",
                SOC_PBMP_FMT(PBMP_CE_ALL(unit), pfmt));
#endif
        cli_out("     E    ==> %s\n",
                SOC_PBMP_FMT(PBMP_E_ALL(unit), pfmt));
        cli_out("     HG   ==> %s\n",
                SOC_PBMP_FMT(PBMP_HG_ALL(unit), pfmt));
        cli_out("     LP   ==> %s\n",
                SOC_PBMP_FMT(PBMP_LP_ALL(unit), pfmt));
        cli_out("     IL   ==> %s\n",
                SOC_PBMP_FMT(PBMP_IL_ALL(unit), pfmt));
        cli_out("     SCH  ==> %s\n",
                SOC_PBMP_FMT(PBMP_SCH_ALL(unit), pfmt));
        cli_out("     HL   ==> %s\n",
                SOC_PBMP_FMT(PBMP_HL_ALL(unit), pfmt));
        cli_out("     HG2  ==> %s\n",
                SOC_PBMP_FMT(SOC_HG2_PBM(unit), pfmt));
        cli_out("     ST   ==> %s\n",
                SOC_PBMP_FMT(PBMP_ST_ALL(unit), pfmt));
        cli_out("     S    ==> %s\n",
                SOC_PBMP_FMT(SOC_INFO(unit).s_pbm, pfmt));
        cli_out("     GX   ==> %s\n",
                SOC_PBMP_FMT(PBMP_GX_ALL(unit), pfmt));
        cli_out("     XL   ==> %s\n",
                SOC_PBMP_FMT(PBMP_XL_ALL(unit), pfmt));
        cli_out("     XLB0 ==> %s\n",
                SOC_PBMP_FMT(PBMP_XLB0_ALL(unit), pfmt));
        cli_out("     MXQ  ==> %s\n",
                SOC_PBMP_FMT(PBMP_MXQ_ALL(unit), pfmt));
        cli_out("     XG   ==> %s\n",
                SOC_PBMP_FMT(PBMP_XG_ALL(unit), pfmt));
        cli_out("     XQ   ==> %s\n",
                SOC_PBMP_FMT(PBMP_XQ_ALL(unit), pfmt));
        cli_out("     XT   ==> %s\n",
                SOC_PBMP_FMT(PBMP_XT_ALL(unit), pfmt));
        cli_out("     XW   ==> %s\n",
                SOC_PBMP_FMT(PBMP_XW_ALL(unit), pfmt));
        cli_out("     CL   ==> %s\n",
                SOC_PBMP_FMT(PBMP_CL_ALL(unit), pfmt));
        cli_out("     CLG2 ==> %s\n",
                SOC_PBMP_FMT(PBMP_CLG2_ALL(unit), pfmt));
        cli_out("     C    ==> %s\n",
                SOC_PBMP_FMT(PBMP_C_ALL(unit), pfmt));
        cli_out("     CXX  ==> %s\n",
                SOC_PBMP_FMT(PBMP_CXX_ALL(unit), pfmt));
        cli_out("     AXP  ==> %s\n",
                SOC_PBMP_FMT(PBMP_AXP_ALL(unit), pfmt));
        cli_out("     HPLT ==> %s\n",
                SOC_PBMP_FMT(PBMP_HYPLITE_ALL(unit), pfmt));
        cli_out("     PORT ==> %s\n",
                SOC_PBMP_FMT(PBMP_PORT_ALL(unit), pfmt));
        cli_out("     CMIC ==> %s\n",
                SOC_PBMP_FMT(PBMP_CMIC(unit), pfmt));
        cli_out("     LB   ==> %s\n",
                SOC_PBMP_FMT(PBMP_LB(unit), pfmt));
        cli_out("     RDB  ==> %s\n",
                SOC_PBMP_FMT(PBMP_RDB_ALL(unit), pfmt));
        cli_out("     FAE  ==> %s\n",
                SOC_PBMP_FMT(PBMP_FAE_ALL(unit), pfmt));
        cli_out("     MMU  ==> %s\n",
                SOC_PBMP_FMT(PBMP_MMU(unit), pfmt));
        cli_out(" OVERSUB  ==> %s\n",
                SOC_PBMP_FMT(PBMP_OVERSUB(unit), pfmt));
        cli_out("    MGMT  ==> %s\n",
                SOC_PBMP_FMT(PBMP_MANAGEMENT(unit), pfmt));
        cli_out("     ALL  ==> %s\n",
                SOC_PBMP_FMT(PBMP_ALL(unit), pfmt));
        cli_out("  MACSEC  ==> %s\n",
                SOC_PBMP_FMT(PBMP_MACSEC_ALL(unit), pfmt));
        cli_out("  QSGMII  ==> %s\n",
                SOC_PBMP_FMT(PBMP_QSGMII_ALL(unit), pfmt));
        cli_out("   EGPHY  ==> %s\n",
                SOC_PBMP_FMT(PBMP_EGPHY_ALL(unit), pfmt));
        cli_out("     ROE  ==> %s\n",
                SOC_PBMP_FMT(PBMP_ROE_ALL(unit), pfmt));
        cli_out(" SUBTAG  ==>  %s\n",
                SOC_PBMP_FMT(PBMP_SUBTAG_ALL(unit), pfmt));
        cli_out(" UPLINK  ==>  %s\n",
                SOC_PBMP_FMT(PBMP_UPLINK(unit), pfmt));
        cli_out("STACKING ==>  %s\n",
                SOC_PBMP_FMT(PBMP_STACKING(unit), pfmt));

        return CMD_OK;
    }

    if (sal_strcasecmp(c, "bcm") == 0) {
        return cmd_esw_bcm_pbmp(unit, a);
    }

    if (sal_strcasecmp(c, "disabled") == 0) {
        return cmd_esw_disabled_pbmp(unit, a);
    }

    if (sal_strcasecmp(c, "port") == 0) {
	if ((c = ARG_GET(a)) == NULL) {
	    cli_out("ERROR: missing port string\n");
	    return CMD_FAIL;
	}
	if (parse_port(unit, c, &port) < 0) {
	    cli_out("%s: Invalid port string: %s\n", ARG_CMD(a), c);
	    return CMD_FAIL;
	}
	cli_out("    port %s ==> %s (%d)\n",
                c, SOC_PORT_NAME(unit, port), port);
	return CMD_OK;
    }

    if (parse_pbmp(unit, c, &pbmp) < 0) {
	cli_out("%s: Invalid pbmp string (%s); use 'pbmp ?' for more info.\n",
                ARG_CMD(a), c);
	return CMD_FAIL;
    }

    format_pbmp(unit, pbmp_str, sizeof (pbmp_str), pbmp);

    cli_out("    %s ==> %s\n", SOC_PBMP_FMT(pbmp, pfmt), pbmp_str);

    return CMD_OK;
}

#ifdef BCM_XGS12_FABRIC_SUPPORT

char if_xqdump_usage[] =
    "Usage :\n\t"
    "  xqdump <pbmp> [cos] [xqptr]\n\t"
    "          - Display packets pending in the XQ\n\t"
    "          - No COS selects all COS\n\t"
    "          - xqptr will try to dump packets already transmitted\n\t"
    "          - NOTE! This command will probably trash the chip state!\n";

cmd_result_t
if_xqdump(int unit, args_t * args)
{
    char                *argpbm, *argcos, *argptr;
    pbmp_t              pbm;
    int                 rv;
    int		        cos_start, cos_end, xq_ptr;
    soc_port_t          port, dport;

    if (!SOC_IS_XGS12_FABRIC(unit)) {
        cli_out("Command only valid for BCM5670/75 fabric\n");
        return CMD_FAIL;
    }

    if ((argpbm = ARG_GET(args)) == NULL) {
        pbm = PBMP_PORT_ALL(unit);
    } else {
        if (parse_pbmp(unit, argpbm, &pbm) < 0) {
            cli_out("%s: Error: unrecognized port bitmap: %s\n",
                    ARG_CMD(args), argpbm);
            return CMD_FAIL;
        }
    }

    if ((argcos = ARG_GET(args)) == NULL) {
        cos_start = 0;
        cos_end = NUM_COS(unit) - 1;
    } else {
        cos_start = cos_end = sal_ctoi(argcos, 0);

        if (cos_start >= NUM_COS(unit)) {
            cli_out("%s: Error: COS out of range: %s\n",
                    ARG_CMD(args), argcos);
            return CMD_FAIL;
        }
    }

    if ((argptr = ARG_GET(args)) == NULL) {
        /* coverity[overrun-local] */
        DPORT_SOC_PBMP_ITER(unit, pbm, dport, port) {
            if ((rv = diag_dump_cos_packets(unit, port,
                                            cos_start, cos_end)) < 0) {
                cli_out("Error dumping packets for port %d: %s\n",
                        port, soc_errmsg(rv));
                return CMD_FAIL;
            }
        }
    } else {
        xq_ptr = sal_ctoi(argptr, 0);
        DPORT_SOC_PBMP_ITER(unit, pbm, dport, port) {
            diag_dump_xq_packet(unit, SOC_PORT_BLOCK(unit, port), xq_ptr);
        }
    }

    cli_out("XQ state probably trashed.  Recommend resetting the chip.\n");

    return CMD_OK;
}

char if_ibdump_usage[] =
    "Usage :\n\t"
    "  ibdump <pbmp>\n\t"
    "          - Display packets pending in the Ingress Buffer\n";

cmd_result_t
if_ibdump(int unit, args_t * args)
{
    char                *argpbm;
    pbmp_t              pbm;
    int                 rv;
    soc_port_t          port;

    if (!SOC_IS_XGS12_FABRIC(unit)) {
        cli_out("Command only valid for BCM5670/75 fabric\n");
        return CMD_FAIL;
    }

    if (!soc_feature(unit, soc_feature_fabric_debug)) {
        cli_out("Command not valid for BCM5670_A0\n");
        return CMD_FAIL;
    }

    if ((argpbm = ARG_GET(args)) == NULL) {
        pbm = PBMP_PORT_ALL(unit);
    } else {
        if (parse_pbmp(unit, argpbm, &pbm) < 0) {
            cli_out("%s: Error: unrecognized port bitmap: %s\n",
                    ARG_CMD(args), argpbm);
            return CMD_FAIL;
        }
    }

    PBMP_PORT_ITER(unit, port) {
        if (PBMP_MEMBER((pbm), port)) {
            if ((rv = diag_dump_ib_packets(unit, port)) < 0) {
                cli_out("Error dumping packets for port %d: %s\n",
                        port, soc_errmsg(rv));
                return CMD_FAIL;
            }
        }
    }

    return CMD_OK;
}

char if_xqerr_usage[] =
    "Parameters: [PortBitMap=<pbmp>] [COS=<cos>]\n\t"
    "[BitsError=<num>][PacketNum=<num>][BitNum=<bit>]\n\t"
    "          - Introduce bit-errors to packets pending in the XQ\n\t"
    "          - Only one COS should be configured for this port,\n\t"
    "            else the chip state will be hopelessly scrambled.\n";

cmd_result_t
if_xqerr(int unit, args_t * args)
{
    parse_table_t	pt;
    soc_pbmp_t          pbm;
    int                 rv;
    int		        cos, errors, packet, bit;
    soc_port_t          port, dport;

    if (!SOC_IS_XGS12_FABRIC(unit)) {
        cli_out("Command only valid for BCM5670/75 fabric\n");
        return CMD_FAIL;
    }

    SOC_PBMP_CLEAR(pbm);
    cos = 0;
    errors = 1;
    packet = 0;
    bit = 0;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "PortBitMap", PQ_PBMP | PQ_DFL, 0, &pbm, 0);
    parse_table_add(&pt, "COS", PQ_INT | PQ_DFL, 0, &cos, 0);
    parse_table_add(&pt, "BitsError", PQ_INT | PQ_DFL, 0, &errors, 0);
    parse_table_add(&pt, "PacketNum", PQ_INT | PQ_DFL, 0, &packet, 0);
    parse_table_add(&pt, "BitNum", PQ_INT | PQ_DFL, 0, &bit, 0);

    if (parse_arg_eq(args, &pt) < 0) {
	cli_out("%s: Invalid argument: %s\n", ARG_CMD(args), ARG_CUR(args));
	parse_arg_eq_done(&pt);
	return(CMD_FAIL);
    }
    parse_arg_eq_done(&pt);

    if ( cos >= NUM_COS(unit) ) {
        cli_out("%s: Error: COS out of range: %d\n",
                ARG_CMD(args), cos);
            return CMD_FAIL;
    }

    if ( errors > 2 ) {
        cli_out("%s: Error: Excessive bit errors selected: %d\n",
                ARG_CMD(args), errors);
    }

    /* coverity[overrun-local] */
    DPORT_SOC_PBMP_ITER(unit, pbm, dport, port) {
        if ((rv = diag_set_cos_errors(unit, port, cos,
                                     errors, packet, bit)) < 0) {
            cli_out("Error adjusting XQ packets for port %d: %s\n",
                    port, soc_errmsg(rv));
            return CMD_FAIL;
        }
    }

    return CMD_OK;
}

char if_mmu_cfg_usage[] =
    "Usage :\n\t"
    "Parameters: [PortBitMap=<pbmp>] [COSNum=<cos>]\n\t"
    "[LosslessMode=true|false]\n\t"
    "          - Configure MMU for the given ports and number of COS.\n\t"
    "          - Lossless mode is ingress throttle.\n\t"
    "          - not Lossless is throughput mode or egress throttle.\n";

cmd_result_t
if_mmu_cfg(int unit, args_t * args)
{
    parse_table_t       pt;
    pbmp_t              pbm;
    int                 rv;
    int                 num_ports, num_cos, lossless;
    soc_port_t          port, dport;

    if (!SOC_IS_XGS12_FABRIC(unit)) {
        cli_out("Command only valid for BCM5670/75 fabric\n");
        return CMD_FAIL;
    }

    pbm = PBMP_ALL(unit);
    num_cos = 4;     /* Safe for both modes */
    lossless = FALSE;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "PortBitMap", PQ_PBMP | PQ_DFL, 0, &pbm, 0);
    parse_table_add(&pt, "COSNum", PQ_INT | PQ_DFL, 0, &num_cos, 0);
    parse_table_add(&pt, "LosslessMode", PQ_BOOL | PQ_DFL, 0, &lossless, 0);

    if (parse_arg_eq(args, &pt) < 0) {
	cli_out("%s: Invalid argument: %s\n", ARG_CMD(args), ARG_CUR(args));
	parse_arg_eq_done(&pt);
	return(CMD_FAIL);
    }
    parse_arg_eq_done(&pt);

    if (num_cos < 1) {
        cli_out("No COS selected.\n");
        return CMD_FAIL;
    } else if (num_cos > NUM_COS(unit)) {
        cli_out("Too many COS selected, unit only allows %d.\n",
                NUM_COS(unit));
        return CMD_FAIL;
    }

    if (lossless && (num_cos > 4)) {
	cli_out("%s: %d COS selected, lossless mode allows only 4.\n",
                ARG_CMD(args), num_cos);
	return(CMD_FAIL);
    }

    /* Only "real" ports */
    SOC_PBMP_AND(pbm, PBMP_ALL(unit));

    /* Count the ports */
    num_ports = 0;
    SOC_PBMP_ITER(pbm, port) {
        num_ports++;
    }

    if (num_ports < 1) {
        cli_out("No ports selected.\n");
        return CMD_FAIL;
    }

    /* Only want "other" port count, but don't try 0. */
    if (num_ports > 1) {
        num_ports--;
    }

    if (SOC_IS_HERCULES1(unit)) {
        DPORT_SOC_PBMP_ITER(unit, pbm, dport, port) {
            if ((rv = soc_hercules_mmu_limits_config(unit, port,
                                    num_ports, num_cos, lossless)) < 0) {
                cli_out("Error configuring MMU for port %d: %s\n",
                        port, soc_errmsg(rv));
                return CMD_FAIL;
            }
        }
    } else {
        /* coverity[overrun-local] */
        DPORT_SOC_PBMP_ITER(unit, pbm, dport, port) {
            if ((rv = soc_hercules15_mmu_limits_config(unit, port,
                                    num_ports, num_cos, lossless)) < 0) {
                cli_out("Error configuring MMU for port %d: %s\n",
                        port, soc_errmsg(rv));
                return CMD_FAIL;
            }
        }
    }

    return CMD_OK;
}

#endif /* BCM_XGS12_FABRIC_SUPPORT */

#ifdef BCM_XGS_SWITCH_SUPPORT

/*
 * Hash Select
 *
 *   Currently the only thing handled by this routine is HashSelect for
 *   Draco, however it is designed so it can be extended to support any
 *   other hash settings for any other chips.  Any such things should be
 *   added to the parse table below.
 */

char hash_usage[] =
    "Parameters: [HashSelect=<U16|L16|LSB|ZERO|U32|L32>]\n\t"
    "Displays all hash parameters if no arguments given.\n\t"
    "Note: if HashSelect is changed, L2 and L3 tables should be cleared.\n";

cmd_result_t
cmd_hash(int unit, args_t *a)
{
    parse_table_t	pt;
    int			r;
    int			hash_sel=0;
    char		**hashSels;
    static char		*xgs2hashSels[] = {
	"u16", "l16", "lsb", "zero", "u32", "l32", NULL
    };
    static char		*xgs3hashSels[] = {
	"zero", "u32", "l32", "lsb", "l16", "u16", NULL
    };

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    if (!soc_feature(unit, soc_feature_arl_hashed)) {
	cli_out("%s: No hash features on this chip\n", ARG_CMD(a));
	return CMD_FAIL;
    }

    if (SOC_IS_XGS3_SWITCH(unit)) {
	hashSels = xgs3hashSels;
    } else if (SOC_IS_XGS12_SWITCH(unit)) {
	hashSels = xgs2hashSels;
    } else {
	cli_out("%s: No hash features on this chip\n", ARG_CMD(a));
	return CMD_FAIL;
    }

    r = SOC_E_UNAVAIL;
#ifdef	BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
	uint32	hreg;
 	r = READ_HASH_CONTROLr(unit, &hreg);
        if (soc_reg_field_valid(unit,
                                HASH_CONTROLr,
                                L2_AND_VLAN_MAC_HASH_SELECTf)) {
            hash_sel = soc_reg_field_get(unit,
                                         HASH_CONTROLr,
                                         hreg, L2_AND_VLAN_MAC_HASH_SELECTf);
        } else {
            cli_out("%s: feature not available on this device\n",
                    ARG_CMD(a));
            return CMD_FAIL;
        }
    }
#endif
#ifdef	BCM_XGS12_SWITCH_SUPPORT
    if (SOC_IS_XGS12_SWITCH(unit)) {
	r = soc_draco_hash_get(unit, &hash_sel);
    }
#endif

    if (r < 0) {
	cli_out("%s: Error getting hash select: %s\n",
                ARG_CMD(a), soc_errmsg(r));
	return CMD_FAIL;
    }

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "HashSelect", PQ_DFL|PQ_MULTI,
		    (void *)0, &hash_sel, hashSels);

    if (!ARG_CNT(a)) {			/* Display settings */
	cli_out("Current settings:\n");
	cli_out("  HashSelect=%s\n", hashSels[hash_sel]);
	parse_arg_eq_done(&pt);
	return(CMD_OK);
    }

    if (parse_arg_eq(a, &pt) < 0) {
	cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
	parse_arg_eq_done(&pt);
	return(CMD_FAIL);
    }
    parse_arg_eq_done(&pt);

    r = SOC_E_UNAVAIL;
#ifdef	BCM_XGS3_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
	uint32	hreg;
 	r = READ_HASH_CONTROLr(unit, &hreg);
	soc_reg_field_set(unit, HASH_CONTROLr, &hreg,
			  L2_AND_VLAN_MAC_HASH_SELECTf, hash_sel);
	if (r >= 0) {
	    r = WRITE_HASH_CONTROLr(unit, hreg);
	}
    }
#endif
#ifdef	BCM_XGS12_SWITCH_SUPPORT
    if (SOC_IS_XGS12_SWITCH(unit)) {
	r = soc_draco_hash_set(unit, hash_sel);
    }
#endif
    if (r < 0) {
	cli_out("%s: Error setting hash select: %s\n",
                ARG_CMD(a), soc_errmsg(r));
	return CMD_FAIL;
    }

    return CMD_OK;
}

#endif /* BCM_XGS_SWITCH_SUPPORT */

/****************************************************************
 * Interrupt enable/disable commands.
 */
struct  _irq_cmic_stat_info {
    soc_field_t field;
    char *name;
};
static struct _irq_cmic_stat_info irq_info[] = {
    { BSAFE_OP_DONEf,           "BsafeOpDone" },
    { BSE_CMDMEM_DONEf,         "BseCmdDone" },
    { CH0_CHAIN_DONEf,          "ChainDone0" },
    { CH0_DESC_DONEf,           "DescDone0" },
    { CH1_CHAIN_DONEf,          "ChainDone1" },
    { CH1_DESC_DONEf,           "DescDone1" },
    { CH2_CHAIN_DONEf,          "ChainDone2" },
    { CH2_DESC_DONEf,           "DescDone2" },
    { CH3_CHAIN_DONEf,          "ChainDone3" },
    { CH3_DESC_DONEf,           "DescDone3" },
    { CHIP_FUNC_INTR_0f,        "ChipFuncIntr0" },
    { CHIP_FUNC_INTR_1f,        "ChipFuncIntr1" },
    { CHIP_FUNC_INTR_2f,        "ChipFuncIntr2" },
    { CHIP_FUNC_INTR_3f,        "ChipFuncIntr3" },
    { CHIP_FUNC_INTR_4f,        "ChipFuncIntr4" },
    { CHIP_FUNC_INTR_5f,        "ChipFuncIntr5" },
    { CHIP_FUNC_INTR_6f,        "ChipFuncIntr6" },
    { CHIP_FUNC_INTR_7f,        "ChipFuncIntr7" },
    { CSE_CMDMEM_DONEf,         "CseCmdDone" },
#if defined(BCM_HB_GW_SUPPORT)
    { EP_INTRf,                 "EpIntr" },
#endif
    { FIFO_CH0_DMA_INTRf,       "FifoDmaIntr0" },
    { FIFO_CH1_DMA_INTRf,       "FifoDmaIntr1" },
    { FIFO_CH2_DMA_INTRf,       "FifoDmaIntr2" },
    { FIFO_CH3_DMA_INTRf,       "FifoDmaIntr3" },
    { HSE_CMDMEM_DONEf,         "HseCmdDone" },
    { I2C_INTRf,                "I2Cintr" },
#if defined(BCM_HB_GW_SUPPORT)
    { IP_INTRf,                 "IpIntr" },
#endif
    { L2_MOD_FIFO_NOT_EMPTYf,   "L2ModFifoNotEmpty" },
    { LINK_STAT_MODf,           "LinkStatMod" },
    { MEM_FAILf,                "MemFail" },
    { MIIM_OP_DONEf,            "MiimOpDone" },
#ifdef BCM_HERCULES_SUPPORT
    { MMU_GROUP_INTRf,          "MmuGroupIntr" },
#endif
    { PCI_FATAL_ERRf,           "PciFatalErr" },
    { PCI_PARITY_ERRf,          "PciParityErr" },
    { RX_PAUSE_STAT_MODf,       "RxPauseStatMod" },
    { SCHAN_ERRf,               "SchanErr" },
    { SCH_MSG_DONEf,            "SchMsgDone" },
    { SLAM_DMA_COMPLETEf,       "SlamDmaComplete" },
    { STATS_DMA_ITER_DONEf,     "StatIterDone" },
    { STAT_DMA_DONEf,           "StatDmaDone" },
#ifdef BCM_HERCULES_SUPPORT
    { STATS_DMA_DONEf,          "StatDmaDone" },
#endif
    { TABLE_DMA_COMPLETEf,      "TableDmaComplete" },
    { TX_PAUSE_STAT_MODf,       "TxPauseStatMod" },
    { BROADSYNC_INTERRUPTf,     "Broadsync" }
};

#ifdef BCM_CMICM_SUPPORT
static struct _irq_cmic_stat_info irq_cmic_stat0_info[] = { /* array of all possible fields of all supported devices */
    { SBUSDMA_ECCERRf,                    "SbusDmaEccErr" },
    { SW_INTRf,                           "SwIntr" },
    { CROSS_COUPLED_MEMORYDMA_DONEf,      "CrossCoupledmemDMADone" },
    { SCHAN_OP_DONEf,                     "SchanDone0" },
    { CH0_INTR_COALESCING_INTRf,          "Coalescing0" },
    { CH1_INTR_COALESCING_INTRf,          "Coalescing1" },
    { CH2_INTR_COALESCING_INTRf,          "Coalescing2" },
    { CH3_INTR_COALESCING_INTRf,          "Coalescing3" },
    { CH0_CHAIN_DONEf,                    "ChainDone0" },
    { CH0_DESC_DONEf,                     "DescDone0" },
    { CH1_CHAIN_DONEf,                    "ChainDone1" },
    { CH1_DESC_DONEf,                     "DescDone1" },
    { CH2_CHAIN_DONEf,                    "ChainDone2" },
    { CH2_DESC_DONEf,                     "DescDone2" },
    { CH3_CHAIN_DONEf,                    "ChainDone3" },
    { CH3_DESC_DONEf,                     "DescDone3" },
    { MIIM_OP_DONEf,                      "MiimOpDone" },
    { SBUSDMA_CH2_DONEf,                  "SbusDmaDone2" },
    { STATS_DMA_ITER_DONEf,               "StatsDmaIterDone" },
    { FIFO_CH0_DMA_INTRf,                 "FifoDma0" },
    { FIFO_CH1_DMA_INTRf,                 "FifoDma1" },
    { FIFO_CH2_DMA_INTRf,                 "FifoDma2" },
    { FIFO_CH3_DMA_INTRf,                 "FifoDma3" },
    { SBUSDMA_CH0_DONEf,                  "SbusDmaDone0" },
    { SBUSDMA_CH1_DONEf,                  "SbusDmaDone1" },
    { TABLE_DMA_DONEf,                    "TableDmaDone" },
    { SLAM_DMA_DONEf,                     "SlamDmaDone" },
    { CH0_DESC_CONTROLLED_INTRf,          "CntldDescDone0" },
    { CH1_DESC_CONTROLLED_INTRf,          "CntldDescDone1" },
    { CH2_DESC_CONTROLLED_INTRf,          "CntldDescDone2" },
    { CH3_DESC_CONTROLLED_INTRf,          "CntldDescDone3" },
};


static struct _irq_cmic_stat_info irq_cmic_stat1_info[] = { /* array of all possible fields of all supported devices */
    { SER_INTRf,                             "SerIntr" },
    { SRAM_ECC_INTRf,                        "SRAMEcc" },
    { PCIEINTF_NEEDS_CLEANUPf,               "PCIeCleanup" },
    { TIM1_INTR2f,                           "Tim1_2" },
    { TIM1_INTR1f,                           "Tim1_1" },
    { TIM0_INTR2f,                           "Tim0_2" },
    { TIM0_INTR1f,                           "Tim0_1" },
    { WDT_1_INTRf,                           "WDT_1" },
    { WDT_0_INTRf,                           "WDT_0" },
    { UC_1_PMUIRQf,                          "Uc_1" },
    { UC_0_PMUIRQf,                          "Uc_0" },
    { CHIP_FUNC_INTRf,                       "ChipFunc" },
    { GPIO_INTRf,                            "Gpio" },
    { MIIM_OP_DONEf,                         "MiimPhy" },
    { COMMON_SCHAN_DONEf,                    "CommonSchanDone" },
    { UART1_INTERRUPTf,                      "UART1" },
    { UART0_INTERRUPTf,                      "UART0" },
    { SPI_INTERRUPTf,                        "SPI" },
    { PHY_PAUSESCAN_PAUSESTATUS_CHDf,        "PauseChanged" },
    { PHY_LINKSCAN_LINKSTATUS_CHDf,          "PHYLinkChanged" },
    { TIMESYNC_INTRf,                        "TimeSync" },
    { PCIE_ECRC_ERR_INTRf,                   "PCIeEcrcErr" },
    { BROADSYNC_INTRf,                       "BroadSyncIntr" },
    { I2C_INTRf,                             "I2C" }
};

#ifdef BCM_XGS_SUPPORT
static struct _irq_cmic_stat_info irq_cmic_stat2_info[] = {
    { CHIP_PARITY_INTERUPT_STATUSf,          "ChipParityIntr" }
};
#endif

#endif /* BCM_CMICM_SUPPORT */
char cmd_intr_usage[] =
    "Usage:\n"
    " intr enable  [cmc <cmcId> stat <statId>] [<mask>/<irq_name> ...]  - Enable interrupts\n"
    " intr disable [cmc <cmcId> stat <statId>] [<mask>/<irq_name> ...]  - Disable interrupts\n"
    " intr mask    [cmc <cmcId> stat <statId>]                          - Show current mask reg\n"
    " intr pending [cmc <cmcId> stat <statId>]                          - Show current state\n"
    " intr names   [cmc <cmcId> stat <statId>]                          - List interrupt names\n"
    "Note:\n"
    " Valid cmcId range  : 0-2\n"
    " Valid statId range : 0-2\n";


static int parse_irq_list(args_t *a, char **irq_names, uint32 *irq_masks,
                          int count, uint32 *mask)
{
    char *intr;
    int i;

    if ((intr = ARG_GET(a)) == NULL) {
        return -1;
    }

    if ((*mask = sal_ctoi(intr, 0)) == 0) { /* Assume arg list */
        do {
            for (i = 0; i < count; i++) {
                if (parse_cmp(irq_names[i], intr, '\0')) {
                    break;
                }
            }
            if (i == count) {
                return -1;
            }
            *mask |= irq_masks[i];
        } while ((intr = ARG_GET(a)) != NULL);
    }
    return 0;
}


#ifdef BCM_CMICM_SUPPORT
cmd_result_t
cmd_cmicm_intr(int unit,args_t *a)
{
     int cmcId = 0;
     int count = 0;
     int statId = 0;
     int intrCnt = 0;
     int intrName = 0;
     int regName = 0;
     int maskName = 0;
     int hwFieldCnt = 0;
     int swFieldCnt = 0;
     int statArrayCnt = 0;
     int start = 0;
     int end = 0;
    char *subcmd = NULL;
    char *irq_names[33];
    char *cmd_option = NULL;
    uint32 mask = 0, temp_mask = 0;
    uint32 omask = 0;
    uint32 irq_masks[33];
    soc_reg_info_t   *reg_info = NULL;
    soc_field_info_t *field_info = NULL;
    struct _irq_cmic_stat_info *irqInfo = NULL;
    struct _irq_cmic_stat_info *tmpIrqInfo = NULL;
    uint32 (*pIntrEnaFunc)(int, int, uint32) = NULL;
    uint32 (*pIntrDisFunc)(int, int, uint32) = NULL;


    if (!(SOC_REG_IS_VALID (unit, CMIC_IRQ_STATr)) && (ARG_CNT(a) <= 4)) {
        cli_out("Options cmc,stat must be provided\n");
        return CMD_USAGE;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    if (!(parse_cmp("Enable", subcmd, '\0')) &&
            !(parse_cmp("Disable", subcmd, '\0')) &&
            !(parse_cmp("Mask", subcmd, '\0')) &&
            !(parse_cmp("Pending", subcmd, '\0')) &&
            !(parse_cmp("Names", subcmd, '\0'))) {
        return CMD_USAGE;
    }

    cmd_option = subcmd;

    /* Expected subcmd :- cmc */
    subcmd = ARG_GET(a);
    if (subcmd == NULL) {
        return CMD_USAGE;
    } else {
        if (!(parse_cmp("cmc", subcmd, '\0'))) {
            return CMD_USAGE;
        }
    }

    /* Expected subcmd :- <cmcId> */
    subcmd = ARG_GET(a);
    if (subcmd == NULL) {
        return CMD_USAGE;
    } else {
        cmcId = parse_integer (subcmd);
    }

    /* Expected subcmd :- stat */
    subcmd = ARG_GET(a);
    if (subcmd == NULL) {
        return CMD_USAGE;
    } else {
        if (!(parse_cmp("stat", subcmd, '\0'))) {
            return CMD_USAGE;
        }
    }

    /* Expected subcmd :- <statId> */
    subcmd = ARG_GET(a);
    if (subcmd == NULL) {
        return CMD_USAGE;
    } else {
        statId = parse_integer (subcmd);
    }

    switch (statId) {
    case 0:
        switch (cmcId) {
        case 0:
            regName  = CMIC_CMC0_IRQ_STAT0r;
            break;
        case 1:
            regName  = CMIC_CMC1_IRQ_STAT0r;
            break;
        case 2:
            regName  = CMIC_CMC2_IRQ_STAT0r;
            break;
        default:
            return CMD_USAGE;
        }

        irqInfo = irq_cmic_stat0_info;
        statArrayCnt = sizeof(irq_cmic_stat0_info)
                          / sizeof(irq_cmic_stat0_info[0]);
        pIntrEnaFunc = soc_cmicm_cmcx_intr0_enable;
        pIntrDisFunc = soc_cmicm_cmcx_intr0_disable;
        intrName = CMIC_CMCx_IRQ_STAT0_OFFSET(cmcId);
        maskName = CMIC_CMCx_PCIE_IRQ_MASK0_OFFSET(cmcId);
        break;

    case 1:
        switch (cmcId) {
        case 0:
            regName  = CMIC_CMC0_IRQ_STAT1r;
            break;
        case 1:
            regName  = CMIC_CMC1_IRQ_STAT1r;
            break;
        case 2:
            regName  = CMIC_CMC2_IRQ_STAT1r;
            break;
        default:
            return CMD_USAGE;
        }

        irqInfo = irq_cmic_stat1_info;
        statArrayCnt = sizeof(irq_cmic_stat1_info)
                          / sizeof(irq_cmic_stat1_info[0]);
        pIntrEnaFunc = soc_cmicm_cmcx_intr1_enable;
        pIntrDisFunc = soc_cmicm_cmcx_intr1_disable;
        intrName = CMIC_CMCx_IRQ_STAT1_OFFSET(cmcId);
        maskName = CMIC_CMCx_PCIE_IRQ_MASK1_OFFSET(cmcId);
        break;

    case 2:
#ifdef BCM_XGS_SUPPORT
        if (SOC_IS_XGS(unit)) {
            switch (cmcId) {
            case 0:
                regName = CMIC_CMC0_IRQ_STAT2r;
                break;
            case 1:
                regName = CMIC_CMC1_IRQ_STAT2r;
                break;
            case 2:
                regName = CMIC_CMC2_IRQ_STAT2r;
                break;
            default:
                return CMD_USAGE;
            }
            irqInfo = irq_cmic_stat2_info;
            statArrayCnt = sizeof(irq_cmic_stat2_info)
                                / sizeof(irq_cmic_stat2_info[0]);
            pIntrEnaFunc = soc_cmicm_cmcx_intr2_enable;
            pIntrDisFunc = soc_cmicm_cmcx_intr2_disable;
            intrName = CMIC_CMCx_IRQ_STAT2_OFFSET(cmcId);
            maskName = CMIC_CMCx_PCIE_IRQ_MASK2_OFFSET(cmcId);
            break;
        } else
#endif
        {
            return CMD_USAGE;
        }
    default :
        return CMD_USAGE;
    }

    mask = 0;
    count = 0;

    if (!(SOC_REG_IS_VALID (unit, regName)) ) {
        return CMD_FAIL;
    }

    reg_info = &SOC_REG_INFO(unit, regName);

    /* Compare H/W and S/W IRQ_STAT fields and update irq_names, irq_mask*/
    for (hwFieldCnt = 0; hwFieldCnt < reg_info->nFields; hwFieldCnt++) {
        field_info = &reg_info->fields[reg_info->nFields - 1 - hwFieldCnt];

        if (field_info->flags & SOCF_RES) {
            continue;
        }

        tmpIrqInfo = irqInfo;

        for (swFieldCnt = 0; swFieldCnt < statArrayCnt ; swFieldCnt++) {
            if (tmpIrqInfo->field == field_info->field) {
                irq_names[count] = tmpIrqInfo->name;
                break;
            }
            tmpIrqInfo++;
        }

        if (swFieldCnt == statArrayCnt) {
            cli_out("Could not parse %s (0x%08x)\n"
                    "It needs to be added to the list of interrupt names\n",
                    SOC_FIELD_NAME(unit, field_info->field),
                    1 << field_info->bp);
            irq_names[count] = SOC_FIELD_NAME(unit, field_info->field);
        }
        start = field_info->bp;
        end = field_info->bp + field_info->len;
        if (end < 32) {
            temp_mask = (1 << end) - 1;
        } else {
            temp_mask = -1;
        }
        temp_mask = ((uint32)-1 << start) & temp_mask;
        irq_masks[count] = temp_mask;
        mask |= irq_masks[count];
        count++;
    }

    irq_names[count] = "ALL";
    irq_masks[count] = mask;
    count++;

    /* Handler for Sub Commands
     * ========================
     */

    /* INTR Enable  [cmc <cmcId> stat <statId>]  [<mask> | <irq_name> ...] */
    if (parse_cmp("Enable", cmd_option, '\0')) {
        if (parse_irq_list(a, irq_names, irq_masks, count, &mask)) {
            cli_out("Invalid Interupt Name/Value.\n");
            return CMD_USAGE;
        }
        omask = pIntrEnaFunc (unit, cmcId, mask);
        cli_out("Enabled with mask 0x%08x.  "
                "Mask was 0x%08x\n", mask, omask);
    /*  INTR Disable [cmc <cmcId> stat <statId>]  [<mask> | <irq_name> ...] */
    } else if (parse_cmp("Disable", cmd_option , '\0')) {
        if (parse_irq_list(a, irq_names, irq_masks, count, &mask)) {
            cli_out("Invalid Interupt Name/Value.\n");
            return CMD_USAGE;
        }
        omask = pIntrDisFunc (unit, cmcId, mask);
        cli_out("Disabled with mask 0x%08x.  "
                "Mask was 0x%08x\n", mask, omask);
     /* INTR Mask    [cmc <cmcId> stat <statId>]  */
    } else if (parse_cmp("Mask", cmd_option, '\0')) {
        mask = soc_pci_read(unit, maskName);
        if (mask) {
            cli_out("Current interrupt control reg: 0x%08x\n", mask);
            cli_out("Interrupts enabled for the following:\n");
            for (intrCnt = 0; intrCnt < count - 1; intrCnt++) {
                if (mask & irq_masks[intrCnt]) {
                    cli_out("%-30s\t\n", irq_names[intrCnt]);
                    mask &= ~irq_masks[intrCnt];
                }
            }
            if (mask) {
                cli_out("Warning:  Unknown interrupts are enabled: 0x%08x\n",
                        mask);
            }
        } else {
            cli_out("No interrupts enabled\n");
        }
    /* INTR Pending [cmc <cmcId> stat <statId>] */
    } else if (parse_cmp("Pending", cmd_option, '\0')) {
        mask = soc_pci_read(unit, intrName);
        if (mask) {
            cli_out("Current interrupt status reg: 0x%08x\n", mask);
            cli_out("The following interrupts are pending:\n");
            for (intrCnt = 0; intrCnt < count - 1; intrCnt++) {
                if (mask & irq_masks[intrCnt]) {
                    cli_out("%-30s\t\n", irq_names[intrCnt]);
                    mask &= ~irq_masks[intrCnt];
                }
            }
            if (mask) {
                cli_out("Warning:  Unknown interrupts are pending: 0x%08x\n",
                        mask);
            }
        } else {
            cli_out("No interrupts pending\n");
        }
    /* INTR Names   [cmc <cmcId> stat <statId>] */
    } else if (parse_cmp("Names", cmd_option, '\0')) {
        cli_out("%-30s   %s\n", "Name", "Mask");
        for (intrCnt = 0; intrCnt < count; intrCnt++) {
            cli_out("%-30s0x%08x\n", irq_names[intrCnt], irq_masks[intrCnt]);
        }
    } else {
        return CMD_USAGE;
    }
    return CMD_OK;
}
#endif /* BCM_CMICM_SUPPORT */

cmd_result_t
cmd_intr(int unit, args_t *a)
{
    char *subcmd;
    uint32  mask = 0;
    uint32  omask;
    int i, j, count;
    char *irq_names[33];
    uint32 irq_masks[33];
    soc_reg_info_t *reg_info;
    soc_field_info_t *field_info;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

#ifdef BCM_CMICM_SUPPORT
    /* Call cmd_cmicm_intr function if CMICm is supported*/
    if (soc_feature(unit, soc_feature_cmicm)) {
        return cmd_cmicm_intr (unit, a);
    }
#endif /* BCM_CMICM_SUPPORT */

    if (!(SOC_REG_IS_VALID (unit, CMIC_IRQ_STATr))) {
        return CMD_FAIL;
    }

    if ((subcmd = ARG_GET(a)) == NULL) {
        return CMD_USAGE;
    }

    reg_info = &SOC_REG_INFO(unit, CMIC_IRQ_STATr);
    mask = 0;
    count = 0;
    for (i = 0; i < reg_info->nFields; i++) {
        field_info = &reg_info->fields[reg_info->nFields - 1 - i];
        if (field_info->flags & SOCF_RES) {
            continue;
        }
        for (j = 0; j < sizeof(irq_info) / sizeof(irq_info[0]); j++) {
            if (irq_info[j].field == field_info->field) {
                irq_names[count] = irq_info[j].name;
                break;
            }
        }
        if (j == sizeof(irq_info) / sizeof(irq_info[0])) {
            cli_out("Could not parse %s (0x%08x)\n"
                    "It needs to be added to the list of interrupt names\n",
                    SOC_FIELD_NAME(unit, field_info->field),
                    1 << field_info->bp);
            irq_names[count] = SOC_FIELD_NAME(unit, field_info->field);
        }
        irq_masks[count] = 1 << field_info->bp;
        mask |= irq_masks[count];
        count++;
    }
    irq_names[count] = "ALL";
    irq_masks[count] = mask;
    count++;

    if (parse_cmp("Enable", subcmd, '\0')) {
        if (parse_irq_list(a, irq_names, irq_masks, count, &mask)) {
            cli_out("Invalid Interrupt Name/Value.\n");
            cli_out("Note: cmc,stat options are not supported "
                    "in this device\n");
            return CMD_USAGE;
        }
        omask = soc_intr_enable(unit, mask);
         cli_out("Enabled with mask 0x%08x.  "
                 "Mask was 0x%08x\n", mask, omask);
    } else if (parse_cmp("Disable", subcmd, '\0')) {
        if (parse_irq_list(a, irq_names, irq_masks, count, &mask)) {
            cli_out("Invalid Interrupt Name/Value.\n");
            cli_out("Note: cmc,stat options are not supported "
                    "in this device\n");
            return CMD_USAGE;
        }
        omask = soc_intr_disable(unit, mask);
         cli_out("Disabled with mask 0x%08x.  "
                 "Mask was 0x%08x\n", mask, omask);
    } else if (parse_cmp("Mask", subcmd, '\0')) {
        mask = soc_pci_read(unit, CMIC_IRQ_MASK);
        if (mask) {
            cli_out("Current interrupt control reg: 0x%08x\n", mask);
            cli_out("Interrupts enabled for the following:\n");
            for (i = 0; i < count - 1; i++) {
                if (mask & irq_masks[i]) {
                    cli_out("%-30s\t\n", irq_names[i]);
                    mask &= ~irq_masks[i];
                }
            }
            if (mask) {
                cli_out("Warning:  Unknown interrupts are enabled: 0x%08x\n",
                        mask);
            }
        } else {
            cli_out("No interrupts enabled\n");
        }
    } else if (parse_cmp("Pending", subcmd, '\0')) {
        mask = soc_pci_read(unit, CMIC_IRQ_STAT);
        if (mask) {
            cli_out("Current interrupt status reg: 0x%08x\n", mask);
            cli_out("The following interrupts are pending:\n");
            for (i = 0; i < count - 1; i++) {
                if (mask & irq_masks[i]) {
                    cli_out("%-30s\t\n", irq_names[i]);
                    mask &= ~irq_masks[i];
                }
            }
            if (mask) {
                cli_out("Warning:  Unknown interrupts are pending: 0x%08x\n",
                        mask);
            }
        } else {
            cli_out("No interrupts pending\n");
        }
    } else if (parse_cmp("Names", subcmd, '\0')) {
        cli_out("%-30s   %s\n", "Name", "Mask");
        for (i = 0; i < count; i++) {
            cli_out("%-30s0x%08x\n", irq_names[i], irq_masks[i]);
        }
    } else {
        return CMD_USAGE;
    }

    return CMD_OK;
}
