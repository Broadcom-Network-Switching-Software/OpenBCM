/*
* $Id: compat_6523.c,v 1.0 2021/07/16
* This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
* 
* Copyright 2007-2022 Broadcom Inc. All rights reserved.
*
* RPC Compatibility with sdk-6.5.23 routines
*/

#ifdef BCM_RPC_SUPPORT
#include <shared/alloc.h>
#include <bcm/error.h>
#include <bcm_int/compat/compat_6523.h>


/*
 * Function:
 *      _bcm_compat6523in_trunk_info_t
 * Purpose:
 *      Convert the bcm_trunk_info_t datatype from <=6.5.23 to
 *      SDK 6.5.24+
 * Parameters:
 *      from        - (IN) The <=6.5.23 version of the datatype
 *      to          - (OUT) The SDK 6.5.24+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523in_trunk_info_t(
    bcm_compat6523_trunk_info_t *from,
    bcm_trunk_info_t *to)
{
    bcm_trunk_info_t_init(to);
    to->flags = from->flags;
    to->psc = from->psc;
    sal_memcpy(&to->psc_info, &from->psc_info,
               sizeof(bcm_trunk_psc_profile_info_t));
    to->ipmc_psc = from->ipmc_psc;
    to->dlf_index = from->dlf_index;
    to->mc_index = from->mc_index;
    to->ipmc_index = from->ipmc_index;
    to->dynamic_size = from->dynamic_size;
    to->dynamic_age = from->dynamic_age;
    to->dynamic_load_exponent = from->dynamic_load_exponent;
    to->dynamic_expected_load_exponent = from->dynamic_expected_load_exponent;
    to->master_tid = from->master_tid;
    sal_memcpy(&to->dgm, &from->dgm, sizeof(bcm_trunk_dgm_t));
}

/*
 * Function:
 *      _bcm_compat6523out_trunk_info_t
 * Purpose:
 *      Convert the bcm_trunk_info_t datatype from SDK 6.5.24+ to
 *      <=6.5.23
 * Parameters:
 *      from        - (IN) The SDK 6.5.24+ version of the datatype
 *      to          - (OUT) The <=6.5.23 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523out_trunk_info_t(
    bcm_trunk_info_t *from,
    bcm_compat6523_trunk_info_t *to)
{
    to->flags = from->flags;
    to->psc = from->psc;
    sal_memcpy(&to->psc_info, &from->psc_info,
               sizeof(bcm_trunk_psc_profile_info_t));
    to->ipmc_psc = from->ipmc_psc;
    to->dlf_index = from->dlf_index;
    to->mc_index = from->mc_index;
    to->ipmc_index = from->ipmc_index;
    to->dynamic_size = from->dynamic_size;
    to->dynamic_age = from->dynamic_age;
    to->dynamic_load_exponent = from->dynamic_load_exponent;
    to->dynamic_expected_load_exponent = from->dynamic_expected_load_exponent;
    to->master_tid = from->master_tid;
    sal_memcpy(&to->dgm, &from->dgm, sizeof(bcm_trunk_dgm_t));
}

int bcm_compat6523_trunk_get(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6523_trunk_info_t *trunk_info,
    int member_max,
    bcm_trunk_member_t *member_array,
    int *member_count)
{
    int rv;
    bcm_trunk_info_t *new_trunk_info = NULL;

    if (NULL != trunk_info) {
        /* Create from heap to avoid the stack to bloat */
        new_trunk_info = sal_alloc(sizeof(bcm_trunk_info_t), "New Host");
        if (NULL == new_trunk_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_trunk_info_t(trunk_info, new_trunk_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_trunk_get(unit, tid, new_trunk_info, member_max, member_array,
                       member_count);

    if (NULL != new_trunk_info) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6523out_trunk_info_t(new_trunk_info, trunk_info);
        /* Deallocate memory*/
        sal_free(new_trunk_info);
    }

    return rv;
}

int bcm_compat6523_trunk_set(
    int unit,
    bcm_trunk_t tid,
    bcm_compat6523_trunk_info_t *trunk_info,
    int member_count,
    bcm_trunk_member_t *member_array)
{
    int rv;
    bcm_trunk_info_t *new_trunk_info = NULL;

    if (NULL != trunk_info) {
        /* Create from heap to avoid the stack to bloat */
        new_trunk_info = sal_alloc(sizeof(bcm_trunk_info_t), "New Host");
        if (NULL == new_trunk_info) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_trunk_info_t(trunk_info, new_trunk_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_trunk_set(unit, tid, new_trunk_info, member_count, member_array);

    if (NULL != new_trunk_info) {
        /* Deallocate memory*/
        sal_free(new_trunk_info);
    }

    return rv;
}


/*
 * Function:
 *      _bcm_compat6523in_trunk_multiset_info_t
 * Purpose:
 *      Convert the bcm_trunk_multiset_info_t datatype from <=6.5.23 to
 *      SDK 6.5.24+
 * Parameters:
 *      from        - (IN) The <=6.5.23 version of the datatype
 *      to          - (OUT) The SDK 6.5.24+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523in_trunk_multiset_info_t(
    bcm_compat6523_trunk_multiset_info_t *from,
    bcm_trunk_multiset_info_t *to)
{
    to->tid = from->tid;
    _bcm_compat6523in_trunk_info_t(&from->trunk_info, &to->trunk_info);
    to->member_count = from->member_count;
    to->set_result = from->set_result;
}

int bcm_compat6523_trunk_multi_set(
    int unit,
    int trunk_count,
    bcm_compat6523_trunk_multiset_info_t *trunk_array,
    int tot_member_count,
    bcm_trunk_member_t *member_array)

{
    int i, rv;
    bcm_trunk_multiset_info_t *new_trunk_array = NULL;

    if (NULL != trunk_array) {
        /* Create from heap to avoid the stack to bloat */
        new_trunk_array = sal_alloc(
                                sizeof(bcm_trunk_multiset_info_t) * trunk_count,
                                "New Host");
        if (NULL == new_trunk_array) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        for (i = 0; i < trunk_count; i ++) {
            _bcm_compat6523in_trunk_multiset_info_t(trunk_array + i,
                                                    new_trunk_array + i);
        }
    }

    /* Call the BCM API with new format */
    rv = bcm_trunk_multi_set(unit, trunk_count, new_trunk_array,
                             tot_member_count, member_array);

    if (NULL != new_trunk_array) {
        /* Deallocate memory*/
        sal_free(new_trunk_array);
    }

    return rv;
}



#if defined(INCLUDE_L3)

/*
 * Function:
 *      _bcm_compat6523in_flow_match_config_t
 * Purpose:
 *      Convert the bcm_flow_port_t datatype from <=6.5.23 to 6.5.23+
 * Parameters:
 *      from        - (IN) The <=6.5.23 version of the datatype
 *      to          - (OUT) The >6.5.23 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523in_flow_match_config_t(
    bcm_compat6523_flow_match_config_t *from,
    bcm_flow_match_config_t *to)
{
    bcm_flow_match_config_t_init(to);
    to->vnid                = from->vnid;
    to->vlan                = from->vlan;
    to->inner_vlan          = from->inner_vlan;
    to->sip                 = from->sip;
    to->sip_mask            = from->sip_mask;
    to->dip                 = from->dip;
    to->dip_mask            = from->dip_mask;
    to->udp_dst_port        = from->udp_dst_port;
    to->udp_src_port        = from->udp_src_port;
    to->protocol            = from->protocol;
    to->mpls_label          = from->mpls_label;
    to->flow_port           = from->flow_port;
    to->vpn                 = from->vpn;
    to->intf_id             = from->intf_id;
    to->options             = from->options;
    to->criteria            = from->criteria;
    to->valid_elements      = from->valid_elements;
    to->flow_handle         = from->flow_handle;
    to->flow_option         = from->flow_option;
    to->flags               = from->flags;
    sal_memcpy(to->sip6,      from->sip6, sizeof(bcm_ip6_t));
    sal_memcpy(to->sip6_mask, from->sip6_mask, sizeof(bcm_ip6_t));
    sal_memcpy(to->dip6,      from->dip6, sizeof(bcm_ip6_t));
    sal_memcpy(to->dip6_mask, from->dip6_mask, sizeof(bcm_ip6_t));
    to->default_vlan        = from->default_vlan;
    to->default_vpn         = from->default_vpn;
}

/*
 * Function:
 *      _bcm_compat6523out_flow_match_config_t
 * Purpose:
 *      Convert the bcm_flow_port_t datatype from >6.5.23 to <=6.5.23
 * Parameters:
 *      from        - (IN) The >6.5.23 version of the datatype
 *      to          - (OUT) The <=6.5.23 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523out_flow_match_config_t(
    bcm_flow_match_config_t *from,
    bcm_compat6523_flow_match_config_t *to)
{
    to->vnid                = from->vnid;
    to->vlan                = from->vlan;
    to->inner_vlan          = from->inner_vlan;
    to->sip                 = from->sip;
    to->sip_mask            = from->sip_mask;
    to->dip                 = from->dip;
    to->dip_mask            = from->dip_mask;
    to->udp_dst_port        = from->udp_dst_port;
    to->udp_src_port        = from->udp_src_port;
    to->protocol            = from->protocol;
    to->mpls_label          = from->mpls_label;
    to->flow_port           = from->flow_port;
    to->vpn                 = from->vpn;
    to->intf_id             = from->intf_id;
    to->options             = from->options;
    to->criteria            = from->criteria;
    to->valid_elements      = from->valid_elements;
    to->flow_handle         = from->flow_handle;
    to->flow_option         = from->flow_option;
    to->flags               = from->flags;
    sal_memcpy(to->sip6,      from->sip6, sizeof(bcm_ip6_t));
    sal_memcpy(to->sip6_mask, from->sip6_mask, sizeof(bcm_ip6_t));
    sal_memcpy(to->dip6,      from->dip6, sizeof(bcm_ip6_t));
    sal_memcpy(to->dip6_mask, from->dip6_mask, sizeof(bcm_ip6_t));
    to->default_vlan        = from->default_vlan;
    to->default_vpn         = from->default_vpn;
}

int
bcm_compat6523_flow_match_add(
    int unit,
    bcm_compat6523_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_match_config_t *)
            sal_alloc(sizeof(bcm_flow_match_config_t), "New flow match config");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flow_match_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_add(unit, new_info, num_of_fields, field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6523out_flow_match_config_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6523_flow_match_delete(
    int unit,
    bcm_compat6523_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_match_config_t *)
            sal_alloc(sizeof(bcm_flow_match_config_t), "New flow match config");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flow_match_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_delete(unit, new_info, num_of_fields, field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6523out_flow_match_config_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6523_flow_match_get(
    int unit,
    bcm_compat6523_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_match_config_t *)
            sal_alloc(sizeof(bcm_flow_match_config_t), "New flow match config");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flow_match_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_get(unit, new_info, num_of_fields, field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6523out_flow_match_config_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6523_flow_match_flexctr_object_set(
    int unit,
    bcm_compat6523_flow_match_config_t *info,
    uint32 value)
{
    int rv;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_match_config_t *)
            sal_alloc(sizeof(bcm_flow_match_config_t), "New flow match config");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flow_match_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_flexctr_object_set(unit, new_info, value);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6523out_flow_match_config_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6523_flow_match_flexctr_object_get(
    int unit,
    bcm_compat6523_flow_match_config_t *info,
    uint32 *value)
{
    int rv;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_match_config_t *)
            sal_alloc(sizeof(bcm_flow_match_config_t), "New flow match config");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flow_match_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_flexctr_object_get(unit, new_info, value);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6523out_flow_match_config_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6523_flow_match_stat_attach(
    int unit,
    bcm_compat6523_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 stat_counter_id)
{
    int rv;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_match_config_t *)
            sal_alloc(sizeof(bcm_flow_match_config_t), "New flow match config");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flow_match_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_stat_attach(unit, new_info, num_of_fields, field, stat_counter_id);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6523out_flow_match_config_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6523_flow_match_stat_detach(
    int unit,
    bcm_compat6523_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_match_config_t *)
            sal_alloc(sizeof(bcm_flow_match_config_t), "New flow match config");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flow_match_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_stat_detach(unit, new_info, num_of_fields, field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6523out_flow_match_config_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6523_flow_match_stat_id_get(
    int unit,
    bcm_compat6523_flow_match_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 *stat_counter_id)
{
    int rv;
    bcm_flow_match_config_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_flow_match_config_t *)
            sal_alloc(sizeof(bcm_flow_match_config_t), "New flow match config");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flow_match_config_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_match_stat_id_get(unit, new_info, num_of_fields, field, stat_counter_id);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6523out_flow_match_config_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

#endif /** INCLUDE_L3 */

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6523in_flow_encap_config_t
 * Purpose:
 *      Convert the bcm_flow_encap_config_t datatype from <=6.5.20 to
 *      SDK 6.5.21+
 * Parameters:
 *      from        - (IN) The <=6.5.20 version of the datatype
 *      to          - (OUT) The SDK 6.5.21+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523in_flow_encap_config_t(
    bcm_compat6523_flow_encap_config_t *from,
    bcm_flow_encap_config_t *to)
{
    bcm_flow_encap_config_t_init(to);
    to->vpn = from->vpn;
    to->flow_port = from->flow_port;
    to->intf_id = from->intf_id;
    to->dvp_group = from->dvp_group;
    to->oif_group = from->oif_group;
    to->vnid = from->vnid;
    to->pri = from->pri;
    to->cfi = from->cfi;
    to->tpid = from->tpid;
    to->vlan = from->vlan;
    to->flags = from->flags;
    to->options = from->options;
    to->criteria = from->criteria;
    to->valid_elements = from->valid_elements;
    to->flow_handle = from->flow_handle;
    to->flow_option = from->flow_option;
    to->src_flow_port = from->src_flow_port;
    to->class_id = from->class_id;
    to->port_group = from->port_group;
    to->vlan_pri_map_id = from->vlan_pri_map_id;
}

/*
 * Function:
 *      _bcm_compat6523out_flow_encap_config_t
 * Purpose:
 *      Convert the bcm_flow_encap_config_t datatype from SDK 6.5.21+ to
 *      <=6.5.20
 * Parameters:
 *      from        - (IN) The SDK 6.5.21+ version of the datatype
 *      to          - (OUT) The <=6.5.20 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523out_flow_encap_config_t(
    bcm_flow_encap_config_t *from,
    bcm_compat6523_flow_encap_config_t *to)
{
    to->vpn = from->vpn;
    to->flow_port = from->flow_port;
    to->intf_id = from->intf_id;
    to->dvp_group = from->dvp_group;
    to->oif_group = from->oif_group;
    to->vnid = from->vnid;
    to->pri = from->pri;
    to->cfi = from->cfi;
    to->tpid = from->tpid;
    to->vlan = from->vlan;
    to->flags = from->flags;
    to->options = from->options;
    to->criteria = from->criteria;
    to->valid_elements = from->valid_elements;
    to->flow_handle = from->flow_handle;
    to->flow_option = from->flow_option;
    to->src_flow_port = from->src_flow_port;
    to->class_id = from->class_id;
    to->port_group = from->port_group;
    to->vlan_pri_map_id = from->vlan_pri_map_id;
}

/*
 * Function:
 *      bcm_compat6523_flow_encap_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) (IN/OUT) Flow encap configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_flow_encap_add(
    int unit,
    bcm_compat6523_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_encap_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_encap_config_t *)
                     sal_alloc(sizeof(bcm_flow_encap_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_add(unit, new_info, num_of_fields, field);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_flow_encap_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_delete.
 * Parameters:
 *      unit - (IN) BCM device number
 *      info - (IN) Flow encap configuration parameters to specify the key
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_flow_encap_delete(
    int unit,
    bcm_compat6523_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_encap_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_encap_config_t *)
                     sal_alloc(sizeof(bcm_flow_encap_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_delete(unit, new_info, num_of_fields, field);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_flow_encap_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) (IN/OUT) Flow encap configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (INOUT) field array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_flow_encap_get(
    int unit,
    bcm_compat6523_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_encap_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_encap_config_t *)
                     sal_alloc(sizeof(bcm_flow_encap_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_get(unit, new_info, num_of_fields, field);

    /* Transform the entry from the new format to old one */
    _bcm_compat6523out_flow_encap_config_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_flow_encap_flexctr_object_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_flexctr_object_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Flow encap configuration parameters
 *      value - (IN) The flex counter object value
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_flow_encap_flexctr_object_set(
    int unit,
    bcm_compat6523_flow_encap_config_t *info,
    uint32 value)
{
    int rv = BCM_E_NONE;
    bcm_flow_encap_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_encap_config_t *)
                     sal_alloc(sizeof(bcm_flow_encap_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_flexctr_object_set(unit, new_info, value);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_flow_encap_flexctr_object_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_flexctr_object_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Flow encap configuration parameters
 *      value - (OUT) The flex counter object value
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_flow_encap_flexctr_object_get(
    int unit,
    bcm_compat6523_flow_encap_config_t *info,
    uint32 *value)
{
    int rv = BCM_E_NONE;
    bcm_flow_encap_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_encap_config_t *)
                     sal_alloc(sizeof(bcm_flow_encap_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_flexctr_object_get(unit, new_info, value);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_flow_encap_stat_attach
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_stat_attach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Flow encap configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 *      stat_counter_id - (IN) Stat counter ID.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_flow_encap_stat_attach(
    int unit,
    bcm_compat6523_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_flow_encap_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_encap_config_t *)
                     sal_alloc(sizeof(bcm_flow_encap_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_stat_attach(unit, new_info, num_of_fields, field, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_flow_encap_stat_detach
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_stat_detach.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Flow encap configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 *      stat_counter_id - (IN) Stat counter ID.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_flow_encap_stat_detach(
    int unit,
    bcm_compat6523_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv = BCM_E_NONE;
    bcm_flow_encap_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_encap_config_t *)
                     sal_alloc(sizeof(bcm_flow_encap_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_stat_detach(unit, new_info, num_of_fields, field);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_flow_encap_stat_id_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flow_encap_stat_id_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (IN) Flow encap configuration parameters
 *      num_of_fields - (IN) number of elements of field array
 *      field - (IN) field array
 *      stat_counter_id - (OUT) Stat counter ID.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_flow_encap_stat_id_get(
    int unit,
    bcm_compat6523_flow_encap_config_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 *stat_counter_id)
{
    int rv = BCM_E_NONE;
    bcm_flow_encap_config_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_flow_encap_config_t *)
                     sal_alloc(sizeof(bcm_flow_encap_config_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flow_encap_config_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flow_encap_stat_id_get(unit, new_info, num_of_fields, field, stat_counter_id);


    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

#endif /* defined(INCLUDE_L3) */

#if defined(INCLUDE_L3)

/*
 * Function:
 *      _bcm_compat6523in_flow_tunnel_terminator_t
 * Purpose:
 *      Convert the bcm_flow_tunnel_terminator_t datatype from <=6.5.19 to 6.5.19+
 * Parameters:
 *      from        - (IN) The <=6.5.19 version of the datatype
 *      to          - (OUT) The 6.5.19+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523in_flow_tunnel_terminator_t(
    bcm_compat6523_flow_tunnel_terminator_t *from,
    bcm_compat6524_flow_tunnel_terminator_t *to)
{
    int i1 = 0;

    
    to->flags = from->flags;
    to->multicast_flag = from->multicast_flag;
    to->vrf = from->vrf;
    to->sip = from->sip;
    to->dip = from->dip;
    to->sip_mask = from->sip_mask;
    to->dip_mask = from->dip_mask;
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6[i1] = from->sip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6[i1] = from->dip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6_mask[i1] = from->sip6_mask[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6_mask[i1] = from->dip6_mask[i1];
    }
    to->udp_dst_port = from->udp_dst_port;
    to->udp_src_port = from->udp_src_port;
    to->type = from->type;
    to->vlan = from->vlan;
    to->protocol = from->protocol;
    to->valid_elements = from->valid_elements;
    to->flow_handle = from->flow_handle;
    to->flow_option = from->flow_option;
    to->vlan_mask   = from->vlan_mask;
    to->class_id    = from->class_id;
    to->term_pbmp   = from->term_pbmp;
}

/*
 * Function:
 *      _bcm_compat6523out_flow_tunnel_terminator_t
 * Purpose:
 *      Convert the bcm_flow_tunnel_terminator_t datatype f from 6.5.19+ to <=6.5.19.
 * Parameters:
 *      from        - (IN) The 6.5.19+ version of the datatype
 *      to          - (OUT) The <=6.5.19 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523out_flow_tunnel_terminator_t(
    bcm_compat6524_flow_tunnel_terminator_t *from,
    bcm_compat6523_flow_tunnel_terminator_t *to)
{
    int i1 = 0;

    to->flags = from->flags;
    to->multicast_flag = from->multicast_flag;
    to->vrf = from->vrf;
    to->sip = from->sip;
    to->dip = from->dip;
    to->sip_mask = from->sip_mask;
    to->dip_mask = from->dip_mask;
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6[i1] = from->sip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6[i1] = from->dip6[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->sip6_mask[i1] = from->sip6_mask[i1];
    }
    for (i1 = 0; i1 < 16; i1++) {
        to->dip6_mask[i1] = from->dip6_mask[i1];
    }
    to->udp_dst_port = from->udp_dst_port;
    to->udp_src_port = from->udp_src_port;
    to->type = from->type;
    to->vlan = from->vlan;
    to->protocol = from->protocol;
    to->valid_elements = from->valid_elements;
    to->flow_handle = from->flow_handle;
    to->flow_option = from->flow_option;
    to->vlan_mask   = from->vlan_mask;
    to->class_id    = from->class_id;
    to->term_pbmp   = from->term_pbmp;
}

int
bcm_compat6523_flow_tunnel_terminator_create(
    int unit,
    bcm_compat6523_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_compat6524_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_compat6524_flow_tunnel_terminator_t *)
            sal_alloc(sizeof(bcm_compat6524_flow_tunnel_terminator_t), "New flow port encap");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flow_tunnel_terminator_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6524_flow_tunnel_terminator_create(unit, new_info, num_of_fields, field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6523out_flow_tunnel_terminator_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6523_flow_tunnel_terminator_destroy(
    int unit,
    bcm_compat6523_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_compat6524_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_compat6524_flow_tunnel_terminator_t *)
            sal_alloc(sizeof(bcm_compat6524_flow_tunnel_terminator_t), "New flow port encap");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flow_tunnel_terminator_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6524_flow_tunnel_terminator_destroy(unit, new_info, num_of_fields, field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6523out_flow_tunnel_terminator_t(new_info, info);
        sal_free(new_info);
    }

    return rv;

}


int
bcm_compat6523_flow_tunnel_terminator_get(
    int unit,
    bcm_compat6523_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_compat6524_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_compat6524_flow_tunnel_terminator_t *)
            sal_alloc(sizeof(bcm_compat6524_flow_tunnel_terminator_t), "New flow port encap");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flow_tunnel_terminator_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6524_flow_tunnel_terminator_get(unit, new_info, num_of_fields, field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6523out_flow_tunnel_terminator_t(new_info, info);
        sal_free(new_info);
    }

    return rv;

}

int
bcm_compat6523_flow_tunnel_terminator_flexctr_object_set(
    int unit,
    bcm_compat6523_flow_tunnel_terminator_t *info,
    uint32 value)
{
    int rv;
    bcm_compat6524_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_compat6524_flow_tunnel_terminator_t *)
            sal_alloc(sizeof(bcm_compat6524_flow_tunnel_terminator_t), "New flow tunnel_terminator");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flow_tunnel_terminator_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6524_flow_tunnel_terminator_flexctr_object_set(unit, new_info, value);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6523out_flow_tunnel_terminator_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6523_flow_tunnel_terminator_flexctr_object_get(
    int unit,
    bcm_compat6523_flow_tunnel_terminator_t *info,
    uint32 *value)
{
    int rv;
    bcm_compat6524_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_compat6524_flow_tunnel_terminator_t *)
            sal_alloc(sizeof(bcm_compat6524_flow_tunnel_terminator_t), "New flow tunnel_terminator");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flow_tunnel_terminator_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6524_flow_tunnel_terminator_flexctr_object_get(unit, new_info, value);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6523out_flow_tunnel_terminator_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6523_flow_tunnel_terminator_stat_attach(
    int unit,
    bcm_compat6523_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 stat_counter_id)
{
    int rv;
    bcm_compat6524_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_compat6524_flow_tunnel_terminator_t *)
            sal_alloc(sizeof(bcm_compat6524_flow_tunnel_terminator_t), "New flow tunnel_terminator");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flow_tunnel_terminator_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6524_flow_tunnel_terminator_stat_attach(unit, new_info, num_of_fields,
                     field, stat_counter_id);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6523out_flow_tunnel_terminator_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6523_flow_tunnel_terminator_stat_detach(
    int unit,
    bcm_compat6523_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field)
{
    int rv;
    bcm_compat6524_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_compat6524_flow_tunnel_terminator_t *)
            sal_alloc(sizeof(bcm_compat6524_flow_tunnel_terminator_t), "New flow tunnel_terminator");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flow_tunnel_terminator_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6524_flow_tunnel_terminator_stat_detach(unit, new_info, num_of_fields,
                     field);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6523out_flow_tunnel_terminator_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

int
bcm_compat6523_flow_tunnel_terminator_stat_id_get(
    int unit,
    bcm_compat6523_flow_tunnel_terminator_t *info,
    uint32 num_of_fields,
    bcm_flow_logical_field_t *field,
    uint32 *stat_counter_id)
{
    int rv;
    bcm_compat6524_flow_tunnel_terminator_t *new_info = NULL;

    if (info != NULL) {
        /* Create from heap to avoid the stack to bloat */
        new_info = (bcm_compat6524_flow_tunnel_terminator_t *)
            sal_alloc(sizeof(bcm_compat6524_flow_tunnel_terminator_t), "New flow tunnel_terminator");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flow_tunnel_terminator_t(info, new_info);
    }

    /* Call the BCM API with new format */
    rv = bcm_compat6524_flow_tunnel_terminator_stat_id_get(unit, new_info, num_of_fields,
                     field, stat_counter_id);
    if (new_info != NULL) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6523out_flow_tunnel_terminator_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

#endif /* defined(INCLUDE_L3) */

/*
 * Function:
 *   _bcm_compat6523in_switch_pkt_integrity_match_t
 * Purpose:
 *   Convert the bcm_compat6523_switch_pkt_integrity_match_t datatype from
 *   <=6.5.23 to SDK 6.5.23+.
 * Parameters:
 *   from        - (IN) The <=6.5.23 version of the datatype.
 *   to          - (OUT) The SDK 6.5.23+ version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6523in_switch_pkt_integrity_match_t(
    bcm_compat6523_switch_pkt_integrity_match_t *from,
    bcm_switch_pkt_integrity_match_t *to)
{
    bcm_switch_pkt_integrity_match_t_init(to);
    to->tunnel_processing_results_1       = from->tunnel_processing_results_1;
    to->tunnel_processing_results_1_mask  = from->tunnel_processing_results_1_mask;
    to->mpls_flow_type                    = from->mpls_flow_type;
    to->mpls_flow_type_mask               = from->mpls_flow_type_mask;
    to->my_station_1_hit                  = from->my_station_1_hit;
    to->my_station_1_hit_mask             = from->my_station_1_hit_mask;
    to->l4_valid                          = from->l4_valid;
    to->l4_valid_mask                     = from->l4_valid_mask;
    to->icmp_type                         = from->icmp_type;
    to->icmp_type_mask                    = from->icmp_type_mask;
    to->ip_last_protocol                  = from->ip_last_protocol;
    to->ip_last_protocol_mask             = from->ip_last_protocol_mask;
    to->tcp_hdr_len_and_flags             = from->tcp_hdr_len_and_flags;
    to->tcp_hdr_len_and_flags_mask        = from->tcp_hdr_len_and_flags_mask;
    to->fixed_hve_result_0                = from->fixed_hve_result_0;
    to->fixed_hve_result_0_mask           = from->fixed_hve_result_0_mask;
    to->fixed_hve_result_1                = from->fixed_hve_result_1;
    to->fixed_hve_result_1_mask           = from->fixed_hve_result_1_mask;
    to->fixed_hve_result_2                = from->fixed_hve_result_2;
    to->fixed_hve_result_2_mask           = from->fixed_hve_result_2_mask;
    to->fixed_hve_result_3                = from->fixed_hve_result_3;
    to->fixed_hve_result_3_mask           = from->fixed_hve_result_3_mask;
    to->fixed_hve_result_4                = from->fixed_hve_result_4;
    to->fixed_hve_result_4_mask           = from->fixed_hve_result_4_mask;
    to->fixed_hve_result_5                = from->fixed_hve_result_5;
    to->fixed_hve_result_5_mask           = from->fixed_hve_result_5_mask;
    to->flex_hve_result_0                 = from->flex_hve_result_0;
    to->flex_hve_result_0_mask            = from->flex_hve_result_0_mask;
    to->flex_hve_result_1                 = from->flex_hve_result_1;
    to->flex_hve_result_1_mask            = from->flex_hve_result_1_mask;
    to->flex_hve_result_2                 = from->flex_hve_result_2;
    to->flex_hve_result_2_mask            = from->flex_hve_result_2_mask;
}

/*
 * Function:
 *   _bcm_compat6523out_switch_pkt_integrity_match_t
 * Purpose:
 *   Convert the bcm_switch_pkt_integrity_match_t datatype from 6.5.23+ to
 *   <=6.5.23.
 * Parameters:
 *   from     - (IN) The 6.5.23+ version of the datatype.
 *   to       - (OUT) The <=6.5.23 version of the datatype.
 * Returns:
 *   Nothing
 */
STATIC void
_bcm_compat6523out_switch_pkt_integrity_match_t(
    bcm_switch_pkt_integrity_match_t *from,
    bcm_compat6523_switch_pkt_integrity_match_t *to)
{
    to->tunnel_processing_results_1       = from->tunnel_processing_results_1;
    to->tunnel_processing_results_1_mask  = from->tunnel_processing_results_1_mask;
    to->mpls_flow_type                    = from->mpls_flow_type;
    to->mpls_flow_type_mask               = from->mpls_flow_type_mask;
    to->my_station_1_hit                  = from->my_station_1_hit;
    to->my_station_1_hit_mask             = from->my_station_1_hit_mask;
    to->l4_valid                          = from->l4_valid;
    to->l4_valid_mask                     = from->l4_valid_mask;
    to->icmp_type                         = from->icmp_type;
    to->icmp_type_mask                    = from->icmp_type_mask;
    to->ip_last_protocol                  = from->ip_last_protocol;
    to->ip_last_protocol_mask             = from->ip_last_protocol_mask;
    to->tcp_hdr_len_and_flags             = from->tcp_hdr_len_and_flags;
    to->tcp_hdr_len_and_flags_mask        = from->tcp_hdr_len_and_flags_mask;
    to->fixed_hve_result_0                = from->fixed_hve_result_0;
    to->fixed_hve_result_0_mask           = from->fixed_hve_result_0_mask;
    to->fixed_hve_result_1                = from->fixed_hve_result_1;
    to->fixed_hve_result_1_mask           = from->fixed_hve_result_1_mask;
    to->fixed_hve_result_2                = from->fixed_hve_result_2;
    to->fixed_hve_result_2_mask           = from->fixed_hve_result_2_mask;
    to->fixed_hve_result_3                = from->fixed_hve_result_3;
    to->fixed_hve_result_3_mask           = from->fixed_hve_result_3_mask;
    to->fixed_hve_result_4                = from->fixed_hve_result_4;
    to->fixed_hve_result_4_mask           = from->fixed_hve_result_4_mask;
    to->fixed_hve_result_5                = from->fixed_hve_result_5;
    to->fixed_hve_result_5_mask           = from->fixed_hve_result_5_mask;
    to->flex_hve_result_0                 = from->flex_hve_result_0;
    to->flex_hve_result_0_mask            = from->flex_hve_result_0_mask;
    to->flex_hve_result_1                 = from->flex_hve_result_1;
    to->flex_hve_result_1_mask            = from->flex_hve_result_1_mask;
    to->flex_hve_result_2                 = from->flex_hve_result_2;
    to->flex_hve_result_2_mask            = from->flex_hve_result_2_mask;
}

/*
 * Function:
 *   bcm_compat6523_switch_pkt_integrity_check_add
 * Purpose:
 *   Compatibility function for RPC call to bcm_switch_pkt_integrity_check_add.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   options        - (IN) Options flags.
 *   match          - (IN) Integrity match structure.
 *   action         - (IN) Packet control action.
 *   priority       - (IN) Entry priority.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6523_switch_pkt_integrity_check_add(
    int unit,
    uint32 options,
    bcm_compat6523_switch_pkt_integrity_match_t *match,
    bcm_switch_pkt_control_action_t *action,
    int priority)
{
    int rv = 0;
    bcm_switch_pkt_integrity_match_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_switch_pkt_integrity_match_t *)
            sal_alloc(sizeof(bcm_switch_pkt_integrity_match_t), "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6523in_switch_pkt_integrity_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_switch_pkt_integrity_check_add(unit, options, new_match, action, priority);

    if (rv >= 0) {
        _bcm_compat6523out_switch_pkt_integrity_match_t(new_match, match);
    }

    sal_free(new_match);
    return rv;
}

/*
 * Function:
 *   bcm_compat6523_switch_pkt_integrity_check_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_switch_pkt_integrity_check_get.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   match          - (IN) Integrity match structure.
 *   action         - (OUT) Packet control action.
 *   priority       - (OUT) Entry priority.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6523_switch_pkt_integrity_check_get(
    int unit,
    bcm_compat6523_switch_pkt_integrity_match_t *match,
    bcm_switch_pkt_control_action_t *action,
    int *priority)
{
    int rv = 0;
    bcm_switch_pkt_integrity_match_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_switch_pkt_integrity_match_t *)
            sal_alloc(sizeof(bcm_switch_pkt_integrity_match_t), "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6523in_switch_pkt_integrity_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_switch_pkt_integrity_check_get(unit, new_match, action, priority);

    if (rv >= 0) {
        _bcm_compat6523out_switch_pkt_integrity_match_t(new_match, match);
    }

    sal_free(new_match);
    return rv;
}

/*
 * Function:
 *   bcm_compat6523_switch_pkt_integrity_check_delete
 * Purpose:
 *   Compatibility function for RPC call to bcm_switch_pkt_integrity_check_delete.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   match          - (IN) Integrity match structure.
 * Returns:
 *   BCM_E_XXX
 */
int
bcm_compat6523_switch_pkt_integrity_check_delete(
    int unit,
    bcm_compat6523_switch_pkt_integrity_match_t *match)
{
    int rv = 0;
    bcm_switch_pkt_integrity_match_t *new_match = NULL;

    if (match != NULL) {
        new_match = (bcm_switch_pkt_integrity_match_t *)
            sal_alloc(sizeof(bcm_switch_pkt_integrity_match_t), "New match");
        if (new_match == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6523in_switch_pkt_integrity_match_t(match, new_match);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_switch_pkt_integrity_check_delete(unit, new_match);

    if (rv >= 0) {
        _bcm_compat6523out_switch_pkt_integrity_match_t(new_match, match);
    }

    sal_free(new_match);
    return rv;
}

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6523in_l3_egress_ecmp_t
 *
 * Purpose:
 *      Convert the bcm_l3_egress_ecmp_t datatype from <= 6.5.23 to SDK 6.5.24+.
 *
 * Parameters:
 *      from - (IN) <= SDK 6.5.23 version of the datatype.
 *      to   - (OUT) SDK 6.5.24+ version of the datatype.
 *
 * Returns:
 *      Nothing.
 */
STATIC void
_bcm_compat6523in_l3_egress_ecmp_t(bcm_compat6523_l3_egress_ecmp_t *from,
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
        to->rh_random_seed = from->rh_random_seed;
        to->user_profile = from->user_profile;
    }
    return;
}

/*
 * Function:
 *      _bcm_compat6523out_l3_egress_ecmp_t
 *
 * Purpose:
 *      Convert the bcm_l3_egress_ecmp_t datatype from SDK 6.5.24+ to <=6.5.23.
 *
 * Parameters:
 *      from - (IN) SDK 6.5.24+ version of the datatype.
 *      to   - (OUT) <= SDK 6.5.23 version of the datatype.
 *
 * Returns:
 *      Nothing.
 */
STATIC void
_bcm_compat6523out_l3_egress_ecmp_t(bcm_l3_egress_ecmp_t *from,
                                    bcm_compat6523_l3_egress_ecmp_t *to)
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
        to->rh_random_seed = from->rh_random_seed;
        to->user_profile = from->user_profile;
    }
    return;
}

/*
 * Function:
 *      _bcm_compat6523in_l3_egress_ecmp_resilient_entry_t
 *
 * Purpose:
 *      Convert the bcm_l3_egress_ecmp_resilient_entry_t datatype from <= 6.5.23
 *      to SDK 6.5.24+.
 *
 * Parameters:
 *      from - (IN) <= SDK 6.5.23 version of the datatype.
 *      to   - (OUT) SDK 6.5.24+ version of the datatype.
 *
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523in_l3_egress_ecmp_resilient_entry_t(
    bcm_compat6523_l3_egress_ecmp_resilient_entry_t *from,
    bcm_l3_egress_ecmp_resilient_entry_t *to)
{
    if (from && to) {
        to->hash_key = from->hash_key;
        (void)_bcm_compat6523in_l3_egress_ecmp_t(&from->ecmp, &to->ecmp);
        to->intf = from->intf;
    }
    return;
}

/*
 * Function:
 *      bcm_compat6523_l3_ecmp_create
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
bcm_compat6523_l3_ecmp_create(int unit,
                              uint32 options,
                              bcm_compat6523_l3_egress_ecmp_t *ecmp_info,
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
                                            "bcmcompat6523L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6523_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6523in_l3_egress_ecmp_t(ecmp_info, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_ecmp_create(unit, options, new, ecmp_member_count,
                            ecmp_member_array);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as ecmp_info is of INOUT parameter type for this API.
         * - bcm_l3_egress_ecmp_t -> bcm_compat6523_l3_egress_ecmp_t
         */
        (void)_bcm_compat6523out_l3_egress_ecmp_t(new, ecmp_info);
    }
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6523_l3_ecmp_get
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
bcm_compat6523_l3_ecmp_get(int unit,
                           bcm_compat6523_l3_egress_ecmp_t *ecmp_info,
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
                                            "bcmcompat6523L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6523_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6523in_l3_egress_ecmp_t(ecmp_info, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_ecmp_get(unit, new, ecmp_member_size, ecmp_member_array,
                         ecmp_member_count);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as ecmp_info is of INOUT parameter type for this API.
         * - bcm_l3_egress_ecmp_t -> bcm_compat6523_l3_egress_ecmp_t
         */
        (void)_bcm_compat6523out_l3_egress_ecmp_t(new, ecmp_info);
    }
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6523_l3_ecmp_find
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
bcm_compat6523_l3_ecmp_find(int unit,
                            int ecmp_member_count,
                            bcm_l3_ecmp_member_t *ecmp_member_array,
                            bcm_compat6523_l3_egress_ecmp_t *ecmp_info)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp_info == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6523L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6523_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6523in_l3_egress_ecmp_t(ecmp_info, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_ecmp_find(unit, ecmp_member_count, ecmp_member_array, new);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as ecmp_info is of OUT parameter type for this API.
         * - bcm_l3_egress_ecmp_t -> bcm_compat6523_l3_egress_ecmp_t
         */
        (void)_bcm_compat6523out_l3_egress_ecmp_t(new, ecmp_info);
    }
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6523_l3_egress_ecmp_create
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
bcm_compat6523_l3_egress_ecmp_create(int unit,
                                     bcm_compat6523_l3_egress_ecmp_t *ecmp,
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
                                            "bcmcompat6523L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6523_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6523in_l3_egress_ecmp_t(ecmp, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_create(unit, new, intf_count, intf_array);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as ecmp is of INOUT parameter type for this API.
         * - bcm_l3_egress_ecmp_t -> bcm_compat6523_l3_egress_ecmp_t
         */
        (void)_bcm_compat6523out_l3_egress_ecmp_t(new, ecmp);
    }
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6523_l3_egress_ecmp_destroy
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
bcm_compat6523_l3_egress_ecmp_destroy(int unit,
                                      bcm_compat6523_l3_egress_ecmp_t *ecmp)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6523L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6523_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6523in_l3_egress_ecmp_t(ecmp, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_destroy(unit, new);
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6523_l3_egress_ecmp_get
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
bcm_compat6523_l3_egress_ecmp_get(int unit,
                                  bcm_compat6523_l3_egress_ecmp_t *ecmp,
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
                                            "bcmcompat6523L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6523_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6523in_l3_egress_ecmp_t(ecmp, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_get(unit, new, intf_size, intf_array, intf_count);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as ecmp is of INOUT parameter type for this API.
         * - bcm_l3_egress_ecmp_t -> bcm_compat6523_l3_egress_ecmp_t
         */
        (void)_bcm_compat6523out_l3_egress_ecmp_t(new, ecmp);
    }
    if (new) {
        sal_free(new);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_l3_egress_ecmp_add
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
bcm_compat6523_l3_egress_ecmp_add(int unit,
                                  bcm_compat6523_l3_egress_ecmp_t *ecmp,
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
                                            "bcmcompat6523L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6523_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6523in_l3_egress_ecmp_t(ecmp, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_add(unit, new, intf);
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6523_l3_egress_ecmp_delete
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
bcm_compat6523_l3_egress_ecmp_delete(int unit,
                                     bcm_compat6523_l3_egress_ecmp_t *ecmp,
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
                                            "bcmcompat6523L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6523_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6523in_l3_egress_ecmp_t(ecmp, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_egress_ecmp_delete(unit, new, intf);
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6523_l3_egress_ecmp_find
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
bcm_compat6523_l3_egress_ecmp_find(int unit,
                                   int intf_count,
                                   bcm_if_t *intf_array,
                                   bcm_compat6523_l3_egress_ecmp_t *ecmp)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_t *new = NULL;

    /* Validate input parameter. */
    if (ecmp == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_egress_ecmp_t *)sal_alloc(sizeof(*new),
                                            "bcmcompat6523L3EgrEcmpNew");
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
         * - bcm_l3_egress_ecmp_t -> bcm_compat6523_l3_egress_ecmp_t
         */
        (void)_bcm_compat6523out_l3_egress_ecmp_t(new, ecmp);
    }
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6523_l3_egress_ecmp_tunnel_priority_set
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
bcm_compat6523_l3_egress_ecmp_tunnel_priority_set(
    int unit,
    bcm_compat6523_l3_egress_ecmp_t *ecmp,
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
                                            "bcmcompat6523L3EgrEcmpNew");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6523_l3_egress_ecmp_t -> bcm_l3_egress_ecmp_t.
     */
    (void)_bcm_compat6523in_l3_egress_ecmp_t(ecmp, new);

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
 *      bcm_compat6523_l3_egress_ecmp_resilient_replace
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
bcm_compat6523_l3_egress_ecmp_resilient_replace(
    int unit,
    uint32 flags,
    bcm_compat6523_l3_egress_ecmp_resilient_entry_t *match_entry,
    int *num_entries,
    bcm_compat6523_l3_egress_ecmp_resilient_entry_t *replace_entry)
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
                              "bcmcompat6523L3EgrEcmpRhNewMatchEnt");
    if (new_match == NULL) {
        return (BCM_E_MEMORY);
    }
    sal_memset(new_match, 0, sizeof(*new_match));
    new_replace = (bcm_l3_egress_ecmp_resilient_entry_t *)
                        sal_alloc(sizeof(*new_replace),
                                  "bcmcompat6523L3EgrEcmpRhNewReplaceEnt");
    if (new_replace == NULL) {
        sal_free(new_match);
        return (BCM_E_MEMORY);
    }
    sal_memset(new_replace, 0, sizeof(*new_replace));
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6523_l3_egress_ecmp_resilient_entry_t
     *      -> bcm_l3_egress_ecmp_resilient_entry_t.
     */
    (void)_bcm_compat6523in_l3_egress_ecmp_resilient_entry_t(match_entry,
                                                             new_match);
    (void)_bcm_compat6523in_l3_egress_ecmp_resilient_entry_t(replace_entry,
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
 *      bcm_compat6523_l3_egress_ecmp_resilient_add
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
bcm_compat6523_l3_egress_ecmp_resilient_add(
    int unit,
    uint32 flags,
    bcm_compat6523_l3_egress_ecmp_resilient_entry_t *entry)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_resilient_entry_t *new = NULL;

    /* Validate input parameters for NULL ptr. */
    if (entry == NULL) {
        return (BCM_E_PARAM);
    }

    new = (bcm_l3_egress_ecmp_resilient_entry_t *)
                sal_alloc(sizeof(*new), "bcmcompat6523L3EgrEcmpRhNewEnt");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    sal_memset(new, 0, sizeof(*new));
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6523_l3_egress_ecmp_resilient_entry_t
     *      -> bcm_l3_egress_ecmp_resilient_entry_t.
     */
    (void)_bcm_compat6523in_l3_egress_ecmp_resilient_entry_t(entry, new);
    rv = bcm_l3_egress_ecmp_resilient_add(unit, flags, new);
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      bcm_compat6523_l3_egress_ecmp_resilient_delete
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
bcm_compat6523_l3_egress_ecmp_resilient_delete(
    int unit,
    uint32 flags,
    bcm_compat6523_l3_egress_ecmp_resilient_entry_t *entry)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_egress_ecmp_resilient_entry_t *new = NULL;

    /* Validate input parameters for NULL ptr. */
    if (entry == NULL) {
        return (BCM_E_PARAM);
    }

    new = (bcm_l3_egress_ecmp_resilient_entry_t *)
                sal_alloc(sizeof(*new), "bcmcompat6523L3EgrEcmpRhNewEnt");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    sal_memset(new, 0, sizeof(*new));
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6523_l3_egress_ecmp_resilient_entry_t
     *      -> bcm_l3_egress_ecmp_resilient_entry_t.
     */
    (void)_bcm_compat6523in_l3_egress_ecmp_resilient_entry_t(entry, new);
    rv = bcm_l3_egress_ecmp_resilient_delete(unit, flags, new);
    if (new) {
        sal_free(new);
    }
    return rv;
}

/*
 * Function:
 *      _bcm_compat6523in_l3_info_t
 *
 * Purpose:
 *      Convert the bcm_l3_info_t datatype from <= 6.5.23 to SDK 6.5.24+.
 *
 * Parameters:
 *      from - (IN) <= SDK 6.5.23 version of the datatype.
 *      to   - (OUT) SDK 6.5.24+ version of the datatype.
 *
 * Returns:
 *      Nothing.
 */
STATIC void
_bcm_compat6523in_l3_info_t(bcm_compat6523_l3_info_t *from,
                            bcm_l3_info_t *to)
{
    if (from && to) {
        bcm_l3_info_t_init(to);
        to->l3info_max_vrf = from->l3info_max_vrf;
        to->l3info_used_vrf = from->l3info_used_vrf;
        to->l3info_max_intf = from->l3info_max_intf;
        to->l3info_max_intf_group = from->l3info_max_intf_group;
        to->l3info_max_host = from->l3info_max_host;
        to->l3info_max_route = from->l3info_max_route;
        to->l3info_max_ecmp_groups = from->l3info_max_ecmp_groups;
        to->l3info_max_ecmp = from->l3info_max_ecmp;
        to->l3info_used_intf = from->l3info_used_intf;
        to->l3info_used_host = from->l3info_used_host;
        to->l3info_used_route = from->l3info_used_route;
        to->l3info_max_lpm_block = from->l3info_max_lpm_block;
        to->l3info_used_lpm_block = from->l3info_used_lpm_block;
        to->l3info_max_l3 = from->l3info_max_l3;
        to->l3info_max_defip = from->l3info_max_defip;
        to->l3info_used_l3 = from->l3info_used_l3;
        to->l3info_used_defip = from->l3info_used_defip;
        to->l3info_max_nexthop = from->l3info_max_nexthop;
        to->l3info_used_nexthop = from->l3info_used_nexthop;
        to->l3info_max_tunnel_init = from->l3info_max_tunnel_init;
        to->l3info_used_tunnel_init = from->l3info_used_tunnel_init;
        to->l3info_max_tunnel_term = from->l3info_max_tunnel_term;
        to->l3info_used_tunnel_term = from->l3info_used_tunnel_term;
        to->l3info_used_host_ip4 = from->l3info_used_host_ip4;
        to->l3info_used_host_ip6 = from->l3info_used_host_ip6;
    }
    return;
}

/*
 * Function:
 *      _bcm_compat6523out_l3_info_t
 *
 * Purpose:
 *      Convert the bcm_l3_info_t datatype from SDK 6.5.24+ to <=6.5.23.
 *
 * Parameters:
 *      from - (IN) SDK 6.5.24+ version of the datatype.
 *      to   - (OUT) <= SDK 6.5.23 version of the datatype.
 *
 * Returns:
 *      Nothing.
 */
STATIC void
_bcm_compat6523out_l3_info_t(bcm_l3_info_t *from,
                             bcm_compat6523_l3_info_t *to)
{
    if (from && to) {
        to->l3info_max_vrf = from->l3info_max_vrf;
        to->l3info_used_vrf = from->l3info_used_vrf;
        to->l3info_max_intf = from->l3info_max_intf;
        to->l3info_max_intf_group = from->l3info_max_intf_group;
        to->l3info_max_host = from->l3info_max_host;
        to->l3info_max_route = from->l3info_max_route;
        to->l3info_max_ecmp_groups = from->l3info_max_ecmp_groups;
        to->l3info_max_ecmp = from->l3info_max_ecmp;
        to->l3info_used_intf = from->l3info_used_intf;
        to->l3info_used_host = from->l3info_used_host;
        to->l3info_used_route = from->l3info_used_route;
        to->l3info_max_lpm_block = from->l3info_max_lpm_block;
        to->l3info_used_lpm_block = from->l3info_used_lpm_block;
        to->l3info_max_l3 = from->l3info_max_l3;
        to->l3info_max_defip = from->l3info_max_defip;
        to->l3info_used_l3 = from->l3info_used_l3;
        to->l3info_used_defip = from->l3info_used_defip;
        to->l3info_max_nexthop = from->l3info_max_nexthop;
        to->l3info_used_nexthop = from->l3info_used_nexthop;
        to->l3info_max_tunnel_init = from->l3info_max_tunnel_init;
        to->l3info_used_tunnel_init = from->l3info_used_tunnel_init;
        to->l3info_max_tunnel_term = from->l3info_max_tunnel_term;
        to->l3info_used_tunnel_term = from->l3info_used_tunnel_term;
        to->l3info_used_host_ip4 = from->l3info_used_host_ip4;
        to->l3info_used_host_ip6 = from->l3info_used_host_ip6;
    }
    return;
}

/*
 * Function:
 *      bcm_compat6523_l3_ecmp_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_l3_info API.
 *
 * Parameters:
 *      unit    - (IN) Unit number.
 *      l3info  - (INOUT) Available L3 information.
 *
 * Returns:
 *      BCM_E_NONE - Success.
 *      BCM_E_PARAM - Invalid parameter.
 *      BCM_E_MEMORY - Memory allocation failure.
 */
int
bcm_compat6523_l3_info(int unit,
                       bcm_compat6523_l3_info_t *l3info)
{
    int rv = BCM_E_INTERNAL;
    bcm_l3_info_t *new = NULL;

    /* Validate input parameter. */
    if (l3info == NULL) {
        return (BCM_E_PARAM);
    }
    /* Allocate */
    new = (bcm_l3_info_t *)sal_alloc(sizeof(*new), "bcmcompat6523L3Info");
    if (new == NULL) {
        return (BCM_E_MEMORY);
    }
    /*
     * Map the structure members from the old format of the structure to the
     * new format.
     * - bcm_compat6523_l3_info_t -> bcm_l3_info_t.
     */
    (void)_bcm_compat6523in_l3_info_t(l3info, new);

    /* Invoke the BCM API with the new format of the structure. */
    rv = bcm_l3_info(unit, new);
    if (BCM_SUCCESS(rv)) {
        /*
         * Re-map the structure members from the new format to the old format
         * as l3info is of OUT parameter type for this API.
         * - bcm_l3_info_t -> bcm_compat6523_l3_info_t
         */
        (void)_bcm_compat6523out_l3_info_t(new, l3info);
    }
    if (new) {
        sal_free(new);
    }
    return rv;
}
#endif /* INCLUDE_L3 */

/*
 * Function:
 *      _bcm_compat6523in_switch_drop_event_mon_t
 * Purpose:
 *      Convert the bcm_switch_drop_event_mon_t datatype from <=6.5.23 to
 *      SDK 6.5.24+
 * Parameters:
 *      from        - (IN) The <=6.5.23 version of the datatype
 *      to          - (OUT) The SDK 6.5.24+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523in_switch_drop_event_mon_t(
    bcm_compat6523_switch_drop_event_mon_t *from,
    bcm_switch_drop_event_mon_t *to)
{
    bcm_switch_drop_event_mon_t_init(to);
    to->drop_event = from->drop_event;
    to->actions = from->actions;
}

/*
 * Function:
 *      _bcm_compat6523out_switch_drop_event_mon_t
 * Purpose:
 *      Convert the bcm_switch_drop_event_mon_t datatype from SDK 6.5.24+ to
 *      <=6.5.23
 * Parameters:
 *      from        - (IN) The SDK 6.5.24+ version of the datatype
 *      to          - (OUT) The <=6.5.23 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523out_switch_drop_event_mon_t(
    bcm_switch_drop_event_mon_t *from,
    bcm_compat6523_switch_drop_event_mon_t *to)
{
    to->drop_event = from->drop_event;
    to->actions = from->actions;
}

int bcm_compat6523_switch_drop_event_mon_get(
    int unit,
    bcm_compat6523_switch_drop_event_mon_t *monitor)
{
    int rv;
    bcm_switch_drop_event_mon_t *new_monitor = NULL;

    if (NULL != monitor) {
        /* Create from heap to avoid the stack to bloat */
        new_monitor = sal_alloc(sizeof(bcm_switch_drop_event_mon_t), "New Monitor");
        if (NULL == new_monitor) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_switch_drop_event_mon_t(monitor, new_monitor);
    }

    /* Call the BCM API with new format */
    rv = bcm_switch_drop_event_mon_get(unit, new_monitor);

    if (NULL != new_monitor) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6523out_switch_drop_event_mon_t(new_monitor, monitor);
        /* Deallocate memory*/
        sal_free(new_monitor);
    }

    return rv;
}

int bcm_compat6523_switch_drop_event_mon_set(
    int unit,
    bcm_compat6523_switch_drop_event_mon_t *monitor)
{
    int rv;
    bcm_switch_drop_event_mon_t *new_monitor = NULL;

    if (NULL != monitor) {
        /* Create from heap to avoid the stack to bloat */
        new_monitor = sal_alloc(sizeof(bcm_switch_drop_event_mon_t), "New Monitor");
        if (NULL == new_monitor) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_switch_drop_event_mon_t(monitor, new_monitor);
    }

    /* Call the BCM API with new format */
    rv = bcm_switch_drop_event_mon_set(unit, new_monitor);

    if (NULL != new_monitor) {
        /* Deallocate memory*/
        sal_free(new_monitor);
    }

    return rv;
}

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6523in_tunnel_initiator_t
 * Purpose:
 *      Convert the bcm_tunnel_initiator_t datatype from <=6.5.23 to
 *      SDK 6.5.24+
 * Parameters:
 *      from        - (IN) The <=6.5.23 version of the datatype
 *      to          - (OUT) The SDK 6.5.24+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523in_tunnel_initiator_t(
    bcm_compat6523_tunnel_initiator_t *from,
    bcm_tunnel_initiator_t *to)
{
    int i1 = 0;

    bcm_tunnel_initiator_t_init(to);
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
    to->mtu = from->mtu;
    to->vlan = from->vlan;
    to->tpid = from->tpid;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->ip4_id = from->ip4_id;
    to->l3_intf_id = from->l3_intf_id;
    to->span_id = from->span_id;
    to->aux_data = from->aux_data;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->encap_access = from->encap_access;
    to->hw_id = from->hw_id;
    to->switch_id = from->switch_id;
    to->class_id = from->class_id;
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
    to->qos_map_id = from->qos_map_id;
    to->ecn = from->ecn;
    to->flow_label_sel = from->flow_label_sel;
    to->dscp_ecn_sel = from->dscp_ecn_sel;
    to->dscp_ecn_map = from->dscp_ecn_map;
    to->stat_cmd = from->stat_cmd;
    to->counter_command_id = from->counter_command_id;
    to->estimated_encap_size = from->estimated_encap_size;
}

/*
 * Function:
 *      _bcm_compat6523out_tunnel_initiator_t
 * Purpose:
 *      Convert the bcm_tunnel_initiator_t datatype from SDK 6.5.24+ to
 *      <=6.5.23
 * Parameters:
 *      from        - (IN) The SDK 6.5.24+ version of the datatype
 *      to          - (OUT) The <=6.5.23 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523out_tunnel_initiator_t(
    bcm_tunnel_initiator_t *from,
    bcm_compat6523_tunnel_initiator_t *to)
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
    to->mtu = from->mtu;
    to->vlan = from->vlan;
    to->tpid = from->tpid;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->ip4_id = from->ip4_id;
    to->l3_intf_id = from->l3_intf_id;
    to->span_id = from->span_id;
    to->aux_data = from->aux_data;
    to->outlif_counting_profile = from->outlif_counting_profile;
    to->encap_access = from->encap_access;
    to->hw_id = from->hw_id;
    to->switch_id = from->switch_id;
    to->class_id = from->class_id;
    sal_memcpy(&to->egress_qos_model, &from->egress_qos_model, sizeof(bcm_qos_egress_model_t));
    to->qos_map_id = from->qos_map_id;
    to->ecn = from->ecn;
    to->flow_label_sel = from->flow_label_sel;
    to->dscp_ecn_sel = from->dscp_ecn_sel;
    to->dscp_ecn_map = from->dscp_ecn_map;
    to->stat_cmd = from->stat_cmd;
    to->counter_command_id = from->counter_command_id;
    to->estimated_encap_size = from->estimated_encap_size;
}

/*
 * Function:
 *      bcm_compat6523_tunnel_initiator_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_initiator_set.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN) 
 *      tunnel - (IN) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_tunnel_initiator_set(
    int unit,
    bcm_l3_intf_t *intf,
    bcm_compat6523_tunnel_initiator_t *tunnel)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_tunnel = NULL;

    if (tunnel != NULL) {
        new_tunnel = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New tunnel");
        if (new_tunnel == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_tunnel_initiator_t(tunnel, new_tunnel);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_initiator_set(unit, intf, new_tunnel);


    /* Deallocate memory*/
    sal_free(new_tunnel);

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_tunnel_initiator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_initiator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (INOUT) 
 *      tunnel - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_tunnel_initiator_create(
    int unit,
    bcm_l3_intf_t *intf,
    bcm_compat6523_tunnel_initiator_t *tunnel)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_tunnel = NULL;

    if (tunnel != NULL) {
        new_tunnel = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New tunnel");
        if (new_tunnel == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_tunnel_initiator_t(tunnel, new_tunnel);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_initiator_create(unit, intf, new_tunnel);

    /* Transform the entry from the new format to old one */
    _bcm_compat6523out_tunnel_initiator_t(new_tunnel, tunnel);

    /* Deallocate memory*/
    sal_free(new_tunnel);

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_tunnel_initiator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_tunnel_initiator_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      intf - (IN) 
 *      tunnel - (INOUT) 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_tunnel_initiator_get(
    int unit,
    bcm_l3_intf_t *intf,
    bcm_compat6523_tunnel_initiator_t *tunnel)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_tunnel = NULL;

    if (tunnel != NULL) {
        new_tunnel = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New tunnel");
        if (new_tunnel == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_tunnel_initiator_t(tunnel, new_tunnel);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_tunnel_initiator_get(unit, intf, new_tunnel);

    /* Transform the entry from the new format to old one */
    _bcm_compat6523out_tunnel_initiator_t(new_tunnel, tunnel);

    /* Deallocate memory*/
    sal_free(new_tunnel);

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_vxlan_tunnel_initiator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_tunnel_initiator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Initiator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_vxlan_tunnel_initiator_create(
    int unit,
    bcm_compat6523_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_tunnel_initiator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6523out_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_vxlan_tunnel_initiator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_tunnel_initiator_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Initiator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_vxlan_tunnel_initiator_get(
    int unit,
    bcm_compat6523_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_tunnel_initiator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6523out_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_wlan_tunnel_initiator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_wlan_tunnel_initiator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) (IN/OUT) Tunnel initiator structure. \ref bcm_tunnel_initiator_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_wlan_tunnel_initiator_create(
    int unit,
    bcm_compat6523_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_wlan_tunnel_initiator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6523out_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_wlan_tunnel_initiator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_wlan_tunnel_initiator_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel initiator structure. \ref bcm_tunnel_initiator_t
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_wlan_tunnel_initiator_get(
    int unit,
    bcm_compat6523_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_wlan_tunnel_initiator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6523out_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_l2gre_tunnel_initiator_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2gre_tunnel_initiator_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      info - (INOUT) Tunnel Initiator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_l2gre_tunnel_initiator_create(
    int unit,
    bcm_compat6523_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2gre_tunnel_initiator_create(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6523out_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_l2gre_tunnel_initiator_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2gre_tunnel_initiator_get.
 * Parameters:
 *      unit - (IN) BCM device number
 *      info - (INOUT) Tunnel Initiator Info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_l2gre_tunnel_initiator_get(
    int unit,
    bcm_compat6523_tunnel_initiator_t *info)
{
    int rv = BCM_E_NONE;
    bcm_tunnel_initiator_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_tunnel_initiator_t *)
                     sal_alloc(sizeof(bcm_tunnel_initiator_t),
                     "New info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_tunnel_initiator_t(info, new_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2gre_tunnel_initiator_get(unit, new_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6523out_tunnel_initiator_t(new_info, info);

    /* Deallocate memory*/
    sal_free(new_info);

    return rv;
}


#endif /* defined(INCLUDE_L3) */

/*
 * Function:
 *      _bcm_compat6523out_port_config_t
 * Purpose:
 *      Convert the bcm_port_config_t datatype from SDK 6.5.24+ to
 *      <=6.5.23
 * Parameters:
 *      from        - (IN) The SDK 6.5.24+ version of the datatype
 *      to          - (OUT) The <=6.5.23 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523out_port_config_t(
    bcm_port_config_t *from,
    bcm_compat6523_port_config_t *to)
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
    to->lb = from->lb;
}

/*
 * Function:
 *      bcm_compat6523_port_config_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_config_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      config - (OUT) Pointer to port configuration structure populated on successful return.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_port_config_get(
    int unit,
    bcm_compat6523_port_config_t *config)
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
    _bcm_compat6523out_port_config_t(new_config, config);

    /* Deallocate memory*/
    sal_free(new_config);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6523in_flowtracker_flow_key_t
 * Purpose:
 *      Convert the bcm_flowtracker_flow_key_t datatype from <=6.5.23 to
 *      SDK 6.5.24+
 * Parameters:
 *      from        - (IN) The <=6.5.23 version of the datatype
 *      to          - (OUT) The SDK 6.5.24+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523in_flowtracker_flow_key_t(
    bcm_compat6523_flowtracker_flow_key_t *from,
    bcm_flowtracker_flow_key_t *to)
{
    int i1 = 0;

    bcm_flowtracker_flow_key_t_init(to);
    sal_memcpy(&to->src_ip, &from->src_ip, sizeof(bcm_ip_addr_t));
    sal_memcpy(&to->dst_ip, &from->dst_ip, sizeof(bcm_ip_addr_t));
    to->l4_src_port = from->l4_src_port;
    to->l4_dst_port = from->l4_dst_port;
    to->ip_protocol = from->ip_protocol;
    sal_memcpy(&to->inner_src_ip, &from->inner_src_ip, sizeof(bcm_ip_addr_t));
    sal_memcpy(&to->inner_dst_ip, &from->inner_dst_ip, sizeof(bcm_ip_addr_t));
    to->inner_l4_src_port = from->inner_l4_src_port;
    to->inner_l4_dst_port = from->inner_l4_dst_port;
    to->inner_ip_protocol = from->inner_ip_protocol;
    to->vxlan_network_id = from->vxlan_network_id;
    for (i1 = 0; i1 < BCM_FLOWTRACKER_CUSTOM_KEY_MAX_LENGTH; i1++) {
        to->custom[i1] = from->custom[i1];
    }
    to->in_port = from->in_port;
}

/*
 * Function:
 *      _bcm_compat6523out_flowtracker_flow_key_t
 * Purpose:
 *      Convert the bcm_flowtracker_flow_key_t datatype from SDK 6.5.24+ to
 *      <=6.5.23
 * Parameters:
 *      from        - (IN) The SDK 6.5.24+ version of the datatype
 *      to          - (OUT) The <=6.5.23 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523out_flowtracker_flow_key_t(
    bcm_flowtracker_flow_key_t *from,
    bcm_compat6523_flowtracker_flow_key_t *to)
{
    int i1 = 0;

    sal_memcpy(&to->src_ip, &from->src_ip, sizeof(bcm_ip_addr_t));
    sal_memcpy(&to->dst_ip, &from->dst_ip, sizeof(bcm_ip_addr_t));
    to->l4_src_port = from->l4_src_port;
    to->l4_dst_port = from->l4_dst_port;
    to->ip_protocol = from->ip_protocol;
    sal_memcpy(&to->inner_src_ip, &from->inner_src_ip, sizeof(bcm_ip_addr_t));
    sal_memcpy(&to->inner_dst_ip, &from->inner_dst_ip, sizeof(bcm_ip_addr_t));
    to->inner_l4_src_port = from->inner_l4_src_port;
    to->inner_l4_dst_port = from->inner_l4_dst_port;
    to->inner_ip_protocol = from->inner_ip_protocol;
    to->vxlan_network_id = from->vxlan_network_id;
    for (i1 = 0; i1 < BCM_FLOWTRACKER_CUSTOM_KEY_MAX_LENGTH; i1++) {
        to->custom[i1] = from->custom[i1];
    }
    to->in_port = from->in_port;
}

/*
 * Function:
 *      bcm_compat6523_flowtracker_group_data_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_flowtracker_group_data_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flow_group_id - (IN) Flow group ID to which collector and template needs to be deleted.
 *      flow_key - (IN) Parameters that define a flow
 *      flow_data - (OUT) Data associated with a flow
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_flowtracker_group_data_get(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_compat6523_flowtracker_flow_key_t *flow_key,
    bcm_flowtracker_flow_data_t *flow_data)
{
    int rv = BCM_E_NONE;
    bcm_flowtracker_flow_key_t *new_flow_key = NULL;

    if (flow_key != NULL) {
        new_flow_key = (bcm_flowtracker_flow_key_t *)
                     sal_alloc(sizeof(bcm_flowtracker_flow_key_t),
                     "New flow_key");
        if (new_flow_key == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flowtracker_flow_key_t(flow_key, new_flow_key);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flowtracker_group_data_get(unit, flow_group_id, new_flow_key, flow_data);


    /* Deallocate memory*/
    sal_free(new_flow_key);

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_flowtracker_static_flow_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_flowtracker_static_flow_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flow_group_id - (IN) Flowtracker flow group software ID.
 *      flow_key - (IN) The packet fields which constitutes a flow.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_flowtracker_static_flow_add(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_compat6523_flowtracker_flow_key_t *flow_key)
{
    int rv = BCM_E_NONE;
    bcm_flowtracker_flow_key_t *new_flow_key = NULL;

    if (flow_key != NULL) {
        new_flow_key = (bcm_flowtracker_flow_key_t *)
                     sal_alloc(sizeof(bcm_flowtracker_flow_key_t),
                     "New flow_key");
        if (new_flow_key == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flowtracker_flow_key_t(flow_key, new_flow_key);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flowtracker_static_flow_add(unit, flow_group_id, new_flow_key);


    /* Deallocate memory*/
    sal_free(new_flow_key);

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_flowtracker_static_flow_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_flowtracker_static_flow_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flow_group_id - (IN) Flowtracker flow group software ID.
 *      flow_key - (IN) The packet fields which constitutes a flow.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_flowtracker_static_flow_delete(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    bcm_compat6523_flowtracker_flow_key_t *flow_key)
{
    int rv = BCM_E_NONE;
    bcm_flowtracker_flow_key_t *new_flow_key = NULL;

    if (flow_key != NULL) {
        new_flow_key = (bcm_flowtracker_flow_key_t *)
                     sal_alloc(sizeof(bcm_flowtracker_flow_key_t),
                     "New flow_key");
        if (new_flow_key == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_flowtracker_flow_key_t(flow_key, new_flow_key);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flowtracker_static_flow_delete(unit, flow_group_id, new_flow_key);


    /* Deallocate memory*/
    sal_free(new_flow_key);

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_flowtracker_static_flow_get_all
 * Purpose:
 *      Compatibility function for RPC call to bcm_flowtracker_static_flow_get_all.
 * Parameters:
 *      unit - (IN) Unit number.
 *      flow_group_id - (IN) Flowtracker flow group software ID.
 *      max_size - (IN) Max number of flow keys that can be accomodated within flow_key_arr array.
 *      flow_key_arr - (OUT) List of static flows' keys.
 *      list_size - (OUT) The actual number of flow keys that were filled in flow_key_arr.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_flowtracker_static_flow_get_all(
    int unit,
    bcm_flowtracker_group_t flow_group_id,
    int max_size,
    bcm_compat6523_flowtracker_flow_key_t *flow_key_arr,
    int *list_size)
{
    int rv = BCM_E_NONE;
    bcm_flowtracker_flow_key_t *new_flow_key_arr = NULL;
    int i = 0;

    if (flow_key_arr != NULL && max_size > 0) {
        new_flow_key_arr = (bcm_flowtracker_flow_key_t *)
                     sal_alloc(max_size * sizeof(bcm_flowtracker_flow_key_t),
                     "New flow_key_arr");
        if (new_flow_key_arr == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_flowtracker_static_flow_get_all(unit, flow_group_id, max_size, new_flow_key_arr, list_size);

    for (i = 0; i < max_size; i++) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6523out_flowtracker_flow_key_t(&new_flow_key_arr[i], &flow_key_arr[i]);
    }

    /* Deallocate memory*/
    sal_free(new_flow_key_arr);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6523in_oam_loopback_t
 *
 * Purpose:
 *      Convert the bcm_oam_loopback_t datatype from <= 6.5.23 to SDK 6.5.24+.
 *
 * Parameters:
 *      from - (IN) <= SDK 6.5.23 version of the datatype.
 *      to   - (OUT) SDK 6.5.24+ version of the datatype.
 *
 * Returns:
 *      Nothing.
 */
STATIC void
_bcm_compat6523in_oam_loopback_t(bcm_compat6523_oam_loopback_t *from,
                                   bcm_oam_loopback_t *to)
{
    int i = 0;
    if (from && to) {
        bcm_oam_loopback_t_init(to);
        to->flags = from->flags;
        to->id = from->id;
        to->remote_id = from->remote_id;
        to->period = from->period;
        to->ttl = from->ttl;
        sal_memcpy(&to->discovered_me,
                   &from->discovered_me,
                   sizeof(bcm_oam_loopback_discovery_record_t));
        to->rx_count = from->rx_count;
        to->tx_count = from->tx_count;
        to->drop_count = from->drop_count;
        to->unexpected_response = from->unexpected_response;
        to->out_of_sequence = from->out_of_sequence;
        to->local_mipid_missmatch = from->local_mipid_missmatch;
        to->remote_mipid_missmatch = from->remote_mipid_missmatch;
        to->invalid_target_mep_tlv = from->invalid_target_mep_tlv;
        to->invalid_mep_tlv_subtype = from->invalid_mep_tlv_subtype;
        to->invalid_tlv_offset = from->invalid_tlv_offset;
        sal_memcpy(&to->peer_da_mac_address,&from->peer_da_mac_address,sizeof(bcm_mac_t));
        to->num_tlvs = from->num_tlvs;
        to->invalid_tlv_count = from->invalid_tlv_count;
        to->ctf_id = from->ctf_id;
        to->gtf_id = from->gtf_id;
        for (i = 0; i < BCM_OAM_MAX_NUM_TLVS_FOR_LBM; i++) {
            sal_memcpy(&to->tlvs[i], &from->tlvs[i], sizeof(bcm_oam_tlv_t));
        }
        for (i = 0; i < BCM_OAM_LM_COUNTER_MAX; i++) {
            to->lb_tx_update_lm_counter_base_id[i] = from->lb_tx_update_lm_counter_base_id[i];
        }
        for (i = 0; i < BCM_OAM_LM_COUNTER_MAX; i++) {
            to->lb_tx_update_lm_counter_offset[i] = from->lb_tx_update_lm_counter_offset[i];
        }
        to->lb_tx_update_lm_counter_size = from->lb_tx_update_lm_counter_size;
        sal_memcpy(&to->int_pri,&from->int_pri,sizeof(bcm_cos_t));
        to->pkt_pri = from->pkt_pri;
        to->inner_pkt_pri = from->inner_pkt_pri;
    }
    return;
}

/*
 * Function:
 *      _bcm_compat6523out_oam_loopback_t
 *
 * Purpose:
 *      Convert the bcm_oam_loopback_t datatype from SDK 6.5.24+ to <=6.5.23.
 *
 * Parameters:
 *      from - (IN) SDK 6.5.24+ version of the datatype.
 *      to   - (OUT) <= SDK 6.5.23 version of the datatype.
 *
 * Returns:
 *      Nothing.
 */
STATIC void
_bcm_compat6523out_oam_loopback_t(bcm_oam_loopback_t *from,
                                    bcm_compat6523_oam_loopback_t *to)
{
    int i = 0;
    if (from && to) {
        to->flags = from->flags;
        to->id = from->id;
        to->remote_id = from->remote_id;
        to->period = from->period;
        to->ttl = from->ttl;
        sal_memcpy(&to->discovered_me,
                   &from->discovered_me,
                   sizeof(bcm_oam_loopback_discovery_record_t));
        to->rx_count = from->rx_count;
        to->tx_count = from->tx_count;
        to->drop_count = from->drop_count;
        to->unexpected_response = from->unexpected_response;
        to->out_of_sequence = from->out_of_sequence;
        to->local_mipid_missmatch = from->local_mipid_missmatch;
        to->remote_mipid_missmatch = from->remote_mipid_missmatch;
        to->invalid_target_mep_tlv = from->invalid_target_mep_tlv;
        to->invalid_mep_tlv_subtype = from->invalid_mep_tlv_subtype;
        to->invalid_tlv_offset = from->invalid_tlv_offset;
        sal_memcpy(&to->peer_da_mac_address,&from->peer_da_mac_address,sizeof(bcm_mac_t));
        to->num_tlvs = from->num_tlvs;
        to->invalid_tlv_count = from->invalid_tlv_count;
        to->ctf_id = from->ctf_id;
        to->gtf_id = from->gtf_id;
        for (i = 0; i < BCM_OAM_MAX_NUM_TLVS_FOR_LBM; i++) {
            sal_memcpy(&to->tlvs[i], &from->tlvs[i], sizeof(bcm_oam_tlv_t));
        }
        for (i = 0; i < BCM_OAM_LM_COUNTER_MAX; i++) {
            to->lb_tx_update_lm_counter_base_id[i] = from->lb_tx_update_lm_counter_base_id[i];
        }
        for (i = 0; i < BCM_OAM_LM_COUNTER_MAX; i++) {
            to->lb_tx_update_lm_counter_offset[i] = from->lb_tx_update_lm_counter_offset[i];
        }
        to->lb_tx_update_lm_counter_size = from->lb_tx_update_lm_counter_size;
        sal_memcpy(&to->int_pri,&from->int_pri,sizeof(bcm_cos_t));
        to->pkt_pri = from->pkt_pri;
        to->inner_pkt_pri = from->inner_pkt_pri;
    }
    return;
}

/*
 * Function:
 *   bcm_compat6523_oam_loopback_add
 * Purpose:
 *   Compatibility function for RPC call to bcm_oam_loopback_add.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   match          - (IN) OAM loopback structure.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6523_oam_loopback_add(
    int unit, 
    bcm_compat6523_oam_loopback_t *loopback_ptr)
{
    int rv = 0;
    bcm_oam_loopback_t *new_loopback_ptr = NULL;

    if (loopback_ptr != NULL) {
        new_loopback_ptr = (bcm_oam_loopback_t *)
            sal_alloc(sizeof(bcm_oam_loopback_t), "New loopback");
        if (new_loopback_ptr == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6523in_oam_loopback_t(loopback_ptr, new_loopback_ptr);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_oam_loopback_add(unit, new_loopback_ptr);

    if (rv >= 0) {
        _bcm_compat6523out_oam_loopback_t(new_loopback_ptr, loopback_ptr);
    }

    sal_free(new_loopback_ptr);
    return rv;
}
/*
 * Function:
 *   bcm_compat6523_oam_loopback_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_oam_loopback_get.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   match          - (IN) OAM loopback structure.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6523_oam_loopback_get(
    int unit, 
    bcm_compat6523_oam_loopback_t *loopback_ptr)
{
    int rv = 0;
    bcm_oam_loopback_t *new_loopback_ptr = NULL;

    if (loopback_ptr != NULL) {
        new_loopback_ptr = (bcm_oam_loopback_t *)
            sal_alloc(sizeof(bcm_oam_loopback_t), "New loopback");
        if (new_loopback_ptr == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6523in_oam_loopback_t(loopback_ptr, new_loopback_ptr);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_oam_loopback_get(unit, new_loopback_ptr);

    if (rv >= 0) {
        _bcm_compat6523out_oam_loopback_t(new_loopback_ptr, loopback_ptr);
    }

    sal_free(new_loopback_ptr);
    return rv;
}
/*
 * Function:
 *   bcm_compat6523_oam_loopback_delete
 * Purpose:
 *   Compatibility function for RPC call to bcm_oam_loopback_delete.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   match          - (IN) OAM loopback structure.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6523_oam_loopback_delete(
    int unit, 
    bcm_compat6523_oam_loopback_t *loopback_ptr)
{
    int rv = 0;
    bcm_oam_loopback_t *new_loopback_ptr = NULL;

    if (loopback_ptr != NULL) {
        new_loopback_ptr = (bcm_oam_loopback_t *)
            sal_alloc(sizeof(bcm_oam_loopback_t), "New loopback");
        if (new_loopback_ptr == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6523in_oam_loopback_t(loopback_ptr, new_loopback_ptr);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_oam_loopback_delete(unit, new_loopback_ptr);

    if (rv >= 0) {
        _bcm_compat6523out_oam_loopback_t(new_loopback_ptr, loopback_ptr);
    }

    sal_free(new_loopback_ptr);
    return rv;
}

/*
 * Function:
 *      _bcm_compat6523in_oam_tst_tx_t
 * Purpose:
 *      Convert the bcm_oam_tst_tx_t datatype from <=6.5.23 to
 *      SDK 6.5.24+
 * Parameters:
 *      from        - (IN) The <=6.5.23 version of the datatype
 *      to          - (OUT) The SDK 6.5.24+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523in_oam_tst_tx_t(
    bcm_compat6523_oam_tst_tx_t *from,
    bcm_oam_tst_tx_t *to)
{
    if (from && to) {
        bcm_oam_tst_tx_t_init(to);
        to->endpoint_id = from->endpoint_id;
        to->gtf_id = from->gtf_id;
        to->flags = from->flags;
        sal_memcpy(&to->peer_da_mac_address,&from->peer_da_mac_address,sizeof(bcm_mac_t));
        sal_memcpy(&to->tlv,&from->tlv,sizeof(bcm_oam_tlv_t));
    }
}

/*
 * Function:
 *      _bcm_compat6523out_oam_tst_tx_t
 * Purpose:
 *      Convert the bcm_oam_tst_tx_t datatype from SDK 6.5.24+ to
 *      <=6.5.23
 * Parameters:
 *      from        - (IN) The SDK 6.5.24+ version of the datatype
 *      to          - (OUT) The <=6.5.23 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523out_oam_tst_tx_t(
    bcm_oam_tst_tx_t *from,
    bcm_compat6523_oam_tst_tx_t *to)
{
    if (from && to) {
        to->endpoint_id = from->endpoint_id;
        to->gtf_id = from->gtf_id;
        to->flags = from->flags;
        sal_memcpy(&to->peer_da_mac_address,&from->peer_da_mac_address,sizeof(bcm_mac_t));
        sal_memcpy(&to->tlv,&from->tlv,sizeof(bcm_oam_tlv_t));
    }
}

/*
 * Function:
 *   bcm_compat6523_oam_tst_tx_add
 * Purpose:
 *   Compatibility function for RPC call to bcm_oam_tst_tx_add.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   match          - (IN) OAM TST tx structure.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6523_oam_tst_tx_add(
    int unit, 
    bcm_compat6523_oam_tst_tx_t *tst_ptr)
{
    int rv = 0;
    bcm_oam_tst_tx_t *new_tst_ptr = NULL;

    if (tst_ptr != NULL) {
        new_tst_ptr = (bcm_oam_tst_tx_t *)
            sal_alloc(sizeof(bcm_oam_tst_tx_t), "New tst");
        if (new_tst_ptr == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6523in_oam_tst_tx_t(tst_ptr, new_tst_ptr);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_oam_tst_tx_add(unit, new_tst_ptr);

    if (rv >= 0) {
        _bcm_compat6523out_oam_tst_tx_t(new_tst_ptr, tst_ptr);
    }

    sal_free(new_tst_ptr);
    return rv;
}
/*
 * Function:
 *   bcm_compat6523_oam_tst_tx_get
 * Purpose:
 *   Compatibility function for RPC call to bcm_oam_tst_tx_get.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   match          - (IN) OAM TST tx structure.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6523_oam_tst_tx_get(
    int unit, 
    bcm_compat6523_oam_tst_tx_t *tst_ptr)
{
    int rv = 0;
    bcm_oam_tst_tx_t *new_tst_ptr = NULL;

    if (tst_ptr != NULL) {
        new_tst_ptr = (bcm_oam_tst_tx_t *)
            sal_alloc(sizeof(bcm_oam_tst_tx_t), "New tst");
        if (new_tst_ptr == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6523in_oam_tst_tx_t(tst_ptr, new_tst_ptr);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_oam_tst_tx_get(unit, new_tst_ptr);

    if (rv >= 0) {
        _bcm_compat6523out_oam_tst_tx_t(new_tst_ptr, tst_ptr);
    }

    sal_free(new_tst_ptr);
    return rv;
}
/*
 * Function:
 *   bcm_compat6523_oam_tst_tx_delete
 * Purpose:
 *   Compatibility function for RPC call to bcm_oam_tst_tx_delete.
 * Parameters:
 *   unit           - (IN) BCM device number.
 *   match          - (IN) OAM TST tx structure.
 * Returns:
 *   BCM_E_XXX
 */
int bcm_compat6523_oam_tst_tx_delete(
    int unit, 
    bcm_compat6523_oam_tst_tx_t *tst_ptr)
{
    int rv = 0;
    bcm_oam_tst_tx_t *new_tst_ptr = NULL;

    if (tst_ptr != NULL) {
        new_tst_ptr = (bcm_oam_tst_tx_t *)
            sal_alloc(sizeof(bcm_oam_tst_tx_t), "New tst");
        if (new_tst_ptr == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6523in_oam_tst_tx_t(tst_ptr, new_tst_ptr);
    } else {
        return BCM_E_PARAM;
    }

    rv = bcm_oam_tst_tx_delete(unit, new_tst_ptr);

    if (rv >= 0) {
        _bcm_compat6523out_oam_tst_tx_t(new_tst_ptr, tst_ptr);
    }

    sal_free(new_tst_ptr);
    return rv;
}

/*
 * Function:
 *      _bcm_compat6523in_oam_group_info_t
 * Purpose:
 *      Convert the bcm_oam_group_info_t datatype from <=6.5.23 to
 *      SDK 6.5.24+
 * Parameters:
 *      from        - (IN) The <=6.5.23 version of the datatype
 *      to          - (OUT) The SDK 6.5.24+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523in_oam_group_info_t(
    bcm_compat6523_oam_group_info_t *from,
    bcm_oam_group_info_t *to)
{
    bcm_oam_group_info_t_init(to);
    to->flags = from->flags;
    to->id = from->id;
    sal_memcpy(&to->name, &from->name,
               BCM_OAM_GROUP_NAME_LENGTH);
    sal_memcpy(&to->rx_name, &from->rx_name,
               BCM_OAM_GROUP_NAME_LENGTH);
    to->faults = from->faults;
    to->persistent_faults = from->persistent_faults;
    to->clear_persistent_faults = from->clear_persistent_faults;
    to->lowest_alarm_priority = from->lowest_alarm_priority;
    to->group_name_index = from->group_name_index;
}

/*
 * Function:
 *      _bcm_compat6523out_oam_group_info_t
 * Purpose:
 *      Convert the bcm_oam_group_info_t datatype from SDK 6.5.24+ to
 *      <=6.5.23
 * Parameters:
 *      from        - (IN) The SDK 6.5.24+ version of the datatype
 *      to          - (OUT) The <=6.5.23 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523out_oam_group_info_t(
    bcm_oam_group_info_t *from,
    bcm_compat6523_oam_group_info_t *to)
{
    to->flags = from->flags;
    to->id = from->id;
    sal_memcpy(&to->name, &from->name,
               BCM_OAM_GROUP_NAME_LENGTH);
    sal_memcpy(&to->rx_name, &from->rx_name,
               BCM_OAM_GROUP_NAME_LENGTH);
    to->faults = from->faults;
    to->persistent_faults = from->persistent_faults;
    to->clear_persistent_faults = from->clear_persistent_faults;
    to->lowest_alarm_priority = from->lowest_alarm_priority;
    to->group_name_index = from->group_name_index;
}

/*
 * Function:
 *      bcm_compat6523_oam_group_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_oam_group_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      group_info - (INOUT) Pointer to an OAM Group structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_oam_group_create(
    int unit,
    bcm_compat6523_oam_group_info_t *group_info)
{
    int rv = BCM_E_NONE;
    bcm_oam_group_info_t *new_group_info = NULL;

    if (group_info != NULL) {
        new_group_info = (bcm_oam_group_info_t *)
        sal_alloc(sizeof(bcm_oam_group_info_t),
                  "New oam_group_info");
        if (new_group_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_oam_group_info_t(group_info, new_group_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_oam_group_create(unit, new_group_info);

    /* Deallocate memory*/
    sal_free(new_group_info);

  return rv;
}

/*
 * Function:
 *      bcm_compat6523_oam_group_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_oam_group_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      group - (IN) The ID of the group object to get
 *      group_info - (OUT) Pointer to OAM group structure to receive the data.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_oam_group_get(
    int unit,
    bcm_oam_group_t group,
    bcm_compat6523_oam_group_info_t *group_info)
{
    int rv = BCM_E_NONE;
    bcm_oam_group_info_t *new_group_info = NULL;

    if (group_info != NULL) {
        new_group_info = (bcm_oam_group_info_t *)
        sal_alloc(sizeof(bcm_oam_group_info_t),
                  "New oam_group_info");
        if (new_group_info == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_oam_group_get(unit, group, new_group_info);

    /* Transform the entry from the new format to old one */
    _bcm_compat6523out_oam_group_info_t(new_group_info, group_info);

    /* Deallocate memory*/
    sal_free(new_group_info);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6523in_oam_endpoint_info_t
 * Purpose:
 *      Convert the bcm_oam_endpoint_info_t datatype from <=6.5.23 to
 *      SDK 6.5.24+
 * Parameters:
 *      from        - (IN) The <=6.5.23 version of the datatype
 *      to          - (OUT) The SDK 6.5.24+ version of the datatype
 * Returns:
 *      Nothing
 */
  STATIC void
_bcm_compat6523in_oam_endpoint_info_t(
    bcm_compat6523_oam_endpoint_info_t *from,
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
    to->trunk_index= from->trunk_index;
    to->intf_id = from->intf_id;
    to->mpls_label = from->mpls_label;
    to->egress_label = from->egress_label;
    to->mpls_network_info = from->mpls_network_info;

    sal_memcpy(to->dst_mac_address, from->dst_mac_address, 6);
    sal_memcpy(to->src_mac_address, from->src_mac_address, 6);
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
        to->tlvs[i1] = from->tlvs[i1];
    }
    to->subport_tpid = from->subport_tpid;
    to->remote_gport = from->remote_gport;
    to->mpls_out_gport = from->mpls_out_gport;
    to->sampling_ratio = from->sampling_ratio;
    to->lm_payload_offset = from->lm_payload_offset;
    to->lm_cos_offset= from->lm_cos_offset;

    to->lm_ctr_type = from->lm_ctr_type;
    to->lm_ctr_sample_size = from->lm_ctr_sample_size;
    to->pri_map_id= from->pri_map_id;
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
    to->measurement_period = from->measurement_period;
    to->test_id = from->test_id;
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
 *      _bcm_compat6523out_oam_endpoint_info_t
 * Purpose:
 *      Convert the bcm_oam_endpoint_info_t datatype from SDK 6.5.24+ to
 *      <=6.5.23
 * Parameters:
 *      from        - (IN) The SDK 6.5.24+ version of the datatype
 *      to          - (OUT) The <=6.5.23 version of the datatype
 * Returns:
 *      Nothing
 */
  STATIC void
_bcm_compat6523out_oam_endpoint_info_t(
    bcm_oam_endpoint_info_t *from,
    bcm_compat6523_oam_endpoint_info_t *to)
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
    to->trunk_index= from->trunk_index;
    to->intf_id = from->intf_id;
    to->mpls_label = from->mpls_label;
    to->egress_label = from->egress_label;
    to->mpls_network_info = from->mpls_network_info;

    sal_memcpy(to->dst_mac_address, from->dst_mac_address, 6);
    sal_memcpy(to->src_mac_address, from->src_mac_address, 6);
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
        to->tlvs[i1] = from->tlvs[i1];
    }
    to->subport_tpid = from->subport_tpid;
    to->remote_gport = from->remote_gport;
    to->mpls_out_gport = from->mpls_out_gport;
    to->sampling_ratio = from->sampling_ratio;
    to->lm_payload_offset = from->lm_payload_offset;
    to->lm_cos_offset= from->lm_cos_offset;

    to->lm_ctr_type = from->lm_ctr_type;
    to->lm_ctr_sample_size = from->lm_ctr_sample_size;
    to->pri_map_id= from->pri_map_id;
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
    to->measurement_period = from->measurement_period;
    to->test_id = from->test_id;
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
 *      bcm_compat6523_oam_endpoint_create
 * Purpose:
 *      Compatibility function for RPC call to bcm_oam_endpoint_create.
 * Parameters:
 *      unit - (IN) Unit number.
 *      endpoint_info - (INOUT) Pointer to an OAM endpoint structure.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_oam_endpoint_create(
    int unit,
    bcm_compat6523_oam_endpoint_info_t *endpoint_info)
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
    _bcm_compat6523in_oam_endpoint_info_t(endpoint_info, new_endpoint_info);
  } else {
    return BCM_E_PARAM;
  }

  /* Call the BCM API with new format */
  rv = bcm_oam_endpoint_create(unit, new_endpoint_info);

  /* Deallocate memory*/
  sal_free(new_endpoint_info);

  return rv;
}

/*
 * Function:
 *      bcm_compat6523_oam_endpoint_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_oam_endpoint_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      endpoint - (IN) The ID of the endpoint object to get
 *      endpoint_info - (OUT) Pointer to an OAM endpoint structure to receive the data.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_oam_endpoint_get(
    int unit,
    bcm_oam_endpoint_t endpoint,
    bcm_compat6523_oam_endpoint_info_t *endpoint_info)
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
  _bcm_compat6523out_oam_endpoint_info_t(new_endpoint_info, endpoint_info);

  /* Deallocate memory*/
  sal_free(new_endpoint_info);

  return rv;
}

/*
 * Function:
 *      _bcm_compat6523in_vlan_match_info_t
 * Purpose:
 *      Convert the bcm_vlan_match_info_t datatype from <=6.5.23 to
 *      SDK 6.5.24+
 * Parameters:
 *      from        - (IN) The <=6.5.23 version of the datatype
 *      to          - (OUT) The SDK 6.5.24+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523in_vlan_match_info_t(
    bcm_compat6523_vlan_match_info_t *from,
    bcm_vlan_match_info_t *to)
{
    int i1 = 0;

    bcm_vlan_match_info_t_init(to);
    to->match = from->match;
    to->port = from->port;
    for (i1 = 0; i1 < 6; i1++) {
        to->src_mac[i1] = from->src_mac[i1];
    }
    to->port_class = from->port_class;
}

/*
 * Function:
 *      _bcm_compat6523out_vlan_match_info_t
 * Purpose:
 *      Convert the bcm_vlan_match_info_t datatype from SDK 6.5.24+ to
 *      <=6.5.23
 * Parameters:
 *      from        - (IN) The SDK 6.5.24+ version of the datatype
 *      to          - (OUT) The <=6.5.23 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523out_vlan_match_info_t(
    bcm_vlan_match_info_t *from,
    bcm_compat6523_vlan_match_info_t *to)
{
    int i1 = 0;

    to->match = from->match;
    to->port = from->port;
    for (i1 = 0; i1 < 6; i1++) {
        to->src_mac[i1] = from->src_mac[i1];
    }
    to->port_class = from->port_class;
}

/*
 * Function:
 *      bcm_compat6523_vlan_match_action_add
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
int bcm_compat6523_vlan_match_action_add(
    int unit,
    uint32 options,
    bcm_compat6523_vlan_match_info_t *match_info,
    bcm_vlan_action_set_t *action_set)
{
    int rv = BCM_E_NONE;
    bcm_vlan_match_info_t *new_match_info = NULL;

    if (match_info != NULL) {
        new_match_info = (bcm_vlan_match_info_t *)
                     sal_alloc(sizeof(bcm_vlan_match_info_t),
                     "New match_info");
        if (new_match_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_vlan_match_info_t(match_info, new_match_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_match_action_add(unit, options, new_match_info, action_set);


    /* Deallocate memory*/
    sal_free(new_match_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_vlan_match_action_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_match_action_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      match_info - (IN) Match criteria info
 *      action_set - (OUT) Action Set.
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_vlan_match_action_get(
    int unit,
    bcm_compat6523_vlan_match_info_t *match_info,
    bcm_vlan_action_set_t *action_set)
{
    int rv = BCM_E_NONE;
    bcm_vlan_match_info_t *new_match_info = NULL;

    if (match_info != NULL) {
        new_match_info = (bcm_vlan_match_info_t *)
                     sal_alloc(sizeof(bcm_vlan_match_info_t),
                     "New match_info");
        if (new_match_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_vlan_match_info_t(match_info, new_match_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_match_action_get(unit, new_match_info, action_set);


    /* Deallocate memory*/
    sal_free(new_match_info);

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_vlan_match_action_multi_get
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
int bcm_compat6523_vlan_match_action_multi_get(
    int unit,
    bcm_vlan_match_t match,
    int size,
    bcm_compat6523_vlan_match_info_t *match_info_array,
    bcm_vlan_action_set_t *action_set_array,
    int *count)
{
    int rv = BCM_E_NONE;
    bcm_vlan_match_info_t *new_match_info_array = NULL;
    int i = 0;

    if (match_info_array != NULL && size > 0) {
        new_match_info_array = (bcm_vlan_match_info_t *)
                     sal_alloc(size * sizeof(bcm_vlan_match_info_t),
                     "New match_info_array");
        if (new_match_info_array == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_match_action_multi_get(unit, match, size, new_match_info_array, action_set_array, count);

    for (i = 0; i < size; i++) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6523out_vlan_match_info_t(&new_match_info_array[i], &match_info_array[i]);
    }

    /* Deallocate memory*/
    sal_free(new_match_info_array);

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_vlan_match_action_delete
 * Purpose:
 *      Compatibility function for RPC call to bcm_vlan_match_action_delete.
 * Parameters:
 *      unit - (IN) Unit number.
 *      match_info - (IN) Match criteria info
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_vlan_match_action_delete(
    int unit,
    bcm_compat6523_vlan_match_info_t *match_info)
{
    int rv = BCM_E_NONE;
    bcm_vlan_match_info_t *new_match_info = NULL;

    if (match_info != NULL) {
        new_match_info = (bcm_vlan_match_info_t *)
                     sal_alloc(sizeof(bcm_vlan_match_info_t),
                     "New match_info");
        if (new_match_info == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_vlan_match_info_t(match_info, new_match_info);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vlan_match_action_delete(unit, new_match_info);


    /* Deallocate memory*/
    sal_free(new_match_info);

    return rv;
}


#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat6523in_l2gre_port_t
 * Purpose:
 *      Convert the bcm_l2gre_port_t datatype from <=6.5.23 to
 *      SDK 6.5.24+
 * Parameters:
 *      from        - (IN) The <=6.5.23 version of the datatype
 *      to          - (OUT) The SDK 6.5.24+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523in_l2gre_port_t(
    bcm_compat6523_l2gre_port_t *from,
    bcm_l2gre_port_t *to)
{
    bcm_l2gre_port_t_init(to);
    to->l2gre_port_id = from->l2gre_port_id;
    to->flags = from->flags;
    to->if_class = from->if_class;
    to->int_pri = from->int_pri;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->egress_service_tpid = from->egress_service_tpid;
    to->egress_service_vlan = from->egress_service_vlan;
    to->mtu = from->mtu;
    to->match_port = from->match_port;
    to->criteria = from->criteria;
    to->match_vlan = from->match_vlan;
    to->match_inner_vlan = from->match_inner_vlan;
    to->egress_tunnel_id = from->egress_tunnel_id;
    to->match_tunnel_id = from->match_tunnel_id;
    to->egress_if = from->egress_if;
    to->network_group_id = from->network_group_id;
}

/*
 * Function:
 *      _bcm_compat6523out_l2gre_port_t
 * Purpose:
 *      Convert the bcm_l2gre_port_t datatype from SDK 6.5.24+ to
 *      <=6.5.23
 * Parameters:
 *      from        - (IN) The SDK 6.5.24+ version of the datatype
 *      to          - (OUT) The <=6.5.23 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523out_l2gre_port_t(
    bcm_l2gre_port_t *from,
    bcm_compat6523_l2gre_port_t *to)
{
    to->l2gre_port_id = from->l2gre_port_id;
    to->flags = from->flags;
    to->if_class = from->if_class;
    to->int_pri = from->int_pri;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->egress_service_tpid = from->egress_service_tpid;
    to->egress_service_vlan = from->egress_service_vlan;
    to->mtu = from->mtu;
    to->match_port = from->match_port;
    to->criteria = from->criteria;
    to->match_vlan = from->match_vlan;
    to->match_inner_vlan = from->match_inner_vlan;
    to->egress_tunnel_id = from->egress_tunnel_id;
    to->match_tunnel_id = from->match_tunnel_id;
    to->egress_if = from->egress_if;
    to->network_group_id = from->network_group_id;
}

/*
 * Function:
 *      bcm_compat6523_l2gre_port_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2gre_port_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2vpn - (IN) 
 *      l2gre_port - (INOUT) (IN/OUT) L2GRE port information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_l2gre_port_add(
    int unit,
    bcm_vpn_t l2vpn,
    bcm_compat6523_l2gre_port_t *l2gre_port)
{
    int rv = BCM_E_NONE;
    bcm_l2gre_port_t *new_l2gre_port = NULL;

    if (l2gre_port != NULL) {
        new_l2gre_port = (bcm_l2gre_port_t *)
                     sal_alloc(sizeof(bcm_l2gre_port_t),
                     "New l2gre_port");
        if (new_l2gre_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_l2gre_port_t(l2gre_port, new_l2gre_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2gre_port_add(unit, l2vpn, new_l2gre_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6523out_l2gre_port_t(new_l2gre_port, l2gre_port);

    /* Deallocate memory*/
    sal_free(new_l2gre_port);

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_l2gre_port_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2gre_port_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2vpn - (IN) 
 *      l2gre_port - (INOUT) (IN/OUT) L2GRE port information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_l2gre_port_get(
    int unit,
    bcm_vpn_t l2vpn,
    bcm_compat6523_l2gre_port_t *l2gre_port)
{
    int rv = BCM_E_NONE;
    bcm_l2gre_port_t *new_l2gre_port = NULL;

    if (l2gre_port != NULL) {
        new_l2gre_port = (bcm_l2gre_port_t *)
                     sal_alloc(sizeof(bcm_l2gre_port_t),
                     "New l2gre_port");
        if (new_l2gre_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_l2gre_port_t(l2gre_port, new_l2gre_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2gre_port_get(unit, l2vpn, new_l2gre_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6523out_l2gre_port_t(new_l2gre_port, l2gre_port);

    /* Deallocate memory*/
    sal_free(new_l2gre_port);

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_l2gre_port_get_all
 * Purpose:
 *      Compatibility function for RPC call to bcm_l2gre_port_get_all.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2vpn - (IN) VPN Instance ID
 *      port_max - (IN) Maximum number of L2GRE ports in array
 *      port_array - (OUT) Array of L2GRE ports
 *      port_count - (OUT) Number of L2GRE ports returned in array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_l2gre_port_get_all(
    int unit,
    bcm_vpn_t l2vpn,
    int port_max,
    bcm_compat6523_l2gre_port_t *port_array,
    int *port_count)
{
    int rv = BCM_E_NONE;
    bcm_l2gre_port_t *new_port_array = NULL;
    int i = 0;

    if (port_array != NULL && port_max > 0) {
        new_port_array = (bcm_l2gre_port_t *)
                     sal_alloc(port_max * sizeof(bcm_l2gre_port_t),
                     "New port_array");
        if (new_port_array == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_l2gre_port_get_all(unit, l2vpn, port_max, new_port_array, port_count);

    for (i = 0; i < port_max; i++) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6523out_l2gre_port_t(&new_port_array[i], &port_array[i]);
    }

    /* Deallocate memory*/
    sal_free(new_port_array);

    return rv;
}

/*
 * Function:
 *      _bcm_compat6523in_vxlan_port_t
 * Purpose:
 *      Convert the bcm_vxlan_port_t datatype from <=6.5.23 to
 *      SDK 6.5.24+
 * Parameters:
 *      from        - (IN) The <=6.5.23 version of the datatype
 *      to          - (OUT) The SDK 6.5.24+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523in_vxlan_port_t(
    bcm_compat6523_vxlan_port_t *from,
    bcm_vxlan_port_t *to)
{
    bcm_vxlan_port_t_init(to);
    to->vxlan_port_id = from->vxlan_port_id;
    to->flags = from->flags;
    to->if_class = from->if_class;
    to->int_pri = from->int_pri;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->egress_service_tpid = from->egress_service_tpid;
    to->egress_service_vlan = from->egress_service_vlan;
    to->mtu = from->mtu;
    to->match_port = from->match_port;
    to->criteria = from->criteria;
    to->match_vlan = from->match_vlan;
    to->match_inner_vlan = from->match_inner_vlan;
    to->egress_tunnel_id = from->egress_tunnel_id;
    to->match_tunnel_id = from->match_tunnel_id;
    to->egress_if = from->egress_if;
    to->network_group_id = from->network_group_id;
    to->vnid = from->vnid;
    to->qos_map_id = from->qos_map_id;
    to->tunnel_pkt_pri = from->tunnel_pkt_pri;
    to->tunnel_pkt_cfi = from->tunnel_pkt_cfi;
    to->flags2 = from->flags2;
    to->default_vpn = from->default_vpn;
}

/*
 * Function:
 *      _bcm_compat6523out_vxlan_port_t
 * Purpose:
 *      Convert the bcm_vxlan_port_t datatype from SDK 6.5.24+ to
 *      <=6.5.23
 * Parameters:
 *      from        - (IN) The SDK 6.5.24+ version of the datatype
 *      to          - (OUT) The <=6.5.23 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat6523out_vxlan_port_t(
    bcm_vxlan_port_t *from,
    bcm_compat6523_vxlan_port_t *to)
{
    to->vxlan_port_id = from->vxlan_port_id;
    to->flags = from->flags;
    to->if_class = from->if_class;
    to->int_pri = from->int_pri;
    to->pkt_pri = from->pkt_pri;
    to->pkt_cfi = from->pkt_cfi;
    to->egress_service_tpid = from->egress_service_tpid;
    to->egress_service_vlan = from->egress_service_vlan;
    to->mtu = from->mtu;
    to->match_port = from->match_port;
    to->criteria = from->criteria;
    to->match_vlan = from->match_vlan;
    to->match_inner_vlan = from->match_inner_vlan;
    to->egress_tunnel_id = from->egress_tunnel_id;
    to->match_tunnel_id = from->match_tunnel_id;
    to->egress_if = from->egress_if;
    to->network_group_id = from->network_group_id;
    to->vnid = from->vnid;
    to->qos_map_id = from->qos_map_id;
    to->tunnel_pkt_pri = from->tunnel_pkt_pri;
    to->tunnel_pkt_cfi = from->tunnel_pkt_cfi;
    to->flags2 = from->flags2;
    to->default_vpn = from->default_vpn;
}

/*
 * Function:
 *      bcm_compat6523_vxlan_port_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_port_add.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2vpn - (IN) 
 *      vxlan_port - (INOUT) (IN/OUT) VXLAN port information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_vxlan_port_add(
    int unit,
    bcm_vpn_t l2vpn,
    bcm_compat6523_vxlan_port_t *vxlan_port)
{
    int rv = BCM_E_NONE;
    bcm_vxlan_port_t *new_vxlan_port = NULL;

    if (vxlan_port != NULL) {
        new_vxlan_port = (bcm_vxlan_port_t *)
                     sal_alloc(sizeof(bcm_vxlan_port_t),
                     "New vxlan_port");
        if (new_vxlan_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_vxlan_port_t(vxlan_port, new_vxlan_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_port_add(unit, l2vpn, new_vxlan_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6523out_vxlan_port_t(new_vxlan_port, vxlan_port);

    /* Deallocate memory*/
    sal_free(new_vxlan_port);

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_vxlan_port_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_port_get.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2vpn - (IN) 
 *      vxlan_port - (INOUT) (IN/OUT) VXLAN port information
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_vxlan_port_get(
    int unit,
    bcm_vpn_t l2vpn,
    bcm_compat6523_vxlan_port_t *vxlan_port)
{
    int rv = BCM_E_NONE;
    bcm_vxlan_port_t *new_vxlan_port = NULL;

    if (vxlan_port != NULL) {
        new_vxlan_port = (bcm_vxlan_port_t *)
                     sal_alloc(sizeof(bcm_vxlan_port_t),
                     "New vxlan_port");
        if (new_vxlan_port == NULL) {
            return BCM_E_MEMORY;
        }
        /* Transform the entry from the old format to new one */
        _bcm_compat6523in_vxlan_port_t(vxlan_port, new_vxlan_port);
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_port_get(unit, l2vpn, new_vxlan_port);

    /* Transform the entry from the new format to old one */
    _bcm_compat6523out_vxlan_port_t(new_vxlan_port, vxlan_port);

    /* Deallocate memory*/
    sal_free(new_vxlan_port);

    return rv;
}

/*
 * Function:
 *      bcm_compat6523_vxlan_port_get_all
 * Purpose:
 *      Compatibility function for RPC call to bcm_vxlan_port_get_all.
 * Parameters:
 *      unit - (IN) Unit number.
 *      l2vpn - (IN) 
 *      port_max - (IN) Maximum number of VXLAN ports in array
 *      port_array - (OUT) Array of VXLAN ports
 *      port_count - (OUT) Number of VXLAN ports returned in array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6523_vxlan_port_get_all(
    int unit,
    bcm_vpn_t l2vpn,
    int port_max,
    bcm_compat6523_vxlan_port_t *port_array,
    int *port_count)
{
    int rv = BCM_E_NONE;
    bcm_vxlan_port_t *new_port_array = NULL;
    int i = 0;

    if (port_array != NULL && port_max > 0) {
        new_port_array = (bcm_vxlan_port_t *)
                     sal_alloc(port_max * sizeof(bcm_vxlan_port_t),
                     "New port_array");
        if (new_port_array == NULL) {
            return BCM_E_MEMORY;
        }
    } else {
        return BCM_E_PARAM;
    }

    /* Call the BCM API with new format */
    rv = bcm_vxlan_port_get_all(unit, l2vpn, port_max, new_port_array, port_count);

    for (i = 0; i < port_max; i++) {
        /* Transform the entry from the new format to old one */
        _bcm_compat6523out_vxlan_port_t(&new_port_array[i], &port_array[i]);
    }

    /* Deallocate memory*/
    sal_free(new_port_array);

    return rv;
}


#endif /* defined(INCLUDE_L3) */
#endif /* BCM_RPC_SUPPORT */
