/*
* $Id: compat_6525.c,v 1.0 2021/11/18
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2022 Broadcom Inc. All rights reserved.
*
* RPC Compatibility with sdk-6.5.25 routines
*/

#ifdef BCM_RPC_SUPPORT
#include <shared/alloc.h>
#include <bcm/error.h>
#include <bcm_int/compat/compat_6525.h>




/*
 * Function:
 *      _bcm_compat6525in_subport_config_t
 * Purpose:
 *      Convert the bcm_subport_config_t datatype from <=6.5.25 to
 *      SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.25 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525in_subport_config_t(
    bcm_compat6525_subport_config_t *from,
    bcm_subport_config_t *to)
{
    int i1 = 0;

    bcm_subport_config_t_init(to);
    to->group = from->group;
    to->pkt_vlan = from->pkt_vlan;
    to->inner_vlan = from->inner_vlan;
    to->outer_vlan = from->outer_vlan;
    to->int_pri = from->int_pri;
    to->prop_flags = from->prop_flags;
    for (i1 = 0; i1 < BCM_SUBPORT_CONFIG_MAX_STREAMS; i1++) {
        to->stream_id_array[i1] = from->stream_id_array[i1];
    }
    to->num_streams = from->num_streams;
    to->color = from->color;
    to->criteria = from->criteria;
    to->subport_modport = from->subport_modport;
}

/*
 * Function:
 *      _bcm_compat6525out_subport_config_t
 * Purpose:
 *      Convert the bcm_subport_config_t datatype from SDK 6.5.26+ to
 *      <=6.5.25
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525out_subport_config_t(
    bcm_subport_config_t *from,
    bcm_compat6525_subport_config_t *to)
{
    int i1 = 0;

    to->group = from->group;
    to->pkt_vlan = from->pkt_vlan;
    to->inner_vlan = from->inner_vlan;
    to->outer_vlan = from->outer_vlan;
    to->int_pri = from->int_pri;
    to->prop_flags = from->prop_flags;
    for (i1 = 0; i1 < BCM_SUBPORT_CONFIG_MAX_STREAMS; i1++) {
        to->stream_id_array[i1] = from->stream_id_array[i1];
    }
    to->num_streams = from->num_streams;
    to->color = from->color;
    to->criteria = from->criteria;
    to->subport_modport = from->subport_modport;
}

/*
 * Function:
 *      bcm_compat6525_subport_port_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_subport_port_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      config - (IN) Subport config
 *      port - (OUT) GPORT identifier
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_subport_port_add(
    int unit,
    bcm_compat6525_subport_config_t *config,
    bcm_gport_t *port)
{
    int rv = BCM_E_NONE;
    bcm_subport_config_t *new_config = NULL;

    if (config != NULL) {
        new_config = (bcm_subport_config_t *)
                     sal_alloc(sizeof(bcm_subport_config_t),
                     "New config");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_subport_config_t(config, new_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_subport_port_add(unit, new_config, port);


    /* Deallocate memory*/
    sal_free(new_config);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_subport_port_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_subport_port_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) GPORT identifier
 *      config - (OUT) Subport config
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_subport_port_get(
    int unit,
    bcm_gport_t port,
    bcm_compat6525_subport_config_t *config)
{
    int rv = BCM_E_NONE;
    bcm_subport_config_t *new_config = NULL;

    if (config != NULL) {
        new_config = (bcm_subport_config_t *)
                     sal_alloc(sizeof(bcm_subport_config_t),
                     "New config");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_subport_port_get(unit, port, new_config);

    /* Transform the entry from the new format to old one */
    _bcm_compat6525out_subport_config_t(new_config, config);

    /* Deallocate memory*/
    sal_free(new_config);

    return rv;
}


/*
 * Function:
 *      _bcm_compat6525in_l2_egress_t
 * Purpose:
 *      Convert the bcm_l2_egress_t datatype from <=6.5.25 to
 *      SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.25 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525in_l2_egress_t(
    bcm_compat6525_l2_egress_t *from,
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
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
}

/*
 * Function:
 *      _bcm_compat6525out_l2_egress_t
 * Purpose:
 *      Convert the bcm_l2_egress_t datatype from SDK 6.5.26+ to
 *      <=6.5.25
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525out_l2_egress_t(
    bcm_l2_egress_t *from,
    bcm_compat6525_l2_egress_t *to)
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
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
}

/*
 * Function:
 *      bcm_compat6525_l2_egress_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_egress_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      egr - (INOUT) (IN/OUT) pointer to an L2 egress structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_l2_egress_create(
    int unit,
    bcm_compat6525_l2_egress_t *egr)
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
        _bcm_compat6525in_l2_egress_t(egr, new_egr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_egress_create(unit, new_egr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6525out_l2_egress_t(new_egr, egr);

    /* Deallocate memory*/
    sal_free(new_egr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l2_egress_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_egress_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      encap_id - (IN) encap_id of the entry to which to get config for
 *      egr - (OUT) config of the entry
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_l2_egress_get(
    int unit,
    bcm_if_t encap_id,
    bcm_compat6525_l2_egress_t *egr)
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

    /* Call the BCM API with new format */
    rv = bcm_l2_egress_get(unit, encap_id, new_egr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6525out_l2_egress_t(new_egr, egr);

    /* Deallocate memory*/
    sal_free(new_egr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l2_egress_find
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_egress_find.
 * Parameters:
 *      unit - (IN) Unit number.
 *      egr - (IN) egress config to search for
 *      encap_id - (OUT) encap ID of the entry
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_l2_egress_find(
    int unit,
    bcm_compat6525_l2_egress_t *egr,
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
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_l2_egress_t(egr, new_egr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_egress_find(unit, new_egr, encap_id);


    /* Deallocate memory*/
    sal_free(new_egr);

    return rv;
}


/*
 * Function:
 *      _bcm_compat6525in_l2_addr_t
 * Purpose:
 *      Convert the bcm_l2_addr_t datatype from <=6.5.25 to
 *      SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.25 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525in_l2_addr_t(
    bcm_compat6525_l2_addr_t *from,
    bcm_l2_addr_t *to)
{
    int i1 = 0;

    sal_memset(to, 0, sizeof(*to));
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->station_flags = from->station_flags;
    for (i1 = 0; i1 < 6; i1++) {
        to->mac[i1] = from->mac[i1];
    }
    to->vid = from->vid;
    to->port = from->port;
    to->modid = from->modid;
    to->tgid = from->tgid;
    to->cos_dst = from->cos_dst;
    to->cos_src = from->cos_src;
    to->l2mc_group = from->l2mc_group;
    to->egress_if = from->egress_if;
    to->l3mc_group = from->l3mc_group;
    to->block_bitmap = from->block_bitmap;
    to->auth = from->auth;
    to->group = from->group;
    to->distribution_class = from->distribution_class;
    to->encap_id = from->encap_id;
    to->age_state = from->age_state;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    for (i1 = 0; i1 < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i1++) {
        sal_memcpy(&to->logical_fields[i1], &from->logical_fields[i1], sizeof(bcm_flow_logical_field_t));
    }
    to->num_of_fields = from->num_of_fields;
    to->sa_source_filter_pbmp = from->sa_source_filter_pbmp;
    to->tsn_flowset = from->tsn_flowset;
    to->sr_flowset = from->sr_flowset;
    to->policer_id = from->policer_id;
    to->taf_gate_primap = from->taf_gate_primap;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->gbp_src_id = from->gbp_src_id;
    to->opaque_ctrl_id = from->opaque_ctrl_id;
}

/*
 * Function:
 *      _bcm_compat6525out_l2_addr_t
 * Purpose:
 *      Convert the bcm_l2_addr_t datatype from SDK 6.5.26+ to
 *      <=6.5.25
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525out_l2_addr_t(
    bcm_l2_addr_t *from,
    bcm_compat6525_l2_addr_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->station_flags = from->station_flags;
    for (i1 = 0; i1 < 6; i1++) {
        to->mac[i1] = from->mac[i1];
    }
    to->vid = from->vid;
    to->port = from->port;
    to->modid = from->modid;
    to->tgid = from->tgid;
    to->cos_dst = from->cos_dst;
    to->cos_src = from->cos_src;
    to->l2mc_group = from->l2mc_group;
    to->egress_if = from->egress_if;
    to->l3mc_group = from->l3mc_group;
    to->block_bitmap = from->block_bitmap;
    to->auth = from->auth;
    to->group = from->group;
    to->distribution_class = from->distribution_class;
    to->encap_id = from->encap_id;
    to->age_state = from->age_state;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    for (i1 = 0; i1 < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i1++) {
        sal_memcpy(&to->logical_fields[i1], &from->logical_fields[i1], sizeof(bcm_flow_logical_field_t));
    }
    to->num_of_fields = from->num_of_fields;
    to->sa_source_filter_pbmp = from->sa_source_filter_pbmp;
    to->tsn_flowset = from->tsn_flowset;
    to->sr_flowset = from->sr_flowset;
    to->policer_id = from->policer_id;
    to->taf_gate_primap = from->taf_gate_primap;
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    to->gbp_src_id = from->gbp_src_id;
    to->opaque_ctrl_id = from->opaque_ctrl_id;
}

/*
 * Function:
 *      bcm_compat6525_l2_addr_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_addr_add.
 * Parameters:
 *      unit - (IN) BCM device number
 *      l2addr - (IN) L2 address which is properly initialized
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_l2_addr_add(
    int unit,
    bcm_compat6525_l2_addr_t *l2addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2addr = NULL;

    if (l2addr != NULL) {
        new_l2addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2addr");
        if (new_l2addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_l2_addr_t(l2addr, new_l2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_add(unit, new_l2addr);


    /* Deallocate memory*/
    sal_free(new_l2addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l2_addr_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_addr_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mac_addr - (IN) Input MAC address to search
 *      vid - (IN) Input VLAN ID to search
 *      l2addr - (OUT) Pointer to L2 address structure to receive results
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_l2_addr_get(
    int unit,
    bcm_mac_t mac_addr,
    bcm_vlan_t vid,
    bcm_compat6525_l2_addr_t *l2addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2addr = NULL;

    if (l2addr != NULL) {
        new_l2addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2addr");
        if (new_l2addr == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_get(unit, mac_addr, vid, new_l2addr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6525out_l2_addr_t(new_l2addr, l2addr);

    /* Deallocate memory*/
    sal_free(new_l2addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l2_conflict_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_conflict_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      addr - (IN) L2 address to search for conflicts
 *      cf_array - (OUT) List of L2 addresses conflicting with addr
 *      cf_max - (IN) Number of entries allocated to cf_array
 *      cf_count - (OUT) Actual number of cf_array entries filled
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_l2_conflict_get(
    int unit,
    bcm_compat6525_l2_addr_t *addr,
    bcm_compat6525_l2_addr_t *cf_array,
    int cf_max,
    int *cf_count)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_addr = NULL;
    bcm_l2_addr_t *new_cf_array = NULL;
    int i = 0;

    if (addr != NULL && cf_array != NULL && cf_max > 0) {
        new_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New addr");
        if (new_addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_l2_addr_t(addr, new_addr);
        new_cf_array = (bcm_l2_addr_t *)
                     sal_alloc(cf_max * sizeof(bcm_l2_addr_t),
                     "New cf_array");
        if (new_cf_array == NULL) {
            sal_free(new_addr);
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_conflict_get(unit, new_addr, new_cf_array, cf_max, cf_count);

    for (i = 0; i < cf_max; i++) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6525out_l2_addr_t(&new_cf_array[i], &cf_array[i]);
    }

    /* Deallocate memory*/
    sal_free(new_addr);
    sal_free(new_cf_array);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l2_stat_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_stat_get.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      l2_addr - (IN) Pointer to an L2 address structure.
 *      stat - (IN) Type of the counter to retrieve, as defined by bcm_l2_stat_t.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_l2_stat_get(
    int unit,
    bcm_compat6525_l2_addr_t *l2_addr,
    bcm_l2_stat_t stat,
    uint64 *val)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2_addr = NULL;

    if (l2_addr != NULL) {
        new_l2_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2_addr");
        if (new_l2_addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_l2_addr_t(l2_addr, new_l2_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_get(unit, new_l2_addr, stat, val);


    /* Deallocate memory*/
    sal_free(new_l2_addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l2_stat_get32
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_stat_get32.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2_addr - (IN) Pointer to an L2 address structure.
 *      stat - (IN) Type of the counter to retrieve, as defined by bcm_l2_stat_t.
 *      val - (OUT) Pointer to a counter value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_l2_stat_get32(
    int unit,
    bcm_compat6525_l2_addr_t *l2_addr,
    bcm_l2_stat_t stat,
    uint32 *val)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2_addr = NULL;

    if (l2_addr != NULL) {
        new_l2_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2_addr");
        if (new_l2_addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_l2_addr_t(l2_addr, new_l2_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_get32(unit, new_l2_addr, stat, val);


    /* Deallocate memory*/
    sal_free(new_l2_addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l2_stat_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_stat_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2_addr - (IN) Pointer to an L2 address structure.
 *      stat - (IN) Type of the counter to set, as defined by bcm_l2_stat_t.
 *      val - (IN) Counter value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_l2_stat_set(
    int unit,
    bcm_compat6525_l2_addr_t *l2_addr,
    bcm_l2_stat_t stat,
    uint64 val)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2_addr = NULL;

    if (l2_addr != NULL) {
        new_l2_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2_addr");
        if (new_l2_addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_l2_addr_t(l2_addr, new_l2_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_set(unit, new_l2_addr, stat, val);


    /* Deallocate memory*/
    sal_free(new_l2_addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l2_stat_set32
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_stat_set32.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2_addr - (IN) Pointer to an L2 address structure.
 *      stat - (IN) Type of the counter to set, as defined by bcm_l2_stat_t.
 *      val - (IN) Counter value.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_l2_stat_set32(
    int unit,
    bcm_compat6525_l2_addr_t *l2_addr,
    bcm_l2_stat_t stat,
    uint32 val)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2_addr = NULL;

    if (l2_addr != NULL) {
        new_l2_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2_addr");
        if (new_l2_addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_l2_addr_t(l2_addr, new_l2_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_set32(unit, new_l2_addr, stat, val);


    /* Deallocate memory*/
    sal_free(new_l2_addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l2_stat_enable_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_stat_enable_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2_addr - (IN) pointer to an L2 address structure.
 *      enable - (IN) Zero to disable, anything else to enable.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_l2_stat_enable_set(
    int unit,
    bcm_compat6525_l2_addr_t *l2_addr,
    int enable)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2_addr = NULL;

    if (l2_addr != NULL) {
        new_l2_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2_addr");
        if (new_l2_addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_l2_addr_t(l2_addr, new_l2_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_enable_set(unit, new_l2_addr, enable);


    /* Deallocate memory*/
    sal_free(new_l2_addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l2_replace
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_replace.
 * Parameters:
 *      unit - (IN) BCM device number
 *      flags - (IN) Replace/delete flags BCM_L2_REPLACE_*
 *      match_addr - (IN) L2 parameters to match on delete/replace
 *      new_module - (IN) New module ID for a replace
 *      new_port - (IN) New port for a replace
 *      new_trunk - (IN) New trunk ID for a replace
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_l2_replace(
    int unit,
    uint32 flags,
    bcm_compat6525_l2_addr_t *match_addr,
    bcm_module_t new_module,
    bcm_port_t new_port,
    bcm_trunk_t new_trunk)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_match_addr = NULL;

    if (match_addr != NULL) {
        new_match_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New match_addr");
        if (new_match_addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_l2_addr_t(match_addr, new_match_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_replace(unit, flags, new_match_addr, new_module, new_port, new_trunk);


    /* Deallocate memory*/
    sal_free(new_match_addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l2_replace_match
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_replace_match.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flags - (IN) Replace/delete flags BCM_L2_REPLACE_*
 *      match_addr - (IN) L2 parameters to match on delete/replace
 *      mask_addr - (IN) L2 mask to match on delete/replace
 *      replace_addr - (IN) value to replace match entries (not relevant in case of delete)
 *      replace_mask_addr - (IN) sets which fields/bits from replace_addr are relevant for replace. Unmasked fields/bit will not be affected. (not relevant for delete)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_l2_replace_match(
    int unit,
    uint32 flags,
    bcm_compat6525_l2_addr_t *match_addr,
    bcm_compat6525_l2_addr_t *mask_addr,
    bcm_compat6525_l2_addr_t *replace_addr,
    bcm_compat6525_l2_addr_t *replace_mask_addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_match_addr = NULL;
    bcm_l2_addr_t *new_mask_addr = NULL;
    bcm_l2_addr_t *new_replace_addr = NULL;
    bcm_l2_addr_t *new_replace_mask_addr = NULL;

    if (match_addr != NULL && mask_addr != NULL && replace_addr != NULL && replace_mask_addr != NULL) {
        new_match_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New match_addr");
        if (new_match_addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_l2_addr_t(match_addr, new_match_addr);
        new_mask_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New mask_addr");
        if (new_mask_addr == NULL) {
            sal_free(new_match_addr);
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_l2_addr_t(mask_addr, new_mask_addr);
        new_replace_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New replace_addr");
        if (new_replace_addr == NULL) {
            sal_free(new_match_addr);
            sal_free(new_mask_addr);
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_l2_addr_t(replace_addr, new_replace_addr);
        new_replace_mask_addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New replace_mask_addr");
        if (new_replace_mask_addr == NULL) {
            sal_free(new_match_addr);
            sal_free(new_mask_addr);
            sal_free(new_replace_addr);
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_l2_addr_t(replace_mask_addr, new_replace_mask_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_replace_match(unit, flags, new_match_addr, new_mask_addr, new_replace_addr, new_replace_mask_addr);


    /* Deallocate memory*/
    sal_free(new_match_addr);
    sal_free(new_mask_addr);
    sal_free(new_replace_addr);
    sal_free(new_replace_mask_addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l2_addr_multi_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_addr_multi_add.
 * Parameters:
 *      unit - (IN) BCM device number
 *      l2addr - (IN) L2 address which is properly initialized
 *      count - (IN) L2 address count
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_l2_addr_multi_add(
    int unit,
    bcm_compat6525_l2_addr_t *l2addr,
    int count)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2addr = NULL;

    if (l2addr != NULL) {
        new_l2addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2addr");
        if (new_l2addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_l2_addr_t(l2addr, new_l2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_multi_add(unit, new_l2addr, count);


    /* Deallocate memory*/
    sal_free(new_l2addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l2_addr_multi_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_addr_multi_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2addr - (IN) Pointer to layer2 address type<br>L2 address which is properly initialized
 *      count - (IN) L2 address count
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_l2_addr_multi_delete(
    int unit,
    bcm_compat6525_l2_addr_t *l2addr,
    int count)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2addr = NULL;

    if (l2addr != NULL) {
        new_l2addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2addr");
        if (new_l2addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_l2_addr_t(l2addr, new_l2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_multi_delete(unit, new_l2addr, count);


    /* Deallocate memory*/
    sal_free(new_l2addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l2_addr_by_struct_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_addr_by_struct_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2addr - (INOUT) Pointer to layer2 address type
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_l2_addr_by_struct_get(
    int unit,
    bcm_compat6525_l2_addr_t *l2addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2addr = NULL;

    if (l2addr != NULL) {
        new_l2addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2addr");
        if (new_l2addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_l2_addr_t(l2addr, new_l2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_by_struct_get(unit, new_l2addr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6525out_l2_addr_t(new_l2addr, l2addr);

    /* Deallocate memory*/
    sal_free(new_l2addr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l2_addr_by_struct_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2_addr_by_struct_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2addr - (IN) Pointer to layer2 address type
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_l2_addr_by_struct_delete(
    int unit,
    bcm_compat6525_l2_addr_t *l2addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *new_l2addr = NULL;

    if (l2addr != NULL) {
        new_l2addr = (bcm_l2_addr_t *)
                     sal_alloc(sizeof(bcm_l2_addr_t),
                     "New l2addr");
        if (new_l2addr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_l2_addr_t(l2addr, new_l2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_by_struct_delete(unit, new_l2addr);


    /* Deallocate memory*/
    sal_free(new_l2addr);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6525in_vlan_port_t
 * Purpose:
 *      Convert the bcm_vlan_port_t datatype from <=6.5.25 to
 *      SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.25 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525in_vlan_port_t(
    bcm_compat6525_vlan_port_t *from,
    bcm_vlan_port_t *to)
{
    bcm_vlan_port_t_init(to);
    to->criteria = from->criteria;
    to->flags = from->flags;
    to->vsi = from->vsi;
    to->match_vlan = from->match_vlan;
    to->match_inner_vlan = from->match_inner_vlan;
    to->match_pcp = from->match_pcp;
    to->match_tunnel_value = from->match_tunnel_value;
    to->match_ethertype = from->match_ethertype;
    to->port = from->port;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->egress_service_tpid = from->egress_service_tpid;
    to->egress_vlan = from->egress_vlan;
    to->egress_inner_vlan = from->egress_inner_vlan;
    to->egress_tunnel_value = from->egress_tunnel_value;
    to->encap_id = from->encap_id;
    to->qos_map_id = from->qos_map_id;
    to->policer_id = from->policer_id;
    to->egress_policer_id = from->egress_policer_id;
    to->failover_id = from->failover_id;
    to->failover_port_id = from->failover_port_id;
    to->vlan_port_id = from->vlan_port_id;
    to->failover_mc_group = from->failover_mc_group;
    to->ingress_failover_id = from->ingress_failover_id;
    to->egress_failover_id = from->egress_failover_id;
    to->egress_failover_port_id = from->egress_failover_port_id;
    to->ingress_network_group_id = from->ingress_network_group_id;
    to->egress_network_group_id = from->egress_network_group_id;
    to->inlif_counting_profile = from->inlif_counting_profile;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->if_class = from->if_class;
    to->tunnel_id = from->tunnel_id;
    to->group = from->group;
    to->ingress_failover_port_id = from->ingress_failover_port_id;
    to->class_id = from->class_id;
    to->match_class_id = from->match_class_id;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
    to->flags2 = from->flags2;
}

/*
 * Function:
 *      _bcm_compat6525out_vlan_port_t
 * Purpose:
 *      Convert the bcm_vlan_port_t datatype from SDK 6.5.26+ to
 *      <=6.5.25
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525out_vlan_port_t(
    bcm_vlan_port_t *from,
    bcm_compat6525_vlan_port_t *to)
{
    to->criteria = from->criteria;
    to->flags = from->flags;
    to->vsi = from->vsi;
    to->match_vlan = from->match_vlan;
    to->match_inner_vlan = from->match_inner_vlan;
    to->match_pcp = from->match_pcp;
    to->match_tunnel_value = from->match_tunnel_value;
    to->match_ethertype = from->match_ethertype;
    to->port = from->port;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->egress_service_tpid = from->egress_service_tpid;
    to->egress_vlan = from->egress_vlan;
    to->egress_inner_vlan = from->egress_inner_vlan;
    to->egress_tunnel_value = from->egress_tunnel_value;
    to->encap_id = from->encap_id;
    to->qos_map_id = from->qos_map_id;
    to->policer_id = from->policer_id;
    to->egress_policer_id = from->egress_policer_id;
    to->failover_id = from->failover_id;
    to->failover_port_id = from->failover_port_id;
    to->vlan_port_id = from->vlan_port_id;
    to->failover_mc_group = from->failover_mc_group;
    to->ingress_failover_id = from->ingress_failover_id;
    to->egress_failover_id = from->egress_failover_id;
    to->egress_failover_port_id = from->egress_failover_port_id;
    to->ingress_network_group_id = from->ingress_network_group_id;
    to->egress_network_group_id = from->egress_network_group_id;
    to->inlif_counting_profile = from->inlif_counting_profile;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->if_class = from->if_class;
    to->tunnel_id = from->tunnel_id;
    to->group = from->group;
    to->ingress_failover_port_id = from->ingress_failover_port_id;
    to->class_id = from->class_id;
    to->match_class_id = from->match_class_id;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
    to->flags2 = from->flags2;
}

/*
 * Function:
 *      bcm_compat6525_vlan_port_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_port_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan_port - (INOUT) (IN/OUT) Layer 2 Logical port.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_vlan_port_create(
    int unit,
    bcm_compat6525_vlan_port_t *vlan_port)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t *new_vlan_port = NULL;

    if (vlan_port != NULL) {
        new_vlan_port = (bcm_vlan_port_t *)
                     sal_alloc(sizeof(bcm_vlan_port_t),
                     "New vlan_port");
        if (new_vlan_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_vlan_port_t(vlan_port, new_vlan_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_create(unit, new_vlan_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6525out_vlan_port_t(new_vlan_port, vlan_port);

    /* Deallocate memory*/
    sal_free(new_vlan_port);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_vlan_port_find
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_port_find.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vlan_port - (INOUT) (IN/OUT) Layer 2 logical port
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_vlan_port_find(
    int unit,
    bcm_compat6525_vlan_port_t *vlan_port)
{
    int rv = BCM_E_NONE;
    bcm_vlan_port_t *new_vlan_port = NULL;

    if (vlan_port != NULL) {
        new_vlan_port = (bcm_vlan_port_t *)
                     sal_alloc(sizeof(bcm_vlan_port_t),
                     "New vlan_port");
        if (new_vlan_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_vlan_port_t(vlan_port, new_vlan_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_port_find(unit, new_vlan_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6525out_vlan_port_t(new_vlan_port, vlan_port);

    /* Deallocate memory*/
    sal_free(new_vlan_port);

    return rv;
}

int bcm_compat6525_field_qualify_RangeCheckGroup(
    int unit,
    bcm_field_entry_t entry,
    uint8 data,
    uint8 mask)
{
    int rv = BCM_E_NONE;
    uint32 data_n = 0;
    uint32 mask_n = 0;

    data_n = data;
    mask_n = mask;

    rv = bcm_field_qualify_RangeCheckGroup(unit, entry, data_n, mask_n);

    return rv;
}

int bcm_compat6525_field_qualify_RangeCheckGroup_get(
    int unit,
    bcm_field_entry_t entry,
    uint8 *data,
    uint8 *mask)
{
    int rv = BCM_E_NONE;
    uint32 data_n = 0;
    uint32 mask_n = 0;

    rv = bcm_field_qualify_RangeCheckGroup_get(unit, entry, &data_n, &mask_n);
    if (BCM_SUCCESS(rv)) {
        *data = (uint8)data_n;
        *mask = (uint8)mask_n;
    }

    return rv;
}

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6525in_mpls_port_t
 * Purpose:
 *      Convert the bcm_mpls_port_t datatype from <=6.5.25 to
 *      SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.25 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525in_mpls_port_t(
    bcm_compat6525_mpls_port_t *from,
    bcm_mpls_port_t *to)
{
    bcm_mpls_port_t_init(to);
    to->mpls_port_id = from->mpls_port_id;
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->if_class = from->if_class;
    to->exp_map = from->exp_map;
    to->int_pri = from->int_pri;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->service_tpid = from->service_tpid;
    to->port = from->port;
    to->criteria = from->criteria;
    to->match_vlan = from->match_vlan;
    to->match_inner_vlan = from->match_inner_vlan;
    to->match_label = from->match_label;
    to->egress_tunnel_if = from->egress_tunnel_if;
    sal_memcpy(&to->egress_label, &from->egress_label, sizeof(bcm_mpls_egress_label_t));
    to->mtu = from->mtu;
    to->egress_service_vlan = from->egress_service_vlan;
    to->pw_seq_number = from->pw_seq_number;
    to->encap_id = from->encap_id;
    to->ingress_failover_id = from->ingress_failover_id;
    to->ingress_failover_port_id = from->ingress_failover_port_id;
    to->failover_id = from->failover_id;
    to->failover_port_id = from->failover_port_id;
    to->policer_id = from->policer_id;
    to->failover_mc_group = from->failover_mc_group;
    to->pw_failover_id = from->pw_failover_id;
    to->pw_failover_port_id = from->pw_failover_port_id;
    to->vccv_type = from->vccv_type;
    to->network_group_id = from->network_group_id;
    to->match_subport_pkt_vid = from->match_subport_pkt_vid;
    to->tunnel_id = from->tunnel_id;
    to->per_flow_queue_base = from->per_flow_queue_base;
    to->qos_map_id = from->qos_map_id;
    to->egress_failover_id = from->egress_failover_id;
    to->egress_failover_port_id = from->egress_failover_port_id;
    to->ecn_map_id = from->ecn_map_id;
    to->class_id = from->class_id;
    to->egress_class_id = from->egress_class_id;
    to->inlif_counting_profile = from->inlif_counting_profile;
    sal_memcpy(&to->egress_tunnel_label, &from->egress_tunnel_label, sizeof(bcm_mpls_egress_label_t));
    to->nof_service_tags = from->nof_service_tags;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    to->context_label = from->context_label;
    to->ingress_if = from->ingress_if;
    to->port_group = from->port_group;
    to->dscp_map_id = from->dscp_map_id;
    to->vlan_pri_map_id = from->vlan_pri_map_id;
    to->match_default_vlan = from->match_default_vlan;
    to->match_default_vpn = from->match_default_vpn;
}

/*
 * Function:
 *      _bcm_compat6525out_mpls_port_t
 * Purpose:
 *      Convert the bcm_mpls_port_t datatype from SDK 6.5.26+ to
 *      <=6.5.25
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525out_mpls_port_t(
    bcm_mpls_port_t *from,
    bcm_compat6525_mpls_port_t *to)
{
    to->mpls_port_id = from->mpls_port_id;
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->if_class = from->if_class;
    to->exp_map = from->exp_map;
    to->int_pri = from->int_pri;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->service_tpid = from->service_tpid;
    to->port = from->port;
    to->criteria = from->criteria;
    to->match_vlan = from->match_vlan;
    to->match_inner_vlan = from->match_inner_vlan;
    to->match_label = from->match_label;
    to->egress_tunnel_if = from->egress_tunnel_if;
    sal_memcpy(&to->egress_label, &from->egress_label, sizeof(bcm_mpls_egress_label_t));
    to->mtu = from->mtu;
    to->egress_service_vlan = from->egress_service_vlan;
    to->pw_seq_number = from->pw_seq_number;
    to->encap_id = from->encap_id;
    to->ingress_failover_id = from->ingress_failover_id;
    to->ingress_failover_port_id = from->ingress_failover_port_id;
    to->failover_id = from->failover_id;
    to->failover_port_id = from->failover_port_id;
    to->policer_id = from->policer_id;
    to->failover_mc_group = from->failover_mc_group;
    to->pw_failover_id = from->pw_failover_id;
    to->pw_failover_port_id = from->pw_failover_port_id;
    to->vccv_type = from->vccv_type;
    to->network_group_id = from->network_group_id;
    to->match_subport_pkt_vid = from->match_subport_pkt_vid;
    to->tunnel_id = from->tunnel_id;
    to->per_flow_queue_base = from->per_flow_queue_base;
    to->qos_map_id = from->qos_map_id;
    to->egress_failover_id = from->egress_failover_id;
    to->egress_failover_port_id = from->egress_failover_port_id;
    to->ecn_map_id = from->ecn_map_id;
    to->class_id = from->class_id;
    to->egress_class_id = from->egress_class_id;
    to->inlif_counting_profile = from->inlif_counting_profile;
    sal_memcpy(&to->egress_tunnel_label, &from->egress_tunnel_label, sizeof(bcm_mpls_egress_label_t));
    to->nof_service_tags = from->nof_service_tags;
    sal_memcpy(&to->ingress_qos_model, &from->ingress_qos_model, sizeof(bcm_qos_ingress_model_t));
    to->context_label = from->context_label;
    to->ingress_if = from->ingress_if;
    to->port_group = from->port_group;
    to->dscp_map_id = from->dscp_map_id;
    to->vlan_pri_map_id = from->vlan_pri_map_id;
    to->match_default_vlan = from->match_default_vlan;
    to->match_default_vpn = from->match_default_vpn;
}

/*
 * Function:
 *      bcm_compat6525_mpls_port_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_port_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vpn - (IN) VPN ID
 *      mpls_port - (INOUT) (IN/OUT) MPLS port information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_mpls_port_add(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6525_mpls_port_t *mpls_port)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t *new_mpls_port = NULL;

    if (mpls_port != NULL) {
        new_mpls_port = (bcm_mpls_port_t *)
                     sal_alloc(sizeof(bcm_mpls_port_t),
                     "New mpls_port");
        if (new_mpls_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_mpls_port_t(mpls_port, new_mpls_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_port_add(unit, vpn, new_mpls_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6525out_mpls_port_t(new_mpls_port, mpls_port);

    /* Deallocate memory*/
    sal_free(new_mpls_port);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_mpls_port_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_port_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vpn - (IN) VPN ID
 *      mpls_port - (INOUT) (IN/OUT) MPLS port information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_mpls_port_get(
    int unit,
    bcm_vpn_t vpn,
    bcm_compat6525_mpls_port_t *mpls_port)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t *new_mpls_port = NULL;

    if (mpls_port != NULL) {
        new_mpls_port = (bcm_mpls_port_t *)
                     sal_alloc(sizeof(bcm_mpls_port_t),
                     "New mpls_port");
        if (new_mpls_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_mpls_port_t(mpls_port, new_mpls_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_port_get(unit, vpn, new_mpls_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6525out_mpls_port_t(new_mpls_port, mpls_port);

    /* Deallocate memory*/
    sal_free(new_mpls_port);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_mpls_port_get_all
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_port_get_all.
 * Parameters:
 *      unit - (IN) Unit number.
 *      vpn - (IN) VPN ID
 *      port_max - (IN) Maximum number of ports in array
 *      port_array - (OUT) Array of MPLS ports
 *      port_count - (OUT) Number of ports returned in array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_mpls_port_get_all(
    int unit,
    bcm_vpn_t vpn,
    int port_max,
    bcm_compat6525_mpls_port_t *port_array,
    int *port_count)
{
    int rv = BCM_E_NONE;
    bcm_mpls_port_t *new_port_array = NULL;
    int i = 0;

    if (port_array != NULL && port_max > 0) {
        new_port_array = (bcm_mpls_port_t *)
                     sal_alloc(port_max * sizeof(bcm_mpls_port_t),
                     "New port_array");
        if (new_port_array == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mpls_port_get_all(unit, vpn, port_max, new_port_array, port_count);

    for (i = 0; i < port_max; i++) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6525out_mpls_port_t(&new_port_array[i], &port_array[i]);
    }

    /* Deallocate memory*/
    sal_free(new_port_array);

    return rv;
}


#endif /* defined(INCLUDE_L3) */

/*
 * Function:
 *      _bcm_compat6525out_cosq_ebst_data_entry_t
 * Purpose:
 *      Convert the bcm_cosq_ebst_data_entry_t datatype from SDK 6.5.26+ to
 *      <=6.5.25
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525out_cosq_ebst_data_entry_t(
    bcm_cosq_ebst_data_entry_t *from,
    bcm_compat6525_cosq_ebst_data_entry_t *to)
{
    to->timestamp = from->timestamp;
    to->bytes = from->bytes;
    to->green_drop_state = from->green_drop_state;
    to->yellow_drop_state = from->yellow_drop_state;
    to->red_drop_state = from->red_drop_state;
}

/*
 * Function:
 *      bcm_compat6525_cosq_ebst_data_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_cosq_ebst_data_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      object_id - (IN) Compound index containing gport, cosq, buffer, etc. See \ref bcm_cosq_object_id_t_init
 *      bid - (IN) BST stat ID to identify the COSQ resource/object
 *      array_size - (IN) Array size
 *      entry_array - (OUT) Entry array
 *      count - (OUT) Array Count
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_cosq_ebst_data_get(
    int unit,
    bcm_cosq_object_id_t *object_id,
    bcm_bst_stat_id_t bid,
    int array_size,
    bcm_compat6525_cosq_ebst_data_entry_t *entry_array,
    int *count)
{
    int rv = BCM_E_NONE;
    bcm_cosq_ebst_data_entry_t *new_entry_array = NULL;
    int i = 0;

    if (entry_array != NULL && array_size > 0) {
        new_entry_array = (bcm_cosq_ebst_data_entry_t *)
                     sal_alloc(array_size * sizeof(bcm_cosq_ebst_data_entry_t),
                     "New entry_array");
        if (new_entry_array == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_cosq_ebst_data_get(unit, object_id, bid, array_size, new_entry_array, count);

    for (i = 0; i < array_size; i++) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6525out_cosq_ebst_data_entry_t(&new_entry_array[i], &entry_array[i]);
    }

    /* Deallocate memory*/
    sal_free(new_entry_array);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6525out_port_config_t
 * Purpose:
 *      Convert the bcm_port_config_t datatype from SDK 6.5.26+ to
 *      <=6.5.25
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525out_port_config_t(
    bcm_port_config_t *from,
    bcm_compat6525_port_config_t *to)
{
    int i1 = 0;

    to->fe = from->fe;
    to->ge = from->ge;
    to->xe = from->xe;
    to->ce = from->ce;
    to->le = from->le;
    to->cc = from->cc;
    to->cd = from->cd;
    to->e = from->e;
    to->hg = from->hg;
    to->sci = from->sci;
    to->sfi = from->sfi;
    to->spi = from->spi;
    to->spi_subport = from->spi_subport;
    to->port = from->port;
    to->cpu = from->cpu;
    to->all = from->all;
    to->stack_int = from->stack_int;
    to->stack_ext = from->stack_ext;
    to->tdm = from->tdm;
    to->pon = from->pon;
    to->llid = from->llid;
    to->il = from->il;
    to->xl = from->xl;
    to->rcy = from->rcy;
    to->sat = from->sat;
    to->ipsec = from->ipsec;
    for (i1 = 0; i1 < BCM_PIPES_MAX; i1++) {
        to->per_pipe[i1] = from->per_pipe[i1];
    }
    to->nif = from->nif;
    to->control = from->control;
    to->eventor = from->eventor;
    to->olp = from->olp;
    to->oamp = from->oamp;
    to->erp = from->erp;
    to->roe = from->roe;
    to->rcy_mirror = from->rcy_mirror;
    for (i1 = 0; i1 < BCM_PP_PIPES_MAX; i1++) {
        to->per_pp_pipe[i1] = from->per_pp_pipe[i1];
    }
    to->stat = from->stat;
    to->crps = from->crps;
    to->d3c = from->d3c;
    return;
}

/*
 * Function:
 *      bcm_compat6525_port_config_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_config_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      config - (OUT) Pointer to port configuration structure populated on successful return.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_port_config_get(
    int unit,
    bcm_compat6525_port_config_t *config)
{
    int rv = BCM_E_NONE;
    bcm_port_config_t *new_config = NULL;

    if (config != NULL) {
        new_config = (bcm_port_config_t *)
                     sal_alloc(sizeof(bcm_port_config_t),
                     "New config");
        if (new_config == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_port_config_get(unit, new_config);

    /* Transform the entry from the new format to old one */
    _bcm_compat6525out_port_config_t(new_config, config);

    /* Deallocate memory*/
    sal_free(new_config);

    return rv;
}

/*
 * Function:
 *   _bcm_compat6525in_port_resource_t
 * Purpose:
 *   Convert the bcm_port_resource_t datatype from <=6.5.25 to
 *   SDK 6.5.26+.
 * Parameters:
 *   from        - (IN) The <=6.5.25 version of the datatype.
 *   to           - (OUT) The SDK 6.5.26+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6525in_port_resource_t(
    bcm_compat6525_port_resource_t *from,
    bcm_port_resource_t *to)
{
    bcm_port_resource_t_init(to);
    to->flags = from->flags;
    to->port = from->port;
    to->physical_port = from->physical_port;
    to->speed = from->speed;
    to->rx_speed = from->rx_speed;
    to->lanes = from->lanes;
    to->encap = from->encap;
    to->fec_type = from->fec_type;
    to->phy_lane_config = from->phy_lane_config;
    to->link_training = from->link_training;
    to->roe_compression = from->roe_compression;
    to->num_subports = from->num_subports;
    to->line_card = from->line_card;
    to->base_flexe_instance = from->base_flexe_instance;
    to->mmu_lossless = from->mmu_lossless;
}

/*
 * Function:
 *      _bcm_compat6525out_port_resource_t
 * Purpose:
 *      Convert the bcm_port_resource_t datatype from 6.5.26+ to
 *      <=6.5.25
 * Parameters:
 *      from       - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525out_port_resource_t(
    bcm_port_resource_t *from,
    bcm_compat6525_port_resource_t *to)
{
    to->flags = from->flags;
    to->port = from->port;
    to->physical_port = from->physical_port;
    to->speed = from->speed;
    to->rx_speed = from->rx_speed;
    to->lanes = from->lanes;
    to->encap = from->encap;
    to->fec_type = from->fec_type;
    to->phy_lane_config = from->phy_lane_config;
    to->link_training = from->link_training;
    to->roe_compression = from->roe_compression;
    to->num_subports = from->num_subports;
    to->line_card = from->line_card;
    to->base_flexe_instance = from->base_flexe_instance;
    to->mmu_lossless = from->mmu_lossless;
}

/*
 * Perform a FlexPort operation changing the port resources for a given
 * logical port.
 */
int
bcm_compat6525_port_resource_set(
    int unit,
    bcm_gport_t port,
    bcm_compat6525_port_resource_t *resource)
{
    int rv = BCM_E_NONE;
    bcm_port_resource_t *new_resource = NULL;

    if (resource != NULL) {
        new_resource = (bcm_port_resource_t *)
                       sal_alloc(sizeof(bcm_port_resource_t),
                       "New resource");
        if (new_resource == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_port_resource_t(resource, new_resource);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_port_resource_set(unit, port, new_resource);

    /* Deallocate memory*/
    if (new_resource != NULL) {
        sal_free(new_resource);
    }

    return rv;
}

/* Get the port resource configuration. */
int
bcm_compat6525_port_resource_get(
    int unit,
    bcm_gport_t port,
    bcm_compat6525_port_resource_t *resource)
{
    int rv = BCM_E_NONE;
    bcm_port_resource_t *new_resource = NULL;

    if (resource != NULL) {
        new_resource = (bcm_port_resource_t *)
                       sal_alloc(sizeof(bcm_port_resource_t),
                       "New resource");
        if (new_resource == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_port_resource_get(unit, port, new_resource);

    if (new_resource != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6525out_port_resource_t(new_resource, resource);

        /* Deallocate memory*/
        sal_free(new_resource);
    }

    return rv;
}

/*
 * Get suggested default resource values prior to calling
 * bcm_port_resource_set.
 */
int
bcm_compat6525_port_resource_default_get(
    int unit,
    bcm_gport_t port,
    uint32 flags,
    bcm_compat6525_port_resource_t *resource)
{
    int rv = BCM_E_NONE;
    bcm_port_resource_t *new_resource = NULL;

    if (resource != NULL) {
        new_resource = (bcm_port_resource_t *)
                       sal_alloc(sizeof(bcm_port_resource_t),
                       "New resource");
        if (new_resource == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_port_resource_t(resource, new_resource);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_port_resource_default_get(unit, port, flags, new_resource);

    if (new_resource != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6525out_port_resource_t(new_resource, resource);

        /* Deallocate memory*/
        sal_free(new_resource);
    }

    return rv;
}

/*
 * Performs a FlexPort operation changing the port resources for a set of
 * ports.
 */
int
bcm_compat6525_port_resource_multi_set(
    int unit,
    int nport,
    bcm_compat6525_port_resource_t *resource)
{
    int rv = BCM_E_NONE;
    bcm_port_resource_t *new_resource = NULL;
    int i = 0;

    if (resource != NULL && nport > 0) {
        new_resource = (bcm_port_resource_t *)
                       sal_alloc(nport * sizeof(bcm_port_resource_t),
                       "New resource");
        if (new_resource == NULL) {
            return BCM_E_MEMORY;
        }
        for (i = 0; i < nport; i++) {
            /* Transform the entry from the old format to new one */
            _bcm_compat6525in_port_resource_t(&resource[i], &new_resource[i]);
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_port_resource_multi_set(unit, nport, new_resource);

    /* Deallocate memory*/
    if (new_resource != NULL) {
        sal_free(new_resource);
    }

    return rv;
}

/*
 * Validate a port's speed configuration on the BCM56980 by fetching the
 * affected ports.
 */
int
bcm_compat6525_port_resource_speed_config_validate(
    int unit,
    bcm_compat6525_port_resource_t *resource,
    bcm_pbmp_t *pbmp)
{
    int rv = BCM_E_NONE;
    bcm_port_resource_t *new_resource = NULL;

    if (resource != NULL) {
        new_resource = (bcm_port_resource_t *)
                       sal_alloc(sizeof(bcm_port_resource_t),
                       "New resource");
        if (new_resource == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_port_resource_t(resource, new_resource);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_port_resource_speed_config_validate(unit, new_resource, pbmp);

    /* Deallocate memory*/
    if (new_resource != NULL) {
        sal_free(new_resource);
    }

    return rv;
}

/* Get/Modify the speed for a given port. */
int
bcm_compat6525_port_resource_speed_get(
    int unit,
    bcm_gport_t port,
    bcm_compat6525_port_resource_t *resource)
{
    int rv = BCM_E_NONE;
    bcm_port_resource_t *new_resource = NULL;

    if (resource != NULL) {
        new_resource = (bcm_port_resource_t *)
                       sal_alloc(sizeof(bcm_port_resource_t),
                       "New resource");
        if (new_resource == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_port_resource_speed_get(unit, port, new_resource);

    if (new_resource != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6525out_port_resource_t(new_resource, resource);

        /* Deallocate memory*/
        sal_free(new_resource);
    }

    return rv;
}

/* Get/Modify the speed for a given port. */
int
bcm_compat6525_port_resource_speed_set(
    int unit,
    bcm_gport_t port,
    bcm_compat6525_port_resource_t *resource)
{
    int rv = BCM_E_NONE;
    bcm_port_resource_t *new_resource = NULL;

    if (resource != NULL) {
        new_resource = (bcm_port_resource_t *)
                       sal_alloc(sizeof(bcm_port_resource_t),
                       "New resource");
        if (new_resource == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_port_resource_t(resource, new_resource);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_port_resource_speed_set(unit, port, new_resource);

    /* Deallocate memory*/
    if (new_resource != NULL) {
        sal_free(new_resource);
    }

    return rv;
}

/* Modify the Port Speeds for a set of ports in a single Port Macro. */
int
bcm_compat6525_port_resource_speed_multi_set(
    int unit,
    int nport,
    bcm_compat6525_port_resource_t *resource)
{
    int rv = BCM_E_NONE;
    bcm_port_resource_t *new_resource = NULL;
    int i = 0;

    if (resource != NULL && nport > 0) {
        new_resource = (bcm_port_resource_t *)
                       sal_alloc(nport * sizeof(bcm_port_resource_t),
                       "New resource");
        if (new_resource == NULL) {
            return BCM_E_MEMORY;
        }
        for (i = 0; i < nport; i++) {
            /* Transform the entry from the old format to new one */
            _bcm_compat6525in_port_resource_t(&resource[i], &new_resource[i]);
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_port_resource_speed_multi_set(unit, nport, new_resource);

    /* Deallocate memory*/
    if (new_resource != NULL) {
        sal_free(new_resource);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6525in_mirror_pkt_dnx_pp_header_t
 * Purpose:
 *      Convert the bcm_mirror_pkt_dnx_pp_header_t datatype from <=6.5.25 to
 *      SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.25 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525in_mirror_pkt_dnx_pp_header_t(
    bcm_compat6525_mirror_pkt_dnx_pp_header_t *from,
    bcm_mirror_pkt_dnx_pp_header_t *to)
{
    int i1 = 0;

    to->tail_edit_profile = from->tail_edit_profile;
    to->bytes_to_remove = from->bytes_to_remove;
    to->eth_header_remove = from->eth_header_remove;
    to->force_mirror = from->force_mirror;
    for (i1 = 0; i1 < 3; i1++) {
        to->out_vport_ext[i1] = from->out_vport_ext[i1];
    }
    to->vsi = from->vsi;
}

/*
 * Function:
 *      _bcm_compat6525out_mirror_pkt_dnx_pp_header_t
 * Purpose:
 *      Convert the bcm_mirror_pkt_dnx_pp_header_t datatype from SDK 6.5.26+ to
 *      <=6.5.25
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525out_mirror_pkt_dnx_pp_header_t(
    bcm_mirror_pkt_dnx_pp_header_t *from,
    bcm_compat6525_mirror_pkt_dnx_pp_header_t *to)
{
    int i1 = 0;

    to->tail_edit_profile = from->tail_edit_profile;
    to->bytes_to_remove = from->bytes_to_remove;
    to->eth_header_remove = from->eth_header_remove;
    to->force_mirror = from->force_mirror;
    for (i1 = 0; i1 < 3; i1++) {
        to->out_vport_ext[i1] = from->out_vport_ext[i1];
    }
    to->vsi = from->vsi;
}

/*
 * Function:
 *      _bcm_compat6525in_mirror_header_info_t
 * Purpose:
 *      Convert the bcm_mirror_header_info_t datatype from <=6.5.25 to
 *      SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.25 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525in_mirror_header_info_t(
    bcm_compat6525_mirror_header_info_t *from,
    bcm_mirror_header_info_t *to)
{
    int i1 = 0;

    bcm_mirror_header_info_t_init(to);
    sal_memcpy(&to->tm, &from->tm, sizeof(bcm_mirror_pkt_dnx_ftmh_header_t));
    _bcm_compat6525in_mirror_pkt_dnx_pp_header_t(&from->pp, &to->pp);
    for (i1 = 0; i1 < 4; i1++) {
        sal_memcpy(&to->udh[i1], &from->udh[i1], sizeof(bcm_pkt_dnx_udh_t));
    }
}

/*
 * Function:
 *      _bcm_compat6525out_mirror_header_info_t
 * Purpose:
 *      Convert the bcm_mirror_header_info_t datatype from SDK 6.5.26+ to
 *      <=6.5.25
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525out_mirror_header_info_t(
    bcm_mirror_header_info_t *from,
    bcm_compat6525_mirror_header_info_t *to)
{
    int i1 = 0;

    sal_memcpy(&to->tm, &from->tm, sizeof(bcm_mirror_pkt_dnx_ftmh_header_t));
    _bcm_compat6525out_mirror_pkt_dnx_pp_header_t(&from->pp, &to->pp);
    for (i1 = 0; i1 < 4; i1++) {
        sal_memcpy(&to->udh[i1], &from->udh[i1], sizeof(bcm_pkt_dnx_udh_t));
    }
}

/*
 * Function:
 *      bcm_compat6525_mirror_header_info_set
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
int bcm_compat6525_mirror_header_info_set(
    int unit,
    uint32 flags,
    bcm_gport_t mirror_dest_id,
    bcm_compat6525_mirror_header_info_t *mirror_header_info)
{
    int rv = BCM_E_NONE;
    bcm_mirror_header_info_t *new_mirror_header_info = NULL;

    if (mirror_header_info != NULL) {
        new_mirror_header_info = (bcm_mirror_header_info_t *)
                     sal_alloc(sizeof(bcm_mirror_header_info_t),
                     "New mirror_header_info");
        if (new_mirror_header_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_mirror_header_info_t(mirror_header_info, new_mirror_header_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_header_info_set(unit, flags, mirror_dest_id, new_mirror_header_info);


    /* Deallocate memory*/
    sal_free(new_mirror_header_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_mirror_header_info_get
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
int bcm_compat6525_mirror_header_info_get(
    int unit,
    bcm_gport_t mirror_dest_id,
    uint32 *flags,
    bcm_compat6525_mirror_header_info_t *mirror_header_info)
{
    int rv = BCM_E_NONE;
    bcm_mirror_header_info_t *new_mirror_header_info = NULL;

    if (mirror_header_info != NULL) {
        new_mirror_header_info = (bcm_mirror_header_info_t *)
                     sal_alloc(sizeof(bcm_mirror_header_info_t),
                     "New mirror_header_info");
        if (new_mirror_header_info == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_header_info_get(unit, mirror_dest_id, flags, new_mirror_header_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6525out_mirror_header_info_t(new_mirror_header_info, mirror_header_info);

    /* Deallocate memory*/
    sal_free(new_mirror_header_info);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6525in_field_destination_match_t
 * Purpose:
 *      Convert the bcm_field_destination_match_t datatype from <=6.5.25 to
 *      SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.25 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525in_field_destination_match_t(
    bcm_compat6525_field_destination_match_t *from,
    bcm_field_destination_match_t *to)
{
    bcm_field_destination_match_t_init(to);
    to->priority = from->priority;
    to->gport = from->gport;
    to->gport_mask = from->gport_mask;
    to->mc_group = from->mc_group;
    to->mc_group_mask = from->mc_group_mask;
    to->port_encap = from->port_encap;
    to->port_encap_mask = from->port_encap_mask;
    to->color = from->color;
    to->color_mask = from->color_mask;
    to->elephant_color = from->elephant_color;
    to->elephant_color_mask = from->elephant_color_mask;
    to->elephant = from->elephant;
    to->elephant_mask = from->elephant_mask;
    to->elephant_notify = from->elephant_notify;
    to->elephant_notify_mask = from->elephant_notify_mask;
    to->int_pri = from->int_pri;
    to->int_pri_mask = from->int_pri_mask;
    to->opaque_object_2 = from->opaque_object_2;
    to->opaque_object_2_mask = from->opaque_object_2_mask;
    to->ifp_opaque_ctrl_id = from->ifp_opaque_ctrl_id;
    to->ifp_opaque_ctrl_id_mask = from->ifp_opaque_ctrl_id_mask;
    to->drop_reason = from->drop_reason;
    to->drop_reason_mask = from->drop_reason_mask;
}

/*
 * Function:
 *      _bcm_compat6525out_field_destination_match_t
 * Purpose:
 *      Convert the bcm_field_destination_match_t datatype from SDK 6.5.26+ to
 *      <=6.5.25
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525out_field_destination_match_t(
    bcm_field_destination_match_t *from,
    bcm_compat6525_field_destination_match_t *to)
{
    to->priority = from->priority;
    to->gport = from->gport;
    to->gport_mask = from->gport_mask;
    to->mc_group = from->mc_group;
    to->mc_group_mask = from->mc_group_mask;
    to->port_encap = from->port_encap;
    to->port_encap_mask = from->port_encap_mask;
    to->color = from->color;
    to->color_mask = from->color_mask;
    to->elephant_color = from->elephant_color;
    to->elephant_color_mask = from->elephant_color_mask;
    to->elephant = from->elephant;
    to->elephant_mask = from->elephant_mask;
    to->elephant_notify = from->elephant_notify;
    to->elephant_notify_mask = from->elephant_notify_mask;
    to->int_pri = from->int_pri;
    to->int_pri_mask = from->int_pri_mask;
    to->opaque_object_2 = from->opaque_object_2;
    to->opaque_object_2_mask = from->opaque_object_2_mask;
    to->ifp_opaque_ctrl_id = from->ifp_opaque_ctrl_id;
    to->ifp_opaque_ctrl_id_mask = from->ifp_opaque_ctrl_id_mask;
    to->drop_reason = from->drop_reason;
    to->drop_reason_mask = from->drop_reason_mask;
}

/*
 * Function:
 *      _bcm_compat6525in_field_destination_action_t
 * Purpose:
 *      Convert the bcm_field_destination_action_t datatype from <=6.5.25 to
 *      SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.25 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525in_field_destination_action_t(
    bcm_compat6525_field_destination_action_t *from,
    bcm_field_destination_action_t *to)
{
    bcm_field_destination_action_t_init(to);
    to->flags = from->flags;
    to->drop = from->drop;
    to->copy_to_cpu = from->copy_to_cpu;
    to->color = from->color;
    to->destination_type = from->destination_type;
    to->gport = from->gport;
    to->mc_group = from->mc_group;
    to->mirror_dest_id = from->mirror_dest_id;
    to->uc_cos = from->uc_cos;
    to->mc_cos = from->mc_cos;
    to->rqe_cos = from->rqe_cos;
    to->cpu_cos = from->cpu_cos;
}

/*
 * Function:
 *      _bcm_compat6525out_field_destination_action_t
 * Purpose:
 *      Convert the bcm_field_destination_action_t datatype from SDK 6.5.26+ to
 *      <=6.5.25
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525out_field_destination_action_t(
    bcm_field_destination_action_t *from,
    bcm_compat6525_field_destination_action_t *to)
{
    to->flags = from->flags;
    to->drop = from->drop;
    to->copy_to_cpu = from->copy_to_cpu;
    to->color = from->color;
    to->destination_type = from->destination_type;
    to->gport = from->gport;
    to->mc_group = from->mc_group;
    to->mirror_dest_id = from->mirror_dest_id;
    to->uc_cos = from->uc_cos;
    to->mc_cos = from->mc_cos;
    to->rqe_cos = from->rqe_cos;
    to->cpu_cos = from->cpu_cos;
}

/*
 * Function:
 *      bcm_compat6525_field_destination_entry_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_destination_entry_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      options - (IN) Entry operation options
 *      match - (IN) Field destination match
 *      action - (IN) Field destination action
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_field_destination_entry_add(
    int unit,
    uint32 options,
    bcm_compat6525_field_destination_match_t *match,
    bcm_compat6525_field_destination_action_t *action)
{
    int rv = BCM_E_NONE;
    bcm_field_destination_match_t *new_match = NULL;
    bcm_field_destination_action_t *new_action = NULL;

    if (match != NULL && action != NULL) {
        new_match = (bcm_field_destination_match_t *)
                     sal_alloc(sizeof(bcm_field_destination_match_t),
                     "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_field_destination_match_t(match, new_match);
        new_action = (bcm_field_destination_action_t *)
                     sal_alloc(sizeof(bcm_field_destination_action_t),
                     "New action");
        if (new_action == NULL) {
            sal_free(new_match);
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_field_destination_action_t(action, new_action);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_destination_entry_add(unit, options, new_match, new_action);


    /* Deallocate memory*/
    sal_free(new_match);
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_field_destination_entry_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_destination_entry_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      match - (INOUT) Field destination match
 *      action - (OUT) Field destination action
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_field_destination_entry_get(
    int unit,
    bcm_compat6525_field_destination_match_t *match,
    bcm_compat6525_field_destination_action_t *action)
{
    int rv = BCM_E_NONE;
    bcm_field_destination_match_t *new_match = NULL;
    bcm_field_destination_action_t *new_action = NULL;

    if (match != NULL && action != NULL) {
        new_match = (bcm_field_destination_match_t *)
                     sal_alloc(sizeof(bcm_field_destination_match_t),
                     "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_field_destination_match_t(match, new_match);
        new_action = (bcm_field_destination_action_t *)
                     sal_alloc(sizeof(bcm_field_destination_action_t),
                     "New action");
        if (new_action == NULL) {
            sal_free(new_match);
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_destination_entry_get(unit, new_match, new_action);

    /* Transform the entry from the new format to old one */
    _bcm_compat6525out_field_destination_match_t(new_match, match);
    /* Transform the entry from the new format to old one */
    _bcm_compat6525out_field_destination_action_t(new_action, action);

    /* Deallocate memory*/
    sal_free(new_match);
    sal_free(new_action);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_field_destination_entry_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_destination_entry_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      match - (IN) Field destination match
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_field_destination_entry_delete(
    int unit,
    bcm_compat6525_field_destination_match_t *match)
{
    int rv = BCM_E_NONE;
    bcm_field_destination_match_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_field_destination_match_t *)
                     sal_alloc(sizeof(bcm_field_destination_match_t),
                     "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_field_destination_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_destination_entry_delete(unit, new_match);


    /* Deallocate memory*/
    sal_free(new_match);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_field_destination_stat_attach
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_destination_stat_attach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      match - (IN) Field destination match
 *      stat_counter_id - (IN) Statistic counter ID
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_field_destination_stat_attach(
    int unit,
    bcm_compat6525_field_destination_match_t *match,
    uint32 stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_field_destination_match_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_field_destination_match_t *)
                     sal_alloc(sizeof(bcm_field_destination_match_t),
                     "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_field_destination_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_destination_stat_attach(unit, new_match, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_match);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_field_destination_stat_id_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_destination_stat_id_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      match - (IN) Field destination match
 *      stat_counter_id - (OUT) Statistic counter ID
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_field_destination_stat_id_get(
    int unit,
    bcm_compat6525_field_destination_match_t *match,
    uint32 *stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_field_destination_match_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_field_destination_match_t *)
                     sal_alloc(sizeof(bcm_field_destination_match_t),
                     "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_field_destination_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_destination_stat_id_get(unit, new_match, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_match);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_field_destination_stat_detach
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_destination_stat_detach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      match - (IN) Field destination match
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_field_destination_stat_detach(
    int unit,
    bcm_compat6525_field_destination_match_t *match)
{
    int rv = BCM_E_NONE;
    bcm_field_destination_match_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_field_destination_match_t *)
                     sal_alloc(sizeof(bcm_field_destination_match_t),
                     "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_field_destination_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_destination_stat_detach(unit, new_match);


    /* Deallocate memory*/
    sal_free(new_match);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_field_destination_flexctr_object_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_destination_flexctr_object_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      match - (IN) Field destination match
 *      value - (IN) Field destination match
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_field_destination_flexctr_object_set(
    int unit,
    bcm_compat6525_field_destination_match_t *match,
    uint32 value)
{
    int rv = BCM_E_NONE;
    bcm_field_destination_match_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_field_destination_match_t *)
                     sal_alloc(sizeof(bcm_field_destination_match_t),
                     "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_field_destination_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_destination_flexctr_object_set(unit, new_match, value);


    /* Deallocate memory*/
    sal_free(new_match);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_field_destination_flexctr_object_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_field_destination_flexctr_object_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      match - (IN) Field destination match
 *      value - (OUT)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_field_destination_flexctr_object_get(
    int unit,
    bcm_compat6525_field_destination_match_t *match,
    uint32 *value)
{
    int rv = BCM_E_NONE;
    bcm_field_destination_match_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_field_destination_match_t *)
                     sal_alloc(sizeof(bcm_field_destination_match_t),
                     "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_field_destination_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_field_destination_flexctr_object_get(unit, new_match, value);


    /* Deallocate memory*/
    sal_free(new_match);

    return rv;
}


#endif /* BCM_RPC_SUPPORT */
