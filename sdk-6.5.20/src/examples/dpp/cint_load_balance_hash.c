/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File: cint_load_balance_hash.c
 *
 * Purpose: Example of setting LAG hash. 
 *
 * Usage:
 * 
 *
    To run this example:

    SOC: custom_feature_extra_hash_enable=1
 
    cint ../../../../src/examples/dpp/utility/cint_utils_l3.c 
    cint ../../../../src/examples/dpp/utility/cint_utils_vlan.c 
    cint ../../../../src/examples/dpp/cint_ip_route.c 
    cint ../../../../src/examples/dpp/cint_load_balance_hash.c
    c
    print lb_hash_pmf_config(0,0);
    print lb_hash_fer_config(0,0);
    print lb_hash_app_lag_config(0,13,15,3);
    print lb_hash_lag_background_config(0,14,200,2);
 */



/*
 * hash_type: 0-lag LB, 1-ECMP LB 
 */
int lb_hash_pmf_config(int unit, int hash_type)
{
    int rv;
    
    bcm_field_group_config_t grp0;
    bcm_field_entry_t entry;
    bcm_field_data_qualifier_t dq0;
    bcm_field_extraction_action_t extract;
    bcm_field_extraction_field_t ext_field[2];

    /* create data quelifiers */
    bcm_field_data_qualifier_t_init(&dq0); 
    dq0.flags  = BCM_FIELD_DATA_QUALIFIER_OFFSET_PREDEFINED | BCM_FIELD_DATA_QUALIFIER_LENGTH_BIT_RES | BCM_FIELD_DATA_QUALIFIER_OFFSET_BIT_RES;
    dq0.stage  = bcmFieldStageIngress;
    dq0.qualifier = bcmFieldQualifyContainer;
    dq0.offset = 32; 
    dq0.length = 20;
    rv = bcm_field_data_qualifier_create(unit, &dq0);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_data_qualifier_create error...\n");
        return -1;
    }

    /* Create grp0 with the PSET */
    bcm_field_group_config_t_init(&grp0);
    BCM_FIELD_QSET_INIT(grp0.qset);
    BCM_FIELD_QSET_ADD(grp0.qset, bcmFieldQualifyStageIngress);
    /*BCM_FIELD_QSET_ADD(grp0.qset, bcmFieldQualifyInPort);*/
    
    rv = bcm_field_qset_data_qualifier_add(unit, &grp0.qset, dq0.qual_id);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_qset_data_qualifier_add error...\n");
        return -1;
    }

    BCM_FIELD_ASET_INIT(grp0.aset);
    if (hash_type == 0) {
        BCM_FIELD_ASET_ADD(grp0.aset, bcmFieldActionTrunkHashKeySet);
    } else if (hash_type == 1) {
        BCM_FIELD_ASET_ADD(grp0.aset, bcmFieldActionMultipathHashAdditionalInfo);
    }
    BCM_FIELD_ASET_ADD(grp0.aset, bcmFieldActionClassDestSet);

    grp0.group = -1;
    grp0.priority = 0;
    grp0.flags = BCM_FIELD_GROUP_CREATE_WITH_MODE | BCM_FIELD_GROUP_CREATE_WITH_ASET;
    grp0.mode = bcmFieldGroupModeDirectExtraction;
    rv = bcm_field_group_config_create(unit, &grp0);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_config_create error...\n");
        return -1;
    }

    /* Create an entry and add action */
    rv = bcm_field_entry_create(unit, grp0.group, &entry);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_entry_create error...\n");
        return -1;
    }

    /*
    rv = bcm_field_qualify_InPort(unit, entry, 13, 0xffffffff);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_qualify_InPort error...\n");
        return -1;
    }*/
    
    bcm_field_extraction_action_t_init(&extract);
    bcm_field_extraction_field_t_init(&ext_field);

    if (hash_type == 0) {
        extract.action = bcmFieldActionTrunkHashKeySet;
    } else if (hash_type == 1) {
        extract.action = bcmFieldActionMultipathHashAdditionalInfo;
    }    
    extract.bias = 0;
    ext_field[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext_field[0].bits  = 20;
    ext_field[0].lsb = 0;
    ext_field[0].qualifier = dq0.qual_id; 

    /* set PEM data to LB-Data */
    rv = bcm_field_direct_extraction_action_add(unit,entry,extract,1,&ext_field);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_direct_extraction_action_add error...\n");
        return -1;
    }

    /* set PEM data to UDH */
    extract.action = bcmFieldActionClassDestSet;
    extract.bias = 0;
    ext_field[0].flags = BCM_FIELD_EXTRACTION_FLAG_DATA_FIELD;
    ext_field[0].bits  = 16;
    ext_field[0].lsb = 0;
    ext_field[0].qualifier = dq0.qual_id;
    rv = bcm_field_direct_extraction_action_add(unit,entry,extract,1,&ext_field);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_direct_extraction_action_add error...\n");
        return -1;
    }

    rv =  bcm_field_group_install(unit, grp0.group);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_install error...\n");
        return -1;
    }
    printf("Config PMF for LB hash sucessfully!\n");

    return rv;
}


int lb_hash_fer_config(int unit, int sel)
{
    int rv;
    bcm_switch_control_t type;
    int arg;

    if (sel == 0) {
        type = bcmSwitchTrunkHashConfig;
        arg = BCM_HASH_CONFIG_FP_DATA;
    } else if (sel == 1) {
        type = bcmSwitchECMPHashConfig;
        arg = BCM_HASH_CONFIG_NONE;
    }

    rv = bcm_switch_control_set(unit, type, arg);
    if (rv != BCM_E_NONE) {
        printf("bcm_petra_switch_control_set error\n");
    }
    return rv;
}

 
/* 
 * Reference from ../../../../src/examples/dpp/cint_lag_hashing.c
 * Create three fwd service: L2/mpls/ipv4 to forward to LAG port.
 *
 * number of out_port: out_port, out_port++, out_port++ ....
 */ 
int lb_hash_app_lag_config(int unit, int in_port, int out_port, int nof_ports)
{    
    int CINT_NO_FLAGS = 0;
    int rc, i;
    int vrf = 5;
    int in_vlan = 17;
    int out_vlan = 18;
    int ingress_intfs[2]; /* in-RIF and out-RIF */
    bcm_if_t egress_intf; /* FEC */
    int encap_id;
    
    bcm_trunk_t trunk_id; /* trunk */
    bcm_trunk_member_t member;
    int trunk_gport;
    
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x01, 0x23};  /* my-MAC */
    bcm_mac_t next_mac_address  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* outgoing DA */
    
    int host = 0x0a00ff00; /* 10.0.255.0 */
    bcm_l3_host_t l3host;
    
    int in_label = 44;
    int eg_label = 66;
    bcm_mpls_tunnel_switch_t mpls_tunnel_info;
    
    bcm_mac_t dest_mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x01, 0x24};  /* dest-MAC */
    bcm_l2_addr_t l2addr;
    
    /* create in-RIF */
    rc = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);  
    if (rc != BCM_E_NONE) {
      printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
    }
    rc = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
    if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
      printf("fail add port(0x%08x) to vlan(%d)\n", in_port, in_vlan);
      return rc;
    }
    
    create_l3_intf_s intf;
    intf.vsi = in_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;
    intf.vrf_valid = 1;
    intf.vrf = vrf;
    
    rc = l3__intf_rif__create(unit, &intf);
    ingress_intfs[0] = intf.rif;        
    if (rc != BCM_E_NONE) {
      printf("Error, l3__intf_rif__create\n");
    }
    
    /* create out-RIF */
    rc = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);  
    if (rc != BCM_E_NONE) {
      printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
    }
    rc = bcm_vlan_gport_add(unit, out_vlan, out_port, 0);
    if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
      printf("fail add port(0x%08x) to vlan(%d)\n", out_port, out_vlan);
      return rc;
    }
    
    intf.vsi = out_vlan;
    
    rc = l3__intf_rif__create(unit, &intf);
    ingress_intfs[1] = intf.rif;        
    if (rc != BCM_E_NONE) {
      printf("Error, l3__intf_rif__create\n");
    }
    
    /* create trunk */
    rc = bcm_trunk_create(unit, CINT_NO_FLAGS, &trunk_id);
    if (rc != BCM_E_NONE) {
      printf ("bcm_trunk_create failed: %d \n", rc);
      return rc;
    }
        
    /* add ports to trunk */
    bcm_trunk_member_t_init(&member);
    out_port--;
    for (i = 0; i < nof_ports; i++) {
    
        out_port++;
        BCM_GPORT_LOCAL_SET(member.gport, out_port); /* phy port to local port */
        
        rc = bcm_trunk_member_add(unit, trunk_id, &member);
        if (rc != BCM_E_NONE) {
          printf ("bcm_trunk_member_add with port %d failed: %d \n", out_port, rc);
          return rc;
        }
    }
    
    /* create FEC and send to LAG (instead of out-port) */
    BCM_GPORT_TRUNK_SET(trunk_gport, trunk_id); /* create a trunk gport and give this gport to create_l3_egress() instead of the dest-port */
      create_l3_egress_s l3eg2;
      sal_memcpy(l3eg2.next_hop_mac_addr, next_mac_address , 6);
      l3eg2.out_tunnel_or_rif = ingress_intfs[1];
      l3eg2.out_gport = trunk_gport;
      l3eg2.vlan = out_vlan;
      l3eg2.fec_id = egress_intf;
      l3eg2.arp_encap_id = encap_id;
    
      rc = l3__egress__create(unit,&l3eg2);
      if (rc != BCM_E_NONE) {
          printf("Error, l3__egress__create\n");
          return rc;
      }
    
      egress_intf = l3eg2.fec_id;
      encap_id = l3eg2.arp_encap_id;
    
    /* add host entry and point to the FEC pointing at LAG */
    bcm_l3_host_t_init(&l3host);
    l3host.l3a_ip_addr = host;
    l3host.l3a_vrf = vrf;
    l3host.l3a_intf = egress_intf; /* FEC */
    
    rc = bcm_l3_host_add(unit, &l3host);
    if (rc != BCM_E_NONE) {
      printf ("bcm_l3_host_add failed: %x \n", rc);
      return rc;
    }
    
    /* add ipv6 route */
    bcm_ip6_t ip6_mask = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
    bcm_ip6_t ip6_addr = {0x20,0x18,0x11,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xaa,0xbb};
    bcm_l3_route_t l3rt;
    
    bcm_l3_route_t_init(&l3rt);
    
    l3rt.l3a_flags = BCM_L3_IP6;
    l3rt.l3a_intf = egress_intf;
    l3rt.l3a_vrf = vrf;
    sal_memcpy(&(l3rt.l3a_ip6_net), (ip6_addr), 16);
    sal_memcpy(&(l3rt.l3a_ip6_mask), (ip6_mask), 16);
    l3rt.l3a_modid = 0;
    l3rt.l3a_port_tgid = 0;
    
    rc = bcm_l3_route_add(unit, &l3rt);
    if (rc != BCM_E_NONE) {
      printf ("bcm_l3_route_add failed for ipv6: %d \n", rc);
    }
    
    /* add switch entry to swap labels and map to LAG (FEC pointing at LAG) */
    bcm_mpls_tunnel_switch_t_init(&mpls_tunnel_info);
    mpls_tunnel_info.action = BCM_MPLS_SWITCH_ACTION_SWAP;
    mpls_tunnel_info.flags = BCM_MPLS_SWITCH_TTL_DECREMENT; /* TTL decrement has to be present */
    mpls_tunnel_info.flags |= BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;
    mpls_tunnel_info.label = in_label; /* incomming label */
    mpls_tunnel_info.egress_label.label = eg_label; /* outgoing (egress) label */
    mpls_tunnel_info.egress_if = egress_intf; /* FEC */
    
    rc = bcm_mpls_tunnel_switch_create(unit, &mpls_tunnel_info);
    if (rc != BCM_E_NONE) {
      printf ("bcm_mpls_tunnel_switch_create failed: %x \n", rc);
      return rc;
    }
    
    /* add Mac table entry and point to the LAG, for bridging */
    bcm_l2_addr_t_init(&l2addr, dest_mac_address, in_vlan);
    l2addr.port = trunk_gport; /* dest is LAG */
    
    bcm_mac_t bc_mac_address  = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};  /* dest-MAC */
    rc = bcm_l2_addr_add(unit, &l2addr);
    if (rc != BCM_E_NONE) {
        printf("bcm_l2_addr_add failed: %x \n", rc);
        return rc;
    }
    
    sal_memcpy(l2addr.mac, bc_mac_address, 6);
    rc = bcm_l2_addr_add(unit, &l2addr);
    if (rc != BCM_E_NONE) {
        printf("bcm_l2_addr_add 2 failed: %x \n", rc);
        return rc;
    }
    
    return 0;
}


int lb_hash_app_ecmp_config(int unit, int in_port, int out_port, int ecmp_size)
{
    int CINT_NO_FLAGS = 0;
    int rc, i;
    int vrf = 5;
    int in_vlan = 17;
    int out_vlan = 17;
    bcm_pbmp_t pbmp, ubmp;
    
    if (ecmp_size > 1000) {
        printf("Error - Max ECMP size allowed is 1000.\n");
        return BCM_E_PARAM;
    }
    
    int ingress_intfs[2]; /* in-RIF and out-RIF */  
    int encap_ids[ecmp_size];
    bcm_if_t multipath_id; /* ECMP */
    
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x01, 0x23};  /* my-MAC */
    bcm_mac_t next_mac_address  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* outgoing DA */
    
    int host = 0x0a00ff00; /* 10.0.255.0 */
    bcm_l3_host_t l3host;
    
    int in_label = 44;
    int eg_label = 66;
    bcm_mpls_tunnel_switch_t mpls_tunnel_info;
    
    BCM_PBMP_CLEAR(ubmp);
    
    /* create in-RIF */
    rc = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);
    if (rc != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
    }
    rc = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
    if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", in_port, in_vlan);
        return rc;
    }
    
    create_l3_intf_s intf;
    intf.vsi = in_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;
    intf.vrf_valid = 1;
    intf.vrf = vrf;
    
    rc = l3__intf_rif__create(unit, &intf);
    ingress_intfs[0] = intf.rif;
    if (rc != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }
    
    /* create out-RIF */
    rc = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);  
    if (rc != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
    }
    rc = bcm_vlan_gport_add(unit, out_vlan, out_port, 0);
    if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", out_port, out_vlan);
        return rc;
    }
    
    intf.vsi = out_vlan;    
    rc = l3__intf_rif__create(unit, &intf);
    ingress_intfs[1] = intf.rif;        
    if (rc != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }
    
    /* create 10 FEC entries (all with the same out-RIF)
       each FEC will point to a different out-port.
       also set vlan-port membership. each out-port will have a different vlan */
    out_port--;
    for (i = 0; i < ecmp_size; i++) {    
        out_port++;
        out_vlan++;    
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_PORT_ADD(pbmp, out_port);
    
        /* create FEC */
        create_l3_egress_s l3eg;
        sal_memcpy(l3eg.next_hop_mac_addr, next_mac_address , 6);
        l3eg.allocation_flags = CINT_NO_FLAGS;
        l3eg.l3_flags = CINT_NO_FLAGS;
        l3eg.out_tunnel_or_rif = ingress_intfs[1];
        l3eg.out_gport = out_port;
        l3eg.vlan = out_vlan;
        l3eg.fec_id = cint_ecmp_hashing_data.egress_intfs[i];
        l3eg.arp_encap_id = encap_ids[i];
    
        rc = l3__egress__create(unit,&l3eg);
        if (rc != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rc;
        }
    
        cint_ecmp_hashing_data.egress_intfs[i] = l3eg.fec_id;
        encap_ids[i] = l3eg.arp_encap_id;
    
        /* set vlan port membership for out-vlan and out-port */
        rc = bcm_vlan_port_add(unit, out_vlan, pbmp, ubmp);
        if (rc != BCM_E_NONE) {
            printf ("bcm_vlan_port_add no. %d failed: %d \n", i, rc);
            return rc;
        }  
    }
    
    if (cint_ecmp_hashing_cfg.ecmp_api_is_ecmp) {
        bcm_l3_egress_ecmp_t_init(&cint_ecmp_hashing_data.ecmp);
        cint_ecmp_hashing_data.ecmp.max_paths = ecmp_size;
    
        /* create an ECMP, containing the FEC entries */
        rc = bcm_l3_egress_ecmp_create(unit, &cint_ecmp_hashing_data.ecmp, ecmp_size, cint_ecmp_hashing_data.egress_intfs);
        if (rc != BCM_E_NONE) {
            printf ("bcm_l3_egress_ecmp_create failed: %d \n", rc);
            return rc;
        }
    } else {
        rc = bcm_l3_egress_multipath_create(unit, CINT_NO_FLAGS, ecmp_size, cint_ecmp_hashing_data.egress_intfs, &multipath_id);
        if (rc != BCM_E_NONE) {
            printf ("bcm_l3_egress_multipath_create failed: %d \n", rc);
            return rc;
        }
    }
    
    /* add host entry and point to the ECMP */
    bcm_l3_host_t_init(&l3host);
    l3host.l3a_ip_addr = host;
    l3host.l3a_vrf = vrf;
    if (cint_ecmp_hashing_cfg.ecmp_api_is_ecmp) {
        l3host.l3a_intf = cint_ecmp_hashing_data.ecmp.ecmp_intf;
    } else {
        l3host.l3a_intf = multipath_id; /* ECMP */
    }
    
    rc = bcm_l3_host_add(unit, &l3host);
    if (rc != BCM_E_NONE) {
        printf ("bcm_l3_host_add failed: %x \n", rc);
        return rc;
    }
    
    /* add switch entry to swap labels and map to ECMP */
    bcm_mpls_tunnel_switch_t_init(&mpls_tunnel_info);
    mpls_tunnel_info.action = BCM_MPLS_SWITCH_ACTION_SWAP;
    mpls_tunnel_info.flags = BCM_MPLS_SWITCH_TTL_DECREMENT; /* TTL decrement has to be present */
    mpls_tunnel_info.flags |= BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;
    mpls_tunnel_info.label = in_label; /* incomming label */
    mpls_tunnel_info.egress_label.label = eg_label; /* outgoing (egress) label */
    if (cint_ecmp_hashing_cfg.ecmp_api_is_ecmp) {
        mpls_tunnel_info.egress_if = cint_ecmp_hashing_data.ecmp.ecmp_intf;
    } else {
        mpls_tunnel_info.egress_if = multipath_id; /* ECMP */
    }
    
    rc = bcm_mpls_tunnel_switch_create(unit, &mpls_tunnel_info);
    if (rc != BCM_E_NONE) {
        printf ("bcm_mpls_tunnel_switch_create failed: %x \n", rc);
        return rc;
    }
    
    return 0;
}


int lb_hash_lag_background_config(int unit, int in_port, int out_port, int nof_ports)
{
    int CINT_NO_FLAGS = 0;
    int rc, i;

    bcm_trunk_t trunk_id; /* trunk */
    bcm_trunk_member_t member;
    int trunk_gport;

    bcm_mac_t dest_mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x01, 0x25};  /* dest-MAC */
    bcm_l2_addr_t l2addr;

    /* create trunk */
    rc = bcm_trunk_create(unit, CINT_NO_FLAGS, &trunk_id);
    if (rc != BCM_E_NONE) {
      printf ("bcm_trunk_create failed: %d \n", rc);
      return rc;
    }
    
    /* add ports to trunk */
    bcm_trunk_member_t_init(&member);
    out_port--;
    for (i = 0; i < nof_ports; i++) {
        out_port++;
        BCM_GPORT_LOCAL_SET(member.gport, out_port); /* phy port to local port */
        
        rc = bcm_trunk_member_add(unit, trunk_id, &member);
        if (rc != BCM_E_NONE) {
          printf ("bcm_trunk_member_add with port %d failed: %d \n", out_port, rc);
          return rc;
        }
    }

    BCM_GPORT_TRUNK_SET(trunk_gport, trunk_id);

    bcm_vlan_port_t vlan_port_1;
    bcm_vlan_port_t vlan_port_2;
    bcm_vlan_port_t_init(&vlan_port_1);
    bcm_vlan_port_t_init(&vlan_port_2);
    vlan_port_1.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port_2.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port_1.port = in_port;
    vlan_port_2.port = trunk_gport;

    rc =  bcm_vlan_port_create(unit, &vlan_port_1);
    if (rc != BCM_E_NONE) {
        printf ("bcm_vlan_port_create for vlan_port_1 failed\n");
        return rc;
    }
    rc = bcm_vlan_port_create(unit, &vlan_port_2);
    if (rc != BCM_E_NONE) {
        printf ("bcm_vlan_port_create for vlan_port_2 failed\n");
        return rc;
    }

    bcm_vswitch_cross_connect_t gports;
    gports.port1= vlan_port_1.vlan_port_id;
    gports.port2= vlan_port_2.vlan_port_id;
    gports.encap1=BCM_FORWARD_ENCAP_ID_INVALID;
    gports.encap2=BCM_FORWARD_ENCAP_ID_INVALID;
    rc = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rc != BCM_E_NONE) {
        printf ("bcm_vswitch_cross_connect_add failed\n");
        return rc;
    }

    return rc;
}


int lb_hash_ecmp_background_config(int unit, int in_port, int out_port, int ecmp_size)
{
    int CINT_NO_FLAGS = 0;
    int rc, i;
    int vrf = 5;
    int in_vlan = 17;
    int out_vlan = 17;
    bcm_pbmp_t pbmp, ubmp;
    
    if (ecmp_size > 1000) {
        printf("Error - Max ECMP size allowed is 1000.\n");
        return BCM_E_PARAM;
    }
    
    int ingress_intfs[2]; /* in-RIF and out-RIF */  
    int encap_ids[ecmp_size];
    bcm_if_t multipath_id; /* ECMP */
    
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x01, 0x23};  /* my-MAC */
    bcm_mac_t next_mac_address  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* outgoing DA */
    
    int host = 0x0a01ff00; /* 10.1.255.0 */
    bcm_l3_host_t l3host;
    
    int in_label = 45;
    int eg_label = 66;
    bcm_mpls_tunnel_switch_t mpls_tunnel_info;
    
    BCM_PBMP_CLEAR(ubmp);
    
    /* create in-RIF */
    rc = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);
    if (rc != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
    }
    rc = bcm_vlan_gport_add(unit, in_vlan, in_port, 0);
    if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", in_port, in_vlan);
        return rc;
    }
    
    create_l3_intf_s intf;
    intf.vsi = in_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;
    intf.vrf_valid = 1;
    intf.vrf = vrf;
    
    rc = l3__intf_rif__create(unit, &intf);
    ingress_intfs[0] = intf.rif;
    if (rc != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }
    
    /* create out-RIF */
    rc = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);  
    if (rc != BCM_E_NONE) {
        printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
    }
    rc = bcm_vlan_gport_add(unit, out_vlan, out_port, 0);
    if (rc != BCM_E_NONE && rc != BCM_E_EXISTS) {
        printf("fail add port(0x%08x) to vlan(%d)\n", out_port, out_vlan);
        return rc;
    }
    
    intf.vsi = out_vlan;    
    rc = l3__intf_rif__create(unit, &intf);
    ingress_intfs[1] = intf.rif;        
    if (rc != BCM_E_NONE) {
        printf("Error, l3__intf_rif__create\n");
    }
    
    /* create 10 FEC entries (all with the same out-RIF)
       each FEC will point to a different out-port.
       also set vlan-port membership. each out-port will have a different vlan */
    out_port--;
    for (i = 0; i < ecmp_size; i++) {    
        out_port++;
        out_vlan++;    
        BCM_PBMP_CLEAR(pbmp);
        BCM_PBMP_PORT_ADD(pbmp, out_port);
    
        /* create FEC */
        create_l3_egress_s l3eg;
        sal_memcpy(l3eg.next_hop_mac_addr, next_mac_address , 6);
        l3eg.allocation_flags = CINT_NO_FLAGS;
        l3eg.l3_flags = CINT_NO_FLAGS;
        l3eg.out_tunnel_or_rif = ingress_intfs[1];
        l3eg.out_gport = out_port;
        l3eg.vlan = out_vlan;
        l3eg.fec_id = cint_ecmp_hashing_data.egress_intfs[i];
        l3eg.arp_encap_id = encap_ids[i];
    
        rc = l3__egress__create(unit,&l3eg);
        if (rc != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rc;
        }
    
        cint_ecmp_hashing_data.egress_intfs[i] = l3eg.fec_id;
        encap_ids[i] = l3eg.arp_encap_id;
    
        /* set vlan port membership for out-vlan and out-port */
        rc = bcm_vlan_port_add(unit, out_vlan, pbmp, ubmp);
        if (rc != BCM_E_NONE) {
            printf ("bcm_vlan_port_add no. %d failed: %d \n", i, rc);
            return rc;
        }  
    }
    
    if (cint_ecmp_hashing_cfg.ecmp_api_is_ecmp) {
        bcm_l3_egress_ecmp_t_init(&cint_ecmp_hashing_data.ecmp);
        cint_ecmp_hashing_data.ecmp.max_paths = ecmp_size;
    
        /* create an ECMP, containing the FEC entries */
        rc = bcm_l3_egress_ecmp_create(unit, &cint_ecmp_hashing_data.ecmp, ecmp_size, cint_ecmp_hashing_data.egress_intfs);
        if (rc != BCM_E_NONE) {
            printf ("bcm_l3_egress_ecmp_create failed: %d \n", rc);
            return rc;
        }
    } else {
        rc = bcm_l3_egress_multipath_create(unit, CINT_NO_FLAGS, ecmp_size, cint_ecmp_hashing_data.egress_intfs, &multipath_id);
        if (rc != BCM_E_NONE) {
            printf ("bcm_l3_egress_multipath_create failed: %d \n", rc);
            return rc;
        }
    }
    
    /* add host entry and point to the ECMP */
    bcm_l3_host_t_init(&l3host);
    l3host.l3a_ip_addr = host;
    l3host.l3a_vrf = vrf;
    if (cint_ecmp_hashing_cfg.ecmp_api_is_ecmp) {
        l3host.l3a_intf = cint_ecmp_hashing_data.ecmp.ecmp_intf;
    } else {
        l3host.l3a_intf = multipath_id; /* ECMP */
    }
    
    rc = bcm_l3_host_add(unit, &l3host);
    if (rc != BCM_E_NONE) {
        printf ("bcm_l3_host_add failed: %x \n", rc);
        return rc;
    }
    
    /* add switch entry to swap labels and map to ECMP */
    bcm_mpls_tunnel_switch_t_init(&mpls_tunnel_info);
    mpls_tunnel_info.action = BCM_MPLS_SWITCH_ACTION_SWAP;
    mpls_tunnel_info.flags = BCM_MPLS_SWITCH_TTL_DECREMENT; /* TTL decrement has to be present */
    mpls_tunnel_info.flags |= BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;
    mpls_tunnel_info.label = in_label; /* incomming label */
    mpls_tunnel_info.egress_label.label = eg_label; /* outgoing (egress) label */
    if (cint_ecmp_hashing_cfg.ecmp_api_is_ecmp) {
        mpls_tunnel_info.egress_if = cint_ecmp_hashing_data.ecmp.ecmp_intf;
    } else {
        mpls_tunnel_info.egress_if = multipath_id; /* ECMP */
    }
    
    rc = bcm_mpls_tunnel_switch_create(unit, &mpls_tunnel_info);
    if (rc != BCM_E_NONE) {
        printf ("bcm_mpls_tunnel_switch_create failed: %x \n", rc);
        return rc;
    }
    
    return 0;
}






