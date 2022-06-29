/*
 * $Id: cint_dnx_rcy_extended_encap_default_outlif.c
 * Exp $
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2022 Broadcom Inc. All rights reserved.
 */

/*
 * Test Scenario
 * bcm shell "config add ucode_port_40.0=RCY.0:core_1.40"
 * bcm shell "config add tm_port_header_type_in_40.0=RCH_1" (if ibch1_supported, "config add tm_port_header_type_in_40.0=IBCH1_MODE")
 * bcm shell "0: tr 141 NoInit=1"
 *
 * cint ../../../../src/./examples/dnx/utility/cint_sand_utils_global.c
 * cint ../../../../src/./examples/dnx/rcy/cint_dnx_rcy_extended_encap_default_outlif.c
 * cint
 * cint_dnx_rcy_extended_encap_default_outlif(0,201,200,40);
 * exit;
 *
 * //Unknown MC into tunnel and RCY
 * tx 1 psrc=201 data=0xffffffffffff00000000001181000064ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f20
 * //Received packets on unit 0 should be:
 * //Source port: 0, Destination port: 0
 * Data: 0xffffffffffff00000000001181000064ffff000102030405060708090a0b0c0d0e0f101112131415161718191a1b1c1d1e1f200000000000000000000000
 *
 * The following cint connect packet from  port inPort and vlan(100) to a MC group with 2 entries:
 * 1. back to CPU
 * 2. to RCY (ext-encap)
 */

int cint_dnx_rcy_extended_encap_default_outlif(int unit, int inPort, int outPort, int rcyPort)
{
    int rv;
    int vlan = 100;
    bcm_vlan_port_t vlan_port;
    int mpls_port_outlif = 0xcce8;
    int rch_outlif = 0xecec;
    int mpls_pwe_label = 300;
    uint32 ibch1_supported = *dnxc_data_get(unit, "headers", "system_headers", "system_headers_ibch1_supported", NULL);
    /*
     * Create Vlan object
     */
    rv = bcm_vlan_create(unit, vlan);
    if(rv)
    {
        printf("Error! bcm_vlan_create\n");
        return -1;
    }

    /*
     * Allow vlan on in port
     */
    bcm_pbmp_t pbmp_loc;
    bcm_pbmp_t ubmp_loc;
    BCM_PBMP_CLEAR(pbmp_loc);
    BCM_PBMP_CLEAR(ubmp_loc);
    BCM_PBMP_PORT_ADD(pbmp_loc, inPort);
    BCM_PBMP_PORT_ADD(ubmp_loc, inPort);
    rv = bcm_vlan_port_add(unit, vlan, pbmp_loc, ubmp_loc);
    if(rv)
    {
        printf("Error! bcm_vlan_port_add\n");
        return -1;
    }

    /*
     * Create AC with inPort match criteria
     */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = inPort;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
    vlan_port.vsi = vlan;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if(rv)
    {
        printf("Error! bcm_vlan_port_create\n");
        return -1;
    }

    /*
      * Configure unknown multicast information on VSI (==Vlan)
      */
    bcm_gport_t gport_unknown_dest;
    bcm_vlan_control_vlan_t control_local = {0};
    BCM_MCAST_GPORT_GROUP_ID_SET(gport_unknown_dest, vlan);
    control_local.unknown_unicast_group = gport_unknown_dest;
    control_local.unknown_multicast_group = gport_unknown_dest;
    control_local.broadcast_group = gport_unknown_dest;
    control_local.forwarding_vlan = vlan;
    rv = bcm_vlan_control_vlan_set(unit, vlan, control_local);
    if(rv)
    {
        printf("Error! bcm_vlan_control_vlan_set\n");
        return -1;
    }

    /*
    * Create multicast group (==Vlan)
    */
    bcm_multicast_t group = vlan;
    rv = bcm_multicast_create(unit, BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP, &group);
    if(rv)
    {
        printf("Error! bcm_multicast_create\n");
        return -1;
    }



    /*
     * Create PPMC extensions with an PWE and RCH outlifs
     */
    bcm_if_t pwe_if;
    BCM_L3_ITF_SET(pwe_if, BCM_L3_ITF_TYPE_LIF, mpls_port_outlif);
    bcm_if_t rch_if;
    BCM_L3_ITF_SET(rch_if, BCM_L3_ITF_TYPE_LIF, rch_outlif);
    int rep_index = 0x300000;
    bcm_if_t cuds[2] = {rch_if, pwe_if};
    rv = bcm_multicast_encap_extension_create(unit, BCM_MULTICAST_ENCAP_EXTENSION_WITH_ID, &rep_index, 2, cuds);
    if(rv)
    {
        printf("Error! bcm_multicast_encap_extension_create\n");
        return -1;
    }

    /*
     * Add a single entry to Multicast group, pointing to the PPMC replications
     */
    bcm_multicast_replication_t rep_array;
    bcm_multicast_replication_t_init(&rep_array);
    rep_array.encap1 = rep_index;
    rep_array.port = rcyPort;
    rv = bcm_multicast_add(unit, group, BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP, 1, &rep_array);
    if(rv)
    {
        printf("Error! bcm_multicast_create\n");
        return -1;
    }

    /*
     * Add another single entry to Multicast group, pointing to outport
     */
    bcm_multicast_replication_t_init(&rep_array);
    rep_array.port = outPort;
    rv = bcm_multicast_add(unit, group, BCM_MULTICAST_WITH_ID | BCM_MULTICAST_INGRESS_GROUP, 1, &rep_array);
    if(rv)
    {
        printf("Error! bcm_multicast_create\n");
        return -1;
    }

    bcm_mpls_port_t  mpls_port;
    bcm_mpls_port_t_init(&mpls_port);
    BCM_GPORT_SUB_TYPE_LIF_SET(mpls_port.mpls_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, mpls_port_outlif);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, mpls_port.mpls_port_id);
    mpls_port.flags = BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL | BCM_MPLS_PORT_CONTROL_WORD ;
    mpls_port.flags2 = BCM_MPLS_PORT2_EGRESS_ONLY;
    mpls_port.egress_label.flags =  BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    mpls_port.egress_label.label = mpls_pwe_label;
    mpls_port.egress_label.encap_access = bcmEncapAccessRif;
    rv = bcm_mpls_port_add(unit, 0, &mpls_port);
    if(rv)
    {
        printf("Error! bcm_mpls_port_add\n");
        return -1;
    }

    /*
     * Create an Ext-encap RCH outlif
     */
    bcm_l2_egress_t recycle_entry = {0};
    recycle_entry.flags = BCM_L2_EGRESS_RECYCLE_HEADER | BCM_L2_EGRESS_DEST_PORT | BCM_L2_EGRESS_WITH_ID;
    if (ibch1_supported)
    {
        recycle_entry.flags |= BCM_L2_EGRESS_PORT_INJECTION_MAP;
    }
    recycle_entry.dest_port = 0x98050001; /** Invalid dest, the packet should be dropped */
    recycle_entry.encap_id = rch_if;
    recycle_entry.recycle_app = bcmL2EgressRecycleAppExtendedEncap;
    rv = bcm_l2_egress_create(unit, &recycle_entry);
    if(rv)
    {
        printf("Error! bcm_l2_egress_create\n");
        return -1;
    }

    /*
     * Configure the RCY port as ext-encap port
     */
    
    /** if (!ibch1_supported) */
    {
        rv = bcm_port_control_set(unit, rcyPort, bcmPortControlRecycleApp, bcmPortControlRecycleAppExtendedEncap);
        if(rv)
        {
            printf("Error! bcm_port_control_set\n");
            return -1;
        }
    }

    /*
     * Configure no learning on the inPort
     * This is to allow checking signals of the second pass - after RCY.
     */
    rv = bcm_port_learn_set(unit, inPort, BCM_PORT_LEARN_FWD);
    if(rv)
    {
        printf("Error! bcm_port_control_set\n");
        return -1;
    }

    return 0;
}
