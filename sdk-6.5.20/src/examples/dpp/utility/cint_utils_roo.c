/* 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * This file provides ROO basic functionality 
 */


/*
 * Helps to migrate from Jericho version to QAX: 
 * Until Jericho, we build link layer using bcm_l2_egress_create API. 
 * From QAX, we use bcm_l3_egress_create instead. 
 * allocation_flags: For QAX only, additional flags we can't convert from l2_egress to l3_egress. 
 *          BCM_L3_KEEP_DSTMAC, BCM_L3_KEEP_VLAN 
 */
int roo__overlay_link_layer_create(int unit, 
                                   bcm_l2_egress_t* l2_egress, 
                                   int allocation_flags, 
                                   bcm_if_t* encap_id,
                                   int flags2) {

    int rv = BCM_E_NONE; 

    /* For QAX, translate bcm_l2_egress_t to bcm_l3_egress_t  */
    if (is_device_or_above(unit,JERICHO_PLUS)) {
        create_l3_egress_s l3_egress; 

        /* convert from l2_egress to l3_egress */
        l3_egress.next_hop_mac_addr = l2_egress->dest_mac; 
        l3_egress.arp_encap_id = l2_egress->encap_id; 
        l3_egress.out_tunnel_or_rif = l2_egress->l3_intf; 
        l3_egress.vlan = l2_egress->outer_vlan; 


        if (l2_egress->flags & BCM_L2_EGRESS_WITH_ID) {
            l3_egress.allocation_flags |= BCM_L3_WITH_ID; 
        }

        if (l2_egress->flags & BCM_L2_EGRESS_REPLACE) {
            l3_egress.allocation_flags |= BCM_L3_REPLACE; 
        }

        l3_egress.allocation_flags |= allocation_flags; 
        l3_egress.l3_flags2 |= flags2;


        rv = l3__egress_only_encap__create(unit, &l3_egress); 
        if (rv != BCM_E_NONE) {
            printf("Error, l3__egress_only_encap__create\n");
            return rv;
        }
        *encap_id = l3_egress.arp_encap_id; 
    } 
    /* until jericho, use bcm_l3_egress_create */
    else {
        rv = bcm_l2_egress_create(unit, l2_egress);
        if (rv != BCM_E_NONE) {
            printf("Error, bcm_l2_egress_create\n");
            return rv;
        }
        *encap_id = l2_egress->encap_id; 
    }
    return rv; 
}


/*
 * Helps to migrate from Jericho version to QAX: 
 * Until Jericho, we build link layer using bcm_l2_egress_create API. 
 * From QAX, we use bcm_l3_egress_create instead. 
 *  
 * in l2_egress_create, up to 2 vlan can be configured. 
 * For l3_egress_create, if 1 vlan, vlan = vsi (from the tunnel.vlan), so no extra configuration required. 
 *                       if 2 vlans, we must perform EVE
 *  
 * incoming parameters: 
 * - l2_egress 
 * - port, vsi: used for EVE only. perform port x vsi -> outAC. 
 */
int roo__overlay_eve(int unit, 
                    bcm_l2_egress_t* l2_egress, 
                    bcm_port_t port, 
                    bcm_vlan_t vsi) {
    int rv = BCM_E_NONE; 

    if (is_device_or_above(unit,JERICHO_PLUS)) {
        rv = vlan__eve__routing__set(unit,
                                port, vsi, /* perform port x vsi -> outAC.  */
                                l2_egress->outer_vlan, l2_egress->inner_vlan);  /* vlans to add to the packet. */
        if (rv != BCM_E_NONE) {
            printf("Error, vlan__eve__routing__set\n");
            return rv;
        }
    }

    return rv; 

}

