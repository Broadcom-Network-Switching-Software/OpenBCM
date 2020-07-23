/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:    tunnel.c
 * Purpose: Manages tunnel configuration
 */

#include <soc/defs.h>

#ifdef INCLUDE_L3

#include <sal/core/libc.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>

#include <bcm/l3.h>
#include <bcm/tunnel.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw_dispatch.h>
#ifdef  BCM_XGS_SWITCH_SUPPORT
#include <bcm_int/esw/firebolt.h>

#if defined(BCM_TRX_SUPPORT)
#include <bcm_int/esw/trx.h>
#endif /* BCM_TRX_SUPPORT */

#if defined(BCM_TRIUMPH2_SUPPORT)
#include <bcm_int/esw/triumph2.h>
#endif /* BCM_TRIUMPH2_SUPPORT */

#if defined(BCM_TRIUMPH3_SUPPORT)
#include <bcm_int/esw/triumph3.h>
#endif /* BCM_TRIUMPH2_SUPPORT */


#endif  /* BCM_XGS_SWITCH_SUPPORT */


/* 
 * Function:
 *      bcm_esw_tunnel_terminator_add
 * Purpose: 
 *      Add a tunnel terminator for DIP-SIP
 * Parameters: 
 *      unit - (IN)SOC unit number.
 *      info - (IN)Tunnel information.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      The following restrictions are used:
 *       - DIP must not be 0; DIP mask must be 0xFFFFFFFF
 *       - SIP mask must be either 0xFFFFFFFF or 0.  If it is
 *         0xFFFFFFFF, then SIP can not be 0.
 */
int
bcm_esw_tunnel_terminator_add(int unit, bcm_tunnel_terminator_t *info)
{
    /* Input parameters check. */
    if (NULL == info) {
        return (BCM_E_PARAM);
    }

    /* Vrf is in device supported range check. */
    if ((info->vrf > SOC_VRF_MAX(unit)) || 
        (info->vrf < BCM_L3_VRF_DEFAULT)) {
        return (BCM_E_PARAM);
    }
  
    /*  Check that device supports ipv6. */
    if (BCM_L3_NO_IP6_SUPPORT(unit, info->flags)) {
        return (BCM_E_UNAVAIL);
    }
	
    if (soc_feature(unit, soc_feature_no_tunnel)) {
        return (BCM_E_UNAVAIL);
    }

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_HAWKEYE(unit) &&
        soc_feature(unit, soc_feature_l3)) {
        return bcm_xgs3_tunnel_terminator_add(unit, info);
    }
#endif  /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}

/* 
 * Function:
 *      bcm_esw_tunnel_terminator_delete
 * Purpose: 
 *      Delete a tunnel terminator.
 * Parameters: 
 *      unit - (IN)SOC unit number.
 *      info - (IN)Tunnel deletion lookup key.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_tunnel_terminator_delete(int unit, bcm_tunnel_terminator_t *info)
{
    /* Input parameters check. */
    if (NULL == info) {
        return (BCM_E_PARAM);
    }

    /* Vrf is in device supported range check. */
    if ((info->vrf > SOC_VRF_MAX(unit)) || 
        (info->vrf < BCM_L3_VRF_DEFAULT)) {
        return (BCM_E_PARAM);
    }
  
    /*  Check that device supports ipv6. */
    if (BCM_L3_NO_IP6_SUPPORT(unit, info->flags)) {
        return (BCM_E_UNAVAIL);
    }

    if (soc_feature(unit, soc_feature_no_tunnel)) {
        return (BCM_E_UNAVAIL);
    }

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_HAWKEYE(unit) &&
        soc_feature(unit, soc_feature_l3)) {
        return bcm_xgs3_tunnel_terminator_delete(unit, info);
    }
#endif  /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}

/* 
 * Function:
 *      bcm_esw_tunnel_terminator_update
 * Purpose: 
 *      Update a tunnel terminator for DIP-SIP
 * Parameters: 
 *      unit - (IN)SOC unit number.
 *      info - (IN)Tunnel lookup key & updated tunnel information.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_tunnel_terminator_update(int unit, bcm_tunnel_terminator_t *info)
{
    /* Input parameters check. */
    if (NULL == info) {
        return (BCM_E_PARAM);
    }
	
    /* Vrf is in device supported range check. */
    if ((info->vrf > SOC_VRF_MAX(unit)) || 
        (info->vrf < BCM_L3_VRF_DEFAULT)) {
        return (BCM_E_PARAM);
    }
  
    /*  Check that device supports ipv6. */
    if (BCM_L3_NO_IP6_SUPPORT(unit, info->flags)) {
        return (BCM_E_UNAVAIL);
    }

    if (soc_feature(unit, soc_feature_no_tunnel)) {
        return (BCM_E_UNAVAIL);
    }

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_HAWKEYE(unit) &&
        soc_feature(unit, soc_feature_l3)) {
        return bcm_xgs3_tunnel_terminator_update(unit, info);
    }
#endif  /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}

/* 
 * Function:
 *      bcm_esw_tunnel_terminator_get
 * Purpose: 
 *      Get a tunnel terminator info.  
 * Parameters: 
 *      unit - (IN)SOC unit number.
 *      info - (IN/OUT)Lookup key & extracted tunnel information.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_tunnel_terminator_get(int unit, bcm_tunnel_terminator_t *info)
{
    /* Input parameters check. */
    if (NULL == info) {
        return (BCM_E_PARAM);
    }

    /* Vrf is in device supported range check. */
    if ((info->vrf > SOC_VRF_MAX(unit)) || 
        (info->vrf < BCM_L3_VRF_DEFAULT)) {
        return (BCM_E_PARAM);
    }
  
    /*  Check that device supports ipv6. */
    if (BCM_L3_NO_IP6_SUPPORT(unit, info->flags)) {
        return (BCM_E_UNAVAIL);
    }

    if (soc_feature(unit, soc_feature_no_tunnel)) {
        return (BCM_E_UNAVAIL);
    }

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_HAWKEYE(unit) &&
        soc_feature(unit, soc_feature_l3)) {
        return bcm_xgs3_tunnel_terminator_get(unit, info);
    }
#endif  /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_tunnel_terminator_traverse
 * Purpose:
 *      Traverse all tunnel terminator entries
 * Parameters:
 *      unit - (IN)SOC unit number. 
 *      cb - (IN)User callback function, called once per entry
 *      user_data - (IN)User supplied cookie used in parameter in callback function
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_tunnel_terminator_traverse(int unit, 
                             bcm_tunnel_terminator_traverse_cb cb,
                             void *user_data)
{
    if (soc_feature(unit, soc_feature_no_tunnel)) {
        return (BCM_E_UNAVAIL);
    }

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_HAWKEYE(unit) &&
        soc_feature(unit, soc_feature_l3)) {
        return bcm_xgs3_tunnel_terminator_traverse(unit, cb, user_data);
    }
#endif  /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_tunnel_initiator_set
 * Purpose:
 *      Set the tunnel property for the given L3 interface
 * Parameters:
 *      unit  - (IN)SOC unit number.
 *      intf  - (IN)L3 interface info.(ONLY ifindex used to identify interface).
 *      tunnel -(IN)The tunnel header information.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_tunnel_initiator_set(int unit, bcm_l3_intf_t *intf,
                             bcm_tunnel_initiator_t *tunnel)
{
    /* Input parameters check. */
    if (NULL == tunnel) {
        return (BCM_E_PARAM);
    }

    /*  Check that device supports ipv6. */
    if (BCM_L3_NO_IP6_SUPPORT(unit, tunnel->flags)) {
        return (BCM_E_UNAVAIL);
    }

    if (soc_feature(unit, soc_feature_no_tunnel)) {
        return (BCM_E_UNAVAIL);
    }

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_HAWKEYE(unit) &&
        soc_feature(unit, soc_feature_l3)) {
        return (bcm_xgs3_tunnel_initiator_set(unit, intf, tunnel));
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_tunnel_initiator_clear
 * Purpose:
 *      Delete the tunnel association for the given L3 interface
 * Parameters:
 *      unit - (IN)SOC unit number.
 *      intf - (IN)L3 interface info.(ONLY ifindex used to identify interface).
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_tunnel_initiator_clear(int unit, bcm_l3_intf_t *intf)
{
    if (soc_feature(unit, soc_feature_no_tunnel)) {
        return (BCM_E_UNAVAIL);
    }

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_HAWKEYE(unit) &&
        soc_feature(unit, soc_feature_l3)) {
        return bcm_xgs3_tunnel_initiator_clear(unit, intf);
    }
#endif  /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_tunnel_initiator_get
 * Purpose:
 *      Get the tunnel properties for the given L3 interface
 * Parameters:
 *      unit   - (IN)SOC unit number. 
 *      intf   - (IN)L3 interface info (ONLY ifindex used to find interface).
 *      tunnel - (IN/OUT) The tunnel header information
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_tunnel_initiator_get(int unit, bcm_l3_intf_t *l3_intf,
                             bcm_tunnel_initiator_t *tunnel)
{
    if (soc_feature(unit, soc_feature_no_tunnel)) {
        return (BCM_E_UNAVAIL);
    }

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_HAWKEYE(unit) &&
        soc_feature(unit, soc_feature_l3)) {
        return bcm_xgs3_tunnel_initiator_get(unit, l3_intf, tunnel);
    }
#endif  /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_tunnel_initiator_traverse
 * Purpose:
 *      Traverse all tunnel initiator entries
 * Parameters:
 *      unit - (IN)SOC unit number. 
 *      cb - (IN)User callback function, called once per entry
 *      user_data - (IN)User supplied cookie used in parameter in callback function
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_tunnel_initiator_traverse(int unit, 
                             bcm_tunnel_initiator_traverse_cb cb,
                             void *user_data)
{
    if (soc_feature(unit, soc_feature_no_tunnel)) {
        return (BCM_E_UNAVAIL);
    }

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_HAWKEYE(unit) &&
        soc_feature(unit, soc_feature_l3)) {
        return bcm_xgs3_tunnel_initiator_traverse(unit, cb, user_data);
    }
#endif  /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_esw_tunnel_config_set
 * Purpose:
 *      Set the global tunnel property
 * Parameters:
 *      unit    - (IN)SOC unit number 
 *      tconfig - (IN)Global information about the L3 tunneling config.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_tunnel_config_set(int unit, bcm_tunnel_config_t *tconfig)
{
    if (soc_feature(unit, soc_feature_no_tunnel)) {
        return (BCM_E_UNAVAIL);
    }

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_HAWKEYE(unit) &&
        soc_feature(unit, soc_feature_l3)) {
        return bcm_xgs3_tunnel_config_set(unit, tconfig);
    }
#endif  /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function: 
 *      bcm_tunnel_dscp_map_create
 * Purpose:
 *      Create a tunnel DSCP map instance.
 * Parameters:
 *      unit         - (IN)  SOC unit #
 *      flags        - (IN)  BCM_L3_XXX
 *      dscp_map_id  - (OUT) Allocated DSCP map ID
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_tunnel_dscp_map_create(int unit, uint32 flags, int *dscp_map_id)
{
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_HAWKEYE(unit) &&
        soc_feature(unit, soc_feature_l3)) {
        return (bcm_xgs3_tunnel_dscp_map_create(unit, flags, dscp_map_id));
    }
#endif
    return BCM_E_UNAVAIL;
}
    
/*
 * Function:
 *      bcm_tunnel_dscp_map_destroy
 * Purpose:
 *      Destroy an existing tunnel DSCP map instance.
 * Parameters:
 *      unit        - (IN) SOC unit #
 *      dscp_map_id - (IN) DSCP map ID
 * Returns:
 *      BCM_E_XXX
 */ 
int
bcm_esw_tunnel_dscp_map_destroy(int unit, int dscp_map_id)
{   
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_HAWKEYE(unit) &&
        soc_feature(unit, soc_feature_l3)) {
        return (bcm_xgs3_tunnel_dscp_map_destroy(unit, dscp_map_id));
    }
#endif
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_tunnel_dscp_map_set
 * Purpose:
 *      Set the mapping of { internal priority, color }
 *      to a DSCP value for outer tunnel headers
 *      in the specified DSCP map instance.
 * Parameters:
 *      unit         - (IN) SOC unit #
 *      dscp_map_id  - (IN) DSCP map ID
 *      priority - (IN) Internal priority
 *      color    - (IN) bcmColor*
 *      dscp     - (IN) DSCP value
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_tunnel_dscp_map_set(int unit, int dscp_map_id, 
                            bcm_tunnel_dscp_map_t *dscp_map)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRX_SUPPORT)
    bcm_port_t port;
#endif /* BCM_TRX_SUPPORT */


    if ((0 == SOC_IS_XGS3_SWITCH(unit)) ||  
        (1 == SOC_IS_HAWKEYE(unit)) || 
        (0 == soc_feature(unit, soc_feature_l3))) {
        return (rv);
    }

#if defined(BCM_XGS3_SWITCH_SUPPORT)

    if ((dscp_map == NULL) ||
        (dscp_map_id < 0) ||
        (dscp_map_id >= BCM_XGS3_L3_TUNNEL_DSCP_MAP_TBL_SIZE(unit)) ||
        (dscp_map->priority < BCM_PRIO_MIN) ||
        (dscp_map->priority > BCM_PRIO_MAX) ||
        (dscp_map->dscp < 0) ||
        (dscp_map->dscp > 63)) {
        return (BCM_E_PARAM);
    }

    if (!BCM_L3_DSCP_MAP_USED_GET(unit, dscp_map_id)) {
        return (BCM_E_NOT_FOUND);
    }

#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit)) {

        /* Set dscp map for each port. */
        PBMP_PORT_ITER(unit, port) {
            rv = bcm_esw_tunnel_dscp_map_port_set(unit, port, dscp_map);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    } else 
#endif /* BCM_TRX_SUPPORT */
    { 
        rv = bcm_xgs3_tunnel_dscp_map_set(unit, dscp_map_id, dscp_map);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return (rv);
}
    
/*
 * Function:
 *      bcm_tunnel_dscp_map_get
 * Purpose:
 *      Set the mapping of { internal priority, color }
 *      to a DSCP value for outer tunnel headers
 *      in the specified DSCP map instance.
 * Parameters:
 *      unit         - (IN)  SOC unit #
 *      dscp_map_id  - (IN)  DSCP map ID
 *      priority - (IN)  Internal priority
 *      color    - (IN)  bcmColor*
 *      dscp     - (OUT) DSCP value
 * Returns:
 *      BCM_E_XXX
 */     
int     
bcm_esw_tunnel_dscp_map_get(int unit, int dscp_map_id, 
                            bcm_tunnel_dscp_map_t *dscp_map)
{
    int rv = BCM_E_UNAVAIL;
#if defined(BCM_TRX_SUPPORT)
    bcm_port_t port;
#endif /* BCM_TRX_SUPPORT */

    if ((0 == SOC_IS_XGS3_SWITCH(unit)) ||  
        (1 == SOC_IS_HAWKEYE(unit)) || 
        (0 == soc_feature(unit, soc_feature_l3))) {
        return (rv);
    }

#if defined(BCM_XGS3_SWITCH_SUPPORT)

    if ((dscp_map == NULL) ||
        (dscp_map_id < 0) ||
        (dscp_map_id >= BCM_XGS3_L3_TUNNEL_DSCP_MAP_TBL_SIZE(unit)) ||
        (dscp_map->priority < BCM_PRIO_MIN) ||
        (dscp_map->priority > BCM_PRIO_MAX) ||
        (dscp_map->dscp < 0) ||
        (dscp_map->dscp > 63)) {
        return (BCM_E_PARAM);
    }

    if (!BCM_L3_DSCP_MAP_USED_GET(unit, dscp_map_id)) {
        return (BCM_E_NOT_FOUND);
    }

#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit)) {
        /* Read dscp map from forst available port. */
        PBMP_PORT_ITER(unit, port) {
            rv = bcm_esw_tunnel_dscp_map_port_get(unit, port, dscp_map);
            break;
        }
    } else 
#endif /* BCM_TRX_SUPPORT */
    {
        rv = bcm_xgs3_tunnel_dscp_map_get(unit, dscp_map_id, dscp_map);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return (rv);
}

/*
* Function:
*      bcm_tunnel_dscp_map_port_set
* Purpose:
*      Set the mapping of { internal priority, color }
*      to a DSCP value for outer tunnel headers
*      for a specific port.
* Parameters:
*      unit         - (IN) BCM device number
*      port         - (IN) Egress port number.
*      dscp_map     - (IN) mapping of internal priority & color to a dscp
* Returns:
*      BCM_E_XXX
*/
int
bcm_esw_tunnel_dscp_map_port_set(int unit, bcm_port_t port,
                             bcm_tunnel_dscp_map_t *dscp_map)
{
#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_l3)) {
        return (_bcm_trx_tunnel_dscp_map_port_set(unit, port, dscp_map));
    }
#endif
    return BCM_E_UNAVAIL;
}
/*
* Function:
*      bcm_tunnel_dscp_map_port_get
* Purpose:
*      Get the mapping of { internal priority, color }
*      to a DSCP value for outer tunnel headers
*      for a specific port.
* Parameters:
*      unit         - (IN) SOC unit #
*      port         - (IN) Egress port number.
*      dscp_map     - (IN/OUT) IN priority, color
*                              OUT dscp value
* Returns:
*      BCM_E_XXX
*/
int
bcm_esw_tunnel_dscp_map_port_get(int unit, bcm_port_t port,
                                 bcm_tunnel_dscp_map_t *dscp_map)
{
#if defined(BCM_TRX_SUPPORT)
    if (SOC_IS_TRX(unit) && soc_feature(unit, soc_feature_l3)) {
        return (_bcm_trx_tunnel_dscp_map_port_get(unit, port, dscp_map));
    }
#endif
    return (BCM_E_UNAVAIL);
}

/*
 * Function:
 *      bcm_esw_tunnel_config_get 
 * Purpose:
 *      Get the global tunnel property
 * Parameters:
 *      unit    - (IN)SOC unit number. 
 *      tconfig - (IN/OUT)Global information about the L3 tunneling config.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_tunnel_config_get(int unit, bcm_tunnel_config_t *tconfig)
{
    if (soc_feature(unit, soc_feature_no_tunnel)) {
        return (BCM_E_UNAVAIL);
    }

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit) && !SOC_IS_HAWKEYE(unit) &&
        soc_feature(unit, soc_feature_l3)) {
        return bcm_xgs3_tunnel_config_get(unit, tconfig);
    }
#endif  /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      bcm_tunnel_terminator_vlan_set 
 * Purpose:
 *      Set the allowed VLANs for a WLAN tunnel
 * Parameters:
 *      unit    - (IN)SOC unit number. 
 *      tunnel  - (IN)Tunnel ID.
 *      vlan_vec - (IN)VLAN vector
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_tunnel_terminator_vlan_set(int unit, bcm_gport_t tunnel, 
                               bcm_vlan_vector_t vlan_vec)
{
    int rv = BCM_E_UNAVAIL;

    if (soc_feature(unit, soc_feature_no_tunnel)) {
        return (BCM_E_UNAVAIL);
    }

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_wlan)) {
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
            rv = bcm_tr3_wlan_tunnel_terminator_vlan_set(unit, tunnel, vlan_vec);
        } else 
#endif /* BCM_TRIUMPH3_SUPPORT */
        {
            rv = bcm_tr2_tunnel_terminator_vlan_set(unit, tunnel, vlan_vec);

        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT || BCM_TRIUMPH3_SUPPORT */
    return rv;
}

/*
 * Function:
 *      bcm_tunnel_terminator_vlan_get 
 * Purpose:
 *      Get the allowed VLANs for a WLAN tunnel
 * Parameters:
 *      unit    - (IN)SOC unit number. 
 *      tunnel  - (IN)Tunnel ID.
 *      vlan_vec - (OUT)VLAN vector
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_esw_tunnel_terminator_vlan_get(int unit, bcm_gport_t tunnel, 
                               bcm_vlan_vector_t *vlan_vec)
{
    int rv = BCM_E_UNAVAIL;

    if (soc_feature(unit, soc_feature_no_tunnel)) {
        return (BCM_E_UNAVAIL);
    }

#if defined(BCM_TRIUMPH2_SUPPORT) || defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_wlan)) {
#if defined(BCM_TRIUMPH3_SUPPORT)
        if (SOC_IS_TRIUMPH3(unit)) {
            rv = bcm_tr3_wlan_tunnel_terminator_vlan_get(unit, tunnel, vlan_vec);
        } else
#endif /* BCM_TRIUMPH3_SUPPORT */
        {
            rv = bcm_tr2_tunnel_terminator_vlan_get(unit, tunnel, vlan_vec);
        }
    }
#endif /* BCM_TRIUMPH2_SUPPORT || BCM_TRIUMPH3_SUPPORT */
    return rv;
}

#else /* INCLUDE_L3 */
typedef int _bcm_esw_tunnel_not_empty; /* Make ISO compilers happy. */
#endif /* INCLUDE_L3 */
