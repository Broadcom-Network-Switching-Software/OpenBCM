/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Rate CLI commands
 *
 * Note: the "rate" controls Storm Control, as opposed to packet rate
 * control (bcm_port_rate_*).
 */

#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#include <appl/diag/dport.h>
#include <shared/bsl.h>
#include <bcm/init.h>
#include <bcm/error.h>
#include <bcm/rate.h>
#include <bcm/debug.h>

#include <soc/drv.h>
#define _DFT_MAGIC_NUMBER -123456798

/*
 * Manage packet rate controls
 */
cmd_result_t
cmd_esw_rate(int unit, args_t *a)
{
    int limit = _DFT_MAGIC_NUMBER; /* magic number */
    int	fRateBCast;
    int fRateMCast;
    int fRateDLF;
    int fmask = 0;
    int flags = 0;
    int r;
    parse_table_t pt;
    bcm_pbmp_t pbmp;
    bcm_port_config_t pcfg;
    int limitGet = FALSE;
    int bRateSet, mRateSet,dlfRateSet;
    int bRateFlGet, mRateFlGet, dlfRateFlGet;
    int bRateLmGet, mRateLmGet, dlfRateLmGet;
    int displayOnly = 0;
    soc_port_t p, dport;

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return CMD_FAIL;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    pbmp       = pcfg.all;  /* All ports by default */
    limit      = _DFT_MAGIC_NUMBER;
    fRateBCast = _DFT_MAGIC_NUMBER;
    fRateMCast = _DFT_MAGIC_NUMBER;
    fRateDLF   = _DFT_MAGIC_NUMBER;

    if (ARG_CNT(a)) {
        parse_table_init(unit, &pt);
        parse_table_add(&pt, "PortBitMap", PQ_DFL|PQ_PBMP|PQ_BCM,
                    (void *)0, &pbmp, 0);
        parse_table_add(&pt, "Limit", PQ_DFL|PQ_HEX,
                    INT_TO_PTR(limit) , &limit, 0);
        parse_table_add(&pt, "Bcast", PQ_DFL|PQ_BOOL,
                    (void *)0, &fRateBCast, 0);
        parse_table_add(&pt, "Mcast",        PQ_BOOL,
                    INT_TO_PTR(fRateMCast), &fRateMCast, 0);
        parse_table_add(&pt, "Dlf", PQ_DFL|PQ_BOOL,
                    INT_TO_PTR(fRateDLF), &fRateDLF, 0);

        if (parse_arg_eq(a, &pt) < 0) {
            cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return(CMD_FAIL);
        }
        parse_arg_eq_done(&pt);
        fmask = flags = 0;   

        /* Verify what is being changed */
        if (fRateBCast != _DFT_MAGIC_NUMBER) {
            fmask |= BCM_RATE_BCAST;
            if (fRateBCast == TRUE) {
                flags |= BCM_RATE_BCAST;
            }
        }
        if (fRateMCast != _DFT_MAGIC_NUMBER) {
            fmask |= BCM_RATE_MCAST;
            if (fRateMCast == TRUE) {
                flags |= BCM_RATE_MCAST;
            }
        }
        if (fRateDLF != _DFT_MAGIC_NUMBER) {
            fmask |= BCM_RATE_DLF;
            if (fRateDLF == TRUE) {
                flags |= BCM_RATE_DLF;
            }
        }
        if (limit < 0 && limit != _DFT_MAGIC_NUMBER) {
            cli_out("Error: Negative rate limit: %d is not " 
                    "allowed\n", (int)limit);
        }

        /* Check whether original value shall be read back */ 
        if (fmask == 0 && limit == _DFT_MAGIC_NUMBER) {
            /* only PBM is specified */
            displayOnly = TRUE;
        } else if (limit == _DFT_MAGIC_NUMBER) {
            limitGet = TRUE;
        }

    } else {
      displayOnly = TRUE;
    }

    if (displayOnly) {
        cli_out("Current settings:\n");

        DPORT_BCM_PBMP_ITER(unit, pbmp, dport, p) {
            cli_out("%4s:",BCM_PORT_NAME(unit, p));
            if ((r = bcm_rate_bcast_get(unit, &limit, &flags, p)) < 0) {
                cli_out(" Bcast = UNKNOWN, %s;", bcm_errmsg(r));
            } else if (flags == BCM_RATE_BCAST) {
                cli_out(" Bcast= TRUE,Limit=%d;", limit);
            } else {
                cli_out(" Bcast=FALSE ;");
            }
            if ((r = bcm_rate_mcast_get(unit, &limit, &flags, p)) < 0) {
                cli_out(" Mcast=UNKNOWN, %s;", bcm_errmsg(r));
            } else if (flags == BCM_RATE_MCAST) {
                cli_out(" Mcast= TRUE,Limit=%d;", limit);
            } else {
                cli_out(" Mcast=FALSE ;");
            }
            if ((r = bcm_rate_dlfbc_get(unit, &limit, &flags, p)) < 0) {
                cli_out(" Dlf=UNKNOWN, %s;", bcm_errmsg(r));
            } else if (flags == BCM_RATE_DLF) {
                cli_out(" Dlfbc= TRUE,Limit=%d;", limit);
            } else {
                cli_out(" Dlfbc=FALSE ;");
            }
             cli_out("\n");
        }
        return(CMD_OK);
    }

    bRateSet   = mRateSet   = dlfRateSet   = FALSE;
    bRateFlGet = mRateFlGet = dlfRateFlGet = FALSE;
    bRateLmGet = mRateLmGet = dlfRateLmGet = FALSE;
    fRateBCast = fRateMCast = fRateDLF     = 0;

    if (!fmask) { 
        if (limitGet == FALSE) {
            /* only new limit specified */
            bRateSet   = mRateSet   = dlfRateSet   = TRUE;
            bRateFlGet = mRateFlGet = dlfRateFlGet = TRUE;
        }
    } else {
        if ((fmask & BCM_RATE_BCAST)) {
            bRateSet = TRUE;
            fRateBCast = flags & BCM_RATE_BCAST;
            if (fRateBCast && limitGet) {
                bRateLmGet = TRUE;
            }
        } 
        if ((fmask & BCM_RATE_MCAST)) {
            mRateSet = TRUE;
            fRateMCast = flags & BCM_RATE_MCAST;
            if (fRateMCast && limitGet) {
                mRateLmGet = TRUE;
            }
        } 
        if ((fmask & BCM_RATE_DLF)) {
            dlfRateSet = TRUE;
            fRateDLF = flags & BCM_RATE_DLF;
            if (fRateDLF && limitGet) {
                dlfRateLmGet = TRUE;
            }
        } 
    }

    /* Set new config in ports */
    /* coverity[overrun-local] */
    DPORT_BCM_PBMP_ITER(unit, pbmp, dport, p) {
        /* Brocast meter */
        if (bRateSet) {
            int limitPro, flagPro;
            int oriLimit, oriFlag;
            int r = 0;

            if (bRateFlGet || bRateLmGet) {
                if ((r = bcm_rate_bcast_get(unit, &oriLimit, &oriFlag, p)) < 0) {
                    cli_out("%4s Error: can not get Brocast limit info: %s\n", 
                            BCM_PORT_NAME(unit, p), bcm_errmsg(r));
                }
             }

             limitPro = (!r && bRateLmGet)? oriLimit : limit;
             flagPro  = (!r && bRateFlGet)? oriFlag  : fRateBCast;

             if (!r && (r = bcm_rate_bcast_set(unit, 
                                        limitPro, flagPro, p)) < 0) {
                 cli_out("%4s Error: can not set Broadcast rate: %s\n", 
                         BCM_PORT_NAME(unit, p), bcm_errmsg(r));
             }
          }
        /* Mcast meter control */
        if (mRateSet) {
            int limitPro, flagPro;
            int oriLimit, oriFlag;
            int r = 0;

            if (mRateFlGet || mRateLmGet) {
                if ((r = bcm_rate_mcast_get(unit, &oriLimit, &oriFlag, p)) < 0) {
                    cli_out("%4s Error: can not get Mcast limit info: %s\n", 
                            BCM_PORT_NAME(unit, p), bcm_errmsg(r));
                }
             }

             limitPro = (!r && mRateLmGet)? oriLimit : limit;
             flagPro  = (!r && mRateFlGet)? oriFlag  : fRateMCast;

             if (!r && (r = bcm_rate_mcast_set(unit, 
                                        limitPro, flagPro, p)) < 0) {
                 cli_out("%4s Error: can not set Mcast rate: %s\n", 
                         BCM_PORT_NAME(unit, p), bcm_errmsg(r));
             }
          }
        /* Dlf meter */
        if (dlfRateSet) {
            int limitPro, flagPro;
            int oriLimit, oriFlag;
            int r = 0;

            if (dlfRateFlGet || dlfRateLmGet) {
                if ((r = bcm_rate_dlfbc_get(unit, &oriLimit, &oriFlag, p)) < 0) {
                    cli_out("%4s Error: can not get DLF limit info: %s\n", 
                            BCM_PORT_NAME(unit, p), bcm_errmsg(r));
                }
             }

             limitPro = (!r && dlfRateLmGet)? oriLimit : limit;
             flagPro  = (!r && dlfRateFlGet)? oriFlag  : fRateDLF;

             if (!r && (r = bcm_rate_dlfbc_set(unit, 
                                        limitPro, flagPro, p)) < 0) {
                 cli_out("%4s Error: can not set DLF rate: %s\n", 
                         BCM_PORT_NAME(unit, p), bcm_errmsg(r));
             }
          }
    }
    return CMD_OK;
}

/*
 * Function:
 *      cmd_bw_rate
 * Purpose:
 *      Set/display of bandwidth rate mitiring limits.
 * Parameters:
 *      unit - SOC unit #
 *      args - pointer to comand line arguments
 * Returns:
 *      CMD_OK/CMD_FAIL
 */
char cmd_bw_rate_usage[] =
    "Set/Display port bandwidth rate limiting.\n"
    "Parameters: <pbm> flags rate max_burst\n\t"
    "    flag is one of the bcast | mcast | dlf\n\t"
    "    rate and max_burst are values in kilobits (1000 bits) per second\n";
                            
cmd_result_t
cmd_bw_rate(int unit, args_t *a)
{
    bcm_pbmp_t      pbm;
    bcm_port_config_t pcfg;
    soc_port_t      p, dport;
    int             flags = 0;
    int             rv;
    uint32          rate = 0xFFFFFFFF;
    uint32          burst = 0xFFFFFFFF;
    char           *c;


    if (!sh_check_attached(ARG_CMD(a), unit)) {
        return CMD_FAIL;
    }

    /* Check for metering capabilities */
    if (! soc_feature(unit, soc_feature_storm_control)) {
        cli_out("%s: Error: bandwidth limit unavailable for this device\n", 
                ARG_CMD(a));
        return CMD_OK;
    }

    if (bcm_port_config_get(unit, &pcfg) != BCM_E_NONE) {
        cli_out("%s: Error: bcm ports not initialized\n", ARG_CMD(a));
        return CMD_FAIL;
    }

    if ((c = ARG_GET(a)) == NULL) {
        cli_out("%s: Error: unspecified port bitmap: %s\n",
                ARG_CMD(a), c);
        return CMD_FAIL;
    } else if (parse_bcm_pbmp(unit, c, &pbm) < 0) {
        cli_out("%s: Error: unrecognized port bitmap: %s\n",
                ARG_CMD(a), c);
        return CMD_FAIL;
    }

    /* Apply PortRate only to those ports which support it */
    BCM_PBMP_AND(pbm, pcfg.e);
    if (BCM_PBMP_IS_NULL(pbm)) {
        cli_out("No ports specified.\n");
        return CMD_OK;
    }
    
    /* retrieving the flags */
    if ((c = ARG_GET(a)) != NULL) {
        /* Get user provided flag */
        if (0 == sal_strncasecmp(c, "bcast", sal_strlen(c))){
            flags = BCM_RATE_BCAST;
        } else if (0 == sal_strncasecmp(c, "mcast", sal_strlen(c))) {
            flags = BCM_RATE_MCAST;
        } else if (0 == sal_strncasecmp(c, "dlf", sal_strlen(c))) {
            flags = BCM_RATE_DLF;
        } else {
            cli_out("%s: Error: unrecognized rate type: %s\n",
                    ARG_CMD(a), c);
            return CMD_FAIL;
        }
    }
    else {
        cli_out("%s: Error: missing rate type: %s need bcast or mcast or dlf\n",
                ARG_CMD(a), c);
        return CMD_FAIL;
    }

    /* Set or get */
    if ((c = ARG_GET(a)) == NULL) {     /* Get part */
        /* coverity[overrun-local] */
        DPORT_BCM_PBMP_ITER(unit, pbm, dport, p) {
            rv = bcm_rate_bandwidth_get(unit, p, flags, &rate, &burst);
            if (rv < 0 ) {
                cli_out("%s port %s: ERROR: bcm_rate_bandwidth_get: "
                        "%s\n",
                        ARG_CMD(a), BCM_PORT_NAME(unit, p), bcm_errmsg(rv));
                return CMD_FAIL;
            }
            if (rate) {
                cli_out("%4s:", BCM_PORT_NAME(unit, p));
                if (rate < 64) { 
                    cli_out("\tBandwidth meter: ? kbps ? kbits max burst.\n");
                } else {
                    cli_out("\tBandwidth meter: "
                            "%8d kbps %8d kbits max burst.\n",
                            rate, burst);
                }
            }
        }
    } else {      /* Set Part */
        rate = parse_integer(c);
        if ((c = ARG_GET(a)) != NULL) {
            burst = parse_integer(c);
        } else {
            cli_out("%s: Error: missing port burst size\n",
                    ARG_CMD(a));
            return CMD_FAIL;
        }
        DPORT_BCM_PBMP_ITER(unit, pbm, dport, p) {
            rv = bcm_rate_bandwidth_set(unit, p, flags, rate, burst); 
            if (rv < 0) {
                cli_out("%s: ERROR: bcm_rate_bandwidth_set: %s\n",
                        ARG_CMD(a), bcm_errmsg(rv));
                return CMD_FAIL;
            }
        }
    }

    return CMD_OK;
}

