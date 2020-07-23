/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*
 * This script is for flow based testing of the pp and tm.
 * A traditional bridge is set up for vlan id 3 with 2 ports (0 and 1).
 * cint_tm_fap_config2.c is run to set up the TM hierarchy on these ports.
 * The hierarchy contains both flow and sysport based queue groups.
 * The flow based queue groups are used for the vlan bridge.  These queue groups
 * are returned in the call to BCM_GPORT_UNICAST_QUEUE_GROUP_SET and passed in when
 * the vlan is created.
 * 
 * to run:
  cint cint_port_tpid.c
  cint cint_pp_config2.c
  cint_tm_fap_config2.c
  cint
  pp_config2_run(unit, 0, 1, 0);
*
* Send in a packet on port 0 with mac da address 00 00 00 00 00 22 and vlan id 3 and receive
* on port 1.  Queue used will be flow based queue.
*
* to verify, one might send traffic continuously and execute this command to see credit rate.
*  dune 'soc_petra_api scheduler print_flow_and_up flow_ndx 44'
*/
 
/* set to one for informative prints */
int verbose = 1;
struct pp_config2_s {
  bcm_port_t port1; /* physical port, where the gport1 is defined */
  bcm_port_t port2;
  int p_vlans[4];
  
};


pp_config2_s pp_config2_info;

void 
pp_config2_info_init(int port1, int port2, int rcy_port_type) {

    if (rcy_port_type) {
        pp_config2_info.port1 = tm_config2_get_ucast_handle(port1);
        pp_config2_info.port2 = tm_config2_get_ucast_handle(port2);
    } else {
        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(pp_config2_info.port1, (port1*8) + 24);
        BCM_GPORT_UNICAST_QUEUE_GROUP_SET(pp_config2_info.port2, (port2*8) + 24);
    }
     printf("ucast queue gport(0x%08x)\n", pp_config2_info.port1);
     printf("ucast queue gport(0x%08x)\n", pp_config2_info.port2);

     pp_config2_info.p_vlans[0] = 3; /* p1 in/eg vlans */
     pp_config2_info.p_vlans[1] = 3;
     pp_config2_info.p_vlans[2] = 3;  /* p2 in/eg vlans */
     pp_config2_info.p_vlans[3] = 3;
}
/*
 * create vlan-port
 *   vlan-port: is Logical interface identified by (port-vlan-vlan). 
 */
int
pp_config2_add_port_1(int unit,  bcm_gport_t *port_id) {
  int rv;
  bcm_vlan_port_t vp1;
  bcm_vlan_port_t_init(&vp1);
  
  /* the match criteria is port:1, out-vlan:10,*/
  vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
  vp1.port = pp_config2_info.port1;
  vp1.match_vlan = pp_config2_info.p_vlans[0];
  vp1.match_inner_vlan = 0;
  vp1.egress_vlan = pp_config2_info.p_vlans[1];
  vp1.egress_inner_vlan = 0;
  vp1.flags = 0;
 /* this is relevant only when get the gport, not relevant for creation */
 /* will be pupulated when the gport is added to service, using vswitch_port_add */
  vp1.vsi = 0;
  rv = bcm_vlan_port_create(unit,&vp1);
  
  if (rv != BCM_E_NONE) {
    printf("Error, bcm_vlan_port_create\n");
    print rv;
    return rv;
  }
  
  /* handle of the created gport */
  *port_id = vp1.vlan_port_id;
  printf("vlan_port_id(0x%08x)\n", *port_id);
  return rv;
}

/*
 * create vlan-port
 *   vlan-port: is Logical interface identified by (port-vlan-vlan). 
 */
int
pp_config2_add_port_2(int unit,  bcm_gport_t *port_id){
  int rv;
  bcm_vlan_port_t vp1;
  bcm_vlan_port_t_init(&vp1);
  
  vp1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
  vp1.port = pp_config2_info.port2;
  vp1.match_vlan = pp_config2_info.p_vlans[2];
  vp1.match_inner_vlan = 0;

  vp1.egress_vlan = pp_config2_info.p_vlans[3];
  vp1.egress_inner_vlan = 0;
  vp1.flags = 0;
 /* this is relevant only when get the gport, not relevant for creation */
 /* will be populated when the gport is added to service, using vswitch_port_add */
  vp1.vsi = 0;
  rv = bcm_vlan_port_create(unit,&vp1);
  /* handle of the created gport */
  *port_id = vp1.vlan_port_id;
  
  return rv;
}

/*
 * add L2 entry points to the given gport
 */
int
pp_config2_add_l2_addr_to_gport(int unit, int gport_id, bcm_mac_t mac1, bcm_vlan_t v1){
  int rv;
  bcm_gport_t g1;
  /*bcm_mac_t mac1;*/
  bcm_l2_addr_t l2_addr1;
  /*bcm_vlan_t v1;*/
  
  /* local: MACT, add entry points to local-port */
  bcm_l2_addr_t_init(&l2_addr1,mac1,v1); 
  l2_addr1.flags = 0x00000020; /* static entry */
  g1 = gport_id;
  l2_addr1.port = g1;
  rv = bcm_l2_addr_add(unit,&l2_addr1);
  
  printf("  0x%08x\n\r",rv);
  return rv;
}

int
pp_config2_run(int unit, int port1, int port2, int rcy_port2_type) {
  int rv;
  bcm_gport_t gport1;
  bcm_gport_t gport2;
  bcm_mac_t kmac;
  bcm_vlan_t kvlan = 3;

  pp_config2_info_init(port1, port2, rcy_port2_type);
  
  port_tpid_init(port1,1,0);
  rv = port_tpid_set(unit);
  if (rv != BCM_E_NONE) {
    printf("Error, port_tpid_set\n");
    print rv;
    return rv;
  }

  if (!rcy_port2_type) {
    port_tpid_init(port2,1,0);
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
      printf("Error, port_tpid_set\n");
      print rv;
      return rv;
    }
  }

  rv = bcm_vlan_create(unit,kvlan);
  if (rv != BCM_E_NONE) {
    printf("Error, in bcm_vlan_create\n");
    return rv;
  }
	
  /* bcm_vlan_port_create() */
  rv = pp_config2_add_port_1(unit, &gport1);
  if (rv != BCM_E_NONE) {
    printf("Error, pp_config2_add_port_1\n");
    print rv;
    return rv;
  }
  if(verbose){
    printf("created vlan-port   0x%08x\n\r",gport1);
  }


  /* bcm_vswitch_add_port() */

  rv = pp_config2_vswitch_add_port(unit, kvlan, gport1);
  if (rv != BCM_E_NONE) {
    printf("Error, vswitch_add_port\n");
    return rv;
  }


  /* bcm_vlan_port_create() */
  rv = pp_config2_add_port_2(unit, &gport2);
  if (rv != BCM_E_NONE) {
    printf("Error, pp_config2_add_port_2\n");
    print rv;
    return rv;
  }

  rv = pp_config2_vswitch_add_port(unit, kvlan, gport2);
  if (rv != BCM_E_NONE) {
    printf("Error, vswitch_add_port\n");
    return rv;
  }

  if(verbose){
      printf("created vlan-port   0x%08x\n\r",gport2);
   }

 kmac[5] = 0x11;
  /* bcm_l2_addr_add() */
  rv = pp_config2_add_l2_addr_to_gport(unit, gport1, kmac, kvlan);
  if (rv != BCM_E_NONE) {
    printf("Error, vswitch_add_l2_addr_to_gport\n");
    return rv;
  }

  kmac[5] = 0x22;
  /* bcm_l2_addr_add() */
  rv = pp_config2_add_l2_addr_to_gport(unit, gport2, kmac, kvlan);
  if (rv != BCM_E_NONE) {
    printf("Error, vswitch_add_l2_addr_to_gport\n");
    return rv;
  }

  return rv;
}

int pp_config2_vswitch_add_port(int unit, bcm_vlan_t  vsi, bcm_gport_t gport ){
   int rv;
   
   /* add to vswitch */
   rv = bcm_vswitch_port_add(unit, vsi, gport);
   if (rv != BCM_E_NONE) {
    printf("Error, bcm_vswitch_port_add\n");
    return rv;
  }
  if(verbose){
      printf("add port   0x%08x to vswitch   0x%08x \n\r",gport, vsi);
   }
  return BCM_E_NONE;
}

