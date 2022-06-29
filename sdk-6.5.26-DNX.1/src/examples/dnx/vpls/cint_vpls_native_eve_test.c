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
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/dnx/vpls/cint_vpls_pwe_tagged_mode_basic.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint ../../../../src/examples/dnx/vpls/cint_vpls_native_eve_test.c
 * cint
 * int unit = 0; 
 * int rv = 0; 
 * int in_port = 200; 
 * int out_port = 201;
 * rv = vpls_pwe_tagged_mode_neve_esem(unit,in_port,out_port);
 * rv = vpls_pwe_tagged_mode_neve_eedb(unit,in_port,out_port);
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
    int rv = BCM_E_NONE;
    int nof_labels = 2;
    int dummy_encap_id;

    vpls_pwe_tagged_mode_basic_init(access_port, core_port);

    /*
     * Configure AC and PWE ports
     */
    rv = vpls_pwe_tagged_mode_basic_configure_port_properties(unit, cint_vpls_pwe_tagged_mode_basic_info.access_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_configure_port_properties\n", rv);
        return rv;
    }

    rv = vpls_pwe_tagged_mode_basic_create_vsis(unit, cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_vsis\n", rv);
        return rv;
    }

    /*
     * Configure L3 interfaces 
     */
    rv = vpls_pwe_tagged_mode_basic_create_l3_interfaces(unit, cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif,
                                                         cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif_mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_l3_interfaces\n", rv);
        return rv;
    }

    /*
     * Configure an ARP entries
     */
    rv = vpls_pwe_tagged_mode_basic_create_arp_entry(unit, &(cint_vpls_pwe_tagged_mode_basic_info.core_arp_id),
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_next_hop_mac,
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_arp_entry\n", rv);
        return rv;
    }

    /*
     * Configure a push entry.
     */
    rv = vpls_pwe_tagged_mode_basic_create_mpls_tunnel(unit,
                                                       cint_vpls_pwe_tagged_mode_basic_info.service_mpls_tunnel_label_1,
                                                       cint_vpls_pwe_tagged_mode_basic_info.service_mpls_tunnel_label_2,
                                                       2, cint_vpls_pwe_tagged_mode_basic_info.core_arp_id,
                                                       &cint_vpls_pwe_tagged_mode_basic_info.mpls_tunnel_id,
                                                       1);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_mpls_tunnel\n", rv);
        return rv;
    }

    /*
     * Configure fec entry 
     */
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    rv = vpls_pwe_tagged_mode_basic_create_fec_entry(unit, cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id,
                                                     cint_vpls_pwe_tagged_mode_basic_info.mpls_tunnel_id,
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_fec_entry\n", rv);
        return rv;
    }

    /*
     * configure PWE tunnel - egress flow 
     */
    rv = vpls_pwe_tagged_mode_basic_mpls_port_add_encapsulation(unit,
                                                                &(cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_eg),
                                                                cint_vpls_pwe_tagged_mode_basic_info.service_pwe_label,
                                                                &(cint_vpls_pwe_tagged_mode_basic_info.pwe_encap_id),
                                                                1);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_mpls_port_add_encapsulation\n", rv);
        return rv;
    }

    /** define ac and pwe ports */
    rv = vpls_pwe_tagged_mode_basic_vswitch_add_core_outer_ac(unit, cint_vpls_pwe_tagged_mode_basic_info.core_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif,
                                                              &
                                                              (cint_vpls_pwe_tagged_mode_basic_info.outer_ac_vlan_port_id));
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_vswitch_add_core_outer_ac\n", rv);
        return rv;
    }

    /*
     * add l2 addresses to be defined as static - no learning needed
     */
    rv = vpls_pwe_tagged_mode_basic_l2_addr_add(unit, cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                cint_vpls_pwe_tagged_mode_basic_info.service_mac_2,
                                                cint_vpls_pwe_tagged_mode_basic_info.pwe_encap_id,
                                                cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_l2_addr_add\n", rv);
        return rv;
    }

    /*
     * Set tag formats
     */
    rv = vpls_pwe_tagged_mode_basic_tag_formats_neve(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_tag_formats_neve\n", rv);
        return rv;
    }

    /*
     * Add AC for inner ETH EVE (adding service tags)
     * For Egress flow
     */
    rv = vpls_pwe_tagged_mode_add_core_native_ac(unit, cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                 cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_eg,
                                                 &(cint_vpls_pwe_tagged_mode_basic_info.egress_native_ac_vlan_port_id),
                                                 1, &dummy_encap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_add_core_native_ac\n", rv);
        return rv;
    }

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
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_translation_set\n");
        return rv;
    }

    /*
     * Create action ID
     */
    rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_WITH_ID|BCM_VLAN_ACTION_SET_EGRESS,&native_eve_action_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }

    /*
     * Set translation action class
     */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = cint_vpls_pwe_tagged_mode_basic_info.egress_vlan_edit_profile;
    action_class.tag_format_class_id = native_eve_untag;
    action_class.vlan_translation_action_id = native_eve_action_id;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    action_class.tag_format_class_id = native_eve_stag;
    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    action_class.tag_format_class_id = native_eve_dtag;
    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    return rv;
}

int
vpls_pwe_tagged_mode_neve_eedb(
    int unit,
    int access_port,
    int core_port)
{
    int rv = BCM_E_NONE;
    int nof_labels = 2;
    int dummy_encap_id;

    vpls_pwe_tagged_mode_basic_init(access_port, core_port);

    /*
     * Configure AC and PWE ports
     */
    rv = vpls_pwe_tagged_mode_basic_configure_port_properties(unit, cint_vpls_pwe_tagged_mode_basic_info.access_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_configure_port_properties\n", rv);
        return rv;
    }

    rv = vpls_pwe_tagged_mode_basic_create_vsis(unit, cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_vsis\n", rv);
        return rv;
    }

    /*
     * Configure L3 interfaces 
     */
    rv = vpls_pwe_tagged_mode_basic_create_l3_interfaces(unit, cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif,
                                                         cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif_mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_l3_interfaces\n", rv);
        return rv;
    }

    /*
     * Configure an ARP entries
     */
    rv = vpls_pwe_tagged_mode_basic_create_arp_entry(unit, &(cint_vpls_pwe_tagged_mode_basic_info.core_arp_id),
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_next_hop_mac,
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_arp_entry\n", rv);
        return rv;
    }

    /*
     * Configure a push entry.
     */
    rv = vpls_pwe_tagged_mode_basic_create_mpls_tunnel(unit,
                                                       cint_vpls_pwe_tagged_mode_basic_info.service_mpls_tunnel_label_1,
                                                       cint_vpls_pwe_tagged_mode_basic_info.service_mpls_tunnel_label_2,
                                                       2, cint_vpls_pwe_tagged_mode_basic_info.core_arp_id,
                                                       &cint_vpls_pwe_tagged_mode_basic_info.mpls_tunnel_id,
                                                       0);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_mpls_tunnel\n", rv);
        return rv;
    }

    /*
     * Configure fec entry 
     */
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    rv = vpls_pwe_tagged_mode_basic_create_fec_entry(unit, cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id,
                                                     cint_vpls_pwe_tagged_mode_basic_info.mpls_tunnel_id,
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_fec_entry\n", rv);
        return rv;
    }

    /*
     * configure PWE tunnel - egress flow 
     */
    rv = vpls_pwe_tagged_mode_basic_mpls_port_add_encapsulation(unit,
                                                                &(cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_eg),
                                                                cint_vpls_pwe_tagged_mode_basic_info.service_pwe_label,
                                                                &(cint_vpls_pwe_tagged_mode_basic_info.pwe_encap_id),
                                                                0);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_mpls_port_add_encapsulation\n", rv);
        return rv;
    }

    /** define ac and pwe ports */
    rv = vpls_pwe_tagged_mode_basic_vswitch_add_core_outer_ac(unit, cint_vpls_pwe_tagged_mode_basic_info.core_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif,
                                                              &
                                                              (cint_vpls_pwe_tagged_mode_basic_info.outer_ac_vlan_port_id));
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_vswitch_add_core_outer_ac\n", rv);
        return rv;
    }

    /*
     * Set tag formats
     */
    rv = vpls_pwe_tagged_mode_basic_tag_formats_neve(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_tag_formats_neve\n", rv);
        return rv;
    }

    /*
     * Add AC for inner ETH EVE (adding service tags)
     * For Egress flow
     */
    rv = vpls_pwe_tagged_mode_add_core_native_ac(unit, 0,
                                                 cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_eg,
                                                 &(cint_vpls_pwe_tagged_mode_basic_info.egress_native_ac_vlan_port_id),
                                                 0, &(cint_vpls_pwe_tagged_mode_basic_info.indexed_ac_encap_id));
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_add_core_native_ac\n", rv);
        return rv;
    }

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
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_translation_set\n");
        return rv;
    }

    /*
     * Create action ID
     */
    rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_WITH_ID|BCM_VLAN_ACTION_SET_EGRESS,&native_eve_action_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }

    /*
     * Set translation action class
     */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = cint_vpls_pwe_tagged_mode_basic_info.egress_vlan_edit_profile;
    action_class.tag_format_class_id = native_eve_untag;
    action_class.vlan_translation_action_id = native_eve_action_id;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    action_class.tag_format_class_id = native_eve_stag;
    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    action_class.tag_format_class_id = native_eve_dtag;
    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    /*
     * add l2 addresses to be defined as static - no learning needed
     */
    rv = vpls_pwe_tagged_mode_basic_l2_addr_add(unit, cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                cint_vpls_pwe_tagged_mode_basic_info.service_mac_2,
                                                cint_vpls_pwe_tagged_mode_basic_info.indexed_ac_encap_id,
                                                cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_l2_addr_add\n", rv);
        return rv;
    }

    return rv;
}

int
vpls_pwe_tagged_mode_neve_base(
    int unit,
    int access_port,
    int core_port)
{
    int rv = BCM_E_NONE;
    int nof_labels = 2;
    int dummy_encap_id;

    vpls_pwe_tagged_mode_basic_init(access_port, core_port);

    /*
     * Configure AC and PWE ports
     */
    rv = vpls_pwe_tagged_mode_basic_configure_port_properties(unit, cint_vpls_pwe_tagged_mode_basic_info.access_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_configure_port_properties\n", rv);
        return rv;
    }
    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, cint_vpls_pwe_tagged_mode_basic_info.access_port, cint_vpls_pwe_tagged_mode_basic_info.service_vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    rv = vpls_pwe_tagged_mode_basic_create_vsis(unit, cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_vsis\n", rv);
        return rv;
    }

    /*
     * Configure L3 interfaces 
     */
    rv = vpls_pwe_tagged_mode_basic_create_l3_interfaces(unit, cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif,
                                                         cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif_mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_l3_interfaces\n", rv);
        return rv;
    }

    /*
     * Configure an ARP entries
     */
    rv = vpls_pwe_tagged_mode_basic_create_arp_entry(unit, &(cint_vpls_pwe_tagged_mode_basic_info.core_arp_id),
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_next_hop_mac,
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_arp_entry\n", rv);
        return rv;
    }

    /*
     * Configure a push entry.
     */
    rv = vpls_pwe_tagged_mode_basic_create_mpls_tunnel(unit,
                                                       cint_vpls_pwe_tagged_mode_basic_info.service_mpls_tunnel_label_1,
                                                       cint_vpls_pwe_tagged_mode_basic_info.service_mpls_tunnel_label_2,
                                                       2, cint_vpls_pwe_tagged_mode_basic_info.core_arp_id,
                                                       &cint_vpls_pwe_tagged_mode_basic_info.mpls_tunnel_id,
                                                       1);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_mpls_tunnel\n", rv);
        return rv;
    }

    /*
     * Configure fec entry 
     */
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    rv = vpls_pwe_tagged_mode_basic_create_fec_entry(unit, cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id,
                                                     cint_vpls_pwe_tagged_mode_basic_info.mpls_tunnel_id,
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_fec_entry\n", rv);
        return rv;
    }

    /*
     * configure PWE tunnel - egress flow 
     */
    rv = vpls_pwe_tagged_mode_basic_mpls_port_add_encapsulation(unit,
                                                                &(cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_eg),
                                                                cint_vpls_pwe_tagged_mode_basic_info.service_pwe_label,
                                                                &(cint_vpls_pwe_tagged_mode_basic_info.pwe_encap_id),
                                                                1);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_mpls_port_add_encapsulation\n", rv);
        return rv;
    }

    /** define ac and pwe ports */
    rv = vpls_pwe_tagged_mode_basic_vswitch_add_core_outer_ac(unit, cint_vpls_pwe_tagged_mode_basic_info.core_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif,
                                                              &
                                                              (cint_vpls_pwe_tagged_mode_basic_info.outer_ac_vlan_port_id));
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_vswitch_add_core_outer_ac\n", rv);
        return rv;
    }

    /*
     * add l2 addresses to be defined as static - no learning needed
     */
    rv = vpls_pwe_tagged_mode_basic_l2_addr_add(unit, cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                cint_vpls_pwe_tagged_mode_basic_info.service_mac_2,
                                                cint_vpls_pwe_tagged_mode_basic_info.pwe_encap_id,
                                                cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_l2_addr_add\n", rv);
        return rv;
    }

    /* Set tag formats */
    rv = vpls_pwe_tagged_mode_basic_tag_formats_neve(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_tag_formats_neve\n", rv);
        return rv;
    }

    return rv;
}

int
vpls_pwe_tagged_mode_basic_tag_formats_neve(
    int unit)
{
    int rv = BCM_E_NONE;

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
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_tpid_class_set\n");
        return rv;
    }

    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_ANY;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = native_eve_stag;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_NATIVE_EVE;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_tpid_class_set\n");
        return rv;
    }

    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_ANY;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_ANY;
    port_tpid_class.tag_format_class_id = native_eve_dtag;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY | BCM_PORT_TPID_CLASS_NATIVE_EVE;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_tpid_class_set\n");
        return rv;
    }

    return rv;
}

int
vpls_pwe_tagged_mode_basic_default_neve(int unit) {

    int rv = BCM_E_NONE;

    /* Add default native AC */
    bcm_vlan_port_t vlan_port;
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION;
    vlan_port.flags |= BCM_VLAN_PORT_DEFAULT;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_vlan_port_create\n", rv);
        return rv;
    }

    default_native_out_ac = vlan_port.vlan_port_id;

    /* Add match to the native AC */
    bcm_port_match_info_t match_info;
    bcm_port_match_info_t_init(&match_info);
    match_info.flags = BCM_PORT_MATCH_EGRESS_ONLY | BCM_PORT_MATCH_NATIVE;
    match_info.port = cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_eg;
    match_info.match = BCM_PORT_MATCH_PORT;
    rv = bcm_port_match_add(unit, vlan_port.vlan_port_id, &match_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_port_match_add\n", rv);
        return rv;
    }

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
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_translation_set\n");
        return rv;
    }

    /*
     * Create action ID
     */
    rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_WITH_ID|BCM_VLAN_ACTION_SET_EGRESS,&native_eve_action_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }

    /*
     * Set translation action class
     */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = cint_vpls_pwe_tagged_mode_basic_info.egress_vlan_edit_profile;
    action_class.tag_format_class_id = native_eve_untag;
    action_class.vlan_translation_action_id = native_eve_action_id;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    action_class.tag_format_class_id = native_eve_stag;
    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    action_class.tag_format_class_id = native_eve_dtag;
    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }
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
    int rv = BCM_E_NONE;
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
    rv = vpls_pwe_tagged_mode_basic_configure_port_properties(unit, cint_vpls_pwe_tagged_mode_basic_info.access_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_configure_port_properties\n", rv);
        return rv;
    }

    rv = vpls_pwe_tagged_mode_basic_create_vsis(unit, cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_vsis\n", rv);
        return rv;
    }

    /*
     * Configure L3 interfaces 
     */
    rv = vpls_pwe_tagged_mode_basic_create_l3_interfaces(unit, cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif,
                                                         cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif_mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_l3_interfaces\n", rv);
        return rv;
    }

    /*
     * Configure an ARP entries
     */
    rv = vpls_pwe_tagged_mode_basic_create_arp_entry(unit, &(cint_vpls_pwe_tagged_mode_basic_info.core_arp_id),
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_next_hop_mac,
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_arp_entry\n", rv);
        return rv;
    }

    /*
     * Configure a push entry.
     */
    rv = vpls_pwe_tagged_mode_basic_create_mpls_tunnel(unit,
                                                       cint_vpls_pwe_tagged_mode_basic_info.service_mpls_tunnel_label_1,
                                                       cint_vpls_pwe_tagged_mode_basic_info.service_mpls_tunnel_label_2,
                                                       2, cint_vpls_pwe_tagged_mode_basic_info.core_arp_id,
                                                       &cint_vpls_pwe_tagged_mode_basic_info.mpls_tunnel_id,
                                                       1);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_mpls_tunnel\n", rv);
        return rv;
    }

    /*
     * Configure fec entry 
     */
    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, 0, &cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }
    rv = vpls_pwe_tagged_mode_basic_create_fec_entry(unit, cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id,
                                                     cint_vpls_pwe_tagged_mode_basic_info.mpls_tunnel_id,
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_fec_entry\n", rv);
        return rv;
    }

    /*
     * configure several PWE tunnels with the same ESEM name space - egress flow 
     */
    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags = BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags2 = BCM_MPLS_PORT2_EGRESS_ONLY | BCM_MPLS_PORT2_PLATFORM_NAMESPACE;
    mpls_port.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

    for (i=0; i < 4; i++) {
        mpls_port.egress_label.label = cint_vpls_pwe_tagged_mode_basic_info.service_pwe_label + i;
        rv = bcm_mpls_port_add(unit, 0, mpls_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in bcm_mpls_port_add encapsulation\n", rv);
            return rv;
        }

        pwe_encap_id[i] = mpls_port.encap_id;

        /* set mpls port ESEM name space */
        rv= bcm_port_class_set(unit, mpls_port.mpls_port_id, bcmPortClassEgress, name_space);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_port_class_set\n");
            return rv;
        }
    }

    /* save one egress mpls port for adding egress native AC */ 
    cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_eg = mpls_port.mpls_port_id;

    /** define ac and pwe ports */
    rv = vpls_pwe_tagged_mode_basic_vswitch_add_core_outer_ac(unit, cint_vpls_pwe_tagged_mode_basic_info.core_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif,
                                                              &
                                                              (cint_vpls_pwe_tagged_mode_basic_info.outer_ac_vlan_port_id));
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_vswitch_add_core_outer_ac\n", rv);
        return rv;
    }

    /*
     * add l2 addresses to be defined as static - no learning needed
     */
    for (i=0; i < 4; i++) {
        rv = vpls_pwe_tagged_mode_basic_l2_addr_add(unit, cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                    cint_vpls_pwe_tagged_mode_basic_info.service_mac_2,
                                                    pwe_encap_id[i],
                                                    cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id);
        cint_vpls_pwe_tagged_mode_basic_info.service_mac_2[5]++;
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in vpls_pwe_tagged_mode_basic_l2_addr_add\n", rv);
            return rv;
        }
    }

    /*
     * Set tag formats
     */
    rv = vpls_pwe_tagged_mode_basic_tag_formats_neve(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_tag_formats_neve\n", rv);
        return rv;
    }

    /*
     * Add AC for inner ETH EVE (adding service tags)
     * For Egress flow
     */
    rv = vpls_pwe_tagged_mode_add_core_native_ac(unit, cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                 cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_eg,
                                                 &(cint_vpls_pwe_tagged_mode_basic_info.egress_native_ac_vlan_port_id),
                                                 1, &dummy_encap_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_add_core_native_ac\n", rv);
        return rv;
    }

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
    rv = bcm_vlan_port_translation_set(unit, &port_trans);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_translation_set\n");
        return rv;
    }

    /*
     * Create action ID
     */
    rv = bcm_vlan_translate_action_id_create(unit, BCM_VLAN_ACTION_SET_WITH_ID|BCM_VLAN_ACTION_SET_EGRESS,&native_eve_action_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_id_create\n");
        return rv;
    }

    /*
     * Set translation action class
     */
    bcm_vlan_translate_action_class_t_init(&action_class);
    action_class.vlan_edit_class_id = cint_vpls_pwe_tagged_mode_basic_info.egress_vlan_edit_profile;
    action_class.tag_format_class_id = native_eve_untag;
    action_class.vlan_translation_action_id = native_eve_action_id;
    action_class.flags = BCM_VLAN_ACTION_SET_EGRESS;
    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    action_class.tag_format_class_id = native_eve_stag;
    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    action_class.tag_format_class_id = native_eve_dtag;
    rv = bcm_vlan_translate_action_class_set(unit, &action_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_translate_action_class_set\n");
        return rv;
    }

    return rv;
}
