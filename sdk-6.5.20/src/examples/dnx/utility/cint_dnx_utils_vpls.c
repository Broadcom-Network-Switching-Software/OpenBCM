/*
 * 
 This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 
 Copyright 2007-2020 Broadcom Inc. All rights reserved. File: cint_dnx_utils_vpls.c Purpose: utility for VPLS. 
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
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint ../../../../src/examples/sand/cint_sand_advanced_vlan_translation_mode.c
 * cint
 * int unit = 0; 
 * int rv = 0; 
 * int in_port = 200; 
 * int out_port = 201;
 * rv = vpls_main(unit,ac_port,pwe_port);
 * print rv; 
 * 
 * 
 
 *  Scenario configured in this cint:
 * 
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *   VPLS Termination
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
 *   |    |0C:00:02:00:01   |00:00:00:cd:1d   ||Label:3333||Label:3456  ||11:00:00:01:12 |01:02:03:04:05    || 5    | 0x8100
 *   |    |                         |                       ||Exp:0     ||                  ||                  |                               ||
 *   |    |                         |                       ||TTL:20    ||                  ||                  |                           ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * 
 *  Receiving packet on port == 201:   <------------
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA              |     SA                || vlan | tpid
 *   |    |11:00:00:01:12 |01:02:03:04:05    || 5    | 0x8100
 *   |    |              |              ||          ||               ||      ||
 *   |    |              |              ||          ||               ||      ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~
 *   VPLS encapsulation
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
 *   |    |      DA                 |     SA                || vlan    || tpid
 *   |    |00:0c:00:02:00:00        |00:11:00:00:01:12      || 5       || 0x8100
 *   |    |                         |                       ||         ||
 *   |    |                         |                       ||         ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 *  Sending packet from port == 200 :   ----------->
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-
 *   |    |      DA                 | SA                    ||   MPLS   ||  PWE         ||      DA                  ||     SA                || vlan | tpid
 *   |    |00:00:00:00:cd:1d   |00:0c:00:02:00:01      ||Label:3333||Label:3456    || 00:0c:00:02:00:00        ||00:11:00:00:01:12      || 1111 | 0x8100
 *   |    |                         |                       ||Exp:0     ||              ||                          ||                       ||
 *   |    |                         |                       ||TTL:20    ||              ||                          ||                       ||
 *   |    +-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+--+-+-+-
 *   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * ###############################################################################################
 *
 * Following features are supported by this cint:
 * - Basic PWE.
 * - Special labels.
 *
 * In order to manually modify the setup run:
 * cint;
 * cint_reset();
 * exit;
 * cint ../../../../src/examples/sand/utility/cint_sand_utils_global.c
 * cint ../../../../src/examples/sand/cint_ip_route_basic.c
 * cint ../../../../src/examples/sand/cint_sand_advanced_vlan_translation_mode.c
 * cint ../../../../src/examples/dnx/utility/cint_dnx_utils_vpls.c
 * cint
 * int unit = 0; 
 * int rv = 0; 
 * int port1 = 200; 
 * int port2 = 201;
 * init_default_vpls_params(unit);
 * update configuration as needed
 * rv = vpls_main(unit,port1,port2);
 * print rv; 
 */

struct l2_port_properties_s
{
    bcm_port_t port;
    int intf;
    int arp;
    bcm_mac_t mac_addr;         /* source mac address of the port */
    bcm_mac_t next_hop;         /* next hop mac address to be used by PWE, irrelevant for AC */
    bcm_if_t tunnel_id;         /* jericho and below only, tunnel id for mpls encapsulation entry */
    int encap_fec_id;           /* fec id for encapsulation entry - outlif for MPLS entry in EEDB */
    bcm_if_t encap_id;          /* Encapsulation index */
    bcm_gport_t vlan_port_id;   /* vlan port id of the ac_port, used for ingress */
    int nwk_group_valid;        /* indication to whether nwd group is present */
    int ac_nwk_group_id;        /* ac network group id */
    int stat_id;                /* Port statistics id */
    int stat_pp_profile;        /* PP statistics profile id */
};

struct mpls_tunnel_switch_create_s
{
    bcm_mpls_switch_action_t action;    /* MPLS label action. */
    bcm_mpls_label_t label;             /* Incoming label value. */
    bcm_failover_t failover_id;         /* Failover Object Identifier for protected tunnel. Used for 1+1 protection also */
    bcm_gport_t failover_tunnel_id;     /* Failover Tunnel ID. */
    bcm_gport_t tunnel_id;
    uint32 flags;
    uint32 flags2;
};

struct mpls_tunnel_initiator_create_s
{
    bcm_mpls_egress_action_t action;    /* MPLS label action, relevant when BCM_MPLS_EGRESS_LABEL_ACTION_VALID is set. */
    bcm_mpls_label_t label[2];
    uint32 flags;               /* BCM_MPLS_EGRESS_LABEL_xxx. */
    uint32 flags2;               /* BCM_MPLS_EGRESS_LABEL_FLAGS2_xxx. */
    bcm_if_t tunnel_id;         /* Tunnel Interface. */
    bcm_if_t l3_intf_id;        /* l3 Interface ID. */
    int num_labels;
    bcm_encap_access_t encap_access;    /* Encapsulation Access stage */
    int qos_map_id;             /* QOS map identifier. */
    bcm_qos_egress_model_t egress_qos_model;
    int exp;
};

struct mpls_port_add_s
{
    bcm_gport_t mpls_port_id;   /* GPORT identifier. */
    uint32 flags;               /* BCM_MPLS_PORT_xxx. */
    uint32 flags2;              /* BCM_MPLS_PORT2_xxx. */
    bcm_if_t encap_id;          /* Encap Identifier. */
    bcm_mpls_label_t label;
    uint32 label_flags;         /* BCM_MPLS_EGRESS_LABEL_xxx flags related to the label */
    bcm_if_t label_l3_intf_id;                /* l3 Interface ID. */
    bcm_mpls_port_match_t criteria;     /* Match criteria. for ingress */
    bcm_mpls_label_t match_label;       /* VC label to match. */
    bcm_if_t egress_tunnel_if;  /* MPLS tunnel egress object. */
    bcm_vpn_t forwarding_domain;
    int qos_map_id;             /* QOS map identifier. */
    int exp;
    bcm_qos_ingress_model_t ingress_qos_model;   /*ingress qos model*/
    bcm_encap_access_t encap_access;    /* Encapsulation Access stage */
    int nwk_group_valid;             /* indication to whether nwd group is present */
    int pwe_nwk_group_id;             /* pwe network group id */
    bcm_mpls_port_control_channel_type_t vccv_type; /* Indicate VCCV Control Channel */
    bcm_failover_t  failover_id;        /* Failover ID */
    bcm_multicast_t  failover_mc_group; /* Failover ID Multicast group ID*/
    bcm_gport_t failover_port_id;       /* Failover Port ID */
    int nof_service_tags;               /* Number of expected VLAN tags in inner ETH */
    bcm_qos_egress_model_t egress_qos_model; /*egress qos model*/
};

int vpls_util_verbose = 1;
int params_set = 0;             /* indicates if cint parameters were initilized */
int outer_mpls_tunnel_index = 0;
int MAX_NOF_TUNNELS = 4;
int MIN_LABEL = 0;
int MAX_LABEL = 0x000FFFFF;
bcm_vpn_t vpn;                  /* vsi id */
int pwe_cw = 0;                 /* CW must be 0 */
bcm_mac_t mac1 = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x01 };
bcm_mac_t mac2 = { 0x00, 0x00, 0x00, 0x00, 0xcd, 0x1d };
bcm_mac_t mac3 = { 0x00, 0x0c, 0x00, 0x02, 0x00, 0x00 };
bcm_mac_t mac4 = { 0x00, 0x11, 0x00, 0x00, 0x01, 0x12 };

l2_port_properties_s pwe_port;
l2_port_properties_s ac_port;
mpls_tunnel_switch_create_s mpls_term_tunnel[MAX_NOF_TUNNELS];
mpls_tunnel_initiator_create_s mpls_encap_tunnel[MAX_NOF_TUNNELS];
mpls_port_add_s pwe_encap;
mpls_port_add_s pwe_term;
int             mpls_tunnel_in_eei=0;
int             is_contain_wide_data=0;
int             add_svtag_lookup=0;
int             egress_pointed_tunnel_id;


void vpls_l2_print_mac(bcm_mac_t mac_address){
    unsigned int a,b,c,d,e,f;
    a = 0xff & mac_address[0];
    b = 0xff & mac_address[1];
    c = 0xff & mac_address[2];
    d = 0xff & mac_address[3];
    e = 0xff & mac_address[4];
    f = 0xff & mac_address[5];
    printf("%02x:%02x:%02x:%02x:%02x:%02x",
           a,b,c,
           d,e,f);
}

void
mpls_tunnel_initiator_create_s_init(
    mpls_tunnel_initiator_create_s * tunnel)
{
    int i;

    for (i = 0; i < MAX_NOF_TUNNELS; i++)
    {
        tunnel[i].action = BCM_MPLS_EGRESS_ACTION_SWAP;
        tunnel[i].flags = 0;
        tunnel[i].flags2 = 0;
        tunnel[i].l3_intf_id = 0;
        tunnel[i].num_labels = 0;
        tunnel[i].label[0] = BCM_MPLS_LABEL_INVALID;
        tunnel[i].label[1] = BCM_MPLS_LABEL_INVALID;
        tunnel[i].tunnel_id = 0;
        tunnel[i].encap_access = bcmEncapAccessInvalid;
        tunnel[i].qos_map_id = 0;
    }
}

void
mpls_tunnel_switch_create_s_init(
    mpls_tunnel_switch_create_s * info)
{
    int i;

    for (i = 0; i < MAX_NOF_TUNNELS; i++)
    {
        info[i].label = BCM_MPLS_LABEL_INVALID;
        info[i].action = BCM_MPLS_SWITCH_ACTION_INVALID;
        info[i].failover_id = 0;
        info[i].failover_tunnel_id = 0;
    }
}

void
mpls_port_add_s_init(
    mpls_port_add_s * port_info)
{
    port_info->criteria = BCM_MPLS_PORT_MATCH_INVALID;
    port_info->egress_tunnel_if = 0;
    port_info->encap_id = 0;
    port_info->flags = 0;
    port_info->flags2 = 0;
    port_info->forwarding_domain = 0;
    port_info->label = BCM_MPLS_LABEL_INVALID;
    port_info->label_flags = 0;
    port_info->match_label = BCM_MPLS_LABEL_INVALID;
    port_info->mpls_port_id = 0;
    port_info->qos_map_id = 0;
    port_info->encap_access = bcmEncapAccessInvalid;
    port_info->vccv_type = bcmMplsPortControlChannelNone;
    port_info->failover_id = 0;
    port_info->failover_mc_group = 0;
    port_info->failover_port_id = 0;
    port_info->nof_service_tags = 0;
}

int
init_default_vpls_params(
    int unit)
{
    int rv = BCM_E_NONE;

    vpn = 5;

    /*
     * l2, mac, arp, fec parameters 
     */
    pwe_port.intf = 30;
    pwe_port.mac_addr = mac1;
    pwe_port.next_hop = mac2;
    pwe_port.port = 200;
    pwe_port.arp = 9001;
    pwe_port.stat_id = 0;
    pwe_port.stat_pp_profile = 0;

    ac_port.intf = 5;
    ac_port.mac_addr = mac3;
    ac_port.next_hop = mac4;
    ac_port.port = 201;
    ac_port.vlan_port_id = 0;
    ac_port.stat_id = 0;
    ac_port.stat_pp_profile = 0;

    rv = get_first_fec_in_range_which_not_in_ecmp_range(unit, 0, &ac_port.encap_fec_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, get_first_fec_in_range_which_not_in_ecmp_range\n");
        return rv;
    }

    /*
     * mpls encapsulation tunnel parameters 
     */
    mpls_tunnel_initiator_create_s_init(mpls_encap_tunnel);
    mpls_encap_tunnel[0].label[0] = 3333;
    mpls_encap_tunnel[0].num_labels = 1;
    mpls_encap_tunnel[0].encap_access = bcmEncapAccessTunnel2;
    mpls_encap_tunnel[0].flags |= BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    mpls_encap_tunnel[0].tunnel_id = 8194;

    /*
     * mpls tunnel termination parameters 
     */
    mpls_tunnel_switch_create_s_init(mpls_term_tunnel);
    mpls_term_tunnel[0].action = BCM_MPLS_SWITCH_ACTION_POP;
    mpls_term_tunnel[0].label = mpls_encap_tunnel[0].label[0];

    /*
     * PWE tunnel to be encapsulated parameters 
     */
    mpls_port_add_s_init(&pwe_encap);
    pwe_encap.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL | BCM_MPLS_PORT_ENCAP_WITH_ID;
    pwe_encap.flags2 = BCM_MPLS_PORT2_EGRESS_ONLY;
    pwe_encap.label = 3456;
    pwe_encap.label_flags |= BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    pwe_encap.mpls_port_id = 8888;
    pwe_encap.encap_access = bcmEncapAccessTunnel1;
    pwe_encap.encap_id = 8888;

    /*
     * PWE tunnel to be terminated parameters 
     */
    mpls_port_add_s_init(&pwe_term);
    pwe_term.criteria = BCM_MPLS_PORT_MATCH_LABEL;
    pwe_term.encap_id = pwe_encap.encap_id;
    pwe_term.flags |= BCM_MPLS_PORT_WITH_ID | BCM_MPLS_PORT_EGRESS_TUNNEL;
    pwe_term.flags2 |= BCM_MPLS_PORT2_INGRESS_ONLY;
    pwe_term.match_label = pwe_encap.label;
    pwe_term.mpls_port_id = 9999;

    params_set = 1;

    return rv;
}

int
vpls_configure_port_properties(
    int unit,
    l2_port_properties_s * l2_port,
    uint8 create_rif)
{
    int rv = BCM_E_NONE;
    int group_id;
    char *proc_name = "vpls_configure_port_properties";

    /*
     * set class for both ports 
     */
    /*
     * The port vlan domain value range is 0-511. If the port is trunk gport, in order to fit this range, we use the group id as the trunk vlan domain
     */
    if(!BCM_GPORT_IS_TRUNK(l2_port->port)) {
        rv = bcm_port_class_set(unit, l2_port->port, bcmPortClassId, l2_port->port);
    } else {
        BCM_TRUNK_ID_GROUP_GET(group_id, BCM_GPORT_TRUNK_GET(l2_port->port));
        rv = bcm_port_class_set(unit, l2_port->port, bcmPortClassId, group_id);
    }
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_class_set, port=%d, \n", l2_port->port);
        return rv;
    }

    /*
     * Set Out-Port default properties 
     */
    rv = out_port_set(unit, l2_port->port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, out_port_set intf_out, port=%d\n", l2_port->port);
        return rv;
    }

    if (create_rif)
    {
        /*
         * Set In-Port to In ETh-RIF
         */
        rv = in_port_intf_set(unit, l2_port->port, l2_port->intf);
        if (rv != BCM_E_NONE)
        {
            printf("Error, in_port_intf_set intf_in, port=%d\n", l2_port->port);
            return rv;
        }
    }
    else if (is_device_or_above(unit, JERICHO2))
    {
        /*
         * In JR2, port must be added to the vlan for membership check.
         * In JR1, port can be added to the vlan only when the relevant egress MC is open.
         */
        rv = bcm_vlan_gport_add(unit, l2_port->intf, l2_port->port, 0);
        if (rv != BCM_E_NONE)
        {
            printf("Error, bcm_vlan_gport_add \n");
            return rv;
        }
    }

    return rv;
}

int
vpls_create_l3_interfaces(
    int unit,
    l2_port_properties_s * port)
{
    int rv;
    char *proc_name = "vpls_create_l3_interfaces";

    /*
     * Create ETH-RIF and set its properties 
     */
    rv = intf_eth_rif_create(unit, port->intf, port->mac_addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error, intf_eth_rif_create pwe_intf\n");
        return rv;
    }

    if(vpls_util_verbose >= 1) {
        printf("%s: intf = 0x%x, mac = ", proc_name, port->intf);
        vpls_l2_print_mac(port->mac_addr);
        printf("\n");
    }

    return rv;
}

int
vpls_create_arp_entry(
    int unit,
    l2_port_properties_s * port)
{
    int rv;
    char *proc_name = "vpls_create_arp_entry";

    /*
     * Configure ARP entry 
     */
    rv = l3__egress_only_encap__create(unit, 0, &port->arp, port->next_hop, port->intf);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create AC egress object ARP only\n");
        return rv;
    }

    if(vpls_util_verbose >= 1) {
        printf("%s: arp = 0x%x, intf = 0x%x, mac = ", proc_name, port->arp, port->intf);
        vpls_l2_print_mac(port->mac_addr);
        printf("\n");
    }

    return rv;
}

/*
* Update predefined ARP entry with required MPLS tunnel.
*
* Relevant only for Jericho devices.
*/
int
vpls_update_arp_entry(
    int unit,
    l2_port_properties_s * port)
{
    int rv;
    bcm_l3_egress_t l3eg;
    bcm_if_t l3egid_null;       /* not intersting */
    uint32 flags = BCM_L3_WITH_ID | BCM_L3_REPLACE | BCM_L3_EGRESS_ONLY;
    char *proc_name = "vpls_update_arp_entry";

    bcm_l3_egress_t_init(&l3eg);

    l3eg.mac_addr = port->next_hop;
    l3eg.encap_id = port->arp;
    l3eg.vlan = port->pwe_intf;
    l3eg.intf = port->tunnel_id;

    rv = bcm_l3_egress_create(unit, flags, &l3eg, &l3egid_null);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object ARP only\n");
        return rv;
    }

    if(vpls_util_verbose >= 1) {
        printf("%s: encap_id = 0x%x, vlan = 0x%x, intf = 0x%x, mac = ", proc_name, l3eg.encap_id, l3eg.vlan, l3eg.intf);
        vpls_l2_print_mac(l3eg.mac_addr);
        printf("\n");
    }

    return rv;
}

int
vpls_create_fec_entry(
    int unit,
    int fec_id,
    bcm_if_t mpls_tunnel_id,
    int port,
    int flag)
{
    int rv;
    int temp_out_rif;
    int flags = flag;
    char *proc_name = "vpls_create_fec_entry";

    /*
     * Create FEC: 
     * 1) Give a fec id. 
     * 2) Tunnel id of the encapsulation entry. 
     * 3) Arp id will be given with the only_encap call, so give 0. 
     * 4) Give the out port. 
     */
    rv = l3__egress_only_fec__create(unit, fec_id, mpls_tunnel_id, 0, port, flags);
    if (rv != BCM_E_NONE)
    {
        printf("Error, create egress object FEC only\n");
        return rv;
    }

    if(vpls_util_verbose >= 1) {
        printf("%s: fec_id = 0x%x, mpls_tunnel_id = 0x%x, port = %d, flags = 0x%x\n", proc_name, fec_id, mpls_tunnel_id, port, flags);
    }

    /*
     * The termination scenario doesn't need FEC. 
     * We bridge out of the core with no usage of FEC.
     */

    return rv;
}
/**
 * Procedure for MPLS tunnel configuration.
 * INPUT:
 *   mpls_tunnel   - mpls tunnel data
 * Note
 *  By default, code is assumed to occupy GLEM entries only when
 *  necessary (and not just when it does no harm). 
 */
int
vpls_create_mpls_tunnel(
    int unit,
    mpls_tunnel_initiator_create_s * mpls_tunnel)
{
    bcm_mpls_egress_label_t label_array[2];
    int rv, i, ii;
    char *proc_name;
    char *occupy_glem;

    proc_name = "vpls_create_mpls_tunnel";

    bcm_mpls_egress_label_t_init(&label_array[0]);
    /*
     * Label to be pushed as part of the MPLS tunnel.
     */
    for (i = MAX_NOF_TUNNELS - 1; i >= 0; i--)
    {

        if ((mpls_tunnel[i].label[0] >= MIN_LABEL) && (mpls_tunnel[i].label[0] <= MAX_LABEL))
        {
            label_array[0].label = mpls_tunnel[i].label[0];
            label_array[0].flags = mpls_tunnel[i].flags;
            label_array[0].flags2 = mpls_tunnel[i].flags2;
            BCM_L3_ITF_SET(label_array[0].tunnel_id, BCM_L3_ITF_TYPE_LIF, mpls_tunnel[i].tunnel_id);
            label_array[0].l3_intf_id = mpls_tunnel[i].l3_intf_id;
            label_array[0].action = mpls_tunnel[i].action;
            label_array[0].encap_access = mpls_tunnel[i].encap_access;
            label_array[0].qos_map_id = mpls_tunnel[i].qos_map_id;
            label_array[0].exp = mpls_tunnel[i].exp;
            label_array[0].egress_qos_model.egress_qos = mpls_tunnel->egress_qos_model.egress_qos;
            label_array[0].egress_qos_model.egress_ttl = mpls_tunnel->egress_qos_model.egress_ttl;

            label_array[1].label = mpls_tunnel[i].label[1];
            label_array[1].flags = mpls_tunnel[i].flags;
            BCM_L3_ITF_SET(label_array[1].tunnel_id, BCM_L3_ITF_TYPE_LIF, mpls_tunnel[i].tunnel_id);
            label_array[1].l3_intf_id = mpls_tunnel[i].l3_intf_id;
            label_array[1].action = mpls_tunnel[i].action;
            label_array[1].encap_access = mpls_tunnel[i].encap_access;
            label_array[1].qos_map_id = mpls_tunnel[i].qos_map_id;
            label_array[1].egress_qos_model.egress_qos = mpls_tunnel->egress_qos_model.egress_qos;
            label_array[1].egress_qos_model.egress_ttl = mpls_tunnel->egress_qos_model.egress_ttl;

            /*
             * By default, GLEM occupation should be minimized and if this cycle is not
             * the 'root' of the linked list (for which GLEM entry is required) then do
             * not load GLEM. This is achieved by setting the BCM_MPLS_EGRESS_LABEL_VIRTUAL_EGRESS_POINTED
             * flag.
             */
            occupy_glem = "YES";
            if (i != 0)
            {
                label_array[0].flags |= BCM_MPLS_EGRESS_LABEL_VIRTUAL_EGRESS_POINTED;
                occupy_glem = "NO";
            }
            else
            {
                label_array[0].flags &= ~BCM_MPLS_EGRESS_LABEL_VIRTUAL_EGRESS_POINTED;
                occupy_glem = "YES";
            }

            rv = bcm_mpls_tunnel_initiator_create(unit, 0, mpls_tunnel[i].num_labels, label_array);
            if (rv != BCM_E_NONE)
            {
                printf("%s(): Error, in bcm_mpls_tunnel_initiator_create\n", proc_name);
                return rv;
            }
            mpls_tunnel[i].tunnel_id = label_array[0].tunnel_id;
            if (i != 0)
            {
                mpls_tunnel[i - 1].l3_intf_id = mpls_tunnel[i].tunnel_id;
            }
            
            if(vpls_util_verbose == 1)
            {
                printf("%s(): i = %d, label_1 = 0x%x, label_2 = 0x%x, occupy_glem = %s, l3_intf_id = 0x%08X, tunnel_id 0x%08X, flags = 0x%08X\n",
                       proc_name, i, label_array[0].label, label_array[1].label, occupy_glem, label_array[0].l3_intf_id, label_array[0].tunnel_id, label_array[0].flags);
            }
        }
    }

    return rv;
}

int
vpls_create_termination_stack(
    int unit,
    mpls_tunnel_switch_create_s * tunnel_term)
{
    bcm_mpls_tunnel_switch_t entry;
    int rv, i;
    char *proc_name = "vpls_create_termination_stack";

    /*
     * Create a stack of MPLS labels to be terminated 
     */
    for (i = 0; i < MAX_NOF_TUNNELS; i++)
    {
        if ((tunnel_term[i].label >= MIN_LABEL) && (tunnel_term[i].label <= MAX_LABEL))
        {
            bcm_mpls_tunnel_switch_t_init(&entry);

            entry.action = tunnel_term[i].action;
            entry.label = tunnel_term[i].label;
            entry.flags = tunnel_term[i].flags;
            entry.flags2 = tunnel_term[i].flags2;
            if (soc_property_get(unit, "mpls_termination_label_index_enable", 0))
            {
                BCM_MPLS_INDEXED_LABEL_SET(entry.label, entry.label, 1);
            }

            /*
             * Set protection parameters if we have MPLS Ingress 1+1 Protection
             */
            entry.failover_id = tunnel_term[i].failover_id;
            entry.failover_tunnel_id = tunnel_term[i].failover_tunnel_id;

            rv = bcm_mpls_tunnel_switch_create(unit, &entry);
            if (rv != BCM_E_NONE)
            {
                printf("Error, in bcm_mpls_tunnel_switch_create\n");
                return rv;
            }
            
            if(vpls_util_verbose >= 1) {
                printf("%s: label_1 = 0x%x, label_2 = 0x%x, ingress_if = 0x%x, egress_if = 0x%x, tunnel_id = 0x%x\n", proc_name, entry.label, entry.second_label, entry.ingress_if, entry.egress_if, entry.tunnel_id);
            }
        }
    }
    mpls_term_tunnel[outer_mpls_tunnel_index].tunnel_id = entry.tunnel_id;
    return rv;
}

int
vpls_l2_addr_add(
    int unit,
    bcm_vpn_t vpn,
    bcm_mac_t mac_addr,
    bcm_gport_t gport,
    int stat_id,
    int stat_pp_profile)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t l2addr;
    char *proc_name = "vpls_l2_addr_add";

    bcm_l2_addr_t_init(&l2addr, mac_addr, vpn);
    l2addr.flags |= BCM_L2_STATIC;
    l2addr.port = gport;
    l2addr.stat_id = stat_id;
    l2addr.stat_pp_profile = stat_pp_profile;

    rv = bcm_l2_addr_add(unit, l2addr);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_l2_addr_add\n", rv);
        return rv;
    }

    if(vpls_util_verbose >= 1) {
        printf("%s: mac = ", proc_name);
        vpls_l2_print_mac(mac_addr);
        printf(" vpn = %d, port = 0x%x, stat_id = %d, stat_profile = %d\n", vpn, l2addr.port, l2addr.stat_id, l2addr.stat_pp_profile);
    }

    return rv;
}

int
vpls_mpls_port_add_encapsulation(
    int unit,
    mpls_port_add_s * mpls_port_encap)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;
    char *proc_name = "vpls_mpls_port_add_encapsulation";

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags = mpls_port_encap->flags;
    mpls_port.flags2 = mpls_port_encap->flags2;
    mpls_port.encap_id = mpls_port_encap->encap_id;
    mpls_port.egress_tunnel_if = mpls_port_encap->egress_tunnel_if;
    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, mpls_port_encap->mpls_port_id);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
    mpls_port.egress_label.label = mpls_port_encap->label;
    mpls_port.egress_label.flags = mpls_port_encap->label_flags;
    mpls_port.egress_label.encap_access = mpls_port_encap->encap_access;
    mpls_port.egress_label.qos_map_id = mpls_port_encap->qos_map_id;
    mpls_port.egress_label.exp = mpls_port_encap->exp;
    mpls_port.egress_label.egress_qos_model.egress_qos = mpls_port_encap->egress_qos_model.egress_qos;
    mpls_port.egress_label.egress_qos_model.egress_ttl = mpls_port_encap->egress_qos_model.egress_ttl;

    if (mpls_port_encap->nwk_group_valid)
    {
        mpls_port.network_group_id = mpls_port_encap->pwe_nwk_group_id;
        mpls_port.flags |= is_device_or_above(unit, JERICHO2) ? 0 : BCM_MPLS_PORT_NETWORK;
    }

    rv = bcm_mpls_port_add(unit, 0, mpls_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_mpls_port_add encapsulation\n", rv);
        return rv;
    }

    if(vpls_util_verbose >= 1) {
        printf("%s: mpls_port_id = 0x%x, flags = 0x%x, flags2 = 0x%x, label = %d, label_flags = 0x%x, qos_map_id = %d\n", 
            proc_name, mpls_port.mpls_port_id, mpls_port.flags, mpls_port.flags2, mpls_port.egress_label.label, mpls_port.egress_label.flags, mpls_port.egress_label.qos_map_id);
    }

    mpls_port_encap->mpls_port_id = mpls_port.mpls_port_id;
    mpls_port_encap->encap_id = mpls_port.encap_id;

    return rv;
}

int
vpls_mpls_port_add_termination(
    int unit,
    mpls_port_add_s * port_term)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t mpls_port;
    bcm_gport_t gport_id;
    char *proc_name = "vpls_mpls_port_add_termination";

    bcm_mpls_port_t_init(&mpls_port);
    mpls_port.flags = port_term->flags;
    mpls_port.flags2 = port_term->flags2;
    mpls_port.ingress_qos_model.ingress_phb = port_term->ingress_qos_model.ingress_phb;
    mpls_port.ingress_qos_model.ingress_remark = port_term->ingress_qos_model.ingress_remark;
    mpls_port.ingress_qos_model.ingress_ttl = port_term->ingress_qos_model.ingress_ttl;
    BCM_GPORT_SUB_TYPE_LIF_SET(gport_id, 0, port_term->mpls_port_id);
    BCM_GPORT_MPLS_PORT_ID_SET(mpls_port.mpls_port_id, gport_id);
    mpls_port.criteria = port_term->criteria;
    mpls_port.match_label = port_term->match_label;
    mpls_port.nof_service_tags = port_term->nof_service_tags;

    if (port_term->nwk_group_valid)
    {
        mpls_port.network_group_id = port_term->pwe_nwk_group_id;
        mpls_port.flags |= is_device_or_above(unit, JERICHO2) ? 0 : BCM_MPLS_PORT_NETWORK;
    }

    /** Set protection parameters if we have VPLS Ingress 1+1 Protection */
    mpls_port.ingress_failover_id = port_term->failover_id;
    mpls_port.ingress_failover_port_id = port_term->failover_port_id;
    /** connect PWE entry to created Failover MC group for learning */
    mpls_port.failover_mc_group = port_term->failover_mc_group;

    /* 
     * In the case of Ingres protection the egress_tunnel_if and encap_id must be 0.
     * The learning information is taken from the failover_mc_group
     */
    if (port_term->failover_id == 0)
    {
        /** connect PWE entry to created MPLS encapsulation entry for learning */
        BCM_L3_ITF_SET(mpls_port.egress_tunnel_if, BCM_L3_ITF_TYPE_FEC, port_term->egress_tunnel_if);
        /*
         * encap_id is the egress outlif - used for learning
         */
        mpls_port.encap_id = port_term->encap_id;
    }

    mpls_port.vccv_type = port_term->vccv_type;

    rv = bcm_mpls_port_add(unit, port_term->forwarding_domain, mpls_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in bcm_mpls_port_add termination\n", rv);
        return rv;
    }

    if(vpls_util_verbose >= 1) {
        printf("%s: mpls_port_id = 0x%x, flags = 0x%x, flags2 = 0x%x, match_label = %d, egress_tunnel_if = 0x%x\n", 
            proc_name, mpls_port.mpls_port_id, mpls_port.flags, mpls_port.flags2, mpls_port.match_label, mpls_port.egress_tunnel_if);
    }

    port_term->mpls_port_id = mpls_port.mpls_port_id;

    return rv;
}

int
vpls_vswitch_add_access_port(
    int unit,
    l2_port_properties_s * port,
    bcm_vpn_t vpn)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t vlan_port;
    char *proc_name = "vpls_vswitch_add_access_port";

    /*
     * add port, according to port_vlan_vlan 
     */
    bcm_vlan_port_t_init(&vlan_port);

    /*
     * set port attribures, key <port-vlan-vlan>
     */
    vlan_port.criteria = BCM_VLAN_PORT_MATCH_PORT_VLAN;
    vlan_port.port = port->port;
    vlan_port.match_vlan = vpn;
    vlan_port.vsi = 0;
    vlan_port.flags = 0;
    
    if (is_contain_wide_data) 
    {
        vlan_port.flags |= BCM_VLAN_PORT_INGRESS_WIDE;
    }

    if (port->nwk_group_valid)
    {
        vlan_port.ingress_network_group_id = port->ac_nwk_group_id;
        vlan_port.egress_network_group_id = port->ac_nwk_group_id;
    }

    rv = bcm_vlan_port_create(unit, &vlan_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vlan_port_create\n");
        return rv;
    }
    port->vlan_port_id = vlan_port.vlan_port_id;
    port->encap_id = vlan_port.encap_id;

    rv = bcm_vswitch_port_add(unit, vpn, vlan_port.vlan_port_id);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_vswitch_port_add\n");
        return rv;
    }

    if(vpls_util_verbose >= 1) {
        printf("%s: vlan_port_id = 0x%x, port = %d, vpn = %d\n", proc_name, vlan_port.vlan_port_id, port->port, vpn);
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
    if (!is_device_or_above(unit, JERICHO2))
    {
        printf("Only Jericho2 is supported\n");
        return BCM_E_CONFIG;
    }

    return rv;
}

/*
 * Main function for basic mpls vpls scenario.
 */
int
vpls_main(
    int unit,
    int port1,
    int port2)
{
    int rv = BCM_E_NONE;
    int ii;
    char *proc_name;

    proc_name = "vpls_main";
    if(vpls_util_verbose == 1)
    {
        printf("%s(): ENTER. port1 %d,port2 %d\r\n", proc_name, port1, port2);
    }

    rv = vpls_mp_check_support(unit);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_mp_check_support\n", rv);
        return rv;
    }

    if (!params_set)
    {
        init_default_vpls_params(unit);
    }
    ac_port.port = port1;
    pwe_port.port = port2;

    /** Configure control word to be used in case needed for special label */
    rv = bcm_switch_control_set(unit, bcmSwitchMplsPWControlWord, pwe_cw);
    if (rv != BCM_E_NONE)
    {
        printf("Error, bcm_switch_control_set\n");
        return rv;
    }

    /*
     * Configure AC and PWE ports
     */
    rv = vpls_configure_port_properties(unit, ac_port, 0 /* create_rif */ );
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_configure_port_properties ac_port\n", rv);
        return rv;
    }
    rv = vpls_configure_port_properties(unit, pwe_port, 1 /* create_rif */ );
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_configure_port_properties pwe_port\n", rv);
        return rv;
    }

    /*
     * create vlan based on the vsi (vpn) 
     */
    rv = bcm_vlan_create(unit, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("Failed (%d) to create VLAN %d\n", rv, vpn);
        return rv;
    }
    pwe_term.forwarding_domain = vpn;

    /*
     * Configure L3 interfaces 
     */
    rv = vpls_create_l3_interfaces(unit, &pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_create_l3_interfaces\n", rv);
        return rv;
    }

    /*
     * Configure an ARP entries
     */
    rv = vpls_create_arp_entry(unit, &pwe_port);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_create_arp_entry\n", rv);
        return rv;
    }
    printf("%s(): Going to load mpls_encap_tunnel[%d].l3_intf_id by 0x%08X\r\n",proc_name, outer_mpls_tunnel_index, pwe_port.arp);
    mpls_encap_tunnel[outer_mpls_tunnel_index].l3_intf_id = pwe_port.arp;

    /*
     * Configure a push entry.
     */
    rv = vpls_create_mpls_tunnel(unit, mpls_encap_tunnel);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_create_mpls_tunnel\n", rv);
        return rv;
    }
    pwe_port.tunnel_id = mpls_encap_tunnel[0].tunnel_id;
    egress_pointed_tunnel_id = mpls_encap_tunnel[1].tunnel_id;

    /*
     * Configure fec entry 
     */
    rv = vpls_create_fec_entry(unit, ac_port.encap_fec_id, mpls_encap_tunnel[0].tunnel_id, pwe_port.port,mpls_tunnel_in_eei?BCM_L3_ENCAP_SPACE_OPTIMIZED:0);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_create_fec_entry\n", rv);
        return rv;
    }
    pwe_term.egress_tunnel_if = ac_port.encap_fec_id;

    /*
     * Configure a termination label for the ingress flow  
     */
    rv = vpls_create_termination_stack(unit, mpls_term_tunnel);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_create_termination_stack\n", rv);
        return rv;
    }

    /*
     * configure PWE tunnel - egress flow 
     */
    rv = vpls_mpls_port_add_encapsulation(unit, &pwe_encap);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_mpls_port_add_encapsulation\n", rv);
        return rv;
    }
    pwe_term.encap_id = pwe_encap.encap_id;

    /*
     * configure PWE tunnel - ingress flow 
     */
    rv = vpls_mpls_port_add_termination(unit, &pwe_term);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_mpls_port_add_termination\n", rv);
        return rv;
    }

    /** define ac and pwe ports */
    rv = vpls_vswitch_add_access_port(unit, &ac_port, vpn);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_vswitch_add_access_port\n", rv);
        return rv;
    }

    /*
     * add l2 addresses to be defined as static - no learning needed
     */
    rv = vpls_l2_addr_add(unit, vpn, ac_port.mac_addr, pwe_term.mpls_port_id, ac_port.stat_id, ac_port.stat_pp_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_l2_addr_add\n", rv);
        return rv;
    }
    rv = vpls_l2_addr_add(unit, vpn, ac_port.next_hop, ac_port.vlan_port_id, pwe_port.stat_id, pwe_port.stat_pp_profile);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vpls_l2_addr_add\n", rv);
        return rv;
    }

    if(vpls_util_verbose == 1)
    {
        printf("%s(): EXIT. port1 %d,port2 %d\r\n", proc_name, port1, port2);
    }
    return rv;
}

/**
 * \brief
 *
 * This function performs the following steps:
 * 1. Sets port tpid class:
 * 2. Set a vlan translation command (replace outer) with the
 * following parameters: edit_profile=5, tag_format=5,
 * new_vlan=1111
 */
int
vpls_mp_basic_ve_swap(
    int unit,
    int port,
    int is_ive)
{
    int rv = BCM_E_NONE;

    bcm_port_tpid_class_t port_tpid_class;
    /*
     * set tag format 4 for tagged 0x8100 packets
     */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = 0x8100;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = 4;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    if (is_ive == 0)
    {
        port_tpid_class.flags |= BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    }
    else if (is_ive == 1)
    {
        port_tpid_class.flags |= BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    }
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port);
        return rv;
    }

    /*
     * configure ive swap
     */
    rv = vlan_translate_ive_eve_translation_set(unit, cint_vpls_basic_info.vlan_port_id, 0x8100, 0, bcmVlanActionReplace, bcmVlanActionNone, 1111, 0, 5        /* edit_profile 
                                  */ , 4 /* tag_format */ , is_ive);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vlan_translate_ive_eve_translation_set\n", rv);
        return rv;
    }
    return rv;
}

/**
 * \brief
 *
 * This function performs the following steps:
 * 1. Sets port tpid class
 * 2. Set a vlan translation command (replace outer, and add
 * priority parameters) with the following parameters:
 * edit_profile=5, tag_format=9, new_vlan=1111
 */
int
vpls_mp_basic_ve_pri_action_swap(
    int unit,
    int port,
    int is_ive)
{
    int rv = BCM_E_NONE;

    bcm_port_tpid_class_t port_tpid_class;
    /*
     * set tag format 0 for untagged packets
     */
    bcm_port_tpid_class_t_init(&port_tpid_class);
    port_tpid_class.port = port;
    port_tpid_class.tpid1 = 0x8100;
    port_tpid_class.tpid2 = BCM_PORT_TPID_CLASS_TPID_INVALID;
    port_tpid_class.tag_format_class_id = 4;
    port_tpid_class.flags = BCM_PORT_TPID_CLASS_OUTER_NOT_PRIO;
    if (is_ive == 0)
    {
        port_tpid_class.flags |= BCM_PORT_TPID_CLASS_EGRESS_ONLY;
    }
    else if (is_ive == 1)
    {
        port_tpid_class.flags |= BCM_PORT_TPID_CLASS_INGRESS_ONLY;
    }
    port_tpid_class.vlan_translation_action_id = 0;
    rv = bcm_port_tpid_class_set(unit, &port_tpid_class);
    if (rv != BCM_E_NONE)
    {
        printf("Error, in bcm_port_tpid_class_set, port=%d, \n", port);
        return rv;
    }

    /*
     * configure ive swap
     */
    rv = vlan_translate_ive_eve_translation_set_with_pri_action(unit, cint_vpls_basic_info.vlan_port_id, 0x8100, 0x09100, bcmVlanTpidActionNone, bcmVlanTpidActionNone, bcmVlanActionReplace, bcmVlanActionNone, bcmVlanActionAdd, bcmVlanActionNone, 1111, 0, 5        /* edit_profile 
                                                                 */ , 4 /* tag_format */ , is_ive);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), in vlan_translate_ive_eve_translation_set_with_pri_action\n", rv);
        return rv;
    }
    return rv;
}

/**
 * \brief
 *  It's exmaple use bcm_mpls_port_traverse to implement delete all function.
 */
int
vpls_mpls_port_delete_all(int unit, int vpn_id)
{
    bcm_mpls_port_traverse_info_t additional_info;
    int rv = BCM_E_NONE;

    sal_memset(&additional_info, 0, sizeof(additional_info));
    additional_info.vpn = vpn_id;
    additional_info.traverse_flags = BCM_MPLS_TRAVERSE_DELETE;
    rv = bcm_mpls_port_traverse(unit, additional_info, NULL, NULL);
    if (rv != BCM_E_NONE)
    {
        printf("Error(%d), try to delete mpls entries by calling bcm_mpls_port_traverse\n", rv);
        return rv;
    }
    return rv;
}

/**
 * \brief
 *  Return next layer network domain which encoding as arr prefix + value(3 bits).
 */
int
vpls_mpls_next_layer_network_domain_value(int unit, int next_layer_network_domain)
    {
    int arr_prefix_size = 24;
    int arr_prefix_value = *(dnxc_data_get(unit, "arr", "prefix", "IRPP_IN_LIF_FORMATS___prefix_11", NULL));
    int nof_bits = *(dnxc_data_get(unit, "l2", "vlan_domain", "nof_bits_next_layer_network_domain", NULL)); /* including prefix+value */

    return ((arr_prefix_value >> (arr_prefix_size - nof_bits)) | next_layer_network_domain);
}


int
vpls_egress_pointed_main(
    int unit,
    int port1,
    int port2)
{
    int rv = BCM_E_NONE;
    char *proc_name;

    proc_name = "vpls_egress_pointed_main";
    init_default_vpls_params(unit);

    BCM_L3_ITF_SET(mpls_encap_tunnel[0].l3_intf_id, BCM_L3_ITF_TYPE_LIF, 12290);
    mpls_encap_tunnel[1].label[0] = 3334;
    mpls_encap_tunnel[1].num_labels = 1;
    mpls_encap_tunnel[1].encap_access = bcmEncapAccessTunnel3;
    mpls_encap_tunnel[1].flags  |= BCM_MPLS_EGRESS_LABEL_WITH_ID | BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    mpls_encap_tunnel[1].flags2 |= add_svtag_lookup ? BCM_MPLS_EGRESS_LABEL_FLAGS2_SVTAG_ENABLE : 0;
    mpls_encap_tunnel[1].tunnel_id = 12290;
    outer_mpls_tunnel_index = 1;

    rv = vpls_main(unit, port1, port2);
    if (rv != BCM_E_NONE)
    {
        printf("%s(): Error(%d), in vpls_mp_check_support\n", proc_name, rv);
        return rv;
    }

    return rv;
}
