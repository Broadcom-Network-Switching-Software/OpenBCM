/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_vpls_mp_basic.c Purpose: basic example for VPLS. 
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
 * cint ../../../../src/examples/dnx/vpls/cint_vpls_pwe_tagged_mode_basic.c
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_vlan_translate.c
 * cint
 * int unit = 0; 
 * int rv = 0; 
 * int in_port = 200; 
 * int out_port = 201;
 * rv = vpls_pwe_tagged_mode_basic_main(unit,in_port,out_port);
 * print rv; 
 * 
 * 
 *  Scenario configured in this cint:
 * 
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *   Basic VPLS Termination
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  One MPLS label is terminated with the PWE label. 
 *  Lookup in MAC table for ethernet forwarding.
 *
 *  Traffic:
 * 
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              | SA                   ||   MPLS   ||  PWE         ||      DA              |     SA                || vlan | tpid
 *   |    |0C:00:02:00:01   |0c:00:02:00:00   ||Label:3333||Label:3456  ||11:00:00:01:12 |0c:00:02:00:00    || 5    | 0x8100
 *   |    |                         |                       ||Exp:0     ||                  ||                  |                               ||
 *   |    |                         |                       ||TTL:20    ||                  ||                  |                           ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              |     SA                || vlan | tpid
 *   |    |11:00:00:01:12 |0c:00:02:00:00    || 5    | 0x8100
 *   |    |              |              ||          ||               ||      ||
 *   |    |              |              ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *   Basic VPLS encapsulation
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Route into a PWE over MPLS core. 
 *  Exit with a packet including an MPLS and PWE labels.
 *
 *  Traffic:
 * 
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              |     SA                || vlan | tpid
 *   |    |0c:00:02:00:00   |11:00:00:01:12    || 5    | 0x8100
 *   |    |              |              ||          ||               ||      ||
 *   |    |              |              ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              | SA                   ||   MPLS   ||  PWE         ||      DA              |     SA                || vlan | tpid
 *   |    |0c:00:02:00:01   |00:00:00:cd:1d   ||Label:3333||Label:3456  || 0c:00:02:00:00   |11:00:00:01:12  || 1111 | 0x8100
 *   |    |                         |                       ||Exp:0     ||                  ||               ||      ||
 *   |    |                         |                       ||TTL:20    ||                  ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *
 * ###############################################################################################
 * P2P (cross connect):
 * This cint shows also two examples for VPLS P2P traffic:
 * Gliful and Glifless PWE tagged mode - both for access->core and for core->access traffic
 * vpls_pwe_tagged_mode_p2p_basic_main
 * vpls_pwe_tagged_mode_basic_indexed_native_ac_p2p_main
 * ###############################################################################################
 */

struct cint_vpls_pwe_tagged_mode_basic_info_s
{
    int access_port;            /* incoming port from acsess side */
    int core_port;              /* incoming port from core side */
    int core_eth_rif;           /* core RIF */
    bcm_vpn_t service_vsi;      /* vsi id */
    bcm_gport_t mpls_port_id_ing;       /* global lif encoded as MPLS port for service (ingress side) */
    bcm_gport_t mpls_port_id_eg;        /* global lif encoded as MPLS port for service (egress side) */
    bcm_gport_t outer_ac_vlan_port_id;  /* vlan port id of the ac_port, used for ingress */
    bcm_if_t pwe_encap_id;      /* The global lif of the EEDB PWE entry. */
    bcm_if_t indexed_ac_encap_id;       /* The global lif of the indexed AC entry. */
    int core_arp_id;            /* Id for core ARP entry */
    int mpls_fec_id;            /* service 1 fec id for encapsulation entry - outlif for MPLS entry in EEDB */
    bcm_mac_t core_eth_rif_mac; /* mac for core RIF */
    bcm_mac_t core_next_hop_mac;        /* mac for next hop */
    bcm_mac_t service_mac_1;    /* first mac address for service 1 */
    bcm_mac_t service_mac_2;    /* second mac address for service 1 */
    int outer_tpid;             /* Outer tpid for application */
    int inner_tpid;             /* Inner tpid for application */
    bcm_if_t mpls_tunnel_id;    /* tunnel id for encapsulation entry */
    bcm_mpls_label_t service_mpls_tunnel_label_1;       /* pushed label */
    bcm_mpls_label_t service_mpls_tunnel_label_2;       /* pushed label */
    bcm_mpls_label_t service_pwe_label; /* pwe label */
    bcm_vlan_t outer_ac_egress_outer_vlan;
    bcm_vlan_t outer_ac_egress_inner_vlan;
    bcm_vlan_t ingress_canonical_outer_vlan;
    bcm_vlan_t ingress_canonical_inner_vlan;
    bcm_vlan_t egress_canonical_outer_vlan;
    bcm_vlan_t egress_canonical_inner_vlan;
    bcm_port_tag_format_class_t tag_format_untagged;
    bcm_port_tag_format_class_t tag_format_double_tagged;
    bcm_gport_t ingress_native_ac_vlan_port_id; /* vlan port id of the inner ac_port, used for ingress */
    bcm_gport_t egress_native_ac_vlan_port_id;  /* vlan port id of the inner ac_port, used for egress */
    bcm_gport_t access_egress_vlan_port_id;     /* vlan port id of the access port, used for egress at terination flow */
    uint32 egress_vlan_edit_profile, ingress_vlan_edit_profile;
    bcm_gport_t access_port_ac_in_lif;
};

cint_vpls_pwe_tagged_mode_basic_info_s cint_vpls_pwe_tagged_mode_basic_info = {
    /*
     * ports : access_port | core_port 
     */
    200, 201,
    /*
     * core_eth_rif
     */
    30,
    /*
     * service_vsi  
     */
    10,
    /*
     * mpls_port_id_ing|eg | outer_ac_vlan_port_id | pwe_encap_id | indexed_ac_encap_id
     */
    9999, 8888, 1111, 0, 0,
    /*
     * core_arp_id
     */
    12290,
    /*
     * mpls_fec_id  
     */
    50001,
    /*
     * core_eth_rif_mac
     */
    {0x00, 0x0F, 0x00, 0x02, 0x0A, 0x22},
    /*
     * core_next_hop_mac  
     */
    {0x00, 0x00, 0x00, 0x00, 0xCD, 0x1D},
    /*
     * service_mac_1   |   service_mac_2 
     */
    {0x00, 0x0C, 0x00, 0x02, 0x0C, 0x33}, {0x00, 0x0C, 0x00, 0x02, 0x0C, 0x44},
    /*
     * outer_tpid | inner_tpid 
     */
    0x9100, 0x8100,
    /*
     * mpls_tunnel_id 
     */
    8194,
    /*
     * service_mpls_tunnel_label_1  | service_mpls_tunnel_label_2 |  service_pwe_label
     */
    4444, 5555, 3333,
    /*
     * outer_ac_egress_outer_vlan | outer_ac_egress_inner_vlan
     */
    327, 427,
    /*
     * ingress_canonical_outer_vlan | ingress_canonical_inner_vlan
     */
    1000, 2000,
    /*
     * egress_canonical_outer_vlan | egress_canonical_inner_vlan
     */
    1000, 2000,
    /*
     * tag_format_untagged,  tag_format_double_tagged
     */
    0, 16,
    /*
     * ingress_native_ac_vlan_port_id,egress_native_ac_vlan_port_id,access_egress_vlan_port_id
     */
    0x1234, 0x4321, 0x4567,
    /*
     * egress_vlan_edit_profile, ingress_vlan_edit_profile 
     */
    4, 5,
    /*
     * access_port ac in lif
     */
    0

};

int verbose1 = 1;

/*
 * This variable is relevant to OAM.
 * For downmep, global lif should be set in ingress LIF DB.
 * After setting is_oam to 1, then global lif for native AC in pwe indexed tagged mode will be set.
 */
int is_oam = 0;

int pwe_network_domain = 507;

void
vpls_pwe_tagged_mode_basic_init(
    int access_port,
    int core_port)
{

    cint_vpls_pwe_tagged_mode_basic_info.access_port = access_port;
    cint_vpls_pwe_tagged_mode_basic_info.core_port = core_port;

}

/*
 * Creates the AC In-Lif for Access AC port.
 * In case of p2p - keep the value to cross connect it
 */
int
vpls_pwe_tagged_mode_basic_access_ac_create(
    int unit,
    int in_port,
    int eth_rif,
    int is_p2p,
    bcm_gport_t *ac_in_lif)
{
    int rv;

    if(!is_p2p)
    {
        rv = in_port_intf_set(unit, in_port, eth_rif);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in_port_intf_set\n");
            return rv;
        }
    }
    else
    {
        bcm_vlan_port_t vlan_port;
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.port = in_port;
        vlan_port.vsi = eth_rif;
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT;
        vlan_port.flags = BCM_VLAN_PORT_CREATE_INGRESS_ONLY;

        rv = bcm_vlan_port_create(unit, vlan_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_port_create\n");
            return rv;
        }
        printf("vpls_pwe_tagged_mode_basic_access_ac_create(): port = %d, in_lif = 0x%08X\n", vlan_port.port, vlan_port.vlan_port_id);
        *ac_in_lif = vlan_port.vlan_port_id;
    }

    return rv;
}

int
vpls_pwe_tagged_mode_basic_configure_port_properties(
    int unit,
    int access_port,
    int core_port,
    bcm_vpn_t service_vsi,
    int core_eth_rif,
    int is_cross_connect)
{
    int rv = BCM_E_NONE;

    /*
     * set class for both ports 
     */
    if (!BCM_GPORT_IS_TRUNK(access_port) && !BCM_GPORT_IS_TRUNK(core_port)) {
        rv = bcm_port_class_set(unit, access_port, bcmPortClassId, access_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_port_class_set, port=%d, \n", access_port);
            return rv;
        }

        rv = bcm_port_class_set(unit, core_port, bcmPortClassId, core_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_port_class_set, port=%d, \n", core_port);
            return rv;
        }
    }

    /*
     * Set In-Port to In ETh-RIF 
     */

    rv = vpls_pwe_tagged_mode_basic_access_ac_create(unit, access_port, service_vsi, is_cross_connect, &(cint_vpls_pwe_tagged_mode_basic_info.access_port_ac_in_lif));
    if (rv != BCM_E_NONE)
    {
        printf("Error, vpls_pwe_tagged_mode_basic_access_ac_create intf_in\n");
        return rv;
    }

    /*
     * Set In-Port to In ETh-RIF 
     */
    rv = in_port_intf_set(unit, core_port, core_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    /*
     * Set Out-Port default properties 
     */
    rv = out_port_set(unit, access_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set intf_out\n");
        return rv;
    }

    /*
     * Set Out-Port default properties 
     */
    rv = out_port_set(unit, core_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set intf_out\n");
        return rv;
    }

    return rv;
}

int
vpls_pwe_tagged_mode_basic_create_vsis(
    int unit,
    bcm_vpn_t service_vsi,
    int core_eth_rif)
{

    int rv;

    /*
     * create vlan based on the vsi (vpn) 
     */
    rv = bcm_vlan_create(unit, service_vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Failed (%d) to create VLAN %d\n", rv, service_vsi);
        return rv;
    }

    /*
     * create vlan based on the vsi (vpn) 
     */
    rv = bcm_vlan_create(unit, core_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Failed (%d) to create VLAN %d\n", rv, core_eth_rif);
        return rv;
    }

    return rv;
}

int
vpls_pwe_tagged_mode_basic_create_l3_interfaces(
    int unit,
    int core_eth_rif,
    bcm_mac_t core_eth_rif_mac)
{

    int rv;

    rv = intf_eth_rif_create(unit, core_eth_rif, core_eth_rif_mac);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create pwe_intf\n");
    }

    return rv;
}

int
vpls_pwe_tagged_mode_basic_create_arp_entry(
    int unit,
    int *core_arp_id,
    bcm_mac_t core_next_hop_mac,
    int core_eth_rif)
{
    int rv;

    /*
     * Configure ARP entry 
     */
    rv = l3__egress_only_encap__create(unit, 0, core_arp_id, core_next_hop_mac, core_eth_rif);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
    }

    return rv;

}

int
vpls_pwe_tagged_mode_basic_create_fec_entry(
    int unit,
    int fec_id,
    bcm_if_t tunnel_id,
    int port)
{
    int rv;
    int temp_out_rif;
    int flags = 0;

    /**
     * Under below scenarios, this case tests the FEC format of dest+EEI:
     * 1. JR2 works under JR1 system headers mode;
     * 2. JR1 device;
     */
    if (!is_device_or_above(unit, JERICHO2)
        || (is_device_or_above(unit, JERICHO2) && (soc_property_get(unit , "system_headers_mode",1) == 0)))
    {
        flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
    }
    /*
     * Create FEC: 
     * 1) Give a fec id. 
     * 2) Tunnel id of the encapsulation entry. 
     * 3) Arp id will be given with the only_encap call, so give 0. 
     * 4) Give the out port. 
     */
    rv = l3__egress_only_fec__create(unit, fec_id, tunnel_id, 0, port, flags);
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
vpls_pwe_tagged_mode_basic_create_mpls_tunnel(
    int unit,
    bcm_mpls_label_t outer_label,
    bcm_mpls_label_t inner_label,
    int num_labels,
    int arp_id,
    bcm_if_t * tunnel_id,
    int is_virtual)
{

    bcm_mpls_egress_label_t label_array[2];
    int rv;

    bcm_mpls_egress_label_t_init(&label_array[0]);
    bcm_mpls_egress_label_t_init(&label_array[1]);

    /*
     * Label to be pushed as part of the MPLS tunnel.
     */
    label_array[0].label = outer_label;

    label_array[0].flags = BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    if (!is_virtual)
    {
        label_array[0].encap_access = bcmEncapAccessTunnel3;
    }
    BCM_L3_ITF_SET(label_array[0].tunnel_id, BCM_L3_ITF_TYPE_LIF, *tunnel_id);
    
    /*
     * Set the next pointer of this entry to be the arp. This configuration is new compared to Jericho, where the arp
     * pointer used to be connected to the EEDB entry via bcm_l3_egress_create (with egress flag indication).
     */
    label_array[0].l3_intf_id = arp_id;

    if (num_labels == 2)
    {
        label_array[1].label = inner_label;
        label_array[1].flags = BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        label_array[1].tunnel_id = label_array[0].tunnel_id;
        label_array[1].l3_intf_id = label_array[0].l3_intf_id;
        label_array[1].encap_access = label_array[0].encap_access;
    }

    rv = bcm_mpls_tunnel_initiator_create(unit, 0, num_labels, label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    *tunnel_id = label_array[0].tunnel_id;

    return rv;

}

int
vpls_pwe_tagged_mode_basic_create_termination_stack(
    int unit,
    bcm_mpls_label_t * labels,
    int nof_labels)
{
    bcm_mpls_tunnel_switch_t entry;
    int rv;
    int i;
    /*
     * Create a stack of MPLS labels to be terminated 
     */

    for (i = 0; i < nof_labels; i++)
    {
        bcm_mpls_tunnel_switch_t_init(&entry);

        entry.action = BCM_MPLS_SWITCH_ACTION_POP;

        /*
         * incoming label
         * only the mpls tunnel label needs to be defined here.
         * pwe label will be handed as part of the ingress mpls_port_add api
         */
        entry.label = labels[i];

        rv = bcm_mpls_tunnel_switch_create(unit, &entry);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_mpls_tunnel_switch_create\n");
            return rv;
        }
    }

    return rv;

}

int
vpls_pwe_tagged_mode_basic_l2_addr_add(
    int unit,
    int vsi,
    bcm_mac_t mac,
    bcm_if_t encap_id,
    int fec_id)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2addr;

    bcm_l2_addr_t_init(&l2addr, mac, vsi);
    l2addr.flags |= BCM_L2_STATIC;
    l2addr.encap_id = encap_id;
    BCM_GPORT_FORWARD_PORT_SET(l2addr.port, fec_id);
    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_l2_addr_add\n", rv);
        return rv;
    }

    return rv;
}

int
vpls_pwe_tagged_mode_basic_mpls_port_add_encapsulation(
    int unit,
    bcm_gport_t * mpls_port_id,
    bcm_mpls_label_t pwe_label,
    bcm_if_t * encap_id,
    int is_virtual)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;

    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, *mpls_port_id);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
    mpls_port.egress_label.label = pwe_label;
    mpls_port.egress_label.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;

    if (!is_virtual)
    {
        /* If the native AC is Gliful, the phase of the PWE entry should be higher than the former */
        mpls_port.egress_label.encap_access = bcmEncapAccessTunnel2;
    }

    rv = bcm_mpls_port_add(unit, 0, mpls_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_mpls_port_add encapsulation\n", rv);
        return rv;
    }

    *mpls_port_id = mpls_port.mpls_port_id;
    *encap_id = mpls_port.encap_id;

    return rv;
}

int
vpls_pwe_tagged_mode_basic_mpls_port_add_termination(
    int unit,
    bcm_gport_t * mpls_port_id,
    bcm_mpls_label_t pwe_label,
    bcm_if_t * encap_id,
    int fec_id,
    bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;
    int isj2c_q2a = 0;

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_SERVICE_TAGGED | BCM_MPLS_PORT_EGRESS_TUNNEL;
    mpls_port.flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY | (vpn ? 0 : BCM_MPLS_PORT2_CROSS_CONNECT);
    mpls_port.nof_service_tags = 2;

    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, *mpls_port_id);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
    /*
     * encap_id is the egress outlif - used for learning 
     */
    mpls_port.encap_id = *encap_id;
    mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    mpls_port.match_label = pwe_label;
    /** connect PWE entry to created MPLS encapsulation entry for learning */
    BCM_L3_ITF_SET(mpls_port.egress_tunnel_if, BCM_L3_ITF_TYPE_FEC, fec_id);

    rv = bcm_mpls_port_add(unit, vpn, mpls_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_mpls_port_add termination\n", rv);
        return rv;
    }

    *mpls_port_id = mpls_port.mpls_port_id;

    isj2c_q2a = is_device_or_above(unit, JERICHO2C) && !is_device_or_above(unit, JERICHO2P);
    pwe_network_domain = isj2c_q2a?0x7e3:507;
    rv = bcm_port_class_set(unit, mpls_port.mpls_port_id, bcmPortClassIngress, pwe_network_domain);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_class_set, port=%d, \n", mpls_port.mpls_port_id);
        return rv;
    }
    return rv;
}

int
vpls_pwe_tagged_mode_basic_vswitch_add_core_outer_ac(
    int unit,
    bcm_gport_t port,
    int vsi,
    bcm_gport_t * vlan_port_id)
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
    vlan_port.port = port;
    vlan_port.flags = BCM_VLAN_PORT_CREATE_EGRESS_ONLY | BCM_VLAN_PORT_VLAN_TRANSLATION | BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.vsi = vsi;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }
    *vlan_port_id = vlan_port.vlan_port_id;

    printf("outer_ac_vlan_port_id: 0x%08x\n", *vlan_port_id);

    /*
     * rv = bcm_vswitch_port_add(unit, cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif, vlan_port.vlan_port_id);
     */
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }

    return rv;
}

int
vpls_pwe_tagged_mode_check_support(
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
 * For Ingress:
 * Define AC for inner ETH validation on packets going from core to customer.
 * The resulting vlan_port_id is saved in cint_vpls_pwe_tagged_mode_basic_info.ingress_native_ac_vlan_port_id.
 * For Egress:
 * Define a VLAN port to forward the native eth DA to. 
 * The resulting vlan_port_id is saved in cint_vpls_pwe_tagged_mode_basic_info.access_egress_vlan_port_id. 
 * Connect the Native DA to the egress vlan port.
 */
int
vpls_pwe_tagged_mode_add_network_native_ac(
    int unit,
    bcm_vpn_t vsi,
    bcm_mac_t mac,
    bcm_gport_t * ingress_vlan_port_id,
    bcm_gport_t * egress_vlan_port_id,
    bcm_gport_t mpls_port_id,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_port_t port,
    int is_virtual,
    int ingress_ac_native_wide_data_enable,
    int is_cross_connect,
    int is_set_global_lif)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;
    bcm_l2_addr_t l2addr;
    /*
     * add port.
     */
    bcm_vlan_port_t_init(&vlan_port);
    if (is_set_global_lif) {
        vlan_port.flags = BCM_VLAN_PORT_NATIVE;
    } else {
        vlan_port.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
    }

    if (is_virtual)
    {
        vlan_port.flags |= BCM_VLAN_PORT_VLAN_TRANSLATION;
    }
    else
    {
        vlan_port.flags |= BCM_VLAN_PORT_WITH_ID;
        if (is_set_global_lif) {
            BCM_GPORT_SUB_TYPE_LIF_SET(vlan_port.vlan_port_id,
                0, *ingress_vlan_port_id);
        } else {
            BCM_GPORT_SUB_TYPE_LIF_SET(vlan_port.vlan_port_id,
                BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY, *ingress_vlan_port_id);
        }
        BCM_GPORT_VLAN_PORT_ID_SET(vlan_port.vlan_port_id, vlan_port.vlan_port_id);
    }
    if (ingress_ac_native_wide_data_enable)
    {
        vlan_port.flags |= BCM_VLAN_PORT_INGRESS_WIDE;    
    }
    vlan_port.vsi = is_set_global_lif ? 0 : vsi;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_vlan_port_create\n", rv);
        return rv;
    }
    /*
     * Save the created vlan_port_id
     */
    *ingress_vlan_port_id = vlan_port.vlan_port_id;
    if (is_set_global_lif) {
        rv = bcm_vswitch_port_add(unit, vsi, vlan_port.vlan_port_id);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vswitch_port_add\n");
            return rv;
        }
    }
    
    printf("native ac = 0x%08x\n", vlan_port.vlan_port_id);

    /*
     * Add Match
     */
    bcm_port_match_info_t match_info;
    bcm_port_match_info_t_init(&match_info);
    match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY | BCM_PORT_MATCH_NATIVE;
    match_info.port = mpls_port_id;
    match_info.match = BCM_PORT_MATCH_PORT_VLAN_STACKED;        /* Match 2 tags */
    match_info.match_vlan = outer_vlan;
    match_info.match_inner_vlan = inner_vlan;
    rv = bcm_port_match_add(unit, vlan_port.vlan_port_id, &match_info);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_port_match_add\n", rv);
        return rv;
    }

    /*
     * Add VLAN port in egress
     */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags |= BCM_VLAN_PORT_WITH_ID;
    vlan_port.vsi = 0;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    vlan_port.port = port;
    BCM_GPORT_SUB_TYPE_LIF_SET(vlan_port.vlan_port_id, 0, *egress_vlan_port_id);
    BCM_GPORT_VLAN_PORT_ID_SET(vlan_port.vlan_port_id, vlan_port.vlan_port_id);
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_vlan_port_create\n", rv);
        return rv;
    }
    *egress_vlan_port_id = vlan_port.vlan_port_id;
    printf("egress_vlan_port_id: 0x%08x\n", *egress_vlan_port_id);

    rv = bcm_vswitch_port_add(unit, vsi, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }

    /*
     * Connect the native DA to the egress vlan port
     * In case of not P2P case
     */
    if(!is_cross_connect)
    {
        bcm_l2_addr_t_init(&l2addr, mac, vsi);
        l2addr.flags |= BCM_L2_STATIC;
        l2addr.port = *egress_vlan_port_id;
        rv = bcm_l2_addr_add(unit, l2addr);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in bcm_l2_addr_add\n", rv);
            return rv;
        }
    }
    return rv;
}

/*
 * Creating p2p connection from the native AC to the out AC
 * The connection is used for core->access packets.
 */
int
vpls_pwe_tagged_mode_add_cross_connect_native_ac(
    int unit,
    bcm_gport_t ingress_vlan_port_id,
    bcm_gport_t egress_vlan_port_id)
{
    int rv = BCM_E_NONE;
    bcm_vswitch_cross_connect_t gports;

    bcm_vswitch_cross_connect_t_init(&gports);
    gports.port1 = ingress_vlan_port_id;
    gports.port2 = egress_vlan_port_id;
    gports.encap1 = BCM_FORWARD_ENCAP_ID_INVALID;
    gports.encap2 = BCM_FORWARD_ENCAP_ID_INVALID;
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    rv = bcm_vswitch_cross_connect_add(unit, gports);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_dnx_vswitch_cross_connect_add\n");
        return rv;
    }
    return rv;
}

/*
 * Creating p2p connection from the access AC to the Native AC encapsulated in pwe packets
 * The access AC contain the FEC and the MPLS tunnel.
 * FEC is pointing to the out Native AC
 * The connection is used for access->core packets.
 */
int
vpls_pwe_tagged_mode_add_cross_connect_access_ac(
    int unit,
    bcm_gport_t ac_inlif,
    bcm_if_t encap_id,
    int mpls_fec_id)
{
    int rv = BCM_E_NONE;
    bcm_vswitch_cross_connect_t gports;

    bcm_vswitch_cross_connect_t_init(&gports);
    gports.port1 = ac_inlif;
    BCM_GPORT_FORWARD_PORT_SET(gports.port2, mpls_fec_id);
    gports.encap2 = encap_id;
    gports.encap1 = BCM_FORWARD_ENCAP_ID_INVALID;
    gports.flags = BCM_VSWITCH_CROSS_CONNECT_DIRECTIONAL;
    rv = bcm_vswitch_cross_connect_add(unit, gports);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_dnx_vswitch_cross_connect_add\n");
        return rv;
    }
    return rv;
}

/*
 * Define AC for inner ETH validation on packets going from customer to core.
 * This creates a SW DB entry that is later used by VLAN translation APIs for setting inner ETH editting.
 * The resulting vlan_port_id is saved in vlan_port_id.
 */
int
vpls_pwe_tagged_mode_add_core_native_ac(
    int unit,
    bcm_vpn_t vsi,
    bcm_gport_t mpls_port_id,
    bcm_gport_t * vlan_port_id,
    int is_virtual,
    int *encap_id)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;
    /*
     * add port.
     */
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_CREATE_EGRESS_ONLY;
    if (is_virtual)
    {
        vlan_port.flags |= BCM_VLAN_PORT_VLAN_TRANSLATION | BCM_VLAN_PORT_VLAN_TRANSLATION_TWO_VLAN_TAGS;
    }
    vlan_port.vsi = vsi;
    if (!is_virtual)
    {
        vlan_port.tunnel_id = mpls_port_id;
        vlan_port.flags |= BCM_VLAN_PORT_WITH_ID;
        BCM_GPORT_SUB_TYPE_LIF_SET(vlan_port.vlan_port_id,
            BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, *vlan_port_id);
        BCM_GPORT_VLAN_PORT_ID_SET(vlan_port.vlan_port_id, vlan_port.vlan_port_id);
        
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    }
    else
    {
        vlan_port.port = mpls_port_id;
        vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    }
    
    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_vlan_port_create\n", rv);
        return rv;
    }
    /*
     * Save the created vlan_port_id
     */
    *vlan_port_id = vlan_port.vlan_port_id;
    *encap_id = vlan_port.encap_id;

    printf("core native ac = 0x%x\n", *vlan_port_id);

    return rv;
}

/*
 * Main function for basic mpls vpls scenario.
 */
int
vpls_pwe_tagged_mode_basic_main(
    int unit,
    int access_port,
    int core_port)
{
    int rv = BCM_E_NONE;
    int nof_labels = 2;
    int dummy_encap_id;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    vpls_pwe_tagged_mode_basic_init(access_port, core_port);

    rv = vpls_pwe_tagged_mode_check_support(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_check_support\n", rv);
        return rv;
    }

    /*
     * Configure AC and PWE ports
     */
    rv = vpls_pwe_tagged_mode_basic_configure_port_properties(unit, cint_vpls_pwe_tagged_mode_basic_info.access_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif, FALSE);
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
    rv = vpls_pwe_tagged_mode_basic_create_fec_entry(unit, cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id,
                                                     cint_vpls_pwe_tagged_mode_basic_info.mpls_tunnel_id,
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_fec_entry\n", rv);
        return rv;
    }

    /*
     * Configure a termination label for the ingress flow  
     */
    bcm_mpls_label_t label_array[2];
    label_array[0] = cint_vpls_pwe_tagged_mode_basic_info.service_mpls_tunnel_label_1;
    label_array[1] = cint_vpls_pwe_tagged_mode_basic_info.service_mpls_tunnel_label_2;
    rv = vpls_pwe_tagged_mode_basic_create_termination_stack(unit, label_array, 2);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_termination_stack\n", rv);
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

    /*
     * configure PWE tunnel - ingress flow 
     */
    rv = vpls_pwe_tagged_mode_basic_mpls_port_add_termination(unit,
                                                              &(cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_ing),
                                                              cint_vpls_pwe_tagged_mode_basic_info.service_pwe_label,
                                                              &(cint_vpls_pwe_tagged_mode_basic_info.pwe_encap_id),
                                                              cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id,
                                                              cint_vpls_pwe_tagged_mode_basic_info.service_vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_mpls_port_add_termination\n", rv);
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
     * Set tag formats for un tagged and double tagged packets
     */
    rv = vpls_pwe_tagged_mode_basic_tag_formats_set(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_tag_formats_set\n", rv);
        return rv;
    }

    /*
     * Configure EVE for access to core traffic: add two service tags 
     */
    rv = vlan_translate_ive_eve_translation_set(unit, cint_vpls_pwe_tagged_mode_basic_info.outer_ac_vlan_port_id,
                                                cint_vpls_pwe_tagged_mode_basic_info.outer_tpid,
                                                cint_vpls_pwe_tagged_mode_basic_info.inner_tpid, bcmVlanActionAdd,
                                                bcmVlanActionAdd,
                                                cint_vpls_pwe_tagged_mode_basic_info.outer_ac_egress_outer_vlan,
                                                cint_vpls_pwe_tagged_mode_basic_info.outer_ac_egress_inner_vlan,
                                                cint_vpls_pwe_tagged_mode_basic_info.egress_vlan_edit_profile,
                                                cint_vpls_pwe_tagged_mode_basic_info.tag_format_untagged, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in vlan_translate_ive_eve_translation_set, port=%d, \n", port);
        return rv;
    }

    /*
     * Add AC for inner ETH IVE (removing/replacing service tags)
     * For Ingress flow
     */
    printf("Inner AC glifless configuration\n\n");
    rv = vpls_pwe_tagged_mode_add_network_native_ac(unit, cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                    cint_vpls_pwe_tagged_mode_basic_info.service_mac_1,
                                                    &
                                                    (cint_vpls_pwe_tagged_mode_basic_info.ingress_native_ac_vlan_port_id),
                                                    &
                                                    (cint_vpls_pwe_tagged_mode_basic_info.access_egress_vlan_port_id),
                                                    cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_ing,
                                                    cint_vpls_pwe_tagged_mode_basic_info.ingress_canonical_outer_vlan,
                                                    cint_vpls_pwe_tagged_mode_basic_info.ingress_canonical_inner_vlan,
                                                    cint_vpls_pwe_tagged_mode_basic_info.access_port, 1, FALSE, FALSE, FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_add_network_native_ac\n", rv);
        return rv;
    }

    /*
     * Configure IVE for core to access traffic: delete two service tags 
     */
    rv = vlan_translate_ive_eve_translation_set(unit,
                                                cint_vpls_pwe_tagged_mode_basic_info.ingress_native_ac_vlan_port_id,
                                                0,
                                                0,
                                                bcmVlanActionDelete,
                                                bcmVlanActionDelete,
                                                0,
                                                0,
                                                cint_vpls_pwe_tagged_mode_basic_info.ingress_vlan_edit_profile,
                                                cint_vpls_pwe_tagged_mode_basic_info.tag_format_double_tagged, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in vlan_translate_ive_eve_translation_set, port=0x%08x, \n",
               cint_vpls_pwe_tagged_mode_basic_info.ingress_native_ac_vlan_port_id);
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
    /*
     * Add EVE for the AC created in the previous step
     */
    rv = vlan_translate_ive_eve_translation_set(unit,
                                                cint_vpls_pwe_tagged_mode_basic_info.egress_native_ac_vlan_port_id,
                                                cint_vpls_pwe_tagged_mode_basic_info.outer_tpid,
                                                cint_vpls_pwe_tagged_mode_basic_info.inner_tpid,
                                                bcmVlanActionAdd,
                                                bcmVlanActionAdd,
                                                cint_vpls_pwe_tagged_mode_basic_info.egress_canonical_outer_vlan,
                                                cint_vpls_pwe_tagged_mode_basic_info.egress_canonical_inner_vlan,
                                                cint_vpls_pwe_tagged_mode_basic_info.egress_vlan_edit_profile,
                                                cint_vpls_pwe_tagged_mode_basic_info.tag_format_untagged, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in vlan_translate_ive_eve_translation_set, port=0x%08x, \n",
               cint_vpls_pwe_tagged_mode_basic_info.egress_native_ac_vlan_port_id);
        return rv;
    }

    return rv;
}

/*
 * Main function indexed native ac scenario.
 */
int
vpls_pwe_tagged_mode_basic_indexed_native_ac_main(
    int unit,
    int access_port,
    int core_port,
    int ingress_ac_native_wide_data_enable)
{
    int rv = BCM_E_NONE;
    int nof_labels = 2;
    int is_set_global_lif = is_oam ? TRUE: FALSE;

    cint_vpls_pwe_tagged_mode_basic_info.ingress_native_ac_vlan_port_id = *dnxc_data_get(unit, "l3", "rif", "nof_rifs", NULL) + 0x234;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    vpls_pwe_tagged_mode_basic_init(access_port, core_port);

    rv = vpls_pwe_tagged_mode_check_support(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_check_support\n", rv);
        return rv;
    }

    /*
     * Configure AC and PWE ports
     */
    rv = vpls_pwe_tagged_mode_basic_configure_port_properties(unit, cint_vpls_pwe_tagged_mode_basic_info.access_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif, FALSE);
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
    rv = vpls_pwe_tagged_mode_basic_create_fec_entry(unit, cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id,
                                                     cint_vpls_pwe_tagged_mode_basic_info.mpls_tunnel_id,
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_fec_entry\n", rv);
        return rv;
    }

    /*
     * Configure a termination label for the ingress flow  
     */
    bcm_mpls_label_t label_array[2];
    label_array[0] = cint_vpls_pwe_tagged_mode_basic_info.service_mpls_tunnel_label_1;
    label_array[1] = cint_vpls_pwe_tagged_mode_basic_info.service_mpls_tunnel_label_2;
    rv = vpls_pwe_tagged_mode_basic_create_termination_stack(unit, label_array, 2);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_termination_stack\n", rv);
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

    /*
     * configure PWE tunnel - ingress flow 
     */
    rv = vpls_pwe_tagged_mode_basic_mpls_port_add_termination(unit,
                                                              &(cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_ing),
                                                              cint_vpls_pwe_tagged_mode_basic_info.service_pwe_label,
                                                              &(cint_vpls_pwe_tagged_mode_basic_info.pwe_encap_id),
                                                              cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id,
                                                              cint_vpls_pwe_tagged_mode_basic_info.service_vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_mpls_port_add_termination\n", rv);
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
     * Set tag formats for un tagged and double tagged packets
     */
    rv = vpls_pwe_tagged_mode_basic_tag_formats_set(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_tag_formats_set\n", rv);
        return rv;
    }

    /*
     * Configure EVE for access to core traffic: add two service tags 
     */
    rv = vlan_translate_ive_eve_translation_set(unit, cint_vpls_pwe_tagged_mode_basic_info.outer_ac_vlan_port_id,
                                                cint_vpls_pwe_tagged_mode_basic_info.outer_tpid,
                                                cint_vpls_pwe_tagged_mode_basic_info.inner_tpid, bcmVlanActionAdd,
                                                bcmVlanActionAdd,
                                                cint_vpls_pwe_tagged_mode_basic_info.outer_ac_egress_outer_vlan,
                                                cint_vpls_pwe_tagged_mode_basic_info.outer_ac_egress_inner_vlan,
                                                cint_vpls_pwe_tagged_mode_basic_info.egress_vlan_edit_profile,
                                                cint_vpls_pwe_tagged_mode_basic_info.tag_format_untagged, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in vlan_translate_ive_eve_translation_set, port=%d, \n", port);
        return rv;
    }

    /*
     * Add AC for inner ETH IVE (removing/replacing service tags)
     * For Ingress flow. Also add the outer AC that is the result of 
     * Ethernet forwarding with the exposed DA. 
     */
    rv = vpls_pwe_tagged_mode_add_network_native_ac(unit, cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                    cint_vpls_pwe_tagged_mode_basic_info.service_mac_1,
                                                    &
                                                    (cint_vpls_pwe_tagged_mode_basic_info.ingress_native_ac_vlan_port_id),
                                                    &
                                                    (cint_vpls_pwe_tagged_mode_basic_info.access_egress_vlan_port_id),
                                                    cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_ing,
                                                    cint_vpls_pwe_tagged_mode_basic_info.ingress_canonical_outer_vlan,
                                                    cint_vpls_pwe_tagged_mode_basic_info.ingress_canonical_inner_vlan,
                                                    cint_vpls_pwe_tagged_mode_basic_info.access_port, 0, 
                                                    ingress_ac_native_wide_data_enable, FALSE, is_set_global_lif);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_add_network_native_ac\n", rv);
        return rv;
    }
    /*
     * Add IVE for the AC created in the previous step
     */
    rv = vlan_translate_ive_eve_translation_set(unit,
                                                cint_vpls_pwe_tagged_mode_basic_info.ingress_native_ac_vlan_port_id,
                                                0,
                                                0,
                                                bcmVlanActionDelete,
                                                bcmVlanActionDelete,
                                                0,
                                                0,
                                                cint_vpls_pwe_tagged_mode_basic_info.ingress_vlan_edit_profile,
                                                cint_vpls_pwe_tagged_mode_basic_info.tag_format_double_tagged, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in vlan_translate_ive_eve_translation_set, port=0x%08x, \n",
               cint_vpls_pwe_tagged_mode_basic_info.ingress_native_ac_vlan_port_id);
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
    /*
     * Add EVE for the AC created in the previous step
     */
    rv = vlan_translate_ive_eve_translation_set(unit,
                                                cint_vpls_pwe_tagged_mode_basic_info.egress_native_ac_vlan_port_id,
                                                cint_vpls_pwe_tagged_mode_basic_info.outer_tpid,
                                                cint_vpls_pwe_tagged_mode_basic_info.inner_tpid,
                                                bcmVlanActionAdd, bcmVlanActionAdd,
                                                cint_vpls_pwe_tagged_mode_basic_info.egress_canonical_outer_vlan,
                                                cint_vpls_pwe_tagged_mode_basic_info.egress_canonical_inner_vlan,
                                                cint_vpls_pwe_tagged_mode_basic_info.egress_vlan_edit_profile,
                                                cint_vpls_pwe_tagged_mode_basic_info.tag_format_untagged, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in vlan_translate_ive_eve_translation_set, port=0x%08x, \n",
               cint_vpls_pwe_tagged_mode_basic_info.egress_native_ac_vlan_port_id);
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

/* Set tag formats for un tagged and double tagged packets*/
int
vpls_pwe_tagged_mode_basic_tag_formats_set(
    int unit)
{
    int rv = BCM_E_NONE;

    bcm_port_tpid_class_t port_tpid_class;
    /*
     * set tag format for untagged packets 
     */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = cint_vpls_pwe_tagged_mode_basic_info.core_port;
    port_tpid_class.tpid1 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = cint_vpls_pwe_tagged_mode_basic_info.tag_format_untagged;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port);
        return rv;
    }

    /*
     * set tag format for double tagged packets 
     */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_ing;
    port_tpid_class.tpid1 = cint_vpls_pwe_tagged_mode_basic_info.outer_tpid;
    port_tpid_class.tpid2 = cint_vpls_pwe_tagged_mode_basic_info.inner_tpid;
    port_tpid_class.tag_format_class_id = cint_vpls_pwe_tagged_mode_basic_info.tag_format_double_tagged;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port);
        return rv;
    }

    return rv;
}


/*
 * Main function for basic mpls vpls scenario.
 */
int
vpls_pwe_tagged_mode_p2p_basic_main(
    int unit,
    int access_port,
    int core_port)
{
    int rv = BCM_E_NONE;
    int nof_labels = 2;
    int dummy_encap_id;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    vpls_pwe_tagged_mode_basic_init(access_port, core_port);

    rv = vpls_pwe_tagged_mode_check_support(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_check_support\n", rv);
        return rv;
    }

    /*
     * Configure AC and PWE ports
     */
    rv = vpls_pwe_tagged_mode_basic_configure_port_properties(unit, cint_vpls_pwe_tagged_mode_basic_info.access_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif, TRUE);
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
    rv = vpls_pwe_tagged_mode_basic_create_fec_entry(unit, cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id,
                                                     cint_vpls_pwe_tagged_mode_basic_info.mpls_tunnel_id,
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_fec_entry\n", rv);
        return rv;
    }

    /*
     * Configure a termination label for the ingress flow
     */
    bcm_mpls_label_t label_array[2];
    label_array[0] = cint_vpls_pwe_tagged_mode_basic_info.service_mpls_tunnel_label_1;
    label_array[1] = cint_vpls_pwe_tagged_mode_basic_info.service_mpls_tunnel_label_2;
    rv = vpls_pwe_tagged_mode_basic_create_termination_stack(unit, label_array, 2);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_termination_stack\n", rv);
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

    /*
     * configure PWE tunnel - ingress flow
     */
    rv = vpls_pwe_tagged_mode_basic_mpls_port_add_termination(unit,
                                                              &(cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_ing),
                                                              cint_vpls_pwe_tagged_mode_basic_info.service_pwe_label,
                                                              &(cint_vpls_pwe_tagged_mode_basic_info.pwe_encap_id),
                                                              cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id,
                                                              cint_vpls_pwe_tagged_mode_basic_info.service_vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_mpls_port_add_termination\n", rv);
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
     * cross connecting the access ac port id with the pwe and mpls tunnels
     */
    rv = vpls_pwe_tagged_mode_add_cross_connect_access_ac(unit,
                                                cint_vpls_pwe_tagged_mode_basic_info.access_port_ac_in_lif,
                                                cint_vpls_pwe_tagged_mode_basic_info.pwe_encap_id, cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_add_cross_connect_access_ac\n", rv);
        return rv;
    }

    /*
     * Set tag formats for un tagged and double tagged packets
     */
    rv = vpls_pwe_tagged_mode_basic_tag_formats_set(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_tag_formats_set\n", rv);
        return rv;
    }

    /*
     * Configure EVE for access to core traffic: add two service tags
     */
    rv = vlan_translate_ive_eve_translation_set(unit, cint_vpls_pwe_tagged_mode_basic_info.outer_ac_vlan_port_id,
                                                cint_vpls_pwe_tagged_mode_basic_info.outer_tpid,
                                                cint_vpls_pwe_tagged_mode_basic_info.inner_tpid, bcmVlanActionAdd,
                                                bcmVlanActionAdd,
                                                cint_vpls_pwe_tagged_mode_basic_info.outer_ac_egress_outer_vlan,
                                                cint_vpls_pwe_tagged_mode_basic_info.outer_ac_egress_inner_vlan,
                                                cint_vpls_pwe_tagged_mode_basic_info.egress_vlan_edit_profile,
                                                cint_vpls_pwe_tagged_mode_basic_info.tag_format_untagged, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in vlan_translate_ive_eve_translation_set, port=%d, \n", port);
        return rv;
    }

    /*
     * Add AC for inner ETH IVE (removing/replacing service tags)
     * For Ingress flow
     */
    printf("Inner AC glifless configuration\n\n");
    rv = vpls_pwe_tagged_mode_add_network_native_ac(unit, cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                    cint_vpls_pwe_tagged_mode_basic_info.service_mac_1,
                                                    &
                                                    (cint_vpls_pwe_tagged_mode_basic_info.ingress_native_ac_vlan_port_id),
                                                    &
                                                    (cint_vpls_pwe_tagged_mode_basic_info.access_egress_vlan_port_id),
                                                    cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_ing,
                                                    cint_vpls_pwe_tagged_mode_basic_info.ingress_canonical_outer_vlan,
                                                    cint_vpls_pwe_tagged_mode_basic_info.ingress_canonical_inner_vlan,
                                                    cint_vpls_pwe_tagged_mode_basic_info.access_port, 0, FALSE, TRUE, FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_add_network_native_ac\n", rv);
        return rv;
    }

    /*
     * Add Cross connect in case of p2p
     */
    rv = vpls_pwe_tagged_mode_add_cross_connect_native_ac(unit,
                                                cint_vpls_pwe_tagged_mode_basic_info.ingress_native_ac_vlan_port_id,
                                                cint_vpls_pwe_tagged_mode_basic_info.access_egress_vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_add_cross_connect_native_ac\n", rv);
        return rv;
    }
    /*
     * Configure IVE for core to access traffic: delete two service tags
     */
    rv = vlan_translate_ive_eve_translation_set(unit,
                                                cint_vpls_pwe_tagged_mode_basic_info.ingress_native_ac_vlan_port_id,
                                                0,
                                                0,
                                                bcmVlanActionDelete,
                                                bcmVlanActionDelete,
                                                0,
                                                0,
                                                cint_vpls_pwe_tagged_mode_basic_info.ingress_vlan_edit_profile,
                                                cint_vpls_pwe_tagged_mode_basic_info.tag_format_double_tagged, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in vlan_translate_ive_eve_translation_set, port=0x%08x, \n",
               cint_vpls_pwe_tagged_mode_basic_info.ingress_native_ac_vlan_port_id);
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
    /*
     * Add EVE for the AC created in the previous step
     */
    rv = vlan_translate_ive_eve_translation_set(unit,
                                                cint_vpls_pwe_tagged_mode_basic_info.egress_native_ac_vlan_port_id,
                                                cint_vpls_pwe_tagged_mode_basic_info.outer_tpid,
                                                cint_vpls_pwe_tagged_mode_basic_info.inner_tpid,
                                                bcmVlanActionAdd,
                                                bcmVlanActionAdd,
                                                cint_vpls_pwe_tagged_mode_basic_info.egress_canonical_outer_vlan,
                                                cint_vpls_pwe_tagged_mode_basic_info.egress_canonical_inner_vlan,
                                                cint_vpls_pwe_tagged_mode_basic_info.egress_vlan_edit_profile,
                                                cint_vpls_pwe_tagged_mode_basic_info.tag_format_untagged, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in vlan_translate_ive_eve_translation_set, port=0x%08x, \n",
               cint_vpls_pwe_tagged_mode_basic_info.egress_native_ac_vlan_port_id);
        return rv;
    }

    return rv;
}

/*
 * Main function indexed native ac scenario.
 */
int
vpls_pwe_tagged_mode_basic_indexed_native_ac_p2p_main(
    int unit,
    int access_port,
    int core_port,
    int ingress_ac_native_wide_data_enable)
{
    int rv = BCM_E_NONE;
    int nof_labels = 2;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    vpls_pwe_tagged_mode_basic_init(access_port, core_port);

    rv = vpls_pwe_tagged_mode_check_support(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_check_support\n", rv);
        return rv;
    }

    /*
     * Configure AC and PWE ports
     */
    rv = vpls_pwe_tagged_mode_basic_configure_port_properties(unit, cint_vpls_pwe_tagged_mode_basic_info.access_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_port,
                                                              cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                              cint_vpls_pwe_tagged_mode_basic_info.core_eth_rif, TRUE);
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
    rv = vpls_pwe_tagged_mode_basic_create_fec_entry(unit, cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id,
                                                     cint_vpls_pwe_tagged_mode_basic_info.mpls_tunnel_id,
                                                     cint_vpls_pwe_tagged_mode_basic_info.core_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_fec_entry\n", rv);
        return rv;
    }

    /*
     * Configure a termination label for the ingress flow
     */
    bcm_mpls_label_t label_array[2];
    label_array[0] = cint_vpls_pwe_tagged_mode_basic_info.service_mpls_tunnel_label_1;
    label_array[1] = cint_vpls_pwe_tagged_mode_basic_info.service_mpls_tunnel_label_2;
    rv = vpls_pwe_tagged_mode_basic_create_termination_stack(unit, label_array, 2);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_create_termination_stack\n", rv);
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

    /*
     * configure PWE tunnel - ingress flow
     */
    rv = vpls_pwe_tagged_mode_basic_mpls_port_add_termination(unit,
                                                              &(cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_ing),
                                                              cint_vpls_pwe_tagged_mode_basic_info.service_pwe_label,
                                                              &(cint_vpls_pwe_tagged_mode_basic_info.pwe_encap_id),
                                                              cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id,
                                                              cint_vpls_pwe_tagged_mode_basic_info.service_vsi);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_mpls_port_add_termination\n", rv);
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
     * Set tag formats for un tagged and double tagged packets
     */
    rv = vpls_pwe_tagged_mode_basic_tag_formats_set(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_basic_tag_formats_set\n", rv);
        return rv;
    }

    /*
     * Configure EVE for access to core traffic: add two service tags
     */
    rv = vlan_translate_ive_eve_translation_set(unit, cint_vpls_pwe_tagged_mode_basic_info.outer_ac_vlan_port_id,
                                                cint_vpls_pwe_tagged_mode_basic_info.outer_tpid,
                                                cint_vpls_pwe_tagged_mode_basic_info.inner_tpid, bcmVlanActionAdd,
                                                bcmVlanActionAdd,
                                                cint_vpls_pwe_tagged_mode_basic_info.outer_ac_egress_outer_vlan,
                                                cint_vpls_pwe_tagged_mode_basic_info.outer_ac_egress_inner_vlan,
                                                cint_vpls_pwe_tagged_mode_basic_info.egress_vlan_edit_profile,
                                                cint_vpls_pwe_tagged_mode_basic_info.tag_format_untagged, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in vlan_translate_ive_eve_translation_set, port=%d, \n", port);
        return rv;
    }

    /*
     * Add AC for inner ETH IVE (removing/replacing service tags)
     * For Ingress flow. Also add the outer AC that is the result of
     * Ethernet forwarding with the exposed DA.
     */
    rv = vpls_pwe_tagged_mode_add_network_native_ac(unit, cint_vpls_pwe_tagged_mode_basic_info.service_vsi,
                                                    cint_vpls_pwe_tagged_mode_basic_info.service_mac_1,
                                                    &
                                                    (cint_vpls_pwe_tagged_mode_basic_info.ingress_native_ac_vlan_port_id),
                                                    &
                                                    (cint_vpls_pwe_tagged_mode_basic_info.access_egress_vlan_port_id),
                                                    cint_vpls_pwe_tagged_mode_basic_info.mpls_port_id_ing,
                                                    cint_vpls_pwe_tagged_mode_basic_info.ingress_canonical_outer_vlan,
                                                    cint_vpls_pwe_tagged_mode_basic_info.ingress_canonical_inner_vlan,
                                                    cint_vpls_pwe_tagged_mode_basic_info.access_port, 0,
                                                    ingress_ac_native_wide_data_enable, TRUE, FALSE);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_add_network_native_ac\n", rv);
        return rv;
    }

    /*
     * Add Cross connect in case of p2p
     */
    rv = vpls_pwe_tagged_mode_add_cross_connect_native_ac(unit,
                                                cint_vpls_pwe_tagged_mode_basic_info.ingress_native_ac_vlan_port_id,
                                                cint_vpls_pwe_tagged_mode_basic_info.access_egress_vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_add_cross_connect_native_ac\n", rv);
        return rv;
    }

    /*
     * Add IVE for the AC created in the previous step
     */
    rv = vlan_translate_ive_eve_translation_set(unit,
                                                cint_vpls_pwe_tagged_mode_basic_info.ingress_native_ac_vlan_port_id,
                                                0,
                                                0,
                                                bcmVlanActionDelete,
                                                bcmVlanActionDelete,
                                                0,
                                                0,
                                                cint_vpls_pwe_tagged_mode_basic_info.ingress_vlan_edit_profile,
                                                cint_vpls_pwe_tagged_mode_basic_info.tag_format_double_tagged, 1);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in vlan_translate_ive_eve_translation_set, port=0x%08x, \n",
               cint_vpls_pwe_tagged_mode_basic_info.ingress_native_ac_vlan_port_id);
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
    /*
     * Add EVE for the AC created in the previous step
     */
    rv = vlan_translate_ive_eve_translation_set(unit,
                                                cint_vpls_pwe_tagged_mode_basic_info.egress_native_ac_vlan_port_id,
                                                cint_vpls_pwe_tagged_mode_basic_info.outer_tpid,
                                                cint_vpls_pwe_tagged_mode_basic_info.inner_tpid,
                                                bcmVlanActionAdd, bcmVlanActionAdd,
                                                cint_vpls_pwe_tagged_mode_basic_info.egress_canonical_outer_vlan,
                                                cint_vpls_pwe_tagged_mode_basic_info.egress_canonical_inner_vlan,
                                                cint_vpls_pwe_tagged_mode_basic_info.egress_vlan_edit_profile,
                                                cint_vpls_pwe_tagged_mode_basic_info.tag_format_untagged, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in vlan_translate_ive_eve_translation_set, port=0x%08x, \n",
               cint_vpls_pwe_tagged_mode_basic_info.egress_native_ac_vlan_port_id);
        return rv;
    }

    rv = vpls_pwe_tagged_mode_add_cross_connect_access_ac(unit,
                                                cint_vpls_pwe_tagged_mode_basic_info.access_port_ac_in_lif,
                                                cint_vpls_pwe_tagged_mode_basic_info.indexed_ac_encap_id, cint_vpls_pwe_tagged_mode_basic_info.mpls_fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_pwe_tagged_mode_add_cross_connect_access_ac\n", rv);
        return rv;
    }
    return rv;
}

