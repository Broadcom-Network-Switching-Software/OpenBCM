/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/* 
 * This cint  demonstrated the configuration of VPLS tagged mode application.
 * The cint uses cint_vswitch_cross_connect_p2p_multi_device.c to configure P2P connection, 
 * VPWS tagged mode functionality is built on top of existing cross connect. 
 *  
 * Supported scenarios are: 
 * PWE -> AC
 * PWE -> PWE
 * Native Ethernet can be single/double tagged
 * 
 * The configuration in this cint:
 *         Input Packet                     VTT                                  FLP                                   Output Packet
 *        --------------                    ===                                  ===                                   -------------
 *    DATAoETH'oPWEoLSPoETH  ->     terminate PWEoLSPoETH       If ETH' is tagged lookup {PWE InLif + VLAN(s)}   ->      DATAoETH'
 *  
 * Similar configuration with PWE P2P is available
 *  
 *
 * Test Scenario 
 *
 * ./bcm.user
 * cd ../../../../regress/bcm
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dpp/utility/cint_utils_multicast.c
 * cint ../../src/examples/dpp/utility/cint_utils_vlan.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_mpls.c
 * cint ../../src/examples/dpp/utility/cint_utils_mpls_port.c
 * cint ../../src/examples/dpp/utility/cint_utils_l3.c
 * cint ../../src/examples/dpp/cint_vswitch_metro_mp.c
 * cint ../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
 * cint ../../src/examples/dpp/cint_port_tpid.c
 * cint ../../src/examples/dpp/cint_multi_device_utils.c
 * cint ../../src/examples/dpp/cint_vswitch_cross_connect_p2p_multi_device.c
 * cint ../../src/examples/dpp/cint_qos_vpls.c
 * cint ../../src/examples/dpp/cint_vswitch_vpws_tagged.c
 * cint
 * run_vpws_tagged(0,1811939528,1811939529,1,0,0);
 * exit;
 *
 * mpls - vlan ; port1 to port2 (Pwe to Access) single tag
 * tx 1 psrc=200 data=0x0000000000110000e0ead2e38100000a88470001474000000b604af10000f07b5e698100000affffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x00000b604af10000f07b5e69810003e9ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * mpls - vlan ; port1 to port2 (Pwe to Access) no tag
 * tx 1 psrc=200 data=0x0000000000110000e0ead2e38100000a88470001474000000b604af10000f07b5e69ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x00000b604af10000f07b5e69810003e8ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * mpls - vlan ; port1 to port2 (Pwe to Access) two tags
 * tx 1 psrc=200 data=0x0000000000110000e0ead2e38100000a88470001474000000b604af10000f07b5e698100000a8100000affffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x00000b604af10000f07b5e69810003ea8100000affffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dpp/utility/cint_utils_multicast.c
 * cint ../../src/examples/dpp/utility/cint_utils_vlan.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_mpls.c
 * cint ../../src/examples/dpp/utility/cint_utils_mpls_port.c
 * cint ../../src/examples/dpp/utility/cint_utils_l3.c
 * cint ../../src/examples/dpp/cint_vswitch_metro_mp.c
 * cint ../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
 * cint ../../src/examples/dpp/cint_port_tpid.c
 * cint ../../src/examples/dpp/cint_multi_device_utils.c
 * cint ../../src/examples/dpp/cint_vswitch_cross_connect_p2p_multi_device.c
 * cint ../../src/examples/dpp/cint_qos_vpls.c
 * cint ../../src/examples/dpp/cint_vswitch_vpws_tagged.c
 * cint
 * run_vpws_tagged(0,1811939528,1811939529,2,0,0);
 * exit;
 *
 * mpls - mpls ; port1 to port2 (Pwe to Pwe) single tag 
 * tx 1 psrc=200 data=0x00000000001100005e74eaa48100000a88470001474000004853c61c0000f91b8a628100000affffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * Received packets on unit 0 should be: 
 * Source port: 0, Destination port: 0 
 * Data: 0x0000000000220000000000118100001688470002a02800016014000291ff00004853c61c0000f91b8a628100000affffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * mpls - mpls ; port1 to port2 (Pwe to Pwe) two tags
 * tx 1 psrc=200 data=0x00000000001100005e74eaa48100000a88470001474000004853c61c0000f91b8a628100000a8100000affffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0 
 * Data: 0x0000000000220000000000118100001788470002b02800017014000291ff00004853c61c0000f91b8a628100000a8100000affffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * mpls - mpls ; port1 to port2 (Pwe to Pwe) no tag 
 * tx 1 psrc=200 data=0x00000000001100005e74eaa48100000a88470001474000004853c61c0000f91b8a62ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * Received packets on unit 0 should be: 
 * Source port: 0, Destination port: 0 
 * Data: 0x0000000000220000000000118100001588470002902800015014000291ff00004853c61c0000f91b8a62ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dpp/utility/cint_utils_multicast.c
 * cint ../../src/examples/dpp/utility/cint_utils_vlan.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_mpls.c
 * cint ../../src/examples/dpp/utility/cint_utils_mpls_port.c
 * cint ../../src/examples/dpp/utility/cint_utils_l3.c
 * cint ../../src/examples/dpp/cint_vswitch_metro_mp.c
 * cint ../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
 * cint ../../src/examples/dpp/cint_port_tpid.c
 * cint ../../src/examples/dpp/cint_multi_device_utils.c
 * cint ../../src/examples/dpp/cint_vswitch_cross_connect_p2p_multi_device.c
 * cint ../../src/examples/dpp/cint_qos_vpls.c
 * cint ../../src/examples/dpp/cint_vswitch_vpws_tagged.c
 * cint
 * run_vpws_tagged(0,1811939528,1811939529,1,0,0);
 * exit;
 *
 * mpls - vlan ; port1 to port2 (Pwe to Access) single tag
 * tx 1 psrc=200 data=0x0000000000110000cfc824e88100000a8847000147400000425996b00000bb8ab5e48100000affffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0 
 * Data: 0x0000425996b00000bb8ab5e4810003e9ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * mpls - vlan ; port1 to port2 (Pwe to Access) no tag 
 * tx 1 psrc=200 data=0x0000000000110000cfc824e88100000a8847000147400000425996b00000bb8ab5e4ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * Received packets on unit 0 should be: 
 * Source port: 0, Destination port: 0 
 * Data: 0x0000425996b00000bb8ab5e4810003e8ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * mpls - vlan ; port1 to port2 (Pwe to Access) two tags
 * tx 1 psrc=200 data=0x0000000000110000cfc824e88100000a8847000147400000425996b00000bb8ab5e48100000a8100000affffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0 
 * Data: 0x0000425996b00000bb8ab5e4810003ea8100000affffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5 
 *
 * cint ../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../src/examples/dpp/utility/cint_utils_multicast.c
 * cint ../../src/examples/dpp/utility/cint_utils_vlan.c
 * cint ../../src/examples/sand/utility/cint_sand_utils_mpls.c
 * cint ../../src/examples/dpp/utility/cint_utils_mpls_port.c
 * cint ../../src/examples/dpp/utility/cint_utils_l3.c
 * cint ../../src/examples/dpp/cint_vswitch_metro_mp.c
 * cint ../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
 * cint ../../src/examples/dpp/cint_port_tpid.c
 * cint ../../src/examples/dpp/cint_multi_device_utils.c
 * cint ../../src/examples/dpp/cint_vswitch_cross_connect_p2p_multi_device.c
 * cint ../../src/examples/dpp/cint_qos_vpls.c
 * cint ../../src/examples/dpp/cint_vswitch_vpws_tagged.c
 * cint
 * run_vpws_tagged(0,1811939528,1811939529,2,0,0);
 * exit;
 *
 * mpls - mpls ; port1 to port2 (Pwe to Pwe) single tag
 * tx 1 psrc=200 data=0x0000000000110000e4173e578100000a88470001474000007d4ee21c0000053e4d828100000affffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x0000000000220000000000118100001688470002a02800016014000291ff00007d4ee21c0000053e4d828100000affffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * mpls - mpls ; port1 to port2 (Pwe to Pwe) two tags
 * tx 1 psrc=200 data=0x0000000000110000e4173e578100000a88470001474000007d4ee21c0000053e4d828100000a8100000affffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x0000000000220000000000118100001788470002b02800017014000291ff00007d4ee21c0000053e4d828100000a8100000affffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * mpls - mpls ; port1 to port2 (Pwe to Pwe) no tag
 * tx 1 psrc=200 data=0x0000000000110000e4173e578100000a88470001474000007d4ee21c0000053e4d82ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 *
 * Received packets on unit 0 should be:
 * Source port: 0, Destination port: 0
 * Data: 0x0000000000220000000000118100001588470002902800015014000291ff00007d4ee21c0000053e4d82ffffc5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5c5
 */

int mpls_termination_label_index_enable = 0;
int mpls_termination_label_index_database_mode = 0;

struct vswitch_vpls_tagged_info_s {
    int vlan;
    int outer_vlan;
    int inner_vlan;

    bcm_gport_t gport_out_tagged;
    bcm_gport_t gport_out_double_tagged;
};

vswitch_vpls_tagged_info_s vswitch_vpls_tagged_info;

void vswitch_vpls_tagged_info_s_init(int unit, int port_in, int port_out1, int port_out2) {
    vswitch_vpls_tagged_info.vlan = 20;
    vswitch_vpls_tagged_info.outer_vlan = 30;
    vswitch_vpls_tagged_info.inner_vlan = 25;
}


/* 
 * This function adds support for VPWS tagged mode 
 * by connecting mpls_port + vlans to forwarding destination
 * same_single_double = 1 - for nof_vlans=1 use same match_vlan for both single and double tags to check lookup priority 
 */
int vswitch_cross_connect_p2p_multi_device_vpws_tagged_add(int unit, int nof_vlans, int same_single_double) {

    int rv = BCM_E_NONE;
    uint32 flags;
    bcm_vswitch_flexible_connect_match_t port_match;
    bcm_vswitch_flexible_connect_fwd_t port_fwd;
    int match_vlan = same_single_double ? vswitch_vpls_tagged_info.outer_vlan : vswitch_vpls_tagged_info.vlan;

    bcm_vswitch_flexible_connect_match_t_init(&port_match);
    bcm_vswitch_flexible_connect_fwd_t_init(&port_fwd);

    port_match.match = nof_vlans > 1 ? BCM_PORT_MATCH_PORT_VLAN_STACKED : BCM_PORT_MATCH_PORT_VLAN;
    port_match.match_port = cross_connect_info.mpls_port_1.mpls_port_id;
    port_match.match_vlan = nof_vlans > 1 ? vswitch_vpls_tagged_info.outer_vlan : match_vlan;
    port_match.match_inner_vlan = nof_vlans > 1 ? vswitch_vpls_tagged_info.inner_vlan : 0;

    port_fwd.forward_port = nof_vlans > 1 ? vswitch_vpls_tagged_info.gport_out_double_tagged : vswitch_vpls_tagged_info.gport_out_tagged;

    rv = bcm_vswitch_flexible_connect_add(unit, flags, &port_match, &port_fwd);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_flexible_connect_add \n");
        return rv;
    }

    return rv;
}

/* 
 * Clear created connection
 */
int vswitch_cross_connect_p2p_multi_device_vpws_tagged_remove(int unit, int nof_vlans, int same_single_double) {

    int rv = BCM_E_NONE;
    bcm_vswitch_flexible_connect_match_t port_match;
    int match_vlan = same_single_double ? vswitch_vpls_tagged_info.outer_vlan : vswitch_vpls_tagged_info.vlan;

    bcm_vswitch_flexible_connect_match_t_init(&port_match);

    port_match.match = nof_vlans > 1 ? BCM_PORT_MATCH_PORT_VLAN_STACKED : BCM_PORT_MATCH_PORT_VLAN;
    port_match.match_port = cross_connect_info.mpls_port_1.mpls_port_id;
    port_match.match_vlan = nof_vlans > 1 ? vswitch_vpls_tagged_info.outer_vlan : match_vlan;
    port_match.match_inner_vlan = nof_vlans > 1 ? vswitch_vpls_tagged_info.inner_vlan : 0;

    rv = bcm_vswitch_flexible_connect_delete(unit, &port_match);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vswitch_flexible_connect_delete \n");
        return rv;
    }

    return rv;
}

int vswitch_vpws_tagged_double_tag_port_configuration(int unit,int in_port){
    int rv=0;

	mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
	mpls_termination_label_index_database_mode = soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_DATABASE_MODE, 2);


	if ((mpls_termination_label_index_enable == 1) &&
	   ((mpls_termination_label_index_database_mode == 22) || (mpls_termination_label_index_database_mode == 23))) {
	   bcm_port_t local_in_port;

		rv = bcm_vlan_control_port_set(unit, in_port, bcmVlanPortDoubleLookupEnable, 1);
		if (rv != BCM_E_NONE) {
			printf("(%s) \n",bcm_errmsg(rv));
			return rv;
		}
		/* need the local port from the gport */
		rv = bcm_port_local_get(unit, in_port, &local_in_port);
		if (rv != BCM_E_NONE) {
		    printf("Error, bcm_port_class_set %d\n", rv);
			return rv;
		}

		/* when a port is configured with  "bcmVlanPortDoubleLookupEnable" the VLAN domain must be unique in the device */
		rv = bcm_port_class_set(unit, local_in_port, bcmPortClassId, local_in_port);
		if (rv != BCM_E_NONE) {
		    printf("Error, bcm_port_class_set %d\n", rv);
			return rv;
		}
	}
	return rv;
}

int
run_vpws_tagged(int unit, int in_port, int out_port, int type_out /* PWE or AC */, int protection_out, int second_port_out /* used if protection2 is set */, int same_single_double /* share match_vlan for both modes */) {
    int rv = BCM_E_NONE;
    uint32 flags = 0;
    mpls_info_s mpls_info;
    int nof_tags;
    bcm_mpls_port_t mpls_port;

    /* configure the port for double tag packets - this function relevant for termination mode 22-23 only */
    rv = vswitch_vpws_tagged_double_tag_port_configuration(unit,in_port);
    if (rv != BCM_E_NONE) {
        printf("Error, in run cross_connect_p2p \n");
        return rv;
    }

    vswitch_vpls_tagged_info_s_init(unit, in_port, out_port, out_port);

    /* Create P2P connection */
    rv = run(&unit, 1/*nof_units*/, in_port, 2/*type - pwe*/, 0 /*protection1*/, 0 /* used if protection1 is set */,
             out_port, type_out, protection_out, second_port_out /* used if protection2 is set */, 0 /* facility or path protection type */);
    if (rv != BCM_E_NONE) {
        printf("Error, in run cross_connect_p2p \n");
        return rv;
    }

    rv = bcm_vlan_create(unit, vswitch_vpls_tagged_info.vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create with vlan %d continue...\n", vswitch_vpls_tagged_info.vlan);
    }

    rv = bcm_vlan_create(unit, vswitch_vpls_tagged_info.outer_vlan);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_create with vlan %d continue...\n", vswitch_vpls_tagged_info.outer_vlan);
    }

    for (nof_tags = 1; nof_tags <= 2; nof_tags++) {
        /* Create out port for tagged packets */
        if (type_out == 1) {
            /* AC */
            rv = vlan_port_create(unit,
                                  out_port,
                                  cross_connect_info.outer_vlan_base,
                                  cross_connect_info.eg_vlan_base,
                                  flags,
                                  (nof_tags == 1) ? &(vswitch_vpls_tagged_info.gport_out_tagged) : &(vswitch_vpls_tagged_info.gport_out_double_tagged),
                                  0/*failover_id_1*/,
                                  0,
                                  0 /*cross_connect_info.protection_mc1*/
                                  );
            if (rv != BCM_E_NONE) {
                printf("Error, in vlan_port_create for tagged \n");
                return rv;
            }

            /* Advance vlans*/
            cross_connect_info.outer_vlan_base++;
            cross_connect_info.eg_vlan_base++;
        }
        if (type_out == 2) {
            /* PWE */
            mpls_info_s_init(&mpls_info);
            bcm_mpls_port_t_init(&mpls_port);

            rv = mpls_port_create(unit,
                                  out_port,
                                  flags,
                                  &mpls_info,
                                  &mpls_port,
                                  0/*failover_id_1*/,
                                  0);
            if (rv != BCM_E_NONE) {
                printf("Error, in mpls_port_create for tagged \n");
                return rv;
            }

            if (nof_tags == 1) {
                vswitch_vpls_tagged_info.gport_out_tagged = mpls_port.mpls_port_id;
            }
            else {
                vswitch_vpls_tagged_info.gport_out_double_tagged = mpls_port.mpls_port_id;
            }

            cross_connect_info.in_vc_label_base++;
            cross_connect_info.eg_vc_label_base++;
            cross_connect_info.vlan_base++;
            cross_connect_info.mpls_eg_vlan_base++;
        }

        /* Connect PWE+TAG to created port */
        rv = vswitch_cross_connect_p2p_multi_device_vpws_tagged_add(unit, nof_tags, same_single_double);
        if (rv != BCM_E_NONE) {
            printf("Error, in vswitch_cross_connect_p2p_multi_device_vpws_tagged_add\n");
            return rv;
        }
    }

    return rv;

}
