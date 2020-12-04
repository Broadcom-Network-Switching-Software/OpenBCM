/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/
/*
 * Test Scenario: flooding, UC packets
 *
 * soc properties:
 * egress_acl_two_pass_enable=1
 * field_presel_mgmt_advanced_mode=1
 *
 * RCY port:
 * ucode_port_40.BCM88670=RCY.0:core_0.40
 * ucode_port_41.BCM88670=RCY.0:core_1.41
 * first_header_size_40=48
 * first_header_size_41=48
 *
 * how to run the test: 
 * cd ../../../../src/examples/dpp
 * cint ../sand/utility/cint_sand_utils_global.c  
 * cint ../sand/utility/cint_sand_utils_mpls.c
 * cint utility/cint_utils_l3.c 
 * cint cint_ipv6_fap.c 
 * cint cint_two_pass_egress_acl.c
 * c
 * two_pass_egress_acl_rcy_port[0]=40;
 * two_pass_egress_acl_rcy_port[1]=202;
 * two_pass_egress_acl_service_init(200,13,10,20,0);
 * two_pass_egress_acl_service_init(201,15,11,21,1);
 * two_pass_egress_acl_dvapi(0);
 * 
 * Tests:
 * 1. Core 1 in port -> RCY port 40 -> ETH port 13:
 *     tx 1 psrc=200 data=0x0000000055340000000000018100000a86dd608000100014060a0000000000000000000000000000012301001600350070000000db07000000003344556600a1a2a300b1b2b3500f01f536620000
 *
 *     Expected RX (packet is routed with correct encapsulation):
 *         00000000CD1D0000000055348100001486DD600000000028007F000000000000000000000000000001230100160035007000
 *
 * 2. Core 0 in port -> RCY port 202 (CPU)
 *     tx 1 psrc=201 data=0x0000000055340000000000018100000b86dd608000100014060a0000000000000000000000000000012301001600350070010000db07000000003344556600a1a2a300b1b2b3500f01f536620000
 * 
 *     Expected RX  (packet has proper system header and dummy ETH header):
 *         01600064e5220001580810934c0000540000000080c01001000000000000000000000000000000000000000000000000 <-- system header
 *         00000000112210203040506086dd60000000002800800000000000000000000000000000012301001600350070010000db070000000000000000 <-- payload with dummy ETH header
 */
struct two_pass_egress_acl_service_s{
    bcm_gport_t voq;
    bcm_gport_t voq_conn;
    int in_port;
    int out_port;
    int in_vlan;
    int out_vlan;
    int in_rif;
    int out_rif;
    int next_hop;
    int egress_intf;
    bcm_l3_route_t route_entry;
};

two_pass_egress_acl_service_s two_pass_egress_acl_service[2];

struct two_pass_egress_acl_group_s{
    bcm_field_group_t first_pass_add_dsp_grp;
    bcm_field_entry_t first_pass_add_dsp_ent;
    bcm_field_group_t second_pass_update_profile_grp;
    bcm_field_entry_t second_pass_update_profile_ent;
    bcm_field_group_t second_pass_update_dest_grp;
    bcm_field_entry_t second_pass_update_dest_ent;
};

two_pass_egress_acl_group_s two_pass_egress_acl_group;

bcm_mac_t mac_l3_ingress = {0x00, 0x00, 0x00, 0x00, 0x55, 0x34}; /* incoming DA (myMAc) */
bcm_mac_t mac_l3_egress  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* outgoing DA */

bcm_mac_t dummy_eth_da = {0x00, 0x00, 0x00, 0x00, 0x11, 0x22}; /* DA of the dummy ETH header */
bcm_mac_t dummy_eth_sa = {0x10, 0x20, 0x30, 0x40, 0x50, 0x60}; /* SA of the dummy ETH header */

int MAX_NOF_CORES = 2;
int two_pass_egress_acl_nof_service = 2;
int two_pass_egress_acl_in_port_presel_id = 10;
int two_pass_egress_acl_rcy_port_presel_id = 11;
int two_pass_egress_acl_rcy_port[MAX_NOF_CORES] = { 40, 41 };

int two_pass_egress_acl_service_init(int in_port, int out_port, int in_vlan, int out_vlan, int index) {
    two_pass_egress_acl_service[index].in_port = in_port;
    two_pass_egress_acl_service[index].out_port = out_port;
    two_pass_egress_acl_service[index].in_vlan = in_vlan;
    two_pass_egress_acl_service[index].out_vlan = out_vlan;

    return 0;
}

/*
 * All traffics that requires two pass egress ACL processing are forwarded to the recycle port at the egress FAP.
 * Packets are forwarded to system port ID { 1'b1, 11'b dst_port }, where dst_port is the real destination.
 * System port { 1'b1, 11'b dst_port } is forwarded to the rcy_port which resides on the same FAP & core of the dst_port.
 * At egress FAP after recycling, system port is used to map to the real destination.
 *
 *     system port 0x801 \              / front panel port 1
 *     system port 0x802 - - rcy_port - - front panel port 2
 *     system port 0x803 /              \ front panel port 3
 */
int two_pass_egress_acl_add_system_port_mapping(int unit, int out_port, two_pass_egress_acl_service_s *service) {
    bcm_gport_t sysport;
    bcm_gport_t modport;
    bcm_port_interface_info_t interface_info;
    bcm_port_mapping_info_t mapping_info;
    bcm_cosq_ingress_queue_bundle_gport_config_t voq_conf;
    bcm_gport_t queue;
    bcm_gport_t voq_con;
    bcm_cosq_voq_connector_gport_t config;
    int cos;
    int num_cos = 8;
    uint32 flags;
    int in_core;
    int out_core;
    int base_modid;
    int in_modid;
    int out_modid;
    int tm_port;
    int rv;

    service->out_port = out_port;
    
    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport, 0x800 | out_port);

    rv = bcm_port_get(unit, out_port, &flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_get failed\n");
        return rv;
    }
    
    if (MAX_NOF_CORES == 2) {
        out_core = mapping_info.core;
        in_core = (mapping_info.core == 0); /* the other core */
    }
    else {
        in_core = out_core = mapping_info.core;
    }

    rv = bcm_stk_my_modid_get(unit, &base_modid);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stk_my_modid_get\n");
        return rv;
    }

    rv = bcm_port_get(unit, two_pass_egress_acl_rcy_port[out_core], &flags, &interface_info, &mapping_info);
    if (rv != BCM_E_NONE) {
        printf("bcm_port_get failed\n");
        return rv;
    }

    if (MAX_NOF_CORES == 2) {
        out_modid = base_modid + mapping_info.core;
        in_modid = base_modid + (mapping_info.core == 0); /* the other core */
    }
    else {
        in_modid = out_modid = base_modid;
    }

    tm_port = mapping_info.tm_port;
    
    BCM_GPORT_MODPORT_SET(modport, out_modid, tm_port);
	rv = bcm_stk_sysport_gport_set(unit, sysport, modport);
    if (rv != BCM_E_NONE) {
        printf("bcm_stk_sysport_gport_set failed\n");
    }

    voq_conf.flags = BCM_COSQ_GPORT_UCAST_QUEUE_GROUP;
    voq_conf.local_core_id = BCM_CORE_ALL;
    voq_conf.port = modport;
    voq_conf.numq = num_cos;
    for (cos = 0; cos < num_cos; ++cos) {
        voq_conf.queue_atrributes[cos].delay_tolerance_level = BCM_COSQ_DELAY_TOLERANCE_10G_SLOW_ENABLED;
        voq_conf.queue_atrributes[cos].rate_class = 0;
    }
    rv = bcm_cosq_ingress_queue_bundle_gport_add(unit, &voq_conf, &queue);    
    service->voq = queue;

    config.flags = BCM_COSQ_GPORT_VOQ_CONNECTOR;
    config.nof_remote_cores = 2;
    config.numq = 8;
    config.remote_modid = 0/*in_modid*/;
    BCM_COSQ_GPORT_E2E_PORT_SET(config.port, two_pass_egress_acl_rcy_port[out_core]);
    
    rv = bcm_cosq_voq_connector_gport_add(unit, &config, &voq_con);
    service->voq_conn = voq_con;

    for (cos = 0; cos < num_cos; cos++) {
        bcm_cosq_gport_info_t gport_info;

        /* Each VOQ connector attach suitable HR */
        rv = bcm_cosq_gport_sched_set(unit, voq_con, cos, BCM_COSQ_SP0,0);
    
        BCM_COSQ_GPORT_E2E_PORT_TC_SET(gport_info.in_gport, two_pass_egress_acl_rcy_port[out_core]);
        gport_info.cosq = cos/4;
        rv = bcm_cosq_gport_handle_get(unit, bcmCosqGportTypeSched, &gport_info);     
          
        /* attach HR SPi to connecter */    
        rv = bcm_cosq_gport_attach(unit, gport_info.out_gport, voq_con, cos);
    }

    bcm_cosq_gport_connection_t connection;

    connection.flags = BCM_COSQ_GPORT_CONNECTION_INGRESS | BCM_COSQ_GPORT_CONNECTION_MODID_OVERRIDE;
    connection.remote_modid = out_modid;
    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(
       connection.voq, BCM_CORE_ALL, BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(queue));
     connection.voq_connector = voq_con;
    rv = bcm_cosq_gport_connection_set(unit, &connection);

    connection.flags = BCM_COSQ_GPORT_CONNECTION_EGRESS;
    connection.remote_modid = in_modid;
    BCM_GPORT_UNICAST_QUEUE_GROUP_CORE_QUEUE_SET(
       connection.voq, BCM_CORE_ALL, BCM_GPORT_UNICAST_QUEUE_GROUP_QID_GET(queue));
     connection.voq_connector = voq_con;
    rv = bcm_cosq_gport_connection_set(unit, &connection);

    return rv;
}

/*
 * The destination used for out_port is system port { 1'b1, 11'b out_port }.
 */
int two_pass_egress_acl_add_ipv6_routing(int unit, int in_port, int out_port, int in_vlan, int out_vlan, two_pass_egress_acl_service_s *service, uint8 subnet) {
    create_l3_intf_s intf;    
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid;
    bcm_gport_t sysport;
    int egress_intf;
    int encap_id;
    bcm_ip6_t dip;
    bcm_ip6_t mask;
    bcm_l3_route_t l3rt;
    int i;
    int rv;

    rv = bcm_vlan_create(unit, in_vlan);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("failed to create vlan %d", in_vlan);
    }
    service->in_vlan = in_vlan;

    rv = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
    if (rv != BCM_E_NONE) {
        printf("fail add port(0x%08x) to vlan(%d)\n", in_port, in_vlan);
        return rv;
    }
    service->in_port = in_port;

    intf.vsi = in_vlan;
    intf.my_global_mac = mac_l3_ingress;
    intf.my_lsb_mac = mac_l3_ingress;

    rv = l3__intf_rif__create(unit, &intf);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create");
        return rv;
    }
    service->in_rif = intf.rif;

    /* create out-rif */
    rv = bcm_vlan_create(unit, out_vlan);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
        printf("failed to create vlan %d", out_vlan);
        return rv;
    }

    intf.vsi = out_vlan;
    rv = l3__intf_rif__create(unit, &intf);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create");
        return rv;
    }
    service->out_vlan = out_vlan;
    service->out_rif = intf.rif;

    rv = bcm_vlan_gport_add(unit, out_vlan, out_port, 0);
    if (rv != BCM_E_NONE) {
        printf("fail add port(0x%08x) to vlan(%d)\n", out_port, out_vlan);
        return rv;
    }

    BCM_GPORT_SYSTEM_PORT_ID_SET(sysport, 0x800 | out_port);

    bcm_l3_egress_t_init(&l3eg);

    sal_memcpy(l3eg.mac_addr, mac_l3_egress, 6);
    l3eg.intf = service->out_rif;
    l3eg.vlan = out_vlan;                                
    l3eg.port = sysport;    
    l3egid = 0;
    rv = bcm_l3_egress_create(unit, 0, &l3eg, &l3egid);

    service->next_hop = l3eg.encap_id;
    service->egress_intf = l3egid;
    
    /* UC IPV6 DIP: */
    dip[15]= 0; /* LSB */
    dip[14]= 0;
    dip[13]= 0;
    dip[12]= 0;
    dip[11]= 0x7;
    dip[10]= 0xdb;
    dip[9] = 0;
    dip[8] = 0;
    dip[7] = subnet;
    dip[6] = 0x70;
    dip[5] = 0;
    dip[4] = 0x35;
    dip[3] = 0;
    dip[2] = 0x16;
    dip[1] = 0;
    dip[0] = 0x1; /* MSB */
    
    /* UC IPV6 DIP MASK: */
    mask[15]= 0xff;
    mask[14]= 0xff;
    mask[13]= 0xff;
    mask[12]= 0xff;
    mask[11]= 0xff;
    mask[10]= 0xff;
    mask[9] = 0xff;
    mask[8] = 0xff;
    mask[7] = 0;
    mask[6] = 0;
    mask[5] = 0;
    mask[4] = 0;
    mask[3] = 0;
    mask[2] = 0;
    mask[1] = 0;
    mask[0] = 0;    
    
    bcm_l3_route_t_init(&l3rt);
    
    l3rt.l3a_flags = BCM_L3_IP6;
    l3rt.l3a_intf = l3egid;
    sal_memcpy(&l3rt.l3a_ip6_net,dip,16);
    sal_memcpy(&l3rt.l3a_ip6_mask,mask,16);

    rv = bcm_l3_route_add(unit, l3rt);

    if (rv != BCM_E_NONE) {
        printf("bcm_l3_route_add failed: %d\n", rv);
        return rv;
    }

    service->route_entry = l3rt;

    return BCM_E_NONE;
}

/*
 * Pre-selector to qualify those in port in the first pass.
 * One can also add the IPv6 qualifier in the pre-selector.
 */
int two_pass_egress_acl_in_port_selector_create(int unit) {
    bcm_pbmp_t pbm;
    bcm_pbmp_t pbm_mask;
    int core;
    int index;
    int rv;
    int presel_flags = BCM_FIELD_QUALIFY_PRESEL;
          
    /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        rv = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, two_pass_egress_acl_in_port_presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            return rv;
        }
        presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
    } else {
        rv = bcm_field_presel_create_id(unit, two_pass_egress_acl_in_port_presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create_id\n");
            return rv;
        }
    }
    
    /* Define the set of in/out ports */
    BCM_PBMP_CLEAR(pbm);
    for (index=0; index<MAX_NOF_CORES; index++) {
        BCM_PBMP_PORT_ADD(pbm, two_pass_egress_acl_service[index].in_port);
    }
      
    for(index=0; index<512; index++) { 
        BCM_PBMP_PORT_ADD(pbm_mask, index); 
    }

    rv = bcm_field_qualify_Stage(unit, two_pass_egress_acl_in_port_presel_id | presel_flags, bcmFieldStageIngress);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_Stage\n");
        return rv;
    }

    rv = bcm_field_qualify_InPorts(unit, two_pass_egress_acl_in_port_presel_id | presel_flags, pbm, pbm_mask);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_InPorts\n");
        return rv;
    }
	
    return BCM_E_NONE;
}

/*
 * 1st pass ingress PMF group:
 *     Set header profile to add DSP.
 *     For simpilicity it adds DSP for all packets. 
 *     Should be set only for those packets that will be forwarded to egress ACL recycle port.
 */
int two_pass_egress_acl_ipv6_add_dsp(int unit) {
    bcm_field_qset_t  qset;
    bcm_field_aset_t  aset;
    bcm_field_group_config_t grp;
    bcm_field_presel_set_t psset;
    bcm_field_entry_t ent;
    bcm_field_action_core_config_t config;
    bcm_gport_t gport;
    char *propval;
    int lb_ext = 1;
    int ONE_BYTE = 8;
    int rv = 0;

    BCM_FIELD_PRESEL_INIT(psset);
    BCM_FIELD_PRESEL_ADD(psset, two_pass_egress_acl_in_port_presel_id);      

    bcm_field_group_config_t_init(&grp);
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyDstIp6);

    grp.priority = BCM_FIELD_GROUP_PRIO_ANY;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.preselset = psset;

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &grp));
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionFabricHeaderSet);
    BCM_IF_ERROR_RETURN(bcm_field_group_action_set(unit, grp.group, aset));

    /* Match all packets */
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, grp.group, &ent));

    /* Indicate that do not generate new system header */
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, ent, bcmFieldActionFabricHeaderSet, bcmFieldFabricHeaderEthernetDSP, 0));
    BCM_IF_ERROR_RETURN(bcm_field_group_install(unit, grp.group));
	
    two_pass_egress_acl_group.first_pass_add_dsp_grp = grp.group;
    two_pass_egress_acl_group.first_pass_add_dsp_ent = ent;

    return BCM_E_NONE;
}

/*
 * Pre-selector to qualify those egress ACL recycle ports.
 * Used to match those packets received by ACL recycle port in the 2nd pass.
 */
int two_pass_egress_acl_rcy_port_selector_create(int unit) {
    bcm_pbmp_t pbm;
    bcm_pbmp_t pbm_mask;
    int core;
    int index;
    int rv;
    int presel_flags = BCM_FIELD_QUALIFY_PRESEL;
          
    /* Create a presel entity */
    if(soc_property_get(unit, spn_FIELD_PRESEL_MGMT_ADVANCED_MODE, FALSE)) {
        rv = bcm_field_presel_create_stage_id(unit, bcmFieldStageIngress, two_pass_egress_acl_rcy_port_presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create_stage_id\n");
            return rv;
        }
        presel_flags |= BCM_FIELD_QUALIFY_PRESEL_ADVANCED_MODE_STAGE_INGRESS;
    } else {
        rv = bcm_field_presel_create_id(unit, two_pass_egress_acl_rcy_port_presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error in bcm_field_presel_create_id\n");
            return rv;
        }
    }
    
    /* Define the set of in/out ports */
    BCM_PBMP_CLEAR(pbm);
    for (index=0; index<MAX_NOF_CORES; index++) {
        BCM_PBMP_PORT_ADD(pbm, two_pass_egress_acl_rcy_port[index]);
    }
      
    for(index=0; index<512; index++) { 
        BCM_PBMP_PORT_ADD(pbm_mask, index); 
    }

    rv = bcm_field_qualify_Stage(unit, two_pass_egress_acl_rcy_port_presel_id | presel_flags, bcmFieldStageIngress);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_Stage\n");
        return rv;
    }

    rv = bcm_field_qualify_InPorts(unit, two_pass_egress_acl_rcy_port_presel_id | presel_flags, pbm, pbm_mask);
    if (BCM_E_NONE != rv) {
        printf("Error in bcm_field_qualify_InPorts\n");
        return rv;
    }
	
    return BCM_E_NONE;
}

/*
 * 2nd pass ingress PMF group:
 *     Indicate that do not generate new system header
 *     Indicate that do not strip any bytes from packet header
 *     Redirect the packet to the front panel port according to destination-system-port extension
 */
int two_pass_egress_acl_second_pass_redirect(int unit) {
    bcm_field_qset_t  qset;
    bcm_field_aset_t  aset;
    bcm_field_group_config_t grp;
    bcm_field_presel_set_t psset;
    bcm_field_entry_t ent;
    bcm_field_action_core_config_t config;
    bcm_gport_t gport;
    char *propval;
    int lb_ext = 1;
    int ONE_BYTE = 8;
    int rv = 0;

    BCM_FIELD_PRESEL_INIT(psset);
    BCM_FIELD_PRESEL_ADD(psset, two_pass_egress_acl_rcy_port_presel_id);      

    bcm_field_group_config_t_init(&grp);
    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyDstIp6);

    grp.priority = BCM_FIELD_GROUP_PRIO_ANY;
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_PRESELSET;
    grp.preselset = psset;

    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &grp));
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionFabricHeaderSet);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionStartPacketStrip);
    BCM_IF_ERROR_RETURN(bcm_field_group_action_set(unit, grp.group, aset));

    /* Match all packets */
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, grp.group, &ent));

    /* Indicate that do not generate new system header */
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, ent, bcmFieldActionFabricHeaderSet, bcmFieldFabricHeaderStacking, 0));
    /* Indicate that do not strip system header */
    BCM_IF_ERROR_RETURN(bcm_field_action_add(unit, ent, bcmFieldActionStartPacketStrip, bcmFieldStartToConfigurableStrip, 0));
    BCM_IF_ERROR_RETURN(bcm_field_group_install(unit, grp.group));

    two_pass_egress_acl_group.second_pass_update_profile_grp = grp.group;
    two_pass_egress_acl_group.second_pass_update_profile_ent = ent;

    propval = soc_property_get_str(unit, "system_ftmh_load_balancing_ext_mode");    

    if ((!propval) || (sal_strcmp(propval, "DISABLED") == 0)) {
        lb_ext = 0;
    }

	bcm_field_data_qualifier_t dsp_qualifier;	
	bcm_field_data_qualifier_t_init(&dsp_qualifier); 
    dsp_qualifier.flags = BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    dsp_qualifier.offset = 74 + lb_ext * ONE_BYTE;
    dsp_qualifier.length = 11;
    dsp_qualifier.offset_base = bcmFieldDataOffsetBasePacketStart;
    BCM_IF_ERROR_RETURN(bcm_field_data_qualifier_create(unit, &dsp_qualifier));

    bcm_field_group_config_t_init(&grp);
    grp.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_PRESELSET; 
	grp.mode = bcmFieldGroupModeDirectExtraction;
    grp.preselset = psset;
	grp.priority = BCM_FIELD_GROUP_PRIO_ANY;

    BCM_FIELD_QSET_INIT(grp.qset);
    BCM_FIELD_QSET_ADD(grp.qset, bcmFieldQualifyStageIngress);
	BCM_IF_ERROR_RETURN(bcm_field_qset_data_qualifier_add(unit, grp.qset, dsp_qualifier.qual_id));
    BCM_IF_ERROR_RETURN(bcm_field_group_config_create(unit, &grp));

	BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);
	
    BCM_IF_ERROR_RETURN(bcm_field_group_action_set(unit, grp.group, aset));
    BCM_IF_ERROR_RETURN(bcm_field_entry_create(unit, grp.group, &ent));

    bcm_field_extraction_action_t extract;
    bcm_field_extraction_field_t ext_action[4];

    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&ext_action[0]);
    bcm_field_extraction_field_t_init(&ext_action[1]);
    bcm_field_extraction_field_t_init(&ext_action[2]);
    bcm_field_extraction_field_t_init(&ext_action[3]);

    extract.action = bcmFieldActionRedirect;
    extract.bias = 0;

    /* 2b'0, 1'b1, 5'b0, 11'bxxx*/
    ext_action[0].bits = 11;  
    ext_action[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext_action[0].lsb = 0;
    ext_action[0].qualifier = dsp_qualifier.qual_id;

    ext_action[1].bits = 5;  
    ext_action[1].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    ext_action[1].value = 0;

    ext_action[2].bits = 1;  
    ext_action[2].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    ext_action[2].value = 1;

    ext_action[3].bits = 2;  
    ext_action[3].flags = BCM_FIELD_EXTRACTION_FLAG_CONSTANT;
    ext_action[3].value = 0;
    
    BCM_IF_ERROR_RETURN(bcm_field_direct_extraction_action_add(unit, ent, extract, 4, &ext_action[0]));
	BCM_IF_ERROR_RETURN(bcm_field_group_install(unit, grp.group));

    two_pass_egress_acl_group.second_pass_update_dest_grp = grp.group;
    two_pass_egress_acl_group.second_pass_update_dest_ent = ent;
	
    return BCM_E_NONE;
}

int two_pass_egress_acl_dvapi(int unit) {
    int i;    
    uint32 port_var;

    for (i=0; i<two_pass_egress_acl_nof_service; i++) {
        BCM_IF_ERROR_RETURN(two_pass_egress_acl_add_system_port_mapping(unit, two_pass_egress_acl_service[i].out_port, &two_pass_egress_acl_service[i]));
        BCM_IF_ERROR_RETURN(two_pass_egress_acl_add_ipv6_routing(unit, two_pass_egress_acl_service[i].in_port, 
                                two_pass_egress_acl_service[i].out_port, 
                                two_pass_egress_acl_service[i].in_vlan, 
                                two_pass_egress_acl_service[i].out_vlan, 
                                &two_pass_egress_acl_service[i], i));    
    }    

    for (i=0; i<MAX_NOF_CORES; i++) {
        /* Enable egress ACL editing on ACL recycle port */
        BCM_IF_ERROR_RETURN(bcm_port_control_set(unit, two_pass_egress_acl_rcy_port[i], bcmPortControlEgressAclTwoPassEditing, 1));
        /* Set DA/SA of the dummy ETH header */
        port_var = (dummy_eth_sa[2] << 24) | (dummy_eth_sa[3] << 16) | (dummy_eth_sa[4] << 8) | dummy_eth_sa[5];
        BCM_IF_ERROR_RETURN(bcm_port_class_set(unit, two_pass_egress_acl_rcy_port[i], bcmPortClassProgramEditorEgressTrafficManagement, port_var)); 
        port_var = (dummy_eth_da[4] << 24) | (dummy_eth_da[5] << 16) | (dummy_eth_sa[0] << 8) | dummy_eth_sa[1];
        BCM_IF_ERROR_RETURN(bcm_port_class_set(unit, two_pass_egress_acl_rcy_port[i], bcmPortClassProgramEditorEgressPacketProcessing, port_var)); 
    }
    
    BCM_IF_ERROR_RETURN(two_pass_egress_acl_in_port_selector_create(0));
    BCM_IF_ERROR_RETURN(two_pass_egress_acl_ipv6_add_dsp(0));
    BCM_IF_ERROR_RETURN(two_pass_egress_acl_rcy_port_selector_create(unit));
    BCM_IF_ERROR_RETURN(two_pass_egress_acl_second_pass_redirect(unit));

    return BCM_E_NONE;
}

int two_pass_egress_acl_dvapi_clean_up(int unit) {
    int i;
    
    bcm_field_entry_destroy(unit, two_pass_egress_acl_group.first_pass_add_dsp_ent);
    bcm_field_group_destroy(unit, two_pass_egress_acl_group.first_pass_add_dsp_ent);
    bcm_field_entry_destroy(unit, two_pass_egress_acl_group.second_pass_update_dest_ent);
    bcm_field_group_destroy(unit, two_pass_egress_acl_group.second_pass_update_dest_grp);
    bcm_field_entry_destroy(unit, two_pass_egress_acl_group.second_pass_update_profile_ent);
    bcm_field_group_destroy(unit, two_pass_egress_acl_group.second_pass_update_profile_grp);
    bcm_field_presel_destroy(unit, two_pass_egress_acl_in_port_presel_id);
    bcm_field_presel_destroy(unit, two_pass_egress_acl_rcy_port_presel_id);

    for (i=0; i<two_pass_egress_acl_nof_service; i++) {
        bcm_l3_route_delete(unit, two_pass_egress_acl_service[i].route_entry);
        bcm_l3_egress_destroy(unit, two_pass_egress_acl_service[i].next_hop);
    }

    return BCM_E_NONE;    
}

