/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Loopback Tests, version 2
 *
 *	MAC	Sends packets from CPU to MAC in loopback mode, and back
 *	PHY	Sends packets from CPU to PHY in loopback mode, and back
 *	EXT	Sends packets from CPU out one port, in another, and back
 *	SNAKE	Sends packets from CPU through ports via MAC or PHY loopback
 */

#include <shared/bsl.h>

#include <sal/core/boot.h>
#include <sal/appl/sal.h>
#include <shared/bsl.h>
#include <appl/diag/system.h>
#include <appl/diag/system.h>
#include <appl/diag/test.h>

#include <soc/debug.h>
#include <soc/hash.h>
#include <soc/drv.h>

#include <soc/higig.h>

#ifdef BCM_ESW_SUPPORT
#include <bcm_int/esw/mbcm.h>
#endif

#include <bcm/error.h>
#include <bcm/vlan.h>
#include <bcm/link.h>
#include <bcm/stg.h>
#include <bcm/stat.h>
#include <bcm/mcast.h>
#include <bcm/stack.h>

#include <appl/test/loopback2.h>

#include "testlist.h"

/*
 * Loopback work structure
 */

STATIC loopback2_test_t lb2_work[SOC_MAX_NUM_DEVICES];

int
lb2_mac_init(int unit, args_t *a, void **pa)
/*
 * Function: 	lb2_mac_init
 * Purpose:	Initialize MAC loopback test.
 * Parameters:	unit - unit number.
 * Returns:	0 for success, -1 for failed.
 */
{
    loopback2_test_t	*lw = &lb2_work[unit];
    loopback2_testdata_t	*lp = &lw->params[LB2_TT_MAC];
    parse_table_t	pt;

    lbu_setup(unit, lw);

    parse_table_init(unit, &pt);
    lbu_pkt_param_add(unit, &pt, lp);
    lbu_port_param_add(unit, &pt, lp);
    lbu_other_param_add(unit, &pt, lp);

    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) != 0) {
	test_error(unit,
		   "%s: Invalid option: %s\n",
		   ARG_CMD(a),
		   ARG_CUR(a) ? ARG_CUR(a) : "*");
	parse_arg_eq_done(&pt);
	return(-1);
    }
    parse_arg_eq_done(&pt);

    if (lbu_check_parms(lw, lp)) {
	return(-1);
    }

    /* Perform common loopback initialization */

    if (lbu_init(lw, lp)) {
	return(-1);
    }

    *pa = lw;

    return(0);
}

int
lb2_phy_init(int unit, args_t *a, void **pa)
/*
 * Function: 	lb2_phy_init
 * Purpose:	Initialize PHY loopback test.
 * Parameters:	unit - unit number.
 * Returns:	0 for success, -1 for failed.
 */
{
    loopback2_test_t	*lw = &lb2_work[unit];
    loopback2_testdata_t	*lp = &lw->params[LB2_TT_PHY];
    parse_table_t	pt;

    lbu_setup(unit, lw);

    parse_table_init(unit, &pt);
    lbu_pkt_param_add(unit, &pt, lp);
    lbu_port_param_add(unit, &pt, lp);
    lbu_other_param_add(unit, &pt, lp);

    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) != 0) {
	test_error(unit,
		   "%s: Invalid option: %s\n",
		   ARG_CMD(a),
		   ARG_CUR(a) ? ARG_CUR(a) : "*");
	parse_arg_eq_done(&pt);
	return(-1);
    }
    parse_arg_eq_done(&pt);


    if (lbu_check_parms(lw, lp)) {
	return(-1);
    }

    /* Perform common loopback initialization */

    if (lbu_init(lw, lp)) {
	return(-1);
    }

    *pa = lw;

    return(0);
}

/*ARGSUSED*/
int
lb2_port_test(int unit, args_t *a, void *pa)
/*
 * Function: 	lb2_port_test
 * Purpose:	Perform MAC loopback tests.
 * Parameters:	unit - unit #
 *		a - arguments (Not used, assumed parse by lb_mac_init).
 * Returns:
 */
{
    loopback2_test_t	*lw = (loopback2_test_t *)pa;
    loopback2_testdata_t	*lp = lw->cur_params;
    int		port, rv;
    pbmp_t	pbm, tpbm;

    COMPILER_REFERENCE(a);

    LOG_INFO(BSL_LS_APPL_TESTS,
             (BSL_META_U(unit,
                         "lb_mac_test[%d]: Starting ....\n"), unit));

    lbu_stats_init(lw);

    BCM_PBMP_ASSIGN(pbm, lp->pbm);
    BCM_PBMP_ASSIGN(tpbm, lp->pbm);
    BCM_PBMP_AND(tpbm, PBMP_FE_ALL(unit));

    ENET_SET_MACADDR(lw->base_mac_src, lp->mac_src);
    ENET_SET_MACADDR(lw->base_mac_dst, lp->mac_dst);

    /*
     * Used to determine if ARL entry needs to be setup. Typically
     * required only for _E_ Ports.
     */

    BCM_PBMP_ASSIGN(tpbm, pbm);
    BCM_PBMP_AND(tpbm, PBMP_E_ALL(unit));
    if (BCM_PBMP_NOT_NULL(tpbm)) { 
        /* Don't setup arl if only hg ports */
        /*
         * Get our current MAC address for this port and be sure the
         * SOC will route packets to the CPU.
         */
        LOG_INFO(BSL_LS_APPL_TESTS,
                 (BSL_META_U(unit,
                             "Setting up ARL\n")));
        if (lbu_setup_arl_cmic(lw)) {
            return(-1);
        }
    }

#if defined(__KERNEL__) && (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICDV2_SUPPORT))
    (void)soc_cmicm_cmcx_intr0_enable(unit, 0, 0xff00);
    sal_usleep(50);
#endif

    PBMP_ITER(pbm, port) {
        lw->tx_len = 0;                 /* Start off clean */

        lw->tx_port = port;
        lw->rx_port = port;

        rv = lbu_serial_txrx(lw);

        if (lp->inject) { /* Diagnostic */
            return 0;
        }

        if (rv < 0) {
            return -1;
        }
    }
    
    if (BCM_PBMP_NOT_NULL(tpbm)) { 
        lbu_cleanup_arl(lw);
    }

    lbu_stats_done(lw);

    return 0;
}

int
lb2_done(int unit, void *pa)
/*
 * Function: 	lb2_done
 * Purpose:	Clean up after new loopback tests.
 * Parameters:	unit - unit number.
 * Returns:	0 for success, -1 for failed.
 */
{
    soc_port_t	p;
    int		rv;

    if (pa) {
	loopback2_testdata_t *lp = ((loopback2_test_t *)pa)->cur_params;

        if (lp->inject) { /* Diagnostic */
            return 0;
        }

	/* Clear MAC loopback - Only if pa is set */

	PBMP_ITER(lp->pbm, p) {
	    rv = bcm_port_loopback_set(unit, p, BCM_PORT_LOOPBACK_NONE);
	    if (rv != BCM_E_NONE) {
		test_error(unit,
			   "Port %s: Failed to reset MAC loopback: %s\n",
			   SOC_PORT_NAME(unit, p), bcm_errmsg(rv));
                return rv;
	    }
	}

        if (lp->test_type == LB2_TT_SNAKE) {
            if ((rv = lbu_snake_done(&lb2_work[unit])) < 0) {
                return rv;
            }
        }
    }

    if ((rv = lbu_done(&lb2_work[unit])) < 0) {
        return rv;
    }

#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit)) {
        loopback2_test_t    *lw = &lb2_work[unit];
        loopback2_testdata_t    *lp_snake = &lw->params[LB2_TT_SNAKE];
        int num_ports;
        SOC_PBMP_COUNT(lp_snake->saved_pbm, num_ports);
        if (num_ports > LB2_TH_MAX_PORTS_WITH_PROFILE) {
            SOC_PBMP_ASSIGN(lp_snake->pbm, lp_snake->saved_pbm);
        }
    }
#endif

    return 0;
}

int
lb2_snake_init(int unit, args_t *a, void **pa)
/*
 * Function: 	lb2_snake_init
 * Purpose:	Initialize snake loopback test.
 * Parameters:	unit - unit number.
 * Returns:	0 for success, -1 for failed.
 */
{
    loopback2_test_t	*lw = &lb2_work[unit];
    loopback2_testdata_t	*lp = &lw->params[LB2_TT_SNAKE];
    parse_table_t	pt;
    int rv;

#if defined(BCM_TOMAHAWK_SUPPORT)
    bcm_port_t port;
    int port_count;
    int num_ports;
#endif

#if defined(__KERNEL__) && (defined(BCM_CMICM_SUPPORT) || defined(BCM_CMICDV2_SUPPORT))
    (void)soc_cmicm_cmcx_intr0_enable(unit, 0, 0xff00);
    sal_usleep(50);
#endif

    lbu_setup(unit, lw);

    parse_table_init(unit, &pt);
    lbu_pkt_param_add(unit, &pt, lp);
    lbu_port_param_add(unit, &pt, lp);
    lbu_other_param_add(unit, &pt, lp);

    if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) != 0) {
	test_error(unit,
		   "%s: Invalid option: %s\n",
		   ARG_CMD(a),
		   ARG_CUR(a) ? ARG_CUR(a) : "*");
	return(-1);
    }

    if (lbu_check_parms(lw, lp)) {
	return(-1);
    }

    /* Snake test supported only on Ethernet ports on FB & ER */
    if (SOC_IS_FB_FX_HX(unit)) {
        SOC_PBMP_AND(lp->pbm, PBMP_E_ALL(unit));
    }


#if defined(BCM_TOMAHAWK_SUPPORT)
    if (SOC_IS_TOMAHAWKX(unit)) {
        /*
        *Due to the profile management limitation Only support maximum 127 ports for the test;
        *When there are 128 ports for the TH/TH+. port_select_mode are uese to choose the 127 ports
        */
        SOC_PBMP_ASSIGN(lp->saved_pbm, lp->pbm);
        SOC_PBMP_COUNT(lp->pbm, num_ports);
        port_count = 0;
        if (num_ports > LB2_TH_MAX_PORTS_WITH_PROFILE) {
            PBMP_ITER(lp->pbm, port) {
                port_count = port_count + 1;
                if (lp->port_select_mode == LB2_TH_ASCEND_PORT_MODE) {
                    if (port_count > LB2_TH_MAX_PORTS_WITH_PROFILE) {
                        SOC_PBMP_PORT_REMOVE(lp->pbm, port);
                    }
                } else if (lp->port_select_mode == LB2_TH_DSCEND_PORT_MODE) {
                    if (port_count <= (num_ports-LB2_TH_MAX_PORTS_WITH_PROFILE)) {
                        SOC_PBMP_PORT_REMOVE(lp->pbm, port);
                    }
                } else {
                    test_error(unit, "Invalid PortSelectMode \n");
                    return -1;
                }
            }
            lp->port_select_mode = LB2_TH_ASCEND_PORT_MODE;
        }
    }
#endif

    /* Perform common loopback initialization */
    if (lbu_init(lw, lp)) {
	return(-1);
    }

    /* Perform snake specific initialization */
    rv = lbu_snake_init(lw, lp);
    if (rv < 0) {
        return rv;
    }

    *pa = lw;

    return(0);
}

int
lb2_snake_test(int unit, args_t *a, void *pa)
/*
 * Function: 	lb2_snake_test
 * Purpose:	Run snake loopback test.
 * Parameters:	unit - unit number.
 * Returns:	0 for success, -1 for failed.
 */
{
    loopback2_test_t	   *lw = (loopback2_test_t *) pa;
    loopback2_testdata_t   *lp = lw->cur_params;
    int		    c_count, ix;
    int		    rv, ret_val = 0;
    lb2_port_stat_t  *stats;

    COMPILER_REFERENCE(unit);
    COMPILER_REFERENCE(a);
    COMPILER_REFERENCE(lp);

    stats = sal_alloc(SOC_MAX_NUM_PORTS *
				  sizeof(lb2_port_stat_t),
				  "Stats");

    lw->tx_cos = lp->cos_start;
    lw->tx_len = lp->len_start;
    lw->tx_ppt = lp->ppt_start;


    for (c_count = 0; c_count < lp->iterations; c_count++) {

	cli_out("\nLB: loop %d of %d: "
                "circular test on ports for %d seconds\n",
                c_count + 1, lp->iterations, lp->duration);

        /* bcm call here someday.  For now, bcm_stat_clear is
         * just a wrapper for this function, and only does one port */
        if (!lp->inject) {
#ifdef BCM_ESW_SUPPORT
            if ((rv = soc_counter_set32_by_port(unit,  
                      PBMP_PORT_ALL(unit), 0)) < 0) {
                test_error(unit,
                           "Could not clear counters: %s\n",
                           soc_errmsg(rv));
                ret_val = -1;
                break;
            }
#endif
        }

        if (lp->inject) {
            lbu_snake_tx(lw);
            ret_val = 0;
            break;
        }

        for (ix = 0; ix < SOC_MAX_NUM_PORTS; ix++) {
            stats[ix].initialized = FALSE;
        }

        if ((rv = lbu_snake_txrx(lw, stats)) < 0) {
            test_error(unit, "Snake test failed\n");
            ret_val = -1;
            break;
        }

        /* Increment parameters for next cycle */
        lw->tx_cos += 1;
        if (lw->tx_cos > lp->cos_end) {
            lw->tx_cos = lp->cos_start;
        }
        lw->tx_len += lp->len_inc;
        if (lw->tx_len > lp->len_end) {
            lw->tx_len = lp->len_start;
        }
        lw->tx_ppt += lp->ppt_inc;
        if (lw->tx_ppt > lp->ppt_end) {
            lw->tx_ppt = lp->ppt_start;
        }
    }

    sal_free(stats);
    return ret_val;
}

