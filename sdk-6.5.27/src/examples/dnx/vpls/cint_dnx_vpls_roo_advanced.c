/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved. File: cint_vpls_roo_advanced.c Purpose: adding advanced scenarios of vpls roo
 */

/*
 *
 * Configuration:
 *
 * Test Scenario - start
 * cint;
 * cint_reset();
 * exit;
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_basic.c
  cint ../../../../src/examples/dnx/l3/cint_dnx_ip_route_rpf_basic.c
  cint ../../../../src/examples/dnx/vpls/cint_dnx_vpls_roo_basic.c
  cint ../../../../src/examples/dnx/vpls/cint_dnx_vpls_roo_advanced.c
  cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vlan_translate.c
  cint
  int unit = 0;
  int rv = 0;
  int access_port = 200;
  int core_port = 201;
  rv = vpls_roo_basic_main(unit,access_port,core_port);
  print rv;
 * Test Scenario - end
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
 **
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
    char error_msg[200]={0,};

    /*
     * set class for v6 core port
     */
    snprintf(error_msg, sizeof(error_msg), "port=%d", core_port);
    BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, core_port, bcmPortClassId, core_port), error_msg);
    return BCM_E_NONE;
}

int
vpls_roo_advanced_create_eth_rifs(
    int unit)
{
    char error_msg[200]={0,};
    /*
     * create vlan based on the vsi (vpn)
     */
    snprintf(error_msg, sizeof(error_msg), "Failed to create VLAN %d", cint_vpls_roo_advanced_info.v6_core_native_eth_rif);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, cint_vpls_roo_advanced_info.v6_core_native_eth_rif), error_msg);

    /*
     * create vlan based on the vsi (vpn)
     */
    snprintf(error_msg, sizeof(error_msg), "Failed to create VLAN %d", cint_vpls_roo_advanced_info.v6_core_overlay_eth_rif);
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_create(unit, cint_vpls_roo_advanced_info.v6_core_overlay_eth_rif), error_msg);

    return BCM_E_NONE;
}

int
vpls_roo_advanced_create_l3_interfaces(
    int unit)
{


    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_vpls_roo_advanced_info.v6_core_native_eth_rif,
                             cint_vpls_roo_advanced_info.v6_core_native_eth_rif_mac), "pwe_intf");

    BCM_IF_ERROR_RETURN_MSG(intf_eth_rif_create(unit, cint_vpls_roo_advanced_info.v6_core_overlay_eth_rif,
                             cint_vpls_roo_advanced_info.v6_core_overlay_eth_rif_mac), "pwe_intf");
    return BCM_E_NONE;

}

int
vpls_roo_advanced_create_arp_entries(
    int unit)
{

    /*
     * Configure native ARP entry
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create_inner(unit, 0, &(cint_vpls_roo_advanced_info.v6_core_native_arp_id),
                                       cint_vpls_roo_advanced_info.v6_core_native_next_hop_mac,
                                       cint_vpls_roo_advanced_info.v6_core_native_eth_rif,
                                       BCM_L3_NATIVE_ENCAP,
                                       BCM_L3_FLAGS2_VLAN_TRANSLATION), "create AC egress object ARP only");

    /*
     * Configure overlay ARP entry
     */
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_encap__create(unit, 0, &(cint_vpls_roo_advanced_info.v6_core_overlay_arp_id),
                                       cint_vpls_roo_advanced_info.v6_core_overlay_next_hop_mac,
                                       cint_vpls_roo_advanced_info.v6_core_overlay_eth_rif), "create AC egress object ARP only");
    return BCM_E_NONE;
}

int
vpls_roo_advanced_mpls_port_add_encapsulation(
    int unit)
{
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;
    uint32 system_headers_mode = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_mode", NULL);
    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;
    mpls_port.flags |= BCM_MPLS_PORT_EGRESS_TUNNEL;

    mpls_port.egress_label.label = cint_vpls_roo_advanced_info.v6_core_overlay_pwe_label;
    mpls_port.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    if (system_headers_mode == 0)
    {
        /** jericho system header mode */
        mpls_port.egress_tunnel_if =cint_vpls_roo_advanced_info.v6_core_overlay_mpls_tunnel_id;
    }
    BCM_IF_ERROR_RETURN_MSG(bcm_mpls_port_add(unit, 0, mpls_port), "encapsulation");

    cint_vpls_roo_advanced_info.v6_core_overlay_mpls_port_id_eg = mpls_port.mpls_port_id;
    cint_vpls_roo_advanced_info.v6_core_overlay_pwe_encap_id = mpls_port.encap_id;

    return BCM_E_NONE;
}
int
vpls_roo_advanced_create_fec_entries(
    int unit)
{
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
    dnx_utils_l3_egress_s_init(unit,0,&l3_egress);
    BCM_GPORT_LOCAL_SET(gport, cint_vpls_roo_advanced_info.v6_core_port);
    l3_egress.l3_flags = BCM_L3_3RD_HIERARCHY;
    l3_egress.allocation_flags = BCM_L3_INGRESS_ONLY;
    l3_egress.out_gport = gport;
    l3_egress.out_tunnel_or_rif = cint_vpls_roo_advanced_info.v6_core_overlay_mpls_tunnel_id;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        l3_egress.l3_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_egress_create(unit, &l3_egress),
        "in creating overlay FEC, 3rd hierarchy, for routing into overlay");

    cint_vpls_roo_advanced_info.v6_core_overlay_fec_id2 = l3_egress.fec_id;

    /*
     * Create overlay FEC (second hierarchy) for routing into overlay:
     * 1) PWE gport converted to type l3_itf.
     * 2) Point to third hierarchy FEC (that points to MPLS tunnel).
     */

    gport = 0;
    dnx_utils_l3_egress_s_init(unit,0,&l3_egress);
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_vpls_roo_advanced_info.v6_core_overlay_fec_id2);
    /* Convert to type L3_ITF, according to APT's expectation */
    pointer_to_pwe = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(cint_vpls_roo_advanced_info.v6_core_overlay_mpls_port_id_eg);
    BCM_L3_ITF_SET(pointer_to_pwe,BCM_L3_ITF_TYPE_LIF,pointer_to_pwe);
    l3_egress.out_tunnel_or_rif = pointer_to_pwe;
    l3_egress.out_gport = gport;
    l3_egress.allocation_flags = BCM_L3_INGRESS_ONLY;
    l3_egress.l3_flags = BCM_L3_2ND_HIERARCHY;
    if (*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        l3_egress.l3_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_egress_create(unit, &l3_egress),
        "in creating overlay FEC, second hierarchy, for routing into overlay");

    cint_vpls_roo_advanced_info.v6_core_overlay_fec_id = l3_egress.fec_id;

    /*
     * Create native FEC (first hierarchy) for routing into overlay:
     * 1) Native out rif.
     * 2) Native arp id.
     * 3) Point to overlay fec (second hierarchy).
     */
    gport = 0;
    dnx_utils_l3_egress_s_init(unit,0,&l3_egress);
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_vpls_roo_advanced_info.v6_core_overlay_fec_id);
    l3_egress.out_tunnel_or_rif = cint_vpls_roo_advanced_info.v6_core_native_eth_rif;
    l3_egress.arp_encap_id = cint_vpls_roo_advanced_info.v6_core_native_arp_id;
    l3_egress.out_gport = gport;
    l3_egress.allocation_flags = BCM_L3_INGRESS_ONLY;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        l3_egress.l3_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_egress_create(unit, &l3_egress),
        "in creating native FEC, first hierarchy,  for routing into overlay");

    cint_vpls_roo_advanced_info.v6_core_native_fec_id = l3_egress.fec_id;

    return BCM_E_NONE;
}

int
vpls_roo_advanced_create_fec_entries_interop(
    int unit)
{
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
    dnx_utils_l3_egress_s_init(unit,0,&l3_egress);
    BCM_GPORT_LOCAL_SET(gport, cint_vpls_roo_advanced_info.v6_core_port);
    /* Convert to type L3_ITF, according to APT's expectation */
    pointer_to_pwe = BCM_GPORT_SUB_TYPE_LIF_VAL_GET(cint_vpls_roo_advanced_info.v6_core_overlay_mpls_port_id_eg);
    BCM_L3_ITF_SET(pointer_to_pwe,BCM_L3_ITF_TYPE_LIF,pointer_to_pwe);
    l3_egress.out_tunnel_or_rif = pointer_to_pwe;
    l3_egress.out_gport = gport;
    l3_egress.allocation_flags = BCM_L3_INGRESS_ONLY;
    l3_egress.l3_flags = BCM_L3_2ND_HIERARCHY;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        l3_egress.l3_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_egress_create(unit, &l3_egress),
        "in creating overlay FEC, second hierarchy, for routing into overlay");

    cint_vpls_roo_advanced_info.v6_core_overlay_fec_id = l3_egress.fec_id;

    /*
     * Create native FEC (first hierarchy) for routing into overlay:
     * 1) Native out rif.
     * 2) Native arp id.
     * 3) Point to overlay fec (second hierarchy).
     */
    gport = 0;
    dnx_utils_l3_egress_s_init(unit,0,&l3_egress);
    BCM_GPORT_FORWARD_PORT_SET(gport, cint_vpls_roo_advanced_info.v6_core_overlay_fec_id);
    l3_egress.out_tunnel_or_rif = cint_vpls_roo_advanced_info.v6_core_native_eth_rif;
    l3_egress.arp_encap_id = cint_vpls_roo_advanced_info.v6_core_native_arp_id;
    l3_egress.out_gport = gport;
    l3_egress.allocation_flags = BCM_L3_INGRESS_ONLY;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        l3_egress.l3_flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    BCM_IF_ERROR_RETURN_MSG(dnx_utils_l3_egress_create(unit, &l3_egress),
        "in creating native FEC, first hierarchy,  for routing into overlay");

    cint_vpls_roo_advanced_info.v6_core_native_fec_id = l3_egress.fec_id;

    return BCM_E_NONE;
}




/* URPF  check on Tunnel to Native flow*/
int
vpls_roo_urpf_check(
        int unit)
{
    bcm_l3_ingress_t l3_ing_if;
    bcm_gport_t gport = 0;

    bcm_l3_ingress_t_init(&l3_ing_if);
    l3_ing_if.flags = BCM_L3_INGRESS_WITH_ID;
    if(*dnxc_data_get(unit, "l3", "feature", "public_routing_support", NULL))
    {
        l3_ing_if.flags |= BCM_L3_INGRESS_GLOBAL_ROUTE;
        l3_ing_if.vrf = 0;
    }
    else
    {
        l3_ing_if.vrf = 0;
    }
    l3_ing_if.urpf_mode = bcmL3IngressUrpfStrict;

    /* Enabling strict URPF on core_native_eth_rif*/
    BCM_IF_ERROR_RETURN_MSG(bcm_l3_ingress_create(unit, l3_ing_if, cint_vpls_roo_basic_info.core_native_eth_rif),
        "for rpf check");

    BCM_GPORT_LOCAL_SET(gport, cint_vpls_roo_basic_info.core_port);
    int fec_flags2 = 0;
    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        fec_flags2 |= BCM_L3_FLAGS2_RPF_ONLY;
    }
    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 1, &cint_vpls_roo_advanced_info.rpf_fec_id), "");
    cint_vpls_roo_advanced_info.rpf_fec_id += 0x500;
    BCM_IF_ERROR_RETURN_MSG(l3__egress_only_fec__create(unit, cint_vpls_roo_advanced_info.rpf_fec_id, cint_vpls_roo_basic_info.core_native_eth_rif, 0/*encap_id_rpf_strict*/, gport, 0, fec_flags2),
        "create egress object FEC only, fec_rpf_strict");

    BCM_IF_ERROR_RETURN_MSG(add_route_ipv4_raw_rpf(unit, cint_vpls_roo_advanced_info.sip ,0xffffffff, l3_ing_if.vrf, cint_vpls_roo_advanced_info.rpf_fec_id),
        "sip, cint_vpls_roo_advanced_info.rpf_fec_id");

    /** Config traps  */
    BCM_IF_ERROR_RETURN_MSG(l3_dnx_ip_rpf_config_traps(&unit,1), "");

    return BCM_E_NONE;
}

int
ipv6_routes_add(
        int unit)
{
    /*sip entry for rpf lookup*/
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6_rpf(unit,cint_vpls_roo_advanced_info.v6_sip,cint_vpls_roo_advanced_info.v6_mask, 0, cint_vpls_roo_advanced_info.rpf_fec_id),
        "for sip entry");

    /*dip entry for fwd lookup*/
    BCM_IF_ERROR_RETURN_MSG(add_route_ipv6(unit,cint_vpls_roo_advanced_info.v6_dip,cint_vpls_roo_advanced_info.v6_mask, 0, cint_vpls_roo_advanced_info.v6_core_native_fec_id),
        "for dip entry");

    return BCM_E_NONE;
}


int
vpls_roo_advanced_main(
    int unit,
    int access_port,
    int core_port_1,
    int core_port_2)
{
    uint32 system_headers_mode = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_mode", NULL);

    /* Assign v6 core port to struct*/
    cint_vpls_roo_advanced_info.v6_core_port = core_port_2;

    /*call roo basic function*/
    BCM_IF_ERROR_RETURN_MSG(vpls_roo_basic_main(unit,access_port,core_port_1), "");

    /*configure v6 pwe port*/
    BCM_IF_ERROR_RETURN_MSG(vpls_roo_advanced_configure_port_properties(unit,cint_vpls_roo_advanced_info.v6_core_port), "");

    /*v6 core eth rif create*/
    BCM_IF_ERROR_RETURN_MSG(vpls_roo_advanced_create_eth_rifs(unit), "");

    /*v6 core l3 intf create*/
    BCM_IF_ERROR_RETURN_MSG(vpls_roo_advanced_create_l3_interfaces(unit), "");

    /* Configure ARP entries */
    BCM_IF_ERROR_RETURN_MSG(vpls_roo_advanced_create_arp_entries(unit), "");

     /* MPLS Tunnel label push */
    BCM_IF_ERROR_RETURN_MSG(vpls_roo_basic_create_mpls_tunnel(unit,
                                           cint_vpls_roo_advanced_info.v6_core_overlay_mpls_tunnel_label_1,
                                           cint_vpls_roo_advanced_info.v6_core_overlay_mpls_tunnel_label_2,
                                           1, cint_vpls_roo_advanced_info.v6_core_overlay_arp_id,
                                           &cint_vpls_roo_advanced_info.v6_core_overlay_mpls_tunnel_id), "");

    /* PWE label encapsulation*/
    BCM_IF_ERROR_RETURN_MSG(vpls_roo_advanced_mpls_port_add_encapsulation(unit), "");

    /*Add fec entries*/
    if (system_headers_mode == 0)
    {
        /** jericho system header mode */
        BCM_IF_ERROR_RETURN_MSG(vpls_roo_advanced_create_fec_entries_interop(unit), "");
    } else {
        BCM_IF_ERROR_RETURN_MSG(vpls_roo_advanced_create_fec_entries(unit), "");
    }

    /* RPF check*/
    BCM_IF_ERROR_RETURN_MSG(vpls_roo_urpf_check(unit), "");

    BCM_IF_ERROR_RETURN_MSG(ipv6_routes_add(unit), "");


    return BCM_E_NONE;

}

