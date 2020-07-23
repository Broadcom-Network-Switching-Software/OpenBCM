/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        port.c
 * Purpose:     Apache SOC Port driver.
 *
 *              Contains information and interfaces for the
 *              physical port in the device.
 */

#include <shared/bsl.h>
#include <soc/defs.h>

#if defined(BCM_APACHE_SUPPORT)
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/error.h>
#include <soc/apache.h>
#include <soc/trident2.h>
#include <soc/bondoptions.h>
#include <soc/esw/port.h>
#include <soc/esw/portctrl.h>
#include <soc/esw/portctrl_internal.h>
#include <soc/portmod/portmod_legacy_phy.h>
#include <soc/portmod/portmod_defs.h>
#include <soc/mem.h>
#include <sal/appl/sal.h>

#define PORT_BLOCK_BASE_PORT(port) \
        (1 + ((SOC_INFO(unit).port_l2p_mapping[port] - 1) & ~0x3));

#ifdef BCM_WARM_BOOT_SUPPORT

#define SOC_FLEXPORT_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define SOC_FLEXPORT_WB_DEFAULT_VERSION            SOC_FLEXPORT_WB_VERSION_1_0

#endif

/*
 * Physical Port Information
 *
 *     Some information are defined in HW and others are defined
 *     through SOC properties during configuration time.
 */
typedef struct ap_phy_port_s {
    /*
     * The following fields are defined in HW
     */
    soc_port_lane_info_t lane_info;    /* Lane information in PGW block */
    uint32               lanes_valid;  /* Lanes capabilities */
    int                  pipe;         /* unused */
    int                  sisters_ports[_AP_PORTS_PER_TSC];
                                       /* Ports part of the XLPORT/XLMAC/TSC-4
                                          where the physical port resides */

    /*
     * The following fields are configurable throught SOC properties
     */
    uint16               prio_mask;    /* Packet priority to priority
                                          group mapping mask */
    int                  flex;         /* Flex enable by SOC property
                                          'port_flex_enable' */
    uint32               cl91_enabled; /* Indicated if CL91 is enabled */
} ap_phy_port_t;

/*
 * Physical Device information
 */
typedef struct ap_info_s {
    ap_phy_port_t  phy_port[SOC_MAX_NUM_PORTS];
    int              phy_ports_max;   /* Max physical ports in device */
    uint32           speed_valid[SOC_PORT_RESOURCE_LANES_MAX+1];
                                      /* Port rate ability on serdes lane */
    int              speed_max;       /* Max speed on any port in device */
    int              port_max;        /* Max logical ports */

    /*
     * The following fields are configurable throught SOC properties
     */
    int              mmu_lossless;    /* MMU lossless */
    int              flex_legacy;     /* Flex enable by legacy ':i' in
                                         SOC property 'portmap' */

} ap_info_t;

/*
 * Contains information corresponding to each physical port.
 * This information is fixed in a device and is calculated
 * only during init time.
 */
static ap_info_t                  *ap_info[SOC_MAX_NUM_DEVICES];

#define AP_INFO(_u)               (ap_info[(_u)])
#define AP_PHY_PORT(_u, _p)       (AP_INFO(_u)->phy_port[(_p)])
#define AP_PHY_PORT_LANE(_u, _p)  (AP_PHY_PORT(_u, _p).lane_info)


/* Lanes support */
#define AP_PHY_XLPORT_LANES_1      (1 << 0)
#define AP_PHY_XLPORT_LANES_2      (1 << 1)
#define AP_PHY_XLPORT_LANES_4      (1 << 2)
#define AP_PHY_XLPORT_LANES_10     (1 << 3)
#define AP_PHY_XLPORT_LANES_12     (1 << 4)


/* Port Rate support */
/* Ethernet */
#define AP_PHY_PORT_RATE_1       (1 << 0)
#define AP_PHY_PORT_RATE_2_5     (1 << 1)
#define AP_PHY_PORT_RATE_5       (1 << 2)
#define AP_PHY_PORT_RATE_10      (1 << 3)
#define AP_PHY_PORT_RATE_20      (1 << 4)
#define AP_PHY_PORT_RATE_25      (1 << 5)
#define AP_PHY_PORT_RATE_40      (1 << 6)
#define AP_PHY_PORT_RATE_50      (1 << 7)
#define AP_PHY_PORT_RATE_100     (1 << 8)
/* HG */
#define AP_PHY_PORT_RATE_11      (1 << 9)
#define AP_PHY_PORT_RATE_21      (1 << 10)
#define AP_PHY_PORT_RATE_27      (1 << 11)
#define AP_PHY_PORT_RATE_42      (1 << 12)
#define AP_PHY_PORT_RATE_53      (1 << 13)
#define AP_PHY_PORT_RATE_106     (1 << 14)
#define AP_PHY_PORT_RATE_127     (1 << 15)


/* Default for the max port speed configured for device */
#define AP_FLEX_MAX_NUM_PORTS        76
#define AP_FLEX_SPEED_MAX_DEFAULT    106000

/* 100GE Mode Lanes */
#define APACHE_100GE_MLD_MAP_4_4_2_STR  "0xff9876543210"
#define APACHE_100GE_MLD_MAP_3_4_3_STR  "0xf9876543f210"
#define APACHE_100GE_MLD_MAP_2_4_4_STR  "0x98765432ff10"


/*
 * Define:
 *      AP_INFO_INIT_CHECK
 * Purpose:
 *      Causes a routine to return SOC_E_INIT if module is not yet initialized.
 */
#define AP_INFO_INIT_CHECK(_u) \
    if (ap_info[_u] == NULL) { return SOC_E_INIT; }

/*
 * Define:
 *      AP_PHY_PORT_ADDRESSABLE
 * Purpose:
 *      Checks that physical port is addressable (within valid range).
 */
#define AP_PHY_PORT_ADDRESSABLE(_u, _phy_port)                  \
    (soc_ap_phy_port_addressable((_u), (_phy_port)) == SOC_E_NONE)

#define PORT_IS_FALCON(pport) \
    ((((pport >= 29) && (pport <= 36)) || ((pport >= 65) && (pport <= 72))) ? TRUE : FALSE)
#define PORT_IS_CXX(pport) \
    (((pport == 17) || (pport == 53)) ? TRUE : FALSE)

/*
 * Function:
 *      soc_ap_soc_info_misc_t_init
 * Purpose:
 *      Initialize soc_info_misc_t structure by the current SOC_INFO(unit).
 * Parameters:
 *      unit - StrataSwitch Unit #.
 *      si_m - soc_info_misc data
 * Note:
 *      port_speed is initialized by si->port_speed_max
 */
STATIC void
_soc_ap_soc_info_misc_t_init(int unit, soc_info_misc_t *si_m)
{
    soc_info_t *si = &SOC_INFO(unit);
    int i;
    int logic_port;
    int phy_port;
    int speed;

    sal_memset(si_m, 0, sizeof(*si_m));

    /* Get current information */
    for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
        si_m->port_l2p_mapping[i] = si->port_l2p_mapping[i];
        si_m->port_p2l_mapping[i] = si->port_p2l_mapping[i];
        si_m->port_speed_max[i] = si->port_speed_max[i];
        si_m->port_num_lanes[i] = si->port_num_lanes[i];
        si_m->port_speed[i] = -1;
        si_m->port_encap[i] = -1;

        logic_port = i;
        phy_port = si->port_l2p_mapping[logic_port];

        /* Skip ports not defined, LB, CPU, etc. */
        /* coverity[overrun-local] */
        if (!SOC_PORT_ADDRESSABLE(unit, logic_port) ||
            !((phy_port >= 0) && (phy_port <= SOC_MAX_NUM_PORTS)) ||
            IS_LB_PORT(unit, logic_port) ||
            IS_RDB_PORT(unit, logic_port) ||
            IS_CPU_PORT (unit, logic_port)) {
            continue;
        }

        /* Get current port speed */
        speed = si->port_speed_max[logic_port];
        si_m->port_speed[logic_port] = speed;

        /*
         * Get encapsulation mode
         *
         * Validate need to differentiate between HG and non-HG modes,
         * so using SOC_ENCAP_HIGIG2 and SOC_ENCAP_IEEE will suffice
         * to indicate HG vs non-HG.
         */
        /* coverity[overrun-local] */
        if (IS_HG_PORT(unit, logic_port)) {
            si_m->port_encap[logic_port] = SOC_ENCAP_HIGIG2;
        } else {
            si_m->port_encap[logic_port] = SOC_ENCAP_IEEE;
        }
    }
    si_m->bandwidth = si->bandwidth;
    SOC_PBMP_ASSIGN(si_m->oversub_pbm, si->oversub_pbm);
    SOC_PBMP_ASSIGN(si_m->disabled_bitmap, si->all.disabled_bitmap);

    return;
}

/*
 * Function:
 *      soc_ap_port_oversub_get
 * Purpose:
 *      Get the Oversub for given port.
 * Parameters:
 *      unit         - (IN) Unit number.
 *      phy_port     - (IN) Physical port number, used
 *                          for NEW property.
 *      logical_port - (IN) Logical port number.
 *                          This is used for LEGACY property in
 *                          case the NEW property is not present.
 *      oversub      - (OUT) Returns TRUE if port is set for Oversub,
 *                           FALSE otherwise.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Assumes port is valid.
 */
STATIC int
soc_ap_port_oversub_get(int unit, int phy_port, soc_port_t logical_port, int *oversub)
{
    soc_pbmp_t pbmp;
    int oversub_mode = 0;

    SOC_PBMP_CLEAR(pbmp);

    oversub_mode = soc_property_get(unit, spn_OVERSUBSCRIBE_MODE, 0);

    if (oversub_mode == 0) {
        *oversub = 0;
    } else if (oversub_mode == 1) {
        *oversub = 1;
    } else if (oversub_mode == 2) {
        /*
         * Check per physical port property first.
         * If property is not available on physical port,
         * use property on logical port.
         */
        *oversub = soc_property_phys_port_get(unit, phy_port,
                                              spn_PORT_OVERSUBSCRIBE, 0xFFFFFFFF);
        if (*oversub == 0xFFFFFFFF) {
            *oversub = 0;
            if (logical_port != -1) {
                pbmp = soc_property_get_pbmp(unit, spn_PBMP_OVERSUBSCRIBE, 0);
                if (SOC_PBMP_MEMBER(pbmp, logical_port)) {
                    *oversub = 1;
                }
            }
        }
    }
    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_ap_post_soc_info_get
 * Purpose:
 *      Get the post FlexPort SOC INFO port information (subset).
 *
 *      Certain validations need the post FlexPort information for all
 *      the ports at the same time (mostly contained in SOC_INFO).
 *      The global SOC_INFO is updated after the validation phase so
 *      the information is not available yet.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN/OUT) Port Resource FlexPort configuration.
 *      post_si        - (OUT) Returns the new SOC port information
 *                       after FlexPort.  Note that this does NOT
 *                       modify the global SOC_INFO SW database.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      This function does NOT modify the global SOC_INFO SW database,
 *      the post information is returned in the local argument.
 *
 *      Assumes values in resource are valid (logical port, etc.)
 */
STATIC int
_soc_ap_post_soc_info_get(int unit,
                            int nport,
                            soc_port_resource_t *resource,
                            soc_info_misc_t *post_si)
{
    int rv;
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *pr;
    int i;
    int oversub;
    int logic_port;
    int phy_port;

    _soc_ap_soc_info_misc_t_init(unit, post_si);

    /* First process 'delete' mappings */
    for (i = 0, pr = &resource[0];
         (i < nport) && (pr->physical_port == -1);
         i++, pr++) {
        logic_port = pr->logical_port;
        phy_port = si->port_l2p_mapping[logic_port];

        if (pr->flags & SOC_PORT_RESOURCE_I_MAP) {
            SOC_PBMP_PORT_ADD(post_si->disabled_bitmap, logic_port);
            continue;
        }

        post_si->port_l2p_mapping[logic_port] = -1;
        if (phy_port >= 0) {
            post_si->port_p2l_mapping[phy_port] = -1;
        }
        post_si->port_speed_max[logic_port] = -1;
        post_si->port_speed[logic_port] = -1;
        post_si->port_num_lanes[logic_port] = -1;
        post_si->port_encap[logic_port] = -1;

        SOC_PBMP_PORT_REMOVE(post_si->oversub_pbm, logic_port);
        SOC_PBMP_PORT_REMOVE(post_si->disabled_bitmap, logic_port);
    }

    /* Continue with 'add' mappings in rest of array */
    for (; i < nport; i++, pr++) {
        logic_port = pr->logical_port;
        phy_port = pr->physical_port;

        post_si->port_l2p_mapping[logic_port] = phy_port;
        post_si->port_p2l_mapping[phy_port] = logic_port;
        post_si->port_speed_max[logic_port] = pr->speed;
        post_si->port_speed[logic_port] = pr->speed;
        post_si->port_num_lanes[logic_port] = pr->num_lanes;
        post_si->port_encap[logic_port] = pr->encap;

        rv = soc_ap_port_oversub_get(unit, phy_port, logic_port, &oversub);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "Cannot get oversubscription mode, "
                                  "logical_port=%d physical_port=%d rv=%d\n"),
                       logic_port, phy_port, rv));
            return SOC_E_INTERNAL;
        }
        if (oversub) {
            SOC_PBMP_PORT_ADD(post_si->oversub_pbm, logic_port);
        } else {
            SOC_PBMP_PORT_REMOVE(post_si->oversub_pbm, logic_port);
        }

        SOC_PBMP_PORT_REMOVE(post_si->disabled_bitmap, logic_port);
    }


    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "\n--- SOC INFO Post FlexPort Data ---\n")));
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "Index L2P  P2L  MaxSpeed   Speed  "
                            "Encap Ovs Disabled\n")));
    for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
        if ((post_si->port_l2p_mapping[i] == -1) &&
            (post_si->port_p2l_mapping[i] == -1)) {
            continue;
        }
        if (!AP_PHY_PORT_ADDRESSABLE(unit, i)) {
            continue;
        }
        /* coverity[overrun-local] */
        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                " %3d  %3d  %3d   %6d   %6d   "
                                "%3s   %1d     %1d\n"),
                     i,
                     post_si->port_l2p_mapping[i],
                     post_si->port_p2l_mapping[i],
                     post_si->port_speed_max[i],
                     post_si->port_speed[i],
                     (post_si->port_encap[i] == SOC_ENCAP_HIGIG2) ?
                     "HG" : "!HG",
                     SOC_PBMP_MEMBER(post_si->oversub_pbm, i) ? 1 : 0,
                     SOC_PBMP_MEMBER(post_si->disabled_bitmap, i) ? 1 : 0));
    }
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "\n")));

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_ap_phy_port_lane_info_init
 * Purpose:
 *      Initialize the physical port lane information in the
 *      SW device information data structure.
 * Parameters:
 *      unit     - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumes SW data structure memory is valid.
 */
STATIC int
_soc_ap_phy_port_lane_info_init(int unit)
{
    int i;
    int tsc, pgw;
    int blk, bindex;
    int phy_port, phy_port_base;

    /* Set information to invalid */
    for (phy_port = 0; phy_port < SOC_MAX_NUM_PORTS; phy_port++) {
        AP_PHY_PORT(unit, phy_port).pipe = -1;
        AP_PHY_PORT_LANE(unit, phy_port).pgw = -1;
        AP_PHY_PORT_LANE(unit, phy_port).xlp = -1;
        AP_PHY_PORT_LANE(unit, phy_port).port_index = -1;
    }

    for (phy_port = 0; phy_port < SOC_MAX_NUM_PORTS; phy_port++) {

        /* Check for end of port list */
        if (SOC_PORT_BLOCK(unit, phy_port) < 0 &&
            SOC_PORT_BINDEX(unit, phy_port) < 0) {
            break;
        }

        /* Skip internal ports */
        if ((phy_port == AP_PHY_PORT_CPU) ||
            (phy_port == AP_PHY_PORT_RDB) ||
            (phy_port == AP_PHY_PORT_LB)) {
            continue;
        }

        /* Set lane information */
        for (i = 0; i < SOC_DRIVER(unit)->port_num_blktype; i++) {
            /* Find PGW block */
            blk = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
            bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, i);

            if ((blk < 0) || (bindex < 0)) {
                continue;
            }

            /* Set PGW block information for lane */
            if (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_PGW_CL) {
                /* Get PGW, TSC and Port Index values */
                AP_PHY_PORT_LANE(unit, phy_port).pgw =
                    SOC_BLOCK_NUMBER(unit, blk);
                AP_PHY_PORT_LANE(unit, phy_port).tsc =
                    bindex / _AP_PORTS_PER_TSC;
                AP_PHY_PORT_LANE(unit, phy_port).port_index =
                    bindex % _AP_PORTS_PER_TSC;
            }

            /* Set port lane capabilites */
            if ((SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_XLPORT) ||
                (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_XLPORTB0)) {
                AP_PHY_PORT(unit, phy_port).lanes_valid =
                    AP_PHY_XLPORT_LANES_1;
                if (bindex == 0) {
                    AP_PHY_PORT(unit, phy_port).lanes_valid |=
                        AP_PHY_XLPORT_LANES_2 | AP_PHY_XLPORT_LANES_4;
                } else if (bindex == 2) {
                    AP_PHY_PORT(unit, phy_port).lanes_valid |=
                        AP_PHY_XLPORT_LANES_2;
                }
            } else if ((SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CLPORT) ||
                       (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CLG2PORT)) {
                AP_PHY_PORT(unit, phy_port).lanes_valid =
                    AP_PHY_XLPORT_LANES_1;
                if (bindex == 0) {
                    AP_PHY_PORT(unit, phy_port).lanes_valid |=
                        AP_PHY_XLPORT_LANES_2 | AP_PHY_XLPORT_LANES_4;
                } else if (bindex == 2) {
                    AP_PHY_PORT(unit, phy_port).lanes_valid |=
                        AP_PHY_XLPORT_LANES_2;
                }
            }

            if (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CXXPORT) {
                AP_PHY_PORT(unit, phy_port).lanes_valid |=
                    AP_PHY_XLPORT_LANES_10 | AP_PHY_XLPORT_LANES_12;
            }

        } /* for port_num_blktype */

    } /* for phy_port */

    /* Set total number of physical ports in device */
    AP_INFO(unit)->phy_ports_max = phy_port;

    /*
     * Fill sisters ports
     * This logic assumes a given physical layout in the device
     */
    for (phy_port = 0; phy_port < AP_INFO(unit)->phy_ports_max;) {

        if (AP_PHY_PORT_LANE(unit, phy_port).pgw == -1) {
            phy_port++;
            continue;
        }

        pgw = AP_PHY_PORT_LANE(unit, phy_port).pgw;
        tsc = AP_PHY_PORT_LANE(unit, phy_port).tsc;
        phy_port_base = phy_port;
        while ((AP_PHY_PORT_LANE(unit, phy_port).pgw == pgw) &&
               (AP_PHY_PORT_LANE(unit, phy_port).tsc == tsc)) {
            for (i = 0; i < _AP_PORTS_PER_TSC; i++) {
                AP_PHY_PORT(unit, phy_port).sisters_ports[i] =
                    phy_port_base + i;
            }
            phy_port++;
        }
    }

    return SOC_E_NONE;
}



/*
 * Function:
 *      _soc_ap_prio_mask_init
 * Purpose:
 *      Initialize the packet priority to priority group mapping mask
 *      for physical ports in the SW device information data structure.
 * Parameters:
 *      unit     - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumes SW data structure memory is valid.
 */
STATIC int
_soc_ap_prio_mask_init(int unit)
{
    int phy_port;
    uint16 prio_mask;

    for (phy_port = 0; phy_port < SOC_MAX_NUM_PORTS; phy_port++) {
        /* Skip ports not addressable */
        if (!AP_PHY_PORT_ADDRESSABLE(unit, phy_port)) {
            continue;
        }

        prio_mask = 0xffff;
        /*
         * Check per physical port property first.
         * If property is not available on physical port,
         * use property on logical port.
         */

        /*
         * TBD once SOC property is approved.
         * if (property_physical) {
         *     ...
         * } else {
         *     ... logical property
         * }
         */

#if 0
        /* Set based on logical port property */
        port = si->port_p2l_mapping[phy_port];
        if (port != -1) {
            SOC_IF_ERROR_RETURN
                (soc_ap_get_prio_map(unit, port, &prio_mask));
        }
#endif

        AP_PHY_PORT(unit, phy_port).prio_mask = prio_mask;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_ap_flex_enable_init
 * Purpose:
 *      Initialize the flex enable capability
 *      for physical ports in the SW device information data structure.
 *
 *      The flexing capabality on a port is defined based on
 *      these SOC properties:
 *
 *      (1) 'portmap' with ':i' option (inactive ports)
 *          When defined, this allows flexing in the entire device.
 *          This sets AP_INFO(unit)->flex_legacy
 *
 *      (2) 'port_flex_enable'
 *           This SOC property is set per TSC.
 *           When set, FlexPort operations are allowed in the TSC.
 *           Property should be specified in the base physical port of a TSC.
 *           This sets AP_PHY_PORT(unit, phy_port).flex
 *
 *       The defined SDK behaviour is that if both, (1) and (2) are true,
 *       (1) takes precedence.
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumes SW data structure memory is valid.
 *      Assumes _soc_apache_phy_port_lane_info_init() has been called.
 */
STATIC int
_soc_ap_flex_enable_init(int unit)
{
    int phy_port;
    int port_base;
    int flex;
    int i;
    soc_info_t *si = &SOC_INFO(unit);

    /*
     * This field is set in SOC_INFO if inactive ports are specified
     * in the SOC property 'portmap' with the ':i' option.
     */
    AP_INFO(unit)->flex_legacy = si->flex_eligible;

    for (phy_port = 0; phy_port < SOC_MAX_NUM_PORTS;) {
        /* Skip ports not addressable, CPU and Loopback ports */
        if (!AP_PHY_PORT_ADDRESSABLE(unit, phy_port) ||
            (phy_port == AP_PHY_PORT_CPU) ||
            (phy_port == AP_PHY_PORT_RDB) ||
            (phy_port == AP_PHY_PORT_LB)) {
            phy_port++;
            continue;
        }

        /* Check for flex ability in 'port_flex_enable' SOC property */
        port_base = AP_PHY_PORT(unit, phy_port).sisters_ports[0];
        flex = soc_property_phys_port_get(unit, port_base,
                                          spn_PORT_FLEX_ENABLE, 0);
        /*
         * Set flex ability in all ports in the TSC based on
         * property set on base port in TSC
         *
         * Assumes Apache specific physical device layout
 */
        for (i = 0; i < _AP_PORTS_PER_TSC; i++) {
            AP_PHY_PORT(unit, phy_port).flex = flex;
            phy_port++;
            if (phy_port >= SOC_MAX_NUM_PORTS) {
                break;
            }
        }
    }

    return SOC_E_NONE;
}



/*
 * Function:
 *      _soc_ap_speed_valid_init
 * Purpose:
 *      Initialize the SW device information data structure
 *      for the valid port rates according to the number of serdes lanes.
 * Parameters:
 *      unit      - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      - Assumes SW data structure memory is valid.
 */
STATIC int
_soc_ap_speed_valid_init(int unit)
{
    int lanes;
    uint32 speed_valid;

    /* Set port rate based on number of serdes lanes */
    for (lanes = 0; lanes <= SOC_PORT_RESOURCE_LANES_MAX; lanes++) {

        if (lanes == 1) {
            speed_valid =
                AP_PHY_PORT_RATE_1 |
                AP_PHY_PORT_RATE_2_5 |
                AP_PHY_PORT_RATE_5 |
                AP_PHY_PORT_RATE_10 |
                AP_PHY_PORT_RATE_11 |
                AP_PHY_PORT_RATE_25 |
                AP_PHY_PORT_RATE_27 ;

        } else if (lanes == 2) {
            speed_valid =
                AP_PHY_PORT_RATE_10 |
                AP_PHY_PORT_RATE_11 |
                AP_PHY_PORT_RATE_20 |
                AP_PHY_PORT_RATE_21 |
                AP_PHY_PORT_RATE_40 |
                AP_PHY_PORT_RATE_42 |
                AP_PHY_PORT_RATE_50 |
                AP_PHY_PORT_RATE_53 ;

        } else if (lanes == 4) {
            speed_valid =
                AP_PHY_PORT_RATE_10 |
                AP_PHY_PORT_RATE_11 |
                AP_PHY_PORT_RATE_20 |
                AP_PHY_PORT_RATE_21 |
                AP_PHY_PORT_RATE_25 |
                AP_PHY_PORT_RATE_40 |
                AP_PHY_PORT_RATE_42 |
                AP_PHY_PORT_RATE_100 |
                AP_PHY_PORT_RATE_106;

        } else if (lanes == 10) {
            speed_valid =
                AP_PHY_PORT_RATE_100 |
                AP_PHY_PORT_RATE_106;

        } else if (lanes == 12) {
            speed_valid =
                AP_PHY_PORT_RATE_127;

        } else {
            speed_valid = 0;
        }

        AP_INFO(unit)->speed_valid[lanes] = speed_valid;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_ap_ports_pipe_max_init
 * Purpose:
 *      Initialize the SW device information data structure for:
 *      - speed_max      : Maximum allowed port speed in device.
 *      - port_max       : Maximum logical port number.
 *
 *
 * Parameters:
 *      unit      - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      - Assumes SW data structure memory is valid.
 *      - Assumes SOC information is initialized.
 */
STATIC int
_soc_ap_ports_pipe_max_init(int unit)
{
    int speed_max;

    /* Get speed from SOC property */
    speed_max = soc_property_get(unit, spn_PORT_FLEX_SPEED_MAX,
                                 AP_FLEX_SPEED_MAX_DEFAULT);

    AP_INFO(unit)->speed_max = speed_max;
    AP_INFO(unit)->port_max = 74;

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_ap_mmu_lossless_init
 * Purpose:
 *      Initialize the SW device information data structure
 *      for the MMU lossless.
 * Parameters:
 *      unit      - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumes SW data structure memory is valid.
 */
STATIC int
_soc_ap_mmu_lossless_init(int unit)
{
    AP_INFO(unit)->mmu_lossless =
        soc_property_get(unit, spn_MMU_LOSSLESS, 0);

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_ap_phy_info_init
 * Purpose:
 *      Initialize the main SW device information data structure.
 *
 *      It populates the information of the device that are relevant
 *      to the physical port interface such as,
 *      PGW, XLP, port index, lane capabilities, speed, etc.
 *
 *      Information is derived from the HW port design specification
 *      and configuration properties on a physical port.
 *
 *      This routine does not modify any HW.  It only
 *      modifies the SW data structure.
 * Parameters:
 *      unit     - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      - Assumes SOC Control information has been initialized.
 *      - Must be called after the AP port configuration init routine
 *        soc_apache_port_config_init().
 */
int
soc_ap_phy_info_init(int unit)
{
    int phy_port;    /* Physical port */
    int lanes;       /* Serdes lane */

    /* Allocate memory for main SW data structure */
    if (ap_info[unit] == NULL) {
        ap_info[unit] = sal_alloc(sizeof(ap_info_t), "ap_info");
        if (ap_info[unit] == NULL) {
            LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "Unable to allocate memory for "
                                  "apache_info")));
            return SOC_E_MEMORY;
        }
    }

    /* Clear data structure */
    sal_memset(ap_info[unit], 0, sizeof(ap_info_t));

    /* Initialize device information */
    SOC_IF_ERROR_RETURN(_soc_ap_phy_port_lane_info_init(unit));

    SOC_IF_ERROR_RETURN(_soc_ap_prio_mask_init(unit));

    SOC_IF_ERROR_RETURN(_soc_ap_flex_enable_init(unit));

    SOC_IF_ERROR_RETURN(_soc_ap_speed_valid_init(unit));

    SOC_IF_ERROR_RETURN(_soc_ap_ports_pipe_max_init(unit));

    SOC_IF_ERROR_RETURN(_soc_ap_mmu_lossless_init(unit));

    /* Debug output */
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "Physical ports max: %d  speed_max=%d "
                            "port_max=%d mmu_lossless=%d flex_legacy=%d\n\n"),
                 AP_INFO(unit)->phy_ports_max,
                 AP_INFO(unit)->speed_max,
                 AP_INFO(unit)->port_max,
                 AP_INFO(unit)->mmu_lossless,
                 AP_INFO(unit)->flex_legacy));

    for (phy_port = 0; phy_port < AP_INFO(unit)->phy_ports_max; phy_port++) {
        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "Phy port=%d pgw=%d tsc=%d index=%d "
                                "valid=0x%x "
                                "prio_mask=0x%x flex=%d\n"),
                     phy_port,
                     AP_PHY_PORT_LANE(unit, phy_port).pgw,
                     AP_PHY_PORT_LANE(unit, phy_port).tsc,
                     AP_PHY_PORT_LANE(unit, phy_port).port_index,
                     AP_PHY_PORT(unit, phy_port).lanes_valid,
                     AP_PHY_PORT(unit, phy_port).prio_mask,
                     AP_PHY_PORT(unit, phy_port).flex));
        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "    sisters_ports={%d,%d,%d,%d}\n"),
                     AP_PHY_PORT(unit, phy_port).sisters_ports[0],
                     AP_PHY_PORT(unit, phy_port).sisters_ports[1],
                     AP_PHY_PORT(unit, phy_port).sisters_ports[2],
                     AP_PHY_PORT(unit, phy_port).sisters_ports[3]));

    }

    for (lanes = 0; lanes <= SOC_PORT_RESOURCE_LANES_MAX; lanes++) {
        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "Serdes lane=%d port_rate=0x%x\n"),
                     lanes, AP_INFO(unit)->speed_valid[lanes]));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_ap_phy_info_deinit
 * Purpose:
 *      Free ap_info data structure.
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_ap_phy_info_deinit(int unit)
{
    /* Allocate memory for main SW data structure */
    if (ap_info[unit] != NULL) {
        sal_free(ap_info[unit]);
        ap_info[unit] = NULL;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_ap_phy_port_flex_valid
 * Purpose:
 *      Check that FlexPort operation is valid on given physical port.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      phy_port  - (IN) Physical port number.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_ap_phy_port_flex_valid(int unit, int phy_port)
{
    if (!AP_INFO(unit)->flex_legacy &&
        !AP_PHY_PORT(unit, phy_port).flex) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "FlexPort operation is not enabled on "
                              "physical port %d\n"),
                   phy_port));
        return SOC_E_CONFIG;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_apache_is_flex_enable
 * Purpose:
 *      Check that FlexPort operation is valid on the unit.
 *      Checks for both legacy and new flex operation.
 * Parameters:
 *      unit      - (IN) Unit number.
 * Returns:
 *      TRUE or FALSE.
 */
int
soc_apache_is_flex_enable(int unit)
{
    int phy_port, num_phy_port;
    soc_info_t *si = &SOC_INFO(unit);

    num_phy_port = 130;
    if (si->flex_eligible) {
        return TRUE;
    } else {
        for (phy_port = 0; phy_port < num_phy_port;
             phy_port++) {
            if (soc_property_phys_port_get(unit,
                                           phy_port,
                                           spn_PORT_FLEX_ENABLE, 0)) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

/*
 * Function:
 *      soc_apache_subsidiary_ports_get
 * Purpose:
 *      Given a controlling port, return a set of ancillary ports
 *      belonging to the group (port block) that can be configured to operate
 *      either as a single high-speed port or multiple low speed ports. If the input
 *      port is not a controlling port, SOC_E_PORT error will be returned.
 * Parameters:
 *      unit - (IN) Unit number.
 *      port - (IN) Port
 *      pbmp - (OUT) Set of ancillary ports
 * Returns:
 *      SOC_E_XXX
 */
soc_error_t
soc_apache_subsidiary_ports_get(int unit, soc_port_t port, soc_pbmp_t *pbmp)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_t phy_port_base, phy_port;
    soc_pbmp_t pbm;
    int max_subport;

    if (!SOC_PORT_VALID(unit, port) || IS_CPU_PORT(unit, port) ||
        IS_LB_PORT(unit, port) || IS_RDB_PORT(unit, port)) {
         /* CPU, LB, RDB ports do not support flexport */
        return SOC_E_PORT;
    }

    phy_port = si->port_l2p_mapping[port];
    phy_port_base = PORT_BLOCK_BASE_PORT(port);

    /* Only lane 0 and lane 2 of a PM can be controlling ports */
    if ((phy_port_base != phy_port) && ((phy_port_base + 2) != phy_port)) {
        /* Not a controlling port */
        return SOC_E_PORT;
    }
    max_subport = phy_port + (4 - ((phy_port - 1) % 4));
    SOC_PBMP_CLEAR(pbm);
    for (; phy_port < max_subport; phy_port++) {
        if (si->port_p2l_mapping[phy_port] == -1) {
            continue;
        }
        SOC_PBMP_PORT_ADD(pbm, si->port_p2l_mapping[phy_port]);
    }
    SOC_PBMP_ASSIGN(*pbmp, pbm);

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_apache_port_lane_config_get
 * Purpose:
 *      Get the mode lane configuration for given physical port.
 *      This is only applicable on 100GE ports.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      phy_port  - (IN) Physical port number.
 *      mode_lane - (OUT) Returns the lane distribution for given port
 *                        SOC_LANE_CONFIG_100G_XYZ
 *      found     - (OUT) Returns TRUE if property is specified,
 *                        FALSE if property is not found.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Assumes physical port is valid.
 */
int
soc_apache_port_lane_config_get(int unit, int phy_port, int *mode_lane,
                                int *found)
{
    char *str;

    /*
     * The PHY_MLD_MAP SOC property (Multi Lane Distribution) value
     * is interpreted as follows:
     *
     * Each nibble-index is the TSC lane number (0..11) and
     * the nibble value is the 100GE port lane number.
     * A nibble value of "f" indicates the TSC lane is not used.
     *
     * Supported 100GE lane distributions:
     *   TSC 100g 4-4-2 mode:  phy_mld_map{x} = 0xff9876543210
     *   TSC 100g 3-4-3 mode:  phy_mld_map{x} = 0xf9876543f210
     *   TSC 100g 2-4-4 mode:  phy_mld_map{x} = 0x98765432ff10
     *
     * Default is 4-4-2
     * This matches Apache port init soc_apache_port_config_init().
     */
    str = soc_property_phy_get_str(unit, phy_port, -1, -1, -1,
                                   spn_PHY_MLD_MAP);

    /* Return default if property is not found */
    if (str == NULL) {
        *mode_lane = SOC_LANE_CONFIG_100G_DEFAULT;
        *found = FALSE;
        return SOC_E_NONE;
    }

    *found = TRUE;
    if (!sal_strcasecmp(str, APACHE_100GE_MLD_MAP_4_4_2_STR)) {
        *mode_lane = SOC_LANE_CONFIG_100G_4_4_2;
    } else if (!sal_strcasecmp(str, APACHE_100GE_MLD_MAP_3_4_3_STR)) {
        *mode_lane = SOC_LANE_CONFIG_100G_3_4_3;
    } else if (!sal_strcasecmp(str, APACHE_100GE_MLD_MAP_2_4_4_STR)) {
        *mode_lane = SOC_LANE_CONFIG_100G_2_4_4;
    } else {
        LOG_WARN(BSL_LS_SOC_PORT,
                 (BSL_META_U(unit,
                             "Physical Port %d: "
                             "Invalid lane configuration.\n"
                             "Using default lane mode.\n"),
                  phy_port));
        *mode_lane = SOC_LANE_CONFIG_100G_DEFAULT;
        *found = FALSE;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_apache_port_autoneg_core_get
 * Purpose:
 *      Get the autoneg configuration for given physical port.
 *      This is only applicable on 100GE ports.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      phy_port  - (IN) Physical port number.
 *      an_core   - (OUT) Returns the an_core index for given port
 *      found     - (OUT) Returns TRUE if property is specified,
 *                        FALSE if property is not found.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Assumes physical port is valid.
 */
int
soc_apache_port_autoneg_core_get(int unit, int phy_port, int *an_core,
                                 int *found)
{
    *found = TRUE;
    *an_core = soc_property_phy_get(unit, phy_port, -1, -1, -1,
                                    spn_PHY_AN_CORE_NUM,
                                    -1);
    if (*an_core < 0 || *an_core > 2) {
        *an_core = SOC_LANE_CONFIG_100G_AN_CORE_DEFAULT;
        *found = FALSE;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_ap_speed_valid
 * Purpose:
 *      Check that given speed is valid for the number of
 *      serdes lanes.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      phy_port - (IN) Physical port number.
 *      lanes    - (IN) Number of PHY lanes.
 *      speed    - (IN) Port rate to check.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_ap_speed_valid(int unit, int phy_port, int lanes, int speed)
{
    uint32 speed_mask = 0;
    int speed_max;

    SOC_IF_ERROR_RETURN
        (soc_ap_port_resource_speed_max_get(unit, &speed_max));

    if (speed > speed_max) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Invalid speed configuration for "
                              "physical_port=%d, speed=%d, max=%d\n"),
                   phy_port, speed, speed_max));
        return SOC_E_CONFIG;
    }

    if (lanes == 1) {
        switch (speed) {
            case 10:
            case 100:
                speed_mask = PORT_IS_FALCON(phy_port) ? 0 : AP_PHY_PORT_RATE_1;
                break;
            case 1000:
                speed_mask = AP_PHY_PORT_RATE_1;
                break;
            case 2500:
                speed_mask = PORT_IS_FALCON(phy_port) ? 0 : AP_PHY_PORT_RATE_2_5;
                break;
            case 5000:
                speed_mask = PORT_IS_FALCON(phy_port) ? 0 : AP_PHY_PORT_RATE_5;
                break;
            case 10000:
                speed_mask = AP_PHY_PORT_RATE_10;
                break;
            case 11000:
                speed_mask = AP_PHY_PORT_RATE_11;
                break;
            case 25000:
                speed_mask = PORT_IS_FALCON(phy_port) ? AP_PHY_PORT_RATE_25 : 0;
                break;
            case 27000:
                speed_mask = PORT_IS_FALCON(phy_port) ? AP_PHY_PORT_RATE_27 : 0;
                break;
        }
    } else if (lanes == 2) {
        switch (speed) {
            case 10000:
                speed_mask = PORT_IS_FALCON(phy_port) ? 0 : AP_PHY_PORT_RATE_10;
                break;
            case 11000:
                speed_mask = PORT_IS_FALCON(phy_port) ? 0 : AP_PHY_PORT_RATE_11;
                break;
            case 20000:
                speed_mask = AP_PHY_PORT_RATE_20;
                break;
            case 21000:
                speed_mask = AP_PHY_PORT_RATE_21;
                break;
            case 40000:
                speed_mask = PORT_IS_FALCON(phy_port) ? AP_PHY_PORT_RATE_40 : 0;
                break;
            case 42000:
                speed_mask = PORT_IS_FALCON(phy_port) ? AP_PHY_PORT_RATE_42 : 0;
                break;
            case 50000:
                speed_mask = PORT_IS_FALCON(phy_port) ? AP_PHY_PORT_RATE_50 : 0;
                break;
            case 53000:
                speed_mask = PORT_IS_FALCON(phy_port) ? AP_PHY_PORT_RATE_53 : 0;
                break;
        }
    } else if (lanes == 4) {
        switch (speed) {
            case 10000:
                speed_mask = PORT_IS_FALCON(phy_port) ? 0 : AP_PHY_PORT_RATE_10;
                break;
            case 11000:
                speed_mask = PORT_IS_FALCON(phy_port) ? 0 : AP_PHY_PORT_RATE_11;
                break;
            case 20000:
                speed_mask = PORT_IS_FALCON(phy_port) ? 0 : AP_PHY_PORT_RATE_20;
                break;
            case 21000:
                speed_mask = PORT_IS_FALCON(phy_port) ? 0 : AP_PHY_PORT_RATE_21;
                break;
            case 25000:
                speed_mask = PORT_IS_FALCON(phy_port) ? 0 : AP_PHY_PORT_RATE_25;
                break;
            case 40000:
                speed_mask = AP_PHY_PORT_RATE_40;
                break;
            case 42000:
                speed_mask = AP_PHY_PORT_RATE_42;
                break;
            case 100000:
                speed_mask = PORT_IS_FALCON(phy_port) ? AP_PHY_PORT_RATE_100 : 0;
                break;
            case 106000:
                speed_mask = PORT_IS_FALCON(phy_port) ? AP_PHY_PORT_RATE_106 : 0;
                break;
        }
    }

    if (!(AP_INFO(unit)->speed_valid[lanes] & speed_mask)) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Invalid speed configuration for "
                              "physical_port=%d, speed=%d\n"),
                   phy_port, speed));
        return SOC_E_CONFIG;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_ap_phy_port_lanes_valid
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
_soc_ap_phy_port_lanes_valid(int unit, int phy_port, int lanes)
{
    uint32 lane_mask = 0;

    if (lanes == 1) {
        lane_mask = AP_PHY_XLPORT_LANES_1;
    } else if (lanes == 2) {
        lane_mask = AP_PHY_XLPORT_LANES_2;
    } else if (lanes == 4) {
        lane_mask = AP_PHY_XLPORT_LANES_4;
    } else if (lanes == 10) {
        lane_mask = AP_PHY_XLPORT_LANES_10;
    } else if (lanes == 12) {
        lane_mask = AP_PHY_XLPORT_LANES_12;
    } else {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Invalid number of lanes for "
                              "physical_port=%d, lanes=%d\n"),
                   phy_port, lanes));
        return SOC_E_CONFIG;
    }

    if (!(AP_PHY_PORT(unit, phy_port).lanes_valid & lane_mask)) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Invalid lane configuration for "
                              "physical port %d, lane=%d, "
                              "valid_lanes=%s %s %s  %s %s\n"),
                   phy_port, lanes,
                   (AP_PHY_PORT(unit, phy_port).lanes_valid &
                    AP_PHY_XLPORT_LANES_1) ? "1" : "",
                   (AP_PHY_PORT(unit, phy_port).lanes_valid &
                    AP_PHY_XLPORT_LANES_2) ? "2" : "",
                   (AP_PHY_PORT(unit, phy_port).lanes_valid &
                    AP_PHY_XLPORT_LANES_4) ? "4" : "",
                   (AP_PHY_PORT(unit, phy_port).lanes_valid &
                    AP_PHY_XLPORT_LANES_10) ? "10" : "",
                   (AP_PHY_PORT(unit, phy_port).lanes_valid &
                    AP_PHY_XLPORT_LANES_12) ? "12" : ""));

        return SOC_E_CONFIG;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_ap_port_lanes_valid
 * Purpose:
 *      Check that given lane setting is valid for logical port.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port     - (IN) Logical port.
 *      lanes    - (IN) Number of PHY lanes.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_ap_port_lanes_valid(int unit, soc_port_t port, int lanes)
{
    soc_info_t *si = &SOC_INFO(unit);
    int phy_port;

    AP_INFO_INIT_CHECK(unit);

    phy_port = si->port_l2p_mapping[port];
    if (phy_port == -1) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Invalid physical port for logical port %d\n"),
                   port));
        return SOC_E_PORT;
    }

    return _soc_ap_phy_port_lanes_valid(unit, phy_port, lanes);
}


/*
 * Function:
 *      soc_ap_port_resource_speed_max_get
 * Purpose:
 *      Get the maximum allowed speed in the device.
 *
 *      This information is used to determine the total number of
 *      logical  ports allowed in the system as well as the
 *      pre-allocation of MMU port numbers.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      speed - (OUT) Maximum speed of any port in device.
 * Returns:
 *      SOC_E_XXX
 * Note:
 */
int
soc_ap_port_resource_speed_max_get(int unit, int *speed)
{
    AP_INFO_INIT_CHECK(unit);

    *speed = AP_INFO(unit)->speed_max;

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_ap_port_addressable
 * Purpose:
 *      Check that given logical port number is addressable.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      port  - (IN) Logical port number.
 * Returns:
 *      SOC_E_NONE - If logical port number is addressable
 *      SOC_E_PORT - If logical port number is not addressable.
 * NOTE:
 *      This routine only checks that the logical port number is
 *      within the valid range.  It does NOT check whether the logical
 *      port has a valid port mapping.
 */
int
soc_ap_port_addressable(int unit, soc_port_t port)
{
    AP_INFO_INIT_CHECK(unit);

    if ((port < 0) ||
        (port > AP_INFO(unit)->port_max)) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Invalid logical port number %d. "
                              "Valid logical ports are %d to %d.\n"),
                   port, 0, AP_INFO(unit)->port_max));
        return SOC_E_PORT;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_ap_phy_port_addressable
 * Purpose:
 *      Check that given physical port number is addressable.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      phy_port  - (IN) Physical port number.
 * Returns:
 *      SOC_E_NONE - If physical port number is addressable
 *      SOC_E_PORT - If physical port number is not addressable.
 * NOTE:
 *      This routine only checks that the physical port number is
 *      within the valid range.  It does NOT check whether the physical
 *      port has a port mapping.
 */
int
soc_ap_phy_port_addressable(int unit, int phy_port)
{
    AP_INFO_INIT_CHECK(unit);

    if ((phy_port < 0) ||
        (phy_port >= AP_INFO(unit)->phy_ports_max)) {
        return SOC_E_PORT;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_ap_phy_port_pgw_info_get
 * Purpose:
 *      Provide the PGW information for given physical port.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      phy_port    - (IN) Physical port number.
 *      pgw         - (OUT) PGW instance where port resides.
 *      tsc         - (OUT) TSC number within PGW
 *      port_index  - (OUT) Port index within TSC
 * Returns:
 *      SOC_E_XXX
 */
int
soc_ap_phy_port_pgw_info_get(int unit, int phy_port,
                               int *pgw, int *tsc, int *port_index)
{
    AP_INFO_INIT_CHECK(unit);

    if (phy_port == -1) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Invalid physical port %d\n"),
                   phy_port));
        return SOC_E_PORT;
    }

    *pgw = AP_PHY_PORT_LANE(unit, phy_port).pgw;
    *tsc = AP_PHY_PORT_LANE(unit, phy_port).tsc;
    *port_index = AP_PHY_PORT_LANE(unit, phy_port).port_index;

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_ap_logic_ports_max_validate
 * Purpose:
 *      Check that number of logical ports do not exceed maximum allowed
 *      based on physical port mappings provided in bitmap.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      phy_pbmp - (IN) Bitmap of physical ports.
 *                      A bit set indicates physical port is mapped.
 * Returns:
 *      SOC_E_XXX
 * Note:
 */
STATIC int
_soc_ap_logic_ports_max_validate(int unit, portmod_pbmp_t phy_pbmp)
{
    int phy_port;
    int ports_max;
    int port_count = 0;

    /* Get maximum number of logical ports allowed */
    ports_max = AP_INFO(unit)->port_max;

    /* Check number of logical ports do not exceed maximum allowed */
    for (phy_port = 0; phy_port < SOC_MAX_NUM_PORTS; phy_port++) {
        /* Skip CPU, Loopback */
        if ((phy_port == AP_PHY_PORT_CPU) ||
            (phy_port == AP_PHY_PORT_LB)  ||
             (phy_port == AP_PHY_PORT_RDB)) {
            continue;
        }

        /* Skip physical ports not mapped */
        /* coverity[overrun-local] */
        if (!PORTMOD_PBMP_MEMBER(phy_pbmp, phy_port)) {
            continue;
        }

        port_count++;
    }

    if (port_count > ports_max) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Number of logical ports exceeds "
                              "max allowed: port_coun=%d ports_max=%d\n"),
                   port_count, ports_max));
        return SOC_E_RESOURCE;
    }

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "Number of logical ports exceeds "
                            "max allowed: port_coun=%d ports_max=%d\n"),
                 port_count, ports_max));

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_ap_port_resource_tdm_config_validate
 * Purpose:
 *      Validate that the given port configuration is a
 *      standard configuration supported by the TDM.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 *      phy_pbm  - (IN) Physical port bitmap.  A bit set indicates
 *                      physical port is defined (mapped).
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      - Resource is not NULL.
 *      - Assumes array order is correct
 *        ('delete' operations are first in array).
 *      - Ports are not replicated.
 */
STATIC int
_soc_ap_port_resource_tdm_config_validate(int unit,
                                            int nport,
                                            soc_port_resource_t *resource,
                                            portmod_pbmp_t phy_pbmp)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *pr;
    int xlport_mask[_AP_PGWS_PER_DEV][_AP_TSCS_PER_PGW];
    int xlport_validated[_AP_PGWS_PER_DEV][_AP_TSCS_PER_PGW];
    int pgw;
    int tsc;
    int port_index;
    int phy_port;
    int i;

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- VALIDATE: TDM configuration\n")));

    /* Clear data */
    for (pgw = 0; pgw < _AP_PGWS_PER_DEV; pgw++) {
        for (tsc = 0; tsc < _AP_TSCS_PER_PGW; tsc++) {
            xlport_mask[pgw][tsc] = 0x0;
            xlport_validated[pgw][tsc] = 0;
        }
    }

    /*
     * Determine configuration on each XLPORT (PortMacro or TSC-4)
     *
     * A bit set means physical port is defined (mapped).
     */
    for (phy_port = 0; phy_port < SOC_MAX_NUM_PORTS; phy_port++) {
        pgw = AP_PHY_PORT_LANE(unit, phy_port).pgw;
        if (pgw == -1) {
            continue;
        }

        tsc = AP_PHY_PORT_LANE(unit, phy_port).tsc;
        port_index = AP_PHY_PORT_LANE(unit, phy_port).port_index;

        /* Check if physical port is mapped */
        /* coverity[overrun-local] */
        if (PORTMOD_PBMP_MEMBER(phy_pbmp, phy_port)) {
            xlport_mask[pgw][tsc] |= 1 << port_index;
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit, "Check Port Configuration block\n")));
    for (pgw = 0; pgw < _AP_PGWS_PER_DEV; pgw++) {
        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit, "    PGW_%d:"), pgw));
        for (tsc = 0; tsc < _AP_TSCS_PER_PGW; tsc++) {
            LOG_VERBOSE(BSL_LS_SOC_PORT,
                        (BSL_META_U(unit,
                                    " tsc_%d=0x%x"), tsc,
                         xlport_mask[pgw][tsc]));
        }
        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit, "\n")));
    }

    /*
     * Check configuration only on blocks where ports are being flexed.
     * Assume configuration on other blocks is valid.
     */
    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        if (pr->physical_port == -1) {
            phy_port = si->port_l2p_mapping[pr->logical_port];
        } else {
            phy_port = pr->physical_port;
        }

        pgw = AP_PHY_PORT_LANE(unit, phy_port).pgw;
        tsc = AP_PHY_PORT_LANE(unit, phy_port).tsc;

        if (pgw == -1) {
            LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "Invalid PGW block -1 for "
                                  "physical port %d\n"),
                       phy_port));
            return SOC_E_INTERNAL;
        }

        if (xlport_validated[pgw][tsc]) {
            /* Configuration in block was already validated. skip */
            continue;
        }

        /*
         * Check that configuration is a valid TDM case
         *
         * If the XLPORT/CLPORT block has any physical ports used (mapped),
         * then at least the physical port on Lane 0 MUST be defined (mapped).
         */
        if ((xlport_mask[pgw][tsc] != 0x0) &&
            !(xlport_mask[pgw][tsc] & 0x1)) {
            LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "Invalid configuration on physical "
                                  "ports %d %d %d %d "
                                  "(pgw=%d tsc=%d physical_port_mask=0x%x)\n"),
                       AP_PHY_PORT(unit, phy_port).sisters_ports[0],
                       AP_PHY_PORT(unit, phy_port).sisters_ports[1],
                       AP_PHY_PORT(unit, phy_port).sisters_ports[2],
                       AP_PHY_PORT(unit, phy_port).sisters_ports[3],
                       pgw, tsc, xlport_mask[pgw][tsc]));
            LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "Physical port %d must be defined\n"),
                       AP_PHY_PORT(unit, phy_port).sisters_ports[0]));
            return SOC_E_CONFIG;
        }

        xlport_validated[pgw][tsc] = 1;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_ap_port_mapping_validate
 * Purpose:
 *      Validate:
 *      - Port numbers are available and mappings are not replicated.
 *      - Number of logical port numbers do not exceed max allowed.
 *      - Port configuration mode is valid (based on valid TDM cases).
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      - Resource is not NULL.
 *      - Assumes array order is correct
 *        ('delete' operations are first in array).
 */
STATIC int
_soc_ap_port_mapping_validate(int unit, 
                                int nport,
                                soc_port_resource_t *resource)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *pr;
    char phy_pfmt[SOC_PBMP_FMT_LEN];
    char logic_pfmt[SOC_PBMP_FMT_LEN];
    portmod_pbmp_t phy_pbmp;
    pbmp_t logic_pbmp;
    pbmp_t phy_pbmp_tmp;
    int phy_port;
    int i;
    int lane;
    int num_lanes;

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- VALIDATE: Port mappings\n")));
    /*
     * Determine bitmaps of used/mapped logical ports and
     * physical ports.
     *
     * A bit set indicates port is mapped.
     */
    PORTMOD_PBMP_CLEAR(phy_pbmp);
    SOC_PBMP_CLEAR(logic_pbmp);

    /* Get current port assignment */
    for (phy_port = 0; phy_port < SOC_MAX_NUM_PORTS; phy_port++) {
        /* Skip not-addressable and invalid ports */
        if (!AP_PHY_PORT_ADDRESSABLE(unit, phy_port) ||
            (si->port_p2l_mapping[phy_port] == -1)) {
            continue;
        }

        SOC_PBMP_PORT_ADD(logic_pbmp, si->port_p2l_mapping[phy_port]);
        /* coverity[overrun-local] */
        PORTMOD_PBMP_PORT_ADD(phy_pbmp, phy_port);
    }

    /* First 'delete' mappings */
    for (i = 0, pr = &resource[0];
         (i < nport) && (pr->physical_port == -1);
         i++, pr++) {
        phy_port = si->port_l2p_mapping[pr->logical_port];
        if (phy_port == -1) {
            LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "Logical port %d is not currently mapped\n"),
                       pr->logical_port));
            return SOC_E_CONFIG;
        }
        SOC_PBMP_PORT_REMOVE(logic_pbmp, pr->logical_port);
        PORTMOD_PBMP_PORT_REMOVE(phy_pbmp, phy_port);
    }

    /* Continue with 'add' mappings in rest of array */
    for (; i < nport; i++, pr++) {
        /* Check that port number is 'available' */
        if (SOC_PBMP_MEMBER(logic_pbmp, pr->logical_port) ||
            PORTMOD_PBMP_MEMBER(phy_pbmp, pr->physical_port)) {
            LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "Either Logical port %d or "
                                  "Physical port %d is already mapped\n"),
                       pr->logical_port, pr->physical_port));
            return SOC_E_BUSY;
        }

        SOC_PBMP_PORT_ADD(logic_pbmp, pr->logical_port);
        PORTMOD_PBMP_PORT_ADD(phy_pbmp, pr->physical_port);
    }

    SOC_PBMP_CLEAR(phy_pbmp_tmp);
    PORTMOD_PBMP_OR(phy_pbmp_tmp, phy_pbmp);
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "New port bitmap\n"
                            "    logical  = %s\n"
                            "    physical = %s\n"),
                 SOC_PBMP_FMT(logic_pbmp, logic_pfmt),
                 SOC_PBMP_FMT(phy_pbmp_tmp, phy_pfmt)));

    /* Check maximum number of logical ports allowed */
    SOC_IF_ERROR_RETURN
        (_soc_ap_logic_ports_max_validate(unit, phy_pbmp));


    /*
     * Lanes availability
     *
     * Check that lanes needed by the physical port are not
     * not being used by any other port.
     *
     * Assume physical port numbers are consecutive in device
     * with respect of the lanes.
     */
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- VALIDATE: Port lanes assignment\n")));
    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        /* Skip delete entries */
        if (pr->physical_port == -1) {
            continue;
        }

        /*
         * We need to check all 12 lanes
         * even though only 10 lanes are actually used.
         */
        num_lanes = pr->num_lanes;
        if (num_lanes == 10) {
            num_lanes = 12;
        }

        for (lane = 1; lane < num_lanes; lane++) {
            /* Check lane is not being used */
            if (PORTMOD_PBMP_MEMBER(phy_pbmp, pr->physical_port + lane)) {
                LOG_ERROR(BSL_LS_SOC_PORT,
                          (BSL_META_U(unit,
                                      "Port=%d physical_port=%d, lane on "
                                      "physical port %d is being used\n"),
                                      pr->logical_port, pr->physical_port,
                                      pr->physical_port + lane));
                return SOC_E_BUSY;
            }
        }
    }

    /* Check TDM port configuration */
    SOC_IF_ERROR_RETURN
        (_soc_ap_port_resource_tdm_config_validate(unit,
                                                     nport, resource,
                                                     phy_pbmp));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_apache_port_mode_get
 * Description:
 *      Get port mode by giving first logical port of TSC4
 * Parameters:
 *      unit          - Device number
 *      port          - Logical port
 *      int*          - Port Mode
 *
 * Each TSC4 can be configured into following 5 mode:
 *   Lane number    0    1    2    3
 *   ------------  ---  ---  ---  ---
 *      quad port  10G  10G  10G  10G
 *   tri_012 port  10G  10G  20G   x
 *   tri_023 port  20G   x   10G  10G
 *      dual port  20G   x   20G   x
 *    single port  40G   x    x    x
 */
int
soc_apache_port_mode_get(int unit, int logical_port, int *mode)
{
    soc_info_t *si;
    int lane;
    int port, first_phyport, phy_port;
    int num_lanes[_AP_PORTS_PER_TSC];

    si = &SOC_INFO(unit);
    first_phyport = si->port_l2p_mapping[logical_port];

    for (lane = 0; lane < _AP_PORTS_PER_TSC; lane++) {
        phy_port = first_phyport + lane;
        port = si->port_p2l_mapping[phy_port];
        if (port == -1 || SOC_PBMP_MEMBER(si->all.disabled_bitmap, port)) {
            num_lanes[lane] = -1;
        } else {
            num_lanes[lane] = si->port_num_lanes[port];
        }
    }

    if (num_lanes[0] == 4) {
        *mode = SOC_APACHE_PORT_MODE_QUAD;
    } else if (num_lanes[0] == 2 && num_lanes[2] == 2) {
        *mode = SOC_APACHE_PORT_MODE_DUAL;
    } else if (num_lanes[0] == 2 && num_lanes[2] == 1 && num_lanes[3] == 1) {
        *mode = SOC_APACHE_PORT_MODE_TRI_023;
    } else if (num_lanes[0] == 1 && num_lanes[1] == 1 && num_lanes[2] == 2) {
        *mode = SOC_APACHE_PORT_MODE_TRI_012;
    } else {
        *mode = SOC_APACHE_PORT_MODE_SINGLE;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_ap_port_mode_get
 * Description:
 *      Get port mode by giving first logical port of TSC4
 *      Same as above function but uses post soc info to calculate mode
 * Parameters:
 *      unit          - Device number
 *      logical_port  - Logical port
 *      post_si       - Post Soc info
 *      int*          - Port Mode
 *
 * Each TSC4 can be configured into following 5 mode:
 *   Lane number    0    1    2    3
 *   ------------  ---  ---  ---  ---
 *      quad port  10G  10G  10G  10G
 *   tri_012 port  10G  10G  20G   x
 *   tri_023 port  20G   x   10G  10G
 *      dual port  20G   x   20G   x
 *    single port  40G   x    x    x
 */
STATIC int
_soc_ap_port_mode_get(int unit, int logical_port, soc_info_misc_t *post_si, int *mode)
{
    int lane;
    int port, first_phyport, phy_port;
    int num_lanes[_AP_PORTS_PER_TSC];

    first_phyport = post_si->port_l2p_mapping[logical_port];

    for (lane = 0; lane < _AP_PORTS_PER_TSC; lane++) {
        phy_port = first_phyport + lane;
        port = post_si->port_p2l_mapping[phy_port];
        if (port == -1 || SOC_PBMP_MEMBER(post_si->disabled_bitmap, port)) {
            num_lanes[lane] = -1;
        } else {
            num_lanes[lane] = post_si->port_num_lanes[port];
        }
    }

    if (num_lanes[0] == 4) {
        *mode = SOC_APACHE_PORT_MODE_QUAD;
    } else if (num_lanes[0] == 2 && num_lanes[2] == 2) {
        *mode = SOC_APACHE_PORT_MODE_DUAL;
    } else if (num_lanes[0] == 2 && num_lanes[2] == 1 && num_lanes[3] == 1) {
        *mode = SOC_APACHE_PORT_MODE_TRI_023;
    } else if (num_lanes[0] == 1 && num_lanes[1] == 1 && num_lanes[2] == 2) {
        *mode = SOC_APACHE_PORT_MODE_TRI_012;
    } else {
        *mode = SOC_APACHE_PORT_MODE_SINGLE;
    }

    return SOC_E_NONE;
}

#define APACHE_MAX_OVS_SPG 4
/*
 * Function:
 *      _soc_ap_port_resource_ovs_speed_group_validate
 * Purpose:
 *      Validate oversub speed group of all ports:
 *      Total number of oversub speed groups cannot exceed 4 at any given time
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      post_si  - (IN) Post SOC INFO information
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_ap_port_resource_ovs_speed_group_validate(int unit, soc_info_misc_t *post_si)
{
    int i, j, found;
    int logic_port, phy_port;
    int speed, spd_cls, spg_count = 0;
    int spg_num[APACHE_MAX_OVS_SPG] = {0};
    uint32 sp_spacing = 0;

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- VALIDATE: Oversub Speed groups\n")));

    for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
        logic_port = i;
        phy_port = post_si->port_l2p_mapping[logic_port];

        /* Skip ports not defined, LB, CPU, etc. */
        /* coverity[overrun-local] */
        if (!SOC_PORT_ADDRESSABLE(unit, logic_port) ||
            !((phy_port >= 0) && (phy_port <= SOC_MAX_NUM_PORTS)) ||
            IS_LB_PORT(unit, logic_port) ||
            IS_RDB_PORT(unit, logic_port) ||
            IS_CPU_PORT (unit, logic_port)) {
            continue;
        }

        /* Skip Line Rate ports */
        /* coverity[overrun-local] */
        if (!SOC_PBMP_MEMBER(post_si->oversub_pbm, logic_port)) {
            continue;
        }

        /* Skip inactive ports */
        /* coverity[overrun-local] */
        if (SOC_PBMP_MEMBER(post_si->disabled_bitmap, logic_port)) {
            continue;
        }


        /* Get speed class */
        speed = post_si->port_speed[logic_port];
        SOC_IF_ERROR_RETURN(_soc_apache_mmu_ovs_speed_class_map_get(unit,
                            &speed, &spd_cls, &sp_spacing));
        found = 0;
        for (j = 0; j < APACHE_MAX_OVS_SPG; j++) {
            /* Check if speed group already exists */
            if (spd_cls == spg_num[j]) {
                found = 1;
                break;
            }
        }
        if (!found) {
            if (spg_count == APACHE_MAX_OVS_SPG) {
                LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                          "More than %d oversub speed groups not supported\n"),
                           APACHE_MAX_OVS_SPG));
                return SOC_E_CONFIG;
            }
            /* Keep track of all speed groups */
            spg_num[spg_count++] = spd_cls;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_ap_port_ovs_speed_group_validate
 * Purpose:
 *      Validate oversub speed group of all ports:
 *      Total number of oversub speed groups cannot exceed 4 at any given time
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      post_si  - (IN) Post SOC INFO information
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumes that this function is called at early stage of initialization
 *      (For example, soc_apache_port_config_init).
 *      Because portctrl/portmod API is not ready to use,
 *      use port_speed_max or port_init_speed instead.
 */
int
soc_ap_port_ovs_speed_group_validate(int unit)
{
    int i, j, found;
    int logic_port, phy_port;
    int speed, spd_cls, spg_count = 0;
    int spg_num[APACHE_MAX_OVS_SPG] = {0};
    uint32 sp_spacing = 0;
    soc_info_t *si = &SOC_INFO(unit);

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- VALIDATE: Oversub Speed groups\n")));

    for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
        logic_port = i;
        phy_port = si->port_l2p_mapping[logic_port];

        /* Skip ports not defined, LB, CPU, etc. */
        /* coverity[overrun-local] */
        if (!SOC_PORT_ADDRESSABLE(unit, logic_port) ||
            !((phy_port >= 0) && (phy_port <= SOC_MAX_NUM_PORTS)) ||
            IS_LB_PORT(unit, logic_port) ||
            IS_RDB_PORT(unit, logic_port) ||
            IS_CPU_PORT (unit, logic_port)) {
            continue;
        }

        /* Skip Line Rate ports */
        /* coverity[overrun-local] */
        if (!SOC_PBMP_MEMBER(si->oversub_pbm, logic_port)) {
            continue;
        }

        /* Skip inactive ports */
        /* coverity[overrun-local] */
        if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, logic_port)) {
            continue;
        }


        /* Get speed class */
        speed = si->port_speed_max[logic_port];
        SOC_IF_ERROR_RETURN(_soc_apache_mmu_ovs_speed_class_map_get(unit,
                            &speed, &spd_cls, &sp_spacing));
        found = 0;
        for (j = 0; j < APACHE_MAX_OVS_SPG; j++) {
            /* Check if speed group already exists */
            if (spd_cls == spg_num[j]) {
                found = 1;
                break;
            }
        }
        if (!found) {
            if (spg_count == APACHE_MAX_OVS_SPG) {
                LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                          "More than %d oversub speed groups not supported\n"),
                           APACHE_MAX_OVS_SPG));
                return SOC_E_CONFIG;
            }
            /* Keep track of all speed groups */
            spg_num[spg_count++] = spd_cls;
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_ap_port_resource_mixed_sisters_validate
 * Purpose:
 *      Validate mixed speed in sister ports:
 *      - Mixed-sister line rate ports are allowed.
 *      - Mixed-sister oversub ports are NOT allowed
 *        All active ports in a oversub Port Macro (TSC) must
 *        be the same speed except 10g and 1g on PM4x10
 *      - Tri-Mode on Pm4x25 not supported
 *
 *      Different speed ports in the same Port Macro (TSC) are referred
 *      to as mixed-sister ports.
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 *      post_si  - (IN) Post SOC INFO information
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      - Resource is not NULL.
 *      - Assumes array order is correct
 *        ('delete' operations are first in array).
 *      - Assumes oversub rule has been validated (sister ports must
 *        have same oversubscription mode)
 */
STATIC int
_soc_ap_port_resource_mixed_sisters_validate(int unit,
                                             int nport,
                                             soc_port_resource_t *resource,
                                             soc_info_misc_t *post_si)
{
    soc_port_resource_t *pr;
    int i;
    int j;
    int logic_port;
    int phy_port, num_lanes, encap, mode;
    int sister_port;
    int speed, sister_speed;
    int spd_cls, sister_spd_cls;
    uint32 sp_spacing = 0;
    uint32_t pll_div, sister_pll_div;

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- VALIDATE: Mixed speed sister ports\n")));

     for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        phy_port = pr->physical_port;

        /* Skip delete entries */
        if (phy_port == -1) {
            continue;
        }

        sister_port = AP_PHY_PORT(unit, phy_port).sisters_ports[0];
        /* PM4x25 Tri-Mode not supported */
        if (PORT_IS_FALCON(sister_port)) {
            logic_port = post_si->port_p2l_mapping[sister_port];
            SOC_IF_ERROR_RETURN(_soc_ap_port_mode_get(unit, logic_port, post_si, &mode));

            if ((mode == SOC_APACHE_PORT_MODE_TRI_012) ||
                (mode == SOC_APACHE_PORT_MODE_TRI_023)) {
                LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "Tri-Portmode not supported on PM4x25 physical ports "
                           "%d %d %d %d\n"),
                    AP_PHY_PORT(unit, phy_port).sisters_ports[0],
                    AP_PHY_PORT(unit, phy_port).sisters_ports[1],
                    AP_PHY_PORT(unit, phy_port).sisters_ports[2],
                    AP_PHY_PORT(unit, phy_port).sisters_ports[3]));
                return SOC_E_CONFIG;
            }
        }

        speed = pr->speed;
        num_lanes = pr->num_lanes;
        encap = pr->encap;
        logic_port = pr->logical_port;

        /*
         * Check that all sister ports have same VCO(PLL div) value.
         */
        SOC_IF_ERROR_RETURN(
            soc_esw_portctrl_pll_div_get(unit, logic_port, phy_port, speed, num_lanes,
                                         encap, &pll_div));

        for (j = 0; j < _AP_PORTS_PER_TSC; j++) {
            sister_port = AP_PHY_PORT(unit, phy_port).sisters_ports[j];
            logic_port = post_si->port_p2l_mapping[sister_port];

            if (logic_port < 0) {
                continue;  /* Skip if port is not defined */
            }
            if (SOC_PBMP_MEMBER(post_si->disabled_bitmap, logic_port)) {
                continue;  /* Skip inactive ports */
            }

            num_lanes = post_si->port_num_lanes[logic_port];
            encap = post_si->port_encap[logic_port];
            sister_speed = post_si->port_speed[logic_port];
            SOC_IF_ERROR_RETURN(
                soc_esw_portctrl_pll_div_get(unit, logic_port, sister_port, sister_speed, num_lanes,
                                             encap, &sister_pll_div));
            if (pll_div != sister_pll_div) {
                LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "TSC PLL divider requirements for all physical ports in a PM must be same "
                        "%d %d %d %d\n"),
                    AP_PHY_PORT(unit, phy_port).sisters_ports[0],
                    AP_PHY_PORT(unit, phy_port).sisters_ports[1],
                    AP_PHY_PORT(unit, phy_port).sisters_ports[2],
                    AP_PHY_PORT(unit, phy_port).sisters_ports[3]));
                LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "Pll div mismatch: \n"
                    "  logical_port=%d pll_div=%d\n"
                    "  logical_port=%d sister pll_div=%d\n"),
                    pr->logical_port, pll_div,
                    logic_port, sister_pll_div));
                return SOC_E_CONFIG;
            }
        }

        /* Skip Line Rate ports */
        if (!SOC_PBMP_MEMBER(post_si->oversub_pbm, pr->logical_port)) {
            continue;
        }

        /*
         * Check that Oversub sister ports (ports within a Port Macro)
         * have the same speed class
         */
        speed = pr->speed;
        SOC_IF_ERROR_RETURN(_soc_apache_mmu_ovs_speed_class_map_get(unit,
                                &speed, &spd_cls, &sp_spacing));

        for (j = 0; j < _AP_PORTS_PER_TSC; j++) {
            sister_port = AP_PHY_PORT(unit, phy_port).sisters_ports[j];
            logic_port = post_si->port_p2l_mapping[sister_port];

            if (logic_port < 0) {
                continue;  /* Skip if port is not defined */
            }
            if (SOC_PBMP_MEMBER(post_si->disabled_bitmap, logic_port)) {
                continue;  /* Skip inactive ports */
            }

            sister_speed = post_si->port_speed[logic_port];
            if (sister_speed <= 0) {
                continue;
            }

            SOC_IF_ERROR_RETURN(_soc_apache_mmu_ovs_speed_class_map_get(unit,
                                     &sister_speed, &sister_spd_cls, &sp_spacing));

            /* For FB5, even though <=10G speeds belong in different speed class
             * <=5g & 10g mixed speed is supported on PM4x10 and PM4x25
             */
            if (((speed == 10000) && (post_si->port_speed[logic_port] <= 5000)) ||
                ((pr->speed <= 5000) && (sister_speed == 10000))) {
                continue;
            }

            if (spd_cls != sister_spd_cls)
            {
                LOG_ERROR(BSL_LS_SOC_PORT,
                            (BSL_META_U(unit,
                                  "Speed class must be the same on "
                                  "oversub sister physical ports "
                                  "%d %d %d %d\n"),
                            AP_PHY_PORT(unit, phy_port).sisters_ports[0],
                            AP_PHY_PORT(unit, phy_port).sisters_ports[1],
                            AP_PHY_PORT(unit, phy_port).sisters_ports[2],
                            AP_PHY_PORT(unit, phy_port).sisters_ports[3]));
                LOG_ERROR(BSL_LS_SOC_PORT,
                            (BSL_META_U(unit,
                                  "Speed class mismatch: \n"
                                  "  logical_port=%d physical_port=%d "
                                  "speed=%d\n"
                                  "  logical_port=%d physical_port=%d "
                                  "speed=%d\n"),
                            pr->logical_port, pr->physical_port, speed,
                            logic_port, sister_port, sister_speed));
                return SOC_E_CONFIG;
            }
        }
     }

     return SOC_E_NONE;
}

/*
 * Function:
 *      soc_ap_port_mixed_sister_speed_validate
 * Purpose:
 *      Check all sister ports has same speed in oversub mode except 10g and 1g on PM4x10
 *      Tri-Mode on Pm4x25 not allowed
 * Parameters:
 *      unit     - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumes that this function is called at early stage of initialization
 *      (For example, soc_apache_port_config_init).
 *      Because portctrl/portmod API is not ready to use,
 *      use port_speed_max or port_init_speed instead.
 */
int
soc_ap_port_mixed_sister_speed_validate(int unit)
{
    int i;
    int lport, sister_lport, phy_port, sister_port;
    int speed, sister_speed;
    int spd_cls, sister_spd_cls, mode;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 sp_spacing = 0;

    /* Loop for 1st physical port in PortMacro */
    for (phy_port = 1; phy_port < SOC_MAX_NUM_PORTS; phy_port += _AP_PORTS_PER_TSC) {

        lport = si->port_p2l_mapping[phy_port];
        if (lport < 0) {
            continue;
        }

        sister_port = AP_PHY_PORT(unit, phy_port).sisters_ports[0];
        /* PM4x25 Tri-Mode not supported */
        if (PORT_IS_FALCON(sister_port)) {
            lport = si->port_p2l_mapping[sister_port];
            SOC_IF_ERROR_RETURN(soc_apache_port_mode_get(unit, lport, &mode));

            if ((mode == SOC_APACHE_PORT_MODE_TRI_012) ||
                (mode == SOC_APACHE_PORT_MODE_TRI_023)) {
                LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "Tri-Portmode not supported on PM4x25 physical ports "
                           "%d %d %d %d\n"),
                    AP_PHY_PORT(unit, phy_port).sisters_ports[0],
                    AP_PHY_PORT(unit, phy_port).sisters_ports[1],
                    AP_PHY_PORT(unit, phy_port).sisters_ports[2],
                    AP_PHY_PORT(unit, phy_port).sisters_ports[3]));
                return SOC_E_CONFIG;
            }
        }

        lport = si->port_p2l_mapping[phy_port];
        /* Skip Line Rate ports
           Assumes mixed oversub mode is already checked */
        if (!SOC_PBMP_MEMBER(si->oversub_pbm, lport)) {
            continue;
        }

        /*
         * Check that Oversub sister ports (ports within a Port Macro)
         * have the same speed class
         */
        speed = si->port_speed_max[lport];

        SOC_IF_ERROR_RETURN(_soc_apache_mmu_ovs_speed_class_map_get(unit,
                                &speed, &spd_cls, &sp_spacing));

        for (i = 1; i < _AP_PORTS_PER_TSC; i++) {
            sister_port = AP_PHY_PORT(unit, phy_port).sisters_ports[i];
            sister_lport = si->port_p2l_mapping[sister_port];
            if (sister_lport < 0) {
                continue;
            }

            if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, sister_lport)) {
                continue;
            }

            sister_speed = si->port_speed_max[sister_lport];

            SOC_IF_ERROR_RETURN(_soc_apache_mmu_ovs_speed_class_map_get(unit,
                                    &sister_speed, &sister_spd_cls, &sp_spacing));

            /* For FB5, even though <=10G speeds belong in different speed class
             * <=5g & 10g mixed speed is supported on PM4x10 and PM4x25
             */
            if (((speed == 10000) && (si->port_speed_max[sister_lport] <= 5000)) ||
                ((si->port_speed_max[lport] <= 5000) && (sister_speed == 10000))) {
                continue;
            }

            /* For MV, even though <=10G speeds belong in same speed class
             * <=5g & 10g mixed speed is supported only in TSCe PM4x10
             */
            if (spd_cls != sister_spd_cls)
            {
                LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                          "Mixed port speed is not acceptable in oversub mode "
                          "on physical ports %d %d %d %d\n"),
                           AP_PHY_PORT(unit, phy_port).sisters_ports[0],
                           AP_PHY_PORT(unit, phy_port).sisters_ports[1],
                           AP_PHY_PORT(unit, phy_port).sisters_ports[2],
                           AP_PHY_PORT(unit, phy_port).sisters_ports[3]));
                return SOC_E_CONFIG;
            }
        }
    }

    return SOC_E_NONE;
}

int
_soc_ap_port_resource_bandwidth_check(int unit, soc_info_misc_t *post_si)
{
    int rv = SOC_E_NONE;
    int oversub_mode;
    int port_bandwidth;
    int *blk_port = NULL;
    int *blk_port_bandwidth = NULL;
    int index, port, phy_port, lane_count, blk, pgw;
    int is_fb5_device = 0;
    int speed_lanes_1 = 0, speed_lanes_2 = 0;
    int blk_linerate_bandwidth, blk_oversub_bandwidth;
    int pgw_port[_AP_PORTS_PER_PGW];
    int pgw_port_bandwidth[_AP_PORTS_PER_PGW];
    int pgw_linerate_bandwidth[_AP_PGWS_PER_DEV], pgw_oversub_bandwidth[_AP_PGWS_PER_DEV];
    int max_pgw_core_bandwidth[_AP_PGWS_PER_DEV];
    int max_pgw_oversub_bandwidth[_AP_PGWS_PER_DEV];
    int os_ratio[_AP_PGWS_PER_DEV];
    /* MAX OS ratio for a few FB5 configs is > 1.5 but is limited only to a single PGW */
    int max_os_ratio = 1750, def_os_ratio = 1500;
    uint32 blk_configured_map, blk_inactive_map, blk_oversub_map;
    int falcon_lr_bandwidth = 0, falcon_os_bandwidth = 0, *falcon_bw_ptr = 0;

    for (pgw = 0; pgw < _AP_PGWS_PER_DEV; pgw++) {
        max_pgw_core_bandwidth[pgw] = (SOC_INFO(unit).bandwidth) / _AP_PGWS_PER_DEV;
        max_pgw_oversub_bandwidth[pgw] = (SOC_INFO(unit).io_bandwidth) / _AP_PGWS_PER_DEV;

        /* OVSB TDM per slot bandwidth granularity is 10G
         * 56068 SKU has 2x1G ports which will be passed as 2x10g to algo-TDM
         * We don't care even if io_bandwidth overshoots
         * as long as TDM calendar allocates only 1g worth of slots.
         */
        max_pgw_oversub_bandwidth[pgw] += (SOC_INFO(unit).frequency == 841 ? 10000 : 0);
    }

    oversub_mode = soc_property_get(unit, spn_OVERSUBSCRIBE_MODE, 0);

    /* Bandwidth check for ports in each XLPORT/CLPORT block */
    for (pgw = 0; pgw < _AP_PGWS_PER_DEV; pgw++) {
        pgw_linerate_bandwidth[pgw] = 0;
        pgw_oversub_bandwidth[pgw] = 0;

        /* Prepare per PGW local variables for ease of processing */
        for (index = 0; index < _AP_PORTS_PER_PGW; index++) {
            phy_port = 1 + pgw * _AP_PORTS_PER_PGW + index;
            port = post_si->port_p2l_mapping[phy_port];
            pgw_port[index] = port;
            if (port == -1) {
                pgw_port_bandwidth[index] = 0;
            } else {
                pgw_port_bandwidth[index] = post_si->port_speed_max[port];
                APACHE_TDM_SPEED_ADJUST(unit, port, pgw_port_bandwidth[index],
                                        SOC_PBMP_MEMBER(post_si->oversub_pbm, port));
            }
        }

        for (blk = 0; blk < _AP_TSCS_PER_PGW; blk++) {

            blk_port = &pgw_port[blk * _AP_PORTS_PER_TSC];
            blk_port_bandwidth = &pgw_port_bandwidth[blk * _AP_PORTS_PER_TSC];

            blk_configured_map = 0;
            for (index = 0; index < _AP_PORTS_PER_TSC; index++) {
                if (blk_port[index] != -1) {
                    blk_configured_map |= 1 << index;
                }
            }
            if (blk_configured_map == 0) {
                continue;
            }

            /* Normal port block handling */
            blk_inactive_map = 0;
            blk_oversub_map = 0;
            blk_linerate_bandwidth = 0;
            blk_oversub_bandwidth = 0;
            for (index = 0; index < _AP_PORTS_PER_TSC; index++) {
                port = blk_port[index];
                phy_port = post_si->port_l2p_mapping[port];
                if (port == -1) {
                    continue;
                }
                port_bandwidth = blk_port_bandwidth[index];
                if (SOC_PBMP_MEMBER(post_si->oversub_pbm, port)) {
                    blk_oversub_map |= 1 << index;
                }
                if (SOC_PBMP_MEMBER(post_si->disabled_bitmap, port)) {
                    blk_inactive_map |= 1 << index;
                } else {
                    if (blk_oversub_map & (1 << index)) {
                        falcon_bw_ptr = &falcon_os_bandwidth;
                        blk_oversub_bandwidth += port_bandwidth;
                    } else {
                        falcon_bw_ptr = &falcon_lr_bandwidth;
                        blk_linerate_bandwidth += port_bandwidth;
                    }
                }

                /* 0-3 Blocks are PM4x10 and 4-6 are PM12x10 */
                if (blk >= 0 && blk <= 6) {
                    if ((port_bandwidth > 106000)  ||
                        (port_bandwidth > 42000 && (blk != 4))  ||
                        (port_bandwidth > 21000 && (index != 0)) ||
                        (port_bandwidth > 11000 && (index & 1))) {

                        LOG_CLI((BSL_META_U(unit,
                                            "Port %d: Physical port %d can not "
                                            "be configured to %d Gb\n"),
                                 port, phy_port, (port_bandwidth / 1000)));
                        rv = SOC_E_FAIL;
                    }

                    speed_lanes_1 = 11000;
                    speed_lanes_2 = 21000;
               }

                /* 7-8 Blocks are PM4x25 */
                if (blk >= 7 && blk <= 8) {
                    if ((port_bandwidth > 106000) ||
                        (port_bandwidth > 53000 && (index != 0)) ||
                        (port_bandwidth > 27000 && (index & 1))) {

                        LOG_CLI((BSL_META_U(unit,
                                            "Port %d: Physical port %d can not "
                                            "be configured to %d Gb\n"),
                                 port, phy_port, (port_bandwidth / 1000)));
                        rv = SOC_E_FAIL;
                    }

                    speed_lanes_1 = 27000;
                    speed_lanes_2 = 53000;

                    *falcon_bw_ptr += port_bandwidth;
                }
            }

            /* Check if any of the lanes used by first port of block is
             * not used by other port */
            port_bandwidth = blk_port_bandwidth[0];
            if (port_bandwidth > speed_lanes_2) {
                lane_count = 4;
            } else if (port_bandwidth > speed_lanes_1) {
                lane_count = 2;
            } else {
                lane_count = 1;
            }

            for (index = 1; index < lane_count; index++) {
                port = blk_port[index];
                if (port == -1 || SOC_PBMP_MEMBER(post_si->disabled_bitmap, port)) {
                    continue;
                }
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d bandwidth %d Gb and "
                                    "port %d can not be both configured\n"),
                         blk_port[0], port_bandwidth / 1000, port));
                rv = SOC_E_FAIL;
            }

            /* Check if any of the lanes used by third port of block is
             * not used by other port */
            port_bandwidth = blk_port_bandwidth[2];
            if (port_bandwidth > speed_lanes_1) {
                port = blk_port[3];
                if (port != -1 && !SOC_PBMP_MEMBER(post_si->disabled_bitmap, port)) {
                    LOG_CLI((BSL_META_U(unit,
                                        "Port %d bandwidth %d Gb and port %d "
                                        "can not be both configured\n"),
                             blk_port[2], port_bandwidth / 1000, port));
                    rv = SOC_E_FAIL;
                }
                if (blk_port[0] == -1) {
                    LOG_CLI((BSL_META_U(unit,
                               "Physical port %d needs to be "
                               "configured in order to work with "
                               "port %d\n"),
                               1 + pgw * _AP_PORTS_PER_PGW +
                               blk * _AP_PORTS_PER_TSC,
                               blk_port[2]));
                    rv = SOC_E_FAIL;
                }
            }

            if (blk_inactive_map == blk_configured_map) {
                LOG_CLI((BSL_META_U(unit, "Flex port")));
                for (index = 0; index < _AP_PORTS_PER_TSC; index++) {
                    if (blk_port[index] != -1) {
                        LOG_CLI((BSL_META_U(unit, " %d"), blk_port[index]));
                    }
                }
                LOG_CLI((BSL_META_U(unit, ": Can not be all inactive\n")));
                rv = SOC_E_FAIL;
                continue;
            }

            if (blk_inactive_map != 0) {
                if (blk_oversub_map != 0 &&
                    blk_oversub_map != blk_configured_map) {
                    LOG_CLI((BSL_META_U(unit, "Flex port")));
                    for (index = 0; index < _AP_PORTS_PER_TSC;
                         index++) {
                        if (blk_port[index] != -1) {
                            LOG_CLI((BSL_META_U(unit, " %d"), blk_port[index]));
                        }
                    }
                    LOG_CLI((BSL_META_U(unit,
                                        ": Can not be configured to "
                                        "different oversubscription "
                                        "mode within a port block\n")));
                    rv = SOC_E_FAIL;
                    continue;
                }
            }

            if (falcon_lr_bandwidth != 0 && falcon_os_bandwidth != 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "TDM algorith does not support mixed LR and OS ports "
                                    "modes on PM4x25 ports. LR BW = %d Gbps & OS BW = %d Gbps\n"),
                         falcon_lr_bandwidth / 1000, falcon_os_bandwidth / 1000));
                rv = SOC_E_FAIL;
            }


            if (blk_linerate_bandwidth != 0 && blk_oversub_map != 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "TSC %d cannot have mixed LR and OS ports "
                                    "LR Bandwidth = %d Gbps & OS Bandwidth = %d Gbps\n"),
                         blk, blk_linerate_bandwidth / 1000, blk_oversub_bandwidth / 1000));
                rv = SOC_E_FAIL;
            }

            pgw_linerate_bandwidth[pgw] += blk_linerate_bandwidth;
            pgw_oversub_bandwidth[pgw] += blk_oversub_bandwidth;
        } /* for (blk = 0; blk < _AP_TSCS_PER_PGW; blk++) */

        if ((oversub_mode == 1) && (pgw_linerate_bandwidth[pgw] != 0)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit, "Oversub mode: Linerate BW for PGW %d is not 0.\n"), pgw));
            rv = SOC_E_FAIL;
        } else if ((oversub_mode == 0) && (pgw_oversub_bandwidth[pgw] != 0)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit, "Linerate mode: Oversub BW for PGW %d is not 0.\n"), pgw));
            rv = SOC_E_FAIL;
        } else if (oversub_mode == 2) {
            /*
             * OS ratio = (IO BW - LR BW) / (MAX LR BW - IO LR BW)
             */
            os_ratio[pgw] = (((pgw_oversub_bandwidth[pgw]) * 1000) / (max_pgw_core_bandwidth[pgw] - pgw_linerate_bandwidth[pgw]));
        }

        if (soc_feature(unit, soc_feature_untethered_otp)) {
            if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureFamilyFirebolt5)) {
                is_fb5_device = 1;
            }
        }

        /*
         * On some FB5 SKUs, there will be a few MGIG ports for which the estimated
         * effective bandwidth will be 10G for each port in theory.
         * This would make the bandwidth checks below to fail.
         * It is expected that the users do not run these ports beyond 1G or
         * whatever is constrained by the HW Datasheet. These checks are relaxed for FB5 configs.
         */
        if ((oversub_mode != 2) && !is_fb5_device){
            /* Linerate Bandwidth check */
            if (pgw_linerate_bandwidth[pgw] > max_pgw_core_bandwidth[pgw]) {
                LOG_CLI((BSL_META_U(unit,
                                    "PGW_CL%d total line rate bandwidth "
                                    "(%d Gb) exceeds %d Gb\n"),
                        pgw, (pgw_linerate_bandwidth[pgw] / 1000),
                        (max_pgw_core_bandwidth[pgw] / 1000)));
                rv = SOC_E_FAIL;
            }

            /* Oversub Bandwidth check */
            if ((pgw_linerate_bandwidth[pgw] + pgw_oversub_bandwidth[pgw]) > max_pgw_oversub_bandwidth[pgw]) {
                LOG_CLI((BSL_META_U(unit,
                                    "PGW_CL%d total oversub bandwidth "
                                    "(%d Gb) exceeds %d Gb\n"),
                        pgw, ((pgw_linerate_bandwidth[pgw] + pgw_oversub_bandwidth[pgw]) / 1000),
                        (max_pgw_oversub_bandwidth[pgw] / 1000)));
                rv = SOC_E_FAIL;
            }
        }
    }

    if (oversub_mode == 2) {
        uint16 dev_id;
        uint8  rev_id;

        soc_cm_get_id(unit, &dev_id, &rev_id);

        /* For mixed-mode we can have asymmetrical bandwidth distribution
         * Check the OS ratio in this case
         */
        def_os_ratio = dev_id == BCM56565_DEVICE_ID ? 1300 : def_os_ratio;
        if ((os_ratio[0] > def_os_ratio) && (os_ratio[0] < max_os_ratio)) {
            if ((os_ratio[1] > def_os_ratio) ||
                (os_ratio[1] > 1000 && dev_id == BCM56565_DEVICE_ID)) {
                LOG_CLI((BSL_META_U(unit,
                                    "PGW_CL1 total oversub bandwidth "
                                    "(%d Gb) exceeds limit\n"),
                                    ((pgw_oversub_bandwidth[1]) / 1000)));
                rv = SOC_E_FAIL;
            }
        } else if ((os_ratio[1] > def_os_ratio) && (os_ratio[1] < max_os_ratio)) {
            if ((os_ratio[0] > def_os_ratio) ||
                (os_ratio[0] > 1000 && dev_id == BCM56565_DEVICE_ID)) {
                LOG_CLI((BSL_META_U(unit,
                                    "PGW_CL0 total oversub bandwidth "
                                    "(%d Gb) exceeds limit\n"),
                                    ((pgw_oversub_bandwidth[0]) / 1000)));
                rv = SOC_E_FAIL;
            }
        }
    }

    return rv;
}

int
soc_apache_port_config_bandwidth_check(int unit)
{
    int rv = SOC_E_NONE;
    int oversub_mode;
    int port_bandwidth;
    int *blk_port = NULL;
    int *blk_port_bandwidth = NULL;
    int index, port, phy_port, lane_count, blk, pgw;
    int is_fb5_device = 0;
    int speed_lanes_1 = 0, speed_lanes_2 = 0;
    int blk_linerate_bandwidth, blk_oversub_bandwidth;
    int pgw_port[_AP_PORTS_PER_PGW];
    int pgw_port_bandwidth[_AP_PORTS_PER_PGW];
    int pgw_linerate_bandwidth[_AP_PGWS_PER_DEV], pgw_oversub_bandwidth[_AP_PGWS_PER_DEV];
    int max_pgw_core_bandwidth[_AP_PGWS_PER_DEV];
    int max_pgw_oversub_bandwidth[_AP_PGWS_PER_DEV];
    int os_ratio[_AP_PGWS_PER_DEV];
    /* MAX OS ratio for a few FB5 configs is > 1.5 but is limited only to a single PGW */
    int max_os_ratio = 1750, def_os_ratio = 1500;
    uint32 blk_configured_map, blk_inactive_map, blk_oversub_map;
    int falcon_lr_bandwidth = 0, falcon_os_bandwidth = 0, *falcon_bw_ptr = 0;

    for (pgw = 0; pgw < _AP_PGWS_PER_DEV; pgw++) {
        max_pgw_core_bandwidth[pgw] = (SOC_INFO(unit).bandwidth) / _AP_PGWS_PER_DEV;
        max_pgw_oversub_bandwidth[pgw] = (SOC_INFO(unit).io_bandwidth) / _AP_PGWS_PER_DEV;

        /* OVSB TDM per slot bandwidth granularity is 10G
         * 56068 SKU has 2x1G ports which will be passed as 2x10g to algo-TDM
         * We don't care even if io_bandwidth overshoots
         * as long as TDM calendar allocates only 1g worth of slots.
         */
        max_pgw_oversub_bandwidth[pgw] += (SOC_INFO(unit).frequency == 841 ? 10000 : 0);
    }

    oversub_mode = soc_property_get(unit, spn_OVERSUBSCRIBE_MODE, 0);

    /* Bandwidth check for ports in each XLPORT/CLPORT block */
    for (pgw = 0; pgw < _AP_PGWS_PER_DEV; pgw++) {
        pgw_linerate_bandwidth[pgw] = 0;
        pgw_oversub_bandwidth[pgw] = 0;

        /* Prepare per PGW local variables for ease of processing */
        for (index = 0; index < _AP_PORTS_PER_PGW; index++) {
            phy_port = 1 + pgw * _AP_PORTS_PER_PGW + index;
            port = SOC_INFO(unit).port_p2l_mapping[phy_port];
            pgw_port[index] = port;
            if (port == -1) {
                pgw_port_bandwidth[index] = 0;
            } else {
                pgw_port_bandwidth[index] = SOC_INFO(unit).port_speed_max[port];
                APACHE_TDM_SPEED_ADJUST(unit, port, pgw_port_bandwidth[index],
                                        SOC_PBMP_MEMBER(SOC_INFO(unit).oversub_pbm, port));
            }
        }

        for (blk = 0; blk < _AP_TSCS_PER_PGW; blk++) {

            blk_port = &pgw_port[blk * _AP_PORTS_PER_TSC];
            blk_port_bandwidth = &pgw_port_bandwidth[blk * _AP_PORTS_PER_TSC];

            blk_configured_map = 0;
            for (index = 0; index < _AP_PORTS_PER_TSC; index++) {
                if (blk_port[index] != -1) {
                    blk_configured_map |= 1 << index;
                }
            }
            if (blk_configured_map == 0) {
                continue;
            }

            /* Normal port block handling */
            blk_inactive_map = 0;
            blk_oversub_map = 0;
            blk_linerate_bandwidth = 0;
            blk_oversub_bandwidth = 0;
            for (index = 0; index < _AP_PORTS_PER_TSC; index++) {
                port = blk_port[index];
                phy_port = SOC_INFO(unit).port_l2p_mapping[port];
                if (port == -1) {
                    continue;
                }
                port_bandwidth = blk_port_bandwidth[index];
                if (SOC_PBMP_MEMBER(SOC_INFO(unit).oversub_pbm, port)) {
                    blk_oversub_map |= 1 << index;
                }
                if (SOC_PBMP_MEMBER(SOC_INFO(unit).all.disabled_bitmap, port)) {
                    blk_inactive_map |= 1 << index;
                } else {
                    if (blk_oversub_map & (1 << index)) {
                        falcon_bw_ptr = &falcon_os_bandwidth;
                        blk_oversub_bandwidth += port_bandwidth;
                    } else {
                        falcon_bw_ptr = &falcon_lr_bandwidth;
                        blk_linerate_bandwidth += port_bandwidth;
                    }
                }

                /* 0-3 Blocks are PM4x10 and 4-6 are PM12x10 */
                if (blk >= 0 && blk <= 6) {
                    if ((port_bandwidth > 106000)  ||
                        (port_bandwidth > 42000 && (blk != 4))  ||
                        (port_bandwidth > 21000 && (index != 0)) ||
                        (port_bandwidth > 11000 && (index & 1))) {

                        LOG_CLI((BSL_META_U(unit,
                                            "Port %d: Physical port %d can not "
                                            "be configured to %d Gb\n"),
                                 port, phy_port, (port_bandwidth / 1000)));
                        rv = SOC_E_FAIL;
                    }

                    speed_lanes_1 = 11000;
                    speed_lanes_2 = 21000;
               }

                /* 7-8 Blocks are PM4x25 */
                if (blk >= 7 && blk <= 8) {
                    if ((port_bandwidth > 106000) ||
                        (port_bandwidth > 53000 && (index != 0)) ||
                        (port_bandwidth > 27000 && (index & 1))) {

                        LOG_CLI((BSL_META_U(unit,
                                            "Port %d: Physical port %d can not "
                                            "be configured to %d Gb\n"),
                                 port, phy_port, (port_bandwidth / 1000)));
                        rv = SOC_E_FAIL;
                    }

                    speed_lanes_1 = 27000;
                    speed_lanes_2 = 53000;

                    *falcon_bw_ptr += port_bandwidth;
                }
            }

            /* Check if any of the lanes used by first port of block is
             * not used by other port */
            port_bandwidth = blk_port_bandwidth[0];
            if (port_bandwidth > speed_lanes_2) {
                lane_count = 4;
            } else if (port_bandwidth > speed_lanes_1) {
                lane_count = 2;
            } else {
                lane_count = 1;
            }

            for (index = 1; index < lane_count; index++) {
                port = blk_port[index];
                if (port == -1 || SOC_PBMP_MEMBER(SOC_INFO(unit).all.disabled_bitmap, port)) {
                    continue;
                }
                LOG_CLI((BSL_META_U(unit,
                                    "Port %d bandwidth %d Gb and "
                                    "port %d can not be both configured\n"),
                         blk_port[0], port_bandwidth / 1000, port));
                rv = SOC_E_FAIL;
            }

            /* Check if any of the lanes used by third port of block is
             * not used by other port */
            port_bandwidth = blk_port_bandwidth[2];
            if (port_bandwidth > speed_lanes_1) {
                port = blk_port[3];
                if (port != -1 && !SOC_PBMP_MEMBER(SOC_INFO(unit).all.disabled_bitmap, port)) {
                    LOG_CLI((BSL_META_U(unit,
                                        "Port %d bandwidth %d Gb and port %d "
                                        "can not be both configured\n"),
                             blk_port[2], port_bandwidth / 1000, port));
                    rv = SOC_E_FAIL;
                }
                if (blk_port[0] == -1) {
                    LOG_CLI((BSL_META_U(unit,
                               "Physical port %d needs to be "
                               "configured in order to work with "
                               "port %d\n"),
                               1 + pgw * _AP_PORTS_PER_PGW +
                               blk * _AP_PORTS_PER_TSC,
                               blk_port[2]));
                    rv = SOC_E_FAIL;
                }
            }

            if (blk_inactive_map == blk_configured_map) {
                LOG_CLI((BSL_META_U(unit, "Flex port")));
                for (index = 0; index < _AP_PORTS_PER_TSC; index++) {
                    if (blk_port[index] != -1) {
                        LOG_CLI((BSL_META_U(unit, " %d"), blk_port[index]));
                    }
                }
                LOG_CLI((BSL_META_U(unit, ": Can not be all inactive\n")));
                rv = SOC_E_FAIL;
                continue;
            }

            if (blk_inactive_map != 0) {
                if (blk_oversub_map != 0 &&
                    blk_oversub_map != blk_configured_map) {
                    LOG_CLI((BSL_META_U(unit, "Flex port")));
                    for (index = 0; index < _AP_PORTS_PER_TSC;
                         index++) {
                        if (blk_port[index] != -1) {
                            LOG_CLI((BSL_META_U(unit, " %d"), blk_port[index]));
                        }
                    }
                    LOG_CLI((BSL_META_U(unit,
                                        ": Can not be configured to "
                                        "different oversubscription "
                                        "mode within a port block\n")));
                    rv = SOC_E_FAIL;
                    continue;
                }
            }

            if (falcon_lr_bandwidth != 0 && falcon_os_bandwidth != 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "TDM algorith does not support mixed LR and OS ports "
                                    "modes on PM4x25 ports. LR BW = %d Gbps & OS BW = %d Gbps\n"),
                         falcon_lr_bandwidth / 1000, falcon_os_bandwidth / 1000));
                rv = SOC_E_FAIL;
            }


            if (blk_linerate_bandwidth != 0 && blk_oversub_map != 0) {
                LOG_CLI((BSL_META_U(unit,
                                    "TSC %d cannot have mixed LR and OS ports "
                                    "LR Bandwidth = %d Gbps & OS Bandwidth = %d Gbps\n"),
                         blk, blk_linerate_bandwidth / 1000, blk_oversub_bandwidth / 1000));
                rv = SOC_E_FAIL;
            }

            pgw_linerate_bandwidth[pgw] += blk_linerate_bandwidth;
            pgw_oversub_bandwidth[pgw] += blk_oversub_bandwidth;
        } /* for (blk = 0; blk < _AP_TSCS_PER_PGW; blk++) */

        if ((oversub_mode == 1) && (pgw_linerate_bandwidth[pgw] != 0)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit, "Oversub mode: Linerate BW for PGW %d is not 0.\n"), pgw));
            rv = SOC_E_FAIL;
        } else if ((oversub_mode == 0) && (pgw_oversub_bandwidth[pgw] != 0)) {
            LOG_ERROR(BSL_LS_SOC_COMMON,
                      (BSL_META_U(unit, "Linerate mode: Oversub BW for PGW %d is not 0.\n"), pgw));
            rv = SOC_E_FAIL;
        } else if (oversub_mode == 2) {
            /*
             * OS ratio = (IO BW - LR BW) / (MAX LR BW - IO LR BW)
             */
            os_ratio[pgw] = (((pgw_oversub_bandwidth[pgw]) * 1000) / (max_pgw_core_bandwidth[pgw] - pgw_linerate_bandwidth[pgw]));
        }

        if (soc_feature(unit, soc_feature_untethered_otp)) {
            if (SOC_BOND_INFO_FEATURE_GET(unit, socBondInfoFeatureFamilyFirebolt5)) {
                is_fb5_device = 1;
            }
        }

        /*
         * On some FB5 SKUs, there will be a few MGIG ports for which the estimated
         * effective bandwidth will be 10G for each port in theory.
         * This would make the bandwidth checks below to fail.
         * It is expected that the users do not run these ports beyond 1G or
         * whatever is constrained by the HW Datasheet. These checks are relaxed for FB5 configs.
         */
        if ((oversub_mode != 2) && !is_fb5_device){
            /* Linerate Bandwidth check */
            if (pgw_linerate_bandwidth[pgw] > max_pgw_core_bandwidth[pgw]) {
                LOG_CLI((BSL_META_U(unit,
                                    "PGW_CL%d total line rate bandwidth "
                                    "(%d Gb) exceeds %d Gb\n"),
                        pgw, (pgw_linerate_bandwidth[pgw] / 1000),
                        (max_pgw_core_bandwidth[pgw] / 1000)));
                rv = SOC_E_FAIL;
            }

            /* Oversub Bandwidth check */
            if ((pgw_linerate_bandwidth[pgw] + pgw_oversub_bandwidth[pgw]) > max_pgw_oversub_bandwidth[pgw]) {
                LOG_CLI((BSL_META_U(unit,
                                    "PGW_CL%d total oversub bandwidth "
                                    "(%d Gb) exceeds %d Gb\n"),
                        pgw, ((pgw_linerate_bandwidth[pgw] + pgw_oversub_bandwidth[pgw]) / 1000),
                        (max_pgw_oversub_bandwidth[pgw] / 1000)));
                rv = SOC_E_FAIL;
            }
        }
    }

    if (oversub_mode == 2) {
        uint16 dev_id;
        uint8  rev_id;

        soc_cm_get_id(unit, &dev_id, &rev_id);

        /* For mixed-mode we can have asymmetrical bandwidth distribution
         * Check the OS ratio in this case
         */
        def_os_ratio = dev_id == BCM56565_DEVICE_ID ? 1300 : def_os_ratio;
        if ((os_ratio[0] > def_os_ratio) && (os_ratio[0] < max_os_ratio)) {
            if ((os_ratio[1] > def_os_ratio) ||
                (os_ratio[1] > 1000 && dev_id == BCM56565_DEVICE_ID)) {
                LOG_CLI((BSL_META_U(unit,
                                    "PGW_CL1 total oversub bandwidth "
                                    "(%d Gb) exceeds limit\n"),
                                    ((pgw_oversub_bandwidth[1]) / 1000)));
                rv = SOC_E_FAIL;
            }
        } else if ((os_ratio[1] > def_os_ratio) && (os_ratio[1] < max_os_ratio)) {
            if ((os_ratio[0] > def_os_ratio) ||
                (os_ratio[0] > 1000 && dev_id == BCM56565_DEVICE_ID)) {
                LOG_CLI((BSL_META_U(unit,
                                    "PGW_CL0 total oversub bandwidth "
                                    "(%d Gb) exceeds limit\n"),
                                    ((pgw_oversub_bandwidth[0]) / 1000)));
                rv = SOC_E_FAIL;
            }
        }
    }

    return rv;
}

/*
 * Function:
 *      _soc_ap_port_resource_validate_output
 * Purpose:
 *      Debug output for given port resource array.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN) Port Resource FlexPort configuration.
 * Returns:
 *      SOC_E_XXX
 */
STATIC void
_soc_ap_port_resource_validate_output(int unit,
                                        int nport,
                                        soc_port_resource_t *resource)
{
    int i;
    soc_port_resource_t *pr;

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- SOC Port Resource Input Array ---\n")));

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "Logical Physical  Speed  Lanes   "
                            "Encap  Flags\n")));

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {

        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "  %3d     %3d    "),
                     pr->logical_port, pr->physical_port));
        if (pr->physical_port == -1) {
            LOG_VERBOSE(BSL_LS_SOC_PORT,
                        (BSL_META_U(unit,
                                    "---------------------  0x%8.8x\n"),
                         pr->flags));
            continue;
        }

        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "%6d    %2d  %6s   0x%8.8x\n"),
                     pr->speed, pr->num_lanes,
                     (pr->encap == BCM_PORT_ENCAP_HIGIG2) ? "HG" : "!HG",
                     pr->flags));
    }

    return;
}

/*
 * Function:
 *      soc_ap_port_resource_validate
 * Purpose:
 *      Validate that the given FlexPort configuration is valid.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 * Returns:
 *      SOC_E_XXX
 * Note:
 */
int
soc_ap_port_resource_validate(int unit, int nport, soc_port_resource_t *resource)
{
    int i, rv;
    soc_port_resource_t *pr;
    soc_info_misc_t *post_si;
    int is_speed_set = FALSE;

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "\n=============================================\n"
                            "========= SOC PORT RESOURCE VALIDATE ========\n"
                            "=============================================\n"
                            )));
    _soc_ap_port_resource_validate_output(unit, nport, resource);

    /*
     * Check lanes and speed assignment
     */
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- VALIDATE: Port flex enable, lanes, speed\n")));
    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {

        if (!(pr->flags & SOC_PORT_RESOURCE_SPEED)) {
            if (pr->physical_port == -1) {
                /* Check if flex operation is enabled on old phy port */
                SOC_IF_ERROR_RETURN(_soc_ap_phy_port_flex_valid
                    (unit, SOC_INFO(unit).port_l2p_mapping[pr->logical_port]));
                /* Skip delete entries */
                continue;
            } else {
                /* Check if flex operation is enabled on new phy port */
                SOC_IF_ERROR_RETURN(_soc_ap_phy_port_flex_valid
                                        (unit, pr->physical_port));
            }

            /* Check lane assignment */
            SOC_IF_ERROR_RETURN(_soc_ap_phy_port_lanes_valid(unit, pr->physical_port,
                                                             pr->num_lanes));
            /* Check speed (port rate) */
            SOC_IF_ERROR_RETURN(_soc_ap_speed_valid(unit, pr->physical_port,
                                                    pr->num_lanes, pr->speed));
        } else {
            is_speed_set = TRUE;
        }
    }

    /* Check port mappings */
    if (is_speed_set != TRUE) {
        SOC_IF_ERROR_RETURN(_soc_ap_port_mapping_validate(unit, nport, resource));
    }

    post_si = sal_alloc(sizeof(soc_info_misc_t), "soc_info_misc");
    if (NULL == post_si) {
        return SOC_E_MEMORY;
    }

    /*
     * Get additional port information needed for validation,
     * Post FlexPort SOC INFO port data
     *
     * NOTE that this is a local subset of the global SOC_INFO
     * which at this point should not be updated yet.
     */
    rv = _soc_ap_post_soc_info_get(unit, nport, resource, post_si);

    if (SOC_SUCCESS(rv)) {
        /* Validate oversub speed group limits */
        rv = _soc_ap_port_resource_ovs_speed_group_validate(unit, post_si);
    }
    /* Check mixed speed on sister ports */
    if (SOC_SUCCESS(rv)) {
        rv = _soc_ap_port_resource_mixed_sisters_validate(unit, nport, resource, post_si);
    }

    /* Check bandwidth restrictions */
    if (SOC_SUCCESS(rv)) {
        rv = _soc_ap_port_resource_bandwidth_check(unit, post_si);
    }
    sal_free(post_si);
    return rv;
}


/*
 * Function:
 *      _soc_ap_port_resource_data_cleanup
 * Purpose:
 *      Cleanup give data structure (deallocate memory).
 * Parameters:
 *      unit           - (IN) Unit number.
 *      pre_resource   - (IN/OUT) Memory to deallocate.
 * Returns:
 *      None
 * Note:
 */
STATIC void
_soc_ap_port_resource_data_cleanup(soc_port_resource_t **pre_resource)
{
    if (*pre_resource != NULL) {
        sal_free(*pre_resource);
        *pre_resource = NULL;
    }

    return;
}

/*
 * Function:
 *      _soc_ap_port_resource_mode_get
 * Purpose:
 *      Get and return the port mode in the given
 *      Port Resource configuration structure.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN/OUT) Port Resource FlexPort configuration.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      If some ports are not present (inactive), routine will
 *      try to figure out best match.
 */
STATIC int
_soc_ap_port_resource_mode_get(int unit,
                                 int nport,
                                 soc_port_resource_t *resource)
{
    soc_info_t *si = &SOC_INFO(unit); 
    soc_port_resource_t *pr;
    int i;
    int j;
    int logic_port;
    int phy_port;
    int sister_port;
    int num_lanes[_AP_PORTS_PER_TSC];
    int one_active_lane;
    int two_active_lane;
    int four_active_lane;
    char *modes_str[] = {"Quad", "Tri_012", "Tri_023", "Dual", "Single"};

    soc_pbmp_t new_disabled_bitmap;
    int new_port_p2l_mapping[SOC_MAX_NUM_PORTS];
    int new_port_num_lanes[SOC_MAX_NUM_PORTS];

    /*
     * Build information needed to determine port mode
     */

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- Get Port Mode\n")));

    /* Get current information */
    for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
        new_port_p2l_mapping[i] = si->port_p2l_mapping[i];
        new_port_num_lanes[i] = si->port_num_lanes[i];
    }
    SOC_PBMP_ASSIGN(new_disabled_bitmap, si->all.disabled_bitmap);

    /* First 'delete' mappings */
    for (i = 0, pr = &resource[0];
         (i < nport) && (pr->physical_port == -1);
         i++, pr++) {
        logic_port = pr->logical_port;

        if (pr->flags & SOC_PORT_RESOURCE_I_MAP) {
            SOC_PBMP_PORT_ADD(new_disabled_bitmap, logic_port);
            continue;
        }

        SOC_PBMP_PORT_REMOVE(new_disabled_bitmap, logic_port);
        new_port_p2l_mapping[si->port_l2p_mapping[logic_port]] = -1;
        new_port_num_lanes[logic_port] = -1;
    }

    /* Continue with 'add' mappings in rest of array */
    for (; i < nport; i++, pr++) {
        logic_port = pr->logical_port;
        phy_port = pr->physical_port;

        SOC_PBMP_PORT_REMOVE(new_disabled_bitmap, logic_port);
        new_port_p2l_mapping[phy_port] = logic_port;
        new_port_num_lanes[logic_port] = pr->num_lanes;
    }


    /*
     * Determine port mode based on sister ports lane configuration
     */
    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        /* Skip delete entries (assume these to be first in array) */
        if (pr->physical_port == -1) {
            continue;
        }

        one_active_lane = 0;
        two_active_lane = 0;
        four_active_lane = 0;

        /* Get number of active lanes on sister ports */
        phy_port = pr->physical_port;


        for (j = 0; j < _AP_PORTS_PER_TSC; j++) {
            sister_port = AP_PHY_PORT(unit, phy_port).sisters_ports[j];
            logic_port = new_port_p2l_mapping[sister_port];

            if (logic_port < 0) {
                num_lanes[j] = 0;
                continue;
            }

            if (SOC_PBMP_MEMBER(new_disabled_bitmap, logic_port)) {
                num_lanes[j] = 0;
            } else {
                num_lanes[j] = new_port_num_lanes[logic_port];
            }
        }


        /*
         * Quad:    Lane0, Lane1, Lane2, Lane3 are active (individually)
         * Dual:    Lane0, Lane2, are active (Lane0 and Lane2 are dual)
         * Single:  Lane0 is active, one physical port using all lanes, 4/10/12
         * Tri_012: Lane0, Lane1, Lane2 are active (Lane2 is dual)
         * Tri_023: Lane0, Lane2, Lane3 are active (Lane0 is dual)
         *
         * If any lane is -1, make best guess.
         */

        /* Count active lanes */
        for (j = 0; j < _AP_PORTS_PER_TSC; j++) {
            if (num_lanes[j] == 1) {
                one_active_lane++;
            } else if (num_lanes[j] == 2) {
                two_active_lane++;
            } else if (num_lanes[j] >= 4) {
                /* No more checking is needed */
                four_active_lane++;
                break;
            }
        }

        /* Get mode based on active lanes */
        if (four_active_lane) {
            pr->mode = SOC_APACHE_PORT_MODE_SINGLE;
        } else if (one_active_lane && !two_active_lane) {
            pr->mode = SOC_APACHE_PORT_MODE_QUAD;
        } else if (!one_active_lane && two_active_lane) {
            pr->mode = SOC_APACHE_PORT_MODE_DUAL;
        } else if (one_active_lane && two_active_lane) {
            if (num_lanes[0] == 1) {
                pr->mode = SOC_APACHE_PORT_MODE_TRI_012;
            } else {
                pr->mode = SOC_APACHE_PORT_MODE_TRI_023;
            }
        } else {
            LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "Invalid number of lanes "
                                  "logical_port=%d physical_port=%d "
                                  "num_lanes=%d\n"),
                       pr->logical_port, pr->physical_port, pr->num_lanes));
            return SOC_E_INTERNAL;
        }

        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "Port mode is %s for "
                                "logical_port=%d physical_port=%d\n"),
                     modes_str[pr->mode],
                     pr->logical_port, pr->physical_port));
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_ap_port_resource_output
 * Purpose:
 *      Debug output for given port resource array.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      header         - (IN) Header outout string.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN) Port Resource FlexPort configuration.
 * Returns:
 *      SOC_E_XXX
 */
STATIC void
_soc_ap_port_resource_output(int unit, char *header,
                               int nport, soc_port_resource_t *resource)
{
    int i;
    int lane;
    soc_port_resource_t *pr;
    char *modes_str[] = {"Quad", "T012", "T023", "Dual", "Sngl"};
    char *mode_str;

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "%s\n"), header));

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "Logical Physical MMU "
                            "Speed Lanes Mode Ovs PrioMask Flags       "
                            "PGW XLP PIDX\n")));

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "  %3d     %3d    "),
                     pr->logical_port, pr->physical_port));
        if (pr->physical_port == -1) {
            LOG_VERBOSE(BSL_LS_SOC_PORT,
                        (BSL_META_U(unit,
                                    "------------------------------------- "
                                    "0x%8.8x\n"),
                         pr->flags));
            continue;
        }

        /*
         * This arrays used to determine the 'string' assumes
         * certain values on the Port Modes defined in soc_apache_port_mode_e.
         */
        if ((pr->mode < 0) ||  (pr->mode > 4)) {
            mode_str = "----";
        } else {
            mode_str = modes_str[pr->mode];
        }
        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "  %3d    "
                                "%6d  %2d   %4s  %1d  0x%4.4x 0x%8.8x"),
                     pr->mmu_port, pr->speed, pr->num_lanes, mode_str,
                     pr->oversub, pr->prio_mask, pr->flags));
        if (pr->num_lanes == 0) {
            LOG_VERBOSE(BSL_LS_SOC_PORT,
                        (BSL_META_U(unit,
                                    "  %2d  %2d  %2d\n"),
                         -1, -1, -1));
        } else {
            for (lane = 0; lane < pr->num_lanes; lane++) {
                if (lane > 0) {
                    LOG_VERBOSE(BSL_LS_SOC_PORT,
                                (BSL_META_U(unit, "%65s"), " "));
                }
                LOG_VERBOSE(BSL_LS_SOC_PORT,
                            (BSL_META_U(unit,
                                        "  %2d  %2d  %2d\n"),
                         pr->lane_info[lane]->pgw,
                         pr->lane_info[lane]->tsc,
                         pr->lane_info[lane]->port_index));
            }
        }
    }

    return;
}


/*
 * Function:
 *      _soc_ap_port_resource_data_output
 * Purpose:
 *      Debug output.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN) Port Resource FlexPort configuration.
 *      pre_count      - (IN) Array size for pre-FlexPort array.
 *      pre_resource   - (IN) Pre-FlexPort configuration array.
 *      post_count     - (IN) Array size for post-FlexPort array.
 *      post_resource  - (IN) Pre-FlexPort configuration array.
 * Returns:
 *      SOC_E_XXX
 */
STATIC void
_soc_ap_port_resource_data_output(int unit,
                                    int nport, soc_port_resource_t *resource,
                                    int pre_count,
                                    soc_port_resource_t *pre_resource,
                                    int post_count,
                                    soc_port_resource_t *post_resource)
{
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "\n=============================================\n"
                            "=========== SOC PORT RESOURCE DATA ==========\n"
                            "=============================================\n"
                            )));

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit, "\n")));
    _soc_ap_port_resource_output(unit,
                                   "=== FlexPort Port Resource Data ===",
                                   nport, resource);
    
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit, "\n")));
    _soc_ap_port_resource_output(unit,
                                   "=== Pre-FlexPort Port Resource Data ===",
                                   pre_count, pre_resource);

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit, "\n")));
    _soc_ap_port_resource_output(unit,
                                   "=== Post-FlexPort Port Resource Data ===",
                                   post_count, post_resource);

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit, "\n")));
    return;
}


/*
 * Function:
 *      _soc_ap_port_resource_data_init
 * Purpose:
 *      Allocate and get pre and post FlexPort Port Resource
 *      information for given FlexPort configuration.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN/OUT) Port Resource FlexPort configuration.
 *      pre_count      - (OUT) Array size for pre-FlexPort array.
 *      pre_resource   - (OUT) Pre-FlexPort configuration array.
 *      post_count     - (OUT) Array size for post-FlexPort array.
 *      post_resource  - (OUT) Pre-FlexPort configuration array.
 *      pre_soc_info   - (OUT) A subset of the SOC_INFO information
 *                             before the FlexPort operation.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      - Assumes data is Port Resource has been validated.
 *      - Assumes array order is correct (deletes are first in array).
 *      - Assumes pointer parameters are not null.
 *      - Assumes the following fields have been filled already in input
 *        SOC Port Resource data structure:
 *          flags
 *          logical_port
 *          physical_port
 *          speed
 *          num_lanes 
 *
 *
 * The 'resource' or main-FlexPort list contains both, all the elements
 * in the pre-FlexPort list followed by the elements in the post-FlexPort
 * list (in that order):
 *     resource = pre_resource + post_resource
 *
 * The pre-FlexPort and post-FlexPort operation arrays are filled
 * out as follows:
 *
 * Pre-FlexPort array
 *     This is the array of 'old' ports.
 *     It contains current information for ports that are configured in the
 *     system and are to be modified after the FlexPort operation.
 *     This should include all mappings where ports are deleted/cleared
 *     (i.e. physical_port is -1).
 *     NOTE that even though the port is set to be 'deleted',
 *     the actual final port mapping (after FlexPort) may
 *     result to be the same, destroyed, or remapped based on what the rest
 *     of the array configuration.
 *
 * Post-FlexPort array
 *     This is the array of 'new' ports.
 *     It contains the configuration information on the new ports that will
 *     be configured in the system and are active (present) after
 *     the FlexPort operation.
 *     This should include mappings where the physical ports are not (-1).
 *     The actual ports mappings can result to be the same, remapped or new,
 *     based on the rest of the array information.
 *
 * Example:
 *
 *     Logical_port --> Physical_port
 *
 *     Main FlexPort array ('resource')
 *               L1 --> -1 (current mapped to P1), mapping is SAME
 *               L2 --> -1 (current mapped to P2), mapping is REMAP
 *               L3 --> -1 (current mapped to P3), mapping is DESTROY
 *               L1 --> P1                       , mapping is SAME
 *               L2 --> P5                       , mapping is REMAP
 *               L9 --> P9                       , mapping is NEW
 *
 *     Pre-FlexPort array
 *               L1 --> P1 mapping is SAME
 *               L2 --> P2 mapping is REMAP
 *               L3 --> P3 mapping is DESTROY
 *
 *     Post-FlexPort array
 *               L1 --> P1 mapping is SAME
 *               L2 --> P5 mapping is REMAP
 *               L9 --> P9 mapping is NEW
 */
STATIC int
_soc_ap_port_resource_data_init(int unit,
                                  int nport, soc_port_resource_t *resource,
                                  int *pre_count,
                                  soc_port_resource_t **pre_resource,
                                  int *post_count,
                                  soc_port_resource_t **post_resource,
                                  soc_ap_info_t *pre_soc_info)
{
    int i;
    int lane;
    int num_lanes;
    int phy_port;
    int delete_count = 0;
    soc_port_resource_t *pr;
    soc_port_resource_t *pre_pr;
    soc_info_t *si = &SOC_INFO(unit);

    /* Init parameters */
    *pre_count = 0;
    *pre_resource = NULL;
    *post_count = 0;
    *post_resource = NULL;


    /*************************************
     * Fill main FlexPort information
     */


    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "\n====== SOC PORT RESOURCE DATA GATHER =====\n")));

    /* Get rest of SOC FlexPort information */
    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        pr->mode = -1;
        /* Skip delete entries (assume these to be first in array) */
        if (pr->physical_port == -1) {
            delete_count++;
            continue;
        }

        phy_port = pr->physical_port;
        pr->prio_mask = AP_PHY_PORT(unit, phy_port).prio_mask;
        SOC_IF_ERROR_RETURN
            (soc_ap_port_oversub_get(unit, phy_port, pr->logical_port,
                                       &pr->oversub));

        /*
         * Get Lanes information
         * Assume lanes are arranged consecutively with respect to
         * the physical port number.
         */
        for (lane = 0; lane < pr->num_lanes; lane++) {
            pr->lane_info[lane] = &AP_PHY_PORT_LANE(unit, phy_port + lane);
        }
    }

    /*
     * Get port mode (quad, dual, single, tri) into 'resource' array.
     */
    SOC_IF_ERROR_RETURN
        (_soc_ap_port_resource_mode_get(unit,
                                          nport, resource));

    /* Set count for pre and post FlexPort arrays */
    *pre_count = delete_count;
    *post_count = nport - delete_count;


    /********************************
     * Pre-FlexPort array
     */
    /* Allocate memory */
    if ((*pre_count) > 0) {
        *pre_resource = sal_alloc(sizeof(soc_port_resource_t) * (*pre_count),
                                  "pre_port_resource");
        if (*pre_resource == NULL) {
            LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "Unable to allocate memory for pre resource "
                                  "array in FlexPort operation\n")));
            return SOC_E_MEMORY;
        }
        sal_memset(*pre_resource, 0,
                   sizeof(soc_port_resource_t) * (*pre_count));
    }
    /* Fill information */
    for (i = 0, pre_pr = *pre_resource, pr = &resource[0];
         i < *pre_count;
         i++, pre_pr++, pr++) {

        /* Assume physical port is valid */
        phy_port = si->port_l2p_mapping[pr->logical_port];

        pre_pr->flags = pr->flags;
        pre_pr->logical_port = pr->logical_port;
        pre_pr->physical_port = phy_port;
        pre_pr->mmu_port = si->port_p2m_mapping[phy_port];
        pre_pr->num_lanes = si->port_num_lanes[pr->logical_port];
        pre_pr->prio_mask = AP_PHY_PORT(unit, phy_port).prio_mask;
        pre_pr->oversub =
            SOC_PBMP_MEMBER(si->oversub_pbm, pre_pr->logical_port) ? 1 : 0;

        pre_pr->speed = si->port_speed_max[pr->logical_port];

        /* Set inactive flag to indicate port is disabled */
        if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, pr->logical_port)) {
            pre_pr->flags |= SOC_PORT_RESOURCE_INACTIVE;

            pre_pr->mode = -1;
        } else {
            SOC_IF_ERROR_RETURN
                (soc_portctrl_port_mode_get(unit, pr->logical_port,
                                            &(pre_pr->mode), &num_lanes));
        }

        /*
         * Get Lanes information
         * Assume lanes are arranged consecutively with respect to
         * the physical port number.
         */
        for (lane = 0; lane < pre_pr->num_lanes; lane++) {
            pre_pr->lane_info[lane] =
                &AP_PHY_PORT_LANE(unit, phy_port + lane);
        }

    }


    /********************************
     * Post-FlexPort array
     *
     * Just use the main FlexPort array since this has
     * all the new information that is needed for the 'post' array.
     */
    if ((*post_count) > 0) {
        *post_resource = &resource[(*pre_count)];
    }


    /********************************
     * Pre-FlexPort SOC info data
     *
     * This contains partial information of the current
     * SOC information (pre-FlexPort) needed during later
     * stages of the FlexPort reconfiguration sequence.
     */
    sal_memset(pre_soc_info, 0, sizeof(*pre_soc_info));
    for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
        pre_soc_info->port_l2p_mapping[i] = si->port_l2p_mapping[i];
        pre_soc_info->port_p2l_mapping[i] = si->port_p2l_mapping[i];
        pre_soc_info->port_p2m_mapping[i] = si->port_p2m_mapping[i];
        pre_soc_info->port_m2p_mapping[i] = si->port_m2p_mapping[i];
        pre_soc_info->port_group[i] = si->port_group[i];
        pre_soc_info->port_speed_max[i] = si->port_speed_max[i];
        pre_soc_info->port_num_lanes[i] = si->port_num_lanes[i];
    }

    SOC_PBMP_ASSIGN(pre_soc_info->xpipe_pbm, si->xpipe_pbm);
    SOC_PBMP_ASSIGN(pre_soc_info->oversub_pbm, si->oversub_pbm);

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_ap_soc_info_ptype_update
 * Purpose:
 *      Update the SOC_INFO ptype information and port names based
 *      on the port type bitmaps.
 * Parameters:
 *      unit           - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Assumes linkscan is paused, COUNTER_LOCK and SOC_CONTROL_LOCK
 *      locks are taken.
 */
STATIC int
_soc_ap_soc_info_ptype_update(int unit)
{
    soc_info_t *si = &SOC_INFO(unit);
    int phy_port;
    int logic_port;
    int max_port;
    int blk;
    int bindex;
    int port_idx;

    /*
     * Update ptype information
     */
#define RECONFIGURE_PORT_TYPE_INFO(ptype)                   \
    si->ptype.num = 0;                                      \
    si->ptype.min = si->ptype.max = -1;                     \
    sal_memset(si->ptype.port, 0, sizeof(si->ptype.port)); \
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
    RECONFIGURE_PORT_TYPE_INFO(cxx);
    RECONFIGURE_PORT_TYPE_INFO(ce);
    RECONFIGURE_PORT_TYPE_INFO(xl);
    RECONFIGURE_PORT_TYPE_INFO(xlb0);
    RECONFIGURE_PORT_TYPE_INFO(cl);
    RECONFIGURE_PORT_TYPE_INFO(clg2);
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

        for (port_idx = 0; port_idx < SOC_DRIVER(unit)->port_num_blktype; port_idx++) {
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
    for (phy_port = 0; phy_port < AP_INFO(unit)->phy_ports_max; phy_port++) {
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
 *      _soc_ap_soc_info_ptype_ports_add
 * Purpose:
 *      Set the port type bitmaps and block information in SOC_INFO
 *      for ports to be configured during the FlexPort operation.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      post_count     - (IN) Array size.
 *      post_resource  - (IN) Array of ports to add.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Assumes linkscan is paused, COUNTER_LOCK and SOC_CONTROL_LOCK
 *      locks are taken.
 */
STATIC int
_soc_ap_soc_info_ptype_ports_add(int unit,
                                   int post_count,
                                   soc_port_resource_t *post_resource)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *pr;
    int i;
    int phy_port;
    int logic_port;
    int blk;
    int bindex;
    int blktype;
    int old_blktype;
    int port_idx;

    /*
     * Add to port type bitmaps and block information
     */
    for (i = 0, pr = &post_resource[0]; i < post_count; i++, pr++) {

        logic_port = pr->logical_port;
        phy_port = pr->physical_port;

        if (!si->port_speed_max[logic_port]) {
            LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                                        "Invalid speed %d for "
                                        "logical port %d\n"),
                                        si->port_speed_max[logic_port], logic_port));
            return SOC_E_INTERNAL;
        }

        /* Clear bitmaps */
        SOC_PBMP_PORT_REMOVE(si->cl.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->cxx.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->clg2.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->xl.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->xlb0.bitmap, logic_port);

        SOC_PBMP_PORT_REMOVE(si->ether.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->hg.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->st.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->xe.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->ce.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->ge.bitmap, logic_port);

        if (pr->encap == SOC_ENCAP_IEEE) {
            if (si->port_speed_max[logic_port] >= 100000) {    /* CE */
                SOC_PBMP_PORT_ADD(si->ce.bitmap, logic_port);
            } else if (si->port_speed_max[logic_port] < 10000) {    /* GE */
                SOC_PBMP_PORT_ADD(si->ge.bitmap, logic_port);
            } else { /* XE */
                SOC_PBMP_PORT_ADD(si->xe.bitmap, logic_port);
            }
            SOC_PBMP_PORT_ADD(si->ether.bitmap, logic_port);
        } else { /* HG */
                SOC_PBMP_PORT_ADD(si->hg.bitmap, logic_port);
                SOC_PBMP_PORT_ADD(si->st.bitmap, logic_port);
        }

        SOC_PBMP_PORT_ADD(si->port.bitmap, logic_port);
        SOC_PBMP_PORT_ADD(si->all.bitmap, logic_port);
        SOC_PBMP_PORT_ADD(si->gx.bitmap, logic_port);


        blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
        bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, 0);
        if (blk < 0 && bindex < 0) { /* End of regsfile port list */
            continue;
        }

        blktype = -1;
        for (port_idx = 0; port_idx < SOC_DRIVER(unit)->port_num_blktype; port_idx++) {

            blk = SOC_PORT_IDX_BLOCK(unit, phy_port, port_idx);
            if (blk < 0) { /* End of block list of each port */
                break;
            }

            old_blktype = blktype;
            blktype = SOC_BLOCK_INFO(unit, blk).type;

            switch (blktype) {

            case SOC_BLK_CLPORT:
                if (PORT_IS_FALCON(phy_port) ||
                    ((PORT_IS_CXX(phy_port) && (si->port_speed_max[logic_port] >= 100000)))) {
                    /*PM4x25 or PM12x10 in CAUI10 mode */
                    SOC_PBMP_PORT_ADD(si->cl.bitmap, logic_port);
                } else {
                    blktype = old_blktype;
                    continue;
                }
                break;

            case SOC_BLK_CLG2PORT:
                if (PORT_IS_FALCON(phy_port)) {
                    SOC_PBMP_PORT_ADD(si->clg2.bitmap, logic_port);
                } else {
                    blktype = old_blktype;
                    continue;
                }
                break;

            case SOC_BLK_XLPORT:
                if ((PORT_IS_CXX(phy_port)) && (si->port_speed_max[logic_port] >= 100000)) {
                    /* In CAUI10 mode, enable XLPORT block for all 3 cores */
                    if (si->block_port[blk] < 0) {
                        si->block_port[blk] = logic_port;
                    }
                    if (si->block_port[blk + 1] < 0) {
                        si->block_port[blk + 1] = logic_port;
                    }
                    if (si->block_port[blk + 2] < 0) {
                        si->block_port[blk + 2] = logic_port;
                    }
                    si->block_valid[blk] += 1;
                    si->block_valid[blk + 1] += 1;
                    si->block_valid[blk + 2] += 1;
                    /* No need to add in bitmap */
                    continue;
                } else {
                    SOC_PBMP_PORT_ADD(si->xl.bitmap, logic_port);
                }
                break;

            case SOC_BLK_XLPORTB0:
                SOC_PBMP_PORT_ADD(si->xlb0.bitmap, logic_port);
                break;

            case SOC_BLK_CXXPORT:
                SOC_PBMP_PORT_ADD(si->cxx.bitmap, logic_port);
                SOC_PBMP_PORT_ADD(si->block_bitmap[blk], logic_port);
                /* Don't use the name from this block */
                blktype = old_blktype;
                continue;

            case SOC_BLK_PGW_CL:
                /* Don't use the name from this block */
                blktype = old_blktype;
                break;

            default:
                break;
            }

            /* Block info is still intact in case of legacy flexport
             * Only update port bitmaps and continue
             */
            if (pr->flags & SOC_PORT_RESOURCE_I_MAP) {
                continue;
            }

            /* Update block information */
            si->block_valid[blk] += 1;

            if (si->block_port[blk] < 0) {
                si->block_port[blk] = logic_port;
            }
            SOC_PBMP_PORT_ADD(si->block_bitmap[blk], logic_port);

        } /* For each block in the given port */

        /* Block info is still intact, so skip updating it in case of legacy flexport
         */
        if (pr->flags & SOC_PORT_RESOURCE_I_MAP) {
            continue;
        }

        si->port_type[logic_port] = blktype;

        /* Update CXXPORT block information */
        if (SOC_SUCCESS(soc_apache_port_reg_blk_index_get
                    (unit, logic_port, SOC_BLK_CXXPORT, &blk))) {
            si->block_valid[blk] += 1;
            if (si->block_port[blk] < 0) {
                si->block_port[blk] = logic_port;
            }
        }

    } /* For each port */

    /* Need to update rest of ptype information */
    SOC_IF_ERROR_RETURN(_soc_ap_soc_info_ptype_update(unit));

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_ap_soc_info_ptype_ports_delete
 * Purpose:
 *      Clear the port type bitmaps and block information in SOC_INFO
 *      for ports to be removed during the FlexPort operation.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      pre_count      - (IN) Array size.
 *      pre_resource   - (IN) Array for ports to delete.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Assumes linkscan is paused, COUNTER_LOCK and SOC_CONTROL_LOCK
 *      locks are taken.
 */
STATIC int
_soc_ap_soc_info_ptype_ports_delete(int unit,
                                      int pre_count,
                                      soc_port_resource_t *pre_resource)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *pr;
    int i;
    int phy_port;
    int logic_port;
    int blk;
    int blktype;
    int bindex;
    int port_idx;

    /*
     * Clear port bitmaps and block information
     */
    for (i = 0, pr = &pre_resource[0]; i < pre_count; i++, pr++) {

        /* Skip deletion of block info in case of legacy flexport
         * Port bitmaps will be cleared and updated in _soc_ap_soc_info_ptype_ports_add
         * as per post_resource
         */
        if (pr->flags & SOC_PORT_RESOURCE_I_MAP) {
            continue;
        }

        logic_port = pr->logical_port;
        phy_port = pr->physical_port;

        if (phy_port == -1) {
            continue;
        }

        blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
        bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, 0);
        if (blk < 0 && bindex < 0) { /* End of regsfile port list */
            continue;
        }

        blktype = -1;
        /* Clear block information */
        for (port_idx = 0; port_idx < SOC_DRIVER(unit)->port_num_blktype; port_idx++) {

            blk = SOC_PORT_IDX_BLOCK(unit, phy_port, port_idx);
            blktype = SOC_BLOCK_INFO(unit, blk).type;
            if (blk < 0) { /* End of block list of each port */
                break;
            }

            /* CXXPORT block handled seperately */
            if (blktype != SOC_BLK_CXXPORT) {
                /* Update block information */
                if (si->block_valid[blk] > 0) {
                    si->block_valid[blk] -= 1;
                }
                if (si->block_port[blk] == logic_port) {
                    si->block_port[blk] = REG_PORT_ANY;
                }
            }
            if (blktype == SOC_BLK_XLPORT) {
                /* Disable XLPORT block for last 2 cores in case CAUI10 port is being deleted */
                if (PORT_IS_CXX(phy_port) && (si->port_speed_max[logic_port] >= 100000)) {
                    if (si->block_valid[blk + 1] > 0) {
                        si->block_valid[blk + 1] -= 1;
                    }
                    if (si->block_valid[blk + 2] > 0) {
                        si->block_valid[blk + 2] -= 1;
                    }
                    if (si->block_port[blk + 1] == logic_port) {
                        si->block_port[blk + 1] = REG_PORT_ANY;
                    }
                    if (si->block_port[blk + 2] == logic_port) {
                        si->block_port[blk + 2] = REG_PORT_ANY;
                    }
                }
            }

            SOC_PBMP_PORT_REMOVE(si->block_bitmap[blk], logic_port);

        } /* For each block in the given port */

        si->port_type[logic_port] = 0;
        si->port_offset[logic_port] = 0;

        /* Clear CXXPORT block information */
        if (SOC_SUCCESS(soc_apache_port_reg_blk_index_get
                    (unit, logic_port, SOC_BLK_CXXPORT, &blk))) {
            if (si->block_valid[blk] > 0) {
                si->block_valid[blk] -= 1;
            }
            if (si->block_port[blk] == logic_port) {
                si->block_port[blk] = REG_PORT_ANY;
            }
        }

        /*
        * Clear port from all possible port bitmaps where a flexed port
        * can be a member of.
        */
        SOC_PBMP_PORT_REMOVE(si->ge.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->xe.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->cxx.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->ce.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->xl.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->xlb0.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->cl.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->clg2.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->gx.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->ether.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->hg.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->st.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->port.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->all.bitmap, logic_port);

    } /* For each port */

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_ap_soc_counter_ports_add
 * Purpose:
 *      Add ports to SOC counter map.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      post_count     - (IN) Array size.
 *      post_resource  - (IN) Array of ports to add.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Assumes linkscan is paused, COUNTER_LOCK and SOC_CONTROL_LOCK
 *      locks are taken.
 */
STATIC int
_soc_ap_soc_counter_ports_add(int unit,
                                int post_count,
                                soc_port_resource_t *post_resource)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_port_resource_t *pr;
    int i;
    int phy_port;
    int logic_port;
    int blk;
    int bindex;
    int blktype;
    int port_idx;
    soc_ctr_type_t ctype;
    int rv = SOC_E_NONE;

    for (i = 0, pr = &post_resource[0]; i < post_count; i++, pr++) {
        logic_port = pr->logical_port;
        phy_port = pr->physical_port;

        blk = SOC_PORT_IDX_BLOCK(unit, phy_port, 0);
        bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, 0);
        if (blk < 0 && bindex < 0) { /* End of regsfile port list */
            continue;
        }

        for (port_idx = 0; port_idx < SOC_DRIVER(unit)->port_num_blktype; port_idx++) {
            blk = SOC_PORT_IDX_BLOCK(unit, phy_port, port_idx);
            if (blk < 0) { /* End of block list of each port */
                break;
            }

            blktype = SOC_BLOCK_INFO(unit, blk).type;

            switch (blktype) {
            case SOC_BLK_CLPORT:
            case SOC_BLK_XLPORT:
            case SOC_BLK_XLPORTB0:
                ctype = SOC_CTR_TYPE_XE;
                break;
            case SOC_BLK_CLG2PORT:
                ctype = SOC_CTR_TYPE_CE;
                break;
            default:
                ctype = -1;
                break;
            }

            if (ctype != -1) {
                /* Adding of dma counter descriptors is only necessary when the
                 * counter thread is running
                 * this check is necessary to avoid reallocating the dma descriptos
                 * when the conter thread is stopped running before the
                 * flex operation
                 */
                if (soc->counter_interval) {
                    rv = soc_counter_port_sbusdma_desc_alloc(unit, logic_port);
                    if (SOC_FAILURE(rv)) {
                        LOG_ERROR(BSL_LS_SOC_PORT,
                        (BSL_META_U(unit,
                        "Error! Unable to allocate SBUS DMA descriptors per"
                        " logical_port %d  \n"),
                             logic_port));
                       return rv;
                    }
                }
                SOC_IF_ERROR_RETURN
                    (soc_port_cmap_set(unit, logic_port, ctype));
                SOC_PBMP_PORT_ADD(soc->counter_pbmp, logic_port);
                break;
            }
        } /* For each block in the given port */

    } /* For each port */

    SOC_IF_ERROR_RETURN(soc_counter_non_dma_pbmp_update(unit));

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_ap_soc_counter_ports_delete
 * Purpose:
 *      Delete ports from SOC counter map.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      pre_count      - (IN) Array size.
 *      pre_resource   - (IN) Array of ports to delete.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Assumes linkscan is paused, COUNTER_LOCK and SOC_CONTROL_LOCK
 *      locks are taken.
 */
STATIC int
_soc_ap_soc_counter_ports_delete(int unit,
                                   int pre_count,
                                   soc_port_resource_t *pre_resource)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_port_resource_t *pr;
    int i;
    int phy_port;
    int logic_port;
    int  rv = SOC_E_NONE;

    /* Clear counter */
    for (i = 0, pr = &pre_resource[0]; i < pre_count; i++, pr++) {
        logic_port = pr->logical_port;
        phy_port = pr->physical_port;

        if (phy_port == -1) {
            continue;
        }

        /* Delete the counter dma descriptors */
        rv = soc_counter_port_sbusdma_desc_free(unit, logic_port);
        if (SOC_FAILURE(rv)) {
           LOG_ERROR(BSL_LS_SOC_PORT,
               (BSL_META_U(unit,
                "Error! Unable to delete SBUS DMA descriptors per"
                            " logical_port %d  \n"),
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

    SOC_IF_ERROR_RETURN(soc_counter_non_dma_pbmp_update(unit));

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_ap_soc_info_ports_add
 * Purpose:
 *      Add ports to the SOC_INFO and SOC data structures.
 *
 *      Set information for mappings that are:
 *      - Same     : logical to physical mapping are the same.
 *      - Remapped : different port mapping assignment, logical port existed.
 *      - New      : logical port did not exist prior to FlexPort.
 *
 * Parameters:
 *      unit           - (IN) Unit number.
 *      pre_count      - (IN) Array size.
 *      pre_resource   - (IN) Array for ports to delete.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Assumes linkscan is paused, COUNTER_LOCK and SOC_CONTROL_LOCK
 *      locks are taken.
 */
STATIC int
_soc_ap_soc_info_ports_add(int unit,
                             int post_count,
                             soc_port_resource_t *post_resource)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *pr;
    int i;
    int logic_port;
    int phy_port, port_is_hsp;

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "SOC_INFO Ports Add\n")));


    for (i = 0, pr = &post_resource[0]; i < post_count; i++, pr++) {
        logic_port = pr->logical_port;
        phy_port = pr->physical_port;

        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "  SOC_INFO: "
                                "Add logical=%d physical=%d\n"),
                     logic_port, phy_port));


        /* Always remove from disabled bitmap */
        SOC_PBMP_PORT_REMOVE(si->all.disabled_bitmap, logic_port);

        /* Port Mapping related information */
        si->port_l2p_mapping[logic_port] = phy_port;
        si->port_p2l_mapping[phy_port] = logic_port;

        /* Pipeline */
        SOC_PBMP_PORT_ADD(si->xpipe_pbm, logic_port);
        si->port_pipe[logic_port] = 0;

        /* Port data information */
        if (!(pr->flags & SOC_PORT_RESOURCE_SPEED)) {
            si->port_init_speed[logic_port] = pr->speed;
        }
        si->port_speed_max[logic_port] = pr->speed;
        si->port_num_lanes[logic_port] = pr->num_lanes;
        si->port_group[logic_port] = AP_PHY_PORT_LANE(unit, phy_port).pgw;
        si->port_serdes[logic_port] = (phy_port - 1) / _AP_PORTS_PER_TSC;
        if (pr->oversub) {
            SOC_PBMP_PORT_ADD(si->oversub_pbm, logic_port);
        } else {
            SOC_PBMP_PORT_REMOVE(si->oversub_pbm, logic_port);
        }

        /* If this is a high speed port, we need to add it to the extended
           queues port bitmap so that nodes and queues will be able to be
           assigned properly. */
        port_is_hsp = (soc_property_port_get(unit, logic_port, spn_PORT_SCHED_HSP, -1));
        if (port_is_hsp == -1) {
            /* speed_set operation should not cause a change in schedulers */
            port_is_hsp = soc_apache_mmu_hsp_port_reserve(unit, logic_port,
                                              si->port_init_speed[logic_port]);
        }

        if (port_is_hsp) {
            SOC_PBMP_PORT_ADD(si->eq_pbm, logic_port);
        } else {
            SOC_PBMP_PORT_REMOVE(si->eq_pbm, logic_port);
        }
        AP_PHY_PORT(unit, phy_port).cl91_enabled = pr->cl91_enabled;
    }

    SOC_PBMP_ASSIGN(si->pipe_pbm[0], si->xpipe_pbm);


    /* Add to port ptype bitmap and block information */
    SOC_IF_ERROR_RETURN
        (_soc_ap_soc_info_ptype_ports_add(unit,
                                            post_count, post_resource));

    /* Add to counter map */
    SOC_IF_ERROR_RETURN
        (_soc_ap_soc_counter_ports_add(unit,
                                         post_count, post_resource));

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_ap_soc_info_ports_delete
 * Purpose:
 *      Delete ports from the SOC_INFO and SOC data structures.
 *
 *      This would be called to clear information for mappings that are:
 *      - Same     : logical to physical mapping are the same after FlexPort.
 *      - Remapped : different port mapping assignment, logical port existed.
 *      - Destroyed: logical port do no longer exist after FlexPort.
 *
 * Parameters:
 *      unit           - (IN) Unit number.
 *      pre_count      - (IN) Array size.
 *      pre_resource   - (IN) Array for ports to delete.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Assumes linkscan is paused, COUNTER_LOCK and SOC_CONTROL_LOCK
 *      locks are taken.
 */
STATIC int
_soc_ap_soc_info_ports_delete(int unit,
                                int pre_count,
                                soc_port_resource_t *pre_resource)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *pr;
    int i;
    int logic_port;
    int phy_port;

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "SOC_INFO Ports Delete\n")));


    /* Delete from counter map */
    SOC_IF_ERROR_RETURN
        (_soc_ap_soc_counter_ports_delete(unit,
                                            pre_count, pre_resource));

    /* Delete from port ptype bitmap and block information */
    SOC_IF_ERROR_RETURN
        (_soc_ap_soc_info_ptype_ports_delete(unit,
                                               pre_count, pre_resource));

    /* Port Mapping related information */
    for (i = 0, pr = &pre_resource[0]; i < pre_count; i++, pr++) {
        logic_port = pr->logical_port;
        phy_port = pr->physical_port;

        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "  SOC_INFO: "
                                "Delete logical=%d physical=%d\n"),
                     logic_port, phy_port));

        /*
         * If 'inactive' flag, just add port to disabled bitmap.
         * This is part of the legacy API behavior.
         */
        if (pr->flags & SOC_PORT_RESOURCE_I_MAP) {
            SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, logic_port);
            continue;
        }

        /* Remove from disabled bitmap */
        SOC_PBMP_PORT_REMOVE(si->all.disabled_bitmap, logic_port);

        /* Port Mapping related information */
        si->port_l2p_mapping[logic_port] = -1;
        if (phy_port != -1) {
            si->port_p2l_mapping[phy_port] = -1;
        }

        /* Pipeline */
        if (phy_port != -1) {
            SOC_PBMP_PORT_REMOVE(si->xpipe_pbm, logic_port);
        }

        if (SOC_PBMP_MEMBER(si->eq_pbm, logic_port)) {
            SOC_PBMP_PORT_REMOVE(si->eq_pbm, logic_port);
        }

        /* Logical port related information */
        if (!(pr->flags & SOC_PORT_RESOURCE_SPEED)) {
            si->port_init_speed[logic_port] = -1;
        }
        si->port_speed_max[logic_port] = -1;
        si->port_num_lanes[logic_port] = -1;
        si->port_group[logic_port] = -1;
        si->port_serdes[logic_port] = -1;
        SOC_PBMP_PORT_REMOVE(si->oversub_pbm, logic_port);
    }

    /* Update rest of ptype information */
    SOC_IF_ERROR_RETURN(_soc_ap_soc_info_ptype_update(unit));

    SOC_PBMP_ASSIGN(si->pipe_pbm[0], si->xpipe_pbm);

    return SOC_E_NONE;
}


/*
 * Function:
 *       _soc_ap_clear_enabled_port_data
 * Purpose:
 *      Clear set of HW register in order to have correct parity value.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      port      - (IN) Logical Port to clear.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_ap_clear_enabled_port_data(int unit, soc_port_t port)
{
    uint64 rval64;
    uint32 evc = 0;
    egr_vlan_control_1_entry_t entry_1;
    egr_vlan_control_2_entry_t entry_2;
    egr_vlan_control_3_entry_t entry_3;

    /* Some registers are implemented in memory, need to clear them in order
     * to have correct parity value */
    COMPILER_64_ZERO(rval64);

    sal_memset(&entry_1, 0, sizeof(entry_1));
    sal_memset(&entry_2, 0, sizeof(entry_2));
    sal_memset(&entry_3, 0, sizeof(entry_3));

    SOC_IF_ERROR_RETURN
        (WRITE_EGR_VLAN_CONTROL_1m(unit, MEM_BLOCK_ANY, port, &entry_1));
    SOC_IF_ERROR_RETURN
        (WRITE_EGR_VLAN_CONTROL_2m(unit, MEM_BLOCK_ANY, port, &entry_2));
    SOC_IF_ERROR_RETURN
        (WRITE_EGR_VLAN_CONTROL_3m(unit, MEM_BLOCK_ANY, port, &entry_3));

    SOC_IF_ERROR_RETURN(WRITE_EGR_PVLAN_EPORT_CONTROLr(unit, port, 0));
    SOC_IF_ERROR_RETURN(WRITE_EGR_SF_SRC_MODID_CHECKr(unit, port, rval64));
    SOC_IF_ERROR_RETURN(WRITE_EGR_1588_LINK_DELAY_64r(unit, port, rval64));
    SOC_IF_ERROR_RETURN(WRITE_EGR_FCOE_INVALID_CRC_FRAMESr(unit, port, 0));
    SOC_IF_ERROR_RETURN(WRITE_EGR_FCOE_DELIMITER_ERROR_FRAMESr(unit, port, 0));
    SOC_IF_ERROR_RETURN(WRITE_EGR_IPMC_CFG2m(unit, MEM_BLOCK_ALL, port, &evc));
    SOC_IF_ERROR_RETURN(WRITE_ING_TRILL_ADJACENCYr(unit, port, rval64));
    SOC_IF_ERROR_RETURN(WRITE_STORM_CONTROL_METER_CONFIGr(unit, port, 0));
    SOC_IF_ERROR_RETURN(WRITE_SFLOW_ING_THRESHOLDr(unit, port, 0));

    return SOC_E_NONE;
}


/*
 * Function:
 *       _soc_apache_misc_port_attach
 * Purpose:
 *      Initialize HW for new attached port.
 *      This routine covers the port initialization performed by
 *      the SOC MISC routine soc_misc_init()
 * Parameters:
 *      unit      - (IN) Unit number.
 *      port      - (IN) Logical Port.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_apache_misc_port_attach(int unit, soc_port_t port)
{
    soc_info_t *si;
    int phy_port;
    uint32 rval;
    soc_pbmp_t pbmp;
    egr_ing_port_entry_t egr_ing_entry;
    ing_en_efilter_bitmap_entry_t ing_en_efilter_entry;
    egr_vlan_control_1_entry_t egr_vlan_entry;
    int higig2;
    int rv = SOC_E_NONE;
    soc_control_t  *soc = SOC_CONTROL(unit);

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "  SOC MISC attach unit=%d port=%d(%s)\n"),
                 unit, port, SOC_PORT_NAME(unit, port)));

    si = &SOC_INFO(unit);

    if (IS_LB_PORT(unit, port) || IS_CPU_PORT (unit, port) ||
        IS_TDM_PORT(unit, port)) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Port cannot be a Loopback, CPU, or TDM port "
                              "unit=%d port=%d\n"),
                   unit, port));
        return SOC_E_PORT;
    }

    phy_port = si->port_l2p_mapping[port];
    if (!AP_PHY_PORT_ADDRESSABLE(unit, phy_port)) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Invalid physical port "
                              "unit=%d port=%d physical=%d\n"),
                   unit, port, phy_port));
        return SOC_E_INTERNAL;
    }


    /*****************
     * Clear port data */
    SOC_IF_ERROR_RETURN
        (_soc_ap_clear_enabled_port_data(unit, port));


    /*****************
     * Port Mappings
     *
     * NOTE: This only covers IFP_GM_LOGICAL_TO_PHYSICAL_MAPPING.
     * It does NOT set the IP or EP port mappings.  These
     * are covered in the following functions as specified by the
     *   - soc_ap_port_resource_ip_set()
     *   - soc_ap_port_resource_ep_set()
     */
    rval = 0;
    soc_reg_field_set(unit, IFP_GM_LOGICAL_TO_PHYSICAL_MAPPINGr, &rval,
                      VALIDf, 1);
    /*
     * IFP_GM_LOGICAL_TO_PHYSICAL_MAPPING.PHYSICAL_PORT_NUM is
     * merely a unique stream ID, it's not physical port number.
     */
    soc_reg_field_set(unit, IFP_GM_LOGICAL_TO_PHYSICAL_MAPPINGr, &rval,
                      PHYSICAL_PORT_NUMf,
                      si->port_p2m_mapping[phy_port] & 0x7f);
    MEM_LOCK(unit,FP_GLOBAL_MASK_TCAMm);
    rv = WRITE_IFP_GM_LOGICAL_TO_PHYSICAL_MAPPINGr(unit, port, rval);
    if (rv < 0) {
        MEM_UNLOCK(unit,FP_GLOBAL_MASK_TCAMm);
        return rv;
    }
    soc_mem_fp_global_mask_tcam_cache_update_set(unit, TRUE);
    MEM_UNLOCK(unit,FP_GLOBAL_MASK_TCAMm);
    sal_sem_give(soc->mem_scan_notify);

    /*****************
     * Ingress Port egress configuration
     */
    sal_memset(&egr_ing_entry, 0, sizeof(egr_ing_port_entry_t));
    if (IS_HG_PORT(unit, port)) {
        soc_mem_field32_set(unit, EGR_ING_PORTm, &egr_ing_entry,
                            PORT_TYPEf, 1);
        higig2 = soc_property_port_get(unit, port, spn_HIGIG2_HDR_MODE,
            soc_feature(unit, soc_feature_no_higig_plus) ? 1 : 0) ? 1 : 0;
        soc_mem_field32_set(unit, EGR_ING_PORTm, &egr_ing_entry,
                            HIGIG2f, higig2);
    }
    SOC_IF_ERROR_RETURN
        (WRITE_EGR_ING_PORTm(unit, MEM_BLOCK_ALL, port, &egr_ing_entry));


    /*****************
     * Cut through
     */
    SOC_IF_ERROR_RETURN
        (soc_apache_port_asf_speed_set(unit, port, si->port_speed_max[port]));


    /*****************
     * VLAN Controls
     * The HW defaults for EGR_VLAN_CONTROL_1.VT_MISS_UNTAG == 1, which
     * causes the outer tag to be removed from packets that don't have
     * a hit in the egress vlan tranlation table. Set to 0 to disable this.
     */
    sal_memset(&egr_vlan_entry, 0, sizeof(egr_vlan_control_1_entry_t));
    soc_mem_field32_set(unit, EGR_VLAN_CONTROL_1m, &egr_vlan_entry,
                        VT_MISS_UNTAGf, 0);
    /* Enable pri/cfi remarking on egress ports. */
    soc_mem_field32_set(unit, EGR_VLAN_CONTROL_1m, &egr_vlan_entry,
                        REMARK_OUTER_DOT1Pf, 1);
    SOC_IF_ERROR_RETURN
        (WRITE_EGR_VLAN_CONTROL_1m(unit, MEM_BLOCK_ALL, port,
                                   &egr_vlan_entry));

    /* Ingress port VLAN membership check */
    SOC_IF_ERROR_RETURN
        (READ_ING_EN_EFILTER_BITMAPm(unit, MEM_BLOCK_ANY, 0,
                                     &ing_en_efilter_entry));
    soc_mem_pbmp_field_get(unit, ING_EN_EFILTER_BITMAPm,
                           &ing_en_efilter_entry, BITMAPf, &pbmp);
    SOC_PBMP_PORT_ADD(pbmp, port);
    soc_mem_pbmp_field_set(unit, ING_EN_EFILTER_BITMAPm,
                           &ing_en_efilter_entry, BITMAPf, &pbmp);
    SOC_IF_ERROR_RETURN
        (WRITE_ING_EN_EFILTER_BITMAPm(unit, MEM_BLOCK_ANY, 0,
                                      &ing_en_efilter_entry));

    return SOC_E_NONE;
}


/*
 * Function:
 *       _soc_apache_misc_port_detach
 * Purpose:
 *      Clear HW for port to be detached.
 *      This routine clears what is programmed during during
 *      the equivalent attach routine _soc_apache_misc_port_attach().
 * Parameters:
 *      unit      - (IN) Unit number.
 *      port      - (IN) Logical Port.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_apache_misc_port_detach(int unit, soc_port_t port)
{
    soc_info_t *si;
    int phy_port;
    uint32 rval;
    soc_pbmp_t pbmp;
    egr_ing_port_entry_t egr_ing_entry;
    ing_en_efilter_bitmap_entry_t ing_en_efilter_entry;
    int rv = SOC_E_NONE;
    soc_control_t  *soc = SOC_CONTROL(unit);

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "  SOC MISC detach unit=%d port=%d(%s)\n"),
                 unit, port, SOC_PORT_NAME(unit, port)));

     si = &SOC_INFO(unit);

    if (IS_LB_PORT(unit, port) || IS_CPU_PORT (unit, port) ||
        IS_TDM_PORT(unit, port)) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Port cannot be a Loopback, CPU, or TDM port "
                              "unit=%d port=%d\n"),
                   unit, port));
        return SOC_E_PORT;
    }

    phy_port = si->port_l2p_mapping[port];
    if (!AP_PHY_PORT_ADDRESSABLE(unit, phy_port)) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Invalid physical port "
                              "unit=%d port=%d physical=%d\n"),
                   unit, port, phy_port));
        return SOC_E_INTERNAL;
    }


    /*****************
     * Clear port data */
    SOC_IF_ERROR_RETURN
        (_soc_ap_clear_enabled_port_data(unit, port));


    /*****************
     * Port Mappings
     *
     * NOTE: This only covers IFP_GM_LOGICAL_TO_PHYSICAL_MAPPING.
     * It does NOT set the IP or EP port mappings.  These
     * are covered in the following functions as specified by the
     *   - soc_ap_port_resource_ip_set()
     *   - soc_ap_port_resource_ep_set()
     */
    rval = 0;
    MEM_LOCK(unit,FP_GLOBAL_MASK_TCAMm);
    rv = WRITE_IFP_GM_LOGICAL_TO_PHYSICAL_MAPPINGr(unit, port, rval);
    if (rv < 0) {
        MEM_UNLOCK(unit,FP_GLOBAL_MASK_TCAMm);
        return rv;
    }
    soc_mem_fp_global_mask_tcam_cache_update_set(unit, TRUE);
    MEM_UNLOCK(unit,FP_GLOBAL_MASK_TCAMm);
    sal_sem_give(soc->mem_scan_notify);


    /*****************
     * Ingress Port egress configuration
     */
    sal_memset(&egr_ing_entry, 0, sizeof(egr_ing_port_entry_t));
    SOC_IF_ERROR_RETURN
        (WRITE_EGR_ING_PORTm(unit, MEM_BLOCK_ALL, port, &egr_ing_entry));


    /*****************
     * VLAN Controls
     */
    /* Ingress port VLAN membership check */
    SOC_IF_ERROR_RETURN
        (READ_ING_EN_EFILTER_BITMAPm(unit, MEM_BLOCK_ANY, 0,
                                     &ing_en_efilter_entry));
    soc_mem_pbmp_field_get(unit, ING_EN_EFILTER_BITMAPm,
                           &ing_en_efilter_entry, BITMAPf, &pbmp);
    SOC_PBMP_PORT_REMOVE(pbmp, port);
    soc_mem_pbmp_field_set(unit, ING_EN_EFILTER_BITMAPm,
                           &ing_en_efilter_entry, BITMAPf, &pbmp);
    SOC_IF_ERROR_RETURN
        (WRITE_ING_EN_EFILTER_BITMAPm(unit, MEM_BLOCK_ANY, 0,
                                      &ing_en_efilter_entry));

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_ap_port_resource_misc_attach
 * Purpose:
 *      Initialize the SOC MISC area for given set of ports.
 *      It covers HW programming that is done during SOC MISC.
 *
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN) Port Resource FlexPort configuration.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      - Assumes this is NOT called during Warmboot.
 *      - Assumes SOC_INFO is current.
 */
STATIC int
_soc_ap_port_resource_misc_attach(int unit,
                                    int nport,
                                    soc_port_resource_t *resource)
{
    int rv = SOC_E_NONE;
    soc_port_resource_t *pr;
    int i;

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "SOC MISC Attach\n")));

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        if (pr->flags & SOC_PORT_RESOURCE_ATTACH) {
            rv = _soc_apache_misc_port_attach(unit, pr->logical_port);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_PORT,
                          (BSL_META_U(unit,
                                      "Error: Unable to initialize SOC MISC "
                                      "unit=%d port=%d rv=%d\n"),
                           unit, pr->logical_port, rv));
                return rv;
            }
        }
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_ap_port_resource_misc_detach
 * Purpose:
 *      Clears the SOC MISC area for given set of ports.
 *      It covers HW programming that is done during SOC MISC.
 *
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN) Port Resource FlexPort configuration.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      - Assumes this is NOT called during Warmboot.
 *      - Assumes SOC_INFO is current.
 */
STATIC int
_soc_ap_port_resource_misc_detach(int unit,
                                    int nport,
                                    soc_port_resource_t *resource)
{
    int rv = SOC_E_NONE;
    soc_port_resource_t *pr;
    int i;

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "SOC MISC Detach\n")));

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        if (pr->flags & SOC_PORT_RESOURCE_DETACH) {
            rv = _soc_apache_misc_port_detach(unit, pr->logical_port);
            if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_PORT,
                          (BSL_META_U(unit,
                                      "Error: Unable to clear SOC MISC "
                                      "unit=%d port=%d rv=%d\n"),
                           unit, pr->logical_port, rv));
                return rv;
            }
        }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_ap_port_resource_asf_set
 * Purpose:
 *      Configure Cut-Through mode.
 *
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN) Port Resource FlexPort configuration.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      - Assumes this is NOT called during Warmboot.
 *      - Assumes SOC_INFO is current.
 */
STATIC int
_soc_ap_port_resource_asf_set(int unit,
                              int nport,
                              soc_port_resource_t *resource)
{
    int rv = SOC_E_NONE;
    soc_port_resource_t *pr;
    int i;

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "SOC MISC Detach\n")));

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        /*****************
         * Cut through
         */
        rv = soc_apache_port_asf_speed_set(unit, pr->logical_port,
                                  SOC_INFO(unit).port_speed_max[pr->logical_port]);

        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "Error: Unable to set asf")));
            return rv;
        }
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_ap_port_resource_speed_set
 * Purpose:
 *      Execute the Speed Set operation
 *
 *      This function is very similar with _soc_ap_port_resource_execute
 *      which execute full flex operation.
 *      Following functions are not called.
 *      - soc_esw_portctrl_pm_ports_delete
 *      - soc_esw_portctrl_pm_ports_add
 *      - _soc_ap_port_resource_misc_detach
 *      - _soc_ap_port_resource_misc_attach
 *      - soc_ap_port_resource_pgw_set
 *      - soc_ap_port_resource_ip_set
 *      - soc_ap_port_resource_ep_set
 *      But speed set operation may re-assign MMU port.
 *      So speed_set operation also requires to call
 *      - _soc_ap_soc_info_ports_delete
 *      - _soc_ap_soc_info_ports_add
 *      - _soc_ap_port_resource_asf_set
 *
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN) Port Resource FlexPort configuration.
 *      pre_count      - (IN) Array size for pre-FlexPort array.
 *      pre_resource   - (IN) Pre-FlexPort configuration array.
 *      post_count     - (IN) Array size for post-FlexPort array.
 *      post_resource  - (IN) Pre-FlexPort configuration array.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      - Assumes all data is Port Resource arrays has been validated.
 */
STATIC int
_soc_ap_port_resource_speed_set(int unit,
                                int nport, soc_port_resource_t *resource,
                                int pre_count,
                                soc_port_resource_t *pre_resource,
                                int post_count,
                                soc_port_resource_t *post_resource,
                                soc_ap_info_t *pre_soc_info)
{
    int delete_count;

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "\n====== SOC PORT RESOURCE SPEED SET ======\n")));

    delete_count = pre_count;

    /* Clear MMU Port Mappings */
    SOC_IF_ERROR_RETURN
        (soc_ap_port_resource_mmu_mapping_set(unit,
                                                delete_count, resource));

    SOC_IF_ERROR_RETURN
        (_soc_ap_soc_info_ports_delete(unit,
                                         pre_count, pre_resource));

    /* Add ports to SOC SW */
    SOC_IF_ERROR_RETURN
        (_soc_ap_soc_info_ports_add(unit,
                                      post_count, post_resource));

    /*
     * Note: At this point, the SOC_INFO data structure reflects
     * the new information for all the ports.
     */

    /* Reconfigure Cut-Through */
    SOC_IF_ERROR_RETURN(_soc_ap_port_resource_asf_set(unit, nport, resource));

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "\n--- SOC FLEXPORT RECONFIGURE ---\n")));
    /* Check speed class is same */
    if (!soc_esw_portctrl_is_same_speed_class(unit, pre_resource[0].speed,
                                                    post_resource[0].speed)) {
        /* Reconfigure Schedulers */
        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "--- TDM Reconfigure\n")));
        SOC_IF_ERROR_RETURN
            (soc_ap_port_resource_tdm_set(unit,
                                            pre_count, pre_resource,
                                            post_count, post_resource,
                                            pre_soc_info,
                                            AP_INFO(unit)->mmu_lossless));
    }

    /* Reconfigure MMU */
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- MMU Reconfigure\n")));

    SOC_IF_ERROR_RETURN
        (soc_ap_port_resource_mmu_set(unit, post_count, post_resource));

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_ap_port_resource_execute
 * Purpose:
 *      Execute the FlexPort operation.
 *
 *      This function updates the main SOC_INFO SW data structure and
 *      makes changes to HW.
 * 
 *      If any error occurs the operation, changes cannot be undone.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN) Port Resource FlexPort configuration.
 *      pre_count      - (IN) Array size for pre-FlexPort array.
 *      pre_resource   - (IN) Pre-FlexPort configuration array.
 *      post_count     - (IN) Array size for post-FlexPort array.
 *      post_resource  - (IN) Pre-FlexPort configuration array.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      - Assumes all data is Port Resource arrays has been validated.
 */
STATIC int
_soc_ap_port_resource_execute(int unit,
                                int nport, soc_port_resource_t *resource,
                                int pre_count,
                                soc_port_resource_t *pre_resource,
                                int post_count,
                                soc_port_resource_t *post_resource,
                                soc_ap_info_t *pre_soc_info)
{
    int delete_count;
    /**********************************************
     * Execution Flow
     *
     * The flow consists of the following high level steps,
     * the order is important.
     *
     * - Detach 'old' ports from SOC layer:
     *     + These are all ports to be cleared/deleted (indicated by physical
     *       port -1), EXCEPT those that will become inactive after the
     *       FlexPort operation (this is only true with legacy API).
     *     + Clear ports in HW MISC
     *     + Clearing any other port references in HW
     *     + Delete ports from Port Macro
     *     + Clear port information in SOC_INFO SW
     * - Attach ports in SOC layer
     *     + These are all ports that are active after the FlexPort operation.
     *     + Add port information in SOC_INFO SW
     *     + Init ports in HW MISC
     *     + Add ports to Port Macro
     * - Execute FlexPort operation 
     *
     * ------------------
     * Port Mapping
     * ------------
     * A logical to physical port mapping is given as part of the port resource
     * array.  This can lead to the following operations:
     * - Port Mapping Delete (or clear)
     *   This is indicated when the physical port is set to (-1) and
     *   the port to be flexed is considered an 'old' port.
     *   The logical port in the array entry must always be valid.
     *   A 'delete' may result in the following actions for a port mapping:
     *      (1) Destroyed
     *      (2) Same
     *      (3) Remapped
     *
     * - Port Mapping Add (or set)
     *   This is indicated when the physical port is set to a valid
     *   physical port number (>=0).
     *   The logical port in the array entry must always be valid.
     *   An 'add' may result in the following actions for a port mapping:
     *      (1) New
     *      (2) Same
     *      (3) Remapped
     *
     * Destroyed: Logical port no longer exist after FlexPort.
     * Remapped:  Logical port will continue to exist after FlexPort
     *            but is mapped to a different physical port.
     * New:       Logical port did not exist before FlexPort.
     * Same:      Logical to physical port mapping remains the same.
     *
     * NOTE that the 'PortMacro' module handles things
     * a bit different.  The Port Macro requires ALL ports, including
     * those going inactive, to always be 'deleted'
     * and 'added' back.
     *
     * ------------------
     * SOC_INFO Update
     * ---------------
     * Some HW updates need to be performed BEFORE the
     * SOC_INFO structure is updated, and some AFTER.
     *
     * Once the SOC_INFO structure is updated, the SOC register
     * functions will no longer be able to access addresses of
     * ports that were 'deleted' from SOC_INFO.  The mapping will
     * either not be available or not be correct (remapped to a new port).
     *
     * For example, the soc_reg_addr_get() figures out the
     * correct index/address looking into the 'l2p' or 'p2m' in SOC_INFO.
     *
     * Another example is the Port Macro reconfiguration:
     * This operation needs to done in two steps:
     * (1) Deletion of ports from the Port Macro needs to be done
     *     BEFORE the SOC_INFO is updated.  PortMod needs to
     *     access the port mappings before the pre-FlexPort to clear
     *     old ports.
     *
     * (2) Addition of new ports to the Port Macro needs to be done
     *     AFTER the SOC_INFO is updated.  PortMod needs to
     *     access the new ports using the new port mappings.
     *
     * In general:
     * - HW programming on 'older' ports needs to be done BEFORE
     *   the SOC_INFO is updated.
     * - HW programming on 'new' ports needs to be done AFTER
     *   the SOC_INFO is updated.
     */
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "\n====== SOC PORT RESOURCE EXECUTE ======\n")));

    /**********************************************
     *
     * DELETE
     *
     * Clear ports to be marked as 'delete'.
     * These operations must take place BEFORE SOC_INFO is updated.
     */
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- SOC DETACH PORTS ---\n")));
    delete_count = pre_count;

    /* Clear SOC MISC */
    SOC_IF_ERROR_RETURN
        (_soc_ap_port_resource_misc_detach(unit,
                                             nport, resource));
    /* Clear MMU Port Mappings */
    SOC_IF_ERROR_RETURN
        (soc_ap_port_resource_mmu_mapping_set(unit,
                                                delete_count, resource));
    /* Delete ports from Port Macro */
    SOC_IF_ERROR_RETURN
        (soc_esw_portctrl_pm_ports_delete(unit, delete_count, pre_resource));

    /*
     * Delete ports from SOC SW
     * This must happen last during the 'delete' process
     */
    SOC_IF_ERROR_RETURN
        (_soc_ap_soc_info_ports_delete(unit,
                                         pre_count, pre_resource));


    /**********************************************
     *
     * ADD
     *
     * Configure ports to be added.
     */
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "\n--- SOC ATTACH PORTS ---\n")));

    /*
     * Add ports to SOC SW
     * This must happen first during the 'add' process
     */
    SOC_IF_ERROR_RETURN
        (_soc_ap_soc_info_ports_add(unit,
                                      post_count, post_resource));

    /* Enable/Disable used/unused TSCs */
    if (soc_property_get(unit, "disable_unused_port_blocks", TRUE)) {
        SOC_IF_ERROR_RETURN(soc_apache_tsc_disable(unit));
    }

    /*
     * Note: At this point, the SOC_INFO data structure reflects
     * the new information for all the ports.
     */

    /* Init SOC MISC */
    SOC_IF_ERROR_RETURN
        (_soc_ap_port_resource_misc_attach(unit,
                                             nport, resource));

    /* Add ports to Port Macro */
    SOC_IF_ERROR_RETURN
        (soc_esw_portctrl_pm_ports_add(unit, nport, resource));


    /**********************************************
     *
     * FLEXPORT
     *
     * Device reconfiguration. 
     */
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "\n--- SOC FLEXPORT RECONFIGURE ---\n")));
    /* Reconfigure Schedulers */
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- TDM Reconfigure\n")));
    SOC_IF_ERROR_RETURN
        (soc_ap_port_resource_tdm_set(unit,
                                        pre_count, pre_resource,
                                        post_count, post_resource,
                                        pre_soc_info,
                                        AP_INFO(unit)->mmu_lossless));
    
    /* Reconfigure PGW */
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- PGW Reconfigure\n")));
    SOC_IF_ERROR_RETURN
        (soc_ap_port_resource_pgw_set(unit,
                                        pre_count, pre_resource,
                                        post_count, post_resource,
                                        AP_INFO(unit)->mmu_lossless));

    /* Reconfigure IP */
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- IP Reconfigure\n")));
    SOC_IF_ERROR_RETURN
        (soc_ap_port_resource_ip_set(unit,
                                       pre_count, pre_resource,
                                       post_count, post_resource,
                                       pre_soc_info));

    /* Reconfigure EP */
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- EP Reconfigure\n")));
    SOC_IF_ERROR_RETURN
        (soc_ap_port_resource_ep_set(unit,
                                       pre_count, pre_resource,
                                       post_count, post_resource,
                                       pre_soc_info));

    /* Reconfigure MMU */
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- MMU Reconfigure\n")));

    SOC_IF_ERROR_RETURN
        (soc_ap_port_resource_mmu_set(unit, post_count, post_resource));

    /*
     * LED reinit is not being called here because we would have setup the led config at init time
     * for all the physical ports regardless of whether they are defined in the config.bcm or not.
     * Reconfiguring here again would corrupt the led configuration in cases where
     * customer has used custom mapping.
     */

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_ap_port_resource_configure
 * Purpose:
 *      Configure device ports based on given FlexPort information.
 *
 *      This routine validates that the given FlexPort configuration
 *      is valid and if successful, executes the FlexPort operation.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 *      flag     - (IN) Check if Full flex sequence or speed set sequence
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumes:
 *      - Input has been validated.
 *        E.g.
 *          . Array is in order (deletes are first)
 *          . Logical and physical ports are addressable
 *          . Speeds and lanes are valid for physical port.
 *          . etc.
 *      - Caller has lock.
 */
int
soc_ap_port_resource_configure(int unit, int nport,
                                 soc_port_resource_t *resource,
                                 int flag)
{
    int rv;
    int pre_count;
    int post_count;
    soc_port_resource_t *pre_resource;
    soc_port_resource_t *post_resource;
    soc_ap_info_t *pre_soc_info = NULL;

    AP_INFO_INIT_CHECK(unit);

    pre_soc_info = sal_alloc(sizeof(soc_ap_info_t), "pre_soc_info");

    if (pre_soc_info == NULL) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit, "Memory Allocation Failure")));
        rv = SOC_E_MEMORY;
        return rv;
    }

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "\n=============================================\n"
                            "======== SOC PORT RESOURCE CONFIGURE ========\n"
                            "=============================================\n"
                            )));

    /* Get all Port Resource information needed for operation */
    rv = _soc_ap_port_resource_data_init(unit, nport, resource,
                                           &pre_count, &pre_resource,
                                           &post_count, &post_resource,
                                           pre_soc_info);
    if (SOC_FAILURE(rv)) {
        _soc_ap_port_resource_data_cleanup(&pre_resource);
        sal_free(pre_soc_info);
        return rv;
    }
    /* Pause linkscan */
    soc_linkscan_pause(unit);

    /* Pause counter collection */
    COUNTER_LOCK(unit);

    /* Lock the SOC structures */
    SOC_CONTROL_LOCK(unit);

    /*
     * Execute FlexPort operation
     *
     * At this point:
     *   - Validation is successfull
     *   - FlexPort information is ready
     *
     * Changes are done to the to the SOC_INFO SW data structure and HW.
     * If any error occurs the operation, changes cannot be undone.
     */
    if (flag == SOC_PORT_RESOURCE_CONFIGURE_FLEX) {
        rv = _soc_ap_port_resource_execute(unit, nport, resource,
                                           pre_count, pre_resource,
                                           post_count, post_resource,
                                           pre_soc_info);
    } else {
        rv = _soc_ap_port_resource_speed_set(unit, nport, resource,
                                             pre_count, pre_resource,
                                             post_count, post_resource,
                                             pre_soc_info);
    }
    /* Unlock the SOC structures */
    SOC_CONTROL_UNLOCK(unit);

    /* Resume counter collection */
    COUNTER_UNLOCK(unit);

    /* Resume linkscan */
    soc_linkscan_continue(unit);

    /* Debug output */
    _soc_ap_port_resource_data_output(unit,
                                        nport, resource,
                                        pre_count, pre_resource,
                                        post_count, post_resource);

    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Error executing FlexPort operation (%d)\n"),
                   rv));
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Changes to device may have been partially "
                              "executed.  System may be unstable.\n")));
    }

    /* Deallocate memory */
    _soc_ap_port_resource_data_cleanup(&pre_resource);
    sal_free(pre_soc_info);

    return rv;
}

int
soc_apache_port_cl91_status_get(int unit, soc_port_t port, uint32 *cl91_enabled)
{
    soc_info_t *si = &SOC_INFO(unit);
    int phy_port;

    AP_INFO_INIT_CHECK(unit);

    phy_port = si->port_l2p_mapping[port];
    *cl91_enabled = AP_PHY_PORT(unit, phy_port).cl91_enabled;

    return SOC_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
int
soc_apache_flexport_scache_allocate(int unit)
{
    int rv = SOC_E_NONE;
    uint8 *flexport_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_get = 0;
    uint32 alloc_size = 0;
    int stable_size;
    int default_ver = SOC_FLEXPORT_WB_DEFAULT_VERSION;


    alloc_size =   (sizeof(int) * AP_FLEX_MAX_NUM_PORTS) + /* phy to logical*/
                   (sizeof(int) * AP_FLEX_MAX_NUM_PORTS) + /* logical to phy */
                   (sizeof(int) * AP_FLEX_MAX_NUM_PORTS) + /* phy to mmu */
                   (sizeof(int) * AP_FLEX_MAX_NUM_PORTS) + /* mmu to phy */
                   (sizeof(int) * AP_FLEX_MAX_NUM_PORTS) + /* num lanes */
                   (sizeof(int) * AP_FLEX_MAX_NUM_PORTS) + /* group number */
                   (sizeof(int) * AP_FLEX_MAX_NUM_PORTS) + /* serdes number */
                   (sizeof(int) * AP_FLEX_MAX_NUM_PORTS) + /* max port speed */
                   (sizeof(pbmp_t))                      + /* oversub pbm */
                   (sizeof(pbmp_t));                       /* Disabled bitmap */

    alloc_get = alloc_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FLEXIO_HANDLE, 0);

    /* check to see if an scache table has been configured */
    rv = soc_stable_size_get(unit, &stable_size);
    if (SOC_FAILURE(rv) || stable_size <= 0) {
        return rv;
    }

    rv = soc_versioned_scache_ptr_get(unit, scache_handle, TRUE, &alloc_get,
                                      &flexport_scache_ptr, default_ver, NULL);

    if (rv  == SOC_E_CONFIG) {
        /* Probably Level1 */
        return SOC_E_NONE;
    }

    if (NULL == flexport_scache_ptr) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "ERROR: scache memory not allocated for flexport"
                 "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
        return SOC_E_MEMORY;
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));

    return SOC_E_NONE;
}

int
soc_apache_flexport_scache_sync(int unit)
{
    uint8 *flexport_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_get=0;
    uint32 alloc_size = 0;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 scache_offset=0;
    uint32 var_size, hole_size;
    int rv = 0;


    alloc_size =   (sizeof(int) * AP_FLEX_MAX_NUM_PORTS) + /* phy to logical*/
                   (sizeof(int) * AP_FLEX_MAX_NUM_PORTS) + /* logical to phy */
                   (sizeof(int) * AP_FLEX_MAX_NUM_PORTS) + /* phy to mmu */
                   (sizeof(int) * AP_FLEX_MAX_NUM_PORTS) + /* mmu to phy */
                   (sizeof(int) * AP_FLEX_MAX_NUM_PORTS) + /* num lanes */
                   (sizeof(int) * AP_FLEX_MAX_NUM_PORTS) + /* group number */
                   (sizeof(int) * AP_FLEX_MAX_NUM_PORTS) + /* serdes number */
                   (sizeof(int) * AP_FLEX_MAX_NUM_PORTS) + /* max port speed */
                   (sizeof(pbmp_t))                      + /* oversub pbm */
                   (sizeof(pbmp_t));                       /* Disabled bitmap */

    hole_size = (sizeof(int));
    var_size = (sizeof(int) * (AP_FLEX_MAX_NUM_PORTS - 1));
    alloc_get = alloc_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FLEXIO_HANDLE, 0);

    rv = soc_versioned_scache_ptr_get(unit, scache_handle, FALSE, &alloc_get, &flexport_scache_ptr,
                                      SOC_FLEXPORT_WB_DEFAULT_VERSION, NULL);
    if (rv == SOC_E_NOT_FOUND) {
        /* Probably Level1 */
        return SOC_E_NONE;
    }

    if (alloc_get != alloc_size) {
        /* Expected size doesn't match retrieved size */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "ERROR: scache memory for flexport size mismatch"
                 "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
        return SOC_E_INTERNAL;
    }

    if (NULL == flexport_scache_ptr) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "ERROR: scache memory not allocated for flexport"
                 "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
        return SOC_E_MEMORY;
    }

    /* Physical to logical port mapping */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_p2l_mapping, var_size);
    scache_offset += (var_size + hole_size);

    /* Logical to Physical port mapping */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_l2p_mapping, var_size);
    scache_offset += (var_size + hole_size);

    /* Physical to mmu port mapping */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_p2m_mapping, var_size);
    scache_offset += (var_size + hole_size);

    /* MMU to Physical port mapping */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_m2p_mapping, var_size);
    scache_offset += (var_size + hole_size);

    /* Num lanes per port */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_num_lanes, var_size);
    scache_offset += (var_size + hole_size);

    /* Port Group */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_group, var_size);
    scache_offset += (var_size + hole_size);

    /* Port Serdes */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_serdes, var_size);
    scache_offset += (var_size + hole_size);

    /* Max port speed */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_speed_max, var_size);
    scache_offset += (var_size + hole_size);

    /* Oversub pbm */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               &si->oversub_pbm, sizeof(pbmp_t));
    scache_offset += sizeof(pbmp_t);

    /* Disabled Port Bitmap */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               &si->all.disabled_bitmap, sizeof(pbmp_t));
    scache_offset += sizeof(pbmp_t);

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));

    return SOC_E_NONE;
}

int
soc_apache_flexport_scache_recovery(int unit)
{
    uint32 alloc_get=0;
    uint32 alloc_size = 0;
    uint32 additional_scache_size = 0;
    int rv = SOC_E_NONE;
    int phy_port;
    uint8 *flexport_scache_ptr = NULL;
    soc_scache_handle_t scache_handle;
    uint32 scache_offset=0;
    uint32 var_size, hole_size;
    uint16 recovered_ver = 0;
    soc_info_t *si = &SOC_INFO(unit);


    alloc_size =   (sizeof(int) * AP_FLEX_MAX_NUM_PORTS) + /* phy to logical*/
                   (sizeof(int) * AP_FLEX_MAX_NUM_PORTS) + /* logical to phy */
                   (sizeof(int) * AP_FLEX_MAX_NUM_PORTS) + /* phy to mmu */
                   (sizeof(int) * AP_FLEX_MAX_NUM_PORTS) + /* mmu to phy */
                   (sizeof(int) * AP_FLEX_MAX_NUM_PORTS) + /* num lanes */
                   (sizeof(int) * AP_FLEX_MAX_NUM_PORTS) + /* group number */
                   (sizeof(int) * AP_FLEX_MAX_NUM_PORTS) + /* serdes number */
                   (sizeof(int) * AP_FLEX_MAX_NUM_PORTS) + /* max port speed */
                   (sizeof(pbmp_t))                      + /* oversub pbm */
                   (sizeof(pbmp_t));                       /* Disabled bitmap */

    hole_size = (sizeof(int));
    var_size = (sizeof(int) * (AP_FLEX_MAX_NUM_PORTS - 1));
    alloc_get = alloc_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FLEXIO_HANDLE, 0);
    rv = soc_versioned_scache_ptr_get(unit, scache_handle, FALSE, &alloc_get,
                                      &flexport_scache_ptr, SOC_FLEXPORT_WB_DEFAULT_VERSION,
                                      &recovered_ver);

    if (SOC_FAILURE(rv)) {
        if ((rv == SOC_E_CONFIG) ||
            (rv == SOC_E_NOT_FOUND)) {
            /* warmboot file does not contain this
            * module, or the warmboot state does not exist.
            * in this case return SOC_E_NOT_FOUND
            */
            return SOC_E_NOT_FOUND;
        } else {
            /* Only Level2 - flexport treat this as a error */
            LOG_ERROR(BSL_LS_SOC_COMMON,
                     (BSL_META_U(unit,
                     "Failed to recover scache data - %s\n"),soc_errmsg(rv)));
            return rv;
        }
    }

    if (alloc_get != alloc_size) {
        /* Expected size doesn't match retrieved size */
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "ERROR: scache recovery for flexport"
                 "%s()[LINE:%d] DONE \n"),
                  FUNCTION_NAME(),  __LINE__));
        return SOC_E_INTERNAL;
    }

    if (NULL == flexport_scache_ptr) {
        LOG_ERROR(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "ERROR: scache recovery for flexport"
                 "%s()[LINE:%d] DONE \n"),
                  FUNCTION_NAME(),  __LINE__));
        return SOC_E_MEMORY;
    }

    /* Physical to logical port mapping */
    sal_memcpy(si->port_p2l_mapping,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += (var_size + hole_size);

    /* Logical to Physical port mapping */
    sal_memcpy(si->port_l2p_mapping,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += (var_size + hole_size);

    /* Physical to mmu port mapping */
    sal_memcpy(si->port_p2m_mapping,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += (var_size + hole_size);

    /* MMU to Physical port mapping */
    sal_memcpy(si->port_m2p_mapping,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += (var_size + hole_size);

    /* Num lanes per port */
    sal_memcpy(si->port_num_lanes,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += (var_size + hole_size);

    /* Port Group */
    sal_memcpy(si->port_group,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += (var_size + hole_size);

    /* Port Serdes */
    sal_memcpy(si->port_serdes,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += (var_size + hole_size);

    /* Max port speed */
    sal_memcpy(si->port_speed_max,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += (var_size + hole_size);

    /* Oversub pbm */
    sal_memcpy(&si->oversub_pbm,
               &flexport_scache_ptr[scache_offset],
               sizeof(pbmp_t));
    scache_offset += sizeof(pbmp_t);

    /* Disabled Port Bitmap */
    sal_memcpy(&si->all.disabled_bitmap,
           &flexport_scache_ptr[scache_offset],
           sizeof(pbmp_t));
    scache_offset += sizeof(pbmp_t);

    if (additional_scache_size > 0) {
        SOC_IF_ERROR_RETURN(soc_scache_realloc(unit, scache_handle,
                                               additional_scache_size));
    }

    SOC_PBMP_CLEAR(si->xpipe_pbm);
    SOC_PBMP_CLEAR(si->pipe_pbm[0]);

    for (phy_port = 1; phy_port < SOC_MAX_NUM_PORTS; phy_port++) {
        if (si->port_p2l_mapping[phy_port] == -1) {
            continue;
        }
        SOC_PBMP_PORT_ADD(si->xpipe_pbm, si->port_p2l_mapping[phy_port]);
        SOC_PBMP_PORT_ADD(si->pipe_pbm[0], si->port_p2l_mapping[phy_port]);
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));

    return SOC_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
void
soc_apache_flexport_sw_dump(int unit) {
    int port;
    int portGroup;
    int portSerdes;
    int portLanes;
    int bandwidth;
    int l2p;
    int p2m;
    int m2p;
    int p2l;
    char  pfmt[SOC_PBMP_FMT_LEN];
    int phy_port, mmu_port, cosq, numq, uc_cosq, uc_numq;

    LOG_CLI((BSL_META_U(unit,
                            "           logical  p2l  "
               "l2p  p2m   m2p  ucast_Qbase/Numq  mcast_Qbase/Numq\n")));
    for (phy_port = 1; phy_port < SOC_MAX_NUM_PORTS; phy_port++) {
        port = SOC_INFO(unit).port_p2l_mapping[phy_port];
        if (port == -1) {
            continue;
        }
        mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
        cosq = SOC_INFO(unit).port_cosq_base[port];
        numq = SOC_INFO(unit).port_num_cosq[port];
        uc_cosq = SOC_INFO(unit).port_uc_cosq_base[port];
        uc_numq = SOC_INFO(unit).port_num_uc_cosq[port];
        cosq = soc_apache_logical_qnum_hw_qnum(unit, port, cosq, 0);
        uc_cosq = soc_apache_logical_qnum_hw_qnum(unit, port, uc_cosq,
                                               1);
        l2p = phy_port;
        p2m = mmu_port;

        m2p = SOC_INFO(unit).port_m2p_mapping[mmu_port];
        p2l = SOC_INFO(unit).port_p2l_mapping[phy_port];
        LOG_CLI((BSL_META_U(unit,
                            "  %8s  %3d     %3d  "
                "%3d   %3d  %3d      %4d/%-4d            %4d/%-4d\n"),
                SOC_INFO(unit).port_name[port], port,
                p2l, l2p, p2m, m2p,
                uc_cosq, uc_numq, cosq, numq));
    }
    LOG_CLI((BSL_META_U(unit,
              "\nlogical  physical  bandwidth  "
              "portLanes  portGroup  portSerdes  ")));

    for (phy_port = 1; phy_port < SOC_MAX_NUM_PORTS; phy_port++) {
        port = SOC_INFO(unit).port_p2l_mapping[phy_port];
        if (port == -1) {
            continue;
        }
        bandwidth = SOC_INFO(unit).port_speed_max[port];
        portLanes = SOC_INFO(unit).port_num_lanes[port];
        portGroup = SOC_INFO(unit).port_group[port];
        portSerdes = SOC_INFO(unit).port_serdes[port];
        LOG_CLI((BSL_META_U(unit,
                          "\n%3d     %3d       "
                          "%9d        "
                          "%3d        "
                          "%3d        "
                          "%3d         "),
                          port, phy_port, bandwidth,
                          portLanes, portGroup, portSerdes));
    }

    LOG_CLI((BSL_META_U(unit,
                        "\n    Oversub Bitmap: %s"),
       SOC_PBMP_FMT(SOC_INFO(unit).oversub_pbm, pfmt)));
    LOG_CLI((BSL_META_U(unit,
                        "\n    Disabled Bitmap: %s \n"),
       SOC_PBMP_FMT(SOC_INFO(unit).all.disabled_bitmap, pfmt)));


}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */


#endif /* BCM_APACHE_SUPPORT */
