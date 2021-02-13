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
#endif /* BCM_RPC_SUPPORT */
