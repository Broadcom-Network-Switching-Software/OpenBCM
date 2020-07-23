/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
*
* File: cint_pon_dn_ratelimit_dml.c  
* 
* Purpose: examples of TM scheduler BCM API.  
*
* - Port Scheduler
*   - HR Scheduler (SP mode).
*     - L1 CL Scheduler (WFQ independent mode), Hookup to HR Scheduler (SP1).
*       - L2 CL Schedulers (SP mode)  
*         Instance 0 used for 10G channel. 
*         Instance 1 used for 1G channel.
*         Instance 0 and 1 hookup to L1 CL Scheduler (W1, W2)(W1:W2=10:1).
*         - L3 CL Schedulers (SP mode)
*           Instance 0 used for 10G unicast channel. 
*           Instance 1 used for 10G multicast channel.   
*           Instance 2 used for 1G unicast channel.
*           Instance 3 used for 1G multicast channnel.
*           Instance 0 and 1 hookup to L2 CL Scheduler Instance 0 (SP1, SP0).  
*           Instance 2 and 3 hookup to L2 CL Scheduler Instance 1 (SP1, SP0).  
*           - Multicast VOQ and VOQ connector.
*             Instance 2 used for 10G multicast channel.
*             Instance 3 used for 1G multicast channel.
*             Instance 2 (COS 6:7, COS 4:5, COS 2:3, COS 0:1) hookup to L3 CL Scheduler Instance 1 (SP0, SP1, SP2, SP3).  
*             Instance 3 (COS 6:7, COS 4:5, COS 2:3, COS 0:1) hookup to L3 CL Scheduler Instance 3 (SP0, SP1, SP2, SP3).
*           - L4 CL Schedulers (WFQ indenpendent mode) 
*             Instance 0 used for 10G unicast CIR channel. 
*             Instance 1 used for 10G unicast PIR channel.
*             Instance 2 used for 1G unicast CIR channel.
*             Instance 3 used for 1G unicast PIR channel.
*             Instance 0 and 1 hookup to L3 CL Scheduler Instance 0 (SP0, SP1).  
*             Instance 2 and 3 hookup to L3 CL Scheduler Instance 2 (SP0, SP1).
*             - L5 CL Schedulers (SP composite mode).  
*               Instance 0 used for 10G unicast ONU-1 channel.  
*               Instance 1 used for 10G unicast ONU-2 channel.
*               Instance 2 used for 1G unicast ONU-3 channel.
*               Instance 3 used for 1G unicast ONU-4 channel.
*               Instance 0 hookup to L4 CL Scheduler Instance 0 and 1 (W1, W1).   
*               Instance 1 hookup to L4 CL Scheduler Instance 0 and 1 (W2, W2).
*               Instance 2 hookup to L4 CL Scheduler Instance 2 and 3 (W1, W1).
*               Instance 3 hookup to L4 CL Scheduler Instance 2 and 3 (W2, W2).
*               - Unicast VOQ and VOQ connector.
*                 Instance 0 used for 10G unicast ONU-1.  
*                 Instance 1 used for 10G unicast ONU-2.
*                 Instance 4 used for 1G unicast ONU-3.
*                 Instance 5 used for 1G unicast ONU-4.
*                 Instance 0 (COS 6:7, COS 4:5, COS 2:3, COS 0:1) hookup to L5 CL Scheduler Instance 0 (SP0, SP1, SP2, SP3).      
*                 Instance 1 (COS 6:7, COS 4:5, COS 2:3, COS 0:1) hookup to L5 CL Scheduler Instance 1 (SP0, SP1, SP2, SP3).
*                 Instance 4 (COS 6:7, COS 4:5, COS 2:3, COS 0:1) hookup to L5 CL Scheduler Instance 2 (SP0, SP1, SP2, SP3).
*                 Instance 5 (COS 6:7, COS 4:5, COS 2:3, COS 0:1) hookup to L5 CL Scheduler Instance 3 (SP0, SP1, SP2, SP3).
* 
********************************************************************************                
*                                                                                               
*    (V0)         (L4.0)          (L3.0)                                                        
*  +-----+        +----+          +----+                                                        
*  |     |COS:6,7 |     \         |     \                                                       
*  |  V  |<-------|SP0   \        |      \                                                      
*  |     |COS:4,5 |       +-------|W1     +                                                     
*  |  O  |<-------|SP1  S |       |     W |_ _                                                  
*  |     |COS:2,3 |     P |       |     F |   |   (L2.0)                                        
*  |  Q  |<-------|SP2    +       |W2   Q +   |   +----+                                        
*  |     |COS:0,1 |      / \     /|      /    |   |     \                                       
*  |     |<-------|SP3  /   \   / |     /     |_ _|SP0   \                                      
*  +-----+        +----+     \ /  +----+          |       +                                     
*    (V1)         (L4.1)      /   (L3.1)          |     S |_ _                                  
*  +-----+        +----+     / \  +----+       _ _|     P |   |                                 
*  |     |COS:6,7 |     \   /   \ |     \     |   |SP1    +   |                                 
*  |  V  |<-------|SP0   \ /     \|      \    |   |      /    |                                 
*  |     |COS:4,5 |       +       |W1     +   |   |     /     |                                 
*  |  O  |<-------|SP1  S |       |     W |_ _|   +----+      |                                 
*  |     |COS:2,3 |     P |       |     F |                   |                                 
*  |  Q  |<-------|SP2    +-------|W2   Q +                   |UCAST                            
*  |     |COS:0,1 |      /        |      /                    |                                 
*  |     |<-------|SP3  /         |     /                     |                                 
*  +-----+        +----+          +----+                      |       (L1.0)                    
*                                                             |       +----+                    
*                                                             |       |10G  \                   
*                                                             |       |      \                  	
*                                                             |       |       +                 
*                                                             |_ _ _ _|     H |_ _              
*                                                                     |SP0  R |   |             
*                                                                     |       +   |             
*                                                                     |      /    |   +----+    
*                                                                     |     /     |   |     \   
*                                                                     +----+      |_ _|      \  
*                                                                                     |TC0  P + 
*                                                                                     |     O | 
*                                                                                     |     R | 
*                                                                     (L1.1)       _ _|     T + 
*                                                                     +----+      |   |TC1   /  
*                                                                     |1G   \     |   |     /   
*                                                                     |      \    |   +----+    
*                                                                     |       +   |             
*                                                                     |     H |_ _|             
*                                                              _ _ _ _|SP0  R |                 
*                                                             |       |       +                 
*                                                             |       |      /                  
*    (V2)         (L4.2)          (L3.2)                      |       |     /                   
*  +-----+        +----+          +----+                      |       +----+                    
*  |     |COS:6,7 |     \         |     \                     |                                 
*  |  V  |<-------|SP0   \        |      \                    |                                 
*  |     |COS:4,5 |       +-------|W1     +                   |                                 
*  |  O  |<-------|SP1  S |       |     W |_ _                |UCAST                            
*  |     |COS:2,3 |     P |       |     F |   |   (L2.1)      |                                 
*  |  Q  |<-------|SP2    +       |W2   Q +   |   +----+      |                                 
*  |     |COS:0,1 |      / \     /|      /    |   |     \     |                                 
*  |     |<-------|SP3  /   \   / |     /     |_ _|SP0   \    |                                 
*  +-----+        +----+     \ /  +----+          |       +   |                                 
*    (V3)         (L4.3)      /   (L3.3)          |     S |_ _|                                 
*  +-----+        +----+     / \  +----+       _ _|     P |                                     
*  |     |COS:6,7 |     \   /   \ |     \     |   |SP1    +                                     
*  |  V  |<-------|SP0   \ /     \|      \    |   |      /                                      
*  |     |COS:4,5 |       +       |W1     +   |   |     /                                       
*  |  O  |<-------|SP1  S |       |     W |_ _|   +----+                                        
*  |     |COS:2,3 |     P |       |     F |                                                     
*  |  Q  |<-------|SP2    +-------|W2   Q +                                                     
*  |     |COS:0,1 |      /        |      /                                                      
*  |     |<-------|SP3  /         |     /                                                       
*  +-----+        +----+          +----+                                                        
*                                                                                               
*                                 SCHEDULER MODEL                                               
*                                                                                               
* ******************************************************************************                


*       
* Note: If we want to use the WFQ independent mode, we must calculate the least
*    common multiple(LMM), then use the LMM divide the expect weight value(WE) 
*    to calculate the actual weight value.   
*    For example: WE1 : WE2 : WE3 : WE4 = 4 : 3 : 3 : 2.
*        LMM = 12.
*        WA1 = LMM / WE1 = 12 / 4 = 3.         
*        WA2 = LMM / WE2 = 12 / 3 = 4.
*        WA3 = LMM / WE3 = 12 / 3 = 4.
*        WA4 = LMM / WE4 = 12 / 2 = 6.
*        So WA1 : WA2 : WA3 : WA4 = 3 : 4 : 4 : 6.
*
* PP Model:
*     The PP model is created based on the TM model.
*     VOQ 0 (1000:1007) <--> 10G Unicast LLID 0 (LLID-1000, AC-0)
*     VOQ 1 (1008:1015) <--> 10G Unicast LLID 1 (LLID-1001, AC-1)

*     VOQ 2 (2000:2008) <--> 1G Unicast LLID 2 (LLID-2000, AC-2)
*     VOQ 3 (2008:2015) <--> 1G Unicast LLID 2 (LLID-2000, AC-3)

*
* Map PKT_PRI_CFI to System (TC/DP) by calling bcm_qos_map_add as follows:
*     Ingress(PKT_PRI/CFI)    System(TC/DP)
*     0/0,0/1,1/0,1/1         0/GREEN
*     2/0,2/1,3/0,3/1         1/GREEN
*     4/0,4/1,5/0,5/1         2/GREEN
*     6/0,6/1,7/0,7/1         3/GREEN
* 
* Map PKT_PRI to System TC for 1G traffic by using PMF as follows:
*     Ingress(PKT_PRI/CFI)    System TC
*     0                       4
*     1                       5
*     2                       6
*     3                       7
*     4                       4
*     5                       5
*     6                       6
*     7                       7
*
* Map System TC to Ingress TC for 1G traffic by calling bcm_cosq_port_mapping_set as follows:
*     System TC               Ingress(TC/DP)
*     4                       0
*     5                       1
*     6                       2
*     7                       3
*
* Map System (TC/DP) to Egress TC for 1G traffic by calling bcm_cosq_gport_egress_map_set as follows: 
*     System(TC/DP)           Egress TC
*     0/0,0/1,0/2,0/3         1
*     1/0,1/1,1/2,1/3         1
*     2/0,2/1,2/2,2/3         1
*     3/0,3/1,3/2,3/3         1
* 
* Map System (TC/DP) to Egress TC for 10G traffic by calling bcm_cosq_gport_egress_map_set as follows: 
*     System(TC/DP)           Egress TC
*     0/0,0/1,0/2,0/3         0
*     1/0,1/1,1/2,1/3         0
*     2/0,2/1,2/2,2/3         0
*     3/0,3/1,3/2,3/3         0
* 
*
* We support the below 4 Rate limit:
*     1. Rate limit on 10G/1G PON.
*     2. Rate limit on LLID. (10G unicast/multicast LLID)
*     3. Rate limit on VOQ. (1G multicast queue)
*     4. Rate limit on AC. (1G unicast LLID AC)
* For QAX device, need add followed additional configuration.
*     device_core_mode.BCM88470=SINGLE_CORE
*     dtm_flow_nof_remote_cores_region_1.BCM88470=1
*     dtm_flow_nof_remote_cores_region_2.BCM88470=1
*     dtm_flow_nof_remote_cores_region_3.BCM88470=1
*     .......
*     dtm_flow_nof_remote_cores_region_60.BCM88470=1
*
*
* To Activate Above Settings Run:
*     BCM> cint examples/dpp/cint_port_tpid.c
*     BCM> cint examples/dpp/pon/cint_pon_utils.c
*     BCM> cint examples/dpp/pon/cint_pon_dn_ratelimit_dml.c
*     BCM> cint
*     cint> pon_dn_ratelimit_init(0,4,128);
*     cint> pon_dn_ratelimit_setup(0);
*     cint> pon_dn_ratelimit_service_Init(0);
*     cint> pon_dn_ratelimit_service_setup(0);
*     cint> pon_dn_ratelimit_service_cleanup(0);
*     cint> pon_dn_ratelimit_cleanup(0);
* 
* To Activate PFC Reception Settings on PON port132 Run:
*     BCM> cint examples/dpp/pon/cint_pon_pfc_inbnd_receive.c
*     BCM> cint
*     cint> cint_arad_test_pfc_inband_rec_set(0, 132);
*
* To Activate Mapping from PKT_PRI to System TC for 1G traffic Run:
*     BCM> cint examples/dpp/pon/cint_pon_cos_remark_llid_dml.c
*     BCM> cint
*     cint> pon_dn_cos_remark(0, 5);
 */


/*******************************************************************************
*
*  Downstream rate limit schefuler
 */
int voq_base_queue_10G = 1000;
int voq_base_queue_1G = 2000;
bcm_port_t pon_port = 0;
bcm_port_t nni_port = 128;
bcm_gport_t pon_gport;
bcm_gport_t e2e_pon_gport;
/* Global scheduler */
bcm_gport_t gport_scheduler_1[2];    
/* Unicast scheduler, voq and voq connector */
bcm_gport_t gport_ucast_scheduler_2[2];
bcm_gport_t gport_ucast_scheduler_3[2][2];   
bcm_gport_t gport_ucast_scheduler_4[2][2];  
bcm_gport_t gport_ucast_scheduler_4_sf[2][2];
bcm_gport_t gport_ucast_voq_connector_group[2][2]; 
bcm_gport_t gport_ucast_voq_group[2][2];
int ucast_voq_connector_flow_id[2][2];
int ucast_voq_queue_id[2][2];

int l3_scheduler_weight[2][2] = { {1, 1}, {1, 1} };  
int l3_scheduler_weight_set[2][2];

/*******************************************************************************
* Calculate the common divisor
 */
int pon_dn_ratelimit_common_divisor(int a, int b)
{
    int temp;

    temp = (a > b) ? a : b;

    while(temp != 0)
    {
        temp = a % b;
        a = b;
        b = temp;
    }

    return a;
}


/*******************************************************************************
* Calculate the common multiple
 */
int pon_dn_ratelimit_common_multiple(int a, int b)
{
    int temp;

    temp = pon_dn_ratelimit_common_divisor(a, b);

    return (a * b / temp);
}


/*******************************************************************************
* Calculate the min multiple
 */
int pon_dn_ratelimit_min_multiple(int *array, int nums)
{
    int i;
    int multiple;
    int pre;
    int next;

    multiple = array[0];

    for (i = 0; i < (nums - 1); i++)
    {
        pre = multiple;
        next = array[i + 1];
        multiple = pon_dn_ratelimit_common_multiple(pre, next);
    }

    return multiple;
}


/*******************************************************************************
* Downstream L3 scheduler weight configuration
 */
void pon_dn_ratelimit_l3_weight_config(void)
{   
    int i, j;
    int multiple;
    
    for (i = 0; i < 2; i++)
    {
        multiple = pon_dn_ratelimit_min_multiple(l3_scheduler_weight[i], 2);

        for ( j = 0; j < 2; j++)
        {    
            l3_scheduler_weight_set[i][j] = multiple/l3_scheduler_weight[i][j];
        } 
    }   
}  


/*******************************************************************************
* Downstream rate limit init                                            
 */
int pon_dn_ratelimit_init(int unit, bcm_port_t port_pon, bcm_port_t port_nni)
{
    pon_port = port_pon;
    nni_port = port_nni;    
    /***************************************************************************
    /* 10G: 2 unicast LLID with double AC.*/
 */
    ucast_voq_queue_id[0][0] = voq_base_queue_10G;
    ucast_voq_connector_flow_id[0][0] = ucast_voq_queue_id[0][0];  

    ucast_voq_queue_id[0][1] = voq_base_queue_10G + 8;
    ucast_voq_connector_flow_id[0][1] = ucast_voq_queue_id[0][1]; 
    
    /***************************************************************************
    /* 1G: 2 unicast LLID with double AC. */
 */
    ucast_voq_queue_id[1][0] = voq_base_queue_1G;
    ucast_voq_connector_flow_id[1][0] = ucast_voq_queue_id[1][0];

    ucast_voq_queue_id[1][1] = voq_base_queue_1G + 8;
    ucast_voq_connector_flow_id[1][1] = ucast_voq_queue_id[1][1]; 

    pon_dn_ratelimit_l3_weight_config();
}    
     
     
/*******************************************************************************
* Downstream rate limit setup                                                 
 */ 
int pon_dn_ratelimit_setup(int unit)
{   
    int my_modid = 0;
    int num_cos = 4;
    int flags = 0;    
    int type;
    int index;
    int cosq;
    int voq;
    int kbits_sec_max;
    int max_burst;  bcm_gport_t gport_scheduler;
    bcm_error_t rv = BCM_E_NONE;    
    bcm_gport_t e2e_parent_gport;      
    bcm_cosq_gport_connection_t connection;
    int tc, dp, queue;
    bcm_gport_t  ucast_gport;
    bcm_cosq_gport_info_t gport_tc_info;
    bcm_cosq_voq_connector_gport_t config;
    int remote_cores = 0;
    int is_qax = 0;

    rv = is_qumran_ax_only(unit, &is_qax);
    if (rv != BCM_E_NONE) {
        printf("is_qumran_ax_only failed $rv\n");
        return rv;
    }

    if(is_qax) {
        remote_cores = soc_property_get(unit , "dtm_flow_nof_remote_cores_region_1.BCM88470",1);
    } else {
        remote_cores = soc_property_get(unit , "dtm_flow_nof_remote_cores_region_1.BCM88675",2);
    }

    rv = bcm_stk_modid_get(unit, &my_modid);
    if (rv != BCM_E_NONE) {
        printf("bcm_stk_my_modid_get failed $rv\n");
        return rv;
    }
    
    BCM_GPORT_MODPORT_SET(pon_gport, my_modid, pon_port); 
    
    /**************************************************************************/
    /* create Layer 1 HR scheduler */
    /**************************************************************************/
	/* type meaning: 0 <-> 10G, 1 <-> 1G */
    for (type = 0; type < 2; type++) 
    {
        BCM_COSQ_GPORT_E2E_PORT_TC_SET(gport_tc_info.in_gport, pon_port);
        gport_tc_info.cosq = type;
        rv = bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, &gport_tc_info);     
        if (rv != BCM_E_NONE) {
            LOG_CLI((BSL_META("bcm_cosq_gport_handle_get failed %d. Error:%d \n"), gport_tc_info.in_gport, rv));
            return rv;
        }

        flags = BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_HR_SINGLE_WFQ;
		    
        rv = bcm_cosq_gport_add(unit, pon_gport, 1, 
            flags, &gport_scheduler_1[type]);
		
        if (rv != BCM_E_NONE) {
            printf("L1 bcm_cosq_gport_add scheduler failed $rv\n");
            return rv;
        }
		
        rv = bcm_cosq_gport_sched_set(unit,
                 gport_scheduler_1[type],
                 0,
                 BCM_COSQ_SP0 + type,
                 0);
        if (rv != BCM_E_NONE) {
            printf("L1 bcm_cosq_gport_sched_set failed $rv\n");
            return rv;
        }
		
        BCM_COSQ_GPORT_E2E_PORT_SET(e2e_pon_gport, pon_port);
		
        rv = bcm_cosq_gport_attach(unit, gport_tc_info.out_gport, 
                 gport_scheduler_1[type], 0);
		    
        if (rv != BCM_E_NONE) {
            printf("L1 bcm_cosq_gport_attach failed $rv\n");
            return rv;
        }  
    }

    /* type meaning: 0 <-> 10G, 1 <-> 1G */
    for (type = 0; type < 2; type ++) 
    {
        /**************************************************************************/
        /* create Layer 2 CL scheduler for unicast*/
        /**************************************************************************/
        flags = BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE1_4SP;;
        
        rv = bcm_cosq_gport_add(unit, pon_gport, 1, 
                    flags, &gport_ucast_scheduler_2[type]);
        
        if (rv != BCM_E_NONE) {
            printf("L2 ucast (%d) bcm_cosq_gport_add scheduler failed $rv\n", type);
            return rv;
        }
        rv = bcm_cosq_gport_sched_set(unit,
                      gport_ucast_scheduler_2[type],
                      0,
                      BCM_COSQ_SP1,
                      0);
        if (rv != BCM_E_NONE) {
            printf("L2 ucast (%d) bcm_cosq_gport_sched_set failed $rv\n", type);
            return rv;
        }
        
        rv = bcm_cosq_gport_attach(unit, gport_scheduler_1[type], 
                       gport_ucast_scheduler_2[type], 0);
        if (rv != BCM_E_NONE) {
            printf("L2 ucast (%d) bcm_cosq_gport_attach failed $rv\n", type);
            return rv;
        }

        /**************************************************************************/
        /* create Layer 3 CL scheduler for unicast*/
        /**************************************************************************/
        for (index = 0; index < 2; index++) 
        {
            flags = BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE5_1SP_WFQ | BCM_COSQ_GPORT_SCHEDULER_CLASS_WFQ_MODE_INDEPENDENT;
            
            rv = bcm_cosq_gport_add(unit, pon_gport, 1, 
                        flags, &gport_ucast_scheduler_3[type][index]);
            
            if (rv != BCM_E_NONE) {
                printf("L3 ucast (%d, %d) bcm_cosq_gport_add scheduler failed $rv\n", type, index);
                return rv;
            }
            rv = bcm_cosq_gport_sched_set(unit,
                          gport_ucast_scheduler_3[type][index],
                          0,
                          BCM_COSQ_SP0 + index,
                          0);
            if (rv != BCM_E_NONE) {
                printf("L3 ucast (%d, %d) bcm_cosq_gport_sched_set failed $rv\n", type, index);
                return rv;
            }
            
            rv = bcm_cosq_gport_attach(unit, gport_ucast_scheduler_2[type], 
                           gport_ucast_scheduler_3[type][index], 0);
            if (rv != BCM_E_NONE) {
                printf("L3 ucast (%d, %d) bcm_cosq_gport_attach failed $rv\n", type, index);
                return rv;
            }  
        }

        /**************************************************************************/
        /* create Layer 4 Composite CL scheduler for unicast*/
        /**************************************************************************/
        for (index = 0; index < 2; index++) 
        {
            flags = BCM_COSQ_GPORT_SCHEDULER | BCM_COSQ_GPORT_SCHEDULER_CLASS_MODE1_4SP | BCM_COSQ_GPORT_COMPOSITE;
            
            rv = bcm_cosq_gport_add(unit, pon_gport, 1, 
                        flags, &gport_ucast_scheduler_4[type][index]);
            
            if (rv != BCM_E_NONE) {
                printf("L4 ucast (%d, %d) bcm_cosq_gport_add scheduler failed $rv\n", type, index);
                return rv;
            }
            rv = bcm_cosq_gport_sched_set(unit,
                          gport_ucast_scheduler_4[type][index],
                          0,
                          BCM_COSQ_SP0,
                          l3_scheduler_weight_set[type][index]);
            if (rv != BCM_E_NONE) {
                printf("L4 ucast (%d, %d) bcm_cosq_gport_sched_set failed $rv\n", type, index);
                return rv;
            }
            
            rv = bcm_cosq_gport_attach(unit, gport_ucast_scheduler_3[type][0], 
                           gport_ucast_scheduler_4[type][index], 0);
            if (rv != BCM_E_NONE) {
                printf("L4 ucast (%d, %d) bcm_cosq_gport_attach failed $rv\n", type, index);
                return rv;
            }  

            BCM_COSQ_GPORT_COMPOSITE_SF2_SET(gport_ucast_scheduler_4_sf[type][index], gport_ucast_scheduler_4[type][index]); 
            rv = bcm_cosq_gport_sched_set(unit,
                          gport_ucast_scheduler_4_sf[type][index],
                          0,
                          BCM_COSQ_SP0,
                          l3_scheduler_weight_set[type][index]);
            if (rv != BCM_E_NONE) {
                printf("L4 ucast sf (%d, %d) bcm_cosq_gport_sched_set failed $rv\n", type, index);
                return rv;
            }
            
            rv = bcm_cosq_gport_attach(unit, gport_ucast_scheduler_3[type][1], 
                           gport_ucast_scheduler_4_sf[type][index], 0);
            if (rv != BCM_E_NONE) {
                printf("L4 ucast sf (%d, %d) bcm_cosq_gport_attach failed $rv\n", type, index);
                return rv;
            } 
        }

        /**************************************************************************/
        /* create voq connector for unicast*/     
        /**************************************************************************/
        for (index = 0; index < 2; index++)
        {
            flags = BCM_COSQ_GPORT_VOQ_CONNECTOR | BCM_COSQ_GPORT_WITH_ID; 
            BCM_COSQ_GPORT_VOQ_CONNECTOR_SET(gport_ucast_voq_connector_group[type][index], ucast_voq_connector_flow_id[type][index]);

            if (is_device_or_above(unit, JERICHO)) {
                config.port = pon_gport;
                config.flags = flags ;
                config.numq = num_cos ;
                config.remote_modid = my_modid ;
                config.nof_remote_cores = remote_cores;
                rv = bcm_cosq_voq_connector_gport_add(unit, &config,&gport_ucast_voq_connector_group[type][index]);
            }else{
                rv = bcm_cosq_gport_add(unit, pon_gport, num_cos, 
                            flags, &gport_ucast_voq_connector_group[type][index]);
            }

            if (rv != BCM_E_NONE) {
                printf("voq connector ucast (%d, %d) bcm_cosq_gport_add connector failed $rv\n", type, index);
                return rv;
            }  
            
            for (cosq = 0; cosq < 4; cosq++)
            {
                /* COS 0-3 */
                rv = bcm_cosq_gport_sched_set(unit,
                                  gport_ucast_voq_connector_group[type][index],
                                  cosq,
                                  BCM_COSQ_SP3 - cosq,
                                  0);
                if (rv != BCM_E_NONE) {
                    printf("voq connector ucast (%d, %d) bcm_cosq_gport_sched_set failed $rv\n", type, index);
                    return rv;
                }

                
                rv = bcm_cosq_gport_attach(unit, gport_ucast_scheduler_4[type][index], 
                                   gport_ucast_voq_connector_group[type][index], cosq);
                    if (rv != BCM_E_NONE) {
                        printf("voq connector ucast (%d, %d) bcm_cosq_gport_attach failed $rv\n", type, index);
                        return rv;
                    }
                }
            }

        /**************************************************************************/
        /* create voq for unicast*/     
        /**************************************************************************/
        for (index = 0; index < 2; index++)
        {
            flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP | BCM_COSQ_GPORT_TM_FLOW_ID | BCM_COSQ_GPORT_WITH_ID;

            /* Create by the VLAN TRANSLATION */
            BCM_GPORT_UNICAST_QUEUE_GROUP_SET(gport_ucast_voq_group[type][index], ucast_voq_queue_id[type][index]);
            rv = bcm_cosq_gport_add(unit, pon_gport, 4, 
                        flags, &gport_ucast_voq_group[type][index]);

            if (rv != BCM_E_NONE) {
                printf("voq ucast (%d, %d) bcm_cosq_gport_add queue failed $rv\n", type, index);
                return rv;
            }
        }

        /**************************************************************************/
        /* connect voq to voq connector for unicast*/     
        /**************************************************************************/
        for (index = 0; index < 2; index++)
        {
            /* connect voq to flow */
            connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS;
            connection.remote_modid = my_modid;
            connection.voq = gport_ucast_voq_group[type][index];
            connection.voq_connector = gport_ucast_voq_connector_group[type][index];
            
            rv = bcm_cosq_gport_connection_set(unit, &connection);

            if (rv != BCM_E_NONE) {
                printf("connect ucast (%d, %d) bcm_cosq_gport_connection_set ingress failed $rv\n",type, index);
                return rv;
            }

            /* connect voq to flow */
            connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
            connection.remote_modid = my_modid;
            connection.voq = gport_ucast_voq_group[type][index];
            connection.voq_connector = gport_ucast_voq_connector_group[type][index];
            
            rv = bcm_cosq_gport_connection_set(unit, &connection);
            if (rv != BCM_E_NONE) {
                printf("connect ucast (%d, %d) bcm_cosq_gport_connection_set egress failed $rv\n",type, index);
                return rv;
            }
        }
    }

    /*10G traffic is maped through default TC-mapping profile */

    /* 1G */
    type = 1;
    for (index = 0; index < 2; index++) {
        for (tc = 4; tc < 8; tc++) {
            rv = bcm_cosq_port_mapping_set(unit, gport_ucast_voq_group[type][index], tc, (tc-4));
            if (rv != BCM_E_NONE) {
                printf("bcm_cosq_port_mapping_set uc type %d, index %d, tc %d, failed $rv\n",type, index, tc);
                return rv;
            }
        }
    }

    /*
     * Set rate on the E2E Interface
     */
    kbits_sec_max = 10000000; /* 10Gbps */

    BCM_COSQ_GPORT_E2E_PORT_SET(e2e_pon_gport, pon_port);

    rv = bcm_fabric_port_get(unit,
                 e2e_pon_gport,
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
        printf("bcm_cosq_gport_bandwidth_set if failed A $rv\n");
        return rv;
    }
    
    
    /*
     * Set rate on the E2E port
     */
    kbits_sec_max = 10000000; /* 10Gbps */
    
    BCM_COSQ_GPORT_E2E_PORT_SET(e2e_pon_gport, pon_port);

    rv = bcm_cosq_gport_bandwidth_set(unit,
                      e2e_pon_gport,
                      0,
                      0,
                      kbits_sec_max,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set failed B $rv\n");
        return rv;
    }    
    printf("pon_dn_ratelimit_setup completed with status (%s)\n", bcm_errmsg(rv));

    /* 10G */
    queue = 0;    
    BCM_COSQ_GPORT_UCAST_EGRESS_QUEUE_SET(ucast_gport, pon_gport);
    for (tc = 0; tc < 4; tc++) {
        for (dp = 0; dp < 4; dp++) {
            rv = bcm_cosq_gport_egress_map_set(unit, ucast_gport, tc, dp, queue);
            if (rv != BCM_E_NONE) {
                printf("Error, in uc queue mapping, ucast_gport: 0x%x, tc: %d, dp: %d, queue %d, $rv\n", pon_gport, tc, dp, queue);
                return(rv);
            }
        }
    }

    /* 1G */
    queue = 1;    
    for (tc = 4; tc < 8; tc++) {
        for (dp = 0; dp < 4; dp++) {
            rv = bcm_cosq_gport_egress_map_set(unit, ucast_gport, tc, dp, queue);
            if (rv != BCM_E_NONE) {
                printf("Error, in mc queue mapping, ucast_gport: 0x%x, tc: %d, dp: %d, queue %d, $rv\n", pon_gport, tc, dp, queue);
                return(rv);
            }
        }
    }

    return rv;
}


/*******************************************************************************
* Downstream rate limit cleanup                                                    
 */
int pon_dn_ratelimit_cleanup(int unit)
{  
    int my_modid = 0;
    int num_cos = 4;
    int flags = 0;    
    int type;
    int index;
    int cosq; 
    int voq;
    bcm_error_t rv = BCM_E_NONE;      
    bcm_cosq_gport_connection_t connection;
    
    rv = bcm_stk_modid_get(unit, &my_modid);
    if (rv != BCM_E_NONE) {
        printf("bcm_stk_my_modid_get failed $rv\n");
        return rv;
    }       
    
    /* type meaning: 0 <-> 10G, 1 <-> 1G */
    for (type = 0; type < 2; type ++) 
    {        
        /**************************************************************************/
        /* dis-connect voq to voq connector for unicast*/     
        /**************************************************************************/
        for (index = 0; index < 2; index++)
        {
            /* dis-connect voq to flow */
            connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
            connection.remote_modid = my_modid;
            connection.voq = gport_ucast_voq_group[type][index];
            connection.voq_connector = gport_ucast_voq_connector_group[type][index];
            
            rv = bcm_cosq_gport_connection_set(unit, &connection);

            if (rv != BCM_E_NONE) {
                printf("connect ucast (%d, %d) bcm_cosq_gport_connection_set ingress failed $rv\n",type, index);
                return rv;
            }

            /* dis-connect voq to flow */
            connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS | BCM_COSQ_GPORT_CONNECTION_INVALID;
            connection.remote_modid = my_modid;
            connection.voq = gport_ucast_voq_group[type][index];
            connection.voq_connector = gport_ucast_voq_connector_group[type][index];
            
            rv = bcm_cosq_gport_connection_set(unit, &connection);
            if (rv != BCM_E_NONE) {
                printf("connect ucast (%d, %d) bcm_cosq_gport_connection_set egress failed $rv\n",type, index);
                return rv;
            }
        }
        /**************************************************************************/
        /* delete voq for unicast*/     
        /**************************************************************************/
        for (index = 0; index < 2; index++)
        {
            rv = bcm_cosq_gport_delete(unit, gport_ucast_voq_group[type][index]);
            if (rv != BCM_E_NONE) {
                printf("voq ucast (%d, %d) bcm_cosq_gport_delete queue failed $rv\n", type, index);
                return rv;
            }
        }
        /**************************************************************************/
        /* detach voq connector for unicast*/     
        /**************************************************************************/
        for (index = 0; index < 2; index++)
        {
            for (cosq = 0; cosq < 4; cosq++)
            {    
                rv = bcm_cosq_gport_detach(unit, gport_ucast_scheduler_4[type][index], 
                                   gport_ucast_voq_connector_group[type][index], cosq);
                if (rv != BCM_E_NONE) {
                    printf("voq connector ucast (%d, %d) bcm_cosq_gport_detach failed $rv\n", type, index);
                    return rv;
                }
            }
        }
        /**************************************************************************/
        /* delete voq connector for unicast*/     
        /**************************************************************************/
        for (index = 0; index < 2; index++)
        {
            rv = bcm_cosq_gport_delete(unit, gport_ucast_voq_connector_group[type][index]);
            if (rv != BCM_E_NONE) {
                printf("voq connector ucast (%d, %d) bcm_cosq_gport_delete failed $rv\n", type, index);
                return rv;
            }
        }
        /**************************************************************************/
        /* detach Layer 4 Composite CL scheduler for unicast*/
        /**************************************************************************/
        for (index = 0; index < 2; index++) 
        {    
            rv = bcm_cosq_gport_detach(unit, gport_ucast_scheduler_3[type][0], 
                           gport_ucast_scheduler_4[type][index], 0);
            if (rv != BCM_E_NONE) {
                printf("L4 ucast (%d, %d) bcm_cosq_gport_detach failed $rv\n", type, index);
                return rv;
            }              
            rv = bcm_cosq_gport_detach(unit, gport_ucast_scheduler_3[type][1], 
                           gport_ucast_scheduler_4_sf[type][index], 0);
            if (rv != BCM_E_NONE) {
                printf("L4 ucast sf (%d, %d) bcm_cosq_gport_detach failed $rv\n", type, index);
                return rv;
            } 
        }
        /**************************************************************************/
        /* delete Layer 4 Composite CL scheduler for unicast*/
        /**************************************************************************/
        for (index = 0; index < 2; index++) 
        {              
            rv = bcm_cosq_gport_delete(unit, gport_ucast_scheduler_4[type][index]);
            if (rv != BCM_E_NONE) {
                printf("L4 ucast (%d, %d) bcm_cosq_gport_delete failed $rv\n", type, index);
                return rv;
            }  
        }
        /**************************************************************************/
        /* detach Layer 3 CL scheduler for unicast*/
        /**************************************************************************/
        for (index = 0; index < 2; index++) 
        {   
            rv = bcm_cosq_gport_detach(unit, gport_ucast_scheduler_2[type], 
                           gport_ucast_scheduler_3[type][index], 0);
            if (rv != BCM_E_NONE) {
                printf("L3 ucast (%d, %d) bcm_cosq_gport_detach failed $rv\n", type, index);
                return rv;
            }  
        }
        /**************************************************************************/
        /* delete Layer 3 CL scheduler for unicast*/
        /**************************************************************************/
        for (index = 0; index < 2; index++) 
        {           
            rv = bcm_cosq_gport_delete(unit, gport_ucast_scheduler_3[type][index]);
            if (rv != BCM_E_NONE) {
                printf("L43 ucast (%d, %d) bcm_cosq_gport_delete failed $rv\n", type, index);
                return rv;
            }  
        }
        /**************************************************************************/
        /* detach Layer 2 CL scheduler for unicast*/
        /**************************************************************************/        
        rv = bcm_cosq_gport_detach(unit, gport_scheduler_1[type], 
                       gport_ucast_scheduler_2[type], 0);
        if (rv != BCM_E_NONE) {
            printf("L3 ucast (%d) bcm_cosq_gport_detach failed $rv\n", type);
            return rv;
        }

        /**************************************************************************/
        /* delete Layer 2 CL scheduler for unicast*/
        /**************************************************************************/          
        rv = bcm_cosq_gport_delete(unit, gport_ucast_scheduler_2[type]);
        if (rv != BCM_E_NONE) {
            printf("L3 ucast (%d) bcm_cosq_gport_delete failed $rv\n", type);
            return rv;
        }
    }

    /**************************************************************************/
    /* delete Layer 1 HR scheduler */
    /**************************************************************************/    
    for (type = 0; type < 2; type++) 
    {
        rv = bcm_cosq_gport_delete(unit, gport_scheduler_1[type]);
		    
        if (rv != BCM_E_NONE) {
            printf("L1 bcm_cosq_gport_delete failed $rv\n");
            return rv;
        }  
	}
    
    printf("pon_dn_ratelimit_cleanup deleted with status (%s)\n", bcm_errmsg(rv));
    return rv;
}


/*******************************************************************************
* Downstream rate limit on 10G/1G PON                                                   
 */
int pon_dn_ratelimit_pon(int unit, int tenG, int cir)
{
    bcm_error_t rv = BCM_E_NONE;          
    
    rv = bcm_cosq_gport_bandwidth_set(unit,
                      gport_ucast_scheduler_2[tenG ? 0 : 1],
                      0,
                      0,
                      cir,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set connector failed I $rv\n");
        return rv;
    }
    return rv;
}

/*******************************************************************************
* Downstream rate limit on 10G/1G PON unicast llid                                              
 */
int pon_dn_ratelimit_llid(int unit, int tenG, int index, int cir, int eir)
{
    bcm_error_t rv = BCM_E_NONE;      
    bcm_gport_t gport_scheduler;
    bcm_gport_t gport_scheduler_sf;

    gport_scheduler = gport_ucast_scheduler_4[tenG ? 0 : 1][index];
    gport_scheduler_sf = gport_ucast_scheduler_4_sf[tenG ? 0 : 1][index];
    
    rv = bcm_cosq_gport_bandwidth_set(unit,
                      gport_scheduler,
                      0,
                      0,
                      cir,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set cir failed I $rv\n");
        return rv;
    }
		
    rv = bcm_cosq_gport_bandwidth_set(unit,
	                  gport_scheduler_sf,
	                  0,
	                  0,
	                  (eir == 0) ? 1 : eir,
	                  0);
	if (rv != BCM_E_NONE) {
	        printf("bcm_cosq_gport_bandwidth_set eir failed I $rv\n");
	        return rv;
    }

    return rv;
}


/*******************************************************************************
* Downstream rate limit on 10G/1G PON unicast queue                                                  
 */
int pon_dn_ratelimit_queue(int unit, int tenG, int index, int cosq, int cir)
{
    bcm_error_t rv = BCM_E_NONE;      
    bcm_gport_t gport_scheduler;
    bcm_gport_t gport_scheduler_sf;

    gport_scheduler = gport_ucast_voq_connector_group[tenG ? 0 : 1][index];
    
    rv = bcm_cosq_gport_bandwidth_set(unit,
                      gport_scheduler,
                      cosq,
                      0,
                      cir,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set connector failed I $rv\n");
        return rv;
    }
    
    return rv;
}


/*******************************************************************************
* Downstream rate limit on 10G/1G PON AC                                                
 */
int pon_dn_ratelimit_ac(int unit, int index, int cir, int eir)
{
    bcm_error_t rv = BCM_E_NONE;      
    bcm_gport_t gport_scheduler;
    bcm_gport_t gport_scheduler_sf;

    gport_scheduler = gport_ucast_scheduler_5[tenG ? 0 : 1][index];
    gport_scheduler_sf = gport_ucast_scheduler_5_sf[tenG ? 0 : 1][index];
    
    rv = bcm_cosq_gport_bandwidth_set(unit,
                      gport_scheduler,
                      0,
                      0,
                      cir,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set connector failed I $rv\n");
        return rv;
    }
        
    rv = bcm_cosq_gport_bandwidth_set(unit,
                      gport_scheduler_sf,
                      0,
                      0,
                      eir,
                      0);
    if (rv != BCM_E_NONE) {
        printf("bcm_cosq_gport_bandwidth_set connector failed I $rv\n");
        return rv;
    }
    return rv;
}


/*******************************************************************************
*
*  Downstream rate limit PP model
 */
struct pon_service_info_s{
    int service_mode;
    int up_lif_type;
    bcm_tunnel_id_t tunnel_id;
    bcm_vlan_t up_ovlan;
    bcm_vlan_t up_ivlan;
    uint8 up_pcp;
    bcm_ethertype_t up_ethertype;
    int down_lif_type;
    bcm_vlan_t down_ovlan;
    bcm_vlan_t down_ivlan;
    bcm_gport_t pon_gport;
    bcm_gport_t nni_gport;
    int flow_id;
    bcm_gport_t flow_gport;
};

bcm_vlan_t n_1_service_uc_vsi;
int num_of_n_1_uc_services = 8;
pon_service_info_s pon_n_1_service_uc_info[8];
int mc_index[2] = { 1, 2 };
bcm_mac_t mc_mac[2] = { {0x01, 0x00, 0x5E, 0x01, 0x01, 0x01}, 
                        {0x01, 0x00, 0x5E, 0x01, 0x01, 0x02} };
int qos_map_id;
int pon_qos_map_l2_internal_pri[16] = {0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3};

                        	
/*******************************************************************************
* Downstream rate limit cos remark create                                                   
 */                    	
int pon_dn_ratelimit_cos_remark_create(int unit)
{
    int rv;
    bcm_qos_map_t l2_in_map;
    int i = 0;

    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS, &qos_map_id);

    if (rv != BCM_E_NONE) {
        printf("error in ingress PCP bcm_qos_map_create() %d $rv\n", i);
        return rv;
    }

    for (i = 0; i < 16; i++) {
        bcm_qos_map_t_init(&l2_in_map);
    
        /* Set ingress pkt_pri/cfi Priority */
        l2_in_map.pkt_pri = i>>1;
        l2_in_map.pkt_cfi = i&1;

        /* Set internal priority for this ingress pri */
        l2_in_map.int_pri = pon_qos_map_l2_internal_pri[i];    
        /* Set color for this ingress Priority  */
        l2_in_map.color = bcmColorGreen;
    
        rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L2, &l2_in_map, qos_map_id);
        if (rv != BCM_E_NONE) {
            printf("error in ingress PCP bcm_qos_map_create() %d $rv\n", i);
            return rv;
        }
    }

    return rv;
}

/*******************************************************************************
* Downstream rate limit cos remark destroy                                                  
 */   
int pon_dn_ratelimit_cos_remark_destroy(int unit)
{
    int rv;
    bcm_qos_map_t l2_in_map;
    int i = 0;

    for (i = 0; i < 16; i++) {
        bcm_qos_map_t_init(&l2_in_map);
                
        /* Set ingress pkt_pri/cfi Priority */
        l2_in_map.pkt_pri = i>>1;
        l2_in_map.pkt_cfi = i&1;
        
        rv = bcm_qos_map_delete(unit, BCM_QOS_MAP_L2, &l2_in_map, qos_map_id);
        if (rv != BCM_E_NONE) {
            printf("error in ingress bcm_qos_map_delete() %d $rv\n", i);
            return rv;
        }
    }

    rv = bcm_qos_map_destroy(unit, qos_map_id);
    if (rv != BCM_E_NONE) {
        printf("error in ingress PCP bcm_qos_map_destroy() %d $rv\n", i);
        return rv;
    }

    return rv;
}


/*******************************************************************************
* Add multicast address
 */
int pon_dn_ratelimit_mc_addr_add(int unit, bcm_mac_t mc_mac, int mc_index)
{
	bcm_l2_addr_t l2addr;
	int rv = 0;

    bcm_l2_addr_t_init(&l2addr, mc_mac, n_1_service_mc_vsi);
    l2addr.flags = BCM_L2_STATIC | BCM_L2_MCAST;
    l2addr.l2mc_index = mc_index;

    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error: bcm_l2_addr_add %d %s\n", bcm_errmsg(rv), mc_index);
        return rv;
    }

    return rv;
}

/*******************************************************************************
* Downstream rate limit PP model Init                                                   
 */
int pon_dn_ratelimit_service_Init(int unit)
{
    bcm_error_t rv = BCM_E_NONE;  
    int index;

    /* PON Port 4 Tunnel-ID 1000 CVLAN 10 <-VSI-> NNI Port 128 CVLAN 100*/    
    index = 0;
    pon_n_1_service_uc_info[index].service_mode = otag_to_otag2;
    pon_n_1_service_uc_info[index].tunnel_id  = 1000;
    pon_n_1_service_uc_info[index].up_lif_type = match_otag;
    pon_n_1_service_uc_info[index].up_ovlan   = 10;
    pon_n_1_service_uc_info[index].down_lif_type = match_otag;
    pon_n_1_service_uc_info[index].down_ovlan = 100;
    pon_n_1_service_uc_info[index].flow_id = ucast_voq_queue_id[0][0];
    pon_n_1_service_uc_info[index].flow_gport = gport_ucast_voq_group[0][0];

    /* PON Port 4 Tunnel-ID 1000 <-VSI-> NNI Port 128*/
	index++;
    pon_n_1_service_uc_info[index].service_mode = untag_to_untag;
    pon_n_1_service_uc_info[index].tunnel_id  = 1000;
    pon_n_1_service_uc_info[index].up_lif_type = match_untag;
    pon_n_1_service_uc_info[index].up_ovlan   = 0;
    pon_n_1_service_uc_info[index].down_lif_type = match_untag;
    pon_n_1_service_uc_info[index].down_ovlan = 0;
    pon_n_1_service_uc_info[index].flow_id = ucast_voq_queue_id[0][0];
    pon_n_1_service_uc_info[index].flow_gport = gport_ucast_voq_group[0][0];

    /* PON Port 4 Tunnel-ID 1001 CVLAN 11 <-VSI-> NNI Port 128 CVLAN 100*/    
    index++;
    pon_n_1_service_uc_info[index].service_mode = otag_to_otag2;
    pon_n_1_service_uc_info[index].tunnel_id  = 1001;
    pon_n_1_service_uc_info[index].up_lif_type = match_otag;
    pon_n_1_service_uc_info[index].up_ovlan   = 11;
    pon_n_1_service_uc_info[index].down_lif_type = match_otag;
    pon_n_1_service_uc_info[index].down_ovlan = 100;
    pon_n_1_service_uc_info[index].flow_id = ucast_voq_queue_id[0][1];
    pon_n_1_service_uc_info[index].flow_gport = gport_ucast_voq_group[0][1];

    /* PON Port 4 Tunnel-ID 1001 <-VSI-> NNI Port 128*/
	index++;
    pon_n_1_service_uc_info[index].service_mode = untag_to_untag;
    pon_n_1_service_uc_info[index].tunnel_id  = 1001;
    pon_n_1_service_uc_info[index].up_lif_type = match_untag;
    pon_n_1_service_uc_info[index].up_ovlan   = 0;
    pon_n_1_service_uc_info[index].down_lif_type = match_untag;
    pon_n_1_service_uc_info[index].down_ovlan = 0;
    pon_n_1_service_uc_info[index].flow_id = ucast_voq_queue_id[0][1];
    pon_n_1_service_uc_info[index].flow_gport = gport_ucast_voq_group[0][1];

   /* PON Port 4 Tunnel-ID 2000 CVLAN 20 <-VSI-> NNI Port 128 CVLAN 100*/    
    index++;
    pon_n_1_service_uc_info[index].service_mode = otag_to_otag2;
    pon_n_1_service_uc_info[index].tunnel_id  = 2000;
    pon_n_1_service_uc_info[index].up_lif_type = match_otag;
    pon_n_1_service_uc_info[index].up_ovlan   = 20;
    pon_n_1_service_uc_info[index].down_lif_type = match_otag;
    pon_n_1_service_uc_info[index].down_ovlan = 100;
    pon_n_1_service_uc_info[index].flow_id = ucast_voq_queue_id[1][0];
    pon_n_1_service_uc_info[index].flow_gport = gport_ucast_voq_group[1][0];

    /* PON Port 4 Tunnel-ID 2000 <-VSI-> NNI Port 128*/
	index++;
    pon_n_1_service_uc_info[index].service_mode = untag_to_untag;
    pon_n_1_service_uc_info[index].tunnel_id  = 2000;
    pon_n_1_service_uc_info[index].up_lif_type = match_untag;
    pon_n_1_service_uc_info[index].up_ovlan   = 0;
    pon_n_1_service_uc_info[index].down_lif_type = match_untag;
    pon_n_1_service_uc_info[index].down_ovlan = 0;
    pon_n_1_service_uc_info[index].flow_id = ucast_voq_queue_id[1][0];
    pon_n_1_service_uc_info[index].flow_gport = gport_ucast_voq_group[1][0];

    /* PON Port 4 Tunnel-ID 2001 CVLAN 21 <-VSI-> NNI Port 128 CVLAN 100*/    
    index++;
    pon_n_1_service_uc_info[index].service_mode = otag_to_otag2;
    pon_n_1_service_uc_info[index].tunnel_id  = 2001;
    pon_n_1_service_uc_info[index].up_lif_type = match_otag;
    pon_n_1_service_uc_info[index].up_ovlan   = 21;
    pon_n_1_service_uc_info[index].down_lif_type = match_otag;
    pon_n_1_service_uc_info[index].down_ovlan = 100;
    pon_n_1_service_uc_info[index].flow_id = ucast_voq_queue_id[1][1];
    pon_n_1_service_uc_info[index].flow_gport = gport_ucast_voq_group[1][1];

    /* PON Port 4 Tunnel-ID 2001 <-VSI-> NNI Port 128*/
	index++;
    pon_n_1_service_uc_info[index].service_mode = untag_to_untag;
    pon_n_1_service_uc_info[index].tunnel_id  = 2001;
    pon_n_1_service_uc_info[index].up_lif_type = match_untag;
    pon_n_1_service_uc_info[index].up_ovlan   = 0;
    pon_n_1_service_uc_info[index].down_lif_type = match_untag;
    pon_n_1_service_uc_info[index].down_ovlan = 0;
    pon_n_1_service_uc_info[index].flow_id = ucast_voq_queue_id[1][1];
    pon_n_1_service_uc_info[index].flow_gport = gport_ucast_voq_group[1][1];

    
    rv = pon_app_init(unit, pon_port, nni_port, 0, 0);
    if (rv != BCM_E_NONE) {
        printf("pon_app_init failed $rv\n");
        return rv;
    } 
    
    rv = pon_dn_ratelimit_cos_remark_create(unit);
    if (rv != BCM_E_NONE) {
        printf("pon_dn_ratelimit_cos_remark_create failed $rv\n");
        return rv;
    }
}


/*******************************************************************************
* Downstream rate limit PP model setup                                                   
 */
int pon_dn_ratelimit_service_setup(int unit)
{ 
    bcm_error_t rv = BCM_E_NONE;  
    int index;
    bcm_vswitch_cross_connect_t gports;
    int service_mode;
    bcm_tunnel_id_t tunnel_id;
    bcm_vlan_t up_ovlan;
    bcm_vlan_t up_ivlan;
    uint8 up_pcp;
    bcm_ethertype_t up_ethertype;
    bcm_vlan_t down_ovlan;
    bcm_vlan_t down_ivlan;
    bcm_if_t encap_id;
    bcm_gport_t pon_gport, nni_gport;
    int pon_lif_type, nni_lif_type;
    bcm_vlan_action_set_t action;
    int voq_queue_id;
    bcm_gport_t gport_voq_group;   
    int flow_id;
    bcm_gport_t flow_gport;

    rv = vswitch_create(unit, &n_1_service_uc_vsi);
    if (rv != BCM_E_NONE) {
        printf("uc vswitch_create failed $rv\n");
        return rv;
    }

    for (index = 0; index < num_of_n_1_uc_services; index++)    
    {   
        pon_gport = 0;
        nni_gport = 0;
        service_mode = pon_n_1_service_uc_info[index].service_mode;        
        tunnel_id    = pon_n_1_service_uc_info[index].tunnel_id;        
        pon_lif_type = pon_n_1_service_uc_info[index].up_lif_type;        
        up_ovlan     = pon_n_1_service_uc_info[index].up_ovlan;        
        up_ivlan     = pon_n_1_service_uc_info[index].up_ivlan;        
        up_pcp       = pon_n_1_service_uc_info[index].up_pcp;        
        up_ethertype = pon_n_1_service_uc_info[index].up_ethertype;        
        nni_lif_type = pon_n_1_service_uc_info[index].down_lif_type;        
        down_ovlan   = pon_n_1_service_uc_info[index].down_ovlan;        
        down_ivlan   = pon_n_1_service_uc_info[index].down_ivlan; 
        flow_id      = pon_n_1_service_uc_info[index].flow_id;
        flow_gport   = pon_n_1_service_uc_info[index].flow_gport;

        /* Create PON LIF */        
        rv = pon_lif_create(unit, 0, pon_lif_type, 1, flow_id, (tunnel_id & 0x7FF), tunnel_id, up_ovlan, up_ivlan, up_pcp, up_ethertype, &pon_gport); 
        if (rv != BCM_E_NONE) {
            printf("uc pon_lif_create failed (%d) $rv\n", index);
            return rv;
        }
        /* Add PON LIF to vswitch */        
        rv = bcm_vswitch_port_add(unit, n_1_service_uc_vsi, pon_gport);        
        if (rv != BCM_E_NONE) {
            printf("uc bcm_vswitch_port_add failed (%d) $rv\n", index);
            return rv;
        }
        /*For downstream*/
        rv = multicast_vlan_port_add(unit, n_1_service_uc_vsi, flow_gport, pon_gport);
        if (rv != BCM_E_NONE) {
            printf("uc multicast_vlan_port_add failed (%d) $rv\n", index);
            return rv;
        }
        /* Set PON LIF ingress VLAN editor */       
        rv = pon_port_ingress_vt_set(unit, service_mode, down_ovlan, down_ivlan, pon_gport, 0, NULL); 
        if (rv != BCM_E_NONE) {
            printf("uc pon_port_ingress_vt_set failed (%d) $rv\n", index);
            return rv;
        }
        /* Set PON LIF egress VLAN editor */        
        rv = pon_port_egress_vt_set(unit, service_mode, tunnel_id, up_ovlan, up_ivlan, pon_gport);   
        if (rv != BCM_E_NONE) {
            printf("uc pon_port_egress_vt_set failed (%d) $rv\n", index);
            return rv;
        }

        /*Only create one NNI LIF*/        
        if (index == 0) {                        
            /* Create NNI LIF */
            rv = nni_lif_create(unit, nni_port, match_all_tags, 0, 0, 0, 0, &nni_gport, &encap_id);
            if (rv != BCM_E_NONE) {
                printf("uc nni_lif_create failed (%d) $rv\n", index);
                return rv;
            }
            /*For upstream*/            
            rv = multicast_vlan_port_add(unit, n_1_service_uc_vsi, nni_port, nni_gport); 
            if (rv != BCM_E_NONE) {
                printf("uc multicast_vlan_port_add failed (%d) $rv\n", index);
                return rv;
            }
            /* add to vswitch */            
            rv = bcm_vswitch_port_add(unit, n_1_service_uc_vsi, nni_gport);            
            if (rv != BCM_E_NONE) {
                printf("uc bcm_vswitch_port_add failed (%d) $rv\n", index);
                return rv;
            }            
        } else {           
            nni_gport = pon_n_1_service_uc_info[0].nni_gport;        
        }

        pon_n_1_service_uc_info[index].pon_gport = pon_gport;        
        pon_n_1_service_uc_info[index].nni_gport = nni_gport;
        
        rv = bcm_qos_port_map_set(unit, nni_gport, qos_map_id, -1);
        if (rv != BCM_E_NONE) {
            printf("uc bcm_qos_port_map_set failed (%d) $rv\n", index);
            return rv;
        }
    }


    
    return rv;
}


/*******************************************************************************
* Downstream rate limit PP model cleanup                                                   
 */
int pon_dn_ratelimit_service_cleanup(int unit)
{ 
    bcm_error_t rv = BCM_E_NONE;
    int index;
    
    rv = pon_dn_ratelimit_cos_remark_destroy(unit);
    if (rv != BCM_E_NONE) {
        printf("pon_dn_ratelimit_cos_remark_destroy failed $rv\n");
        return rv;
    }

    for (index = 0; index < num_of_n_1_uc_services; index++)
    {
        rv = vswitch_delete_port(unit, num_of_n_1_uc_services, pon_n_1_service_uc_info[index].pon_gport);
        if (rv != BCM_E_NONE) {
            printf("uc vswitch_delete_port failed (%d) $rv\n", index);
            return rv;
        }
    }
    rv = vswitch_delete_port(unit, num_of_n_1_uc_services, pon_n_1_service_uc_info[0].nni_gport);
    if (rv != BCM_E_NONE) {
        printf("uc vswitch_delete_port failed (%d) $rv\n", 0);
        return rv;
    }
    
    rv = vswitch_delete(unit, n_1_service_uc_vsi);
    if (rv != BCM_E_NONE) {
        printf("uc vswitch_delete failed $rv\n");
        return rv;
    }
    
    return rv;
}

