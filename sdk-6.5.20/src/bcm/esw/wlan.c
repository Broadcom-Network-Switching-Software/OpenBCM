/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 * All Rights Reserved.$
 *
 * WLAN module
 */
#ifdef INCLUDE_L3

#include <sal/core/libc.h>

#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/l2u.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <bcm/l2.h>
#include <bcm/l3.h>
#include <bcm/port.h>
#include <bcm/error.h>
#include <bcm/vlan.h>
#include <bcm/rate.h>
#include <bcm/ipmc.h>
#include <bcm/wlan.h>
#include <bcm/stack.h>
#include <bcm/topo.h>

#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/l3.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/xgs3.h>
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/triumph2.h>
#endif /* BCM_TRIUMPH2_SUPPORT */
#if defined(BCM_TRIUMPH3_SUPPORT)
#include <bcm_int/esw/triumph3.h>
#endif /* BCM_TRIUMPH3_SUPPORT */

/* Initialize the WLAN module. */
int 
bcm_esw_wlan_init(int unit)
{
    int rv = BCM_E_UNAVAIL;

    if (!soc_feature(unit, soc_feature_wlan)) {
        return rv;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_axp)) {
        rv =  bcm_tr3_wlan_init(unit);
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */
  
#ifdef BCM_TRIUMPH2_SUPPORT
        rv = bcm_tr2_wlan_init(unit);
#endif
    return rv;
}

/* Detach the WLAN module. */
int 
bcm_esw_wlan_detach(int unit)
{
    int rv = BCM_E_NONE;

    if (!soc_feature(unit, soc_feature_wlan)) {
        return rv;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_axp)) {
        rv = bcm_tr3_wlan_detach(unit);
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIUMPH2_SUPPORT
        rv = bcm_tr2_wlan_detach(unit);
#endif
    return rv;
}

/* Add a WLAN client to the database. */
int 
bcm_esw_wlan_client_add(int unit, bcm_wlan_client_t *info)
{
    int rv = BCM_E_UNAVAIL;

    if (!soc_feature(unit, soc_feature_wlan)) {
        return rv;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_axp)) {
        rv = bcm_tr3_wlan_client_add(unit, info);
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIUMPH2_SUPPORT
        rv = bcm_tr2_wlan_client_add(unit, info);
#endif
    return rv;
}

/* Delete a WLAN client from the database. */
int 
bcm_esw_wlan_client_delete(int unit, bcm_mac_t mac)
{
    int rv = BCM_E_UNAVAIL;

    if (!soc_feature(unit, soc_feature_wlan)) {
        return rv;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_axp)) {
        rv = bcm_tr3_wlan_client_delete(unit, mac);
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIUMPH2_SUPPORT
        rv = bcm_tr2_wlan_client_delete(unit, mac);
#endif
    return rv;
}


/* Delete all WLAN clients. */
int 
bcm_esw_wlan_client_delete_all(int unit)
{
    int rv = BCM_E_UNAVAIL;

    if (!soc_feature(unit, soc_feature_wlan)) {
        return rv;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_axp)) {
        rv = bcm_tr3_wlan_client_delete_all(unit);
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIUMPH2_SUPPORT
        rv = bcm_tr2_wlan_client_delete_all(unit);
#endif
    return rv;
}

/* Get a WLAN client by MAC. */
int 
bcm_esw_wlan_client_get(int unit, bcm_mac_t mac, bcm_wlan_client_t *info)
{
    int rv = BCM_E_UNAVAIL;

    if (!soc_feature(unit, soc_feature_wlan)) {
        return rv;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_axp)) {
        rv = bcm_tr3_wlan_client_get(unit, mac, info);
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIUMPH2_SUPPORT
        rv = bcm_tr2_wlan_client_get(unit, mac, info);
#endif
    return rv;
}

/* bcm_wlan_client_traverse */
int 
bcm_esw_wlan_client_traverse(int unit, bcm_wlan_client_traverse_cb cb, 
                             void *user_data)
{
    int rv = BCM_E_UNAVAIL;

    if (!soc_feature(unit, soc_feature_wlan)) {
        return rv;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_axp)) {
        rv = bcm_tr3_wlan_client_traverse(unit, cb, user_data);
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIUMPH2_SUPPORT
        rv = bcm_tr2_wlan_client_traverse(unit, cb, user_data);
#endif
    return rv;
}

/* bcm_wlan_port_add */
int 
bcm_esw_wlan_port_add(int unit, bcm_wlan_port_t *info)
{
    int rv = BCM_E_UNAVAIL;

    if (!soc_feature(unit, soc_feature_wlan)) {
        return rv;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_axp)) {
        rv = bcm_tr3_wlan_port_add(unit, info);
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIUMPH2_SUPPORT
        rv = bcm_tr2_wlan_port_add(unit, info);
#endif
    return rv;
}

/* bcm_wlan_port_delete */
int 
bcm_esw_wlan_port_delete(int unit, bcm_gport_t wlan_port_id)
{
    int rv = BCM_E_UNAVAIL;

    if (!soc_feature(unit, soc_feature_wlan)) {
        return rv;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_axp)) {
        rv = bcm_tr3_wlan_port_delete(unit, wlan_port_id);
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIUMPH2_SUPPORT
        rv = bcm_tr2_wlan_port_delete(unit, wlan_port_id);
#endif
    return rv;
}

/* bcm_wlan_port_delete_all */
int 
bcm_esw_wlan_port_delete_all(int unit)
{
    int rv = BCM_E_UNAVAIL;

    if (!soc_feature(unit, soc_feature_wlan)) {
        return rv;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_axp)) {
        rv = bcm_tr3_wlan_port_delete_all(unit);
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIUMPH2_SUPPORT
        rv = bcm_tr2_wlan_port_delete_all(unit);
#endif
    return rv;
}

/* bcm_wlan_port_get */
int 
bcm_esw_wlan_port_get(int unit, bcm_gport_t wlan_port_id, bcm_wlan_port_t *info)
{
    int rv = BCM_E_UNAVAIL;

    if (!soc_feature(unit, soc_feature_wlan)) {
        return rv;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_axp)) {
        rv = bcm_tr3_wlan_port_get(unit, wlan_port_id, info);
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIUMPH2_SUPPORT
        rv = bcm_tr2_wlan_port_get(unit, wlan_port_id, info);
#endif
    return rv;
}

/* bcm_wlan_port_traverse */
int 
bcm_esw_wlan_port_traverse(int unit, bcm_wlan_port_traverse_cb cb, 
                           void *user_data)
{
    int rv = BCM_E_UNAVAIL;

    if (!soc_feature(unit, soc_feature_wlan)) {
        return rv;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_axp)) {
        rv = bcm_tr3_wlan_port_traverse(unit, cb, user_data);
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIUMPH2_SUPPORT
        rv = bcm_tr2_wlan_port_traverse(unit, cb, user_data);
#endif
    return rv;
}

/* WLAN tunnel initiator create */
int
bcm_esw_wlan_tunnel_initiator_create(int unit, bcm_tunnel_initiator_t *info)
{
    int rv = BCM_E_UNAVAIL;

    if (!soc_feature(unit, soc_feature_wlan)) {
        return rv;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_axp)) {
        rv = bcm_tr3_wlan_tunnel_initiator_create(unit, info);
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIUMPH2_SUPPORT
        rv = bcm_tr2_wlan_tunnel_initiator_create(unit, info);
#endif
    return rv;
}

/* WLAN tunnel initiator destroy */
int
bcm_esw_wlan_tunnel_initiator_destroy(int unit, bcm_gport_t wlan_tunnel_id)
{
    int rv = BCM_E_UNAVAIL;

    if (!soc_feature(unit, soc_feature_wlan)) {
        return rv;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_axp)) {
        rv = bcm_tr3_wlan_tunnel_initiator_destroy(unit, wlan_tunnel_id);
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIUMPH2_SUPPORT
        rv = bcm_tr2_wlan_tunnel_initiator_destroy(unit, wlan_tunnel_id);
#endif
    return rv;
}

/* WLAN tunnel initiator get */
int
bcm_esw_wlan_tunnel_initiator_get(int unit, bcm_tunnel_initiator_t *info)
{
    int rv = BCM_E_UNAVAIL;

    if (!soc_feature(unit, soc_feature_wlan)) {
        return rv;
    }

#if defined(BCM_TRIUMPH3_SUPPORT)
    if (soc_feature(unit, soc_feature_axp)) {
        rv = bcm_tr3_wlan_tunnel_initiator_get(unit, info);
    } else
#endif /* BCM_TRIUMPH3_SUPPORT */

#ifdef BCM_TRIUMPH2_SUPPORT
        rv = bcm_tr2_wlan_tunnel_initiator_get(unit, info);
#endif
    return rv;
}

#else /* INCLUDE_L3 */
typedef int _bcm_esw_wlan_not_empty; /* Make ISO compilers happy. */
#endif  /* INCLUDE_L3 */
