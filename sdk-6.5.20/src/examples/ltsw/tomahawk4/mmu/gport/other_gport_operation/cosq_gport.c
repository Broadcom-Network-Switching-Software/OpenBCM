/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */
/*  Feature  : cosq gport
 *
 *  Usage    : BCM.0> cint cosq_gport.c
 *
 *  config   : cosq_gport_config.bcm
 *
 *  Log file : cosq_gport_log.txt
 *
 *  Test Topology : None
 *
 *  This script can verify below cosq gport APIs:
 *  ========
 *    bcm_cosq_gport_get
 *    bcm_cosq_gport_traverse
 *    bcm_cosq_gport_attach
 *    bcm_cosq_gport_attach_get
 *    bcm_cosq_gport_detach
 *    bcm_cosq_gport_child_get
 *
 *  Summary:
 *  ========
 *  This cint example demonstrate attaching/detaching CPU queue to CPU L0 sch node
 *
 *    Detailed steps done in the CINT script:
 *    =======================================
 *    1) Step1 - Test Setup (Done in test_setup())
 *    ============================================
 *      a) Retrieve all the CPU queues and L0 sch nodes, and save them to local data structure
 *
 *    2) Step2 - Configuration (Done in cpu_queue_2_l0_sch_remap())
 *    ======================================================
 *      a) Remap CPU queues to L0 sch nodes((queue id)% (NUM_SCH_NODES-1)).
 *
 *    3) Step3 - Verification (Done in verify())
 *    ==========================================
 *      a) Verify the result (CPU queues to L2 sch nodes mapping) using 'dump sw cosq'
 *      b) Expected Result:
 *         ================
 *         In 'dump sw cosq' output
 *             - CPU queues were remapped to L0 sch nodes((queue id)% (NUM_SCH_NODES-1)).
 */

cint_reset();

int unit=0;
int rv;
int NUM_CPU_QUEUES = 48;
int NUM_SCH_NODES = 12;
int prev_port = -1;

/* Structure for each of 48 CPU queues */
typedef struct cpu_queue_t {
    bcm_gport_t gport;
    bcm_gport_t input; /* The sch node CPU queue attached to */
    int         input_cos;/* The number of sch node CPU queue attached to */
};

/* Structure used to store all gports */
typedef struct cpu_queues_t {
    cpu_queue_t cpu_queues[NUM_CPU_QUEUES];
    bcm_gport_t sched_nodes[NUM_SCH_NODES];
};

cpu_queues_t cpu_queues;
bcm_error_t test_setup(int unit)
{
    /*
    * Call once and save gports, the mapping does not change so these values
    * only need to be obtained once.
 */
    BCM_IF_ERROR_RETURN(cpu_gport_get(unit, &cpu_queues));

    return BCM_E_NONE;
}

 /*
 * Callback function for bcm_cosq_gport_traverse().
 *
 * Find and save all 48 CPU queue gports and 10 L0 gports.
 */
static bcm_error_t cosq_gport_traverse_callback(int unit,
                                     bcm_gport_t port, int numq, uint32 flags,
                                     bcm_gport_t gport, void *user_data)
{
    const int CPU_PORT = 0;
    cpu_queues_t *cpu_queues = (auto) user_data;

    int         local_port=prev_port;
    int         num_cos_levels;
    uint32      port_flags;
    bcm_gport_t input_port;
    bcm_cos_queue_t cosq;

    /* Only care about CPU queues (multicast queues for port 0) */
    if (BCM_GPORT_IS_UCAST_QUEUE_GROUP(gport)) {
     /* Only care about multicast queues */
     return BCM_E_NONE;
    }

    /* Only parse the local port from multicast queues, as the local port parsed from sch node
      *    can't exactly represent the local port
      */
    if (BCM_GPORT_IS_MCAST_QUEUE_GROUP(gport)) {
     BCM_IF_ERROR_RETURN(bcm_cosq_gport_get(unit, gport, &local_port,
                                            &num_cos_levels, &port_flags));
    }

    if (prev_port != local_port) {
     prev_port = local_port;
     printf("cosq_gport_traverse_callback: local_port = %d\n", local_port);
    }

    if (local_port != CPU_PORT) {
     /* Only care about port 0 (the CPU port) */
     return BCM_E_NONE;
    }

    /* Get input port and cosq using bcm_cosq_gport_attach_get()*/
    BCM_IF_ERROR_RETURN(bcm_cosq_gport_attach_get(unit, gport, &input_port, &cosq));

    if (BCM_GPORT_IS_SCHEDULER(gport)) {
     cpu_queues->sched_nodes[cosq] = gport;
     printf("cpu_queues->sched_nodes[%d]=0x%x\n", cosq, gport);
    } else {
     cpu_queues->cpu_queues[cosq].gport = gport;
     cpu_queues->cpu_queues[cosq].input = input_port;
     printf("cpu_queues[%d](0x%x) was attached to sched_nodes=0x%x\n",
            cosq, gport, input_port);
    }
    return BCM_E_NONE;
}

/*
 * Find all CPU queues and scheduler nodes using the gport traverse API and save them. The gport
 * values are static and only need to be obtained once.
 */
bcm_error_t
cpu_gport_get(int unit, cpu_queues_t * cpu_queues)
{
    int         cos_idx;
    int         input_idx;

    BCM_IF_ERROR_RETURN(
        bcm_cosq_gport_traverse(unit, cosq_gport_traverse_callback, cpu_queues));
    for (cos_idx = 0; cos_idx < NUM_CPU_QUEUES; cos_idx++) {
        for (input_idx = 0; input_idx < NUM_SCH_NODES; input_idx++) {
            if (cpu_queues->sched_nodes[input_idx] ==
                cpu_queues->cpu_queues[cos_idx].input) {
                /*The cos_idx-th CPU queue was attached to the input_idx-th node of sch*/
                cpu_queues->cpu_queues[cos_idx].input_cos = input_idx;
                printf("cpu_gport_get cpu_queues[%d](0x%x) was attached to sched_nodes[%d]=0x%x\n",
                       cos_idx, cpu_queues->cpu_queues[cos_idx].gport, input_idx,
                       cpu_queues->sched_nodes[input_idx]);
                break;
            }
        }
    }

    prev_port = -1;

    return BCM_E_NONE;
}

/*
 * Map a CPU queue to PCI Rx channel connection. bcm_cosq_gport_detach should be
 * used in place of bcm_rx_queue_channel_set() for Tomahawk device.
 */
bcm_error_t
rx_queue_channel_set(int unit,
                              cpu_queues_t * cpu_queues,
                              bcm_cos_queue_t queue_id, bcm_rx_chan_t chan_id)
{
    bcm_gport_t input_port;
    bcm_cos_queue_t cosq;
    bcm_gport_t gport;

    /* Disconnect previous CMC RX channel */
    BCM_IF_ERROR_RETURN(bcm_cosq_gport_detach
                        (unit, cpu_queues->cpu_queues[queue_id].gport,
                         cpu_queues->sched_nodes[cpu_queues->
                                                 cpu_queues[queue_id].input_cos],
                         queue_id));
    printf("rx_queue_channel_set:cpu_queues->cpu_queues[%d].input_cos: %d, chan_id: %d\n",
           queue_id, cpu_queues->cpu_queues[queue_id].input_cos, chan_id);

    /* Update connection database */
    cpu_queues->cpu_queues[queue_id].input_cos = chan_id;
    cpu_queues->cpu_queues[queue_id].input = cpu_queues->sched_nodes[chan_id];

    /* Attach new CMC RX channel */
    BCM_IF_ERROR_RETURN(bcm_cosq_gport_attach
                        (unit, cpu_queues->cpu_queues[queue_id].gport,
                         cpu_queues->sched_nodes[chan_id],
                         queue_id));

    printf("rx_queue_channel_set: Attached the %dth CPU queue 0x%x to the %dth node of L0(0x%x)\n",
            queue_id, cpu_queues->cpu_queues[queue_id].gport, chan_id,
            cpu_queues->sched_nodes[chan_id]);

    BCM_IF_ERROR_RETURN(bcm_cosq_gport_child_get
                        (unit, cpu_queues->sched_nodes[chan_id],
                         queue_id, &input_port));

    printf("rx_queue_channel_set: Attach get: The %dth child of the %dth sch node 0x%x is 0x%x\n",
            queue_id, chan_id, cpu_queues->sched_nodes[chan_id], input_port);

    return BCM_E_NONE;
}

 /* Function to verify the result */
bcm_error_t cpu_queue_2_l0_sch_remap(int unit)
{
    const int   max_rx_cos = NUM_CPU_QUEUES;
    int         cos;
    int         rv = BCM_E_NONE;

    for (cos = 0; cos < max_rx_cos; cos++) {
     /* Pick an arbitrary channel assignment */
     /* Sch node 11 was reservered, queue will be attached to sch node 11 when it was detached. */
     /* So, max value use (NUM_SCH_NODES-1) instead of NUM_SCH_NODES*/
     const int   rx_queue_channel = cos % (NUM_SCH_NODES-1);
     bcm_error_t return_status;

     printf("Map CPU COS %d to rx channel %d\n", cos, rx_queue_channel);
     if (BCM_FAILURE
         (return_status =
         rx_queue_channel_set(unit, &cpu_queues, cos, rx_queue_channel))) {
         printf("Failed to set rx queue channel to %d on Rx cos %d: %s\n",
                rx_queue_channel, cos, bcm_errmsg(return_status));
         rv = return_status;
     }
    }
    return BCM_E_NONE;
}

  /* Function to verify the result */
bcm_error_t verify(int unit)
{
    /*check output for port on which new scheduler profile is attached */
    bshell(unit, "dump sw cosq");
}

bcm_error_t execute()
{
    bcm_error_t rv;
    int unit = 0;

    bshell(unit, "config show; a ; version");

    /* Retrieve all the CPU queues and L0 sch nodes, and save them to local data structure */
    if (BCM_FAILURE((rv = test_setup(unit)))) {
      printf("test_setup() failed.\n");
      return -1;
    }

    /* Remap CPU queues to L0 sch nodes((queue id)% (NUM_SCH_NODES-1))*/
    if (BCM_FAILURE((rv = cpu_queue_2_l0_sch_remap(unit)))) {
       printf("cpu_queue_2_l0_sch_remap() failed.\n");
       return -1;
    }

    /* Verify the result (Check CPU queues to L2 sch nodes mapping) */
    verify(unit);
    return BCM_E_NONE;
}

const char *auto_execute = (ARGC == 1) ? ARGV[0] : "YES";
if (!sal_strcmp(auto_execute, "YES")) {
    print execute();
}

