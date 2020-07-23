/*~~~~~~~~~~~~~~~~~~~~~~~Mulitpoint VPLS Service~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_vswitch_vpls_3level_protection.c
 * Purpose: Example of Open Multi-Point VPLS service with 3-level PWE-MPLS protection.
 * This PWE-MPLS protection example provides the possibility to support both PWE 1:1 protection,
 * LSP 1:1 protection,LDP 1:1 protection i.e. 3-layer protection.
 * This feature is supported from Jericho / Q-MX and above.

 * solution brief:
 *  1-level: PMF resolve the PW protection status.
 *  2-level : 1st FEC resolve the LSP protection status.
 *  3-level: 2nd FEC resolve the LDP protection status.

 * there are 2 mpls tunnel EEDBs encapsulated to packet, So the first MPLS tunnel holds the PW header info, and the second MPLS tunnel
 * holds the LSP and LDP MPLS header info.
 * So for a standard L3 protection scenario, there will be 2(PW) + 8(lsp*ldp) = 10 EEDB entries consumed.
 * In this example, for simplication, the primary PW and protected PW will use the same LSP tunnel.


 * Note: Current implementation supports unicast packets only.
 *
 * Headers:
 *
 * Primary path:
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |  | DA |SA||TIPD1 |Prio|VID||  LDP    |  LSP    ||   PWE    ||Eth||Data ||
 *   |  |0:11|  ||0x8847|    |100||Label:2000|Label:1000||Lable:2010||   ||     || 
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |                +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                  |
 *   |                | Figure 1 : Packets Received from PWE  |                  | 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    |
 *   |  |DA|SA||TIPD1 |Prio|VID||  LDP    |  LSP    ||   PWE    ||Eth||Data |  |
 *   |  |  |  ||0x8847|    |200||Label:2111|Label:1111||Lable:1982||   ||     |  | 
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    |
 *   |                +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                 |
 *   |                | Figure 2 : Packets Transmitted to PWE  |primary LSP,primary LDP                 | 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  

 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    |
 *   |  |DA|SA||TIPD1 |Prio|VID||  LDP    |  LSP    ||   PWE    ||Eth||Data |  |
 *   |  |  |  ||0x8847|    |200||Label:2112|Label:1111||Lable:1982||   ||     |  | 
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    |
 *   |                +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                 |
 *   |                | Figure 2 : Packets Transmitted to PWE  |primary LSP,procted LDP                 | 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  

 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    |
 *   |  |DA|SA||TIPD1 |Prio|VID||  LDP    |  LSP    ||   PWE    ||Eth||Data |  |
 *   |  |  |  ||0x8847|    |200||Label:2311|Label:1311||Lable:1982||   ||     |  | 
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    |
 *   |                +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                 |
 *   |                | Figure 2 : Packets Transmitted to PWE  |protected LSP  and primary LDP               | 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  

 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    |
 *   |  |DA|SA||TIPD1 |Prio|VID||  LDP    |  LSP    ||   PWE    ||Eth||Data |  |
 *   |  |  |  ||0x8847|    |200||Label:2312|Label:1311||Lable:1982||   ||     |  | 
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    |
 *   |                +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                 |
 *   |                | Figure 2 : Packets Transmitted to PWE  |protected LSP  and protected LDP              | 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
 *
 * Protected path:
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |  | DA |SA||TIPD1 |Prio|VID||  LDP    |  LSP    ||   PWE    ||Eth||Data ||
 *   |  |0:11|  ||0x8847|    |100||Label:2001|Label:1001||Lable:2020||   ||     || 
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |                +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                  |
 *   |                | Figure 1 : Packets Received from PWE  |                  | 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    |
 *   |  |DA|SA||TIPD1 |Prio|VID||  LDP    |  LSP    ||   PWE    ||Eth||Data |  |
 *   |  |  |  ||0x8847|    |200||Label:2111|Label:1111||Lable:1992||   ||     |  | 
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    |
 *   |                +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                 |
 *   |                | Figure 2 : Packets Transmitted to PWE  |                 | 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
 * 
 * Access side packets:
 * 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |                       tag1                 tag2                           |
 *   |   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   |
 *   |   | DA | SA || TIPD1 | Prio | VID || TIPD2 | Prio | VID ||    Data    |   |
 *   |   |    |    || 0x8100|      | 10  || 0x9100|      | 20  ||            |   |
 *   |   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+   |
 *   |        +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+       |
 *   |        | Figure 3 : Packets Received/Transmitted on Access Port   |       |
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
 *  
 * 
 * 
 * Calling sequence:
 *  -   Port TPIDs setting:
 *      -   For P2 set outer TPID to 0x8100 and inner TPID to 0x9100 (using cint_port_tpid.c).
 *      -   For P1 set outer TPID to 0x8100 and no inner TPID (using cint_port_tpid.c).
 *  -   Create multipoint VSI #6202. Refer to mpls_port__vswitch_vpls_vpn_create__set.
 *      -   Use open_ingress_mc_group() to open multicast group for flooding.
 *          -    Multicast group ID is equal to VPN ID.
 *  -   Create egress LL object with outgoing port and vlan information.
 *          -    Calls l3__egress_only_encap__create().
 *  -   Set up LSP-LDP tunnels. Refer to mpls_tunnels_config().
 *      -   Set MPLS L2 termination (on ingress).
 *          -    MPLS packets arriving with DA 00:00:00:00:00:11 and VID 100 causes L2 termination.  
 *          -    Calls bcm_l3_intf_create().
 *      -   Add pop entries to MPLS switch. Refer to mpls_add_pop_entry.
 *          -    Calls bcm_mpls_tunnel_switch_create().
 *          -    Create MPLS L3 interface (on egress).
 *          -    Packet routed to this L3 interface is set with this MAC. 
 *          -    Calls bcm_l3_intf_create().
 *      -   Create MPLs tunnels over the created l3 interface. Refer to mpls__create_tunnel_initiator__set().
 *  -   Create cascaded FEC object for LDP protection.
 *          -    Calls l3__egress__create().
 *  -   Add AC and MPLS-ports to the VSI.
 *      -   vswitch_vpls_add_access_port_1 creates attachment circuit and add them to the Vswitch and 
 *          update the Multicast group.
 *      -   vswitch_vpls_add_network_port_1 creates PWE and add them to the VSI and update the 
 *          multicast group. This call consists of 3 parts which are 3 calls to bcm_mpls_port_add:
 *          - Egress only : PWE Outlif
 *          - FEC only : LSP Protection FEC pointing to LDP cascaded FEC and PWE outlif
 *          - Ingress only : PWE termination
 * -  Add PWE portection mechanism in ingress PMF stage.
 *      -  field_3_level_protection_tcam_group_set  install group in PMF
 *      - field_3_level_protection_tcam_entry_create add rule for a PWE protection group

 * Remarks:
 *  -   To set the protection properties use vswitch_vpls_hvpls_xxx_failover_set
 *
 * To Activate Above Settings Run:
 *      BCM> cint examples/dpp/cint_port_tpid.c
 *      BCM> cint examples/dpp/cint_advanced_vlan_translation_mode.c
 *      BCM> cint examples/dpp/cint_qos.c
 *      BCM> cint examples/dpp/cint_mpls_lsr.c
 *      BCM> cint examples/dpp/cint_vswitch_metro_mp.c
 *      BCM> cint examples/dpp/cint_vswitch_vpls.c
 *      BCM> cint examples/dpp/utility/cint_utils_mpls_port.c
 *  	BCM> cint examples/sand/utility/cint_sand_utils_mpls.c
 *  	BCM> cint examples/dpp/utility/cint_utils_multicast.c
 *      BCM> cint examples/dpp/cint_vswitch_vpls.c 
 *      BCM> cint examples/dpp/cint_vswitch_vpls_3level_portection.c 
 *      BCM> cint
 *      cint> int rv;
 *      cint> rv = vswitch_vpls_3level_protection_run_with_defaults(unit); 
 */

struct cint_vswitch_vpls_hvpls_info_s {

    /* tunnels info */
    int in_tunnel_label_primary;
    int in_tunnel_label_protected;
    int out_tunnel_label_primary;
    int out_tunnel_label_protected;
    /* 
      * egress tunnel eedb entry will store ldp label info beside lsp label
      * primary ldp label = mpls tunnel label+1000;
      * backup ldp label = mpls tunnel label+1001;
      */
    int primary_pw_mpls_tunnel_label;

    int backup_pw_mpls_tunnel_label;


    bcm_if_t primary_pw_ldp_fec; /* ldp FEC */	
    bcm_if_t backup_pw_ldp_fec; /* ldp FEC */	

    bcm_gport_t primary_pwe_fec; /* PWE FEC */	
    bcm_gport_t backup_pwe_fec; /* PWE FEC */	


    bcm_if_t primary_pw_ldp_tunnel_id_primary; /* Interface id of MPLS tunnel */	
    bcm_if_t primary_pw_ldp_tunnel_id_protected; /* Interface id of MPLS tunnel */	

    bcm_if_t backup_pw_ldp_tunnel_id_primary; /* Interface id of MPLS tunnel */	
    bcm_if_t backup_pw_ldp_tunnel_id_protected; /* Interface id of MPLS tunnel */	


    int ll_intf_id; /* LL interface id */

    int vlan; /* vlan in Core direction. For vpls LL */

    int pwe_outlif_primary; /* outlifs are configured with id in this mode. */
    int pwe_outlif_protected; /* outlifs are configured with id in this mode. */

    int in_vc_label_primary; /* pwe label primary */
    int in_vc_label_protected; /* pwe label protected */
    int eg_vc_label_primary; /* pwe label primary */
    int eg_vc_label_protected; /* pwe label protected */

    uint8 mac[6]; /* next hop for vpls LL */

    bcm_failover_t primary_pwe_mpls_failover_id_fec;
    bcm_failover_t backup_pwe_mpls_failover_id_fec;

    bcm_failover_t pwe_failover_id_outlif;


    bcm_failover_t primary_pw_ldp_failover_id_fec;
    bcm_failover_t backup_pw_ldp_failover_id_fec;
    bcm_failover_t primary_pw_ldp_failover_id_outlif;
    bcm_failover_t backup_pw_ldp_failover_id_outlif;


};

cint_vswitch_vpls_hvpls_info_s vswitch_vpls_hvpls_info;
bcm_field_group_t pw_protection_state_resolve_group;
bcm_field_entry_t pw_protection_ent = -1;

void
vswitch_vpls_hvpls_3level_info_init(int unit, int ac_port, int pwe_in_port, int pwe_out_port, int ac_port1_outer_vlan, int ac_port1_inner_vlan, int vpn_id) {

    int i, rc, is_qux;
    rc = is_qumran_ux_only(unit, &is_qux);
    if (rc != BCM_E_NONE) {
        printf("Error, is_qumran_ux_only()\n");
        return rc;
    }

    vswitch_vpls_hvpls_info.vlan = 200;

    vswitch_vpls_hvpls_info.pwe_outlif_primary = is_qux? 0x2100 : 0x7000; 
    vswitch_vpls_hvpls_info.pwe_outlif_protected = is_qux? 0x2102 : 0x7002;

    for (i=0; i<6; i++) {
        vswitch_vpls_hvpls_info.mac[i] = 0;
    }
    vswitch_vpls_hvpls_info.mac[5] = 0x55;

    /* VPN id to create */
    if (vpn_id < 0) {
        vswitch_vpls_info_1.vpn_id = default_vpn_id;
    } else {
        vswitch_vpls_info_1.vpn_id = vpn_id;
    }

    /* incomming packet attributes */
    vswitch_vpls_info_1.ac_in_port = ac_port;
    vswitch_vpls_info_1.pwe_in_port = pwe_in_port;
    vswitch_vpls_info_1.pwe_out_port = pwe_out_port;

    /* VC label */
    vswitch_vpls_hvpls_info.in_vc_label_primary = 2010;
    vswitch_vpls_hvpls_info.in_vc_label_protected = 2020;
    vswitch_vpls_hvpls_info.eg_vc_label_primary = 1982;
    vswitch_vpls_hvpls_info.eg_vc_label_protected = 1992;

    /* tunnel info */
    vswitch_vpls_hvpls_info.in_tunnel_label_primary = 1000;
    vswitch_vpls_hvpls_info.in_tunnel_label_protected = 1001;
    vswitch_vpls_hvpls_info.out_tunnel_label_primary = 2000;
    vswitch_vpls_hvpls_info.out_tunnel_label_protected = 2001;

    vswitch_vpls_hvpls_info.primary_pw_mpls_tunnel_label = 1111;
    vswitch_vpls_hvpls_info.backup_pw_mpls_tunnel_label = 1311;
  
    vswitch_vpls_info_1.ac_port1_outer_vlan = ac_port1_outer_vlan;
    vswitch_vpls_info_1.ac_port1_inner_vlan = ac_port1_inner_vlan;

    vswitch_vpls_info_1.ac_port2_outer_vlan = ac_port1_outer_vlan+5;
    vswitch_vpls_info_1.access_index_1 = 2;
    vswitch_vpls_info_1.access_index_2 = 3;
    vswitch_vpls_info_1.access_index_3 = 3;

    /* set mpls tunneling information with default values */
    mpls_lsr_init(vswitch_vpls_info_1.pwe_in_port, vswitch_vpls_info_1.pwe_out_port, 0x11, 0x22, 0/*in_label*/, 0/*out_label*/, 100, 200, 0);
    mpls_lsr_info_1.eg_port = vswitch_vpls_info_1.pwe_out_port;


    egress_mc = 1;

    /* Init utility functions cint */
    mpls__init(unit);

}

/* 
 * This function configures MPLS tunnels 
 * these mpls tunnel will encapsulate LSP label and LDP label
 */
int
mpls_tunnels_config(int unit){


    int CINT_NO_FLAGS = 0;
    int ingress_intf;
    bcm_pbmp_t pbmp;
    int rv;
    bcm_mpls_label_t label_for_pop_entry_in, label_for_pop_entry_out;
    uint8 is_primary;
    mpls__egress_tunnel_utils_s mpls_tunnel_properties;

    mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
    
    /* l2 termination for mpls routing: packet received on those VID MAC will be L2 terminated  */
    create_l3_intf_s intf;
    intf.vsi = mpls_lsr_info_1.in_vid;
    intf.my_global_mac = mpls_lsr_info_1.my_mac;
    intf.my_lsb_mac = mpls_lsr_info_1.my_mac;
    
    rv = l3__intf_rif__create(unit, &intf);
    ingress_intf = intf.rif;
    if (rv != BCM_E_NONE) {
    	printf("Error, in l3__intf_rif__create\n");
    	return rv;
    }
    
    /*  create ingress object, packet will be routed to */
    intf.vsi = mpls_lsr_info_1.eg_vid;
    
    rv = l3__intf_rif__create(unit, &intf);
    ingress_intf = intf.rif;
    if (rv != BCM_E_NONE) {
    	printf("Error, in l3__intf_rif__create\n");
    	return rv;
    }

    /*primary pw ldp tunnel {*/

    /* Create an ldp Tunnel Egress failover ID */
    rv = bcm_failover_create(unit, BCM_FAILOVER_ENCAP, &vswitch_vpls_hvpls_info.primary_pw_ldp_failover_id_outlif);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for ldp Egress, rv - %d\n", rv);
        return rv;
    }
    printf("primary pw LDP egress Failover id: 0x%x\n", vswitch_vpls_hvpls_info.primary_pw_ldp_failover_id_outlif);

    /* set tunnel over this l3 interface, so packet forwarded to this interface will be tunneled */
    /* Protected  ldp */
    mpls_tunnel_properties.tunnel_id = 0;
    mpls_tunnel_properties.next_pointer_intf =  ingress_intf;
    mpls_tunnel_properties.label_in =  vswitch_vpls_hvpls_info.primary_pw_mpls_tunnel_label;
    mpls_tunnel_properties.label_out =  vswitch_vpls_hvpls_info.primary_pw_mpls_tunnel_label+1001;
    mpls_tunnel_properties.egress_failover_id = vswitch_vpls_hvpls_info.primary_pw_ldp_failover_id_outlif;
    mpls_tunnel_properties.egress_failover_if_id = 0;
    mpls_tunnel_properties.flags = BCM_MPLS_EGRESS_LABEL_PROTECTION;
    mpls_tunnel_properties.flags_out = BCM_MPLS_EGRESS_LABEL_PROTECTION;

    rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__create_tunnel_initiator__set\n");
        return rv;
    }

    vswitch_vpls_hvpls_info.primary_pw_ldp_tunnel_id_protected= mpls_tunnel_properties.tunnel_id;

    if (verbose1) {
        printf("MPLS tunnel: add primary-pw-protected-tunnel id 0x%08x\n\r", mpls_tunnel_properties.tunnel_id);
    }

    /* Primary ldp */
    mpls_tunnel_properties.next_pointer_intf =  ingress_intf;
    mpls_tunnel_properties.label_in =  vswitch_vpls_hvpls_info.primary_pw_mpls_tunnel_label;
    mpls_tunnel_properties.label_out =  vswitch_vpls_hvpls_info.primary_pw_mpls_tunnel_label+1000;
    mpls_tunnel_properties.egress_failover_id = vswitch_vpls_hvpls_info.primary_pw_ldp_failover_id_outlif;
    mpls_tunnel_properties.egress_failover_if_id = mpls_tunnel_properties.tunnel_id;
    mpls_tunnel_properties.tunnel_id = 0;
    mpls_tunnel_properties.flags = BCM_MPLS_EGRESS_LABEL_PROTECTION;
    mpls_tunnel_properties.flags_out = BCM_MPLS_EGRESS_LABEL_PROTECTION;

    rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__create_tunnel_initiator__set\n");
        return rv;
    }

    vswitch_vpls_hvpls_info.primary_pw_ldp_tunnel_id_primary= mpls_tunnel_properties.tunnel_id;

    if (verbose1) {
        printf("MPLS tunnel: add primary-pw-primary-tunnel id 0x%08x\n\r", mpls_tunnel_properties.tunnel_id);
    }

    bcm_mpls_egress_label_t label_array[2];
    int label_count;
    bcm_mpls_egress_label_t_init(&label_array[0]);
    bcm_mpls_egress_label_t_init(&label_array[1]);

    rv = bcm_mpls_tunnel_initiator_get(unit, mpls_tunnel_properties.tunnel_id, 2, label_array, &label_count);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_get\n");
        return rv;
    }
    printf("bcm_mpls_tunnel_initiator_get for Primary pw ldp: tunnel_id - %d, egress_failover_id - %d, egress_failover_if_id - %d\n\r",
           label_array[0].tunnel_id, label_array[0].egress_failover_id, label_array[0].egress_failover_if_id);


    /*primary pw ldp tunnel }*/

    /*backup pw ldp tunnel {*/

    /* Create an ldp Tunnel Egress failover ID */
    rv = bcm_failover_create(unit, BCM_FAILOVER_ENCAP, &vswitch_vpls_hvpls_info.backup_pw_ldp_failover_id_outlif);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for ldp Egress, rv - %d\n", rv);
        return rv;
    }
    printf("backup pw LDP egress Failover id: 0x%x\n", vswitch_vpls_hvpls_info.backup_pw_ldp_failover_id_outlif);

    /* set tunnel over this l3 interface, so packet forwarded to this interface will be tunneled */
    /* Protected  ldp */
    mpls_tunnel_properties.tunnel_id = 0;
    mpls_tunnel_properties.next_pointer_intf =  ingress_intf;
    mpls_tunnel_properties.label_in =  vswitch_vpls_hvpls_info.backup_pw_mpls_tunnel_label;
    mpls_tunnel_properties.label_out =  vswitch_vpls_hvpls_info.backup_pw_mpls_tunnel_label+1001;
    mpls_tunnel_properties.egress_failover_id = vswitch_vpls_hvpls_info.backup_pw_ldp_failover_id_outlif;
    mpls_tunnel_properties.egress_failover_if_id = 0;
    mpls_tunnel_properties.flags = BCM_MPLS_EGRESS_LABEL_PROTECTION;
    mpls_tunnel_properties.flags_out = BCM_MPLS_EGRESS_LABEL_PROTECTION;

    rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__create_tunnel_initiator__set\n");
        return rv;
    }

    vswitch_vpls_hvpls_info.backup_pw_ldp_tunnel_id_protected= mpls_tunnel_properties.tunnel_id;

    if (verbose1) {
        printf("MPLS tunnel: add protected-pw-backup-tunnel id 0x%08x\n\r", mpls_tunnel_properties.tunnel_id);
    }

    /* Primary ldp */
    mpls_tunnel_properties.next_pointer_intf =  ingress_intf;
    mpls_tunnel_properties.label_in =  vswitch_vpls_hvpls_info.backup_pw_mpls_tunnel_label;
    mpls_tunnel_properties.label_out =  vswitch_vpls_hvpls_info.backup_pw_mpls_tunnel_label+1000;
    mpls_tunnel_properties.egress_failover_id = vswitch_vpls_hvpls_info.backup_pw_ldp_failover_id_outlif;
    mpls_tunnel_properties.egress_failover_if_id = mpls_tunnel_properties.tunnel_id;
    mpls_tunnel_properties.tunnel_id = 0;
    mpls_tunnel_properties.flags = BCM_MPLS_EGRESS_LABEL_PROTECTION;
    mpls_tunnel_properties.flags_out = BCM_MPLS_EGRESS_LABEL_PROTECTION;

    rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__create_tunnel_initiator__set\n");
        return rv;
    }

    vswitch_vpls_hvpls_info.backup_pw_ldp_tunnel_id_primary= mpls_tunnel_properties.tunnel_id;

    if (verbose1) {
        printf("MPLS tunnel: add protected-pw-primary-tunnel id 0x%08x\n\r", mpls_tunnel_properties.tunnel_id);
    }

    bcm_mpls_egress_label_t_init(&label_array[0]);
    bcm_mpls_egress_label_t_init(&label_array[1]);

    rv = bcm_mpls_tunnel_initiator_get(unit, mpls_tunnel_properties.tunnel_id, 2, label_array, &label_count);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_mpls_tunnel_initiator_get\n");
        return rv;
    }
    printf("bcm_mpls_tunnel_initiator_get for backup pw ldp: tunnel_id - %d, egress_failover_id - %d, egress_failover_if_id - %d\n\r",
           label_array[0].tunnel_id, label_array[0].egress_failover_id, label_array[0].egress_failover_if_id);


    /* Adding pop entries for protected and primary tunnels */
    for (is_primary=0; is_primary<=1; is_primary++) {

        if (is_primary) {
            label_for_pop_entry_out = vswitch_vpls_hvpls_info.out_tunnel_label_primary;
            label_for_pop_entry_in = vswitch_vpls_hvpls_info.in_tunnel_label_primary;
             if (mpls_termination_label_index_enable) {
                BCM_MPLS_INDEXED_LABEL_SET(label_for_pop_entry_out, vswitch_vpls_hvpls_info.out_tunnel_label_primary, 1);
                BCM_MPLS_INDEXED_LABEL_SET(label_for_pop_entry_in, vswitch_vpls_hvpls_info.in_tunnel_label_primary, 1);
            }
        }
        else {
            label_for_pop_entry_out = vswitch_vpls_hvpls_info.out_tunnel_label_protected;
            label_for_pop_entry_in = vswitch_vpls_hvpls_info.in_tunnel_label_protected;
            if (mpls_termination_label_index_enable) {
                BCM_MPLS_INDEXED_LABEL_SET(label_for_pop_entry_out, vswitch_vpls_hvpls_info.out_tunnel_label_protected, 1);
                BCM_MPLS_INDEXED_LABEL_SET(label_for_pop_entry_in, vswitch_vpls_hvpls_info.in_tunnel_label_protected, 1);
            }
        }

        rv = mpls_add_pop_entry(unit, label_for_pop_entry_out);
        if (rv != BCM_E_NONE) {
            printf("Error, in mpls_add_pop_entry\n");
            return rv;
        }

        rv = mpls_add_pop_entry(unit, label_for_pop_entry_in);
        if (rv != BCM_E_NONE) {
            printf("Error, in mpls_add_pop_entry\n");
            return rv;
        }
    }
       /*primary pw ldp tunnel }*/

    return rv;
}

/* 
 * Configure mpls port INGRESS attributes: InLif and learning information. 
 * Currently only one InLif is added. 
 */
int
vswitch_vpls_hvpls_ingress_mpls_port_create(int unit){
    int rv;
    bcm_mpls_port_t mpls_port_ingress;
    int modid;
    int modport;
    int is_primary;
    int val;

    mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);

    /* 
     *  Ingress_only - primary and protected
     */
    for (is_primary=0; is_primary<=1; is_primary++) {
        bcm_mpls_port_t_init(&mpls_port_ingress);
        mpls_port_ingress.flags = BCM_MPLS_PORT_NETWORK|BCM_MPLS_PORT_EGRESS_TUNNEL|BCM_MPLS_PORT_WITH_ID|BCM_MPLS_PORT_ENCAP_WITH_ID;
        mpls_port_ingress.flags2 = BCM_MPLS_PORT2_INGRESS_ONLY; /* Indicate we only create PWE-In-LIF allocation */
        rv = mpls_port__update_network_group_id(unit, &mpls_port_ingress);
        if (rv != BCM_E_NONE) {
            printf("Error, mpls_port__update_network_group_id\n");
            return rv;
        }

        /* Set parameters for ingress mpls port */
        mpls_port_ingress.criteria = BCM_MPLS_PORT_MATCH_LABEL;
        mpls_port_ingress.match_label = is_primary ? vswitch_vpls_hvpls_info.in_vc_label_primary : vswitch_vpls_hvpls_info.in_vc_label_protected;
        if (mpls_termination_label_index_enable) {
            val = mpls_port_ingress.match_label;
            BCM_MPLS_INDEXED_LABEL_SET(&mpls_port_ingress.match_label, val, 2);
        }

        mpls_port_ingress.port = BCM_GPORT_INVALID; /* Port is not required for PWE-InLIF */
        mpls_port_ingress.egress_tunnel_if = 0; /* PWE In-LIF in this case do not care about MPLS egress_if. It is not part of learning information. */

        mpls_port_ingress.failover_port_id = vswitch_vpls_hvpls_info.backup_pwe_fec; /* Learning - PWE FEC */

        /* Only one inlif is configured. It should be equal to one of the Outlifs to get same-interface filtering (Multicast group member) */
        BCM_GPORT_MPLS_PORT_ID_SET(mpls_port_ingress.mpls_port_id, is_primary?vswitch_vpls_hvpls_info.pwe_outlif_primary:vswitch_vpls_hvpls_info.pwe_outlif_protected); 
        mpls_port_ingress.encap_id = is_primary?vswitch_vpls_hvpls_info.pwe_outlif_primary:vswitch_vpls_hvpls_info.pwe_outlif_protected; 

        rv = bcm_mpls_port_add(unit, vswitch_vpls_shared_info_1.vpn, &mpls_port_ingress);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_mpls_port_add\n");
            return rv;
        }

        if (verbose1) {
            printf("INGRESS: add port 0x%08x to vpn \n\r", mpls_port_ingress.mpls_port_id);
        }

        if (is_primary) {
            /* handle of the created gport */
            network_port_id = mpls_port_ingress.mpls_port_id;
        }
    }
        
    return rv;   
}

/* 
 * Configure mpls port EGRESS attributes: OutLif
 */
int
vswitch_vpls_hvpls_egress_mpls_port_create(int unit){
    int rv;
    bcm_mpls_port_t mpls_port_egress;
    int modid;
    int modport;
    int tm_port;
    
    /* 
     *  Egress only
     */

    /* create egress failover id for PWE */
    rv = bcm_failover_create(unit, BCM_FAILOVER_ENCAP, &vswitch_vpls_hvpls_info.pwe_failover_id_outlif);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for PWE Egress, rv - %d\n", rv);
        return rv;
    } 
    printf("PWE egress Failover id: 0x%x\n", vswitch_vpls_hvpls_info.pwe_failover_id_outlif);

    bcm_mpls_port_t_init(&mpls_port_egress);
    mpls_port_egress.flags = BCM_MPLS_PORT_NETWORK|BCM_MPLS_PORT_EGRESS_TUNNEL | BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_ENCAP_WITH_ID;
    mpls_port_egress.flags2 = BCM_MPLS_PORT2_EGRESS_ONLY | BCM_MPLS_PORT2_EGRESS_PROTECTION; /* Indicate creation of PWE Out-LIF only */
    rv = mpls_port__update_network_group_id(unit, &mpls_port_egress);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_port__update_network_group_id\n");
        return rv;
    }

    /* Set parameters for egress mpls port */
    if (pwe_cw) {
        mpls_port_egress.flags |= BCM_MPLS_PORT_CONTROL_WORD;
    }
    mpls_port_egress.port = BCM_GPORT_INVALID; /* Port is not required for PWE-OutLIF */
    /* egress label parameters for pwe encapsulation configuration */
    mpls_port_egress.egress_label.ttl = 10;
    mpls_port_egress.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_SET;

    /* ***** PWE outlif protected **** */
    mpls_port_egress.egress_failover_id = vswitch_vpls_hvpls_info.pwe_failover_id_outlif;
    mpls_port_egress.egress_failover_port_id = 0;
    mpls_port_egress.egress_label.label = vswitch_vpls_hvpls_info.eg_vc_label_protected;
    mpls_port_egress.encap_id = vswitch_vpls_hvpls_info.pwe_outlif_protected; /* encap id is the outlif */
    mpls_port__egress_tunnel_if_egress_only__set(unit, vswitch_vpls_info_1.tunnel_id, 1, &mpls_port_egress);
    /* mpls_port_id is the outlif id encapsulated as MPLS_PORT */
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port_egress.mpls_port_id, mpls_port_egress.encap_id);

    if (verbose1) {
        printf("ADDING MPLS PORT EGRESS (protected): mpls_port_id: 0x%08x, encap_id: 0x%08x\n", mpls_port_egress.mpls_port_id, mpls_port_egress.encap_id);
    }

    rv = bcm_mpls_port_add(unit, 0, &mpls_port_egress);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add\n");
        return rv;
    }
    if (verbose1) {
        printf("EGRESS: add port 0x%08x to vpn \n\r", mpls_port_egress.mpls_port_id);
    }

    /* update Multicast table */
    if (is_device_or_above(unit,JERICHO)) {
        rv = get_core_and_tm_port_from_port(unit, vswitch_vpls_info_1.pwe_in_port, &modid, &tm_port);
        if (rv != BCM_E_NONE) {
            printf("Error, in get_core_and_tm_port_from_port\n");
            return rv;
        }

        BCM_GPORT_MODPORT_SET(modport, modid, tm_port);
    }
    else {
        rv = bcm_stk_modid_get(unit, &modid);
        if (rv != BCM_E_NONE){
            printf("Error, in bcm_stk_modid_get\n");
            return rv;
        }
        BCM_GPORT_MODPORT_SET(modport, modid, vswitch_vpls_info_1.pwe_in_port);
    }

    /* ***** PWE outlif primary **** */
    mpls_port_egress.egress_failover_id = vswitch_vpls_hvpls_info.pwe_failover_id_outlif;
    mpls_port_egress.egress_failover_port_id = mpls_port_egress.encap_id;
    mpls_port_egress.egress_label.label = vswitch_vpls_hvpls_info.eg_vc_label_primary;
    mpls_port_egress.encap_id = vswitch_vpls_hvpls_info.pwe_outlif_primary; /* encap id is the outlif */
    mpls_port__egress_tunnel_if_egress_only__set(unit, vswitch_vpls_info_1.tunnel_id, 1, &mpls_port_egress);
    /* mpls_port_id is the outlif id encapsulated as MPLS_PORT */
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port_egress.mpls_port_id, mpls_port_egress.encap_id);

    if (verbose1) {
        printf("ADDING MPLS PORT EGRESS (primary): mpls_port_id: 0x%08x, encap_id: 0x%08x\n", mpls_port_egress.mpls_port_id, mpls_port_egress.encap_id);
    }

    rv = bcm_mpls_port_add(unit, 0, &mpls_port_egress);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add\n");
        return rv;
    }
    if (verbose1) {
        printf("EGRESS: add port 0x%08x to vpn \n\r", mpls_port_egress.mpls_port_id);
    }

    /* Adding egress mpls port to multicast because when egress and ingress are configured separately the multicast should point to the outlif */

    return rv;
}

/* 
 * Configure mpls port mpls tunnel FEC attributes
 */
int
vswitch_vpls_hvpls_fec_mpls_port_create(int unit){
    int rv;
    mpls_port__fec_only_info_s mpls_port_fec_info;

    /*create primary pw mpls tunnel fec {*/
    /* create failover id for PWE */
    rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &vswitch_vpls_hvpls_info.primary_pwe_mpls_failover_id_fec);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for primary PWE mpls tunnel FEC, rv - %d\n", rv); 
        return rv;
    }
    printf("primary PWE mpls tunnel Failover id: 0x%x\n", vswitch_vpls_hvpls_info.primary_pwe_mpls_failover_id_fec);

    mpls_port_fec_info.port_id = BCM_GPORT_INVALID;
    mpls_port_fec_info.failover_id = vswitch_vpls_hvpls_info.primary_pwe_mpls_failover_id_fec;
    mpls_port_fec_info.failover_port_id = 0;/* this is secondary*/
    BCM_L3_ITF_SET(mpls_port_fec_info.encap_id, BCM_L3_ITF_TYPE_LIF, vswitch_vpls_hvpls_info.pwe_outlif_primary);
    mpls_port_fec_info.egress_tunnel_if = vswitch_vpls_hvpls_info.backup_pw_ldp_fec;

    rv = mpls_port__fec_only__create(unit, &mpls_port_fec_info);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_port__fec_only__create 1\n"); 
        return rv;
    }

    mpls_port_fec_info.failover_port_id = mpls_port_fec_info.gport;
    BCM_L3_ITF_SET(mpls_port_fec_info.encap_id, BCM_L3_ITF_TYPE_LIF, vswitch_vpls_hvpls_info.pwe_outlif_primary);
    mpls_port_fec_info.egress_tunnel_if = vswitch_vpls_hvpls_info.primary_pw_ldp_fec;
    mpls_port_fec_info.gport = 0;

    rv = mpls_port__fec_only__create(unit, &mpls_port_fec_info);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_port__fec_only__create 1\n"); 
        return rv;
    }
    
    vswitch_vpls_hvpls_info.primary_pwe_fec = mpls_port_fec_info.gport;
    /*create primary pw mpls tunnel fec }*/

    /*create backup pw mpls tunnel fec {*/
    /* create failover id for PWE */
    rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &vswitch_vpls_hvpls_info.backup_pwe_mpls_failover_id_fec);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for backup PWE mpls tunnel FEC, rv - %d\n", rv); 
        return rv;
    }
    printf("backup PWE mpls tunnel Failover id: 0x%x\n", vswitch_vpls_hvpls_info.backup_pwe_mpls_failover_id_fec);

    mpls_port_fec_info.gport = 0;
    mpls_port_fec_info.port_id = BCM_GPORT_INVALID;
    mpls_port_fec_info.failover_id = vswitch_vpls_hvpls_info.backup_pwe_mpls_failover_id_fec;
    mpls_port_fec_info.failover_port_id = 0;/* this is secondary*/
    BCM_L3_ITF_SET(mpls_port_fec_info.encap_id, BCM_L3_ITF_TYPE_LIF, vswitch_vpls_hvpls_info.pwe_outlif_protected);
    mpls_port_fec_info.egress_tunnel_if = vswitch_vpls_hvpls_info.backup_pw_ldp_fec;

    rv = mpls_port__fec_only__create(unit, &mpls_port_fec_info);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_port__fec_only__create 1\n"); 
        return rv;
    }

    mpls_port_fec_info.failover_port_id = mpls_port_fec_info.gport;/* this is secondary*/
    BCM_L3_ITF_SET(mpls_port_fec_info.encap_id, BCM_L3_ITF_TYPE_LIF, vswitch_vpls_hvpls_info.pwe_outlif_protected);
    mpls_port_fec_info.egress_tunnel_if = vswitch_vpls_hvpls_info.primary_pw_ldp_fec;
    mpls_port_fec_info.gport = 0;

    rv = mpls_port__fec_only__create(unit, &mpls_port_fec_info);
    if (rv != BCM_E_NONE) {
        printf("Error, mpls_port__fec_only__create 1\n"); 
        return rv;
    }
    
    vswitch_vpls_hvpls_info.backup_pwe_fec = mpls_port_fec_info.gport;
    /*create backup pw mpls tunnel fec }*/

    return rv;  
    
}

/*
 * Create mpls-port: created using separate ingress, egress and FEC only calls 
 * Also creates ECMP group. 
 */
int
vswitch_vpls_hvpls_add_network_port_1(int unit){
    int rv;

    rv = vswitch_vpls_hvpls_egress_mpls_port_create(unit);
    if (rv != BCM_E_NONE) {
        printf("vswitch_vpls_hvpls_egress_mpls_port_create failed: %d \n", rv);
        return rv;
    }

    rv = vswitch_vpls_hvpls_fec_mpls_port_create(unit);
    if (rv != BCM_E_NONE) {
        printf("vswitch_vpls_hvpls_fec_mpls_port_create failed: %d \n", rv);
        return rv;
    }

    rv = vswitch_vpls_hvpls_ingress_mpls_port_create(unit);
    if (rv != BCM_E_NONE) {
        printf("vswitch_vpls_hvpls_ingress_mpls_port_create failed: %d \n", rv);
        return rv;
    }
   return rv;
}


int field_3_level_protection_tcam_group_set(int unit, int priority)
{
    bcm_field_qset_t  qset;
    bcm_field_aset_t  aset;
    bcm_field_group_t group;
    int rv;

    BCM_FIELD_QSET_INIT(qset);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyStageIngress);
    BCM_FIELD_QSET_ADD(qset, bcmFieldQualifyDstL3Egress);

    rv = bcm_field_group_create(unit, qset, priority, &group);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_create failed \n");
        return rv;
    }
    
    BCM_FIELD_ASET_INIT(aset);
    BCM_FIELD_ASET_ADD(aset, bcmFieldActionRedirect);
    
    rv = bcm_field_group_action_set(0, group, aset);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_action_set failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_group_install(0, group);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_group_install failed: %d\n", rv);
        return rv;
    }

    pw_protection_state_resolve_group = group;

    return rv;
}

int field_3_level_protection_tcam_entry_create(int unit, bcm_if_t if_id, uint32 state)
{
    bcm_field_entry_t ent;
    bcm_gport_t fec_dest;
    int fec_id;
    int ps_bit = 1; /* FEC bit 1 is the protection state bit */
    int rv;
    int update = (pw_protection_ent == -1)?0:1;

    if (state >1) { /* state: 0/1 */
        return BCM_E_PARAM;
    }

    if (update) {
       ent = pw_protection_ent;
    } else {
        rv = bcm_field_entry_create(0, pw_protection_state_resolve_group, &ent);
        if (rv != BCM_E_NONE) {
            printf("bcm_field_entry_create failed: %d\n", rv);
            return rv;
        }

        rv = bcm_field_qualify_DstL3Egress(unit, ent, if_id);
        if (rv != BCM_E_NONE) {
            printf("Error in bcm_field_qualify_DstL3Egress: %d\n", rv);
            return rv;
        }
    }

    if (state) {
        fec_id = BCM_GPORT_MPLS_PORT_ID_GET(vswitch_vpls_hvpls_info.primary_pwe_fec);
    } else {
        fec_id = BCM_GPORT_MPLS_PORT_ID_GET(vswitch_vpls_hvpls_info.backup_pwe_fec);
    }

    BCM_GPORT_FORWARD_PORT_SET(fec_dest, fec_id);

    rv = bcm_field_action_add(0, ent, bcmFieldActionRedirect, 0, fec_dest);
    if (rv != BCM_E_NONE) {
        printf("bcm_field_action_add failed: %d\n", rv);
        return rv;
    }

    rv = bcm_field_entry_install(0, ent);
    
    if (rv != BCM_E_NONE) {
        printf("bcm_field_entry_install failed: %d\n", rv);
        return rv;
    }

    if (!update) {
       pw_protection_ent = ent;
    }
    return rv;
}

/* 
 * This function ctrates VPLS hvpls configuration. 
 * In contrary to the known VPLS sequence, here we first create LL, then mpls_port and only afterwards the MPLS tunnels.
 */
int
vswitch_vpls_hvpls_run(int unit){

    int nof_outer_tpids;
    int nof_inner_tpids;
    bcm_mpls_vpn_config_t vpn_info;
    bcm_vlan_t  vpn;
    int rv;
    int trap_code;
    bcm_rx_trap_config_t trap_config;
    int native_ingress_intf; /* l3_intf for native routing. */
    int CINT_NO_FLAGS = 0;
    bcm_if_t ll_intfs;
    int ll_encap_id;
    bcm_multicast_t mc_id;
    bcm_multicast_replication_t reps[5] = {{0}};
  

    mpls_termination_label_index_enable = soc_property_get(unit , "mpls_termination_label_index_enable",0);
    if (pwe_cw) {
        rv = bcm_switch_control_set(unit, bcmSwitchMplsPWControlWord, pwe_cw);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_switch_control_set\n");
            return rv;
        }
    }
  
    if (verbose1) {
        printf("vswitch_vpls_hvpls_run %d\n", vswitch_vpls_info_1.vpn_id);
    }

    port_tpid_init(vswitch_vpls_info_1.pwe_in_port,1,0);
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set\n");
        return rv;
    }
    port_tpid_init(vswitch_vpls_info_1.ac_in_port,1,1);
    rv = port_tpid_set(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, port_tpid_set\n");
        return rv;
    }

    /* In advanced vlan translation mode, the default ingress/ egress actions and mapping
        are not configured. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        rv = vlan_translation_default_mode_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_translation_default_mode_init\n");
            return rv;
        }
    }
   
    /* Init vlan */  
    vlan__init_vlan(unit,vswitch_vpls_info_1.ac_port1_outer_vlan);
    vlan__init_vlan(unit,vswitch_vpls_info_1.ac_port1_inner_vlan);

    if (verbose1) {
        printf("vswitch_vpls_hvpls_run create vswitch\n");
    }
    /* create vswitch */
    rv = mpls_port__vswitch_vpls_vpn_create__set(unit, vswitch_vpls_info_1.vpn_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in vswitch_vpls_vpn_create\n");
        return rv;
    }

    create_l3_egress_s create_l3_egress_encap_params;
    create_l3_egress_encap_params.vlan = vswitch_vpls_hvpls_info.vlan;
    create_l3_egress_encap_params.next_hop_mac_addr = vswitch_vpls_hvpls_info.mac;

    /* create LL for VPLS tunnel */
    /* switching over PWE */
    rv = l3__egress_only_encap__create(unit, &create_l3_egress_encap_params);
    if (rv != BCM_E_NONE) {
        printf("Error, in l3__egress_only_encap__create\n");
        return rv;
    }

    vswitch_vpls_hvpls_info.ll_intf_id = create_l3_egress_encap_params.arp_encap_id;

    if (verbose1) {
        printf("Configured LL: 0x%08x\n", vswitch_vpls_hvpls_info.ll_intf_id);
    }

    /* For hvpls first the MPLS TUNNEL should be configured and only then the MPLS PORT */
    /* configure MPLS tunnels */
    rv  = mpls_tunnels_config(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_tunnels_config\n");
        return rv;
    }

    /*primary pw ldp tunnel fec {*/
    /* create failover id for primary pw mpls tunnel */
    rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &vswitch_vpls_hvpls_info.primary_pw_ldp_failover_id_fec);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for MPLS FEC, rv - %d\n", rv);
        return rv;
    }
    printf("primary pw LDP Failover id: 0x%x\n", vswitch_vpls_hvpls_info.primary_pw_ldp_failover_id_fec);


    /* create egress object that points to the newly created tunnel */
    /* this function creates FEC and LL */
    /* FEC is created with CASCADED flag to indicate hierarchy no. 2 */
    create_l3_egress_s create_l3_egress_params;

    /* Create protected FEC */
    create_l3_egress_params.l3_flags = BCM_L3_CASCADED|BCM_L3_ENCAP_SPACE_OPTIMIZED; /* MPLS FEC must be optimized (include EEI) */
    create_l3_egress_params.out_gport = mpls_lsr_info_1.eg_port;  /* outgoing port */
    create_l3_egress_params.vlan = mpls_lsr_info_1.eg_vid; 
    create_l3_egress_params.out_tunnel_or_rif = vswitch_vpls_hvpls_info.primary_pw_ldp_tunnel_id_protected;
    create_l3_egress_params.next_hop_mac_addr = mpls_lsr_info_1.next_hop_mac;  /* next hop mac */
    create_l3_egress_params.failover_id = vswitch_vpls_hvpls_info.primary_pw_ldp_failover_id_fec;  
    create_l3_egress_params.failover_if_id = 0;

    rv = l3__egress__create(unit, &create_l3_egress_params); 
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
      return rv;
    } 

    vswitch_vpls_hvpls_info.primary_pw_ldp_fec = create_l3_egress_params.fec_id;

    /* Create primary FEC */
    create_l3_egress_params.out_tunnel_or_rif = vswitch_vpls_hvpls_info.primary_pw_ldp_tunnel_id_primary;
    create_l3_egress_params.failover_if_id = vswitch_vpls_hvpls_info.primary_pw_ldp_fec;  
    create_l3_egress_params.arp_encap_id = 0;

    rv = l3__egress__create(unit, &create_l3_egress_params); 
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
      return rv;
    } 

    vswitch_vpls_hvpls_info.primary_pw_ldp_fec = create_l3_egress_params.fec_id;

    /*primary pw ldp tunnel fec }*/


    /*backup pw ldp tunnel fec {*/
    /* create failover id for primary pw mpls tunnel */
    rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &vswitch_vpls_hvpls_info.backup_pw_ldp_failover_id_fec);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for MPLS FEC, rv - %d\n", rv);
        return rv;
    }
    printf("backup pw LDP Failover id: 0x%x\n", vswitch_vpls_hvpls_info.backup_pw_ldp_failover_id_fec);


    /* create egress object that points to the newly created tunnel */
    /* this function creates FEC and LL */
    /* FEC is created with CASCADED flag to indicate hierarchy no. 2 */

    /* Create protected FEC */
    create_l3_egress_params.l3_flags = BCM_L3_CASCADED|BCM_L3_ENCAP_SPACE_OPTIMIZED; /* MPLS FEC must be optimized (include EEI) */
    create_l3_egress_params.out_gport = mpls_lsr_info_1.eg_port;  /* outgoing port */
    create_l3_egress_params.vlan = mpls_lsr_info_1.eg_vid; 
    create_l3_egress_params.out_tunnel_or_rif = vswitch_vpls_hvpls_info.backup_pw_ldp_tunnel_id_protected;
    create_l3_egress_params.next_hop_mac_addr = mpls_lsr_info_1.next_hop_mac;  /* next hop mac */
    create_l3_egress_params.failover_id = vswitch_vpls_hvpls_info.backup_pw_ldp_failover_id_fec;  
    create_l3_egress_params.failover_if_id = 0;
    create_l3_egress_params.arp_encap_id = 0;

    rv = l3__egress__create(unit, &create_l3_egress_params); 
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
      return rv;
    } 

    vswitch_vpls_hvpls_info.backup_pw_ldp_fec = create_l3_egress_params.fec_id;

    /* Create primery FEC */
    create_l3_egress_params.out_tunnel_or_rif = vswitch_vpls_hvpls_info.backup_pw_ldp_tunnel_id_primary;
    create_l3_egress_params.failover_if_id = vswitch_vpls_hvpls_info.backup_pw_ldp_fec;  
    create_l3_egress_params.arp_encap_id = 0;

    rv = l3__egress__create(unit, &create_l3_egress_params); 
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
      return rv;
    } 

    vswitch_vpls_hvpls_info.backup_pw_ldp_fec = create_l3_egress_params.fec_id;

    /*backup pw ldp tunnel fec }*/


    if (verbose1) {
        printf("vswitch_vpls_hvpls_run add vlan access port\n");
    }

    /* add mpls access port */
    rv = vswitch_vpls_add_access_port_1(unit, -1/*second_unit*/, &vswitch_vpls_info_1.access_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in vswitch_vpls_add_access_port_1\n");
        return rv;
    }

    if (verbose1) {
        printf("vswitch_vpls_hvpls_run add mpls network port\n");
    }
    /* add mpls network port */
    rv = vswitch_vpls_hvpls_add_network_port_1(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in vswitch_vpls_hvpls_add_network_port_1\n");
        return rv;
    }

    if (verbose1) {
        printf("vswitch_vpls_hvpls_run add mpls tunnel\n");
    }

    /* Egress Multicast configuration */
    mc_id = default_vpn_id;
    bcm_multicast_replication_t_init(reps);
    bcm_multicast_replication_t_init(reps + 1);
    bcm_multicast_replication_t_init(reps + 2);
    bcm_multicast_replication_t_init(reps + 3);

    /* AC outlif 1 */
    BCM_GPORT_LOCAL_SET(reps[0].port, vswitch_vpls_info_1.ac_in_port);
    reps[0].encap1 = vlan_port_encap_id1;

    /* PWE outlif primery with second CUD for MPLS tunnel primery */
    BCM_GPORT_LOCAL_SET(reps[1].port, vswitch_vpls_info_1.pwe_out_port);
    reps[1].encap1 = vswitch_vpls_hvpls_info.pwe_outlif_primary;
    reps[1].encap2 = BCM_L3_ITF_VAL_GET(vswitch_vpls_hvpls_info.primary_pw_ldp_tunnel_id_primary);
    reps[1].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID;

    /* PWE outlif primery with second CUD for MPLS tunnel protected */
    BCM_GPORT_LOCAL_SET(reps[2].port, vswitch_vpls_info_1.pwe_out_port);
    reps[2].encap1 = vswitch_vpls_hvpls_info.pwe_outlif_primary;
    reps[2].encap2 = BCM_L3_ITF_VAL_GET(vswitch_vpls_hvpls_info.primary_pw_ldp_tunnel_id_protected);
    reps[2].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID;

    /* PWE outlif protected with second CUD for MPLS tunnel primery */
    BCM_GPORT_LOCAL_SET(reps[3].port, vswitch_vpls_info_1.pwe_out_port);
    reps[3].encap1 = vswitch_vpls_hvpls_info.pwe_outlif_protected;
    reps[3].encap2 = BCM_L3_ITF_VAL_GET(vswitch_vpls_hvpls_info.backup_pw_ldp_tunnel_id_primary);
    reps[3].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID;

    /* PWE outlif protected with second CUD for MPLS tunnel protected */
    BCM_GPORT_LOCAL_SET(reps[4].port, vswitch_vpls_info_1.pwe_out_port);
    reps[4].encap1 = vswitch_vpls_hvpls_info.pwe_outlif_protected;
    reps[4].encap2 = BCM_L3_ITF_VAL_GET(vswitch_vpls_hvpls_info.backup_pw_ldp_tunnel_id_protected);
    reps[4].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID;

    bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID | BCM_MULTICAST_EGRESS_GROUP, &mc_id);
    bcm_multicast_set(unit, mc_id, 0, 5, reps);

    return rv;
}



int
vswitch_vpls_3level_protection_run_with_defaults(int unit){
    int rv = BCM_E_NONE;
    bcm_if_t  pw_fec_lif;

    rv = mpls_port__init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_port__init\n");
        return rv;
    }

    if (!is_device_or_above(unit,JERICHO)) {
        printf("Error, Hierarchical VPLS is supported only from Jericho \n");
        return -1;
    }

    vswitch_vpls_hvpls_3level_info_init(unit,1,13,13,10,20,6202);
    
    vswitch_vpls_hvpls_run(unit);

     field_3_level_protection_tcam_group_set(unit,10);

     BCM_L3_ITF_SET(pw_fec_lif,BCM_L3_ITF_TYPE_FEC,BCM_GPORT_MPLS_PORT_ID_GET(vswitch_vpls_hvpls_info.backup_pwe_fec));
     field_3_level_protection_tcam_entry_create(unit,pw_fec_lif,1);

    
}

int
vswitch_vpls_3level_protection_run_with_defaults_dvapi(int unit, int acP, int pweP){
    int rv = BCM_E_NONE;
    bcm_if_t  pw_fec_lif;

    rv = mpls_port__init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_port__init\n");
        return rv;
    }

    if (!is_device_or_above(unit,JERICHO)) {
        printf("Error, Hierarchical VPLS is supported only from Jericho \n");
        return -1;
    }

    vswitch_vpls_hvpls_3level_info_init(unit,acP,pweP,pweP,10,20,6202);
    
    vswitch_vpls_hvpls_run(unit);

    field_3_level_protection_tcam_group_set(unit,10);

    BCM_L3_ITF_SET(pw_fec_lif,BCM_L3_ITF_TYPE_FEC,BCM_GPORT_MPLS_PORT_ID_GET(vswitch_vpls_hvpls_info.backup_pwe_fec));
    field_3_level_protection_tcam_entry_create(unit,pw_fec_lif,1);
}


/*
 * Failover Set
 */
int vswitch_vpls_hvpls_lsp_failover_set(int unit, int primary_pw, int state){
    int rv;
    bcm_failover_t failover_id, egress_failover_id;
    int egress_enable;

    failover_id = primary_pw? vswitch_vpls_hvpls_info.primary_pwe_mpls_failover_id_fec : vswitch_vpls_hvpls_info.backup_pwe_mpls_failover_id_fec;

    rv = bcm_failover_set(unit, failover_id, state);    
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_failover_set failover_id:  0x%08x \n", failover_id);
        return rv;
    }
    return rv;
}

/*
 * Failover Set
 */
int vswitch_vpls_hvpls_ldp_failover_set(int unit, int primary, int state){
    int rv;
    bcm_failover_t failover_id, egress_failover_id;
    int egress_enable;

    failover_id = primary? vswitch_vpls_hvpls_info.primary_pw_ldp_failover_id_fec : vswitch_vpls_hvpls_info.backup_pw_ldp_failover_id_fec;
    egress_failover_id = primary? vswitch_vpls_hvpls_info.primary_pw_ldp_failover_id_outlif : vswitch_vpls_hvpls_info.backup_pw_ldp_failover_id_outlif;
   
    rv = bcm_failover_set(unit, failover_id, state);    
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_failover_set failover_id:  0x%08x \n", failover_id);
        return rv;
    }

    rv = bcm_failover_set(unit, egress_failover_id, state);    
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_failover_set failover_id:  0x%08x \n", failover_id);
        return rv;
    }
    return rv;
}

int vswitch_vpls_hvpls_pw_failover_set(int unit, int state){
    int rv;
    bcm_failover_t  egress_failover_id;
    int egress_enable;
    bcm_if_t  pw_fec_lif;

    egress_failover_id =  vswitch_vpls_hvpls_info.pwe_failover_id_outlif;

     BCM_L3_ITF_SET(pw_fec_lif,BCM_L3_ITF_TYPE_FEC,BCM_GPORT_MPLS_PORT_ID_GET(vswitch_vpls_hvpls_info.backup_pwe_fec));
     field_3_level_protection_tcam_entry_create(unit,pw_fec_lif,state);

    rv = bcm_failover_set(unit, egress_failover_id, state);    
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_failover_set failover_id:  0x%08x \n", egress_failover_id);
        return rv;
    }
    return rv;
}



