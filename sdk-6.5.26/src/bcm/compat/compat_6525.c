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

/*
 * Function:
 *      _bcm_compat6525in_l3_egress_t
 * Purpose:
 *      Convert the bcm_l3_egress_t datatype from <=6.5.25 to
 *      SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.25 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525in_l3_egress_t(
    bcm_compat6525_l3_egress_t *from,
    bcm_l3_egress_t *to)
{
    int i1 = 0;

    bcm_l3_egress_t_init(to);
    to->flags = from->flags;
    to->flags2 = from->flags2;
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
    to->encap_access          = from->encap_access;
}

/*
 * Function:
 *      _bcm_compat6525out_l3_egress_t
 * Purpose:
 *      Convert the bcm_l3_egress_t datatype from SDK 6.5.26+ to
 *      <=6.5.25
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525out_l3_egress_t(
    bcm_l3_egress_t *from,
    bcm_compat6525_l3_egress_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->flags2 = from->flags2;
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
    to->encap_access          = from->encap_access;
}

/*
 * Function:
 *      bcm_compat6525_l3_egress_create
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
int bcm_compat6525_l3_egress_create(
    int unit,
    uint32 flags,
    bcm_compat6525_l3_egress_t *egr,
    bcm_if_t *if_id)
{
    int rv = BCM_E_NONE;
    bcm_l3_egress_t *new_egr = NULL;

    if (egr != NULL) {
        new_egr = (bcm_l3_egress_t *)
                   sal_alloc(sizeof(bcm_l3_egress_t), "New egr");
        if (new_egr == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_l3_egress_t(egr, new_egr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_create(unit, flags, new_egr, if_id);

    /* Transform the entry from the new format to old one */
    _bcm_compat6525out_l3_egress_t(new_egr, egr);

    /* Deallocate memory*/
    sal_free(new_egr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l3_egress_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_get.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      intf - (IN) L3 interface ID pointing to Egress object.
 *      egr - (OUT) Egress forwarding path properties.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_l3_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6525_l3_egress_t *egr)
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
    _bcm_compat6525out_l3_egress_t(new_egr, egr);

    /* Deallocate memory*/
    sal_free(new_egr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l3_egress_find
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_find.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      egr - (IN) Egress forwarding path properties.
 *      intf - (OUT) L3 interface ID pointing to Egress object.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_l3_egress_find(
    int unit,
    bcm_compat6525_l3_egress_t *egr,
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
        _bcm_compat6525in_l3_egress_t(egr, new_egr);
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
 *      bcm_compat6525_l3_egress_allocation_get
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
int bcm_compat6525_l3_egress_allocation_get(
    int unit,
    uint32 flags,
    bcm_compat6525_l3_egress_t *egr,
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
        _bcm_compat6525in_l3_egress_t(egr, new_egr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_allocation_get(unit, flags, new_egr, nof_members, if_id);

    /* Transform the entry from the new format to old one */
    _bcm_compat6525out_l3_egress_t(new_egr, egr);

    /* Deallocate memory*/
    sal_free(new_egr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_failover_egress_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_failover_egress_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN)
 *      failover_egr - (INOUT)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_failover_egress_set(
    int unit,
    bcm_if_t intf,
    bcm_compat6525_l3_egress_t *failover_egr)
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
        _bcm_compat6525in_l3_egress_t(failover_egr, new_failover_egr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_failover_egress_set(unit, intf, new_failover_egr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6525out_l3_egress_t(new_failover_egr, failover_egr);

    /* Deallocate memory */
    sal_free(new_failover_egr);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_failover_egress_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_failover_egress_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN)
 *      failover_egr - (INOUT)
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_failover_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6525_l3_egress_t *failover_egr)
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
        _bcm_compat6525in_l3_egress_t(failover_egr, new_failover_egr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_failover_egress_get(unit, intf, new_failover_egr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6525out_l3_egress_t(new_failover_egr, failover_egr);

    /* Deallocate memory */
    sal_free(new_failover_egr);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6525in_l3_egress_ecmp_t
 *
 * Purpose:
 *      Convert the bcm_l3_egress_ecmp_t datatype from <= 6.5.25 to SDK 6.5.26+.
 *
 * Parameters:
 *      from - (IN) <= SDK 6.5.25 version of the datatype.
 *      to   - (OUT) SDK 6.5.26+ version of the datatype.
 *
 * Returns:
 *      Nothing.
 */
STATIC void
_bcm_compat6525in_l3_egress_ecmp_t(bcm_compat6525_l3_egress_ecmp_t *from,
                                   bcm_l3_egress_ecmp_t *to)
{
    if (from && to) {
        bcm_l3_egress_ecmp_t_init(to);
        to->flags = from->flags;
        to->ecmp_intf = from->ecmp_intf;
        to->max_paths = from->max_paths;
        to->ecmp_group_flags = from->ecmp_group_flags;
        to->dynamic_mode = from->dynamic_mode;
        to->dynamic_size = from->dynamic_size;
        to->dynamic_load_exponent = from->dynamic_load_exponent;
        sal_memcpy(&to->dgm,
                   &from->dgm,
                   sizeof(bcm_l3_dgm_t));
        to->stat_id = from->stat_id;
        to->stat_pp_profile = from->stat_pp_profile;
        to->rpf_mode = from->rpf_mode;
        sal_memcpy(&to->tunnel_priority,
                   &from->tunnel_priority,
                   sizeof(bcm_l3_tunnel_priority_info_t));
        to->rh_random_seed          = from->rh_random_seed;
        to->user_profile            = from->user_profile;
        to->failover_id             = from->failover_id;
        to->failover_size           = from->failover_size;
        to->failover_base_egress_id = from->failover_base_egress_id;
        to->failover_lb_mode        = from->failover_lb_mode;
    }
    return;
}

/*
 * Function:
 *      _bcm_compat6525out_l3_egress_ecmp_t
 *
 * Purpose:
 *      Convert the bcm_l3_egress_ecmp_t datatype from SDK 6.5.26+ to <=6.5.25.
 *
 * Parameters:
 *      from - (IN) SDK 6.5.26+ version of the datatype.
 *      to   - (OUT) <= SDK 6.5.25 version of the datatype.
 *
 * Returns:
 *      Nothing.
 */
STATIC void
_bcm_compat6525out_l3_egress_ecmp_t(bcm_l3_egress_ecmp_t *from,
                                    bcm_compat6525_l3_egress_ecmp_t *to)
{
    if (from && to) {
        to->flags = from->flags;
        to->ecmp_intf = from->ecmp_intf;
        to->max_paths = from->max_paths;
        to->ecmp_group_flags = from->ecmp_group_flags;
        to->dynamic_mode = from->dynamic_mode;
        to->dynamic_size = from->dynamic_size;
        to->dynamic_load_exponent = from->dynamic_load_exponent;
        sal_memcpy(&to->dgm,
                   &from->dgm,
                   sizeof(bcm_l3_dgm_t));
        to->stat_id = from->stat_id;
        to->stat_pp_profile = from->stat_pp_profile;
        to->rpf_mode = from->rpf_mode;
        sal_memcpy(&to->tunnel_priority,
                   &from->tunnel_priority,
                   sizeof(bcm_l3_tunnel_priority_info_t));
        to->rh_random_seed          = from->rh_random_seed;
        to->user_profile            = from->user_profile;
        to->failover_id             = from->failover_id;
        to->failover_size           = from->failover_size;
        to->failover_base_egress_id = from->failover_base_egress_id;
        to->failover_lb_mode        = from->failover_lb_mode;
    }
    return;
}

/*
 * Function:
 *      _bcm_compat6525in_l3_egress_ecmp_resilient_entry_t
 *
 * Purpose:
 *      Convert the bcm_l3_egress_ecmp_resilient_entry_t datatype from <= 6.5.25
 *      to SDK 6.5.26+.
 *
 * Parameters:
 *      from - (IN) <= SDK 6.5.25 version of the datatype.
 *      to   - (OUT) SDK 6.5.26+ version of the datatype.
 *
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525in_l3_egress_ecmp_resilient_entry_t(
    bcm_compat6525_l3_egress_ecmp_resilient_entry_t *from,
    bcm_l3_egress_ecmp_resilient_entry_t *to)
{
    if (from && to) {
        to->hash_key = from->hash_key;
        (void)_bcm_compat6525in_l3_egress_ecmp_t(&from->ecmp, &to->ecmp);
        to->intf = from->intf;
    }
    return;
}

/*
 * Function:
 *      bcm_compat6525_l3_ecmp_create
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_ecmp_create.
 *
 * Parameters:
 *      unit              - (IN) BCM device number.
 *      options           - (IN) L3_ECMP_O_xxx flags.
 *      ecmp_info         - (INOUT) ECMP group info.
 *      ecmp_member_count - (IN) Number of elements in ecmp_member_array.
 *      ecmp_member_array - (IN) Member array of egress forwarding objects.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6525_l3_ecmp_create(int unit,
                              uint32 options,
                              bcm_compat6525_l3_egress_ecmp_t *ecmp_info,
                              int ecmp_member_count,
                              bcm_l3_ecmp_member_t *ecmp_member_array)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp_info == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6525L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6525_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6525in_l3_egress_ecmp_t(ecmp_info, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_ecmp_create(unit, options, new, ecmp_member_count,
                            ecmp_member_array);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as ecmp_info is of INOUT parameter type for this API.
         * - bcm_l3_egress_ecmp_t -> bcm_compat6525_l3_egress_ecmp_t
         */
        (void)_bcm_compat6525out_l3_egress_ecmp_t(new, ecmp_info);
    }
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l3_ecmp_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_ecmp_get.
 *
 * Parameters:
 *      unit              - (IN) Unit number.
 *      ecmp_info         - (INOUT) ECMP group info.
 *      ecmp_member_size  - (IN) Size of allocated entries in ecmp_member_array.
 *      ecmp_member_array - (OUT) Member array of Egress forwarding objects.
 *      ecmp_member_count - (OUT) Number of entries of ecmp_member_count
 *                                actually filled in. This will be a value less
 *                                than or equal to the value passed in as
 *                                ecmp_member_size unless ecmp_member_size is 0.
 *                                If ecmp_member_size is 0 then
 *                                ecmp_member_array is ignored and
 *                                ecmp_member_count is filled in with the number
 *                                of entries that would have been filled into
 *                                ecmp_member_array if ecmp_member_size was
 *                                arbitrarily large.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6525_l3_ecmp_get(int unit,
                           bcm_compat6525_l3_egress_ecmp_t *ecmp_info,
                           int ecmp_member_size,
                           bcm_l3_ecmp_member_t *ecmp_member_array,
                           int *ecmp_member_count)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp_info == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6525L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6525_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6525in_l3_egress_ecmp_t(ecmp_info, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_ecmp_get(unit, new, ecmp_member_size, ecmp_member_array,
                         ecmp_member_count);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as ecmp_info is of INOUT parameter type for this API.
         * - bcm_l3_egress_ecmp_t -> bcm_compat6525_l3_egress_ecmp_t
         */
        (void)_bcm_compat6525out_l3_egress_ecmp_t(new, ecmp_info);
    }
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l3_ecmp_find
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_ecmp_find.
 *
 * Parameters:
 *      unit              - (IN) BCM device number.
 *      ecmp_member_count - (IN) Number of member in ecmp_member_array.
 *      ecmp_member_array - (IN) Member array of egress forwarding objects.
 *      ecmp_info         - (OUT) ECMP group info.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6525_l3_ecmp_find(int unit,
                            int ecmp_member_count,
                            bcm_l3_ecmp_member_t *ecmp_member_array,
                            bcm_compat6525_l3_egress_ecmp_t *ecmp_info)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp_info == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6525L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6525_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6525in_l3_egress_ecmp_t(ecmp_info, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_ecmp_find(unit, ecmp_member_count, ecmp_member_array, new);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as ecmp_info is of OUT parameter type for this API.
         * - bcm_l3_egress_ecmp_t -> bcm_compat6525_l3_egress_ecmp_t
         */
        (void)_bcm_compat6525out_l3_egress_ecmp_t(new, ecmp_info);
    }
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l3_egress_ecmp_create
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_create.
 *
 * Parameters:
 *      unit       - (IN) BCM device number.
 *      ecmp       - (INOUT) ECMP group info.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6525_l3_egress_ecmp_create(int unit,
                                     bcm_compat6525_l3_egress_ecmp_t *ecmp,
                                     int intf_count,
                                     bcm_if_t *intf_array)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6525L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6525_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6525in_l3_egress_ecmp_t(ecmp, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_create(unit, new, intf_count, intf_array);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as ecmp is of INOUT parameter type for this API.
         * - bcm_l3_egress_ecmp_t -> bcm_compat6525_l3_egress_ecmp_t
         */
        (void)_bcm_compat6525out_l3_egress_ecmp_t(new, ecmp);
    }
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l3_egress_ecmp_destroy
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_destroy.
 *
 * Parameters:
 *      unit - (IN) BCM device number.
 *      ecmp - (IN) ECMP group info.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6525_l3_egress_ecmp_destroy(int unit,
                                      bcm_compat6525_l3_egress_ecmp_t *ecmp)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6525L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6525_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6525in_l3_egress_ecmp_t(ecmp, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_destroy(unit, new);
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l3_egress_ecmp_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_get.
 *
 * Parameters:
 *      unit       - (IN) BCM device number.
 *      ecmp       - (INOUT) ECMP group info.
 *      intf_size  - (IN) Size of allocated entries in intf_array.
 *      intf_array - (OUT) Array of Egress forwarding objects.
 *      intf_count - (OUT) Number of entries of intf_count actually filled in.
 *                         This will be a value less than or equal to the value
 *                         passed in as intf_size unless intf_size is 0. If
 *                         intf_size is 0 then intf_array is ignored and
 *                         intf_count is filled in with the number of entries
 *                         that would have been filled into intf_array if
 *                         intf_size was arbitrarily large.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6525_l3_egress_ecmp_get(int unit,
                                  bcm_compat6525_l3_egress_ecmp_t *ecmp,
                                  int intf_size,
                                  bcm_if_t *intf_array,
                                  int *intf_count)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6525L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6525_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6525in_l3_egress_ecmp_t(ecmp, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_get(unit, new, intf_size, intf_array, intf_count);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as ecmp is of INOUT parameter type for this API.
         * - bcm_l3_egress_ecmp_t -> bcm_compat6525_l3_egress_ecmp_t
         */
        (void)_bcm_compat6525out_l3_egress_ecmp_t(new, ecmp);
    }
    if (new) {
        sal_free(new);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l3_egress_ecmp_add
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_add.
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      ecmp - (IN) ECMP group info.
 *      intf - (IN) L3 interface ID pointing to Egress forwarding object.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6525_l3_egress_ecmp_add(int unit,
                                  bcm_compat6525_l3_egress_ecmp_t *ecmp,
                                  bcm_if_t intf)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6525L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6525_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6525in_l3_egress_ecmp_t(ecmp, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_add(unit, new, intf);
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l3_egress_ecmp_delete
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_delete.
 *
 * Parameters:
 *      unit - (IN) Unit number.
 *      ecmp - (IN) ECMP group info.
 *      intf - (IN) L3 interface ID pointing to Egress forwarding object.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6525_l3_egress_ecmp_delete(int unit,
                                     bcm_compat6525_l3_egress_ecmp_t *ecmp,
                                     bcm_if_t intf)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6525L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6525_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6525in_l3_egress_ecmp_t(ecmp, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_delete(unit, new, intf);
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l3_egress_ecmp_find
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_find.
 *
 * Parameters:
 *      unit       - (IN) Unit number.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 *      ecmp       - (OUT) ECMP group info.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6525_l3_egress_ecmp_find(int unit,
                                   int intf_count,
                                   bcm_if_t *intf_array,
                                   bcm_compat6525_l3_egress_ecmp_t *ecmp)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6525L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /* Initialize the structure members. */
    bcm_l3_egress_ecmp_t_init(new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_find(unit, intf_count, intf_array, new);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as ecmp is of OUT parameter type for this API.
         * - bcm_l3_egress_ecmp_t -> bcm_compat6525_l3_egress_ecmp_t
         */
        (void)_bcm_compat6525out_l3_egress_ecmp_t(new, ecmp);
    }
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l3_egress_ecmp_tunnel_priority_set
 *
 * Purpose:
 *      Compatibility function for RPC call to
 *      bcm_l3_egress_ecmp_tunnel_priority_set.
 *
 * Parameters:
 *      unit       - (IN) BCM device number.
 *      ecmp       - (IN) ECMP group info.
 *      intf_count - (IN) Number of elements in intf_array.
 *      intf_array - (IN) Array of Egress forwarding objects.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6525_l3_egress_ecmp_tunnel_priority_set(
    int unit,
    bcm_compat6525_l3_egress_ecmp_t *ecmp,
    int intf_count,
    bcm_if_t *intf_array)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6525L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6525_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6525in_l3_egress_ecmp_t(ecmp, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_tunnel_priority_set(unit, new, intf_count,
                                                intf_array);
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l3_egress_ecmp_resilient_replace
 *
 * Purpose:
 *      Compatibility function for RPC call to
 *      bcm_l3_egress_ecmp_resilient_replace.
 *
 * Parameters:
 *      unit          - (IN) Unit number.
 *      flags         - (IN) BCM_TRUNK_RESILIENT flags.
 *      match_entry   - (IN) Matching criteria
 *      num_entries   - (OUT) Number of entries matched.
 *      replace_entry - (IN) Replacing entry when the action is replace.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6525_l3_egress_ecmp_resilient_replace(
    int unit,
    uint32 flags,
    bcm_compat6525_l3_egress_ecmp_resilient_entry_t *match_entry,
    int *num_entries,
    bcm_compat6525_l3_egress_ecmp_resilient_entry_t *replace_entry)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_resilient_entry_t *new_match = NULL;
    bcm_l3_egress_ecmp_resilient_entry_t *new_replace = NULL;

    /* Validate input parameters for NULL ptr. */
    if (match_entry == NULL || replace_entry == NULL) {
        return (BCM_E_PARAM);
    }

    new_match = (bcm_l3_egress_ecmp_resilient_entry_t *)
                    sal_alloc(sizeof(*new_match),
                              "bcmcompat6525L3EgrEcmpRhNewMatchEnt");
    if (new_match == NULL) {
        return (BCM_E_MEMORY);
    }
    sal_memset(new_match, 0, sizeof(*new_match));
    new_replace = (bcm_l3_egress_ecmp_resilient_entry_t *)
                        sal_alloc(sizeof(*new_replace),
                                  "bcmcompat6525L3EgrEcmpRhNewReplaceEnt");
    if (new_replace == NULL) {
        sal_free(new_match);
        return (BCM_E_MEMORY);
    }
    sal_memset(new_replace, 0, sizeof(*new_replace));
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6525_l3_egress_ecmp_resilient_entry_t
     *      -> bcm_l3_egress_ecmp_resilient_entry_t.
     */
    (void)_bcm_compat6525in_l3_egress_ecmp_resilient_entry_t(match_entry,
                                                             new_match);
    (void)_bcm_compat6525in_l3_egress_ecmp_resilient_entry_t(replace_entry,
                                                             new_replace);
    rv = bcm_l3_egress_ecmp_resilient_replace(unit, flags, new_match,
                                              num_entries, new_replace);
    if (new_match) {
        sal_free(new_match);
    }
    if (new_replace) {
        sal_free(new_replace);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l3_egress_ecmp_resilient_add
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_egress_ecmp_resilient_add.
 *
 * Parameters:
 *      unit  - (IN) Unit number.
 *      flags - (IN) BCM_L3_ECMP_RESILIENT flags.
 *      entry - (IN) Entry criteria
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6525_l3_egress_ecmp_resilient_add(
    int unit,
    uint32 flags,
    bcm_compat6525_l3_egress_ecmp_resilient_entry_t *entry)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_resilient_entry_t *new = NULL;

    /* Validate input parameters for NULL ptr. */
    if (entry == NULL) {
        return (BCM_E_PARAM);
    }

    new = (bcm_l3_egress_ecmp_resilient_entry_t *)
                sal_alloc(sizeof(*new), "bcmcompat6525L3EgrEcmpRhNewEnt");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    sal_memset(new, 0, sizeof(*new));
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6525_l3_egress_ecmp_resilient_entry_t
     *      -> bcm_l3_egress_ecmp_resilient_entry_t.
     */
    (void)_bcm_compat6525in_l3_egress_ecmp_resilient_entry_t(entry, new);
    rv = bcm_l3_egress_ecmp_resilient_add(unit, flags, new);
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6525_l3_egress_ecmp_resilient_delete
 *
 * Purpose:
 *      Compatibility function for RPC call to
 *      bcm_l3_egress_ecmp_resilient_delete.
 *
 * Parameters:
 *      unit  - (IN) Unit number.
 *      flags - (IN) BCM_L3_ECMP_RESILIENT flags.
 *      entry - (IN) Entry criteria
 *
 * Returns:
 *      BCM_E_NONE - Success.
 */
int
bcm_compat6525_l3_egress_ecmp_resilient_delete(
    int unit,
    uint32 flags,
    bcm_compat6525_l3_egress_ecmp_resilient_entry_t *entry)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_resilient_entry_t *new = NULL;

    /* Validate input parameters for NULL ptr. */
    if (entry == NULL) {
        return (BCM_E_PARAM);
    }

    new = (bcm_l3_egress_ecmp_resilient_entry_t *)
                sal_alloc(sizeof(*new), "bcmcompat6525L3EgrEcmpRhNewEnt");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    sal_memset(new, 0, sizeof(*new));
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6525_l3_egress_ecmp_resilient_entry_t
     *      -> bcm_l3_egress_ecmp_resilient_entry_t.
     */
    (void)_bcm_compat6525in_l3_egress_ecmp_resilient_entry_t(entry, new);
    rv = bcm_l3_egress_ecmp_resilient_delete(unit, flags, new);
    if (new) {
        sal_free(new);
    }
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
 *      _bcm_compat6525in_mirror_sample_profile_t
 * Purpose:
 *      Convert the bcm_mirror_sample_profile_t datatype from <=6.5.25 to
 *      SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.25 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525in_mirror_sample_profile_t(
    bcm_compat6525_mirror_sample_profile_t *from,
    bcm_mirror_sample_profile_t *to)
{
    bcm_mirror_sample_profile_t_init(to);
    to->enable = from->enable;
    to->rate = from->rate;
    to->pool_count_enable = from->pool_count_enable;
    to->sample_count_enable = from->sample_count_enable;
    to->trace_enable = from->trace_enable;
    to->mirror_mode = from->mirror_mode;
    to->copy_to_cpu = from->copy_to_cpu;
    to->mirror = from->mirror;
}

/*
 * Function:
 *      _bcm_compat6525out_mirror_sample_profile_t
 * Purpose:
 *      Convert the bcm_mirror_sample_profile_t datatype from SDK 6.5.26+ to
 *      <=6.5.25
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525out_mirror_sample_profile_t(
    bcm_mirror_sample_profile_t *from,
    bcm_compat6525_mirror_sample_profile_t *to)
{
    to->enable = from->enable;
    to->rate = from->rate;
    to->pool_count_enable = from->pool_count_enable;
    to->sample_count_enable = from->sample_count_enable;
    to->trace_enable = from->trace_enable;
    to->mirror_mode = from->mirror_mode;
    to->copy_to_cpu = from->copy_to_cpu;
    to->mirror = from->mirror;
}

/*
 * Function:
 *      bcm_compat6525_mirror_sample_profile_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_sample_profile_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      type - (IN) Sample type
 *      profile_id - (IN) Sample profile id
 *      profile - (IN) Sample profile entry
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_mirror_sample_profile_set(
    int unit,
    bcm_mirror_sample_type_t type,
    int profile_id,
    bcm_compat6525_mirror_sample_profile_t *profile)
{
    int rv = BCM_E_NONE;
    bcm_mirror_sample_profile_t *new_profile = NULL;

    if (profile != NULL) {
        new_profile = (bcm_mirror_sample_profile_t *)
                     sal_alloc(sizeof(bcm_mirror_sample_profile_t),
                     "New profile");
        if (new_profile == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_mirror_sample_profile_t(profile, new_profile);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_sample_profile_set(unit, type, profile_id, new_profile);


    /* Deallocate memory*/
    sal_free(new_profile);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_mirror_sample_profile_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_mirror_sample_profile_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      type - (IN) Sample type
 *      profile_id - (IN) Sample profile id
 *      profile - (OUT) Sample profile entry
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_mirror_sample_profile_get(
    int unit,
    bcm_mirror_sample_type_t type,
    int profile_id,
    bcm_compat6525_mirror_sample_profile_t *profile)
{
    int rv = BCM_E_NONE;
    bcm_mirror_sample_profile_t *new_profile = NULL;

    if (profile != NULL) {
        new_profile = (bcm_mirror_sample_profile_t *)
                     sal_alloc(sizeof(bcm_mirror_sample_profile_t),
                     "New profile");
        if (new_profile == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_mirror_sample_profile_get(unit, type, profile_id, new_profile);

    /* Transform the entry from the new format to old one */
    _bcm_compat6525out_mirror_sample_profile_t(new_profile, profile);

    /* Deallocate memory*/
    sal_free(new_profile);

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

/*
 * Function:
 *      _bcm_compat6525in_policer_config_t
 * Purpose:
 *      Convert the bcm_policer_config_t datatype from <=6.5.25 to
 *      SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.25 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525in_policer_config_t(
    bcm_compat6525_policer_config_t *from,
    bcm_policer_config_t *to)
{
    to->flags = from->flags;
    to->mode = from->mode;
    to->ckbits_sec = from->ckbits_sec;
    to->cbits_sec_lower = from->cbits_sec_lower;
    to->max_ckbits_sec = from->max_ckbits_sec;
    to->max_cbits_sec_lower = from->max_cbits_sec_lower;
    to->ckbits_burst = from->ckbits_burst;
    to->cbits_burst_lower = from->cbits_burst_lower;
    to->pkbits_sec = from->pkbits_sec;
    to->pbits_sec_lower = from->pbits_sec_lower;
    to->max_pkbits_sec = from->max_pkbits_sec;
    to->max_pbits_sec_lower = from->max_pbits_sec_lower;
    to->pkbits_burst = from->pkbits_burst;
    to->pbits_burst_lower = from->pbits_burst_lower;
    to->kbits_current = from->kbits_current;
    to->bits_current_lower = from->bits_current_lower;
    to->action_id = from->action_id;
    to->sharing_mode = from->sharing_mode;
    to->entropy_id = from->entropy_id;
    to->pool_id = from->pool_id;
    to->bucket_width = from->bucket_width;
    to->token_gran = from->token_gran;
    to->bucket_init_val = from->bucket_init_val;
    to->bucket_rollover_val = from->bucket_rollover_val;
    to->core_id = from->core_id;
    to->ncoflow_policer_id = from->ncoflow_policer_id;
    to->npoflow_policer_id = from->npoflow_policer_id;
    to->actual_ckbits_sec = from->actual_ckbits_sec;
    to->actual_cbits_sec_lower = from->actual_cbits_sec_lower;
    to->actual_ckbits_burst = from->actual_ckbits_burst;
    to->actual_cbits_burst_lower = from->actual_cbits_burst_lower;
    to->actual_pkbits_sec = from->actual_pkbits_sec;
    to->actual_pbits_sec_lower = from->actual_pbits_sec_lower;
    to->actual_pkbits_burst = from->actual_pkbits_burst;
    to->actual_pbits_burst_lower = from->actual_pbits_burst_lower;
    to->average_pkt_size = from->average_pkt_size;
    to->mark_drop_as_black = from->mark_drop_as_black;
    to->color_resolve_profile = from->color_resolve_profile;
}

/*
 * Function:
 *      _bcm_compat6525out_policer_config_t
 * Purpose:
 *      Convert the bcm_policer_config_t datatype from SDK 6.5.26+ to
 *      <=6.5.25
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525out_policer_config_t(
    bcm_policer_config_t *from,
    bcm_compat6525_policer_config_t *to)
{
    to->flags = from->flags;
    to->mode = from->mode;
    to->ckbits_sec = from->ckbits_sec;
    to->cbits_sec_lower = from->cbits_sec_lower;
    to->max_ckbits_sec = from->max_ckbits_sec;
    to->max_cbits_sec_lower = from->max_cbits_sec_lower;
    to->ckbits_burst = from->ckbits_burst;
    to->cbits_burst_lower = from->cbits_burst_lower;
    to->pkbits_sec = from->pkbits_sec;
    to->pbits_sec_lower = from->pbits_sec_lower;
    to->max_pkbits_sec = from->max_pkbits_sec;
    to->max_pbits_sec_lower = from->max_pbits_sec_lower;
    to->pkbits_burst = from->pkbits_burst;
    to->pbits_burst_lower = from->pbits_burst_lower;
    to->kbits_current = from->kbits_current;
    to->bits_current_lower = from->bits_current_lower;
    to->action_id = from->action_id;
    to->sharing_mode = from->sharing_mode;
    to->entropy_id = from->entropy_id;
    to->pool_id = from->pool_id;
    to->bucket_width = from->bucket_width;
    to->token_gran = from->token_gran;
    to->bucket_init_val = from->bucket_init_val;
    to->bucket_rollover_val = from->bucket_rollover_val;
    to->core_id = from->core_id;
    to->ncoflow_policer_id = from->ncoflow_policer_id;
    to->npoflow_policer_id = from->npoflow_policer_id;
    to->actual_ckbits_sec = from->actual_ckbits_sec;
    to->actual_cbits_sec_lower = from->actual_cbits_sec_lower;
    to->actual_ckbits_burst = from->actual_ckbits_burst;
    to->actual_cbits_burst_lower = from->actual_cbits_burst_lower;
    to->actual_pkbits_sec = from->actual_pkbits_sec;
    to->actual_pbits_sec_lower = from->actual_pbits_sec_lower;
    to->actual_pkbits_burst = from->actual_pkbits_burst;
    to->actual_pbits_burst_lower = from->actual_pbits_burst_lower;
    to->average_pkt_size = from->average_pkt_size;
    to->mark_drop_as_black = from->mark_drop_as_black;
    to->color_resolve_profile = from->color_resolve_profile;
}

/*
 * Function:
 *      bcm_compat6525_policer_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_policer_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      pol_cfg - (IN) Pointer to policer config structure.
 *      policer_id - (OUT) Policer ID.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_policer_create(
    int unit,
    bcm_compat6525_policer_config_t *pol_cfg,
    bcm_policer_t *policer_id)
{
    int rv = BCM_E_NONE;
    bcm_policer_config_t *new_pol_cfg = NULL;

    if (pol_cfg != NULL) {
        new_pol_cfg = (bcm_policer_config_t *)
                     sal_alloc(sizeof(bcm_policer_config_t),
                     "New policer config");
        if (new_pol_cfg == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6525in_policer_config_t(pol_cfg, new_pol_cfg);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_policer_create(unit, new_pol_cfg, policer_id);

    if (new_pol_cfg != NULL) {
        _bcm_compat6525out_policer_config_t(new_pol_cfg, pol_cfg);
        sal_free(new_pol_cfg);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_policer_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_policer_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      policer_id - (IN) Policer ID.
 *      pol_cfg - (IN) Pointer to policer config structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_policer_set(
    int unit,
    bcm_policer_t policer_id,
    bcm_compat6525_policer_config_t *pol_cfg)
{
    int rv = BCM_E_NONE;
    bcm_policer_config_t *new_pol_cfg = NULL;

    if (pol_cfg != NULL) {
        new_pol_cfg = (bcm_policer_config_t *)
                     sal_alloc(sizeof(bcm_policer_config_t),
                     "New policer config");
        if (new_pol_cfg == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6525in_policer_config_t(pol_cfg, new_pol_cfg);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_policer_set(unit, policer_id, new_pol_cfg);

    if (new_pol_cfg != NULL) {
        _bcm_compat6525out_policer_config_t(new_pol_cfg, pol_cfg);
        sal_free(new_pol_cfg);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_policer_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_policer_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      policer_id - (IN) Policer ID.
 *      pol_cfg - (OUT) Pointer to policer config structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_policer_get(
    int unit,
    bcm_policer_t policer_id,
    bcm_compat6525_policer_config_t *pol_cfg)
{
    int rv = BCM_E_NONE;
    bcm_policer_config_t *new_pol_cfg = NULL;

    if (pol_cfg != NULL) {
        new_pol_cfg = (bcm_policer_config_t *)
                     sal_alloc(sizeof(bcm_policer_config_t),
                     "New policer config");
        if (new_pol_cfg == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6525in_policer_config_t(pol_cfg, new_pol_cfg);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_policer_get(unit, policer_id, new_pol_cfg);

    if (new_pol_cfg != NULL) {
        _bcm_compat6525out_policer_config_t(new_pol_cfg, pol_cfg);
        sal_free(new_pol_cfg);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6525in_cosq_bst_profile_t
 * Purpose:
 *      Convert the bcm_cosq_bst_profile_t datatype from <=6.5.25 to
 *      SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.25 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525in_cosq_bst_profile_t(
    bcm_compat6525_cosq_bst_profile_t *from,
    bcm_cosq_bst_profile_t *to)
{
    bcm_cosq_bst_profile_t_init(to);
    to->byte = from->byte;
}

/*
 * Function:
 *      _bcm_compat6525out_cosq_bst_profile_t
 * Purpose:
 *      Convert the bcm_cosq_bst_profile_t datatype from SDK 6.5.26+ to
 *      <=6.5.25
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525out_cosq_bst_profile_t(
    bcm_cosq_bst_profile_t *from,
    bcm_compat6525_cosq_bst_profile_t *to)
{
    to->byte = from->byte;
}

/*
 * Function:
 *      bcm_compat6525_cosq_bst_profile_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_cosq_bst_profile_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      gport - (IN) 
 *      cosq - (IN) Cosq object offset identifier
 *      bid - (IN) BST stat ID to identify the COSQ resource/object
 *      profile - (IN) BST profile configuration
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_cosq_bst_profile_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_bst_stat_id_t bid,
    bcm_compat6525_cosq_bst_profile_t *profile)
{
    int rv = BCM_E_NONE;
    bcm_cosq_bst_profile_t *new_profile = NULL;

    if (profile != NULL) {
        new_profile = (bcm_cosq_bst_profile_t *)
                     sal_alloc(sizeof(bcm_cosq_bst_profile_t),
                     "New profile");
        if (new_profile == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_cosq_bst_profile_t(profile, new_profile);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_cosq_bst_profile_set(unit, gport, cosq, bid, new_profile);


    /* Deallocate memory*/
    sal_free(new_profile);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_cosq_bst_profile_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_cosq_bst_profile_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      gport - (IN) 
 *      cosq - (IN) Cosq object offset identifier
 *      bid - (IN) BST stat ID to identify the COSQ resource/object
 *      profile - (OUT) BST profile configuration
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_cosq_bst_profile_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_bst_stat_id_t bid,
    bcm_compat6525_cosq_bst_profile_t *profile)
{
    int rv = BCM_E_NONE;
    bcm_cosq_bst_profile_t *new_profile = NULL;

    if (profile != NULL) {
        new_profile = (bcm_cosq_bst_profile_t *)
                     sal_alloc(sizeof(bcm_cosq_bst_profile_t),
                     "New profile");
        if (new_profile == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_cosq_bst_profile_get(unit, gport, cosq, bid, new_profile);

    /* Transform the entry from the new format to old one */
    _bcm_compat6525out_cosq_bst_profile_t(new_profile, profile);

    /* Deallocate memory*/
    sal_free(new_profile);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_cosq_bst_multi_profile_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_cosq_bst_multi_profile_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      gport - (IN) 
 *      cosq - (IN) Cosq object offset identifier
 *      bid - (IN) BST stat ID to identify the COSQ resource/object
 *      array_size - (IN) Max size of array
 *      profile_array - (IN) (for "_set") Array of BST profile configuration<br>(for "_get") Array of BST profile configuration
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_cosq_bst_multi_profile_set(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_bst_stat_id_t bid,
    int array_size,
    bcm_compat6525_cosq_bst_profile_t *profile_array)
{
    int rv = BCM_E_NONE;
    bcm_cosq_bst_profile_t *new_profile_array = NULL;
    int i = 0;

    if (profile_array != NULL && array_size > 0) {
        new_profile_array = (bcm_cosq_bst_profile_t *)
                     sal_alloc(array_size * sizeof(bcm_cosq_bst_profile_t),
                     "New profile_array");
        if (new_profile_array == NULL) {
            return BCM_E_MEMORY;
        }
        for (i = 0; i < array_size; i++) {
            /* Transform the entry from the old format to new one */
            _bcm_compat6525in_cosq_bst_profile_t(&profile_array[i], &new_profile_array[i]);
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_cosq_bst_multi_profile_set(unit, gport, cosq, bid, array_size, new_profile_array);


    /* Deallocate memory*/
    sal_free(new_profile_array);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_cosq_bst_multi_profile_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_cosq_bst_multi_profile_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      gport - (IN) 
 *      cosq - (IN) Cosq object offset identifier
 *      bid - (IN) BST stat ID to identify the COSQ resource/object
 *      array_size - (IN) Max size of array
 *      profile_array - (OUT) (for "_set") Array of BST profile configuration<br>(for "_get") Array of BST profile configuration
 *      count - (OUT) (for "_get") Actual size of the profile array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_cosq_bst_multi_profile_get(
    int unit,
    bcm_gport_t gport,
    bcm_cos_queue_t cosq,
    bcm_bst_stat_id_t bid,
    int array_size,
    bcm_compat6525_cosq_bst_profile_t *profile_array,
    int *count)
{
    int rv = BCM_E_NONE;
    bcm_cosq_bst_profile_t *new_profile_array = NULL;
    int i = 0;

    if (profile_array != NULL && array_size > 0) {
        new_profile_array = (bcm_cosq_bst_profile_t *)
                     sal_alloc(array_size * sizeof(bcm_cosq_bst_profile_t),
                     "New profile_array");
        if (new_profile_array == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_cosq_bst_multi_profile_get(unit, gport, cosq, bid, array_size, new_profile_array, count);

    for (i = 0; i < array_size; i++) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6525out_cosq_bst_profile_t(&new_profile_array[i], &profile_array[i]);
    }

    /* Deallocate memory*/
    sal_free(new_profile_array);

    return rv;
}


/*
 * Function:
 *      _bcm_compat6525in_multicast_encap_t
 * Purpose:
 *      Convert the bcm_multicast_encap_t datatype from <=6.5.25 to
 *      SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.25 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525in_multicast_encap_t(
    bcm_compat6525_multicast_encap_t *from,
    bcm_multicast_encap_t *to)
{
    bcm_multicast_encap_t_init(to);
    to->encap_type = from->encap_type;
    to->ul_egress_if = from->ul_egress_if;
    to->l3_intf = from->l3_intf;
    to->port_id = from->port_id;
}

/*
 * Function:
 *      _bcm_compat6525out_multicast_encap_t
 * Purpose:
 *      Convert the bcm_multicast_encap_t datatype from SDK 6.5.26+ to
 *      <=6.5.25
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525out_multicast_encap_t(
    bcm_multicast_encap_t *from,
    bcm_compat6525_multicast_encap_t *to)
{
    to->encap_type = from->encap_type;
    to->ul_egress_if = from->ul_egress_if;
    to->l3_intf = from->l3_intf;
    to->port_id = from->port_id;
}

/*
 * Function:
 *      bcm_compat6525_multicast_encap_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_multicast_encap_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mc_encap - (IN) Multicast encapsualtion information.
 *      encap_id - (OUT) Returned encapsulation ID
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_multicast_encap_create(
    int unit,
    bcm_compat6525_multicast_encap_t *mc_encap,
    bcm_if_t *encap_id)
{
    int rv = BCM_E_NONE;
    bcm_multicast_encap_t *new_mc_encap = NULL;

    if (mc_encap != NULL) {
        new_mc_encap = (bcm_multicast_encap_t *)
                     sal_alloc(sizeof(bcm_multicast_encap_t),
                     "New mc_encap");
        if (new_mc_encap == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_multicast_encap_t(mc_encap, new_mc_encap);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_multicast_encap_create(unit, new_mc_encap, encap_id);


    /* Deallocate memory*/
    sal_free(new_mc_encap);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_multicast_encap_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_multicast_encap_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      encap_id - (IN) Encapsulation ID.
 *      mc_encap - (OUT) Returned multicast encapsualtion info.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_multicast_encap_get(
    int unit,
    bcm_if_t encap_id,
    bcm_compat6525_multicast_encap_t *mc_encap)
{
    int rv = BCM_E_NONE;
    bcm_multicast_encap_t *new_mc_encap = NULL;

    if (mc_encap != NULL) {
        new_mc_encap = (bcm_multicast_encap_t *)
                     sal_alloc(sizeof(bcm_multicast_encap_t),
                     "New mc_encap");
        if (new_mc_encap == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_multicast_encap_get(unit, encap_id, new_mc_encap);

    /* Transform the entry from the new format to old one */
    _bcm_compat6525out_multicast_encap_t(new_mc_encap, mc_encap);

    /* Deallocate memory*/
    sal_free(new_mc_encap);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6525in_switch_ep_recirc_profile_t
 * Purpose:
 *      Convert the bcm_switch_ep_recirc_profile_t datatype from <=6.5.25 to
 *      SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.25 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525in_switch_ep_recirc_profile_t(
    bcm_compat6525_switch_ep_recirc_profile_t *from,
    bcm_switch_ep_recirc_profile_t *to)
{
    bcm_switch_ep_recirc_profile_t_init(to);
    to->recirc_profile_id = from->recirc_profile_id;
    to->flags = from->flags;
    to->redirect_type = from->redirect_type;
    to->tocpu_type = from->tocpu_type;
    to->truncate_cell_length = from->truncate_cell_length;
    to->buffer_priority = from->buffer_priority;
    to->nih_cfg = from->nih_cfg;
    to->cpu_dma_cfg = from->cpu_dma_cfg;
}

/*
 * Function:
 *      _bcm_compat6525out_switch_ep_recirc_profile_t
 * Purpose:
 *      Convert the bcm_switch_ep_recirc_profile_t datatype from SDK 6.5.26+ to
 *      <=6.5.25
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525out_switch_ep_recirc_profile_t(
    bcm_switch_ep_recirc_profile_t *from,
    bcm_compat6525_switch_ep_recirc_profile_t *to)
{
    to->recirc_profile_id = from->recirc_profile_id;
    to->flags = from->flags;
    to->redirect_type = from->redirect_type;
    to->tocpu_type = from->tocpu_type;
    to->truncate_cell_length = from->truncate_cell_length;
    to->buffer_priority = from->buffer_priority;
    to->nih_cfg = from->nih_cfg;
    to->cpu_dma_cfg = from->cpu_dma_cfg;
}

/*
 * Function:
 *      bcm_compat6525_switch_ep_recirc_profile_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_switch_ep_recirc_profile_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      mc_encap - (IN) Multicast encapsualtion information.
 *      encap_id - (OUT) Returned encapsulation ID
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_switch_ep_recirc_profile_create(
    int unit,
    bcm_compat6525_switch_ep_recirc_profile_t *recirc_profile,
    int *recirc_profile_id)
{
    int rv = BCM_E_NONE;
    bcm_switch_ep_recirc_profile_t *new_recirc_profile = NULL;

    if (recirc_profile != NULL) {
        new_recirc_profile = (bcm_switch_ep_recirc_profile_t *)
                     sal_alloc(sizeof(bcm_switch_ep_recirc_profile_t),
                     "New recirc profile");
        if (new_recirc_profile == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_switch_ep_recirc_profile_t(recirc_profile,
                                                     new_recirc_profile);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_switch_ep_recirc_profile_create(unit, new_recirc_profile,
                                             recirc_profile_id);


    /* Deallocate memory*/
    sal_free(new_recirc_profile);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_switch_ep_recirc_profile_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_switch_ep_recirc_profile_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      encap_id - (IN) Encapsulation ID.
 *      mc_encap - (OUT) Returned multicast encapsualtion info.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_switch_ep_recirc_profile_get(
    int unit,
    int recirc_profile_id,
    bcm_compat6525_switch_ep_recirc_profile_t *recirc_profile)
{
    int rv = BCM_E_NONE;
    bcm_switch_ep_recirc_profile_t *new_recirc_profile = NULL;

    if (recirc_profile != NULL) {
        new_recirc_profile = (bcm_switch_ep_recirc_profile_t *)
                     sal_alloc(sizeof(bcm_switch_ep_recirc_profile_t),
                     "New recirc profile");
        if (new_recirc_profile == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_switch_ep_recirc_profile_get(unit, recirc_profile_id,
                                          new_recirc_profile);

    /* Transform the entry from the new format to old one */
    _bcm_compat6525out_switch_ep_recirc_profile_t(new_recirc_profile,
                                                  recirc_profile);

    /* Deallocate memory*/
    sal_free(new_recirc_profile);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6525in_vlan_translate_egress_key_config_t
 * Purpose:
 *      Convert the bcm_vlan_translate_egress_key_config_t datatype from <=6.5.25 to
 *      SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.25 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525in_vlan_translate_egress_key_config_t(
    bcm_compat6525_vlan_translate_egress_key_config_t *from,
    bcm_vlan_translate_egress_key_config_t *to)
{
    bcm_vlan_translate_egress_key_config_t_init(to);
    to->flags = from->flags;
    to->key_type = from->key_type;
    to->inner_vlan = from->inner_vlan;
    to->outer_vlan = from->outer_vlan;
    to->port = from->port;
    to->otag_preserve = from->otag_preserve;
    to->itag_preserve = from->itag_preserve;
    to->ingress_port = from->ingress_port;
}

/*
 * Function:
 *      bcm_compat6525_vlan_translate_egress_action_extended_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_action_extended_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN) 
 *      action - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_vlan_translate_egress_action_extended_add(
    int unit,
    bcm_compat6525_vlan_translate_egress_key_config_t *key_config,
    bcm_vlan_translate_egress_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_translate_egress_key_config_t *new_key_config = NULL;

    if (key_config != NULL) {
        new_key_config = (bcm_vlan_translate_egress_key_config_t *)
                     sal_alloc(sizeof(bcm_vlan_translate_egress_key_config_t),
                     "New key_config");
        if (new_key_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_vlan_translate_egress_key_config_t(key_config, new_key_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_action_extended_add(unit, new_key_config, action);


    /* Deallocate memory*/
    sal_free(new_key_config);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_vlan_translate_egress_action_extended_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_action_extended_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_vlan_translate_egress_action_extended_delete(
    int unit,
    bcm_compat6525_vlan_translate_egress_key_config_t *key_config)
{
    int rv = BCM_E_NONE;
    bcm_vlan_translate_egress_key_config_t *new_key_config = NULL;

    if (key_config != NULL) {
        new_key_config = (bcm_vlan_translate_egress_key_config_t *)
                     sal_alloc(sizeof(bcm_vlan_translate_egress_key_config_t),
                     "New key_config");
        if (new_key_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_vlan_translate_egress_key_config_t(key_config, new_key_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_action_extended_delete(unit, new_key_config);


    /* Deallocate memory*/
    sal_free(new_key_config);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_vlan_translate_egress_action_extended_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_action_extended_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN) 
 *      action - (OUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_vlan_translate_egress_action_extended_get(
    int unit,
    bcm_compat6525_vlan_translate_egress_key_config_t *key_config,
    bcm_vlan_translate_egress_action_set_t *action)
{
    int rv = BCM_E_NONE;
    bcm_vlan_translate_egress_key_config_t *new_key_config = NULL;

    if (key_config != NULL) {
        new_key_config = (bcm_vlan_translate_egress_key_config_t *)
                     sal_alloc(sizeof(bcm_vlan_translate_egress_key_config_t),
                     "New key_config");
        if (new_key_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_vlan_translate_egress_key_config_t(key_config, new_key_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_action_extended_get(unit, new_key_config, action);


    /* Deallocate memory*/
    sal_free(new_key_config);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_vlan_translate_egress_flexctr_extended_attach
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_flexctr_extended_attach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN) Egress VLAN translation key configuration.
 *      config - (IN) Flex counter configuration.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_vlan_translate_egress_flexctr_extended_attach(
    int unit,
    bcm_compat6525_vlan_translate_egress_key_config_t *key_config,
    bcm_vlan_flexctr_config_t *config)
{
    int rv = BCM_E_NONE;
    bcm_vlan_translate_egress_key_config_t *new_key_config = NULL;

    if (key_config != NULL) {
        new_key_config = (bcm_vlan_translate_egress_key_config_t *)
                     sal_alloc(sizeof(bcm_vlan_translate_egress_key_config_t),
                     "New key_config");
        if (new_key_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_vlan_translate_egress_key_config_t(key_config, new_key_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_flexctr_extended_attach(unit, new_key_config, config);


    /* Deallocate memory*/
    sal_free(new_key_config);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_vlan_translate_egress_flexctr_extended_detach
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_flexctr_extended_detach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN) Egress VLAN translation key configuration.
 *      config - (IN) Flex counter configuration.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_vlan_translate_egress_flexctr_extended_detach(
    int unit,
    bcm_compat6525_vlan_translate_egress_key_config_t *key_config,
    bcm_vlan_flexctr_config_t *config)
{
    int rv = BCM_E_NONE;
    bcm_vlan_translate_egress_key_config_t *new_key_config = NULL;

    if (key_config != NULL) {
        new_key_config = (bcm_vlan_translate_egress_key_config_t *)
                     sal_alloc(sizeof(bcm_vlan_translate_egress_key_config_t),
                     "New key_config");
        if (new_key_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_vlan_translate_egress_key_config_t(key_config, new_key_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_flexctr_extended_detach(unit, new_key_config, config);


    /* Deallocate memory*/
    sal_free(new_key_config);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_vlan_translate_egress_flexctr_extended_detach_all
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_flexctr_extended_detach_all.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN) Egress VLAN translation key configuration.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_vlan_translate_egress_flexctr_extended_detach_all(
    int unit,
    bcm_compat6525_vlan_translate_egress_key_config_t *key_config)
{
    int rv = BCM_E_NONE;
    bcm_vlan_translate_egress_key_config_t *new_key_config = NULL;

    if (key_config != NULL) {
        new_key_config = (bcm_vlan_translate_egress_key_config_t *)
                     sal_alloc(sizeof(bcm_vlan_translate_egress_key_config_t),
                     "New key_config");
        if (new_key_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_vlan_translate_egress_key_config_t(key_config, new_key_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_flexctr_extended_detach_all(unit, new_key_config);


    /* Deallocate memory*/
    sal_free(new_key_config);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_vlan_translate_egress_flexctr_extended_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_translate_egress_flexctr_extended_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      key_config - (IN) Egress VLAN translation key configuration.
 *      array_size - (IN) Size of allocated entries in config array.
 *      config_array - (OUT) Array of flex counter configurations.
 *      count - (OUT) Number of elements in config array. If array_size is 0, config array will be ignored, and count will be filled with the actual number.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_vlan_translate_egress_flexctr_extended_get(
    int unit,
    bcm_compat6525_vlan_translate_egress_key_config_t *key_config,
    int array_size,
    bcm_vlan_flexctr_config_t *config_array,
    int *count)
{
    int rv = BCM_E_NONE;
    bcm_vlan_translate_egress_key_config_t *new_key_config = NULL;

    if (key_config != NULL) {
        new_key_config = (bcm_vlan_translate_egress_key_config_t *)
                     sal_alloc(sizeof(bcm_vlan_translate_egress_key_config_t),
                     "New key_config");
        if (new_key_config == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_vlan_translate_egress_key_config_t(key_config, new_key_config);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_translate_egress_flexctr_extended_get(unit, new_key_config, array_size, config_array, count);


    /* Deallocate memory*/
    sal_free(new_key_config);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6525in_switch_pkt_trace_info_t
 * Purpose:
 *      Convert the bcm_switch_pkt_trace_info_t datatype from <=6.5.25 to
 *      SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.25 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525in_switch_pkt_trace_info_t(
    bcm_compat6525_switch_pkt_trace_info_t *from,
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
    for (i1 = 0; i1 < BCM_SWITCH_PKT_TRACE_FIELD_STAGE_MAX; i1++) {
        sal_memcpy(&(to->pkt_trace_field_info[i1]),
                   &(from->pkt_trace_field_info[i1]),
                   sizeof(bcm_switch_pkt_trace_field_info_t));
    }
}

/*
 * Function:
 *      _bcm_compat6525out_switch_pkt_trace_info_t
 * Purpose:
 *      Convert the bcm_switch_pkt_trace_info_t datatype from SDK 6.5.26+ to
 *      <=6.5.25
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525out_switch_pkt_trace_info_t(
    bcm_switch_pkt_trace_info_t *from,
    bcm_compat6525_switch_pkt_trace_info_t *to)
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
    for (i1 = 0; i1 < BCM_SWITCH_PKT_TRACE_FIELD_STAGE_MAX; i1++) {
        sal_memcpy(&(to->pkt_trace_field_info[i1]),
                   &(from->pkt_trace_field_info[i1]),
                   sizeof(bcm_switch_pkt_trace_field_info_t));
    }
}

/*
 * Function:
 *      bcm_compat6525_switch_pkt_trace_info_get
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
int bcm_compat6525_switch_pkt_trace_info_get(
    int unit,
    uint32 options,
    uint8 port,
    int len,
    uint8 *data,
    bcm_compat6525_switch_pkt_trace_info_t *pkt_trace_info)
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
        _bcm_compat6525in_switch_pkt_trace_info_t(
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
    _bcm_compat6525out_switch_pkt_trace_info_t(
                           new_pkt_trace_info,
                           pkt_trace_info);

    /* Deallocate memory*/
    sal_free(new_pkt_trace_info);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6525in_flowtracker_group_info_t
 * Purpose:
 *      Convert the bcm_flowtracker_group_info_t datatype from <=6.5.25 to
 *      SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.25 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525in_flowtracker_group_info_t(
        bcm_compat6525_flowtracker_group_info_t *from,
        bcm_flowtracker_group_info_t *to)
{
    bcm_flowtracker_group_info_t_init(to);
    to->observation_domain_id = from->observation_domain_id;
    to->group_class = from->group_class;
    to->group_flags = from->group_flags;
    to->group_type = from->group_type;

    sal_memcpy(&to->field_group[0], &from->field_group[0],
            sizeof (bcm_field_group_t) * BCM_PIPES_MAX);
}

/*
 * Function:
 *      _bcm_compat6525out_flowtracker_group_info_t
 * Purpose:
 *      Convert the bcm_flowtracker_group_info_t datatype from SDK 6.5.26+ to
 *      <=6.5.25
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525out_flowtracker_group_info_t(
        bcm_flowtracker_group_info_t *from,
        bcm_compat6525_flowtracker_group_info_t *to)
{
    to->observation_domain_id = from->observation_domain_id;
    to->group_class = from->group_class;
    to->group_flags = from->group_flags;
    to->group_type = from->group_type;

    sal_memcpy(&to->field_group[0], &from->field_group[0],
            sizeof (bcm_field_group_t) * BCM_PIPES_MAX);
}

/*
 * Function:
 *   bcm_compat6525_flowtracker_group_create
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_group_create
 * Parameters:
 *   unit              - (IN)    BCM device number.
 *   options           - (IN)    Group create options
 *   group_id          - (INOUT) Group Id
 *   group_info        - (IN)    Group Info
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6525_flowtracker_group_create(
        int unit,
        uint32 options,
        bcm_flowtracker_group_t *group_id,
        bcm_compat6525_flowtracker_group_info_t *group_info)
{
    int rv = BCM_E_NONE;
    bcm_flowtracker_group_info_t *new_group_info = NULL;

    if (group_info != NULL) {
        new_group_info = (bcm_flowtracker_group_info_t *)
            sal_alloc(sizeof(bcm_flowtracker_group_info_t),
                    "New group_info");
        if (new_group_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_flowtracker_group_info_t(group_info, new_group_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flowtracker_group_create(unit, options, group_id, new_group_info);

    if (new_group_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6525out_flowtracker_group_info_t(new_group_info, group_info);

        /* Deallocate memory*/
        sal_free(new_group_info);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_flowtracker_group_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_flowtracker_group_get
 * Parameters:
 *   unit              - (IN)  BCM device number
 *   id                - (IN)  Group Id
 *   group_info        - (OUT) Group Info
 * Returns:
 *   BCM_E_XXX   - BCM error code.
 */
int
bcm_compat6525_flowtracker_group_get(
        int unit,
        bcm_flowtracker_group_t id,
        bcm_compat6525_flowtracker_group_info_t *group_info)
{
    int rv = BCM_E_NONE;
    bcm_flowtracker_group_info_t *new_group_info = NULL;

    if (group_info != NULL) {
        new_group_info = (bcm_flowtracker_group_info_t *)
            sal_alloc(sizeof(bcm_flowtracker_group_info_t),
                    "New group info");
        if (new_group_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_flowtracker_group_info_t(group_info, new_group_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flowtracker_group_get(unit, id, new_group_info);

    if (new_group_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6525out_flowtracker_group_info_t(new_group_info, group_info);

        /* Deallocate memory*/
        sal_free(new_group_info);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6525in_switch_pkt_info_t
 * Purpose:
 *      Convert the bcm_switch_pkt_info_t datatype from <=6.5.25 to
 *      SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.25 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525in_switch_pkt_info_t(
        bcm_compat6525_switch_pkt_info_t *from,
        bcm_switch_pkt_info_t *to)
{
    int i1 = 0;
    bcm_switch_pkt_info_t_init(to);
    to->flags = from->flags;
    to->src_gport = from->src_gport;
    to->vid = from->vid;
    to->ethertype = from->ethertype;
    for (i1 = 0; i1 < 6; i1++) {
        to->src_mac[i1] = from->src_mac[i1];
    }
    for (i1 = 0; i1 < 6; i1++) {
        to->dst_mac[i1] = from->dst_mac[i1];
    }
    to->sip = from->sip;
    to->dip = from->dip;
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6[i1] = from->sip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6[i1] = from->dip6[i1];
    }
    to->protocol = from->protocol;
    to->src_l4_port = from->src_l4_port;
    to->trunk_gport = from->trunk_gport;
    to->mpintf = from->mpintf;
    to->fwd_reason = from->fwd_reason;
    to->top_label = from->top_label;
    to->second_label = from->second_label;
    to->third_label = from->third_label;
}

/*
 * Function:
 *      _bcm_compat6525out_switch_pkt_info_t
 * Purpose:
 *      Convert the bcm_switch_pkt_info_t datatype from SDK 6.5.26+ to
 *      <=6.5.25
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525out_switch_pkt_info_t(
        bcm_switch_pkt_info_t *from,
        bcm_compat6525_switch_pkt_info_t *to)
{
    int i1 = 0;
    from->flags = to->flags;
    from->src_gport = to->src_gport;
    from->vid = to->vid;
    from->ethertype = to->ethertype;
    for (i1 = 0; i1 < 6; i1++) {
        from->src_mac[i1] = to->src_mac[i1];
    }
    for (i1 = 0; i1 < 6; i1++) {
        from->dst_mac[i1] = to->dst_mac[i1];
    }
    from->sip = to->sip;
    from->dip = to->dip;
    for (i1 = 0; i1 < 16; i1++) {
        from->sip6[i1] = to->sip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        from->dip6[i1] = to->dip6[i1];
    }
    from->protocol = to->protocol;
    from->src_l4_port = to->src_l4_port;
    from->trunk_gport = to->trunk_gport;
    from->mpintf = to->mpintf;
    from->fwd_reason = to->fwd_reason;
    from->top_label = to->top_label;
    from->second_label = to->second_label;
    from->third_label = to->third_label;
}

/*
 * Function:
 *      bcm_compat6525_switch_pkt_info_hash_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_switch_pkt_info_hash_get
 * Parameters:
 *   unit              - (IN)  BCM device number
 *   pkt_info          - (IN)  Packet parameter information for hash calculation
 *   dst_gport         - (OUT) Destination module and port
 *   dst_intf          - (OUT) Destination L3 interface ID egress object
 * Returns:
 *   BCM_E_XXX   - BCM error code.
 */
int
bcm_compat6525_switch_pkt_info_hash_get(
    int unit,
    bcm_compat6525_switch_pkt_info_t *pkt_info,
    bcm_gport_t *dst_gport,
    bcm_if_t *dst_intf)
{
    int rv = BCM_E_NONE;
    bcm_switch_pkt_info_t *new_pkt_info = NULL;

    if (pkt_info != NULL) {
        new_pkt_info = (bcm_switch_pkt_info_t *)
            sal_alloc(sizeof(bcm_switch_pkt_info_t),
                    "New pkt info");
        if (new_pkt_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_switch_pkt_info_t(pkt_info, new_pkt_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_switch_pkt_info_hash_get(unit, new_pkt_info, dst_gport, dst_intf);

    if (new_pkt_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6525out_switch_pkt_info_t(new_pkt_info, pkt_info);

        /* Deallocate memory*/
        sal_free(new_pkt_info);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6525in_ifa_header_t
 * Purpose:
 *      Convert the bcm_ifa_header_t datatype from <=6.5.25 to
 *      SDK 6.5.26+.
 * Parameters:
 *      from        - (IN) The 6.5.26+ version of the datatype
 *      to          - (OUT) The SDK <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525in_ifa_header_t(bcm_compat6525_ifa_header_t *from,
                               bcm_ifa_header_t *to)
{
    bcm_ifa_header_t_init(to);

    to->ip_protocol = from->ip_protocol;
    to->max_length  = from->max_length;
    to->hop_limit   = from->hop_limit;
}

/*
 * Function:
 *      _bcm_compat6525out_ifa_header_t
 * Purpose:
 *      Convert the bcm_ifa_header_t datatype from 6.5.26+ to
 *      SDK <=6.5.25
 * Parameters:
 *      from        - (IN) The 6.5.26+ version of the datatype
 *      to          - (OUT) The SDK <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525out_ifa_header_t(bcm_ifa_header_t *from,
                                bcm_compat6525_ifa_header_t *to)
{
    sal_memset(to, 0, sizeof(bcm_compat6525_ifa_header_t));

    to->ip_protocol = from->ip_protocol;
    to->max_length  = from->max_length;
    to->hop_limit   = from->hop_limit;
}

/*!
 * \brief Create the IFA header.
 *
 * \param [in] unit Unit number.
 * \param [in] options Create options
 * \param [in/out] header_id Header ID pointing to IFA header
 * \param [in] header  IFA header
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_compat6525_ifa_header_create(int unit, uint32 options, int *header_id,
                                     bcm_compat6525_ifa_header_t *header)
{
    int rv;
    bcm_ifa_header_t *new_header = NULL;

    if (header != NULL) {
        /* Convert to new form. */
        new_header = sal_alloc(sizeof (bcm_ifa_header_t), "New IFA header");
        if (new_header == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6525in_ifa_header_t(header, new_header);
    }

    /* Call BCM API */
    rv = bcm_ifa_header_create(unit, options, header_id, new_header);
    if (new_header != NULL) {
        /* Transform back to old form */
        _bcm_compat6525out_ifa_header_t(new_header, header);
        sal_free(new_header);
        new_header = NULL;
    }

    return rv;
}

/*!
 * \brief Get the IFA header.
 *
 * \param [in] unit Unit Number
 * \param [in] header_id Header ID pointing to IFA header
 * \param [out] header IFA header
 *
 * \retval SHR_E_NONE Success.
 * \retval !SHR_E_NONE Failure.
 */
int bcm_compat6525_ifa_header_get(int unit, int header_id,
                                  bcm_compat6525_ifa_header_t *header)
{
    int rv;
    bcm_ifa_header_t *new_header = NULL;

    if (header != NULL) {
        /* Convert to new form. */
        new_header = sal_alloc(sizeof (bcm_ifa_header_t), "New IFA header");
        if (new_header == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6525in_ifa_header_t(header, new_header);
    }

    /* Call BCM API */
    rv = bcm_ifa_header_get(unit, header_id, new_header);
    if (new_header != NULL) {
        /* Transform back to old form */
        _bcm_compat6525out_ifa_header_t(new_header, header);
        sal_free(new_header);
        new_header = NULL;
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6525in_tsn_taf_profile_t
 * Purpose:
 *      Convert the bcm_tsn_taf_profile_t datatype from <=6.5.25 to
 *      SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.25 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525in_tsn_taf_profile_t(
    bcm_compat6525_tsn_taf_profile_t *from,
    bcm_tsn_taf_profile_t *to)
{
    int i1 = 0;

    bcm_tsn_taf_profile_t_init(to);
    to->num_entries = from->num_entries;
    for (i1 = 0; i1 < BCM_TSN_TAF_CALENDAR_TABLE_SIZE_6525; i1++) {
        sal_memcpy(&to->entries[i1], &from->entries[i1], sizeof(bcm_tsn_taf_entry_t));
    }
    for (i1 = BCM_TSN_TAF_CALENDAR_TABLE_SIZE_6525; i1 < BCM_TSN_TAF_CALENDAR_TABLE_SIZE; i1++) {
        sal_memset(&to->entries[i1], 0, sizeof(bcm_tsn_taf_entry_t));
    }
    sal_memcpy(&to->ptp_base_time, &from->ptp_base_time, sizeof(bcm_ptp_timestamp_t));
    to->ptp_cycle_time = from->ptp_cycle_time;
    to->ptp_max_cycle_extension = from->ptp_max_cycle_extension;
    to->ptp_cycle_time_upper = 0;
}

/*
 * Function:
 *      _bcm_compat6525out_tsn_taf_profile_t
 * Purpose:
 *      Convert the bcm_tsn_taf_profile_t datatype from SDK 6.5.26+ to
 *      <=6.5.25
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525out_tsn_taf_profile_t(
    bcm_tsn_taf_profile_t *from,
    bcm_compat6525_tsn_taf_profile_t *to)
{
    int i1 = 0;

    to->num_entries = from->num_entries;
    for (i1 = 0; i1 < BCM_TSN_TAF_CALENDAR_TABLE_SIZE_6525; i1++) {
        sal_memcpy(&to->entries[i1], &from->entries[i1], sizeof(bcm_tsn_taf_entry_t));
    }
    sal_memcpy(&to->ptp_base_time, &from->ptp_base_time, sizeof(bcm_ptp_timestamp_t));
    to->ptp_cycle_time = from->ptp_cycle_time;
    to->ptp_max_cycle_extension = from->ptp_max_cycle_extension;
}

/*
 * Function:
 *      bcm_compat6525_tsn_taf_profile_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_tsn_taf_profile_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      taf_gate - (IN)
 *      profile - (IN) Pointer to profile structure which specifies entries in calendar table, ptp time information for PTP mode
 *      pid - (OUT) profile id
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_tsn_taf_profile_create(
    int unit,
    int taf_gate,
    bcm_compat6525_tsn_taf_profile_t *profile,
    bcm_tsn_taf_profile_id_t *pid)
{
    int rv = BCM_E_NONE;
    bcm_tsn_taf_profile_t *new_profile = NULL;

    if (profile != NULL) {
        new_profile = (bcm_tsn_taf_profile_t *)
                     sal_alloc(sizeof(bcm_tsn_taf_profile_t),
                     "New profile");
        if (new_profile == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_tsn_taf_profile_t(profile, new_profile);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tsn_taf_profile_create(unit, taf_gate, new_profile, pid);


    /* Deallocate memory*/
    sal_free(new_profile);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_tsn_taf_profile_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_tsn_taf_profile_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      taf_gate - (IN)
 *      pid - (IN) profile id
 *      profile - (IN) profile
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_tsn_taf_profile_set(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_compat6525_tsn_taf_profile_t *profile)
{
    int rv = BCM_E_NONE;
    bcm_tsn_taf_profile_t *new_profile = NULL;

    if (profile != NULL) {
        new_profile = (bcm_tsn_taf_profile_t *)
                     sal_alloc(sizeof(bcm_tsn_taf_profile_t),
                     "New profile");
        if (new_profile == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_tsn_taf_profile_t(profile, new_profile);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tsn_taf_profile_set(unit, taf_gate, pid, new_profile);


    /* Deallocate memory*/
    sal_free(new_profile);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_tsn_taf_profile_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_tsn_taf_profile_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      taf_gate - (IN)
 *      pid - (IN) profile id
 *      profile - (OUT) profile
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_tsn_taf_profile_get(
    int unit,
    int taf_gate,
    bcm_tsn_taf_profile_id_t pid,
    bcm_compat6525_tsn_taf_profile_t *profile)
{
    int rv = BCM_E_NONE;
    bcm_tsn_taf_profile_t *new_profile = NULL;

    if (profile != NULL) {
        new_profile = (bcm_tsn_taf_profile_t *)
                     sal_alloc(sizeof(bcm_tsn_taf_profile_t),
                     "New profile");
        if (new_profile == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tsn_taf_profile_get(unit, taf_gate, pid, new_profile);

    /* Transform the entry from the new format to old one */
    _bcm_compat6525out_tsn_taf_profile_t(new_profile, profile);

    /* Deallocate memory*/
    sal_free(new_profile);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6525in_udf_pkt_format_info_t
 * Purpose:
 *      Convert the bcm_udf_pkt_format_info_t datatype from <=6.5.25 to
 *      SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.25 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525in_udf_pkt_format_info_t(
    bcm_compat6525_udf_pkt_format_info_t *from,
    bcm_udf_pkt_format_info_t *to)
{
    bcm_udf_pkt_format_info_t_init(to);
    to->prio                = from->prio;
    to->ethertype           = from->ethertype;
    to->ethertype_mask      = from->ethertype_mask;
    to->ip_protocol         = from->ip_protocol;
    to->ip_protocol_mask    = from->ip_protocol_mask;
    to->l2                  = from->l2;
    to->vlan_tag            = from->vlan_tag;
    to->outer_ip            = from->outer_ip;
    to->inner_ip            = from->inner_ip;
    to->tunnel              = from->tunnel;
    to->mpls                = from->mpls;
    to->fibre_chan_outer    = from->fibre_chan_outer;
    to->fibre_chan_inner    = from->fibre_chan_inner;
    to->higig               = from->higig;
    to->vntag               = from->vntag;
    to->etag                = from->etag;
    to->cntag               = from->cntag;
    to->icnm                = from->icnm;
    to->subport_tag         = from->subport_tag;
    to->class_id            = from->class_id;
    to->inner_protocol      = from->inner_protocol;
    to->inner_protocol_mask = from->inner_protocol_mask;
    to->l4_dst_port         = from->l4_dst_port;
    to->l4_dst_port_mask    = from->l4_dst_port_mask;
    to->opaque_tag_type     = from->opaque_tag_type;
    to->opaque_tag_type_mask = from->opaque_tag_type_mask;
    to->int_pkt             = from->int_pkt;
    to->src_port            = from->src_port;
    to->src_port_mask       = from->src_port_mask;
    to->lb_pkt_type         = from->lb_pkt_type;
    to->first_2bytes_after_mpls_bos = from->first_2bytes_after_mpls_bos;
    to->first_2bytes_after_mpls_bos_mask = from->first_2bytes_after_mpls_bos_mask;
    to->outer_ifa = from->outer_ifa;
    to->inner_ifa = from->inner_ifa;
    to->ip_gre_first_2bytes = from->ip_gre_first_2bytes;
    to->ip_gre_first_2bytes_mask = from->ip_gre_first_2bytes_mask;
    return;
}

/*
 * Function:
 *      _bcm_compat6525out_udf_pkt_format_info_t
 * Purpose:
 *      Convert the bcm_udf_pkt_format_info_t datatype from 6.5.26+ to
 *      SDK <=6.5.25
 * Parameters:
 *      from        - (IN) The 6.5.26+ version of the datatype
 *      to          - (OUT) The SDK <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525out_udf_pkt_format_info_t(
    bcm_udf_pkt_format_info_t *from,
    bcm_compat6525_udf_pkt_format_info_t *to)
{
    to->prio                = from->prio;
    to->ethertype           = from->ethertype;
    to->ethertype_mask      = from->ethertype_mask;
    to->ip_protocol         = from->ip_protocol;
    to->ip_protocol_mask    = from->ip_protocol_mask;
    to->l2                  = from->l2;
    to->vlan_tag            = from->vlan_tag;
    to->outer_ip            = from->outer_ip;
    to->inner_ip            = from->inner_ip;
    to->tunnel              = from->tunnel;
    to->mpls                = from->mpls;
    to->fibre_chan_outer    = from->fibre_chan_outer;
    to->fibre_chan_inner    = from->fibre_chan_inner;
    to->higig               = from->higig;
    to->vntag               = from->vntag;
    to->etag                = from->etag;
    to->cntag               = from->cntag;
    to->icnm                = from->icnm;
    to->subport_tag         = from->subport_tag;
    to->class_id            = from->class_id;
    to->inner_protocol      = from->inner_protocol;
    to->inner_protocol_mask = from->inner_protocol_mask;
    to->l4_dst_port         = from->l4_dst_port;
    to->l4_dst_port_mask    = from->l4_dst_port_mask;
    to->opaque_tag_type     = from->opaque_tag_type;
    to->opaque_tag_type_mask = from->opaque_tag_type_mask;
    to->int_pkt             = from->int_pkt;
    to->src_port            = from->src_port;
    to->src_port_mask       = from->src_port_mask;
    to->lb_pkt_type         = from->lb_pkt_type;
    to->first_2bytes_after_mpls_bos = from->first_2bytes_after_mpls_bos;
    to->first_2bytes_after_mpls_bos_mask = from->first_2bytes_after_mpls_bos_mask;
    to->outer_ifa = from->outer_ifa;
    to->inner_ifa = from->inner_ifa;
    to->ip_gre_first_2bytes = from->ip_gre_first_2bytes;
    to->ip_gre_first_2bytes_mask = from->ip_gre_first_2bytes_mask;
    return;
}

int
bcm_compat6525_udf_pkt_format_create(
    int unit,
    bcm_udf_pkt_format_options_t options,
    bcm_compat6525_udf_pkt_format_info_t *pkt_format,
    bcm_udf_pkt_format_id_t *pkt_format_id)
{
    int rv;
    bcm_udf_pkt_format_info_t *new_pkt_format = NULL;

    if (pkt_format != NULL) {
        new_pkt_format = sal_alloc(sizeof (bcm_udf_pkt_format_info_t),
                "New Pkt fmt");
        if (new_pkt_format == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6525in_udf_pkt_format_info_t(pkt_format, new_pkt_format);
    }

    /* Call BCM API */
    rv = bcm_udf_pkt_format_create(unit, options,
            new_pkt_format, pkt_format_id);
    if (new_pkt_format != NULL) {
        /* Transform back to old form */
        _bcm_compat6525out_udf_pkt_format_info_t(new_pkt_format, pkt_format);
        sal_free(new_pkt_format);
    }

    return rv;
}

int
bcm_compat6525_udf_pkt_format_info_get(
    int unit,
    bcm_udf_pkt_format_id_t pkt_format_id,
    bcm_compat6525_udf_pkt_format_info_t *pkt_format)
{
    int rv;
    bcm_udf_pkt_format_info_t *new_pkt_format = NULL;

    if (pkt_format != NULL) {
        new_pkt_format = sal_alloc(sizeof (bcm_udf_pkt_format_info_t),
                "New Pkt fmt");
        if (new_pkt_format == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6525in_udf_pkt_format_info_t(pkt_format, new_pkt_format);
    }

    /* Call BCM API */
    rv = bcm_udf_pkt_format_info_get(unit, pkt_format_id, new_pkt_format);
    if (new_pkt_format != NULL) {
        /* Transform back to old form */
        _bcm_compat6525out_udf_pkt_format_info_t(new_pkt_format, pkt_format);
        sal_free(new_pkt_format);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6525in_cosq_tas_profile_t
 * Purpose:
 *      Convert the bcm_cosq_tas_profile_t datatype from <=6.5.25 to
 *      SDK 6.5.26+
 * Parameters:
 *      from        - (IN) The <=6.5.25 version of the datatype
 *      to          - (OUT) The SDK 6.5.26+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525in_cosq_tas_profile_t(
    bcm_compat6525_cosq_tas_profile_t *from,
    bcm_cosq_tas_profile_t *to)
{
    int i1 = 0;

    bcm_cosq_tas_profile_t_init(to);
    to->num_entries = from->num_entries;
    for (i1 = 0; i1 < BCM_COSQ_MAX_TAS_CALENDAR_TABLE_SIZE; i1++) {
        sal_memcpy(&to->entries[i1], &from->entries[i1], sizeof(bcm_cosq_tas_entry_t));
    }
    sal_memcpy(&to->ptp_base_time, &from->ptp_base_time, sizeof(bcm_ptp_timestamp_t));
    to->ptp_cycle_time = from->ptp_cycle_time;
    to->ptp_max_cycle_extension = from->ptp_max_cycle_extension;
    to->ptp_cycle_time_upper = 0;
}

/*
 * Function:
 *      _bcm_compat6525out_cosq_tas_profile_t
 * Purpose:
 *      Convert the bcm_cosq_tas_profile_t datatype from SDK 6.5.26+ to
 *      <=6.5.25
 * Parameters:
 *      from        - (IN) The SDK 6.5.26+ version of the datatype
 *      to          - (OUT) The <=6.5.25 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6525out_cosq_tas_profile_t(
    bcm_cosq_tas_profile_t *from,
    bcm_compat6525_cosq_tas_profile_t *to)
{
    int i1 = 0;

    to->num_entries = from->num_entries;
    for (i1 = 0; i1 < BCM_COSQ_MAX_TAS_CALENDAR_TABLE_SIZE; i1++) {
        sal_memcpy(&to->entries[i1], &from->entries[i1], sizeof(bcm_cosq_tas_entry_t));
    }
    sal_memcpy(&to->ptp_base_time, &from->ptp_base_time, sizeof(bcm_ptp_timestamp_t));
    to->ptp_cycle_time = from->ptp_cycle_time;
    to->ptp_max_cycle_extension = from->ptp_max_cycle_extension;
}

/*
 * Function:
 *      bcm_compat6525_cosq_tas_profile_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_cosq_tas_profile_create.
 * Parameters:
 *      unit - (IN) BCM device number
 *      port - (IN)
 *      profile - (IN) Pointer to profile structure which specifies entries in calendar table, ptp time information for PTP mode
 *      pid - (OUT) profile id
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_cosq_tas_profile_create(
    int unit,
    bcm_gport_t port,
    bcm_compat6525_cosq_tas_profile_t *profile,
    bcm_cosq_tas_profile_id_t *pid)
{
    int rv = BCM_E_NONE;
    bcm_cosq_tas_profile_t *new_profile = NULL;

    if (profile != NULL) {
        new_profile = (bcm_cosq_tas_profile_t *)
                     sal_alloc(sizeof(bcm_cosq_tas_profile_t),
                     "New profile");
        if (new_profile == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_cosq_tas_profile_t(profile, new_profile);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_cosq_tas_profile_create(unit, port, new_profile, pid);


    /* Deallocate memory*/
    sal_free(new_profile);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_cosq_tas_profile_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_cosq_tas_profile_set.
 * Parameters:
 *      unit - (IN) BCM device number
 *      port - (IN)
 *      pid - (IN) profile id
 *      profile - (IN) profile
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_cosq_tas_profile_set(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_id_t pid,
    bcm_compat6525_cosq_tas_profile_t *profile)
{
    int rv = BCM_E_NONE;
    bcm_cosq_tas_profile_t *new_profile = NULL;

    if (profile != NULL) {
        new_profile = (bcm_cosq_tas_profile_t *)
                     sal_alloc(sizeof(bcm_cosq_tas_profile_t),
                     "New profile");
        if (new_profile == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6525in_cosq_tas_profile_t(profile, new_profile);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_cosq_tas_profile_set(unit, port, pid, new_profile);


    /* Deallocate memory*/
    sal_free(new_profile);

    return rv;
}

/*
 * Function:
 *      bcm_compat6525_cosq_tas_profile_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_cosq_tas_profile_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      port - (IN)
 *      pid - (IN) profile id
 *      profile - (OUT) profile
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6525_cosq_tas_profile_get(
    int unit,
    bcm_gport_t port,
    bcm_cosq_tas_profile_id_t pid,
    bcm_compat6525_cosq_tas_profile_t *profile)
{
    int rv = BCM_E_NONE;
    bcm_cosq_tas_profile_t *new_profile = NULL;

    if (profile != NULL) {
        new_profile = (bcm_cosq_tas_profile_t *)
                     sal_alloc(sizeof(bcm_cosq_tas_profile_t),
                     "New profile");
        if (new_profile == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_cosq_tas_profile_get(unit, port, pid, new_profile);

    /* Transform the entry from the new format to old one */
    _bcm_compat6525out_cosq_tas_profile_t(new_profile, profile);

    /* Deallocate memory*/
    sal_free(new_profile);

    return rv;
}

#endif /* BCM_RPC_SUPPORT */
