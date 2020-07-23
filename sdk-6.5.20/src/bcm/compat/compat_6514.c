/*
 * $Id: compat_6514.h,v 2.0 2018/04/09
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-6.5.14 routines
 */

#ifdef BCM_RPC_SUPPORT
#include <shared/alloc.h>
#include <bcm/error.h>
#include <bcm_int/compat/compat_6514.h>

/*
 * Function:
 *  _bcm_compat6514in_l2_addr_t
 * Purpose:
 *  Convert the bcm_l2_addr_t datatype from <=6.5.14 to
 *  SDK 6.5.15+
 * Parameters:
 *  from        - (IN) The <=6.5.16 version of the datatype
 *  to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6514in_l2_addr_t(bcm_compat6514_l2_addr_t *from,
                                         bcm_l2_addr_t *to)
{
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->station_flags = from->station_flags;
    sal_memcpy(to->mac, from->mac, sizeof(bcm_mac_t));
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
    to->flow_handle = from-> flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    sal_memcpy(to->logical_fields, from->logical_fields,
            BCM_FLOW_MAX_NOF_LOGICAL_FIELDS * sizeof(bcm_flow_logical_field_t));
    to->num_of_fields = from->num_of_fields;
    to->sa_source_filter_pbmp = from->sa_source_filter_pbmp;
    to->tsn_flowset = from->tsn_flowset;
    to->sr_flowset = from->sr_flowset;
    to->policer_id = from->policer_id;
    to->taf_gate_primap = from->taf_gate_primap;
    to->stat_id = 0;
    to->stat_pp_profile = 0;
    to->gbp_src_id = 0;
}

/*
 * Function:
 *  _bcm_compat6514out_l2_addr_t
 * Purpose:
 *  Convert the bcm_l2_addr_t datatype from 6.5.15+ to
 *  <=6.5.14
 * Parameters:
 *  from        - (IN) The SDK 6.5.16+ version of the datatype
 *  to          - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *  Nothing
 */
STATIC void
_bcm_compat6514out_l2_addr_t(bcm_l2_addr_t *from,
                             bcm_compat6514_l2_addr_t *to)
{
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->station_flags = from->station_flags;
    sal_memcpy(to->mac, from->mac, sizeof(bcm_mac_t));
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
    to->flow_handle = from-> flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    sal_memcpy(to->logical_fields, from->logical_fields,
            BCM_FLOW_MAX_NOF_LOGICAL_FIELDS * sizeof(bcm_flow_logical_field_t));
    to->num_of_fields = from->num_of_fields;
    to->sa_source_filter_pbmp = from->sa_source_filter_pbmp;
    to->tsn_flowset = from->tsn_flowset;
    to->sr_flowset = from->sr_flowset;
    to->policer_id = from->policer_id;
    to->taf_gate_primap = from->taf_gate_primap;
}

/*
 * Function: bcm_compat6514_l2_addr_add
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_addr_add.
 *
 * Parameters:
 *  unit   - (IN) BCM device number.
 *  l2addr - (IN) l2 address entry with old format
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat6514_l2_addr_add(int unit,
                           bcm_compat6514_l2_addr_t *l2addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6514in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_add(unit, newl2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6514_l2_addr_multi_add
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_addr_multi_add.
 *
 * Parameters:
 *  unit   - (IN) BCM device number.
 *  l2addr - (IN) l2 address entry with old format.
 *  count  - (IN) count of l2 address to add.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6514_l2_addr_multi_add(int unit,
                                 bcm_compat6514_l2_addr_t *l2addr,
                                 int count)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6514in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_multi_add(unit, newl2addr,count);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6514_l2_addr_multi_delete
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_addr_multi_delete.
 *
 * Parameters:
 *  unit   - (IN) BCM device number.
 *  l2addr - (IN) L2 address entry with old format.
 *  count  - (IN) count of l2 address to delete.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6514_l2_addr_multi_delete(int unit,
                                    bcm_compat6514_l2_addr_t *l2addr,
                                    int count)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6514in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_multi_delete(unit, newl2addr,count);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6514_l2_addr_get
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_addr_get.
 *
 * Parameters:
 *  unit     - (IN)  BCM device number.
 *  mac_addr - (IN)  Input mac address entry to search.
 *  vid      - (IN)  Input VLAN ID to search.
 *  l2addr   - (OUT) l2 address entry with old format.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6514_l2_addr_get(int unit,
                           bcm_mac_t mac_addr, bcm_vlan_t vid,
                           bcm_compat6514_l2_addr_t *l2addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_get(unit, mac_addr, vid, newl2addr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6514out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6514_l2_conflict_get
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_conflict_get.
 *
 * Parameters:
 *  unit     - (IN)  BCM device number.
 *  addr     - (IN)  l2 address entry with old format.
 *  cf_array - (OUT) conflicting address array.
 *  cf_max   - (IN)  maximum conflicting address.
 *  cf_count - (OUT) actual count of conflicting address found.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6514_l2_conflict_get(int unit, bcm_compat6514_l2_addr_t *addr,
                               bcm_compat6514_l2_addr_t *cf_array, int cf_max,
                               int *cf_count)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *naddr = NULL;
    bcm_l2_addr_t *ncf_array = NULL;
    int i = 0;

    if ((NULL != addr) && (NULL != cf_array) && (0 < cf_max)) {
        /* Create from heap to avoid the stack to bloat */
        naddr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New naddr");
        if (NULL == naddr) {
            return BCM_E_MEMORY;
        }
        ncf_array = (bcm_l2_addr_t *)
                     sal_alloc(cf_max * sizeof(bcm_l2_addr_t),
                    "compat6514_l2_conflict_get");
        if (NULL == ncf_array) {
            sal_free(naddr);
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Transform the entry from the old format to new one */
    _bcm_compat6514in_l2_addr_t(addr, naddr);

    /* Call the BCM API with new format */
    rv = bcm_l2_conflict_get(unit, naddr, ncf_array, cf_max, cf_count);

    if (rv >= 0) {
        for(i = 0; i < *cf_count; i++) {
            _bcm_compat6514out_l2_addr_t(&ncf_array[i], &cf_array[i]);
        }
    }

    /* Deallocate memory*/
    sal_free(ncf_array);
    sal_free(naddr);
    return rv;
}

/*
 * Function: bcm_compat6514_l2_replace
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_replace.
 *
 * Parameters:
 *  unit       - (IN) BCM device number.
 *  flags      - (IN) L2 replace flags.
 *  match_addr - (IN) L2 address entry with old format.
 *  new_module - (IN) module id.
 *  new_port   - (IN) port.
 *  new_trunk  - (IN) trunk group id.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6514_l2_replace(int unit, uint32 flags,
                          bcm_compat6514_l2_addr_t *match_addr,
                          bcm_module_t new_module,
                          bcm_port_t new_port, bcm_trunk_t new_trunk)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newmatch_addr = NULL;

    if (NULL != match_addr) {
        /* Create from heap to avoid the stack to bloat */
        newmatch_addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newmatch_addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6514in_l2_addr_t(match_addr, newmatch_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_replace(unit, flags, newmatch_addr, new_module, new_port, new_trunk);

    /* Deallocate memory*/
    sal_free(newmatch_addr);

    return rv;
}

/*
 * Function: bcm_compat6514_l2_replace_match
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_replace_match.
 *
 * Parameters:
 *  unit              - (IN) BCM device number.
 *  match_addr        - (IN) l2 address entry with old format.
 *  mask_addr         - (IN) mask l2 address entry with old format.
 *  replace_addr      - (IN) replace l2 address entry in old format.
 *  replace_mask_addr - (IN) replace l2 mask address entry in old format.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6514_l2_replace_match(int unit, uint32 flags,
                                bcm_compat6514_l2_addr_t *match_addr,
                                bcm_compat6514_l2_addr_t *mask_addr,
                                bcm_compat6514_l2_addr_t *replace_addr,
                                bcm_compat6514_l2_addr_t *replace_mask_addr)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newmatch_addr = NULL;
    bcm_l2_addr_t *newmask_addr = NULL;
    bcm_l2_addr_t *newreplace_addr = NULL;
    bcm_l2_addr_t *newreplace_mask_addr = NULL;

    if ((NULL != match_addr) && (NULL != mask_addr) &&
            (NULL != replace_addr) && (NULL != replace_mask_addr)) {
        /* Create from heap to avoid the stack to bloat */
        newmatch_addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newmatch_addr) {
            return BCM_E_MEMORY;
        }

        newmask_addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr mask");
        if (NULL == newmask_addr) {
            sal_free(newmatch_addr);
            return BCM_E_MEMORY;
        }

        newreplace_addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "Replace l2 addr");
        if (NULL == newreplace_addr) {
            sal_free(newmatch_addr);
            sal_free(newmask_addr);
            return BCM_E_MEMORY;
        }

        newreplace_mask_addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "Replace l2 addr mask");
        if (NULL == newreplace_mask_addr) {
            sal_free(newmatch_addr);
            sal_free(newmask_addr);
            sal_free(newreplace_addr);
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6514in_l2_addr_t(match_addr, newmatch_addr);
        _bcm_compat6514in_l2_addr_t(mask_addr, newmask_addr);
        _bcm_compat6514in_l2_addr_t(replace_addr, newreplace_addr);
        _bcm_compat6514in_l2_addr_t(replace_mask_addr, newreplace_mask_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_replace_match(unit, flags, newmatch_addr, newmask_addr,
            newreplace_addr, newreplace_mask_addr);

    /* Deallocate memory*/
    sal_free(newmatch_addr);
    sal_free(newmask_addr);
    sal_free(newreplace_addr);
    sal_free(newreplace_mask_addr);

    return rv;
}

/*
 * Function: bcm_compat6514_l2_stat_get
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_stat_get.
 *
 * Parameters:
 *  unit   - (IN)  BCM device number.
 *  l2addr - (IN)  l2 address entry with old format.
 *  stat   - (IN)  stat to collect.
 *  val    - (OUT) stat value.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6514_l2_stat_get(int unit,
                           bcm_compat6514_l2_addr_t *l2addr,
                           bcm_l2_stat_t stat, uint64 *val)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6514in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_get(unit, newl2addr, stat, val);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6514_l2_stat_get32
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_stat_get32.
 *
 * Parameters:
 *  unit   - (IN)  BCM device number.
 *  l2addr - (IN)  l2 address entry with old format.
 *  stat   - (IN)  stat to collect.
 *  val    - (OUT) stat value.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6514_l2_stat_get32(int unit,
                             bcm_compat6514_l2_addr_t *l2addr,
                             bcm_l2_stat_t stat, uint32 *val)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6514in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_get32(unit, newl2addr, stat, val);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6514_l2_stat_set
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_stat_set.
 *
 * Parameters:
 *  unit   - (IN) BCM device number.
 *  l2addr - (IN) l2 address entry with old format.
 *  stat   - (IN) stat to set.
 *  val    - (IN) stat value.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6514_l2_stat_set(int unit,
                           bcm_compat6514_l2_addr_t *l2addr,
                           bcm_l2_stat_t stat, uint64 val)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6514in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_set(unit, newl2addr, stat, val);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6514_l2_stat_set32
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_stat_set32.
 *
 * Parameters:
 *  unit   - (IN) BCM device number.
 *  l2addr - (IN) l2 address entry with old format.
 *  stat   - (IN) stat to set.
 *  val    - (IN) stat value.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6514_l2_stat_set32(int unit,
                             bcm_compat6514_l2_addr_t *l2addr,
                             bcm_l2_stat_t stat, uint32 val)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6514in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_set32(unit, newl2addr, stat, val);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6514_l2_stat_enable_set
 *
 * Purpose:
 *  Compatibility function for RPC call to bcm_l2_stat_enable_set.
 *
 * Parameters:
 *  unit   - (IN) BCM device number.
 *  l2addr - (IN) l2 address entry with old format.
 *  enable - (IN) enable value.
 *
 * Returns:
 *  BCM_E_XXX
 */
int
bcm_compat6514_l2_stat_enable_set(int unit,
                                  bcm_compat6514_l2_addr_t *l2addr,
                                  int enable)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t *newl2addr = NULL;

    if (NULL != l2addr) {
        /* Create from heap to avoid the stack to bloat */
        newl2addr = (bcm_l2_addr_t *)
            sal_alloc(sizeof(bcm_l2_addr_t), "New l2 addr");
        if (NULL == newl2addr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6514in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_enable_set(unit, newl2addr, enable);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6514in_vlan_control_vlan_t
 * Purpose:
 *      Convert the _bcm_compat6514in_vlan_control_vlan_t datatype from <=6.5.14
 *      to SDK 6.5.15+
 * Parameters:
 *      from        - (IN) The <=6.5.15 version of the datatype
 *      to          - (OUT) The SDK 6.5.15+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6514in_vlan_control_vlan_t(
    bcm_compat6514_vlan_control_vlan_t *from,
    bcm_compat6518_vlan_control_vlan_t *to)
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
    sal_memcpy(&to->protocol_pkt, &from->protocol_pkt,
               sizeof(bcm_vlan_protocol_packet_ctrl_t));
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
    to->ingress_stat_id = 0;
    to->ingress_stat_pp_profile = 0;
    to->egress_stat_id = 0;
    to->egress_stat_pp_profile = 0;
}

/*
 * Function:
 *      _bcm_compat6514out_vlan_control_vlan_t
 * Purpose:
 *      Convert the bcm_vlan_control_vlan_t datatype from 6.5.15+ to
 *      <=SDK 6.5.14
 * Parameters:
 *      from        - (IN) The SDK 6.5.15+ version of the datatype
 *      to          - (OUT) The <=6.5.14 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6514out_vlan_control_vlan_t(
    bcm_compat6518_vlan_control_vlan_t *from,
    bcm_compat6514_vlan_control_vlan_t *to)
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
    sal_memcpy(&to->protocol_pkt, &from->protocol_pkt,
               sizeof(bcm_vlan_protocol_packet_ctrl_t));
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
}

int
bcm_compat6514_vlan_control_vlan_selective_get(
    int unit,
    bcm_vlan_t vlan,
    uint32 valid_fields,
    bcm_compat6514_vlan_control_vlan_t *control)
{
    int rv;
    bcm_compat6518_vlan_control_vlan_t *new_ctrl = NULL;


    if (NULL != control) {
        /* Create from heap to avoid the stack to bloat */
        new_ctrl = (bcm_compat6518_vlan_control_vlan_t *)
            sal_alloc(sizeof(bcm_compat6518_vlan_control_vlan_t), "New vlan ctrl");
        if (NULL == new_ctrl) {
            return BCM_E_MEMORY;
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6518_vlan_control_vlan_selective_get(unit, vlan,
                                             valid_fields, new_ctrl);

    if (NULL != new_ctrl) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6514out_vlan_control_vlan_t(new_ctrl, control);
        /* Deallocate memory*/
        sal_free(new_ctrl);
    }

    return rv;
}

int
bcm_compat6514_vlan_control_vlan_selective_set(
    int unit,
    bcm_vlan_t vlan,
    uint32 valid_fields,
    bcm_compat6514_vlan_control_vlan_t *control)
{
    int rv;
    bcm_compat6518_vlan_control_vlan_t *new_ctrl = NULL;

    if (NULL != control) {
        /* Create from heap to avoid the stack to bloat */
        new_ctrl = (bcm_compat6518_vlan_control_vlan_t *)
            sal_alloc(sizeof(bcm_compat6518_vlan_control_vlan_t), "New vlan ctrl");
        if (NULL == new_ctrl) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6514in_vlan_control_vlan_t(control, new_ctrl);
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6518_vlan_control_vlan_selective_set(unit, vlan,
                                             valid_fields, new_ctrl);

    if (NULL != new_ctrl) {
        /* Deallocate memory*/
        sal_free(new_ctrl);
    }

    return rv;
}

int
bcm_compat6514_vlan_control_vlan_get(
    int unit,
    bcm_vlan_t vlan,
    bcm_compat6514_vlan_control_vlan_t *control)
{
    int rv;
    bcm_compat6518_vlan_control_vlan_t *new_ctrl = NULL;

    if (NULL != control) {
        /* Create from heap to avoid the stack to bloat */
        new_ctrl = (bcm_compat6518_vlan_control_vlan_t *)
            sal_alloc(sizeof(bcm_compat6518_vlan_control_vlan_t), "New vlan ctrl");
        if (NULL == new_ctrl) {
            return BCM_E_MEMORY;
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6518_vlan_control_vlan_get(unit, vlan, new_ctrl);

    if (NULL != new_ctrl) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6514out_vlan_control_vlan_t(new_ctrl, control);
        /* Deallocate memory*/
        sal_free(new_ctrl);
    }

    return rv;
}

int
bcm_compat6514_vlan_control_vlan_set(
    int unit,
    bcm_vlan_t vlan,
/* coverity[pass_by_value] */
    bcm_compat6514_vlan_control_vlan_t control)
{
    int rv;
    bcm_compat6518_vlan_control_vlan_t *new_ctrl = NULL;

    /* Create from heap to avoid the stack to bloat */
    new_ctrl = (bcm_compat6518_vlan_control_vlan_t *)
        sal_alloc(sizeof(bcm_compat6518_vlan_control_vlan_t), "New vlan ctrl");
    if (NULL == new_ctrl) {
        return BCM_E_MEMORY;
    }

    /* Transform the entry from the old format to new one */
    _bcm_compat6514in_vlan_control_vlan_t(&control, new_ctrl);

    /* Call the BCM API with new format */
    rv = bcm_compat6518_vlan_control_vlan_set(unit, vlan, *new_ctrl);

    if (NULL != new_ctrl) {
        /* Deallocate memory*/
        sal_free(new_ctrl);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6514in_port_match_info_t
 * Purpose:
 *      Convert the bcm_port_match_info_t datatype from <=6.5.14 to
 *      SDK 6.5.15+
 * Parameters:
 *      from        - (IN) The <=6.5.14 version of the datatype
 *      to          - (OUT) The SDK 6.5.15+ version of the datatype
 * Returns:
 *      Nothing
*/
STATIC void
_bcm_compat6514in_port_match_info_t(
    bcm_compat6514_port_match_info_t *from,
    bcm_port_match_info_t *to)
{

    to->match = from->match;
    to->port = from->port;
    to->match_vlan = from->match_vlan;
    to->match_vlan_max = from->match_vlan_max;
    to->match_inner_vlan = from->match_inner_vlan;
    to->match_inner_vlan_max = from->match_inner_vlan_max;
    to->match_tunnel_vlan = from->match_tunnel_vlan;
    sal_memcpy(&to->match_tunnel_srcmac, &from->match_tunnel_srcmac,
            sizeof(to->match_tunnel_srcmac));
    to->match_label = from->match_label;
    to->flags = from->flags;
    to->match_pon_tunnel = from->match_pon_tunnel;
    to->match_ethertype = from->match_ethertype;
    to->match_pcp = from->match_pcp;

    to->action->new_outer_vlan = from->action->new_outer_vlan;
    to->action->new_inner_vlan = from->action->new_inner_vlan;
    to->action->new_inner_pkt_prio = from->action->new_inner_pkt_prio;
    to->action->new_outer_cfi = from->action->new_outer_cfi;
    to->action->new_inner_cfi = from->action->new_inner_cfi;
    to->action->ingress_if = from->action->ingress_if;
    to->action->priority = from->action->priority;
    to->action->dt_outer = from->action->dt_outer;
    to->action->dt_outer_prio = from->action->dt_outer_prio;
    to->action->dt_outer_pkt_prio = from->action->dt_outer_pkt_prio;
    to->action->dt_outer_cfi = from->action->dt_outer_cfi;
    to->action->dt_inner = from->action->dt_inner;
    to->action->dt_inner_prio = from->action->dt_inner_prio;
    to->action->dt_inner_pkt_prio = from->action->dt_inner_pkt_prio;
    to->action->dt_inner_cfi = from->action->dt_inner_cfi;
    to->action->ot_outer = from->action->ot_outer;
    to->action->ot_outer_prio = from->action->ot_outer_prio;
    to->action->ot_outer_pkt_prio = from->action->ot_outer_pkt_prio;
    to->action->ot_outer_cfi = from->action->ot_outer_cfi;
    to->action->ot_inner = from->action->ot_inner;
    to->action->ot_inner_pkt_prio = from->action->ot_inner_pkt_prio;
    to->action->ot_inner_cfi = from->action->ot_inner_cfi;
    to->action->it_outer = from->action->it_outer;
    to->action->it_outer_pkt_prio = from->action->it_outer_pkt_prio;
    to->action->it_outer_cfi = from->action->it_outer_cfi;
    to->action->it_inner = from->action->it_inner;
    to->action->it_inner_prio = from->action->it_inner_prio;
    to->action->it_inner_pkt_prio = from->action->it_inner_pkt_prio;
    to->action->it_inner_cfi = from->action->it_inner_cfi;
    to->action->ut_outer = from->action->ut_outer;
    to->action->ut_outer_pkt_prio = from->action->ut_outer_pkt_prio;
    to->action->ut_outer_cfi = from->action->ut_outer_cfi;
    to->action->ut_inner = from->action->ut_inner;
    to->action->ut_inner_pkt_prio = from->action->ut_inner_pkt_prio;
    to->action->ut_inner_cfi = from->action->ut_inner_cfi;
    to->action->outer_pcp = from->action->outer_pcp;
    to->action->inner_pcp = from->action->inner_pcp;
    to->action->policer_id = from->action->policer_id;
    to->action->outer_tpid = from->action->outer_tpid;
    to->action->inner_tpid = from->action->inner_tpid;
    to->action->outer_tpid_action = from->action->outer_tpid_action;
    to->action->inner_tpid_action = from->action->inner_tpid_action;
    to->action->action_id = from->action->action_id;
    to->action->class_id = from->action->class_id;
    to->action->taf_gate_primap = from->action->taf_gate_primap;
    to->action->flags = from->action->flags;

    to->extended_port_vid = from->extended_port_vid;
    to->vpn = from->vpn;
    to->niv_port_vif = from->niv_port_vif;
    to->isid = from->isid;
}

/*
 * Function:
 *      _bcm_compat6514out_port_match_info_t
 * Purpose:
 *      Convert the bcm_port_match_info_t datatype from 6.5.15+ to
 *      <=6.5.14
 * Parameters:
 *      from        - (IN) The SDK 6.5.15+ version of the datatype
 *      to          - (OUT) The <=6.5.14 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6514out_port_match_info_t(
    bcm_port_match_info_t *from,
    bcm_compat6514_port_match_info_t *to)
{
    to->match = from->match;
    to->port = from->port;
    to->match_vlan = from->match_vlan;
    to->match_vlan_max = from->match_vlan_max;
    to->match_inner_vlan = from->match_inner_vlan;
    to->match_inner_vlan_max = from->match_inner_vlan_max;
    to->match_tunnel_vlan = from->match_tunnel_vlan;
    sal_memcpy(&to->match_tunnel_srcmac, &from->match_tunnel_srcmac,
            sizeof(to->match_tunnel_srcmac));
    to->match_label = from->match_label;
    to->flags = from->flags;
    to->match_pon_tunnel = from->match_pon_tunnel;
    to->match_ethertype = from->match_ethertype;
    to->match_pcp = from->match_pcp;

    to->action->new_outer_vlan = from->action->new_outer_vlan;
    to->action->new_inner_vlan = from->action->new_inner_vlan;
    to->action->new_inner_pkt_prio = from->action->new_inner_pkt_prio;
    to->action->new_outer_cfi = from->action->new_outer_cfi;
    to->action->new_inner_cfi = from->action->new_inner_cfi;
    to->action->ingress_if = from->action->ingress_if;
    to->action->priority = from->action->priority;
    to->action->dt_outer = from->action->dt_outer;
    to->action->dt_outer_prio = from->action->dt_outer_prio;
    to->action->dt_outer_pkt_prio = from->action->dt_outer_pkt_prio;
    to->action->dt_outer_cfi = from->action->dt_outer_cfi;
    to->action->dt_inner = from->action->dt_inner;
    to->action->dt_inner_prio = from->action->dt_inner_prio;
    to->action->dt_inner_pkt_prio = from->action->dt_inner_pkt_prio;
    to->action->dt_inner_cfi = from->action->dt_inner_cfi;
    to->action->ot_outer = from->action->ot_outer;
    to->action->ot_outer_prio = from->action->ot_outer_prio;
    to->action->ot_outer_pkt_prio = from->action->ot_outer_pkt_prio;
    to->action->ot_outer_cfi = from->action->ot_outer_cfi;
    to->action->ot_inner = from->action->ot_inner;
    to->action->ot_inner_pkt_prio = from->action->ot_inner_pkt_prio;
    to->action->ot_inner_cfi = from->action->ot_inner_cfi;
    to->action->it_outer = from->action->it_outer;
    to->action->it_outer_pkt_prio = from->action->it_outer_pkt_prio;
    to->action->it_outer_cfi = from->action->it_outer_cfi;
    to->action->it_inner = from->action->it_inner;
    to->action->it_inner_prio = from->action->it_inner_prio;
    to->action->it_inner_pkt_prio = from->action->it_inner_pkt_prio;
    to->action->it_inner_cfi = from->action->it_inner_cfi;
    to->action->ut_outer = from->action->ut_outer;
    to->action->ut_outer_pkt_prio = from->action->ut_outer_pkt_prio;
    to->action->ut_outer_cfi = from->action->ut_outer_cfi;
    to->action->ut_inner = from->action->ut_inner;
    to->action->ut_inner_pkt_prio = from->action->ut_inner_pkt_prio;
    to->action->ut_inner_cfi = from->action->ut_inner_cfi;
    to->action->outer_pcp = from->action->outer_pcp;
    to->action->inner_pcp = from->action->inner_pcp;
    to->action->policer_id = from->action->policer_id;
    to->action->outer_tpid = from->action->outer_tpid;
    to->action->inner_tpid = from->action->inner_tpid;
    to->action->outer_tpid_action = from->action->outer_tpid_action;
    to->action->inner_tpid_action = from->action->inner_tpid_action;
    to->action->action_id = from->action->action_id;
    to->action->class_id = from->action->class_id;
    to->action->taf_gate_primap = from->action->taf_gate_primap;
    to->action->flags = from->action->flags;

    to->extended_port_vid = from->extended_port_vid;
    to->vpn = from->vpn;
    to->niv_port_vif = from->niv_port_vif;
    to->isid = from->isid;
}

/*
 * Function:
 *      bcm_compat6514_port_match_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_match_add.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - (IN) Port or gport
 *      match -(IN) Match criteria
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6514_port_match_add(
    int unit,
    bcm_gport_t port,
    bcm_compat6514_port_match_info_t *match)
{
    int rv = 0;
    bcm_port_match_info_t *new_match = NULL;
    bcm_vlan_action_set_t action;

    if (match != NULL) {
        new_match = (bcm_port_match_info_t *)
            sal_alloc(sizeof(bcm_port_match_info_t), "New port match info");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }

        bcm_port_match_info_t_init(new_match);
        bcm_vlan_action_set_t_init(&action);
        new_match->action = &action;

        _bcm_compat6514in_port_match_info_t(match, new_match);
    }

    rv = bcm_port_match_add(unit, port, new_match);

    if (new_match != NULL) {
        sal_free(new_match);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6514_port_match_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_match_delete.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - (IN) Port or gport
 *      match -(IN) Match criteria
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6514_port_match_delete(
    int unit,
    bcm_gport_t port,
    bcm_compat6514_port_match_info_t *match)
{
    int rv = 0;
    bcm_port_match_info_t *new_match = NULL;
    bcm_vlan_action_set_t action;

    if (match != NULL) {
        new_match = (bcm_port_match_info_t *)
            sal_alloc(sizeof(bcm_port_match_info_t), "New port match info");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }

        bcm_port_match_info_t_init(new_match);
        bcm_vlan_action_set_t_init(&action);
        new_match->action = &action;

        _bcm_compat6514in_port_match_info_t(match, new_match);
    }

    rv = bcm_port_match_delete(unit, port, new_match);

    if (new_match != NULL) {
        sal_free(new_match);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6514_port_match_replace
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_match_replace.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - (IN) Port or gport
 *      old_match - (IN) Old match criteria
 *      new_match - (IN) New match criteria
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6514_port_match_replace(
    int unit,
    bcm_gport_t port,
    bcm_compat6514_port_match_info_t *old_match,
    bcm_compat6514_port_match_info_t *new_match)
{
    int rv = 0;
    bcm_port_match_info_t *new_old_match = NULL;
    bcm_port_match_info_t *new_new_match = NULL;
    bcm_vlan_action_set_t old_action;
    bcm_vlan_action_set_t new_action;

    if (old_match != NULL) {
        new_old_match = (bcm_port_match_info_t *)
            sal_alloc(sizeof(bcm_port_match_info_t), "New port match info");
        if (new_old_match == NULL) {
            return BCM_E_MEMORY;
        }

        bcm_port_match_info_t_init(new_old_match);
        bcm_vlan_action_set_t_init(&old_action);
        new_old_match->action = &old_action;

        _bcm_compat6514in_port_match_info_t(old_match, new_old_match);
    }
    if (new_match != NULL) {
        new_new_match = (bcm_port_match_info_t *)
            sal_alloc(sizeof(bcm_port_match_info_t), "New port match info");
        if (new_new_match == NULL) {
            sal_free(new_old_match);
            return BCM_E_MEMORY;
        }

        bcm_port_match_info_t_init(new_new_match);
        bcm_vlan_action_set_t_init(&new_action);
        new_new_match->action = &new_action;

        _bcm_compat6514in_port_match_info_t(new_match, new_new_match);
    }

    rv = bcm_port_match_replace(unit, port, new_old_match, new_new_match);

    if (new_old_match != NULL) {
        sal_free(new_old_match);
    }
    if (new_new_match != NULL) {
        sal_free(new_new_match);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6514_port_match_multi_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_match_multi_get.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - (IN) Port or gport
 *      size - (IN) Number of elements in match array
 *      match_array - (OUT) Match array
 *      count - (OUT) Match count
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6514_port_match_multi_get(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_compat6514_port_match_info_t *match_array,
    int *count)
{
    int rv = 0, i = 0;
    bcm_port_match_info_t *new_match_array = NULL;
    bcm_vlan_action_set_t action[size];

    if (match_array != NULL) {
        new_match_array = (bcm_port_match_info_t *)
            sal_alloc(size * sizeof(bcm_port_match_info_t),
                    "New port match info array");
        if (new_match_array == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        for (i = 0; i < size; i++) {
            bcm_port_match_info_t_init(&new_match_array[i]);
            bcm_vlan_action_set_t_init(&action[i]);
            new_match_array[i].action = &action[i];
            _bcm_compat6514in_port_match_info_t(&match_array[i],
                                                &new_match_array[i]);
        }
    }

    rv = bcm_port_match_multi_get(unit, port, size, new_match_array, count);

    if (new_match_array != NULL) {
        for (i = 0; i < size; i++) {
            _bcm_compat6514out_port_match_info_t(&new_match_array[i],
                    &match_array[i]);
        }
        sal_free(new_match_array);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6514_port_match_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_match_set.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - (IN) Port or gport
 *      size - (IN) Number of elements in match array
 *      match_array - (IN) Match array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6514_port_match_set(
    int unit,
    bcm_gport_t port,
    int size,
    bcm_compat6514_port_match_info_t *match_array)
{
    int rv = 0, i = 0;
    bcm_port_match_info_t *new_match_array = NULL;
    bcm_vlan_action_set_t action[size];

    if (match_array != NULL) {
        new_match_array = (bcm_port_match_info_t *)
            sal_alloc(size * sizeof(bcm_port_match_info_t),
                    "New port match info array");
        if (new_match_array == NULL) {
            return BCM_E_MEMORY;
        }
        for (i = 0; i < size; i++) {
            bcm_port_match_info_t_init(&new_match_array[i]);
            bcm_vlan_action_set_t_init(&action[i]);
            new_match_array[i].action = &action[i];
            _bcm_compat6514in_port_match_info_t(&match_array[i],
                    &new_match_array[i]);
        }
    }

    rv = bcm_port_match_set(unit, port, size, new_match_array);

    if (new_match_array != NULL) {
        sal_free(new_match_array);
    }

    return rv;
}

#ifdef INCLUDE_L3
/*
 * Function:
 *      _bcm_compat6514in_ipmc_addr_t
 * Purpose:
 *      Convert the bcm_ipmc_addr_t datatype from <=6.5.14 to
 *      SDK 6.5.15+
 * Parameters:
 *      from        - (IN) The <=6.5.15 version of the datatype
 *      to          - (OUT) The SDK 6.5.15+ version of the datatype
 * Returns:
 *      Nothing
*/
STATIC void
_bcm_compat6514in_ipmc_addr_t(
    bcm_compat6514_ipmc_addr_t *from,
    bcm_ipmc_addr_t *to)
{
    bcm_ipmc_addr_t_init(to);
    to->s_ip_addr = from->s_ip_addr;
    to->mc_ip_addr = from->mc_ip_addr;
    sal_memcpy(&to->s_ip6_addr, &from->s_ip6_addr, sizeof(to->s_ip6_addr));
    sal_memcpy(&to->mc_ip6_addr, &from->mc_ip6_addr, sizeof(to->mc_ip6_addr));
    to->vid = from->vid;
    to->vrf = from->vrf;
    to->cos = from->cos;
    to->ts = from->ts;
    to->port_tgid = from->port_tgid;
    to->v = from->v;
    to->mod_id = from->mod_id;
    to->group = from->group;
    to->flags = from->flags;
    to->lookup_class = from->lookup_class;
    to->distribution_class = from->distribution_class;
    to->l3a_intf = from->l3a_intf;
    to->rp_id = from->rp_id;
    to->s_ip_mask = from->s_ip_mask;
    to->ing_intf = from->ing_intf;
    to->mc_ip_mask = from->mc_ip_mask;
    sal_memcpy(&to->mc_ip6_mask, &from->mc_ip6_mask, sizeof(to->mc_ip6_mask));
    to->group_l2 = from->group_l2;
    to->stat_id = 0;
    to->stat_pp_profile = 0;
}

/*
 * Function:
 *      _bcm_compat6514out_ipmc_addr_t
 * Purpose:
 *      Convert the bcm_ipmc_addr_t datatype from 6.5.15+ to
 *      <=6.5.14
 * Parameters:
 *      from        - (IN) The SDK 6.5.15+ version of the datatype
 *      to          - (OUT) The <=6.5.15 version of the datatype
 * Returns:
 *      Nothing
*/
STATIC void
_bcm_compat6514out_ipmc_addr_t(
    bcm_ipmc_addr_t *from,
    bcm_compat6514_ipmc_addr_t *to)
{
    to->s_ip_addr = from->s_ip_addr;
    to->mc_ip_addr = from->mc_ip_addr;
    sal_memcpy(&to->s_ip6_addr, &from->s_ip6_addr, sizeof(to->s_ip6_addr));
    sal_memcpy(&to->mc_ip6_addr, &from->mc_ip6_addr, sizeof(to->mc_ip6_addr));
    to->vid = from->vid;
    to->vrf = from->vrf;
    to->cos = from->cos;
    to->ts = from->ts;
    to->port_tgid = from->port_tgid;
    to->v = from->v;
    to->mod_id = from->mod_id;
    to->group = from->group;
    to->flags = from->flags;
    to->lookup_class = from->lookup_class;
    to->distribution_class = from->distribution_class;
    to->l3a_intf = from->l3a_intf;
    to->rp_id = from->rp_id;
    to->s_ip_mask = from->s_ip_mask;
    to->ing_intf = from->ing_intf;
    to->mc_ip_mask = from->mc_ip_mask;
    sal_memcpy(&to->mc_ip6_mask, &from->mc_ip6_mask, sizeof(to->mc_ip6_mask));
    to->group_l2 = from->group_l2;
}

/*
 * Function:
 *      bcm_compat6514_ipmc_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_ipmc_add.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      info - (IN) IPMC addr info.
 * Returns:
 *      BCM_E_XXX
*/
int bcm_compat6514_ipmc_add(
    int unit,
    bcm_compat6514_ipmc_addr_t *info)
{
    int rv = 0;
    bcm_ipmc_addr_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_ipmc_addr_t*)
            sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC addr info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6514in_ipmc_addr_t(info, new_info);
    }

    rv = bcm_ipmc_add(unit, new_info);

    if (new_info != NULL) {
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6514_ipmc_find
 * Purpose:
 *      Compatibility function for RPC call to bcm_ipmc_find
 * Parameters:
 *      unit - (IN) BCM device number.
 *      info - (IN) IPMC addr info.
 * Returns:
 *      BCM_E_XXX
*/
int bcm_compat6514_ipmc_find(
    int unit,
    bcm_compat6514_ipmc_addr_t *info)
{
    int rv = 0;
    bcm_ipmc_addr_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_ipmc_addr_t*)
            sal_alloc(sizeof(bcm_ipmc_addr_t), "IPMC addr info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6514in_ipmc_addr_t(info, new_info);
    }

    rv = bcm_ipmc_find(unit, new_info);    

    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6514out_ipmc_addr_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6514_ipmc_remove
 * Purpose:
 *      Compatibility function for RPC call to bcm_ipmc_remove.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      info - (IN) IPMC addr info.
 * Returns:
 *      BCM_E_XXX
*/
int bcm_compat6514_ipmc_remove(
    int unit,
    bcm_compat6514_ipmc_addr_t *info)
{
    int rv = 0;
    bcm_ipmc_addr_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_ipmc_addr_t*)
            sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC addr info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6514in_ipmc_addr_t(info, new_info);
    }

    rv = bcm_ipmc_remove(unit, new_info);

    if (new_info != NULL) {
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6514in_l3_egress_t
 * Purpose:
 *      Convert the bcm_l3_egress_t datatype from <=6.5.14 to
 *      SDK 6.5.15+
 * Parameters:
 *      from        - (IN) The <=6.5.16 version of the datatype
 *      to          - (OUT) The SDK 6.5.16+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6514in_l3_egress_t(
    bcm_compat6514_l3_egress_t *from,
    bcm_l3_egress_t *to)
{
    int i = 0;

    bcm_l3_egress_t_init(to);
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->intf = from->intf;
    sal_memcpy(to->mac_addr, from->mac_addr, sizeof(bcm_mac_t));
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
    to->vntag = from->vntag;
    to->vntag_action = from->vntag_action;
    to->etag = from->etag;
    to->etag_action = from->etag_action;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    to->flow_label_option_handle = from->flow_label_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i] = from->logical_fields[i];
    }
    to->num_of_fields = from->num_of_fields;
    to->counting_profile = from->counting_profile;
    to->mpls_ecn_map_id = from->mpls_ecn_map_id;
    to->urpf_mode = from->urpf_mode;
    to->mc_group = from->mc_group;
    to->stat_id = 0;
    to->stat_pp_profile = 0;
}

/*
 * Function:
 *      _bcm_compat6514out_l3_egress_t
 * Purpose:
 *      Convert the bcm_l3_egress_t datatype from 6.5.15+ to
 *      <=6.5.14
 * Parameters:
 *      from        - (IN) The SDK 6.5.16+ version of the datatype
 *      to          - (OUT) The <=6.5.16 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6514out_l3_egress_t(
    bcm_l3_egress_t *from,
    bcm_compat6514_l3_egress_t *to)
{
    int i = 0;

    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->intf = from->intf;
    sal_memcpy(to->mac_addr, from->mac_addr, sizeof(bcm_mac_t));
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
    to->vntag = from->vntag;
    to->vntag_action = from->vntag_action;
    to->etag = from->etag;
    to->etag_action = from->etag_action;
    to->flow_handle = from->flow_handle;
    to->flow_option_handle = from->flow_option_handle;
    to->flow_label_option_handle = from->flow_label_option_handle;
    for (i = 0; i < BCM_FLOW_MAX_NOF_LOGICAL_FIELDS; i++) {
        to->logical_fields[i] = from->logical_fields[i];
    }
    to->num_of_fields = from->num_of_fields;
    to->counting_profile = from->counting_profile;
    to->mpls_ecn_map_id = from->mpls_ecn_map_id;
    to->urpf_mode = from->urpf_mode;
    to->mc_group = from->mc_group;
}

int
bcm_compat6514_l3_egress_create(
    int unit,
    uint32 flags,
    bcm_compat6514_l3_egress_t *egr,
    bcm_if_t *intf)
{
    int rv;
    bcm_l3_egress_t *new_egr = NULL;

    if (NULL != egr) {
        /* Create from heap to avoid the stack to bloat */
        new_egr = sal_alloc(sizeof(bcm_l3_egress_t), "New Egress");
        if (NULL == new_egr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6514in_l3_egress_t(egr, new_egr);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_create(unit, flags, new_egr, intf);

    if (NULL != new_egr) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6514out_l3_egress_t(new_egr, egr);
        /* Deallocate memory*/
        sal_free(new_egr);
    }

    return rv;
}

int
bcm_compat6514_l3_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6514_l3_egress_t *egr)
{
    int rv;
    bcm_l3_egress_t *new_egr = NULL;

    if (NULL != egr) {
        /* Create from heap to avoid the stack to bloat */
        new_egr = sal_alloc(sizeof(bcm_l3_egress_t), "New Egress");
        if (NULL == new_egr) {
            return BCM_E_MEMORY;
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_get(unit, intf, new_egr);

    if (NULL != new_egr) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6514out_l3_egress_t(new_egr, egr);
        /* Deallocate memory*/
        sal_free(new_egr);
    }

    return rv;
}

int
bcm_compat6514_l3_egress_find(
    int unit,
    bcm_compat6514_l3_egress_t *egr,
    bcm_if_t *intf)
{
    int rv;
    bcm_l3_egress_t *new_egr = NULL;

    if (NULL != egr) {
        /* Create from heap to avoid the stack to bloat */
        new_egr = sal_alloc(sizeof(bcm_l3_egress_t), "New Egress");
        if (NULL == new_egr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6514in_l3_egress_t(egr, new_egr);
    }

    /* Call the BCM API with new format */
    rv = bcm_l3_egress_find(unit, new_egr, intf);

    if (NULL != new_egr) {
        /* Deallocate memory*/
        sal_free(new_egr);
    }

    return rv;
}

int
bcm_compat6514_failover_egress_set(
    int unit,
    bcm_if_t intf,
    bcm_compat6514_l3_egress_t *egr)
{
    int rv;
    bcm_l3_egress_t *new_egr = NULL;

    if (NULL != egr) {
        /* Create from heap to avoid the stack to bloat */
        new_egr = (bcm_l3_egress_t *)
            sal_alloc(sizeof(bcm_l3_egress_t), "New Egress");
        if (NULL == new_egr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6514in_l3_egress_t(egr, new_egr);
    }

    /* Call the BCM API with new format */
    rv = bcm_failover_egress_set(unit, intf, new_egr);

    if (NULL != new_egr) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6514out_l3_egress_t(new_egr, egr);
        /* Deallocate memory*/
        sal_free(new_egr);
    }

    return rv;
}

int
bcm_compat6514_failover_egress_get(
    int unit,
    bcm_if_t intf,
    bcm_compat6514_l3_egress_t *egr)
{
    int rv;
    bcm_l3_egress_t *new_egr = NULL;

    if (NULL != egr) {
        /* Create from heap to avoid the stack to bloat */
        new_egr = (bcm_l3_egress_t *)
            sal_alloc(sizeof(bcm_l3_egress_t), "New Egress");
        if (NULL == new_egr) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6514in_l3_egress_t(egr, new_egr);
    }

    /* Call the BCM API with new format */
    rv = bcm_failover_egress_get(unit, intf, new_egr);

    if (NULL != new_egr) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6514out_l3_egress_t(new_egr, egr);
        /* Deallocate memory*/
        sal_free(new_egr);
    }

    return rv;
}

/*
 * Function:
 *      _bcm_compat6514in_mpls_egress_label_t
 * Purpose:
 *      Convert the bcm_mpls_egress_label_t datatype from <=6.5.14 to
 *      SDK 6.5.15+
 * Parameters:
 *      from        - (IN) The <=6.5.14 version of the datatype
 *      to          - (OUT) The SDK 6.5.15+ version of the datatype
 * Returns:
 *      Nothing
*/
STATIC void
_bcm_compat6514in_mpls_egress_label_t(
    bcm_compat6514_mpls_egress_label_t *from,
    bcm_mpls_egress_label_t *to)
{
    to->flags = from->flags;
    to->label = from->label;
    to->qos_map_id = from->qos_map_id;
    to->exp = from->exp;
    to->ttl = from->ttl;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->tunnel_id = from->tunnel_id;
    to->l3_intf_id = from->l3_intf_id;
    to->action = from->action;
    to->ecn_map_id = from->ecn_map_id;
    to->int_cn_map_id = from->int_cn_map_id;
    to->egress_failover_id = from->egress_failover_id;
    to->egress_failover_if_id = from->egress_failover_if_id;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->spl_label_push_type = from->spl_label_push_type;
    to->encap_access = from->encap_access;
}

/*
 * Function:
 *      _bcm_compat6514out_mpls_egress_label_t
 * Purpose:
 *      Convert the bcm_mpls_egress_label_t datatype from 6.5.15+ to
 *      <=6.5.14
 * Parameters:
 *      from        - (IN) The SDK 6.5.15+ version of the datatype
 *      to          - (OUT) The <=6.5.14 version of the datatype
 * Returns:
 *      Nothing
*/
STATIC void
_bcm_compat6514out_mpls_egress_label_t(
    bcm_mpls_egress_label_t *from,
    bcm_compat6514_mpls_egress_label_t *to)
{
    to->flags = from->flags;
    to->label = from->label;
    to->qos_map_id = from->qos_map_id;
    to->exp = from->exp;
    to->ttl = from->ttl;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->tunnel_id = from->tunnel_id;
    to->l3_intf_id = from->l3_intf_id;
    to->action = from->action;
    to->ecn_map_id = from->ecn_map_id;
    to->int_cn_map_id = from->int_cn_map_id;
    to->egress_failover_id = from->egress_failover_id;
    to->egress_failover_if_id = from->egress_failover_if_id;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->spl_label_push_type = from->spl_label_push_type;
    to->encap_access = from->encap_access;
}

/*
 * Function:
 *      _bcm_compat6514in_mpls_tunnel_switch_t
 * Purpose:
 *      Convert the bcm_mpls_tunnel_switch_t datatype from <=6.5.14 to
 *      SDK 6.5.15+
 * Parameters:
 *      from        - (IN) The <=6.5.14 version of the datatype
 *      to          - (OUT) The SDK 6.5.15+ version of the datatype
 * Returns:
 *      Nothing
*/
STATIC void
_bcm_compat6514in_mpls_tunnel_switch_t(
    bcm_compat6514_mpls_tunnel_switch_t *from,
    bcm_mpls_tunnel_switch_t *to)
{
    bcm_mpls_tunnel_switch_t_init(to);
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->label = from->label;
    to->second_label = from->second_label;
    to->port = from->port;
    to->action = from->action;
    to->action_if_bos = from->action_if_bos;
    to->action_if_not_bos = from->action_if_not_bos;
    to->mc_group = from->mc_group;
    to->exp_map = from->exp_map;
    to->int_pri = from->int_pri;
    to->policer_id = from->policer_id;
    to->vpn = from->vpn;
    _bcm_compat6514in_mpls_egress_label_t(&from->egress_label, &to->egress_label);
    to->egress_if = from->egress_if;
    to->ingress_if = from->ingress_if;
    to->mtu = from->mtu;
    to->qos_map_id = from->qos_map_id;
    to->failover_id = from->failover_id;
    to->tunnel_id = from->tunnel_id;
    to->failover_tunnel_id = from->failover_tunnel_id;
    to->tunnel_if = from->tunnel_if;
    to->egress_port = from->egress_port;
    to->oam_global_context_id = from->oam_global_context_id;
    to->class_id = from->class_id;
    to->inlif_counting_profile = from->inlif_counting_profile;
    to->ecn_map_id = from->ecn_map_id;
    to->tunnel_term_ecn_map_id = from->tunnel_term_ecn_map_id;
}

/*
 * Function:
 *      _bcm_compat6514out_mpls_tunnel_switch_t
 * Purpose:
 *      Convert the bcm_mpls_tunnel_switch_t datatype from 6.5.15+ to
 *      <=6.5.14
 * Parameters:
 *      from        - (IN) The SDK 6.5.15+ version of the datatype
 *      to          - (OUT) The <=6.5.14 version of the datatype
 * Returns:
 *      Nothing
*/
STATIC void
_bcm_compat6514out_mpls_tunnel_switch_t(
    bcm_mpls_tunnel_switch_t *from,
    bcm_compat6514_mpls_tunnel_switch_t *to)
{
    to->flags = from->flags;
    to->flags2 = from->flags2;
    to->label = from->label;
    to->second_label = from->second_label;
    to->port = from->port;
    to->action = from->action;
    to->action_if_bos = from->action_if_bos;
    to->action_if_not_bos = from->action_if_not_bos;
    to->mc_group = from->mc_group;
    to->exp_map = from->exp_map;
    to->int_pri = from->int_pri;
    to->policer_id = from->policer_id;
    to->vpn = from->vpn;
    _bcm_compat6514out_mpls_egress_label_t(&from->egress_label, &to->egress_label);
    to->egress_if = from->egress_if;
    to->ingress_if = from->ingress_if;
    to->mtu = from->mtu;
    to->qos_map_id = from->qos_map_id;
    to->failover_id = from->failover_id;
    to->tunnel_id = from->tunnel_id;
    to->failover_tunnel_id = from->failover_tunnel_id;
    to->tunnel_if = from->tunnel_if;
    to->egress_port = from->egress_port;
    to->oam_global_context_id = from->oam_global_context_id;
    to->class_id = from->class_id;
    to->inlif_counting_profile = from->inlif_counting_profile;
    to->ecn_map_id = from->ecn_map_id;
    to->tunnel_term_ecn_map_id = from->tunnel_term_ecn_map_id;
}

/*
 * Function:
 *      bcm_compat6514_mpls_tunnel_switch_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_switch_add.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      info - (IN) MPLS tunnel switch info.
 * Returns:
 *      BCM_E_XXX
*/
int bcm_compat6514_mpls_tunnel_switch_add(
    int unit,
    bcm_compat6514_mpls_tunnel_switch_t *info)
{
    int rv = 0;
    bcm_mpls_tunnel_switch_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_mpls_tunnel_switch_t*)
            sal_alloc(sizeof(bcm_mpls_tunnel_switch_t), "New tunnel switch info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6514in_mpls_tunnel_switch_t(info, new_info);
    }

    rv = bcm_mpls_tunnel_switch_add(unit, new_info);

    if (new_info != NULL) {
        _bcm_compat6514out_mpls_tunnel_switch_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6514_mpls_tunnel_switch_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_switch_delete.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      info - (IN) MPLS tunnel switch info.
 * Returns:
 *      BCM_E_XXX
*/
int bcm_compat6514_mpls_tunnel_switch_delete(
    int unit,
    bcm_compat6514_mpls_tunnel_switch_t *info)
{
    int rv = 0;
    bcm_mpls_tunnel_switch_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_mpls_tunnel_switch_t*)
            sal_alloc(sizeof(bcm_mpls_tunnel_switch_t), "New tunnel switch info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6514in_mpls_tunnel_switch_t(info, new_info);
    }

    rv = bcm_mpls_tunnel_switch_delete(unit, new_info);

    if (new_info != NULL) {
        _bcm_compat6514out_mpls_tunnel_switch_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6514_mpls_tunnel_switch_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_mpls_tunnel_switch_get.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      info - (IN) MPLS tunnel switch info.
 * Returns:
 *      BCM_E_XXX
*/
int bcm_compat6514_mpls_tunnel_switch_get(
    int unit,
    bcm_compat6514_mpls_tunnel_switch_t *info)
{
    int rv = 0;
    bcm_mpls_tunnel_switch_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_mpls_tunnel_switch_t*)
            sal_alloc(sizeof(bcm_mpls_tunnel_switch_t), "New tunnel switch info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6514in_mpls_tunnel_switch_t(info, new_info);
    }

    rv = bcm_mpls_tunnel_switch_get(unit, new_info);

    if (new_info != NULL) {
        _bcm_compat6514out_mpls_tunnel_switch_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}
#endif

#endif /* BCM_RPC_SUPPORT */
