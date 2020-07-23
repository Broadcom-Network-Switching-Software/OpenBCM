/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~OAM test~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 *
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File: cint_oam_up_mep_over_vpls.c
 * Purpose: Example of creating Ethernet OAM endpoint connected to VPLS tunnel.
 *
 * Usage:
 * 
cd ../../../../src/examples/dpp
cint cint_utils_multicast.c
cint cint_utils_vlan.c
cint cint_utils_mpls_port.c
cint cint_utils_oam.c
cint cint_port_tpid.c
cint cint_advanced_vlan_translation_mode.c
cint cint_l2_mact.c
cint cint_vswitch_metro_mp.c
cint cint_multi_device_utils.c
cint cint_queue_tests.c 
cint cint_oam_acceleration_advanced_features.c
cint cint_oam.c
cint cint_oam_up_mep_over_vpls.c

cint
int unit =0;
int port_1=200, port_2 = 202, port3 = 203;
print oam_run_up_mep_over_vpls_with_defaults(unit,port_1,port_2);

*/
 
/* Globals - MAC addresses , ports & gports*/

    bcm_mac_t dst_up_mac = {0x00, 0x20, 0x8f, 0x1e, 0x24, 0x06};
    bcm_mac_t src_up_mac = { 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
    bcm_mac_t s_mac = {0x00, 0x00, 0x00, 0x00, 0x02, 0x22};
    bcm_mac_t myMac = {0x00,0x20,0x8f,0x1e,0x24,0x11};
    bcm_mac_t egrDa = {0x00,0x20,0x8f,0x1e,0x24,0x22};

    int lm_counter_base_id;
    int out_tunnel_id;
    int tunnel_egr_if;
    int mpls_id;
    int g_snoop_to_cpu_gport;

/**
 * Create LSP for cross conected ports
 * 
 * @param unit 
 * @param outPort 
 * 
 * @return int 
 */
int oam_create_lsp_for_ccp(int unit, int outPort) {

    bcm_vlan_t outVlan = 1028;
    bcm_vlan_t vsi = 20;
    int flags = 0;
    bcm_mpls_label_t outLabel = 3;
    bcm_l3_intf_t egrL3intf;    
    bcm_error_t rv;
    bcm_l2_station_t myMacMsb;   
    bcm_mpls_egress_label_t  egrLable;  
    int stationId = 0;   
    create_l3_egress_s l3Egr;
    int egrIfId = 0;
    int sampling_ratio = 0;
 
    bcm_vlan_port_t vlan_port_1;
    
    /*1 set egress vlan configuration*/
    rv = bcm_port_class_set( unit, outPort, bcmPortClassId, outPort);
    if (rv != BCM_E_NONE) {
        printf("Error,bcm_port_class_set(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_vswitch_create_with_id(unit, vsi);
    if (rv != BCM_E_NONE) {
        printf("Error,bcm_vswitch_create_with_id(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    
    bcm_vlan_port_t_init(&vlan_port_1);
    vlan_port_1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port_1.port = outPort;
    vlan_port_1.vsi = vsi;
    vlan_port_1.match_vlan = outVlan;
    vlan_port_1.egress_vlan = outVlan;
    vlan_port_1.flags =  BCM_VLAN_PORT_INNER_VLAN_PRESERVE | BCM_VLAN_PORT_OUTER_VLAN_PRESERVE ;
    rv = bcm_vlan_port_create(unit, &vlan_port_1);
    if (rv != BCM_E_NONE) {
        printf("Error,bcm_vlan_port_create(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    /* In advanced vlan translation mode, bcm_vlan_port_create does not create ingress / egress
    action mapping. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        rv = vlan_translation_vlan_port_create_to_translation(unit, &vlan_port_1);
        if (rv != BCM_E_NONE) {
            printf("Error: vlan_translation_vlan_port_create_to_translation\n");
        }
    }

    rv = bcm_vswitch_port_add(unit,vsi,vlan_port_1.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error,bcm_vswitch_port_add(%s) \n", bcm_errmsg(rv));
        return rv;
    }
        
        
    /* create l3intf(inRif and OutRif), used to term ethernet layer*/
    bcm_l3_intf_t_init(&egrL3intf);
    sal_memcpy(egrL3intf.l3a_mac_addr, myMac, sizeof(myMac));
    egrL3intf.l3a_vid = vsi;
    rv = bcm_l3_intf_create(unit, &egrL3intf);
    if(rv != BCM_E_NONE){
        printf("error, bcm_l3_intf_create %d failed, rv = %s \n", outVlan, bcm_errmsg(rv));
        return rv;    
    }   

    bcm_mpls_egress_label_t_init(&egrLable);
    egrLable.label = 3;
    egrLable.action = BCM_MPLS_EGRESS_ACTION_PUSH;
    egrLable.flags = BCM_MPLS_EGRESS_LABEL_TTL_SET | BCM_MPLS_EGRESS_LABEL_EXP_COPY;
    egrLable.flags  |=  BCM_MPLS_EGRESS_LABEL_ACTION_VALID;    
    egrLable.ttl = 25;
    egrLable.exp = 1;
    egrLable.l3_intf_id = egrL3intf.l3a_intf_id;
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, &egrLable);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_tunnel_initiator_create label %d failed,rv = %s \n", outLabel, bcm_errmsg(rv));
        return rv;  
    }

    /*create FEC object*/
    sal_memcpy(l3Egr.next_hop_mac_addr, egrDa, sizeof(egrDa));
    l3Egr.out_gport = outPort;
    l3Egr.out_tunnel_or_rif = egrLable.tunnel_id;
    l3Egr.l3_flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
    l3Egr.fec_id = egrIfId;

    rv = l3__egress__create(unit,&l3Egr);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create (vlan %d, port %d, intf %d) failed, rv=%s \n",
               outVlan, outPort,l3Egr.out_tunnel_or_rif, bcm_errmsg(rv));
        return rv;
    }

    out_tunnel_id = l3Egr.arp_encap_id;
    tunnel_egr_if = l3Egr.fec_id;
    return rv;
}

/**
 * Create PW port 
 * 
 * @param unit 
 * @param outPort 
 * 
 * @return int 
 */
int oam_pw_port_create(int unit, int outPort) {

    bcm_error_t rv;
    bcm_mpls_port_t mpls_port;
    int mpls_label_indexed_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
    int is_qux;

    rv = is_qumran_ux_only(unit, &is_qux);
    if (rv != BCM_E_NONE) {
        printf("Failed(%d) is_qumran_ax_only\n", rv);
        return rv;
    }

    bcm_mpls_port_t_init(&mpls_port);

    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, (is_qux ? (16*1024-50) : (32*1024-50)));
    mpls_port.flags |= BCM_MPLS_PORT_WITH_ID;
    mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port.match_label = 5000; /* Matching Label*/
    if (mpls_label_indexed_enable) {
        BCM_MPLS_INDEXED_LABEL_SET(mpls_port.match_label, 5000, 1);
    }
    mpls_port.egress_tunnel_if = tunnel_egr_if; 
    mpls_port.flags |= BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.port = outPort;
    mpls_port.egress_label.label = 6000; /* Out Label*/
    mpls_port.vccv_type = bcmMplsPortControlChannelTtl;
    mpls_port.flags2 |= BCM_MPLS_PORT2_INGRESS_WIDE;
    mpls_port.flags |= BCM_MPLS_PORT_COUNTED;
    mpls_port.flags &= ~BCM_MPLS_PORT_NETWORK; 
    mpls_port.network_group_id = 0;

    /* Create PW port*/
    rv = bcm_mpls_port_add(unit,0, &mpls_port);       
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    mpls_id = mpls_port.mpls_port_id;
    return rv;
}


/**
 * Function creates an accelerated Up-MEP with a 
 * matching remote endpoint. 
 *  
 * @author atanasov (2/8/2016)
 * 
 * @param unit 
 * @param port 
 * 
 * @return int 
 */
int oam_create_acc_up_mep(int unit, int port) {

    bcm_error_t rv;
    bcm_oam_endpoint_info_t oam_endpoint_info;
    bcm_oam_endpoint_info_t rmep_info;

    /*Endpoint will be created on LIF. */
    rv = set_counter_source_and_engines(unit,&lm_counter_base_id,port);
    BCM_IF_ERROR_RETURN(rv); 

    /* Set the group for the MEP with the short name */
    bcm_oam_group_info_t_init(&group_info_short_ma);
    sal_memcpy(group_info_short_ma.name, short_name, BCM_OAM_GROUP_NAME_LENGTH);
    rv = bcm_oam_group_create(unit, &group_info_short_ma);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    
    /* Adding acc MEP - upmep */
    bcm_oam_endpoint_info_t_init(&oam_endpoint_info);

    /*TX*/
    oam_endpoint_info.type = bcmOAMEndpointTypeEthernet;
    oam_endpoint_info.group =group_info_short_ma.id;
    oam_endpoint_info.level = 3;
    mep_acc_info.tx_gport = BCM_GPORT_INVALID; /*Up MEP requires gport invalid.*/
    oam_endpoint_info.name = 123;
    oam_endpoint_info.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_10MS;
    oam_endpoint_info.flags |= BCM_OAM_ENDPOINT_UP_FACING; 
    oam_endpoint_info.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
    oam_endpoint_info.flags |= BCM_OAM_ENDPOINT_WITH_ID;
    oam_endpoint_info.id = (is_device_or_above(unit, JERICHO)) ? 1 : 0;

    oam_endpoint_info.vlan = 3333;
    oam_endpoint_info.pkt_pri = oam_endpoint_info.pkt_pri = 0 +(2<<1);  /*dei(1bit) + (pcp(3bit) << 1) */
    oam_endpoint_info.outer_tpid = 0x8100;
    oam_endpoint_info.inner_vlan = 0;
    oam_endpoint_info.inner_pkt_pri = 0;
    oam_endpoint_info.inner_tpid = 0;
    oam_endpoint_info.int_pri = 3 +(1<<2);
    oam_endpoint_info.timestamp_format = bcmOAMTimestampFormatIEEE1588v1; 
    if (device_type >= device_type_arad_plus) {
        /* Take RDI only from scanner*/
        mep_acc_info.flags2 =  BCM_OAM_ENDPOINT_FLAGS2_RDI_FROM_RX_DISABLE;
    }

    sal_memcpy(oam_endpoint_info.src_mac_address, src_up_mac, 6);
    sal_memcpy(oam_endpoint_info.dst_mac_address, dst_up_mac, 6);
    oam_endpoint_info.gport = gport1;

    printf("Creating accelerated Up-MEP.\n");
    rv = bcm_oam_endpoint_create(unit, oam_endpoint_info);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    printf("created MEP with id %d\n", oam_endpoint_info.id);
    ep1.id = oam_endpoint_info.id;
    ep1.gport = oam_endpoint_info.gport;

/*
* Adding Remote MEP
*/
    bcm_oam_endpoint_info_t_init(rmep_info);
    rmep_info.name = 0xff;
    rmep_info.local_id = oam_endpoint_info.id;
    rmep_info.type = bcmOAMEndpointTypeEthernet;
    rmep_info.ccm_period = 0;
    rmep_info.flags |= BCM_OAM_ENDPOINT_REMOTE | BCM_OAM_ENDPOINT_WITH_ID;
    rmep_info.loc_clear_threshold = 1;
    rmep_info.id = oam_endpoint_info.id;

    printf("bcm_oam_endpoint_create RMEP\n");
    rv = bcm_oam_endpoint_create(unit, rmep_info);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    ep1.rmep_id = rmep_info.id;
    printf("created RMEP with id %d\n", rmep_info.id);

    return rv;
}

/**
 * Function create Ethernet OAM endpoint connected to VPLS tunnel.
 * 
 * @author atanasov (2/11/2016)
 * 
 * @param unit 
 * @param port  -  VPLS port
 * @param port2  -  Ethernet + OAM endpoint port
 * 
 * @return int 
 */
int oam_up_mep_over_vpls_run_with_defaults_dpp(int unit, int port, int port2) {

    bcm_error_t rv;
    bcm_vswitch_cross_connect_t gports = {0};
    bcm_vswitch_cross_connect_t_init(&gports);
    bcm_vlan_port_t vp;
    
    /* When using new vlan translation mode, vlan actions and mapping must be configured manually */
    advanced_vlan_translation_mode = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);
    if (advanced_vlan_translation_mode){
        port_tpid_init(port, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with port_1\n");
            print rv;
            return rv;
        }

        port_tpid_init(port2, 1, 1);
        rv = port_tpid_set(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set with port_1\n");
            print rv;
            return rv;
        }


        rv = vlan_translation_default_mode_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_translation_default_mode_init\n");
            return rv;
        }
    }

    printf("Registering OAM events\n");
    rv = register_events(unit);
    if (rv != BCM_E_NONE) {
        printf("Error,register_events(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    rv = oam_create_lsp_for_ccp(unit,port);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    rv = oam_pw_port_create(unit,port);    
    if (rv != BCM_E_NONE) {
        printf("Error,oam_pw_port_create(%s) \n",bcm_errmsg(rv));
        return rv;
    }   

    /* Create vlan-port */
    bcm_vlan_port_t_init(&vp);  
    vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vp.port = port2; 
    vp.match_vlan = 3333;    
    vp.egress_vlan = 3333;
    rv = bcm_vlan_port_create(unit,&vp);    
    if (rv != BCM_E_NONE) {
        printf("Error,bcm_vlan_port_create(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    /* In advanced vlan translation mode, bcm_vlan_port_create does not create ingress / egress
    action mapping. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        rv = vlan_translation_vlan_port_create_to_translation(unit, &vp);
        if (rv != BCM_E_NONE) {
            printf("Error: vlan_translation_vlan_port_create_to_translation\n");
        }
    }
    gport1 = vp.vlan_port_id;
    print gport1;
    
    bcm_l2_addr_t addr;
    bcm_l2_addr_t_init(&addr, s_mac, 3333);
    BCM_GPORT_LOCAL_SET(addr.port, port); 
    rv = bcm_l2_addr_add(unit, &addr);
    if (rv != BCM_E_NONE) {
        printf("Error,bcm_l2_addr_add(%s) \n",bcm_errmsg(rv));
        return rv;
    }
        
    gports.port1 = gport1;
    gports.port2 = mpls_id;
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE) {
        printf("Error,bcm_vswitch_cross_connect_add(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    rv = oam_create_acc_up_mep(unit,port);
    if (rv != BCM_E_NONE) {
        printf("Error,oam_create_acc_up_mep(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    return rv;
}

