/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : BCM RX
 *
 *  Usage    : BCM.0> cint bcm_rx_cpu_cosq_mapping.c
 *
 *  config   : bcm_rx_config.bcm
 *
 *  Log file : bcm_rx_cpu_cosq_mapping_log.txt
 *
 *  Test Topology :
 *
 *                   +------------------------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |  
 *  +----------------+          SWITCH              +-----------------+
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   |                              |
 *                   +------------------------------+
 *
 *
 *  Summary:
 *  ========
 *    This CINT script demonstrate how to map cpu cos queues to RX DMA channels 
 *    BCM APIs.
 *    This exampls hsows how application can register RX callback function
 *    and process the packets received by CPU port.
 *
 *    Detailed steps done in the CINT script:
 *    =====================================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    =====================================
 *      a) Stop packet watcher application
 *      b) Start packet watchher application with all 7 RX DMA channels
 *  
 *    2) Step2 - Configuration (Done in rx_cpu_cosq_setup())
 *    ==================================================================
 *      a) Build cpu cosq mapping by calling bcm_cosq_gport_traverse() API 
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Remap cpu cosq to different RX DMA channels as below and 
 *         dump cmic_cmc0_pktdma_ch(x)_cos_ctrl_rx_[0..1] register
 *         Map 0-1 cos to RX DMA channel 0
 *         Map 2-3 cos to RX DMA channel 1
 *         Map 4-5 cos to RX DMA channel 2
 *         Map 6-7 cos to RX DMA channel 3
 *         Map 8-9 cos to RX DMA channel 4
 *         Map 10-11 cos to RX DMA channel 5
 *         Map 12-13 cos to RX DMA channel 6
 *      b) Expected Result:
 *         ================
 *         RX DMA channel cos control registers values should match as per 
 *         the above mapping done in step# a)
 */


/*
 * Tomahawk3 Rx Queue Channel Configuration
 *
 * The mechanism for configuring Rx Queue channels on Tomahawk3
 * (BCM56980) differs from previous devices. Previous devices used
 * bcm_tx_queue_channel_set(). This API does not apply to Tomahawk3. For
 * Tomahawk3 devices, the user needs to use a combination of cosq attach/detach
 * operations on L0 gports. On Tomahawk3, the L0 nodes are directly wired
 * to different DMA channels. The L0 gports can be obtained with the
 * bcm_cosq_gport_traverse() API. The L0 gport values do not change so
 * they only need to be obtained once. Once the L0 gports are known, the
 * Rx channels can be reconfigured as desired. The following CINT sample
 * demonstrates how to obtain the L0 gports and use them to reconfigure the
 * Rx channel configurations on CMIC 0 (CPU).
 * 
 * bcm_error_t
 * tomahawk_rx_queue_channel_set(int unit,
 *                               cpu_queues_t * cpu_queues,
 *                               bcm_cos_queue_t queue_id,
 *                               bcm_rx_chan_t chan_id);
 *
 * The L0 nodes are mapped to CMIC Rx/Tx channels as described below. This
 * mapping cannot be changed.
 *
 * CHANNEL MAPPING FOR HOST CPU
 * Rx 0 => L0.0 => CMIC 0 (HOST CPU PCI)/Channel 0 => PCI Rx Channel 0
 * Rx 1 => L0.1 => CMIC 0 (HOST CPU PCI)/Channel 1 => PCI Rx Channel 1
 * Rx 2 => L0.2 => CMIC 0 (HOST CPU PCI)/Channel 2 => PCI Rx Channel 2
 * Rx 3 => L0.3 => CMIC 0 (HOST CPU PCI)/Channel 3 => PCI Tx Channel 3
 * Rx 4 => L0.4 => CMIC 0 (HOST CPU PCI)/Channel 3 => PCI Tx Channel 4 
 * Rx 5 => L0.5 => CMIC 0 (HOST CPU PCI)/Channel 3 => PCI Tx Channel 5
 * Rx 6 => L0.6 => CMIC 0 (HOST CPU PCI)/Channel 3 => PCI Tx Channel 6
 *
 * CMIC channel assignments:
 *
 * CMC0 Host CPU (PCI)
 *     CH0     TX 
 *     CH[1-6] RX 
 */


cint_reset();

/* Structure for each of 48 CPU queues */
typedef struct cpu_queue_t {
    bcm_gport_t gport;
    bcm_gport_t input;
    int         input_cos;
};

/* Structure used to store all gports needed for Rx channel mapping */
typedef struct cpu_queues_t {
    cpu_queue_t cpu_queues[48];
    bcm_gport_t sched_nodes[12];
};

int sched_cosq_idx=0;

/* cpu cosq mapping database */
cpu_queues_t cpu_cosq_map_db;

/*
 * Traverse function for bcm_cosq_gport_traverse().
 *
 * Find and save each of the 48 CPU queue gports.  Find and save each of
 * the 12 L0 (CMIC) gports.
 */
int
traverse_callback(int unit, bcm_gport_t port, int numq, uint32 flags,
                  bcm_gport_t gport, void *user_data)
{
    int rv = BCM_E_NONE;
    bcm_port_t cpu_port = 0;
    bcm_gport_t cpu_gport;  
    cpu_queues_t *cpu_queues = (auto) user_data;
    bcm_gport_t input_port;
    bcm_cos_queue_t cosq;
    bcm_port_gport_get(0, cpu_port, &cpu_gport);

    /* We are looking for CPU queues (multicast cast queues for port 0) */
    if (port != cpu_gport) {
        return BCM_E_NONE;
    }

    if (flags == BCM_COSQ_GPORT_SCHEDULER) {
        cpu_queues->sched_nodes[sched_cosq_idx] = gport;
        sched_cosq_idx++;
    } else if (flags == BCM_COSQ_GPORT_MCAST_QUEUE_GROUP) {
        /*
        * Get input port and cosq using bcm_cosq_gport_attach_get()
        */
        rv = bcm_cosq_gport_attach_get(unit, gport, &input_port, &cosq);
        if(BCM_FAILURE(rv)) {
            printf("\nError in bcm_cosq_gport_attach_get(): %s.\n",bcm_errmsg(rv));
            return rv;
        }
        cpu_queues->cpu_queues[cosq].gport = gport;
        cpu_queues->cpu_queues[cosq].input = input_port;
    }
    return BCM_E_NONE;
}

/*
 * Find all CPU queues using the gport traverse API and save them. The gport
 * values are static and only need to be obtained once.
 */
bcm_error_t
get_tomahawk3_cpu_queues(int unit, cpu_queues_t * cpu_queues)
{
    int cos_idx;
    int input_idx;
    int rv = BCM_E_NONE;
    rv = bcm_cosq_gport_traverse(unit, traverse_callback, cpu_queues);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_cosq_gport_traverse(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    for (cos_idx = 0; cos_idx < 48; cos_idx++) {
        for (input_idx = 0; input_idx < 10; input_idx++) {
            if (cpu_queues->sched_nodes[input_idx] ==
                cpu_queues->cpu_queues[cos_idx].input) {
                cpu_queues->cpu_queues[cos_idx].input_cos = input_idx;
                break;
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Map a CPU queue to PCI Rx channel connection. This routine should be
 * used in place of bcm_rx_queue_channel_set() for Tomahawk3 device.
 */
bcm_error_t
tomahawk3_rx_queue_channel_set(int unit,
                              cpu_queues_t * cpu_queues,
                              bcm_cos_queue_t queue_id, bcm_rx_chan_t chan_id)
{
    int rv = BCM_E_NONE;
    /* Disconnect previous DMA channel */
    rv = bcm_cosq_gport_detach
                        (unit, cpu_queues->cpu_queues[queue_id].gport,
                         cpu_queues->sched_nodes[cpu_queues->
                         cpu_queues[queue_id].input_cos],
                         queue_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_cosq_gport_detach(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    /* Update connection database */
    cpu_queues->cpu_queues[queue_id].input_cos = chan_id;
    cpu_queues->cpu_queues[queue_id].input = cpu_queues->sched_nodes[chan_id];

    /* Attach new DMA channel */
    rv = bcm_cosq_gport_attach
                        (unit, cpu_queues->cpu_queues[queue_id].gport,
                         cpu_queues->sched_nodes[cpu_queues->
                         cpu_queues[queue_id].input_cos],
                         queue_id);
    if(BCM_FAILURE(rv)) {
        printf("\nError in bcm_cosq_gport_attach(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    return rv;
}


/*
 * rx_cpu_cosq_setup()
 * This function will build the cpu cosq map database
*/
int rx_cpu_cosq_setup(int unit)
{
    int rv = BCM_E_NONE;

    /*
     * Call once and save gports, the mapping does not change so these values
     * only need to be obtained once.
     */
    rv = get_tomahawk3_cpu_queues(unit, &cpu_cosq_map_db);
    if(BCM_FAILURE(rv)) {
        printf("\nError in get_tomahawk3_cpu_queues(): %s.\n",bcm_errmsg(rv));
        return rv;
    }

    return rv;    
}

/*
 * test_setup()
 * This function will start Rx thread by launching packet watcher
*/
int test_setup(int unit)
{

    int rv = BCM_E_NONE;
    char        command[128];   /* Buffer for diagnostic shell commands */
    
    sprintf(command, "pw stop");
    bshell(unit, command);
    sprintf(command, "pw start channel 1 2 3 4 5 6 7");
    bshell(unit, command);
    
    return rv;
}

/*
 * Sample code test driver.
 *
 * Demonstrates how to map Rx PCI channels for Tomahawk3.
 * verify the cosq mapping by dumping cmic_cmc0_pktdma_ch1/7_cos_ctrl_rx0/1
*/
bcm_error_t
test_harness(int unit)
{
    const int   max_rx_cos = 14;
    int         cos;
    int         rv = BCM_E_NONE;
    int         cos_idx;
    int         input_idx;
 
    /* Map 0-1 cos to RX DMA channel 0
     * Map 2-3 cos to RX DMA channel 1 
     * Map 4-5 cos to RX DMA channel 2 
     * Map 6-7 cos to RX DMA channel 3 
     * Map 8-9 cos to RX DMA channel 4 
     * Map 10-11 cos to RX DMA channel 5 
     * Map 12-13 cos to RX DMA channel 6 
    */ 
    for (cos = 0; cos < max_rx_cos; cos++) {
        const int   rx_queue_channel = cos / 2; 
        printf("Map CPU COS %d to Rx DMA channel %d\n", cos, rx_queue_channel);
        rv = tomahawk3_rx_queue_channel_set(unit, &cpu_cosq_map_db, cos, rx_queue_channel);
        if(BCM_FAILURE(rv)) {
            printf("Failed to set rx queue channel to %d on Rx cos %d: %s\n",
                   rx_queue_channel, cos, bcm_errmsg(rv));
            return rv;
        }
    }
    
    return BCM_E_NONE;
}


bcm_error_t verify(int unit)
{
    bcm_error_t rv = BCM_E_NONE;
    char command[128];   /* Buffer for diagnostic shell commands */
    int ch=0, offset=0;   
    if (BCM_FAILURE(rv = test_harness(0))) {
        printf("Rx CPU Cosq mapping verification failed: %s\n", bcm_errmsg(rv));
        return rv;
    }

    printf("\nDump CPU cos mapping registers\n");
    printf("==========================================\n");
    /* Dump cmic_cmc0_pktdma_ch[1..7]_cos_ctrl_rx_[0..1] registers */
    for (ch=1; ch<8; ch++) {
        for (offset=0; offset<2; offset++) {
            sprintf(command, "g cmic_cmc0_pktdma_ch%d_cos_ctrl_rx_%d", ch, offset);
            bshell(unit, command);
        }
    }
    return rv;
}

bcm_error_t execute()
{
    bcm_error_t rv;
    int unit =0;
    bshell(unit, "config show; a ; version");
    if (BCM_FAILURE((rv = test_setup(unit)))) {
        printf("test_setup() failed.\n");
        return -1;
    }

    if (BCM_FAILURE((rv = rx_cpu_cosq_setup(unit)))) {
        printf("Rx CPU Cosq Setup Failed\n");
        return -1;
    }

    rv = verify(unit);
    return rv;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
  print execute();
}

