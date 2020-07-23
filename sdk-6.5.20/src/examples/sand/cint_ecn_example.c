/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * ECN Example script
 */

/* 
Forwarding example: 
-------------------  
1) run:
cint ../sand/utility/cint_sand_utils_global.c 
cint utility/cint_utils_l3.c 
cint cint_ip_route.c 
cint cint_qos_l3_rif_cos.c 
cint cint_field_ecn_cni_extract.c 
cint cint_multi_device_utils.c 
cint cint_ecn_example.c
cint 
ecn_basic_example(unit, <in_port>, <out_port>); 
 
This will enable ECN on the device and set extraction of ECN (capable and congestion bits). 
It will also configure IPV4 forwarding. 

run traffic: 
    ethernet header with DA 0:c:0:2:0:0 and vlan tag id 1
    and IPV4 header with DIP 127.255.255.3 and TOS 24/27
expected: 
    ethernet header with DA 0:0:0:0:cd:1d, SA 0:c:0:2:0:0 and vlan tag id 100
    and IPV4 header TOS 24/27 (respectively)
 
2) run: 
ecn_congestion_set(unit, <out_port>, is_wred);
 
Now congestion is configured for every packet that is sent to out_port.  

run traffic: 
    ethernet header with DA 0:0:0:0:0:33 and vlan tag id 200
    MPLS header with label 6000 and exp 1/2 
    ethernet header with DA 0:c:0:2:0:0 and vlan tag id 1
    and IPV4 header with DIP 127.255.255.3 and TOS 9/10
expected: 
    ethernet header with DA 0:0:0:0:0:44 and vlan tag id 400
    and MPLS header with label 1000 and exp 3 
    ethernet header with DA 0:0:0:0:cd:1d, SA 0:c:0:2:0:0 and vlan tag id 100
    and IPV4 header TOS 11
 
 
Encapsulation example: 
---------------------- 
1) run: 
cint cint_qos.c 
cint utility/cint_utils_l3.c   
cint cint_mpls_lsr.c 
cint cint_ecn_example.c 
cint 
ecn_mpls_encap_example(unit, <in_port>, <out_port>); 
 
run traffic: 
    ethernet header with DA 0:0:0:0:0:11 and vlan tag id 20
    IPV4 header with TOS 0-2
 
expected: 
    ethernet header with DA 0:0:0:0:0:44 and vlan tag id 400
    and MPLS header with label 1000 and exp 3
    ethernet header with DA 0:0:0:0:0:11 and vlan tag id 20
    IPV4 header with TOS 3
*/


/* 
 * Enable ECN on the device and set extraction of ECN (capable and congestion bits) 
 * Add IPV4 route 
 */
int ecn_basic_example(int unit, int in_port, int out_port){

    int rv;
    uint32 flags;
    int is_dnx;

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_device_member_get, $rv \n");
        return rv;
    }

    if (!is_dnx)
    {
        /** On jericho2 and above device, ECN is always enabled */

        /* Enable ECN on the device */
        rv = bcm_cosq_discard_get(unit, &flags);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_discard_get\n");
            return rv;
        }
        rv = bcm_cosq_discard_set(unit, flags | BCM_COSQ_DISCARD_MARK_CONGESTION);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_cosq_discard_set with flag BCM_COSQ_DISCARD_MARK_CONGESTION\n");
            return rv;
        }
    }

    /* Set the rules to extract the ECN value (2 bits) */
    rv = ecn_extract_example(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, ecn_extract_example\n");
        return rv;
    }

    /* Add IPV4 route and send to out_port */
    return basic_example(&unit, 1, in_port, out_port);
}

/* 
 * Configures congestion for every packet that is sent to out_port.
 * if is_wred is set, WRED ECN threshold will be set, otherwise regular ECN q size threshold will be set.
 */
int ecn_congestion_set(int unit, int out_port, int is_wred){

    int rv;
    int is_qax;
    bcm_cosq_gport_discard_t ecn_config;
    int modid;
    bcm_gport_t base_q;
    bcm_cos_queue_t cosq = 0;
    bcm_gport_t queue_gport;
    int is_dnx, is_qax_or_above;
    /** assume the queue is associated with rate class 0 */
    int rate_class_id = 0;

    rv = bcm_device_member_get(unit, 0, bcmDeviceMemberDNX, &is_dnx);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_device_member_get, $rv \n");
        return rv;
    }

    rv = is_qumran_ax(unit, &is_qax);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) is_qumran_ax\n", rv);
        return rv;
    }

    is_qax_or_above = (is_qax || is_dnx);

    /* configure ECN for a VOQ */
    bcm_cosq_gport_discard_t_init(&ecn_config);
    if (is_dnx)
    {
        bcm_switch_profile_mapping_t profile_mapping;
        /** In Jericho2, ingress congestion thresholds are configured per rate class and not per queue */
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
        modid = modid_array[0].modid;

        appl_dnx_e2e_scheme_logical_port_to_base_voq_get(unit, modid, out_port, &base_q);
        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(queue_gport, base_q);
        profile_mapping.profile_type = bcmCosqIngressQueueToRateClass;
        bcm_cosq_profile_mapping_get(unit, queue_gport, cosq, 0, &profile_mapping); /* if you use non zero pcp, cosq should be equal to  pcp */
        rate_class_id = profile_mapping.mapped_profile;
        BCM_GPORT_PROFILE_SET(base_q, rate_class_id);
    }
    else
    {
        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(base_q, 24 + (out_port * 8));
    }
    ecn_config.flags = BCM_COSQ_DISCARD_MARK_CONGESTION;
    if (is_qax_or_above) {
        ecn_config.flags |= BCM_COSQ_DISCARD_BYTES;
    } else {
        ecn_config.flags |= BCM_COSQ_DISCARD_BUFFER_DESC /* queue size in BDs */; 
    }

    rv = bcm_cosq_gport_discard_get(unit, base_q, cosq, &ecn_config);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_gport_discard_get\n");
        return rv;
    }

    if (is_wred)
    {
        ecn_config.min_thresh = 0;
        ecn_config.max_thresh = 0x1000;
        ecn_config.drop_probability = 100;
        ecn_config.flags |= BCM_COSQ_DISCARD_ENABLE;
    }
    else
    {
        ecn_config.ecn_thresh = 0;  /* size of queue in bytes to mark congestion. */
    }

    /* change max size configuration */
    rv = bcm_cosq_gport_discard_set(unit, base_q, cosq, &ecn_config);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_cosq_gport_discard_set with ecn_thresh %d\n", ecn_config.ecn_thresh);
        return rv;
    }

    printf("ecn_congestion_set: PASS\n\n");
    return rv;
}

/* 
 * Configure MPLS encapsulation 
 * and and Egress Remark profile mapping of in-DSCP,DP to out-DSCP 
 */
int ecn_mpls_encap_example(int unit, int in_port, int out_port){

    int rv;
    int CINT_NO_FLAGS = 0;
    int eg_qos_id = 2;
    int dscp, dp;
    bcm_qos_map_t l3_ing_map, l3_eg_map, l2_map;
    int ing_intf; /* in-Rif */
    int ing_intf_out; /* out-Rif */
    int egress_intf; /* FEC */
    int encap_id;
    int mpls_termination_label_index_enable;
    bcm_pbmp_t pbmp;
    bcm_mpls_port_t mpls_port;   
    bcm_l2_addr_t l2addr;
    bcm_mac_t my_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x33};
    bcm_mac_t inner_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x11};
    bcm_mac_t next_hop_mac = {0x00, 0x00, 0x00, 0x00, 0x00, 0x44};
    bcm_mpls_vpn_config_t vpn_info;

    bcm_mpls_vpn_config_t_init(&vpn_info);
    
    /* incomming params */ 
    int in_vlan = 200;
    int in_label = 6000;
    int inner_vlan = 20;

    /* egress atrributes */
    int out_label = 1000;    
    int out_vlan = 400;

    vpn_info.vpn = 5000;
    vpn_info.flags = BCM_MPLS_VPN_VPLS|BCM_MPLS_VPN_WITH_ID; 
    vpn_info.broadcast_group = vpn_info.vpn;
    vpn_info.unknown_multicast_group = vpn_info.vpn;
    vpn_info.unknown_unicast_group = vpn_info.vpn;
    rv = bcm_mpls_vpn_id_create(unit, vpn_info);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_vpn_id_create\n");
        return rv;
    }

    /* L3 interface for incoming mpls rounting */
    create_l3_intf_s intf;
    intf.vsi = in_vlan;
    intf.my_global_mac = my_mac;
    intf.my_lsb_mac = my_mac;
    
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }
    
    /* Create QOS (Remark) profile ID */
    rv = bcm_qos_map_create(unit, BCM_QOS_MAP_EGRESS, &eg_qos_id);
    if (rv != BCM_E_NONE) {
        printf("Error, create QoS ID, bcm_qos_map_create with flag BCM_QOS_MAP_EGRESS\n");
        return rv;
    }
    printf("created Remark-profile-id =0x%08x, \n", eg_qos_id);
    
    /* set QoS 1:1 mapping for L3 in egress: map Out-DSCP-EXP = In-DSCP-EXP (IPv4 TOS) */
    for (dscp = 0; dscp < 8; dscp++) {
        printf("\n");
        printf("dscp=%d \n", dscp);
        printf("\n");
        for (dp = 0; dp < 3; dp++) {
            bcm_qos_map_t_init(&l3_eg_map);
            l3_eg_map.color = dp; /* Set internal color (DP) */
            l3_eg_map.int_pri = dscp; /* in-DSCP-EXP (TOS) */
            l3_eg_map.remark_int_pri = dscp; /* in-DSCP-EXP */
            l3_eg_map.dscp = dscp; /* out-DSCP */
            l3_eg_map.exp = dscp; /* out-EXP */
    
            rv = bcm_qos_map_add(unit, BCM_QOS_MAP_L2 | BCM_QOS_MAP_ENCAP, l3_eg_map, eg_qos_id);
            if (rv != BCM_E_NONE) {
                printf("Error, set egress QoS mapping, bcm_qos_map_add, dscp=%d \n", dscp);
                return rv;
            }
        }
    }
    printf("Set egress QoS mapping for L3\n");
    
    /* L3 interface for outgoing mpls routing: Associate Out-RIF to QOS profile ID */
    intf.vsi = out_vlan;
    
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_out = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }
    printf("End of create_l3_intf_qos\n");
    
    /* Allocate an Egress object (FEC) that will point to MPLS tunnel (LL) and will be associated to the QOS (Remark) profile */
    create_l3_egress_s l3_eg;
    sal_memcpy(l3_eg.next_hop_mac_addr, next_hop_mac, 6);  

    l3_eg.allocation_flags = CINT_NO_FLAGS;
    l3_eg.out_tunnel_or_rif   = 0;
    l3_eg.vlan   = out_vlan;
    l3_eg.out_gport  = out_port;
    l3_eg.qos_map_id = eg_qos_id;
    l3_eg.arp_encap_id = encap_id;
    l3_eg.fec_id = egress_intf;

    rv = l3__egress__create(unit, &l3_eg);
    if (rv != BCM_E_NONE) {
        printf("Error, create egress object, out_port=%d, \n", out_port);
        return rv;
    }
    
    encap_id = l3_eg.arp_encap_id;
    egress_intf = l3_eg.fec_id;
    
    printf("Created FEC: %d\n", egress_intf);
    
    /* create MPLS port, for termination of the MPLS header */
    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port.match_label = in_label;
    mpls_port.egress_tunnel_if = egress_intf; /* FEC */
    mpls_port.flags = BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.port = in_port;
    mpls_port.egress_label.label = out_label;
    
    /* read mpls index soc property */
    mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
    if (mpls_termination_label_index_enable) {
    	BCM_MPLS_INDEXED_LABEL_SET(&mpls_port.match_label,in_label,1);
    }
    
    /* Create multipoint mpls port */
    rv = bcm_mpls_port_add(unit, vpn_info.vpn, &mpls_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add\n");
        print rv;
        return rv;
    }
    
    /* Add Static entry 00:00:00:00:00:11 points to MPLS PORT encapsulation*/    
    l2addr.flags = BCM_L2_STATIC;
    bcm_l2_addr_t_init(&l2addr, inner_mac, vpn_info.vpn);
    l2addr.port = mpls_port.mpls_port_id;
    rv = bcm_l2_addr_add(unit, &l2addr);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l2_addr_add\n");
        print rv;
        return rv;
    }
    
    
    return rv;
}

