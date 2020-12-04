/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/


/* 
 * This file consist FCoE examples, service function and general information about FCoE.
 * 
 * How to run:
 *
 *   Soc properties:
 *   bcm886xx_fcoe_switch_mode=3
 *   ipv4_num_vrfs=512
 *   bcm886xx_fcoe_num_vrf=254: this soc propery has to be smaller than 256(it is a uint8) and smaller than ipv4_num_vrfs-1!!!
 *
 *   cint;
 *   cint_reset();
 *   exit;
 *   cint ../../../../src/examples/dpp/cint_multi_device_utils.c
 *   cint ../../../../src/examples/dpp/utility/cint_utils_l3.c
 *   cint ../../../../src/examples/dpp/cint_ip_route.c 
 *   cint ../../../../src/examples/dpp/cint_mact.c  
 *   cint ../../../../src/examples/dpp/cint_trap_mgmt.c 
 *   cint ../../../../src/examples/dpp/cint_field_fiber_channel.c  
 *   cint ../../../../src/examples/dpp/cint_fcoe_route.c
 *   cint
 *
 *   Examples
 *   --------
 *   (1)run transit: print fcoe_transit_example(unit, src_port, dst_port);
 *   (2)run fcf: print fcoe_fcf_example(unit,src_port, dst_port, entry_id, insertion_flags, mask);
 *   (3)run fcf zoning: print fcf_zoning_example(units_ids, 1, src_port, dst_port);
 * 
 *   NPV
 *   ---
 *   npv is a switch mode, in this mode all ports cam be configured to be n_port or np_port.
 *   zoning is disabled in this mode.
 *   n_port perform source routing
 *   np_port perform destination routing.
 *   for npv switch configurations see fcoe_fcf_npv_configure()
 * 
 *   Zoning
 *   ------
 *   Zoning allow to filter unregisterd src hosts.
 *   to enable zone filtering run fcoe_zone_filter_enable()
 *   to add zone entries (enries that will not be filterd) run fcoe_fcf_zone_entry_add()
 * 
 *   Notes
 *   -----
 * 
 *   FCoE packets that will be sent when the soc property bcm886xx_fcoe_switch_mode is not present or equal to 0 will be dropped!!!
 *   (thier beavior will be unprodictable because the flp program that will be choose for them is the program is the default program)
 */ 

/* 
 
route add insertion flags table 
------------------------------- 
 
API Flags:	                    DB	                        constraints 
----------------------------------------------------------------------------------------------
---------------------------------------------------------------------------------------------- 
BCM_FCOE_DOMAIN_ROUTE           LPM	                    prefix 0-7 --> add D_ID[23:X] to LPM
---------------------------------------------------------------------------------------------- 
BCM_FCOE_HOST_ROUTE             LEM                     If Prefix 24 --> D_ID to LEM
                                                        If prefix == 8 --> then add D_ID[23:16] to LEM
---------------------------------------------------------------------------------------------- 
BCM_FCOE_LOCAL_ADDRESS|         SEM: add to 8 msb.	    Prefix 24 (no mask)
BCM_FCOE_HOST_ROUTE             LEM: 24 bits.	
----------------------------------------------------------------------------------------------
BCM_FCOE_LOCAL_ADDRESS          SEM: my domain	        Prefix = 8.                                     
---------------------------------------------------------------------------------------------- 
BCM_FCOE_SOURCE_ROUTE|          LEM                     If Prefix 24 --> S_ID to LEM (source routing logical database)
BCM_FCOE_HOST_ROUTE              
---------------------------------------------------------------------------------------------- 
BCM_FCOE_SOURCE_ROUTE|          LPM                     prefix 0-7 --> add S_ID[23:X] to LPM 
BCM_FCOE_DOMAIN_ROUTE 
----------------------------------------------------------------------------------------------  
*/	


/* debug prints */
int verbose = 1;

/* global parameters */
int pmf_group_id_for_zoning = 11;
/************************************
 * FCoE Utilities
 */

void print_fc_addr(uint32 host)
{
  int a,b,c,d;

  a = (host >> 24) & 0xff;
  b = (host >> 16) & 0xff;
  c = (host >> 8 ) & 0xff;
  d = host & 0xff;
  printf("%d.%d.%d.%d", a,b,c,d);
}

void print_route(char *type, uint32 d_id, uint32 mask, int vft)
{
    printf("%s  vrf:%d   ", type, vft);
    print_fc_addr(d_id);
    printf("/");
    print_fc_addr(mask);
}

/* recieve array of bcm_fcoe_route_t size at lease 4 entries */
void fcoe_init_routes(bcm_fcoe_route_t *fc_routes){

    /* remote domain in LPM */
    fc_routes[0].flags = BCM_FCOE_DOMAIN_ROUTE;     fc_routes[0].nport_id_mask = 0xFE0000; fc_routes[0].nport_id = 0xBA0000;
    /* remote in LEM */
    fc_routes[1].flags = BCM_FCOE_HOST_ROUTE;       fc_routes[1].nport_id_mask = 0xFF0000; fc_routes[1].nport_id = 0xBB0000;

    /* local <SEM + LEM> */
    fc_routes[2].flags = BCM_FCOE_LOCAL_ADDRESS;    fc_routes[2].nport_id_mask = 0xFF0000; fc_routes[2].nport_id = 0xAA0000;
    fc_routes[3].flags = BCM_FCOE_HOST_ROUTE;       fc_routes[3].nport_id_mask = 0xFFFFFF; fc_routes[3].nport_id = 0xAAAAAA;

}


int delete_fcf_route(int unit, uint32 d_id, uint32 d_id_mask, int vft, int intf, int flags) {
  int rc;
  bcm_fcoe_route_t l3rt;
 
  bcm_fcoe_route_t_init(l3rt);
 
  l3rt.vrf = vft;
  l3rt.nport_id_mask = d_id_mask;
  l3rt.nport_id = d_id;
  l3rt.intf = intf;
  l3rt.flags = flags;
 
  rc = bcm_fcoe_route_delete(unit, l3rt);
 
  return rc;
}

/* print find_route(unit,0x00ffff00,0x00ffffff,0,BCM_FCOE_HOST_ROUTE);*/
int find_route(int unit, uint32 d_id, uint32 d_id_mask, int vft, int flags) {
  int rc;
  bcm_fcoe_route_t l3rt;

  bcm_fcoe_route_t_init(l3rt);

  l3rt.vrf = vft;
  l3rt.nport_id_mask = d_id_mask;
  l3rt.nport_id = d_id;
  l3rt.intf = 0;
  l3rt.flags = flags;

  rc = bcm_fcoe_route_find(unit, l3rt);
  if (rc != BCM_E_NOT_FOUND) {
    printf ("not found \n");
  }
  else if (rc != BCM_E_NONE) {
    printf ("bcm_fcoe_route_find failed: %d \n", rc);
  }

  if(verbose >= 1) {
      print l3rt;
  }

  return rc;
}

/************************************************************************************************************************************ 
 * FCoE service functions.
 *
 * How to run:
 * -----------
 *   
 *  to configure trap for FIP packets that ingressing from src_port:
 *      fcoe_fip_trap_configuration(unit, src_port);
 * 
 *  to enable trap for security filtering (SA != source Id): (for aradB0 and above)
 *      fcoe_sa_mismatch_trap_cofiguration(unit);
 * 
 *  to insert authorized mac entries to mact:
 *      fcoe_authorized_mact_entry_add(unit, mac, vlan, dest_gport);
 * 
 *  to configure switch to be FCF, packets with DA == my_mac will be zonned and fwrd according to FCF rules:
 *      fcoe_fcf_switch_create(int unit, bcm_mac_t my_mac, int src_port, int dst_port, int in_vlan, int out_vlan);
 * 
 *  to configure switch to be NPV, and a specific port to be n_port :
 *      fcoe_fcf_npv_configure();
 */ 

/*
 * Add Route <vrf, addr, mask> --> intf
 * - d_di: D_ID address 32 bit value
 * - d_id_mask 1: to consider 0: to ingore, for example for /24 mask = 0xffffff00
 * - vft: max value depeneds on soc-property in arad.soc
 * - intf: egress object created using create_l3_egress
 * - flags see above
 */
int add_fcf_route(int unit, uint32 d_id, uint32 d_id_mask, int vft, int intf, int flags) {
  int rc;
  bcm_fcoe_route_t l3rt;

  bcm_fcoe_route_t_init(l3rt);

  l3rt.vrf = vft;
  l3rt.nport_id_mask = d_id_mask;
  l3rt.nport_id = d_id;
  l3rt.intf = intf;
  l3rt.flags = flags;

  rc = bcm_fcoe_route_add(unit, l3rt);
  if (rc != BCM_E_NONE) {
    printf ("bcm_fcoe_route_add failed: %d \n", rc);
  }

  if(verbose >= 1) {
      print_route("add route", d_id, d_id_mask,vft);
      printf("---> egress-object=0x%08x, \n", intf);
  }

  return rc;
}


/* FCF:
   - fix FCF forwarding header offset to be right after ethernet before SoF
 */
int fcf_workaround(int *units_ids, int nof_units){
    int i;
    int rv = BCM_E_NONE;
    int unit;
    int trap_id;
    int fix_gport;

    rc = bcm_rx_trap_type_get(unit,0, bcmRxTrapFcoeFcfPacket,&trap_id);
    if (rc != BCM_E_NONE) 
    {
       printf ("bcm_rx_trap_type_get failed: %x \n", rc);
	}
    BCM_GPORT_TRAP_SET(fix_gport,trap_id,4,0);

    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];

        /* pmf: to fix the offset of forwarding header*/
        rv = fcf_setup(unit,0,fix_gport,10);
        if (rv != BCM_E_NONE) {
            printf("Error, FCF header setting unit %d \n", unit);
        }        
    }
    return rv;
}

/* 
   Traps setting
   int units_ids[1] = {0};
   fcf_traps_set(units_ids,1,1);
   drop = 1, not forwarding fail or zoning fail packet will be dropped
 */
int fcf_traps_set(int *units_ids, int nof_units,uint8 drop){
    int i;
    int rv = BCM_E_NONE;
    int unit;
    int dest_gport;
    int strength;
    
    if (drop) {
        dest_gport = BCM_GPORT_BLACK_HOLE;
        strength = 7;
    }
    else{
        strength = 0;
        dest_gport = BCM_GPORT_LOCAL_CPU; /* doesn't matter */
    }

    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = trap_type_set(unit,bcmRxTrapFcoeZoneCheckFail,strength,dest_gport);
        if (rv != BCM_E_NONE) {
            printf("Error, FCF setting unit %d \n", in_sysport, unit);
        }
    }

    return rv;
}
    
        
/* this is a standard L3 interface creation */    
int fcoe_fcf_switch_create(int unit, bcm_mac_t my_mac, int src_port, int dst_port, int in_vlan, int out_vlan, int flags, int* ing_intf_in, int* ing_intf_out)
{
    int rv;
    
    /*** create ingress router interface ***/
    rv = vlan__open_vlan_per_mc(unit, in_vlan, 0x1);  
    if (rv != BCM_E_NONE) {
    	printf("Error, open_vlan=%d, in unit %d \n", in_vlan, unit);
    }
    rv = bcm_vlan_gport_add(unit, in_vlan, src_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
    	printf("fail add port(0x%08x) to vlan(%d)\n", src_port, in_vlan);
      return rv;
    }
    
    create_l3_intf_s intf;
    intf.vsi = in_vlan;
    intf.my_global_mac = my_mac;
    intf.my_lsb_mac = my_mac;
    
    rv = l3__intf_rif__create(unit, &intf);
    *ing_intf_in = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }
    
    /*** create egress router interface ***/
    rv = vlan__open_vlan_per_mc(unit, out_vlan, 0x1);  
    if (rv != BCM_E_NONE) {
    	printf("Error, open_vlan=%d, in unit %d \n", out_vlan, unit);
    }
    rv = bcm_vlan_gport_add(unit, out_vlan, dst_port, 0);
    if (rv != BCM_E_NONE && rv != BCM_E_EXISTS) {
    	printf("fail add port(0x%08x) to vlan(%d)\n", dst_port, out_vlan);
      return rv;
    }
    
    intf.vsi = out_vlan;
    
    rv = l3__intf_rif__create(unit, &intf);
    *ing_intf_out = intf.rif;        
    if (rv != BCM_E_NONE) {
    	printf("Error, l3__intf_rif__create\n");
    }
    
    return rv;
}

int fcoe_fip_trap_configuration(int unit, int src_port)
{
    int rv, index, act_index;
    bcm_l2_cache_addr_t cache_addr;
    int dest_gport;

    bcm_l2_cache_addr_t_init(cache_addr);

    index = -1;
    cache_addr.ethertype = 0x8914    /*FIP_ETH_TYPE*/;
    cache_addr.ethertype_mask = 0xffff;
    cache_addr.flags = BCM_L2_CACHE_CPU | BCM_L2_CACHE_LEARN_DISABLE;
    cache_addr.src_port = src_port;

    BCM_GPORT_LOCAL_SET(dest_gport, 0);  
    cache_addr.dest_port = dest_gport;

    rv = bcm_l2_cache_set(unit,index, &cache_addr, &act_index);

    if (rv != BCM_E_NONE) {printf("Error, FIP configuration indx:%d unit %d, \n", act_index, unit);}

    return rv;
}

int fcoe_authorized_mact_entry_add(int unit, bcm_mac_t mac, bcm_vlan_t vlan, int dest_gport)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2_addr;

    bcm_l2_addr_t_init(&l2_addr, mac, vlan);   

    /* add static entry */
    l2_addr.flags = BCM_L2_STATIC;
    l2_addr.port = dest_gport;
    
    rv = bcm_l2_addr_add(unit,&l2_addr);

    if (rv != BCM_E_NONE) {printf("Error, mact_entry_add unit %d, \n", unit);}

    return rv;
}

int fcoe_sa_mismatch_trap_cofiguration(int unit)
{
    int rv = BCM_E_NONE;
    int trap_id;
    bcm_rx_trap_config_t trap_config;

    bcm_rx_trap_config_t_init(trap_config);

	trap_config.trap_strength = 3;
	/* trap_config.flags = BCM_RX_TRAP_UPDATE_DEST;*/ /* if unmark this comment mismatch packets will be snooped to cpu */ 
    
	rv = bcm_rx_trap_type_create(unit, 0, bcmRxTrapFcoeSrcIdMismatchSa, &trap_id);
	if (rv != BCM_E_NONE) {printf("Error, bcm_rx_trap_type_create => trap_id = %d, rv = $rv - %s \n", trap_id,bcm_errmsg(rv)); return rv; }

	rv = bcm_rx_trap_set(unit,trap_id, &trap_config);
	if (rv != BCM_E_NONE) {printf("Error, bcm_rx_trap_set => trap_config = %d, rv = $rv - %s \n", trap_config, bcm_errmsg(rv)); return rv; }

    return rv;
}

/*
 *  Configure switch to be npv and src_port to be n_port
 */
int fcoe_fcf_npv_configure(int unit, int src_port)
{
    int rv;    

    rv = bcm_fcoe_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_fcoe_init unit %d, \n", unit);
        return rv;
    }

    rv = bcm_switch_control_set(unit,bcmSwitchFcoeNpvModeEnable,1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set unit %d, \n", unit);
        return rv;
    }

    rv = bcm_port_control_set(unit, src_port, bcmPortControlFcoeNetworkPort, 0);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set unit %d, \n", unit);
        return rv;
    }    

    return rv;
}

/*
 *  Configure switch to work vsan from vft mode, and for src_port to configure default VFT value when packet has no VFT.
 */
int fcoe_fcf_vsan_from_vft_default_value_set(int unit, int src_port, int default_vft)
{
    int rv;

    /* to configure the switch to work on VFT mode... */
    /* rv = bcm_port_control_set(unit, -1, bcmPortControlFcoeFabricSel, 0);  */

    rv = bcm_port_control_set(unit, src_port, bcmPortControlFcoeFabricId, default_vft);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set unit %d, \n", unit);
        return rv;
    }
    
    return rv;    
}

/*
 *  Configure switch to work vsan from vsi mode
 */
int fcoe_fcf_vsan_from_vsi_configure(int unit)
{
    int rv;    

    rv = bcm_port_control_set(unit, -1, bcmPortControlFcoeFabricSel, bcmPortFcoeVsanSelectOuterVlan);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set unit %d, \n", unit);
        return rv;
    }

    return rv;
}


/*
 * packets will be dropped if zone entry not found
 */
int fcoe_fcf_zone_filter_enable(int unit)
{
    int rv;

    /* pmf: to drop packet that zoning lookup fail for them */
    rv = lem_zoning_setup(unit,1,pmf_group_id_for_zoning);
    if (rv != BCM_E_NONE) {
        printf("Error, FCF zoning setting unit %d \n", unit);
    }
	
	return rv;
}

int fcoe_fcf_zone_filter_disable(int unit)
{
    int rv;
    
    rv = bcm_field_group_destroy(unit, pmf_group_id_for_zoning);
    if (BCM_E_NONE != rv) {
      printf("Error in bcm_field_group_destroy\n");      
    }

    return rv;
}

int fcoe_fcf_zone_entry_add(int unit, int vft, uint32 src, uint32 dst)
{
    int rv;
    bcm_fcoe_zone_entry_t zone_entry;
    
    bcm_fcoe_zone_entry_t_init(zone_entry);
    zone_entry.action = bcmFcoeZoneActionAllow; /* this is the only action supported for zoning */
    /* add FCF zoning entries */
    zone_entry.s_id = src;
    zone_entry.d_id = dst;
    zone_entry.vsan_id = vft;

    /* add to all units in system*/
    rv = bcm_fcoe_zone_add(unit, &zone_entry); 
    if (rv != BCM_E_NONE) {
        printf("Error, adding zoning entry indx:%d unit %d, \n", indx, unit);
    }    

    return rv;
}


/***************************************************************************************** 
  * FCoE examples
 */

/*
* this is a simple example to configure a transit switch. 
*  
* in this example we authorize packets with SRC MAC 00:00:00:00:00:12 
* all FIP packets are snooped to CPU. 
* packet that has mismatch in SRC MAC and source ID will dropped. 
*  
* learning is disabeled for all FIP nad FCoE packets.
 */
int fcoe_transit_example(int unit, int fip_src_port, int dest_port)
{
    bcm_mac_t  mac  = {0};
    bcm_vlan_t vlan = 1;
    int        rv   = BCM_E_NONE;

    mac[5] = 0x12;

    bcm_fcoe_init(unit);

    rv = fcoe_fip_trap_configuration(unit, fip_src_port);
    if (rv != BCM_E_NONE) {printf("Error, fcoe_fip_trap_configuration unit %d, \n", unit);}

    rv = fcoe_sa_mismatch_trap_cofiguration(unit);
    if (rv != BCM_E_NONE) {printf("Error, fcoe_sa_mismatch_trap_cofiguration unit %d, \n", unit);}

    rv = fcoe_authorized_mact_entry_add(unit, mac, vlan, dest_port);
    if (rv != BCM_E_NONE) {printf("Error, fcoe_fip_trap_configuration unit %d, \n", unit);}

    return rv;
}

/*
* this example configures a fcf switch and adding route according to the following rules. 
*   route a: host_a, no vft route FW to dmac [01]
*   route b: host_b, with vft2 FW to dmac [02]
*   route c: lpm entry for hosts 0x12:XX:XX:XX | 0x11:XX:XX:XX, and vft2 FW to dmac[03] (remote route)
*/
int fcoe_fcf_example(int unit,int in_sysport, int out_sysport, uint32 insertion_flags, int mask, int vft_2)
{
    int rv;
    int ing_intf_in  = 0; 
    int ing_intf_out = 0; 
    int flags        = 0;
    int indx         = 0;
    int in_vlan      = 1; 
    int out_vlan     = 100;
    int fec[3]       = {0};
    int encap_id[3]  = {0};     
    bcm_mac_t mac_address   = {0x00, 0x00, 0x00, 0x00, 0x00, 0x12};  /* my-MAC */
    bcm_mac_t next_hop_mac_array[]  = {{0x00, 0x00, 0x00, 0x00, 0xcd, 0x01}, {0x00, 0x00, 0x00, 0x00, 0xcd, 0x02},{0x00, 0x00, 0x00, 0x00, 0xcd, 0x03}}; 
    int host_a = 0x111111;
    int host_b = 0x121111;
    uint32 lpm_flag = BCM_FCOE_DOMAIN_ROUTE;
    create_l3_egress_s l3eg;

    rv = fcoe_fcf_switch_create(unit, mac_address, in_sysport, out_sysport, in_vlan, out_vlan, flags, &ing_intf_in, &ing_intf_out);
    if (rv != BCM_E_NONE){
            printf("Error, fcoe_fcf_switch_create, in unit %d \n", in_sysport, unit);
    }    

    rv = bcm_fcoe_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_fcoe_init fail \n");
        return rv;
    }
    
    /* create egress objects 1 */
    for (indx = 0; indx < 3; indx++) {
        
        sal_memcpy(l3eg.next_hop_mac_addr, next_hop_mac_array[indx] , 6);
        l3eg.allocation_flags = 0;
        l3eg.l3_flags = 0;
        l3eg.out_tunnel_or_rif = ing_intf_out;
        l3eg.out_gport = out_sysport;
        l3eg.vlan = in_vlan;
        l3eg.fec_id = fec[indx];
        l3eg.arp_encap_id = encap_id[indx];

        rv = l3__egress__create(unit,&l3eg);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress__create\n");
            return rv;
        }

        fec[indx] = l3eg.fec_id;
        encap_id[indx] = l3eg.arp_encap_id;

        if(verbose >= 1) {
            printf("created FEC-id =0x%08x, in unit %d \n", fec[indx], unit);
            printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[indx], unit);
        }
    }

    rv = add_fcf_route(unit, host_a, mask, 0, fec[0], insertion_flags);
    if (rv != BCM_E_NONE) {
        printf("Error, add_fcf_route unit %d, \n",unit);
    }    

    rv = add_fcf_route(unit, host_b, mask, vft_2, fec[1], insertion_flags);
    if (rv != BCM_E_NONE) {
        printf("Error, add_fcf_route unit %d, \n",unit);
    }

    /* LPM entry  */
    if (insertion_flags & BCM_FCOE_SOURCE_ROUTE) {
        lpm_flag = BCM_FCOE_SOURCE_ROUTE | BCM_FCOE_DOMAIN_ROUTE;
    }
    rv = add_fcf_route(unit, host_a, 0xfe0000, vft_2, fec[2], lpm_flag);
    if (rv != BCM_E_NONE) {
        printf("Error, add_fcf_route unit %d, \n",unit);
    }

    /* adding small semantic test delete and add the entry */
    rv = delete_fcf_route(unit, host_a, 0xfe0000, vft_2, fec[2], lpm_flag);
    if (rv != BCM_E_NONE) {
        printf("Error, deleting fcf route unit %d, \n",unit);
    }

    rv = add_fcf_route(unit, host_a, 0xfe0000, vft_2, fec[2], lpm_flag);
    if (rv != BCM_E_NONE) {
        printf("Error, add_fcf_route unit %d, \n",unit);
    }

    return rv;
}


/*
 * packet will be routed from in_port to out-port 
 */

/* 
 * FCF routing example.
 * 
 * open router interface (vlan=1,DA=00:0c:00:02:00:00}
 * fill FCF routing table
  ------------------------------------------------------------------------ -------------------------------
  | Remote domain in LPM  |BCM_FCOE_DOMAIN_ROUTE      |nport_id = 0xBA0000|nport_id_mask = 0xFE0000 |intf1|
  ------------------------------------------------------------------------ -------------------------------
  | Remote domain in LEM  |BCM_FCOE_HOST_ROUTE        |nport_id = 0xBB0000|nport_id_mask = 0xFF0000 |intf2|
  ------------------------------------------------------------------------ -------------------------------
  | Local <SEM + LEM>     |BCM_FCOE_LOCAL_ADDRESS     |nport_id = 0xAA0000|nport_id_mask = 0xFF0000 |     |
  |                       |BCM_FCOE_HOST_ROUTE        |nport_id = 0xAAAAAA|nport_id_mask = 0xFFFFFF |intf3|
   -------------------------------------------------------------------------------------------------------  

*/ 

int fcf_zoning_example(int *units_ids, int nof_units, int in_sysport, int out_sysport){
    int i;
    int rv;
    int unit;
    int ing_intf_in = 0; 
    int ing_intf_out = 0; 
    int indx;
    int flags = 0;
    int flags1 = 0;
    int in_vlan = 10; 
    int out_vlan = 100;
    int vrf = 0;
    int open_vlan=1;
    int insertion_flags;
    bcm_mac_t mac_address  = {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00};  /* my-MAC */
    bcm_mac_t next_hop_mac  = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d}; /* next_hop_mac1 */
    bcm_fcoe_route_t fcf_routes[4];
    int fec[4] = {0x0};
    int encap_id[4]={0x0}; 
    int nof_next_hops = 4;
    int not_found_drop = 0;/* 0 means don't drop, 1 is drop*/

    /* zoning info */
    bcm_fcoe_zone_entry_t zone_entry;
    bcm_fcoe_nport_id_t allowed_src = 0xcccccc; /* allowed src set for all entries for simplicity*/
        
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = fcoe_fcf_switch_create(unit, mac_address, in_sysport, out_sysport, in_vlan, out_vlan, flags, &ing_intf_in, &ing_intf_out);
        if (rv != BCM_E_NONE){
            printf("Error, fcoe_fcf_switch_create, in unit %d \n", in_sysport, unit);
        }
        flags |= BCM_L3_WITH_ID;
    }

    /* init routes */
    fcoe_init_routes(fcf_routes);
    vrf = 0x0;

    /*** init FC module ***/
    rv = bcm_fcoe_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_fcoe_init fail \n");
        return rv;
    }

    /* needed for FCF, fix header building*/
    rv = fcf_workaround(units_ids,nof_units);
    if (rv != BCM_E_NONE) {
        printf("Error, fcf_workaround fail.. continue \n");
    }

    /* packets will be dropped if zone entry not found */
    for (i = 0 ; i < nof_units ; i++){
        unit = units_ids[i];
        rv = fcoe_fcf_zone_filter_enable(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, zone filter enable failed... continue \n");
        }
    }

    /* FCF traps setting: what to do when Forwarding or zonning lookup fail*/
    rv = fcf_traps_set(units_ids,nof_units,not_found_drop);
    if (rv != BCM_E_NONE) {
        printf("Error, fcf_workaround fail \n");
        return rv;
    }

    create_l3_egress_s l3eg;
    /* create all egress object with different DA each time */
    for (indx = 0; indx < nof_next_hops; indx++) {
        flags1 = 0;
        for (i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];
            
            sal_memcpy(l3eg.next_hop_mac_addr, next_hop_mac , 6);
            l3eg.allocation_flags = flags1;
            l3eg.l3_flags = flags1;
            l3eg.out_tunnel_or_rif = ing_intf_out;
            l3eg.out_gport = out_sysport;
            l3eg.vlan = out_vlan;
            l3eg.fec_id = fec[indx];
            l3eg.arp_encap_id = encap_id[indx];

            rv = l3__egress__create(unit,&l3eg);
            if (rv != BCM_E_NONE) {
                printf("Error, l3__egress__create\n");
                return rv;
            }

            fec[indx] = l3eg.fec_id;
            encap_id[indx] = l3eg.arp_encap_id;
            if(verbose >= 1) {
                printf("created FEC-id =0x%08x, in unit %d \n", fec[indx], unit);
                printf("next hop mac at encap-id %08x, in unit %d\n", encap_id[indx], unit);
            }
            flags1 |= BCM_L3_WITH_ID;
        }
        /* increment next-hop-DA*/
        mac_inc(&next_hop_mac);
    }

    /* add FCF routing entries */
    for (indx = 0; indx < nof_next_hops; indx++) {
        /*** add route point to FEC ***/        
        insertion_flags = BCM_FCOE_HOST_ROUTE;
        /* Units order does not matter*/
        for (i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];
            rv = add_fcf_route(unit, fcf_routes[indx].nport_id, fcf_routes[indx].nport_id_mask, vrf, fec[indx], fcf_routes[indx].flags); 
            if (rv != BCM_E_NONE) {
                printf("Error, create egress object, in_sysport=%d in unit %d, \n", in_sysport, unit);
            }
        }
    }

    /***** zonning setting ***/
    bcm_fcoe_zone_entry_t_init(zone_entry);
    zone_entry.action = bcmFcoeZoneActionAllow; /* this is only allowed action, if entry not found it will be dropped */
    /* add FCF zoning entries */
    for (indx = 0; indx < nof_next_hops; indx++) {
        zone_entry.s_id = allowed_src;
        zone_entry.d_id = fcf_routes[indx].nport_id;
        zone_entry.vsan_id = vrf;

        /* add to all units in system*/
        for (i = 0 ; i < nof_units ; i++){
            unit = units_ids[i];
            rv = bcm_fcoe_zone_add(unit, &zone_entry); 
            if (rv != BCM_E_NONE) {
                printf("Error, adding zoning entry indx:%d unit %d, \n", indx, unit);
            }
        }
    }

    return rv;
}



