/*~~~~~~~~~~~~~~~~~~~~~~~~~~Cosq: E2E Scheduling~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_e2e_dual_shaper_not_cl_cir.c
 * Purpose: Example of Dual Shaper Validation
 * 
 * Environment
 *    - Default Egress queue mapping in play.
 *    - Default Egress Port Scheduler hierarchy in play.
 *    - Default Ingress Scheduling Discipline (delay tolerance), queues sizes
 *    - Packet Size 1500 - size should be aligned with credit source settings
 *
 * - Port Scheduler (HR)
 *   -> setup HR scheduling mode (via *_REPLACE flag)
 *   -> setup flow control mapping
 *   - 2 FQ schedulers
 *       -> hookup 1st FQ scheduler to SP0 of HR and 2nd FQ scheduler to SP1 of HR
 *   - Dual Shaper scheduling element, PIR (CL - SP mode) and CIR (FQ) scheduling elements
 *       -> Hookup PIR (CL) scheduler to 1st FQ scheduler and CIR (FQ) scheduler to 2nd FQ Scheduler
 *       -> PIR (CL) scheduler rate set to 100 Mbps
 *       -> CIR (FQ) scheduler rate set to 500 Mbps
 *   - 1 Queue Group / Connector Group (4 elements in bundle)
 *       -> 1st Connector element hooked up to PIR (CL) scheduler
 *       -> 2nd Connector element hooked up to CIR (FQ) scheduler. Slow configuration enabled.
 *
 * - Input
 *   - Queue - Cos 0
 *     Input 400 Mbps
 *   - Queue - Cos 1
 *     Input 100 Mbps
 *
 * - Expected Output
 *   - Queue - Cos 0
 *     Expected Output 400 Mbps
 *     Observed 400 Mbps
 *   - Queue - Cos 1
 *     Expected Output 100 Mbps
 *     Observed 100 Mbps
 * 
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  |                                                                              | 
 *  |                                        |                                     | 
 *  |                                       SPR (500 Mbps)                         | 
 *  |                                        |                                     | 
 *  |                                       \/                                     | 
 *  |                                      +----+                                  | 
 *  |                                     /      \                                 | 
 *  |                                    /   HR   \                                | 
 *  |                                   /          \                               | 
 *  |                                  +------------+                              | 
 *  |                                    /       \                                 | 
 *  |                                   /         \                                | 
 *  |                                 SPR        SPR                               | 
 *  |                             (500 Mbps)  (500 Mbps)                           | 
 *  |                              /                 \                             | 
 *  |                             /                   \                            | 
 *  |                           |/                    \/                           | 
 *  |                       +------+               +------+                        | 
 *  |                      /   FQ   \             /   FQ   \                       | 
 *  |                     +----------+           +----------+                      |
 *  |                          |                       |                           |
 *  |                          |                       |                           |
 *  |                         SPR                     SPR                          |
 *  |                     (100 Mbps)              (500 Mbps)                       |
 *  |                          |                       |                           |
 *  |                          |                       |                           |
 *  |                         \/                      \/                           |
 *  |  +---------------------------------------------------------------+           |
 *  |  |              +-----------------           +------+            |           |
 *  |  |  Dual       / CL (4 SP levels) \         /   FQ   \           |           |
 *  |  |            /                    \       +----------+          |           |
 *  |  | Shaper    / SP0   SP1  SP2   SP3 \            |               |           |
 *  |  |          +------------------------+           |               |           | 
 *  |  +---------------------------------------------------------------+           |
 *  |                         |                        |                           | 
 *  |                         |                        |                           | 
 *  |                        SPR                      SPR                          |
 *  |                     (500 Mbps)               (500 Mbps)                      |
 *  |                         |                        |                           | 
 *  |                         |                        |                           |
 *  |                        \/                       \/                           |
 *  |                     |     |                   |     |                        |
 *  |                     |     |                   |     |                        | 
 *  |                     |     |                   |     |                        |
 *  |                     |     |                   |     |                        |
 *  |                     |     |                   |     |                        |
 *  |                     +-----+                   +-----+    +----------------+  |
 *  |                   Input:400 Mbps         Input:100 Mbps  |      KEY       |  |
 *  |                                                          +----------------+  |
 *  |                                                          |SPR- Rate Shaper|  |
 *  |                                                          |                |  |
 *  |                                                          +----------------+  |
 *  |                     +~~~~~~~~~~~~~~~~~~~~~~~~~~~+                            |
 *  |                     |   Figure 9: Dual Shaper   |                            |      
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 */

/*
 * General Definitions
 */

int C6_PETRA_CARD_NUM_PORTS = 1;

int C6_DEFAULT_E2E_SCHED_FLAGS = (BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_HR_SINGLE_WFQ);

int C6_E2E_INTERFACE_KBITS_SEC_MAX = 500000; /* 500 mbps */
int C6_E2E_INTERFACE_KBITS_SEC_MAX_MARGIN = 20; /* 20 kbps */

int C6_E2E_PORT_KBITS_SEC_MAX = 500000; /* 500mbps */
int C6_E2E_PORT_KBITS_SEC_MAX_MARGIN = 10; /* 10 kbps */

int C6_NBR_FQ_SCHEDULERS = 2;
int C6_SCHED_FLAGS = (BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_FQ);
int C6_SCHEDULER0_MODE = BCM_COSQ_SP0;
int C6_SCHEDULER1_MODE = BCM_COSQ_SP1;
int C6_SCHED_KBITS_SEC_MAX = 500000; /* 500 Mbps */
int C6_SCHED_KBITS_SEC_MAX_MARGIN = 10; /* 10 Kbps */
int C6_SCHED_MAX_BURST = 3000;
int C6_SCHED_MAX_BURST_MARGIN = 75;

int C6_SCHED_DUAL_FLAGS = (BCM_COSQ_GPORT_SCHEDULER |
                           BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE1_4SP |
                           BCM_COSQ_GPORT_SCHEDULER_DUAL_SHAPER);

int C6_SCHED_DUAL_CIR_MODE = BCM_COSQ_SP0;
int C6_SCHED_DUAL_CIR_KBITS_SEC_MAX = 500000; /* 500 Mbps */
int C6_SCHED_DUAL_CIR_KBITS_SEC_MAX_MARGIN = 10; /* 10 Kbps */

int C6_SCHED_DUAL_EIR_MODE = BCM_COSQ_SP0;
int C6_SCHED_DUAL_EIR_KBITS_SEC_MAX = 100000; /* 100 Mbps */
int C6_SCHED_DUAL_EIR_KBITS_SEC_MAX_MARGIN = 10; /* 10 Kbps */

/* handles */
int c6_unit = 0;
int c6_my_modid = 0;

bcm_gport_t c6_gport_ucast_voq_connector_group[32]; /* connector group */
bcm_gport_t c6_gport_ucast_queue_group[32];         /* queue group */
bcm_gport_t c6_gport_ucast_scheduler[32];           /* cl dual scheduler */
bcm_gport_t c6_gport_ucast_scheduler_cir[32];       /* cl - cir scheduler */
bcm_gport_t c6_gport_ucast_scheduler_pir[32];       /* cl - pir scheduler */
bcm_gport_t c6_gport_ucast_scheduler_fq0[32];
bcm_gport_t c6_gport_ucast_scheduler_fq1[32];
bcm_gport_t c6_e2e_gport[32];
bcm_gport_t c6_e2e_parent_gport[32];
int c6_use_same_handles = 0;


void e2e_dual_shaper_display_handles(int idx)
{
    printf("  e2eGport(0x%x)\n", c6_e2e_gport[idx]);
    printf("    l2_fq0(0x%x) l2_fq1(0x%x)\n",
                c6_gport_ucast_scheduler_fq0[idx], c6_gport_ucast_scheduler_fq1[idx]);
    printf("    schedGport(0x%x), schedCirGport(0x%x) schedCirGport(0x%x)\n",
                c6_gport_ucast_scheduler[idx], c6_gport_ucast_scheduler_cir[idx],
                c6_gport_ucast_scheduler_pir[idx]);
    printf("    connGrpGport(0x%x)\n", c6_gport_ucast_voq_connector_group[idx]);
    printf("    ucastGrpGport(0x%x)\n", c6_gport_ucast_queue_group[idx]);
}


int e2e_dual_shaper_setup(int port_idx)
{
    int idx, port, system_port;
    int num_cos = 4;
    int flags = 0, default_flags = 0;

    bcm_gport_t dest_gport[32];
    bcm_gport_t sys_gport[32];

    bcm_cosq_gport_connection_t connection;
    int kbits_sec_max;
    int max_burst;
    int cosq = 0;

    bcm_cosq_voq_connector_gport_t config;

    bcm_error_t rv = BCM_E_NONE;

    int is_dnx;

    /*
     * setup module id
     */

/*
    rv = bcm_stk_my_modid_set(c6_unit, c6_my_modid);
    if (rv != BCM_E_NONE) {
    printf("bcm_stk_my_modid_set failed $rv\n");
        return(rv);
    }
*/
    printf("nSetup\n");

    rv = bcm_device_member_get(c6_unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_device_member_get, $rv \n");
        return rv;
    }

    if (c6_use_same_handles == 1) {
        default_flags = BCM_COSQ_GPORT_WITH_ID;
        e2e_dual_shaper_display_handles(0);
    }

    if (!is_dnx) {
        /*
         * Setup 2 slow device rates
         */
        rv = bcm_cosq_control_set(c6_unit, 0, 0, bcmCosqControlFlowSlowRate1, 200);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_control_set() failed $rv\n");
            return(rv);
    }
        
        rv = bcm_cosq_control_set(c6_unit, 0, 0, bcmCosqControlFlowSlowRate2, 800);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_control_set() failed $rv\n");
            return(rv);
        }
    }

 printf("1\n");
    idx = port_idx;
    /* for (idx = 0; idx < C6_PETRA_CARD_NUM_PORTS; idx++) { */

    port = idx + 1;
    system_port = idx + 1;
    BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport[idx], system_port);
    BCM_GPORT_LOCAL_SET(dest_gport[idx], port);

    rv = bcm_stk_sysport_gport_set(c6_unit, sys_gport[idx], dest_gport[idx]);
    if (rv != BCM_E_NONE) {
        printf("bcm_stk_sysport_gport_set failed $rv\n");
        return(rv);
    }
 printf("2\n");

    /* 
     * set port scheduler mode
     */
    BCM_COSQ_GPORT_E2E_PORT_SET(c6_e2e_gport[idx], port);
    flags = C6_DEFAULT_E2E_SCHED_FLAGS | BCM_COSQ_GPORT_REPLACE;
    rv = bcm_cosq_gport_add(c6_unit, dest_gport[idx], 1, flags, &c6_e2e_gport[idx]);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return(rv);
    }
printf("23\n");

    /* 
     * set HR flow control mapping
     */
/*  BCM_COSQ_GPORT_E2E_PORT_SET(c6_e2e_gport[idx], port);
    rv = bcm_cosq_control_set(c6_unit, c6_e2e_gport[idx], -1, bcmCosqControlFlowControlPriority, 1);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_control_set() failed $rv\n");
        return(rv);
    } */
 printf("3\n");
/*
    BCM_COSQ_GPORT_E2E_PORT_SET(c6_e2e_gport[idx], port);
        rv = bcm_cosq_control_set(c6_unit, c6_e2e_gport, -1, bcmCosqControlFlowControlPriority, 2);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_control_set() failed $rv\n");
        return(rv);
    }
*/

    /*
     * create scheduler FQ scheduler (instance 0)
     */
    flags = C6_SCHED_FLAGS | default_flags;

    rv = bcm_cosq_gport_add(c6_unit, dest_gport[idx], 1,
                            flags, &c6_gport_ucast_scheduler_fq0[idx]);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return(rv);
    }

    /* FQ scheduler (instance 0) scheduling discipline configuration */
    rv = bcm_cosq_gport_sched_set(c6_unit,
                                  c6_gport_ucast_scheduler_fq0[idx],
                                  0,
                                  C6_SCHEDULER0_MODE,
                                  0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return(rv);
    }

    BCM_COSQ_GPORT_E2E_PORT_SET(c6_e2e_gport[idx], port);

    /* FQ scheduler (instance 0) attach to Port scheduler */
    rv = bcm_cosq_gport_attach(c6_unit, c6_e2e_gport[idx], c6_gport_ucast_scheduler_fq0[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return(rv);
    }
printf("4\n");
    /*
     * create scheduler FQ scheduler (instance 1)
     */
    flags = C6_SCHED_FLAGS | default_flags;

    rv = bcm_cosq_gport_add(c6_unit, dest_gport[idx], 1,
                            flags, &c6_gport_ucast_scheduler_fq1[idx]);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return(rv);
    }

    /* FQ scheduler (instance 1) scheduling discipline configuration */
    rv = bcm_cosq_gport_sched_set(c6_unit,
                                  c6_gport_ucast_scheduler_fq1[idx],
                                  0,
                                  C6_SCHEDULER1_MODE,
                                  0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return(rv);
    }

    BCM_COSQ_GPORT_E2E_PORT_SET(c6_e2e_gport[idx], port);

    /* FQ scheduler (instance 1) attach to Port scheduler */
    rv = bcm_cosq_gport_attach(c6_unit, c6_e2e_gport[idx], c6_gport_ucast_scheduler_fq1[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return(rv);
    }
printf("5\n");
        
    /*
     * create Dual Shaper Scheduling Element (CL-CIR, FQ-EIR) scheduler
     */
    flags = C6_SCHED_DUAL_FLAGS | default_flags;
    rv = bcm_cosq_gport_add(c6_unit, dest_gport[idx], 1, 
                flags, &c6_gport_ucast_scheduler[idx]);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return(rv);
    }

    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    uint32 uflags;

    rv = bcm_port_get(c6_unit, port, &uflags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_get: rv(%d), unit(%d), failed to get port(%d) info \n",rv, unit,port);
        /*return rv;*/
    }


    /* CIR, PIR scheduler handles */
    BCM_COSQ_GPORT_SCHED_CIR_CORE_SET(c6_gport_ucast_scheduler_cir[idx], c6_gport_ucast_scheduler[idx], mapping_info.core);
    BCM_COSQ_GPORT_SCHED_PIR_CORE_SET(c6_gport_ucast_scheduler_pir[idx], c6_gport_ucast_scheduler[idx], mapping_info.core);

    /* PIR (CL) scheduler element scheduling discipline configuration */
    rv = bcm_cosq_gport_sched_set(c6_unit,
                      c6_gport_ucast_scheduler_pir[idx],
                      0,
                      C6_SCHED_DUAL_EIR_MODE,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return(rv);
    }
printf("6\n");
    /* PIR (CL) scheduler element attach to FQ (1st instance) scheduler */
    rv = bcm_cosq_gport_attach(c6_unit, c6_gport_ucast_scheduler_fq0[idx], 
                   c6_gport_ucast_scheduler_pir[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return(rv);
    }

    /* CIR (FQ) scheduler element scheduling discipline configuration */
    rv = bcm_cosq_gport_sched_set(c6_unit,
                      c6_gport_ucast_scheduler_cir[idx],
                      0,
                      C6_SCHED_DUAL_CIR_MODE,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return(rv);
    }
printf("7\n");
    /* CIR (FQ) scheduler element attach to FQ (2nd instance)scheduler */
    rv = bcm_cosq_gport_attach(c6_unit, c6_gport_ucast_scheduler_fq1[idx], 
                   c6_gport_ucast_scheduler_cir[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return(rv);
    }


    /*
     * create voq connector bundle, 4 elements in bundle
     */          
    BCM_GPORT_LOCAL_SET(config.port, port);
    config.flags = BCM_COSQ_GPORT_VOQ_CONNECTOR | default_flags;
    config.numq = num_cos;
    config.remote_modid = c6_my_modid;
    config.nof_remote_cores = is_dnx ? 2: 1;
    rv = bcm_cosq_voq_connector_gport_add(c6_unit, &config, &c6_gport_ucast_voq_connector_group[idx]);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add connector failed $rv\n");
        return(rv);
    }

    /* COS 0 (bundle 1st element), scheduling discipline setup */
    rv = bcm_cosq_gport_sched_set(c6_unit,
                      c6_gport_ucast_voq_connector_group[idx],
                      0,
                      BCM_COSQ_SP0,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return(rv);
    }
printf("8\n");
    /* COS 0 (bundle 1st element, 1st flow), attach to PIR (CL) scheduler element */
    rv = bcm_cosq_gport_attach(c6_unit, c6_gport_ucast_scheduler_pir[idx], 
               c6_gport_ucast_voq_connector_group[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return(rv);
    }

    /* COS 1 (bundle 2nd element), scheduling discipline setup */
    rv = bcm_cosq_gport_sched_set(c6_unit,
                      c6_gport_ucast_voq_connector_group[idx],
                      1,
                      BCM_COSQ_SP0,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return(rv);
    }

    /* COS 1 (bundle 2nd element), attach to CIR (FQ) scheduler element */
    rv = bcm_cosq_gport_attach(c6_unit, c6_gport_ucast_scheduler_cir[idx], 
               c6_gport_ucast_voq_connector_group[idx], 1);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return(rv);
    }

    /* setup COS 1 to be in slow state */
    rv = bcm_cosq_control_set(c6_unit, c6_gport_ucast_voq_connector_group[idx], 1, bcmCosqControlFlowSlowRate, 1);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return(rv);
    }
printf("9\n");
    /*
     * create queue group bundle, 4 elements in bundle
     */          
    flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | default_flags;
    rv = bcm_cosq_gport_add(c6_unit, sys_gport[idx], num_cos, 
                flags, &c6_gport_ucast_queue_group[idx]);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add queue failed $rv\n");
        return(rv);
    }
    printf("ucast gport(0x%08x)\n", c6_gport_ucast_queue_group[idx]);

    /*
     * connect VoQ to flow
     */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
    connection.remote_modid = c6_my_modid;
    connection.voq = c6_gport_ucast_queue_group[idx];
    connection.voq_connector = c6_gport_ucast_voq_connector_group[idx];
    
    rv = bcm_cosq_gport_connection_set(c6_unit, &connection);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_connection_set ingress failed $rv\n");
        return(rv);
    }

    /*
     * connect flow to VoQ
     */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
    connection.remote_modid = c6_my_modid;
    connection.voq = c6_gport_ucast_queue_group[idx];
    connection.voq_connector = c6_gport_ucast_voq_connector_group[idx];
    
    rv = bcm_cosq_gport_connection_set(c6_unit, &connection);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_connection_set egress failed $rv\n");
        return(rv);
    }

    /*
     * Rate - Interface
     */
    kbits_sec_max = C6_E2E_INTERFACE_KBITS_SEC_MAX;
printf("10\n");
    BCM_COSQ_GPORT_E2E_PORT_SET(c6_e2e_gport[idx], port);

    rv = bcm_fabric_port_get(c6_unit,
                 c6_e2e_gport[idx],
                 0,
                 &c6_e2e_parent_gport[idx]);
    if (rv != BCM_E_NONE) {
        printf("bcm_fabric_port_get failed $rv\n");
        return(rv);
    }

    rv = bcm_cosq_gport_bandwidth_set(c6_unit,
                      c6_e2e_parent_gport[idx],
                      0,
                      0,
                      kbits_sec_max,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set if failed $rv\n");
        return(rv);
    }

    /*
     * Rate - E2E port
     */
    kbits_sec_max = C6_E2E_PORT_KBITS_SEC_MAX;
    
    BCM_COSQ_GPORT_E2E_PORT_SET(c6_e2e_gport[idx], port);

    rv = bcm_cosq_gport_bandwidth_set(c6_unit,
                      c6_e2e_gport[idx],
                      0,
                      0,
                      kbits_sec_max,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set failed $rv\n");
        return(rv);
    }

    /*
     * Rate - FQ (1st Instance) scheduler 
     */
    kbits_sec_max = C6_SCHED_KBITS_SEC_MAX;
    max_burst = C6_SCHED_MAX_BURST;
    rv = bcm_cosq_gport_bandwidth_set(c6_unit,
                                      c6_gport_ucast_scheduler_fq0[idx],
                                      0,
                                      0,
                                      kbits_sec_max,
                                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set sched failed $rv\n");
        return(rv);
    }
printf("11\n");
    rv = bcm_cosq_control_set(c6_unit,
                              c6_gport_ucast_scheduler_fq0[idx],
                              0,
                              bcmCosqControlBandwidthBurstMax,
                              max_burst);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_control_set max burst sched failed $rv\n");
        return(rv);
    }

    /*
     * Rate - FQ (2nd Instance) scheduler 
     */
    kbits_sec_max = C6_SCHED_KBITS_SEC_MAX;
    max_burst = C6_SCHED_MAX_BURST;
    rv = bcm_cosq_gport_bandwidth_set(c6_unit,
                                      c6_gport_ucast_scheduler_fq1[idx],
                                      0,
                                      0,
                                      kbits_sec_max,
                                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set sched failed $rv\n");
        return(rv);
    }

    rv = bcm_cosq_control_set(c6_unit,
                              c6_gport_ucast_scheduler_fq1[idx],
                              0,
                              bcmCosqControlBandwidthBurstMax,
                              max_burst);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_control_set max burst sched failed $rv\n");
        return(rv);
    }

    /*
     * Rate - PIR (CL) Scheduler --
     */
    kbits_sec_max = C6_SCHED_DUAL_EIR_KBITS_SEC_MAX; /* 100 Mbps */
    max_burst = C6_SCHED_MAX_BURST;
    rv = bcm_cosq_gport_bandwidth_set(c6_unit,
                      c6_gport_ucast_scheduler_pir[idx],
                      0,
                      0,
                      kbits_sec_max,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set sched failed $rv\n");
        return(rv);
    }
    
    /* Set max burst on the Scheduler */
    rv = bcm_cosq_control_set(c6_unit,
                  c6_gport_ucast_scheduler_pir[idx],
                  0,
                  bcmCosqControlBandwidthBurstMax,
                  max_burst);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_control_set max burst sched failed $rv\n");
        return(rv);
    }
    
    /*
     * Rate - CIR (FQ) Scheduler
     */
    kbits_sec_max = C6_SCHED_DUAL_CIR_KBITS_SEC_MAX; /* 500 Mbps */
    max_burst = C6_SCHED_MAX_BURST;
    rv = bcm_cosq_gport_bandwidth_set(c6_unit,
                      c6_gport_ucast_scheduler_cir[idx],
                      0,
                      0,
                      kbits_sec_max,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set sched failed $rv\n");
        return(rv);
    }
    
    /* Set max burst on the Scheduler */
    rv = bcm_cosq_control_set(c6_unit,
                  c6_gport_ucast_scheduler_cir[idx],
                  0,
                  bcmCosqControlBandwidthBurstMax,
                  max_burst);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_control_set max burst sched failed $rv\n");
        return(rv);
    }
    printf("13\n");
    /*
     * Rate - COS 0/1
     */
    kbits_sec_max = 500000; /* 300 Mbps */
    max_burst = 2500;
    for (cosq = 0; cosq < 2; cosq++) {
        /* Set rate on the Connector flow */
        rv = bcm_cosq_gport_bandwidth_set(c6_unit,
                          c6_gport_ucast_voq_connector_group[idx],
                          cosq,
                          0,
                          kbits_sec_max,
                          0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_bandwidth_set connector failed $rv\n");
            return(rv);
        }

        /* Set maxburst on the Connector flow */
        rv = bcm_cosq_control_set(c6_unit,
                      c6_gport_ucast_voq_connector_group[idx],
                      cosq,
                      bcmCosqControlBandwidthBurstMax,
                      max_burst);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_control_set connector maxburst failed $rv\n");
            return(rv);
        }
    }

    if (c6_use_same_handles == 0) {
        e2e_dual_shaper_display_handles(idx);
    }

    printf("port(%d) sysport(%d) ucast gport(0x%08x)\n", port, system_port, c6_gport_ucast_queue_group[idx]);
    /* } */

    printf("cint_e2e_dual_shaper.c completed with status (%s)\n", bcm_errmsg(rv));

    return(rv);
}

int e2e_dual_shaper_teardown(int port_idx)
{
    int                           rc = BCM_E_NONE;
    int                           idx = port_idx;
    int                           nbr_fq_sched;
    bcm_gport_t                   gport_scheduler_fq;
    bcm_gport_t                   gport_sched;
    bcm_cosq_gport_connection_t   connection;
    int                           cosq = 0;

    printf("nTeardown\n");

    /*for (idx = 0; idx < C6_PETRA_CARD_NUM_PORTS; idx++) {*/

        e2e_dual_shaper_display_handles(idx);

        /* detach scheduler sub-tree from E2E hierarchy */
        for (nbr_fq_sched = 0; nbr_fq_sched < C6_NBR_FQ_SCHEDULERS; nbr_fq_sched++) {
            switch (nbr_fq_sched) {
                case 0: gport_scheduler_fq = c6_gport_ucast_scheduler_fq0[idx]; break;
                default: gport_scheduler_fq = c6_gport_ucast_scheduler_fq1[idx]; break;
            }

            rc = bcm_cosq_gport_detach(c6_unit, c6_e2e_gport[idx], gport_scheduler_fq, 0);
            if (BCM_FAILURE(rc)) {
                printf("detach for SE(0x%x) failed, Error (%d, 0x%x)\n", gport_scheduler_fq, rc, rc);
                return(rc);
            }
            printf("  detached schedGport(0x%x)\n", gport_scheduler_fq);
        }


        /* dis-associate queue from connectors */
        connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
        connection.remote_modid = c6_my_modid;
        connection.voq = c6_gport_ucast_queue_group[idx];
        connection.voq_connector = c6_gport_ucast_voq_connector_group[idx];

        rc = bcm_cosq_gport_connection_set(c6_unit, &connection);
        if (BCM_FAILURE(rc)) {
            printf("resetting queue2connector association failed, queue(0x%x) Error (%d, 0x%x)\n", c6_gport_ucast_queue_group[idx], rc, rc);
            return(rc);
        }
        printf("  disconnected (Ingress) ucastGport(0x%x) from connGport(0x%x)\n", c6_gport_ucast_queue_group[idx], c6_gport_ucast_voq_connector_group[idx]);


        /* dis-associate connectors from queues */
        connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
        connection.remote_modid = c6_my_modid;
        connection.voq = c6_gport_ucast_queue_group[idx];
        connection.voq_connector = c6_gport_ucast_voq_connector_group[idx];

        rc = bcm_cosq_gport_connection_set(c6_unit, &connection);
        if (BCM_FAILURE(rc)) {
            printf("resetting connector2queue association failed, Connector(0x%x) Error (%d, 0x%x)\n", c6_gport_ucast_voq_connector_group[idx], rc, rc);
            return(rc);
        }
        printf("  disconnected (Engress) connGport(0x%x) from ucastGport(0x%x)\n", c6_gport_ucast_voq_connector_group[idx], c6_gport_ucast_queue_group[idx]);


        /* delete queues */
        rc = bcm_cosq_gport_delete(c6_unit, c6_gport_ucast_queue_group[idx]);
        if (BCM_FAILURE(rc)) {
            printf("delete for QueueGroup(0x%x) failed, Error (%d, 0x%x)\n", c6_gport_ucast_queue_group[idx], rc, rc);
            return(rc);
        }
        printf("  deleted ucastGport(0x%x)\n", c6_gport_ucast_queue_group[idx]);


        /* detach connectors */
        for (cosq = 0; cosq < 2; cosq++) {
            switch (cosq) {
                case 0:
                    gport_sched = c6_gport_ucast_scheduler_pir[idx];
                    break;
                default:
                    gport_sched = c6_gport_ucast_scheduler_cir[idx];
                    break;
            }

            rc = bcm_cosq_gport_detach(c6_unit, gport_sched, c6_gport_ucast_voq_connector_group[idx], cosq);
            if (BCM_FAILURE(rc)) {
                printf("detach for Connector:cosq(0x%x:%d) from Sched(0x%x) failed, Error (%d, 0x%x)\n", c6_gport_ucast_voq_connector_group[idx], cosq, gport_sched, rc, rc);
                return(rc);
            }
            printf("  detached connGport:cosq(0x%x:%d) from schedGport(0x%x)\n", c6_gport_ucast_voq_connector_group[idx], cosq, gport_sched);
        }


        /* delete connectors */
        rc = bcm_cosq_gport_delete(c6_unit, c6_gport_ucast_voq_connector_group[idx]);
        if (BCM_FAILURE(rc)) {
            printf("delete for ConnectorGroup(0x%x) failed, Error (%d, 0x%x)\n", c6_gport_ucast_voq_connector_group[idx], rc, rc);
            return(rc);
        }
        printf("  delete connGport(0x%x)\n", c6_gport_ucast_voq_connector_group[idx]);


        /* detach intermediate Scheduler elements (SE) */
        for (nbr_fq_sched = 0; nbr_fq_sched < C6_NBR_FQ_SCHEDULERS; nbr_fq_sched++) {
            switch (nbr_fq_sched) {
                case 0: gport_scheduler_fq = c6_gport_ucast_scheduler_fq0[idx];
                        gport_sched = c6_gport_ucast_scheduler_pir[idx];
                        break;
                default: gport_scheduler_fq = c6_gport_ucast_scheduler_fq1[idx];
                        gport_sched = c6_gport_ucast_scheduler_cir[idx];
                        break;
            }

            rc = bcm_cosq_gport_detach(c6_unit, gport_scheduler_fq, gport_sched, 0);
            if (BCM_FAILURE(rc)) {
                printf("detach for SE(0x%x) failed, Error (%d, 0x%x)\n", gport_sched, rc, rc);
                return(rc);
            }
            printf("  detached schedGport(0x%x)\n", gport_sched);
        }


        /* delete intermediate Scheduler elements (SE) */
        rc = bcm_cosq_gport_delete(c6_unit, c6_gport_ucast_scheduler[idx]);
        if (BCM_FAILURE(rc)) {
            printf("delete for SE(0x%x) failed, Error (%d, 0x%x)\n", c6_gport_ucast_scheduler[idx], rc, rc);
            return(rc);
        }
        printf("  delete schedGport(0x%x)\n", c6_gport_ucast_scheduler[idx]);

        /* delete scheduler  element at top of sub-tree */
        for (nbr_fq_sched = 0; nbr_fq_sched < C6_NBR_FQ_SCHEDULERS; nbr_fq_sched++) {
            switch (nbr_fq_sched) {
                case 0: gport_scheduler_fq = c6_gport_ucast_scheduler_fq0[idx]; break;
                default: gport_scheduler_fq = c6_gport_ucast_scheduler_fq1[idx]; break;
            }

            rc = bcm_cosq_gport_delete(c6_unit, gport_scheduler_fq);
            if (BCM_FAILURE(rc)) {
                printf("delete for SE(0x%x) failed, Error (%d, 0x%x)\n", gport_scheduler_fq, rc, rc);
                return(rc);
            }
            printf("  delete schedGport(0x%x)\n", gport_scheduler_fq);
        }

    /*} end of for loop */

    printf("cint_e2e_dual_shaper.c (teardown) completed with status (%s)\n", bcm_errmsg(rc));

    c6_use_same_handles = 1;

    return(rc);
}

int
e2e_dual_shaper_verify_mode(int unit, bcm_gport_t gport, int cosq, int exp_mode, int exp_weight)
{
    int                           rc = BCM_E_NONE;
    int                           mode, weight;


    rc = bcm_cosq_gport_sched_get(unit, gport, cosq, &mode, &weight);
    if (rc != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_get on Sched (0x%x) failed, error (%d)\n", gport_ucast_scheduler, rc);
        return(rc);
    }
    if (mode != exp_mode) {
        printf("sched mode mismatch on gport (0x%x) expected(BCM_COSQ_SP%d) retrieved (BCM_COSQ_SP%d)\n", gport, (exp_mode - BCM_COSQ_SP0), (mode - BCM_COSQ_SP0));
    }
    if (weight != exp_weight) {
        printf("sched weight mismatch on gport (0x%x) expected(%d) retrieved (%d)\n", gport, 0, weight);
    }

    return(rc);
}

int
e2e_dual_shaper_verify_bandwidth(int unit, bcm_gport_t gport, int cosq, int exp_rate, int margin)
{
    int                           rc = BCM_E_NONE;
    uint32                           kbits_sec_min, kbits_sec_max, flags;


    rc = bcm_cosq_gport_bandwidth_get(unit, gport, cosq, &kbits_sec_min, &kbits_sec_max, &flags);
    if (rc != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_get on gport (0x%x) failed, error (%d)\n", gport, rc);
        return(rc);
    }
    if ( (kbits_sec_max < (exp_rate - margin)) || (kbits_sec_max > (exp_rate + margin)) ) {
        printf("bandwidth_get mismatch on gport (0x%x) expected(%d) retrieved (%d)\n", gport, exp_rate, kbits_sec_max);
    }

    return(rc);
}

int
e2e_dual_shaper_verify_burst_size(int unit, bcm_gport_t gport, int cosq, int exp_burst_size, int margin)
{
    int                           rc = BCM_E_NONE;
    int                           max_burst;


    rc = bcm_cosq_control_get(unit, gport, cosq, bcmCosqControlBandwidthBurstMax, &max_burst);
    if (rc != BCM_E_NONE) {
        printf("bcm_cosq_control_get-BandwidthBurstMax on gport (0x%x) failed, error (%d)\n", gport, rc);
        return(rc);
    }
    if ( (max_burst < (exp_burst_size - margin)) || (max_burst > (exp_burst_size + margin)) ) {
        printf("control_get-BandwidthBurstMax mismatch on Sched (0x%x) expected(%d) retrieved (%d)\n", gport, exp_burst_size, max_burst);
    }

    return(rc);
}


int e2e_dual_shaper_verify(int port_idx)
{
    int                           rc = BCM_E_NONE;
    int                           idx = port_idx;
    bcm_gport_t                   physical_port;
    int                           num_cos_levels;
    uint32                        flags;
    int                           nbr_fq_sched, nbr_sched;
    bcm_gport_t                   gport_scheduler_fq;
    int                           exp_mode, exp_margin, exp_kbits_sec_max;
    bcm_gport_t                   gport_sched;
    bcm_cosq_gport_connection_t   connection;
    int                           cosq = 0;


    printf("nVerify Setup\n");

    /*for (idx = 0; idx < C6_PETRA_CARD_NUM_PORTS; idx++) {*/

        rc = e2e_dual_shaper_verify_bandwidth(c6_unit, c6_e2e_parent_gport[idx], 0, 
                         C6_E2E_INTERFACE_KBITS_SEC_MAX, C6_E2E_INTERFACE_KBITS_SEC_MAX_MARGIN);
        if (rc != BCM_E_NONE) {
            return(rc);
        }
        
        rc = e2e_dual_shaper_verify_bandwidth(c6_unit, c6_e2e_gport[idx], 0, 
                         C6_E2E_PORT_KBITS_SEC_MAX, C6_E2E_PORT_KBITS_SEC_MAX_MARGIN);
        if (rc != BCM_E_NONE) {
            return(rc);
        }
        
        /* retrieve e2e port scheduling element */
        rc = bcm_cosq_gport_get(c6_unit, c6_e2e_gport[idx], &physical_port, &num_cos_levels, &flags);
        if (rc != BCM_E_NONE) {
            printf("bcm_cosq_gport_get on e2e Sched (0x%x) failed, error (%d)\n", c6_e2e_gport[idx], rc); 
            return(rc);
        }
        if (flags != C6_DEFAULT_E2E_SCHED_FLAGS) {
            printf("gport get mismatch on E2E Sched (0x%x) expected flags(0x%x) retrieved flags (0x%x)\n", c6_e2e_gport[idx], C6_DEFAULT_E2E_SCHED_FLAGS, flags);
        }
    
        /* retrieve scheduler element at top of sub-tree */
        for (nbr_fq_sched = 0; nbr_fq_sched < C6_NBR_FQ_SCHEDULERS; nbr_fq_sched++) {
            switch (nbr_fq_sched) {
                case 0: gport_scheduler_fq = c6_gport_ucast_scheduler_fq0[idx];
                        exp_mode = C6_SCHEDULER0_MODE;
                        break;
                default: gport_scheduler_fq = c6_gport_ucast_scheduler_fq1[idx];
                        exp_mode = C6_SCHEDULER1_MODE;
                        break;
            }
    
            /* retrieve type of scheduling element */
            rc = bcm_cosq_gport_get(c6_unit, gport_scheduler_fq,
                                    &physical_port, &num_cos_levels, &flags);
            if (rc != BCM_E_NONE) {
                printf("bcm_cosq_gport_get on Sched (0x%x) failed, error (%d)\n", gport_scheduler_fq, rc);
                return(rc);
            }
            if (flags != C6_SCHED_FLAGS) {
                printf("gport get mismatch on Sched (0x%x) expected flags(0x%x) retrieved flags (0x%x)\n", gport_scheduler_fq, C6_SCHED_FLAGS, flags);
            }
    
            /* retrieve SE scheduling discipline */
            rc = e2e_dual_shaper_verify_mode(c6_unit, gport_scheduler_fq, 0, exp_mode, 0);
            if (rc != BCM_E_NONE) {
                return(rc);
            }
    
            /* retrieve rate of Scheduler */
            rc = e2e_dual_shaper_verify_bandwidth(c6_unit, gport_scheduler_fq, 0, C6_SCHED_KBITS_SEC_MAX, C6_SCHED_KBITS_SEC_MAX_MARGIN);
            if (rc != BCM_E_NONE) {
                 return(rc);
            }
    
            /* retrieve scheduler burst size */
            rc = e2e_dual_shaper_verify_burst_size(c6_unit, gport_scheduler_fq, 0, C6_SCHED_MAX_BURST, C6_SCHED_MAX_BURST_MARGIN);
            if (rc != BCM_E_NONE) {
                return(rc);
            }
        }
    
        /* retrieve type of intermediate scheduling element */
        rc = bcm_cosq_gport_get(c6_unit, c6_gport_ucast_scheduler[idx],
                                               &physical_port, &num_cos_levels, &flags);
        if (rc != BCM_E_NONE) {
            printf("bcm_cosq_gport_get on Sched (0x%x) failed, error (%d)\n", c6_gport_ucast_scheduler[idx], rc);
            return(rc);
        }
        if (flags != C6_SCHED_DUAL_FLAGS) {
            printf("gport get mismatch on Sched (0x%x) expected flags(0x%x) retrieved flags (0x%x)\n", c6_gport_ucast_scheduler[idx], C6_SCHED_DUAL_FLAGS, flags);
        }
    
        /* retrieve intermediate Scheduler elements (SE) atributes */
        for (nbr_sched = 0; nbr_sched < 2; nbr_sched++) {
            switch (nbr_sched) {
                case 0: gport_sched = c6_gport_ucast_scheduler_pir[idx];
                        exp_mode = C6_SCHED_DUAL_EIR_MODE;
                        exp_kbits_sec_max = C6_SCHED_DUAL_EIR_KBITS_SEC_MAX;
                        exp_margin = C6_SCHED_DUAL_EIR_KBITS_SEC_MAX_MARGIN; break;
                default: gport_sched = c6_gport_ucast_scheduler_cir[idx];
                        exp_mode = C6_SCHED_DUAL_CIR_MODE;
                        exp_kbits_sec_max = C6_SCHED_DUAL_CIR_KBITS_SEC_MAX;
                        exp_margin = C6_SCHED_DUAL_CIR_KBITS_SEC_MAX_MARGIN; break;
            }
    
            /* retrieve SE scheduling discipline */
            rc = e2e_dual_shaper_verify_mode(c6_unit, gport_sched, 0, exp_mode, 0);
            if (rc != BCM_E_NONE) {
                return(rc);
            }
    
            /* retrieve rate of Scheduler */
            rc = e2e_dual_shaper_verify_bandwidth(c6_unit, gport_sched, 0, exp_kbits_sec_max, exp_margin);
            if (rc != BCM_E_NONE) {
                 return(rc);
            }
    
            /* retrieve scheduler burst size */
            rc = e2e_dual_shaper_verify_burst_size(c6_unit, gport_sched, 0, C6_SCHED_MAX_BURST, C6_SCHED_MAX_BURST_MARGIN);
            if (rc != BCM_E_NONE) {
                return(rc);
            }
        }
    /*} end of for loop */


    /* Reference other scripts for verification of remaining hierarchy */

    printf("cint_e2e_dual_shaper.c (verify) completed with status (%s)\n", bcm_errmsg(rc));

    return(rc);
}

/* Traffic passes after this step */
e2e_dual_shaper_setup(12);

/* Traffic stops after this step */
/* e2e_dual_shaper_teardown(12); */

/* verify setup */
e2e_dual_shaper_verify(12);

