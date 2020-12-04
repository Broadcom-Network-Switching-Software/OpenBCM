/*
 * $Id: compat_658.c,v 2.0 2017/03/23
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with <=sdk-6.5.8 routines
 */

#ifdef	BCM_RPC_SUPPORT
#include <shared/alloc.h>
#include <bcm/error.h>
#include <bcm_int/compat/compat_658.h>

#if defined(INCLUDE_L3)
/*
 * Function:
 *      _bcm_compat658in_ecn_map_t
 * Purpose:
 *      Convert the bcm_ecn_map_t datatype from <=6.5.8 to
 *      SDK 6.5.8+
 * Parameters:
 *      from        - (IN) The <=6.5.8 version of the datatype
 *      to          - (OUT) The SDK 6.5.8+ version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat658in_ecn_map_t(bcm_compat658_ecn_map_t *from,
                           bcm_ecn_map_t *to)
{
    bcm_ecn_map_t_init(to);
    to->action_flags = from->action_flags;
    to->int_cn = from->int_cn;
    to->inner_ecn = from->inner_ecn;
    to->ecn = from->ecn;
    to->exp = from->exp;
    to->new_ecn = from->new_ecn;
    to->new_exp = from->new_exp;
}

/*
 * Function:
 *      _bcm_compat658out_ecn_map_t
 * Purpose:
 *      Convert the bcm_field_qset_t datatype from 6.5.8+ to
 *      <=6.5.8
 * Parameters:
 *      from        - (IN) The SDK 6.5.8+ version of the datatype
 *      to          - (OUT) The <=6.5.8 version of the datatype
 * Returns:
 *      Nothing
 */
STATIC void
_bcm_compat658out_ecn_map_t(bcm_ecn_map_t *from,
                            bcm_compat658_ecn_map_t *to)
{
    to->action_flags = from->action_flags;
    to->int_cn = from->int_cn;
    to->inner_ecn = from->inner_ecn;
    to->ecn = from->ecn;
    to->exp = from->exp;
    to->new_ecn = from->new_ecn;
    to->new_exp = from->new_exp;
}

/*
 * Function: bcm_compat658_ecn_map_get
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_esw_extender_forward_get.
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     extender_forward_entry - (IN/OUT) Forwarding entry with old format
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat658_ecn_map_get(int unit, int ecn_map_id,
                                   bcm_compat658_ecn_map_t *ecn_map)
{
    int rv;
    bcm_ecn_map_t *new_ecn_map = NULL;

    if (NULL != ecn_map) {
        /* Create from heap to avoid the stack to bloat */
        new_ecn_map = (bcm_ecn_map_t *)
            sal_alloc(sizeof(bcm_ecn_map_t), "New Ecn Map");
        if (NULL == new_ecn_map) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat658in_ecn_map_t(ecn_map, new_ecn_map);
    }

    /* Call the BCM API with new format */
    rv = bcm_ecn_map_get(unit, ecn_map_id, new_ecn_map);

    if (NULL != new_ecn_map) {
        /* Transform the entry from the new format to old one */
        _bcm_compat658out_ecn_map_t(new_ecn_map, ecn_map);
        /* Deallocate memory*/
        sal_free(new_ecn_map);
    }

    return rv;
}

/*
 * Function: bcm_compat658_ecn_map_set
 *
 * Purpose:
 *      Compatibility function for RPC call to bcm_esw_extender_forward_get.
 *
 * Parameters:
 *     unit - (IN) BCM device number.
 *     extender_forward_entry - (IN/OUT) Forwarding entry with old format
 *
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_compat658_ecn_map_set(int unit, uint32 options, int ecn_map_id,
                                   bcm_compat658_ecn_map_t *ecn_map)
{
    int rv;
    bcm_ecn_map_t *new_ecn_map = NULL;

    if (NULL != ecn_map) {
        /* Create from heap to avoid the stack to bloat */
        new_ecn_map = (bcm_ecn_map_t *)
            sal_alloc(sizeof(bcm_ecn_map_t), "New Ecn Map");
        if (NULL == new_ecn_map) {
            return BCM_E_MEMORY;
        }

        /* Transform the entry from the old format to new one */
        _bcm_compat658in_ecn_map_t(ecn_map, new_ecn_map);
    }

    /* Call the BCM API with new format */
    rv = bcm_ecn_map_set(unit, options, ecn_map_id, new_ecn_map);

    if (NULL != new_ecn_map) {
        /* Transform the entry from the new format to old one */
        _bcm_compat658out_ecn_map_t(new_ecn_map, ecn_map);
        /* Deallocate memory*/
        sal_free(new_ecn_map);
    }

    return rv;
}
#endif
#endif  /* BCM_RPC_SUPPORT */
