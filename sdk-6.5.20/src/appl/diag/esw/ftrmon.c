/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * FlowTracker Remote record monitor
 */

#include <sal/types.h>
#include <shared/bsl.h>
#ifndef __KERNEL__
#include <sys/types.h>
#include <netinet/in.h>
#endif

#include <soc/debug.h>
#include <soc/cm.h>

#include <sal/appl/io.h>
#include <sal/appl/sal.h>
#include <sal/core/thread.h>
#include <sal/core/sync.h>
#include <sal/core/spl.h>
#include <sal/core/libc.h>

#include <appl/diag/shell.h>
#include <appl/diag/parse.h>
#include <appl/diag/system.h>
#include <appl/diag/decode.h>

#include <bcm/rx.h>
#include <bcm/error.h>
#include <bcm/flowtracker.h>

#include <soc/shared/shr_pkt.h>

#if defined(BCM_FLOWTRACKER_SUPPORT)

#ifdef GHS
#define d_packet_format(pfx, decode, pkt, count, p)
#endif

/* Maximum name length of export elements. */
#define BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX       40

/*
 * Maximum packet size we will read in.
 */
#define PW_PACKET_SIZE    10240

#define PW_COUNT_DEFAULT  100


static char ft_rmon_name[SOC_MAX_NUM_DEVICES][16];

/*
 * Define 2 of the public fields in the dv structure for our use.
 */
#define  dv_ft_rmon_pup   dv_public1.ptr
#define  dv_ft_rmon_chan  dv_public2.u32


/*
 * Typedef:  pup_t
 * Purpose:  Maps out each packet array entry - pkt_cnt is sort of
 *           useless, but it makes life easier so we use it to track
 *           the pack # received.
 *
 * Notes:    Each entry is either FREE or on a circular/double linked
 *           list used for the LOG. When the LOG is full, there are 3 extra
 *           pups used to suppor the up to 3 outstanding DMA operations.
 */
typedef struct pup_s {
    struct pup_s *pup_next, *pup_prev; /* Next and previous pointers */
    dv_t         *pup_esw_dv;   /* DMA descriptor, ESW SOC mode only */
    int           pup_seqno;    /* Packet # */
    void         *pup_pkt;      /* Actual packet pointer, for soc */
    bcm_pkt_t    rx_pkt;        /* Packet data for RX */
} pup_t;

#define REPORT_COUNT    0x01    /* Report packet count */
#define REPORT_RAW      0x02    /* Report Raw packet data */
#define REPORT_DECODE   0x04    /* Report Decode */
#define REPORT_DMA      0x08    /* Report RX Dma */
#define REPORT_CHANNEL  0x10    /* Report RX channel */

typedef struct pwu_s {
    volatile uint32 pu_flags;       /* Flags this unit PU_F_XXX */
#   define      PU_F_RUN    0x01
#   define      PU_F_STOP   0x02
#   define      PU_F_SYNC   0x04    /* Wake up request thread */
    sal_mutex_t pu_lock;            /* Syncronize updates */
    sal_spinlock_t  pu_spinlock;    /* Protect updates */
    int32   pu_channel;             /* RX channel list 1 << ch # */
#   define  PU_CHANNEL(_c)  (1 << (_c))
    uint32  pu_report;      /* Reporting level */
    uint32  pu_dump_options;    /* Reporting level when dumping logs */
    COMPILER_DOUBLE pu_count_last;  /* Last count displayed */
    COMPILER_DOUBLE pu_count_time;  /* Last time count was displayed */
    sal_thread_t pu_pid;    /* pid this unit */
    sal_sem_t   pu_sema;        /* Sleep semaphore this unit */
    sal_sem_t   pu_sync;        /* Sync command semaphore */

    /*
     * pu_pending - a single linked list, where the head's prev pointer
     *                 points to the tail.
     * pu_ch_active  - pointers to DMA active on the given channel.
     */

    VOL pup_t   *pu_pending;    /* Pending interrupt servicing */
    VOL pup_t   *pu_ch_active[MAX_N_DMA_CHAN+1]; /* Active DMA operations */
    int     pu_ch_count[MAX_N_DMA_CHAN+1]; /* # received on channel */
    pup_t   *pu_log;        /* Last N packets - head is LRU */
    pup_t   *pu_log_free;       /* Last N packets - head is LRU */
    pup_t   *pup_list;      /* List of all PUPs for free */
    int     pu_log_cnt;     /* # packets on LOG list */
    int     pu_log_max;     /* Max # packets logged */
#   define  PU_LOG_CNT  64  /* Default # log entries */
    uint32  pu_packet_received; /* total # received packets */
    int     pu_interval;        /* usec between pkt rcv & next dma */

    int         mode;                   /* What mode are we in */
#define         PW_MODE_RX              0
#define         PW_MODE_PMUX            2
#define PW_INIT_STRINGS \
    "RX", \
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

char *ft_rmon_modes[2] = {PW_INIT_STRINGS};

STATIC bcm_rx_t         ft_rmon_rx_callback(int unit, bcm_pkt_t *pkt,
                                       void *cookie);

STATIC pup_t *          ft_rmon_pup_alloc(int unit);
STATIC void             ft_rmon_pup_free(int unit, pup_t *pup);
STATIC void             ft_rmon_dump_packet(int unit, pup_t *pup, uint32 report);
STATIC int              ft_rmon_channel_config(int unit);
STATIC void             ft_rmon_thread(void *up);
STATIC void             ft_rmon_process_pending(int unit);
STATIC cmd_result_t     ft_rmon_dump_log(int unit, int n);

pwu_t   ft_rmon_units[SOC_MAX_NUM_DEVICES];

STATIC void
ft_rmon_init(int unit)
{
    pwu_t   *pu = &ft_rmon_units[unit]; /* Control this unit */

    if (pu->init_done) {
        return;
    }

    sal_snprintf(ft_rmon_name[unit], sizeof(ft_rmon_name[unit]), "bcmFtRMon.%d", unit);

    /*
     * sync and lock semaphores are only created once, the others
     * are created and destroyed every time the packet watcher is
     * started and stopped.
     *
     * We need to check if they already exist to support the
     * pw reset option.
     */
    if (!pu->pu_sync) {
        pu->pu_sync = sal_sem_create("ft_rmon_sync", sal_sem_BINARY, 0);
    }
    if (!pu->pu_lock) {
        pu->pu_lock = sal_mutex_create("ft_rmon_lock");
    }
    if (!pu->pu_spinlock) {
        pu->pu_spinlock = sal_spinlock_create("ft_rmon_spinlock");
    }

    if (!pu->pu_sync || !pu->pu_lock || !pu->pu_spinlock) {
        cli_out("%s ERROR:  Could not allocate sync/lock\n", ft_rmon_name[unit]);
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
_ft_rmon_set_rate(int unit)
{
    pwu_t   *pu = &ft_rmon_units[unit]; /* Control this unit */
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
        default:
            break;
    }

    return rv;
}

#define PW_LOCK(_u) sal_mutex_take(ft_rmon_units[_u].pu_lock, \
                       sal_mutex_FOREVER)
#define PW_UNLOCK(_u)   sal_mutex_give(ft_rmon_units[_u].pu_lock)
#define PW_SPIN_LOCK(_u)    sal_spinlock_lock(ft_rmon_units[_u].pu_spinlock)
#define PW_SPIN_UNLOCK(_u)  sal_spinlock_unlock(ft_rmon_units[_u].pu_spinlock)

int
ft_rmon_start(int unit, int sync)
/*
 * Function:    ft_rmon_start
 * Purpose: Start the ipfix packet monitor
 * Parameters:  unit - unit to start.
 *      sync - if true, wait for mon to start.
 * Returns: 0 - success, -1 failed.
 */
{
    pwu_t   *pu = &ft_rmon_units[unit]; /* Control this unit */

    ft_rmon_init(unit);
    if (pu->pu_sync == NULL || pu->pu_lock == NULL || pu->pu_spinlock == NULL) {
        return -1;
    }

    sal_memset(pu->pu_ch_active, 0, sizeof(pu->pu_ch_active));
    sal_memset(pu->pu_ch_count, 0, sizeof(pu->pu_ch_count));

    pu->pu_pending = NULL;
    pu->pu_log = NULL;
    pu->pu_log_free = NULL;
    pu->pu_log_cnt = 0;

        if (ft_rmon_channel_config(unit) < 0) {
            return -1;
        }

    pu->pu_sema = sal_sem_create("ft_rmon_thread", sal_sem_BINARY, 0);
    if (pu->pu_sema == 0) {
        return -1;
    }

    pu->pu_flags |= PU_F_RUN;
    if (sync) {
        pu->pu_flags |= PU_F_SYNC;
    }

    pu->pu_pid =
        sal_thread_create(ft_rmon_name[unit], SAL_THREAD_STKSZ,
                          soc_property_get(unit, spn_DIAG_PW_THREAD_PRI, 100),
              ft_rmon_thread, INT_TO_PTR(unit));

    if (pu->pu_pid == SAL_THREAD_ERROR) {
        cli_out("%s: Unable to start task\n", ft_rmon_name[unit]);
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
            cli_out("%s: Failed to wait for start\n", ft_rmon_name[unit]);
            return -1;
        }
    }

    return 0;
}

cmd_result_t
ft_rmon_stop(int unit, int sync)
/*
 * Function:    ft_rmon_stop
 * Purpose: Stop the ipfix packet monitor
 * Parameters:  unit - unit to stop.
 *      sync - if true, wait for rmon to finish.
 * Returns: 0 - success, -1 failed.
 */
{
    pwu_t   *pu = &ft_rmon_units[unit]; /* Control this unit */
    soc_timeout_t  to;
    sal_usecs_t     ft_rmon_timeout;

    ft_rmon_timeout = soc_property_get(unit, spn_CDMA_TIMEOUT_USEC, 10000000);

    pu->pu_flags |= PU_F_STOP;
    if (sync) {
    pu->pu_flags |= PU_F_SYNC;
    }
    sal_sem_give(pu->pu_sema);
    soc_timeout_init(&to, ft_rmon_timeout, 0);

    /* Check for ft_rmon_thread exit */
    while (pu->pu_pid != SAL_THREAD_ERROR) {
         /* Handle the case of ft_rmon_thread not exiting */
         if (soc_timeout_check(&to)) {
             cli_out("ft_rmon_stop: ft_rmon_thread did not exit\n");
             pu->pu_pid = SAL_THREAD_ERROR;
             break;
         }

         sal_usleep(80000);
    }

    if  (pu->pu_pid == SAL_THREAD_ERROR) {
        /* Abnormal thread exit: Free semaphore resource */
    /*    coverity[check_after_deref]    */
        if (pu->pu_sema) {
            /* Must check, because may have been destroyed by ft_rmon_exit */
            sal_sem_destroy(pu->pu_sema);
            pu->pu_sema = 0;
        }
    }

    if (sync) {
        (void)sal_sem_take(pu->pu_sync, sal_sem_FOREVER);
    } else {
        cli_out("%s: Termination requested...\n", ft_rmon_name[unit]);
    }
    return CMD_OK;
}

int
ft_rmon_running(int unit)
/*
 * Function:    ft_rmon_running
 * Purpose: Determine if ipfix rmon is running for the specified
 *      unit.
 * Parameters:  unit - unit #
 * Returns: 0 - not running, !0 - running
 */
{
    return ft_rmon_units[unit].pu_flags & PU_F_RUN;
}

STATIC  void
ft_rmon_exit(int unit, int stat)
/*
 * Function:    ft_rmon_exit
 * Purpose: Exit a ftrmon thread.
 * Parameters:  unit - unit #.
 *      stat - exit status (passed to exit).
 * Returns: Does not return.
 *
 * Notes:   ASSUMES LOCK held on entry, released in this routine
 */
{
    pwu_t *pu = &ft_rmon_units[unit];
    int rv;
    char thread_name[SAL_THREAD_NAME_MAX_LEN];
    sal_thread_t    thread;

    thread = sal_thread_self();
    thread_name[0] = 0;
    sal_thread_name(thread, thread_name, sizeof (thread_name));

    switch (pu->mode) {
        case PW_MODE_RX:
            /* ft_rmon_exit() is called holding the lock. Give it up
             * momentarily while we stop the RX task.
             */
            PW_UNLOCK(unit);
            if ((rv = bcm_rx_stop(unit, NULL)) < 0) {
                cli_out("FtRemoteMon stop error: Cannot stop RX: %s.\n",
                        bcm_errmsg(rv));
            }
            if ((rv = bcm_rx_queue_unregister(unit, BCM_RX_COS_ALL,
                                              ft_rmon_rx_callback, pu->rx_pri))
                       < 0) {
                cli_out("FtRemoteMon stop error: Cannot unregister handler: %s.\n",
                        bcm_errmsg(rv));
            }
            PW_LOCK(unit);

            break;
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
        pup_t   *pup = pu->pu_log->pup_next;
        ft_rmon_pup_free(unit, pu->pu_log);
        pu->pu_log = pup;
    }
    }

    /* Now free the free list */

    while (pu->pu_log_free) {
    pup_t   *pup = pu->pu_log_free;
    pu->pu_log_free = pup->pup_next;
    }
    if (pu->pup_list) {
        sal_free(pu->pup_list);
    }

    if (pu->pu_flags & PU_F_SYNC) {
    sal_sem_give(pu->pu_sync);
    } else {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "FtRemoteMon-Daemon[%d]: Exiting\n"), unit));
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

/* Start the ipfix packet rmon operation depending on the mode */
STATIC int
_ft_rmon_start_op(int unit)
{
    pwu_t       *pu = &ft_rmon_units[unit];
    int pkt_size;
    int rv = BCM_E_NONE;

    switch (pu->mode) {
    case PW_MODE_RX:
        pu->rx_cfg.global_pps = pu->rate;

        /*
         * Allow the ipfix rmon to run in truncating mode
         * by allocating smaller Rx buffers and accepting
         * oversized packets on all Rx DMA channels.
         */
        pkt_size = soc_property_get(unit, spn_DIAG_PW_BUFFER_SIZE, -1);

        if (pkt_size >= 68) {
            int chan;
            dma_chan_t  dma_chan;
            pu->rx_cfg.pkt_size = pkt_size;

            if (soc_feature(unit, soc_feature_cmicx)) {
                dma_chan = BCM_CMICX_RX_CHANNELS;
            } else {
                dma_chan = BCM_RX_CHANNELS;
            }

            for (chan = 0; chan < dma_chan; chan++) {
                pu->rx_cfg.chan_cfg[chan].flags |= BCM_RX_F_OVERSIZED_OK;
            }
        }

        if ((rv = bcm_rx_start(unit, &pu->rx_cfg)) < 0) {
            return rv;
        }

        if ((rv = bcm_rx_queue_register(unit, "RX CMD", BCM_RX_COS_ALL,
                      ft_rmon_rx_callback, pu->rx_pri, NULL, pu->rx_flags)) < 0) {
            return rv;
        }
        break;
    default:
        assert("Unknown pkt watcher mode");
        break;
    }

    return rv;
}

STATIC void
ft_rmon_thread(void *up)
/*
 * Function:    ft_rmon_thread
 * Purpose:     Waits for packets to arrive destined for the CPU and
 *              saves them in a queue.
 * Parameters:  unit - SOC unit #
 * Returns:     Does not return.
 */
{
    int         unit = PTR_TO_INT(up);
    int         i;
    pwu_t       *pu = &ft_rmon_units[unit];
    int         rv;
    int         pup_count;
    pup_t      *pup;

    PW_LOCK(unit);

    /* Allocate packet pointer array */

    if (0 == pu->pu_log_max) {
        pu->pu_log_max = PW_COUNT_DEFAULT;
    }

    pup_count = pu->pu_log_max;

    /* Bump up pup_count to be twice the log size so pups will
     * be available even when log queue is full.
     */
    pup_count += pu->pu_log_max;

    pu->pup_list = sal_alloc(sizeof(pup_t) * pup_count, "FtRemoteMon-pup");
    if (pu->pup_list == NULL) {
        ft_rmon_exit(unit, -1);
    }
    /* No forwaqrding null. If the allocation failed, the call to ft_rmon_exit will end the process and we sould never reach here. */
    /* coverity[var_deref_model:FALSE] */
    sal_memset(pu->pup_list, 0, sizeof(pup_t) * pup_count);

    for (i = 0; i < pup_count; i++) {

        pup = &pu->pup_list[i];

        ft_rmon_pup_free(unit, pup);         /* Put on free list */
    }

    if (pu->pu_flags & PU_F_SYNC) {
        pu->pu_flags &= ~PU_F_SYNC;
        sal_sem_give(pu->pu_sync);
    } else {
        cli_out("FtRemoteMon-daemon[%d] -- Started\n", unit);
    }

    /* Kick Start channels */
    if ((rv = _ft_rmon_start_op(unit)) < 0) {
        cli_out("FtRemoteMon rx mode: Cannot start %s: %s.\n", ft_rmon_modes[pu->mode],
                bcm_errmsg(rv));
        ft_rmon_exit(unit, -1);
    }

    PW_UNLOCK(unit);

    /*
     * Check if there are any actions for us.
     */
    while (1) {
        int     sem_rv;

        sem_rv = sal_sem_take(ft_rmon_units[unit].pu_sema, sal_sem_FOREVER);

        if (sem_rv < 0) {
            cli_out("Failed sem_take, exiting\n");
            ft_rmon_exit(unit, -1);
        }

        PW_LOCK(unit);                  /* Block log dumps */

        if (pu->pu_flags & PU_F_STOP) { /* Time to exit */
            ft_rmon_exit(unit, 0);
        }

        ft_rmon_process_pending(unit);

        PW_UNLOCK(unit);
    }

    ft_rmon_exit(unit, 0);
}

STATIC int
ft_rmon_channel_config(int unit)
/*
 * Function:    ft_rmon_channel_config
 * Purpose: Configure DMA RX channels for PW.
 * Parameters:  unit - unit to configure.
 * Returns: 0 - success
 *      -1 - failed.
 */
{
    pwu_t   *pu = &ft_rmon_units[unit]; /* Control this unit */
    dma_chan_t  tx, i;
    int     rv = SOC_E_NONE;
    dma_chan_t  dma_chan;

    if (pu->pu_channel == 0) {      /* Use default */
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
    if (pu->pu_channel & (1 << i)) {
        rv = soc_dma_chan_config(unit, i, DV_RX, SOC_DMA_F_INTR);
        if (rv < 0) {
        cli_out("%s: DMA channel configuration failed: %s\n",
                        ft_rmon_name[unit], soc_errmsg(rv));
        }
    } else if (tx == -1) {
        tx = i;
    }
    }

    if ((tx == -1) ||
    ((rv = soc_dma_chan_config(unit, tx, DV_TX,
                   SOC_DMA_F_INTR |SOC_DMA_F_DEFAULT)) < 0)) {
    cli_out("%s: Unable to configure TX DMA channel: %s\n",
                ft_rmon_name[unit],
                tx == -1 ? "No remaining channels" : soc_errmsg(rv));

    rv = soc_dma_init(unit);
    cli_out("%s: Unable to re-initialize DMA: %s\n",
                ft_rmon_name[unit],
                soc_errmsg(rv));

    return -1;
    }
    return 0;
}

static const parse_pm_t ft_rmon_report_table[] = {
    {"Count",   REPORT_COUNT},
    {"DECode",  REPORT_DECODE},
    {"Raw",     REPORT_RAW},
    {"DMA",     REPORT_DMA},
    {"CHannel", REPORT_CHANNEL},
    {"@ALL",    ~0},
    {"@*",  ~0},
    {NULL,  0}
};

static const parse_pm_t ft_rmon_channel_table[] = {
    {"0",   1 << 0},
    {"1",   1 << 1},
    {"2",   1 << 2},
    {"3",   1 << 3},
    {"4",   1 << 4},
    {"5",   1 << 5},
    {"6",   1 << 6},
    {"7",   1 << 7},
    {NULL,  0}
};

/****************************************************************
 *
 * Common ipfix rmon routines
 */


STATIC  pup_t *
ft_rmon_pup_alloc(int unit)
/*
 * Function:    ft_rmon_alloc
 * Purpose:     Alloc a pup off the free list.
 * Parameters:  unit - unit #
 * Returns:     Pointer to pup.
 */
{
    pwu_t       *pu = &ft_rmon_units[unit];
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
ft_rmon_pup_free(int unit, pup_t *pup)
/*
 * Function:    ft_rmon_pup_free
 * Purpose:     Put a pup on the free list.
 * Parameters:  unit - unit #
 *              pup - pointer to pup to free.
 * Returns:     Nothing.
 */
{
    pwu_t       *pu = &ft_rmon_units[unit];

    pup->pup_next = pu->pu_log_free;
    pu->pu_log_free = pup;

    if (pu->mode == PW_MODE_RX) {
        bcm_rx_free(unit, pup->rx_pkt.alloc_ptr);
    }
}

STATIC void
ft_rmon_dump_start(int unit, char *pfx, pup_t *pup, uint32 report,
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


extern int
bcma_ft_setid_template_id_get(int unit, uint16 setid,
                              bcm_flowtracker_export_template_t *template_id);

#define BCMI_FT_XLATE_ELEM(_name, _size) \
        bcmFlowtrackerExportElementType##_name, \
        _size, \
        #_name


typedef struct bcma_ft_export_elements_map_s {
    bcm_flowtracker_export_element_type_t export_element;
    int default_data_size;
    char element_name[BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX];
} bcma_ft_export_elements_map_t;


static bcma_ft_export_elements_map_t bcma_ft_export_elements_map[] = {
            {   BCMI_FT_XLATE_ELEM(SrcIPv4,            4  )   },
            {   BCMI_FT_XLATE_ELEM(DstIPv4,            4  )   },
            {   BCMI_FT_XLATE_ELEM(L4SrcPort,          2  )   },
            {   BCMI_FT_XLATE_ELEM(L4DstPort,          2  )   },
            {   BCMI_FT_XLATE_ELEM(SrcIPv6,            16 )   },
            {   BCMI_FT_XLATE_ELEM(DstIPv6,            16 )   },
            {   BCMI_FT_XLATE_ELEM(IPProtocol,         1  )   },
            {   BCMI_FT_XLATE_ELEM(PktCount,           4  )   },
            {   BCMI_FT_XLATE_ELEM(ByteCount,          4  )   },
            {   BCMI_FT_XLATE_ELEM(VRF,                2  )   },
            {   BCMI_FT_XLATE_ELEM(TTL,                1  )   },
            {   BCMI_FT_XLATE_ELEM(IPLength,           2  )   },
            {   BCMI_FT_XLATE_ELEM(IP6Length,          2  )   },
            {   BCMI_FT_XLATE_ELEM(TcpWindowSize,      2  )   },
            {   BCMI_FT_XLATE_ELEM(DosAttack,          4  )   },
            {   BCMI_FT_XLATE_ELEM(VxlanNetworkId,     3  )   },
            {   BCMI_FT_XLATE_ELEM(NextHeader,         1  )   },
            {   BCMI_FT_XLATE_ELEM(HopLimit,           1  )   },
            {   BCMI_FT_XLATE_ELEM(InnerSrcIPv4,       4  )   },
            {   BCMI_FT_XLATE_ELEM(InnerDstIPv4,       4  )   },
            {   BCMI_FT_XLATE_ELEM(InnerL4SrcPort,     2  )   },
            {   BCMI_FT_XLATE_ELEM(InnerL4DstPort,     2  )   },
            {   BCMI_FT_XLATE_ELEM(InnerSrcIPv6,       16 )   },
            {   BCMI_FT_XLATE_ELEM(InnerDstIPv6,       16 )   },
            {   BCMI_FT_XLATE_ELEM(InnerIPProtocol,    1  )   },
            {   BCMI_FT_XLATE_ELEM(InnerTTL,           1  )   },
            {   BCMI_FT_XLATE_ELEM(InnerIPLength,      2  )   },
            {   BCMI_FT_XLATE_ELEM(InnerNextHeader,    1  )   },
            {   BCMI_FT_XLATE_ELEM(InnerHopLimit,      1  )   },
            {   BCMI_FT_XLATE_ELEM(InnerIP6Length,     2  )   },
            {   BCMI_FT_XLATE_ELEM(TcpFlags,           1  )   },
            {   BCMI_FT_XLATE_ELEM(OuterVlanTag,       2  )   },
            {   BCMI_FT_XLATE_ELEM(FlowtrackerGroup,   2  )   },
            {   BCMI_FT_XLATE_ELEM(ExportReasons,      2  )   },
            {   BCMI_FT_XLATE_ELEM(ExportFlags,        1  )   },
            {   BCMI_FT_XLATE_ELEM(Reserved,           1  )   },
            {   BCMI_FT_XLATE_ELEM(TimestampNewLearn,     6  )   },
            {   BCMI_FT_XLATE_ELEM(TimestampFlowStart,    6  )   },
            {   BCMI_FT_XLATE_ELEM(TimestampFlowEnd,      6  )   },
            {   BCMI_FT_XLATE_ELEM(TimestampCheckEvent1, 6  )   },
            {   BCMI_FT_XLATE_ELEM(TimestampCheckEvent2, 6  )   },
            {   BCMI_FT_XLATE_ELEM(InnerDosAttack,        4  )   },
            {   BCMI_FT_XLATE_ELEM(TunnelClass,           3  )   },
            {   BCMI_FT_XLATE_ELEM(FlowtrackerCheck,      4  )   },
            {   BCMI_FT_XLATE_ELEM(ChipDelay, 4  )   },
            {   BCMI_FT_XLATE_ELEM(E2EDelay, 4  )   },
            {   BCMI_FT_XLATE_ELEM(IPATDelay , 4  )   },
            {   BCMI_FT_XLATE_ELEM(IPDTDelay, 4  )   },
            {   BCMI_FT_XLATE_ELEM(IngPortPGMinCongestionLevel, 1  )   },
            {   BCMI_FT_XLATE_ELEM(IngPortPGSharedCongestionLevel, 1  )   },
            {   BCMI_FT_XLATE_ELEM(IngPortPoolMinCongestionLevel, 1  )   },
            {   BCMI_FT_XLATE_ELEM(IngPortPoolSharedCongestionLevel , 1  )   },
            {   BCMI_FT_XLATE_ELEM(IngPoolSharedCongestionLevel, 1  )   },
            {   BCMI_FT_XLATE_ELEM(IngPortPGHeadroomCongestionLevel, 1  )   },
            {   BCMI_FT_XLATE_ELEM(IngPortPoolHeadroomCongestionLevel, 1  )   },
            {   BCMI_FT_XLATE_ELEM(IngGlobalHeadroomCongestionLevel, 1  )   },
            {   BCMI_FT_XLATE_ELEM(EgrPoolSharedCongestionLevel, 1  )   },
            {   BCMI_FT_XLATE_ELEM(EgrQueueMinCongestionLevel, 1  )   },
            {   BCMI_FT_XLATE_ELEM(EgrQueueSharedCongestionLevel, 1  )   },
            {   BCMI_FT_XLATE_ELEM(EgrQueueGroupMinCongestionLevel, 1  )   },
            {   BCMI_FT_XLATE_ELEM(EgrQueueGroupSharedCongestionLevel, 1  )   },
            {   BCMI_FT_XLATE_ELEM(EgrPortPoolSharedCongestionLevel, 1  )   },
            {   BCMI_FT_XLATE_ELEM(EgrRQEPoolSharedCongestionLevel, 1  )   },
            {   BCMI_FT_XLATE_ELEM(CFAPCongestionLevel, 1  )   },
            {   BCMI_FT_XLATE_ELEM(MMUCongestionLevel, 1  )   },
            {   BCMI_FT_XLATE_ELEM(MMUQueueId, 2  )   },
            {   BCMI_FT_XLATE_ELEM(ECMPGroupIdLevel1, 2  )   },
            {   BCMI_FT_XLATE_ELEM(ECMPGroupIdLevel2, 2  )   },
            {   BCMI_FT_XLATE_ELEM(TrunkMemberId, 1  )   },
            {   BCMI_FT_XLATE_ELEM(TrunkGroupId, 1  )   },
            {   BCMI_FT_XLATE_ELEM(IngPort, 1  )   },
            {   BCMI_FT_XLATE_ELEM(IngDropReasonGroupIdVector, 2  )   },
            {   BCMI_FT_XLATE_ELEM(NextHopB, 2  )   },
            {   BCMI_FT_XLATE_ELEM(NextHopA, 2  )   },
            {   BCMI_FT_XLATE_ELEM(IPPayloadLength, 2  )   },
            {   BCMI_FT_XLATE_ELEM(QosAttr, 2  )   },
            {   BCMI_FT_XLATE_ELEM(EgrPort, 2  )   },
            {   BCMI_FT_XLATE_ELEM(MMUCos, 2  )   },
            {   BCMI_FT_XLATE_ELEM(EgrDropReasonGroupIdVector, 2  )   },
            {   BCMI_FT_XLATE_ELEM(TimestampCheckEvent3 , 6  )   },
            {   BCMI_FT_XLATE_ELEM(TimestampCheckEvent4, 6  )   },
            {   BCMI_FT_XLATE_ELEM(FlowIdentifier, 4 )  },
            {   BCMI_FT_XLATE_ELEM(MMUQueueGroupId, 2 )  },
            {   BCMI_FT_XLATE_ELEM(MMUPacketTos , 1 )  },
            {   BCMI_FT_XLATE_ELEM(EgrPacketTos, 1 )  },
            {   BCMI_FT_XLATE_ELEM(IngPacketTos, 1 )  },
            {   BCMI_FT_XLATE_ELEM(L2ForwardingField, 2 )  },
            {   BCMI_FT_XLATE_ELEM(InnerTcpFlags, 1 )  },
            {   BCMI_FT_XLATE_ELEM(GbpSrcId, 2 ) },
            {   BCMI_FT_XLATE_ELEM(TimestampIngress, 6 ) },
            {   BCMI_FT_XLATE_ELEM(TimestampEgress, 6 ) },
            {   BCMI_FT_XLATE_ELEM(TcpSeqNumber, 4 ) },
            {   BCMI_FT_XLATE_ELEM(Custom, 0 ) }
};

STATIC int
bcma_ft_export_elem_name_get(int unit,
                             bcm_flowtracker_export_element_type_t info_elem,
                             int *data_size,
                             char *elem_name)
{
    int ix;

    for (ix = 0; ix < COUNTOF(bcma_ft_export_elements_map); ix++) {
        if (bcma_ft_export_elements_map[ix].export_element == info_elem) {
            if (elem_name) {
                sal_strncpy(elem_name, bcma_ft_export_elements_map[ix].element_name,
                    BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX);
            }
            if (data_size) {
                *data_size = bcma_ft_export_elements_map[ix].default_data_size;
            }
            break;
        }
    }

    if (ix == COUNTOF(bcma_ft_export_elements_map)) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

void
format_ts(char buf[20], uint8 *ts, int data_size)
{
    if (data_size == 8) {
        sal_sprintf(buf, "%02x%02x%02x%02x%02x%02x%02x%02x",
                (uint8)ts[0], (uint8)ts[1], (uint8)ts[2],
                (uint8)ts[3], (uint8)ts[4], (uint8)ts[5],
                (uint8)ts[6], (uint8)ts[7]);
    } else {
        sal_sprintf(buf, "%02x%02x%02x%02x%02x%02x",
                (uint8)ts[0], (uint8)ts[1], (uint8)ts[2],
                (uint8)ts[3], (uint8)ts[4], (uint8)ts[5]);
    }
}


STATIC int
bcma_ipfix_data_record_print(int unit,
                             int set_length,
                             uint8 *data_set,
                             int num_elems,
                             bcm_flowtracker_export_element_info_t *list_elems)
{
    int ix, ex;
    char elem_name[BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX];
    uint8 u8;
    uint16 u16;
    uint32 u32;
    uint8 *data, *data_tmp;
    int data_size;
    bcm_ip6_t ip6;
    char ip_str[IP6ADDR_STR_LEN];
    bcm_flowtracker_export_element_info_t *elem;
    uint8 ts[8];
    char ts_str[20];


    data = data_set;

    if (0) {
        int ix;
        for (ix = 0; ix < (set_length - 4); ix+=8) {
            cli_out("%08x   %02x %02x %02x %02x %02x %02x %02x %02x\n",
                    ix, data[ix], data[ix+1], data[ix+2], data[ix+3],
                    data[ix+4], data[ix+5], data[ix+6], data[ix+7]);
        }
    }


    for (ex = 0; ex <  num_elems; ex++) {
        elem = &(list_elems[ex]);
        data_tmp = data;

        if (BCM_FAILURE(bcma_ft_export_elem_name_get(unit, elem->element, &data_size, elem_name))) {
            cli_out("Element name not found: elem = %d\n", elem->element);
            return -1;
        }
        data_size = elem->data_size;

        switch (elem->element) {
            case bcmFlowtrackerExportElementTypeSrcIPv4:
            case bcmFlowtrackerExportElementTypeDstIPv4:
            case bcmFlowtrackerExportElementTypeInnerSrcIPv4:
            case bcmFlowtrackerExportElementTypeInnerDstIPv4:
                SHR_PKT_UNPACK_U32(data_tmp, u32);
                format_ipaddr(ip_str, u32);
                cli_out("Info Element:: %02d %20s | Size: %02d | Value: (%s)\n", elem->element, elem_name, elem->data_size, ip_str);
                break;
            case bcmFlowtrackerExportElementTypeSrcIPv6:
            case bcmFlowtrackerExportElementTypeDstIPv6:
            case bcmFlowtrackerExportElementTypeInnerSrcIPv6:
            case bcmFlowtrackerExportElementTypeInnerDstIPv6:
                for (ix = 0; ix < 16; ix++) {
                    SHR_PKT_UNPACK_U8(data_tmp, u8);
                    ip6[ix] = u8;
                }
                format_ip6addr(ip_str, ip6);
                cli_out("Info Element:: %02d %20s | Size: %02d | Value: (%s)\n", elem->element, elem_name, elem->data_size, ip_str);
                break;
            case bcmFlowtrackerExportElementTypeTimestampNewLearn:
            case bcmFlowtrackerExportElementTypeTimestampFlowStart:
            case bcmFlowtrackerExportElementTypeTimestampFlowEnd:
            case bcmFlowtrackerExportElementTypeTimestampCheckEvent1:
            case bcmFlowtrackerExportElementTypeTimestampCheckEvent2:
            case bcmFlowtrackerExportElementTypeTimestampCheckEvent3:
            case bcmFlowtrackerExportElementTypeTimestampCheckEvent4:
                for (ix = 0; ix < data_size; ix++) {
                    SHR_PKT_UNPACK_U8(data_tmp, u8);
                    ts[ix] = u8;
                }
                sal_memset(ts_str, 0, 20);
                format_ts(ts_str, ts, data_size);
                cli_out("Info Element:: %02d %20s | Size: %02d | Value: (%s)\n", elem->element, elem_name, data_size, ts_str);
                break;

            case bcmFlowtrackerExportElementTypeFlowtrackerGroup:
                SHR_PKT_UNPACK_U16(data_tmp, u16);
                cli_out("Info Element:: %02d %20s | Size: %02d | Value: 0x%04x(%04d)\n", elem->element, elem_name, elem->data_size, u16 & 0xFFF, u16 & 0xFFF);
                break;
            case bcmFlowtrackerExportElementTypeReserved:
                cli_out("Info Element:: %02d %20s | Size: %02d | Value: (%s)\n", elem->element, elem_name, elem->data_size, "????");
                break;
            default:
                switch (data_size) {
                    case 1:
                        SHR_PKT_UNPACK_U8(data_tmp, u8);
                        if (elem->element == bcmFlowtrackerExportElementTypeCustom) {
                            cli_out("Info Element:: %02d %20s(%d) | Size: %02d | Value: 0x%02x(%02d)\n", elem->element, elem_name, elem->custom_id, elem->data_size, u8, u8);
                        } else if (elem->element == bcmFlowtrackerExportElementTypeFlowtrackerCheck) {
                            cli_out("Info Element:: %02d %20s(0x%x) | Size: %02d | Value: 0x%02x(%02d)\n", elem->element, elem_name, elem->check_id, elem->data_size, u8, u8);
                        } else {
                            cli_out("Info Element:: %02d %20s | Size: %02d | Value: 0x%02x(%02d)\n", elem->element, elem_name, elem->data_size, u8, u8);
                        }
                        break;
                    case 2:
                        SHR_PKT_UNPACK_U16(data_tmp, u16);
                        if (elem->element == bcmFlowtrackerExportElementTypeCustom) {
                            cli_out("Info Element:: %02d %20s(%d) | Size: %02d | Value: 0x%04x(%04d)\n", elem->element, elem_name, elem->custom_id, elem->data_size, u16, u16);
                        } else if (elem->element == bcmFlowtrackerExportElementTypeFlowtrackerCheck) {
                            cli_out("Info Element:: %02d %20s(0x%x) | Size: %02d | Value: 0x%04x(%04d)\n", elem->element, elem_name, elem->check_id, elem->data_size, u16, u16);
                        } else {
                            cli_out("Info Element:: %02d %20s | Size: %02d | Value: 0x%04x(%04d)\n", elem->element, elem_name, elem->data_size, u16, u16);
                        }
                        break;
                    case 3:
                        SHR_PKT_UNPACK_U8(data_tmp, u8);
                        SHR_PKT_UNPACK_U16(data_tmp, u16);
                        u32 = ((u8 << 16) | u16);
                        if (elem->element == bcmFlowtrackerExportElementTypeCustom) {
                            cli_out("Info Element:: %02d %20s(%d) | Size: %02d | Value: 0x%06x(%06d)\n", elem->element, elem_name, elem->custom_id, elem->data_size, u32, u32);
                        } else if (elem->element == bcmFlowtrackerExportElementTypeFlowtrackerCheck) {
                            cli_out("Info Element:: %02d %20s(0x%x) | Size: %02d | Value: 0x%06x(%06d)\n", elem->element, elem_name, elem->check_id, elem->data_size, u32, u32);
                        } else {
                            cli_out("Info Element:: %02d %20s | Size: %02d | Value: 0x%06x(%06d)\n", elem->element, elem_name, elem->data_size, u32, u32);
                        }
                        break;
                    case 4:
                        SHR_PKT_UNPACK_U32(data_tmp, u32);
                        if (elem->element == bcmFlowtrackerExportElementTypeCustom) {
                            cli_out("Info Element:: %02d %20s(%d) | Size: %02d | Value: 0x%08x(%08d)\n", elem->element, elem_name, elem->custom_id, elem->data_size, u32, u32);
                        } else if (elem->element == bcmFlowtrackerExportElementTypeFlowtrackerCheck) {
                            cli_out("Info Element:: %02d %20s(0x%x) | Size: %02d | Value: 0x%08x(%08d)\n", elem->element, elem_name, elem->check_id, elem->data_size, u32, u32);
                        } else {
                            cli_out("Info Element:: %02d %20s | Size: %02d | Value: 0x%08x(%08d)\n", elem->element, elem_name, elem->data_size, u32, u32);
                        }
                        break;
                    default:
                        break;

                }
        }

        data += (elem->data_size);
    }

    return BCM_E_NONE;
}


int bcma_ft_ipfix_print_record(int unit, int set_c, int payload_length, uint8 *data)
{
    int rv = BCM_E_NONE;
    uint8 *data_set;
    uint16 set_id, set_length;
    int num_els;
    bcm_flowtracker_export_template_t template_id;
    bcm_flowtracker_export_element_info_t *elem_list;
#if 0
    int ix = 0;
    char elem_name[BCMI_FT_EXPORT_ELEMENT_NAME_LEN_MAX];
#endif

    data_set = data;
    SHR_PKT_UNPACK_U16(data_set, set_id);
    SHR_PKT_UNPACK_U16(data_set, set_length);
    if (set_length == 0) {
        cli_out("Data Set with invalid length: id= %02d len = %02d\n", set_id, set_length);
        return -1;
    }

    cli_out("*************************************************\n");
    cli_out("IPFIX data set # %02d, set_id = %02d (0x%02x) record length = %02d\n",
            set_c, set_id, set_id, set_length);

    rv = bcma_ft_setid_template_id_get(unit, set_id, &template_id);
    if (BCM_FAILURE(rv)) {
        cli_out("No template created with set_id = %d\n", set_id);
        return -1;
    }

    rv = bcm_flowtracker_export_template_get(unit, template_id,
                                             &set_id, 0, NULL, &num_els);
    if (BCM_FAILURE(rv)) {
        cli_out("template_get count failed with rv = %d\n", rv);
        return -1;
    }


    elem_list = sal_alloc((sizeof(bcm_flowtracker_export_element_info_t) * num_els), "template_get element list");

    rv = bcm_flowtracker_export_template_get(unit, template_id,
                                             &set_id, num_els,
                                             elem_list, &num_els);
    if (BCM_FAILURE(rv)) {
        cli_out("template_get list failed with rv = %d\n", rv);
        return -1;
    }

#if 0
    cli_out("Template Id matching with set id %d is %d and num of elements = %d\n", set_id, template_id, num_els);
    for (ix = 0; ix < num_els; ix++) {

        rv = bcma_ft_export_elem_name_get(unit, elem_list[ix].element, NULL, elem_name);
        BCM_IF_ERROR_RETURN(rv);
        if (elem_list[ix].element == bcmFlowtrackerExportElementTypeFlowtrackerCheck) {
            cli_out("Template Id: %2d Info Element(%2d) = %2d (%20s) Size = %02d"
                    " (0x%x)\n", template_id, ix, elem_list[ix].element,
                    elem_name, elem_list[ix].data_size, elem_list[ix].check_id);
        } else {
            cli_out("Template Id: %2d Info Element(%2d) = %2d (%20s) Size = %02d\n",
                    template_id, ix, elem_list[ix].element, elem_name, elem_list[ix].data_size);
        }
    }
#endif

    rv = bcma_ipfix_data_record_print(unit, set_length, data_set,
                                      num_els, elem_list);
    if (BCM_FAILURE(rv)) {
        cli_out("ipfix data_set parsing failed for set = %d\n", set_id);
        return -1;
    }

    cli_out("*************************************************\n");


    return BCM_E_NONE;
}


int bcma_ft_ipfix_template_print(int unit, int payload_length, uint8 *payload_data)
{
    uint16 d16;
    uint32 d32;
    uint16 set_id;
    int ix, set_c, set_length;
    uint8 *data, *data_set;

    data = payload_data;

    if (0) {
        for (ix = 0; ix < (payload_length - 4); ix+=8) {
            cli_out("%04d: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x\n",
                    (ix/8), data[ix], data[ix+1], data[ix+2], data[ix+3],
                            data[ix+4], data[ix+5], data[ix+6], data[ix+7]);
        }
    }

    if (payload_length < 16) {
        cli_out("ipfix header parsing failed\n");
        return BCM_E_FAIL;
    }

    /* Print ipfix header. */
    cli_out("*************************************************\n");
    SHR_PKT_UNPACK_U16(data, d16);
    cli_out("IPFIX version = %02d (0x%02x)\n", d16, d16);

    SHR_PKT_UNPACK_U16(data, d16);
    cli_out("IPFIX message length = %02d (0x%02x)\n", d16, d16);

    SHR_PKT_UNPACK_U32(data, d32);
    cli_out("IPFIX export timestamp = %02d (0x%02x)\n", d32, d32);

    SHR_PKT_UNPACK_U32(data, d32);
    cli_out("IPFIX sequence number = %02d (0x%02x)\n", d32, d32);

    SHR_PKT_UNPACK_U32(data, d32);
    cli_out("IPFIX observation domain id = %02d (0x%02x)\n", d32, d32);
    cli_out("*************************************************\n");

    payload_length -= 16;

    set_c = 1;
    while (payload_length > 0) {
        data_set = data;
        SHR_PKT_UNPACK_U16(data_set, set_id);
        SHR_PKT_UNPACK_U16(data_set, set_length);

        data_set = data;
        if (bcma_ft_ipfix_print_record(unit, set_c, set_length, data_set) != BCM_E_NONE) {
            cli_out("set_id:%d, set:%d:ipfix parsing failed\n", set_id, set_c);
            return BCM_E_FAIL;
        }

        set_c++;
        data += set_length;
        payload_length -= set_length;
    }

    return BCM_E_NONE;

}

STATIC char *_ft_rmon_reason_names[bcmRxReasonCount] = BCM_RX_REASON_NAMES_INITIALIZER;

STATIC void
ft_rmon_dump_packet_rx(int unit, pup_t *pup, uint32 report)
{
    bcm_pkt_t *pkt;
    char        pfx[64];
    char        info_str[256];
    pwu_t       *pu = &ft_rmon_units[unit];
    int         i;

    pkt = &pup->rx_pkt;


    ft_rmon_dump_start(unit, pfx, pup, report, pkt->dma_channel,
                  pu->pu_packet_received);

    if (report & REPORT_DMA) {
        if (pkt->_dv) {
            soc_dma_dump_dv(unit, pfx, (dv_t *)pkt->_dv);
        }
    }

    if (report & REPORT_RAW) {
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
            if (SOC_IS_TRIDENT3X(unit)) {
                cli_out("%slength %d (%d). rx-port %d. cos %d. prio_int %d. vlan %d. "
                    "reason 0x%x. %s.\n", pfx,
                    pkt->pkt_len, pkt->tot_len, pkt->rx_port,
                    pkt->cos, pkt->prio_int, pkt->vlan,
                    pkt->rx_reason,
                    (pkt->rx_untagged) ?
                    "Untagged" : "Ingress tagged");
            } else {
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

            cli_out("%s%sopcode %d. %s matched %d. classification-tag %d.\n",
                pfx, info_str,
                pkt->opcode,
                (pkt->flags & BCM_RX_TRUNCATED) ? "Truncated." : "",
                pkt->rx_matched, pkt->rx_classification_tag);

        for (i = 0; i < bcmRxReasonCount; i++) {
            if (BCM_RX_REASON_GET(pkt->rx_reasons, i)) {
                cli_out("%sreasons: %s\n", pfx, _ft_rmon_reason_names[i]);
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
            int ix;
            uint8 *data;
            int encap_length;

            cli_out("*************************************************\n");
            data = &(pkt->_pkt_data.data[0]);
            for (ix = 0; ix < (pkt->pkt_len); ix+=8) {
                cli_out("%08x   %02x %02x %02x %02x %02x %02x %02x %02x\n",
                        ix, data[ix], data[ix+1], data[ix+2], data[ix+3],
                        data[ix+4], data[ix+5], data[ix+6], data[ix+7]);
            }
            cli_out("*************************************************\n");

            d_packet_format(pfx, DECODE_ETHER, pkt->_pkt_data.data,
                            pkt->pkt_len, NULL);

            /* DA+SA+TPID+VTAG+TPID+IP+UDP */
            encap_length = 46;

            if (bcma_ft_ipfix_template_print(unit,
                    (pkt->pkt_len - encap_length - 4), &(pkt->_pkt_data.data[encap_length]))) {
                cli_out("ipfix parsing failed\n");
            }


        }
    }
#endif /* __KERNEL__ */
#endif /* __STRICT_ANSI__ */
#endif /* NO_SAL_APPL */

}

STATIC void
ft_rmon_dump_packet(int unit, pup_t *pup, uint32 report)
{
    pwu_t       *pu = &ft_rmon_units[unit];

    switch (pu->mode) {
        case PW_MODE_RX:
            ft_rmon_dump_packet_rx(unit, pup, report);
            break;
        default:
            break;
    }
}

STATIC cmd_result_t
ft_rmon_dump_log(int unit, int n)
/*
 * Function:    ft_rmon_dump_log
 * Purpose:     Dump out currently logged packets
 * Parameters:  unit - unit #
 *              n - number of packets to dump, -n means last "n"
 *                  packets.
 * Returns:     CMD_OK - success
 */
{
    pwu_t    *pu = &ft_rmon_units[unit];
    pup_t    *pup;
    int       abs_n;

    PW_LOCK(unit); /* Stop updates */

    if ((pu->pu_log == NULL) || (n == 0)) { /* Nothing to show */
    PW_UNLOCK(unit);
    cli_out("ft_rmon_dump_log[%d]: Warning: no packets logged "
                "or selected to dump\n", unit);
    return CMD_OK;          /* Return nothing */
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

    if (n < 0) {            /* Dump last N packets */
    for (pup = pu->pu_log; abs_n-- != 0; pup = pup->pup_next) {
        ft_rmon_dump_packet(unit, pup, pu->pu_dump_options);
    }
    } else {                /* Dump first N packets */
    for (pup = pu->pu_log->pup_prev; abs_n-- != 0; pup = pup->pup_prev) {
        ft_rmon_dump_packet(unit, pup, pu->pu_dump_options);
    }
    }
    PW_UNLOCK(unit);

    return CMD_OK;
}

STATIC  void
ft_rmon_log_packet(int unit, pup_t *pup)
/*
 * Function:    ft_rmon_log_packet
 * Purpose: Commit a recieved packet to the logs.
 * Parameters:  unit - unit #
 *      pup - pointer to pup to add to log.
 * Returns: Nothing.
 *
 * Notes:   Assumes PW_LOCK held.
 */
{
    pwu_t   *pu = &ft_rmon_units[unit];

    /*
     * There are 2 cases:
     * 1) First entry
     * 2) Log is full so the LRU packet is dropped (common case)
     * 3) Log is not full, no packet is dropped.
     */

    if (pu->pu_log_cnt == 0) {      /* 1) First entry */
        pu->pu_log = pup->pup_next = pup->pup_prev = pup;
        pu->pu_log_cnt = 1;
    } else if (pu->pu_log_cnt == pu->pu_log_max) { /* 2) Full log, drop one */
        pup_t   *tp;

        tp = pu->pu_log->pup_prev;  /* Entry to delete */
        pup->pup_next = pu->pu_log; /* Link new one in at head */
        pup->pup_prev = tp->pup_prev;
        pup->pup_next->pup_prev = pup;
        pup->pup_prev->pup_next = pup;

        ft_rmon_pup_free(unit, tp);
    } else {                /* 3) Just add to chain */
        /*
         * Case 2 - add new entry and increment log count.
         */
        pu->pu_log_cnt++;
        pup->pup_next = pu->pu_log;
        pup->pup_prev = pu->pu_log->pup_prev;
        pup->pup_next->pup_prev = pup;
        pup->pup_prev->pup_next = pup;
    }
    pu->pu_log = pup;           /* Set new list */
}


STATIC  void
ft_rmon_process_pending(int unit)
/*
 * Function:    ft_rmon_process_pending
 * Purpose: Process packets queued from interrupt
 * Parameters:  unit - unit #
 * Returns: Nothing.
 *
 * Notes:   Can be called only from a non-interrupt context.
 */
{
    pwu_t       *pu = &ft_rmon_units[unit];
    pup_t       *pup;
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
        pup_t  *tp = pup->pup_next;

        pup->pup_seqno = ++pu->pu_packet_received;
        ft_rmon_log_packet(unit, (pup_t *)pup);
        chan = 0; /* Per-channel counted in Rx call-back */
        pu->pu_ch_count[chan]++;
        ft_rmon_dump_packet(unit, (pup_t *)pup, pu->pu_report); /* Dump packet */
        pup = tp;
        pu->last_pkt_count++;
    }
}

/****************************************************************
 *
 * RX API packet watcher code, non-interrupt
 */
STATIC bcm_rx_t
ft_rmon_rx_callback(int unit, bcm_pkt_t *pkt, void *cookie)
{
    pup_t *pup;
    pwu_t   *pu = &ft_rmon_units[unit];
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

    pup = ft_rmon_pup_alloc(unit);
    if (!pup) {
        LOG_VERBOSE(BSL_LS_SOC_COMMON,
                    (BSL_META_U(unit,
                                "FtRemoteMon: Failed to allocate pup struct.  Discarding\n")));
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

    sal_sem_give(ft_rmon_units[unit].pu_sema);
    return BCM_RX_HANDLED_OWNED;
}

static uint8 *
convert_to_hex(int unit, char *packet_data, uint32 *length)
{
    uint8 *p;
    char tmp, data_iter;
    int data_len, i, j, pkt_len, data_base;

    /* If string data starts with 0x or 0X, skip it */
    if ((packet_data[0] == '0')
        && (packet_data[1] == 'x' || packet_data[1] == 'X')) {
        data_base = 2;
    } else {
        data_base = 0;
    }

    data_len = strlen(packet_data) - data_base;
    pkt_len = (data_len + 1) / 2;

    p = (uint8 *)sal_alloc(pkt_len, "payload");;
    if (p == NULL) {
        return NULL;
    }
    sal_memset(p, 0, pkt_len);
    /* Convert char to value */
    i = j = 0;
    while (j < data_len) {
        data_iter = packet_data[data_base + j];
        if (('0' <= data_iter) && (data_iter <= '9')) {
            tmp = data_iter - '0';
        } else if (('a' <= data_iter) && (data_iter <= 'f')) {
            tmp = data_iter - 'a' + 10;
        } else if (('A' <= data_iter) && (data_iter <= 'F')) {
            tmp = data_iter - 'A' + 10;
        } else if (data_iter == ' ') {
            ++j;
            continue;
        } else {
            sal_free((void *)p);
            return NULL;
        }

        /* String input is in 4b unit. Below we're filling in 8b:
           offset is /2, and we shift by 4b if the input char is odd */
        p[i / 2] |= tmp << (((i + 1) % 2) * 4);
        ++i;
        ++j;
    }

    *length = i / 2;
    return p;
}

cmd_result_t
bcmi_ftrmon_data_parse(int unit, args_t *a)
{
    int rv = BCM_E_NONE;
    parse_table_t pt;
    char *data = NULL;
    uint8 *hexdata = NULL;
    uint32 length = 0 ;

    parse_table_init(unit, &pt);
    parse_table_add(&pt, "Data", PQ_DFL|PQ_STRING, NULL, &data, NULL);

    if (ARG_CNT(a)) {
        if (parse_arg_eq(a, &pt) < 0 || ARG_CNT(a) > 0) {
            cli_out("%s: Error: Unknown option: %s\n", ARG_CMD(a), ARG_CUR(a));
            parse_arg_eq_done(&pt);
            return CMD_FAIL;
        }
    } else {
        cli_out("Error: Data not given\n");
        parse_arg_eq_done(&pt);
        return CMD_FAIL;
    }
    hexdata = convert_to_hex(unit, data, &length);
    parse_arg_eq_done(&pt);

    rv = bcma_ft_ipfix_template_print(unit, (int) length, hexdata);
    sal_free(hexdata);

    if (BCM_FAILURE(rv)) {
        return CMD_USAGE;
    }

    return CMD_OK;
}

#endif /* BCM_FLOWTRACKER_SUPPORT */

char cmd_ft_rmon_usage[] =
    "Parameters: [report +/-<values>] [stop|start] [count]\n"
#ifndef COMPILER_STRING_CONST_LIMIT
    "\tlog # - specifies # packets to remember\n"
    "\tdump [# | options] - # > 0 dumps # packets from start of log\n"
    "\t                     # < 0 dumps last # packets\n"
    "\t                     nothing - dumps all logs\n"
    "\t                     options - count   - print packet # only\n"
    "\t                               decode  - attempt to decode packet\n"
    "\t                               raw     - dump raw byte stream (host order)\n"
    "\t                               Prepend + to set, - to clear, nothing to toggle\n"
    "\treport- count   - print packet # only\n"
    "\t        decode  - attempt to decode packet\n"
    "\t        raw     - dump raw byte stream (host order)\n"
    "\t        Prepend + to set, - to clear, nothing to toggle\n"
    "\tquiet - disable all reporting\n"
    "\tstart - start the FtRemoteMon-daemon\n"
    "\tstop  - stop the FtRemoteMon-daemon and deallocate log space\n"
    "\tcount - print # packets received.\n"
    "\treset - reinitialize configuration from bcm_rx_* API state\n"
#endif
    ;

cmd_result_t
cmd_ft_rmon_command(int unit, args_t *a)
/*
 * Function:    ft_rmon_command
 * Purpose: Command interface to the "ipfix rmon" thread.
 * Parameters:
 *      "verbose" - print lots of stuff.
 *      "start" - start ipfix rmon  thread.
 *      "stop" - stop ipfix rmon thread.
 *      "quiet" - print no stuff.
 * Returns: CMD_OK - success
 *      CMD_FAIL - failed
 *      CMD_USAGE - print usage instructions
 */
{
#if defined(BCM_FLOWTRACKER_SUPPORT)
    char    *c;
    int     n;
    uint32  on = 0;         /* flags to turn on */
    pwu_t   *pu = &ft_rmon_units[unit]; /* Control this unit */

    if (!sh_check_attached(ARG_CMD(a), unit)) {
    return(CMD_FAIL);
    }

    if (!pu->init_done) {
        ft_rmon_init(unit);
    }

    if (!ARG_CNT(a)) {          /* Nothing passed */
    cli_out("%s: Status: %s. Mode %s. Buffering up to %d packets.%s\n",
                ft_rmon_name[unit],
                (pu->pu_flags & PU_F_RUN) ? "Running" : "Not Running",
                ft_rmon_modes[pu->mode], pu->pu_log_max,
                (pu->pu_flags & PU_F_STOP) ? " STOP Requested." : "");
        if (pu->rate <= 0) {
            cli_out("Rate limiting is off.\n");
        } else {
            cli_out("Rate limit is %d (soc intvl %d).\n", pu->rate,
                    pu->pu_interval);
        }

    cli_out("Reporting is enabled for: ");
    parse_mask_format(80, ft_rmon_report_table, pu->pu_report);
    cli_out("Reporting is disabled for: ");
    parse_mask_format(80, ft_rmon_report_table, pu->pu_report ^ ~0);

        cli_out("Dump options are enabled for: ");
    parse_mask_format(80, ft_rmon_report_table, pu->pu_dump_options);
    cli_out("Dump options are disabled for: ");
    parse_mask_format(80, ft_rmon_report_table, pu->pu_dump_options ^ ~0);

        cli_out("RX on for channel(s): ");
    if (pu->pu_channel) {
        parse_mask_format(50, ft_rmon_channel_table, pu->pu_channel);
        cli_out("RX off for channel(s): ");
        parse_mask_format(50, ft_rmon_channel_table, ~pu->pu_channel);
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
        cli_out("%s: WARNING: Already running\n", ft_rmon_name[unit]);
        } else {
        on |= PU_F_RUN;
        }
    } else if (!sal_strcasecmp(c, "stop")) {
        if (!(pu->pu_flags & PU_F_RUN)) {
        cli_out("%s: WARNING: Not running\n", ft_rmon_name[unit]);
        return(CMD_OK);     /* Return OK to avoid rc aborts */
        }
        on |= PU_F_STOP;
    } else if (!sal_strcasecmp(c, "reset")) {
        if (pu->pu_flags & PU_F_RUN) {
        cli_out("%s: Unable to reset configuration "
                        "while running\n", ft_rmon_name[unit]);
        return(CMD_FAIL);
        } else {
                pu->init_done = FALSE;
                ft_rmon_init(unit);
                return CMD_OK;
            }
    } else if (!sal_strcasecmp(c, "channel")) {
        if (ARG_CNT(a) == 0) {
        cli_out("%s: RX on for channel(s): ", ft_rmon_name[unit]);
        if (pu->pu_channel) {
            parse_mask_format(50, ft_rmon_channel_table, pu->pu_channel);
            cli_out("%s: RX off for channel(s): ", ft_rmon_name[unit]);
            parse_mask_format(50, ft_rmon_channel_table, ~pu->pu_channel);
        } else {
            cli_out(" -- using default --\n");
        }
        return(CMD_OK);
        } else if (pu->pu_flags & PU_F_RUN) {
        cli_out("%s: Unable to configure DMA channels "
                        "while running\n", ft_rmon_name[unit]);
        return(CMD_FAIL);
        }

        /* Set channels */
        while ((c = ARG_CUR(a)) != NULL &&
           !parse_mask(c, ft_rmon_channel_table,
                   (uint32 *)&pu->pu_channel)) {
        ARG_NEXT(a);    /* Bump arg if matched report */
        }

        if (pu->pu_flags & PU_F_RUN) {
        cli_out("%s: Warning channel re-assignment will not take\n"
                        "%s: until restarted.\n",
                        ft_rmon_name[unit], ft_rmon_name[unit]);
        }
    } else if (!sal_strcasecmp(c, "quiet")) {
        pu->pu_report = 0;
    } else if (!sal_strcasecmp(c, "rate")) {
        c = ARG_GET(a);
            if (c == NULL) {
                if (pu->rate <= 0) {
                    cli_out("%s: No rate limiting.\n", ft_rmon_name[unit]);
                } else {
                    cli_out("%s: Rate limit is currently %d pkts/sec.\n",
                            ft_rmon_name[unit], pu->rate);
                }
                return CMD_OK;
            }
            pu->rate = strtoul(c, NULL, 10);
            if (_ft_rmon_set_rate(unit) < 0) {
                return CMD_FAIL;
            }
            return CMD_OK;
    } else if (!sal_strcasecmp(c, "report")) {
        if (ARG_CNT(a)) {
        while ((c = ARG_CUR(a)) != NULL &&
               !parse_mask(c, ft_rmon_report_table, &pu->pu_report)) {
            ARG_NEXT(a);    /* Bump arg if matched report */
        }
        } else {            /* Print values */
        cli_out("%s: Reporting on for: ", ft_rmon_name[unit]);
        parse_mask_format(50, ft_rmon_report_table, pu->pu_report);
        cli_out("%s: Reporting off for: ", ft_rmon_name[unit]);
        parse_mask_format(50, ft_rmon_report_table, ~pu->pu_report);
        }
    } else if (!sal_strcasecmp(c, "log")) {
        c = ARG_GET(a);
        if (c) {            /* We have an option */
        if (pu->pu_flags & PU_F_RUN) {
            cli_out("%s: Can not change \"log\" "
                            "while FtRemoteMon-daemon running\n", ft_rmon_name[unit]);
            return(CMD_FAIL);
        }
        if (!sal_strcasecmp(c, "on")) {
            n = PU_LOG_CNT;
        } else if (!sal_strcasecmp(c, "off")) {
            n = 1;
        } else if (isint(c)) {  /* Set log size */
            n = parse_integer(c);
            if (n < 1) {
            cli_out("%s: What is %s?\n", ft_rmon_name[unit], c);
            n = 1;
            }
        } else {
            cli_out("%s: Invalid count \"%s\"\n", ft_rmon_name[unit], c);
            return(CMD_FAIL);
        }
        pu->pu_log_max = n;
        }
        cli_out("%s: Logging(%d-packets)\n",
                    ft_rmon_name[unit], pu->pu_log_max);
        if (!c || !*c) {
        return(CMD_OK);
        }
    } else if (!sal_strcasecmp(c, "dump")) {
        c = ARG_GET(a);
        if (!c) {           /* no args */
        n = pu->pu_log_cnt;
        } else if (!isint(c)) {
                if (!sal_strcasecmp(c, "options")) {
                    if (ARG_CNT(a)) {
                        while ((c = ARG_CUR(a)) != NULL &&
                               !parse_mask(c, ft_rmon_report_table,
                                           &pu->pu_dump_options)) {
                            ARG_NEXT(a);    /* Bump arg if matched report */
                        }
                    } else {            /* Print values */
                        cli_out("%s: Dump options on for: ", ft_rmon_name[unit]);
                        parse_mask_format(50, ft_rmon_report_table, pu->pu_dump_options);
                        cli_out("%s: Dump options off for: ", ft_rmon_name[unit]);
                        parse_mask_format(50, ft_rmon_report_table, ~pu->pu_dump_options);
                    }
                    return(CMD_OK);
                } else {
                    cli_out("%s: Invalid log count \"%s\"\n", ft_rmon_name[unit], c);
                    return(CMD_FAIL);
                }
            } else {
        n = parse_integer(c);
        }
        return(ft_rmon_dump_log(unit, n));
    } else if (!sal_strcasecmp(c, "count")) {
        COMPILER_DOUBLE cur_time, last_time;
        int     cur_count, last_count;
        int     rate;

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
                    ft_rmon_name[unit],
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
        cli_out("%s: Invalid interval: %s\n", ft_rmon_name[unit], c);
        return(CMD_FAIL);
        }
        pu->pu_interval = n;
    } else if (!sal_strcasecmp(c, "mode")) {
        if ((c = ARG_GET(a)) == NULL) {
        cli_out("Current mode is %s\n", ft_rmon_modes[pu->mode]);
        return CMD_OK;
        }

        if (ft_rmon_running(unit)) {
        cli_out("Can't set modes while running\n");
        return CMD_FAIL;
        }

        if (!sal_strcasecmp(c, "rx")) {
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
    } else if (!sal_strcasecmp(c, "parse")) {
        return bcmi_ftrmon_data_parse(unit, a);
    } else {
        return(CMD_USAGE);
    }
    }

    /* Everything OK, start or stop ipfix rmon now */

    if (on & PU_F_RUN) {        /* start off thread */
    return(ft_rmon_start(unit, TRUE));
    } else if (on & PU_F_STOP) {    /* stop task */
    return(ft_rmon_stop(unit, TRUE));
    }
#endif

    return(CMD_OK);
}
