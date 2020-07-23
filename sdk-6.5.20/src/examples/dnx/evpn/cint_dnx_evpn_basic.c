/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * File: cint_dnx_evpn_basic.c
 * Purpose: Example of EVPN basic configuration.
 */
/*
 * This cint demonstrate configuration of a PE in a EVPN network. It is meant to copy the configuration
 * demonstrated in cint_evpn.c, but with the DNX style L3 setup. To see the emulated network topology and
 * handled flows, please refer to cint_evpn.c documentation. The configuration steps may be a little
 * different here, but the flows and topology should remain the same.
 */

/*
 * Test Scenario
 *
 * cint src/examples/sand/utility/cint_sand_utils_global.c
 * cint src/examples/sand/utility/cint_sand_utils_global.c
 * cint src/examples/sand/utility/cint_sand_utils_l3.c
 * cint src/examples/sand/utility/cint_sand_utils_vlan.c
 * cint src/examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint src/examples/sand/utility/cint_sand_utils_multicast.c
 * cint src/examples/sand/cint_ip_route_basic.c
 * cint src/examples/dnx/utility/cint_dnx_utility_mpls.c
 * cint src/examples/dnx/field/cint_field_evpn_esi.c
 * cint src/examples/dnx/evpn/cint_dnx_evpn_basic.c
 * cint
 * evpn_main(0,200,201,202,203);
 * exit;
 *
 * AC -> EVPN UC
 * tx 1 psrc=200 data=0x00000000011300000000011281000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x000000000115001100000111810000148847014500fe01f401fe00000000011300000000011281000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 *
 * EVPN -> AC UC
 * tx 1 psrc=202 data=0x0011000001110000000001158100001488470145004001f4014000000000011200000000011381000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00000000011200000000011381000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *
 * AC -> Unknown-MC
 * tx 1 psrc=200 data=0x00000000011700000000011281000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00000000011700000000011281000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 * Data: 0x000000000115001100000111810000148847014b40fe0189c1fe00000000011700000000011281000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 * Data: 0x000000000116001100000111810000148847013ec0fe017d40fe000c810100000000011700000000011281000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 * Data: 0x000000000115001100000111810000148847014500fe018381fe00000000011700000000011281000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 *
 * MC (no ESI) -> Local Flooding
 * tx 1 psrc=202 data=0x001100000111000000000116810000158847013ec040017d414000000000011800000000011781000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00000000011800000000011781000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * Data: 0x00000000011800000000011781000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *
 * AC -> Unknown-MC
 * tx 1 psrc=200 data=0x00000000011700000000011281000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00000000011700000000011281000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 * Data: 0x000000000115001100000111810000148847014b40fe0189c1fe00000000011700000000011281000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 * Data: 0x000000000116001100000111810000148847013ec0fe017d40fe000c810100000000011700000000011281000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 * Data: 0x000000000115001100000111810000148847014500fe018381fe00000000011700000000011281000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 *
 * MC (ESI) -> Local Flooding
 * tx 1 psrc=203 data=0x001100000111000000000116810000158847013ec040017d4040000c814000000000011800000000011781000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00000000011800000000011781000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 *
 * AC -> Unknown-MC
 * tx 1 psrc=200 data=0x00000000011700000000011281000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 *
 * Received packets on unit 0 should be:
 * Data: 0x00000000011700000000011281000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 * Data: 0x000000000115001100000111810000148847014b40fe0189c1fe00000000011700000000011281000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 * Data: 0x000000000116001100000111810000148847013ec0fe017d40fe000c810100000000011700000000011281000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 * Data: 0x000000000115001100000111810000148847014500fe018381fe00000000011700000000011281000014ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 */

/** For testting only.*/
int extra_esi_label = 400;
int extra_port_id = 13;
int has_cw = 0;
int has_fl = 0;
int is_evpn_etree = 0;


/*
 * General structure to hold the objects generated to implement
 * the scenario described in cint_evpn.c.
 */
struct evpn_info_s {
    int ce1_port; /* ce1 port (access)*/
    int ce3_port; /* ce3 port (access) */
    int p1_port;  /* p1 port (provider) */
    int p2_port;  /* p2 port (provider) */
    int ce1_vlan; /* ce1's vlan */
    int ce2_vlan; /* ce2's vlan */
    int ce3_vlan; /* ce3's vlan */
    int p1_vlan;  /* p1's vlan */
    int p2_vlan;  /* p2's vlan */
    int native_vlan;  /* native vlan */
    int p1_inner_vlan; /* p1's inner vlan*/
    int p2_inner_vlan; /* p2's inner vlan*/
    int tpid;     /* tpid value for all vlan tags in the application*/
    int vsi; /* vsi representing the service in this application */
    int pe2_id; /* pe2's id */
    int pe3_id; /* pe3's id */
    int pe4_id; /* pe4's id */
    int pe1_pe2_ce3_esi_label; /* The esi of pe1, pe2, ce3 */
    int pe1_pe2_ce1_esi_label; /* The esi of pe1, pe2, ce1 */
    bcm_mac_t global_mac; /* global mac of PE1 */
    bcm_mac_t my_mac_lsb; /* PE1's my mac lsb facing p1 */
    bcm_mac_t my_mac; /* PE1's my mac facing p1 */
    bcm_mac_t my_mac2; /* PE1's my mac facing p1 */
    bcm_mac_t ce1_mac;  /* ce1's mac address*/
    bcm_mac_t ce2_mac; /* ce2's mac address*/
    bcm_mac_t ce3_mac ; /* ce3's mac address*/
    bcm_mac_t p1_mac; /* p1's mac address*/
    bcm_mac_t p2_mac; /* p2's mac address*/
    bcm_mac_t pe3_mac; /* pe3's mac address*/
    bcm_if_t pe1_rif; /* rif of flooding domain */
    bcm_if_t pe1_rif2; /* rif of flooding domain */
    bcm_gport_t p1_vport_id; /* vlan port id of p1's AC */
    bcm_gport_t p2_vport_id; /* vlan port id of p2's AC */
    bcm_gport_t ce1_vport_id; /* vlan port id of ce1's AC */
    bcm_gport_t ce3_vport_id; /* vlan port id of ce3's AC */
    bcm_mpls_label_t evpn_label; /* EVPN label for ingress and egress pe */
    bcm_mpls_label_t pe1_pe2_lsp_label; /* lsp label in pe1<-->pe2 */
    bcm_mpls_label_t pe1_pe3_lsp_label; /* lsp label in pe1<-->pe3 */
    bcm_mpls_label_t pe1_pe4_lsp_label; /* lsp label in pe1<-->pe4 */
    bcm_mpls_label_t pe1_pe2_lsp2_label; /* lsp2 label in pe1<-->pe2 */
    bcm_mpls_label_t pe1_pe3_lsp2_label; /* lsp2 label in pe1<-->pe3 */
    bcm_mpls_label_t pe1_pe2_iml_label; /* iml label in pe1<-->pe2 */
    bcm_mpls_label_t pe1_pe3_iml_label; /* lsp label in pe1<-->pe3 */
    bcm_mpls_label_t pe1_pe4_iml_label; /* lsp label in pe1<-->pe4 */
    bcm_mpls_label_t iml_range_low_label; /* low bound of iml range in egress pe */
    bcm_mpls_label_t iml_range_high_label; /* high bound of iml range in egress pe */
    bcm_mpls_label_t pe1_frr_label; /* frr (fast re-route) label for tunnel starting at pe1  */
    bcm_mpls_label_t pe2_frr_label; /* frr (fast re-route) label for tunnel starting at pe2  */
    uint32 evpn_profile; /* user defined evpn profile : 0-7 */
    bcm_ip_t ce2_ip; /* ce2's ip */ 
    bcm_ip_t mc_ip;  /* multicast ip for multicast routing into evpn */
    int pe1_linker_layer_encap_id; /* Linker layer encap id for pe1 */
    int pe2_linker_layer_encap_id; /* Linker layer encap id for pe2 */
    int pe1_pe3_iml_no_esi_tunnel_id; /* Egress Tunnel id of pe1<-->pe3 iml over lsp (no esi) */
    int pe1_pe4_iml_no_esi_tunnel_id; /* Egress Tunnel id of pe1<-->pe4 iml over lsp (no esi) */
    int pe1_pe2_iml_esi_tunnel_id;    /* Egress Tunnel id of pe1<-->pe2 iml over lsp (with esi) */
    int pe1_pe3_evpn_lsp_tunnel_id;   /* Egress Tunnel id of pe1<-->pe3 evpn over lsp */
    bcm_gport_t pe1_pe2_iml_label_term_tunnel_id; /* Tunnel id of pe1_pe2_iml terminated label */
    bcm_gport_t pe1_pe2_iml_label_term_tunnel_id_2; /* Tunnel id of pe1_pe2_iml terminated label */
    int pe1_pe3_evpn_lsp_fec;         /* Overlay FEC pointing to evpn over lsp tunnel */ 
    int pe1_ce2_native_fec;           /* Native FEC pointing to Overlay FEC pointing to evpn over lsp tunnel*/
    bcm_multicast_t mc_id;            /* multicast id for routing into evpn bum traffic copies*/    


};

struct evpn_global_info_s
{
    bcm_vlan_port_t ce1_vlan_port;
    bcm_vlan_port_t ce3_vlan_port;
    bcm_mpls_egress_label_t evpn_egress_label;
    bcm_mpls_egress_label_t pe1_pe2_iml_egress_label;
    bcm_mpls_egress_label_t pe1_pe3_iml_egress_label;
    bcm_mpls_egress_label_t pe1_pe4_iml_egress_label;
    bcm_mpls_tunnel_switch_t evpn_in_tunnel;
    bcm_mpls_tunnel_switch_t pe1_pe2_iml_tunnel_switch_no_esi;
    bcm_mpls_tunnel_switch_t pe1_pe2_iml_tunnel_switch_esi;
    bcm_vlan_port_t default_native_ac;
    int in_lif_acl_profile_esi;
    int dual_homed_peer_profile;
    int use_split_horizon;
    int evpn_network_group_id;
};

struct evpn_etree_additional_info_s
{
    int etree_flush_group;   /** Make sense for etree only. Suppose the MSB is used.*/
    bcm_mac_t ce4_mac;      /* ce4's mac address*/
    int pe1_pe2_ce3_leaf_label; /* The Leaf label of pe1, pe2, ce3 */
    int pe1_pe3_ce3_leaf_label; /* The Leaf label of pe1, pe3, ce3 */
    int pe1_pe4_ce3_leaf_label; /* The Leaf label of pe1, pe4, ce3 */
};


evpn_info_s cint_evpn_info = 
/* ports :
   ce1_port | ce3_port | p1_port | p2_port  */
    {200,      201,        202 ,     203,               
/* vlans:
   ce1_vlan | ce2_vlan | ce3_vlan | p1_vlan | p2_vlan | native_vlan | p1_inner_vlan | p2_inner_vlan */
    20,          20,        20,        20,      20,          30,           40,              50,
/* tpid */
   0x8100,
/* vsi */
    20, 
/* pe2_id  | pe3_id  |  pe4_id*/
    100,      125,        150, 
/* pe1_pe2_ce3_esi_label    |  pe1_pe2_ce1_esi_label */ 
            300,                       200,     
/*               global_mac              |            my_mac_lsb             |               my_mac                | */              
    {0x00, 0x11, 0x00, 0x00, 0x00, 0x00}, {0x00, 0x00, 0x00, 0x00, 0x01, 0x11}, {0x00, 0x11, 0x00, 0x00, 0x01, 0x11},
/*                 my_mac2                             ce1_mac                               ce2_mac                 */
    {0x00, 0x11, 0x00, 0x00, 0x01, 0x18}, {0x00, 0x00, 0x00, 0x00, 0x01, 0x12}, {0x00, 0x00, 0x00, 0x00, 0x01, 0x13},
/*                 ce3_mac                              p1_mac                                p2_mac                 */     
    {0x00, 0x00, 0x00, 0x00, 0x01, 0x14},{0x00, 0x00, 0x00, 0x00, 0x01, 0x15}, {0x00, 0x00, 0x00, 0x00, 0x01, 0x16}, 
/*                 pe3_mac              */
    {0x00, 0x00, 0x00, 0x00, 0x01, 0x17},
/*  pe1_rif | pe1_rif2 */ 
       0,        0,
/* p1_vport_id | p2_vport_id | ce1_vport_id | ce3_vport_id */
        0,            0 ,           0,              0, 
/* evpn_label | pe1_pe2_lsp_label | pe1_pe3_lsp_label | pe1_pe4_lsp_label */                                                                                                                 
       8000,          5100,               5200,               5300,
/* pe1_pe2_lsp2_label | pe1_pe3_lsp2_label */
       4100,                 4200,
/* pe1_pe2_iml_label | pe1_pe3_iml_label | pe1_pe4_iml_label | iml_range_low_label */ 
         6100,               6200,               6300,                6000,
/* iml_range_high_label | pe1_frr_label | pe2_frr_label */                                                                         
       7000,                  828300,            828600, 
/* evpn_profile */
        1,
/*   ce2_ip  */
   0xA0A1A1A2 /* 160.161.161.162 */,
/*   mc_ip   */ 
   0xE0000103 /* 224.0.1.3 */,
/* pe1_linker_layer_encap_id | pe2_linker_layer_encap_id*/
              0,                          0, 
/* pe1_pe3_iml_no_esi_tunnel_id | pe1_pe4_iml_no_esi_tunnel_id | pe1_pe2_iml_esi_tunnel_id | pe1_pe3_evpn_lsp_tunnel_id | pe1_pe2_iml_label_term_tunnel_id | pe1_pe2_iml_label_term_tunnel_id_2 */
              0,                              0,                            0,                            0,                              0,                                0,
/* pe1_pe3_evpn_lsp_fec */
             0,
/* pe1_ce2_native_fec */
             0,
/* mc_id */
   10960,
};

evpn_etree_additional_info_s evpn_etree_info = 
{
    /** etree_flush_group  | ce4_mac  */
    1 << 3,                 {0x00, 0x00, 0x00, 0x00, 0x04, 0x13},
    /** pe1_pe2_ce3_leaf | pe1_pe3_ce3_leaf | pe1_pe4_ce3_leaf  */
    301,                   302,               303,
};

evpn_global_info_s evpn_global_info;


int
init_evpn_mpls_util_params(
    int unit)
{
    int rv = BCM_E_NONE;

    printf("Configuring Entity 0 - MPLS tunnels from PE1 to PE3, PE4 through P1\n");
    mpls_util_entity[0].arps[0].next_hop = cint_evpn_info.p1_mac;
    printf("- PE1 to PE3,PE4 use ARP+AC entry for this tunnel, to prevent overriding the IML ESI lookup,\n");
    printf("  by setting BCM_L3_FLAGS2_VLAN_TRANSLATION for the l3_egress object.\n");
    mpls_util_entity[0].arps[0].flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;
    mpls_util_entity[0].rifs[0].mac_addr = cint_evpn_info.my_mac;
    mpls_util_entity[0].rifs[0].intf = cint_evpn_info.vsi;
    printf("- Configure MPLS LSP encapsulation 0\n");
    mpls_util_entity[0].mpls_encap_tunnel[0].num_labels = 1;
    mpls_util_entity[0].mpls_encap_tunnel[0].label[0] = cint_evpn_info.pe1_pe3_lsp_label;
    mpls_util_entity[0].mpls_encap_tunnel[0].encap_access = bcmEncapAccessTunnel2;
    mpls_util_entity[0].mpls_encap_tunnel[0].l3_intf_id = &mpls_util_entity[0].arps[0].arp;
    printf("  - Reconnect entity 0 to it's own out port\n");
    mpls_util_entity[0].fecs[0].port = &mpls_util_entity[0].ports[0].port;
    printf("- Configure another MPLS tunnel for entity 0\n");
    mpls_util_entity[0].mpls_encap_tunnel[1].num_labels = 1;
    mpls_util_entity[0].mpls_encap_tunnel[1].label[0] = cint_evpn_info.pe1_pe4_lsp_label;
    mpls_util_entity[0].mpls_encap_tunnel[1].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    mpls_util_entity[0].mpls_encap_tunnel[1].encap_access = bcmEncapAccessTunnel2;
    mpls_util_entity[0].mpls_encap_tunnel[1].l3_intf_id = &mpls_util_entity[0].arps[0].arp;
    printf("- Configure another fec for entity 0\n");
    mpls_util_entity[0].fecs[1].fec_id = 0;
    mpls_util_entity[0].fecs[1].port = &mpls_util_entity[0].ports[0].port;
    mpls_util_entity[0].fecs[1].tunnel_gport = &mpls_util_entity[0].mpls_encap_tunnel[1].tunnel_id;
    mpls_util_entity[0].fecs[1].tunnel_gport2 = &MPLS_UTIL_INVALID_GPORT;
    printf("- Configure MPLS LSP termination\n");
    mpls_util_entity[0].mpls_switch_tunnel[0].action = BCM_MPLS_SWITCH_ACTION_POP;
    mpls_util_entity[0].mpls_switch_tunnel[0].label = cint_evpn_info.pe1_pe3_lsp_label;
    mpls_util_entity[0].mpls_switch_tunnel[0].vrf = 1;

    printf("Configure Entity 1 - MPLS tunnel from PE1 to PE2 through P2\n");
    mpls_util_entity[1].arps[0].next_hop = cint_evpn_info.p2_mac;
    printf("- PE1 to PE2 may need ESI, so using ARP+AC entry for this tunnel\n");
    printf("  by setting BCM_L3_FLAGS2_VLAN_TRANSLATION for the l3_egress object.\n");
    mpls_util_entity[1].arps[0].flags2 |= BCM_L3_FLAGS2_VLAN_TRANSLATION;
    mpls_util_entity[1].rifs[0].mac_addr = cint_evpn_info.my_mac;
    mpls_util_entity[1].rifs[0].intf = cint_evpn_info.vsi + 1;
    printf("- Configure MPLS LSP encapsulation 0\n");
    mpls_util_entity[1].mpls_encap_tunnel[0].num_labels = 1;
    mpls_util_entity[1].mpls_encap_tunnel[0].label[0] = cint_evpn_info.pe1_pe2_lsp_label;
    printf("  this tunnel is encapsulated.\n");
    mpls_util_entity[1].mpls_encap_tunnel[0].flags |=
            BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    mpls_util_entity[1].mpls_encap_tunnel[0].encap_access = bcmEncapAccessTunnel2;
    mpls_util_entity[1].mpls_encap_tunnel[0].l3_intf_id = &mpls_util_entity[1].arps[0].arp;
    printf("  - Reconnect entity 1 to it's own out port\n");
    mpls_util_entity[1].fecs[0].port = &mpls_util_entity[1].ports[0].port;
    printf("- Reset entity 1 fec 0");
    mpls_util_entity[1].fecs[0].tunnel_gport = &mpls_util_entity[1].mpls_encap_tunnel[0].tunnel_id;
    mpls_util_entity[1].fecs[0].tunnel_gport2 = &MPLS_UTIL_INVALID_GPORT;
    printf("- Configure MPLS LSP termination\n");
    mpls_util_entity[1].mpls_switch_tunnel[0].action = BCM_MPLS_SWITCH_ACTION_POP;
    mpls_util_entity[1].mpls_switch_tunnel[0].label = cint_evpn_info.pe1_pe2_lsp_label;
    mpls_util_entity[1].mpls_switch_tunnel[0].vrf = 2;

    /* When JR2 works under IOP mode, use FEC format:dest+EEI */
    if ((soc_property_get(unit , "system_headers_mode",1) == 0)) {
        mpls_util_entity[0].fecs[0].flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
        mpls_util_entity[0].fecs[1].flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
    }

    return rv;
}

/*
 * Select EVPN misc profiles
 */
int
evpn_profiles_set(
    int unit)
{
    /*
     * By default, FP in-lif profile is 2 bits. The value for these bits is managed by the application, in
     * consideration of other ACL application that may use these bits.
     * Need to select a value between 0-3. 0 should be reserved for non-ESI profiles.
     * Selected 1 arbitrarily.
     */
    evpn_global_info.in_lif_acl_profile_esi = 1;

    /*
     * PE1 has 1 peer dual homed to it (PE2). Need to assign 9bit profile to that peer
     * for ESI DB lookup on IML tunnels destined to to that peer.
     */
    evpn_global_info.dual_homed_peer_profile = 0x123;

    /*
     * Enable setup of split horizon parameters in this application
     */
    evpn_global_info.use_split_horizon = 1;

    /*
     * Setting EVPN core network group for split horizon filtering.
     */
    evpn_global_info.evpn_network_group_id = 1;

    return BCM_E_NONE;
}

/*
 * Look for a different ethernet port than the inputs
 */
int
evpn_extra_port_get(int unit, int *excluded_ports, int nof_excluded_ports, int *legal_port)
{
    int rv = BCM_E_NONE;
    int port_idx, excluded_port_idx;
    bcm_port_config_t port_config;

    bcm_port_config_t_init(&port_config);
    rv = bcm_port_config_get(unit, &port_config);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_extra_port_get\n");
        return rv;
    }

    BCM_PBMP_ITER(port_config.e, port_idx) {
        *legal_port = -1;
        for (excluded_port_idx = 0; excluded_port_idx < nof_excluded_ports; excluded_port_idx++)
        {
            if (port_idx == excluded_ports[excluded_port_idx])
            {
                *legal_port = -2;
                break;
            }
        }

        if (*legal_port == -2)
        {
            continue;
        }
        else
        {
            *legal_port = port_idx;
            break;
        }
    }

    if (*legal_port == -2)
    {
        printf("ERROR: in evpn_extra_port_get, No Available port found!\n");
        return BCM_E_NOT_FOUND;
    }
    else
    {
        printf("Found an extra port for testing ESI encap, port = %d.\n", *legal_port);
        return BCM_E_NONE;
    }
}

/*
 * Main function to configure the scenario described above and in cint_evpn.c
 * The function performs the following activities:
 * - Setup the VSI
 * - Create ETH segments - in this example - AC links
 * - Configure a L3 MPLS core connecting the device to neighboring MPLS routers
 * - Setup EVPN services for L2 MC and UC over the MPLS core
 * - Add static entries in MACT to emulate BGP triggered address advertising.
 */
int
evpn_main(
    int unit,
    int ce1_port,
    int ce3_port,
    int p1_port,
    int p2_port)
{
    int rv = BCM_E_NONE;
    int vswitch_ports[4];

    printf("~~ EVPN Basic Example ~~\n");

    /* If it's IOP mode, use egress MC due to multiple outlifs in MC entries */
    if ((soc_property_get(unit , "system_headers_mode",1) == 0)) {
        egress_mc = 1;
    }

    printf("Load application profiles and misc configs\n");
    rv = evpn_profiles_set(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_profiles_set\n");
        return rv;
    }

    printf("Create VPN (%d)\n", cint_evpn_info.vsi);
    rv = evpn_vswitch_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_mpls_vpn_create\n");
        return rv;
    }

    vswitch_ports[0] = ce1_port;
    vswitch_ports[1] = ce3_port;
    vswitch_ports[2] = p1_port;
    vswitch_ports[3] = p2_port;
    rv = evpn_vswitch_add_ports(unit, 4, vswitch_ports);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, evpn_vswitch_add_ports");
        return rv;
    }

    /** For testing only. Refer to the extra_port_id usage*/
    rv = evpn_extra_port_get(unit, vswitch_ports, 4, &extra_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, evpn_extra_port_get");
        return rv;
    }

    printf("Init ESI filter FP application\n");
    field_esi_filter__evpn_cw_enable = has_cw;
    field_esi_filter__evpn_fl_enable = has_fl;
    rv = evpn_esi_filter_init(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, evpn_esi_filter_init");
        return rv;
    }

    printf("Create ETH segments\n");
    rv = evpn_eth_segments_create(unit, ce1_port, ce3_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_eth_segment_create\n");
        return rv;
    }

    printf("Setup MPLS L3 network\n");
    rv = evpn_mpls_core_setup(unit, p1_port, p2_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_mpls_core_setup\n");
        return rv;
    }

    printf("Setup EVPN Services\n");
    rv = evpn_service_egress_config(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_egress_tunnels_config\n");
        return rv;
    }
    rv = evpn_service_ingress_config(unit, cint_evpn_info.vsi);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_ingress_tunnels_config\n");
        return rv;
    }

    printf("Connect ES to egress in UC flow\n");
    rv = evpn_egress_uc_l2_address_add(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_egress_uc_l2_address_add\n");
        return rv;
    }

    /* Configure ESI filter for IOP mode */
    if ((soc_property_get(unit , "system_headers_mode",1) == 0)) {
        /* Init forbidden port */
        field_evpn_info_init(unit, 0, ce1_port, p2_port, 0, ce1_port, ce3_port);

        /* Set IPMF to set forbidden port to learn_extension */
        rv = field_jr2_comp_evpn_esi_db(unit, jr2_ing_presel_id-1/*presel_id*/);
        if (BCM_E_NONE != rv) {
            printf("Error,unit: %d, in esi_db\n", unit);
            return rv;
        }

        /* Configure EPMF to drop when dst_port matches learn_extension */
        rv = field_jr2_comp_evpn_egress_filter_db(unit, jr2_egr_presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error,unit: %d,  in evpn_egress_filter_db\n", unit);
            return rv;
        }
    }

    return rv;
}

/*
 * Create L2 VPN (VSI and MC group)
 */
int
evpn_vswitch_create(
    int unit)
{
    int rv = BCM_E_NONE;

    printf("- Create VSI\n");
    rv = bcm_vswitch_create_with_id(unit, cint_evpn_info.vsi);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_dnx_vswitch_create_with_id\n");
        return rv;
    }

    printf("- Create MC group\n");
    rv = multicast__open_mc_group(unit, &cint_evpn_info.vsi, 0);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in multicast__open_mc_group\n");
        return rv;
    }

    return rv;
}

/*
 * Add ports to VPN
 */
int
evpn_vswitch_add_ports(
    int unit,
    int nof_ports,
    int *ports)
{
    int rv = BCM_E_NONE;

    while (nof_ports--)
    {
        rv = bcm_vlan_gport_add(unit, cint_evpn_info.vsi, ports[nof_ports], 0);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR, bcm_vlan_gport_add port=%d\n", ports[nof_ports]);
            return rv;
        }

        rv = bcm_vlan_gport_add(unit, cint_evpn_info.vsi + 1, ports[nof_ports], 0);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR, bcm_vlan_gport_add port=%d\n", ports[nof_ports]);
            return rv;
        }
    }

    return rv;
}

/*
 * Setup Field Processor based ESI filter.
 */
int
evpn_esi_filter_init(
    int unit)
{
    int rv = BCM_E_NONE;

    rv = cint_field_evpn_ingress(unit, evpn_global_info.in_lif_acl_profile_esi);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, cint_field_evpn_ingress\n");
        return rv;
    }

    rv = cint_field_evpn_egress(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, cint_field_evpn_egress\n");
        return rv;
    }

    return rv;
}

/*
 * Configure in/out port
 */
int
evpn_configure_port(
    int unit,
    int port)
{
    int rv = BCM_E_NONE;

    /*
     * set class
     */
    rv = bcm_port_class_set(unit, port, bcmPortClassId, port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_port_class_set, port=%d, \n", port);
        return rv;
    }

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: out_port_set, port=%d\n", port);
        return rv;
    }


    return rv;
}

/*
 * Information for ES creation
 */
struct evpn_es_info_s
{
    int port;                       /* Local port */
    int vid;                        /* VLAN tag VID*/
    int vsi;                        /* VSI */
    int label;                      /* ES identifier. May be set on an ESI label. Used for filtering */
    uint32 extra_flags;             /* Additional flags for bcm_vlan_port_create */
    bcm_vlan_port_t * vlan_port;    /* Pointer to allocated vlan_port object */
    int add_to_flooding;            /* Add to the global flooding domain */
    int flush_group;                /* Currently The MSB is used by Leaf indication. */
};

/*
 * Create AC and (optionally) add to flooding domain of the VSI.
 */
int
evpn_es_add(
    int unit,
    evpn_es_info_s * esi)
{
    int rv = BCM_E_NONE;
    bcm_field_entry_t esi_port_handle;
    bcm_field_entry_t esi_label_handle;
    bcm_vlan_port_t * vport = esi->vlan_port;

    rv = evpn_configure_port(unit, esi->port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_configure_port\n");
        return rv;
    }


    bcm_vlan_port_t_init(vport);
    vport->criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vport->port = esi->port;
    vport->match_vlan = esi->vid;
    vport->flags |= esi->extra_flags;
    vport->group = esi->flush_group; 
    rv = bcm_vlan_port_create(unit, vport);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_vlan_port_create\n");
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, esi->vsi, esi->port, 0);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_vlan_gport_add\n");
        return rv;
    }

    if (esi->vsi)
    {
        rv = bcm_vswitch_port_add(unit, esi->vsi, vport->vlan_port_id);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in bcm_vswitch_port_add\n");
            return rv;
        }
    }

    printf("Add ESI filter entry to FP ESI DB\n");
    rv = cint_field_evpn_egress_esi_port_entry_add(unit, dnx_evpn_field_info.egr_fg.context_id, dnx_evpn_field_info.egr_fg.fg_id,
                                                   &esi_port_handle, esi->label, esi->port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in cint_field_evpn_egress_esi_port_entry_add\n");
        return rv;
    }
    rv = cint_field_evpn_egress_esi_entry_add(unit, dnx_evpn_field_info.egr_fg.context_id, dnx_evpn_field_info.egr_fg.fg_id,
                                              &esi_label_handle, esi->label);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in cint_field_evpn_egress_esi_entry_add\n");
        return rv;
    }

    /* Add to flooding domain */
    if (esi->add_to_flooding)
    {
        bcm_gport_t gport;
        BCM_GPORT_MODPORT_SET(gport, unit, esi->port);
        rv = multicast__add_multicast_entry(unit, esi->vsi, &gport,
                                            &vport->encap_id, 1, egress_mc);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in multicast__add_multicast_entry\n");
            return rv;
        }
    }

    /* Set inLIF wide-data for Leaf indication if it's a Leaf site.*/
    if (is_evpn_etree && vport->group == 8)
    {
        uint64 wide_data;

        COMPILER_64_ZERO(wide_data);
        COMPILER_64_SET(wide_data, 0x0, 0x1);   /* Suppose the wide_data[0] is used for Leaf indication.*/

        rv = bcm_port_wide_data_set(unit, vport->vlan_port_id, BCM_PORT_WIDE_DATA_INGRESS, wide_data);
        if(rv != BCM_E_NONE) {
            printf("Error in bcm_port_wide_data_set\n");
            return rv;
        }

        printf("Set In-LIF profile to select E-Tree FP ingress application\n");
        rv = bcm_port_class_set(unit,vport->vlan_port_id, bcmPortClassFieldIngressVport, evpn_global_info.in_lif_acl_profile_esi);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in bcm_port_class_set inAC 0x%08X\n", vport->vlan_port_id);
            return rv;
        }
    }

    return rv;
}

/*
 * Create ETH segments connected to the PE using AC.
 * See cint_evpn.c for specific of ESI 200 and ESI 300.
 */
int
evpn_eth_segments_create(
    int unit,
    int ce1_port,
    int ce3_port)
{
    int rv = BCM_E_NONE;
    evpn_es_info_s esi_200;
    evpn_es_info_s esi_300;

    printf("- Create IML pass field entry in ESI DB\n");
    if (has_fl) {
        /** If flow lable exists, system_header.UDH.label may be IML, let it go...*/
        bcm_field_entry_t entry_handle_iml;
        rv = cint_field_evpn_egress_esi_entry_add(unit, dnx_evpn_field_info.egr_fg.context_id, dnx_evpn_field_info.egr_fg.fg_id,
                                                  &entry_handle_iml, cint_evpn_info.pe1_pe2_iml_label);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in cint_field_evpn_egress_esi_entry_add\n");
            return rv;
        }

        rv = cint_field_evpn_egress_esi_entry_add(unit, dnx_evpn_field_info.egr_fg.context_id, dnx_evpn_field_info.egr_fg.fg_id,
                                                  &entry_handle_iml, cint_evpn_info.pe1_pe3_iml_label);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in cint_field_evpn_egress_esi_entry_add\n");
            return rv;
        }
        rv = cint_field_evpn_egress_esi_entry_add(unit, dnx_evpn_field_info.egr_fg.context_id, dnx_evpn_field_info.egr_fg.fg_id,
                                                  &entry_handle_iml, cint_evpn_info.pe1_pe4_iml_label);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in cint_field_evpn_egress_esi_entry_add\n");
            return rv;
        }
    }

    printf("- Create CE1 ES (ESI 200)\n");
    esi_200.add_to_flooding = 1;
    esi_200.port = ce1_port;
    esi_200.vid = cint_evpn_info.ce1_vlan;
    esi_200.vsi = cint_evpn_info.vsi;
    esi_200.label = cint_evpn_info.pe1_pe2_ce1_esi_label;
    esi_200.vlan_port = &evpn_global_info.ce1_vlan_port;
    esi_200.flush_group = 0;
    rv = evpn_es_add(unit, &esi_200);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_esi_create ce1_vlan\n");
        return rv;
    }


    printf("- Create CE3 ES (ESI 300)\n");
    esi_300.add_to_flooding = 1;
    esi_300.port = ce3_port;
    esi_300.vid = cint_evpn_info.ce3_vlan;
    esi_300.vsi = cint_evpn_info.vsi;
    esi_300.label = cint_evpn_info.pe1_pe2_ce3_esi_label;
    esi_300.vlan_port = &evpn_global_info.ce3_vlan_port;
    esi_300.flush_group = is_evpn_etree ? evpn_etree_info.etree_flush_group : 0;
    rv = evpn_es_add(unit, &esi_300);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_esi_create ce1_vlan\n");
        return rv;
    }

    return rv;
}

/*
 * Create EVPN L3 network to route packets from PE1 to P1, P2
 * (see cint_evpn.c for details)
 */
int
evpn_mpls_core_setup(
    int unit,
    int p1_port,
    int p2_port)
{
    int rv = BCM_E_NONE;

    printf("Initializing MPLS utility.\n");
    rv = init_default_mpls_params(unit, p1_port, p2_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in init_default_mpls_params\n");
        return rv;
    }

    printf("Copy cint_evpn.c details to MPLS utility.\n");
    rv = init_evpn_mpls_util_params(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_mpls_utility_re_init\n");
        return rv;
    }

    printf("Run MPLS utility\n");
    rv = mpls_util_main(unit, 0/*ignored*/, 0/*ignored*/);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in mpls_util_main\n");
        return rv;
    }

    printf("Add vlan translation to the PE1->PE2 MPLS tunnel\n");
    rv = evpn_mpls_arp_plus_ac_configure(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_mpls_arp_plus_ac_configure\n");
        return rv;
    }

    return rv;
}

/*
 * Set vlan translation in ARP+AC entries, used for MPLS tunnels
 * with ESI DB lookup trigger.
 */
int
evpn_mpls_arp_plus_ac_configure(
    int unit)
{
    int rv = BCM_E_NONE;
    int mpls_entity;

    for (mpls_entity = 0; mpls_entity < 2; mpls_entity++)
    {
        bcm_if_t arp_if = mpls_util_entity[mpls_entity].arps[0].arp;
        bcm_gport_t arp_gport;
        BCM_GPORT_SUB_TYPE_L3_VLAN_TRANSLATION_SET(arp_gport, BCM_L3_ITF_VAL_GET(arp_if));
        BCM_GPORT_VLAN_PORT_ID_SET(arp_gport, arp_gport);
        rv = vlan_translate_ive_eve_translation_set(unit, arp_gport,
                                                    0x8100,                           0,
                                                    bcmVlanActionArpVlanTranslateAdd, bcmVlanActionNone,
                                                    cint_evpn_info.p2_vlan,           0,
                                                    3, 0, 0);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in vlan_translate_ive_eve_translation_set\n");
            return rv;
        }
    }

    return rv;
}

struct iml_encap_info_s
{
    bcm_mpls_label_t iml_label;         /* IML label value */
    int dual_homed_peer_profile;        /* Used to lookup ESI additional header */
    int add_to_flooding;                /* Add to the global flooding domain */
    bcm_gport_t lsp_tunnel_id;          /* If (add_to_flooding) this is used for mc copy encap */
    bcm_gport_t out_port;               /* If (add_to_flooding) this is used for mc copy out port */
};

/*
 * Add IML encapsulation with/out ESI label and adds it to the VSI
 * flooding group.
 *   [in]  iml_info - all the required information to set up the label (see iml_encap_info_s for field description).
 *   [out] plabel - allocated egress label object pointer. Will be filled by the function.
 */
int
iml_encap_add(
    int unit,
    iml_encap_info_s * iml_info,
    bcm_mpls_egress_label_t * plabel)
{
    int rv = BCM_E_NONE;
    bcm_gport_t gport;

    printf("- Create egress IML label\n");
    bcm_mpls_egress_label_t_init(plabel);
    plabel->label = iml_info->iml_label;
    plabel->flags = BCM_MPLS_EGRESS_LABEL_IML | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    plabel->flags |= (has_cw ? BCM_MPLS_EGRESS_LABEL_CONTROL_WORD : 0);
    plabel->flags |= (has_fl ? BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE : 0);
    plabel->egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    plabel->egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    plabel->encap_access = bcmEncapAccessTunnel1;
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, plabel);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }
    printf(" - IML encapsulation created, ID = 0x%08x\n", plabel->tunnel_id);
    /*
     * tunnel_id is generated as ITF in bcm_mpls_tunnel_initiator_create
     * convert to GPort for further processing.
     */
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, plabel->tunnel_id);

    if (iml_info->dual_homed_peer_profile)
    {
        printf("- Set dual homed peer profile on IML label lif\n");
        rv = bcm_port_class_set(unit, gport, bcmPortClassEgress, iml_info->dual_homed_peer_profile);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in bcm_port_class_set for dual homed peer id (%d) of lif (0x%08x)\n",
                   iml_info->dual_homed_peer_profile, lif_gport);
            return rv;
        }
    }

    if (iml_info->add_to_flooding)
    {
        if (iml_info->lsp_tunnel_id > 0)
        {
            /*
             * Add egress label and "FEC" to the flooding group.
             * "FEC" is simulated by adding PPMC entry that contain
             * both the new label and the MPLS encapsulation, and
             * the entry ID + out port is added to the MC group.
             */
            int mc_ext_id = 0;
            int cuds[2];
            printf("- Add IML and MPLS LSP to PPMC\n");
            cuds[0] = plabel->tunnel_id;
            cuds[1] = iml_info->lsp_tunnel_id;
            rv = bcm_multicast_encap_extension_create(unit, 0, &mc_ext_id, 2, cuds);
            if (rv != BCM_E_NONE)
            {
                printf("ERROR: bcm_multicast_encap_extension_create\n");
                return rv;
            }
            printf(" - Created PPMC entry with ID = 0x%08x\n", mc_ext_id);

            printf("- Add to PPMC pointer and port to MC group\n");
            rv = multicast__add_multicast_entry(unit, cint_evpn_info.vsi,
                                                &iml_info->out_port, &mc_ext_id, 1, egress_mc);
            if (rv != BCM_E_NONE)
            {
                printf("ERROR: multicast__add_multicast_entry\n");
                return rv;
            }
            printf(" - <port=%d, rep_idx=0x%08x> added to MC group %d\n",
                iml_info->out_port, mc_ext_id, cint_evpn_info.vsi);
        }
        else
        {
            /*
             * Add only the egress label to the flooding group. It is expected to point
             * to the next egress object in the encapsulation stack.
             */
            int encap_id = BCM_L3_ITF_VAL_GET(plabel->tunnel_id);
            printf("- Add to outlif and port to MC group\n");
            rv = multicast__add_multicast_entry(unit, cint_evpn_info.vsi,
                                                &iml_info->out_port, &encap_id, 1, egress_mc);
            if (rv != BCM_E_NONE)
            {
                printf("ERROR: multicast__add_multicast_entry\n");
                return rv;
            }
            printf(" - <port=%d, outlif=0x%08x> added to MC group %d\n",
                iml_info->out_port, plabel->tunnel_id, cint_evpn_info.vsi);
        }
    }

    if (evpn_global_info.use_split_horizon)
    {
        int nwk_grp_from_esem = *dnxc_data_get(unit, "esem", "feature", "etps_properties_assignment", NULL);

        printf("- Set network group for encapsulated IML\n");
        rv = bcm_port_class_set(unit, gport, bcmPortClassForwardEgress,
                                evpn_global_info.evpn_network_group_id);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in bcm_port_class_set iml egress label\n");
            return rv;
        }

        if (nwk_grp_from_esem)
        {
            bcm_port_match_info_t match_info;
            printf("- Connect IML to default native AC to receive network group ID for Split horizon\n");
            match_info.match = BCM_PORT_MATCH_PORT;
            match_info.flags |= BCM_PORT_MATCH_NATIVE | BCM_PORT_MATCH_EGRESS_ONLY;
            match_info.port = gport;
            rv = bcm_port_match_add(unit, evpn_global_info.default_native_ac.vlan_port_id, &match_info);
            if (rv != BCM_E_NONE)
            {
                printf("ERROR: bcm_port_match_add IML to default native AC\n");
                return rv;
            }
        }
    }

    return rv;
}

/*
 * Configure EVPN encapsulation for traffic from PE1 to other PEs
 */
int
evpn_service_egress_config(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_mpls_egress_label_t *plabel = &evpn_global_info.evpn_egress_label;
    iml_encap_info_s pe1_pe2_iml_info;
    iml_encap_info_s pe1_pe3_iml_info;
    iml_encap_info_s pe1_pe4_iml_info;
    bcm_mpls_esi_info_t pe1_pe2_esi_200_encap;
    bcm_mpls_esi_info_t pe1_pe2_esi_300_encap;
    bcm_mpls_esi_info_t pe1_pe4_esi_400_encap;
    bcm_gport_t gport;
    int nwk_grp_from_esem;
    int system_headers_mode;

    printf("Create EVPN encapsulation for UC traffic\n");
    bcm_mpls_egress_label_t_init(plabel);
    plabel->label = cint_evpn_info.evpn_label;
    plabel->flags = BCM_MPLS_EGRESS_LABEL_EVPN | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    plabel->flags |= (has_cw ? BCM_MPLS_EGRESS_LABEL_CONTROL_WORD : 0);
    plabel->flags |= (has_fl ? BCM_MPLS_EGRESS_LABEL_ENTROPY_ENABLE : 0);
    plabel->egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    plabel->egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    plabel->encap_access = bcmEncapAccessTunnel1;
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, plabel);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_tunnel_initiator_create evpn label\n");
        return rv;
    }

    nwk_grp_from_esem = *dnxc_data_get(unit, "esem", "feature", "etps_properties_assignment", NULL);
    if (evpn_global_info.use_split_horizon && nwk_grp_from_esem)
    {
        /*
         * Default egress native AC with network group for the IMLs (used for
         * setting up split horizon)
         */
        printf("Create egress default native AC with EVPN network group ID for split horizon\n");
        bcm_vlan_port_t_init(&evpn_global_info.default_native_ac);
        evpn_global_info.default_native_ac.criteria = BCM_VLAN_PORT_MATCH_NONE;
        evpn_global_info.default_native_ac.flags = BCM_VLAN_PORT_DEFAULT | BCM_VLAN_PORT_NATIVE
                | BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
        evpn_global_info.default_native_ac.egress_network_group_id = evpn_global_info.evpn_network_group_id;
        rv = bcm_vlan_port_create(unit, &evpn_global_info.default_native_ac);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: bcm_vlan_port_create default_native_ac\n");
            return rv;
        }
    }

    /** JR2: Create fabric MC for each core in case of egress Multicast. */
    system_headers_mode = soc_property_get(unit, "system_headers_mode", 1);
    if (egress_mc && (system_headers_mode == 0) && (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL)))
    {
        int nof_cores = *dnxc_data_get(unit, "device", "general", "nof_cores", NULL);
        bcm_module_t mreps[2];
        mreps[0] = 0;
        mreps[1] = 1; /* Ignore it in single core devices*/
        rv = bcm_fabric_multicast_set(unit, cint_evpn_info.vsi, 0, nof_cores, mreps);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_fabric_multicast_set \n");
            return rv;
        }
    }

    printf("Create IML to carry BUM from PE1 to PE2 - with ESI\n");
    pe1_pe2_iml_info.add_to_flooding = 1;
    pe1_pe2_iml_info.iml_label = cint_evpn_info.pe1_pe2_iml_label;
    pe1_pe2_iml_info.dual_homed_peer_profile = evpn_global_info.dual_homed_peer_profile;
    pe1_pe2_iml_info.lsp_tunnel_id = *(mpls_util_entity[1].fecs[0].tunnel_gport);
    pe1_pe2_iml_info.out_port = *(mpls_util_entity[1].fecs[0].port);
    rv = iml_encap_add(unit, &pe1_pe2_iml_info, &evpn_global_info.pe1_pe2_iml_egress_label);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in iml_encap_add pe1 to pe2\n");
        return rv;
    }

    printf("Create IML to carry BUM from PE1 to PE3 - without ESI\n");
    pe1_pe3_iml_info.add_to_flooding = 1;
    pe1_pe3_iml_info.iml_label = cint_evpn_info.pe1_pe3_iml_label;
    pe1_pe3_iml_info.dual_homed_peer_profile = 0; /* Peer is not dual homed to this PE */
    pe1_pe3_iml_info.lsp_tunnel_id = *(mpls_util_entity[0].fecs[0].tunnel_gport);
    pe1_pe3_iml_info.out_port = *(mpls_util_entity[0].fecs[0].port);
    rv = iml_encap_add(unit, &pe1_pe3_iml_info, &evpn_global_info.pe1_pe3_iml_egress_label);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in iml_encap_add pe1 to pe3\n");
        return rv;
    }

    printf("Create IML to carry BUM from PE1 to PE4 - without ESI\n");
    pe1_pe4_iml_info.add_to_flooding = 1;
    pe1_pe4_iml_info.iml_label = cint_evpn_info.pe1_pe4_iml_label;
    pe1_pe4_iml_info.dual_homed_peer_profile = evpn_global_info.dual_homed_peer_profile - 1; /* For testing, Peer is
                                                                                                dual homed to this PE */
    pe1_pe4_iml_info.lsp_tunnel_id = *(mpls_util_entity[0].fecs[1].tunnel_gport);
    pe1_pe4_iml_info.out_port = *(mpls_util_entity[0].fecs[1].port);
    rv = iml_encap_add(unit, &pe1_pe4_iml_info, &evpn_global_info.pe1_pe4_iml_egress_label);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in iml_encap_add pe1 to pe4\n");
        return rv;
    }

    printf("Add ESI encapsulation for IML traffic from ESI-200 to PE2\n");
    bcm_mpls_esi_info_t_init(&pe1_pe2_esi_200_encap);
    pe1_pe2_esi_200_encap.esi_label = cint_evpn_info.pe1_pe2_ce1_esi_label;
    pe1_pe2_esi_200_encap.out_class_id = evpn_global_info.dual_homed_peer_profile;
    pe1_pe2_esi_200_encap.src_port = evpn_global_info.ce1_vlan_port.port;
    rv = bcm_mpls_esi_encap_add(unit, &pe1_pe2_esi_200_encap);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_esi_encap_add esi-200\n");
        return rv;
    }

    printf("Add ESI encapsulation for IML traffic from ESI-300 to PE2\n");
    if (!is_evpn_etree)
    {
        /**We think CE3 as Leaf site in Etree, so no ESI encap is needed in that case.*/
        bcm_mpls_esi_info_t_init(&pe1_pe2_esi_300_encap);
        pe1_pe2_esi_300_encap.esi_label = cint_evpn_info.pe1_pe2_ce3_esi_label;
        pe1_pe2_esi_300_encap.out_class_id = evpn_global_info.dual_homed_peer_profile;
        pe1_pe2_esi_300_encap.src_port = evpn_global_info.ce3_vlan_port.port;
        rv = bcm_mpls_esi_encap_add(unit, &pe1_pe2_esi_300_encap);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in bcm_mpls_esi_encap_add esi-300\n");
            return rv;
        }
    }

    /*
     * For testing only - step out of the scenario described in cint_evpn.c by setting a dual-homed peer profile
     * to PE4, and adding an ESI encapsulation in case traffic arrives from port 13. This is meant to test traffic
     * to a peer with set dual-homed profile and ESI DB entry with this profile in the key, and check that the
     * ESI label isn't really added to the egressing traffic.
     */
    printf("Add ESI encapsulation for IML traffic from ESI-400 to PE4\n");
    bcm_mpls_esi_info_t_init(&pe1_pe4_esi_400_encap);
    pe1_pe4_esi_400_encap.esi_label = extra_esi_label;
    pe1_pe4_esi_400_encap.out_class_id = evpn_global_info.dual_homed_peer_profile - 1;
    pe1_pe4_esi_400_encap.src_port = extra_port_id;
    rv = bcm_mpls_esi_encap_add(unit, &pe1_pe4_esi_400_encap);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_esi_encap_add esi-400\n");
        return rv;
    }

    return rv;
}

/*
 * Configures an IML termination with or without expected VSI
 */
int
iml_term_add(
    int unit,
    int vsi,
    bcm_mpls_label_t iml_label,
    bcm_mpls_tunnel_switch_t *pswitch,
    int expect_esi)
{
    int rv = BCM_E_NONE;
    int feature_mpls_term_1_or_2_labels = *dnxc_data_get(unit, "mpls", "general", "mpls_term_1_or_2_labels", NULL);

    printf("- Create IML termination for MC traffic\n");
    
    bcm_mpls_tunnel_switch_t_init(pswitch);
    pswitch->action = BCM_MPLS_SWITCH_ACTION_POP;
    pswitch->flags = BCM_MPLS_SWITCH_EVPN_IML;
    pswitch->flags |= (has_fl ? BCM_MPLS_SWITCH_ENTROPY_ENABLE : 0);
    pswitch->flags2 |= (vsi ? 0 : BCM_MPLS_SWITCH2_CROSS_CONNECT);
    pswitch->flags2 |= (has_cw ? BCM_MPLS_SWITCH2_CONTROL_WORD : 0);
    if ((!expect_esi) && (!feature_mpls_term_1_or_2_labels))
    {
        pswitch->flags |= BCM_MPLS_SWITCH_EXPECT_BOS;
    }
    pswitch->label = iml_label;
    pswitch->vpn = vsi;
    rv = bcm_mpls_tunnel_switch_create(unit, pswitch);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_tunnel_switch_create iml label\n");
        return rv;
    }

    if (evpn_global_info.use_split_horizon)
    {
        printf("- Set network group for terminated IML\n");
        rv = bcm_port_class_set(unit, pswitch->tunnel_id, bcmPortClassForwardIngress,
                                evpn_global_info.evpn_network_group_id);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in bcm_port_class_set iml ingress label\n");
            return rv;
        }
    }

    /*
     * mpls_term_1_or_2_labels == 0: Set In-LIF profile for iPMF selection of ESI filter application
     * mpls_term_1_or_2_labels == 1: Not needed, since iPMF qualifies IML-range-Qual
     */
    if ((expect_esi) && (!feature_mpls_term_1_or_2_labels))
    {
        printf("Set In-LIF profile to select ESI FP ingress application\n");
        rv = bcm_port_class_set(unit, pswitch->tunnel_id, bcmPortClassFieldIngressVport,
                                evpn_global_info.in_lif_acl_profile_esi);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in bcm_port_class_set iml label\n");
            return rv;
        }
    }

    printf(" - IML termination %s ESI - created. ID = 0x%08x\n",
        (expect_esi ? "with" : "without"), pswitch->tunnel_id);
    return rv;
}

/*
 * Configure EVPN termination for traffic arriving from other PEs.
 * if vsi is 0, the LIFs would be created as P2P.
 */
int
evpn_service_ingress_config(
    int unit,
    int vsi)
{
    int rv = BCM_E_NONE;
    bcm_mpls_tunnel_switch_t *tunnel = &evpn_global_info.evpn_in_tunnel;
    bcm_mpls_range_action_t action;
    int feature_mpls_term_1_or_2_labels = *dnxc_data_get(unit, "mpls", "general", "mpls_term_1_or_2_labels", NULL); 
    char *proc_name;

    proc_name = "evpn_service_ingress_config";
    printf("%s(): Create EVPN termination for UC traffic\n",proc_name);
    bcm_mpls_tunnel_switch_t_init(tunnel);
    tunnel->action = BCM_MPLS_SWITCH_ACTION_POP;
    tunnel->flags = BCM_MPLS_SWITCH_NEXT_HEADER_L2;
    tunnel->flags |= (has_fl ? BCM_MPLS_SWITCH_ENTROPY_ENABLE : 0);
    tunnel->flags2 |= (vsi ? 0 : BCM_MPLS_SWITCH2_CROSS_CONNECT);
    tunnel->flags2 |= (has_cw ? BCM_MPLS_SWITCH2_CONTROL_WORD : 0);
    tunnel->label = cint_evpn_info.evpn_label;
    tunnel->vpn = vsi;
    rv = bcm_mpls_tunnel_switch_create(unit, tunnel);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): ERROR: in bcm_mpls_tunnel_switch_create evpn tunnel\n",proc_name);
        return rv;
    }
    printf("%s(): EVPN termination - created. ID = 0x%08x\n", proc_name, tunnel->tunnel_id);

    printf("%s(): Set IML range\n", proc_name);
    action.flags = BCM_MPLS_RANGE_ACTION_EVPN_IML;
    rv = bcm_mpls_range_action_add(unit,cint_evpn_info.iml_range_low_label,
                                   cint_evpn_info.iml_range_high_label, &action);
    if (rv != BCM_E_NONE) {
        printf("%s(): ERROR: in bcm_mpls_range_action_add\n", proc_name);
        return rv;
    }
    printf("%s(): IML range set to MIN = %d(0x%05x), MAX = %d(0x%05x)\n",
        proc_name, cint_evpn_info.iml_range_low_label, cint_evpn_info.iml_range_low_label,
        cint_evpn_info.iml_range_high_label, cint_evpn_info.iml_range_high_label);

    if (!has_fl) {
        /** If FL exists, only one entry with BOS0 is needed.*/
        printf("%s(): Add IML termination without ESI\n", proc_name);
        rv = iml_term_add(unit, vsi, cint_evpn_info.pe1_pe2_iml_label,
                          &evpn_global_info.pe1_pe2_iml_tunnel_switch_no_esi,
                          0 /** No ESI **/);
        if (rv != BCM_E_NONE) {
            printf("%s(): ERROR: in iml_term_add no ESI\n", proc_name);
            return rv;
        }
    }

    if (!feature_mpls_term_1_or_2_labels) {
        /**If the feature term 1 or 2 mpls labels is available, no need to add the 2nd IML term entry.*/
        printf("%s(): Add IML termination with ESI\n", proc_name);
        rv = iml_term_add(unit, vsi, cint_evpn_info.pe1_pe2_iml_label,
                          &evpn_global_info.pe1_pe2_iml_tunnel_switch_esi,
                          1 /** Expect ESI **/);
        if (rv != BCM_E_NONE) {
            printf("%s(): ERROR: in iml_term_add expect ESI\n", proc_name);
            return rv;
        }
    }

    return rv;
}

/*
 * Emulate BGP learning - static add MACT entries to forward
 * packets arriving from different ethernet segments.
 */
int
evpn_egress_uc_l2_address_add(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2_addr;
    bcm_mpls_egress_label_t *plabel = &evpn_global_info.evpn_egress_label;
    uint32 evpn_encap_id;
    bcm_gport_t fec_gport;

    evpn_encap_id = BCM_L3_ITF_VAL_GET(plabel->tunnel_id);
    BCM_GPORT_FORWARD_PORT_SET(fec_gport, mpls_util_entity[0].fecs[0].fec_id);

    printf("Add CE2 L2 Address to MACT\n");
    bcm_l2_addr_t_init(&l2_addr, cint_evpn_info.ce2_mac, cint_evpn_info.vsi);
    l2_addr.flags |= BCM_L2_STATIC;
    /** The EVPN encapsulation */
    l2_addr.encap_id = evpn_encap_id;
    /** FEC for LSP + port */
    l2_addr.port = fec_gport;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_l2_addr_add\n");
        return rv;
    }

    if (is_evpn_etree)
    {
        bcm_l2_addr_t_init(&l2_addr, evpn_etree_info.ce4_mac, cint_evpn_info.vsi);
        l2_addr.group = evpn_etree_info.etree_flush_group;
        l2_addr.encap_id = evpn_encap_id;
        l2_addr.port = fec_gport;
        rv = bcm_l2_addr_add(unit, &l2_addr);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in bcm_l2_addr_add\n");
            return rv;
        }
    }

    return rv;
}



/* *********************************************************************
 *                  P2P Example
 */

/*
 * Main function to configure a P2P scenario:
 * CE1->EVPN
 * EVPN->CE3
 * IML->MC-Group
 * The function performs the following activities:
 * - Create ETH segments - in this example - P2P AC links
 * - Configure a L3 MPLS core connecting the device to neighboring MPLS routers
 * - Setup P2P EVPN services for L2 UC over the MPLS core
 * - Cross-connect AC1 to EVPN1 and EVPN2 to AC2
 * - Create a MC group with the two ACs and connect IML termination to it.
 */
int
evpn_main_p2p(
    int unit,
    int ce1_port,
    int ce3_port,
    int p1_port,
    int p2_port)
{
    int rv = BCM_E_NONE;
    int vswitch_ports[4];

    printf("~~ EVPN P2P Basic Example ~~\n");

    printf("Load application profiles and misc configs\n");
    rv = evpn_profiles_set(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_profiles_set\n");
        return rv;
    }

    vswitch_ports[0] = ce1_port;
    vswitch_ports[1] = ce3_port;
    vswitch_ports[2] = p1_port;
    vswitch_ports[3] = p2_port;
    rv = evpn_vswitch_add_ports(unit, 4, vswitch_ports);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, evpn_vswitch_add_ports");
        return rv;
    }

    printf("Init ESI filter FP application\n");
    rv = evpn_esi_filter_init(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, evpn_esi_filter_init");
        return rv;
    }

    printf("Create ETH segments\n");
    rv = evpn_p2p_eth_segments_create(unit, vswitch_ports[0], vswitch_ports[1]);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_eth_segment_create\n");
        return rv;
    }

    printf("Setup MPLS L3 network\n");
    rv = evpn_mpls_core_setup(unit, vswitch_ports[2], vswitch_ports[3]);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_mpls_core_setup\n");
        return rv;
    }

    printf("Setup EVPN Services\n");
    rv = evpn_p2p_service_egress_config(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_egress_tunnels_config\n");
        return rv;
    }
    rv = evpn_service_ingress_config(unit, 0/** vsi */);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_ingress_tunnels_config\n");
        return rv;
    }

    printf("Cross connect CEs to UC EVPN services");
    rv = evpn_p2p_cross_connect_uc(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_p2p_cross_connect_uc\n");
        return rv;
    }

    printf("Cross connect IML to CEs MC group\n");
    rv = evpn_p2p_cross_connect_mc(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_p2p_cross_connect_uc\n");
        return rv;
    }

    /* Configure ESI filter for IOP mode */
    if ((soc_property_get(unit , "system_headers_mode",1) == 0)) {
        /* Init forbidden port */
        field_evpn_info_init(unit, 0, ce1_port, p2_port, 0, ce1_port, ce3_port);

        /* Set IPMF to set forbidden port to learn_extension */
        rv = field_jr2_comp_evpn_esi_db(unit, jr2_ing_presel_id-1/*presel_id*/);
        if (BCM_E_NONE != rv) {
            printf("Error,unit: %d, in esi_db\n", unit);
            return rv;
        }

        /* Configure EPMF to drop when dst_port matches learn_extension */
        rv = field_jr2_comp_evpn_egress_filter_db(unit, jr2_egr_presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error,unit: %d,  in evpn_egress_filter_db\n", unit);
            return rv;
        }
    }

    return rv;
}

/*
 * Create VLAN Ports for CE1 and CE3, Both with P2P service type.
 */
int
evpn_p2p_eth_segments_create(
    int unit,
    int ce1_port,
    int ce3_port)
{
    int rv = BCM_E_NONE;
    evpn_es_info_s esi_200;
    evpn_es_info_s esi_300;

    printf("- Create CE1 ES (ESI 200)\n");
    esi_200.port = ce1_port;
    esi_200.vid = cint_evpn_info.ce1_vlan;
    esi_200.label = cint_evpn_info.pe1_pe2_ce1_esi_label;
    esi_200.vlan_port = &evpn_global_info.ce1_vlan_port;
    esi_200.extra_flags = 0;
    rv = evpn_es_add(unit, &esi_200);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_esi_create ce1_vlan\n");
        return rv;
    }


    printf("- Create CE3 ES (ESI 300)\n");
    esi_300.port = ce3_port;
    esi_300.vid = cint_evpn_info.ce3_vlan;
    esi_300.label = cint_evpn_info.pe1_pe2_ce3_esi_label;
    esi_300.vlan_port = &evpn_global_info.ce3_vlan_port;
    esi_300.extra_flags = 0;
    rv = evpn_es_add(unit, &esi_300);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_esi_create ce1_vlan\n");
        return rv;
    }

    return rv;
}

/*
 * Configure EVPN encapsulation for P2P traffic
 */
int
evpn_p2p_service_egress_config(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_mpls_egress_label_t *plabel = &evpn_global_info.evpn_egress_label;

    printf("Create EVPN encapsulation for UC traffic\n");
    bcm_mpls_egress_label_t_init(plabel);
    plabel->label = cint_evpn_info.evpn_label;
    plabel->flags = BCM_MPLS_EGRESS_LABEL_EVPN | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    plabel->egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    plabel->egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    plabel->encap_access = bcmEncapAccessTunnel1;
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, plabel);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_tunnel_initiator_create evpn label\n");
        return rv;
    }

    return rv;
}

/*
 * Cross connect CE1's VLAN port to Egress EVPN label and MPLS LSP towards P1.
 * Cross connect EVPN termination to CE3's VLAN port.
 */
int
evpn_p2p_cross_connect_uc(
    int unit)
{
    int rv = BCM_E_NONE;

    bcm_vswitch_cross_connect_t gports;
    bcm_vswitch_cross_connect_t_init(&gports);

    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.encap1 = BCM_FORWARD_ENCAP_ID_INVALID;

    printf("Cross Connecting AC to EVPN\n");
    gports.port1 = evpn_global_info.ce1_vlan_port.vlan_port_id;
    gports.encap2 = BCM_L3_ITF_VAL_GET(evpn_global_info.evpn_egress_label.tunnel_id);
    BCM_GPORT_FORWARD_PORT_SET(gports.port2, mpls_util_entity[0].fecs[0].fec_id);
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_vswitch_cross_connect_add AC(0x%08X) to EVPN(0x%08X)\n",
               evpn_global_info.ce1_vlan_port.vlan_port_id, mpls_util_entity[0].fecs[0].fec_id);
        return rv;
    }

    printf("Cross Connecting EVPN to AC\n");
    gports.port1 = evpn_global_info.evpn_in_tunnel.tunnel_id;
    gports.encap2 = BCM_FORWARD_ENCAP_ID_INVALID;
    gports.port2 = evpn_global_info.ce3_vlan_port.vlan_port_id;
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_vswitch_cross_connect_add EVPN(0x%08X) to AC(0x%08X)\n",
               evpn_global_info.evpn_in_tunnel.tunnel_id, evpn_global_info.ce3_vlan_port.vlan_port_id);
        return rv;
    }

    return rv;
}

/*
 * Create a MC Group with both CE's VLAN ports and cross-connect the IML terminations to it.
 */
int
evpn_p2p_cross_connect_mc(
    int unit)
{
    int mc_id = 1000;
    bcm_gport_t ce_ports[2];
    int ce_encaps[2];
    bcm_vswitch_cross_connect_t gports;
    int rv = BCM_E_NONE;
    int system_headers_mode;
    int feature_mpls_term_1_or_2_labels = *dnxc_data_get(unit, "mpls", "general", "mpls_term_1_or_2_labels", NULL);

    printf("- Create MC group\n");
    rv = multicast__open_mc_group(unit, &mc_id, egress_mc);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in multicast__open_mc_group\n");
        return rv;
    }

    /** JR2: Create fabric MC for each core in case of egress Multicast. */
    system_headers_mode = soc_property_get(unit, "system_headers_mode", 1);
    if (egress_mc && (system_headers_mode == 0) && (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL)))
    {
        int nof_cores = *dnxc_data_get(unit, "device", "general", "nof_cores", NULL);
        bcm_module_t mreps[2];
        mreps[0] = 0;
        mreps[1] = 1; /* Ignore it in single core devices*/
        rv = bcm_fabric_multicast_set(unit, mc_id, 0, nof_cores, mreps);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_fabric_multicast_set \n");
            return rv;
        }
    }

    /**  CE1 */
    BCM_GPORT_MODPORT_SET(ce_ports[0], unit, evpn_global_info.ce1_vlan_port.port);
    ce_encaps[0] = evpn_global_info.ce1_vlan_port.encap_id;

    /** CE3 */
    BCM_GPORT_MODPORT_SET(ce_ports[1], unit, evpn_global_info.ce3_vlan_port.port);
    ce_encaps[1] = evpn_global_info.ce3_vlan_port.encap_id;

    printf("- Add CEs to MC grup\n");
    rv = multicast__add_multicast_entry(unit, mc_id, ce_ports, ce_encaps, 2, egress_mc);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in multicast__add_multicast_entry\n");
        return rv;
    }

    bcm_vswitch_cross_connect_t_init(&gports);
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    gports.encap1 = BCM_FORWARD_ENCAP_ID_INVALID;
    gports.encap2 = BCM_FORWARD_ENCAP_ID_INVALID;

    printf("- Cross connect IML termination to MC group\n");
    gports.port1 = evpn_global_info.pe1_pe2_iml_tunnel_switch_no_esi.tunnel_id;
    BCM_GPORT_MCAST_SET(gports.port2, mc_id);
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: bcm_vswitch_cross_connect_add AC to EVPN\n");
        return rv;
    }

    if (!feature_mpls_term_1_or_2_labels) {
        /** If the feature is supported, this tunnel LIF wouldn't exist since one IML termination LIF is enough.*/
        printf("- Cross connect IML+ESI termination to MC group\n");
        gports.port1 = evpn_global_info.pe1_pe2_iml_tunnel_switch_esi.tunnel_id;
        BCM_GPORT_MCAST_SET(gports.port2, mc_id);
        rv = bcm_vswitch_cross_connect_add(unit, &gports);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: bcm_vswitch_cross_connect_add AC to EVPN\n");
            return rv;
        }
    }

    return rv;
}


/* *******************************************************************************
 *                  "PHP" example
 */

/*
 * This variant shows a configuration similar to the basic, except that there is an ETH connection
 * between PE1 and PE2. The packets destined to PE2 should get EVPN/IML/IML+ESI encapsulation and
 * then LL (without LSP). This is implemented, in this example, by pointing from the EVPN/IML outlif
 * to the ARP entry, and forwarding packets to Outlif+Port, instead of Outlif+FEC.
 */
int
evpn_main_php(
    int unit,
    int ce1_port,
    int ce3_port,
    int p1_port,
    int p2_port)
{
    int rv = BCM_E_NONE;
    int vswitch_ports[4];

    printf("~~ EVPN Basic Example ~~\n");

    printf("Load application profiles and misc configs\n");
    rv = evpn_profiles_set(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_profiles_set\n");
        return rv;
    }

    printf("Create VPN (%d)\n", cint_evpn_info.vsi);
    rv = evpn_vswitch_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_mpls_vpn_create\n");
        return rv;
    }

    vswitch_ports[0] = ce1_port;
    vswitch_ports[1] = ce3_port;
    vswitch_ports[2] = p1_port;
    vswitch_ports[3] = p2_port;
    rv = evpn_vswitch_add_ports(unit, 4, vswitch_ports);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, evpn_vswitch_add_ports");
        return rv;
    }

    printf("Init ESI filter FP application\n");
    rv = evpn_esi_filter_init(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, evpn_esi_filter_init");
        return rv;
    }

    printf("Create ETH segments\n");
    rv = evpn_eth_segments_create(unit, ce1_port, ce3_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_eth_segment_create\n");
        return rv;
    }

    printf("Setup MPLS L3 network\n");
    rv = evpn_mpls_core_setup(unit, p1_port, p2_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_mpls_core_setup\n");
        return rv;
    }

    printf("Setup EVPN Services\n");
    rv = evpn_php_service_egress_config(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_egress_tunnels_config\n");
        return rv;
    }
    rv = evpn_service_ingress_config(unit, cint_evpn_info.vsi);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_ingress_tunnels_config\n");
        return rv;
    }

    printf("Connect ES to egress in UC flow\n");
    rv = evpn_php_egress_uc_l2_address_add(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_egress_uc_l2_address_add\n");
        return rv;
    }

    /* Configure ESI filter for IOP mode */
    if ((soc_property_get(unit , "system_headers_mode",1) == 0)) {
        /* Init forbidden port */
        field_evpn_info_init(unit, 0, ce1_port, p2_port, 0, ce1_port, ce3_port);

        /* Set IPMF to set forbidden port to learn_extension */
        rv = field_jr2_comp_evpn_esi_db(unit, jr2_ing_presel_id-1/*presel_id*/);
        if (BCM_E_NONE != rv) {
            printf("Error,unit: %d, in esi_db\n", unit);
            return rv;
        }

        /* Configure EPMF to drop when dst_port matches learn_extension */
        rv = field_jr2_comp_evpn_egress_filter_db(unit, jr2_egr_presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error,unit: %d,  in evpn_egress_filter_db\n", unit);
            return rv;
        }
    }

    return rv;
}

/*
 * Configure EVPN encapsulation for traffic from PE1 to other PEs
 */
int
evpn_php_service_egress_config(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_mpls_egress_label_t *plabel = &evpn_global_info.evpn_egress_label;
    iml_encap_info_s pe1_pe2_iml_info;
    iml_encap_info_s pe1_pe3_iml_info;
    iml_encap_info_s pe1_pe4_iml_info;
    bcm_mpls_esi_info_t pe1_pe2_esi_200_encap;
    bcm_mpls_esi_info_t pe1_pe2_esi_300_encap;
    bcm_mpls_esi_info_t pe1_pe4_esi_400_encap;
    bcm_gport_t gport;
    int nwk_grp_from_esem;
    int system_headers_mode;

    printf("Create EVPN encapsulation for UC traffic\n");
    bcm_mpls_egress_label_t_init(plabel);
    plabel->label = cint_evpn_info.evpn_label;
    plabel->flags = BCM_MPLS_EGRESS_LABEL_EVPN | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    plabel->egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    plabel->egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    plabel->encap_access = bcmEncapAccessTunnel1;
    plabel->l3_intf_id = mpls_util_entity[1].arps[0].arp;
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, plabel);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_tunnel_initiator_create evpn label\n");
        return rv;
    }

    nwk_grp_from_esem = *dnxc_data_get(unit, "esem", "feature", "etps_properties_assignment", NULL);
    if (evpn_global_info.use_split_horizon && nwk_grp_from_esem)
    {
        /*
         * Default egress native AC with network group for the IMLs (used for
         * setting up split horizon)
         */
        printf("Create egress default native AC with EVPN network group ID for split horizon\n");
        bcm_vlan_port_t_init(&evpn_global_info.default_native_ac);
        evpn_global_info.default_native_ac.criteria = BCM_VLAN_PORT_MATCH_NONE;
        evpn_global_info.default_native_ac.flags = BCM_VLAN_PORT_DEFAULT | BCM_VLAN_PORT_NATIVE
                | BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
        evpn_global_info.default_native_ac.egress_network_group_id = evpn_global_info.evpn_network_group_id;
        rv = bcm_vlan_port_create(unit, &evpn_global_info.default_native_ac);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: bcm_vlan_port_create default_native_ac\n");
            return rv;
        }
    }

    /** JR2: Create fabric MC for each core in case of egress Multicast. */
    system_headers_mode = soc_property_get(unit, "system_headers_mode", 1);
    if (egress_mc && (system_headers_mode == 0) && (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL)))
    {
        int nof_cores = *dnxc_data_get(unit, "device", "general", "nof_cores", NULL);
        bcm_module_t mreps[2];
        mreps[0] = 0;
        mreps[1] = 1; /* Ignore it in single core devices*/
        rv = bcm_fabric_multicast_set(unit, cint_evpn_info.vsi, 0, nof_cores, mreps);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_fabric_multicast_set \n");
            return rv;
        }
    }

    printf("Create IML to carry BUM from PE1 to PE2 - with ESI\n");
    pe1_pe2_iml_info.add_to_flooding = 1;
    pe1_pe2_iml_info.iml_label = cint_evpn_info.pe1_pe2_iml_label;
    pe1_pe2_iml_info.dual_homed_peer_profile = evpn_global_info.dual_homed_peer_profile;
    pe1_pe2_iml_info.out_port = *(mpls_util_entity[1].fecs[0].port);
    rv = iml_encap_add(unit, &pe1_pe2_iml_info, &evpn_global_info.pe1_pe2_iml_egress_label);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in iml_encap_add pe1 to pe2\n");
        return rv;
    }
    rv = evpn_php_egress_next_encap_add(unit, &evpn_global_info.pe1_pe2_iml_egress_label,
                                        mpls_util_entity[1].arps[0].arp);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_php_egress_next_encap_add pe1 to pe2\n");
        return rv;
    }

    printf("Create IML to carry BUM from PE1 to PE3 - without ESI\n");
    pe1_pe3_iml_info.add_to_flooding = 1;
    pe1_pe3_iml_info.iml_label = cint_evpn_info.pe1_pe3_iml_label;
    pe1_pe3_iml_info.dual_homed_peer_profile = 0; /* Peer is not dual homed to this PE */
    pe1_pe3_iml_info.out_port = *(mpls_util_entity[0].fecs[0].port);
    rv = iml_encap_add(unit, &pe1_pe3_iml_info, &evpn_global_info.pe1_pe3_iml_egress_label);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in iml_encap_add pe1 to pe3\n");
        return rv;
    }
    rv = evpn_php_egress_next_encap_add(unit, &evpn_global_info.pe1_pe3_iml_egress_label,
                                        mpls_util_entity[0].mpls_encap_tunnel[0].tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_php_egress_next_encap_add pe1 to pe3\n");
        return rv;
    }

    printf("Create IML to carry BUM from PE1 to PE4 - without ESI\n");
    pe1_pe4_iml_info.add_to_flooding = 1;
    pe1_pe4_iml_info.iml_label = cint_evpn_info.pe1_pe4_iml_label;
    pe1_pe4_iml_info.dual_homed_peer_profile = evpn_global_info.dual_homed_peer_profile - 1; /* For testing, Peer is
                                                                                                dual homed to this PE */
    pe1_pe4_iml_info.out_port = *(mpls_util_entity[0].fecs[1].port);
    rv = iml_encap_add(unit, &pe1_pe4_iml_info, &evpn_global_info.pe1_pe4_iml_egress_label);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in iml_encap_add pe1 to pe4\n");
        return rv;
    }
    rv = evpn_php_egress_next_encap_add(unit, &evpn_global_info.pe1_pe4_iml_egress_label,
                                        mpls_util_entity[0].mpls_encap_tunnel[1].tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_php_egress_next_encap_add pe1 to pe4\n");
        return rv;
    }

    printf("Add ESI encapsulation for IML traffic from ESI-200 to PE2\n");
    bcm_mpls_esi_info_t_init(&pe1_pe2_esi_200_encap);
    pe1_pe2_esi_200_encap.esi_label = cint_evpn_info.pe1_pe2_ce1_esi_label;
    pe1_pe2_esi_200_encap.out_class_id = evpn_global_info.dual_homed_peer_profile;
    pe1_pe2_esi_200_encap.src_port = evpn_global_info.ce1_vlan_port.port;
    rv = bcm_mpls_esi_encap_add(unit, &pe1_pe2_esi_200_encap);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_esi_encap_add esi-200\n");
        return rv;
    }

    printf("Add ESI encapsulation for IML traffic from ESI-300 to PE2\n");
    bcm_mpls_esi_info_t_init(&pe1_pe2_esi_300_encap);
    pe1_pe2_esi_300_encap.esi_label = cint_evpn_info.pe1_pe2_ce3_esi_label;
    pe1_pe2_esi_300_encap.out_class_id = evpn_global_info.dual_homed_peer_profile;
    pe1_pe2_esi_300_encap.src_port = evpn_global_info.ce3_vlan_port.port;
    rv = bcm_mpls_esi_encap_add(unit, &pe1_pe2_esi_300_encap);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_esi_encap_add esi-300\n");
        return rv;
    }


    return rv;
}

int
evpn_php_egress_next_encap_add(
    int unit,
    bcm_mpls_egress_label_t * eg_label,
    bcm_if_t egress_if)
{
    int rv = BCM_E_NONE;
    bcm_gport_t gport;

    eg_label->flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_REPLACE;
    eg_label->l3_intf_id = egress_if;
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, eg_label);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    /** Set the network_group_id again since the replace operation will clear it*/
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, eg_label->tunnel_id);
    if (evpn_global_info.use_split_horizon)
    {
        rv = bcm_port_class_set(unit, gport, bcmPortClassForwardEgress,
                                evpn_global_info.evpn_network_group_id);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in bcm_port_class_set iml egress label\n");
            return rv;
        }
    }

    return rv;
}

/*
 * Setup UC Fwd for CE5 mac (Not in cint_evpn.c) to PE2, through outlif+port entry in the MACT.
 */
int
evpn_php_egress_uc_l2_address_add(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2_addr;
    bcm_mpls_egress_label_t *plabel = &evpn_global_info.evpn_egress_label;
    uint32 evpn_encap_id;
    int out_port;
    bcm_mac_t ce5_mac = {0x00, 0x00, 0x00, 0x00, 0x01, 0x20};

    evpn_encap_id = BCM_L3_ITF_VAL_GET(plabel->tunnel_id);
    out_port = mpls_util_entity[1].ports[0].port;

    printf("Add CE5 L2 Address to MACT\n");
    bcm_l2_addr_t_init(&l2_addr, ce5_mac, cint_evpn_info.vsi);
    l2_addr.flags |= BCM_L2_STATIC;
    /** The EVPN encapsulation */
    l2_addr.encap_id = evpn_encap_id;
    /** Local out port */
    l2_addr.port = out_port;
    rv = bcm_l2_addr_add(unit, &l2_addr);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_l2_addr_add\n");
        return rv;
    }

    return rv;
}


/* *******************************************************************************
 *                  Control Word example
 */

/*
 * This variant shows a configuration with control word termination and encapsulation.
 */
int
evpn_cw_main(
    int unit,
    int ce1_port,
    int ce3_port,
    int p1_port,
    int p2_port)
{
    int rv = BCM_E_NONE;
    int vswitch_ports[4];

    printf("~~ EVPN Basic Example ~~\n");

    /* If it's IOP mode, use egress MC due to multiple outlifs in MC entries */
    if ((soc_property_get(unit , "system_headers_mode",1) == 0)) {
        egress_mc = 1;
    }

    printf("Load application profiles and misc configs\n");
    rv = evpn_profiles_set(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_profiles_set\n");
        return rv;
    }

    printf("Create VPN (%d)\n", cint_evpn_info.vsi);
    rv = evpn_vswitch_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_mpls_vpn_create\n");
        return rv;
    }

    vswitch_ports[0] = ce1_port;
    vswitch_ports[1] = ce3_port;
    vswitch_ports[2] = p1_port;
    vswitch_ports[3] = p2_port;
    rv = evpn_vswitch_add_ports(unit, 4, vswitch_ports);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, evpn_vswitch_add_ports");
        return rv;
    }

    /** For testing only. Refer to the extra_port_id usage*/
    rv = evpn_extra_port_get(unit, vswitch_ports, 4, &extra_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, evpn_extra_port_get");
        return rv;
    }

    printf("Init ESI filter FP application\n");
    field_esi_filter__evpn_cw_enable = 1;
    rv = evpn_esi_filter_init(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, evpn_esi_filter_init");
        return rv;
    }

    printf("Create ETH segments\n");
    rv = evpn_eth_segments_create(unit, ce1_port, ce3_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_eth_segment_create\n");
        return rv;
    }

    printf("Setup MPLS L3 network\n");
    rv = evpn_mpls_core_setup(unit, p1_port, p2_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_mpls_core_setup\n");
        return rv;
    }

    printf("Setup EVPN Services\n");
    rv = evpn_cw_service_egress_config(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_egress_tunnels_config\n");
        return rv;
    }
    rv = evpn_cw_service_ingress_config(unit, cint_evpn_info.vsi);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_ingress_tunnels_config\n");
        return rv;
    }

    printf("Connect ES to egress in UC flow\n");
    rv = evpn_egress_uc_l2_address_add(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_egress_uc_l2_address_add\n");
        return rv;
    }

    /* Configure ESI filter for IOP mode */
    if ((soc_property_get(unit , "system_headers_mode",1) == 0)) {
        has_cw = 1;
        /* Init forbidden port */
        field_evpn_info_init(unit, 0, ce1_port, p2_port, 0, ce1_port, ce3_port);

        /* Set IPMF to set forbidden port to learn_extension */
        rv = field_jr2_comp_evpn_esi_db(unit, jr2_ing_presel_id-1/*presel_id*/);
        if (BCM_E_NONE != rv) {
            printf("Error,unit: %d, in esi_db\n", unit);
            return rv;
        }

        /* Configure EPMF to drop when dst_port matches learn_extension */
        rv = field_jr2_comp_evpn_egress_filter_db(unit, jr2_egr_presel_id);
        if (BCM_E_NONE != rv) {
            printf("Error,unit: %d,  in evpn_egress_filter_db\n", unit);
            return rv;
        }
    }

    return rv;
}

/*
 * Configure EVPN encapsulation for traffic from PE1 to other PEs
 */
int
evpn_cw_service_egress_config(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_mpls_egress_label_t *plabel = &evpn_global_info.evpn_egress_label;
    iml_encap_info_s pe1_pe2_iml_info;
    iml_encap_info_s pe1_pe3_iml_info;
    iml_encap_info_s pe1_pe4_iml_info;
    bcm_mpls_esi_info_t pe1_pe2_esi_200_encap;
    bcm_mpls_esi_info_t pe1_pe2_esi_300_encap;
    bcm_mpls_esi_info_t pe1_pe4_esi_400_encap;
    bcm_gport_t gport;
    int nwk_grp_from_esem;
    int system_headers_mode;

    printf("Create EVPN encapsulation for UC traffic\n");
    bcm_mpls_egress_label_t_init(plabel);
    plabel->label = cint_evpn_info.evpn_label;
    plabel->flags = BCM_MPLS_EGRESS_LABEL_EVPN | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT
            | BCM_MPLS_EGRESS_LABEL_CONTROL_WORD;
    plabel->egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    plabel->egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    plabel->encap_access = bcmEncapAccessTunnel1;
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, plabel);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_tunnel_initiator_create evpn label\n");
        return rv;
    }

    nwk_grp_from_esem = *dnxc_data_get(unit, "esem", "feature", "etps_properties_assignment", NULL);
    if (evpn_global_info.use_split_horizon && nwk_grp_from_esem)
    {
        /*
         * Default egress native AC with network group for the IMLs (used for
         * setting up split horizon)
         */
        printf("Create egress default native AC with EVPN network group ID for split horizon\n");
        bcm_vlan_port_t_init(&evpn_global_info.default_native_ac);
        evpn_global_info.default_native_ac.criteria = BCM_VLAN_PORT_MATCH_NONE;
        evpn_global_info.default_native_ac.flags = BCM_VLAN_PORT_DEFAULT | BCM_VLAN_PORT_NATIVE
                | BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
        evpn_global_info.default_native_ac.egress_network_group_id = evpn_global_info.evpn_network_group_id;
        rv = bcm_vlan_port_create(unit, &evpn_global_info.default_native_ac);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: bcm_vlan_port_create default_native_ac\n");
            return rv;
        }
    }

    /** JR2: Create fabric MC for each core in case of egress Multicast. */
    system_headers_mode = soc_property_get(unit, "system_headers_mode", 1);
    if (egress_mc && (system_headers_mode == 0) && (*dnxc_data_get(unit, "fabric", "general", "blocks_exist", NULL)))
    {
        int nof_cores = *dnxc_data_get(unit, "device", "general", "nof_cores", NULL);
        bcm_module_t mreps[2];
        mreps[0] = 0;
        mreps[1] = 1; /* Ignore it in single core devices*/
        rv = bcm_fabric_multicast_set(unit, cint_evpn_info.vsi, 0, nof_cores, mreps);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_fabric_multicast_set \n");
            return rv;
        }
    }

    printf("Create IML to carry BUM from PE1 to PE2 - with CW + ESI\n");
    pe1_pe2_iml_info.add_to_flooding = 1;
    pe1_pe2_iml_info.iml_label = cint_evpn_info.pe1_pe2_iml_label;
    pe1_pe2_iml_info.dual_homed_peer_profile = evpn_global_info.dual_homed_peer_profile;
    pe1_pe2_iml_info.lsp_tunnel_id = *(mpls_util_entity[1].fecs[0].tunnel_gport);
    pe1_pe2_iml_info.out_port = *(mpls_util_entity[1].fecs[0].port);
    rv = iml_cw_encap_add(unit, &pe1_pe2_iml_info, &evpn_global_info.pe1_pe2_iml_egress_label);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in iml_encap_add pe1 to pe2\n");
        return rv;
    }

    printf("Create IML to carry BUM from PE1 to PE3 - with CW no ESI\n");
    pe1_pe3_iml_info.add_to_flooding = 1;
    pe1_pe3_iml_info.iml_label = cint_evpn_info.pe1_pe3_iml_label;
    pe1_pe3_iml_info.dual_homed_peer_profile = 0; /* Peer is not dual homed to this PE */
    pe1_pe3_iml_info.lsp_tunnel_id = *(mpls_util_entity[0].fecs[0].tunnel_gport);
    pe1_pe3_iml_info.out_port = *(mpls_util_entity[0].fecs[0].port);
    rv = iml_cw_encap_add(unit, &pe1_pe3_iml_info, &evpn_global_info.pe1_pe3_iml_egress_label);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in iml_encap_add pe1 to pe3\n");
        return rv;
    }

    printf("Create IML to carry BUM from PE1 to PE4 - No CW No ESI\n");
    pe1_pe4_iml_info.add_to_flooding = 1;
    pe1_pe4_iml_info.iml_label = cint_evpn_info.pe1_pe4_iml_label;
    pe1_pe4_iml_info.dual_homed_peer_profile = evpn_global_info.dual_homed_peer_profile - 1; /* For testing, Peer is
                                                                                                dual homed to this PE */
    pe1_pe4_iml_info.lsp_tunnel_id = *(mpls_util_entity[0].fecs[1].tunnel_gport);
    pe1_pe4_iml_info.out_port = *(mpls_util_entity[0].fecs[1].port);
    rv = iml_encap_add(unit, &pe1_pe4_iml_info, &evpn_global_info.pe1_pe4_iml_egress_label);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in iml_encap_add pe1 to pe4\n");
        return rv;
    }

    printf("Add ESI encapsulation for IML traffic from ESI-200 to PE2\n");
    bcm_mpls_esi_info_t_init(&pe1_pe2_esi_200_encap);
    pe1_pe2_esi_200_encap.esi_label = cint_evpn_info.pe1_pe2_ce1_esi_label;
    pe1_pe2_esi_200_encap.out_class_id = evpn_global_info.dual_homed_peer_profile;
    pe1_pe2_esi_200_encap.src_port = evpn_global_info.ce1_vlan_port.port;
    rv = bcm_mpls_esi_encap_add(unit, &pe1_pe2_esi_200_encap);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_esi_encap_add esi-200\n");
        return rv;
    }

    printf("Add ESI encapsulation for IML traffic from ESI-300 to PE2\n");
    bcm_mpls_esi_info_t_init(&pe1_pe2_esi_300_encap);
    pe1_pe2_esi_300_encap.esi_label = cint_evpn_info.pe1_pe2_ce3_esi_label;
    pe1_pe2_esi_300_encap.out_class_id = evpn_global_info.dual_homed_peer_profile;
    pe1_pe2_esi_300_encap.src_port = evpn_global_info.ce3_vlan_port.port;
    rv = bcm_mpls_esi_encap_add(unit, &pe1_pe2_esi_300_encap);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_esi_encap_add esi-300\n");
        return rv;
    }

    /*
     * For testing only - step out of the scenario described in cint_evpn.c by setting a dual-homed peer profile
     * to PE4, and adding an ESI encapsulation in case traffic arrives from port 13. This is meant to test traffic
     * to a peer with set dual-homed profile and ESI DB entry with this profile in the key, and check that the
     * ESI label isn't really added to the egressing traffic.
     */
    printf("Add ESI encapsulation for IML traffic from ESI-400 to PE4\n");
    bcm_mpls_esi_info_t_init(&pe1_pe4_esi_400_encap);
    pe1_pe4_esi_400_encap.esi_label = extra_esi_label;
    pe1_pe4_esi_400_encap.out_class_id = evpn_global_info.dual_homed_peer_profile - 1;
    pe1_pe4_esi_400_encap.src_port = extra_port_id;
    rv = bcm_mpls_esi_encap_add(unit, &pe1_pe4_esi_400_encap);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_esi_encap_add esi-400\n");
        return rv;
    }

    return rv;
}

/*
 * Add IML encapsulation with/out ESI label and adds it to the VSI
 * flooding group.
 *   [in]  iml_info - all the required information to set up the label (see iml_encap_info_s for field description).
 *   [out] plabel - allocated egress label object pointer. Will be filled by the function.
 */
int
iml_cw_encap_add(
    int unit,
    iml_encap_info_s * iml_info,
    bcm_mpls_egress_label_t * plabel)
{
    int rv = BCM_E_NONE;
    bcm_gport_t gport;

    printf("- Create egress IML label\n");
    bcm_mpls_egress_label_t_init(plabel);
    plabel->label = iml_info->iml_label;
    plabel->flags = BCM_MPLS_EGRESS_LABEL_IML | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT |
            BCM_MPLS_EGRESS_LABEL_CONTROL_WORD;
    plabel->egress_qos_model.egress_qos = bcmQosEgressModelUniform;
    plabel->egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
    plabel->encap_access = bcmEncapAccessTunnel1;
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, plabel);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }
    printf(" - IML encapsulation created, ID = 0x%08x\n", plabel->tunnel_id);
    /*
     * tunnel_id is generated as ITF in bcm_mpls_tunnel_initiator_create
     * convert to GPort for further processing.
     */
    BCM_L3_ITF_LIF_TO_GPORT_TUNNEL(gport, plabel->tunnel_id);

    if (iml_info->dual_homed_peer_profile)
    {
        printf("- Set dual homed peer profile on IML label lif\n");
        rv = bcm_port_class_set(unit, gport, bcmPortClassEgress, iml_info->dual_homed_peer_profile);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in bcm_port_class_set for dual homed peer id (%d) of lif (0x%08x)\n",
                   iml_info->dual_homed_peer_profile, lif_gport);
            return rv;
        }
    }

    if (iml_info->add_to_flooding)
    {
        if (iml_info->lsp_tunnel_id > 0)
        {
            /*
             * Add egress label and "FEC" to the flooding group.
             * "FEC" is simulated by adding PPMC entry that contain
             * both the new label and the MPLS encapsulation, and
             * the entry ID + out port is added to the MC group.
             */
            int mc_ext_id = 0;
            int cuds[2];
            printf("- Add IML and MPLS LSP to PPMC\n");
            cuds[0] = plabel->tunnel_id;
            cuds[1] = iml_info->lsp_tunnel_id;
            rv = bcm_multicast_encap_extension_create(unit, 0, &mc_ext_id, 2, cuds);
            if (rv != BCM_E_NONE)
            {
                printf("ERROR: bcm_multicast_encap_extension_create\n");
                return rv;
            }
            printf(" - Created PPMC entry with ID = 0x%08x\n", mc_ext_id);

            printf("- Add to PPMC pointer and port to MC group\n");
            rv = multicast__add_multicast_entry(unit, cint_evpn_info.vsi,
                                                &iml_info->out_port, &mc_ext_id, 1, egress_mc);
            if (rv != BCM_E_NONE)
            {
                printf("ERROR: multicast__add_multicast_entry\n");
                return rv;
            }
            printf(" - <port=%d, rep_idx=0x%08x> added to MC group %d\n",
                iml_info->out_port, mc_ext_id, cint_evpn_info.vsi);
        }
        else
        {
            /*
             * Add only the egress label to the flooding group. It is expected to point
             * to the next egress object in the encapsulation stack.
             */
            int encap_id = BCM_L3_ITF_VAL_GET(plabel->tunnel_id);
            printf("- Add to outlif and port to MC group\n");
            rv = multicast__add_multicast_entry(unit, cint_evpn_info.vsi,
                                                &iml_info->out_port, &encap_id, 1, egress_mc);
            if (rv != BCM_E_NONE)
            {
                printf("ERROR: multicast__add_multicast_entry\n");
                return rv;
            }
            printf(" - <port=%d, outlif=0x%08x> added to MC group %d\n",
                iml_info->out_port, plabel->tunnel_id, cint_evpn_info.vsi);
        }
    }

    if (evpn_global_info.use_split_horizon)
    {
        int nwk_grp_from_esem = *dnxc_data_get(unit, "esem", "feature", "etps_properties_assignment", NULL);

        printf("- Set network group for encapsulated IML\n");
        rv = bcm_port_class_set(unit, gport, bcmPortClassForwardEgress,
                                evpn_global_info.evpn_network_group_id);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in bcm_port_class_set iml egress label\n");
            return rv;
        }

        if (nwk_grp_from_esem)
        {
            bcm_port_match_info_t match_info;
            printf("- Connect IML to default native AC to receive network group ID for Split horizon\n");
            match_info.match = BCM_PORT_MATCH_PORT;
            match_info.flags |= BCM_PORT_MATCH_NATIVE | BCM_PORT_MATCH_EGRESS_ONLY;
            match_info.port = gport;
            rv = bcm_port_match_add(unit, evpn_global_info.default_native_ac.vlan_port_id, &match_info);
            if (rv != BCM_E_NONE)
            {
                printf("ERROR: bcm_port_match_add IML to default native AC\n");
                return rv;
            }
        }
    }

    return rv;
}

/*
 * Configure EVPN termination for traffic arriving from other PEs.
 * if vsi is 0, the LIFs would be created as P2P.
 */
int
evpn_cw_service_ingress_config(
    int unit,
    int vsi)
{
    int rv = BCM_E_NONE;
    bcm_mpls_tunnel_switch_t *tunnel = &evpn_global_info.evpn_in_tunnel;
    bcm_mpls_range_action_t action;
    int feature_mpls_term_1_or_2_labels = *dnxc_data_get(unit, "mpls", "general", "mpls_term_1_or_2_labels", NULL); 
    char *proc_name;

    proc_name = "evpn_service_ingress_config";
    printf("%s(): Create EVPN termination for UC traffic\n",proc_name);
    bcm_mpls_tunnel_switch_t_init(tunnel);
    tunnel->action = BCM_MPLS_SWITCH_ACTION_POP;
    tunnel->flags = BCM_MPLS_SWITCH_NEXT_HEADER_L2;
    tunnel->flags2 = BCM_MPLS_SWITCH2_CONTROL_WORD;
    tunnel->flags2 |= (vsi ? 0 : BCM_MPLS_SWITCH2_CROSS_CONNECT);
    tunnel->label = cint_evpn_info.evpn_label;
    tunnel->vpn = vsi;
    rv = bcm_mpls_tunnel_switch_create(unit, tunnel);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): ERROR: in bcm_mpls_tunnel_switch_create evpn tunnel\n",proc_name);
        return rv;
    }
    printf("%s(): EVPN termination - created. ID = 0x%08x\n", proc_name, tunnel->tunnel_id);

    printf("%s(): Set IML range\n", proc_name);
    action.flags = BCM_MPLS_RANGE_ACTION_EVPN_IML;
    rv = bcm_mpls_range_action_add(unit,cint_evpn_info.iml_range_low_label,
                                   cint_evpn_info.iml_range_high_label, &action);
    if (rv != BCM_E_NONE) {
        printf("%s(): ERROR: in bcm_mpls_range_action_add\n", proc_name);
        return rv;
    }
    printf("%s(): IML range set to MIN = %d(0x%05x), MAX = %d(0x%05x)\n",
        proc_name, cint_evpn_info.iml_range_low_label, cint_evpn_info.iml_range_low_label,
        cint_evpn_info.iml_range_high_label, cint_evpn_info.iml_range_high_label);

    printf("%s(): Add IML termination without ESI\n", proc_name);
    rv = iml_cw_term_add(unit, vsi, cint_evpn_info.pe1_pe2_iml_label,
                      &evpn_global_info.pe1_pe2_iml_tunnel_switch_no_esi,
                      0 /** No ESI **/);
    if (rv != BCM_E_NONE) {
        printf("%s(): ERROR: in iml_term_add no ESI\n", proc_name);
        return rv;
    }

    if (!feature_mpls_term_1_or_2_labels) {
        /**If the feature term 1 or 2 mpls labels is available, no need to add the 2nd IML term entry.*/
        printf("%s(): Add IML termination with ESI\n", proc_name);
        rv = iml_cw_term_add(unit, vsi, cint_evpn_info.pe1_pe2_iml_label,
                             &evpn_global_info.pe1_pe2_iml_tunnel_switch_esi,
                             1 /** Expect ESI **/);
        if (rv != BCM_E_NONE) {
            printf("%s(): ERROR: in iml_term_add expect ESI\n", proc_name);
            return rv;
        }
    }

    return rv;
}

/*
 * Configures an IML termination with or without expected VSI
 */
int
iml_cw_term_add(
    int unit,
    int vsi,
    bcm_mpls_label_t iml_label,
    bcm_mpls_tunnel_switch_t *pswitch,
    int expect_esi)
{
    int rv = BCM_E_NONE;
    int feature_mpls_term_1_or_2_labels = *dnxc_data_get(unit, "mpls", "general", "mpls_term_1_or_2_labels", NULL); 

    printf("- Create IML termination for MC traffic\n");
    bcm_mpls_tunnel_switch_t_init(pswitch);
    pswitch->action = BCM_MPLS_SWITCH_ACTION_POP;
    pswitch->flags = BCM_MPLS_SWITCH_EVPN_IML;
    pswitch->flags2 = BCM_MPLS_SWITCH2_CONTROL_WORD;
    pswitch->flags2 |= (vsi ? 0 : BCM_MPLS_SWITCH2_CROSS_CONNECT);
    if ((!expect_esi) && (!feature_mpls_term_1_or_2_labels))
    {
        pswitch->flags |= BCM_MPLS_SWITCH_EXPECT_BOS;
    }
    pswitch->label = iml_label;
    pswitch->vpn = vsi;
    rv = bcm_mpls_tunnel_switch_create(unit, pswitch);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_tunnel_switch_create iml label\n");
        return rv;
    }

    if (evpn_global_info.use_split_horizon)
    {
        printf("- Set network group for terminated IML\n");
        rv = bcm_port_class_set(unit, pswitch->tunnel_id, bcmPortClassForwardIngress,
                                evpn_global_info.evpn_network_group_id);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in bcm_port_class_set iml ingress label\n");
            return rv;
        }
    }

    /*
     * Set In-LIF profile for iPMF selection of ESI filter application
     */
    if (expect_esi)
    {
        printf("Set In-LIF profile to select ESI FP ingress application\n");
        rv = bcm_port_class_set(unit, pswitch->tunnel_id, bcmPortClassFieldIngressVport,
                                evpn_global_info.in_lif_acl_profile_esi);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in bcm_port_class_set iml label\n");
            return rv;
        }
    }

    printf(" - IML termination %s ESI - created. ID = 0x%08x\n",
        (expect_esi ? "with" : "without"), pswitch->tunnel_id);

    return rv;
}


/*****************************************************************************************************
 * Flooding group profile example
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * This example replaces one of the EVPNs and one of IMLs flooding groups to a new group
 * (manifast in fwd destination).
 * Note: the MC groups themselves are not created in this example, so packets are dropped.
 */
int
evpn_main_flood_group(
    int unit,
    int ce1_port,
    int ce3_port,
    int p1_port,
    int p2_port)
{
    int rv = BCM_E_NONE;
    int feature_mpls_term_1_or_2_labels = *dnxc_data_get(unit, "mpls", "general", "mpls_term_1_or_2_labels", NULL);

    rv = evpn_main(unit, ce1_port, ce3_port, p1_port, p2_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_main\n");
        return rv;
    }

    printf("Setting flooding group profiles to EVPN/IML lifs\n");

    rv = evpn_flood_group_profile_set(unit, evpn_global_info.evpn_in_tunnel.tunnel_id, 1);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_flood_group_profile_set evpn\n");
        return rv;
    }
    rv = evpn_flood_group_profile_set(unit, evpn_global_info.pe1_pe2_iml_tunnel_switch_no_esi.tunnel_id, 2);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_flood_group_profile_set iml no esi\n");
        return rv;
    }
    if (!feature_mpls_term_1_or_2_labels) {
        /** If the device support to terminate one or two labels in one LIF, this tunnel-id (LIF) won't be created.*/
        rv = evpn_flood_group_profile_set(unit, evpn_global_info.pe1_pe2_iml_tunnel_switch_esi.tunnel_id, 3);
        if (rv != BCM_E_NONE)
        {
            printf("ERROR: in evpn_flood_group_profile_set iml with esi\n");
            return rv;
        }
    }

    return rv;
}

int
evpn_flood_group_profile_set(
    int unit,
    bcm_gport_t gport,
    int flooding_group_profile)
{
    int rv = BCM_E_NONE;
    bcm_port_flood_group_t flood_groups;

    printf("- GPort: 0x%08x -> Profile: %d\n", gport, flooding_group_profile);

    bcm_port_flood_group_t_init(&flood_groups);
    BCM_GPORT_MCAST_SET(flood_groups.unknown_unicast_group, flooding_group_profile);
    flood_groups.unknown_multicast_group = flood_groups.unknown_unicast_group;
    flood_groups.broadcast_group = flood_groups.unknown_unicast_group;

    rv = bcm_port_flood_group_set(unit, gport, 0, &flood_groups);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_port_flood_group_set\n");
        return rv;
    }

    return rv;
}


/*
 * The fucntion is based on evpn_main(). it should be run after the main function.
 */
int
evpn_bum_recycle_lsp_swap_set(
    int unit,
    int recyle_port,
    int p1_port,
    int p2_port)
{
    int i;
    int rv = BCM_E_NONE;
    bcm_l2_egress_t recycle_entry;
    int arp_encap_id[3] = { 0 };
    int dst_port[3] = {p1_port, p1_port, p2_port};
    int dst_fec[3] = { 0 };

    char *proc_name = "evpn_bum_recycle_lsp_swap_set";

    /** Create recycle encap */
    bcm_l2_egress_t_init(&recycle_entry);
    recycle_entry.flags = BCM_L2_EGRESS_RECYCLE_HEADER;
    recycle_entry.recycle_app = bcmL2EgressRecycleAppExtendedEncap;
    rv = bcm_l2_egress_create(unit, &recycle_entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l2_egress_create \n");
        return rv;
    }
    if (mpls_util_verbose >= 1)
    {
        printf("%s(): entity 0x%08X: Configure recycle encap entries\n", proc_name, recycle_entry.encap_id);
    }

    /** configure port as recycle port, Part of context selection at 2nd pass VTT1.*/
    rv = bcm_port_control_set(unit, recyle_port, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedEncapUncollapse);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_port_control_set \n");
        return rv;
    }

    /** Replace ARP encap with recycle encap in IML tunnel.*/
    arp_encap_id[0] = *(mpls_util_entity[0].mpls_encap_tunnel[0].l3_intf_id);
    arp_encap_id[1] = *(mpls_util_entity[0].mpls_encap_tunnel[1].l3_intf_id);
    arp_encap_id[2] = *(mpls_util_entity[1].mpls_encap_tunnel[0].l3_intf_id);
    for (i = 0; i < 2; i++)
    {
        if (mpls_util_verbose >= 1)
        {
            printf("%s(): entity %d: update push entries\n", proc_name, i);
        }
        mpls_util_entity[i].mpls_encap_tunnel[0].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_REPLACE;
        mpls_util_entity[i].mpls_encap_tunnel[1].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_REPLACE;
        *(mpls_util_entity[i].mpls_encap_tunnel[0].l3_intf_id) = recycle_entry.encap_id;
        if (i == 0)
        {
            *(mpls_util_entity[i].mpls_encap_tunnel[1].l3_intf_id) = recycle_entry.encap_id;
        }
        rv = mpls_create_initiator_tunnels(unit, mpls_util_entity[i].mpls_encap_tunnel, 2);
        if (rv != BCM_E_NONE)
        {
            printf("%s(): Error, in mpls_create_initiator_tunnels\n", proc_name);
            return rv;
        }
    }

    /** Create FEC entry with ARP encap for pe1_pe3, pe1_pe4, pe1_pe2*/
    bcm_l3_egress_t l3_egr;
    bcm_l3_egress_t_init(&l3_egr);
    l3_egr.flags = 0;
    for (i = 0; i < 3; i++)
    {
        l3_egr.intf = arp_encap_id[i];
        l3_egr.port = dst_port[i];
        rv = bcm_l3_egress_create(unit, BCM_L3_INGRESS_ONLY, &l3_egr, &(dst_fec[i]));
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_l3_egress_create \n");
            return rv;
        }
    }

    /** Configure MPLS swap for LSP in 2nd pass.*/
    bcm_mpls_tunnel_switch_t pswitch;
    bcm_mpls_tunnel_switch_t_init(pswitch);
    pswitch.action = BCM_MPLS_SWITCH_ACTION_SWAP;
    pswitch.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;

    pswitch.port = p1_port;
    pswitch.label = mpls_util_entity[0].mpls_encap_tunnel[0].label[0];  /** pe1_pe3_lsp */
    pswitch.egress_label.label = pswitch.label+1;
    pswitch.egress_if = dst_fec[0];
    rv = bcm_mpls_tunnel_switch_create(unit, &pswitch);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_tunnel_switch_create iml label\n");
        return rv;
    }
    printf("%s(): create SWAP entry with incoming label = %d, outgoing label = %d\n",
           proc_name, pswitch.label, pswitch.egress_label.label);

    pswitch.port = p1_port;
    pswitch.label = mpls_util_entity[0].mpls_encap_tunnel[1].label[0]; /** pe1_pe4_lsp */
    pswitch.egress_label.label = pswitch.label+1;
    pswitch.egress_if = dst_fec[1];
    rv = bcm_mpls_tunnel_switch_create(unit, &pswitch);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_tunnel_switch_create iml label\n");
        return rv;
    }
    printf("%s(): create SWAP entry with incoming label = %d, outgoing label = %d\n",
           proc_name, pswitch.label, pswitch.egress_label.label);

    pswitch.port = p2_port;
    pswitch.label = mpls_util_entity[1].mpls_encap_tunnel[0].label[0]; /** pe1_pe2_lsp */
    pswitch.egress_label.label = pswitch.label+1;
    pswitch.egress_if = dst_fec[2];
    rv = bcm_mpls_tunnel_switch_create(unit, &pswitch);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_tunnel_switch_create iml label\n");
        return rv;
    }
    printf("%s(): create SWAP entry with incoming label = %d, outgoing label = %d\n",
           proc_name, pswitch.label, pswitch.egress_label.label);

    return rv;
}


/* 
 *  EVPN core diagram:
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  |                                            .  .  . . .      . . .   . . .                                         |
 *  |                                          .            .  .       .       .                                        |
 *  |   Access                               .                                  .                            Access     |                       
 *  |                                      .                MPLS Core            . .                                    |
 *  |                                    .   /+-----+  -   -   -   -   -   -+-----+  .                                  |             
 *  |   +--------+                     .      | P1  | \                 /   | P3  | \                        +--------+ | 
 *  |  / Root   /|                    .   /   |     |                       |     | \  .                    / Root   /| | 
 *  | +--------+ |                   .        +-----+   \             /     +-----+   \  .                 +--------+ | |
 *  | |  CE1   | |\                 .  /       / |                             |      \   .               /|  CE2   | | |
 *  | |        |/  \    +--------+ .                      \         /                   \   .+--------+  / |        |/  |
 *  | +--------+\ esi  /        /| /        /    |                             |       \    /        /| esi+--------+   |
 *  |             200\+--------+ |                          \     /                       \+--------+ |/100/            |
 *  |             \   |  PE1   | |      /        |                             |        \  |   PE3  | |   /             |
 *  |             DF  |        |/ \                          \ /                           |        |/   DF             |
 *  |               \ +--------+     /           |                             |         \/+--------+\  /               |
 *  |                 /             \                        / \                                      \/                |
 *  |                             /              |                             |        / \           /\                |
 *  |               /  \+--------+    \                    /     \                         +--------+/  \               |
 *  |             DF   /        /|\              |                             |      /   /        /|    DF             |
 *  |             /   +--------+ |      \                /         \                     +--------+ |\    \             |
 *  |   +--------+ esi|  PE2   | |. \            |                             |    /    |   PE4  | | \ esi\ +--------+ |
 *  |  / leaf   /| 300|        |/  .      \            /             \                   |        |/   \400 / Leaf   /| |
 *  | +--------+ |  / +--------+    . \          |                             |  /     /+--------+     \  +--------+ | |
 *  | |  CE3   | | /                 .      \+-----+  /                 \   +-----+       .              \ |  CE4   | | |
 *  | |        |/                      .\    | P2  |                        | P4  |   / .                 \|        |/  |
 *  | +--------+ /                      . \  |     |/                     \ |     |    .                   +--------+   |
 *  |                                   .   \+-----+  -   -   -   -   -  -  +-----+ / .                                 |
 *  |                                     .                                   . .  .                                    |
 *  |                                      .               . .       .      .                                           |
 *  |                                        .  .   .  .  .   .  .  . . . .                                             |
 *  |                                                                                                                   |
 *  |                                                                                                                   |
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/*
 * Main function to configure the scenario described above and in cint_evpn.c
 * The function performs the following activities:
 * - Setup the VSI
 * - Create ETH segments - in this example - AC links
 * - Configure a L3 MPLS core connecting the device to neighboring MPLS routers
 * - Setup EVPN services for L2 MC and UC over the MPLS core
 * - Add static entries in MACT to emulate BGP triggered address advertising.
 * Traffic for the tests:
 * - UC traffic from CE1 to CE2 via PE4, passed with EVPN encapsulation. SA learned without Leaf indication.
 * - UC traffic from CE2 to CE1 via PE2, passed with EVPN termination.
 * - UC traffic from CE3 to CE4 via PE3, droped due to Leaf filter(PMF). SA learned with Leaf indication.
 * - PE1 BUM traffic from CE1, 4 copies:
 *   -- One copy to CE3 (ETH), droped due to orientation filter;
 *   -- one copy to PE2 (IML with ESI)
 *   -- two copies to PE3/PE4 (IML)
 * - PE1 BUM traffic from P2 (without Leaf/ESI label), 4 copies
 *   -- One copy to CE1, passed
 *   -- One copy to CE3, passed
 *   -- Two copies back to core dropped due to orientation.
 * - PE1 BUM traffic from P2 (with ESI label), 4 copies
 *   -- One copy to CE1, droped due to ESI filter.
 *   -- One copy to CE3, passed.
 *   -- Two copies back to core dropped due to orientation.
 * - PE1 BUM traffic from CE3 (ETH), 4 copies
 *   -- One copy to CE1 (ETH), passed.
 *      (The copy should not exist actually since PE1 is not the DF of CE1. We set it for observing Leaf filter.)
 *   -- Three copies to PE2/PE3/PE4 (IML with Leaf).
 * - PE1 BUM traffic from P2 (with Leaf label), 4 copies
 *   -- One copy to CE1 (ETH), Passed.
 *      (The copy should not exist actually since PE1 is not the DF of CE1. We set it for observing Leaf filter.)
 *   -- One copy to CE3 (ETH), droped due to Leaf filter(orientation).
 *   -- Two copies back to core dropped due to orientation.
 */
int
evpn_etree_main(
    int unit,
    int ce1_port,
    int ce3_port,
    int p1_port,
    int p2_port)
{
    int rv = BCM_E_NONE;
    int vswitch_ports[4];

    printf("~~ EVPN E-Tree Basic Example ~~\n");
    is_evpn_etree = TRUE;

    printf("Load application profiles and misc configs\n");
    rv = evpn_profiles_set(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_profiles_set\n");
        return rv;
    }

    printf("Create VPN (%d)\n", cint_evpn_info.vsi);
    rv = evpn_vswitch_create(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_mpls_vpn_create\n");
        return rv;
    }

    vswitch_ports[0] = ce1_port;
    vswitch_ports[1] = ce3_port;
    vswitch_ports[2] = p1_port;
    vswitch_ports[3] = p2_port;
    rv = evpn_vswitch_add_ports(unit, 4, vswitch_ports);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, evpn_vswitch_add_ports");
        return rv;
    }

    printf("Init ESI filter FP application\n");
    rv = evpn_esi_filter_init(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, evpn_esi_filter_init");
        return rv;
    }

    printf("Init Etree filter FP application\n");
    rv = evpn_etree_filter_init(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, evpn_etree_filter_init");
        return rv;
    }

    printf("Create ETH segments\n");
    rv = evpn_eth_segments_create(unit, ce1_port, ce3_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_eth_segment_create\n");
        return rv;
    }

    printf("Setup MPLS L3 network\n");
    rv = evpn_mpls_core_setup(unit, p1_port, p2_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_mpls_core_setup\n");
        return rv;
    }

    printf("Setup EVPN Services\n");
    rv = evpn_service_egress_config(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_egress_tunnels_config\n");
        return rv;
    }
    printf("Setup EVPN Etree leaf encap\n");
    rv = evpn_etree_leaf_encap_config(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_etree_leaf_encap_config\n");
        return rv;
    }
    rv = evpn_service_ingress_config(unit, cint_evpn_info.vsi);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_ingress_tunnels_config\n");
        return rv;
    }

    printf("Connect ES to egress in UC flow\n");
    rv = evpn_egress_uc_l2_address_add(unit);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in evpn_egress_uc_l2_address_add\n");
        return rv;
    }

    return rv;
}

/*
 * Setup Field Processor based Etree filter.
 * in UC, the filter for leaf -> leaf is done in ingress PE with iPMF.
 *     Suggested preselectors: in_lif_profile, fwd_layer_index, etc.
 *     Suggested qualifiers:
 *        inAC.leaf_indication, UDQ based on bcmFieldQualifyAcInLifWideData
 *        MACT.leaf_indication, UDQ based on bcmFieldQualifyDstClassL2
 *     actions: drop if both are set.
 * in BUM, the filter for leaf -> leaf is done in egress PE with iPMF and ePMF.
 *   The recommended solution is as below:
 *     iPMF (Leaf DB) --
 *     Suggested preselectors: in_lif_profile, fwd_layer_index, fwd_layer_type, etc.
 *     Suggested qualifiers: MPLS.LABEL (the label after IML).
 *     actions: drop(if no match), bcmFieldActionInVportClass0(update in_lif_profile0 with nwk_group_id).
 *     Thus, the traffic is filtered by split-horizon.
 *   User can also use other solution if according to his applications, such as use the same
 *   Filter meachnism and DBs as ESI as below:
 *     iPMF -- Same as ESI.
 *     ePMF -- Leaf entries should be added as below:
 *        Leaf_lable + out_port -> pass for port to root site.       
 *
 *   Here to simplify the example we use the latter solution.
 */
int
evpn_etree_filter_init(
    int unit)
{
    int rv = BCM_E_NONE;

    /** We use the same in-lif-profile as ESI since the fwd_layer_index is different*/
    rv = cint_field_evpn_etree_ingress(unit, evpn_global_info.in_lif_acl_profile_esi);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, cint_field_evpn_etree_ingress\n");
        return rv;
    }

    /** Add leaf -> root entries for traffic*/
    rv = cint_field_evpn_etree_egress_leaf_port_entry_add(unit, evpn_etree_info.pe1_pe2_ce3_leaf_label, cint_evpn_info.ce1_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, cint_field_evpn_etree_egress_leaf_port_entry_add\n");
        return rv;
    }

    rv = cint_field_evpn_etree_egress_leaf_port_entry_add(unit, evpn_etree_info.pe1_pe3_ce3_leaf_label, cint_evpn_info.ce1_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, cint_field_evpn_etree_egress_leaf_port_entry_add\n");
        return rv;
    }

    rv = cint_field_evpn_etree_egress_leaf_port_entry_add(unit, evpn_etree_info.pe1_pe4_ce3_leaf_label, cint_evpn_info.ce1_port);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR, cint_field_evpn_etree_egress_leaf_port_entry_add\n");
        return rv;
    }

    return rv;
}

/*
 * Configure EVPN Etree Leaf encapsulation for traffic from PE1 to other PEs
 */
int
evpn_etree_leaf_encap_config(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_mpls_esi_info_t pe1_pe2_esi_200_encap;
    bcm_mpls_esi_info_t pe1_pe3_esi_400_encap;
    bcm_mpls_esi_info_t pe1_pe4_esi_100_encap;


    printf("Add ESI encapsulation for IML traffic from ESI-300 to PE2\n");
    bcm_mpls_esi_info_t_init(&pe1_pe2_esi_200_encap);
    pe1_pe2_esi_200_encap.esi_label = evpn_etree_info.pe1_pe2_ce3_leaf_label;
    pe1_pe2_esi_200_encap.out_class_id = evpn_global_info.dual_homed_peer_profile;
    pe1_pe2_esi_200_encap.src_port = evpn_global_info.ce3_vlan_port.port;
    rv = bcm_mpls_esi_encap_add(unit, &pe1_pe2_esi_200_encap);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_esi_encap_add esi-300\n");
        return rv;
    }

    printf("Add ESI encapsulation for IML traffic from ESI-300 to PE3\n");
    bcm_mpls_esi_info_t_init(&pe1_pe3_esi_400_encap);
    pe1_pe3_esi_400_encap.esi_label = evpn_etree_info.pe1_pe3_ce3_leaf_label;
    pe1_pe3_esi_400_encap.out_class_id = 0;
    pe1_pe3_esi_400_encap.src_port = evpn_global_info.ce3_vlan_port.port;
    rv = bcm_mpls_esi_encap_add(unit, &pe1_pe3_esi_400_encap);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_esi_encap_add esi-400\n");
        return rv;
    }

    printf("Add ESI encapsulation for IML traffic from ESI-300 to PE4\n");
    bcm_mpls_esi_info_t_init(&pe1_pe4_esi_100_encap);
    pe1_pe4_esi_100_encap.esi_label = evpn_etree_info.pe1_pe4_ce3_leaf_label;
    pe1_pe4_esi_100_encap.out_class_id = evpn_global_info.dual_homed_peer_profile - 1;
    pe1_pe4_esi_100_encap.src_port = evpn_global_info.ce3_vlan_port.port;
    rv = bcm_mpls_esi_encap_add(unit, &pe1_pe4_esi_100_encap);
    if (rv != BCM_E_NONE)
    {
        printf("ERROR: in bcm_mpls_esi_encap_add esi-100\n");
        return rv;
    }

    return rv;
}



