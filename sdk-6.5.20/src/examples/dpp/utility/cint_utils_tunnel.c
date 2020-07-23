/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * Purpose: This file provides IPv4 tunnel basic functionality and defines global variables
 */

/* MPLS tunnel Parameters */
struct mpls_tunnel_info_s{
    bcm_mpls_label_t mpls_encap_label;    /* MPLS tunnel encap. label */
    uint8 mpls_encap_ttl;                 /* MPLS tunnel encap. ttl */
    uint32 flags;                         /* MPLS tunnel encap flags */
    bcm_mpls_egress_action_t action;      /* MPLS tunnel encap. action */
};

/* Global Tunnelling parameters
 * This struct includes all tunnels and forwarding definitions */
struct tunnel_utils__s{

    int in_vlan;        /* in vlan */
    bcm_mac_t my_mac;   /* mymac */
    int vrf;            /* vrf */

    /* tunnels START */
    mpls_tunnel_info_s mpls_tunnel;        /* MPLS tunnel parameters*/
    /* tunnels END */

    /* forwarding START*/
    int ipv4_dip1;                        /* ipv4 forwarding dip 1 */
    int ipv4_dip2;                        /* ipv4 forwarding dip 2 */
    int ipv4_mask1;                        /* ipv4 forwarding mask 1 */
    int ipv4_mask2;                        /* ipv4 forwarding mask 2 */

    bcm_ip6_t ipv6_dip1;                  /* ipv6 ipv4 forwarding dip 1 */
    bcm_ip6_t ipv6_dip2;                  /* ipv6 ipv4 forwarding dip 2 */
    bcm_ip6_t ipv6_mask1;                  /* ipv6 forwarding mask 1 */
    bcm_ip6_t ipv6_mask2;                  /* ipv6 forwarding mask 2 */

    bcm_mpls_label_t in_mpls_lbl_1;       /* MPLS forwarding label 1 */
    bcm_mpls_label_t in_mpls_lbl_2;       /* MPLS forwarding label 2 */
    /* forwarding END */
};

tunnel_utils__s g__tunnel_utils =
{
    2,      /* in vlan */
    {0x00, 0x0c, 0x00, 0x02, 0x00, 0x00},  /* my-MAC */
    0,      /* vrf */

    {    /* MPLS tunnel parameters */
         200,    /* mpls tunnel encap. label */
         67,     /* mpls tunnel encap. ttl */
         BCM_MPLS_EGRESS_LABEL_TTL_SET,   /* mpls tunnel encap. flags */
         BCM_MPLS_EGRESS_ACTION_PUSH      /* mpls tunnel encap. action */
    },

    0x7fffff00,   /* ipv4 forwarding dip 1 - 127.255.255.0 */
    0x6fffff00,   /* ipv4 forwarding dip 2 - 111.255.255.0 */
    0xfffffff0,   /* ipv4 forwarding mask 1 */
    0xfffffff0,   /* ipv4 forwarding mask 2 */

    {    /* ipv6 forwarding dip 1 */
        0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8,
        0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xec
    },
    {    /* ipv6 forwarding dip 2 */
        0xea, 0xeb, 0xec, 0xed, 0xe5, 0xe6, 0xe7, 0xe8,
        0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, 0xec
    },
    {    /* ipv6 forwarding mask 1 */
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
    },
    {    /* ipv6 forwarding mask 2 */
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
    },

    500,    /* MPLS forwarding label 1 */
    700     /* MPLS forwarding label 2 */
};

/* Get test data struct */
void tunnel__get_struct(tunnel_utils__s *test_data)
{
    sal_memcpy(test_data, &g__tunnel_utils, sizeof(*test_data));
    return;
}

/* Initialize test data struct */
int tunnel__init(int unit, tunnel_utils__s *test_data)
{
    if (test_data != NULL) {
       sal_memcpy(&g__tunnel_utils, test_data, sizeof(g__tunnel_utils));
    }

    return BCM_E_NONE;
}

/* create MPLS tunnel (mpls_tunnel_info_s mpls_tunnel)
 * Parameters: out_intf_ids - MPLS tunnel will point to this intf
 *             mpls_intf_id - interface id of the created MPLS tunnel */
int tunnel__mpls_encap_create(int unit, bcm_if_t out_intf_id, bcm_if_t* mpls_intf_id)
{
    int rv;

    bcm_mpls_egress_label_t label;
    label.label = g__tunnel_utils.mpls_tunnel.mpls_encap_label;   /* default: 200 */
    label.ttl = g__tunnel_utils.mpls_tunnel.mpls_encap_ttl;       /* default: 67 */
    label.flags = g__tunnel_utils.mpls_tunnel.flags;              /* default: BCM_MPLS_EGRESS_LABEL_TTL_SET */
    label.action = g__tunnel_utils.mpls_tunnel.action;            /* default: BCM_MPLS_EGRESS_ACTION_PUSH */
    if (out_intf_id != -1) {
        label.l3_intf_id = out_intf_id;
    }

    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, &label);
    if (rv != BCM_E_NONE) {
        printf ("Error, bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    *mpls_intf_id = label.tunnel_id;

    return rv;
}

/* add IPv4 route
 * Parameters: intf_id - routing FEC
 *             dip_index - ipv4_dip index in the global struct, starting from 1*/
int tunnel__add_ipv4_route(int unit, bcm_if_t intf_id, int dip_index)
{
    int rv;
    bcm_ip_t forwarding_dip;
    int forwarding_mask;

    switch (dip_index)
    {
    case 1:
       forwarding_dip = g__tunnel_utils.ipv4_dip1; /* default: 127.255.255.0 */
       forwarding_mask = g__tunnel_utils.ipv4_mask1;    /*default: 0xfffffff0 */
       break;
    case 2:
       forwarding_dip = g__tunnel_utils.ipv4_dip2; /*default: 111.255.255.0 */
       forwarding_mask = g__tunnel_utils.ipv4_mask2;    /*default: 0xfffffff0 */
       break;
    }

    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);
    l3rt.l3a_flags = BCM_L3_RPF;
    l3rt.l3a_subnet = forwarding_dip;
    l3rt.l3a_ip_mask = forwarding_mask;
    l3rt.l3a_vrf = g__tunnel_utils.vrf;
    l3rt.l3a_intf = intf_id;
    l3rt.l3a_modid = 0;
    l3rt.l3a_port_tgid = 0;

    rv = bcm_l3_route_add(unit, l3rt);
    if (rv != BCM_E_NONE) {
        printf ("Error, bcm_l3_route_add\n");
        return rv;
    }

    return rv;
}

/* add IPv6 route
 * Parameters: intf_id - routing FEC
 *             dip_index - ipv6_dip index in the global struct, starting from 1*/
int tunnel__add_ipv6_route(int unit, int intf_id, int dip_index)
{
    int rv;
    bcm_ip6_t forwarding_dip;
    bcm_ip6_t forwarding_mask;

    switch (dip_index)
    {
    case 1:
       forwarding_dip = g__tunnel_utils.ipv6_dip1; /* default: e1, e2... */
       forwarding_mask = g__tunnel_utils.ipv6_mask1;    /*default: ff,..,ff */
       break;
    case 2:
       forwarding_dip = g__tunnel_utils.ipv6_dip2; /* default: ea, eb... */
       forwarding_mask = g__tunnel_utils.ipv6_mask2;    /*default: ff,..,ff */
       break;
    }

    bcm_l3_route_t l3rt;
    bcm_l3_route_t_init(l3rt);
    l3rt.l3a_flags = BCM_L3_IP6;
    l3rt.l3a_ip6_net = forwarding_dip;
    l3rt.l3a_ip6_mask = forwarding_mask;
    l3rt.l3a_vrf = g__tunnel_utils.vrf;
    l3rt.l3a_intf = intf_id;
    l3rt.l3a_modid = 0;
    l3rt.l3a_port_tgid = 0;

    rv = bcm_l3_route_add(unit, l3rt);
    if (rv != BCM_E_NONE) {
        printf ("Error, bcm_l3_route_add\n");
        return rv;
    }

    return rv;
}

/* add MPLS route
 * Parameters: in_port - in port
 *             intf_id - routing FEC
 *             dip_index - mpls forwarding index in the global struct, starting from 1*/
int tunnel__add_mpls_route(int unit, int in_port, int intf_id, int label_index)
{
    int rv = 0;
    bcm_mpls_label_t label_val;

    switch (label_index)
    {
    case 1:
       label_val = g__tunnel_utils.in_mpls_lbl_1;  /* default: 500 */
       break;
    case 2:
       label_val = g__tunnel_utils.in_mpls_lbl_2;  /* default 700 */
       break;
    }

    bcm_mpls_egress_label_t label;
    label.flags = BCM_MPLS_EGRESS_LABEL_TTL_DECREMENT;
    label.action = BCM_MPLS_EGRESS_ACTION_SWAP;
    label.label = label_val;
    rv = bcm_mpls_tunnel_initiator_create(unit, 0, 1, &label);
    if (rv != BCM_E_NONE) {
        printf ("Error, bcm_mpls_tunnel_initiator_create\n");
        return rv;
    }

    bcm_mpls_tunnel_switch_t tun_switch;
    tun_switch.egress_if = intf_id;
    tun_switch.port = in_port;
    tun_switch.label = g__tunnel_utils.in_mpls_lbl_1;
    tun_switch.egress_label = label;
    tun_switch.action = BCM_MPLS_SWITCH_ACTION_SWAP;
    tun_switch.flags = BCM_MPLS_SWITCH_TTL_DECREMENT;
    tun_switch.label = label_val;
    rv = bcm_mpls_tunnel_switch_create(unit, tun_switch);
    if (rv != BCM_E_NONE) {
        printf ("Error, bcm_mpls_tunnel_switch_create\n");
        return rv;
    }

    return rv;
}

