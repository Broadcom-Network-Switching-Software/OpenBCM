/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/*
 * Purpose:
 * Example for Native Egress Vlan Translation with VPLS applications. The native AC may come
 * From EEDB or ESEM depending on the application and configurations.
 * For detailed VPLS config, refer to cint_vpls_pwe_tagged_mode_basic
 * this is a basic config of VPLS from access to core side, plus native EVE config
 *   vpls_pwe_tagged_mode_neve_esem -> vlan+port lookup in ESEM and get VLAN edit result
 *   vpls_pwe_tagged_mode_neve_eedb -> look up in EEDB and get VLAN edit result
 *   vpls_pwe_tagged_mode_basic_default_neve -> default native EVE configurations for the specific PWE.
 *   vpls_pwe_tagged_mode_neve_esem_match_name_space -> example for native AC per PWE.namespace.
 *                                                      Many PWEs can share the same namespace per the application.
 *
 * Configuration:
 * Test Scenario - start
 * cint;
 * cint_reset();
 * exit;
  cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
  cint ../../../../src/examples/sand/cint_ip_route_basic.c
  cint ../../../../src/examples/dnx/vpls/cint_vpls_pwe_tagged_mode_basic.c
  cint ../../../../src/examples/sand/utility/cint_sand_utils_vlan_translate.c
  cint ../../../../src/examples/dnx/vpls/cint_vpls_native_eve_test.c
  cint
  int unit = 0; 
  int rv = 0; 
  int in_port = 200; 
  int out_port = 201;
  rv = vpls_pwe_tagged_mode_neve_esem(unit,in_port,out_port);
  rv = vpls_pwe_tagged_mode_neve_eedb(unit,in_port,out_port);
 * Test Scenario - end
 *
 * Traffic:
 * Incoming packet - IPv4oETH1
 *   DA: 00:0C:00:02:0C:44
 *   SA: 00:0C:00:02:0C:33
 *   VLAN -
 *     TPID: 0x9100
 *     VID : 10
 * Outgoing packet - IPv4oETHxoMPLSoMPLSoMPLSoETH1
 *   x may be 0,1,2 depends on the tests and configurations.
 *
 */

/* Globals */
int native_eve_action_id = 2;
int native_eve_untag = 0;
int native_eve_stag = 4;
int native_eve_dtag = 16;
int default_native_out_ac = -1;

int
vpls_pwe_tagged_mode_neve_esem(
    int unit,
    int access_port,
    int core_port)
{
    int nof_labels = 2;
    int dummy_encap_id;

    vpls_pwe_tagged_mode_basic_init(access_port, core_port);

    /*
     * Configure AC and PWE ports
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_configure_port_properties(unit, cint_vpls_pwe_tagged_mode_basic_info.access_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif, 0), "");

    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_create_vsis(unit, cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif), "");

    /*
     * Configure L3 interfaces 
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_create_l3_interfaces(unit, cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif,
                                                         cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif_mac), "");

    /*
     * Configure an ARP entries
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_create_arp_entry(unit, &(cint_vpls_pwe_tagged_mode_basic_info.core_arp_id),
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_next_hop_mac,
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif), "");

    /*
     * Configure a push entry.
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_create_mpls_tunnel(unit,
                                                       cint_vpls_pwe_tagged_mode_basic_info.service_mpls_tunnel_label_1,
                                                       cint_vpls_pwe_tagged_mode_basic_info.service_mpls_tunnel_label_2,
                                                       2, cint_vpls_pwe_tagged_mode_basic_info.core_arp_id,
                                                       &cint_vpls_pwe_tagged_mode_basic_info.mpls_tunnel_id,
                                                       1), "");

    /*
     * Configure fec entry 
     */
    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id), "");
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_create_fec_entry(unit, cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id,
                                                     cint_vpls_pwe_tagged_mode_basic_info.mpls_tunnel_id,
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_port), "");

    /*
     * configure PWE tunnel - egress flow 
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_mpls_port_add_encapsulation(unit,
                                                                &(cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_eg),
                                                                cint_vpls_pwe_tagged_mode_basic_info.service_pwe_label,
                                                                &(cint_vpls_pwe_tagged_mode_basic_info.pwe_encap_id),
                                                                1), "");

    /** define ac and pwe ports */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_vswitch_add_core_outer_ac(unit, cint_vpls_pwe_tagged_mode_basic_info.core_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif,
                                                              &(cint_vpls_pwe_tagged_mode_basic_info.outer_ac_vlan_port_id)), "");

    /*
     * add l2 addresses to be defined as static - no learning needed
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_l2_addr_add(unit, cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                cint_vpls_pwe_tagged_mode_basic_info.service_mac_2,
                                                cint_vpls_pwe_tagged_mode_basic_info.pwe_encap_id,
                                                cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id), "");

    /*
     * Set tag formats
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_tag_formats_neve(unit), "");

    /*
     * Add AC for inner ETH EVE (adding service tags)
     * For Egress flow
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_add_core_native_ac(unit, cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                 cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_eg,
                                                 &(cint_vpls_pwe_tagged_mode_basic_info.egress_native_ac_vlan_port_id),
                                                 1, &dummy_encap_id), "");

    bcm_vlan_action_set_t action;
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_port_translation_t port_trans;

    /*
     * set edit profile for ingress/egress LIF 
     */
    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.new_outer_vlan = cint_vpls_pwe_tagged_mode_basic_info.egress_canonical_outer_vlan;
    port_trans.new_inner_vlan = cint_vpls_pwe_tagged_mode_basic_info.egress_canonical_inner_vlan;
    port_trans.gport = cint_vpls_pwe_tagged_mode_basic_info.egress_native_ac_vlan_port_id;
    port_trans.vlan_edit_class_id = cint_vpls_pwe_tagged_mode_basic_info.egress_vlan_edit_profile;
    port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_translation_set(unit, &port_trans), "");

    /*
     * Create action ID
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_WITH_ID|BCM_VLAN_ACTION_SET_EGRESS,&native_eve_action_id), "");

    /*
     * Set translation action class
     */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = cint_vpls_pwe_tagged_mode_basic_info.egress_vlan_edit_profile;
    action_class.tag_format_class_id = native_eve_untag;
    action_class.vlan_translation_action_id = native_eve_action_id;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_class_set(unit, &action_class), "");

    action_class.tag_format_class_id = native_eve_stag;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_class_set(unit, &action_class), "");

    action_class.tag_format_class_id = native_eve_dtag;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_class_set(unit, &action_class), "");

    return BCM_E_NONE;
}

int
vpls_pwe_tagged_mode_neve_eedb(
    int unit,
    int access_port,
    int core_port)
{
    int nof_labels = 2;
    int dummy_encap_id;

    vpls_pwe_tagged_mode_basic_init(access_port, core_port);

    /*
     * Configure AC and PWE ports
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_configure_port_properties(unit, cint_vpls_pwe_tagged_mode_basic_info.access_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif, 0), "");

    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_create_vsis(unit, cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif), "");

    /*
     * Configure L3 interfaces 
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_create_l3_interfaces(unit, cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif,
                                                         cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif_mac), "");

    /*
     * Configure an ARP entries
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_create_arp_entry(unit, &(cint_vpls_pwe_tagged_mode_basic_info.core_arp_id),
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_next_hop_mac,
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif), "");

    /*
     * Configure a push entry.
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_create_mpls_tunnel(unit,
                                                       cint_vpls_pwe_tagged_mode_basic_info.service_mpls_tunnel_label_1,
                                                       cint_vpls_pwe_tagged_mode_basic_info.service_mpls_tunnel_label_2,
                                                       2, cint_vpls_pwe_tagged_mode_basic_info.core_arp_id,
                                                       &cint_vpls_pwe_tagged_mode_basic_info.mpls_tunnel_id,
                                                       0), "");

    /*
     * Configure fec entry 
     */
    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id), "");
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_create_fec_entry(unit, cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id,
                                                     cint_vpls_pwe_tagged_mode_basic_info.mpls_tunnel_id,
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_port), "");

    /*
     * configure PWE tunnel - egress flow 
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_mpls_port_add_encapsulation(unit,
                                                                &(cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_eg),
                                                                cint_vpls_pwe_tagged_mode_basic_info.service_pwe_label,
                                                                &(cint_vpls_pwe_tagged_mode_basic_info.pwe_encap_id),
                                                                0), "");

    /** define ac and pwe ports */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_vswitch_add_core_outer_ac(unit, cint_vpls_pwe_tagged_mode_basic_info.core_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif,
                                                              &(cint_vpls_pwe_tagged_mode_basic_info.outer_ac_vlan_port_id)), "");

    /*
     * Set tag formats
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_tag_formats_neve(unit), "");

    /*
     * Add AC for inner ETH EVE (adding service tags)
     * For Egress flow
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_add_core_native_ac(unit, 0,
                                                 cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_eg,
                                                 &(cint_vpls_pwe_tagged_mode_basic_info.egress_native_ac_vlan_port_id),
                                                 0, &(cint_vpls_pwe_tagged_mode_basic_info.indexed_ac_encap_id)), "");

    bcm_vlan_action_set_t action;
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_port_translation_t port_trans;

    /*
     * set edit profile for ingress/egress LIF 
     */
    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.new_outer_vlan = cint_vpls_pwe_tagged_mode_basic_info.egress_canonical_outer_vlan;
    port_trans.new_inner_vlan = cint_vpls_pwe_tagged_mode_basic_info.egress_canonical_inner_vlan;
    port_trans.gport = cint_vpls_pwe_tagged_mode_basic_info.egress_native_ac_vlan_port_id;
    port_trans.vlan_edit_class_id = cint_vpls_pwe_tagged_mode_basic_info.egress_vlan_edit_profile;
    port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_translation_set(unit, &port_trans), "");

    /*
     * Create action ID
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_WITH_ID|BCM_VLAN_ACTION_SET_EGRESS,&native_eve_action_id), "");

    /*
     * Set translation action class
     */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = cint_vpls_pwe_tagged_mode_basic_info.egress_vlan_edit_profile;
    action_class.tag_format_class_id = native_eve_untag;
    action_class.vlan_translation_action_id = native_eve_action_id;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_class_set(unit, &action_class), "");

    action_class.tag_format_class_id = native_eve_stag;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_class_set(unit, &action_class), "");

    action_class.tag_format_class_id = native_eve_dtag;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_class_set(unit, &action_class), "");

    /*
     * add l2 addresses to be defined as static - no learning needed
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_l2_addr_add(unit, cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                cint_vpls_pwe_tagged_mode_basic_info.service_mac_2,
                                                cint_vpls_pwe_tagged_mode_basic_info.indexed_ac_encap_id,
                                                cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id), "");

    return BCM_E_NONE;
}

int
vpls_pwe_tagged_mode_neve_base(
    int unit,
    int access_port,
    int core_port)
{
    int nof_labels = 2;
    int dummy_encap_id;

    vpls_pwe_tagged_mode_basic_init(access_port, core_port);

    /*
     * Configure AC and PWE ports
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_configure_port_properties(unit, cint_vpls_pwe_tagged_mode_basic_info.access_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif, 0), "");
    /*
     * Set In-Port to In ETh-RIF
     */
    BCM_IF_ERROR_RETURN_MSG(in_port_intf_set(unit, cint_vpls_pwe_tagged_mode_basic_info.access_port, cint_vpls_pwe_tagged_mode_basic_info.service_vsi), "intf_in");

    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_create_vsis(unit, cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif), "");

    /*
     * Configure L3 interfaces 
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_create_l3_interfaces(unit, cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif,
                                                         cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif_mac), "");

    /*
     * Configure an ARP entries
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_create_arp_entry(unit, &(cint_vpls_pwe_tagged_mode_basic_info.core_arp_id),
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_next_hop_mac,
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif), "");

    /*
     * Configure a push entry.
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_create_mpls_tunnel(unit,
                                                       cint_vpls_pwe_tagged_mode_basic_info.service_mpls_tunnel_label_1,
                                                       cint_vpls_pwe_tagged_mode_basic_info.service_mpls_tunnel_label_2,
                                                       2, cint_vpls_pwe_tagged_mode_basic_info.core_arp_id,
                                                       &cint_vpls_pwe_tagged_mode_basic_info.mpls_tunnel_id,
                                                       1), "");

    /*
     * Configure fec entry 
     */
    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id), "");

    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_create_fec_entry(unit, cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id,
                                                     cint_vpls_pwe_tagged_mode_basic_info.mpls_tunnel_id,
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_port), "");

    /*
     * configure PWE tunnel - egress flow 
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_mpls_port_add_encapsulation(unit,
                                                                &(cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_eg),
                                                                cint_vpls_pwe_tagged_mode_basic_info.service_pwe_label,
                                                                &(cint_vpls_pwe_tagged_mode_basic_info.pwe_encap_id),
                                                                1), "");

    /** define ac and pwe ports */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_vswitch_add_core_outer_ac(unit, cint_vpls_pwe_tagged_mode_basic_info.core_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif,
                                                              &(cint_vpls_pwe_tagged_mode_basic_info.outer_ac_vlan_port_id)), "");

    /*
     * add l2 addresses to be defined as static - no learning needed
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_l2_addr_add(unit, cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                cint_vpls_pwe_tagged_mode_basic_info.service_mac_2,
                                                cint_vpls_pwe_tagged_mode_basic_info.pwe_encap_id,
                                                cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id), "");

    /* Set tag formats */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_tag_formats_neve(unit), "");

    return BCM_E_NONE;
}

int
vpls_pwe_tagged_mode_basic_tag_formats_neve(
    int unit)
{

    bcm_port_tpid_class_t port_tpid_class;
    /*
     * set tag format for untagged packets 
     */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = native_eve_untag;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_NATIVE_EVE;
    port_tpid_class.vlan_translation_action_id = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &port_tpid_class), "");

    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_ANY;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = native_eve_stag;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_NATIVE_EVE;
    port_tpid_class.vlan_translation_action_id = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &port_tpid_class), "");

    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_ANY;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    port_tpid_class.tag_format_class_id = native_eve_dtag;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_NATIVE_EVE;
    port_tpid_class.vlan_translation_action_id = 0;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_tpid_class_set(unit, &port_tpid_class), "");

    return BCM_E_NONE;
}

int
vpls_pwe_tagged_mode_basic_default_neve(int unit) {


    /* Add default native AC */
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
    vlan_port.flags |= BCM_VLAN_PORT_DEFAULT;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_create(unit, &vlan_port), "");

    default_native_out_ac = vlan_port.vlan_port_id;

    /* Add match to the native AC */
    bcm_port_match_info_t match_info;
    bcm_port_match_info_t_init(&match_info);
    match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY | BCM_PORT_MATCH_NATIVE;
    match_info.port = cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_eg;
    match_info.match = BCM_PORT_MATCH_PORT;
    BCM_IF_ERROR_RETURN_MSG(bcm_port_match_add(unit, vlan_port.vlan_port_id, &match_info), "");

    bcm_vlan_action_set_t action;
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_port_translation_t port_trans;

    /*
     * set edit profile for ingress/egress LIF 
     */
    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.new_outer_vlan = cint_vpls_pwe_tagged_mode_basic_info.egress_canonical_outer_vlan;
    port_trans.new_inner_vlan = 0;
    port_trans.gport = vlan_port.vlan_port_id;
    port_trans.vlan_edit_class_id = cint_vpls_pwe_tagged_mode_basic_info.egress_vlan_edit_profile;
    port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_translation_set(unit, &port_trans), "");

    /*
     * Create action ID
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_WITH_ID|BCM_VLAN_ACTION_SET_EGRESS,&native_eve_action_id), "");

    /*
     * Set translation action class
     */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = cint_vpls_pwe_tagged_mode_basic_info.egress_vlan_edit_profile;
    action_class.tag_format_class_id = native_eve_untag;
    action_class.vlan_translation_action_id = native_eve_action_id;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_class_set(unit, &action_class), "");

    action_class.tag_format_class_id = native_eve_stag;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_class_set(unit, &action_class), "");

    action_class.tag_format_class_id = native_eve_dtag;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_class_set(unit, &action_class), "");
    return BCM_E_NONE;
}

/*
* Several egress PWEs share the same ESEM name space
* thus hit the same ESEM entry
*/
int
vpls_pwe_tagged_mode_neve_esem_match_name_space(
    int unit,
    int access_port,
    int core_port)
{
    int nof_labels = 2;
    int dummy_encap_id;
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;
    int name_space = 511;
    int pwe_encap_id[4];
    int i;

    vpls_pwe_tagged_mode_basic_init(access_port, core_port);

    /*
     * Configure AC and PWE ports
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_configure_port_properties(unit, cint_vpls_pwe_tagged_mode_basic_info.access_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif, 0), "");

    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_create_vsis(unit, cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif), "");

    /*
     * Configure L3 interfaces 
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_create_l3_interfaces(unit, cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif,
                                                         cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif_mac), "");

    /*
     * Configure an ARP entries
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_create_arp_entry(unit, &(cint_vpls_pwe_tagged_mode_basic_info.core_arp_id),
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_next_hop_mac,
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif), "");

    /*
     * Configure a push entry.
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_create_mpls_tunnel(unit,
                                                       cint_vpls_pwe_tagged_mode_basic_info.service_mpls_tunnel_label_1,
                                                       cint_vpls_pwe_tagged_mode_basic_info.service_mpls_tunnel_label_2,
                                                       2, cint_vpls_pwe_tagged_mode_basic_info.core_arp_id,
                                                       &cint_vpls_pwe_tagged_mode_basic_info.mpls_tunnel_id,
                                                       1), "");

    /*
     * Configure fec entry 
     */
    BCM_IF_ERROR_RETURN_MSG(get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id), "");
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_create_fec_entry(unit, cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id,
                                                     cint_vpls_pwe_tagged_mode_basic_info.mpls_tunnel_id,
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_port), "");

    /*
     * configure several PWE tunnels with the same ESEM name space - egress flow 
     */
    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags = BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags2 = BCM_MPLS_PORT2_EGRESS_ONLY | BCM_MPLS_PORT2_PLATFORM_NAMESPACE;
    mpls_port.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

    for (i=0; i < 4; i++) {
        mpls_port.egress_label.label = cint_vpls_pwe_tagged_mode_basic_info.service_pwe_label + i;
        BCM_IF_ERROR_RETURN_MSG(bcm_mpls_port_add(unit, 0, mpls_port), "");

        pwe_encap_id[i] = mpls_port.encap_id;

        /* set mpls port ESEM name space */
        BCM_IF_ERROR_RETURN_MSG(bcm_port_class_set(unit, mpls_port.mpls_port_id, bcmPortClassEgress, name_space), "");
    }

    /* save one egress mpls port for adding egress native AC */ 
    cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_eg = mpls_port.mpls_port_id;

    /** define ac and pwe ports */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_vswitch_add_core_outer_ac(unit, cint_vpls_pwe_tagged_mode_basic_info.core_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif,
                                                              &(cint_vpls_pwe_tagged_mode_basic_info.outer_ac_vlan_port_id)), "");

    /*
     * add l2 addresses to be defined as static - no learning needed
     */
    for (i=0; i < 4; i++) {
        BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_l2_addr_add(unit, cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                    cint_vpls_pwe_tagged_mode_basic_info.service_mac_2,
                                                    pwe_encap_id[i],
                                                    cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id), "");
        cint_vpls_pwe_tagged_mode_basic_info.service_mac_2[5]++;
    }

    /*
     * Set tag formats
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_basic_tag_formats_neve(unit), "");

    /*
     * Add AC for inner ETH EVE (adding service tags)
     * For Egress flow
     */
    BCM_IF_ERROR_RETURN_MSG(vpls_pwe_tagged_mode_add_core_native_ac(unit, cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                 cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_eg,
                                                 &(cint_vpls_pwe_tagged_mode_basic_info.egress_native_ac_vlan_port_id),
                                                 1, &dummy_encap_id), "");

    bcm_vlan_action_set_t action;
    bcm_vlan_translate_action_class_t action_class;
    bcm_vlan_port_translation_t port_trans;

    /*
     * set edit profile for ingress/egress LIF 
     */
    bcm_vlan_port_translation_t_init(&port_trans);
    port_trans.new_outer_vlan = cint_vpls_pwe_tagged_mode_basic_info.egress_canonical_outer_vlan;
    port_trans.new_inner_vlan = cint_vpls_pwe_tagged_mode_basic_info.egress_canonical_inner_vlan;
    port_trans.gport = cint_vpls_pwe_tagged_mode_basic_info.egress_native_ac_vlan_port_id;
    port_trans.vlan_edit_class_id = cint_vpls_pwe_tagged_mode_basic_info.egress_vlan_edit_profile;
    port_trans.flags = BCM_VLAN_ACTION_SET_EGRESS;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_port_translation_set(unit, &port_trans), "");

    /*
     * Create action ID
     */
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_WITH_ID|BCM_VLAN_ACTION_SET_EGRESS,&native_eve_action_id), "");

    /*
     * Set translation action class
     */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = cint_vpls_pwe_tagged_mode_basic_info.egress_vlan_edit_profile;
    action_class.tag_format_class_id = native_eve_untag;
    action_class.vlan_translation_action_id = native_eve_action_id;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_class_set(unit, &action_class), "");

    action_class.tag_format_class_id = native_eve_stag;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_class_set(unit, &action_class), "");

    action_class.tag_format_class_id = native_eve_dtag;
    BCM_IF_ERROR_RETURN_MSG(bcm_vlan_translate_action_class_set(unit, &action_class), "");

    return BCM_E_NONE;
}
