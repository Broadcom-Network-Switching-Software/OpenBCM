/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 */
 
/* Examples for:
 * 1) In-RIF Remark (LIF COS profile) and Egress Forwarding layer (Remark profile) configuration. 
 *      in ingress: - DSCP mapping to in-DSCP
 *                  - in-DSCP mapping to TC,DP 
 *      in egress:  - in-DSCP,DP mapping to out-DSCP  
 *  
 * run: 
 * cint examples/dpp/utility/cint_utils_l3.c 
 * cint examples/dpp/cint_ip_route.c 
 * cint examples/dpp/cint_qos_l3_rif_cos.c
 * cint
 * lif_cos_and_remark_example(unit, <in_port>, <out_port>); 
 */
 
  
/********** 
  Globals/Aux Variables
 */

/* debug prints */
verbose = 1;


/********** 
  functions
 */

int ing_qos_id, eg_qos_id, pcp_vlan_profile;
int qos_map_l3_ingress_dscp_profile(int unit)
{   
    bcm_qos_map_t l3_ing_map;
    int dscp;
    int rv;
    
    /* create QoS ID (new cos profile) */
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_INGRESS, &ing_qos_id);
    if (rv != BCM_E_NONE) {
        printf("Error, create ingress QoS ID, bcm_qos_map_create with flag BCM_QOS_MAP_INGRESS\n");
        return rv;
    }
    printf("created QoS-id =0x%08x, \n", ing_qos_id);
    
    /* set QoS mapping for L3 in ingress:
       map In-DSCP-EXP = IP-Header.TOS - 1 */
    for (dscp=0; dscp<256; dscp++) {
        bcm_qos_map_t_init(&l3_ing_map);
        l3_ing_map.dscp = dscp; /* packet DSCP (TOS) */
        l3_ing_map.int_pri = dscp % 8; /* TC */
        l3_ing_map.color = dscp % 2; /* DP */
        l3_ing_map.remark_int_pri = (dscp - 1) & 0xff; /* in-DSCP-exp */

        rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L3|BCM_QOS_MAP_IPV4, &l3_ing_map, ing_qos_id);
        if (rv != BCM_E_NONE) {
            printf("Error, set QoS mapping, bcm_qos_map_add, dscp=%d \n", dscp);
            return rv;
        }
    }
    printf("set ingress QoS mapping for L3\n");
    
    return rv;
}
    
int qos_map_l3_egress_dscp_profile(int unit) 
{
    bcm_qos_map_t l3_eg_map;
    int dscp, dp;
    int rv;
    
    /* Create QOS (Remark) profile ID */
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS, &eg_qos_id);
    if (rv != BCM_E_NONE) {
        printf("Error, create QoS ID, bcm_qos_map_create with flag BCM_QOS_MAP_EGRESS\n");
        return rv;
    }
    printf("created Remark-profile-id =0x%08x, \n", eg_qos_id);

    /* set QoS mapping for L3 in egress:
       map Out-DSCP-EXP (TOS) = In-DSCP-EXP + DP */
    for (dscp = 0; dscp < 256; dscp++) {
        for (dp = 0; dp < 2; dp++) {
            bcm_qos_map_t_init(&l3_eg_map);
            l3_eg_map.color = dp; /* Set internal color (DP) */
            l3_eg_map.int_pri = dscp; /* in-DSCP-EXP */
            l3_eg_map.remark_int_pri = (dscp + dp) & 0xff; /* TOS */

            rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L3, l3_eg_map, eg_qos_id);
            if (rv != BCM_E_NONE) {
                printf("Error, set egress QoS mapping, bcm_qos_map_add, dscp=%d \n", dscp);
                return rv;
            }
        }
    }
    printf("Set egress QoS mapping for L3\n");
    
    return rv;
} 

int qos_map_l3_egress_dscp_to_pri_cfi_profile(int unit)    
{
    bcm_qos_map_t l3_pcp_map;
    int dscp;
    int rv;
    
    /* Create QOS profile (PCP-VLAN) ID */
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS|BCM_QOS_MAP_L2_VLAN_PCP, &pcp_vlan_profile);
    if (rv != BCM_E_NONE) {
        printf("Error, create QoS ID, bcm_qos_map_create with flags BCM_QOS_MAP_EGRESS|BCM_QOS_MAP_L2_VLAN_PCP\n");
        return rv;
    }
    printf("created PCP-profile-id =0x%08x, \n", pcp_vlan_profile);

    /* set QoS mapping for L2: map DSCP => PCP, DEIin */
    for (dscp = 0; dscp < 64; dscp++) {
        bcm_qos_map_t_init(&l3_pcp_map);
        l3_pcp_map.dscp = dscp; /* packet DSCP (TOS) */
        l3_pcp_map.pkt_pri = 2; /* PCP */
        l3_pcp_map.pkt_cfi = 1; /* DEI */

        rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L3|BCM_QOS_MAP_L2_VLAN_PCP|BCM_QOS_MAP_IPV4, l3_pcp_map, pcp_vlan_profile);
        if (rv != BCM_E_NONE) {
            printf("Error, set L3 PCP QoS mapping, bcm_qos_map_add, dscp=%d \n", dscp);
            return rv;
        }
    }
    
    return rv;
}

int qos_map_l2_egress_tcdp_to_pri_cfi_profile(int unit)
{
    bcm_qos_map_t l2_pcp_map;
    int rv;
    int tc,dp;
    int flags;

    /* Create QOS profile (PCP-VLAN) ID */
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS|BCM_QOS_MAP_L2_VLAN_PCP, &pcp_vlan_profile);
    if (rv != BCM_E_NONE) {
        printf("Error, create QoS ID, bcm_qos_map_create with flags BCM_QOS_MAP_EGRESS|BCM_QOS_MAP_L2_VLAN_PCP $rv \n");
        return rv;
    }
    printf("created PCP-profile-id =0x%08x, \n", pcp_vlan_profile);

    for (tc=0; tc<8; tc++) {
        for (dp=0; dp<2; dp++) {
            flags = BCM_QOS_MAP_L2_UNTAGGED | BCM_QOS_MAP_L2 | BCM_QOS_MAP_L2_VLAN_PCP;
            if (tc != 7) { 
                /* Set TC -> PCP  */
                l2_pcp_map.int_pri = tc; /* TC Input */
                l2_pcp_map.color = dp; /* Color Input */
                l2_pcp_map.pkt_pri = tc + 1; /* PCP Output */
                l2_pcp_map.pkt_cfi = dp; /* CFI Output */
            } else { 
                /* Set TC -> PCP  */
                l2_pcp_map.int_pri = tc; /* TC Input */
                l2_pcp_map.color = dp; /* Color Input */
                l2_pcp_map.pkt_pri = tc; /* PCP Output */
                l2_pcp_map.pkt_cfi = dp; /* CFI Output */
            }

            rv = bcm_qos_map_add(unit, flags, &l2_pcp_map, pcp_vlan_profile);    
            if (rv != BCM_E_NONE) {
                printf("Error, bcm_qos_map_add\n");
                return rv;
            }
        }
    }  
}

int eve_action(int unit, int out_port, bcm_vlan_t out_vlan)
{
    int rv;
    bcm_vlan_action_set_t action;

    rv = bcm_port_class_set(unit, out_port, bcmPortClassId, out_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_class_set\n");
        return rv;
    }

    /* Associate Out-AC to QOS profile ID, by egress vlan editing */
    bcm_vlan_action_set_t_init(&action);
    action.ut_outer = bcmVlanActionAdd; /* Replace VLAN tag */
    action.new_outer_vlan = out_vlan;
    action.ut_outer_pkt_prio = bcmVlanActionAdd;
    action.priority = pcp_vlan_profile;
   
    rv = bcm_vlan_translate_egress_action_add(unit, out_port, out_vlan, BCM_VLAN_NONE, &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_egress_action_add\n");
        return rv;
    }

    return rv;
}

int eve_action_advance(int unit, bcm_gport_t out_port, bcm_vlan_t out_vlan)
{
    int rv;
	int i, j;
    int action_id = 1;
    bcm_vlan_action_set_t action;
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_port_translation_t port_trans;
 
    bcm_vlan_action_set_t_init(&action);

     /* get the vlan edit translation id, if it hasn't existed, then create directly; if it has existed, then destroy it at first, and create */
    rv = bcm_vlan_translate_action_id_get(unit, BCM_VLAN_ACTION_SET_EGRESS, action_id, &action);
    if (rv == BCM_E_NOT_FOUND) {
        rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &action_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create ing $rv\n");
            return rv;
        }
    } else {
        rv = bcm_vlan_translate_action_id_destroy(unit, BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID, action_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_destroy ing $rv\n");
            return rv;
        }
        
        rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID, &action_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_translate_action_id_create ing $rv\n");
            return rv;
        }
    }

    /* Associate Out-AC to QOS profile ID, by egress vlan editing */
    action.flags = BCM_VLAN_ACTION_SET_EGRESS;
    action.outer_tpid_action = bcmVlanTpidActionModify;
    action.outer_tpid = 0x8100;
    action.dt_outer = bcmVlanActionReplace; /* Replace VLAN tag */
    action.new_outer_vlan = out_vlan;
    action.dt_outer_pkt_prio = bcmVlanActionReplace;
    rv = bcm_vlan_translate_action_id_set( unit, BCM_VLAN_ACTION_SET_EGRESS, action_id, &action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set\n");
        return rv;
    }
    port_trans.new_outer_vlan = out_vlan;
    port_trans.gport = out_port;
    port_trans.vlan_edit_class_id = 1;
    port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_vlan_port_translation_set $rv \n");
        return rv;
    }
    rv = bcm_qos_port_map_set(unit, out_port, -1, pcp_vlan_profile);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_qos_port_map_set $rv \n");
        return rv;
    }

    /* Set translation action class for c_tag packets */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = 1;
    action_class.tag_format_class_id = 2;
    action_class.vlan_translation_action_id = action_id;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_translate_action_class_set(unit,  &action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set egr $rv\n");
        return rv;
    }
    return rv;
}


int qos_delete_all_maps(int unit, int map_id) 
{
    int rv = BCM_E_NONE;
    bcm_qos_map_t l3_in_map;
    int dscp = 0;

    /* Clear structure */
    bcm_qos_map_t_init(&l3_in_map);
    for (dscp = 0; dscp < 64; dscp++) {
        l3_in_map.dscp = dscp;
        rv = bcm_qos_map_delete(unit, BCM_QOS_MAP_L3|BCM_QOS_MAP_L2_VLAN_PCP|BCM_QOS_MAP_IPV4, l3_in_map, map_id);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_qos_map_delete $rv \n");
            return rv;
        }
    }
    return rv;
}

int qos_map_id_destroy(int unit, int map_id) 
{
    int rv;
    rv = bcm_qos_map_destroy(unit, map_id);
    if (rv != BCM_E_NONE) {
        printf("Error in bcm_qos_map_destroy $rv \n");
        return rv;
    }
    return rv;
}


/******* Run examples ******/
 
/* 
 * Example of: 
 * In-RIF Remark (LIF COS profile) and Egress Forwarding layer (Remark profile). 
 *  
 * packet will be routed from in_port to out-port 
 * in ingress: packet DSCP will be mapped to in-DSCP, which will be mapped to TC,DP 
 * in egress: in-DSCP,DP will be mapped to out-DSCP 
 *  
 * packet to send: 
 *  - in port = in_port
 *  - vlan 15.
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0x7fffff03 (127.255.255.3)
 *  - TOS between 0-31
 *  
 * expected: 
 *  - out port = out_port
 *  - vlan 2.
 *  - DA = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  TTL decremented
 *  IPV4 TOS according to mapping
 */

int lif_cos_and_remark_example(int unit, int in_port, int out_port){
    int rv;
    int l3_intf_id, ing_intf_out; /* in-rif and out-rif */
    int fec;
    int in_vlan = 15, out_vlan = 2; /* in and out vlan */
    int vrf = 0;
    int host;
    int encap_id;
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */

    rv = qos_map_l3_ingress_dscp_profile(unit);
    if (rv != BCM_E_NONE) {
        printf("error in qos_map_l3_ingress_dscp_profile $rv");
        return rv;
    }
    
    /* create ingress router interface: Associate In-RIF to QOS profile ID */
    rv = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
    if (rv != BCM_E_NONE) {
        printf("fail add port(0x%08x) to vlan(%d)\n", port, vlan);
        return rv;
    }

    create_l3_intf_s intf;
    intf.vsi = in_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;

    intf.rpf_valid = 1;
    intf.flags |= BCM_L3_RPF;
    intf.urpf_mode = bcmL3IngressUrpfLoose;
    intf.ingress_flags |= BCM_L3_INGRESS_GLOBAL_ROUTE | BCM_L3_INGRESS_DSCP_TRUST;

    intf.qos_map_valid = 1;
    intf.qos_map_id = ing_qos_id;

    intf.mtu_valid = 1;
    intf.mtu = 0;
    intf.mtu_forwarding = 0;

    rv = l3__intf_rif__create(unit, &intf);
    l3_intf_id = intf.rif;
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create");
    }

    rv = qos_map_l3_egress_dscp_profile(unit);
    if (rv != BCM_E_NONE) {
        printf("error in qos_map_l3_egress_dscp_profile $rv");
        return rv;
    }
    
    /* create egress router interface: Associate Out-RIF to QOS profile ID */
    rv = bcm_vlan_create(unit, out_vlan);
    if (rv != BCM_E_NONE) {
      printf("fail open vlan(%d)\n", out_vlan);
    }
    rv = bcm_vlan_gport_add(unit, out_vlan, in_port, 0);
    if (rv != BCM_E_NONE) {
        printf("fail add port(0x%08x) to vlan(%d)\n", port, vlan);
      return rv;
    }

    intf.vsi = out_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;

    intf.rpf_valid = 1;
    intf.flags |= BCM_L3_RPF;
    intf.urpf_mode = bcmL3IngressUrpfLoose;
    intf.ingress_flags |= BCM_L3_INGRESS_GLOBAL_ROUTE | BCM_L3_INGRESS_DSCP_TRUST;
    intf.qos_map_id = eg_qos_id;

    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_out = intf.rif;
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create");
    }

    /* create egress object (FEC) with the created Out-RIF */
    create_l3_egress_s l3eg;
    sal_memcpy(l3eg.next_hop_mac_addr, next_hop_mac , 6);
    l3eg.out_tunnel_or_rif = ing_intf_out;
    l3eg.out_gport = out_port;
    l3eg.vlan = out_vlan;
    l3eg.fec_id = fec;
    l3eg.arp_encap_id = encap_id;

    rv = l3__egress__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    fec = l3eg.fec_id;
    encap_id = l3eg.arp_encap_id;
    printf("created FEC-id =0x%08x, \n", fec);
    printf("next hop mac at encap-id %08x, \n", encap_id);

    /* add host and point to FEC */
    host = 0x7fffff03;
    rv = add_host(unit, 0x7fffff03, vrf, fec);
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, in_port=%d, \n", in_port);
        return rv;
    }
    print_host("add entry ", host,vrf);
    printf("---> egress-object=0x%08x, port=%d, \n", fec, out_port);

    return rv;
}

 
/* 
 * Example of: 
 * EVE PCP-DEI. 
 *  
 * packet will be routed from in_port to out-port 
 * 
 * packet to send: 
 *  - in port = in_port
 *  - vlan 15.
 *  - DA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  - DIP = 0x7fffff03 (127.255.255.3)
 *  
 * expected: 
 *  - out port = out_port
 *  - vlan 2.
 *  - DA = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}
 *  TTL decremented
 *  IPV4 TOS according to mapping
 */
int pcp_dei_example(int unit, int in_port, int out_port){
    int rv;
    int l3_intf_id, ing_intf_out; /* in-rif and out-rif */
    int pcp_vlan_profile;
    bcm_qos_map_t l3_pcp_map;
    int dscp;
    int fec;
    int in_vlan = 15, out_vlan = 2;
    bcm_vlan_action_set_t action;
    int vrf = 0;
    int host;
    int encap_id;    
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    int is_advanced_vlan_translation_mode = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);
    int gport_in, gport_out;

    /* create ingress router interface */
    rv = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
    if (rv != BCM_E_NONE) {
        printf("fail add port(0x%08x) to vlan(%d)\n", port, vlan);
        return rv;
    }
    
    create_l3_intf_s intf;
    intf.vsi = in_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;
    
    intf.rpf_valid = 1;
    intf.flags |= BCM_L3_RPF;
    intf.urpf_mode = bcmL3IngressUrpfLoose;
    intf.ingress_flags |= BCM_L3_INGRESS_GLOBAL_ROUTE | BCM_L3_INGRESS_DSCP_TRUST;
    
    intf.qos_map_valid = 1;
    intf.qos_map_id = ing_qos_id;
    
    rv = l3__intf_rif__create(unit, &intf);
    l3_intf_id = intf.rif;
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create");
    }
    
    printf("created L3-ingress-id =0x%08x, \n", l3_intf_id);
    
    /* create egress router interface */
    rv = bcm_vlan_create(unit, out_vlan);
    if (rv != BCM_E_NONE) {
      printf("fail open vlan(%d)\n", out_vlan);
    }
    rv = bcm_vlan_gport_add(unit, out_vlan, in_port, 0);
    if (rv != BCM_E_NONE) {
        printf("fail add port(0x%08x) to vlan(%d)\n", port, vlan);
      return rv;
    }
    
    intf.vsi = out_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;
    
    intf.rpf_valid = 1;
    intf.flags |= BCM_L3_RPF;
    intf.urpf_mode = bcmL3IngressUrpfLoose;
    intf.ingress_flags |= BCM_L3_INGRESS_GLOBAL_ROUTE | BCM_L3_INGRESS_DSCP_TRUST;
    
    intf.qos_map_valid = 1;
    intf.qos_map_id = 0;
    
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_out = intf.rif;
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create");
    }

    printf("created L3-ingress-id =0x%08x, \n", l3_intf_id);

    /* create egress object with the created Out-RIF */
    create_l3_egress_s l3eg;
    sal_memcpy(l3eg.next_hop_mac_addr, next_hop_mac , 6);
    l3eg.out_tunnel_or_rif = ing_intf_out;
    l3eg.out_gport = out_port;
    l3eg.vlan = out_vlan;
    l3eg.fec_id = fec;
    l3eg.arp_encap_id = encap_id;

    rv = l3__egress__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    fec = l3eg.fec_id;
    encap_id = l3eg.arp_encap_id;
    printf("created FEC-id =0x%08x, \n", fec);
    printf("next hop mac at encap-id %08x, \n", encap_id);

    /* add host point to FEC */
    host = 0x7fffff03;
    rv = add_host(unit, 0x7fffff03, vrf, fec); 
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, in_port=%d, \n", in_port);
        return rv;
    }
    print_host("add entry ", host,vrf);
    printf("---> egress-object=0x%08x, port=%d, \n", fec, out_port);

    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.vsi = out_vlan;
    vlan_port.port = out_port;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.match_vlan        = out_vlan;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("bcm_vlan_port_create index failed! %s\n", bcm_errmsg(rv));
    }
    gport_out = vlan_port.vlan_port_id;

    print gport_out;


    rv = qos_map_l3_egress_dscp_to_pri_cfi_profile(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, qos_map_l3_egress_dscp_to_pri_cfi_profile\n");
        return rv;
    }
    if (is_advanced_vlan_translation_mode) {
        rv = eve_action_advance(unit, gport_out, out_vlan);
        if (rv != BCM_E_NONE) {
            printf("Error, eve_action_advance\n");
            return rv;
        }
    }
    else {    
        rv = eve_action(unit, out_port, out_vlan);
        if (rv != BCM_E_NONE) {
            printf("Error, eve_action\n");
            return rv;
        }
    }

    return rv;
}

