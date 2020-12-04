/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* 
* cint_vlan_ive_pon_outer_add_2tag.c
*
*  This CINT is from customer to check the EPNI_ETHERNET_HEADER_EDITING_SIZE configuration.
*
*  JR+ ETPP Termination calculates Ethernet header size according to the NOF tags and whether or not there is IPv4 above it, using the 
*  EPNI_ETHERNET_HEADER_EDITING_SIZE register.
*  When do the packet bridging, the ethernet header will add the additonal size in the ETPP forwarding stage:
*      IPv6: additional size is 2.
*      IPv4: additional size is 12.
*      MPLS: addtional size is 4.
*  Previously, this register was configured as 0x318318318318, that the IPv4 2 tag value is 12.
*  SET ive action as pop outer and add 2 tags, incoming packet is S+C tag, so the egress header editing size is 12 which normally will add extra 12 
*  bytes by the above logic, but due to the egress packet is 3 tag, it will not add extra bytes that caused by vlan tag is not added as expectation.
*  So add this CINT to check this issue.
 */
bcm_vlan_port_t vlan_port1; 
bcm_vlan_port_t vlan_port2; 
bcm_gport_t lif_id_1 = 0x4480201e; 
bcm_gport_t lif_id_2 = 0x4480201f; 
int inner_vlan_1 = 70; 
int outer_vlan_1 = 50; 
int inner_vlan_2 = 80; 
int outer_vlan_2 = 60; 
int in_action_id = 10; 
int eg_action_id = 33; 
int edit_class_id = 3; 

int vlan_port_create(int unit, bcm_gport_t port, bcm_gport_t lif_id, bcm_vlan_port_t *vlan_port) 
{ 
    int rv;
    
    bcm_vlan_port_t_init(vlan_port); 

    vlan_port->criteria = BCM_VLAN_PORT_MATCH_NONE; 
    vlan_port->port = port; 
    vlan_port->match_vlan = 0; 
    vlan_port->egress_vlan = 0; 
    vlan_port->vsi = 0; 
    vlan_port->flags = BCM_VLAN_PORT_OUTER_VLAN_PRESERVE | BCM_VLAN_PORT_INNER_VLAN_PRESERVE; 

    vlan_port->flags |= BCM_VLAN_PORT_WITH_ID; 
    vlan_port->vlan_port_id = lif_id; 

    rv = bcm_vlan_port_create(unit, vlan_port); 
    if (rv != BCM_E_NONE) { 
        printf("vlan_port_create err return %s \n", bcm_errmsg(rv)); 
        return rv;
    } 

    return rv;
} 

int vlan_port_ingress_match_add(int unit, int inner_vlan, int outer_vlan, bcm_vlan_port_t *vlan_port) 
{ 
    int rv;
    
    bcm_port_match_info_t matchInfo; 
    bcm_port_match_info_t_init(&matchInfo); 
    matchInfo.match = BCM_PORT_MATCH_PORT_VLAN; 
    matchInfo.flags = BCM_PORT_MATCH_INGRESS_ONLY; 
    matchInfo.match_vlan = outer_vlan; 
    matchInfo.match_inner_vlan = inner_vlan; 
    matchInfo.port = vlan_port->port; 
    printf("local port 0x%x, vlan_port_id 0x%x\n", vlan_port->port, vlan_port->vlan_port_id); 
    rv = bcm_port_match_add(unit,vlan_port->vlan_port_id, &matchInfo); 
    if (rv != BCM_E_NONE) { 
        printf("bcm_port_match_add err return %s \n", bcm_errmsg(rv)); 
        return rv;
    } 

    bcm_port_match_info_t_init(&matchInfo); 
    matchInfo.match = BCM_PORT_MATCH_PORT_VLAN_STACKED; 
    matchInfo.flags = BCM_PORT_MATCH_INGRESS_ONLY; 
    matchInfo.match_vlan = outer_vlan; 
    matchInfo.match_inner_vlan = inner_vlan; 
    matchInfo.port = vlan_port->port; 
    printf("local port 0x%x, vlan_port_id 0x%x\n", vlan_port->port, vlan_port->vlan_port_id); 
    rv = bcm_port_match_add(unit,vlan_port->vlan_port_id, &matchInfo); 
    if (rv != BCM_E_NONE) { 
        printf("bcm_port_match_add err return %s \n", bcm_errmsg(rv)); 
        return rv;
    } else{ 
        printf("bcm_port_match_add with BCM_PORT_MATCH_PORT_VLAN_STACKED return OK *************\n"); 
    } 

    return rv;
} 

int vlan_port_egress_match_add(int unit, int inner_vlan, int outer_vlan, bcm_vlan_port_t * vlan_port) 
{ 
    int rv;
    
    bcm_port_match_info_t matchInfo; 
    bcm_port_match_info_t_init(&matchInfo); 
    matchInfo.match = BCM_PORT_MATCH_PORT_VLAN; 
    matchInfo.flags = BCM_PORT_MATCH_EGRESS_ONLY; 
    matchInfo.match_vlan = outer_vlan; 
    matchInfo.match_inner_vlan = inner_vlan; 
    matchInfo.port = vlan_port->port; 
    printf("local port 0x%x, vlan_port_id 0x%x\n", vlan_port->port, vlan_port->vlan_port_id); 
    rv = bcm_port_match_add(unit,vlan_port->vlan_port_id, &matchInfo); 
    if (rv != BCM_E_NONE) { 
        printf("bcm_port_match_add err return %s \n", bcm_errmsg(rv)); 
        return rv;
    } 

    return rv;
} 

int vlan_port_egress_match_delete(int unit, int vsi, bcm_vlan_port_t *vlan_port) 
{ 
    int rv;
    
    /*add for vsi match for the L3 egress*/ 
    bcm_port_match_info_t matchInfo; 
    bcm_port_match_info_t_init(&matchInfo); 
    matchInfo.match = BCM_PORT_MATCH_PORT_VLAN; 
    matchInfo.flags = BCM_PORT_MATCH_EGRESS_ONLY; 
    matchInfo.match_vlan = vsi; 
    matchInfo.port = vlan_port->port; 

    printf("local port 0x%x, vlan_port_id 0x%x, vsi %d \n", vlan_port->port, vlan_port->vlan_port_id, vsi); 
    rv = bcm_port_match_delete(unit,vlan_port->vlan_port_id,&matchInfo); 
    if (rv != BCM_E_NONE) { 
        printf("bcm_port_match_delete ERR return %s \n", bcm_errmsg(rv));
        return rv;
    } 

    return rv;
} 



int port_vlan_domain_set(int unit, int local_port) 
{ 
    int rv = bcm_port_class_set(unit, local_port, bcmPortClassId, local_port); 
    if (rv != BCM_E_NONE) { 
        printf("trunk vlan domain set failed, rc %s \n", bcm_errmsg(rv)); 
        return rv;
    } 

    return rv;
} 

int set_port_ethertype(int unit, bcm_gport_t hwport_id) 
{ 
    int rv = BCM_E_NONE; /* sdk operation status. */ 
    bcm_port_tpid_class_t port_tpid_class; 

    rv = bcm_port_tpid_delete_all(unit, hwport_id); 
    if (BCM_E_NONE != rv) { 
        printf("bcm_port_tpid_delete_all failed, rc %s \n", bcm_errmsg(rv)); 
        return rv;
    } 


    rv = bcm_port_tpid_add(unit, hwport_id, 0x8100, 0); 
    if (BCM_E_NONE != rv) { 
        printf("bcm_port_tpid_add failed, rc %s \n", bcm_errmsg(rv));
        return rv;
    } 

    rv = bcm_port_tpid_add(unit, hwport_id, 0x88A8, 0); 
    if (BCM_E_NONE != rv) { 
        printf("bcm_port_tpid_add failed, rc %s \n", bcm_errmsg(rv));
        return rv;
    } 

    bcm_port_tpid_class_t_init(&port_tpid_class); 
    port_tpid_class.port = hwport_id; 
    port_tpid_class.tpid1 = 0x8100; 
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY; 
    port_tpid_class.tag_format_class_id = 12; 
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY; 
    port_tpid_class.vlan_translation_action_id = 0; 
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class); 
    if (BCM_E_NONE != rv) { 
        printf("bcm_port_tpid_class_set failed, rc %s \n", bcm_errmsg(rv)); 
        return rv;
    } 

    /* C TAG egress */ 
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY; 
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class); 
    if (BCM_E_NONE != rv) { 
        printf("bcm_port_tpid_class_set failed, rc %s \n", bcm_errmsg(rv)); 
        return rv;
    } 

    /* Priority C TAG ingress */ 
    bcm_port_tpid_class_t_init(&port_tpid_class); 
    port_tpid_class.port = hwport_id; 
    port_tpid_class.tpid1 = 0x8100; 
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY; 
    port_tpid_class.tag_format_class_id = 13; 
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_OUTER_IS_PRIO | BCM_PORT_TPID_CLASS_INGRESS_ONLY; 
    port_tpid_class.vlan_translation_action_id = 0; 
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class); 
    if (BCM_E_NONE != rv) { 
        printf("bcm_port_tpid_class_set failed, rc %s \n", bcm_errmsg(rv)); 
        return rv;
    } 

    /* Priority C TAG egress */ 
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_OUTER_IS_PRIO | BCM_PORT_TPID_CLASS_EGRESS_ONLY; 
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class); 
    if (BCM_E_NONE != rv) { 
        printf("bcm_port_tpid_class_set failed, rc %s \n", bcm_errmsg(rv)); 
        return rv;
    } 

    /* C_C TAG ingress */ 
    bcm_port_tpid_class_t_init(&port_tpid_class); 
    port_tpid_class.port = hwport_id; 
    port_tpid_class.tpid1 = 0x8100; 
    port_tpid_class.tpid2 = 0x8100; 
    port_tpid_class.tag_format_class_id = 8; 
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY; 
    port_tpid_class.vlan_translation_action_id = 0; 
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class); 
    if (BCM_E_NONE != rv) { 
        printf("bcm_port_tpid_class_set failed, rc %s \n", bcm_errmsg(rv)); 
        return rv;
    } 

    /* C_C TAG egress */ 
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY; 
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class); 
    if (BCM_E_NONE != rv) { 
        printf("bcm_port_tpid_class_set failed, rc %s \n", bcm_errmsg(rv)); 
        return rv;
    } 


    /* C_S TAG ingress*/ 
    bcm_port_tpid_class_t_init(&port_tpid_class); 
    port_tpid_class.port = hwport_id; 
    port_tpid_class.tpid1 = 0x8100; 
    port_tpid_class.tpid2 = 0x88A8; 
    port_tpid_class.tag_format_class_id = 9; 
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY; 
    port_tpid_class.vlan_translation_action_id = 0; 
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class); 
    if (BCM_E_NONE != rv) { 
        printf("bcm_port_tpid_class_set failed, rc %s \n", bcm_errmsg(rv)); 
        return rv;
    } 

    /* C_C TAG egress */ 
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY; 
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class); 
    if (BCM_E_NONE != rv) { 
        printf("bcm_port_tpid_class_set failed, rc %s \n", bcm_errmsg(rv)); 
        return rv;
    } 

    /* S-TAG */ 
    bcm_port_tpid_class_t_init(&port_tpid_class); 
    port_tpid_class.port = hwport_id; 
    port_tpid_class.tpid1 = 0x88A8; 
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY; 
    port_tpid_class.tag_format_class_id = 14; 
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY; 
    port_tpid_class.vlan_translation_action_id = 0; 
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class); 
    if (BCM_E_NONE != rv) { 
        printf("bcm_port_tpid_class_set failed, rc %s \n", bcm_errmsg(rv)); 
        return rv;
    } 

    /* S TAG egress */ 
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY; 
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class); 
    if (BCM_E_NONE != rv) { 
        printf("bcm_port_tpid_class_set failed, rc %s \n", bcm_errmsg(rv)); 
        return rv;
    } 

    /* Priority S TAG ingress */ 
    bcm_port_tpid_class_t_init(&port_tpid_class); 
    port_tpid_class.port = hwport_id; 
    port_tpid_class.tpid1 = 0x88A8; 
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY; 
    port_tpid_class.tag_format_class_id = 15; 
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_OUTER_IS_PRIO | BCM_PORT_TPID_CLASS_INGRESS_ONLY; 
    port_tpid_class.vlan_translation_action_id = 0; 
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class); 
    if (BCM_E_NONE != rv) { 
        printf("bcm_port_tpid_class_set failed, rc %s \n", bcm_errmsg(rv)); 
        return rv;
    } 

    /* Priority S TAG egress */ 
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_OUTER_IS_PRIO | BCM_PORT_TPID_CLASS_EGRESS_ONLY; 
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class); 
    if (BCM_E_NONE != rv) { 
        printf("bcm_port_tpid_class_set failed, rc %s \n", bcm_errmsg(rv)); 
        return rv;
    } 

    /* S_C TAG */ 
    bcm_port_tpid_class_t_init(&port_tpid_class); 
    port_tpid_class.port = hwport_id; 
    port_tpid_class.tpid1 = 0x88A8; 
    port_tpid_class.tpid2 = 0x8100; 
    port_tpid_class.tag_format_class_id = 10; 
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY; 
    port_tpid_class.vlan_translation_action_id = 0; 
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class); 
    if (BCM_E_NONE != rv) { 
        printf("bcm_port_tpid_class_set failed, rc %s \n", bcm_errmsg(rv)); 
        return rv;
    } 

    /* S_C TAG egress */ 
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY; 
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class); 
    if (BCM_E_NONE != rv) { 
        printf("bcm_port_tpid_class_set failed, rc %s \n", bcm_errmsg(rv)); 
        return rv;
    } 

    /* S_S TAG */ 
    bcm_port_tpid_class_t_init(&port_tpid_class); 
    port_tpid_class.port = hwport_id; 
    port_tpid_class.tpid1 = 0x88A8; 
    port_tpid_class.tpid2 = 0x88A8; 
    port_tpid_class.tag_format_class_id = 11; 
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY; 
    port_tpid_class.vlan_translation_action_id = 0; 
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class); 
    if (BCM_E_NONE != rv) { 
        printf("bcm_port_tpid_class_set failed, rc %s \n", bcm_errmsg(rv)); 
        return rv;
    } 

    /* S_S TAG egress */ 
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY; 
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class); 
    if (BCM_E_NONE != rv) { 
        printf("bcm_port_tpid_class_set failed, rc %s \n", bcm_errmsg(rv)); 
        return rv;
    } 

    return rv; 
} 

int port_class_set (int unit, int lif_gport) 
{ 
    int rv; 
    uint32 class_id = 0; 

    rv = bcm_port_class_get(unit, 
                                lif_gport, 
                                bcmPortClassFieldIngressPacketProcessing, 
                                &class_id); 
    if (rv != BCM_E_NONE) { 
        printf("bcm_port_class_get, lif_gport 0x%x %s", lif_gport, bcm_errmsg(rv)); 
        return rv; 
    } 

    class_id |= 4; 

    rv = bcm_port_class_set(unit, 
                                lif_gport, 
                                bcmPortClassFieldIngressPacketProcessing, 
                                class_id); 
    if (rv != BCM_E_NONE) { 
        printf("bcm_port_class_get, lif_gport 0x%x %s", lif_gport, bcm_errmsg(rv)); 
        return rv; 
    } 

    return rv; 
} 


int vlan_translate_action_id_create(int unit ,int action_ingress, int action_egress, uint32 flags_ingress, uint32 flags_egress) 
{ 
    int rv; 

    rv = bcm_vlan_translate_action_id_create(unit,flags_ingress,&action_ingress); 
    if (rv != BCM_E_NONE) { 
        printf("bcm_vlan_translate_action_id_create ERR return %s \n", bcm_errmsg(rv)); 
        return rv; 
    } 
    rv = bcm_vlan_translate_action_id_create(unit,flags_egress,&action_egress); 
    if (rv != BCM_E_NONE) { 
        printf("bcm_vlan_translate_action_id_create ERR return %s \n", bcm_errmsg(rv)); 
        return rv; 
    } 

    return rv;
} 

int vlan_translate_download_action(int unit, int class_id, int action_id) 
{ 
    bcm_vlan_action_set_t action; 
    bcm_vlan_translate_action_class_t action_class; 
    int flags; 
    int rv; 
    int i; 


    bcm_vlan_action_set_t_init(&action); 
    action.dt_outer = 2; 
    action.dt_inner = 1; 
    action.outer_tpid = 0x88a8; 
    action.inner_tpid = 0x8100; 
    action.outer_tpid_action = 1; 
    action.inner_tpid_action = 1; 
    action.dt_outer_pkt_prio = 1; 
    action.dt_inner_pkt_prio = 0; 

    rv = bcm_vlan_translate_action_id_set(unit, 5, action_id, &action); 
    if (rv != BCM_E_NONE) { 
        printf("Error, bcm_vlan_translate_action_id_set, err code=%d\n", rv); 
        return rv; 
    } 

    for (i=1;i<16;i++) 
    { 
        bcm_vlan_translate_action_class_t_init(&action_class); 
        action_class.vlan_edit_class_id = class_id; 
        action_class.tag_format_class_id = i; 
        action_class.vlan_translation_action_id = action_id; 
        action_class.flags = BCM_VLAN_ACTION_SET_INGRESS; 

        rv = bcm_vlan_translate_action_class_set(unit, &action_class); 
        if (rv != BCM_E_NONE) { 
            printf("Error, bcm_vlan_translate_action_class_set, err code=%d\n", rv); 
            return rv; 
        } 
    } 

    return rv; 
} 

int vlan_translate_profile_binding(int unit,
                                    int outer_vid, 
                                    int inner_vid, 
                                    bcm_gport_t vlan_port, 
                                    int edit_class_id, 
                                    int dir) 
{ 
    bcm_vlan_port_translation_t port_trans; 
    int rv; 
    /* Set port translation */ 
    bcm_vlan_port_translation_t_init(&port_trans); 
    port_trans.new_outer_vlan = outer_vid; 

    if (inner_vid != 0 ) 
    { 
      port_trans.new_inner_vlan = inner_vid; 
    } 

    port_trans.gport = vlan_port; 
    port_trans.vlan_edit_class_id = edit_class_id; 
    if (0 == dir) 
    { 
        port_trans.flags = BCM_VLAN_ACTION_SET_INGRESS; 
    } 
    else if (1 == dir) 
    { 
        port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS; 
    } 
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE) { 
        printf("Error, bcm_vlan_port_translation_set %d\n", rv); 
        return rv; 
    } 

    return rv; 
} 

int xc_create(int unit,
                bcm_gport_t gport1, 
                bcm_gport_t gport2, 
                int encap1, 
                int encap2) 
{ 
    bcm_vswitch_cross_connect_t gports; 
    int rv; 

    gports.port1 = gport1; 
    gports.port2 = gport2; 
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL; 
    gports.encap1 = encap1 & 0xffff; 
    gports.encap2 = encap2 & 0xffff; 

    rv = bcm_vswitch_cross_connect_add(unit, &gports); 
    if(rv != BCM_E_NONE) { 
        printf("cross connect create failed, return code %d",rv); 
        return rv; 
    } 

    return rv; 
} 

int vlan_ive_pop_outer_add_2tag(int unit,
                                int in_port,
                                int out_port)
{
    int rv;
    uint32 flags_ingress;
    uint32 flags_egress;

    rv = vlan_port_create(unit, in_port, lif_id_1, &vlan_port1);
    if(rv != BCM_E_NONE) { 
        printf("vlan_port_create failed, return code %d",rv); 
        return rv; 
    }

    rv = vlan_port_create(unit, out_port, lif_id_2, &vlan_port2);
    if(rv != BCM_E_NONE) { 
        printf("vlan_port_create failed, return code %d",rv); 
        return rv; 
    }

    rv = set_port_ethertype(unit, in_port);
    if(rv != BCM_E_NONE) { 
        printf("set_port_ethertype failed, return code %d",rv); 
        return rv; 
    }

    rv = set_port_ethertype(unit, out_port);
    if(rv != BCM_E_NONE) { 
        printf("set_port_ethertype failed, return code %d",rv); 
        return rv; 
    }

    rv = port_vlan_domain_set(unit, in_port);
    if(rv != BCM_E_NONE) { 
        printf("port_vlan_domain_set failed, return code %d",rv); 
        return rv; 
    }

    rv = port_vlan_domain_set(unit, out_port);
    if(rv != BCM_E_NONE) { 
        printf("port_vlan_domain_set failed, return code %d",rv); 
        return rv; 
    }

    rv = port_class_set(unit, lif_id_1);
    if(rv != BCM_E_NONE) { 
        printf("port_class_set failed, return code %d",rv); 
        return rv; 
    }

    rv = port_class_set(unit, lif_id_2);
    if(rv != BCM_E_NONE) { 
        printf("port_class_set failed, return code %d",rv); 
        return rv; 
    }

    rv = vlan_port_ingress_match_add(unit, inner_vlan_1, outer_vlan_1, &vlan_port1);
    if(rv != BCM_E_NONE) { 
        printf("vlan_port_ingress_match_add failed, return code %d",rv); 
        return rv; 
    }

    rv = vlan_port_egress_match_add(unit, inner_vlan_2, outer_vlan_2, &vlan_port1);
    if(rv != BCM_E_NONE) { 
        printf("vlan_port_ingress_match_add failed, return code %d",rv); 
        return rv; 
    }

    rv = vlan_port_ingress_match_add(unit, inner_vlan_2, outer_vlan_2, &vlan_port2);
    if(rv != BCM_E_NONE) { 
        printf("vlan_port_ingress_match_add failed, return code %d",rv); 
        return rv; 
    }

    rv = vlan_port_egress_match_add(unit, inner_vlan_2, outer_vlan_2, &vlan_port2);
    if(rv != BCM_E_NONE) { 
        printf("vlan_port_ingress_match_add failed, return code %d",rv); 
        return rv; 
    }

    flags_ingress = BCM_VLAN_ACTION_SET_WITH_ID | BCM_VLAN_ACTION_SET_INGRESS;
    flags_egress = BCM_VLAN_ACTION_SET_WITH_ID | BCM_VLAN_ACTION_SET_EGRESS;
    rv = vlan_translate_action_id_create(unit, in_action_id, eg_action_id, flags_ingress, flags_egress);
    if(rv != BCM_E_NONE) { 
        printf("vlan_translate_action_id_create failed, return code %d",rv); 
        return rv; 
    }

    rv = vlan_translate_download_action(unit, edit_class_id, in_action_id);
    if(rv != BCM_E_NONE) { 
        printf("vlan_translate_download_action failed, return code %d",rv); 
        return rv; 
    }

    rv = vlan_translate_profile_binding(unit, outer_vlan_2, inner_vlan_2, lif_id_1, edit_class_id, 0);
    if(rv != BCM_E_NONE) { 
        printf("vlan_translate_profile_binding failed, return code %d",rv); 
        return rv; 
    }

    rv = xc_create(unit, lif_id_1, lif_id_2, vlan_port1.vlan_port_id, vlan_port2.vlan_port_id); 
    if(rv != BCM_E_NONE) { 
        printf("vlan_translate_profile_binding failed, return code %d",rv); 
        return rv; 
    }

    return rv;
}


