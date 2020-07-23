/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_vpls_roo_advanced.c Purpose: adding advanced scenarios of vpls roo
 */

/*
 *
 * Configuration:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/sand/cint_ip_route_rpf_basic.c
 * cint ../../../../src/examples/sand/cint_vpls_roo_basic.c
 * cint ../../../../src/examples/sand/cint_vpls_roo_advanced.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int access_port = 200;
 * int core_port = 201;
 * rv = vpls_roo_basic_main(unit,access_port,core_port);
 * print rv;
 *
 *
 * This file will have functions related to the vpls_roo advanced scenarios
 * Scenarios supported:
 *      URPF check in Tunnel to native vpls roo scenario
 *      VPLS ROO tunnel to tunnel
 *
 * URPF check Traffic:
 *
 *  Send through port 201 IPv4oETHoMPLSoETH out of core. Route into access with IPv4oETH on port 200:
 *
 *  #####################################################################################################################################
 *   ----------->
 *
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+++-+-++-+-+++-+-++-+-+-+-+-+-+-+-+-+-++-+--++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-++-+-+-+-+-+-
 *   |    |      DA         | SA              |  mpls    |MPLS (PWE)|      DA         | SA              ||  SIP         ||  DIP          ||  TTL ||
 *   |    |00:0F:00:02:0A:33|00:00:00:00:CD:1E|Label:4444|Label:3333|00:0F:00:02:0A:22|00:00:00:00:FD:2F||127.255.255.01||127.255.255.03 || 128  ||
 *   |    |                 |                 |Exp:0     |Exp:0     |                 |                 ||127.255.255.04||               ||      ||
 *   |    |                 |                 |          |          |                 |                 ||              ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+++-+-+-+-+++-+-+--++-+-+-+-+-+-+-+-+-+--+--++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-++-+-+-+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *   <------------
 *
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-++-+-+-+-+-+-
 *   |    |      DA         | SA              ||TPID1 |VID||  SIP         ||  DIP         ||  TTL ||
 *   |    |00:00:00:00:CD:1F|00:0F:00:02:0A:44||0x8100|10 ||127.255.255.01||127.255.255.03|| 127  ||
 *   |    |                 |                 ||      |   ||              ||              ||      ||
 *   |    |                 |                 ||      |   ||              ||              ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-++-+-+-+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * For sip = 127.255.255.01 RPF check will pass and packet exits core
 * For sip = 127.255.255.04 RPF check will fail and packet gets dropped
 * ##################################################################################################
 *
 *
 *  ROO Tunnel to Tunnel traffic :
 *
 *  Send through port 201 IPv4oETHoMPLSoETH out of core. Route into access with IPv4oETH on port 202:
 *
 *  #####################################################################################################################################
 *   ----------->
 *
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+---+-+++-+-++-+-+++-+-++-+-+-+-+-+-+-+-+-+-++-+--++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-++-+-+-+-+-+-
 *   |    |      DA         | SA              |  mpls    |MPLS (PWE)|      DA         | SA              ||  SIP          ||  DIP          ||  TTL ||
 *   |    |00:0F:00:02:0A:33|00:00:00:00:CD:1e|Label:4444|Label:3333|00:0F:00:02:0A:22|00:00:00:00:FD:2F||0000:0000:0000:||0000:0000:0000:|| 128  ||
 *   |    |                 |                 |Exp:0     |Exp:0     |                 |                 ||:0000:1234:0000||:0000:1234:CDEF||      ||
 *   |    |                 |                 |          |          |                 |                 ||  FF00:FF13    ||  FF00:1234    ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+---+-+++-+-+-+-+++-+-+--+-+-+-+-+-+-+-+-+-+--+--++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-++-+-+-+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *   <------------

  *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+---+-+++-+-++----+-+++-+-++----+-+-+-+-+-+-+-+-+-++-+--++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-++-+-+-+-+-+-
 *   |    |      DA         | SA              |  mpls       |MPLS (PWE)   |      DA         | SA              ||  SIP          ||  DIP          ||  TTL ||
 *   |    |00:0F:00:02:0A:66|00:00:00:00:CD:2e|Label:0xaaaa |Label:0xcccc |00:0F:00:02:0A:55|00:00:00:00:CD:2D||0000:0000:0000:||0000:0000:0000:|| 127  ||
 *   |    |                 |                 |Exp:0        |Exp:0        |                 |                 ||:0000:1234:0000||:0000:1234:CDEF||      ||
 *   |    |                 |                 |             |             |                 |                 ||  FF00:FF13    ||  FF00:1234    ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+---+-+++-+-+----+-+++-+-+-----+-+-+-+-+-+-+-+-+-+--+--++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-++-+-+-+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

struct cint_vpls_roo_advanced_info_s
{
    uint32 sip;                                             /*SIP of the v4 tunnel native header for rpf checks*/
    int rpf_fec_id;                                         /*Fec id for rpf check on Tunnel to native scenario*/
    bcm_ip6_t v6_sip;                                       /*v6 sip*/
    bcm_ip6_t v6_dip;                                       /*v6 dip*/
    bcm_ip6_t v6_mask;
    int v6_core_native_fec_id;                              /* v6 core native fec id for native arp and rif*/
    int v6_core_overlay_fec_id;                             /* v6 core overlay fec id to get overlay info*/
    int v6_core_overlay_fec_id2;                             /* core overlay fec id for overlay (third hierarchy) */
    int v6_core_port;                                       /* v6 core port*/
    int v6_core_native_eth_rif;                             /* v6 core native RIF */
    int v6_core_overlay_eth_rif;                            /* v6 core overlay RIF */
    bcm_mac_t v6_core_native_eth_rif_mac;                   /* v6 core native eth rif*/
    bcm_mac_t v6_core_overlay_eth_rif_mac;                  /* v6 core overlay rif*/
    bcm_mac_t v6_core_native_next_hop_mac;                  /* mac for next v6 core native hop */
    bcm_mac_t v6_core_overlay_next_hop_mac;                 /* mac for next v6 core overlay hop */
    int v6_core_native_arp_id;                              /* Id for v6 core native ARP entry */
    int v6_core_overlay_arp_id;                             /* Id for v6 core overlay ARP entry */
    bcm_if_t v6_core_overlay_mpls_tunnel_id;                /* tunnel id for encapsulation entry */
    bcm_mpls_label_t v6_core_overlay_mpls_tunnel_label_1;   /* pushed label */
    bcm_mpls_label_t v6_core_overlay_mpls_tunnel_label_2;   /* pushed label */
    bcm_mpls_label_t v6_core_overlay_pwe_label;             /* pwe label */
    bcm_gport_t v6_core_overlay_mpls_port_id_eg;            /* global lif encoded as MPLS port for overlay 1 (egress side) */
    bcm_if_t v6_core_overlay_pwe_encap_id;                  /* The global lif of the EEDB PWE entry. */
};

/*struct inititalization*/
cint_vpls_roo_advanced_info_s cint_vpls_roo_advanced_info = {
    /*sip*/
    0x7fffff01,
    /*rpf_fec_id*/
    78650,
    /*v6_sip*/
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0x00, 0x00, 0xFF, 0x00, 0xFF, 0x13 },
    /*v6_dip*/
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x34, 0xcd, 0xef, 0xFF, 0x00, 0x12, 0x34 },
    /*v6_mask*/
    { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },
    /*v6_core_native_fec_id | v6_core_overlay_fec_id | v6_core_overlay_fec_id2*/
    0,0,0,
    /* v6_core_port*/
    202,
    /*v6_core_native_eth_rif*/
    40,
    /*v6_core_overlay_eth_rif*/
    50,
    /*v6_core_native_eth_rif_mac | v6_core_overlay_eth_rif_mac*/
    {0x00, 0x0F, 0x00, 0x02, 0x0A, 0x55}, {0x00, 0x0F, 0x00, 0x02, 0x0A, 0x66},
    /* v6_core_native_next_hop_mac |  v6_core_overlay_next_hop_mac */
    {0x00, 0x00, 0x00, 0x00, 0xCD, 0x2D}, {0x00, 0x00, 0x00, 0x00, 0xCD, 0x2E},
    /*v6_core_native_arp_id | v6_core_overlay_arp_id*/
    0,0,
     /* v6_core_overlay_mpls_tunnel_id */
    8195,
    /*
     * v6_core_overlay_mpls_tunnel_label_1  | v6_core_overlay_mpls_tunnel_label_2 |  v6_core_overlay_pwe_label
     */
    0xaaaa, 0xbbbb, 0xcccc,
    /*v6_core_overlay_mpls_port_id_eg | v6_core_overlay_pwe_encap_id*/
    0,0

};

int
vpls_roo_advanced_configure_port_properties(
    int unit,
    int core_port)
{
    int rv = BCM_E_NONE;

    /*
     * set class for v6 core port
     */
    rv = bcm_port_class_set(unit, core_port, bcmPortClassId, core_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_class_set, port=%d, \n", core_port);
        return rv;
    }
    return rv;
}

int
vpls_roo_advanced_create_eth_rifs(
    int unit)
{
    int rv;
    /*
     * create vlan based on the vsi (vpn)
     */
    rv = bcm_vlan_create(unit, cint_vpls_roo_advanced_info.v6_core_native_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Failed (%d) to create VLAN %d\n", rv, cint_vpls_roo_advanced_info.v6_core_native_eth_rif);
        return rv;
    }

    /*
     * create vlan based on the vsi (vpn)
     */
    rv = bcm_vlan_create(unit, cint_vpls_roo_advanced_info.v6_core_overlay_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Failed (%d) to create VLAN %d\n", rv, cint_vpls_roo_advanced_info.v6_core_overlay_eth_rif);
        return rv;
    }

    return rv;
}

int
vpls_roo_advanced_create_l3_interfaces(
    int unit)
{

    int rv;

    rv = intf_eth_rif_create(unit, cint_vpls_roo_advanced_info.v6_core_native_eth_rif,
                             cint_vpls_roo_advanced_info.v6_core_native_eth_rif_mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create pwe_intf\n");
    }

    rv = intf_eth_rif_create(unit, cint_vpls_roo_advanced_info.v6_core_overlay_eth_rif,
                             cint_vpls_roo_advanced_info.v6_core_overlay_eth_rif_mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create pwe_intf\n");
    }
    return rv;

}

int
vpls_roo_advanced_create_arp_entries(
    int unit)
{
    int rv;

    /*
     * Configure native ARP entry
     */
    rv = l3__egress_only_encap__create_inner(unit, 0, &(cint_vpls_roo_advanced_info.v6_core_native_arp_id),
                                       cint_vpls_roo_advanced_info.v6_core_native_next_hop_mac,
                                       cint_vpls_roo_advanced_info.v6_core_native_eth_rif,
                                       BCM_L3_NATIVE_ENCAP,
                                       BCM_L3_FLAGS2_VLAN_TRANSLATION);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
    }

    /*
     * Configure overlay ARP entry
     */
    rv = l3__egress_only_encap__create(unit, 0, &(cint_vpls_roo_advanced_info.v6_core_overlay_arp_id),
                                       cint_vpls_roo_advanced_info.v6_core_overlay_next_hop_mac,
                                       cint_vpls_roo_advanced_info.v6_core_overlay_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
    }
    return rv;
}

int
vpls_roo_advanced_mpls_port_add_encapsulation(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;
    mpls_port.flags |= BCM_MPLS_PORT_EGRESS_TUNNEL;

    mpls_port.egress_label.label = cint_vpls_roo_advanced_info.v6_core_overlay_pwe_label;
    mpls_port.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    if(soc_property_get(unit, spn_SYSTEM_HEADERS_MODE, 1) == 0){
        mpls_port.egress_tunnel_if =cint_vpls_roo_advanced_info.v6_core_overlay_mpls_tunnel_id;
    }

    rv = bcm_mpls_port_add(unit, 0, mpls_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_mpls_port_add encapsulation\n", rv);
        return rv;
    }

    cint_vpls_roo_advanced_info.v6_core_overlay_mpls_port_id_eg = mpls_port.mpls_port_id;
    cint_vpls_roo_advanced_info.v6_core_overlay_pwe_encap_id = mpls_port.encap_id;

    return rv;
}
int
vpls_roo_advanced_create_fec_entries(
    int unit)
{
    int rv;
    int flags = 0;
    bcm_gport_t gport;
    int pointer_to_pwe;
    l3_egress_s l3_egress;

    /*
     * Create overlay FEC (third hierarchy)  for routing into overlay:
     * 1) Tunnel id of the overlay MPLS .
     * 2) Out port.
     */
    gport = 0;
    sand_utils_l3_egress_s_init(unit,0,&l3_egress);
    BCM_GPORT_LOCAL_SET(gport, cint_vpls_roo_advanced_info.v6_core_port);
    l3_egress.l3_flags = BCM_L3_3RD_HIERARCHY;
    l3_egress.allocation_flags = BCM_L3_INGRESS_ONLY;
    l3_egress.out_gport = gport;
    l3_egress.out_tunnel_or_rif = cint_vpls_roo_advanced_info.v6_core_overlay_mpls_tunnel_id;
    rv = sand_utils_l3_egress_create(unit, &l3_egress);
    if (rv != BCM_E_NONE)
    {
        printf("Error in creating overlay FEC, 3rd hierarchy, for routing into overlay\n");
        return rv;
    }

    cint_vpls_roo_advanced_info.v6_core_overlay_fec_id2 = l3_egress.fec_id;

    /*
     * Create overlay FEC (second hierarchy) for routing into overlay:
     * 1) PWE gport converted to type l3_itf.
     * 2) Point to third hierarchy FEC (that points to MPLS tunnel).
     */

    gport = 0;
    sand_utils_l3_egress_s_init(unit,0,&l3_egress);
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_vpls_roo_advanced_info.v6_core_overlay_fec_id2);
    /* Convert to type L3_ITF, according to APT's expectation */
    pointer_to_pwe = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(cint_vpls_roo_advanced_info.v6_core_overlay_mpls_port_id_eg);
    BCM_L3_ITF_SET(pointer_to_pwe,BCM_L3_ITF_TYPE_LIF,pointer_to_pwe);
    l3_egress.out_tunnel_or_rif = pointer_to_pwe;
    l3_egress.out_gport = gport;
    l3_egress.allocation_flags = BCM_L3_INGRESS_ONLY;
    l3_egress.l3_flags = BCM_L3_2ND_HIERARCHY;
    rv = sand_utils_l3_egress_create(unit, &l3_egress);
    if (rv != BCM_E_NONE)
    {
        printf("Error in creating overlay FEC, second hierarchy, for routing into overlay\n");
        return rv;
    }

    cint_vpls_roo_advanced_info.v6_core_overlay_fec_id = l3_egress.fec_id;

    /*
     * Create native FEC (first hierarchy) for routing into overlay:
     * 1) Native out rif.
     * 2) Native arp id.
     * 3) Point to overlay fec (second hierarchy).
     */
    gport = 0;
    sand_utils_l3_egress_s_init(unit,0,&l3_egress);
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_vpls_roo_advanced_info.v6_core_overlay_fec_id);
    l3_egress.out_tunnel_or_rif = cint_vpls_roo_advanced_info.v6_core_native_eth_rif;
    l3_egress.arp_encap_id = cint_vpls_roo_advanced_info.v6_core_native_arp_id;
    l3_egress.out_gport = gport;
    l3_egress.allocation_flags = BCM_L3_INGRESS_ONLY;
    rv = sand_utils_l3_egress_create(unit, &l3_egress);
    if (rv != BCM_E_NONE)
    {
        printf("Error in creating native FEC, first hierarchy,  for routing into overlay\n");
        return rv;
    }

    cint_vpls_roo_advanced_info.v6_core_native_fec_id = l3_egress.fec_id;

    return rv;
}

int
vpls_roo_advanced_create_fec_entries_interop(
    int unit)
{
    int rv;
    int flags = 0;
    bcm_gport_t gport;
    int pointer_to_pwe;
    l3_egress_s l3_egress;

    /*
     * Create overlay FEC (second hierarchy) for routing into overlay:
     * 1) PWE gport converted to type l3_itf.
     * 2) Point to second hierarchy FEC (that points to MPLS tunnel).
     */

    gport = 0;
    sand_utils_l3_egress_s_init(unit,0,&l3_egress);
    BCM_GPORT_LOCAL_SET(gport, cint_vpls_roo_advanced_info.v6_core_port);
    /* Convert to type L3_ITF, according to APT's expectation */
    pointer_to_pwe = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(cint_vpls_roo_advanced_info.v6_core_overlay_mpls_port_id_eg);
    BCM_L3_ITF_SET(pointer_to_pwe,BCM_L3_ITF_TYPE_LIF,pointer_to_pwe);
    l3_egress.out_tunnel_or_rif = pointer_to_pwe;
    l3_egress.out_gport = gport;
    l3_egress.allocation_flags = BCM_L3_INGRESS_ONLY;
    l3_egress.l3_flags = BCM_L3_2ND_HIERARCHY;
    rv = sand_utils_l3_egress_create(unit, &l3_egress);
    if (rv != BCM_E_NONE)
    {
        printf("Error in creating overlay FEC, second hierarchy, for routing into overlay\n");
        return rv;
    }

    cint_vpls_roo_advanced_info.v6_core_overlay_fec_id = l3_egress.fec_id;

    /*
     * Create native FEC (first hierarchy) for routing into overlay:
     * 1) Native out rif.
     * 2) Native arp id.
     * 3) Point to overlay fec (second hierarchy).
     */
    gport = 0;
    sand_utils_l3_egress_s_init(unit,0,&l3_egress);
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_vpls_roo_advanced_info.v6_core_overlay_fec_id);
    l3_egress.out_tunnel_or_rif = cint_vpls_roo_advanced_info.v6_core_native_eth_rif;
    l3_egress.arp_encap_id = cint_vpls_roo_advanced_info.v6_core_native_arp_id;
    l3_egress.out_gport = gport;
    l3_egress.allocation_flags = BCM_L3_INGRESS_ONLY;
    rv = sand_utils_l3_egress_create(unit, &l3_egress);
    if (rv != BCM_E_NONE)
    {
        printf("Error in creating native FEC, first hierarchy,  for routing into overlay\n");
        return rv;
    }

    cint_vpls_roo_advanced_info.v6_core_native_fec_id = l3_egress.fec_id;

    return rv;
}




/* URPF  check on Tunnel to Native flow*/
int
vpls_roo_urpf_check(
        int unit)
{
    bcm_l3_ingress_t l3_ing_if;
    int rv;
    bcm_gport_t gport = 0;

    bcm_l3_ingress_t_init(&l3_ing_if);
    l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID | BCM_L3_INGRESS_GLOBAL_ROUTE;
    l3_ing_if.vrf = 0;
	l3_ing_if.urpf_mode = bcmL3IngressUrpfStrict;

    /* Enabling strict URPF on core_native_eth_rif*/
    rv = bcm_l3_ingress_create(unit, l3_ing_if, cint_vpls_roo_basic_info.core_native_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_l3_ingress_create for rpf check\n");
        return rv;
    }

    BCM_GPORT_LOCAL_SET(gport, cint_vpls_roo_basic_info.core_port);
    rv = l3__egress_only_fec__create(unit, cint_vpls_roo_advanced_info.rpf_fec_id, cint_vpls_roo_basic_info.core_native_eth_rif, 0/*encap_id_rpf_strict*/, gport, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only, fec_rpf_strict\n");
        return rv;
    }

    rv = add_route_ipv4(unit, cint_vpls_roo_advanced_info.sip ,0xffffffff,0, cint_vpls_roo_advanced_info.rpf_fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_route, sip, cint_vpls_roo_advanced_info.rpf_fec_id\n");
        return rv;
    }

    /** Config traps  */
    rv = l3_dnx_ip_rpf_config_traps(&unit,1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, l3_dnx_ip_rpf_config_traps\n");
        return rv;
    }

    return rv;
}

int
ipv6_routes_add(
        int unit)
{
    int rv;
    /*sip entry for rpf lookup*/
    rv = add_route_ipv6(unit,cint_vpls_roo_advanced_info.v6_sip,cint_vpls_roo_advanced_info.v6_mask, 0,cint_vpls_roo_advanced_info.rpf_fec_id );
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv6 for sip entry \n");
        return rv;
    }

    /*dip entry for fwd lookup*/
    rv = add_route_ipv6(unit,cint_vpls_roo_advanced_info.v6_dip,cint_vpls_roo_advanced_info.v6_mask, 0,cint_vpls_roo_advanced_info.v6_core_native_fec_id );
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv6 for dip entry \n");
        return rv;
    }

    return rv;
}


int
vpls_roo_advanced_main(
    int unit,
    int access_port,
    int core_port_1,
    int core_port_2)
{
    int rv;

    /* Assign v6 core port to struct*/
    cint_vpls_roo_advanced_info.v6_core_port = core_port_2;

    /*call roo basic function*/
    rv = vpls_roo_basic_main(unit,access_port,core_port_1);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_basic_main\n", rv);
        return rv;
    }

    /*configure v6 pwe port*/
    rv = vpls_roo_advanced_configure_port_properties(unit,cint_vpls_roo_advanced_info.v6_core_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_advanced_configure_port_properties\n", rv);
        return rv;
    }

    /*v6 core eth rif create*/
    rv = vpls_roo_advanced_create_eth_rifs(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_advanced_create_eth_rifs\n", rv);
        return rv;
    }

    /*v6 core l3 intf create*/
    rv = vpls_roo_advanced_create_l3_interfaces(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_advanced_create_l3_interfaces\n", rv);
        return rv;
    }

    /* Configure ARP entries */
    rv = vpls_roo_advanced_create_arp_entries(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_advanced_create_arp_entries\n", rv);
        return rv;
    }

     /* MPLS Tunnel label push */
    rv = vpls_roo_basic_create_mpls_tunnel(unit,
                                           cint_vpls_roo_advanced_info.v6_core_overlay_mpls_tunnel_label_1,
                                           cint_vpls_roo_advanced_info.v6_core_overlay_mpls_tunnel_label_2,
                                           1, cint_vpls_roo_advanced_info.v6_core_overlay_arp_id,
                                           &cint_vpls_roo_advanced_info.v6_core_overlay_mpls_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_basic_create_mpls_tunnel\n", rv);
        return rv;
    }

    /* PWE label encapsulation*/
    rv = vpls_roo_advanced_mpls_port_add_encapsulation(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_advanced_mpls_port_add_encapsulation\n", rv);
        return rv;
    }

    /*Add fec entries*/
    if(soc_property_get(unit, spn_SYSTEM_HEADERS_MODE, 1) == 0){
        rv = vpls_roo_advanced_create_fec_entries_interop(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in vpls_roo_advanced_create_fec_entries\n", rv);
            return rv;
        }
    } else {
        rv = vpls_roo_advanced_create_fec_entries(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in vpls_roo_advanced_create_fec_entries\n", rv);
            return rv;
        }
    }



    /* RPF check*/
    rv = vpls_roo_urpf_check(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_roo_basic_urpf_check\n", rv);
        return rv;
    }

    rv = ipv6_routes_add(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in ipv6_routes_add\n", rv);
        return rv;
    }


    return rv;

}

