/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_indexed_pwe_tagged_roo.c Purpose: test indexed VPLS with ROO
 */

/*
 * 
 * Configuration:
 * 
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_tpid.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_l3.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utility_mpls.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utility_vpls_advanced.c
 * cint ../../../../src/examples/dnx/vpls/cint_indexed_pwe_tagged_roo.c
 * cint
 * int unit = 0; 
 * int rv = 0; 
 * int in_port = 200; 
 * int out_port = 201;
 * rv = indexed_pwe_tagged_roo_main(unit,in_port,out_port);
 * print rv; 
 * 
 * 
 *  Scenario configured in this cint:
 * entity 0 configures Access -> Core flow
 * entity 1 configures Core -> Access flow
 * ###############################################################################################
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  1) ROO encapsulation
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Route into an IPv4 core.
 * Exit with a packet including an MPLS label, indexed PWE
 *
 *  Traffic:
 * 
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Sending packet from port == port1 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA         | SA              ||  SIP         ||  DIP         ||
 *   |    |00:0c:00:02:00:00|00:0c:00:02:00:01||127.255.255.02||127.255.255.03||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on port == port2:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-
 *   |    |      DA         | SA              ||   MPLS   ||   MPLS   ||      DA         | SA              ||OUTER-VID||OUTER-TPID|| VID  || TPID  ||  SIP         ||  DIP         ||
 *   |    |00:22:00:00:01:12|00:00:00:00:11:1D||Label:3333||Label:3456||00:11:00:00:01:12|00:00:00:00:CD:1D||  0x8100 ||   1000   ||0x81A8|| 2000  ||127.255.255.02||127.255.255.03||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *   2) ROO + PWE tagged termination
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 * MPLS, PWE labels and native ETH are terminated.
 * Lookup in routing table results in IP forwarding.
 *
 *  Traffic:
 * 
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Sending packet from port == port2 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-
 *   |    |      DA         | SA              ||   MPLS   ||   MPLS   ||      DA         | SA              ||OUTER-VID||OUTER-TPID|| VID  || TPID  ||  SIP         ||  DIP         ||
 *   |    |00:00:00:00:11:1D|00:22:00:00:01:12||Label:3333||Label:3456||00:00:00:00:CD:1D|00:11:00:00:01:12||  0x8100 ||   1000   ||0x81A8|| 2000  ||127.255.255.03||127.255.255.02||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on port == port1:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA         | SA              ||  SIP         ||  DIP         ||
 *   |    |00:0c:00:02:00:01|00:0c:00:02:00:00||127.255.255.03||127.255.255.02||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *
 * ###############################################################################################
*/

struct indexed_pwe_tagged_roo_l3_route_s
{
    int valid;
    bcm_vrf_t vrf;                  /* Virtual router instance. */
    bcm_ip_t ip_addr;                /* IP subnet address (IPv4). */
    bcm_ip_t ip_mask;               /* IP subnet mask (IPv4). */
    bcm_if_t *intf;                  /* L3 interface associated with route. */
};
int INDEXED_PWE_TAGGED_ROO_MAX_NOF_L3_ROUTE = 2;

indexed_pwe_tagged_roo_l3_route_s l3_routes[INDEXED_PWE_TAGGED_ROO_MAX_NOF_L3_ROUTE];


int indexed_pwe_tagged_roo_params_set = 0;

bcm_mac_t indexed_pwe_tagged_roo_access_sa = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x01 };
bcm_mac_t indexed_pwe_tagged_roo_access_da = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };
bcm_mac_t indexed_pwe_tagged_roo_core_inner_sa = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };
bcm_mac_t indexed_pwe_tagged_roo_core_inner_da = { 0x00, 0x11, 0x00, 0x00, 0x01, 0x12 };
bcm_mac_t indexed_pwe_tagged_roo_core_outer_sa = { 0x00, 0x00, 0x00, 0x00, 0x11, 0x1d };
bcm_mac_t indexed_pwe_tagged_roo_core_outer_da = { 0x00, 0x22, 0x00, 0x00, 0x01, 0x12 };

VPLS_UTIL_ADVANCED_MAX_NOF_ENTITIES = 2;
MPLS_UTIL_MAX_NOF_ARPS = 3;
MPLS_UTIL_MAX_NOF_FECS = 3;


void
indexed_pwe_tagged_roo_l3_route_s_init(
    indexed_pwe_tagged_roo_l3_route_s *l3_routes,
    int nof)
{
    int idx;

    for(idx = 0; idx < nof; idx++)
    {
        l3_routes[idx].valid = 0;
        l3_routes[idx].ip_addr = 0;
        l3_routes[idx].ip_mask = 0;
        l3_routes[idx].vrf = 0;
    }
}


int
indexed_pwe_tagged_roo_params_init(
    int unit,
    int port1,
    int port2)
{
    int rv = BCM_E_NONE;
    char* proc_name = "indexed_pwe_tagged_roo_params_init";
    int isj2c_q2a = 0;

    if(vpls_util_advanced_verbose)
    {
        printf("%s(): ENTER. port1 %d, port2 %d \r\n", proc_name, port1, port2);
    }

    vpls_util_advanced_vpn = 100;

    mpls_util_s_init(mpls_util_entity, MPLS_UTIL_MAX_NOF_ENTITIES);

    /*
     * l2, mac, arp, fec parameters
     */
    mpls_util_entity[0].ports[0].port = port1;
    mpls_util_entity[0].ports[0].eth_rif_id = &mpls_util_entity[0].rifs[0].intf;

    mpls_util_entity[1].ports[0].port = port2;
    mpls_util_entity[1].ports[0].eth_rif_id = &mpls_util_entity[1].rifs[1].intf;
    /***************************************/
    mpls_util_entity[0].rifs[0].intf = 30;
    mpls_util_entity[0].rifs[0].mac_addr = indexed_pwe_tagged_roo_access_da;
    mpls_util_entity[0].rifs[0].vrf = 100;
    mpls_util_entity[0].rifs[0].ingress_flags = BCM_L3_INGRESS_GLOBAL_ROUTE;

    mpls_util_entity[1].rifs[0].intf = 40;
    mpls_util_entity[1].rifs[0].mac_addr = indexed_pwe_tagged_roo_core_inner_da;
    mpls_util_entity[1].rifs[0].vrf = 100;
    mpls_util_entity[1].rifs[0].ingress_flags = BCM_L3_INGRESS_GLOBAL_ROUTE;
    /*ROO should update native rif qos model to uniform (default short_pipe)*/
    mpls_util_entity[1].rifs[0].ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
    mpls_util_entity[1].rifs[0].ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
    mpls_util_entity[1].rifs[0].ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;

    mpls_util_entity[1].rifs[1].intf = 50;
    mpls_util_entity[1].rifs[1].mac_addr = indexed_pwe_tagged_roo_core_outer_da;
    mpls_util_entity[1].rifs[1].vrf = 100;
    mpls_util_entity[1].rifs[1].ingress_flags = BCM_L3_INGRESS_GLOBAL_ROUTE;
    /***************************************/
    mpls_util_entity[0].arps[0].arp = 0;
    mpls_util_entity[0].arps[0].next_hop = indexed_pwe_tagged_roo_core_outer_sa;
    mpls_util_entity[0].arps[0].intf = &mpls_util_entity[1].rifs[1].intf;

    mpls_util_entity[0].arps[1].arp = 0;
    mpls_util_entity[0].arps[1].next_hop = indexed_pwe_tagged_roo_core_inner_sa;
    mpls_util_entity[0].arps[1].flags = BCM_L3_NATIVE_ENCAP;
    mpls_util_entity[0].arps[1].intf = &dummy_params[dum_idx++];

    mpls_util_entity[1].arps[0].arp = 0;
    mpls_util_entity[1].arps[0].next_hop = indexed_pwe_tagged_roo_access_sa;
    mpls_util_entity[1].arps[0].intf = &mpls_util_entity[0].rifs[1].intf;
    /***************************************/
    /* encapsulation entry */    
    mpls_util_entity[0].mpls_encap_tunnel[0].label[0] = 3333;
    mpls_util_entity[0].mpls_encap_tunnel[0].num_labels = 1;
    mpls_util_entity[0].mpls_encap_tunnel[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    mpls_util_entity[0].mpls_encap_tunnel[0].l3_intf_id = &mpls_util_entity[0].arps[0].arp;
    mpls_util_entity[0].mpls_encap_tunnel[0].encap_access = bcmEncapAccessTunnel3;

    /* termination entry */
    mpls_util_entity[1].mpls_switch_tunnel[0].action = BCM_MPLS_SWITCH_ACTION_POP;
    mpls_util_entity[1].mpls_switch_tunnel[0].label = mpls_util_entity[0].mpls_encap_tunnel[0].label[0];
    mpls_util_entity[1].mpls_switch_tunnel[0].vrf = 100;
    /***************************************/

    /* 
    * configure VPLS related params
    */
    vpls_util_advanced_entity_s_init(vpls_util_entity, VPLS_UTIL_ADVANCED_MAX_NOF_ENTITIES);
    /* PWE tunnel to be encapsulated parameters */
    vpls_util_entity[0].vpls_egress[0].flags |= BCM_MPLS_PORT_EGRESS_TUNNEL;
    vpls_util_entity[0].vpls_egress[0].flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;
    vpls_util_entity[0].vpls_egress[0].label = 3456;
    vpls_util_entity[0].vpls_egress[0].label_flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    vpls_util_entity[0].vpls_egress[0].encap_access = bcmEncapAccessTunnel2;
    vpls_util_entity[0].vpls_egress[0].mpls_port_id = &dummy_params[dum_idx++];
    vpls_util_entity[0].vpls_egress[0].encap_id = &dummy_params[dum_idx++];
    if (vpls_util_use_non_protected_vpls)
    {
        vpls_util_entity[0].vpls_egress[0].egress_tunnel_if = &mpls_util_entity[0].mpls_encap_tunnel[0].tunnel_id;
    }

    /* PWE tunnel to be terminated parameters */
    vpls_util_entity[1].vpls_ingress[0].criteria = BCM_MPLS_PORT_MATCH_LABEL;
    vpls_util_entity[1].vpls_ingress[0].encap_id = vpls_util_entity[0].vpls_egress[0].encap_id;
    vpls_util_entity[1].vpls_ingress[0].flags |= BCM_MPLS_PORT_EGRESS_TUNNEL | BCM_MPLS_PORT_SERVICE_TAGGED;
    vpls_util_entity[1].vpls_ingress[0].flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY;
    vpls_util_entity[1].vpls_ingress[0].match_label = vpls_util_entity[0].vpls_egress[0].label;
    vpls_util_entity[1].vpls_ingress[0].egress_tunnel_if = &mpls_util_entity[1].fecs[0].fec_id;
    vpls_util_entity[1].vpls_ingress[0].forwarding_domain = &mpls_util_entity[1].rifs[0].intf;
    vpls_util_entity[1].vpls_ingress[0].mpls_port_id = &dummy_params[dum_idx++];
    vpls_util_entity[1].vpls_ingress[0].encap_id = vpls_util_entity[0].vpls_egress[0].encap_id;
    vpls_util_entity[1].vpls_ingress[0].nof_service_tags = 2;

    vpls_util_entity[1].tpids[0].valid = 1;

    isj2c_q2a = is_device_or_above(unit, JERICHO2C) && !is_device_or_above(unit, JERICHO2P);
    vpls_util_entity[1].tpids[0].class_id = isj2c_q2a? 0x7e3 : 508;
    vpls_util_entity[1].tpids[0].port = vpls_util_entity[1].vpls_ingress[0].mpls_port_id;
    vpls_util_entity[1].tpids[0].tpid1 = 0x8100;
    vpls_util_entity[1].tpids[0].tpid2 = 0x81A8;
    vpls_util_entity[1].tpids[0].tag_format_class_id = 16;
    vpls_util_entity[1].tpids[0].flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    vpls_util_entity[1].tpids[0].vlan_translation_action_id = 0;

    /***************************************/
    /* configure IVE */
    vpls_util_entity[1].vlan_edits[0].valid = 1;

    vpls_util_entity[1].vlan_edits[0].vlan.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    vpls_util_entity[1].vlan_edits[0].vlan.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vpls_util_entity[1].vlan_edits[0].vlan.vsi = &mpls_util_entity[1].rifs[0].intf;
    vpls_util_entity[1].vlan_edits[0].vlan.vlan_port_id = &dummy_params[dum_idx++];

    vpls_util_entity[1].vlan_edits[0].port_match.match = BCM_PORT_MATCH_PORT_VLAN_STACKED;        /* Match 2 tags */
    vpls_util_entity[1].vlan_edits[0].port_match.port = vpls_util_entity[1].vpls_ingress[0].mpls_port_id;
    vpls_util_entity[1].vlan_edits[0].port_match.match_vlan = 1000;
    vpls_util_entity[1].vlan_edits[0].port_match.match_inner_vlan = 2000;
    vpls_util_entity[1].vlan_edits[0].port_match.flags = BCM_PORT_MATCH_INGRESS_ONLY | BCM_PORT_MATCH_NATIVE;

    vpls_util_entity[1].vlan_edits[0].ve_translate.lif = vpls_util_entity[1].vlan_edits[0].vlan.vlan_port_id;
    vpls_util_entity[1].vlan_edits[0].ve_translate.outer_tpid = 0;
    vpls_util_entity[1].vlan_edits[0].ve_translate.inner_tpid = 0;
    vpls_util_entity[1].vlan_edits[0].ve_translate.outer_action = bcmVlanActionDelete;
    vpls_util_entity[1].vlan_edits[0].ve_translate.inner_action = bcmVlanActionDelete;
    vpls_util_entity[1].vlan_edits[0].ve_translate.new_outer_vid = 0;
    vpls_util_entity[1].vlan_edits[0].ve_translate.new_inner_vid = 0;
    vpls_util_entity[1].vlan_edits[0].ve_translate.tag_format = vpls_util_entity[1].tpids[0].tag_format_class_id;
    vpls_util_entity[1].vlan_edits[0].ve_translate.vlan_edit_profile = 7;
    vpls_util_entity[1].vlan_edits[0].ve_translate.is_ive = 1;

    /* configure EVE */
    vpls_util_entity[0].vlan_edits[0].valid = 1;

    vpls_util_entity[0].vlan_edits[0].vlan.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
    vpls_util_entity[0].vlan_edits[0].vlan.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vpls_util_entity[0].vlan_edits[0].vlan.vsi = &dummy_params[dum_idx++];
    vpls_util_entity[0].vlan_edits[0].vlan.tunnel_id = vpls_util_entity[0].vpls_egress[0].mpls_port_id;
    vpls_util_entity[0].vlan_edits[0].vlan.vlan_port_id = &dummy_params[dum_idx++];

    vpls_util_entity[0].vlan_edits[0].ve_translate.lif = vpls_util_entity[0].vlan_edits[0].vlan.vlan_port_id;
    vpls_util_entity[0].vlan_edits[0].ve_translate.outer_tpid = vpls_util_entity[1].tpids[0].tpid1;
    vpls_util_entity[0].vlan_edits[0].ve_translate.inner_tpid = vpls_util_entity[1].tpids[0].tpid2;
    vpls_util_entity[0].vlan_edits[0].ve_translate.outer_action = bcmVlanActionReplace;
    vpls_util_entity[0].vlan_edits[0].ve_translate.inner_action = bcmVlanActionAdd;
    vpls_util_entity[0].vlan_edits[0].ve_translate.new_outer_vid = vpls_util_entity[1].vlan_edits[0].port_match.match_vlan;
    vpls_util_entity[0].vlan_edits[0].ve_translate.new_inner_vid = vpls_util_entity[1].vlan_edits[0].port_match.match_inner_vlan;
    vpls_util_entity[0].vlan_edits[0].ve_translate.tag_format = 0;
    vpls_util_entity[0].vlan_edits[0].ve_translate.vlan_edit_profile = 6;
    vpls_util_entity[0].vlan_edits[0].ve_translate.is_ive = 0;
    /***************************************/

    mpls_util_entity[0].skip_fecs = 1;
    mpls_util_entity[0].fecs[0].fec_id = 0;
    mpls_util_entity[0].fecs[0].port = &mpls_util_entity[1].ports[0].port;
    mpls_util_entity[0].fecs[0].tunnel_gport = &mpls_util_entity[0].mpls_encap_tunnel[0].tunnel_id;
    mpls_util_entity[0].fecs[0].tunnel_gport2 = &MPLS_UTIL_INVALID_GPORT;
    mpls_util_entity[0].fecs[0].flags = BCM_L3_3RD_HIERARCHY;

    mpls_util_entity[0].fecs[1].fec_id = 0;
    mpls_util_entity[0].fecs[1].tunnel_gport = vpls_util_entity[0].vlan_edits[0].vlan.vlan_port_id;
    mpls_util_entity[0].fecs[1].tunnel_gport2 = &MPLS_UTIL_INVALID_GPORT;
    mpls_util_entity[0].fecs[1].flags = BCM_L3_2ND_HIERARCHY;
    mpls_util_entity[0].fecs[1].fec = &mpls_util_entity[0].fecs[0].fec_id;

    mpls_util_entity[0].fecs[2].fec_id = 0;
    mpls_util_entity[0].fecs[2].tunnel_gport = &mpls_util_entity[1].rifs[0].intf;
    mpls_util_entity[0].fecs[2].tunnel_gport2 = &mpls_util_entity[0].arps[1].arp;
    mpls_util_entity[0].fecs[2].fec = &mpls_util_entity[0].fecs[1].fec_id;

    mpls_util_entity[1].skip_fecs = 1;
    mpls_util_entity[1].fecs[0].fec_id = 0;
    mpls_util_entity[1].fecs[0].port = &mpls_util_entity[0].ports[0].port;
    mpls_util_entity[1].fecs[0].tunnel_gport = &mpls_util_entity[0].rifs[0].intf;
    mpls_util_entity[1].fecs[0].tunnel_gport2 = &mpls_util_entity[1].arps[0].arp;

    indexed_pwe_tagged_roo_l3_route_s_init(l3_routes, INDEXED_PWE_TAGGED_ROO_MAX_NOF_L3_ROUTE);

    l3_routes[0].valid = 1;
    l3_routes[0].ip_addr = 0x7fffff02; /* 127.255.255.02 */
    l3_routes[0].ip_mask = 0xffffffff;
    l3_routes[0].vrf = 0;
    l3_routes[0].intf = &mpls_util_entity[1].fecs[0].fec_id;

    l3_routes[1].valid = 1;
    l3_routes[1].ip_addr = 0x7fffff03; /* 127.255.255.03 */
    l3_routes[1].ip_mask = 0xffffffff;
    l3_routes[1].vrf = 0;
    l3_routes[1].intf = &mpls_util_entity[0].fecs[2].fec_id;

    mpls_params_set = 1;
    vpls_util_advanced_params_set = 1;
    indexed_pwe_tagged_roo_params_set = 1;

    if(vpls_util_advanced_verbose)
    {
        printf("%s(): EXIT. port1 %d, port2 %d \r\n", proc_name, port1, port2);
    }

    return rv;
}

int
indexed_pwe_tagged_roo_l3_forwarding(
    int unit,
    indexed_pwe_tagged_roo_l3_route_s *l3_routes,
    int nof_l3)
{
    int rv = BCM_E_NONE;
    int idx_l3;
    bcm_l3_route_t l3rt;
    char *proc_name = "indexed_pwe_tagged_roo_l3_forwarding";

    for(idx_l3 = 0; idx_l3 < nof_l3; idx_l3++)
    {
        if(l3_routes[idx_l3].valid)
        {
            bcm_l3_route_t_init(l3rt);
        
            l3rt.l3a_subnet = l3_routes[idx_l3].ip_addr;
            l3rt.l3a_ip_mask = l3_routes[idx_l3].ip_mask;
            l3rt.l3a_vrf = l3_routes[idx_l3].vrf;
            if(l3_routes[idx_l3].intf)
            {
                l3rt.l3a_intf = *l3_routes[idx_l3].intf;
            }

            rv = bcm_l3_route_add(unit, l3rt);
            if (rv != BCM_E_NONE)
            {
                printf("bcm_l3_route_add failed: %x \n", rv);
            }

            if (vpls_util_advanced_verbose)
            {
                printf("%s(): idx_l3 = %d, Addr = ", proc_name, idx_l3);
                _l3_print_ip_addr(l3rt.l3a_subnet);
                printf(", mask = 0x%x, vrf = %d, intf = 0x%x, flags = 0x%x, flags2 = 0x%x\n", l3rt.l3a_ip_mask, l3rt.l3a_vrf, l3rt.l3a_intf, l3rt.l3a_flags, l3rt.l3a_flags2);
            }
        }
    }

    return rv;
}

int
indexed_pwe_tagged_roo_main(
    int unit,
    int port1,
    int port2)
{
    int rv = BCM_E_NONE;
    char *proc_name = "indexed_pwe_tagged_roo_main";
    int i;

    if(!indexed_pwe_tagged_roo_params_set)
    {
        rv = indexed_pwe_tagged_roo_params_init(unit, port1, port2);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in indexed_pwe_tagged_roo_params_init\n", rv);
            return rv;
        }
    }

    /*
     * Delete all global tpids 
     * Note: This will delete all the default settings (see appl_dnx_vlan_init). 
     */
    rv = tpid__tpids_clear_all(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, tpid__tpids_clear_all, rv = %\n", rv);
        return rv;
    }

    rv = vpls_util_advanced_main(unit, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_util_advanced_main\n", rv);
        return rv;
    }

    if (vpls_util_advanced_verbose)
    {
        printf("%s(): ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", proc_name);
    }

    /** Configure fec entries for forwarding flows */
    for (i = 0; i < MPLS_UTIL_MAX_NOF_ENTITIES; i++)
    {
        if (vpls_util_advanced_verbose >= 1)
        {    
            printf("entity %d: Configure fec entries for forwarding flows\n", i);
        }
        rv = mpls_create_fec_entries(unit, mpls_util_entity[i].fecs, MPLS_UTIL_MAX_NOF_FECS);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in mpls_create_fec_entries\n");
            return rv;
        }
    }

    if (vpls_util_advanced_verbose)
    {
        printf("%s(): ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", proc_name);
    }

    /** Configure l3 forwarding entries */
    if (vpls_util_advanced_verbose)
    {
        printf("%s(): Configure l3 forwarding entries\n", proc_name);
    }
    rv = indexed_pwe_tagged_roo_l3_forwarding(unit, l3_routes, INDEXED_PWE_TAGGED_ROO_MAX_NOF_L3_ROUTE);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in indexed_pwe_tagged_roo_l3_forwarding\n");
        return rv;
    }

    return rv;
}
