/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Packet Watcher
 */

#include <sal/types.h>
#include <shared/bsl.h>
#ifndef __KERNEL__
#include <sys/types.h>
#include <netinet/in.h>
#endif

#include <soc/debug.h>
#include <soc/cm.h>
#include <soc/dma.h>

#include <sal/appl/io.h>
#include <sal/appl/sal.h>
#include <sal/core/thread.h>
#include <sal/core/sync.h>
#include <sal/core/spl.h>
#include <sal/core/libc.h>

#include <appl/diag/shell.h>
#include <appl/diag/parse.h>

#include <soc/drv.h>
#include <appl/diag/system.h>
#include <appl/diag/decode.h>

#include <bcm/rx.h>
#include <bcm/error.h>

#if defined (BCM_DNX_SUPPORT)
#include <src/appl/reference/dnx/appl_ref_rx_init.h>
#endif

#ifdef GHS
#define d_packet_format(pfx, decode, pkt, count, p)
#endif

/*
 * Maximum packet size we will read in.
 */
#define	PW_PACKET_SIZE		10240

#define PW_COUNT_DEFAULT	100

static char pw_name[SOC_MAX_NUM_DEVICES][16];

#define PW_COS_BMP_DFLT     0xFF
#define PW_RX_CHAINS_DFLT   8

/*
 * Define 2 of the public fields in the dv structure for our use.
 */
#define	dv_pw_pup	dv_public1.ptr
#define	dv_pw_chan	dv_public2.u32


/*
 * Typedef:	pup_t
 * Purpose:	Maps out each packet array entry - pkt_cnt is sort of
 * 		useless, but it makes life easier so we use it to track
 *		the pack # received.
 *
 * Notes:	Each entry is either FREE or on a circular/double linked 
 *		list used for the LOG. When the LOG is full, there are 3 extra
 *		pups used to suppor the up to 3 outstanding DMA operations.
 */
typedef struct pup_s {
    struct pup_s *pup_next, *pup_prev;	/* Next and previous pointers */
    dv_t	    *pup_esw_dv; 	/* DMA descriptor, ESW SOC mode only */
    int		pup_seqno; 		/* Packet # */
    void	*pup_pkt; 		/* Actual packet pointer, for soc */
    bcm_pkt_t   rx_pkt;                 /* Packet data for RX */
} pup_t;

#define REPORT_COUNT	0x01		/* Report packet count */
#define	REPORT_RAW	0x02		/* Report Raw packet data */
#define REPORT_DECODE	0x04		/* Report Decode */
#define	REPORT_DMA	0x08		/* Report RX Dma */
#define	REPORT_CHANNEL	0x10		/* Report RX channel */

typedef	struct pwu_s {
    volatile uint32 pu_flags;		/* Flags this unit PU_F_XXX */
#   define	PU_F_RUN	0x01
#   define	PU_F_STOP	0x02
#   define	PU_F_SYNC	0x04	/* Wake up request thread */
    sal_mutex_t	pu_lock;		/* Syncronize updates */
    sal_spinlock_t	pu_spinlock;	/* Protect updates */
    int32	pu_channel;		/* RX channel list 1 << ch # */
#   define	PU_CHANNEL(_c)	(1 << (_c))
    uint32	pu_report;		/* Reporting level */
    uint32	pu_dump_options;	/* Reporting level when dumping logs */
    COMPILER_DOUBLE	pu_count_last;	/* Last count displayed */
    COMPILER_DOUBLE	pu_count_time;	/* Last time count was displayed */
    sal_thread_t pu_pid;		/* pid this unit */
    sal_sem_t	pu_sema;		/* Sleep semaphore this unit */
    sal_sem_t	pu_sync;		/* Sync command semaphore */

    /*
     * pu_pending - a single linked list, where the head's prev pointer
     *                 points to the tail.
     * pu_ch_active  - pointers to DMA active on the given channel.
     */

    VOL pup_t	*pu_pending; 	/* Pending interrupt servicing */
    VOL pup_t	*pu_ch_active[MAX_N_DMA_CHAN+1]; /* Active DMA operations */
    int		pu_ch_count[MAX_N_DMA_CHAN+1]; /* # received on channel */
    pup_t	*pu_log;		/* Last N packets - head is LRU */
    pup_t	*pu_log_free;		/* Last N packets - head is LRU */
    pup_t	*pup_list;		/* List of all PUPs for free */
    int		pu_log_cnt;		/* # packets on LOG list */
    int		pu_log_max;		/* Max # packets logged */
#   define	PU_LOG_CNT	64	/* Default # log entries */
    uint32	pu_packet_received;	/* total # received packets */
    int		pu_interval;		/* usec between pkt rcv & next dma */

    int         mode;                   /* What mode are we in */
#define         PW_MODE_RX              0
#define         PW_MODE_SOC             1
#define         PW_MODE_PMUX            2
#define PW_INIT_STRINGS \
    "RX", \
    "SOC", \
    "PMUX"
    int         chan;
    int         rx_pri;
    uint32      rx_flags;
    int         pkt_size;
    bcm_rx_cfg_t  rx_cfg;
    int         rate;      /* Current global rate, pps */
    int         last_pkt_count;   /* How many pkts processed last wake */

    int         init_done;
    uint32      hdr_dump;         /* Header dump for cmicx */
} pwu_t;

char *pw_modes[3] = {PW_INIT_STRINGS};

STATIC bcm_rx_t         pw_rx_callback(int unit, bcm_pkt_t *pkt,
                                       void *cookie);

STATIC void             pw_cleanup_dma(int unit);
STATIC pup_t *          pw_pup_alloc(int unit);
STATIC void             pw_pup_free(int unit, pup_t *pup);
STATIC void             pw_dump_packet(int unit, pup_t *pup, uint32 report);
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
STATIC void             pw_esw_done_intr(int unit, dv_t *dv);
STATIC int              pw_channel_config(int unit);
#endif /* BCM_ESW_SUPPORT */
STATIC cmd_result_t     pw_start_channel(int unit, dma_chan_t chan);
STATIC void             pw_thread(void *up);
STATIC void             pw_process_pending(int unit);
STATIC cmd_result_t     pw_dump_log(int unit, int n);

char pw_usage[] =
    "Parameters: [report +/-<values>] [stop|start] [count]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "\tlog # - specifies # packets to remember\n"
    "\tdump [# | options] - # > 0 dumps # packets from start of log\n"
    "\t                     # < 0 dumps last # packets\n"
    "\t                     nothing - dumps all logs\n"
    "\t                     options - count   - print packet # only\n"
    "\t                               decode  - attempt to decode packet\n"
    "\t                               raw     - dump raw byte stream (host order)\n"
    "\t                               dma     - dump DMA descriptors (SOC mode only)\n"
    "\t                               channel - dump DMA channel\n"
    "\t                               Prepend + to set, - to clear, nothing to toggle\n"
    "\tinterval # - # us to wait after a packet arrives before next\n"
    "\t        DMA starts (SOC mode only)\n"
    "\treport- count   - print packet # only\n"
    "\t        decode  - attempt to decode packet\n"
    "\t        raw     - dump raw byte stream (host order)\n"
    "\t        dma     - dump DMA descriptors (SOC mode only)\n"
    "\t        channel - dump DMA channel\n"
    "\t        Prepend + to set, - to clear, nothing to toggle\n"
    "\tquiet - disable all reporting\n"
    "\tchannel x [y z] - configure PW to use the specified RX channels\n"
    "\trate    n       - Set the globally accepted pps rate\n"
    "\tstart - start the PW-daemon\n"
    "\tstop  - stop the PW-daemon and deallocate log space\n"
    "\tcount - print # packets received.\n"
    "\treset - reinitialize configuration from bcm_rx_* API state\n"
    "\tmode [soc | rx]  - set packetwatcher mode\n"
    "\t        With no parameters, reports current mode\n"
    "\t        soc      - Use the soc layer calls directly\n"
    "\t        rx       - Use the RX API\n"
#endif
    ;

pwu_t	pw_units[SOC_MAX_NUM_DEVICES];

STATIC void
pw_init(int unit)
{
    pwu_t	*pu = &pw_units[unit];	/* Control this unit */

    if (pu->init_done) {
        return;
    }

    sal_snprintf(pw_name[unit], sizeof(pw_name[unit]), "bcmPW.%d", unit);

    /* 
     * sync and lock semaphores are only created once, the others
     * are created and destroyed every time the packet watcher is
     * started and stopped.
     *
     * We need to check if they already exist to support the
     * pw reset option.
     */
    if (!pu->pu_sync) {
        pu->pu_sync = sal_sem_create("pw_sync", sal_sem_BINARY, 0);
    }
    if (!pu->pu_lock) {
        pu->pu_lock = sal_mutex_create("pw_lock");
    }
    if (!pu->pu_spinlock) {
        pu->pu_spinlock = sal_spinlock_create("pw_spinlock");
    }

    if (!pu->pu_sync || !pu->pu_lock || !pu->pu_spinlock) {
        cli_out("%s ERROR:  Could not allocate sync/lock\n", pw_name[unit]);
    }
    pu->chan = 0;
    pu->rx_pri = 100;
    pu->rx_flags = BCM_RCO_F_ALL_COS;
    pu->pkt_size = 10 * 1024;

    bcm_rx_cfg_get(unit, &pu->rx_cfg);
    pu->rate = pu->rx_cfg.global_pps;

    pu->init_done = TRUE;
}

STATIC int
_pw_set_rate(int unit)
{
    pwu_t	*pu = &pw_units[unit];	/* Control this unit */
    int          rv = BCM_E_NONE;

    /* Always calculate SOC interval in case modes are switched. */
    if (pu->rate <= 0) {
	pu->pu_interval = 1000000;
	pu->rate = 0;
    } else {
	pu->pu_interval = 1000000 / pu->rate;
    }

    switch (pu->mode) {
        case PW_MODE_RX:
            if ((rv = bcm_rx_rate_set(unit, pu->rate)) < 0) {
                cli_out("PW RX rate set error: %s.\n", bcm_errmsg(rv));
            }
            /* Then set the burst size to equal the rate */
            if ((rv = bcm_rx_burst_set(unit, pu->rate)) < 0) {
                cli_out("PW RX burst set error: %s.\n", bcm_errmsg(rv));
            }
            break;
        case PW_MODE_SOC:
            break;
        default:
            break;
    }

    return rv;
}

#define	PW_LOCK(_u)	sal_mutex_take(pw_units[_u].pu_lock, \
				       sal_mutex_FOREVER)
#define	PW_UNLOCK(_u)	sal_mutex_give(pw_units[_u].pu_lock)
#define	PW_SPIN_LOCK(_u)    sal_spinlock_lock(pw_units[_u].pu_spinlock)
#define	PW_SPIN_UNLOCK(_u)  sal_spinlock_unlock(pw_units[_u].pu_spinlock)

int
pw_start(int unit, int sync)
/*
 * Function: 	pw_start
 * Purpose:	Start the packet watcher
 * Parameters:	unit - unit to start.
 *		sync - if true, wait for watcher to start.
 * Returns:	0 - success, -1 failed.
 */
{
    pwu_t	*pu = &pw_units[unit];	/* Control this unit */

    pw_init(unit);
    if (pu->pu_sync == NULL || pu->pu_lock == NULL || pu->pu_spinlock == NULL) {
        return -1;
    }

    sal_memset(pu->pu_ch_active, 0, sizeof(pu->pu_ch_active));
    sal_memset(pu->pu_ch_count, 0, sizeof(pu->pu_ch_count));

    pu->pu_pending = NULL;
    pu->pu_log = NULL;
    pu->pu_log_free = NULL;
    pu->pu_log_cnt = 0;
#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(unit) ) {
       if (bcm_rx_active(unit)) {
          if (bcm_rx_stop(unit, NULL) < 0) {
              cli_out("%s: Failed to stop RX\n", pw_name[unit]);
              return -1;
          }
       }
    }
#endif /* BCM_PETRA_SUPPORT */
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit) ) {
       if (bcm_rx_active(unit)) {
          if (bcm_rx_stop(unit, NULL) < 0) {
              cli_out("%s: Failed to stop RX\n", pw_name[unit]);
              return -1;
          }
       }
    }
#endif /* BCM_DNX_SUPPORT */


#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
    if (SOC_IS_ESW(unit) || SOC_IS_ARAD(unit)) {
        if (pw_channel_config(unit) < 0) {
	    return -1;
        }
    }
#endif
#if defined(BCM_DNX_SUPPORT)
    if (SOC_IS_DNX(unit)) {
        if (pw_channel_config(unit) < 0) {
            return -1;
        }
    }
#endif

    pu->pu_sema = sal_sem_create("pw_thread", sal_sem_BINARY, 0);
    if (pu->pu_sema == 0) {
        return -1;
    }

    pu->pu_flags |= PU_F_RUN;
    if (sync) {
	pu->pu_flags |= PU_F_SYNC;
    }

    pu->pu_pid =
	sal_thread_create(pw_name[unit], SAL_THREAD_STKSZ,
			  soc_property_get(unit, spn_DIAG_PW_THREAD_PRI, 100),
			  pw_thread, INT_TO_PTR(unit));

    if (pu->pu_pid == SAL_THREAD_ERROR) {
	cli_out("%s: Unable to start task\n", pw_name[unit]);
	sal_sem_destroy(pu->pu_sema);
	pu->pu_sema = 0;
	pu->pu_flags &= PU_F_RUN|PU_F_SYNC;
	return -1;
    }

    pu->pu_count_last = 0;
    pu->pu_count_time = SAL_TIME_DOUBLE();

    pu->pu_packet_received = 0;

    if (sync) {
	if (sal_sem_take(pu->pu_sync, sal_sem_FOREVER)) {
	    cli_out("%s: Failed to wait for start\n", pw_name[unit]);
	    return -1;
	}
    }

    return 0;
}    

cmd_result_t 
pw_stop(int unit, int sync)
/*
 * Function: 	pw_stop
 * Purpose:	Stop the packet watcher
 * Parameters:	unit - unit to stop.
 *		sync - if true, wait for watcher to finish.
 * Returns:	0 - success, -1 failed.
 */
{
    pwu_t	*pu = &pw_units[unit];	/* Control this unit */	
    soc_timeout_t  to;
    sal_usecs_t     pw_timeout;

    pw_timeout = soc_property_get(unit, spn_CDMA_TIMEOUT_USEC, 10000000);

    pu->pu_flags |= PU_F_STOP;
    if (sync) {
	pu->pu_flags |= PU_F_SYNC;
    }
    sal_sem_give(pu->pu_sema);
    soc_timeout_init(&to, pw_timeout, 0);

    /* Check for pw_thread exit */
    while (pu->pu_pid != SAL_THREAD_ERROR) {
	  /* Handle the case of pw_thread not exiting */
         if (soc_timeout_check(&to)) {
             cli_out("pw_stop: pw_thread did not exit\n");
             pu->pu_pid = SAL_THREAD_ERROR;
             break;
         }
	  
         sal_usleep(80000);
    }
	
    if  (pu->pu_pid == SAL_THREAD_ERROR) {
        /* Abnormal thread exit: Free semaphore resource */
    /*    coverity[check_after_deref]    */
        if (pu->pu_sema) {
            /* Must check, because may have been destroyed by pw_exit */
            sal_sem_destroy(pu->pu_sema);
            pu->pu_sema = 0;
        }
    }

    if (sync) {
	    (void)sal_sem_take(pu->pu_sync, sal_sem_FOREVER);
    } else {
	    cli_out("%s: Termination requested...\n", pw_name[unit]);
    }
    return CMD_OK;
}

int
pw_running(int unit)
/*
 * Function:	pw_running
 * Purpose:	Determine if packet watcher is running for the specified
 *		unit.
 * Parameters:	unit - unit #
 * Returns:	0 - not running, !0 - running
 */
{
    return pw_units[unit].pu_flags & PU_F_RUN;
}

STATIC	void
pw_exit(int unit, int stat)
/*
 * Function:	pw_exit
 * Purpose:	Exit a packetwatcher thread.
 * Parameters:	unit - unit #.
 * 		stat - exit status (passed to exit).
 * Returns:	Does not return.
 *
 * Notes:	ASSUMES LOCK held on entry, released in this routine
 */
{
    pwu_t *pu = &pw_units[unit];
    int rv;
    char thread_name[SAL_THREAD_NAME_MAX_LEN];
    sal_thread_t	thread;
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
    bcm_rx_cfg_t cfg;
#endif

    thread = sal_thread_self();
    thread_name[0] = 0;
    sal_thread_name(thread, thread_name, sizeof (thread_name));
    
    switch (pu->mode) {
        case PW_MODE_RX:
            /* pw_exit() is called holding the lock. Give it up
             * momentarily while we stop the RX task.
             */
            PW_UNLOCK(unit);
            if ((rv = bcm_rx_stop(unit, NULL)) < 0) {
                cli_out("PW stop error: Cannot stop RX: %s.\n",
                        bcm_errmsg(rv));
            }
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
#if defined(BCM_DNX_SUPPORT)
            /* init rx cfg */
            if (SOC_IS_DNX(unit)) {
                if ((rv = appl_dnx_rx_init(unit)) < 0) {
                    cli_out("PW re-start error: Cannot re-start RX: %s.\n",
                        bcm_errmsg(rv));
                }
            }
            else
#endif
            {
                /* init rx cfg as no cos */
                bcm_rx_cfg_t_init(&cfg);
                cfg.pkt_size = 16*1024;
                cfg.pkts_per_chain = 15;
                cfg.global_pps = 0;
                cfg.max_burst = 15;
                cfg.rx_alloc = NULL;
                cfg.rx_free = NULL;
                cfg.flags = 0;
                cfg.chan_cfg[1].chains = 4;
                cfg.chan_cfg[1].flags = 0;
                cfg.chan_cfg[1].cos_bmp = 0xFffffffF;

                if ((rv = bcm_rx_start(unit, &cfg)) < 0) {
                    cli_out("PW re-start error: Cannot re-start RX: %s.\n",
                            bcm_errmsg(rv));
                }
            }
#endif
            if ((rv = bcm_rx_queue_unregister(unit, BCM_RX_COS_ALL, 
                                              pw_rx_callback, pu->rx_pri))
                       < 0) {
                cli_out("PW stop error: Cannot unregister handler: %s.\n",
                        bcm_errmsg(rv));
            }
            PW_LOCK(unit);
            break;
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
        case PW_MODE_SOC:
            PW_UNLOCK(unit);
            if (bcm_rx_active(unit)) {
                if ((rv = bcm_rx_stop(unit, NULL)) < 0) {
                    cli_out("PW stop error: Cannot stop RX: %s.\n",
                            bcm_errmsg(rv));
                }
            }
#if defined(BCM_DNX_SUPPORT)
            /* re-start rx */
            if (SOC_IS_DNX(unit)) {
                if ((rv = appl_dnx_rx_init(unit)) < 0) {
                    cli_out("PW re-start error: Cannot re-start RX: %s.\n",
                        bcm_errmsg(rv));
                }
            }
            else
#endif
            {
                /* init rx cfg as no cos */
                bcm_rx_cfg_t_init(&cfg);
                cfg.pkt_size = 16*1024;
                cfg.pkts_per_chain = 15;
                cfg.global_pps = 0;
                cfg.max_burst = 15;
                cfg.rx_alloc = NULL;
                cfg.rx_free = NULL;
                cfg.flags = 0;
                cfg.chan_cfg[1].chains = 4;
                cfg.chan_cfg[1].flags = 0;
                cfg.chan_cfg[1].cos_bmp = 0xFffffffF;
                if ((rv = bcm_rx_start(unit, &cfg)) < 0) {
                    cli_out("PW re-start error: Cannot re-start RX: %s.\n",
                            bcm_errmsg(rv));
                }
            }
            PW_LOCK(unit);
            break;
#endif
        default:
            break;
    }

    if (pu->pu_sema) {
	sal_sem_destroy(pu->pu_sema);
	pu->pu_sema = 0;
    }

    pu->pu_pid = SAL_THREAD_ERROR;

    /* Put logs on the free list - not fast but .... oh well */

    if (pu->pu_log != NULL) {
	pu->pu_log->pup_prev->pup_next = NULL; /* NUll list */
	while (pu->pu_log != NULL) {
	    pup_t	*pup = pu->pu_log->pup_next;
	    pw_pup_free(unit, pu->pu_log);
	    pu->pu_log = pup;
	}
    }
    
    /* Now free the free list */

    while (pu->pu_log_free) {
	pup_t	*pup = pu->pu_log_free;
	pu->pu_log_free = pup->pup_next;
        if (pu->mode == PW_MODE_SOC) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
            soc_dma_dv_free(unit, pup->pup_esw_dv);
#endif
            soc_cm_sfree(unit, pup->pup_pkt);
        }
    }
    if (pu->pup_list) {
        sal_free(pu->pup_list);
    }

    if (pu->pu_flags & PU_F_SYNC) {
	sal_sem_give(pu->pu_sync);
    } else {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "PW-Daemon[%d]: Exiting\n"), unit));
    }

    pu->pu_flags = 0;

    PW_UNLOCK(unit);   
    if (stat < 0) {
        LOG_ERROR(BSL_LS_SOC_PKT,
                  (BSL_META_U(unit,
                              "AbnormalThreadExit:%s\n"), thread_name));  
    }
    
    sal_thread_exit(stat);
}

/* Start the packet watcher operation depending on the mode */
STATIC int
_pw_start_op(int unit)
{
    pwu_t       *pu = &pw_units[unit];
    int pkt_size;
    int rv = BCM_E_NONE;
    dma_chan_t  chan;
    dma_chan_t  dma_chan;

    if (soc_feature(unit, soc_feature_cmicx)) {
        dma_chan = BCM_CMICX_RX_CHANNELS;
    } else {
        dma_chan = BCM_RX_CHANNELS;
    }

    switch (pu->mode) {
    case PW_MODE_RX:
        pu->rx_cfg.global_pps = pu->rate;

        /*
         * Allow the packet watcher to run in truncating mode
         * by allocating smaller Rx buffers and accepting
         * oversized packets on all Rx DMA channels.
         */
        pkt_size = soc_property_get(unit, spn_DIAG_PW_BUFFER_SIZE, -1);

        if (pkt_size >= 68) {
            pu->rx_cfg.pkt_size = pkt_size;

            for (chan = 0; chan < dma_chan; chan++) {
                pu->rx_cfg.chan_cfg[chan].flags |= BCM_RX_F_OVERSIZED_OK;
            }
        }

        if ((rv = bcm_rx_start(unit, &pu->rx_cfg)) < 0) {
            return rv;
        }
        if ((rv = bcm_rx_queue_register(unit, "RX CMD", BCM_RX_COS_ALL, 
                      pw_rx_callback, pu->rx_pri, NULL, pu->rx_flags)) < 0) {
            return rv;
        }

        break;
    case PW_MODE_SOC:
        if (pu->pu_channel == 0) {              /* Use default RX channel */
#ifdef BCM_DNX_SUPPORT
            if (SOC_IS_DNX(unit) ) {
                for (chan = 0; chan < dma_chan; chan++) {
                    /* RX_CHAN_USED */
                    if(pu->rx_cfg.chan_cfg[chan].chains != 0) {
                        pw_start_channel(unit, chan);
                    }
                }
            }
            else
#endif /* BCM_DNX_SUPPORT */
            {
                pw_start_channel(unit, -1);
            }
        } else {
            for (chan = 0; chan < dma_chan; chan++) {
                if (pu->pu_channel & PU_CHANNEL(chan)) {
                    pw_start_channel(unit, chan);
                }
            }
        }
        break;
    default:
        assert("Unknown pkt watcher mode");
        break;
    }

    return rv;
}

STATIC void 
pw_thread(void *up)
/*
 * Function:    pw_thread
 * Purpose:     Waits for packets to arrive destined for the CPU and
 *              saves them in a queue. 
 * Parameters:  unit - SOC unit #
 * Returns:     Does not return.
 */
{
    int         unit = PTR_TO_INT(up);
    int         i;
    pwu_t       *pu = &pw_units[unit];
    int         rv;
    int         pup_count;
    pup_t      *pup;

    PW_LOCK(unit);

    /* Allocate packet pointer array */ 

    if (0 == pu->pu_log_max) {
        pu->pu_log_max = PW_COUNT_DEFAULT;
    }

    /* Extra for multiple DMA channels */
    if (pu->mode == PW_MODE_SOC) {
        if (soc_feature(unit, soc_feature_cmicx)) {
            pup_count = pu->pu_log_max + CMICX_N_DMA_CHAN;
        } else {
            pup_count = pu->pu_log_max + N_DMA_CHAN;
        }
    } else {
        pup_count = pu->pu_log_max;
    }
    
    /* Bump up pup_count to be twice the log size so pups will
     * be available even when log queue is full.
     */
    pup_count += pu->pu_log_max;

    pu->pup_list = sal_alloc(sizeof(pup_t) * pup_count, "PW-pup");
    if (pu->pup_list == NULL) {
        pw_exit(unit, -1);
    }
    /* No forwaqrding null. If the allocation failed, the call to pw_exit will end the process and we sould never reach here. */
    /* coverity[var_deref_model:FALSE] */
    sal_memset(pu->pup_list, 0, sizeof(pup_t) * pup_count);

    for (i = 0; i < pup_count; i++) {

        pup = &pu->pup_list[i];

        if (pu->mode == PW_MODE_SOC) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
            pup->pup_esw_dv = soc_dma_dv_alloc(unit, DV_RX, 1);
            if (pup->pup_esw_dv == NULL) {
                pw_exit(unit, -1);
            }
#endif                
            pup->pup_pkt = soc_cm_salloc(unit, PW_PACKET_SIZE, "pw_thread");
            if (pup->pup_pkt == NULL) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
                soc_dma_dv_free(unit, pup->pup_esw_dv);
#endif
                pw_exit(unit, -1);
            }
        }
        pw_pup_free(unit, pup);         /* Put on free list */
    }

    if (pu->pu_flags & PU_F_SYNC) {
        pu->pu_flags &= ~PU_F_SYNC;
        sal_sem_give(pu->pu_sync);
    } else {
        cli_out("PW-daemon[%d] -- Started\n", unit);
    }

    /* Kick Start channels */
    if ((rv = _pw_start_op(unit)) < 0) {
        cli_out("PW rx mode: Cannot start %s: %s.\n", pw_modes[pu->mode],
                bcm_errmsg(rv));
        pw_exit(unit, -1);
    }

    PW_UNLOCK(unit);

    /*
     * Check if there are any actions for us.
     */
    while (1) {
        int     sem_rv;

        sem_rv = sal_sem_take(pw_units[unit].pu_sema, sal_sem_FOREVER);

        if (sem_rv < 0) {
            cli_out("Failed sem_take, exiting\n");
            pw_exit(unit, -1);
        }

        PW_LOCK(unit);                  /* Block log dumps */

        if (pu->pu_flags & PU_F_STOP) { /* Time to exit */
            if (pu->mode == PW_MODE_SOC) {
                pw_cleanup_dma(unit);
            }
            pw_exit(unit, 0);
        }

        pw_process_pending(unit);

        PW_UNLOCK(unit);

        /* If limiting using interval, sleep intvl * num pkts processed */
        if (pu->mode == PW_MODE_SOC) {
            if (pu->pu_interval != 0 && pu->last_pkt_count != 0) {
                sal_usleep(pu->pu_interval * pu->last_pkt_count);
            }
        }
    }

    pw_exit(unit, 0);
}

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
STATIC int
pw_channel_config(int unit)
/*
 * Function:    pw_channel_config
 * Purpose:	Configure DMA RX channels for PW.
 * Parameters:  unit - unit to configure.
 * Returns:	0 - success
 *		-1 - failed.
 */
{
    pwu_t	*pu = &pw_units[unit];	/* Control this unit */	
    dma_chan_t	tx, i;
    int		rv = SOC_E_NONE;
    dma_chan_t  dma_chan;
    int cos_bmp = 0;

    if (pu->pu_channel == 0) {		/* Use default */
	return 0;
    }

    if (soc_feature(unit, soc_feature_cmicx)) {
        dma_chan = CMICX_N_DMA_CHAN;
    } else {
        dma_chan = N_DMA_CHAN;
    }

    /* Pick lowest # free channel for TX default */
    tx = -1;
    for (i = 0; i < dma_chan; i++) {
        if (pu->pu_channel & PU_CHANNEL(i)) {
            rv = soc_dma_chan_config(unit, i, DV_RX, SOC_DMA_F_INTR);
            if (rv < 0) {
                cli_out("%s: DMA channel configuration failed: %s\n",
                        pw_name[unit], soc_errmsg(rv));
            }
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
            /* enable all cos on the channel */
            soc_dma_chan_cos_ctrl_set(unit, i, 1, 0xffffffff);
            soc_dma_chan_cos_ctrl_set(unit, i, 2, 0xffffffff);
#endif
        } else if (tx == -1) {
            tx = i;
        }
    }

    /* Update rx_cfg */
    for (i = 0; i < dma_chan; i++) {
        bcm_rx_cfg_t *cfg = &pu->rx_cfg;

        if (pu->pu_channel & PU_CHANNEL(i)) {
            if (cfg->chan_cfg[i].cos_bmp != 0) {
                cos_bmp = 1;
            }
            if (cfg->chan_cfg[i].chains == 0) {
                cfg->chan_cfg[i].chains = PW_RX_CHAINS_DFLT;
                if (!cos_bmp) {
                    cfg->chan_cfg[i].cos_bmp = PW_COS_BMP_DFLT;
                    cos_bmp = 1;
                }
            }
        } else {
            cfg->chan_cfg[i].cos_bmp = 0;
            cfg->chan_cfg[i].chains = 0;
        }

    }

    if ((tx == -1) || 
	((rv = soc_dma_chan_config(unit, tx, DV_TX,
				   SOC_DMA_F_INTR |SOC_DMA_F_DEFAULT)) < 0)) {
	cli_out("%s: Unable to configure TX DMA channel: %s\n", 
                pw_name[unit],
                tx == -1 ? "No remaining channels" : soc_errmsg(rv));

	rv = soc_dma_init(unit);
	cli_out("%s: Unable to re-initialize DMA: %s\n", 
                pw_name[unit],
                soc_errmsg(rv));

	return -1;
    }
    return 0;
}
#endif

static const parse_pm_t pw_report_table[] = {
    {"Count", 	REPORT_COUNT},
    {"DECode", 	REPORT_DECODE},
    {"Raw", 	REPORT_RAW},
    {"DMA", 	REPORT_DMA},
    {"CHannel",	REPORT_CHANNEL},
    {"@ALL",	~0},
    {"@*",	~0},
    {NULL, 	0}
};

static const parse_pm_t pw_channel_table[] = {
    {"0",	1 << 0},
    {"1",	1 << 1},
    {"2",	1 << 2},
    {"3",	1 << 3},
    {"4",	1 << 4},
    {"5",	1 << 5},
    {"6",	1 << 6},
    {"7",	1 << 7},
    {NULL,	0}
};

cmd_result_t
pw_command(int unit, args_t *a)
/*
 * Function:	pw_command
 * Purpose:	Command interface to the "packet watcher" thread.
 * Parameters:
 *		"verbose" - print lots of stuff.
 *		"start" - start packet watcher thread.
 *		"stop" - stop packet watcher thread.
 *		"quiet" - print no stuff.
 * Returns:	CMD_OK - success
 *		CMD_FAIL - failed
 *		CMD_USAGE - print usage instructions
 */
{
    char	*c;
    int		n;
    uint32	on = 0;			/* flags to turn on */
    pwu_t	*pu = &pw_units[unit];	/* Control this unit */

    if (!sh_check_attached(ARG_CMD(a), unit)) {
	return(CMD_FAIL);
    }

    if (!pu->init_done) {
        pw_init(unit);
    }

    if (!ARG_CNT(a)) {			/* Nothing passed */
	cli_out("%s: Status: %s. Mode %s. Buffering up to %d packets.%s\n",
                pw_name[unit],
                (pu->pu_flags & PU_F_RUN) ? "Running" : "Not Running",
                pw_modes[pu->mode], pu->pu_log_max,
                (pu->pu_flags & PU_F_STOP) ? " STOP Requested." : "");
	if (pu->mode == PW_MODE_SOC) {
            if (pu->pu_interval != 0) {
                cli_out("Interval %d us", pu->pu_interval);
            }
        }
        if (pu->rate <= 0) {
            cli_out("Rate limiting is off.\n");
        } else {
            cli_out("Rate limit is %d (soc intvl %d).\n", pu->rate,
                    pu->pu_interval);
        }
            
	cli_out("Reporting is enabled for: ");
	parse_mask_format(80, pw_report_table, pu->pu_report);
	cli_out("Reporting is disabled for: ");
	parse_mask_format(80, pw_report_table, pu->pu_report ^ ~0);

        cli_out("Dump options are enabled for: ");
	parse_mask_format(80, pw_report_table, pu->pu_dump_options);
	cli_out("Dump options are disabled for: ");
	parse_mask_format(80, pw_report_table, pu->pu_dump_options ^ ~0);

        cli_out("RX on for channel(s): ");
	if (pu->pu_channel) {
	    parse_mask_format(50, pw_channel_table, pu->pu_channel);
	    cli_out("RX off for channel(s): ");
	    parse_mask_format(50, pw_channel_table, ~pu->pu_channel);
	} else {
	    cli_out(" -- using default --\n");
	}
#ifdef BROADCOM_DEBUG
        if (pu->mode == PW_MODE_RX) {
            bcm_rx_show(unit);
        }
#endif /* BROADCOM_DEBUG */
	return(CMD_OK);
    }

    /* Scan args and see what's up */

    while ((c = ARG_GET(a)) != NULL) {
	if (!sal_strcasecmp(c, "start")) {
	    if (pu->pu_flags & PU_F_RUN) {
		cli_out("%s: WARNING: Already running\n", pw_name[unit]);
	    } else {
		on |= PU_F_RUN;
	    }
	} else if (!sal_strcasecmp(c, "stop")) {
	    if (!(pu->pu_flags & PU_F_RUN)) {
		cli_out("%s: WARNING: Not running\n", pw_name[unit]);
		return(CMD_OK);		/* Return OK to avoid rc aborts */
	    }
	    on |= PU_F_STOP;
	} else if (!sal_strcasecmp(c, "reset")) {
	    if (pu->pu_flags & PU_F_RUN) {
		cli_out("%s: Unable to reset configuration "
                        "while running\n", pw_name[unit]);
		return(CMD_FAIL);
	    } else {
                pu->init_done = FALSE;
                pw_init(unit);
                return CMD_OK;
            }
	} else if (!sal_strcasecmp(c, "channel")) {

	    if (ARG_CNT(a) == 0) {
		cli_out("%s: RX on for channel(s): ", pw_name[unit]);
		if (pu->pu_channel) {
		    parse_mask_format(50, pw_channel_table, pu->pu_channel);
		    cli_out("%s: RX off for channel(s): ", pw_name[unit]);
		    parse_mask_format(50, pw_channel_table, ~pu->pu_channel);
		} else {
		    cli_out(" -- using default --\n");
		}
		return(CMD_OK);
	    } else if (pu->pu_flags & PU_F_RUN) {
		cli_out("%s: Unable to configure DMA channels "
                        "while running\n", pw_name[unit]);
		return(CMD_FAIL);
	    }

	    /* Set channels */
	    while ((c = ARG_CUR(a)) != NULL && 
		   !parse_mask(c, pw_channel_table,
			       (uint32 *)&pu->pu_channel)) {
		ARG_NEXT(a);	/* Bump arg if matched report */
	    }

	    if (pu->pu_flags & PU_F_RUN) {
		cli_out("%s: Warning channel re-assignment will not take\n"
                        "%s: until restarted.\n",
                        pw_name[unit], pw_name[unit]);
	    }
	} else if (!sal_strcasecmp(c, "quiet")) {
	    pu->pu_report = 0;
	} else if (!sal_strcasecmp(c, "rate")) {
	    c = ARG_GET(a);
            if (c == NULL) {
                if (pu->rate <= 0) {
                    cli_out("%s: No rate limiting.\n", pw_name[unit]);
                } else {
                    cli_out("%s: Rate limit is currently %d pkts/sec.\n",
                            pw_name[unit], pu->rate);
                }
                return CMD_OK;
            }
            pu->rate = strtoul(c, NULL, 10);
            if (_pw_set_rate(unit) < 0) {
                return CMD_FAIL;
            }
            return CMD_OK;
	} else if (!sal_strcasecmp(c, "report")) {
	    if (ARG_CNT(a)) {
		while ((c = ARG_CUR(a)) != NULL && 
		       !parse_mask(c, pw_report_table, &pu->pu_report)) {
		    ARG_NEXT(a);	/* Bump arg if matched report */
		}
	    } else {			/* Print values */
		cli_out("%s: Reporting on for: ", pw_name[unit]);
		parse_mask_format(50, pw_report_table, pu->pu_report);
		cli_out("%s: Reporting off for: ", pw_name[unit]);
		parse_mask_format(50, pw_report_table, ~pu->pu_report);
	    }
	} else if (!sal_strcasecmp(c, "log")) {
	    c = ARG_GET(a);
	    if (c) {			/* We have an option */
		if (pu->pu_flags & PU_F_RUN) {
		    cli_out("%s: Can not change \"log\" "
                            "while PW-daemon running\n", pw_name[unit]);
		    return(CMD_FAIL);
		}			
		if (!sal_strcasecmp(c, "on")) {
		    n = PU_LOG_CNT;
		} else if (!sal_strcasecmp(c, "off")) {
		    n = 1;
		} else if (isint(c)) {	/* Set log size */
		    n = parse_integer(c);
		    if (n < 1) {
			cli_out("%s: What is %s?\n", pw_name[unit], c);
			n = 1;
		    }
		} else {
		    cli_out("%s: Invalid count \"%s\"\n", pw_name[unit], c);
		    return(CMD_FAIL);
		}
		pu->pu_log_max = n;
	    }
	    cli_out("%s: Logging(%d-packets)\n",
                    pw_name[unit], pu->pu_log_max);
	    if (!c || !*c) {
		return(CMD_OK);
	    }
	} else if (!sal_strcasecmp(c, "dump")) {
	    c = ARG_GET(a);
	    if (!c) {			/* no args */
		n = pu->pu_log_cnt;
	    } else if (!isint(c)) {
                if (!sal_strcasecmp(c, "options")) {
                    if (ARG_CNT(a)) {
                        while ((c = ARG_CUR(a)) != NULL && 
                               !parse_mask(c, pw_report_table,
                                           &pu->pu_dump_options)) {
                            ARG_NEXT(a);	/* Bump arg if matched report */
                        }
                    } else {			/* Print values */
                        cli_out("%s: Dump options on for: ", pw_name[unit]);
                        parse_mask_format(50, pw_report_table, pu->pu_dump_options);
                        cli_out("%s: Dump options off for: ", pw_name[unit]);
                        parse_mask_format(50, pw_report_table, ~pu->pu_dump_options);
                    }
                    return(CMD_OK);
                } else {
                    cli_out("%s: Invalid log count \"%s\"\n", pw_name[unit], c);
                    return(CMD_FAIL);
                }
            } else {
		n = parse_integer(c);
	    }
	    return(pw_dump_log(unit, n));
	} else if (!sal_strcasecmp(c, "count")) {
	    COMPILER_DOUBLE	cur_time, last_time;
	    int		cur_count, last_count;
	    int		rate;
	    
	    cur_time = SAL_TIME_DOUBLE();

	    last_time = pu->pu_count_time;
	    last_count = pu->pu_count_last;
	    cur_count = pu->pu_packet_received;

	    if (cur_time == last_time) {
		rate = 0;
	    } else {
		rate = (int) ((cur_count - last_count) /
			      (cur_time - last_time));
	    }

	    cli_out("%s: Received %d packets [(%d),%d,%d,%d,%d] (%d/sec), "
                    "last %d packet(s) logged\n",
                    pw_name[unit],
                    pu->pu_packet_received,
                    pu->pu_ch_count[0], 
                    pu->pu_ch_count[1], 
                    pu->pu_ch_count[2], 
                    pu->pu_ch_count[3], 
                    pu->pu_ch_count[4], 
                    rate,
                    pu->pu_log_cnt);

	    pu->pu_count_last = cur_count;
	    pu->pu_count_time = cur_time;
	} else if (!sal_strcasecmp(c, "interval")) {
	    c = ARG_GET(a);
	    if (!c || !isint(c)) {
		return(CMD_USAGE);
	    }
	    n = parse_integer(c);
	    if (n < 0) {
		cli_out("%s: Invalid interval: %s\n", pw_name[unit], c);
		return(CMD_FAIL);
	    }
	    pu->pu_interval = n;
	} else if (!sal_strcasecmp(c, "mode")) {
	    if ((c = ARG_GET(a)) == NULL) {
		cli_out("Current mode is %s\n", pw_modes[pu->mode]);
		return CMD_OK;
	    }

	    if (pw_running(unit)) {
		cli_out("Can't set modes while running\n");
		return CMD_FAIL;
	    }

	    if (!sal_strcasecmp(c, "soc")) {
		pu->mode = PW_MODE_SOC;
	    } else if (!sal_strcasecmp(c, "rx")) {
		pu->mode = PW_MODE_RX;
	    } else {
		cli_out("Unknown mode: %s\n", c);
		return CMD_FAIL;        
	    }

	    return CMD_OK;
        } else if (!sal_strcasecmp(c, "hdr")) {
            c = ARG_GET(a);
            if (c) {            /* We have an option */
                if (!sal_strcasecmp(c, "on")) {
                    pu->hdr_dump = 1;
                } else if (!sal_strcasecmp(c, "off")) {
                    pu->hdr_dump = 0;
                }
            }
	} else {
	    return(CMD_USAGE);
	}
    }
    
    /* Everything OK, start or stop packet watcher now */

    if (on & PU_F_RUN) {		/* start off thread */
	return(pw_start(unit, TRUE));
    } else if (on & PU_F_STOP) {	/* stop task */
        /* Restore to default settings. */
        pu->pu_channel = 0;
	return(pw_stop(unit, TRUE));
    }

    return(CMD_OK);
}

/****************************************************************
 *
 * Common packet watcher routines
 */


STATIC  pup_t *
pw_pup_alloc(int unit)
/*
 * Function:    pw_alloc
 * Purpose:     Alloc a pup off the free list.
 * Parameters:  unit - unit #
 * Returns:     Pointer to pup.
 */
{
    pwu_t       *pu = &pw_units[unit];
    pup_t       *pup;

    PW_LOCK(unit);
    pup = pu->pu_log_free;
    if (pup) {
        pu->pu_log_free = pup->pup_next;
    }
    PW_UNLOCK(unit);

    return(pup);
}

STATIC  void
pw_pup_free(int unit, pup_t *pup)
/*
 * Function:    pw_pup_free
 * Purpose:     Put a pup on the free list.
 * Parameters:  unit - unit #
 *              pup - pointer to pup to free.
 * Returns:     Nothing.
 */
{
    pwu_t       *pu = &pw_units[unit];

    pup->pup_next = pu->pu_log_free;
    pu->pu_log_free = pup;

    if (pu->mode == PW_MODE_RX) {
        bcm_rx_free(unit, pup->rx_pkt.alloc_ptr);
    }
}

STATIC void
pw_dump_start(int unit, char *pfx, pup_t *pup, uint32 report,
              int chan, int count)
{
    char        *channel;

    if (!report) {
        return;
    } 

    cli_out("\n");

    if (report & REPORT_CHANNEL) {
        switch (chan) {
        case -1:        channel = "";           break;
        case 0:         channel = "-chn0";      break;
        case 1:         channel = "-chn1";      break;
        case 2:         channel = "-chn2";      break;
        case 3:         channel = "-chn3";      break;
        case 4:         channel = "-chn4";      break;
        case 5:         channel = "-chn5";      break;
        case 6:         channel = "-chn6";      break;
        case 7:         channel = "-chn7";      break;
        default:        channel = "-----";      break;
        }
    } else {
        channel = "";
    }

    sal_sprintf(pfx, "Packet[%d]%s: ", pup->pup_seqno, channel);

    if ((report & REPORT_COUNT) && (count >= 0)) {
        cli_out("%sTotal %d\n", pfx, count);
    }

}


STATIC void
pw_dump_packet_soc(int unit, pup_t *pup, uint32 report)
/*
 * Function:    pw_dump_packet
 * Purpose:     dumps out a packet based on the flags set in pw_flags.
 * Parameters:  unit - SOC unit #.
 *              pup - packet structure
 *              report - report flags
 * Returns:     d - pointer to dcb containing packet
 */
{

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
        dcb_t       *esw_dcb = NULL;
#endif

    char        pfx[64];

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
    esw_dcb = pup->pup_esw_dv->dv_dcb;
#endif

    
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
    pw_dump_start(unit, pfx, pup, report, (int)pup->pup_esw_dv->dv_pw_chan, -1);
#endif

    if (report & REPORT_COUNT) {
        cli_out("%sReceived\n", pfx);
#ifdef BROADCOM_DEBUG
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
        SOC_DCB_REASON_DUMP(unit, esw_dcb, pfx);
#endif
#endif /* BROADCOM_DEBUG */
    }

    if (report & REPORT_DMA) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
        soc_dma_dump_dv(unit, pfx, pup->pup_esw_dv);
#endif
    }

    if (report & REPORT_RAW) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
        soc_dma_dump_pkt(unit, pfx,
                         (uint8 *) SOC_DCB_ADDR_GET(unit, esw_dcb), 
                         SOC_DCB_XFERCOUNT_GET(unit, esw_dcb),
			 TRUE);
#endif
    }

#ifndef NO_SAL_APPL
#ifndef __STRICT_ANSI__
#ifndef __KERNEL__
    if (report & REPORT_DECODE) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
        d_packet_format(pfx, DECODE_ETHER, pup->pup_pkt,
			SOC_DCB_XFERCOUNT_GET(unit, esw_dcb),
			SOC_IS_ENDURO(unit)?esw_dcb:NULL);
#endif
    }
#endif /* __KERNEL__ */
#endif /* __STRICT_ANSI__ */
#endif /* NO_SAL_APPL */
}

STATIC char *_pw_reason_names[bcmRxReasonCount] = BCM_RX_REASON_NAMES_INITIALIZER;

STATIC void
pw_dump_packet_rx(int unit, pup_t *pup, uint32 report)
{
    bcm_pkt_t *pkt;
    char        pfx[64];
    char        info_str[256];
    pwu_t       *pu = &pw_units[unit];
    int         i;
#ifndef NO_SAL_APPL
#ifndef __STRICT_ANSI__
#ifndef __KERNEL__
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
    dv_t        *pdv;
#endif
#endif /* __KERNEL__ */
#endif /* __STRICT_ANSI__ */
#endif /* NO_SAL_APPL */
#if defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
    int         hdr_len = 0;
#endif 

    pkt = &pup->rx_pkt;

#ifdef BCM_PETRA_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        hdr_len = pkt->tot_len - pkt->pkt_len;
    }
#endif
#ifdef BCM_DNX_SUPPORT
    if (SOC_IS_DNX(unit)) {
        hdr_len = pkt->tot_len - pkt->pkt_len;
    }
#endif

    pw_dump_start(unit, pfx, pup, report, pkt->dma_channel,
                  pu->pu_packet_received);

    if (report & REPORT_DMA) {
        if (pkt->_dv) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
            soc_dma_dump_dv(unit, pfx, (dv_t *)pkt->_dv);
#endif
        }
    }

    if (report & REPORT_RAW) {
#ifdef BCM_XGS12_FABRIC_SUPPORT
        if (SOC_IS_XGS12_FABRIC(unit)) {
            soc_dma_higig_dump(unit, pfx, pkt->_higig, 0, pkt->pkt_len, NULL);
        }
#endif
#if defined(BCM_PETRA_SUPPORT)
        if (SOC_IS_ARAD(unit)) {
            soc_dma_ether_dump(unit, pfx, pkt->_pkt_data.data + hdr_len, pkt->pkt_len, 0);
        }
        else
#endif
#if defined(BCM_DNX_SUPPORT)
        if (SOC_IS_DNX(unit)) {
            soc_dma_ether_dump(unit, pfx, pkt->_pkt_data.data + hdr_len, pkt->pkt_len, 0);
        }
        else
#endif
        {
#if defined(BCM_ESW_SUPPORT)
#if defined(BCM_CMICX_SUPPORT)
            if (soc_feature(unit, soc_feature_cmicx)) {
                if (pu->hdr_dump == 1) {
                    soc_dma_ep_to_cpu_hdr_dump(unit, pfx, pkt->_pkt_data.data - pkt->egress_to_cpu_hdr_size,
                            pkt->egress_to_cpu_hdr_size, 0);
                }

                if (report & REPORT_DMA) {
                    uint8 *c = (uint8 *)pkt->_pkt_data.data;
                    c -= pkt->egress_to_cpu_hdr_size;
#ifdef  LE_HOST
                {
                    /* Higig field accessors expect network byte ordering,
                     * so we must reverse the bytes on LE hosts */
                    int word;
                    uint32 *hg_data = (uint32 *)c;
                    for (word = 0; word < BYTES2WORDS(pkt->egress_to_cpu_hdr_size); word++) {
                        hg_data[word] = _shr_swap32(hg_data[word]);
                    }
                    c = (uint8 *)hg_data;
                }
#endif
                    soc_dma_higig_dump(unit, pfx, c, 0, 0, NULL);
                    soc_dma_ep_to_cpu_hdr_decoded_dump(unit, pfx, (void *)c, 1, pkt->egress_to_cpu_hdr_size);
                }
            }
#endif
            soc_dma_ether_dump(unit, pfx, pkt->_pkt_data.data, pkt->pkt_len, 0);
#endif
        }
        if (SOC_IS_ARAD(unit) || SOC_IS_DNX(unit)) {
            cli_out("%slength %d (%d). rx-port %d. cos %d. prio_int %d. vlan %d. "
                "reason 0x%x.\n", pfx,
                    pkt->pkt_len, pkt->tot_len, pkt->rx_port,
                    pkt->cos, pkt->prio_int, pkt->vlan,
                    pkt->rx_reason);
        }
        else {
            cli_out("%slength %d (%d). rx-port %d. cos %d. prio_int %d. vlan %d. "
                    "reason 0x%x. %s.\n", pfx,
                    pkt->pkt_len, pkt->tot_len, pkt->rx_port, 
                    pkt->cos, pkt->prio_int, pkt->vlan,
                    pkt->rx_reason,
                    (pkt->rx_untagged & BCM_PKT_OUTER_UNTAGGED) ?
                    ((pkt->rx_untagged & BCM_PKT_INNER_UNTAGGED) ?
                    "Untagged" : "Inner tagged") :
                    ((pkt->rx_untagged & BCM_PKT_INNER_UNTAGGED) ?
                    "Outer tagged" : "Double tagged"));
        }

        if ((pkt->stk_flags & BCM_PKT_STK_F_SRC_PORT) &&
            (pkt->stk_flags & BCM_PKT_STK_F_DST_PORT)) {

            sal_sprintf(info_str, 
                "dest-gport %d. src-gport %d. ",
                pkt->dst_gport, pkt->src_gport);
        } else if ((pkt->stk_flags & BCM_PKT_STK_F_SRC_PORT)) {

            sal_sprintf(info_str,
                "dest-port %d. dest-mod %d. src-gport %d. ",
                pkt->dest_port, pkt->dest_mod, pkt->src_gport);
        } else if ((pkt->stk_flags & BCM_PKT_STK_F_DST_PORT)) {

            sal_sprintf(info_str,
                "dest-gport %d. %s %d. src-mod %d. ",
                pkt->dst_gport, 
                (pkt->flags & BCM_PKT_F_TRUNK) ? "src-trunk" : "src-port",
                (pkt->flags & BCM_PKT_F_TRUNK) ? pkt->src_trunk : pkt->src_port, 
                pkt->src_mod);
        } else {

            sal_sprintf(info_str,
                "dest-port %d. dest-mod %d. %s %d. src-mod %d. ",
                pkt->dest_port, pkt->dest_mod,
                (pkt->flags & BCM_PKT_F_TRUNK) ? "src-trunk" : "src-port",
                (pkt->flags & BCM_PKT_F_TRUNK) ? pkt->src_trunk : pkt->src_port, 
                pkt->src_mod);
        }
        if (!SOC_IS_ARAD(unit)) {
            cli_out("%s%sopcode %d. %s matched %d. classification-tag %d.\n",
                pfx, info_str, 
                pkt->opcode,
                (pkt->flags & BCM_RX_TRUNCATED) ? "Truncated." : "",
                pkt->rx_matched, pkt->rx_classification_tag);
        }
        else {
            cli_out("%s%s%s \n",
                pfx, info_str, 
                (pkt->flags & BCM_RX_TRUNCATED) ? "Truncated." : "");
        }
        for (i = 0; i < bcmRxReasonCount; i++) {
            if (BCM_RX_REASON_GET(pkt->rx_reasons, i)) {
                cli_out("%sreasons: %s\n", pfx, _pw_reason_names[i]);
            }
        }

        /*
         * Seamless Redundancy(SR) copy to CPU
         * SR custom RX reason code (1~63) for IFP action bcmFieldActionSrCopyToCpu .
         * construct reason code (1~63) from 6-bit binary encoding
         */
        #define SR_CUSTOM_RX_REASON_CODE \
            ((BCM_RX_REASON_GET(pkt->rx_reasons, bcmRxReasonSrCopyToCpuBit0) << 0)| \
             (BCM_RX_REASON_GET(pkt->rx_reasons, bcmRxReasonSrCopyToCpuBit1) << 1)| \
             (BCM_RX_REASON_GET(pkt->rx_reasons, bcmRxReasonSrCopyToCpuBit2) << 2)| \
             (BCM_RX_REASON_GET(pkt->rx_reasons, bcmRxReasonSrCopyToCpuBit3) << 3)| \
             (BCM_RX_REASON_GET(pkt->rx_reasons, bcmRxReasonSrCopyToCpuBit4) << 4)| \
             (BCM_RX_REASON_GET(pkt->rx_reasons, bcmRxReasonSrCopyToCpuBit5) << 5))

        /* no copy to CPU when SR_CUSTOM_RX_REASON_CODE == 0 */
        if(SR_CUSTOM_RX_REASON_CODE != 0) {
            cli_out("%sSR custom RX reason code: %d\n",
                    pfx,
                    SR_CUSTOM_RX_REASON_CODE);
        }
    }

#ifndef NO_SAL_APPL
#ifndef __STRICT_ANSI__
#ifndef __KERNEL__
    if (report & REPORT_DECODE) {
        if (pkt->_dv) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
	    pdv = (dv_t *)(pkt->_dv);
#if defined(BCM_PETRA_SUPPORT)
            if (SOC_IS_ARAD(unit)) {
                d_packet_format(pfx, DECODE_ETHER, pkt->_pkt_data.data + hdr_len,
			        pkt->pkt_len, SOC_IS_ENDURO(unit)? pdv->dv_dcb:NULL);
            }
            else
#endif
#if defined(BCM_DNX_SUPPORT)
            if (SOC_IS_DNX(unit)) {
                d_packet_format(pfx, DECODE_ETHER, pkt->_pkt_data.data + hdr_len,
                    pkt->pkt_len, SOC_IS_ENDURO(unit)? pdv->dv_dcb:NULL);
            }
            else
#endif
            {
                d_packet_format(pfx, DECODE_ETHER, pkt->_pkt_data.data,
			        pkt->pkt_len, SOC_IS_ENDURO(unit)? pdv->dv_dcb:NULL);
            }
#endif
	} else {
            d_packet_format(pfx, DECODE_ETHER, pkt->_pkt_data.data,
                            pkt->pkt_len, NULL);
        }
    }
#endif /* __KERNEL__ */
#endif /* __STRICT_ANSI__ */
#endif /* NO_SAL_APPL */

}

STATIC void
pw_dump_packet(int unit, pup_t *pup, uint32 report)
{
    pwu_t       *pu = &pw_units[unit];

    switch (pu->mode) {
        case PW_MODE_RX:
            pw_dump_packet_rx(unit, pup, report);
            break;
        case PW_MODE_SOC:
            pw_dump_packet_soc(unit, pup, report);
            break;
        default:
            break;
    }
}

STATIC cmd_result_t
pw_dump_log(int unit, int n)
/*
 * Function:	pw_dump_log
 * Purpose:	Dump out currently logged packets
 * Parameters:  unit - unit #
 *		n - number of packets to dump, -n means last "n"
 *			packets.
 * Returns:	CMD_OK - success
 */
{
    pwu_t	*pu = &pw_units[unit];
    pup_t	*pup;
    int		abs_n;

    PW_LOCK(unit);			/* Stop updates */

    if ((pu->pu_log == NULL) || (n == 0)) { /* Nothing to show */
	PW_UNLOCK(unit);
	cli_out("pw_dump_log[%d]: Warning: no packets logged "
                "or selected to dump\n", unit);
	return CMD_OK;			/* Return nothing */
    }

    /*
     * Loop through list of packets, stop when we have dumped the correct
     * number - we know there is at least one logged since we checked above.
     * If dumping the last n packets, move backwards, otherwise move forwards.
     * Anything on this list is complete, so the PW_LOCK protects this 
     * operation.
     */
    abs_n = n < 0 ? -n : n;
    abs_n = abs_n > pu->pu_log_cnt ? pu->pu_log_cnt : abs_n;

    if (n < 0) {			/* Dump last N packets */
	for (pup = pu->pu_log; abs_n-- != 0; pup = pup->pup_next) {
	    pw_dump_packet(unit, pup, pu->pu_dump_options);
	}
    } else {				/* Dump first N packets */
	for (pup = pu->pu_log->pup_prev; abs_n-- != 0; pup = pup->pup_prev) {
	    pw_dump_packet(unit, pup, pu->pu_dump_options);
	}	    
    }
    PW_UNLOCK(unit);
	
    return CMD_OK;
}

STATIC	void
pw_log_packet(int unit, pup_t *pup)
/*
 * Function: 	pw_log_packet
 * Purpose:	Commit a recieved packet to the logs.
 * Parameters:	unit - unit #
 * 		pup - pointer to pup to add to log.
 * Returns:	Nothing.
 *
 * Notes:	Assumes PW_LOCK held.
 */
{
    pwu_t	*pu = &pw_units[unit];

    /*
     * There are 2 cases:
     * 1) First entry
     * 2) Log is full so the LRU packet is dropped (common case)
     * 3) Log is not full, no packet is dropped.
     */

    if (pu->pu_log_cnt == 0) {		/* 1) First entry */
	pu->pu_log = pup->pup_next = pup->pup_prev = pup;
	pu->pu_log_cnt = 1;
    } else if (pu->pu_log_cnt == pu->pu_log_max) { /* 2) Full log, drop one */
	pup_t	*tp;

	tp = pu->pu_log->pup_prev;	/* Entry to delete */
	pup->pup_next = pu->pu_log;	/* Link new one in at head */
	pup->pup_prev = tp->pup_prev;
	pup->pup_next->pup_prev = pup;
	pup->pup_prev->pup_next = pup;

	pw_pup_free(unit, tp);
    } else {				/* 3) Just add to chain */
	/*
	 * Case 2 - add new entry and increment log count.
	 */
	pu->pu_log_cnt++;
	pup->pup_next = pu->pu_log;
	pup->pup_prev = pu->pu_log->pup_prev;
	pup->pup_next->pup_prev = pup;
	pup->pup_prev->pup_next = pup;
    }
    pu->pu_log = pup;			/* Set new list */
}


STATIC	void
pw_process_pending(int unit)
/*
 * Function:	pw_process_pending
 * Purpose:	Process packets queued from interrupt
 * Parameters:	unit - unit #
 * Returns:	Nothing.
 *
 * Notes: 	Can be called only from a non-interrupt context.
 */
{
    pwu_t	*pu = &pw_units[unit];
    pup_t	*pup;
    int         chan;
    chan = 0;
    /*
     * Pick up current done list and process in order.
     */
    PW_SPIN_LOCK(unit);
    pup = (pup_t *)pu->pu_pending;
    pu->pu_pending = NULL;
    PW_SPIN_UNLOCK(unit);

    pu->last_pkt_count = 0;
    while (pup) {
        pup_t	*tp = pup->pup_next;

        pup->pup_seqno = ++pu->pu_packet_received;
        pw_log_packet(unit, (pup_t *)pup);
        if (pu->mode == PW_MODE_SOC) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
            pw_start_channel(unit, (dma_chan_t)pup->pup_esw_dv->dv_pw_chan);
            chan = (int)pup->pup_esw_dv->dv_pw_chan + 1;
#endif
        } else {
            chan = 0; /* Per-channel counted in Rx call-back */
        }
        pu->pu_ch_count[chan]++;
        pw_dump_packet(unit, (pup_t *)pup, pu->pu_report); /* Dump packet */
        pup = tp;
        pu->last_pkt_count++;
    }
}

/****************************************************************
 *
 * RX API packet watcher code, non-interrupt
 */
STATIC bcm_rx_t
pw_rx_callback(int unit, bcm_pkt_t *pkt, void *cookie)
{
    pup_t *pup;
    pwu_t	*pu = &pw_units[unit];
    int8        chan;
    dma_chan_t  dma_chan;

    chan = pkt->dma_channel;

    if (soc_feature(unit, soc_feature_cmicx)) {
        dma_chan = CMICX_N_DMA_CHAN;
    } else {
        dma_chan = N_DMA_CHAN;
    }

    if (chan >= 0 && chan < dma_chan) {
        pu->pu_ch_count[chan + 1]++;
    }
    if (pu->pu_report == 0 && pu->pu_log_max == 1) {
        /* No post-processing - just count packet */
        return BCM_RX_HANDLED;
    }

    pup = pw_pup_alloc(unit);
    if (!pup) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "PW: Failed to allocate pup struct.  Discarding\n")));
        return BCM_RX_NOT_HANDLED;
    }

    sal_memcpy(&pup->rx_pkt, pkt, sizeof(bcm_pkt_t));

    PW_SPIN_LOCK(unit);
    pup->pup_next = NULL;
    if (pu->pu_pending == NULL) {
        pup->pup_prev = pup;
        pu->pu_pending = pup;
    } else {
        pu->pu_pending->pup_prev->pup_next = pup;
        pu->pu_pending->pup_prev = pup;
    }
    PW_SPIN_UNLOCK(unit);

    sal_sem_give(pw_units[unit].pu_sema);
    return BCM_RX_HANDLED_OWNED;
}

/****************************************************************
 *
 * SOC layer packet watcher code
 */


STATIC	cmd_result_t
pw_start_channel(int unit, dma_chan_t chan)
/*
 * Function:	pw_start_chanel
 * Purpose:	Re-init dv and start a new DMA read operation.
 * Parameters:	unit - SOC unit #
 *		chan - DMA channel, if -1, uses default RX channel.
 * Returns:	CMD_OK/CMD_FAIL
 */
{
    pwu_t	*pu = &pw_units[unit];
    pup_t	*pup;
    int		rv = 0;
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
    dv_t	*esw_dv;
    pbmp_t	empty;
#endif

    pup = pw_pup_alloc(unit);

    assert(pu->pu_ch_active[chan + 1] == NULL);
    assert(pup);

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
    SOC_PBMP_CLEAR(empty);
#endif
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
    esw_dv = pup->pup_esw_dv;
    soc_dma_dv_reset(DV_RX, esw_dv);
    soc_dma_desc_add(esw_dv, (sal_vaddr_t)pup->pup_pkt, PW_PACKET_SIZE, 
		     PBMP_ALL(unit), empty, empty, 0, NULL);
    soc_dma_desc_end_packet(esw_dv);
    esw_dv->dv_done_chain = pw_esw_done_intr;
    esw_dv->dv_flags |= DV_F_NOTIFY_CHN;

    esw_dv->dv_pw_chan = (uint32)chan;
    esw_dv->dv_pw_pup  = pup;

    pu->pu_ch_active[chan + 1] = pup;

    rv = soc_dma_start(unit, chan, esw_dv);
    if (rv < 0) {
	pu->pu_ch_active[chan + 1] = NULL;
	pw_pup_free(unit, pup);
	cli_out("Failed to start DMA: %s\n", soc_errmsg(rv));
    }
#endif


    return(rv < 0 ? CMD_FAIL : CMD_OK);
}

STATIC  void
pw_cleanup_dma(int unit)
/*
 * Function:    pw_cleanup_dma
 * Purpose:     Clean up outstanding DMA operations.
 * Parameters:  unit - unit #
 * Returns:     Nothing.
 *
 * Notes: Cleans up active DMA operations, and DMA operations that have
 *        completed but have not been processed yet.
 */
{
    pwu_t       *pu = &pw_units[unit];
    dma_chan_t  chan, dma_chan;

    if (soc_feature(unit, soc_feature_cmicx)) {
        dma_chan = CMICX_N_DMA_CHAN;
    } else {
        dma_chan = N_DMA_CHAN;
    }
    for (chan = 0; chan < dma_chan + 1; chan++) {

        /* Clean up Active DMA */

        if (pu->pu_ch_active[chan]) {
#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT)
	    if (SOC_IS_ESW(unit) || SOC_IS_ARAD(unit)) {
                (void)soc_dma_abort_dv(unit, pu->pu_ch_active[chan]->pup_esw_dv);
            }
#endif
#if defined(BCM_DNX_SUPPORT)
            if (SOC_IS_DNX(unit)) {
                (void)soc_dma_abort_dv(unit, pu->pu_ch_active[chan]->pup_esw_dv);
            }
#endif
            pw_pup_free(unit, (pup_t *)pu->pu_ch_active[chan]);
            pu->pu_ch_active[chan] = NULL;
        }

    }
    
    /* Clean up those that are complete but not processed */

    PW_SPIN_LOCK(unit);
    while (pu->pu_pending != NULL) {
        pup_t   *tp = (pup_t *)pu->pu_pending;
        pu->pu_pending = tp->pup_next;
        pw_pup_free(unit, tp);
    }
    PW_SPIN_UNLOCK(unit);
}

#if defined(BCM_ESW_SUPPORT) || defined(BCM_PETRA_SUPPORT) || defined(BCM_DNX_SUPPORT)
/*
 * Function:	pw_esw_done_intr
 * Purpose:	Process a chain done interrupt.
 * Parameters:	unit - unit #
 *		dv - pointer to dv_chain associated with request.
 * Returns:	Nothing.
 * 
 * Notes:	All this routine does is wake up the packet watcher
 *		thread. 
 */
STATIC	void
pw_esw_done_intr(int unit, dv_t *dv)
{
    pwu_t	*pu  = &pw_units[unit];
    pup_t	*pup = (pup_t *)dv->dv_pw_pup;

    pu->pu_ch_active[(int)dv->dv_pw_chan + 1] = NULL;
    pup->pup_next = NULL;
    PW_SPIN_LOCK(unit);
    if (pu->pu_pending == NULL) {
        pup->pup_prev = pup;
        pu->pu_pending = pup;
    } else {
        pu->pu_pending->pup_prev->pup_next = pup;
        pu->pu_pending->pup_prev = pup;
    }
    PW_SPIN_UNLOCK(unit);

    sal_sem_give(pw_units[unit].pu_sema);
}
#endif /* BCM_ESW_SUPPORT */
