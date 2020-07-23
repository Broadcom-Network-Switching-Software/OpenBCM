/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Multi-Dev VPLS hierprotection~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_sand_multi_device_vpls_hier_protection.c
 * Purpose: Demo Hier protection VPLS under multi device system
 *
 * Multi-Device system require the ingress/egress configuration separately and relatively. 
 * This cint demo how to configurer the H-VPLS service under multi-device system.
 *  1. Create ARP and out-RIF configuration on egress device;
 *  2. Create In-RIF on ingress device
 *  3. Create PWE port and MPLS tunnel on core side device;
 *  4. Create AC port in access side device;
 *  5. Create failover on access/Core device;
 
 * 2 scenarios are tested:
 *  1. L2 flooding over H-VPLS;
 *  2. L2 unicast over H-VPLS;
 *
 *
 * Example:
 * Test Scenario 
 *
 *  PWEs flood to ACs 
 * tx 1 psrc=200 data=0x0000000000110000000000018847007d000a003e810a000000010101010203040506ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *  Received packets on unit 1 should be: 
 *  Source port: 200, Destination port: 201 
 *  Data: 0x000000010101010203040506ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
 *
 * cint
 * multi_dev_vpls_hier_protection_failover_set(1,2,1,1);
 * exit;
 *
 * cint
 * multi_dev_vpls_hier_protection_failover_set(1,2,0,1);
 * exit;
 *
 *  UNICAST Creating packet  
 * tx 1 psrc=201 data=0x0102030405060708090a0b0c8100000a91000014ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *  Received packets on unit 2 should be: 
 *  Source port: 201, Destination port: 200 
 *  Data: 0x000000000022000000000011810000c8884700fa000a00bb810a0102030405060708090a0b0c8100000a91000014ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
 *
 *  MultiCAST Creating packet  
 * tx 1 psrc=201 data=0x0000741590fc0708090a0b0c8100000a91000014ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *  Received packets on unit 2 should be: 
 *  Source port: 201, Destination port: 200 
 *  Data: 0x000000000022000000000011810000c8884700fa000a00bb810a0000741590fc0708090a0b0c8100000a91000014ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
 *
 * cint
 * multi_dev_vpls_hier_protection_failover_set(1,2,1,0);
 * exit;
 *
 * cint
 * multi_dev_vpls_hier_protection_failover_set(1,2,0,1);
 * exit;
 *
 *  UNICAST Creating packet  
 * tx 1 psrc=201 data=0x0102030405060708090a0b0c8100000a91000014ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *  Received packets on unit 2 should be: 
 *  Source port: 201, Destination port: 200 
 *  Data: 0x000000000022000000000011810000c8884700fa000a00bc210a0102030405060708090a0b0c8100000a91000014ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
 *
 *  MultiCAST Creating packet  
 * tx 1 psrc=201 data=0x0000c9020a6e0708090a0b0c8100000a91000014ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *  Received packets on unit 2 should be: 
 *  Source port: 201, Destination port: 200 
 *  Data: 0x000000000022000000000011810000c8884700fa000a00bc210a0000c9020a6e0708090a0b0c8100000a91000014ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
 *
 * cint
 * multi_dev_vpls_hier_protection_failover_set(1,2,1,1);
 * exit;
 *
 * cint
 * multi_dev_vpls_hier_protection_failover_set(1,2,0,0);
 * exit;
 *
 *  UNICAST Creating packet  
 * tx 1 psrc=201 data=0x0102030405060708090a0b0c8100000a91000014ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *  Received packets on unit 2 should be: 
 *  Source port: 201, Destination port: 200 
 *  Data: 0x000000000022000000000011810000c8884700faa00a00bb810a0102030405060708090a0b0c8100000a91000014ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
 *
 *  MultiCAST Creating packet  
 * tx 1 psrc=201 data=0x0000af8b2e9f0708090a0b0c8100000a91000014ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *  Received packets on unit 2 should be: 
 *  Source port: 201, Destination port: 200 
 *  Data: 0x000000000022000000000011810000c8884700faa00a00bb810a0000af8b2e9f0708090a0b0c8100000a91000014ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
 *
 * cint
 * multi_dev_vpls_hier_protection_failover_set(1,2,1,0);
 * exit;
 *
 * cint
 * multi_dev_vpls_hier_protection_failover_set(1,2,0,0);
 * exit;
 *
 *  UNICAST Creating packet  
 * tx 1 psrc=201 data=0x0102030405060708090a0b0c8100000a91000014ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *  Received packets on unit 2 should be: 
 *  Source port: 201, Destination port: 200 
 *  Data: 0x000000000022000000000011810000c8884700faa00a00bc210a0102030405060708090a0b0c8100000a91000014ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
 *
 *  MultiCAST Creating packet  
 * tx 1 psrc=201 data=0x00003e15b4cf0708090a0b0c8100000a91000014ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 *  Received packets on unit 2 should be: 
 *  Source port: 201, Destination port: 200 
 *  Data: 0x000000000022000000000011810000c8884700faa00a00bc210a00003e15b4cf0708090a0b0c8100000a91000014ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
 */


struct cint_multi_dev_vpls_hier_protection_info_s 
{
    int in_rif;                                /* access RIF */
    int out_rif;                            /* out RIF */
    int vrf;                                /*  VRF */
    bcm_mac_t my_mac;                       /* mac for Incoming L3 intf */
    bcm_mac_t next_hop_mac;                 /* next hop mac for outgoing station */
    bcm_ip_t vpn_id;                           /* dip */

    int primary_pwe_label;                  /*ingress label for primary pwe*/
    int backup_pwe_label;                  /*ingress label for backup pwe*/

    int primary_mpls_tunnel_label;          /*ingress label for primary tunnel*/
    int backup_mpls_tunnel_label;          /*ingress label for backup tunnel*/

    int primary_pwe_encap_label;            /*egress label for primary pwe*/
    int backup_pwe_encap_label;            /*egress label for backup pwe*/

    int primary_mpls_tunnel_encap_label;         /*egress label for primary tunnel*/
    int backup_mpls_tunnel_encap_label;         /*egress label for backup tunnel*/

    int arp_encap_id;                       /*Next hop intf ID*/ 
    int primary_pwe_encap_id;                /*primary pwe encap ID*/ 
    int backup_pwe_encap_id;                /*backup pwe encap ID*/ 

    int primary_tunnel_encap_id;         /*primary tunnel encap ID*/ 
    int backup_tunnel_encap_id;          /*backup tunnel encap ID*/  

    int pwe_failover_id;
    int pwe_egress_failover_id;

    int access_side_tunnel_failover_id;
    int core_side_tunnel_failover_id;
    int tunnel_egress_failover_id;

    int primary_pwe_fec_id;
    int backup_pwe_fec_id;

    int primary_tunnel_fec_id;
    int backup_tunnel_fec_id;

    int ac_outer_vlan;
    int ac_inner_vlan;

    int ac_local_port;                           
    int pwe_local_port;               
    int ac_encap_id;                           

    int ac_sys_port;                           
    int pwe_sys_port;               


};
  
  
  cint_multi_dev_vpls_hier_protection_info_s cint_hvpls_info=
 {
     /*
     * In-RIF,
     */
     100,
     /*
     * Out-RIF,
     */
     200,
 
     /*
     * vrf,
     */
     1,
     /*
     * my_mac | next_hop_mac 
     */
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x11},
     {0x00, 0x00, 0x00, 0x00, 0x00, 0x22},
 
     /*
     * vpn_id
     */
     6500 ,
 
     /*
     * primary_pwe_label | backup_pwe_label | primary_mpls_tunnel_label | backup_mpls_tunnel_label
     */
     1000,                 1010,               2000,                          2010,

     /*
     * primary_pwe_encap_label | backup_pwe_encap_label | primary_mpls_tunnel_encap_label | backup_mpls_tunnel_encap_label
     */
     3000,                       3010,                    4000,                               4010,

     /*
     * arp_encap_id
     */
     0x3000,

     /*
     * primary_pwe_encap_id | backup_pwe_encap_id | primary_tunnel_encap_id | backup_tunnel_encap_id
     */
     0x3001,                  0x3002,               0x3003,                   0x3004, 
 
     /*
     * pwe_failover_id   |  pwe_egress_failover_id  | access_side_tunnel_failover_id  | core_side_tunnel_failover_id| tunnel_egress_failover_id
     */
     0x4000,                    0,                          0,                              0,                              0,

     /*
     * primary_pwe_fec   | backup_pwe_fec   |  primary_tunnel_fec_id|  backup_tunnel_fec_id 
     */
     0xccce,                    0xcccf,           0xc000,                 0xc001,

     /*
     * ac_outer_vlan |  ac_inner_vlan 
     */
     10,                  20


 
 };
 
 /**
  * Initiation for H-VPLS service.
  * INPUT:
  *   access_unit- traffic incoming unit 
  *   core_unit- traffic ougtoing unit 
  *   ac_sys_port- traffic incoming port 
  *   pwe_sys_port- traffic outgoing port 
 */
int
multi_dev_vpls_hier_protection_init(
    int access_unit,
    int core_unit,
    int ac_sys_port,
    int pwe_sys_port)
{
    int rv;
    int local_port,is_local;

    /*
    * 0: Convert the sysport to local port
    */
    cint_hvpls_info.ac_sys_port = ac_sys_port;
    multi_dev_system_port_to_local(access_unit,ac_sys_port,&cint_hvpls_info.ac_local_port);
    cint_hvpls_info.pwe_sys_port = pwe_sys_port;
    multi_dev_system_port_to_local(core_unit,pwe_sys_port,&cint_hvpls_info.pwe_local_port);

    /*
    * create Vlan first, for LIF creation
    */
    rv = bcm_vlan_create(access_unit, cint_hvpls_info.in_rif);
    if (rv != BCM_E_NONE) {
     printf("Error, bcm_vlan_create\n");
     return rv;
    }


    /*
    * for JR2,Align the stpid as 0x9100
    */
    if (is_device_or_above(access_unit, JERICHO2)) {
     /* In jr2, we use the stag:0x9100, ctag:0x8100*/
     int jr2_tag_format_s_c = 16;
     bcm_port_tpid_class_t port_tpid_class;
     
     bcm_port_tpid_class_t_init(&port_tpid_class);
     port_tpid_class.port = cint_hvpls_info.ac_local_port;
     port_tpid_class.tpid1 = 0x8100;
     port_tpid_class.tpid2 = 0x9100;
     port_tpid_class.tag_format_class_id = jr2_tag_format_s_c;
     
     rv = bcm_port_tpid_class_set(access_unit, &port_tpid_class);
     if (rv != BCM_E_NONE)
     {
         printf("Error, in bcm_port_tpid_class_set\n");
         return rv;
     }

     rv = bcm_vlan_gport_add(access_unit, cint_hvpls_info.ac_outer_vlan, cint_hvpls_info.ac_local_port, 0);
     if (rv != BCM_E_NONE)
     {
         printf("Error in %s(): bcm_vlan_gport_add \n", proc_name);
         return rc;
     }

    }
    else
    {
        port_tpid_init(cint_hvpls_info.ac_local_port,1,1);
        rv = port_tpid_set(access_unit);
        if (rv != BCM_E_NONE) {
            printf("Error, port_tpid_set\n");
            return rv;
        }
    }
    
    egress_mc = 1;

    return rv;
}
 
/**
 * port init
 */
int
multi_dev_vpls_hier_protection_l2_port_configuration(
     int unit)
{
    int rv;
    bcm_vlan_port_t vlan_port;
    int rc;
    char *proc_name;

    proc_name = "multi_dev_vpls_hier_protection_port_configuration";
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = cint_hvpls_info.pwe_local_port;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.vsi = cint_hvpls_info.in_rif;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;

    rc = bcm_vlan_port_create(unit, vlan_port);
    if (rc != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_vlan_port_create\n", proc_name);
        return rc;
    }

    printf("%s(): port = %d, in_lif = 0x%08X\n", proc_name, vlan_port.port, vlan_port.vlan_port_id);
    rc = bcm_vlan_gport_add(unit, cint_hvpls_info.in_rif, cint_hvpls_info.pwe_local_port, 0);
    if (rc != BCM_E_NONE)
    {
        printf("Error in %s(): bcm_vlan_gport_add \n", proc_name);
        return rc;
    }

    return rv;
 
}
 
 /**
  * L3 intf init
  * INPUT:
  *   unit  - traffic incoming dev
 */
int
multi_dev_vpls_hier_protection_l3_intf_configuration(
   int unit)
{
    int rv;
    bcm_l3_intf_t l3_intf;
    sand_utils_l3_eth_rif_s eth_rif_structure;

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_intf_id = cint_hvpls_info.in_rif;
    rv = bcm_l3_intf_get(unit,&l3_intf);
    if (rv != BCM_E_NONE) {
     sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_hvpls_info.in_rif, 0, 0, cint_hvpls_info.my_mac, cint_hvpls_info.vrf);
     rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
     if (rv != BCM_E_NONE)
     {
      printf("Error, sand_utils_l3_eth_rif_create for in rif\n");
      return rv;
     }
    }

    bcm_l3_intf_t_init(&l3_intf);
    l3_intf.l3a_intf_id = cint_hvpls_info.out_rif;
    rv = bcm_l3_intf_get(unit,&l3_intf);
    if (rv != BCM_E_NONE) {
     sand_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_hvpls_info.out_rif, 0, 0, cint_hvpls_info.my_mac, cint_hvpls_info.vrf);
     rv = sand_utils_l3_eth_rif_create(unit, &eth_rif_structure);
     if (rv != BCM_E_NONE)
     {
      printf("Error, sand_utils_l3_eth_rif_create out rif\n");
      return rv;
     }

    }    

    return rv;
}
 
/**
  * ARP adding
  * INPUT:
  *   unit    - traffic incoming dev 
  */
int
multi_dev_vpls_hier_protection_tunnel_encapsulation_configuration(
     int unit)
{
    int rv;
    bcm_mpls_egress_label_t label_array[2];

    sand_utils_l3_arp_s arp_structure;

    /*
    * 1. ARP       
    */
    sand_utils_l3_arp_s_common_init(unit, 0, &arp_structure, cint_hvpls_info.arp_encap_id, 0, BCM_L3_FLAGS2_VLAN_TRANSLATION , cint_hvpls_info.next_hop_mac, cint_hvpls_info.out_rif);
    rv = sand_utils_l3_arp_create(unit, &arp_structure);
    if (rv != BCM_E_NONE)
    {
     printf("Error, create AC egress object ARP only\n");
     return rv;
    }
    cint_hvpls_info.arp_encap_id = arp_structure.l3eg_arp_id;

    /*
    * 2. mpls tunnel   
    */


    /* Create an MPLS Tunnel Egress failover ID */
    rv = bcm_failover_create(unit, BCM_FAILOVER_ENCAP, &cint_hvpls_info.tunnel_egress_failover_id);
    if (rv != BCM_E_NONE) {
     printf("Error, bcm_failover_create for MPLS Egress, rv - %d\n", rv);
     return rv;
    }
    printf("MPLS Tunnel egress Failover id: 0x%x\n", cint_hvpls_info.tunnel_egress_failover_id);




       /*
        * Backup tunnel
        */
    
    bcm_mpls_egress_label_t_init(&label_array[0]);
    bcm_mpls_egress_label_t_init(&label_array[1]);
    
    label_array[0].label = cint_hvpls_info.backup_mpls_tunnel_encap_label;
    label_array[0].flags = (BCM_MPLS_EGRESS_LABEL_WITH_ID|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);  
    if (!is_device_or_above(unit, JERICHO2))
    {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY | BCM_MPLS_EGRESS_LABEL_TTL_COPY;
    }
    label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_PROTECTION;
    label_array[0].egress_failover_id = cint_hvpls_info.tunnel_egress_failover_id;

    label_array[0].tunnel_id = cint_hvpls_info.backup_tunnel_encap_id;
    label_array[0].l3_intf_id = cint_hvpls_info.arp_encap_id; 
    label_array[0].encap_access = bcmEncapAccessTunnel2; 
    
    /** Create the mpls tunnel with one call.*/
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, label_array);
    if (rv != BCM_E_NONE)
    {
      printf("Error, in bcm_mpls_tunnel_initiator_create\n");
      return rv;
    }  
    cint_hvpls_info.backup_tunnel_encap_id = label_array[0].tunnel_id;       
        



        /*
         * Primary tunnel
         */

    bcm_mpls_egress_label_t_init(&label_array[0]);
    bcm_mpls_egress_label_t_init(&label_array[1]);

    label_array[0].label = cint_hvpls_info.primary_mpls_tunnel_encap_label;
    label_array[0].flags = (BCM_MPLS_EGRESS_LABEL_WITH_ID|BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT);  
    if (!is_device_or_above(unit, JERICHO2))
    {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY | BCM_MPLS_EGRESS_LABEL_TTL_COPY;
    }
    label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_PROTECTION;
    label_array[0].egress_failover_id = cint_hvpls_info.tunnel_egress_failover_id;
    label_array[0].egress_failover_if_id = cint_hvpls_info.backup_tunnel_encap_id;


    label_array[0].tunnel_id = cint_hvpls_info.primary_tunnel_encap_id;
    label_array[0].l3_intf_id = cint_hvpls_info.arp_encap_id; 
    label_array[0].encap_access = bcmEncapAccessTunnel2; 

    /** Create the mpls tunnel with one call.*/
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, label_array);
    if (rv != BCM_E_NONE)
    {
      printf("Error, in bcm_mpls_tunnel_initiator_create\n");
      return rv;
    }  
    cint_hvpls_info.primary_tunnel_encap_id = label_array[0].tunnel_id;  

 
    return rv;
 }
 
 
 /**
  * MPLS termination adding
  * INPUT:
  *   in_port    - traffic incoming port 
  *   out_port - traffic outgoing port
 */
  int
  multi_dev_vpls_hier_protection_tunnel_termination_configuration(
     int unit)
 {
     bcm_mpls_tunnel_switch_t entry;
     int rv = 0;

     /*
      * Primary tunnel
      */

     bcm_mpls_tunnel_switch_t_init(&entry);
     
     entry.action = BCM_MPLS_SWITCH_ACTION_POP;
     entry.label = cint_hvpls_info.primary_mpls_tunnel_label;
     entry.vpn = cint_hvpls_info.vpn_id;
 
     rv = bcm_mpls_tunnel_switch_create(unit, &entry);
     if (rv != BCM_E_NONE)
     {
         printf("Error, in bcm_mpls_tunnel_switch_create\n");
         return rv;
     }

     /*
      * Backup tunnel
      */

     bcm_mpls_tunnel_switch_t_init(&entry);
     
     entry.action = BCM_MPLS_SWITCH_ACTION_POP;
     entry.label = cint_hvpls_info.backup_mpls_tunnel_label;
     entry.vpn = cint_hvpls_info.vpn_id;
 
     rv = bcm_mpls_tunnel_switch_create(unit, &entry);
     if (rv != BCM_E_NONE)
     {
         printf("Error, in bcm_mpls_tunnel_switch_create\n");
         return rv;
     }

   
     return rv;
 }

 /**
  * PWE encapsulation
  * INPUT:
  *   unit    - unit
  *   mpls_port_encap - encapsulation information
 */
int
multi_dev_vpls_hier_protection_pwe_port_encap(
      int unit)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;
    char *proc_name = "multi_dev_vpls_hier_protection_pwe_port_encap";


    /* create egress failover id for PWE */
    rv = bcm_failover_create(unit, BCM_FAILOVER_ENCAP, &cint_hvpls_info.pwe_egress_failover_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for PWE Egress, rv - %d\n", rv);
        return rv;
    } 
    printf("PWE egress Failover id: 0x%x\n", cint_hvpls_info.pwe_egress_failover_id);


    /*
    * Backup PWE
    */

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags = BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL | BCM_MPLS_PORT_ENCAP_WITH_ID;
    mpls_port.flags2 = BCM_MPLS_PORT2_EGRESS_ONLY;
    mpls_port.encap_id = cint_hvpls_info.backup_pwe_encap_id;
    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, cint_hvpls_info.backup_pwe_encap_id);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
    mpls_port.egress_label.label = cint_hvpls_info.backup_pwe_encap_label;
    mpls_port.egress_label.encap_access = bcmEncapAccessTunnel1;

    mpls_port.egress_label.ttl = 10;
    if (is_device_or_above(unit, JERICHO2)) {
        mpls_port.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        mpls_port.egress_label.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    } else {
        mpls_port.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_SET;
    }

    mpls_port.flags2 |= BCM_MPLS_PORT2_EGRESS_PROTECTION;
    mpls_port.egress_failover_id = cint_hvpls_info.pwe_egress_failover_id;



    rv = bcm_mpls_port_add(unit, 0, mpls_port);
    if (rv != BCM_E_NONE)
    {
      printf("Error(%d), in bcm_mpls_port_add encapsulation\n", rv);
      return rv;
    }

    BCM_L3_ITF_SET(cint_hvpls_info.backup_pwe_encap_id, BCM_L3_ITF_TYPE_LIF, mpls_port.encap_id);



    /*
    * Primary PWE
    */

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags = BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL | BCM_MPLS_PORT_ENCAP_WITH_ID;
    mpls_port.flags2 = BCM_MPLS_PORT2_EGRESS_ONLY;
    mpls_port.encap_id = cint_hvpls_info.primary_pwe_encap_id;
    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, cint_hvpls_info.primary_pwe_encap_id);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
    mpls_port.egress_label.label = cint_hvpls_info.primary_pwe_encap_label;
    mpls_port.egress_label.encap_access = bcmEncapAccessTunnel1;

    mpls_port.egress_label.ttl = 10;
    if (is_device_or_above(unit, JERICHO2)) {
        mpls_port.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        mpls_port.egress_label.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    } else {
        mpls_port.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_SET;
    }

    mpls_port.flags2 |= BCM_MPLS_PORT2_EGRESS_PROTECTION;
    mpls_port.egress_failover_id = cint_hvpls_info.pwe_egress_failover_id;
    mpls_port.egress_failover_port_id = cint_hvpls_info.backup_pwe_encap_id;


    rv = bcm_mpls_port_add(unit, 0, mpls_port);
    if (rv != BCM_E_NONE)
    {
      printf("Error(%d), in bcm_mpls_port_add encapsulation\n", rv);
      return rv;
    }

    BCM_L3_ITF_SET(cint_hvpls_info.primary_pwe_encap_id, BCM_L3_ITF_TYPE_LIF, mpls_port.encap_id);

    return rv;
  }
 
 /**
  * PWE termination
  * INPUT:
  *   unit    - unit
 */
int
multi_dev_vpls_hier_protection_pwe_port_term(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t mpls_port;
    char *proc_name = "multi_dev_vpls_hier_protection_pwe_port_term";
    int i = 0;


    for (i = 0;i < 2; i++) 
    {
        bcm_mpls_port_t_init(&mpls_port);
        mpls_port.flags = BCM_MPLS_PORT_EGRESS_TUNNEL|BCM_MPLS_PORT_WITH_ID;
        /* In JR2,BCM_MPLS_PORT_NETWORK is not used, we use network_group_id directlt. */
        mpls_port.flags |= is_device_or_above(unit, JERICHO2) ? 0 : BCM_MPLS_PORT_NETWORK;
        /* In JR2,BCM_MPLS_PORT_ENCAP_WITH_ID can't be set with INGRESS_ONLY. */
        mpls_port.flags |= is_device_or_above(unit, JERICHO2) ? 0 : BCM_MPLS_PORT_ENCAP_WITH_ID;
        mpls_port.flags2 = BCM_MPLS_PORT2_INGRESS_ONLY; /* Indicate we only create PWE-In-LIF allocation */
       
        mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL; 
        if (is_device_or_above(unit, JERICHO2)) {
            /* Jr2 requires subtype of mpls_port_id.*/
            BCM_GPORT_SUB_TYPE_LIF_SET(mpls_port.mpls_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY, 
                                        (i == 0)?cint_hvpls_info.backup_pwe_encap_id:cint_hvpls_info.primary_pwe_encap_id);
        } else {
            mpls_port.encap_id = (i == 0)?cint_hvpls_info.backup_pwe_encap_id:cint_hvpls_info.primary_pwe_encap_id;
            mpls_port.network_group_id = 1; 
        }
        
        BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, mpls_port.mpls_port_id);
        mpls_port.port = BCM_GPORT_INVALID; /* Port is not required for PWE-InLIF */
        mpls_port.egress_tunnel_if = 0; /* PWE In-LIF in this case do not care about MPLS egress_if. It is not part of learning information. */
 
        BCM_GPORT_FORWARD_PORT_SET(mpls_port.failover_port_id, cint_hvpls_info.primary_pwe_fec_id & 0x7FFF); /* Learning - 15 Bits PWE FEC */    
        mpls_port.match_label = (i == 0)?cint_hvpls_info.backup_pwe_label:cint_hvpls_info.primary_pwe_label;        
        rv = bcm_mpls_port_add(unit, cint_hvpls_info.vpn_id, &mpls_port);
        if (rv != BCM_E_NONE) {
            printf("%s:Error, bcm_mpls_port_add\n",proc_name);
            return rv;
        }

    }
    



    return rv;
}

/**
   * FEC configuration
   * INPUT:
   *   unit    - unit
  */
 int
 multi_dev_vpls_hier_protection_fec_add(
     int unit,
     int access_side)
 {
    int rv = BCM_E_NONE;
    int flag = 0,failover_id=0;
    int *pfailover;

    flag = BCM_FAILOVER_FEC | (is_device_or_above(unit, JERICHO2) ? BCM_FAILOVER_FEC_2ND_HIERARCHY : 0); 
    /*JR2 and JR have different failover ID range for 2nd level FEC*/
    pfailover = access_side?&cint_hvpls_info.access_side_tunnel_failover_id:&cint_hvpls_info.core_side_tunnel_failover_id;
    /*Tunnel FEC*/
    /* create failover id for tunnel */
    rv = bcm_failover_create(unit, flag, pfailover);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for tunnel FEC, rv - %d\n", rv); 
        return rv;
    }


    /**JR2: l3_egress_create must be used to create FEC entris.*/
    create_l3_egress_s l3_egress;
    sal_memset(&l3_egress, 0, sizeof (l3_egress));
    /* this is secondary*/
    l3_egress.failover_id = *pfailover;
    l3_egress.failover_if_id = 0;
    l3_egress.out_tunnel_or_rif = 0;
    l3_egress.out_gport = BCM_GPORT_INVALID;

    BCM_L3_ITF_SET(l3_egress.out_tunnel_or_rif,BCM_L3_ITF_TYPE_LIF,  cint_hvpls_info.backup_tunnel_encap_id);
    l3_egress.out_gport  = cint_hvpls_info.pwe_sys_port;
    l3_egress.l3_flags = BCM_L3_CASCADED;

    rv = l3__egress_only_fec__create(unit, &l3_egress);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_fec__create 1\n"); 
        return rv;
    }

    cint_hvpls_info.backup_tunnel_fec_id = l3_egress.fec_id;

    /* this is primery*/
    l3_egress.out_tunnel_or_rif = 0;
    l3_egress.failover_if_id = cint_hvpls_info.backup_tunnel_fec_id;

    BCM_L3_ITF_SET(l3_egress.out_tunnel_or_rif,BCM_L3_ITF_TYPE_LIF,  cint_hvpls_info.primary_tunnel_encap_id);

    rv = l3__egress_only_fec__create(unit, &l3_egress);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_fec__create 1\n"); 
        return rv;
    } 

    cint_hvpls_info.primary_tunnel_fec_id = l3_egress.fec_id;

    /*PWE FEC*/

    /* create failover id for PWE */
    rv = bcm_failover_create(unit, BCM_FAILOVER_FEC|BCM_FAILOVER_WITH_ID, &cint_hvpls_info.pwe_failover_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for PWE FEC, rv - %d\n", rv); 
        return rv;
    }


    /**JR2: l3_egress_create must be used to create FEC entris.*/
    sal_memset(&l3_egress, 0, sizeof (l3_egress));
    /* this is secondary*/
    l3_egress.failover_id = cint_hvpls_info.pwe_failover_id;
    l3_egress.failover_if_id = 0;
    l3_egress.out_tunnel_or_rif = 0;
    l3_egress.out_gport = BCM_GPORT_INVALID;

    BCM_L3_ITF_SET(l3_egress.out_tunnel_or_rif,BCM_L3_ITF_TYPE_LIF,  cint_hvpls_info.backup_pwe_encap_id);
    BCM_GPORT_FORWARD_PORT_SET(l3_egress.out_gport, BCM_L3_ITF_VAL_GET(cint_hvpls_info.primary_tunnel_fec_id));

    l3_egress.allocation_flags = BCM_L3_WITH_ID;
    l3_egress.l3_flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
    /*in JR, it only support up to 15 bits wide FEC ID in learning information*/
    l3_egress.fec_id = is_device_or_above(unit, JERICHO2)?cint_hvpls_info.backup_pwe_fec_id:(cint_hvpls_info.backup_pwe_fec_id & 0x7FFF);
    rv = l3__egress_only_fec__create(unit, &l3_egress);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_fec__create 1\n"); 
        return rv;
    }

    /* this is primery*/
    l3_egress.out_tunnel_or_rif = 0;
    l3_egress.failover_if_id = is_device_or_above(unit, JERICHO2)?cint_hvpls_info.backup_pwe_fec_id:(cint_hvpls_info.backup_pwe_fec_id & 0x7FFF)`;

    BCM_L3_ITF_SET(l3_egress.out_tunnel_or_rif,BCM_L3_ITF_TYPE_LIF,  cint_hvpls_info.primary_pwe_encap_id);
    BCM_GPORT_FORWARD_PORT_SET(l3_egress.out_gport, BCM_L3_ITF_VAL_GET(cint_hvpls_info.primary_tunnel_fec_id));
    l3_egress.l3_flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
    l3_egress.fec_id = is_device_or_above(unit, JERICHO2)?cint_hvpls_info.primary_pwe_fec_id:(cint_hvpls_info.primary_pwe_fec_id & 0x7FFF);

    rv = l3__egress_only_fec__create(unit, &l3_egress);
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress_only_fec__create 1\n"); 
        return rv;
    } 

    return rv;
 }
 
  /**
   * fabric mc set
   * INPUT:
   *   unit    - unit
  */
 int
 multi_dev_vpls_hier_protection_fabric_mc_set(
    int access_unit,
    int core_unit,
    int ac_sys_port,
    int pwe_sys_port) 
{
    int rv = 0;
    int mc_id = cint_hvpls_info.vpn_id;
    int dest_fap_id[2] = {0,0};
    int fabric_unit = 0;
    int is_fabric_supported = 1;
    bcm_gport_t  gport;
 
    for (fabric_unit = 0 ; fabric_unit < 3; fabric_unit++) {
        if((fabric_unit != access_unit) && (fabric_unit != core_unit)) 
        {
            break;
        }
    }
    rv = bcm_multicast_create(fabric_unit, BCM_MULTICAST_WITH_ID, &mc_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }

    rv = bcm_stk_sysport_gport_get(access_unit, ac_sys_port, &gport);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stk_sysport_gport_get \n");
        return rv;
    }
    dest_fap_id[0] = BCM_GPORT_MODPORT_MODID_GET(gport);

    rv = bcm_stk_sysport_gport_get(core_unit, pwe_sys_port, &gport);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stk_sysport_gport_get \n");
        return rv;
    }
    dest_fap_id[1] = BCM_GPORT_MODPORT_MODID_GET(gport);


    if (is_device_or_above(access_unit, JERICHO2)) {
        is_fabric_supported = *(dnxc_data_get(access_unit, "fabric", "general", "blocks_exist", NULL));
    }
	
    if (is_fabric_supported) {
        rv = bcm_fabric_multicast_set(fabric_unit, mc_id, 0, 2, dest_fap_id);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_fabric_multicast_set \n");
            return rv;
        }
    }

    return rv;
} 
 
 /**
  * FEC configuration
  * INPUT:
  *   unit    - unit
  */
int
multi_dev_vpls_hier_protection_vswitch_flood_group_set(
      int unit,
      int is_access_side)
{
    int rv = BCM_E_NONE;
    bcm_multicast_replication_t reps[5] = {{0}};
    bcm_multicast_t mc_id;
    uint32 flags = 0;

 
    mc_id = cint_hvpls_info.vpn_id;
    bcm_multicast_replication_t_init(reps);
    bcm_multicast_replication_t_init(reps + 1);
    bcm_multicast_replication_t_init(reps + 2);
    bcm_multicast_replication_t_init(reps + 3);
    bcm_multicast_replication_t_init(reps + 4);

    /* AC outlif 1 */
    BCM_GPORT_LOCAL_SET(reps[0].port, cint_hvpls_info.ac_local_port);
    reps[0].encap1 = cint_hvpls_info.ac_encap_id;

    if (is_device_or_above(unit, JERICHO2)) {
        /** If more than one encapsulation pointers for the same packet exist, PPMC must be used in JR2.*/
        int pwe_if_primary, pwe_if_protected;
        bcm_if_t cuds[2];
        bcm_if_t rep_index;


        /* PWE outlif primery with second CUD for MPLS tunnel primery */
        BCM_L3_ITF_SET(cuds[0], BCM_L3_ITF_TYPE_LIF, cint_hvpls_info.primary_pwe_encap_id);
        BCM_L3_ITF_SET(cuds[1], BCM_L3_ITF_TYPE_LIF, cint_hvpls_info.primary_tunnel_encap_id);
        rv = bcm_multicast_encap_extension_create(unit, 0, &rep_index, 2, cuds);
        if (rv != BCM_E_NONE) {
            printf("Error in 2nd copy, bcm_multicast_encap_extension_create. cud0 = 0x%08X, cud1 = 0x%08X\n", cuds[0],cuds[1]);
            return rv;
        }
        reps[1].encap1 = rep_index;
        BCM_GPORT_LOCAL_SET(reps[1].port, cint_hvpls_info.pwe_local_port);
        /* PWE outlif primery with second CUD for MPLS tunnel protected */
        BCM_L3_ITF_SET(cuds[0], BCM_L3_ITF_TYPE_LIF, cint_hvpls_info.primary_pwe_encap_id);
        BCM_L3_ITF_SET(cuds[1], BCM_L3_ITF_TYPE_LIF, cint_hvpls_info.backup_tunnel_encap_id);
        rv = bcm_multicast_encap_extension_create(unit, 0, &rep_index, 2, cuds);
        if (rv != BCM_E_NONE) {
         printf("Error in 3rd copy, bcm_multicast_encap_extension_create. cud0 = 0x%08X, cud1 = 0x%08X\n", cuds[0],cuds[1]);
         return rv;
        }
        reps[2].encap1 = rep_index;
        BCM_GPORT_LOCAL_SET(reps[2].port, cint_hvpls_info.pwe_local_port);

        /* PWE outlif protected with second CUD for MPLS tunnel primery */
        BCM_L3_ITF_SET(cuds[0], BCM_L3_ITF_TYPE_LIF, cint_hvpls_info.backup_pwe_encap_id);
        BCM_L3_ITF_SET(cuds[1], BCM_L3_ITF_TYPE_LIF, cint_hvpls_info.primary_tunnel_encap_id);
        rv = bcm_multicast_encap_extension_create(unit, 0, &rep_index, 2, cuds);
        if (rv != BCM_E_NONE) {
         printf("Error in 4th copy, bcm_multicast_encap_extension_create. cud0 = 0x%08X, cud1 = 0x%08X\n", cuds[0],cuds[1]);
         return rv;
        }
        reps[3].encap1 = rep_index;
        BCM_GPORT_LOCAL_SET(reps[3].port, cint_hvpls_info.pwe_local_port);


        /* PWE outlif protected with second CUD for MPLS tunnel protected */
        BCM_L3_ITF_SET(cuds[0], BCM_L3_ITF_TYPE_LIF, cint_hvpls_info.backup_pwe_encap_id);
        BCM_L3_ITF_SET(cuds[1], BCM_L3_ITF_TYPE_LIF, cint_hvpls_info.backup_tunnel_encap_id);
        rv = bcm_multicast_encap_extension_create(unit, 0, &rep_index, 2, cuds);
        if (rv != BCM_E_NONE) {
         printf("Error in 5th copy, bcm_multicast_encap_extension_create. cud0 = 0x%08X, cud1 = 0x%08X\n", cuds[0],cuds[1]);
         return rv;
        }
        reps[4].encap1 = rep_index;
        BCM_GPORT_LOCAL_SET(reps[4].port, cint_hvpls_info.pwe_local_port);


        flags = BCM_MULTICAST_EGRESS_GROUP;

    } else {
        /* PWE outlif primery with second CUD for MPLS tunnel primery */
        BCM_GPORT_LOCAL_SET(reps[1].port, cint_hvpls_info.pwe_local_port);
        reps[1].encap1 = BCM_L3_ITF_VAL_GET(cint_hvpls_info.primary_pwe_encap_id);
        reps[1].encap2 = BCM_L3_ITF_VAL_GET(cint_hvpls_info.primary_tunnel_encap_id);
        reps[1].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID;

        /* PWE outlif primery with second CUD for MPLS tunnel protected */
        BCM_GPORT_LOCAL_SET(reps[2].port, cint_hvpls_info.pwe_local_port);
        reps[2].encap1 = BCM_L3_ITF_VAL_GET(cint_hvpls_info.primary_pwe_encap_id);
        reps[2].encap2 = BCM_L3_ITF_VAL_GET(cint_hvpls_info.backup_tunnel_encap_id);
        reps[2].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID;

        /* PWE outlif protected with second CUD for MPLS tunnel primery */
        BCM_GPORT_LOCAL_SET(reps[3].port, cint_hvpls_info.pwe_local_port);
        reps[3].encap1 = BCM_L3_ITF_VAL_GET(cint_hvpls_info.backup_pwe_encap_id);
        reps[3].encap2 = BCM_L3_ITF_VAL_GET(cint_hvpls_info.primary_tunnel_encap_id);
        reps[3].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID;

        /* PWE outlif protected with second CUD for MPLS tunnel protected */
        BCM_GPORT_LOCAL_SET(reps[4].port, cint_hvpls_info.pwe_local_port);
        reps[4].encap1 = BCM_L3_ITF_VAL_GET(cint_hvpls_info.backup_pwe_encap_id);
        reps[4].encap2 = BCM_L3_ITF_VAL_GET(cint_hvpls_info.backup_tunnel_encap_id);
        reps[4].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID;

        flags = 0;
    }

    if (is_access_side) {
       rv = bcm_multicast_set(unit, mc_id, flags, 1, reps);
    } else
    {
       rv = bcm_multicast_set(unit, mc_id, flags, 4, &reps[1]);
    }
    if (rv != BCM_E_NONE) {
    printf("Error, in bcm_multicast_set\n");
    return rv;
    }

    return rv;
}
 
 /*
  * create vlan-port
  *   vlan-port: is Logical interface identified by (port-vlan-vlan). 
  */
 int
 multi_dev_vpls_hier_protection_add_access_port(int unit){
    int rv;  
    bcm_vlan_port_t vlan_port_1;

    /* add port, according to port_vlan_vlan */
    bcm_vlan_port_t_init(&vlan_port_1);


    /* set port attribures, key <port-vlan-vlan>*/
    vlan_port_1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
    vlan_port_1.port = cint_hvpls_info.ac_local_port;
    vlan_port_1.match_vlan = cint_hvpls_info.ac_outer_vlan;
    vlan_port_1.match_inner_vlan = cint_hvpls_info.ac_inner_vlan; 
    vlan_port_1.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : cint_hvpls_info.ac_outer_vlan;
    vlan_port_1.egress_inner_vlan = is_device_or_above(unit, JERICHO2) ? 0 : cint_hvpls_info.ac_inner_vlan;

    rv = bcm_vlan_port_create(unit, &vlan_port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }


    cint_hvpls_info.ac_encap_id = vlan_port_1.encap_id;

    /* In advanced vlan translation mode, bcm_vlan_port_create does not create ingress / egress
       action mapping. This is here to compensate. */
    if (advanced_vlan_translation_mode || is_device_or_above(unit, JERICHO2)) {
        rv = vlan_translation_vlan_port_create_to_translation(unit, &vlan_port_1);
        if (rv != BCM_E_NONE) {
            printf("Error: vlan_translation_vlan_port_create_to_translation\n");
        }
    }


    rv = bcm_vswitch_port_add(unit, cint_hvpls_info.vpn_id, vlan_port_1.vlan_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }
    /* update Multicast to have the added port  */

    rv = multicast__vlan_port_add(unit, cint_hvpls_info.vpn_id, cint_hvpls_info.ac_sys_port, vlan_port_1.vlan_port_id, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, multicast__vlan_port_add\n");
        return rv;
    }

    rv = fec_id_alignment_field_wa_add(unit, cint_hvpls_info.ac_local_port, BCM_L3_ITF_VAL_GET(cint_hvpls_info.primary_pwe_fec_id));
    if (rv != BCM_E_NONE)
    {
        printf("Error, fec_id_alignment_field_wa_entry_add\n");
        return rv;
    }

    return rv;
 }


 /**
  * access side configuration
  * INPUT:
  *   in_port    - traffic incoming port 
  *   out_sys_port - traffic outgoing port
 */
int
multi_dev_vpls_hier_protection_access_side_configuration(
     int unit)
{
    int rv;

    rv = multi_dev_vpls_hier_protection_general_configuration(unit,1);
    if (rv != BCM_E_NONE)
    {   
        printf("Error, multi_dev_vpls_hier_protection_general_configuration\n");
        return rv;
    }

    rv = multi_dev_vpls_hier_protection_add_access_port(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multi_dev_vpls_hier_protection_add_access_port\n");
        return rv;
    }

    rv = multi_dev_vpls_hier_protection_vswitch_flood_group_set(unit, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multi_dev_vpls_hier_protection_vswitch_flood_group_set\n");
        return rv;
    }

    return rv;
 }


 /**
  * FEC and flood configuration should be added in
  * both device.
  * INPUT:
  *   in_port    - traffic incoming port 
  *   out_sys_port - traffic outgoing port
 */
int
multi_dev_vpls_hier_protection_general_configuration(
     int unit,
     int access_side)
{
    int rv;

    rv = mpls_port__vswitch_vpls_vpn_create__set(unit, cint_hvpls_info.vpn_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in vswitch_vpls_vpn_create\n");
        return rv;
    }

    rv = multi_dev_vpls_hier_protection_fec_add(unit,access_side);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multi_dev_vpls_hier_protection_fec_add\n");
        return rv;
    }

     return rv;
 }


/**
 * core side configuration
 * INPUT:
 *   in_port    - traffic incoming port 
 */
int
multi_dev_vpls_hier_protection_core_side_configuration(
      int unit)
{
    int rv;

    rv = multi_dev_vpls_hier_protection_l2_port_configuration(unit);
    if (rv != BCM_E_NONE)
    {
     printf("Error, multi_dev_vpls_hier_protection_l2_port_configuration\n");
     return rv;
    }

    rv = multi_dev_vpls_hier_protection_general_configuration(unit,0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multi_dev_vpls_hier_protection_general_configuration\n");
        return rv;
    }

    rv = multi_dev_vpls_hier_protection_l3_intf_configuration(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multi_dev_vpls_hier_protection_l3_intf_configuration\n");
        return rv;
    }


    rv = multi_dev_vpls_hier_protection_tunnel_encapsulation_configuration(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multi_dev_vpls_hier_protection_tunnel_encapsulation_configuration\n");
        return rv;
    }

    rv = multi_dev_vpls_hier_protection_tunnel_termination_configuration(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multi_dev_vpls_hier_protection_tunnel_termination_configuration\n");
        return rv;
    }


    rv = multi_dev_vpls_hier_protection_pwe_port_encap(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multi_dev_vpls_hier_protection_pwe_port_encap\n");
        return rv;
    }


    rv = multi_dev_vpls_hier_protection_pwe_port_term(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multi_dev_vpls_hier_protection_tunnel_encapsulation_configuration\n");
        return rv;
    }

    rv = multi_dev_vpls_hier_protection_vswitch_flood_group_set(unit, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multi_dev_vpls_hier_protection_vswitch_flood_group_set\n");
        return rv;
    }

    return rv;
}


/*
 * Failover Set
 */
int 
multi_dev_vpls_hier_protection_failover_set(
    int access_unit,
    int core_unit,
    uint8 is_pwe,
    int enable)
{
    int rv;
    bcm_failover_t access_side_failover_id,core_side_failover_id, egress_failover_id;
    int egress_enable;
    int i = 0, unit = 0;

    if (is_pwe) {
       access_side_failover_id = core_side_failover_id = cint_hvpls_info.pwe_failover_id; 
    }
    else {
       access_side_failover_id= cint_hvpls_info.access_side_tunnel_failover_id;
       core_side_failover_id = cint_hvpls_info.core_side_tunnel_failover_id;
    }

    rv = bcm_failover_set(access_unit, access_side_failover_id, enable);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_failover_set failover_id:  0x%08x \n", failover_id);
        return rv;
    }

    rv = bcm_failover_set(core_unit, core_side_failover_id, enable);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_failover_set failover_id:  0x%08x \n", failover_id);
        return rv;
    }


    egress_failover_id = is_pwe? cint_hvpls_info.pwe_egress_failover_id: cint_hvpls_info.tunnel_egress_failover_id;
    egress_enable = enable;
    rv = bcm_failover_set(core_unit, egress_failover_id, egress_enable);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_failover_set failover_id:  0x%08x \n", egress_failover_id);
        return rv;
    }

    return rv;
 }


 
 /**
  * Main entrance for HVPLS on multi-device
  * INPUT:
  *   ac_sys_port    - traffic incoming port 
  *   pwe_sys_port - traffic outgoing port
 */
int
multi_dev_vpls_hier_protection_example(
    int access_unit,
    int core_unit,
    int ac_sys_port,
    int pwe_sys_port)
{
    int rv;

    rv = multi_dev_vpls_hier_protection_init(access_unit,core_unit,ac_sys_port,pwe_sys_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multi_dev_vpls_hier_protection_init\n");
        return rv;
    }

    rv = multi_dev_vpls_hier_protection_access_side_configuration(access_unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multi_dev_vpls_hier_protection_access_side_configuration\n");
        return rv;
    }

    rv = multi_dev_vpls_hier_protection_core_side_configuration(core_unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multi_dev_vpls_hier_protection_core_side_configuration\n");
        return rv;
    }

    rv = multi_dev_vpls_hier_protection_fabric_mc_set(access_unit,core_unit,ac_sys_port,pwe_sys_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, multi_dev_vpls_hier_protection_fabric_mc_set\n");
        return rv;
    }


    return rv;
 }
 
 



