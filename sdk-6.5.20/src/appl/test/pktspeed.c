/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        pktspeed.c
 * Purpose:     Run a TX or RX reload test to check DMA speed
 *
 * Test Parameters:
 *       PORT       - TX port
 *       RXPORT     - RX port
 *       PKTsize    - What size packets to use
 *       ALLOCsize  - What allocation size to use for packets
 *       ChainLen   - How many DMA'd packets to set up in loop
 *       Seconds    - How many seconds to run
 *
 *   Test reports speed once per second.
 *
 * Notes:       Code originally from appl/diag/txrx.c
 *              Moved here to address PR SDK/422
 */

#include <sal/core/time.h>
#include <sal/types.h>
#include <sal/appl/io.h>
#include <shared/bsl.h>
#include <soc/types.h>
#include <soc/cm.h>
#include <soc/dma.h>
#include <soc/dcb.h>
#include <soc/drv.h>

#include <bcm/tx.h>
#include <bcm/pkt.h>
#include <bcm/rx.h>
#include <bcm/port.h>
#include <bcm/cosq.h>
#include <bcm/error.h>

#ifdef BCM_CMICM_SUPPORT
#include <soc/cmicm.h>
#endif

#include "testlist.h"

#if defined (BCM_PETRA_SUPPORT)
#include <soc/dpp/drv.h>
#else
#define SOC_DPP_PP_ENABLE(unit) (0)
#endif

#if defined (BCM_ESW_SUPPORT) || defined (BCM_PETRA_SUPPORT)

#define TEST_FAIL -1
#define TEST_OK    0

typedef struct pktspeed_param_s {
        int unit,        /* Which unit to test on */
            tx_port,     /* packet TX port (TX/RX test) */
            rx_port,     /* packet RX port (RX test) */
            tx_packet_count,    /* tx packet count (RX test) */
            pkt_size,    /* Size of actual pkt */
            alloc_size,  /* Allocation size of packet */
            chain_len,   /* Number of pkts to set up per cycle */
            seconds,     /* Number of seconds to run */
            poll;        /* T=poll F=interrupt */
        int p_l_start;   /* first pkt length for iteration*/
        int p_l_end;     /* last pkt length for iteration */
        int p_l_inc;    /* pkt length increment between iterations */
    volatile COMPILER_DOUBLE count; /* Packet count */
} pktspeed_param_t;

static volatile int desc_intr_count = 0;
static volatile int chain_intr_count = 0;

static pktspeed_param_t     *pktspeed_param[SOC_MAX_NUM_DEVICES];

/*
 * Function:
 *      pktspeed_print_param
 *
 * Purpose:
 *      Print parameter structure
 *
 * Parameters:
 *      psp             - parameter structure
 *
 * Returns:
 *      Allocated structure or NULL if alloc failed.
 */

STATIC void
pktspeed_print_param(pktspeed_param_t *psp)
{
    if (psp != NULL) {
        cli_out("port=%d ",  psp->tx_port);
        cli_out("rx=%d ",    psp->rx_port);
        cli_out("pkt=%d ",   psp->pkt_size);
        cli_out("alloc=%d ", psp->alloc_size);
        cli_out("cl=%d ",    psp->chain_len);
        cli_out("sec=%d ",   psp->seconds);
        cli_out("\n");
    } else {
        cli_out("psp(null)\n");
    }
}

/*
 * Function:
 *      pktspeed_print_report
 *
 * Purpose:
 *      Print speed report 
 *
 * Parameters:
 *      psp             - parameter structure
 *      t_diff          - time difference (usec)
 *
 * Returns:
 *      void
 */

STATIC void
pktspeed_print_report(pktspeed_param_t *psp, COMPILER_DOUBLE count, int t_diff)
{
    COMPILER_DOUBLE bytes_per_second, pkts_per_second;
    COMPILER_DOUBLE t_diff_dbl, pkt_size_dbl;

    if (psp != NULL) {
        COMPILER_32_TO_DOUBLE(t_diff_dbl, t_diff);
        COMPILER_32_TO_DOUBLE(pkt_size_dbl, psp->pkt_size);
        pkts_per_second = count/t_diff_dbl;
        bytes_per_second = (count * pkt_size_dbl)/t_diff_dbl;
        cli_out("    %15" COMPILER_DOUBLE_FORMAT
                " %15" COMPILER_DOUBLE_FORMAT
                " %15" COMPILER_DOUBLE_FORMAT "\n",
                count, pkts_per_second, bytes_per_second);
    } else {
        cli_out("psp(null)\n");
    }
}

/*
 * Function:
 *      pktspeed_done_desc
 *
 * Purpose:
 *      dv_done_desc callout increments packet count in interrupt context
 *
 * Parameters:
 *      u               - unit
 *      dv              - dv struct
 *      dcb             - dcp struct
 *
 * Returns:
 *      void
 */

STATIC void
pktspeed_done_desc(int u, struct dv_s *dv, dcb_t *dcb)
{
    pktspeed_param_t *psp = (pktspeed_param_t *)dv->dv_public1.ptr;

    psp->count += psp->chain_len;

    desc_intr_count += 1;
}

STATIC void
pktspeed_done_chain(int unit, dv_t *dv_chain)
{
    chain_intr_count += 1;
}

/*
 * Function:
 *      pktspeed_run_test
 *
 * Purpose:
 *      Run actual pktspeed test
 *
 * Parameters:
 *      tx_test         - TRUE for TX test, FALSE for RX test
 *      psp             - test parameters
 *
 * Returns:
 *      TEST_OK
 *      TEST_FAIL
 *
 * Notes:
 *      Could possibly use a pause scheme at the end of the while loop.
 */

STATIC int
pktspeed_run_test(int tx_test, pktspeed_param_t *psp)
{
    pbmp_t tx_pbm;
    static pbmp_t empty_pbm;    /* Initialized to 0 automatically */
    char *pkt_data = NULL;
    dv_t *dv = NULL;
    sal_usecs_t start_time, cur_time;
    COMPILER_DOUBLE count;
    int t_diff, last_rpt;
    int tot_dcb;
    int i;
    int rv = TEST_OK;
    int rv2 = TEST_OK;
    dcb_t	*d;
    int channel;
    bcm_pkt_t *pkt = NULL;
    bcm_vlan_t tx_vlan = 0x123;
    bcm_pbmp_t pbmp;
    bcm_pbmp_t ubmp;
    bcm_gport_t sch_gport;
    bcm_gport_t egq_gport;
    static sal_mac_addr_t mac_dest = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
    static sal_mac_addr_t mac_src  = {0x02, 0x11, 0x22, 0x33, 0x44, 0x66};

    if (psp == NULL) {
        cli_out("pktspeed test: NULL param struct\n");
        return(TEST_FAIL);
    }

    COMPILER_32_TO_DOUBLE(count, 0);

    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);

    if (tx_test) {

        rv = bcm_port_enable_set(psp->unit, psp->tx_port, TRUE);
        if (rv != TEST_OK ) {
            goto done;
        }
        SOC_PBMP_PORT_SET(tx_pbm, psp->tx_port);
        channel = 0;
        if (soc_dma_chan_config(psp->unit, channel, DV_TX, SOC_DMA_F_DEFAULT) < 0) {
            cli_out("pktspeed test: Could not configure u %d, chan %d\n",
                    psp->unit,
                    channel);
            rv = TEST_FAIL;
            goto done;
        }
    } else {  /* Set ports to loopback and send a broadcast packet */        
        rv = bcm_port_loopback_set(psp->unit, psp->tx_port, BCM_PORT_LOOPBACK_MAC);
        if (rv != TEST_OK ) {
            goto done;
        }
        rv = bcm_port_loopback_set(psp->unit, psp->rx_port, BCM_PORT_LOOPBACK_MAC);
        if (rv != TEST_OK ) {
            goto done;
        }
        rv = bcm_port_enable_set(psp->unit, psp->tx_port, TRUE);
        if (rv != TEST_OK ) {
            goto done;
        }
        rv = bcm_port_enable_set(psp->unit, psp->rx_port, TRUE);
        if (rv != TEST_OK ) {
            goto done;
        }
        if ((bcm_pkt_alloc(psp->unit,
                           psp->pkt_size,
                           BCM_TX_CRC_REGEN, &pkt)) !=
                BCM_E_NONE) {
            cli_out("Could not allocate packet to tx\n");
            return TEST_FAIL;
        }
        /* Set up broadcast packet */
        if (!SOC_IS_ARAD(psp->unit) || !SOC_DPP_PP_ENABLE(psp->unit)) {   
            BCM_PKT_HDR_DMAC_SET(pkt, mac_dest);
            BCM_PKT_HDR_SMAC_SET(pkt, mac_src);
            BCM_PKT_HDR_TPID_SET(pkt, ENET_DEFAULT_TPID);
            BCM_PKT_HDR_VTAG_CONTROL_SET(pkt, VLAN_CTRL(0, 0, 1));
        } else {
            /* Add the rx_port, rx_port and cpu port to the broadcast vlan domain */
            rv = bcm_vlan_create(psp->unit, tx_vlan);
            if (rv != TEST_OK ) {
                goto done;
            }
            BCM_PBMP_PORT_ADD(pbmp, psp->tx_port);
            BCM_PBMP_PORT_ADD(pbmp, psp->rx_port);
            BCM_PBMP_PORT_ADD(pbmp, 0);
            rv = bcm_vlan_port_add(psp->unit, tx_vlan, pbmp, ubmp);
            if (rv != TEST_OK ) {
                goto done;
            }
            /* Add the PTCH header */
            (pkt)->_pkt_data.data[0] = 0xD0;
            (pkt)->_pkt_data.data[1] = psp->tx_port;
            /* Add the Ethernet packet */
            sal_memcpy((pkt)->_pkt_data.data + 2, mac_dest, 6);
            sal_memcpy((pkt)->_pkt_data.data + 8, mac_src, 6);
            _BCM_HTONS_CVT_SET(pkt, ENET_DEFAULT_TPID, 14);
            _BCM_HTONS_CVT_SET(pkt, VLAN_CTRL(0, 0, tx_vlan), 16);
            /* Configure the CPU sch and egq scheduler as the rate of 1G bps */
            BCM_COSQ_GPORT_E2E_PORT_SET(sch_gport, 0);
            rv = bcm_cosq_gport_bandwidth_set(psp->unit, sch_gport, 0, 0, 10000000, 0);
            if (rv != TEST_OK ) {
                goto done;
            }
            BCM_GPORT_LOCAL_SET(egq_gport, 0);
            rv = bcm_cosq_gport_bandwidth_set(psp->unit, egq_gport, 0, 0, 10000000, 0);
            if (rv != TEST_OK ) {
                goto done;
            }
        }
        SOC_PBMP_PORT_SET(pkt->tx_pbmp, psp->tx_port);
        pkt->opcode = BCM_HG_OPCODE_BC;

        channel = 1;
        if (DV_RX != soc_dma_chan_dvt_get(psp->unit, channel)) {
            if (soc_dma_chan_config(psp->unit, channel, DV_RX, 0) < 0) {
                cli_out("pktspeed test: Could not configure u %d, chan %d\n",
                        psp->unit,
                        channel);
                rv = TEST_FAIL;
                goto done;
            }
        }

        rv = bcm_rx_queue_channel_set(psp->unit, -1, 1);
        if (rv != TEST_OK ) {
            goto done;
        }

        {
            int i;
            for (i=0; i< psp->tx_packet_count; i++) {
                /* Send broadcast packet */
                if (bcm_tx(psp->unit, pkt, NULL) != BCM_E_NONE) {
                    cli_out("Failed to transmit packet\n");
                    return TEST_FAIL;
                }
            }
        }
        
    }

    tot_dcb = psp->chain_len + 1;    /* Need one for reload */

    /* Round packet size up to 8 byte boundary */
    psp->alloc_size = (psp->alloc_size + 7) & 0xffffff8;

    if ((pkt_data = soc_cm_salloc(psp->unit, psp->alloc_size * psp->chain_len,
                                  "reload_data")) == NULL) {
        cli_out("Could not allocate packet data\n");
        rv = TEST_FAIL;
        goto done;
    }

    if ((dv = soc_dma_dv_alloc(psp->unit, tx_test ? DV_TX : DV_RX,
                               tot_dcb)) == NULL) {
        cli_out("Could not allocate DV\n");
        rv = TEST_FAIL;
        goto done;
    }

    /* DCBs MUST start off 0 */
    sal_memset(dv->dv_dcb, 0, SOC_DCB_SIZE(psp->unit) * tot_dcb);

    for (i = 0; i < psp->chain_len; i++) {
        if (soc_dma_desc_add(dv, (sal_vaddr_t)&pkt_data[i * psp->alloc_size],
                             psp->alloc_size, tx_pbm, empty_pbm,
                             empty_pbm, 0, NULL) < 0) {
            cli_out("Could not setup packet %d\n", i);
            rv = TEST_FAIL;
            goto done;
        }
        soc_dma_desc_end_packet(dv);
    }

    /* Set reload bit of last descriptor to point to first */
    if (soc_dma_rld_desc_add(dv, (sal_vaddr_t)dv->dv_dcb) < 0) {
        cli_out("Could not setup reload dcb\n");
        rv = TEST_FAIL;
        goto done;
    }

    d = SOC_DCB_IDX2PTR(psp->unit, dv->dv_dcb, dv->dv_vcnt - 1);
    SOC_DCB_CHAIN_SET(psp->unit, d, 1);

    /* Disable DMA interrupts - prevent soc_start_dma() from choking */
#ifdef BCM_CMICM_SUPPORT
    if(soc_feature(psp->unit, soc_feature_cmicm)) {
        (void)soc_cmicm_intr0_disable(psp->unit,
                        IRQ_CMCx_DESC_DONE(channel) | IRQ_CMCx_CHAIN_DONE(channel));
    } else
#endif
    {
        soc_intr_disable(psp->unit,
                         IRQ_DESC_DONE(channel) | IRQ_CHAIN_DONE(channel));
    }
    if (!psp->poll) {
        /* Setup callout if interrupt */
        dv->dv_done_desc   = pktspeed_done_desc;
        dv->dv_done_chain   = pktspeed_done_chain;
        dv->dv_public1.ptr = (void *)psp;
        dv->dv_flags      |= DV_F_NOTIFY_DSC | DV_F_NOTIFY_CHN;
    }

    cur_time = start_time = sal_time_usecs();
    t_diff = 0;
    last_rpt = 0;
    count = psp->count = 0;

    /* Start DMA */
    if (soc_dma_start(psp->unit, channel, dv) < 0) {
        cli_out("pktspeed test: Could not start dv, u %d, chan %d\n",
                psp->unit, channel);
        rv = TEST_FAIL;
        goto done;
    }

    cli_out("Starting %s reload test:\n",
            tx_test ? "TX" : "RX");
    pktspeed_print_param(psp);
    cli_out("    %15s %15s %15s\n",
            "Total Pkts", "Pkts/Second", "Bytes/Second");
    while (t_diff <= psp->seconds) {
        if (psp->poll) {
            /* Monitor done bit in last real dcb */
            soc_cm_sinval(psp->unit, d, SOC_DCB_SIZE(psp->unit));
            if (SOC_DCB_DONE_GET(psp->unit, d)) {
                psp->count += psp->chain_len;
                count = psp->count;
                SOC_DCB_DONE_SET(psp->unit, d, 0);
                soc_cm_sflush(psp->unit, d, SOC_DCB_SIZE(psp->unit));
            }
        } else {
            sal_sleep(1);
            count = psp->count;
        }
        /* Report every second */
        if (t_diff >= last_rpt + 1) {
            last_rpt = t_diff;
            pktspeed_print_report(psp, count, t_diff);
        }  
        cur_time = sal_time_usecs();
        t_diff = SAL_USECS_SUB(cur_time, start_time)/1000000;
    }

done:     
    if (SOC_IS_ARAD(psp->unit) && !tx_test) {
        bcm_vlan_port_remove(psp->unit, tx_vlan, pbmp);
        bcm_vlan_destroy(psp->unit, tx_vlan);
    }
    rv2 = bcm_port_loopback_set(psp->unit, psp->tx_port, BCM_PORT_LOOPBACK_NONE);
    if (rv2 != TEST_OK ) {
        cli_out("Re tx port loopback failed\n");
    }
    if (!tx_test) {
        rv2= bcm_port_loopback_set(psp->unit, psp->rx_port,
                              BCM_PORT_LOOPBACK_NONE);
        if (rv2 != TEST_OK ) {
            cli_out("Disable rx port loopback failed\n");
        }
    }
	
    if (dv) {
        soc_dma_abort_dv(psp->unit, dv);
        soc_dma_dv_free(psp->unit, dv);
    }
    if (pkt_data) {
        soc_cm_sfree(psp->unit, pkt_data);
    }
    if (pkt) {
        bcm_pkt_free(psp->unit, pkt);
    }
	


    return rv;
}

/*
 * Function:
 *      pktspeed_alloc
 *
 * Purpose:
 *      Allocate and initialize test parameter structure
 *
 * Parameters:
 *      unit            - Unit these parameters are for
 *
 * Returns:
 *      Allocated structure or NULL if alloc failed.
 */

STATIC pktspeed_param_t *
pktspeed_alloc(int unit)
{
    bcm_port_t port = 0;
    pktspeed_param_t *psp =
      (pktspeed_param_t *)sal_alloc(sizeof(pktspeed_param_t),
                                    "Pktspeed test config");

    if (psp != NULL) {
        psp->unit       = unit;
        if(SOC_IS_APOLLO(unit) || SOC_IS_VALKYRIE2(unit)) {
            psp->tx_port = 30;
        } else if(SOC_IS_ENDURO(unit) || SOC_IS_HURRICANE(unit)) {
            psp->tx_port = 2;
        } else if (SOC_IS_KATANA(unit) || SOC_IS_GREYHOUND(unit)) {
            BCM_PBMP_ITER(PBMP_PORT_ALL(unit), port) { 
            break;
            }
            psp->tx_port = port;
        } else if (SOC_IS_ARAD(unit)) { 
            psp->tx_port = 13;
        } else {
            psp->tx_port = 1;
        }
        psp->rx_port    = -1;   /* Select automatically */
        if (!SOC_IS_ARAD(psp->unit)) {
            psp->tx_packet_count   = 100;
        } else {
            psp->tx_packet_count   = 1;
        }
        psp->pkt_size   = 68;
        psp->alloc_size = 68;
        psp->chain_len  = 1000;
        psp->seconds    = 10;
        psp->poll       = TRUE;
        COMPILER_32_TO_DOUBLE(psp->count, 0);
        psp->p_l_start = 0;  /* 0 means no iteration */
        psp->p_l_end = 0;
        psp->p_l_inc = 64;
    }   
    return psp;
}

/*
 * Function:
 *      pktspeed_free
 *
 * Purpose:
 *      Free test parameter structure
 *
 * Parameters:
 *      psp             - parameter structure pointer
 *
 * Returns:
 *      TEST_OK
 *      TEST_FAIL
 */

STATIC int pktspeed_free(pktspeed_param_t *psp)
{
    int rv = TEST_FAIL;

    if (!psp) {
        return rv;
    }
    sal_free(psp);

    return TEST_OK;
}

/*
 * Function:
 *      pktspeed_test_init
 *
 * Purpose:
 *      Parse test arguments and save parameter structure locally
 *
 * Parameters:
 *      unit            - unit to test
 *      args            - test arguments
 *      pa              - test cookie (not used)
 *
 * Returns:
 *      TEST_OK
 *      TEST_FAIL
 */

int
pktspeed_test_init(int unit, args_t *args, void **pa)
{
    parse_table_t	pt;
    pktspeed_param_t    *psp = pktspeed_alloc(unit);

    if (psp == NULL) {
        cli_out("%s: out of memory\n", ARG_CMD(args));
        return(TEST_FAIL);
    }

    parse_table_init(unit, &pt);

    parse_table_add(&pt, "PORT", PQ_DFL|PQ_PORT, 0,
                    &psp->tx_port, NULL);
    parse_table_add(&pt, "RXport", PQ_DFL|PQ_PORT, 0,
                    &psp->rx_port, NULL);
    parse_table_add(&pt, "TxPktCount", PQ_DFL|PQ_INT, 0,
                    &psp->tx_packet_count, NULL);
    parse_table_add(&pt, "PKTsize", PQ_DFL|PQ_INT, 0,
                    &psp->pkt_size, NULL);
    parse_table_add(&pt, "ALLOCsize", PQ_DFL|PQ_INT, 0,
                    &psp->alloc_size, NULL);
    parse_table_add(&pt, "ChainLen", PQ_DFL|PQ_INT, 0,
                    &psp->chain_len, NULL);
    parse_table_add(&pt, "SEConds", PQ_DFL|PQ_INT, 0,
                    &psp->seconds, NULL);
    parse_table_add(&pt, "LengthStart", PQ_DFL|PQ_INT, 0,
                    &psp->p_l_start, NULL);
    parse_table_add(&pt, "LengthEnd", PQ_DFL|PQ_INT, 0,
                    &psp->p_l_end, NULL);
    parse_table_add(&pt, "LengthInc", PQ_DFL|PQ_INT, 0,
                    &psp->p_l_inc, NULL);

    /* Parse remaining arguments */
    if (0 > parse_arg_eq(args, &pt)) {
    	test_error(unit,
    		   "%s: Invalid option: %s\n",
    		   ARG_CMD(args),
    		   ARG_CUR(args) ? ARG_CUR(args) : "*");
    	parse_arg_eq_done(&pt);
    	goto fail;
    }
    parse_arg_eq_done(&pt);

    if (psp->pkt_size > psp->alloc_size) {
    	test_error(unit,
                       "%s: Error: packet size > alloc size\n",
                       ARG_CMD(args));
    	parse_arg_eq_done(&pt);
    	goto fail;
    }
    parse_arg_eq_done(&pt);

    if (psp->rx_port < 0) {     /* Find an unused port */
        int p;

        /* Find first available port != tx_port */
        PBMP_ITER(PBMP_PORT_ALL(unit), p) {
            if (p != psp->tx_port) {
                psp->rx_port = p;
                break;
            }
        }

        /* Check to see if one was found */
        if (psp->rx_port < 0) {
            test_error(unit,
                       "%s: Could not find an available RX port.\n",
                       ARG_CMD(args));
            goto fail;
        }

    } else {                    /* RX port specified */

        /* Make sure selected RX port is valid */
        if (!BCM_PBMP_MEMBER(PBMP_PORT_ALL(unit), psp->rx_port)) {
            test_error(unit,
                       "%s: Invalid RX port %d.\n",
                       ARG_CMD(args), psp->rx_port);
            goto fail;
        }
    }

    /* Make sure TX port is valid, too */
    if (!BCM_PBMP_MEMBER(PBMP_PORT_ALL(unit), psp->tx_port)) {
        test_error(unit,
                   "%s: Invalid TX port %d.\n",
                   ARG_CMD(args), psp->tx_port);
        goto fail;
    }

    pktspeed_param[unit] = psp;
    

    return TEST_OK;
fail:
    if (psp) {
        sal_free(psp);
    }
    return(TEST_FAIL);
}

/*
 * Function:
 *      pktspeed_test_tx
 *
 * Purpose:
 *      Runs TX test
 *
 * Parameters:
 *      unit            - unit to test
 *      a               - test arguments (ignored)
 *      pa              - test cookie (ignored)
 *
 * Returns:
 *      TEST_OK
 *      TEST_FAIL
 */

int
pktspeed_test_tx(int unit, args_t *a, void *pa)
{
    pktspeed_param_t *psp = pktspeed_param[unit];
    int rv = TEST_OK;
    int len;

    if (psp->p_l_start == 0) { /* single length only */
        psp->p_l_start = psp->p_l_end = psp->alloc_size;
    }

    for (len = psp->p_l_start; len <= psp->p_l_end; len += psp->p_l_inc) {
        psp->alloc_size = len;  /* alloc size placed in DCB determines amount
                                   of data tx; pkt_size is not used in tx test */
        rv = pktspeed_run_test(TRUE, psp);
        if (rv != TEST_OK) {
            break;
        }
    }

    return rv;
}

/*
 * Function:
 *      pktspeed_test_rx
 *
 * Purpose:
 *      Runs RX test
 *
 * Parameters:
 *      unit            - unit to test
 *      a               - test arguments (ignored)
 *      pa              - test cookie (ignored)
 *
 * Returns:
 *      TEST_OK
 *      TEST_FAIL
 */

int
pktspeed_test_rx(int unit, args_t *a, void *pa)
{
    pktspeed_param_t *psp = pktspeed_param[unit];
    int rv = TEST_OK;
    int len;

    if (psp->p_l_start == 0) { /* single length only */
        psp->p_l_start = psp->p_l_end = psp->pkt_size;
    }

    for (len = psp->p_l_start; len <= psp->p_l_end; len += psp->p_l_inc) {
        psp->pkt_size = len;
        rv = pktspeed_run_test(FALSE, psp);
        if (rv != TEST_OK) {
            break;
        }
    }

    return rv;
}

/*
 * Function:
 *      pktspeed_test_done
 *
 * Purpose:
 *      Cleans up after tx or rx test completes
 *
 * Parameters:
 *      unit            - unit to test
 *      pa              - test cookie (ignored)
 *
 * Returns:
 *      TEST_OK
 *      TEST_FAIL
 */

int
pktspeed_test_done(int unit, void *pa)
{
    return  pktspeed_free(pktspeed_param[unit]);
}

#endif /* BCM_ESW_SUPPORT */
