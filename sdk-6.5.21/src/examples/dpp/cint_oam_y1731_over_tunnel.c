/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~OAM test~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File: cint_oam_y13731.c
 * Purpose: Example of using Y.1731 OAM over MPLS-TP/PWE. 
 *
 * Usage:
 * 
 MPLS comes in two version, one in which the in/out LIFs are presumed to be symetric:
 cint utility/cint_utils_l3.c
 cint cint_ip_route.c
 cint ../sand/cint_sand_oam_y1731_over_tunnel.c
 cint cint_oam_y1731_over_tunnel.c
 cint
 print oam_run_with_defaults_mpls_tp(unit,13,14,15,0);
 
 "New" MPLS calling sequence, using assymetric LIFs (for LMM counting purposes). MPLS LIFs created with mpls_lsr_tunnel_example()
 cint cint_qos.c
 cint cint_multi_device_utils.c
 cint utility/cint_utils_l3.c
 cint cint_mpls_lsr.c
 cint ../sand/cint_sand_oam_y1731_over_tunnel.c
 cint cint_oam_y1731_over_tunnel.c
 cint cint_system_vswitch_encoding.c
 cint
 print oam_run_with_defaults_mpls_tp(unit,13,14,15,1);
 
 cint ../sand/utility/cint_sand_utils_global.c
 cint ../sand/utility/cint_sand_utils_mpls.c
 cint cint_port_tpid.c
 cint cint_qos.c cint_vswitch_vpls.c
 cint ../sand/cint_sand_oam_y1731_over_tunnel.c
 cint cint_oam_y1731_over_tunnel.c
 cint utility/cint_utils_l3.c
 cint cint_mpls_lsr.c
 cint cint_advanced_vlan_translation_mode.c
 cint
 print oam_run_with_defaults_pwe(unit,13,14,15);
 
 cint ../sand/utility/cint_sand_utils_global.c
 cint ../sand/utility/cint_sand_utils_mpls.c
 cint cint_port_tpid.c
 cint cint_qos.c cint_vswitch_vpls.c
 cint cint_oam_y1731_over_tunnel.c
 cint utility/cint_utils_l3.c
 cint cint_mpls_lsr.c
 cint cint_advanced_vlan_translation_mode.c
 cint
 print oam_o_gach_o_gal_o_pwe_o_lsp_example(unit,13,14);

 * 
 * This cint uses cint_vswitch_metro_mp_single_vlan.c to build the following vswitch:
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *  |                                                                       |
 *  |                   o  \vlan        vlan         o                      |
 *  |                    \  \<4>        <10>/  /                            |
 *  |                  /\ \  \   -----------    /  /                        |
 *  |                   \  \ \/ /  \   /\   \  /  / /\                      |
 *  |             \  \  |    \  /     |\/ /  /                              |
 *  |                     \  \ |     \/      |  /  /                        |
 *  |                    p3=15\|     /\      | /  /                         |
 *  |                          |    /  \     |/p1 = 13                      |             
 *  |                         /|   \/   \    |                              |
 *  |                        /  \  VSwitch  /                               | 
 *  |                   /\  /p2=14----------                                |
 *  |         vlan <5>/  /  /                                               |
 *  |                  /  /  /                                              |
 *  |                 /  /  /                                               |
 *  |                   /  \/                                               |
 *  |                  o                                                    |
 *  |                                                                       | 
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * The following MAs:
 *         1) MA with id 1 & short name format:    1-const; 3-short format; 2-length; all the rest - MA name
 *        short_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 3, 2, 0xcd, 0xab}
 * 
 * The following MEPs:
 *         1) MEP with id 4096: accellerated, mdlevel 7,
 *      3) RMEP with id 0
 * 
 * In addition, get & delete APIs are used for testing.
 * 
 * Event callback registration example is provided
 * 
 * comments:
 * 1) In order to prevent from OAMP send packets do: BCM.0> m OAMP_MODE_REGISTER TRANSMIT_ENABLE=0
 */
 

/* GLOBAL VAR*/

    int unknown_label = 13; /*by default GAL label */
    int y1711_lsp_pwe_flag = 0;
    int y1711_cnt_1731_pkt_flag = 0;
    int two_1711_mep_flag = 0;
    oam__ep_info_s ep2; /* store endpoint information*/
    oam__ep_info_s ep_y1731; /* store endpoint information*/
    oam__ep_info_s ep_y1711; /* store second 1711 endpoint information*/
    int lb_tst_gtf_id=0;
    int mpls_pwe_set_flag=0;

    int lm_counter_base_id_1_1711_ep;
    int count_pwe = 0;
    /*
    * Creating vswitch and adding mac table entries
    */

    /* Globals - MAC addresses , ports & gports*/
    bcm_multicast_t mc_group = 1234;
    bcm_gport_t gport1, gport2, gport3; /* these are the ports created by the vswitch*/
  	bcm_oam_endpoint_info_t mep_acc_info_y1731;
    bcm_oam_endpoint_info_t mep_acc_info_y1711;
  	bcm_oam_endpoint_info_t rmep_info_y1731;
    bcm_oam_endpoint_info_t rmep_info_y1711;
    bcm_oam_sd_sf_detection_t  sdsf;
    int mpls_label = 100;

    int sd_events_set_count = 0;
    int sf_events_set_count = 0;
    int sd_events_clear_count = 0;
    int sf_events_clear_count = 0;


/*
 * Adds PWE endpoint with PWE label 2000
 * This is used only when add_extra_pwe_tunnel is true.
 */
int add_extra_pwe_ep = 0;

/*
 * Adds default PWE endpoint with PWE label 2000
 * This is used only when add_extra_pwe_tunnel is true.
 */
int add_extra_default_pwe_ep = 0;


/* 
* Add GAL label in order to avoit trap unknown_label
* Point to egress-object: egress_intf, returned by create_l3_egress
*/
int
mpls_add_gal_entry(int unit)
{
    int rv;
	int mpls_termination_label_index_enable;
    bcm_mpls_tunnel_switch_t entry;
    
    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_NOP;
    /* TTL decrement has to be present */
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    /* Uniform: inherit TTL and EXP, 
     * in general valid options: 
     * both present (uniform) or none of them (Pipe)
     */
    entry.flags |= BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;
    
    /* by default GAL label can be y1711 label*/
    entry.label = unknown_label;
    
     
	/* read mpls index soc property */
    mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
	if (mpls_termination_label_index_enable) {
		BCM_MPLS_INDEXED_LABEL_SET(&entry.label,is_y1711 ? 14 : 13,2);
	}
    
    /* egress attribures*/   
    entry.egress_if = 0;    
    
    rv = bcm_mpls_tunnel_switch_create(unit,&entry);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
    return rv;
}


/* creating l3 interface */


int l3_interface_init(int unit, int in_sysport, int out_sysport, int * _next_hop_mac_encap_id, int * encap_id_l2, int is_mpls){
    int rv;
    int ing_intf_in; 
    int ing_intf_out; 
    int fec[2] = {0x0,0x0};
    int flags1;
    int in_vlan = 1; 
    int out_vlan = 100;
    int vrf = 0;
    int host;
    int encap_id[2]={0};
    int route;
    int mask;
    int l3_eg_int;
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    bcm_mac_t next_hop_mac2  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */

	mpls__egress_tunnel_utils_s mpls_tunnel_properties;
	if (device_type == device_type_jericho || device_type == device_type_jericho_b  ) {
		
		encap_id[0] = 0x2000;
		encap_id[1] = 0x4000;
	}

    /*** create ingress router interface ***/
	rv = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);  
	if (rv != BCM_E_NONE) {
		printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
	}
	rv = bcm_vlan_gport_add(unit, in_vlan, in_sysport, 0);
	if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
		printf("fail add port(0x%08x) to vlan(%d)\n", in_sysport, in_vlan);
	  return rv;
	}

    create_l3_intf_s intf;
    intf.vsi = in_vlan;
    intf.my_global_mac = mac_address;
    intf.my_lsb_mac = mac_address;
    intf.vrf_valid = 1;
    intf.vrf = vrf;
    
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_in = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }

    /*** create egress router interface ***/
	rv = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);  
	if (rv != BCM_E_NONE) {
		printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
	}
	rv = bcm_vlan_gport_add(unit, out_vlan, out_sysport, 0);
	if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
		printf("fail add port(0x%08x) to vlan(%d)\n", out_sysport, out_vlan);
	  return rv;
	}

    intf.vsi = out_vlan;
    
    rv = l3__intf_rif__create(unit, &intf);
    ing_intf_out = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }

    /*** create egress object 1 ***/
    if (is_mpls) {
        /*** Create tunnel ***/
		mpls_tunnel_properties.label_in = 200;
		mpls_tunnel_properties.label_out = 0;
		mpls_tunnel_properties.next_pointer_intf = ing_intf_out;
	
		printf("Trying to create tunnel initiator\n");
		rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
		if (rv != BCM_E_NONE) {
           printf("Error, in mpls__create_tunnel_initiator__set\n");
           return rv;
        }

        l3_eg_int = mpls_tunnel_properties.tunnel_id;
    }
    else {
        l3_eg_int = ing_intf_out;
    }

    /*** Create egress object1 ***/
    flags1 = 0;
    create_l3_egress_s l3eg;
    sal_memcpy(l3eg.next_hop_mac_addr, next_hop_mac , 6);
    l3eg.out_tunnel_or_rif = l3_eg_int;
    l3eg.out_gport = out_sysport;
    l3eg.vlan = out_vlan;
    l3eg.fec_id = fec[0];
    l3eg.arp_encap_id = encap_id[0];

    rv = l3__egress__create(unit,&l3eg);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    fec[0] = l3eg.fec_id;
    encap_id[0] = l3eg.arp_encap_id;
    if(verbose >= 1) {
        printf("created FEC-id =0x%08x, in unit %d \n", fec[0], unit);
        printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[0], unit);
    }

    /* Add another label for swapping.*/
    bcm_mpls_tunnel_switch_t entry;
    bcm_mpls_tunnel_switch_t_init(&entry);
    entry.action = BCM_MPLS_SWITCH_ACTION_SWAP;
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT | BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;
    /* incomming label */
    entry.label = 120; /*mpls labels 120 will get swapped (100 will get terminated)*/
    entry.egress_label.label =mpls_label +2;
    entry.egress_if = fec[0];
    rv = bcm_mpls_tunnel_switch_create(unit,&entry);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }

    /*** add host point to FEC ***/
    host = 0x7fffff03;
    rv = add_host(unit, 0x7fffff03, vrf, fec[0]); 
    if (rv != BCM_E_NONE) {
        printf("Error, add_host, in unit %d \n", unit);
        return rv;
    }

    /*** create egress object 2***/
    /* We're allocating a lif. out_sysport unit should be first, and it's already first */    
    flags1 = 0;
    create_l3_egress_s l3eg1;
    sal_memcpy(l3eg1.next_hop_mac_addr, next_hop_mac2 , 6);
    l3eg1.out_tunnel_or_rif = l3_eg_int;
    l3eg1.out_gport = out_sysport;
    l3eg1.vlan = out_vlan;
    l3eg1.fec_id = fec[1];
    l3eg1.arp_encap_id = encap_id[1];

    rv = l3__egress__create(unit,&l3eg1);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
        return rv;
    }

    fec[1] = l3eg1.fec_id;
    encap_id[1] = l3eg1.arp_encap_id;
    *_next_hop_mac_encap_id = encap_id[1];
    if(verbose >= 1) {
        printf("created FEC-id =0x%08x, in unit %d\n", fec[1], unit);
        printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[1], unit);
    }
    *encap_id_l2 = encap_id[0];

    /*** add route point to FEC2 ***/
    route = 0x7fffff00;
    mask  = 0xfffffff0;
    rv = add_route(unit, route, mask , vrf, fec[1]); 
    if (rv != BCM_E_NONE) {
        printf("Error, add_route in unit %d, \n", unit);
        return rv;
    }
    return rv;
}



/*
 * Creating mpls tunnel and termination
 */
int mpls_init(int unit, bcm_mpls_tunnel_switch_t *tunnel_switch, int *encap_id) {
  bcm_error_t rv;

  rv = l3_interface_init(unit, port_1, port_2, &next_hop_mac, encap_id, 1);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }


  rv = mpls_add_term_entry_ex(unit, mpls_label, 0, tunnel_switch);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n",bcm_errmsg(rv));
      return rv;
  }

  if (device_type <= device_type_arad_plus) {
      /* In Jericho, GAL label is recognized by the special labels mechanism */
      rv = mpls_add_gal_entry(unit);
      if (rv != BCM_E_NONE) {
          printf("(%s) \n", bcm_errmsg(rv));
          return rv;
      }
  }
  return rv;
}

int mpls_mep_ingress_only_set(void)
{
    ingress_only_flag=1;
    return 0;
}
int mpls_mep_egress_only_set(void)
{
    egress_only_flag=1;
    return 0;
}

/**
 *
 * It's use to create second y.1711  endpoint of type
 */
int add_y1711_mep_over_tunnel(int unit,  bcm_oam_group_t group_id,int gport,int outlif ,int label,bcm_gport_t out_gport)
{
  bcm_error_t rv;
  bcm_oam_endpoint_info_t mep_acc_test_info;
  bcm_oam_endpoint_info_t rmep_test_info;
  bcm_mpls_tunnel_switch_t tunnel_switch;
  bcm_oam_endpoint_info_t default_info;
  int encap_id;
  int mpls_termination_label_index_enable;

  rv = set_counter_source_and_engines(unit, &lm_counter_base_id_1_1711_ep,port_1);
  BCM_IF_ERROR_RETURN(rv);

  /*
  * Adding acc MEP
  */
  bcm_oam_endpoint_info_t_init(&mep_acc_info_y1711);
  /*RX*/
  mep_acc_info_y1711.type = bcmOAMEndpointTypeMPLSNetwork;
  mep_acc_info_y1711.group = group_id;
  mep_acc_info_y1711.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
  mep_acc_info_y1711.level = 0; /*Y1711 level*/
  mep_acc_info_y1711.gport = gport; /* in lif */
  mep_acc_info_y1711.mpls_out_gport = out_gport; /* out lif */
  mep_acc_info_y1711.lm_flags = BCM_OAM_LM_PCP * is_pcp;

  /*Use the same counter ID with 1711, which can make 1711 count 1731 packet*/
  mep_acc_info_y1711.lm_counter_base_id  = lm_counter_base_id_1_1711_ep;

  /*TX*/
  BCM_GPORT_SYSTEM_PORT_ID_SET(mep_acc_info_y1711.tx_gport, port_2); /* port that the traffic will be transmitted on */
  mep_acc_info_y1711.name = 123;
  mep_acc_info_y1711.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
  mep_acc_info_y1711.intf_id = outlif;

  mep_acc_info_y1711.egress_label.label = label;
  mep_acc_info_y1711.egress_label.ttl = 0xa;
  mep_acc_info_y1711.egress_label.exp = 1;

  if(y1711_lsp_pwe_flag == 1){
    mep_acc_info_y1711.mpls_network_info.function_type = 0x01; /*indicate lsp lm*/
  }
  else{
    mep_acc_info_y1711.mpls_network_info.function_type = 0x02;/*indicate pwe lm*/
  }

  /*The default timestamp_format is bcmOAMTimestampFormatNTP, set "oam_dm_ntp_enable" SOC to "0" to enable bcmOAMTimestampFormatIEEE1588v1*/
  mep_acc_info_y1711.timestamp_format = bcmOAMTimestampFormatIEEE1588v1;

  printf("bcm_oam_endpoint_create ( y1711_2nd )mep_acc_info\n");
  rv = bcm_oam_endpoint_create(unit, &mep_acc_info_y1711);
  if (rv != BCM_E_NONE) {
    printf("(%s) \n",bcm_errmsg(rv));
    return rv;
  }

  printf("created  ( y1711_2nd ) MEP with id %d\n", mep_acc_info_y1711.id);

  /*
  * Adding Remote MEP
  */
  bcm_oam_endpoint_info_t_init(&rmep_info_y1711);
  rmep_info_y1711.name = 0xff;
  rmep_info_y1711.local_id = mep_acc_info_y1711.id;
  rmep_info_y1711.type = bcmOAMEndpointTypeMPLSNetwork;
  rmep_info_y1711.ccm_period = 0;
  rmep_info_y1711.flags |= BCM_OAM_ENDPOINT_REMOTE;
  rmep_info_y1711.loc_clear_threshold = 1;

  printf("bcm_oam_endpoint_create  ( y1711_2nd ) RMEP\n");
  rv = bcm_oam_endpoint_create(unit, &rmep_info_y1711);
  if (rv != BCM_E_NONE) {
    printf("(%s) \n",bcm_errmsg(rv));
    return rv;
  }
  printf("created  ( y1711_2nd ) RMEP with id %d\n", rmep_info_y1711.id);

  /* Store endpoint info in global parameter.*/
  ep_y1711.gport =  mep_acc_info_y1711.gport;
  ep_y1711.id = mep_acc_info_y1711.id;
  ep_y1711.rmep_id = rmep_info_y1711.id;


  bcm_oam_endpoint_info_t_init(&rmep_test_info);
  printf("bcm_oam_endpoint_get  ( y1711_2nd ) rmep_test_info\n");
  rv = bcm_oam_endpoint_get(unit, mep_acc_info_y1711.id, &rmep_test_info);
  if (rv != BCM_E_NONE) {
    printf("(%s) \n",bcm_errmsg(rv));
    return rv;
  }

  return rv;
}

/**
 *
 * It's use to create accelerated y.1731 OAM endpoint of type MPLS-TP or PWE
 * User can test 1711 MEP and 1731 MEP at the same lif, and use 1711 MEP to count 1731 packet.
 */
int add_y1731_mep_over_tunnel(int unit,  bcm_oam_group_t group_id,bcm_oam_endpoint_type_t type, int gport,int outlif ,int label,bcm_gport_t out_gport)
{
  bcm_error_t rv;
  bcm_oam_endpoint_info_t mep_acc_test_info;
  bcm_oam_endpoint_info_t rmep_test_info;
  bcm_mpls_tunnel_switch_t tunnel_switch;
  bcm_oam_endpoint_info_t default_info;
  int encap_id;
  int mpls_termination_label_index_enable;

  /*
  * Adding acc MEP
  */
  bcm_oam_endpoint_info_t_init(&mep_acc_info_y1731);
  /*RX*/
  mep_acc_info_y1731.type = type;
  mep_acc_info_y1731.group = group_id;
  mep_acc_info_y1731.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
  mep_acc_info_y1731.level = 3; /*Y1731 level*/
  mep_acc_info_y1731.gport = gport; /* in lif */
  mep_acc_info_y1731.mpls_out_gport = (type==bcmOAMEndpointTypeBHHPwe) ? gport : out_gport; /* out lif */
  mep_acc_info_y1731.lm_flags = BCM_OAM_LM_PCP * is_pcp;

  /*Use the same counter ID with 1711, which can make 1711 count 1731 packet*/
  mep_acc_info_y1731.lm_counter_base_id  = two_1711_mep_flag ? lm_counter_base_id_1_1711_ep : lm_counter_base_id_1_mpls_ep;

  /*TX*/
  BCM_GPORT_SYSTEM_PORT_ID_SET(mep_acc_info_y1731.tx_gport, port_2); /* port that the traffic will be transmitted on */
  mep_acc_info_y1731.name = 123;
  mep_acc_info_y1731.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
  mep_acc_info_y1731.intf_id = (type == bcmOAMEndpointTypeBHHPwe) ? mpls_tunnel_encap_id1 : outlif;

  if(soc_property_suffix_num_get(unit, -1, spn_CUSTOM_FEATURE, "oam_injected_over_lsp_cnt",0)
     && (type == bcmOAMEndpointTypeBHHMPLS)){
    /* Use below field to build GAL lable, ttl.exp */
    /* MPLSTP-GAL: Label=13;Exp=0,BOS=1,TTL=64,which is from Y1731oMplstpGal*/
    mep_acc_info_y1731.egress_label.label = 13;
    mep_acc_info_y1731.egress_label.ttl = 0x40;
    mep_acc_info_y1731.egress_label.exp = 0;
    mep_acc_info_y1731.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
    /* Put MEP's outlif to ITMH's oulitf*/
    mep_acc_info_y1731.intf_id = out_gport;
    bcm_oam_mpls_tp_channel_type_tx_set(0,bcmOamMplsTpChannelPweonoam,0x8902);
  } else{
    mep_acc_info_y1731.egress_label.label = label;
    mep_acc_info_y1731.egress_label.ttl = 0xa;
    mep_acc_info_y1731.egress_label.exp = 1;
 }

  if (soc_property_get(unit,"oam_use_double_outlif_injection",1)
    && (type==bcmOAMEndpointTypeBHHPwe)){
    mep_acc_info_y1731.flags2 = BCM_OAM_ENDPOINT_FLAGS2_USE_DOUBLE_OUTLIF_INJ;
  }

  /*The default timestamp_format is bcmOAMTimestampFormatNTP, set "oam_dm_ntp_enable" SOC to "0" to enable bcmOAMTimestampFormatIEEE1588v1*/
  mep_acc_info_y1731.timestamp_format = bcmOAMTimestampFormatIEEE1588v1;

  printf("bcm_oam_endpoint_create ( y1731 )mep_acc_info\n");
  rv = bcm_oam_endpoint_create(unit, &mep_acc_info_y1731);
  if (rv != BCM_E_NONE) {
    printf("(%s) \n",bcm_errmsg(rv));
    return rv;
  }

  printf("created  ( y1731 ) MEP with id %d\n", mep_acc_info_y1731.id);

  /*
  * Adding Remote MEP
  */
  bcm_oam_endpoint_info_t_init(&rmep_info_y1731);
  rmep_info_y1731.name = 0xff;
  rmep_info_y1731.local_id = mep_acc_info_y1731.id;
  rmep_info_y1731.type = type;
  rmep_info_y1731.ccm_period = 0;
  rmep_info_y1731.flags |= BCM_OAM_ENDPOINT_REMOTE;
  rmep_info_y1731.loc_clear_threshold = 1;

  printf("bcm_oam_endpoint_create  ( y1731 ) RMEP\n");
  rv = bcm_oam_endpoint_create(unit, &rmep_info_y1731);
  if (rv != BCM_E_NONE) {
    printf("(%s) \n",bcm_errmsg(rv));
    return rv;
  }
  printf("created  ( y1731 ) RMEP with id %d\n", rmep_info_y1731.id);

  /* Store endpoint info in global parameter.*/
  ep_y1731.gport =  mep_acc_info_y1731.gport;
  ep_y1731.id = mep_acc_info_y1731.id;
  ep_y1731.rmep_id = rmep_info_y1731.id;


  bcm_oam_endpoint_info_t_init(&rmep_test_info);
  printf("bcm_oam_endpoint_get  ( y1731 ) rmep_test_info\n");
  rv = bcm_oam_endpoint_get(unit, mep_acc_info_y1731.id, &rmep_test_info);
  if (rv != BCM_E_NONE) {
    printf("(%s) \n",bcm_errmsg(rv));
    return rv;
  }

  return rv;
}



/**
 *  An example of creating Loopback and TST function base on MPLS-TP
 * 
 *  1,  Create accelerated y.1731 OAM endpoint of type MPLS-TP  
 *  2,  Create a LoopBack or TST function MPLS-TP or PWE
 *  3,  Configure SAT to send lb or tst packet.
 *
 * @author xiao (13/04/2016)
 * 
 * @param unit 
 * @param port1 physical ports to be used
 * @param port2 
 * @param port3 
 * @param lb_tst_flag: 1means creating lb, 2 means creating tst
 * @param mpls_pwe_flag: 1means mpls , 2 means pwe, 3 means MPLS-TP Section OAM
 * @return int 
 */

int oam_run_lb_tst_mpls_pwe(int unit, int port1, int port2, int port3, int lb_tst_flag,int mpls_pwe_flag,
                            bcm_oam_endpoint_t mpls_ep_id)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_oam_loopback_t lb;
    int gtf_id=0;
    bcm_oam_tst_rx_t  tst_rx_ptr;
    bcm_oam_tst_tx_t  tst_tx_ptr;
    bcm_oam_endpoint_t ep_id;

    if(mpls_pwe_flag == 1){ /*mpls*/
        if(mpls_pwe_set_flag == 0){
            rv= oam_run_with_defaults_mpls_tp (unit, port1, port2, port3, 0);
            if (rv != BCM_E_NONE) {
               printf("Error, in oam_run_with_defaults_mpls_tp\n");
               return rv;
            }
            mpls_pwe_set_flag = 1;
       }
       ep_id = mep_acc_info.id;
   }
    else if(mpls_pwe_flag == 2){/*pwe*/
        if(mpls_pwe_set_flag == 0){
            rv= oam_run_with_defaults_pwe (unit, port1, port2, port3);
            if (rv != BCM_E_NONE) {
               printf("Error, in oam_run_with_defaults_mpls_tp\n");
               return rv;
            }
            mpls_pwe_set_flag = 1;
       }
       ep_id = mep_acc_info.id;
    } else {/*MPLS-TP Section */
       ep_id = mpls_ep_id; 
    }

    bcm_oam_loopback_t_init(&lb);
    bcm_oam_tst_rx_t_init(&tst_rx_ptr);
    bcm_oam_tst_tx_t_init(&tst_tx_ptr);
    tst_tx_ptr.endpoint_id=tst_rx_ptr.endpoint_id =  lb.id = ep_id;
    if (1 == lb_tst_flag){ 
        lb.num_tlvs =1;   
        lb.tlvs[0].four_byte_repeatable_pattern =0x0;
        lb.tlvs[0].tlv_type =bcmOamTlvTypeData;
        lb.tlvs[0].tlv_length =36;
        lb.peer_da_mac_address[1] = 0xab;
       rv = bcm_oam_loopback_add(unit,&lb);
       if (rv != BCM_E_NONE) {
           printf("(%s) \n",bcm_errmsg(rv));
           return rv;
       }
       
       lb_tst_gtf_id = lb.gtf_id;
       printf("MEP id %d,  lb gtf %d \n\n",ep_id, lb.gtf_id);
    }
    else if (2 == lb_tst_flag){
        tst_tx_ptr.tlv.tlv_length = 36;
        tst_tx_ptr.tlv.tlv_type =  bcmOamTlvTypeTestNullWithoutCRC;
        tst_rx_ptr.expected_tlv.tlv_length = 36;
        tst_rx_ptr.expected_tlv.tlv_type =  bcmOamTlvTypeTestNullWithoutCRC;
        rv = bcm_oam_tst_rx_add(unit, &tst_rx_ptr);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        rv = bcm_oam_tst_tx_add(unit, &tst_tx_ptr);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        lb_tst_gtf_id = tst_tx_ptr.gtf_id;
        printf("`````````  tst gtf %d ctf %d  ``````````\n\n",tst_tx_ptr.gtf_id,tst_rx_ptr.ctf_id);

    }
    
    /*start to send lb/tst packet*/
    rv = oam_sat_start(unit, lb_tst_gtf_id);
    if (rv != BCM_E_NONE) {
          printf("(%s) \n", bcm_errmsg(rv));
          return rv;
    }

    return rv;
}


 /*
 * configure SAT starting to send packet.
 */
int oam_sat_start(int unit, int gtf_id)
 {
     bcm_error_t rv;
     bcm_sat_gtf_bandwidth_t bw1,bw2;
     bcm_sat_gtf_rate_pattern_t rate_pattern1,rate_pattern2;
     
     bcm_sat_gtf_bandwidth_t_init(&bw1);
     bcm_sat_gtf_bandwidth_t_init(&bw2);
     bcm_sat_gtf_rate_pattern_t_init(&rate_pattern1);
     bcm_sat_gtf_rate_pattern_t_init(&rate_pattern2);
      
     /*1 Set Bandwidth */
     bw1.rate = 128; /*gtf_bw_cir_rate;*/
     bw1.max_burst = 200;/*gtf_bw_cir_max_burst;*/
     rv = bcm_sat_gtf_bandwidth_set(unit, gtf_id, 0, &bw1);
     if (rv != BCM_E_NONE) {
         printf("(%s) \n", bcm_errmsg(rv));
         return rv;
     }
     
     bw1.rate = 128;/*gtf_bw_cir_rate + gtf_bw_eir_rate;*/
     bw1.max_burst = 500;/*gtf_bw_cir_max_burst+ gtf_bw_eir_max_burst;*/
     rv = bcm_sat_gtf_bandwidth_set(unit, gtf_id, 1, &bw1);
     if (rv != BCM_E_NONE) {
         printf("(%s) \n", bcm_errmsg(rv));
         return rv; 
     }
     
     bw1.rate = 256;/*gtf_bw_cir_rate + gtf_bw_eir_rate;*/
     bw1.max_burst = 500;/*gtf_bw_cir_max_burst+ gtf_bw_eir_max_burst;*/
     rv = bcm_sat_gtf_bandwidth_set(unit, gtf_id, -1, &bw1);
     if (rv != BCM_E_NONE) {
         printf("(%s) \n", bcm_errmsg(rv));
         return rv;
     }
     
     /* 2. Set Rate Pattern */
     rate_pattern1.rate_pattern_mode = bcmSatGtfRatePatternContinues; /*gtf_rate_pattern_mode;*/
     rate_pattern1.high_threshold = 10; /*gtf_rate_pattern_high;*/
     rate_pattern1.low_threshold =10 ; /*gtf_rate_pattern_low;*/
     rate_pattern1.stop_iter_count = 1000; /*gtf_rate_pattern_stop_iter_count;*/
     rate_pattern1.stop_burst_count = 10000; /*gtf_rate_pattern_stop_burst_count;*/
     rate_pattern1.stop_interval_count = 10000; /*gtf_rate_pattern_stop_interval_count;*/
     rate_pattern1.burst_packet_weight = 1; /*gtf_rate_pattern_burst_packet_weight;*/
     rate_pattern1.interval_packet_weight = 1; /*gtf_rate_pattern_interval_packet_weight;*/
     rv = bcm_sat_gtf_rate_pattern_set(unit,gtf_id, 0, &rate_pattern1);
     if (rv != BCM_E_NONE) {
         printf("(%s) \n", bcm_errmsg(rv));
         return rv;
     }
	 /*CIR and EIR have the same sequence number, so we just need to start one of them*/
     rv = bcm_sat_gtf_packet_start(unit, gtf_id, bcmSatGtfPriCir);
     if (rv != BCM_E_NONE) {
         printf("(%s) \n", bcm_errmsg(rv));
         return rv;
     }
	 
     return 0;
  }
 
 int register_sd_sf_events(int unit) {
   bcm_error_t rv = BCM_E_NONE;
   bcm_oam_event_types_t sdSet;
   bcm_oam_event_types_t sdClear;
   bcm_oam_event_types_t sfSet;
   bcm_oam_event_types_t sfClear;
 
   BCM_OAM_EVENT_TYPE_SET(sdSet, bcmOAMEventEndpointSdSet);
 
   rv = bcm_oam_event_register(unit, sdSet, sd_sf_cb, (void*)1);
 
   BCM_OAM_EVENT_TYPE_SET(sdClear, bcmOAMEventEndpointSdClear);
 
   rv = bcm_oam_event_register(unit, sdClear, sd_sf_cb, (void*)2);
   
   BCM_OAM_EVENT_TYPE_SET(sfSet, bcmOAMEventEndpointSfSet);
 
   rv = bcm_oam_event_register(unit, sfSet, sd_sf_cb, (void*)3);
 
   BCM_OAM_EVENT_TYPE_SET(sfClear, bcmOAMEventEndpointSfClear);
 
   rv = bcm_oam_event_register(unit, sfClear, sd_sf_cb, (void*)4);
   
   return rv;
 }

/**
 *  An example of using OAM sd/sf function
 * 
 *  1, create Y1731oMpls-tp OAM instance  
 *  2, configure SD/SF threshold and sliding window
 *  3, send and stop cmm packet to verify the sd/sf set and clear.
 * 
 * @author xiao (03/03/2016)
 * 
 * @param unit 
 * @param port1 physical ports to be used
 * @param port2 
 * @param port3 
 * @param event_enable
 * @return int 
 */
int oam_run_sd_sf(int unit, int port1, int port2, int port3,int event_enable)
{
    bcm_error_t rv = BCM_E_NONE;
    bcm_oam_sd_sf_detection_t_init(&sdsf);
    sd_sf_enable = 1;
    rv= oam_run_with_defaults_mpls_tp (unit, port1, port2, port3, 0);
    if (rv != BCM_E_NONE) {
      printf("Error, in oam_run_with_defaults_mpls_tp\n");
      return rv;
    }
    sdsf.mep_id = mep_acc_info.id;
    sdsf.rmep_id = rmep_info.id;
    sdsf.sd_set_threshold = 2;
    sdsf.sf_set_threshold = 2;
    sdsf.sd_clear_threshold = 1;
    sdsf.sf_clear_threshold = 1;
    sdsf.sliding_window_length = (event_enable==1) ? 1 : 250;
    /*sdsf.flags |= BCM_OAM_SD_SF_FLAGS_ALERT_METHOD;
    sdsf.flags |= BCM_OAM_SD_SF_FLAGS_ALERT_SUPRESS;
    */
    rv = bcm_oam_sd_sf_detection_add(unit,&sdsf);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    rv = register_sd_sf_events(unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    return rv;
}
/**
 * OAM_endpoint_action_set calling sequence example.
 * 
 * @param unit 
 * @param port - Must be BCM_GPORT_INVALID for actions requiring
 *             invalid gport. otherwise, trap
 * @param endpoint_id 
 * @param action_type 
 * @param opcode - OAM opcode upon which action will be applied
 * 
 * @return int 
 */
int oam_action_set(int unit, int dest_port, int endpoint_id, bcm_oam_action_type_t action_type, int opcode) {
    bcm_error_t rv;
    bcm_oam_endpoint_action_t action;
    int trap_code;
    bcm_rx_trap_config_t trap_config;
    bcm_rx_trap_t trap_type;
    bcm_oam_endpoint_info_t endpoint_info;

    bcm_oam_endpoint_action_t_init(&action);

    action.destination = dest_port;
    if (dest_port != BCM_GPORT_INVALID && !BCM_GPORT_IS_TRAP(dest_port)) {
        /* action.destination can only receive trap as destination. Allocate new trap */
        trap_type = bcmRxTrapUserDefine;

        /* if MEP is up, Different trap type is used to prevent two sets of system headers */
        rv = bcm_oam_endpoint_get(unit, endpoint_id, &endpoint_info);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }

        if (endpoint_info.flags & BCM_OAM_ENDPOINT_UP_FACING) {
            trap_type = bcmRxTrapOamUpMEP4;
        }

        rv = bcm_rx_trap_type_create(unit, 0, trap_type, &trap_code);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }

        bcm_rx_trap_config_t_init(&trap_config);
        trap_config.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_TRAP;
        rv = port_to_system_port(unit, dest_port, &trap_config.dest_port);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }

        rv = bcm_rx_trap_set(unit, trap_code, trap_config);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }

        printf("Trap set, trap_code = %d \n", trap_code);
        BCM_GPORT_TRAP_SET(action.destination, trap_code, 7, 0);
    }

    BCM_OAM_OPCODE_SET(action, opcode);
    BCM_OAM_ACTION_SET(action, action_type);

    rv = bcm_oam_endpoint_action_set(unit, endpoint_id, &action);
    printf( "Action set created\n");

    return rv;
}

/* OAM-TS header:
 *
 * OAM-TS header is the following:
 * bits                       meaning
 * ===================================
 * 47:46                      type of OAM-TS extension: 0-OAM 
 * 45:43                      OAM-Sub-Type: 0-CCM; 1-LM; 2-DM (1588 ToD) 3-DM (NTP ToD)
 * 42                         Up-MEP ('1') or Down-MEP ('0')
 * 41:8                       OAM-TS-Data: Interpretation of OAM-TS-Data depends on the OAM-TS type, sub type, and location in the system.
 *                                        Location in the system may be:
 *                                        IRPP - following ITMH on an injected packet
 *                                        ETPP or Out I/F - following FTMH
 *                                        OAM-Data is a function of (Location, Sub-Type, MEP-Type, Inject/Trap):
 *                                        "    (IRPP, LM, Up, Inj) - NA
 *                                        "    (IRPP, LM, Dn, Inj) - Counter-Pointer // bypass
 *                                        "    (IRPP, DM, Up, Inj) - NA
 *                                        "    (IRPP, DM, Dn, Inj) - Null // bypass
 *                                        "    (IRPP, LM/DM, Up/Dn, Trp) - NA
 *                                        "    (ETPP, LM, Up, Inj) - Counter-Value // Stamp into packet
 *                                        "    (ETPP, LM, Dn, Inj) - Counter-Pointer // Read counter and stamp into packet
 *                                        "    (ETPP, DM, Up, Inj) - ToD // Stamp into packet
 *                                        "    (ETPP, DM, Dn, Inj) -Null //Read ToD and stamp into packet
 *
 *                                        "    (ETPP, LM, Up, Trp) - NA // ETPP build OAM-TS Header w/ Counter-Value
 *                                        "    (ETPP, LM, Dn, Trp) - Counter-Value // bypass to Out I/F
 *                                        "    (ETPP, DM, Up, Trp) - NA // ETPP build OAM-TS Header w/ ToD
 *                                          "    (ETPP, DM, Dn, Trp) - ToD// bypass to Out I/F
 * 7:0                         Offset from end of System Headers to where to stamp/update.
 * 
 * parsing pkt_dm_down packet:
 *     ITMH      OAM-TS                ETH.               VLAN    MPLS LSP   GAL     G-ACH           PDU  
 * 0x800c000d|180000000022|00000000cd1d000c000200008100|00648847|0006 420a|0000D140|10008202|e02f002000000000000003e70000 
 * ITMH - send packet to port 13
 * OAM-TS - packet type DM, stamping offset - 0x22
 * 
 * parsing pkt_lm_down packet:
 * 0x800c000d|080000000522|00000000cd1d000c000200008100|00648847|0006 420a|0000D140|10008202|e02b000c000000000000000000000000
 *    ITMH       OAM-TS            ETH.                  VLAN     MPLS LSP   GAL     G-ACH           PDU  
 * ITMH - send packet to port 13
 * OAM-TS - packet type LM, stamping offset - 16, counter id - 5
 */
int inject_dm_and_lm_packets(int unit) {
  bcm_error_t rv;
  char *pkt_dm_down, *pkt_lm_down;
  int ptch_down[2];

  pkt_dm_down = "800c000d18000000002200000000cd1d000c000200008100006488470006420a0000D14010008202e02f002000000000000003e70000"; /*DM down*/
  pkt_lm_down = "800c000d08000000052200000000cd1d000c000200008100006488470006420a0000D14010008202e02b000c00000000000000000000"; /*LM down*/
  ptch_down[0] = 0; /* The next header is ITMH */
  ptch_down[1] = 0; /* in port is port_1 */

  rv = tx_packet_via_bcm(unit, pkt_lm_down, ptch_down, 2);
  rv = tx_packet_via_bcm(unit, pkt_dm_down, ptch_down, 2);
  return rv;
}


int sd_sf_cb(
    int unit,
    uint32 flags,
    bcm_oam_event_type_t event_type,
    bcm_oam_group_t group,
    bcm_oam_endpoint_t endpoint,
    void *user_data)
    {
        bcm_error_t rv = BCM_E_NONE;
        print unit;
        print flags;
        print event_type;
        print group;
        print endpoint;

        if (event_type == bcmOAMEventEndpointSdSet) {
            sd_events_set_count ++ ;
        }
        else if (event_type == bcmOAMEventEndpointSfSet){
            sf_events_set_count ++ ;
        }
        else if (event_type == bcmOAMEventEndpointSdClear){
            sd_events_clear_count ++ ;
        }
        else if (event_type == bcmOAMEventEndpointSfClear){
            sf_events_clear_count ++ ;
        }

        return rv;
    }

/*
* It use to read sd/sf event counters.
* sd_sf_flag: 1-sd, 2-sf
* set_clear_flag: 1-set event, 2-clear event
* expected_event_count 
*/
int read_sd_sf_event_count(int sd_sf_flag, int set_clear_flag, int expected_event_count) {
    int rv = BCM_E_NONE;
    int event_counter =0;
    if((sd_sf_flag !=1)&&(sd_sf_flag !=2)){
        return BCM_E_PARAM;
    }
    if((set_clear_flag !=1)&&(set_clear_flag !=2)){
        return BCM_E_PARAM;
    }

    if(sd_sf_flag == 1){
        event_counter =  (set_clear_flag ==1 ) ? sd_events_set_count : sd_events_clear_count;
    } else{
        event_counter =  (set_clear_flag ==1 ) ? sf_events_set_count : sf_events_clear_count;
    }
    if (event_counter==expected_event_count) {
        return BCM_E_NONE;
    }
    else {
        return BCM_E_FAIL;
    }
 }


/* enable y1711*/
int oam_run_with_defaults_mpls_tp_y1711(int unit, int port1, int port2, int port3, int use_mpls_out_gport) {
    bcm_error_t rv;

    is_y1711=1; /*enable y1711*/

    rv= oam_run_with_defaults_mpls_tp (unit, port1, port2, port3, use_mpls_out_gport);
    if (rv != BCM_E_NONE) {
      printf("Error, in oam_run_with_defaults_mpls_tp\n");
      return rv;
    }

    return rv;
}


/*
 *  An example of using 1711-LM function
 * 
 *  1, create Y1711 OAM instance base on LSP or PWE
 *  2, create LM 
 *  3, verify LMM sending and LMR receiving.
 * 
 * @author xiao (08/03/2016)
 * 
 * @param port1 
 * @param port2 
 * @param port3 
 * @param lsp_pwe_flag: (lsp:1, pwe:2)
 * @param is_two_1711_mep: whether create two 1711 MEP to test or not
 * @return int 
 */

int oam_run_with_defaults_y1711_lm(int unit, int port1, int port2, int port3, int lsp_pwe_flag,int is_two_1711_mep ) {
    bcm_error_t rv;
    port_1 = port1;
    port_2 = port2;
    port_3 = port3;
    bcm_oam_loss_t loss_obj;
    
    if(lsp_pwe_flag == 1){
        y1711_lsp_pwe_flag=1;    /* y1711oLsp */
    }
    else if(lsp_pwe_flag == 2){
        y1711_lsp_pwe_flag=2;    /*y1711oPwe */
    }
    else{
        printf("lsp_pwe_flag is error \n");
        return BCM_E_PARAM;
    }
    
    is_y1711=1;
    
    rv = mpls__mpls_pipe_mode_exp_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__mpls__mpls_pipe_mode_exp_set\n");
        return rv;
    }
    
    rv = oam__device_type_get(unit, &device_type);
    if (rv < 0) {
        printf("Error checking whether the device is arad+.\n");
        print rv;
        return rv;
    }

    if(is_two_1711_mep == 1){
        two_1711_mep_flag=1;
    }
    rv = oam_example_over_tunnel(unit, bcmOAMEndpointTypeMPLSNetwork, 0);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    rv = register_events(unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    
    bcm_oam_loss_t_init(&loss_obj);
    loss_obj.id = mep_acc_info.id;
    loss_obj.period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
    loss_obj.flags = BCM_OAM_LOSS_SINGLE_ENDED | BCM_OAM_LOSS_WITH_ID;
    loss_obj.loss_id = 0x100;
    
    rv = bcm_oam_loss_add(unit,&loss_obj);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    if(is_two_1711_mep == 1){
        bcm_oam_loss_t_init(&loss_obj);
        loss_obj.id = mep_acc_info_y1711.id;
        loss_obj.period = BCM_OAM_ENDPOINT_CCM_PERIOD_1S;
        loss_obj.flags = BCM_OAM_LOSS_SINGLE_ENDED | BCM_OAM_LOSS_WITH_ID;
        loss_obj.loss_id = 0x200;

        rv = bcm_oam_loss_add(unit,&loss_obj);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    return rv;
    
}
/* enable y1711*/
int oam_run_with_defaults_pwe_y1711 (int unit, int port1, int port2, int port3) {
    bcm_error_t rv;

    is_y1711=1; /*enable y1711*/

    rv= oam_run_with_defaults_pwe (unit, port1, port2, port3);
    if (rv != BCM_E_NONE) {
      printf("Error, in oam_run_with_defaults_mpls_tp\n");
      return rv;
    }

    return rv;
}

/* Replace the intf_id in the MEP created by oam_run_with_defaults_mpls_tp()
 * Changes the MEP's egress vlan from 100 to 200
 */
int oam_mpls_tp_replace_intf_id(int unit)
{
    bcm_oam_endpoint_info_t endpoint;
    bcm_l3_egress_t         egr_old, egr_new;
    bcm_if_t                l3_eg_id;
    bcm_error_t             rv;

    bcm_oam_endpoint_info_t_init(&endpoint);

    rv = bcm_oam_endpoint_get(unit, ep.id, &endpoint);
    if (rv != BCM_E_NONE) {
        printf("Cannot find endpoint (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    bcm_l3_egress_t_init(&egr_old);
    rv = bcm_l3_egress_get(unit, endpoint.intf_id, &egr_old);
    if (rv != BCM_E_NONE) {
        printf("Cannot find egress object (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    bcm_l3_egress_t_init(&egr_new);
    sal_memcpy(egr_new.mac_addr, egr_old.mac_addr, 6);
    egr_new.intf   = egr_old.intf;
    egr_new.port   = egr_old.port;
    egr_new.vlan   = 200;
    rv = bcm_l3_egress_create(unit, 0, &egr_new, &l3_eg_id);
    if (rv != BCM_E_NONE) {
        printf("Cannot create egress object (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    endpoint.intf_id = egr_new.encap_id;
    endpoint.flags  |= (BCM_OAM_ENDPOINT_REPLACE | BCM_OAM_ENDPOINT_WITH_ID);
    rv = bcm_oam_endpoint_create(unit, &endpoint);
    if (rv != BCM_E_NONE) {
        printf("Cannot replace endpoint (%s) \n",bcm_errmsg(rv));
        return rv;
    }

    return BCM_E_NONE;
}
