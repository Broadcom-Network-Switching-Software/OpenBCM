/*
 * 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        cint_tm_fap_config_large_scheduling_scheme.c
 * Purpose:     Example of E2E scheduler configuration where resource management is by SDK
 *
 * The example shows how to configure:
 *     - Egress port scheduler with 2 HR's.
 *     - 3 levels of Composite CL schedulers.
 *     - CIR and EIR shaping.
 *     - Independent WFQ
 *
 * Usage:
 *     1. To setup the full TM hierarchy, call the following function from cint:
 *        cint_tm_setup_full();
 *
 *     2. To change rate of an egress port:
 *        cint_change_port_rate(unit,port_num,rate);
 *
 *     3. To change CIR/EIR rate of a SE:
 *        cint_change_se_rate(unit,level,index,cir,eir);
 *
 *     4. To change weight of SE:
 *        cint_change_se_weight(unit,level,index,eir_weight);
 *
 *     5. To change rate of a VOQ:
 *        cint_change_vc_rate(unit,voq_bundle,cos,rate);
 *
 *     6. To change weight of VOQ:
 *        cint_change_vc_weight(unit,voq_bundle,cos,eir_weight);
 */



 /* Environment
 *    - Default Egress queue mapping disabled.
 *    - Default Egress Port Scheduler hierarchy disabled.
 *
+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
|                                                                                                                                                                              |
|                                                                   +              +                                                                                           |
|                                                                   |              |                                                                                           |
|                                                                  10G            10G                                                                                          |
|                                                                   +              +                                                                                           |
|                                                                +--v--+        +--v--+                                                                                        |
|                                                                | HR1 |        | HR4 |                                                                                        |
|                                                                +-----+        +--+--+                                                                                        |
|                                                         CIR - 1.6G|              |  CIR - 1.6G                                                                               |
|                                                         EIR - 8.4G|              |  EIR - 8.4G                                                                               |
|                                                                   |              |                                                                                           |
|                                        +-----------------------------------------+--------------------------------------+                                                    |
|                                        |                          |                                                     |                                                    |
|                         +-----------------------------------------+--------------------------------------+              |                                                    |
|                         |              |                                                                 |              |                                                    |
|                         |              |                                                                 |              |                                                    |
|                       +-v--------------v-+                                                             +-v--------------v-+                                                  |
|                       |       CL         | ...x18                                                      |        CL        |   ...x18                    (x18)                |
|                       +--+------------+--+                                                             +--+------------+--+                                                  |
|                          |            |                                                                   |            |                                                     |
|                   +------+            +----------+                                                 +------+            +----------+                                          |
|        CIR + 65M  |                              | CIR + 65M                            CIR + 65M  |                              | CIR + 65M                                |
|        EIR + 940M |                              | EIR + 940M                           EIR + 940M |                              | EIR + 940M                               |
|                   |                              |                                                 |                              |                                          |
|       +-----------v----------+               +---v------------------+                    +---------v------------+             +---v------------------+                       |
|       |         CL           | ...x18        |         CL           | ...x18             |         CL           | ...x18      |         CL           |    ...x18             |
|       +-----+----------+-----+               +-----+----------+-----+                    +-----+----------+-----+             +-----+----------+-----+                       |
|             |          |                           |          |                                |          |                         |          |                             |
|             |          |                           |          |                                |          |                         |          |                             |
|             |          |                           |          |                                |          |                         |          |                             |
|             |          |                           |          |                                |          |                         |          |                             |
|  CIR + 10M  |          |                CIR + 10M  |          |                     CIR + 10M  |          |              CIR + 10M  |          |                             |
|  EIR + 990M |          |                EIR + 990M |          |                     EIR + 990M |          |              EIR + 990M |          |                             |
|             |          |                           |          |                                |          |                         |          |                             |
|          +--v--+    +--v--+                     +--v--+    +--v--+                          +--v--+    +--v--+                   +--v--+    +--v--+                          |
|          | CL  |    | CL  |                     | CL  |    | CL  |                          | CL  |    | CL  |                   | CL  |    | CL  |                          |
|          +--+--+    +--+--+                     +--+--+    +--+--+                          +--+--+    +--+--+                   +--+--+    +--+--+                          |
|             |          |                           |          |                                |          |                         |          |                             |
|             |          |   ...x6                   |          |   ...x6                        |          |   ...x6                 |          |   ...x6                     |
|             |          |                           |          |                                |          |                         |          |                             |
|          |  v  |    |  v  |                     |  v  |    |  v  |                          |  v  |    |  v  |                   |  v  |    |  v  |                          |
|          | NO  |    | NO  |                     | NO  |    | NO  |                          | NO  |    | NO  |                   | NO  |    | NO  |                          |
|          |LIMIT|    |LIMIT|                     |LIMIT|    |LIMIT|                          |LIMIT|    |LIMIT|                   |LIMIT|    |LIMIT|                          |
|          |     |    |     |                     |     |    |     |                          |     |    |     |                   |     |    |     |                          |
|          +-----+    +-----+                     +-----+    +-----+                          +-----+    +-----+                   +-----+    +-----+                          |
|                                                                                                                                                                              |
|                                                                                                                                                                              |
|                                                                                                                                                                              |
+------------------------------------------------------------------------------------------------------------------------------------------------------------------------------+
 *  ALL VOQs are with a bundle of 4
 *  All CL are in mode - 2SP, last WFQ!
 */

/**** Define global variables to make them accessible from CINT *********/

int          PORT_NUM_BASE = 13;  /* CGE-1*/
int          NUMBER_OF_PORTS = 1;
int          HR_PER_PORTS = 8; /*Assume port priority to be 8*/
int          L4_NUM_ELEMENTS = 18;
int          L3_NUM_ELEMENTS = 18;
int          L2_NUM_ELEMENTS = 6;
int          EIR_SP_MODE[3] = {BCM_COSQ_SP1, BCM_COSQ_SP1, BCM_COSQ_SP3};
int          L4_EIR_WEIGHT[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
int          L3_EIR_WEIGHT[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
                                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
int          L2_EIR_WEIGHT[] = {1, 1, 1, 1, 1, 1};

int          UNIT_1K = 1000;
int          CREDIT_ADJUST = -13;

/* Units for all below rates and burst sizes are in Kbps */
int PORT_RATE    = 100 * UNIT_1K * UNIT_1K;

int L4_CIR_RATE  = 1660 * UNIT_1K;
int L4_EIR_RATE  = 8340 * UNIT_1K;
int L4_CIR_BURST = 4000;
int L4_EIR_BURST = 4000;

int L3_CIR_RATE  = 65 * UNIT_1K;
int L3_EIR_RATE  = 935 * UNIT_1K;
int L3_CIR_BURST = 4000;
int L3_EIR_BURST = 4000;

int L2_CIR_RATE  = 10 * UNIT_1K;
int L2_EIR_RATE  = 990 * UNIT_1K;
int L2_CIR_BURST = 4000;
int L2_EIR_BURST = 4000;

/** Practically these VOQ rates are not used.
 * In the traffic test are passed streams with TCs bigger than 2!*/
int VOQ_RATE[2]       = {1900, 3900};
int VOQ_COS_SP[4]     = {BCM_COSQ_SP0, BCM_COSQ_SP1, BCM_COSQ_SP1, BCM_COSQ_SP1};
int VOQ_COS_WEIGHT[4] = {0, 1, 3, 9};
int VOQ_MAX_BURST     = 3000;
int is_dnx;



/* CINT does not support multi-dimensional array.*/

bcm_gport_t  hr_info_gport_arr[(NUMBER_OF_PORTS*HR_PER_PORTS)];
bcm_gport_t  level4_gport_arr[(NUMBER_OF_PORTS*L4_NUM_ELEMENTS)];
bcm_gport_t  level3_gport_arr[(NUMBER_OF_PORTS*L4_NUM_ELEMENTS*L3_NUM_ELEMENTS)];
bcm_gport_t  level2_gport_arr[(NUMBER_OF_PORTS*L4_NUM_ELEMENTS*L3_NUM_ELEMENTS*L2_NUM_ELEMENTS)];
bcm_gport_t  voq_gport_arr[(NUMBER_OF_PORTS*L4_NUM_ELEMENTS*L3_NUM_ELEMENTS*L2_NUM_ELEMENTS)];



/*
    Configure a Composite SE.
    [1] Create a gport. The driver creates two gpors: one for CIR and another for EIR.
    [2] Configure CIR and CBS on the CIR flow (gport).
    [3] Configure EIR and EBS on the EIR flow (gport).
    [4] Configure SP scheduling mode for cir
    [5] Configure WFQ scheduling mode and weigth for cir
*/

bcm_gport_t cint_composite_sched_config (int unit,
                                         int port,
                                         int flags,
                                         int cir_rate,
                                         int eir_rate,
                                         int cir_burst,
                                         int eir_burst,
                                         int eir_mode,
                                         int eir_weight)
{
    int rv;
    bcm_gport_t cir_gport, eir_gport, dest_gport;
    bcm_cosq_gport_info_t gport_info;

    BCM_GPORT_LOCAL_SET(dest_gport, port);

    if ((rv = bcm_cosq_gport_add(unit, dest_gport, 1, flags, &cir_gport)) != BCM_E_NONE) {
        printf("bcm_cosq_gport_add() failed $rv\n");
        return NULL;
    }
    /*
        The gport of the EIR flow is created as a sub-flow of CIR flow by driver.
        The user just retrieves it.
     */
    gport_info.cosq=0;
    gport_info.in_gport = cir_gport;
    bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeCompositeFlow2,&gport_info);
    eir_gport = gport_info.out_gport;

    if (cir_rate > 0) {
        if ((rv = bcm_cosq_gport_bandwidth_set(unit, cir_gport, 0, 0,
                                           cir_rate, 0)) != BCM_E_NONE) {
            printf("bcm_cosq_gport_bandwidth_set() failed $rv: cir_gport=0x%x\n",
                                       cir_gport);
            return NULL;
        }

        if ((rv = bcm_cosq_control_set(unit, cir_gport, 0,
                     bcmCosqControlBandwidthBurstMax, cir_burst)) != BCM_E_NONE) {
            printf("bcm_cosq_control_set() for CIR failed $rv: cir_gport=0x%x\n",
                                       cir_gport);
            return NULL;
        }
    }

    if (eir_rate > 0) {
        if ((rv = bcm_cosq_gport_bandwidth_set(unit, eir_gport, 0, 0,
                                           eir_rate, 0)) != BCM_E_NONE) {
            printf("bcm_cosq_gport_bandwidth_set() for EIR failed $rv: eir_gport=0x%x\n", eir_gport);
            return NULL;
        }

        if ((rv = bcm_cosq_control_set(unit, eir_gport, 0,
                  bcmCosqControlBandwidthBurstMax, eir_burst)) != BCM_E_NONE) {
            printf("bcm_cosq_control_set() for EIR failed $rv: eir_gport=0x%x\n",
                              eir_gport);
            return NULL;
        }
    }

    if ((rv = bcm_cosq_gport_sched_set(unit, cir_gport, 0,
                                       BCM_COSQ_SP0, 0)) != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set() for CIR failed $rv: cir_gport=0x%x\n",
                                       cir_gport);
        return NULL;
    }

    if ((rv = bcm_cosq_gport_sched_set(unit, eir_gport, 0,
                                       eir_mode, eir_weight)) != BCM_E_NONE) {
        printf("bcm_cosq_gport_sched_set() for EIR failed $rv: eir_gport=0x%x\n",
                                       eir_gport);
        return NULL;
    }

    return cir_gport;
}


int get_nif_port_bmp(int unit,
                     int port_base,
                     bcm_pbmp_t* port_bmp,
                     int* num_of_ports)
{
    bcm_port_config_t       port_config;
    bcm_pbmp_t              port_set;
    int                     port,port_index,num_of_ports_get;
    bcm_error_t             rv = BCM_E_NONE;
    BCM_PBMP_CLEAR(port_bmp);
    rv = bcm_port_config_get(unit, &port_config);

    BCM_PBMP_OR(port_bmp,port_config.sfi); /* fabric_links */
    BCM_PBMP_OR(port_bmp,port_config.cpu); /* CPU */
    BCM_PBMP_NEGATE(port_bmp, port_bmp);
    BCM_PBMP_AND(port_bmp,port_config.all); /* All - fabric_links - CPU = Ports in system (no CPU)*/
    BCM_PBMP_COUNT(port_bmp,num_of_ports_get);
    if ((*num_of_ports <= 0) || (*num_of_ports >= num_of_ports_get))
    {
        *num_of_ports=num_of_ports_get;
    }
    else
    {
        port_base=(port_base>0)?num_of_ports_get:0;
        port_index=0;
        BCM_PBMP_CLEAR(port_set);
        BCM_PBMP_ITER(port_bmp,port) {
            if (port<port_base)
            {
                continue;
            }
            if(port_index==*num_of_ports)
            {
                break;
            }
            BCM_PBMP_PORT_ADD(&port_set, port);
            port_index++;
        }
        *port_bmp=port_set;
    }
    return BCM_E_NONE;
}

/*
    Main function to configure the whole TM hierarchy.
*/
/**
 * Note - JR2 required setting credit request profiles before creating VOQ.
 * So before running cint_tm_fap_config_large_scheduling_scheme.c cint, make sure to set the profiles:
 * cint src/examples/dnx/ingress_tm/cint_credit_request_profile.c
 * cint_credit_request_profile_backward_compatibilty_set(0, 4096);
 *
 * cint_tm_fap_config_large_scheduling_scheme.c expects that all regions for the schedulers allocated are with nof_remote_cores=1
 * This can be set with the following soc property:
 * dtm_flow_nof_remote_cores_region_X=1
 */

int cint_tm_setup(int unit,
                  int port_base,
                  int num_ports,
                  int num_l4_se,
                  int num_l3_se,
                  int num_l2_se)
{
    bcm_module_t my_modid =0;
    int          voq_base = 32;

    int CREDIT_OVER_SUBSCRIBE = 1; /* percent*/

    double dbl;
    int rv=BCM_E_NONE;
    int se_index;
    bcm_gport_t parent_gport, cir_gport=0, eir_gport=0, gsrc_port=0;
    bcm_gport_t vc_group=0, gloc_port=0;
    bcm_gport_t system_port_gport=0, portmod_gport=0, flow_voq_gport=0;
    bcm_gport_t gucast_queue_group=0, gvoq_connector=0, gvoq_base=0,ge2e=0;
    bcm_cosq_voq_connector_gport_t voq_connector;
    int core = 0, connector_id;
    bcm_cosq_gport_info_t       gport_info;
    bcm_cosq_gport_connection_t connection;
    int remote_credit_size, is_dnx;
    uint32 port_flags;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;

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
        int credit_worth = *(dnxc_data_get(unit, "iqs", "credit", "worth", NULL));
        int legacy_credit_worth = 1024;
        int multiplier = (credit_worth / legacy_credit_worth);
        L4_CIR_BURST *= multiplier;
        L4_EIR_BURST *= multiplier;
        L3_CIR_BURST *= multiplier;
        L3_EIR_BURST *= multiplier;
        L2_CIR_BURST *= multiplier;
        L2_EIR_BURST *= multiplier;
        VOQ_MAX_BURST *= multiplier;
    }



    int port,port_index, prio, l4_index, l3_index,voq_index, cos,se_elem, flags, flags2;
    unsigned int uflags;
    bcm_cosq_gport_info_t gport_info_porttc, gport_info_hr;
    bcm_pbmp_t port_set;
    /*int CARD_NUM_OF_PORTS;*/
    /*if (port_base != -1) PORT_NUM_BASE   = port_base;*/
    /*if (num_ports != -1) NUMBER_OF_PORTS = num_ports;*/
    if (is_dnx) {
        /*get_nif_port_bmp practically will always return 13 for port_set and 1 for num_ports*/
        BCM_PBMP_CLEAR(port_set);
        BCM_PBMP_PORT_ADD(&port_set, port_base);
        num_ports = 1;
    } else {
        get_nif_port_bmp(unit,port_base,&port_set,&num_ports);
    }

    NUMBER_OF_PORTS = num_ports;
    if (num_l4_se != -1) L4_NUM_ELEMENTS = num_l4_se;
    if (num_l3_se != -1) L3_NUM_ELEMENTS = num_l3_se;
    if (num_l2_se != -1) L2_NUM_ELEMENTS = num_l2_se;

    if (!is_dnx) {
        if ((rv = bcm_stk_my_modid_set(unit, my_modid)) != BCM_E_NONE) {
            printf("bcm_stk_my_modid_set failed $rv\n");
            return rv;
        }
    }

    printf ("------------------------------------------------\n");
    printf ("Setting Scheduling Hirarchy: num_ports=%d l4_se=%d l3_se=%d l2_se=%d\n\n",
                    NUMBER_OF_PORTS,L4_NUM_ELEMENTS, L3_NUM_ELEMENTS, L2_NUM_ELEMENTS);

    /* IPS thresholds  for 1G rate , Slow disabled (ingress side)*/
    bcm_cosq_delay_tolerance_t delay_tol = {0};

    delay_tol.credit_request_hungry_normal_to_slow_thresh             = -32*1024;
    delay_tol.credit_request_hungry_off_to_normal_thresh              = -2*1024;
    delay_tol.credit_request_hungry_slow_to_normal_thresh             = -32*1024;
    delay_tol.credit_request_hungry_off_to_slow_thresh                = -2*1024;
    delay_tol.credit_request_satisfied_backlog_enter_thresh           = 4096;
    delay_tol.credit_request_satisfied_backlog_exit_thresh            = 2048;
    delay_tol.credit_request_satisfied_backoff_enter_thresh           = 8*1024;
    delay_tol.credit_request_satisfied_backoff_exit_thresh            = 6*1024;
    delay_tol.credit_request_satisfied_empty_queue_max_balance_thresh = 5000;
    delay_tol.credit_request_satisfied_empty_queue_thresh             = 4000;
    delay_tol.flags                                                   = BCM_COSQ_DELAY_TOLERANCE_TOLERANCE_OCB_ONLY;

    /** For jericho2 use a special backward compatable cint to set
     *  the credit request profiles */
    if (!is_dnx) {
        if ((rv = bcm_cosq_delay_tolerance_level_set(unit, BCM_COSQ_DELAY_TOLERANCE_NORMAL,
                                                     &delay_tol)) != BCM_E_NONE) {
            printf("bcm_cosq_delay_tolerance_level_set() failed $rv\n");
            return rv;
        }
    } else {
        uint32 *data_val = dnxc_data_get(unit, "iqs", "credit", "worth", NULL);
        remote_credit_size = *data_val;
        rv = cint_credit_request_profile_backward_compatibilty_set(unit, remote_credit_size);
        if (rv != BCM_E_NONE) {
            printf("cint_credit_request_profile_backward_compatibilty_set() failed $rv\n");
            return rv;
        }
    }

    /* Build an array of GPORTS of HRs.
       Each HR represents priority of specific port.
       These HRs are used to connect the aggregation level to the port.
    */
    port_index=0;
    BCM_PBMP_ITER(port_set, port) {
        /* Create GPORT for locoal port*/
        BCM_GPORT_LOCAL_SET(gloc_port, port);

        /* Get GPORT of Port TC of the designated local port*/
        gport_info_porttc.cosq = 0;
        gport_info_porttc.in_gport = gloc_port;

        if ((rv = bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeE2EPortTC,
                                            &gport_info_porttc)) != BCM_E_NONE) {
            printf("bcm_cosq_gport_handle_get for bcmCosqGportTypeE2EPortTC failed $rv\n");
            return rv;
        }

        /* Get GPORT of HR */
        gport_info_hr.in_gport = gport_info_porttc.out_gport;
        rv = bcm_port_get(unit, port, &uflags, &interface_info, &mapping_info);
        if (rv != BCM_E_NONE) {
            printf("bcm_port_get: rv(%d), unit(%d), failed to get port(%d) info \n",rv, unit,port);
            /*return rv;*/
        }

        for (prio=0; prio < mapping_info.num_priorities; prio++) {
             gport_info_hr.cosq = prio;
             if ((rv = bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSched,
                                                 &gport_info_hr)) != BCM_E_NONE) {
                 printf("bcm_cosq_gport_handle_get for bcmCosqGportTypeSched failed $rv: pri=$prio\n");
                 return rv;
             }
             hr_info_gport_arr[(port_index)*HR_PER_PORTS+prio] = gport_info_hr.out_gport;
        }

        /* Get a handler to the E2E Port-Scheduler*/
        BCM_COSQ_GPORT_E2E_PORT_SET(&ge2e, &port);

        /* Set shaper rate */
        if ((rv = bcm_cosq_gport_bandwidth_set(unit, ge2e, 0, 0,
                                               PORT_RATE, 0)) != BCM_E_NONE) {
            printf("bcm_cosq_gport_bandwidth_set for port failed $rv: pri=$prio\n");
            return rv;
        }
        port_index++;
    }

    /* Build level 4:
       [1] Greate a GPORT of a composite CL of type SP (only 2 SP's are used).
       [2] CIR gport is attached to SP1
       [3] EIR gport is attached to SP2
       [4] Define the connection point on the "father" HR
       [5] connect the CL to the "father" (credit source).
    */

    printf ("\n**** Level 4****\n");
    dbl = L4_CIR_RATE + L4_EIR_RATE;
    printf ("CR Rate: %3.3f Mbits, SE Elements: %d\n", (dbl/UNIT_1K), L4_NUM_ELEMENTS);

    /* The following flags are used for L4 and L3 SE's */
    flags = (BCM_COSQ_GPORT_SCHEDULER |
             BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE4_2SP_WFQ |
             BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_INDEPENDENT |
             BCM_COSQ_GPORT_COMPOSITE);
    port_index=0;
    BCM_PBMP_ITER(port_set,port) {
         for (se_elem = 0; se_elem < L4_NUM_ELEMENTS; se_elem++) {

             cir_gport = cint_composite_sched_config (unit, port, flags, L4_CIR_RATE,
                                          L4_EIR_RATE, L4_CIR_BURST, L4_EIR_BURST,
                                          EIR_SP_MODE[4-2], L4_EIR_WEIGHT[se_elem]);
             if (cir_gport == NULL)
             {
                 printf("Configuring L4 SE %d ERROR: port=%d l4=%d\n",
                        (port_index)*L4_NUM_ELEMENTS + se_elem,
                         port, se_elem);
                 return -1;
             }

             gport_info.cosq=0;
             gport_info.in_gport = cir_gport;
             bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeCompositeFlow2,
                                       &gport_info);
             eir_gport = gport_info.out_gport;

             if ((rv = bcm_cosq_gport_attach(unit,
                             hr_info_gport_arr[(port_index)*HR_PER_PORTS+0],
                             cir_gport, 0)) != BCM_E_NONE) {
                 printf("bcm_cosq_gport_attach() for SP0 failed $rv: cir_gport=0x%x\n", cir_gport);
                 return rv;
             }

             if ((rv = bcm_cosq_gport_attach(unit,
                              hr_info_gport_arr[(port_index)*HR_PER_PORTS+1],
                              eir_gport, 0)) != BCM_E_NONE) {
                 printf("bcm_cosq_gport_attach() for SP1 failed $rv: eir_gport=0x%x\n", eir_gport);
                 return rv;
             }
             level4_gport_arr[(port_index)*L4_NUM_ELEMENTS + se_elem] = cir_gport;
         }
         port_index++;
    }

    /* Build level 3 SE:
       [1] Greate a GPORT of a composite CL of type single WFQ.
       [2] get EIR gport
       [3] Set the shaper of the CIR, and disable the shaper of the EIR.
       [4] Define the connection point on the "father" level4 element
       [5] connect the CL to the "father" (credit source).
    */

    printf ("\n**** Level 3 ****\n");
    dbl = L3_CIR_RATE + L3_EIR_RATE;
    printf ("CR Rate: %3.3f Mbits, SE Elements: %d\n", (dbl/UNIT_1K), L3_NUM_ELEMENTS);
    port_index=0;
    BCM_PBMP_ITER(port_set,port) {
        for (l4_index = 0; l4_index < L4_NUM_ELEMENTS; l4_index++) {
            for (se_elem = 0; se_elem < L3_NUM_ELEMENTS; se_elem++) {

                se_index = (port_index)*L4_NUM_ELEMENTS*L3_NUM_ELEMENTS  +
                            l4_index*L3_NUM_ELEMENTS + se_elem;

                cir_gport = cint_composite_sched_config (unit, port, flags, L3_CIR_RATE,
                                L3_EIR_RATE, L3_CIR_BURST, L3_EIR_BURST,
                                EIR_SP_MODE[3-2], L3_EIR_WEIGHT[se_elem]);
                if (cir_gport == NULL)
                {
                    printf("Configuring L3 SE %d ERROR: port=%d l4=%d l3=%d\n",
                           se_index, port, l4_index, se_elem);
                    return -1;
                }

                gport_info.cosq=0;
                gport_info.in_gport = cir_gport;
                bcm_cosq_gport_handle_get (unit, bcmCosqGportTypeCompositeFlow2,
                                           &gport_info);
                eir_gport = gport_info.out_gport;

                parent_gport = level4_gport_arr[l4_index];

                if ((rv = bcm_cosq_gport_attach(unit, parent_gport, cir_gport, 0))
                                      != BCM_E_NONE) {
                    printf("bcm_cosq_gport_attach for CIR failed $rv: se_index=$se_index\n");
                    return rv;
                }

                if ((rv = bcm_cosq_gport_attach(unit, parent_gport,
                                eir_gport, 0)) != BCM_E_NONE) {
                    printf("bcm_cosq_gport_attach for EIR failed $rv: se_index=$se_index\n");
                    return rv;
                }

                level3_gport_arr[se_index] = cir_gport;
            }
        }
        port_index++;
    }

    /* Build level 2:
       [1] Greate a GPORT of a composite CL of type 3 SPs with single last WFQ.
       [2] Define the connection point on the "father" level3 element
       [3] CIR gport is attached to SP1 of father SE
       [4] EIR gport is attached to SP2 of father SE
       [5] connect the CL to the "father" (credit source).
    */

    printf ("\n**** Level 2 ****\n");
    dbl = L2_CIR_RATE+L2_EIR_RATE;
    printf ("CR Rate: %3.3f Mbits, SE Elements: %d\n", (dbl/UNIT_1K), L2_NUM_ELEMENTS);

    flags = (BCM_COSQ_GPORT_SCHEDULER |
             BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE4_2SP_WFQ |
             BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_INDEPENDENT |
             BCM_COSQ_GPORT_COMPOSITE);
    port_index=0;
    BCM_PBMP_ITER(port_set,port) {
        for (l4_index = 0; l4_index < L4_NUM_ELEMENTS; l4_index++) {
            for (l3_index = 0; l3_index < L3_NUM_ELEMENTS; l3_index++) {
                for (se_elem=0; se_elem < L2_NUM_ELEMENTS; se_elem++) {

                    se_index = (port_index)*L4_NUM_ELEMENTS*L3_NUM_ELEMENTS*L2_NUM_ELEMENTS +
                                l4_index*L3_NUM_ELEMENTS*L2_NUM_ELEMENTS +
                                l3_index*L2_NUM_ELEMENTS + se_elem;

                    cir_gport = cint_composite_sched_config (unit, port, flags,
                                          L2_CIR_RATE, L2_EIR_RATE, L2_CIR_BURST,
                                          L2_EIR_BURST, EIR_SP_MODE[2-2],
                                          L2_EIR_WEIGHT[se_elem]);
                    if (cir_gport == NULL)
                    {
                        printf("Configuring L2 SE %d ERROR: port=%d l4=%d l3=%d l2=%d\n",
                               se_index, port, l4_index, l3_index, se_elem);
                        return -1;
                    }

                    gport_info.cosq=0;
                    gport_info.in_gport = cir_gport;
                    if ((rv = bcm_cosq_gport_handle_get(unit,
                                      bcmCosqGportTypeCompositeFlow2,
                                      &gport_info)) != BCM_E_NONE) {
                        printf("bcm_cosq_gport_handle_get for EIR failed $rv: se_index=$se_index\n");
                        return rv;
                    }
                    eir_gport = gport_info.out_gport;

    		    parent_gport = level3_gport_arr[(port_index)*L4_NUM_ELEMENTS +
                                                    l4_index*L3_NUM_ELEMENTS + l3_index];

                    if ((rv = bcm_cosq_gport_attach(unit, parent_gport,
                                                    cir_gport, 0)) != BCM_E_NONE) {
                        printf("bcm_cosq_gport_attach for CIR failed $rv: se_index=$se_index\n");
                        return rv;
                    }
                    if ((rv = bcm_cosq_gport_attach(unit, parent_gport,
                                                    eir_gport, 0)) != BCM_E_NONE) {
                        printf("bcm_cosq_gport_attach for EIR failed $rv: se_index=$se_index\n");
                        return rv;
                    }

                    level2_gport_arr[se_index] = cir_gport;
                }
            }
        }
        port_index++;
    }

    /*
         Create VOQ connectors and VOQs
     */

    /* VOQ connector flags */
    flags  = BCM_COSQ_GPORT_VOQ_CONNECTOR | BCM_COSQ_GPORT_WITH_ID;

    /* VOQ flags */
    flags2 = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP |  BCM_COSQ_GPORT_WITH_ID;

    printf ("\n**** SET VOQs and VOQ connectors ****\n");
	port_index=0;
    BCM_PBMP_ITER(port_set,port) {
        /* create gport  */
        BCM_GPORT_LOCAL_SET(portmod_gport, port);
        /* encapsulate sys port id to gport */
        BCM_GPORT_SYSTEM_PORT_ID_SET(system_port_gport,(my_modid * 100) + port);

        if ((rv = bcm_stk_sysport_gport_set(unit, system_port_gport, portmod_gport))
                                 != BCM_E_NONE) {
            printf ("bcm_stk_sysport_gport_set failed $rv: system_port_gport=0x%x portmod_gport=0x%x\n",
                       system_port_gport, portmod_gport);
            return rv;
        }

        for (l4_index = 0; l4_index < L4_NUM_ELEMENTS; l4_index++) {

            printf("Configuring VOQ[%d:%d:%d:%d]: voq_index=%d flow_id=0x%x\n",
                port, l4_index, 0, 0,
                (port_index)*L4_NUM_ELEMENTS*L3_NUM_ELEMENTS*L2_NUM_ELEMENTS +
                l4_index* L3_NUM_ELEMENTS*L2_NUM_ELEMENTS,
                (((port_index)*L4_NUM_ELEMENTS*L3_NUM_ELEMENTS*L2_NUM_ELEMENTS +
                l4_index* L3_NUM_ELEMENTS*L2_NUM_ELEMENTS) * 4) + voq_base);

            for (l3_index = 0; l3_index < L3_NUM_ELEMENTS; l3_index++) {
                for (se_elem = 0; se_elem < L2_NUM_ELEMENTS; se_elem++) {

                    voq_index = (port_index)*L4_NUM_ELEMENTS*L3_NUM_ELEMENTS*L2_NUM_ELEMENTS +
                                l4_index* L3_NUM_ELEMENTS*L2_NUM_ELEMENTS +
                                l3_index* L2_NUM_ELEMENTS + se_elem;
                    /* create VOQ connector for a VOQ bundle */
                    connector_id = (voq_index * 4) + voq_base;

                    BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(vc_group,
                             connector_id, mapping_info.core);

                    voq_connector.flags = flags;
                    voq_connector.port = portmod_gport;
                    voq_connector.nof_remote_cores = 1;
                    voq_connector.remote_modid = my_modid + mapping_info.core;
                    voq_connector.numq = 4;
                    rv = bcm_cosq_voq_connector_gport_add(unit, voq_connector, &vc_group);
                    if (rv != BCM_E_NONE) {
                        printf("bcm_cosq_gport_add failed $rv: vc_group=0x%x\n",
                                    vc_group);
                        return rv;
                    }

                    /* set VOQ connector shaper */
                    for (cos = 0; cos < 2; cos++) {
                        if ((rv = bcm_cosq_gport_bandwidth_set(unit, vc_group, cos,
                                          0, VOQ_RATE[cos], 0)) != BCM_E_NONE) {
                            printf("bcm_cosq_gport_bandwidth_set failed $rv: vc_group=0x%x\n", vc_group);
                            return rv;
                        }

                        if ((rv = bcm_cosq_control_set(unit, vc_group, cos,
                                                  bcmCosqControlBandwidthBurstMax,
                                                  VOQ_MAX_BURST)) != BCM_E_NONE) {
                            printf("bcm_cosq_control_set failed $rv: vc_group=0x%x\n", vc_group);
                            return rv;
                        }
                    }

                    for (cos = 0; cos < 4; cos++) {
                        if ((rv = bcm_cosq_gport_sched_set(unit, vc_group, cos,
                                 VOQ_COS_SP[cos], VOQ_COS_WEIGHT[cos])) != BCM_E_NONE) {
                            printf("bcm_cosq_gport_sched_set failed $rv: vc_group=x%x cos=%d\n",
                                            vc_group, cos);
                            return rv;
                        }

			/* set slow disable for voq */
                        if ((rv = bcm_cosq_control_set(unit, vc_group, cos,
                                            bcmCosqControlFlowSlowRate,
                                            0)) != BCM_E_NONE) {
                            printf("bcm_cosq_control_set failed $rv: vc_group=0x%x cos=%d\n",
                                            vc_group, cos);
                            return rv;
                        }

			/* attach VOQ connector to level 2 CL */
                        if ((rv = bcm_cosq_gport_attach(unit,
                                           level2_gport_arr[voq_index],
                                           vc_group, cos)) != BCM_E_NONE) {
                            printf("bcm_cosq_gport_sched_set failed $rv: vc_group=0x%x cos=%d\n", vc_group, cos);
                            return rv;
                        }
                    }

                    /*
                       [1] Create VOQ GPORT
                       [2] Assotiate the VOQ with the system port
                    */
                    /** For Jericho2 only Symetric mode is supported */
                    if (is_dnx) {
                        core = BCM_CORE_ALL;
                    } else {
                        core = mapping_info.core;
                    }
                    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(gvoq_base, core,
                                                      (voq_index * 4) + voq_base);

                    if ((rv = bcm_cosq_gport_add(unit, system_port_gport, 4, flags2,
                                                 &gvoq_base)) != BCM_E_NONE) {
                        printf("bcm_cosq_gport_add failed $rv: gvoq_base=0x%x\n",
                                            gvoq_base);
                        return rv;
                    }
                    /** bcm_cosq_gport_add will return a gport with core == BCM_CORE_ALL
                     * In order to be complient with the asymmetric scheme created
                     * the VOQ gport BCM_CORE_ALL should be replaced with the relevant core */
                    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(gvoq_base, mapping_info.core,
                                                                 (voq_index * 4) + voq_base);

                    if (voq_index < sizeof(voq_gport_arr)/sizeof(voq_gport_arr[0]))
                        voq_gport_arr[voq_index] = gvoq_base;
                    /** In Jr2 the compensation is configured in
                     *  cint_credit_request_profile_backward_compatibilty_set
                     *  cint */
                    if (!is_dnx) {
                        for (cos = 0; cos < 4; cos++) {
                            if ((rv = bcm_cosq_control_set(unit, gvoq_base, cos,
                                                 bcmCosqControlPacketLengthAdjust,
                                                 CREDIT_ADJUST)) != BCM_E_NONE) {
                                printf ("failed setting the credit discount $rv: voq_gport=0x%x cosq=%d\n",
                                                 gvoq_base, cos);
                            }
                        }
                    }

                    /* Egress side: connect VOQ to voq connector */

                    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
                    connection.remote_modid = my_modid + mapping_info.core;
                    connection.voq = gvoq_base;
                    connection.voq_connector = vc_group;
                    if ((rv = bcm_cosq_gport_connection_set(unit, &connection))
                                != BCM_E_NONE) {
                        printf("bcm_cosq_gport_connection_set failed $rv\n");
                        return rv;
                    }

                    /* Ingress side: connect VOQ to VOQ-connector */

                    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
                    connection.remote_modid = my_modid + mapping_info.core;
                    connection.voq = gvoq_base;
                    connection.voq_connector = vc_group;

                    if ((rv = bcm_cosq_gport_connection_set(unit, &connection))
                                 != BCM_E_NONE) {
                        printf("bcm_cosq_gport_connection_set failed $rv\n");
                        return rv;
                    }
                }
            }
        }
        printf	("\n" );
        port_index++;
    }

    /* Enable traffic. This step can be done any time after bcm and soc are initialized  */
    if ((rv = bcm_stk_module_enable(unit, my_modid, NUMBER_OF_PORTS, 1))
                       != BCM_E_NONE) {
        printf("bcm_stk_module_enable() failed $rv\n");
        return rv;
    }
    return rv;
}


/* Change the BW of the egress port.
   unit: 0
   port: egress port number.
   rate: unit is in kbps.
*/

int cint_change_port_rate (int  unit,
                           int  port,
                           int  rate)
{
    int          rv;
    bcm_gport_t  ge2e;

    /* Get a handler to the E2E Port-Scheduler */
    BCM_COSQ_GPORT_E2E_PORT_SET(&ge2e, &port);

    /* Set shaper rate */
    if ((rv = bcm_cosq_gport_bandwidth_set(unit, ge2e, 0, 0,
                                           rate, 0)) != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set for port failed $rv: gport=0x%x port=%d\n",
                                           ge2e, port);
        return rv;
    }
}


/* Change the CIR/EIR BW of an SE.

   unit: 0
   level: user=2, port=3, LM=4
   Index: 0 - 1st user/port/LM ... 5/23/23 - last user/port/LM
   cir_rate: unit is kbps.
             To change only EIR, set cir_rate to -1.
             Setting rate=0 will disable rate shaping (unlimited).
   eir_rate: unit is kbps.
             To change only CIR, set eir_rate to -1.
             Setting rate=0 will disable rate shaping (unlimited).
*/

int cint_change_se_rate (int  unit,
                         int  level,
                         int  index,
                         int  cir_rate,
                         int  eir_rate)
{
   int                    rv;
   bcm_gport_t            cir_gport;
   bcm_cosq_gport_info_t  gport_info;
   bcm_gport_t            *gport_p[] = {level2_gport_arr,
                                        level3_gport_arr,
                                        level4_gport_arr};

   /* default to level2 if level is invalid. */
   cir_gport = gport_p[(level >= 2 && level <= 4) ? (level-2):0][index];

   if (cir_rate != -1 && (rv = bcm_cosq_gport_bandwidth_set(unit, cir_gport,
                                          0, 0, cir_rate, 0)) != BCM_E_NONE) {
       printf("bcm_cosq_gport_bandwidth_set() failed $rv: cir_gport=0x%x\n",
                                     cir_gport);
   }

   gport_info.cosq = 0;
   gport_info.in_gport = cir_gport;
   bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeCompositeFlow2, &gport_info);

   if (eir_rate != -1 && (rv = bcm_cosq_gport_bandwidth_set(unit, gport_info.out_gport,
                                          0, 0, eir_rate, 0)) != BCM_E_NONE) {
       printf("bcm_cosq_gport_bandwidth_set() failed $rv: eir_gport=0x%x\n",
                                     gport_info.out_gport);
   }

   return rv;
}



/* unit: 0
   level: user=2, port=3, LM=4
   Index: 0 - 1st user/port/LM ... 5/23/23 - last user/port/LM
   eir_weigth: unit is 1..63. The lower weight gets the higher rate.
*/

int cint_change_se_weight (int  unit,
                           int  level,
                           int  index,
                           int  eir_weight)
{
   int                    rv;
   bcm_gport_t            cir_gport;
   bcm_cosq_gport_info_t  gport_info;
   bcm_gport_t            *gport_p[] = {level2_gport_arr,
                                        level3_gport_arr,
                                        level4_gport_arr};

   /* default to level2 if level is invalid. */
   if (level < 2 || level > 4) level = 2;
   cir_gport = gport_p[level-2][index];

   gport_info.cosq = 0;
   gport_info.in_gport = cir_gport;
   bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeCompositeFlow2, &gport_info);

   if ((rv = bcm_cosq_gport_sched_set(unit, gport_info.out_gport, 0,
                                   EIR_SP_MODE[level-2], eir_weight)) != BCM_E_NONE) {
       printf("bcm_cosq_gport_sched_set() for EIR failed $rv: eir_gport=0x%x\n",
                                   eir_gport);
       return NULL;
   }

   return rv;
}


/* Change the BW of a VOQ connector.

   unit: 0
   voq_bundle_id: 1st voq of a VOQ bundle.
   cos: 0-3. voq_bundle_id+cos gets a VOQ.
   rate: unit is kbps. Setting rate=0 will disable rate shaping (unlimited).
*/

int cint_change_vc_rate (int  unit,
                         int  core,
                         int  voq_bundle_id,  /* 1st voq of 4 VOQ's*/
                         int  cos,
                         int  rate)
{
    int         rv;
    bcm_gport_t vc_group;

    BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(vc_group, voq_bundle_id, core);

    if ((rv = bcm_cosq_gport_bandwidth_set(unit, vc_group, cos,
                                           0, rate, 0)) != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set failed $rv: vc_group=0x%x\n", vc_group);
        return rv;
    }
}


/* Change the weight of a VOQ connector.

   unit: 0
   voq_bundle_id: 1st voq of a VOQ bundle.
   cos: 0-3. voq_bundle_id+cos gets a VOQ.
   weight: relative weight, 0..63.
*/

int cint_change_vc_weight (int  unit,
                           int  voq_bundle_id,  /* 1st voq of 4 VOQ's */
                           int  cos,
                           int  eir_weight)
{
    int         rv;
    bcm_gport_t vc_group;

    BCM_COSQ_GPORT_VOQ_CONNECTOR_CORE_SET(vc_group, voq_bundle_id, 0);

    if ((rv = bcm_cosq_gport_sched_set(unit, vc_group, cos,
                              BCM_COSQ_SP1, eir_weight)) != BCM_E_NONE) {
         printf("bcm_cosq_gport_sched_set failed $rv: vc_group=x%x cos=%d\n",
                              vc_group, cos);
    }
    return rv;
}


void cint_tm_rate_no_limit (int unit, int port_rate)
{
    int level;

    if (port_rate != -1)
        cint_change_port_rate(unit,2,port_rate);

    for (level=2; level <= 4; level++) {
        cint_change_se_rate(unit,level,0,0,0);
    }
    cint_change_vc_rate(unit, 0x20, 0, 0);
}


int cint_tm_setup_full (int unit,int port_base,int nof_ports)
{
    return cint_tm_setup(unit, port_base, nof_ports,
                         L4_NUM_ELEMENTS, L3_NUM_ELEMENTS, L2_NUM_ELEMENTS);
}


/* Uncommenting out the following code will trigger automatic execution of this function
   when cint is started.
*/

/* cint_tm_setup(PORT_NUM_BASE, NUMBER_OF_PORTS, L4_NUM_ELEMENTS, L3_NUM_ELEMENTS, L2_NUM_ELEMENTS); */


