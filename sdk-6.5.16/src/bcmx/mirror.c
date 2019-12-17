/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        bcmx/mirror.c
 * Purpose:     BCMX Mirroring APIs
 */

#include <sal/core/libc.h>
#include <bcm/types.h>
#include <bcm/topo.h>

#include <bcmx/mirror.h>
#include <bcmx/lport.h>
#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>

#include "bcmx_int.h"

#define BCMX_MIRROR_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_MIRROR_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_MIRROR_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_MIRROR_DELETE_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_DELETE_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_MIRROR_GET_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)


#define BCMX_MIRROR_DESTINATION_T_PTR_TO_BCM(_dest)    \
    ((bcm_mirror_destination_t *)(_dest))


/* Mirror-to port and preserve-format are cached locally */
static bcmx_lport_t mirror_to_lport = BCMX_LPORT_INVALID;

/*
  Get system-wide state of directed mirroring from the first switch
  found.
*/

STATIC int
_bcmx_mirror_directed_get(int *directed)
{
    int bcm_unit, i, rv;
    bcm_info_t info;

    rv = BCM_E_NONE;
    *directed = 0;

    BCMX_UNIT_ITER(bcm_unit, i) {

        BCM_IF_ERROR_RETURN(bcm_info_get(bcm_unit, &info));

        if (info.capability & BCM_INFO_SWITCH) {
            rv = bcm_switch_control_get(bcm_unit,
                                        bcmSwitchDirectedMirroring,
                                        directed);
            if (rv == BCM_E_NONE || rv != BCM_E_UNAVAIL) {
                break;
            }
        }
    }

    if (rv == BCM_E_UNAVAIL) {
        rv = BCM_E_NONE;
    }

    return rv;
}


/*
 * Function:
 *     bcmx_mirror_destination_t_init
 */
void
bcmx_mirror_destination_t_init(bcmx_mirror_destination_t *mirror_dest)
{
    if (mirror_dest != NULL) {
        bcm_mirror_destination_t_init
            (BCMX_MIRROR_DESTINATION_T_PTR_TO_BCM(mirror_dest));
    }
}


/*
 * Function:
 *      bcmx_mirror_init
 */

int
bcmx_mirror_init(void)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_MIRROR_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mirror_init(bcm_unit);
        if (BCM_FAILURE(BCMX_MIRROR_ERROR_CHECK(bcm_unit, tmp_rv, &rv))) {
            break;
        }
    }

    mirror_to_lport = BCMX_LPORT_INVALID;

    return rv;
}

/*
 * Function:
 *      bcmx_mirror_mode_set
 */

int
bcmx_mirror_mode_set(int mode)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int i, bcm_unit;

    BCMX_MIRROR_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mirror_mode_set(bcm_unit, mode);
        BCM_IF_ERROR_RETURN(BCMX_MIRROR_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_mirror_mode_get
 */

int
bcmx_mirror_mode_get(int *mode)
{
    int rv;
    int i, bcm_unit;

    BCMX_MIRROR_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mode);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_mirror_mode_get(bcm_unit, mode);
        if (BCMX_MIRROR_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    *mode = BCM_MIRROR_DISABLE;
    return BCM_E_UNAVAIL;
}

/*
 * Function:
 *      _bcmx_port_info
 *
 * A device is a switch if it has one or more Ethernet ports.
 * A device is a fabric if it has two or more stack ports.
 * A device could be both a switch and a fabric at the same time.
 */

STATIC void
_bcmx_device_info(bcm_port_config_t *config, int *switchlike, int *fabriclike)
{
    int eport, sport;

    BCM_PBMP_COUNT(config->e, eport);
    BCM_PBMP_COUNT(config->stack_ext, sport);
    *switchlike = (eport > 0);
    *fabriclike = (sport > 1);
}

STATIC int
_bcmx_undirected_mirror_fabric_set(int unit,
                                   bcm_port_config_t *config,
                                   bcm_module_t mirror_modid)
{
    int               rv;
    bcm_port_t        hport;
    bcm_pbmp_t        pbmp;

    BCM_PBMP_ITER(config->stack_ext, hport) {
        /*
         *  Fabric device needs to use the UC bitmap
         *  to set each stack port individually
         */
        BCM_PBMP_CLEAR(pbmp);
        BCM_IF_ERROR_RETURN
            (bcm_stk_ucbitmap_get(unit, hport,
                                  mirror_modid, &pbmp));
        /* Remove the CMIC from the bitmap */
        BCM_PBMP_REMOVE(pbmp, config->cpu);
        rv = bcm_mirror_to_pbmp_set(unit, hport, pbmp);
        if (rv != BCM_E_UNAVAIL) {
            BCM_IF_ERROR_RETURN(rv);
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmx_undirected_mirror_stk_clear
 *
 * Clear stack ports of any undirected mirroring forwarding
 * configuration.
 */

STATIC int
_bcmx_undirected_mirror_stk_clear(int unit)
{
    int               rv;
    bcm_port_config_t config;
    bcm_port_t        hport;
    bcm_pbmp_t        pbmp;


    BCM_PBMP_CLEAR(pbmp);
    BCM_IF_ERROR_RETURN(bcm_port_config_get(unit, &config));
    BCM_PBMP_ITER(config.stack_ext, hport) {
        rv = bcm_mirror_to_pbmp_set(unit, hport, pbmp);
        if (rv != BCM_E_UNAVAIL) {
            BCM_IF_ERROR_RETURN(rv);
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmx_mirror_to_set
 */

int
bcmx_mirror_to_set(bcmx_lport_t port)
{
    int bcm_unit, i;
    bcm_port_t port_on_src;
    bcm_module_t mirror_modid;
    int unit_src;
    bcm_port_t modport;
    bcm_port_config_t config;
    int switchlike, fabriclike;

    BCMX_MIRROR_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port,
                                 &unit_src, &port_on_src,
                                 BCMX_DEST_CONVERT_DEFAULT));

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_modid_port(port,
                                  &mirror_modid, &modport,
                                  BCMX_DEST_CONVERT_DEFAULT));

    mirror_to_lport = port;
    BCMX_UNIT_ITER(bcm_unit, i) {
        if (bcm_unit == unit_src) {
            BCM_IF_ERROR_RETURN(bcm_mirror_to_set(bcm_unit, port_on_src));
        } else {
            BCM_IF_ERROR_RETURN(bcm_port_config_get(bcm_unit, &config));
            _bcmx_device_info(&config, &switchlike, &fabriclike);

            if (fabriclike) {
                BCM_IF_ERROR_RETURN
                    (_bcmx_undirected_mirror_fabric_set(bcm_unit, &config,
                                                        mirror_modid));
            }
            if (switchlike) {
                int port_on_src;

                BCM_IF_ERROR_RETURN
                    (bcm_topo_port_get(bcm_unit, mirror_modid,
                                       &port_on_src));
                BCM_IF_ERROR_RETURN
                    (bcm_mirror_to_set(bcm_unit, port_on_src));
            }
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmx_mirror_to_get
 */

int
bcmx_mirror_to_get(bcmx_lport_t *port)
{
    BCMX_MIRROR_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(port);

    *port = mirror_to_lport;

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmx_mirror_ingress_set
 */

int
bcmx_mirror_ingress_set(bcmx_lport_t port, int val)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_MIRROR_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_mirror_ingress_set(bcm_unit, bcm_port, val);
}


/*
 * Function:
 *      bcmx_mirror_ingress_get
 */

int
bcmx_mirror_ingress_get(bcmx_lport_t port, int *val)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_MIRROR_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_mirror_ingress_get(bcm_unit, bcm_port, val);
}


/*
 * Function:
 *      bcmx_mirror_egress_set
 */

int
bcmx_mirror_egress_set(bcmx_lport_t port, int val)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_MIRROR_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_mirror_egress_set(bcm_unit, bcm_port, val);
}


/*
 * Function:
 *      bcmx_mirror_egress_get
 */

int
bcmx_mirror_egress_get(bcmx_lport_t port, int *val)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_MIRROR_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_mirror_egress_get(bcm_unit, bcm_port, val);
}


/*
 * Function:
 *      bcmx_mirror_fabric_list_set
 */

int
bcmx_mirror_fabric_list_set(bcmx_lport_t port, bcmx_lplist_t lplist)
{
    int bcm_unit, i;
    int rv = BCM_E_UNAVAIL, tmp_rv;
    bcm_pbmp_t pbmp;
    int         lport_unit;
    bcm_port_t  lport_port;

    BCMX_MIRROR_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port,
                                 &lport_unit, &lport_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    BCMX_UNIT_ITER(bcm_unit, i) {
        BCMX_LPLIST_TO_PBMP(lplist, bcm_unit, pbmp);
        if (BCM_PBMP_NOT_NULL(pbmp)) {
            tmp_rv = bcm_mirror_to_pbmp_set(bcm_unit, lport_port,
                                            pbmp);
            BCM_IF_ERROR_RETURN
                (BCMX_MIRROR_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
        }
    }

    return rv;
}


/*
 * Function:
 *      bcmx_mirror_fabric_list_get
 */

int
bcmx_mirror_fabric_list_get(bcmx_lport_t port, bcmx_lplist_t *lplist)
{
    int bcm_unit, i;
    int rv = BCM_E_UNAVAIL, tmp_rv;
    bcm_pbmp_t pbmp;
    int         lport_unit;
    bcm_port_t  lport_port;

    BCMX_MIRROR_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port,
                                 &lport_unit, &lport_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    if (BCMX_LPLIST_IS_NULL(lplist)) {
        return BCM_E_PARAM;
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mirror_to_pbmp_get(bcm_unit,
                                        lport_port,
                                        &pbmp);
        if (BCMX_MIRROR_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_FAILURE(tmp_rv)) {
                break;
            }
            if (BCM_PBMP_NOT_NULL(pbmp)) {
                BCMX_LPLIST_PBMP_ADD(lplist, bcm_unit, pbmp);
            }
        }
    }

    return rv;
}


/*
 * Function:
 *      _bcmx_directed_mirror_port_set
 *
 * When the system supports directed mirroring, program the mirrored
 * port and the MTP on the unit with the mirrored port.
 */

STATIC int
_bcmx_directed_mirror_port_set(bcmx_lport_t lport,
                               bcmx_lport_t dest_lport,
                               uint32 flags)
{
    int          bcm_unit, i;
    int          lport_unit;
    bcm_port_t   lport_port;
    bcm_module_t dest_modid;
    bcm_port_t   dest_modport;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(lport,
                                 &lport_unit, &lport_port,
                                 BCMX_DEST_CONVERT_DEFAULT));
    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_modid_port(dest_lport,
                                  &dest_modid, &dest_modport,
                                  BCMX_DEST_CONVERT_DEFAULT));

    /* Clear any existing undirected mirroring settings */
    BCMX_UNIT_ITER(bcm_unit, i) {
        BCM_IF_ERROR_RETURN
            (_bcmx_undirected_mirror_stk_clear(bcm_unit));

    }

    /* set mirror port and MTP */
    BCM_IF_ERROR_RETURN(bcm_mirror_port_set(lport_unit, lport_port,
                                            dest_modid, dest_modport,
                                            flags));
    
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmx_undirected_mirror_port_set
 *
 * When the system does not support directed mirroring, program the
 * stack ports to be MTPs on all units that are not the unit the
 * system MTP is on.
 *
 * If mirroring is being disabled, only disable the mirror port
 * not any intermediate fabrics or MTP only units, as they may still
 * be in use by other mirrored ports.
 */

STATIC int
_bcmx_undirected_mirror_port_set(bcmx_lport_t lport,
                                 bcmx_lport_t dest_lport,
                                 uint32 flags)
{
    int               bcm_unit, i;
    bcm_module_t      mirror_modid;
    int               enable;
    int               lport_unit, dest_unit;
    bcm_port_t        lport_port, dest_port, dest_modport;
    bcm_port_config_t config;
    int               switchlike, fabriclike;
    

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(lport,
                                 &lport_unit, &lport_port,
                                 BCMX_DEST_CONVERT_DEFAULT));
    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(dest_lport,
                                 &dest_unit, &dest_port,
                                 BCMX_DEST_CONVERT_DEFAULT));
    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_modid_port(dest_lport,
                                  &mirror_modid, &dest_modport,
                                  BCMX_DEST_CONVERT_DEFAULT));

    enable = ((flags & (BCM_MIRROR_PORT_INGRESS |
                        BCM_MIRROR_PORT_EGRESS  |
                        BCM_MIRROR_PORT_ENABLE)) != 0);

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (bcm_unit == lport_unit) {
            /* bcm_unit is the mirrored port unit. If this happens to
               be the MTP unit, too, then the MTP will be taken care
               of here. */
            BCM_IF_ERROR_RETURN
                (bcm_mirror_port_set(bcm_unit, lport_port,
                                     mirror_modid, dest_modport, flags));
        } else if (bcm_unit == dest_unit) {
            BCM_IF_ERROR_RETURN
                (bcm_mirror_port_set(bcm_unit, BCM_GPORT_INVALID,
                                     mirror_modid, dest_modport, flags));
        } else {
            /* bcm_unit is neither a mirrored port unit or an MTP unit */
            if (enable) {
                BCM_IF_ERROR_RETURN(bcm_port_config_get(bcm_unit, &config));
                _bcmx_device_info(&config, &switchlike, &fabriclike);

                if (fabriclike) {
                    BCM_IF_ERROR_RETURN
                        (_bcmx_undirected_mirror_fabric_set(bcm_unit, &config,
                                                            mirror_modid));
                }
                if (switchlike) {
                    BCM_IF_ERROR_RETURN
                        (bcm_mirror_port_set(bcm_unit, BCM_GPORT_INVALID,
                                             mirror_modid, dest_modport,
                                             flags));
                }
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmx_mirror_port_set
 */

int
bcmx_mirror_port_set(bcmx_lport_t lport,
		     bcmx_lport_t dest_lport,
		     uint32 flags)
{
    int directed;

    BCMX_MIRROR_INIT_CHECK;

    BCMX_LPORT_CHECK(lport);
    BCMX_LPORT_CHECK(dest_lport);

    BCM_IF_ERROR_RETURN(_bcmx_mirror_directed_get(&directed));

    return directed ?
        _bcmx_directed_mirror_port_set(lport, dest_lport, flags) :
        _bcmx_undirected_mirror_port_set(lport, dest_lport, flags);
}


/*
 * Function:
 *      bcmx_mirror_port_get
 * Notes:
 *      dest_lport is undefined when no flags are set in returning function.
 */

int
bcmx_mirror_port_get(bcmx_lport_t lport,
		     bcmx_lport_t *dest_lport,
		     uint32 *flags)
{
    bcm_module_t  dest_mod;
    bcm_port_t    dest_port;
    int           bcm_unit;
    bcm_port_t    bcm_port;

    BCMX_MIRROR_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(dest_lport);
    BCMX_PARAM_NULL_CHECK(flags);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(lport, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    BCM_IF_ERROR_RETURN(bcm_mirror_port_get(bcm_unit, bcm_port,
                                            &dest_mod, &dest_port,
                                            flags));

    /*
     * Convert only when flags is set, since dest_mod/dest_port are
     * undefined when BCM returns no flags set.
     */
    if (*flags != 0) {
        BCM_IF_ERROR_RETURN
            (_bcmx_dest_from_modid_port(dest_lport, dest_mod, dest_port,
                                        BCMX_DEST_CONVERT_DEFAULT));
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmx_mirror_vlan_set
 */

int
bcmx_mirror_vlan_set(bcmx_lport_t lport,
		     uint16 tpid,
		     uint16 vlan)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_MIRROR_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(lport, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_mirror_vlan_set(bcm_unit, bcm_port, tpid, vlan);
}


/*
 * Function:
 *      bcmx_mirror_vlan_get
 */

int
bcmx_mirror_vlan_get(bcmx_lport_t lport,
		     uint16 *tpid,
		     uint16 *vlan)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_MIRROR_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(tpid);
    BCMX_PARAM_NULL_CHECK(vlan);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(lport, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_mirror_vlan_get(bcm_unit, bcm_port, tpid, vlan);
}


/*
 * Function:
 *     _bcmx_directed_mirror_port_dest_add
 *
 * Notes:
 *     When the system supports directed mirroring, add the
 *     destination mirror on the unit with the mirrored port.
 */
STATIC int
_bcmx_directed_mirror_port_dest_add(bcmx_lport_t port, uint32 flags,
                                    bcm_gport_t mirror_dest_id)
{
    int         i;
    int         bcm_unit;
    bcm_port_t  bcm_port;

    /* Clear any existing undirected mirroring settings */
    BCMX_UNIT_ITER(bcm_unit, i) {
        BCM_IF_ERROR_RETURN
            (_bcmx_undirected_mirror_stk_clear(bcm_unit));

    }

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port,
                                 &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_mirror_port_dest_add(bcm_unit, bcm_port,
                                    flags, mirror_dest_id);
}

/*
 * Function:
 *     _bcmx_undirected_mirror_port_dest_add
 * Notes:
 *     Non-direct (undirected) mirroring mode only supports
 *     single mirroring destination.
 *
 *     Assumes that BCM API will handle cases where
 *     the 'same' port is being added (need to check if BCM returns
 *     an error, if so, it should at least return BCM_E_EXISTS.
 */
STATIC int
_bcmx_undirected_mirror_port_dest_add(bcmx_lport_t port, uint32 flags,
                                      bcm_gport_t mirror_dest_id)
{
    int          bcm_unit, i;
    int          mirrored_unit;
    bcm_port_t   mirrored_port;
    bcm_port_config_t config;
    bcmx_mirror_destination_t mirror_dest;
    bcm_module_t mirror_modid;
    int          switchlike, fabriclike;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port,
                                 &mirrored_unit, &mirrored_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    bcmx_mirror_destination_t_init(&mirror_dest);
    BCM_IF_ERROR_RETURN
        (bcmx_mirror_destination_get(mirror_dest_id, &mirror_dest));

    mirror_modid = BCM_GPORT_MODPORT_MODID_GET(mirror_dest.gport);

    if (mirror_modid < 0) {
        return BCM_E_PORT;
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (bcm_unit == mirrored_unit) {
            /* bcm_unit is the mirrored port unit. If this happens to
               be the MTP unit, too, then the MTP will be taken care
               of here. */
            BCM_IF_ERROR_RETURN
                (bcm_mirror_port_dest_add(mirrored_unit, mirrored_port,
                                          flags, mirror_dest_id));
        } else {
            /* bcm_unit is not a mirrored port unit */
            BCM_IF_ERROR_RETURN(bcm_port_config_get(bcm_unit, &config));
            _bcmx_device_info(&config, &switchlike, &fabriclike);

            if (fabriclike) {
                BCM_IF_ERROR_RETURN
                    (_bcmx_undirected_mirror_fabric_set(bcm_unit, &config,
                                                        mirror_modid));
            }
            if (switchlike) {
                BCM_IF_ERROR_RETURN
                    (bcm_mirror_port_dest_add(bcm_unit,BCM_GPORT_INVALID,
                                              flags, mirror_dest_id));
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *     bcmx_mirror_port_dest_add
 */
int
bcmx_mirror_port_dest_add(bcmx_lport_t port, uint32 flags,
                          bcm_gport_t mirror_dest_id)
{
    int directed;

    BCMX_MIRROR_INIT_CHECK;

    BCMX_LPORT_CHECK(port);

    BCM_IF_ERROR_RETURN(_bcmx_mirror_directed_get(&directed));

    return directed ?
        _bcmx_directed_mirror_port_dest_add(port, flags, mirror_dest_id)   :
        _bcmx_undirected_mirror_port_dest_add(port, flags, mirror_dest_id);

}

/*
 * Function:
 *     _bcmx_undirected_dest_delete_transit
 *
 * Delete any "transit device" settings programmed by
 *   _bcmx_undirected_mirror_port_dest_add()
 */
STATIC int
_bcmx_undirected_dest_delete_transit(bcmx_lport_t port,
                                     uint32 flags,
                                     bcm_gport_t mirror_dest_id)
{
    int        i, bcm_unit;
    int        mirrored_unit;
    bcm_port_t mirrored_port;
    int        switchlike, fabriclike;
    bcm_port_config_t config;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port,
                                 &mirrored_unit, &mirrored_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    BCMX_UNIT_ITER(bcm_unit, i) {
        if (bcm_unit != mirrored_unit) {
            BCM_IF_ERROR_RETURN(bcm_port_config_get(bcm_unit, &config));
            _bcmx_device_info(&config, &switchlike, &fabriclike);
            if (switchlike) {
                BCM_IF_ERROR_RETURN
                    (bcm_mirror_port_dest_delete(bcm_unit,
                                                 BCM_GPORT_INVALID,
                                                 flags, mirror_dest_id));
            }
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     bcmx_mirror_port_dest_delete
 */
int
bcmx_mirror_port_dest_delete(bcmx_lport_t port, uint32 flags,
                             bcm_gport_t mirror_dest_id)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;
    int         directed;

    BCMX_MIRROR_INIT_CHECK;

    BCM_IF_ERROR_RETURN(_bcmx_mirror_directed_get(&directed));
    if (!directed) {
        BCM_IF_ERROR_RETURN
            (_bcmx_undirected_dest_delete_transit(port,
                                                  flags,
                                                  mirror_dest_id));
    }
    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_mirror_port_dest_delete(bcm_unit, bcm_port, flags,
                                       mirror_dest_id);
}

/*
 * Function:
 *     bcmx_mirror_port_dest_delete_all
 */
int
bcmx_mirror_port_dest_delete_all(bcmx_lport_t port, uint32 flags)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;
    int         directed;

    BCMX_MIRROR_INIT_CHECK;

    BCM_IF_ERROR_RETURN(_bcmx_mirror_directed_get(&directed));
    if (!directed) {
        bcm_gport_t mirror_dest_id;
        int mirror_dest_count;

        /* Delete any transit device configuration */
        BCM_IF_ERROR_RETURN
            (bcmx_mirror_port_dest_get(port, flags,
                                       1, &mirror_dest_id,
                                       &mirror_dest_count));

        if (mirror_dest_count > 0) {
            BCM_IF_ERROR_RETURN
                (_bcmx_undirected_dest_delete_transit(port,
                                                      flags,
                                                      mirror_dest_id));
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_mirror_port_dest_delete_all(bcm_unit, bcm_port, flags);
}


/*
 * Function:
 *     bcmx_mirror_port_dest_get
 */
int
bcmx_mirror_port_dest_get(bcmx_lport_t port, uint32 flags,
                          int mirror_dest_size, bcm_gport_t *mirror_dest_id,
                          int *mirror_dest_count)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_MIRROR_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(mirror_dest_size, mirror_dest_id);
    BCMX_PARAM_NULL_CHECK(mirror_dest_count);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_mirror_port_dest_get(bcm_unit, bcm_port, flags,
                                    mirror_dest_size, mirror_dest_id,
                                    mirror_dest_count);
}


/*
 * Function:
 *     bcmx_mirror_destination_create
 */
int
bcmx_mirror_destination_create(bcmx_mirror_destination_t *mirror_dest)
{
    int          rv = BCM_E_UNAVAIL, tmp_rv;
    int          i, bcm_unit;
    bcm_gport_t  gport_orig;
    uint32       flags_orig;

    BCMX_MIRROR_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mirror_dest);

    /*
     * Need to save original 'gport' value and copy it back to structure
     * for each iteration since BCM API may change its value (this happens
     * when the MTP does not reside in the local unit).
     */
    gport_orig = mirror_dest->gport;

    /* Store original 'xxx_WITH_ID' flag bit */
    flags_orig = mirror_dest->flags & BCM_MIRROR_DEST_WITH_ID;

    BCMX_UNIT_ITER(bcm_unit, i) {
        mirror_dest->gport = gport_orig;

        tmp_rv = bcm_mirror_destination_create
            (bcm_unit,
             BCMX_MIRROR_DESTINATION_T_PTR_TO_BCM(mirror_dest));

        if (BCM_FAILURE(BCMX_MIRROR_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv))) {
            break;
        }

        /*
         * Use the ID from first successful 'create' if mirror destination ID
         * is not specified.
         */
        if (!(mirror_dest->flags & BCM_MIRROR_DEST_WITH_ID)) {
            if (BCM_SUCCESS(tmp_rv)) {
                mirror_dest->flags |= BCM_MIRROR_DEST_WITH_ID;
            }
        }
    }

    /* Restore 'xxx_WITH_ID' flag bit */
    mirror_dest->flags &= ~BCM_MIRROR_DEST_WITH_ID;
    mirror_dest->flags |= flags_orig;

    return rv;
}

/*
 * Function:
 *     bcmx_mirror_destination_destroy
 */
int
bcmx_mirror_destination_destroy(bcm_gport_t mirror_dest_id)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_MIRROR_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_mirror_destination_destroy(bcm_unit, mirror_dest_id);
        BCM_IF_ERROR_RETURN
            (BCMX_MIRROR_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Function:
 *     _bcmx_undirected_mirror_destination_get
 *
 * Iterate on units and return mirror destination information from
 * first successful call on unit that returns an MTP that is
 * a front-panel (ethernet) port.
 */
int
_bcmx_undirected_mirror_destination_get(bcm_gport_t mirror_dest_id, 
                                        bcmx_mirror_destination_t *mirror_dest)
{
    int           rv;
    int           i, bcm_unit;
    bcm_port_encap_config_t mode;
    uint32        stk_mode;

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_mirror_destination_get(bcm_unit, mirror_dest_id,
                                        BCMX_MIRROR_DESTINATION_T_PTR_TO_BCM
                                        (mirror_dest));

        if (!BCMX_MIRROR_GET_IS_VALID(bcm_unit, rv)) {
            continue;
        }

        /*
         * The MTP must be a modport, so skip if not.
         */
        if (BCM_FAILURE(rv) || !BCM_GPORT_IS_MODPORT(mirror_dest->gport)) {
            continue;
        }
            
        /* make sure the mirror_dest is an Ethernet port. In
           undirected mode, that's the only kind of port an MTP can
           be. */

        sal_memset(&mode, 0, sizeof(mode));
        rv = bcm_port_encap_config_get(bcm_unit, mirror_dest->gport, &mode);
        if (BCM_FAILURE(rv)) {
            continue;
        }
        
        rv = bcm_stk_mode_get(bcm_unit, &stk_mode);
        if (BCM_FAILURE(rv)) {
            continue;
        }
        
        if (mode.encap == BCM_PORT_ENCAP_IEEE && stk_mode != BCM_STK_SL) {
            /* Ethernet port found */
            return BCM_E_NONE;
        }
    }

    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *     bcmx_mirror_destination_get
 */
int
bcmx_mirror_destination_get(bcm_gport_t mirror_dest_id, 
                            bcmx_mirror_destination_t *mirror_dest)
{
    int  rv;
    int  i, bcm_unit;
    int  directed;

    BCMX_MIRROR_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mirror_dest);

    /*
     * Directed Mirroring -
     *   Return information from the first successful call on a unit.
     *
     * Non-Directed Mirroring -
     *   Needs special handling.
     *   Iterate over units until a front-panel MTP is found.
     */

    BCM_IF_ERROR_RETURN(_bcmx_mirror_directed_get(&directed));

    if (directed) {
        BCMX_UNIT_ITER(bcm_unit, i) {
            rv = bcm_mirror_destination_get(bcm_unit, mirror_dest_id,
                                            BCMX_MIRROR_DESTINATION_T_PTR_TO_BCM
                                            (mirror_dest));
            if (BCMX_MIRROR_GET_IS_VALID(bcm_unit, rv)) {
                return rv;
            }
        }

    } else {
        return _bcmx_undirected_mirror_destination_get(mirror_dest_id,
                                                       mirror_dest);
    }

    return BCM_E_UNAVAIL;
}

