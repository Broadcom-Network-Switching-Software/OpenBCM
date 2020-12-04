/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File: cint_eth_udh.c
 * Purpose: Example for UDH_ETH.
 */

/*
 * This cint demonstrate configuration of UDH_ETH with vxlan service.
 *
 * SOC:
 *      field_class_id_size_3=16
 *      bcm886xx_vxlan_enable=1
 *      bcm886xx_vxlan_tunnel_lookup_mode=2
 *      bcm886xx_ip4_tunnel_termination_mode=0
 *      tm_port_header_type_out_$out_port=UDH_ETH
 *
 *
 * Usage:
 *
 * cint ../../../../src/examples/dpp/cint_eth_udh.c
 * cint
 * pmf_static_udh($unit,$udh_value);
 * vxlan_l2_setup($unit,$in_pot,$out_port);
 */

void pmf_static_udh(int unit, int udh_value) {
    int rv;
    
    bcm_field_group_config_t grp;
    bcm_field_group_config_t_init(&grp);
 
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyIpProtocolCommon);
 
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE;
    grp.mode = bcmFieldGroupModeDirectExtraction;
    rv = bcm_field_group_config_create(unit, &grp);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_config_create\n", rv);
        return rv;
    }

 
    bcm_field_aset_t aset;
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionClassSourceSet);
 
    rv = bcm_field_group_action_set(unit, grp.group, aset);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_group_config_create\n", rv);
        return rv;
    }

 
    bcm_field_entry_t ent;
    rv = bcm_field_entry_create(unit, grp.group, &ent);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_create\n", rv);
        return rv;
    }

 
    bcm_field_extraction_action_t ext_action;
    bcm_field_extraction_action_t_init(&ext_action);
    ext_action.action = bcmFieldActionClassSourceSet;
    ext_action.bias = 0;
 
    bcm_field_extraction_field_t ext_info;
    bcm_field_extraction_field_t_init(&ext_info);
    ext_info.flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    ext_info.bits  = 16;
    ext_info.value = udh_value;
 
    rv = bcm_field_direct_extraction_action_add(unit, ent, ext_action, 1, &ext_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_direct_extraction_action_add\n", rv);
        return rv;
    }

    rv = bcm_field_entry_install(unit, ent);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_install\n", rv);
        return rv;
    }

    return rv;
}



int vxlan_l2_setup(int unit, int in_port, int out_port) {
    int rv;
    bcm_vxlan_init(unit);
 
    int vpn = 100;
    bcm_vxlan_vpn_config_t vpn_config;
    bcm_vxlan_vpn_config_t_init(&vpn_config);
    vpn_config.flags = BCM_VXLAN_VPN_WITH_ID | BCM_VXLAN_VPN_WITH_VPNID;
    vpn_config.vpn = vpn;
    vpn_config.broadcast_group = vpn;
    vpn_config.unknown_unicast_group = vpn;
    vpn_config.unknown_multicast_group = vpn;
    vpn_config.vnid = vpn;
    bcm_vxlan_vpn_create(unit, &vpn_config);
 
    bcm_mac_t mac_address  = {0x00, 0x00, 0x00, 0x00, 0x00, 0x2};
 
    bcm_l3_intf_t l3if;
    bcm_l3_intf_t_init(&l3if);
    l3if.l3a_vid = vpn;
    l3if.l3a_intf_id = vpn;
    sal_memcpy(l3if.l3a_mac_addr, mac_address, 6);
    rv = bcm_l3_intf_create(unit, l3if);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_install\n", rv);
        return rv;
    }

 
    bcm_l3_intf_t l3_intf;
    bcm_l3_intf_t_init(&l3_intf);
    bcm_tunnel_initiator_t tunnel;
    bcm_tunnel_initiator_t_init(&tunnel);
    tunnel.sip = 0xC0A80001;
    tunnel.dip = 0xC0A80002;
    tunnel.vlan = vpn;
    tunnel.type = bcmTunnelTypeVxlan;
    rv = bcm_tunnel_initiator_create(unit, &l3_intf, &tunnel);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_install\n", rv);
        return rv;
    }

 
    bcm_if_t tunnel_fec;
    bcm_l3_egress_t l3eg;
    bcm_l3_egress_t_init(&l3eg);
    l3eg.intf           = l3_intf.l3a_intf_id;
    l3eg.port           = out_port;
    sal_memcpy(l3eg.mac_addr, mac_address , 6);
    l3eg.vlan       = vpn;
    rv = bcm_l3_egress_create(unit, 0, &l3eg, &tunnel_fec);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_install\n", rv);
        return rv;
    }

 
    bcm_tunnel_terminator_t tunnel_term;
    bcm_tunnel_terminator_t_init(&tunnel_term);    
    tunnel_term.dip = 0xC0A80002;
    tunnel_term.dip_mask = 0xffffffff;  
    tunnel_term.sip = 0;  
    tunnel_term.sip_mask = 0; 
    tunnel_term.vrf = vpn; 
    tunnel_term.type = bcmTunnelTypeVxlan; 
    tunnel_term.tunnel_if = BCM_IF_INVALID; 
    rv = bcm_tunnel_terminator_create(unit, &tunnel_term);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_install\n", rv);
        return rv;
    }
 
    bcm_vxlan_port_t vxlan_port;
    bcm_vxlan_port_t_init(&vxlan_port);
    vxlan_port.criteria = BCM_VXLAN_PORT_MATCH_VN_ID;
    vxlan_port.match_port = in_port;
    vxlan_port.match_tunnel_id = tunnel_term.tunnel_id;
    vxlan_port.egress_if =tunnel_fec;
    vxlan_port.flags = BCM_VXLAN_PORT_EGRESS_TUNNEL;
    vxlan_port.egress_tunnel_id = tunnel.tunnel_id;
    rv = bcm_vxlan_port_add(unit, vpn, &vxlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_vxlan_port_add\n", rv);
        return rv;
    }

 
    bcm_l2_addr_t l2addr;
    bcm_l2_addr_t_init(&l2addr, mac_address, vpn);
    l2addr.port = vxlan_port.vxlan_port_id;
    rv = bcm_l2_addr_add(0, &l2addr);
     if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_l2_addr_add\n", rv);
        return rv;
    }
     
    rv = bcm_vlan_gport_add(unit, vpn, in_port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error (%d), in bcm_field_entry_install\n", rv);
        return rv;
    }
    return rv;

}


