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
 *                                                                           *
 *                                                                           *
 *   # Enable lookback on port and send traffic.                             *
 *  BCM>c coe_subport_l2uc.c                                                 *
 *  BCM> port xe0 lb=mac                                                     *
    BCM>tx 1 pbm=xe0 data=00 00 00 00 AA AA 00 00 00 00 BB BB 81 00 00 15 08 00 45 00 00 2E 00 00 00 00 40 FF AF CB 0A 00 00 01 C0 00 00 05 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F 10 11 12 13 14 15 16 17 18 19
 */





cint_reset();

bcm_if_t nh_index[2];
printf("Create CoE module ...\n");


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
encap_config.vlan = vlan;
encap_config.pri=i;

/* Make this a macsec enabled flow. */
encap_config.flags |= BCM_FLOW_PORT_ENCAP_FLAG_MACSEC_ENCRYPT;
/* CLass id will carry the encap subport number. */
encap_config.class_id = 20;
encap_config.criteria = BCM_FLOW_ENCAP_CRITERIA_DGPP_VLAN_INT_PRI;
encap_config.valid_elements = (BCM_FLOW_ENCAP_FLOW_PORT_VALID | BCM_FLOW_ENCAP_CLASS_ID_VALID | BCM_FLOW_ENCAP_VLAN_VALID);


print bcm_flow_encap_add(unit, &encap_config, 0, &logical_fields);


print bshell(0,"d EGR_VLAN_XLATE_2_SINGLE");
}



int create_subport(int vlan, int phy_port, bcm_gport_t *subport_gport) {

int unit = 0;
bcm_stk_modid_config_t mod_cfg;

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

                int subtag_vlan = 4;

                subport_cfg.pkt_vlan = subtag_vlan;

                print bcm_subport_port_add(unit, &subport_cfg, subport_gport);
                print *subport_gport;


                printf("Setting the port vlan membership. ");
                print bcm_port_vlan_member_set(unit, *subport_gport, BCM_PORT_VLAN_MEMBER_INGRESS | BCM_PORT_VLAN_MEMBER_EGRESS);

                printf("Setting the port vlan membership for vlan= %d \n", vlan);
                print bcm_vlan_gport_add(unit, vlan, *subport_gport, 0);

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
add_to_l2_table(int unit, bcm_mac_t mac, bcm_vpn_t vpn_id, int port)
{
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, mac, vpn_id);
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = port;
    return bcm_l2_addr_add(unit, &l2_addr);
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


void execute(int unit, int debug, int init)
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
    bcm_mac_t mac_s;

    sal_memcpy(mac_s, src_mac, 6);

    printf("Create vlans and add ports \n");
    create_vlan_and_add_ports(unit, vlan1, ge1, 0);
    create_vlan_and_add_ports(unit, vlan1, ge3, 0);
    create_vlan_and_add_ports(unit, vlan2, ge3, 0);
    printf("About to call configure_nexthop\n");


    /*Create subport gport. */
    create_subport(vlan1, ge3, &subport_gport);

    configure_flex_macsec_encoding(vlan1, subport_gport);

    /* configure EFP */
    printf(" Creating the EFP config\n");
    print config_efp(unit);

    /* Add a l2 address. */
    add_to_l2_table(unit, mac_s, vlan1, subport_gport);

}

print execute(0,0,0);
