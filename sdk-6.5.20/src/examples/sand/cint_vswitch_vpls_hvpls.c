/*~~~~~~~~~~~~~~~~~~~~~~~Mulitpoint VPLS Service~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File: cint_vswitch_vpls_hvpls.c
 * Purpose: Example of Open Multi-Point VPLS service with Hierarchical PWE-MPLS protection.
 * Hierarchical PWE-MPLS protection provides the possibility to support both PWE 1:1 protection
 * and MPLS 1:1 protection i.e. 2-layer protection.
 * This feature is supported from Jericho / Q-MX and above.
 * 
 * Note: Current implementation supports unicast packets only.
 *
 * 
 *   Following figure shows HW model for Hierarchical VPLS:
 *   Format A (mpls_port_eei = 0):
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *|                                           Ingress-Egress           EEDB            |
 *|                                 Outlif           |     +----+  +----------+        |
 *|                        ------------------------------->|- - |->|   PWE    |        |
 *|                        |                         |     |    |  |----------|        |
 *| +-------+   FEC    +------+  FEC MPLS  +------+ EEI    |GLEM|  |          |        |
 *| |       |   PWE    |      |----------->|      |(Outlif)|    |  |----------|        |                              
 *| | MACT  | -------->| FEC  |            | FEC  |------->|- - |->|   MPLS   |- |     | 
 *| |       |          | PWE  |            | MPLS |  |     |    |  |----------|  |     |
 *| +-------+          +------+            +------+        +----+  |          |  |     |
 *|                                                                |          |  |     | 
 *|                                                                |----------|  |     |
 *|                                                                |Link-Layer|<-|     |  
 *|                                                                +----------+        |
 *|                                                                                    |        
 *|------------------------------------------------------------------------------------|
 * 
 * 
 *   Format B (mpls_port_eei = 1):
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *|                                           Ingress-Egress           EEDB            |
 *|                                 EEI (Outlif)     |     +----+  +----------+        |
 *|                        ------------------------------->|- - |->|   PWE    |        |
 *|                        |                         |     |    |  |----------|        |
 *| +-------+   FEC    +------+  FEC MPLS  +------+  |     |GLEM|  |          |        |
 *| |       |   PWE    |      |----------->|      | Outlif |    |  |----------|        |                              
 *| | MACT  | -------->| FEC  |            | FEC  |------->|- - |->|   MPLS   |- |     | 
 *| |       |          | PWE  |            | MPLS |  |     |    |  |----------|  |     |
 *| +-------+          +------+            +------+        +----+  |          |  |     |
 *|                                                                |          |  |     | 
 *|                                                                |----------|  |     |
 *|                                                                |Link-Layer|<-|     |  
 *|                                                                +----------+        |
 *|                                                                                    |        
 *|------------------------------------------------------------------------------------|
 * 
 *
 * Headers:
 *
 * Primary path:
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |  | DA |SA||TIPD1 |Prio|VID||  MPLS    |  MPLS    ||   PWE    ||Eth||Data ||
 *   |  |0:11|  ||0x8847|    |100||Label:2000|Label:1000||Lable:2010||   ||     || 
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |                +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                  |
 *   |                | Figure 1 : Packets Received from PWE  |                  | 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    |
 *   |  |DA|SA||TIPD1 |Prio|VID||  MPLS    |  MPLS    ||   PWE    ||Eth||Data |  |
 *   |  |  |  ||0x8847|    |200||Label:2000|Label:1000||Lable:1982||   ||     |  | 
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    |
 *   |                +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                 |
 *   |                | Figure 2 : Packets Transmitted to PWE  |                 | 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
 *
 * Protected path:
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |  | DA |SA||TIPD1 |Prio|VID||  MPLS    |  MPLS    ||   PWE    ||Eth||Data ||
 *   |  |0:11|  ||0x8847|    |100||Label:2001|Label:1001||Lable:2020||   ||     || 
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |                +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                  |
 *   |                | Figure 1 : Packets Received from PWE  |                  | 
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+    |
 *   |  |DA|SA||TIPD1 |Prio|VID||  MPLS    |  MPLS    ||   PWE    ||Eth||Data |  |
 *   |  |  |  ||0x8847|    |200||Label:2001|Label:1001||Lable:1992||   ||     |  | 
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
 *  -   Set up MPLS tunnels. Refer to mpls_tunnels_config().
 *      -   Set MPLS L2 termination (on ingress).
 *          -    MPLS packets arriving with DA 00:00:00:00:00:11 and VID 100 causes L2 termination.  
 *          -    Calls bcm_l3_intf_create().
 *      -   Add pop entries to MPLS switch. Refer to mpls_add_pop_entry.
 *          -    Calls bcm_mpls_tunnel_switch_create().
 *          -    Create MPLS L3 interface (on egress).
 *          -    Packet routed to this L3 interface is set with this MAC. 
 *          -    Calls bcm_l3_intf_create().
 *      -   Create MPLs tunnels over the created l3 interface. Refer to mpls__create_tunnel_initiator__set().
 *  -   Create cascaded FEC object for MPLS protection.
 *          -    Calls l3__egress__create().
 *  -   Add AC and MPLS-ports to the VSI.
 *      -   vswitch_vpls_add_access_port_1 creates attachment circuit and add them to the Vswitch and 
 *          update the Multicast group.
 *      -   vswitch_vpls_add_network_port_1 creates PWE and add them to the VSI and update the 
 *          multicast group. This call consists of 3 parts which are 3 calls to bcm_mpls_port_add:
 *          - Egress only : PWE Outlif
 *          - FEC only : PWE Protection FEC pointing to MPLS cascaded FEC and PWE outlif
 *          - Ingress only : PWE termination
 *
 * -   Support for MC ID as destination of FEC:
 *     -   This feature will allow MC ID as destination of FEC, which means, when FEC is resolved, the PP will
 *         get a MC ID. Packet will finally be replicated based MC DB. 
 *     -   Calling API:   vswitch_vpls_mc_in_fec_run_with_defaults_dvapi(0,13,13);
 *     -   Traffic : sending packet from PW side for mac learning, then send unicast traffic from AC side. The
 *         should hit MAC table and be replicated based MC group . 
 *
 * Remarks:
 *  -   To set the protection properties use vswitch_vpls_hvpls_failover_set
 *
 * To Activate Above Settings Run:
 *      BCM> cint examples/dpp/cint_port_tpid.c
 *      BCM> cint examples/dpp/cint_advanced_vlan_translation_mode.c
 *      BCM> cint examples/dpp/cint_qos.c
 *      BCM> cint examples/dpp/cint_mpls_lsr.c
 *      BCM> cint examples/dpp/cint_vswitch_metro_mp.c
 *      BCM> cint examples/sand/cint_vswitch_vpls.c
 *      BCM> cint examples/dpp/utility/cint_utils_mpls_port.c
 *      BCM> cint examples/sand/utility/cint_sand_utils_mpls_port.c
 *  	BCM> cint examples/sand/utility/cint_sand_utils_mpls.c
 *  	BCM> cint examples/dpp/utility/cint_utils_multicast.c
 *      BCM> cint examples/dpp/cint_vswitch_vpls_hvpls.c 
 *      BCM> cint
 *      cint> int rv;
 *      cint> rv = vswitch_vpls_hvpls_run_with_defaults(unit, second_unit, extend_example); 
 */

int mc_in_fec  = 0;/*whether use a MC group as destination of FEC entry*/
int mpls_port_eei = 0; /* If 0 MPLS PORT FEC destination is OutLif & MPLS tunnel FEC destination is EEI, 
                          else MPLS PORT FEC destination is EEI & MPLS tunnel FEC destination is OutLif */

struct cint_vswitch_vpls_hvpls_info_s {

    /* tunnels info */
    int in_tunnel_label_primary;
    int in_tunnel_label_protected;
    int out_tunnel_label_primary;
    int out_tunnel_label_protected;

    bcm_if_t mpls_fec; /* mpls FEC */	
    bcm_gport_t pwe_fec; /* PWE FEC */	

    bcm_if_t mpls_tunnel_id_primary; /* Interface id of MPLS tunnel */	
    bcm_if_t mpls_tunnel_id_protected; /* Interface id of MPLS tunnel */	

    int ll_intf_id; /* LL interface id */

    int vlan; /* vlan in Core direction. For vpls LL */

    int pwe_outlif_primary; /* outlifs are configured with id in this mode. */
    int pwe_outlif_protected; /* outlifs are configured with id in this mode. */

    int in_vc_label_primary; /* pwe label primary */
    int in_vc_label_protected; /* pwe label protected */
    int eg_vc_label_primary; /* pwe label primary */
    int eg_vc_label_protected; /* pwe label protected */

    uint8 mac[6]; /* next hop for vpls LL */

    bcm_failover_t pwe_failover_id_fec;
    bcm_failover_t pwe_failover_id_outlif;
    bcm_failover_t mpls_failover_id_fec;
    bcm_failover_t mpls_failover_id_outlif;

    /* misc configurations */
    int use_two_calls;


    bcm_gport_t mpls_out_port_id; /* pwe mpls out port primary */
    bcm_gport_t mpls_port_id; /* pwe mpls port primary */

};

cint_vswitch_vpls_hvpls_info_s vswitch_vpls_hvpls_info;

void
vswitch_vpls_hvpls_info_init(int unit, int ac_port, int pwe_in_port, int pwe_out_port, int ac_port1_outer_vlan, int ac_port1_inner_vlan, int vpn_id, int two_calls) {

    int i,rc;
    int is_qux;

    vswitch_vpls_hvpls_info.use_two_calls = two_calls;

    vswitch_vpls_hvpls_info.vlan = 200;

    rc = is_qumran_ux_only(unit, &is_qux);
    if (rc != BCM_E_NONE) {
        printf("Error, is_qumran_ux_only()\n");
        return rc;
    }

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
        
    vswitch_vpls_info_1.ac_port1_outer_vlan = ac_port1_outer_vlan;
    vswitch_vpls_info_1.ac_port1_inner_vlan = ac_port1_inner_vlan;

    if(mc_in_fec) {
       vswitch_vpls_info_2.ac_in_port = ac_port;
       vswitch_vpls_info_1.ac_port2_outer_vlan = ac_port1_outer_vlan+5;
       vswitch_vpls_info_1.ac_port2_inner_vlan = ac_port1_inner_vlan+5;
    }
    vswitch_vpls_info_1.ac_port2_outer_vlan = ac_port1_outer_vlan+5;
    vswitch_vpls_info_1.access_index_1 = 2;
    vswitch_vpls_info_1.access_index_2 = 3;
    vswitch_vpls_info_1.access_index_3 = 3;

    /* set mpls tunneling information with default values */
    mpls_lsr_init(vswitch_vpls_info_1.pwe_in_port, vswitch_vpls_info_1.pwe_out_port, 0x11, 0x22, 0/*in_label*/, 0/*out_label*/, 100, 200, 0);
    mpls_lsr_info_1.eg_port = vswitch_vpls_info_1.pwe_out_port;

    /* 
     * MC with two pointers are needed, if not QAX, must use egress replicator
     * JR2 support MC with two pointers by PPMC. but in consideration of inter-operation with JR1,
     * we use egress replicator always
     */
    egress_mc = (is_device_or_above(unit,QUMRAN_AX) && !is_qux) ? 0 : 1;
    egress_mc = is_device_or_above(unit, JERICHO2) ? 1 : egress_mc;


    /* Init utility functions cint */
    mpls__init(unit);

}

/* 
 * This function configures MPLS tunnels on already existing PWE encapsulation DB entries.
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
    mpls_termination_label_index_database_mode = soc_property_get(unit, spn_BCM886XX_MPLS_TERMINATION_DATABASE_MODE, 2); 
    
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

    /* Create an MPLS Tunnel Egress failover ID */
    rv = bcm_failover_create(unit, BCM_FAILOVER_ENCAP, &vswitch_vpls_hvpls_info.mpls_failover_id_outlif);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for MPLS Egress, rv - %d\n", rv);
        return rv;
    }
    printf("MPLS Tunnel egress Failover id: 0x%x\n", vswitch_vpls_hvpls_info.mpls_failover_id_outlif);

    /* set tunnel over this l3 interface, so packet forwarded to this interface will be tunneled */
    /* Protected */
    mpls_tunnel_properties.tunnel_id = 0;
    mpls_tunnel_properties.next_pointer_intf = is_device_or_above(unit, JERICHO2) ? 0 : ingress_intf;
    mpls_tunnel_properties.label_in =  vswitch_vpls_hvpls_info.in_tunnel_label_protected;
    mpls_tunnel_properties.label_out =  vswitch_vpls_hvpls_info.out_tunnel_label_protected;
    mpls_tunnel_properties.egress_failover_id = vswitch_vpls_hvpls_info.mpls_failover_id_outlif;
    mpls_tunnel_properties.egress_failover_if_id = 0;
    mpls_tunnel_properties.flags = BCM_MPLS_EGRESS_LABEL_PROTECTION;
    mpls_tunnel_properties.flags_out = BCM_MPLS_EGRESS_LABEL_PROTECTION;

    rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__create_tunnel_initiator__set\n");
        return rv;
    }

    vswitch_vpls_hvpls_info.mpls_tunnel_id_protected = mpls_tunnel_properties.tunnel_id;

    if (verbose1) {
        printf("MPLS tunnel: add protected tunnel id 0x%08x\n\r", mpls_tunnel_properties.tunnel_id);
    }

    /* Primary */
    mpls_tunnel_properties.next_pointer_intf = is_device_or_above(unit, JERICHO2) ? 0 : ingress_intf;
    mpls_tunnel_properties.label_in =  vswitch_vpls_hvpls_info.in_tunnel_label_primary;
    mpls_tunnel_properties.label_out =  vswitch_vpls_hvpls_info.out_tunnel_label_primary;
    mpls_tunnel_properties.egress_failover_id = vswitch_vpls_hvpls_info.mpls_failover_id_outlif;
    mpls_tunnel_properties.egress_failover_if_id = mpls_tunnel_properties.tunnel_id;
    mpls_tunnel_properties.tunnel_id = 0;
    mpls_tunnel_properties.flags = BCM_MPLS_EGRESS_LABEL_PROTECTION;
    mpls_tunnel_properties.flags_out = BCM_MPLS_EGRESS_LABEL_PROTECTION;

    rv = mpls__create_tunnel_initiator__set(unit, &mpls_tunnel_properties);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls__create_tunnel_initiator__set\n");
        return rv;
    }

    vswitch_vpls_hvpls_info.mpls_tunnel_id_primary = mpls_tunnel_properties.tunnel_id;

    if (verbose1) {
        printf("MPLS tunnel: add primary tunnel id 0x%08x\n\r", mpls_tunnel_properties.tunnel_id);
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
    printf("bcm_mpls_tunnel_initiator_get for Primary: tunnel_id - %d, egress_failover_id - %d, egress_failover_if_id - %d\n\r",
           label_array[0].tunnel_id, label_array[0].egress_failover_id, label_array[0].egress_failover_if_id);


    /* Adding pop entries for protected and primary tunnels */
    for (is_primary=0; is_primary<=1; is_primary++) {

        if (is_primary) {
            label_for_pop_entry_out = vswitch_vpls_hvpls_info.out_tunnel_label_primary;
            label_for_pop_entry_in = vswitch_vpls_hvpls_info.in_tunnel_label_primary;
        }
        else {
            label_for_pop_entry_out = vswitch_vpls_hvpls_info.out_tunnel_label_protected;
            label_for_pop_entry_in = vswitch_vpls_hvpls_info.in_tunnel_label_protected;
        }

        /* add switch entries to pop MPLS labels  */
        if (mpls_termination_label_index_enable) {
            int label_for_pop_entry_out_int = label_for_pop_entry_out;
            int label_for_pop_entry_in_int = label_for_pop_entry_in;

            if (mpls_termination_label_index_database_mode == 12) {
                BCM_MPLS_INDEXED_LABEL_SET(&label_for_pop_entry_out, &label_for_pop_entry_out_int, 1);
            } else {
                BCM_MPLS_INDEXED_LABEL_SET(&label_for_pop_entry_out, &label_for_pop_entry_out_int, vswitch_vpls_info_1.access_index_1);
            }
            BCM_MPLS_INDEXED_LABEL_SET(&label_for_pop_entry_in, &label_for_pop_entry_in_int, 1);
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

    /* 
     *  Ingress_only - primary and protected
     */
    for (is_primary=0; is_primary<=1; is_primary++) {
        bcm_mpls_port_t_init(&mpls_port_ingress);
        mpls_port_ingress.flags = BCM_MPLS_PORT_EGRESS_TUNNEL|BCM_MPLS_PORT_WITH_ID;
        /* In JR2,BCM_MPLS_PORT_NETWORK is not used, we use network_group_id directlt. */
        mpls_port_ingress.flags |= is_device_or_above(unit, JERICHO2) ? 0 : BCM_MPLS_PORT_NETWORK;
        /* In JR2,BCM_MPLS_PORT_ENCAP_WITH_ID can't be set with INGRESS_ONLY. */
        mpls_port_ingress.flags |= is_device_or_above(unit, JERICHO2) ? 0 : BCM_MPLS_PORT_ENCAP_WITH_ID;
        mpls_port_ingress.flags2 = BCM_MPLS_PORT2_INGRESS_ONLY; /* Indicate we only create PWE-In-LIF allocation */
        rv = mpls_port__update_network_group_id(unit, &mpls_port_ingress);
        if (rv != BCM_E_NONE) {
            printf("Error, mpls_port__update_network_group_id\n");
            return rv;
        }

        /* Set parameters for ingress mpls port */
        mpls_port_ingress.criteria = BCM_MPLS_PORT_MATCH_LABEL;
        if (mpls_termination_label_index_enable) {
            BCM_MPLS_INDEXED_LABEL_SET(&mpls_port_ingress.match_label,
                                       is_primary ? &vswitch_vpls_hvpls_info.in_vc_label_primary : &vswitch_vpls_hvpls_info.in_vc_label_protected,
                                       vswitch_vpls_info_1.access_index_2);

        }
        else {
            mpls_port_ingress.match_label = is_primary ? vswitch_vpls_hvpls_info.in_vc_label_primary : vswitch_vpls_hvpls_info.in_vc_label_protected;
        }

        mpls_port_ingress.port = BCM_GPORT_INVALID; /* Port is not required for PWE-InLIF */
        mpls_port_ingress.egress_tunnel_if = 0; /* PWE In-LIF in this case do not care about MPLS egress_if. It is not part of learning information. */

        mpls_port_ingress.failover_port_id = vswitch_vpls_hvpls_info.pwe_fec; /* Learning - PWE FEC */

        /* Only one inlif is configured. It should be equal to one of the Outlifs to get same-interface filtering (Multicast group member) */
        mpls_port_ingress.encap_id = is_primary?vswitch_vpls_hvpls_info.pwe_outlif_primary:vswitch_vpls_hvpls_info.pwe_outlif_protected; 
        if (is_device_or_above(unit, JERICHO2)) {
            /* Jr2 requires subtype of mpls_port_id.*/
            BCM_GPORT_SUB_TYPE_LIF_SET(mpls_port_ingress.mpls_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY, mpls_port_ingress.encap_id);
            mpls_port_ingress.encap_id = 0;
        }
        BCM_GPORT_MPLS_PORT_ID_SET(mpls_port_ingress.mpls_port_id, mpls_port_ingress.mpls_port_id);

        rv = bcm_mpls_port_add(unit, vswitch_vpls_shared_info_1.vpn, &mpls_port_ingress);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_mpls_port_add\n");
            return rv;
        }

        if (verbose1) {
            printf("INGRESS: add mpls port 0x%08x to vpn \n\r", mpls_port_ingress.mpls_port_id);
        }
        vswitch_vpls_hvpls_info.mpls_port_id = mpls_port_ingress.mpls_port_id;
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
    mpls_port_egress.flags = BCM_MPLS_PORT_EGRESS_TUNNEL | BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_ENCAP_WITH_ID;
    mpls_port_egress.flags |= is_device_or_above(unit, JERICHO2) ? 0 : BCM_MPLS_PORT_NETWORK;
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
    if (is_device_or_above(unit, JERICHO2)) {
        mpls_port_egress.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        mpls_port_egress.egress_label.egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
    } else {
        mpls_port_egress.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_SET;
    }

    /* ***** PWE outlif protected **** */
    mpls_port_egress.egress_failover_id = vswitch_vpls_hvpls_info.pwe_failover_id_outlif;
    mpls_port_egress.egress_failover_port_id = 0;
    mpls_port_egress.egress_label.label = vswitch_vpls_hvpls_info.eg_vc_label_protected;
    mpls_port_egress.encap_id = vswitch_vpls_hvpls_info.pwe_outlif_protected; /* encap id is the outlif */
    mpls_port__egress_tunnel_if_egress_only__set(unit, vswitch_vpls_info_1.tunnel_id, 1, &mpls_port_egress);
    /* mpls_port_id is the outlif id encapsulated as MPLS_PORT */
    if (is_device_or_above(unit, JERICHO2)) {
        /* Jr2 requires subtype of mpls_port_id.*/
        BCM_GPORT_SUB_TYPE_LIF_SET(mpls_port_egress.mpls_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, mpls_port_egress.encap_id);
    }
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port_egress.mpls_port_id, mpls_port_egress.mpls_port_id);

    if (verbose1) {
        printf("ADDING MPLS PORT EGRESS (protected): mpls_port_id: 0x%08x, encap_id: 0x%08x\n", mpls_port_egress.mpls_port_id, mpls_port_egress.encap_id);
    }

    rv = bcm_mpls_port_add(unit, 0, &mpls_port_egress);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add\n");
        return rv;
    }

    if (verbose1) {
        printf("EGRESS: add port 0x%08x/encap 0x%08x to vpn \n\r", mpls_port_egress.mpls_port_id, mpls_port_egress.encap_id);
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
    if (is_device_or_above(unit, JERICHO2)) {
        /* Jr2 requires subtype of mpls_port_id.*/
        BCM_GPORT_SUB_TYPE_LIF_SET(mpls_port_egress.mpls_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, mpls_port_egress.encap_id);
    }
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port_egress.mpls_port_id, mpls_port_egress.mpls_port_id);

    if (verbose1) {
        printf("ADDING MPLS PORT EGRESS (primary): mpls_port_id: 0x%08x, encap_id: 0x%08x\n", mpls_port_egress.mpls_port_id, mpls_port_egress.encap_id);
    }

    rv = bcm_mpls_port_add(unit, 0, &mpls_port_egress);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_mpls_port_add\n");
        return rv;
    }
    if (verbose1) {
        printf("EGRESS: add port 0x%08x/encap 0x%08x to vpn \n\r", mpls_port_egress.mpls_port_id, mpls_port_egress.encap_id);
    }

    vswitch_vpls_hvpls_info.mpls_out_port_id = mpls_port_egress.mpls_port_id;
    /* Adding egress mpls port to multicast because when egress and ingress are configured separately the multicast should point to the outlif */

    return rv;
}

/* 
 * Configure mpls port FEC attributes
 */
int
vswitch_vpls_hvpls_fec_mpls_port_create(int unit){
    int rv;
    bcm_if_t pwe_encap_id;
    bcm_multicast_t mc_group_id = 0;
    mpls_port__fec_only_info_s mpls_port_fec_info;

    /* create failover id for PWE */
    rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &vswitch_vpls_hvpls_info.pwe_failover_id_fec);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for PWE FEC, rv - %d\n", rv); 
        return rv;
    }
    printf("PWE Failover id: 0x%x\n", vswitch_vpls_hvpls_info.pwe_failover_id_fec);

    /* create two FEC entries, each point to <MPLS-tunnel-FEC,LIF> and share the same Failover-id*/
    if (is_device_or_above(unit, JERICHO2)) {
        /**JR2: l3_egress_create must be used to create FEC entris.*/
        create_l3_egress_s l3_egress;
        sal_memset(&l3_egress, 0, sizeof (l3_egress));
        /* this is secondary*/
        l3_egress.failover_id = vswitch_vpls_hvpls_info.pwe_failover_id_fec;
        l3_egress.failover_if_id = 0;
        l3_egress.out_tunnel_or_rif = 0;
        l3_egress.out_gport = BCM_GPORT_INVALID;
        if (mc_in_fec) {
            BCM_GPORT_MCAST_SET(l3_egress.out_gport, default_vpn_id);
        } else
        {
            BCM_L3_ITF_SET(l3_egress.out_tunnel_or_rif,BCM_L3_ITF_TYPE_LIF,  vswitch_vpls_hvpls_info.pwe_outlif_protected);
            BCM_GPORT_FORWARD_PORT_SET(l3_egress.out_gport, BCM_L3_ITF_VAL_GET(vswitch_vpls_hvpls_info.mpls_fec));
        }
        
        rv = l3__egress_only_fec__create(unit, &l3_egress);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress_only_fec__create 1\n"); 
            return rv;
        }

        vswitch_vpls_hvpls_info.pwe_fec = l3_egress.fec_id;

        /* this is primery*/
        l3_egress.out_tunnel_or_rif = 0;
        l3_egress.failover_if_id = vswitch_vpls_hvpls_info.pwe_fec;
        if (mc_in_fec) {
            BCM_GPORT_MCAST_SET(l3_egress.out_gport, default_vpn_id);
        } else 
        {
            BCM_L3_ITF_SET(l3_egress.out_tunnel_or_rif,BCM_L3_ITF_TYPE_LIF,  vswitch_vpls_hvpls_info.pwe_outlif_primary);
            BCM_GPORT_FORWARD_PORT_SET(l3_egress.out_gport, BCM_L3_ITF_VAL_GET(vswitch_vpls_hvpls_info.mpls_fec));
        }
        rv = l3__egress_only_fec__create(unit, &l3_egress);
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress_only_fec__create 1\n"); 
            return rv;
        }

    } else {
        /** JR1: mpls_port_add can be used to create FEC entries. Keep the sequence to show the differences.*/
        mpls_port_fec_info.port_id = BCM_GPORT_INVALID;
        mpls_port_fec_info.failover_id = vswitch_vpls_hvpls_info.pwe_failover_id_fec;
        mpls_port_fec_info.failover_port_id = 0;/* this is secondary*/
        BCM_L3_ITF_SET(mpls_port_fec_info.encap_id, BCM_L3_ITF_TYPE_LIF, vswitch_vpls_hvpls_info.pwe_outlif_protected);
        mpls_port_fec_info.egress_tunnel_if = vswitch_vpls_hvpls_info.mpls_fec;
        if (mc_in_fec) {
            BCM_MULTICAST_L2_SET(mc_group_id,default_vpn_id);
            mpls_port_fec_info.mc_group_id = mc_group_id;
        }
        mpls_port_fec_info.flags2 = mpls_port_eei?BCM_MPLS_PORT2_ENCAP_OPTIMIZED:0;

        rv = mpls_port__fec_only__create(unit, &mpls_port_fec_info);
        if (rv != BCM_E_NONE) {
            printf("Error, mpls_port__fec_only__create 1\n"); 
            return rv;
        }

        vswitch_vpls_hvpls_info.pwe_fec = mpls_port_fec_info.gport;

        BCM_L3_ITF_SET(mpls_port_fec_info.encap_id, BCM_L3_ITF_TYPE_LIF, vswitch_vpls_hvpls_info.pwe_outlif_primary);
        mpls_port_fec_info.failover_port_id = mpls_port_fec_info.gport;/* this is primery*/

        rv = mpls_port__fec_only__create(unit, &mpls_port_fec_info);
        if (rv != BCM_E_NONE) {
            printf("Error, mpls_port__fec_only__create 1\n"); 
            return rv;
        } 
    }

    return rv;
}


/* 
 * Configure mpls port attributes in single call
 */
int
vswitch_vpls_hvpls_mpls_port_single_call_create(int unit){
    int rv;
    bcm_mpls_port_t mpls_port;
    int modid;
    int modport;
    int is_primary;
    int failover_port_id = 0, egress_failover_port_id = 0;
    int tm_port;


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

    /* create failover id for PWE */
    rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &vswitch_vpls_hvpls_info.pwe_failover_id_fec);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for PWE FEC, rv - %d\n", rv);
        return rv;
    }
    printf("PWE Failover id: 0x%x\n", vswitch_vpls_hvpls_info.pwe_failover_id_fec);

    /* 
     *  Egress protection
     */

    /* create egress failover id for PWE */
    rv = bcm_failover_create(unit, BCM_FAILOVER_ENCAP, &vswitch_vpls_hvpls_info.pwe_failover_id_outlif);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for PWE Egress, rv - %d\n", rv);
        return rv;
    }
    printf("PWE egress Failover id: 0x%x\n", vswitch_vpls_hvpls_info.pwe_failover_id_outlif);


    /* ***** PWE outlif primery and protected **** */
    for (is_primary=0;is_primary<=1;is_primary++) {

        bcm_mpls_port_t_init(&mpls_port);
        mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;

        mpls_port.flags = BCM_MPLS_PORT_NETWORK|BCM_MPLS_PORT_EGRESS_TUNNEL;
        mpls_port.flags2 = BCM_MPLS_PORT2_EGRESS_PROTECTION|(mpls_port_eei?BCM_MPLS_PORT2_ENCAP_OPTIMIZED:0);

        rv = mpls_port__update_network_group_id(unit, &mpls_port);
        if (rv != BCM_E_NONE) {
            printf("Error, mpls_port__update_network_group_id\n");
            return rv;
        }

        /* Set parameters for egress mpls port */
        if (pwe_cw) {
            mpls_port.flags |= BCM_MPLS_PORT_CONTROL_WORD;
        }
        /* egress label parameters for pwe encapsulation configuration */
        mpls_port.egress_label.ttl = 10;
        mpls_port.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_SET;

        if (mpls_termination_label_index_enable) {
            BCM_MPLS_INDEXED_LABEL_SET(&mpls_port.match_label,
                                       is_primary ? &vswitch_vpls_hvpls_info.in_vc_label_primary : &vswitch_vpls_hvpls_info.in_vc_label_protected,
                                       vswitch_vpls_info_1.access_index_2);

        }
        else {
            mpls_port.match_label = is_primary ? vswitch_vpls_hvpls_info.in_vc_label_primary : vswitch_vpls_hvpls_info.in_vc_label_protected;
        }

        mpls_port.failover_id = vswitch_vpls_hvpls_info.pwe_failover_id_fec;
        mpls_port.failover_port_id = is_primary ? failover_port_id : 0;
        mpls_port.egress_failover_id = vswitch_vpls_hvpls_info.pwe_failover_id_outlif;
        mpls_port.egress_failover_port_id = egress_failover_port_id;
        mpls_port.egress_label.label = is_primary ? vswitch_vpls_hvpls_info.eg_vc_label_primary : vswitch_vpls_hvpls_info.eg_vc_label_protected;
        mpls_port.egress_tunnel_if = vswitch_vpls_hvpls_info.mpls_fec;

        if (verbose1) {
            printf("ADDING MPLS PORT (primary - %d)\n", is_primary);
        }

        rv = bcm_mpls_port_add(unit, vswitch_vpls_shared_info_1.vpn, &mpls_port);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_mpls_port_add\n");
            return rv;
        }
        if (verbose1) {
            printf("MPLS PORT: add port 0x%08x to vpn \n\r", mpls_port.mpls_port_id);
        }

        if (!is_primary) {
            failover_port_id = mpls_port.mpls_port_id;
            egress_failover_port_id = mpls_port.encap_id;
        }

        if (is_primary) {
            /* handle of the created gport */
            network_port_id = mpls_port.mpls_port_id;
            vswitch_vpls_hvpls_info.pwe_outlif_primary = mpls_port.encap_id;
            printf("pwe_outlif_primary= 0x%08x \n\r", vswitch_vpls_hvpls_info.pwe_outlif_primary);
        }
        else {
            vswitch_vpls_hvpls_info.pwe_outlif_protected = mpls_port.encap_id;
            printf("pwe_outlif_protected= 0x%08x \n\r", vswitch_vpls_hvpls_info.pwe_outlif_protected);
        }
    }

    vswitch_vpls_hvpls_info.pwe_fec = mpls_port.mpls_port_id;


    return rv;
}



/*
 * Create mpls-port: created using separate ingress, egress and FEC only calls 
 * Also creates ECMP group. 
 */
int
vswitch_vpls_hvpls_add_network_port_1(int unit){
    int rv;

    if (vswitch_vpls_hvpls_info.use_two_calls) {
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
    }
    else {
        rv = vswitch_vpls_hvpls_mpls_port_single_call_create(unit);
        if (rv != BCM_E_NONE) {
            printf("vswitch_vpls_hvpls_mpls_port_single_call_create failed: %d \n", rv);
            return rv;
        }
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
    uint32 flags = 0;
  
    /* configure the port for double tag packets - this function relevant for termination mode 22-23 only */
    rv = vswitch_vpls_pwe_side_double_tag_port_configuration(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_switch_control_set\n");
        return rv;
    }

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

    if (is_device_or_above(unit, JERICHO2)) 
    {
        /* In jr2, we use the stag:0x9100, ctag:0x8100*/
        int jr2_tag_format_s_c = 16;
        bcm_port_tpid_class_t port_tpid_class;

        bcm_port_tpid_class_t_init(&port_tpid_class);
        port_tpid_class.port = vswitch_vpls_info_1.ac_in_port;
        port_tpid_class.tpid1 = 0x8100;
        port_tpid_class.tpid2 = 0x9100;
        port_tpid_class.tag_format_class_id = jr2_tag_format_s_c;

        rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_port_tpid_class_set\n");
            return rv;
        }

    }
    else {
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
    }

    /* In advanced vlan translation mode, the default ingress/ egress actions and mapping
        are not configured. This is here to compensate. */
    if (advanced_vlan_translation_mode || is_device_or_above(unit, JERICHO2)) {
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

    /* create failover id for MPLS */
    flags = BCM_FAILOVER_FEC | (is_device_or_above(unit, JERICHO2) ? BCM_FAILOVER_FEC_2ND_HIERARCHY : 0);
    rv = bcm_failover_create(unit, flags, &vswitch_vpls_hvpls_info.mpls_failover_id_fec);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for MPLS FEC, rv - %d\n", rv);
        return rv;
    }
    printf("MPLS Failover id: 0x%x\n", vswitch_vpls_hvpls_info.mpls_failover_id_fec);


    /* create egress object that points to the newly created tunnel */
    /* this function creates FEC and LL */
    /* FEC is created with CASCADED flag to indicate hierarchy no. 2 */
    create_l3_egress_s create_l3_egress_params;

    /* Create protected FEC */
    create_l3_egress_params.l3_flags = BCM_L3_CASCADED;
    /* MPLS FEC optimized includes EEI, otherwise OutLif */
    create_l3_egress_params.l3_flags |= (mpls_port_eei || is_device_or_above(unit,JERICHO2))? 0 : BCM_L3_ENCAP_SPACE_OPTIMIZED;
    create_l3_egress_params.out_gport = mpls_lsr_info_1.eg_port;  /* outgoing port */
    create_l3_egress_params.vlan = mpls_lsr_info_1.eg_vid; 
    create_l3_egress_params.out_tunnel_or_rif = vswitch_vpls_hvpls_info.mpls_tunnel_id_protected;
    create_l3_egress_params.next_hop_mac_addr = mpls_lsr_info_1.next_hop_mac;  /* next hop mac */
    create_l3_egress_params.failover_id = vswitch_vpls_hvpls_info.mpls_failover_id_fec;  

    rv = l3__egress__create(unit, &create_l3_egress_params); 
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
      return rv;
    } 

    vswitch_vpls_hvpls_info.mpls_fec = create_l3_egress_params.fec_id;

    /* Create primery FEC */
    create_l3_egress_params.out_tunnel_or_rif = vswitch_vpls_hvpls_info.mpls_tunnel_id_primary;
    create_l3_egress_params.failover_if_id = vswitch_vpls_hvpls_info.mpls_fec;  
    create_l3_egress_params.arp_encap_id = 0;

    rv = l3__egress__create(unit, &create_l3_egress_params); 
    if (rv != BCM_E_NONE) {
        printf("Error, l3__egress__create\n");
      return rv;
    } 

    vswitch_vpls_hvpls_info.mpls_fec = create_l3_egress_params.fec_id;

    if (verbose1) {
        printf("vswitch_vpls_hvpls_run add vlan access port\n");
    }
    /* add mpls access port */
    rv = vswitch_vpls_add_access_port_1(unit, -1/*second_unit*/, &vswitch_vpls_info_1.access_port_id);
    if (rv != BCM_E_NONE) {
        printf("Error, in vswitch_vpls_add_access_port_1\n");
        return rv;
    }

    /*
     * In JR2, add the port to vlan for vlan membership check which is enabled by default.
     * It is done only for JR2 since JR1 needs the egress mc group(in_vid) open which actually is not created.
     */
    if (is_device_or_above(unit, JERICHO2)) {
        rv = bcm_vlan_gport_add(unit, mpls_lsr_info_1.in_vid, mpls_lsr_info_1.in_port, 0);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_vlan_gport_add\n");
            return rv;
        }
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

    if (is_device_or_above(unit, JERICHO2)) {
        /** If more than one encapsulation pointers for the same packet exist, PPMC must be used in JR2.*/
        int pwe_if_primary, pwe_if_protected;
        bcm_if_t cuds[2];
        bcm_if_t rep_index;

        BCM_L3_ITF_SET(pwe_if_primary, BCM_L3_ITF_TYPE_LIF, vswitch_vpls_hvpls_info.pwe_outlif_primary);
        BCM_L3_ITF_SET(pwe_if_protected, BCM_L3_ITF_TYPE_LIF, vswitch_vpls_hvpls_info.pwe_outlif_protected);

        /* PWE outlif primery with second CUD for MPLS tunnel primery */
        cuds[0] = pwe_if_primary;
        cuds[1] = vswitch_vpls_hvpls_info.mpls_tunnel_id_primary;
        rv = bcm_multicast_encap_extension_create(unit, 0, &rep_index, 2, cuds);
        if (rv != BCM_E_NONE) {
            printf("Error in 2nd copy, bcm_multicast_encap_extension_create. cud0 = 0x%08X, cud1 = 0x%08X\n", cuds[0],cuds[1]);
            return rv;
        }
        reps[1].encap1 = rep_index;
        BCM_GPORT_LOCAL_SET(reps[1].port, vswitch_vpls_info_1.pwe_out_port);

        /* PWE outlif primery with second CUD for MPLS tunnel protected */
        cuds[0] = pwe_if_primary;
        cuds[1] = vswitch_vpls_hvpls_info.mpls_tunnel_id_protected;
        rv = bcm_multicast_encap_extension_create(unit, 0, &rep_index, 2, cuds);
        if (rv != BCM_E_NONE) {
            printf("Error in 3rd copy, bcm_multicast_encap_extension_create. cud0 = 0x%08X, cud1 = 0x%08X\n", cuds[0],cuds[1]);
            return rv;
        }
        reps[2].encap1 = rep_index;
        BCM_GPORT_LOCAL_SET(reps[2].port, vswitch_vpls_info_1.pwe_out_port);

        /* PWE outlif protected with second CUD for MPLS tunnel primery */
        cuds[0] = pwe_if_protected;
        cuds[1] = vswitch_vpls_hvpls_info.mpls_tunnel_id_primary;
        rv = bcm_multicast_encap_extension_create(unit, 0, &rep_index, 2, cuds);
        if (rv != BCM_E_NONE) {
            printf("Error in 4th copy, bcm_multicast_encap_extension_create. cud0 = 0x%08X, cud1 = 0x%08X\n", cuds[0],cuds[1]);
            return rv;
        }
        reps[3].encap1 = rep_index;
        BCM_GPORT_LOCAL_SET(reps[3].port, vswitch_vpls_info_1.pwe_out_port);

        /* PWE outlif protected with second CUD for MPLS tunnel protected */
        cuds[0] = pwe_if_protected;
        cuds[1] = vswitch_vpls_hvpls_info.mpls_tunnel_id_protected;
        rv = bcm_multicast_encap_extension_create(unit, 0, &rep_index, 2, cuds);
        if (rv != BCM_E_NONE) {
            printf("Error in 5th copy, bcm_multicast_encap_extension_create. cud0 = 0x%08X, cud1 = 0x%08X\n", cuds[0],cuds[1]);
            return rv;
        }
        reps[4].encap1 = rep_index;
        BCM_GPORT_LOCAL_SET(reps[4].port, vswitch_vpls_info_1.pwe_out_port);

        flags = egress_mc ? BCM_MULTICAST_EGRESS_GROUP : BCM_MULTICAST_INGRESS_GROUP;
        if (egress_mc) {
            rv = multicast__open_fabric_mc_or_ingress_mc_for_egress_mc(unit, mc_id, mc_id);
            if (rv != BCM_E_NONE)
            {
                printf("Error, multicast__open_fabric_mc_or_ingress_mc_for_egress_mc \n");
                return rv;
            }
        }
    } else {
        /* PWE outlif primery with second CUD for MPLS tunnel primery */
        BCM_GPORT_LOCAL_SET(reps[1].port, vswitch_vpls_info_1.pwe_out_port);
        reps[1].encap1 = vswitch_vpls_hvpls_info.pwe_outlif_primary;
        reps[1].encap2 = BCM_L3_ITF_VAL_GET(vswitch_vpls_hvpls_info.mpls_tunnel_id_primary);
        reps[1].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID;

        /* PWE outlif primery with second CUD for MPLS tunnel protected */
        BCM_GPORT_LOCAL_SET(reps[2].port, vswitch_vpls_info_1.pwe_out_port);
        reps[2].encap1 = vswitch_vpls_hvpls_info.pwe_outlif_primary;
        reps[2].encap2 = BCM_L3_ITF_VAL_GET(vswitch_vpls_hvpls_info.mpls_tunnel_id_protected);
        reps[2].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID;

        /* PWE outlif protected with second CUD for MPLS tunnel primery */
        BCM_GPORT_LOCAL_SET(reps[3].port, vswitch_vpls_info_1.pwe_out_port);
        reps[3].encap1 = vswitch_vpls_hvpls_info.pwe_outlif_protected;
        reps[3].encap2 = BCM_L3_ITF_VAL_GET(vswitch_vpls_hvpls_info.mpls_tunnel_id_primary);
        reps[3].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID;

        /* PWE outlif protected with second CUD for MPLS tunnel protected */
        BCM_GPORT_LOCAL_SET(reps[4].port, vswitch_vpls_info_1.pwe_out_port);
        reps[4].encap1 = vswitch_vpls_hvpls_info.pwe_outlif_protected;
        reps[4].encap2 = BCM_L3_ITF_VAL_GET(vswitch_vpls_hvpls_info.mpls_tunnel_id_protected);
        reps[4].flags = BCM_MUTICAST_REPLICATION_ENCAP2_VALID;

        flags = egress_mc ? 0 : BCM_MULTICAST_INGRESS;
    }

    rv = bcm_multicast_set(unit, mc_id, flags, 5, reps);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_multicast_set\n");
        return rv;
    }

    return rv;
}



int
vswitch_vpls_hvpls_run_with_defaults(int unit){
    int rv = BCM_E_NONE;

    rv = mpls_port__init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_port__init\n");
        return rv;
    }

    if (!is_device_or_above(unit,JERICHO)) {
        printf("Error, Hierarchical VPLS is supported only from Jericho \n");
        return -1;
    }

    vswitch_vpls_hvpls_info_init(unit,1,13,13,10,20,6202,0);
    return vswitch_vpls_hvpls_run(unit);
}

int
vswitch_vpls_hvpls_run_with_defaults_dvapi(int unit, int acP, int pweP, int two_calls){
    int rv = BCM_E_NONE;

    rv = mpls_port__init(unit);
    if (rv != BCM_E_NONE) {
        printf("Error, in mpls_port__init\n");
        return rv;
    }

    if (!is_device_or_above(unit,JERICHO)) {
        printf("Error, Hierarchical VPLS is supported only from Jericho \n");
        return -1;
    }

    vswitch_vpls_hvpls_info_init(unit,acP,pweP,pweP,10,20,6202,two_calls);
    return vswitch_vpls_hvpls_run(unit);
}


int vswitch_vpls_mc_in_fec_run_with_defaults_dvapi(int unit,int acP, int pweP)
{
    int rv = BCM_E_NONE;
    bcm_gport_t access_port2_id;
    mc_in_fec = 1;

    vswitch_vpls_hvpls_run_with_defaults_dvapi(unit,acP,pweP,1);

    /*Add another AC port, sending unicast packet from this AC*/
    if (is_device_or_above(unit, JERICHO2)) {
        /* Init vlan(add related eth ports to the vlan),
         * since in JR2 vlan membership check is enabled by default.
         * JR1 can also do the init but the related egress mc group must be open.
         */
        vlan__init_vlan(unit,vswitch_vpls_info_1.ac_port2_outer_vlan);
        vlan__init_vlan(unit,vswitch_vpls_info_1.ac_port2_inner_vlan);

        /** In IOP mode, need to translate the glocal FEC-ID to local for L2 learning*/
        int is_jericho2_mode = *(dnxc_data_get(unit,  "headers", "system_headers", "system_headers_mode", NULL));
        if (!is_jericho2_mode) {
            rv = fec_id_alignment_field_wa_add(unit, acP, BCM_L3_ITF_VAL_GET(vswitch_vpls_hvpls_info.pwe_fec));
            if (rv != BCM_E_NONE){
                 printf("Error, fec_id_alignment_field_wa_entry_add\n");
                 return rv;
            }
        }
    }
    vswitch_vpls_add_access_port_2(unit,&access_port2_id);
}

/*
 * Failover Set
 */
int vswitch_vpls_hvpls_failover_set(int unit, uint8 is_pwe, int enable){
    int rv;
    bcm_failover_t failover_id, egress_failover_id;
    int egress_enable;

    failover_id = is_pwe? vswitch_vpls_hvpls_info.pwe_failover_id_fec : vswitch_vpls_hvpls_info.mpls_failover_id_fec;
    egress_failover_id = is_pwe? vswitch_vpls_hvpls_info.pwe_failover_id_outlif : vswitch_vpls_hvpls_info.mpls_failover_id_outlif;

    rv = bcm_failover_set(unit, failover_id, enable);    
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_failover_set failover_id:  0x%08x \n", failover_id);
        return rv;
    }

    egress_enable = enable;
    rv = bcm_failover_set(unit, egress_failover_id, egress_enable);    
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_failover_set failover_id:  0x%08x \n", egress_failover_id);
        return rv;
    }

    return rv;
}

