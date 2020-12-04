/*~~~~~~~~~~~~~~~~~~~~~~~~~~Cosq: E2E Scheduling~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_tm_fap_config4.c
 * Purpose: Example of Composite SE Validation
 *
 * Environment
 *    - Default Egress queue mapping in play.
 *    - Default Egress Port Scheduler hierarchy in play.
 *    - Default Ingress Scheduling Discipline (delay tolerance), queues sizes
 *    - Packet Size 1500 packet - size should be aligned with credit source settings
 *
 * - Port Scheduler
 *   - CL Scheduler Instance 0 (SP mode, Composite), Hookup both to Port Schedulers (SP0, SP2)
 *   - CL Scheduler Instance 1 (SP mode, Composite), Hookup both to Port Schedulers (SP1, SP4)
 *     - 4 FQ Schedulers
 *       Instance 0 and 1 hooked to CL Instance 0 (SP0, SP1)
 *       Instance 2 and 3 hooked to CL Instance 1 (SP0, SP1)
 *       - 1st Queue Group / Connector Group (4 elements in bundle) connected to
 *         corresponding FQ schedulers
 *       - 2nd Queue Group / Connector Group (4 elements in bundle) connected to
 *         corresponding FQ schedulers
 *
 * - test run 1
 *   - Input
 *     - Queue Group 1 - Cos 0
 *       Input 500 Mbps
 *     - Queue Group 1 - Cos 2
 *       Input 100 Mbps
 *
 *   - Expected Output
 *     - Queue Group 1 - Cos 0
 *       Expected Output 300 Mbps (CL instance 0 CIR/1st flow)
 *       Observed =>
 *       Expected Output 100 Mbps (CL instance 0 EIR/2nd flow)
 *       Observed =>
 *     - Queue Group 1 - Cos 2
 *       Expected Output 100 Mbps (CLI instance 1 CIR/1st flow)
 *       Observed =>
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  |                                                                                |
 *  |                                       |                                        |
 *  |                                      SPR (500 Mbps)                            |
 *  |                                       |                                        |
 *  |                                      \/                                        |
 *  |                               +-------------------+                            |
 *  |                              /          HR         \                           |
 *  |                             /                       \                          |
 *  |                            / SP0  SP1  SP2  SP3  SP4 \                         |
 *  |                           +---------------------------+                        |
 *  |                              /     \   /          |                            |
 *  |                             /       \ /           |                            |
 *  |                           SPR       /\           SPR                           |
 *  |                      (300 Mbps)   SPR SPR     (200 Mbps)                       |
 *  |                          / (200 Mbps) (300 Mbps)  |                            |
 *  |                         /        /      \         |                            |
 *  |                        \/       \/      \/       \/                            |
 *  |                  +--------------+       +--------------+                       |
 *  |                 /CL (4 SP levels)\     /CL (4 SP levels)\                      |
 *  |                /                  \   /                  \                     |
 *  |               / SP0  SP1  SP2  SP3 \ / SP0  SP1  SP2  SP3 \                    |
 *  |              +---------------------++----------------------+                   |
 *  |                 /     \                /     \                                 |
 *  |                /       \              /       \                                |
 *  |              SPR       SPR          SPR       SPR                              |
 *  |          (500 Mbps) (500 Mbps)  (500 Mbps) (500 Mbps)                          |
 *  |             /             \        /             \                             |
 *  |            \/             \/      \/             \/                            |
 *  |         +------+      +------+   +------+      +------+                        |
 *  |        /   FQ   \    /   FQ   \ /   FQ   \    /   FQ   \                       |
 *  |       +----------+  +----------+----------+  +----------+                      |
 *  |            |             |          |              |                           |
 *  |            |             |          |              |                           |
 *  |           SPR           SPR        SPR            SPR                          |
 *  |        (500 Mbps)    (500 Mbps) (500 Mbps)    (500 Mbps)                       |
 *  |            |             |          |              |                           |
 *  |            |             |          |              |                           |
 *  |           \/            \/         \/             \/                           |
 *  |         |     |      |     |    |     |         |     |                        |
 *  |         |     |      |     |    |     |         |     |                        |
 *  |         |     |      |     |    |     |         |     |                        |
 *  |         |     |      |     |    |     |         |     |                        |
 *  |         |     |      |     |    |     |         |     |                        |
 *  |         +-----+      +-----+    +-----+         +-----+                        |
 *  |      Input:500 Mbps          Input:100 Mbps                +----------------+  |
 *  |                                                            |      KEY       |  |
 *  |                                                            +----------------+  |
 *  |                                                            |SPR- Rate Shaper|  |
 *  |                                                            |                |  |
 *  |                                                            +----------------+  |
 *  |                     +~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                             |
 *  |                     |   Figure 8: Composite SE   |                             |
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/*
 * General Definitions
 */

int C4_PETRA_CARD_NUM_PORTS = 1;
int C4_NUM_COS = 4;

int C4_DEFAULT_E2E_SCHED_FLAGS = (BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_HR_ENHANCED);
int C4_SCHED_FLAGS = (BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE1_4SP |
                                                                 BCM_COSQ_GPORT_COMPOSITE);
int C4_NBR_SCHEDULERS = 2;
int C4_NBR_SCHEDULER_FLOWS = 2;
int C4_SCHEDULER0_CIR_MODE = BCM_COSQ_SP0;
int C4_SCHEDULER0_EIR_MODE = BCM_COSQ_SP2;
int C4_SCHEDULER1_CIR_MODE = BCM_COSQ_SP1;
int C4_SCHEDULER1_EIR_MODE = BCM_COSQ_SP4;
int C4_SCHED_CIR_KBITS_SEC_MAX = 300000; /* 300 Mbps */
int C4_SCHED_CIR_KBITS_SEC_MAX_MARGIN = 10; /* 10 Kbps */
int C4_SCHED_EIR_KBITS_SEC_MAX = 200000; /* 200 Mbps */
int C4_SCHED_EIR_KBITS_SEC_MAX_MARGIN = 2600; /* 2600 Kbps */
int C4_SCHED_KBITS_SEC_MAX = 500000; /* 500 Mbps */

int C4_NBR_QUEUE_GROUPS = 2;
int C4_NBR_FQ_SCHEDULERS = 4;


/* handles */
int c4_my_modid = 0;


bcm_gport_t c4_gport_ucast_voq_connector_group0[32];
bcm_gport_t c4_gport_ucast_voq_connector_group1[32];
bcm_gport_t c4_gport_ucast_queue_group0[32];
bcm_gport_t c4_gport_ucast_queue_group1[32];
bcm_gport_t c4_gport_ucast_scheduler[32];
bcm_gport_t c4_gport_ucast_scheduler_sf2[32]; /* sub-flow two */
bcm_gport_t c4_gport_ucast_scheduler1[32];
bcm_gport_t c4_gport_ucast_scheduler1_sf2[32]; /* sub-flow two */
bcm_gport_t c4_gport_ucast_scheduler_fq0[32];
bcm_gport_t c4_gport_ucast_scheduler_fq1[32];
bcm_gport_t c4_gport_ucast_scheduler_fq2[32];
bcm_gport_t c4_gport_ucast_scheduler_fq3[32];
bcm_gport_t c4_e2e_gport[32];
bcm_gport_t c4_e2e_parent_gport[32];

int c4_use_same_handles = 0;


void e2e_composite_se_display_handles(int idx)
{
    printf("  e2eGport(0x%x)\n", c4_e2e_gport[idx]);
    printf("    sched0Gport(0x%x), sched0Sf2(0x%x) sched1Gport(0x%x), sched1Sf2(0x%x)\n",
            c4_gport_ucast_scheduler[idx], c4_gport_ucast_scheduler_sf2[idx],
            c4_gport_ucast_scheduler1[idx], c4_gport_ucast_scheduler1_sf2[idx]);
    printf("    l3_fq0(0x%x) l3_fq1(0x%x) l3_fq2(0x%x) l3_fq3(0x%x)\n",
            c4_gport_ucast_scheduler_fq0[idx], c4_gport_ucast_scheduler_fq1[idx],
            c4_gport_ucast_scheduler_fq2[idx], c4_gport_ucast_scheduler_fq3[idx]);
    printf("    connGrp0Gport(0x%x) connGrp1Gport(0x%x)\n",
            c4_gport_ucast_voq_connector_group0[idx], c4_gport_ucast_voq_connector_group1[idx]);
    printf("    ucastGrp0Gport(0x%x) ucastGrp1Gport(0x%x)\n",
            c4_gport_ucast_queue_group0[idx], c4_gport_ucast_queue_group1[idx]);
}


/* Added parameter: kbits_sec_max_port identify maximum bandwidth for the Interface and port. */
/* One example of running (see Negev UM for figure description is by running: main(500000)) */
int e2e_composite_se_setup(int unit, int kbits_sec_max_port, int port_idx)
{
    int idx, port, system_port;
    bcm_gport_t dest_gport[32];
    bcm_gport_t sys_gport[32];
    int num_cos = 4;
    int flags = 0, default_flags = 0;
    bcm_error_t rv = BCM_E_NONE;
    int voq;
    bcm_cosq_gport_connection_t connection;
    int max_burst;
    int cosq = 0;
    bcm_cosq_delay_tolerance_t delay_tolerance;
    int kbits_sec_max;
    bcm_cosq_voq_connector_gport_t config;
    uint32 dummy_flags ;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info ;
/*
    rv = bcm_stk_my_modid_set(unit, c4_my_modid);
    if (rv != BCM_E_NONE) {
    printf("bcm_stk_my_modid_set failed $rv\n");
      return rv;
    }
*/

    printf("\nSetup\n");

    int is_dnx;
    bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    int C4_SCHED_MAX_BURST = 3000;
    int C4_SCHED_MAX_BURST_MARGIN = 75;
    if (is_dnx) {
        /* Default credit size for DPP was 1K.
         * In order to utilize the cint also for DNX device
         * where credit size by default is 4K
         * we multiply by 4 the MaxBurst
         */
        int credit_worth = *(dnxc_data_get(unit, "iqs", "credit", "worth", NULL));
        C4_SCHED_MAX_BURST *= (credit_worth / 1024);
        C4_SCHED_MAX_BURST_MARGIN *= (credit_worth / 1024);

        /* Some of the shapers of the DNX device's shapers are with bigger granularity because of their higher BW.
           That's why in order for the test to pass the rates are increased */
        C4_SCHED_CIR_KBITS_SEC_MAX *= 10;
        C4_SCHED_EIR_KBITS_SEC_MAX *= 10;
        C4_SCHED_KBITS_SEC_MAX *= 10;

    }


    if (c4_use_same_handles == 1) {
        default_flags = BCM_COSQ_GPORT_WITH_ID;
        e2e_composite_se_display_handles(0);
    }

    /* for (idx=0; idx<C4_PETRA_CARD_NUM_PORTS; idx++) { */
    /*idx = 0;*/
    idx = port_idx;

    port = idx + 1;
    system_port = idx + 1;
    BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport[idx], system_port);
    BCM_GPORT_LOCAL_SET(dest_gport[idx], port);

    rv = bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_get scheduler failed $rv\n");
        return rv;
    }

    rv = bcm_stk_sysport_gport_set(unit, sys_gport[idx], dest_gport[idx]);
    if (rv != BCM_E_NONE) {
        printf("bcm_stk_sysport_gport_set failed $rv\n");
        return rv;
    }

    /*
     * create CL scheduler (Instance 0)
     */
    flags = C4_SCHED_FLAGS | default_flags;

    rv = bcm_cosq_gport_add(unit, dest_gport[idx], 1,
                 flags, &c4_gport_ucast_scheduler[idx]);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return rv;
    }


    /* COS 0, CL scheduler (Instance 0) scheduling discipline configuration */
    rv = bcm_cosq_gport_sched_set(unit,
                      c4_gport_ucast_scheduler[idx],
                      0,
                      C4_SCHEDULER0_CIR_MODE,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    BCM_COSQ_GPORT_E2E_PORT_SET(c4_e2e_gport[idx], port);

    /* CL scheduler (instance 0) attach to E2E port scheduler */
    rv = bcm_cosq_gport_attach(unit, c4_e2e_gport[idx],
                 c4_gport_ucast_scheduler[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }

    /* CL (sf2) setup */
    BCM_COSQ_GPORT_COMPOSITE_SF2_CORE_SET(c4_gport_ucast_scheduler_sf2[idx],c4_gport_ucast_scheduler[idx], mapping_info.core);
    /* COS 0, CL scheduler instance 0 (sf2) scheduling discipline configuration */
    rv = bcm_cosq_gport_sched_set(unit,
                      c4_gport_ucast_scheduler_sf2[idx],
                      0,
                      C4_SCHEDULER0_EIR_MODE,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    /* CL scheduler instance 0 (sf2) attach to E2E port scheduler */
    rv = bcm_cosq_gport_attach(unit, c4_e2e_gport[idx],
                 c4_gport_ucast_scheduler_sf2[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }


    /*
     * create CL scheduler (Instance 1)
     */
    flags = C4_SCHED_FLAGS | default_flags;

    rv = bcm_cosq_gport_add(unit, dest_gport[idx], 1,
                 flags, &c4_gport_ucast_scheduler1[idx]);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return rv;
    }


    /* COS 0, CL scheduler instance 1 scheduling discipline configuration */
    rv = bcm_cosq_gport_sched_set(unit,
                      c4_gport_ucast_scheduler1[idx],
                      0,
                      C4_SCHEDULER1_CIR_MODE,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    BCM_COSQ_GPORT_E2E_PORT_SET(c4_e2e_gport[idx], port);

    /* CL scheduler instance 1 attach to E2E port scheduler */
    rv = bcm_cosq_gport_attach(unit, c4_e2e_gport[idx],
                 c4_gport_ucast_scheduler1[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }

    /* CL instance 1 (sf2) setup */
    BCM_COSQ_GPORT_COMPOSITE_SF2_CORE_SET(c4_gport_ucast_scheduler1_sf2[idx],c4_gport_ucast_scheduler1[idx], mapping_info.core);
    /* COS 0, CL scheduler instance 1 (sf2) scheduling discipline configuration */
    rv = bcm_cosq_gport_sched_set(unit,
                      c4_gport_ucast_scheduler1_sf2[idx],
                      0,
                      C4_SCHEDULER1_EIR_MODE,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    /* CL scheduler instance 1 (sf2) attach to E2E port scheduler */
    rv = bcm_cosq_gport_attach(unit, c4_e2e_gport[idx],
                 c4_gport_ucast_scheduler1_sf2[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }


    /* create scheduler FQ scheduler (instance 0) - will aggregrate all cos 0 queues */
    flags = BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_FQ | default_flags;

    rv = bcm_cosq_gport_add(unit, dest_gport[idx], 1,
                flags, &c4_gport_ucast_scheduler_fq0[idx]);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return rv;
    }

    /* COS 0, FQ scheduler (instance 0) scheduling discipline configuration */
    rv = bcm_cosq_gport_sched_set(unit,
                      c4_gport_ucast_scheduler_fq0[idx],
                      0,
                      BCM_COSQ_SP0,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    /* FQ scheduler (instance 0) attach to CL scheduler */
    rv = bcm_cosq_gport_attach(unit, c4_gport_ucast_scheduler[idx],
                 c4_gport_ucast_scheduler_fq0[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }


    /* create scheduler FQ scheduler (instance 1) - will aggregrate all cos 1 queues */
    flags = BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_FQ | default_flags;

    rv = bcm_cosq_gport_add(unit, dest_gport[idx], 1,
                 flags, &c4_gport_ucast_scheduler_fq1[idx]);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return rv;
    }

    /* COS 0, FQ scheduler (instance 1) scheduling discipline configuration */
    rv = bcm_cosq_gport_sched_set(unit,
                      c4_gport_ucast_scheduler_fq1[idx],
                      0,
                      BCM_COSQ_SP1,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }


    /* FQ scheduler (instance 1) attach to CL scheduler */
    rv = bcm_cosq_gport_attach(unit, c4_gport_ucast_scheduler[idx],
                 c4_gport_ucast_scheduler_fq1[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }


    /* create scheduler FQ scheduler (instance 2) - will aggregrate all cos 2 queues */
    flags = BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_FQ | default_flags;

    rv = bcm_cosq_gport_add(unit, dest_gport[idx], 1,
                flags, &c4_gport_ucast_scheduler_fq2[idx]);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return rv;
    }

    /* COS 0, FQ scheduler (instance 2) scheduling discipline configuration */
    rv = bcm_cosq_gport_sched_set(unit,
                      c4_gport_ucast_scheduler_fq2[idx],
                      0,
                      BCM_COSQ_SP0,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }


        /* FQ scheduler (instance 2) attach to CL scheduler */
    rv = bcm_cosq_gport_attach(unit, c4_gport_ucast_scheduler1[idx],
                 c4_gport_ucast_scheduler_fq2[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }


    /* create scheduler FQ scheduler (instance 3) - will aggregrate all cos 3 queues */
    flags = BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_FQ | default_flags;

    rv = bcm_cosq_gport_add(unit, dest_gport[idx], 1,
                flags, &c4_gport_ucast_scheduler_fq3[idx]);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return rv;
    }

    /* COS 0, FQ scheduler (instance 3) scheduling discipline configuration */
    rv = bcm_cosq_gport_sched_set(unit,
                      c4_gport_ucast_scheduler_fq3[idx],
                      0,
                      BCM_COSQ_SP1,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }


    /* FQ scheduler (instance 3) attach to CL scheduler */
    rv = bcm_cosq_gport_attach(unit, c4_gport_ucast_scheduler1[idx],
                   c4_gport_ucast_scheduler_fq3[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }


    /* create voq connector bundle, 4 elements in bundle */
    BCM_GPORT_LOCAL_SET(config.port, port);
    config.flags = BCM_COSQ_GPORT_VOQ_CONNECTOR | default_flags;
    config.numq = num_cos;
    config.remote_modid = c4_my_modid + mapping_info.core;
    config.nof_remote_cores = 1;
    rv = bcm_cosq_voq_connector_gport_add(unit, &config, &c4_gport_ucast_voq_connector_group0[idx]);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add connector failed $rv\n");
        return rv;
    }

    /* COS 0 (bundle 1st element), scheduling discipline setup */
    rv = bcm_cosq_gport_sched_set(unit,
                      c4_gport_ucast_voq_connector_group0[idx],
                      0,
                      BCM_COSQ_SP0,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    /* COS 0 (bundle 1st element), attach to FQ scheduler (instance 0) */
    rv = bcm_cosq_gport_attach(unit, c4_gport_ucast_scheduler_fq0[idx],
               c4_gport_ucast_voq_connector_group0[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }


    /* COS 1, (bundle 2nd element), scheduling discipline setup */
    rv = bcm_cosq_gport_sched_set(unit,
                      c4_gport_ucast_voq_connector_group0[idx],
                      1,
                      BCM_COSQ_SP0,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    /* COS 1 (bundle 2nd element), attach to FQ scheduler (instance 1) */
    rv = bcm_cosq_gport_attach(unit, c4_gport_ucast_scheduler_fq1[idx],
               c4_gport_ucast_voq_connector_group0[idx], 1);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }


    /* COS 2, (bundle 3nd element), scheduling discipline setup */
    rv = bcm_cosq_gport_sched_set(unit,
                      c4_gport_ucast_voq_connector_group0[idx],
                      2,
                      BCM_COSQ_SP0,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    /* COS 2 (bundle 3rd element), attach to FQ scheduler (instance 2) */
    rv = bcm_cosq_gport_attach(unit, c4_gport_ucast_scheduler_fq2[idx],
               c4_gport_ucast_voq_connector_group0[idx], 2);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }

    /* COS 3, (bundle 4th element), scheduling discipline setup */
    rv = bcm_cosq_gport_sched_set(unit,
                      c4_gport_ucast_voq_connector_group0[idx],
                      3,
                      BCM_COSQ_SP0,
                      0);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    /* COS 3 (bundle 4th element), attach to FQ scheduler (instance 3) */
    rv = bcm_cosq_gport_attach(unit, c4_gport_ucast_scheduler_fq3[idx],
               c4_gport_ucast_voq_connector_group0[idx], 3);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }


    /* create queue group bundle, 4 elements in bundle */
    flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | default_flags;
    rv = bcm_cosq_gport_add(unit, sys_gport[idx], num_cos,
                flags, &c4_gport_ucast_queue_group0[idx]);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add queue failed $rv\n");
        return rv;
    }
    printf("ucast gport(0x%08x)\n", c4_gport_ucast_queue_group0[idx]);


    /* connect VoQ to flow */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
    connection.remote_modid = c4_my_modid  + mapping_info.core;
    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET( c4_gport_ucast_queue_group0[idx], mapping_info.core, BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(c4_gport_ucast_queue_group0[idx]));
    connection.voq = c4_gport_ucast_queue_group0[idx];
    connection.voq_connector = c4_gport_ucast_voq_connector_group0[idx];

    rv = bcm_cosq_gport_connection_set(unit, &connection);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_connection_set ingress failed $rv\n");
        return rv;
    }

    /* connect flow to VoQ */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
    connection.remote_modid = c4_my_modid + mapping_info.core;
    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET( c4_gport_ucast_queue_group0[idx], mapping_info.core, BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(c4_gport_ucast_queue_group0[idx]));
    connection.voq = c4_gport_ucast_queue_group0[idx];
    connection.voq_connector = c4_gport_ucast_voq_connector_group0[idx];

    rv = bcm_cosq_gport_connection_set(unit, &connection);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_connection_set egress failed $rv\n");
        return rv;
    }

    /* configuration for another queue group */
    /* create voq connector bundle, 4 elements in the bundle */
    BCM_GPORT_LOCAL_SET(config.port, port);
    config.flags = BCM_COSQ_GPORT_VOQ_CONNECTOR | default_flags;
    config.numq = num_cos;
    config.remote_modid = c4_my_modid + mapping_info.core;
    config.nof_remote_cores = 1;
    rv = bcm_cosq_voq_connector_gport_add(unit, &config, &c4_gport_ucast_voq_connector_group1[idx]);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add connector failed $rv\n");
        return rv;
    }

    /* COS 0 (bundle 1st element), scheduling discipline setup */
    rv = bcm_cosq_gport_sched_set(unit,
                      c4_gport_ucast_voq_connector_group1[idx],
                      0,
                      BCM_COSQ_SP0,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    /* COS 0 (bundle 1st element), attach to FQ scheduler (instance 0) */
    rv = bcm_cosq_gport_attach(unit, c4_gport_ucast_scheduler_fq0[idx],
               c4_gport_ucast_voq_connector_group1[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }

    /* COS 1 (bundle 2nd element), scheduling discipline setup */
    rv = bcm_cosq_gport_sched_set(unit,
                      c4_gport_ucast_voq_connector_group1[idx],
                      1,
                      BCM_COSQ_SP0,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    /* COS 1 (bundle 2nd element), attach to FQ scheduler (instance 1) */
    rv = bcm_cosq_gport_attach(unit, c4_gport_ucast_scheduler_fq1[idx],
               c4_gport_ucast_voq_connector_group1[idx], 1);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }

    /* COS 2 (bundle 3rd element), scheduling discipline setup */
    rv = bcm_cosq_gport_sched_set(unit,
                      c4_gport_ucast_voq_connector_group1[idx],
                      2,
                      BCM_COSQ_SP0,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    /* COS 2 (bundle 3rd element), attach to FQ scheduler (instance 2) */
    rv = bcm_cosq_gport_attach(unit, c4_gport_ucast_scheduler_fq2[idx],
               c4_gport_ucast_voq_connector_group1[idx], 2);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }

    /* COS 3 (bundle 4th element), scheduling discipline setup */
    rv = bcm_cosq_gport_sched_set(unit,
                      c4_gport_ucast_voq_connector_group1[idx],
                      3,
                      BCM_COSQ_SP0,
                      0);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    /* COS 3 (bundle 4th element), attach to FQ scheduler (instance 3) */
    rv = bcm_cosq_gport_attach(unit, c4_gport_ucast_scheduler_fq3[idx],
               c4_gport_ucast_voq_connector_group1[idx], 3);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }

    /* create queue group bundle, 4 elements in bundle */
    flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | BCM_COSQ_GPORT_TM_FLOW_ID | default_flags;
    rv = bcm_cosq_gport_add(unit, sys_gport[idx], num_cos,
                flags, &c4_gport_ucast_queue_group1[idx]);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add queue failed $rv\n");
        return rv;
    }
    printf("ucast gport(0x%08x)\n", c4_gport_ucast_queue_group1[idx]);

    /* connect VoQ to flow */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
    connection.remote_modid = c4_my_modid + mapping_info.core;
    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET( c4_gport_ucast_queue_group1[idx], mapping_info.core, BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(c4_gport_ucast_queue_group1[idx]));
    connection.voq = c4_gport_ucast_queue_group1[idx];
    connection.voq_connector = c4_gport_ucast_voq_connector_group1[idx];

    rv = bcm_cosq_gport_connection_set(unit, &connection);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_connection_set ingress failed $rv\n");
        return rv;
    }

    /* connect flow to VoQ */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
    connection.remote_modid = c4_my_modid + mapping_info.core;
    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(c4_gport_ucast_queue_group1[idx], mapping_info.core, BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(c4_gport_ucast_queue_group1[idx]));
    connection.voq = c4_gport_ucast_queue_group1[idx];
    connection.voq_connector = c4_gport_ucast_voq_connector_group1[idx];

    rv = bcm_cosq_gport_connection_set(unit, &connection);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_connection_set egress failed $rv\n");
        return rv;
    }

    /*
     * Set rate on the E2E Interface
     */

    BCM_COSQ_GPORT_E2E_PORT_SET(c4_e2e_gport[idx], port);

    rv = bcm_fabric_port_get(unit,
                     c4_e2e_gport[idx],
                     0,
                     &c4_e2e_parent_gport[idx]);
    if (rv != BCM_E_NONE) {
        printf("bcm_fabric_port_get failed $rv\n");
        return rv;
    }

    rv = bcm_cosq_gport_bandwidth_set(unit,
                      c4_e2e_parent_gport[idx],
                      0,
                      0,
                      kbits_sec_max_port,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set if failed $rv\n");
        return rv;
    }

    /*
     * Set rate on the E2E port
     */

    BCM_COSQ_GPORT_E2E_PORT_SET(c4_e2e_gport[idx], port);

    rv = bcm_cosq_gport_bandwidth_set(unit,
                      c4_e2e_gport[idx],
                      0,
                      0,
                      kbits_sec_max_port,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set failed $rv\n");
        return rv;
    }

    /*
     * bandwidth set for CL instance 0 (PIR) flow
     */
    kbits_sec_max = C4_SCHED_CIR_KBITS_SEC_MAX;
    max_burst = C4_SCHED_MAX_BURST;
    rv = bcm_cosq_gport_bandwidth_set(unit,
                                          c4_gport_ucast_scheduler[idx],
                                          0,
                                          0,
                                          kbits_sec_max,
                                          0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set connector failed $rv\n");
        return rv;
    }

    /* Set max burst on the CL instance 0 Scheduler - PIR flow */
    rv = bcm_cosq_control_set(unit,
                      c4_gport_ucast_scheduler[idx],
                      0,
                      bcmCosqControlBandwidthBurstMax,
                      max_burst);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_control_set max burst sched failed $rv\n");
        return rv;
    }

    /*
     * bandwidth set for CL instance 0 (EIR) flow
     */
    kbits_sec_max = C4_SCHED_EIR_KBITS_SEC_MAX;
    max_burst = C4_SCHED_MAX_BURST;
    rv = bcm_cosq_gport_bandwidth_set(unit,
                                          c4_gport_ucast_scheduler_sf2[idx],
                                          0,
                                          0,
                                          kbits_sec_max,
                                          0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set connector failed $rv\n");
        return rv;
    }

    /* Set max burst on the CL instance 0 Scheduler - EIR flow */
    rv = bcm_cosq_control_set(unit,
                      c4_gport_ucast_scheduler_sf2[idx],
                      0,
                      bcmCosqControlBandwidthBurstMax,
                      max_burst);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_control_set max burst sched failed $rv\n");
        return rv;
    }

    /*
     * bandwidth set for CL instance 1 (PIR) flow
     */
    kbits_sec_max = C4_SCHED_CIR_KBITS_SEC_MAX;
    max_burst = C4_SCHED_MAX_BURST;
    rv = bcm_cosq_gport_bandwidth_set(unit,
                                          c4_gport_ucast_scheduler1[idx],
                                          0,
                                          0,
                                          kbits_sec_max,
                                          0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set connector failed $rv\n");
        return rv;
    }

    /* Set max burst on the CL instance 0 Scheduler - PIR flow */
    rv = bcm_cosq_control_set(unit,
                      c4_gport_ucast_scheduler1[idx],
                      0,
                      bcmCosqControlBandwidthBurstMax,
                      max_burst);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_control_set max burst sched failed $rv\n");
        return rv;
    }


    /*
     * bandwidth set for CL instance 0 (EIR) flow
     */
    kbits_sec_max = C4_SCHED_EIR_KBITS_SEC_MAX;
    max_burst = C4_SCHED_MAX_BURST;
    rv = bcm_cosq_gport_bandwidth_set(unit,
                                          c4_gport_ucast_scheduler1_sf2[idx],
                                          0,
                                          0,
                                          kbits_sec_max,
                                          0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set connector failed $rv\n");
        return rv;
    }


    /* Set max burst on the CL instance 0 Scheduler - EIR flow */
    rv = bcm_cosq_control_set(unit,
                      c4_gport_ucast_scheduler1_sf2[idx],
                      0,
                      bcmCosqControlBandwidthBurstMax,
                      max_burst);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_control_set max burst sched failed $rv\n");
        return rv;
    }

    /*
     * FQ scheduler (instance 0)
     */
    kbits_sec_max = C4_SCHED_KBITS_SEC_MAX; /* 500 Mbps */
    max_burst =  C4_SCHED_MAX_BURST;
    rv = bcm_cosq_gport_bandwidth_set(unit,
                                      c4_gport_ucast_scheduler_fq0[idx],
                                      0,
                                      0,
                                      kbits_sec_max,
                                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set sched failed $rv\n");
        return(rv);
    }

    rv = bcm_cosq_control_set(unit,
                              c4_gport_ucast_scheduler_fq0[idx],
                              0,
                              bcmCosqControlBandwidthBurstMax,
                              max_burst);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_control_set max burst sched failed $rv\n");
        return(rv);
    }

    /*
     * FQ scheduler (instance 1)
     */
    kbits_sec_max = C4_SCHED_KBITS_SEC_MAX; /* 500 Mbps */
    max_burst =  C4_SCHED_MAX_BURST;
    rv = bcm_cosq_gport_bandwidth_set(unit,
                                      c4_gport_ucast_scheduler_fq1[idx],
                                      0,
                                      0,
                                      kbits_sec_max,
                                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set sched failed $rv\n");
        return(rv);
    }

    rv = bcm_cosq_control_set(unit,
                              c4_gport_ucast_scheduler_fq1[idx],
                              0,
                              bcmCosqControlBandwidthBurstMax,
                              max_burst);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_control_set max burst sched failed $rv\n");
        return(rv);
    }

    /*
     * FQ scheduler (instance 2)
     */
    kbits_sec_max = C4_SCHED_KBITS_SEC_MAX; /* 500 Mbps */
    max_burst =  C4_SCHED_MAX_BURST;
    rv = bcm_cosq_gport_bandwidth_set(unit,
                                      c4_gport_ucast_scheduler_fq2[idx],
                                      0,
                                      0,
                                      kbits_sec_max,
                                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set sched failed $rv\n");
        return(rv);
    }

    rv = bcm_cosq_control_set(unit,
                              c4_gport_ucast_scheduler_fq2[idx],
                              0,
                              bcmCosqControlBandwidthBurstMax,
                              max_burst);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_control_set max burst sched failed $rv\n");
        return(rv);
    }

    /*
     * FQ scheduler (instance 3)
     */
    kbits_sec_max = C4_SCHED_KBITS_SEC_MAX; /* 500 Mbps */
    max_burst =  C4_SCHED_MAX_BURST;
    rv = bcm_cosq_gport_bandwidth_set(unit,
                                      c4_gport_ucast_scheduler_fq3[idx],
                                      0,
                                      0,
                                      kbits_sec_max,
                                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set sched failed $rv\n");
        return(rv);
    }

    rv = bcm_cosq_control_set(unit,
                              c4_gport_ucast_scheduler_fq3[idx],
                              0,
                              bcmCosqControlBandwidthBurstMax,
                              max_burst);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_control_set max burst sched failed $rv\n");
        return(rv);
    }

    /*
     * Connector Group 0
     */
    kbits_sec_max = C4_SCHED_KBITS_SEC_MAX; /* 500 Mbps */
    max_burst =  C4_SCHED_MAX_BURST;
    for (cosq=0; cosq<4; cosq++) {
        /* Set rate on the Connector flow */
        rv = bcm_cosq_gport_bandwidth_set(unit,
                          c4_gport_ucast_voq_connector_group0[idx],
                          cosq,
                          0,
                          kbits_sec_max,
                          0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_bandwidth_set connector failed $rv\n");
            return rv;
        }

        /* Set maxburst on the Connector flow */
        rv = bcm_cosq_control_set(unit,
                          c4_gport_ucast_voq_connector_group0[idx],
                          cosq,
                          bcmCosqControlBandwidthBurstMax,
                          max_burst);

        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_control_set connector maxburst failed $rv\n");
            return rv;
        }
    }

    /*
     * Connector Group 1
     */
    kbits_sec_max = C4_SCHED_KBITS_SEC_MAX; /* 500 Mbps */
    max_burst =  C4_SCHED_MAX_BURST;
    for (cosq=0; cosq<4; cosq++) {
        /* Set rate on the Connector flow */
        rv = bcm_cosq_gport_bandwidth_set(unit,
                          c4_gport_ucast_voq_connector_group1[idx],
                          cosq,
                          0,
                          kbits_sec_max,
                          0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_bandwidth_set connector failed $rv\n");
            return rv;
        }

        /* Set maxburst on the Connector flow */
        rv = bcm_cosq_control_set(unit,
                          c4_gport_ucast_voq_connector_group1[idx],
                          cosq,
                          bcmCosqControlBandwidthBurstMax,
                          max_burst);

        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_control_set connector maxburst failed $rv\n");
            return rv;
        }
    }

    if (c4_use_same_handles == 0) {
        e2e_composite_se_display_handles(idx);
    }

    printf("port(%d) sysport(%d) ucast gport(0x%08x)\n", port, system_port, c4_gport_ucast_queue_group0[idx]);
    /* } */
    printf("cint_e2e_composite_se.c completed with status (%s)\n", bcm_errmsg(rv));
    return rv;
}

int e2e_composite_se_teardown(int unit, int port_idx)
{
    int                           rc = BCM_E_NONE;
    int                           idx = port_idx;
    bcm_gport_t                   gport_ucast_voq_connector_group;
    bcm_gport_t                   gport_ucast_queue_group;
    bcm_gport_t                   gport_scheduler_fq;
    bcm_gport_t                   gport_ucast_scheduler;
    int                           nbr_queue_groups, nbr_fq_sched, nbr_sched, nbr_sched_flw;
    bcm_cosq_gport_connection_t   connection;
    int                           cosq = 0;
    uint32 dummy_flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;

    int C4_NBR_SCHEDULER_FLOWS = 2;
    printf("\nTeardown\n");

    e2e_composite_se_display_handles(idx);

    rc = bcm_port_get(unit, port_idx, &dummy_flags, &interface_info, &mapping_info);
    if (rc != BCM_E_NONE) {
        printf("bcm_port_get scheduler failed $rc\n");
        return rc;
    }

    /* detach scheduler sub-tree from E2E hierarchy */
    for (nbr_sched = 0; nbr_sched < C4_NBR_SCHEDULERS; nbr_sched++) {
        switch(nbr_sched) {
            case 0:
                for (nbr_sched_flw = 0; nbr_sched_flw < C4_NBR_SCHEDULER_FLOWS; nbr_sched_flw++) {
                    switch(nbr_sched_flw) {
                        case 0: gport_ucast_scheduler = c4_gport_ucast_scheduler[idx]; break;
                        default: gport_ucast_scheduler = c4_gport_ucast_scheduler_sf2[idx]; break;
                    }

                    rc = bcm_cosq_gport_detach(unit, c4_e2e_gport[idx], gport_ucast_scheduler, 0);
                    if (BCM_FAILURE(rc)) {
                        printf("detach for SE(0x%x) failed, Error (%d, 0x%x)\n", gport_ucast_scheduler, rc, rc);
                        return(rc);
                    }
                    printf("  detached schedGport(0x%x)\n", gport_ucast_scheduler);
                }
                break;

            default:
                for (nbr_sched_flw = 0; nbr_sched_flw < C4_NBR_SCHEDULER_FLOWS; nbr_sched_flw++) {
                    switch(nbr_sched_flw) {
                        case 0: gport_ucast_scheduler = c4_gport_ucast_scheduler1[idx]; break;
                        default: gport_ucast_scheduler = c4_gport_ucast_scheduler1_sf2[idx]; break;
                    }

                    rc = bcm_cosq_gport_detach(unit, c4_e2e_gport[idx], gport_ucast_scheduler, 0);
                    if (BCM_FAILURE(rc)) {
                        printf("detach for SE(0x%x) failed, Error (%d, 0x%x)\n", gport_ucast_scheduler, rc, rc);
                        return(rc);
                    }
                    printf("  detached schedGport(0x%x)\n", gport_ucast_scheduler);
                }
                break;
        }
    }

    /* dis-associate queue from connectors */
    for (nbr_queue_groups = 0; nbr_queue_groups < C4_NBR_QUEUE_GROUPS; nbr_queue_groups++) {
        switch (nbr_queue_groups) {
            case 0: gport_ucast_queue_group = c4_gport_ucast_queue_group0[idx];
                    gport_ucast_voq_connector_group = c4_gport_ucast_voq_connector_group0[idx]; break;
            default: gport_ucast_queue_group = c4_gport_ucast_queue_group1[idx];
                     gport_ucast_voq_connector_group = c4_gport_ucast_voq_connector_group1[idx]; break;
        }

        connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
        connection.remote_modid = c4_my_modid + mapping_info.core;
        connection.voq = gport_ucast_queue_group;
        connection.voq_connector = gport_ucast_voq_connector_group;

        rc = bcm_cosq_gport_connection_set(unit, &connection);
        if (BCM_FAILURE(rc)) {
            printf("resetting queue2connector association failed, queue(0x%x) Error (%d, 0x%x)\n", gport_ucast_queue_group, rc, rc);
            return(rc);
        }
        printf("  disconnected (Ingress) ucastGport(0x%x) from connGport(0x%x)\n", gport_ucast_queue_group, gport_ucast_voq_connector_group);
    }

    /* dis-associate connectors from queues */
    for (nbr_queue_groups = 0; nbr_queue_groups < C4_NBR_QUEUE_GROUPS; nbr_queue_groups++) {
        switch (nbr_queue_groups) {
            case 0: gport_ucast_queue_group = c4_gport_ucast_queue_group0[idx];
                    gport_ucast_voq_connector_group = c4_gport_ucast_voq_connector_group0[idx]; break;
            default: gport_ucast_queue_group = c4_gport_ucast_queue_group1[idx];
                     gport_ucast_voq_connector_group = c4_gport_ucast_voq_connector_group1[idx]; break;
        }

        connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
        connection.remote_modid = c4_my_modid + mapping_info.core;
        connection.voq = gport_ucast_queue_group;
        connection.voq_connector = gport_ucast_voq_connector_group;

        rc = bcm_cosq_gport_connection_set(unit, &connection);
        if (BCM_FAILURE(rc)) {
            printf("resetting connector2queue association failed, Connector(0x%x) Error (%d, 0x%x)\n", gport_ucast_voq_connector_group, rc, rc);
            return(rc);
        }
        printf("  disconnected (Engress) connGport(0x%x) from ucastGport(0x%x)\n", gport_ucast_voq_connector_group, gport_ucast_queue_group);
    }

    /* delete queues */
    for (nbr_queue_groups = 0; nbr_queue_groups < C4_NBR_QUEUE_GROUPS; nbr_queue_groups++) {
        switch (nbr_queue_groups) {
            case 0: gport_ucast_queue_group = c4_gport_ucast_queue_group0[idx]; break;
            default: gport_ucast_queue_group = c4_gport_ucast_queue_group1[idx]; break;
        }

        rc = bcm_cosq_gport_delete(unit, gport_ucast_queue_group);
        if (BCM_FAILURE(rc)) {
            printf("delete for QueueGroup(0x%x) failed, Error (%d, 0x%x)\n", gport_ucast_queue_group, rc, rc);
            return(rc);
        }
        printf("  deleted ucastGport(0x%x)\n", gport_ucast_queue_group);
    }

    /* detach connectors */
    for (nbr_queue_groups = 0; nbr_queue_groups < C4_NBR_QUEUE_GROUPS; nbr_queue_groups++) {
        switch (nbr_queue_groups) {
            case 0: gport_ucast_voq_connector_group = c4_gport_ucast_voq_connector_group0[idx]; break;
            default: gport_ucast_voq_connector_group = c4_gport_ucast_voq_connector_group1[idx]; break;
        }

        for (cosq = 0; cosq < 4; cosq++) {
            switch (cosq) {
                case 0: gport_scheduler_fq = c4_gport_ucast_scheduler_fq0[idx]; break;
                case 1: gport_scheduler_fq = c4_gport_ucast_scheduler_fq1[idx]; break;
                case 2: gport_scheduler_fq = c4_gport_ucast_scheduler_fq2[idx]; break;
                default: gport_scheduler_fq = c4_gport_ucast_scheduler_fq3[idx]; break;
            }

            rc = bcm_cosq_gport_detach(unit, gport_scheduler_fq, gport_ucast_voq_connector_group, cosq);
            if (BCM_FAILURE(rc)) {
                printf("detach for Connector:cosq(0x%x:%d) from Sched(0x%x) failed, Error (%d, 0x%x)\n", gport_ucast_voq_connector_group, cosq, gport_scheduler_fq, rc, rc);
                return(rc);
            }
            printf("  detached connGport:cosq(0x%x:%d) from schedGport(0x%x)\n", gport_ucast_voq_connector_group, cosq, gport_scheduler_fq);
        }
    }

    /* delete connectors */
    for (nbr_queue_groups = 0; nbr_queue_groups < C4_NBR_QUEUE_GROUPS; nbr_queue_groups++) {
        switch (nbr_queue_groups) {
            case 0: gport_ucast_voq_connector_group = c4_gport_ucast_voq_connector_group0[idx]; break;
            default: gport_ucast_voq_connector_group = c4_gport_ucast_voq_connector_group1[idx]; break;
        }

        /* delete connectors */
        rc = bcm_cosq_gport_delete(unit, gport_ucast_voq_connector_group);
        if (BCM_FAILURE(rc)) {
            printf("delete for ConnectorGroup(0x%x) failed, Error (%d, 0x%x)\n", gport_ucast_voq_connector_group, rc, rc);
            return(rc);
        }
        printf("  delete connGport(0x%x)\n", gport_ucast_voq_connector_group);
    }

    /* detach intermediate Scheduler elements (SE) */
    for (nbr_fq_sched = 0; nbr_fq_sched < C4_NBR_FQ_SCHEDULERS; nbr_fq_sched++) {
        switch (nbr_fq_sched) {
            case 0: gport_scheduler_fq = c4_gport_ucast_scheduler_fq0[idx];
                    gport_ucast_scheduler = c4_gport_ucast_scheduler[idx]; break;
            case 1: gport_scheduler_fq = c4_gport_ucast_scheduler_fq1[idx];
                    gport_ucast_scheduler = c4_gport_ucast_scheduler[idx]; break;
            case 2: gport_scheduler_fq = c4_gport_ucast_scheduler_fq2[idx];
                    gport_ucast_scheduler = c4_gport_ucast_scheduler1[idx]; break;
            default: gport_scheduler_fq = c4_gport_ucast_scheduler_fq3[idx];
                     gport_ucast_scheduler = c4_gport_ucast_scheduler1[idx]; break;
        }

        rc = bcm_cosq_gport_detach(unit, gport_ucast_scheduler, gport_scheduler_fq, 0);
        if (BCM_FAILURE(rc)) {
            printf("detach for SE(0x%x) failed, Error (%d, 0x%x)\n", gport_scheduler_fq, rc, rc);
            return(rc);
        }
        printf("  detached schedGport(0x%x)\n", gport_scheduler_fq);
    }

    /* delete intermediate Scheduler elements (SE) */
    for (nbr_fq_sched = 0; nbr_fq_sched < C4_NBR_FQ_SCHEDULERS; nbr_fq_sched++) {
        switch (nbr_fq_sched) {
            case 0: gport_scheduler_fq = c4_gport_ucast_scheduler_fq0[idx]; break;
            case 1: gport_scheduler_fq = c4_gport_ucast_scheduler_fq1[idx]; break;
            case 2: gport_scheduler_fq = c4_gport_ucast_scheduler_fq2[idx]; break;
            default: gport_scheduler_fq = c4_gport_ucast_scheduler_fq3[idx]; break;
        }

        rc = bcm_cosq_gport_delete(unit, gport_scheduler_fq);
        if (BCM_FAILURE(rc)) {
            printf("delete for SE(0x%x) failed, Error (%d, 0x%x)\n", gport_scheduler_fq, rc, rc);
            return(rc);
        }
        printf("  delete schedGport(0x%x)\n", gport_scheduler_fq);
    }

    /* delete scheduler  element at top of sub-tree */
    for (nbr_sched = 0; nbr_sched < C4_NBR_SCHEDULERS; nbr_sched++) {
        switch(nbr_sched) {
            case 0: gport_ucast_scheduler = c4_gport_ucast_scheduler[idx]; break;
            default: gport_ucast_scheduler = c4_gport_ucast_scheduler1[idx]; break;
        }

        rc = bcm_cosq_gport_delete(unit, gport_ucast_scheduler);
        if (BCM_FAILURE(rc)) {
            printf("delete for SE(0x%x) failed, Error (%d, 0x%x)\n", gport_ucast_scheduler, rc, rc);
            return(rc);
        }
        printf("  delete schedGport(0x%x)\n", gport_ucast_scheduler);
    }

    printf("cint_e2e_composite_se.c (teardown) completed with status (%s)\n", bcm_errmsg(rc));

    c4_use_same_handles = 1;

    return(rc);
}

int
e2e_composite_se_verify_mode(int unit, bcm_gport_t gport, int cosq, int exp_mode)
{
    int                           rc = BCM_E_NONE;
    int                           mode, weight;


    rc = bcm_cosq_gport_sched_get(unit, gport, cosq, &mode, &weight);
    if (rc != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_get on Sched (0x%x) failed, error (%d)\n", gport_ucast_scheduler, rc);
        return(rc);
    }
    if (mode != exp_mode) {
        printf("sched mode mismatch on gport (0x%x) expected(BCM_COSQ_SP%d) retreived (BCM_COSQ_SP%d)\n", gport, (exp_mode - BCM_COSQ_SP0), (mode - BCM_COSQ_SP0));
    }

    return(rc);
}

int
e2e_composite_se_verify_bandwidth(int unit, bcm_gport_t gport, int cosq, int exp_rate, int margin)
{
    int                           rc = BCM_E_NONE;
    uint32                        kbits_sec_min, kbits_sec_max, flags;


    rc = bcm_cosq_gport_bandwidth_get(unit, gport, cosq, &kbits_sec_min, &kbits_sec_max, &flags);
    if (rc != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_get on gport (0x%x) failed, error (%d)\n", gport, rc);
        return(rc);
    }
    if ( (kbits_sec_max < (exp_rate - margin)) ||
         (kbits_sec_max > (exp_rate + margin)) ) {
        printf("bandwidth_get mismatch on gport (0x%x) expected(%d) retreived (%d)\n", gport, exp_rate, kbits_sec_max);
    }

    return(rc);
}

int
e2e_composite_se_verify_burst_size(int unit, bcm_gport_t gport, int cosq, int exp_burst_size, int margin)
{
    int                           rc = BCM_E_NONE;
    int                           max_burst;


    rc = bcm_cosq_control_get(unit, gport, cosq, bcmCosqControlBandwidthBurstMax, &max_burst);
    if (rc != BCM_E_NONE) {
        printf("bcm_cosq_control_get-BandwidthBurstMax on gport (0x%x) failed, error (%d)\n", gport, rc);
        return(rc);
    }
    if ( (max_burst < (exp_burst_size - margin)) || (max_burst > (exp_burst_size + margin)) ) {
        printf("control_get-BandwidthBurstMax mismatch on Sched (0x%x) expected(%d) retreived (%d)\n", gport, exp_burst_size, max_burst);
    }

    return(rc);
}

int
e2e_composite_se_verify(int unit, int kbits_sec_max_port, int port_idx)
{
    int                           rc = BCM_E_NONE;
    int                           idx = port_idx;
    bcm_gport_t                   physical_port;
    int                           num_cos_levels;
    uint32                        flags;
    int                           exp_mode, exp_weight, exp_margin, exp_kbits_sec_max;
    int                           cosq, slow_rate;
    bcm_gport_t                   gport_ucast_voq_connector_group;
    bcm_gport_t                   gport_ucast_queue_group;
    bcm_gport_t                   gport_scheduler_fq;
    bcm_gport_t                   gport_ucast_scheduler;
    int                           nbr_queue_groups, nbr_fq_sched, nbr_sched, nbr_sched_flw;


    printf("\nVerify Setup\n");

    exp_margin = kbits_sec_max_port /100; /* 1% error */
    rc = e2e_composite_se_verify_bandwidth(unit, c4_e2e_parent_gport[idx], 0, kbits_sec_max_port, exp_margin);
    if (rc != BCM_E_NONE) {
        return(rc);
    }

    exp_margin = kbits_sec_max_port /100; /* 1% error */
    rc = e2e_composite_se_verify_bandwidth(unit, c4_e2e_gport[idx], 0, kbits_sec_max_port, exp_margin);
    if (rc != BCM_E_NONE) {
        return(rc);
    }

    /* retrive e2e port scheduling element */
    rc = bcm_cosq_gport_get(unit, c4_e2e_gport[idx], &physical_port, &num_cos_levels, &flags);
    if (rc != BCM_E_NONE) {
        printf("bcm_cosq_gport_get on e2e Sched (0x%x) failed, error (%d)\n", c4_e2e_gport[idx], rc);
        return(rc);
    }
    if (flags != C4_DEFAULT_E2E_SCHED_FLAGS) {
        printf("gport get mismatch on E2E Sched (0x%x) expected flags(0x%x) retreived flags (0x%x)\n", c4_e2e_gport[idx], C4_DEFAULT_E2E_SCHED_FLAGS, flags);
    }

    /* retreive scheduler element at top of sub-tree */
    for (nbr_sched = 0; nbr_sched < C4_NBR_SCHEDULERS; nbr_sched++) {
        switch(nbr_sched) {
            case 0: gport_ucast_scheduler = c4_gport_ucast_scheduler[idx]; break;
            default: gport_ucast_scheduler = c4_gport_ucast_scheduler1[idx]; break;
        }

        /* retreive type of scheduling element */
        rc = bcm_cosq_gport_get(unit, gport_ucast_scheduler,
                                               &physical_port, &num_cos_levels, &flags);
        if (rc != BCM_E_NONE) {
            printf("bcm_cosq_gport_get on Sched (0x%x) failed, error (%d)\n", gport_ucast_scheduler, rc);
            return(rc);
        }
        if (flags != C4_SCHED_FLAGS) {
            printf("gport get mismatch on Sched (0x%x) expected flags(0x%x) retreived flags (0x%x)\n", gport_ucast_scheduler, C4_SCHED_FLAGS, flags);
        }
    }

    /* retreive scheduler element at top of sub-tree */
    for (nbr_sched = 0; nbr_sched < C4_NBR_SCHEDULERS; nbr_sched++) {
        switch(nbr_sched) {
            case 0:
                for (nbr_sched_flw = 0; nbr_sched_flw < C4_NBR_SCHEDULER_FLOWS; nbr_sched_flw++) {
                    switch(nbr_sched_flw) {
                        case 0: gport_ucast_scheduler = c4_gport_ucast_scheduler[idx];
                                exp_mode = C4_SCHEDULER0_CIR_MODE;
                                exp_kbits_sec_max = C4_SCHED_CIR_KBITS_SEC_MAX;
                                exp_margin = C4_SCHED_CIR_KBITS_SEC_MAX_MARGIN; break;
                        default: gport_ucast_scheduler = c4_gport_ucast_scheduler_sf2[idx];
                                 exp_mode = C4_SCHEDULER0_EIR_MODE;
                                 exp_kbits_sec_max = C4_SCHED_EIR_KBITS_SEC_MAX;
                                 exp_margin = C4_SCHED_EIR_KBITS_SEC_MAX_MARGIN; break;
                    }

                    /* retreive SE scheduling discipline */
                    rc = e2e_composite_se_verify_mode(unit, gport_ucast_scheduler, 0, exp_mode);
                    if (rc != BCM_E_NONE) {
                        return(rc);
                    }

                    /* retreive rate of Scheduler */
                    rc = e2e_composite_se_verify_bandwidth(unit, gport_ucast_scheduler, 0, exp_kbits_sec_max, exp_margin);
                    if (rc != BCM_E_NONE) {
                        return(rc);
                    }

                    /* retreive scheduler burst size */
                    rc = e2e_composite_se_verify_burst_size(unit, gport_ucast_scheduler, 0, C4_SCHED_MAX_BURST, C4_SCHED_MAX_BURST_MARGIN);
                    if (rc != BCM_E_NONE) {
                        return(rc);
                    }
                }
                break;

            default:
                for (nbr_sched_flw = 0; nbr_sched_flw < C4_NBR_SCHEDULER_FLOWS; nbr_sched_flw++) {
                    switch(nbr_sched_flw) {
                        case 0: gport_ucast_scheduler = c4_gport_ucast_scheduler1[idx];
                                exp_mode = C4_SCHEDULER1_CIR_MODE;
                                exp_kbits_sec_max = C4_SCHED_CIR_KBITS_SEC_MAX;
                                exp_margin = C4_SCHED_CIR_KBITS_SEC_MAX_MARGIN; break;
                        default: gport_ucast_scheduler = c4_gport_ucast_scheduler1_sf2[idx];
                                 exp_mode = C4_SCHEDULER1_EIR_MODE;
                                 exp_kbits_sec_max = C4_SCHED_EIR_KBITS_SEC_MAX;
                                 exp_margin = C4_SCHED_EIR_KBITS_SEC_MAX_MARGIN; break;
                    }

                    /* retreive SE scheduling discipline */
                    rc = e2e_composite_se_verify_mode(unit, gport_ucast_scheduler, 0, exp_mode);
                    if (rc != BCM_E_NONE) {
                        return(rc);
                    }

                    /* retreive rate of Scheduler */
                    rc = e2e_composite_se_verify_bandwidth(unit, gport_ucast_scheduler, 0, exp_kbits_sec_max, exp_margin);
                    if (rc != BCM_E_NONE) {
                        return(rc);
                    }

                    /* retreive scheduler burst size */
                    rc = e2e_composite_se_verify_burst_size(unit, gport_ucast_scheduler, 0, C4_SCHED_MAX_BURST, C4_SCHED_MAX_BURST_MARGIN);
                    if (rc != BCM_E_NONE) {
                        return(rc);
                    }
                }
                break;
        }
    }

    /* Reference other scripts for verification of remaining hierarchy */

    printf("cint_e2e_composite_se.c (verify) completed with status (%s)\n", bcm_errmsg(rc));

    return(rc);
}


/* Traffic passes after this step */
/*e2e_composite_se_setup(500000, 12);*/

/* Traffic stops after this step */
/* e2e_composite_se_teardown(12); */

/* verify setup */
/*e2e_composite_se_verify(500000, 12);*/


