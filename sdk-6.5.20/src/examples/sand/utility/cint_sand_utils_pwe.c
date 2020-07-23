/* $Id: cint_sand_utils_pwe.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
*/

/*
 *
 * Configuration:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_pwe.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int pwe_port = 202;
 * rv = pwe_basic_main(unit,pwe_port);
 * print rv;
 *
 *
 *  Scenario configured in this cint:
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *   Basic VPLS Termination
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  One MPLS label is terminated with the PWE label.
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *   Basic VPLS encapsulation
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Route into a PWE over MPLS core.
 *  Exit with a packet including an MPLS and PWE labels.
 */

struct cint_pwe_basic_info_s
{
    int pwe_port;               /* incoming port from pwe side */
    int pwe_intf;               /* pwe RIF */
    bcm_gport_t mpls_port_id_ingress;   /* global lif encoded as MPLS port, used for ingress, must be as egress */
    bcm_gport_t mpls_port_id_egress;    /* global lif encoded as MPLS port, used for egress, must be as ingress */
    bcm_gport_t vlan_port_id;   /* vlan port id of the ac_port, used for ingress */
    bcm_if_t encap_id;          /* The global lif of the EEDB PWE entry. */
    int core_arp_id;            /* Id for ac ARP entry */
    int mpls_encap_fec_id;      /* ac fec id for encapsulation entry - outlif for MPLS entry in EEDB */
    bcm_mac_t pwe_intf_mac_address;     /* mac for pwe RIF */
    bcm_mac_t pwe_arp_next_hop_mac;     /* mac for egress next hop */
    bcm_if_t mpls_tunnel_id;    /* tunnel id for encapsulation entry */
    bcm_mpls_label_t mpls_tunnel_label; /* pushed label */
    bcm_mpls_label_t pwe_label; /* pwe label */
};

cint_pwe_basic_info_s cint_pwe_basic_info = {
    /*
     * port: pwe_port
     */
    0,
    /*
     * pwe_intf
     */
    103,
    /*
     * mpls_port_id_ingress |  mpls_port_id_egress | vlan_port_id
     */
    9999, 8888, 0,
    /*
     * encap_id
     */
    8888,
    /*
     * core_arp_id,
     */
    12290,
    /*
     * mpls_encap_fec_id
     */
    50001,
    /*
     * pwe_intf_mac_address | pwe_arp_next_hop_mac |
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}, {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d},
    /*
     * mpls_tunnel_id
     */
    8194,
    /*
     * mpls_tunnel_label  , pwe_label
     */
    3333, 3456
};

int
pwe_basic_configure_port_properties(
    int unit,
    int pwe_port)
{
    int rv = BCM_E_NONE;

    /*
     * set class for pwe port
     */

    rv = bcm_port_class_set(unit, pwe_port, bcmPortClassId, pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_class_set, port=%d, \n", pwe_port);
        return rv;
    }
    cint_pwe_basic_info.pwe_port = pwe_port;

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, cint_pwe_basic_info.pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set intf_out\n");
        return rv;
    }

    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, cint_pwe_basic_info.pwe_port, cint_pwe_basic_info.pwe_intf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    return rv;
}

int
pwe_basic_create_l3_interfaces(
    int unit)
{

    int rv;

    rv = intf_eth_rif_create(unit, cint_pwe_basic_info.pwe_intf, cint_pwe_basic_info.pwe_intf_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create pwe_intf\n");
    }

    return rv;
}

int
pwe_basic_create_arp_entry(
    int unit)
{
    int rv;

    /*
     * Configure ARP entry
     */
    rv = l3__egress_only_encap__create(unit, 0, cint_pwe_basic_info.core_arp_id,
                                       cint_pwe_basic_info.pwe_arp_next_hop_mac, cint_pwe_basic_info.pwe_intf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
    }

    return rv;

}

/*
* Update predefined ARP entry with required MPLS tunnel.
*
* Relevant only for Jericho devices.
*/
int
pwe_basic_update_arp_entry(
    int unit)
{
    int rv;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;       /* not intersting */
    uint32 flags = BCM_L3_WITH_ID | BCM_L3_REPLACE | BCM_L3_EGRESS_ONLY;

    bcm_l3_egress_t_init(&l3eg);

    l3eg.mac_addr = cint_pwe_basic_info.pwe_arp_next_hop_mac;
    l3eg.encap_id = cint_pwe_basic_info.core_arp_id;
    l3eg.vlan = cint_pwe_basic_info.pwe_intf;
    l3eg.intf = cint_pwe_basic_info.mpls_tunnel_id;

    rv = bcm_l3_egress_create(unit, flags, &l3eg, &l3egid_null);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only\n");
    }

    return rv;
}

int
pwe_basic_create_fec_entry(
    int unit)
{
    int rv;
    int temp_out_rif;
    int flags = 0;

    if (!is_device_or_above(unit, JERICHO2))
    {
        flags |= BCM_L3_ENCAP_SPACE_OPTIMIZED;
    }
    /*
     * Create FEC:
     * 1) Give a fec id.
     * 2) Tunnel id of the encapsulation entry.
     * 3) Arp id will be given with the only_encap call, so give 0.
     * 4) Give the out port.
     */
    rv = l3__egress_only_fec__create(unit, cint_pwe_basic_info.mpls_encap_fec_id,
                                     cint_pwe_basic_info.mpls_tunnel_id, 0, cint_pwe_basic_info.pwe_port, flags);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
    }

    /*
     * The termination scenario doesn't need FEC.
     * We bridge out of the core with no usage of FEC.
     */

    return rv;
}

int
pwe_basic_create_mpls_tunnel(
    int unit)
{

    bcm_mpls_egress_label_t label_array[1];
    int num_labels = 1;
    int rv;

    bcm_mpls_egress_label_t_init(&label_array[0]);

    /*
     * Label to be pushed as part of the MPLS tunnel.
     */
    label_array[0].label = cint_pwe_basic_info.mpls_tunnel_label;
    if (is_device_or_above(unit, JERICHO2))
    {
        label_array[0].flags = BCM_MPLS_EGRESS_LABEL_WITH_ID;
        BCM_L3_ITF_SET(label_array[0].tunnel_id, BCM_L3_ITF_TYPE_LIF, cint_pwe_basic_info.mpls_tunnel_id);

        /*
         * Set the next pointer of this entry to be the arp. This configuration is new compared to Jericho, where the arp
         * pointer used to be connected to the EEDB entry via bcm_l3_egress_create (with egress flag indication).
         */
        label_array[0].l3_intf_id = cint_pwe_basic_info.core_arp_id;
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    }

    /*
     * In non-jericho2 device SWAP_OR_PUSH action needs to be defined in order to use EEDB entry for swap action.
     */
    if (!is_device_or_above(unit, JERICHO2))
    {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
        label_array[0].action = BCM_MPLS_EGRESS_ACTION_SWAP_OR_PUSH;
    }

    rv = bcm_mpls_tunnel_initiator_create(unit, 0, num_labels, label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    cint_pwe_basic_info.mpls_tunnel_id = label_array[0].tunnel_id;

    return rv;

}

int
pwe_basic_create_termination_stack(
    int unit)
{
    bcm_mpls_tunnel_switch_t entry;
    int rv;

    /*
     * Create a stack of MPLS labels to be terminated
     */
    bcm_mpls_tunnel_switch_t_init(&entry);

    entry.action = BCM_MPLS_SWITCH_ACTION_POP;

    /*
     * incoming label
     * only the mpls tunnel label needs to be defined here.
     * pwe label will be handed as part of the ingress mpls_port_add api
     */
    entry.label = cint_pwe_basic_info.mpls_tunnel_label;
    if (soc_property_get(unit, "mpls_termination_label_index_enable", 0))
    {
        int label = cint_pwe_basic_info.mpls_tunnel_label;
        BCM_MPLS_INDEXED_LABEL_SET(entry.label, label, 1);
    }

    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }

    return rv;

}

int
pwe_basic_mpls_port_add_encapsulation(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL | BCM_MPLS_PORT_ENCAP_WITH_ID;
    mpls_port.encap_id = cint_pwe_basic_info.encap_id;

    mpls_port.flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;

    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, cint_pwe_basic_info.mpls_port_id_egress);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
    mpls_port.egress_label.label = cint_pwe_basic_info.pwe_label;
    mpls_port.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

    rv = bcm_mpls_port_add(unit, 0, mpls_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_mpls_port_add encapsulation\n", rv);
        return rv;
    }

    cint_pwe_basic_info.mpls_port_id_egress = mpls_port.mpls_port_id;
    cint_pwe_basic_info.encap_id = mpls_port.encap_id;

    return rv;
}

int
pwe_basic_mpls_port_add_termination(
    int unit,
    bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY;

    if (!is_device_or_above(unit, JERICHO2))
    {
        mpls_port.flags |= BCM_MPLS_PORT_ENCAP_WITH_ID;
        /** We are using EEI(outlif) model in Jericho which is analogous to J2. For this LEARN_ENCAP flag is required */
        mpls_port.flags2 |= BCM_MPLS_PORT2_LEARN_ENCAP;
    }
    else
    {
        /** JR2 and above */
        mpls_port.flags2 |= (vpn ? 0 : BCM_MPLS_PORT2_CROSS_CONNECT);
    }

    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, cint_pwe_basic_info.mpls_port_id_ingress);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
    /*
     * encap_id is the egress outlif - used for learning
     */
    mpls_port.encap_id = cint_pwe_basic_info.encap_id;
    mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port.match_label = cint_pwe_basic_info.pwe_label;
    if (soc_property_get(unit, "mpls_termination_label_index_enable", 0))
    {
        int label = cint_pwe_basic_info.pwe_label;
        BCM_MPLS_INDEXED_LABEL_SET(mpls_port.match_label, label, 2);
    }
    /** connect PWE entry to created MPLS encapsulation entry for learning */
    BCM_L3_ITF_SET(mpls_port.egress_tunnel_if, BCM_L3_ITF_TYPE_FEC, cint_pwe_basic_info.mpls_encap_fec_id);

    rv = bcm_mpls_port_add(unit, vpn, mpls_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_mpls_port_add termination\n", rv);
        return rv;
    }

    cint_pwe_basic_info.mpls_port_id_ingress = mpls_port.mpls_port_id;

    return rv;
}

int
pwe_basic_vswitch_add_access_port(
    int unit,
    bcm_gport_t port_id)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;
    int is_two_dev = 0;

    /*
     * add port, according to port_vlan_vlan
     */
    bcm_vlan_port_t_init(&vlan_port);

    /*
     * set port attribures, key <port-vlan-vlan>
     */
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = port_id;
    vlan_port.match_vlan = cint_pwe_basic_info.vpn;
    vlan_port.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : cint_pwe_basic_info.vpn;
    vlan_port.vsi = cint_pwe_basic_info.vpn;
    vlan_port.flags = 0;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }
    cint_pwe_basic_info.vlan_port_id = vlan_port.vlan_port_id;

    rv = bcm_vswitch_port_add(unit, cint_pwe_basic_info.vpn, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }

    return rv;
}

int
pwe_check_support(
    int unit)
{
    int rv = BCM_E_NONE;
    int is_qax = 0;

    rv = is_qumran_ax_only(unit, &is_qax);
    if (rv != BCM_E_NONE)
    {
        printf("Failed(%d) is_qumran_ax_only\n", rv);
        return rv;
    }
    if (!((!is_device_or_above(unit, JERICHO2) && is_device_or_above(unit, JERICHO_PLUS) && !is_qax) ||
          is_device_or_above(unit, JERICHO2)))
    {
        printf("Only Jericho2 and Jericho_plus are supported\n");
        return BCM_E_CONFIG;
    }

    return rv;
}

/*
 * Main function for basic mpls vpls scenario.
 */
int
pwe_basic_main(
    int unit,
    int pwe_port)
{
    int rv = BCM_E_NONE;

    rv = pwe_check_support(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in pwe_check_support\n", rv);
        return rv;
    }

    /*
     * Configure PWE port
     */
    rv = pwe_basic_configure_port_properties(unit, pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in pwe_basic_configure_port_properties\n", rv);
        return rv;
    }

    if (!is_device_or_above(unit, JERICHO2))
    {
        rv = mpls_port__vswitch_vpls_vpn_create__set(unit, cint_pwe_basic_info.vpn);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in mpls_port__vswitch_vpls_vpn_create__set\n", rv);
            return rv;
        }
    }

    /*
     * Configure L3 interfaces
     */
    rv = pwe_basic_create_l3_interfaces(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in pwe_basic_create_l3_interfaces\n", rv);
        return rv;
    }

    /*
     * Configure an ARP entries
     */
    rv = pwe_basic_create_arp_entry(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in pwe_basic_create_arp_entry\n", rv);
        return rv;
    }

    /*
     * Configure a push entry.
     */
    rv = pwe_basic_create_mpls_tunnel(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in pwe_basic_create_mpls_tunnel\n", rv);
        return rv;
    }

    /*
     * Configure fec entry
     */
    rv = pwe_basic_create_fec_entry(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in pwe_basic_create_fec_entry\n", rv);
        return rv;
    }

    /*
     * Configure a termination label for the ingress flow
     */
    rv = pwe_basic_create_termination_stack(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in pwe_basic_create_termination_stack\n", rv);
        return rv;
    }

    /*
     * In non jericho 2, ARP entry configuration is done with bcm_l3_egress_create and a tunnel id of the MPLS entry.
     * It connects the MPLS entry to the ARP, Whereas in Jericho 2, bcm_mpls_tunnel_initiator_create is used to connect the MPLS to the given arp id.
     * Therefore, we need to update the ARP entry with correct MPLS tunnel.
     */
    if (!is_device_or_above(unit, JERICHO2))
    {
        rv = pwe_basic_update_arp_entry(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in pwe_basic_update_arp_entry\n", rv);
            return rv;
        }
    }

    /*
     * configure PWE tunnel - egress flow
     */
    rv = pwe_basic_mpls_port_add_encapsulation(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in pwe_basic_mpls_port_add_encapsulation\n", rv);
        return rv;
    }

    /*
     * configure PWE tunnel - ingress flow vpn is set to 0 - so P2P is created.
     */
    rv = pwe_basic_mpls_port_add_termination(unit, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in pwe_basic_mpls_port_add_termination\n", rv);
        return rv;
    }

    return rv;
}
