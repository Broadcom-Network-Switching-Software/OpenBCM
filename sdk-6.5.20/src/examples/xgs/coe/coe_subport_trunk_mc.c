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
 *  BCM>c coe_subport_subport_trunk_mc.c                                     *
 *  BCM> port xe0 lb=mac                                                     *
 *  BCM>tx 1 pbm=xe0 data=01005E0101010000000000118100000C08004500002E0000000040FFC324C0A81402E1010101000102030405060708090A0B0C0D0E0F10111213141516171819F620FE33
 */


/* Reset c interpreter*/
cint_reset ();

int trunk_create(int unit, bcm_gport_t port, int nport, int psc, bcm_trunk_t *tid)
{
    int i, rv = 0;
    int flags = 0;
    bcm_trunk_info_t    trunk_info;
    bcm_trunk_member_t  *p_trunk_member;

    bcm_trunk_member_t  trunk_member_tmp;

    bcm_trunk_info_t_init(&trunk_info);

    /* sizeof(bcm_trunk_member_t) is 16 */
    p_trunk_member = sal_alloc (sizeof(trunk_member_tmp) * nport, "Trunk Members");

    trunk_info.psc        = psc;
    trunk_info.dlf_index  = 0;
    trunk_info.mc_index   = 0;
    trunk_info.ipmc_index = 0;
    /* Create trunk group, allocate trunk group id  */
    printf("Create trunk group ");
    rv = bcm_trunk_create(unit, flags, tid);
    if (BCM_FAILURE(rv)) {
        printf ("failed, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf(": Done. Trunk group id: %d\n", *tid);
    /* Add port to trunk group */
    for(i=0; i<nport; i++) {
        bcm_trunk_member_t_init (&p_trunk_member[i]);
        p_trunk_member[i].gport = port;

    }

    printf("Add ports to trunk group %d ", *tid);
    rv = bcm_trunk_set(unit, *tid, &trunk_info, nport, p_trunk_member);
    if (BCM_FAILURE(rv)) {
        printf ("failed, error code: %s\n", bcm_errmsg(rv));
        return rv;
    }
    printf(": Done\n");
    sal_free (p_trunk_member);
    return 0;
}

int
vlan_create_add_port(int unit, int vid, int port)
{
    bcm_pbmp_t         pbmp, upbmp;

    BCM_IF_ERROR_RETURN(bcm_vlan_create(unit, vid));
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(upbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    BCM_PBMP_PORT_ADD(pbmp, port+1);
    BCM_IF_ERROR_RETURN(bcm_vlan_port_add(unit, vid, pbmp, upbmp));

    return BCM_E_NONE;
}

int
create_l3_interface(int unit, bcm_mac_t local_mac, int vid, bcm_if_t *intf_id)
{
    bcm_l3_intf_t      l3_intf;

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_flags |= BCM_L3_ADD_TO_ARL;
    sal_memcpy(l3_intf.l3a_mac_addr, local_mac, sizeof(local_mac));
    l3_intf.l3a_vid = vid;
    BCM_IF_ERROR_RETURN(bcm_l3_intf_create(unit, &l3_intf));
    *intf_id = l3_intf.l3a_intf_id;

    return BCM_E_NONE;
}



/* Create L3 egress object */
int
create_l3_egr_obj(int unit, int flags, int l3_if, bcm_mac_t nh_mac,
               bcm_gport_t gport, bcm_vlan_t p_vlan,
               bcm_mpls_label_t vc_label_1, bcm_if_t *egr_obj_id)
{   
    bcm_l3_egress_t    l3_egress;

    bcm_l3_egress_t_init(&l3_egress);
    l3_egress.flags = flags|BCM_L3_IPMC;
    sal_memcpy(l3_egress.mac_addr, nh_mac, sizeof(nh_mac));
    l3_egress.intf  = l3_if;
    l3_egress.port = gport;
    l3_egress.vlan = p_vlan;
    BCM_IF_ERROR_RETURN(bcm_l3_egress_create(unit, 0,
                                             &l3_egress, egr_obj_id));

    return BCM_E_NONE;
}

/* Add IPMC entry */
int
add_ipmc(int unit, bcm_vlan_t vid, bcm_gport_t gport,
         bcm_ip_t mc_ip, bcm_multicast_t ipmcast_group)
{   
    bcm_ipmc_addr_t        ipmc_addr;

    bcm_ipmc_addr_t_init(&ipmc_addr);
    ipmc_addr.flags = BCM_IPMC_SOURCE_PORT_NOCHECK;
    ipmc_addr.group = ipmcast_group;
    ipmc_addr.mc_ip_addr = mc_ip;
    ipmc_addr.vid = vid;
    ipmc_addr.port_tgid = gport;
    BCM_IF_ERROR_RETURN(bcm_ipmc_add(unit, &ipmc_addr));

    return BCM_E_NONE;
}

/* Create multicast egress object */
int
create_mc_egr_obj(int unit, bcm_if_t l3_egr_if, bcm_gport_t gport,
                  bcm_multicast_t mcg)
{
    bcm_if_t           encap_id;

    BCM_IF_ERROR_RETURN(bcm_multicast_egress_object_encap_get(unit, mcg,
                                                              l3_egr_if,
                                                              &encap_id));
    BCM_IF_ERROR_RETURN(bcm_multicast_egress_add(unit, mcg, gport, encap_id));

    return BCM_E_NONE;
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
    print bcm_field_action_add(unit, entry, bcmFieldActionSvtagEnable, 0, 0);
printf(" Installing the Entry\n");
    print bcm_field_entry_install(unit,entry);

    bshell(0, "d chg EFP_POLICY_TABLE 3072");


    return BCM_E_NONE;
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
    encap_config.class_id = 20;
    encap_config.criteria = BCM_FLOW_ENCAP_CRITERIA_DGPP_VLAN_INT_PRI;
    encap_config.valid_elements = (BCM_FLOW_ENCAP_FLOW_PORT_VALID | BCM_FLOW_ENCAP_CLASS_ID_VALID | BCM_FLOW_ENCAP_VLAN_VALID);
    print bcm_flow_encap_add(unit, &encap_config, 0, &logical_fields);

    return BCM_E_NONE;
}


int create_subport(int vlan, int phy_port, bcm_gport_t *subport_gport)
{

    int unit = 0;
    bcm_stk_modid_config_t mod_cfg;
    int subtag_vlan = 4; /* Used to decap subtag's subport number. */
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



int main(int unit)
{

    bcm_mpls_label_t           tun_label_1 = 101;
    bcm_mpls_label_t           vc_label_1 = 201;
    bcm_mpls_label_t           tun_label_2 = 301;
    bcm_mpls_label_t           vc_label_2 = 401;
    bcm_multicast_t            ipmc_group;
    bcm_ipmc_addr_t            ipmc;
    bcm_gport_t                gport_in;
    bcm_gport_t                gport_tunnel_1;
    bcm_gport_t                gport_tunnel_2;
    bcm_port_t                 port_in = 2;
    bcm_port_t                 port_tunnel_1 = 7;
    bcm_port_t                 port_tunnel_2 = 8;
    bcm_pbmp_t                 pbmp, upbmp;
    bcm_vlan_t                 vid_in = 12;
    bcm_vlan_t                 tunnel_vid_1 = 13;
    bcm_vlan_t                 tunnel_vid_2 = 14;
    bcm_mac_t                  tunnel_mac  = {0x00,0x00,0x00,0x00,0xBB,0xBB};
    bcm_mac_t                  tunnel_nh_mac_1  = {0x00,0x00,0x00,0x00,0x13,0x13};
    bcm_mac_t                  tunnel_nh_mac_2  = {0x00,0x00,0x00,0x00,0x14,0x14};
    bcm_mac_t                  tunnel_nh_mac_null  = {0x00,0x00,0x00,0x00,0x00,0x00};
    bcm_ip_t                   mc_ip_addr=0xe1010101;
    bcm_if_t                   l3_egr_if_1;
    bcm_if_t                   l3_egr_if_2;
    bcm_if_t                   tunnel_obj_egr_1;
    bcm_if_t                   tunnel_obj_egr_2;
    int                        unit = 0;
    uint8                      ttl=64;
    bcm_gport_t                subport_gport = 0;
    bcm_trunk_t                tid;
    bcm_gport_t                trunk_gport;

    bcm_multicast_subport_trunk_info_t info;

    bcm_multicast_subport_trunk_info_t_init(&info);

    bcm_switch_control_set(unit, bcmSwitchL3EgressMode, TRUE);

    bcm_port_gport_get(unit, port_in, &gport_in);
    bcm_port_gport_get(unit, port_tunnel_1, &gport_tunnel_1);
    bcm_port_gport_get(unit, port_tunnel_2, &gport_tunnel_2);

    /* Create VLAN & add member ports */
    print vlan_create_add_port(unit, vid_in, port_in);
    print vlan_create_add_port(unit, tunnel_vid_1, port_tunnel_1);
    /*print vlan_create_add_port(unit, tunnel_vid_1, port_tunnel_2);*/

    /* Tunnel L3 interface */
    print create_l3_interface(unit, tunnel_mac, tunnel_vid_1, &l3_egr_if_1);


    /* create subport. */
    print create_subport(tunnel_vid_1, 7, &subport_gport);

    /* Create the flex encoding. */
    print configure_flex_macsec_encoding(tunnel_vid_1, subport_gport);

    /* configure EFP */
    printf(" Creating the EFP config\n");
    print config_efp(unit);

    /* Tunnel egress object */
    print create_l3_egr_obj(unit, 0, l3_egr_if_1, tunnel_nh_mac_null, subport_gport,
                        tunnel_vid_1, vc_label_1, &tunnel_obj_egr_1);
    printf("the egress object created = %x \n", tunnel_obj_egr_1);

    /* Create L3 multicast group */
    print bcm_multicast_create(unit, BCM_MULTICAST_TYPE_L3, &ipmc_group);
    print ipmc_group;

    /* Set up IPMC  */
    print add_ipmc(unit, vid_in, gport_in, mc_ip_addr, ipmc_group);

    /* Create trunk. */
    printf("Create the subport trunk.. ");
    print trunk_create(unit, subport_gport, 1, BCM_TRUNK_PSC_SRCDSTMAC, &tid);
    BCM_GPORT_TRUNK_SET(trunk_gport, tid);

    info.trunk_gport = trunk_gport;
    info.fwd_subport_gport = subport_gport;

    /* Set the trunk forwarding port. */
    bcm_multicast_subport_trunk_forwarding_set(unit, ipmc_group, &info);

    /* L3 encap */
    print create_mc_egr_obj(unit, tunnel_obj_egr_1, trunk_gport, ipmc_group);

    return BCM_E_NONE;
}
