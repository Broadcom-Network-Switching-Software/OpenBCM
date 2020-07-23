/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        rxmon.c
 * Purpose:     Implementation of rxmon command
 * Requires:    
 */

#include <shared/bsl.h>

#include <sal/core/thread.h>
#include <soc/types.h>
#include <soc/higig.h>
#include <soc/dma.h>
#include <soc/dcb.h>
#include <soc/drv.h>

#ifdef __KERNEL__
#include <linux/kernel.h>
#include <linux/net.h>
#include <linux/in.h>
#else
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include <bcm/pkt.h>
#include <bcm/rx.h>
#include <bcm/error.h>

#include <appl/diag/system.h>

/* Default data for configuring RX system */
static bcm_rx_cfg_t rx_cfg = {
    8 * 1024, /* packet alloc size */
    4,        /* Packets per chain */
    1000,     /* Default pkt rate, global (all COS for this device) */
    100,      /* Burst doesn't matter */
    {  /* Just configure channel 1 */
        {0},  /* Channel 0 is usually TX */
        {    /* Channel 1, default RX */
            4,                     /* DV count (number of chains) */
            1000,                  /* Default pkt rate */
            0,                     /* No flags */
            0xff                   /* All COS to this channel */
        }
    },
    NULL,     /* Used default RX alloc and free routines */
    NULL,
    0        /* flags */
};

static bcm_rx_chan_cfg_t rx_chan_cfg = {
    4,                     /* DV count (number of chains) */
    1000,                  /* Default pkt rate */
    0,                     /* No flags */
    0xff                   /* All COS to this channel */
};

static int rx_cb_count;

/* Packet free queue; use first word of data buffer as next ptr */
static sal_mutex_t pkt_queue_lock[BCM_MAX_NUM_UNITS];
static sal_sem_t pkts_are_ready[BCM_MAX_NUM_UNITS];
static volatile uint32 *pkt_free_queue[BCM_MAX_NUM_UNITS];
static volatile int enqueue_pkts[BCM_MAX_NUM_UNITS];
static volatile int rx_pkt_count[BCM_MAX_NUM_UNITS];

STATIC bcm_rx_t
rx_cb_handler(int unit, bcm_pkt_t *info, void *cookie)
{
    int		count;

    COMPILER_REFERENCE(cookie);

    count = ++rx_cb_count;

    LOG_INFO(BSL_LS_APPL_RX,
             (BSL_META_U(unit,
                         "RX packet %d: unit=%d len=%d rx_port=%d reason=%d cos=%d\n"),
              count, unit, info->tot_len, info->rx_port, info->rx_reason,
              info->cos));

#ifdef	BCM_XGS_SUPPORT
    if (SOC_IS_XGS12_FABRIC(unit)) {
        if (LOG_CHECK(BSL_LS_APPL_RX | BSL_INFO)) {
            soc_higig_dump(unit, "HG HEADER: ",
                           (soc_higig_hdr_t *)BCM_PKT_HG_HDR(info));
        }
    }
#endif	/* BCM_XGS_SUPPORT */

    LOG_INFO(BSL_LS_APPL_RX,
             (BSL_META_U(unit,
                         "Parsed packet info:\n")));
    LOG_INFO(BSL_LS_APPL_RX,
             (BSL_META_U(unit,
                         "    src mod=%d. src port=%d. op=%d.\n"),
              info->src_mod, info->src_port, info->opcode));
    LOG_INFO(BSL_LS_APPL_RX,
             (BSL_META_U(unit,
                         "    dest mod=%d. dest port=%d. chan=%d.\n"),
              info->dest_mod, info->dest_port, info->dma_channel));

    if (LOG_CHECK(BSL_LS_APPL_RX | BSL_INFO)) {
        soc_dma_dump_pkt(unit, "Data: ", BCM_PKT_DMAC(info),
			 info->tot_len, TRUE);
    }

    if (enqueue_pkts[unit] > 0) {
        sal_mutex_take(pkt_queue_lock[unit], sal_mutex_FOREVER);
        *(uint32 **)(info->alloc_ptr) = (uint32 *)pkt_free_queue[unit];
        pkt_free_queue[unit] = info->alloc_ptr;
        rx_pkt_count[unit]++;
        if (rx_pkt_count[unit] >= enqueue_pkts[unit]) {
            sal_sem_give(pkts_are_ready[unit]);
        }
        sal_mutex_give(pkt_queue_lock[unit]);
#if defined(BCM_RXP_DEBUG)
        bcm_rx_pool_own(info->alloc_ptr, "rxmon");
#endif
        return BCM_RX_HANDLED_OWNED;
    }

    return BCM_RX_HANDLED;
}

STATIC void
rx_free_pkts(void *cookie)
{
    uint32 *to_free;
    uint32 *next;
    int unit = PTR_TO_INT(cookie);

    while (TRUE) {
        sal_sem_take(pkts_are_ready[unit], sal_sem_FOREVER);
        sal_mutex_take(pkt_queue_lock[unit], sal_mutex_FOREVER);
        to_free = (uint32 *)pkt_free_queue[unit];
        pkt_free_queue[unit] = NULL;
        rx_pkt_count[unit] = 0;
        sal_mutex_give(pkt_queue_lock[unit]);
        while (to_free != NULL) {
            next = *(uint32 **)to_free;
            bcm_rx_free(unit, to_free);
            to_free = next;
        }
    }
}

#define BASIC_PRIO 100

STATIC int
_init_rx_api(int unit)
{
    int r;

    if (bcm_rx_active(unit)) {
        cli_out("RX is already running\n");
        return -1;
    }

    if (pw_running(unit)) {
        cli_out("rxmon: Error: Cannot start RX with packetwatcher running\n");
        return -1;
    }

    if ((r = bcm_rx_start(unit, &rx_cfg)) < 0) {
        cli_out("rxmon: Error: Cannot start RX: %s.\n", bcm_errmsg(r));
        return -1;
    }

    return 0;
}

cmd_result_t
cmd_esw_rx_mon(int unit, args_t *args)
/*
 * Function:    rx
 * Purpose:     Perform simple RX test
 * Parameters:  unit - unit number
 *              args - arguments
 * Returns:     CMD_XX
 */
{
    char                *c;
    uint32              active;
    int                 r;
    int rv = CMD_OK;

    if (!sh_check_attached(ARG_CMD(args), unit)) {
        return(CMD_FAIL);
    }

    bcm_rx_channels_running(unit, &active);

    c = ARG_GET(args);
    if (c == NULL) {
        cli_out("Active bitmap for RX is %x.\n", active);
        return CMD_OK;
    }

    if (sal_strcasecmp(c, "init") == 0) {
        if (_init_rx_api(unit) < 0) {
            return CMD_FAIL;
        } else {
            return CMD_OK;
        }
    } else if (sal_strcasecmp(c, "enqueue") == 0) {
        if (pkt_queue_lock[unit] == NULL) { /* Init free pkt stuff */
            pkt_queue_lock[unit] = sal_mutex_create("rxmon");
            pkts_are_ready[unit] = sal_sem_create("rxmon", sal_sem_BINARY, 0);
            if (sal_thread_create("rxmon", SAL_THREAD_STKSZ, 80, rx_free_pkts,
                                  INT_TO_PTR(unit)) == SAL_THREAD_ERROR) {
                cli_out("FAILED to start rxmon packet free thread\n");
                sal_mutex_destroy(pkt_queue_lock[unit]);
                pkt_queue_lock[unit] = NULL;
                sal_sem_destroy(pkts_are_ready[unit]);
                pkts_are_ready[unit] = NULL;
                return CMD_FAIL;
            }
        }
        enqueue_pkts[unit] = 1;
        if ((c = ARG_GET(args)) != NULL) {
            enqueue_pkts[unit] = strtoul(c, NULL, 0);
        }
    } else if (sal_strcasecmp(c, "-enqueue") == 0) {
        enqueue_pkts[unit] = 0;
    } else if (sal_strcasecmp(c, "start") == 0) {
        rx_cb_count = 0;

        if (!bcm_rx_active(unit)) { /* Try to initialize */
            if (_init_rx_api(unit) < 0) {
                cli_out("Warning:  init failed.  Will attempt register\n");
            }
        }

        /* Register to accept all cos */
        if ((r = bcm_rx_register(unit, "RX CMD", rx_cb_handler,
                    BASIC_PRIO, NULL, BCM_RCO_F_ALL_COS)) < 0) {
            cli_out("%s: bcm_rx_register failed: %s\n",
                    ARG_CMD(args), bcm_errmsg(r));
            return CMD_FAIL;
        }

        cli_out("NOTE:  'debugmod diag rx' required for rxmon output\n");

    } else if (sal_strcasecmp(c, "stop") == 0) {
        if ((r = bcm_rx_stop(unit, &rx_cfg)) < 0) {
            cli_out("%s: Error: Cannot stop RX: %s.  Is it running?\n",
                    ARG_CMD(args), bcm_errmsg(r));
            return CMD_FAIL;
        }
        /* Unregister handler */
        if ((r = bcm_rx_unregister(unit, rx_cb_handler, BASIC_PRIO)) < 0) {
            cli_out("%s: bcm_rx_unregister failed: %s\n",
                    ARG_CMD(args), bcm_errmsg(r));
            return CMD_FAIL;
        }

    } else if (sal_strcasecmp(c, "show") == 0) {
#ifdef	BROADCOM_DEBUG
        bcm_rx_show(unit);
#else
	cli_out("%s: ERROR: cannot show in non-BROADCOM_DEBUG compilation\n",
                ARG_CMD(args));
	return CMD_FAIL;
#endif	/* BROADCOM_DEBUG */
    } else {
        return CMD_USAGE;
    }

    return rv;
}

static int free_buffers;

cmd_result_t
cmd_esw_rx_cfg(int unit, args_t *args)
/*
 * Function:    rx
 * Purpose:     Perform simple RX test
 * Parameters:  unit - unit number
 *              args - arguments
 * Returns:     CMD_XX
 */
{
    int chan;
    parse_table_t	pt;
    bcm_cos_queue_t queue_max;
    /* This isn't configurable per unit yet. */
    int cos_pps[BCM_RX_COS];
    char cospps_str[BCM_RX_COS][20];
    uint8 cos_pps_init = 0;
    int rv = BCM_E_NONE;
    int i;
    int system_pps = 0;

    if (!cos_pps_init) {
        for (i = 0; i < BCM_RX_COS; i++) {
            cos_pps[i] = 100;
        }
        cos_pps_init = 1;
    }

    if (!sh_check_attached(ARG_CMD(args), unit)) {
        return(CMD_FAIL);
    }

    if (BCM_FAILURE(bcm_rx_queue_max_get(unit, &queue_max))) {
        return (CMD_FAIL);
    }

    if (!ARG_CUR(args)) {
        int spps;

        /* Display current configuration */
        cli_out("Current RX configuration:\n");
        cli_out("    Pkt Size %d. Pkts/chain %d. All COS PPS %d. Burst %d\n",
                rx_cfg.pkt_size, rx_cfg.pkts_per_chain,
                rx_cfg.global_pps, rx_cfg.max_burst);
        for (chan = 0; chan < BCM_RX_CHANNELS; chan++) {
            cli_out("    Channel %d:  Chains %d. PPS %d. COSBMP 0x%x.\n",
                    chan, rx_cfg.chan_cfg[chan].chains,
                    rx_cfg.chan_cfg[chan].rate_pps,
                    rx_cfg.chan_cfg[chan].cos_bmp);
        }
        if ((rv = bcm_rx_cpu_rate_get(unit, &spps)) < 0) {
            cli_out("ERROR getting system rate limit:  %s\n",
                    bcm_errmsg(rv));
        } else {
            cli_out("    System wide rate limit:  %d\n", spps);
        }
        return CMD_OK;
    }

    if (isint(ARG_CUR(args))) {
        chan = parse_integer(ARG_GET(args));
        if (chan < 0 || chan >= BCM_RX_CHANNELS) {
            cli_out("Error: Bad channel %d\n", chan);
            return CMD_FAIL;
        }
    } else {
        chan = -1;
    }

    parse_table_init(unit, &pt);

    /* SPPS must be first, or else adjust pt_entries index below */
    parse_table_add(&pt, "SPPS", PQ_DFL|PQ_INT, 0,
                    &system_pps, NULL);
    parse_table_add(&pt, "GPPS", PQ_DFL|PQ_INT, 0,
                    &rx_cfg.global_pps, NULL);
    parse_table_add(&pt, "PKTSIZE", PQ_DFL|PQ_INT, 0,
                    &rx_cfg.pkt_size, NULL);
    parse_table_add(&pt, "PPC", PQ_DFL|PQ_INT, 0,
                    &rx_cfg.pkts_per_chain, NULL);
    parse_table_add(&pt, "BURST", PQ_DFL|PQ_INT, 0,
                    &rx_cfg.max_burst, NULL);
    parse_table_add(&pt, "FREE", PQ_DFL|PQ_BOOL, 0,
                    &free_buffers, NULL);
    
    if (queue_max >= BCM_RX_COS) {
        cli_out("Error: Too many queues %d > %d\n", queue_max, BCM_RX_COS);
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    for (i = 0; i < queue_max; i++) {
        sal_sprintf(cospps_str[i], "COSPPS%d", i);
        parse_table_add(&pt, cospps_str[i], PQ_DFL|PQ_INT, 0,
                        &cos_pps[i], NULL);
    }

    if (chan >= 0) {
        parse_table_add(&pt, "CHAINS", PQ_DFL|PQ_INT, 0,
                        &rx_chan_cfg.chains, NULL);
        parse_table_add(&pt, "PPS", PQ_DFL|PQ_INT, 0,
                        &rx_chan_cfg.rate_pps, NULL);
        parse_table_add(&pt, "COSBMP", PQ_DFL|PQ_HEX, 0,
                        &rx_chan_cfg.cos_bmp, NULL);
    }

    /* Parse remaining arguments */
    if (0 > parse_arg_eq(args, &pt)) {
	cli_out("%s: Error: Invalid option or malformed expression: %s\n",
                ARG_CMD(args), ARG_CUR(args));
	parse_arg_eq_done(&pt);
	return(CMD_FAIL);
    }

    /* Check if SPPS was entered; if so do only that */
    if (pt.pt_entries[0].pq_type & PQ_PARSED) {
        rv = bcm_rx_cpu_rate_set(unit, system_pps);
        parse_arg_eq_done(&pt);
        if (rv < 0) {
            cli_out("Warning:  system rate set (to %d) returned %s\n",
                    system_pps, bcm_errmsg(rv));
            return CMD_FAIL;
        }
        return CMD_OK;
    }

    parse_arg_eq_done(&pt);

    for (i = 0; i < queue_max; i++) {
        rv = bcm_rx_cos_rate_set(unit, i, cos_pps[i]);
        if (rv < 0) {
            cli_out("Warning:  cos rate set(%d to %d) returned %s\n", i,
                    cos_pps[i], bcm_errmsg(rv));
        }
    }

    if (chan >=0 ) { /* Copy external chan cfg to right place */
        sal_memcpy(&rx_cfg.chan_cfg[chan], &rx_chan_cfg,
                   sizeof(bcm_rx_chan_cfg_t));
    }
    return CMD_OK;
}

