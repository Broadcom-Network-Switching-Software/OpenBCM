/* 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/* 
    This test demonstrates trill transparent serve for unicast/multicast ingress, egress and transit.
 
    It creates three trill ports where trill_port_local is the local Rbridge.
 
    Ingress: A l2_addr is added to forward the packet to trill_port_rb1.
        The ingress packet is encapsulated with an outer LL header defined in l3_if1, egr_obj_rb1.
        and a trill header: ing_nick=trill_port_local egr_nick=trill_port_rb1.
  
    Ttransit:
        In the trill header TTL is decreased, and all other fields are unchanged.  
 
    Egress: packet is decapsulated.

       Network diagram:
 
       ----------    vid_1 (host)
       | Host 1 | ---------
       ----------          |    ------------------
                           |    |    Local RB    |
       ----------          |    |                |
       | RB1    | --------------| port-1         |
       ----------          |    |                |
                           |    |                |
       ----------          |    ------------------
       | RB2    |----------|
       ----------    vid_2_desig (designated vlan)

    Trill transparent service only support for single tag and untag ethernet packet, the IVE and EVE action change is as below:
    
    single tag IVE action: replace outer and copy inner.
    single tag EVE action: use EGRESS_REMOVE_TAGS_PUSH_1_PROFILE profile to remove two tags and push one tag.
    
    untag IVE action: add outer and none inner.
    untag EVE action: use EGRESS_REMOVE_TAGS_PROFILE profile to remove one tag.

a) Trill unicast untag fgl service:

    Ingress scenario: 
        Send: 
            eth : c_dmac, c_smac
        Recv: 
            eth:    remote_mac_rb1, trill_vsi_mac_2, vid_2_desig
            trill:  trill_ethertype, hopcount, nickname_rb1, nickname_rb_local
            eth:    c_dmac, c_smac, FGL_outer_vid, FGL_inner_vid
  
    Transit scenario:
        Send: 
            eth:    trill_vsi_mac_2, remote_mac_rb2, vid2 
            trill:  trill_ethertype, hopcount,nickname_rb2, nickname_rb1
            eth:    host2_mac, host3_mac, FGL_outer_vid, FGL_inner_vid
        Recv:
            eth:    remote_mac_rb2, trill_vsi_mac_2, vid_2_desig
            trill:  0x2109, hopcount-1,nickname_rb2, nickname_rb1
            eth:    host2_mac, host3_mac, FGL_outer_vid, FGL_inner_vid        
     
    Egress scenario:
        Send: 
            eth:    trill_vsi_mac_2, remote_mac_rb1, vid_2_desig 
            trill:  trill_ethertype, hopcount,nickname_rb_local, nickname_rb1
            eth:    host1_mac, host4_mac, FGL_outer_vid, FGL_inner_vid
        Recv:
            eth:    host1_mac, host4_mac

b) Trill transparent service unicast untag fgl service:

    Ingress scenario: 
        Send: 
            eth : c_dmac, c_smac
        Recv: 
            eth:    remote_mac_rb1, trill_vsi_mac_2, vid_2_desig
            trill:  trill_ethertype, hopcount, nickname_rb1, nickname_rb_local
            eth:    c_dmac, c_smac, FGL_outer_vid
  
    Transit scenario:
        Send: 
            eth:    trill_vsi_mac_2, remote_mac_rb2, vid2 
            trill:  trill_ethertype, hopcount,nickname_rb2, nickname_rb1
            eth:    host2_mac, host3_mac, FGL_outer_vid
        Recv:
            eth:    remote_mac_rb2, trill_vsi_mac_2, vid_2_desig
            trill:  0x2109, hopcount-1,nickname_rb2, nickname_rb1
            eth:    host2_mac, host3_mac, FGL_outer_vid        
     
    Egress scenario:
        Send: 
            eth:    trill_vsi_mac_2, remote_mac_rb1, vid_2_desig 
            trill:  trill_ethertype, hopcount,nickname_rb_local, nickname_rb1
            eth:    host1_mac, host4_mac, FGL_outer_vid
        Recv:
            eth:    host1_mac, host4_mac

c) Trill transparent service unicast fgl service:

    Ingress scenario: 
        Send: 
            eth : c_dmac_tts, c_smac, vid_tts
        Recv: 
            eth:    remote_mac_rb1, trill_vsi_mac_2, vid_2_desig
            trill:  trill_ethertype, hopcount, nickname_rb1, nickname_rb_local
            eth:    c_dmac_tts, c_smac, FGL_outer_vid, vid_tts
  
    Transit scenario:
        Send: 
            eth:    trill_vsi_mac_2, remote_mac_rb2, vid2 
            trill:  trill_ethertype, hopcount,nickname_rb2, nickname_rb1
            eth:    host2_mac, host3_mac, FGL_outer_vid, vid_tts
        Recv:
            eth:    remote_mac_rb2, trill_vsi_mac_2, vid_2_desig
            trill:  0x2109, hopcount-1,nickname_rb2, nickname_rb1
            eth:    host2_mac, host3_mac, FGL_outer_vid, vid_tts        
     
    Egress scenario:
        Send: 
            eth:    trill_vsi_mac_2, remote_mac_rb1, vid_2_desig 
            trill:  trill_ethertype, hopcount,nickname_rb_local, nickname_rb1
            eth:    host2_mac, host4_mac, FGL_outer_vid, vid_tts
        Recv:
            eth:    host2_mac, host4_mac, vid_tts

d) Trill transparent service multicast transmit fecless fgl service:

    Ingress scenario: 
        Send: 
            eth : c_dmac_tts, c_smac, vid1
        Recv: 
            eth:    remote_mac_rb1, trill_vsi_mac_2, vid_2_desig
            trill:  trill_ethertype, mc_flag, hopcount, dist_tree_1, nickname_rb_local
            eth:    c_dmac_tts, c_smac, FGL_outer_vid, vid_tts
        Recv: 
            eth:    remote_mac_rb2, trill_vsi_mac_2, vid_2_desig
            trill:  trill_ethertype, mc_flag, hopcount, dist_tree_1, nickname_rb_local
            eth:    c_dmac_tts, c_smac, FGL_outer_vid, vid_tts
        Recv: 
            eth:    c_dmac_tts, c_smac, vid1
  
    Transit scenario:
        Send: 
            eth:    trill_vsi_mac_2, remote_mac_rb2, vid2 
            trill:  trill_ethertype, mc_flag, hopcount, dist_tree_1, nickname_rb_loca
            eth:    host2_mac, host3_mac, FGL_outer_vid, vid_tts
        Recv:
            eth:    remote_mac_rb2, trill_vsi_mac_2, vid_2_desig
            trill:  0x2109, hopcount-1,nickname_rb2, nickname_rb1
            eth:    host2_mac, host3_mac, FGL_outer_vid, vid_tts      
        Recv:
            eth:   host2_mac, host3_mac, vid1
        Recv:
            eth:   host2_mac, host3_mac, vid1

*/ 

bcm_mac_t c_dmac_tts = {0x00, 0x00, 0x00, 0x00, 0xcc, 0x55};
bcm_vlan_t vid_tts = 10;
bcm_multicast_t l2mc_group_tts = 7172;
int trill_uni_untag_configured_fgl = FALSE;
int trill_uni_configured_fgl_tts = FALSE;

/*
 *Allocate the LIF, map it to VSI and set the VLAN translation..
 */
int
pvid_port_set(int unit, int port, bcm_vlan_t pvid, bcm_vlan_t vsi) {
    int rv = 0;
    bcm_vlan_port_t  port_vlan;
    int trill_mode, is_fgl = 0;
    bcm_vlan_action_set_t action;

    trill_mode = soc_property_get(unit , "trill_mode",0);
    is_fgl = (trill_mode == 2);

    rv =  bcm_port_untagged_vlan_set(unit, port, pvid);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_untagged_vlan_set unit %d, in_port %d, vid %d, rv %d\n", unit, in_port, p_vid, rv);
        return rv;
    }
    
    /* Add a <VD,pvid> -> lif mapping
       port_vlan.vlan_port_id is filled with a gport, which is a SW handle to the lif.
       HW constraint in ARAD: lif must be equal to vsi. Use ENCAP_WITH_ID,
       with encap_id = vsi to force this. */
    bcm_vlan_port_t_init(port_vlan);
    port_vlan.criteria = BCM_VLAN_PORT_MATCH_PORT_INITIAL_VLAN;
    port_vlan.port = port;
    port_vlan.match_vlan = pvid;
    port_vlan.vsi = vsi;
    if (is_fgl) {
        port_vlan.flags = BCM_VLAN_PORT_INNER_VLAN_PRESERVE|BCM_VLAN_PORT_OUTER_VLAN_PRESERVE;
    } else {
        port_vlan.flags = BCM_VLAN_PORT_ENCAP_WITH_ID|BCM_VLAN_PORT_INNER_VLAN_PRESERVE|BCM_VLAN_PORT_OUTER_VLAN_PRESERVE;
        port_vlan.encap_id = vsi;
    }

    rv = bcm_vlan_port_create(unit,&port_vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create\n");
        print port_vlan;
        return rv;
    }
  
    /* Map LIF -> VSI */
    rv = bcm_vswitch_port_add(unit, vsi, port_vlan.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_port_add\n");
        return rv;
    }
    
    if (advanced_vlan_translation_mode) { 
        rv = trill_advanced_vlan_translation_map_local_vsi_to_global_vsi(unit, vsi, port_vlan.egress_vlan, port_vlan.vlan_port_id, trill_mode);
        if (rv != BCM_E_NONE) {
            printf("Error, in trill_advanced_vlan_translation_map_local_vsi_to_global_vsi\n");
            print port_vlan;
            return rv;
        }
    } else {
	    /* Single tag ot - remove outer (S-Tag) */ 
        action.ot_outer = bcmVlanActionDelete;
		action.ot_inner = bcmVlanActionNone;
		/* Single tag it - remove inner (C-Tag) */ 
		action.it_outer = bcmVlanActionNone;
		action.it_inner = bcmVlanActionDelete;

        /* Double tag - remove outer (S-Tag) and inner (C-Tag) */
        action.dt_outer = bcmVlanActionDelete;        
        action.dt_inner = bcmVlanActionDelete;
        
        rv = bcm_vlan_translate_egress_action_add(unit, port_vlan.vlan_port_id, BCM_VLAN_NONE, BCM_VLAN_NONE, &action);
        if (rv != BCM_E_NONE) {
          printf("Error, in bcm_vlan_translate_egress_action_add %s\n", bcm_errmsg(rv));
          return rv;
        }
    }

    return rv;
}

int
pvid_port_unset(int unit, int port, bcm_vlan_t pvid, bcm_vlan_t vsi) {
    int rv = 0;
    bcm_vlan_port_t  port_vlan;
    
    bcm_vlan_port_t_init(port_vlan);
    port_vlan.match_vlan = pvid;
    port_vlan.port = port;
    port_vlan.criteria = BCM_VLAN_PORT_MATCH_PORT_INITIAL_VLAN;

    rv =  bcm_vlan_port_find(unit, &port_vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_find\n");
        print port_vlan;
        return rv;
    }
    /* Unmap LIF -> VSI */
    rv = bcm_vswitch_port_delete(unit, vsi, port_vlan.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_port_delete\n");
        return rv;
    }

    rv = bcm_vlan_port_destroy(unit,port_vlan.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_destroy\n");
        print port_vlan;
        return rv;
    }
    
    return rv;
}


/*
 *Create trill unicast untag fgl service.
 */
int
trill_uni_untag_fgl(int unit, int double_tag, int is_tts) {

    int rv = BCM_E_NONE;
    int vsi1 = vid_1;
    bcm_trill_vpn_config_t              vpn_config;
    bcm_vlan_action_set_t               action;
    bcm_l2_addr_t                       l2_addr;
    bcm_vlan_t  vsi_2_desig;

    vsi_2_desig = vid_2_desig; 

    if (trill_uni_untag_configured_fgl) {
        return rv;
    }
    printf("trill_uni_FGL %d %d\n",port_1, port_2);
    /* Initialize trill module (SW related databases) */
    rv = bcm_trill_init(unit);
    if ((rv != BCM_E_NONE ) &&( rv != BCM_E_INIT)) {
        printf("Error, in bcm_trill_init\n");
        return rv;
    }    
    rv = global_config(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in global_config\n");
        return rv;
    }
    rv = port_config(unit, port_1, vid_2_desig);
    if (rv != BCM_E_NONE) {
        printf("Error, in port_config: $port_1\n");
        return rv;
    }
    
    rv = port_config_FGL(unit, port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in port_config_FGL: port_1\n");
        return rv;
    }

    /* In advanced vlan translation mode, default vlan translation actions are not created in the device.
       We call this function to compensate. If the device is in normal vlan translation mode, it does nothing. */
    rv = trill_advanced_vlan_translation_actions_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in trill_advanced_vlan_translation_actions_init\n");
        return rv;
    }

    rv = port_config(unit, port_2, vid_2_desig);
    if (rv != BCM_E_NONE) {
        printf("Error, in port_config: $port_2\n");
        return rv;
    }    

    rv = port_config_FGL(unit, port_2);
    if (rv != BCM_E_NONE) {
        printf("Error, in port_config_FGL: port_2\n");
        return rv;
    }

    rv = bcm_vswitch_create_with_id(unit, vsi1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_create_with_id\n");
        return rv;
    } 
    printf("Created vswitch id %d\n",vsi1);

    rv = vlan_init(unit, vid_2_desig);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_init\n");
        return rv;
    }

    rv = vlan_port_init(unit, vid_2_desig, port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_init\n");
        return rv;
    }

    rv = vlan_port_init(unit, vid_2_desig, port_2);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_init\n");
        return rv;
    }  
    vpn_config.vpn = vsi1;
    vpn_config.high_vid = FGL_outer_vid;
    if (double_tag && !is_tts) {
        vpn_config.low_vid = FGL_inner_vid;
    } else {
        vpn_config.low_vid = BCM_VLAN_INVALID;
    }
    if (is_tts) {
        vpn_config.flags = BCM_BCM_TRILL_VPN_NETWORK | BCM_BCM_TRILL_VPN_ACCESS | BCM_TRILL_VPN_TRANSPARENT_SERVICE;
    } else {
        vpn_config.flags = BCM_BCM_TRILL_VPN_NETWORK | BCM_BCM_TRILL_VPN_ACCESS;
    }
    
    rv = bcm_trill_vpn_create(unit, &vpn_config);
    if (rv != BCM_E_NONE) {
       printf("Error, in bcm_trill_vpn_create\n");
       print vpn_config;
       return rv;
    }
    printf("Created VPN id %d , high_vid=%d, low_vid=%d\n",vsi1, vpn_config.high_vid, vpn_config.low_vid);
    
    rv = pvid_port_set(unit, port_1, port_1, vsi1); 
    if (rv != BCM_E_NONE) {
       printf("Error, in vlan_port_set: vsi=%d\n",vsi1 );
       return rv;
    }

    rv = l3_intf_create(unit, trill_vsi_mac_2, vsi_2_desig, &l3_intf_id);
    if (rv != BCM_E_NONE) {
       printf("Error, in l3_intf_create: vsi=%d\n",vsi_2_desig );
       return rv;
    }

    /* Add TRILL Virtual Port for local RB (configures my nickname) */
    rv = config_local_rbridge(unit, nickname_rb_local, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, in config_local_rbridge: vsi=$vsi_1_bridge\n");
        return rv;
    }
    printf("configured local bridge %d\n", nickname_rb_local );

    rv = add_remote_rbridge(unit, remote_mac_rb1, vsi_2_desig, port_1, nickname_rb1, 0, &trill_port_rb1);
    if (rv != BCM_E_NONE) {
        printf("Error, in add_remote_rbridge: vsi=%d\n", vsi1);
        return rv;
    }
    
    rv = add_remote_rbridge(unit, remote_mac_rb2, vsi_2_desig, port_2, nickname_rb2, 0, &trill_port_rb2);
    if (rv != BCM_E_NONE) {
        printf("Error, in add_remote_rbridge: vsi=%d\n", vsi1 );
        return rv;
    }
       
    /* Add L2 entry for Customer packet to TRILL tunnel:
       Adds an entry to the LEM with <c_dmac, nickname (taken from trill_port)> -> FEC */
    bcm_l2_addr_t_init(&l2_addr, c_dmac, vsi1);
    l2_addr.port = trill_port_rb1.trill_port_id;

    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l2_addr_add\n");
        print l2_addr;
        return rv;
    }

    /* Add L2 entry for Customer packet to TRILL tunnel: for egress rb flow
       Adds an entry to the LEM with <da= host1_mac -> port1 */
    bcm_l2_addr_t_init(&l2_addr, host1_mac, vsi1);
    l2_addr.port = port_1;
 
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l2_addr_add\n");
        print l2_addr;
        return rv;
    }

    /* Add L2 entry for Customer packet to TRILL tunnel: for bridge flow
       Adds an entry to the LEM with <da= host4_mac -> port1 */
    bcm_l2_addr_t_init(&l2_addr, host4_mac, vsi1);
    l2_addr.port = port_2;
 
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l2_addr_add\n");
        print l2_addr;
        return rv;
    }

    trill_uni_untag_configured_fgl = TRUE;

    return rv;
}

/*
 *Create trill transparent unicast fgl service.
 */

int
trill_uni_fgl_tts(int unit, int double_tag) {

    int rv = BCM_E_NONE;
    int vsi1 = vid_1;
    int vsi2 = vid_tts;
    bcm_trill_vpn_config_t              vpn_config;
    bcm_vlan_action_set_t               action;
    bcm_l2_addr_t                       l2_addr;
    bcm_vlan_t  vsi_2_desig;

    vsi_2_desig = vid_2_desig; 

    if (trill_uni_configured_fgl_tts) {
        return rv;
    }
    printf("trill_uni_tts_FGL %d %d\n",port_1, port_2);
    /* Initialize trill module (SW related databases) */
    rv = bcm_trill_init(unit);
    if ((rv != BCM_E_NONE ) &&( rv != BCM_E_INIT)) {
        printf("Error, in bcm_trill_init\n");
        return rv;
    }    
    rv = global_config(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in global_config\n");
        return rv;
    }
    rv = port_config(unit, port_1, vid_2_desig);
    if (rv != BCM_E_NONE) {
        printf("Error, in port_config: $port_1\n");
        return rv;
    }
    
    rv = port_config_FGL(unit, port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in port_config_FGL: port_1\n");
        return rv;
    }

    /* In advanced vlan translation mode, default vlan translation actions are not created in the device.
       We call this function to compensate. If the device is in normal vlan translation mode, it does nothing. */
    rv = trill_advanced_vlan_translation_actions_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in trill_advanced_vlan_translation_actions_init\n");
        return rv;
    }

    rv = port_config(unit, port_2, vid_2_desig);
    if (rv != BCM_E_NONE) {
        printf("Error, in port_config: $port_2\n");
        return rv;
    }    

    rv = port_config_FGL(unit, port_2);
    if (rv != BCM_E_NONE) {
        printf("Error, in port_config_FGL: port_2\n");
        return rv;
    }

    rv = bcm_vswitch_create_with_id(unit, vsi1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_create_with_id\n");
        return rv;
    } 
    printf("Created vswitch id %d\n",vsi1);

    rv = vlan_init(unit, vid_2_desig);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_init\n");
        return rv;
    }

    rv = vlan_port_init(unit, vsi1, port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_port_init\n");
        return rv;
    }
    rv = vlan_port_init(unit, vid_2_desig, port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_init\n");
        return rv;
    }

    rv = vlan_port_init(unit, vid_2_desig, port_2);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_init\n");
        return rv;
    }  
    vpn_config.vpn = vsi1;
    vpn_config.high_vid = FGL_outer_vid;
    if ((double_tag)) {
        vpn_config.low_vid = FGL_inner_vid;
    } else {
        vpn_config.low_vid = BCM_VLAN_INVALID;
    }

    vpn_config.flags = BCM_BCM_TRILL_VPN_NETWORK | BCM_BCM_TRILL_VPN_ACCESS;
    
    rv = bcm_trill_vpn_create(unit, &vpn_config);
    if (rv != BCM_E_NONE) {
       printf("Error, in bcm_trill_vpn_create\n");
       print vpn_config;
       return rv;
    }
    printf("Created VPN id %d , high_vid=%d, low_vid=%d\n",vsi1, vpn_config.high_vid, vpn_config.low_vid);
    
    rv = vlan_port_set(unit, port_1, vsi1, vsi1); 
    if (rv != BCM_E_NONE) {
       printf("Error, in vlan_port_set: vsi=%d\n",vsi1 );
       return rv;
    }

    rv = l3_intf_create(unit, trill_vsi_mac_2, vsi_2_desig, &l3_intf_id);
    if (rv != BCM_E_NONE) {
       printf("Error, in l3_intf_create: vsi=%d\n",vsi_2_desig );
       return rv;
    }

    /* Add TRILL Virtual Port for local RB (configures my nickname) */
    rv = config_local_rbridge(unit, nickname_rb_local, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, in config_local_rbridge: vsi=$vsi_1_bridge\n");
        return rv;
    }
    printf("configured local bridge %d\n", nickname_rb_local );

    rv = add_remote_rbridge(unit, remote_mac_rb1, vsi_2_desig, port_1, nickname_rb1, 0, &trill_port_rb1);
    if (rv != BCM_E_NONE) {
        printf("Error, in add_remote_rbridge: vsi=%d\n", vsi1);
        return rv;
    }
    
    rv = add_remote_rbridge(unit, remote_mac_rb2, vsi_2_desig, port_2, nickname_rb2, 0, &trill_port_rb2);
    if (rv != BCM_E_NONE) {
        printf("Error, in add_remote_rbridge: vsi=%d\n", vsi1 );
        return rv;
    }
       
    /* Add L2 entry for Customer packet to TRILL tunnel:
       Adds an entry to the LEM with <c_dmac, nickname (taken from trill_port)> -> FEC */
    bcm_l2_addr_t_init(&l2_addr, c_dmac, vsi1);
    l2_addr.port = trill_port_rb1.trill_port_id;

    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l2_addr_add\n");
        print l2_addr;
        return rv;
    }

    /* Add L2 entry for Customer packet to TRILL tunnel: for egress rb flow
       Adds an entry to the LEM with <da= host1_mac -> port1 */
    bcm_l2_addr_t_init(&l2_addr, host1_mac, vid_1);
    l2_addr.port = port_1;
 
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l2_addr_add\n");
        print l2_addr;
        return rv;
    }

    /* Add L2 entry for Customer packet to TRILL tunnel: for bridge flow
       Adds an entry to the LEM with <da= host4_mac -> port1 */
    bcm_l2_addr_t_init(&l2_addr, host4_mac, vid_1);
    l2_addr.port = port_2;
 
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l2_addr_add\n");
        print l2_addr;
        return rv;
    }

    rv = bcm_vswitch_create_with_id(unit, vsi2);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_create_with_id\n");
        return rv;
    } 
    printf("Created vswitch id %d\n",vsi2);

    rv = vlan_port_init(unit, vsi2, port_2);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_port_init\n");
        return rv;
    }
    
    vpn_config.vpn = vsi2;
    vpn_config.high_vid = FGL_outer_vid;
    vpn_config.low_vid = BCM_VLAN_INVALID;

    vpn_config.flags = BCM_BCM_TRILL_VPN_NETWORK | BCM_BCM_TRILL_VPN_ACCESS | BCM_TRILL_VPN_TRANSPARENT_SERVICE;
    
    rv = bcm_trill_vpn_create(unit, &vpn_config);
    if (rv != BCM_E_NONE) {
       printf("Error, in bcm_trill_vpn_create\n");
       print vpn_config;
       return rv;
    }
    printf("Created VPN id %d , high_vid=%d, low_vid=%d\n",vsi2, vpn_config.high_vid, vpn_config.low_vid);
    
    rv = vlan_port_set(unit, port_2, vsi2, vsi2); 
    if (rv != BCM_E_NONE) {
       printf("Error, in vlan_port_set: vsi=%d\n",vsi2 );
       return rv;
    }
        
    /* Add L2 entry for Customer packet to TRILL tunnel:
         Adds an entry to the LEM with <c_dmac, nickname (taken from trill_port)> -> FEC */
    bcm_l2_addr_t_init(&l2_addr, c_dmac_tts, vsi2);
    l2_addr.port = trill_port_rb2.trill_port_id;

    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l2_addr_add\n");
        print l2_addr;
        return rv;
    }

    /* Add L2 entry for Customer packet to TRILL tunnel: for egress rb flow
         Adds an entry to the LEM with <da= host1_mac -> port1 */
    bcm_l2_addr_t_init(&l2_addr, host2_mac, vid_tts);
    l2_addr.port = port_2;
 
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l2_addr_add\n");
        print l2_addr;
        return rv;
    }
    trill_uni_configured_fgl_tts = TRUE;

    return rv;
}

/*
 *Create trill transparent multicast transmit fecless fgl service.
 */
int 
trill_mc_transit_fecless_config_tts(int unit, int is_fgl, int double_tag, int is_2pass) {
    int rv = 0;
    bcm_if_t encap_id;
    uint32 flags;
    /*bcm_trill_port_t root_port;*/
    bcm_l2_addr_t     l2_addr; 
    bcm_trill_multicast_entry_t mc_entry;
    bcm_if_t              host_encap;
    bcm_field_group_t rpf_group = 100;
    int qual_id_ing_nickname;
    int qual_id_eg_nickname;

    /*enable RPF Trap*/
    rv = rpf_drop_set(unit,10, rpf_group, &qual_id_ing_nickname, &qual_id_eg_nickname);
    BCM_IF_ERROR_RETURN(rv);

    if (is_fgl) {
        rv = trill_uni_fgl_tts(unit, double_tag);
    } else {
        rv = trill_uni(unit);
    }
    BCM_IF_ERROR_RETURN(rv);
    
    rv = mc_global(unit);
    BCM_IF_ERROR_RETURN(rv);
    
    rv = add_root_rbridge(unit, dist_tree_1, &dist_tree_1_port,-1);
    BCM_IF_ERROR_RETURN(rv);

    /* Create Multicast Group */  
    rv = create_mc_group(unit, l2mc_group, 1, !is_2pass);
    BCM_IF_ERROR_RETURN(rv);

    /* bind MC group to distribution-tree */
    bcm_trill_multicast_entry_t_init(&mc_entry);
    mc_entry.root_name = dist_tree_1;
    if (is_fgl) {
        mc_entry.c_vlan = FGL_outer_vid;
        if (double_tag) {
            mc_entry.c_vlan_inner = FGL_inner_vid;
        } else {
            mc_entry.c_vlan_inner = BCM_VLAN_INVALID; 
        }
    } else {
        mc_entry.c_vlan = vid_1;
    }
    mc_entry.group = l2mc_group;
    rv =  bcm_trill_multicast_entry_add(unit, &mc_entry);
    BCM_IF_ERROR_RETURN(rv);
    
    bcm_l2_addr_t_init(&l2_addr, c_dmac, vid_1);
    l2_addr.l2mc_group  = l2mc_group;
    l2_addr.port = dist_tree_1_port.trill_port_id;

    /* Add mac addr with forward to MC group*/
    rv = bcm_l2_addr_add(unit, &l2_addr);
    BCM_IF_ERROR_RETURN(rv);

    /* Add RPF-Check entry to the TCAM */
     rv = bcm_multicast_trill_encap_get(unit, l2mc_group, trill_port_rb1.trill_port_id ,0, &encap_id); 
    BCM_IF_ERROR_RETURN(rv);
    printf ("TRILL encap for RPF check (Adj-eep)= %d \n", encap_id);
    
    rv = trill_multicast_source_add(unit, dist_tree_1, nickname_rb1, port_1,encap_id, rpf_group,
                                         qual_id_ing_nickname, qual_id_eg_nickname);
    BCM_IF_ERROR_RETURN(rv);

    
      /* this is in order to check the trap - mc key not found  (add this to vtt lookup at the TCAM)*/
    rv = add_root_rbridge(unit, dist_tree_2, &dist_tree_2_port, -1);
    BCM_IF_ERROR_RETURN(rv);
    
    rv = trill_multicast_source_add(unit, dist_tree_2, nickname_rb1, port_1,encap_id, rpf_group,
                                         qual_id_ing_nickname, qual_id_eg_nickname);
    BCM_IF_ERROR_RETURN(rv);
    
    /* this is in order to check the trap - RPF_CHECK not found (add this to flp lookup at the LEM)*/ 
    
    rv = add_root_rbridge(unit, dist_tree_3, &dist_tree_3_port, -1);
    BCM_IF_ERROR_RETURN(rv);
    
   /* Create Multicast Group */  
    rv = bcm_multicast_create(unit, BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_WITH_ID, &dummy_mc_group);
    BCM_IF_ERROR_RETURN(rv);
    printf ("TRILL l2mc_group = %d (dummy_mc_group)\n", dummy_mc_group); 
    

    bcm_trill_multicast_entry_t_init(&mc_entry);
    mc_entry.root_name = dist_tree_3;
    mc_entry.c_vlan = vid_1;
    mc_entry.group = dummy_mc_group;
    rv = bcm_trill_multicast_entry_add(unit, &mc_entry);
    BCM_IF_ERROR_RETURN(rv);    

    /* Create Multicast Group for tts service*/  
    rv = create_mc_group(unit, l2mc_group_tts, 1, !is_2pass);
    BCM_IF_ERROR_RETURN(rv);
    
    bcm_trill_multicast_entry_t_init(&mc_entry);
    mc_entry.root_name = dist_tree_2;
    mc_entry.c_vlan = FGL_outer_vid;
    mc_entry.c_vlan_inner = vid_1; 
    mc_entry.flags |= BCM_TRILL_MULTICAST_TRANSPARENT_SERVICE;
    mc_entry.group = l2mc_group_tts;
    rv =  bcm_trill_multicast_entry_add(unit, &mc_entry);
    BCM_IF_ERROR_RETURN(rv);

    bcm_l2_addr_t_init(&l2_addr, c_dmac_tts, vid_tts);
    l2_addr.l2mc_group  = l2mc_group_tts;
    l2_addr.port = dist_tree_2_port.trill_port_id;

    /* Add mac addr with forward to MC group*/
    rv = bcm_l2_addr_add(unit, &l2_addr);
    BCM_IF_ERROR_RETURN(rv);

    return rv;
}

/*
 *Test trill unicast untag fgl service.
 */
int
test_trill_uni_untag_fgl(int unit, int p1, int p2, int trill_ether_type,int hop, int double_tag) {
    port_1 =  p1;
    port_2  = p2;
    trill_ethertype = trill_ether_type;
    hopcount = hop;
    
    trill_uni_untag_fgl(unit, double_tag, 0);
}

/*
 *Test trill transparent unicast untag fgl service.
 */
int
test_trill_uni_untag_fgl_tts(int unit, int p1, int p2, int trill_ether_type,int hop, int double_tag) {
    port_1 =  p1;
    port_2  = p2;
    trill_ethertype = trill_ether_type;
    hopcount = hop;
    
    trill_uni_untag_fgl(unit, double_tag, 1);
}

/*
 *Test trill transparent unicast fgl service.
 */
int
test_trill_uni_fgl_tts(int unit, int p1, int p2, int trill_ether_type,int hop, int double_tag) {
    port_1 =  p1;
    port_2  = p2;
    trill_ethertype = trill_ether_type;
    hopcount = hop;
    
    trill_uni_fgl_tts(unit, double_tag);
}

/*
 *Test trill transparent multicast transmit fecless service.
 */
int
test_trill_mc_transit_fecless_fgl_tts(int unit, int p1, int p2, int trill_ether_type,int hop, bcm_multicast_t group, int double_tag, int is_2pass) {
    int rv = BCM_E_NONE;

    port_1 =  p1;
    port_2  = p2;
    trill_ethertype = trill_ether_type;
    hopcount = hop;
    l2mc_group = group;    

    trill_mc_transit_fecless_config_tts(unit, 1, double_tag, 0);
}

/*
 *Test clear trill unicast untag fgl service.
 */
int
test_clear_trill_uni_untag_fgl_config(int unit) {

   int rv = BCM_E_NONE;
    bcm_vlan_t
        vsi_1_bridge,
        vsi_2_desig;
 
    bcm_l2_addr_t               l2_addr; 
    int                         vsi1=vid_1;
    vsi_1_bridge = vid_1;
    rv = l3_intf_delete(unit, l3_intf_id);
    BCM_IF_ERROR_RETURN(rv);
    printf("deleted l3_intf %d\n", l3_intf_id);

    rv = vlan_term(unit, vid_2_desig);
    BCM_IF_ERROR_RETURN(rv);
    printf("vlan_term  %d\n", vid_2_desig);

    vsi_2_desig = vid_2_desig;        
           
    rv = pvid_port_unset(unit, port_1, port_1, vid_1);
    BCM_IF_ERROR_RETURN(rv);

    rv = vlan_port_term(unit, vid_2_desig, port_1);
    BCM_IF_ERROR_RETURN(rv);

    rv = vlan_port_term(unit, vid_2_desig, port_2);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_trill_port_delete(unit, trill_port_local.trill_port_id);
    BCM_IF_ERROR_RETURN(rv);
    printf("Deleted trill port: id=%d nickname=%d\n", trill_port_local.trill_port_id, trill_port_local.name );
    trill_port_local.trill_port_id= -1;

    /* remove rb1 adjacency entry */
    rv = mult_adjacency(unit,trill_port_rb1.trill_port_id, FALSE);

    if (rv != BCM_E_NONE) {
        printf("Error, in mult_adjacency, \n");
        return rv;
    }

    rv = remove_remote_rbridge(unit, trill_port_rb1.trill_port_id );
    BCM_IF_ERROR_RETURN(rv);
    printf("Deleted trill port: id=%d nickname=%d\n", trill_port_rb1.trill_port_id, trill_port_rb1.name );
    trill_port_rb1.trill_port_id = -1;

    /* remove rb2 adjacency entry */
    rv = mult_adjacency(unit,trill_port_rb2.trill_port_id, FALSE);

    if (rv != BCM_E_NONE) {
        printf("Error, in mult_adjacency, \n");
        return rv;
    }

    rv = remove_remote_rbridge(unit, trill_port_rb2.trill_port_id );
    BCM_IF_ERROR_RETURN(rv);
    printf("Deleted trill port: id=%d nickname=%d\n", trill_port_rb2.trill_port_id, trill_port_rb2.name );
    trill_port_rb2.trill_port_id = -1;

    rv = port_config_reset(unit, port_1, vid_2_desig);
    BCM_IF_ERROR_RETURN(rv);

    printf("Reset config for port = %d\n", port_1 );
    rv = port_config_reset(unit, port_2, vid_2_desig);
    BCM_IF_ERROR_RETURN(rv);

    printf("Reset config for port = %d\n", port_2 );
    rv = traps_unset(unit);
    BCM_IF_ERROR_RETURN(rv);

    rv =  bcm_vswitch_destroy(unit, vsi1);
    BCM_IF_ERROR_RETURN(rv);

    printf("vswitch removed\n" );

    rv=bcm_trill_vpn_destroy(unit, vsi1);
    BCM_IF_ERROR_RETURN(rv);

    printf("vpn removed\n" );

    /* In advanced vlan translation mode, we create vlan translation actions that are not created by default.
       We call this to destroy them. If advanced vlan translation mode is off, it does nothing.*/
    rv = vlan_translation_default_mode_destroy(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translation_default_mode_destroy\n");
        return rv;
    }

    printf("Destroyed vlan translation actions\n");

    trill_uni_untag_configured_fgl = FALSE;
    return rv;
}

/*
 *Test clear trill transparent unicast fgl service.
 */
int
test_clear_trill_uni_fgl_config_tts(int unit) {

   int rv = BCM_E_NONE;
    bcm_vlan_t
        vsi_1_bridge,
        vsi_2_desig;
 
    bcm_l2_addr_t               l2_addr; 
    int                         vsi1=vid_1;
    int                         vsi2=vid_tts;
    vsi_1_bridge = vid_1;
    rv = l3_intf_delete(unit, l3_intf_id);
    BCM_IF_ERROR_RETURN(rv);
    printf("deleted l3_intf %d\n", l3_intf_id);

    rv = vlan_term(unit, vid_2_desig);
    BCM_IF_ERROR_RETURN(rv);
    printf("vlan_term  %d\n", vid_2_desig);

    vsi_2_desig = vid_2_desig;        
           
    rv = vlan_port_unset(unit, port_1, vid_1, vid_1);
    BCM_IF_ERROR_RETURN(rv);

    rv = vlan_port_unset(unit, port_2, vid_tts, vsi2);
    BCM_IF_ERROR_RETURN(rv);

    rv = vlan_port_term(unit, vid_2_desig, port_1);
    BCM_IF_ERROR_RETURN(rv);

    rv = vlan_port_term(unit, vid_1, port_2);
    BCM_IF_ERROR_RETURN(rv);

    rv = vlan_port_term(unit, vid_2_desig, port_2);
    BCM_IF_ERROR_RETURN(rv);

    rv = bcm_trill_port_delete(unit, trill_port_local.trill_port_id);
    BCM_IF_ERROR_RETURN(rv);
    printf("Deleted trill port: id=%d nickname=%d\n", trill_port_local.trill_port_id, trill_port_local.name );
    trill_port_local.trill_port_id= -1;

    /* remove rb1 adjacency entry */
    rv = mult_adjacency(unit,trill_port_rb1.trill_port_id, FALSE);

    if (rv != BCM_E_NONE) {
        printf("Error, in mult_adjacency, \n");
        return rv;
    }

    rv = remove_remote_rbridge(unit, trill_port_rb1.trill_port_id );
    BCM_IF_ERROR_RETURN(rv);
    printf("Deleted trill port: id=%d nickname=%d\n", trill_port_rb1.trill_port_id, trill_port_rb1.name );
    trill_port_rb1.trill_port_id = -1;

    /* remove rb2 adjacency entry */
    rv = mult_adjacency(unit,trill_port_rb2.trill_port_id, FALSE);

    if (rv != BCM_E_NONE) {
        printf("Error, in mult_adjacency, \n");
        return rv;
    }

    rv = remove_remote_rbridge(unit, trill_port_rb2.trill_port_id );
    BCM_IF_ERROR_RETURN(rv);
    printf("Deleted trill port: id=%d nickname=%d\n", trill_port_rb2.trill_port_id, trill_port_rb2.name );
    trill_port_rb2.trill_port_id = -1;

    rv = port_config_reset(unit, port_1, vid_2_desig);
    BCM_IF_ERROR_RETURN(rv);

    printf("Reset config for port = %d\n", port_1 );
    rv = port_config_reset(unit, port_2, vid_2_desig);
    BCM_IF_ERROR_RETURN(rv);

    printf("Reset config for port = %d\n", port_2 );
    rv = traps_unset(unit);
    BCM_IF_ERROR_RETURN(rv);

    rv =  bcm_vswitch_destroy(unit, vsi1);
    BCM_IF_ERROR_RETURN(rv);

    printf("vswitch removed\n" );

    rv=bcm_trill_vpn_destroy(unit, vsi1);
    BCM_IF_ERROR_RETURN(rv);

    printf("vpn removed\n" );

    rv =  bcm_vswitch_destroy(unit, vsi2);
    BCM_IF_ERROR_RETURN(rv);

    printf("vswitch removed\n" );

    rv=bcm_trill_vpn_destroy(unit, vsi2);
    BCM_IF_ERROR_RETURN(rv);

    printf("vpn removed\n" );
    
    /* In advanced vlan translation mode, we create vlan translation actions that are not created by default.
       We call this to destroy them. If advanced vlan translation mode is off, it does nothing.*/
    rv = vlan_translation_default_mode_destroy(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in vlan_translation_default_mode_destroy\n");
        return rv;
    }

    printf("Destroyed vlan translation actions\n");

    trill_uni_configured_fgl = FALSE;
    return rv;
}

/*
 *Test clear trill unicast untag fgl service.
 */
int
test_clear_trill_uni_untag_fgl(int unit) {
    test_clear_trill_uni_untag_fgl_config(unit);
}

/*
 *Test clear trill transparent unicast untag fgl service.
 */
int
test_clear_trill_uni_untag_fgl_tts(int unit) {
    test_clear_trill_uni_untag_fgl_config(unit);
}

/*
 *Test clear trill transparent unicast fgl service.
 */
int
test_clear_trill_uni_fgl_tts(int unit) {
    test_clear_trill_uni_fgl_config_tts(unit);
}

/*
 *Test clear trill transparent multicast transmit fecless fgl service.
 */
int
test_clear_trill_mc_transit_fecless_fgl_tts(int unit, int double_tag) {
    return test_clear_trill_mc_transit_fecless_config(unit,1,double_tag,0);
}

