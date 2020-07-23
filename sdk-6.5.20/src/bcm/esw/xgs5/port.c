/*
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        port.c
 * Purpose:     XGS5 Port common driver.
 *
 * Notes:
 *      More than likely only new Port APIs are found in
 *      this function driver due to a huge amount of legacy code
 *      that were implemented before this model.
 */

#include <shared/bsl.h>
#include <soc/defs.h>
#include <soc/drv.h>

#include <bcm/error.h>
#include <bcm/switch.h>

#ifdef BCM_XGS5_SWITCH_PORT_SUPPORT
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/portctrl.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/mirror.h>
#include <bcm_int/esw/firebolt.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/trident.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/xgs3.h>
#include <bcm_int/esw/stat.h>
#include <bcm_int/esw/link.h>
#include <bcm/port.h>

#include <soc/counter.h>
#include <soc/esw/portctrl.h>
#if defined(BCM_TRIDENT3_SUPPORT)
#include <soc/trident3.h>
#endif /* BCM_TRIDENT3_SUPPORT */
#if defined(BCM_HELIX5_SUPPORT)
#include <soc/helix5.h>
#endif /* BCM_HELIX5_SUPPORT */
#if defined(BCM_HURRICANE4_SUPPORT)
#include <soc/hurricane4.h>
#endif /* BCM_HURRICANE4_SUPPORT */
#if defined(BCM_TOMAHAWK2_SUPPORT)
#include <soc/tomahawk2.h>
#endif /* BCM_TOMAHAWK2_SUPPORT */

#if defined(BCM_TOMAHAWK3_SUPPORT)
#include <soc/tomahawk3.h>
#endif /* BCM_TOMAHAWK3_SUPPORT */
#if defined(BCM_FIREBOLT6_SUPPORT)
#include <soc/firebolt6.h>
#endif /* BCM_FIREBOLT6_SUPPORT */

#undef DSCP_CODE_POINT_CNT
#define DSCP_CODE_POINT_CNT 64

/* Checks for 'null' argument */
#define PARAM_NULL_CHECK(_arg)  \
    if ((_arg) == NULL) {       \
        return BCM_E_PARAM;     \
    }

typedef struct bcmi_xgs5_port_resource_bmp_s {

    pbmp_t inact_pbm_log; /* Logical ports to be inactive via legacy API */
    pbmp_t act_pbm_log; /* Logical ports to be active via legacy API */

    pbmp_t del_pbm_log; /* Logical ports to be deleted in Flexport */
    pbmp_t add_pbm_log; /* Logical ports to be added in Flexport */

    /* Bitmap of local physical ports in each pipe that to be inactived/actived
     * during Flexport.
     * Global physical ports should be local_phy_port + pipe_offset
     */
    pbmp_t del_pbm_phy[SOC_MAX_NUM_PIPES];
    pbmp_t add_pbm_phy[SOC_MAX_NUM_PIPES];

    /* Port Macros to be inactived in Flexport */
    SHR_BITDCL  del_bmp_pm[_SHR_BITDCLSIZE(SOC_MAX_NUM_BLKS)];
    /* Port Macros to be actived in Flexport */
    SHR_BITDCL  add_bmp_pm[_SHR_BITDCLSIZE(SOC_MAX_NUM_BLKS)];
} bcmi_xgs5_port_resource_bmp_t;

/*
 * BCMI Port Driver
 */
static bcmi_xgs5_port_drv_t  *bcmi_xgs5_port_drv[BCM_MAX_NUM_UNITS] = {0};

#define BCMI_PORT_DRV(u_)             (bcmi_xgs5_port_drv[(u_)])
#define BCMI_PORT_DRV_CALL(u_)        (bcmi_xgs5_port_drv[(u_)]->port_calls)
#define BCMI_PORT_DRV_DEV_INFO(u_)    (bcmi_xgs5_port_drv[(u_)]->dev_info[(u_)])
#define BCMI_PORT_DRV_PHY_INFO(u_, _p)    \
    (BCMI_PORT_DRV_DEV_INFO(u_)->phy_port_info[(_p)])

#define BCMI_PORT_SCHEDULE_STATE_INIT_OP_ASF(_op)      \
    (_op & (BCMI_XGS5_PORT_RESOURCE_OP_ADD |           \
            BCMI_XGS5_PORT_RESOURCE_OP_DEL |           \
            BCMI_XGS5_PORT_RESOURCE_OP_REMAP |         \
            BCMI_XGS5_PORT_RESOURCE_OP_LANES |         \
            BCMI_XGS5_PORT_RESOURCE_OP_SPEED |         \
            BCMI_XGS5_PORT_RESOURCE_OP_ACTIVE |         \
            BCMI_XGS5_PORT_RESOURCE_OP_ENCAP_FLEX |    \
            BCMI_XGS5_PORT_RESOURCE_OP_CALENDAR))

#define BCMI_PORT_SCHEDULE_STATE_INIT_OP_TDM(_op)      \
    (_op & (BCMI_XGS5_PORT_RESOURCE_OP_ADD |           \
            BCMI_XGS5_PORT_RESOURCE_OP_DEL |           \
            BCMI_XGS5_PORT_RESOURCE_OP_REMAP |         \
            BCMI_XGS5_PORT_RESOURCE_OP_LANES |         \
            BCMI_XGS5_PORT_RESOURCE_OP_SPEED |         \
            BCMI_XGS5_PORT_RESOURCE_OP_ACTIVE |         \
            BCMI_XGS5_PORT_RESOURCE_OP_ENCAP_FLEX |    \
            BCMI_XGS5_PORT_RESOURCE_OP_CHL        |    \
            BCMI_XGS5_PORT_RESOURCE_OP_CALENDAR))

/* Hardware definitions */
static bcmi_xgs5_port_hw_defs_t  *bcmi_xgs5_port_hw_defs[BCM_MAX_NUM_UNITS] = {0};

#define PORT_HW(u_)    (bcmi_xgs5_port_hw_defs[(u_)])

#define PORT_REDIRECTION_HW_DEFS_EGR_PORT(u_)  \
    (bcmi_xgs5_port_hw_defs[(u_)]->egr_port)


#define BCMI_MAX_STRENGTH_VALUE (0x3E)

static char *bcmi_xgs5_port_attach_str[] = BCMI_XGS5_PORT_ATTACH_PHASES_MSG;
static char *bcmi_xgs5_port_detach_str[] = BCMI_XGS5_PORT_DETACH_PHASES_MSG;

/*
 * Function:
 *      bcmi_xgs5_port_addressable_local_get
 * Description:
 *      Validate that given port/gport parameter is an addressable
 *      local logical port and return local logical port in
 *      BCM port format (non-Gport).
 *      Logical port does not need to be defined (does not
 *      need to be a VALID port, only addressable).
 * Parameters:
 *      unit       - (IN) BCM device number
 *      port       - (IN) Port / Gport to validate
 *      local_port - (OUT) Port number if valid.
 * Return Value:
 *      BCM_E_NONE - Port OK
 *      BCM_E_INIT - Not initialized
 *      BCM_E_PORT - Port Invalid
 *
 * NOTE:
 *      This function only accepts those GPORT types that
 *      deal with logical BCM ports (i.e. MODPORT, LOCAL, etc.).
 *
 *      Also, unlike bcm_esw_port_local_get(), this routine only checks
 *      that the port number is within the valid port range.
 *      It does NOT check whether the port
 *      is a BCM valid port (i.e. a defined/configured port).
 *      This allows functions that requires port validation with
 *      a new logical port that has not been declared/configured
 *      in the system.
 */
int
bcmi_xgs5_port_addressable_local_get(int unit,
                                     bcm_port_t port, bcm_port_t *local_port)
{
    bcm_module_t mod;
    bcm_trunk_t trunk_id;
    int id;
    int is_local;

    /*
     * A logical port can be:
     *   - Defined and active.
     *   - Defined and inactive (SOC property portmap with ':i' option).
     *   - Not defined.
     *
     * The following port definitions/words (i.e. functions, macro names)
     * are used in the SDK as follows:
     *   - VALID       :  The logical port is defined, it may be active or
     *                    inactive.
     *                    There IS a logical-to-physical port mapping.
     *
     *   - ADDRESSABLE :  The logical port number is within the valid
     *                    range.
     *                    The port may or may have not been defined
     *                    (i.e. there is NO logical-to-physical port mapping.)
     */
    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, port, &mod, local_port,
                                    &trunk_id, &id));
        if ((trunk_id != -1) || (id != -1)) {
            return BCM_E_PORT;
        }

        /* Check that port is local */
        BCM_IF_ERROR_RETURN
            (_bcm_esw_modid_is_local(unit, mod, &is_local));

        if (!is_local) {
            return BCM_E_PORT;
        }
    } else {
        *local_port = port;
    }

    /* Check that port is within valid logical port range */
    if (!SOC_PORT_ADDRESSABLE(unit, *local_port)) {
        return BCM_E_PORT;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_soc_info_ptype_update
 * Purpose:
 *      Update the SOC_INFO port type bitmaps and block information.
 * Parameters:
 *      unit                    - (IN) Unit number.
 *      port_schedule_state     - (IN) port_schedule_state
 *
 *
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Assumes linkscan is paused, COUNTER_LOCK and SOC_CONTROL_LOCK
 *      locks are taken.
 */
STATIC int
_bcmi_xgs5_port_soc_info_ptype_update(int unit)
{
    soc_info_t *si = &SOC_INFO(unit);
    int logic_port, phy_port, port_idx, max_port;
    int blk, bindex;

    /*
     * Update ptype information
     */
#define RECONFIGURE_PORT_TYPE_INFO(ptype)                   \
    si->ptype.num = 0;                                      \
    si->ptype.min = si->ptype.max = -1;                     \
    PBMP_ITER(si->ptype.bitmap, logic_port) {               \
        si->port_offset[logic_port] = si->ptype.num;        \
        si->ptype.port[si->ptype.num++] = logic_port;       \
        if (si->ptype.min < 0) {                            \
            si->ptype.min = logic_port;                     \
        }                                                   \
        if (logic_port > si->ptype.max) {                   \
            si->ptype.max = logic_port;                     \
        }                                                   \
    }

    RECONFIGURE_PORT_TYPE_INFO(ge);
    RECONFIGURE_PORT_TYPE_INFO(xe);
    RECONFIGURE_PORT_TYPE_INFO(c);
    RECONFIGURE_PORT_TYPE_INFO(ce);
    RECONFIGURE_PORT_TYPE_INFO(cde);
    RECONFIGURE_PORT_TYPE_INFO(xl);
    RECONFIGURE_PORT_TYPE_INFO(cl);
    RECONFIGURE_PORT_TYPE_INFO(gx);
    RECONFIGURE_PORT_TYPE_INFO(ether);
    RECONFIGURE_PORT_TYPE_INFO(hg);
    RECONFIGURE_PORT_TYPE_INFO(st);
    RECONFIGURE_PORT_TYPE_INFO(port);
    RECONFIGURE_PORT_TYPE_INFO(all);

#undef  RECONFIGURE_PORT_TYPE_INFO


    /*
     * Update block port
     * Use first logical port on block (logic follows ESW driver
     * soc_info_config().
     */
    for (phy_port = 0; ; phy_port++) {

        blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
        bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, 0);
        if (blk < 0 && bindex < 0) { /* End of port list */
            break;
        }

        logic_port = si->port_p2l_mapping[phy_port];
        if (logic_port < 0) {
            continue;
        }

        for (port_idx = 0; port_idx < SOC_DRIVER(unit)->port_num_blktype;
             port_idx++) {
            blk = SOC_PORT_IDX_BLOCK(unit, phy_port, port_idx);
            if (blk < 0) { /* End of block list of each port */
                break;
            }

            if (si->block_port[blk] < 0) {
                si->block_port[blk] = logic_port;
            }
        }
    }

    /* Update MAX_PORT(unit) */
    max_port = -1;
    for (phy_port = 0; phy_port < BCMI_PORT_DRV_DEV_INFO(unit)->phy_ports_max;
         phy_port++) {
        logic_port = si->port_p2l_mapping[phy_port];
        if (logic_port < 0) {
            continue;
        }
        if (max_port < logic_port) {
            max_port = logic_port;
        }
    }
    si->port_num = max_port + 1;

    /* Update user port mappings (includes port names update) */
    soc_esw_dport_init(unit);

    return SOC_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_management_port_soc_info_ptype_update
 * Purpose:
 *      Update the SOC_INFO port type bitmaps for management port.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      port           - (IN) Logical port.
 *      speed        - (IN) speed for given port.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
STATIC int
_bcmi_xgs5_management_port_soc_info_ptype_update(int unit,
                                                bcm_port_t port, int speed)
{
    soc_info_t *si = &SOC_INFO(unit);
    int rv = BCM_E_NONE;

    if (!SOC_PBMP_MEMBER(si->st.bitmap, port) 
        && (!SOC_PBMP_MEMBER(si->hg.bitmap, port))) {
        SOC_CONTROL_LOCK(unit);
        SOC_PBMP_PORT_REMOVE(si->ge.bitmap, port);
        SOC_PBMP_PORT_REMOVE(si->xe.bitmap, port);

        /* Current devices only support XE/GE/HG type management port */
        if (speed < 10000) {    /* GE */
            SOC_PBMP_PORT_ADD(si->ge.bitmap, port);
        } else {  /* XE */
            SOC_PBMP_PORT_ADD(si->xe.bitmap, port);
        }

        rv = _bcmi_xgs5_port_soc_info_ptype_update(unit);
        SOC_CONTROL_UNLOCK(unit);
    }

    return rv;

}


/*
 * Function:
 *      bcmi_xgs5_port_fn_drv_init
 * Purpose:
 *      Initialize the Port function driver.
 * Parameters:
 *      unit - (IN) Unit number.
 *      drv  - (IN) Pointer to the Port function driver.
 *      bcmi_drv - (IN) Pointer to the BCMI Port function driver.
 *      hw_defs - (IN) Pointer to the h/w definition struct.
 * Returns:
 *      BCM_E_xxx
 * Notes:
 *      This routine only intializes the function driver.
 *      The main port routine is still in the esw/port.c file.
 */
int
bcmi_xgs5_port_fn_drv_init(int unit, bcm_esw_port_drv_t *drv,
                           bcmi_xgs5_port_drv_t *bcmi_drv,
                           bcmi_xgs5_port_hw_defs_t *hw_defs)
{
    PARAM_NULL_CHECK(drv);

    /* Assign the driver */
    BCM_ESW_PORT_DRV(unit) = drv;

    /* Assign the bcmi port driver */
    if (bcmi_drv) {
        BCMI_PORT_DRV(unit) = bcmi_drv;
    }

    /* Assign the hardware definitions */
    if(hw_defs) {
        PORT_HW(unit) = hw_defs;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_resource_set
 * Purpose:
 *      Modify the following port resources:
 *      - Logical to physical port mapping
 *      - Speed
 *      - Number of PHY lanes
 *      - Encapsulation mode
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port     - (IN) Logical port.
 *      resource - (IN) Port resource configuration.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Assumes caller has lock.
 */
int
bcmi_xgs5_port_resource_set(int unit,
                            bcm_gport_t port, bcm_port_resource_t *resource)
{
    bcm_port_t  lport1, lport2;

    PARAM_NULL_CHECK(resource);

    /* Check that given port matches logical port field in structure */
    BCM_IF_ERROR_RETURN
        (bcmi_xgs5_port_addressable_local_get(unit,
                                              port, &lport1));
    BCM_IF_ERROR_RETURN
        (bcmi_xgs5_port_addressable_local_get(unit,
                                              resource->port, &lport2));
    if (lport1 != lport2) {
        return BCM_E_PARAM;
    }

    return BCM_ESW_PORT_DRV(unit)->resource_multi_set(unit, 1, resource);
}


/*
 * Function:
 *      bcmi_xgs5_port_resource_get
 * Purpose:
 *      Get the port resource configuration for the specified logical port.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      port      - (IN) Logical port.
 *      resource  - (OUT) Returns port resource information.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Assumes caller has lock.
 */
int
bcmi_xgs5_port_resource_get(int unit,
                            bcm_gport_t port, bcm_port_resource_t *resource)
{
    bcm_port_t lport;
    soc_info_t *si = &SOC_INFO(unit);
    int encap = 0;
#if defined(PORTMOD_SUPPORT) && defined(PORTMOD_PM8X50_SUPPORT)
    portmod_speed_config_t speed_config;
    int rv = BCM_E_FAIL;
#endif

    PARAM_NULL_CHECK(resource);

    BCM_IF_ERROR_RETURN
        (bcmi_xgs5_port_addressable_local_get(unit,
                                              port, &lport));
    bcm_port_resource_t_init(resource);
    resource->flags = 0x0;
    resource->port = lport;
    resource->physical_port = si->port_l2p_mapping[lport];
    resource->lanes = si->port_num_lanes[lport];
    BCM_IF_ERROR_RETURN
        (bcm_esw_port_speed_get(unit, lport, &resource->speed));
    BCM_IF_ERROR_RETURN
        (bcm_esw_port_encap_get(unit, lport, &encap));
    resource->encap = encap;

#if defined(PORTMOD_SUPPORT) && defined(PORTMOD_PM8X50_SUPPORT)
    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        rv = bcmi_esw_portctrl_speed_config_get(unit, port, &speed_config);
    }

    if (BCM_SUCCESS(rv) && !SAL_BOOT_SIMULATION) {
        /* PM8x50's autoneg can change the number of lanes used,
         * so if the user is in autoneg and does a bcm_port_resource_get
         * they need to get the # of lanes being used in hardware
         */
        resource->lanes = speed_config.num_lane;

        resource->fec_type = speed_config.fec;
        resource->phy_lane_config = speed_config.lane_config;
        resource->link_training = speed_config.link_training;
    } else
#endif
    {
        /* Upon error fetching from hardware, or if running BCMSIM,
         * return the database values */
        resource->fec_type = SOC_INFO(unit).port_fec_type[lport];
        resource->phy_lane_config = SOC_INFO(unit).port_phy_lane_config[lport];
        resource->link_training = SOC_INFO(unit).port_link_training[lport];
    }

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    /* Add the channelized port information. */
    if (soc_feature(unit, soc_feature_channelized_switching)) {
        resource->num_subports = si->port_num_subport[lport];
        resource->line_card = si->port_lc_mapping[lport];

        if ((resource->num_subports > 0) || (resource->line_card > 0)) {
            resource->flags |= BCM_PORT_RESOURCE_CASCADED;
        }
    }
#endif
    /* Flag BCM_PORT_RESOURCE_25G_USE_50G_CALENDAR indicates 25G port use 50G TDM.
     */
    if (SOC_PBMP_MEMBER(si->pbm_25g_use_50g_tdm, lport)) {
        resource->flags |= BCM_PORT_RESOURCE_25G_USE_50G_CALENDAR;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_resource_resolve
 * Purpose:
 *      Convert logical port number GPORT to BCM local port format.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN/OUT) Port resource configuration array.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      - Assumes caller has lock.
 *      - Resource is not NULL.
 */
STATIC int
_bcmi_xgs5_port_resource_resolve(int unit, 
                                int nport,
                                bcm_port_resource_t *resource)
{
    int i;
    bcm_port_resource_t *pr;
    bcmi_xgs5_dev_info_t *dev_info = BCMI_PORT_DRV_DEV_INFO(unit);

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {

        /* Check that logical port number is addressable and convert */
        BCM_IF_ERROR_RETURN
            (bcmi_xgs5_port_addressable_local_get(unit,
                                                  pr->port,
                                                  &pr->port));

        /* Check that physical port is within the valid range */
        if (pr->physical_port != -1 &&
            pr->physical_port >= dev_info->phy_ports_max) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Invalid physical port %d\n"),
                       pr->physical_port));
            return BCM_E_PORT;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_resource_speed_multi_set
 * Purpose:
 *      Modify the following port resources:
 *      - Speed
 *      - fec_type
 *      - phy_lane_config
 *      - link_training
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Assumes caller has lock.
 */
int
bcmi_xgs5_port_resource_speed_multi_set(int unit, 
                                  int nport, bcm_port_resource_t *resource)
{

    soc_info_t *si = &SOC_INFO(unit);
    int encap, i, soc_num, rindex, rv;
    bcm_port_resource_t *local_resource = NULL;
    int man_count = 0;

    /* double the number of passed entries because we need to delete the ports,
     * then add them. */
    soc_num = nport * 2;

    local_resource = sal_alloc(soc_num * sizeof(bcm_port_resource_t),
                                "resource array");
    if (NULL == local_resource) {
        return BCM_E_MEMORY;
    }

    for (i = 0; i < soc_num; i++) {
        bcm_port_resource_t_init(&local_resource[i]);
    }

    /* Build port resource for delete */
    for (i = 0, rindex = 0; rindex < nport; i++, rindex++) {
        /* Don't include Management Ports in this */
        if ((!BCMI_PORT_DRV_DEV_INFO(unit)->aux_port_flexible) &&
            IS_MANAGEMENT_PORT(unit,resource[rindex].port)) {
            /* back off one for this management port */
            i--;
            continue;
        }

        local_resource[i].flags = SOC_PORT_RESOURCE_I_MAP| SOC_PORT_RESOURCE_SPEED;
        local_resource[i].port = resource[rindex].port;
        local_resource[i].physical_port = -1;
    }


    /* Build resource for add */
    for(i = nport, rindex = 0; rindex < nport; i++, rindex++) {

        /* Do nothing except update port type in case auxiliary ports don't
         * support flexport */
        /* Don't flex Management Ports; just update ptype */
        if ((!BCMI_PORT_DRV_DEV_INFO(unit)->aux_port_flexible) &&
            IS_MANAGEMENT_PORT(unit,resource[rindex].port)) {
            BCM_IF_ERROR_RETURN
                (_bcmi_xgs5_management_port_soc_info_ptype_update(unit, 
                                    resource[rindex].port, resource[rindex].speed));
            /* back off one for this management port */
            i--;
            man_count++;
            continue;
        }


        /* Keep encap, lanes, phy_port the same */
        local_resource[i].flags = SOC_PORT_RESOURCE_I_MAP | SOC_PORT_RESOURCE_SPEED;
        local_resource[i].port = resource[rindex].port;
        local_resource[i].physical_port = si->port_l2p_mapping[resource[rindex].port];
        local_resource[i].speed = resource[rindex].speed;
        local_resource[i].lanes = si->port_num_lanes[resource[rindex].port];

        /* Get current port encap */
        rv = bcm_esw_port_encap_get(unit, resource[rindex].port,
                            &encap);
        if (!BCM_SUCCESS(rv)) {
            goto cleanup;
        }
        local_resource[i].encap = encap;

        /* Required Fields for new speed set */
        local_resource[i].fec_type = resource[rindex].fec_type;
        local_resource[i].phy_lane_config = resource[rindex].phy_lane_config;
        local_resource[i].link_training = resource[rindex].link_training;

    }

    rv = bcmi_xgs5_port_resource_multi_set(unit, soc_num - (man_count * 2), local_resource);

cleanup:
    if (NULL != local_resource) {
        sal_free(local_resource);
    }

    return rv;
}

/*
 * Function:
 *      _bcmi_xgs5_port_flexible_validate
 * Purpose:
 *      Check that FlexPort operation is valid on given physical port.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      phy_port  - (IN) Physical port number
 * Returns:
 *      TRUE or FALSE.
 */
STATIC int
_bcmi_xgs5_port_flexible_validate(int unit, int phy_port)
{
    if (phy_port != -1 && !BCMI_PORT_DRV_PHY_INFO(unit, phy_port).flex) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "FlexPort operation is not enabled on "
                              "physical port %d\n"),
                   phy_port));
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _soc_td2p_phy_port_lanes_valid
 * Purpose:
 *      Check that given lane setting is valid for physical port.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port     - (IN) Base physical port.
 *      lanes    - (IN) Number of PHY lanes.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_bcmi_xgs5_port_lanes_validate(int unit, int phy_port, int lanes)
{
    uint32 lane_mask;

    switch (lanes) {
    case 1:
        lane_mask = BCMI_XGS5_PORT_LANES_1;
        break;
    case 2:
        lane_mask = BCMI_XGS5_PORT_LANES_2;
        break;
    case 4:
        lane_mask = BCMI_XGS5_PORT_LANES_4;
        break;
    case 8:
        lane_mask = BCMI_XGS5_PORT_LANES_8;
        break;
    case 10:
        lane_mask = BCMI_XGS5_PORT_LANES_10;
        break;
    case 12:
        lane_mask = BCMI_XGS5_PORT_LANES_12;
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit,
                                "Invalid number of lanes for "
                                "physical_port=%d, lanes=%d\n"),
                    phy_port, lanes));
        return BCM_E_PARAM;
    }

    if (!(BCMI_PORT_DRV_PHY_INFO(unit, phy_port).lanes_valid & lane_mask)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Invalid lane configuration for "
                              "physical_port=%d, lane=%d, "
                              "valid_lanes=%s %s %s %s %s %s\n"),
                   phy_port, lanes,
                   (BCMI_PORT_DRV_PHY_INFO(unit, phy_port).lanes_valid &
                    BCMI_XGS5_PORT_LANES_1) ? "1" : "",
                   (BCMI_PORT_DRV_PHY_INFO(unit, phy_port).lanes_valid &
                    BCMI_XGS5_PORT_LANES_2) ? "2" : "",
                   (BCMI_PORT_DRV_PHY_INFO(unit, phy_port).lanes_valid &
                    BCMI_XGS5_PORT_LANES_4) ? "4" : "",
                   (BCMI_PORT_DRV_PHY_INFO(unit, phy_port).lanes_valid &
                    BCMI_XGS5_PORT_LANES_8) ? "8" : "",
                   (BCMI_PORT_DRV_PHY_INFO(unit, phy_port).lanes_valid &
                    BCMI_XGS5_PORT_LANES_10) ? "10" : "",
                   (BCMI_PORT_DRV_PHY_INFO(unit, phy_port).lanes_valid &
                    BCMI_XGS5_PORT_LANES_12) ? "12" : ""));

        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_speed_validate
 * Purpose:
 *      Check that given speed is valid for the number of serdes lanes.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port     - (IN) Logical port number.
 *      phy_port - (IN) Physical port number.
 *      lanes    - (IN) Number of PHY lanes.
 *      encap    - (IN) Encap mode.
 *      speed    - (IN) Port rate to check.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_bcmi_xgs5_port_speed_validate(int unit, int port, int phy_port, int lanes,
                               int encap, int speed)
{
    uint32 speed_mask;
    char speed_valid[256], *pos;
    int i, string_len, len, is_hg_speed;
    char *port_ability_speed_string[] = SOC_PA_SPEED_STRING;
#if defined(PORTMOD_SUPPORT) && defined(PORTMOD_PM8X50_SUPPORT)
    portmod_dispatch_type_t pm_type;
    int rv, existing_physical_port, autoneg = 0;
#endif

    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) ||
        IS_MANAGEMENT_PORT(unit, port)) {
        return BCM_E_NONE;
    }

    if (speed == 1000 && encap != BCM_PORT_ENCAP_IEEE) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Speed and encapsulation configuration unmatch "
                              "for port=%d: 1G ports don't support HG mode.\n"),
                   port));
        return BCM_E_PARAM;
    }

    if (speed > lanes * BCMI_PORT_DRV_PHY_INFO(unit, phy_port).max_lane_speed) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Invalid speed port=%d: Max lane speed is %d.\n"),
                   port,
                   BCMI_PORT_DRV_PHY_INFO(unit, phy_port).max_lane_speed));
        return BCM_E_PARAM;
    }

#if defined(PORTMOD_SUPPORT) && defined(PORTMOD_PM8X50_SUPPORT)
    /* PM8x50 don't support bcm_port_ability. Speeds will be verified
     * in portmod, but check to make sure the device supports the speed too */
    BCM_IF_ERROR_RETURN(portmod_phy_pm_type_get(unit, phy_port, &pm_type));
    if (pm_type == portmodDispatchTypePm8x50) {

        /* Autoneg in the PM8x50 allows scenarios like 50G with 4 lanes:
         * Starting with a 200G port (4 lanes), autoneg can scale down to 1 lane
         * with 50G.  So we don't want to do device speed/lane validations
         * if the port is in autoneg
         */
        existing_physical_port = SOC_INFO(unit).port_l2p_mapping[port];
        if (existing_physical_port != -1) {
            BCM_IF_ERROR_RETURN(bcm_esw_port_autoneg_get(unit,
                                    port, &autoneg));
            rv = BCM_E_NONE;
        }

        if (autoneg == 0) {
            rv = BCM_E_CONFIG;

            if (BCM_ESW_PORT_DRV(unit) != NULL) {
                if (BCM_ESW_PORT_DRV(unit)->device_speed_check != NULL) {
                    rv = (BCM_ESW_PORT_DRV(unit)->device_speed_check(unit,
                                        speed, lanes));
                }
            }
            /* speed and lane configuration wasn't found */
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Invalid speed configuration for "
                                  "port=%d, speed=%d, lanes=%d\n"),
                       port, speed, lanes));
                return rv;
            }
        }

        return rv;
    }
#endif

    speed_mask = SOC_PA_SPEED(speed);
    is_hg_speed = SOC_PA_IS_HG_SPEED(speed_mask);

    /* Expect non-higig speed for IEEE encapsulation mode. */
    if (is_hg_speed && encap == BCM_PORT_ENCAP_IEEE) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Speed and encapsulation configuration unmatch "
                              "for port=%d, speed=%d, encap=%d\n"),
                   port, speed, encap));
        return BCM_E_PARAM;
    }

    if (!(BCMI_PORT_DRV_DEV_INFO(unit)->speed_valid[lanes] & speed_mask)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Invalid speed configuration for "
                              "port=%d, speed=%d\n"),
                   port, speed));

        sal_memset(speed_valid, 0, sizeof(speed_valid));
        string_len = sizeof(port_ability_speed_string)/sizeof(char *);
        pos = speed_valid;
        for (i = 0;i < string_len;i++){
            if (BCMI_PORT_DRV_DEV_INFO(unit)->speed_valid[lanes] & (1<<i)){
                len = sal_sprintf(pos, "%s ", port_ability_speed_string[i]);
                if (len < 0) {
                    LOG_ERROR(BSL_LS_BCM_COMMON,
                              (BSL_META_U(unit,
                                          "len = %d\n"),
                               len));
                    return BCM_E_RESOURCE;
                }
                pos += len;
            }
        }

        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit, "valid speeds: %s\n"), speed_valid));
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_encap_validate
 * Purpose:
 *      Check that given encap is valid for unit.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port     - (IN) Logical port number.
 *      phy_port - (IN) Physical port number.
 *      encap    - (IN) Encap mode.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_bcmi_xgs5_port_encap_validate(int unit, int port, int phy_port,
                                       int encap)
{
    uint32 encap_mask;

    switch (encap) {
    case BCM_PORT_ENCAP_IEEE:
        encap_mask = BCM_PA_ENCAP_IEEE;
        break;
    case BCM_PORT_ENCAP_HIGIG:
        encap_mask = BCM_PA_ENCAP_HIGIG;
        break;
    case BCM_PORT_ENCAP_B5632:
        encap_mask = BCM_PA_ENCAP_B5632;
        break;
    case BCM_PORT_ENCAP_HIGIG2:
        encap_mask = BCM_PA_ENCAP_HIGIG2;
        break;
    case BCM_PORT_ENCAP_HIGIG2_LITE:
        encap_mask = BCM_PA_ENCAP_HIGIG2_LITE;
        break;
    case BCM_PORT_ENCAP_HIGIG2_L2:
        encap_mask = BCM_PA_ENCAP_HIGIG2_L2;
        break;
    case BCM_PORT_ENCAP_HIGIG2_IP_GRE:
        encap_mask = BCM_PA_ENCAP_HIGIG2_IP_GRE;
        break;
    case BCM_PORT_ENCAP_HIGIG_OVER_ETHERNET:
        encap_mask = BCM_PA_ENCAP_HIGIG_OVER_ETHERNET;
        break;
    default:
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Invalid encapsulation for "
                              "unit=%d, encap=%d\n"),
                   unit, encap));
        return BCM_E_PARAM;
    }

    if (!(BCMI_PORT_DRV_DEV_INFO(unit)->encap_mask & encap_mask)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Invalid encapsulation configuration for "
                              "unit=%d, encap=%d\n"),
                   unit, encap));
        return BCM_E_PARAM;
    }

    if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) ||
        IS_MANAGEMENT_PORT(unit, port)) {
        return BCM_E_NONE;
    }

    if (BCMI_PORT_DRV_DEV_INFO(unit)->phy_port_info[phy_port].force_hg &&
        !(BCMI_PORT_DRV_DEV_INFO(unit)->hg_encap_mask & encap_mask)) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Encapsulation configuration %d is invalid on "
                              "hg-only port %d of unit %d\n"),
                   encap, port, unit));
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_flex_max_validate
 * Purpose:
 *      Check that the port num in a port macro shall not exceed the max num
 *      that it could flex to.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN/OUT) Port resource configuration array.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      - Assumes caller has lock.
 *      - Resource is not NULL.
 */
STATIC int
_bcmi_xgs5_port_flex_max_validate(int unit,
                                  int nport,
                                  bcm_port_resource_t *resource)
{
    soc_info_t *si = &SOC_INFO(unit);
    bcm_port_resource_t *pr;
    int logic_port, phy_port, port_idx, blk;
    pbmp_t *blk_bitmap;
    int port_count;
    int i;

    blk_bitmap = sal_alloc(SOC_MAX_NUM_BLKS * sizeof(pbmp_t), "blk_bitmap");
    sal_memcpy(blk_bitmap, si->block_bitmap, SOC_MAX_NUM_BLKS * sizeof(pbmp_t));

    /* Entries to delete ports */
    for (i = 0, pr = &resource[0]; (i < nport) && pr->physical_port == -1;
         i++, pr++) {
        logic_port = pr->port;
        phy_port = si->port_l2p_mapping[logic_port];
        if (phy_port == -1) {
            continue;
        }

        for (port_idx = 0; port_idx < SOC_DRIVER(unit)->port_num_blktype;
             port_idx++) {
            blk = SOC_PORT_IDX_BLOCK(unit, phy_port, port_idx);
            if (blk < 0) { /* End of block list of each port */
                break;
            }
            SOC_PBMP_COUNT(blk_bitmap[blk], port_count);
            if (port_count == 0) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "Port Macro %d doesn't have valid port "
                                      "to delete.\n"),
                           SOC_BLOCK_NUMBER(unit,blk)));
                sal_free(blk_bitmap);
                return BCM_E_INTERNAL;
            }
            if (SOC_PBMP_MEMBER(blk_bitmap[blk], logic_port)) {
                SOC_PBMP_PORT_REMOVE(blk_bitmap[blk], logic_port);
            }
        } /* For each block in the given port */
    }

    /* Entries to add ports */
    for (;i < nport; i++) {
        pr = &resource[i];
        logic_port = pr->port;
        phy_port = pr->physical_port;
        for (port_idx = 0; port_idx < SOC_DRIVER(unit)->port_num_blktype;
             port_idx++) {
            blk = SOC_PORT_IDX_BLOCK(unit, phy_port, port_idx);
            if (blk < 0) { /* End of block list of each port */
                break;
            }
            if (!SOC_PBMP_MEMBER(blk_bitmap[blk], logic_port)) {
                SOC_PBMP_PORT_ADD(blk_bitmap[blk], logic_port);
                SOC_PBMP_COUNT(blk_bitmap[blk], port_count);
                if (port_count > si->pm_max_ports[blk]) {
                    LOG_ERROR(BSL_LS_BCM_PORT,
                              (BSL_META_U(unit,
                                      "Port num on Port Macro %d would "
                                      "exceed the Max num %d that it could "
                                      "flex to.\n"),
                               SOC_BLOCK_NUMBER(unit,blk),
                               si->pm_max_ports[blk]));
                    sal_free(blk_bitmap);
                    return BCM_E_PARAM;
                }
            }
        } /* For each block in the given port */
    }

    sal_free(blk_bitmap);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_resource_input_validate
 * Purpose:
 *      Validate function input requirements.
 *
 *      This routine checks for function semantics and guidelines
 *      from the API perspective:
 *      - Check that logical port number is valid and convert GPORT port
 *        format to BCM local port format.
 *      - Logical and physical port number must be within valid range.
 *      - Check the order of elements in array, 'delete' operations must
 *        be placed first in array.
 *      - Check that physical port is flexible.
 *      - Check that lanes, speed and encap are valid.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN/OUT) Port resource configuration array.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      - Assumes caller has lock.
 *      - Resource is not NULL.
 */
STATIC int
_bcmi_xgs5_port_resource_input_validate(int unit, 
                                        int nport,
                                        bcm_port_resource_t *resource)
{
    int i;
    bcm_port_resource_t *pr;
    int enable;
    int delete = 1;
    soc_info_t *si = &SOC_INFO(unit);
    int old_phy_port;
    pbmp_t del_pbmp;


    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "==== PORT RESOURCE INPUT VALIDATE ====\n")));

    /* Convert port format and validate logical and physical port numbers */
    BCM_IF_ERROR_RETURN
        (_bcmi_xgs5_port_resource_resolve(unit, 
                                         nport, resource));

    SOC_PBMP_CLEAR(del_pbmp);

    /* Check array order and port state disable */
    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {

        /* Check that delete operations are first */
        if (pr->physical_port != -1) {  /* First non-delete found */
            delete = 0;
        } else if (pr->physical_port == -1) {
            if (!delete) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "Delete operations must be "
                                      "first in array\n")));
                return BCM_E_PARAM;
            }
            SOC_PBMP_PORT_ADD(del_pbmp, pr->port);
        }

        old_phy_port = si->port_l2p_mapping[pr->port];

        /* Check if flex is enabled on port */
        if (!(pr->flags & SOC_PORT_RESOURCE_I_MAP)) {
            BCM_IF_ERROR_RETURN
                (_bcmi_xgs5_port_flexible_validate(unit, pr->physical_port));

            BCM_IF_ERROR_RETURN
                (_bcmi_xgs5_port_flexible_validate(unit, old_phy_port));
        }

        if (si->port_l2p_mapping[pr->port] != -1 &&
            !(pr->flags & SOC_PORT_RESOURCE_SPEED)) {
            /* Check that ports are disabled */
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_enable_get(unit, pr->port, &enable));
            if (enable) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "Port %d needs to be disabled\n"),
                           pr->port));
                return BCM_E_BUSY;
            }
        }

        if ((!BCMI_PORT_DRV_DEV_INFO(unit)->aux_port_flexible) &&
            SOC_PBMP_MEMBER(BCMI_PORT_DRV_DEV_INFO(unit)->aux_pbm, pr->port)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "FlexPort is not supported for auxiliary "
                                  "port %d.\n"),
                       pr->port));
            return BCM_E_PARAM;
        }

        if (pr->physical_port != -1) {

            /* Delete existing ports which are part of the FlexPort operation */
            if (old_phy_port != -1 && !SOC_PBMP_MEMBER(del_pbmp, pr->port)) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "Delete entry is required for Port %d\n"),
                           pr->port));
                return BCM_E_PARAM;
            }

            /* Check Lanes assignment */
            BCM_IF_ERROR_RETURN
                (_bcmi_xgs5_port_lanes_validate(unit, pr->physical_port,
                                                pr->lanes));

            /* Check speeds */
            BCM_IF_ERROR_RETURN
                (_bcmi_xgs5_port_speed_validate(unit, pr->port,
                                                pr->physical_port,
                                                pr->lanes, pr->encap,
                                                pr->speed));

            /* Check Encapsulation */
            BCM_IF_ERROR_RETURN
                (_bcmi_xgs5_port_encap_validate(unit, pr->port,
                                                pr->physical_port,
                                                pr->encap));
        }
    }

    /* Check that the port num in a port macro shall not exceed the max num
     * that it could flex to. */
    BCM_IF_ERROR_RETURN
        (_bcmi_xgs5_port_flex_max_validate(unit, nport, resource));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_resource_op_set
 * Purpose:
 *      Get the type of FlexPort operations/changes:
 *          none, add, delete remap, lanes, speed, encap.
 *
 * Parameters:
 *      unit         - (IN) Unit number.
 *      nport        - (IN) Number of elements in array resource.
 *      resource     - (IN) BCM port resource configuration array.
 *      soc_resource - (IN) SOC Port resource configuration array.
 *                   - (OUT) resource[i].op is set with FlexPort operation
 *      op           - (OUT) FlexPort operation BCMI_XGS5_PORT_RESOURCE_OP_...
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Assumes caller has lock.
 *      Resource is not NULL.
 *      Logical port in 'resource' is in BCM port format (non GPORT).
 * Example:
 *      L1 --> -1 (current mapped to P1 with 1 lane), op is LANES 
 *      L2 --> -1 (current mapped to P2 with 1 lane), op is REMAP 
 *      L3 --> -1 (current mapped to P3 with 1 lane), op is DELETE
 *      L1 --> P1 (with 4 lane)                     , op is SAME 
 *      L2 --> P5 (with 1 lane)                     , op is REMAP 
 *      L9 --> P9 (with 4 lane)                     , op is NEW
 *      L6 --> P6 (speed/encap update)              , op is SPEED/ENCAP
 */
STATIC int
_bcmi_xgs5_port_resource_op_set(int unit, int nport,
                                bcm_port_resource_t *resource,
                                soc_port_resource_t *soc_resource,
                                int *op)
{
    int i;
    bcm_port_resource_t *r, cr;
    soc_port_resource_t *pr;
    soc_info_t *si = &SOC_INFO(unit);
    int lport, pport, pre_pport, iport, pre_iport;
    pbmp_t att_pbm, remap_pbm, lane_pbm, speed_pbm, encap_pbm, chl_pbm;
#if defined(PORTMOD_SUPPORT) && defined(PORTMOD_PM8X50_SUPPORT)
    portmod_dispatch_type_t pm_type;
    int autoneg = 0;
#endif

    *op = BCMI_XGS5_PORT_RESOURCE_OP_NONE;
    SOC_PBMP_CLEAR(att_pbm);
    SOC_PBMP_CLEAR(remap_pbm);
    SOC_PBMP_CLEAR(lane_pbm);
    SOC_PBMP_CLEAR(speed_pbm);
    SOC_PBMP_CLEAR(encap_pbm);
    SOC_PBMP_CLEAR(chl_pbm);

    for (i = nport-1, pr = &soc_resource[nport-1], r = &resource[nport-1];
         i >= 0; i--, pr--, r--) {
        lport = pr->logical_port;
        pport = pr->physical_port;
        pre_pport = si->port_l2p_mapping[lport];
        iport = pr->idb_port;
        pre_iport = si->port_l2i_mapping[lport];

        if (pport != -1) {
#if defined(PORTMOD_SUPPORT) && defined(PORTMOD_PM8X50_SUPPORT)
            /* If it's PM8x50, need to check if fec_type, link training
             * or phy lane config changed */
            BCM_IF_ERROR_RETURN(portmod_phy_pm_type_get(unit, pport, &pm_type));
#endif

            /* Entries for port-attach */
            SOC_PBMP_PORT_ADD(att_pbm, lport);

            /* Set op to ADD if port is non-existing */
            if (pre_pport == -1) {
                pr->op = BCMI_XGS5_PORT_RESOURCE_OP_ADD;
            } else {
                if ((pport != pre_pport) || (iport != pre_iport)) {
                    pr->op = BCMI_XGS5_PORT_RESOURCE_OP_REMAP;
                    SOC_PBMP_PORT_ADD(remap_pbm, lport);
                } else {
                    bcm_port_resource_t_init(&cr);
                    BCM_IF_ERROR_RETURN
                        (bcm_esw_port_resource_get(unit, lport, &cr));
#if defined(PORTMOD_SUPPORT) && defined(PORTMOD_PM8X50_SUPPORT)
                    if (pm_type == portmodDispatchTypePm8x50) {
                        /* On PM8x50, number of lanes can be changed during AN.
                         * However, BCMI_XGS5_PORT_RESOURCE_OP_LANES should not
                         * be set for AN ports.
                         */
                        BCM_IF_ERROR_RETURN
                            (bcm_esw_port_autoneg_get(unit, lport, &autoneg));
                    }
                    if (!autoneg)
#endif
                    {
                        if (r->lanes != cr.lanes) {
                            pr->op |= BCMI_XGS5_PORT_RESOURCE_OP_LANES;
                            SOC_PBMP_PORT_ADD(lane_pbm, lport);
                        }
                    }
                    if (r->speed != si->port_init_speed[lport] ||
                        r->speed != cr.speed) {
                        pr->op |= BCMI_XGS5_PORT_RESOURCE_OP_SPEED;
                        SOC_PBMP_PORT_ADD(speed_pbm, lport);
                    }
#if defined(PORTMOD_SUPPORT) && defined(PORTMOD_PM8X50_SUPPORT)
                    if (pm_type == portmodDispatchTypePm8x50) {
                        /* Always set the Speed Op for PM8x50 if there's a port
                         * passed in. There may have been a portmod VCO change
                         * operation that requires the port to be reconfigured
                         */
                        pr->op |= BCMI_XGS5_PORT_RESOURCE_OP_SPEED;
                        SOC_PBMP_PORT_ADD(speed_pbm, lport);
                    }
#endif

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
                    if (soc_feature(unit, soc_feature_channelized_switching)) {
                        if ((r->num_subports != si->port_num_subport[lport]) ||
                            (r->line_card != si->port_lc_mapping[lport])) {
                            pr->op |= BCMI_XGS5_PORT_RESOURCE_OP_CHL;
                            SOC_PBMP_PORT_ADD(chl_pbm, lport);
                        }
                    }
#endif /*BCM_CHANNELIZED_SWITCHING_SUPPORT */

                    if (r->encap != cr.encap) {
                        if (BCMI_PORT_DRV_DEV_INFO(unit)->encap_change_flex) {
                            pr->op |= BCMI_XGS5_PORT_RESOURCE_OP_ENCAP_FLEX;
                            SOC_PBMP_PORT_ADD(encap_pbm, lport);
                        } else {
                            pr->op |= BCMI_XGS5_PORT_RESOURCE_OP_ENCAP;
                        }
                    }
                    if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, lport)) {
                        pr->op |= BCMI_XGS5_PORT_RESOURCE_OP_ACTIVE;
                    }

                    /* Only TDM calendar change. Currently only between 25G port use 25G
                                   * TDM calendar and 25G port use 50G TDM.
                                   */
                    if ((SOC_PBMP_MEMBER(si->pbm_25g_use_50g_tdm, lport) &&
                        !(pr->flags & BCM_PORT_RESOURCE_25G_USE_50G_CALENDAR)
                        && (r->speed == 25000))
                        ||
                        (!SOC_PBMP_MEMBER(si->pbm_25g_use_50g_tdm, lport) &&
                        (cr.speed == 25000) &&
                        (pr->flags & BCM_PORT_RESOURCE_25G_USE_50G_CALENDAR)))
                        {
                        pr->op |= BCMI_XGS5_PORT_RESOURCE_OP_CALENDAR;
                    }
                }
            }
        } else {
            /* Entries for port-detach */
            /* Lane_num/speed/encap update don't require delete-port */
            if (!SOC_PBMP_MEMBER(att_pbm, lport)) {
                pr->op = BCMI_XGS5_PORT_RESOURCE_OP_DEL;
            } else if (SOC_PBMP_MEMBER(remap_pbm, lport)) {
                pr->op = BCMI_XGS5_PORT_RESOURCE_OP_REMAP;
            } else if (SOC_PBMP_MEMBER(lane_pbm, lport)) {
                pr->op = BCMI_XGS5_PORT_RESOURCE_OP_LANES;
            } else if (SOC_PBMP_MEMBER(speed_pbm, lport)) {
                pr->op = BCMI_XGS5_PORT_RESOURCE_OP_SPEED;
            } else if (SOC_PBMP_MEMBER(encap_pbm, lport)) {
                pr->op = BCMI_XGS5_PORT_RESOURCE_OP_ENCAP_FLEX;
            } else if (SOC_PBMP_MEMBER(chl_pbm, lport)) {
                pr->op = BCMI_XGS5_PORT_RESOURCE_OP_CHL;
            } else {
                pr->op = BCMI_XGS5_PORT_RESOURCE_OP_NONE;
            }
#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
            if (soc_feature(unit, soc_feature_channelized_switching)) {
                if ((r->num_subports != si->port_num_subport[lport]) ||
                    (r->line_card != si->port_lc_mapping[lport])) {
                    pr->op |= BCMI_XGS5_PORT_RESOURCE_OP_CHL;
                }
            }
#endif /*BCM_CHANNELIZED_SWITCHING_SUPPORT */

        }
        *op |= pr->op;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_soc_schedule_state_init
 * Purpose:
 *      Construct soc_port_schedule_state_t structure which will pass to
 *      FlexPort Tier1 API as input.
 *
 * Parameters:
 *      unit                - (IN) Unit number.
 *      nport               - (IN) Number of elements in array resource.
 *      soc_resource        - (IN) SOC Port resource configuration array.
 *      op                  - (IN) Operation code
 *      port_schedule_state - (OUT) soc_port_schedule_state_t.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Assumes caller has lock.
 *      Resource is not NULL.
 *      Logical port in 'resource' is in BCM port format (non GPORT).
 */
STATIC int
_bcmi_xgs5_port_soc_schedule_state_init(int unit, int nport, int op,
                                 soc_port_resource_t *soc_resource,
                                 soc_port_schedule_state_t *port_schedule_state)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_map_type_t *in_port_map;
    soc_asf_prop_t *asf;
    int lossless, port, asf_mode, asf_prof;
    int speed, lanes, encap;
    int rv;
#ifdef BCM_TOMAHAWK3_SUPPORT
    int dpr_freq;
#endif

    lossless = soc_property_get(unit, spn_MMU_LOSSLESS,
                                    BCMI_PORT_DRV_DEV_INFO(unit)->mmu_lossless);

    /* port_schedule_state structure initialization */
    sal_memset(port_schedule_state, 0, sizeof(soc_port_schedule_state_t));

    /* Check the size, because port_schedule_state->resource is statically
     * allocated and we don't want to overflow the buffer */
    if (nport > MAX_NUM_FLEXPORTS) {
        LOG_VERBOSE(BSL_LS_BCM_PORT,
                 (BSL_META_U(unit,
                             "Number of flexports (%d) exceeds maximum for "
                             "structure soc_port_schedule_state_t (%d)\n"),
                              nport, MAX_NUM_FLEXPORTS));
        return BCM_E_INTERNAL;
    }

    port_schedule_state->nport = nport;
    sal_memcpy(port_schedule_state->resource, soc_resource,
               nport * sizeof(soc_port_resource_t));

    if (SOC_IS_FIREBOLT6(unit)) {
        port_schedule_state->frequency =
            soc_property_get(unit, spn_BCM_TDM_FREQUENCY, 0);
    } else {
        port_schedule_state->frequency = si->frequency;
    }

    port_schedule_state->bandwidth = si->bandwidth;
    port_schedule_state->io_bandwidth = si->io_bandwidth;
    port_schedule_state->lossless = lossless;
    port_schedule_state->is_flexport = TRUE;

    /* ASF Setting */
    if (soc_feature(unit, soc_feature_asf_multimode))  {

        if (BCMI_PORT_SCHEDULE_STATE_INIT_OP_ASF(op)) {
            asf = &port_schedule_state->cutthru_prop;

            asf->switch_bypass_mode = SOC_SWITCH_BYPASS_MODE(unit);

            PBMP_PORT_ITER(unit, port) {
                if (IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port) ||
                    IS_MANAGEMENT_PORT(unit, port)) {
                    continue;
                }
                BCM_IF_ERROR_RETURN
                    (bcm_esw_switch_control_port_get(unit, port,
                                                     bcmSwitchAlternateStoreForward,
                                                     &asf_mode));
                asf->asf_modes[port] = asf_mode;
            }

            asf_prof = BCMI_PORT_DRV_DEV_INFO(unit)->asf_prof_default;
            asf_prof = soc_property_get(unit, spn_ASF_MEM_PROFILE, asf_prof);

            if (!((asf_prof >= _SOC_ASF_MEM_PROFILE_NONE) &&
                  (asf_prof < _SOC_ASF_MEM_PROFILE_END))) {
                asf_prof = BCMI_PORT_DRV_DEV_INFO(unit)->asf_prof_default;
            }
            asf->asf_mem_prof = asf_prof;
        }
    }

    /* Input port mapping */
    in_port_map = &port_schedule_state->in_port_map;
    PBMP_PORT_ITER(unit, port) {
        if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
            continue;
        }

        speed = si->port_init_speed[port];
        lanes = si->port_num_lanes[port];
        if (LOG_CHECK(BSL_LS_BCM_PORT|BSL_VERBOSE)) {
            if (IS_HG_PORT(unit, port)) {
                if (soc_feature(unit, soc_feature_no_higig_plus)) {
                    encap = SOC_ENCAP_HIGIG2;
                } else {
                    encap = soc_property_port_get(unit, port,
                                                  spn_HIGIG2_HDR_MODE, 0) ?
                            SOC_ENCAP_HIGIG2 : SOC_ENCAP_HIGIG;
                }
            } else {
                encap = SOC_ENCAP_IEEE;
            }

            rv = _bcmi_xgs5_port_speed_validate(unit, port,
                                                si->port_l2p_mapping[port],
                                                lanes, encap, speed);
            if (BCM_FAILURE(rv)) {
                LOG_VERBOSE(BSL_LS_BCM_PORT,
                         (BSL_META_U(unit,
                                     "Invalid configuration for existing "
                                     "port %d: physical_port=%d, speed=%d, "
                                     "lanes=%d, encap=%d\n"),
                          port, si->port_l2p_mapping[port], speed,
                          lanes, encap));
            }
        }
        if (speed < BCMI_PORT_DRV_DEV_INFO(unit)->tdm_speed_min) {
            speed = BCMI_PORT_DRV_DEV_INFO(unit)->tdm_speed_min;
        }
        in_port_map->log_port_speed[port] = speed;
        in_port_map->port_num_lanes[port] = lanes;
#ifdef BCM_MAVERICK2_SUPPORT
        if (SOC_IS_MAVERICK2(unit) && IS_MANAGEMENT_PORT(unit, port)) {
            in_port_map->port_num_lanes[port] = 1;
        }
#endif
    }

#ifdef BCM_HURRICANE4_SUPPORT
    if (SOC_IS_HURRICANE4(unit)) {
        PBMP_MACSEC_ITER(unit, port) {
            if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
                continue;
            }

            if (si->port_l2p_mapping[port] == -1) {
                continue;
            }

            speed = si->port_init_speed[port];
            if (speed <= 0) {
                continue;
            } else {
                if (soc_feature(unit, soc_feature_untethered_otp) &&
                    !soc_feature(unit, soc_feature_xflow_macsec)) {
                    continue;
                }
            }

            lanes = si->port_num_lanes[port];
            if (speed < BCMI_PORT_DRV_DEV_INFO(unit)->tdm_speed_min) {
                speed = BCMI_PORT_DRV_DEV_INFO(unit)->tdm_speed_min;
            }

            in_port_map->log_port_speed[port] = speed;
            in_port_map->port_num_lanes[port] = lanes;
        }
    }
#endif

#ifdef BCM_TOMAHAWK3_SUPPORT
    /* DPR Frequency is stored in this unused field so that MMU can have
       PFC Optimized settings */
    if (SOC_IS_TOMAHAWK3(unit)) {
        dpr_freq = soc_property_get(unit, spn_DPR_CLOCK_FREQUENCY, -1);
        in_port_map->port_p2PBLK_inst_mapping[0] = dpr_freq;
    }
#endif

    sal_memcpy(in_port_map->port_p2l_mapping, si->port_p2l_mapping,
                sizeof(int)*SOC_MAX_NUM_PORTS);
    sal_memcpy(in_port_map->port_l2p_mapping, si->port_l2p_mapping,
                sizeof(int)*SOC_MAX_NUM_PORTS);
    sal_memcpy(in_port_map->port_p2m_mapping, si->port_p2m_mapping,
                sizeof(int)*SOC_MAX_NUM_PORTS);
    sal_memcpy(in_port_map->port_m2p_mapping, si->port_m2p_mapping,
                sizeof(int)*SOC_MAX_NUM_MMU_PORTS);
    sal_memcpy(in_port_map->port_l2i_mapping, si->port_l2i_mapping,
                sizeof(int)*SOC_MAX_NUM_PORTS);
    sal_memcpy(&in_port_map->physical_pbm, &si->physical_pbm, sizeof(pbmp_t));
    sal_memcpy(&in_port_map->hg2_pbm, &si->hg2_pbm, sizeof(pbmp_t));
    sal_memcpy(&in_port_map->management_pbm, &si->management_pbm,
                sizeof(pbmp_t));
    sal_memcpy(&in_port_map->oversub_pbm, &si->oversub_pbm, sizeof(pbmp_t));

    /* TDM info */
    if (BCMI_PORT_SCHEDULE_STATE_INIT_OP_TDM(op)) {
        if (BCMI_PORT_DRV_CALL(unit)->pre_flexport_tdm) {
            BCM_IF_ERROR_RETURN
                (BCMI_PORT_DRV_CALL(unit)->pre_flexport_tdm(unit,
                                                            port_schedule_state));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_info_post_flex
 * Purpose:
 *      Populate Post Flexport soc info.
 *
 *      Certain validations need the post FlexPort information for all
 *      the ports at the same time (mostly contained in SOC_INFO).
 *      The global SOC_INFO is updated after the validation phase so
 *      the information is not available yet.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      port_schedule_state  - (IN/OUT) soc_port_schedule_state_t.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      This function does NOT modify the global SOC_INFO SW database,
 *      the post information is returned in the local argument.
 *
 *      Assumes values in resource are valid (logical port, etc.)
 */
STATIC int
_bcmi_xgs5_port_info_post_flex(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *pr;
    int nport = port_schedule_state->nport;
    soc_port_map_type_t *post_pi = &port_schedule_state->out_port_map;
    int i;
    int logic_port, phy_port, mmu_port;

    /* Get current information */
    sal_memcpy(post_pi, &port_schedule_state->in_port_map,
                sizeof(soc_port_map_type_t));

    /* First process 'delete' mappings */
    for (i = 0, pr = port_schedule_state->resource;
         (i < nport) && (pr->physical_port == -1);
         i++, pr++) {
        logic_port = pr->logical_port;

        post_pi->log_port_speed[logic_port] = 0;
        post_pi->port_num_lanes[logic_port] = 0;
        SOC_PBMP_PORT_REMOVE(post_pi->oversub_pbm, logic_port);
        SOC_PBMP_PORT_REMOVE(post_pi->hg2_pbm, logic_port);
        SOC_PBMP_PORT_REMOVE(post_pi->physical_pbm, logic_port);

        phy_port = si->port_l2p_mapping[logic_port];
        if (phy_port == -1) {
            continue;
        }

        mmu_port = si->port_p2m_mapping[phy_port];

        post_pi->port_l2p_mapping[logic_port] = -1;
        post_pi->port_p2l_mapping[phy_port] = -1;
        post_pi->port_p2m_mapping[phy_port] = -1;
        post_pi->port_m2p_mapping[mmu_port] = -1;
        post_pi->port_l2i_mapping[logic_port] = -1;
    }

    /* Continue with 'add' mappings in rest of array */
    for (; i < nport; i++, pr++) {
        logic_port = pr->logical_port;
        phy_port = pr->physical_port;
        mmu_port = pr->mmu_port;

        post_pi->log_port_speed[logic_port] = pr->speed;
        post_pi->port_num_lanes[logic_port] = pr->num_lanes;
        post_pi->port_l2p_mapping[logic_port] = phy_port;
        post_pi->port_p2l_mapping[phy_port] = logic_port;
        post_pi->port_p2m_mapping[phy_port] = mmu_port;
        post_pi->port_m2p_mapping[mmu_port] = phy_port;
        post_pi->port_l2i_mapping[logic_port] = pr->idb_port;

        if (pr->oversub) {
            SOC_PBMP_PORT_ADD(post_pi->oversub_pbm, logic_port);
        } else {
            SOC_PBMP_PORT_REMOVE(post_pi->oversub_pbm, logic_port);
        }
        if (pr->encap == _SHR_PORT_ENCAP_HIGIG2) {
            SOC_PBMP_PORT_ADD(post_pi->hg2_pbm, logic_port);
        } else {
            SOC_PBMP_PORT_REMOVE(post_pi->hg2_pbm, logic_port);
        }

        /* physical_pbm used to store the port bitmap of 25G ports using 50G
               * TDM calendar.
               */
        if (pr->flags & BCM_PORT_RESOURCE_25G_USE_50G_CALENDAR) {
            SOC_PBMP_PORT_ADD(post_pi->physical_pbm, logic_port);
        }
    }

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "\n--- SOC INFO Post FlexPort Data ---\n")));
    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "Index L2P   Speed  Encap Ovs\n")));
    for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
        if (post_pi->port_l2p_mapping[i] == -1) {
            continue;
        }
        /* coverity[overrun-local] */
        LOG_VERBOSE(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit,
                                " %3d  %3d   %6d   %3s   %1d\n"),
                     i,
                     post_pi->port_l2p_mapping[i],
                     post_pi->log_port_speed[i],
                     SOC_PBMP_MEMBER(post_pi->hg2_pbm, i) ? "HG" : "!HG",
                     SOC_PBMP_MEMBER(post_pi->oversub_pbm, i) ? 1 : 0));
    }
    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "\n")));

    return SOC_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_info_post_flex_clear
 * Purpose:
 *      Clear Post Flexport soc info in port_schedule_state.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      port_schedule_state  - (IN/OUT) soc_port_schedule_state_t.
 * Returns:
 *
 * Note:
 */
STATIC void
_bcmi_xgs5_port_info_post_flex_clear(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    sal_memset(&port_schedule_state->out_port_map, 0,
                sizeof(soc_port_map_type_t));
}
/*
 * Function:
 *      _bcmi_xgs5_port_linkscan_mode_validate
 * Purpose:
 *      Check logical ports to be 'detached' do not have linkscan mode set.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port_schedule_state - (IN) soc_port_schedule_state_t.
 * Returns:
 *      SOC_E_XXX
 * Note:
 */
STATIC int
_bcmi_xgs5_port_linkscan_mode_validate(int unit,
                                soc_port_schedule_state_t *port_schedule_state)
{
    soc_port_resource_t *pr;
    int nport = port_schedule_state->nport;
    int i, rv;

    for (i = 0, pr = port_schedule_state->resource; i < nport; i++, pr++) {
        if (pr->physical_port != -1) {
            continue;
        }

        if (pr->op & (BCMI_XGS5_PORT_RESOURCE_OP_DEL |
                      BCMI_XGS5_PORT_RESOURCE_OP_REMAP)) {
            rv = bcm_esw_linkscan_enable_port_get(unit, pr->logical_port);
            if (rv == BCM_E_PORT) {
                return rv;
            }
            if (rv != BCM_E_DISABLED) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "Linkscan mode needs to be "
                                      "disabled on ports to be detached, "
                                      "port=%d\n"),
                           pr->logical_port));
                return BCM_E_BUSY;
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_range_validate
 * Purpose:
 *      Check logical port and physical port are in same pipe
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port_schedule_state - (IN) soc_port_schedule_state_t.
 * Returns:
 *      SOC_E_XXX
 * Note:
 */
STATIC int
_bcmi_xgs5_port_range_validate(int unit,
                                soc_port_schedule_state_t *port_schedule_state)
{
    soc_port_resource_t *pr;
    int nport = port_schedule_state->nport;
    bcmi_xgs5_dev_info_t *dev_info = BCMI_PORT_DRV_DEV_INFO(unit);
    bcmi_xgs5_phy_port_t *phy_info;
    int i;

    if (!dev_info->pipe_bound) {
        return BCM_E_NONE;
    }

    /* Check logical port and physical port are in same pipe */
    for (i = 0, pr = port_schedule_state->resource; i < nport; i++, pr++) {
        if (pr->physical_port == -1) {
            continue;
        }

        phy_info = &BCMI_PORT_DRV_PHY_INFO(unit, pr->physical_port);
        if (pr->pipe != phy_info->pipe) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Physical port %d and Logical port %d "
                                  "are not in same pipe\n"),
                       pr->physical_port, pr->logical_port));
            return BCM_E_PORT;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_logic_ports_max_validate
 * Purpose:
 *      Check that number of logical ports do not exceed maximum allowed
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port_schedule_state - (IN) soc_port_schedule_state_t.
 * Returns:
 *      SOC_E_XXX
 * Note:
 */
STATIC int
_bcmi_xgs5_logic_ports_max_validate(int unit,
                                soc_port_schedule_state_t *port_schedule_state)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *pr;
    int nport = port_schedule_state->nport;
    int num_ports[SOC_MAX_NUM_PIPES];
    int i;

    /* Get current num of logical ports per each pipe */
    for (i = 0; i < SOC_MAX_NUM_PIPES; i++) {
        SOC_PBMP_COUNT(si->pipe_pbm[i], num_ports[i]);
    }

    /* Calculate num of logical ports after flexport */
    for (i = 0, pr = port_schedule_state->resource; i < nport; i++, pr++) {
        if (pr->op & BCMI_XGS5_PORT_RESOURCE_OP_DEL) {
            num_ports[pr->pipe]--;
        } else if (pr->op & BCMI_XGS5_PORT_RESOURCE_OP_ADD) {
            num_ports[pr->pipe]--;
        }
    }

    for (i = 0; i < SOC_MAX_NUM_PIPES; i++) {
        if (num_ports[i] > BCMI_PORT_DRV_DEV_INFO(unit)->ports_pipe_max[i]) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Number of logical ports exceeds max "
                                  "allowed: pipe=%d count=%d max_count=%d\n"),
                       i, num_ports[i],
                       BCMI_PORT_DRV_DEV_INFO(unit)->ports_pipe_max[i]));
            return BCM_E_PARAM;
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_mapping_validate
 * Purpose:
 *      Validate:
 *      - Port numbers are available and mappings are not replicated.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port_schedule_state - (IN) soc_port_schedule_state_t.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      - Resource is not NULL.
 *      - Assumes array order is correct
 *        ('delete' operations are first in array).
 */
STATIC int
_bcmi_xgs5_port_mapping_validate(int unit,
                                soc_port_schedule_state_t *port_schedule_state)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *pr;
    int nport = port_schedule_state->nport;
    pbmp_t logic_pbmp;
    uint8 lane_taken[SOC_MAX_NUM_PORTS];
    int phy_port, logic_port, lane, i;

    SOC_PBMP_CLEAR(logic_pbmp);
    sal_memset(lane_taken, 0, sizeof(lane_taken));

    /* Get current port assignment */
    for (phy_port = 0; phy_port < SOC_MAX_NUM_PORTS; phy_port++) {
        if (si->port_p2l_mapping[phy_port] == -1) {
            continue;
        }

#ifdef BCM_TRIDENT3_SUPPORT
        if (SOC_IS_TRIDENT3(unit)) {
            if ((phy_port == 128) && SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), 130)) {
                continue;
            }
        }
#endif

        lane_taken[phy_port] = 1;
        logic_port = si->port_p2l_mapping[phy_port];
        SOC_PBMP_PORT_ADD(logic_pbmp, logic_port);

        for (lane = 1; lane < si->port_num_lanes[logic_port]; lane++) {
            phy_port++;
            if (phy_port >= SOC_MAX_NUM_PORTS) {
                break;
            }
            lane_taken[phy_port] = 1;
        }
    }

    /* First 'delete' mappings */
    for (i = 0, pr = port_schedule_state->resource;
         (i < nport) && (pr->physical_port == -1);
         i++, pr++) {
        phy_port = si->port_l2p_mapping[pr->logical_port];
        if (phy_port == -1) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Logical port %d is not currently mapped\n"),
                       pr->logical_port));
            return SOC_E_PARAM;
        }

        if (pr->flags & SOC_PORT_RESOURCE_I_MAP) {
            continue;
        }

        lane_taken[phy_port] = 0;
        SOC_PBMP_PORT_REMOVE(logic_pbmp, pr->logical_port);

        for (lane = 1; lane < si->port_num_lanes[pr->logical_port];
             lane++) {
            phy_port++;
            lane_taken[phy_port] = 0;
        }
    }

    /* Continue with 'add' mappings in rest of array */
    for (; i < nport; i++, pr++) {
        if (pr->flags & SOC_PORT_RESOURCE_I_MAP) {
            continue;
        }
        if (pr->op & (BCMI_XGS5_PORT_RESOURCE_OP_ADD |
                        BCMI_XGS5_PORT_RESOURCE_OP_LANES |
                        BCMI_XGS5_PORT_RESOURCE_OP_REMAP)) {
            /* Check that port number is 'available' */
            if (SOC_PBMP_MEMBER(logic_pbmp, pr->logical_port)) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "Logical port %d is already mapped\n"),
                           pr->logical_port));
                return BCM_E_BUSY;
            }

            for (lane = 0; lane < pr->num_lanes; lane++) {
                phy_port = pr->physical_port + lane;
                if (lane_taken[phy_port]) {
                    LOG_ERROR(BSL_LS_BCM_PORT,
                              (BSL_META_U(unit,
                                          "Physical port %d is already "
                                          "taken by other port\n"),
                               phy_port));
                    return BCM_E_BUSY;
                }
            }

            SOC_PBMP_PORT_ADD(logic_pbmp, pr->logical_port);
            for (lane = 0; lane < pr->num_lanes; lane++) {
                phy_port = pr->physical_port + lane;
                lane_taken[phy_port] = 1;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_flex_restriction_mix_lr_ovs_dev
 * Purpose:
 *       Port configurations with a mixture of line-rate and oversub regular
 *       ports are not supported.
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port_schedule_state - (IN) soc_port_schedule_state_t.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcmi_xgs5_port_flex_restriction_mix_lr_ovs_dev(int unit,
                                 soc_port_schedule_state_t *port_schedule_state)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *pr;
    int nport = port_schedule_state->nport;
    int oversub_mode;
    int i;

    oversub_mode = si->oversub_mode;

    for (i = 0, pr = port_schedule_state->resource; i < nport; i++, pr++) {
        if (pr->physical_port == -1) {
            continue;
        }
        if (oversub_mode ^ pr->oversub) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "-- Restriction: Port configurations "
                                  "with a mixture of line-rate and oversub "
                                  "ports are not supported\n")));
            return BCM_E_PARAM;
        }
    }

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "-- Flexport Restriction mix_lr_ovs_dev: PASS\n")));
    return BCM_E_NONE;
}

#if defined(BCM_TRIDENT3_SUPPORT)
/*
 * Function:
 *      _bcmi_xgs5_port_flex_restriction_pipe_lr_slot
 * Purpose:
 *       FlexPort cannot change the total calender length available per pipeline to
 *       linerate ports.
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port_schedule_state - (IN) soc_port_schedule_state_t.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcmi_xgs5_port_flex_restriction_pipe_lr_slot(int unit,
                                 soc_port_schedule_state_t *port_schedule_state)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *pr;
    int nport = port_schedule_state->nport;
    int callen[SOC_MAX_NUM_PIPES] = {0};
    int i, pipe, logic_port, speed, tdm_speed_min;
    int eth = 1;
    soc_port_map_type_t *post_pi = &port_schedule_state->out_port_map;
    uint32 cal = 0;

    tdm_speed_min = BCMI_PORT_DRV_DEV_INFO(unit)->tdm_pipe_lr_bw_limit_checker_speed_min;

    if (SOC_PBMP_NOT_NULL(post_pi->hg2_pbm)) {
        eth = 0;
    }
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        callen[pipe] = 0;

        SOC_PBMP_ITER(si->pipe_pbm[pipe], logic_port) {
            if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, logic_port) ||
                SOC_PBMP_MEMBER(si->oversub_pbm, logic_port)) {
                continue;
            }
            if (IS_CPU_PORT(unit, logic_port) ||
                IS_MANAGEMENT_PORT(unit, logic_port) ||
                IS_LB_PORT(unit, logic_port)) {
                continue;
            }

            speed = si->port_init_speed[logic_port];
            if (speed < tdm_speed_min) {
                speed = tdm_speed_min;
            }
            callen[pipe] += tdm_td3_cmn_get_speed_slots(speed);
        }
    }

    for (i = 0, pr = port_schedule_state->resource; i < nport; i++, pr++) {
        if (pr->oversub == 1) {
            continue;
        }
        if (pr->physical_port == -1 &&
            pr->op & (BCMI_XGS5_PORT_RESOURCE_OP_DEL |
                      BCMI_XGS5_PORT_RESOURCE_OP_REMAP)) {
            pipe = si->port_pipe[pr->logical_port];
            speed = si->port_init_speed[pr->logical_port];
            if (speed < tdm_speed_min) {
                speed = tdm_speed_min;
            }
            callen[pipe] -= tdm_td3_cmn_get_speed_slots(speed);
        } else if (pr->physical_port != -1 &&
                   pr->op & (BCMI_XGS5_PORT_RESOURCE_OP_ADD|
                             BCMI_XGS5_PORT_RESOURCE_OP_REMAP |
                             BCMI_XGS5_PORT_RESOURCE_OP_ACTIVE)) {
            speed = pr->speed;
            if (speed < tdm_speed_min) {
                speed = tdm_speed_min;
            }
            callen[pr->pipe] += tdm_td3_cmn_get_speed_slots(speed);
        } else if (pr->physical_port != -1 &&
                   pr->op & (BCMI_XGS5_PORT_RESOURCE_OP_LANES|
                             BCMI_XGS5_PORT_RESOURCE_OP_ENCAP_FLEX|
                             BCMI_XGS5_PORT_RESOURCE_OP_SPEED)) {
            pipe = si->port_pipe[pr->logical_port];
            speed = si->port_init_speed[pr->logical_port];
            if (speed < tdm_speed_min) {
                speed = tdm_speed_min;
            }
            callen[pipe] -= tdm_td3_cmn_get_speed_slots(speed);
            speed = pr->speed;
            if (speed < tdm_speed_min) {
                speed = tdm_speed_min;
            }
            callen[pr->pipe] += tdm_td3_cmn_get_speed_slots(speed);
        }
    }
    if (SOC_IS_TRIDENT3(unit)) {
        soc_td3_max_lr_cal_len(unit, &cal, eth);
    }
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        if (callen[pipe] > cal) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "-- Restriction: FlexPort requesting more calender slots"
                                  "per pipeline than the available slots per pipe."
                                  "pipe=%d slots req=%d max_slots=%d\n"),
                       pipe, callen[pipe],cal));
            return BCM_E_PARAM;
        }
    }

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                         "-- Flexport restriction pipe_lr max calender length: PASS\n")));

    return BCM_E_NONE;

}
#endif

/*
 * Function:
 *      _bcmi_xgs5_port_flex_restriction_pipe_lr_bandwidth
 * Purpose:
 *       FlexPort cannot change the total throughput available per pipeline to
 *       linerate ports.
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port_schedule_state - (IN) soc_port_schedule_state_t.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcmi_xgs5_port_flex_restriction_pipe_lr_bandwidth(int unit,
                                 soc_port_schedule_state_t *port_schedule_state)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *pr;
    int nport = port_schedule_state->nport;
    int bandwidth[SOC_MAX_NUM_PIPES] = {0};
    int i, pipe, logic_port, speed, tdm_speed_min;

    tdm_speed_min = BCMI_PORT_DRV_DEV_INFO(unit)->tdm_pipe_lr_bw_limit_checker_speed_min;

    /* Get current bandwidth of line rate ports per pipe */
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        bandwidth[pipe] = 0;

        SOC_PBMP_ITER(si->pipe_pbm[pipe], logic_port) {
            if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, logic_port) ||
                SOC_PBMP_MEMBER(si->oversub_pbm, logic_port)) {
                continue;
            }
            if (IS_CPU_PORT(unit, logic_port) ||
                IS_MANAGEMENT_PORT(unit, logic_port) ||
                IS_LB_PORT(unit, logic_port)) {
                continue;
            }

            speed = si->port_init_speed[logic_port];
            if (speed < tdm_speed_min) {
                speed = tdm_speed_min;
            }
            bandwidth[pipe] += speed;
        }
    }

    for (i = 0, pr = port_schedule_state->resource; i < nport; i++, pr++) {
        if (pr->oversub == 1) {
            continue;
        }
        if (pr->physical_port == -1 &&
            pr->op & (BCMI_XGS5_PORT_RESOURCE_OP_DEL |
                      BCMI_XGS5_PORT_RESOURCE_OP_REMAP)) {
            pipe = si->port_pipe[pr->logical_port];
            speed = si->port_init_speed[pr->logical_port];
            if (speed < tdm_speed_min) {
                speed = tdm_speed_min;
            }
            bandwidth[pipe] -= speed;
        } else if (pr->physical_port != -1 &&
                   pr->op & (BCMI_XGS5_PORT_RESOURCE_OP_ADD|
                             BCMI_XGS5_PORT_RESOURCE_OP_REMAP |
                             BCMI_XGS5_PORT_RESOURCE_OP_ACTIVE)) {

            speed = pr->speed;
            if (speed < tdm_speed_min) {
                speed = tdm_speed_min;
            }
            bandwidth[pr->pipe] += speed;
        } else if (pr->physical_port != -1 &&
                   pr->op & (BCMI_XGS5_PORT_RESOURCE_OP_LANES|
                             BCMI_XGS5_PORT_RESOURCE_OP_ENCAP_FLEX|
                             BCMI_XGS5_PORT_RESOURCE_OP_SPEED)) {
            pipe = si->port_pipe[pr->logical_port];
            speed = si->port_init_speed[pr->logical_port];
            if (speed < tdm_speed_min) {
                speed = tdm_speed_min;
            }
            bandwidth[pipe] -= speed;
            speed = pr->speed;
            if (speed < tdm_speed_min) {
                speed = tdm_speed_min;
            }
            bandwidth[pr->pipe] += speed;
        }
    }

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        if (bandwidth[pipe] > BCMI_PORT_DRV_DEV_INFO(unit)->pipe_lr_bw) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "-- Restriction: FlexPort cannot change "
                                  "the total throughput available per "
                                  "pipeline to linerate ports. "
                                  "pipe=%d bandwidth=%d max_bandwidth=%d\n"),
                       pipe, bandwidth[pipe],
                       BCMI_PORT_DRV_DEV_INFO(unit)->pipe_lr_bw));
            return BCM_E_PARAM;
        }
    }

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                         "-- Flexport Restriction pipe_lr_bandwidth: PASS\n")));

#if defined(BCM_TRIDENT3_SUPPORT)
    if (SOC_IS_TRIDENT3(unit)) {
        BCM_IF_ERROR_RETURN
             (_bcmi_xgs5_port_flex_restriction_pipe_lr_slot(unit,
                                                port_schedule_state));
    }
#endif
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_flex_restriction_speed_class
 * Purpose:
 *       No port configuration with more than Maximum port speed classes are
 *       supported.
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port_schedule_state - (IN) soc_port_schedule_state_t.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcmi_xgs5_port_flex_restriction_speed_class(int unit,
                                 soc_port_schedule_state_t *port_schedule_state)
{
    uint32 speed_mask, mask, speed_class_num;
    int i, speed;
    soc_port_map_type_t *post_pi = &port_schedule_state->out_port_map;
    bcmi_xgs5_speed_class_t *speed_class;
#ifdef PORTMOD_PM8X50_SUPPORT
    int speed_200 = 0, speed_400 = 0;
#endif

    mask = 0;
    speed_mask = 0;
    speed_class_num = 0;

    for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
        /* coverity[overrun-local] */
        if (post_pi->log_port_speed[i] <= 0 || IS_MANAGEMENT_PORT(unit,i)) {
            continue;
        }
        speed = soc_port_speed_higig2eth(post_pi->log_port_speed[i]);
        /* <=10G speeds in one speed class */
        speed = speed < 10000 ? 10000 : speed;
#ifdef PORTMOD_PM8X50_SUPPORT
        if (speed == 400000) {
            speed_400 = 1;
        } else if (speed == 200000) {
            speed_200 = 1;
        } else
#endif
        {
        mask = SOC_PA_SPEED(speed);

        if (!(speed_mask & mask)) {
            speed_mask |= mask;
            speed_class_num++;
        }
    }
    }

#ifdef PORTMOD_PM8X50_SUPPORT
    speed_class_num += speed_200;
    speed_class_num += speed_400;
#endif

    speed_class = &BCMI_PORT_DRV_DEV_INFO(unit)->speed_class;

    if (speed_class_num > speed_class->speed_class_num) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "-- Restriction: No port configuration with more "
                              "than %d port speed classes are supported. "
                              "Speed class after flexport is %d\n"),
                   speed_class->speed_class_num,
                   speed_class_num));
        return BCM_E_PARAM;
    }

    if (speed_class->no_mix_speed_mask &&
        ((speed_mask & speed_class->no_mix_speed_mask) ==
         speed_class->no_mix_speed_mask)){
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "-- Restriction: Not supported combination of "
                              "speed classes. \n")));
        return BCM_E_PARAM;
    }

    if (BCMI_PORT_DRV_CALL(unit)->speed_mix_validate) {
        BCM_IF_ERROR_RETURN
            (BCMI_PORT_DRV_CALL(unit)->speed_mix_validate(unit, speed_mask));
    }

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                         "-- Flexport Restriction speed class: PASS\n")));
    return BCM_E_NONE;
}
/*
 * Function:
 *      _bcmi_xgs5_port_flex_restriction_vco_validation
 * Purpose:
 *       Validate the following:
 *       1. FEC, Link Training, Phy Lane Config in flex situations
 *       2. VCO configuration is valid for Port macros
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port_schedule_state - (IN) soc_port_schedule_state_t.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcmi_xgs5_port_flex_restriction_vco_validation(int unit,
                                 soc_port_schedule_state_t *port_schedule_state)
{
    int i;
    int autoneg;
    int logical_port;
    int existing_physical_port;
    int all_autoneg = 1;
    soc_port_resource_t *resource;
    
    for (i = 0; i < port_schedule_state->nport; i++) {
        resource = &port_schedule_state->resource[i];

        /* Skip ports being deleted */
        if (resource->physical_port == -1) {
            continue;
        }

        logical_port = resource->logical_port;
        existing_physical_port = SOC_INFO(unit).port_l2p_mapping[logical_port];

        /* If the passed port already exists, and the lanes aren't changing,
         * check it for autoneg.  We don't need to validate VCO if autoneg.
         * resource_speed_set disables autoneg, so this code should only
         * execute and skip VCO validation on a port update
         */
        if (existing_physical_port != -1 &&
            resource->op == BCMI_XGS5_PORT_RESOURCE_OP_SPEED) {

            BCM_IF_ERROR_RETURN(bcm_esw_port_autoneg_get(unit,
                                    resource->logical_port,
                                    &autoneg));

            /* We don't need to do validation if the port is in autoneg */
            if (autoneg == 1) {
                continue;
            }
        }

        /* Not all of the ports in port_schedule_state are autoneg, so
         * port_schedule_state will need to be validated
         */
        all_autoneg = 0;

        /* Validate FEC */
        if ((resource->fec_type >= socPortPhyFecCount) ||
                (resource->fec_type < socPortPhyFecNone)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "-- Restriction: All ports must have a valid "
                                  "FEC.\n")));

            return BCM_E_PARAM;
        }

        /* Validate Link Training */
        if ((resource->link_training > 1) ||
                (resource->link_training < 0)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "-- Restriction: All ports must have "
                                  "Link Training enabled or disabled.\n")));

            return BCM_E_PARAM;
        }

        /* Phy Lane Config is validated inside Portmod since it's a bitmap
         * and there isn't a clear range to check at this layer*/
    }

    /* This info is filled in later, since it needs to be calculated. We'll use
     * it later in PM VCO calculation
     */
    soc_esw_portctrl_pm_vco_store_clear(unit);

    /* all the ports in port_schedule_state are autoneg, so we don't want to
     * do any validation
     */
    if (all_autoneg == 1) {
        return BCM_E_NONE;
    } else {
        return soc_esw_portctrl_flex_vco_validation(unit, port_schedule_state);
    }
}

/*
 * Function:
 *      _bcmi_xgs5_port_flex_restriction_pm_sister_ports
 * Purpose:
 *       Validate sister ports:
 *       1. All active ports in an oversub Port Macro must be the same speed
 *       2. All ports in a port macro must be derived from a common VCO freq
 *       3. Oversub and Line rate ports cannot coexist in one port macro
 *       4. Ethernet and HiGig ports cannot coexist in one port macro
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port_schedule_state - (IN) soc_port_schedule_state_t.
 *      mask     - (IN) Restriction mask
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcmi_xgs5_port_flex_restriction_pm_sister_ports(int unit,
                                 soc_port_schedule_state_t *port_schedule_state,
                                 uint32 mask)
{
    soc_port_resource_t *pr;
    soc_port_map_type_t *post_pi = &port_schedule_state->out_port_map;
    int nport, check_ovs_lr, check_ovs_mix_speed, check_pll, check_mix_encap, check_hg_mix;
    int *sister_ports, len;
    int logic_port, phy_port, blk, bindex, btype_idx, first_p, blktype;
    int speed, first_speed, num_lanes, encap, first_encap, ovs, first_ovs, check_lr_mix_speed;
    int mixed_sister_25_50_en, mixed_sister_25_10_en;
    SHR_BITDCL blk_bmp[_SHR_BITDCLSIZE(SOC_MAX_NUM_BLKS)];
    uint32 pll_div, first_pll_div;
    int i, offset, rv;
    uint32 speed_mask, mix_speed_mask, pm_mixed_pbm, pm_used_pbm;
    uint32 mix_10G_1000MB_speed_mask, mix_25G_10G_1000MB_speed_mask;
    soc_info_t *si = &SOC_INFO(unit);

    check_ovs_lr =
        mask & BCMI_XGS5_FLEXPORT_RESTRICTION_MIX_LR_OVS_PM ? 1: 0;

    check_ovs_mix_speed =
        mask & BCMI_XGS5_FLEXPORT_RESTRICTION_PM_OVS_MIX_SISTER ? 1 : 0;

    check_pll =
        mask & BCMI_XGS5_FLEXPORT_RESTRICTION_PM_SINGLE_PLL ? 1 : 0;

    check_mix_encap =
        mask & BCMI_XGS5_FLEXPORT_RESTRICTION_PM_MIX_ETH_HIGIG ? 1 : 0;

    check_hg_mix =
        mask & BCMI_XGS5_FLEXPORT_RESTRICTION_PM_HG_MIX_SPEED ? 1 : 0;

    check_lr_mix_speed =
        mask & BCMI_XGS5_FLEXPORT_RESTRICTION_PM_LR_MIX_SISTER ? 1 : 0;

    mix_speed_mask = SOC_PA_SPEED_50GB | SOC_PA_SPEED_25GB | SOC_PA_SPEED_1000MB;
    mix_10G_1000MB_speed_mask = SOC_PA_SPEED_10GB| SOC_PA_SPEED_1000MB | SOC_PA_SPEED_2500MB | SOC_PA_SPEED_5000MB;
    mix_25G_10G_1000MB_speed_mask = SOC_PA_SPEED_25GB | SOC_PA_SPEED_10GB| SOC_PA_SPEED_1000MB;

    if (!check_ovs_lr && !check_ovs_mix_speed && !check_pll &&
        !check_hg_mix && !check_lr_mix_speed &&
        !check_mix_encap) {
        return BCM_E_NONE;
    }

    /* Alloc sister_ports[SOC_MAX_NUM_BLKS][BCMI_XGS5_PORTS_PER_PBLK] */
    len = SOC_MAX_NUM_BLKS * BCMI_XGS5_PORTS_PER_PBLK * sizeof(int);
    sister_ports = sal_alloc(len, "sister_ports");
    if (sister_ports == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(sister_ports, -1, len);

    /* Get bitmap for block_id that have port flexing */
    nport = port_schedule_state->nport;
    SHR_BITCLR_RANGE(blk_bmp, 0, SOC_MAX_NUM_BLKS);
    for (i = 0, pr = port_schedule_state->resource; i < nport; i++, pr++) {
        if (pr->physical_port == -1) {
            continue;
        }
        phy_port = pr->physical_port;
        for (btype_idx = 0; btype_idx < SOC_DRIVER(unit)->port_num_blktype;
             btype_idx++) {
            blk = SOC_PORT_IDX_BLOCK(unit, phy_port, btype_idx);
            bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, btype_idx);
            if ((blk < 0) || (bindex < 0)) {
                continue;
            }
            SHR_BITSET(blk_bmp, blk);
        }
    }

    /* Get sister ports for each block */
    for (phy_port = 0;
         phy_port < BCMI_PORT_DRV_DEV_INFO(unit)->phy_ports_max;
         phy_port++) {

        if (SOC_PORT_BLOCK(unit, phy_port) < 0 &&
            SOC_PORT_BINDEX(unit, phy_port) < 0) {
            break;
        }
        for (i = 0; i < SOC_DRIVER(unit)->port_num_blktype;
             i++) {
            blk = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
            bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, i);
            if ((blk < 0) || (bindex < 0)) {
                continue;
            }
            blktype = SOC_BLOCK_INFO(unit, blk).type;
            if ((blktype == SOC_BLK_XLPORT) || (blktype == SOC_BLK_CLPORT)) {
                if (bindex > BCMI_XGS5_PORTS_PER_PBLK) {
                    LOG_WARN(BSL_LS_BCM_PORT,
                             (BSL_META_U(unit,
                                         "Index of port within block(bindex) "
                                         "exceeds the maximum.\n")));
                    continue;
                }
                offset = blk * BCMI_XGS5_PORTS_PER_PBLK + bindex;
                sister_ports[offset] = phy_port;
            }
        }
    }

    mixed_sister_25_50_en = si->os_mixed_sister_25_50_enable;

    /* For HX5 25g/10g mixed support is limited to LR ports only */
    mixed_sister_25_10_en = SOC_IS_MAVERICK2(unit) ? 1 : 0;

    /* Check that sister ports (ports within a Port Macro) have the same VCO */
    SHR_BIT_ITER(blk_bmp, SOC_MAX_NUM_BLKS, blk) {
        first_p = 1;
        speed = 0;
        first_speed = 0;
        pll_div = 0;
        first_pll_div = 0;
        encap = -1;
        first_encap = -1;
        ovs = -1;
        first_ovs = -1;
        speed_mask = 0;
        pm_mixed_pbm = 0;
        pm_used_pbm = 0;

        blktype = SOC_BLOCK_INFO(unit, blk).type;
        if (!((blktype == SOC_BLK_XLPORT) || (blktype == SOC_BLK_CLPORT))) {
            continue;
        }

        for (i = 0; i < BCMI_XGS5_PORTS_PER_PBLK; i++) {
            offset = blk * BCMI_XGS5_PORTS_PER_PBLK + i;
#ifdef BCM_TRIDENT3_SUPPORT
            if (SOC_IS_TRIDENT3(unit)) {
                if ((offset == 135) && SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), 130)) {
                    continue;
                }
            }
#endif
            phy_port = sister_ports[offset];
            if (phy_port == -1) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit, "No sister ports.\n")));
                sal_free(sister_ports);
                return BCM_E_INTERNAL;
            }
            logic_port = post_pi->port_p2l_mapping[phy_port];
            if (logic_port == -1) {
                continue;
            }
            /* Skip inactive ports */
            if (post_pi->log_port_speed[logic_port] <= 0) {
                continue;
            }

            speed = post_pi->log_port_speed[logic_port];
            num_lanes = post_pi->port_num_lanes[logic_port];
            encap = SOC_PBMP_MEMBER(post_pi->hg2_pbm, logic_port) ?
                    SOC_ENCAP_HIGIG2 : SOC_ENCAP_IEEE;
            ovs = SOC_PBMP_MEMBER(post_pi->oversub_pbm, logic_port) ? 1 : 0;
            rv = soc_esw_portctrl_pll_div_get(unit, logic_port, phy_port, speed, num_lanes,
                                              encap, &pll_div);
            speed_mask |=  SOC_PA_SPEED(soc_port_speed_higig2eth(speed));
            if (BCM_FAILURE(rv)) {
                LOG_VERBOSE(BSL_LS_BCM_PORT,
                            (BSL_META_U(unit, "Failed to get PLL\n")));
                sal_free(sister_ports);
                return rv;
            }
            if (first_p) {
                first_pll_div = pll_div;
                first_encap = encap;
                first_speed = speed;
                first_ovs = ovs;
                first_p = 0;
            }
            if (ovs && mixed_sister_25_50_en) {
                speed_mask |= SOC_PA_SPEED(soc_port_speed_higig2eth(speed));
                pm_used_pbm |= 1 << ((phy_port - 1) % BCMI_XGS5_PORTS_PER_PBLK);
                if (SOC_PBMP_MEMBER(post_pi->physical_pbm, logic_port)) {
                    if (SOC_PA_SPEED(soc_port_speed_higig2eth(speed))
                        & SOC_PA_SPEED_25GB){
                        pm_mixed_pbm |=
                            1 << ((phy_port - 1) % BCMI_XGS5_PORTS_PER_PBLK);
                    } else {
                        LOG_ERROR(BSL_LS_SOC_PORT,
                          (BSL_META_U(unit,
                              "Only 25G port can with flag "
                              "BCM_PORT_RESOURCE_25G_USE_50G_CALENDAR, "
                              "port %d with speed %d can't "
                              "have flag "
                              "BCM_PORT_RESOURCE_25G_USE_50G_CALENDAR!\n"),
                              logic_port, speed));
                        sal_free(sister_ports);
                        return SOC_E_CONFIG;
                    }
                }
            }
            if (check_pll && (first_pll_div != pll_div)) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "-- Restriction: All ports in a port "
                                      "macro must be derived from a common VCO "
                                      "frequency\n")));
                sal_free(sister_ports);
                return BCM_E_PARAM;
            }
            if (check_mix_encap && (first_encap != encap)) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "-- Restriction: Ethernet and HiGig "
                                      "ports cannot coexist in one port macro"
                                      "\n")));
                sal_free(sister_ports);
                return BCM_E_PARAM;
            }
            if (check_ovs_lr && (first_ovs != ovs)) {
                LOG_ERROR(BSL_LS_BCM_PORT,
                          (BSL_META_U(unit,
                                      "-- Restriction: Oversub and Line-rate "
                                      "ports cannot coexist in one port macro"
                                      "\n")));
                sal_free(sister_ports);
                return BCM_E_PARAM;
            }
            if (ovs && check_ovs_mix_speed && (soc_port_speed_higig2eth(first_speed) !=
                                               soc_port_speed_higig2eth(speed))) {
                /* Support 25G/50G mix sister speeds in a PM.
                 * Only include 25G and 50G speeds.
                 */
                if (mixed_sister_25_50_en &&
                    (!(speed_mask & (~mix_speed_mask)))) {
                    continue;
                } else if (!(speed_mask & (~mix_10G_1000MB_speed_mask))) {
                    /* 10G and <10G is considered same speed from TDM perspective although
                     * speed mask is for mixed speed 
                     */
                    continue;
                } else if (mixed_sister_25_10_en &&
                           (!(speed_mask & (~mix_25G_10G_1000MB_speed_mask)))) {
                    /* 25G/10G/1G mix supported */
                    continue;
                } else {
                    LOG_ERROR(BSL_LS_BCM_PORT,
                              (BSL_META_U(unit,
                                          "-- Restriction: Mixed-sister oversub "
                                          "ports are not supported.\n")));
                    sal_free(sister_ports);
                    return BCM_E_PARAM;
                }
            }
            if ((encap == SOC_ENCAP_HIGIG2)  && (check_hg_mix) && (soc_port_speed_higig2eth(first_speed) !=
                                               soc_port_speed_higig2eth(speed))) {
                 LOG_ERROR(BSL_LS_BCM_PORT,
                            (BSL_META_U(unit,
                                       "-- Restriction:  different speed sub ports in HG2 "
                                       "are not supported.\n")));
                 sal_free(sister_ports);
                 return BCM_E_PARAM;
            }
        }
        if ((!ovs) && check_lr_mix_speed) {
            if (((speed_mask & SOC_PA_SPEED_25GB) && (speed_mask & SOC_PA_SPEED_10GB)) ||
                ((speed_mask & SOC_PA_SPEED_25GB) && (speed_mask & SOC_PA_SPEED_1000MB)))  {
                LOG_ERROR(BSL_LS_BCMAPI_PORT,
                          (BSL_META_U(unit,
                                       "-- Restriction: Mixed-sister Linerate "
                                       "10G/1G and 25G ports are not supported.\n")));
                sal_free(sister_ports);
                return BCM_E_PARAM;
            }
        }
        if (ovs && mixed_sister_25_50_en) {
            if (pm_mixed_pbm && (pm_used_pbm & 0xa)){
                LOG_ERROR(BSL_LS_BCMAPI_PORT,
                          (BSL_META_U(unit,
                                      "Mixed port speed configuration is wrong "
                                      "on port macro %d!\n Either more than 2 "
                                      "ports in this port macro or single lane "
                                      "ports on 2/4th lane!\n"),
                          SOC_BLOCK_NUMBER(unit, blk)));
                sal_free(sister_ports);
                return BCM_E_PARAM;
            } else if (!pm_mixed_pbm && (speed_mask == mix_speed_mask)){
                LOG_ERROR(BSL_LS_BCMAPI_PORT,
                          (BSL_META_U(unit,
                                      "Mixed port speed bitmap not set on "
                                      "25G ports of port macro %d!\n"),
                          SOC_BLOCK_NUMBER(unit, blk)));
                sal_free(sister_ports);
                return BCM_E_PARAM;
            }
        }
    }

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "-- Flexport Restriction for sister ports: "
                            "PASS\n")));
    sal_free(sister_ports);
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_flex_restriction
 * Purpose:
 *      Validate that the given FlexPort operation is valid.
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port_schedule_state - (IN) soc_port_schedule_state_t.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcmi_xgs5_port_flex_restriction(int unit,
                                 soc_port_schedule_state_t *port_schedule_state)
{
    uint32 restrict_mask;

    restrict_mask = BCMI_PORT_DRV_DEV_INFO(unit)->restriction_mask;

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "---- FLEXPORT RESTRICTION VALIDATE ----\n")));

    /*
     * Restriction:
     *   Port configurations with a mixture of line-rate and oversub regular
     *   ports are not supported.
     */
    if (restrict_mask & BCMI_XGS5_FLEXPORT_RESTRICTION_MIX_LR_OVS_DEV) {
        BCM_IF_ERROR_RETURN
            (_bcmi_xgs5_port_flex_restriction_mix_lr_ovs_dev(unit,
                                                        port_schedule_state));
    }

    /*
     * Restriction:
     *   FlexPort cannot change the total throughput available per pipeline to
     *   linerate ports.
     */
    if (restrict_mask & BCMI_XGS5_FLEXPORT_RESTRICTION_PIPE_BANDWIDTH) {
        BCM_IF_ERROR_RETURN
            (_bcmi_xgs5_port_flex_restriction_pipe_lr_bandwidth(unit,
                                                        port_schedule_state));
    }

    /*
     * Restriction:
     *   No port configuration with more than Maximum port speed classes are
     *   supported.
     */
    if (restrict_mask & BCMI_XGS5_FLEXPORT_RESTRICTION_SPEED_CLASS) {
        BCM_IF_ERROR_RETURN
            (_bcmi_xgs5_port_flex_restriction_speed_class(unit,
                                                        port_schedule_state));
    }

    if (restrict_mask & BCMI_XGS5_FLEXPORT_RESTRICTION_PM_DOUBLE_PLL) {
        BCM_IF_ERROR_RETURN
            (_bcmi_xgs5_port_flex_restriction_vco_validation(unit,
                                                       port_schedule_state));
    } else {
    /*
     * Restriction:
     *   Validate sister ports:
     *     1. All active ports in an oversub Port Macro must be the same speed
     *     2. All ports in a port macro must be derived from a common VCO freq
     *     3. Oversub and Line rate ports cannot coexist in one port macro
     *     4. Ethernet and HiGig ports cannot coexist in one port macro
     */
    BCM_IF_ERROR_RETURN
        (_bcmi_xgs5_port_flex_restriction_pm_sister_ports(unit,
                                                        port_schedule_state,
                                                        restrict_mask));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_resource_validate
 * Purpose:
 *      - Check the port mapping is available.
 *      - Validate that the given FlexPort configuration is valid.
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      resource - (IN) bcm_port_resource_t
 *      port_schedule_state - (OUT) soc_port_schedule_state_t.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcmi_xgs5_port_resource_validate(int unit,
                                 soc_port_schedule_state_t *port_schedule_state)
{
    int rv;

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "==== PORT RESOURCE VALIDATE ====\n")));

    /* Check that ports to be 'detached' do not have linkscan mode set. */
    BCM_IF_ERROR_RETURN
        (_bcmi_xgs5_port_linkscan_mode_validate(unit, port_schedule_state));

    /* Check logical port and physical port are in same pipe */
    BCM_IF_ERROR_RETURN
        (_bcmi_xgs5_port_range_validate(unit, port_schedule_state));

    /* Check that number of logical ports do not exceed maximum allowed */
    BCM_IF_ERROR_RETURN
        (_bcmi_xgs5_logic_ports_max_validate(unit, port_schedule_state));

    /* Check port mapping */
    BCM_IF_ERROR_RETURN
        (_bcmi_xgs5_port_mapping_validate(unit, port_schedule_state));

    /* Flexport restrictions */

    /* Populate Post Flexport soc info in port_schedule_state->out_port_map,
     * which will be used in the following validation. */
    BCM_IF_ERROR_RETURN
        (_bcmi_xgs5_port_info_post_flex(unit, port_schedule_state));

    if (BCMI_PORT_DRV_CALL(unit)->port_resource_validate) {
        BCM_IF_ERROR_RETURN
            (BCMI_PORT_DRV_CALL(unit)->port_resource_validate(unit,
                                                         port_schedule_state));
    }

    rv = _bcmi_xgs5_port_flex_restriction(unit, port_schedule_state);

    /* Clear the port_schedule_state->out_port_map */
    _bcmi_xgs5_port_info_post_flex_clear(unit, port_schedule_state);

    BCM_IF_ERROR_RETURN(rv);

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_pm_reset
 * Purpose:
 *      Reset newly bring up Port Macro
 *
 * Parameters:
 *      unit        - (IN) Unit number.
 *      add_bmp_pm  - (IN) Bitmap of Port Macros actived by Flexport.
 *      add_pbm_phy - (IN) Bitmap of Physical ports actived by Flexport.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcmi_xgs5_port_pm_reset(int unit, SHR_BITDCL *add_bmp_pm, pbmp_t *add_pbm_phy)
{
    soc_info_t *si = &SOC_INFO(unit);
    int blk, bindex, phy_port, local_phy_port, tmp, block, is_clport, port_idx;
    int pipe;
    soc_reg_t reg;
    uint64 rval64, val64_zero;
    uint32 addr;
    uint8 at;
    int sleep_usec = SAL_BOOT_QUICKTURN ? 500000 : 1100;
    static soc_field_t port_fields[4] = {
        PORT0f, PORT1f, PORT2f, PORT3f
    };

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "--- Port Macro Reset ---\n")));
    COMPILER_64_ZERO(val64_zero);
    SHR_BIT_ITER(add_bmp_pm, SOC_MAX_NUM_BLKS, blk) {
        block = SOC_BLOCK_INFO(unit, blk).cmic;
        is_clport = SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_CLPORT ? 1 : 0;

        /*******************************
         * Configure MAC Control
         */
        reg = is_clport ? CLPORT_MAC_CONTROLr : XLPORT_MAC_CONTROLr;
        COMPILER_64_ZERO(rval64);
        addr = soc_reg_addr_get(unit, reg, REG_PORT_ANY, 0,
                                SOC_REG_ADDR_OPTION_WRITE, &tmp, &at);
        SOC_REG_RST_VAL_GET(unit, reg, rval64);
        soc_reg64_field32_set(unit, reg, &rval64, XMAC0_RESETf, 1);
        BCM_IF_ERROR_RETURN(_soc_reg64_set(unit, block, at, addr, rval64));
        sal_udelay(10);
        soc_reg64_field32_set(unit, reg, &rval64, XMAC0_RESETf, 0);
        if (is_clport &&
            soc_feature(unit, soc_feature_clmac_16byte_interface_mode)) {
            /* Set PM interface to 16B */
            soc_reg64_field32_set(unit, reg, &rval64, SYS_16B_INTF_MODEf, 1);
        }
        BCM_IF_ERROR_RETURN(_soc_reg64_set(unit, block, at, addr, rval64));

        /*******************************
         * Reset TSC
         */
        if (soc_feature(unit, soc_feature_pm_refclk_master)) {
            /* Port Macros act as refclk master are initialized in system
             * init stage, no action during Flexport
             */
            if (SHR_BITGET(si->pm_ref_master_bitmap, blk)) {
                continue;
            }
        }

        reg = is_clport ? CLPORT_XGXS0_CTRL_REGr : XLPORT_XGXS0_CTRL_REGr;
        COMPILER_64_ZERO(rval64);
        addr = soc_reg_addr_get(unit, reg, REG_PORT_ANY, 0,
                                SOC_REG_ADDR_OPTION_WRITE, &tmp, &at);

        /* Set TSC in master mode */
        SOC_REG_RST_VAL_GET(unit, reg, rval64);
        soc_reg64_field32_set(unit, reg, &rval64, REFIN_ENf,
                              SAL_BOOT_QUICKTURN ? 0 : 1);
        BCM_IF_ERROR_RETURN(_soc_reg64_set(unit, block, at, addr, rval64));

        /* Deassert power down */
        soc_reg64_field32_set(unit, reg, &rval64, PWRDWNf, 0);
        BCM_IF_ERROR_RETURN(_soc_reg64_set(unit, block, at, addr, rval64));
        sal_usleep(sleep_usec);

        /* Reset XGXS */
        soc_reg64_field32_set(unit, reg, &rval64, RSTB_HWf, 0);
        BCM_IF_ERROR_RETURN(_soc_reg64_set(unit, block, at, addr, rval64));

        sal_usleep(sleep_usec + 10000);

        /* Bring XGXS out of reset */
        soc_reg64_field32_set(unit, reg, &rval64, RSTB_HWf, 1);
        BCM_IF_ERROR_RETURN(_soc_reg64_set(unit, block, at, addr, rval64));
        sal_usleep(sleep_usec);
    }

    for (pipe = 0; pipe < si->num_pipe; pipe++) {
        SOC_PBMP_ITER(add_pbm_phy[pipe], local_phy_port) {
            phy_port = local_phy_port +
                        BCMI_PORT_DRV_DEV_INFO(unit)->pipe_phy_port_base[pipe];

            for (port_idx = 0; port_idx < SOC_DRIVER(unit)->port_num_blktype;
                 port_idx++) {

                blk = SOC_PORT_IDX_BLOCK(unit, phy_port, port_idx);
                bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, port_idx);
                if (blk < 0) { /* End of block list of each port */
                    break;
                }
                block = SOC_BLOCK_INFO(unit, blk).cmic;
                is_clport =
                    SOC_BLOCK_INFO(unit, blk).type == SOC_BLK_CLPORT ? 1 : 0;

                /* Initialize PORT MIB counter */
                reg = is_clport ? CLPORT_MIB_RESETr : XLPORT_MIB_RESETr;
                COMPILER_64_ZERO(rval64);
                addr = soc_reg_addr_get(unit, reg, REG_PORT_ANY, 0,
                                        SOC_REG_ADDR_OPTION_WRITE, &tmp, &at);
                soc_reg64_field32_set(unit, reg, &rval64, CLR_CNTf, (1 < bindex));
                BCM_IF_ERROR_RETURN
                    (_soc_reg64_set(unit, block, at, addr, rval64));
                BCM_IF_ERROR_RETURN(_soc_reg64_set(unit, block, at, addr, val64_zero));

                /* CLPORT_MODE_REG is programmed in Flexport Tier1 API */

                reg = is_clport ? CLPORT_SOFT_RESETr : XLPORT_SOFT_RESETr;
                COMPILER_64_ZERO(rval64);
                addr = soc_reg_addr_get(unit, reg, REG_PORT_ANY, 0,
                                        SOC_REG_ADDR_OPTION_WRITE, &tmp, &at);
                /* Assert CLPORT/XLPORT soft reset */
                soc_reg64_field32_set(unit, reg, &rval64, port_fields[bindex], 1);
                BCM_IF_ERROR_RETURN
                    (_soc_reg64_set(unit, block, at, addr, rval64));
                /* De-assert CLPORT/XLPORT soft reset */
                BCM_IF_ERROR_RETURN(_soc_reg64_set(unit, block, at, addr, val64_zero));

                reg = is_clport ? CLPORT_ENABLE_REGr : XLPORT_ENABLE_REGr;
                COMPILER_64_ZERO(rval64);
                addr = soc_reg_addr_get(unit, reg, REG_PORT_ANY, 0,
                                        SOC_REG_ADDR_OPTION_NONE, &tmp, &at);
                BCM_IF_ERROR_RETURN
                    (_soc_reg64_get(unit, block, at, addr, &rval64));
                soc_reg64_field32_set(unit, reg, &rval64, port_fields[bindex], 1);
                addr = soc_reg_addr_get(unit, reg, REG_PORT_ANY, 0,
                                        SOC_REG_ADDR_OPTION_WRITE, &tmp, &at);
                BCM_IF_ERROR_RETURN
                    (_soc_reg64_set(unit, block, at, addr, rval64));

                reg = is_clport ? CLPORT_POWER_SAVEr : XLPORT_POWER_SAVEr;
                COMPILER_64_ZERO(rval64);
                addr = soc_reg_addr_get(unit, reg, REG_PORT_ANY, 0,
                                        SOC_REG_ADDR_OPTION_WRITE, &tmp, &at);
                soc_reg64_field32_set(unit, reg, &rval64, XPORT_CORE0f, 0);
                BCM_IF_ERROR_RETURN
                    (_soc_reg64_set(unit, block, at, addr, rval64));
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_ipep_reconfigure
 * Purpose:
 *      Update the port related reg/mem in IP/EP after flexport
 *
 * Parameters:
 *      unit        - (IN) Unit number.
 *      add_pbm_log - (IN) Bitmap of newly added logical ports.
 *      del_pbm_log - (IN) Bitmap of deleted logical ports.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcmi_xgs5_port_ipep_reconfigure(int unit,
                                 pbmp_t add_pbm_log,
                                 pbmp_t del_pbm_log)
{
    int logic_port, is_clport, is_cdport;
    soc_reg_t reg;
    uint32 rval;
    uint32 entry[SOC_MAX_MEM_WORDS];
    pbmp_t pbmp;
    int cntmaxsize;

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "--- Port IP/EP Reconfigure ---\n")));

    SOC_PBMP_ITER(add_pbm_log, logic_port) {
        is_clport = IS_CL_PORT(unit, logic_port);
        is_cdport = IS_CD_PORT(unit, logic_port);

        reg = INVALIDr;
        if (is_cdport) {
            reg = INVALIDr;
        } else if (is_clport) {
            reg = CLPORT_CNTMAXSIZEr;
        } else if (IS_QSGMII_PORT(unit, logic_port)) {
            reg = GPORT_CNTMAXSIZEr;
        } else {
            reg = XLPORT_CNTMAXSIZEr;
        }

        if (reg != INVALIDr) {
        rval = 0;
        cntmaxsize = is_clport ? BCMI_PORT_DRV_DEV_INFO(unit)->cntmaxsize_cl :
                                 BCMI_PORT_DRV_DEV_INFO(unit)->cntmaxsize_xl;
        soc_reg_field_set(unit, reg, &rval, CNTMAXSIZEf, cntmaxsize);
        BCM_IF_ERROR_RETURN
            (soc_reg32_set(unit, reg, logic_port, 0, rval));
        }

        reg = INVALIDr;
        if (is_cdport) {
            reg = CDPORT_FLOW_CONTROL_CONFIGr;
        } else if (is_clport) {
            reg = CLPORT_FLOW_CONTROL_CONFIGr;
        } else if (IS_QSGMII_PORT(unit, logic_port)) {
            /* No Flow control register for GPORT's */
            reg = INVALIDr;
        } else {
            reg = XLPORT_FLOW_CONTROL_CONFIGr;
        }

        rval = 0;
        if (reg != INVALIDr) {
            soc_reg_field_set(unit, reg, &rval, MERGE_MODE_ENf, 1);
            BCM_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, logic_port, 0, rval));
        }
        sal_memset(entry, 0, sizeof(egr_ing_port_entry_t));
        if (IS_HG_PORT(unit, logic_port)) {
            soc_mem_field32_set(unit, EGR_ING_PORTm, entry, PORT_TYPEf, 1);
        }
        BCM_IF_ERROR_RETURN
            (soc_mem_write(unit, EGR_ING_PORTm, MEM_BLOCK_ALL, logic_port,
                           entry));
#ifdef BCM_TRIDENT2PLUS_SUPPORT
        if (soc_feature(unit, soc_feature_egr_vlan_control_is_memory)) {
            if (soc_feature(unit, soc_feature_egr_all_profile)) {
                BCM_IF_ERROR_RETURN(bcm_esw_port_egr_lport_field_set(unit,
                        logic_port, EGR_VLAN_CONTROL_1m, REMARK_OUTER_DOT1Pf, 1));

                BCM_IF_ERROR_RETURN(bcm_esw_port_egr_lport_field_set(unit,
                        logic_port, EGR_VLAN_CONTROL_1m, VT_MISS_UNTAGf, 0));

            } else {
                SOC_IF_ERROR_RETURN(soc_mem_field32_modify(unit,
                        EGR_VLAN_CONTROL_1m, logic_port, REMARK_OUTER_DOT1Pf, 1));

                SOC_IF_ERROR_RETURN(soc_mem_field32_modify(unit,
                        EGR_VLAN_CONTROL_1m, logic_port, VT_MISS_UNTAGf, 0));
           }
        } else
#endif
        {
            reg = EGR_VLAN_CONTROL_1r;
            rval = 0;
            if (SOC_REG_FIELD_VALID(unit, reg, VT_MISS_UNTAGf)) {
            soc_reg_field_set(unit, reg, &rval, VT_MISS_UNTAGf, 0);
            }
            if (SOC_REG_FIELD_VALID(unit, reg, REMARK_OUTER_DOT1Pf)) {
            soc_reg_field_set(unit, reg, &rval, REMARK_OUTER_DOT1Pf, 1);
            }
            BCM_IF_ERROR_RETURN(WRITE_EGR_VLAN_CONTROL_1r(unit, logic_port, rval));
        }
    }

    BCM_IF_ERROR_RETURN(soc_mem_read(unit, ING_EN_EFILTER_BITMAPm,
                                     MEM_BLOCK_ANY, 0, &entry));
    soc_mem_pbmp_field_get(unit, ING_EN_EFILTER_BITMAPm, &entry,
                           BITMAPf, &pbmp);
    SOC_PBMP_REMOVE(pbmp, del_pbm_log);
    SOC_PBMP_OR(pbmp, add_pbm_log);
    soc_mem_pbmp_field_set(unit, ING_EN_EFILTER_BITMAPm, &entry,
                           BITMAPf, &pbmp);
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, ING_EN_EFILTER_BITMAPm,
                                      MEM_BLOCK_ANY, 0, &entry));

    return BCM_E_NONE;
}

#if 0
/*
 * Function:
 *      _bcmi_xgs5_port_soc_info_ptype_update
 * Purpose:
 *      Update the SOC_INFO port type bitmaps and block information.
 * Parameters:
 *      unit                    - (IN) Unit number.
 *      port_schedule_state     - (IN) port_schedule_state
 *
 *
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Assumes linkscan is paused, COUNTER_LOCK and SOC_CONTROL_LOCK
 *      locks are taken.
 */
STATIC int
_bcmi_xgs5_port_soc_info_ptype_update(int unit)
{
    soc_info_t *si = &SOC_INFO(unit);
    int logic_port, phy_port, port_idx, max_port;
    int blk, bindex;

    /*
     * Update ptype information
     */
#define RECONFIGURE_PORT_TYPE_INFO(ptype)                   \
    si->ptype.num = 0;                                      \
    si->ptype.min = si->ptype.max = -1;                     \
    PBMP_ITER(si->ptype.bitmap, logic_port) {               \
        si->port_offset[logic_port] = si->ptype.num;        \
        si->ptype.port[si->ptype.num++] = logic_port;       \
        if (si->ptype.min < 0) {                            \
            si->ptype.min = logic_port;                     \
        }                                                   \
        if (logic_port > si->ptype.max) {                   \
            si->ptype.max = logic_port;                     \
        }                                                   \
    }

    RECONFIGURE_PORT_TYPE_INFO(ge);
    RECONFIGURE_PORT_TYPE_INFO(xe);
    RECONFIGURE_PORT_TYPE_INFO(c);
    RECONFIGURE_PORT_TYPE_INFO(ce);
    RECONFIGURE_PORT_TYPE_INFO(xl);
    RECONFIGURE_PORT_TYPE_INFO(cl);
    RECONFIGURE_PORT_TYPE_INFO(gx);
    RECONFIGURE_PORT_TYPE_INFO(ether);
    RECONFIGURE_PORT_TYPE_INFO(hg);
    RECONFIGURE_PORT_TYPE_INFO(st);
    RECONFIGURE_PORT_TYPE_INFO(port);
    RECONFIGURE_PORT_TYPE_INFO(all);

#undef  RECONFIGURE_PORT_TYPE_INFO


    /*
     * Update block port
     * Use first logical port on block (logic follows ESW driver
     * soc_info_config().
     */
    for (phy_port = 0; ; phy_port++) {

        blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
        bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, 0);
        if (blk < 0 && bindex < 0) { /* End of port list */
            break;
        }

        logic_port = si->port_p2l_mapping[phy_port];
        if (logic_port < 0) {
            continue;
        }

        for (port_idx = 0; port_idx < SOC_DRIVER(unit)->port_num_blktype;
             port_idx++) {
            blk = SOC_PORT_IDX_BLOCK(unit, phy_port, port_idx);
            if (blk < 0) { /* End of block list of each port */
                break;
            }

            if (si->block_port[blk] < 0) {
                si->block_port[blk] = logic_port;
            }
        }
    }

    /* Update MAX_PORT(unit) */
    max_port = -1;
    for (phy_port = 0; phy_port < BCMI_PORT_DRV_DEV_INFO(unit)->phy_ports_max;
         phy_port++) {
        logic_port = si->port_p2l_mapping[phy_port];
        if (logic_port < 0) {
            continue;
        }
        if (max_port < logic_port) {
            max_port = logic_port;
        }
    }
    si->port_num = max_port + 1;

    /* Update user port mappings (includes port names update) */
    soc_esw_dport_init(unit);

    return SOC_E_NONE;
}
#endif

/*
 * Function:
 *      _soc_xgs5_port_soc_info_ptype_block_add
 * Purpose:
 *      Set the port type bitmaps and block information in SOC_INFO
 *      for ports to be configured during the FlexPort operation.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      post_count     - (IN) Array size for post-FlexPort array.
 *      post_resource  - (IN) Pre-FlexPort configuration array.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Assumes linkscan is paused, COUNTER_LOCK and SOC_CONTROL_LOCK
 *      locks are taken.
 */
STATIC int
_bcmi_xgs5_port_soc_info_ptype_block_add(int unit,
                                soc_port_schedule_state_t *port_schedule_state)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *resource, *pr;
    int logic_port, phy_port, port_idx;
    int blk, bindex, blktype, old_blktype;
    int nport, i;

    nport = port_schedule_state->nport;
    resource = port_schedule_state->resource;

    /*
     * Add to port type bitmaps and block information
     */
    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        if (pr->physical_port == -1) {
            continue;
        }

        logic_port = pr->logical_port;
        phy_port = pr->physical_port;

        blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
        bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, 0);
        if (blk < 0 && bindex < 0) { /* End of regsfile port list */
            continue;
        }

        blktype = -1;
        for (port_idx = 0; port_idx < SOC_DRIVER(unit)->port_num_blktype;
             port_idx++) {

            blk = SOC_PORT_IDX_BLOCK(unit, phy_port, port_idx);
            if (blk < 0) { /* End of block list of each port */
                break;
            }

            old_blktype = blktype;
            blktype = SOC_BLOCK_INFO(unit, blk).type;

            switch (blktype) {
            case SOC_BLK_CDPORT:
                SOC_PBMP_PORT_ADD(si->cd.bitmap, logic_port);
                if ((pr->encap == SOC_ENCAP_IEEE) &&
                    ((si->port_init_speed[logic_port] == 200000) ||
                    (si->port_init_speed[logic_port] == 400000))) { /* CDE */
                    SOC_PBMP_PORT_ADD(si->cde.bitmap, logic_port);
                    SOC_PBMP_PORT_ADD(si->ether.bitmap, logic_port);
                    SOC_PBMP_PORT_ADD(si->port.bitmap, logic_port);
                    SOC_PBMP_PORT_ADD(si->all.bitmap, logic_port);
                    break;
                }
                /* fall through to case for SOC_BLK_CLPORT */

            case SOC_BLK_CLPORT:
                if (blktype == SOC_BLK_CLPORT) {
                SOC_PBMP_PORT_ADD(si->cl.bitmap, logic_port);
                }
                if (si->port_init_speed[logic_port] >= 100000) {
                    /* Catch both CE and HG ports */
                    SOC_PBMP_PORT_ADD(si->c.bitmap, logic_port);
                }
                if ((pr->encap == SOC_ENCAP_IEEE) &&
                    ((si->port_init_speed[logic_port] == 100000) ||
                    (si->port_init_speed[logic_port] == 120000))) { /* CE */
                    SOC_PBMP_PORT_ADD(si->ce.bitmap, logic_port);
                    SOC_PBMP_PORT_ADD(si->ether.bitmap, logic_port);
                    SOC_PBMP_PORT_ADD(si->port.bitmap, logic_port);
                    SOC_PBMP_PORT_ADD(si->all.bitmap, logic_port);
                    break;
                }
                /* fall through to case for SOC_BLK_XLPORT */
                /* coverity[fallthrough: FALSE] */

            case SOC_BLK_XLPORT:
                if (!si->port_init_speed[logic_port]) {
                    return SOC_E_INTERNAL;
                }

                /* For 100G or more, use CLPORT block type */
                if ((SOC_BLK_XLPORT == blktype) &&
                    (si->port_init_speed[logic_port] >= 100000)) {
                    blktype = old_blktype;
                    break;
                }

                if (si->port_init_speed[logic_port] < 10000) {    /* GE */
                    SOC_PBMP_PORT_ADD(si->ge.bitmap, logic_port);
                    if (pr->encap == SOC_ENCAP_HIGIG2_LITE) {
                        /* GE in Higig Lite Mode. Higig Like stack capable
                         * port */
                        SOC_PBMP_PORT_ADD(si->st.bitmap, logic_port);
                        SOC_PBMP_PORT_ADD(si->hl.bitmap, logic_port);
                    } else {
                        SOC_PBMP_PORT_ADD(si->ether.bitmap, logic_port);
                    }
                } else if (pr->encap == SOC_ENCAP_IEEE) {  /* XE */
                    SOC_PBMP_PORT_ADD(si->xe.bitmap, logic_port);
                    SOC_PBMP_PORT_ADD(si->ether.bitmap, logic_port);
                } else {                                   /* HG */
                    SOC_PBMP_PORT_ADD(si->hg.bitmap, logic_port);
                    SOC_PBMP_PORT_ADD(si->st.bitmap, logic_port);
                    /* Name is default to hg by lookup above */
                }

                SOC_PBMP_PORT_ADD(si->port.bitmap, logic_port);
                SOC_PBMP_PORT_ADD(si->all.bitmap, logic_port);

                /*
                 * Part of legacy code to add to these extra bitmaps
                 */
                if (!IS_CL_PORT(unit, logic_port) &&
                    !IS_CD_PORT(unit, logic_port)) {
                    SOC_PBMP_PORT_ADD(si->xl.bitmap, logic_port);
                }
                if (!(SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit))) {
                    SOC_PBMP_PORT_ADD(si->gx.bitmap, logic_port);
                }
                break;
            case SOC_BLK_GXPORT:
            case SOC_BLK_GXFPPORT:
                if (pr->encap == SOC_ENCAP_IEEE) {
                    SOC_PBMP_PORT_ADD(si->ether.bitmap, logic_port);
                }
                if (si->port_init_speed[logic_port] < 10000) {
                    SOC_PBMP_PORT_ADD(si->ge.bitmap, logic_port);
                }
                SOC_PBMP_PORT_ADD(si->port.bitmap, logic_port);
                SOC_PBMP_PORT_ADD(si->all.bitmap, logic_port);
                SOC_PBMP_PORT_ADD(si->gx.bitmap, logic_port);
            default:
                break;
            }

            /* Update block information */
            si->block_valid[blk] += 1;

            if (si->block_port[blk] < 0) {
                si->block_port[blk] = logic_port;
            }
            SOC_PBMP_PORT_ADD(si->block_bitmap[blk], logic_port);

        } /* For each block in the given port */

        si->port_type[logic_port] = blktype;

    } /* For each port */

    BCM_IF_ERROR_RETURN
        (_bcmi_xgs5_port_soc_info_ptype_update(unit));

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_xgs5_port_soc_info_ptype_block_delete
 * Purpose:
 *      Clear the port type bitmaps and block information in SOC_INFO
 *      for ports to be removed during the FlexPort operation.
 * Parameters:
 *      unit           - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Assumes linkscan is paused, COUNTER_LOCK and SOC_CONTROL_LOCK
 *      locks are taken.
 */
STATIC int
_bcmi_xgs5_port_soc_info_ptype_block_delete(int unit,
                                soc_port_schedule_state_t *port_schedule_state)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *resource, *pr;
    soc_port_map_type_t *pre_port_map;
    int logic_port, phy_port, port_idx;
    int blk, bindex;
    int nport, i;

    nport = port_schedule_state->nport;
    resource = port_schedule_state->resource;
    pre_port_map = &port_schedule_state->in_port_map;

    /*
     * Clear port bitmaps and block information
     */
    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        /*
         * If not a delete entry, don't do anything
         * If 'inactive' flag, don't do anything
         */
        if (pr->physical_port != -1 ||
            ((pr->flags & SOC_PORT_RESOURCE_I_MAP) &&
             (pr->op == BCMI_XGS5_PORT_RESOURCE_OP_DEL))) {
            continue;
        }

        logic_port = pr->logical_port;
        phy_port = pre_port_map->port_l2p_mapping[logic_port];

        /*
         * Clear port from all possible port bitmaps where a flexed port
         * can be a member of.
         */
        SOC_PBMP_PORT_REMOVE(si->cde.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->ge.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->xe.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->c.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->ce.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->xl.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->cl.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->gx.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->ether.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->hg.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->st.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->port.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->all.bitmap, logic_port);

        if (phy_port == -1) {
            continue;
        }

        blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
        bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, 0);
        if (blk < 0 && bindex < 0) { /* End of regsfile port list */
            continue;
        }

        /* Clear block information */
        for (port_idx = 0; port_idx < SOC_DRIVER(unit)->port_num_blktype;
             port_idx++) {

            blk = SOC_PORT_IDX_BLOCK(unit, phy_port, port_idx);
            if (blk < 0) { /* End of block list of each port */
                break;
            }

            /* Update block information */
            if (si->block_valid[blk] > 0) {
                si->block_valid[blk] -= 1;
            }
            if (si->block_port[blk] == logic_port) {
                si->block_port[blk] = REG_PORT_ANY;
            }
            SOC_PBMP_PORT_REMOVE(si->block_bitmap[blk], logic_port);

        } /* For each block in the given port */

        si->port_type[logic_port] = 0;
        si->port_offset[logic_port] = 0;

    } /* For each port */

    return SOC_E_NONE;
}



/*
 * Function:
 *      _soc_xgs5_port_soc_counter_add
 * Purpose:
 *      Add ports to SOC counter map.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      post_count     - (IN) Array size.
 *      post_resource  - (IN) Array of ports to add. * Returns:
 *      SOC_E_XXX * Notes:
 *      Assumes linkscan is paused, COUNTER_LOCK and SOC_CONTROL_LOCK
 *      locks are taken.
 */
STATIC int
_bcmi_xgs5_port_soc_counter_add(int unit,
                           soc_port_schedule_state_t *port_schedule_state)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_port_resource_t *resource, *pr;
    soc_counter_non_dma_t *non_dma;
    int i, nport, dma_id;
    int phy_port;
    int logic_port;
    int blk;
    int bindex;
    int blktype;
    int port_idx;
    soc_ctr_type_t ctype;
    int rv = BCM_E_NONE;
    
    nport = port_schedule_state->nport;
    resource = port_schedule_state->resource;

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {

        /* Skip 'delete' ports */
        if (pr->physical_port == -1) {
            continue;
        }

        logic_port = pr->logical_port;
        phy_port = pr->physical_port;
        blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
        bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, 0);
        if (blk < 0 && bindex < 0) { /* End of regsfile port list */
            continue;
        }
        for (port_idx = 0; port_idx < SOC_DRIVER(unit)->port_num_blktype;
             port_idx++) {
            blk = SOC_PORT_IDX_BLOCK(unit, phy_port, port_idx);
            if (blk < 0) { /* End of block list of each port */
                break;
            }
            blktype = SOC_BLOCK_INFO(unit, blk).type;
            switch (blktype) {
            case SOC_BLK_CDPORT:
                ctype = SOC_CTR_TYPE_CD;
                break;
            case SOC_BLK_CLPORT:
                ctype = SOC_CTR_TYPE_CE;
                break;
            case SOC_BLK_XLPORT:
            case SOC_BLK_GXPORT:
                if(IS_QSGMII_PORT(unit, logic_port) ||
                   IS_EGPHY_PORT(unit, logic_port)) {
                    ctype = SOC_CTR_TYPE_GE;
                } else {
                    ctype = SOC_CTR_TYPE_XE;
                }
                break;
            case SOC_BLK_GXFPPORT:
                ctype = SOC_CTR_TYPE_GE;
                break;
            default:
                ctype = -1;
                break;
            }
            if (ctype != -1) {
                /* Adding of dma counter descriptors is only necessary when the
                 * counter thread is running
                 * this check is necessary to avoid reallocating the dma
                 * descriptos when the conter thread is stopped running before
                 * the flex operation
                 */
                if (soc->counter_interval) {
                    rv = soc_counter_port_sbusdma_desc_alloc(unit, logic_port);
                    if (BCM_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_BCM_PORT,
                                  (BSL_META_U(unit,
                                              "Error! Unable to allocate SBUS "
                                              "DMA descriptors per logical "
                                              "port %d  \n"),
                                   logic_port));
                        return rv;
                    }
                }
                BCM_IF_ERROR_RETURN
                    (soc_port_cmap_set(unit, logic_port, ctype));
                SOC_PBMP_PORT_ADD(soc->counter_pbmp, logic_port);

                for (dma_id = 0;
                     dma_id < SOC_COUNTER_NON_DMA_END - SOC_COUNTER_NON_DMA_START;
                     dma_id++) {
                    non_dma = &soc->counter_non_dma[dma_id];
                    if (non_dma->entries_per_port > 0) {
                        SOC_PBMP_PORT_ADD(non_dma->pbmp, logic_port);
                    }
                }
                break;
            }
        } /* For each block in the given port */
    } /* For each port */

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_xgs5_port_soc_counter_delete
 * Purpose:
 *      Delete ports from SOC counter map.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      pre_count      - (IN) Array size.
 *      pre_resource   - (IN) Array of ports to delete.
 * Returns:
 *      SOC_E_XXX * Notes:
 *      Assumes linkscan is paused, COUNTER_LOCK and SOC_CONTROL_LOCK
 *      locks are taken.
 */
STATIC int
_bcmi_xgs5_port_soc_counter_delete(int unit,
                           soc_port_schedule_state_t *port_schedule_state)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_port_resource_t *resource, *pr;
    int i, nport;
    int phy_port;
    int logic_port;
    int rv = BCM_E_NONE;

    nport = port_schedule_state->nport;
    resource = port_schedule_state->resource;

    /* Clear counter */
    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {

        logic_port = pr->logical_port;
        phy_port = port_schedule_state->in_port_map.port_l2p_mapping[logic_port];

        if (phy_port == -1) {
            continue;
        }

        /* Delete the counter dma descriptors */
        rv = soc_counter_port_sbusdma_desc_free(unit, logic_port);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Error! Unable to delete SBUS DMA "
                                  "descriptors per logical port %d  \n"),
                       logic_port));
            return rv;
        }

        /*
         * If 'inactive' flag, don't do anything
         */
        if (pr->flags & SOC_PORT_RESOURCE_I_MAP) {
            continue;
        }

        soc->counter_map[logic_port] = 0;
        SOC_PBMP_PORT_REMOVE(soc->counter_pbmp, logic_port);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_software_free
 * Purpose:
 *      Deallocates memory for a given port from the BCM PORT module
 *      data structure.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      port   - (IN) Logical port, BCM format.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      - Assumes main BCM PORT module initialization has been executed.
 *      - Assumes port is valid.
 */
STATIC int
_bcmi_xgs5_port_software_free(int unit, bcm_port_t port)
{
    _bcm_port_info_t *pinfo;

    /* Get PORT info structure pointer */
    BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));
    if  (pinfo == NULL) {
        return BCM_E_INTERNAL;
    }

    /* Proto-based VLAN_DATA state */
    if (pinfo->p_vd_pbvl != NULL) {
        sal_free(pinfo->p_vd_pbvl);
        pinfo->p_vd_pbvl = NULL;
    }

    /* End-to-end congestion control configuration */
    if (pinfo->e2ecc_config != NULL) {
        sal_free(pinfo->e2ecc_config);
        pinfo->e2ecc_config = NULL;
    }

    /* Reset all fields */
    sal_memset(pinfo, 0, sizeof(_bcm_port_info_t));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_skip_attach_phase
 * Purpose:
 *      Check if the phase in attach should be skipped.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      phase   - (IN) Phase number in attach/detach.
 * Returns:
 *      1: skip the phase
 *      0: Don't skip the phase
 * Notes:
 */
STATIC int
_bcmi_xgs5_port_skip_attach_phase(int unit, int phase)
{
    int skip;
    switch (phase)
    {
        case PORT_ATTACH_EXEC__STG:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                              SHR_BPROF_BCM_STG);
            break;
        case PORT_ATTACH_EXEC__VLAN:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                              SHR_BPROF_BCM_VLAN);
            break;
        case PORT_ATTACH_EXEC__TRUNK:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                              SHR_BPROF_BCM_TRUNK);
            break;
        case PORT_ATTACH_EXEC__COSQ:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                              SHR_BPROF_BCM_COSQ);
            break;
        case PORT_ATTACH_EXEC__LINKSCAN:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                              SHR_BPROF_BCM_LINKSCAN);
            break;
        case PORT_ATTACH_EXEC__STATISTIC:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                              SHR_BPROF_BCM_STAT);
            break;
        case PORT_ATTACH_EXEC__STACK:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                              SHR_BPROF_BCM_STK);
            break;
        case PORT_ATTACH_EXEC__RATE:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                              SHR_BPROF_BCM_RATE);
            break;
        case PORT_ATTACH_EXEC__FIELD:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                              SHR_BPROF_BCM_FIELD);
            break;
        case PORT_ATTACH_EXEC__MIRROR:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                              SHR_BPROF_BCM_MIRROR);
            break;
        case PORT_ATTACH_EXEC__L3:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                              SHR_BPROF_BCM_L3);
            break;
        case PORT_ATTACH_EXEC__IPMC:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                              SHR_BPROF_BCM_IPMC);
            break;
        case PORT_ATTACH_EXEC__IPMC_REPL:
            if (SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                           SHR_BPROF_BCM_IPMC) &&
                soc_feature(unit, soc_feature_ip_mcast_repl)) {
                skip = 0;
            } else {
                skip = 1;
            }
            break;
        case PORT_ATTACH_EXEC__MPLS:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                              SHR_BPROF_BCM_MPLS);
            break;
        case PORT_ATTACH_EXEC__MIM:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                              SHR_BPROF_BCM_MIM);
            break;
        default:
            skip = 0;
            break;
    }
    return skip;
}

/*
 * Function:
 *      _bcm_xgs5_port_resource_attach
 * Purpose:
 *      Attach ports to the BCM layer and initialize them
 *      to the default state.
 *
 *      Attach only those ports that are flagged as 'new' or 'remapped'.
 *      If a port currently exists and the mapping is not changed, this
 *      does not need to be attached.
 *      - New:       Logical port did not exist before FlexPort.
 *      - Remapped:  Logical port will continue to exist after FlexPort
 *                   but is mapped to a different physical port.
 *                   The port needs to be 'detached' and 'attached'.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN) Port Resource FlexPort configuration.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      - Assumes caller have detached any ports that will be attached
 *        in this routine (this is for the REMAP case).
 *      - Assumes this is NOT called during Warmboot.
 */
STATIC int
_bcmi_xgs5_port_resource_attach(int unit,
                                soc_port_schedule_state_t *port_schedule_state)
{
    int rv = BCM_E_NONE;
    soc_port_resource_t *pr, *resource;
    int i, j, nport, my_modid;

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "--- BCM Attach ---\n")));

    nport = port_schedule_state->nport;
    resource = port_schedule_state->resource;

    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &my_modid));

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        /*
         * If 'inactive' flag, don't do anything
         */
        if (pr->flags & SOC_PORT_RESOURCE_I_MAP) {
#ifdef BCM_TOMAHAWK3_SUPPORT
            if (SOC_IS_TOMAHAWK3(unit)) {
                if (pr->flags & SOC_PORT_RESOURCE_SPEED) {
                    BCM_IF_ERROR_RETURN(soc_th3_flex_reconf_thresholds(unit,
                                pr->logical_port, pr->pipe));
                }
            }
#endif
            continue;
        }

        /* Skip 'delete' ports */
        if (pr->physical_port == -1) {
            continue;
        }

        if ((pr->op & BCMI_XGS5_PORT_RESOURCE_OP_ADD) ||
            (pr->op & BCMI_XGS5_PORT_RESOURCE_OP_REMAP) ||
            (pr->op & BCMI_XGS5_PORT_RESOURCE_OP_CHL)) {

            for(j = 0; j < PORT_ATTACH_EXEC__MAX; j++) {
                if(BCMI_PORT_DRV_CALL(unit)->port_attach_exec[j] != NULL) {

                    if (_bcmi_xgs5_port_skip_attach_phase(unit, j)) {
                        LOG_VERBOSE(BSL_LS_BCM_PORT,
                                    (BSL_META_U(unit,
                                        "Skip Attach Phase %s\n"),
                                        bcmi_xgs5_port_attach_str[j]));
                        continue;
                    }

                    LOG_VERBOSE(BSL_LS_BCM_PORT,
                                (BSL_META_U(unit,
                                    "BCM PORT %d (%s) Attach Phase %s\n"),
                                    pr->logical_port,
                                    SOC_PORT_NAME(unit, pr->logical_port),
                                    bcmi_xgs5_port_attach_str[j]));

                    rv = BCMI_PORT_DRV_CALL(unit)->port_attach_exec[j](unit,
                                                            pr->logical_port);
                    if (BCM_FAILURE(rv)) {
                        _bcmi_xgs5_port_software_free(unit, pr->logical_port);
                        LOG_ERROR(BSL_LS_BCM_PORT,
                                  (BSL_META_U(unit,
                                      "Error: Unable to attach BCM port. "
                                      "Failed at phase %d - %s. "
                                      "unit=%d port=%d rv=%d(%s)\n"),
                                      j, bcmi_xgs5_port_attach_str[j], unit,
                                      pr->logical_port, rv, bcm_errmsg(rv)));
                        return rv;
                    }
                }
            }
        }
    }

    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_set(unit, my_modid));

    return BCM_E_NONE;
}

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
/*
 * Function:
 *      _bcmi_xgs5_chl_soc_info_validate
 * Purpose:
 *      Validate incoming channelized configuration.
 *
 * Parameters:
 *      unit                - (IN) Unit number.
 *      port_schedule_state - (IN) Port schedule state.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_bcmi_xgs5_chl_soc_info_validate(int unit,
                             bcm_port_resource_t *resource,
                             soc_port_schedule_state_t *port_schedule_state)
{

    soc_info_t *si = &SOC_INFO(unit);
    int i;
    int nport;
    bcm_port_t port;
    int total_subports, line_card;

    nport = port_schedule_state->nport;
    /* Update mapping */
    for (i = 0;  i < nport; i++) {

        if (resource[i].physical_port == -1) {
            continue;
        }

        total_subports = 0;
        line_card = resource[i].line_card;
      
        if ((port_schedule_state->resource[i].coe_port) &&
            (line_card < 1) && (resource[i].num_subports < 1)) {
            return BCM_E_PARAM;
        }

        line_card = resource[i].line_card;
        if (line_card < 1) {
            continue;
        }

        if (resource[i].num_subports < 1) {
            return BCM_E_PARAM;
        }
        total_subports = resource[i].num_subports;

        BCM_PBMP_ITER(PBMP_PORT_ALL(unit), port) {
            if (si->port_lc_mapping[port] == line_card) {

                total_subports += si->port_num_subport[port];
                /* Max 48 subports on each line card. */
                if (total_subports > FB6_SUBPORT_COE_PORT_PER_CASPORT_MAX) {
                    return BCM_E_PARAM;
                }
            }
         }
     }
     return BCM_E_NONE;
}
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

/*
 * Function:
 *      _bcmi_xgs5_port_soc_info_add
 * Purpose:
 *      Add port information in SOC_INFO SW
 *
 * Parameters:
 *      unit                - (IN) Unit number.
 *      port_schedule_state - (IN) Port schedule state.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_bcmi_xgs5_port_soc_info_add(int unit,
                             bcm_port_resource_t *resource,
                             soc_port_schedule_state_t *port_schedule_state)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_persist_t *sop = SOC_PERSIST(unit);
    soc_port_resource_t *spr, *pr;
    soc_pmvco_info_t pm_vco_info;
    int i;
    int port, phy_port, mmu_port, idb_port;
    int nport;
    int rv = BCM_E_NONE;

    nport = port_schedule_state->nport;
    spr = port_schedule_state->resource;

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
    BCM_IF_ERROR_RETURN(_bcmi_xgs5_chl_soc_info_validate(unit, resource,
        port_schedule_state));
#endif /* BCM_CHANNELIZED_SWITCHING_SUPPORT */

    SOC_CONTROL_LOCK(unit);
    /* Update mapping */
    for (i = 0, pr = spr; i < nport; i++, pr++) {
        port = pr->logical_port;
        phy_port = pr->physical_port;
        idb_port = pr->idb_port;
        mmu_port = pr->mmu_port;

        if (phy_port == -1) {
           /* Remove port from physical_pbm, which is used to store the port bitmap of 25G
            *  ports using 50G TDM calendar.
            */
            if (SOC_PBMP_MEMBER(
                    port_schedule_state->in_port_map.physical_pbm, port)){
                SOC_PBMP_PORT_REMOVE(
                    port_schedule_state->in_port_map.physical_pbm, port);
            }
            continue;
        }

        if (pr->op & (BCMI_XGS5_PORT_RESOURCE_OP_ADD |
		      BCMI_XGS5_PORT_RESOURCE_OP_REMAP)) {

            /* IPG Configuration per port */
            sop->ipg[port].hd_10    = 96;
            sop->ipg[port].hd_100   = 96;
            sop->ipg[port].hd_1000  = 96;
            sop->ipg[port].hd_2500  = 96;
            sop->ipg[port].fd_10    = 96;
            sop->ipg[port].fd_100   = 96;
            sop->ipg[port].fd_1000  = 96;
            sop->ipg[port].fd_2500  = 96;
            sop->ipg[port].fd_10000 = 96;
            sop->ipg[port].fd_xe    = 96;
            sop->ipg[port].fd_hg    = 64;
            sop->ipg[port].fd_hg2   = 96;

            SOC_PBMP_PORT_REMOVE(si->all.disabled_bitmap, port);

            /* Port Mapping related information */
            si->port_l2p_mapping[port] = phy_port;
            si->port_p2l_mapping[phy_port] = port;
            si->port_l2i_mapping[port] = idb_port;
            si->port_p2m_mapping[phy_port] = mmu_port;
            if (mmu_port != -1) {
                si->port_m2p_mapping[mmu_port] = phy_port;
            }

            /* Pipe */
            si->port_pipe[port] = pr->pipe;
            SOC_PBMP_PORT_ADD(si->pipe_pbm[pr->pipe], port);

            si->port_speed_max[port] = resource[i].speed;
            si->port_init_speed[port] = resource[i].speed;
            si->port_num_lanes[port] = pr->num_lanes;
#if defined(BCM_TOMAHAWK3_SUPPORT)
            if (SOC_IS_TOMAHAWK3(unit)) {
                si->port_serdes[port] =
                    (phy_port - 1) / BCMI_XGS5_PORTS_PER_PM8X50_PBLK;
            } else
#endif
            {
            si->port_serdes[port] = (phy_port - 1) / BCMI_XGS5_PORTS_PER_PBLK;
            }

            si->port_fec_type[port] = resource[i].fec_type;
            si->port_phy_lane_config[port] = resource[i].phy_lane_config;
            si->port_link_training[port] = resource[i].link_training;

#ifdef BCM_HURRICANE4_SUPPORT
            if (SOC_IS_HURRICANE4(unit)) {
                si->port_serdes[port] = HR4_PHY_PORT_SERDERS(phy_port);
            }
#endif

#ifdef BCM_HELIX5_SUPPORT
            if (SOC_IS_HELIX5(unit)) {
                if (phy_port <= 48) {
                    si->port_serdes[port] = (phy_port - 1) / _HX5_PORTS_PER_PMQ_PBLK;
                } else {
                    si->port_serdes[port] = ((phy_port - 1) / _HX5_PORTS_PER_PBLK) - 9;
                }
            }
#endif

            /* Oversub */
            if (pr->oversub) {
                SOC_PBMP_PORT_ADD(si->oversub_pbm, port);
            } else {
                SOC_PBMP_PORT_REMOVE(si->oversub_pbm, port);
            }

            /* ENCAP */
            if ((pr->encap == SOC_ENCAP_HIGIG2) ||
                (pr->encap == SOC_ENCAP_HIGIG2_LITE) ||
                (pr->encap == SOC_ENCAP_HIGIG2_L2) ||
                (pr->encap == SOC_ENCAP_HIGIG2_IP_GRE)) {
                SOC_PBMP_PORT_ADD(si->hg2_pbm, port);
            } else {
                SOC_PBMP_PORT_REMOVE(si->hg2_pbm, port);
            }

            /* High Speed Port */
            if (pr->hsp) {
                SOC_PBMP_PORT_ADD(si->eq_pbm, port);
            } else {
                SOC_PBMP_PORT_REMOVE(si->eq_pbm, port);
            }
        } else {
            if (pr->op & (BCMI_XGS5_PORT_RESOURCE_OP_SPEED |
                             BCMI_XGS5_PORT_RESOURCE_OP_LANES)) {
                if ((si->port_speed_max[pr->logical_port] == 0) ||
                    (pr->op & BCMI_XGS5_PORT_RESOURCE_OP_LANES)){
                    si->port_speed_max[pr->logical_port] = resource[i].speed;
                }

                si->port_init_speed[pr->logical_port] = resource[i].speed;
                si->port_num_lanes[port] = pr->num_lanes;

                si->port_fec_type[port] = resource[i].fec_type;
                si->port_phy_lane_config[port] = resource[i].phy_lane_config;
                si->port_link_training[port] = resource[i].link_training;


                SOC_PBMP_PORT_REMOVE(si->eq_pbm, port);
                if (pr->hsp) {
                    SOC_PBMP_PORT_ADD(si->eq_pbm, port);
                }

                /*
                 * Update bitmaps and port names when switching b/w CE <=> XE ports.
                 *  - Remove port from bitmap first
                 */
                SOC_PBMP_PORT_REMOVE(si->cde.bitmap, port);
                SOC_PBMP_PORT_REMOVE(si->ce.bitmap, port);
                SOC_PBMP_PORT_REMOVE(si->xe.bitmap, port);
                SOC_PBMP_PORT_REMOVE(si->ge.bitmap, port);
            }
            /* ENCAP */
            if ((pr->op & BCMI_XGS5_PORT_RESOURCE_OP_ENCAP) ||
                (pr->op & BCMI_XGS5_PORT_RESOURCE_OP_ENCAP_FLEX)) {
                if ((pr->encap == SOC_ENCAP_HIGIG2) ||
                    (pr->encap == SOC_ENCAP_HIGIG2_LITE) ||
                    (pr->encap == SOC_ENCAP_HIGIG2_L2) ||
                    (pr->encap == SOC_ENCAP_HIGIG2_IP_GRE)) {
                    SOC_PBMP_PORT_ADD(si->hg2_pbm, port);
                } else {
                    SOC_PBMP_PORT_REMOVE(si->hg2_pbm, port);
                }
            }
            if (pr->op & BCMI_XGS5_PORT_RESOURCE_OP_ACTIVE) {
                SOC_PBMP_PORT_REMOVE(si->all.disabled_bitmap, port);
            }
        }

        /* Update physical_pbm, which is used to store the port bitmap of 25G ports using
         * 50G TDM calendar.
         */
        if (pr->flags & BCM_PORT_RESOURCE_25G_USE_50G_CALENDAR) {
            SOC_PBMP_PORT_ADD(port_schedule_state->in_port_map.physical_pbm,
                              port);
        } else if (SOC_PBMP_MEMBER(
                   port_schedule_state->in_port_map.physical_pbm, port)){
            SOC_PBMP_PORT_REMOVE(port_schedule_state->in_port_map.physical_pbm,
                                 port);
        }
#ifdef BCM_TOMAHAWK3_SUPPORT
        if (SOC_IS_TOMAHAWK3(unit)) {
            soc_tomahawk3_num_cosq_init(unit, port);
        }
#endif /* BCM_TOMAHAWK3_SUPPORT */

#if defined(BCM_CHANNELIZED_SWITCHING_SUPPORT)
        if (soc_feature(unit, soc_feature_channelized_switching)) {
           if (port_schedule_state->resource[i].coe_port) {
                si->port_num_subport[port] =
                    resource[i].num_subports;
                si->port_lc_mapping[port] =
                    resource[i].line_card;
                BCM_PBMP_PORT_ADD(si->subtag_allowed_pbm, port);
            } else {
                si->port_num_subport[port] = -1;
                si->port_lc_mapping[port] = -1;
                BCM_PBMP_PORT_REMOVE(si->subtag_allowed_pbm, port);
            }
        }
#endif
    }

    /* Add port ptype bitmap and block information */
    rv = _bcmi_xgs5_port_soc_info_ptype_block_add(unit, port_schedule_state);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Add counter map */
    rv = _bcmi_xgs5_port_soc_counter_add(unit, port_schedule_state);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    /* Update VCO information for devices that need it */
    for (i = 0; i < SOC_MAX_NUM_BLKS; i++) {
        rv = soc_esw_portctrl_pm_vco_fetch(unit, i, &pm_vco_info);
        if (BCM_SUCCESS(rv)) {
            if (pm_vco_info.is_tvco_new == 1 ||
                pm_vco_info.is_ovco_new == 1) {
                sal_memcpy(&si->pm_vco_info[i],
                           &pm_vco_info,
                           sizeof(soc_pmvco_info_t));
            }
        } else {
            rv = BCM_E_NONE;
        }
    }

    /* Update the flexible half pipe mappings. */
#ifdef BCM_FIREBOLT6_SUPPORT
    if (SOC_IS_FIREBOLT6(unit)) {
       (void) _soc_fb6_mmu_idb_ports_assign(unit);
    }
#endif /* BCM_FIREBOLT6_SUPPORT */

cleanup:
    SOC_CONTROL_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcmi_xgs5_port_skip_detach_phase
 * Purpose:
 *      Check if the phase in detach should be skipped.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      phase   - (IN) Phase number in attach/detach.
 * Returns:
 *      1: skip the phase
 *      0: Don't skip the phase
 * Notes:
 */
STATIC int
_bcmi_xgs5_port_skip_detach_phase(int unit, int phase)
{
    int skip;
    switch (phase)
    {
        case PORT_DETACH_EXEC__STG:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                               SHR_BPROF_BCM_STG);
            break;
        case PORT_DETACH_EXEC__VLAN:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                               SHR_BPROF_BCM_VLAN);
            break;
        case PORT_DETACH_EXEC__TRUNK:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                               SHR_BPROF_BCM_TRUNK);
            break;
        case PORT_DETACH_EXEC__COSQ:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                               SHR_BPROF_BCM_COSQ);
            break;
        case PORT_DETACH_EXEC__STACK:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                               SHR_BPROF_BCM_STK);
            break;
        case PORT_DETACH_EXEC__RATE:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                               SHR_BPROF_BCM_RATE);
            break;
        case PORT_DETACH_EXEC__FIELD:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                               SHR_BPROF_BCM_FIELD);
            break;
        case PORT_DETACH_EXEC__MIRROR:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                               SHR_BPROF_BCM_MIRROR);
            break;
        case PORT_DETACH_EXEC__L3:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                               SHR_BPROF_BCM_L3);
            break;
        case PORT_DETACH_EXEC__IPMC:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                               SHR_BPROF_BCM_IPMC);
            break;
        case PORT_DETACH_EXEC__IPMC_REPL:
            if (SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                           SHR_BPROF_BCM_IPMC) &&
                soc_feature(unit, soc_feature_ip_mcast_repl)) {
                skip = 0;
            } else {
                skip = 1;
            }
            break;
        case PORT_DETACH_EXEC__MPLS:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                               SHR_BPROF_BCM_MPLS);
            break;
        case PORT_DETACH_EXEC__MIM:
            skip = !SHR_BITGET(BCMI_PORT_DRV_DEV_INFO(unit)->init_cond,
                               SHR_BPROF_BCM_MIM);
            break;
        default:
            skip = 0;
            break;
    }
    return skip;
}

/*
 * Function:
 *      _bcmi_xgs5_port_resource_detach
 * Purpose:
 *      Detach ports from the BCM layer.
 *
 * Parameters:
 *      unit                - (IN) Unit number.
 *      port_schedule_state - (IN) Port schedule state.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_bcmi_xgs5_port_resource_detach(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    int rv = BCM_E_NONE;
    soc_port_resource_t *pr, *resource;
    int i, j, nport;
    pbmp_t      pbmp;

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                (BSL_META_U(unit,
                            "--- BCM Detach ---\n")));

    SOC_PBMP_CLEAR(pbmp);

    nport = port_schedule_state->nport;
    resource = port_schedule_state->resource;

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        /* Skip 'add' ports */
        if (pr->physical_port != -1) {
            continue;
        }

        SOC_PBMP_PORT_ADD(pbmp, pr->logical_port);

        /*
         * If 'inactive' flag, don't do anything
         */
        if (pr->flags & SOC_PORT_RESOURCE_I_MAP) {
            continue;
        }

        if ((pr->op & BCMI_XGS5_PORT_RESOURCE_OP_DEL) ||
            (pr->op & BCMI_XGS5_PORT_RESOURCE_OP_CHL) ||
            (pr->op & BCMI_XGS5_PORT_RESOURCE_OP_REMAP)) {
            for(j = 0; j < PORT_DETACH_EXEC__MAX; j++) {
                if(BCMI_PORT_DRV_CALL(unit)->port_detach_exec[j] != NULL) {
                    if (_bcmi_xgs5_port_skip_detach_phase(unit, j)) {
                        LOG_VERBOSE(BSL_LS_BCM_PORT,
                                    (BSL_META_U(unit,
                                        "Skip Detach Phase %s\n"),
                                        bcmi_xgs5_port_detach_str[j]));
                        continue;
                    }

                    LOG_VERBOSE(BSL_LS_BCM_PORT,
                                (BSL_META_U(unit,
                                    "BCM PORT %d (%s) Detach Phase %s\n"),
                                    pr->logical_port,
                                    SOC_PORT_NAME(unit, pr->logical_port),
                                    bcmi_xgs5_port_detach_str[j]));

                    rv = BCMI_PORT_DRV_CALL(unit)->port_detach_exec[j](unit,
                                                            pr->logical_port);
                    if (BCM_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_BCM_PORT,
                                  (BSL_META_U(unit,
                                      "Error: Unable to detach BCM port. "
                                      "Failed at phase %d - %s. "
                                      "unit=%d port=%d rv=%d(%s)\n"),
                                      j, bcmi_xgs5_port_detach_str[j], unit,
                                      pr->logical_port, rv, bcm_errmsg(rv)));
                        return rv;
                    }
                }
            }
        }
    }

    /* Clear counters for ports in 'delete' entries */
    BCM_IF_ERROR_RETURN(soc_counter_set32_by_port(unit, pbmp, 0));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_pm_delete
 * Purpose:
 *      Remove port from Port Macro if Portmod enabled.
 *      Detach PHY if portmod disabled.
 *
 * Parameters:
 *      unit   - (IN) Unit number.
 *      pr_bmp - (IN) Bitmaps of port to be added/deleted during flexing.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
STATIC int
_bcmi_xgs5_port_pm_delete(int unit,
                          bcmi_xgs5_port_resource_bmp_t *pr_bmp)
{
    int delete_count, inact_count, nport, i, rv = BCM_E_NONE;
    soc_port_resource_t *del_res;
    bcm_port_t port;
    int loopback = BCM_PORT_LOOPBACK_NONE;

    SOC_PBMP_COUNT(pr_bmp->del_pbm_log, delete_count);
    SOC_PBMP_COUNT(pr_bmp->inact_pbm_log, inact_count);

    if ((delete_count <= 0) && (inact_count <= 0)) {
        return BCM_E_NONE;
    }

    nport = delete_count + inact_count;
    del_res = sal_alloc(sizeof(soc_port_resource_t) * nport,
                        "Delete_port_resources");
    if (del_res == NULL) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Unable to allocate memory for delete "
                              "resource array in FlexPort operation\n")));
        return SOC_E_MEMORY;
    }
    sal_memset(del_res, 0, sizeof(soc_port_resource_t) * nport);
    i = 0;

    SOC_PBMP_ITER(pr_bmp->del_pbm_log, port) {
        del_res[i].logical_port = port;
        i++;
    }
    SOC_PBMP_ITER(pr_bmp->inact_pbm_log, port) {
        del_res[i].logical_port = port;
        i++;
    }

    if (SOC_USE_PORTCTRL(unit)) {
        /* Clear loopback */
        for (i = 0; i < nport; i++) {
            port = del_res[i].logical_port;
            rv = bcm_esw_port_loopback_get(unit, port, &loopback);
            if (SOC_SUCCESS(rv) && loopback != BCM_PORT_LOOPBACK_NONE) {
                rv = bcmi_esw_portctrl_loopback_set(unit, port,
                                                    BCM_PORT_LOOPBACK_NONE);
                if (SOC_SUCCESS(rv)) {
                   rv = bcmi_xgs5_port_enable_set(unit, port, 0);
                }
            }
            if (BCM_FAILURE(rv)) {
                sal_free(del_res);
                LOG_ERROR(BSL_LS_SOC_PORT,
                          (BSL_META_U(unit,
                                      "Unable to clear loopback in "
                                      "FlexPort operation\n")));
                return rv;
            }
        }
        rv = soc_esw_portctrl_pm_ports_delete(unit, nport, del_res);
    } else {
        for (i = 0; i < nport; i++) {
            port = del_res[i].logical_port;
            rv = soc_phyctrl_detach(unit, port);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    }

    sal_free(del_res);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Unable to detach MAC/PHY in "
                              "FlexPort operation\n")));
    }
    return rv;
}

/*
 * Function:
 *      _bcmi_xgs5_port_pm_vco_reconfigure
 * Purpose:
 *      Reconfigure the VCOs of the Port Macros if the device supports
 *      this and the PMs require reconfiguring.
 *
 *      For now, this calls down to a device specific portctrl
 *            VCO reconfigure
 *
 * Parameters:
 *      unit   - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      For now, this calls down to a device specific portctrl
 *      VCO reconfigure
 */
STATIC int
_bcmi_xgs5_port_pm_vco_reconfigure(int unit)
{
    
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        return soc_esw_portctrl_pm_vco_reconfigure(unit);
    } else
#endif
    {
        return BCM_E_NONE;
    }
}

STATIC int
_bcmi_xgs5_port_sbus_bcast_setup(int unit)
{
#if defined(BCM_TOMAHAWK3_SUPPORT)
    if (SOC_IS_TOMAHAWK3(unit)) {
        return soc_esw_portctrl_sbus_bcast_setup(unit);
    } else
#endif
    {
        return BCM_E_NONE;
    }
}

/*
 * Function:
 *      _bcmi_xgs5_port_pm_add
 * Purpose:
 *      Add port to Port Macro if portmod enabled.
 *      Probe MAC/PHY for port if portmod disabled. 
 *
 * Parameters:
 *      unit   - (IN) Unit number.
 *      pr_bmp - (IN) Bitmaps of port to be added/deleted during flexing.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
STATIC int
_bcmi_xgs5_port_pm_add(int unit,
                          bcmi_xgs5_port_resource_bmp_t *pr_bmp)
{
    int add_count, act_count, inact_count, nport, i, rv = BCM_E_NONE;
    soc_port_resource_t *add_res;
    bcm_port_t port, phy_port;
    uint32 rval;
    int blk, blk_type, is_clport;
    pbmp_t inact_pbm;

    SOC_PBMP_COUNT(pr_bmp->add_pbm_log, add_count);
    SOC_PBMP_COUNT(pr_bmp->act_pbm_log, act_count);

    SOC_PBMP_ASSIGN(inact_pbm,pr_bmp->inact_pbm_log);
    SOC_PBMP_REMOVE(inact_pbm,pr_bmp->act_pbm_log);
    SOC_PBMP_COUNT(inact_pbm, inact_count);

    nport = add_count + act_count + inact_count;

    if (nport <= 0) {
        return BCM_E_NONE;
    }

    add_res = sal_alloc(sizeof(soc_port_resource_t) * nport,
                        "Add_port_resources");
    if (add_res == NULL) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Unable to allocate memory for add "
                              "resource array in FlexPort operation\n")));
        return SOC_E_MEMORY;
    }
    sal_memset(add_res, 0, sizeof(soc_port_resource_t) * nport);
    i = 0;

    SOC_PBMP_ITER(inact_pbm, port) {
        add_res[i].logical_port = port;
        add_res[i].physical_port = -1;
        i++;
    }
    SOC_PBMP_ITER(pr_bmp->add_pbm_log, port) {
        add_res[i].logical_port = port;
        add_res[i].physical_port = SOC_INFO(unit).port_l2p_mapping[port];
        i++;
    }
    SOC_PBMP_ITER(pr_bmp->act_pbm_log, port) {
        add_res[i].logical_port = port;
        add_res[i].physical_port = SOC_INFO(unit).port_l2p_mapping[port];
        i++;
    }

    if (SOC_USE_PORTCTRL(unit)) {
        rv = soc_esw_portctrl_pm_ports_add(unit, nport, add_res);
    } else {
        int okay;
        for (i = 0; i < nport; i++) {
            rv = _bcm_port_probe(unit, add_res[i].logical_port, &okay);
            if (BCM_FAILURE(rv) || !okay) {
                break;
            }
            rv = _bcm_port_mode_setup(unit, add_res[i].logical_port, FALSE);
            if (BCM_FAILURE(rv)) {
                break;
            }
        }
    }

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Unable to attach MAC/PHY in "
                              "FlexPort operation\n")));
        sal_free(add_res);
        return rv;
    }

    /* Set port configured with force_lb to PHY loopback */
    for (i = 0; i < nport; i++) {
        if (add_res[i].physical_port == -1) {
            continue;
        }
        port = add_res[i].logical_port;
        phy_port = add_res[i].physical_port;
        if (BCMI_PORT_DRV_PHY_INFO(unit, phy_port).force_lb) {
            rv = bcm_esw_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_PHY);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_PORT,
                          (BSL_META_U(unit,
                                      "Fail to set port %d to PHY loopback\n"),
                           port));
                sal_free(add_res);
                return rv;
            }
        }
    }

    sal_free(add_res);

    /* Set PM interface to 16B, currently this is not covered by PortMod */
    if (soc_feature(unit, soc_feature_clmac_16byte_interface_mode)) {
        SHR_BIT_ITER(pr_bmp->add_bmp_pm, SOC_MAX_NUM_BLKS, blk) {
            port = SOC_BLOCK_PORT(unit, blk);
            blk_type = SOC_BLOCK_INFO(unit, blk).type;
            if (blk_type == SOC_BLOCK_TYPE_INVALID) {
                break;
            }
            is_clport = blk_type == SOC_BLK_CLPORT ? 1 : 0;

            if (is_clport) {
                BCM_IF_ERROR_RETURN
                    (READ_CLPORT_MAC_CONTROLr(unit, port, &rval));
                soc_reg_field_set(unit, CLPORT_MAC_CONTROLr, &rval,
                                    SYS_16B_INTF_MODEf, 1);
                BCM_IF_ERROR_RETURN
                    (WRITE_CLPORT_MAC_CONTROLr(unit, port, rval));
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_pm_setup
 * Purpose:
 *      Setup port(s) in Port Macro if portmod enabled.
 *
 * Parameters:
 *      unit   - (IN) Unit number.
 *      port_schedule_state - (IN) Port schedule state.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
STATIC int
_bcmi_xgs5_port_pm_setup(int unit, soc_port_schedule_state_t *port_schedule_state)
{
    int nport, i;
    soc_port_resource_t *resource, *pr;
    bcm_port_t port;
    soc_info_t *si = &SOC_INFO(unit);
    soc_asf_mode_e asf_mode;
    int pause;

    nport    = port_schedule_state->nport;
    resource = port_schedule_state->resource;
    if (SOC_USE_PORTCTRL(unit)) {
        for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
            /*
             * Check if 'delete' port needs to be added (NULL driver).
             */
            port = pr->logical_port;
            if ((pr->physical_port == -1) &&
                !SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
                /* Port is deleted and is not inactive, nothing to add */
                continue;
            }
            if (IS_HG_PORT(unit, port)) {
                pause = 0;
            } else {
                pause = 1;
            }
            asf_mode = port_schedule_state->cutthru_prop.asf_modes[port];
            BCM_IF_ERROR_RETURN
                (soc_esw_portctrl_pause_set(unit, port, pause, 
                                            asf_mode != _SOC_ASF_MODE_SAF ? 0 : 
                                            pause));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_pm_enable
 * Purpose:
 *      Power down or power up the port macro by programming
 *      TOP_TSC_ENABLEr, TOP_TSC_ENABLE_1r
 * Parameters:
 *      unit   - (IN) Unit number.
 *      pr_bmp - (IN) Bitmaps of port/PM to be added/deleted during flexing.
 *      enable - (IN) 1: enable; 0: disable
 * Returns:
 *      SOC_E_XXX
 * Notes:
 */
STATIC int
_bcmi_xgs5_pm_enable(int unit, bcmi_xgs5_port_resource_bmp_t *pr_bmp,
                     int enable)
{
    SHR_BITDCL *pm_bmp;
    int blk, pm, i, offset, count[2], pm_max = 64;
    soc_reg_t reg[2] = {TOP_TSC_ENABLEr, TOP_TSC_ENABLE_1r};
    uint32 rval[2];

    /* TOP_TSC_ENABLEr cover PM 0-31, TOP_TSC_ENABLE_1r cover PM 32-63 */
#define PM_NUM_PER_REG 32

    if (SOC_IS_TRIDENT3X(unit) || SOC_IS_TOMAHAWK3(unit)) {
        pm_max = 32;
#ifdef BCM_HELIX5_SUPPORT
        
        if (SOC_IS_HELIX5(unit) || SOC_IS_HURRICANE4(unit)) {
            return BCM_E_NONE;
        }
#endif
    }
    for (i = 0; i < 2; i++) {
        count[i] = 0;
        rval[i] = 0;
        if (SOC_REG_IS_VALID(unit, reg[i])){
            BCM_IF_ERROR_RETURN
                (soc_reg32_get(unit, reg[i], REG_PORT_ANY, 0, &rval[i]));
        }
    }

    pm_bmp = enable ? pr_bmp->add_bmp_pm : pr_bmp->del_bmp_pm;

    SHR_BIT_ITER(pm_bmp, SOC_MAX_NUM_BLKS, blk) {
        if (SOC_BLOCK_TYPE(unit, blk) == SOC_BLOCK_TYPE_INVALID) {
            break;
        }
        if (SHR_BITGET(SOC_INFO(unit).pm_ref_master_bitmap, blk)) {
            continue;
        }
        if (SOC_BLOCK_TYPE(unit, blk) != SOC_BLK_CLPORT &&
            SOC_BLOCK_TYPE(unit, blk) != SOC_BLK_CDPORT) {
            continue;
        }
        pm = SOC_BLOCK_NUMBER(unit, blk);
        if (pm >= pm_max) {
            continue;
        }

#if defined(BCM_TRIDENT3_SUPPORT)
        /* PM31 is the last PM in the LED scan chain in TD3.  Since the LED
           firmware will always program LAST_PORT=124, we need to ensure that this
           PM is always functional and will respond to the LED accumulation logic */
        if (SOC_IS_TRIDENT3(unit) && (pm == 31) && (!enable)) {
            continue;
        }
#endif /* BCM_TRIDENT3_SUPPORT */

        i = pm / PM_NUM_PER_REG;
        offset = pm % PM_NUM_PER_REG;
        if (enable) {
            rval[i] |= (1 << offset);
        } else {
            /* Disabling of TOP_TSC_ENABLE for TH3 causes link flapping in
             * PMs not being modified, so do not allow TOP_TSC_ENABLE
             * to be disabled during TH3 flex
             */
            if (!SOC_IS_TOMAHAWK3(unit)) {
                rval[i] &= ~(1 << offset);
            }
        }
        count[i]++;
        /* If shut down or power up many port blocks at the same time, PCI
         * bus would become abnormal and BUS error happened. So update
         * TOP_TSC_ENABLEx for 10 blocks change each time. */
        if (count[i] > 10 && SOC_REG_IS_VALID(unit, reg[i])) {
            BCM_IF_ERROR_RETURN(
                soc_reg32_set(unit, reg[i], REG_PORT_ANY, 0, rval[i]));
            count[i] = 0;
            sal_usleep(100000);
        }
    }
    for (i = 0; i < 2; i++) {
        if (count[i] > 0 && SOC_REG_IS_VALID(unit, reg[i])) {
            BCM_IF_ERROR_RETURN(
                soc_reg32_set(unit, reg[i], REG_PORT_ANY, 0, rval[i]));
            sal_usleep(100000);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_soc_info_delete
 * Purpose:
 *      Clear port information in SOC_INFO SW
 *
 * Parameters:
 *      unit                - (IN) Unit number.
 *      port_schedule_state - (IN) Port schedule state.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_bcmi_xgs5_port_soc_info_delete(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_persist_t *sop = SOC_PERSIST(unit);
    soc_port_resource_t *resource, *pr;
    soc_pmvco_info_t pm_vco_info;
    int i, pipe;
    int port, phy_port, mmu_port;
    int nport;
    int rv = BCM_E_NONE;

    SOC_CONTROL_LOCK(unit);
    /* Clear counter map */
    rv = _bcmi_xgs5_port_soc_counter_delete(unit, port_schedule_state);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    nport = port_schedule_state->nport;
    resource = port_schedule_state->resource;

    for (i = 0, pr = resource; i < nport; i++, pr++) {
        port = pr->logical_port;
        phy_port = si->port_l2p_mapping[port];
        mmu_port = si->port_p2m_mapping[phy_port];

        if (pr->physical_port == -1) {
            if (pr->op & (BCMI_XGS5_PORT_RESOURCE_OP_DEL |
		        BCMI_XGS5_PORT_RESOURCE_OP_REMAP )) {

                /*
                 * If 'inactive' flag, just add port to disabled bitmap.
                 * This is part of the legacy API behavior.
                 */
                if (pr->flags & SOC_PORT_RESOURCE_I_MAP) {
                    SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, port);
                    continue;
                }

                /* Clear IPG configuration */
                sal_memset(&sop->ipg[port], 0, sizeof(soc_ipg_t));

                /* Port Mapping related information */
                si->port_l2p_mapping[port] = -1;
                si->port_l2i_mapping[port] = -1;
                if (phy_port != -1) {
                    si->port_p2l_mapping[phy_port] = -1;
                    if (!(SOC_IS_FIREBOLT6(unit))) {
                        si->port_p2m_mapping[phy_port] = -1;
                    }
#ifdef BCM_HELIX5_SUPPORT
                    if (SOC_IS_HELIX5(unit) &&
                        !((phy_port <= 60) && !pr->oversub)) {
                        si->port_p2m_mapping[phy_port] = mmu_port;
                    }
#endif
                }
                if (mmu_port != -1) {
                    si->port_m2p_mapping[mmu_port] = -1;
#ifdef BCM_HELIX5_SUPPORT
                    if (SOC_IS_HELIX5(unit) &&
                        !((phy_port <= 60) && !pr->oversub))
                        si->port_m2p_mapping[mmu_port] = phy_port;
#endif
                }

                /* Pipeline */
                if (phy_port != -1) {
                    pipe = si->port_pipe[port];
                    SOC_PBMP_PORT_REMOVE(si->pipe_pbm[pipe], port);
                    si->port_pipe[port] = -1;
                }

                /* Logical port related information */
                si->port_speed_max[port] = 0;
                si->port_init_speed[port] = 0;
                si->port_num_lanes[port] = 0;
                si->port_group[port] = -1;
                si->port_serdes[port] = -1;

                si->port_fec_type[port] = _SHR_PORT_PHY_FEC_NONE;
                si->port_phy_lane_config[port] = -1;
                si->port_link_training[port] = 0;
                si->port_num_subport[port] = -1;
                si->port_lc_mapping[port] = -1;

                BCM_PBMP_PORT_REMOVE(si->subtag_allowed_pbm, port);
                SOC_PBMP_PORT_REMOVE(si->oversub_pbm, port);
                SOC_PBMP_PORT_REMOVE(si->hg2_pbm, port);
                SOC_PBMP_PORT_REMOVE(si->eq_pbm, port);

                continue;
            } else if ((pr->op & (BCMI_XGS5_PORT_RESOURCE_OP_SPEED |
                                  BCMI_XGS5_PORT_RESOURCE_OP_LANES)) &&
                       (!(pr->flags & SOC_PORT_RESOURCE_SPEED))) {
                si->port_speed_max[pr->logical_port] = 0;
            }
        }
    }

    /* Clear out soc info's pm vco info if it's going to change */
    for (i = 0; i < SOC_MAX_NUM_BLKS; i++) {
        rv = soc_esw_portctrl_pm_vco_fetch(unit, i, &pm_vco_info);

        if (BCM_SUCCESS(rv)) {
            if (pm_vco_info.is_tvco_new != 0 ||
                pm_vco_info.is_ovco_new != 0) {

                si->pm_vco_info[i].tvco = -1;
                si->pm_vco_info[i].ovco = -1;
                si->pm_vco_info[i].is_tvco_new = 0;
                si->pm_vco_info[i].is_ovco_new = 0;
            }
        }
    }

    /* Clear port ptype bitmap and block information */
    rv = _bcmi_xgs5_port_soc_info_ptype_block_delete(unit, port_schedule_state);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

cleanup:
    SOC_CONTROL_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *      _bcmi_xgs5_port_reconfigure_bmp_get
 * Purpose:
 *      Get bitmaps of newly added logical ports, physical ports, port macros
 *      and deleted logical port, physical port, port macro during Flexport.
 * Parameters:
 *      unit                - (IN) Unit number.
 *      port_schedule_state - (IN) Port schedule state.
 *      pr_bmp              - (OUT) Bitmaps of port, port macro that to be
 *                                  added and deleted.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumes:
 *      - Basic checks are already performed by caller:
 *          . Array is in order (deletes are first)
 *          . Logical and physical ports are addressable
 *      - Caller has lock.
 */
STATIC int
_bcmi_xgs5_port_reconfigure_bmp_get(int unit,
                            soc_port_schedule_state_t *port_schedule_state,
                            bcmi_xgs5_port_resource_bmp_t *pr_bmp)
{
    soc_info_t *si = &SOC_INFO(unit);
    int nport = port_schedule_state->nport;
    soc_port_resource_t *resource = port_schedule_state->resource, *pr;
    int blk, logic_port, phy_port, old_phy_port, port_idx, local_phy_port;
    int i, pipe;
    pbmp_t pbm_and;
    char block_valid[SOC_MAX_NUM_BLKS];

    sal_memset(pr_bmp, 0, sizeof(bcmi_xgs5_port_resource_bmp_t));

    for (i = 0, pr = resource; i < nport; i++, pr++) {
        /* Legacy speed update API doesn't require port add/del */
        if (pr->flags & SOC_PORT_RESOURCE_SPEED) {
            continue;
        }

        logic_port = pr->logical_port;
        phy_port = pr->physical_port;
        old_phy_port = si->port_l2p_mapping[logic_port];
        pipe = pr->pipe;

        /* If 'inactive' flag, update the inact_pbm_log/act_pbm_log */
        if (pr->flags & SOC_PORT_RESOURCE_I_MAP) {
            if (pr->physical_port == -1) {
                SOC_PBMP_PORT_ADD(pr_bmp->inact_pbm_log, logic_port);
            } else {
                SOC_PBMP_PORT_ADD(pr_bmp->act_pbm_log, logic_port);
            }
            continue;
        }

        if (pr->op & (BCMI_XGS5_PORT_RESOURCE_OP_REMAP |
                      BCMI_XGS5_PORT_RESOURCE_OP_ADD |
                      BCMI_XGS5_PORT_RESOURCE_OP_DEL |
                      BCMI_XGS5_PORT_RESOURCE_OP_LANES |
                      BCMI_XGS5_PORT_RESOURCE_OP_ENCAP_FLEX |
                      BCMI_XGS5_PORT_RESOURCE_OP_SPEED)) {
            if (pr->physical_port == -1) {
                local_phy_port = old_phy_port -
                        BCMI_PORT_DRV_DEV_INFO(unit)->pipe_phy_port_base[pipe];

                SOC_PBMP_PORT_ADD(pr_bmp->del_pbm_log, logic_port);
                SOC_PBMP_PORT_ADD(pr_bmp->del_pbm_phy[pipe], local_phy_port);
            } else {
                local_phy_port = phy_port -
                        BCMI_PORT_DRV_DEV_INFO(unit)->pipe_phy_port_base[pipe];

                if ((pr->op & (BCMI_XGS5_PORT_RESOURCE_OP_REMAP |
                               BCMI_XGS5_PORT_RESOURCE_OP_ADD |
                               BCMI_XGS5_PORT_RESOURCE_OP_LANES)) ||
                    SOC_PBMP_MEMBER(pr_bmp->del_pbm_log, logic_port)) {
                    SOC_PBMP_PORT_ADD(pr_bmp->add_pbm_phy[pipe],
                                      local_phy_port);
                    SOC_PBMP_PORT_ADD(pr_bmp->add_pbm_log, logic_port);
                }
            }
        }
    }

    sal_memcpy(block_valid, si->block_valid, SOC_MAX_NUM_BLKS);
    for (pipe = 0; pipe < si->num_pipe; pipe++) {
        SOC_PBMP_ASSIGN(pbm_and, pr_bmp->add_pbm_phy[pipe]);
        SOC_PBMP_AND(pbm_and, pr_bmp->del_pbm_phy[pipe]);
        SOC_PBMP_REMOVE(pr_bmp->add_pbm_phy[pipe], pbm_and);
        SOC_PBMP_REMOVE(pr_bmp->del_pbm_phy[pipe], pbm_and);

        SOC_PBMP_ITER(pr_bmp->add_pbm_phy[pipe], local_phy_port) {
            phy_port = local_phy_port +
                        BCMI_PORT_DRV_DEV_INFO(unit)->pipe_phy_port_base[pipe];

            for (port_idx = 0; port_idx < SOC_DRIVER(unit)->port_num_blktype;
                 port_idx++) {

                blk = SOC_PORT_IDX_BLOCK(unit, phy_port, port_idx);
                if (blk < 0) { /* End of block list of each port */
                    break;
                }
                block_valid[blk] ++;
            }
        }

        SOC_PBMP_ITER(pr_bmp->del_pbm_phy[pipe], local_phy_port) {
            phy_port = local_phy_port +
                        BCMI_PORT_DRV_DEV_INFO(unit)->pipe_phy_port_base[pipe];

            for (port_idx = 0; port_idx < SOC_DRIVER(unit)->port_num_blktype;
                 port_idx++) {

                blk = SOC_PORT_IDX_BLOCK(unit, phy_port, port_idx);
                if (blk < 0) { /* End of block list of each port */
                    break;
                }
                if (!block_valid[blk]) {
                    return BCM_E_INTERNAL;
                }
                block_valid[blk]--;
            }
        }
    }

    for (blk = 0; blk < SOC_MAX_NUM_BLKS; blk ++) {
        if (block_valid[blk] && (!(si->block_valid[blk]))) {
            SHR_BITSET(pr_bmp->add_bmp_pm, blk);
        }
        if ((!block_valid[blk]) && (si->block_valid[blk])) {
            SHR_BITSET(pr_bmp->del_bmp_pm, blk);
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_speed_update
 * Purpose:
 *      Update port for the case only speed or lanes change.
 *
 * Parameters:
 *      unit                - (IN) Unit number.
 *      port_schedule_state - (IN) Port schedule state.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumes:
 *      - Basic checks are already performed by caller:
 *          . Array is in order (deletes are first)
 *          . Logical and physical ports are addressable
 *      - Caller has lock.
 */
STATIC int
_bcmi_xgs5_port_speed_update(int unit,
                            soc_port_schedule_state_t *port_schedule_state)
{
    soc_port_resource_t *resource, *pr;
    int i;
    int port, phy_port;
    int nport;
    int count_width;
    uint32 regval = 0;

    nport = port_schedule_state->nport;
    resource = port_schedule_state->resource;

    for (i = 0, pr = resource; i < nport; i++, pr++) {
        port = pr->logical_port;
        phy_port = pr->physical_port;

        if (phy_port == -1 ||
            !(pr->op & BCMI_XGS5_PORT_RESOURCE_OP_SPEED) ||
            (pr->op & (BCMI_XGS5_PORT_RESOURCE_OP_ADD |
                      BCMI_XGS5_PORT_RESOURCE_OP_REMAP))) {
            continue;
        }

        if (SOC_REG_IS_VALID(unit, PORT_INITIAL_COPY_COUNT_WIDTHr)) {
        count_width = pr->hsp ? 1 : 0;

            if (!soc_feature(unit, soc_feature_skip_port_initial_copy_count)) {
                soc_reg_field_set(unit, PORT_INITIAL_COPY_COUNT_WIDTHr, &regval,
                        BIT_WIDTHf, count_width);
                BCM_IF_ERROR_RETURN(WRITE_PORT_INITIAL_COPY_COUNT_WIDTHr
                        (unit, port, regval));
            }
        }

        /* For speed change operations, update link delay */
        BCM_IF_ERROR_RETURN(_bcm_esw_port_link_delay_update(unit, port,
                                                            pr->speed));
    }

    return BCM_E_NONE;
}

STATIC int
_bcmi_xgs5_port_pm_callback_unregister(int unit,
                                soc_port_schedule_state_t *port_schedule_state)
{
    soc_port_resource_t *resource, *pr;
    int logic_port;
    int nport, i;

    nport = port_schedule_state->nport;
    resource = port_schedule_state->resource;

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        /*
         * If not a delete entry, don't do anything
         * If 'inactive' flag, don't do anything
         */
        if (pr->physical_port != -1 ||
            ((pr->flags & SOC_PORT_RESOURCE_I_MAP) &&
             (pr->op == BCMI_XGS5_PORT_RESOURCE_OP_DEL))) {
            continue;
        }
        logic_port = pr->logical_port;
        /*unregister the portmod callback*/
        BCM_IF_ERROR_RETURN
            (soc_esw_portctrl_portmod_callback_unregister(unit, logic_port));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_resource_configure
 * Purpose:
 *      Configure device ports based on given FlexPort information.
 *
 *      This routine executes the FlexPort operation.
 * Parameters:
 *      unit                - (IN) Unit number.
 *      port_schedule_state - (IN) Port schedule state.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumes:
 *      - Basic checks are already performed by caller:
 *          . Array is in order (deletes are first)
 *          . Logical and physical ports are addressable
 *      - Caller has lock.
 */
STATIC int
_bcmi_xgs5_port_reconfigure(int unit,
                            bcm_port_resource_t *resource,
                            soc_port_schedule_state_t *port_schedule_state)
{
    soc_info_t *si = &SOC_INFO(unit);
    int blk, logic_port;
    bcmi_xgs5_port_resource_bmp_t pr_bmp;
    int rv = BCM_E_NONE;

    _bcmi_xgs5_port_reconfigure_bmp_get(unit, port_schedule_state, &pr_bmp);

    /* Enable Port Macro in TOP_TSC_ENABLE */
    BCM_IF_ERROR_RETURN
        (_bcmi_xgs5_pm_enable(unit, &pr_bmp, 1));

    if (!SOC_USE_PORTCTRL(unit)) {
        /* Port Macro reset */
        BCM_IF_ERROR_RETURN
            (_bcmi_xgs5_port_pm_reset(unit, pr_bmp.add_bmp_pm,
                                      pr_bmp.add_pbm_phy));
    }
    /***********************************************
     * Because SOC reg/mem read/write functions use l2p_mapping or
     * p2m_mapping in SOC_INFO to figuire out the access address of
     * registers/memories of ports, SDK MUST follow below guideline
     * to handle ADD/DEL/REMAP operations
     *
     * - HW programming on 'older' ports MUST to be done BEFORE
     *   the SOC_INFO is updated.
     * - HW programming on 'new' ports MUST to be done AFTER
     *   the SOC_INFO is updated.
     */


    /***********************************************
     * DETACH
     *
     * - Clearing any port references in HW
     * - Removing port from Port Macro
     * - Clear port information in SOC_INFO SW
     */
    COUNTER_UNLOCK(unit);
    rv = _bcmi_xgs5_port_resource_detach(unit, port_schedule_state);
    COUNTER_LOCK(unit);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /*
     * Unregister the pm callback for the speed setting.
     * soc info deletion for speed setting may cause regsiter
     * accessing fail in portmod periodic event callback.
     */
    BCM_IF_ERROR_RETURN
        (_bcmi_xgs5_port_pm_callback_unregister(unit, port_schedule_state));

    BCM_IF_ERROR_RETURN
        (_bcmi_xgs5_port_pm_delete(unit, &pr_bmp));
    if (!SOC_USE_PORTCTRL(unit)) {
        /* Power down the tsc that no longer used */
        SHR_BIT_ITER(pr_bmp.del_bmp_pm, SOC_MAX_NUM_BLKS, blk) {
            logic_port = si->block_port[blk];
            BCM_IF_ERROR_RETURN
                (soc_pm_power_mode_set(unit, logic_port, 0, 1));
        }
    }

    BCM_IF_ERROR_RETURN
        (_bcmi_xgs5_port_soc_info_delete(unit, port_schedule_state));

    /***********************************************
     * Execute FlexPort operation by calling Flexport Tier1 API
     *
     * Changes are done to the HW.
     * If any error occurs the operation, changes cannot be undone.
     */
    if (BCMI_PORT_DRV_CALL(unit)->reconfigure_ports) {
        BCM_IF_ERROR_RETURN
            (BCMI_PORT_DRV_CALL(unit)->reconfigure_ports(unit,
                                                         port_schedule_state));
    }

    /***********************************************
     * ATTACH
     *
     * - Add port information in SOC_INFO SW
     * - Configure new port in HW
     * - Add port to Port Macro
     */
    BCM_IF_ERROR_RETURN
        (_bcmi_xgs5_port_soc_info_add(unit, resource, port_schedule_state));

    BCM_IF_ERROR_RETURN
        (_bcmi_xgs5_port_pm_vco_reconfigure(unit));

    BCM_IF_ERROR_RETURN
        (_bcmi_xgs5_port_pm_add(unit, &pr_bmp));

    BCM_IF_ERROR_RETURN
        (_bcmi_xgs5_port_pm_setup(unit, port_schedule_state));

    COUNTER_UNLOCK(unit);
    rv = _bcmi_xgs5_port_resource_attach(unit, port_schedule_state);
    COUNTER_LOCK(unit);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Update for the case that only SPEED or LANES change */
    BCM_IF_ERROR_RETURN
        (_bcmi_xgs5_port_speed_update(unit, port_schedule_state));

    /* Update TDM info */
    if (BCMI_PORT_DRV_CALL(unit)->post_flexport_tdm) {
        BCM_IF_ERROR_RETURN
            (BCMI_PORT_DRV_CALL(unit)->post_flexport_tdm(unit,
                                                        port_schedule_state));
    }

    /* Update Port Macro */
    if (BCMI_PORT_DRV_CALL(unit)->port_macro_update) {
        BCM_IF_ERROR_RETURN
            (BCMI_PORT_DRV_CALL(unit)->port_macro_update(unit,
                                                         port_schedule_state));
    }

    /* Disable Port Macro in TOP_TSC_ENABLE */
    BCM_IF_ERROR_RETURN
        (_bcmi_xgs5_pm_enable(unit, &pr_bmp, 0));

    /* Update the port related reg/mem in IP/EP after flexport */
    BCM_IF_ERROR_RETURN
        (_bcmi_xgs5_port_ipep_reconfigure(unit, pr_bmp.add_pbm_log,
                                          pr_bmp.del_pbm_log));

    /* Update cos_map info */
    if (BCMI_PORT_DRV_CALL(unit)->port_cosmap_update) {
        BCM_IF_ERROR_RETURN
            (BCMI_PORT_DRV_CALL(unit)->port_cosmap_update(unit, pr_bmp.del_pbm_log, 0));
    }

    /* Update cos_map info */
    if (BCMI_PORT_DRV_CALL(unit)->port_cosmap_update) {
        BCM_IF_ERROR_RETURN
            (BCMI_PORT_DRV_CALL(unit)->port_cosmap_update(unit, pr_bmp.add_pbm_log, 1));
    }

    /* Go through PMs and configure broadcast block and last PM accordingly */
    BCM_IF_ERROR_RETURN
        (_bcmi_xgs5_port_sbus_bcast_setup(unit));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_resource_execute
 * Purpose:
 *      Perform a FlexPort operation.
 *      This includes changes that involve:
 *      - Logical to physical port mapping
 *      - Speed
 *      - Number of PHY lanes
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 *      soc_resource - (IN) SOC Port Resource
 *      port_schedule_state - (IN) SOC port schedule state
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Assumes caller has lock.
 *      Assumes BCM data has been validated.
 */
STATIC int
_bcmi_xgs5_port_resource_execute(int unit, int nport,
                                 bcm_port_resource_t *resource,
                                 soc_port_resource_t *soc_resource,
                                 soc_port_schedule_state_t *port_schedule_state)
{
    int rv;
    int op, i;


    /* SOC Port Structure initialization */
    BCM_IF_ERROR_RETURN
        (BCMI_PORT_DRV_CALL(unit)->soc_resource_init(unit, nport, resource,
                                                 soc_resource));
    /* Check type of FlexPort operations */
    BCM_IF_ERROR_RETURN
        (_bcmi_xgs5_port_resource_op_set(unit, nport, resource, soc_resource,
                                         &op));

    LOG_VERBOSE(BSL_LS_BCM_PORT,
                     (BSL_META_U(unit,
                             "FlexPort operations 0x%x: %s %s %s %s %s %s %s %s %s\n"),
                      op,
                      (op & BCMI_XGS5_PORT_RESOURCE_OP_REMAP) ? "remap" : "",
                      (op & BCMI_XGS5_PORT_RESOURCE_OP_SPEED) ? "speed" : "",
                      (op & BCMI_XGS5_PORT_RESOURCE_OP_LANES) ? "lanes" : "",
                      (op & BCMI_XGS5_PORT_RESOURCE_OP_ENCAP) ? "encap" : "",
                      (op & BCMI_XGS5_PORT_RESOURCE_OP_ENCAP_FLEX) ? "encap_flex" : "",
                      (op & BCMI_XGS5_PORT_RESOURCE_OP_ADD)   ? "add"   : "",
                      (op & BCMI_XGS5_PORT_RESOURCE_OP_DEL)   ? "delete": "",
                      (op & BCMI_XGS5_PORT_RESOURCE_OP_ACTIVE)? "active": "",
                      (op & BCMI_XGS5_PORT_RESOURCE_OP_CALENDAR)? "calendar": ""));

    if (((op & BCMI_XGS5_PORT_RESOURCE_OP_ENCAP) ||
         (op & BCMI_XGS5_PORT_RESOURCE_OP_ENCAP_FLEX)) &&
        (!soc_feature(unit, soc_feature_xport_convertible))) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Encapsulation converting between HIGIG and IEEE "
                              "is not allowed.\n")));
        return BCM_E_UNAVAIL;
    }

    /*
    * Operations:
    *     NONE                             => Nothing to do
    *     ADD/DEL/REMAP/SPEED/LANES        => FlexPort sequence
    *     ENCAP                            => Encap set
    */
    if (op == BCMI_XGS5_PORT_RESOURCE_OP_NONE) {
        return BCM_E_NONE;     /* No changes, just return */
    }

    if ((op & BCMI_XGS5_PORT_RESOURCE_OP_ADD) ||
        (op & BCMI_XGS5_PORT_RESOURCE_OP_DEL) ||
        (op & BCMI_XGS5_PORT_RESOURCE_OP_REMAP) ||
        (op & BCMI_XGS5_PORT_RESOURCE_OP_LANES) ||
        (op & BCMI_XGS5_PORT_RESOURCE_OP_SPEED) ||
        (op & BCMI_XGS5_PORT_RESOURCE_OP_ACTIVE) ||
        (op & BCMI_XGS5_PORT_RESOURCE_OP_ENCAP_FLEX) ||
        (op & BCMI_XGS5_PORT_RESOURCE_OP_CHL) ||
        (op & BCMI_XGS5_PORT_RESOURCE_OP_CALENDAR)) {

        LOG_VERBOSE(BSL_LS_BCM_PORT,
                 (BSL_META_U(unit,
                             "Execute FlexPort sequence\n")));

        /* port_schedule_state structure initialization */
        BCM_IF_ERROR_RETURN
            (_bcmi_xgs5_port_soc_schedule_state_init(unit, nport, op,
                                                     soc_resource,
                                                     port_schedule_state));

        /* Validate SOC Port Resource data */
        BCM_IF_ERROR_RETURN
            (_bcmi_xgs5_port_resource_validate(unit, port_schedule_state));

        /* Pause linkscan */
        soc_linkscan_pause(unit);

        /* Pause counter collection */
        COUNTER_LOCK(unit);

        /* Flexport operations */
        rv = _bcmi_xgs5_port_reconfigure(unit, resource, port_schedule_state);

        /* Resume counter collection */
        COUNTER_UNLOCK(unit);

        /* Resume linkscan */
        soc_linkscan_continue(unit);

        if (rv != BCM_E_NONE) {
            LOG_ERROR(BSL_LS_BCM_PORT,
                     (BSL_META_U(unit,
                                 "Flexport operations failed: %d (%s)\n"),
                                 rv, bcm_errmsg(rv)));
            LOG_ERROR(BSL_LS_BCM_PORT,
                      (BSL_META_U(unit,
                                  "Changes to device may have been partially "
                                  "executed.  System may be unstable.\n")));
            return rv;
        }

    }

    if ((op & BCMI_XGS5_PORT_RESOURCE_OP_ENCAP) ||
        (op & BCMI_XGS5_PORT_RESOURCE_OP_ENCAP_FLEX)) {
        LOG_VERBOSE(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit,
                                "Execute Encap change\n")));
        for (i = 0; i < nport; i++) {
            if (!(soc_resource[i].op & BCMI_XGS5_PORT_RESOURCE_OP_ENCAP) &&
                !(soc_resource[i].op & BCMI_XGS5_PORT_RESOURCE_OP_ENCAP_FLEX)) {
                continue;
            }
            BCM_IF_ERROR_RETURN
                (bcmi_xgs5_port_encap_speed_check(unit, resource[i].port,
                                                  resource[i].encap,
                                                  resource[i].speed));
            if (SOC_PORT_USE_PORTCTRL(unit, resource[i].port)) {
                BCM_IF_ERROR_RETURN
                    (bcmi_esw_portctrl_encap_set(unit, resource[i].port,
                                                 resource[i].encap, TRUE));
            } else {
                BCM_IF_ERROR_RETURN
                    (bcmi_esw_port_encap_set(unit, resource[i].port,
                                            resource[i].encap));
            }
        }
    }

    /* Update speed */
    for (i = 0; i < nport; i++) {
        if (resource[i].physical_port == -1 ||
            soc_resource[i].op == BCMI_XGS5_PORT_RESOURCE_OP_ENCAP ||
            soc_resource[i].op == BCMI_XGS5_PORT_RESOURCE_OP_ENCAP_FLEX) {
            continue;
        }
        if (((resource[i].speed < BCMI_PORT_DRV_DEV_INFO(unit)->tdm_speed_min) 
#if defined(BCM_TRIDENT3_SUPPORT)
            || SOC_IS_TRIDENT3X(unit)
#endif
            )
                &&
            BCMI_PORT_DRV_CALL(unit)->no_tdm_speed_update) {
            BCM_IF_ERROR_RETURN
                (BCMI_PORT_DRV_CALL(unit)->no_tdm_speed_update(unit,
                                                            resource[i].port,
                                                            resource[i].speed));
        }
    }

    /*
     * Bring new ports down to ensure port state is proper
     */
    for (i = 0; i < nport; i++) {
        if ((resource[i].physical_port == -1) ||
            (resource[i].flags & SOC_PORT_RESOURCE_SPEED)) {
            continue;
        }
        LOG_VERBOSE(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit,
                                "Disable port %d\n"), resource[i].port));

        BCM_IF_ERROR_RETURN
            (bcm_esw_port_enable_set(unit, resource[i].port, 0));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_resource_multi_set
 * Purpose:
 *      Modify the following port resources:
 *      - Logical to physical port mapping
 *      - Speed
 *      - Number of PHY lanes
 *      - Encapsulation mode
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      Assumes caller has lock.
 */
int
bcmi_xgs5_port_resource_multi_set(int unit, 
                                  int nport, bcm_port_resource_t *resource)
{
    soc_port_resource_t *soc_resource;
    soc_port_schedule_state_t *port_schedule_state;
    int rv;

    if (resource == NULL) {
        return BCM_E_PARAM;
    }

    if ((BCMI_PORT_DRV_CALL(unit)->reconfigure_ports == NULL) ||
        (BCMI_PORT_DRV_CALL(unit)->soc_resource_init == NULL)) {
        return BCM_E_UNAVAIL;
    }

    /*
     * Validate function input requirements
     *
     * This steps checks for function semantics and guidelines
     * from the API perspective.
     */
    BCM_IF_ERROR_RETURN
        (_bcmi_xgs5_port_resource_input_validate(unit, nport, resource));

    /* Allocate memory for SOC Port Resource array data structure */
    soc_resource = sal_alloc(sizeof(soc_port_resource_t) * nport,
                             "port_resource");
    if (soc_resource == NULL) {
        return BCM_E_MEMORY;
    }

    /* Allocate memory for soc_port_schedule_state_t structure */
    port_schedule_state = sal_alloc(sizeof(soc_port_schedule_state_t),
                                    "port schedule state");
    if (port_schedule_state == NULL) {
        sal_free(soc_resource);
        return BCM_E_MEMORY;
    }

    rv = _bcmi_xgs5_port_resource_execute(unit, nport, resource, soc_resource,
                                          port_schedule_state);

    if (rv != BCM_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                 (BSL_META_U(unit,
                             "Port resource execute failed: %d (%s)\n"),
                             rv, bcm_errmsg(rv)));
    }
    sal_free(soc_resource);
    sal_free(port_schedule_state);

    return rv;
}

/*
 * Function:
 *      bcmi_xgs5_port_resource_traverse
 * Purpose:
 *      Iterates over the port resource configurations on a given
 *      unit and calls user-provided callback for every entry.
 * Parameters:
 *      unit       - (IN) Unit number.
 *      trav_fn    - (IN) Callback function to execute.
 *      user_data  - (IN) Pointer to any user data.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_xgs5_port_resource_traverse(int unit, 
                                 bcm_port_resource_traverse_cb trav_fn, 
                                 void *user_data)
{
    bcm_port_t port;
    bcm_port_resource_t resource;
    soc_info_t *si = &SOC_INFO(unit);

    PARAM_NULL_CHECK(trav_fn);

    for (port = 0; port < SOC_MAX_NUM_PORTS ; port++) {
        /* Skip logical ports with no assignment and management ports */
        /* coverity[overrun-local] */
        if ((si->port_l2p_mapping[port] == -1) ||
            IS_CPU_PORT(unit, port) || IS_LB_PORT(unit, port)) {
                continue;
        }

        /* Get port information */
        if (BCM_FAILURE(bcmi_xgs5_port_resource_get(unit, port, &resource))) {
            continue;
        }

        /* Call user-provided callback routine */
        BCM_IF_ERROR_RETURN(trav_fn(unit, &resource, user_data));
    }

    return BCM_E_NONE;
}

/***************************Legacy API *******************************/
/*
 * Function:
 *      bcmi_xgs5_port_lanes_set
 * Purpose:
 *      Set the number of PHY lanes for the given port.
 *
 *      This function should only be used to support the legacy
 *      FlexPort API bcm_port_control_set(... , bcmPortControlLanes).
 *
 *      The legacy API does not delete the port mappings when flexing
 *      to fewer lanes.  Ports which lanes are used by the base
 *      physical port becomes inactive.
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port     - (IN) Logical port.
 *      lanes    - (IN) Number of lanes to set on given port.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      - Only to be used by legacy API to support flexport behavior
 *        bcm_port_control_set(... , bcmPortControlLanes).
 *      - Supports only up to 4 lanes (TSC-4).
 *      - 'port' must be in BCM port format (non-GPORT) and mapped to
 *        the base physical port.
 *      - Several calls to legacy API may be required to
 *        achieve desired configuration.
 */

int
bcmi_xgs5_port_lanes_set(int unit, bcm_port_t port, int lanes)
{
    soc_info_t *si = &SOC_INFO(unit);
    bcm_port_resource_t cur_resource, resource[8];  /* Max size to support up to 4 lanes */
    int max_array_cnt;
    int i;
    int phy_port;
    int speed;
    int num_ports_clear;
    int num_ports_new;

    bcm_port_resource_t_init(&cur_resource);
    BCM_IF_ERROR_RETURN(bcm_esw_port_resource_get(unit, port, &cur_resource));

    /*
     * Select speed based on number of lanes and the encap
     */
    switch(lanes) {
    case 1:
        speed = (cur_resource.encap == BCM_PORT_ENCAP_IEEE) ? 25000 : 27000;
        break;
    case 2:
        speed = (cur_resource.encap == BCM_PORT_ENCAP_IEEE) ? 50000 : 53000;
        break;
    case 4:
        speed = (cur_resource.encap == BCM_PORT_ENCAP_IEEE) ? 100000 : 106000;
        break;
    default:
        /* Support legacy API up to 4 lanes */
        return BCM_E_PARAM;
    }

    /* Check current number of lanes */
    if (cur_resource.lanes == lanes) {
        return BCM_E_NONE;
    }

    max_array_cnt = COUNTOF(resource);
    sal_memset(resource, 0, sizeof(resource));

    /*
     * Build port resource array
     * Certain information is derived from 'best' guess,
     * similar to the legacy behaviour.
     */
    switch (cur_resource.lanes) {
    case 1:
        if (lanes == 2) {
            num_ports_clear = 2;
            num_ports_new = 1;
        } else {
            num_ports_clear = 4;
            num_ports_new = 1;
        }
        break;
    case 2:
        if (lanes == 1) {
            num_ports_clear = 2;
            num_ports_new = 2;
        } else {
            num_ports_clear = 4;
            num_ports_new = 1;
        }
        break;
    case 4:
        if (lanes == 1) {
            num_ports_clear = 4;
            num_ports_new = 4;
        } else {
            num_ports_clear = 4;
            num_ports_new = 2;
        }
        break;
    default:
        return BCM_E_PARAM;
    }

    if ((num_ports_clear + num_ports_new) > max_array_cnt) {
        return BCM_E_INTERNAL;
    }

    /*
     * Clear mapping for physical ports involved in FlexPort operation.
     * Assume physical ports are numbered consecutively in device.
     */
    phy_port = si->port_l2p_mapping[port];
    for (i = 0; i < num_ports_clear; i++) {
        resource[i].flags = SOC_PORT_RESOURCE_I_MAP;
        resource[i].port = si->port_p2l_mapping[phy_port++];
        resource[i].physical_port = -1;
    }

    /*
     * Map new ports
     *
     * Legacy API requires that logical-physical port mapping
     * is valid in the SOC_INFO SW data.
     */
    phy_port = si->port_l2p_mapping[port];
    for (; i < (num_ports_clear + num_ports_new); i++) {
        /* Check valid physical port */
        if (phy_port == -1) {
            return BCM_E_INTERNAL;
        }

        resource[i].flags = SOC_PORT_RESOURCE_I_MAP;
        resource[i].port = si->port_p2l_mapping[phy_port];
        resource[i].physical_port = phy_port;
        resource[i].lanes = lanes;
        resource[i].speed = speed;
        resource[i].encap = cur_resource.encap; /* Keep encap the same */
        phy_port += lanes;
    }

    BCM_IF_ERROR_RETURN
        (bcmi_xgs5_port_resource_multi_set(unit,
                                         (num_ports_clear + num_ports_new),
                                         resource));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_lanes_get
 * Purpose:
 *      Get the number of PHY lanes for the given port.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port     - (IN) Logical port.
 *      lanes    - (OUT) Returns number of lanes for port.
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_xgs5_port_lanes_get(int unit, bcm_port_t port, int *lanes)
{
    bcm_port_resource_t resource;

    BCM_IF_ERROR_RETURN(bcm_esw_port_resource_get(unit, port, &resource));

    *lanes = resource.lanes;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_speed_set
 * Purpose:
 *      Set the speed for the given port.
 *
 *      This function should only be used to support the legacy
 *      FlexPort API bcm_port_speed_set().
 *
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port     - (IN) Logical port.
 *      speed    - (IN) speed for given port.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      - Only to be used by legacy API to support flexport behavior
 *        bcm_port_speed_set().
 *      - 'port' must be in BCM port format (non-GPORT) and mapped to
 *        the base physical port.
 *      - Several calls to legacy API may be required to
 *        achieve desired configuration.
 */

int
bcmi_xgs5_port_speed_set(int unit, bcm_port_t port, int speed)
{
    soc_info_t *si = &SOC_INFO(unit);
    bcm_port_resource_t resource[2];
    int encap, i;

    if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
        LOG_VERBOSE(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit,
                                "bcmi_xgs5_port_speed_set: "
                                "Set speed on disabled port, do nothing\n")));
        return BCM_E_NONE;
    }

    /* Do nothing except update port type in case auxiliary ports don't
     * support flexport */
    if ((!BCMI_PORT_DRV_DEV_INFO(unit)->aux_port_flexible) &&
        IS_MANAGEMENT_PORT(unit,port)) {
        BCM_IF_ERROR_RETURN
            (_bcmi_xgs5_management_port_soc_info_ptype_update(unit, 
                                                              port, speed));
        return BCM_E_NONE;
    }

    /*
     * No reconfiguration is needed if new speed matches current speed
     */
    if (speed == si->port_init_speed[port]) {
        LOG_VERBOSE(BSL_LS_BCM_PORT,
                    (BSL_META_U(unit,
                                "Speed is already configured as %d\n"),
                     speed));
        return BCM_E_NONE;
    }

    for (i = 0; i < 2; i++) {
        bcm_port_resource_t_init(&resource[i]);
    }
    /*
     * Build port resource
     */

    /* Get current port encap */
    BCM_IF_ERROR_RETURN(bcm_esw_port_encap_get(unit, port, &encap));

    /* Keep encap, lanes, phy_port the same */
    resource[0].flags = SOC_PORT_RESOURCE_I_MAP| SOC_PORT_RESOURCE_SPEED;
    resource[0].port = port;
    resource[0].physical_port = -1;

    resource[1].flags = SOC_PORT_RESOURCE_I_MAP| SOC_PORT_RESOURCE_SPEED;
    resource[1].port = port;
    resource[1].physical_port = si->port_l2p_mapping[port];
    resource[1].speed = speed;
    resource[1].lanes = si->port_num_lanes[port];
    resource[1].encap = encap;

    BCM_IF_ERROR_RETURN
        (bcmi_xgs5_port_resource_multi_set(unit, 2, resource));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_attach_dscp
 * Purpose:
 *      Set default DSCP profile index to port table
 *      Increase reference count of default DSCP profile
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_attach_dscp(int unit, bcm_port_t port)
{

    int dscp_ptr = 0;

    if (SOC_IS_FIREBOLT6(unit)) {
        /* Minimum valid index is 128. */
        dscp_ptr = 128;
    }
    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_tab_set(unit, port, _BCM_CPU_TABS_ETHER,
                               TRUST_DSCP_PTRf,
                               dscp_ptr));
    SOC_IF_ERROR_RETURN(_bcm_dscp_table_entry_reference(unit, dscp_ptr * 64,
                                                        64));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_attach_software
 * Purpose:
 *      Initialize BCM PORT info structure (bcm_port_info[unit][port])
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Same port related initialization covered in _bcm_port_software_init()
 */
int
bcmi_xgs5_port_attach_software(int unit, bcm_port_t port)
{
    _bcm_port_info_t *pinfo;
    int index, idxmax;
    int inds_bytes;
    int enabled;
    uint32 value;

    /* Free memory in case this is already allocated */
    BCM_IF_ERROR_RETURN(_bcmi_xgs5_port_software_free(unit, port));

    /* Get PORT info structure pointer */
    BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));

    /*
     * Initialization of vd_pbvl bitmap in port
     */
    idxmax = 0;
    if (SOC_MEM_IS_VALID(unit, VLAN_PROTOCOLm)) {
    idxmax = soc_mem_index_count(unit, VLAN_PROTOCOLm);

    inds_bytes = (idxmax + (_BCM_PORT_VD_PBVL_ESIZE -  1)) / \
                _BCM_PORT_VD_PBVL_ESIZE; /* Round to the next entry */

    pinfo->p_vd_pbvl = sal_alloc(inds_bytes, "vdv_info");
    if (NULL == pinfo->p_vd_pbvl) {
        return BCM_E_MEMORY;
    }

    sal_memset(pinfo->p_vd_pbvl, 0, inds_bytes);
    } else {
        pinfo->p_vd_pbvl = NULL;
    }

    /*
     * Update TPID reference count
     */
    if (soc_feature(unit, soc_feature_vlan_ctrl)) {
        if (soc_feature(unit, soc_feature_egr_vlan_control_is_memory)) {
            if (soc_feature(unit, soc_feature_egr_all_profile)) {
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_egr_lport_field_get(
                    unit, port, EGR_VLAN_CONTROL_1m,
                    OUTER_TPID_INDEXf, (uint32 *)&index));
            } else {
                BCM_IF_ERROR_RETURN(
                    READ_EGR_VLAN_CONTROL_1m(unit, MEM_BLOCK_ANY, port, &value));
                index = soc_mem_field32_get(unit, EGR_VLAN_CONTROL_1m,
                                                        &value, OUTER_TPID_INDEXf);
            }
        } else {
            BCM_IF_ERROR_RETURN
                (READ_EGR_VLAN_CONTROL_1r(unit, port, &value));
            index = soc_reg_field_get(unit, EGR_VLAN_CONTROL_1r, 
                                  value, OUTER_TPID_INDEXf);
        }

        BCM_IF_ERROR_RETURN
            (_bcm_fb2_outer_tpid_tab_ref_count_add(unit, index, 1));

        enabled = 0;
        BCM_IF_ERROR_RETURN
            (_bcm_esw_port_config_get(unit, port,
                                      _bcmPortOuterTpidEnables, &enabled));
        index = 0;
        while (enabled) {
            if (enabled & 1) {
                    BCM_IF_ERROR_RETURN
                        (_bcm_fb2_outer_tpid_tab_ref_count_add(unit, index, 1));
            }
            enabled = enabled >> 1;
            index++;
        }
    }

#if defined(BCM_TRIDENT3_SUPPORT)
      if (SOC_IS_TRIDENT3X(unit)) {
            BCM_IF_ERROR_RETURN(_bcm_esw_egr_port_tab_set(unit, port,
                              EGR_QOS_PROFILE_INDEXf,0));
      } else
#endif
    /*
     * Initialize priority map
     */
    if (soc_feature(unit, soc_feature_color_prio_map)) {
        bcm_color_t color;
        bcm_color_t color_array[] = {bcmColorGreen, bcmColorYellow, bcmColorRed};
        int priority;
        int cfi;

        for (index = 0; index < 3; index++) {
            color = color_array[index];
            for (priority = 0; priority <= 7; priority++) {
                cfi = (color == bcmColorRed) ? 1 : 0;
                BCM_IF_ERROR_RETURN
                    (bcm_esw_port_vlan_priority_unmap_set(unit, port, priority,
                                                          color, priority, cfi));
            }
        }
    }

    pinfo->dtag_mode = BCM_PORT_DTAG_MODE_NONE;

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_attach_egr_vlan_action
 * Purpose:
 *      Set the egress port's default vlan tag actions
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_attach_egr_vlan_action(int unit, bcm_port_t port)
{
    bcm_vlan_action_set_t action;

    if (soc_feature(unit, soc_feature_vlan_action) &&
        (!SOC_IS_TRIDENT3X(unit))) {

        /* Update Egress VLAN profile table count */
        _bcm_trx_egr_vlan_action_profile_entry_increment(unit,
                                    BCM_TRX_EGR_VLAN_ACTION_PROFILE_DEFAULT);

        if (IS_HG_PORT(unit, port)) {

            BCM_IF_ERROR_RETURN
                (_bcm_trx_vlan_port_egress_default_action_get(unit,
                                                             port, &action));
            /* Backward compatible defaults */
            action.ot_outer = bcmVlanActionDelete;
            action.dt_outer = bcmVlanActionDelete;
            BCM_IF_ERROR_RETURN
                (_bcm_trx_vlan_port_egress_default_action_set(unit,
                                                             port, &action));
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_bridge_port_init
 * Purpose:
 *      This function initializes PORT_BRIDGE_BMAP memory (for a normal port),
 *      and/or PORT_BRIDGE_MIRROR_BMAP (for a Higig port). The initialization is
 *      required for ensuring that port bit in one of the above memories
 *      (depending on port type), and PORT_TAB.PORT_BRIDGE bit have the same
 *      value. See notes.
 * Parameters:
 *      unit   - (IN) Unit number.
 *      port   - (IN) Logical port, BCM format.
 *      value  - 1 Set bit in PORT_BRIDGE_BMAP and/or PORT_BRIDGE_MIRROR_BMAP
 *                 memory.
 *               0 Reset bit in PORT_BRIDGE_BMAP and/or PORT_BRIDGE_MIRROR_BMAP
 *                 memory.
 *                 See Notes.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      - PORT_TAB.PORT_BRIDGE is reset in function
 *        mbcm_driver[unit]->mbcm_port_cfg_init()
 *      - If this function is called outside of port attach and detach
 *        functions then the caller must make sure that PORT_TAB.PORT_BRIDGE is
 *        programmed to the same value as in these memories, for a given port
 *        Refer to memory description of these two memories for more information
 *      - Assumes port is valid.
 *      - Assumes this is NOT called during Warmboot.
 */
STATIC int
_bcmi_xgs5_port_bridge_port_init(int unit, bcm_port_t port, int value)
{
    int rv = BCM_E_NONE;
    bcm_pbmp_t  pbmp;

    if ((IS_E_PORT(unit, port) ||
         IS_CPU_PORT(unit, port) ||
         IS_HG_PORT(unit, port)) &&
        SOC_MEM_IS_VALID(unit, PORT_BRIDGE_BMAPm)) {
        port_bridge_bmap_entry_t entry;

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, PORT_BRIDGE_BMAPm,
                            MEM_BLOCK_ANY, 0, &entry));
        soc_mem_pbmp_field_get(unit, PORT_BRIDGE_BMAPm, &entry,
                               BITMAPf, &pbmp);

        if (value) {
            SOC_PBMP_PORT_ADD(pbmp, port);
        } else {
            SOC_PBMP_PORT_REMOVE(pbmp, port);
        }

        soc_mem_pbmp_field_set(unit, PORT_BRIDGE_BMAPm, &entry,
                               BITMAPf, &pbmp);

        SOC_IF_ERROR_RETURN(soc_mem_write(unit, PORT_BRIDGE_BMAPm,
                            MEM_BLOCK_ANY, 0, &entry));
    }

    if (IS_HG_PORT(unit, port) &&
        SOC_MEM_IS_VALID(unit, PORT_BRIDGE_MIRROR_BMAPm)) {
        port_bridge_mirror_bmap_entry_t entry;

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, PORT_BRIDGE_MIRROR_BMAPm,
                            MEM_BLOCK_ANY, 0, &entry));
        soc_mem_pbmp_field_get(unit, PORT_BRIDGE_MIRROR_BMAPm, &entry,
                               BITMAPf, &pbmp);
        if (value) {
            SOC_PBMP_PORT_ADD(pbmp, port);
        } else {
            SOC_PBMP_PORT_REMOVE(pbmp, port);
        }
        soc_mem_pbmp_field_set(unit, PORT_BRIDGE_MIRROR_BMAPm, &entry,
                               BITMAPf, &pbmp);
        SOC_IF_ERROR_RETURN(soc_mem_write(unit, PORT_BRIDGE_MIRROR_BMAPm,
                            MEM_BLOCK_ANY, 0, &entry));
    }

    return rv;
}

/*
 * Function:
 *      bcmi_xgs5_port_cfg_init
 * Purpose:
 *      Initialize the port configuration according to Initial System
 *      Configuration (see init.c)
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_cfg_init(int unit, bcm_port_t port)
{
    bcm_pbmp_t port_pbmp;
    bcm_vlan_data_t vd;
    bcm_vlan_t d_vid;

    BCM_IF_ERROR_RETURN(bcm_esw_vlan_default_get(unit, &d_vid));

    SOC_PBMP_CLEAR(port_pbmp);
    BCM_PBMP_ASSIGN(port_pbmp, PBMP_ALL(unit));
    SOC_PBMP_REMOVE(port_pbmp, PBMP_TDM(unit));

    vd.vlan_tag = d_vid;
    BCM_PBMP_ASSIGN(vd.port_bitmap, port_pbmp);
    BCM_PBMP_ASSIGN(vd.ut_port_bitmap, port_pbmp);
    BCM_PBMP_REMOVE(vd.ut_port_bitmap, PBMP_CMIC(unit));

    BCM_IF_ERROR_RETURN
        (mbcm_driver[unit]->mbcm_port_cfg_init(unit, port, &vd));

    BCM_IF_ERROR_RETURN
        (_bcmi_xgs5_port_bridge_port_init(unit, port, 0));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_attach_port_probe
 * Purpose:
 *      Probe the PHY and set up the PHY and MAC for the specified ports.
 *      This is purely a discovery routine and does no configuration.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Only for Non PortMod unit.
 *      Unit with PortMod enabled probe PHY and MAC via
 *      _bcmi_xgs5_port_pm_delete/add().
 */
int
bcmi_xgs5_port_attach_port_probe(int unit, bcm_port_t port)
{
    int okay;

    if (!SOC_PORT_USE_PORTCTRL(unit, port)) {

        BCM_IF_ERROR_RETURN(_bcm_port_probe(unit, port, &okay));
        if (!okay) {
            return BCM_E_INTERNAL;
        }
        BCM_IF_ERROR_RETURN(_bcm_port_mode_setup(unit, port, FALSE));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_attach_frame_length_check
 * Purpose:
 *      Control 802.3 frame length field check in MAC
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_attach_frame_length_check(int unit, bcm_port_t port)
{
    int length_check;
    soc_reg_t reg = INVALIDr;
    uint32 rval;

    
    length_check = soc_property_get(unit, spn_MAC_LENGTH_CHECK_ENABLE, 0);
    if (IS_CL_PORT(unit, port) &&
        SOC_REG_IS_VALID(unit, CLPORT_MAC_RSV_MASKr)) {
        reg = CLPORT_MAC_RSV_MASKr;
    } else if (IS_QSGMII_PORT(unit, port)) {
        reg = GPORT_RSV_MASKr;
    } else if (SOC_REG_IS_VALID(unit, XLPORT_MAC_RSV_MASKr)) {
        reg = XLPORT_MAC_RSV_MASKr;
    }

    if (reg != INVALIDr) {
        BCM_IF_ERROR_RETURN(soc_reg32_get(unit, reg, port, 0, &rval));
        if (length_check) {
            rval |= 0x20;  /* set bit 5 to enable frame length check */
        } else {
            rval &= ~0x20; /* clear bit 5 to disable frame length check */
        }
        if (!IS_QSGMII_PORT(unit, port)) {
            /* set PFC frame detected indicator
             * No RSV[34] bit in GPORT_RSV_MASK
             */
            rval |= 1 << 18;
        }
        BCM_IF_ERROR_RETURN(soc_reg32_set(unit, reg, port, 0, rval));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_attach_rcpu_mtu
 * Purpose:
 *      Set the maximum receive frame size to JUMBO size for RCPU port
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_attach_rcpu_mtu(int unit, bcm_port_t port)
{
#ifdef BCM_RCPU_SUPPORT
    if ((uint32)port == soc_property_get(unit, spn_RCPU_PORT, -1)) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_frame_max_set(unit, port, BCM_PORT_JUMBO_MAXSZ));
    }
#endif /* BCM_RCPU_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_attach_outer_tpid
 * Purpose:
 *      Enable the default Outer TPID
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_attach_outer_tpid(int unit, bcm_port_t port)
{
    if (soc_feature(unit, soc_feature_egr_all_profile)) {
        soc_field_t flds[3] = {
            VT_MISS_UNTAGf,
            REMARK_OUTER_DOT1Pf,
            OUTER_TPID_VALIDf
        };
        uint32 vals[3] = {0, 1, 1};

        BCM_IF_ERROR_RETURN(bcm_esw_port_egr_lport_fields_set(unit,
            port, EGR_VLAN_CONTROL_1m, 3, flds, vals));
    }

    if (soc_feature(unit, soc_feature_vlan_ctrl)) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_tpid_set(unit, port,
                               _bcm_fb2_outer_tpid_default_get(unit)));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_egr_block_profile_init
 * Purpose:
 *      Initialize egress block profile pointer to invalid value
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_egr_block_profile_init(int unit, bcm_port_t port)
{
    /* Initialize egress block profile pointer to invalid value */
    BCM_IF_ERROR_RETURN
        (bcmi_esw_port_egr_prof_ptr_set(unit, port, -1));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_attach_vlan_protocol
 * Purpose:
 *      Program the VLAN_PROTOCOL_DATA_INDEX, FP_PORT_FIELD_SEL_INDEX,
 *      PROTOCOL_PKT_INDEX pointers
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_attach_vlan_protocol(int unit, bcm_port_t port)
{
    _bcm_port_info_t *pinfo;
    uint32 profile_index;
    vlan_protocol_data_entry_t vlan_protocol_data_entries[16];
    uint32 vlan_protocol_data_index =0;
    void *entries_buf[1];
    int vlan_prot_entries;
    int i;

    /* Get PORT info structure pointer */
    BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));
    if  (pinfo == NULL) {
        return BCM_E_INTERNAL;
    }

    if (!IS_LB_PORT(unit, port) && !IS_TDM_PORT(unit, port)) {
        if (SOC_MEM_IS_VALID(unit, VLAN_PROTOCOLm)) {
            sal_memset(vlan_protocol_data_entries, 0,
                       sizeof(vlan_protocol_data_entries));

            entries_buf[0] = &vlan_protocol_data_entries;
#if defined(BCM_TRIDENT3_SUPPORT)
            if (SOC_IS_TRIDENT3X(unit)) {
                /* Trident3 default profile 0 */
                BCM_IF_ERROR_RETURN(
                 _bcm_port_vlan_protocol_data_entry_get(unit, 0, 16, entries_buf));
            }
#endif
            BCM_IF_ERROR_RETURN(
                 _bcm_port_vlan_protocol_data_entry_add(unit, entries_buf,
                                 16, (uint32 *)&vlan_protocol_data_index));
        }

        if (SOC_SWITCH_BYPASS_MODE(unit) == SOC_SWITCH_BYPASS_MODE_NONE) {
            if (soc_mem_field_valid(unit, PORT_TABm,
                        VLAN_PROTOCOL_DATA_INDEXf)) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_port_tab_set(unit, port,
                               _BCM_CPU_TABS_ETHER,
                               VLAN_PROTOCOL_DATA_INDEXf,
                               vlan_protocol_data_index/16));
            }
            pinfo->vlan_prot_ptr = vlan_protocol_data_index;
        }

        if (SOC_IS_TOMAHAWK2(unit)) {
            vlan_prot_entries = soc_mem_index_count(unit, VLAN_PROTOCOLm);

            for (i = 0; i < vlan_prot_entries; i++) {
                _bcm_trx_vlan_action_profile_entry_increment(unit, 0);
            }
        }

        if (soc_mem_field_valid(unit, PORT_TABm,
            FP_PORT_FIELD_SEL_INDEXf)) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_port_tab_set(unit, port,
                           _BCM_CPU_TABS_ETHER,
                            FP_PORT_FIELD_SEL_INDEXf, port));
        }
        if (soc_mem_field_valid(unit, PORT_TABm,
                                PROTOCOL_PKT_INDEXf)) {
            if (SOC_REG_INFO(unit, PROTOCOL_PKT_CONTROLr).regtype ==
                soc_portreg) {
                    BCM_IF_ERROR_RETURN(
                        _bcm_esw_port_tab_set(unit, port,
                                           _BCM_CPU_TABS_ETHER,
                                           PROTOCOL_PKT_INDEXf,
                                           port));
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_prot_pkt_ctrl_add(unit, 0, 0,
                                            &profile_index));
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_port_tab_set(unit, port,
                                           _BCM_CPU_TABS_ETHER,
                                           PROTOCOL_PKT_INDEXf,
                                           profile_index));
            }
        }
    }
    return BCM_E_NONE;
}
/*
 * Function:
 *      bcmi_xgs5_port_attach_eee
 * Purpose:
 *      EEE config
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_attach_eee(int unit, bcm_port_t port)
{
    int value = 0, rv;
    _bcm_port_info_t *port_info;

    if (soc_feature (unit, soc_feature_eee)) {
        if (!IS_LB_PORT(unit, port) && !IS_CPU_PORT (unit, port) &&
            !IS_TDM_PORT(unit, port) && !IS_RDB_PORT(unit,port)) {
            /* EEE standard compliance Work Around:
             * Initialize the software state of native eee in MAC
             */
            BCM_IF_ERROR_RETURN
                (bcmi_esw_port_eee_cfg_set(unit, port, 0));
            /* Check if MAC supports Native EEE and set the value of
             * eee_cfg by reading the EEE status from MAC*/
            if (SOC_PORT_USE_PORTCTRL(unit, port)) {
                rv = bcmi_esw_portctrl_eee_enable_get(unit, port, &value);
            } else {
                _bcm_port_info_access(unit, port, &port_info);
                rv = MAC_CONTROL_GET(port_info->p_mac, unit, port,
                                     SOC_MAC_CONTROL_EEE_ENABLE,
                                     &value);
            }
            if (rv != BCM_E_UNAVAIL) {
                BCM_IF_ERROR_RETURN
                    (bcmi_esw_port_eee_cfg_set(unit, port, value));
            }
        }
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_attach_higig
 * Purpose:
 *      Set Higig/Higig2 mode
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_attach_higig(int unit, bcm_port_t port)
{
    /**********
     * HIGIG
     */
    if (IS_HG_PORT(unit, port)) {
        if (SOC_PORT_USE_PORTCTRL(unit, port)) {
            BCM_IF_ERROR_RETURN
                (bcmi_esw_portctrl_higig_mode_set(unit, port, TRUE));
        }
#ifdef BCM_HIGIG2_SUPPORT
        /* soc_feature_higig2 */
        if (soc_feature(unit, soc_feature_higig2)) {
            if (soc_property_port_get(unit, port, spn_HIGIG2_HDR_MODE,
                    soc_feature(unit, soc_feature_no_higig_plus) ? 1 : 0) ||
                SOC_PORT_USE_PORTCTRL(unit, port)) {
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_port_higig2_mode_set(unit, port, TRUE));
                SOC_HG2_ENABLED_PORT_ADD(unit, port);
            }
        }
#endif /* BCM_HIGIG2_SUPPORT */
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_attach_port_info_cfg
 * Purpose:
 *      Set SW PORT info
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_attach_port_info_cfg(int unit, bcm_port_t port)
{
    _bcm_port_info_t *pinfo;
    uint32 rx_los;
    int enable;
    int rv;

    /* Get PORT info structure pointer */
    BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));
    if  (pinfo == NULL) {
        return BCM_E_INTERNAL;
    }

    /***********
     * SW RX LOS configuration from PHY driver
     */
    if (IS_E_PORT(unit, port)) {
        rv = bcm_esw_port_phy_control_get(unit, port,
                                          BCM_PORT_PHY_CONTROL_SOFTWARE_RX_LOS,
                                          &rx_los);
        if (BCM_SUCCESS(rv)) {
            pinfo->rx_los = rx_los;
        }
    }

    /***********
     * SW port enable status from HW
     */
    rv = bcm_esw_port_enable_get(unit, port, &enable);
    if (BCM_SUCCESS(rv)) {
        pinfo->enable = enable;
    }

    return rv;
}

/*
 * Function:
 *      bcmi_xgs5_port_attach_egr_link_delay
 * Purpose:
 *      Set port link delay
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_attach_egr_link_delay(int unit, bcm_port_t port)
{
    int rv;
    int speed;
    uint64 rval64;
    uint32 rval32;

    /**********
     * Egress Link Delay
     */
    if (SOC_REG_PORT_VALID(unit, EGR_1588_LINK_DELAYr, port)) {
        rval32 = 0;
        SOC_IF_ERROR_RETURN(WRITE_EGR_1588_LINK_DELAYr(unit, port, rval32));
    } else if (SOC_REG_PORT_VALID(unit, EGR_1588_LINK_DELAY_64r, port)) {
        COMPILER_64_ZERO(rval64);
        SOC_IF_ERROR_RETURN(WRITE_EGR_1588_LINK_DELAY_64r(unit, port, rval64));
    } else {
        return BCM_E_NONE;
    }

    rv = bcm_esw_port_speed_get(unit, port, &speed);
    if (BCM_SUCCESS(rv)) {
        rv = _bcm_esw_port_link_delay_update(unit, port, speed);
    }

    return rv;
}

/*
 * Function:
 *      bcmi_xgs5_port_attach_stg
 * Purpose:
 *      Set STP state of port to FORWARDING
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_attach_stg(int unit, bcm_port_t port)
{
    bcm_vlan_t d_vid;

    BCM_IF_ERROR_RETURN(bcm_esw_vlan_default_get(unit, &d_vid));

    /* STG */
    BCM_IF_ERROR_RETURN
        (bcm_esw_vlan_stp_set(unit, d_vid, port, BCM_STG_STP_FORWARD));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_attach_vlan
 * Purpose:
 *      Add port into default vlan 1
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_attach_vlan(int unit, bcm_port_t port)
{
#ifndef BCM_VLAN_NO_DEFAULT_ETHER
    /* Add ports to default VLAN only if vlan policy allows it */
    bcm_pbmp_t pbmp;
    bcm_vlan_t d_vid;

    BCM_IF_ERROR_RETURN(bcm_esw_vlan_default_get(unit, &d_vid));

    /* VLAN */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    BCM_IF_ERROR_RETURN
        (bcm_esw_vlan_port_add(unit, d_vid, pbmp, pbmp));
#endif
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_attach_trunk
 * Purpose:
 *      Initializes trunk tables for newly added port
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_attach_trunk(int unit, bcm_port_t port)
{
    bcm_pbmp_t pbmp;
    higig_trunk_control_entry_t entry;
    int rv = BCM_E_NONE;

    /* TRUNK */
    if (soc_feature(unit, soc_feature_hg_trunking)) {
        
        soc_mem_lock(unit, HIGIG_TRUNK_CONTROLm);

        rv = soc_mem_read(unit, HIGIG_TRUNK_CONTROLm, MEM_BLOCK_ANY, 0, &entry);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, HIGIG_TRUNK_CONTROLm);
            return rv;
        }

        BCM_PBMP_CLEAR(pbmp);

        /* Read port bmap from hardware, and add new port to the bitmap */
        soc_mem_pbmp_field_get(unit, HIGIG_TRUNK_CONTROLm, &entry,
                               ACTIVE_PORT_BITMAPf, &pbmp);

        BCM_PBMP_PORT_ADD(pbmp, port);

        soc_mem_pbmp_field_set(unit, HIGIG_TRUNK_CONTROLm, &entry,
                           ACTIVE_PORT_BITMAPf, &pbmp);

        rv = soc_mem_write(unit, HIGIG_TRUNK_CONTROLm, MEM_BLOCK_ALL, 0,
                       &entry);

        soc_mem_unlock(unit, HIGIG_TRUNK_CONTROLm);
    }
    return rv;
}

/*
 * Function:
 *      bcmi_xgs5_port_attach_linkscan
 * Purpose:
 *      Initializes linkscan on port
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_attach_linkscan(int unit, bcm_port_t port)
{

    /* Skip linkscan if module hasn't init */
    if (_bcm_esw_lc_check_init(unit) != BCM_E_NONE) {
        return BCM_E_NONE;
    }

    /* LINKSCAN */
    BCM_IF_ERROR_RETURN
        (_soc_linkscan_hw_port_init(unit, port));

    /* Initialze to SW linkscan mode */
    BCM_IF_ERROR_RETURN(bcm_esw_linkscan_port_attach(unit, port));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_attach_stack
 * Purpose:
 *      Initializes statistic on port
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_attach_stat(int unit, bcm_port_t port)
{
    pbmp_t      pbmp;
    int         config_threshold;

    config_threshold = soc_property_get(unit, spn_BCM_STAT_JUMBO, 1518);
    if ((config_threshold < 1518) ||
        (config_threshold > 0x3fff) ) {
        config_threshold = 1518;
    }

    BCM_IF_ERROR_RETURN
        (_bcm_esw_stat_ovr_threshold_set(unit, port, config_threshold));

    /* Clear counters */
    SOC_PBMP_CLEAR(pbmp);
    SOC_PBMP_PORT_ADD(pbmp, port);
    BCM_IF_ERROR_RETURN(soc_counter_set32_by_port(unit, pbmp, 0));

    /* Note: Assuming soc_counter_start would be called later after
             all the modules for the new port are initialised */

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_attach_stack
 * Purpose:
 *      Initializes stack on port
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_attach_stack(int unit, bcm_port_t port)
{

    /* Set HG ingress CPU Opcode map to the CPU */
    if (IS_ST_PORT(unit, port)) {
        pbmp_t      pbmp;
        icontrol_opcode_bitmap_entry_t entry;

        SOC_PBMP_CLEAR(pbmp);
        SOC_PBMP_PORT_SET(pbmp, CMIC_PORT(unit));

        BCM_IF_ERROR_RETURN
            (READ_ICONTROL_OPCODE_BITMAPm(unit, MEM_BLOCK_ANY, port,
                                          &entry));
        soc_mem_pbmp_field_set(unit, ICONTROL_OPCODE_BITMAPm, &entry,
                               BITMAPf, &pbmp);
        BCM_IF_ERROR_RETURN
            (WRITE_ICONTROL_OPCODE_BITMAPm(unit, MEM_BLOCK_ANY, port,
                                           &entry));
    }

    /* Initialize modport map profiles ptr on port.*/
    if (soc_feature(unit, soc_feature_modport_map_dest_is_port_or_trunk)) {
        BCM_IF_ERROR_RETURN
            (bcm_td_stk_modport_map_port_attach(unit, port));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_attach_rate
 * Purpose:
 *      Initialize port related information in the Rate module.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_attach_rate(int unit, bcm_port_t port)
{
    soc_field_t fields[] = {PACKET_QUANTUMf, BYTE_MODEf};
    uint32 values[] = {0x100, 1};

#ifdef BCM_FIREBOLT6_SUPPORT
    if (soc_feature(unit, soc_feature_storm_control_meter_is_mem)) {


        SOC_IF_ERROR_RETURN(soc_mem_write(unit,
           STORM_CONTROL_METER_CONFIGm, MEM_BLOCK_ALL, port,
           soc_mem_entry_null(unit, STORM_CONTROL_METER_CONFIGm)));


        SOC_IF_ERROR_RETURN(soc_mem_fields32_modify(unit,
            STORM_CONTROL_METER_CONFIGm, port, COUNTOF(fields),
            fields, values));
    } else
#endif
    {

        SOC_IF_ERROR_RETURN
            (WRITE_STORM_CONTROL_METER_CONFIGr(unit, port, 0));

        SOC_IF_ERROR_RETURN
            (soc_reg_fields32_modify(unit,
                 STORM_CONTROL_METER_CONFIGr,
                 port, COUNTOF(fields),
                 fields, values));
    }
#if 0
    BCM_IF_ERROR_RETURN
        (WRITE_STORM_CONTROL_METER_CONFIGr(unit, port, 0));

    BCM_IF_ERROR_RETURN
        (soc_reg_fields32_modify(unit,
                                 STORM_CONTROL_METER_CONFIGr,
                                 port, COUNTOF(fields),
                                 fields, values));
#endif
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_attach_field
 * Purpose:
 *      Enable filter lookup, ingress filter, egress filter on port
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_attach_field(int unit, bcm_port_t port)
{
#ifdef BCM_FIELD_SUPPORT
    /* FIELD */
    if (soc_feature(unit, soc_feature_field)) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_control_set(unit, port,
                                      bcmPortControlFilterLookup, 1));
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_control_set(unit, port,
                                      bcmPortControlFilterIngress, 1));
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_control_set(unit, port,
                                      bcmPortControlFilterEgress, 1));
    }
    BCM_IF_ERROR_RETURN
        (_bcm_field_flex_port_attach(unit, port));
#endif /* BCM_FIELD_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_attach_mirror
 * Purpose:
 *      Initialize the mirror per-port configuration in the runtime
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_attach_mirror(int unit, bcm_port_t port)
{
    /* MIRROR */
    BCM_IF_ERROR_RETURN
        (bcmi_esw_mirror_port_attach(unit, port));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_attach_l3
 * Purpose:
 *      Enable IPv4, IPv6 on port
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_attach_l3(int unit, bcm_port_t port)
{
#ifdef INCLUDE_L3
    /* L3 */
    if (soc_feature(unit, soc_feature_l3) &&
        soc_property_get(unit, spn_L3_ENABLE, 1) &&
        !IS_ST_PORT(unit, port)) {

        BCM_IF_ERROR_RETURN
            (bcm_esw_port_control_set(unit, port, bcmPortControlIP4, 1));
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_control_set(unit, port, bcmPortControlIP6, 1));
    }
#endif /* INCLUDE_L3 */
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_attach_ipmc
 * Purpose:
 *      Initialize IPMC per-port configuration
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_attach_ipmc(int unit, bcm_port_t port)
{
#ifdef INCLUDE_L3
    soc_field_t fields_lport[3] = {V4IPMC_ENABLEf, V6IPMC_ENABLEf, IPMC_DO_VLANf};
    uint32      field_values[3] = {1, 1, 1};
    /* IPMC */
    if (soc_feature(unit, soc_feature_ip_mcast)) {

        int do_vlan = soc_property_get(unit, spn_IPMC_DO_VLAN, 1);

        field_values[2] = (do_vlan) ? 1 : 0;

        BCM_IF_ERROR_RETURN
            (_bcm_esw_port_config_set(unit, port, _bcmPortIpmcV4Enable, 1));
        BCM_IF_ERROR_RETURN
            (_bcm_esw_port_config_set(unit, port, _bcmPortIpmcV6Enable, 1));
        BCM_IF_ERROR_RETURN
            (_bcm_esw_port_config_set(unit, port, _bcmPortIpmcVlanKey,
                                      do_vlan ? 1 : 0));
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_lport_fields_set(unit, port, LPORT_PROFILE_LPORT_TAB, 3, 
                                          fields_lport, field_values)); 

        if (soc_feature(unit, soc_feature_no_egr_ipmc_cfg)) {
            return BCM_E_NONE;
        }
        if (IS_E_PORT(unit,port)) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_ipmc_egress_port_set(unit, port, _soc_mac_all_zeroes,
                                              0, 0, 0));
        }
    }
#endif /* INCLUDE_L3 */
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_attach_mpls
 * Purpose:
 *      Enable MPLS on port.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_attach_mpls(int unit, bcm_port_t port)
{
    int rv = BCM_E_NONE;

#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
    /* MPLS */
    if (soc_feature(unit, soc_feature_mpls) &&
        !IS_ST_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_control_set(unit, port, bcmPortControlMpls, 1));
    }
#endif /* L3 && BCM_MPLS_SUPPORT */
    return rv;
}

/*
 * Function:
 *      bcmi_xgs5_port_attach_mim
 * Purpose:
 *      Enable MAC-in-MAC frame processing on port.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_attach_mim(int unit, bcm_port_t port)
{
#ifdef INCLUDE_L3
    /* MIM */
    if (soc_feature(unit, soc_feature_mim) &&
        !IS_ST_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_control_set(unit, port, bcmPortControlMacInMac, 1));
    }
#endif /* INCLUDE_L3 */
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_detach_dscp
 * Purpose:
 *      Detach port from default DSCP profile
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_detach_dscp(int unit, bcm_port_t port)
{
    int value;
    uint32 dscp_index;

    BCM_IF_ERROR_RETURN
        (_bcm_esw_port_tab_get(unit, port, TRUST_DSCP_PTRf, &value));
    dscp_index = value * 64;
    if (0 == dscp_index) {
        BCM_IF_ERROR_RETURN
            (_bcm_dscp_table_entry_delete(unit, dscp_index));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_detach_port_info_init
 * Purpose:
 *      Initialize BCM PORT info structure (bcm_port_info[unit][port])
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_detach_software(int unit, bcm_port_t port)
{
    _bcm_port_info_t *pinfo;
    int index;
    int enabled;
    uint32 value;

    /*
     * Update TPID reference count
     */
    if (soc_feature(unit, soc_feature_vlan_ctrl)) {
        if (soc_feature(unit, soc_feature_egr_vlan_control_is_memory)) {
            if (soc_feature(unit, soc_feature_egr_all_profile)) {
                BCM_IF_ERROR_RETURN(
                    bcm_esw_port_egr_lport_field_get(
                    unit, port, EGR_VLAN_CONTROL_1m,
                    OUTER_TPID_INDEXf, (uint32 *)&index));
            } else {
                BCM_IF_ERROR_RETURN(
                    READ_EGR_VLAN_CONTROL_1m(unit, MEM_BLOCK_ANY, port, &value));
                index = soc_mem_field32_get(unit, EGR_VLAN_CONTROL_1m,
                                            &value, OUTER_TPID_INDEXf);
            }
        } else {
            BCM_IF_ERROR_RETURN
                (READ_EGR_VLAN_CONTROL_1r(unit, port, &value));
            index = soc_reg_field_get(unit, EGR_VLAN_CONTROL_1r,
                                  value, OUTER_TPID_INDEXf);
        }

        /* Ignore error when deleting */
        (void)_bcm_fb2_outer_tpid_entry_delete(unit, index);

        enabled = 0;
        BCM_IF_ERROR_RETURN
            (_bcm_esw_port_config_get(unit, port,
                                      _bcmPortOuterTpidEnables, &enabled));
        index = 0;
        while (enabled) {
            if (enabled & 1) {
                /* Ignore error when deleting */
                (void)_bcm_fb2_outer_tpid_entry_delete(unit, index);
            }
            enabled = enabled >> 1;
            index++;
        }
    }

    /* Clear the Port INFO */
    BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));
    if  (pinfo == NULL) {
        return BCM_E_INIT;
    }

    if (pinfo->p_vd_pbvl != NULL) {
        sal_free(pinfo->p_vd_pbvl);
        pinfo->p_vd_pbvl = NULL;
    }
    if (pinfo->e2ecc_config != NULL) {
        sal_free(pinfo->e2ecc_config);
        pinfo->e2ecc_config = NULL;
    }

    sal_memset(pinfo, 0, sizeof(_bcm_port_info_t));


    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_detach_egr_vlan_action
 * Purpose:
 *      Set the egress port's default vlan tag actions
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_detach_egr_vlan_action(int unit, bcm_port_t port)
{
    if (soc_feature(unit, soc_feature_vlan_action)) {
        BCM_IF_ERROR_RETURN
            (_bcm_trx_vlan_port_egress_default_action_delete(unit, port));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_detach_phy
 * Purpose:
 *      Probe the PHY and set up the PHY and MAC for the specified ports.
 *      This is purely a discovery routine and does no configuration.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_detach_phy(int unit, bcm_port_t port)
{
    int rv;
    soc_persist_t *sop = SOC_PERSIST(unit);

    /* If PortMod enable, detach phy is handled in _bcmi_xgs5_port_pm_delete */
    if (SOC_PORT_USE_PORTCTRL(unit, port)) {
        return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(soc_phyctrl_detach(unit, port));

    PORT_LOCK(unit);
    rv = _bcm_port_mode_setup(unit, port, FALSE);
    PORT_UNLOCK(unit);
    BCM_IF_ERROR_RETURN(rv);

    if (SOC_PBMP_MEMBER(sop->lc_pbm_fc, port)) {
        /* Bring down the internal PHY */
        BCM_IF_ERROR_RETURN(bcm_esw_port_update(unit, port, FALSE));
        SOC_PBMP_PORT_REMOVE(sop->lc_pbm_fc, port);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_detach_vlan_protocol
 * Purpose:
 *      Clear the VLAN_PROTOCOL_DATA_INDEX, FP_PORT_FIELD_SEL_INDEX,
 *      PROTOCOL_PKT_INDEX pointers
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_detach_vlan_protocol(int unit, bcm_port_t port)
{
    _bcm_port_info_t *pinfo;
    int value = 0;
    int i, idx, vlan_prot_entries;
    vlan_protocol_data_entry_t proto_entry;
    int rv;

    /* Get PORT info structure pointer */
    BCM_IF_ERROR_RETURN(_bcm_port_info_get(unit, port, &pinfo));
    if  (pinfo == NULL) {
        return BCM_E_INTERNAL;
    }

    if (!IS_LB_PORT(unit, port) && !IS_TDM_PORT(unit, port)) {

        if (SOC_SWITCH_BYPASS_MODE(unit) == SOC_SWITCH_BYPASS_MODE_NONE) {
            if (soc_mem_field_valid(unit, PORT_TABm,
                        VLAN_PROTOCOL_DATA_INDEXf)) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_port_tab_set(unit, port,
                               _BCM_CPU_TABS_ETHER,
                               VLAN_PROTOCOL_DATA_INDEXf,
                               0));
        }
        }

        if (soc_mem_field_valid(unit, PORT_TABm,
            FP_PORT_FIELD_SEL_INDEXf)) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_port_tab_set(unit, port,
                           _BCM_CPU_TABS_ETHER,
                            FP_PORT_FIELD_SEL_INDEXf, 0));
        }
        if (soc_mem_field_valid(unit, PORT_TABm,
                                PROTOCOL_PKT_INDEXf)) {
            if (SOC_REG_INFO(unit, PROTOCOL_PKT_CONTROLr).regtype ==
                soc_portreg) {
                    BCM_IF_ERROR_RETURN(
                        _bcm_esw_port_tab_set(unit, port,
                                           _BCM_CPU_TABS_ETHER,
                                           PROTOCOL_PKT_INDEXf,
                                           0));
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_port_tab_get(unit, port,
                                           PROTOCOL_PKT_INDEXf,
                                           &value));
                BCM_IF_ERROR_RETURN
                    (_bcm_prot_pkt_ctrl_delete(unit, value));
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_port_tab_set(unit, port,
                                           _BCM_CPU_TABS_ETHER,
                                           PROTOCOL_PKT_INDEXf,
                                           0));
            }
        }

        if (SOC_IS_TOMAHAWK2(unit)) {
            vlan_prot_entries = soc_mem_index_count(unit, VLAN_PROTOCOLm);

            for (i = 0; i < vlan_prot_entries; i++) {
                rv = READ_VLAN_PROTOCOL_DATAm(unit, MEM_BLOCK_ANY,
                         pinfo->vlan_prot_ptr + i,
                         &proto_entry);
                if (BCM_FAILURE(rv)) {
                    return (rv);
                }
                idx = soc_VLAN_PROTOCOL_DATAm_field32_get(unit,
                          &proto_entry, TAG_ACTION_PROFILE_PTRf);
                SOC_IF_ERROR_RETURN
                    (_bcm_trx_vlan_action_profile_entry_delete(unit, idx));
            }
        }

    }
    BCM_IF_ERROR_RETURN(
        _bcm_port_vlan_protocol_data_entry_delete(unit, pinfo->vlan_prot_ptr));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_detach_eee
 * Purpose:
 *      EEE config
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_detach_eee(int unit, bcm_port_t port)
{
    if (!IS_LB_PORT(unit, port) && !IS_CPU_PORT (unit, port) &&
        !IS_TDM_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN
            (bcmi_esw_port_eee_cfg_set(unit, port, 0));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_detach_higig
 * Purpose:
 *      Remove port from hg2_pbm
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_detach_higig(int unit, bcm_port_t port)
{
    /**********
     * HIGIG
     */
    if (IS_HG_PORT(unit, port)) {
        if (SOC_PORT_USE_PORTCTRL(unit, port)) {
            BCM_IF_ERROR_RETURN
                (bcmi_esw_portctrl_higig_mode_set(unit, port, FALSE));
        }
#ifdef BCM_HIGIG2_SUPPORT
        /* soc_feature_higig2 */
        if (soc_feature(unit, soc_feature_higig2)) {
            if (soc_property_port_get(unit, port, spn_HIGIG2_HDR_MODE,
                    soc_feature(unit, soc_feature_no_higig_plus) ? 1 : 0) ||
                SOC_PORT_USE_PORTCTRL(unit, port)) {
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_port_higig2_mode_set(unit, port, FALSE));
                SOC_HG2_ENABLED_PORT_REMOVE(unit, port);
            }
        }
#endif /* BCM_HIGIG2_SUPPORT */
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_detach_egr_link_delay
 * Purpose:
 *      Clear port link delay
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_detach_egr_link_delay(int unit, bcm_port_t port)
{
    if (SOC_REG_PORT_VALID(unit, EGR_1588_LINK_DELAYr, port)) {
        BCM_IF_ERROR_RETURN
            (soc_reg_field32_modify(unit, EGR_1588_LINK_DELAYr, port,
                                    LINK_DELAYf, 0));
    } else if (SOC_REG_PORT_VALID(unit, EGR_1588_LINK_DELAY_64r, port)) {
        BCM_IF_ERROR_RETURN
            (soc_reg_above_64_field32_modify(unit, EGR_1588_LINK_DELAY_64r,
                                             port, 0, LINK_DELAYf, 0));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_detach_stg
 * Purpose:
 *      Set STP state of port to DISABLE
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_detach_stg(int unit, bcm_port_t port)
{
    bcm_vlan_t d_vid;

    BCM_IF_ERROR_RETURN(bcm_esw_vlan_default_get(unit, &d_vid));

    /* STG */
    BCM_IF_ERROR_RETURN
        (bcm_esw_vlan_stp_set(unit, d_vid, port, BCM_STG_STP_DISABLE));
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_detach_vlan
 * Purpose:
 *      Remove port from default vlan 1
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_detach_vlan(int unit, bcm_port_t port)
{
    bcm_pbmp_t pbmp;
    bcm_vlan_t d_vid;

    BCM_IF_ERROR_RETURN(bcm_esw_vlan_default_get(unit, &d_vid));

    /* VLAN */
    BCM_PBMP_CLEAR(pbmp);
    BCM_PBMP_PORT_ADD(pbmp, port);
    BCM_IF_ERROR_RETURN
        (bcm_esw_vlan_port_remove(unit, d_vid, pbmp));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_detach_trunk
 * Purpose:
 *      Update the trunk tables for a deleted port
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_detach_trunk(int unit, bcm_port_t port)
{
    bcm_pbmp_t pbmp;
    higig_trunk_control_entry_t entry;
    int rv = BCM_E_NONE;

    /* TRUNK */
    if (soc_feature(unit, soc_feature_hg_trunking)) {
        
        soc_mem_lock(unit, HIGIG_TRUNK_CONTROLm);

        rv = soc_mem_read(unit, HIGIG_TRUNK_CONTROLm, MEM_BLOCK_ANY, 0, &entry);
        if (BCM_FAILURE(rv)) {
            soc_mem_unlock(unit, HIGIG_TRUNK_CONTROLm);
            return rv;
        }

        BCM_PBMP_CLEAR(pbmp);

        /* Read port bmap from hardware, and remove new port to the bitmap */
        soc_mem_pbmp_field_get(unit, HIGIG_TRUNK_CONTROLm, &entry,
                               ACTIVE_PORT_BITMAPf, &pbmp);

        BCM_PBMP_PORT_REMOVE(pbmp, port);

        soc_mem_pbmp_field_set(unit, HIGIG_TRUNK_CONTROLm, &entry,
                           ACTIVE_PORT_BITMAPf, &pbmp);

        rv = soc_mem_write(unit, HIGIG_TRUNK_CONTROLm, MEM_BLOCK_ALL, 0,
                       &entry);

        soc_mem_unlock(unit, HIGIG_TRUNK_CONTROLm);
    }
    return rv;
}

/*
 * Function:
 *      bcmi_xgs5_port_detach_stack
 * Purpose:
 *      Clear port related information in the Stack module.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_detach_stack(int unit, bcm_port_t port)
{

    /* Clear HG ingress CPU Opcode bitmap */
    if (IS_ST_PORT(unit, port)) {
        pbmp_t      pbmp;
        icontrol_opcode_bitmap_entry_t entry;

        SOC_PBMP_CLEAR(pbmp);

        BCM_IF_ERROR_RETURN
            (READ_ICONTROL_OPCODE_BITMAPm(unit, MEM_BLOCK_ANY, port,
                                          &entry));
        soc_mem_pbmp_field_set(unit, ICONTROL_OPCODE_BITMAPm, &entry,
                               BITMAPf, &pbmp);
        BCM_IF_ERROR_RETURN
            (WRITE_ICONTROL_OPCODE_BITMAPm(unit, MEM_BLOCK_ANY, port,
                                           &entry));
    }

    /* detach port from modport map profiles */
    if (soc_feature(unit, soc_feature_modport_map_dest_is_port_or_trunk)) {
        BCM_IF_ERROR_RETURN
            (bcm_td_stk_modport_map_port_detach(unit, port));
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_detach_rate
 * Purpose:
 *      Clear port related information in the Rate module.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_detach_rate(int unit, bcm_port_t port)
{
#ifdef BCM_FIREBOLT6_SUPPORT
    storm_control_meter_config_entry_t entry;
    if (SOC_IS_FIREBOLT6(unit)) {
        sal_memset(&entry, 0, sizeof(storm_control_meter_config_entry_t));
        BCM_IF_ERROR_RETURN
            (soc_mem_write(unit, STORM_CONTROL_METER_CONFIGm,
            MEM_BLOCK_ALL, port, &entry));
    } else
#endif
    {
        BCM_IF_ERROR_RETURN
            (WRITE_STORM_CONTROL_METER_CONFIGr(unit, port, 0));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_detach_field
 * Purpose:
 *      Disaable filter lookup, ingress filter, egress filter on port
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_detach_field(int unit, bcm_port_t port)
{
#ifdef BCM_FIELD_SUPPORT
    /* FIELD */
    if (soc_feature(unit, soc_feature_field)) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_control_set(unit, port,
                                      bcmPortControlFilterLookup, 0));
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_control_set(unit, port,
                                      bcmPortControlFilterIngress, 0));
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_control_set(unit, port,
                                      bcmPortControlFilterEgress, 0));
    }
    BCM_IF_ERROR_RETURN
        (_bcm_field_flex_port_detach(unit, port));
#endif /* BCM_FIELD_SUPPORT */

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_detach_mirror
 * Purpose:
 *      Clear the mirror per-port configuration in the runtime
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_detach_mirror(int unit, bcm_port_t port)
{
    /* MIRROR */
    BCM_IF_ERROR_RETURN
        (bcmi_esw_mirror_port_detach(unit, port));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_detach_l3
 * Purpose:
 *      Disable IPv4, IPv6 on port
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_detach_l3(int unit, bcm_port_t port)
{
#ifdef INCLUDE_L3
    /* L3 */
    if (soc_feature(unit, soc_feature_l3) &&
        soc_property_get(unit, spn_L3_ENABLE, 1) &&
        !IS_ST_PORT(unit, port)) {

        BCM_IF_ERROR_RETURN
            (bcm_esw_port_control_set(unit, port, bcmPortControlIP4, 0));
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_control_set(unit, port, bcmPortControlIP6, 0));
    }
#endif /* INCLUDE_L3 */
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_detach_ipmc
 * Purpose:
 *      Clear IPMC per-port configuration
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_detach_ipmc(int unit, bcm_port_t port)
{
#ifdef INCLUDE_L3
    soc_field_t fields_lport[3] = {V4IPMC_ENABLEf, V6IPMC_ENABLEf, IPMC_DO_VLANf};
    uint32      field_values[3] = {0, 0, 0};
    /* IPMC */
    if (soc_feature(unit, soc_feature_ip_mcast)) {

        BCM_IF_ERROR_RETURN
            (_bcm_esw_port_config_set(unit, port, _bcmPortIpmcV4Enable, 0));
        BCM_IF_ERROR_RETURN
            (_bcm_esw_port_config_set(unit, port, _bcmPortIpmcV6Enable, 0));
        BCM_IF_ERROR_RETURN
            (_bcm_esw_port_config_set(unit, port, _bcmPortIpmcVlanKey, 0));

        BCM_IF_ERROR_RETURN(
            bcm_esw_port_lport_fields_set(unit, port, LPORT_PROFILE_LPORT_TAB, 3, 
                                          fields_lport, field_values));
        if (soc_feature(unit, soc_feature_no_egr_ipmc_cfg)) {
            return BCM_E_NONE;
        }
        if (IS_E_PORT(unit,port)) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_ipmc_egress_port_set(unit, port, _soc_mac_all_zeroes,
                                              0, 0, 0));
        }
    }
#endif /* INCLUDE_L3 */
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_detach_mpls
 * Purpose:
 *      Disable MPLS on port.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_detach_mpls(int unit, bcm_port_t port)
{
    int rv = BCM_E_NONE;

#if defined(INCLUDE_L3) && defined(BCM_MPLS_SUPPORT)
    /* MPLS */
    if (soc_feature(unit, soc_feature_mpls) &&
        !IS_ST_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_control_set(unit, port, bcmPortControlMpls, 0));
    }
#endif /* L3 && BCM_MPLS_SUPPORT */
    return rv;
}

/*
 * Function:
 *      bcmi_xgs5_port_detach_mim
 * Purpose:
 *      Disable MAC-in-MAC frame processing on port.
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical port.
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_detach_mim(int unit, bcm_port_t port)
{
#ifdef INCLUDE_L3
    /* MIM */
    if (soc_feature(unit, soc_feature_mim) &&
        !IS_ST_PORT(unit, port)) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_control_set(unit, port, bcmPortControlMacInMac, 0));
    }
#endif /* INCLUDE_L3 */
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_sched_structure
 * Purpose:
 *      Populate port_schedule_state
 * Parameters:
 *      unit  - (IN) Unit number.
 *      nport - (IN) Number of elements in array port
 *      port  - (IN) Array of logical port.
 *      post  - (IN) Indicate populate out_port_map or not
 *                   1: set out_port_map with post flex info;
 *                   0: NOT set out_port_map
 *      port_schedule_state - (OUT) port_schedule_state
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
bcmi_xgs5_port_sched_structure(int unit, int nport, bcm_port_t *port, int post,
                               soc_port_schedule_state_t *port_schedule_state)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *soc_resource;
    int logic_port, phy_port, encap;
    int i, rv = BCM_E_NONE;

    soc_resource = sal_alloc(sizeof(soc_port_resource_t) * nport,
                             "soc_port_resource");
    if (soc_resource == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(soc_resource, 0, sizeof(soc_port_resource_t) * nport);

    for (i = 0; i< nport; i++) {
        logic_port = port[i];
        phy_port = si->port_l2p_mapping[logic_port];

        rv = bcm_esw_port_encap_get(unit, logic_port, &encap);
        if (BCM_FAILURE(rv)) {
            break;
        }
        soc_resource[i].logical_port = logic_port;
        soc_resource[i].physical_port = phy_port;
        soc_resource[i].mmu_port = si->port_p2m_mapping[phy_port];
        soc_resource[i].idb_port = si->port_l2i_mapping[logic_port];
        soc_resource[i].pipe = si->port_pipe[logic_port];
        soc_resource[i].speed = si->port_init_speed[logic_port];
        soc_resource[i].encap = encap;
        soc_resource[i].num_lanes = si->port_num_lanes[logic_port];
        soc_resource[i].oversub =
                        SOC_PBMP_MEMBER(si->oversub_pbm, logic_port) ? 1 : 0;
        soc_resource[i].hsp =
                        SOC_PBMP_MEMBER(si->eq_pbm, logic_port) ? 1 : 0;
    }

    if (BCM_SUCCESS(rv)) {
        rv = _bcmi_xgs5_port_soc_schedule_state_init(unit, nport,
                                            BCMI_XGS5_PORT_RESOURCE_OP_ENABLE,
                                            soc_resource,
                                            port_schedule_state);
    }

    if (BCM_SUCCESS(rv) && post) {
        rv = _bcmi_xgs5_port_info_post_flex(unit, port_schedule_state);
    }

    sal_free(soc_resource);
    return rv;
}

/*
 * Function:
 *      bcmi_xgs5_port_enable_set
 * Purpose:
 *      Enable or disable a port.
 *
 *      To reuse the port up/down routines from DV for port enable/disable 
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical BCM Port number (non-GPORT).
 *      enable  - (IN) Indicates whether to enable or disable port.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Must be called with PORT_LOCK held.
 *      Must be called with PORTMOD enabled.
 */
int
bcmi_xgs5_port_enable_set(int unit, bcm_port_t port, int enable)
{
    soc_port_schedule_state_t *port_schedule_state;
    int rv;

    /* Allocate memory for soc_port_schedule_state_t structure */
    port_schedule_state = sal_alloc(sizeof(soc_port_schedule_state_t),
                                    "port schedule state");
    if (port_schedule_state == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(port_schedule_state, 0, sizeof(soc_port_schedule_state_t));

    rv = bcmi_xgs5_port_sched_structure(unit, 1, &port, 1, port_schedule_state);
    if (BCM_FAILURE(rv)) {
        sal_free(port_schedule_state);
        return rv;
    }

    if (enable && BCMI_PORT_DRV_CALL(unit)->port_enable) {
        /* Port UP */
        rv = BCMI_PORT_DRV_CALL(unit)->port_enable(unit, port_schedule_state);
    } else if ((!enable) && BCMI_PORT_DRV_CALL(unit)->port_disable) {
        /* Physical_port = -1 indicates bringing down a port */
        port_schedule_state->resource[0].physical_port = -1;
        /* Port Down */
        rv = BCMI_PORT_DRV_CALL(unit)->port_disable(unit, port_schedule_state);
    } else {
        rv = BCM_E_UNAVAIL;
    }

    sal_free(port_schedule_state);
    return rv;
}

#if defined(INCLUDE_XFLOW_MACSEC) && defined(BCM_HURRICANE4_SUPPORT)
/*
 * Function:
 *      _bcmi_xflow_macsec_hr4_port_enable_set
 * Purpose:
 *      Enable or disable a macsec port.
 *
 *      To reuse the port up/down routines from DV for port enable/disable
 * Parameters:
 *      unit    - (IN) Unit number.
 *      port    - (IN) Logical BCM Port number (non-GPORT).
 *      enable  - (IN) Indicates whether to enable or disable port.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *      Must be called with PORTMOD enabled.
 */
int
_bcmi_xflow_macsec_hr4_port_enable_set(int unit, int nport,
                                       bcm_port_t *port_array, int enable)
{
    int i, rv = BCM_E_NONE;
    int port, phy_port;
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *soc_resource;
    soc_port_schedule_state_t *port_schedule_state;


    if (!SOC_IS_HURRICANE4(unit)) {
        return BCM_E_NONE;
    }

    /* Allocate memory for soc_port_schedule_state_t structure */
    port_schedule_state = sal_alloc(sizeof(soc_port_schedule_state_t),
                                    "port schedule state");
    if (port_schedule_state == NULL) {
        return BCM_E_MEMORY;
    }
    sal_memset(port_schedule_state, 0, sizeof(soc_port_schedule_state_t));

    /* Allocate memory for soc_port_resource_t structure */
    soc_resource = sal_alloc(sizeof(soc_port_resource_t) * nport,
                             "soc_port_resource");
    if (soc_resource == NULL) {
        sal_free(port_schedule_state);
        return BCM_E_MEMORY;
    }
    sal_memset(soc_resource, 0, sizeof(soc_port_resource_t) * nport);

    for (i = 0; i < nport; i++) {
        port = port_array[i];
        phy_port = si->port_l2p_mapping[port];

        soc_resource[i].logical_port = port;
        if (enable) {
            soc_resource[i].physical_port = phy_port;
        } else {
            soc_resource[i].physical_port = -1;
        }

        soc_resource[i].mmu_port = si->port_p2m_mapping[phy_port];
        soc_resource[i].idb_port = si->port_l2i_mapping[port];
        soc_resource[i].pipe = 0;
        soc_resource[i].speed = si->port_speed_max[port];
        soc_resource[i].num_lanes = si->port_num_lanes[port];
        soc_resource[i].oversub = IS_OVERSUB_PORT(unit, port) ? 1 : 0;
        soc_resource[i].hsp = SOC_PBMP_MEMBER(si->eq_pbm, port) ? 1 : 0;
    }

    rv = _bcmi_xgs5_port_soc_schedule_state_init(unit, nport,
                                                 BCMI_XGS5_PORT_RESOURCE_OP_ENABLE,
                                                 soc_resource,
                                                 port_schedule_state);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    rv = _bcmi_xgs5_port_info_post_flex(unit, port_schedule_state);
    if (BCM_FAILURE(rv)) {
        goto cleanup;
    }

    if (enable && BCMI_PORT_DRV_CALL(unit)->port_enable) {
        /* Port UP */
        rv = BCMI_PORT_DRV_CALL(unit)->port_enable(unit, port_schedule_state);
    } else if ((!enable) && BCMI_PORT_DRV_CALL(unit)->port_disable) {
        /* Port Down */
        rv = BCMI_PORT_DRV_CALL(unit)->port_disable(unit, port_schedule_state);
    } else {
        rv = BCM_E_UNAVAIL;
    }

cleanup:
    sal_free(soc_resource);
    sal_free(port_schedule_state);

    return rv;
}
#endif /* INCLUDE_XFLOW_MACSEC  && BCM_HURRICANE4_SUPPORT */

/*
 * Function:
 *      bcmi_xgs5_flexport_based_speed_ability_get
 * Purpose:
 *      Retrieve the local port abilities.
 * Parameters:
 *      unit - Unit.
 *      port - Logical port number.
 *      mask - (OUT) Mask of BCM_PORT_ABILITY_ values indicating the
 *              supported speeds of the port.
 * Returns:
 *      BCM_E_NONE
 *      BCM_E_XXX
 */
int
bcmi_xgs5_flexport_based_speed_ability_get(int unit, bcm_port_t port,
                                           bcm_port_abil_t *mask)
{
    if (BCMI_PORT_DRV_CALL(unit)->speed_ability_get) {
        BCMI_PORT_DRV_CALL(unit)->speed_ability_get(unit, port, mask);
    } else {
        *mask = SOC_PA_ABILITY_ALL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_encap_speed_check
 * Purpose:
 *      Restriction: Higig speed with IEEE encap mode is not supported.
 * Parameters:
 *      unit     - (IN) Unit number
 *      port     - (IN) Logical port number
 *      encap    - (IN) encap mode
 *      speed    - (IN) speed
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_xgs5_port_encap_speed_check(int unit, bcm_port_t port, int encap,
                                 int speed)
{
    int is_hg_speed;
    uint32 speed_mask;

    if (speed == 0) {
        BCM_IF_ERROR_RETURN
            (bcm_esw_port_speed_get(unit, port, &speed));
    }

    speed_mask = SOC_PA_SPEED(speed);
    is_hg_speed = SOC_PA_IS_HG_SPEED(speed_mask);

    /* Expect non-higig speed for IEEE encapsulation mode. */
    if (is_hg_speed && encap == BCM_PORT_ENCAP_IEEE) {
        LOG_ERROR(BSL_LS_BCM_PORT,
                  (BSL_META_U(unit,
                              "Speed and encapsulation configuration unmatch "
                              "for port=%d, speed=%d, encap=%d\n"),
                   port, speed, encap));
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_redirect_config_set
 * Purpose:
 *      Configure the redirection data
 * Parameters:
 *      unit             - (IN) device id.
 *      port             - (IN) Port number.
 *      redirect_config  - (IN) Pointer to the redirection config structure
 * Returns:
 *      None
 */
int _bcmi_xgs5_port_redirect_config_set(int unit, bcm_port_t port,
                          bcm_port_redirect_config_t *redirect_config)
{
    bcm_trunk_t trunk_id = -1;
    bcm_module_t mod_id = -1;
    bcm_port_t port_id = -1;
    int id = -1, redirect_set = 1, color = -1;
    int curr_dest_type = 0;
    int dest_type = _BCM_REDIRECT_DEST_TYPE_NONE, dest_value = -1;
    int dest_type_flags = 0;
    egr_port_entry_t egr_port_entry;
#if defined(BCM_EP_REDIRECT_VERSION_2)
    int redirect_action_value = 0, redirect_source_value = 0;
    int buffer_priority = 0;
#endif

    /* If the dest_type_field is invalid, this feature is not
       supported on this device */
    if(PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->dest_type == INVALIDf) {
        return BCM_E_UNAVAIL;
    }

    dest_type_flags = (redirect_config->flags &
                           (BCM_PORT_REDIRECT_DESTINATION_PORT |
                            BCM_PORT_REDIRECT_DESTINATION_TRUNK |
                            BCM_PORT_REDIRECT_DESTINATION_MCAST));

    /* Make sure only one destination type is set */
    if(dest_type_flags & (dest_type_flags - 1)) {
        return BCM_E_PARAM;
    }

    /* Switch based on the destination type */
    switch(dest_type_flags) {

        case BCM_PORT_REDIRECT_DESTINATION_PORT:

            /* Make sure that the supplied destination is valid */
            if(!BCM_GPORT_IS_MODPORT(redirect_config->destination)) {
                return BCM_E_PORT;
            }

            BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit,
                                         redirect_config->destination,
                                         &mod_id, &port_id, &trunk_id, &id));

            /* Set the destination type as GPP(Port) */
            dest_type = _BCM_REDIRECT_DEST_TYPE_PORT;

            /* Set the (Mod,Port) value as destination */
            BCMI_MOD_PORT_SET(dest_value, mod_id, port_id);

            break;

        case BCM_PORT_REDIRECT_DESTINATION_TRUNK:

            /* Make sure that the supplied destination is valid */
            if(!BCM_GPORT_IS_SET(redirect_config->destination) ||
               !BCM_GPORT_IS_TRUNK(redirect_config->destination)) {
                return BCM_E_PARAM;
            }

            /* Get the trunk id */
            trunk_id = BCM_GPORT_TRUNK_GET(redirect_config->destination);

            /* Check that the trunk id is valid */
            BCM_IF_ERROR_RETURN(_bcm_trunk_id_validate(unit, trunk_id));

            /* Set the destination type as GPP(Port) */
            dest_type = _BCM_REDIRECT_DEST_TYPE_TRUNK;

            /* Set the trunk id as the destiantion */
            dest_value = trunk_id;

            break;

        case BCM_PORT_REDIRECT_DESTINATION_MCAST:

            /* Check the destination and make suure it is a valid l2 MC */
            if (_BCM_MULTICAST_IS_SET(redirect_config->destination)) {
                if (!_BCM_MULTICAST_IS_L2(redirect_config->destination)) {
                    return (BCM_E_PARAM);
                }
            }

            /* Set the destination type as GPP(Port) */
            dest_type = _BCM_REDIRECT_DEST_TYPE_MCAST;

            /* Set the l2 mc index as the destiantion */
            dest_value = _BCM_MULTICAST_ID_GET(redirect_config->destination);

            break;

        default:

            /* Reset the redirection flag */
            redirect_set = 0;

            /* Reset the destination type */
            dest_type = _BCM_REDIRECT_DEST_TYPE_NONE;

            /* Reset the destination */
            dest_value = 0;

            break;

    } /* End switch */

    /* Resolve the gport we are setting the redirection properties on */
    BCM_IF_ERROR_RETURN(_bcm_esw_gport_resolve(unit, port,
                                               &mod_id, &port_id,
                                               &trunk_id, &id));

    /* Get the egr port entry */
    BCM_IF_ERROR_RETURN(soc_mem_read(unit,
             PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->mem, MEM_BLOCK_ANY,
             port_id, &egr_port_entry));

    /* Get the current dest_type */
    curr_dest_type = soc_EGR_PORTm_field32_get(unit, &egr_port_entry,
                    PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->dest_type);

    /* Change the dest-type and dest-value only for non-mtp destinations,
       this is because this gets setup in the 'mirror' module */
    if(curr_dest_type != _BCM_REDIRECT_DEST_TYPE_MTP) {
        /* Setup the destination type and destination values */
        soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                         PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->dest_type,
                         dest_type);

        soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                         PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->dest_value,
                         dest_value);
    }

    /* Based on the redirection being set/cleared, set the drop_original flag,
      when rediretion is set, we will drop the original packet */
    soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
            PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->drop_original,
            (redirect_set ? 1 : 0));

    /* Set the redirected packet color */
    if(soc_feature(unit, soc_feature_ep_redirect_v2)) {
        /* If the redirection is being added, set the user provided
           color value, if it being disabled force the color to 0,
           which is a hardware default */
        if (redirect_set) {
            /* Map the external color values to internal ones */
            switch(redirect_config->color) {
                case bcmPortRedirectColorGreen:
                    color = 0;
                    break;
                case bcmPortRedirectColorYellow:
                    color = 1;
                    break;
                case bcmPortRedirectColorRed:
                    color = 2;
                    break;
                default:
                    break;
            }

            /* Set the color, if a valid value is supplied */
            if (color != -1) {
                soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                        PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_color, color);
                soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                        PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_change_color,
                        1);
            } else {
                soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                        PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_change_color,
                        0);
            }
        } else {
            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                             PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_color,
                             0);
            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                    PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_change_color,
                    0);
        }
    } else {
        /* For the older scheme, the color from enum maps directly */
       if (redirect_set) {
            if(redirect_config->color < bcmPortRedirectColorNone) {
                    soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                             PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_color,
                             redirect_config->color);
            }
       } else {
           soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                    PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_color,
                    0);
       }
    }

    /* Set the redirected packet priority, if redirection is being disabled, set
       to hardware defaults */
    if (redirect_set) {
        if(redirect_config->priority >= 0 && redirect_config->priority <= 15) {
            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                             PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_priority,
                             redirect_config->priority);
            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
              PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_change_priority, 1);
        } else if (redirect_config->priority == -1){
            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
              PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_change_priority, 0);
        } else {
            return BCM_E_PARAM;
        }
    } else {
        soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                         PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_priority,
                         0);
        soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_change_priority, 0);
    }

#if defined(BCM_EP_REDIRECT_VERSION_2)
    /* For the newer feature, set the additionally supported fields */
    if(soc_feature(unit, soc_feature_ep_redirect_v2)) {

        /* Set the trigger strength, if disabling redirection set it to
           default */
        if (redirect_set) {
            if((redirect_config->strength >= 0) &&
               (redirect_config->strength < BCMI_MAX_STRENGTH_VALUE)) {
                soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                       PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->strength,
                       redirect_config->strength);
            } else {
                return BCM_E_NONE;
            }
        } else {
            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                   PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->strength,
                   0);
        }

        /* Set the buffer priority, if disabling redirection set it to
           default */
        if (redirect_set) {
            if(redirect_config->buffer_priority &
               BCM_PORT_REDIRECT_BUFFER_PRIORITY_LOW) {
                buffer_priority = _BCM_PORT_REDIRECT_BUFFER_PRIORITY_LOW;
            } else if(redirect_config->buffer_priority &
               BCM_PORT_REDIRECT_BUFFER_PRIORITY_MEDIUM) {
                buffer_priority = _BCM_PORT_REDIRECT_BUFFER_PRIORITY_MEDIUM;
            } else if(redirect_config->buffer_priority &
               BCM_PORT_REDIRECT_BUFFER_PRIORITY_HIGH) {
                buffer_priority = _BCM_PORT_REDIRECT_BUFFER_PRIORITY_HIGH;
            }

            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->buffer_priority,
                buffer_priority);
        } else {
            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->buffer_priority,
                0);
        }

        /* Set the redirected packet action, based on the flag. If the
           redirection is being disabled, set it to default */
        if (redirect_set) {
            if(redirect_config->flags &
               BCM_PORT_REDIRECT_DROPPED_PACKET) {
                redirect_action_value = _BCM_REDIRECT_IF_DROPPED;
            } else if(redirect_config->flags &
                      BCM_PORT_REDIRECT_NOT_DROPPED_PACKET) {
                redirect_action_value = _BCM_REDIRECT_IF_NOT_DROPPED;
            } else {
                /* Default to always redirect */
                redirect_action_value = _BCM_REDIRECT_ALWAYS;
            }

            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                    PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->action,
                    redirect_action_value);
        } else {
            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                    PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->action,
                    0);
        }

        /* Set the redirected packet source to use, based on the flags.
           If the redirection is being disabled, set it to default */
        if (redirect_set) {
            if(redirect_config->flags &
               BCM_PORT_REDIRECT_SOURCE_USE_ORIGINAL_DESTINATION) {
                redirect_source_value =
                    _BCM_REDIRECT_SOURCE_USE_ORIGINAL_DESTINATION;
            } else if(redirect_config->flags &
                      _BCM_REDIRECT_SOURCE_USE_ORIGINAL_SOURCE) {
                redirect_source_value =
                    _BCM_REDIRECT_SOURCE_USE_ORIGINAL_SOURCE;
            } else {
                /* Default it to original source */
                redirect_source_value =
                    _BCM_REDIRECT_SOURCE_USE_ORIGINAL_SOURCE;
            }

            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->redir_pkt_source,
                redirect_source_value);
        } else {
            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->redir_pkt_source,
                0);
        }

        /* Set the truncate option, if user has chosen the same. If the
           redirection is being disabled, set it to default */
        if (redirect_set) {
            if(redirect_config->flags & BCM_PORT_REDIRECT_TRUNCATE) {
                soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                    PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->redir_pkt_truncate,
                    1);
            }
        } else {
            soc_EGR_PORTm_field32_set(unit, &egr_port_entry,
                PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->redir_pkt_truncate,
                0);
        }
    } /* soc_feature_ep_redirect_v2 */
#endif /* BCM_EP_REDIRECT_VERSION_2 */

    /* Write the entry to hardware */
    BCM_IF_ERROR_RETURN(soc_mem_write(unit, EGR_PORTm,
                                      MEM_BLOCK_ALL, port_id,
                                      &egr_port_entry));
    return BCM_E_NONE;
}


/*
 * Function:
 *      bcmi_xgs5_port_redirect_config_set
 * Purpose:
 *      Initialize the redirection config structure
 * Parameters:
 *      unit             - (IN) device id.
 *      port             - (IN) Port number.
 *      redirect_config  - (IN) Pointer to the redirection config structure
 * Returns:
 *      None
 */
int bcmi_xgs5_port_redirect_config_set(int unit, bcm_gport_t port,
                                     bcm_port_redirect_config_t *redirect_config)
{
    int local_port = -1, flags = 0;

    /* Sanity check */
    if(redirect_config == NULL) {
        return BCM_E_PARAM;
    }

    /* Check the sanity of flags */
    flags = redirect_config->flags;
    if((flags & BCM_PORT_REDIRECT_SOURCE_USE_ORIGINAL_SOURCE) &&
       (flags & BCM_PORT_REDIRECT_SOURCE_USE_ORIGINAL_DESTINATION)) {
        return BCM_E_PARAM;
    }

    if((flags & BCM_PORT_REDIRECT_DROPPED_PACKET) &&
       (flags & BCM_PORT_REDIRECT_NOT_DROPPED_PACKET)) {
        return BCM_E_PARAM;
    }

    /* Check flags that are only applicable on the redirection
       2.0 */
    if(!soc_feature(unit, soc_feature_ep_redirect_v2)) {
        if(flags & (BCM_PORT_REDIRECT_TRUNCATE |
                    BCM_PORT_REDIRECT_SOURCE_USE_ORIGINAL_SOURCE |
                    BCM_PORT_REDIRECT_SOURCE_USE_ORIGINAL_DESTINATION |
                    BCM_PORT_REDIRECT_DROPPED_PACKET |
                    BCM_PORT_REDIRECT_NOT_DROPPED_PACKET |
                    BCM_PORT_REDIRECT_BUFFER_PRIORITY_LOW |
                    BCM_PORT_REDIRECT_BUFFER_PRIORITY_MEDIUM |
                    BCM_PORT_REDIRECT_BUFFER_PRIORITY_HIGH)) {
                return BCM_E_PARAM;
        }
    }

    /* Validate the port and extract the local_port */
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &local_port));

    /* Set the data */
    BCM_IF_ERROR_RETURN(_bcmi_xgs5_port_redirect_config_set(unit,
                                            port, redirect_config));

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcmi_xgs5_port_redirect_get
 * Purpose:
 *      Configure the redirection data
 * Parameters:
 *      unit             - (IN) device id.
 *      port             - (IN) Port number.
 *      redirect_config  - (OUT) Pointer to the redirection config structure
 * Returns:
 *      None
 */
int _bcmi_xgs5_port_redirect_get(int unit, bcm_port_t port,
                          bcm_port_redirect_config_t *redirect_config)
{
    int color = 0, mod_id = -1, port_id = -1;
    int dest_type, dest_value = -1;
    egr_port_entry_t  egr_port_entry;
#if defined(BCM_EP_REDIRECT_VERSION_2)
    int redirect_action_value = 0, redirect_source_value = 0;
    int buffer_priority = 0, truncate_flag = 0;
#endif

    /* If the dest_type_field is invalid, this feature is not
       supported on this device */
    if (PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->dest_type == INVALIDf) {
        return BCM_E_UNAVAIL;
    }

    /* Read the given egress port entry */
    BCM_IF_ERROR_RETURN(soc_mem_read(unit,
            PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->mem, MEM_BLOCK_ANY,
            port, &egr_port_entry));

    /* Zero out the outgoing struct */
    sal_memset(redirect_config, 0, sizeof(bcm_port_redirect_config_t));

    /* Reset flags */
    redirect_config->flags = 0;
    redirect_config->buffer_priority = 0;

    /* Get the dest_type and dest values */
    dest_type = soc_EGR_PORTm_field32_get(unit, &egr_port_entry,
                    PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->dest_type);
    dest_value = soc_EGR_PORTm_field32_get(unit, &egr_port_entry,
                    PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->dest_value);

    /* Switch based on the destination type */
    switch(dest_type) {

        case _BCM_REDIRECT_DEST_TYPE_PORT:

            /* Extract the (Mod,Port) value from destination */
            BCMI_MOD_PORT_MODID_GET(dest_value, mod_id);
            BCMI_MOD_PORT_PORTID_GET(dest_value, port_id);

            BCM_GPORT_MODPORT_SET(redirect_config->destination,
                                  mod_id, port_id);
            redirect_config->flags |= BCM_PORT_REDIRECT_DESTINATION_PORT;

            break;

        case _BCM_REDIRECT_DEST_TYPE_TRUNK:

            /* Put together a trunk gport and set the type */
            BCM_GPORT_TRUNK_SET(redirect_config->destination, dest_value);
            redirect_config->flags |= BCM_PORT_REDIRECT_DESTINATION_TRUNK;

            break;

        case _BCM_REDIRECT_DEST_TYPE_MCAST:

            /* Put together a trunk gport and set the type */
            _BCM_MULTICAST_GROUP_SET(redirect_config->destination,
                                     _BCM_MULTICAST_TYPE_L2,
                                     redirect_config->destination);
            redirect_config->flags |= BCM_PORT_REDIRECT_DESTINATION_MCAST;

            break;

        default:

            /* Reset the destination type */
            dest_type = _BCM_REDIRECT_DEST_TYPE_NONE;

            /* Reset the destination */
            dest_value = 0;

            break;

    } /* End switch */

    /* Get the redirected packet color */
    color = soc_EGR_PORTm_field32_get(unit, &egr_port_entry,
                    PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_color);

#if defined(BCM_EP_REDIRECT_VERSION_2)
    /* Map the internal color values to external ones */
    if(soc_feature(unit, soc_feature_ep_redirect_v2)) {
        switch(color) {
            case 0:
                redirect_config->color = bcmPortRedirectColorGreen;
                break;
            case 1:
                redirect_config->color = bcmPortRedirectColorYellow;
                break;
            case 2:
                redirect_config->color = bcmPortRedirectColorRed;
                break;
            default:
                break;
        }
    } else {
#endif
    switch(color) {
        case 0:
            redirect_config->color = bcmPortRedirectColorNone;
            break;
        case 1:
            redirect_config->color = bcmPortRedirectColorGreen;
            break;
        case 2:
            redirect_config->color = bcmPortRedirectColorYellow;
            break;
        case 3:
            redirect_config->color = bcmPortRedirectColorRed;
            break;

        default:
            break;
    }
#if defined(BCM_EP_REDIRECT_VERSION_2)
    }
#endif

    /* Get the redirected packet priority */
    redirect_config->priority = soc_EGR_PORTm_field32_get(unit,
               &egr_port_entry,
               PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->pkt_priority);

#if defined(BCM_EP_REDIRECT_VERSION_2)
    /* For the newer redirection, get the additionally supported fields */
    if (soc_feature(unit, soc_feature_ep_redirect_v2)) {

        /* Get the redirected packet strength */
        redirect_config->strength = soc_EGR_PORTm_field32_get(unit,
                   &egr_port_entry,
                   PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->strength);

        /* Get the redirected packet buffer priority */
        buffer_priority = soc_EGR_PORTm_field32_get(unit, &egr_port_entry,
                   PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->buffer_priority);

        if (buffer_priority ==
            _BCM_PORT_REDIRECT_BUFFER_PRIORITY_LOW) {
            redirect_config->buffer_priority |=
                BCM_PORT_REDIRECT_BUFFER_PRIORITY_LOW;
        } else if(buffer_priority ==
                  _BCM_PORT_REDIRECT_BUFFER_PRIORITY_MEDIUM) {
            redirect_config->buffer_priority |=
                BCM_PORT_REDIRECT_BUFFER_PRIORITY_MEDIUM;
        } else if(buffer_priority ==
                  _BCM_PORT_REDIRECT_BUFFER_PRIORITY_HIGH) {
            redirect_config->buffer_priority |=
                BCM_PORT_REDIRECT_BUFFER_PRIORITY_HIGH;
        }

        /* Get the redirection action value */
        redirect_action_value = soc_EGR_PORTm_field32_get(unit,
               &egr_port_entry,
               PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->action);

        /* Set the redirected packet action, based on the flags */
        if(redirect_action_value == _BCM_REDIRECT_IF_DROPPED) {
            redirect_config->flags |= BCM_PORT_REDIRECT_DROPPED_PACKET;
        } else if(redirect_action_value ==
                  _BCM_REDIRECT_IF_NOT_DROPPED) {
            redirect_config->flags |= BCM_PORT_REDIRECT_NOT_DROPPED_PACKET;
        }

        /* Get the redirection source value */
        redirect_source_value = soc_EGR_PORTm_field32_get(unit,
                  &egr_port_entry,
                  PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->redir_pkt_source);

        /* Get the redirected packet source to use and update flags */
        if(redirect_source_value ==
           _BCM_REDIRECT_SOURCE_USE_ORIGINAL_DESTINATION) {
            redirect_config->flags |=
                BCM_PORT_REDIRECT_SOURCE_USE_ORIGINAL_DESTINATION;
        } else if(redirect_source_value ==
                  _BCM_REDIRECT_SOURCE_USE_ORIGINAL_SOURCE) {
            redirect_config->flags |=
                BCM_PORT_REDIRECT_SOURCE_USE_ORIGINAL_SOURCE;
        }

        /* Set the redirected packet truncate flag, if it is set */
        truncate_flag = soc_EGR_PORTm_field32_get(unit, &egr_port_entry,
                      PORT_REDIRECTION_HW_DEFS_EGR_PORT(unit)->redir_pkt_truncate);
        if(truncate_flag) {
            redirect_config->flags |= BCM_PORT_REDIRECT_TRUNCATE;
        }
    }

#endif

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_redirect_config_get
 * Purpose:
 *      Get the redirection config data
 * Parameters:
 *      unit             - (IN) device id.
 *      port             - (IN) Port number.
 *      redirect_config  - (OUT) Pointer to the redirection config structure
 * Returns:
 *      None
 */
int bcmi_xgs5_port_redirect_config_get(int unit, bcm_gport_t port,
                                 bcm_port_redirect_config_t *redirect_config)
{
    int local_port = -1;

    /* Sanity check */
    if(redirect_config == NULL) {
        return BCM_E_PARAM;
    }

    /* Validate the port and extract the local_port */
    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit,
                                                     port,
                                                     &local_port));

    /* Get the data */
    BCM_IF_ERROR_RETURN(_bcmi_xgs5_port_redirect_get(unit,
                                                     local_port,
                                                     redirect_config));

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_force_lb_set
 * Purpose:
 *      Force port into internal loopback accroding to bond options.
 * Parameters:
 *      unit          - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_xgs5_port_force_lb_set(int unit)
{
    soc_info_t *si;
    int port, phy_port;

    si = &SOC_INFO(unit);

    PBMP_PORT_ITER(unit, port) {
        if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, port) ||
            IS_MANAGEMENT_PORT(unit,port)) {
            continue;
        }
        phy_port = si->port_l2p_mapping[port];
        if (BCMI_PORT_DRV_PHY_INFO(unit, phy_port).force_lb) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_PHY));
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_force_lb_check
 * Purpose:
 *      Check if port should be forced into internal loopback.
 * Parameters:
 *      unit          - (IN) Unit number.
 *      port          - (IN) Logical port, BCM format.
 *      loopback      - (IN) one of:
 *                           BCM_PORT_LOOPBACK_NONE
 *                           BCM_PORT_LOOPBACK_MAC
 *                           BCM_PORT_LOOPBACK_PHY
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
bcmi_xgs5_port_force_lb_check(int unit, int port, int loopback)
{
    soc_info_t *si;
    int phy_port, local_port, phy_lane, phyn, sys_side;

    si = &SOC_INFO(unit);
    local_port = -1;

    BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_phyn_validate(unit, port,
                         &local_port, &phyn, &phy_lane, &sys_side));

    if (local_port == -1) {
       BCM_IF_ERROR_RETURN(_bcm_esw_port_gport_validate(unit, port, &port));
    } else {
        port = local_port;
    }

    if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, port) ||
        IS_MANAGEMENT_PORT(unit,port)) {
        return BCM_E_NONE;
    }

    phy_port = si->port_l2p_mapping[port];
    if (BCMI_PORT_DRV_PHY_INFO(unit, phy_port).force_lb &&
        loopback != BCM_PORT_LOOPBACK_PHY) {
        return BCM_E_PARAM;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_resource_status_update
 * Purpose:
 *      Update the port resource status to depedent modules after
 *      flex operation.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 * Returns:
 *      BCM_E_XXX
 * Note:
 *      - Assumes caller has lock.
 *      - This is to be called by the API dispatch driver.
 *      - Should be invoked from resource_multi_set() routine
 *        after completion of flex operation.
 */
int
bcmi_xgs5_port_resource_status_update(int unit,
                                      int nport,
                                      bcm_port_resource_t *resource)
{
    int i;
    bcm_pbmp_t detached_pbmp;
    bcm_pbmp_t attached_pbmp;
    bcm_port_t local_port;

    if (resource == NULL) {
        return BCM_E_PARAM;
    }

    BCM_PBMP_CLEAR(detached_pbmp);
    BCM_PBMP_CLEAR(attached_pbmp);
    for (i = 0; i < nport; i++) {
        /* Check that logical port number is addressable and convert */
        BCM_IF_ERROR_RETURN
            (bcmi_xgs5_port_addressable_local_get(unit,
                                                  resource[i].port,
                                                  &local_port));
        if (resource[i].physical_port == -1) {
           BCM_PBMP_PORT_ADD(detached_pbmp, local_port);
        } else {
           BCM_PBMP_PORT_ADD(attached_pbmp, local_port);
        }
    }

    /* Update SDK field module about the detached and attached ports to the device. */
    return _bcm_field_flex_port_update(unit, detached_pbmp, attached_pbmp);
}

/* Debug functions for Flexport */

/*
 * Function:
 *      bcmi_xgs5_num_tsc
 * Purpose:
 *      Get number of TSC in the device, not include management TSC
 * Parameters:
 *      unit    - (IN) Unit number.
 *      num     - (OUT) NUmber of TSC
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_num_tsc(int unit, int *num)
{
    if (BCMI_PORT_DRV(unit) == NULL) {
        return BCM_E_UNAVAIL;
    }

    if (num == NULL) {
        return BCM_E_PARAM;
    }

    *num = BCMI_PORT_DRV_DEV_INFO(unit)->num_tsc;

    return SOC_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_phy_to_pipe
 * Purpose:
 *      Get pipe from the physical port
 * Parameters:
 *      unit    - (IN) Unit number.
 *      phy_port- (IN) Physical port
 *      pipe    - (OUT) Pipe
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_phy_to_pipe(int unit, int phy_port, int *pipe)
{
    if (BCMI_PORT_DRV(unit) == NULL) {
        return BCM_E_UNAVAIL;
    }

    if (pipe == NULL) {
        return BCM_E_PARAM;
    }

    *pipe = BCMI_PORT_DRV_PHY_INFO(unit, phy_port).pipe;

    return SOC_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_port_range
 * Purpose:
 *      Get logical port range in a pipe
 * Parameters:
 *      unit    - (IN) Unit number.
 *      pipe    - (IN) Pipe
 *      port_start - (OUT) First logical port of the pipe
 *      port_end   - (OUT) Last logical port of the pipe
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_port_range(int unit, int pipe, int *port_start, int *port_end)
{
    bcmi_xgs5_dev_info_t *dev_info;

    if (BCMI_PORT_DRV(unit) == NULL) {
        return BCM_E_UNAVAIL;
    }

    if ((port_start == NULL) || (port_end == NULL)) {
        return BCM_E_PARAM;
    }

    dev_info = BCMI_PORT_DRV_DEV_INFO(unit);
    if (dev_info->pipe_bound) {
        *port_start = dev_info->pipe_log_port_base[pipe];
        *port_end   = *port_start + dev_info->ports_pipe_max[pipe] - 1;
    } else {
        *port_start = 0;
        *port_end   = dev_info->ports_pipe_max[dev_info->num_pipe - 1] - 1;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      bcmi_xgs5_speed_to_lanes
 * Purpose:
 *      Get number of lanes based on ports speed in a PM.
 *      If a speed have multiple lane mode (eg, 2-lane 40G and 4-lane 40G),
 *      select as much lanes as possible.
 * Example:
 *      PM need to be configured as 1x40G+2x10G
 *      Input speed[] = {40000, 0, 10000, 10000}
 *      Output lanes[] = {2, 0, 1, 1}
 * Parameters:
 *      unit        - (IN) Unit number.
 *      lane_max    - (IN) Max lanes in the PM
 *      speed       - (IN) Port speed array in the PM
 *      lanes       - (OUT) Lanes array in the PM
 * Returns:
 *      SOC_E_XXX
 */
int
bcmi_xgs5_speed_to_lanes(int unit, int lane_max, int *speed, int *lanes)
{
    int i, j;
    int speed_mask[SOC_PORT_RESOURCE_LANES_MAX];
    int port_lanes[SOC_PORT_RESOURCE_LANES_MAX] = {0};
    bcmi_xgs5_dev_info_t *dev_info;

    if (BCMI_PORT_DRV(unit) == NULL) {
        return BCM_E_UNAVAIL;
    }

    if ((speed == NULL) || (lanes == NULL)) {
        return BCM_E_PARAM;
    }

    dev_info = BCMI_PORT_DRV_DEV_INFO(unit);

    /* calculate max lanes of each port */
    for (i = 0, j = 0; i < lane_max; i ++) {
        speed_mask[i] = SOC_PA_SPEED(speed[i]);
        if (speed_mask[i] != 0) {
            j = i;
        }
        port_lanes[j] ++;
    }

    /* select max lanes of a speed */
    for (i = 0; i < lane_max; i ++) {
        if (port_lanes[i] > 0) {
            for (j = port_lanes[i]; j > 0; j --) {
                if (dev_info->speed_valid[j] & speed_mask[i]) {
                    lanes[i] = j;
                    break;
                }
            }
            if (j == 0) {
                return SOC_E_PARAM;
            }
        } else {
            lanes[i] = 0;
        }
    }

    return SOC_E_NONE;
}

/* End of debug functions for Flexport */

#endif /* BCM_XGS5_SWITCH_PORT_SUPPORT */
