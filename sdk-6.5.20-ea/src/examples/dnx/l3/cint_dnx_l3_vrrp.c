/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 */


/*
 * This function configure IPv4 and IPv6 L3 forwarding entries using the same VSI.
 * The My MAC process should be done using the VRRP where each of the IP version should use a different VRID
 */
int
l3_vrrp_protocol_field_test(
 int unit,
 int in_port,
 int out_port)
{
 int rv;
 int intf_in = 15;           /* Incoming packet ETH-RIF */
 int intf_out = 100;         /* Outgoing packet ETH-RIF */

 int vrf = 1;
 int encap_id = 0x1384;         /* ARP-Link-layer (needs to be higher than 8192 for Jer Plus) */
 bcm_mac_t intf_in_mac_address = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };     /* my-MAC */
 bcm_mac_t intf_out_mac_address = { 0x00, 0x12, 0x34, 0x56, 0x78, 0x9a };    /* my-MAC */
 bcm_mac_t arp_next_hop_mac = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };        /* next_hop_mac */
 bcm_gport_t gport;
 l3_ingress_intf ing_rif;
 l3_ingress_intf_init(&ing_rif);
 uint32 route = 0x7fffff00;
 uint32 host = 0x7fffff02;
 uint32 mask = 0xfffffff0;
 bcm_ip6_t ipv6_route = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x10 };
 bcm_ip6_t ipv6_mask =  { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0 };
 l3_ingress_intf_init(&ingress_rif);
 uint32 flags2 = 0;
 int vlan = 100;
 int fec;
 int vrid_4 = 7;
 int vrid_6 = 8;
 /*
  * used to pass fec in host.l3a_intf
  */
 int encoded_fec;

 char *err_proc_name;
 char *proc_name;

 proc_name = "l3_vrrp_protocol_field_test";

 get_first_fec_in_range_which_not_in_ecmp_range(unit,0,&fec);

 /*
  * 1. Set In-Port to In ETh-RIF
  */
 rv = in_port_intf_set(unit, in_port, intf_in);
 if (rv != BCM_E_NONE)
 {
     printf("%s(): Error, in_port_intf_set intf_in\n",proc_name);
     return rv;
 }
 /*
  * 2. Set Out-Port default properties, in case of ARP+AC no need
  */
 rv = out_port_set(unit, out_port);
 if (rv != BCM_E_NONE)
 {
     printf("%s(): Error, out_port_set intf_out out_port %d\n",proc_name,out_port);
     return rv;
 }

 /*
  * 3. Create ETH-RIF and set its properties
  */
 rv = intf_eth_rif_create(unit, intf_in, intf_in_mac_address);
 if (rv != BCM_E_NONE)
 {
     printf("%s(): Error, intf_eth_rif_create intf_in %d\n",proc_name,intf_in);
     return rv;
 }
 rv = intf_eth_rif_create(unit, intf_out, intf_out_mac_address);
 if (rv != BCM_E_NONE)
 {
     printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
     return rv;
 }
 /*
  * 4. Set Incoming ETH-RIF properties
  */
 ing_rif.vrf = vrf;
 ing_rif.intf_id = intf_in;
 rv = intf_ingress_rif_set(unit, &ing_rif);
 if (rv != BCM_E_NONE)
 {
     printf("%s(): Error, intf_eth_rif_create intf_out\n",proc_name);
     return rv;
 }

 /*
  * 5. Create ARP and set its properties
  */
 rv = create_arp_plus_ac_type(unit, 0, arp_next_hop_mac, &encap_id, vlan);
 if (rv != BCM_E_NONE)
 {
     printf("%s(): Error, create_arp_plus_ac_type\n",proc_name);
     return rv;
 }


 /*
  * 6. Create FEC and set its properties
  * only in case the host format is not "no-fec"
  */
 BCM_GPORT_LOCAL_SET(gport, out_port);
 rv = l3__egress_only_fec__create_inner(unit, fec, 0, encap_id, gport, 0,
                                       0, 0,&encoded_fec);
 if (rv != BCM_E_NONE)
 {
     printf("%s(): Error, create egress object FEC only\n",proc_name);
     return rv;
 }

 /*
  * 7. Add Route entry
  */
 rv = add_route_ipv4(unit, route, mask, vrf, fec);
 if (rv != BCM_E_NONE)
 {
     printf("%s(): Error, in function add_route_ipv4, \n",proc_name);
     return rv;
 }
 /*
  * 8. Add forwarding entries to the IPv4 and IPv6 entry
  */
 int _l3_itf;
 BCM_L3_ITF_SET(&_l3_itf, BCM_L3_ITF_TYPE_FEC, fec);
 BCM_GPORT_LOCAL_SET(gport, out_port);

 rv = add_host_ipv4(unit, host, vrf, _l3_itf, 0, 0);
 if (rv != BCM_E_NONE)
 {
     printf("%s(): Error, in function add_host_ipv4(), \n",proc_name);
     return rv;
 }


 rv = add_route_ipv6(unit, ipv6_route, ipv6_mask, vrf, fec);
 if (rv != BCM_E_NONE)
 {
     printf("%s(): Error, in function add_route_ipv6(), \n",proc_name);
     return rv;
 }

 /*
  * 9. Add VRRP EXEM entries for the IPv4 and IPv6 with the same VSI but with different VRID
  */

 /* only IPv4, DMAC: 00:00:5e:00:01:07 */
 rv = bcm_l3_vrrp_config_add(unit, BCM_L3_VRRP_EXTENDED | BCM_L3_VRRP_IPV4, intf_in, vrid_4);
 if (rv != BCM_E_NONE) {
     printf("Error, bcm_l3_vrrp_config_add VRID %d\n", vrid_4);
     return rv;
 }

 /* only IPv6, DMAC: 00:00:5e:00:02:08 */
 rv = bcm_l3_vrrp_config_add(unit, BCM_L3_VRRP_EXTENDED | BCM_L3_VRRP_IPV6, intf_in, vrid_6);
 if (rv != BCM_E_NONE) {
     printf("Error, bcm_l3_vrrp_config_add VRID %d\n", vrid_6);
     return rv;
 }




 printf("%s(): Exit\r\n",proc_name);
 return rv;

}
