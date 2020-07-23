/*~~~~~~~~~~~~~~~~~~~~~~~~~~Cosq: E2E Scheduling~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_tm_fap_config3.c
 * Purpose: Example of WFQ Validation
 *
 * Environment
 *    - Default Egress queue mapping in play.
 *    - Default Egress Port Scheduler hierarchy in play.
 *    - Default Ingress Scheduling Discipline (delay tolerance), queues sizes
 *    - Packet Size 1500
 *
 * - Port Scheduler
 *   - CL Scheduler (WFQ Independent Mode)
 *     - 4 FQ Schedulers
 *       - 1 Queue Group / Connector Group (4 elements in bundle) connected to
 *         corresponding FQ schedulers
 *       - 2nd Queue Group / Connector Group (4 elements in bundle) connected to
 *         corresponding FQ schedulers - Created by specifying BCM_COSQ_GPORT_TM_FLOW_ID flag.
 * - Input
 *   - Queue - Cos 0
 *     Input 500 Mbps
 *   - Queue - Cos 1
 *     Input 500 Mbps
 *
 * - Expected Output
 *   - Queue - Cos 0
 *     Expected Output 333 Mbps
 *     Observed 400 Mbps (NOTE: ratios are correct)
 *   - Queue - Cos 1
 *     Expected Output 166 Mbps
 *     Observed 200 Mbps (NOTE: ratios are correct)
 * 
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  |                                                                           | 
 *  |                                  |                                        | 
 *  |                                 SPR (500 Mbps)                            | 
 *  |                                  |                                        | 
 *  |                                 \/                                        | 
 *  |                                +----+                                     | 
 *  |                               /      \                                    | 
 *  |                              /   HR   \                                   | 
 *  |                             /          \                                  | 
 *  |                            +------------+                                 | 
 *  |                                  |                                        | 
 *  |                                 SPR (500 Mbps)                            | 
 *  |                                  |                                        | 
 *  |                                 \/                                        | 
 *  |                          +----------------+                               | 
 *  |                         / CL (1 SP level, independent WFQ)                | 
 *  |                        /                    \                             | 
 *  |                       /         SP0          \                            | 
 *  |                      +------------------------+                           | 
 *  |                         /    |       |      \                             |
 *  |                   Weight-2 Weight-4 Weight-8 Weight-32                    | 
 *  |                      /       |       |       \                            |
 *  |                   SPR       SPR     SPR      SPR                          |
 *  |               (500 Mbps)(500 Mbps)(500 Mbps)(500 Mbps)                    | 
 *  |                  /           |       |          \                         | 
 *  |                 |/          |/      \/          \/                        | 
 *  |             +------+   +------+   +------+   +------+                     | 
 *  |            /   FQ   \ /   FQ   \ /   FQ   \ /   FQ   \                    | 
 *  |           +----------+----------+----------+----------+                   | 
 *  |               \            |          |          /                        | 
 *  |                \           |          |         /                         | 
 *  |               SPR         SPR        SPR       SPR                        | 
 *  |            (500 Mbps) (500 Mbps) (500 Mbps) (500 Mbps)                    | 
 *  |                  \         |          |      /                            |
 *  |                  \/       \/         \/     \/                            | 
 *  |               |     |  |     |   |     |  |     |                         |
 *  |               |     |  |     |   |     |  |     |                         | 
 *  |               |     |  |     |   |     |  |     |                         |
 *  |               |     |  |     |   |     |  |     |                         |
 *  |               |     |  |     |   |     |  |     |                         |
 *  |               +-----+  +-----+   +-----+  +-----+     +----------------+  |
 *  |               Input:    Input:                        |      KEY       |  |
 *  |              500 Mbps  500 Mbps                       +----------------+  |
 *  |                                                       |SPR- Rate Shaper|  |
 *  |                                                       |                |  |
 *  |                                                       +----------------+  |
 *  |                       +~~~~~~~~~~~~~~~~~~~+                               |
 *  |                       |   Figure 6: WFQ   |                               |      
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
 */


/*
 * General Definitions
 */ 

struct e2e_wfq_sched_scheme_s{
    int cl_kbits_sec_max;
    int cl_max_burst;

    int fq_kbits_sec_max;
    int fq_max_burst;

    int voq_conn_kbits_sec_max;
    int voq_conn_max_burst;

    int fq0_weight;
    int fq1_weight;
    int fq2_weight;
    int fq3_weight;

    int wfq_mode;   /* WFQ mode options: 0 - Independent, 1 - Descrete, 2 - Descrete-Class*/
};

struct e2e_wfq_margin_s {
    int cl_kbits_sec_max_margin;
    int cl_max_burst_margin;

    int fq_kbits_sec_max_margin;
    int fq_max_burst_margin;

    int voq_conn_kbits_sec_max_margin;
    int voq_conn_max_burst_margin;
};

struct e2e_wfq_s {
    e2e_wfq_sched_scheme_s   sched;
    e2e_wfq_margin_s         margin;

};

e2e_wfq_s g_e2e_wfq;


int C3_NUM_COS = 4;
/*int C3_SCHED_MODE = BCM_COSQ_SP1; */
int C3_SCHED_FQ3_UPDATED_WEIGHT = 32; 

int C3_DEFAULT_E2E_SCHED_FLAGS = (BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_HR_ENHANCED);
int C3_SCHED_FLAGS[3] =
{
    (BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE5_1SP_WFQ | BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_INDEPENDENT),
    (BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE5_1SP_WFQ | BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_DISCREET),
    (BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE5_1SP_WFQ | BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_CLASS),
};
int C3_QUEUE_FLAGS = (BCM_COSQ_GPORT_UCAST_QUEUE_GROUP);
int C3_SCHED_FQ_FLAGS = (BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_FQ);
int C3_CONN_FLAGS = (BCM_COSQ_GPORT_VOQ_CONNECTOR);

int C3_SCHED_FQ0_DWGT_AP = BCM_COSQ_WEIGHT_DISCRETE_WEIGHT_LEVEL0; /* discrete Weight Attach Point */
int C3_SCHED_FQ1_DWGT_AP = BCM_COSQ_WEIGHT_DISCRETE_WEIGHT_LEVEL1; /* discrete Weight Attach Point */
int C3_SCHED_FQ2_DWGT_AP = BCM_COSQ_WEIGHT_DISCRETE_WEIGHT_LEVEL2; /* discrete Weight Attach Point */
int C3_SCHED_FQ3_DWGT_AP = BCM_COSQ_WEIGHT_DISCRETE_WEIGHT_LEVEL3; /* discrete Weight Attach Point */

int C3_SCHED_KBITS_SEC_MAX = 500000;


/* handles */
/*int unit = 0;*/
int my_modid = 0;

struct e2e_wfq_gports_s {
    bcm_gport_t gport_ucast_voq_connector_group0;
    bcm_gport_t gport_ucast_voq_connector_group1;
    bcm_gport_t gport_ucast_queue_group0;
    bcm_gport_t gport_ucast_queue_group1;
    bcm_gport_t gport_ucast_scheduler;
    bcm_gport_t gport_ucast_scheduler_fq0;
    bcm_gport_t gport_ucast_scheduler_fq1;
    bcm_gport_t gport_ucast_scheduler_fq2;
    bcm_gport_t gport_ucast_scheduler_fq3;
    bcm_gport_t e2e_gport;
};

e2e_wfq_gports_s g_e2e_wfq_gports;

int C3_NBR_QUEUE_GROUPS = 2;
int C3_NBR_FQ_SCHEDULERS = 4;

/*
 * Init Functions
 */
void e2e_wfq_cl_init(int rate, int max_burst)
{
    g_e2e_wfq.sched.cl_kbits_sec_max = rate;
    g_e2e_wfq.sched.cl_max_burst = max_burst;
}

void e2e_wfq_fq_init(int rate, int max_burst)
{
    g_e2e_wfq.sched.fq_kbits_sec_max = rate;
    g_e2e_wfq.sched.fq_max_burst = max_burst;
}

void e2e_wfq_voq_conn_init(int rate, int max_burst)
{
    g_e2e_wfq.sched.voq_conn_kbits_sec_max = rate;
    g_e2e_wfq.sched.voq_conn_max_burst = max_burst;
}

void e2e_wfq_fq_weight_init(int fq0_weight, int fq1_weight, int fq2_weight, int fq3_weight)
{
    g_e2e_wfq.sched.fq0_weight = fq0_weight;
    g_e2e_wfq.sched.fq1_weight = fq1_weight;
    g_e2e_wfq.sched.fq2_weight = fq2_weight;
    g_e2e_wfq.sched.fq3_weight = fq3_weight;
}

void e2e_wfq_wfq_mode_init(int wfq_mode)
{
    g_e2e_wfq.sched.wfq_mode = wfq_mode;
}

void e2e_wfq_margin_cl_init(int rate, int max_burst)
{
    g_e2e_wfq.margin.cl_kbits_sec_max_margin = rate;
    g_e2e_wfq.margin.cl_max_burst_margin = max_burst;
}

void e2e_wfq_margin_fq_init(int rate, int max_burst)
{
    g_e2e_wfq.margin.fq_kbits_sec_max_margin = rate;
    g_e2e_wfq.margin.fq_max_burst_margin = max_burst;
}

void e2e_wfq_margin_voq_conn_init(int rate, int max_burst)
{
    g_e2e_wfq.margin.voq_conn_kbits_sec_max_margin = rate;
    g_e2e_wfq.margin.voq_conn_max_burst_margin = max_burst;
}

int e2e_wfq_modid_init(int unit)
{
    int rv;
    int is_dnx;

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_device_member_get, $rv \n");
        return rv;
    }
    if (is_dnx) {
        int modid_count, actual_modid_count;
        rv = bcm_stk_modid_count(unit, &modid_count);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_stk_modid_count\n");
            return rv;
        }
        bcm_stk_modid_config_t modid_array[modid_count];

        rv = bcm_stk_modid_config_get_all(unit, modid_count, modid_array, &actual_modid_count);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_stk_modid_config_get_all\n");
            return rv;
        }
        my_modid = modid_array[0].modid;
    }
    else {

        rv = bcm_stk_modid_get(unit, &my_modid);
        if (rv != BCM_E_NONE) {
            printf("bcm_stk_modid_get failed $rv\n");
        }
    }
    return rv;
}

int e2e_wfq_init(int unit)
{
    int is_dnx, multiplier = 1;
    int rv = BCM_E_NONE;
    uint32 credit_worth;

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_device_member_get, $rv \n");
        return rv;
    }

    if (is_dnx) {
        /* Default credit size for DPP was 1K.
         * In order to utilize the cint also for DNX device
         * where credit size by default is 4K
         * we multiply by 4 the MaxBurst
         */
        credit_worth = *(dnxc_data_get(unit, "iqs", "credit", "worth", NULL));
        multiplier = credit_worth / 1024;
    }
    e2e_wfq_cl_init(500000, multiplier * 3000);
    e2e_wfq_fq_init(500000, multiplier * 2500);
    e2e_wfq_voq_conn_init(500000, multiplier * 2500);
    e2e_wfq_fq_weight_init(2, 4, 8, 16);
    e2e_wfq_wfq_mode_init(0);
    e2e_wfq_margin_cl_init(15000, multiplier * 75);
    e2e_wfq_margin_fq_init(15000, multiplier * 775);
    e2e_wfq_margin_voq_conn_init(15000, multiplier * 775);

    return e2e_wfq_modid_init(unit);
}


void e2e_wfq_display_handles()
{
    printf("  e2eGport(0x%x) schedGport(0x%x)\n",
            g_e2e_wfq_gports.e2e_gport, g_e2e_wfq_gports.gport_ucast_scheduler);
    printf("    l3_fq0(0x%x) l3_fq1(0%xx) l3_fq2(0x%x) l3_fq3(0x%x)\n",
            g_e2e_wfq_gports.gport_ucast_scheduler_fq0, g_e2e_wfq_gports.gport_ucast_scheduler_fq1,
            g_e2e_wfq_gports.gport_ucast_scheduler_fq2, g_e2e_wfq_gports.gport_ucast_scheduler_fq3);
    printf("    connGrp0Gport(0x%x) connGrp1Gport(0x%x)\n",
            g_e2e_wfq_gports.gport_ucast_voq_connector_group0, g_e2e_wfq_gports.gport_ucast_voq_connector_group1);
    printf("    ucastGrp0Gport(0x%x) ucastGrp1Gport(0x%x)\n",
            g_e2e_wfq_gports.gport_ucast_queue_group0, g_e2e_wfq_gports.gport_ucast_queue_group1);
}

int e2e_wfq_sched_cl_wfq_run(int unit, bcm_gport_t sched_gport)
{
    int                 rv = BCM_E_NONE;
    bcm_cosq_control_t  type;
    int                 weight;
    int                 nbr_ap = 4, cur_ap;
   

    for (cur_ap = 0; cur_ap < nbr_ap; cur_ap++) {
        switch(cur_ap) {
            case 0:
                type = bcmCosqControlDiscreteWeightLevel0;
                weight = g_e2e_wfq.sched.fq0_weight;
                break;

            case 1:
                type = bcmCosqControlDiscreteWeightLevel1;
                weight = g_e2e_wfq.sched.fq1_weight;
                break;

            case 2:
                type = bcmCosqControlDiscreteWeightLevel2;
                weight = g_e2e_wfq.sched.fq2_weight;
                break;

            case 3:
            default:
                type = bcmCosqControlDiscreteWeightLevel3;
                weight = g_e2e_wfq.sched.fq3_weight;
                break;
        }

        rv = bcm_cosq_control_set( unit, sched_gport, 0, type, weight);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_control_set (Discrete weight - level(%d)) failed for gport (0x%x), error (%d)\n", cur_ap, sched_gport, rv);
            return(rv);
        }
    }

    return(rv);
}

int e2e_wfq_sched_cl_wfq_verify(int unit, bcm_gport_t sched_gport)
{
    int                 rv = BCM_E_NONE;
    bcm_cosq_control_t  type;
    int                 weight;
    int                 exp_weight;
    int                 nbr_ap = 4, cur_ap;


    for (cur_ap = 0; cur_ap < nbr_ap; cur_ap++) {
        switch(cur_ap) {
            case 0:
                type = bcmCosqControlDiscreteWeightLevel0;
                exp_weight = g_e2e_wfq.sched.fq0_weight;
                break;

            case 1:
                type = bcmCosqControlDiscreteWeightLevel1;
                exp_weight = g_e2e_wfq.sched.fq1_weight;
                break;

            case 2:
                type = bcmCosqControlDiscreteWeightLevel2;
                exp_weight = g_e2e_wfq.sched.fq2_weight;
                break;

            case 3:
            default:
                type = bcmCosqControlDiscreteWeightLevel3;
                exp_weight = g_e2e_wfq.sched.fq3_weight;
                break;
        }

        rv = bcm_cosq_control_get( unit, sched_gport, 0, type, &weight);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_control_get (Discrete weight - level(%d)) failed for gport (0x%x), error (%d)\n", cur_ap, sched_gport, rv);
            return(rv);
        }

        if (weight != exp_weight) {
            printf("sched WFQ  mismatch on Sched:level (0x%x:%d) expected(%d) retreived (%d)\n", sched_gport, cur_ap, exp_weight, weight);
        }
    }

    return(rv);
}

int e2e_wfq_start_run(int unit, int port, int mode)
{
    int system_port;
    bcm_gport_t dest_gport;
    bcm_gport_t sys_gport;
    int num_cos = 4;
    int flags = 0, default_flags = 0;
    bcm_error_t rv = BCM_E_NONE;
    bcm_gport_t e2e_parent_gport;
    int voq;
    bcm_cosq_gport_connection_t connection;
    int kbits_sec_max;
    int max_burst;
    int cosq = 0;
    bcm_cosq_delay_tolerance_t delay_tolerance;
    int weight;
    bcm_cosq_voq_connector_gport_t config;
    uint32 dummy_flags ;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    int is_dnx;

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_device_member_get, $rv \n");
        return rv;
    }

    printf("\nSetup\n");

    if (mode == -1)  {
        mode = 0;
    }

    switch (mode) {
        case 0:
        printf("  WFQ mode Independent\n");
            break;
       
        case 1:
        printf("  WFQ mode Discrete\n");
            break;
       
        case 2:
        printf("  WFQ mode Discrete - Class\n");
            break;
       
        default:
        printf("Invalid WFQ mode %d\n", mode);
            return(-1);
            break;
    }
    g_e2e_wfq.sched.wfq_mode = mode;
    rv = bcm_port_get(unit, port, &dummy_flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_get scheduler failed $rv\n");
        return rv;
    }
    system_port = port;
    BCM_GPORT_SYSTEM_PORT_ID_SET(sys_gport, system_port);
    BCM_GPORT_LOCAL_SET(dest_gport, port);

    rv = bcm_stk_sysport_gport_set(unit, sys_gport, dest_gport);

    if (rv != BCM_E_NONE) {
        printf("bcm_stk_sysport_gport_set failed $rv\n");
        return rv;
    }

    /* create CL scheduler */
    flags = C3_SCHED_FLAGS[g_e2e_wfq.sched.wfq_mode] | default_flags;

    rv = bcm_cosq_gport_add(unit, dest_gport, 1, 
                flags, &g_e2e_wfq_gports.gport_ucast_scheduler);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return rv;
    }


    /* COS 0, CL scheduler scheduling discipline configuration */
    rv = bcm_cosq_gport_sched_set(unit,
                      g_e2e_wfq_gports.gport_ucast_scheduler,
                      0,
                      BCM_COSQ_SP1,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    BCM_COSQ_GPORT_E2E_PORT_SET(g_e2e_wfq_gports.e2e_gport, port);

        /* CL scheduler attach to E2E port scheduler */
    rv = bcm_cosq_gport_attach(unit, g_e2e_wfq_gports.e2e_gport, 
                   g_e2e_wfq_gports.gport_ucast_scheduler, 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }

    /* configure CL - WFQ scheduler */
    if (g_e2e_wfq.sched.wfq_mode != 0) {
        e2e_wfq_sched_cl_wfq_run(unit, g_e2e_wfq_gports.gport_ucast_scheduler);
    }

    /* create scheduler FQ scheduler (instance 0) - will aggregrate all cos 0 queues */
    flags = BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_FQ | default_flags;

    rv = bcm_cosq_gport_add(unit, dest_gport, 1, 
                flags, &g_e2e_wfq_gports.gport_ucast_scheduler_fq0);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return rv;
    }

    /* COS 0, FQ scheduler (instance 0) scheduling discipline configuration, weight of 2 */
    weight = (g_e2e_wfq.sched.wfq_mode == 0) ? g_e2e_wfq.sched.fq0_weight : C3_SCHED_FQ0_DWGT_AP;
    rv = bcm_cosq_gport_sched_set(unit,
                      g_e2e_wfq_gports.gport_ucast_scheduler_fq0,
                      0,
                      BCM_COSQ_SP0,
                      weight);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

        /* FQ scheduler (instance 0) attach to CL scheduler */
    rv = bcm_cosq_gport_attach(unit, g_e2e_wfq_gports.gport_ucast_scheduler, 
                   g_e2e_wfq_gports.gport_ucast_scheduler_fq0, 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }

    /* create scheduler FQ scheduler (instance 1) - will aggregrate all cos 1 queues */
    flags = BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_FQ | default_flags;

    rv = bcm_cosq_gport_add(unit, dest_gport, 1, 
                flags, &g_e2e_wfq_gports.gport_ucast_scheduler_fq1);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return rv;
    }

    /* COS 0, FQ scheduler (instance 1) scheduling discipline configuration, weight of 4 */
    weight = (g_e2e_wfq.sched.wfq_mode == 0) ? g_e2e_wfq.sched.fq1_weight : C3_SCHED_FQ1_DWGT_AP;
    rv = bcm_cosq_gport_sched_set(unit,
                      g_e2e_wfq_gports.gport_ucast_scheduler_fq1,
                      0,
                      BCM_COSQ_SP0,
                      weight);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }


    /* FQ scheduler (instance 1) attach to CL scheduler */
    rv = bcm_cosq_gport_attach(unit, g_e2e_wfq_gports.gport_ucast_scheduler, 
                   g_e2e_wfq_gports.gport_ucast_scheduler_fq1, 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }


    /* create scheduler FQ scheduler (instance 2) - will aggregrate all cos 2 queues */
    flags = BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_FQ | default_flags;

    rv = bcm_cosq_gport_add(unit, dest_gport, 1, 
                flags, &g_e2e_wfq_gports.gport_ucast_scheduler_fq2);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return rv;
    }

    /* COS 0, FQ scheduler (instance 2) scheduling discipline configuration, weight of 8 */
    weight = (g_e2e_wfq.sched.wfq_mode == 0) ? g_e2e_wfq.sched.fq2_weight : C3_SCHED_FQ2_DWGT_AP;
    rv = bcm_cosq_gport_sched_set(unit,
                      g_e2e_wfq_gports.gport_ucast_scheduler_fq2,
                      0,
                      BCM_COSQ_SP0,
                      weight);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }


    /* FQ scheduler (instance 2) attach to CL scheduler */
    rv = bcm_cosq_gport_attach(unit, g_e2e_wfq_gports.gport_ucast_scheduler, 
                   g_e2e_wfq_gports.gport_ucast_scheduler_fq2, 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }


    /* create scheduler FQ scheduler (instance 3) - will aggregrate all cos 3 queues */
    flags = BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_FQ | default_flags;

    rv = bcm_cosq_gport_add(unit, dest_gport, 1, 
                flags, &g_e2e_wfq_gports.gport_ucast_scheduler_fq3);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return rv;
    }

    /* COS 0, FQ scheduler (instance 3) scheduling discipline configuration, weight of 16 */
    weight = (g_e2e_wfq.sched.wfq_mode == 0) ? g_e2e_wfq.sched.fq3_weight : C3_SCHED_FQ3_DWGT_AP;
    rv = bcm_cosq_gport_sched_set(unit,
                      g_e2e_wfq_gports.gport_ucast_scheduler_fq3,
                      0,
                      BCM_COSQ_SP0,
                      weight);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }


    /* FQ scheduler (instance 3) attach to CL scheduler */
    rv = bcm_cosq_gport_attach(unit, g_e2e_wfq_gports.gport_ucast_scheduler, 
                   g_e2e_wfq_gports.gport_ucast_scheduler_fq3, 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }

    /* Modify - Weight */
    weight = (g_e2e_wfq.sched.wfq_mode == 0) ? C3_SCHED_FQ3_UPDATED_WEIGHT : C3_SCHED_FQ3_DWGT_AP;
    rv = bcm_cosq_gport_sched_set(unit,
                      g_e2e_wfq_gports.gport_ucast_scheduler_fq3,
                      0,
                      BCM_COSQ_SP0,
                      weight);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }


    /* create voq connector bundle, 4 elements in bundle */          
    BCM_GPORT_LOCAL_SET(config.port, port);
    config.flags = C3_CONN_FLAGS | default_flags;
    config.numq = num_cos;
    config.remote_modid = my_modid + mapping_info.core;
    config.nof_remote_cores = 1;

    rv = bcm_cosq_voq_connector_gport_add(unit, &config, &g_e2e_wfq_gports.gport_ucast_voq_connector_group0);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add connector failed $rv\n");
        return rv;
    }

    
    /* COS 0 (bundle 1st element), scheduling discipline setup */
    rv = bcm_cosq_gport_sched_set(unit,
                      g_e2e_wfq_gports.gport_ucast_voq_connector_group0,
                      0,
                      BCM_COSQ_SP0,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    /* COS 0 (bundle 1st element), attach to FQ scheduler (instance 0) */
    rv = bcm_cosq_gport_attach(unit, g_e2e_wfq_gports.gport_ucast_scheduler_fq0, 
               g_e2e_wfq_gports.gport_ucast_voq_connector_group0, 0);
    if (rv != BCM_E_NONE) {
      printf("bcm_cosq_gport_attach failed $rv\n");
      return rv;
    }
    
    /* COS 1, (bundle 2nd element), scheduling discipline setup */
    rv = bcm_cosq_gport_sched_set(unit,
                      g_e2e_wfq_gports.gport_ucast_voq_connector_group0,
                      1,
                      BCM_COSQ_SP0,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    /* COS 1 (bundle 2nd element), attach to FQ scheduler (instance 1) */
    rv = bcm_cosq_gport_attach(unit, g_e2e_wfq_gports.gport_ucast_scheduler_fq1, 
                   g_e2e_wfq_gports.gport_ucast_voq_connector_group0, 1);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }


    /* COS 2, (bundle 3nd element), scheduling discipline setup */
    rv = bcm_cosq_gport_sched_set(unit,
                      g_e2e_wfq_gports.gport_ucast_voq_connector_group0,
                      2,
                      BCM_COSQ_SP0,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    /* COS 2 (bundle 3rd element), attach to FQ scheduler (instance 2) */
    rv = bcm_cosq_gport_attach(unit, g_e2e_wfq_gports.gport_ucast_scheduler_fq2, 
               g_e2e_wfq_gports.gport_ucast_voq_connector_group0, 2);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }
    
    /* COS 3, (bundle 4th element), scheduling discipline setup */
    rv = bcm_cosq_gport_sched_set(unit,
                      g_e2e_wfq_gports.gport_ucast_voq_connector_group0,
                      3,
                      BCM_COSQ_SP0,
                      0);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    /* COS 3 (bundle 4th element), attach to FQ scheduler (instance 3) */
    rv = bcm_cosq_gport_attach(unit, g_e2e_wfq_gports.gport_ucast_scheduler_fq3, 
               g_e2e_wfq_gports.gport_ucast_voq_connector_group0, 3);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }

 
    /* create queue group bundle, 4 elements in bundle */          
    flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | default_flags;
    /** Creating VOQs only with system port gport is mandatory for JR2 (JR1 also supports it) */
    rv = bcm_cosq_gport_add(unit, sys_gport, num_cos, 
                flags, &g_e2e_wfq_gports.gport_ucast_queue_group0);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add queue failed $rv\n");
        return rv;
    }
    printf("ucast gport(0x%08x)\n", g_e2e_wfq_gports.gport_ucast_queue_group0);
    /*bcm_cosq_gport_add will a voq gport with core - BCM_CORE_ALL. 
      In order to be consistent with the asymmetric scheme to be created
      the core is overwrited.*/
    int voq_id = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(g_e2e_wfq_gports.gport_ucast_queue_group0);
    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(g_e2e_wfq_gports.gport_ucast_queue_group0, mapping_info.core, voq_id);

    /* connect VoQ to flow */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
    connection.remote_modid = my_modid + mapping_info.core;
    connection.voq = g_e2e_wfq_gports.gport_ucast_queue_group0;
    connection.voq_connector = g_e2e_wfq_gports.gport_ucast_voq_connector_group0;
    
    rv = bcm_cosq_gport_connection_set(unit, &connection);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_connection_set ingress failed $rv\n");
        return rv;
    }

    /* connect flow to VoQ */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
    connection.remote_modid = my_modid + mapping_info.core;
    connection.voq = g_e2e_wfq_gports.gport_ucast_queue_group0;
    connection.voq_connector = g_e2e_wfq_gports.gport_ucast_voq_connector_group0;

    rv = bcm_cosq_gport_connection_set(unit, &connection);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_connection_set egress failed $rv\n");
        return rv;
    }


    /* configuration for another queue group */
    /* create voq connector bundle, 4 elements in the bundle */          
    BCM_GPORT_LOCAL_SET(config.port, port);
    config.flags = C3_CONN_FLAGS | default_flags;
    config.numq = num_cos;
    config.remote_modid = my_modid + mapping_info.core;
    config.nof_remote_cores = 1;

    rv = bcm_cosq_voq_connector_gport_add(unit, &config, &g_e2e_wfq_gports.gport_ucast_voq_connector_group1);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add connector failed $rv\n");
        return rv;
    }

    
    /* COS 0 (bundle 1st element), scheduling discipline setup */
    rv = bcm_cosq_gport_sched_set(unit,
                      g_e2e_wfq_gports.gport_ucast_voq_connector_group1,
                      0,
                      BCM_COSQ_SP0,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    /* COS 0 (bundle 1st element), attach to FQ scheduler (instance 0) */
    rv = bcm_cosq_gport_attach(unit, g_e2e_wfq_gports.gport_ucast_scheduler_fq0, 
               g_e2e_wfq_gports.gport_ucast_voq_connector_group1, 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }

    
    /* COS 1 (bundle 2nd element), scheduling discipline setup */
    rv = bcm_cosq_gport_sched_set(unit,
                      g_e2e_wfq_gports.gport_ucast_voq_connector_group1,
                      1,
                      BCM_COSQ_SP0,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    /* COS 1 (bundle 2nd element), attach to FQ scheduler (instance 1) */
    rv = bcm_cosq_gport_attach(unit, g_e2e_wfq_gports.gport_ucast_scheduler_fq1, 
               g_e2e_wfq_gports.gport_ucast_voq_connector_group1, 1);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }


    /* COS 2 (bundle 3rd element), scheduling discipline setup */
    rv = bcm_cosq_gport_sched_set(unit,
                      g_e2e_wfq_gports.gport_ucast_voq_connector_group1,
                      2,
                      BCM_COSQ_SP0,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    /* COS 2 (bundle 3rd element), attach to FQ scheduler (instance 2) */
    rv = bcm_cosq_gport_attach(unit, g_e2e_wfq_gports.gport_ucast_scheduler_fq2, 
               g_e2e_wfq_gports.gport_ucast_voq_connector_group1, 2);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }
    
    /* COS 3 (bundle 4th element), scheduling discipline setup */
    rv = bcm_cosq_gport_sched_set(unit,
                      g_e2e_wfq_gports.gport_ucast_voq_connector_group1,
                      3,
                      BCM_COSQ_SP0,
                      0);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    /* COS 3 (bundle 4th element), attach to FQ scheduler (instance 3) */
    rv = bcm_cosq_gport_attach(unit, g_e2e_wfq_gports.gport_ucast_scheduler_fq3, 
               g_e2e_wfq_gports.gport_ucast_voq_connector_group1, 3);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }

 
    /* create queue group bundle, 4 elements in bundle */          
    flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | BCM_COSQ_GPORT_TM_FLOW_ID | default_flags;
    /** Creating VOQs only with system port gport is mandatory for JR2 (JR1 also supports it) */
    rv = bcm_cosq_gport_add(unit, sys_gport, num_cos, 
                flags, &g_e2e_wfq_gports.gport_ucast_queue_group1);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add queue failed $rv\n");
        return rv;
    }
    printf("ucast gport(0x%08x)\n", g_e2e_wfq_gports.gport_ucast_queue_group1);

    voq_id = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(g_e2e_wfq_gports.gport_ucast_queue_group1);
    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(g_e2e_wfq_gports.gport_ucast_queue_group1, mapping_info.core, voq_id);

    /* connect VoQ to flow */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
    connection.remote_modid = my_modid + mapping_info.core;
    connection.voq = g_e2e_wfq_gports.gport_ucast_queue_group1;
    connection.voq_connector = g_e2e_wfq_gports.gport_ucast_voq_connector_group1;
    
    rv = bcm_cosq_gport_connection_set(unit, &connection);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_connection_set ingress failed $rv\n");
        return rv;
    }

    /* connect flow to VoQ */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
    connection.remote_modid = my_modid + mapping_info.core;
    connection.voq = g_e2e_wfq_gports.gport_ucast_queue_group1;
    connection.voq_connector = g_e2e_wfq_gports.gport_ucast_voq_connector_group1;
    
    rv = bcm_cosq_gport_connection_set(unit, &connection);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_connection_set egress failed $rv\n");
        return rv;
    }

    
    /*
     * Set rate on the E2E port
     */
    kbits_sec_max = 500000; /* 500Mbps */
    
    BCM_COSQ_GPORT_E2E_PORT_SET(g_e2e_wfq_gports.e2e_gport, port);

    rv = bcm_cosq_gport_bandwidth_set(unit,
                      g_e2e_wfq_gports.e2e_gport,
                      0,
                      0,
                      kbits_sec_max,
                      0);

    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set failed $rv\n");
        return rv;
    }


    /*
     * Set rate on the E2E Interface
     */
    kbits_sec_max = 1000000; /* 1Gbps */

    rv = bcm_fabric_port_get(unit,
                 g_e2e_wfq_gports.e2e_gport,
                 0,
                 &e2e_parent_gport);
    

    if (rv != BCM_E_NONE) {
        printf("bcm_fabric_port_get failed $rv\n");
        return rv;
    }

    rv = bcm_cosq_gport_bandwidth_set(unit,
                      e2e_parent_gport,
                      0,
                      0,
                      kbits_sec_max,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set if failed $rv\n");
        return rv;
    }
    
    
    /*
     * Set rate on the CL Scheduler
     */
    kbits_sec_max = g_e2e_wfq.sched.cl_kbits_sec_max;
    max_burst = g_e2e_wfq.sched.cl_max_burst;
    /* Scheduler */
    rv = bcm_cosq_gport_bandwidth_set(unit,
                      g_e2e_wfq_gports.gport_ucast_scheduler,
                      0,
                      0,
                      kbits_sec_max,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set sched failed $rv\n");
        return rv;
    }
    
    /* Set max burst on the Scheduler */
    rv = bcm_cosq_control_set(unit,
                  g_e2e_wfq_gports.gport_ucast_scheduler,
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
    kbits_sec_max = g_e2e_wfq.sched.fq_kbits_sec_max;
    max_burst = g_e2e_wfq.sched.fq_max_burst;
    rv = bcm_cosq_gport_bandwidth_set(unit,
                                      g_e2e_wfq_gports.gport_ucast_scheduler_fq0,
                                      0,
                                      0,
                                      kbits_sec_max,
                                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set sched failed $rv\n");
        return(rv);
    }

    rv = bcm_cosq_control_set(unit,
                              g_e2e_wfq_gports.gport_ucast_scheduler_fq0,
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
    kbits_sec_max = g_e2e_wfq.sched.fq_kbits_sec_max;
    max_burst = g_e2e_wfq.sched.fq_max_burst;
    rv = bcm_cosq_gport_bandwidth_set(unit,
                                      g_e2e_wfq_gports.gport_ucast_scheduler_fq1,
                                      0,
                                      0,
                                      kbits_sec_max,
                                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set sched failed $rv\n");
        return(rv);
    }

    rv = bcm_cosq_control_set(unit,
                              g_e2e_wfq_gports.gport_ucast_scheduler_fq1,
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
    kbits_sec_max = g_e2e_wfq.sched.fq_kbits_sec_max;
    max_burst = g_e2e_wfq.sched.fq_max_burst;
    rv = bcm_cosq_gport_bandwidth_set(unit,
                                      g_e2e_wfq_gports.gport_ucast_scheduler_fq2,
                                      0,
                                      0,
                                      kbits_sec_max,
                                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set sched failed $rv\n");
        return(rv);
    }

    rv = bcm_cosq_control_set(unit,
                              g_e2e_wfq_gports.gport_ucast_scheduler_fq2,
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
    kbits_sec_max = g_e2e_wfq.sched.fq_kbits_sec_max;
    max_burst = g_e2e_wfq.sched.fq_max_burst;
    rv = bcm_cosq_gport_bandwidth_set(unit,
                                      g_e2e_wfq_gports.gport_ucast_scheduler_fq3,
                                      0,
                                      0,
                                      kbits_sec_max,
                                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set sched failed $rv\n");
        return(rv);
    }

    rv = bcm_cosq_control_set(unit,
                              g_e2e_wfq_gports.gport_ucast_scheduler_fq3,
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
    kbits_sec_max = g_e2e_wfq.sched.voq_conn_kbits_sec_max;
    max_burst = g_e2e_wfq.sched.voq_conn_max_burst;
    for (cosq = 0; cosq < 4; cosq++) {
        /* Set rate on the Connector flow */
        rv = bcm_cosq_gport_bandwidth_set(unit,
                          g_e2e_wfq_gports.gport_ucast_voq_connector_group0,
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
                      g_e2e_wfq_gports.gport_ucast_voq_connector_group0,
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
    kbits_sec_max = g_e2e_wfq.sched.voq_conn_kbits_sec_max;
    max_burst = g_e2e_wfq.sched.voq_conn_max_burst;
    for (cosq = 0; cosq < 4; cosq++) {
        /* Set rate on the Connector flow */
        rv = bcm_cosq_gport_bandwidth_set(unit,
                                          g_e2e_wfq_gports.gport_ucast_voq_connector_group1,
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
                                  g_e2e_wfq_gports.gport_ucast_voq_connector_group1,
                                  cosq,
                                  bcmCosqControlBandwidthBurstMax,
                                  max_burst);

        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_control_set connector maxburst failed $rv\n");
            return rv;
        }
    }

    printf("port(%d) sysport(%d) ucast gport(0x%08x)\n", port, system_port, g_e2e_wfq_gports.gport_ucast_queue_group0);
    /* } */
    printf("cint_e2e_wfq.c completed with status (%s)\n", bcm_errmsg(rv));
    return rv;
}


int e2e_wfq_teardown(int unit)
{
    int                           rc = BCM_E_NONE;
    bcm_gport_t                   gport_ucast_voq_connector_group;
    bcm_gport_t                   gport_ucast_queue_group;
    bcm_gport_t                   gport_scheduler_fq;
    int                           nbr_queue_groups, nbr_fq_sched;
    bcm_cosq_gport_connection_t   connection;
    int                           cosq = 0;


    printf("\nTeardown\n");

    e2e_wfq_display_handles();

    /* detach scheduler sub-tree from E2E hierarchy */
    rc = bcm_cosq_gport_detach(unit, g_e2e_wfq_gports.e2e_gport, g_e2e_wfq_gports.gport_ucast_scheduler, 0);
    if (BCM_FAILURE(rc)) {
        printf("detach for SE(0x%x) failed, Error (%d, 0x%x)\n", g_e2e_wfq_gports.gport_ucast_scheduler, rc, rc);
        return(rc);
    }
    printf("  detached schedGport(0x%x)\n", g_e2e_wfq_gports.gport_ucast_scheduler);

    /* dis-associate queue from connectors */
    for (nbr_queue_groups = 0; nbr_queue_groups < C3_NBR_QUEUE_GROUPS; nbr_queue_groups++) {
        switch (nbr_queue_groups) {
            case 0: gport_ucast_queue_group = g_e2e_wfq_gports.gport_ucast_queue_group0;
                    gport_ucast_voq_connector_group = g_e2e_wfq_gports.gport_ucast_voq_connector_group0; break;
            default: gport_ucast_queue_group = g_e2e_wfq_gports.gport_ucast_queue_group1;
                     gport_ucast_voq_connector_group = g_e2e_wfq_gports.gport_ucast_voq_connector_group1; break;
        }

        connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
        connection.remote_modid = my_modid;
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
    for (nbr_queue_groups = 0; nbr_queue_groups < C3_NBR_QUEUE_GROUPS; nbr_queue_groups++) {
        switch (nbr_queue_groups) {
            case 0: gport_ucast_queue_group = g_e2e_wfq_gports.gport_ucast_queue_group0;
                    gport_ucast_voq_connector_group = g_e2e_wfq_gports.gport_ucast_voq_connector_group0; break;
            default: gport_ucast_queue_group = g_e2e_wfq_gports.gport_ucast_queue_group1;
                     gport_ucast_voq_connector_group = g_e2e_wfq_gports.gport_ucast_voq_connector_group1; break;
        }

        connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
        connection.remote_modid = my_modid;
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
    for (nbr_queue_groups = 0; nbr_queue_groups < C3_NBR_QUEUE_GROUPS; nbr_queue_groups++) {
        switch (nbr_queue_groups) {
            case 0: gport_ucast_queue_group = g_e2e_wfq_gports.gport_ucast_queue_group0; break;
            default: gport_ucast_queue_group = g_e2e_wfq_gports.gport_ucast_queue_group1; break;
        }

        rc = bcm_cosq_gport_delete(unit, gport_ucast_queue_group);
        if (BCM_FAILURE(rc)) {
            printf("delete for QueueGroup(0x%x) failed, Error (%d, 0x%x)\n", gport_ucast_queue_group, rc, rc);
            return(rc);
        }
        printf("  deleted ucastGport(0x%x)\n", gport_ucast_queue_group);
    }

    /* detach connectors */
    for (nbr_queue_groups = 0; nbr_queue_groups < C3_NBR_QUEUE_GROUPS; nbr_queue_groups++) {
        switch (nbr_queue_groups) {
            case 0: gport_ucast_voq_connector_group = g_e2e_wfq_gports.gport_ucast_voq_connector_group0; break;
            default: gport_ucast_voq_connector_group = g_e2e_wfq_gports.gport_ucast_voq_connector_group1; break;
        }

        for (cosq = 0; cosq < 4; cosq++) {
            switch (cosq) {
                case 0: gport_scheduler_fq = g_e2e_wfq_gports.gport_ucast_scheduler_fq0; break;
                case 1: gport_scheduler_fq = g_e2e_wfq_gports.gport_ucast_scheduler_fq1; break;
                case 2: gport_scheduler_fq = g_e2e_wfq_gports.gport_ucast_scheduler_fq2; break;
                default: gport_scheduler_fq = g_e2e_wfq_gports.gport_ucast_scheduler_fq3; break;
            }

            rc = bcm_cosq_gport_detach(unit, gport_scheduler_fq, gport_ucast_voq_connector_group, cosq);
            if (BCM_FAILURE(rc)) {
                printf("detach for Connector:cosq(0x%x:%d) from Sched(0x%x) failed, Error (%d, 0x%x)\n", gport_ucast_voq_connector_group, cosq, gport_scheduler_fq, rc, rc);
                return(rc);
            }
            printf("  detached connGport(0x%x) from schedGport(0x%x)\n", gport_ucast_voq_connector_group, gport_scheduler_fq);
        }
    }

    /* delete connectors */
    for (nbr_queue_groups = 0; nbr_queue_groups < C3_NBR_QUEUE_GROUPS; nbr_queue_groups++) {
        switch (nbr_queue_groups) {
            case 0: gport_ucast_voq_connector_group = g_e2e_wfq_gports.gport_ucast_voq_connector_group0; break;
            default: gport_ucast_voq_connector_group = g_e2e_wfq_gports.gport_ucast_voq_connector_group1; break;
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
    for (nbr_fq_sched = 0; nbr_fq_sched < C3_NBR_FQ_SCHEDULERS; nbr_fq_sched++) {
        switch (nbr_fq_sched) {
            case 0: gport_scheduler_fq = g_e2e_wfq_gports.gport_ucast_scheduler_fq0; break;
            case 1: gport_scheduler_fq = g_e2e_wfq_gports.gport_ucast_scheduler_fq1; break;
            case 2: gport_scheduler_fq = g_e2e_wfq_gports.gport_ucast_scheduler_fq2; break;
            default: gport_scheduler_fq = g_e2e_wfq_gports.gport_ucast_scheduler_fq3; break;
        }

        rc = bcm_cosq_gport_detach(unit, g_e2e_wfq_gports.gport_ucast_scheduler, gport_scheduler_fq, 0);
        if (BCM_FAILURE(rc)) {
            printf("detach for SE(0x%x) failed, Error (%d, 0x%x)\n", gport_scheduler_fq, rc, rc);
            return(rc);
        }
        printf("  detached schedGport(0x%x)\n", gport_scheduler_fq);
    }

    /* delete intermediate Scheduler elements (SE) */
    for (nbr_fq_sched = 0; nbr_fq_sched < C3_NBR_FQ_SCHEDULERS; nbr_fq_sched++) {
        switch (nbr_fq_sched) {
            case 0: gport_scheduler_fq = g_e2e_wfq_gports.gport_ucast_scheduler_fq0; break;
            case 1: gport_scheduler_fq = g_e2e_wfq_gports.gport_ucast_scheduler_fq1; break;
            case 2: gport_scheduler_fq = g_e2e_wfq_gports.gport_ucast_scheduler_fq2; break;
            default: gport_scheduler_fq = g_e2e_wfq_gports.gport_ucast_scheduler_fq3; break;
        }

        rc = bcm_cosq_gport_delete(unit, gport_scheduler_fq);
        if (BCM_FAILURE(rc)) {
            printf("delete for SE(0x%x) failed, Error (%d, 0x%x)\n", gport_scheduler_fq, rc, rc);
            return(rc);
        }
        printf("  delete schedGport(0x%x)\n", gport_scheduler_fq);
    }

    /* delete scheduler  element at top of sub-tree */
    rc = bcm_cosq_gport_delete(unit, g_e2e_wfq_gports.gport_ucast_scheduler);
    if (BCM_FAILURE(rc)) {
        printf("delete for SE(0x%x) failed, Error (%d, 0x%x)\n", g_e2e_wfq_gports.gport_ucast_scheduler, rc, rc);
        return(rc);
    }
    printf("  delete schedGport(0x%x)\n", g_e2e_wfq_gports.gport_ucast_scheduler);

    printf("cint_e2e_wfq.c (teardown) completed with status (%s)\n", bcm_errmsg(rc));

    return(rc);
}


int e2e_wfq_verify(int unit)
{
    int                           rc = BCM_E_NONE;
    bcm_gport_t                   physical_port;
    int                           num_cos_levels;
    uint32                        flags, kbits_sec_min, kbits_sec_max;
    int                           mode, weight, exp_mode, exp_weight;
    int                           cosq, max_burst, slow_rate;
    bcm_gport_t                   gport_scheduler_fq;
    bcm_gport_t                   gport_ucast_voq_connector_group;
    bcm_gport_t                   gport_ucast_queue_group;
    int                           nbr_queue_groups, nbr_fq_sched;
    bcm_cosq_gport_connection_t   connection;


    printf("\nVerify Setup\n");

    /* Get rate on the E2E port */
    rc = bcm_cosq_gport_bandwidth_get(unit,g_e2e_wfq_gports.e2e_gport,0, &kbits_sec_min, &kbits_sec_max, &flags);
    if (rc != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set failed $rc\n");
        return rc;
    }
    if (kbits_sec_max != 500000) {
        printf("E2E interface(0x%08x) bandwidth_get rate(%d) expected rate(500000)\n", g_e2e_wfq_gports.e2e_gport, kbits_sec_max);
    }    

    /* retrive e2e port scheduling element */
    rc = bcm_cosq_gport_get(unit, g_e2e_wfq_gports.e2e_gport, &physical_port, &num_cos_levels, &flags);
    if (rc != BCM_E_NONE) {
        printf("bcm_cosq_gport_get on e2e Sched (0x%x) failed, error (%d)\n", g_e2e_wfq_gports.e2e_gport, rc); 
        return(rc);
    }
    if (flags != C3_DEFAULT_E2E_SCHED_FLAGS) {
        printf("gport get mismatch on E2E Sched (0x%x) expected flags(0x%x) retreived flags (0x%x)\n", g_e2e_wfq_gports.e2e_gport, C3_DEFAULT_E2E_SCHED_FLAGS, flags);
    }

    /* retreive type of scheduling element */
    rc = bcm_cosq_gport_get(unit, g_e2e_wfq_gports.gport_ucast_scheduler,
                                           &physical_port, &num_cos_levels, &flags);
    if (rc != BCM_E_NONE) {
        printf("bcm_cosq_gport_get on Sched (0x%x) failed, error (%d)\n", g_e2e_wfq_gports.gport_ucast_scheduler, rc);
        return(rc);
    }
    if (flags != C3_SCHED_FLAGS[g_e2e_wfq.sched.wfq_mode]) {
        printf("gport get mismatch on Sched (0x%x) expected flags(0x%x) retreived flags (0x%x)\n", g_e2e_wfq_gports.gport_ucast_scheduler, C3_SCHED_FLAGS[g_e2e_wfq.sched.wfq_mode], flags);
    }

    /* retreive SE scheduling discipline */
    rc = bcm_cosq_gport_sched_get(unit, g_e2e_wfq_gports.gport_ucast_scheduler, 0, &mode, &weight);
    if (rc != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_get on Sched (0x%x) failed, error (%d)\n", g_e2e_wfq_gports.gport_ucast_scheduler, rc);
        return(rc);
    }
    if (mode != BCM_COSQ_SP1) {
        printf("sched mode mismatch on Sched (0x%x) expected(BCM_COSQ_SP%d) retreived (BCM_COSQ_SP%d)\n", g_e2e_wfq_gports.gport_ucast_scheduler, (BCM_COSQ_SP1 - BCM_COSQ_SP0), (mode - BCM_COSQ_SP0));
    }
    if (weight != 0) {
        printf("sched weight mismatch on Sched (0x%x) expected(%d) retreived (%d)\n", g_e2e_wfq_gports.gport_ucast_scheduler, 0, weight);
    }

    /* retreive rate of Scheduler */
    rc = bcm_cosq_gport_bandwidth_get(unit, g_e2e_wfq_gports.gport_ucast_scheduler, 0,
                                                        &kbits_sec_min, &kbits_sec_max, &flags);
    if (rc != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_get on Sched (0x%x) failed, error (%d)\n", g_e2e_wfq_gports.gport_ucast_scheduler, rc);
        return(rc);
    }
    if ( (kbits_sec_max < (g_e2e_wfq.sched.cl_kbits_sec_max - g_e2e_wfq.margin.cl_kbits_sec_max_margin)) ||
         (kbits_sec_max > (g_e2e_wfq.sched.cl_kbits_sec_max + g_e2e_wfq.margin.cl_kbits_sec_max_margin)) ) {
        printf("bandwidth_get mismatch on Sched (0x%x) expected(%d) retreived (%d)\n", g_e2e_wfq_gports.gport_ucast_scheduler, C3_SCHED_KBITS_SEC_MAX, kbits_sec_max);
    }

    /* retreive scheduler burst size */
    rc = bcm_cosq_control_get(unit, g_e2e_wfq_gports.gport_ucast_scheduler, 0,
                                  bcmCosqControlBandwidthBurstMax, &max_burst);
    if (rc != BCM_E_NONE) {
        printf("bcm_cosq_control_get-BandwidthBurstMax on Sched (0x%x) failed, error (%d)\n", g_e2e_wfq_gports.gport_ucast_scheduler, rc);
        return(rc);
    }
    if ( (max_burst < (g_e2e_wfq.sched.cl_max_burst - g_e2e_wfq.margin.cl_max_burst_margin)) ||
         (max_burst > (g_e2e_wfq.sched.cl_max_burst + g_e2e_wfq.margin.cl_max_burst_margin)) ) {
        printf("control_get-BandwidthBurstMax mismatch on Sched (0x%x) expected(%d) retreived (%d)\n", g_e2e_wfq_gports.gport_ucast_scheduler, g_e2e_wfq.sched.cl_max_burst, max_burst);
    }

    /* retreive CL - WFQ scheduler configuration */
    if (g_e2e_wfq.sched.wfq_mode != 0) {
        e2e_wfq_sched_cl_wfq_verify(unit, g_e2e_wfq_gports.gport_ucast_scheduler);
    }

    /* retreive intermediate Scheduler elements (SE) */
    for (nbr_fq_sched = 0; nbr_fq_sched < C3_NBR_FQ_SCHEDULERS; nbr_fq_sched++) {
        switch (nbr_fq_sched) {
            case 0: gport_scheduler_fq = g_e2e_wfq_gports.gport_ucast_scheduler_fq0;
                    exp_mode = BCM_COSQ_SP0;
                    exp_weight = (g_e2e_wfq.sched.wfq_mode == 0) ? g_e2e_wfq.sched.fq0_weight: C3_SCHED_FQ0_DWGT_AP;
                    break;
            case 1: gport_scheduler_fq = g_e2e_wfq_gports.gport_ucast_scheduler_fq1;
                    exp_mode = BCM_COSQ_SP0;
                    exp_weight = (g_e2e_wfq.sched.wfq_mode == 0) ? g_e2e_wfq.sched.fq1_weight: C3_SCHED_FQ1_DWGT_AP;
                    break;
            case 2: gport_scheduler_fq = g_e2e_wfq_gports.gport_ucast_scheduler_fq2;
                    exp_mode = BCM_COSQ_SP0;
                    exp_weight = (g_e2e_wfq.sched.wfq_mode == 0) ? g_e2e_wfq.sched.fq2_weight: C3_SCHED_FQ2_DWGT_AP;
                    break;
            default: gport_scheduler_fq = g_e2e_wfq_gports.gport_ucast_scheduler_fq3;
                     exp_mode = BCM_COSQ_SP0;
                     exp_weight = (g_e2e_wfq.sched.wfq_mode == 0) ? C3_SCHED_FQ3_UPDATED_WEIGHT : C3_SCHED_FQ3_DWGT_AP;
                     break;
        }

        /* retreive type of scheduling element */
        rc = bcm_cosq_gport_get(unit, gport_scheduler_fq,
                                           &physical_port, &num_cos_levels, &flags);
        if (rc != BCM_E_NONE) {
            printf("bcm_cosq_gport_get on Sched (0x%x) failed, error (%d)\n", gport_scheduler_fq, rc);
            return(rc);
        }
        if (flags != C3_SCHED_FQ_FLAGS) {
            printf("gport get mismatch on Sched (0x%x) expected flags(0x%x) retreived flags (0x%x)\n", gport_scheduler_fq, C3_SCHED_FQ_FLAGS, flags);
        }


        /* retreive SE scheduling discipline */
        rc = bcm_cosq_gport_sched_get(unit, gport_scheduler_fq, 0, &mode, &weight);
        if (rc != BCM_E_NONE) {
            printf("bcm_cosq_gport_sched_get on Sched (0x%x) failed, error (%d)\n", gport_scheduler_fq, rc);
            return(rc);
        }
        if (mode != exp_mode) {
            printf("sched mode mismatch on Sched (0x%x) expected(BCM_COSQ_SP%d) retreived (BCM_COSQ_SP%d)\n", gport_scheduler_fq, (exp_mode - BCM_COSQ_SP0), (mode - BCM_COSQ_SP0));
        }
        if (weight != exp_weight) {
            printf("sched weight mismatch on Sched (0x%x) expected(%d) retreived (%d)\n", gport_scheduler_fq, exp_weight, weight);
        }

        /* retreive rate of Scheduler */
        rc = bcm_cosq_gport_bandwidth_get(unit, gport_scheduler_fq, 0,
                                                        &kbits_sec_min, &kbits_sec_max, &flags);
        if (rc != BCM_E_NONE) {
            printf("bcm_cosq_gport_bandwidth_get on Sched (0x%x) failed, error (%d)\n", gport_scheduler_fq, rc);
            return(rc);
        }
        if ( (kbits_sec_max < (g_e2e_wfq.sched.fq_kbits_sec_max - g_e2e_wfq.margin.fq_kbits_sec_max_margin)) ||
             (kbits_sec_max > (g_e2e_wfq.sched.fq_kbits_sec_max + g_e2e_wfq.margin.fq_kbits_sec_max_margin)) ) {
            printf("bandwidth_get mismatch on Sched (0x%x) expected(%d) retreived (%d)\n", gport_scheduler_fq, g_e2e_wfq.sched.fq_kbits_sec_max, kbits_sec_max);
        }


        /* retreive scheduler burst size */
        rc = bcm_cosq_control_get(unit, gport_scheduler_fq, 0,
                                  bcmCosqControlBandwidthBurstMax, &max_burst);
        if (rc != BCM_E_NONE) {
            printf("bcm_cosq_control_get-BandwidthBurstMax on Sched (0x%x) failed, error (%d)\n", gport_scheduler_fq, rc);
            return(rc);
        }
        if ( (max_burst < (g_e2e_wfq.sched.fq_max_burst - g_e2e_wfq.margin.fq_max_burst_margin)) ||
             (max_burst > (g_e2e_wfq.sched.fq_max_burst + g_e2e_wfq.margin.fq_max_burst_margin)) ) {
            printf("control_get-BandwidthBurstMax mismatch on Sched (0x%x) expected(%d) retreived (%d)\n", gport_scheduler_fq, g_e2e_wfq.sched.fq_max_burst, max_burst);
        }
    }

    /* retreive connectors */
    for (nbr_queue_groups = 0; nbr_queue_groups < C3_NBR_QUEUE_GROUPS; nbr_queue_groups++) {
        switch (nbr_queue_groups) {
            case 0: gport_ucast_voq_connector_group = g_e2e_wfq_gports.gport_ucast_voq_connector_group0;
                    gport_ucast_queue_group = g_e2e_wfq_gports.gport_ucast_queue_group0; break;
            default: gport_ucast_voq_connector_group = g_e2e_wfq_gports.gport_ucast_voq_connector_group1;
                     gport_ucast_queue_group = g_e2e_wfq_gports.gport_ucast_queue_group1; break;
        }

        int voq_id = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(gport_ucast_queue_group);
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(gport_ucast_queue_group, BCM_CORE_ALL, voq_id);
        rc = bcm_cosq_gport_get(unit, gport_ucast_queue_group,
                                &physical_port, &num_cos_levels, &flags);

        if (rc != BCM_E_NONE) {
            printf("bcm_cosq_gport_get on ucast queue group(0x%x) failed, error (%d)\n", gport_ucast_queue_group, rc);
            return(rc);
        }
    
        if (gport_ucast_queue_group == g_e2e_wfq_gports.gport_ucast_queue_group0) {
            if (flags != C3_QUEUE_FLAGS) {
                printf("gport get mismatch on ucast queue (0x%x) failed expected flags(0x%x) retrieved flags(0x%x)\n",
                       gport_ucast_queue_group, C3_QUEUE_FLAGS, flags);
            }
        } else {
            if (flags != (C3_QUEUE_FLAGS | BCM_COSQ_GPORT_TM_FLOW_ID)) {
                printf("gport get mismatch on ucast queue (0x%x) failed expected flags(0x%x) retrieved flags(0x%x)\n",
                       gport_ucast_queue_group, C3_QUEUE_FLAGS, flags);
            }
        }
        if (num_cos_levels != C3_NUM_COS) {
            printf("gport get mismatch on ucast queue (0x%x) expected numCos(%d) retreived numCos(%d)\n",
                       gport_ucast_queue_group, C3_NUM_COS, num_cos_levels);
        }


        /* retreive type of connectors */
        rc = bcm_cosq_gport_get(unit, gport_ucast_voq_connector_group,
                                           &physical_port, &num_cos_levels, &flags);
        if (rc != BCM_E_NONE) {
            printf("bcm_cosq_gport_get on Connector (0x%x) failed, error (%d)\n", gport_ucast_voq_connector_group, rc);
            return(rc);
        }
        if (flags != C3_CONN_FLAGS) {
            printf("gport get mismatch on Connector (0x%x) expected flags(0x%x) retreived flags (0x%x\n", gport_ucast_voq_connector_group, C3_CONN_FLAGS, flags);
        }
        if (num_cos_levels != C3_NUM_COS) {
            printf("gport get mismatch on Connector (0x%x) expected numCos(%d) retreived numCos(%d)\n", gport_ucast_voq_connector_group, C3_NUM_COS, num_cos_levels);
        }

        /* retreive rate of Connectors and corresponding burst size */
        for (cosq = 0; cosq < 4; cosq++) {
            /* retreive SE scheduling discipline */
            rc = bcm_cosq_gport_sched_get(unit, gport_ucast_voq_connector_group, cosq, &mode, &weight);
            if (rc != BCM_E_NONE) {
                printf("bcm_cosq_gport_sched_get on connector:cos (0x%x:%d) failed, error (%d)\n", gport_ucast_voq_connector_group, cosq, rc);
                return(rc);
            }
            if (mode != BCM_COSQ_SP0) {
                printf("connector mode mismatch on connector:cos (0x%x:%d) expected(BCM_COSQ_SP%d) retreived (BCM_COSQ_SP%d)\n", gport_ucast_voq_connector_group, cosq, (BCM_COSQ_SP0 - BCM_COSQ_SP0), (mode - BCM_COSQ_SP0));
            }
            if (weight != 0) {
                printf("connector weight mismatch on connector:cos (0x%x:%d) expected(%d) retreived (%d)\n", gport_ucast_voq_connector_group, cosq, 0, weight);
            }

            /* retreive rate on the VOQ Connector */
            rc = bcm_cosq_gport_bandwidth_get(unit, gport_ucast_voq_connector_group, cosq,
                                                                    &kbits_sec_min, &kbits_sec_max, &flags);
            if (rc != BCM_E_NONE) {
                printf("bcm_cosq_gport_bandwidth_get on connector:cosq (0x%x:%d) failed, error (%d)\n", gport_ucast_voq_connector_group, cosq, rc);
                return(rc);
            }

            if ( (kbits_sec_max < (g_e2e_wfq.sched.voq_conn_kbits_sec_max - g_e2e_wfq.margin.voq_conn_kbits_sec_max_margin)) ||
                 (kbits_sec_max > (g_e2e_wfq.sched.voq_conn_kbits_sec_max + g_e2e_wfq.margin.voq_conn_kbits_sec_max_margin)) ) {
                printf("bandwidth_get mismatch on connector:cosq (0x%x:%d) expected(%d) retreived (%d)\n", gport_ucast_voq_connector_group, cosq, g_e2e_wfq.sched.voq_conn_kbits_sec_max, kbits_sec_max);
            }

            rc = bcm_cosq_control_get(unit, gport_ucast_voq_connector_group, cosq,
                                  bcmCosqControlBandwidthBurstMax, &max_burst);
            if (rc != BCM_E_NONE) {
                printf("bcm_cosq_control_get-BandwidthBurstMax on Connector:cosq (0x%x:%d) failed, error (%d)\n", gport_ucast_voq_connector_group, cosq, rc);
                return(rc);
            }
            if ( (max_burst < (g_e2e_wfq.sched.voq_conn_max_burst - g_e2e_wfq.margin.voq_conn_max_burst_margin)) ||
                 (max_burst > (g_e2e_wfq.sched.voq_conn_max_burst + g_e2e_wfq.margin.voq_conn_max_burst_margin)) ) {
                printf("control_get-BandwidthBurstMax mismatch on Connector:cosq(0x%x:%d) expected(%d) retreived (%d)\n", gport_ucast_voq_connector_group, cosq, g_e2e_wfq.sched.cl_max_burst, max_burst);

            }

            /* retreive VOQ Connector slow state */
            rc = bcm_cosq_control_get(unit, gport_ucast_voq_connector_group, cosq,
                                  bcmCosqControlFlowSlowRate, &slow_rate);
            if (rc != BCM_E_NONE) {
                printf("bcm_cosq_control_get-FlowSlowRate on Connector:cosq (0x%x:%d) failed, error (%d)\n", gport_ucast_voq_connector_group, cosq, rc);
                return(rc);
            }
            if (slow_rate != 0) {
                printf("control_get-FlowSlowRate mismatch on Connector:cosq(0x%x:%d) expected(%d) retreived (%d)\n", gport_ucast_voq_connector_group, cosq, 0, slow_rate);
            }
        }

        /* retreive egress connection set information */
        connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
        connection.voq_connector = gport_ucast_voq_connector_group;
        rc = bcm_cosq_gport_connection_get(unit, &connection);
        if (rc != BCM_E_NONE) {
            printf("bcm_cosq_gport_connection_get on Egress, Connector (0x%x) failed, error (%d)\n", gport_ucast_voq_connector_group, rc);
            return(rc);
        }
        if (connection.voq != gport_ucast_queue_group) {
            printf("gport_connection_get on Egress, Connector (0x%x) VoQ association mismatch, Expected (0x%x), retreived (0x%x)\n", gport_ucast_voq_connector_group, gport_ucast_queue_group, connection.voq);
        }

        /* retreive ingress connection set information */
        connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
        connection.voq = gport_ucast_queue_group;
        rc = bcm_cosq_gport_connection_get(unit, &connection);
        if (rc != BCM_E_NONE) {
            printf("bcm_cosq_gport_connection_get on Ingress, VoQ (0x%x) failed, error (%d)\n", gport_ucast_queue_group, rc);
            return(rc);
        }
        if (connection.voq_connector != gport_ucast_voq_connector_group) {
            printf("gport_connection_get on Ingress, VoQ (0x%x) connector association mismatch, Expected (0x%x), retreived (0x%x)\n", gport_ucast_queue_group, gport_ucast_voq_connector_group, connection.voq_connector);
        }
    }

    printf("cint_e2e_wfq.c (verify) completed with status (%s)\n", bcm_errmsg(rc));

    return(rc);
}

/*
int e2e_wfq_init(int unit);
int e2e_wfq_start_run(int unit, int port, int mode); 
int e2e_wfq_verify(int unit);
int e2e_wfq_teardown(int unit);
*/

/*e2e_wfq_init(0);*/

/* Traffic passes after this step */
/*   mode => 0: independent, 1: discrete, 2: discrete class, -1: default (independent) */
/*e2e_wfq_start_run(0, 13, 0);*/

/* Traffic stops after this step */
/* e2e_wfq_teardown(0); */

/* Traffic passes after this step */
/* e2e_wfq_start_run(0, 13, 0); */

/* verify setup */
/*e2e_wfq_verify(0);*/
