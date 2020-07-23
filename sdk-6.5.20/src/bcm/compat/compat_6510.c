/*
 * $Id: compat_6510.c,v 1.0 2018/02/26
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with <=sdk-6.5.10 routines
 */

#ifdef	BCM_RPC_SUPPORT
#include <shared/alloc.h>

#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/l2.h>

#include <bcm_int/compat/compat_6510.h>

/*
 * Function:
 *      _bcm_compat6510in_l2_addr_t
 * Purpose:
 *      Convert the bcm_l2_addr_t datatype from <=6.5.10 to
 *      SDK 6.5.10+
 * Parameters:
 *      from        - (IN) The <=6.5.10 version of the datatype
 *      to          - (OUT) The SDK 6.5.10+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6510in_l2_addr_t(bcm_compat6510_l2_addr_t *from,
                            bcm_l2_addr_t *to)
{
    to->flags = from->flags;
    to->station_flags = from->station_flags;
    sal_memcpy(to->mac, from->mac, sizeof(bcm_mac_t));
    to->vid = from->vid;
    to->port = from->port;
    to->modid = from->modid;
    to->tgid = from->tgid;
    to->cos_dst = from->cos_dst;
    to->cos_src = from->cos_src;
    to->l2mc_group = from->l2mc_group;
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
    to->flags2 = 0;
    to->egress_if = 0;
}

/*
 * Function:
 *      _bcm_compat6510out_l2_addr_t
 * Purpose:
 *      Convert the bcm_l2_addr_t datatype from 6.5.10+ to
 *      <=6.5.10
 * Parameters:
 *      from        - (IN) The SDK 6.5.10+ version of the datatype
 *      to          - (OUT) The <=6.5.10 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6510out_l2_addr_t(bcm_l2_addr_t *from,
                             bcm_compat6510_l2_addr_t *to)
{
    to->flags = from->flags;
    to->station_flags = from->station_flags;
    sal_memcpy(to->mac, from->mac, sizeof(bcm_mac_t));
    to->vid = from->vid;
    to->port = from->port;
    to->modid = from->modid;
    to->tgid = from->tgid;
    to->cos_dst = from->cos_dst;
    to->cos_src = from->cos_src;
    to->l2mc_group = from->l2mc_group;
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
 * Function: bcm_compat6510_l2_addr_add
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
bcm_compat6510_l2_addr_add(int unit,
                           bcm_compat6510_l2_addr_t *l2addr)
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
        _bcm_compat6510in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_add(unit, newl2addr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6510out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6510_l2_addr_multi_add
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
bcm_compat6510_l2_addr_multi_add(int unit,
                                 bcm_compat6510_l2_addr_t *l2addr,
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
        _bcm_compat6510in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_multi_add(unit, newl2addr,count);

    /* Transform the entry from the new format to old one */
    _bcm_compat6510out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6510_l2_addr_multi_delete
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
bcm_compat6510_l2_addr_multi_delete(int unit,
                                    bcm_compat6510_l2_addr_t *l2addr,
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
        _bcm_compat6510in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_multi_delete(unit, newl2addr,count);

    /* Transform the entry from the new format to old one */
    _bcm_compat6510out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6510_l2_addr_get
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
bcm_compat6510_l2_addr_get(int unit,
                           bcm_mac_t mac_addr, bcm_vlan_t vid,
                           bcm_compat6510_l2_addr_t *l2addr)
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
        _bcm_compat6510in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_addr_get(unit, mac_addr, vid, newl2addr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6510out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6510_l2_conflict_get
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
bcm_compat6510_l2_conflict_get(int unit, bcm_compat6510_l2_addr_t *addr,
                               bcm_compat6510_l2_addr_t *cf_array, int cf_max,
                               int *cf_count)
{
    int rv = BCM_E_NONE;
    bcm_l2_addr_t naddr;
    bcm_l2_addr_t *ncf_array = NULL;
    int i = 0;

    if ((NULL != addr) && (NULL != cf_array) && (0 < cf_max)) {
        /* Create from heap to avoid the stack to bloat */
        ncf_array = (bcm_l2_addr_t *)
                     sal_alloc(cf_max * sizeof(bcm_l2_addr_t),
                    "compat6510_l2_conflict_get");
        if (NULL == ncf_array) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Transform the entry from the old format to new one */
    _bcm_compat6510in_l2_addr_t(addr, &naddr);

    /* Call the BCM API with new format */
    rv = bcm_l2_conflict_get(unit, &naddr, ncf_array, cf_max, cf_count);

    if (rv >= 0) {
        for(i = 0; i < *cf_count; i++) {
            _bcm_compat6510out_l2_addr_t(&ncf_array[i], &cf_array[i]);
        }
    }

    /* Deallocate memory*/
    sal_free(ncf_array);
    return rv;
}

/*
 * Function: bcm_compat6510_l2_replace
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
bcm_compat6510_l2_replace(int unit, uint32 flags,
                          bcm_compat6510_l2_addr_t *match_addr,
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
        _bcm_compat6510in_l2_addr_t(match_addr, newmatch_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_replace(unit, flags, newmatch_addr, new_module, new_port, new_trunk);

    /* Transform the entry from the new format to old one */
    _bcm_compat6510out_l2_addr_t(newmatch_addr, match_addr);

    /* Deallocate memory*/
    sal_free(newmatch_addr);

    return rv;
}

/*
 * Function: bcm_compat6510_l2_replace_match
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
bcm_compat6510_l2_replace_match(int unit, uint32 flags,
                                bcm_compat6510_l2_addr_t *match_addr,
                                bcm_compat6510_l2_addr_t *mask_addr,
                                bcm_compat6510_l2_addr_t *replace_addr,
                                bcm_compat6510_l2_addr_t *replace_mask_addr)
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
        _bcm_compat6510in_l2_addr_t(match_addr, newmatch_addr);
        _bcm_compat6510in_l2_addr_t(mask_addr, newmask_addr);
        _bcm_compat6510in_l2_addr_t(replace_addr, newreplace_addr);
        _bcm_compat6510in_l2_addr_t(replace_mask_addr, newreplace_mask_addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_replace_match(unit, flags, newmatch_addr, newmask_addr,
                              newreplace_addr, newreplace_mask_addr);

    /* Transform the entry from the new format to old one */
    _bcm_compat6510out_l2_addr_t(newmatch_addr, match_addr);
    _bcm_compat6510out_l2_addr_t(newmask_addr, mask_addr);
    _bcm_compat6510out_l2_addr_t(newreplace_addr, replace_addr);
    _bcm_compat6510out_l2_addr_t(newreplace_mask_addr, replace_mask_addr);

    /* Deallocate memory*/
    sal_free(newmatch_addr);
    sal_free(newmask_addr);
    sal_free(newreplace_addr);
    sal_free(newreplace_mask_addr);

    return rv;
}

/*
 * Function: bcm_compat6510_l2_stat_get
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
bcm_compat6510_l2_stat_get(int unit,
                           bcm_compat6510_l2_addr_t *l2addr,
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
        _bcm_compat6510in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_get(unit, newl2addr, stat, val);

    /* Transform the entry from the new format to old one */
    _bcm_compat6510out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6510_l2_stat_get32
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
bcm_compat6510_l2_stat_get32(int unit,
                             bcm_compat6510_l2_addr_t *l2addr,
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
        _bcm_compat6510in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_get32(unit, newl2addr, stat, val);

    /* Transform the entry from the new format to old one */
    _bcm_compat6510out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6510_l2_stat_set
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
bcm_compat6510_l2_stat_set(int unit,
                           bcm_compat6510_l2_addr_t *l2addr,
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
        _bcm_compat6510in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_set(unit, newl2addr, stat, val);

    /* Transform the entry from the new format to old one */
    _bcm_compat6510out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6510_l2_stat_set32
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
bcm_compat6510_l2_stat_set32(int unit,
                             bcm_compat6510_l2_addr_t *l2addr,
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
        _bcm_compat6510in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_set32(unit, newl2addr, stat, val);

    /* Transform the entry from the new format to old one */
    _bcm_compat6510out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}

/*
 * Function: bcm_compat6510_l2_stat_enable_set
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
bcm_compat6510_l2_stat_enable_set(int unit,
                                  bcm_compat6510_l2_addr_t *l2addr,
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
        _bcm_compat6510in_l2_addr_t(l2addr, newl2addr);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2_stat_enable_set(unit, newl2addr, enable);

    /* Transform the entry from the new format to old one */
    _bcm_compat6510out_l2_addr_t(newl2addr, l2addr);

    /* Deallocate memory*/
    sal_free(newl2addr);

    return rv;
}
#endif  /* BCM_RPC_SUPPORT */
