/*~~~~~~~~~~~~~~~~~~~~~~~~~~Metro Bridge: P2P Service~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved. 
 *  
 * File: cint_vswitch_metro_p2p.c 
 * Purpose: Example of Point-to-Point service.  
 *  
 * P2P VPN (VPWS): Where two logical interfaces are attached to the service and traffic 
 *                 is statically switched from one interface to another.
 * 
 * This example creates a VSI and connects two logical ports to this VSI. 
 * Traffic inserted from one port will be forwarded to the second port with the required editing. 
 *  
 * Open P2P service and attach two logical ports:
 *  o   For port 1, set outer TPID to 0x8100 and inner TPID to 0x9100 (using cint_port_tpid.c).
 *      The association of the packet to logical port (MPLS/VLAN - port) is according to:
 *      -    <port, outer-VID, inner-vlan> for double-tagged packets
 *      -    <port, outer-VID > for single-tagged packets
 *  o   Create P2P VPN using bcm_mpls_vpn_id_create()
 *  o   Define and add VLAN ports to the created VPN:
 *      -    bcm_vlan_port_create(unit, &vlan_port_1);
 *           o   In-port: 1, outer-vlan: 100, inner-vlan: 101
 *      -    bcm_vlan_port_create(unit, &vlan_port_2);
 *           o   In-port: 1, outer-vlan: 800, inner-vlan: 801
 *  
 * Traffic:
 *   Injecting the following Ethernet packet from port 1: 
 *  
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *   |                                                                       |
 *   |                      tag1                 tag2                        |
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |  | DA | SA || TIPD1 | Prio | VID || TIPD2 | Prio | VID ||   Data   |  |
 *   |  |    |    || 0x8100|      | 100 || 0x9100|      | 101 ||          |  |
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |                                                                       |
 *   |               +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                |
 *   |               |Figure 10: Ethernet Packet from Port 1|                |
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  
 *   Packet will be sent out to port 1 with the following Ethernet header (VIDs updated):
 *
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
 *   |                                                                       |
 *   |                      tag1                 tag2                        |
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |  | DA | SA || TIPD1 | Prio | VID || TIPD2 | Prio | VID ||   Data   |  |
 *   |  |    |    || 0x8100|      | 800 || 0x9100|      | 801 ||          |  |
 *   |  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+  |
 *   |                                                                       |
 *   |                 +~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~+                   |
 *   |                 |Figure 11: Port 1 Ethernet Header|                   |
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *   Observe that there is no learning, as service is p2p.
 *
 * To Activate Above Settings Run:
 * 		BCM> cint examples/dpp/utility/cint_utils_vlan.c
 *      BCM> cint examples/dpp/cint_port_tpid.c
 *      BCM> cint examples/dpp/cint_advanced_vlan_translation_mode.c
 *      BCM> cint examples/dpp/cint_vswitch_metro_p2p.c
 *      BCM> cint
 *      cint> vswitch_p2p_init(unit, port, outer_valn1, inner_vlan1, outer_valn2, inner_vlan2);
 *
 * Note: This example is also available for a two-unit system. 
 *       More details can be found in cint_vswitch_metro_p2p_multi_device.c
 */

struct vswitch_info_s {
    int p1;         /* phy port */
    int p1_out_vid; /* outer vlan */
    int p1_in_vid;  /* inner vlan */
    bcm_gport_t gport1; /* vlan port 1 */
    
    int p2;         /* phy port */
    int p2_out_vid; /* outer vlan */
    int p2_in_vid;  /* inner vlan */
    bcm_gport_t gport2; /* vlan port 2 */
};

vswitch_info_s vswitch_info_1;

void
vswitch_info_init(int port1, int port2, int outer_valn1, int inner_vlan1, int outer_valn2, int inner_vlan2) {
    vswitch_info_1.p1 = port1;
    vswitch_info_1.p1_out_vid = outer_valn1;
    vswitch_info_1.p1_in_vid = inner_vlan1;
    
    vswitch_info_1.p2 = port2;
    vswitch_info_1.p2_out_vid = outer_valn2;
    vswitch_info_1.p2_in_vid = inner_vlan2;
}

/* open vpn service and attach 2 gports according to vswitch_info_1 config */
int
vswitch_p2p_init(int unit, int port1, int port2, int outer_valn1, int inner_vlan1, int outer_valn2, int inner_vlan2) {
    int nof_outer_tpids;
    int nof_inner_tpids;
    bcm_mpls_vpn_config_t p2p_vpn_info;
    bcm_vlan_port_t vlan_port_1;
    bcm_vlan_port_t vlan_port_2;
    bcm_vswitch_cross_connect_t gports; 
    
    int rv;
    
    /* init values */
    
    vswitch_info_init(port1, port2, outer_valn1, inner_vlan1, outer_valn2, inner_vlan2);
    
    rv = vlan__init_vlan(unit,outer_valn1);
    if (rv != BCM_E_NONE)
    {
        return rv;
    }
    rv = vlan__init_vlan(unit,outer_valn2);
    if (rv != BCM_E_NONE)
    {
        return rv;
    }
    rv = vlan__init_vlan(unit,inner_vlan1);
    if (rv != BCM_E_NONE)
    {
        return rv;
    }
    rv = vlan__init_vlan(unit,inner_vlan2);
    if (rv != BCM_E_NONE)
    {
        return rv;
    }
    
    /* in order to be capable of identifying two tags
     * the relevant ports shuold be set with 2 tpids 
     * set tpids: 0x8100 for outer and 0x9100 for inner
     * the next calls from port_tpid.c
     */
    
    nof_outer_tpids = 1;
    nof_inner_tpids = 1;
    
    port_tpid_init(vswitch_info_1.p1,nof_outer_tpids,nof_inner_tpids);
    port_tpid_set(unit);
    port_tpid_init(vswitch_info_1.p2,nof_outer_tpids,nof_inner_tpids);
    port_tpid_set(unit);

    /* In advanced vlan translation mode, the default ingress/ egress actions and mapping
       are not configured. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        rv = vlan_translation_default_mode_init(unit);
        if (rv != BCM_E_NONE) {
            printf("Error, in vlan_translation_default_mode_init\n");
            return rv;
        }
    }
    
    /* add first port, according to port_vlan_vlan */
    bcm_vlan_port_t_init(&vlan_port_1);
    
    /* set port attribures, key <port-vlan-vlan>*/
    
    vlan_port_1.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
    vlan_port_1.port = vswitch_info_1.p1;
    vlan_port_1.match_vlan = vswitch_info_1.p1_out_vid;
    vlan_port_1.match_inner_vlan = vswitch_info_1.p1_in_vid;
    vlan_port_1.egress_vlan = outer_valn1;
    vlan_port_1.egress_inner_vlan = inner_vlan1;
    vlan_port_1.flags = 0;

    rv = bcm_vlan_port_create(unit, &vlan_port_1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create 1\n");
        return rv;
    }

    vswitch_info_1.gport1 = vlan_port_1.vlan_port_id;

    /* In advanced vlan translation mode, bcm_vlan_port_create does not create ingress / egress
       action mapping. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        rv = vlan_translation_vlan_port_create_to_translation(unit, &vlan_port_1);
        if (rv != BCM_E_NONE) {
            printf("Error: vlan_translation_vlan_port_create_to_translation\n");
        }
    }
    
    /* note: in this moment, service setup is not complete, so packet incomming from this
    Port/ Attachment Curcuit will be dropped */
    
    /* add second port of the vpn */
    
    bcm_vlan_port_t_init(&vlan_port_2);
    
    /* set port attribures, key <port-vlan-vlan> */
    
    vlan_port_2.port = vswitch_info_1.p2;
    vlan_port_2.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN_STACKED;
    vlan_port_2.match_vlan = vswitch_info_1.p2_out_vid;
    vlan_port_2.match_inner_vlan = vswitch_info_1.p2_in_vid; 
    vlan_port_2.egress_vlan = outer_valn2; /* vswitch_info_1.p2_out_vid;*/
    vlan_port_2.egress_inner_vlan =  inner_vlan2;
    vlan_port_2.flags = 0;
    
    rv = bcm_vlan_port_create(unit, &vlan_port_2);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_create 2\n");
        return rv;
    }

    vswitch_info_1.gport2 = vlan_port_2.vlan_port_id;

    /* In advanced vlan translation mode, bcm_vlan_port_create does not create ingress / egress
       action mapping. This is here to compensate. */
    if (advanced_vlan_translation_mode) {
        rv = vlan_translation_vlan_port_create_to_translation(unit, &vlan_port_2);
        if (rv != BCM_E_NONE) {
            printf("Error: vlan_translation_vlan_port_create_to_translation\n");
        }

    }
    
    print("vlan_port_1 gport=0x%08x\n", vlan_port_1.vlan_port_id);
    print("vlan_port_2 gport=0x%08x\n", vlan_port_2.vlan_port_id);

    /* cross connect the 2 ports */
    bcm_vswitch_cross_connect_t_init(&gports);

    gports.port1 = vlan_port_1.vlan_port_id;
    gports.port2 = vlan_port_2.vlan_port_id;
    rv = bcm_vswitch_cross_connect_add(unit, &gports);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_add\n");
        print rv;
        return rv;
    }

    return rv;
}

/* close vpn service and detach 2 cross-connected gports according to vswitch_info_1 config */
int vswitch_p2p_deinit(int unit)
{
    int rv;
    bcm_vswitch_cross_connect_t gports;
    
    /* remove cross connect of the 2 ports */
    gports.port1 = vswitch_info_1.gport1;
    gports.port2 = vswitch_info_1.gport2;
    rv = bcm_vswitch_cross_connect_delete(unit, gports);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_vswitch_cross_connect_delete\n");
        print rv;
        return rv;
    }

    rv = bcm_vlan_port_destroy(unit, vswitch_info_1.gport1);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_destroy 1\n");
        return rv;
    }

    rv = bcm_vlan_port_destroy(unit, vswitch_info_1.gport2);
    if (rv != BCM_E_NONE) {
        printf("Error, in bcm_vlan_port_destroy 2\n");
        return rv;
    }

    rv = bcm_vlan_destroy(unit, vswitch_info_1.p1_in_vid);
    if (rv != BCM_E_NONE)
    {
        return rv;
    }

    rv = bcm_vlan_destroy(unit, vswitch_info_1.p1_out_vid);
    if (rv != BCM_E_NONE)
    {
        return rv;
    }

    rv = bcm_vlan_destroy(unit, vswitch_info_1.p2_in_vid);
    if (rv != BCM_E_NONE)
    {
        return rv;
    }

    rv = bcm_vlan_destroy(unit, vswitch_info_1.p2_out_vid);
    if (rv != BCM_E_NONE)
    {
        return rv;
    }

    return rv;
}

