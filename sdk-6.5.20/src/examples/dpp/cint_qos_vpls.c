/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *
 * Cint VPLS QOS Setup example code
 *
 * 
 * This CINT example try to provide a specific examples of VPLS QOS application:
 *  1. In case of VPLS termination, map EXP to PCP Ethernet header.      
 *      a. Have In-PWE COS-profile settings TC = MPLS.EXP
 *      b. Set EVE to take TC to PCP.
 *  2. In case of VPLS encapsulation, map PCP to EXP
 *      a. At the ingress: Map PCP -> Ingress TC
 *      b. At the egress: Out-DSCP-EXP = TC (by HW)
 *      c. Set PWE.Encapsulation = Out-DSCP-EXP (uniform)
 * 
 * In the specific example we map PCP 1:1 EXP values 
 * Note: Application works from ARAD-B and above. 
 * 
 * copy to /usr/local/sbin location, run bcm.user
 * Run script:
 * 
 * cint /src/examples/sand/utility/cint_sand_utils_global.c
 * cint /src/examples/dpp/utility/cint_utils_multicast.c
 * cint /src/examples/dpp/utility/cint_utils_vlan.c
 * cint /src/examples/dpp/utility/cint_utils_l3.c
 * cint /src/examples/sand/utility/cint_sand_utils_mpls.c
 * cint /src/examples/dpp/utility/cint_utils_mpls_port.c
 * cint /src/examples/dpp/cint_port_tpid.c
 * cint /src/examples/dpp/cint_advanced_vlan_translation_mode.c
 * cint /src/examples/dpp/cint_vswitch_metro_mp.c
 * cint /src/examples/dpp/cint_multi_device_utils.c
 * cint /src/examples/dpp/cint_vswitch_cross_connect_p2p_multi_device.c
 * cint /src/examples/dpp/cint_qos_vpls.c
 
 * c
 * int nof_units = 1;
 * int units[2] = {0, 0}; 
 * int sysport1, sysport2;
 * print port_to_system_port(units[0], 13, &sysport1);
 * print port_to_system_port(units[0], 14, &sysport2);
 * qos_cross_connect_enable(units[0]);
 * qos_map_vpls(units[0]);
 * run(units,1,sysport1,2,0,0,sysport2,1,0,0,0); 
 * bcm_qos_port_map_set(units[0], cross_connect_info.vlan_port_2, qos_map_id_vpls_ingress_get(units[0]), qos_map_id_vpls_pcp_egress_get(units[0]));
 * bcm_qos_port_map_set(units[0], cross_connect_info.mpls_port_1.mpls_port_id, qos_map_id_vpls_ingress_get(units[0]), -1);
 *
 * For Uniform configuration set:
 * cross_connect_info.is_uniform=1;
 * 
 * Traffic example:
 *  Send AC to PWE:
 *  Ethernet DA1 , SA1 , VLAN: TPID 0x8100 , VID 100, PCP X
 *  Expect:
 *  Ethernet DA1 , SA1
 *  PWE LABEL 40 EXP X
 *  MPLS LABEL 20, EXP 2
 *  MPLS LABEL 40, EXP 4
 *  Ethernet DA 00:00:00:00:00:22, SA 00:00:00:00:00:11
 *
 *  Send PWE to AC:
 *  Ethernet DA 00:00:00:00:00:11 SA2, VID 10
 *  PWE LABEL 20 EXP X
 *  Ethernet DA1, SA1, VID 10
 *
 *  tx 1 psrc=201 data=0x0000000000110000201f82478100000a8847003e744000014740000007fc595b0000ef6dab158100000a0899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *  Expect:
 *  Ethernet DA1, SA1, VID 1000, PCP X
 *  0x000007fc595b0000ef6dab15810043e80899c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *  In case of Uniform model expect MPLS EXP on the packet.
 */ 

static int vpls_in_map_id; /* QOS COS-profile for PWE-LIF and AC-LIF */
static int vpls_pcp_eg_map_id; /* QOS PCP Edit profile for EVE */
static int vpls_remark_eg_map_id; /* QOS Remark profile for PCP->EXP */

/* 
 * Map Ingress COS-profile to do EXP-> TC and PCP -> TC. 
 */
int
qos_map_vpls_ingress_profile(int unit)
{
    bcm_qos_map_t vpls_in_map;
    int flags = 0;
    int exp, option, pcp, cfi;
    int rv = BCM_E_NONE;

    /* Clear structure */

    bcm_qos_map_t_init(&vpls_in_map);
    
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS, &vpls_in_map_id);

    if (rv != BCM_E_NONE) {
       printf("error in ingress vpls bcm_qos_map_create()\n");
       printf("rv is: $rv \n");
       printf("(%s)\n", bcm_errmsg(rv));
       return rv;
    }

    /* EXP In-PWE */
    for (exp=0; exp<8; exp++) {

      flags = BCM_QOS_MAP_MPLS;
      
      /* Set ingress EXP */
      vpls_in_map.exp = exp; /* EXP Input */
      
      /* Set internal priority for this ingress DSCP  */
      vpls_in_map.int_pri = exp; /* TC Output */

      /* Set color for this ingress EXP  */
      vpls_in_map.color = 0; /* DP Color Output */

      /* Set In-DSCP-EXP = EXP */
      vpls_in_map.remark_int_pri = exp; /* In-DSCP-EXP Output */

      rv = bcm_qos_map_add(unit, flags, &vpls_in_map, vpls_in_map_id);

      if (rv != BCM_E_NONE) {
        printf("error in L3 EXP ingress bcm_qos_map_add()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
        return rv;
      }
    }

    /* PCP In-AC */
    for (pcp=0; pcp<8; pcp++) {
      for (option = 0; option < 2; option++) {
        if (option == 0) {
          flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_OUTER_TAG;
        } else {
          flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_INNER_TAG;
        }

        for (cfi=0; cfi<2; cfi++) {  
          /* Set ingress PCP */
          vpls_in_map.pkt_pri = pcp; /* PCP Input */
          vpls_in_map.pkt_cfi = cfi; /* Assuming CFI 0 Input */

          /* Set internal priority (TC) for this ingress PCP  */
          vpls_in_map.int_pri = pcp; /* TC Output */

          /* Set color for this ingress PCP  */
          vpls_in_map.color = cfi; /* DP Color Output */

          rv = bcm_qos_map_add(unit, flags, &vpls_in_map, vpls_in_map_id);

          if (rv != BCM_E_NONE) {
            printf("error in L2 PCP ingress bcm_qos_map_add()\n");
            printf("rv is: $rv \n");
            printf("(%s)\n", bcm_errmsg(rv));
            return rv;
          }
        }
      }
    }
    return rv;
}

/* Map Egress PCP-DEI-Profile to do TC -> PCP */
int
qos_map_vpls_egress_pcp_profile(int unit)
{
    bcm_qos_map_t vpls_eg_map;
    int flags = 0;
    int tc, dp;
    int rv;
 
    /* Clear structure */
    bcm_qos_map_t_init(&vpls_eg_map);

    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_L2_VLAN_PCP, &vpls_pcp_eg_map_id);

    if (rv != BCM_E_NONE) {
        printf("error in VPLS egress bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
    }

    for (tc=0; tc<8; tc++) {
      for (dp=0; dp<2; dp++) {
      
        flags = BCM_QOS_MAP_L2_UNTAGGED | BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP;
        /* Set TC -> PCP  */
        vpls_eg_map.int_pri = tc; /* TC Input */
        vpls_eg_map.color = dp; /* Color Input */
        vpls_eg_map.pkt_pri = tc; /* PCP Output */
        vpls_eg_map.pkt_cfi = dp; /* CFI Output */
        
        rv = bcm_qos_map_add(unit, flags, &vpls_eg_map, vpls_pcp_eg_map_id);
           
        if (rv != BCM_E_NONE) {
           printf("error in VPLS egress bcm_qos_map_add()\n");
           printf("rv is: $rv \n");
           printf("(%s)\n", bcm_errmsg(rv));
           return rv;
        }
      }
    }
    return rv;
}

/* Map Egress Remark-Profile to do TC->EXP */
int
qos_map_vpls_egress_remark_profile(int unit)
{
    bcm_qos_map_t vpls_eg_map;
    int flags = 0;
    int int_pri;
    int rv;
 
    /* Clear structure */
    bcm_qos_map_t_init(&vpls_eg_map);

    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS, &vpls_remark_eg_map_id);

    if (rv != BCM_E_NONE) {
        printf("error in VPLS egress bcm_qos_map_create()\n");
        printf("rv is: $rv \n");
        printf("(%s)\n", bcm_errmsg(rv));
    }

    for (int_pri=0; int_pri<8; int_pri++) {      
        
        flags = BCM_QOS_MAP_L2 | BCM_QOS_MAP_ENCAP;
        /* Set PCP -> EXP by mapping TC -> EXP (assume PCP 1:1 TC) */
        vpls_eg_map.remark_int_pri = int_pri; /* Out-DSCP-EXP equals TC Input */
        vpls_eg_map.exp = int_pri; /* PWE.EXP Output */        
        
        rv = bcm_qos_map_add(unit, flags, &vpls_eg_map, vpls_remark_eg_map_id);
           
        if (rv != BCM_E_NONE) {
           printf("error in VPLS egress bcm_qos_map_add()\n");
           printf("rv is: $rv \n");
           printf("(%s)\n", bcm_errmsg(rv));
           return rv;
        }
    }
    return rv;
}


/* 
 * Main function set all three QOS profiles
 */
int
qos_map_vpls(int unit)
{
    int rv = BCM_E_NONE;

    rv = qos_map_vpls_ingress_profile(unit);
    if (rv) {
        printf("error setting up ingress qos profile\n");
        return rv;
    }

    rv = qos_map_vpls_egress_pcp_profile(unit);

    if (rv) {
        printf("error setting up egress PCP qos profile\n");
        return rv;
    }

    rv = qos_map_vpls_egress_remark_profile(unit);

    if (rv) {
        printf("error setting up egress Remark qos profile\n");
        return rv;
    }
    return rv;
}

/* Return the map id set up for pcp egress  */
int
qos_map_id_vpls_pcp_egress_get(int unit)
{
  return vpls_pcp_eg_map_id;
}

/* Return the map id set up for remark egress  */
int
qos_map_id_vpls_remark_egress_get(int unit)
{
  return vpls_remark_eg_map_id;
}

/* Return the map id set up for ingress  */
int
qos_map_id_vpls_ingress_get(int unit)
{
  return vpls_in_map_id;
}

/* Enable cross connect QOS funciton */
int
qos_cross_connect_enable(int unit)
{
  cross_connect_info.qos_example = 1; 
  return 0;
}
