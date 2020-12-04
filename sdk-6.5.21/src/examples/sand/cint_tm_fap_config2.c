/*~~~~~~~~~~~~~~~~~~~~~~~~~~Cosq: E2E Scheduling~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File: cint_tm_fap_config2.c
 * Purpose: Example of SP Validation
 * 
 * Environment
 *    -    Default Egress queue mapping in play.
 *    -    Default Egress Port Scheduler hierarchy in play.
 *    -    Default Ingress Scheduling Discipline (delay tolerance), queues sizes
 *    - Packet Size 1500
 *
 * - Port Scheduler
 *   - CL Scheduler (SP mode)
 *     - 4 FQ Schedulers
 *       - 1 Queue Group / Connector Group (4 elements in bundle) connected to
 *         corresponding FQ schedulers
 *       - 2nd Queue Group / Connector Group (4 elements in bundle) connected to
 *         corresponding FQ schedulers - Created by specifying BCM_COSQ_GPORT_TM_FLOW_ID flag.
 *
 * - test run 1
 *   - Destination port: System port. See SYS_PORT variable inside main. If
 *     destination port is not on modid 0, ingress configuration is done on unit 0
 *     and egress is done on unitid=sys_port_mod_id.
 *
 *   - Input
 *     - Queue Group 1 - Cos 0
 *       Input 500 Mbps
 *     - Queue Group 1 - Cos 1
 *       Input 500 Mbps
 *
 *   - Expected Output
 *     - Queue Group 1 - Cos 0
 *       Expected Output 500 Mbps
 *       Observed 500 Mbps
 *     - Queue Group 1 - Cos 1
 *       Expected Output 000 Mbps
 *       Observed 000 Mbps
 *
 * - test run 2
 *   - Input
 *     - Queue Group 1 - Cos 0
 *       Input 200 Mbps
 *     - Queue Group 1 - Cos 1
 *       Input 500 Mbps
 *
 *   - Expected Output
 *     - Queue Group 1 - Cos 0
 *       Expected Output 200 Mbps
 *       Observed 197 Mbps
 *     - Queue Group 1 - Cos 1
 *       Expected Output 300 Mbps
 *       Observed 328 Mbps
 * 
 * 
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  |                                                                                 | 
 *  |                                        |                                        | 
 *  |                                       SPR (500 Mbps)                            | 
 *  |                                        |                                        | 
 *  |                                       \/                                        | 
 *  |                                      +----+                                     | 
 *  |                                     /      \                                    | 
 *  |                                    /   HR   \                                   | 
 *  |                                   /          \                                  | 
 *  |                                  +------------+                                 | 
 *  |                                        |                                        | 
 *  |                                       SPR (500 Mbps)                            | 
 *  |                                        |                                        | 
 *  |                                       \/                                        | 
 *  |                                +----------------+                               | 
 *  |                               / CL (4 SP levels) \                              | 
 *  |                              /                    \                             | 
 *  |                             / SP0   SP1  SP2   SP3 \                            | 
 *  |                            +------------------------+                           | 
 *  |                              /      |      |     \                              | 
 *  |                             /       |      |      \                             | 
 *  |                           SPR     SPR     SPR    SPR                            | 
 *  |                    (500 Mbps)(500 Mbps)(500 Mbps)(500 Mbps)                     | 
 *  |                         /           |      |         \                          | 
 *  |                        /            |      |          \                         | 
 *  |                       |/           \/     \/          \/                        | 
 *  |                   +------+   +------+   +------+   +------+                     | 
 *  |                  /   FQ   \ /   FQ   \ /   FQ   \ /   FQ   \                    | 
 *  |                 +----------+----------+----------+----------+                   | 
 *  |                     \            |          |          /                        | 
 *  |                      \           |          |         /                         | 
 *  |                     SPR         SPR        SPR       SPR                        | 
 *  |                  (500 Mbps) (500 Mbps) (500 Mbps) (500 Mbps)                    | 
 *  |                        \         |          |      /                            |
 *  |                        \/       \/         \/     \/                            |
 *  |                     |     |  |     |   |     |  |     |                         |
 *  | +----------------+  |     | +----------------+  |     |                         | 
 *  | |Input2: 200 Mbps|  |     | |Input2: 500 Mbps|  |     |                         |
 *  | +----------------+  |     | +----------------+  |     |                         |
 *  |                     |     |  |     |   |     |  |     |                         |
 *  |                     +-----+  +-----+   +-----+  +-----+     +----------------+  |
 *  |                     Input1:500 Mbps                         |      KEY       |  |
 *  |                                                             +----------------+  |
 *  |                                                             |SPR- Rate Shaper|  |
 *  |                                                             |                |  |
 *  |                                                             +----------------+  |
 *  |                              +~~~~~~~~~~~~~~~~~~+                               |
 *  |                              |   Figure 5: SP   |                               |      
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 * 
 * 
 * Note - JR2 required setting credit request profiles before creating VOQ.
 * So before running cint_fap_config2.c cint, make sure to set the profiles:
 * cint src/examples/dnx/ingress_tm/cint_credit_request_profile.c
 * cint_credit_request_profile_backward_compatibilty_set(0, 4096);
 *
 * ISQ 1:
 * set config.bcm with:
 * diag_cosq_disable=1
 * flow_mapping_queue_base=0
 *
 * cd ../../../src/examples/sand
 * cint cint_tm_fap_config2.c
 * cint
 * tm_config2_set_queue_range(0, 4, 1023, 2);
 * tm_config2_set_queue_range(0, 1024, 32767, 0);
 * tm_config2_setup(in_port-1, 0 *C2_FRONT_PANEL_PORT_TYPE*, 0 *C2_RESOURCE_TYPE_LEGACY*);
 * tm_config2_setup(0, 2 *C2_ISQ_PORT_TYPE*, 0 *C2_RESOURCE_TYPE_LEGACY*);
 *
 * ISQ 2: 
 * set config.bcm with:
 * diag_cosq_disable=1
 * flow_mapping_queue_base=1024
 *
 * cd ../../../src/examples/sand
 * cint cint_tm_fap_config2.c
 * cint
 * tm_config2_set_queue_range(0, 1024, 30719, 1);
 * tm_config2_set_queue_range(0, 31744, 32767, 2);
 * tm_config2_setup(1, 2 *C2_ISQ_PORT_TYPE*, 0 *C2_RESOURCE_TYPE_LEGACY*);
 *
 * Warmboot test sequence:
 * coldboot
 *   export SOC_BOOT_FLAGS=0x000000
 *   ./bcm.user
 *   cint cint_warmboot_utilities.c
 *   cint cint_tm_fap_config2.c
 *
 *   cint
 *   tm_config2_setup(0, 0 *C2_FRONT_PANEL_PORT_TYPE*, 1 *C2_RESOURCE_TYPE_FIXED*);
 *   quit;
 *
 *   cint
 *   wb_sync_initiate(0);
 *   quit;
 *
 * warmboot
 *   export SOC_BOOT_FLAGS=0x200000
 *   ./bcm.user
 *   cint cint_tm_petra_config2.c
 *
 *   cint
 *   tm_config2_verify(0, 0 *C2_FRONT_PANEL_PORT_TYPE*, 1 *C2_RESOURCE_TYPE_FIXED*);
 *   quit;
 */

/*
 * General Definitions
 */
int C2_NUM_COS = 4;

int C2_E2E_INTERFACE_KBITS_SEC_MAX = 1000000; /* 1gbps */
int C2_E2E_INTERFACE_KBITS_SEC_MAX_MARGIN = 500; /* 500 kbps */

int C2_E2E_PORT_KBITS_SEC_MAX = 500000; /* 500mbps */
int C2_E2E_PORT_KBITS_SEC_MAX_MARGIN = 10; /* 10 kbits */

int C2_DEFAULT_E2E_SCHED_FLAGS = (BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_HR_ENHANCED);
int C2_SCHED_FLAGS = (BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE1_4SP);
int C2_SCHED_MODE = BCM_COSQ_SP1;
int C2_SCHED_KBITS_SEC_MAX = 500000; /* 500 Mbps */
int C2_SCHED_KBITS_SEC_MAX_MARGIN = 10; /* 10 Kbps */
int C2_SCHED_MAX_BURST = 3000;
int C2_SCHED_MAX_BURST_MARGIN = 75;

int C2_SCHED_FQ_FLAGS = (BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_FQ);
int C2_SCHED_FQ_KBITS_SEC_MAX = 500000; /* 500 Mbps */
int C2_SCHED_FQ_KBITS_SEC_MAX_MARGIN = 10; /* 10 Kbps */
int C2_SCHED_FQ_MAX_BURST = 3000;
int C2_SCHED_FQ_MAX_BURST_MARGIN = 75;

int C2_CONN_FLAGS = (BCM_COSQ_GPORT_VOQ_CONNECTOR);
int C2_CONN_KBITS_SEC_MAX = 500000; /* 500 Mbps */
int C2_CONN_KBITS_SEC_MAX_MARGIN = 10; /* 10 Kbps */
int C2_CONN_MAX_BURST = 3000;
int C2_CONN_MAX_BURST_MARGIN = 75;

int C2_QUEUE_FLAGS = (BCM_COSQ_GPORT_UCAST_QUEUE_GROUP);

int C2_ISQ_ROOT_KBITS_SEC_MAX = 500000; /* 500000 Mbps */
int C2_ISQ_ROOT_KBITS_SEC_MAX_MARGIN = 500; /* 500 kbps */

/* handles */
int c2_ingr_unit = 0;
int c2_egr_unit = 0;
int c2_ingr_modid = 0;
int c2_egr_modid = 0;
bcm_gport_t c2_gport_ucast_voq_connector_group0[32];
bcm_gport_t c2_gport_ucast_voq_connector_group1[32];
bcm_gport_t c2_gport_ucast_queue_group0[32];
bcm_gport_t c2_gport_ucast_queue_group1[32];
bcm_gport_t c2_gport_ucast_scheduler[32];
bcm_gport_t c2_gport_ucast_scheduler_fq0[32];
bcm_gport_t c2_gport_ucast_scheduler_fq1[32];
bcm_gport_t c2_gport_ucast_scheduler_fq2[32];
bcm_gport_t c2_gport_ucast_scheduler_fq3[32];
bcm_gport_t c2_e2e_parent_gport[32];
bcm_gport_t c2_e2e_gport[32];
bcm_gport_t c2_sys_gport[32];

int is_dnx;
bcm_device_member_get(c2_ingr_unit, 0, bcmDeviceMemberDNX, &is_dnx);
if (is_dnx) {
    /* Default credit size for DPP was 1K.
     * In order to utilize the cint also for DNX device
     * where credit size by default is 4K
     * we multiply by 4 the MaxBurst
     */
    int credit_worth = *(dnxc_data_get(c2_ingr_unit, "iqs", "credit", "worth", NULL));
    int multiplier = credit_worth / 1024;

    C2_SCHED_MAX_BURST *= multiplier;
    C2_SCHED_MAX_BURST_MARGIN *= multiplier;
    C2_SCHED_FQ_MAX_BURST *= multiplier;
    C2_SCHED_FQ_MAX_BURST_MARGIN *= multiplier;
    C2_CONN_MAX_BURST *= multiplier;
    C2_CONN_MAX_BURST_MARGIN *= multiplier;
}


int C2_NBR_QUEUE_GROUPS = 2;
int C2_NBR_FQ_SCHEDULERS = 4;

int c2_use_same_handles = 0;

/* for port type variable */
int C2_FRONT_PANEL_PORT_TYPE = 0;
int C2_RCY_PORT_TYPE = 1;
int C2_ISQ_PORT_TYPE = 2;

/* resource type variable */
int C2_RESOURCE_TYPE_LEGACY = 0;
int C2_RESOURCE_TYPE_FIXED = 1;

 
int c2_base_sched = 32768;
int c2_base_fq_sched = 32773;
int c2_base_voq_connector = 32;
int c2_base_voq = 4;


bcm_gport_t tm_config2_get_flow_handle(int port_id)
{
    int idx = port_id;
    return c2_gport_ucast_queue_group1[idx];
}
bcm_gport_t tm_config2_get_ucast_handle(int port_id)
{
    int idx = port_id;
    return c2_gport_ucast_queue_group0[idx];
}
bcm_gport_t tm_config2_get_sysport_handle(int port_id)
{
    int idx = port_id;
    return c2_sys_gport[idx];
}
void tm_config2_display_handles(int port_id)
{
    int idx = port_id;

    printf("  e2eGport(0x%x) schedGport(0x%x)\n",
            c2_e2e_gport[idx], c2_gport_ucast_scheduler[idx]);
    printf("    l3_fq0(0x%x) l3_fq1(0x%x) l3_fq2(0x%x) l3_fq3(0x%x)\n",
            c2_gport_ucast_scheduler_fq0[idx], c2_gport_ucast_scheduler_fq1[idx],
            c2_gport_ucast_scheduler_fq2[idx], c2_gport_ucast_scheduler_fq3[idx]);
    printf("    connGrp0Gport(0x%x) connGrp1Gport(0x%x)\n",
            c2_gport_ucast_voq_connector_group0[idx], c2_gport_ucast_voq_connector_group1[idx]);
    printf("    ucastGrp0Gport(0x%x) ucastGrp1Gport(0x%x)\n",
            c2_gport_ucast_queue_group0[idx], c2_gport_ucast_queue_group1[idx]);
}

int tm_config2_form_handles(int port_id, int port_type)
{
    int idx = port_id;
    int port, system_port;
    bcm_gport_t dest_gport[32];
    int rv = BCM_E_NONE;

    if (port_type == C2_FRONT_PANEL_PORT_TYPE) {
        port = idx + 1;
        system_port = idx + 1;
    
        BCM_GPORT_SYSTEM_PORT_ID_SET(c2_sys_gport[idx], system_port);
    
        rv = bcm_stk_sysport_gport_get(c2_ingr_unit, c2_sys_gport[idx], dest_gport[idx]);
        if (rv != BCM_E_NONE) {
            printf("bcm_stk_sysport_gport_get gport failed: $rv, unit=%d, sysport=0x%x, destport=0x%x\n",
                   c2_ingr_unit, c2_sys_gport[idx], dest_gport[idx]);
            return rv;
        }

        port = BCM_GPORT_MODPORT_PORT_GET(dest_gport[idx]);
        BCM_COSQ_GPORT_E2E_PORT_SET(c2_e2e_gport[idx], port);

        rv = bcm_fabric_port_get(c2_egr_unit,
                      c2_e2e_gport[idx],
                     0,
                     &c2_e2e_parent_gport[idx]);
        if (rv != BCM_E_NONE) {
              printf("bcm_fabric_port_get failed $rv\n");
            return(rv);
        }

        printf("c2_e2e_parent_gport: 0x%x\n", c2_e2e_parent_gport[idx]);
    }
    else {
        return(BCM_E_PARAM);
    }

    BCM_GPORT_SCHEDULER_CORE_SET( c2_gport_ucast_scheduler[idx], (c2_base_sched + (port_id * 4)), 0 );

    BCM_GPORT_SCHEDULER_CORE_SET( c2_gport_ucast_scheduler_fq0[idx], (c2_base_fq_sched + (port_id * 16)), 0 );
    BCM_GPORT_SCHEDULER_CORE_SET( c2_gport_ucast_scheduler_fq1[idx], (c2_base_fq_sched + (port_id * 16) + 4), 0 );
    BCM_GPORT_SCHEDULER_CORE_SET( c2_gport_ucast_scheduler_fq2[idx], (c2_base_fq_sched + (port_id * 16) + 8), 0 );
    BCM_GPORT_SCHEDULER_CORE_SET( c2_gport_ucast_scheduler_fq3[idx], (c2_base_fq_sched + (port_id * 16) + 12), 0 );

    BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET( c2_gport_ucast_voq_connector_group0[idx], (c2_base_voq_connector + (port_id * 8)), 0 );
    BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET( c2_gport_ucast_voq_connector_group1[idx], (c2_base_voq_connector + (port_id * 8) + 4), 0 );

    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_SET( c2_gport_ucast_queue_group0[idx], (c2_base_voq + (port_id * 8)), 0 );
    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_SET( c2_gport_ucast_queue_group1[idx], (c2_base_voq + (port_id * 8) + 4), 0 );

    return(rv);
}

/* 
 * Set queue ranges - must be done before any queues are allocated
 */
int tm_config2_set_queue_range(int unit, int bottom_queue, int top_queue, int port_type)
{
    bcm_error_t rv;
    bcm_fabric_control_t control_type;

    if (port_type == C2_ISQ_PORT_TYPE) {
        control_type = bcmFabricShaperQueueMin;
        rv = bcm_fabric_control_set(unit, control_type, bottom_queue);
        if (rv != BCM_E_NONE) {
            printf("Error, in queue range set, $control_type $rv \n");
            return rv;
        }
        control_type = bcmFabricShaperQueueMax;
        rv = bcm_fabric_control_set(unit, control_type, top_queue);
        if (rv != BCM_E_NONE) {
            printf("Error, in queue range set, $control_type $rv \n");
            return rv;
        }
    } else {
        control_type = bcmFabricQueueMin;
        rv = bcm_fabric_control_set(unit, control_type, bottom_queue);
        if (rv != BCM_E_NONE) {
            printf("Error, in queue range set, $control_type $rv \n");
            return rv;
        }
        control_type = bcmFabricQueueMax;
        rv = bcm_fabric_control_set(unit, control_type, top_queue);
        if (rv != BCM_E_NONE) {
            printf("Error, in queue range set, $control_type $rv \n");
            return rv;
        }
    }
    
    return rv;
}
int tm_config2_configure_isq_root(int unit, int kbits_sec_max)
{
    int rv;
    bcm_gport_t isq_root;
    
    BCM_COSQ_GPORT_ISQ_ROOT_CORE_SET(isq_root, 0);
    
    rv = bcm_cosq_gport_bandwidth_set(unit,
                    isq_root,
                    0,
                    0,
                    kbits_sec_max,
                    0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set for ISQ root failed $rv\n");
    }
    
    return rv;
}

int tm_config2_setup(int port_id, int port_type, int res_type)
{
    int idx, port, system_port;
    bcm_gport_t dest_gport[32];
    int num_cos = 4;
    int flags = 0, default_flags = 0;
    bcm_error_t rv = BCM_E_NONE;
    int voq;
    bcm_cosq_gport_connection_t connection;
    int kbits_sec_max;
    int max_burst;
    int cosq = 0;
    bcm_cosq_delay_tolerance_t delay_tolerance;
    bcm_cosq_voq_connector_gport_t config;
    uint32 dummy_flags ;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    int core = 0;
    int voq_id;

    printf("\nSetup\n");

    idx = port_id;

    if ( (res_type != C2_RESOURCE_TYPE_LEGACY) && (res_type != C2_RESOURCE_TYPE_FIXED) ) {
        printf("invaid res_type(%d)\n", res_type);
        return(BCM_E_PARAM);
    }

    if ( (res_type == C2_RESOURCE_TYPE_FIXED) && (port_type != C2_FRONT_PANEL_PORT_TYPE) ) {
        printf("For fixed resource type Port type has to be Front Panel, invalid port type (%d)\n", port_type);
        return(BCM_E_PARAM);
    }

    if (c2_use_same_handles == 1) {
        default_flags = BCM_COSQ_GPORT_WITH_ID;
        tm_config2_display_handles(idx);
    }

    if (port_type == C2_FRONT_PANEL_PORT_TYPE) {
        port = idx + 1;
        system_port = idx + 1;
    
        rv = bcm_port_get(c2_egr_unit, port, &dummy_flags, &interface_info, &mapping_info);
        if (rv != BCM_E_NONE) {
            printf("bcm_port_get failed $rv\n");
            return rv;
        }
        printf("\n core(%d)\n",  mapping_info.core);    
        core =  mapping_info.core;

        BCM_GPORT_SYSTEM_PORT_ID_SET(c2_sys_gport[idx], system_port);
        BCM_GPORT_MODPORT_SET(dest_gport[idx], c2_egr_modid + core, port); 
    
        rv = bcm_stk_sysport_gport_get(c2_ingr_unit, c2_sys_gport[idx], dest_gport[idx]);
        if (rv != BCM_E_NONE) {
            printf("bcm_stk_sysport_gport_get gport failed: $rv, unit=%d, sysport=0x%x, destport=0x%x\n",
                   c2_ingr_unit, c2_sys_gport[idx], dest_gport[idx]);
            return rv;
        }
    } else if (port_type == C2_RCY_PORT_TYPE) {

        /* Use port 10 for test */
        port = idx + 1;
        system_port = idx + 1;
        BCM_GPORT_SYSTEM_PORT_ID_SET(c2_sys_gport[idx], system_port);
        BCM_GPORT_MODPORT_SET(dest_gport[idx], c2_egr_modid, port);

        rv = bcm_stk_sysport_gport_set(c2_ingr_unit, c2_sys_gport[idx], dest_gport[idx]);

        if (rv != BCM_E_NONE) {
            printf("bcm_stk_sysport_gport_set failed $rv: unit=%d, sysport=0x%x, destport=0x%x\n",
                    c2_ingr_unit, c2_sys_gport[idx], dest_gport[idx]);
            return rv;
        }
    } else if (port_type == C2_ISQ_PORT_TYPE) {
        system_port = idx + 1;
        rv = tm_config2_configure_isq_root(c2_ingr_unit, C2_ISQ_ROOT_KBITS_SEC_MAX);
        
        /* For ISQ, all setup is flow based */
        BCM_COSQ_GPORT_ISQ_ROOT_CORE_SET(dest_gport[idx], 0);
        /*  BCM_GPORT_MODPORT_SET(dest_gport[idx], c2_ingr_modid, 0);*/

        BCM_GPORT_SYSTEM_PORT_ID_SET(c2_sys_gport[idx], system_port);
        
        rv = bcm_stk_sysport_gport_set(c2_ingr_unit, c2_sys_gport[idx], dest_gport[idx]);

        
        if (rv != BCM_E_NONE) {
            printf("bcm_stk_sysport_gport_set failed $rv: unit=%d, sysport=0x%x, destport=0x%x\n",
                    c2_ingr_unit, c2_sys_gport[idx], dest_gport[idx]);
            return rv;
        }      
          
    } else {
        printf("Invalid port type(%d) passed in 0-voq 1-rcy 2-isq\n", port_type);
        return BCM_E_PARAM;
    }


    port = BCM_GPORT_MODPORT_PORT_GET(dest_gport[idx]);    
    printf("idx(%d) dest_gport(%d) port(%d) modid(%d)\n",  idx, dest_gport[idx], port, c2_egr_modid + core);    

    if (res_type == C2_RESOURCE_TYPE_FIXED) {
        default_flags = BCM_COSQ_GPORT_WITH_ID;
        rv = tm_config2_form_handles(idx, port_type);
        if (rv != BCM_E_NONE) {
            printf("error in forming handles\n");
            return(rv);
        }
    }

    /* create scheduler */
    flags = C2_SCHED_FLAGS | default_flags;

    rv = bcm_cosq_gport_add(c2_egr_unit, dest_gport[idx], 1,
                flags, &c2_gport_ucast_scheduler[idx]);
    
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return rv;
    }
    
    /* COS 0 */
    rv = bcm_cosq_gport_sched_set(c2_egr_unit,
                  c2_gport_ucast_scheduler[idx],
                  0,
                  C2_SCHED_MODE,
                  0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }

    if (port_type == C2_ISQ_PORT_TYPE) {
        BCM_COSQ_GPORT_ISQ_ROOT_CORE_SET(c2_e2e_gport[idx], 0);
    } else { 
        BCM_COSQ_GPORT_E2E_PORT_SET(c2_e2e_gport[idx], port);
    }

    rv = bcm_cosq_gport_attach(c2_egr_unit, c2_e2e_gport[idx], 
                   c2_gport_ucast_scheduler[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }
    
    /* create scheduler */
    flags = C2_SCHED_FQ_FLAGS | default_flags;
    
    rv = bcm_cosq_gport_add(c2_egr_unit, dest_gport[idx], 1,
                flags, &c2_gport_ucast_scheduler_fq0[idx]);
    
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return rv;
    }
    rv = bcm_cosq_gport_sched_set(c2_egr_unit,
                  c2_gport_ucast_scheduler_fq0[idx],
                  0,
                  BCM_COSQ_SP0,
                  0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }
    
    rv = bcm_cosq_gport_attach(c2_egr_unit, c2_gport_ucast_scheduler[idx], 
                   c2_gport_ucast_scheduler_fq0[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }
    
    
    /* create scheduler */
    flags = C2_SCHED_FQ_FLAGS | default_flags;
     
    rv = bcm_cosq_gport_add(c2_egr_unit, dest_gport[idx], 1,
                flags, &c2_gport_ucast_scheduler_fq1[idx]);
    
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return rv;
    }
    rv = bcm_cosq_gport_sched_set(c2_egr_unit,
                  c2_gport_ucast_scheduler_fq1[idx],
                  0,
                  BCM_COSQ_SP1,
                  0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }
    
    rv = bcm_cosq_gport_attach(c2_egr_unit, c2_gport_ucast_scheduler[idx], 
                   c2_gport_ucast_scheduler_fq1[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }
    
    
    /* create scheduler */
    flags = C2_SCHED_FQ_FLAGS | default_flags;
    
    rv = bcm_cosq_gport_add(c2_egr_unit, dest_gport[idx], 1,
                flags, &c2_gport_ucast_scheduler_fq2[idx]);
    
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return rv;
    }
    
    rv = bcm_cosq_gport_sched_set(c2_egr_unit,
                  c2_gport_ucast_scheduler_fq2[idx],
                  0,
                  BCM_COSQ_SP2,
                  0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }
    
    
    rv = bcm_cosq_gport_attach(c2_egr_unit, c2_gport_ucast_scheduler[idx], 
                   c2_gport_ucast_scheduler_fq2[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }
    
    /* create scheduler */
    flags = C2_SCHED_FQ_FLAGS | default_flags;
    
    rv = bcm_cosq_gport_add(c2_egr_unit, dest_gport[idx], 1,
                flags, &c2_gport_ucast_scheduler_fq3[idx]);
    
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add scheduler failed $rv\n");
        return rv;
    }
    
    rv = bcm_cosq_gport_sched_set(c2_egr_unit,
                  c2_gport_ucast_scheduler_fq3[idx],
                  0,
                  BCM_COSQ_SP3,
                  0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }
    
    rv = bcm_cosq_gport_attach(c2_egr_unit, c2_gport_ucast_scheduler[idx], 
                   c2_gport_ucast_scheduler_fq3[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }
    
    /* create voq connector */          
    if(port_type == C2_ISQ_PORT_TYPE){
        BCM_COSQ_GPORT_ISQ_ROOT_CORE_SET(config.port, 0);
    } else {
        BCM_GPORT_MODPORT_SET(config.port, c2_egr_modid + core, port);
    }

    config.flags = C2_CONN_FLAGS | default_flags;
    config.numq = num_cos;
    config.remote_modid = c2_ingr_modid + core;
    config.nof_remote_cores = 1;
    rv = bcm_cosq_voq_connector_gport_add(c2_egr_unit, &config, &c2_gport_ucast_voq_connector_group0[idx]);
    
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_voq_connector_gport_add connector failed $rv\n");
        return rv;
    }
    
    /* COS 0 */
    rv = bcm_cosq_gport_sched_set(c2_egr_unit,
                  c2_gport_ucast_voq_connector_group0[idx],
                  0,
                  BCM_COSQ_SP0,
                  0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }
    
    rv = bcm_cosq_gport_attach(c2_egr_unit, c2_gport_ucast_scheduler_fq0[idx], 
                   c2_gport_ucast_voq_connector_group0[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }
    
    
    /* COS 1 */
    rv = bcm_cosq_gport_sched_set(c2_egr_unit,
                  c2_gport_ucast_voq_connector_group0[idx],
                  1,
                  BCM_COSQ_SP0,
                  0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }
    
    rv = bcm_cosq_gport_attach(c2_egr_unit, c2_gport_ucast_scheduler_fq1[idx], 
                   c2_gport_ucast_voq_connector_group0[idx], 1);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }

    /* COS 2 */
    rv = bcm_cosq_gport_sched_set(c2_egr_unit,
                  c2_gport_ucast_voq_connector_group0[idx],
                  2,
                  BCM_COSQ_SP0,
                  0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }
    
    rv = bcm_cosq_gport_attach(c2_egr_unit, c2_gport_ucast_scheduler_fq2[idx], 
                   c2_gport_ucast_voq_connector_group0[idx], 2);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }
    
    /* COS 3 */
    rv = bcm_cosq_gport_sched_set(c2_egr_unit,
                  c2_gport_ucast_voq_connector_group0[idx],
                  3,
                  BCM_COSQ_SP0,
                  0);
    
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }
    
    rv = bcm_cosq_gport_attach(c2_egr_unit, c2_gport_ucast_scheduler_fq3[idx], 
                   c2_gport_ucast_voq_connector_group0[idx], 3);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }
    
    /* Create VOQ */
    flags = C2_QUEUE_FLAGS | default_flags;
    
    if (port_type == C2_ISQ_PORT_TYPE) {
        flags = BCM_COSQ_GPORT_ISQ | BCM_COSQ_GPORT_TM_FLOW_ID | default_flags;
    }
    
    /** Creating VOQs only with system port gport is mandatory for JR2 (JR1 also supports it) */
    rv = bcm_cosq_gport_add(c2_ingr_unit, c2_sys_gport[idx], num_cos,
                flags, &c2_gport_ucast_queue_group0[idx]);
    
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add queue 0 failed $rv\n");
        return rv;
    }

    if (port_type != C2_ISQ_PORT_TYPE)
    {
        /*bcm_cosq_gport_add will a voq gport with core - BCM_CORE_ALL. 
          In order to be consistent with the asymmetric scheme to be created
          the core is overwrited.*/
        voq_id = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(c2_gport_ucast_queue_group0[idx]);
        BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(c2_gport_ucast_queue_group0[idx], core, voq_id);
    }


    printf("ucast gport(0x%08x)\n", c2_gport_ucast_queue_group0[idx]);
    /* connect voq to flow */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
    connection.remote_modid = c2_egr_modid + core;
    connection.voq = c2_gport_ucast_queue_group0[idx];
    connection.voq_connector = c2_gport_ucast_voq_connector_group0[idx];
    
    rv = bcm_cosq_gport_connection_set(c2_ingr_unit, &connection);
    
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_connection_set ingress 0 failed $rv\n");
        return rv;
    }
    
    /* connect flow to voq */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
    connection.remote_modid = c2_ingr_modid + core;
    connection.voq = c2_gport_ucast_queue_group0[idx];
    connection.voq_connector = c2_gport_ucast_voq_connector_group0[idx];
    
    rv = bcm_cosq_gport_connection_set(c2_egr_unit, &connection);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_connection_set egress failed $rv\n");
        return rv;
    }
    
    
    /* configuration for another queue group */
    /* create voq connector */          
    if(port_type == C2_ISQ_PORT_TYPE){
            BCM_COSQ_GPORT_ISQ_ROOT_CORE_SET(config.port, 0);
    } else {
        BCM_GPORT_MODPORT_SET(config.port, c2_egr_modid + core, port);
    }

    config.flags = C2_CONN_FLAGS | default_flags;
    config.numq = num_cos;
    config.remote_modid = c2_ingr_modid + core;
    config.nof_remote_cores = 1;
    
    rv = bcm_cosq_voq_connector_gport_add(c2_egr_unit, &config, &c2_gport_ucast_voq_connector_group1[idx]);
    
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_voq_connector_gport_add connector failed $rv\n");
        return rv;
    }
    
    
    /* COS 0 */
    rv = bcm_cosq_gport_sched_set(c2_egr_unit,
                  c2_gport_ucast_voq_connector_group1[idx],
                  0,
                  BCM_COSQ_SP0,
                  0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }
    
    rv = bcm_cosq_gport_attach(c2_egr_unit, c2_gport_ucast_scheduler_fq0[idx], 
                   c2_gport_ucast_voq_connector_group1[idx], 0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }
        
    /* COS 1 */
    rv = bcm_cosq_gport_sched_set(c2_egr_unit,
                  c2_gport_ucast_voq_connector_group1[idx],
                  1,
                  BCM_COSQ_SP0,
                  0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }
    
    rv = bcm_cosq_gport_attach(c2_egr_unit, c2_gport_ucast_scheduler_fq1[idx], 
                   c2_gport_ucast_voq_connector_group1[idx], 1);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }

    /* COS 2 */
    rv = bcm_cosq_gport_sched_set(c2_egr_unit,
                  c2_gport_ucast_voq_connector_group1[idx],
                  2,
                  BCM_COSQ_SP0,
                  0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }
    
    rv = bcm_cosq_gport_attach(c2_egr_unit, c2_gport_ucast_scheduler_fq2[idx], 
                   c2_gport_ucast_voq_connector_group1[idx], 2);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }
    
    /* COS 3 */
    rv = bcm_cosq_gport_sched_set(c2_egr_unit,
                  c2_gport_ucast_voq_connector_group1[idx],
                  3,
                  BCM_COSQ_SP0,
                  0);
    
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set failed $rv\n");
        return rv;
    }
    
    rv = bcm_cosq_gport_attach(c2_egr_unit, c2_gport_ucast_scheduler_fq3[idx], 
                   c2_gport_ucast_voq_connector_group1[idx], 3);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_attach failed $rv\n");
        return rv;
    }
    
    flags = C2_QUEUE_FLAGS |  BCM_COSQ_GPORT_TM_FLOW_ID | default_flags;

    if (port_type == C2_ISQ_PORT_TYPE) {
        flags = BCM_COSQ_GPORT_ISQ | BCM_COSQ_GPORT_TM_FLOW_ID | default_flags;
    }

    /** Creating VOQs only with system port gport is mandatory for JR2 (JR1 also supports it) */
    rv = bcm_cosq_gport_add(c2_egr_unit, c2_sys_gport[idx], num_cos,
                flags, &c2_gport_ucast_queue_group1[idx]);
    
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_add queue 1 failed $rv\n");
        return rv;
    }
    printf("ucast gport(0x%08x)\n", c2_gport_ucast_queue_group1[idx]);
    
    if (port_type != C2_ISQ_PORT_TYPE)
    {
        /*bcm_cosq_gport_add will a voq gport with core - BCM_CORE_ALL. 
          In order to be consistent with the asymmetric scheme to be created
          the core is overwrited.*/
         voq_id = BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(c2_gport_ucast_queue_group1[idx]);
         BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(c2_gport_ucast_queue_group1[idx], core, voq_id);
    }

    /* connect voq to flow */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
    connection.remote_modid = c2_egr_modid + core;
    connection.voq = c2_gport_ucast_queue_group1[idx];
    connection.voq_connector = c2_gport_ucast_voq_connector_group1[idx];
    
    rv = bcm_cosq_gport_connection_set(c2_ingr_unit, &connection);
    
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_connection_set ingress failed $rv\n");
        return rv;
    }
    
    /* connect voq to flow */
    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
    connection.remote_modid = c2_ingr_modid + core;
    connection.voq = c2_gport_ucast_queue_group1[idx];
    connection.voq_connector = c2_gport_ucast_voq_connector_group1[idx];
    
    rv = bcm_cosq_gport_connection_set(c2_egr_unit, &connection);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_connection_set egress failed $rv\n");
        return rv;
    }
    
    
    /*
     * Set rate on the E2E Interface
     */
    if ((port_type != C2_ISQ_PORT_TYPE) &&
        (port_type != C2_RCY_PORT_TYPE)){

        BCM_COSQ_GPORT_E2E_PORT_SET(c2_e2e_gport[idx], port);
        kbits_sec_max = C2_E2E_INTERFACE_KBITS_SEC_MAX; /* 1Gbps */
     
        rv = bcm_fabric_port_get(c2_egr_unit,
                                 c2_e2e_gport[idx],
                                 0,
                                 &c2_e2e_parent_gport[idx]);
        if (rv != BCM_E_NONE) {
            printf("bcm_fabric_port_get failed $rv\n");
            return rv;
        }
    
        rv = bcm_cosq_gport_bandwidth_set(c2_egr_unit,
                                          c2_e2e_parent_gport[idx],
                                          0,
                                          0,
                                          kbits_sec_max,
                                          0);
        if (rv != BCM_E_NONE) {
             printf("bcm_cosq_gport_bandwidth_set if failed $rv\n");
             return rv;
        }
    }  
  
    /*
     * Set rate on the E2E port
     */
    kbits_sec_max = C2_E2E_PORT_KBITS_SEC_MAX; /* 500Mbps */

    if (port_type == C2_ISQ_PORT_TYPE) {
        BCM_COSQ_GPORT_ISQ_ROOT_CORE_SET(c2_e2e_gport[idx], 0);
    } else { 
        BCM_COSQ_GPORT_E2E_PORT_SET(c2_e2e_gport[idx], port);
    }
    rv = bcm_cosq_gport_bandwidth_set(c2_egr_unit,
                      c2_e2e_gport[idx],
                      0,
                      0,
                      kbits_sec_max,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set failed $rv\n");
        return rv;
    }
        
    /*
     * Set rate on the Scheduler (CL)
     */
    kbits_sec_max = C2_SCHED_KBITS_SEC_MAX;
    max_burst = C2_SCHED_MAX_BURST;
    /* Scheduler */
    rv = bcm_cosq_gport_bandwidth_set(c2_egr_unit,
                      c2_gport_ucast_scheduler[idx],
                      0,
                      0,
                      kbits_sec_max,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set sched failed $rv\n");
        return rv;
    }
    
    /* Set max burst on the Scheduler */
    rv = bcm_cosq_control_set(c2_egr_unit,
                  c2_gport_ucast_scheduler[idx],
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
    kbits_sec_max = C2_SCHED_FQ_KBITS_SEC_MAX;
    max_burst = C2_SCHED_FQ_MAX_BURST;
    rv = bcm_cosq_gport_bandwidth_set(c2_egr_unit,
                      c2_gport_ucast_scheduler_fq0[idx],
                      0,
                      0,
                      kbits_sec_max,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set sched failed $rv\n");
        return(rv);
    }
    
    rv = bcm_cosq_control_set(c2_egr_unit,
                  c2_gport_ucast_scheduler_fq0[idx],
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
    kbits_sec_max = C2_SCHED_FQ_KBITS_SEC_MAX;
    max_burst = C2_SCHED_FQ_MAX_BURST;
    rv = bcm_cosq_gport_bandwidth_set(c2_egr_unit,
                      c2_gport_ucast_scheduler_fq1[idx],
                      0,
                      0,
                      kbits_sec_max,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set sched failed $rv\n");
        return(rv);
    }
    
    rv = bcm_cosq_control_set(c2_egr_unit,
                  c2_gport_ucast_scheduler_fq1[idx],
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
    kbits_sec_max = C2_SCHED_FQ_KBITS_SEC_MAX;
    max_burst = C2_SCHED_FQ_MAX_BURST;
    rv = bcm_cosq_gport_bandwidth_set(c2_egr_unit,
                      c2_gport_ucast_scheduler_fq2[idx],
                      0,
                      0,
                      kbits_sec_max,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set sched failed $rv\n");
        return(rv);
    }
    
    rv = bcm_cosq_control_set(c2_egr_unit,
                  c2_gport_ucast_scheduler_fq2[idx],
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
    kbits_sec_max = C2_SCHED_FQ_KBITS_SEC_MAX;
    max_burst = C2_SCHED_FQ_MAX_BURST;
    rv = bcm_cosq_gport_bandwidth_set(c2_egr_unit,
                      c2_gport_ucast_scheduler_fq3[idx],
                      0,
                      0,
                      kbits_sec_max,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set sched failed $rv\n");
        return(rv);
    }
    
    rv = bcm_cosq_control_set(c2_egr_unit,
                  c2_gport_ucast_scheduler_fq3[idx],
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
    kbits_sec_max = C2_CONN_KBITS_SEC_MAX;
    max_burst = C2_CONN_MAX_BURST;

    for (cosq = 0; cosq < 4; cosq++) {
        /* Set rate on the Connector flow */
        rv = bcm_cosq_gport_bandwidth_set(c2_egr_unit,
                          c2_gport_ucast_voq_connector_group0[idx],
                          cosq,
                          0,
                          kbits_sec_max,
                          0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_bandwidth_set connector failed $rv\n");
            return rv;
        }
        
        /* Set maxburst on the Connector flow */
        rv = bcm_cosq_control_set(c2_egr_unit,
                      c2_gport_ucast_voq_connector_group0[idx],
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
    kbits_sec_max = C2_CONN_KBITS_SEC_MAX;
    max_burst = C2_CONN_MAX_BURST;
    for (cosq=0; cosq<4; cosq++) {
        /* Set rate on the Connector flow */
        rv = bcm_cosq_gport_bandwidth_set(c2_egr_unit,
                          c2_gport_ucast_voq_connector_group1[idx],
                          cosq,
                          0,
                          kbits_sec_max,
                          0);
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_gport_bandwidth_set connector failed $rv\n");
            return rv;
        }
        
        /* Set maxburst on the Connector flow */
        rv = bcm_cosq_control_set(c2_egr_unit,
                      c2_gport_ucast_voq_connector_group1[idx],
                      cosq,
                      bcmCosqControlBandwidthBurstMax,
                      max_burst);
        
        if (rv != BCM_E_NONE) {
            printf("bcm_cosq_control_set connector maxburst failed $rv\n");
            return rv;
        }
    }
    
    tm_config2_display_handles(idx);
    
    printf("port(%d) sysport(%d) ucast gport(0x%08x)\n", port, system_port, c2_gport_ucast_queue_group0[idx]);
    printf("cint_tm_petra_config2.c completed with status (%s)\n", bcm_errmsg(rv));
    return rv;
}


int tm_config2_teardown(int port_id, int port_type)
{
    int                           rc = BCM_E_NONE;
    int                           idx = 0;
    bcm_gport_t                   gport_ucast_voq_connector_group;
    bcm_gport_t                   gport_ucast_queue_group;
    bcm_gport_t                   gport_scheduler_fq;
    int                           nbr_queue_groups, nbr_fq_sched;
    bcm_cosq_gport_connection_t   connection;
    int                           cosq = 0;


    printf("\nTeardown\n");

    idx = port_id;

    tm_config2_display_handles(idx);

    /* detach scheduler sub-tree from E2E hierarchy */
    rc = bcm_cosq_gport_detach(c2_egr_unit, c2_e2e_gport[idx], c2_gport_ucast_scheduler[idx], 0);
    if (BCM_FAILURE(rc)) {
        printf("detach for SE(0x%x) failed, Error (%d, 0x%x)\n", c2_gport_ucast_scheduler[idx], rc, rc);
        return(rc);
    }
    printf("  detached schedGport(0x%x)\n", c2_gport_ucast_scheduler[idx]);

    /* dis-associate queue from connectors */
    for (nbr_queue_groups = 0; nbr_queue_groups < C2_NBR_QUEUE_GROUPS; nbr_queue_groups++) {
        switch (nbr_queue_groups) {
            case 0: gport_ucast_queue_group = c2_gport_ucast_queue_group0[idx];
                    gport_ucast_voq_connector_group = c2_gport_ucast_voq_connector_group0[idx]; break;
            default: gport_ucast_queue_group = c2_gport_ucast_queue_group1[idx];
                     gport_ucast_voq_connector_group = c2_gport_ucast_voq_connector_group1[idx]; break;
        }

        connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
        connection.remote_modid = c2_egr_modid;
        connection.voq = gport_ucast_queue_group;
        connection.voq_connector = gport_ucast_voq_connector_group;

        rc = bcm_cosq_gport_connection_set(c2_ingr_unit, &connection);
        if (BCM_FAILURE(rc)) {
            printf("resetting queue2connector association failed, queue(0x%x) Error (%d, 0x%x)\n", gport_ucast_queue_group, rc, rc);
            return(rc);
        }
        printf("  disconnected (Ingress) ucastGport(0x%x) from connGport(0x%x)\n", gport_ucast_queue_group, gport_ucast_voq_connector_group);
    } 

    /* dis-associate connectors from queues */
    for (nbr_queue_groups = 0; nbr_queue_groups < C2_NBR_QUEUE_GROUPS; nbr_queue_groups++) {
        switch (nbr_queue_groups) {
            case 0: gport_ucast_queue_group = c2_gport_ucast_queue_group0[idx];
                    gport_ucast_voq_connector_group = c2_gport_ucast_voq_connector_group0[idx]; break;
            default: gport_ucast_queue_group = c2_gport_ucast_queue_group1[idx];
                     gport_ucast_voq_connector_group = c2_gport_ucast_voq_connector_group1[idx]; break;
        }

        connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
        connection.remote_modid = c2_ingr_modid;
        connection.voq = gport_ucast_queue_group;
        connection.voq_connector = gport_ucast_voq_connector_group;
     
        rc = bcm_cosq_gport_connection_set(c2_egr_unit, &connection);
        if (BCM_FAILURE(rc)) {
            printf("resetting connector2queue association failed, Connector(0x%x) Error (%d, 0x%x)\n", gport_ucast_voq_connector_group, rc, rc);
            return(rc);
        }
        printf("  disconnected (Engress) connGport(0x%x) from ucastGport(0x%x)\n", gport_ucast_voq_connector_group, gport_ucast_queue_group);
    }

    /* delete queues */
    for (nbr_queue_groups = 0; nbr_queue_groups < C2_NBR_QUEUE_GROUPS; nbr_queue_groups++) {
        switch (nbr_queue_groups) {
            case 0: gport_ucast_queue_group = c2_gport_ucast_queue_group0[idx]; break;
            default: gport_ucast_queue_group = c2_gport_ucast_queue_group1[idx]; break;
        }

        rc = bcm_cosq_gport_delete(c2_ingr_unit, gport_ucast_queue_group);
        if (BCM_FAILURE(rc)) {
            printf("delete for QueueGroup(0x%x) failed, Error (%d, 0x%x)\n", gport_ucast_queue_group, rc, rc);
            return(rc);
        }
        printf("  deleted ucastGport(0x%x)\n", gport_ucast_queue_group);
    }

    /* detach connectors */
    for (nbr_queue_groups = 0; nbr_queue_groups < C2_NBR_QUEUE_GROUPS; nbr_queue_groups++) {
        switch (nbr_queue_groups) {
            case 0: gport_ucast_voq_connector_group = c2_gport_ucast_voq_connector_group0[idx]; break;
            default: gport_ucast_voq_connector_group = c2_gport_ucast_voq_connector_group1[idx]; break;
        }

        for (cosq = 0; cosq < 4; cosq++) {
            switch (cosq) {
                case 0: gport_scheduler_fq = c2_gport_ucast_scheduler_fq0[idx]; break;
                case 1: gport_scheduler_fq = c2_gport_ucast_scheduler_fq1[idx]; break;
                case 2: gport_scheduler_fq = c2_gport_ucast_scheduler_fq2[idx]; break;
                default: gport_scheduler_fq = c2_gport_ucast_scheduler_fq3[idx]; break;
            }

            rc = bcm_cosq_gport_detach(c2_egr_unit, gport_scheduler_fq, gport_ucast_voq_connector_group, cosq);
            if (BCM_FAILURE(rc)) {
                printf("detach for Connector:cosq(0x%x:%d) from Sched(0x%x) failed, Error (%d, 0x%x)\n", gport_ucast_voq_connector_group, cosq, gport_scheduler_fq, rc, rc);
                return(rc);
            }
            printf("  detached connGport(0x%x) from schedGport(0x%x)\n", gport_ucast_voq_connector_group, gport_scheduler_fq);
        }
    }

    /* delete connectors */
    for (nbr_queue_groups = 0; nbr_queue_groups < C2_NBR_QUEUE_GROUPS; nbr_queue_groups++) {
        switch (nbr_queue_groups) {
            case 0: gport_ucast_voq_connector_group = c2_gport_ucast_voq_connector_group0[idx]; break;
            default: gport_ucast_voq_connector_group = c2_gport_ucast_voq_connector_group1[idx]; break;
        }

        /* delete connectors */
        rc = bcm_cosq_gport_delete(c2_egr_unit, gport_ucast_voq_connector_group);
        if (BCM_FAILURE(rc)) {
            printf("delete for ConnectorGroup(0x%x) failed, Error (%d, 0x%x)\n", gport_ucast_voq_connector_group, rc, rc);
            return(rc);
        }
        printf("  delete connGport(0x%x)\n", gport_ucast_voq_connector_group);
    }

    /* detach intermediate Scheduler elements (SE) */
    for (nbr_fq_sched = 0; nbr_fq_sched < C2_NBR_FQ_SCHEDULERS; nbr_fq_sched++) {
        switch (nbr_fq_sched) {
            case 0: gport_scheduler_fq = c2_gport_ucast_scheduler_fq0[idx]; break;
            case 1: gport_scheduler_fq = c2_gport_ucast_scheduler_fq1[idx]; break;
            case 2: gport_scheduler_fq = c2_gport_ucast_scheduler_fq2[idx]; break;
            default: gport_scheduler_fq = c2_gport_ucast_scheduler_fq3[idx]; break;
        }

        rc = bcm_cosq_gport_detach(c2_egr_unit, c2_gport_ucast_scheduler[idx], gport_scheduler_fq, 0);
        if (BCM_FAILURE(rc)) {
            printf("detach for SE(0x%x) failed, Error (%d, 0x%x)\n", gport_scheduler_fq, rc, rc);
            return(rc);
        }
        printf("  detached schedGport(0x%x)\n", gport_scheduler_fq);
    }

    /* delete intermediate Scheduler elements (SE) */
    for (nbr_fq_sched = 0; nbr_fq_sched < C2_NBR_FQ_SCHEDULERS; nbr_fq_sched++) {
        switch (nbr_fq_sched) {
            case 0: gport_scheduler_fq = c2_gport_ucast_scheduler_fq0[idx]; break;
            case 1: gport_scheduler_fq = c2_gport_ucast_scheduler_fq1[idx]; break;
            case 2: gport_scheduler_fq = c2_gport_ucast_scheduler_fq2[idx]; break;
            default: gport_scheduler_fq = c2_gport_ucast_scheduler_fq3[idx]; break;
        }

        rc = bcm_cosq_gport_delete(c2_egr_unit, gport_scheduler_fq);
        if (BCM_FAILURE(rc)) {
            printf("delete for SE(0x%x) failed, Error (%d, 0x%x)\n", gport_scheduler_fq, rc, rc);
            return(rc);
        }
        printf("  delete schedGport(0x%x)\n", gport_scheduler_fq);
    }

    /* delete scheduler  element at top of sub-tree */
    rc = bcm_cosq_gport_delete(c2_egr_unit, c2_gport_ucast_scheduler[idx]);
    if (BCM_FAILURE(rc)) {
        printf("delete for SE(0x%x) failed, Error (%d, 0x%x)\n", c2_gport_ucast_scheduler[idx], rc, rc);
        return(rc);
    }
    printf("  delete schedGport(0x%x)\n", c2_gport_ucast_scheduler[idx]);

    printf("cint_tm_petra_config2.c (teardown) completed with status (%s)\n", bcm_errmsg(rc));

    c2_use_same_handles = 1;

    return(rc);
}

int
tm_config2_verify(int port_id, int port_type, int res_type)
{
    int                           rc = BCM_E_NONE;
    int                           idx = 0;
    bcm_gport_t                   physical_port;
    int                           num_cos_levels;
    uint32                        flags;
    int                           mode, weight, exp_mode;
    int                           cosq, kbits_sec_max, max_burst, slow_rate;
    bcm_gport_t                   gport_scheduler_fq;
    bcm_gport_t                   gport_ucast_voq_connector_group;
    bcm_gport_t                   gport_ucast_queue_group;
    int                           nbr_queue_groups, nbr_fq_sched;
    bcm_cosq_gport_connection_t   connection;


    printf("\nVerify Setup\n");

    idx = port_id;

    if ( (res_type != C2_RESOURCE_TYPE_LEGACY) && (res_type != C2_RESOURCE_TYPE_FIXED) ) {
        printf("invaid res_type(%d)\n", res_type);
        return(BCM_E_PARAM);
    }

    if ( (res_type == C2_RESOURCE_TYPE_FIXED) && (port_type != C2_FRONT_PANEL_PORT_TYPE) ) {
        printf("For fixed resource type Port type has to be Front Panel, invalid port type (%d)\n", port_type);
        return(BCM_E_PARAM);
    }

    if (res_type == C2_RESOURCE_TYPE_FIXED) {
        rc = tm_config2_form_handles(idx, port_type);
        if (rc != BCM_E_NONE) {
            printf("error in forming handles\n");
            return(rc);
        }
        tm_config2_display_handles(idx);
    }

    /* Get rate of the E2E Interface */
    rc = bcm_cosq_gport_bandwidth_get(c2_egr_unit,c2_e2e_parent_gport[idx], 0,0,&kbits_sec_max,0);
    if (rc != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_get if failed $rc\n");
        return rc;
    }
    /* actual hardware value set returned */
    if ((kbits_sec_max > C2_E2E_INTERFACE_KBITS_SEC_MAX + C2_E2E_INTERFACE_KBITS_SEC_MAX_MARGIN) || 
        (kbits_sec_max < C2_E2E_INTERFACE_KBITS_SEC_MAX - C2_E2E_INTERFACE_KBITS_SEC_MAX_MARGIN)) {
        printf("E2E interface(0x%08x) bandwidth_get rate(%d) expected rate(1000000)\n", c2_e2e_parent_gport[idx], kbits_sec_max);
    }

    /* Get rate on the E2E port */
    rc = bcm_cosq_gport_bandwidth_get(c2_egr_unit,c2_e2e_gport[idx],0, 0,kbits_sec_max, 0);
    if (rc != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set failed $rc\n");
        return rc;
    }
   if ((kbits_sec_max > C2_E2E_PORT_KBITS_SEC_MAX + C2_E2E_PORT_KBITS_SEC_MAX_MARGIN) || 
        (kbits_sec_max < C2_E2E_PORT_KBITS_SEC_MAX - C2_E2E_PORT_KBITS_SEC_MAX_MARGIN)) {
        printf("E2E interface(0x%08x) bandwidth_get rate(%d) expected rate(1000000)\n", c2_e2e_parent_gport[idx], kbits_sec_max);
    }

    if (kbits_sec_max != C2_E2E_PORT_KBITS_SEC_MAX) {
        printf("E2E interface(0x%08x) bandwidth_get rate(%d) expected rate(500000)\n", c2_e2e_gport[idx], kbits_sec_max);
    }    
    
    /* retrieve e2e port scheduling element */
    rc = bcm_cosq_gport_get(c2_egr_unit, c2_e2e_gport[idx], &physical_port, &num_cos_levels, &flags);
    if (rc != BCM_E_NONE) {
        printf("bcm_cosq_gport_get on e2e Sched (0x%x) failed, error (%d)\n", c2_e2e_gport[idx], rc);  
        return(rc);
    }
    if (flags != C2_DEFAULT_E2E_SCHED_FLAGS) {
        printf("gport get mismatch on E2E Sched (0x%x) expected flags(0x%x) retreived flags (0x%x)\n", c2_e2e_gport[idx], C2_DEFAULT_E2E_SCHED_FLAGS, flags);
    }
    
    /* retreive type of scheduling element */
    rc = bcm_cosq_gport_get(c2_egr_unit, c2_gport_ucast_scheduler[idx],
                                           &physical_port, &num_cos_levels, &flags);
    if (rc != BCM_E_NONE) {
        printf("bcm_cosq_gport_get on Sched (0x%x) failed, error (%d)\n", c2_gport_ucast_scheduler[idx], rc);
        return(rc);
    }
    if (flags != C2_SCHED_FLAGS) {
        printf("gport get mismatch on Sched (0x%x) expected flags(0x%x) retreived flags (0x%x)\n", c2_gport_ucast_scheduler[idx], C2_SCHED_FLAGS, flags);
    }

    /* retreive SE scheduling discipline */
    rc = bcm_cosq_gport_sched_get(c2_egr_unit, c2_gport_ucast_scheduler[idx], 0, &mode, &weight);
    if (rc != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_get on Sched (0x%x) failed, error (%d)\n", c2_gport_ucast_scheduler[idx], rc);
        return(rc);
    }
    if (mode != C2_SCHED_MODE) {
        printf("sched mode mismatch on Sched (0x%x) expected(BCM_COSQ_SP%d) retreived (BCM_COSQ_SP%d)\n", c2_gport_ucast_scheduler[idx], (C2_SCHED_MODE - BCM_COSQ_SP0), (mode - BCM_COSQ_SP0));
    }
    if (weight != 0) {
        printf("sched weight mismatch on Sched (0x%x) expected(%d) retreived (%d)\n", c2_gport_ucast_scheduler[idx], 0, weight);
    }

    /* retreive rate of Scheduler */
    rc = bcm_cosq_gport_bandwidth_get(c2_egr_unit, c2_gport_ucast_scheduler[idx], 0,
                                                        NULL, &kbits_sec_max, NULL);
    if (rc != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_get on Sched (0x%x) failed, error (%d)\n", c2_gport_ucast_scheduler[idx], rc);
        return(rc);
    }
    if ( (kbits_sec_max < (C2_SCHED_KBITS_SEC_MAX - C2_SCHED_KBITS_SEC_MAX_MARGIN)) ||
         (kbits_sec_max > (C2_SCHED_KBITS_SEC_MAX + C2_SCHED_KBITS_SEC_MAX_MARGIN)) ) {
        printf("bandwidth_get mismatch on Sched (0x%x) expected(%d) retreived (%d)\n", c2_gport_ucast_scheduler[idx], C2_SCHED_KBITS_SEC_MAX, kbits_sec_max);
    }

    /* retreive scheduler burst size */
    rc = bcm_cosq_control_get(c2_egr_unit, c2_gport_ucast_scheduler[idx], 0,
                                  bcmCosqControlBandwidthBurstMax, &max_burst);
    if (rc != BCM_E_NONE) {
        printf("bcm_cosq_control_get-BandwidthBurstMax on Sched (0x%x) failed, error (%d)\n", c2_gport_ucast_scheduler[idx], rc);
        return(rc);
    }
    if ( (max_burst < (C2_SCHED_MAX_BURST - C2_SCHED_MAX_BURST_MARGIN)) ||
          (max_burst > (C2_SCHED_MAX_BURST + C2_SCHED_MAX_BURST_MARGIN)) ) {
        printf("control_get-BandwidthBurstMax mismatch on Sched (0x%x) expected(%d) retreived (%d)\n", c2_gport_ucast_scheduler[idx], C2_SCHED_MAX_BURST, max_burst);
    }


    /* retreive intermediate Scheduler elements (SE) */
    for (nbr_fq_sched = 0; nbr_fq_sched < C2_NBR_FQ_SCHEDULERS; nbr_fq_sched++) {
        switch (nbr_fq_sched) {
            case 0: gport_scheduler_fq = c2_gport_ucast_scheduler_fq0[idx];
                    exp_mode = BCM_COSQ_SP0; break;
            case 1: gport_scheduler_fq = c2_gport_ucast_scheduler_fq1[idx];
                    exp_mode = BCM_COSQ_SP1; break;
            case 2: gport_scheduler_fq = c2_gport_ucast_scheduler_fq2[idx];
                    exp_mode = BCM_COSQ_SP2; break;
            default: gport_scheduler_fq = c2_gport_ucast_scheduler_fq3[idx];
                     exp_mode = BCM_COSQ_SP3; break;
        }

        /* retreive type of scheduling element */
        rc = bcm_cosq_gport_get(c2_egr_unit, gport_scheduler_fq,
                                           &physical_port, &num_cos_levels, &flags);
        if (rc != BCM_E_NONE) {
            printf("bcm_cosq_gport_get on Sched (0x%x) failed, error (%d)\n", gport_scheduler_fq, rc);
            return(rc);
        }
        if (flags != C2_SCHED_FQ_FLAGS) {
            printf("gport get mismatch on Sched (0x%x) expected flags(0x%x) retreived flags (0x%x)\n", gport_scheduler_fq, C2_SCHED_FQ_FLAGS, flags);
        }

        /* retreive SE scheduling discipline */
        rc = bcm_cosq_gport_sched_get(c2_egr_unit, gport_scheduler_fq, 0, &mode, &weight);
        if (rc != BCM_E_NONE) {
            printf("bcm_cosq_gport_sched_get on Sched (0x%x) failed, error (%d)\n", gport_scheduler_fq, rc);
            return(rc);
        }
        if (mode != exp_mode) {
            printf("sched mode mismatch on Sched (0x%x) expected(BCM_COSQ_SP%d) retreived (BCM_COSQ_SP%d)\n", gport_scheduler_fq, (exp_mode - BCM_COSQ_SP0), (mode - BCM_COSQ_SP0));
        }
        if (weight != 0) {
            printf("sched weight mismatch on Sched (0x%x) expected(%d) retreived (%d)\n", gport_scheduler_fq, 0, weight);
        }

        /* retreive rate of Scheduler */
        rc = bcm_cosq_gport_bandwidth_get(c2_egr_unit, gport_scheduler_fq, 0,
                                                        NULL, &kbits_sec_max, NULL);
        if (rc != BCM_E_NONE) {
            printf("bcm_cosq_gport_bandwidth_get on Sched (0x%x) failed, error (%d)\n", gport_scheduler_fq, rc);
            return(rc);
        }
        if ( (kbits_sec_max < (C2_SCHED_FQ_KBITS_SEC_MAX - C2_SCHED_FQ_KBITS_SEC_MAX_MARGIN)) ||
             (kbits_sec_max > (C2_SCHED_FQ_KBITS_SEC_MAX + C2_SCHED_FQ_KBITS_SEC_MAX_MARGIN)) ) {
            printf("bandwidth_get mismatch on Sched (0x%x) expected(%d) retreived (%d)\n", gport_scheduler_fq, C2_SCHED_FQ_KBITS_SEC_MAX, kbits_sec_max);
        }

        /* retreive scheduler burst size */
        rc = bcm_cosq_control_get(c2_egr_unit, gport_scheduler_fq, 0,
                                  bcmCosqControlBandwidthBurstMax, &max_burst);
        if (rc != BCM_E_NONE) {
            printf("bcm_cosq_control_get-BandwidthBurstMax on Sched (0x%x) failed, error (%d)\n", gport_scheduler_fq, rc);
            return(rc);
        }
        if ( (max_burst < (C2_SCHED_FQ_MAX_BURST - C2_SCHED_FQ_MAX_BURST_MARGIN)) ||
             (max_burst > (C2_SCHED_FQ_MAX_BURST + C2_SCHED_FQ_MAX_BURST_MARGIN)) ) {
            printf("control_get-BandwidthBurstMax mismatch on Sched (0x%x) expected(%d) retreived (%d)\n", gport_scheduler_fq, C2_SCHED_FQ_MAX_BURST, max_burst);
        }
    }

    /* retreive connectors and queue groups */
    for (nbr_queue_groups = 0; nbr_queue_groups < C2_NBR_QUEUE_GROUPS; nbr_queue_groups++) {
        switch (nbr_queue_groups) {
            case 0: gport_ucast_voq_connector_group = c2_gport_ucast_voq_connector_group0[idx];
                    gport_ucast_queue_group = c2_gport_ucast_queue_group0[idx]; break;
            default: gport_ucast_voq_connector_group = c2_gport_ucast_voq_connector_group1[idx];
                     gport_ucast_queue_group = c2_gport_ucast_queue_group1[idx]; break;
        }

        rc = bcm_cosq_gport_get(c2_egr_unit, gport_ucast_queue_group,
                    &physical_port, &num_cos_levels, &flags);

        if (rc != BCM_E_NONE) {
            printf("bcm_cosq_gport_get on ucast queue group(0x%x) failed, error (%d)\n", gport_ucast_queue_group, rc);
            return(rc);
        }
        
        if (gport_ucast_queue_group == c2_gport_ucast_queue_group0[idx]) {
            if (flags != C2_QUEUE_FLAGS) {
                printf("gport get mismatch on ucast queue (0x%x) failed expected flags(0x%x) retrieved flags(0x%x)\n",
                       gport_ucast_queue_group, C2_QUEUE_FLAGS, flags);
            }
        } else {
            if (flags != (C2_QUEUE_FLAGS | BCM_COSQ_GPORT_TM_FLOW_ID)) {
                printf("gport get mismatch on ucast queue (0x%x) failed expected flags(0x%x) retrieved flags(0x%x)\n",
                       gport_ucast_queue_group, C2_QUEUE_FLAGS, flags);
            }
        }
        if (num_cos_levels != C2_NUM_COS) {
            printf("gport get mismatch on ucast queue (0x%x) expected numCos(%d) retreived numCos(%d)\n",
                   gport_ucast_queue_group, C2_NUM_COS, num_cos_levels);
        }

        /* retreive type of connectors */
        rc = bcm_cosq_gport_get(c2_egr_unit, gport_ucast_voq_connector_group,
                                           &physical_port, &num_cos_levels, &flags);
        if (rc != BCM_E_NONE) {
            printf("bcm_cosq_gport_get on Connector (0x%x) failed, error (%d)\n", gport_ucast_voq_connector_group, rc);
            return(rc);
        }
        if (flags != C2_CONN_FLAGS) {
            printf("gport get mismatch on Connector (0x%x) expected flags(0x%x) retreived flags (0x%x\n", gport_ucast_voq_connector_group, C2_CONN_FLAGS, flags);
        }
        if (num_cos_levels != C2_NUM_COS) {
            printf("gport get mismatch on Connector (0x%x) expected numCos(%d) retreived numCos(%d)\n", gport_ucast_voq_connector_group, C2_NUM_COS, num_cos_levels);
        }

        /* retreive rate of Connectors and corresponding burst size */
        for (cosq = 0; cosq < 4; cosq++) {
            /* retreive SE scheduling discipline */
            rc = bcm_cosq_gport_sched_get(c2_egr_unit, gport_ucast_voq_connector_group, cosq, &mode, &weight);
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
            rc = bcm_cosq_gport_bandwidth_get(c2_egr_unit, gport_ucast_voq_connector_group, cosq,
                                                                    NULL, &kbits_sec_max, NULL);
            if (rc != BCM_E_NONE) {
                printf("bcm_cosq_gport_bandwidth_get on connector:cosq (0x%x:%d) failed, error (%d)\n", gport_ucast_voq_connector_group, cosq, rc);
                return(rc);
            }

            if ( (kbits_sec_max < (C2_CONN_KBITS_SEC_MAX - C2_CONN_KBITS_SEC_MAX_MARGIN)) ||
                 (kbits_sec_max > (C2_CONN_KBITS_SEC_MAX + C2_CONN_KBITS_SEC_MAX_MARGIN)) ) {
                printf("bandwidth_get mismatch on connector:cosq (0x%x:%d) expected(%d) retreived (%d)\n", gport_ucast_voq_connector_group, cosq, C2_CONN_KBITS_SEC_MAX, kbits_sec_max);
            }

            rc = bcm_cosq_control_get(c2_egr_unit, gport_ucast_voq_connector_group, cosq,
                                  bcmCosqControlBandwidthBurstMax, &max_burst);
            if (rc != BCM_E_NONE) {
                printf("bcm_cosq_control_get-BandwidthBurstMax on Connector:cosq (0x%x:%d) failed, error (%d)\n", gport_ucast_voq_connector_group, cosq, rc);
                return(rc);
            }
            if ( (max_burst < (C2_CONN_MAX_BURST - C2_CONN_MAX_BURST_MARGIN)) ||
                 (max_burst > (C2_CONN_MAX_BURST + C2_CONN_MAX_BURST_MARGIN)) ) {
                printf("control_get-BandwidthBurstMax mismatch on Connector:cosq(0x%x:%d) expected(%d) retreived (%d)\n", gport_ucast_voq_connector_group, cosq, C2_SCHED_MAX_BURST, max_burst);

            }

            /* retreive VOQ Connector slow state */
            rc = bcm_cosq_control_get(c2_egr_unit, gport_ucast_voq_connector_group, cosq,
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
        rc = bcm_cosq_gport_connection_get(c2_egr_unit, &connection);
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
        rc = bcm_cosq_gport_connection_get(c2_ingr_unit, &connection);
        if (rc != BCM_E_NONE) {
            printf("bcm_cosq_gport_connection_get on Ingress, VoQ (0x%x) failed, error (%d)\n", gport_ucast_queue_group, rc);
            return(rc);
        }
        if (connection.voq_connector != gport_ucast_voq_connector_group) {
            printf("gport_connection_get on Ingress, VoQ (0x%x) connector association mismatch, Expected (0x%x), retreived (0x%x)\n", gport_ucast_queue_group, gport_ucast_voq_connector_group, connection.voq_connector);
        }
    }

    printf("cint_tm_petra_config2.c (verify) completed with status (%s)\n", bcm_errmsg(rc));

    return(rc);
}

/* Traffic passes on port 0/1 after this step */
/* tm_config2_setup(0, 0 /*C2_FRONT_PANEL_PORT_TYPE*/, 0 /*C2_RESOURCE_TYPE_LEGACY*/); */
/* tm_config2_setup(1, 0 /*C2_FRONT_PANEL_PORT_TYPE*/, 0 /*C2_RESOURCE_TYPE_LEGACY*/); */

/* Traffic stops on port 0/1 after this step */
/* tm_config2_teardown(0, 0 /*C2_FRONT_PANEL_PORT_TYPE*/); */
/* tm_config2_teardown(1, 0 /*C2_FRONT_PANEL_PORT_TYPE*/); */

/* Traffic passes on port 0/1 after this step */
/* tm_config2_setup(0, 0 /*C2_FRONT_PANEL_PORT_TYPE*/, 0 /*C2_RESOURCE_TYPE_LEGACY*/); */
/* tm_config2_setup(1, 0 /*C2_FRONT_PANEL_PORT_TYPE*/, 0 /*C2_RESOURCE_TYPE_LEGACY*/); */

/* verify setup */
/* tm_config2_verify(0, 0 /*C2_FRONT_PANEL_PORT_TYPE*/, 0 /*C2_RESOURCE_TYPE_LEGACY*/); */
/* tm_config2_verify(1, 0 /*C2_FRONT_PANEL_PORT_TYPE*/, 0 /*C2_RESOURCE_TYPE_LEGACY*/); */

/* rcy port setup */
/* tm_config2_setup(10, 1 /*C2_RCY_PORT_TYPE*/, 0 /*C2_RESOURCE_TYPE_LEGACY*/); */
/* tm_config2_verify(10, 1 /*C2_RCY_PORT_TYPE*/, 0 /*C2_RESOURCE_TYPE_LEGACY*/); */
/* tm_config2_teardown(10, 1 /*C2_RCY_PORT_TYPE*/); */

