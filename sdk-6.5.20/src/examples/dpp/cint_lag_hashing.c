/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Lag hashing example script
 */

/* 
the cint creates a Trunk (LAG), containing several ports, and points a MAC-table entry to this LAG. 
 
default LAG hashing example: 
----------------------------
run: 
cint utility/cint_utils_l3.c 
cint utility/cint_utils_vlan.c 
cint cint_ip_route.c
cint cint_lag_hashing.c 
cint 
print lag_hashing_main(unit, <in_port>, <out_port>, <nof_ports>); 
 
traffic example: 
run: 
    1) ethernet header with DA 00:0C:00:02:01:23 and vlan tag id 17
       and IPV4 header with DIP 10.0.255.0 and various SIPs (random)
    2) ethernet header with DA 00:0C:00:02:01:23 and vlan tag id 17
       MPLS header with label_1 44 and various label_2 (incremental)
    3) ethernet header with DA 00:0C:00:02:01:24, vlan tag id 17 and various SA (random)
 
lag_hashing_main() does not change the default LAG hashing configuration, which is to look at all the packet fields: 
for L2 packet - Ether type, Next header, MAC destination address, MAC source address. 
for IPV4 packet - DIP, SIP, protocol, dest L4 port and src L4 port.
for MPLS packet - fisrt label, second label and third label. 
 
however, by default the number of headers to use for LAG hashing is 0, so all traffic will go to the same LAG member (same port). 
 
run: 
config_per_port_lag_hashing(unit, <in_port>, 1, 1); 
 
now the number of headers for LAG hashing is 1 and it will start from the forwarding header. 
traffic will be divided equally between the LAG memebers (different ports). 
 
LAG hashing according to terminated header example:
--------------------------------------------------- 
run: 
    1) ethernet header with DA 0:C:0:2:01:23, vlan tag id 17 and various SA (random)
       and IPV4 header with DIP 10.0.255.0 and SIP 0.0.0.0
    2) ethernet header with DA 0:C:0:2:01:23, vlan tag id 17 and various SA (random)
       MPLS header with label_1 44 and label_2 55
    3) ethernet header with DA 0:C:0:2:01:24, vlan tag id 17 and various SA (random)
 
LAG hashing is set to look at headers starting from the forwarding header, so changes in the terminated header will not affect the hashing. 
all traffic will go to the same LAG member (same port). 
 
run: 
config_per_port_lag_hashing(unit, <in_port>, 3, 0); 
 
run: 
    1) ethernet header with DA 0:C:0:2:01:23, vlan tag id 17 and various SA (random)
       and IPV4 header with DIP 10.0.255.0 and SIP 0.0.0.0
    2) ethernet header with DA 0:C:0:2:01:23, vlan tag id 17 and various SA (random)
       MPLS header with label_1 44 and label_2 55 
 
now LAG hashing is set to look at the terminated header (as well as the forward header). 
traffic will be divided equally between the LAG memebers (different ports). 
 
run: 
disable_lag_hashing(unit); 
 
This will disable LAG hashing. No part of the packet will be used for hashing, so all traffic will go to the same LAG member (same port). 
 
run: 
lag_hash_l2_src(unit); 
 
Now LAG hashing is set to look at only the L2 SRC MAC address (SA), so traffic will be divided again equally between the LAG memebers (different ports). 
 
LAG hashing according to L4 example:
------------------------------------
run: 
lag_hash_l4_dest(unit); 
 
run: 
    ethernet header with DA 0:C:0:2:01:23, vlan tag id 17 and SA 0
    IPV4 header with DIP 10.0.255.0 and SIP 0.0.0.0
    and TCP header with vairous dest ports (random)
 
now LAG hashing is set to look at 3 headers (terminated - Eth, forward - IPV4 and L4 - TCP). it is also set to look at the L4 dest port.
traffic will be divided equally between the LAG memebers (different ports). 
 
LAG hashing according to SRC port:
----------------------------------
run: 
lag_src_port_enable(unit, 1); 
 
run: 
    ethernet header with DA 0:C:0:2:01:23, vlan tag id 17 and SA 0
    IPV4 header with DIP 10.0.255.0 and SIP 0.0.0.0
    from 2 different incoming (src) ports.
 
now LAG hashing is set to look at the source port. traffic from one in-port will be sent to one of the LAG memebers (one port) 
and traffic from the other in-port, will be sent to a different one of the LAG members (different port). 
 
Non polynomial LAG hashing:
---------------------------
 
run: 
disable_lag_hashing(unit); 
lag_hash_func_config(unit, BCM_HASH_CONFIG_ROUND_ROBIN); 
 
Now instead of a polynomial hashing, a counter that is incremented on every packet is used.
Traffic (random or not) will be divided equally between the LAG memebers, although LAG hashing is disabled.  


ELI BOS hashing:
---------------------------
run
lag_hashing_eli_bos(int unit, int in_port, int outPort0, int outPort1, int outPort2)

This cint use to test ELI-BOS LAG feature, it creates a LAG of size 3.
The MPLS switch tunnel points to a FEC that uses the LAG

*/

struct cint_lag_hashing_cfg_s {
  int is_stateful; /* If 1 then lag_hashing_main does stateful load balancing. */
};

cint_lag_hashing_cfg_s cint_lag_hashing_cfg = {
  0, /* is_stateful */
};

/* 
config_per_port_lag_hashing(): 
set per-port LAG hashing properties: 
1) nof_headers - the number of headers to use for LAG hashing. range: 0-3 
2) forward - first header for LAG hashing. if forward==1 first header will be the forwarding header, 
             else first header will be the header below the forwarding header (the terminated header) 
*/ 
int config_per_port_lag_hashing(int unit, int in_port, int nof_headers, int forward) {
  int rc;
  int arg;
  bcm_switch_control_t type;

  /* select which starting header to use for LAG hashing */
  type = bcmSwitchTrunkHashPktHeaderSelect;
  if (forward) { /* first header for LAG hashing will be the forwarding header */
      arg = BCM_HASH_HEADER_FORWARD; 
  }
  else { /* first header will be the header below the forwarding header (the terminated header) */
      arg = BCM_HASH_HEADER_TERMINATED;
  }

  rc = bcm_switch_control_port_set(unit, in_port, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_switch_control_port_set with arg %d failed: %d \n", arg, rc);
  }

  /* select number of headers to consider in LAG hashing */
  type = bcmSwitchTrunkHashPktHeaderCount;
  arg = nof_headers;

  rc = bcm_switch_control_port_set(unit, in_port, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_switch_control_port_set with arg %d failed: %d \n", arg, rc);
  }

  return rc;
}

/* 
disable_lag_hashing(): 
set LAG hashing to "look at nothing". 
no part of the header will be used in hashing. 
in this case, hashing result will be the same for every packet that arrives.
*/ 
int disable_lag_hashing(int unit) {
  int rc;
  int arg = 0; /* arg = 0 so no field in the Eth header will be looked at */
  bcm_switch_control_t type;

  /* disable L2 hashing */
  type = bcmSwitchHashL2Field0;
  rc = bcm_switch_control_set(unit, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchHashL2Field0 failed: %d \n", rc);
  }

  /* disable IPV4 hashing */
  type = bcmSwitchHashIP4Field0;
  rc = bcm_switch_control_set(unit, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchHashIP4Field0 failed: %d \n", rc);
  }

  /* disable MPLS hashing */
  type = bcmSwitchHashMPLSField0;
  rc = bcm_switch_control_set(unit, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchHashMPLSField0 failed: %d \n", rc);
  }

  type = bcmSwitchHashMPLSField1;
  rc = bcm_switch_control_set(unit, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchHashMPLSField1 failed: %d \n", rc);
  }

  return rc;
}

/* 
lag_hash_l4_dest(): 
set LAG hashing to done according to destination L4 port. 
*/ 
int lag_hash_l4_dest(int unit) {
  int rc;
  int arg = BCM_HASH_FIELD_DSTL4;
  bcm_switch_control_t type = bcmSwitchHashIP4Field0;

  rc = bcm_switch_control_set(unit, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchHashIP4Field0 and arg %d failed: %d \n", arg, rc);
  }

  return rc;
}

/* 
lag_hash_l2_src(): 
set LAG hashing to done according to L2 SA. 
*/ 
int lag_hash_l2_src(int unit) {
  int rc;
  int arg = BCM_HASH_FIELD_MACSA_LO | BCM_HASH_FIELD_MACSA_MI | BCM_HASH_FIELD_MACSA_HI;
  bcm_switch_control_t type = bcmSwitchHashL2Field0;

  rc = bcm_switch_control_set(unit, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchHashL2Field0 and arg %d failed: %d \n", arg, rc);
  }

  return rc;
}

/* 
lag_hash_l2_dst(): 
set LAG hashing to done according to L2 DA. 
*/ 
int lag_hash_l2_dst(int unit) {
  int rc;
  int arg = BCM_HASH_FIELD_MACDA_LO | BCM_HASH_FIELD_MACDA_MI | BCM_HASH_FIELD_MACDA_HI;
  bcm_switch_control_t type = bcmSwitchHashL2Field0;

  rc = bcm_switch_control_set(unit, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchHashL2Field0 and arg %d failed: %d \n", arg, rc);
  }

  return rc;
}

/* 
lag_src_port_enable(): 
Make the Source port a part of the LAG hash.
arg = 1- enable, 0- disable. 
*/ 
int lag_src_port_enable(int unit, int arg) {
  int rc;
  bcm_switch_control_t type = bcmSwitchTrunkHashSrcPortEnable;

  rc = bcm_switch_control_set(unit, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchTrunkHashSrcPortEnable failed: %d \n", rc);
  }

  return rc;
}

/* 
lag_hash_func_config(): 
set LAG hashing function (polynomial). 
bcm_hash_config = BCM_HASH_CONFIG_*
*/ 
int lag_hash_func_config(int unit, int bcm_hash_config) {
  int rc;
  bcm_switch_control_t type = bcmSwitchTrunkHashConfig;

  rc = bcm_switch_control_set(unit, type, bcm_hash_config);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchTrunkHashConfig failed: %d \n", rc);
  }

  return rc;
}

/* 
lag_hash_label5(): 
set LAG hashing to be done according to MPLS label5. 
in this case, hashing result will be the same for every packet with the same label5.
*/ 
int lag_hash_label5(int unit) {
  int rc;
  int arg = BCM_HASH_FIELD_2ND_LABEL;
  bcm_switch_control_t type = bcmSwitchHashMPLSField1;

  rc = bcm_switch_control_set(unit, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchHashMPLSField1 failed: %d \n", rc);
  }

  return rc;
}

/* 
lag_hash_label2(): 
set LAG hashing to be done according to MPLS label2. 
in this case, hashing result will be the same for every packet with the same label2.
*/ 
int lag_hash_label2(int unit) {
  int rc;
  int arg = BCM_HASH_FIELD_2ND_LABEL;
  bcm_switch_control_t type = bcmSwitchHashMPLSField0;

  rc = bcm_switch_control_set(unit, type, arg);
  if (rc != BCM_E_NONE) {
    printf ("bcm_petra_switch_control_set with type bcmSwitchHashMPLSField0 failed: %d \n", rc);
  }

  return rc;
}

/* 
lag_hashing_main():
1) create LAG and add ports to it
2) Add IPV4 host entry and point to LAG.
3) Add switch entry to swap MPLS labels and also send to LAG
 
nof_ports = the number of ports that will be added to the LAG (members)
out_port  = the out ports numbers will be: <out_port>, <out_port> + 1, ... , <out_port> + <nof_ports> -1 
*/ 
int lag_hashing_main(int unit, int in_port, int out_port, int nof_ports) {

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

  if (cint_lag_hashing_cfg.is_stateful) {
    rc = bcm_trunk_psc_set(unit, trunk_id, BCM_TRUNK_PSC_DYNAMIC_RESILIENT);
    if (rc != BCM_E_NONE) {
      printf ("The following call failed: \n", rc);
      printf ("bcm_trunk_psc_set(%d, %d, BCM_TRUNK_PSC_DYNAMIC_RESILIENT);\n", unit, trunk_id);
      print rc;
      return rc;
    }
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

/*
 * This functions is used for cleanup after running the cint. It destroys all trunks on the device.
 */

int
lag_hashing_trunk_destroy(int unit){
    int rv;

    rv = bcm_trunk_detach(unit);
    if (rv != BCM_E_NONE){
        printf("Error, in bcm_trunk_detach\n");
        return rv;
    }

    rv = bcm_trunk_init(unit);

    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_trunk_init\n");
        return rv;
    }

    return rv;
}

/*
 * This cint use to test ELIBOS LAG feature, it creates a LAG of size 3.
 * The MPLS tunnel switch points to a FEC that uses the created LAG
 */
int lag_hashing_eli_bos(int unit, int in_port, int outPort0, int outPort1, int outPort2)
{
    int rv,i;
    bcm_l3_intf_t l3InIf;
    bcm_if_t l3egid;
    bcm_l3_egress_t l3eg;
    bcm_mac_t mac_l3_ingress_add = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d};
    bcm_mac_t mac_l3_engress_add = {0x00, 0x00, 0x00, 0x00, 0x66, 0x22};
    bcm_trunk_t trunk_id; /* trunk */
    bcm_trunk_member_t member;
    bcm_mpls_tunnel_switch_t mpls_tunnel_info;
    int outPorts[3];
    int nof_ports = 3;
    int trunk_gport;

    bcm_mpls_tunnel_switch_t entry;
    bcm_mpls_egress_label_t label_array;

    outPorts[0] = outPort0;
    outPorts[1] = outPort1;
    outPorts[2] = outPort2;


    /*Create L3 interface - In-Rif*/
    bcm_l3_intf_t_init(l3InIf);

    sal_memcpy(l3InIf.l3a_mac_addr, mac_l3_ingress_add, 6);
    l3InIf.l3a_vid = 400;
    l3InIf.l3a_vrf = 400;
    rv = bcm_l3_intf_create(unit, &l3InIf);
    if (rv != BCM_E_NONE) {
       return rv;
    }

    /* create trunk */
    rv = bcm_trunk_create(unit, 0, &trunk_id);
    if (rv != BCM_E_NONE) {
      printf ("bcm_trunk_create failed: %d \n", rv);
      return rv;
    }

    /* add ports to trunk */
    for (i = 0; i < nof_ports; i++) {
        bcm_trunk_member_t_init(&member);
        BCM_GPORT_LOCAL_SET(member.gport, outPorts[i]); /* phy port to local port */
        rv = bcm_trunk_member_add(unit, trunk_id, &member);
        if (rv != BCM_E_NONE) {
          printf ("bcm_trunk_member_add with port %d failed: %d \n", out_port, rv);
          return rv;
        }
    }

    /* create FEC and send to LAG (instead of out-port) */
    BCM_GPORT_TRUNK_SET(trunk_gport, trunk_id); /* create a trunk gport and give this gport to create_l3_egress() instead of the dest-port */

    /*Create L3 egress*/
    bcm_l3_egress_t_init(&l3eg);


    sal_memcpy(l3eg.mac_addr, mac_l3_engress_add, 6);
    l3eg.vlan   = 300;
    l3eg.port   = trunk_gport;
    l3eg.flags  = BCM_L3_EGRESS_ONLY;
    rv = bcm_l3_egress_create(unit, 0, &l3eg, &l3egid);
    if (rv != BCM_E_NONE) {
      printf ("bcm_l3_egress_create failed \n");
      return rv;
    }

    for (i = 0; i < 100; i++) {
        /* add switch entry to swap labels and map to LAG (FEC pointing at LAG) */
        bcm_mpls_tunnel_switch_t_init(&mpls_tunnel_info);
        mpls_tunnel_info.action = BCM_MPLS_SWITCH_ACTION_SWAP;
        mpls_tunnel_info.flags = BCM_MPLS_SWITCH_TTL_DECREMENT; /* TTL decrement has to be present */
        mpls_tunnel_info.flags |= BCM_MPLS_SWITCH_OUTER_TTL|BCM_MPLS_SWITCH_OUTER_EXP;
        mpls_tunnel_info.label = 0x100 + i; /* incomming label */
        mpls_tunnel_info.egress_label.label = 0x101; /* outgoing (egress) label */
        mpls_tunnel_info.egress_if = l3egid; /* FEC */

        rv = bcm_mpls_tunnel_switch_create(unit, &mpls_tunnel_info);
        if (rv != BCM_E_NONE) {
          printf ("bcm_mpls_tunnel_switch_create failed: %x \n", rv);
          return rv;
        }
    }

    return rv;
}


/*
 * This cint is used to configure LAG setup of 3 ports,  3 MPLS labels popping and creating and IPv4/6
 * routes to test LB cases in which the MPLS is terminated and the forward is over the IP.
 */
int lag_hashing_pop_mpls_labels_ip_frwrd(int unit, int in_port, int outPort0, int outPort1, int outPort2)
{
    int rv,i,bos;
    bcm_l3_intf_t l3InIf;
    bcm_if_t l3egid;
    bcm_l3_egress_t l3eg;
    bcm_mac_t mac_l3_ingress_add = {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d};
    bcm_mac_t mac_l3_engress_add = {0x00, 0x00, 0x00, 0x00, 0x66, 0x22};
    uint32 addr                  = 0x2c2c2c2c;
    uint32 mask                  = 0xFFFF0000;
    bcm_ip6_t l3a_ip6_addr       = {0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44};
    bcm_ip6_t l3a_ip6_mask       = {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    int index;
    bcm_trunk_t trunk_id; /* trunk */
    bcm_trunk_member_t member;
    bcm_mpls_tunnel_switch_t mpls_tunnel_info;
    int outPorts[3];
    int nof_ports = 3;
    int trunk_gport;
    int vrf = 400;
    bcm_mpls_tunnel_switch_t entry;
    bcm_mpls_egress_label_t label_array;

    outPorts[0] = outPort0;
    outPorts[1] = outPort1;
    outPorts[2] = outPort2;


    /*Create L3 interface - In-Rif*/
    bcm_l3_intf_t_init(l3InIf);
    sal_memcpy(l3InIf.l3a_mac_addr, mac_l3_ingress_add, 6);
    l3InIf.l3a_vid = vrf;
    l3InIf.l3a_vrf = vrf;
    rv = bcm_l3_intf_create(unit, &l3InIf);
    if (rv != BCM_E_NONE) {
       return rv;
    }

    /* create trunk */
    rv = bcm_trunk_create(unit, 0, &trunk_id);
    if (rv != BCM_E_NONE) {
      printf ("bcm_trunk_create failed: %d \n", rv);
      return rv;
    }

    /* add ports to trunk */
    for (i = 0; i < nof_ports; i++) {
        bcm_trunk_member_t_init(&member);
        BCM_GPORT_LOCAL_SET(member.gport, outPorts[i]); /* phy port to local port */
        rv = bcm_trunk_member_add(unit, trunk_id, &member);
        if (rv != BCM_E_NONE) {
          printf ("bcm_trunk_member_add with port %d failed: %d \n", out_port, rv);
          return rv;
        }
    }

    /* create FEC and send to LAG (instead of out-port) */
    BCM_GPORT_TRUNK_SET(trunk_gport, trunk_id); /* create a trunk gport and give this gport to create_l3_egress() instead of the dest-port */

    /*Create L3 egress*/
    bcm_l3_egress_t_init(&l3eg);
    sal_memcpy(l3eg.mac_addr, mac_l3_engress_add, 6);
    l3eg.vlan   = 300;
    l3eg.port   = trunk_gport;
    l3eg.flags  = BCM_L3_EGRESS_ONLY;
    rv = bcm_l3_egress_create(unit, 0, &l3eg, &l3egid);
    if (rv != BCM_E_NONE) {
      printf ("bcm_l3_egress_create failed \n");
      return rv;
    }


    /*POP three MPLS label MPLS3 = 3000, MPLS2 = 2000, MPLS1 = 1000,*/
    for (bos =0; bos < 2; bos++) {
        for (i = 1000; i <=3000 ; i += 1000 ) {

            bcm_mpls_tunnel_switch_t_init(&mpls_tunnel_info);
            mpls_tunnel_info.action = BCM_MPLS_SWITCH_ACTION_POP;
            mpls_tunnel_info.flags = BCM_MPLS_SWITCH_TTL_DECREMENT ; /* TTL decrement has to be present */
            /* To support ARAD there should be a different between BOS labels and none BOS labels. */
            if (!is_device_or_above(unit,ARAD_PLUS) && bos == 1) {
                mpls_tunnel_info.flags |= BCM_MPLS_SWITCH_NEXT_HEADER_IPV4|BCM_MPLS_SWITCH_NEXT_HEADER_IPV6;
            }
            index = i/1000;
            BCM_MPLS_INDEXED_LABEL_SET(mpls_tunnel_info.label, i+bos, index);
            mpls_tunnel_info.egress_label.label = 0x101; /* outgoing (egress) label */
            mpls_tunnel_info.egress_if = l3egid; /* FEC */
            rv = bcm_mpls_tunnel_switch_create(unit, &mpls_tunnel_info);
            if (rv != BCM_E_NONE) {
              printf ("bcm_mpls_tunnel_switch_create failed: %x \n", rv);
              return rv;
            }
            printf ("lif:%x \n", mpls_tunnel_info.tunnel_id);

        }
    }


    /* IPv4 route */
    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);
    l3rt.l3a_subnet = addr;
    l3rt.l3a_ip_mask = mask;
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = l3egid;
    rv = bcm_l3_route_add(unit, l3rt);
    if (rv != BCM_E_NONE) {
      printf ("bcm_l3_route_add failed: %x \n", rv);
      return rv;
    }

    /* IPv6 route */
    bcm_l3_route_t_init(l3rt);
    l3rt.l3a_flags = BCM_L3_IP6;
    sal_memcpy(&(l3rt.l3a_ip6_net),(l3a_ip6_addr),16);
    sal_memcpy(&(l3rt.l3a_ip6_mask),(l3a_ip6_mask),16);
    l3rt.l3a_vrf = vrf;
    l3rt.l3a_intf = l3egid;
    rv = bcm_l3_route_add(unit, l3rt);
    if (rv != BCM_E_NONE) {
      printf ("bcm_l3_route_add failed: %x \n", rv);
      return rv;
    }

    return rv;
}



struct cint_lag_hashing_Lag_result{
    uint32 lag_key[2]; /* Lag key results can save up to two keys for the two cores */
};

cint_lag_hashing_Lag_result cint_lag_hashing_lag_key_result;

int lag_hashing_lag_key_signal(int unit, int core_num, int save_compare)
{
    uint32 signal_value;
    int core_id;
    int rv = BCM_E_NONE;
    int keyAreTheSame = 1;

    for (core_id = 0; core_id < core_num; core_id++) {
        rv = dpp_dsig_read(unit, core_id, "IRPP", "FER", "LBP", "LAG_LB_Key", &signal_value, 1);

        printf ("LAG key of core %d is 0x%x \n",core_id,signal_value);

        if (save_compare == 0 /*save*/) {
            cint_lag_hashing_lag_key_result.lag_key[core_id] = signal_value;
        } else if (cint_lag_hashing_lag_key_result.lag_key[core_id] != signal_value) { /* compare */
            keyAreTheSame = 0;
            printf ("Both keys are equal failed: %x \n", rv);
        }
    }

    if ( save_compare == 1 && keyAreTheSame == 1) {
        printf ("Both keys are equal \n");
        rv = BCM_E_FAIL;
    }

    return rv;
}

