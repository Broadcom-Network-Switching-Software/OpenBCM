/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_mpls_encapsulation_basic.c Purpose: Various examples for MPLS/VPLS.
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
 * cint ../../../../src/examples/sand/cint_mpls_encapsulation_basic.c
 * cint
 * int unit = 0;
 * int rv = 0;
 * int in_port = 200;
 * int out_port = 201;
 * int ttl_value = 0;
 * rv = mpls_encapsulation_basic_main(unit,in_port,out_port,ttl_value);
 * print rv;
 *
 *
 *  Scenarios configured in this cint:
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  1) Basic MPLS forwarding
 *  ~~~~~~~~~~~~~~~~~~~~~~~~
 *  An MPLS label is swapped. Lookup in ILM results in an EEDB entry holding swap information.
 *  The ILM points to the EEDB (outlif) and to a FEC entry (holding the next hop information).
 *
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA      | SA           ||   MPLS   ||  SIP     ||  DIP          ||  TTL ||
 *   |    |0C:00:02:00:00|11:00:00:01:12||Label:7777||160.0.0.17||160.161.161.162|| 64   ||
 *   |    |              |              ||Exp:0     ||          ||               ||      ||
 *   |    |              |              ||TTL:20    ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA      | SA           ||   MPLS   ||  SIP     ||  DIP          ||  TTL ||
 *   |    |00:00:00:CD:1D|0C:00:02:00:01||Label:3333||160.0.0.17||160.161.161.162|| 63   ||
 *   |    |              |              ||Exp:0     ||          ||               ||      ||
 *   |    |              |              ||TTL:20    ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  2) Basic MPLS Encapsulation
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Route into an MPLS core. Exit with a packet including an MPLS label
 *
 *  Traffic:
 *
 *  Send the following traffic:
 *
 *  ##############################################################################################
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA      | SA           ||  SIP     ||  DIP          ||  TTL ||
 *   |    |0C:00:02:00:00|11:00:00:01:12||160.0.0.17||160.161.161.163|| 64   ||
 *   |    |              |              ||          ||               ||      ||
 *   |    |              |              ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA      | SA           ||   MPLS   ||  SIP     ||  DIP          ||  TTL ||
 *   |    |00:00:00:CD:1D|0C:00:02:00:01||Label:3333||160.0.0.17||160.161.161.163|| 63   ||
 *   |    |              |              ||Exp:0     ||          ||               ||      ||
 *   |    |              |              ||TTL:20    ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *
 * Note: All three scenarios use the same EEDB entry for the relevant information. The label
 *       in the entry will swap the label(s) that was (were) matched (the "swapped" label(s)) in the ILM
 *       in case the forwarding context is MPLS. The label in the entry will be pushed in case
 *       the forwarding context is IP.
 */

struct cint_mpls_encapsulation_basic_info_s
{
    int in_port;                /* incoming port */
    int out_port;               /* outgoing port */
    int intf_in;                /* in RIF */
    int intf_out;               /* out RIF */
    bcm_mac_t intf_in_mac_address;      /* mac for tin RIF */
    bcm_mac_t intf_out_mac_address;     /* mac for out RIF */
    bcm_mac_t arp_next_hop_mac; /* mac for next hop */
    int vrf;                    /* VRF */
    int arp_id_push;            /* Id for ARP entry for push/swap entries */
    int arp_id_php;             /* Id for ARP entry for php entries */
    bcm_ip_t dip;               /* dip 1 */
    uint32 mask;                /* mask for dip */
    int fec_id_enc;             /* fec id for encapsulation entry - outlif for MPLS entry in EEDB */
    int fec_id_fwd;             /* fec id for FWD entry - outlif for ARP entry in EEDB */
    int fec_id_php;             /* fec id for FWD entry - outlif for ARP entry in EEDB */
    int fec_id_swap;            /* fec id for FWD entry - outlif for ARP entry in EEDB */
    int push_tunnel_id;         /* tunnel id for encapsulation entry */
    int php_tunnel_id;          /* tunnel id for encapsulation php entry to be popped */
    bcm_mpls_label_t pushed_or_swapping_label;  /* pushed or swapping label */
    bcm_mpls_label_t swapped_label;     /* label to be swapped */
    bcm_mpls_label_t php_label; /* php label to be popped */
    int ingress_qos_profile;    /* qos profile, ingress */
    int egress_qos_profile;     /* qos profile, egress */
    int eei_as_outlif;          /* Use EEI as Out-LIF, JR1 system header mode only */
    int svtag_enable;           /* enable and SVTAG lookup */
    bcm_ip6_t ipv6_dip;         /* ipv6 forwarding dip */
};

cint_mpls_encapsulation_basic_info_s cint_mpls_encapsulation_basic_info =
    /*
     * ports : in_port | out_port
     */
{ 200, 201,
    /*
     * intf_in | intf_out
     */
    20, 42,
    /*
     * intf_in_mac_address | intf_out_mac_address | arp_next_hop_mac |
     */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00}, {0x00, 0x0c, 0x00, 0x02, 0x0f, 0x55}, {0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d},
    /*
     * vrf
     */
    1,
    /*
     * arp_id_push | arp_id_php
     */
    0, 0,
    /*
     * dip
     */
    0xA0A1A1A3 /* 160.161.161.163 */ ,
    /*
     * mask
     */
    0xfffffff0,
    /*
     * fec_id_enc | fec_id_fwd | fec_id_php | fec_id_swap
     */
    0x2000C351, 0x2000C353, 0x2000C355, 0x2000C357,
    /*
     * push_tunnel_id, php_tunnel_id
     */
    0, 0,
    /*
     * pushed_or_swapping_label |  swapped_label | php_label
     */
    3333, 7777, 3377,
    /*
     * ingress_qos_profile | egress_qos_profile |eei_as_outlif
     */
    3, 1, 0,
     /*
     * svtag_enable
     */
    0,

    /*
     * ipv6_dip
     */
    {0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8,
     0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xec}
};

static int ing_qos_profile = -1;
static int egr_qos_profile = -1;
int fec_id_deviation = 0;

int
mpls_encapsulation_basic_check_support(
    int unit)
{
    int rv = BCM_E_NONE;
    int is_qax_a0 = 0;

    rv = is_qumran_ax_a0_only(unit, &is_qax_a0);
    if (rv != BCM_E_NONE)
    {
        printf("Failed(%d) is_qumran_ax_only\n", rv);
        return rv;
    }
    if (!((!is_device_or_above(unit, JERICHO2) && is_device_or_above(unit, JERICHO_PLUS) && !is_qax_a0) ||
          is_device_or_above(unit, JERICHO2)))
    {
        printf("Only Jericho2 and Jericho_plus are supported\n");
        return BCM_E_CONFIG;
    }

    return rv;
}

/*
 * Main function for basic swap and push mpls scnenarios in Jericho 2: 1) Swap scenario: Do swap with ILM pointing to an EEDB swap
 * entry  2) Push scenario: Forward into an MPLS core with 1 label.
 * ttl_value - If ttl_value!=0, tunnel is configured in PIPE mode, which will set ttl_value in out packet
 *             If ttl_value==0, configuration is UNIFORM, which takes the ttl_value from in_packet
 */
int
mpls_encapsulation_basic_main(
    int unit,
    int port1,
    int port2,
    uint8 ttl_value)
{
    int rv = BCM_E_NONE;


    if (is_device_or_above(unit, JERICHO2))
    {
        int first_fec_id_in_hierarchy;
        rv = get_first_fec_in_range_which_not_in_ecmp_range(unit,0,&first_fec_id_in_hierarchy);
        first_fec_id_in_hierarchy += 70;
        if (rv != BCM_E_NONE)
        {
            printf("Error(%d), get_first_fec_in_range_which_not_in_ecmp_range\n", rv);
            return rv;
        }
        cint_mpls_encapsulation_basic_info.fec_id_enc = first_fec_id_in_hierarchy++;
        cint_mpls_encapsulation_basic_info.fec_id_fwd = first_fec_id_in_hierarchy++;
        cint_mpls_encapsulation_basic_info.fec_id_php = first_fec_id_in_hierarchy++;
        cint_mpls_encapsulation_basic_info.fec_id_swap = first_fec_id_in_hierarchy;
    }


    rv = mpls_encapsulation_basic_check_support(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in mpls_encapsulation_basic_check_support\n", rv);
        return rv;
    }

    cint_mpls_encapsulation_basic_info.in_port = port1;
    cint_mpls_encapsulation_basic_info.out_port = port2;

    /*
     * Check if user's qos profile is valid.
     */
    if (ing_qos_profile != -1)
    {
        cint_mpls_encapsulation_basic_info.ingress_qos_profile = ing_qos_profile;
    }

    if (egr_qos_profile != -1)
    {
        cint_mpls_encapsulation_basic_info.egress_qos_profile = egr_qos_profile;
    }

    /*
     * Configure port properties for this application
     */
    rv = mpls_encapsulation_basic_configure_port_properties(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_basic_configure_port_properties\n");
        return rv;
    }

    /*
     * Configure L3 interfaces
     */
    rv = mpls_encapsulation_basic_create_l3_interfaces(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_basic_create_l3_interfaces\n");
        return rv;
    }

    /*
     * Configure an ARP entry
     */
    rv = mpls_encapsulation_basic_create_arp_entry(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_basic_create_arp_entry\n");
        return rv;
    }

    /*
     * Configure a push or swap entry. The label in the entry will swap the swapped label in case
     * context is forwarding. In case context is ip routing, label will be pushed.
     */
    rv = mpls_encapsulation_basic_create_tunnels(unit, ttl_value);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_encapsulation_basic_create_tunnels\n");
        return rv;
    }

    /*
     * Configure fec entry for encapsulation flow
     */
    rv = mpls_encapsulation_basic_create_fec_entry_enc(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_encapsulation_basic_create_fec_entry_push\n");
        return rv;
    }
    /*
     * Configure fec entry for forwarding flow
     */
    rv = mpls_encapsulation_basic_create_fec_entry(unit,
                                                   cint_mpls_encapsulation_basic_info.fec_id_fwd,
                                                   cint_mpls_encapsulation_basic_info.push_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_encapsulation_basic_create_fec_entry_swap\n");
        return rv;
    }

    /*
     * Configure fec entry for php flow
     */
    rv = mpls_encapsulation_basic_create_fec_entry(unit,
                                                   cint_mpls_encapsulation_basic_info.fec_id_php,
                                                   cint_mpls_encapsulation_basic_info.php_tunnel_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_encapsulation_basic_create_fec_entry_php\n");
        return rv;
    }

    /*
     * Configure an ILM entries
     */
    rv = mpls_encapsulation_basic_create_ilm_switch_tunnel(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_encapsulation_basic_create_ilm_switch_tunnel\n");
        return rv;
    }

    /*
     * Connect MPLS tunnel entry to ARP.
     * ARP entry configuration can be also done inside bcm_mpls_tunnel_inititator_create.
     */
    rv = mpls_encapsulation_basic_update_arp_entry(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_basic_create_arp_entry\n");
        return rv;
    }

    /*
     * Create l3 forwarding entry for the pushed label
     */
    rv = mpls_encapsulation_basic_create_l3_forwarding(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_encapsulation_basic_create_l3_forwarding\n");
        return rv;
    }
    return rv;
}

int
mpls_encapsulation_basic_configure_port_properties(
    int unit)
{

    int rv;
    /*
     * Set In-Port to In ETh-RIF
     */
    rv = in_port_intf_set(unit, cint_mpls_encapsulation_basic_info.in_port, cint_mpls_encapsulation_basic_info.intf_in);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in_port_intf_set intf_in\n");
        return rv;
    }

    /*
     * Set Out-Port default properties
     */

    rv = out_port_set(unit, cint_mpls_encapsulation_basic_info.out_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_intf_set intf_out\n");
        return rv;
    }

    return rv;
}

int
mpls_encapsulation_basic_create_l3_interfaces(
    int unit)
{

    int rv;
    l3_ingress_intf ingr_itf;
    l3_ingress_intf_init(&ingr_itf);
    /*
     * Create ETH-RIF and set its properties
     */
    rv = intf_eth_rif_create(unit, cint_mpls_encapsulation_basic_info.intf_in,
                             cint_mpls_encapsulation_basic_info.intf_in_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_in\n");
        return rv;
    }
    rv = intf_eth_rif_create(unit, cint_mpls_encapsulation_basic_info.intf_out,
                             cint_mpls_encapsulation_basic_info.intf_out_mac_address);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    /*
     * Set Incoming ETH-RIF properties
     */
    ingr_itf.intf_id = cint_mpls_encapsulation_basic_info.intf_in;
    bcm_qos_map_id_get_by_profile(unit, BCM_QOS_MAP_INGRESS | BCM_QOS_MAP_PHB | BCM_QOS_MAP_REMARK,
                            cint_mpls_encapsulation_basic_info.ingress_qos_profile, &ingr_itf.qos_map);
    ingr_itf.vrf = cint_mpls_encapsulation_basic_info.vrf;
    rv = intf_ingress_rif_set(unit, &ingr_itf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create intf_out\n");
        return rv;
    }

    return rv;
}

int
mpls_encapsulation_basic_create_fec_entry_enc(
    int unit)
{

    int rv;
    int flag = 0;

    if (is_device_or_above(unit, JERICHO2) &&
        (soc_property_get(unit, spn_SYSTEM_HEADERS_MODE, 1) == 0))
    {
       flag = BCM_L3_ENCAP_SPACE_OPTIMIZED;
    }
    /*
     * Create FEC:
     * 1) Give a fec id.
     * 2) Tunnel id of the encapsulation entry.
     * 3) Arp id will be given with the only_encap call, so give 0.
     * 4) Give the out port.
     */
    rv = l3__egress_only_fec__create(unit, cint_mpls_encapsulation_basic_info.fec_id_enc,
                                     cint_mpls_encapsulation_basic_info.push_tunnel_id, 0,
                                     cint_mpls_encapsulation_basic_info.out_port, flag);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    return rv;
}

int
mpls_encapsulation_basic_create_fec_entry(
    int unit,
    int fec_id,
    int tunnel_id)
{
    int rv;
    int flags;

    /**
     * Under below scenarios, this case tests the FEC format of dest+EEI:
     * 1. JR2 works under JR1 system headers mode and eei-as-outlif enabled;
     * 2. JR1 device;
     */
    if (!is_device_or_above(unit, JERICHO2)
        || (is_device_or_above(unit, JERICHO2) && (soc_property_get(unit, "system_headers_mode", 1) == 0)
            && (cint_mpls_encapsulation_basic_info.eei_as_outlif == 1)))
    {
        flags = BCM_L3_ENCAP_SPACE_OPTIMIZED;
    }

    /*
     * Create FEC:
     * 1) Give a fec id.
     * 2) Arp id of the forwarding arp entry.
     * 3) Arp id will be given with the only_encap call, so give 0.
     * 4) Give the out port.
     */
    rv = l3__egress_only_fec__create(unit, fec_id, tunnel_id, 0, cint_mpls_encapsulation_basic_info.out_port, flags);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    return rv;
}

int
mpls_encapsulation_basic_create_ilm_switch_tunnel(
    int unit)
{
    int rv;

    /*
     * Configure an ILM (MPLS forwarding) entry, pointing to the above EEDB push or swap entry
     */
    rv = mpls_encapsulation_basic_create_ilm_switch_tunnel_create(unit,
                                                                  cint_mpls_encapsulation_basic_info.swapped_label,
                                                                  cint_mpls_encapsulation_basic_info.fec_id_fwd);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_encapsulation_basic_create_ilm_switch_tunnel_create forwarding\n");
        return rv;
    }

    /*
     * Configure an ILM (MPLS php) entry, pointing to the above EEDB php entry
     */
    rv = mpls_encapsulation_basic_create_ilm_switch_tunnel_create(unit, cint_mpls_encapsulation_basic_info.php_label,
                                                                  cint_mpls_encapsulation_basic_info.fec_id_php);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_encapsulation_basic_create_ilm_switch_tunnel_create php\n");
        return rv;
    }

    return rv;
}

int
mpls_encapsulation_basic_create_ilm_switch_tunnel_create(
    int unit,
    bcm_mpls_label_t label,
    int fec_id)
{

    int rv;
    bcm_mpls_tunnel_switch_t entry;

    /*
     * Action is NOP since the information for the swap/php action is included in the EEDB,
     * pointed from the ILM.
     */

    bcm_mpls_tunnel_switch_t_init(&entry);

    /** incoming label */
    entry.label = label;

    /** FEC entry pointing to port */
    BCM_L3_ITF_SET(entry.egress_if, BCM_L3_ITF_TYPE_FEC, fec_id);

    entry.action = BCM_MPLS_SWITCH_ACTION_NOP;
    entry.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;

    rv = bcm_mpls_tunnel_switch_create(unit, &entry);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_switch_create NOP\n");
        return rv;
    }

    return rv;
}

int
mpls_encapsulation_basic_create_arp_entry(
    int unit)
{

    int rv;
    /*
     * Configure ARP entry for push and swap actions
     */
    rv = l3__egress_only_encap__create(unit, 0, &(cint_mpls_encapsulation_basic_info.arp_id_push),
                                       cint_mpls_encapsulation_basic_info.arp_next_hop_mac,
                                       cint_mpls_encapsulation_basic_info.intf_out);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only push\n");
        return rv;
    }

    /*
     * Configure ARP entry for php action
     */
    rv = l3__egress_only_encap__create(unit, 0, &(cint_mpls_encapsulation_basic_info.arp_id_php),
                                       cint_mpls_encapsulation_basic_info.arp_next_hop_mac,
                                       cint_mpls_encapsulation_basic_info.intf_out);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only php\n");
        return rv;
    }

    return rv;

}

/** Update predefined ARP entry with required MPLS tunnel. */
int
mpls_encapsulation_basic_update_arp_entry(
    int unit)
{
    int rv;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;       /* not intersting */
    uint32 flags = BCM_L3_WITH_ID | BCM_L3_REPLACE | BCM_L3_EGRESS_ONLY | BCM_L3_KEEP_DSTMAC | BCM_L3_KEEP_VLAN;

    /** configure arp for push action */
    bcm_l3_egress_t_init(&l3eg);
    l3eg.mac_addr = cint_mpls_encapsulation_basic_info.arp_next_hop_mac;
    l3eg.encap_id = cint_mpls_encapsulation_basic_info.arp_id_push;
    l3eg.vlan = cint_mpls_encapsulation_basic_info.intf_out;
    l3eg.intf = cint_mpls_encapsulation_basic_info.push_tunnel_id;

    rv = bcm_l3_egress_create(unit, flags, &l3eg, &l3egid_null);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only\n");
        return rv;
    }

    /** configure arp for php action */
    bcm_l3_egress_t_init(&l3eg);
    l3eg.mac_addr = cint_mpls_encapsulation_basic_info.arp_next_hop_mac;
    l3eg.encap_id = cint_mpls_encapsulation_basic_info.arp_id_php;
    l3eg.vlan = cint_mpls_encapsulation_basic_info.intf_out;
    l3eg.intf = cint_mpls_encapsulation_basic_info.php_tunnel_id;

    rv = bcm_l3_egress_create(unit, flags, &l3eg, &l3egid_null);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only\n");
        return rv;
    }

    return rv;

}

int
mpls_encapsulation_basic_create_l3_forwarding(
    int unit)
{
    int rv;

    /*
     * Create a routing entry : 1) Give the Destination
     * 2) Give the mask for the sub net of the DIP 3) Give VRF 4) Give the fec id associated with this
     * forwarding instance.
     */
    /*
     * No KAPS support in the adapter yet
     */
    /*
     * rv = add_route_ipv4(unit, cint_mpls_encapsulation_basic_info.dip, cint_mpls_encapsulation_basic_info.mask,
     * cint_mpls_encapsulation_basic_info.vrf, cint_mpls_encapsulation_basic_info.fec_id_enc);
     */
    rv = add_host_ipv4(unit, cint_mpls_encapsulation_basic_info.dip, cint_mpls_encapsulation_basic_info.vrf,
                       cint_mpls_encapsulation_basic_info.fec_id_enc, 0, 0);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv4, \n");
        return rv;
    }
    cint_mpls_encapsulation_basic_info.fec_id_enc |= 0x20000000;
    rv = add_host_ipv6(unit, cint_mpls_encapsulation_basic_info.ipv6_dip, cint_mpls_encapsulation_basic_info.vrf,
                       cint_mpls_encapsulation_basic_info.fec_id_enc);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in function add_host_ipv6, \n");
        return rv;
    }

    return rv;
}

int
mpls_encapsulation_basic_create_push_or_swap_tunnel(
    int unit,
    uint8 ttl_value)
{

    bcm_mpls_egress_label_t label_array[1];
    int num_labels = 1;
    int egress_qos_profile;
    int rv;

    bcm_mpls_egress_label_t_init(&label_array[0]);

    /*
     * Pushed or Swapping label.
     * The label will be pushed for IP forwarding context and swapped for
     * MPLS forwarding context.
     */
    label_array[0].label = cint_mpls_encapsulation_basic_info.pushed_or_swapping_label;
    /*
     * In JR2: To set the next pointer of this entry to be the arp, use label_array[0].l3_intf_id.
     * Here this link is done in later stage by updating ARP entry with
     * BCM_L3_KEEP_DSTMAC | BCM_L3_KEEP_VLAN flags
     * In JR1: Use l3_intf_id to connect outRif
     */
    if (!is_device_or_above(unit, JERICHO2))
    {
        label_array[0].l3_intf_id = cint_mpls_encapsulation_basic_info.intf_out;
    }

    /*
     * If ttl_value != 0, means this value should be taken from the
     * table. (TTL_SET)
     * In case of 0, thee value will be taken from pipe (TTL_COPY)
     */
    label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    if (ttl_value != 0)
    {
        if (is_device_or_above(unit, JERICHO2))
        {
            label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelPipeNextNameSpace;
            label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelPipeMyNameSpace;
        }
        else
        {
            label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET | BCM_MPLS_EGRESS_LABEL_EXP_SET;
        }
        label_array[0].ttl = ttl_value;
    }
    else
    {
        if (is_device_or_above(unit, JERICHO2))
        {
            label_array[0].egress_qos_model.egress_qos = bcmQosEgressModelUniform;
            label_array[0].egress_qos_model.egress_ttl = bcmQosEgressModelUniform;
        }
        else
        {
            label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_COPY | BCM_MPLS_EGRESS_LABEL_EXP_COPY;
        }
    }

    /*
     * In non-jericho2 device SWAP_OR_PUSH action needs to be defined in order to use EEDB entry for swap action.
     */
    if (!is_device_or_above(unit, JERICHO2))
    {
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID;
        label_array[0].action = BCM_MPLS_EGRESS_ACTION_SWAP_OR_PUSH;
    }

    /** qos remark profile */
    rv = bcm_qos_map_id_get_by_profile(unit, BCM_QOS_MAP_EGRESS | BCM_QOS_MAP_REMARK,
                                       cint_mpls_encapsulation_basic_info.egress_qos_profile, &egress_qos_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_qos_map_id_get_by_profile\n");
        return rv;
    }

    label_array[0].qos_map_id = egress_qos_profile;

    if(cint_mpls_encapsulation_basic_info.svtag_enable)
    {
        label_array[0].flags2 |= BCM_MPLS_EGRESS_LABEL_FLAGS2_SVTAG_ENABLE;
    }

    rv = bcm_mpls_tunnel_initiator_create(unit, 0, num_labels, label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    cint_mpls_encapsulation_basic_info.push_tunnel_id = label_array[0].tunnel_id;

    return rv;

}

int
mpls_encapsulation_basic_create_php_tunnel(
    int unit)
{

    bcm_mpls_egress_label_t label_array[1];
    int num_labels = 1;
    int rv;

    bcm_mpls_egress_label_t_init(&label_array[0]);

    label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_ACTION_VALID | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    label_array[0].action = BCM_MPLS_EGRESS_ACTION_PHP;
    label_array[0].l3_intf_id = cint_mpls_encapsulation_basic_info.arp_id_php;

    if (!is_device_or_above(unit, JERICHO2))
    {
        label_array[0].label = 0;
        label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_TTL_SET | BCM_MPLS_EGRESS_LABEL_PHP_IPV4;
        label_array[0].l3_intf_id = cint_mpls_encapsulation_basic_info.arp_id_php;
    }

    rv = bcm_mpls_tunnel_initiator_create(unit, 0, num_labels, label_array);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    cint_mpls_encapsulation_basic_info.php_tunnel_id = label_array[0].tunnel_id;

    return rv;

}

int
mpls_encapsulation_basic_create_tunnels(
    int unit,
    uint8 ttl_value)
{
    int rv;

    rv = mpls_encapsulation_basic_create_push_or_swap_tunnel(unit, ttl_value);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_encapsulation_basic_create_push_or_swap_tunnel\n");
        return rv;
    }

    rv = mpls_encapsulation_basic_create_php_tunnel(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in mpls_encapsulation_basic_create_php_tunnel\n");
        return rv;
    }

    return rv;
}

