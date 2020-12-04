/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    multicast.c
 * Purpose: Manages multicast functions
 */

#include <soc/defs.h>
#include <sal/core/libc.h>

#if defined(BCM_XGS3_SWITCH_SUPPORT) && defined(INCLUDE_L3)

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>

#include <bcm/error.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/multicast.h>
#include <bcm_int/esw/stack.h>

#include <bcm_int/esw_dispatch.h>

/*
 * Function:
 *      bcm_xgs3_multicast_group_get
 * Purpose:
 *      Allocate a multicast group index
 * Parameters:
 *      unit       - (IN)   Device Number
 *      group      - (IN)  Group ID
 *      flags      - (OUT)   BCM_MULTICAST_*
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_xgs3_multicast_group_get(int unit, bcm_multicast_t group, uint32 *flags)
{
    int mc_index, is_set = FALSE;

    mc_index = _BCM_MULTICAST_ID_GET(group);
    if (_BCM_MULTICAST_IS_L2(group)) {
        BCM_IF_ERROR_RETURN
            (_bcm_xgs3_l2mc_index_is_set(unit, mc_index, &is_set));
        *flags = BCM_MULTICAST_TYPE_L2;
    } else {
        if (!_BCM_MULTICAST_IS_L3(group)) {
            return BCM_E_PARAM;
        }

        BCM_IF_ERROR_RETURN
            (bcm_xgs3_ipmc_id_is_set(unit, mc_index, &is_set));
        *flags = BCM_MULTICAST_TYPE_L3;
    }

    if (is_set) {
        *flags |= BCM_MULTICAST_WITH_ID;
    } else {
        *flags = 0;
        return BCM_E_NOT_FOUND;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcm_xgs3_multicast_group_traverse
 * Purpose:
 *      Iterate over the defined multicast groups of the type
 *      specified in 'flags'.  If all types are desired, use
 *      MULTICAST_TYPE_MASK.
 * Parameters:
 *      unit - (IN) Unit number.
 *      trav_fn - (IN) Callback function.
 *      flags - (IN) BCM_MULTICAST_*
 *      user_data - (IN) User data to be passed to callback function.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_xgs3_multicast_group_traverse(int unit,
                                  bcm_multicast_group_traverse_cb_t trav_fn, 
                                  uint32 flags, void *user_data)
{
    int l2mc_size, l3mc_size, mc_index, is_set;
    uint32 group_flags;
    bcm_multicast_t group;

    if (flags & BCM_MULTICAST_TYPE_L2) {
        BCM_IF_ERROR_RETURN(_bcm_xgs3_l2mc_size_get(unit, &l2mc_size));
        group_flags = BCM_MULTICAST_TYPE_L2 | BCM_MULTICAST_WITH_ID;

        for (mc_index = 0; mc_index < l2mc_size; mc_index++) {
            BCM_IF_ERROR_RETURN
                (_bcm_xgs3_l2mc_index_is_set(unit, mc_index, &is_set));
            if (is_set) {
                _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_L2,
                                         mc_index);
                BCM_IF_ERROR_RETURN
                    ((*trav_fn)(unit, group, group_flags, user_data));
            }
        }
    }

    if (flags & BCM_MULTICAST_TYPE_L3) {
        l3mc_size = soc_mem_index_count(unit, L3_IPMCm);
        group_flags = BCM_MULTICAST_TYPE_L3 | BCM_MULTICAST_WITH_ID;

        for (mc_index = 0; mc_index < l3mc_size; mc_index++) {
            BCM_IF_ERROR_RETURN
                (bcm_xgs3_ipmc_id_is_set(unit, mc_index, &is_set));
            if (is_set) {
                _BCM_MULTICAST_GROUP_SET(group, _BCM_MULTICAST_TYPE_L3,
                                         mc_index);
                BCM_IF_ERROR_RETURN
                    ((*trav_fn)(unit, group, group_flags, user_data));
            }
        }
    }

    return BCM_E_NONE;
}

#else  /* INCLUDE_L3 && BCM_XGS3_SWITCH_SUPPORT */
typedef int bcm_esw_xgs3_multicast_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_L3 && BCM_XGS3_SWITCH_SUPPORT */
