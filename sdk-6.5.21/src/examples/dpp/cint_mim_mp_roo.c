/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*=================================
 *
 *   MiM ROO UC/MC flows
 *
 *=================================
 */

/*
 * the cint creates 2 MiM (backbone) ports connected to B-VID
 * and 2 vlan (access) ports connected to VSI.
 *
 *
 * set following soc properties
 * # Specify the "Auxiliary table" mode. 0: Private VLAN support, 1: Split horizon mode, 2: Mac-In-Mac support.
 * bcm886xx_auxiliary_table_mode=2
 * # Select whether to enable/disable SA authentication on the device
 * sa_auth_enabled=0
 * # Set the ARP table (next Hop MAC address) not to be extended
 * bcm886xx_next_hop_mac_extension_enable=0
 * run:
 *
 cd ../../../../src/examples/dpp
 cint cint_utils_vlan.c
 cint cint_utils_l2.c
 cint cint_utils_l3.c
 cint cint_port_tpid.c
 cint cint cint_mim_mp.c
 cint cint_mim_mp_roo.c
 cint
 int mim_vsi = 8;
 int isid   16777215
 * main_roo(unit, port1, port2, port3, port4, mim_vsi, isid);
 *
 *
 * =========  UC ==========
 * for termination:
 *
 * inject a IPv4oETH1oPBB packet from <port1>:
 *      mac-in-mac header with B-DA 010203040506, B-SA abcdef123412
 *      B-tag: B-VID 5, ethertype 0x88a8
 *      I-tag: I-SID 16777215, ethertype 0x88e7
 *      ethernet header with DA 08:02:03:04:05:55, SA 02:02:02:02:02:99
 *      vid = 8
 *      IPv4 hdr with DIP: 32.54.67.124  , SIP: 0.01.01.01
 *      port1 and Vid=5-> lands on B-VSI, PBB.DA is matching the B-MAC.
 *      ISID lookup returns isid_mp lif, pointing to regular VSI. ETH.DA is
 *      matching VSI.MyMac and the lif_rif is enabled for routing
 *      routing is on VRF=8 with fwd code for IPv4.
 *      destination is fec, poinitng to outrif.
 *
 * the packet will arrive at <port4> with:
 *      ethernet header with DA 06:06:06:06:06:06, SA 08:02:03:04:05:55
 *      vid=8
 *      IPv4 hdr with DIP: 32.54.67.124  , SIP: 0.01.01.01
 *
 *
 *
 *
 * for encapsulation:
 *
 *  IPv4oEth injection on port 4
 *  Eth with DA: 08:02:03:04:05:55, SA:  06:06:06:06:06:06 and VID 8
 *  IPv4 hdr with DIP: 10.4.43.15  , SIP: 0.01.01.01
 *  port4 with Vid 8 - lif lands on VSI=8, MyMac is enabled and matching ETH.DA
 *  LifRif is pointing to VRF=8 and routing is enabled.
 *  destination is fec, pointing to another fec (hierarchical)
 *  fec1 is giving the native Eth, second fec points MiM port, configured with adding New LL EDB format
 *  In EPNI LL EVE with 1 tag add happens
 *  PRGE program is making the magic
 *
 * outgoing packet is IPv4oETH1oPBB via port3
 *  PBB DA: AB:CD:EF:12:34:12; SA 01:02:03:04:05:06
 *  ISID 0xFFFFFF; OuterVID: 5
 *
 * Eth DA: 0B:0B:0B:0B:0B:0B; SA: 08:02:03:04:05:55; VID=7
 *  IPv4 hdr with DIP: 10.4.43.15  , SIP: 0.01.01.01
 *
 * second packet but this time IPv6oEth is injected on port 4
 * IPv6 DIP: 0100:1600:3500:7000:0000:DB07:DDCC:BBAA, SIP: 0000:0000:0000:0000:0000:0000:0000:0123
 * same flow, but the MiM port is different and outgoing port is port1
 *
 *
 * ========= MC =============
 *
 * Native:
 * IPv4oEth packet is injected,
 * Eth DA: 01:00:5E:00:01:01; SA:06:06:06:06:06:06, VID=3
 * IPv4 DIP:  224.00.01.01; SIP: 00.03.03.03
 * on port4 with vid=3 result in lif, leading to vsi nv_mc_vsi
 * Packet is compatible MC, routing MC is enabled - goes to VRF mc_vrf
 * fwd code is IPv4 MC, destination is MC group with 2 replications:
 * -> route to port - encapsulate LL
 * -> route to tunnel - encapsulate MiM over IPv4
 *
 * Out of tunnel
 *  IPv4oEthoPBB with:
 *  PBB DA: 01:02:03:04:05:06  SA: AB:CD:EF:12:34:12  VID: 5
 *  ISID: 0xFFFF00
 *  ETH: DA: 08:02:03:04:05:66; SA: 02:02:02:02:02:99; VID 8
 *  IPv4: DIP:  224.00.01.01; SIP: 00.04.04.01
 * IPv4oEthoPBB is injected on port1, BMAC is recognised, ISID=0xFFFF00 leads to nv_mc_vsi
 * Eth.DA is matching VSI.MyMac, goes to VRF, use the same MC group as Native with the same destinations
 * -> route to port
 * -> route to tunnel
 */



struct mim_info_roo_s {
    /* physical ports: */
    /* MiM (backbone) ports */
    bcm_gport_t port_1;
    bcm_gport_t port_2;
    /* vlan (access) ports */
    bcm_port_t port_3;
    bcm_port_t port_4;

    int vlan_id; /* in VID */
    int bvlan_id; /* in B-VID */
    int nv_mc_vlan_id; /* in VID for MC*/
    int dc_mc_vlan_id; /* in VID for MC*/

    int    new_outer_vid ;
    int    new_inner_vid ;

    /* logical vlan ports */
    bcm_gport_t vlan_port_1;
    bcm_gport_t vlan_port_2;
    bcm_gport_t vlan_port_3;
    bcm_gport_t vlan_port_4;

    /* logical MiM ports */
    bcm_mim_port_t mim_port_1;
    bcm_mim_port_t mim_port_2;

    bcm_mim_vpn_config_t bvid; /* B-VID */
    bcm_mim_vpn_config_t vsi; /* VSI */
    bcm_mim_vpn_config_t mc_vsi; /* MC VSI */

    int nv_mc_vsi;
    int dc_mc_vsi;
    int mc_vrf;

    bcm_mac_t src_bmac; /* out B-SA */
    bcm_mac_t dest_bmac; /* out B-DA = in B-SA */
    bcm_mac_t dest_bmac2; /* in B-DA */
    bcm_mac_t dest_mac; /* DA */
    bcm_mac_t l2_global_mac   ;

    bcm_l2_addr_t l2addr_1;
    bcm_l2_addr_t l2addr_2;

    bcm_gport_t    fec_id_1;                                 /*Need to allocate two consecutive FEC for egress BEB learning, for mim_port_1 and mim_port_3*/
    bcm_gport_t    fec_id_2;                                 /*Need to allocate two consecutive FEC for egress BEB learning, for mim_port_2 and mim_port_4*/
    bcm_gport_t    fec_id_3;                                 /*Need to allocate two consecutive FEC for egress BEB learning, for mim_port_1 and mim_port_3,
                                                               this encapsulation utilizes ROO linker layer*/
    bcm_gport_t    fec_id_4;                                 /*Need to allocate two consecutive FEC for egress BEB learning, for mim_port_2 and mim_port_4,
                                                               this encapsulation utilizes ROO linker layer*/

    /*ROO related*/
    bcm_mac_t my_cda;                                      /* CDA used for second my mac termination, backbone -> customer */
    bcm_mac_t my_cda_term;                                 /* CDA used for second my mac termination, backbone -> customer, l2_station_add requires masking */
    bcm_mac_t my_csa;                                      /* CSA used for new ethernet header in case of ROO, backbone -> customer */
    bcm_mac_t roo_cda_0;                                      /* DSA used for new ethernet header in case of ROO, customer -> backbone */
    bcm_mac_t roo_cda_1;
    bcm_mac_t my_cda_mc;
    bcm_mac_t my_cda_dc_mc;

    bcm_mac_t dest_mac_host1;                               /* DA of the host on port_3, SMLT */
    bcm_mac_t dest_mac_host2;                               /* DA of the host on port_4, single-homed*/

    bcm_l3_intf_t l3_ingress_intf;

    bcm_l3_intf_t l3_ingress_intf_nv_mc;
    bcm_l3_ingress_t l3_ing_if_nv_mc;

    bcm_l3_intf_t l3_ingress_intf_dc_mc;
    bcm_l3_ingress_t l3_ing_if_dc_mc;

    bcm_l3_intf_t l3_ingress_intf_dc_mc_1;
    bcm_l3_ingress_t l3_ing_if_dc_mc_1;

    bcm_l3_egress_t l3_egr_intf_port_4;
    bcm_if_t        l3_egr_intf_port_4_id;

    bcm_l3_egress_t l3_egr_intf_port_1;
    bcm_if_t        l3_egr_intf_port_1_id;

    bcm_l3_egress_t l3_egr_intf_port_2;
    bcm_if_t        l3_egr_intf_port_2_id;

    bcm_l3_egress_t l3_egr_intf_port_3;
    bcm_if_t        l3_egr_intf_port_3_id;

    /*mc stuff*/
    bcm_l3_egress_t l3_egr_intf_mc_port_3;
    bcm_if_t        l3_egr_intf_mc_port_3_id;

    bcm_l3_egress_t l3_egr_intf_mc_port_2;
    bcm_if_t        l3_egr_intf_mc_port_2_id;

    bcm_l3_route_t customer_access_route;                  /* Route which leads to port_4, dest_mac_host2, ROO termination */
    int            customer_access_route_ip;
    int            customer_access_route_mask;

    bcm_l3_host_t  backbone_host_route;
    bcm_l3_route_t backbone_route;                  /* Route which leads to port_1, dest_bmac_1, ROO encapsulation */
    int            backbone_route_ip;   /*ip4 addr*/
    int            backbone_route_mask;
    bcm_ip6_t      route_ip6;
    bcm_ip6_t      mask_ip6;

    bcm_ipmc_addr_t ipmc_entry_nv;
    bcm_ipmc_addr_t ipmc_entry_dc;
};

mim_info_roo_s mim_info_roo;


int
mim_info_roo_init(int unit, int port1, int port2, int port3, int port4) {

    int rv;

    /* MiM (backbone) ports */
    mim_info_roo.port_1 = port3;
    mim_info_roo.port_2 = port1;
    /* access ports */
    mim_info_roo.port_3 = port2;
    mim_info_roo.port_4 = port4;

    mim_info_roo.vlan_id = 8; /* in VID */
    mim_info_roo.bvlan_id = 5; /* in B-VID */
    mim_info_roo.nv_mc_vlan_id = 3;
    mim_info_roo.dc_mc_vlan_id = 9;
    mim_info_roo.nv_mc_vsi = 3;
    mim_info_roo.dc_mc_vsi = 9;

    mim_info_roo.mc_vrf = 22;
    bcm_mim_vpn_config_t_init(&(mim_info_roo.bvid));
    bcm_mim_vpn_config_t_init(&(mim_info_roo.vsi));
    bcm_mim_vpn_config_t_init(&(mim_info_roo.mc_vsi));

    bcm_mim_port_t_init(&(mim_info_roo.mim_port_1));
    bcm_mim_port_t_init(&(mim_info_roo.mim_port_2));

    mim_info_roo.l2_global_mac[0]    = 0x08;
    mim_info_roo.l2_global_mac[1]    = 0x02;
    mim_info_roo.l2_global_mac[2]    = 0x03;
    mim_info_roo.l2_global_mac[3]    = 0x04;
    mim_info_roo.l2_global_mac[4]    = 0x00;
    mim_info_roo.l2_global_mac[5]    = 0x00;

    /* when going out of MiM port: */
    mim_info_roo.dest_bmac[0] = 0xab;
    mim_info_roo.dest_bmac[1] = 0xcd;
    mim_info_roo.dest_bmac[2] = 0xef;
    mim_info_roo.dest_bmac[3] = 0x12;
    mim_info_roo.dest_bmac[4] = 0x34;
    mim_info_roo.dest_bmac[5] = 0x12;

    mim_info_roo.src_bmac[0] = 0x01;
    mim_info_roo.src_bmac[1] = 0x02;
    mim_info_roo.src_bmac[2] = 0x03;
    mim_info_roo.src_bmac[3] = 0x04;
    mim_info_roo.src_bmac[4] = 0x05;
    mim_info_roo.src_bmac[5] = 0x06;

    /* when arriving in MiM port: */
    mim_info_roo.dest_bmac2[0] = 0x00;
    mim_info_roo.dest_bmac2[1] = 0x11;
    mim_info_roo.dest_bmac2[2] = 0x22;
    mim_info_roo.dest_bmac2[3] = 0x33;
    mim_info_roo.dest_bmac2[4] = 0x44;
    mim_info_roo.dest_bmac2[5] = 0x55;

    mim_info_roo.dest_mac[0] = 0x0;
    mim_info_roo.dest_mac[1] = 0x0;
    mim_info_roo.dest_mac[2] = 0x0;
    mim_info_roo.dest_mac[3] = 0x0;
    mim_info_roo.dest_mac[4] = 0x0;
    mim_info_roo.dest_mac[5] = 0x5;

    mim_info_roo.dest_mac_host1[0] = 0x5;
    mim_info_roo.dest_mac_host1[1] = 0x5;
    mim_info_roo.dest_mac_host1[2] = 0x5;
    mim_info_roo.dest_mac_host1[3] = 0x5;
    mim_info_roo.dest_mac_host1[4] = 0x5;
    mim_info_roo.dest_mac_host1[5] = 0x5;

    mim_info_roo.dest_mac_host2[0] = 0x6;
    mim_info_roo.dest_mac_host2[1] = 0x6;
    mim_info_roo.dest_mac_host2[2] = 0x6;
    mim_info_roo.dest_mac_host2[3] = 0x6;
    mim_info_roo.dest_mac_host2[4] = 0x6;
    mim_info_roo.dest_mac_host2[5] = 0x6;

    bcm_l2_addr_t_init(&(mim_info_roo.l2addr_1), mim_info_roo.dest_bmac, 0);
    bcm_l2_addr_t_init(&(mim_info_roo.l2addr_2), mim_info_roo.dest_mac, 0);

    /*rv = bcm_stg_create(unit, &(mim_info_roo.stg));
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_stg_create\n");
        print rv;
    }*/
    mim_info_roo.new_outer_vid = 7;
    mim_info_roo.new_inner_vid = 4;
    mim_info_roo.fec_id_1 = (16 << 26) + 12000;/*0x40000000 + 12000;*/
    mim_info_roo.fec_id_2 = (16 << 26) + 12002;/*0x40000000 + 12002;*/
    mim_info_roo.fec_id_3 = (16 << 26) + 0x3000;/*0x40000000 + 0x3000;*/
    mim_info_roo.fec_id_4 = (16 << 26) + 0x3002;/*0x40000000 + 0x3002;*/

    mim_info_roo.my_cda[0] = 0x08;
    mim_info_roo.my_cda[1] = 0x02;
    mim_info_roo.my_cda[2] = 0x03;
    mim_info_roo.my_cda[3] = 0x04;
    mim_info_roo.my_cda[4] = 0x05;
    mim_info_roo.my_cda[5] = 0x55;

    mim_info_roo.my_cda_mc[0] = 0x08;
    mim_info_roo.my_cda_mc[1] = 0x02;
    mim_info_roo.my_cda_mc[2] = 0x03;
    mim_info_roo.my_cda_mc[3] = 0x04;
    mim_info_roo.my_cda_mc[4] = 0x05;
    mim_info_roo.my_cda_mc[5] = 0x66;

    mim_info_roo.my_cda_dc_mc[0] = 0x08;
    mim_info_roo.my_cda_dc_mc[1] = 0x02;
    mim_info_roo.my_cda_dc_mc[2] = 0x03;
    mim_info_roo.my_cda_dc_mc[3] = 0x04;
    mim_info_roo.my_cda_dc_mc[4] = 0x05;
    mim_info_roo.my_cda_dc_mc[5] = 0x77;

    mim_info_roo.my_cda_term[0] = 0x00;
    mim_info_roo.my_cda_term[1] = 0x00;
    mim_info_roo.my_cda_term[2] = 0x00;
    mim_info_roo.my_cda_term[3] = 0x00;
    mim_info_roo.my_cda_term[4] = 0x05;
    mim_info_roo.my_cda_term[5] = 0x55;

    mim_info_roo.my_csa[0] = 0xc;
    mim_info_roo.my_csa[1] = 0xc;
    mim_info_roo.my_csa[2] = 0xc;
    mim_info_roo.my_csa[3] = 0xc;
    mim_info_roo.my_csa[4] = 0xc;
    mim_info_roo.my_csa[5] = 0xc;

    mim_info_roo.roo_cda_0[0] = 0xb;
    mim_info_roo.roo_cda_0[1] = 0xb;
    mim_info_roo.roo_cda_0[2] = 0xb;
    mim_info_roo.roo_cda_0[3] = 0xb;
    mim_info_roo.roo_cda_0[4] = 0xb;
    mim_info_roo.roo_cda_0[5] = 0xb;

    mim_info_roo.roo_cda_1[0] = 0xc;
    mim_info_roo.roo_cda_1[1] = 0x1;
    mim_info_roo.roo_cda_1[2] = 0xd;
    mim_info_roo.roo_cda_1[3] = 0x1;
    mim_info_roo.roo_cda_1[4] = 0x21;
    mim_info_roo.roo_cda_1[5] = 0x25;

    mim_info_roo.customer_access_route_ip = 0x2036437C; /*32.54.67.124*/
    mim_info_roo.customer_access_route_mask = 0xFFFFFF00;

    mim_info_roo.backbone_route_ip = 0xa042B0F; /*10.4.43.15*/
    mim_info_roo.backbone_route_mask = 0xFFFFFF00;

    mim_info_roo.route_ip6[15] = 0xaa; /* LSB */;
    mim_info_roo.route_ip6[14] = 0xbb;
    mim_info_roo.route_ip6[13] = 0xcc;
    mim_info_roo.route_ip6[12] = 0xdd;
    mim_info_roo.route_ip6[11] = 0x07;
    mim_info_roo.route_ip6[10] = 0xdb;
    mim_info_roo.route_ip6[9] =  0;
    mim_info_roo.route_ip6[8] =  0;
    mim_info_roo.route_ip6[7] =  0;
    mim_info_roo.route_ip6[6] =  0x70;
    mim_info_roo.route_ip6[5] =  0;
    mim_info_roo.route_ip6[4] =  0x35;
    mim_info_roo.route_ip6[3] =  0;
    mim_info_roo.route_ip6[2] =  0x16;
    mim_info_roo.route_ip6[1] =  0;
    mim_info_roo.route_ip6[0] =  0x01; /* MSB */

    mim_info_roo.mask_ip6[15]= 0xff;
    mim_info_roo.mask_ip6[14]= 0xff;
    mim_info_roo.mask_ip6[13]= 0xff;
    mim_info_roo.mask_ip6[12]= 0xff;
    mim_info_roo.mask_ip6[11]= 0xff;
    mim_info_roo.mask_ip6[10]= 0xff;
    mim_info_roo.mask_ip6[9] = 0xff;
    mim_info_roo.mask_ip6[8] = 0xff;
    mim_info_roo.mask_ip6[7] = 0xff;
    mim_info_roo.mask_ip6[6] = 0;
    mim_info_roo.mask_ip6[5] = 0;
    mim_info_roo.mask_ip6[4] = 0;
    mim_info_roo.mask_ip6[3] = 0;
    mim_info_roo.mask_ip6[2] = 0;
    mim_info_roo.mask_ip6[1] = 0;
    mim_info_roo.mask_ip6[0] = 0;
    return rv;
}

/* configure bcm_mim_port_t with the given parameters */
void
mim_port_config_roo(
    int unit,
    bcm_mim_port_t *mim_port,
    int port, /* physical port */
    uint16 vpn, /* in B-VID */
    bcm_mac_t src_mac, /* in B-SA */
    uint16 bvid, /* out B-VID */
    bcm_mac_t dest_mac, /* out B-DA */
    bcm_mac_t    src_bmac,
    bcm_gport_t   fec,
    uint32 isid /* egress I-SID - relevant for P2P ports only */
    ) 
{
    mim_port->flags = BCM_MIM_PORT_TYPE_BACKBONE | BCM_MIM_PORT_WITH_ID | BCM_MIM_PORT_CASCADED;
    mim_port->flags |= is_device_or_above(unit, JERICHO_PLUS) ? 0 : BCM_MIM_PORT_ENCAP_OVERLAY;
    mim_port->mim_port_id = fec;


    mim_port->port = port;
    mim_port->match_tunnel_vlan = vpn;
    sal_memcpy(mim_port->match_tunnel_srcmac, dest_mac, 6);
    mim_port->egress_tunnel_vlan = bvid;
    sal_memcpy(mim_port->egress_tunnel_dstmac, dest_mac, 6);
    sal_memcpy(mim_port->egress_tunnel_srcmac, src_bmac, 6);
    mim_port->egress_service_tpid  = 0x88a8;
    mim_port->egress_tunnel_service = isid;
    mim_port->failover_id = 0;
    mim_port->failover_gport_id = BCM_GPORT_TYPE_NONE;
}

int
mim_l2_station_add_termination(
    int unit,
    bcm_mac_t mac,
    bcm_vlan_t vlan,
    int *station_id
    ) {
    int rv;
    bcm_l2_station_t station;

    bcm_l2_station_t_init(&station);

    station.flags = BCM_L2_STATION_MIM;

    station.vlan = vlan;
    station.vlan_mask = 0xFFFF;

    sal_memcpy(station.dst_mac, mac, 6);

    station.dst_mac_mask[0] = 0xff;
    station.dst_mac_mask[1] = 0xff;
    station.dst_mac_mask[2] = 0xff;
    station.dst_mac_mask[3] = 0xff;
    station.dst_mac_mask[4] = 0xff;
    station.dst_mac_mask[5] = 0xff;

    rv = bcm_l2_station_add(unit, station_id, &station);
    if (rv != BCM_E_NONE) {
        printf("bcm_l2_station_add  %d \n", *station_id);
        print rv;
    }
    return BCM_E_NONE;
}

int advanced_vlan_edit_egress_roo(int unit, int *action_id, bcm_vlan_port_translation_t *vlan_port_translation, bcm_gport_t gport, bcm_vlan_translate_action_class_t *action_class,  int vlan_edit_profile,  bcm_vlan_action_t action_type, bcm_vlan_action_set_t *action,  int flags, uint32 tpid, uint32 new_outer_vlan, uint32 new_inner_vlan){

    int rv = BCM_E_NONE;

    bcm_vlan_port_translation_t_init(vlan_port_translation);
    vlan_port_translation->flags = BCM_VLAN_ACTION_SET_EGRESS;
    vlan_port_translation->gport = gport;
    vlan_port_translation->vlan_edit_class_id = vlan_edit_profile;
    vlan_port_translation->new_outer_vlan = new_outer_vlan;
    vlan_port_translation->new_inner_vlan = new_inner_vlan;
    rv = bcm_vlan_port_translation_set(unit, vlan_port_translation);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_translation_set\n");
        return rv;
    }

    /* Map the VLAN edit profile and tag format 0 (untagged) to an action ID */
    bcm_vlan_translate_action_class_t_init(action_class);
    action_class->flags = BCM_VLAN_ACTION_SET_EGRESS;
    action_class->vlan_edit_class_id = vlan_edit_profile;
    action_class->tag_format_class_id = 0;
    action_class->vlan_translation_action_id = *action_id;
    rv = bcm_vlan_translate_action_class_set(unit, action_class);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    bcm_vlan_action_set_t_init(action);
    action->action_id = *action_id;

    printf("action_id: %d\n", *action_id);

    rv = bcm_vlan_translate_action_id_create(unit,flags,action_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }

    action->dt_outer = action_type; /* performing AVT on the lif. It will add the vsi, in this case: B-VID*/
    action->outer_tpid = tpid;

    rv = bcm_vlan_translate_action_id_set(unit,flags,*action_id,action);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_translate_action_id_set\n");
        return rv;
    }

    return rv;
}

int mim_setup_roo_termination(int unit) {
    int rv;
   l2__mymac_properties_s mymac_properties;

    int station_id;
    bcm_l2_station_t station;


    bcm_l3_intf_t_init(&mim_info_roo.l3_ingress_intf);
    sal_memcpy(mim_info_roo.l3_ingress_intf.l3a_mac_addr, mim_info_roo.my_cda, 6);
    mim_info_roo.l3_ingress_intf.l3a_vid = mim_info_roo.vlan_id;
    mim_info_roo.l3_ingress_intf.l3a_vrf = mim_info_roo.vlan_id;
    rv = bcm_l3_intf_create(unit, mim_info_roo.l3_ingress_intf);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_create\n");
        return rv;
    }


    bcm_l3_egress_t_init(&mim_info_roo.l3_egr_intf_port_3);
    sal_memcpy(mim_info_roo.l3_egr_intf_port_3.mac_addr, mim_info_roo.dest_mac_host1, 6);
    mim_info_roo.l3_egr_intf_port_3.port = mim_info_roo.port_3;
    mim_info_roo.l3_egr_intf_port_3.intf = mim_info_roo.l3_ingress_intf.l3a_intf_id;
    rv = bcm_l3_egress_create(unit, 0/*flags*/, &mim_info_roo.l3_egr_intf_port_3, &mim_info_roo.l3_egr_intf_port_3_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_egress_create\n");
        return rv;
    }


    bcm_l3_egress_t_init(&mim_info_roo.l3_egr_intf_port_4);
    sal_memcpy(mim_info_roo.l3_egr_intf_port_4.mac_addr, mim_info_roo.dest_mac_host2, 6);
    mim_info_roo.l3_egr_intf_port_4.port = mim_info_roo.port_4;
    mim_info_roo.l3_egr_intf_port_4.intf = mim_info_roo.l3_ingress_intf.l3a_intf_id;
    rv = bcm_l3_egress_create(unit, 0/*flags*/, &mim_info_roo.l3_egr_intf_port_4, &mim_info_roo.l3_egr_intf_port_4_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_egress_create\n");
        return rv;
    }

    bcm_l3_route_t_init(&mim_info_roo.customer_access_route);
    mim_info_roo.customer_access_route.l3a_subnet = mim_info_roo.customer_access_route_ip;
    mim_info_roo.customer_access_route.l3a_ip_mask = mim_info_roo.customer_access_route_mask;
    mim_info_roo.customer_access_route.l3a_vrf = mim_info_roo.vlan_id;
    mim_info_roo.customer_access_route.l3a_intf = mim_info_roo.l3_egr_intf_port_4_id;
    rv = bcm_l3_route_add(unit, &mim_info_roo.customer_access_route);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_route_add\n");
        return rv;
    }

    return BCM_E_NONE;
}

int mim_setup_mc(int unit) {
    int rv;
    int ipmc_index_nv = 5000;
    int ipmc_index_dc = 5020;

    /* Set my_mac (12 LSB bits)*/
    l2__mymac_properties_s mymac_properties;

    sal_memset(&mymac_properties, 0, sizeof(mymac_properties));
    mymac_properties.mymac_type = l2__mymac_type_ipv4_vsi;

    sal_memcpy(mymac_properties.mymac, mim_info_roo.my_cda_mc, 6);
    mymac_properties.vsi =  mim_info_roo.nv_mc_vsi;

    rv = l2__mymac__set(unit, &mymac_properties);
    if (rv != BCM_E_NONE) {
      printf("Fail  l2__mymac__set: \n");
      return rv;
    }

        /* Set my_mac (12 LSB bits)*/

    sal_memcpy(mymac_properties.mymac, mim_info_roo.my_cda_dc_mc, 6);
    mymac_properties.vsi =  mim_info_roo.dc_mc_vsi;

    rv = l2__mymac__set(unit, &mymac_properties);
    if (rv != BCM_E_NONE) {
      printf("Fail  l2__mymac__set: \n");
      return rv;
    }

    /*for MC lif_ac - going to mc vsi*/
    rv = l2__port_vlan__create(unit,0,  mim_info_roo.port_4, mim_info_roo.nv_mc_vlan_id, mim_info_roo.nv_mc_vsi, &mim_info_roo.vlan_port_4);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_port_create with port_4 and mc vsi\n");
        return rv;
    }

    rv = l2__port_vlan__create(unit,0,  mim_info_roo.port_3, mim_info_roo.dc_mc_vlan_id, mim_info_roo.dc_mc_vsi, &mim_info_roo.vlan_port_3);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_port_create with port_4 and mc vsi\n");
        return rv;
    }

    bcm_l3_intf_t_init(&mim_info_roo.l3_ingress_intf_nv_mc);
    bcm_l3_ingress_t_init(&mim_info_roo.l3_ing_if_nv_mc);

    mim_info_roo.l3_ingress_intf_nv_mc.l3a_vid= mim_info_roo.nv_mc_vsi  ;
    sal_memcpy(mim_info_roo.l3_ingress_intf_nv_mc.l3a_mac_addr, mim_info_roo.my_cda_mc, 6);

    rv = bcm_l3_intf_create(unit, mim_info_roo.l3_ingress_intf_nv_mc);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_create %d\n",rv);
        return rv;
    }

    mim_info_roo.l3_ing_if_nv_mc.flags = BCM_L3_INGRESS_WITH_ID; /* must, as we update exist RIF*/
    mim_info_roo.l3_ing_if_nv_mc.vrf = mim_info_roo.mc_vrf;
    rv = bcm_l3_ingress_create(unit, mim_info_roo.l3_ing_if_nv_mc, mim_info_roo.l3_ingress_intf_nv_mc.l3a_intf_id);
    if (rv != BCM_E_NONE) {
      printf("Error, bcm_l3_ingress_create\n");
      return rv;
    }
    printf("\n In rif creation done\n");


    bcm_l3_intf_t_init(&mim_info_roo.l3_ingress_intf_dc_mc);
    bcm_l3_ingress_t_init(&mim_info_roo.l3_ing_if_dc_mc);

    mim_info_roo.l3_ingress_intf_dc_mc.l3a_vid= mim_info_roo.dc_mc_vsi  ;
    sal_memcpy(mim_info_roo.l3_ingress_intf_dc_mc.l3a_mac_addr, mim_info_roo.my_cda_dc_mc, 6);

    rv = bcm_l3_intf_create(unit, mim_info_roo.l3_ingress_intf_dc_mc);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_intf_create %d\n",rv);
        return rv;
    }

    mim_info_roo.l3_ing_if_dc_mc.flags = BCM_L3_INGRESS_WITH_ID; /* must, as we update exist RIF*/
    mim_info_roo.l3_ing_if_dc_mc.vrf = mim_info_roo.mc_vrf;
    rv = bcm_l3_ingress_create(unit, mim_info_roo.l3_ing_if_dc_mc, mim_info_roo.l3_ingress_intf_dc_mc.l3a_intf_id);
    if (rv != BCM_E_NONE) {
      printf("Error, bcm_l3_ingress_create\n");
      return rv;
    }
    printf("\n Out rif creation done ,  mim_info_roo.l3_ingress_intf_dc_mc.l3a_intf_id %x \n", mim_info_roo.l3_ingress_intf_dc_mc.l3a_intf_id);


    /*mc group and destination*/
    /* create mc group */
    rv = bcm_multicast_create(unit, BCM_MULTICAST_EGRESS_GROUP | BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID, &ipmc_index_nv);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_create \n");
        return rv;
    }
    printf("multicast group created: %x \n", ipmc_index_nv);

    /** add provider mc entries (entries with 2 CUDS)
     *  and add access mc entries (entries with 1 CUD) **/
    bcm_multicast_replication_t reps[2];

    bcm_multicast_replication_t_init(reps[0]);
    bcm_multicast_replication_t_init(reps[1]);


    /*overlay tunnel, double cud - native rif + outer lif*/
    /* add backbone mc entry: to port 2 */
    /* cud outRif */
    rv = bcm_multicast_l3_encap_get(unit, -1 /* not used */, -1 /* not used */,
                               mim_info_roo.l3_ingress_intf.l3a_intf_id, &(reps[0].encap1));
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_l3_encap_get \n");
        return rv;
    }

    /* cud outLif */
    reps[0].encap2 = mim_info_roo.mim_port_1.encap_id;

    BCM_GPORT_LOCAL_SET(reps[0].port, mim_info_roo.port_1);
    reps[0].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID  | BCM_MUTICAST_REPLICATION_ENCAP1_L3_INTF;

    /* OuRIF, single cud entry, routing working it is*/
    rv = bcm_multicast_l3_encap_get(unit, -1 /* not used */, -1 /* not used */,
                               mim_info_roo.l3_ingress_intf_dc_mc.l3a_intf_id, &(reps[1].encap1));


    BCM_GPORT_LOCAL_SET(reps[1].port, mim_info_roo.port_3);
    reps[1].flags = BCM_MUTICAST_REPLICATION_ENCAP1_L3_INTF;

    printf("\n \t\t\t\tmulticast entries added to 0x%x \n", ipmc_index_nv);
    printf("mc entry with 1 cud: outRif: 0x%x port: 0x%x added to 0x%x \n", reps[1].encap1, reps[1].port, ipmc_index_nv);
    printf("\t\t mc entry with 2 cuds: outLif: 0x%x outRif: 0x%x port: 0x%x added to 0x%x \n", reps[0].encap2, reps[0].encap1, reps[0].port, ipmc_index_nv);

    rv = bcm_multicast_set(unit, ipmc_index_nv, 0, 2, reps);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_multicast_set \n");
        return rv;
    }

    /** add routing table entry **/
    bcm_ipmc_addr_t_init(mim_info_roo.ipmc_entry_nv);
    mim_info_roo.ipmc_entry_nv.vrf = mim_info_roo.mc_vrf;
    mim_info_roo.ipmc_entry_nv.group = ipmc_index_nv;
    mim_info_roo.ipmc_entry_nv.mc_ip_addr = 0xE0000101; /* 224.0.1.1 */
    mim_info_roo.ipmc_entry_nv.mc_ip_mask = 0xffffffff;

    /* Creates the entry */
    rv = bcm_ipmc_add(unit,&mim_info_roo.ipmc_entry_nv);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_ipmc_add \n");
        return rv;
    }

    printf("mc group entry %x added to routing table \n ", ipmc_index_nv);

    return BCM_E_NONE;
}


int mim_setup_roo_encapsulation(int unit) {
    int rv;

    /*IPv4 entry in LEM*/
    bcm_l3_egress_t_init(&mim_info_roo.l3_egr_intf_port_1);
    sal_memcpy(mim_info_roo.l3_egr_intf_port_1.mac_addr, mim_info_roo.roo_cda_0, 6);
    mim_info_roo.l3_egr_intf_port_1.port = mim_info_roo.mim_port_1.mim_port_id;
    mim_info_roo.l3_egr_intf_port_1.intf = mim_info_roo.l3_ingress_intf.l3a_intf_id;
    mim_info_roo.l3_egr_intf_port_1.failover_id = 0;
    mim_info_roo.l3_egr_intf_port_1.failover_if_id = 0;
    mim_info_roo.l3_egr_intf_port_1.flags = is_device_or_above(unit, JERICHO_PLUS) ? BCM_L3_NATIVE_ENCAP : 0;

    rv = bcm_l3_egress_create(unit, 0/*flags*/, &mim_info_roo.l3_egr_intf_port_1, &mim_info_roo.l3_egr_intf_port_1_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_egress_create\n");
        return rv;
    }

    bcm_l3_host_t_init(&mim_info_roo.backbone_host_route);

    mim_info_roo.backbone_host_route.l3a_flags = 0;
    mim_info_roo.backbone_host_route.l3a_vrf = mim_info_roo.vlan_id;
    mim_info_roo.backbone_host_route.l3a_ip_addr = mim_info_roo.backbone_route_ip;
    mim_info_roo.backbone_host_route.l3a_intf = mim_info_roo.l3_egr_intf_port_1_id;

    printf("\n \t\t Port 1 egress If port id 0x%x  mim_info_roo.l3_egr_intf_port_1.encap_id 0x%x vlan  0x%x \n\n",mim_info_roo.backbone_host_route.l3a_intf, mim_info_roo.l3_egr_intf_port_1.encap_id, mim_info_roo.l3_egr_intf_port_1.vlan  );

    rv = bcm_l3_host_add(unit, &mim_info_roo.backbone_host_route);
    if (rv != BCM_E_NONE) {
      printf("Error, bcm_l3_host_add\n");
      return rv;
    }

    /*IPv6 entry in LPM(KAPS)*/
    /* route_ip6*/

    bcm_l3_egress_t_init(&mim_info_roo.l3_egr_intf_port_2);
    sal_memcpy(mim_info_roo.l3_egr_intf_port_2.mac_addr, mim_info_roo.roo_cda_1, 6);
    mim_info_roo.l3_egr_intf_port_2.port = mim_info_roo.mim_port_2.mim_port_id;
    mim_info_roo.l3_egr_intf_port_2.intf = mim_info_roo.l3_ingress_intf.l3a_intf_id;
    mim_info_roo.l3_egr_intf_port_2.failover_id = 0;
    mim_info_roo.l3_egr_intf_port_2.failover_if_id = 0;
    mim_info_roo.l3_egr_intf_port_2.flags = is_device_or_above(unit, JERICHO_PLUS) ? BCM_L3_NATIVE_ENCAP : 0;

    rv = bcm_l3_egress_create(unit, 0/*flags*/, &mim_info_roo.l3_egr_intf_port_2, &mim_info_roo.l3_egr_intf_port_2_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_egress_create\n");
        return rv;
    }

    bcm_l3_route_t_init(&mim_info_roo.backbone_route);

    mim_info_roo.backbone_route.l3a_flags = BCM_L3_IP6;
    sal_memcpy(&(mim_info_roo.backbone_route.l3a_ip6_net),(mim_info_roo.route_ip6),16);
    sal_memcpy(&(mim_info_roo.backbone_route.l3a_ip6_mask),(mim_info_roo.mask_ip6),16);
    mim_info_roo.backbone_route.l3a_modid = 0;
    mim_info_roo.backbone_route.l3a_port_tgid = 0;
    mim_info_roo.backbone_route.l3a_vrf = mim_info_roo.vlan_id;
    mim_info_roo.backbone_route.l3a_intf = mim_info_roo.l3_egr_intf_port_2_id;

    rv = bcm_l3_route_add(unit, &mim_info_roo.backbone_route);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_l3_route_add\n");
        return rv;
    }

    return BCM_E_NONE;
}



int
main_roo(int unit, int port1, int port2, int port3, int port4,  int differential_flooding, int mim_vsi, int isid_id) {

    int rv, flags, ingress_action_id, egress_action_id, flags_ingress, flags_egress, vlan_edit_profile_1, vlan_edit_profile_2;
    uint32 advanced_vlan_translation_mode;
    bcm_pbmp_t pbmp, ubmp;
    bcm_if_t encap_id_1, encap_id_2;
    bcm_multicast_t mc_group, mc_group_1, mc_group_2;
    bcm_vlan_action_set_t action;
    bcm_vlan_port_translation_t vlan_port_translation;
    bcm_vlan_translate_action_class_t action_class;
    bcm_port_tpid_class_t tpid_class, tpid_class_2;
    bcm_gport_t gport;
    bcm_vlan_port_t vp;
    bcm_port_match_info_t match_info;
    int station_id;
    l2__mymac_properties_s mymac_properties;

    /* Configurations for advanced vlan editing*/
    sal_srand(sal_time());
    ingress_action_id =  (sal_rand() % 60) + 4; /* we deal only with action ids above 3*/

    vlan_edit_profile_1 = (sal_rand() % 7) + 1; /* we deal only with vlan_edit_profile between 1 and 7*/
    vlan_edit_profile_2 = vlan_edit_profile_1;
    while (vlan_edit_profile_1 == vlan_edit_profile_2) {vlan_edit_profile_2 =  (sal_rand() % 7) + 1;}
    flags_ingress = BCM_VLAN_ACTION_SET_INGRESS | BCM_VLAN_ACTION_SET_WITH_ID;
    flags_egress = BCM_VLAN_ACTION_SET_EGRESS | BCM_VLAN_ACTION_SET_WITH_ID;
    advanced_vlan_translation_mode = soc_property_get(unit , "bcm886xx_vlan_translate_mode",0);

    /*Necessary for multiple my-mac termination*/
    rv = bcm_switch_l3_protocol_group_set(unit, BCM_SWITCH_L3_PROTOCOL_GROUP_MIM/*flags*/, 2/*group*/);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_l3_protocol_group_set\n");
        return rv;
    }
    printf(" bcm_switch_l3_protocol_group_set done\n");

    rv = mim_info_roo_init(unit, port1, port2, port3, port4);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_info_roo_init\n");
        return rv;
    }
    printf("mim_info_roo_init done \n");

    /* init MiM on the device */
    rv = bcm_mim_init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mim_init\n");
        print rv;
        return rv;
    }
    printf("bcm_mim_init done \n");

    /* set the PBB-TE B-VID ranges */
    rv = bcm_switch_control_set(unit, bcmSwitchMiMTeBvidLowerRange, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set for lower range\n");
        print rv;
        return rv;
    }

    rv = bcm_switch_control_set(unit, bcmSwitchMiMTeBvidUpperRange, 11);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set for upper range\n");
        print rv;
        return rv;
    }

    /* set 2 physical ports as MiM (backbone) ports */
    rv = bcm_port_control_set(unit, mim_info_roo.port_1, bcmPortControlMacInMac, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set with port_1\n");
        print rv;
        return rv;
    }
    printf(" bcm_port_control_set with port_1 done\n");

    rv = bcm_port_control_set(unit, mim_info_roo.port_2, bcmPortControlMacInMac, 1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_port_control_set with port_2\n");
        print rv;
        return rv;
    }

    /* make the MiM physical port recognize the B-tag ethertype */
    /* Set drop to all packets without I-tag. */
    port_tpid_init(mim_info_roo.port_1, 1, 0);
    port_tpid_info1.outer_tpids[0] = 0x88a8;
    port_tpid_info1.discard_frames = BCM_PORT_DISCARD_TAG;
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set with port_1\n");
        print rv;
        return rv;
    }
        printf(" port_tpid_set with port_1 done\n");

    /* Set drop to all packets without I-tag. */
    port_tpid_init(mim_info_roo.port_2, 1, 0);
    port_tpid_info1.outer_tpids[0] = 0x88a8;
    port_tpid_info1.discard_frames = BCM_PORT_DISCARD_TAG;
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set with port_2\n");
        print rv;
        return rv;
    }

    /* set MiM ports ingress vlan-editing to remove B-tag if exists */
    bcm_vlan_action_set_t_init(&action);

    if (advanced_vlan_translation_mode) {

        /* remove outer tag in case of single tagged packet in avt mode */
        rv = advanced_vlan_edit_ingress(unit, &ingress_action_id,bcmVlanActionDelete ,&action, flags_ingress, mim_info_roo.port_1, mim_info_roo.port_2, 0x88a8);
        if (rv != BCM_E_NONE) {
            printf("Error, advanced_vlan_edit_ingress\n");
            print rv;
            return rv;
        }

        /* creating an alternative default out ac for AVT in MIM*/
        /*bcm_vlan_port_t_init(&vp); */

        rv = create_alternative_default_mim_ac(unit, &vp, mim_info_roo.port_1, &match_info, &gport);
        if (rv != BCM_E_NONE) {
            printf("Error, create_alternative_default_mim_ac\n");
            print rv;
            return rv;
        }

        /* Attach the MIM vlan_port_ids to a VLAN edit profile */
        egress_action_id = ingress_action_id;

        while (egress_action_id == ingress_action_id) {egress_action_id =  (sal_rand() % 252) + 4;} /* we deal only with action ids above 3*/
        printf("egress_action_id: %d\n",egress_action_id );

        rv = advanced_vlan_edit_egress_roo(unit, &egress_action_id, &vlan_port_translation, gport, &action_class, vlan_edit_profile_1,  bcmVlanActionAdd, &action,  flags_egress, 0x8100,mim_info_roo.new_outer_vid, mim_info_roo.new_inner_vid );
        if (rv != BCM_E_NONE) {
            printf("Error, advanced_vlan_edit_egress_roo\n");
            return rv;
        }

        /*bcm_vlan_port_t_init(&vp);*/
        gport = 0;


        rv = create_alternative_default_mim_ac(unit, &vp, mim_info_roo.port_2, &match_info, &gport);
        if (rv != BCM_E_NONE) {
            printf("Error, create_alternative_default_mim_ac\n");
            print rv;
            return rv;
        }

        bcm_vlan_action_set_t_init(&action);
        ingress_action_id = egress_action_id;

        while (egress_action_id == ingress_action_id) {egress_action_id =  (sal_rand() % 252) + 4;}
        printf("egress_action_id: %d\n",egress_action_id );

        rv = advanced_vlan_edit_egress_roo(unit, &egress_action_id, &vlan_port_translation, gport, &action_class, vlan_edit_profile_2,  bcmVlanActionAdd, &action,  flags_egress, 0x8100,mim_info_roo.new_outer_vid, mim_info_roo.new_inner_vid);
        if (rv != BCM_E_NONE) {
            printf("Error, advanced_vlan_edit_egress_roo\n");
            return rv;
        }


    }
    else{
        action.ot_outer = bcmVlanActionDelete; /* remove outer tag in case of single tagged packet */

        rv = bcm_vlan_port_default_action_set(unit, mim_info_roo.port_1, &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_tpid_set with mim_info_roo.port_1\n");
            print rv;
            return rv;
        }

        rv = bcm_vlan_port_default_action_set(unit, mim_info_roo.port_2, &action);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_port_tpid_set with mim_info_roo.port_2\n");
            print rv;
            return rv;
        }
    }

    /* prepare the port bit maps to set the B-VID port membership, with the 2 backbone ports (used for filtering) */
    BCM_PBMP_CLEAR(pbmp); /* port bit map for tagged ports */
    BCM_PBMP_CLEAR(ubmp); /* port bit map for untagged ports */
    BCM_PBMP_PORT_ADD(pbmp, mim_info_roo.port_1);
    BCM_PBMP_PORT_ADD(pbmp, mim_info_roo.port_2);

    /* create B-VID */
    rv = mim_bvid_create(unit, &(mim_info_roo.bvid), mim_info_roo.bvlan_id, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_vpn_create with bvid\n");
        return rv;
    }
        printf(" mim_vpn_create with bvid done \n");

    /* create 2 MiM (backbone) MP ports */
    /*                         mim_port */                 /* physical port */      /* in B-VID */        /* in B-SA */
    mim_port_config_roo(unit, &(mim_info_roo.mim_port_1), mim_info_roo.port_1,    mim_info_roo.bvid.vpn,  mim_info_roo.src_bmac,
                         mim_info_roo.bvid.vpn, mim_info_roo.dest_bmac, mim_info_roo.src_bmac,mim_info_roo.fec_id_3,  0);
      rv = bcm_mim_port_add(unit, 1, &(mim_info_roo.mim_port_1)); /* use valid VPN (=1) to create MP port */
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mim_port_add with mim_port_1\n");
        print rv;
        return rv;
    }
    printf(" bcm_mim_port_add with mim_port_1 done \n");

    mim_port_config_roo(unit, &(mim_info_roo.mim_port_2), mim_info_roo.port_2, mim_info_roo.bvid.vpn, mim_info_roo.src_bmac,
                    mim_info_roo.bvid.vpn, mim_info_roo.dest_bmac, mim_info_roo.src_bmac,mim_info_roo.fec_id_4, 0);
    rv = bcm_mim_port_add(unit, 1, &(mim_info_roo.mim_port_2));
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mim_port_add with mim_port_2\n");
        print rv;
        return rv;
    }

    /* create VSI */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_CLEAR(ubmp);

    /* prepare the port bit maps to set the VSI port membership, with the 2 access ports (used for filtering) */
    BCM_PBMP_PORT_ADD(pbmp, mim_info_roo.port_3);
    BCM_PBMP_PORT_ADD(pbmp, mim_info_roo.port_4);

    /* VSI should be < 4K */
    rv = mim_vsi_create(unit, &(mim_info_roo.vsi), mim_vsi, isid_id);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_vpn_create with vsi\n");
        return rv;
    }

    /*to connect native vsi to new isid, lets see the run:*/
    rv = mim_vsi_create(unit, &(mim_info_roo.mc_vsi),mim_info_roo.nv_mc_vsi , 0xFFFF00 /*isid_id-0xFF*/ );
    if (rv != BCM_E_NONE) {
        printf("Error, mim_vpn_create with mc_vsi\n");
        return rv;
    }


    /* create 2 access (vlan) ports: */
    rv = vlan_port_create(unit, mim_info_roo.port_3, &(mim_info_roo.vlan_port_1), mim_info_roo.vlan_id);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_port_create with port_3\n");
        return rv;
    }

    rv = vlan_port_create(unit, mim_info_roo.port_4, &(mim_info_roo.vlan_port_2), mim_info_roo.vlan_id);
    if (rv != BCM_E_NONE) {
        printf("Error, vlan_port_create with port_4\n");
        return rv;
    }


    /* add the vlan-ports to the vswitch, both will point at the same VSI */
    rv = bcm_vswitch_port_add(unit, mim_info_roo.vsi.vpn, mim_info_roo.vlan_port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }

    rv = bcm_vswitch_port_add(unit, mim_info_roo.vsi.vpn, mim_info_roo.vlan_port_2);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }

    /* set Vlan port membership */
    rv = bcm_vlan_port_add(unit, mim_info_roo.vlan_id, pbmp, ubmp);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vlan_port_add for vlan %d\n", mim_info_roo.vsi.vpn);
        print rv;
    }


    /* for the packet to arrive at MiM ports with the correct B-SA: */
    /* set global B-SA MSB */
    rv = mim_l2_station_add(unit, 1 /* MSB */, 0, mim_info_roo.src_bmac, &station_id);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_l2_station_add (global)\n");
        print rv;
        return rv;
    }

     rv = mim_l2_station_add_termination(unit, mim_info_roo.src_bmac, mim_info_roo.bvlan_id + 0x7000/*backbone VSI*/, &station_id);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_l2_station_add_termination\n");
        print rv;
        return rv;
    }

    /* set per-src-access-port B-SA LSB */
    rv = mim_l2_station_add(unit, 0 /* LSB */, mim_info_roo.port_4, mim_info_roo.src_bmac, &station_id);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_l2_station_add (per port)\n");
        print rv;
        return rv;
    }
    rv = mim_l2_station_add(unit, 0 /* LSB */, mim_info_roo.port_3,  mim_info_roo.dest_bmac, &station_id);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_l2_station_add (per port)\n");
        print rv;
        return rv;
    }

    /*adding termination elements*/
    rv = mim_setup_roo_termination(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_setup_roo_termination\n");
        return rv;
    }

    rv = mim_setup_roo_encapsulation(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_setup_roo_encapsulation\n");
        return rv;
    }


    /*adding mc flow*/
    rv = mim_setup_mc(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, mim_setup_mc\n");
        return rv;
    }

    return BCM_E_NONE;
}

