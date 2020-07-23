/*
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */

/*****************************************************************************
 *                                                                           *
 *                     REQUIRED CONFIGURATIONs.                              *
 *                                                                           *
 *                                                                           *
 * #Logical port 7 is used as cascaded/backplace port.                       *
 * pbmp_subport=0x0000000080                                                 *
 *                                                                           *
 * # Number of subport on backplane port 7 are 48                            *
 * num_subports_7=48                                                         *
 *                                                                           *
 * # The subport addition happens through flex encofing. enable flex mode.   *
 * flow_init_mode=1                                                          *
 */





cint_reset();

bcm_if_t nh_index[2];



int configure_v4_defroute(int unit,int vrf,  uint32 ip, uint32 subnet_mask, bcm_if_t nh_index) {

    bcm_l3_route_t defroute;
    int status;
    int i = 0;

    bcm_l3_route_t_init(&defroute);
    defroute.l3a_subnet = ip & subnet_mask;
    defroute.l3a_ip_mask = subnet_mask;
    defroute.l3a_intf = nh_index;
    defroute.l3a_vrf = vrf;
    status =  bcm_l3_route_add(unit, &defroute);
    if (status < 0) {
        printf("bcm_l3_route_add failed for 0x%x !!! status = %d\n", ip, status);
        return status;
    }      
    return BCM_E_NONE;
}


int configure_nexthop(int unit, int dest_port, int vid, unsigned char* smac, unsigned char* dmac, bcm_if_t *nh_index, int flags) {
    bcm_l3_intf_t intf;
    bcm_l3_egress_t   egress_object;   
    int status;

    bcm_l3_intf_t_init(&intf);  

    sal_memcpy(intf.l3a_mac_addr, smac, 6);
    printf("Hey hey i am in configure_nexthop\n");
    intf.l3a_vid =  vid;
    intf.l3a_mtu = 4096;
    intf.l3a_flags = BCM_L3_ADD_TO_ARL;
    intf.l3a_vrf = 10;
    status = bcm_l3_intf_create(unit, &intf);
    if (status < 0) {
        printf("bcm_l3_intf_create failed!!! status = %d\n",status);
        return status;
    }

    printf("bcm_l3_intf_create intf - %d\n", intf.l3a_intf_id);

    /* Now create the l3_egress object which gives the index to l3 interface during lookup */
    bcm_l3_egress_t_init(&egress_object);

    /* Now copy the nexthop destmac, set dest port and index of L3_INTF table which is created above */
    sal_memcpy(egress_object.mac_addr, dmac,6);
    egress_object.intf = intf.l3a_intf_id; 
    egress_object.port = dest_port;  
    egress_object.flags = flags; 
    /* go ahead and create the egress object */ 
    status = bcm_l3_egress_create(unit, 0, &egress_object, nh_index); 
    if (status < 0) {
        printf("1 bcm_l3_egress_create failed !!! status = %d\n",status);
        return status;    
    }
    return 0;
}

int configure_flex_macsec_encoding(int vlan, bcm_gport_t subport_gport)
{

    int unit =0;
    int i = 0;
    bcm_flow_handle_t handle;
    bcm_flow_option_id_t option_id;
    bcm_flow_encap_config_t encap_config;
    bcm_flow_encap_config_t_init(&encap_config);
    bcm_flow_logical_field_t logical_fields;
    bcm_flow_logical_field_t_init(&logical_fields);
    bcm_flow_field_id_t field_id;
    print bcm_flow_handle_get(unit, "SUBPORT_SVTAG_ENCRYPT", &handle);
    print handle;
    /*print bcm_flow_option_id_get(unit, handle, "LOOKUP_DGPP_OVLAN_ASSIGN_CLASS_ID", &option_id);*/
    print bcm_flow_option_id_get(unit, handle, "LOOKUP_DGPP_OVLAN_INTPRI_ASSIGN_CLASS_ID", &option_id);
    print option_id;

    encap_config.flow_handle = handle;
    encap_config.flow_option = option_id;
    encap_config.flow_port = subport_gport; /* Destination port number */
    encap_config.flags |= BCM_FLOW_PORT_ENCAP_FLAG_MACSEC_ENCRYPT;
    encap_config.vlan = vlan;
    encap_config.pri=i;
    encap_config.class_id = 15;
    encap_config.criteria = BCM_FLOW_ENCAP_CRITERIA_DGPP_VLAN_INT_PRI;
    encap_config.valid_elements = (BCM_FLOW_ENCAP_FLOW_PORT_VALID | BCM_FLOW_ENCAP_CLASS_ID_VALID | BCM_FLOW_ENCAP_VLAN_VALID);
    print bcm_flow_encap_add(unit, &encap_config, 0, &logical_fields);

    return BCM_E_NONE;
}

int delete_subport(int unit, bcm_port_t subport_gport)
{

    bcm_subport_config_t       subport_cfg;
    bcm_gport_t                      subport_gport = 0;

    /* add subports to subport group */
    bcm_subport_config_t_init(&subport_cfg);


    printf("Get the subport from port. \n");
    print bcm_subport_port_get(unit, subport_gport, &subport_cfg);
    print subport_cfg;

    printf("Delete the subport 0x%x\n", subport_gport);
    print bcm_subport_port_delete(unit, subport_gport);

    return BCM_E_NONE;
}

int create_subport(int vlan, int phy_port, bcm_gport_t *subport_gport)
{

    int unit = 0;
    bcm_stk_modid_config_t mod_cfg;
    int subtag_vlan = 15; /* Used to decap subtag's subport number. */
    mod_cfg.modid = 5;
    mod_cfg.num_ports = 10;
    mod_cfg.modid_type = bcmStkModidTypeCoe;

    print bcm_stk_modid_config_add(0, &mod_cfg);

    printf("Create subport group ...\n");

    int                                         port = phy_port;  /* Physical port number */
    bcm_gport_t                         port_gport;
    uint16                                   subport_tag_tpid = 0x8100;

    print bcm_port_control_set(0, port, bcmPortControlSubportTagEnable, 1);
    print bcm_port_gport_get(0, port, &port_gport);

    bcm_subport_group_config_t  subport_group_cfg;
    bcm_gport_t                     subport_group_gport;
    subport_group_cfg.port  = port_gport;
    subport_group_cfg.flags = BCM_SUBPORT_GROUP_TYPE_SUBPORT_TAG;
    print bcm_subport_group_create(unit, &subport_group_cfg, &subport_group_gport);
    print subport_group_gport;
    printf("Adding subports  ...\n");
    bcm_subport_config_t       subport_cfg;
    /* add subports to subport group */
    bcm_subport_config_t_init(&subport_cfg);
    subport_cfg.group = subport_group_gport;
     /* PHB */
    subport_cfg.prop_flags  = BCM_SUBPORT_PROPERTY_PHB;
    subport_cfg.int_pri     = 2;
    subport_cfg.color       = 2;
    subport_cfg.subport_modport = 0;
    subport_cfg.pkt_vlan = subtag_vlan;
    print bcm_subport_port_add(unit, &subport_cfg, subport_gport);
    print *subport_gport;


    printf("Setting the port vlan membership. ");
    print bcm_port_vlan_member_set(unit, *subport_gport, BCM_PORT_VLAN_MEMBER_INGRESS | BCM_PORT_VLAN_MEMBER_EGRESS);

    printf("Setting the port vlan membership for vlan= %d \n", vlan);
    print bcm_vlan_gport_add(unit, vlan, *subport_gport, 0);

    return BCM_E_NONE;
}



int create_vlan_and_add_ports(int unit, int vid, int tport, int uport) {
    bcm_pbmp_t pbmp;
    bcm_pbmp_t upbmp;
    int status;

    status = bcm_vlan_create(unit, vid);
    if (status < 0 && status != -8) { /* -8 is EEXISTS*/
        return -1;
    }
    BCM_PBMP_PORT_ADD(pbmp, tport);
    BCM_PBMP_PORT_ADD(upbmp, uport);
    status = bcm_vlan_port_add(unit, vid, pbmp,upbmp);
    if (status < 0) {
        printf("Failed to add ports to vlan %d with status = %d\n",vid, status);
        return -1;
    }
}

int
config_efp(int unit)
{
    bcm_field_group_config_t group_config;
    uint32 data,mask;
    bcm_field_entry_t entry;
    print bcm_field_group_config_t_init(&group_config);
    print BCM_FIELD_QSET_INIT(group_config.qset);
    print BCM_FIELD_ASET_INIT(group_config.aset);

    print BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyStageEgress);
    print BCM_FIELD_QSET_ADD(group_config.qset, bcmFieldQualifyEgressPortCtrlType);
    group_config.priority = 11;
printf(" Creating the group\n");
    print bcm_field_group_config_create(unit, &group_config);
printf(" Creating the Entry\n");
    print bcm_field_group_config_create(unit, &group_config);
    print bcm_field_entry_create(unit, group_config.group, &entry);
    print entry;
    /* Qualify the egress port type to add SVTAG. */
    print bcm_field_qualify_EgressPortCtrlType(unit, entry, bcmFieldEgressPortCtrlTypeSubportSvtagEncrypt);
    print bcm_field_action_add(unit, entry, bcmFieldActionSvtagSignature, 0x11, 0);
    print bcm_field_entry_install(unit,entry);
    print bcm_field_action_add(unit, entry, bcmFieldActionSvtagOffset, 0xE8, 0);
    print bcm_field_entry_install(unit,entry);
    print bcm_field_action_add(unit, entry, bcmFieldActionSvtagPrio, 0, 0);
    print bcm_field_entry_install(unit,entry);
    print bcm_field_action_add(unit, entry, bcmFieldActionSvtagSubportNum, 10, 0);
    print bcm_field_entry_install(unit,entry);
    print bcm_field_action_add(unit, entry, bcmFieldActionSvtagEnable, 0, 1);
    printf(" Installing the Entry\n");
    print bcm_field_entry_install(unit,entry);

    return BCM_E_NONE;
}

void tx_ipv4_packet_from_cpu(int unit, int vlan_tag,
                             bcm_mac_t src_mac, bcm_mac_t dst_mac,
                             bcm_ip_t src_ip, bcm_ip_t dst_ip, bcm_port_t dest_port, bcm_gport_t subport)
{   
    bcm_pkt_t *pkt;
    int pkt_size = 72; /* including VLAN Tag if BCM_PKT_F_NO_VTAG = 0 */
    int flags = BCM_TX_CRC_APPEND;

    /* EtherType(2B) Version+IHL+TOS(2B) Total Length (2B) + Identification (2B)
       Flags+Fragment offset(2B) TTL(1B: TTL=0x10) Protocol(1B: UDP=0x11) Header Checksum(2B) */
    char ipv4_ether_type[2] = {0x08, 0x00};
    char ether_type[2] = {0x81, 0x00};
    char vlan[2] = {0x00, 0x16};
    /* ipv4.total_length = 68 - 12 - 2 - 4 - 4 = 46; */
    char ipv4_hdr[24]    = {0x45, 0x00, 0x00, 0x2E,
                            0x00, 0x00, 0x00, 0x00,
                            0x40, 0xFF, 0xDA, 0x7E,
                            0xC0, 0xA8, 0x0A, 0x01,
                            0xC0, 0xA8, 0x14, 0x01};

    if (BCM_E_NONE != bcm_pkt_alloc(unit, pkt_size, flags, &pkt)) {
        printf("TX: Failed to allocate packets\n");
    }

    printf("Start setting pkt data ... \n");
    /* packet init */
    sal_memset(pkt->_pkt_data.data, 0x00, pkt_size);

    printf("Start setting pkt data DMAC, SMAC ... \n");
    BCM_PKT_HDR_DMAC_SET(&pkt, dst_mac);
    BCM_PKT_HDR_SMAC_SET(&pkt, src_mac);
    printf("Set svtag ... \n");
    BCM_PKT_HDR_TPID_SET(&pkt, 0x8847);
    BCM_PKT_HDR_VTAG_CONTROL_SET(&pkt, 0xf);
    printf("Set vlan and ip header ... \n");
    sal_memcpy(pkt->pkt_data->data+16, ether_type, 2);
    sal_memcpy(pkt->pkt_data->data+18, vlan, 2);
    sal_memcpy(pkt->pkt_data->data+20, ipv4_ether_type, 2);
    sal_memcpy(pkt->pkt_data->data+22, ipv4_hdr, 24);
    BCM_PKT_PORT_SET(&pkt, dest_port, 0, 0);
    pkt->dst_gport = subport;

    pkt->call_back = NULL;

    if (BCM_E_NONE != bcm_tx(unit, pkt, NULL)) {
        printf("bcm tx error\n");
    }

    bcm_pkt_free(unit, pkt);
}


int
add_to_l2_table(int unit, bcm_mac_t mac, bcm_vpn_t vpn_id, int port)
{
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, mac, vpn_id);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = port;
    return bcm_l2_addr_add(unit, &l2_addr);
}


void execute(int unit)
{
    unsigned char src_mac[] = {0x00,0x00,0x00,0x00,0xaa,0xaa};
    unsigned char src1_mac[]  = {0x00,0x01,0x02,0x03,0x04,0x09}; 
    unsigned char dst_mac[] = {0x00,0x00,0x00,0x00,0xbb,0xbb};
    unsigned char dst_mac1[] = {0x00,0x06,0x07,0x08,0x09,0x0b};
    int vlan1 = 21, vlan2 = 22;
    int ge1 = 1;
    int ge3 = 7;
    int i,x, vrf = 0;
    bcm_gport_t subport_gport = 0;

    bcm_switch_control_set(unit,bcmSwitchL3EgressMode,1);
    printf("Create vlans and add ports \n");
    create_vlan_and_add_ports(unit, vlan1, ge1, 0);
    create_vlan_and_add_ports(unit, vlan2, ge3, 0);
    printf("About to call configure_nexthop\n");

    configure_nexthop(unit, ge1, vlan1,  src_mac, dst_mac, &nh_index[0], 0);
    printf("nexthop created with index  =  %d\n", nh_index[0]);

    create_subport(vlan2, ge3, &subport_gport);

    configure_flex_macsec_encoding(vlan2, subport_gport);

    /* config EFP */
    config_efp(unit);

    printf("Create the egress object with the subport. \n");

    printf("Setting routes for forwaring.. \n");
    configure_v4_defroute(unit, 10,  0x0a000001, 0xfffffff0, nh_index[0]);
    configure_v4_defroute(unit, 10,  0xC0A81401, 0xfffffff0, nh_index[0]);

     bshell(unit, "port xe2 lb=mac");

     tx_ipv4_packet_from_cpu(unit, vlan2, src_mac, dst_mac, 0x010aa8c0, 0x0114a8c0, ge3, subport_gport);

}

print execute(0);
