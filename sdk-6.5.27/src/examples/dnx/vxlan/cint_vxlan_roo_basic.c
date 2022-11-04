/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/*
 * Feature label : FL-VXLAN
 *
 * How to execute the cint:
 * from the BCM shell ($SDK/systems/linux/user/slk_be) use the following steps :
 * Test Scenario - start
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_multicast.c
  cint ../../../../src/examples/dnx/vxlan/cint_vxlan_roo_basic.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vlan_translate.c
  cint
  int unit = 0;
  int rv = 0;
  int access_port = 200;
  int core_port = 203;
  cint_vxlan_roo_basic_info.tunnel_type = bcmTunnelTypeVxlan;
  print vxlan_roo_basic(unit,access_port,core_port);
  exit;
 *
 * SOC properties : Use default BRCM SOC properties
 *
 * Packet to send for encapsulation :IPv4oETH0
 * Tx = PORT:200
 *   =>   ETHERNET HEADER <=
 *   dst_mac => 00:0F:00:02:0A:44
 *   src_mac => 00:00:00:00:DD:2F
 *   tpid    => -1
 *   prio    => -1
 *   cfi     => -1
 *   vid     => -1
 *   ether type   => 0x0800 (ipv4)
 *
 *   => IPV4 HEADER <=
 *   version => 4
 *   header length  => 5
 *   tos value    => 0
 *   dscp value   => 0
 *   ip_flags     => 0
 *   frag offset  => 0
 *   ttl          => 128
 *   protocol     => 0
 *   src ip       => 0x7fffff01 (127.255.255.1)
 *   dst ip       => 0x7fffff02 (127.255.255.2)
 *
 * Use the following command from shell to send the packet:
  tx 1 psrc=200 data=0x000f00020a4400000000dd2f0800450000350000000080003cc67fffff017fffff02000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Packet received after encapsulation : IPv4oETH1oVXLANoUDPoIPV4oETH1
 * Rx = 1 PORT:203
 *   ETH1             DA=0x00000000CD1E SA=0x000F00020A33 TPID=0x8100 PCP=0x5 DEI=0x0 VID=0x014 ETHERTYPE=0x0800
 *   IPV4             VERSION=0x4 IHL=0x5 DSCP=0x02 ECN=0x2 LENGTH=0x006B ID=0x0000 RESERVED1=0x0 DF=0x0 MF=0x0
 *                    FO=0x0000 TTL=0x40 PROTOCOL=0x11 CHECKSUM=0x2376 SIP=0xAC000001 DIP=0xAB000001
 *   UDP              SRC_PORT=0x7560 DST_PORT=0x12B5 LENGTH=0x0057 CHECKSUM=0x0000
 *   VXLAN            RESERVED1=0x0 VERSION=0x0 VNI_FLAG=0x1 NEXT_PROTOCOL_FLAG=0x0 BUM_FLAG=0x0 OAM_FLAG=0x0
 *                    RESERVED2=0x0000 NEXT_PROTOCOL=0x00 VNI=0x001388 RESERVED3=0x00
 *   ETH1             DA=0x00000000CD1D SA=0x000F00020A22 TPID=0x8100 PCP=0x0 DEI=0x0 VID=0x00C ETHERTYPE=0x0800
 *   IPV4             VERSION=0x4 IHL=0x5 DSCP=0x00 ECN=0x0 LENGTH=0x0035 ID=0x0000 RESERVED1=0x0 DF=0x0 MF=0x0
 *                    FO=0x0000 TTL=0x7F PROTOCOL=0x00 CHECKSUM=0x3DC6 SIP=0x7FFFFF01 DIP=0x7FFFFF02
 * 
 * Packet to send for termination :IPv4oETH0oVXLANoUDPoIPV4oETH0
 * Tx = PORT:203
 *   =>  ETHERNET HEADER <=
 *   dst_mac => 00:0F:00:02:0A:33
 *   src_mac => 00:00:00:00:CD:1E
 *   tpid    => -1
 *   prio    => -1
 *   cfi     => -1
 *   vid     => -1
 *   ether type   => 0x0800 (ipv4)
 *
 *   =>  IPV4 HEADER <=
 *   version => 4
 *   header length  => 5
 *   tos value    => 0
 *   dscp value   => 0
 *   ip_flags     => 0
 *   frag offset  => 0
 *   ttl          => 128
 *   protocol     => 17
 *   src ip       => 0xac000001 (172.0.0.1)
 *   dst ip       => 0xab000001 (171.0.0.1)
 *
 *   =>   UDP HEADER <=
 *   l4 source port => 0x5555 (21845)
 *   l4 dest port   => 0x12b6 (4790)
 *   length         => 0x0053
 *   VXLANoUDPoIPV4oETH0        =>
 *     0000:: 0x08 0x00 0x00 0x00 0x00 0x13 0x88 0x00   0x00 0x0f 0x00 0x02 0x0a 0x22 0x00 0x00
 *     0016:: 0x00 0x00 0xfd 0x2f 0x08 0x00 0x45 0x00   0x00 0x35 0x00 0x00 0x00 0x00 0x80 0x00
 *     0032:: 0x3c 0xc5 0x7f 0xff 0xff 0x01 0x7f 0xff   0xff 0x03 0x00 0x01 0x02 0x03 0x04 0x05
 *     0048:: 0x06 0x07 0x08 0x09 0x0a 0x0b 0x0c 0x0d   0x0e 0x0f 0x10 0x11 0x12 0x13 0x14 0x15
 *     0064:: 0x16 0x17 0x18 0x19 0x1a 0x1b 0x1c 0x1d   0x1e 0x1f 0x20
 *
 * Use the following command from shell to send the packet:
  tx 1 psrc=203 data=0x000f00020a3300000000cd1e080045000067000000008011e383ac000001ab000001555512b6005300000800000000138800000f00020a2200000000fd2f0800450000350000000080003cc57fffff017fffff03000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Packet received after encapsulation :  IPv4oETH1
 * Rx = 1 PORT:200
 * ETH1             DA=0x00000000CD1F SA=0x000F00020A44 TPID=0x8100 PCP=0x0 DEI=0x0 VID=0x00A ETHERTYPE=0x0800
 * IPV4             VERSION=0x4 IHL=0x5 DSCP=0x00 ECN=0x0 LENGTH=0x0035 ID=0x0000 RESERVED1=0x0 DF=0x0 MF=0x0
 *                  FO=0x0000 TTL=0x7F PROTOCOL=0x00 CHECKSUM=0x3DC5 SIP=0x7FFFFF01 DIP=0x7FFFFF03
 * 
 * DBAL tables that are relevant for this case :
  dbal table dump table=EEDB_ARP
  dbal table dump table=EEDB_IPV4_TUNNEL
  dbal table dump table=FWD_MACT
  dbal table dump table=IPV4_UNICAST_PRIVATE_HOST
  dbal table dump table=GLOBAL_LIF_EM
 * Signals that are relevant for this case :
  sig get name=ees
  sig get name=Fwd_Action_Dst
 * Test Scenario - end
 *
 */

/*
 * VXLAN global structure
 */
struct cint_vxlan_roo_basic_info_s
{
    int vxlan_network_group_id;             /* ingress and egress orientation for VXLAN */
    int tunnel_type;                        /* bcmTunnelTypeVxlan/bcmTunnelTypeVxlan6 or bcmTunnelTypeGeneve/bcmTunnelTypeGeneve6 */
    bcm_ip_t tunnel_dip;                    /* tunnel DIP */
    bcm_ip_t tunnel_dip_mask;               /* tunnel DIP mask */
    bcm_ip_t tunnel_sip;                    /* tunnel SIP */
    bcm_ip_t tunnel_sip_mask;               /* tunnel SIP mask*/
    bcm_ip6_t tunnel_dip6;                  /* tunnel DIP6 */
    bcm_ip6_t tunnel_dip6_mask;             /* tunnel DIP6 mask */
    bcm_ip6_t tunnel_sip6;                  /* tunnel SIP6 */
    bcm_ip6_t tunnel_sip6_mask;             /* tunnel SIP6 mask*/
    bcm_gport_t tunnel_id;                  /* Tunnel ID */
    int tunnel_if;                          /* Tunnel Interface */
    int vpn_id;                             /* VPN ID */
    int vni;                                /* VNI of the VXLAN */
    int access_port1;                       /* incoming port from access side */
    int access_port2;                       /* incoming port from access side */
    int core_port;                          /* incoming port from core side */
    int core_native_eth_rif;                /* core native RIF */
    int core_overlay_eth_rif;               /* core overlay RIF */
    int core_overlay_eth_vrf;               /* core overlay VRF */
    int access_eth_rif;                     /* access RIF */
    int access_eth_vrf;                     /* access VRF */
    bcm_vlan_t access_vid;                  /* access vid for AC */
    bcm_gport_t outer_ac_vlan_port_id;      /* vlan port id of the ac_port, used for ingress */
    int core_native_arp_id;                 /* Id for core native ARP entry */
    int core_overlay_arp_id;                /* Id for core overlay ARP entry */
    int access_arp_id;                      /* Id for access ARP entry */
    int core_native_fec_id;                 /* core native fec id for native outrif and native arp */
    int core_overlay_fec_id;                /* core overlay fec id for overlay */
    int access_fec_id;                      /* access fec id for access outrif and access arp */
    bcm_mac_t access_eth_fwd_mac;           /* mac for bridge fwd */
    bcm_mac_t core_native_eth_rif_mac;      /* mac for core native RIF */
    bcm_mac_t core_overlay_eth_rif_mac;     /* mac for core overlay RIF */
    bcm_mac_t access_eth_rif_mac;           /* mac for access RIF */
    bcm_mac_t core_native_next_hop_mac;     /* mac for next core native hop */
    bcm_mac_t core_overlay_next_hop_mac;    /* mac for next core overlay hop */
    bcm_mac_t access_next_hop_mac;          /* mac for next access hop */
    bcm_ip_t route_dip_into_overlay;        /* first dip */
    bcm_ip_t route_dip_out_of_overlay;      /* second dip */
    bcm_ip6_t routev6_dip_in_of_overlay;
    bcm_ip6_t routev6_dip_out_of_overlay;
    bcm_ip6_t maskv6_dip_out_of_overlay;
    int outer_tpid;                         /* Outer tpid for application */
    int inner_tpid;                         /* Inner tpid for application */
    bcm_gport_t egress_native_ac_vlan_port_id;  /* vlan port id of the inner ac_port, used for egress */
    bcm_gport_t vswitch_vlan_port_id; /* vlan port created for use with vswitch */
    uint32 egress_vlan_edit_profile, ingress_vlan_edit_profile;
    int tag_format;
    int tunnel_termination_vrf;
    bcm_if_t core_overlay_encap_tunnel_intf;
    int encap_tunnel_id;
    int term_tunnel_id;
    int native_vlan; /* native vlan. Set by native vlan editing. */
    bcm_gport_t vxlan_port_id;              /* VXLAN port id */
    int term_bud;                           /* indicate to tunnel term bud node. */
    int use_local_lif_for_native_arp;       /* For native ARP, use local Out-LIF directly */
    int enable_vrrp;                        /* indicate to configure VRRP */
    int ing_qos_id;                         /* ingress qos profile */
    int egr_qos_id;                         /* egress qos profile */
};


cint_vxlan_roo_basic_info_s cint_vxlan_roo_basic_info =
{
    /*
     * VXLAN Orientation,
     */
    1,
    /*
     * Tunnel type,
     */
    bcmTunnelTypeVxlan,
    /*
     * tunnel DIP
     */
    0xAB000001, /* 171.0.0.1 */
    /*
     * tunnel DIP mask
     */
    0xffffffff,
    /*
     * tunnel SIP
     */
    0xAC000001, /* 172.0.0.1 */
    /*
     * tunnel SIP mask
     */
    0xffffffff,
    /*
     * tunnel DIP6
     */
    { 0x20, 0x01, 0x0D, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, 0x22, 0xFF, 0xFE, 0x33, 0x44, 0x55 },
    /*
     * tunnel DIP6 mask
     */
    { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
    /*
     * tunnel DIP6
     */
    { 0x20, 0x01, 0x0D, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x02, 0x11, 0x22, 0xFF, 0xFE, 0x77, 0x88, 0x99 },
    /*
     * tunnel SIP6 mask
     */
    { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff },
    /*
     * tunnel id | tunnel_if
     */
    20, 10,
    /*
     * VPN ID
     */
    15,
    /*
     * VNI
     */
    5000,
    /*
     * ports : access_port1 | access_port2 | core_port
     */
    200, 201, 202,
    /*
     * core_native_eth_rif
     */
    15,
    /*
     * core_overlay_eth_rif, core_overlay_eth_vrf
     */
    20, 5,
    /*
     * access_eth_rif, access_eth_vrf
     */
    10, 1,
    /*
     *  access_vid | outer_ac_vlan_port_id
     */
     0x5a5, 8888,
    /*
     * core_native_arp_id | core_overlay_arp_id | access_arp_id
     */
    0, 5050, 0,
    /*
     * core_native_fec_id | core_overlay_fec_id | access_fec_id | mpls_fec_id
     */
    40961, 65537, 40963,
    /*
     * access_eth_fwd_mac | core_native_eth_rif_mac | core_overlay_eth_rif_mac | access_eth_rif_mac
     */
    {0x00, 0x00, 0xA5, 0x5A, 0xA5, 0x5A},
    {0x00, 0x0F, 0x00, 0x02, 0x0A, 0x22},
    {0x00, 0x0F, 0x00, 0x02, 0x0A, 0x33},
    {0x00, 0x0F, 0x00, 0x02, 0x0A, 0x44},
    /*
     * core_native_next_hop_mac |  core_overlay_next_hop_mac | access_next_hop_mac
     */
    {0x00, 0x00, 0x00, 0x00, 0xCD, 0x1D},
    {0x00, 0x00, 0x00, 0x00, 0xCD, 0x1E},
    {0x00, 0x00, 0x00, 0x00, 0xCD, 0x1F},
    /*
     *route_dip_into_overlay | route_dip_out_of_overlay
     127.255.255.02 , 127.255.255.03
     */
    0x7fffff02, 0x7fffff03,
    /*
     * routev6_dip_in_of_overlay | routev6_dip_out_of_overlay | maskv6_dip_out_of_overlay
     */
    { 0x20, 0x00, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x10 },
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xF0 },
    /*
     * outer_tpid | inner_tpid
     */
    0x8100, 0x88a8,
    /*
     * egress_native_ac_vlan_port_id
     */
    0,
    /*
     * vswitch_vlan_port_id
     */
    0,
    /*
     * egress_vlan_edit_profile, ingress_vlan_edit_profile
     */
    4, 5,
    /*
     * tag_format | tunnel_termination_vrf | core_overlay_encap_tunnel_intf
     */
    0, 5, 0,
    /*
     * encap_tunnel_id, term_tunnel_id
     */
    0, 0,
    /*
     *
     */
    0xC,

};

int native_eve_action_id;
/* When set, RIF will be part of the native ARP, not the FEC. Required for bfd_vxlan_extended_sip*/
int is_native_arp_with_rif;
/* Used for bfd ipv4 over vxlan */
int bfd_vxlan_extended_sip;

int vxlan_roo_udf_tunnel_type = 0;

void
vxlan_roo_basic_init(
    int access_port1,
    int access_port2,
    int core_port)
{
    cint_vxlan_roo_basic_info.access_port1 = access_port1;
    cint_vxlan_roo_basic_info.access_port2 = access_port2;
    cint_vxlan_roo_basic_info.core_port = core_port;
}

/*
 * The basic VXLAN example with parameters:
 * Unit - relevant unit
 * access_port - port where core host is connected to.
 * core_port - port where DC Fabric router is connected to.
 */
int vxlan_roo_basic(
    int unit,
    int access_port1,
    int access_port2,
    int core_port)
{
    char error_msg[200]={0,};
    bcm_if_t encap_tunnel_intf;
    bcm_gport_t vlan_port_id;
    bcm_gport_t vxlan_port_id;
    l3_ingress_intf ingr_itf;
    bcm_gport_t tunnel_id;



    l3_ingress_intf_init(&ingr_itf);

    vxlan_roo_basic_init(access_port1, access_port2, core_port);

    /*
     * Build L2 VPN
     */
    snprintf(error_msg, sizeof(error_msg), "vpn=%d", cint_vxlan_roo_basic_info.vpn_id);
    BCM_IF_ERROR_RETURN_MSG(vxlan_roo_basic_vxlan_open_vpn(unit, cint_vxlan_roo_basic_info.vpn_id, cint_vxlan_roo_basic_info.vni), error_msg);

    /* configure entry: VNI+AC
       At ESEM for vsi, netork_domain -> VNI, AC information
       create a virtual AC which represent the AC information in ESEM entry.
       The virtual AC save the key to access the ESEM entry.
       In this case: vsi, network domain
       */
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.vsi = cint_vxlan_roo_basic_info.vpn_id; /*  vsi, part of the key in ESEM lookup */
    vlan_port.match_class_id = 0;  /* network domain, part of the key in ESEM lookup */

    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NAMESPACE_VSI;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");

    /* Save native AC */
    cint_vxlan_roo_basic_info.egress_native_ac_vlan_port_id = vlan_port.vlan_port_id;

    /*
     * Configure native VLAN translation
     */
    native_eve_action_id = 4;
    int vlan_edit_profile = 3;
    int tag_format_untag = 0;
    action_id_1 = native_eve_action_id;
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set_with_pri_action(unit, vlan_port.vlan_port_id,        /* outlif                  */
                                                                      0x8100,                        /* outer_tpid              */
                                                                      0,                             /* inner_tpid              */
                                                                      bcmVlanTpidActionModify,       /* Outer tpid action modify*/
                                                                      bcmVlanTpidActionNone,         /* Inner tpid action none  */
                                                                      bcmVlanActionAdd,              /* outer_action            */
                                                                      bcmVlanActionNone,             /* inner_action            */
                                                                      bcmVlanActionAdd,              /* outer_pri_action        */
                                                                      bcmVlanActionNone,             /* inner_pri_action        */
                                                                      cint_vxlan_roo_basic_info.native_vlan,    /* new_outer_vid */
                                                                      0,                             /* new_inner_vid           */
                                                                      vlan_edit_profile,             /* vlan_edit_profile       */
                                                                      tag_format_untag,              /* tag_format - here is untag */
                                                                      FALSE                          /* is_ive                  */
                                                                      ), "");
    
    BCM_IF_ERROR_RETURN_MSG(vxlan_roo_basic_configure_port_properties(unit, cint_vxlan_roo_basic_info.access_port1, cint_vxlan_roo_basic_info.access_port2,
                                                  cint_vxlan_roo_basic_info.core_port), "");

    /*
     * Configure L3 interfaces
     */
    BCM_IF_ERROR_RETURN_MSG(vxlan_roo_basic_create_l3_interfaces(unit), "");

    /*
     * Configure VRRP
     */
    if (cint_vxlan_roo_basic_info.enable_vrrp)
    {
        BCM_IF_ERROR_RETURN_MSG(vxlan_roo_my_mac_vrrp(unit), "");
    }

    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    BCM_IF_ERROR_RETURN_MSG(vxlan_tunnel_terminator_create(unit, &cint_vxlan_roo_basic_info.term_tunnel_id), "");

    /*
     *Configure ARP entries
     */
    BCM_IF_ERROR_RETURN_MSG(vxlan_roo_basic_create_arp_entries(unit), "");

    /*
     * Create the tunnel initiator
     */
    BCM_IF_ERROR_RETURN_MSG(vxlan_tunnel_initiator_create(unit, cint_vxlan_roo_basic_info.core_overlay_arp_id, &(cint_vxlan_roo_basic_info.core_overlay_encap_tunnel_intf), &cint_vxlan_roo_basic_info.encap_tunnel_id), "");

    /*
     * Configure fec entry
     */
    BCM_IF_ERROR_RETURN_MSG(vxlan_roo_basic_create_fec_entries(unit), "");

    /*
     * Add VXLAN port properties and configure the tunnel terminator In-Lif and tunnel FEC
     */
    BCM_IF_ERROR_RETURN_MSG(vxlan_port_add(unit, core_port, cint_vxlan_roo_basic_info.term_tunnel_id, &vxlan_port_id), "");

    cint_vxlan_roo_basic_info.vxlan_port_id = vxlan_port_id;

    BCM_IF_ERROR_RETURN_MSG(vxlan_roo_basic_l3_forwarding(unit), "");

    BCM_IF_ERROR_RETURN_MSG(vlan_port_add(unit, access_port1, cint_vxlan_roo_basic_info.access_vid, &cint_vxlan_roo_basic_info.vswitch_vlan_port_id), "");

    BCM_IF_ERROR_RETURN_MSG(vswitch_add_port(unit, cint_vxlan_roo_basic_info.vpn_id, cint_vxlan_roo_basic_info.vswitch_vlan_port_id), "");


    /*
     * Create an MACT entry to VXLAN tunnel, mapped to the VPN
     */
    BCM_IF_ERROR_RETURN_MSG(l2_fwd_entry_add(unit, cint_vxlan_roo_basic_info.access_eth_fwd_mac, cint_vxlan_roo_basic_info.vpn_id, vxlan_port_id), "");

    BCM_IF_ERROR_RETURN_MSG(l2_fwd_entry_add(unit, cint_vxlan_roo_basic_info.access_eth_fwd_mac, cint_vxlan_roo_basic_info.access_eth_rif, vxlan_port_id), "");


    return BCM_E_NONE;
}

int
vxlan_roo_basic_l3_forwarding(
    int unit)
{

    /*
     * Add route entry with mask as 0xffffffff for access to core traffic
     */
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, cint_vxlan_roo_basic_info.route_dip_into_overlay, 0xffffffff, cint_vxlan_roo_basic_info.access_eth_vrf, cint_vxlan_roo_basic_info.core_native_fec_id), "");

    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_vxlan_roo_basic_info.routev6_dip_in_of_overlay, cint_vxlan_roo_basic_info.maskv6_dip_out_of_overlay,
                        cint_vxlan_roo_basic_info.access_eth_vrf, cint_vxlan_roo_basic_info.core_native_fec_id), "");

    /*
     * Add route entry with mask as 0xffffffff for core to access traffic
     */
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv4(unit, cint_vxlan_roo_basic_info.route_dip_out_of_overlay, 0xffffffff, cint_vxlan_roo_basic_info.tunnel_termination_vrf, cint_vxlan_roo_basic_info.access_fec_id), "");

    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_vxlan_roo_basic_info.routev6_dip_out_of_overlay, cint_vxlan_roo_basic_info.maskv6_dip_out_of_overlay,
                        cint_vxlan_roo_basic_info.tunnel_termination_vrf, cint_vxlan_roo_basic_info.access_fec_id), "");

    /*
     * Add route entry for access to core traffic
     */
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_vxlan_roo_basic_info.routev6_dip_in_of_overlay, cint_vxlan_roo_basic_info.maskv6_dip_out_of_overlay, cint_vxlan_roo_basic_info.access_eth_vrf, cint_vxlan_roo_basic_info.core_native_fec_id), "");
    /*
     * Add route entry for core to access traffic
     */
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, cint_vxlan_roo_basic_info.routev6_dip_out_of_overlay, cint_vxlan_roo_basic_info.maskv6_dip_out_of_overlay, cint_vxlan_roo_basic_info.tunnel_termination_vrf, cint_vxlan_roo_basic_info.access_fec_id), "");

    return BCM_E_NONE;
}

int
vxlan_roo_basic_create_fec_entries(
    int unit)
{
    int temp_out_rif;
    int flags = 0;
    int fec_id;
    bcm_gport_t gport;

    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &fec_id), "");

    cint_vxlan_roo_basic_info.core_native_fec_id = fec_id++;
    cint_vxlan_roo_basic_info.access_fec_id = fec_id++;

    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 1, 0, &fec_id), "");
    cint_vxlan_roo_basic_info.core_overlay_fec_id = fec_id ;

    bcm_if_t next_if = cint_vxlan_roo_basic_info.core_native_arp_id;
    if (bfd_vxlan_extended_sip)
    {
        /* When BFD extended SIP is used, FEC points to extrened-sip outlif which points to native ARP and includes the RIF.
           RIF in this case may not be part of the FEC due to encap-access considerations. Encap access for exteneded-SIP must be
           bcmEncapAccessRif*/
        BCM_IF_ERROR_RETURN_MSG(dnx_bfd_extended_sip_configuration(unit, 0x10203040, next_if, bcmEncapAccessRif, &next_if), "");
    }

    /*
     * Create native FEC for routing into overlay:
     *  1) Give a fec id.
     *  2) Native out rif.
     *  3) Native arp id.
     *  4) Point to overlay fec.
     */
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_vxlan_roo_basic_info.core_overlay_fec_id);
    int fwd_flag = (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) ? BCM_L3_FLAGS2_FWD_ONLY : 0;
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_vxlan_roo_basic_info.core_native_fec_id,
                                     is_native_arp_with_rif?0:cint_vxlan_roo_basic_info.core_native_eth_rif,
                                     (cint_vxlan_roo_basic_info.use_local_lif_for_native_arp?cint_vxlan_roo_basic_info.core_overlay_encap_tunnel_intf:next_if),
                                     gport, flags, fwd_flag), "in creating native FEC for routing into overlay");
    printf("For native FEC: fec id 0x%x, \n eth rif 0x%x, \n arp 0x%x, \n port (fec): 0x%x \n", cint_vxlan_roo_basic_info.core_native_fec_id,
                               cint_vxlan_roo_basic_info.core_native_eth_rif,
                               cint_vxlan_roo_basic_info.core_native_arp_id,
                               gport);


    /*
     * Create overlay FEC for routing into overlay:
     * 1) Give a fec id.
     * 2) Tunnel id of the overlay VXLAN Tunnel.
     * 3) Tunnel id of the overlay Eth.
     * 4) Out port.
     */
    gport = 0;
    flags |= BCM_L3_CASCADED;
    BCM_GPORT_LOCAL_SET(gport, cint_vxlan_roo_basic_info.core_port);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_vxlan_roo_basic_info.core_overlay_fec_id,
                                     (cint_vxlan_roo_basic_info.use_local_lif_for_native_arp?cint_vxlan_roo_basic_info.core_native_arp_id:cint_vxlan_roo_basic_info.core_overlay_encap_tunnel_intf),
                                     0, gport, flags, fwd_flag), "in creating overlay FEC for routing into overlay");

    /*
     * Create FEC for routing out of overlay:
     * 1) Give a fec id.
     * 2) Access out rif.
     * 3) Access arp id.
     * 4) Out port.
     */
    flags = 0;
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_vxlan_roo_basic_info.access_fec_id,
                                     cint_vxlan_roo_basic_info.access_eth_rif, cint_vxlan_roo_basic_info.access_arp_id,
                                     cint_vxlan_roo_basic_info.access_port1, flags, fwd_flag), "in creating FEC routing out of overlay");
	return BCM_E_NONE;
}

int
vxlan_roo_basic_create_arp_entries(
    int unit)
{

    /*
     * Configure native ARP entry
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, 0, &(cint_vxlan_roo_basic_info.core_native_arp_id),
                                       cint_vxlan_roo_basic_info.core_native_next_hop_mac,
                                       is_native_arp_with_rif ? cint_vxlan_roo_basic_info.core_native_eth_rif:0,
                                       BCM_L3_NATIVE_ENCAP,
                                       0), "create AC egress object ARP only");

    /*
     * Configure overlay ARP entry
     */
    if (cint_vxlan_roo_basic_info.egr_qos_id) {
        uint32 flags2 = 0;
        dnx_utils_l3_arp_s arp_structure;

        /* ARP+AC entry */
        flags2 = BCM_L3_FLAGS2_VLAN_TRANSLATION;

        dnx_utils_l3_arp_s_common_init(unit, 0, &arp_structure, cint_vxlan_roo_basic_info.core_overlay_arp_id, 0, flags2,
                                       cint_vxlan_roo_basic_info.core_overlay_next_hop_mac, cint_vxlan_roo_basic_info.core_overlay_eth_rif);
        arp_structure.qos_map_id = cint_vxlan_roo_basic_info.egr_qos_id;
        BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_arp_create(unit, &arp_structure), "");

        cint_vxlan_roo_basic_info.core_overlay_arp_id = arp_structure.l3eg_arp_id;
    } else {
        BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, 0, &(cint_vxlan_roo_basic_info.core_overlay_arp_id),
                                           cint_vxlan_roo_basic_info.core_overlay_next_hop_mac,
                                           cint_vxlan_roo_basic_info.core_overlay_eth_rif),
                                           "create AC egress object ARP only");
    }

    /*
     * Configure access ARP entry
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, 0, &(cint_vxlan_roo_basic_info.access_arp_id),
                                       cint_vxlan_roo_basic_info.access_next_hop_mac,
                                       cint_vxlan_roo_basic_info.access_eth_rif),
                                       "create AC egress object ARP only");

    return BCM_E_NONE;

}

int
vxlan_roo_basic_configure_port_properties(
    int unit,
    int access_port1,
    int access_port2,
    int core_port)
{
    char error_msg[200]={0,};
    /*
     * set class for ports
     */
    snprintf(error_msg, sizeof(error_msg), "port=%d", access_port1);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, access_port1, bcmPortClassId, access_port1), error_msg);

    snprintf(error_msg, sizeof(error_msg), "port=%d", access_port2);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, access_port2, bcmPortClassId, access_port2), error_msg);

    snprintf(error_msg, sizeof(error_msg), "port=%d", core_port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, core_port, bcmPortClassId, core_port), error_msg);

    /*
     * Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, cint_vxlan_roo_basic_info.access_port1, cint_vxlan_roo_basic_info.access_eth_rif), "intf_in");

    /*
     * Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, cint_vxlan_roo_basic_info.access_port2, cint_vxlan_roo_basic_info.vpn_id), "intf_in");

    /*
     * Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, cint_vxlan_roo_basic_info.core_port, cint_vxlan_roo_basic_info.core_overlay_eth_rif), "intf_in");

    /*
     * Set Out-Port default properties
     */

    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, cint_vxlan_roo_basic_info.access_port1), "intf_out");

    /*
     * Set Out-Port default properties
     */

    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, cint_vxlan_roo_basic_info.access_port2), "intf_out");

    /*
     * Set Out-Port default properties
     */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, cint_vxlan_roo_basic_info.core_port), "intf_out");

    /*
     * Core to access, VLAN tranlsation and QoS set
     */
    if (cint_vxlan_roo_basic_info.egr_qos_id)
    {
        bcm_vlan_port_t vp;
        bcm_vlan_port_t_init(&vp);
        vp.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
        vp.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
        vp.port = cint_vxlan_roo_basic_info.access_port1;
        vp.vsi = cint_vxlan_roo_basic_info.access_eth_rif;
        BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vp), "");

        BCM_IF_ERROR_RETURN_MSG(bcm_qos_port_map_set(unit, vp.vlan_port_id, -1, cint_vxlan_roo_basic_info.egr_qos_id), "");

        /*
         * Configure native VLAN translation
         */
        BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set_with_pri_action(unit, vp.vlan_port_id,               /* outlif                  */
                                                                          0x8100,                        /* outer_tpid              */
                                                                          0,                             /* inner_tpid              */
                                                                          bcmVlanTpidActionModify,       /* Outer tpid action modify*/
                                                                          bcmVlanTpidActionNone,         /* Inner tpid action none  */
                                                                          bcmVlanActionAdd,              /* outer_action            */
                                                                          bcmVlanActionNone,             /* inner_action            */
                                                                          bcmVlanActionAdd,              /* outer_pri_action        */
                                                                          bcmVlanActionNone,             /* inner_pri_action        */
                                                                          50,                            /* new_outer_vid */
                                                                          0,                             /* new_inner_vid           */
                                                                          4,                             /* vlan_edit_profile       */
                                                                          0,                             /* tag_format - here is untag */
                                                                          FALSE                          /* is_ive                  */
                                                                          ), "");
    }

    return BCM_E_NONE;
}

int
vxlan_roo_basic_create_l3_interfaces(
    int unit)
{
    bcm_l3_ingress_t ingress_l3_interface;
    bcm_l3_ingress_t_init(&ingress_l3_interface);
    l3_ingress_intf ingr_itf_access;
    l3_ingress_intf ingr_itf_overlay;
    dnx_utils_l3_eth_rif_s eth_rif_structure;

    dnx_utils_l3_eth_rif_s_common_init(unit, 0, &eth_rif_structure, cint_vxlan_roo_basic_info.core_native_eth_rif, 0, 0, cint_vxlan_roo_basic_info.core_native_eth_rif_mac, cint_vxlan_roo_basic_info.tunnel_termination_vrf);
    if (cint_vxlan_roo_basic_info.ing_qos_id)
    {
        eth_rif_structure.ing_qos_map_id = cint_vxlan_roo_basic_info.ing_qos_id;
        eth_rif_structure.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_eth_rif_create(unit, &eth_rif_structure), "core_native_eth_rif");

    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_vxlan_roo_basic_info.core_overlay_eth_rif,
                             cint_vxlan_roo_basic_info.core_overlay_eth_rif_mac), "pwe_intf");

    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_vxlan_roo_basic_info.access_eth_rif,
                             cint_vxlan_roo_basic_info.access_eth_rif_mac), "pwe_intf");
    /*
     * Set Incoming ETH-RIF properties, VRF is updated for the rif.
     * This VRF value is used for IP-Tunnel-Term.VRF lookup key.
     */
    l3_ingress_intf_init(&ingr_itf_access);
    ingr_itf_access.intf_id = cint_vxlan_roo_basic_info.access_eth_rif;
    ingr_itf_access.vrf = cint_vxlan_roo_basic_info.access_eth_vrf;
    ingr_itf_access.qos_map = cint_vxlan_roo_basic_info.ing_qos_id;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf_access), "ingr_itf_access");

    l3_ingress_intf_init(&ingr_itf_overlay);
    ingr_itf_overlay.intf_id = cint_vxlan_roo_basic_info.core_overlay_eth_rif;
    ingr_itf_overlay.vrf = cint_vxlan_roo_basic_info.core_overlay_eth_vrf;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf_overlay), "ingr_itf_overlay");

    return BCM_E_NONE;

}

int
vxlan_roo_basic_create_eth_rifs(
    int unit)
{

    char error_msg[200]={0,};

    /*
     * create vlan based on the vsi (vpn)
     */
    snprintf(error_msg, sizeof(error_msg), "Failed to create VLAN %d", cint_vxlan_roo_basic_info.core_native_eth_rif);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, cint_vxlan_roo_basic_info.core_native_eth_rif), error_msg);

    /*
     * create vlan based on the vsi (vpn)
     */
    snprintf(error_msg, sizeof(error_msg), "Failed to create VLAN %d", cint_vxlan_roo_basic_info.core_overlay_eth_rif);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, cint_vxlan_roo_basic_info.core_overlay_eth_rif), error_msg);



    return BCM_E_NONE;
}


int vxlan_roo_my_mac_vrrp(
    int unit)
{
    int vrrp_ip_flags[2] = {BCM_L3_VRRP_IPV4, BCM_L3_VRRP_IPV6};
    /*
     *  Configure 00:00:5e:00:01:77 for IPv4 and 00:00:5e:00:02:77 for IPv6 as a mymac address for all VSIs.
     *  The regular Router A mac is configured as mymac when calling bcm_l3_intf_create.
     *  Devices below JR2 also have the option of supporting 00:00:5e:00:01:77 for IPv6.
     *
     */
    int vrid = 0x77;
    int ip_flag_iter;
    int nof_iters = 2;

    for(ip_flag_iter = 0; ip_flag_iter < nof_iters; ip_flag_iter++)
    {
        BCM_IF_ERROR_RETURN_MSG(bcm_l3_vrrp_config_add(unit, vrrp_ip_flags[ip_flag_iter] , cint_vxlan_roo_basic_info.access_eth_rif, vrid), "");

        BCM_IF_ERROR_RETURN_MSG(bcm_l3_vrrp_config_add(unit, vrrp_ip_flags[ip_flag_iter] , cint_vxlan_roo_basic_info.core_overlay_eth_rif, vrid), "");

        BCM_IF_ERROR_RETURN_MSG(bcm_l3_vrrp_config_add(unit, vrrp_ip_flags[ip_flag_iter] , cint_vxlan_roo_basic_info.core_native_eth_rif, vrid), "");
    }

    return BCM_E_NONE;
}

int l2_fwd_entry_add(
    int unit,
    bcm_mac_t mac,
    int vsi,
    int gport)
{
    bcm_l2_addr_t l2addr;
    int separate_fwd_learn_mact = *dnxc_data_get(unit, "l2", "general", "separate_fwd_learn_mact", NULL);

    bcm_l2_addr_t_init(&l2addr, mac, vsi);
    l2addr.port = gport;
    l2addr.flags = separate_fwd_learn_mact ? 0 : BCM_L2_STATIC;

    BCM_IF_ERROR_RETURN_MSG(bcm_l2_addr_add(unit, &l2addr), "");
    return BCM_E_NONE;
}

int vxlan_port_add(
    int unit,
    int core_port,
    int tunnel_term_id,
    bcm_gport_t *vxlan_port_id)
{
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;
    bcm_vxlan_port_t vxlan_port;

    bcm_vxlan_port_t_init(&vxlan_port);
    vxlan_port.criteria = BCM_VXLAN_PORT_MATCH_VN_ID;
    vxlan_port.flags = BCM_VXLAN_PORT_EGRESS_TUNNEL;
    vxlan_port.match_port = core_port;
    vxlan_port.match_tunnel_id = tunnel_term_id;
    vxlan_port.network_group_id = cint_vxlan_roo_basic_info.vxlan_network_group_id;
    BCM_L3_ITF_SET(vxlan_port.egress_if, BCM_L3_ITF_TYPE_FEC, cint_vxlan_roo_basic_info.core_overlay_fec_id);

    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_port_add(unit, cint_vxlan_roo_basic_info.vpn_id, &vxlan_port), "");

    *vxlan_port_id = vxlan_port.vxlan_port_id;

    /* Add to multicast group */
    int encap_id =  BCM_L3_ITF_VAL_GET(cint_vxlan_roo_basic_info.core_overlay_encap_tunnel_intf);
    BCM_IF_ERROR_RETURN_MSG(multicast__add_multicast_entry(unit, cint_vxlan_roo_basic_info.vpn_id, &core_port, &encap_id, 1, egress_mc), "mc_group_id");

    return BCM_E_NONE;
}

int vxlan_tunnel_initiator_create(
    int unit,
    bcm_if_t arp_itf,
    bcm_if_t *tunnel_intf,
    int *encap_tunnel_id)
{
    bcm_tunnel_initiator_t tunnel_init;
    bcm_l3_intf_t l3_intf;

    bcm_l3_intf_t_init(&l3_intf);
    /* Create IP tunnel initiator for encapsulating Vxlan tunnel header*/
    bcm_tunnel_initiator_t_init(&tunnel_init);
    if (cint_vxlan_roo_basic_info.tunnel_type == bcmTunnelTypeVxlan6 || cint_vxlan_roo_basic_info.tunnel_type == bcmTunnelTypeGeneve6)
    {
        tunnel_init.type = vxlan_roo_udf_tunnel_type? vxlan_roo_udf_tunnel_type:cint_vxlan_roo_basic_info.tunnel_type;
        tunnel_init.dip6 = cint_vxlan_roo_basic_info.tunnel_dip6; /* default: 2001:0DB8:0000:0000:0211:22FF:FE33:4455 */
        tunnel_init.sip6 = cint_vxlan_roo_basic_info.tunnel_sip6; /* default: 2001:0DB8:0000:0000:0211:22FF:FE77:8899 */
    } else {
        tunnel_init.type = cint_vxlan_roo_basic_info.tunnel_type;
        tunnel_init.dip = cint_vxlan_roo_basic_info.tunnel_dip; /* default: 171.0.0.1 */
        tunnel_init.sip = cint_vxlan_roo_basic_info.tunnel_sip; /* default: 172.0.0.1 */
    }
    tunnel_init.flags = 0;
    tunnel_init.dscp = 10; /* default: 10 */
    tunnel_init.egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
    tunnel_init.l3_intf_id = arp_itf;
    tunnel_init.ttl = 64; /* default: 64 */
    tunnel_init.outlif_counting_profile = BCM_STAT_LIF_COUNTING_PROFILE_NONE; /* default: -1 */
    tunnel_init.encap_access = bcmEncapAccessTunnel2;
    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_initiator_create(unit,&l3_intf, &tunnel_init), "");
    *tunnel_intf = l3_intf.l3a_intf_id;
    *encap_tunnel_id = tunnel_init.tunnel_id;

    /* update tunnel init orientation */
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, tunnel_init.tunnel_id, bcmPortClassForwardEgress, cint_vxlan_roo_basic_info.vxlan_network_group_id), "");

    return BCM_E_NONE;
}

/*
 * Main function for configuring the VXLAN VPN
 */
int vxlan_roo_basic_vxlan_open_vpn(
    int unit,
    int vpn_id,
    int vni)
{

    bcm_vxlan_vpn_config_t vpn_config;
    bcm_vxlan_vpn_config_t_init(&vpn_config);
    vpn_config.flags = BCM_VXLAN_VPN_WITH_ID|BCM_VXLAN_VPN_WITH_VPNID;
    vpn_config.vpn = vpn_id;
    vpn_config.broadcast_group = vpn_id;
    vpn_config.unknown_unicast_group = vpn_id;
    vpn_config.unknown_multicast_group = vpn_id;
    vpn_config.vnid = vni;
    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_vpn_create(unit, &vpn_config), "");

    /* Create multicast group */
    BCM_IF_ERROR_RETURN_MSG(multicast__open_mc_group(unit, &vpn_id, 0), "");

    return BCM_E_NONE;
}

int vxlan_tunnel_terminator_create(
    int unit,
    int *term_tunnel_id)
{
    bcm_tunnel_terminator_t tunnel_term;

    /* Create IP tunnel terminator for SIP,DIP, VRF lookup */
    bcm_tunnel_terminator_t_init(&tunnel_term);
    if (cint_vxlan_roo_basic_info.tunnel_type == bcmTunnelTypeVxlan6 || cint_vxlan_roo_basic_info.tunnel_type == bcmTunnelTypeGeneve6)
    {
        tunnel_term.type = vxlan_roo_udf_tunnel_type? vxlan_roo_udf_tunnel_type:cint_vxlan_roo_basic_info.tunnel_type;
        tunnel_term.dip6 = cint_vxlan_roo_basic_info.tunnel_dip6;
        tunnel_term.dip6_mask = cint_vxlan_roo_basic_info.tunnel_dip6_mask;
        tunnel_term.sip6 = cint_vxlan_roo_basic_info.tunnel_sip6;
        tunnel_term.sip6_mask = cint_vxlan_roo_basic_info.tunnel_sip6_mask;
    } else {
        tunnel_term.type = cint_vxlan_roo_basic_info.tunnel_type;
        tunnel_term.dip = cint_vxlan_roo_basic_info.tunnel_dip;
        tunnel_term.dip_mask = cint_vxlan_roo_basic_info.tunnel_dip_mask;
        tunnel_term.sip = cint_vxlan_roo_basic_info.tunnel_sip;
        tunnel_term.sip_mask = cint_vxlan_roo_basic_info.tunnel_sip_mask;
    }
    tunnel_term.vrf = cint_vxlan_roo_basic_info.core_overlay_eth_vrf;
    BCM_GPORT_TUNNEL_ID_SET(tunnel_term.tunnel_id, 0x3000);
    tunnel_term.flags = BCM_TUNNEL_TERM_TUNNEL_WITH_ID;
    /* check term bud */
    if (cint_vxlan_roo_basic_info.term_bud) {
        tunnel_term.flags |= BCM_TUNNEL_TERM_BUD;
    }
    if (cint_vxlan_roo_basic_info.ing_qos_id) {
        tunnel_term.ingress_qos_model.ingress_remark = bcmQosIngressModelPipe;
    }

    BCM_IF_ERROR_RETURN_MSG(bcm_tunnel_terminator_create(unit,&tunnel_term), "");

    *term_tunnel_id = tunnel_term.tunnel_id;

    /* Set ingress qos profile */
    if (cint_vxlan_roo_basic_info.ing_qos_id) {
        BCM_IF_ERROR_RETURN_MSG(bcm_qos_port_map_set(unit, tunnel_term.tunnel_id, cint_vxlan_roo_basic_info.ing_qos_id, -1),
            "for tunnel termination");
    }

    return BCM_E_NONE;
}

int vlan_port_add(int unit, bcm_gport_t in_port, bcm_vlan_t vid, bcm_gport_t *port_id){
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);

    /* the match criteria is port:1, out-vlan:510   */
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = in_port;
    vlan_port.match_vlan = vid;
    vlan_port.flags = 0;
    vlan_port.vsi = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");


    /* handle of the created gport */
    *port_id = vlan_port.vlan_port_id;

    return BCM_E_NONE;
}

int vswitch_add_port(int unit, bcm_vlan_t vsi, bcm_gport_t gport){

    /* add to vswitch */
    BCM_IF_ERROR_RETURN_MSG(bcm_vswitch_port_add(unit, vsi, gport), "");

    return BCM_E_NONE;
}

/*
 * Default egress outer VLAN edit
 */
int vxlan_roo_default_out_ac_allocate_and_set(
    int unit,
    int port)
{
    int vlan_edit_profile = 4;
    int tag_format_untag = 0;
    int new_outer_vlan = 16;
    int new_inner_vlan = 32;

    /* Create out AC LIF */
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_DEFAULT | BCM_VLAN_PORT_VLAN_TRANSLATION;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");

    /* Add match */
    bcm_port_match_info_t port_match_info;
    bcm_port_match_info_t_init(&port_match_info);

    port_match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY;
    port_match_info.match = BCM_PORT_MATCH_PORT; /* indicate we set default out ac per port */
    port_match_info.match_vlan = BCM_VLAN_ALL;
    port_match_info.port = port;

    BCM_IF_ERROR_RETURN_MSG(bcm_port_match_add(unit, vlan_port.vlan_port_id, &port_match_info), "");

    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set(unit, vlan_port.vlan_port_id,      /* lif  */
                                                       0x8100,                     /* outer_tpid */
                                                       0x9100,                     /* inner_tpid */
                                                       bcmVlanActionAdd,           /* outer_action */
                                                       bcmVlanActionAdd,           /* inner_action */
                                                       new_outer_vlan,             /* new_outer_vid */
                                                       new_inner_vlan,             /* new_inner_vid */
                                                       vlan_edit_profile,          /* vlan_edit_profile */
                                                       tag_format_untag,           /* tag_format - here is untag */
                                                       FALSE                       /* is_ive */
                                                       ), "");
    return BCM_E_NONE;
}

/* modify current configuration to build the native ethernet header with specific number of vlans:
 * - update EVE. Add "nbr_native_vlans" vlans. (0 or 1 tag)
 */
int vxlan_roo_modify_native_vlan_number(int unit, int nbr_native_vlans) {
    uint32 flags = BCM_VLAN_ACTION_SET_EGRESS;
    bcm_vlan_action_set_t action;

    bcm_vlan_action_t outer_action, inner_action;
    if (nbr_native_vlans == 1) {
        outer_action = bcmVlanActionAdd;
        inner_action = bcmVlanActionNone;
    } else if (nbr_native_vlans == 0) {
        outer_action = bcmVlanActionNone;
        inner_action = bcmVlanActionNone;
    } else {
        printf("Error, invalid number of native vlans \n");
        return BCM_E_PARAM;
    }

    bcm_vlan_action_set_t_init(&action);
    action.dt_outer = outer_action;
    action.dt_inner = inner_action;
    action.outer_tpid = 0x8100;
    action.inner_tpid = 0x9100;

    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_id_set(unit, flags, native_eve_action_id, &action), "");

    return BCM_E_NONE;
}

/*
 * Main function for destroying the VXLAN VPN
 */
int
vxlan_port_destroy(
    int unit)
{

    /* Delete VXLAN port */
    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_port_delete(unit, cint_vxlan_roo_basic_info.vpn_id, cint_vxlan_roo_basic_info.vxlan_port_id), "");

    /* Destroy VPN */
    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_vpn_destroy(unit, cint_vxlan_roo_basic_info.vpn_id), "");

    return BCM_E_NONE;
}


/*
 * The VXLAN ROO NNI to NNI example with parameters:
 * Unit - relevant unit
 * core_port_in - ingress port where core router is connected to, for termination
 * core_port_out - egress port where core router is connected to, for encapsulation
 * access_port - port where host is connected to.
 *
 * cint_vxlan_roo_basic_info.tunnel_type - define VXLAN6 or VXLAN, GENEVE6 or GENEVE tunnel to be created
 */
int vxlan_roo_nni2nni_example(
    int unit,
    int core_port_in,
    int core_port_out,
    int access_port1,
    int access_port2)
{
    bcm_if_t encap_tunnel_intf;
    int term_tunnel_id;
    int encap_tunnel_id;
    bcm_gport_t vlan_port_id;
    bcm_gport_t vxlan_port_id;
    l3_ingress_intf ingr_itf;
    bcm_gport_t tunnel_id;
    int egress_core_native_arp_id;
    int egress_core_overlay_arp_id;
    int egress_core_native_fec_id;
    int egress_core_overlay_fec_id;
    bcm_gport_t gport;
    int egress_core_overlay_eth_rif = 200;
    bcm_mac_t egress_core_overlay_eth_rif_mac = {0x00, 0x00, 0x00, 0x00, 0xAA, 0xCC};
    bcm_mac_t egress_core_native_next_hop_mac = {0x00, 0x00, 0x00, 0x11, 0xCD, 0x1D};
    bcm_mac_t egress_core_overlay_next_hop_mac = {0x00, 0x00, 0x00, 0x22, 0xCD, 0x1D};
    bcm_ip_t dip_v4 = 0x7fffff05; /* 127.255.255.05 to egress core*/
    bcm_ip6_t dip_v6 = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x20 };


    vxlan_roo_basic_init(access_port1, access_port2, core_port_in);

    /*
     * Configure port
     */
    BCM_IF_ERROR_RETURN_MSG(vxlan_roo_basic_configure_port_properties(unit, cint_vxlan_roo_basic_info.access_port1, cint_vxlan_roo_basic_info.access_port2,
                                                  cint_vxlan_roo_basic_info.core_port), "");

    /*
     * Set egress core port default properties
     */
    BCM_IF_ERROR_RETURN_MSG(out_port_set(unit, core_port_out), "intf_out");

    /*
     * Configure L3 interfaces
     */
    BCM_IF_ERROR_RETURN_MSG(vxlan_roo_basic_create_l3_interfaces(unit), "");

    /* Egress core side overlay RIF */
    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, egress_core_overlay_eth_rif, egress_core_overlay_eth_rif_mac), "pwe_intf");

    /* Create multicast group */
    BCM_IF_ERROR_RETURN_MSG(multicast__open_mc_group(unit, &cint_vxlan_roo_basic_info.vpn_id, 0), "");

    /* Create L2VPN */
    bcm_vxlan_vpn_config_t vpn_config;
    bcm_vxlan_vpn_config_t_init(&vpn_config);
    vpn_config.flags = BCM_VXLAN_VPN_WITH_ID|BCM_VXLAN_VPN_WITH_VPNID;
    vpn_config.vpn = cint_vxlan_roo_basic_info.vpn_id;
    vpn_config.broadcast_group = cint_vxlan_roo_basic_info.vpn_id;
    vpn_config.unknown_unicast_group = cint_vxlan_roo_basic_info.vpn_id;
    vpn_config.unknown_multicast_group = cint_vxlan_roo_basic_info.vpn_id;
    vpn_config.vnid = BCM_VXLAN_VNI_INVALID;
    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_vpn_create(unit,&vpn_config), "");

    /* Configure ingress VNI to VSI mapping */
    bcm_vxlan_network_domain_config_t vxlan_vpn_config;
    bcm_vxlan_network_domain_config_t_init(&vxlan_vpn_config);
    vxlan_vpn_config.network_domain  = 0;
    vxlan_vpn_config.vni = cint_vxlan_roo_basic_info.vni;
    vxlan_vpn_config.vsi = cint_vxlan_roo_basic_info.vpn_id;
    vxlan_vpn_config.flags = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_INGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING;

    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_network_domain_config_add(unit, &vxlan_vpn_config), "");

    /* Configure egress VSI to VNI mapping */
    cint_vxlan_roo_basic_info.vni++; /* Use different VNI than ingress */
    bcm_vxlan_network_domain_config_t_init(&vxlan_vpn_config);
    vxlan_vpn_config.network_domain  = 0;
    vxlan_vpn_config.vni = cint_vxlan_roo_basic_info.vni;
    vxlan_vpn_config.vsi = cint_vxlan_roo_basic_info.vpn_id;
    vxlan_vpn_config.flags = BCM_VXLAN_NETWORK_DOMAIN_CONFIG_EGRESS_ONLY | BCM_VXLAN_NETWORK_DOMAIN_CONFIG_L2_MAPPING;

    BCM_IF_ERROR_RETURN_MSG(bcm_vxlan_network_domain_config_add(unit, &vxlan_vpn_config), "");

    /* VXLAN tunnel for ingress core side */

    /*
     * Create the tunnel termination lookup and the tunnel termination inlif
     */
    BCM_IF_ERROR_RETURN_MSG(vxlan_tunnel_terminator_create(unit, &term_tunnel_id), "");

    /*
     *Configure ARP entries,
     */
    BCM_IF_ERROR_RETURN_MSG(vxlan_roo_basic_create_arp_entries(unit), "");

    /*
     * Create the tunnel initiator
     */
    BCM_IF_ERROR_RETURN_MSG(vxlan_tunnel_initiator_create(unit, cint_vxlan_roo_basic_info.core_overlay_arp_id, &(cint_vxlan_roo_basic_info.core_overlay_encap_tunnel_intf), &encap_tunnel_id), "");

    /*
     * Configure fec entry
     */
    BCM_IF_ERROR_RETURN_MSG(vxlan_roo_basic_create_fec_entries(unit), "");

    /*
     * Add VXLAN port properties and configure the tunnel terminator In-Lif and FEC
     */
    BCM_IF_ERROR_RETURN_MSG(vxlan_port_add(unit, core_port_in, term_tunnel_id, &vxlan_port_id), "");

    /* VXLAN tunnel for egress core side */

    /* Configure ARP */
     /*
     * Configure native ARP entry
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, 0, &egress_core_native_arp_id,egress_core_native_next_hop_mac,0,BCM_L3_NATIVE_ENCAP,0),
        "create AC egress object ARP only");

    /*
     * Configure overlay ARP entry
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, 0, &egress_core_overlay_arp_id,egress_core_overlay_next_hop_mac,egress_core_overlay_eth_rif),
        "create AC egress object ARP only");

    /*
     * Create the tunnel initiator, different tunnels
     */
    cint_vxlan_roo_basic_info.tunnel_dip6[15]++;
    cint_vxlan_roo_basic_info.tunnel_sip6[15]++;
    cint_vxlan_roo_basic_info.tunnel_dip++;
    cint_vxlan_roo_basic_info.tunnel_sip++;
    BCM_IF_ERROR_RETURN_MSG(vxlan_tunnel_initiator_create(unit, egress_core_overlay_arp_id, &(cint_vxlan_roo_basic_info.core_overlay_encap_tunnel_intf), &encap_tunnel_id), "");

    /*
     * Create FEC
     */
    egress_core_native_fec_id = cint_vxlan_roo_basic_info.core_native_fec_id + 10;
    egress_core_overlay_fec_id = cint_vxlan_roo_basic_info.core_overlay_fec_id + 15;
    int fwd_flag = (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL)) ? BCM_L3_FLAGS2_FWD_ONLY : 0;
    BCM_GPORT_FORWARD_PORT_SET(gport, egress_core_overlay_fec_id);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, egress_core_native_fec_id,
                                     cint_vxlan_roo_basic_info.core_native_eth_rif,
                                     egress_core_native_arp_id, gport, 0, fwd_flag),
                                     "in creating native FEC for routing into overlay");


    gport = 0;
    BCM_GPORT_LOCAL_SET(gport, core_port_out);
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, egress_core_overlay_fec_id,
                                     cint_vxlan_roo_basic_info.core_overlay_encap_tunnel_intf,
                                     0, gport, BCM_L3_CASCADED, fwd_flag),
                                     "in creating overlay FEC for routing into overlay");

    /*
     * Configure host/route entries
     */
    BCM_IF_ERROR_RETURN_MSG(vxlan_roo_basic_l3_forwarding(unit), "");

    /*
     * Add IPv4 host entry for core to core traffic
     */
    BCM_IF_ERROR_RETURN_MSG(add_host_ipv4(unit, dip_v4, cint_vxlan_roo_basic_info.tunnel_termination_vrf, egress_core_native_fec_id, 0, 0), "");

    /*
     * Add IPv6 host entry for core to core traffic
     */
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit, dip_v6, cint_vxlan_roo_basic_info.maskv6_dip_out_of_overlay, cint_vxlan_roo_basic_info.tunnel_termination_vrf, egress_core_native_fec_id), "");

    return BCM_E_NONE;
}

/*
 * VXLAN roo qos example, specifically
 *   access to core: ingress L3 mapping, egress native ETH mapping
 *   core to access: ingress native L3 mapping, egress ETH mapping
 */
int vxlan_roo_qos(
    int unit,
    int is_ipv6_fwd,
    int access_port,
    int core_port)
{

    l3_ingress_intf ingr_itf_access;
    l3_ingress_intf ingr_itf_native_overlay;
    bcm_vlan_port_t vlan_port;
    int gport;

    /* Basic VXLAN roo config */
    BCM_IF_ERROR_RETURN_MSG(vxlan_roo_basic(unit, access_port, 0, core_port), "");

    /* Ingress L3 profile and mapping */
    BCM_IF_ERROR_RETURN_MSG(qos_map_l3_ingress_profile(unit, is_ipv6_fwd), "");

    /* Update access inRIF's qos profile, access -> core */
    l3_ingress_intf_init(&ingr_itf_access);
    ingr_itf_access.intf_id = cint_vxlan_roo_basic_info.access_eth_rif;
    ingr_itf_access.vrf = cint_vxlan_roo_basic_info.access_eth_vrf;
    ingr_itf_access.qos_map = l3_in_map_id;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf_access), "ingr_itf_access");

    /* Update native inRIF's qos profile, core -> access */
    l3_ingress_intf_init(&ingr_itf_native_overlay);
    ingr_itf_native_overlay.intf_id = cint_vxlan_roo_basic_info.core_native_eth_rif;
    ingr_itf_native_overlay.vrf = cint_vxlan_roo_basic_info.tunnel_termination_vrf;
    ingr_itf_native_overlay.qos_map = l3_in_map_id;
    BCM_IF_ERROR_RETURN_MSG(intf_ingress_rif_set(unit, &ingr_itf_native_overlay), "ingr_itf_native_overlay");

    /* Egress L2 profile and mapping */
    BCM_IF_ERROR_RETURN_MSG(qos_l2_egress_map_create(unit, 0), "");

    /* Update egress VXLAN tunne's qos profile to affect native AC */
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, cint_vxlan_roo_basic_info.core_overlay_encap_tunnel_intf);
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_port_map_set(unit, gport, -1, l2_eg_map_id), "");

    /* Create egress access AC */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
    vlan_port.port = access_port;
    vlan_port.vsi = cint_vxlan_roo_basic_info.access_eth_rif;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");

    /* Update egress AC's qos profile, core -> access */
    BCM_IF_ERROR_RETURN_MSG(bcm_qos_port_map_set(unit, vlan_port.vlan_port_id, -1, l2_eg_map_id), "");

    /* Create EVE */
    BCM_IF_ERROR_RETURN_MSG(vlan_translate_ive_eve_translation_set_with_pri_action(unit, vlan_port.vlan_port_id,        /* outlif                  */
                                                                      0x8100,                        /* outer_tpid              */
                                                                      0x9100,                        /* inner_tpid              */
                                                                      bcmVlanTpidActionModify,       /* Outer tpid action modify*/
                                                                      bcmVlanTpidActionNone,         /* Inner tpid action none  */
                                                                      bcmVlanActionAdd,              /* outer_action            */
                                                                      bcmVlanActionNone,             /* inner_action            */
                                                                      bcmVlanActionAdd,              /* outer_pri_action        */
                                                                      bcmVlanActionNone,             /* inner_pri_action        */
                                                                      cint_vxlan_roo_basic_info.access_vid,    /* new_outer_vid */
                                                                      0,                             /* new_inner_vid           */
                                                                      cint_vxlan_roo_basic_info.egress_vlan_edit_profile, /* vlan_edit_profile */
                                                                      0,                             /* tag_format - here is untag */
                                                                      FALSE                          /* is_ive                  */
                                                                      ), "");

    return BCM_E_NONE;
}
