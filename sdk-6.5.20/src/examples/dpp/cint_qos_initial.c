/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Initial qos~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/
/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 
 * File: cint_qos_initial.c
 * Purpose: An example of the initial qos application based on port

 * Explanation:
   1. Assign packets to a Traffic class and Drop precedence according to incoming packet header and incoming Port
   2. Most configuration is done in Port and VLAN modules 
   
 * Calling up sequence:
 * Set up sequence:
   1. create inlif
      - Call bcm_vlan_port_create() with following criterias:
        BCM_VLAN_PORT_MATCH_PORT, BCM_VLAN_PORT_MATCH_PORT_VLAN
   2. create outlif
      - Call bcm_vlan_port_create() with following criteria:
        BCM_VLAN_PORT_MATCH_PORT_VLAN
   3. set the outlif egress action
      - Call bcm_vlan_translate_egress_action_add()
   4. cross connect the 2 LIFs
      - Call bcm_vswitch_cross_connect_add()
 * Clean up sequence:
   1. Delete the cross connected LIFs
      - Call bcm_vswitch_cross_connect_delete()
   2. Delete inLIF
      - Call bcm_vlan_port_destroy()
   3. Delete outLIF
      - Call bcm_vlan_port_destroy()
      
 * Service Model:
   Port 15   <----------------------Cross Connect---------------------> Port 16
   untagg    <--------------------------------------------------------> vid 200
   vid 100   <--------------------------------------------------------> vid 300
   
 * QoS Map Setting:
   qos_dscp_map_create: Map both TC and DP from the packet TOS
                        including two modes: BCM_PORT_DSCP_MAP_ALL and BCM_PORT_DSCP_MAP_UNTAGGED_ONLY
                        1. BCM_PORT_DSCP_MAP_ALL: targeted for any type of ip packet (tagged ip packet and untagged ip packet)
                           - call bcm_port_dscp_map_mode_set to set the map mode as BCM_PORT_DSCP_MAP_ALL
                           - call bcm_port_dscp_map_set to map both TC and DP
                        2. BCM_PORT_DSCP_MAP_UNTAGGED_ONLY: only targeted for untagged ip packet
                           - call bcm_port_dscp_map_mode_set to set the map mode as BCM_PORT_DSCP_MAP_UNTAGGED_ONLY
                           - call bcm_port_dscp_map_set to map both TC and DP                    
   qos_pcp_map_create:  Map TC and DP from the packet PCP (targeted for tagged packet)
                        including three modes:
                        1. map dei without cfi for s tagged pkt when use_de=0
                           - call bcm_switch_control_port_set(unit, port, bcmSwitchColorSelect, BCM_COLOR_PRIORITY) to set the mode as use_de=0
                           - call bcm_port_vlan_priority_map_set to map both TC and DP
                        2. map dei with cfi for s tagged pkt when use_de=1
                           - call bcm_switch_control_port_set(0,port,bcmSwitchColorSelect,BCM_COLOR_OUTER_CFI) to set the map mode as use_de=1
                           - call bcm_port_vlan_priority_map_set to map TC
                           - call bcm_port_cfi_color_set to map DP
                        3. map up to dp for c tagged pkt
                           - call bcm_port_priority_color_set to map DP
                           - call bcm_port_vlan_priority_map_set to map TC
   qos_dft_map_create. Map TC and DP with the default value by the parameter input
                        1. Default TC
                           - call bcm_port_dscp_map_mode_set to set the mode as BCM_PORT_DSCP_MAP_DEFAULT
                           - call bcm_port_untagged_priority_set to map TC for the untagged packet
                        2. Default DP
                           - call bcm_port_control_set(unit, port, bcmPortControlDropPrecedence, dft_color) to set the map mode and map DP for the untagged packet
  
*  Map ingress and egress as follows 
*  Map TOS                                                    
*  Ingress TOS/TC           INTERNAL           Egress    
    *     IPV4/6            TC/DP              PKT_PRI/CFI
    *     0-31              0/0(1)             0/0(1)
    *     32-63             1/0(1)             1/0(1)
    *     64-95             2/0(1)             2/0(1)
    *     96-127            3/0(1)             3/0(1)
    *     128-159           4/0(1)             4/0(1)
    *     160-191           5/0(1)             5/0(1)
    *     192-223           6/0(1)             6/0(1)
    *     224-255           7/0(1)             7/0(1)

*  Map tc from PCP and dp with (and without) dei for s tagged pkt 
*  Ingress      INTERNAL              Egress 
*  PKT_PRI/CFI   TC/DP                PKT_PRI/CFI
*    0/0         1/0(1)               1/0(1)
*    0/1         1/1(0)               1/1(0)
*    1/0         2/0(1)               2/0(1)
*    1/1         2/1(0)               2/1(0)
*    2/0         3/0(1)               3/0(1)
*    2/1         3/1(0)               3/1(0)
*    3/0         4/0(1)               4/0(1)
*    3/1         4/1(0)               4/1(0)
*    4/0         5/0(1)               5/0(1)
*    4/1         5/1(0)               5/1(0)
*    5/0         6/0(1)               6/0(1)
*    5/1         6/1(0)               6/1(0)
*    6/0         7/0(1)               7/0(1)
*    6/1         7/1(0)               7/1(0)
*    7/0         7/0(1)               7/0(1)
*    7/1         7/1(0)               7/1(0)
    
*  Map dft tc and dp for the untagged packet: tc and dp can be set as the inputted parameter
*  Ingress                  INTERNAL                Egress 
*  PKT_PRI/CFI               TC/DP               PKT_PRI/CFI
*    N/A                       0/0                    0/0
*    N/A                       0/1                    0/1
*    N/A                       1/0                    1/0
*    N/A                       1/1                    1/1
*    N/A                       2/0                    2/0
*    N/A                       2/1                    2/1
*    N/A                       3/0                    3/0
*    N/A                       3/1                    3/1
*    N/A                       4/0                    4/0
*    N/A                       4/1                    4/1
*    N/A                       5/0                    5/0
*    N/A                       5/1                    5/1
*    N/A                       6/0                    6/0
*    N/A                       6/1                    6/1
*    N/A                       7/0                    7/0
*    N/A                       7/1                    7/1

*  To Activate Above Settings Run:
   BCM> cd ../../../src/examples/dpp
   BCM> cint cint_qos_initial.c
   BCM> cint
   cint> initial_qos_service_init(unit,port_in,port_out);
   cint> initial_qos_service(unit);
   cint> qos_dscp_map_create(unit,port,color_idx,BCM_PORT_DSCP_MAP_ALL);
         *  color_idx can be 0 or 1. 0 stands for green pakcet, 1 stands for yellow packet
         *  Send Traffic from port 15 and receive it on port 16:
         *  1. In port 15 tos 0   <-----CrossConnect----->  Out port 16 vid 200 prio 0 cfi $color_idx tos 0
         *  2. In port 15 tos 64  <-----CrossConnect----->  Out port 16 vid 200 prio 2 cfi $color_idx tos 64
         *  3. In port 15 tos 128 <-----CrossConnect----->  Out port 16 vid 200 prio 4 cfi $color_idx tos 128
         *  4. In port 15 tos 192 <-----CrossConnect----->  Out port 16 vid 200 prio 6 cfi $color_idx tos 192
         *  5. In port 15 vid 100 tos 0   <-CrossConnect->  Out port 16 vid 300 prio 0 tos 0
         *  6. In port 15 vid 100 tos 64  <-CrossConnect->  Out port 16 vid 300 prio 2 tos 64
         *  7. In port 15 vid 100 tos 128 <-CrossConnect->  Out port 16 vid 300 prio 4 tos 128
         *  8. In port 15 vid 100 tos 192 <-CrossConnect->  Out port 16 vid 300 prio 6 tos 192      
   cint> qos_dscp_map_create(unit,port,color_idx,BCM_PORT_DSCP_MAP_UNTAGGED_ONLY); 
         *  color_idx can be 0 or 1. 0 stands for green pakcet, 1 stands for yellow packet
         *  Send Traffic from port 15 and receive it on port 16:
         *  1. In port 15 tos 0   <-----CrossConnect----->  Out port 16 vid 200 prio 0 cfi $color_idx tos 0
         *  2. In port 15 tos 64  <-----CrossConnect----->  Out port 16 vid 200 prio 2 cfi $color_idx tos 64
         *  3. In port 15 tos 128 <-----CrossConnect----->  Out port 16 vid 200 prio 4 cfi $color_idx tos 128
         *  4. In port 15 tos 192 <-----CrossConnect----->  Out port 16 vid 200 prio 6 cfi $color_idx tos 192
   cint> qos_pcp_map_create(unit,port_in,use_de);
         *  use_de can be 0 or 1. 0 stands for not mapping dei without cfi; 1 stands for mapping dei with cfi.
         *  Send Traffic from port 15 and receive it on port 16 when use_de is 0:
         *  1. In port 15 vid 100 prio 0 cfi 0  <-----CrossConnect----->  Out port 16 vid 300 prio 1 cfi 1
         *  2. In port 15 vid 100 prio 1 cfi 1  <-----CrossConnect----->  Out port 16 vid 300 prio 2 cfi 0
         *  3. In port 15 vid 100 prio 2 cfi 0  <-----CrossConnect----->  Out port 16 vid 300 prio 3 cfi 1
         *  4. In port 15 vid 100 prio 3 cfi 1  <-----CrossConnect----->  Out port 16 vid 300 prio 4 cfi 0
         *  5. In port 15 vid 100 prio 4 cfi 0  <-----CrossConnect----->  Out port 16 vid 300 prio 5 cfi 1
         *  6. In port 15 vid 100 prio 5 cfi 1  <-----CrossConnect----->  Out port 16 vid 300 prio 6 cfi 0
         *  7. In port 15 vid 100 prio 6 cfi 0  <-----CrossConnect----->  Out port 16 vid 300 prio 7 cfi 1
         *  8. In port 15 vid 100 prio 7 cfi 1  <-----CrossConnect----->  Out port 16 vid 300 prio 7 cfi 0
         *  Send Traffic from port 15 and receive it on port 16 when use_de is 1:
         *  1. In port 15 vid 100 prio 0 cfi 0  <-----CrossConnect----->  Out port 16 vid 300 prio 1 cfi 0
         *  2. In port 15 vid 100 prio 1 cfi 1  <-----CrossConnect----->  Out port 16 vid 300 prio 2 cfi 1
         *  3. In port 15 vid 100 prio 2 cfi 0  <-----CrossConnect----->  Out port 16 vid 300 prio 3 cfi 0
         *  4. In port 15 vid 100 prio 3 cfi 1  <-----CrossConnect----->  Out port 16 vid 300 prio 4 cfi 1
         *  5. In port 15 vid 100 prio 4 cfi 0  <-----CrossConnect----->  Out port 16 vid 300 prio 5 cfi 0
         *  6. In port 15 vid 100 prio 5 cfi 1  <-----CrossConnect----->  Out port 16 vid 300 prio 6 cfi 1
         *  7. In port 15 vid 100 prio 6 cfi 0  <-----CrossConnect----->  Out port 16 vid 300 prio 7 cfi 0
         *  8. In port 15 vid 100 prio 7 cfi 1  <-----CrossConnect----->  Out port 16 vid 300 prio 7 cfi 1
   cint> qos_dft_map_create(unit,port,dft_color,dft_tc);
         *  dft_color can be from 0 to 1 and dft_tc can be from 0 to 7.
         *  Send Traffic from port 15 and receive it on port 16:
         *  1. In port 15  <-----CrossConnect----->  Out port 16 vid 200 prio $dft_tc cfi $dft_color
 */
    
bcm_vlan_t up_ovlan = 100;
bcm_vlan_t down_ovlan1 = 200;
bcm_vlan_t down_ovlan2 = 300;
bcm_vlan_port_t in_vlan_port1, in_vlan_port2, out_vlan_port1, out_vlan_port2;
bcm_vswitch_cross_connect_t gports_untag, gports_tag;
int qos_eg_map_id_dft[2];
/* 
  * 0x2000: stands for the secondary table.
  * 0x400-0x407 stands for the green packet from TC 0 to TC 7, 
  * 0x200-0x207 stands for yellow packet from TC 0 to TC 7. 
  */
int prio[2][2][8] = {{{0x400,0x401,0x402,0x403,0x404,0x405,0x406,0x407},
                    {0x200,0x201,0x202,0x203,0x204,0x205,0x206,0x207}},
                    {{0x2401,0x2403,0x2405,0x2407,0x2402,0x2404,0x2406,0x2400},
                    {0x2201,0x2203,0x2205,0x2207,0x2202,0x2204,0x2206,0x2200}}};
int pkt_pri[8] = {0,1,2,3,4,5,6,7};
int pkt_cfi[8] = {0,1,0,1,0,1,0,1};
int internal_pri[8] = {1,2,3,4,5,6,7,7};
int internal_color_stag_with_use_de[8] = {0,1,0,1,0,1,0,1};
int internal_color_stag_with_no_use_de[8] = {1,0,1,0,1,0,1,0};
int internal_color_ctag[8] = {0,0,0,0,1,1,1,1};           
         
enum service_type_e {
    match_untag,
    match_otag
};
 
enum use_de_type_e {
    with_no_use_de,
    with_use_de
};
 
/* Map TC and DP from the packet TOS */
int qos_dscp_map_create(int unit, int color, int mode) { 
    int rv = BCM_E_NONE;
    int idx;
    int tbl_idx;
    int srccp[256];
    int mapcp[256];

    /* map tos from 0 to 255 */
    for (idx = 0; idx < 256; idx++) {
        srccp[idx] = idx;
        mapcp[idx] = idx;
    }
    
    /* set two different mapping modes from tos 
       BCM_PORT_DSCP_MAP_ALL for tagged and untagged ip packet and BCM_PORT_DSCP_MAP_UNTAGGED_ONLY only for untagged ip packet */
    rv = bcm_port_dscp_map_mode_set(unit, -1, mode);    /* global configuration, port has to be -1 */
    if (rv != BCM_E_NONE) {
        printf("error in bcm_port_dscp_map_mode_set() $rv\n");
        return rv;
    }        
    
    for (tbl_idx = 0; tbl_idx < 2; tbl_idx++) {
        /* map tc and dp from tos, 0 stands for greeen packet, 1 stands for yellow packet */
        for (idx = 0; idx < 256; idx++) {
            rv = bcm_port_dscp_map_set(unit, -1, srccp[idx], mapcp[idx], prio[tbl_idx][color][idx/32]);  /* global configuration, port has to be -1 */
            if (rv != BCM_E_NONE) {
                printf("error in bcm_port_dscp_map_set() $rv\n");
                return rv;
            }
        }
    }

    return rv;
}

/* Map port to profile */
int qos_port_dscp_map_create(int unit, int port, int profile) { 
    int rv = BCM_E_NONE;
    int prio;
    
    if ((port == -1) || (profile > 1)) {
        printf("error in qos_port_dscp_map_create() parameter\n");
        return rv;
    }    
    
    if (profile == 1) {
        prio = BCM_PRIO_SECONDARY;
    }  

    rv = bcm_port_dscp_map_set(unit, port, 0, 0, prio);  /* do the port profile mapping */
    if (rv != BCM_E_NONE) {
        printf("error in bcm_port_dscp_map_set() $rv\n");
        return rv;
    }

    return rv;
}


/* Map TC and DP from the packet PCP */ 
int qos_pcp_map_create(int unit, int port, int is_use_de) {
    int cfi = 0;
    int color = 0;
    int mode = 0;
    int rv = BCM_E_NONE;
    int idx;
    
    /* set the mode of mapping pcp */
    mode = BCM_PORT_DSCP_MAP_NONE;
    rv = bcm_port_dscp_map_mode_set(unit, -1, mode);    /* global configuration, port has to be -1 */
    if (rv != BCM_E_NONE) {
        printf("error in bcm_port_dscp_map_mode_set() $rv\n");
        return rv;
    }

    switch (is_use_de) {
        case with_no_use_de:
            /* set the mode of mapping dei without cfi for s tagged pkt */
            rv = bcm_switch_control_port_set(unit, port, bcmSwitchColorSelect, BCM_COLOR_PRIORITY);
            if (rv != BCM_E_NONE) {
                printf("error in bcm_switch_control_port_set() $rv\n");
                return rv;
            }
            
            /* set the mapping of TC and DP for tagged packet */
            for (idx = 0; idx < 8; idx++) {               
                /* map tc and dp from PCP for tagged pkt */
                rv = bcm_port_vlan_priority_map_set(unit, port, pkt_pri[idx], 0, internal_pri[idx], internal_color_stag_with_no_use_de[idx]);   /* cfi must be 0 because it isn't used */
                if (rv != BCM_E_NONE) {
                    printf("error in bcm_port_vlan_priority_map_set() $rv\n");
                    return rv;
                }
            }
            break;
        case with_use_de:
            /* set the mapping of TC for tagged packet */
            for (idx = 0; idx < 8; idx++) {              
                /* map tc from PCP for tagged packet*/
                rv = bcm_port_vlan_priority_map_set(unit, port, pkt_pri[idx], 0, internal_pri[idx], color); /* cfi must be 0 because it isn't used */
                if (rv != BCM_E_NONE) {
                    printf("error in bcm_port_vlan_priority_map_set() $rv\n");
                    return rv;
                }
                
                /* set the mode of mapping dei with cfi */
                rv = bcm_switch_control_port_set(unit,port,bcmSwitchColorSelect,BCM_COLOR_OUTER_CFI);
                if (rv != BCM_E_NONE) {
                    printf("error in bcm_switch_control_port_set() $rv\n");
                    return rv;
                }
                
                /* map dei with cfi for s tagged pkt */
                rv = bcm_port_cfi_color_set(unit, -1, pkt_cfi[idx], internal_color_stag_with_use_de[idx]);  /* global configuration, port has to be -1 */
                if (rv != BCM_E_NONE) {
                    printf("error in bcm_port_cfi_color_set() $rv\n");
                    return rv;
                }
            }
            break;
        default:
            return BCM_E_PARAM;    
    }
                
    /* map up to dp for c tagged pkt */
    for (idx = 0x0; idx < 0x8; idx++) {
        rv = bcm_port_priority_color_set(unit, port, pkt_pri[idx], internal_color_ctag[idx]);
        if (rv != BCM_E_NONE) {
            printf("error in bcm_port_priority_color_set() $rv\n");
            return rv;
        }
    }
    
    return rv;
} 

/* Map TC and DP with the default value from the inputted parameter */
int qos_dft_map_create(int unit, int port, int dft_color, int dft_tc) {
    int rv = BCM_E_NONE;
    int mode = 0;
    
    /* set the default mode of mapping tc */
    mode = BCM_PORT_DSCP_MAP_DEFAULT;    
    rv = bcm_port_dscp_map_mode_set(unit, port, mode);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_port_dscp_map_mode_set() $rv\n");
        return rv;
    }
    
    /* set the default tc */
    rv = bcm_port_untagged_priority_set(unit, port, dft_tc);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_port_untagged_priority_set() $rv\n");
        return rv;
    }
    
    /* set default dp */
    rv = bcm_port_control_set(unit, -1, bcmPortControlDropPrecedence, dft_color);   /* global configuration, port has to be -1 */
    if (rv != BCM_E_NONE) {
        printf("error in bcm_port_control_set() $rv\n");
        return rv;
    }
    
    return rv;
}
/* 
 * Create default l2 egress mapping profile.
 * When adding or modifying a new VLAN tag, cos of the new tag is based on the default mapping profile.
 */
int qos_map_l2_eg_dft_profile(int unit, int service_type)
{
    bcm_qos_map_t l2_eg_map;
    int flags = 0;
    int idx = 0;
    int rv = BCM_E_NONE;

    /* Clear structure */
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS|BCM_QOS_MAP_L2_VLAN_PCP, &qos_eg_map_id_dft[service_type]);

    if (rv != BCM_E_NONE) {
        printf("error in egress PCP bcm_qos_map_create() $rv\n");
        return rv;
    }

    for (idx=0; idx<16; idx++) {  
        bcm_qos_map_t_init(&l2_eg_map);

        /* Set ingress pkt_pri/cfi Priority */
        l2_eg_map.pkt_pri = idx>>1;
        l2_eg_map.pkt_cfi = idx&1;
        
        /* Set internal priority for this ingress pri */
        l2_eg_map.int_pri = idx>>1;
        
        /* Set color for this ingress Priority 0:bcmColorGreen 1:bcmColorYellow */
        l2_eg_map.color = idx&1;
        
        if (service_type == match_untag) {
            flags = BCM_QOS_MAP_L2|BCM_QOS_MAP_L2_VLAN_PCP|BCM_QOS_MAP_L2_UNTAGGED|BCM_QOS_MAP_EGRESS;        
        } else if (service_type == match_otag) {
            flags = BCM_QOS_MAP_L2|BCM_QOS_MAP_L2_VLAN_PCP|BCM_QOS_MAP_L2_OUTER_TAG|BCM_QOS_MAP_EGRESS;
        }
        
        rv = bcm_qos_map_add(unit, flags, &l2_eg_map, qos_eg_map_id_dft[service_type]);    
        if (rv != BCM_E_NONE) {
            printf("error in PCP egress bcm_qos_map_add() $rv\n");
            return rv;
        }  
    }
    
  return rv;
}

/* edit the eve action, map internal tc and dp to pkt_pri and pkt_cfi */
int initial_qos_service_vlan_action_set(int unit, int service_type)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t *vlan_port = NULL;
    bcm_vlan_action_set_t action;
    bcm_vlan_action_set_t_init(&action);
    
    switch (service_type)
    {
        case match_untag:
            /* eve action for untagged service */
            action.ut_outer = bcmVlanActionAdd;
            action.new_outer_vlan = down_ovlan1;                
            action.ut_outer_pkt_prio = bcmVlanActionAdd;
            action.priority = qos_eg_map_id_dft[service_type];
            vlan_port = &out_vlan_port1;
            break;
        case match_otag:
            /* eve action for tagged service */
            action.ot_outer = bcmVlanActionReplace; 
            action.new_outer_vlan = down_ovlan2;
            action.ot_outer_pkt_prio = bcmVlanActionReplace; /* priority is set according to pcp_vlan_profile mapping */
            action.priority = qos_eg_map_id_dft[match_untag];
            vlan_port = &out_vlan_port2;
            break;
        default:
            return BCM_E_PARAM;
    }

    rv = bcm_vlan_translate_egress_action_add(unit, vlan_port->vlan_port_id, BCM_VLAN_NONE, BCM_VLAN_NONE, &action);
    if (rv != BCM_E_NONE) {
        printf("error in eve translation bcm_vlan_translate_egress_action_add() $rv\n");
        return rv;
    }

    return rv;
}

/* initialize the vlan port and set the default egress qos profile
   service model:  untag pkt -> vid 200
                   vid 100 -> vid 300 
*/
int initial_qos_service_init(int unit, bcm_port_t port_in, bcm_port_t port_out) 
{
    /* initialize the vlan ports */
    int rv = BCM_E_NONE;
    bcm_vlan_port_t_init(&in_vlan_port1);
    in_vlan_port1.criteria = BCM_VLAN_PORT_MATCH_PORT;
    in_vlan_port1.port = port_in;
    in_vlan_port1.flags = (BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE);
    
    bcm_vlan_port_t_init(&out_vlan_port1);
    out_vlan_port1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    out_vlan_port1.match_vlan = down_ovlan1;
    out_vlan_port1.egress_vlan = down_ovlan1;
    out_vlan_port1.port = port_out;
    
    bcm_vlan_port_t_init(&in_vlan_port2);
    in_vlan_port2.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    in_vlan_port2.match_vlan = up_ovlan;
    in_vlan_port2.egress_vlan = up_ovlan;
    in_vlan_port2.port = port_in;
    in_vlan_port2.flags = (BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE);
    
    bcm_vlan_port_t_init(&out_vlan_port2);
    out_vlan_port2.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    out_vlan_port2.match_vlan = down_ovlan2;
    out_vlan_port2.egress_vlan = down_ovlan2;
    out_vlan_port2.port = port_out;
    
    /* set port VLAN domain */
    rv = bcm_port_class_set(unit, port_in, bcmPortClassId, port_in);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_port_class_set() $rv\n");
        return rv;
    }
    
    rv = bcm_port_class_set(unit, port_out, bcmPortClassId, port_out);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_port_class_set() $rv\n");
        return rv;
    }

    /* create default egress qos profile */
    qos_map_l2_eg_dft_profile(unit, match_untag);
    qos_map_l2_eg_dft_profile(unit, match_otag);
    
    return rv;
}

/*
* Set up tagged and untagged sercies, using port cross connect. 
*/ 
int initial_qos_service(int unit)
{
    int rv = BCM_E_NONE;
    bcm_gport_t in_gport1, in_gport2, out_gport1, out_gport2;
    in_gport1 = 0;
    in_gport2 = 0;
    out_gport1 = 0;    
    out_gport2 = 0;

    /* create inLIF for untagged service */
    rv = bcm_vlan_port_create(unit, &in_vlan_port1);
    if (rv != BCM_E_NONE) {
        printf("lif_create failed! %s\n", bcm_errmsg(rv));
        return rv;
    }
    in_gport1 = in_vlan_port1.vlan_port_id;
    printf("%d\n", in_gport1);
    
    /* create inLIF for tagged service */
    rv = bcm_vlan_port_create(unit, &in_vlan_port2);
    if (rv != BCM_E_NONE) {
        printf("lif_create failed! %s\n", bcm_errmsg(rv));
        return rv;
    }
    in_gport2 = in_vlan_port2.vlan_port_id;
    printf("%d\n", in_gport2);
    
    /* create outLIF for untagged service */
    rv = bcm_vlan_port_create(unit, &out_vlan_port1);
    if (rv != BCM_E_NONE) {
        printf("lif_create failed! %s\n", bcm_errmsg(rv));
        return rv;
    }
    out_gport1 = out_vlan_port1.vlan_port_id;
    printf("%d\n", out_gport1);
    
    /* create outLIF for tagged service */
    rv = bcm_vlan_port_create(unit, &out_vlan_port2);
    if (rv != BCM_E_NONE) {
        printf("lif_create failed! %s\n", bcm_errmsg(rv));
        return rv;
    }
    out_gport2 = out_vlan_port2.vlan_port_id;
    printf("%d\n", out_gport2);
    
    /* add eve action for untagged service */
    initial_qos_service_vlan_action_set(unit, match_untag);
    bcm_vswitch_cross_connect_t_init(&gports_untag);

    gports_untag.port1 = in_gport1;
    gports_untag.port2 = out_gport1;
    
    /* cross connect two lifs for untagged service */
    rv = bcm_vswitch_cross_connect_add(unit, &gports_untag);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_vswitch_cross_connect_add() $rv\n");
        return rv;
    }
    
    /* add eve action for tagged service */
    initial_qos_service_vlan_action_set(unit, match_otag);

    bcm_vswitch_cross_connect_t_init(&gports_tag);
    gports_tag.port1 = in_gport2;
    gports_tag.port2 = out_gport2;
    
    /* cross connect two lifs for tagged service */
    rv = bcm_vswitch_cross_connect_add(unit, &gports_tag);
    if (rv != BCM_E_NONE) {
        printf("error in bcm_vswitch_cross_connect_add() $rv\n");
        return rv;
    }
    
    return rv;
}

/*
* clean tagged and untagged sercies 
*/
int initial_qos_service_cleanup(int unit)
{
    int rv = BCM_E_NONE;
    gports_untag.port1 = in_vlan_port1.vlan_port_id;
    gports_untag.port2 = out_vlan_port1.vlan_port_id;
    gports_tag.port1 = in_vlan_port2.vlan_port_id;
    gports_tag.port2 = out_vlan_port2.vlan_port_id;
     
    /* Delete the cross connected LIFs for untagged service */
    rv = bcm_vswitch_cross_connect_delete(unit, &gports_untag);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_delete() $rv\n");
        return rv;
    }
    
    /* Delete the cross connected LIFs for tagged service */
    rv = bcm_vswitch_cross_connect_delete(unit, &gports_tag);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_delete() $rv\n");
        return rv;
    }

    /* Delete inLIF for untagged service */
    rv = bcm_vlan_port_destroy(unit, in_vlan_port1.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_destroy() $rv\n");
        return rv;
    }
    
    /* Delete outLIF for untagged service */
    rv = bcm_vlan_port_destroy(unit, out_vlan_port1.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_destroy() $rv\n");
        return rv;
    }
    
    /* Delete inLIF for tagged service */
    rv = bcm_vlan_port_destroy(unit, in_vlan_port2.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_destroy() $rv\n");
        return rv;
    }
    
    /* Delete outLIF for tagged service*/
    rv = bcm_vlan_port_destroy(unit, out_vlan_port2.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_destroy() $rv\n");
        return rv;
    }
    
    /* destroy the qos profile */
    bcm_qos_map_destroy(unit, qos_eg_map_id_dft[match_untag]);
    bcm_qos_map_destroy(unit, qos_eg_map_id_dft[match_otag]);
    
    return rv;        
}


