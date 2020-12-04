/*
 * $Id: compat_659.c,v 1.0 2017/04/17
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with <=sdk-6.5.9 routines
 */

#ifdef	BCM_RPC_SUPPORT
#include <shared/alloc.h>
#include <bcm/error.h>
#include <bcm_int/compat/compat_659.h>

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat659in_vlan_port_t
 * Purpose:
 *      Convert the bcm_vlan_port_t datatype from <=6.5.9 to
 *      SDK 6.5.9+
 * Parameters:
 *      from        - (IN) The <=6.5.9 version of the datatype
 *      to          - (OUT) The SDK 6.5.9+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat659in_vlan_port_t(bcm_compat659_vlan_port_t *from,
                           bcm_vlan_port_t *to)
{
    bcm_vlan_port_t_init(to);
    to->flags                    = from->flags;
    to->criteria                 = from->criteria;
    to->vsi                      = from->vsi;
    to->match_vlan               = from->match_vlan;
    to->match_inner_vlan         = from->match_inner_vlan;
    to->match_pcp                = from->match_pcp;
    to->match_tunnel_value       = from->match_tunnel_value;
    to->match_ethertype          = from->match_ethertype;
    to->port                     = from->port;
    to->pkt_pri                  = from->pkt_pri;
    to->pkt_cfi                  = from->pkt_cfi;
    to->egress_service_tpid      = from->egress_service_tpid;
    to->egress_vlan              = from->egress_vlan;
    to->egress_inner_vlan        = from->egress_inner_vlan;
    to->egress_tunnel_value      = from->egress_tunnel_value;
    to->encap_id                 = from->encap_id;
    to->qos_map_id               = from->qos_map_id;
    to->policer_id               = from->policer_id;
    to->egress_policer_id        = from->egress_policer_id;
    to->failover_id              = from->failover_id;
    to->failover_port_id         = from->failover_port_id;
    to->vlan_port_id             = from->vlan_port_id;
    to->failover_mc_group        = from->failover_mc_group;
    to->ingress_failover_id      = from->ingress_failover_id;
    to->egress_failover_id       = from->egress_failover_id;
    to->egress_failover_port_id  = from->egress_failover_port_id;
    to->ingress_network_group_id = from->ingress_network_group_id;
    to->egress_network_group_id  = from->egress_network_group_id;
    to->inlif_counting_profile   = from->inlif_counting_profile;
    to->outlif_counting_profile  = from->outlif_counting_profile;
}

/*
 * Function:
 *      _bcm_compat659out_vlan_port_t
 * Purpose:
 *      Convert the bcm_vlan_port_t datatype from 6.5.9+ to
 *      <=6.5.9
 * Parameters:
 *      from        - (IN) The SDK 6.5.9+ version of the datatype
 *      to          - (OUT) The <=6.5.9 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat659out_vlan_port_t(bcm_vlan_port_t *from,
                            bcm_compat659_vlan_port_t *to)
{
    from->flags                     = to->flags;
    from->criteria                  = to->criteria;
    from->vsi                       = to->vsi;
    from->match_vlan                = to->match_vlan;
    from->match_inner_vlan          = to->match_inner_vlan;
    from->match_pcp                 = to->match_pcp;
    from->match_tunnel_value        = to->match_tunnel_value;
    from->match_ethertype           = to->match_ethertype;
    from->port                      = to->port;
    from->pkt_pri                   = to->pkt_pri;
    from->pkt_cfi                   = to->pkt_cfi;
    from->egress_service_tpid       = to->egress_service_tpid;
    from->egress_vlan               = to->egress_vlan;
    from->egress_inner_vlan         = to->egress_inner_vlan;
    from->egress_tunnel_value       = to->egress_tunnel_value;
    from->encap_id                  = to->encap_id;
    from->qos_map_id                = to->qos_map_id;
    from->policer_id                = to->policer_id;
    from->egress_policer_id         = to->egress_policer_id;
    from->failover_id               = to->failover_id;
    from->failover_port_id          = to->failover_port_id;
    from->vlan_port_id              = to->vlan_port_id;
    from->failover_mc_group         = to->failover_mc_group;
    from->ingress_failover_id       = to->ingress_failover_id;
    from->egress_failover_id        = to->egress_failover_id;
    from->egress_failover_port_id   = to->egress_failover_port_id;
    from->ingress_network_group_id  = to->ingress_network_group_id;
    from->egress_network_group_id   = to->egress_network_group_id;
    from->inlif_counting_profile    = to->inlif_counting_profile;
    from->outlif_counting_profile   = to->outlif_counting_profile;

}

/*
 * Function: bcm_compat659_vlan_port_find
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_port_find.
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     vlan_port - (IN/OUT) vlan port with old format
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat659_vlan_port_find(int unit, bcm_compat659_vlan_port_t *vlan_port)
{
    int rv;
    bcm_vlan_port_t *new_vlan_port = NULL;

    if (NULL != vlan_port) {
        /* Create from heap to avoid the stack to bloat */
        new_vlan_port = (bcm_vlan_port_t *)
            sal_alloc(sizeof(bcm_vlan_port_t), "New vlan port");
        if (NULL == new_vlan_port) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat659in_vlan_port_t(vlan_port, new_vlan_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_find(unit, new_vlan_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat659out_vlan_port_t(new_vlan_port, vlan_port);
    /* Deallocate memory*/
    sal_free(new_vlan_port);

    return rv;
}

/*
 * Function: bcm_compat659_vlan_port_create
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_port_create.
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     vlan_port - (IN/OUT) vlan port with old format
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat659_vlan_port_create(int unit, bcm_compat659_vlan_port_t *vlan_port)
{
    int rv;
    bcm_vlan_port_t *new_vlan_port = NULL;

    if (NULL != vlan_port) {
        /* Create from heap to avoid the stack to bloat */
        new_vlan_port = (bcm_vlan_port_t *)
            sal_alloc(sizeof(bcm_vlan_port_t), "New vlan port");
        if (NULL == new_vlan_port) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat659in_vlan_port_t(vlan_port, new_vlan_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_create(unit, new_vlan_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat659out_vlan_port_t(new_vlan_port, vlan_port);
    /* Deallocate memory*/
    sal_free(new_vlan_port);

    return rv;
}

#endif

/*
 * Function:
 *      _bcm_compat659in_vlan_control_vlan_t
 * Purpose:
 *      Convert the bcm_vlan_control_vlan_t datatype from <=6.5.9 to
 *      SDK 6.5.9+
 * Parameters:
 *      from        - (IN) The <=6.5.9 version of the datatype
 *      to          - (OUT) The SDK 6.5.9+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat659in_vlan_control_vlan_t(
                           bcm_compat659_vlan_control_vlan_t *from,
                           bcm_compat6518_vlan_control_vlan_t *to)
{
    
    to->vrf                                        = from->vrf;
    to->forwarding_vlan                            = from->forwarding_vlan;
    to->ingress_if                                 = from->ingress_if;
    to->outer_tpid                                 = from->outer_tpid;
    to->flags                                      = from->flags;
    to->ip6_mcast_flood_mode                       = from->ip6_mcast_flood_mode;
    to->ip4_mcast_flood_mode                       = from->ip4_mcast_flood_mode;
    to->l2_mcast_flood_mode                        = from->l2_mcast_flood_mode;
    to->if_class                                   = from->if_class;
    to->forwarding_mode                            = from->forwarding_mode;
    to->urpf_mode                                  = from->urpf_mode;
    to->cosq                                       = from->cosq;
    to->qos_map_id                                 = from->qos_map_id;
    to->distribution_class                         = from->distribution_class;
    to->broadcast_group                            = from->broadcast_group;
    to->unknown_multicast_group                    = from->unknown_multicast_group;
    to->unknown_unicast_group                      = from->unknown_unicast_group;
    to->trill_nonunicast_group                     = from->trill_nonunicast_group;
    to->source_trill_name                          = from->source_trill_name;
    to->trunk_index                                = from->trunk_index;
    to->protocol_pkt.mmrp_action                   = from->protocol_pkt.mmrp_action;
    to->protocol_pkt.srp_action                    = from->protocol_pkt.srp_action;
    to->protocol_pkt.arp_reply_action              = from->protocol_pkt.arp_reply_action;
    to->protocol_pkt.arp_request_action            = from->protocol_pkt.arp_request_action;
    to->protocol_pkt.nd_action                     = from->protocol_pkt.nd_action;
    to->protocol_pkt.dhcp_action                   = from->protocol_pkt.dhcp_action;
    to->protocol_pkt.igmp_report_leave_action      = from->protocol_pkt.igmp_report_leave_action;
    to->protocol_pkt.igmp_query_action             = from->protocol_pkt.igmp_query_action;
    to->protocol_pkt.igmp_unknown_msg_action       = from->protocol_pkt.igmp_unknown_msg_action;
    to->protocol_pkt.mld_report_done_action        = from->protocol_pkt.mld_report_done_action;
    to->protocol_pkt.mld_query_action              = from->protocol_pkt.mld_query_action;
    to->protocol_pkt.ip4_rsvd_mc_action            = from->protocol_pkt.ip4_rsvd_mc_action;
    to->protocol_pkt.ip6_rsvd_mc_action            = from->protocol_pkt.ip6_rsvd_mc_action;
    to->protocol_pkt.ip4_mc_router_adv_action      = from->protocol_pkt.ip4_mc_router_adv_action;
    to->protocol_pkt.ip6_mc_router_adv_action      = from->protocol_pkt.ip6_mc_router_adv_action;
    to->protocol_pkt.flood_action_according_to_pfm = from->protocol_pkt.flood_action_according_to_pfm;
    to->nat_realm_id                               = from->nat_realm_id;
    to->l3_if_class                                = from->l3_if_class;
    to->vp_mc_ctrl                                 = from->vp_mc_ctrl;
    to->aging_cycles                               = from->aging_cycles;
    to->entropy_id                                 = from->entropy_id;
    to->vpn                                        = from->vpn;
    to->egress_vlan                                = from->egress_vlan;
    to->learn_flags                                = from->learn_flags;
    to->sr_flags                                   = from->sr_flags;
}

/*
 * Function:
 *      _bcm_compat659out_vlan_control_vlan_t
 * Purpose:
 *      Convert the bcm_vlan_control_vlan_t datatype from 6.5.9+ to
 *      <=6.5.9
 * Parameters:
 *      from        - (IN) The SDK 6.5.9+ version of the datatype
 *      to          - (OUT) The <=6.5.9 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat659out_vlan_control_vlan_t(bcm_compat6518_vlan_control_vlan_t *from,
                                          bcm_compat659_vlan_control_vlan_t *to)
{
    to->vrf                                        = from->vrf;
    to->forwarding_vlan                            = from->forwarding_vlan;
    to->ingress_if                                 = from->ingress_if;
    to->outer_tpid                                 = from->outer_tpid;
    to->flags                                      = from->flags;
    to->ip6_mcast_flood_mode                       = from->ip6_mcast_flood_mode;
    to->ip4_mcast_flood_mode                       = from->ip4_mcast_flood_mode;
    to->l2_mcast_flood_mode                        = from->l2_mcast_flood_mode;
    to->if_class                                   = from->if_class;
    to->forwarding_mode                            = from->forwarding_mode;
    to->urpf_mode                                  = from->urpf_mode;
    to->cosq                                       = from->cosq;
    to->qos_map_id                                 = from->qos_map_id;
    to->distribution_class                         = from->distribution_class;
    to->broadcast_group                            = from->broadcast_group;
    to->unknown_multicast_group                    = from->unknown_multicast_group;
    to->unknown_unicast_group                      = from->unknown_unicast_group;
    to->trill_nonunicast_group                     = from->trill_nonunicast_group;
    to->source_trill_name                          = from->source_trill_name;
    to->trunk_index                                = from->trunk_index;
    to->protocol_pkt.mmrp_action                   = from->protocol_pkt.mmrp_action;
    to->protocol_pkt.srp_action                    = from->protocol_pkt.srp_action;
    to->protocol_pkt.arp_reply_action              = from->protocol_pkt.arp_reply_action;
    to->protocol_pkt.arp_request_action            = from->protocol_pkt.arp_request_action;
    to->protocol_pkt.nd_action                     = from->protocol_pkt.nd_action;
    to->protocol_pkt.dhcp_action                   = from->protocol_pkt.dhcp_action;
    to->protocol_pkt.igmp_report_leave_action      = from->protocol_pkt.igmp_report_leave_action;
    to->protocol_pkt.igmp_query_action             = from->protocol_pkt.igmp_query_action;
    to->protocol_pkt.igmp_unknown_msg_action       = from->protocol_pkt.igmp_unknown_msg_action;
    to->protocol_pkt.mld_report_done_action        = from->protocol_pkt.mld_report_done_action;
    to->protocol_pkt.mld_query_action              = from->protocol_pkt.mld_query_action;
    to->protocol_pkt.ip4_rsvd_mc_action            = from->protocol_pkt.ip4_rsvd_mc_action;
    to->protocol_pkt.ip6_rsvd_mc_action            = from->protocol_pkt.ip6_rsvd_mc_action;
    to->protocol_pkt.ip4_mc_router_adv_action      = from->protocol_pkt.ip4_mc_router_adv_action;
    to->protocol_pkt.ip6_mc_router_adv_action      = from->protocol_pkt.ip6_mc_router_adv_action;
    to->protocol_pkt.flood_action_according_to_pfm = from->protocol_pkt.flood_action_according_to_pfm;
    to->nat_realm_id                               = from->nat_realm_id;
    to->l3_if_class                                = from->l3_if_class;
    to->vp_mc_ctrl                                 = from->vp_mc_ctrl;
    to->aging_cycles                               = from->aging_cycles;
    to->entropy_id                                 = from->entropy_id;
    to->vpn                                        = from->vpn;
    to->egress_vlan                                = from->egress_vlan;
    to->learn_flags                                = from->learn_flags;
    to->sr_flags                                   = from->sr_flags;
}

/*
 * Function: bcm_compat659_vlan_control_vlan_selective_set
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_control_vlan_selective_set.
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     vlan     - (IN) VLAN to get the flood setting for.
 *     valid_fields - (IN) Valid fields for VLAN control structure,
 *     BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 *     control - (IN) VLAN control structure
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat659_vlan_control_vlan_selective_set(int unit,
                                 bcm_vlan_t vlan, uint32 valid_fields,
                                 bcm_compat659_vlan_control_vlan_t *control)
{
    int rv;
    bcm_compat6518_vlan_control_vlan_t *new_vlan_control = NULL;

    if (NULL != control) {
        /* Create from heap to avoid the stack to bloat */
        new_vlan_control = (bcm_compat6518_vlan_control_vlan_t *)
            sal_alloc(sizeof(bcm_compat6518_vlan_control_vlan_t), "New vlan control vlan");
        if (NULL == new_vlan_control) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat659in_vlan_control_vlan_t(control, new_vlan_control);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6518_vlan_control_vlan_selective_set(unit, vlan, valid_fields,
                                             new_vlan_control);

    /* Transform the entry from the new format to old one */
    _bcm_compat659out_vlan_control_vlan_t(new_vlan_control, control);
    /* Deallocate memory*/
    sal_free(new_vlan_control);

    return rv;
}

/*
 * Function: bcm_compat659_vlan_control_vlan_selective_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_control_vlan_selective_get.
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     vlan     - (IN) VLAN to get the flood setting for.
 *     valid_fields - (IN) Valid fields for VLAN control structure,
 *     BCM_VLAN_CONTROL_VLAN_XXX_MASK.
 *     control - (OUT) VLAN control structure
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat659_vlan_control_vlan_selective_get(int unit,
                                 bcm_vlan_t vlan, uint32 valid_fields,
                                 bcm_compat659_vlan_control_vlan_t *control)
{
    int rv;
    bcm_compat6518_vlan_control_vlan_t *new_vlan_control = NULL;

    if (NULL != control) {
        /* Create from heap to avoid the stack to bloat */
        new_vlan_control = (bcm_compat6518_vlan_control_vlan_t *)
            sal_alloc(sizeof(bcm_compat6518_vlan_control_vlan_t), "New vlan control vlan");
        if (NULL == new_vlan_control) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat659in_vlan_control_vlan_t(control, new_vlan_control);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6518_vlan_control_vlan_selective_get(unit, vlan, valid_fields,
                                             new_vlan_control);

    /* Transform the entry from the new format to old one */
    _bcm_compat659out_vlan_control_vlan_t(new_vlan_control, control);
    /* Deallocate memory*/
    sal_free(new_vlan_control);

    return rv;
}

/*
 * Function: bcm_compat659_vlan_control_vlan_set
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_control_vlan_set.
 *
 * Parameters:
 *      unit    - (IN) StrataSwitch PCI device unit number (driver internal).
 *      vlan     - (IN) VLAN to get the flood setting for.
 *      control - (IN) VLAN control structure
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat659_vlan_control_vlan_set(int unit, bcm_vlan_t vlan,
                                              /* coverity[pass_by_value] */
                                              bcm_compat659_vlan_control_vlan_t control)
{
    int rv;
    bcm_compat6518_vlan_control_vlan_t *new_vlan_control = NULL;

    /* Create from heap to avoid the stack to bloat */
    new_vlan_control = (bcm_compat6518_vlan_control_vlan_t *)
        sal_alloc(sizeof(bcm_compat6518_vlan_control_vlan_t), "New vlan control vlan");
    if (NULL == new_vlan_control) {
        return BCM_E_MEMORY;
    }

    /* Transform the entry from the old format to new one */
    _bcm_compat659in_vlan_control_vlan_t(&control, new_vlan_control);


    /* Call the BCM API with new format */
    rv = bcm_compat6518_vlan_control_vlan_set(unit, vlan, *new_vlan_control);

    /* Transform the entry from the new format to old one */
    _bcm_compat659out_vlan_control_vlan_t(new_vlan_control, &control);
    /* Deallocate memory*/
    sal_free(new_vlan_control);

    return rv;
}


/*
 * Function: bcm_compat659_vlan_control_vlan_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_control_vlan_get.
 *
 * Parameters:
 *      unit    - (IN) StrataSwitch PCI device unit number (driver internal).
 *      vlan     - (IN) VLAN to get the flood setting for.
 *      control - (OUT) VLAN control structure
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat659_vlan_control_vlan_get(int unit,
                                 bcm_vlan_t vlan,
                                 bcm_compat659_vlan_control_vlan_t *control)
{
    int rv;
    bcm_compat6518_vlan_control_vlan_t *new_vlan_control = NULL;

    if (NULL != control) {
        /* Create from heap to avoid the stack to bloat */
        new_vlan_control = (bcm_compat6518_vlan_control_vlan_t *)
            sal_alloc(sizeof(bcm_compat6518_vlan_control_vlan_t), "New vlan control vlan");
        if (NULL == new_vlan_control) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat659in_vlan_control_vlan_t(control, new_vlan_control);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6518_vlan_control_vlan_get(unit, vlan, new_vlan_control);

    /* Transform the entry from the new format to old one */
    _bcm_compat659out_vlan_control_vlan_t(new_vlan_control, control);
    /* Deallocate memory*/
    sal_free(new_vlan_control);

    return rv;
}


/*
 * Function:
 *      _bcm_compat659in_cosq_gport_discard_t
 * Purpose:
 *      Convert the bcm_cosq_gport_discard_t datatype from <=6.5.9 to
 *      SDK 6.5.9+
 * Parameters:
 *      from        - (IN) The <=6.5.9 version of the datatype
 *      to          - (OUT) The SDK 6.5.9+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat659in_cosq_gport_discard_t(bcm_compat659_cosq_gport_discard_t *from,
                           bcm_cosq_gport_discard_t *to)
{
    bcm_cosq_gport_discard_t_init(to);
    to->flags                    = from->flags;
    to->min_thresh               = from->min_thresh;
    to->max_thresh               = from->max_thresh;
    to->drop_probability         = from->drop_probability;
    to->gain                     = from->gain;
    to->ecn_thresh               = from->ecn_thresh;
    to->refresh_time             = from->refresh_time;
}

/*
 * Function:
 *      _bcm_compat659out_cosq_gport_discard_t
 * Purpose:
 *      Convert the bcm_cosq_gport_discard_t datatype from 6.5.9+ to
 *      <=6.5.9
 * Parameters:
 *      from        - (IN) The SDK 6.5.9+ version of the datatype
 *      to          - (OUT) The <=6.5.9 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat659out_cosq_gport_discard_t(bcm_cosq_gport_discard_t *from,
                            bcm_compat659_cosq_gport_discard_t *to)
{
    to->flags                    = from->flags;
    to->min_thresh               = from->min_thresh;
    to->max_thresh               = from->max_thresh;
    to->drop_probability         = from->drop_probability;
    to->gain                     = from->gain;
    to->ecn_thresh               = from->ecn_thresh;
    to->refresh_time             = from->refresh_time;
}

/*
 * Function: bcm_compat659_cosq_gport_discard_set
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_cosq_gport_discard_set.
 *
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) GPORT ID.
 *      cosq    - (IN) COS queue to configure
 *      discard - (IN) Discard settings
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat659_cosq_gport_discard_set(int unit, bcm_gport_t gport,
                                     bcm_cos_queue_t cosq, 
                                     bcm_compat659_cosq_gport_discard_t *discard)
{
    int rv;
    bcm_cosq_gport_discard_t *new_discard = NULL;

    if (NULL != discard) {
        /* Create from heap to avoid the stack to bloat */
        new_discard = (bcm_cosq_gport_discard_t *)
            sal_alloc(sizeof(bcm_cosq_gport_discard_t), "New discard");
        if (NULL == new_discard) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat659in_cosq_gport_discard_t(discard, new_discard);
    }

    /* Call the BCM API with new format */
    rv = bcm_cosq_gport_discard_set(unit, gport, cosq, new_discard);

    if (NULL != new_discard) {
        /* Transform the entry from the new format to old one */
        _bcm_compat659out_cosq_gport_discard_t(new_discard, discard);
        /* Deallocate memory*/
        sal_free(new_discard);
    }

    return rv;
}

/*
 * Function: bcm_compat659_cosq_gport_discard_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_cosq_gport_discard_get.
 *
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) GPORT ID.
 *      cosq    - (IN) COS queue to get
 *      discard - (IN/OUT) Discard settings
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat659_cosq_gport_discard_get(int unit, bcm_gport_t gport,
                                     bcm_cos_queue_t cosq, 
                                     bcm_compat659_cosq_gport_discard_t *discard)
{
    int rv;
    bcm_cosq_gport_discard_t *new_discard = NULL;

    /* Create from heap to avoid the stack to bloat */
    if (NULL != discard) {
        new_discard = (bcm_cosq_gport_discard_t *)
            sal_alloc(sizeof(bcm_cosq_gport_discard_t), "New discard");
        if (NULL == new_discard) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat659in_cosq_gport_discard_t(discard, new_discard);
    }

    /* Call the BCM API with new format */
    rv = bcm_cosq_gport_discard_get(unit, gport, cosq, new_discard);

    if (NULL != new_discard) {
        /* Transform the entry from the new format to old one */
        _bcm_compat659out_cosq_gport_discard_t(new_discard, discard);
        /* Deallocate memory*/
        sal_free(new_discard);
    }

    return rv;
}

/*
 * Function: bcm_compat659_cosq_gport_discard_extended_set
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_cosq_gport_discard_extended_set.
 *
 * Parameters:
 *      unit    - (IN) Unit number.
 *      id      - (IN) Integrated cosq index
 *      discard - (IN) Discard settings
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat659_cosq_gport_discard_extended_set(int unit, bcm_cosq_object_id_t *id,
                                     bcm_compat659_cosq_gport_discard_t *discard)
{
    int rv;
    bcm_cosq_gport_discard_t *new_discard = NULL;

    if (NULL != discard) {
        /* Create from heap to avoid the stack to bloat */
        new_discard = (bcm_cosq_gport_discard_t *)
            sal_alloc(sizeof(bcm_cosq_gport_discard_t), "New discard");
        if (NULL == new_discard) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat659in_cosq_gport_discard_t(discard, new_discard);
    }

    /* Call the BCM API with new format */
    rv = bcm_cosq_gport_discard_extended_set(unit, id, new_discard);

    if (NULL != new_discard) {
        /* Transform the entry from the new format to old one */
        _bcm_compat659out_cosq_gport_discard_t(new_discard, discard);
        /* Deallocate memory*/
        sal_free(new_discard);
    }

    return rv;
}

/*
 * Function: bcm_compat659_cosq_gport_discard_extended_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_cosq_gport_discard_extended_get.
 *
 * Parameters:
 *      unit    - (IN) Unit number.
 *      id      - (IN) Integrated cosq index
 *      discard - (IN) Discard settings
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat659_cosq_gport_discard_extended_get(int unit, bcm_cosq_object_id_t *id,
                                     bcm_compat659_cosq_gport_discard_t *discard)
{
    int rv;
    bcm_cosq_gport_discard_t *new_discard = NULL;

    /* Create from heap to avoid the stack to bloat */
    if (NULL != discard) {
        new_discard = (bcm_cosq_gport_discard_t *)
            sal_alloc(sizeof(bcm_cosq_gport_discard_t), "New discard");
        if (NULL == new_discard) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat659in_cosq_gport_discard_t(discard, new_discard);
    }

    /* Call the BCM API with new format */
    rv = bcm_cosq_gport_discard_extended_get(unit, id, new_discard);

    if (NULL != new_discard) {
        /* Transform the entry from the new format to old one */
        _bcm_compat659out_cosq_gport_discard_t(new_discard, discard);
        /* Deallocate memory*/
        sal_free(new_discard);
    }

    return rv;
}

#endif  /* BCM_RPC_SUPPORT */
