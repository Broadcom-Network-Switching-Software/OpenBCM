/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Diag CLI Buffer command
 */

#include <soc/defs.h>
#include <shared/bsl.h>
#if defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_KATANA_SUPPORT)

#include <appl/diag/system.h>
#include <appl/diag/parse.h>

#include <bcm/error.h>

#include <soc/triumph3.h>
#include <soc/trident2.h>
#include <soc/katana2.h>
#include <soc/katana.h>
#include <bcm_int/esw/katana.h>
#include <bcm_int/esw/katana2.h>
#if defined(BCM_HELIX5_SUPPORT)
#include <bcm_int/esw/helix5.h>
#endif
#if defined(BCM_FIREBOLT6_SUPPORT)
#include <bcm_int/esw/firebolt6.h>
#endif
#if defined(BCM_APACHE_SUPPORT)
#include <bcm_int/esw/apache.h>
#endif
#if defined(BCM_MONTEREY_SUPPORT)
#include <bcm_int/esw/monterey.h>
#endif
#if defined(BCM_HURRICANE4_SUPPORT)
#include <bcm_int/esw/hurricane4.h>
#endif
#include <soc/apache.h>
#include <soc/monterey.h>

char cmd_lls_usage[] =
    "Usages:\n\t"
    "  lls [PortBitMap=<pbm>]\n"
    "        - Show the schedule tree(s) for the LLS port(s)\n";

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
char cmd_hsp_usage[] =
    "Usages:\n\t"
    "  hsp [PortBitMap=<pbm>]\n"
    "        - Show the schedule tree(s) for the HSP port(s)\n";
#endif /* BCM_TRIDENT2_SUPPORT */

cmd_result_t
cmd_lls(int unit, args_t *a)
{
    int p;
#if defined(BCM_APACHE_SUPPORT) || defined(BCM_SABER2_SUPPORT)
    int subcmd = 0;
#endif
    soc_pbmp_t		pbmp;
    parse_table_t	pt;
#ifdef BCM_KATANA2_SUPPORT
    soc_info_t *si = &SOC_INFO(unit);
    int pp_port;
    int start_pp_port, end_pp_port;
#endif

    SOC_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "PortBitMap", 	PQ_DFL|PQ_PBMP,
		    (void *)(0), &pbmp, NULL);

#if defined(BCM_APACHE_SUPPORT) || defined(BCM_SABER2_SUPPORT)
    if (SOC_IS_APACHE(unit) || SOC_IS_SABER2(unit)) {
        parse_table_add(&pt, "sw", PQ_DFL | PQ_NO_EQ_OPT | PQ_BOOL, 0,
                            (void *)&subcmd, 0);
    }
#endif
    if (parse_arg_eq(a, &pt) < 0) {
	cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
	parse_arg_eq_done(&pt);
	return(CMD_FAIL);
    }
    parse_arg_eq_done(&pt);

    PBMP_ITER(pbmp, p) {
        if (SOC_IS_TRIUMPH3(unit)) {
            soc_tr3_dump_port_lls(unit, p);
        } else if (SOC_IS_TOMAHAWKX(unit)) {
            cli_out("LLS command not supported on this device\n");
            return CMD_FAIL;
#if defined(BCM_MONTEREY_SUPPORT)
        } else if (SOC_IS_MONTEREY(unit)) {
	     return(CMD_OK);
#endif
#if defined(BCM_APACHE_SUPPORT)
        } else if (SOC_IS_APACHE(unit)) {
            if (subcmd) {
                bcm_apache_dump_port_lls_sw(unit, p);
            } else {
                soc_apache_dump_port_lls(unit, p);
            }
#endif
        } else if (SOC_IS_TD2_TT2(unit)) {
            soc_td2_dump_port_lls(unit, p);
        }
#ifdef BCM_KATANA2_SUPPORT
        else if (SOC_IS_KATANA2(unit)) {
#if defined(BCM_SABER2_SUPPORT)
            if (SOC_IS_SABER2(unit) && (subcmd)) {
                /* Display LLS details from SW */
                bcm_kt2_dump_port_lls(unit, p);
            } else
#endif
            if (SOC_PBMP_MEMBER(si->linkphy_pbm, p)) {
                bcm_kt2_dump_port_lls(unit, p);
            } else {
#if defined BCM_METROLITE_SUPPORT
                if (soc_feature(unit, soc_feature_discontinuous_pp_port)) {
                    soc_kt2_dump_port_lls(unit, p, 0);
                    if (si->port_num_subport[p] > 0) {
                        SOC_PBMP_ITER(SOC_PORT_PP_BITMAP(unit, p), pp_port) {
                            soc_kt2_dump_port_lls(unit, p, pp_port);
                        }
                    }
                } else
#endif
                {
                    soc_kt2_dump_port_lls(unit, p, 0);
                    if (si->port_num_subport[p] > 0) {
                        start_pp_port = si->port_subport_base[p];
                        end_pp_port = si->port_subport_base[p] +
                            si->port_num_subport[p];
                        for (pp_port = start_pp_port; pp_port <
                                    end_pp_port; pp_port++) {
                            soc_kt2_dump_port_lls(unit, p, pp_port);
                        }
                    }
                }
            }
        }
#endif
#ifdef BCM_KATANA_SUPPORT
        else if (SOC_IS_KATANA(unit)) {
            bcm_kt_dump_port_lls(unit, p);
        }
#endif
        sal_thread_yield();
    }

    return CMD_OK;
}

#if defined(BCM_TRIDENT2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT) || defined(BCM_HELIX5_SUPPORT)
cmd_result_t
cmd_hsp(int unit, args_t *a)
{
    int p;
    soc_pbmp_t          pbmp;
    parse_table_t       pt;
#if defined(BCM_FIREBOLT6_SUPPORT)
    int subcmd = 0;
#endif

    SOC_PBMP_ASSIGN(pbmp, PBMP_PORT_ALL(unit));

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "PortBitMap",  PQ_DFL|PQ_PBMP,
                    (void *)(0), &pbmp, NULL);

#if defined(BCM_FIREBOLT6_SUPPORT)
    if (SOC_IS_FIREBOLT6(unit)) {
        parse_table_add(&pt, "subport", PQ_DFL|PQ_INT , 0,
                            (void *)&subcmd, 0);
    }
#endif
    if (parse_arg_eq(a, &pt) < 0) {
        cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
        parse_arg_eq_done(&pt);
        return(CMD_FAIL);
    }
    parse_arg_eq_done(&pt);

    PBMP_ITER(pbmp, p) {
        if (SOC_IS_TOMAHAWKX(unit)) {
            cli_out("HSP command not supported on this device\n");
            return CMD_FAIL;
#if defined(BCM_MONTEREY_SUPPORT)
        } else if (SOC_IS_MONTEREY(unit)) {
            soc_monterey_dump_port_hsp(unit, p);
#endif
#if defined(BCM_HELIX5_SUPPORT)
        } else if (SOC_IS_HELIX5(unit)) {
            bcm_hx5_dump_port_hsp(unit, p);
#endif
#if defined(BCM_FIREBOLT6_SUPPORT)
        } else if (SOC_IS_FIREBOLT6(unit)) {
            if(subcmd) {
                bcm_fb6_dump_subport_hsp(unit, p, subcmd);
            } else {
                bcm_fb6_dump_port_hsp(unit, p);
            }
#endif
        } else if (SOC_IS_APACHE(unit)) {
            soc_apache_dump_port_hsp(unit, p);
#if defined(BCM_HURRICANE4_SUPPORT)
        } else if (SOC_IS_HURRICANE4(unit)) {
            bcm_hr4_dump_port_hsp(unit, p);
#endif
        } else if ((SOC_IS_TRIDENT2X(unit) || SOC_IS_TITAN2PLUS(unit))) {
            soc_td2_dump_port_hsp(unit, p);
        }  else if (SOC_IS_TRIUMPH3(unit)) {
            soc_tr3_dump_port_hsp(unit, p);
        }
    }

    return CMD_OK;
}
#endif /* BCM_TRIDENT2_SUPPORT */

#endif /* (BCM_TRIUMPH3_SUPPORT | BCM_KATANA_SUPPORT) */

