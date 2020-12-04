/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* 
 * the cint creates 2 ports of types: <type1>,<type2>, each port with/out protection 
 * of type facility/path and then cross-connects the 2 ports. 
 *  
 * optional ports combinations: 
 * vlan - vlan 
 * vlan - mpls 
 *  
 * type: 1 = vlan, 2 = mpls
 * with_protection: 1 = with, 0 = without 
 * facility_protection: 1 = facitilty type protection, 0 = path
 *  
 * run: 
 * cint ../sand/utility/cint_sand_utils_global.c 
 * cint ../sand/utility/cint_sand_utils_mpls.c
 * cint utility/cint_utils_mpls_port.c 
 * cint utility/cint_utils_multicast.c 
 * cint utility/cint_utils_l2.c 
 * cint utility/cint_utils_l3.c  
 * cint cint_port_tpid.c 
 * cint cint_advanced_vlan_translation_mode.c 
 * cint cint_mpls_lsr.c 
 * cint cint_vswitch_metro_mp.c  
 * cint cint_vswitch_cross_connect_p2p.c 
 * cint 
 * run(unit, <port1>, <type1>, <with_protection_1>, <second_port1>, <port2>, <type2>, <with_protection_2>, <second_port2>, <facility_protection>); 
 *  - with_protection_1 == 0 ==> no protection
 *  - with_protection_1 == 1 ==> 1:1 protection
 *  - with_protection_1 == 2 ==> 1+1 protection
 *  
 *  with_protection_2 can be only 0 or 1.
 *  
 *  if with_protection:
 *  second_port1, second_port2 will be used as well.
 *  
 * run packets: 
 * from vlan port (port1): 
 *      ethernet header with any DA, SA
 *      vlan tag with vlan tag id 100 (without protection) or 101 (with protection)
 *  
 * for case vlan - vlan, second vlan port (port2): 
 *      ethernet header with any DA, SA
 *      vlan tag with vlan tag id 101 (both without protection) ,102 (one with protection) or 103 (both with protection)
 *  
 * from mpls port (port1): 
 *      ethernet header with DA 11 and any SA
 *      vlan tag with vlan tag id 10 (without protection) or 11 (with protection)
 *      mpls header with label 20 (without protection) or 21 (with protection)
 *  
 * the packet will arrive at the port that is cross-connected to the port it was sent from 
 * when arrives at vlan port: 
 *      with vlan tag id 1000 (without protection) or 1001 (with protection)
 * for case vlan - vlan, when arrives at second vlan port: 
 *      with vlan tag id 1001 (both without protection) ,1002 (one with protection) or 1003 (both with protection)
 *  
 * when arrives at mpls port: 
 *      with ethernet header with DA 22, SA 11 
 *      mpls header with label1: 40, exp 0, ttl 40
 *                       label2: 20, exp 0, ttl 20
 *                       label3: 40, exp 0(0), ttl 40(20) (without protection)
 *      or mpls header with label1: 41, exp 0, ttl 40
 *                          label2: 21, exp 0, ttl 20
 *                          label3: 41, exp 0, ttl 40 (with protection)
 *  
 * if protection exists: 
 *      if the port that is cross-connected will fail, packets will arrive at the protection port
 *  
 * run: 
 * traverse_run(unit); - to traverse and print all the cross-connected ports 
 * delete_all(unit); - to delete all the cross-connections 
 */

int verbose = 3;
int vswitch_p2p_encap_id = 0; /* global variable that saves encap id of mpls_port created in mpls_port_create*/
int vswitch_p2p_inlif_mpls_port_id = 0;
int vswitch_p2p_vlan_port_id = 0;
int vswitch_p2p_fec_id = 0;
int vswitch_p2p_create_mpls_port_without_fec_and_replace = 0; /* global variable that indicates whether to create a mpls port without fec (and then replace it) */
int configuration_hub = 0; /* set the vlan port to be HUB */
int decoupled_mode = 0; /* global variable that determines whether we are using decoupled mode in protection*/

struct cross_connect_info_s {

    /* physical ports */
    bcm_port_t port_1; 
    bcm_port_t port_2; 

    bcm_port_t port_5; /* for traverse */

    /* logical ports */
    bcm_gport_t vlan_port_1; /* vlan ports */
    bcm_gport_t vlan_port_2; 
    bcm_mpls_port_t mpls_port_1; /* mpls port */

    /* logical ports for protection */
    bcm_gport_t vlan_port_3; 
    bcm_gport_t vlan_port_4; 
    bcm_mpls_port_t mpls_port_2;

    bcm_vswitch_cross_connect_t gports;
    /* 1+1 protection port*/
    bcm_gport_t prim_gport_1;

    /* data for vlan port */
    bcm_vlan_t outer_vlan_base;
    bcm_vlan_t eg_vlan_base;
    
    /* data for mpls port */
    int in_vc_label_base; /* base for incomming VC label */
    int eg_vc_label_base; /* base for egress VC label */
    int vlan_base;
    int mpls_eg_vlan_base;
    uint8 my_mac[6];
    uint8 nh_mac[6]; /* next hop mac address */

    /* MC for 1+1 protection*/
    int protection_mc1;
    int protection_mc2;

};

cross_connect_info_s cross_connect_info;

void
cross_connect_info_init(int port1, int port2) {

    bcm_vswitch_cross_connect_t_init(&cross_connect_info.gports);

    cross_connect_info.port_1 = BCM_GPORT_SYSTEM_PORT_ID_GET(port1); 
    cross_connect_info.port_2 = BCM_GPORT_SYSTEM_PORT_ID_GET(port2); 

    cross_connect_info.port_5 = 5;

    cross_connect_info.outer_vlan_base = 100;
    cross_connect_info.eg_vlan_base = 1000;

    cross_connect_info.in_vc_label_base = 20;
    cross_connect_info.eg_vc_label_base = 40; 
    cross_connect_info.vlan_base = 10;
    cross_connect_info.mpls_eg_vlan_base = 20;

    cross_connect_info.my_mac[0] = 0x00;
    cross_connect_info.my_mac[1] = 0x00;
    cross_connect_info.my_mac[2] = 0x00;
    cross_connect_info.my_mac[3] = 0x00;
    cross_connect_info.my_mac[4] = 0x00;
    cross_connect_info.my_mac[5] = 0x11;

    cross_connect_info.nh_mac[0] = 0x00;
    cross_connect_info.nh_mac[1] = 0x00;
    cross_connect_info.nh_mac[2] = 0x00;
    cross_connect_info.nh_mac[3] = 0x00;
    cross_connect_info.nh_mac[4] = 0x00;
    cross_connect_info.nh_mac[5] = 0x22;

    cross_connect_info.protection_mc1 = 6001;
    cross_connect_info.protection_mc2 = 6002;
}

/* create vlan_port (Logical interface identified by port-vlan-vlan) */
int
vlan_port_create(
    int unit,
    uint32 flags,
    bcm_port_t port_id,  
    bcm_vlan_t match_vlan, /* outer vlan */
    bcm_vlan_t egress_vlan,
    bcm_gport_t *gport,
    int failover_id,
    bcm_gport_t protect_gport,
    int protect_mc
    ){

    int rv;
    bcm_vlan_port_t vp;
    bcm_pbmp_t pbmp, ubmp;

    rv = bcm_vlan_create(unit, match_vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create with vlan %d continue...\n", match_vlan);
    }

    bcm_vlan_port_t_init(&vp);
    vp.flags = flags;

    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.port = port_id;
    vp.match_vlan = match_vlan;
    vp.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : egress_vlan; /* when forwarded to this port, packet will be set with this out-vlan */
    vp.vsi = 0; /* will be populated when the gport is added to service, using vswitch_port_add */
    vp.flags |= (configuration_hub ? BCM_VLAN_PORT_NETWORK : 0);

    vp.failover_mc_group = protect_mc; 
    /* distinguish between ingress/egress failover according if MC is set*/
    if (protect_mc == 0) {
        vp.failover_id = failover_id;
    }
    else{
        vp.ingress_failover_id = failover_id;
    }
    vp.failover_port_id = protect_gport;

    rv = bcm_vlan_port_create(unit, &vp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        print rv;
        return rv;
    }

    if(verbose >= 2) {
        printf("created vlan-port:0x0%8x  \n", vp.vlan_port_id);
        printf("  encap-id: 0x%8x  \n", vp.encap_id);
        printf("  in-port: %d\n", vp.port);
        printf("  in-vlan: %d\n", vp.match_vlan);
    }
    /* egress_vlan will be used at EVE */
    vp.egress_vlan = egress_vlan; 

    /* return port id to user */
    *gport = vp.vlan_port_id;
    vswitch_p2p_vlan_port_id = vp.vlan_port_id;
    /* In advanced vlan translation mode, bcm_vlan_port_create does not create ingress / egress
       action mapping. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        rv = vlan_translation_vlan_port_create_to_translation(unit, &vp);
        if (rv != BCM_E_NONE) {
            printf("Error: vlan_translation_vlan_port_create_to_translation\n");
        }
    } else {
        BCM_PBMP_CLEAR(pbmp); /* tagged */
        BCM_PBMP_CLEAR(ubmp); /* untagged */
        BCM_PBMP_PORT_ADD(pbmp, vp.port);
        
        /* set vlan port membership */
        rv = bcm_vlan_port_add(unit, match_vlan, pbmp, ubmp);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_port_add with vlan %d\n", match_vlan);
            print rv;
            return rv;
        }
    }
    return BCM_E_NONE;
}

/* initialize the tunnels for mpls routing */
int
mpls_tunnels_config(
    int unit,
    bcm_port_t port,
    int *egress_intf, 
    int in_tunnel_label,
    int out_tunnel_label){

    int ingress_intf;
    int encap_id;
    bcm_mpls_egress_label_t label_array[2];
    bcm_pbmp_t pbmp;
    int rv;
    bcm_vlan_t vlan, eg_vlan;
 
    /* open vlan */
    vlan = cross_connect_info.vlan_base++;
    printf("open vlan %d\n", vlan);
    rv = bcm_vlan_create(unit, vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create with vlan %d continue \n", vlan);
        print rv;
    }

    /* add vlan to pwe_port */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    rv = bcm_vlan_port_add(unit, vlan, pbmp, pbmp);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_add, vlan=%d, \n", vlan);
        print rv;
        return rv;
    }
 
    /* l2 termination for mpls routing: packet received on those VID+MAC will be L2 terminated  */
    create_l3_intf_s intf;
    intf.vsi = vlan;
    intf.my_global_mac = cross_connect_info.my_mac;
    intf.my_lsb_mac = cross_connect_info.my_mac;
    intf.skip_mymac = 1;
    rv = l3__intf_rif__create(unit, &intf);
    ingress_intf = intf.rif;
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create");
    }
    
    /* create ingress object, packet will be routed to */
    eg_vlan = cross_connect_info.mpls_eg_vlan_base++;
    
    intf.vsi = eg_vlan;
    rv = l3__intf_rif__create(unit, &intf);
    ingress_intf = intf.rif;
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create");
    }

    /* set tunnel over this l3 interface, so packet forwarded to this interface will be tunneled */
    bcm_mpls_egress_label_t_init(&label_array[0]);
    bcm_mpls_egress_label_t_init(&label_array[1]);

    label_array[0].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    if (is_device_or_above(unit, JERICHO2)) {
        label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    } else {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
    }

    label_array[0].exp = 0; /* Set it statically 0*/ 
    if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
        if (is_device_or_above(unit, JERICHO2)) {
            label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
        } else {
            label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
        }
    } else {
        if (is_device_or_above(unit, JERICHO2)) {
            label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        } else {
            label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        }
    }
    label_array[0].label = in_tunnel_label;
    label_array[0].ttl = 20;


    label_array[1].exp = 0; /* Set it statically 0 */ 
    label_array[1].flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;  
    if (is_device_or_above(unit, JERICHO2)) {
        label_array[1].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    } else {
        label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET;
    }

    label_array[1].exp = 0; /* Set it statically 0*/ 
    if (!is_device_or_above(unit,ARAD_PLUS) || mpls_pipe_mode_exp_set) {
        if (is_device_or_above(unit, JERICHO2)) {
            label_array[1].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
        } else {
            label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_EXP_SET;
        }
    } else {
        if (is_device_or_above(unit, JERICHO2)) {
            label_array[1].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
        } else {
            label_array[1].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        }
    }
    label_array[1].label = out_tunnel_label;
    label_array[1].ttl = 40;

    if (!is_device_or_above(unit, JERICHO2)) {
        /** In JR1, l3_intf_id is used for RIF or next tunnel-id.*/
        label_array[1].l3_intf_id = ingress_intf;
    } else {
        /** In JR2, l3_intf_id is used for next-outlif which will be set in creating ARP.*/
        label_array[1].l3_intf_id = 0;
    }

    rv = sand_mpls_tunnel_initiator_create(unit, 0, 2, label_array);
    if (rv != BCM_E_NONE) {
        printf("Error, in sand_mpls_tunnel_initiator_create\n");
        print rv;
        return rv;
    }

    create_l3_egress_s l3eg;
    l3eg.out_tunnel_or_rif = label_array[0].tunnel_id;

    sal_memcpy(l3eg.next_hop_mac_addr, cross_connect_info.nh_mac, 6);
    l3eg.vlan   = eg_vlan;
    l3eg.arp_encap_id = encap_id; 
    rv = l3__egress_only_encap__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, in l3__egress_only_encap__create ");
        print rv;
    }
    encap_id = l3eg.arp_encap_id;
    

    /* create egress object points to this tunnel/interface */
    *egress_intf = label_array[0].tunnel_id;    
      
    return rv;
}

int 
mpls_port_create_ingress_egress(
    int unit, 
    bcm_port_t port, 
    bcm_mpls_port_t *mpls_port,
    int failover_id,
    bcm_gport_t protect_gport,
    int protect_mc) {

    int rv;
    int egress_intf;
    int in_vc_label = cross_connect_info.in_vc_label_base++;
    int eg_vc_label = cross_connect_info.eg_vc_label_base++;
    int mpls_termination_label_index_enable;
    mpls_port__forward_group__info_s forward_group_info;
    int is_jr2_a0 = 0;

    mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
        
    rv = mpls_tunnels_config(unit, port, &egress_intf, in_vc_label, eg_vc_label);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_tunnels_config\n");
        return rv;
    }

    /* add port, according to VC label */
    bcm_mpls_port_t_init(mpls_port);
    
    /* Set parameters for egress */
    if (configuration_hub) {
        mpls_port->flags |= BCM_MPLS_PORT_NETWORK;
    }

    rv = mpls_port__update_network_group_id(unit, mpls_port);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_port__update_network_group_id\n");
        return rv;
    }

    mpls_port->flags = BCM_MPLS_PORT_EGRESS_TUNNEL|BCM_MPLS_PORT_ENCAP_WITH_ID;
    mpls_port->flags2 = BCM_MPLS_PORT2_EGRESS_ONLY;
    mpls_port->egress_label.label = eg_vc_label;
    mpls_port->egress_label.flags |= (is_device_or_above(unit, JERICHO2) ? BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT : 0);
    mpls_port->failover_id = failover_id;
    mpls_port->failover_port_id = protect_gport;
    mpls_port->failover_mc_group = protect_mc;
    mpls_port->criteria = BCM_MPLS_PORT_MATCH_INVALID;
    mpls_port->egress_tunnel_if = egress_intf;
    mpls_port->mpls_port_id = 0x18805000;
    mpls_port->encap_id = 0x5000;

    rv = bcm_mpls_port_add(unit, 0, mpls_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add EGRESS\n");
        return rv;
    }

    printf("EGRESS: add port/encap 0x%08x/0x%08x to vpn with tunnel_if = 0x%08x\n\r",mpls_port->mpls_port_id, mpls_port->encap_id, egress_intf);

    /* Set parameters for ingress */
    bcm_mpls_port_t_init(mpls_port);

    /* set port attribures */
    mpls_port->criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port->match_label = in_vc_label;
    if (mpls_termination_label_index_enable) {
        BCM_MPLS_INDEXED_LABEL_SET(mpls_port->match_label, in_vc_label, 1);
    }
    mpls_port->flags = BCM_MPLS_PORT_EGRESS_TUNNEL|BCM_MPLS_PORT_WITH_ID;
    mpls_port->flags |= is_device_or_above(unit, JERICHO2) ? 0 : (BCM_MPLS_PORT_COUNTED|BCM_MPLS_PORT_ENCAP_WITH_ID);
    mpls_port->flags2 = BCM_MPLS_PORT2_INGRESS_ONLY;
    if (is_device_or_above(unit, JERICHO2))
    {
        mpls_port->flags2 |= BCM_MPLS_PORT2_CROSS_CONNECT;
    }
    mpls_port->port = port;
    mpls_port->failover_id = failover_id;
    mpls_port->failover_port_id = protect_gport;
    mpls_port->failover_mc_group = protect_mc;
    mpls_port->mpls_port_id = 0x18805000;
    mpls_port->encap_id = 0x5000;

    rv = is__jericho2_a0_only(unit,&is_jr2_a0);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add INGRESS rv:%d\n", rv);
        return rv;
    }

    if (is_jr2_a0){
        mpls_port->flags2 |= BCM_MPLS_PORT2_STAT_ENABLE;
    }

    /* to create a p2p mpls port that is not connected to p2p vpn - use vpn=0 */
    rv = bcm_mpls_port_add(unit, 0, mpls_port);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add INGRESS\n");
        print rv;
        return rv;
    }

    printf("INGRESS: add port 0x%08x \n\r",mpls_port->mpls_port_id);

    vswitch_p2p_inlif_mpls_port_id = mpls_port->mpls_port_id;

    /* Set parameters for FEC */
    if (is_device_or_above(unit, JERICHO2)) {
        /** In JR2, FEC entry should be created by bcm_l3_egress_create with _INGRESS_ONLY flag.*/
        create_l3_egress_s l3eg;
        int egr_mpls_port_encap_id;
        BCM_L3_ITF_SET(egr_mpls_port_encap_id, BCM_L3_ITF_TYPE_LIF, BCM_GPORT_SUB_TYPE_LIF_VAL_GET(mpls_port->mpls_port_id));
        l3eg.out_tunnel_or_rif = egr_mpls_port_encap_id;
        l3eg.out_gport = port;

        rv = l3__egress_only_fec__create(unit,&l3eg);
        if (rv != BCM_E_NONE) {
            printf("Error, in l3__egress_only_encap__create ");
            print rv;
            return rv;
        }

        vswitch_p2p_fec_id = l3eg.fec_id;
        BCM_L3_ITF_FEC_TO_GPORT_FORWARD_GROUP(vswitch_p2p_fec_id, vswitch_p2p_fec_id);

    } else {
        forward_group_info.port = port;
        forward_group_info.encap_id = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(mpls_port->mpls_port_id); /* this is the outlif, configured by mpls_port with EGRESS_ONLY flag set */

        rv = mpls_port__forward_group__create(unit, &forward_group_info);
        if (rv != BCM_E_NONE) {
            printf("mpls_port__forward_group__create failed: %d \n", rv);
            return rv;
        }

        vswitch_p2p_fec_id = forward_group_info.mpls_port_forward_group_id;
    }

    if (verbose) {
        printf("FEC created: 0x%08x \n\r", forward_group_info.mpls_port_forward_group_id);
    }

    return BCM_E_NONE;
}


int 
run(int unit, int port1, int type1, int port2, int type2) {
    int port, i, failover_flags;
    int rv;
    int vlan;

    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__mpls_pipe_mode_exp_set\n");
        return rv;
    }
   
    cross_connect_info_init(port1, port2);  

    /* check impossible combinations */
    if (type1 == 2 && type2 == 2) {
        printf("Error, 2 mpls ports cannot be cross connected\n");
        return BCM_E_PORT;
    }

    /* When using new vlan translation mode, tpid and vlan actions and mapping must be configured manually */
    advanced_vlan_translation_mode = is_device_or_above(unit, JERICHO2) ? TRUE : soc_property_get(unit, "bcm886xx_vlan_translate_mode", 0);;
    if (advanced_vlan_translation_mode && (type1 == 1 || type2 == 1)) {

        port = cross_connect_info.port_1; 
        for (i = 0 ; i < 2 ; i++) {
            if (!is_device_or_above(unit, JERICHO2)) {
                port_tpid_init(port, 1, 1);
                rv = port_tpid_set(unit);
                if (rv != BCM_E_NONE) {
                    printf("Error, port_tpid_set with port_1\n");
                    print rv;
                    return rv;
                }
            } else {
                /** In JR2, TPID is global. Use port.llvp_profile for vlan classification in port.*/
                bcm_port_tpid_class_t port_tpid_class;
                bcm_port_tpid_class_t_init(&port_tpid_class);
                port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
                port_tpid_class.tpid1 = 0x8100;
                port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
                port_tpid_class.tag_format_class_id = 4;
                port_tpid_class.port = port;
                rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_port_tpid_class_set with port_2\n");
                    print rv;
                    return rv;
                }

                bcm_port_tpid_class_t_init(&port_tpid_class);
                port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY | BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
                port_tpid_class.tpid1 = 0x8100;
                port_tpid_class.tpid2 = 0x9100;
                port_tpid_class.tag_format_class_id = 16;
                port_tpid_class.port = port;
                rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
                if (rv != BCM_E_NONE) {
                    printf("Error, bcm_port_tpid_class_set with port_2\n");
                    print rv;
                    return rv;
                }
            }
            port = cross_connect_info.port_2;
        } 

        rv = vlan_translation_default_mode_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_translation_default_mode_init\n");
            return rv;
        }
    }

    cross_connect_info.protection_mc1 = 0;
    cross_connect_info.protection_mc2 = 0;
        
    switch(type1) {
    case 1:
        /* create vlan port. */
        vlan = cross_connect_info.outer_vlan_base;
        rv = vlan_port_create(unit,
                              0,
                              cross_connect_info.port_1, 
                              cross_connect_info.outer_vlan_base++, 
                              cross_connect_info.eg_vlan_base++, 
                              &(cross_connect_info.vlan_port_1), 
                              0, 
                              0,
                              cross_connect_info.protection_mc1);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_port_create with port %d\n", cross_connect_info.port_1);
            return rv;
        }
        if(verbose >= 1) {
            printf("created vlan_port_1 : 0x0%8x\n", cross_connect_info.vlan_port_1);
        }

        cross_connect_info.gports.port1 = cross_connect_info.vlan_port_1;

        /** Add the gport to vlan*/
        rv = bcm_vlan_gport_add(unit, vlan, cross_connect_info.port_1, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_gport_add with port %d\n", cross_connect_info.port_1);
            return rv;
        }

        break;

    case 2:
        /* create mpls port */
        rv = mpls_port_create_ingress_egress(unit, 
                              cross_connect_info.port_1, 
                              &(cross_connect_info.mpls_port_1), 
                              0, 
                              0,
                              cross_connect_info.protection_mc1);
        if (rv != BCM_E_NONE) {
            printf("Error, in mpls_port_create_ingress_egress with port %d\n", cross_connect_info.port_1);
            return rv;
        }
        cross_connect_info.gports.port1 = cross_connect_info.mpls_port_1.mpls_port_id;

        break;
    default:
        printf("Error, type1 cannot be <1 or >2\n");
        return BCM_E_PARAM;
    }

    switch(type2) {
    case 1:
        /* create vlan port. */
        vlan = cross_connect_info.outer_vlan_base;
        rv = vlan_port_create(unit,
                              0,
                              cross_connect_info.port_2, 
                              cross_connect_info.outer_vlan_base++, 
                              cross_connect_info.eg_vlan_base++, 
                              &(cross_connect_info.vlan_port_2), 
                              0, 
                              0,
                              cross_connect_info.protection_mc2);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_port_create with port %d\n", cross_connect_info.port_2);
            return rv;
        }
        if(verbose >= 1) {
            printf("created vlan_port_2 : 0x0%8x\n", cross_connect_info.vlan_port_2);
        }

        cross_connect_info.gports.port2 = cross_connect_info.vlan_port_2;

        /** Add the gport to vlan*/
        rv = bcm_vlan_gport_add(unit, vlan, cross_connect_info.port_2, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, in bcm_vlan_gport_add with port %d\n", cross_connect_info.port_2);
            return rv;
        }

        break;

    case 2:
        /* create mpls port */
        rv = mpls_port_create_ingress_egress(unit, 
                              cross_connect_info.port_2, 
                              &(cross_connect_info.mpls_port_1), 
                              0, 
                              0,
                              cross_connect_info.protection_mc2);
        if (rv != BCM_E_NONE) {
            printf("Error, in mpls_port_create with port %d\n", cross_connect_info.port_2);
            return rv;
        }

        cross_connect_info.gports.port2 = cross_connect_info.mpls_port_1.mpls_port_id;

        break;
    default:
        printf("Error, type2 cannot be <1 or >2\n");
        return BCM_E_PARAM;
    }



    /* cross connect the 2 ports -> */
    cross_connect_info.gports.port1 = vswitch_p2p_inlif_mpls_port_id;
    cross_connect_info.gports.port2 = vswitch_p2p_vlan_port_id;

    if(verbose >= 1) {
        printf("connect port1:0x0%8x with port2:0x0%8x \n", cross_connect_info.gports.port1, cross_connect_info.gports.port2);
    }

    /* */
    cross_connect_info.gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    rv = bcm_vswitch_cross_connect_add(unit, &cross_connect_info.gports);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_add\n");
        print rv;
        return rv;
    }

    /* cross connect the 2 ports <- */
    cross_connect_info.gports.port1 = vswitch_p2p_vlan_port_id;
    cross_connect_info.gports.port2 = vswitch_p2p_fec_id;

    if(verbose >= 1) {
        printf("connect port1:0x0%8x with port2:0x0%8x \n", cross_connect_info.gports.port1, cross_connect_info.gports.port2);
    }

    /* */
    cross_connect_info.gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    rv = bcm_vswitch_cross_connect_add(unit, &cross_connect_info.gports);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_add\n");
        print rv;
        return rv;
    }

    return BCM_E_NONE;
}


void
print_gport(bcm_gport_t gport) {
    if (BCM_GPORT_IS_MPLS_PORT(gport)) {
        printf("MPLS port id = %d\n", BCM_GPORT_MPLS_PORT_ID_GET(gport));
    }
    else if (BCM_GPORT_IS_VLAN_PORT(gport)) {
        printf("VLAN port id = %d\n", BCM_GPORT_VLAN_PORT_ID_GET(gport));
    }
    else {
        print gport;
    }
}

/* call back for traverse */
int
call_back(int unit,  bcm_vswitch_cross_connect_t *t, int* ud) {
    printf("traverse call back no. %d\n", (*ud)++);
    printf("port 1: ");
    print_gport(t->port1);
    printf("port 2: ");
    print_gport(t->port2);
    printf("\n");
    return BCM_E_NONE;
}

int
traverse_run(int unit) {

    int rv, traverse_no = 1;

    /* add another vlan port, that is not cross-connected to any port */
    rv = vlan_port_create(unit, 
                          cross_connect_info.port_5, 
                          cross_connect_info.outer_vlan_base++, 
                          cross_connect_info.eg_vlan_base++, 
                          &(cross_connect_info.vlan_port_2), 
                          0, 
                          0,
                          0);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_port_create with port %d\n", cross_connect_info.port_5);
        return rv;
    }

    /* traverse - print all the cross-connected ports */
    rv = bcm_vswitch_cross_connect_traverse(unit, call_back, &traverse_no);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_cross_connect_traverse\n");
        return rv;
    }

    return BCM_E_NONE;
}

int 
delete_all(int unit) {

    int rv, traverse_no = 1;

    rv = bcm_vswitch_cross_connect_delete_all(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_cross_connect_delete_all\n");
        print rv;
        return rv;
    }

    rv = bcm_vswitch_cross_connect_traverse(unit, call_back, &traverse_no);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_cross_connect_traverse\n");
        return rv;
    }

    return BCM_E_NONE;
}

int get_vswitch_p2p_encap_id(){
    return vswitch_p2p_encap_id;
}
 

