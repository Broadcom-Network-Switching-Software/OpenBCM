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
 * cint ../../../../src/examples/sand/cint_vpls_mp_basic.c
 * cint ../../../../src/examples/dpp/cint_advanced_vlan_translation_mode.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int in_port = 200;
 * int out_port = 201;
 * rv = vpls_mp_basic_main(unit,in_port,out_port);
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
 */

struct cint_vpls_basic_info_s
{
    int ac_port;                                   /* incoming port from ac side */
    int pwe_port;                                  /* incoming port from pwe side */
    int pwe_intf;                                  /* pwe RIF */
    int pwe_intf_primary;                          /* pwe RIF for primary */
    bcm_vpn_t vpn;                                 /* vsi id */
    bcm_gport_t mpls_port_id_ingress;              /* global lif encoded as MPLS port, used for ingress, must be as egress */
    bcm_gport_t mpls_port_id_ingress_primary;      /* global lif encoded as MPLS port, used for ingress, must be as egress */
    bcm_gport_t mpls_port_id_egress;               /* global lif encoded as MPLS port, used for egress, must be as ingress */
    bcm_gport_t mpls_port_id_egress_primary;       /* global lif encoded as MPLS port, used for egress, must be as ingress */
    bcm_gport_t vlan_port_id;                      /* vlan port id of the ac_port, used for ingress */
    bcm_if_t encap_id;                             /* The global lif of the EEDB PWE entry. */
    bcm_if_t encap_id_primary;
    int core_arp_id;                               /* Id for ac ARP entry */
    int core_arp_id_primary;                       /* Id for ac ARP entry for primary */
    int mpls_encap_fec_id;                         /* ac fec id for encapsulation entry - outlif for MPLS entry in EEDB */
    int mpls_encap_fec_id_primary;
    bcm_mac_t ac_intf_mac_address;                 /* mac for ac RIF */
    bcm_mac_t ac_mac_address;                      /* mac for ingress next hop */
    bcm_mac_t pwe_intf_mac_address;                /* mac for pwe RIF */
    bcm_mac_t pwe_arp_next_hop_mac;                /* mac for egress next hop */
    bcm_if_t mpls_tunnel_id;                       /* tunnel id for encapsulation entry */
    bcm_if_t mpls_tunnel_id_primary;               /* tunnel id for encapsulation entry */
    bcm_mpls_label_t mpls_tunnel_label;            /* pushed label */
    bcm_mpls_label_t mpls_tunnel_label_primary;    /* pushed label */
    bcm_mpls_label_t pwe_label;                    /* pwe label */
    bcm_mpls_label_t pwe_label_primary;            /* pwe label */
    bcm_mpls_label_t pwe_termination_label;        /* label to be terminated */
    int cw_present;                                /* indication to whether CW is present */
    bcm_if_t mpls_tunnel_id_ingress;               /* tunnel id for termination entry */
    bcm_failover_t pwe_failover_id_fec;
    bcm_failover_t pwe_failover_id_outlif;
    int skip_auto_fec_allocation;                  /* In case the FEC ID needs to be allocated outside of the cint */
};

cint_vpls_basic_info_s cint_vpls_basic_info = {
    /*
     * ports : ac_port | pwe_port
     */
    200, 201,
    /*
     * pwe_intf | pwe_intf_primary
     */
    30, 31,
    /*
     * vpn
     */
    5,
    /*
     * mpls_port_id_ingress | mpls_port_id_ingress_primary |  mpls_port_id_egress | mpls_port_id_egress_primary | vlan_port_id
     */
    9999, 9997, 8888, 8886, 0,
    /*
     * encap_id | encap_id_primary
     */
    8888, 8886,
    /*
     * core_arp_id | core_arp_id_primary
     */
    9001, 9003,
    /*
     * mpls_encap_fec_id | mpls_encap_fec_id_primary
     */
    50000, 50001,
    /*
     * ac_intf_mac_address |  ac_mac_address
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}, {0x00, 0x11, 0x00, 0x00, 0x01, 0x12},
    /*
     * pwe_intf_mac_address | pwe_arp_next_hop_mac
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x01}, {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d},
    /*
     * mpls_tunnel_id | mpls_tunnel_id_primary
     */
    8196, 8198,
    /*
     * mpls_tunnel_label | mpls_tunnel_label_primary , pwe_label | pwe_label_primary
     */
    3333, 3335, 3456, 3458,
    /*
     * pwe_termination_label
     */
    3333,
    /*
     * cw_present
     */
    0,
    /*
     * skip_auto_fec_allocation
     */
    0
};

int verbose1 = 1;
int pwe_protection_with_one_fec = 0;
/*Global variable for Section OAM */
/*AC LIF used for Section OAM */
bcm_gport_t section_oam_in_gport = 0;
/*Section OAM indication */
int is_section_oam = 0;
int jr1_mode_remap = 0;

/*
 * Configure and associate symmetric LIF with the ingress port in case of Section OAM
 * - in_port: Incoming port ID
 */
int
in_port_section_oam_intf_set(
    int unit,
    int in_port)
{
    bcm_vlan_port_t vlan_port;
    int rc;
    char *proc_name;
    bcm_port_match_info_t match_info;

    proc_name = "in_port_section_oam_intf_set";
    bcm_vlan_port_t_init(&vlan_port);
    vlan_port.port = in_port;
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_NONE;
    rc = bcm_vlan_port_create(unit, vlan_port);
    if (rc != BCM_E_NONE)
    {
        printf("%s(): Error, bcm_vlan_port_create\n", proc_name);
        return rc;
    }

    section_oam_in_gport = vlan_port.vlan_port_id;
    printf("%s(): port = %d, in_lif = 0x%08X\n", proc_name, vlan_port.port, vlan_port.vlan_port_id);

    /*Associate in-port to AC LIF */
    bcm_port_match_info_t_init(&match_info);
    match_info.match = BCM_PORT_MATCH_PORT;
    match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY;
    /*In-lif for MPLS section OAM */
    if(is_device_or_above(unit, JERICHO2))
    {
        match_info.match_ethertype = 0x8847;
    }
    match_info.port = in_port;
    rc = bcm_port_match_add(unit, vlan_port.vlan_port_id, &match_info);
    if (rc != 0)
    {
        printf("%s(): Error, in bcm_port_match_add \n",proc_name);
        return rc;
    }
    return rc;
}

int
vpls_mp_basic_configure_port_properties(
    int unit,
    int ac_port,
    int pwe_port)
{
    int rv = BCM_E_NONE;

    /*
     * set class for both ports
     */
    rv = bcm_port_class_set(unit, ac_port, bcmPortClassId, ac_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_class_set, port=%d, \n", ac_port);
        return rv;
    }

    rv = bcm_port_class_set(unit, pwe_port, bcmPortClassId, pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_class_set, port=%d, \n", pwe_port);
        return rv;
    }

    cint_vpls_basic_info.ac_port = ac_port;
    cint_vpls_basic_info.pwe_port = pwe_port;

    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, cint_vpls_basic_info.pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set intf_out\n");
        return rv;
    }

    if (!is_section_oam) {
        /*
         * Set In-Port to In ETh-RIF
         */
        rv = in_port_intf_set(unit, cint_vpls_basic_info.pwe_port, cint_vpls_basic_info.pwe_intf);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in_port_intf_set intf_in\n");
            return rv;
        }
    } else {
        /*
         * Configuring and associating symmetric LIF with the ingress port in case of Section OAM
         */
        rv = in_port_section_oam_intf_set(unit, cint_vpls_basic_info.pwe_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in_port_section_oam_intf_set \n");
            return rv;
        }
    }
    /*
     * Set Out-Port default properties
     */
    rv = out_port_set(unit, cint_vpls_basic_info.ac_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set intf_out\n");
        return rv;
    }

    return rv;
}

int
vpls_mp_basic_create_l3_interfaces(
    int unit)
{

    int rv;

    /*
     * Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, cint_vpls_basic_info.pwe_intf, cint_vpls_basic_info.pwe_intf_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create pwe_intf\n");
    }

    if (pwe_protection_with_one_fec)
    {
        rv = intf_eth_rif_create(unit, cint_vpls_basic_info.pwe_intf_primary, cint_vpls_basic_info.pwe_intf_mac_address);
        if (rv != BCM_E_NONE)
        {
            printf("Error, intf_eth_rif_create pwe_intf\n");
        }
    }

    return rv;
}

int
vpls_mp_basic_create_arp_entry(
    int unit)
{
    int rv;

    /*
     * Configure ARP entry
     */
    rv = l3__egress_only_encap__create(unit, 0, cint_vpls_basic_info.core_arp_id,
                                       cint_vpls_basic_info.pwe_arp_next_hop_mac, cint_vpls_basic_info.pwe_intf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
    }

    if (pwe_protection_with_one_fec)
    {
        rv = l3__egress_only_encap__create(unit, 0, cint_vpls_basic_info.core_arp_id_primary,
                                           cint_vpls_basic_info.pwe_arp_next_hop_mac, cint_vpls_basic_info.pwe_intf_primary);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create AC egress object ARP only\n");
        }
    }

    return rv;

}

/*
* Update predefined ARP entry with required MPLS tunnel.
*
* Relevant only for Jericho devices.
*/
int
vpls_mp_basic_update_arp_entry(
    int unit)
{
    int rv;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;       /* not intersting */
    uint32 flags = BCM_L3_WITH_ID | BCM_L3_REPLACE | BCM_L3_EGRESS_ONLY;

    bcm_l3_egress_t_init(&l3eg);

    l3eg.mac_addr = cint_vpls_basic_info.pwe_arp_next_hop_mac;
    l3eg.encap_id = cint_vpls_basic_info.core_arp_id;
    l3eg.vlan = cint_vpls_basic_info.pwe_intf;
    l3eg.intf = cint_vpls_basic_info.mpls_tunnel_id;

    rv = bcm_l3_egress_create(unit, flags, &l3eg, &l3egid_null);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only\n");
    }

    return rv;
}

int
vpls_mp_basic_create_failover (
    int unit,
    bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;

    /* create failover id for PWE */
    rv = bcm_failover_create(unit, BCM_FAILOVER_FEC, &cint_vpls_basic_info.pwe_failover_id_fec);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for PWE FEC, rv - %d\n", rv);
        return rv;
    }
    printf("PWE Failover id: 0x%x\n", cint_vpls_basic_info.pwe_failover_id_fec);


    /* create egress failover id for PWE */
    rv = bcm_failover_create(unit, BCM_FAILOVER_ENCAP, &cint_vpls_basic_info.pwe_failover_id_outlif);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_failover_create for PWE Egress, rv - %d\n", rv);
        return rv;
    }
    printf("PWE egress Failover id: 0x%x\n", cint_vpls_basic_info.pwe_failover_id_outlif);

    return rv;
}


int
vpls_mp_basic_create_fec_entry(
    int unit)
{
    int rv;
    int temp_out_rif;
    int out_rif_or_tunnel;
    int flags = 0;

    /**
     * Under below scenarios, this case tests the FEC format of dest+EEI:
     * 1. JR2 works under JR1 system headers mode ;
     * 2. JR1 device;
     */
    if ((!is_device_or_above(unit, JERICHO2))
         || (is_device_or_above(unit, JERICHO2) && (soc_property_get(unit, "system_headers_mode", 1) == 0)))
    {
        flags |= BCM_L3_ENCAP_SPACE_OPTIMIZED;
    }

    if (is_device_or_above(unit, JERICHO2))
    {
        if(!cint_vpls_basic_info.skip_auto_fec_allocation)
            {
            /*
             * Jericho 2 support static FEC allocation as other cints uses the first FEC ID add +8
             */
            rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &cint_vpls_basic_info.mpls_encap_fec_id);
            cint_vpls_basic_info.mpls_encap_fec_id += 8;
            if (rv != BCM_E_NONE)
            {
                printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
                return rv;
            }
        }
    }
    else
    {
        cint_vpls_basic_info.mpls_encap_fec_id = 10000;
    }
    /*
     * Create FEC:
     * 1) Give a fec id.
     * 2) Tunnel id of the encapsulation entry.
     * 3) Arp id will be given with the only_encap call, so give 0.
     * 4) Give the out port.
     */

    if (pwe_protection_with_one_fec)
    {

        /* Create backup FEC */
        cint_vpls_basic_info.mpls_encap_fec_id++;
        BCM_L3_ITF_SET(out_rif_or_tunnel, BCM_L3_ITF_TYPE_LIF, cint_vpls_basic_info.encap_id);
        rv = l3__egress_only_fec__create_inner(unit, cint_vpls_basic_info.mpls_encap_fec_id,
                                               out_rif_or_tunnel, 0, cint_vpls_basic_info.pwe_port, flags, cint_vpls_basic_info.pwe_failover_id_fec, 0, NULL);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC only\n");
        }

        /* Create primary FEC */
        BCM_L3_ITF_SET(out_rif_or_tunnel, BCM_L3_ITF_TYPE_LIF, cint_vpls_basic_info.encap_id_primary);
        cint_vpls_basic_info.mpls_encap_fec_id_primary = cint_vpls_basic_info.mpls_encap_fec_id - 1;
        rv = l3__egress_only_fec__create_inner(unit, cint_vpls_basic_info.mpls_encap_fec_id_primary,
                                               out_rif_or_tunnel, 0, cint_vpls_basic_info.pwe_port, flags, cint_vpls_basic_info.pwe_failover_id_fec, 1, NULL);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC only\n");
        }
    }
    else
    {
        rv = l3__egress_only_fec__create(unit, cint_vpls_basic_info.mpls_encap_fec_id,
                                         cint_vpls_basic_info.mpls_tunnel_id, 0, cint_vpls_basic_info.pwe_port, flags);
        if (rv != BCM_E_NONE)
        {
            printf("Error, create egress object FEC only\n");
        }
    }

    /*
     * The termination scenario doesn't need FEC.
     * We bridge out of the core with no usage of FEC.
     */

    return rv;
}

int
vpls_mp_basic_create_mpls_tunnel(
    int unit)
{

    bcm_mpls_egress_label_t label_array[1];
    int num_labels = 1;
    int rv;
    int is_primary = 0;
    int nof_objects = pwe_protection_with_one_fec ? 2 : 1;
    bcm_mpls_label_t mpls_tunnel_label;
    bcm_if_t         mpls_tunnel_id;
    int              core_arp_id;

    for (is_primary = 0; is_primary < nof_objects; is_primary++)
    {
        bcm_mpls_egress_label_t_init(&label_array[0]);

        if (is_primary)
        {
            mpls_tunnel_label = cint_vpls_basic_info.mpls_tunnel_label_primary;
            mpls_tunnel_id = cint_vpls_basic_info.mpls_tunnel_id_primary;
            core_arp_id = cint_vpls_basic_info.core_arp_id_primary;
        }
        else
        {
            mpls_tunnel_label = cint_vpls_basic_info.mpls_tunnel_label;
            mpls_tunnel_id = cint_vpls_basic_info.mpls_tunnel_id;
            core_arp_id = cint_vpls_basic_info.core_arp_id;
        }

        /*
         * Label to be pushed as part of the MPLS tunnel.
         */
        label_array[0].label = mpls_tunnel_label;
        if (is_device_or_above(unit, JERICHO2))
        {
            label_array[0].flags = BCM_MPLS_EGRESS_LABEL_WITH_ID;

            BCM_L3_ITF_SET(label_array[0].tunnel_id, BCM_L3_ITF_TYPE_LIF, mpls_tunnel_id);

            /*
             * Set the next pointer of this entry to be the arp. This configuration is new compared to Jericho, where the arp
             * pointer used to be connected to the EEDB entry via bcm_l3_egress_create (with egress flag indication).
             */
            label_array[0].l3_intf_id = core_arp_id;
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

        if (is_primary)
        {
            cint_vpls_basic_info.mpls_tunnel_id_primary = label_array[0].tunnel_id;
        }
        else {
            cint_vpls_basic_info.mpls_tunnel_id = label_array[0].tunnel_id;
        }
    }

    return rv;

}

int
vpls_mp_basic_create_termination_stack(
    int unit)
{
    bcm_mpls_tunnel_switch_t entry;
    int rv;
    int propogation_profile_flags = BCM_MPLS_SWITCH_OUTER_EXP | BCM_MPLS_SWITCH_OUTER_TTL;
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
    entry.label = cint_vpls_basic_info.pwe_termination_label;
    if (soc_property_get(unit, "mpls_termination_label_index_enable", 0))
    {
        int label = cint_vpls_basic_info.pwe_termination_label;
        BCM_MPLS_INDEXED_LABEL_SET(entry.label, label, 1);
    }

    if (is_device_or_above(unit, JERICHO2))
    {
        entry.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
        entry.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
        entry.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
    }
    else
    {
        entry.flags |= propogation_profile_flags;
    }

    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create\n");
        return rv;
    }
    cint_vpls_basic_info.mpls_tunnel_id_ingress = entry.tunnel_id;
    return rv;

}

int
vpls_mp_basic_l2_addr_add(
    int unit,
    bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2addr;

    bcm_l2_addr_t_init(&l2addr, cint_vpls_basic_info.ac_mac_address, vpn);
    l2addr.flags |= BCM_L2_STATIC;
    l2addr.port = cint_vpls_basic_info.vlan_port_id;

    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_l2_addr_add\n", rv);
        return rv;
    }

    return rv;
}

int
vpls_mp_basic_mpls_port_add_encapsulation(
    int unit)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;
    int is_primary;
    int nof_objects = pwe_protection_with_one_fec ? 2 : 1;

    for (is_primary = 0; is_primary < nof_objects; is_primary++)
    {
        bcm_mpls_port_t_init(&mpls_port);
        mpls_port.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
        if (!is_device_or_above(unit, JERICHO2))
        {
            mpls_port.flags |= BCM_MPLS_PORT_ENCAP_WITH_ID;
            mpls_port.encap_id = cint_vpls_basic_info.encap_id;
        }
        mpls_port.flags2 |= BCM_MPLS_PORT2_EGRESS_ONLY;

        if (cint_vpls_basic_info.cw_present)
        {
            mpls_port.flags |= BCM_MPLS_PORT_CONTROL_WORD;
        }

        BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, cint_vpls_basic_info.mpls_port_id_egress);
        BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
        mpls_port.egress_label.label = is_primary ? cint_vpls_basic_info.pwe_label_primary : cint_vpls_basic_info.pwe_label;
        mpls_port.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
        if (!is_device_or_above(unit, JERICHO2))
        {
            mpls_port.egress_label.flags |= BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        }

        if (pwe_protection_with_one_fec)
        {
            mpls_port.egress_failover_id = cint_vpls_basic_info.pwe_failover_id_outlif;
            if (is_primary) {
                mpls_port.egress_failover_port_id = cint_vpls_basic_info.encap_id;
            }

            mpls_port.egress_tunnel_if = is_primary ? cint_vpls_basic_info.mpls_tunnel_id_primary : cint_vpls_basic_info.mpls_tunnel_id;

            mpls_port.flags |= BCM_MPLS_PORT_ENCAP_WITH_ID;
            mpls_port.encap_id = is_primary ? cint_vpls_basic_info.encap_id_primary : cint_vpls_basic_info.encap_id;
            /* mpls_port_id is the outlif id encapsulated as MPLS_PORT */
            if (is_device_or_above(unit, JERICHO2))
            {
                /* Jr2 requires subtype of mpls_port_id.*/
                BCM_GPORT_SUB_TYPE_LIF_SET(mpls_port.mpls_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_EGRESS_ONLY, mpls_port.encap_id);
            }
            BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, mpls_port.mpls_port_id);
        }

        rv = bcm_mpls_port_add(unit, 0, mpls_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in bcm_mpls_port_add encapsulation\n", rv);
            return rv;
        }

        if (is_primary)
        {
            cint_vpls_basic_info.mpls_port_id_egress_primary = mpls_port.mpls_port_id;
            cint_vpls_basic_info.encap_id_primary = mpls_port.encap_id;
        }
        else
        {
            cint_vpls_basic_info.mpls_port_id_egress = mpls_port.mpls_port_id;
            cint_vpls_basic_info.encap_id = mpls_port.encap_id;
        }
    }

    return rv;
}

int
vpls_mp_basic_mpls_port_add_termination(
    int unit,
    bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;
    int is_primary;
    int nof_objects = pwe_protection_with_one_fec ? 2 : 1;

    for (is_primary = 0; is_primary < nof_objects; is_primary++)
    {

        bcm_mpls_port_t_init(&mpls_port);
        mpls_port.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;

        if (cint_vpls_basic_info.cw_present)
        {
            mpls_port.flags |= BCM_MPLS_PORT_CONTROL_WORD;
        }

        if (!is_device_or_above(unit, JERICHO2))
        {
            mpls_port.flags |= BCM_MPLS_PORT_ENCAP_WITH_ID;
            mpls_port.flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY | BCM_MPLS_PORT2_OUTER_EXP | BCM_MPLS_PORT2_OUTER_TTL;
            /** We are using EEI(outlif) model in Jericho which is analogous to J2. For this LEARN_ENCAP flag is required */
            mpls_port.flags2 |= BCM_MPLS_PORT2_LEARN_ENCAP;
        }
        else
        {
            mpls_port.flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY | (vpn ? 0 : BCM_MPLS_PORT2_CROSS_CONNECT);
            mpls_port.ingress_qos_model.ingress_phb = bcmQosIngressModelUniform;
            mpls_port.ingress_qos_model.ingress_remark = bcmQosIngressModelUniform;
            mpls_port.ingress_qos_model.ingress_ttl = bcmQosIngressModelUniform;
        }

        if (is_primary)
        {
            BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, cint_vpls_basic_info.mpls_port_id_ingress_primary);
        }
        else
        {
            BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, cint_vpls_basic_info.mpls_port_id_ingress);
        }
        BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
        /*
         * encap_id is the egress outlif - used for learning
         */
        mpls_port.encap_id = is_primary ? cint_vpls_basic_info.encap_id_primary : cint_vpls_basic_info.encap_id;
        mpls_port.criteria = BCM_MPLS_PORT_MATCH_LABEL;
        mpls_port.match_label = is_primary ? cint_vpls_basic_info.pwe_label_primary : cint_vpls_basic_info.pwe_label;
        if (soc_property_get(unit, "mpls_termination_label_index_enable", 0))
        {
            int label = cint_vpls_basic_info.pwe_label;
            BCM_MPLS_INDEXED_LABEL_SET(mpls_port.match_label, label, 2);
        }

        /** connect PWE entry to created MPLS encapsulation entry for learning */
        if (pwe_protection_with_one_fec)
        {
            BCM_L3_ITF_SET(mpls_port.failover_port_id, BCM_L3_ITF_TYPE_FEC, cint_vpls_basic_info.mpls_encap_fec_id_primary);
        }
        else
        {
            if (jr1_mode_remap == 0)
            {
                BCM_L3_ITF_SET(mpls_port.egress_tunnel_if, BCM_L3_ITF_TYPE_FEC, cint_vpls_basic_info.mpls_encap_fec_id);
            }
            else
            {
                /** In case of JR1 mode and FEC remapping between learned entry destination and forwarding destination,
                 * use mpls_encap_fec_id_primary as FEC ID which will be learned
                 */
                BCM_L3_ITF_SET(mpls_port.egress_tunnel_if, BCM_L3_ITF_TYPE_FEC, cint_vpls_basic_info.mpls_encap_fec_id_primary);
            }
        }

        if (pwe_protection_with_one_fec)
        {
            /* Jr2 requires subtype of mpls_port_id.*/
            BCM_GPORT_SUB_TYPE_LIF_SET(mpls_port.mpls_port_id, BCM_GPORT_SUB_TYPE_LIF_EXC_INGRESS_ONLY, mpls_port.encap_id);
            mpls_port.encap_id = 0;
            BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, mpls_port.mpls_port_id);
        }

        rv = bcm_mpls_port_add(unit, vpn, mpls_port);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in bcm_mpls_port_add termination\n", rv);
            return rv;
        }

        if (is_primary)
        {
            cint_vpls_basic_info.mpls_port_id_ingress_primary = mpls_port.mpls_port_id;
        }
        else
        {
            cint_vpls_basic_info.mpls_port_id_ingress = mpls_port.mpls_port_id;
        }
    }

    return rv;
}

int
vpls_mp_basic_vswitch_add_access_port(
    int unit,
    bcm_gport_t port_id)
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
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = port_id;
    vlan_port.match_vlan = cint_vpls_basic_info.vpn;
    vlan_port.egress_vlan = is_device_or_above(unit, JERICHO2) ? 0 : cint_vpls_basic_info.vpn;
    vlan_port.vsi = 0;
    vlan_port.flags = 0;

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }
    cint_vpls_basic_info.vlan_port_id = vlan_port.vlan_port_id;

    rv = bcm_vswitch_port_add(unit, cint_vpls_basic_info.vpn, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }
    rv = bcm_vlan_gport_add(unit, cint_vpls_basic_info.vpn, port_id, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_gport_add \n");
        return rv;
    }

    return rv;
}

int
vpls_mp_check_support(
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

int
vpls_mp_basic_ive_swap(int unit, int port)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;

    bcm_port_tpid_class_t port_tpid_class;
    /* set tag format 0 for untagged packets */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = 0x8100;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = 4;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO | BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port);
        return rv;
    }

    /* configure ive swap */
    rv = ive_eve_translation_set( unit,  cint_vpls_basic_info.vlan_port_id, 0x8100, 0, bcmVlanActionReplace, bcmVlanActionNone,  1111,  0, 5 /*edit_profile*/, 4 /*tag_format*/, 1);
    if (rv != BCM_E_NONE) {
        printf("Error(%d), in ive_eve_translation_set\n", rv);
        return rv;
    }

    if(is_device_or_above(unit, JERICHO2))
    {
        int next_layer_network_domain = vpls_mpls_next_layer_network_domain_value(unit, 3);
        rv = bcm_port_class_set(unit, cint_vpls_basic_info.mpls_port_id_ingress, bcmPortClassIngress, next_layer_network_domain);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in bcm_port_class_set, port=0x%x, \n", cint_vpls_basic_info.mpls_port_id_ingress);
            return rv;
        }

        /*
         * add port.
         */
        bcm_vlan_port_t_init(&vlan_port);
        vlan_port.flags = BCM_VLAN_PORT_NATIVE | BCM_VLAN_PORT_CREATE_INGRESS_ONLY;
        vlan_port.flags |= BCM_VLAN_PORT_VLAN_TRANSLATION;
        vlan_port.vsi = 5;
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

        printf("native ac = 0x%08x\n", vlan_port.vlan_port_id);

        /*
         * Add Match
         */
        bcm_port_match_info_t match_info;
        bcm_port_match_info_t_init(&match_info);
        match_info.flags = BCM_PORT_MATCH_INGRESS_ONLY | BCM_PORT_MATCH_NATIVE;
        match_info.port = cint_vpls_basic_info.mpls_port_id_ingress;
        match_info.match = BCM_PORT_MATCH_PORT;        /* Match 2 tags */
        rv = bcm_port_match_add(unit, vlan_port.vlan_port_id, &match_info);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in bcm_port_match_add\n", rv);
            return rv;
        }
        printf("match_port done\n");
        /* configure ive swap */
        rv = ive_eve_translation_set( unit,  vlan_port.vlan_port_id, 0x8100, 0, bcmVlanActionReplace, bcmVlanActionNone,  1111,  0, 5 /*edit_profile*/, 8 /*tag_format*/, 1);
        if (rv != BCM_E_NONE) {
            printf("Error(%d), in ive_eve_translation_set\n", rv);
            return rv;
        }
    }

    return rv;
}

/*
 * Main function for basic mpls vpls scenario.
 */
int
vpls_mp_basic_main(
    int unit,
    int ac_port,
    int pwe_port)
{
    int rv = BCM_E_NONE;

    rv = vpls_mp_check_support(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_mp_check_support\n", rv);
        return rv;
    }

    if (pwe_protection_with_one_fec)
    {
        /*
         * configure faiover for PWE tunnel - egress flow
         */
        rv = vpls_mp_basic_create_failover(unit, cint_vpls_basic_info.vpn);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in vpls_mp_basic_create_failover\n", rv);
            return rv;
        }
    }

    cint_vpls_basic_info.ac_port = ac_port;
    cint_vpls_basic_info.pwe_port = pwe_port;

    /*
     * Configure AC and PWE ports
     */
    rv = vpls_mp_basic_configure_port_properties(unit, ac_port, pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_mp_basic_configure_port_properties\n", rv);
        return rv;
    }

    /*
     * create vlan based on the vsi (vpn)
     */
    rv = bcm_vlan_create(unit, cint_vpls_basic_info.vpn);
    if (rv != BCM_E_NONE)
    {
        printf("Failed (%d) to create VLAN %d\n", rv, cint_vpls_basic_info.vpn);
        return rv;
    }

    if (!is_device_or_above(unit, JERICHO2))
    {
        egress_mc = cint_vpls_basic_info.vpn;
        rv = mpls_port__vswitch_vpls_vpn_create__set(unit, cint_vpls_basic_info.vpn);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in mpls_port__vswitch_vpls_vpn_create__set\n", rv);
            return rv;
        }
    }

    if(is_section_oam) {
        if (is_device_or_above(unit, JERICHO2))
        {
            rv = bcm_vlan_create(unit, cint_vpls_basic_info.pwe_intf);
            if (rv != BCM_E_NONE)
            {
                printf("Error(%d), in mpls_port__vswitch_vpls_vpn_create__set\n", rv);
                return rv;
            }
        }
        rv = bcm_vswitch_port_add(unit, cint_vpls_basic_info.pwe_intf, section_oam_in_gport);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vswitch_port_add\n");
            return rv;
        }
    }

    /*
     * Configure L3 interfaces
     */
    rv = vpls_mp_basic_create_l3_interfaces(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_mp_basic_create_l3_interfaces\n", rv);
        return rv;
    }

    /*
     * Configure an ARP entries
     */
    rv = vpls_mp_basic_create_arp_entry(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_mp_basic_create_arp_entry\n", rv);
        return rv;
    }

    /*
     * Configure a push entry.
     */
    rv = vpls_mp_basic_create_mpls_tunnel(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_mp_basic_create_mpls_tunnel\n", rv);
        return rv;
    }

    /*
     * Configure fec entry
     */
    rv = vpls_mp_basic_create_fec_entry(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_mp_basic_create_fec_entry\n", rv);
        return rv;
    }

    /*
     * Configure a termination label for the ingress flow
     */
    rv = vpls_mp_basic_create_termination_stack(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_mp_basic_create_termination_stack\n", rv);
        return rv;
    }

    /*
     * In non jericho 2, ARP entry configuration is done with bcm_l3_egress_create and a tunnel id of the MPLS entry.
     * It connects the MPLS entry to the ARP, Whereas in Jericho 2, bcm_mpls_tunnel_initiator_create is used to connect the MPLS to the given arp id.
     * Therefore, we need to update the ARP entry with correct MPLS tunnel.
     */
    if (!is_device_or_above(unit, JERICHO2))
    {
        rv = vpls_mp_basic_update_arp_entry(unit);
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), in vpls_basic_update_arp_entry\n", rv);
            return rv;
        }
    }

    /*
     * configure PWE tunnel - egress flow
     */
    rv = vpls_mp_basic_mpls_port_add_encapsulation(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_mp_basic_mpls_port_add_encapsulation\n", rv);
        return rv;
    }

    /*
     * configure PWE tunnel - ingress flow
     */
    rv = vpls_mp_basic_mpls_port_add_termination(unit, cint_vpls_basic_info.vpn);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_mp_basic_mpls_port_add_termination\n", rv);
        return rv;
    }

    /** define ac and pwe ports */
    rv = vpls_mp_basic_vswitch_add_access_port(unit, ac_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_mp_basic_vswitch_add_access_port\n", rv);
        return rv;
    }

    /*
     * add l2 addresses to be defined as static - no learning needed
     */
    rv = vpls_mp_basic_l2_addr_add(unit, cint_vpls_basic_info.vpn);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_mp_basic_l2_addr_add\n", rv);
        return rv;
    }

    return rv;
}
