/*
 * Multi-Threaded Tx Blast Test Suite
 *
 * This feature is installed as a new diag shell command "BlastTX" using
 * cmdlist_add().
 *
 * This feature set allows users to exercise the bcm_tx()
 * functionality.  The users can create multiple transmit threads that send
 * through the switch. The objective it to test max TX rates.
 */
#include <sys/time.h>

#include <appl/diag/system.h>
#include <bcm/debug.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/cosq.h>
#include <bcm/link.h>

#define DEFAULT_PACKET_SIZE   256
#define DEFAULT_PKT_PRI         2
#define DEFAULT_TEST_TIME      64
#define DEFAULT_THREADS         4
#define DEFAULT_VLAN	        1
#define DEFAULT_ETHTYPE    0x0800
#define DEFAULT_TPID	   0x8100
#define DEFAULT_CFI 	        0

#define MAX_THREADS            8
#define MAX_NAME               8
#define MAX_PORTS	      128

#define NUM_PKT_BUFFERS	       (1 << 9)
#define MAX_ARRAY_MODE_BUFFERS (1 << 5) /* SDK Limit is 34 */

#define FLAG_TX_START     (1 << 0)
#define FLAG_TX_SYNC      (1 << 1)
#define FLAG_NO_LOOP      (1 << 2)
#define FLAG_FORCE_TX     (1 << 3)
#define FLAG_TX_ARRAY     (1 << 4)
#define FLAG_UNICAST      (1 << 5)
#define FLAG_NO_WAIT      (1 << 6)
#define FLAG_SIMPLE_TX    (1 << 7)
#define FLAG_TX_STATUS    (1 << 8)
#define FLAG_STATUS_CLEAR (1 << 9)
#define FLAG_ACTIVE     (1 << 29)
#define FLAG_WAIT       (1 << 30)

#define TX_STALL_LOOP_COUNT 10000
#define TX_STALL_TIME_MSECS    10

struct blast_tx_totals_s {
    int                 thread_count;   /* Number of threads for this run */
    int                 max_tx_latency;
    double              accumulated_byte_rate;  /* Sum of all thread byte rates */
    double              accumulated_packet_rate;        /* Sum of all thread packet rates */
    unsigned long long  total_bytes;    /* Total bytes transmitted */
    unsigned long long  total_packets;  /* Total packets transmitted */
    unsigned long long  accumulated_tx_latency;
    unsigned int        running;
};

/* L2 Header template */
struct l2header_s {
    bcm_mac_t           mac_dst;
    bcm_mac_t           mac_src;
    unsigned int        ethtype;
    unsigned int        vlan;
    unsigned int        vpri;
    unsigned int        cfi;
    unsigned int        tpid;
};

/* Per thread data structure for managing TX threads */
struct blast_tx_control_s {
    sal_thread_t        tx_tid;
    bcm_port_t          port;   /* Front panel port used by this thread */
    char                name[MAX_NAME]; /* Thread name for this thread */
    uint8               id;     /* Numeric ID for this thread (an index) */
    int                 max_tx_latency;
    int                 num_cos;        /* Number of COSQs */
    int                 packet_size;    /* Size of packets transmitted by this thread */
    int                 unit;   /* Unit this thread is running on */
    uint32              flags;  /* Assorted control flags */
    unsigned int        test_time;
    unsigned int        *thread_running; /* Pointer to active flag, thread exits when cleared */
    unsigned long long  last_packet;    /* Last packet processed by async TX callback */
    unsigned long long  run_time;       /* Total run time for this thread in usecs */
    unsigned long long  start_time;     /* Start time for this thread in usecs */
    unsigned long long  stop_time;      /* Stop time for this thread in usecs */
    unsigned long long  tx_bytes;       /* Total bytes transmitted for this thread */
    unsigned long long  tx_packets;     /* Total packets transmitted for this thread */
    unsigned long long  stall_count;    /* Stall on free packets for this thread */
    unsigned long long  callback_count; /* Number times async callback was called */
    unsigned long long  accumulated_tx_latency; /* Accumulate Tx latency */
    struct blast_tx_totals_s *totals;
    struct l2header_s   l2header;
    int                 save_lb;
    int                 save_discard;
};

/* List of ports eligible for use by TX blast */
struct port_list_s {
    bcm_port_t          list[MAX_PORTS];
    int                 count;
    int                 current;
};

static sal_mutex_t print_mutex = 0;

/* Local Function Prototypes */
static unsigned long long time_usecs(void);
static time_t       time_secs(void);
static char        *blast_time_string(unsigned long long usecs, char *buffer,
                                      size_t len);
static bcm_error_t  get_port_list(int unit, struct port_list_s *port_list,
                                  bcm_pbmp_t pbmp);
static void         my_async_callback(int unit, bcm_pkt_t * pkt, void *cookie);
static void         packet_free_callback(int unit, bcm_pkt_t * pkt, void *cookie);
static bcm_error_t  simple_tx(struct blast_tx_control_s *const thread_control);
static bcm_error_t  tx_setup(struct blast_tx_control_s *thread_control,
                             int *save_lb, int *save_discard);
static bcm_error_t  tx_cleanup(struct blast_tx_control_s *thread_control);
static void         simple_tx_thread(void *param);
static void         tx_thread(void *param);
static bcm_error_t  blast_tx(int unit, struct blast_tx_control_s *thread_list,
                             struct port_list_s *port_list,
                             struct blast_tx_totals_s *totals, const int count,
                             const int size, const int test_time,
                             struct l2header_s *l2header, unsigned int flags,
                             unsigned int *running);
static void         thread_status(struct blast_tx_control_s *thread_list);
static void         reset_thread_stats(struct blast_tx_control_s *thread_list);
int blast_tx_test(int unit, args_t *arg, void *p);

static char         blast_tx_usage[] =
  "Multi-Threaded Blast Tx Test\n"
  "Blast packets using bcm_tx() or bcm_tx_array(). This utility can be\n"
  "used to measure the performance of TX (and RX) peformance. It allows the\n"
  "user to create one or more threads that send packets as fast as possible\n"
  "to the switch. The test can be run for a fixed time or run continuously.\n\n"
  "usage: BlastTX subcmd [options]\n"
  "Where options are:\n"
  "  start                     : Start TX threads\n"
  "     ForceTX=<true/false>   : Force TX on down port\n"
  "     NoLink=<true/false>    : No loop back, assume port is linked up\n"
  "     SYNC=<true/false>      : Synchronous transmission mode (default false)\n"
  "     ARray=<true/false>     : Array transmission mode (default false)\n"
  "     UNIcast=<true/false>   : Transmit unicast packets instead of SOBMH\n"
  "     Size=<packet_size>     : Size of Tx packets in bytes\n"
  "     THreads=<thread_count> : Number of parallel Tx threads to create\n"
  "     TestTime=<test_time>   : Seconds to run test (0 = forever)\n"
  "     VLan=<vlan>            : VLAN for packets\n"
  "     Prio=<priority>        : VLAN packet priority\n"
  "     DstMac=<priority>      : Destination MAC address\n"
  "     SrcMac=<priority>      : Source MAC address\n"
  "  simple                    : Simple test alloc/tx\n"
  "     NoLink=<true/false>    : No loop back, assume port is linked up\n"
  "     SYNC=<true/false>      : Synchronous transmission mode (default false)\n"
  "     UNIcast=<true/false>   : Transmit unicast packets instead of SOBMH\n"
  "     Size=<packet_size>     : Size of Tx packets in bytes\n"
  "     THreads=<thread_count> : Number of parallel Tx threads to create\n"
  "     TestTime=<test_time>   : Seconds to run test (0 = forever)\n"
  "     VLan=<vlan>            : VLAN for packets\n"
  "     Prio=<priority>        : VLAN packet priority\n"
  "     DstMac=<priority>      : Destination MAC address\n"
  "     SrcMac=<priority>      : Source MAC address\n"
  "  ports PortBitMap=<pbm>    : Restrict test to use only these ports\n"
  "  status [clear]            : print status of tx forever threads\n"
  "  stop [thread id ..]       : Stop tx forever threads\n";

/* End Local Function Prototypes */

/* Need an extra long usec time */
static unsigned long long
time_usecs(void)
{
    struct timeval      ltv;

    gettimeofday(&ltv, NULL);
    return (((unsigned long long) ltv.tv_sec * SECOND_USEC) +
            (unsigned long long) ltv.tv_usec);
}

/* Get time of day in seconds since the epoch */
static              time_t
time_secs(void)
{
    struct timeval      ltv;

    gettimeofday(&ltv, NULL);
    return ltv.tv_sec;
}

/* Format a time string for printing */
static char        *
blast_time_string(unsigned long long usecs, char *buffer, size_t len)
{
    const int           levels = 3;
    const char         *const units_list[3] = { "usec", "msec", "sec" };
    const char         *units;
    int                 idx;
    unsigned            scaled_time;

    for (idx = 0; idx < levels; idx++) {
        units = units_list[idx];
        if (usecs < 12000) {
            break;
        }
        usecs = (usecs + 500) / 1000;
    }

    scaled_time = (unsigned) usecs;
    /* "%llu" seems to be broken for our libc so avoid printing
     * unsigned long long
     * */

    sal_snprintf(buffer, len, "%u %s", scaled_time, units);
    return buffer;
}

/* Add ports to list of those eligible for use by this test. */
static              bcm_error_t
get_port_list(int unit, struct port_list_s *port_list, bcm_pbmp_t pbmp)
{
    bcm_port_t          port;

    sal_memset(port_list, 0, sizeof(struct port_list_s));
    BCM_PBMP_ITER(pbmp, port) {
        int                 enable;

        BCM_IF_ERROR_RETURN(bcm_port_enable_get(unit, port, &enable));
        if (enable == 0) {
            /*
             * Disabled ports cannot be used
             */
            continue;
        }
        port_list->list[port_list->count] = port;
        port_list->count++;
    }
    return BCM_E_NONE;
}

/* Asynchronous packet free callback routine (doesn't really free anything) */
static void
my_async_callback(int unit, bcm_pkt_t * pkt, void *cookie)
{
    struct blast_tx_control_s *thread_control = (struct blast_tx_control_s *) cookie;

    /* Make sure this thread is still running */
    if ((thread_control->flags & FLAG_ACTIVE) != 0) {
        const sal_usecs_t   now = sal_time_usecs();
        int                 idx;
        int                 pkt_tx_count;       /* Packet count from packet being freed */
        int                 txLatency;
        sal_usecs_t         pkt_tx_time;        /* Time tag from packet being freed */
        uint8              *pkt_payload = &pkt->_pkt_data.data[16];

        /* Get packet count embedded in the packet header */
        pkt_tx_count = 0;
        for (idx = 0; idx < 4; idx++) {
            pkt_tx_count = (pkt_tx_count << 8) | (*pkt_payload & 0xFF);
            pkt_payload++;
        }
        pkt_tx_time = 0;
        for (idx = 0; idx < 4; idx++) {
            pkt_tx_time = (pkt_tx_time << 8) | (*pkt_payload & 0xFF);
            pkt_payload++;
        }

        *pkt_payload = 0;       /* Clear byte to indicate this packet is free */
        if (pkt_tx_count) {
            thread_control->last_packet = pkt_tx_count;
        }
        txLatency = SAL_USECS_SUB(now, pkt_tx_time);
        if (txLatency > thread_control->max_tx_latency) {
            thread_control->max_tx_latency = txLatency;
        }
        thread_control->accumulated_tx_latency += txLatency;

        thread_control->callback_count++;
    }
}

/* Simple synchronous packet free callback routine. Frees a packet  */
static void
packet_free_callback(int unit, bcm_pkt_t * pkt, void *cookie)
{
    struct blast_tx_control_s *thread_control = (struct blast_tx_control_s *) cookie;

    /* Make sure this thread is still running */
    if ((thread_control->flags & FLAG_ACTIVE) != 0) {
        const sal_usecs_t   now = sal_time_usecs();
        int                 idx;
        int                 txLatency;
        sal_usecs_t         pkt_tx_time;        /* Time tag from packet being freed */
        uint8              *pkt_payload = &pkt->_pkt_data.data[18];     /* After length */

        thread_control->callback_count++;

        pkt_tx_time = 0;
        for (idx = 0; idx < 4; idx++) {
            pkt_tx_time = (pkt_tx_time << 8) | (*pkt_payload & 0xFF);
            pkt_payload++;
        }

        txLatency = SAL_USECS_SUB(now, pkt_tx_time);
        if (txLatency > thread_control->max_tx_latency) {
            thread_control->max_tx_latency = txLatency;
        }
        thread_control->accumulated_tx_latency += txLatency;
    }

    bcm_pkt_free(unit, pkt);
}

/* Simple, self contained TX function */
static              bcm_error_t
simple_tx(struct blast_tx_control_s *const thread_control)
{
    bcm_pkt_t          *txPkt;
    int                 blk;
    int                 packet_len;
    int                 rv;
    int                 vtag;
    uint8              *myData;

    /*
     * Basic, single block packet allocation, allocate a packet with DMA-able
     * data buffer.
     */
    BCM_IF_ERROR_RETURN(bcm_pkt_alloc
                        (thread_control->unit, thread_control->packet_size, 0, &txPkt));

    vtag =
      (thread_control->l2header.
       vlan & 0xFFF) | ((thread_control->l2header.vpri & 0x7) << 13);
    txPkt->vlan = vtag; /* 802.1q VID or VSI or VPN. */

    /* Set up the addresses in the L2 header. */
    BCM_PKT_HDR_SMAC_SET(txPkt, thread_control->l2header.mac_src);
    BCM_PKT_HDR_DMAC_SET(txPkt, thread_control->l2header.mac_dst);

    BCM_PBMP_PORT_SET(txPkt->tx_pbmp, thread_control->port);

    BCM_PKT_HDR_TAGGED_LEN_SET(txPkt, thread_control->packet_size);
    BCM_PKT_HDR_TPID_SET(txPkt, thread_control->l2header.tpid);
    BCM_PKT_HDR_VTAG_CONTROL_SET(txPkt, vtag);

    if ((BCM_E_NONE ==
         bcm_pkt_byte_index(txPkt, BCM_IEEE_HDR_BYTES, &packet_len, &blk, &myData))
        && (myData != 0)) {
        const sal_usecs_t   tx_time = sal_time_usecs();
        const uint32        payload_len = thread_control->packet_size - 22;
        uint8              *end_ptr;
        uint8              *pkt_ptr;
        uint32              val;
        int                 idx;

        /* Set DOT3 length */
        pkt_ptr = myData;
        *pkt_ptr++ = (payload_len >> 8) & 0xFF; /* upper length byte */
        *pkt_ptr++ = payload_len & 0xFF;        /* lower length byte */

        for (idx = 0; idx < 4; idx++) {
            pkt_ptr[3 - idx] = (tx_time >> (idx * 8)) & 0xFF;
        }
        pkt_ptr += 4;

        /* Initialize Payload */
        val = pkt_ptr - BCM_PKT_IEEE(txPkt);
        for (end_ptr = &myData[packet_len - 4]; pkt_ptr < end_ptr; pkt_ptr++) {
            *pkt_ptr = val & 0xFF;
            val++;
        }
        /* Initialize FCS */
        for (end_ptr = &myData[packet_len]; pkt_ptr < end_ptr; pkt_ptr++) {
            *pkt_ptr = 0;
        }
    }

    if ((thread_control->flags & FLAG_TX_SYNC) == 0) {
        txPkt->call_back = packet_free_callback;
        txPkt->cookie = thread_control;
    }
    rv = bcm_tx(thread_control->unit, txPkt, thread_control);
    if ((thread_control->flags & FLAG_TX_SYNC) != 0) {
        bcm_pkt_free(thread_control->unit, txPkt);
    }
    return rv;
}

static bcm_error_t
tx_setup(struct blast_tx_control_s *thread_control, int *save_lb,
         int *save_discard)
{
    /* Usually we want to loop back */
    if ((thread_control->flags & FLAG_NO_LOOP) == 0) {
        if (BCM_FAILURE(bcm_port_loopback_get
                        (thread_control->unit, thread_control->port, save_lb))) {
            cli_out("Get loopback failed: %s\n",
                    BCM_PORT_NAME(thread_control->unit, thread_control->port));
        }
        if (BCM_FAILURE
            (bcm_port_loopback_set
             (thread_control->unit, thread_control->port, BCM_PORT_LOOPBACK_MAC))) {
            cli_out("Set loopback failed: %s\n",
                    BCM_PORT_NAME(thread_control->unit, thread_control->port));
        }
    }

    /*
     * We don't need to do Rx, just discard looped packets.
     */
    if (BCM_FAILURE((bcm_port_discard_get
                     (thread_control->unit, thread_control->port, save_discard)))) {
        cli_out("Get discard failed: %s\n",
                BCM_PORT_NAME(thread_control->unit, thread_control->port));
    }
    if (BCM_FAILURE
        (bcm_port_discard_set
         (thread_control->unit, thread_control->port, BCM_PORT_DISCARD_ALL))) {
        cli_out("Set discard failed: %s\n",
                BCM_PORT_NAME(thread_control->unit, thread_control->port));
    }

    if (BCM_FAILURE
        (bcm_cosq_config_get(thread_control->unit, &thread_control->num_cos))) {
        thread_control->num_cos = 4;
    }

    if ((thread_control->flags & FLAG_UNICAST) != 0) {
        bcm_error_t         rv;
        bcm_l2_addr_t       l2addr;

        bcm_l2_addr_t_init(&l2addr, thread_control->l2header.mac_dst,
                           thread_control->l2header.vlan);
        l2addr.flags = BCM_L2_STATIC;
        l2addr.port = thread_control->port;
        if (BCM_FAILURE(rv = bcm_l2_addr_add(thread_control->unit, &l2addr))) {
            cli_out("Failed to add L2 address [%s]\n", bcm_errmsg(rv));
        }
    }
    return BCM_E_NONE;
}

static bcm_error_t
tx_cleanup(struct blast_tx_control_s *thread_control)
{
    /* Restore discard state */
    if (BCM_FAILURE(bcm_port_discard_set
                    (thread_control->unit, thread_control->port, thread_control->save_discard))) {
        cli_out("Set discard failed: %s\n",
                BCM_PORT_NAME(thread_control->unit, thread_control->port));
    }
    /* Restore loopback state */
    if ((thread_control->flags & FLAG_NO_LOOP) == 0) {
        if (BCM_FAILURE
            (bcm_port_loopback_set
             (thread_control->unit, thread_control->port, thread_control->save_lb))) {
            cli_out("Set loopback failed: %s\n",
                    BCM_PORT_NAME(thread_control->unit, thread_control->port));
        }
    }
    return BCM_E_NONE;
}
static int time_over = 0;
/* Keep a static list of thread control structures */
static struct blast_tx_control_s thread_list[MAX_THREADS];

static void
simple_tx_thread(void *param)
{
    struct blast_tx_control_s *thread_control =
      (struct blast_tx_control_s *) param;
    const int           size =
      thread_control->packet_size < 68 ? 68 : thread_control->packet_size;
    const unsigned long long done_time = (unsigned long long)time_secs() + (unsigned long long)thread_control->test_time;
    bcm_error_t         rv;
    char                buffer1[16];    /* For time */
    char                buffer2[16];    /* For time */
    char                buffer3[16];    /* For time */
    double              byte_rate;
    double              packet_rate;

    cli_out("Simple TX thread %8s started on port %s\n", thread_control->name,
            BCM_PORT_NAME(thread_control->unit, thread_control->port));

    thread_control->last_packet = 0;
    thread_control->start_time = time_usecs();
    /* Set our active bit */
    thread_control->flags |= FLAG_ACTIVE;
    /* Main thread loop, keep sending until told to stop by main thread */
    while (!time_over) {
        if (BCM_FAILURE(rv = simple_tx(thread_control))) {
            goto exit;
        }
        /* Update statistics */
        thread_control->tx_packets++;
        thread_control->tx_bytes += size;
        if (thread_control->tx_packets % 3000 == 0) {
            sal_usleep(100);
        }
        
        if ((thread_control->flags & FLAG_NO_WAIT)
            && (time_secs() >= done_time)) {
            cli_out("break thread %d\r\n", thread_control->id);
            break;
        }
    }
    /* Record elapsed time */
    thread_control->stop_time = time_usecs();
    thread_control->run_time = thread_control->stop_time - thread_control->start_time;

    if ((thread_control->flags & FLAG_TX_SYNC) == 0) {
        const int           max_iterations = 5000;
        int                 idx2;

        /*
         * Wait for TX DMA to complete in asynchronous mode.
         */
        for (idx2 = 0; idx2 < max_iterations; idx2++) {
            const unsigned long long pending = thread_control->tx_packets -
              thread_control->callback_count;

            if (pending == 0 || (thread_control->callback_count > thread_control->tx_packets)) {
                break;
            }
            sal_usleep(200000);
        }
        if (idx2 == max_iterations) {
            cli_out("Internal Error: Dropped callbacks (TX packets > TX callbacks)\n");
        }
    }

    sal_mutex_take(print_mutex, sal_mutex_FOREVER);
    cli_out("Simple Tx Thread Exit: Thread ID: %d\n", thread_control->id);

    packet_rate =
      ((double) thread_control->tx_packets / (double) thread_control->run_time) *
      1000000.0;
    byte_rate =
      ((double) thread_control->tx_bytes / (double) thread_control->run_time) * 1000.0;
    thread_control->totals->accumulated_packet_rate += packet_rate;
    thread_control->totals->accumulated_byte_rate += byte_rate;
    thread_control->totals->total_packets += thread_control->tx_packets;
    thread_control->totals->total_bytes += thread_control->tx_bytes;
    thread_control->totals->accumulated_tx_latency +=
      thread_control->accumulated_tx_latency;
    if (thread_control->totals->max_tx_latency < thread_control->max_tx_latency) {
        thread_control->totals->max_tx_latency = thread_control->max_tx_latency;
    }
    thread_control->totals->thread_count++;

    cli_out("  Run Time: %s; Test Port: %s;\n",
            blast_time_string(thread_control->run_time, buffer1, sizeof(buffer1)),
            BCM_PORT_NAME(thread_control->unit, thread_control->port));
    cli_out("  %5llu packets; %8llu Bytes;\n", thread_control->tx_packets,
            thread_control->tx_bytes);
    cli_out("  %.1f Packets/sec; %.1f KBytes/sec;\n", packet_rate, byte_rate);
    if (thread_control->stall_count > 0) {
        cli_out("  %llu Tx Stalls; Tx Stall Time: %s\n", thread_control->stall_count,
                blast_time_string(thread_control->stall_count * TX_STALL_TIME_MSECS *
                                  1000, buffer1, sizeof(buffer1)));
    }
    cli_out("  %s Max TX Latency; %s Average TX Latency\n",
            blast_time_string(thread_control->max_tx_latency, buffer2, sizeof(buffer2)),
            blast_time_string((thread_control->accumulated_tx_latency /
                               thread_control->callback_count), buffer3,
                              sizeof(buffer3)));
    cli_out("----------------------------------------\n");
    sal_mutex_give(print_mutex);
    (*thread_control->thread_running) &= (~((unsigned int)1 << thread_control->id));

exit:
    thread_control->flags &= (~FLAG_ACTIVE);
    thread_control = NULL;
    sal_thread_exit(0);
}

/* Main Tx thread, multiple tx threads can run at the same time. */
static void
tx_thread(void *param)
{
    struct blast_tx_control_s *const thread_control =
      (struct blast_tx_control_s *) param;
    const int           size =
      thread_control->packet_size < 68 ? 68 : thread_control->packet_size;
    const unsigned long done_time =
      (unsigned long) time_secs() + thread_control->test_time;

    bcm_error_t         rv;
    bcm_pkt_t          *txPkt[NUM_PKT_BUFFERS];
    char                buffer1[16];    /* For time */
    char                buffer2[16];    /* For time */
    char                buffer3[16];    /* For time */
    double              packet_rate;
    double              byte_rate;
    int                 blk;
    int                 cos;
    int                 idx;
    int                 packet_len;
    int                 vtag;
    uint32              alloc_flags = BCM_TX_CRC_APPEND;
    uint8              *myData[NUM_PKT_BUFFERS];

    cli_out("TX thread %8s started on port %s\n", thread_control->name,
            BCM_PORT_NAME(thread_control->unit, thread_control->port));

    if ((thread_control->flags & FLAG_UNICAST) != 0) {
        alloc_flags |= BCM_TX_ETHER;
    }
    for (idx = 0; idx < NUM_PKT_BUFFERS; idx++) {
        /*
         * Basic, single block packet allocation, allocate a packet with DMA-able
         * data buffer. Allocate a buffer for CRC and regenerate CRC.
         */
        if (BCM_FAILURE
            (rv =
             bcm_pkt_alloc(thread_control->unit, size, alloc_flags, &txPkt[idx]))) {
            if (rv == BCM_E_MEMORY) {
                cli_out("Out of memory for packet allocation\n");
            }
            return;
        }
        /*
         * By default, if the link is down, bcm_tx() won't transmit because
         * those packets will just get stuck in the packet buffer.
         */
        if ((thread_control->flags & FLAG_FORCE_TX) != 0) {
            txPkt[idx]->flags |= BCM_TX_LINKDOWN_TRANSMIT;
        }

        /* Create a VLAN tag from VLAN and packet priority. */
        vtag =
          (thread_control->l2header.vlan & 0xFFF) |
          ((thread_control->l2header.cfi & 0x1) << 11) |
          ((thread_control->l2header.vpri & 0x7) << 13);

        /* Select a COS based on packet priority */
        cos = thread_control->l2header.vpri / (8 / thread_control->num_cos);

        /*
         * Set up the addresses and other fields in the L2 header.
         */
        BCM_PKT_HDR_SMAC_SET(txPkt[idx], thread_control->l2header.mac_src);
        BCM_PKT_HDR_DMAC_SET(txPkt[idx], thread_control->l2header.mac_dst);
        if ((thread_control->flags & FLAG_UNICAST) == 0) {
            BCM_PBMP_PORT_SET(txPkt[idx]->tx_pbmp, thread_control->port);
        }
        BCM_PKT_HDR_TAGGED_LEN_SET(txPkt[idx], size);
        BCM_PKT_HDR_TPID_SET(txPkt[idx], thread_control->l2header.tpid);
        BCM_PKT_HDR_VTAG_CONTROL_SET(txPkt[idx], vtag);

        if ((BCM_E_NONE ==
             bcm_pkt_byte_index(txPkt[idx], BCM_IEEE_HDR_BYTES, &packet_len, &blk,
                                &myData[idx]))
            && (myData[idx] != 0)) {
            uint8              *ptr = myData[idx];
            int                 i;

            for (i = 0; i < 4; i++) {
                ptr[7 - i] = 0; /* Bytes 7..4 are 0 */
            }
            /* Embed the thread ID in the header */
            ptr[11 - i] = thread_control->id & 0xFF;
            for (i = 12; i < packet_len; i++) {
                ptr[i] = i & 0xFF;
            }
        }
        txPkt[idx]->vlan = vtag;        /* 802.1q VID or VSI or VPN. */
        txPkt[idx]->cos = cos;  /* Select a cos queue */
        txPkt[idx]->prio_int = thread_control->l2header.vpri;   /* Set priority */

        if ((thread_control->flags & (FLAG_TX_SYNC | FLAG_TX_ARRAY)) == 0) {
            /*
             * Tx Asynchronous mode, set up packet free callback
             */
            txPkt[idx]->call_back = my_async_callback;
            txPkt[idx]->cookie = thread_control;
        }
    }

    thread_control->last_packet = 0;
    thread_control->start_time = time_usecs();
    /* Set our active bit */
    thread_control->flags |= FLAG_ACTIVE;
    /* Main thread loop, keep sending until told to stop by main thread */
    if ((thread_control->flags & FLAG_TX_ARRAY) == 0) {
        while (*(thread_control->thread_running) & (1 << thread_control->id)) {
            const int           pkt_slot =
              (int) thread_control->tx_packets & (NUM_PKT_BUFFERS - 1);
            const sal_usecs_t   tx_time = sal_time_usecs();
            bcm_error_t         rv;
            int                 i;
            volatile uint8     *ptr = myData[pkt_slot];
            int                 loops;

            for (loops = 0; (ptr[8] != 0) && (loops < TX_STALL_LOOP_COUNT); loops++) {
                /* Wait for buffer clean up */
                sal_usleep(TX_STALL_TIME_MSECS * 1000);
                thread_control->stall_count++;
            }
            if (loops == TX_STALL_LOOP_COUNT) {
                cli_out("Packet %llu still busy after %d msecs\n",
                        thread_control->tx_packets,
                        TX_STALL_LOOP_COUNT * TX_STALL_TIME_MSECS);
                break;
            }
            /* Embed the packet count in the header */
            for (i = 0; i < 4; i++) {
                ptr[3 - i] = (thread_control->tx_packets >> (i * 8)) & 0xFF;
            }
            for (i = 0; i < 4; i++) {
                ptr[7 - i] = (tx_time >> (i * 8)) & 0xFF;
            }

            if ((thread_control->flags & FLAG_TX_SYNC) == 0) {
                ptr[8] = 0xFF;
            }

            /* Update statistics */
            thread_control->tx_packets++;
            thread_control->tx_bytes += size;

            /* Send the next packet */
            if (BCM_FAILURE
                (rv = bcm_tx(thread_control->unit, txPkt[pkt_slot], thread_control))) {
                /* Failed tx packets don't get counted */
                thread_control->tx_packets--;
                thread_control->tx_bytes -= size;
                if (rv == BCM_E_MEMORY) {
                    cli_out("TX DV allocation error\n");
                    break;
                } else {
                    cli_out("TX Failed: %s\n", bcm_errmsg(rv));
                    break;
                }
            }
            if ((thread_control->flags & FLAG_NO_WAIT)
                && ((unsigned long) time_secs() >= done_time)) {
                break;
            }
        }
    } else {
        const int           total_bytes = size * MAX_ARRAY_MODE_BUFFERS;
        unsigned long int   call_count = 0;

        while (*(thread_control->thread_running) & (1 << thread_control->id)) {
            const int           pkt_slot =
              (call_count * MAX_ARRAY_MODE_BUFFERS) % NUM_PKT_BUFFERS;
            bcm_pkt_t         **const nextPktBuf = &txPkt[pkt_slot];
            bcm_error_t         rv;
            int                 i;
            volatile uint8     *ptr = myData[pkt_slot];
            int                 loops;

            for (loops = 0; (ptr[4] != 0) && (loops < TX_STALL_LOOP_COUNT); loops++) {
                /* Wait for buffer clean up */
                sal_usleep(TX_STALL_TIME_MSECS * 1000);
                thread_control->stall_count++;
            }
            if (loops == TX_STALL_LOOP_COUNT) {
                cli_out("Packet %llu still busy after %d msecs\n",
                        thread_control->tx_packets,
                        TX_STALL_LOOP_COUNT * TX_STALL_TIME_MSECS);
                break;
            }
            /* Embed the packet count in the header */
            for (i = 0; i < 4; i++) {
                ptr[3 - i] = (thread_control->tx_packets >> (i * 8)) & 0xFF;
            }
            if ((thread_control->flags & FLAG_TX_SYNC) == 0) {
                ptr[4] = 0xFF;
            }

            /* Update statistics */
            thread_control->tx_packets += MAX_ARRAY_MODE_BUFFERS;
            thread_control->tx_bytes += total_bytes;

            if (BCM_FAILURE
                (rv =
                 bcm_tx_array(thread_control->unit, nextPktBuf, MAX_ARRAY_MODE_BUFFERS,
                              ((thread_control->flags & FLAG_TX_SYNC) ==
                               0) ? my_async_callback : NULL, thread_control))) {
                /* Failed tx packets don't get counted */
                thread_control->tx_packets -= MAX_ARRAY_MODE_BUFFERS;
                thread_control->tx_bytes -= total_bytes;
                if (rv == BCM_E_MEMORY) {
                    cli_out("TX DV allocation error\n");
                    break;
                } else {
                    cli_out("TX Failed: %s\n", bcm_errmsg(rv));
                    break;
                }
            }
            call_count++;
            if ((thread_control->flags & FLAG_NO_WAIT)
                && ((unsigned long) time_secs() >= done_time)) {
                break;
            }
        }
    }

    /* Record elapsed time */
    thread_control->stop_time = time_usecs();
    thread_control->run_time = thread_control->stop_time - thread_control->start_time;

    if ((thread_control->flags & FLAG_TX_SYNC) == 0) {
        const int           max_iterations = 2500;
        int                 idx2;

        /*
         * Wait for TX DMA to complete in asynchronous mode.
         */
        for (idx2 = 0; idx2 < max_iterations; idx2++) {
            const unsigned long long pending = thread_control->tx_packets -
              thread_control->callback_count;

            if (pending == 0) {
                break;
            }
            sal_usleep(200000);
        }
        if (idx2 == max_iterations) {
            cli_out("Internal Error: Dropped callbacks (TX packets > TX callbacks)\n");
        }
    }

    sal_mutex_take(print_mutex, 5000000);
    cli_out("Tx Thread Exit: Thread ID: %d\n", thread_control->id);

    packet_rate =
      ((double) thread_control->tx_packets / (double) thread_control->run_time) *
      1000000.0;
    byte_rate =
      ((double) thread_control->tx_bytes / (double) thread_control->run_time) * 1000.0;
    thread_control->totals->accumulated_packet_rate += packet_rate;
    thread_control->totals->accumulated_byte_rate += byte_rate;
    thread_control->totals->total_packets += thread_control->tx_packets;
    thread_control->totals->total_bytes += thread_control->tx_bytes;
    thread_control->totals->thread_count++;

    cli_out("  Run Time: %s; Test Port: %s;\n",
            blast_time_string(thread_control->run_time, buffer1, sizeof(buffer1)),
            BCM_PORT_NAME(thread_control->unit, thread_control->port));
    cli_out("  %5llu packets; %8llu Bytes;\n", thread_control->tx_packets,
            thread_control->tx_bytes);
    cli_out("  %.1f Packets/sec; %.1f KBytes/sec;\n", packet_rate, byte_rate);
    if (thread_control->stall_count > 0) {
        cli_out("  %llu Tx Stalls; Tx Stall Time: %s\n", thread_control->stall_count,
                blast_time_string(thread_control->stall_count * TX_STALL_TIME_MSECS *
                                  1000, buffer1, sizeof(buffer1)));
    }
    cli_out("   %s Max TX Latency; %s Average TX Latency\n",
            blast_time_string(thread_control->max_tx_latency, buffer2, sizeof(buffer2)),
            blast_time_string((thread_control->accumulated_tx_latency /
                               thread_control->callback_count), buffer3,
                              sizeof(buffer3)));
    cli_out("----------------------------------------\n");
    sal_mutex_give(print_mutex);
    for (idx = 0; idx < NUM_PKT_BUFFERS; idx++) {
        /* Free the packets */
        bcm_pkt_free(thread_control->unit, txPkt[idx]);
    }

    /*
     * Reset the thread control slot
     */
    sal_memset(thread_control, 0, sizeof(struct blast_tx_control_s));
}

int debug_print;

/* Fire off the next TX thread */
static              bcm_error_t
blast_tx(int unit,
         struct blast_tx_control_s *thread_list,
         struct port_list_s *port_list,
         struct blast_tx_totals_s *totals,
         const int count,
         const int size,
         const int test_time, struct l2header_s *l2header, unsigned int flags,
         unsigned int *running)
{
    const int           thread_priority = 70;
    int                 thread_count;
    int                 thread_idx;
    int                 idx;
    int                 thread_slots[MAX_THREADS];
    int                 save_lb = 0;
    int                 save_discard = 0;
    bcm_error_t         rv;

    if ((test_time != 0) && ((flags & FLAG_NO_WAIT) == 0)) {
        flags |= FLAG_WAIT;
    }

    time_over = 0;

    /*
     * Add new threads to the current thread list
     */
    for (thread_count = 0; thread_count < count; thread_count++) {

        /*
         * Find a slot in the thread list for next thread
         */
        for (thread_idx = 0;
             (thread_idx < MAX_THREADS) && (thread_list[thread_idx].l2header.vlan != 0);
             thread_idx++);
        if (thread_idx == MAX_THREADS) {
            cli_out("Too many active threads\n");
            break;
        }
        thread_slots[thread_count] = thread_idx;
        thread_list[thread_idx].unit = unit;
        thread_list[thread_idx].port = port_list->list[port_list->current];
        thread_list[thread_idx].packet_size = size;
        thread_list[thread_idx].flags = flags;
        sal_memcpy(&thread_list[thread_idx].l2header, l2header,
                   sizeof(struct l2header_s));
        sal_sprintf(thread_list[thread_idx].name, "btx_%03d", thread_idx);
        thread_list[thread_idx].thread_running = running;
        thread_list[thread_idx].id = thread_idx;
        thread_list[thread_idx].totals = totals;
        thread_list[thread_idx].test_time = test_time;
        if (++port_list->current == port_list->count) {
            port_list->current = 0;
        }
        
        rv = tx_setup(&thread_list[thread_idx], &save_lb, &save_discard);
        if (BCM_FAILURE(rv)) {
            cli_out("TX set up failed: %s\n", bcm_errmsg(rv));
        } else {
            thread_list[thread_idx].save_lb = save_lb;
            thread_list[thread_idx].save_discard = save_discard;            
        }
    }
    /*
     * Start the threads
     */
    for (idx = 0; idx < thread_count; idx++) {
        int                 thread_slot = thread_slots[idx];
        /*
         * Mark this thread as "running"
         */
        *running |= 1 << thread_list[thread_slot].id;
        /*
         * Start Tx thread.
         */
        if (thread_list[thread_slot].flags & FLAG_SIMPLE_TX) {
            thread_list[thread_slot].tx_tid =
              sal_thread_create(thread_list[thread_slot].name, SAL_THREAD_STKSZ,
                                thread_priority, simple_tx_thread,
                                &thread_list[thread_slot]);
        } else {
            thread_list[thread_slot].tx_tid =
              sal_thread_create(thread_list[thread_slot].name, SAL_THREAD_STKSZ,
                                thread_priority, tx_thread, &thread_list[thread_slot]);
        }
    }
    cli_out("Started %d threads\n", thread_count);
    if (flags & FLAG_WAIT) {
        char                buffer1[16];        /* For time */
        char                buffer2[16];        /* For time */
        int                 active_count;
        int                 idx1;
        int                 idx2;

        /* Here's when we need to stop */
        long                done_time = (long) time_secs() + test_time;
        long                remaining_time;
        /*
         * In WAIT mode, do not return control to diagnostic shell until
         * all threads have completed.
         * */
        do {
            sal_usleep(20000);  /* 20 msecs of sleep */
            remaining_time = done_time - (long) time_secs();
        } while ((*running != 0) && (remaining_time > 0));
        time_over = 1;
        cli_out("Tx test done\n");
        /*
         * Wait for threads to stop (no pthread_join)
         */
        for (idx1 = 0; idx1 < 500; idx1++) {
            active_count = 0;
            for (idx2 = 0; idx2 < thread_count; idx2++) {
                int                 thread_slot = thread_slots[idx2];
                if ((*thread_list[thread_slot].thread_running) & (1 << thread_slot)) {
                    active_count++;
                } else {
                    (void)tx_cleanup(&thread_list[thread_slot]);
                }
            }
            if (active_count == 0) {
                break;
            }
            sal_usleep(50000);  /* 50 msecs of sleep */
        }
                
        cli_out("---------------\n");
        cli_out("Total Threads       : %8d\n", totals->thread_count);
        cli_out("Total Packets       : %8llu\n", totals->total_packets);
        cli_out("Total Bytes         : %8llu\n", totals->total_bytes);
        cli_out("Combined Packet Rate: %8.1f packets/sec\n",
                totals->accumulated_packet_rate);
        cli_out("Combined Byte Rate  : %8.1f KBytes/sec\n",
                totals->accumulated_byte_rate);
        cli_out("Average Packet Rate : %8.1f packets/sec\n",
                totals->accumulated_packet_rate / (double) totals->thread_count);
        cli_out("Average Byte Rate   : %8.1f KBytes/sec\n",
                totals->accumulated_byte_rate / (double) totals->thread_count);
        cli_out("Max TX Latency      : %13s\n",
                blast_time_string(totals->max_tx_latency, buffer1, sizeof(buffer1)));
        cli_out("Average TX Latency  : %13s\n",
                blast_time_string((totals->accumulated_tx_latency /
                                   totals->total_packets), buffer2, sizeof(buffer2)));
        cli_out("---------------\n");
        sal_memset(totals, 0, sizeof(*totals));
    }
    return BCM_E_NONE;
}

/* Dump current statistics for all Tx threads */
static void
thread_status(struct blast_tx_control_s *thread_list)
{
    int                 idx;
    unsigned long long  total_tx;
    int                 thread_count;

    total_tx = 0;
    thread_count = 0;
    for (idx = 0; idx < MAX_THREADS; idx++) {
        struct blast_tx_control_s *thread_info = &thread_list[idx];
        if (thread_info->packet_size) {
            total_tx += thread_info->tx_packets;
            thread_count++;
        }
    }

    cli_out("Total Threads: %2d; Total Packets: %llu\n", thread_count, total_tx);
    return;
}

static void
reset_thread_stats(struct blast_tx_control_s *thread_list)
{
    int                 idx;
    for (idx = 0; idx < MAX_THREADS; idx++) {
        struct blast_tx_control_s *thread_info = &thread_list[idx];
        if (thread_info->packet_size) {
            thread_info->tx_packets = 0;
            thread_info->start_time = time_usecs();
        }
    }
}

typedef struct blast_tx_test_s {
    int pkt_size;
    int tcount;
    int test_time;
    int flags;
    struct l2header_s l2header;
} blast_tx_test_t;

static blast_tx_test_t blast_tx_test_param[SOC_MAX_NUM_DEVICES];

int
blast_tx_init(int unit, args_t *arg, void **p)
{
    blast_tx_test_t    *blast_params;
    parse_table_t        pt;
    char               *subcmd = NULL;

    sal_memset(&blast_tx_test_param[unit], 0, sizeof(blast_tx_test_t));

    blast_params = &blast_tx_test_param[unit];
    parse_table_init(unit, &pt);
    
    print_mutex = sal_mutex_create("Blast Print Mutex");


    if ((subcmd = ARG_GET(arg)) == NULL) {
        /* With no args, just print usage. */
        return -1;
    }
    
    if (!sal_strcasecmp(subcmd, "status")){
        char               *option;

        blast_params->flags |= FLAG_TX_STATUS;
        while ((option = ARG_GET(arg)) != NULL) {
            if (sal_strcmp(option, "clear") == 0) {
                blast_params->flags |= FLAG_STATUS_CLEAR;
            } else {
                cli_out("Invalid status option: %s [ignored]\n", option);
            }
        }
    } else {
        /* Start another TX thread */
        parse_table_t       pt;
        int                 array = 0;
        int                 nolink = 0;
        int                 nowait = 0;
        int                 sync = 0;
        int                 unicast = 0;
        uint32              help_status_default = 0;
        uint32              help_status=0;
        struct l2header_s   l2header = {
            {0x90, 0x90, 0x90, 0x00, 0xF1, 0x0},
            {0x80, 0x80, 0x80, 0x99, 0xF1, 0x0},
            DEFAULT_ETHTYPE, DEFAULT_VLAN, DEFAULT_PKT_PRI, DEFAULT_CFI, DEFAULT_TPID
        };

        blast_params->pkt_size = DEFAULT_PACKET_SIZE;
        blast_params->test_time = DEFAULT_TEST_TIME;
        memcpy(&blast_params->l2header, &l2header, sizeof(struct l2header_s));
        parse_table_init(unit, &pt);
        /* Egress packet size */
        parse_table_add(&pt, "Size", PQ_DFL | PQ_INT, 0, &blast_params->pkt_size, 0);
        /* Number of threads to create */
        parse_table_add(&pt, "THreads", PQ_DFL | PQ_INT, 0, &blast_params->tcount, 0);
        /* Number of seconds to sent traffic */
        parse_table_add(&pt, "TestTime", PQ_DFL | PQ_INT, 0, &blast_params->test_time, 0);
        /* VLAN ID */
        parse_table_add(&pt, "VLan", PQ_DFL | PQ_INT, 0, &blast_params->l2header.vlan, 0);
        /* Do synchronous TX, don't do an asynchronous callback */
        parse_table_add(&pt, "SYNC", PQ_DFL | PQ_BOOL | PQ_NO_EQ_OPT, 0, &sync, NULL);
        /* Do "array mode" TX, Send the entire packet list at once */
        parse_table_add(&pt, "ARray", PQ_DFL | PQ_BOOL | PQ_NO_EQ_OPT, 0, &array, NULL);
        /* Set up L2 table, don't use unicast */
        parse_table_add(&pt, "UNIcast", PQ_DFL | PQ_BOOL | PQ_NO_EQ_OPT, 0, &unicast,
                        NULL);
        /* Don't for link up via loop back, assumes front panel port has a link partner */
        parse_table_add(&pt, "NoLink", PQ_DFL | PQ_BOOL | PQ_NO_EQ_OPT, 0, &nolink,
                        NULL);
        /* Don't wait for tx threads to exit */
        parse_table_add(&pt, "NoWait", PQ_DFL | PQ_BOOL | PQ_NO_EQ_OPT, 0, &nowait,
                        NULL);
        parse_table_add(&pt, "DstMac", PQ_DFL | PQ_MAC, 0, (void *) blast_params->l2header.mac_dst,
                        NULL);
        parse_table_add(&pt, "SrcMac", PQ_DFL | PQ_MAC, 0, (void *) blast_params->l2header.mac_src,
                        NULL);
        parse_table_add(&pt, "Help", PQ_DFL|PQ_INT, &help_status_default, &help_status, NULL);

        if (help_status == 1) {
            cli_out("%s\n", blast_tx_usage);
            return 0;
        }
        /* Parse command line parameters */
        if ((parse_arg_eq(arg, &pt)) < 0 || (ARG_CNT(arg) > 0)) {
            cli_out("%s: ERROR: Unknown option: %s\n", ARG_CMD(arg), ARG_CUR(arg));
            parse_arg_eq_done(&pt);
            return -1;
        }

        /* Set various control flags for threads */
        blast_params->flags |= FLAG_TX_START;
        if (array) {
            blast_params->flags |= FLAG_TX_ARRAY;
        }
        if (sync) {
            blast_params->flags |= FLAG_TX_SYNC;
        }
        if (nolink) {
            blast_params->flags |= FLAG_NO_LOOP;
        }
        if (nowait) {
            blast_params->flags |= FLAG_NO_WAIT;
        }
        if (unicast) {
            blast_params->flags |= FLAG_UNICAST;
        }
        blast_params->flags |= FLAG_SIMPLE_TX;

        /* Parameters checks */
        if ((blast_params->tcount < 1) || (blast_params->tcount > MAX_THREADS)) {
            blast_params->tcount = DEFAULT_THREADS;
        }
        if ((blast_params->l2header.vlan < 1) || (blast_params->l2header.vlan > 4095)) {
            cli_out("Invalid VLAN: %d; using %d\n", blast_params->l2header.vlan, DEFAULT_VLAN);
            blast_params->l2header.vlan = DEFAULT_VLAN;
        }
    }

    *p = blast_params;
    parse_arg_eq_done(&pt);
    return 0;
}

/* Accumulated totals */
static struct blast_tx_totals_s totals;

/* Main diag shell entry point for "blast_tx" command. */
int
blast_tx_test(int unit, args_t *arg, void *p)
{
    /* Need to set things up first time through */
    /* Running mask, one bit per thread */
    static unsigned int running = 0;
    /* Keep a list of ports eligible for use by this test */
    struct port_list_s port_list;
    /* Initialize data structures first time through */
    bcm_port_config_t   portConfig;
    blast_tx_test_t *blast_params = p;

    /*
     * Set up list of ports that can be used for this test.
     * List defaults to ALL Ethernet ports.
     */
    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &portConfig));
    get_port_list(unit, &port_list, portConfig.e);

    /* Clear all thread control data structures */
    sal_memset(&thread_list, 0, sizeof(thread_list));
    sal_memset(&totals, 0, sizeof(totals));

    if (blast_params->flags & FLAG_TX_START) {
        /* Start TX threads */
        if (BCM_FAILURE(blast_tx
                        (unit, thread_list, &port_list, &totals,
                         blast_params->tcount, blast_params->pkt_size,
                         blast_params->test_time, &blast_params->l2header,
                         blast_params->flags, &running))) {
            return BCM_E_FAIL;
        }
    }  else if (blast_params->flags & FLAG_TX_STATUS) {
        thread_status(thread_list);
        if (blast_params->flags & FLAG_STATUS_CLEAR) {
            reset_thread_stats(thread_list);
        }
    } 

    return CMD_OK;
}

int blast_tx_done(int unit, void *p) 
{
    sal_memset(&thread_list, 0, sizeof(thread_list));
    sal_memset(&totals, 0, sizeof(totals));

    if (print_mutex != NULL) {
        sal_mutex_destroy(print_mutex);
    }
    return 0;
}
