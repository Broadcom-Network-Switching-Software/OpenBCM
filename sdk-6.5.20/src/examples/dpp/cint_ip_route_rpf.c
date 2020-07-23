/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


/* ********* 
  Globals/Aux Variables
 */

/*
 * run:
 
cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
cint ../../../../src/examples/dpp/cint_multi_device_utils.c 
cint ../../../../src/examples/dpp/utility/cint_utils_l3.c 
cint ../../../../src/examples/dpp/cint_ip_route.c 
cint ../../../../src/examples/dpp/cint_ip_route_rpf.c 
cint ../../../../src/examples/dpp/cint_multi_device_utils.c 
cint ../../../../src/examples/dpp/cint_mim_mp.c
 
from L to R: 
 - for Loose: 
 * packet to send: 
 *  - in syetem port = in_sysport
 *  - vlan 20.
 *  - DA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02}
 *  - DIP = 0x7fffff03 (120.255.255.3)
 *  - SIP = 130.0.0.0 (not found)
 * Expected:
 *  --> packet discarded, loose check FAIL
 *
 * packet to send: 
 *  - in system port = in_sysport
 *  - vlan 20.
 *  - DA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02}
 *  - DIP = 0x7fffff03 (120.255.255.3)
 *  - SIP = 120.255.250.110 (found, with incorrect RIF)
 * Expected:
 *  --> packet forwarded (as check is loose)
 *
 * packet to send: 
 *  - in system port = in_sysport
 *  - vlan 20.
 *  - DA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02}
 *  - DIP = 0x7fffff03 (120.255.255.3)
 *  - SIP = 127.255.250.110 (found, with correct RIF)
 * Expected:
 *  --> packet forwarded (as check is loose)
 *
 *
 - for Strict: 
 * packet to send: 
 *  - in system port = in_sysport
 *  - vlan 20.
 *  - DA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02}
 *  - DIP = 0x7fffff03 (120.255.255.3)
 *  - SIP = 130.0.0.0 (not found)
 * Expected:
 *  --> packet discarded, strict check FAIL
 *
 * packet to send: 
 *  - in system port = in_sysport
 *  - vlan 20.
 *  - DA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02}
 *  - DIP = 0x7fffff03 (120.255.255.3)
 *  - SIP = 120.255.250.110 (found, with incorrect RIF)
 * Expected:
 *  --> packet discarded (as check is strict)
 *
 * packet to send: 
 *  - in system port = in_sysport
 *  - vlan 20.
 *  - DA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02}
 *  - DIP = 0x7fffff03 (120.255.255.3)
 *  - SIP = 127.255.250.110 (found, with correct RIF)
 * Expected:
 *  --> packet forwarded (as found with correct RIF)
 */
int MAX_NUM_UNIT = 8;
int L3_rpf_uc_loose = 0;
int L3_rpf_uc_strict = 0;
int L3_rpf_mc_explicit = 0;
int L3_rpf_mc_sip = 0;
int L3_rpf_fec_cascade = 0;
int L3_rpf_default_fec_enable = 1;
int outlif_to_count = 0;
int outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE;
int global_fec[MAX_NUM_UNIT][2] = {{0x0}};
/********** 
  functions
 */
int turn_off_default_fec(int unit){
    L3_rpf_default_fec_enable = 0;
}

/******* Run example ******/

int example_rpf(int *units_ids, int nof_units, int in_sysport, int out_sysport, bcm_l3_ingress_urpf_mode_t urpf_mode) {
    int rv;
    rv = example_rpf_vrf(units_ids, nof_units, in_sysport, out_sysport, urpf_mode, 0 /*vrf*/);
    if (rv != BCM_E_NONE) {
        printf("Error, example_rpf \n");
    }

    return rv;
}

/*
 * packet will be routed from in_sysport to out-sysport 
 * HOST: 
 * packet to send: 
 *  - in port = in_sysport
 *  - vlan 10.
 *  - DA = 00:00:00:02:00:01 {0x00, 0x00, 0x00, 0x02, 0x00, 0x01}
 *  - DIP = 0x7fffff03 (127.255.255.03) or 0x7ffffa00 (127.255.250.0) - 0x7ffffaff (127.255.250.255)
 * expected: 
 *  - out system port = out_sysport
 *  - vlan 20.
 *  - DA = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1e}
 *  - SA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02}
 *  TTL decremented
 *  
 * Route: 
 * packet to send: 
 *  - in port = in_sysport
 *  - vlan = 20.
 *  - DA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x01}
 *  - DIP = 0x78ffff03 (120.255.255.3) or 0x78fffa00 (120.255.250.0) - 0x78fffaff (120.255.250.255)
 * expected: 
 *  - out system port = out_sysport
 *  - vlan = 10.
 *  - DA = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}
 *  - SA = {0x00, 0x00, 0x00, 0x02, 0x00, 0x01}
 *  TTL decremented
 int units_ids[2];
 int nof_units = 1;
 int in_sysport = 13;
 int out_sysport = 13;
 int urpf_mode = bcmL3IngressUrpfStrict;
 print example_rpf(units_ids, nof_units, in_sysport, out_sysport, urpf_mode);
 */
int example_rpf_vrf(int *units_ids, int nof_units, int in_sysport, int out_sysport, bcm_l3_ingress_urpf_mode_t urpf_mode, int vrf){
    int rv, i;
    int unit, flags;
    int fec[MAX_NUM_UNIT][2] = {{0x0}};
    int itf_flags = BCM_L3_RPF;
    int egr_flags = 0;

    int host_l = 0x78ffff03; /*120.255.255.3*/
    int subnet_l = 0x78fffa00; /*120.255.250.0 /24 */
    int subnet_l_mask =  0xffffff00;
    int vlan_l = 10;
    bcm_mac_t mac_address_l  = {0x00, 0x00, 0x00, 0x02, 0x00, 0x01};  /* my-MAC */
    bcm_mac_t mac_address_next_hop_l  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    int ing_intf_l; 

    int host_r = 0x7fffff03; /*127.255.255.3*/
    int subnet_r = 0x7ffffa00; /*127.255.250.0 /24 */
    int subnet_r_mask =  0xffffff00;
    int vlan_r = 20;
    bcm_mac_t mac_address_r  = {0x00, 0x00, 0x00, 0x02, 0x00, 0x02};  /* my-MAC */
    bcm_mac_t mac_address_next_hop_r  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1e}; /* next_hop_mac1 */
    int ing_intf_r; 

    int encap_id[2]={0x2000, 0x2002};

    create_l3_intf_s intf;

    /* set global definition defined in cint_ip_route.c */
    /* this way all UC-rpf checks is done according to this config */
    L3_uc_rpf_mode = urpf_mode;

    if (L3_rpf_fec_cascade) {
        egr_flags |= BCM_L3_CASCADED;
        printf("FEC's will be cascaded\n");
    }

    rv = l3_ip_rpf_config_traps(units_ids, nof_units);
    if (rv != BCM_E_NONE) {
        printf("Error, l3_ip_rpf_config_traps \n");
    }

    /* create l3 interface 1 (L) */
    /*** create ingress router interface ***/
    units_array_make_local_first(units_ids,nof_units,in_sysport);
    flags = itf_flags;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        rv = vlan__open_vlan_per_mc(unit, vlan_l, 0x1);  
        if (rv != BCM_E_NONE) {
            printf("Error, open_vlan=%d, in unit %d \n", vlan_l, unit);
        }
        rv = bcm_vlan_gport_add(unit, vlan_l, in_sysport, 0);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("fail add port(0x%08x) to vlan(%d)\n", in_sysport, vlan_l);
          return rv;
        }

        intf.vsi = vlan_l;
        intf.my_global_mac = mac_address_l;
        intf.my_lsb_mac = mac_address_l;
        intf.vrf_valid = 1;
        intf.vrf = vrf;
        intf.rpf_valid = 1;
        intf.urpf_mode = urpf_mode;
        intf.flags = flags;

        rv = l3__intf_rif__create(unit, &intf);
        ing_intf_l = intf.rif;        
        if (rv != BCM_E_NONE) {
            printf("Error, l3__intf_rif__create\n");
        }
    }

    /* create l3 interface 2 (R) */
    /*** create ingress router interface ***/
    units_array_make_local_first(units_ids,nof_units,out_sysport);
    flags = itf_flags;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        rv = vlan__open_vlan_per_mc(unit, vlan_r, 0x1);  
        if (rv != BCM_E_NONE) {
            printf("Error, open_vlan=%d, in unit %d \n", vlan_r, unit);
        }
        rv = bcm_vlan_gport_add(unit, vlan_r, out_sysport, 0);
        if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
            printf("fail add port(0x%08x) to vlan(%d)\n", out_sysport, vlan_r);
          return rv;
        }

        intf.vsi = vlan_r;
        intf.my_global_mac = mac_address_r;
        intf.my_lsb_mac = mac_address_r;
        intf.rpf_valid = 1;
        intf.urpf_mode = urpf_mode;
        intf.flags = flags;

        rv = l3__intf_rif__create(unit, &intf);
        ing_intf_r = intf.rif;        
        if (rv != BCM_E_NONE) {
            printf("Error, l3__intf_rif__create\n");
        }
    }

    /* create egress FEC toward (R) */
    /*** create egress object 1 ***/
    /* We're now configuring egress port for out_sysport. Local unit for out_sysport is already first. */
    flags = egr_flags;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = create_l3_egress_with_counting_profile(unit,flags,out_sysport,vlan_r,ing_intf_r,mac_address_next_hop_r, &fec[unit][0], &encap_id[0], outlif_counting_profile);

        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, on port=%d, \n", out_sysport);
        }
        global_fec[unit][0] = fec[unit][0];
        if(verbose >= 1) {
            printf("On unit %d: created FEC-id =0x%08x, \n", unit, fec[unit][0]);
            printf("next hop mac at encap-id %08x, \n", encap_id[0]);
        }
    }

    /* create egress FEC toward (L) */
    /*** create egress object 2 ***/
    units_array_make_local_first(units_ids, nof_units, in_sysport);
    flags = egr_flags;
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = create_l3_egress_with_counting_profile(unit,flags,in_sysport,vlan_l,ing_intf_l,mac_address_next_hop_l, &fec[unit][1], &encap_id[1],BCM_STAT_LIF_COUNTING_PROFILE_NONE);

        if (rv != BCM_E_NONE) {
            printf("Error, create egress object, on port=%d, \n", out_sysport);
        }
        global_fec[unit][1] = fec[unit][1];
        if(verbose >= 1) {
            printf("On unit %d: created FEC-id =0x%08x, \n", unit, fec[unit][1]);
            printf("next hop mac at encap-id %08x, \n", encap_id[1]);
        }
    }


    /* IPuc add routes/host to Routing table: from L to R*/
    /*** add host point to FEC2 ***/
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = add_host(unit, host_r, vrf, fec[unit][0]);
        if (rv != BCM_E_NONE) {
            printf("Error, add host = 0x%08x, \n", host_r);
        }
        if(verbose >= 1) {
            print_host("add host ", host_r,vrf);
            printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, \n", fec[unit][0], ing_intf_r, out_sysport);
        }
    }

    /*** add route point to FEC2 ***/
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = add_route(unit, subnet_r, subnet_r_mask , vrf, fec[unit][0]);
        if (rv != BCM_E_NONE) {
            printf("Error, add subnet = 0x%08x/0x%08x, \n", subnet_r,subnet_r_mask);
        }
        if(verbose >= 1) {
            print_route("add subnet ", subnet_r,subnet_r_mask,vrf);
            printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, \n", fec[unit][0], ing_intf_r, out_sysport);
        }
    }

   /* IPuc add routes/host to Routing table: from R to L*/
    /*** add host point to FEC2 ***/
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = add_host(unit, host_l, vrf, fec[unit][1]);
        if (rv != BCM_E_NONE) {
            printf("Error, add host = 0x%08x, \n", host_l);
        }
        if(verbose >= 1) {
            print_host("add host ", host_l,vrf);
            printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, \n", fec[unit][1], ing_intf_l, in_sysport);
        }
    }

    /*** add route point to FEC2 ***/
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = add_route(unit, subnet_l, subnet_l_mask , vrf, fec[unit][1]);
        if (rv != BCM_E_NONE) {
            printf("Error, add subnet = 0x%08x/0x%08x, \n", subnet_l,subnet_l_mask);
        }
        if(verbose >= 1) {
            print_route("add subnet ", subnet_l,subnet_l_mask,vrf);
            printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, \n", fec[unit][1], ing_intf_l, in_sysport);
        }

        if (L3_rpf_default_fec_enable) {
            /* Add default route for bcmSwitchL3UrpfDefaultRoute */
            rv = add_route_rpf(unit, 0, 0 , vrf, fec[unit][1]);
            if (rv != BCM_E_NONE) {
                printf("Error, add subnet = 0x%08x/0x%08x, \n", subnet_l,subnet_l_mask);
            }
            if(verbose >= 1) {
                print_route("add subnet ", subnet_l,subnet_l_mask,vrf);
	            printf("---> egress-object=0x%08x, outRIF=0x%08x, port=%d, \n", fec[unit][1], ing_intf_l, in_sysport);
	        }
		}
    }
    return rv;
}

int
 l3_ip_rpf_config_traps(int *units_ids, int nof_units) {
 	int rv = BCM_E_NONE, i;
	bcm_rx_trap_config_t config;
	int flags = 0;
    int unit;


    /* 
     * RPF types:
     */

    /* bcmRxTrapUcLooseRpfFail,            : Forwarding Code is IPv4 UC and RPF FEC Pointer Valid is not set. */
    /* bcmRxTrapUcStrictRpfFail,           : UC-RPF-Mode is 'Strict' and OutRIF is not equal to packet InRIF . */
    /* bcmRxTrapMcExplicitRpfFail,         : RPF-Entry. Expected-InRIF is not equal to packet InRIF Relevant when FEC-Entry. MC-RPF-Mode is 'Explicit'. */
    /* bcmRxTrapMcUseSipRpfFail,           : Out-RIF is not equal to In-RIF when MC-RPF-Mode is 'Use-SIP-WITH-ECMP' Note: . */

    /* set uc strict to drop packet */
    if(L3_rpf_uc_strict == 0){
        for(i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];
            rv = bcm_rx_trap_type_create(unit,BCM_RX_TRAP_WITH_ID,bcmRxTrapUcStrictRpfFail,&L3_rpf_uc_strict);
            if (rv != BCM_E_NONE) {
                printf("Error, in trap create, trap bcmRxTrapUcStrictRpfFail \n");
                return rv;
            }
        }
    }
    /* set uc loose to drop packet */
    if(L3_rpf_uc_loose == 0){
        for(i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];
            rv = bcm_rx_trap_type_create(unit,BCM_RX_TRAP_WITH_ID,bcmRxTrapUcLooseRpfFail,&L3_rpf_uc_loose);
            if (rv != BCM_E_NONE) {
                printf("Error, in trap create, trap bcmRxTrapUcLooseRpfFail \n");
                return rv;
            }
        }
    }

    /* set mc SIP to drop packet */
    if(L3_rpf_mc_sip == 0){
        for(i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];
            rv = bcm_rx_trap_type_create(unit,BCM_RX_TRAP_WITH_ID,bcmRxTrapMcUseSipRpfFail,&L3_rpf_mc_sip);
            if (rv != BCM_E_NONE) {
                printf("Error, in trap create, trap bcmRxTrapMcUseSipRpfFail \n");
                return rv;
            }
        }
    }

    /* set mc explict to drop packet */
    if(L3_rpf_mc_explicit == 0){
        for(i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];
            rv = bcm_rx_trap_type_create(unit,BCM_RX_TRAP_WITH_ID,bcmRxTrapMcExplicitRpfFail,&L3_rpf_mc_explicit);
            if (rv != BCM_E_NONE) {
                printf("Error, in trap create, trap bcmRxTrapMcExplicitRpfFail \n");
                return rv;
            }
        }
    }

  /*change dest port for trap */
  bcm_rx_trap_config_t_init(&config);
  config.flags |= (BCM_RX_TRAP_UPDATE_DEST); 
  config.trap_strength = 5;
  config.dest_port=BCM_GPORT_BLACK_HOLE; 
  for(i = 0 ; i < nof_units ; i++){
      unit = units_ids[i];
      rv = bcm_rx_trap_set(unit,L3_rpf_uc_loose,&config);
      if (rv != BCM_E_NONE) {
          printf("Error, in trap set L3_rpf_uc_loose \n");
          return rv;
      }
  }

  for(i = 0 ; i < nof_units ; i++){
      unit = units_ids[i];
      rv = bcm_rx_trap_set(unit,L3_rpf_uc_strict,&config);
      if (rv != BCM_E_NONE) {
          printf("Error, in trap set L3_rpf_uc_strict\n");
          return rv;
      }
  }

  for(i = 0 ; i < nof_units ; i++){
      unit = units_ids[i];
      rv = bcm_rx_trap_set(unit,L3_rpf_mc_explicit,&config);
      if (rv != BCM_E_NONE) {
          printf("Error, in trap set L3_rpf_mc_explicit\n");
          return rv;
      }
  }

  for(i = 0 ; i < nof_units ; i++){
      unit = units_ids[i];
      rv = bcm_rx_trap_set(unit,L3_rpf_mc_sip,&config);
      if (rv != BCM_E_NONE) {
          printf("Error, in trap set L3_rpf_mc_sip\n");
          return rv;
      }
  }

  return rv;
}



/*
* l3_ip_rpf_config_mc_traps_test configuration
* ipv4-mc rpf trapped packets sent to recycle port. 
* to use this make sure you set the right soc properties 
* set the port <port> as recycle port.
*  	parameters:
*   port - the chosen recycle port  
*/
int
 l3_ip_rpf_config_mc_traps_test(int uint, int port) {
 	int rv = BCM_E_NONE;
	bcm_rx_trap_config_t config;
	int flags = 0;
    int unit;


    /* 
     * RPF types:
     */

    /* bcmRxTrapMcExplicitRpfFail,         : RPF-Entry. Expected-InRIF is not equal to packet InRIF Relevant when FEC-Entry. MC-RPF-Mode is 'Explicit'. */
    /* bcmRxTrapMcUseSipRpfFail,           : Out-RIF is not equal to In-RIF when MC-RPF-Mode is 'Use-SIP-WITH-ECMP' Note: . */


	bcm_rx_trap_config_t_init(&config);
	BCM_GPORT_SYSTEM_PORT_ID_SET(config.dest_port, port);
	print config.dest_port;
	config.flags = BCM_RX_TRAP_UPDATE_DEST;
	config.trap_strength = 5;


	rv = bcm_rx_trap_set(unit, L3_rpf_mc_explicit, config);
      if (rv != BCM_E_NONE) {
          printf("Error, in trap set bcm_rx_trap_set\n");
          return rv;
      }
	rv = bcm_rx_trap_set(unit, L3_rpf_mc_sip, config);
      if (rv != BCM_E_NONE) {
          printf("Error, in trap set L3_rpf_McUseSip\n");
          return rv;
      }

  return rv;
}


/*
 * create l3 egress interface.
 */
int
create_l3_egress_with_counting_profile(int unit, uint32 flags, int port, int vlan, int ingress_intf, bcm_mac_t next_hop_mac_addr, int *intf, int *encap_id, int counting_profile)
{
  bcm_l3_egress_t l3eg;
  int rc;
  int mod = 0;
  bcm_if_t l3egid;
  int test_failover_id = 0;
  int test_failover_intf_id = 0;

  bcm_l3_egress_t_init(l3eg);
  l3eg.intf = ingress_intf;
  sal_memcpy(l3eg.mac_addr, &next_hop_mac_addr, 6);
  l3eg.vlan = vlan;
  l3eg.port = port;
  l3eg.failover_id = test_failover_id;
  l3eg.failover_if_id = test_failover_intf_id;
  l3eg.encap_id = *encap_id;
  l3egid = *intf;
  l3eg.counting_profile = counting_profile;
  
  rc = bcm_l3_egress_create(unit, flags, &l3eg, &l3egid);

  /* convert the global lif into GPORT
   * there is no GPORT type L3, but we can use type TUNNEL */
  if (counting_profile != BCM_STAT_LIF_COUNTING_PROFILE_NONE)
  {
      BCM_L3_ITF_LIF_TO_GPORT_TUNNEL (outlif_to_count, l3eg.encap_id);
  }

  if (BCM_FAILURE(rc)) {
    printf("BCM FAIL %d: %s\n", rc, bcm_errmsg(rc));
    return CMD_FAIL;
  }
  printf("L3 egress created, MAC 0x%02x:0x%02x:0x%02x:0x%02x:0x%02x:0x%02x on VLAN 0x%x\n",
    l3eg.mac_addr[0],
    l3eg.mac_addr[1],
    l3eg.mac_addr[2],
    l3eg.mac_addr[3],
    l3eg.mac_addr[4],
    l3eg.mac_addr[5], vlan);

  *encap_id = l3eg.encap_id;
  *intf = l3egid;

  return rc;
}

int outlif_counting_profile_set(int profile)
{
    outlif_counting_profile = profile;
    printf("outlif_counting_profile_set function: outlif_counting_profile=%d \n",outlif_counting_profile);
    return BCM_E_NONE;
}


/*
* l3_ip_rpf_config_mc_traps_test configuration
* ipv4-mc rpf trapped packets sent to recycle port and snooped to CPU. 
* to use this make sure you set the right soc properties 
* set the port <port> as recycle port. 
* set the port <snoop_port> as CPU port 
*   parameters:
*   port - the chosen recycle port  
*/
int
l3_ip_rpf_config_mc_traps_and_snoop_test(int uint, int port, int snoop_port) {
    int rv = BCM_E_NONE;
    bcm_rx_trap_config_t config;
    int flags = 0;
    int unit;
 
    int snoop_trap_id;
    int snoop_command;                        /* Snoop command */
    bcm_field_entry_t snoop_ent;
 
    int result;
    int auxRes;
    int flags2 = 0;                            /* Do not specify any ID for the snoop command / trap */
    int trap_dest_strength = 0;               /* No influence on the destination update */
    int trap_snoop_strength = 3;              /* Strongest snoop strength for this trap */
    bcm_rx_snoop_config_t snoop_config;       /* Snoop attributes */
    bcm_rx_trap_config_t trap_config;
    bcm_gport_t snoop_trap_gport_id;
 
    int snp_g_id;
 
    /* 
    * Create a Snoop Command 
    * The snoop command defines the attribute of the Snoop 
    */
    printf("============---------------- Creating snoop and trap ---------------================\n");
    result = bcm_rx_snoop_create(unit, flags2, &snoop_command); /* Receive a snoop command handle */
    if (result != BCM_E_NONE) {
        printf("Error in bcm_rx_snoop_create\n");
        return result;
    }
    printf("\t Snoop command %x \n", snoop_command);
    /*
      Specify the snoop attributes
      Here, we set the destination and the probability to 100% 
      Snoop the whole packet */
 
    /* Initialize the structure */
    bcm_rx_snoop_config_t_init(&snoop_config);
    snoop_config.flags = (BCM_RX_SNOOP_UPDATE_DEST | BCM_RX_SNOOP_REPLACE);
 
    /* Set the Mirror destination to go to the Destination local port*/
    BCM_GPORT_LOCAL_SET(snoop_config.dest_port, snoop_port);
    snoop_config.size = -1;               /* Full packet snooping */
    snoop_config.probability = 100000;     /* 100000 is 100.000% */
 
    /*Set snoop configuration*/
    result = bcm_rx_snoop_set(unit, snoop_command, &snoop_config);
    if (result != BCM_E_NONE) {
        printf("Error in bcm_rx_snoop_set\n");
        return result;
    }
 
    /* 
     * RPF types:
     */
 
    /* bcmRxTrapMcExplicitRpfFail,         : RPF-Entry. Expected-InRIF is not equal to packet InRIF Relevant when FEC-Entry. MC-RPF-Mode is 'Explicit'. */
    /* bcmRxTrapMcUseSipRpfFail,           : Out-RIF is not equal to In-RIF when MC-RPF-Mode is 'Use-SIP-WITH-ECMP' Note: . */
 
    bcm_rx_trap_config_t_init(&config);
    BCM_GPORT_SYSTEM_PORT_ID_SET(config.dest_port, port);
    print config.dest_port;
    config.flags = BCM_RX_TRAP_UPDATE_DEST|BCM_RX_TRAP_TRAP;
    config.trap_strength = 5;
    config.snoop_cmnd = snoop_command; /* Snoop any frame matched by this trap */
    config.snoop_strength = trap_snoop_strength;
 
 
    rv = bcm_rx_trap_set(unit, L3_rpf_mc_explicit, config);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap set bcm_rx_trap_set\n");
        return rv;
    }
    rv = bcm_rx_trap_set(unit, L3_rpf_mc_sip, config);
    if (rv != BCM_E_NONE) {
        printf("Error, in trap set L3_rpf_McUseSip\n");
        return rv;
    }
 
    return rv;
}

/* for fec global_fec[0] switch the urpf_mode between its 2 modes */
int invert_fec_urpf_mode(int unit){

    int rv = BCM_E_NONE;
    bcm_l3_egress_t egr;
    egr.flags2 = BCM_L3_FLAGS2_INGRESS_URPF_MODE;

    rv = bcm_l3_egress_get(unit,global_fec[unit][0],&egr);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l3_egress_get\n");
        return rv;
    }

    egr.flags2 |= BCM_L3_FLAGS2_INGRESS_URPF_MODE;
    egr.urpf_mode = (egr.urpf_mode == bcmL3IngressUrpfStrict) ? bcmL3IngressUrpfLoose : bcmL3IngressUrpfStrict; /* change value */

    rv = bcm_l3_egress_create(unit,BCM_L3_WITH_ID | BCM_L3_REPLACE,&egr,&global_fec[unit][0]);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_l3_egress_create(update)\n");
        return rv;
    }

    return rv;
}


