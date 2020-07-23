/*
 * $Id: compat_6515.h,v 2.0 2018/09/20
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * RPC Compatibility with sdk-6.5.15 routines
*/

#ifdef BCM_RPC_SUPPORT
#include <shared/alloc.h>
#include <sal/core/libc.h>
#include <bcm/error.h>
#include <bcm_int/compat/compat_6515.h>

#ifdef INCLUDE_L3
/*
 * Function:
 *      _bcm_compat6515in_ipmc_addr_t
 * Purpose:
 *      Convert the bcm_ipmc_addr_t datatype from <=6.5.15 to
 *      SDK 6.5.15+
 * Parameters:
 *      from        - (IN) The <=6.5.15 version of the datatype
 *      to          - (OUT) The SDK 6.5.15+ version of the datatype
 * Returns:
 *      Nothing
*/
STATIC void
_bcm_compat6515in_ipmc_addr_t(
    bcm_compat6515_ipmc_addr_t *from,
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
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    sal_memcpy(&to->s_ip6_mask, &from->s_ip6_mask, sizeof(to->s_ip6_mask));
    to->priority = from->priority;
}

/*
 * Function:
 *      _bcm_compat6515out_ipmc_addr_t
 * Purpose:
 *      Convert the bcm_ipmc_addr_t datatype from 6.5.15+ to
 *      <=6.5.15
 * Parameters:
 *      from        - (IN) The SDK 6.5.15+ version of the datatype
 *      to          - (OUT) The <=6.5.15 version of the datatype
 * Returns:
 *      Nothing
*/
STATIC void
_bcm_compat6515out_ipmc_addr_t(
    bcm_ipmc_addr_t *from,
    bcm_compat6515_ipmc_addr_t *to)
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
    to->stat_id = from->stat_id;
    to->stat_pp_profile = from->stat_pp_profile;
    sal_memcpy(&to->s_ip6_mask, &from->s_ip6_mask, sizeof(to->s_ip6_mask));
    to->priority = from->priority;
}

/*
 * Function:
 *      bcm_compat6515_ipmc_add
 * Purpose:
 *      Compatibility function for RPC call to bcm_ipmc_add.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      info - (IN) IPMC addr info.
 * Returns:
 *      BCM_E_XXX
*/
int bcm_compat6515_ipmc_add(
    int unit,
    bcm_compat6515_ipmc_addr_t *info)
{
    int rv = 0;
    bcm_ipmc_addr_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_ipmc_addr_t*)
            sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC addr info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6515in_ipmc_addr_t(info, new_info);
    }

    rv = bcm_ipmc_add(unit, new_info);

    if (new_info != NULL) {
        _bcm_compat6515out_ipmc_addr_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6515_ipmc_find
 * Purpose:
 *      Compatibility function for RPC call to bcm_ipmc_find
 * Parameters:
 *      unit - (IN) BCM device number.
 *      info - (IN) IPMC addr info.
 * Returns:
 *      BCM_E_XXX
*/
int bcm_compat6515_ipmc_find(
    int unit,
    bcm_compat6515_ipmc_addr_t *info)
{
    int rv = 0;
    bcm_ipmc_addr_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_ipmc_addr_t*)
            sal_alloc(sizeof(bcm_ipmc_addr_t), "IPMC addr info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6515in_ipmc_addr_t(info, new_info);
    }

    rv = bcm_ipmc_find(unit, new_info);

    if (new_info != NULL) {
        _bcm_compat6515out_ipmc_addr_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6515_ipmc_remove
 * Purpose:
 *      Compatibility function for RPC call to bcm_ipmc_remove.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      info - (IN) IPMC addr info.
 * Returns:
 *      BCM_E_XXX
*/
int bcm_compat6515_ipmc_remove(
    int unit,
    bcm_compat6515_ipmc_addr_t *info)
{
    int rv = 0;
    bcm_ipmc_addr_t *new_info = NULL;

    if (info != NULL) {
        new_info = (bcm_ipmc_addr_t*)
            sal_alloc(sizeof(bcm_ipmc_addr_t), "New IPMC addr info");
        if (new_info == NULL) {
            return BCM_E_MEMORY;
        }

        _bcm_compat6515in_ipmc_addr_t(info, new_info);
    }

    rv = bcm_ipmc_remove(unit, new_info);

    if (new_info != NULL) {
        _bcm_compat6515out_ipmc_addr_t(new_info, info);
        sal_free(new_info);
    }

    return rv;
}

#endif

/*
 * Function:
 *      _bcm_compat6515in_bcm_port_resource_t
 * Purpose:
 *      Convert the bcm_mpls_tunnel_switch_t datatype from <=6.5.15 to
 *      SDK 6.5.15+
 * Parameters:
 *      from        - (IN) The <=6.5.15 version of the datatype
 *      to          - (OUT) The SDK 6.5.15+ version of the datatype
 * Returns:
 *      Nothing
*/
STATIC void
_bcm_compat6515in_port_resource_t(
    bcm_compat6515_port_resource_t *from,
    bcm_port_resource_t *to)
{
    bcm_port_resource_t_init(to);
    to->flags = from->flags;
    to->port = from->port;
    to->physical_port = from->physical_port;
    to->speed = from->speed;
    to->lanes = from->lanes;
    to->encap = from->encap;
    to->fec_type = from->fec_type;
    to->phy_lane_config = from->phy_lane_config;
    to->link_training = from->link_training;
}

/*
 * Function:
 *      bcm_compat6515_port_resource_default_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_match_set.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - (IN) Port or gport
 *      flags - (IN)Flags
 *      match_array - (IN) Match array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6515_port_resource_default_get(
    int unit,
    bcm_gport_t port,
    int flags,
    bcm_compat6515_port_resource_t *match_array)
{
    int rv = 0, i = 0;
    bcm_port_resource_t *new_match_array = NULL;

    if (match_array != NULL) {
        new_match_array = (bcm_port_resource_t *)
            sal_alloc(sizeof(bcm_port_resource_t),
                    "New port match info array");
        if (new_match_array == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6515in_port_resource_t(&match_array[i],
                    &new_match_array[i]);
    }

    rv = bcm_port_resource_default_get(unit, port, flags, new_match_array);

    if (new_match_array != NULL) {
        sal_free(new_match_array);
    }

    return rv;
}


/*
 * Function:
 *      bcm_compat6515_port_resource_get
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_resource_get.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - (IN) Port or gport
 *      match_array - (IN) Match array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6515_port_resource_get(
    int unit,
    bcm_gport_t port,
    bcm_compat6515_port_resource_t *match_array)
{
    int rv = 0, i = 0;
    bcm_port_resource_t *new_match_array = NULL;

    if (match_array != NULL) {
        new_match_array = (bcm_port_resource_t *)
            sal_alloc(sizeof(bcm_port_resource_t),
                    "New port match info array");
        if (new_match_array == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6515in_port_resource_t(&match_array[i],
                    &new_match_array[i]);
    }

    rv = bcm_port_resource_get(unit, port, new_match_array);

    if (new_match_array != NULL) {
        sal_free(new_match_array);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6515_port_resource_set
 * Purpose:
 *      Compatibility function for RPC call to bcm_port_resource_set.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - (IN) Port or gport
 *      match_array - (IN) Match array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6515_port_resource_set(
    int unit,
    bcm_gport_t port,
    bcm_compat6515_port_resource_t *match_array)
{
    int rv = 0, i = 0;
    bcm_port_resource_t *new_match_array = NULL;

    if (match_array != NULL) {
        new_match_array = (bcm_port_resource_t *)
            sal_alloc(sizeof(bcm_port_resource_t),
                    "New port match info array");
        if (new_match_array == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6515in_port_resource_t(&match_array[i],
                    &new_match_array[i]);
    }

    rv = bcm_port_resource_set(unit, port, new_match_array);

    if (new_match_array != NULL) {
        sal_free(new_match_array);
    }

    return rv;
}


/*
 * Function:
 *      bcm_compat6515_port_resource_speed_set_
 * Purpose:
 *      Compatibility function for RPC call to port_resource_speed_set.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - (IN) Port or gport
 *      match_array - (IN) Match array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6515_port_resource_speed_set(
    int unit,
    bcm_gport_t port,
    bcm_compat6515_port_resource_t *match_array)
{
    int rv = 0, i = 0;
    bcm_port_resource_t *new_match_array = NULL;

    if (match_array != NULL) {
        new_match_array = (bcm_port_resource_t *)
            sal_alloc(sizeof(bcm_port_resource_t),
                    "New port match info array");
        if (new_match_array == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6515in_port_resource_t(&match_array[i],
                    &new_match_array[i]);
    }

    rv = bcm_port_resource_speed_set(unit, port, new_match_array);

    if (new_match_array != NULL) {
        sal_free(new_match_array);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6515_port_resource_speed_get_
 * Purpose:
 *      Compatibility function for RPC call to port_resource_speed_get.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - (IN) Port or gport
 *      match_array - (IN) Match array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6515_port_resource_speed_get(
    int unit,
    bcm_gport_t port,
    bcm_compat6515_port_resource_t *match_array)
{
    int rv = 0, i = 0;
    bcm_port_resource_t *new_match_array = NULL;

    if (match_array != NULL) {
        new_match_array = (bcm_port_resource_t *)
            sal_alloc(sizeof(bcm_port_resource_t),
                    "New port match info array");
        if (new_match_array == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6515in_port_resource_t(&match_array[i],
                    &new_match_array[i]);
    }

    rv = bcm_port_resource_speed_get(unit, port, new_match_array);

    if (new_match_array != NULL) {
        sal_free(new_match_array);
    }

    return rv;
}

/*
 * Function:
 *      bcm_compat6515_port_resource_speed_config_validate
 * Purpose:
 *      Compatibility function for RPC call to port_resource_speed_config_validate.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      match_array - (IN) Match array
 *      pbmp - (IN) port pbmp 
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6515_port_resource_speed_config_validate(
    int unit,
    bcm_compat6515_port_resource_t *match_array,
    bcm_pbmp_t *pbmp)
{
    int rv = 0, i = 0;
    bcm_port_resource_t *new_match_array = NULL;

    if (match_array != NULL) {
        new_match_array = (bcm_port_resource_t *)
            sal_alloc(sizeof(bcm_port_resource_t),
                    "New port match info array");
        if (new_match_array == NULL) {
            return BCM_E_MEMORY;
        }
        _bcm_compat6515in_port_resource_t(&match_array[i],
                    &new_match_array[i]);
    }

    rv = bcm_port_resource_speed_config_validate(unit,new_match_array,pbmp);

    if (new_match_array != NULL) {
        sal_free(new_match_array);
    }

    return rv;
}


/*
 * Function:
 *      bcm_compat6515_port_resource_multi_set
 * Purpose:
 *      Compatibility function for RPC call to port_resource_speed_get.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - (IN) Port or gport
 *      match_array - (IN) Match array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6515_port_resource_multi_set(
    int unit,
    int nport,
    bcm_compat6515_port_resource_t *match_array)
{
    int rv = 0, i = 0;
    bcm_port_resource_t *new_match_array = NULL;

    if (match_array != NULL) {
        new_match_array = (bcm_port_resource_t *)
            sal_alloc(nport * sizeof(bcm_port_resource_t),
                    "New port match info array");
        if (new_match_array == NULL) {
            return BCM_E_MEMORY;
        }
        for (i = 0 ; i < nport; i++) { 
            _bcm_compat6515in_port_resource_t(&match_array[i],
                    &new_match_array[i]);
        }
    }

    rv = bcm_port_resource_multi_set(unit, nport, new_match_array);

    if (new_match_array != NULL) {
        sal_free(new_match_array);
    }

    return rv;
}


/*
 * Function:
 *      bcm_compat6515_port_resource_speed_multi_set
 * Purpose:
 *      Compatibility function for RPC call to port_resource_speed_multi_set.
 * Parameters:
 *      unit - (IN) BCM device number.
 *      port - (IN) Port or gport
 *      match_array - (IN) Match array
 * Returns:
 *      BCM_E_XXX
 */
int bcm_compat6515_port_resource_speed_multi_set(
    int unit,
    int nport,
    bcm_compat6515_port_resource_t *match_array)
{
    int rv = 0, i = 0;
    bcm_port_resource_t *new_match_array = NULL;

    if (match_array != NULL) {
        new_match_array = (bcm_port_resource_t *)
            sal_alloc(nport * sizeof(bcm_port_resource_t),
                    "New port match info array");
        if (new_match_array == NULL) {
            return BCM_E_MEMORY;
        }
        for (i = 0 ; i < nport; i++) { 
            _bcm_compat6515in_port_resource_t(&match_array[i],
                    &new_match_array[i]);
        }
    }

    rv = bcm_port_resource_speed_multi_set(unit, nport, new_match_array);

    if (new_match_array != NULL) {
        sal_free(new_match_array);
    }

    return rv;
}

#endif /* BCM_RPC_SUPPORT*/
