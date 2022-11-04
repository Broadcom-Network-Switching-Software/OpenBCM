/*~~~~~~~~~~~~~~~~~~~~~~~~~~Cosq: E2E Scheduling~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2022 Broadcom Inc. All rights reserved.
 *
 * File: cint_dnx_e2e_dual_shaper.c
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
 *   - Dual Shaper scheduling element, CIR (CL - SP mode) and PIR (FQ) scheduling elements
 *       -> Hookup CIR (CL) scheduler to 1st FQ scheduler and PIR (FQ) scheduler to 2nd FQ Scheduler
 *       -> CIR (CL) scheduler rate set to 500 Mbps
 *       -> PIR (FQ) scheduler rate set to 100 Mbps
 *   - 1 Queue Group / Connector Group (4 elements in bundle)
 *       -> 1st Connector element hooked up to CIR (CL) scheduler
 *       -> 2nd Connector element hooked up to PIR (FQ) scheduler. Slow configuration enabled.
 *
 * - Input
 *   - Queue - Cos 0
 *     Input 100 Mbps
 *   - Queue - Cos 1
 *     Input 400 Mbps
 *
 * - Expected Output
 *   - Queue - Cos 0
 *     Expected Output 100 Mbps
 *     Observed 100 Mbps
 *   - Queue - Cos 1
 *     Expected Output 400 Mbps
 *     Observed 400 Mbps
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
 *  |                     (500 Mbps)              (100 Mbps)                       |
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
 *  |                   Input:100 Mbps         Input:400 Mbps  |      KEY       |  |
 *  |                                                          +----------------+  |
 *  |                                                          |SPR- Rate Shaper|  |
 *  |                                                          |                |  |
 *  |                                                          +----------------+  |
 *  |                     +~~~~~~~~~~~~~~~~~~~~~~~~~~~+                            |
 *  |                     |   Figure 9: Dual Shaper   |                            |
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 */

/*
 * General Definitions
 */

int C6_DEFAULT_E2E_SCHED_FLAGS = (BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_HR_SINGLE_WFQ);
int C6_DEFAULT_E2E_SCHED_TYPE = bcmCosqSchedulerGportTypeHrSingleWfq;

int C6_E2E_INTERFACE_KBITS_SEC_MAX = 500000; /* 500 mbps */
int C6_E2E_INTERFACE_KBITS_SEC_MAX_MARGIN = 20; /* 20 kbps */

int C6_E2E_PORT_KBITS_SEC_MAX = 500000; /* 500mbps */
int C6_E2E_PORT_KBITS_SEC_MAX_MARGIN = 10; /* 10 kbps */

int C6_NBR_FQ_SCHEDULERS = 2;
int C6_SCHED_FQ_TYPE = bcmCosqSchedulerGportTypeFq;
int C6_SCHEDULER0_MODE = BCM_COSQ_SP0;
int C6_SCHEDULER1_MODE = BCM_COSQ_SP1;
int C6_SCHED_KBITS_SEC_MAX = 500000; /* 500 Mbps */
int C6_SCHED_KBITS_SEC_MAX_MARGIN = 10; /* 10 Kbps */

int C6_SCHED_DUAL_TYPE = bcmCosqSchedulerGportTypeClass4Sp;
int C6_SCHED_DUAL_MODE = bcmCosqSchedulerGportModeSharedDual;
int C6_SCHED_DUAL_CIR_MODE = BCM_COSQ_SP0;
int C6_SCHED_DUAL_CIR_KBITS_SEC_MAX = 500000; /* 500 Mbps */
int C6_SCHED_DUAL_CIR_KBITS_SEC_MAX_MARGIN = 10; /* 10 Kbps */

int C6_SCHED_DUAL_EIR_MODE = BCM_COSQ_SP0;
int C6_SCHED_DUAL_EIR_KBITS_SEC_MAX = 100000; /* 100 Mbps */
int C6_SCHED_DUAL_EIR_KBITS_SEC_MAX_MARGIN = 10; /* 10 Kbps */
int is_dnx;

/* handles */
int c6_unit = 0;
int c6_my_modid = 0;

int credit_worth = *(dnxc_data_get(c6_unit, "iqs", "credit", "worth", NULL));
int C6_SCHED_MAX_BURST = (3000 * (credit_worth / 1024));
int C6_SCHED_MAX_BURST_MARGIN = (75 * (credit_worth / 1024));

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
    int flags = 0, default_flags = 0, default_voq_flags = 0, default_connector_flags = 0;

    bcm_gport_t dest_gport[32];
    bcm_gport_t sys_gport[32];

    bcm_cosq_gport_connection_t connection;
    int kbits_sec_max;
    int max_burst;
    int cosq = 0;
    uint32 dummy_flags ;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_cosq_voq_connector_gport_t config;
    bcm_cosq_ingress_queue_bundle_gport_config_t conf;
    bcm_port_resource_t resource;
    int delay_tolerance_level, rate_class;
    bcm_cosq_scheduler_gport_params_t params;
    bcm_cosq_gport_info_t gport_info;

    printf("nSetup\n");

    if (c6_use_same_handles == 1) {
        default_flags = BCM_COSQ_SCHEDULER_GPORT_WITH_ID;
        default_voq_flags = BCM_COSQ_GPORT_WITH_ID;
        default_connector_flags = BCM_COSQ_VOQ_CONNECTOR_GPORT_F_WITH_ID;
        e2e_dual_shaper_display_handles(0);
    }

    idx = port_idx;

    port = idx + 1;
    system_port = idx + 1;
    BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport[idx], system_port);
    BCM_GPORT_LOCAL_SET(dest_gport[idx], port);

    BCM_IF_ERROR_RETURN_MSG(bcm_stk_sysport_gport_set(c6_unit, sys_gport[idx], dest_gport[idx]), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_port_get(c6_unit, port, &dummy_flags, &interface_info, &mapping_info), "");
    /*
     * set port scheduler mode
     */
    BCM_COSQ_GPORT_E2E_PORT_SET(c6_e2e_gport[idx], port);
    gport_info.in_gport = c6_e2e_gport[idx];
    gport_info.cosq = 0;

    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_handle_get(c6_unit, bcmCosqGportTypeSched, &gport_info), "");
    c6_e2e_gport[idx] = gport_info.out_gport;

    params.type = bcmCosqSchedulerGportTypeHrSingleWfq;
    params.mode = bcmCosqSchedulerGportModeSimple;
    params.cl_wfq_mode = bcmCosqSchedulerGportClWfqModeNone;
    params.core = mapping_info.core;
    params.shared_order = bcmCosqSchedulerGportSharedOrderNone;

    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_scheduler_gport_add(c6_unit, BCM_COSQ_SCHEDULER_GPORT_REPLACE, params, &c6_e2e_gport[idx]), "");

    /*
     * create scheduler FQ scheduler (instance 0)
     */

    params.type = bcmCosqSchedulerGportTypeFq;
    params.mode = bcmCosqSchedulerGportModeSimple;
    params.cl_wfq_mode = bcmCosqSchedulerGportClWfqModeNone;
    params.core = mapping_info.core;
    params.shared_order = bcmCosqSchedulerGportSharedOrderNone;

    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_scheduler_gport_add(c6_unit, default_flags, params, &c6_gport_ucast_scheduler_fq0[idx]), "");

    /* FQ scheduler (instance 0) scheduling discipline configuration */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_sched_set(c6_unit,
                                  c6_gport_ucast_scheduler_fq0[idx],
                                  0,
                                  C6_SCHEDULER0_MODE,
                                  0), "");

    BCM_COSQ_GPORT_E2E_PORT_SET(c6_e2e_gport[idx], port);

    /* FQ scheduler (instance 0) attach to Port scheduler */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_attach(c6_unit, c6_e2e_gport[idx], c6_gport_ucast_scheduler_fq0[idx], 0), "");

    /*
     * create scheduler FQ scheduler (instance 1)
     */

    params.type = bcmCosqSchedulerGportTypeFq;
    params.mode = bcmCosqSchedulerGportModeSimple;
    params.cl_wfq_mode = bcmCosqSchedulerGportClWfqModeNone;
    params.core = mapping_info.core;
    params.shared_order = bcmCosqSchedulerGportSharedOrderNone;

    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_scheduler_gport_add(c6_unit, default_flags, params, &c6_gport_ucast_scheduler_fq1[idx]), "");

    /* FQ scheduler (instance 1) scheduling discipline configuration */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_sched_set(c6_unit,
                                  c6_gport_ucast_scheduler_fq1[idx],
                                  0,
                                  C6_SCHEDULER1_MODE,
                                  0), "");

    BCM_COSQ_GPORT_E2E_PORT_SET(c6_e2e_gport[idx], port);

    /* FQ scheduler (instance 1) attach to Port scheduler */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_attach(c6_unit, c6_e2e_gport[idx], c6_gport_ucast_scheduler_fq1[idx], 0), "");

    /*
     * create Dual Shaper Scheduling Element (CL-CIR, FQ-EIR) scheduler
     */

    params.type = bcmCosqSchedulerGportTypeClass4Sp;
    params.mode = bcmCosqSchedulerGportModeSharedDual;
    params.cl_wfq_mode = bcmCosqSchedulerGportClWfqModeNone;
    params.core = mapping_info.core;
    params.shared_order = bcmCosqSchedulerGportSharedOrderNone;

    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_scheduler_gport_add(c6_unit, default_flags, params, &c6_gport_ucast_scheduler[idx]), "");

    /* CIR, PIR scheduler handles */
    BCM_COSQ_GPORT_SCHED_CIR_CORE_SET(c6_gport_ucast_scheduler_cir[idx], c6_gport_ucast_scheduler[idx], mapping_info.core);
    BCM_COSQ_GPORT_SCHED_PIR_CORE_SET(c6_gport_ucast_scheduler_pir[idx], c6_gport_ucast_scheduler[idx], mapping_info.core);

    /* CIR (CL) scheduler element scheduling discipline configuration */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_sched_set(c6_unit,
                      c6_gport_ucast_scheduler_cir[idx],
                      0,
                      C6_SCHED_DUAL_CIR_MODE,
                      0), "");

    /* CIR (CL) scheduler element attach to FQ (1st instance) scheduler */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_attach(c6_unit, c6_gport_ucast_scheduler_fq0[idx],
                   c6_gport_ucast_scheduler_cir[idx], 0), "");

    /* PIR (FQ) scheduler element scheduling discipline configuration */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_sched_set(c6_unit,
                      c6_gport_ucast_scheduler_pir[idx],
                      0,
                      C6_SCHED_DUAL_EIR_MODE,
                      0), "");

    /* PIR (FQ) scheduler element attach to FQ (2nd instance)scheduler */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_attach(c6_unit, c6_gport_ucast_scheduler_fq1[idx],
                   c6_gport_ucast_scheduler_pir[idx], 0), "");


    /*
     * create voq connector bundle, 4 elements in bundle
     */
    BCM_GPORT_LOCAL_SET(config.port, port);
    config.flags = BCM_COSQ_GPORT_VOQ_CONNECTOR | default_connector_flags;
    config.numq = num_cos;
    config.remote_modid = c6_my_modid + mapping_info.core;
    config.nof_remote_cores = 1;
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_voq_connector_gport_add(c6_unit, &config, &c6_gport_ucast_voq_connector_group[idx]), "");

    /* COS 0 (bundle 1st element), scheduling discipline setup */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_sched_set(c6_unit,
                      c6_gport_ucast_voq_connector_group[idx],
                      0,
                      BCM_COSQ_SP0,
                      0), "");

    /* COS 0 (bundle 1st element, 1st flow), attach to CIR (CL) scheduler element */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_attach(c6_unit, c6_gport_ucast_scheduler_cir[idx],
               c6_gport_ucast_voq_connector_group[idx], 0), "");

    /* COS 1 (bundle 2nd element), scheduling discipline setup */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_sched_set(c6_unit,
                      c6_gport_ucast_voq_connector_group[idx],
                      1,
                      BCM_COSQ_SP0,
                      0), "");

    /* COS 1 (bundle 2nd element), attach to PIR (FQ) scheduler element */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_attach(c6_unit, c6_gport_ucast_scheduler_pir[idx],
               c6_gport_ucast_voq_connector_group[idx], 1), "");

    /* setup COS 1 to be in slow state */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_set(c6_unit, c6_gport_ucast_voq_connector_group[idx], 1, bcmCosqControlFlowSlowRate, 1), "");
    /*
     * create queue group bundle, 4 elements in bundle
     */
    flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | default_voq_flags;

    BCM_IF_ERROR_RETURN_MSG(bcm_port_resource_get(c6_unit, port, &resource), "");
    BCM_IF_ERROR_RETURN_MSG(appl_dnx_e2e_voq_profiles_get(c6_unit, resource.speed, &delay_tolerance_level, &rate_class), "");
    conf.flags = flags;
    conf.port = sys_gport[idx];
    conf.numq = num_cos;
    conf.local_core_id = BCM_CORE_ALL;
    for (cosq = 0; cosq < num_cos; cosq++) {
        conf.queue_atrributes[cosq].delay_tolerance_level = delay_tolerance_level;
        conf.queue_atrributes[cosq].rate_class = rate_class;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_ingress_queue_bundle_gport_add(c6_unit, &conf, &c6_gport_ucast_queue_group[idx]), "");
    printf("ucast gport(0x%08x)\n", c6_gport_ucast_queue_group[idx]);

    /*bcm_cosq_ingress_queue_bundle_gport_add will a voq gport with core - BCM_CORE_ALL.
      In order to be consistent with the asymmetric scheme to be created
      the core is overwritten.*/
    int voq_id = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(c6_gport_ucast_queue_group[idx]);
    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(c6_gport_ucast_queue_group[idx], mapping_info.core, voq_id);

    /*
     * connect VoQ to flow
     */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
    connection.remote_modid = c6_my_modid + mapping_info.core;
    connection.voq = c6_gport_ucast_queue_group[idx];
    connection.voq_connector = c6_gport_ucast_voq_connector_group[idx];

    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_connection_set(c6_unit, &connection), "");

    /*
     * connect flow to VoQ
     */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
    connection.remote_modid = c6_my_modid + mapping_info.core;
    connection.voq = c6_gport_ucast_queue_group[idx];
    connection.voq_connector = c6_gport_ucast_voq_connector_group[idx];

    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_connection_set(c6_unit, &connection), "");

    /*
     * Rate - Interface
     */
    kbits_sec_max = C6_E2E_INTERFACE_KBITS_SEC_MAX;

    BCM_COSQ_GPORT_E2E_PORT_SET(c6_e2e_gport[idx], port);

    BCM_IF_ERROR_RETURN_MSG(bcm_fabric_port_get(c6_unit,
                 c6_e2e_gport[idx],
                 0,
                 &c6_e2e_parent_gport[idx]), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_set(c6_unit,
                      c6_e2e_parent_gport[idx],
                      0,
                      0,
                      kbits_sec_max,
                      0), "");

    /*
     * Rate - E2E port
     */
    kbits_sec_max = C6_E2E_PORT_KBITS_SEC_MAX;

    BCM_COSQ_GPORT_E2E_PORT_SET(c6_e2e_gport[idx], port);

    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_set(c6_unit,
                      c6_e2e_gport[idx],
                      0,
                      0,
                      kbits_sec_max,
                      0), "");

    /*
     * Rate - FQ (1st Instance) scheduler
     */
    kbits_sec_max = C6_SCHED_KBITS_SEC_MAX;
    max_burst = C6_SCHED_MAX_BURST;
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_set(c6_unit,
                                      c6_gport_ucast_scheduler_fq0[idx],
                                      0,
                                      0,
                                      kbits_sec_max,
                                      0), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_set(c6_unit,
                              c6_gport_ucast_scheduler_fq0[idx],
                              0,
                              bcmCosqControlBandwidthBurstMax,
                              max_burst), "");

    /*
     * Rate - FQ (2nd Instance) scheduler
     */
    kbits_sec_max = C6_SCHED_KBITS_SEC_MAX;
    max_burst = C6_SCHED_MAX_BURST;
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_set(c6_unit,
                                      c6_gport_ucast_scheduler_fq1[idx],
                                      0,
                                      0,
                                      kbits_sec_max,
                                      0), "");

    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_set(c6_unit,
                              c6_gport_ucast_scheduler_fq1[idx],
                              0,
                              bcmCosqControlBandwidthBurstMax,
                              max_burst), "");

    /*
     * Rate - CIR (CL) Scheduler
     */
    kbits_sec_max = C6_SCHED_DUAL_CIR_KBITS_SEC_MAX; /* 500 Mbps */
    max_burst = C6_SCHED_MAX_BURST;
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_set(c6_unit,
                      c6_gport_ucast_scheduler_cir[idx],
                      0,
                      0,
                      kbits_sec_max,
                      0), "");

    /* Set max burst on the Scheduler */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_set(c6_unit,
                  c6_gport_ucast_scheduler_cir[idx],
                  0,
                  bcmCosqControlBandwidthBurstMax,
                  max_burst), "");

    /*
     * Rate - PIR (FQ) Scheduler
     */
    kbits_sec_max = C6_SCHED_DUAL_EIR_KBITS_SEC_MAX; /* 100 Mbps */
    max_burst = C6_SCHED_MAX_BURST;
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_set(c6_unit,
                      c6_gport_ucast_scheduler_pir[idx],
                      0,
                      0,
                      kbits_sec_max,
                      0), "");

    /* Set max burst on the Scheduler */
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_set(c6_unit,
                  c6_gport_ucast_scheduler_pir[idx],
                  0,
                  bcmCosqControlBandwidthBurstMax,
                  max_burst), "");
    printf("13\n");
    /*
     * Rate - COS 0/1
     */
    kbits_sec_max = 500000; /* 300 Mbps */
    max_burst = 2500;
    for (cosq = 0; cosq < 2; cosq++) {
        /* Set rate on the Connector flow */
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_set(c6_unit,
                          c6_gport_ucast_voq_connector_group[idx],
                          cosq,
                          0,
                          kbits_sec_max,
                          0), "");

        /* Set maxburst on the Connector flow */
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_set(c6_unit,
                      c6_gport_ucast_voq_connector_group[idx],
                      cosq,
                      bcmCosqControlBandwidthBurstMax,
                      max_burst), "");
    }

    if (c6_use_same_handles == 0) {
        e2e_dual_shaper_display_handles(idx);
    }

    printf("port(%d) sysport(%d) ucast gport(0x%08x)\n", port, system_port, c6_gport_ucast_queue_group[idx]);

    printf("cint_dnx_e2e_dual_shaper.c completed with status (%s)\n", bcm_errmsg(rv));

    return BCM_E_NONE;
}

int e2e_dual_shaper_teardown(int port_idx)
{
    int                           idx = port_idx;
    int                           nbr_fq_sched;
    bcm_gport_t                   gport_scheduler_fq;
    bcm_gport_t                   gport_sched;
    bcm_cosq_gport_connection_t   connection;
    int                           cosq = 0;
    char                          error_msg[200]={0,};

    printf("nTeardown\n");

    e2e_dual_shaper_display_handles(idx);

    /* detach scheduler sub-tree from E2E hierarchy */
    for (nbr_fq_sched = 0; nbr_fq_sched < C6_NBR_FQ_SCHEDULERS; nbr_fq_sched++) {
        switch (nbr_fq_sched) {
        case 0: gport_scheduler_fq = c6_gport_ucast_scheduler_fq0[idx]; break;
        default: gport_scheduler_fq = c6_gport_ucast_scheduler_fq1[idx]; break;
        }

        snprintf(error_msg, sizeof(error_msg), "detach for SE(0x%x) failed", gport_scheduler_fq);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_detach(c6_unit, c6_e2e_gport[idx], gport_scheduler_fq, 0), error_msg);
        printf("  detached schedGport(0x%x)\n", gport_scheduler_fq);
    }


    /* dis-associate queue from connectors */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
    connection.remote_modid = c6_my_modid;
    connection.voq = c6_gport_ucast_queue_group[idx];
    connection.voq_connector = c6_gport_ucast_voq_connector_group[idx];

    snprintf(error_msg, sizeof(error_msg), "resetting queue2connector association failed, queue(0x%x)", c6_gport_ucast_queue_group[idx]);
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_connection_set(c6_unit, &connection), error_msg);
    printf("  disconnected (Ingress) ucastGport(0x%x) from connGport(0x%x)\n", c6_gport_ucast_queue_group[idx], c6_gport_ucast_voq_connector_group[idx]);


    /* dis-associate connectors from queues */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
    connection.remote_modid = c6_my_modid;
    connection.voq = c6_gport_ucast_queue_group[idx];
    connection.voq_connector = c6_gport_ucast_voq_connector_group[idx];

    snprintf(error_msg, sizeof(error_msg), "resetting connector2queue association failed, Connector(0x%x)", c6_gport_ucast_voq_connector_group[idx]);
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_connection_set(c6_unit, &connection), error_msg);
    printf("  disconnected (Engress) connGport(0x%x) from ucastGport(0x%x)\n", c6_gport_ucast_voq_connector_group[idx], c6_gport_ucast_queue_group[idx]);


    /* delete queues */
    snprintf(error_msg, sizeof(error_msg), "delete for QueueGroup(0x%x) failed", c6_gport_ucast_queue_group[idx]);
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_delete(c6_unit, c6_gport_ucast_queue_group[idx]), error_msg);
    printf("  deleted ucastGport(0x%x)\n", c6_gport_ucast_queue_group[idx]);


    /* detach connectors */
    for (cosq = 0; cosq < 2; cosq++) {
        switch (cosq) {
        case 0:
            gport_sched = c6_gport_ucast_scheduler_cir[idx];
            break;
        default:
            gport_sched = c6_gport_ucast_scheduler_pir[idx];
            break;
        }

        snprintf(error_msg, sizeof(error_msg), "detach for Connector:cosq(0x%x:%d) from Sched(0x%x) failed", c6_gport_ucast_voq_connector_group[idx], cosq, gport_sched);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_detach(c6_unit, gport_sched, c6_gport_ucast_voq_connector_group[idx], cosq), error_msg);
        printf("  detached connGport:cosq(0x%x:%d) from schedGport(0x%x)\n", c6_gport_ucast_voq_connector_group[idx], cosq, gport_sched);
    }


    /* delete connectors */
    snprintf(error_msg, sizeof(error_msg), "delete for ConnectorGroup(0x%x) failed", c6_gport_ucast_voq_connector_group[idx]);
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_delete(c6_unit, c6_gport_ucast_voq_connector_group[idx]), error_msg);
    printf("  delete connGport(0x%x)\n", c6_gport_ucast_voq_connector_group[idx]);


    /* detach intermediate Scheduler elements (SE) */
    for (nbr_fq_sched = 0; nbr_fq_sched < C6_NBR_FQ_SCHEDULERS; nbr_fq_sched++) {
        switch (nbr_fq_sched) {
        case 0: gport_scheduler_fq = c6_gport_ucast_scheduler_fq0[idx];
        gport_sched = c6_gport_ucast_scheduler_cir[idx];
        break;
        default: gport_scheduler_fq = c6_gport_ucast_scheduler_fq1[idx];
        gport_sched = c6_gport_ucast_scheduler_pir[idx];
        break;
        }

        snprintf(error_msg, sizeof(error_msg), "detach for SE(0x%x) failed", gport_sched);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_detach(c6_unit, gport_scheduler_fq, gport_sched, 0), error_msg);
        printf("  detached schedGport(0x%x)\n", gport_sched);
    }


    /* delete intermediate Scheduler elements (SE) */
    snprintf(error_msg, sizeof(error_msg), "delete for SE(0x%x) failed", c6_gport_ucast_scheduler[idx]);
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_delete(c6_unit, c6_gport_ucast_scheduler[idx]), error_msg);
    printf("  delete schedGport(0x%x)\n", c6_gport_ucast_scheduler[idx]);

    /* delete scheduler  element at top of sub-tree */
    for (nbr_fq_sched = 0; nbr_fq_sched < C6_NBR_FQ_SCHEDULERS; nbr_fq_sched++) {
        switch (nbr_fq_sched) {
        case 0: gport_scheduler_fq = c6_gport_ucast_scheduler_fq0[idx]; break;
        default: gport_scheduler_fq = c6_gport_ucast_scheduler_fq1[idx]; break;
        }

        snprintf(error_msg, sizeof(error_msg), "delete for SE(0x%x) failed", gport_scheduler_fq);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_delete(c6_unit, gport_scheduler_fq), error_msg);
        printf("  delete schedGport(0x%x)\n", gport_scheduler_fq);
    }


    printf("cint_dnx_e2e_dual_shaper.c (teardown) completed with status (%s)\n", bcm_errmsg(rc));

    c6_use_same_handles = 1;

    return BCM_E_NONE;
}

int
e2e_dual_shaper_verify_mode(int unit, bcm_gport_t gport, int cosq, int exp_mode, int exp_weight)
{
    int                           mode, weight;
    char                          error_msg[200]={0,};


    snprintf(error_msg, sizeof(error_msg), "failed on Sched (0x%x)", gport_ucast_scheduler);
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_sched_get(unit, gport, cosq, &mode, &weight), error_msg);
    if (mode != exp_mode) {
        printf("sched mode mismatch on gport (0x%x) expected(BCM_COSQ_SP%d) retrieved (BCM_COSQ_SP%d)\n", gport, (exp_mode - BCM_COSQ_SP0), (mode - BCM_COSQ_SP0));
    }
    if (weight != exp_weight) {
        printf("sched weight mismatch on gport (0x%x) expected(%d) retrieved (%d)\n", gport, 0, weight);
    }

    return BCM_E_NONE;
}

int
e2e_dual_shaper_verify_bandwidth(int unit, bcm_gport_t gport, int cosq, int exp_rate, int margin)
{
    uint32                           kbits_sec_min, kbits_sec_max, flags;
    char                             error_msg[200]={0,};


    snprintf(error_msg, sizeof(error_msg), "failed on gport (0x%x)", gport);
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_bandwidth_get(unit, gport, cosq, &kbits_sec_min, &kbits_sec_max, &flags), error_msg);
    if ( (kbits_sec_max < (exp_rate - margin)) || (kbits_sec_max > (exp_rate + margin)) ) {
        printf("bandwidth_get mismatch on gport (0x%x) expected(%d) retrieved (%d)\n", gport, exp_rate, kbits_sec_max);
    }

    return BCM_E_NONE;
}

int
e2e_dual_shaper_verify_burst_size(int unit, bcm_gport_t gport, int cosq, int exp_burst_size, int margin)
{
    int                           rc = BCM_E_NONE;
    int                           max_burst;
    char                          error_msg[200]={0,};

    snprintf(error_msg, sizeof(error_msg), "BandwidthBurstMax on gport (0x%x) failed", gport);
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_control_get(unit, gport, cosq, bcmCosqControlBandwidthBurstMax, &max_burst), error_msg);
    if ( (max_burst < (exp_burst_size - margin)) || (max_burst > (exp_burst_size + margin)) ) {
        printf("control_get-BandwidthBurstMax mismatch on Sched (0x%x) expected(%d) retrieved (%d)\n", gport, exp_burst_size, max_burst);
    }

    return BCM_E_NONE;
}

int e2e_dual_shaper_verify(int port_idx)
{
    int                           idx = port_idx;
    int                           nbr_fq_sched, nbr_sched;
    bcm_gport_t                   gport_scheduler_fq;
    int                           exp_mode, exp_margin, exp_kbits_sec_max;
    bcm_gport_t                   gport_sched;
    bcm_cosq_gport_connection_t   connection;
    int                           cosq = 0;
    bcm_cosq_scheduler_gport_params_t params_get;
    bcm_cosq_gport_info_t gport_info;
    char error_msg[200]={0,};


    printf("nVerify Setup\n");

    BCM_IF_ERROR_RETURN_MSG(e2e_dual_shaper_verify_bandwidth(c6_unit, c6_e2e_parent_gport[idx], 0,
            C6_E2E_INTERFACE_KBITS_SEC_MAX, C6_E2E_INTERFACE_KBITS_SEC_MAX_MARGIN), "");

    BCM_IF_ERROR_RETURN_MSG(e2e_dual_shaper_verify_bandwidth(c6_unit, c6_e2e_gport[idx], 0,
            C6_E2E_PORT_KBITS_SEC_MAX, C6_E2E_PORT_KBITS_SEC_MAX_MARGIN), "");

    /* retrieve e2e port scheduling element */
    gport_info.in_gport = c6_e2e_gport[idx];
    gport_info.cosq = 0;

    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_gport_handle_get(c6_unit, bcmCosqGportTypeSched, &gport_info), "");
    c6_e2e_gport[idx] = gport_info.out_gport;
    snprintf(error_msg, sizeof(error_msg), "failed on e2e Sched (0x%x)", c6_e2e_gport[idx]);
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_scheduler_gport_get(c6_unit, 0, c6_e2e_gport[idx], &params_get), error_msg);
    if (params_get.type != C6_DEFAULT_E2E_SCHED_TYPE) {
        printf("gport get mismatch on E2E Sched (0x%x) expected type(%d) retrieved type (%d)\n", c6_e2e_gport[idx], C6_DEFAULT_E2E_SCHED_TYPE, params_get.type);
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
        snprintf(error_msg, sizeof(error_msg), "failed on Sched (0x%x)", gport_scheduler_fq);
        BCM_IF_ERROR_RETURN_MSG(bcm_cosq_scheduler_gport_get(c6_unit, 0, gport_scheduler_fq, &params_get), error_msg);
        if (params_get.type != C6_SCHED_FQ_TYPE) {
            printf("gport get mismatch on Sched (0x%x) expected type(%d) retrieved type (%d)\n", gport_scheduler_fq, C6_SCHED_FQ_TYPE, params_get.type);
        }

        /* retrieve SE scheduling discipline */
        BCM_IF_ERROR_RETURN_MSG(e2e_dual_shaper_verify_mode(c6_unit, gport_scheduler_fq, 0, exp_mode, 0), "");

        /* retrieve rate of Scheduler */
        BCM_IF_ERROR_RETURN_MSG(e2e_dual_shaper_verify_bandwidth(c6_unit, gport_scheduler_fq, 0, C6_SCHED_KBITS_SEC_MAX, C6_SCHED_KBITS_SEC_MAX_MARGIN), "");

        /* retrieve scheduler burst size */
        BCM_IF_ERROR_RETURN_MSG(e2e_dual_shaper_verify_burst_size(c6_unit, gport_scheduler_fq, 0, C6_SCHED_MAX_BURST, C6_SCHED_MAX_BURST_MARGIN), "");
    }

    /* retrieve type of intermediate scheduling element */
    snprintf(error_msg, sizeof(error_msg), "failed on Sched (0x%x)", c6_gport_ucast_scheduler[idx]);
    BCM_IF_ERROR_RETURN_MSG(bcm_cosq_scheduler_gport_get(c6_unit, 0, c6_gport_ucast_scheduler[idx], &params_get), error_msg);
    if (params_get.type != C6_SCHED_DUAL_TYPE) {
        printf("gport get mismatch on Sched (0x%x) expected type(%d) retrieved type (%d)\n", c6_gport_ucast_scheduler[idx], C6_SCHED_DUAL_TYPE, params_get.type);
    }
    if (params_get.mode != C6_SCHED_DUAL_MODE) {
        printf("gport get mismatch on Sched (0x%x) expected mode(%d) retrieved mode (%d)\n", c6_gport_ucast_scheduler[idx], C6_SCHED_DUAL_MODE, params_get.mode);
    }

    /* retrieve intermediate Scheduler elements (SE) atributes */
    for (nbr_sched = 0; nbr_sched < 2; nbr_sched++) {
        switch (nbr_sched) {
        case 0: gport_sched = c6_gport_ucast_scheduler_cir[idx];
        exp_mode = C6_SCHED_DUAL_CIR_MODE;
        exp_kbits_sec_max = C6_SCHED_DUAL_CIR_KBITS_SEC_MAX;
        exp_margin = C6_SCHED_DUAL_CIR_KBITS_SEC_MAX_MARGIN; break;
        default: gport_sched = c6_gport_ucast_scheduler_pir[idx];
        exp_mode = C6_SCHED_DUAL_EIR_MODE;
        exp_kbits_sec_max = C6_SCHED_DUAL_EIR_KBITS_SEC_MAX;
        exp_margin = C6_SCHED_DUAL_EIR_KBITS_SEC_MAX_MARGIN; break;
        }

        /* retrieve SE scheduling discipline */
        BCM_IF_ERROR_RETURN_MSG(e2e_dual_shaper_verify_mode(c6_unit, gport_sched, 0, exp_mode, 0), "");

        /* retrieve rate of Scheduler */
        BCM_IF_ERROR_RETURN_MSG(e2e_dual_shaper_verify_bandwidth(c6_unit, gport_sched, 0, exp_kbits_sec_max, exp_margin), "");

        /* retrieve scheduler burst size */
        BCM_IF_ERROR_RETURN_MSG(e2e_dual_shaper_verify_burst_size(c6_unit, gport_sched, 0, C6_SCHED_MAX_BURST, C6_SCHED_MAX_BURST_MARGIN), "");
    }


    /* Reference other scripts for verification of remaining hierarchy */

    printf("cint_dnx_e2e_dual_shaper.c (verify) completed with status (%s)\n", bcm_errmsg(rc));

    return BCM_E_NONE;
}

/* Traffic passes after this step */
e2e_dual_shaper_setup(12);

/* Traffic stops after this step */
/* e2e_dual_shaper_teardown(12); */

/* verify setup */
e2e_dual_shaper_verify(12);

