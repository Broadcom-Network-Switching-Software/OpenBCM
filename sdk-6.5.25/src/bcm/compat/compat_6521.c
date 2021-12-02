/*
* $Id: compat_6521.c,v 1.0 2020/12/02
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2021 Broadcom Inc. All rights reserved.
*
* RPC Compatibility with sdk-6.5.21 routines
*/

#ifdef BCM_RPC_SUPPORT
#include <shared/alloc.h>
#include <bcm/error.h>
#include <bcm_int/compat/compat_6521.h>




#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6521in_l3_egress_t
 * Purpose:
 *      Convert the bcm_l3_egress_t datatype from <=6.5.21 to
 *      SDK 6.5.22+
 * Parameters:
 *      from        - (IN) The <=6.5.21 version of the datatype
 *      to          - (OUT) The SDK 6.5.22+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521in_l3_egress_t(
    bcm_compat6521_l3_egress_t *from,
    bcm_l3_egress_t *to)
{
    int i1 = 0;

    bcm_l3_egress_t_init(to);
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->flags3 = from->flags3;
    to->intf = from->intf;
    for (i1 = 0; i1 < 6; i1++) {
        to->mac_addr[i1] = from->mac_addr[i1];
    }
    to->vlan = from->vlan;
    to->module = from->module;
    to->port = from->port;
    to->trunk = from->trunk;
    to->mpls_flags = from->mpls_flags;
    to->mpls_label = from->mpls_label;
    to->mpls_action = from->mpls_action;
    to->mpls_qos_map_id = from->mpls_qos_map_id;
    to->mpls_ttl = from->mpls_ttl;
    to->mpls_pkt_pri = from->mpls_pkt_pri;
    to->mpls_pkt_cfi = from->mpls_pkt_cfi;
    to->mpls_exp = from->mpls_exp;
    to->qos_map_id = from->qos_map_id;
    to->encap_id = from->encap_id;
    to->failover_id = from->failover_id;
    to->failover_if_id = from->failover_if_id;
    to->failover_mc_group = from->failover_mc_group;
    to->dynamic_scaling_factor = from->dynamic_scaling_factor;
    to->dynamic_load_weight = from->dynamic_load_weight;
    to->dynamic_queue_size_weight = from->dynamic_queue_size_weight;
    to->intf_class = from->intf_class;
    to->multicast_flags = from->multicast_flags;
    to->oam_global_context_id = from->oam_global_context_id;
    sal_memcpy(&to->vntag, &from->vntag, sizeof(bcm_vntag_t));
    to->vntag_action = from->vntag_action;
    sal_memcpy(&to->etag, &from->etag, sizeof(bcm_etag_t));
    to->etag_action = from->etag_action;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    to->flow_label_option_handle = from->flow_label_option_handle;
    for (i1 = 0; i1 < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i1++) {
        sal_memcpy(&to->logical_fields[i1], &from->logical_fields[i1], sizeof(bcm_flow_logical_field_t));
    }
    to->num_of_fields = from->num_of_fields;
    to->counting_profile = from->counting_profile;
    to->mpls_ecn_map_id = from->mpls_ecn_map_id;
    to->urpf_mode = from->urpf_mode;
    to->mc_group = from->mc_group;
    for (i1 = 0; i1 < 6; i1++) {
        to->src_mac_addr[i1] = from->src_mac_addr[i1];
    }
    to->hierarchical_gport = from->hierarchical_gport;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->vlan_port_id = from->vlan_port_id;
    to->replication_id = from->replication_id;
    to->mtu = from->mtu;
    to->nat_realm_id = from->nat_realm_id;
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
    to->egress_opaque_ctrl_id = from->egress_opaque_ctrl_id;
}

/*
 * Function:
 *      _bcm_compat6521out_l3_egress_t
 * Purpose:
 *      Convert the bcm_l3_egress_t datatype from SDK 6.5.22+ to
 *      <=6.5.21
 * Parameters:
 *      from        - (IN) The SDK 6.5.22+ version of the datatype
 *      to          - (OUT) The <=6.5.21 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521out_l3_egress_t(
    bcm_l3_egress_t *from,
    bcm_compat6521_l3_egress_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->flags3 = from->flags3;
    to->intf = from->intf;
    for (i1 = 0; i1 < 6; i1++) {
        to->mac_addr[i1] = from->mac_addr[i1];
    }
    to->vlan = from->vlan;
    to->module = from->module;
    to->port = from->port;
    to->trunk = from->trunk;
    to->mpls_flags = from->mpls_flags;
    to->mpls_label = from->mpls_label;
    to->mpls_action = from->mpls_action;
    to->mpls_qos_map_id = from->mpls_qos_map_id;
    to->mpls_ttl = from->mpls_ttl;
    to->mpls_pkt_pri = from->mpls_pkt_pri;
    to->mpls_pkt_cfi = from->mpls_pkt_cfi;
    to->mpls_exp = from->mpls_exp;
    to->qos_map_id = from->qos_map_id;
    to->encap_id = from->encap_id;
    to->failover_id = from->failover_id;
    to->failover_if_id = from->failover_if_id;
    to->failover_mc_group = from->failover_mc_group;
    to->dynamic_scaling_factor = from->dynamic_scaling_factor;
    to->dynamic_load_weight = from->dynamic_load_weight;
    to->dynamic_queue_size_weight = from->dynamic_queue_size_weight;
    to->intf_class = from->intf_class;
    to->multicast_flags = from->multicast_flags;
    to->oam_global_context_id = from->oam_global_context_id;
    sal_memcpy(&to->vntag, &from->vntag, sizeof(bcm_vntag_t));
    to->vntag_action = from->vntag_action;
    sal_memcpy(&to->etag, &from->etag, sizeof(bcm_etag_t));
    to->etag_action = from->etag_action;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    to->flow_label_option_handle = from->flow_label_option_handle;
    for (i1 = 0; i1 < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i1++) {
        sal_memcpy(&to->logical_fields[i1], &from->logical_fields[i1], sizeof(bcm_flow_logical_field_t));
    }
    to->num_of_fields = from->num_of_fields;
    to->counting_profile = from->counting_profile;
    to->mpls_ecn_map_id = from->mpls_ecn_map_id;
    to->urpf_mode = from->urpf_mode;
    to->mc_group = from->mc_group;
    for (i1 = 0; i1 < 6; i1++) {
        to->src_mac_addr[i1] = from->src_mac_addr[i1];
    }
    to->hierarchical_gport = from->hierarchical_gport;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->vlan_port_id = from->vlan_port_id;
    to->replication_id = from->replication_id;
    to->mtu = from->mtu;
    to->nat_realm_id = from->nat_realm_id;
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
    to->egress_opaque_ctrl_id = from->egress_opaque_ctrl_id;
}

/*
 * Function:
 *      bcm_compat6521_l3_egress_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) BCM_L3_REPLACE: replace existing. BCM_L3_WITH_ID: intf argument is given.
 *      egr - (INOUT) (IN/OUT) Egress forwarding destination.
 *      if_id - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_l3_egress_create(
    int unit,
    uint32 flags,
    bcm_compat6521_l3_egress_t *egr,
    bcm_if_t *if_id)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_t *new_egr = NULL;

    if (egr != NULL) {
        new_egr = (bcm_l3_egress_t *)
                     sal_alloc(sizeof(bcm_l3_egress_t),
                     "New egr");
        if (new_egr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_l3_egress_t(egr, new_egr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_create(unit, flags, new_egr, if_id);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_l3_egress_t(new_egr, egr);

    /* Deallocate memory*/
    sal_free(new_egr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_l3_egress_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_get.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      intf - (IN) L3 interface ID pointing to Egress object.
 *      egr - (OUT) Egress forwarding path properties.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_l3_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6521_l3_egress_t *egr)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_t *new_egr = NULL;

    if (egr != NULL) {
        new_egr = (bcm_l3_egress_t *)
                     sal_alloc(sizeof(bcm_l3_egress_t),
                     "New egr");
        if (new_egr == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_get(unit, intf, new_egr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_l3_egress_t(new_egr, egr);

    /* Deallocate memory*/
    sal_free(new_egr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_l3_egress_find
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_find.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      egr - (IN) Egress forwarding path properties.
 *      intf - (OUT) L3 interface ID pointing to Egress object.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_l3_egress_find(
    int unit,
    bcm_compat6521_l3_egress_t *egr,
    bcm_if_t *intf)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_t *new_egr = NULL;

    if (egr != NULL) {
        new_egr = (bcm_l3_egress_t *)
                     sal_alloc(sizeof(bcm_l3_egress_t),
                     "New egr");
        if (new_egr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_l3_egress_t(egr, new_egr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_find(unit, new_egr, intf);


    /* Deallocate memory*/
    sal_free(new_egr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_l3_egress_allocation_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_allocation_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) 
 *      egr - (INOUT) 
 *      nof_members - (IN) 
 *      if_id - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_l3_egress_allocation_get(
    int unit,
    uint32 flags,
    bcm_compat6521_l3_egress_t *egr,
    uint32 nof_members,
    bcm_if_t *if_id)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_t *new_egr = NULL;

    if (egr != NULL) {
        new_egr = (bcm_l3_egress_t *)
                     sal_alloc(sizeof(bcm_l3_egress_t),
                     "New egr");
        if (new_egr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_l3_egress_t(egr, new_egr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_allocation_get(unit, flags, new_egr, nof_members, if_id);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_l3_egress_t(new_egr, egr);

    /* Deallocate memory*/
    sal_free(new_egr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_failover_egress_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_failover_egress_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN) 
 *      failover_egr - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_failover_egress_set(
    int unit,
    bcm_if_t intf,
    bcm_compat6521_l3_egress_t *failover_egr)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_t *new_failover_egr = NULL;

    if (failover_egr != NULL) {
        new_failover_egr = (bcm_l3_egress_t *)
                     sal_alloc(sizeof(bcm_l3_egress_t),
                     "New failover_egr");
        if (new_failover_egr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_l3_egress_t(failover_egr, new_failover_egr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_failover_egress_set(unit, intf, new_failover_egr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_l3_egress_t(new_failover_egr, failover_egr);

    /* Deallocate memory*/
    sal_free(new_failover_egr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_failover_egress_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_failover_egress_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN) 
 *      failover_egr - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_failover_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6521_l3_egress_t *failover_egr)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_t *new_failover_egr = NULL;

    if (failover_egr != NULL) {
        new_failover_egr = (bcm_l3_egress_t *)
                     sal_alloc(sizeof(bcm_l3_egress_t),
                     "New failover_egr");
        if (new_failover_egr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_l3_egress_t(failover_egr, new_failover_egr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_failover_egress_get(unit, intf, new_failover_egr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_l3_egress_t(new_failover_egr, failover_egr);

    /* Deallocate memory*/
    sal_free(new_failover_egr);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6521in_l3_egress_ecmp_t
 * Purpose:
 *      Convert the bcm_l3_egress_ecmp_t datatype from <=6.5.21 to
 *      SDK 6.5.22+
 * Parameters:
 *      from        - (IN) The <=6.5.21 version of the datatype
 *      to          - (OUT) The SDK 6.5.22+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521in_l3_egress_ecmp_t(
    bcm_compat6521_l3_egress_ecmp_t *from,
    bcm_l3_egress_ecmp_t *to)
{
    bcm_l3_egress_ecmp_t_init(to);
    to->flags = from->flags;
    to->ecmp_intf = from->ecmp_intf;
    to->max_paths = from->max_paths;
    to->ecmp_group_flags = from->ecmp_group_flags;
    to->dynamic_mode = from->dynamic_mode;
    to->dynamic_size = from->dynamic_size;
    to->dynamic_age = from->dynamic_age;
    to->dynamic_load_exponent = from->dynamic_load_exponent;
    to->dynamic_expected_load_exponent = from->dynamic_expected_load_exponent;
    sal_memcpy(&to->dgm, &from->dgm, sizeof(bcm_l3_dgm_t));
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->rpf_mode = from->rpf_mode;
    sal_memcpy(&to->tunnel_priority, &from->tunnel_priority, sizeof(bcm_l3_tunnel_priority_info_t));
}

/*
 * Function:
 *      _bcm_compat6521out_l3_egress_ecmp_t
 * Purpose:
 *      Convert the bcm_l3_egress_ecmp_t datatype from SDK 6.5.22+ to
 *      <=6.5.21
 * Parameters:
 *      from        - (IN) The SDK 6.5.22+ version of the datatype
 *      to          - (OUT) The <=6.5.21 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521out_l3_egress_ecmp_t(
    bcm_l3_egress_ecmp_t *from,
    bcm_compat6521_l3_egress_ecmp_t *to)
{
    to->flags = from->flags;
    to->ecmp_intf = from->ecmp_intf;
    to->max_paths = from->max_paths;
    to->ecmp_group_flags = from->ecmp_group_flags;
    to->dynamic_mode = from->dynamic_mode;
    to->dynamic_size = from->dynamic_size;
    to->dynamic_age = from->dynamic_age;
    to->dynamic_load_exponent = from->dynamic_load_exponent;
    to->dynamic_expected_load_exponent = from->dynamic_expected_load_exponent;
    sal_memcpy(&to->dgm, &from->dgm, sizeof(bcm_l3_dgm_t));
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->rpf_mode = from->rpf_mode;
    sal_memcpy(&to->tunnel_priority, &from->tunnel_priority, sizeof(bcm_l3_tunnel_priority_info_t));
}

/*
 * Function:
 *      _bcm_compat6521in_l3_egress_ecmp_resilient_entry_t
 * Purpose:
 *      Convert the bcm_l3_egress_ecmp_resilient_entry_t datatype from <=6.5.21 to
 *      SDK 6.5.22+
 * Parameters:
 *      from        - (IN) The <=6.5.21 version of the datatype
 *      to          - (OUT) The SDK 6.5.22+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521in_l3_egress_ecmp_resilient_entry_t(
    bcm_compat6521_l3_egress_ecmp_resilient_entry_t *from,
    bcm_l3_egress_ecmp_resilient_entry_t *to)
{
    to->hash_key = from->hash_key;
    _bcm_compat6521in_l3_egress_ecmp_t(&from->ecmp, &to->ecmp);
    to->intf = from->intf;
}

/*
 * Function:
 *      bcm_compat6521_l3_egress_ecmp_resilient_replace
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_resilient_replace.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) BCM_TRUNK_RESILIENT flags.
 *      match_entry - (IN) Matching criteria
 *      num_entries - (OUT) Number of entries matched.
 *      replace_entry - (IN) Replacing entry when the action is replace.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_l3_egress_ecmp_resilient_replace(
    int unit,
    uint32 flags,
    bcm_compat6521_l3_egress_ecmp_resilient_entry_t *match_entry,
    int *num_entries,
    bcm_compat6521_l3_egress_ecmp_resilient_entry_t *replace_entry)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_resilient_entry_t *new_match_entry = NULL;
    bcm_l3_egress_ecmp_resilient_entry_t *new_replace_entry = NULL;

    if (match_entry != NULL && replace_entry != NULL) {
        new_match_entry = (bcm_l3_egress_ecmp_resilient_entry_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_resilient_entry_t),
                     "New match_entry");
        if (new_match_entry == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_l3_egress_ecmp_resilient_entry_t(match_entry, new_match_entry);
        new_replace_entry = (bcm_l3_egress_ecmp_resilient_entry_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_resilient_entry_t),
                     "New replace_entry");
        if (new_replace_entry == NULL) {
            sal_free(new_match_entry);
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_l3_egress_ecmp_resilient_entry_t(replace_entry, new_replace_entry);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_ecmp_resilient_replace(unit, flags, new_match_entry, num_entries, new_replace_entry);


    /* Deallocate memory*/
    sal_free(new_match_entry);
    sal_free(new_replace_entry);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_l3_egress_ecmp_resilient_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_resilient_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) BCM_L3_ECMP_RESILIENT flags.
 *      entry - (IN) Entry criteria
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_l3_egress_ecmp_resilient_add(
    int unit,
    uint32 flags,
    bcm_compat6521_l3_egress_ecmp_resilient_entry_t *entry)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_resilient_entry_t *new_entry = NULL;

    if (entry != NULL) {
        new_entry = (bcm_l3_egress_ecmp_resilient_entry_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_resilient_entry_t),
                     "New entry");
        if (new_entry == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_l3_egress_ecmp_resilient_entry_t(entry, new_entry);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_ecmp_resilient_add(unit, flags, new_entry);


    /* Deallocate memory*/
    sal_free(new_entry);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_l3_egress_ecmp_resilient_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_resilient_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) BCM_L3_ECMP_RESILIENT flags.
 *      entry - (IN) Entry criteria
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_l3_egress_ecmp_resilient_delete(
    int unit,
    uint32 flags,
    bcm_compat6521_l3_egress_ecmp_resilient_entry_t *entry)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_resilient_entry_t *new_entry = NULL;

    if (entry != NULL) {
        new_entry = (bcm_l3_egress_ecmp_resilient_entry_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_resilient_entry_t),
                     "New entry");
        if (new_entry == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_l3_egress_ecmp_resilient_entry_t(entry, new_entry);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_ecmp_resilient_delete(unit, flags, new_entry);


    /* Deallocate memory*/
    sal_free(new_entry);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_l3_ecmp_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_ecmp_create.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      options - (IN) L3_ECMP_O_xxx flags.
 *      ecmp_info - (INOUT) (IN/OUT) ECMP group info.
 *      ecmp_member_count - (IN) Number of elements in ecmp_member_array.
 *      ecmp_member_array - (IN) Member array of Egress forwarding objects.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_l3_ecmp_create(
    int unit,
    uint32 options,
    bcm_compat6521_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_count,
    bcm_l3_ecmp_member_t *ecmp_member_array)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_t *new_ecmp_info = NULL;

    if (ecmp_info != NULL) {
        new_ecmp_info = (bcm_l3_egress_ecmp_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_t),
                     "New ecmp_info");
        if (new_ecmp_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_l3_egress_ecmp_t(ecmp_info, new_ecmp_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_ecmp_create(unit, options, new_ecmp_info, ecmp_member_count, ecmp_member_array);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_l3_egress_ecmp_t(new_ecmp_info, ecmp_info);

    /* Deallocate memory*/
    sal_free(new_ecmp_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_l3_ecmp_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_ecmp_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ecmp_info - (INOUT) (IN/OUT) ECMP group info.
 *      ecmp_member_size - (IN) Size of allocated entries in ecmp_member_array.
 *      ecmp_member_array - (OUT) Member array of Egress forwarding objects.
 *      ecmp_member_count - (OUT) Number of entries of ecmp_member_count actually filled in. This will be a value less than or equal to the value passed in as ecmp_member_size unless ecmp_member_size is 0. If ecmp_member_size is 0 then ecmp_member_array is ignored and ecmp_member_count is filled in with the number of entries that would have been filled into ecmp_member_array if ecmp_member_size was arbitrarily large.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_l3_ecmp_get(
    int unit,
    bcm_compat6521_l3_egress_ecmp_t *ecmp_info,
    int ecmp_member_size,
    bcm_l3_ecmp_member_t *ecmp_member_array,
    int *ecmp_member_count)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_t *new_ecmp_info = NULL;

    if (ecmp_info != NULL) {
        new_ecmp_info = (bcm_l3_egress_ecmp_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_t),
                     "New ecmp_info");
        if (new_ecmp_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_l3_egress_ecmp_t(ecmp_info, new_ecmp_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_ecmp_get(unit, new_ecmp_info, ecmp_member_size, ecmp_member_array, ecmp_member_count);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_l3_egress_ecmp_t(new_ecmp_info, ecmp_info);

    /* Deallocate memory*/
    sal_free(new_ecmp_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_l3_ecmp_find
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_ecmp_find.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      ecmp_member_count - (IN) Number of member in ecmp_member_array.
 *      ecmp_member_array - (IN) Member array of Egress forwarding objects.
 *      ecmp_info - (OUT) ECMP group info.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_l3_ecmp_find(
    int unit,
    int ecmp_member_count,
    bcm_l3_ecmp_member_t *ecmp_member_array,
    bcm_compat6521_l3_egress_ecmp_t *ecmp_info)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_t *new_ecmp_info = NULL;

    if (ecmp_info != NULL) {
        new_ecmp_info = (bcm_l3_egress_ecmp_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_t),
                     "New ecmp_info");
        if (new_ecmp_info == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_ecmp_find(unit, ecmp_member_count, ecmp_member_array, new_ecmp_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_l3_egress_ecmp_t(new_ecmp_info, ecmp_info);

    /* Deallocate memory*/
    sal_free(new_ecmp_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_l3_egress_ecmp_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_create.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      ecmp - (INOUT) (IN/OUT) ECMP group info.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_l3_egress_ecmp_create(
    int unit,
    bcm_compat6521_l3_egress_ecmp_t *ecmp,
    int intf_count,
    bcm_if_t *intf_array)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_t *new_ecmp = NULL;

    if (ecmp != NULL) {
        new_ecmp = (bcm_l3_egress_ecmp_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_t),
                     "New ecmp");
        if (new_ecmp == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_l3_egress_ecmp_t(ecmp, new_ecmp);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_ecmp_create(unit, new_ecmp, intf_count, intf_array);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_l3_egress_ecmp_t(new_ecmp, ecmp);

    /* Deallocate memory*/
    sal_free(new_ecmp);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_l3_egress_ecmp_destroy
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_destroy.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      ecmp - (IN) ECMP group info.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_l3_egress_ecmp_destroy(
    int unit,
    bcm_compat6521_l3_egress_ecmp_t *ecmp)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_t *new_ecmp = NULL;

    if (ecmp != NULL) {
        new_ecmp = (bcm_l3_egress_ecmp_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_t),
                     "New ecmp");
        if (new_ecmp == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_l3_egress_ecmp_t(ecmp, new_ecmp);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_ecmp_destroy(unit, new_ecmp);


    /* Deallocate memory*/
    sal_free(new_ecmp);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_l3_egress_ecmp_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_get.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      ecmp - (INOUT) (IN/OUT) ECMP group info.
 *      intf_size - (IN) Size of allocated entries in intf_array.
 *      intf_array - (OUT) Array of Egress forwarding objects.
 *      intf_count - (OUT) Number of entries of intf_count actually filled in. This will be a value less than or equal to the value passed in as intf_size unless intf_size is 0. If intf_size is 0 then intf_array is ignored and intf_count is filled in with the number of entries that would have been filled into intf_array if intf_size was arbitrarily large.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_l3_egress_ecmp_get(
    int unit,
    bcm_compat6521_l3_egress_ecmp_t *ecmp,
    int intf_size,
    bcm_if_t *intf_array,
    int *intf_count)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_t *new_ecmp = NULL;

    if (ecmp != NULL) {
        new_ecmp = (bcm_l3_egress_ecmp_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_t),
                     "New ecmp");
        if (new_ecmp == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_l3_egress_ecmp_t(ecmp, new_ecmp);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_ecmp_get(unit, new_ecmp, intf_size, intf_array, intf_count);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_l3_egress_ecmp_t(new_ecmp, ecmp);

    /* Deallocate memory*/
    sal_free(new_ecmp);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_l3_egress_ecmp_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ecmp - (IN) ECMP group info.
 *      intf - (IN) L3 interface ID pointing to Egress forwarding object.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_l3_egress_ecmp_add(
    int unit,
    bcm_compat6521_l3_egress_ecmp_t *ecmp,
    bcm_if_t intf)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_t *new_ecmp = NULL;

    if (ecmp != NULL) {
        new_ecmp = (bcm_l3_egress_ecmp_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_t),
                     "New ecmp");
        if (new_ecmp == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_l3_egress_ecmp_t(ecmp, new_ecmp);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_ecmp_add(unit, new_ecmp, intf);


    /* Deallocate memory*/
    sal_free(new_ecmp);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_l3_egress_ecmp_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ecmp - (IN) ECMP group info.
 *      intf - (IN) L3 interface ID pointing to Egress forwarding object.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_l3_egress_ecmp_delete(
    int unit,
    bcm_compat6521_l3_egress_ecmp_t *ecmp,
    bcm_if_t intf)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_t *new_ecmp = NULL;

    if (ecmp != NULL) {
        new_ecmp = (bcm_l3_egress_ecmp_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_t),
                     "New ecmp");
        if (new_ecmp == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_l3_egress_ecmp_t(ecmp, new_ecmp);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_ecmp_delete(unit, new_ecmp, intf);


    /* Deallocate memory*/
    sal_free(new_ecmp);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_l3_egress_ecmp_find
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_find.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 *      ecmp - (OUT) ECMP group info.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_l3_egress_ecmp_find(
    int unit,
    int intf_count,
    bcm_if_t *intf_array,
    bcm_compat6521_l3_egress_ecmp_t *ecmp)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_t *new_ecmp = NULL;

    if (ecmp != NULL) {
        new_ecmp = (bcm_l3_egress_ecmp_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_t),
                     "New ecmp");
        if (new_ecmp == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_ecmp_find(unit, intf_count, intf_array, new_ecmp);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_l3_egress_ecmp_t(new_ecmp, ecmp);

    /* Deallocate memory*/
    sal_free(new_ecmp);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_l3_egress_ecmp_tunnel_priority_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_tunnel_priority_set.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      ecmp - (IN) ECMP group info.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_l3_egress_ecmp_tunnel_priority_set(
    int unit,
    bcm_compat6521_l3_egress_ecmp_t *ecmp,
    int intf_count,
    bcm_if_t *intf_array)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_ecmp_t *new_ecmp = NULL;

    if (ecmp != NULL) {
        new_ecmp = (bcm_l3_egress_ecmp_t *)
                     sal_alloc(sizeof(bcm_l3_egress_ecmp_t),
                     "New ecmp");
        if (new_ecmp == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_l3_egress_ecmp_t(ecmp, new_ecmp);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_ecmp_tunnel_priority_set(unit, new_ecmp, intf_count, intf_array);


    /* Deallocate memory*/
    sal_free(new_ecmp);

    return rv;
}


#endif /* defined(INCLUDE_L3) */
#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6521in_mpls_esi_info_t
 * Purpose:
 *      Convert the bcm_mpls_esi_info_t datatype from <=6.5.21 to
 *      SDK 6.5.22+
 * Parameters:
 *      from        - (IN) The <=6.5.21 version of the datatype
 *      to          - (OUT) The SDK 6.5.22+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521in_mpls_esi_info_t(
    bcm_compat6521_mpls_esi_info_t *from,
    bcm_mpls_esi_info_t *to)
{
    bcm_mpls_esi_info_t_init(to);
    to->flags = from->flags;
    to->src_port = from->src_port;
    to->out_class_id = from->out_class_id;
    to->esi_label = from->esi_label;
}

/*
 * Function:
 *      _bcm_compat6521out_mpls_esi_info_t
 * Purpose:
 *      Convert the bcm_mpls_esi_info_t datatype from SDK 6.5.22+ to
 *      <=6.5.21
 * Parameters:
 *      from        - (IN) The SDK 6.5.22+ version of the datatype
 *      to          - (OUT) The <=6.5.21 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521out_mpls_esi_info_t(
    bcm_mpls_esi_info_t *from,
    bcm_compat6521_mpls_esi_info_t *to)
{
    to->flags = from->flags;
    to->src_port = from->src_port;
    to->out_class_id = from->out_class_id;
    to->esi_label = from->esi_label;
}

/*
 * Function:
 *      bcm_compat6521_mpls_esi_encap_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_esi_encap_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      esi_info - (IN) ) info about the esi
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_mpls_esi_encap_add(
    int unit,
    bcm_compat6521_mpls_esi_info_t *esi_info)
{
    int rv = BCM_E_NONE;
    bcm_mpls_esi_info_t *new_esi_info = NULL;

    if (esi_info != NULL) {
        new_esi_info = (bcm_mpls_esi_info_t *)
                     sal_alloc(sizeof(bcm_mpls_esi_info_t),
                     "New esi_info");
        if (new_esi_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_mpls_esi_info_t(esi_info, new_esi_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_esi_encap_add(unit, new_esi_info);


    /* Deallocate memory*/
    sal_free(new_esi_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_mpls_esi_encap_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_esi_encap_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      esi_info - (INOUT) ) info about the esi
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_mpls_esi_encap_get(
    int unit,
    bcm_compat6521_mpls_esi_info_t *esi_info)
{
    int rv = BCM_E_NONE;
    bcm_mpls_esi_info_t *new_esi_info = NULL;

    if (esi_info != NULL) {
        new_esi_info = (bcm_mpls_esi_info_t *)
                     sal_alloc(sizeof(bcm_mpls_esi_info_t),
                     "New esi_info");
        if (new_esi_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_mpls_esi_info_t(esi_info, new_esi_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_esi_encap_get(unit, new_esi_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_mpls_esi_info_t(new_esi_info, esi_info);

    /* Deallocate memory*/
    sal_free(new_esi_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_mpls_esi_encap_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_esi_encap_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      esi_info - (IN) info about the esi
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_mpls_esi_encap_delete(
    int unit,
    bcm_compat6521_mpls_esi_info_t *esi_info)
{
    int rv = BCM_E_NONE;
    bcm_mpls_esi_info_t *new_esi_info = NULL;

    if (esi_info != NULL) {
        new_esi_info = (bcm_mpls_esi_info_t *)
                     sal_alloc(sizeof(bcm_mpls_esi_info_t),
                     "New esi_info");
        if (new_esi_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_mpls_esi_info_t(esi_info, new_esi_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_esi_encap_delete(unit, new_esi_info);


    /* Deallocate memory*/
    sal_free(new_esi_info);

    return rv;
}


#endif /* defined(INCLUDE_L3) */

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6521in_flow_tunnel_initiator_t
 * Purpose:
 *      Convert the bcm_flow_tunnel_initiator_t datatype from <=6.5.21 to
 *      SDK 6.5.22+
 * Parameters:
 *      from        - (IN) The <=6.5.21 version of the datatype
 *      to          - (OUT) The SDK 6.5.22+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521in_flow_tunnel_initiator_t(
    bcm_compat6521_flow_tunnel_initiator_t *from,
    bcm_flow_tunnel_initiator_t *to)
{
    int i1 = 0;

    bcm_flow_tunnel_initiator_t_init(to);
    to->flags = from->flags;
    to->type = from->type;
    to->ttl = from->ttl;
    for (i1 = 0; i1 < 6; i1++) {
        to->dmac[i1] = from->dmac[i1];
    }
    to->dip = from->dip;
    to->sip = from->sip;
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6[i1] = from->sip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6[i1] = from->dip6[i1];
    }
    to->flow_label = from->flow_label;
    to->dscp_sel = from->dscp_sel;
    to->dscp = from->dscp;
    to->dscp_map = from->dscp_map;
    to->tunnel_id = from->tunnel_id;
    to->udp_dst_port = from->udp_dst_port;
    to->udp_src_port = from->udp_src_port;
    for (i1 = 0; i1 < 6; i1++) {
        to->smac[i1] = from->smac[i1];
    }
    to->vlan = from->vlan;
    to->tpid = from->tpid;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->ip4_id = from->ip4_id;
    to->l3_intf_id = from->l3_intf_id;
    to->flow_port = from->flow_port;
    to->valid_elements = from->valid_elements;
    to->flow_handle = from->flow_handle;
    to->flow_option = from->flow_option;
    to->dscp_ecn_sel = from->dscp_ecn_sel;
    to->dscp_ecn_map = from->dscp_ecn_map;
    to->ecn = from->ecn;
    to->pri_sel = from->pri_sel;
}

/*
 * Function:
 *      _bcm_compat6521out_flow_tunnel_initiator_t
 * Purpose:
 *      Convert the bcm_flow_tunnel_initiator_t datatype from SDK 6.5.22+ to
 *      <=6.5.21
 * Parameters:
 *      from        - (IN) The SDK 6.5.22+ version of the datatype
 *      to          - (OUT) The <=6.5.21 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521out_flow_tunnel_initiator_t(
    bcm_flow_tunnel_initiator_t *from,
    bcm_compat6521_flow_tunnel_initiator_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->type = from->type;
    to->ttl = from->ttl;
    for (i1 = 0; i1 < 6; i1++) {
        to->dmac[i1] = from->dmac[i1];
    }
    to->dip = from->dip;
    to->sip = from->sip;
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6[i1] = from->sip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6[i1] = from->dip6[i1];
    }
    to->flow_label = from->flow_label;
    to->dscp_sel = from->dscp_sel;
    to->dscp = from->dscp;
    to->dscp_map = from->dscp_map;
    to->tunnel_id = from->tunnel_id;
    to->udp_dst_port = from->udp_dst_port;
    to->udp_src_port = from->udp_src_port;
    for (i1 = 0; i1 < 6; i1++) {
        to->smac[i1] = from->smac[i1];
    }
    to->vlan = from->vlan;
    to->tpid = from->tpid;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->ip4_id = from->ip4_id;
    to->l3_intf_id = from->l3_intf_id;
    to->flow_port = from->flow_port;
    to->valid_elements = from->valid_elements;
    to->flow_handle = from->flow_handle;
    to->flow_option = from->flow_option;
    to->dscp_ecn_sel = from->dscp_ecn_sel;
    to->dscp_ecn_map = from->dscp_ecn_map;
    to->ecn = from->ecn;
    to->pri_sel = from->pri_sel;
}

/*
 * Function:
 *      bcm_compat6521_flow_tunnel_initiator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_tunnel_initiator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) (IN/OUT) Tunnel initiator configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_flow_tunnel_initiator_create(
    int unit,
    bcm_compat6521_flow_tunnel_initiator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_flow_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_flow_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_initiator_create(unit, new_info, num_of_fields, field);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_flow_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_flow_tunnel_initiator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_tunnel_initiator_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) (IN/OUT) Tunnel initiator configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_flow_tunnel_initiator_get(
    int unit,
    bcm_compat6521_flow_tunnel_initiator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_flow_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_flow_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_tunnel_initiator_get(unit, new_info, num_of_fields, field);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_flow_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}


#endif /* defined(INCLUDE_L3) */

#if defined(INCLUDE_BFD)
/*
 *      _bcm_compat6521in_bfd_discard_stat_t
 * Purpose:
 *      Convert the bcm_bfd_discard_stat_t datatype from <=6.5.21 to
 *      SDK 6.5.22+
 * Parameters:
 *      from        - (IN) The <=6.5.21 version of the datatype
 *      to          - (OUT) The SDK 6.5.22+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521in_bfd_discard_stat_t (
    bcm_compat6521_bfd_discard_stat_t *from,
    bcm_bfd_discard_stat_t *to)
{
    sal_memset(to, 0, sizeof(bcm_bfd_discard_stat_t));
    to->bfd_ver_err = from->bfd_ver_err;
    to->bfd_len_err = from->bfd_len_err;
    to->bfd_detect_mult = from->bfd_detect_mult;
    to->bfd_my_disc = from->bfd_my_disc;
    to->bfd_p_f_bit = from->bfd_p_f_bit;
    to->bfd_m_bit = from->bfd_m_bit;
    to->bfd_auth_type_mismatch = from->bfd_auth_type_mismatch;
    to->bfd_auth_simple_err = from->bfd_auth_simple_err;
    to->bfd_auth_m_sha1_err = from->bfd_auth_m_sha1_err;
    to->bfd_sess_mismatch = from->bfd_sess_mismatch;
}

/*
 * Function:
 *      _bcm_compat6521out_bfd_discard_stat_t
 * Purpose:
 *      Convert the bcm_bfd_discard_stat_t datatype from SDK 6.5.22+ to
 *      <=6.5.21
 * Parameters:
 *      from        - (IN) The SDK 6.5.22+ version of the datatype
 *      to          - (OUT) The <=6.5.21 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521out_bfd_discard_stat_t (
    bcm_bfd_discard_stat_t *from,
    bcm_compat6521_bfd_discard_stat_t *to)
{
    to->bfd_ver_err = from->bfd_ver_err;
    to->bfd_len_err = from->bfd_len_err;
    to->bfd_detect_mult = from->bfd_detect_mult;
    to->bfd_my_disc = from->bfd_my_disc;
    to->bfd_p_f_bit = from->bfd_p_f_bit;
    to->bfd_m_bit = from->bfd_m_bit;
    to->bfd_auth_type_mismatch = from->bfd_auth_type_mismatch;
    to->bfd_auth_simple_err = from->bfd_auth_simple_err;
    to->bfd_auth_m_sha1_err = from->bfd_auth_m_sha1_err;
    to->bfd_sess_mismatch = from->bfd_sess_mismatch;
}

/*
 * Function:
 *      bcm_compat6521_bfd_discard_stat_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_bfd_discard_stat_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      discarded_info - (OUT)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_bfd_discard_stat_get (
    int unit,
    bcm_compat6521_bfd_discard_stat_t *discarded_info)
{
    int rv = BCM_E_NONE;
    bcm_bfd_discard_stat_t *new_discarded_info;

    if (discarded_info != NULL) {
        new_discarded_info = (bcm_bfd_discard_stat_t *)
                     sal_alloc(sizeof(bcm_bfd_discard_stat_t),
                     "New Discarded Info");
        if (new_discarded_info == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_bfd_discard_stat_get(unit, new_discarded_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_bfd_discard_stat_t(new_discarded_info, discarded_info);

    /* Deallocate memory*/
    sal_free(new_discarded_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_bfd_discard_stat_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_bfd_discard_stat_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      discarded_info - (OUT)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_bfd_discard_stat_set (
    int unit,
    bcm_compat6521_bfd_discard_stat_t *discarded_info)
{
    int rv = BCM_E_NONE;
    bcm_bfd_discard_stat_t *new_discarded_info;

    if (discarded_info != NULL) {
        new_discarded_info = (bcm_bfd_discard_stat_t *)
                     sal_alloc(sizeof(bcm_bfd_discard_stat_t),
                     "New Discarded Info");
        if (new_discarded_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_bfd_discard_stat_t(discarded_info, new_discarded_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_bfd_discard_stat_set(unit, new_discarded_info);

    if (rv >= 0) {
         /* Transform the entry from the new format to old one */
         _bcm_compat6521out_bfd_discard_stat_t(new_discarded_info, discarded_info);
    }

    /* Deallocate memory*/
    sal_free(new_discarded_info);

    return rv;
}
#endif /* defined(INCLUDE_BFD) */

/*
 * Function:
 *      _bcm_compat6521in_oam_endpoint_info_t
 * Purpose:
 *      Convert the bcm_oam_endpoint_info_t datatype from <=6.5.21 to
 *      SDK 6.5.22+
 * Parameters:
 *      from        - (IN) The <=6.5.21 version of the datatype
 *      to          - (OUT) The SDK 6.5.22+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521in_oam_endpoint_info_t(
    bcm_compat6521_oam_endpoint_info_t *from,
    bcm_oam_endpoint_info_t *to)
{
    int i1 = 0;

    bcm_oam_endpoint_info_t_init(to);
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->opcode_flags = from->opcode_flags;
    to->lm_flags = from->lm_flags;
    to->id = from->id;
    to->type = from->type;
    to->group = from->group;
    to->name = from->name;
    to->local_id = from->local_id;
    to->level = from->level;
    to->ccm_period = from->ccm_period;
    to->vlan = from->vlan;
    to->inner_vlan = from->inner_vlan;
    to->gport = from->gport;
    to->tx_gport = from->tx_gport;
    to->trunk_index = from->trunk_index;
    to->intf_id = from->intf_id;
    to->mpls_label = from->mpls_label;
    sal_memcpy(&to->egress_label, &from->egress_label, sizeof(bcm_mpls_egress_label_t));
    sal_memcpy(&to->mpls_network_info, &from->mpls_network_info, sizeof(bcm_oam_mpls_network_info_t));
    for (i1 = 0; i1 < 6; i1++) {
        to->dst_mac_address[i1] = from->dst_mac_address[i1];
    }
    for (i1 = 0; i1 < 6; i1++) {
        to->src_mac_address[i1] = from->src_mac_address[i1];
    }
    to->pkt_pri = from->pkt_pri;
    to->inner_pkt_pri = from->inner_pkt_pri;
    to->inner_tpid = from->inner_tpid;
    to->outer_tpid = from->outer_tpid;
    to->int_pri = from->int_pri;
    to->cpu_qid = from->cpu_qid;
    for (i1 = 0; i1 < BCM_OAM_INTPRI_MAX; i1++) {
        to->pri_map[i1] = from->pri_map[i1];
    }
    to->faults = from->faults;
    to->persistent_faults = from->persistent_faults;
    to->clear_persistent_faults = from->clear_persistent_faults;
    to->ing_map = from->ing_map;
    to->egr_map = from->egr_map;
    to->ms_pw_ttl = from->ms_pw_ttl;
    to->port_state = from->port_state;
    to->interface_state = from->interface_state;
    to->vccv_type = from->vccv_type;
    to->vpn = from->vpn;
    to->lm_counter_base_id = from->lm_counter_base_id;
    to->lm_counter_if = from->lm_counter_if;
    to->loc_clear_threshold = from->loc_clear_threshold;
    to->timestamp_format = from->timestamp_format;
    for (i1 = 0; i1 < BCM_OAM_MAX_NUM_TLVS_FOR_DM; i1++) {
        sal_memcpy(&to->tlvs[i1], &from->tlvs[i1], sizeof(bcm_oam_tlv_t));
    }
    to->subport_tpid = from->subport_tpid;
    to->remote_gport = from->remote_gport;
    to->mpls_out_gport = from->mpls_out_gport;
    to->sampling_ratio = from->sampling_ratio;
    to->lm_payload_offset = from->lm_payload_offset;
    to->lm_cos_offset = from->lm_cos_offset;
    to->lm_ctr_type = from->lm_ctr_type;
    to->lm_ctr_sample_size = from->lm_ctr_sample_size;
    to->pri_map_id = from->pri_map_id;
    to->lm_ctr_pool_id = from->lm_ctr_pool_id;
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->ccm_tx_update_lm_counter_base_id[i1] = from->ccm_tx_update_lm_counter_base_id[i1];
    }
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->ccm_tx_update_lm_counter_offset[i1] = from->ccm_tx_update_lm_counter_offset[i1];
    }
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->ccm_tx_update_lm_counter_action[i1] = from->ccm_tx_update_lm_counter_action[i1];
    }
    to->ccm_tx_update_lm_counter_size = from->ccm_tx_update_lm_counter_size;
    to->session_id = from->session_id;
    to->session_num_entries = from->session_num_entries;
    to->lm_count_profile = from->lm_count_profile;
    to->mpls_exp = from->mpls_exp;
    to->action_reference_id = from->action_reference_id;
    to->acc_profile_id = from->acc_profile_id;
    to->endpoint_memory_type = from->endpoint_memory_type;
    to->punt_good_packet_period = from->punt_good_packet_period;
    to->extra_data_index = from->extra_data_index;
    to->rx_signal = from->rx_signal;
    to->tx_signal = from->tx_signal;
}

/*
 * Function:
 *      _bcm_compat6521out_oam_endpoint_info_t
 * Purpose:
 *      Convert the bcm_oam_endpoint_info_t datatype from SDK 6.5.22+ to
 *      <=6.5.21
 * Parameters:
 *      from        - (IN) The SDK 6.5.22+ version of the datatype
 *      to          - (OUT) The <=6.5.21 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521out_oam_endpoint_info_t(
    bcm_oam_endpoint_info_t *from,
    bcm_compat6521_oam_endpoint_info_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->opcode_flags = from->opcode_flags;
    to->lm_flags = from->lm_flags;
    to->id = from->id;
    to->type = from->type;
    to->group = from->group;
    to->name = from->name;
    to->local_id = from->local_id;
    to->level = from->level;
    to->ccm_period = from->ccm_period;
    to->vlan = from->vlan;
    to->inner_vlan = from->inner_vlan;
    to->gport = from->gport;
    to->tx_gport = from->tx_gport;
    to->trunk_index = from->trunk_index;
    to->intf_id = from->intf_id;
    to->mpls_label = from->mpls_label;
    sal_memcpy(&to->egress_label, &from->egress_label, sizeof(bcm_mpls_egress_label_t));
    sal_memcpy(&to->mpls_network_info, &from->mpls_network_info, sizeof(bcm_oam_mpls_network_info_t));
    for (i1 = 0; i1 < 6; i1++) {
        to->dst_mac_address[i1] = from->dst_mac_address[i1];
    }
    for (i1 = 0; i1 < 6; i1++) {
        to->src_mac_address[i1] = from->src_mac_address[i1];
    }
    to->pkt_pri = from->pkt_pri;
    to->inner_pkt_pri = from->inner_pkt_pri;
    to->inner_tpid = from->inner_tpid;
    to->outer_tpid = from->outer_tpid;
    to->int_pri = from->int_pri;
    to->cpu_qid = from->cpu_qid;
    for (i1 = 0; i1 < BCM_OAM_INTPRI_MAX; i1++) {
        to->pri_map[i1] = from->pri_map[i1];
    }
    to->faults = from->faults;
    to->persistent_faults = from->persistent_faults;
    to->clear_persistent_faults = from->clear_persistent_faults;
    to->ing_map = from->ing_map;
    to->egr_map = from->egr_map;
    to->ms_pw_ttl = from->ms_pw_ttl;
    to->port_state = from->port_state;
    to->interface_state = from->interface_state;
    to->vccv_type = from->vccv_type;
    to->vpn = from->vpn;
    to->lm_counter_base_id = from->lm_counter_base_id;
    to->lm_counter_if = from->lm_counter_if;
    to->loc_clear_threshold = from->loc_clear_threshold;
    to->timestamp_format = from->timestamp_format;
    for (i1 = 0; i1 < BCM_OAM_MAX_NUM_TLVS_FOR_DM; i1++) {
        sal_memcpy(&to->tlvs[i1], &from->tlvs[i1], sizeof(bcm_oam_tlv_t));
    }
    to->subport_tpid = from->subport_tpid;
    to->remote_gport = from->remote_gport;
    to->mpls_out_gport = from->mpls_out_gport;
    to->sampling_ratio = from->sampling_ratio;
    to->lm_payload_offset = from->lm_payload_offset;
    to->lm_cos_offset = from->lm_cos_offset;
    to->lm_ctr_type = from->lm_ctr_type;
    to->lm_ctr_sample_size = from->lm_ctr_sample_size;
    to->pri_map_id = from->pri_map_id;
    to->lm_ctr_pool_id = from->lm_ctr_pool_id;
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->ccm_tx_update_lm_counter_base_id[i1] = from->ccm_tx_update_lm_counter_base_id[i1];
    }
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->ccm_tx_update_lm_counter_offset[i1] = from->ccm_tx_update_lm_counter_offset[i1];
    }
    for (i1 = 0; i1 < BCM_OAM_LM_COUNTER_MAX; i1++) {
        to->ccm_tx_update_lm_counter_action[i1] = from->ccm_tx_update_lm_counter_action[i1];
    }
    to->ccm_tx_update_lm_counter_size = from->ccm_tx_update_lm_counter_size;
    to->session_id = from->session_id;
    to->session_num_entries = from->session_num_entries;
    to->lm_count_profile = from->lm_count_profile;
    to->mpls_exp = from->mpls_exp;
    to->action_reference_id = from->action_reference_id;
    to->acc_profile_id = from->acc_profile_id;
    to->endpoint_memory_type = from->endpoint_memory_type;
    to->punt_good_packet_period = from->punt_good_packet_period;
    to->extra_data_index = from->extra_data_index;
    to->rx_signal = from->rx_signal;
    to->tx_signal = from->tx_signal;
}

/*
 * Function:
 *      bcm_compat6521_oam_endpoint_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_oam_endpoint_create.
 * Parameters:
 *      unit - (IN) BCM device number
 *      endpoint_info - (INOUT) (IN/OUT) Pointer to an OAM endpoint info structure
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_oam_endpoint_create(
    int unit,
    bcm_compat6521_oam_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_NONE;
    bcm_oam_endpoint_info_t *new_endpoint_info = NULL;

    if (endpoint_info != NULL) {
        new_endpoint_info = (bcm_oam_endpoint_info_t *)
                     sal_alloc(sizeof(bcm_oam_endpoint_info_t),
                     "New endpoint_info");
        if (new_endpoint_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_oam_endpoint_info_t(endpoint_info, new_endpoint_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_oam_endpoint_create(unit, new_endpoint_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_oam_endpoint_info_t(new_endpoint_info, endpoint_info);

    /* Deallocate memory*/
    sal_free(new_endpoint_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_oam_endpoint_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_oam_endpoint_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      endpoint - (IN) ID of the endpoint to get
 *      endpoint_info - (OUT) Pointer to an OAM endpoint info structure
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_oam_endpoint_get(
    int unit,
    bcm_oam_endpoint_t endpoint,
    bcm_compat6521_oam_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_NONE;
    bcm_oam_endpoint_info_t *new_endpoint_info = NULL;

    if (endpoint_info != NULL) {
        new_endpoint_info = (bcm_oam_endpoint_info_t *)
                     sal_alloc(sizeof(bcm_oam_endpoint_info_t),
                     "New endpoint_info");
        if (new_endpoint_info == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_oam_endpoint_get(unit, endpoint, new_endpoint_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_oam_endpoint_info_t(new_endpoint_info, endpoint_info);

    /* Deallocate memory*/
    sal_free(new_endpoint_info);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6521in_port_extender_mapping_info_t
 * Purpose:
 *      Convert the bcm_port_extender_mapping_info_t datatype from <=6.5.21 to
 *      SDK 6.5.22+
 * Parameters:
 *      from        - (IN) The <=6.5.21 version of the datatype
 *      to          - (OUT) The SDK 6.5.22+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521in_port_extender_mapping_info_t(
    bcm_compat6521_port_extender_mapping_info_t *from,
    bcm_port_extender_mapping_info_t *to)
{
    bcm_port_extender_mapping_info_t_init(to);
    to->pp_port = from->pp_port;
    to->tunnel_id = from->tunnel_id;
    to->phy_port = from->phy_port;
    to->vlan = from->vlan;
    to->user_define_value = from->user_define_value;
}

/*
 * Function:
 *      _bcm_compat6521out_port_extender_mapping_info_t
 * Purpose:
 *      Convert the bcm_port_extender_mapping_info_t datatype from SDK 6.5.22+ to
 *      <=6.5.21
 * Parameters:
 *      from        - (IN) The SDK 6.5.22+ version of the datatype
 *      to          - (OUT) The <=6.5.21 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521out_port_extender_mapping_info_t(
    bcm_port_extender_mapping_info_t *from,
    bcm_compat6521_port_extender_mapping_info_t *to)
{
    to->pp_port = from->pp_port;
    to->tunnel_id = from->tunnel_id;
    to->phy_port = from->phy_port;
    to->vlan = from->vlan;
    to->user_define_value = from->user_define_value;
}

/*
 * Function:
 *      bcm_compat6521_port_extender_mapping_info_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_extender_mapping_info_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) BCM_PORT_EXTENDER_MAPPING_XXXX
 *      type - (IN) type of mapping info
 *      mapping_info - (IN) (for "_set") mapping info (PON port+tunnel id/VLAN/user define value)<br>(INOUT) (for "_get") mapping info (PON port+tunnel id/VLAN/user define value)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_port_extender_mapping_info_set(
    int unit,
    uint32 flags,
    bcm_port_extender_mapping_type_t type,
    bcm_compat6521_port_extender_mapping_info_t *mapping_info)
{
    int rv = BCM_E_NONE;
    bcm_port_extender_mapping_info_t *new_mapping_info = NULL;

    if (mapping_info != NULL) {
        new_mapping_info = (bcm_port_extender_mapping_info_t *)
                     sal_alloc(sizeof(bcm_port_extender_mapping_info_t),
                     "New mapping_info");
        if (new_mapping_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_port_extender_mapping_info_t(mapping_info, new_mapping_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_port_extender_mapping_info_set(unit, flags, type, new_mapping_info);


    /* Deallocate memory*/
    sal_free(new_mapping_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_port_extender_mapping_info_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_extender_mapping_info_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) BCM_PORT_EXTENDER_MAPPING_XXXX
 *      type - (IN) type of mapping info
 *      mapping_info - (INOUT) (for "_set") mapping info (PON port+tunnel id/VLAN/user define value)<br>(INOUT) (for "_get") mapping info (PON port+tunnel id/VLAN/user define value)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_port_extender_mapping_info_get(
    int unit,
    uint32 flags,
    bcm_port_extender_mapping_type_t type,
    bcm_compat6521_port_extender_mapping_info_t *mapping_info)
{
    int rv = BCM_E_NONE;
    bcm_port_extender_mapping_info_t *new_mapping_info = NULL;

    if (mapping_info != NULL) {
        new_mapping_info = (bcm_port_extender_mapping_info_t *)
                     sal_alloc(sizeof(bcm_port_extender_mapping_info_t),
                     "New mapping_info");
        if (new_mapping_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_port_extender_mapping_info_t(mapping_info, new_mapping_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_port_extender_mapping_info_get(unit, flags, type, new_mapping_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_port_extender_mapping_info_t(new_mapping_info, mapping_info);

    /* Deallocate memory*/
    sal_free(new_mapping_info);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6521in_mirror_source_t
 * Purpose:
 *      Convert the bcm_mirror_source_t datatype from <=6.5.21 to
 *      SDK 6.5.22+
 * Parameters:
 *      from        - (IN) The <=6.5.21 version of the datatype
 *      to          - (OUT) The SDK 6.5.22+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521in_mirror_source_t(
    bcm_compat6521_mirror_source_t *from,
    bcm_mirror_source_t *to)
{
    bcm_mirror_source_t_init(to);
    to->type = from->type;
    to->flags = from->flags;
    to->port = from->port;
    to->trace_event = from->trace_event;
    to->drop_event = from->drop_event;
    to->mod_profile_id = from->mod_profile_id;
    to->sample_profile_id = from->sample_profile_id;
}

/*
 * Function:
 *      bcm_compat6521_mirror_source_dest_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_source_dest_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      source - (IN) Mirror source pointer
 *      mirror_dest_id - (IN) Mirror destination gport id
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_mirror_source_dest_add(
    int unit,
    bcm_compat6521_mirror_source_t *source,
    bcm_gport_t mirror_dest_id)
{
    int rv = BCM_E_NONE;
    bcm_mirror_source_t *new_source = NULL;

    if (source != NULL) {
        new_source = (bcm_mirror_source_t *)
                     sal_alloc(sizeof(bcm_mirror_source_t),
                     "New source");
        if (new_source == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_mirror_source_t(source, new_source);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_source_dest_add(unit, new_source, mirror_dest_id);


    /* Deallocate memory*/
    sal_free(new_source);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_mirror_source_dest_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_source_dest_delete.
 * Parameters:
 *      unit - (IN) BCM device number
 *      source - (IN) Mirror source pointer
 *      mirror_dest_id - (IN) Mirror destination gport id
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_mirror_source_dest_delete(
    int unit,
    bcm_compat6521_mirror_source_t *source,
    bcm_gport_t mirror_dest_id)
{
    int rv = BCM_E_NONE;
    bcm_mirror_source_t *new_source = NULL;

    if (source != NULL) {
        new_source = (bcm_mirror_source_t *)
                     sal_alloc(sizeof(bcm_mirror_source_t),
                     "New source");
        if (new_source == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_mirror_source_t(source, new_source);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_source_dest_delete(unit, new_source, mirror_dest_id);


    /* Deallocate memory*/
    sal_free(new_source);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_mirror_source_dest_delete_all
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_source_dest_delete_all.
 * Parameters:
 *      unit - (IN) Unit number.
 *      source - (IN) Mirror source pointer
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_mirror_source_dest_delete_all(
    int unit,
    bcm_compat6521_mirror_source_t *source)
{
    int rv = BCM_E_NONE;
    bcm_mirror_source_t *new_source = NULL;

    if (source != NULL) {
        new_source = (bcm_mirror_source_t *)
                     sal_alloc(sizeof(bcm_mirror_source_t),
                     "New source");
        if (new_source == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_mirror_source_t(source, new_source);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_source_dest_delete_all(unit, new_source);


    /* Deallocate memory*/
    sal_free(new_source);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_mirror_source_dest_get_all
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_source_dest_get_all.
 * Parameters:
 *      unit - (IN) Unit number.
 *      source - (IN) Mirror source pointer
 *      array_size - (IN) Size of allocated entries in mirror_dest array
 *      mirror_dest - (OUT) Array for mirror destinations information
 *      count - (OUT) Actual number of mirror destinations
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_mirror_source_dest_get_all(
    int unit,
    bcm_compat6521_mirror_source_t *source,
    int array_size,
    bcm_gport_t *mirror_dest,
    int *count)
{
    int rv = BCM_E_NONE;
    bcm_mirror_source_t *new_source = NULL;

    if (source != NULL) {
        new_source = (bcm_mirror_source_t *)
                     sal_alloc(sizeof(bcm_mirror_source_t),
                     "New source");
        if (new_source == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_mirror_source_t(source, new_source);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_source_dest_get_all(unit, new_source, array_size, mirror_dest, count);


    /* Deallocate memory*/
    sal_free(new_source);

    return rv;
}


#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6521in_vxlan_network_domain_config_t
 * Purpose:
 *      Convert the bcm_vxlan_network_domain_config_t datatype from <=6.5.21 to
 *      SDK 6.5.22+
 * Parameters:
 *      from        - (IN) The <=6.5.21 version of the datatype
 *      to          - (OUT) The SDK 6.5.22+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521in_vxlan_network_domain_config_t(
    bcm_compat6521_vxlan_network_domain_config_t *from,
    bcm_vxlan_network_domain_config_t *to)
{
    bcm_vxlan_network_domain_config_t_init(to);
    to->flags = from->flags;
    to->vsi = from->vsi;
    to->vrf = from->vrf;
    to->network_domain = from->network_domain;
    to->vni = from->vni;
    to->qos_map_id = from->qos_map_id;
    to->vlan_port_id = from->vlan_port_id;
}

/*
 * Function:
 *      _bcm_compat6521out_vxlan_network_domain_config_t
 * Purpose:
 *      Convert the bcm_vxlan_network_domain_config_t datatype from SDK 6.5.22+ to
 *      <=6.5.21
 * Parameters:
 *      from        - (IN) The SDK 6.5.22+ version of the datatype
 *      to          - (OUT) The <=6.5.21 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521out_vxlan_network_domain_config_t(
    bcm_vxlan_network_domain_config_t *from,
    bcm_compat6521_vxlan_network_domain_config_t *to)
{
    to->flags = from->flags;
    to->vsi = from->vsi;
    to->vrf = from->vrf;
    to->network_domain = from->network_domain;
    to->vni = from->vni;
    to->qos_map_id = from->qos_map_id;
    to->vlan_port_id = from->vlan_port_id;
}

/*
 * Function:
 *      bcm_compat6521_vxlan_network_domain_config_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_network_domain_config_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      config - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vxlan_network_domain_config_add(
    int unit,
    bcm_compat6521_vxlan_network_domain_config_t *config)
{
    int rv = BCM_E_NONE;
    bcm_vxlan_network_domain_config_t *new_config = NULL;

    if (config != NULL) {
        new_config = (bcm_vxlan_network_domain_config_t *)
                     sal_alloc(sizeof(bcm_vxlan_network_domain_config_t),
                     "New config");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vxlan_network_domain_config_t(config, new_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_network_domain_config_add(unit, new_config);


    /* Deallocate memory*/
    sal_free(new_config);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vxlan_network_domain_config_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_network_domain_config_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      config - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vxlan_network_domain_config_get(
    int unit,
    bcm_compat6521_vxlan_network_domain_config_t *config)
{
    int rv = BCM_E_NONE;
    bcm_vxlan_network_domain_config_t *new_config = NULL;

    if (config != NULL) {
        new_config = (bcm_vxlan_network_domain_config_t *)
                     sal_alloc(sizeof(bcm_vxlan_network_domain_config_t),
                     "New config");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vxlan_network_domain_config_t(config, new_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_network_domain_config_get(unit, new_config);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_vxlan_network_domain_config_t(new_config, config);

    /* Deallocate memory*/
    sal_free(new_config);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vxlan_network_domain_config_remove
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_network_domain_config_remove.
 * Parameters:
 *      unit - (IN) Unit number.
 *      config - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vxlan_network_domain_config_remove(
    int unit,
    bcm_compat6521_vxlan_network_domain_config_t *config)
{
    int rv = BCM_E_NONE;
    bcm_vxlan_network_domain_config_t *new_config = NULL;

    if (config != NULL) {
        new_config = (bcm_vxlan_network_domain_config_t *)
                     sal_alloc(sizeof(bcm_vxlan_network_domain_config_t),
                     "New config");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vxlan_network_domain_config_t(config, new_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_network_domain_config_remove(unit, new_config);


    /* Deallocate memory*/
    sal_free(new_config);

    return rv;
}


#endif /* defined(INCLUDE_L3) */
/*
 * Function:
 *      _bcm_compat6521in_mirror_pkt_dnx_pp_header_t
 * Purpose:
 *      Convert the bcm_mirror_pkt_dnx_pp_header_t datatype from <=6.5.21 to
 *      SDK 6.5.22+
 * Parameters:
 *      from        - (IN) The <=6.5.21 version of the datatype
 *      to          - (OUT) The SDK 6.5.22+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521in_mirror_pkt_dnx_pp_header_t(
    bcm_compat6521_mirror_pkt_dnx_pp_header_t *from,
    bcm_compat6522_mirror_pkt_dnx_pp_header_t *to)
{
    int i1 = 0;

    to->tail_edit_profile = from->tail_edit_profile;
    to->eth_header_remove = from->eth_header_remove;
    for (i1 = 0; i1 < 3; i1++) {
        to->out_vport_ext[i1] = from->out_vport_ext[i1];
    }
}

/*
 * Function:
 *      _bcm_compat6521out_mirror_pkt_dnx_pp_header_t
 * Purpose:
 *      Convert the bcm_mirror_pkt_dnx_pp_header_t datatype from SDK 6.5.22+ to
 *      <=6.5.21
 * Parameters:
 *      from        - (IN) The SDK 6.5.22+ version of the datatype
 *      to          - (OUT) The <=6.5.21 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521out_mirror_pkt_dnx_pp_header_t(
    bcm_compat6522_mirror_pkt_dnx_pp_header_t *from,
    bcm_compat6521_mirror_pkt_dnx_pp_header_t *to)
{
    int i1 = 0;

    to->tail_edit_profile = from->tail_edit_profile;
    to->eth_header_remove = from->eth_header_remove;
    for (i1 = 0; i1 < 3; i1++) {
        to->out_vport_ext[i1] = from->out_vport_ext[i1];
    }
}

/*
 * Function:
 *      _bcm_compat6521in_mirror_header_info_t
 * Purpose:
 *      Convert the bcm_mirror_header_info_t datatype from <=6.5.21 to
 *      SDK 6.5.22+
 * Parameters:
 *      from        - (IN) The <=6.5.21 version of the datatype
 *      to          - (OUT) The SDK 6.5.22+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521in_mirror_header_info_t(
    bcm_compat6521_mirror_header_info_t *from,
    bcm_compat6522_mirror_header_info_t *to)
{
    int i1 = 0;

    
    sal_memcpy(&to->tm, &from->tm, sizeof(bcm_mirror_pkt_dnx_ftmh_header_t));
    _bcm_compat6521in_mirror_pkt_dnx_pp_header_t(&from->pp, &to->pp);
    for (i1 = 0; i1 < 4; i1++) {
        sal_memcpy(&to->udh[i1], &from->udh[i1], sizeof(bcm_pkt_dnx_udh_t));
    }
}

/*
 * Function:
 *      _bcm_compat6521out_mirror_header_info_t
 * Purpose:
 *      Convert the bcm_mirror_header_info_t datatype from SDK 6.5.22+ to
 *      <=6.5.21
 * Parameters:
 *      from        - (IN) The SDK 6.5.22+ version of the datatype
 *      to          - (OUT) The <=6.5.21 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521out_mirror_header_info_t(
    bcm_compat6522_mirror_header_info_t *from,
    bcm_compat6521_mirror_header_info_t *to)
{
    int i1 = 0;

    sal_memcpy(&to->tm, &from->tm, sizeof(bcm_mirror_pkt_dnx_ftmh_header_t));
    _bcm_compat6521out_mirror_pkt_dnx_pp_header_t(&from->pp, &to->pp);
    for (i1 = 0; i1 < 4; i1++) {
        sal_memcpy(&to->udh[i1], &from->udh[i1], sizeof(bcm_pkt_dnx_udh_t));
    }
}

/*
 * Function:
 *      bcm_compat6521_mirror_header_info_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_header_info_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) BCM_MIRROR_DEST_* flags
 *      mirror_dest_id - (IN) (IN/OUT) Mirrored destination ID
 *      mirror_header_info - (IN) system header information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_mirror_header_info_set(
    int unit,
    uint32 flags,
    bcm_gport_t mirror_dest_id,
    bcm_compat6521_mirror_header_info_t *mirror_header_info)
{
    int rv = BCM_E_NONE;
    bcm_compat6522_mirror_header_info_t *new_mirror_header_info = NULL;

    if (mirror_header_info != NULL) {
        new_mirror_header_info = (bcm_compat6522_mirror_header_info_t *)
                     sal_alloc(sizeof(bcm_compat6522_mirror_header_info_t),
                     "New mirror_header_info");
        if (new_mirror_header_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_mirror_header_info_t(mirror_header_info, new_mirror_header_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6522_mirror_header_info_set(unit, flags, mirror_dest_id, new_mirror_header_info);


    /* Deallocate memory*/
    sal_free(new_mirror_header_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_mirror_header_info_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_header_info_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mirror_dest_id - (IN) Mirrored destination ID
 *      flags - (INOUT) (IN/OUT) BCM_MIRROR_DEST_* flags
 *      mirror_header_info - (OUT) system header information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_mirror_header_info_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    uint32 *flags,
    bcm_compat6521_mirror_header_info_t *mirror_header_info)
{
    int rv = BCM_E_NONE;
    bcm_compat6522_mirror_header_info_t *new_mirror_header_info = NULL;

    if (mirror_header_info != NULL) {
        new_mirror_header_info = (bcm_compat6522_mirror_header_info_t *)
                     sal_alloc(sizeof(bcm_compat6522_mirror_header_info_t),
                     "New mirror_header_info");
        if (new_mirror_header_info == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6522_mirror_header_info_get(unit, mirror_dest_id, flags, new_mirror_header_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_mirror_header_info_t(new_mirror_header_info, mirror_header_info);

    /* Deallocate memory*/
    sal_free(new_mirror_header_info);

    return rv;
}


/*
 * Function:
 *      _bcm_compat6521in_vlan_action_set_t
 * Purpose:
 *      Convert the bcm_vlan_action_set_t datatype from <=6.5.21 to
 *      SDK 6.5.22+
 * Parameters:
 *      from        - (IN) The <=6.5.21 version of the datatype
 *      to          - (OUT) The SDK 6.5.22+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521in_vlan_action_set_t(
    bcm_compat6521_vlan_action_set_t *from,
    bcm_vlan_action_set_t *to)
{
    bcm_vlan_action_set_t_init(to);
    to->new_outer_vlan = from->new_outer_vlan;
    to->new_inner_vlan = from->new_inner_vlan;
    to->new_inner_pkt_prio = from->new_inner_pkt_prio;
    to->new_outer_cfi = from->new_outer_cfi;
    to->new_inner_cfi = from->new_inner_cfi;
    to->ingress_if = from->ingress_if;
    to->priority = from->priority;
    to->dt_outer = from->dt_outer;
    to->dt_outer_prio = from->dt_outer_prio;
    to->dt_outer_pkt_prio = from->dt_outer_pkt_prio;
    to->dt_outer_cfi = from->dt_outer_cfi;
    to->dt_inner = from->dt_inner;
    to->dt_inner_prio = from->dt_inner_prio;
    to->dt_inner_pkt_prio = from->dt_inner_pkt_prio;
    to->dt_inner_cfi = from->dt_inner_cfi;
    to->ot_outer = from->ot_outer;
    to->ot_outer_prio = from->ot_outer_prio;
    to->ot_outer_pkt_prio = from->ot_outer_pkt_prio;
    to->ot_outer_cfi = from->ot_outer_cfi;
    to->ot_inner = from->ot_inner;
    to->ot_inner_pkt_prio = from->ot_inner_pkt_prio;
    to->ot_inner_cfi = from->ot_inner_cfi;
    to->it_outer = from->it_outer;
    to->it_outer_pkt_prio = from->it_outer_pkt_prio;
    to->it_outer_cfi = from->it_outer_cfi;
    to->it_inner = from->it_inner;
    to->it_inner_prio = from->it_inner_prio;
    to->it_inner_pkt_prio = from->it_inner_pkt_prio;
    to->it_inner_cfi = from->it_inner_cfi;
    to->ut_outer = from->ut_outer;
    to->ut_outer_pkt_prio = from->ut_outer_pkt_prio;
    to->ut_outer_cfi = from->ut_outer_cfi;
    to->ut_inner = from->ut_inner;
    to->ut_inner_pkt_prio = from->ut_inner_pkt_prio;
    to->ut_inner_cfi = from->ut_inner_cfi;
    to->outer_pcp = from->outer_pcp;
    to->inner_pcp = from->inner_pcp;
    to->policer_id = from->policer_id;
    to->outer_tpid = from->outer_tpid;
    to->inner_tpid = from->inner_tpid;
    to->outer_tpid_action = from->outer_tpid_action;
    to->inner_tpid_action = from->inner_tpid_action;
    to->action_id = from->action_id;
    to->class_id = from->class_id;
    to->taf_gate_primap = from->taf_gate_primap;
    to->flags = from->flags;
    to->outer_tag = from->outer_tag;
    to->inner_tag = from->inner_tag;
    to->forwarding_domain = from->forwarding_domain;
    to->inner_qos_map_id = from->inner_qos_map_id;
    to->outer_qos_map_id = from->outer_qos_map_id;
    to->opaque_ctrl_id = from->opaque_ctrl_id;
    to->opaque_tag_present = from->opaque_tag_present;
    to->opaque_tag_not_present = from->opaque_tag_not_present;
    to->gport = from->gport;
    to->gport_group = from->gport_group;
}

/*
 * Function:
 *      _bcm_compat6521in_l2_egress_t
 * Purpose:
 *      Convert the bcm_l2_egress_t datatype from <=6.5.21 to
 *      SDK 6.5.22+
 * Parameters:
 *      from        - (IN) The <=6.5.21 version of the datatype
 *      to          - (OUT) The SDK 6.5.22+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521in_l2_egress_t(
    bcm_compat6521_l2_egress_t *from,
    bcm_l2_egress_t *to)
{
    int i1 = 0;

    bcm_l2_egress_t_init(to);
    to->flags = from->flags;
    for (i1 = 0; i1 < 6; i1++) {
        to->dest_mac[i1] = from->dest_mac[i1];
    }
    for (i1 = 0; i1 < 6; i1++) {
        to->src_mac[i1] = from->src_mac[i1];
    }
    to->outer_vlan = from->outer_vlan;
    to->inner_vlan = from->inner_vlan;
    to->ethertype = from->ethertype;
    to->dest_port = from->dest_port;
    to->encap_id = from->encap_id;
    to->outer_tpid = from->outer_tpid;
    to->outer_prio = from->outer_prio;
    to->inner_tpid = from->inner_tpid;
    to->l3_intf = from->l3_intf;
    to->vlan_qos_map_id = from->vlan_qos_map_id;
    to->mpls_extended_label_value = from->mpls_extended_label_value;
    to->vlan_port_id = from->vlan_port_id;
    to->recycle_header_extension_length = from->recycle_header_extension_length;
    to->additional_egress_termination_size = from->additional_egress_termination_size;
    to->recycle_app = from->recycle_app;
    to->dest_encap_id = from->dest_encap_id;
    to->vrf = from->vrf;
}

/*
 * Function:
 *      _bcm_compat6521out_vlan_action_set_t
 * Purpose:
 *      Convert the bcm_vlan_action_set_t datatype from SDK 6.5.22+ to
 *      <=6.5.21
 * Parameters:
 *      from        - (IN) The SDK 6.5.22+ version of the datatype
 *      to          - (OUT) The <=6.5.21 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521out_vlan_action_set_t(
    bcm_vlan_action_set_t *from,
    bcm_compat6521_vlan_action_set_t *to)
{
    to->new_outer_vlan = from->new_outer_vlan;
    to->new_inner_vlan = from->new_inner_vlan;
    to->new_inner_pkt_prio = from->new_inner_pkt_prio;
    to->new_outer_cfi = from->new_outer_cfi;
    to->new_inner_cfi = from->new_inner_cfi;
    to->ingress_if = from->ingress_if;
    to->priority = from->priority;
    to->dt_outer = from->dt_outer;
    to->dt_outer_prio = from->dt_outer_prio;
    to->dt_outer_pkt_prio = from->dt_outer_pkt_prio;
    to->dt_outer_cfi = from->dt_outer_cfi;
    to->dt_inner = from->dt_inner;
    to->dt_inner_prio = from->dt_inner_prio;
    to->dt_inner_pkt_prio = from->dt_inner_pkt_prio;
    to->dt_inner_cfi = from->dt_inner_cfi;
    to->ot_outer = from->ot_outer;
    to->ot_outer_prio = from->ot_outer_prio;
    to->ot_outer_pkt_prio = from->ot_outer_pkt_prio;
    to->ot_outer_cfi = from->ot_outer_cfi;
    to->ot_inner = from->ot_inner;
    to->ot_inner_pkt_prio = from->ot_inner_pkt_prio;
    to->ot_inner_cfi = from->ot_inner_cfi;
    to->it_outer = from->it_outer;
    to->it_outer_pkt_prio = from->it_outer_pkt_prio;
    to->it_outer_cfi = from->it_outer_cfi;
    to->it_inner = from->it_inner;
    to->it_inner_prio = from->it_inner_prio;
    to->it_inner_pkt_prio = from->it_inner_pkt_prio;
    to->it_inner_cfi = from->it_inner_cfi;
    to->ut_outer = from->ut_outer;
    to->ut_outer_pkt_prio = from->ut_outer_pkt_prio;
    to->ut_outer_cfi = from->ut_outer_cfi;
    to->ut_inner = from->ut_inner;
    to->ut_inner_pkt_prio = from->ut_inner_pkt_prio;
    to->ut_inner_cfi = from->ut_inner_cfi;
    to->outer_pcp = from->outer_pcp;
    to->inner_pcp = from->inner_pcp;
    to->policer_id = from->policer_id;
    to->outer_tpid = from->outer_tpid;
    to->inner_tpid = from->inner_tpid;
    to->outer_tpid_action = from->outer_tpid_action;
    to->inner_tpid_action = from->inner_tpid_action;
    to->action_id = from->action_id;
    to->class_id = from->class_id;
    to->taf_gate_primap = from->taf_gate_primap;
    to->flags = from->flags;
    to->outer_tag = from->outer_tag;
    to->inner_tag = from->inner_tag;
    to->forwarding_domain = from->forwarding_domain;
    to->inner_qos_map_id = from->inner_qos_map_id;
    to->outer_qos_map_id = from->outer_qos_map_id;
    to->opaque_ctrl_id = from->opaque_ctrl_id;
    to->opaque_tag_present = from->opaque_tag_present;
    to->opaque_tag_not_present = from->opaque_tag_not_present;
    to->gport = from->gport;
    to->gport_group = from->gport_group;
}

/*
 * Function:
 *      _bcm_compat6521in_fcoe_vsan_action_set_t
 * Purpose:
 *      Convert the bcm_fcoe_vsan_action_set_t datatype from <=6.5.21 to
 *      SDK 6.5.22+
 * Parameters:
 *      from        - (IN) The <=6.5.21 version of the datatype
 *      to          - (OUT) The SDK 6.5.22+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521in_fcoe_vsan_action_set_t(
    bcm_compat6521_fcoe_vsan_action_set_t *from,
    bcm_fcoe_vsan_action_set_t *to)
{
    bcm_fcoe_vsan_action_set_t_init(to);
    to->flags = from->flags;
    to->new_vsan = from->new_vsan;
    to->new_vsan_pri = from->new_vsan_pri;
    sal_memcpy(&to->vft, &from->vft, sizeof(bcm_fcoe_vsan_vft_t));
    to->ingress_if = from->ingress_if;
    to->dt_vsan = from->dt_vsan;
    to->dt_vsan_pri = from->dt_vsan_pri;
    to->it_vsan = from->it_vsan;
    to->it_vsan_pri = from->it_vsan_pri;
    to->ot_vsan = from->ot_vsan;
    to->ot_vsan_pri = from->ot_vsan_pri;
    to->ut_vsan = from->ut_vsan;
    to->ut_vsan_pri = from->ut_vsan_pri;
    _bcm_compat6521in_vlan_action_set_t(&from->vlan_action, &to->vlan_action);
}

/*
 * Function:
 *      bcm_compat6521_fcoe_vsan_translate_action_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_fcoe_vsan_translate_action_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key - (IN) VSAN Translate key configuration
 *      action - (IN) VSAN and VLAN Translate action set
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_fcoe_vsan_translate_action_add(
    int unit,
    bcm_fcoe_vsan_translate_key_config_t *key,
    bcm_compat6521_fcoe_vsan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_fcoe_vsan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_fcoe_vsan_action_set_t *)
                     sal_alloc(sizeof(bcm_fcoe_vsan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_fcoe_vsan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_fcoe_vsan_translate_action_add(unit, key, new_action);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_field_action_vlan_actions_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_action_vlan_actions_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) Field Entry ID.
 *      action - (IN) Field Action value \ref bcm_field_action_t enumeration.
 *      vlan_action_set - (IN) VLAN Action Set \ref BCM_VLAN_ACTION_SET_t structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_field_action_vlan_actions_add(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_compat6521_vlan_action_set_t *vlan_action_set)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_vlan_action_set = NULL;

    if (vlan_action_set != NULL) {
        new_vlan_action_set = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New vlan_action_set");
        if (new_vlan_action_set == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vlan_action_set_t(vlan_action_set, new_vlan_action_set);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_action_vlan_actions_add(unit, entry, action, new_vlan_action_set);


    /* Deallocate memory*/
    sal_free(new_vlan_action_set);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6521out_l2_egress_t
 * Purpose:
 *      Convert the bcm_l2_egress_t datatype from SDK 6.5.22+ to
 *      <=6.5.21
 * Parameters:
 *      from        - (IN) The SDK 6.5.22+ version of the datatype
 *      to          - (OUT) The <=6.5.21 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521out_l2_egress_t(
    bcm_l2_egress_t *from,
    bcm_compat6521_l2_egress_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    for (i1 = 0; i1 < 6; i1++) {
        to->dest_mac[i1] = from->dest_mac[i1];
    }
    for (i1 = 0; i1 < 6; i1++) {
        to->src_mac[i1] = from->src_mac[i1];
    }
    to->outer_vlan = from->outer_vlan;
    to->inner_vlan = from->inner_vlan;
    to->ethertype = from->ethertype;
    to->dest_port = from->dest_port;
    to->encap_id = from->encap_id;
    to->outer_tpid = from->outer_tpid;
    to->outer_prio = from->outer_prio;
    to->inner_tpid = from->inner_tpid;
    to->l3_intf = from->l3_intf;
    to->vlan_qos_map_id = from->vlan_qos_map_id;
    to->mpls_extended_label_value = from->mpls_extended_label_value;
    to->vlan_port_id = from->vlan_port_id;
    to->recycle_header_extension_length = from->recycle_header_extension_length;
    to->additional_egress_termination_size = from->additional_egress_termination_size;
    to->recycle_app = from->recycle_app;
    to->dest_encap_id = from->dest_encap_id;
    to->vrf = from->vrf;
}

/*
 * Function:
 *      bcm_compat6521_field_action_vlan_actions_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_action_vlan_actions_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) Field Entry ID.
 *      action - (IN) Field Action Value.
 *      vlan_action_set - (IN) Reference to \ref BCM_VLAN_ACTION_SET_t structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_field_action_vlan_actions_get(
    int unit,
    bcm_field_entry_t entry,
    bcm_field_action_t action,
    bcm_compat6521_vlan_action_set_t *vlan_action_set)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_vlan_action_set = NULL;

    if (vlan_action_set != NULL) {
        new_vlan_action_set = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New vlan_action_set");
        if (new_vlan_action_set == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vlan_action_set_t(vlan_action_set, new_vlan_action_set);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_action_vlan_actions_get(unit, entry, action, new_vlan_action_set);


    /* Deallocate memory*/
    sal_free(new_vlan_action_set);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_port_default_action_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_port_default_action_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Device port number or logical device
 *      action - (OUT) (for "_set") Default VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t<br>(for "_get", OUT) Returns default VLAN tag action set
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_port_default_action_get(
    int unit,
    bcm_port_t port,
    bcm_compat6521_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_default_action_get(unit, port, new_action);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_vlan_action_set_t(new_action, action);

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_l2_egress_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_egress_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      egr - (INOUT) (IN/OUT) pointer to an L2 egress structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_l2_egress_create(
    int unit,
    bcm_compat6521_l2_egress_t *egr)
{
    int rv = BCM_E_NONE;
    bcm_l2_egress_t *new_egr = NULL;

    if (egr != NULL) {
        new_egr = (bcm_l2_egress_t *)
                     sal_alloc(sizeof(bcm_l2_egress_t),
                     "New egr");
        if (new_egr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_l2_egress_t(egr, new_egr);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l2_egress_create(unit, new_egr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_l2_egress_t(new_egr, egr);

    /* Deallocate memory*/
    sal_free(new_egr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_port_default_action_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_port_default_action_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Device port number or logical device
 *      action - (IN) (for "_set") Default VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t<br>(for "_get", OUT) Returns default VLAN tag action set
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_port_default_action_set(
    int unit,
    bcm_port_t port,
    bcm_compat6521_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_default_action_set(unit, port, new_action);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_port_egress_default_action_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_port_egress_default_action_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Device port number or logical device
 *      action - (OUT) (for "_set") Egress default VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t<br>(for "_get", OUT) Returns egress default VLAN tag action set
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_port_egress_default_action_get(
    int unit,
    bcm_port_t port,
    bcm_compat6521_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_egress_default_action_get(unit, port, new_action);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_vlan_action_set_t(new_action, action);

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_l2_egress_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_egress_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      encap_id - (IN) encap_id of the entry to which to get config for
 *      egr - (OUT) config of the entry
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_l2_egress_get(
    int unit,
    bcm_if_t encap_id,
    bcm_compat6521_l2_egress_t *egr)
{
    int rv = BCM_E_NONE;
    bcm_l2_egress_t *new_egr = NULL;

    if (egr != NULL) {
        new_egr = (bcm_l2_egress_t *)
                     sal_alloc(sizeof(bcm_l2_egress_t),
                     "New egr");
        if (new_egr == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_l2_egress_get(unit, encap_id, new_egr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_l2_egress_t(new_egr, egr);

    /* Deallocate memory*/
    sal_free(new_egr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_port_egress_default_action_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_port_egress_default_action_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Device port number or logical device
 *      action - (IN) (for "_set") Egress default VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t<br>(for "_get", OUT) Returns egress default VLAN tag action set
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_port_egress_default_action_set(
    int unit,
    bcm_port_t port,
    bcm_compat6521_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_egress_default_action_set(unit, port, new_action);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_port_protocol_action_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_port_protocol_action_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Device or logical port number
 *      frame - (IN) Frame type, as specified in \ref BCM_PORT_FRAMETYPE_e
 *      ether - (IN) 16-bit EtherType
 *      action - (IN) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_port_protocol_action_add(
    int unit,
    bcm_port_t port,
    bcm_port_frametype_t frame,
    bcm_port_ethertype_t ether,
    bcm_compat6521_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_protocol_action_add(unit, port, frame, ether, new_action);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_port_protocol_action_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_port_protocol_action_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Device or logical port number
 *      frame - (IN) Frame type, as specified in \ref BCM_PORT_FRAMETYPE_e
 *      ether - (IN) 16-bit EtherType
 *      action - (IN) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_port_protocol_action_get(
    int unit,
    bcm_port_t port,
    bcm_port_frametype_t frame,
    bcm_port_ethertype_t ether,
    bcm_compat6521_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_protocol_action_get(unit, port, frame, ether, new_action);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_mac_action_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_mac_action_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mac - (IN) IEEE 802.3 MAC address
 *      action - (IN) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_mac_action_add(
    int unit,
    bcm_mac_t mac,
    bcm_compat6521_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_mac_action_add(unit, mac, new_action);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_mac_action_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_mac_action_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mac - (IN) IEEE 802.3 MAC address
 *      action - (OUT) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_mac_action_get(
    int unit,
    bcm_mac_t mac,
    bcm_compat6521_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_mac_action_get(unit, mac, new_action);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_vlan_action_set_t(new_action, action);

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6521in_trunk_member_t
 * Purpose:
 *      Convert the bcm_trunk_member_t datatype from <=6.5.21 to
 *      SDK 6.5.22+
 * Parameters:
 *      from        - (IN) The <=6.5.21 version of the datatype
 *      to          - (OUT) The SDK 6.5.22+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521in_trunk_member_t(
    bcm_compat6521_trunk_member_t *from,
    bcm_trunk_member_t *to)
{
    bcm_trunk_member_t_init(to);
    to->flags = from->flags;
    to->gport = from->gport;
    to->dynamic_scaling_factor = from->dynamic_scaling_factor;
    to->dynamic_load_weight = from->dynamic_load_weight;
    to->dynamic_queue_size_weight = from->dynamic_queue_size_weight;
}

/*
 * Function:
 *      _bcm_compat6521out_trunk_member_t
 * Purpose:
 *      Convert the bcm_trunk_member_t datatype from SDK 6.5.22+ to
 *      <=6.5.21
 * Parameters:
 *      from        - (IN) The SDK 6.5.22+ version of the datatype
 *      to          - (OUT) The <=6.5.21 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521out_trunk_member_t(
    bcm_trunk_member_t *from,
    bcm_compat6521_trunk_member_t *to)
{
    to->flags = from->flags;
    to->gport = from->gport;
    to->dynamic_scaling_factor = from->dynamic_scaling_factor;
    to->dynamic_load_weight = from->dynamic_load_weight;
    to->dynamic_queue_size_weight = from->dynamic_queue_size_weight;
}

/*
 * Function:
 *      _bcm_compat6521in_trunk_info_t
 * Purpose:
 *      Convert the bcm_compat6521_trunk_info_t datatype from <=6.5.21 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.21 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521in_trunk_info_t( bcm_compat6521_trunk_info_t *from,
                                bcm_trunk_info_t *to)
{
    sal_memset(to, 0, sizeof (*to));
    to->flags = from->flags;
    to->psc = from->psc;
    to->ipmc_psc = from->ipmc_psc;
    to->dlf_index = from->dlf_index;
    to->mc_index = from->mc_index;
    to->ipmc_index = from->ipmc_index;
    to->dynamic_size = from->dynamic_size;
    to->dynamic_age = from->dynamic_age;
    to->dynamic_load_exponent = from->dynamic_load_exponent;
    to->dynamic_expected_load_exponent = from->dynamic_expected_load_exponent;
    to->master_tid = from->master_tid;
 }

/*
 * Function:
 *      _bcm_compat6521out_trunk_info_t
 * Purpose:
 *      Convert the bcm_trunk_info_t datatype from 6.5.21+ to
 *      <=SDK 6.5.21
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.21 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521out_trunk_info_t( bcm_trunk_info_t *from,
                                 bcm_compat6521_trunk_info_t *to)
{
    to->flags = from->flags;
    to->psc = from->psc;
    to->ipmc_psc = from->ipmc_psc;
    to->dlf_index = from->dlf_index;
    to->mc_index = from->mc_index;
    to->ipmc_index = from->ipmc_index;
    to->dynamic_size = from->dynamic_size;
    to->dynamic_age = from->dynamic_age;
    to->dynamic_load_exponent = from->dynamic_load_exponent;
    to->dynamic_expected_load_exponent = from->dynamic_expected_load_exponent;
    to->master_tid = from->master_tid;
 }

/*
 * Function:
 *      bcm_compat6521_trunk_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_trunk_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      tid - (IN) Trunk ID.
 *      trunk_info - (OUT)
 *      member_max - (IN) Maximum number of members in provided member_array.
 *      member_array - (OUT) Member list.
 *      member_count - (OUT) Number of members returned in member_array.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_trunk_get(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6521_trunk_info_t *trunk_info,
    int member_max,
    bcm_compat6521_trunk_member_t *member_array,
    int *member_count)
{
    int rv = BCM_E_NONE;
    bcm_trunk_member_t *new_member_array = NULL;
    bcm_trunk_info_t *new_info = NULL;
    int i = 0;

    if (member_array != NULL && member_max > 0) {
        new_member_array = (bcm_trunk_member_t *)
                     sal_alloc(member_max * sizeof(bcm_trunk_member_t),
                     "New member_array");
        if (new_member_array == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    if (NULL != trunk_info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_trunk_info_t *)
            sal_alloc(sizeof(bcm_trunk_info_t), "New trunk info");
        if (NULL == new_info) {
            if (NULL != new_member_array)
            {
                sal_free(new_member_array);
            }
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_trunk_info_t(trunk_info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_trunk_get(unit, tid, new_info, member_max, new_member_array, member_count);

    for (i = 0; i < member_max; i++) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6521out_trunk_member_t(&new_member_array[i], &member_array[i]);
    }

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6521out_trunk_info_t(new_info, trunk_info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    /* Deallocate memory*/
    sal_free(new_member_array);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_trunk_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_trunk_set.
 * Parameters:
 *      unit - (IN) BCM device number
 *      tid - (IN) Trunk ID
 *      trunk_info - (IN) Pointer to trunk info structure (see \ref bcm_trunk_info_t)
 *      member_count - (IN) Number of members in member_array.
 *      member_array - (IN) Member list.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_trunk_set(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6521_trunk_info_t *trunk_info,
    int member_count,
    bcm_compat6521_trunk_member_t *member_array)
{
    int rv = BCM_E_NONE;
    bcm_trunk_member_t *new_member_array = NULL;
    bcm_trunk_info_t *new_info = NULL;
    int i = 0;

    if (member_array != NULL && member_count > 0) {
        new_member_array = (bcm_trunk_member_t *)
                     sal_alloc(member_count * sizeof(bcm_trunk_member_t),
                     "New member_array");
        if (new_member_array == NULL) {
            return BCM_E_MEMORY;
        }
        for (i = 0; i < member_count; i++) {
            /* Transform the entry from the old format to new one */
            _bcm_compat6521in_trunk_member_t(&member_array[i], &new_member_array[i]);
        }
    } else {
        return BCM_E_PARAM;
    }

    if (NULL != trunk_info) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_trunk_info_t *)
            sal_alloc(sizeof(bcm_trunk_info_t), "New trunk info");
        if (NULL == new_info) {
            if (NULL != new_member_array)
            {
                sal_free(new_member_array);
            }
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_trunk_info_t(trunk_info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_trunk_set(unit, tid, new_info, member_count, new_member_array);

    if (NULL != new_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6521out_trunk_info_t(new_info, trunk_info);
        /* Deallocate memory*/
        sal_free(new_info);
    }

    /* Deallocate memory*/
    sal_free(new_member_array);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_trunk_member_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_trunk_member_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      tid - (IN) Trunk ID.
 *      member - (IN) Trunk member to add.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_trunk_member_add(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6521_trunk_member_t *member)
{
    int rv = BCM_E_NONE;
    bcm_trunk_member_t *new_member = NULL;

    if (member != NULL) {
        new_member = (bcm_trunk_member_t *)
                     sal_alloc(sizeof(bcm_trunk_member_t),
                     "New member");
        if (new_member == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_trunk_member_t(member, new_member);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_trunk_member_add(unit, tid, new_member);


    /* Deallocate memory*/
    sal_free(new_member);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_trunk_member_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_trunk_member_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      tid - (IN) Trunk ID.
 *      member - (IN) Trunk member to delete.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_trunk_member_delete(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6521_trunk_member_t *member)
{
    int rv = BCM_E_NONE;
    bcm_trunk_member_t *new_member = NULL;

    if (member != NULL) {
        new_member = (bcm_trunk_member_t *)
                     sal_alloc(sizeof(bcm_trunk_member_t),
                     "New member");
        if (new_member == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_trunk_member_t(member, new_member);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_trunk_member_delete(unit, tid, new_member);


    /* Deallocate memory*/
    sal_free(new_member);

    return rv;
}


/*
 * Function:
 *      bcm_compat6521_l2_egress_find
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_egress_find.
 * Parameters:
 *      unit - (IN) Unit number.
 *      egr - (IN) egress config to search for
 *      encap_id - (OUT) encap ID of the entry
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_l2_egress_find(
    int unit,
    bcm_compat6521_l2_egress_t *egr,
    bcm_if_t *encap_id)
{
    int rv = BCM_E_NONE;
    bcm_l2_egress_t *new_egr = NULL;

    if (egr != NULL) {
        new_egr = (bcm_l2_egress_t *)
                     sal_alloc(sizeof(bcm_l2_egress_t),
                     "New egr");
        if (new_egr == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_l2_egress_t(egr, new_egr);

        rv = bcm_l2_egress_find(unit, new_egr, encap_id);

        /* Deallocate memory*/
        sal_free(new_egr);

        return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_translate_action_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_action_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT (global port) identifier. Note: The port parameter is used to indicate port group when the VLAN translation key type
is any of bcmVlanTranslateKeyPortGroupDouble/bcmVlanTranslateKeyPortGroupOuter/bcmVlanTranslateKeyPortGroupInner/bcmVlanTranslateKeyPortGroupOuterTag/
bcmVlanTranslateKeyPortGroupInnerTag/bcmVlanTranslateKeyPortGroupOuterPri.<br>GPORT (global port) identifier
 *      key_type - (IN) VLAN translation key type<br>VLAN translation key type
 *      outer_vlan - (IN) Outer VLAN ID or tag<br>Outer VLAN ID or tag
 *      inner_vlan - (IN) Inner VLAN ID or tag<br>Inner VLAN ID or tag
 *      action - (IN) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t<br>(INOUT) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_translate_action_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6521_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_add(unit, port, key_type, outer_vlan, inner_vlan, new_action);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_translate_action_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_action_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT (global port) identifier. Note: The port parameter is used to indicate port group when the VLAN translation key type
is any of bcmVlanTranslateKeyPortGroupDouble/bcmVlanTranslateKeyPortGroupOuter/bcmVlanTranslateKeyPortGroupInner/bcmVlanTranslateKeyPortGroupOuterTag/
bcmVlanTranslateKeyPortGroupInnerTag/bcmVlanTranslateKeyPortGroupOuterPri.<br>GPORT (global port) identifier
 *      key_type - (IN) VLAN translation key type<br>VLAN translation key type
 *      outer_vlan - (IN) Outer VLAN ID or tag<br>Outer VLAN ID or tag
 *      inner_vlan - (IN) Inner VLAN ID or tag<br>Inner VLAN ID or tag
 *      action - (INOUT) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t<br>(INOUT) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_translate_action_create(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6521_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_create(unit, port, key_type, outer_vlan, inner_vlan, new_action);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_vlan_action_set_t(new_action, action);

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_translate_action_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_action_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT (global port) identifier. Note: The port parameter is used to indicate port group when the VLAN translation key type
is any of bcmVlanTranslateKeyPortGroupDouble/bcmVlanTranslateKeyPortGroupOuter/bcmVlanTranslateKeyPortGroupInner/bcmVlanTranslateKeyPortGroupOuterTag/
bcmVlanTranslateKeyPortGroupInnerTag/bcmVlanTranslateKeyPortGroupOuterPri.
 *      key_type - (IN) VLAN translation key type
 *      outer_vlan - (IN) Outer VLAN ID or tag
 *      inner_vlan - (IN) Inner VLAN ID or tag
 *      action - (INOUT) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_translate_action_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_translate_key_t key_type,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6521_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_get(unit, port, key_type, outer_vlan, inner_vlan, new_action);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_vlan_action_set_t(new_action, action);

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_translate_egress_action_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_action_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port_class - (IN) Port class
 *      outer_vlan - (IN) Outer VLAN ID
 *      inner_vlan - (IN) Inner VLAN ID
 *      action - (IN) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_translate_egress_action_add(
    int unit,
    int port_class,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6521_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_action_add(unit, port_class, outer_vlan, inner_vlan, new_action);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_translate_egress_action_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_action_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port_class - (IN) Port class
 *      outer_vlan - (IN) Outer VLAN ID
 *      inner_vlan - (IN) Inner VLAN ID
 *      action - (INOUT) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_translate_egress_action_get(
    int unit,
    int port_class,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6521_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_action_get(unit, port_class, outer_vlan, inner_vlan, new_action);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_vlan_action_set_t(new_action, action);

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_translate_egress_gport_action_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_gport_action_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Egress gport
 *      outer_vlan - (IN) Outer VLAN ID
 *      inner_vlan - (IN) Inner VLAN ID
 *      action - (IN) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_translate_egress_gport_action_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6521_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_gport_action_add(unit, port, outer_vlan, inner_vlan, new_action);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_translate_egress_gport_action_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_gport_action_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Egress gport
 *      outer_vlan - (IN) Outer VLAN ID
 *      inner_vlan - (IN) Inner VLAN ID
 *      action - (INOUT) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_translate_egress_gport_action_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan,
    bcm_vlan_t inner_vlan,
    bcm_compat6521_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_gport_action_get(unit, port, outer_vlan, inner_vlan, new_action);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_vlan_action_set_t(new_action, action);

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_translate_action_range_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_action_range_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT (global port) identifier
 *      outer_vlan_low - (IN) Low Outer VLAN ID
 *      outer_vlan_high - (IN) High Outer VLAN ID
 *      inner_vlan_low - (IN) Low Inner VLAN ID
 *      inner_vlan_high - (IN) High Inner VLAN ID
 *      action - (IN) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_translate_action_range_add(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan_low,
    bcm_vlan_t outer_vlan_high,
    bcm_vlan_t inner_vlan_low,
    bcm_vlan_t inner_vlan_high,
    bcm_compat6521_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_range_add(unit, port, outer_vlan_low, outer_vlan_high, inner_vlan_low, inner_vlan_high, new_action);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_translate_action_range_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_action_range_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT (global port) identifier
 *      outer_vlan_low - (IN) Low Outer VLAN ID
 *      outer_vlan_high - (IN) High Outer VLAN ID
 *      inner_vlan_low - (IN) Low Inner VLAN ID
 *      inner_vlan_high - (IN) High Inner VLAN ID
 *      action - (OUT) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_translate_action_range_get(
    int unit,
    bcm_gport_t port,
    bcm_vlan_t outer_vlan_low,
    bcm_vlan_t outer_vlan_high,
    bcm_vlan_t inner_vlan_low,
    bcm_vlan_t inner_vlan_high,
    bcm_compat6521_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_range_get(unit, port, outer_vlan_low, outer_vlan_high, inner_vlan_low, inner_vlan_high, new_action);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_vlan_action_set_t(new_action, action);

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_translate_action_id_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_action_id_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) Handling flags of type BCM_VLAN_ACTION_SET_XXX
 *      action_id - (IN) Action ID. Has to be created before it can be set.
 *      action - (IN) Pointer to a structure that holds tag actions per action ID.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_translate_action_id_set(
    int unit,
    uint32 flags,
    int action_id,
    bcm_compat6521_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_id_set(unit, flags, action_id, new_action);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_translate_action_id_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_action_id_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) Handling flags of type BCM_VLAN_ACTION_SET_XXX
 *      action_id - (IN) Action ID. Has to be created before get can be performed.
 *      action - (OUT) Pointer to a structure that holds tag actions per action ID.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_translate_action_id_get(
    int unit,
    uint32 flags,
    int action_id,
    bcm_compat6521_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_action_id_get(unit, flags, action_id, new_action);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_vlan_action_set_t(new_action, action);

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_ip_action_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_ip_action_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan_ip - (IN) A container for subnet-to-VLAN association data
 *      action - (IN) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_ip_action_add(
    int unit,
    bcm_vlan_ip_t *vlan_ip,
    bcm_compat6521_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vlan_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_ip_action_add(unit, vlan_ip, new_action);


    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_ip_action_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_ip_action_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan_ip - (IN) A container for subnet-to-VLAN association data
 *      action - (OUT) VLAN tag action set, as specified in \ref BCM_VLAN_ACTION_SET_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_ip_action_get(
    int unit,
    bcm_vlan_ip_t *vlan_ip,
    bcm_compat6521_vlan_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action = NULL;

    if (action != NULL) {
        new_action = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action");
        if (new_action == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_ip_action_get(unit, vlan_ip, new_action);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_vlan_action_set_t(new_action, action);

    /* Deallocate memory*/
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_match_action_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_match_action_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      options - (IN) Option from BCM_VLAN_MATCH_ACTION_XXX
 *      match_info - (IN) Match criteria info
 *      action_set - (IN) Action Set.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_match_action_add(
    int unit,
    uint32 options,
    bcm_compat6523_vlan_match_info_t *match_info,
    bcm_compat6521_vlan_action_set_t *action_set)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action_set = NULL;

    if (action_set != NULL) {
        new_action_set = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action_set");
        if (new_action_set == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vlan_action_set_t(action_set, new_action_set);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6523_vlan_match_action_add(unit, options, match_info, new_action_set);


    /* Deallocate memory*/
    sal_free(new_action_set);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_match_action_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_match_action_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      match_info - (IN) Match criteria info
 *      action_set - (OUT) Action Set.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_match_action_get(
    int unit,
    bcm_compat6523_vlan_match_info_t *match_info,
    bcm_compat6521_vlan_action_set_t *action_set)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action_set = NULL;

    if (action_set != NULL) {
        new_action_set = (bcm_vlan_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_action_set_t),
                     "New action_set");
        if (new_action_set == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6523_vlan_match_action_get(unit, match_info, new_action_set);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_vlan_action_set_t(new_action_set, action_set);

    /* Deallocate memory*/
    sal_free(new_action_set);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_match_action_multi_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_match_action_multi_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      match - (IN) Vlan match. See bcm_vlan_match_t
 *      size - (IN) Number of elements in match info array.
 *      match_info_array - (OUT) Match criteria info array.
 *      action_set_array - (OUT) Action set array
 *      count - (OUT) Valid Match criteria info count.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_match_action_multi_get(
    int unit,
    bcm_vlan_match_t match,
    int size,
    bcm_compat6523_vlan_match_info_t *match_info_array,
    bcm_compat6521_vlan_action_set_t *action_set_array,
    int *count)
{
    int rv = BCM_E_NONE;
    bcm_vlan_action_set_t *new_action_set_array = NULL;
    int i = 0;

    if (action_set_array != NULL && size > 0) {
        new_action_set_array = (bcm_vlan_action_set_t *)
                     sal_alloc(size * sizeof(bcm_vlan_action_set_t),
                     "New action_set_array");
        if (new_action_set_array == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6523_vlan_match_action_multi_get(unit, match, size, match_info_array, new_action_set_array, count);

    for (i = 0; i < size; i++) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6521out_vlan_action_set_t(&new_action_set_array[i], &action_set_array[i]);
    }

    /* Deallocate memory*/
    sal_free(new_action_set_array);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6521in_vlan_control_vlan_t
 * Purpose:
 *      Convert the bcm_vlan_control_vlan_t datatype from <=6.5.21 to
 *      SDK 6.5.22+
 * Parameters:
 *      from        - (IN) The <=6.5.21 version of the datatype
 *      to          - (OUT) The SDK 6.5.22+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521in_vlan_control_vlan_t(
    bcm_compat6521_vlan_control_vlan_t *from,
    bcm_vlan_control_vlan_t *to)
{
    bcm_vlan_control_vlan_t_init(to);
    to->vrf = from->vrf;
    to->forwarding_vlan = from->forwarding_vlan;
    to->ingress_if = from->ingress_if;
    to->outer_tpid = from->outer_tpid;
    to->flags = from->flags;
    to->ip6_mcast_flood_mode = from->ip6_mcast_flood_mode;
    to->ip4_mcast_flood_mode = from->ip4_mcast_flood_mode;
    to->l2_mcast_flood_mode = from->l2_mcast_flood_mode;
    to->if_class = from->if_class;
    to->forwarding_mode = from->forwarding_mode;
    to->urpf_mode = from->urpf_mode;
    to->cosq = from->cosq;
    to->qos_map_id = from->qos_map_id;
    to->distribution_class = from->distribution_class;
    to->broadcast_group = from->broadcast_group;
    to->unknown_multicast_group = from->unknown_multicast_group;
    to->unknown_unicast_group = from->unknown_unicast_group;
    to->trill_nonunicast_group = from->trill_nonunicast_group;
    to->source_trill_name = from->source_trill_name;
    to->trunk_index = from->trunk_index;
    sal_memcpy(&to->protocol_pkt, &from->protocol_pkt, sizeof(bcm_vlan_protocol_packet_ctrl_t));
    to->nat_realm_id = from->nat_realm_id;
    to->l3_if_class = from->l3_if_class;
    to->vp_mc_ctrl = from->vp_mc_ctrl;
    to->aging_cycles = from->aging_cycles;
    to->entropy_id = from->entropy_id;
    to->vpn = from->vpn;
    to->egress_vlan = from->egress_vlan;
    to->learn_flags = from->learn_flags;
    to->sr_flags = from->sr_flags;
    to->flags2 = from->flags2;
    to->ingress_stat_id = from->ingress_stat_id;
    to->ingress_stat_pp_profile = from->ingress_stat_pp_profile;
    to->egress_stat_id = from->egress_stat_id;
    to->egress_stat_pp_profile = from->egress_stat_pp_profile;
    _bcm_compat6521in_vlan_action_set_t(&from->egress_action, &to->egress_action);
    to->l2_view = from->l2_view;
    to->egress_class_id = from->egress_class_id;
    to->egress_opaque_ctrl_id = from->egress_opaque_ctrl_id;
    to->ingress_opaque_ctrl_id = from->ingress_opaque_ctrl_id;
    to->unknown_dest = from->unknown_dest;
    to->egress_translate_key = from->egress_translate_key;
}

/*
 * Function:
 *      _bcm_compat6521out_vlan_control_vlan_t
 * Purpose:
 *      Convert the bcm_vlan_control_vlan_t datatype from SDK 6.5.22+ to
 *      <=6.5.21
 * Parameters:
 *      from        - (IN) The SDK 6.5.22+ version of the datatype
 *      to          - (OUT) The <=6.5.21 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521out_vlan_control_vlan_t(
    bcm_vlan_control_vlan_t *from,
    bcm_compat6521_vlan_control_vlan_t *to)
{
    to->vrf = from->vrf;
    to->forwarding_vlan = from->forwarding_vlan;
    to->ingress_if = from->ingress_if;
    to->outer_tpid = from->outer_tpid;
    to->flags = from->flags;
    to->ip6_mcast_flood_mode = from->ip6_mcast_flood_mode;
    to->ip4_mcast_flood_mode = from->ip4_mcast_flood_mode;
    to->l2_mcast_flood_mode = from->l2_mcast_flood_mode;
    to->if_class = from->if_class;
    to->forwarding_mode = from->forwarding_mode;
    to->urpf_mode = from->urpf_mode;
    to->cosq = from->cosq;
    to->qos_map_id = from->qos_map_id;
    to->distribution_class = from->distribution_class;
    to->broadcast_group = from->broadcast_group;
    to->unknown_multicast_group = from->unknown_multicast_group;
    to->unknown_unicast_group = from->unknown_unicast_group;
    to->trill_nonunicast_group = from->trill_nonunicast_group;
    to->source_trill_name = from->source_trill_name;
    to->trunk_index = from->trunk_index;
    sal_memcpy(&to->protocol_pkt, &from->protocol_pkt, sizeof(bcm_vlan_protocol_packet_ctrl_t));
    to->nat_realm_id = from->nat_realm_id;
    to->l3_if_class = from->l3_if_class;
    to->vp_mc_ctrl = from->vp_mc_ctrl;
    to->aging_cycles = from->aging_cycles;
    to->entropy_id = from->entropy_id;
    to->vpn = from->vpn;
    to->egress_vlan = from->egress_vlan;
    to->learn_flags = from->learn_flags;
    to->sr_flags = from->sr_flags;
    to->flags2 = from->flags2;
    to->ingress_stat_id = from->ingress_stat_id;
    to->ingress_stat_pp_profile = from->ingress_stat_pp_profile;
    to->egress_stat_id = from->egress_stat_id;
    to->egress_stat_pp_profile = from->egress_stat_pp_profile;
    _bcm_compat6521out_vlan_action_set_t(&from->egress_action, &to->egress_action);
    to->l2_view = from->l2_view;
    to->egress_class_id = from->egress_class_id;
    to->egress_opaque_ctrl_id = from->egress_opaque_ctrl_id;
    to->ingress_opaque_ctrl_id = from->ingress_opaque_ctrl_id;
    to->unknown_dest = from->unknown_dest;
    to->egress_translate_key = from->egress_translate_key;
}

/*
 * Function:
 *      bcm_compat6521_vlan_control_vlan_selective_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_control_vlan_selective_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN/VPN
 *      valid_fields - (IN) Valid fields for VLAN control structure
 *      control - (OUT) (for "_set", IN) (for "_get", OUT) structure which contains VLAN property, see bcm_vlan_control_vlan_t \ref bcm_vlan_control_vlan_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_control_vlan_selective_get(
    int unit,
    bcm_vlan_t vlan,
    uint32 valid_fields,
    bcm_compat6521_vlan_control_vlan_t *control)
{
    int rv = BCM_E_NONE;
    bcm_vlan_control_vlan_t *new_control = NULL;

    if (control != NULL) {
        new_control = (bcm_vlan_control_vlan_t *)
                     sal_alloc(sizeof(bcm_vlan_control_vlan_t),
                     "New control");
        if (new_control == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_control_vlan_selective_get(unit, vlan, valid_fields, new_control);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_vlan_control_vlan_t(new_control, control);

    /* Deallocate memory*/
    sal_free(new_control);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_control_vlan_selective_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_control_vlan_selective_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN/VPN
 *      valid_fields - (IN) Valid fields for VLAN control structure
 *      control - (IN) (for "_set", IN) (for "_get", OUT) structure which contains VLAN property, see bcm_vlan_control_vlan_t \ref bcm_vlan_control_vlan_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_control_vlan_selective_set(
    int unit,
    bcm_vlan_t vlan,
    uint32 valid_fields,
    bcm_compat6521_vlan_control_vlan_t *control)
{
    int rv = BCM_E_NONE;
    bcm_vlan_control_vlan_t *new_control = NULL;

    if (control != NULL) {
        new_control = (bcm_vlan_control_vlan_t *)
                     sal_alloc(sizeof(bcm_vlan_control_vlan_t),
                     "New control");
        if (new_control == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vlan_control_vlan_t(control, new_control);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_control_vlan_selective_set(unit, vlan, valid_fields, new_control);


    /* Deallocate memory*/
    sal_free(new_control);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_control_vlan_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_control_vlan_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN
 *      control - (OUT) (for "_set", IN) (for "_get", OUT) structure which contains VLAN property, see bcm_vlan_control_vlan_t \ref bcm_vlan_control_vlan_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_control_vlan_get(
    int unit,
    bcm_vlan_t vlan,
    bcm_compat6521_vlan_control_vlan_t *control)
{
    int rv = BCM_E_NONE;
    bcm_vlan_control_vlan_t *new_control = NULL;

    if (control != NULL) {
        new_control = (bcm_vlan_control_vlan_t *)
                     sal_alloc(sizeof(bcm_vlan_control_vlan_t),
                     "New control");
        if (new_control == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_control_vlan_get(unit, vlan, new_control);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_vlan_control_vlan_t(new_control, control);

    /* Deallocate memory*/
    sal_free(new_control);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_control_vlan_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_control_vlan_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan - (IN) VLAN
 *      control - (IN) (for "_set", IN) (for "_get", OUT) structure which contains VLAN property, see bcm_vlan_control_vlan_t \ref bcm_vlan_control_vlan_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_control_vlan_set(
    int unit,
    bcm_vlan_t vlan,
    /*
     * COVERITY
     *
     * Passing value instead of reference is kept intentionally as
     * the API was there for a fairly long time.
     */
    /* coverity[pass_by_value] */
    bcm_compat6521_vlan_control_vlan_t control)
{
    int rv = BCM_E_NONE;
    bcm_vlan_control_vlan_t new_control;

    /* Transform the entry from the old format to new one */
    _bcm_compat6521in_vlan_control_vlan_t(&control, &new_control);
    /* Call the BCM API with new format */
    rv = bcm_vlan_control_vlan_set(unit, vlan, new_control);


    return rv;
}

/*
 * Function:
 *      _bcm_compat6521in_vlan_translate_egress_action_set_t
 * Purpose:
 *      Convert the bcm_vlan_translate_egress_action_set_t datatype from <=6.5.21 to
 *      SDK 6.5.22+
 * Parameters:
 *      from        - (IN) The <=6.5.21 version of the datatype
 *      to          - (OUT) The SDK 6.5.22+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521in_vlan_translate_egress_action_set_t(
    bcm_compat6521_vlan_translate_egress_action_set_t *from,
    bcm_vlan_translate_egress_action_set_t *to)
{
    bcm_vlan_translate_egress_action_set_t_init(to);
    to->flags = from->flags;
    _bcm_compat6521in_vlan_action_set_t(&from->vlan_action, &to->vlan_action);
}

/*
 * Function:
 *      _bcm_compat6521out_vlan_translate_egress_action_set_t
 * Purpose:
 *      Convert the bcm_vlan_translate_egress_action_set_t datatype from SDK 6.5.22+ to
 *      <=6.5.21
 * Parameters:
 *      from        - (IN) The SDK 6.5.22+ version of the datatype
 *      to          - (OUT) The <=6.5.21 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521out_vlan_translate_egress_action_set_t(
    bcm_vlan_translate_egress_action_set_t *from,
    bcm_compat6521_vlan_translate_egress_action_set_t *to)
{
    to->flags = from->flags;
    _bcm_compat6521out_vlan_action_set_t(&from->vlan_action, &to->vlan_action);
}

/*
 * Function:
 *      _bcm_compat6521in_vlan_translate_egress_key_config_t
 * Purpose:
 *      Convert the bcm_vlan_translate_egress_key_config_t datatype from <=6.5.21 to
 *      SDK 6.5.22+
 * Parameters:
 *      from        - (IN) The <=6.5.21 version of the datatype
 *      to          - (OUT) The SDK 6.5.22+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521in_vlan_translate_egress_key_config_t(
    bcm_compat6521_vlan_translate_egress_key_config_t *from,
    bcm_compat6522_vlan_translate_egress_key_config_t *to)
{
    
    to->flags = from->flags;
    to->key_type = from->key_type;
    to->inner_vlan = from->inner_vlan;
    to->outer_vlan = from->outer_vlan;
    to->port = from->port;
}

/*
 * Function:
 *      bcm_compat6521_vlan_translate_egress_action_extended_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_action_extended_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN) 
 *      action - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_translate_egress_action_extended_add(
    int unit,
    bcm_compat6521_vlan_translate_egress_key_config_t *key_config,
    bcm_compat6521_vlan_translate_egress_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_compat6522_vlan_translate_egress_key_config_t *new_key_config = NULL;
    bcm_vlan_translate_egress_action_set_t *new_action = NULL;

    if (key_config != NULL && action != NULL) {
        new_key_config = (bcm_compat6522_vlan_translate_egress_key_config_t *)
                     sal_alloc(sizeof(bcm_compat6522_vlan_translate_egress_key_config_t),
                     "New key_config");
        if (new_key_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vlan_translate_egress_key_config_t(key_config, new_key_config);

        new_action = (bcm_vlan_translate_egress_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_translate_egress_action_set_t),
                     "New action");
        if (new_action == NULL) {
            sal_free(new_key_config);
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vlan_translate_egress_action_set_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6522_vlan_translate_egress_action_extended_add(unit, new_key_config, new_action);


    /* Deallocate memory*/
    sal_free(new_key_config);
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_translate_egress_action_extended_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_action_extended_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN) 
 *      action - (OUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_translate_egress_action_extended_get(
    int unit,
    bcm_compat6521_vlan_translate_egress_key_config_t *key_config,
    bcm_compat6521_vlan_translate_egress_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_compat6522_vlan_translate_egress_key_config_t *new_key_config = NULL;
    bcm_vlan_translate_egress_action_set_t *new_action = NULL;

    if (key_config != NULL && action != NULL) {
        new_key_config = (bcm_compat6522_vlan_translate_egress_key_config_t *)
                     sal_alloc(sizeof(bcm_compat6522_vlan_translate_egress_key_config_t),
                     "New key_config");
        if (new_key_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vlan_translate_egress_key_config_t(key_config, new_key_config);

        new_action = (bcm_vlan_translate_egress_action_set_t *)
                     sal_alloc(sizeof(bcm_vlan_translate_egress_action_set_t),
                     "New action");
        if (new_action == NULL) {
            sal_free(new_key_config);
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6522_vlan_translate_egress_action_extended_get(unit, new_key_config, new_action);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_vlan_translate_egress_action_set_t(new_action, action);

    /* Deallocate memory*/
    sal_free(new_key_config);
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_vlan_translate_egress_action_extended_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_action_extended_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_vlan_translate_egress_action_extended_delete(
    int unit,
    bcm_compat6521_vlan_translate_egress_key_config_t *key_config)
{
    int rv = BCM_E_NONE;
    bcm_compat6522_vlan_translate_egress_key_config_t *new_key_config = NULL;

    if (key_config != NULL) {
        new_key_config = (bcm_compat6522_vlan_translate_egress_key_config_t *)
                     sal_alloc(sizeof(bcm_compat6522_vlan_translate_egress_key_config_t),
                     "New key_config");
        if (new_key_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_vlan_translate_egress_key_config_t(key_config, new_key_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6522_vlan_translate_egress_action_extended_delete(unit, new_key_config);


    /* Deallocate memory*/
    sal_free(new_key_config);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6521out_sat_ctf_stat_t
 * Purpose:
 *      Convert the bcm_sat_ctf_stat_t datatype from SDK 6.5.22+ to
 *      <=6.5.21
 * Parameters:
 *      from        - (IN) The SDK 6.5.22+ version of the datatype
 *      to          - (OUT) The <=6.5.21 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521out_sat_ctf_stat_t(
    bcm_sat_ctf_stat_t *from,
    bcm_compat6521_sat_ctf_stat_t *to)
{
    int i1 = 0;

    to->received_packet_cnt = from->received_packet_cnt;
    to->out_of_order_packet_cnt = from->out_of_order_packet_cnt;
    to->err_packet_cnt = from->err_packet_cnt;
    to->received_octet_cnt = from->received_octet_cnt;
    to->out_of_order_octet_cnt = from->out_of_order_octet_cnt;
    to->err_octet_cnt = from->err_octet_cnt;
    to->last_packet_delay = from->last_packet_delay;
    for (i1 = 0; i1 < BCM_SAT_CTF_STAT_MAX_NUM_OF_BINS; i1++) {
        to->bin_delay_counters[i1] = from->bin_delay_counters[i1];
    }
    to->accumulated_delay_count = from->accumulated_delay_count;
    to->max_packet_delay = from->max_packet_delay;
    to->min_packet_delay = from->min_packet_delay;
    to->time_of_day_stamp = from->time_of_day_stamp;
    to->sec_in_avail_state_counter = from->sec_in_avail_state_counter;
    to->last_packet_seq_number = from->last_packet_seq_number;
}

/*
 * Function:
 *      bcm_compat6521_sat_ctf_stat_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_sat_ctf_stat_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      ctf_id - (IN) CTF ID
 *      flags - (IN) BCM_SAT_CTF_STAT_DO_NOT_CLR_ON_READ: do not clear counter when reading
 *      stat - (OUT) Pointer to SAT ctf stat structure
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_sat_ctf_stat_get(
    int unit,
    bcm_sat_ctf_t ctf_id,
    uint32 flags,
    bcm_compat6521_sat_ctf_stat_t *stat)
{
    int rv = BCM_E_NONE;
    bcm_sat_ctf_stat_t *new_stat = NULL;

    if (stat != NULL) {
        new_stat = (bcm_sat_ctf_stat_t *)
                     sal_alloc(sizeof(bcm_sat_ctf_stat_t),
                     "New stat");
        if (new_stat == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_sat_ctf_stat_get(unit, ctf_id, flags, new_stat);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_sat_ctf_stat_t(new_stat, stat);

    /* Deallocate memory*/
    sal_free(new_stat);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6521in_switch_pkt_trace_info_t
 * Purpose:
 *      Convert the bcm_switch_pkt_trace_info_t datatype from <=6.5.21 to
 *      SDK 6.5.22+
 * Parameters:
 *      from        - (IN) The <=6.5.21 version of the datatype
 *      to          - (OUT) The SDK 6.5.22+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521in_switch_pkt_trace_info_t(
    bcm_compat6521_switch_pkt_trace_info_t *from,
    bcm_switch_pkt_trace_info_t *to)
{
    int i1 = 0;

    sal_memcpy(&to->pkt_trace_lookup_status,
               &from->pkt_trace_lookup_status,
               sizeof(bcm_switch_pkt_trace_lookup_result_t));
    to->pkt_trace_resolution = from->pkt_trace_resolution;
    sal_memcpy(&to->pkt_trace_hash_info,
               &from->pkt_trace_hash_info,
               sizeof(bcm_switch_pkt_trace_hashing_info_t));
    to->pkt_trace_stp_state = from->pkt_trace_stp_state;
    for (i1 = 0; i1 < BCM_SWITCH_PKT_TRACE_DROP_REASON_MAX; i1++) {
        to->pkt_trace_drop_reason[i1] = from->pkt_trace_drop_reason[i1];
    }
    to->dest_pipe_num = from->dest_pipe_num;
    to->raw_data_length = from->raw_data_length;
    for (i1 = 0; i1 < BCM_SWITCH_PKT_TRACE_RAW_DATA_MAX; i1++) {
        to->raw_data[i1] = from->raw_data[i1];
    }
    sal_memcpy(&to->pkt_trace_fwd_dst_info,
               &from->pkt_trace_fwd_dst_info,
               sizeof(bcm_switch_pkt_trace_fwd_dst_info_t));
}

/*
 * Function:
 *      _bcm_compat6521out_switch_pkt_trace_info_t
 * Purpose:
 *      Convert the bcm_switch_pkt_trace_info_t datatype from SDK 6.5.22+ to
 *      <=6.5.21
 * Parameters:
 *      from        - (IN) The SDK 6.5.22+ version of the datatype
 *      to          - (OUT) The <=6.5.21 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521out_switch_pkt_trace_info_t(
    bcm_switch_pkt_trace_info_t *from,
    bcm_compat6521_switch_pkt_trace_info_t *to)
{
    int i1 = 0;

    sal_memcpy(&to->pkt_trace_lookup_status,
               &from->pkt_trace_lookup_status,
               sizeof(bcm_switch_pkt_trace_lookup_result_t));
    to->pkt_trace_resolution = from->pkt_trace_resolution;
    sal_memcpy(&to->pkt_trace_hash_info,
               &from->pkt_trace_hash_info,
               sizeof(bcm_switch_pkt_trace_hashing_info_t));
    to->pkt_trace_stp_state = from->pkt_trace_stp_state;
    for (i1 = 0; i1 < BCM_SWITCH_PKT_TRACE_DROP_REASON_MAX; i1++) {
        to->pkt_trace_drop_reason[i1] = from->pkt_trace_drop_reason[i1];
    }
    to->dest_pipe_num = from->dest_pipe_num;
    to->raw_data_length = from->raw_data_length;
    for (i1 = 0; i1 < BCM_SWITCH_PKT_TRACE_RAW_DATA_MAX; i1++) {
        to->raw_data[i1] = from->raw_data[i1];
    }
    sal_memcpy(&to->pkt_trace_fwd_dst_info,
               &from->pkt_trace_fwd_dst_info,
               sizeof(bcm_switch_pkt_trace_fwd_dst_info_t));
}

/*
 * Function:
 *      bcm_compat6521_switch_pkt_trace_info_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_switch_pkt_trace_info_get.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      options        - (IN) FLAGS to select pre-configured
 *                            cpu_pkt_profile register values
 *                            and to provide additional instructions to the SDK
 *      port           - (IN)
 *      len            - (IN) Number of bytes to copy from data
 *      data           - (IN) Source packet buffer to copy from
 *      pkt_trace_info - (INOUT)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_switch_pkt_trace_info_get(
    int unit,
    uint32 options,
    uint8 port,
    int len,
    uint8 *data,
    bcm_compat6521_switch_pkt_trace_info_t *pkt_trace_info)
{
    int rv = BCM_E_NONE;
    bcm_switch_pkt_trace_info_t *new_pkt_trace_info = NULL;

    if (pkt_trace_info != NULL) {
        new_pkt_trace_info = (bcm_switch_pkt_trace_info_t *)
                     sal_alloc(sizeof(bcm_switch_pkt_trace_info_t),
                     "New pkt_trace_info");
        if (new_pkt_trace_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_switch_pkt_trace_info_t(
                         pkt_trace_info,
                         new_pkt_trace_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_switch_pkt_trace_info_get(unit,
                                       options,
                                       port,
                                       len,
                                       data,
                                       new_pkt_trace_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_switch_pkt_trace_info_t(
                           new_pkt_trace_info,
                           pkt_trace_info);

    /* Deallocate memory*/
    sal_free(new_pkt_trace_info);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6521in_switch_reflector_data_t
 * Purpose:
 *      Convert the bcm_switch_reflector_data_t datatype from <=6.5.21 to
 *      SDK 6.5.22+
 * Parameters:
 *      from        - (IN) The <=6.5.21 version of the datatype
 *      to          - (OUT) The SDK 6.5.22+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521in_switch_reflector_data_t(
    bcm_compat6521_switch_reflector_data_t *from,
    bcm_switch_reflector_data_t *to)
{
    int i1 = 0;

    bcm_switch_reflector_data_t_init(to);
    to->type = from->type;
    for (i1 = 0; i1 < 6; i1++) {
        to->mc_reflector_my_mac[i1] = from->mc_reflector_my_mac[i1];
    }
    to->error_estimate = from->error_estimate;
    to->next_encap_id = from->next_encap_id;
}

/*
 * Function:
 *      _bcm_compat6521out_switch_reflector_data_t
 * Purpose:
 *      Convert the bcm_switch_reflector_data_t datatype from SDK 6.5.22+ to
 *      <=6.5.21
 * Parameters:
 *      from        - (IN) The SDK 6.5.22+ version of the datatype
 *      to          - (OUT) The <=6.5.21 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6521out_switch_reflector_data_t(
    bcm_switch_reflector_data_t *from,
    bcm_compat6521_switch_reflector_data_t *to)
{
    int i1 = 0;

    to->type = from->type;
    for (i1 = 0; i1 < 6; i1++) {
        to->mc_reflector_my_mac[i1] = from->mc_reflector_my_mac[i1];
    }
    to->error_estimate = from->error_estimate;
    to->next_encap_id = from->next_encap_id;
}

/*
 * Function:
 *      bcm_compat6521_switch_reflector_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_switch_reflector_create.
 * Parameters:
 *      unit - (IN) BCM device number
 *      flags - (IN) see BCM_SWITCH_REFLECTOR_X
 *      encap_id - (INOUT) the allocated destroyed encap ID
 *      data - (INOUT) data used for the allocated encap ID
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_switch_reflector_create(
    int unit,
    uint32 flags,
    bcm_if_t *encap_id,
    bcm_compat6521_switch_reflector_data_t *data)
{
    int rv = BCM_E_NONE;
    bcm_switch_reflector_data_t *new_data = NULL;

    if (data != NULL) {
        new_data = (bcm_switch_reflector_data_t *)
                     sal_alloc(sizeof(bcm_switch_reflector_data_t),
                     "New data");
        if (new_data == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_switch_reflector_data_t(data, new_data);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_switch_reflector_create(unit, flags, encap_id, new_data);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_switch_reflector_data_t(new_data, data);

    /* Deallocate memory*/
    sal_free(new_data);

    return rv;
}

/*
 * Function:
 *      bcm_compat6521_switch_reflector_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_switch_reflector_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) see BCM_SWITCH_REFLECTOR_X
 *      encap_id - (IN) the allocated destroyed encap ID
 *      data - (INOUT) data used for the allocated encap ID
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6521_switch_reflector_get(
    int unit,
    uint32 flags,
    bcm_if_t encap_id,
    bcm_compat6521_switch_reflector_data_t *data)
{
    int rv = BCM_E_NONE;
    bcm_switch_reflector_data_t *new_data = NULL;

    if (data != NULL) {
        new_data = (bcm_switch_reflector_data_t *)
                     sal_alloc(sizeof(bcm_switch_reflector_data_t),
                     "New data");
        if (new_data == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_switch_reflector_data_t(data, new_data);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_switch_reflector_get(unit, flags, encap_id, new_data);

    /* Transform the entry from the new format to old one */
    _bcm_compat6521out_switch_reflector_data_t(new_data, data);

    /* Deallocate memory*/
    sal_free(new_data);

    return rv;
}

/*
 * Function:
 *   _bcm_compat6521in_field_entry_config_t
 * Purpose:
 *   Convert the bcm_field_entry_config_t datatype from <=6.5.21 to
 *   SDK 6.5.21+.
 * Parameters:
 *   from        - (IN) The <=6.5.21 version of the datatype.
 *   to          - (OUT) The SDK 6.5.21+ version of the datatype.
 * Returns:
 *   Nothing
 */

STATIC void
_bcm_compat6521in_field_entry_config_t(
    bcm_compat6521_field_entry_config_t *from,
    bcm_field_entry_config_t *to)
{
    int idx = 0;
    bcm_field_entry_config_t_init(to);
    BCM_FIELD_ASET_INIT(to->aset);
    for (idx = 0; idx < bcmFieldActionCount; idx++) {
        if (BCM_FIELD_ASET_TEST(from->aset, idx)) {
            BCM_FIELD_ASET_ADD(to->aset, idx);
        }
    }
       to->flags = from->flags;
       to->entry_id = from->entry_id;
    to->group_id = from->group_id;
    to->priority = from->priority;
    to->status_flags = from->status_flags;
    to->flex_counter_action_id = from->flex_counter_action_id;
    to->counter_id = from->counter_id;
    to->gp_counter_id = from->gp_counter_id;
    to->yp_counter_id = from->yp_counter_id;
    to->rp_counter_id = from->rp_counter_id;
}

/*
 * Function:
 *   _bcm_compat6521out_field_entry_config_t
 * Purpose:
 *   Convert the bcm_field_entry_config_t datatype from 6.5.21+ to
 *   <=6.5.21.
 * Parameters:
 *   from     - (IN) The 6.5.21+ version of the datatype.
 *   to       - (OUT) The <=6.5.21 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6521out_field_entry_config_t(
    bcm_field_entry_config_t *from,
    bcm_compat6521_field_entry_config_t *to)
{
    int idx = 0;
    BCM_FIELD_ASET_INIT(to->aset);
    for (idx = 0; idx < bcmFieldActionCount; idx++) {
        if (BCM_FIELD_ASET_TEST(from->aset, idx)) {
            BCM_FIELD_ASET_ADD(to->aset, idx);
        }
    }

       to->flags = from->flags;
       to->entry_id = from->entry_id;
    to->group_id = from->group_id;
    to->priority = from->priority;
    to->status_flags = from->status_flags;
    to->flex_counter_action_id = from->flex_counter_action_id;
    to->counter_id = from->counter_id;
    to->gp_counter_id = from->gp_counter_id;
    to->yp_counter_id = from->yp_counter_id;
    to->rp_counter_id = from->rp_counter_id;
}

/*
 * Function:
 *   bcm_compat6521_field_entry_config_create
 * Purpose:
 *   Compatibility function for RPC call to bcm_field_entry_config_create.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   entry_config   - (INOUT) Entry Config.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6521_field_entry_config_create(
    int unit,
    bcm_compat6521_field_entry_config_t *entry_config)
{
    int rv;
    bcm_field_entry_config_t *new_config = NULL;

    if (entry_config != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_config = sal_alloc(sizeof(bcm_field_entry_config_t),
                               "New entry cfg");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_field_entry_config_t(entry_config, new_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_entry_config_create(unit, new_config);

    if (new_config != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6521out_field_entry_config_t(new_config, entry_config);
        sal_free(new_config);
    }

    return rv;
}

/*
 * Function:
 *   bcm_compat6521_field_entry_config_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_field_entry_config_get.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   entry_config   - (INOUT) Entry Config.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6521_field_entry_config_get(
    int unit,
    bcm_compat6521_field_entry_config_t *entry_config)
{
    int rv;
    bcm_field_entry_config_t *new_config = NULL;

    if (entry_config != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_config = sal_alloc(sizeof(bcm_field_entry_config_t),
                               "New entry cfg");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6521in_field_entry_config_t(entry_config, new_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_entry_config_get(unit, new_config);

    if (new_config != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6521out_field_entry_config_t(new_config, entry_config);
        sal_free(new_config);
    }

    return rv;
}

#endif /* BCM_RPC_SUPPORT */
