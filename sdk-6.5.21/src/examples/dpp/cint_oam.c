/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~OAM test~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 *
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * 
 * File: cint_oam.c
 * Purpose: Example of using OAM APIs.
 *
 * Usage:
 * 
cd ../../../../src/examples/dpp
cint ../sand/utility/cint_sand_utils_global.c
cint utility/cint_utils_multicast.c
cint utility/cint_utils_mpls_port.c
cint utility/cint_utils_oam.c
cint cint_port_tpid.c
cint cint_advanced_vlan_translation_mode.c
cint cint_l2_mact.c
cint cint_vswitch_metro_mp.c
cint utility/cint_multi_device_utils.c
cint cint_queue_tests.c
cint cint_oam.c
cint
int unit =0;
int port_1=13, port_2 = 14; port_3=15;
print oam_run_with_defaults(unit,port_1,port_2,port_3);

 * 
 * This cint uses cint_vswitch_metro_mp_single_vlan.c to build the following vswitch:
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *  |                                                                       |
 *  |                   o  \                         o                      |
 *  |                       \  \<4>       <10,20>/  /                       |
 *  |                  /\ \  \   -----------    /  /                        |
 *  |                   \  \ \/ /  \   /\   \  /  / /\                      |
 *  |                <40>\  \  |    \  /     |\/ /  /                       |
 *  |                     \  \ |     \/      |  /  /<30>                    |
 *  |                       p3\|     /\      | /  /                         |
 *  |                          |    /  \     |/p1                           |             
 *  |                         /|   \/   \    |                              |
 *  |                        /  \  VSwitch  /                               | 
 *  |                   /\  /p2  -----------                                |
 *  |                <5>/  /  /                                             |
 *  |                  /  /  /<3>                                           |
 *  |                 /  /  /                                               |
 *  |                   /  \/                                               |
 *  |                  o                                                    |
 *  |                                                                       | 
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 * The following MAs:
 * 		1) MA with id 0 & long name format:       1-const; 32-long format; 13-length; all the rest - MA name
 *         long_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 32, 13, 1, 2, 3, 4, 5, 6, 7, 8, 9}
 * 		2) MA with id 1 & short name format:    1-const; 3-short format; 2-length; all the rest - MA name
 *        short_name[BCM_OAM_GROUP_NAME_LENGTH] = {1, 3, 2, 0xcd, 0xab}
 * 
 * The following MEPs:
 * 		1) MEP with id 0X400<MEP's LIF> : non-accellerated, mac address {0x00, 0x00, 0x00, 0x01, 0x02, 0x03}, in group 2, MD-Level 4, Down-MEP
 *                   ---> Packets recieved on port_1 with VLAN 0xa and MD-Level 4 will be trapped by this MEP.
 *                                  -- For example (from the bcm shell)
 *                                          tx 1 PtchSRCport=<port_1> DATA=0000000102030000556abcde8100000a8902802b000c00000000000000000000000000000000
 *  	2) MEP with id 4096: accellerated,     mac address {0x00, 0x00, 0x00, 0xff, 0xff, 0xff}, in group 2, MD-Level 5, Up-MEP
 *                   ---> Packets recieved on port_1 with VLAN 0xa and MD-Level 5 will be trapped by this MEP (at the egress, MEP resides on port_2, at the OutLIF)
 *  	    2.a) RMEP with id 0X2001000 and Name 255
 *                          ---> CCMs with Name 255 and the above Level/Vlan will be processed by the RMEP in the OAMP.
 *                                  -- For example (from the bcm shell)
 *                                          tx 1 PtchSRCport=13 DATA=0000000102040000556abcde8100000a8902a00101460000000000ff010302abcd00000000000000
 *                   ---> The OAMP will transmit CCMs with with VLAN 0x5, Up MEP. VLAN editing may be applied in the pipeline.
 *      3)  MEP with id 4097: accellerated,     mac address {0x00, 0x00, 0x00, 0x01, 0x02, 0xff}, in group 2, mdlevel 2, Down-MEP
 *                   ---> Packets recieved on port_1 with VLAN 0xa and MD-Level 4 will be trapped by this MEP.
 *          3.a) RMEP with id 0X2001001 and Name 17
 *                          ---> CCMs with Name 17 and the above Level/Vlan will be processed by the RMEP in the OAMP.
 *                   ---> The OAMP will transmit CCMs with with VLAN 0xa, Down MEP. 
 *                                  -- For example (from the bcm shell)
 *                                          tx 1 PtchSRCport=<port_1> DATA=0000000102030000556abcde8100000a8902402b000c00000000000000000000000000000000
 *
 *
 * 
 * In addition, get & delete APIs are used for testing.
 * 
 * comments:
 * 1) In order to prevent from OAMP send packets do: BCM.0> m OAMP_MODE_REGISTER TRANSMIT_ENABLE=0
 */
 
/*
 * Creating vswitch and adding mac table entries
 */

bcm_mac_t mac_mip = {0x00, 0x00, 0x00, 0x01, 0x02, 0xfe};
bcm_mac_t mac_mep_2_mc = {0x01, 0x80, 0xc2, 0x00, 0x00, 0x35}; /* 01-80-c2-00-00-3 + md_level_2 */
bcm_oam_group_info_t group_info_11b_ma;

/*4-const; 3-MD-Length; MD-Name+Zero-padding; 2-const; MA-Length; MA-name*/
uint8 str_11b_name[BCM_OAM_GROUP_NAME_LENGTH] = {4, 3, 'A', 'T', 'T', 00, 00, 02, 04, 'E', 'V', 'C', '1', 0, 0};

int md_level_mip = 7;
bcm_oam_endpoint_info_t mip_info;

/* Created endpoints information */
oam__ep_info_s acc_ep_up;
oam__ep_info_s acc_ep_down;

/* Whether to add by default bcmOAMActionCountEnable when using oam_action_set */
int count_enable_on_oam_action = 1;

/* Indicates if bcmRxTrapOamUpMEP4 trap has already been created */
int gTrapUp4AlreadyCreated = 0;


/****************************************************************/
/*                              OAM UTILITIES FUNCTIONS                                                       */
/****************************************************************/

/**
 * Prints group id and MEG, then traverses and prints its endpoints
 */
int print_group_eps(int unit, bcm_oam_group_info_t* info, void* user_data) {
    int i;
    printf("Group: %d\nName: 0x", info->id, group_name);
    for(i=0; i<BCM_OAM_GROUP_NAME_LENGTH; i++) {
        printf("%02x", info->name[i]);
    }
    printf("\n");
    bcm_oam_endpoint_traverse(unit, info->id, print_ep, (void*)0);
}

/**
 * Calls "diag oam ep" for a given endpoint
 */
int print_ep(int unit, bcm_oam_endpoint_info_t* info, void* user_data) {
    char line[256];
    sprintf(line, "diag oam ep id=%d", info->id);
    bshell(unit, line);
}

/**
 * Traverses groups, prints group id and MEG, then traverses and calls diag on its endpoints
 *
 * @param unit
 *
 * @return int
 */
int oam_groups_print(int unit) {
    return bcm_oam_group_traverse(unit, print_group_eps, (void*)0);
}


/**
 * Wrapper function for transmitting packet, including PTCH.
 * 
 * 
 * @param unit 
 * @param data 
 * @param ptch0 - set local port from which packet should be 
 *              transmitted.
 * @param ptch1 
 * 
 * @return int 
 */
int oam_tx(int unit, char *data, int ptch0, int ptch1) {
    int ptch[2];
    ptch[0] = ptch0;
    ptch[1] = ptch1;

    return tx_packet_via_bcm(unit, data, ptch, 2);
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
 *										Location in the system may be:
 *										IRPP - following ITMH on an injected packet
 *										ETPP or Out I/F - following FTMH
 *										OAM-Data is a function of (Location, Sub-Type, MEP-Type, Inject/Trap):
 *										"	(IRPP, LM, Up, Inj) - NA
 *										"	(IRPP, LM, Dn, Inj) - Counter-Pointer // bypass
 *										"	(IRPP, DM, Up, Inj) - NA
 *										"	(IRPP, DM, Dn, Inj) - Null // bypass
 *										"	(IRPP, LM/DM, Up/Dn, Trp) - NA
 *										"	(ETPP, LM, Up, Inj) - Counter-Value // Stamp into packet
 *										"	(ETPP, LM, Dn, Inj) - Counter-Pointer // Read counter and stamp into packet
 *										"	(ETPP, DM, Up, Inj) - ToD // Stamp into packet
 *										"	(ETPP, DM, Dn, Inj) -Null //Read ToD and stamp into packet
 *
 *										"	(ETPP, LM, Up, Trp) - NA // ETPP build OAM-TS Header w/ Counter-Value
 *										"	(ETPP, LM, Dn, Trp) - Counter-Value // bypass to Out I/F
 *										"	(ETPP, DM, Up, Trp) - NA // ETPP build OAM-TS Header w/ ToD
 *  										"	(ETPP, DM, Dn, Trp) - ToD// bypass to Out I/F
 * 7:0                         Offset from end of System Headers to where to stamp/update.
 * 
 * parsing pkt_dm_down packet:
 * 0x800c000d|180000000016|000000010203000000ffffff8100|000a8902|A02f002000000000000003e70000
 *    ITMH       OAM-TS            ETH.                  VLAN       OAM
 * ITMH - send packet to port 13
 * OAM-TS - packet type DM, stamping offset - 16
 * 
 * parsing pkt_lm_down packet:
 * 0x800c000d|080000000516|000000ffffff0000000000018100|000a8902|802b000c000000000000000000000000
 *    ITMH       OAM-TS            ETH.                  VLAN       OAM
 * ITMH - send packet to port 13
 * OAM-TS - packet type LM, stamping offset - 16, counter id - 5
 *  
 * parsing pkt_lm_up packet: no headers
 * 0x000000010203000000ffffff8100|000a8902|A02b000c0000000000000000000000000
 *            ETH.                  VLAN       OAM
 */
int inject_dm_and_lm_packets(int unit) {
  bcm_error_t rv;
  char *pkt_dm_down, *pkt_dm_up, *pkt_lm_down, *pkt_lm_up;
  int ptch_down[2];
  int ptch_up[2];

  pkt_dm_down = "0x800c000d180000000016000000010203000000ffffff8100000a8902A02f002000000000000003e70000"; /*DM down*/
  pkt_lm_down = "0x800c000d080000000516000000ffffff0000000000018100000a8902802b000c000000000000000000000000"; /*LM down*/
  ptch_down[0] = 0; /* The next header is ITMH */
  ptch_down[1] = 0; /* in port is port_1 */

  pkt_dm_up = "0x000000010203000000ffffff810000058902A02f0020000000000000000000000000"; /*DM up*/
  pkt_lm_up = "0x000000010203000000ffffff810000058902A02b000c0000000000000000000000000"; /*LM up*/
  ptch_up[0] = 240; /* Next header should be deduced from the SSP; Opaque-PT-Attributes = 7 */
  ptch_up[1] = port_2; /* SSP = port_2 */

  rv = tx_packet_via_bcm(unit, pkt_lm_down, ptch_down, 2);
  rv = tx_packet_via_bcm(unit, pkt_dm_down, ptch_down, 2);
  rv = tx_packet_via_bcm(unit, pkt_lm_up, ptch_up, 2);
  rv = tx_packet_via_bcm(unit, pkt_dm_up, ptch_up, 2);
  return rv;
}


/*****************************************************************************/
/*                                        OAM  MIP BASIC EXAMPLE                                                                        */
/*****************************************************************************/
int create_mip1(int unit, int md_level_mip, bcm_oam_group_t group, bcm_gport_t gport) {
    int rv;
    rv  = create_mip(unit, md_level_mip, group, gport, mac_mep_2, 0);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/**
 * Equivalent to oam_run_with_defaults, but only creates a MIP.
 * MIP will be created on port2.
 * 
 * @param unit 
 * @param port1 
 * @param port2 
 * @param port3 
 * 
 * @return int 
 */
int oam_mip_only_run_with_defaults (int unit, int port1, int port2, int port3) {
  bcm_error_t rv;

  single_vlan_tag = 1;

  port_1 = port1;
  port_2 = port2;
  port_3 = port3;
  
  rv = oam_initialize_mip_settings(unit);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  rv = oam_create_mip_with_defaults(unit);
   if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }

  return 0;
}


/**
 * Initialize settings and global variables for creating a MIP.
 * 
 * @param unit 
 * 
 * @return int 
 */
int oam_initialize_mip_settings(int unit) {
  bcm_error_t rv;

  int md_level_1 = 4;
  int md_level_2 = 5;
  int md_level_3 = 2;
  int lm_counter_base_id_1  = 5;
  int lm_counter_base_id_2  = 6;

  rv = create_vswitch_and_mac_entries(unit);
  if (rv != BCM_E_NONE) {
	  printf("(%s) \n",bcm_errmsg(rv));
	  return rv;
  }



  return rv;
}



/*****************************************************************************/
/*                                        OAM API WRAPPER FUNCTIONS AND EXAMPLES                                         */
/*****************************************************************************/

/**
 * Set a default MEP on the egress. Applies to Jericho B and 
 * above. 
 * Note that by default all outLIFs use oam-outlif-profile 0. In
 * this example configure a specific outlif to have profile 
 * oam-outlif-profile and then define a MEP on 
 * oam-outlif-profile 1. 
 * 
 * @author sinai (01/11/2015)
 * 
 * @param unit 
 * @param port1 
 * @param port2 
 * @param port3 
 * 
 * @return int 
 */
int default_up_ep_example(int unit, int port1, int port2, int port3) {
    bcm_error_t rv;
    single_vlan_tag = 1;
    int oam_egress_profile = 2;
    bcm_oam_endpoint_info_t default_info;

    port_1 = port1;
    port_2 = port2;
    port_3 = port3;

    rv = oam__device_type_get(unit, &device_type);
    if (rv < 0) {
        printf("Error checking whether the device is arad+.\n");
        print rv;
        return rv;
    }

    if (device_type < device_type_jericho_b) {
        print "Cint available for Jericho/QMX B0 and above";
        return 234;
    }

    rv = create_vswitch_and_mac_entries(unit);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    /* Tie gport2, gport1 to outlif profile 2.*/
    rv = bcm_port_control_set(unit, gport1, bcmPortControlOamDefaultProfileEgress, oam_egress_profile);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_port_control_set(unit, gport2, bcmPortControlOamDefaultProfileEgress, oam_egress_profile);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    
    /*  Now Configure an endpoint on oam-outLIF profile 2.*/
    printf("Add default up mep (EGRESS0)\n");
    bcm_oam_endpoint_info_t_init(&default_info);
    default_info.id = BCM_OAM_ENDPOINT_DEFAULT_EGRESS2;
    default_info.flags |= BCM_OAM_ENDPOINT_WITH_ID | BCM_OAM_ENDPOINT_UP_FACING;
    default_info.level = 5;
    default_info.timestamp_format = get_oam_timestamp_format(unit);


    rv = bcm_oam_endpoint_create(unit, &default_info);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
	
	return 0;

}

/**
 * Function creates an accelerated endpoint.
 *  group_info_short_ma  must be initialized before this
 *  function is called.
 *  Likewise the function create_vswitch_and_mac_entries() must
 *  also  be called before this function.
 *  
 * @author sinai (26/11/2013)
 * 
 * @param unit 
 * @param is_up - direction of the endpoint.
 * @param ts_format - one of bcmOAMTimestampFormatNTP or 
 *                  bcmOAMTimestampFormatIEEE1588v1
 * 
 * @return int 
 */
int create_acc_endpoint(int unit , int is_up, bcm_oam_timestamp_format_t ts_format) {
    bcm_oam_endpoint_info_t  acc_endpoint;
    int counter_base_id;
    int rv;

    /** Down*/
   bcm_oam_endpoint_info_t_init(&acc_endpoint);

  /*TX*/
  acc_endpoint.type = bcmOAMEndpointTypeEthernet;
  acc_endpoint.group = group_info_short_ma.id;
  acc_endpoint.opcode_flags |= BCM_OAM_OPCODE_CCM_IN_HW;
  acc_endpoint.outer_tpid = 0x8100;
  acc_endpoint.timestamp_format = ts_format;


  if (is_up) {
        /*TX*/
      rv = set_counter_source_and_engines(unit,&counter_base_id,port_2);
      BCM_IF_ERROR_RETURN(rv);
      acc_endpoint.lm_counter_base_id = counter_base_id;

      acc_endpoint.level = 5;
      acc_endpoint.name = 123;
      acc_endpoint.flags |= BCM_OAM_ENDPOINT_UP_FACING;

      acc_endpoint.vlan = 5;
      acc_endpoint.pkt_pri = 0 + (2<<1); /* dei(1bit) + (pcp(3bit) << 1)*/
      acc_endpoint.outer_tpid = 0x8100;
      acc_endpoint.int_pri = 3 + (1<<2);
      /* The MAC address that the CCM packets are sent with*/
      src_mac_mep_2[5] = port_2;
      sal_memcpy(acc_endpoint.src_mac_address, src_mac_mep_2, 6);
      /*RX*/
      acc_endpoint.gport = gport2;
      sal_memcpy(acc_endpoint.dst_mac_address, mac_mep_2, 6);
      acc_endpoint.tx_gport = BCM_GPORT_INVALID;
  } else { /** Down*/
      rv = set_counter_source_and_engines(unit,&counter_base_id,port_1);
      BCM_IF_ERROR_RETURN(rv);
      acc_endpoint.lm_counter_base_id = counter_base_id;

      acc_endpoint.level = 2;
      BCM_GPORT_SYSTEM_PORT_ID_SET(acc_endpoint.tx_gport, port_1);
      acc_endpoint.name = 456;
      acc_endpoint.ccm_period = BCM_OAM_ENDPOINT_CCM_PERIOD_100MS;
      acc_endpoint.vlan = 10;
      acc_endpoint.pkt_pri = 0 + (1<<1); /* dei(1bit) + (pcp(3bit) << 1)*/
      acc_endpoint.int_pri = 1 + (3<<2);
      /* The MAC address that the CCM packets are sent with*/
      sal_memcpy(acc_endpoint.src_mac_address, src_mac_mep_3, 6);

      /*RX*/
      acc_endpoint.gport = gport1;
      sal_memcpy(acc_endpoint.dst_mac_address, mac_mep_3, 6);
  }

  rv = bcm_oam_endpoint_create(unit, &acc_endpoint);
  if (rv != BCM_E_NONE) {
      printf("(%s) \n", bcm_errmsg(rv));
      return rv;
  }
  if (is_up) { 
      acc_ep_up.id = acc_endpoint.id;
  } else { /* Down*/
      acc_ep_down.id = acc_endpoint.id;
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
    int trap_code, trap_code2;
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
            /* This trap may already been created */
            if (gTrapUp4AlreadyCreated) {
                rv = bcm_rx_trap_type_get(unit, 0, trap_type, &trap_code);
            } else {
                gTrapUp4AlreadyCreated = 1;
                rv = bcm_rx_trap_type_create(unit, 0, trap_type, &trap_code);
            }
            if (rv != BCM_E_NONE) {
                printf("(%s) \n", bcm_errmsg(rv));
                return rv;
            }
        } else {
            rv = bcm_rx_trap_type_create(unit, 0, trap_type, &trap_code);
            if (rv != BCM_E_NONE) {
                printf("(%s) \n", bcm_errmsg(rv));
                return rv;
            }
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

        /* If MIP, two traps shoud be used. One for each direction. Setting the egress-trap, ingress-trap was set above */
        if (endpoint_info.flags & BCM_OAM_ENDPOINT_INTERMEDIATE) {
            trap_type = bcmRxTrapOamUpMEP4;

            /* This trap may already been created */
            if (gTrapUp4AlreadyCreated) {
                rv = bcm_rx_trap_type_get(unit, 0, trap_type, &trap_code2);
            } else {
                gTrapUp4AlreadyCreated = 1;
                rv = bcm_rx_trap_type_create(unit, 0, trap_type, &trap_code2);
            }
            if (rv != BCM_E_NONE) {
                printf("(%s) \n", bcm_errmsg(rv));
                return rv;
            }

            rv = bcm_rx_trap_set(unit, trap_code2, trap_config);
            if (rv != BCM_E_NONE) {
                printf("(%s) \n", bcm_errmsg(rv));
                return rv;
            }

            printf("Trap set, trap_code2 = %d \n", trap_code2);
            BCM_GPORT_TRAP_SET(action.destination2, trap_code2, 7, 0);
        }
    }

    BCM_OAM_OPCODE_SET(action, opcode);
    BCM_OAM_ACTION_SET(action, action_type);
    if (count_enable_on_oam_action) {
        BCM_OAM_ACTION_SET(action, bcmOAMActionCountEnable);
    }

    rv = bcm_oam_endpoint_action_set(unit, endpoint_id, &action);
    printf( "Action set created\n");

    return rv;
}

/**
 * Action profile for packets that causes trap bcmRxTrapOamPassive 
 *
 * This is an example script to assign action for passive demultiplexing.
 * Calling this API sets the global behavior of passive demultiplexing. 
 * For drop action, use inParam port = BCM_GPORT_TYPE_BLACK_HOLE
 *
 * @author Neeraj (06/08/2016)
 * 
 * @param unit 
 * @param port - Destination
 * @return int 
 */
int configure_oam_passive_level_trap(int unit, bcm_port_t port) {

    int rv;
    int trap_code;
    bcm_rx_trap_config_t  oam_trap_config;
    int trap_port;

    /* First, get the trap code used for OAM traps*/
    rv = bcm_rx_trap_type_get(unit,0/* flags */ , bcmRxTrapOamPassive, &trap_code);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }

    /* Now the trap config. */
    rv = bcm_rx_trap_get(unit,trap_code, &oam_trap_config);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }
    oam_trap_config.flags |= BCM_RX_TRAP_TRAP;

    /* Update the destination*/
    oam_trap_config.dest_port = port;

    /* Now update the trap config with the new destination*/
    rv = bcm_rx_trap_set(unit,trap_code, oam_trap_config);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }
    return 0;
}
/**
 * Example shows how to trap CCMs to the CPU for MIPs.
 * (By default CCMs are forwarded). 
 *  
 * @author sinai (15/03/2016)
 * 
 * @param unit 
 * @param endpoint_id -assumed to be a MIP
 * 
 * @return int 
 */
int oam_action_set_for_mips_ccm(int unit, int endpoint_id) {
    bcm_error_t rv;
    bcm_oam_endpoint_action_t action;
    int trap_code, trap_code2;
    bcm_rx_trap_config_t trap_config;
    bcm_rx_trap_t trap_type;
    bcm_oam_endpoint_info_t endpoint_info;

    bcm_oam_endpoint_action_t_init(&action);

  /* First, get the trap code used for OAM traps*/
    rv = bcm_rx_trap_type_get(unit,0/* flags */ ,bcmRxTrapBfdOamDownMEP, &trap_code);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }

    /* Now the same for Up direction*/
    rv = bcm_rx_trap_type_get(unit,0 ,bcmRxTrapOamUpMEP, &trap_code2);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }

    /* Now set the parameters.*/
    BCM_GPORT_TRAP_SET(action.destination2, trap_code2, 7, 0);
    BCM_GPORT_TRAP_SET(action.destination, trap_code, 7, 0);

    BCM_OAM_OPCODE_SET(action, bcmOamOpcodeCCM);
    BCM_OAM_ACTION_SET(action, bcmOAMActionMcFwd);
    rv = bcm_oam_endpoint_action_set(unit, endpoint_id, &action);
    printf( "Action set created\n");

    return rv;
}

/**
 * Snoop OAM packets by endpoint and opcode to given 
 * destination. 
 * Note: Only applies for enbpoints. For MIPs two traps with two 
 * consecutive trap codes must be allocated, one for the ingress 
 * and another for the egress. 
 * 
 * 
 * @param unit 
 * @param dest_snoop_port 
 * @param endpoint_id 
 * @param action_type 
 * @param opcode 
 * 
 * @return int 
 */
int dpp_oam_change_mep_destination_to_snoop(int unit, int dest_snoop_port, int endpoint_id, bcm_oam_action_type_t action_type, int opcode) {
    bcm_error_t rv;
    bcm_rx_trap_config_t trap_config_snoop;
    bcm_rx_snoop_config_t snoop_config_cpu;
    int snoop_cmnd;
    int trap_code;
    bcm_gport_t gport;

    rv = bcm_rx_snoop_create(unit, 0, &snoop_cmnd);
    if (rv != BCM_E_NONE) {
       printf("(%s) \n",bcm_errmsg(rv));
       return rv;
    }

    bcm_rx_snoop_config_t_init(&snoop_config_cpu);
    snoop_config_cpu.flags = (BCM_RX_SNOOP_UPDATE_DEST | BCM_RX_SNOOP_UPDATE_PRIO);
    snoop_config_cpu.dest_port = dest_snoop_port;
    snoop_config_cpu.size = -1;
    snoop_config_cpu.probability = 100000;

    rv =  bcm_rx_snoop_set(unit, snoop_cmnd, &snoop_config_cpu);
    if (rv != BCM_E_NONE) {
       printf("(%s) \n",bcm_errmsg(rv));
       return rv;
    }

    rv =  bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_code);
    if (rv != BCM_E_NONE) {
       printf("(%s) \n",bcm_errmsg(rv));
       return rv;
    }
    printf("Trap created trap_code=%d \n",trap_code);

    bcm_rx_trap_config_t_init(&trap_config_snoop);
    trap_config_snoop.flags = 0;
    trap_config_snoop.trap_strength = 0;
    trap_config_snoop.snoop_cmnd = snoop_cmnd;

    rv = bcm_rx_trap_set(unit, trap_code, trap_config_snoop);
    if (rv != BCM_E_NONE) {
       printf("(%s) \n",bcm_errmsg(rv));
       return rv;
    }

    BCM_GPORT_TRAP_SET(gport, trap_code, 7, 3); /*Taken from default values*/

    bcm_oam_endpoint_action_t action;
    bcm_oam_endpoint_action_t_init(&action);
    action.destination = gport;
    BCM_OAM_OPCODE_SET(action, opcode); /*1-CCM*/
    BCM_OAM_ACTION_SET(action, action_type);  /*bcmOAMActionMcFwd,bcmOAMActionUcFwd*/
    rv =  bcm_oam_endpoint_action_set(unit, endpoint_id, &action);
    if (rv != BCM_E_NONE) {
       printf("(%s) \n",bcm_errmsg(rv));
       return rv;
   }

   return rv;
}
/**
 * By default SLM and SLR share the same opcode profile.
 * The following example sets new SLR opcode profile and creates an action.
 * 
 * OAM_endpoint_opcode_map calling sequence example.
 *  1)Change LMR opcode profile to 15.
 *  2)Update SLR opcode profile to LMR former opcode profile.
 *  3)Set action to SLR opcode.
 * 
 * @param unit 
 * @param dest_port - Must be BCM_GPORT_INVALID for actions requiring
 *             invalid gport. otherwise, trap.
 * @param endpoint_id.
 * 
 * @return int 
 */
int oam_slr_op_map_action_set(int unit,int dest_port, int endpoint_id){
    bcm_error_t rv;
    int op_profile;
    int new_op_profile = 0xf;
    int trap_code;
    bcm_oam_endpoint_info_t endpoint_info;
    bcm_rx_trap_config_t trap_config;
    bcm_rx_trap_t trap_type;
    bcm_oam_endpoint_action_t action;

    /* Get LMR opcode profile */
    rv = bcm_oam_opcode_map_get(unit,42/*LMR*/,&op_profile);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Change LMR opcode profile to 15 */
    rv = bcm_oam_opcode_map_set(unit,42/*LMR*/,new_op_profile);
    if (rv != BCM_E_NONE) {
       printf("(%s) \n",bcm_errmsg(rv));
       return rv;
    }

    /* Map SLR opcode to LMR former opcode profile */
    rv = bcm_oam_opcode_map_set(unit,54/*SLR*/,op_profile);
    if (rv != BCM_E_NONE) {
       printf("(%s) \n",bcm_errmsg(rv));
       return rv;
    }

    /* Set new action to SLR opcode */
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
            /* This trap may already been created */
            if (gTrapUp4AlreadyCreated) {
                rv = bcm_rx_trap_type_get(unit, 0, trap_type, &trap_code);
            } else {
                gTrapUp4AlreadyCreated = 1;
                rv = bcm_rx_trap_type_create(unit, 0, trap_type, &trap_code);
            }
            if (rv != BCM_E_NONE) {
                printf("(%s) \n", bcm_errmsg(rv));
                return rv;
            }
        } else {
            rv = bcm_rx_trap_type_create(unit, 0, trap_type, &trap_code);
            if (rv != BCM_E_NONE) {
                printf("(%s) \n", bcm_errmsg(rv));
                return rv;
            }
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

    BCM_OAM_OPCODE_SET(action, 54 /* SLR */);
    BCM_OAM_ACTION_SET(action, bcmOAMActionUcFwd);
    BCM_OAM_ACTION_SET(action, bcmOAMActionCountEnable);

    rv = bcm_oam_endpoint_action_set(unit, endpoint_id, &action);
    printf( "Action set created\n");

    return rv;
}

/**
 * Count SLM packets for a given endpoint. 
 * This API does the following: 
 * 1)For the LIF on which the given endpoint resides data 
 * packets will not be counted 
 * 2)SLM, SLR, LMM, and LMR packets will be counted for ALL 
 * LIFs. 
 * 
 * @param unit 
 * @param endpoint_id 
 * 
 * @return int 
 */
int slm_set(int unit, int endpoint_id)  {
    bcm_error_t rv;
    bcm_oam_endpoint_action_t action;

    bcm_oam_endpoint_action_t_init(&action);
    action.destination = BCM_GPORT_INVALID;
    BCM_OAM_OPCODE_SET(action, 55); /*SLM*/
    BCM_OAM_OPCODE_SET(action, 54); /*SLR*/

    /* Note that as long is the user requires to count SLM packets the action
       bcmOAMActionSLMEnable Should be added for all subsequnt calls to bcm_oam_endpoint_action_set*/
    BCM_OAM_ACTION_SET(action, bcmOAMActionSLMEnable);
    BCM_OAM_ACTION_SET(action, bcmOAMActionCountEnable);
    rv =  bcm_oam_endpoint_action_set(unit, endpoint_id, &action);
    if (rv != BCM_E_NONE) {
       printf("(%s) \n",bcm_errmsg(rv));
       return rv;
   }

   return rv;
}



/* Arad/Arad+ hard coded values */
int snoop_up_mip = 2;
int snoop_down_mip = 1;


/**
 * Function changes the MIP snoop destination for both 
 * directions (up and down). 
 * Function assumes the soc property "egress_snooping_advanced" 
 * is set to 1. 
 * 
 * @author sinai (22/01/2014)
 * 
 * @param unit 
 * @param dest_snoop_port - destination for both directions.
 * @param mip_endpoint_id 
 * @param action_type 
 * @param opcode 
 * 
 * @return int 
 */
int mip_egress_snooping_advanced(int unit, int dest_snoop_port, int mip_endpoint_id, bcm_oam_action_type_t action_type, int opcode) {
    bcm_error_t rv;
    bcm_rx_trap_config_t trap_config_snoop;
    bcm_rx_snoop_config_t snoop_config_cpu;
    int snoop_cmnd = snoop_down_mip; /* Arad/Arad+ only - higher devices need to create a snoop command */
    int trap_code;
    bcm_gport_t gport;

    bcm_rx_snoop_config_t_init(&snoop_config_cpu);
    snoop_config_cpu.flags = (BCM_RX_SNOOP_UPDATE_DEST | BCM_RX_SNOOP_UPDATE_PRIO);
    snoop_config_cpu.dest_port = dest_snoop_port;
    snoop_config_cpu.size = -1;
    snoop_config_cpu.probability = 100000;

    rv = bcm_rx_snoop_set(unit, snoop_cmnd, &snoop_config_cpu);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    /* create ONLY the trap for the down direction */
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_code);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }
    printf("Trap created trap_code=%d \n", trap_code);

    bcm_rx_trap_config_t_init(&trap_config_snoop);
    trap_config_snoop.flags = 0;
    trap_config_snoop.trap_strength = 0; 
    trap_config_snoop.snoop_cmnd = snoop_cmnd;

    rv = bcm_rx_trap_set(unit, trap_code, trap_config_snoop);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    BCM_GPORT_TRAP_SET(gport, trap_code, 7, 3); /*Taken from default values*/

    bcm_oam_endpoint_action_t action;
    bcm_oam_endpoint_action_t_init(&action);
    action.destination = gport;
    /* For Arad+ Advanced egress snooping destination2 must not be set
       even though the endpoint is a MIP. Anyway both directions will
       be configured. This is a special case. */
    action.destination2 = BCM_GPORT_INVALID;
    BCM_OAM_OPCODE_SET(action, opcode);
    BCM_OAM_ACTION_SET(action, action_type); /*bcmOAMActionMcFwd,bcmOAMActionUcFwd*/
    rv = bcm_oam_endpoint_action_set(unit, mip_endpoint_id, &action);
    if (rv != BCM_E_NONE) {
       printf("(%s) \n",bcm_errmsg(rv));
       return rv;
   }

   return rv;
}

/**
 * Jericho and above only. 
 * Function changes the MIP to snoop OAM PDU types (by opcode) 
 * for both directions (up and down). The function also 
 * demonstrate re-configuring the snoop action by setting a 
 * different snoop port if dest_snoop_port!=BCM_GPORT_INVALID 
 *
 * @param unit
 * @param dest_snoop_port
 * @param mip_endpoint_id
 * @param action_type
 * @param opcode
 *
 * @return int
 */
int mip_set_snoop_by_opcode(int unit, int dest_snoop_port, int mip_endpoint_id, bcm_oam_action_type_t action_type, int opcode) {
    bcm_error_t rv;
    bcm_oam_endpoint_action_t action;
    int trap_id, trap_id_up;

    /* Get trap-code for oam snooped packets */
    rv = bcm_rx_trap_type_get(unit,0,bcmRxTrapSnoopOamPacket,&trap_id);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    /* Now the same for Up direction */
    rv = bcm_rx_trap_type_get(unit,0 ,bcmRxTrapOamUpMEP3, &trap_id_up);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }

    if (dest_snoop_port != BCM_GPORT_INVALID) {
        /* If the dest_snoop_port is valid, reconfigure the snoop command */

        bcm_rx_trap_config_t trap_config, trap_config_up;
        bcm_rx_snoop_config_t snoop_config;
        int snoop_cmd;

        /* First, retreive the trap configuration using the trap-code */
        rv = bcm_rx_trap_get(unit, trap_id, &trap_config);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }

        /* The trap is connected to a snoop command. Get the snoop command ID from the trap configuration */
        snoop_cmd = trap_config.snoop_cmnd;

        /* Retrieve the snoop command configuration */
        rv = bcm_rx_snoop_get(unit, snoop_cmd, &snoop_config);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }

        /* Modify the neccesary field */
        snoop_config.dest_port = dest_snoop_port;

        /* Write the modified configuration back */
        rv = bcm_rx_snoop_set(unit, snoop_cmd, &snoop_config);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }

        /* Using the same trap configuration for Up MEP as Down MEP*/
        /* QAX devices uses trapping instead of snooping for egress snooping.
           Configuring accordingly if that's the case. */
        /* Retreive the trap configuration using the up trap-code */
        rv = bcm_rx_trap_get(unit, trap_id_up, &trap_config_up);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }

        if (!trap_config_up.snoop_cmnd) {
            /* No snoop command on trap. Device is QAX. Using trap instead of snoop. */
            trap_config_up.dest_port = dest_snoop_port;

            rv = bcm_rx_trap_set(unit, trap_id_up, trap_config_up);
        } else {
            rv = bcm_rx_trap_set(unit, trap_id_up, trap_config);
        }
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
        }
    }

    /* Set an action of trapping packets with the specified opcode */
    bcm_oam_endpoint_action_t_init(&action);
    BCM_GPORT_TRAP_SET(action.destination, trap_id, 7, 3);
    BCM_GPORT_TRAP_SET(action.destination2, trap_id_up, 7, 3);
    BCM_OAM_OPCODE_SET(action, opcode);
    BCM_OAM_ACTION_SET(action, action_type); /*bcmOAMActionMcFwd,bcmOAMActionUcFwd*/

    /* Set the action for the MIP */
    rv = bcm_oam_endpoint_action_set(unit, mip_endpoint_id, &action);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
        return rv;
    }

    return rv;
}


/**
 * Sets trapping of packets to specific 1 or 2 MEPs or MIPs to a gport
 * and snooping those packets to a 2nd gport.
 */
int trap_snoop_set(int unit, bcm_oam_endpoint_t endpoint1, bcm_oam_endpoint_t endpoint2,
                   bcm_gport_t trap_gport, bcm_gport_t snoop_gport) {
    bcm_error_t rv;
    bcm_oam_endpoint_action_t action;
    bcm_gport_t gport, gport_up;
    bcm_rx_trap_config_t trap_config_snoop;
    bcm_rx_snoop_config_t snoop_config_cpu;
    int snoop_cmnd;
    int trap_code, trap_code_up;
    bcm_oam_endpoint_info_t endpoint_info1;
    bcm_oam_endpoint_info_t endpoint_info2;
    int is_ep1_up, is_ep2_up;

    bcm_oam_endpoint_info_t_init(&endpoint_info1);
    bcm_oam_endpoint_info_t_init(&endpoint_info2);

    /* Get the endpoints to check their direction*/
    if (endpoint1 != -1) {
        rv = bcm_oam_endpoint_get(unit, endpoint1, &endpoint_info1);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        is_ep1_up = (endpoint_info1.flags & BCM_OAM_ENDPOINT_UP_FACING) > 0;
    }
    if (endpoint2 != -1) {
        rv = bcm_oam_endpoint_get(unit, endpoint2, &endpoint_info2);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        is_ep2_up = (endpoint_info2.flags & BCM_OAM_ENDPOINT_UP_FACING) > 0;
    }

    /* First allocate a new snoop command */
    rv = bcm_rx_snoop_create(unit, 0/*flags*/, &snoop_cmnd);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    print snoop_cmnd;

    /* Setup the new snoop */
    bcm_rx_snoop_config_t_init(&snoop_config_cpu);
    snoop_config_cpu.flags = (BCM_RX_SNOOP_UPDATE_DEST | BCM_RX_SNOOP_UPDATE_PRIO);
    snoop_config_cpu.dest_port = snoop_gport;
    snoop_config_cpu.size = -1;
    snoop_config_cpu.probability = 100000;

    rv =  bcm_rx_snoop_set(unit, snoop_cmnd, &snoop_config_cpu);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Now create a new trap code, each direction require different kind of trap
       Up MWP requires an FTMH trap (to prevent duplicated set of system headers)
       while Down MEP can use a user defined trap */
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_code);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapOamUpMEP2, &trap_code_up);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
    }

    /* Setup the new trap */
    bcm_rx_trap_config_t_init(&trap_config_snoop);
    trap_config_snoop.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_UPDATE_FORWARDING_HEADER | BCM_RX_TRAP_TRAP;
    trap_config_snoop.snoop_strength = 3;
    trap_config_snoop.snoop_cmnd = snoop_cmnd; /* Connect the snoop command to the trap */
    trap_config_snoop.forwarding_header = bcmRxTrapForwardingHeaderOamBfdPdu;
    trap_config_snoop.dest_port = trap_gport;

    rv = bcm_rx_trap_set(unit, trap_code, &trap_config_snoop);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_rx_trap_set(unit, trap_code_up, &trap_config_snoop);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Trap representation as a GPort */
    BCM_GPORT_TRAP_SET(gport, trap_code, 7, 3); /*Taken from default values*/
    BCM_GPORT_TRAP_SET(gport_up, trap_code_up, 7, 3); /*Taken from default values*/

    /* Setup an action that uses the new trap+snoop for MC CCMs */
    bcm_oam_endpoint_action_t_init(&action);
    BCM_OAM_OPCODE_SET(action, 1); /*1-CCM*/
    BCM_OAM_ACTION_SET(action, bcmOAMActionMcFwd);  /*CCMs are multicast*/

    /* Set the action for the required endpoint(s) */
    if (endpoint1 > -1) {
        action.destination = is_ep1_up ? gport_up : gport;
        rv = bcm_oam_endpoint_action_set(unit, endpoint1, &action);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    /* Set the action for the required endpoint(s) */
    if (endpoint2 > -1) {
        action.destination = is_ep2_up ? gport_up : gport;
        rv = bcm_oam_endpoint_action_set(unit, endpoint2, &action);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    return rv;
}
/**
 * Sets trapping of packets to specific 1 or 2 MEPs or MIPs to a gport
 * and snooping those packets to a 2nd gport. Overwrite
 * FHEI values using stamping in the IPT with trap_id and
 * trap_qualifier per snoop-command.
 */
int trap_snoop_fhei_stamping_set(int unit,
                                 bcm_oam_endpoint_t endpoint1,
                                 bcm_oam_endpoint_t endpoint2,
                                 bcm_gport_t trap_gport,
                                 bcm_gport_t snoop_gport,
                                 int stamp_trap_id,
                                 int stamp_trap_qualifier) {
    bcm_error_t rv;
    bcm_oam_endpoint_action_t action;
    bcm_gport_t gport, gport_up;
    bcm_rx_trap_config_t trap_config_snoop;
    bcm_mirror_destination_t snoop_dest;
    int snoop_cmnd;
    int trap_code, trap_code_up;
    bcm_oam_endpoint_info_t endpoint_info1;
    bcm_oam_endpoint_info_t endpoint_info2;
    int is_ep1_up, is_ep2_up;

    bcm_oam_endpoint_info_t_init(&endpoint_info1);
    bcm_oam_endpoint_info_t_init(&endpoint_info2);

    /* Get the endpoints to check their direction*/
    if (endpoint1 != -1) {
        rv = bcm_oam_endpoint_get(unit, endpoint1, &endpoint_info1);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        is_ep1_up = (endpoint_info1.flags & BCM_OAM_ENDPOINT_UP_FACING) > 0;
    }
    if (endpoint2 != -1) {
        rv = bcm_oam_endpoint_get(unit, endpoint2, &endpoint_info2);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n", bcm_errmsg(rv));
            return rv;
        }
        is_ep2_up = (endpoint_info2.flags & BCM_OAM_ENDPOINT_UP_FACING) > 0;
    }

    rv = oam__device_type_get(unit, &device_type);
    if (rv < 0) {
        printf("Error checking whether the device is arad+.\n");
        print rv;
        return rv;
    }

    if (device_type < device_type_jericho) {
        printf("FHEI IPT stamping is supported for jericho only.\n");
        return BCM_E_UNAVAIL;
    }

    /* Now create a new trap code, each direction require different kind of trap
       Up MWP requires an FTMH trap (to prevent duplicated set of system headers)
       while Down MEP can use a user defined trap */
    rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapUserDefine, &trap_code);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_rx_trap_type_get(unit, 0, bcmRxTrapOamUpMEP2, &trap_code_up);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n", bcm_errmsg(rv));
    }

    bcm_mirror_destination_t_init(&snoop_dest);
    snoop_dest.flags = BCM_MIRROR_DEST_IS_SNOOP;
    snoop_dest.packet_control_updates.valid = BCM_MIRROR_PKT_HEADER_UPDATE_PRIO | BCM_MIRROR_PKT_HEADER_UPDATE_FABRIC_HEADER_EDITING;
    snoop_dest.gport = snoop_gport;
    snoop_dest.packet_control_updates.prio = 0;
    snoop_dest.packet_copy_size = 0;
    snoop_dest.sample_rate_dividend = 100000;
    snoop_dest.sample_rate_divisor  = 100000;
    snoop_dest.packet_control_updates.header_fabric.internal_valid = 1;
    snoop_dest.packet_control_updates.header_fabric.internal.trap_id = stamp_trap_id;
    snoop_dest.packet_control_updates.header_fabric.internal.trap_qualifier = stamp_trap_qualifier;
    rv = bcm_mirror_destination_create(unit, &snoop_dest);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    snoop_cmnd = BCM_GPORT_MIRROR_GET(snoop_dest.mirror_dest_id);

    /* Setup the new trap */
    bcm_rx_trap_config_t_init(&trap_config_snoop);
    trap_config_snoop.flags = BCM_RX_TRAP_UPDATE_DEST | BCM_RX_TRAP_UPDATE_FORWARDING_HEADER | BCM_RX_TRAP_TRAP;
    trap_config_snoop.snoop_strength = 3;
    trap_config_snoop.snoop_cmnd = snoop_cmnd; /* Connect the snoop command to the trap */
    trap_config_snoop.forwarding_header = bcmRxTrapForwardingHeaderOamBfdPdu;
    trap_config_snoop.dest_port = trap_gport;
    rv = bcm_rx_trap_set(unit, trap_code, &trap_config_snoop);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    rv = bcm_rx_trap_set(unit, trap_code_up, &trap_config_snoop);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }

    /* Trap representation as a GPort */
    BCM_GPORT_TRAP_SET(gport, trap_code, 7, 3); /*Taken from default values*/
    BCM_GPORT_TRAP_SET(gport_up, trap_code_up, 7, 3); /*Taken from default values*/

    /* Setup an action that uses the new trap+snoop for MC CCMs */
    bcm_oam_endpoint_action_t_init(&action);
    BCM_OAM_OPCODE_SET(action, 1); /*1-CCM*/
    BCM_OAM_ACTION_SET(action, bcmOAMActionMcFwd);  /*CCMs are multicast*/

    /* Set the action for the required endpoint(s) */
    if (endpoint1 > -1) {
        action.destination = is_ep1_up ? gport_up : gport;
        rv = bcm_oam_endpoint_action_set(unit, endpoint1, &action);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    /* Set the action for the required endpoint(s) */
    if (endpoint2 > -1) {
        action.destination = is_ep2_up ? gport_up : gport;
        rv = bcm_oam_endpoint_action_set(unit, endpoint2, &action);
        if (rv != BCM_E_NONE) {
            printf("(%s) \n",bcm_errmsg(rv));
            return rv;
        }
    }

    return rv;
}
/**
 *  Configure snooping to CPU packets that are trapped to the
 *  OAMP.
 *  Use this function to set the snooping of CCMs trapped by up
 *  to 2 accelerated endpoints.
 *
 *  @param endpoint1
 *      MEP ID or -1 to ignore
 *  @param endpoint2
 *      MEP ID or -1 to ignore
 */
int acc_snoop_set(int unit, bcm_oam_endpoint_t endpoint1, bcm_oam_endpoint_t endpoint2) {
    bcm_error_t rv;
    bcm_gport_t oamp_gport;

    rv = oamp_gport_get(unit, &oamp_gport);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    print oamp_gport;

    rv = trap_snoop_set(unit, endpoint1, endpoint2, oamp_gport, 0/*snoop to CPU port*/);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }

    return rv;
}

/**
 *  Configure snooping to CPU packets that are trapped to the
 *  OAMP.
 *  Use this function to set the snooping of CCMs trapped by up
 *  to 2 accelerated endpoints.
 *  And overwrite these values using stamping in the
 *  IPT with trap_id and trap_qualifier per snoop-command.
 *  
 *  @param endpoint1
 *      MEP ID or -1 to ignore
 *  @param endpoint2
 *      MEP ID or -1 to ignore
 */
int acc_snoop_fhei_stamping_set(int unit, bcm_oam_endpoint_t endpoint1, bcm_oam_endpoint_t endpoint2, int stamp_trap_id, int stamp_trap_qualifier) {
    bcm_error_t rv;
    bcm_gport_t oamp_gport;

    rv = oam__device_type_get(unit, &device_type);
    if (rv < 0) {
        printf("Error checking whether the device is arad+.\n");
        print rv;
        return rv;
    }

    if (device_type < device_type_jericho) {
        printf("FHEI IPT stamping is supported for jericho only.\n");
        return BCM_E_UNAVAIL;
    }

    rv = oamp_gport_get(unit, &oamp_gport);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
        return rv;
    }
    print oamp_gport;

    rv = trap_snoop_fhei_stamping_set(unit, endpoint1, endpoint2, oamp_gport, 0/*snoop to CPU port*/, stamp_trap_id, stamp_trap_qualifier);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }
    return rv;
}

/**
 * Function sets the OAM default trapping for all Up MEPs and 
 * Down MEPs. 
 * Also displays an example of getting the trap number for OAM 
 * traps. 
 * 
 * 
 * @param unit 
 * @param oam_trapping_port 
 * 
 * @return int 
 */
int configure_oam_default_trap(int unit, int oam_trapping_port) {
    int rv;
    int trap_code;
    bcm_rx_trap_config_t  oam_trap_config;
    int trap_port;

    /* First, get the trap code used for OAM traps*/
    rv = bcm_rx_trap_type_get(unit,0/* flags */ ,bcmRxTrapBfdOamDownMEP, &trap_code);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }

    /* Now the trap config. */
    rv = bcm_rx_trap_get(unit,trap_code, &oam_trap_config);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }

    oam_trap_config.flags |= BCM_RX_TRAP_TRAP;

    /* Update the destination*/
    BCM_GPORT_LOCAL_SET(oam_trap_config.dest_port, oam_trapping_port);

    /* Now update the trap config with the new destination*/
    rv = bcm_rx_trap_set(unit,trap_code, oam_trap_config);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }

    /* Now the same for Up direction*/
    rv = bcm_rx_trap_type_get(unit,0 ,bcmRxTrapOamUpMEP, &trap_code);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }

    /* Using the same trap configuration for Up MEP as Down MEP*/
    rv = bcm_rx_trap_set(unit,trap_code, oam_trap_config);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }

    /* Note that it may be advisable to do the same with the traps
       bcmRxTrapOamLevel
       bcmRxTrapOamPassive
       */

    return rv;
}

/**
 * Function that sets up MEP snooping destination for QAX and
 * above devices. This snooping is quite 'fake'. a trap is being
 * usen instead of an actual snoop command. The reason for this
 * is the inability to keep original outlif on the snooped
 * packet system headers.
 *
 * @param unit
 *
 * @return int
 */
int configure_default_up_snooping_destination(int unit, int new_trapping_port) {
    int rv;
    int trap_code;
    bcm_rx_trap_config_t oam_trap_config;
    int cpu_port = 0;

    /* This is meant for QAX and above only */
    rv = oam__device_type_get(unit, &device_type);
    BCM_IF_ERROR_RETURN(rv);
    if (device_type < device_type_qax) {
        return BCM_E_UNAVAIL;
    }

    /* First, get the trap code dedicated for up MEP snooping. */
    rv = bcm_rx_trap_type_get(unit,0 ,bcmRxTrapOamUpMEP3, &trap_code);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }

    /* Now the trap config. */
    bcm_rx_trap_config_t_init(&oam_trap_config);
    rv = bcm_rx_trap_get(unit, trap_code, &oam_trap_config);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }

    /* Update the destination. */
    BCM_GPORT_LOCAL_SET(oam_trap_config.dest_port, new_trapping_port);

    /* Update the trap config with the new destination. */
    rv = bcm_rx_trap_set(unit, trap_code, oam_trap_config);
    if (rv != BCM_E_NONE) {
        printf("(%s) \n",bcm_errmsg(rv));
    }

    return rv;
}
