/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2019 Broadcom Inc. All rights reserved.
 *
 * File:        src/bcmx/port.c
 * Purpose:     BCMX port APIs
 */

#include <sal/core/libc.h>
#include <shared/alloc.h>

#include <bcm/types.h>

#include <bcmx/port.h>
#include <bcmx/lport.h>
#include <bcmx/bcmx.h>
#include <bcmx/lplist.h>

#include "bcmx_int.h"

#define BCMX_PORT_INIT_CHECK    BCMX_READY_CHECK

#define BCMX_PORT_SET_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_SET_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_PORT_DELETE_ERROR_CHECK(_unit, _check, _rv)    \
    BCMX_DELETE_ERROR_CHECK(_unit, _check, _rv)

#define BCMX_PORT_GET_IS_VALID(_unit, _rv)    \
    BCMX_ERROR_IS_VALID(_unit, _rv)


#define BCMX_PORT_ABILITY_T_PTR_TO_BCM(_ability)    \
    ((bcm_port_ability_t *)(_ability))

#define BCMX_PORT_ENCAP_CONFIG_T_PTR_TO_BCM(_config)    \
    ((bcm_port_encap_config_t *)(_config))

#define BCMX_PORT_CONGESTION_CONFIG_T_PTR_TO_BCM(_config)    \
    ((bcm_port_congestion_config_t *)(_config))

#define BCMX_PORT_MATCH_INFO_T_PTR_TO_BCM(_port_match_info)    \
    ((bcm_port_match_info_t *)(_port_match_info))

#define BCMX_PORT_TIMESYNC_CONFIG_T_PTR_TO_BCM(_config)    \
    ((bcm_port_phy_timesync_config_t *)(_config))

#define BCMX_PORT_TIMESYNC_ENHANCED_CAPTURE_T_PTR_TO_BCM(_value)    \
    ((bcm_port_phy_timesync_enhanced_capture_t *)(_value))

/*
 * Function:
 *      bcmx_port_config_t_init
 * Purpose:
 *      Initialize a Port config object struct.
 * Parameters:
 *      pconfig - Pointer to the Port Config object struct.
 * Returns:
 *      NONE
 */
void
bcmx_port_config_t_init(bcmx_port_config_t *pconfig)
{
    if (pconfig != NULL) {
        sal_memset(pconfig, 0, sizeof (*pconfig));
    }

    return;
}


/*
 * Function:
 *      bcmx_port_encap_config_t_init
 * Purpose:
 *      Initialize a port encapsulation configuration object struct.
 * Parameters:
 *      encap_config - Pointer to the struct to be initialized.
 * Returns:
 *      NONE
 */
void
bcmx_port_encap_config_t_init(bcmx_port_encap_config_t *encap_config)
{
    if (encap_config != NULL) {
        bcm_port_encap_config_t_init
            (BCMX_PORT_ENCAP_CONFIG_T_PTR_TO_BCM(encap_config));
    }

    return;
}

/*
 * Function:
 *      bcmx_port_congestion_config_t_init
 * Purpose:
 *      Initialize a port congestion configuration object struct.
 * Parameters:
 *      config - Pointer to the struct to be initialized.
 * Returns:
 *      NONE
 */
void
bcmx_port_congestion_config_t_init(bcmx_port_congestion_config_t *config)
{
    if (config != NULL) {
        bcm_port_congestion_config_t_init
            (BCMX_PORT_CONGESTION_CONFIG_T_PTR_TO_BCM(config));
    }

    return;
}

/*
 * Function:
 *      bcmx_port_match_info_t_init
 * Purpose:
 *      Initialize a port match info object struct.
 * Parameters:
 *      port_match_info - Pointer to the struct to be initialized.
 * Returns:
 *      NONE
 */
void
bcmx_port_match_info_t_init(bcmx_port_match_info_t *port_match_info)
{
    if (port_match_info != NULL) {
        bcm_port_match_info_t_init
            (BCMX_PORT_MATCH_INFO_T_PTR_TO_BCM(port_match_info));
    }

    return;
}

/*
 * Function:
 *      bcmx_port_phy_timesync_config_t_init
 * Purpose:
 *      Initialize a port phy_timesync config object struct.
 * Parameters:
 *      conf - Pointer to the struct to be initialized.
 * Returns:
 *      NONE
 */
void
bcmx_port_phy_timesync_config_t_init(bcmx_port_phy_timesync_config_t *conf)
{
    if (conf != NULL) {
        bcm_port_phy_timesync_config_t_init
            (BCMX_PORT_TIMESYNC_CONFIG_T_PTR_TO_BCM(conf));
    }

    return;
}


int
bcmx_port_config_get(bcmx_port_config_t *config)
{
    bcm_port_config_t bcm_cfg;
    int i, bcm_unit;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(config);

    BCM_IF_ERROR_RETURN(bcmx_lplist_clear(&config->fe));
    BCM_IF_ERROR_RETURN(bcmx_lplist_clear(&config->ge));
    BCM_IF_ERROR_RETURN(bcmx_lplist_clear(&config->xe));
    BCM_IF_ERROR_RETURN(bcmx_lplist_clear(&config->ce));
    BCM_IF_ERROR_RETURN(bcmx_lplist_clear(&config->e));
    BCM_IF_ERROR_RETURN(bcmx_lplist_clear(&config->hg));
    BCM_IF_ERROR_RETURN(bcmx_lplist_clear(&config->port));
    BCM_IF_ERROR_RETURN(bcmx_lplist_clear(&config->cpu));
    BCM_IF_ERROR_RETURN(bcmx_lplist_clear(&config->all));
    BCM_IF_ERROR_RETURN(bcmx_lplist_clear(&config->stack_int));
    BCM_IF_ERROR_RETURN(bcmx_lplist_clear(&config->stack_ext));

    BCMX_UNIT_ITER(bcm_unit, i) {
        BCM_IF_ERROR_RETURN(bcm_port_config_get(bcm_unit, &bcm_cfg));
        BCM_IF_ERROR_RETURN
            (BCMX_LPLIST_PBMP_ADD(&config->fe, bcm_unit, bcm_cfg.fe));
        BCM_IF_ERROR_RETURN
            (BCMX_LPLIST_PBMP_ADD(&config->ge, bcm_unit, bcm_cfg.ge));
        BCM_IF_ERROR_RETURN
            (BCMX_LPLIST_PBMP_ADD(&config->xe, bcm_unit, bcm_cfg.xe));
        BCM_IF_ERROR_RETURN
            (BCMX_LPLIST_PBMP_ADD(&config->ce, bcm_unit, bcm_cfg.ce));
        BCM_IF_ERROR_RETURN
            (BCMX_LPLIST_PBMP_ADD(&config->e, bcm_unit, bcm_cfg.e));
        BCM_IF_ERROR_RETURN
            (BCMX_LPLIST_PBMP_ADD(&config->hg, bcm_unit, bcm_cfg.hg));
        BCM_IF_ERROR_RETURN
            (BCMX_LPLIST_PBMP_ADD(&config->port, bcm_unit, bcm_cfg.port));
        BCM_IF_ERROR_RETURN
            (BCMX_LPLIST_PBMP_ADD(&config->cpu, bcm_unit, bcm_cfg.cpu));
        BCM_IF_ERROR_RETURN
            (BCMX_LPLIST_PBMP_ADD(&config->all, bcm_unit, bcm_cfg.all));

        /* Set 'stack_int' for cleanliness in case this field is
         * supported in the future.
         */
        BCM_IF_ERROR_RETURN
            (BCMX_LPLIST_PBMP_ADD(&config->stack_int, bcm_unit,
                                  bcm_cfg.stack_int));
        /*
         * Member 'stack_ext' is all ports using hg/hg2 packet
         * encapsulation and forwarding semantics.
         */
        BCM_IF_ERROR_RETURN
            (BCMX_LPLIST_PBMP_ADD(&config->stack_ext, bcm_unit,
                                  bcm_cfg.stack_ext));
    }

    return BCM_E_NONE;
}

int bcmx_port_lplist_populate(bcmx_lplist_t *list, uint32 flags)
{
    bcm_port_config_t bcm_cfg;
    int i, bcm_unit;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN(bcmx_lplist_clear(list));

    /* All ports requested? */
    if (flags & BCMX_PORT_LP_ALL) {
        flags |= 
            (BCMX_PORT_LP_FE |
             BCMX_PORT_LP_GE |
             BCMX_PORT_LP_XE |
             BCMX_PORT_LP_CE |
             BCMX_PORT_LP_HG |
             BCMX_PORT_LP_E |
             BCMX_PORT_LP_PORT |
             BCMX_PORT_LP_CPU |
             BCMX_PORT_LP_STACK_INT |
             BCMX_PORT_LP_STACK_EXT);
    }

    
    BCMX_UNIT_ITER(bcm_unit, i) {
        
        BCM_IF_ERROR_RETURN(bcm_port_config_get(bcm_unit, &bcm_cfg));
        
        if (flags & BCMX_PORT_LP_FE) {
            BCM_IF_ERROR_RETURN
                (BCMX_LPLIST_PBMP_ADD(list, bcm_unit, bcm_cfg.fe));
        }
        
        if (flags & BCMX_PORT_LP_GE) {
            BCM_IF_ERROR_RETURN
                (BCMX_LPLIST_PBMP_ADD(list, bcm_unit, bcm_cfg.ge));
        }
        
        if (flags & BCMX_PORT_LP_XE) {
            BCM_IF_ERROR_RETURN
                (BCMX_LPLIST_PBMP_ADD(list, bcm_unit, bcm_cfg.xe));
        }
        
        if (flags & BCMX_PORT_LP_CE) {
            BCM_IF_ERROR_RETURN
                (BCMX_LPLIST_PBMP_ADD(list, bcm_unit, bcm_cfg.ce));
        }
        
        if (flags & BCMX_PORT_LP_HG) {
            BCM_IF_ERROR_RETURN
                (BCMX_LPLIST_PBMP_ADD(list, bcm_unit, bcm_cfg.hg));
        }
        
        if (flags & BCMX_PORT_LP_E) {
            BCM_IF_ERROR_RETURN
                (BCMX_LPLIST_PBMP_ADD(list, bcm_unit, bcm_cfg.e));
        }
        
        if (flags & BCMX_PORT_LP_PORT) {
            BCM_IF_ERROR_RETURN
                (BCMX_LPLIST_PBMP_ADD(list, bcm_unit, bcm_cfg.port));
        }
        
        if (flags & BCMX_PORT_LP_CPU) {
            BCM_IF_ERROR_RETURN
                (BCMX_LPLIST_PBMP_ADD(list, bcm_unit, bcm_cfg.cpu));
        }
        
        if (flags & BCMX_PORT_LP_STACK_INT) {
            BCM_IF_ERROR_RETURN
                (BCMX_LPLIST_PBMP_ADD(list, bcm_unit, bcm_cfg.stack_int));
        }

        if (flags & BCMX_PORT_LP_STACK_EXT) {
            BCM_IF_ERROR_RETURN
                (BCMX_LPLIST_PBMP_ADD(list, bcm_unit, bcm_cfg.stack_ext));
        }

    }

    return BCM_E_NONE;
}

int
bcmx_port_enable_set(bcmx_lport_t port, int enable)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_enable_set(bcm_unit, bcm_port,
                               enable);
}


int
bcmx_port_enable_get(bcmx_lport_t port, int *enable)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(enable);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_enable_get(bcm_unit, bcm_port,
                               enable);
}

void
bcmx_port_ability_t_init(bcmx_port_ability_t *ability)
{
    if (ability != NULL) {
        bcm_port_ability_t_init(BCMX_PORT_ABILITY_T_PTR_TO_BCM(ability));
    }
}

int
bcmx_port_ability_advert_set(bcmx_lport_t port,
                             bcmx_port_ability_t *ability_mask)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(ability_mask);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_ability_advert_set(bcm_unit, bcm_port,
                                       BCMX_PORT_ABILITY_T_PTR_TO_BCM
                                       (ability_mask));
}

int
bcmx_port_ability_advert_get(bcmx_lport_t port,
                             bcmx_port_ability_t *ability_mask)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(ability_mask);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_ability_advert_get(bcm_unit, bcm_port,
                                       BCMX_PORT_ABILITY_T_PTR_TO_BCM
                                       (ability_mask));
}

int bcmx_port_ability_remote_get(bcmx_lport_t port,
                                 bcmx_port_ability_t *ability_mask)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(ability_mask);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_ability_remote_get(bcm_unit, bcm_port,
                                       BCMX_PORT_ABILITY_T_PTR_TO_BCM
                                       (ability_mask));
}

int
bcmx_port_ability_local_get(bcmx_lport_t port,
                            bcmx_port_ability_t *ability_mask)

{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(ability_mask);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_ability_local_get(bcm_unit, bcm_port,
                                      BCMX_PORT_ABILITY_T_PTR_TO_BCM
                                      (ability_mask));
}

int
bcmx_port_advert_set(bcmx_lport_t port, bcm_port_abil_t ability_mask)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_advert_set(bcm_unit, bcm_port,
                               ability_mask);
}


int
bcmx_port_advert_get(bcmx_lport_t port, bcm_port_abil_t *ability_mask)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(ability_mask);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_advert_get(bcm_unit, bcm_port,
                               ability_mask);
}


int
bcmx_port_advert_remote_get(bcmx_lport_t port, bcm_port_abil_t *ability_mask)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(ability_mask);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_advert_remote_get(bcm_unit, bcm_port,
                                      ability_mask);
}


int
bcmx_port_ability_get(bcmx_lport_t port, bcm_port_abil_t *local_ability_mask)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(local_ability_mask);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_ability_get(bcm_unit, bcm_port,
                                local_ability_mask);
}


int
bcmx_port_untagged_vlan_set(bcmx_lport_t port, bcm_vlan_t vid)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_untagged_vlan_set(bcm_unit, bcm_port,
                                      vid);
}


int
bcmx_port_untagged_vlan_get(bcmx_lport_t port, bcm_vlan_t *vid_ptr)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(vid_ptr);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_untagged_vlan_get(bcm_unit, bcm_port,
                                      vid_ptr);
}


int
bcmx_port_untagged_priority_set(bcmx_lport_t port, int priority)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_untagged_priority_set(bcm_unit, bcm_port,
                                          priority);
}


int
bcmx_port_untagged_priority_get(bcmx_lport_t port, int *priority)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(priority);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_untagged_priority_get(bcm_unit, bcm_port,
                                          priority);
}


int
bcmx_port_dscp_map_mode_set(bcmx_lport_t port, int mode)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_dscp_map_mode_set(bcm_unit, bcm_port,
                                      mode);
}

int
bcmx_port_dscp_map_mode_get(bcmx_lport_t port, int *mode)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mode);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_dscp_map_mode_get(bcm_unit, bcm_port,
                                      mode);
}

int
bcmx_port_dscp_map_set(bcmx_lport_t port,
                       int srccp,
                       int mapcp,
                       int prio)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;

    BCMX_PORT_INIT_CHECK;

    if (port == BCMX_LPORT_ETHER_ALL) {
        int i, bcm_unit;
        int bcm_port_all = -1; /* all ether ports in BCM */

        BCMX_UNIT_ITER(bcm_unit, i) {
            tmp_rv = bcm_port_dscp_map_set(bcm_unit, bcm_port_all, 
                                           srccp, mapcp, prio);
            BCM_IF_ERROR_RETURN
                (BCMX_PORT_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
        }
    } else {
        int         bcm_unit;
        bcm_port_t  bcm_port;

        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));

        rv = bcm_port_dscp_map_set(bcm_unit, bcm_port,
                                   srccp, mapcp, prio);
    }

    return rv;
}


int
bcmx_port_dscp_map_get(bcmx_lport_t port,
                       int srccp,
                       int *mapcp,
                       int *prio)
{
    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mapcp);
    BCMX_PARAM_NULL_CHECK(prio);

    if (port == BCMX_LPORT_ETHER_ALL) {
        int rv, i, bcm_unit;
        int bcm_port_all = -1; /* all ether ports in BCM */

        BCMX_UNIT_ITER(bcm_unit, i) {
            rv = bcm_port_dscp_map_get(bcm_unit, bcm_port_all, 
                                       srccp, mapcp, prio);
            if (BCMX_PORT_GET_IS_VALID(bcm_unit, rv)) {
                return rv;
            }
        }    
    } else {
        int         bcm_unit;
        bcm_port_t  bcm_port;

        BCM_IF_ERROR_RETURN
            (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                     BCMX_DEST_CONVERT_DEFAULT));

        return bcm_port_dscp_map_get(bcm_unit, bcm_port,
                                     srccp, mapcp, prio);
    }

    return BCM_E_UNAVAIL;
}


int
bcmx_port_linkscan_get(bcmx_lport_t port, int *linkscan)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(linkscan);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_linkscan_get(bcm_unit, bcm_port,
                                 linkscan);
}


int
bcmx_port_linkscan_set(bcmx_lport_t port, int linkscan)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_linkscan_set(bcm_unit, bcm_port,
                                 linkscan);
}


int
bcmx_port_autoneg_get(bcmx_lport_t port, int *autoneg)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(autoneg);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_autoneg_get(bcm_unit, bcm_port,
                                autoneg);
}


int
bcmx_port_autoneg_set(bcmx_lport_t port, int autoneg)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_autoneg_set(bcm_unit, bcm_port,
                                autoneg);
}


int
bcmx_port_speed_max(bcmx_lport_t port, int *speed)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(speed);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_speed_max(bcm_unit, bcm_port,
                              speed);
}


int
bcmx_port_speed_get(bcmx_lport_t port, int *speed)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(speed);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_speed_get(bcm_unit, bcm_port,
                              speed);
}


int
bcmx_port_speed_set(bcmx_lport_t port, int speed)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_speed_set(bcm_unit, bcm_port,
                              speed);
}


int
bcmx_port_master_get(bcmx_lport_t port, int *ms)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(ms);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_master_get(bcm_unit, bcm_port,
                               ms);
}


int
bcmx_port_master_set(bcmx_lport_t port, int ms)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_master_set(bcm_unit, bcm_port,
                               ms);
}


int
bcmx_port_interface_get(bcmx_lport_t port, bcm_port_if_t *intf)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(intf);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_interface_get(bcm_unit, bcm_port,
                                  intf);
}


int
bcmx_port_interface_set(bcmx_lport_t port, bcm_port_if_t intf)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_interface_set(bcm_unit, bcm_port,
                                  intf);
}


int
bcmx_port_duplex_get(bcmx_lport_t port, int *duplex)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_duplex_get(bcm_unit, bcm_port,
                               duplex);
}


int
bcmx_port_duplex_set(bcmx_lport_t port, int duplex)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_duplex_set(bcm_unit, bcm_port,
                               duplex);
}


int
bcmx_port_pause_get(bcmx_lport_t port,
                    int *pause_tx,
                    int *pause_rx)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(pause_tx);
    BCMX_PARAM_NULL_CHECK(pause_rx);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_pause_get(bcm_unit, bcm_port,
                              pause_tx, pause_rx);
}


int
bcmx_port_pause_set(bcmx_lport_t port,
                    int pause_tx,
                    int pause_rx)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_pause_set(bcm_unit, bcm_port,
                              pause_tx, pause_rx);
}


int
bcmx_port_pause_addr_get(bcmx_lport_t port, bcm_mac_t mac)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_pause_addr_get(bcm_unit, bcm_port,
                                   mac);
}


int
bcmx_port_pause_addr_set(bcmx_lport_t port, bcm_mac_t mac)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_pause_addr_set(bcm_unit, bcm_port,
                                   mac);
}


int
bcmx_port_pause_sym_set(bcmx_lport_t port, int pause)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_pause_sym_set(bcm_unit, bcm_port,
                                  pause);
}


int
bcmx_port_pause_sym_get(bcmx_lport_t port, int *pause)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(pause);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_pause_sym_get(bcm_unit, bcm_port,
                                  pause);
}


int
bcmx_port_update(bcmx_lport_t port, int link)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_update(bcm_unit, bcm_port,
                           link);
}


int
bcmx_port_frame_max_set(bcmx_lport_t port, int size)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_frame_max_set(bcm_unit, bcm_port,
                                  size);
}

int
bcmx_port_frame_max_get(bcmx_lport_t port, int *size)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(size);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_frame_max_get(bcm_unit, bcm_port,
                                  size);
}

int
bcmx_port_l3_mtu_set(bcmx_lport_t port, int size)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_l3_mtu_set(bcm_unit, bcm_port,
                               size);
}

int
bcmx_port_l3_mtu_get(bcmx_lport_t port, int *size)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(size);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_l3_mtu_get(bcm_unit, bcm_port,
                               size);
}

int
bcmx_port_jam_set(bcmx_lport_t port, int enable)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_jam_set(bcm_unit, bcm_port,
                            enable);
}

int
bcmx_port_jam_get(bcmx_lport_t port, int *enable)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(enable);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_jam_get(bcm_unit, bcm_port,
                            enable);
}

int
bcmx_port_ifg_set(bcmx_lport_t port,
                  int speed,
                  bcm_port_duplex_t duplex,
                  int bit_times)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_ifg_set(bcm_unit, bcm_port,
                            speed, duplex, bit_times);
}

int
bcmx_port_ifg_get(bcmx_lport_t port,
                  int speed,
                  bcm_port_duplex_t duplex,
                  int *bit_times)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(bit_times);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_ifg_get(bcm_unit, bcm_port,
                            speed, duplex, bit_times);
}

int
bcmx_port_phy_get(bcmx_lport_t port,
                  uint32 flags,
                  uint32 phy_reg_addr,
                  uint32 *phy_data)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(phy_data);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_phy_get(bcm_unit, bcm_port,
                            flags, phy_reg_addr, phy_data);
}

int
bcmx_port_phy_set(bcmx_lport_t port,
                  uint32 flags,
                  uint32 phy_reg_addr,
                  uint32 phy_data)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_phy_set(bcm_unit, bcm_port,
                            flags, phy_reg_addr, phy_data);
}

int
bcmx_port_phy_modify(bcmx_lport_t port,
                     uint32 flags,
                     uint32 phy_reg_addr,
                     uint32 phy_data,
                     uint32 phy_mask)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_phy_modify(bcm_unit, bcm_port,
                               flags, phy_reg_addr, phy_data, phy_mask);
}

int
bcmx_port_mdix_set(bcmx_lport_t port, bcm_port_mdix_t mode)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_mdix_set(bcm_unit, bcm_port,
                             mode);
}

int
bcmx_port_mdix_get(bcmx_lport_t port, bcm_port_mdix_t *mode)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mode);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_mdix_get(bcm_unit, bcm_port,
                             mode);
}

int
bcmx_port_mdix_status_get(bcmx_lport_t port,
                          bcm_port_mdix_status_t *status)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(status);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_mdix_status_get(bcm_unit, bcm_port,
                                    status);
}

int
bcmx_port_medium_config_set(bcmx_lport_t port,
                            bcm_port_medium_t medium,
                            bcm_phy_config_t *config)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(config);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_medium_config_set(bcm_unit, bcm_port,
                                      medium, config);
}

int
bcmx_port_medium_config_get(bcmx_lport_t port,
                            bcm_port_medium_t medium,
                            bcm_phy_config_t *config)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(config);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_medium_config_get(bcm_unit, bcm_port,
                                      medium, config);
}

int
bcmx_port_medium_get(bcmx_lport_t port, bcm_port_medium_t *medium)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(medium);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_medium_get(bcm_unit, bcm_port,
                               medium);
}

int
bcmx_port_loopback_set(bcmx_lport_t port, int loopback)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_loopback_set(bcm_unit, bcm_port,
                                 loopback);
}


int
bcmx_port_loopback_get(bcmx_lport_t port, int *loopback)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(loopback);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_loopback_get(bcm_unit, bcm_port,
                                 loopback);
}


int
bcmx_port_stp_get(bcmx_lport_t port, int *state)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(state);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_stp_get(bcm_unit, bcm_port,
                            state);
}


int
bcmx_port_stp_set(bcmx_lport_t port, int state)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_stp_set(bcm_unit, bcm_port,
                            state);
}


int
bcmx_port_discard_get(bcmx_lport_t port, int *mode)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mode);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_discard_get(bcm_unit, bcm_port,
                                mode);
}


int
bcmx_port_discard_set(bcmx_lport_t port, int mode)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_discard_set(bcm_unit, bcm_port,
                                mode);
}

/*
 * bcmx_port_learn_get/set()
 * Notes:
 *     Supports physical and virtual ports.
 *     If 'port' is a physical port, apply to device where port resides.
 *     If 'port' is a virtual port, apply to all devices.
 */
int
bcmx_port_learn_get(bcmx_lport_t port, uint32 *flags)
{
    int         rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(flags);

    if (!BCM_GPORT_IS_SET(port)) {
        return BCM_E_PORT;
    }

    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_port_learn_get(bcm_unit, bcm_port, flags);
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_port_learn_get(bcm_unit, port, flags);
        if (BCMX_PORT_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

int
bcmx_port_learn_set(bcmx_lport_t port, uint32 flags)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    if (!BCM_GPORT_IS_SET(port)) {
        return BCM_E_PORT;
    }

    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_port_learn_set(bcm_unit, bcm_port, flags);
    }
   
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_port_learn_set(bcm_unit, port, flags);
        BCM_IF_ERROR_RETURN(BCMX_PORT_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


int
bcmx_port_learn_modify(bcmx_lport_t port, uint32 add, uint32 remove)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_learn_modify(bcm_unit, bcm_port,
                                 add, remove);
}


int
bcmx_port_link_status_get(bcmx_lport_t port, int *up)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(up);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_link_status_get(bcm_unit, bcm_port,
                                    up);
}


int
bcmx_port_link_failed_clear(bcm_gport_t port)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_link_failed_clear(bcm_unit, bcm_port);
}


int
bcmx_port_ifilter_get(bcmx_lport_t port, int *mode)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mode);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_ifilter_get(bcm_unit, bcm_port,
                                mode);
}


int
bcmx_port_ifilter_set(bcmx_lport_t port, int mode)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_ifilter_set(bcm_unit, bcm_port,
                                mode);
}


/*
 * bcmx_port_vlan_member_get/set()
 * Notes:
 *     Supports physical and virtual ports.
 *     If 'port' is a physical port, apply to device where port resides.
 *     If 'port' is a virtual port, apply to all devices.
 */
int
bcmx_port_vlan_member_get(bcmx_lport_t port, uint32 *flags)
{
    int         rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(flags);

    if (!BCM_GPORT_IS_SET(port)) {
        return BCM_E_PORT;
    }

    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_port_vlan_member_get(bcm_unit, bcm_port, flags);
    }

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_port_vlan_member_get(bcm_unit, port, flags);
        if (BCMX_PORT_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}

int 
bcmx_port_vlan_member_set(bcmx_lport_t port, uint32 flags)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         i, bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    if (!BCM_GPORT_IS_SET(port)) {
        return BCM_E_PORT;
    }

    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_port_vlan_member_set(bcm_unit, bcm_port, flags);
    }
   
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv =  bcm_port_vlan_member_set(bcm_unit, port, flags);
        BCM_IF_ERROR_RETURN(BCMX_PORT_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}


int
bcmx_port_bpdu_enable_set(bcmx_lport_t port, int enable)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_bpdu_enable_set(bcm_unit, bcm_port,
                                    enable);
}


int
bcmx_port_bpdu_enable_get(bcmx_lport_t port, int *enable)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(enable);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_bpdu_enable_get(bcm_unit, bcm_port,
                                    enable);
}


int
bcmx_port_l3_enable_set(bcmx_lport_t port, int enable)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_l3_enable_set(bcm_unit, bcm_port,
                                  enable);
}


int
bcmx_port_l3_enable_get(bcmx_lport_t port, int *enable)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(enable);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_l3_enable_get(bcm_unit, bcm_port,
                                  enable);
}


int
bcmx_port_tgid_set(bcmx_lport_t port, int tid, int psc)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_tgid_set(bcm_unit, bcm_port,
                             tid, psc);
}

int
bcmx_port_tgid_get(bcmx_lport_t port, int *tid, int *psc)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(tid);
    BCMX_PARAM_NULL_CHECK(psc);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_tgid_get(bcm_unit, bcm_port,
                             tid, psc);
}

int
bcmx_port_pfm_set(bcmx_lport_t port, int mode)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_pfm_set(bcm_unit, bcm_port,
                            mode);
}


int
bcmx_port_pfm_get(bcmx_lport_t port, int *mode)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mode);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_pfm_get(bcm_unit, bcm_port,
                            mode);
}


int
bcmx_port_encap_set(bcmx_lport_t port, int mode)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    BCM_IF_ERROR_RETURN
        (bcm_port_encap_set(bcm_unit, bcm_port,
                            mode));

    /* Update BCMX internal port flag state */
    return _bcmx_port_changed(port);
}


int
bcmx_port_encap_get(bcmx_lport_t port, int *mode)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mode);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_encap_get(bcm_unit, bcm_port,
                              mode);
}


int
bcmx_port_encap_config_set(bcm_gport_t gport, 
                           bcmx_port_encap_config_t *encap_config)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(encap_config);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(gport, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_encap_config_set(bcm_unit, gport,
                                     BCMX_PORT_ENCAP_CONFIG_T_PTR_TO_BCM
                                     (encap_config));
}


int
bcmx_port_encap_config_get(bcm_gport_t gport, 
                           bcmx_port_encap_config_t *encap_config)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(encap_config);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(gport, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_encap_config_get(bcm_unit, gport,
                                     BCMX_PORT_ENCAP_CONFIG_T_PTR_TO_BCM
                                     (encap_config));
}


int
bcmx_port_queued_count_get(bcmx_lport_t port, uint32 *count)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(count);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_queued_count_get(bcm_unit, bcm_port,
                                     count);
}


int
bcmx_port_protocol_vlan_add(bcmx_lport_t port,
                            bcm_port_frametype_t frame,
                            bcm_port_ethertype_t ether,
                            bcm_vlan_t vid)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_protocol_vlan_add(bcm_unit, bcm_port,
                                      frame, ether, vid);
}


int
bcmx_port_protocol_vlan_delete(bcmx_lport_t port,
                               bcm_port_frametype_t frame,
                               bcm_port_ethertype_t ether)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_protocol_vlan_delete(bcm_unit, bcm_port,
                                         frame, ether);
}


int
bcmx_port_protocol_vlan_delete_all(bcmx_lport_t port)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_protocol_vlan_delete_all(bcm_unit, bcm_port);
}


/*
 * Enable traffic from source port "port" to destination ports in
 * the lplist.  Ports not in the lplist will be blocked.
 *
 * The use of "modid" in this routine is deprecated.  That parameter
 * is ignored.
 */

int
bcmx_port_egress_set(bcmx_lport_t port,
                     int modid,
                     bcmx_lplist_t lplist)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int dest_unit;
    bcm_pbmp_t pbmp;
    bcm_module_t src_mod;
    bcm_port_t src_port;
    int i;

    COMPILER_REFERENCE(modid);

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_modid_port(port, &src_mod, &src_port,
                                  BCMX_DEST_CONVERT_DEFAULT));

    /*
     * Iterate over units; for each unit, extract bitmap from
     * lplist.  Call bcm_port_egress_set with that bitmap using
     * the (port, mod-id) derived from the (source) port.
     *
     * If an error is returned on non-NULL bitmap, return that error 
     */
    BCMX_UNIT_ITER(dest_unit, i) {
        BCMX_LPLIST_TO_PBMP(lplist, dest_unit, pbmp);
        tmp_rv = bcm_port_egress_set(dest_unit, src_port, (int)src_mod, pbmp);
        BCM_IF_ERROR_RETURN(BCMX_PORT_SET_ERROR_CHECK(dest_unit, tmp_rv, &rv));
    }

    return rv;
}


/*
 * Get the current settings for enabled traffic from source port "port"
 * to other ports in the system.
 *
 * The use of "modid" in this routine is deprecated.  That parameter
 * is ignored.
 */

int
bcmx_port_egress_get(bcmx_lport_t port,
                     int modid,
                     bcmx_lplist_t *lplist)
{
    int rv = BCM_E_UNAVAIL, tmp_rv;
    int dest_unit;
    bcm_pbmp_t pbmp;
    bcm_module_t src_mod;
    bcm_port_t src_port;
    int i;

    COMPILER_REFERENCE(modid);

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(lplist);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_modid_port(port, &src_mod, &src_port,
                                  BCMX_DEST_CONVERT_DEFAULT));

    BCM_IF_ERROR_RETURN(bcmx_lplist_clear(lplist));

    BCMX_UNIT_ITER(dest_unit, i) {
        tmp_rv = bcm_port_egress_get(dest_unit, src_port,
                                 (int)src_mod, &pbmp);
        if (BCMX_PORT_GET_IS_VALID(dest_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                rv = BCMX_LPLIST_PBMP_ADD(lplist, dest_unit, pbmp);
            } else {
                break;
            }
        }
    }

    return rv;
}


int
bcmx_port_modid_enable_set(bcmx_lport_t port,
                           int modid,
                           int enable)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_modid_enable_set(bcm_unit, bcm_port,
                                     modid, enable);
}


int
bcmx_port_modid_enable_get(bcmx_lport_t port,
                           int modid,
                           int *enable)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(enable);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_modid_enable_get(bcm_unit, bcm_port,
                                     modid, enable);
}


int
bcmx_port_flood_block_set(bcmx_lport_t ingress_port,
                          bcmx_lport_t egress_port,
                          uint32 flags)
{
    int         ingress_bcm_unit, egress_bcm_unit;
    bcm_port_t  ingress_bcm_port, egress_bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(ingress_port,
                                 &ingress_bcm_unit, &ingress_bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(egress_port,
                                 &egress_bcm_unit, &egress_bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    if (ingress_bcm_unit != egress_bcm_unit) {
        return BCM_E_PARAM;
    }

    return bcm_port_flood_block_set(ingress_bcm_unit,
                                    ingress_bcm_port,
                                    egress_bcm_port,
                                    flags);
}


int
bcmx_port_flood_block_get(bcmx_lport_t ingress_port,
                          bcmx_lport_t egress_port,
                          uint32 *flags)
{
    int         ingress_bcm_unit, egress_bcm_unit;
    bcm_port_t  ingress_bcm_port, egress_bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(flags);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(ingress_port,
                                 &ingress_bcm_unit, &ingress_bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(egress_port,
                                 &egress_bcm_unit, &egress_bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    if (ingress_bcm_unit != egress_bcm_unit) {
        return BCM_E_PARAM;
    }

    return bcm_port_flood_block_get(ingress_bcm_unit,
                                    ingress_bcm_port,
                                    egress_bcm_port,
                                    flags);
}


int
bcmx_port_rate_egress_set(bcmx_lport_t port,
                          uint32 kbits_sec,
                          uint32 kbits_burst)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_rate_egress_set(bcm_unit, bcm_port,
                                    kbits_sec, kbits_burst);
}


int
bcmx_port_rate_egress_get(bcmx_lport_t port,
                          uint32 *kbits_sec,
                          uint32 *kbits_burst)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(kbits_sec);
    BCMX_PARAM_NULL_CHECK(kbits_burst);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_rate_egress_get(bcm_unit, bcm_port,
                                    kbits_sec, kbits_burst);
}


int
bcmx_port_rate_ingress_set(bcmx_lport_t port,
                           uint32 kbits_sec,
                           uint32 kbits_burst)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_rate_ingress_set(bcm_unit, bcm_port,
                                     kbits_sec, kbits_burst);
}


int
bcmx_port_rate_ingress_get(bcmx_lport_t port,
                           uint32 *kbits_sec,
                           uint32 *kbits_burst)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(kbits_sec);
    BCMX_PARAM_NULL_CHECK(kbits_burst);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_rate_ingress_get(bcm_unit, bcm_port,
                                     kbits_sec, kbits_burst);
}


int
bcmx_port_rate_pause_set(bcmx_lport_t port,
                         uint32 kbits_pause,
                         uint32 kbits_resume)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_rate_pause_set(bcm_unit, bcm_port,
                                   kbits_pause, kbits_resume);
}


int
bcmx_port_rate_pause_get(bcmx_lport_t port,
                         uint32 *kbits_pause,
                         uint32 *kbits_resume)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(kbits_pause);
    BCMX_PARAM_NULL_CHECK(kbits_resume);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_rate_pause_get(bcm_unit, bcm_port,
                                   kbits_pause, kbits_resume);
}


int
bcmx_port_sample_rate_set(bcmx_lport_t port,
                          int ingress_rate,
                          int egress_rate)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_sample_rate_set(bcm_unit, bcm_port,
                                    ingress_rate, egress_rate);
}


int
bcmx_port_sample_rate_get(bcmx_lport_t port,
                          int *ingress_rate,
                          int *egress_rate)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(ingress_rate);
    BCMX_PARAM_NULL_CHECK(egress_rate);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_sample_rate_get(bcm_unit, bcm_port,
                                    ingress_rate, egress_rate);
}


int
bcmx_port_dtag_mode_set(bcmx_lport_t port, int mode)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_dtag_mode_set(bcm_unit, bcm_port,
                                  mode);
}


int
bcmx_port_dtag_mode_get(bcmx_lport_t port, int *mode)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(mode);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_dtag_mode_get(bcm_unit, bcm_port,
                                  mode);
}


int
bcmx_port_tpid_set(bcmx_lport_t port, uint16 tpid)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_tpid_set(bcm_unit, bcm_port,
                             tpid);
}


int
bcmx_port_tpid_get(bcmx_lport_t port, uint16 *tpid)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(tpid);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_tpid_get(bcm_unit, bcm_port,
                             tpid);
}

int
bcmx_port_tpid_add(bcmx_lport_t port, uint16 tpid, int color_select)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_tpid_add(bcm_unit, bcm_port,
                             tpid, color_select); 
}

int
bcmx_port_tpid_delete(bcmx_lport_t port, uint16 tpid)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_tpid_delete(bcm_unit, bcm_port,
                                tpid);
}

int
bcmx_port_tpid_delete_all(bcmx_lport_t port)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_tpid_delete_all(bcm_unit, bcm_port);
}

int
bcmx_port_inner_tpid_set(bcmx_lport_t port, uint16 tpid)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_inner_tpid_set(bcm_unit, bcm_port,
                                   tpid);
}

int
bcmx_port_inner_tpid_get(bcmx_lport_t port, uint16 *tpid)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(tpid);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_inner_tpid_get(bcm_unit, bcm_port,
                                   tpid);
}

int
bcmx_port_cable_diag(bcmx_lport_t port, bcm_port_cable_diag_t *status)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(status);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_cable_diag(bcm_unit, bcm_port,
                               status);
}

int
bcmx_port_info_get(bcmx_lport_t port, bcm_port_info_t *info)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_info_get(bcm_unit, bcm_port,
                             info);
}


int
bcmx_port_info_set(bcmx_lport_t port, bcm_port_info_t *info)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_info_set(bcm_unit, bcm_port,
                             info);
}


int
bcmx_port_selective_get(bcmx_lport_t port, bcm_port_info_t *info)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_selective_get(bcm_unit, bcm_port,
                                  info);
}


int
bcmx_port_selective_set(bcmx_lport_t port, bcm_port_info_t *info)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_selective_set(bcm_unit, bcm_port,
                                  info);
}


int
bcmx_port_info_restore(bcmx_lport_t port, bcm_port_info_t *info)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_info_restore(bcm_unit, bcm_port,
                                 info);
}


int
bcmx_port_info_save(bcmx_lport_t port, bcm_port_info_t *info)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(info);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_info_save(bcm_unit, bcm_port,
                              info);
}


int
bcmx_port_fault_get(bcmx_lport_t port, uint32 *flags)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(flags);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_fault_get(bcm_unit, bcm_port,
                              flags);
}

int
bcmx_port_priority_color_set(bcmx_lport_t port, int prio, bcm_color_t color)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_priority_color_set(bcm_unit, bcm_port,
                                       prio, color);
}


int
bcmx_port_priority_color_get(bcmx_lport_t port, int prio, bcm_color_t *color)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(color);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_priority_color_get(bcm_unit, bcm_port,
                                       prio, color);
}

int
bcmx_port_cfi_color_set(bcmx_lport_t port, int cfi, bcm_color_t color)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_cfi_color_set(bcm_unit, bcm_port,
                                  cfi, color);
}


int
bcmx_port_cfi_color_get(bcmx_lport_t port, int cfi, bcm_color_t *color)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(color);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_cfi_color_get(bcm_unit, bcm_port,
                                  cfi, color);
}

int
bcmx_port_vlan_priority_map_set(bcmx_lport_t port,
                                int pkt_pri, int cfi,
                                int internal_pri, bcm_color_t color)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_vlan_priority_map_set(bcm_unit, bcm_port,
                                          pkt_pri, cfi,
                                          internal_pri, color);
}

int
bcmx_port_vlan_priority_map_get(bcmx_lport_t port,
                                int pkt_pri, int cfi,
                                int *internal_pri, bcm_color_t *color)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(internal_pri);
    BCMX_PARAM_NULL_CHECK(color);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_vlan_priority_map_get(bcm_unit, bcm_port,
                                          pkt_pri, cfi,
                                          internal_pri, color);
}

int
bcmx_port_vlan_priority_unmap_set(bcmx_lport_t port,
                                  int internal_pri, bcm_color_t color,
                                  int pkt_pri, int cfi)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_vlan_priority_unmap_set(bcm_unit, bcm_port,
                                            internal_pri, color,
                                            pkt_pri, cfi);
}

int
bcmx_port_vlan_priority_unmap_get(bcmx_lport_t port,
                                  int internal_pri, bcm_color_t color,
                                  int *pkt_pri, int *cfi)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(pkt_pri);
    BCMX_PARAM_NULL_CHECK(cfi);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_vlan_priority_unmap_get(bcm_unit, bcm_port,
                                            internal_pri, color,
                                            pkt_pri, cfi);
}

int
bcmx_port_vlan_inner_tag_set(bcmx_lport_t port, uint16 inner_tag)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_vlan_inner_tag_set(bcm_unit, bcm_port,
                                       inner_tag);
}

int
bcmx_port_vlan_inner_tag_get(bcmx_lport_t port, uint16 *inner_tag)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(inner_tag);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_vlan_inner_tag_get(bcm_unit, bcm_port,
                                       inner_tag);
}

int
bcmx_port_class_set(bcmx_lport_t port, bcm_port_class_t pclass, uint32 pclass_id)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_class_set(bcm_unit, bcm_port,
                              pclass,
                              pclass_id);
}

int
bcmx_port_class_get(bcmx_lport_t port, bcm_port_class_t pclass, uint32 *pclass_id)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(pclass_id);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_class_get(bcm_unit, bcm_port,
                              pclass,
                              pclass_id);
}


int 
bcmx_port_l3_modify_set(bcmx_lport_t port, uint32 flags)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_l3_modify_set(bcm_unit, bcm_port,
                                  flags);
}

int 
bcmx_port_l3_modify_get(bcmx_lport_t port, uint32 *flags)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(flags);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_l3_modify_get(bcm_unit, bcm_port,
                                  flags);
}

int 
bcmx_port_ipmc_modify_set(bcmx_lport_t port, uint32 flags)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_ipmc_modify_set(bcm_unit, bcm_port,
                                    flags);
}

int 
bcmx_port_ipmc_modify_get(bcmx_lport_t port, uint32 *flags)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(flags);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_ipmc_modify_get(bcm_unit, bcm_port,
                                    flags);
}

int
bcmx_port_phy_control_set(bcmx_lport_t port,
                          bcm_port_phy_control_t type, uint32 value)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_phy_control_set(bcm_unit, bcm_port,
                                    type, value);
}

int
bcmx_port_phy_control_get(bcmx_lport_t port,
                          bcm_port_phy_control_t type, uint32 *value)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(value);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return  bcm_port_phy_control_get(bcm_unit, bcm_port,
                                     type, value);
}

int
bcmx_port_force_vlan_set(bcmx_lport_t port,
                         bcm_vlan_t vlan, int pkt_prio, uint32 flags)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_force_vlan_set(bcm_unit, bcm_port,
                                   vlan, pkt_prio, flags);
}

int
bcmx_port_force_vlan_get(bcmx_lport_t port,
                         bcm_vlan_t *vlan, int *pkt_prio, uint32 *flags)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(flags);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_force_vlan_get(bcm_unit, bcm_port,
                                   vlan, pkt_prio, flags);
}

int
bcmx_port_gport_get(bcmx_lport_t port, bcm_gport_t *gport)
{
    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(gport);

    BCMX_LPORT_VALID_CHECK(port);
    *gport = port;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmx_port_control_set(bcmx_lport_t port,
                              bcm_port_control_t type, int value)
 * Description:
 *      Enable/Disable specified port feature.
 * Parameters:
 *      port - lport to control
 *      type - Port control parameter
 *      value - new value of control parameter
 * Return Value:
 *      BCM_E_NONE
 *      BCM_E_UNAVAIL - Functionality not available
 */

int
bcmx_port_control_set(bcmx_lport_t port, bcm_port_control_t type, int arg)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_control_set(bcm_unit, bcm_port, type, arg);
}

/*
 * Function:
 *      bcmx_port_control_get(bcmx_lport_t port,
                              bcm_port_control_t type, int *value)
 * Description:
 *      Get the status of specified port feature.
 * Parameters:
 *      port - lport to control
 *      type - Port control parameter
 *      value - (OUT) Current value of control parameter
 * Return Value:
 *      BCM_E_NONE
 *      BCM_E_UNAVAIL - Functionality not available
 */

int
bcmx_port_control_get(bcmx_lport_t port, bcm_port_control_t type, int *arg)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(arg);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_control_get(bcm_unit, bcm_port, type, arg);
}


/*
 * Function:
 *      bcmx_port_stat_enable_set
 * Purpose:
 *      Enable/disable packet and byte counters for the selected
 *      gport.
 * Parameters:
 *      port   - GPORT ID, physical (lport) or virtual port
 *      enable - Non-zero to enable counter collection, zero to disable
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_port_stat_enable_set(bcm_gport_t port, int enable)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         bcm_unit;
    bcm_port_t  bcm_port;
    int         i;

    BCMX_PORT_INIT_CHECK;

    if (!BCM_GPORT_IS_SET(port)) {
        return BCM_E_PORT;
    }

    /* Physical port, apply to unit where port resides */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_port_stat_enable_set(bcm_unit, port, enable);
    }

    /* Virtual port, enable/disable stat in all units */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_port_stat_enable_set(bcm_unit, port, enable);
        BCM_IF_ERROR_RETURN(BCMX_PORT_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_port_stat_get
 * Purpose:
 *      Get 64-bit counter value for specified port statistic type.
 * Parameters:
 *      port - GPORT ID, physical (lport) or virtual port
 *      stat - Type of the counter to retrieve
 *      val  - (OUT) Pointer to a counter value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_port_stat_get(bcm_gport_t port, bcm_port_stat_t stat, uint64 *val)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         bcm_unit;
    bcm_port_t  bcm_port;
    uint64      tmp_val;
    int         i;

    BCMX_PORT_INIT_CHECK;

    if (!BCM_GPORT_IS_SET(port)) {
        return BCM_E_PORT;
    }
    BCMX_PARAM_NULL_CHECK(val);

    /* Physical port, get stat from unit where port resides */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_port_stat_get(bcm_unit, port, stat, val);
    }

    /* Virtual port, gather stats from all units */
    COMPILER_64_ZERO(*val);
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_port_stat_get(bcm_unit, port, stat, &tmp_val);
        if (BCMX_PORT_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                COMPILER_64_ADD_64(*val, tmp_val);
            } else {
                break;
            }
        }
    }

    return rv;
}

/*
 * Function:
 *      bcmx_port_stat_get32
 * Purpose:
 *      Get lower 32-bit counter value for specified port statistic
 *      type.
 * Parameters:
 *      port - GPORT ID, physical (lport) or virtual port
 *      stat - Type of the counter to retrieve
 *      val  - (OUT) Pointer to a counter value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_port_stat_get32(bcm_gport_t port, bcm_port_stat_t stat, uint32 *val)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         bcm_unit;
    bcm_port_t  bcm_port;
    uint32      tmp_val;
    int         i;

    BCMX_PORT_INIT_CHECK;

    if (!BCM_GPORT_IS_SET(port)) {
        return BCM_E_PORT;
    }
    BCMX_PARAM_NULL_CHECK(val);

    /* Physical port, get stat from unit where port resides */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_port_stat_get32(bcm_unit, port, stat, val);
    }

    /* Virtual port, gather stats from all units */
    *val = 0;
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_port_stat_get32(bcm_unit, port, stat, &tmp_val);
        if (BCMX_PORT_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                *val += tmp_val;
            } else {
                break;
            }
        }
    }

    return rv;
}

/*
 * Function:
 *      bcmx_port_stat_set
 * Purpose:
 *      Set 64-bit counter value for specified port statistic type.
 * Parameters:
 *      port - GPORT ID, physical (lport) or virtual port
 *      stat - Type of the counter to set
 *      val  - New counter value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_port_stat_set(bcm_gport_t port, bcm_port_stat_t stat, uint64 val)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         bcm_unit;
    bcm_port_t  bcm_port;
    int         i;

    BCMX_PORT_INIT_CHECK;

    if (!BCM_GPORT_IS_SET(port)) {
        return BCM_E_PORT;
    }

    /* Physical port, set stat in unit where port resides */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_port_stat_set(bcm_unit, port, stat, val);
    }

    /* Virtual port, set stat in all units */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_port_stat_set(bcm_unit, port, stat, val);
        BCM_IF_ERROR_RETURN(BCMX_PORT_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_port_stat_set32
 * Purpose:
 *      Set lower 32-bit counter value for specified port statistic
 *      type.
 * Parameters:
 *      port - GPORT ID, physical (lport) or virtual port
 *      stat - Type of the counter to set
 *      val  - New counter value
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_port_stat_set32(bcm_gport_t port, bcm_port_stat_t stat, uint32 val)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         bcm_unit;
    bcm_port_t  bcm_port;
    int         i;

    BCMX_PORT_INIT_CHECK;

    if (!BCM_GPORT_IS_SET(port)) {
        return BCM_E_PORT;
    }

    /* Physical port, set stat in unit where port resides */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_port_stat_set32(bcm_unit, port, stat, val);
    }

    /* Virtual port, set stat in all units */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_port_stat_set32(bcm_unit, port, stat, val);
        BCM_IF_ERROR_RETURN(BCMX_PORT_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_port_stat_multi_get
 * Purpose:
 *      Get 64-bit counter value for multiple port statistic types.
 * Parameters:
 *      port      - GPORT ID, physical (lport) or virtual port
 *      nstat     - Number of elements in stat array
 *      stat_arr  - Collected statistics descriptors array
 *      value_arr - (OUT) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_port_stat_multi_get(bcm_gport_t port, int nstat,
                         bcm_port_stat_t *stat_arr, uint64 *value_arr)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         bcm_unit;
    bcm_port_t  bcm_port;
    uint64      *tmp_val;
    int         i;

    BCMX_PORT_INIT_CHECK;

    if (!BCM_GPORT_IS_SET(port)) {
        return BCM_E_PORT;
    }
    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, stat_arr);
    BCMX_PARAM_ARRAY_NULL_CHECK(nstat, value_arr);

    /* Physical port, get stat from unit where port resides */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_port_stat_multi_get(bcm_unit, port, nstat,
                                       stat_arr, value_arr);
    }

    /* Virtual port, gather stats from all units */
    tmp_val = sal_alloc(sizeof(uint64) * nstat, "bcmx port stat");
    if (tmp_val == NULL) {
        return BCM_E_MEMORY;
    }

    for (i = 0; i < nstat; i++) {
        COMPILER_64_ZERO(value_arr[i]);
    }
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_port_stat_multi_get(bcm_unit, port, nstat,
                                         stat_arr, tmp_val);
        if (BCMX_PORT_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                for (i = 0; i < nstat; i++) {
                    COMPILER_64_ADD_64(value_arr[i], tmp_val[i]);
                }
            } else {
                break;
            }
        }
    }

    sal_free(tmp_val);

    return rv;
}

/*
 * Function:
 *      bcmx_port_stat_multi_get32
 * Purpose:
 *      Get lower 32-bit counter value for multiple port statistic
 *      types.
 * Parameters:
 *      port      - GPORT ID, physical (lport) or virtual port
 *      nstat     - Number of elements in stat array
 *      stat_arr  - Collected statistics descriptors array
 *      value_arr - (OUT) Collected counters values
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_port_stat_multi_get32(bcm_gport_t port, int nstat,
                           bcm_port_stat_t *stat_arr, uint32 *value_arr)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         bcm_unit;
    bcm_port_t  bcm_port;
    uint32      *tmp_val;
    int         i;

    BCMX_PORT_INIT_CHECK;

    if (!BCM_GPORT_IS_SET(port)) {
        return BCM_E_PORT;
    }
    BCMX_PARAM_NULL_CHECK(stat_arr);
    BCMX_PARAM_NULL_CHECK(value_arr);

    /* Physical port, get stat from unit where port resides */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_port_stat_multi_get32(bcm_unit, port, nstat,
                                         stat_arr, value_arr);
    }

    /* Virtual port, gather stats from all units */
    tmp_val = sal_alloc(sizeof(uint32) * nstat, "bcmx port stat");
    if (tmp_val == NULL) {
        return BCM_E_MEMORY;
    }

    for (i = 0; i < nstat; i++) {
        value_arr[i] = 0;
    }
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_port_stat_multi_get32(bcm_unit, port, nstat,
                                           stat_arr, tmp_val);
        if (BCMX_PORT_GET_IS_VALID(bcm_unit, tmp_rv)) {
            rv = tmp_rv;
            if (BCM_SUCCESS(tmp_rv)) {
                for (i = 0; i < nstat; i++) {
                    value_arr[i] += tmp_val[i];
                }
            } else {
                break;
            }
        }
    }

    sal_free(tmp_val);

    return rv;
}

/*
 * Function:
 *      bcmx_port_stat_multi_set
 * Purpose:
 *      Set 64-bit counter value for multiple port statistic types.
 * Parameters:
 *      port      - GPORT ID, physical (lport) or virtual port
 *      nstat     - Number of elements in stat array
 *      stat_arr  - New statistics descriptors array
 *      value_arr - New counters values to set
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_port_stat_multi_set(bcm_gport_t port, int nstat,
                         bcm_port_stat_t *stat_arr, uint64 *value_arr)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         bcm_unit;
    bcm_port_t  bcm_port;
    int         i;

    BCMX_PORT_INIT_CHECK;

    if (!BCM_GPORT_IS_SET(port)) {
        return BCM_E_PORT;
    }
    BCMX_PARAM_NULL_CHECK(stat_arr);
    BCMX_PARAM_NULL_CHECK(value_arr);

    /* Physical port, set stat in unit where port resides */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_port_stat_multi_set(bcm_unit, port, nstat,
                                       stat_arr, value_arr);
    }

    /* Virtual port, set stat in all units */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_port_stat_multi_set(bcm_unit, port, nstat,
                                         stat_arr, value_arr);
        BCM_IF_ERROR_RETURN(BCMX_PORT_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_port_stat_multi_set32
 * Purpose:
 *      Set lower 32-bit counter value for multiple port statistic
 *      types.
 * Parameters:
 *      port      - GPORT ID, physical (lport) or virtual port
 *      nstat     - Number of elements in stat array
 *      stat_arr  - New statistics descriptors array
 *      value_arr - New counters values to set
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_port_stat_multi_set32(bcm_gport_t port, int nstat,
                           bcm_port_stat_t *stat_arr, uint32 *value_arr)
{
    int         rv = BCM_E_UNAVAIL, tmp_rv;
    int         bcm_unit;
    bcm_port_t  bcm_port;
    int         i;

    BCMX_PORT_INIT_CHECK;

    if (!BCM_GPORT_IS_SET(port)) {
        return BCM_E_PORT;
    }
    BCMX_PARAM_NULL_CHECK(stat_arr);
    BCMX_PARAM_NULL_CHECK(value_arr);

    /* Physical port, set stat in unit where port resides */
    if (BCM_SUCCESS(_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                            BCMX_DEST_CONVERT_DEFAULT))) {
        return bcm_port_stat_multi_set32(bcm_unit, port, nstat,
                                         stat_arr, value_arr);
    }

    /* Virtual port, set stat in all units */
    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_port_stat_multi_set32(bcm_unit, port, nstat,
                                           stat_arr, value_arr);
        BCM_IF_ERROR_RETURN(BCMX_PORT_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *      bcmx_port_subsidiary_ports_get
 * Purpose:
 *      Given a controlling port, this API returns the set of ancillary ports
 *      belonging to the group (port block) that can be configured to operate
 *      either as a single high-speed port or multiple GE ports. If the input
 *      port is not a controlling port, an empty list is returned.
 * Parameters:
 *      port   - Controlling port
 *      lplist - (OUT) List of ports associated with the hot-swap group
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmx_port_subsidiary_ports_get(bcm_gport_t port, bcmx_lplist_t *lplist)
{
    int         rv;
    int         bcm_unit;
    bcm_port_t  bcm_port;
    bcm_pbmp_t  pbmp;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(lplist);

    BCM_IF_ERROR_RETURN(bcmx_lplist_clear(lplist));

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    rv = bcm_port_subsidiary_ports_get(bcm_unit, port, &pbmp);
    if (BCM_SUCCESS(rv)) {
        rv = BCMX_LPLIST_PBMP_ADD(lplist, bcm_unit, pbmp);
    }

    return rv;
}


/*
 * Function:
 *     bcmx_port_congestion_config_set
 * Purpose:
 *     Set end-to-end congestion control.
 * Parameters:
 *     port   - Logical port number
 *     config - Structure containing the congestion configuration
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_port_congestion_config_set(bcm_gport_t port, 
                                bcmx_port_congestion_config_t *config)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(config);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_congestion_config_set
        (bcm_unit, port, BCMX_PORT_CONGESTION_CONFIG_T_PTR_TO_BCM(config));
}

/*
 * Function:
 *     bcmx_port_congestion_config_get
 * Purpose:
 *     Get end-to-end congestion control configuration.
 * Parameters:
 *     port   - Logical port number
 *     config - (OUT) Structure containing the congestion configuration
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_port_congestion_config_get(bcm_gport_t port, 
                                bcmx_port_congestion_config_t *config)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(config);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_congestion_config_get
        (bcm_unit, port, BCMX_PORT_CONGESTION_CONFIG_T_PTR_TO_BCM(config));
}


/*
 * Function:
 *     bcmx_port_match_add
 * Purpose:
 *     Add a match to an existing port.
 * Parameters:
 *     port  - Gport
 *     match - Match criteria
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_port_match_add(bcm_gport_t port, bcmx_port_match_info_t *match)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(match);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_port_match_add
            (bcm_unit, port, BCMX_PORT_MATCH_INFO_T_PTR_TO_BCM(match));

        BCM_IF_ERROR_RETURN(BCMX_PORT_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_port_match_delete
 * Purpose:
 *     Remove a match from an existing port.
 * Parameters:
 *     port  - Gport
 *     match - Match criteria
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_port_match_delete(bcm_gport_t port, bcmx_port_match_info_t *match)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(match);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_port_match_delete
            (bcm_unit, port, BCMX_PORT_MATCH_INFO_T_PTR_TO_BCM(match));

        BCM_IF_ERROR_RETURN
            (BCMX_PORT_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_port_match_replace
 * Purpose:
 *     Replace an old match with a new one for an existing port.
 * Parameters:
 *     port      - Gport
 *     old_match - Old match criteria
 *     new_match - New match criteria
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_port_match_replace(bcm_gport_t port,
                        bcmx_port_match_info_t *old_match, 
                        bcmx_port_match_info_t *new_match)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(old_match);
    BCMX_PARAM_NULL_CHECK(new_match);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_port_match_replace
            (bcm_unit, port,
             BCMX_PORT_MATCH_INFO_T_PTR_TO_BCM(old_match),
             BCMX_PORT_MATCH_INFO_T_PTR_TO_BCM(new_match));

        BCM_IF_ERROR_RETURN(BCMX_PORT_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_port_match_multi_get
 * Purpose:
 *     Replace an old match with a new one for an existing port.
 * Parameters:
 *     port        - Gport
 *     size        - Array size
 *     match_array - (OUT) Match array
 *     count       - (OUT) Number of match entries returned in array
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_port_match_multi_get(bcm_gport_t port, int size, 
                          bcmx_port_match_info_t *match_array, 
                          int *count)
{
    int  rv;
    int  i, bcm_unit;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_ARRAY_NULL_CHECK(size, match_array);
    BCMX_PARAM_NULL_CHECK(count);

    BCMX_UNIT_ITER(bcm_unit, i) {
        rv = bcm_port_match_multi_get
            (bcm_unit, port, size,
             BCMX_PORT_MATCH_INFO_T_PTR_TO_BCM(match_array), count);

        if (BCMX_PORT_GET_IS_VALID(bcm_unit, rv)) {
            return rv;
        }
    }

    return BCM_E_UNAVAIL;
}
    
/*
 * Function:
 *     bcmx_port_match_set
 * Purpose:
 *     Assign a set of matches to an existing port.
 * Parameters:
 *     port        - Gport
 *     size        - Number of entries in match array
 *     match_array - Match array
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_port_match_set(bcm_gport_t port, int size,
                    bcmx_port_match_info_t *match_array)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(match_array);

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_port_match_set
            (bcm_unit, port, size,
             BCMX_PORT_MATCH_INFO_T_PTR_TO_BCM(match_array));

        BCM_IF_ERROR_RETURN(BCMX_PORT_SET_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_port_match_delete_all
 * Purpose:
 *     Delete all matches for an existing port.
 * Parameters:
 *     port - Gport
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_port_match_delete_all(bcm_gport_t port)
{
    int  rv = BCM_E_UNAVAIL, tmp_rv;
    int  i, bcm_unit;

    BCMX_PORT_INIT_CHECK;

    BCMX_UNIT_ITER(bcm_unit, i) {
        tmp_rv = bcm_port_match_delete_all(bcm_unit, port);
        BCM_IF_ERROR_RETURN
            (BCMX_PORT_DELETE_ERROR_CHECK(bcm_unit, tmp_rv, &rv));
    }

    return rv;
}

/*
 * Function:
 *     bcmx_port_phy_timesync_config_set
 * Purpose:
 *     Set phy_timesync configuration for port.
 * Parameters:
 *     port - Port
 *     conf - Timesync configuration
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_port_phy_timesync_config_set(bcm_gport_t port, 
                              bcmx_port_phy_timesync_config_t *conf)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(conf);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_phy_timesync_config_set(bcm_unit, port,
                                        BCMX_PORT_TIMESYNC_CONFIG_T_PTR_TO_BCM
                                        (conf));
}

/*
 * Function:
 *     bcmx_port_phy_timesync_config_get
 * Purpose:
 *     Get phy_timesync configuration for port.
 * Parameters:
 *     port - Port
 *     conf - (OUT) Timesync configuration
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_port_phy_timesync_config_get(bcm_gport_t port, 
                              bcmx_port_phy_timesync_config_t *conf)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(conf);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_phy_timesync_config_get(bcm_unit, port,
                                        BCMX_PORT_TIMESYNC_CONFIG_T_PTR_TO_BCM
                                        (conf));
}

/*
 * Function:
 *     bcmx_port_phy_timesync_enhanced_capture_get
 * Purpose:
 *     Get phy_timesync configuration for port.
 * Parameters:
 *     port - Port
 *     value - (OUT) Enhanced capture info
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_port_phy_timesync_enhanced_capture_get(bcm_gport_t port, 
                              bcmx_port_phy_timesync_enhanced_capture_t *value)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(value);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_phy_timesync_enhanced_capture_get(bcm_unit, port,
                                        BCMX_PORT_TIMESYNC_ENHANCED_CAPTURE_T_PTR_TO_BCM
                                        (value));
}


/*
 * Function:
 *     bcmx_port_control_phy_timesync_set
 * Purpose:
 *     Set phy_timesync control value.
 * Parameters:
 *     port  - Port
 *     type  - Timesync control
 *     value - Value
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_port_control_phy_timesync_set(bcm_gport_t port, 
                               bcm_port_control_phy_timesync_t type, uint64 value)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_control_phy_timesync_set(bcm_unit, port, type, value);
}


/*
 * Function:
 *     bcmx_port_control_phy_timesync_get
 * Purpose:
 *     Get phy_timesync control value.
 * Parameters:
 *     port  - Port
 *     type  - Timesync control
 *     value - (OUT) Value
 * Returns:
 *     BCM_E_XXX
 */
int
bcmx_port_control_phy_timesync_get(bcm_gport_t port, 
                               bcm_port_control_phy_timesync_t type, uint64 *value)
{
    int         bcm_unit;
    bcm_port_t  bcm_port;

    BCMX_PORT_INIT_CHECK;

    BCMX_PARAM_NULL_CHECK(value);

    BCM_IF_ERROR_RETURN
        (_bcmx_dest_to_unit_port(port, &bcm_unit, &bcm_port,
                                 BCMX_DEST_CONVERT_DEFAULT));

    return bcm_port_control_phy_timesync_get(bcm_unit, port, type, value);
}

