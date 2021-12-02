/* $Id: cint_dnx_utils_vpws.c
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2021 Broadcom Inc. All rights reserved.
*/

/*
 *
 * Configuration:
 *
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_global.c
 * cint ../../../../src/examples/dnx/cint_dnx_ip_route_basic.c
 * cint ../../../../src/examples/dnx/cint_dnx_advanced_vlan_translation_mode.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int pwe_port = 202;
 * rv = pwe_basic_main(unit,pwe_port);
 * print rv;
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
 *
 */

int has_cw=0;

struct cint_pwe_basic_info_s
{
    int vpn;
    int pwe_port;               /* incoming port from pwe side */
    int pwe_intf;               /* pwe RIF */
    bcm_gport_t mpls_port_id_ingress;   /* global lif encoded as MPLS port, used for ingress, must be as egress */
    bcm_gport_t mpls_port_id_egress;    /* global lif encoded as MPLS port, used for egress, must be as ingress */
    bcm_gport_t vlan_port_id;   /* vlan port id of the ac_port, used for ingress */
    bcm_if_t ac_encap_id;       /* global lif for egress AC */
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
     * vpn
     */
    0,
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
    9999, 18888, 0,
    /*
     * AC encap_id
     */
    0,
    /*
     * encap_id
     */
    18888,
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
    18196,
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
    
    if ((*dnxc_data_get(unit, "dev_init", "general", "application_v2", NULL)) == 0)
    {
        rv = out_port_set(unit, cint_pwe_basic_info.pwe_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, out_port_set intf_out\n");
            return rv;
        }
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
    int flags = 0, flags2 = 0;

    if(*dnxc_data_get(unit, "l3", "feature", "separate_fwd_rpf_dbs", NULL))
    {
        flags2 |= BCM_L3_FLAGS2_FWD_ONLY;
    }
    /*
     * Create FEC:
     * 1) Give a fec id.
     * 2) Tunnel id of the encapsulation entry.
     * 3) Arp id will be given with the only_encap call, so give 0.
     * 4) Give the out port.
     */
    rv = l3__egress_only_fec__create(unit, cint_pwe_basic_info.mpls_encap_fec_id,
                                 cint_pwe_basic_info.mpls_tunnel_id, 0, cint_pwe_basic_info.pwe_port, flags, flags2);
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

    label_array[0].flags = BCM_MPLS_EGRESS_LABEL_WITH_ID;
    BCM_L3_ITF_SET(label_array[0].tunnel_id, BCM_L3_ITF_TYPE_LIF, cint_pwe_basic_info.mpls_tunnel_id);

    /*
     * Set the next pointer of this entry to be the arp. This configuration is new compared to Jericho, where the arp
     * pointer used to be connected to the EEDB entry via bcm_l3_egress_create (with egress flag indication).
     */
    label_array[0].l3_intf_id = cint_pwe_basic_info.core_arp_id;

    label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

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

    int estimate_encap_size_required = *dnxc_data_get(unit, "dev_init", "general", "egress_estimated_bta_btr_config", NULL);

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags |= BCM_MPLS_PORT_EGRESS_TUNNEL;
    if (has_cw)
    {
        mpls_port.flags |= BCM_MPLS_PORT_CONTROL_WORD;
    }
    mpls_port.encap_id = 0;
    mpls_port.flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;
    mpls_port.egress_label.label = cint_pwe_basic_info.pwe_label;
    mpls_port.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    /** estimate_encap_size feature add restrictions on the global lif id allocation. */
    /** Therefore, if required, it is better to allocate without id, otherwise, use WITH_ID */
    if (!estimate_encap_size_required)
    {
        mpls_port.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_ENCAP_WITH_ID;
        mpls_port.encap_id = cint_pwe_basic_info.encap_id;
        BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, cint_pwe_basic_info.mpls_port_id_egress);
        BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
    }

    rv = bcm_mpls_port_add(unit, 0, mpls_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_mpls_port_add encapsulation\n", rv);
        return rv;
    }

    cint_pwe_basic_info.mpls_port_id_egress = mpls_port.mpls_port_id;
    cint_pwe_basic_info.encap_id = mpls_port.encap_id;
    printf("Egress mpls port id is 0x%x\n", cint_pwe_basic_info.mpls_port_id_egress);
    printf("Egress mpls encap id is (%x)\n", mpls_port.encap_id);

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
    mpls_port.flags |= BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY;

    /** JR2 and above */
    mpls_port.flags2 |= (vpn ? 0 : BCM_MPLS_PORT2_CROSS_CONNECT);
    mpls_port.flags |= BCM_MPLS_PORT_WITH_ID;
    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, cint_pwe_basic_info.encap_id);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);

    if (has_cw)
    {
        mpls_port.flags |= BCM_MPLS_PORT_CONTROL_WORD;
    }

    /*
     * encap_id is the egress outlif - used for learning
     */
    mpls_port.encap_id = cint_pwe_basic_info.encap_id;
    mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port.match_label = cint_pwe_basic_info.pwe_label;
    /** connect PWE entry to created MPLS encapsulation entry for learning */
    BCM_L3_ITF_SET(mpls_port.egress_tunnel_if, BCM_L3_ITF_TYPE_FEC, cint_pwe_basic_info.mpls_encap_fec_id);

    rv = bcm_mpls_port_add(unit, vpn, mpls_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_mpls_port_add termination\n", rv);
        return rv;
    }
    cint_pwe_basic_info.mpls_port_id_ingress = mpls_port.mpls_port_id;
    printf("Ingress mpls port id is (%x)\n", mpls_port.mpls_port_id);
    printf("Ingress mpls encap id is (%x)\n", mpls_port.encap_id);

    return rv;
}

int
pwe_basic_vswitch_add_access_port(
    int unit,
    bcm_gport_t port_id,
    int vid)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;

    /*
     * add port, according to port_vlan_vlan
     */
    bcm_vlan_port_t_init(&vlan_port);

    /*
     * set port attribures, key <port-vlan-vlan>
     */
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_CVLAN;
    vlan_port.port = port_id;
    vlan_port.match_vlan = vid;
    vlan_port.egress_vlan = 0;
    vlan_port.vsi = 0;
    vlan_port.flags = 0;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }
    cint_pwe_basic_info.vlan_port_id = vlan_port.vlan_port_id;
    cint_pwe_basic_info.ac_encap_id = vlan_port.encap_id;

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

    /*
     * Configure PWE port
     */
    rv = pwe_basic_configure_port_properties(unit, pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in pwe_basic_configure_port_properties\n", rv);
        return rv;
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
