/* 
 * 
 * This license is set out in https://raw.githubusercontent.com/Broadcom-Network-Switching-Software/OpenBCM/master/Legal/LICENSE file.
 * 
 * Copyright 2007-2020 Broadcom Inc. All rights reserved.
 *
 * File:        port.c
 * Purpose:     TD2+ SOC Port driver.
 *
 *              Contains information and interfaces for the
 *              physical port in the device.
 */

#include <shared/bsl.h>
#include <soc/defs.h>

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/td2_td2p.h>
#include <soc/trident2.h>
#include <soc/esw/port.h>
#include <soc/esw/portctrl.h>
#include <soc/esw/portctrl_internal.h>
#include <soc/scache.h>
#include <soc/portmod/portmod_legacy_phy.h>
#include <soc/counter.h>
#include <soc/mem.h>
#include <sal/appl/sal.h>

#ifdef BCM_WARM_BOOT_SUPPORT

#define SOC_FLEXPORT_WB_VERSION_1_0                SOC_SCACHE_VERSION(1,0)
#define SOC_FLEXPORT_WB_VERSION_1_1                SOC_SCACHE_VERSION(1,1)
#define SOC_FLEXPORT_WB_DEFAULT_VERSION            SOC_FLEXPORT_WB_VERSION_1_1

#define TD2P_FLEX_MAX_NUM_PORTS 130
#endif

/*
 * Physical Port Information
 *
 *     Some information are defined in HW and others are defined
 *     through SOC properties during configuration time.
 */
typedef struct td2p_phy_port_s {
    /*
     * The following fields are defined in HW
     */
    soc_port_lane_info_t lane_info;    /* Lane information in PGW block */
    uint32               lanes_valid;  /* Lanes capabilities */
    int                  pipe;
    int                  sisters_ports[_TD2_PORTS_PER_XLP];
                                       /* Ports part of the XLPORT/XLMAC/TSC-4
                                          where the physical port resides */

    /*
     * The following fields are configurable throught SOC properties
     */
    uint16               prio_mask;    /* Packet priority to priority
                                          group mapping mask */
    int                  flex;         /* Flex enable by SOC property
                                          'port_flex_enable' */
} td2p_phy_port_t;

/*
 * Physical Device information
 */
typedef struct td2p_info_s {
    td2p_phy_port_t  phy_port[SOC_MAX_NUM_PORTS];
    int              phy_ports_max;   /* Max number physical ports in device */
    uint32           speed_valid[SOC_PORT_RESOURCE_LANES_MAX+1];
                                      /* Port rate ability on serdes lane */
    int              speed_max;       /* Max speed on any port in device */
    int              speed_max_x;     /* Max speed on port in x pipe */
    int              speed_max_y;     /* Max speed on port in y pipe */
    int              ports_pipe_max;  /* Max number of logical ports allowed
                                         per pipe */
    int              ports_pipe_max_x;/* Max number of logical ports allowed
                                         on X pipe */
    int              ports_pipe_max_y;/* Max number of logical ports allowed
                                         on Y pipe */
    int              port_max;        /* Max logical port number value */

    /*
     * The following fields are configurable throught SOC properties
     */
    int              mmu_lossless;    /* MMU lossless */
    int              flex_legacy;     /* Flex enable by legacy ':i' in
                                         SOC property 'portmap' */
} td2p_info_t;


/*
 * Contains information corresponding to each physical port.
 * This information is fixed in a device and is calculated
 * only during init time.
 */
static td2p_info_t                  *td2p_info[SOC_MAX_NUM_DEVICES];
/*
 * This td2p_port_speed is used by cosq module to decide sched type(HSP/LLS)
 * depending on the port speed.
 */
static int *td2p_port_speed[SOC_MAX_NUM_DEVICES];

/*
 * This td2p_port_sched_hsp is used by the cosq module to decide sched type
 * (HSP/LLS) depending on the port_sched_hsp parameter. 
 * port_sched_hsp is a device init parameter
 * that SHOULD NOT have any effect after flexing; once a port is flexed,
 * port_sched_hsp should be set to 0 
 */
static int *td2p_port_sched_hsp[SOC_MAX_NUM_DEVICES];

#define TD2P_INFO(_u)               (td2p_info[(_u)])
#define TD2P_PHY_PORT(_u, _p)       (TD2P_INFO(_u)->phy_port[(_p)])
#define TD2P_PHY_PORT_LANE(_u, _p)  (TD2P_PHY_PORT(_u, _p).lane_info)


/* Lanes support */
#define TD2P_PHY_PORT_LANES_1      (1 << 0)
#define TD2P_PHY_PORT_LANES_2      (1 << 1)
#define TD2P_PHY_PORT_LANES_4      (1 << 2)
#define TD2P_PHY_PORT_LANES_10     (1 << 3)
#define TD2P_PHY_PORT_LANES_12     (1 << 4)

/* Port Rate support */
/* Ethernet */
#define TD2P_PHY_PORT_RATE_1       (1 << 0)
#define TD2P_PHY_PORT_RATE_2_5     (1 << 1)
#define TD2P_PHY_PORT_RATE_10      (1 << 2)
#define TD2P_PHY_PORT_RATE_20      (1 << 3)
#define TD2P_PHY_PORT_RATE_40      (1 << 4)
#define TD2P_PHY_PORT_RATE_100     (1 << 5)
/* HG */
#define TD2P_PHY_PORT_RATE_11      (1 << 6)
#define TD2P_PHY_PORT_RATE_21      (1 << 7)
#define TD2P_PHY_PORT_RATE_42      (1 << 8)
#define TD2P_PHY_PORT_RATE_106     (1 << 9)
#define TD2P_PHY_PORT_RATE_127     (1 << 10)
#define TD2P_PHY_PORT_RATE_13      (1 << 11)
#define TD2P_PHY_PORT_RATE_16      (1 << 12)


/* Default for the max port speed configured for device */
#define TD2P_FLEX_SPEED_MAX_DEFAULT    42000

/* 100GE Mode Lanes */
#define TD2P_100GE_MLD_MAP_4_4_2_STR  "0xff9876543210"
#define TD2P_100GE_MLD_MAP_3_4_3_STR  "0xf9876543f210"
#define TD2P_100GE_MLD_MAP_2_4_4_STR  "0x98765432ff10"

#define SOC_TD2P_PORT_RESOURCE_OP_SPEED   (1 << 2)

/*
 * Define:
 *      TD2P_INFO_INIT_CHECK
 * Purpose:
 *      Causes a routine to return SOC_E_INIT if module is not yet initialized.
 */
#define TD2P_INFO_INIT_CHECK(_u) \
    if (td2p_info[_u] == NULL) { return SOC_E_INIT; }

/*
 * Function:
 *      _soc_td2p_phy_port_lane_info_init
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
_soc_td2p_phy_port_lane_info_init(int unit)
{
    int phy_port;
    int phy_port_base;
    int i;
    int blk;
    int bindex;
    int pgw;
    int xlp;

    /* Set information to invalid */
    for (phy_port = 0; phy_port < SOC_MAX_NUM_PORTS; phy_port++) {
        TD2P_PHY_PORT(unit, phy_port).pipe = -1;
        TD2P_PHY_PORT_LANE(unit, phy_port).pgw = -1;
        TD2P_PHY_PORT_LANE(unit, phy_port).xlp = -1;
        TD2P_PHY_PORT_LANE(unit, phy_port).port_index = -1;
    }

    for (phy_port = 0; phy_port < SOC_MAX_NUM_PORTS; phy_port++) {

        /* Check for end of port list */
        if (SOC_PORT_BLOCK(unit, phy_port) < 0 &&
            SOC_PORT_BINDEX(unit, phy_port) < 0) {
            break;
        }

        /* Set pipe information */
        TD2P_PHY_PORT(unit, phy_port).pipe =
            phy_port / TD2P_PHY_PORTS_PER_PIPE;

        /* Skip CPU and Loopback ports */
        if ((phy_port == TD2P_PHY_PORT_CPU) ||
            (phy_port == TD2P_PHY_PORT_LB)) {
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
                /* Get PGW, XLP and Port Index values */
                TD2P_PHY_PORT_LANE(unit, phy_port).pgw =
                    SOC_BLOCK_NUMBER(unit, blk);
                TD2P_PHY_PORT_LANE(unit, phy_port).xlp =
                    bindex / TD2P_PORTS_PER_XLP;
                TD2P_PHY_PORT_LANE(unit, phy_port).port_index =
                    bindex % TD2P_PORTS_PER_XLP;
            }

            /* Set port lane capabilites */
            if (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_XLPORT) {
                TD2P_PHY_PORT(unit, phy_port).lanes_valid =
                    TD2P_PHY_PORT_LANES_1;
                if (bindex == 0) {
                    TD2P_PHY_PORT(unit, phy_port).lanes_valid |=
                        TD2P_PHY_PORT_LANES_2 | TD2P_PHY_PORT_LANES_4;
                } else if (bindex == 2) {
                    TD2P_PHY_PORT(unit, phy_port).lanes_valid |=
                        TD2P_PHY_PORT_LANES_2;
                }
            }
            if (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CPORT) {
                TD2P_PHY_PORT(unit, phy_port).lanes_valid |=
                    TD2P_PHY_PORT_LANES_10 | TD2P_PHY_PORT_LANES_12;
            }

        } /* for port_num_blktype */

    } /* for phy_port */

    /* Set total number of physical ports in device */
    TD2P_INFO(unit)->phy_ports_max = phy_port;

    /*
     * Fill sisters ports
     * This logic assumes a given physical layout in the device
     */
    for (phy_port = 0; phy_port < TD2P_INFO(unit)->phy_ports_max;) {

        if (TD2P_PHY_PORT_LANE(unit, phy_port).pgw == -1) {
            phy_port++;
            continue;
        }

        pgw = TD2P_PHY_PORT_LANE(unit, phy_port).pgw;
        xlp = TD2P_PHY_PORT_LANE(unit, phy_port).xlp;
        phy_port_base = phy_port;
        while ((TD2P_PHY_PORT_LANE(unit, phy_port).pgw == pgw) &&
               (TD2P_PHY_PORT_LANE(unit, phy_port).xlp == xlp)) {
            for (i = 0; i < _TD2_PORTS_PER_XLP; i++) {
                TD2P_PHY_PORT(unit, phy_port).sisters_ports[i] =
                    phy_port_base + i;
            }
            phy_port++;
        }
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_prio_mask_init
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
_soc_td2p_prio_mask_init(int unit)
{
    int phy_port;
    int port;
    uint16 prio_mask;
    soc_info_t *si = &SOC_INFO(unit);

    for (phy_port = 0; phy_port < SOC_MAX_NUM_PORTS; phy_port++) {
        /* Skip ports not addressable */
        if (!TD2P_PHY_PORT_ADDRESSABLE(unit, phy_port)) {
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

        /* Set based on logical port property */
        port = si->port_p2l_mapping[phy_port];
        if (port != -1) {
            SOC_IF_ERROR_RETURN
                (soc_trident2_get_prio_map(unit, port, &prio_mask));
        }

        TD2P_PHY_PORT(unit, phy_port).prio_mask = prio_mask;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_flex_enable_init
 * Purpose:
 *      Initialize the flex enable capability
 *      for physical ports in the SW device information data structure.
 *
 *      The flexing capabality on a port is defined based on
 *      these SOC properties:
 *
 *      (1) 'portmap' with ':i' option (inactive ports)
 *          When defined, this allows flexing in the entire device.
 *          This sets TD2P_INFO(unit)->flex_legacy
 *
 *      (2) 'port_flex_enable'
 *           This SOC property is set per TSC.
 *           When set, FlexPort operations are allowed in the TSC.
 *           Property should be specified in the base physical port of a TSC.
 *           This sets TD2P_PHY_PORT(unit, phy_port).flex
 *
 *           For a TSC-12, the property of the the FIRST physical port
 *           is applied to all 12 ports (i.e. 4 ports in 3xTSC-4s).
 *
 *           For a TSC-4 only (TSC-4 NOT part of a TSC-12), the property
 *           is applied on just the 4 physical ports.
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
 *      Assumes _soc_td2p_phy_port_lane_info_init() has been called.
 */
STATIC int
_soc_td2p_flex_enable_init(int unit)
{
    int phy_port;
    int port_base;
    int flex;
    int i;
    int xlp;
    int num_ports;
    soc_info_t *si = &SOC_INFO(unit);

    /*
     * This field is set in SOC_INFO if inactive ports are specified
     * in the SOC property 'portmap' with the ':i' option.
     */
    TD2P_INFO(unit)->flex_legacy = si->flex_eligible;

    for (phy_port = 0; phy_port < SOC_MAX_NUM_PORTS;) {
        /* Skip ports not addressable, CPU and Loopback ports */
        if (!TD2P_PHY_PORT_ADDRESSABLE(unit, phy_port) ||
            (phy_port == TD2P_PHY_PORT_CPU) ||
            (phy_port == TD2P_PHY_PORT_LB)) {
            phy_port++;
            continue;
        }

        /* Check for flex ability in 'port_flex_enable' SOC property */
        port_base = TD2P_PHY_PORT(unit, phy_port).sisters_ports[0];
        flex = soc_property_phys_port_get(unit, port_base,
                                          spn_PORT_FLEX_ENABLE, 0);
        /*
         * Set flex ability in all ports in the TSC based on
         * property set on base port in TSC
         *
         * On TSC-12, apply property to all 12 ports (3 TSC-4s).
         * XLPs on a TSC-12 are 0, 1, 2.
         *
         * TSC-4 only (non TSC-12) are XLP #3.
         *
         * Assumes TD2+ specific physical device layout
         * with physical ports and TSCs/XLPs numbering.
 */
        xlp = TD2P_PHY_PORT_LANE(unit, phy_port).xlp;
        if (xlp == 3) {
            num_ports = _TD2_PORTS_PER_XLP;       /* TSC-4 */
        } else {
            num_ports = _TD2_PORTS_PER_XLP * 3;   /* TSC-12 */
        }
        for (i = 0; i < num_ports; i++) {
            TD2P_PHY_PORT(unit, phy_port).flex = flex;
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
 *      _soc_td2p_speed_valid_init
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
_soc_td2p_speed_valid_init(int unit)
{
    int lanes;
    uint32 speed_valid;

    /* Set port rate based on number of serdes lanes */
    for (lanes = 0; lanes <= SOC_PORT_RESOURCE_LANES_MAX; lanes++) {

        if (lanes == 1) {
            speed_valid =
                TD2P_PHY_PORT_RATE_1 |
                TD2P_PHY_PORT_RATE_2_5 |
                TD2P_PHY_PORT_RATE_10 |
                TD2P_PHY_PORT_RATE_11 ;

        } else if (lanes == 2) {
            speed_valid =
                TD2P_PHY_PORT_RATE_10 |
                TD2P_PHY_PORT_RATE_20 |
                TD2P_PHY_PORT_RATE_21 ;

        } else if (lanes == 4) {
            speed_valid =
                TD2P_PHY_PORT_RATE_10 |
                TD2P_PHY_PORT_RATE_20 |
                TD2P_PHY_PORT_RATE_40 |
                TD2P_PHY_PORT_RATE_11 |
                TD2P_PHY_PORT_RATE_42 ;
            if(SOC_IS_TITAN2PLUS(unit)){
                speed_valid |=
                    TD2P_PHY_PORT_RATE_13 |
                    TD2P_PHY_PORT_RATE_16 ;
            }

        } else if (lanes == 10) {
            speed_valid =
                TD2P_PHY_PORT_RATE_100 |
                TD2P_PHY_PORT_RATE_106;

        } else if (lanes == 12) {
            speed_valid =
                TD2P_PHY_PORT_RATE_127;

        } else {
            speed_valid = 0;
        }

        TD2P_INFO(unit)->speed_valid[lanes] = speed_valid;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_ports_pipe_max_init
 * Purpose:
 *      Initialize the SW device information data structure for:
 *      - speed_max      : Maximum allowed port speed in device.
 *      - ports_pipe_max : Maximum allowed number of logical ports per
 *                         Pipe, this depends on the SKU and maximum
 *                         port speed in device.
 *      - port_max       : Maximum logical port number.
 *
 *      NOTE that 'port_max' is the value of the port, whereas
 *      'ports_pipe_max' is the total number of ports.
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
_soc_td2p_ports_pipe_max_init(int unit)
{
    int port;
    int speed_max, speed_max_x, speed_max_y;
    int ports_max, ports_max_x, ports_max_y;
    soc_info_t *si = &SOC_INFO(unit);

    /* Get speed from SOC property */
    speed_max = soc_property_get(unit, spn_PORT_FLEX_SPEED_MAX,
                                 TD2P_FLEX_SPEED_MAX_DEFAULT);

    speed_max_x = soc_property_suffix_num_get_only_suffix(unit, 0, spn_PORT_FLEX_SPEED_MAX,
                                "x", -1);


    speed_max_y = soc_property_suffix_num_get_only_suffix(unit, 0, spn_PORT_FLEX_SPEED_MAX,
                                "y", -1);

    /* If one pipe's max flex speed is defined, the other must be as well */
    if (!((speed_max_x > 0 && speed_max_y > 0) ||
        (speed_max_x < 0 && speed_max_y < 0))) {
        return SOC_E_CONFIG;
    }

    /* Compare with port speed given in 'portmap' configuration */
    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        if (si->port_speed_max[port] > speed_max) {
            speed_max = si->port_speed_max[port];
        }

        /* Compare for each pipe */
        /* coverity[overrun-local] */
        if (SOC_PBMP_MEMBER(si->xpipe_pbm, port)) {
            if (speed_max_x != -1 && si->port_speed_max[port] > speed_max_x) {
                speed_max_x = si->port_speed_max[port];
            }
        } else {
            if (speed_max_y != -1 && si->port_speed_max[port] > speed_max_y) {
                speed_max_y = si->port_speed_max[port];
            }
        }
    }

    /* Set max speed to either 42000 or to 127000 to allow for HG speeds */
    if (speed_max > 42000) {
        speed_max = 127000;
    } else {
        speed_max = TD2P_FLEX_SPEED_MAX_DEFAULT;
    }

    /* Do not define a default speed_max_x or speed_max_y.
       These are optional. */
    if (speed_max_x > 42000) {
        speed_max_x = 127000;
    }

    if (speed_max_y > 42000) {
        speed_max_y = 127000;
    } 

    /*
     * Maximum number of ports depends on the max speed and
     * the device SKU (core bandwidth).
     */
    if (speed_max > 42000) {
        if (TRUE == si->flex_eligible){
            ports_max = 52;
        } else {
            ports_max = 50;
        }
    } else {
        soc_info_t *si = &SOC_INFO(unit);
        switch (si->bandwidth) {
        case 960000:
            ports_max = 52;
            break;
        case 720000:
            ports_max = 48;
            break;
        case 480000:
        default:
            ports_max = 42;
            break;
        }
    }
    /* Both X and Y Pipe have had their max flex speed defined, so use
       those values. */
    if (speed_max_x > 0 && speed_max_y > 0) {
        /* X Pipe */
        if (speed_max_x > 42000) {
            ports_max_x = 44;
        } else {
            soc_info_t *si = &SOC_INFO(unit);
            switch (si->bandwidth) {
            case 960000:
                ports_max_x = 52;
                break;
            case 720000:
                ports_max_x = 48;
                break;
            case 480000:
            default:
                ports_max_x = 42;
                break;
            }
        }

        /* Y Pipe */
        if (speed_max_y > 42000) {
            ports_max_y = 44;
        } else {
            soc_info_t *si = &SOC_INFO(unit);
            switch (si->bandwidth) {
            case 960000:
                ports_max_y = 52;
                break;
            case 720000:
                ports_max_y = 48;
                break;
            case 480000:
            default:
                ports_max_y = 42;
                break;
            }
        }
    } else {
        /* otherwise just set them to ports_max */
        ports_max_x = ports_max;
        ports_max_y = ports_max;
    }

    TD2P_INFO(unit)->speed_max = speed_max;
    TD2P_INFO(unit)->ports_pipe_max = ports_max;


    TD2P_INFO(unit)->speed_max_x = speed_max_x;
    TD2P_INFO(unit)->speed_max_y = speed_max_y;

    TD2P_INFO(unit)->ports_pipe_max_x = ports_max_x;
    TD2P_INFO(unit)->ports_pipe_max_y = ports_max_y;

    /*
     * Logical Port Numbers
     *
     * Some HW logical port tables (such as PORT_TABm) have 107 entries.
     * However, only 106 logical ports are supported (0..105).
     *
     * The additional entry is used for the CPU port to specify
     * properties depending on the type of traffic, Ethernet vs HG.
     * This has been the case for many generation of devices.
     *
     *     Logical Port   0       --> CPU (Ethernet)
     *                                This is used when CPU sends Ethernet
     *                                traffic.
     *     Logical Port   106     --> CPU (Higig)
     *                                This is used to specify properties
     *                                when the CPU sends HG traffic.
     *     Logical Port   105     --> Loopback LB
     *     Logical Ports  1..104  --> ANY physical front panel ports
     *                               (ANY Pipe)
     */
    TD2P_INFO(unit)->port_max = 105;

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_mmu_lossless_init
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
_soc_td2p_mmu_lossless_init(int unit)
{
    TD2P_INFO(unit)->mmu_lossless =
        soc_property_get(unit, spn_MMU_LOSSLESS, 1);

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_td2p_port_speed_init
 * Purpose:
 *      Initialize td2p_port_speed
 * Parameters:
 *      unit      - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumes SW data structure memory is valid.
 */
STATIC int
_soc_td2p_port_speed_init(int unit)
{
    soc_info_t *si = &SOC_INFO(unit);
    int port;

    if (td2p_port_speed[unit] == NULL) {
        td2p_port_speed[unit] = sal_alloc(sizeof(int) * SOC_MAX_NUM_PORTS,
                                          "td2p_port_speed");
        if (td2p_port_speed[unit] == NULL) {
            LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                      "Unable to allocate memory for td2p_port_speed")));
            return SOC_E_MEMORY;
        }
    }

    sal_memset(td2p_port_speed[unit], 0, sizeof(int) * SOC_MAX_NUM_PORTS);

    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        td2p_port_speed[unit][port] = si->port_speed_max[port];
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_td2p_port_sched_hsp_init
 * Purpose:
 *      Initialize td2p_port_sched_hsp. port_sched_hsp is a device init parameter
 *      that SHOULD NOT have any effect after flexing; once a port is flexed,
 *      port_sched_hsp should be set to 0
 * Parameters:
 *      unit      - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumes SW data structure memory is valid.
 */
STATIC int
_soc_td2p_port_sched_hsp_init(int unit)
{
    int port;

    if (td2p_port_sched_hsp[unit] == NULL) {
        td2p_port_sched_hsp[unit] = sal_alloc(sizeof(int) * SOC_MAX_NUM_PORTS,
                                          "td2p_port_sched_hsp");
        if (td2p_port_sched_hsp[unit] == NULL) {
            LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                      "Unable to allocate memory for td2p_port_sched_hsp")));
            return SOC_E_MEMORY;
        }
    }

    sal_memset(td2p_port_sched_hsp[unit], 0, sizeof(int) * SOC_MAX_NUM_PORTS);

    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        td2p_port_sched_hsp[unit][port] = 
                    soc_property_port_get(unit, port, 
                                         spn_PORT_SCHED_HSP, 0);
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_phy_info_init
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
 *      - Must be called after the TD2P port configuration init routine
 *        soc_trident2_port_config_init().
 */
int
soc_td2p_phy_info_init(int unit)
{
    int phy_port;    /* Physical port */
    int lanes;       /* Serdes lane */

    /* Allocate memory for main SW data structure */
    if (td2p_info[unit] == NULL) {
        td2p_info[unit] = sal_alloc(sizeof(td2p_info_t), "td2p_info");
        if (td2p_info[unit] == NULL) {
            LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "Unable to allocate memory for "
                                  "td2p_info")));
            return SOC_E_MEMORY;
        }
    }

    /* Clear data structure */
    sal_memset(td2p_info[unit], 0, sizeof(td2p_info_t));

    /* Initialize device information */
    SOC_IF_ERROR_RETURN(_soc_td2p_phy_port_lane_info_init(unit));

    SOC_IF_ERROR_RETURN(_soc_td2p_prio_mask_init(unit));

    SOC_IF_ERROR_RETURN(_soc_td2p_flex_enable_init(unit));

    SOC_IF_ERROR_RETURN(_soc_td2p_speed_valid_init(unit));

    SOC_IF_ERROR_RETURN(_soc_td2p_ports_pipe_max_init(unit));

    SOC_IF_ERROR_RETURN(_soc_td2p_mmu_lossless_init(unit));

    SOC_IF_ERROR_RETURN(_soc_td2p_port_speed_init(unit));

    SOC_IF_ERROR_RETURN(_soc_td2p_port_sched_hsp_init(unit));

    /* Debug output */
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "Physical ports max: %d  speed_max=%d "
                            "ports_per_pipe_max=%d port_max=%d "
                            "mmu_lossless=%d "
                            "flex_legacy=%d\n\n"),
                 TD2P_INFO(unit)->phy_ports_max,
                 TD2P_INFO(unit)->speed_max,
                 TD2P_INFO(unit)->ports_pipe_max,
                 TD2P_INFO(unit)->port_max,
                 TD2P_INFO(unit)->mmu_lossless,
                 TD2P_INFO(unit)->flex_legacy));

    for (phy_port = 0; phy_port < TD2P_INFO(unit)->phy_ports_max; phy_port++) {
        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "Phy port=%d pgw=%d xlp=%d index=%d "
                                "valid=0x%x pipe=%d "
                                "prio_mask=0x%x flex=%d\n"),
                     phy_port,
                     TD2P_PHY_PORT_LANE(unit, phy_port).pgw,
                     TD2P_PHY_PORT_LANE(unit, phy_port).xlp,
                     TD2P_PHY_PORT_LANE(unit, phy_port).port_index,
                     TD2P_PHY_PORT(unit, phy_port).lanes_valid,
                     TD2P_PHY_PORT(unit, phy_port).pipe,
                     TD2P_PHY_PORT(unit, phy_port).prio_mask,
                     TD2P_PHY_PORT(unit, phy_port).flex));
        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "    sisters_ports={%d,%d,%d,%d}\n"),
                     TD2P_PHY_PORT(unit, phy_port).sisters_ports[0],
                     TD2P_PHY_PORT(unit, phy_port).sisters_ports[1],
                     TD2P_PHY_PORT(unit, phy_port).sisters_ports[2],
                     TD2P_PHY_PORT(unit, phy_port).sisters_ports[3]));
    }

    for (lanes = 0; lanes <= SOC_PORT_RESOURCE_LANES_MAX; lanes++) {
        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "Serdes lane=%d port_rate=0x%x\n"),
                     lanes, TD2P_INFO(unit)->speed_valid[lanes]));
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_td2p_port_oversub_get
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
int
soc_td2p_port_oversub_get(int unit, int phy_port, soc_port_t logical_port,
                          int *oversub)
{
    soc_pbmp_t pbmp;

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

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_phy_port_flex_valid
 * Purpose:
 *      Check that FlexPort operation is valid on given physical port.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      phy_port  - (IN) Physical port number.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_td2p_phy_port_flex_valid(int unit, int phy_port)
{
    if (!TD2P_INFO(unit)->flex_legacy &&
        !TD2P_PHY_PORT(unit, phy_port).flex) {
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
 *      soc_td2p_is_flex_enable
 * Purpose:
 *      Check that FlexPort operation is valid on the unit.
 *      Checks for both legacy and new flex operation.
 * Parameters:
 *      unit      - (IN) Unit number.
 * Returns:
 *      TRUE or FALSE.
 */
int
soc_td2p_is_flex_enable(int unit)
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
 *      soc_td2p_port_lane_config_get
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
soc_td2p_port_lane_config_get(int unit, int phy_port, int *mode_lane,
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
     * This matches TD2+ port init soc_trident2_port_config_init().
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
    if (!sal_strcasecmp(str, TD2P_100GE_MLD_MAP_4_4_2_STR)) {
        *mode_lane = SOC_LANE_CONFIG_100G_4_4_2;
    } else if (!sal_strcasecmp(str, TD2P_100GE_MLD_MAP_3_4_3_STR)) {
        *mode_lane = SOC_LANE_CONFIG_100G_3_4_3;
    } else if (!sal_strcasecmp(str, TD2P_100GE_MLD_MAP_2_4_4_STR)) {
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
 *      soc_td2p_port_autoneg_core_get 
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
soc_td2p_port_autoneg_core_get(int unit, int phy_port, int *an_core,
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
 *      _soc_td2p_speed_valid
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
_soc_td2p_speed_valid(int unit, int phy_port, int lanes, int speed)
{
    uint32 speed_mask = 0;
    int speed_max;

    SOC_IF_ERROR_RETURN
        (soc_td2p_port_resource_speed_max_get(unit, &speed_max));

    if (speed > speed_max) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Invalid speed configuration for "
                              "physical_port=%d, speed=%d, max=%d\n"),
                   phy_port, speed, speed_max));
        return SOC_E_CONFIG;
    }

    if (speed == 1000) {
        speed_mask = TD2P_PHY_PORT_RATE_1;
    } else if (speed == 2500) {
        speed_mask = TD2P_PHY_PORT_RATE_2_5;
    } else if (speed == 10000) {
        speed_mask = TD2P_PHY_PORT_RATE_10;
    } else if (speed == 20000) {
        speed_mask = TD2P_PHY_PORT_RATE_20;
    } else if (speed == 40000) {
        speed_mask = TD2P_PHY_PORT_RATE_40;
    } else if (speed == 100000) {
        speed_mask = TD2P_PHY_PORT_RATE_100;
    } else if (speed == 11000) {
        speed_mask = TD2P_PHY_PORT_RATE_11;
    } else if (SOC_IS_TITAN2PLUS(unit) && (speed == 13000)) {
        speed_mask = TD2P_PHY_PORT_RATE_13;
    } else if (SOC_IS_TITAN2PLUS(unit) && (speed == 16000)) {
        speed_mask = TD2P_PHY_PORT_RATE_16;
    } else if (speed == 21000) {
        speed_mask = TD2P_PHY_PORT_RATE_21;
    } else if (speed == 42000) {
        speed_mask = TD2P_PHY_PORT_RATE_42;
    } else if (speed == 106000) {
        speed_mask = TD2P_PHY_PORT_RATE_106;
    } else if (speed == 127000) {
        speed_mask = TD2P_PHY_PORT_RATE_127;
    } else {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Invalid speed for "
                              "physical_port=%d, speed=%d, max=%d\n"),
                   phy_port, speed, speed_max));
        return SOC_E_CONFIG;
    }

    if (!(TD2P_INFO(unit)->speed_valid[lanes] & speed_mask)) {
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
_soc_td2p_phy_port_lanes_valid(int unit, int phy_port, int lanes)
{
    uint32 lane_mask = 0;

    if (lanes == 1) {
        lane_mask = TD2P_PHY_PORT_LANES_1;
    } else if (lanes == 2) {
        lane_mask = TD2P_PHY_PORT_LANES_2;
    } else if (lanes == 4) {
        lane_mask = TD2P_PHY_PORT_LANES_4;
    } else if (lanes == 10) {
        lane_mask = TD2P_PHY_PORT_LANES_10;
    } else if (lanes == 12) {
        lane_mask = TD2P_PHY_PORT_LANES_12;
    } else {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Invalid number of lanes for "
                              "physical_port=%d, lanes=%d\n"),
                   phy_port, lanes));
        return SOC_E_CONFIG;
    }

    if (!(TD2P_PHY_PORT(unit, phy_port).lanes_valid & lane_mask)) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Invalid lane configuration for "
                              "physical_port=%d, lane=%d, "
                              "valid_lanes=%s %s %s  %s %s\n"),
                   phy_port, lanes,
                   (TD2P_PHY_PORT(unit, phy_port).lanes_valid &
                    TD2P_PHY_PORT_LANES_1) ? "1" : "",
                   (TD2P_PHY_PORT(unit, phy_port).lanes_valid &
                    TD2P_PHY_PORT_LANES_2) ? "2" : "",
                   (TD2P_PHY_PORT(unit, phy_port).lanes_valid &
                    TD2P_PHY_PORT_LANES_4) ? "4" : "",
                   (TD2P_PHY_PORT(unit, phy_port).lanes_valid &
                    TD2P_PHY_PORT_LANES_10) ? "10" : "",
                   (TD2P_PHY_PORT(unit, phy_port).lanes_valid &
                    TD2P_PHY_PORT_LANES_12) ? "12" : ""));

        return SOC_E_CONFIG;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_td2p_port_lanes_valid
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
soc_td2p_port_lanes_valid(int unit, soc_port_t port, int lanes)
{
    soc_info_t *si = &SOC_INFO(unit);
    int phy_port;

    TD2P_INFO_INIT_CHECK(unit);

    phy_port = si->port_l2p_mapping[port];
    if (phy_port == -1) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Invalid physical port for logical port %d\n"),
                   port));
        return SOC_E_PORT;
    }

    return _soc_td2p_phy_port_lanes_valid(unit, phy_port, lanes);
}


/*
 * Function:
 *      soc_td2p_port_resource_speed_max_get
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
soc_td2p_port_resource_speed_max_get(int unit, int *speed)
{
    int speed_max_x, speed_max_y;
    TD2P_INFO_INIT_CHECK(unit);

    speed_max_x = TD2P_INFO(unit)->speed_max_x;
    speed_max_y = TD2P_INFO(unit)->speed_max_y;

    if (speed_max_x > 0 && speed_max_y > 0) {
        *speed = (speed_max_x > speed_max_y) ? speed_max_x : speed_max_y;
    } else {
        *speed = TD2P_INFO(unit)->speed_max;
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_port_resource_speed_max_x_get
 * Purpose:
 *      Get the maximum allowed speed in the x pipe.
 *
 *      This information is used to determine the total number of
 *      logical  ports allowed in the pipe as well as the
 *      pre-allocation of MMU port numbers.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      speed - (OUT) Maximum speed of any port in pipe.
 * Returns:
 *      SOC_E_XXX
 * Note:
 */
int
soc_td2p_port_resource_speed_max_x_get(int unit, int *speed)
{
    TD2P_INFO_INIT_CHECK(unit);

    if(speed == NULL) {
        return SOC_E_PARAM;
    }

    *speed = TD2P_INFO(unit)->speed_max_x;

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_port_resource_speed_max_y_get
 * Purpose:
 *      Get the maximum allowed speed in the y pipe.
 *
 *      This information is used to determine the total number of
 *      logical  ports allowed in the pipe as well as the
 *      pre-allocation of MMU port numbers.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      speed - (OUT) Maximum speed of any port in pipe.
 * Returns:
 *      SOC_E_XXX
 * Note:
 */
int
soc_td2p_port_resource_speed_max_y_get(int unit, int *speed)
{
    TD2P_INFO_INIT_CHECK(unit);

    if(speed == NULL) {
        return SOC_E_PARAM;
    }

    *speed = TD2P_INFO(unit)->speed_max_y;

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_td2p_port_addressable
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
soc_td2p_port_addressable(int unit, soc_port_t port)
{
    TD2P_INFO_INIT_CHECK(unit);

    if ((port < 0) ||
        (port > TD2P_INFO(unit)->port_max)) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Invalid logical port number %d. "
                              "Valid logical ports are %d to %d.\n"),
                   port, 0, TD2P_INFO(unit)->port_max));
        return SOC_E_PORT;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_td2p_phy_port_addressable
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
soc_td2p_phy_port_addressable(int unit, int phy_port)
{
    TD2P_INFO_INIT_CHECK(unit);

    if ((phy_port < 0) ||
        (phy_port >= TD2P_INFO(unit)->phy_ports_max)) {
        return SOC_E_PORT;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_td2p_phy_port_pgw_info_get
 * Purpose:
 *      Provide the PGW information for given physical port.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      phy_port    - (IN) Physical port number.
 *      pgw         - (OUT) PGW instance where port resides.
 *      xlp         - (OUT) XLP number within PGW
 *      port_index  - (OUT) Port index within XLP 
 * Returns:
 *      SOC_E_XXX
 */
int
soc_td2p_phy_port_pgw_info_get(int unit, int phy_port,
                               int *pgw, int *xlp, int *port_index)
{
    TD2P_INFO_INIT_CHECK(unit);

    if (phy_port == -1) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Invalid physical port %d\n"),
                   phy_port));
        return SOC_E_PORT;
    }

    *pgw = TD2P_PHY_PORT_LANE(unit, phy_port).pgw;
    *xlp = TD2P_PHY_PORT_LANE(unit, phy_port).xlp;
    *port_index = TD2P_PHY_PORT_LANE(unit, phy_port).port_index;

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_port_macro_first_phy_port_get
 * Purpose:
 *      This function returns the first physical port associated with a TSC-4
 * Parameters:
 *      unit     - (IN) Unit number
 *      phy_port - (IN) Physical port number
 *      first_phy_port - (OUT) First physical port on a TSC-4
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      SW data structure(td2p_info) must have been initialized before calling
 *      this function
 */
int
soc_td2p_port_macro_first_phy_port_get(int unit, int phy_port,
                                       int *first_phy_port)
{

    TD2P_INFO_INIT_CHECK(unit);

    if (!TD2P_PHY_PORT_ADDRESSABLE(unit, phy_port)) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit, "1st phy port get: "
                              "Invalid physical port %d\n"), phy_port));
        return SOC_E_PORT;
    }

    *first_phy_port = TD2P_PHY_PORT(unit, phy_port).sisters_ports[0];

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_td2p_subsidiary_ports_get
 * Purpose:
 *      This function returns the ancillary ports associated with a given
 *      master/controlling port. If the port is not a master/controlling port,
 *      an error is returned
 * Parameters:
 *      unit     - (IN) Unit number
 *      phy_port - (IN) logical port number
 *      pbmp -     (OUT) Set of ancillary ports for the given master port
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      This function is specific to flexport feature only.
 *      SW data structure(td2p_info. Used via TD2P_* macros below) must have
 *      been initialized before calling this function
 */
int
soc_td2p_subsidiary_ports_get(int unit, soc_port_t port, soc_pbmp_t *pbmp)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_t phy_port_base, phy_port;
    soc_pbmp_t pbm;
    int i;

    TD2P_INFO_INIT_CHECK(unit);

    if (!TD2P_PHY_PORT_ADDRESSABLE(unit, port) || IS_CPU_PORT(unit, port) ||
        IS_LB_PORT(unit, port) ||
        SOC_PBMP_MEMBER(PBMP_MANAGEMENT(unit), port)) {
        /* CPU, LB and Management ports do not support flexport */
        return SOC_E_PORT;
    }

    phy_port = si->port_l2p_mapping[port];
    if (phy_port == -1) {
        return SOC_E_PORT;
    }

    phy_port_base = TD2P_PHY_PORT(unit, phy_port).sisters_ports[0];

    /* Only lane 0 and lane 2 of a PM can be controlling ports */
    if ((phy_port_base != phy_port) && ((phy_port_base + 2) != phy_port)) {
        /* Not a controlling port */
        return SOC_E_PORT;
    }

    /* Set starting index in to sisters_ports array */
    if (phy_port == phy_port_base) {
        i = 0;
    } else if (phy_port == (phy_port_base + 2)) {
        i = 2;
    } else {
        return SOC_E_PORT;
    }

    SOC_PBMP_CLEAR(pbm);
    for (; i < _TD2_PORTS_PER_XLP; i++) {
        /* Skip physical ports which have no logical mapping */
        if (si->port_p2l_mapping[TD2P_PHY_PORT(unit, phy_port_base).sisters_ports[i]] == -1) {
            continue;
        }

        SOC_PBMP_PORT_ADD(pbm,
          si->port_p2l_mapping[TD2P_PHY_PORT(unit, phy_port_base).sisters_ports[i]]);
    }
    SOC_PBMP_ASSIGN(*pbmp, pbm);

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_logic_ports_max_validate
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
int
soc_td2p_logic_ports_max_validate(int unit, portmod_pbmp_t phy_pbmp)
{
    int phy_port;
    int ports_pipe_max, ports_pipe_max_x, ports_pipe_max_y;
    int speed_max_x, speed_max_y;
    int x_pipe_count = 0;
    int y_pipe_count = 0;

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- VALIDATE: Maximum number logical ports\n")));

    /* Get maximum number of logical ports allowed per pipeline */
    ports_pipe_max = TD2P_INFO(unit)->ports_pipe_max;
    ports_pipe_max_x = TD2P_INFO(unit)->ports_pipe_max_x;
    ports_pipe_max_y = TD2P_INFO(unit)->ports_pipe_max_y;

    speed_max_x = TD2P_INFO(unit)->speed_max_x;
    speed_max_y = TD2P_INFO(unit)->speed_max_y;

    /* Check number of logical ports do not exceed maximum allowed */
    for (phy_port = 0; phy_port < SOC_MAX_NUM_PORTS; phy_port++) {
        /* Skip CPU, Loopback */
        if ((phy_port == TD2P_PHY_PORT_CPU) ||
            (phy_port == TD2P_PHY_PORT_LB)) {
            continue;
        }

        /* Skip physical ports not mapped */
        /* coverity[overrun-local] */
        if (!PORTMOD_PBMP_MEMBER(phy_pbmp, phy_port)) {
            continue;
        }

        if (TD2P_PHY_PORT(unit, phy_port).pipe == TD2P_X_PIPE) {
            x_pipe_count++;
        } else {
            y_pipe_count++;
        }
    }

    /* If the max flex speed has been set per pipe (for both X and Y)
       then do an individual pipe comparison to see if either pipe
       has exceeded its maximum rather than using the device wide
       comparison. */
    if (speed_max_x > 0 && speed_max_y > 0) {
        if ((x_pipe_count > ports_pipe_max_x) ||
            (y_pipe_count > ports_pipe_max_y)) {
            LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "Number of logical ports exceeds "
                                  "max allowed: x_pipe_count=%d "
                                  "y_pipe_count=%d max_pipe_count_x=%d "
                                  "max_pipe_count_y=%d\n"),
                       x_pipe_count, y_pipe_count, ports_pipe_max_x,
                       ports_pipe_max_y));

            return SOC_E_RESOURCE;
        }
    } else {
        if ((x_pipe_count > ports_pipe_max) ||
            (y_pipe_count > ports_pipe_max)) {
            LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "Number of logical ports exceeds "
                                  "max allowed: x_pipe_count=%d "
                                  "y_pipe_count=%d max_pipe_count=%d\n"),
                       x_pipe_count, y_pipe_count, ports_pipe_max));

            return SOC_E_RESOURCE;
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "Number of logical ports: "
                            "x_pipe_count=%d y_pipe_count=%d "
                            "max_pipe_count=%d\n"),
                 x_pipe_count, y_pipe_count, ports_pipe_max));

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_port_resource_tdm_config_validate
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
_soc_td2p_port_resource_tdm_config_validate(int unit,
                                            int nport,
                                            soc_port_resource_t *resource,
                                            portmod_pbmp_t phy_pbmp)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *pr;
    int xlport_mask[_TD2_PGWS_PER_DEV][_TD2_XLPS_PER_PGW];
    int xlport_validated[_TD2_PGWS_PER_DEV][_TD2_XLPS_PER_PGW];
    int pgw;
    int xlp;
    int port_index;
    int phy_port;
    int i;

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- VALIDATE: TDM configuration\n")));

    /* Clear data */
    for (pgw = 0; pgw < _TD2_PGWS_PER_DEV; pgw++) {
        for (xlp = 0; xlp < _TD2_XLPS_PER_PGW; xlp++) {
            xlport_mask[pgw][xlp] = 0x0;
            xlport_validated[pgw][xlp] = 0;
        }
    }

    /*
     * Determine configuration on each XLPORT (PortMacro or TSC-4)
     *
     * A bit set means physical port is defined (mapped).
     */
    for (phy_port = 0; phy_port < SOC_MAX_NUM_PORTS; phy_port++) {
        pgw = TD2P_PHY_PORT_LANE(unit, phy_port).pgw;
        if (pgw == -1) {
            continue;
        }

        xlp = TD2P_PHY_PORT_LANE(unit, phy_port).xlp;
        port_index = TD2P_PHY_PORT_LANE(unit, phy_port).port_index;

        /* Check if physical port is mapped */
        /* coverity[overrun-local] */
        if (PORTMOD_PBMP_MEMBER(phy_pbmp, phy_port)) {
            xlport_mask[pgw][xlp] |= 1 << port_index;
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit, "Check Port Configuration block\n")));
    for (pgw = 0; pgw < _TD2_PGWS_PER_DEV; pgw++) {
        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit, "    PGW_%d:"), pgw));
        for (xlp = 0; xlp < _TD2_XLPS_PER_PGW; xlp++) {
            LOG_VERBOSE(BSL_LS_SOC_PORT,
                        (BSL_META_U(unit,
                                    " xlp_%d=0x%x"), xlp,
                         xlport_mask[pgw][xlp]));
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

        pgw = TD2P_PHY_PORT_LANE(unit, phy_port).pgw;
        xlp = TD2P_PHY_PORT_LANE(unit, phy_port).xlp;

        if (pgw == -1) {
            LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "Invalid PGW block -1 for "
                                  "physical port %d\n"),
                       phy_port));
            return SOC_E_INTERNAL;
        }

        if (xlport_validated[pgw][xlp]) {
            /* Configuration in block was already validated. skip */
            continue;
        }

        /*
         * Check that configuration is a valid TDM case
         *
         * If the XLPORT block has any physical ports used (mapped),
         * then at least the physical port on Lane 0 MUST be defined (mapped).
         */
        if ((xlport_mask[pgw][xlp] != 0x0) &&
            !(xlport_mask[pgw][xlp] & 0x1)) {
            LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "Invalid configuration on physical "
                                  "ports %d %d %d %d "
                                  "(pgw=%d xlp=%d physical_port_mask=0x%x)\n"),
                       TD2P_PHY_PORT(unit, phy_port).sisters_ports[0],
                       TD2P_PHY_PORT(unit, phy_port).sisters_ports[1],
                       TD2P_PHY_PORT(unit, phy_port).sisters_ports[2],
                       TD2P_PHY_PORT(unit, phy_port).sisters_ports[3],
                       pgw, xlp, xlport_mask[pgw][xlp]));
            LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "Physical port %d must be defined\n"),
                       TD2P_PHY_PORT(unit, phy_port).sisters_ports[0]));
            return SOC_E_CONFIG;
        }

        xlport_validated[pgw][xlp] = 1;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_port_mapping_validate
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
_soc_td2p_port_mapping_validate(int unit, 
                                int nport,
                                soc_port_resource_t *resource)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *pr;
    char phy_pfmt[SOC_PBMP_FMT_LEN];
    char logic_pfmt[SOC_PBMP_FMT_LEN];
    portmod_pbmp_t phy_pbmp;
    pbmp_t phy_pbmp_tmp;
    pbmp_t logic_pbmp;
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
        if (!TD2P_PHY_PORT_ADDRESSABLE(unit, phy_port) ||
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
        (soc_td2p_logic_ports_max_validate(unit, phy_pbmp));


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
         * For 100GE or more, we need to check all 12 lanes
         * even though only 10 lanes are actually used.
         */
        num_lanes = pr->num_lanes;
        if (pr->speed >= 100000) {
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
        (_soc_td2p_port_resource_tdm_config_validate(unit,
                                                     nport, resource,
                                                     phy_pbmp));

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_port_resource_oversub_validate
 * Purpose:
 *      Validate Oversubscription mode:
 *      - Ports with speed 100GE or more are not in oversubscription mode.
 *      - Line rate or Oversub is the same in all ports within the same
 *        Port Macro (non-mixed mode in sister ports).
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 *      post_si  - (IN) SOC_INFO data after FlexPort.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      - Resource is not NULL.
 *      - Assumes array order is correct
 *        ('delete' operations are first in array).
 */
STATIC int
_soc_td2p_port_resource_oversub_validate(int unit,
                                         int nport,
                                         soc_port_resource_t *resource,
                                         soc_info_misc_t *post_si)
{
    soc_port_resource_t *pr;
    int oversub;
    int i;
    int j;
    int logic_port;
    int phy_port;
    int sister_port;
    int sister_oversub;
    int rv;

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- VALIDATE: Oversubscription mode\n")));

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        phy_port = pr->physical_port;

        /* Skip delete entries */
        if (phy_port == -1) {
            continue;
        }

        /* Get oversubscription mode */
        rv = soc_td2p_port_oversub_get(unit, phy_port, pr->logical_port,
                                       &oversub);
        if (SOC_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "Could not get oversubscription mode for "
                                  "logical_port=%d physical_port=%d\n"),
                       pr->logical_port, pr->physical_port));
            return rv;
        }

        /* Check Oversub is not set on port speeds >= 100GE */
        if ((pr->speed >= 100000) && (oversub)) {
            LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "Oversubscription mode cannot be "
                                  "configured on "
                                  "ports with speed 100 GE or beyond, "
                                  "logical_port=%d physical_port=%d "
                                  "speed=%d\n"),
                       pr->logical_port, pr->physical_port,
                       pr->speed));
            return SOC_E_CONFIG;
        }


        /*
         * Check that ports within a Port Macro (sister ports) are
         * all configured to either Line Rate or Oversub mode (no mixed mode)
         */
        for (j = 0; j < _TD2_PORTS_PER_XLP; j++) {
            sister_port = TD2P_PHY_PORT(unit, phy_port).sisters_ports[j];
            logic_port = post_si->port_p2l_mapping[sister_port];

            if (logic_port < 0) {
                continue;  /* Skip if port is not defined */
            }
            if (SOC_PBMP_MEMBER(post_si->disabled_bitmap, logic_port)) {
                continue;  /* Skip inactive ports */
            }

            if (SOC_PBMP_MEMBER(post_si->oversub_pbm, logic_port)) {
                sister_oversub = 1;
            } else {
                sister_oversub = 0;
            }
            if (oversub != sister_oversub) {
                LOG_ERROR(BSL_LS_SOC_PORT,
                          (BSL_META_U(unit,
                                      "Oversubscription mode must be same "
                                      "on physical ports %d %d %d %d\n"),
                           TD2P_PHY_PORT(unit, phy_port).sisters_ports[0],
                           TD2P_PHY_PORT(unit, phy_port).sisters_ports[1],
                           TD2P_PHY_PORT(unit, phy_port).sisters_ports[2],
                           TD2P_PHY_PORT(unit, phy_port).sisters_ports[3]));
                LOG_ERROR(BSL_LS_SOC_PORT,
                          (BSL_META_U(unit,
                                      "Mismatch: \n"
                                      "logical_port=%d physical_port=%d "
                                      "oversub=%d\n"
                                      "logical_port=%d physical_port=%d "
                                      "oversub=%d\n"),
                           pr->logical_port, pr->physical_port, oversub,
                           logic_port, sister_port, sister_oversub));

                return SOC_E_CONFIG;
            }
        }
    }

    return SOC_E_NONE;
}

#define SOC_TD2P_SPD_CLS_UNKNOWN   (0)
#define SOC_TD2P_SPD_CLS_LR_10G    (1)
#define SOC_TD2P_SPD_CLS_LR_20G    (2)
#define SOC_TD2P_SPD_CLS_LR_40G    (3)
#define SOC_TD2P_SPD_CLS_LR_100G   (4)
#define SOC_TD2P_SPD_CLS_LR_HG127G (5)
#define SOC_TD2P_SPD_CLS_OVS_10G   (6)
#define SOC_TD2P_SPD_CLS_OVS_20G   (7)
#define SOC_TD2P_SPD_CLS_OVS_40G   (8)
/*
 * Function:
 *      _soc_td2p_port_speed_class_get
 * Purpose:
 *      Get port speed class
 *
 * Parameters:
 *      unit     - (IN) Unit number
 *      speed    - (IN) Port speed
 *      oversub  - (IN) Oversubscription
 *      spd_cls  - (OUT) Speed class
 * Returns:
 *      SOC_E_XXX
 *
 * Note:
 *     A total of 5 line-rate port speed classes (10G, 20G, 40G, 100G, HG[127])
 *     A total of 3 oversub port speed classes (10G, 20G, 40G)
 */
STATIC int
_soc_td2p_port_speed_class_get(int unit, int speed, int oversub, int *spd_cls)
{
    int rv = SOC_E_NONE;
    *spd_cls = SOC_TD2P_SPD_CLS_UNKNOWN;
    if (!oversub) {
        switch(speed) {
        case 10:
        case 100:
        case 1000:
        case 2500:
        case 10000:
        case 11000:
            *spd_cls = SOC_TD2P_SPD_CLS_LR_10G;
            break;
        case 13000:
        case 16000:
            if (SOC_IS_TITAN2PLUS(unit)) {
                *spd_cls = SOC_TD2P_SPD_CLS_LR_20G;
            } else {
                *spd_cls = SOC_TD2P_SPD_CLS_UNKNOWN;
                rv = SOC_E_FAIL;
            }
            break;
        case 20000:
        case 21000:
            *spd_cls = SOC_TD2P_SPD_CLS_LR_20G;
            break;
        case 30000:
        case 40000:
        case 42000:
            *spd_cls = SOC_TD2P_SPD_CLS_LR_40G;
            break;
        case 100000:
        case 106000:
            *spd_cls = SOC_TD2P_SPD_CLS_LR_100G;
            break;
        case 120000:
        case 127000:
            *spd_cls = SOC_TD2P_SPD_CLS_LR_HG127G;
            break;
        default:
            *spd_cls = SOC_TD2P_SPD_CLS_UNKNOWN;
            rv = SOC_E_FAIL;
            break;
        }
    } else {
        switch(speed) {
        case 10:
        case 100:
        case 1000:
        case 2500:
        case 10000:
        case 11000:
            *spd_cls = SOC_TD2P_SPD_CLS_OVS_10G;
            break;
        case 13000:
        case 16000:
            if (SOC_IS_TITAN2PLUS(unit)) {
                *spd_cls = SOC_TD2P_SPD_CLS_LR_20G;
            } else {
                *spd_cls = SOC_TD2P_SPD_CLS_UNKNOWN;
                rv = SOC_E_FAIL;
            }
            break;
        case 20000:
        case 21000:
            *spd_cls = SOC_TD2P_SPD_CLS_OVS_20G;
            break;
        case 30000:
        case 40000:
        case 42000:
            *spd_cls = SOC_TD2P_SPD_CLS_OVS_40G;
            break;
        case 100000:
        case 106000:
        case 120000:
        case 127000:
        default:
            *spd_cls = SOC_TD2P_SPD_CLS_UNKNOWN;
            rv = SOC_E_FAIL;
            break;
        }
    }

    LOG_VERBOSE(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                "speed=%d, speed_class=%d\n"),
                speed, *spd_cls));

    return rv;
}

/*
 * Function:
 *      _soc_td2p_port_resource_mixed_sisters_validate
 * Purpose:
 *      Validate mixed speed in sister ports:
 *      - Mixed-sister line rate ports are allowed.
 *      - Mixed-sister oversub ports are NOT allowed.
 *        All active ports in a oversub Port Macro (TSC-4) must
 *        be the same speed.
 *
 *      Different speed ports in the same Port Macro (TSC-4) are referred
 *      to as mixed-sister ports.
 *
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 *      post_si  - (IN) SOC_INFO data after FlexPort.
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
_soc_td2p_port_resource_mixed_sisters_validate(int unit,
                                               int nport,
                                               soc_port_resource_t *resource,
                                               soc_info_misc_t *post_si)
{
    soc_port_resource_t *pr;
    int i;
    int j;
    int logic_port;
    int phy_port, num_lanes, encap;
    int sister_port;
    int speed, sister_speed;
    int spd_cls, sister_spd_cls;
    uint32_t pll_div, sister_pll_div;

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- VALIDATE: Mixed speed sister ports\n")));

     for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        phy_port = pr->physical_port;
        speed = pr->speed;
        num_lanes = pr->num_lanes;
        encap = pr->encap;
        logic_port = pr->logical_port;

        /* Skip delete entries */
        if (phy_port == -1) {
            continue;
        }

        /*
         * Check that all sister ports have same VCO(PLL div) value.
         */
        SOC_IF_ERROR_RETURN(
            soc_esw_portctrl_pll_div_get(unit, logic_port, phy_port, speed, num_lanes,
                                         encap, &pll_div));

        for (j = 0; j < _TD2_PORTS_PER_XLP; j++) {
            sister_port = TD2P_PHY_PORT(unit, phy_port).sisters_ports[j];
            logic_port = post_si->port_p2l_mapping[sister_port];
            num_lanes = post_si->port_num_lanes[logic_port];
            encap = post_si->port_encap[logic_port];

            if (logic_port < 0) {
                continue;  /* Skip if port is not defined */
            }
            if (SOC_PBMP_MEMBER(post_si->disabled_bitmap, logic_port)) {
                continue;  /* Skip inactive ports */
            }

            sister_speed = post_si->port_speed[logic_port];
            SOC_IF_ERROR_RETURN(
                soc_esw_portctrl_pll_div_get(unit, logic_port, sister_port, sister_speed, num_lanes,
                                             encap, &sister_pll_div));
            if (pll_div != sister_pll_div) {
                LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                    "Pll div must be the same on sister physical ports "
                           "%d %d %d %d\n"),
                    TD2P_PHY_PORT(unit, phy_port).sisters_ports[0],
                    TD2P_PHY_PORT(unit, phy_port).sisters_ports[1],
                    TD2P_PHY_PORT(unit, phy_port).sisters_ports[2],
                    TD2P_PHY_PORT(unit, phy_port).sisters_ports[3]));
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
        SOC_IF_ERROR_RETURN(_soc_td2p_port_speed_class_get(unit,
                                speed, 1, &spd_cls));

        for (j = 0; j < _TD2_PORTS_PER_XLP; j++) {
            sister_port = TD2P_PHY_PORT(unit, phy_port).sisters_ports[j];
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

            SOC_IF_ERROR_RETURN(_soc_td2p_port_speed_class_get(unit,
                                     sister_speed, 1, &sister_spd_cls));
            if (spd_cls != sister_spd_cls) {
                LOG_ERROR(BSL_LS_SOC_PORT,
                          (BSL_META_U(unit,
                                      "Speed class must be the same on "
                                      "oversub sister physical ports "
                                      "%d %d %d %d\n"),
                           TD2P_PHY_PORT(unit, phy_port).sisters_ports[0],
                           TD2P_PHY_PORT(unit, phy_port).sisters_ports[1],
                           TD2P_PHY_PORT(unit, phy_port).sisters_ports[2],
                           TD2P_PHY_PORT(unit, phy_port).sisters_ports[3]));
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
 *      soc_td2p_port_mixed_sister_speed_validate
 * Purpose:
 *      Check all sister ports has same speed in oversub mode
 * Parameters:
 *      unit     - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumes that this function is called at early stage of initialization
 *      (For example, soc_trident2_port_config_init).
 *      Because portctrl/portmod API is not ready to use,
 *      use port_speed_max or port_init_speed instead.
 */
int
soc_td2p_port_mixed_sister_speed_validate(int unit)
{
    int i;
    int lport, sister_lport, phy_port, sister_port;
    int speed, sister_speed;
    int spd_cls, sister_spd_cls;
    soc_info_t *si = &SOC_INFO(unit);

    /* Loop for 1st physical port in PortMacro */
    for (phy_port = 1; phy_port < TD2P_INFO(unit)->phy_ports_max - 1;
         phy_port += _TD2_PORTS_PER_XLP) {

         lport = si->port_p2l_mapping[phy_port];
         if (lport < 0) {
             continue;
         }

         /* Skip Line Rate ports
            Assumes mixed oversub mode is already checked */
         if (!SOC_PBMP_MEMBER(si->oversub_pbm, lport)) {
             continue;
         }

         /*
          * Check that Oversub sister ports (ports within a Port Macro)
          * have the same speed class
          */
         speed = soc_property_port_get(unit, lport, spn_PORT_INIT_SPEED,
                                       si->port_speed_max[lport]);

         SOC_IF_ERROR_RETURN(_soc_td2p_port_speed_class_get(unit,
                                 speed, 1, &spd_cls));

         for (i = 1; i < _TD2_PORTS_PER_XLP; i++) {
             sister_port = TD2P_PHY_PORT(unit, phy_port).sisters_ports[i];
             sister_lport = si->port_p2l_mapping[sister_port];
             if (sister_lport < 0) {
                 continue;
             }

             if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, sister_lport)) {
                 continue;
             }

             sister_speed = soc_property_port_get(unit, sister_lport,
                                spn_PORT_INIT_SPEED,
                                si->port_speed_max[sister_lport]);
             SOC_IF_ERROR_RETURN(_soc_td2p_port_speed_class_get(unit,
                                     sister_speed, 1, &sister_spd_cls));
             if (spd_cls != sister_spd_cls) {
                 LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                           "Mixed port speed is not acceptable in oversub mode "
                           "on physical ports %d %d %d %d\n"),
                            TD2P_PHY_PORT(unit, phy_port).sisters_ports[0],
                            TD2P_PHY_PORT(unit, phy_port).sisters_ports[1],
                            TD2P_PHY_PORT(unit, phy_port).sisters_ports[2],
                            TD2P_PHY_PORT(unit, phy_port).sisters_ports[3]));
                 return SOC_E_CONFIG;
             }
         }
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_td2p_post_soc_info_get
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
_soc_td2p_post_soc_info_get(int unit,
                            int nport,
                            soc_port_resource_t *resource,
                            soc_info_misc_t *post_si)
{
    int rv;
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *pr;
    int i;
    int speed;
    int oversub;
    int logic_port;
    int phy_port;

    soc_trident2_soc_info_misc_t_init(unit, post_si);

    /* Update current speed */
    for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
        logic_port = i;
        phy_port = si->port_l2p_mapping[logic_port];

        /* Skip ports not defined, LB, CPU, etc. */
        /* coverity[overrun-local] */
        if (!SOC_PORT_ADDRESSABLE(unit, logic_port) ||
            !TD2P_PHY_PORT_ADDRESSABLE(unit, phy_port) ||
            IS_LB_PORT(unit, logic_port) ||
            IS_CPU_PORT (unit, logic_port)) {
            continue;
        }

        /* Get current port speed */
        rv = soc_esw_portctrl_speed_get(unit, logic_port, &speed);
        if (SOC_FAILURE(rv)) {
            speed = si->port_speed_max[logic_port];
            LOG_WARN(BSL_LS_SOC_PORT,
                     (BSL_META_U(unit,
                                 "Cannot get current port speed, "
                                 " use configured speed=%d "
                                 "logical_port=%d rv=%d\n"),
                      speed, logic_port, rv));
        }
        post_si->port_speed[logic_port] = speed;
    }

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

        rv = soc_td2p_port_oversub_get(unit, phy_port, logic_port, &oversub);
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
        if (!TD2P_PHY_PORT_ADDRESSABLE(unit, i)) {
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
 *      _soc_td2p_port_resource_validate_output
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
_soc_td2p_port_resource_validate_output(int unit,
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
 *      soc_td2p_port_resource_validate
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
soc_td2p_port_resource_validate(int unit, int nport,
                                soc_port_resource_t *resource)
{
    int i, rv = SOC_E_NONE;
    soc_port_resource_t *pr;
    soc_info_misc_t *post_si;
    int is_speed_set = FALSE;

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "\n=============================================\n"
                            "========= SOC PORT RESOURCE VALIDATE ========\n"
                            "=============================================\n"
                            )));
    _soc_td2p_port_resource_validate_output(unit, nport, resource);

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
    rv = _soc_td2p_post_soc_info_get(unit, nport, resource, post_si);
    if (SOC_FAILURE(rv)) {
        sal_free(post_si);
        return rv;
    }

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "=== VALIDATE ===\n")));

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- VALIDATE: Bandwidth\n")));
    rv = soc_trident2_port_bandwidth_validate(unit, post_si);
    if (SOC_FAILURE(rv)) {
        sal_free(post_si);
        return SOC_E_CONFIG;
    }

    /*
     * Check FlexPort enable, lanes and speed assignment
     */
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- VALIDATE: Port flex enable, lanes, speed\n")));
    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        /* Skip delete entries */
        if (pr->physical_port == -1) {
            continue;
        }

        if (!(pr->flags & SOC_PORT_RESOURCE_SPEED)) {
            /* Check if flex is enabled on port */
            rv = _soc_td2p_phy_port_flex_valid(unit, pr->physical_port);
            if (SOC_FAILURE(rv)) {
                sal_free(post_si);
                return rv;
            }

            /* Check lane assignment */
            rv = _soc_td2p_phy_port_lanes_valid(unit, pr->physical_port,
                                                pr->num_lanes);
            if (SOC_FAILURE(rv)) {
                sal_free(post_si);
                return rv;
            }
            /* Check speed (port rate) */
            rv = _soc_td2p_speed_valid(unit, pr->physical_port,
                                       pr->num_lanes, pr->speed);
            if (SOC_FAILURE(rv)) {
                sal_free(post_si);
                return rv;
            }
        } else {
            is_speed_set = TRUE;
        }
    }

    /* Check port mappings */
    if (is_speed_set != TRUE) {
        rv = _soc_td2p_port_mapping_validate(unit, nport, resource);
        if (SOC_FAILURE(rv)) {
            sal_free(post_si);
            return rv;
        }
    }

    /* Check oversubscription mode */
    rv = _soc_td2p_port_resource_oversub_validate(unit, nport, resource,
                                                  post_si);
    if (SOC_FAILURE(rv)) {
        sal_free(post_si);
        return rv;
    }

    /* Check mixed speed on sister ports */
    rv = _soc_td2p_port_resource_mixed_sisters_validate(unit, nport, resource,
                                                        post_si);
    if (SOC_FAILURE(rv)) {
        sal_free(post_si);
        return rv;
    }

    sal_free(post_si);
    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_port_resource_data_cleanup
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
_soc_td2p_port_resource_data_cleanup(soc_port_resource_t **pre_resource)
{
    if (*pre_resource != NULL) {
        sal_free(*pre_resource);
        *pre_resource = NULL;
    }

    return;
}


/*
 * Function:
 *      _soc_td2p_port_resource_mode_get
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
_soc_td2p_port_resource_mode_get(int unit,
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
    int num_lanes[_TD2_PORTS_PER_XLP];
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
        for (j = 0; j < _TD2_PORTS_PER_XLP; j++) {
            sister_port = TD2P_PHY_PORT(unit, phy_port).sisters_ports[j];
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
        for (j = 0; j < _TD2_PORTS_PER_XLP; j++) {
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
            pr->mode = SOC_TD2_PORT_MODE_SINGLE;
        } else if (one_active_lane && !two_active_lane) {
            pr->mode = SOC_TD2_PORT_MODE_QUAD;
        } else if (!one_active_lane && two_active_lane) {
            pr->mode = SOC_TD2_PORT_MODE_DUAL;
        } else if (one_active_lane && two_active_lane) {
            if (num_lanes[0] == 1) {
                pr->mode = SOC_TD2_PORT_MODE_TRI_012;
            } else {
                pr->mode = SOC_TD2_PORT_MODE_TRI_023;
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
 *      _soc_td2p_port_resource_output
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
_soc_td2p_port_resource_output(int unit, char *header,
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
                            "Logical Physical MMU Pipe  "
                            "Speed Lanes Mode Ovs PriMsk Flags       "
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
         * certain values on the Port Modes defined in soc_td2_port_mode_e.
         */
        if ((pr->mode < 0) ||  (pr->mode > 4)) {
            mode_str = "----";
        } else {
            mode_str = modes_str[pr->mode];
        }

        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "%3d  %s   "
                                "%6d  %2d   %4s  %1d  0x%4.4x 0x%8.8x"),
                     pr->mmu_port, ((pr->pipe == TD2P_X_PIPE) ? "X" : "Y"),
                     pr->speed, pr->num_lanes, mode_str,
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
                         pr->lane_info[lane]->xlp,
                         pr->lane_info[lane]->port_index));
            }
        }
    }

    return;
}


/*
 * Function:
 *      _soc_td2p_port_resource_data_output
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
_soc_td2p_port_resource_data_output(int unit,
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
    _soc_td2p_port_resource_output(unit,
                                   "=== FlexPort Port Resource Data ===",
                                   nport, resource);
    
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit, "\n")));
    _soc_td2p_port_resource_output(unit,
                                   "=== Pre-FlexPort Port Resource Data ===",
                                   pre_count, pre_resource);

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit, "\n")));
    _soc_td2p_port_resource_output(unit,
                                   "=== Post-FlexPort Port Resource Data ===",
                                   post_count, post_resource);

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit, "\n")));

    return;
}


/*
 * Function:
 *      _soc_td2p_port_resource_data_init
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
_soc_td2p_port_resource_data_init(int unit,
                                  int nport, soc_port_resource_t *resource,
                                  int *pre_count,
                                  soc_port_resource_t **pre_resource,
                                  int *post_count,
                                  soc_port_resource_t **post_resource,
                                  soc_td2p_info_t *pre_soc_info)
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

    /* Validate and get MMU port assignments */
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- VALIDATE: MMU ports allocation\n")));
    SOC_IF_ERROR_RETURN
        (soc_td2p_mmu_flexport_map_validate(unit, nport, resource));

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
        pr->pipe = TD2P_PHY_PORT(unit, phy_port).pipe;
        pr->prio_mask = TD2P_PHY_PORT(unit, phy_port).prio_mask;
        SOC_IF_ERROR_RETURN
            (soc_td2p_port_oversub_get(unit, phy_port, pr->logical_port,
                                       &pr->oversub));

        /*
         * Get Lanes information
         * Assume lanes are arranged consecutively with respect to
         * the physical port number.
         */
        for (lane = 0; lane < pr->num_lanes; lane++) {
            pr->lane_info[lane] = &TD2P_PHY_PORT_LANE(unit, phy_port + lane);
        }
    }

    /*
     * Get port mode (quad, dual, single, tri) into 'resource' array.
     */
    SOC_IF_ERROR_RETURN
        (_soc_td2p_port_resource_mode_get(unit,
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
        pre_pr->pipe = TD2P_PHY_PORT(unit, phy_port).pipe;
        pre_pr->num_lanes = si->port_num_lanes[pr->logical_port];
        pre_pr->prio_mask = TD2P_PHY_PORT(unit, phy_port).prio_mask;
        pre_pr->oversub =
            SOC_PBMP_MEMBER(si->oversub_pbm, pre_pr->logical_port) ? 1 : 0;

        SOC_IF_ERROR_RETURN
            (soc_esw_portctrl_speed_get(unit, pre_pr->logical_port,
                                        &pre_pr->speed));

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
                &TD2P_PHY_PORT_LANE(unit, phy_port + lane);
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
    SOC_PBMP_ASSIGN(pre_soc_info->ypipe_pbm, si->ypipe_pbm);
    SOC_PBMP_ASSIGN(pre_soc_info->oversub_pbm, si->oversub_pbm);

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_soc_info_ptype_update
 * Purpose:
 *      Update the SOC_INFO ptype information and port names based
 *      on the port type bitmaps.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      op    - (IN) FlexPort operation SOC_TD2P_PORT_RESOURCE_OP_...
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      Assumes linkscan is paused, COUNTER_LOCK and SOC_CONTROL_LOCK
 *      locks are taken.
 */
STATIC int
_soc_td2p_soc_info_ptype_update(int unit, int op)
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
    RECONFIGURE_PORT_TYPE_INFO(c);
    RECONFIGURE_PORT_TYPE_INFO(ce);
    RECONFIGURE_PORT_TYPE_INFO(xl);
    RECONFIGURE_PORT_TYPE_INFO(gx);
    RECONFIGURE_PORT_TYPE_INFO(ether);
    RECONFIGURE_PORT_TYPE_INFO(hg);
    RECONFIGURE_PORT_TYPE_INFO(st);
    /*
     * In legacy speed set oparation soc ptype port and all will
     * not be changed, there is no need to update.
     */
    if (SOC_TD2P_PORT_RESOURCE_OP_SPEED != op) {
        RECONFIGURE_PORT_TYPE_INFO(port);
        RECONFIGURE_PORT_TYPE_INFO(all);
    }

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
    for (phy_port = 0; phy_port < TD2P_INFO(unit)->phy_ports_max;
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
 *      _soc_td2p_soc_info_ptype_ports_add
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
_soc_td2p_soc_info_ptype_ports_add(int unit,
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
    int orig_blktype;
    int port_idx;
    int op = 0;

    /*
     * Add to port type bitmaps and block information
     */
    for (i = 0, pr = &post_resource[0]; i < post_count; i++, pr++) {

        logic_port = pr->logical_port;
        phy_port = pr->physical_port;

        /* We didn't clear these on ptype_ports_delete in case they
           were needed later for inactive ports.  Clear them now
           because they will be added below. */
        if (pr->flags & SOC_PORT_RESOURCE_I_MAP) {
            /*
             * Clear port from all possible port bitmaps where a flexed port
             * can be a member of.
             */
            SOC_PBMP_PORT_REMOVE(si->ge.bitmap, logic_port);
            SOC_PBMP_PORT_REMOVE(si->xe.bitmap, logic_port);
            SOC_PBMP_PORT_REMOVE(si->c.bitmap, logic_port);
            SOC_PBMP_PORT_REMOVE(si->ce.bitmap, logic_port);
            SOC_PBMP_PORT_REMOVE(si->xl.bitmap, logic_port);
            SOC_PBMP_PORT_REMOVE(si->gx.bitmap, logic_port);
            SOC_PBMP_PORT_REMOVE(si->ether.bitmap, logic_port);
            SOC_PBMP_PORT_REMOVE(si->hg.bitmap, logic_port);
            SOC_PBMP_PORT_REMOVE(si->st.bitmap, logic_port);
            if (pr->flags & (SOC_PORT_RESOURCE_I_MAP | SOC_PORT_RESOURCE_SPEED)) {
                op = SOC_TD2P_PORT_RESOURCE_OP_SPEED;
            } else {
                SOC_PBMP_PORT_REMOVE(si->port.bitmap, logic_port);
                SOC_PBMP_PORT_REMOVE(si->all.bitmap, logic_port);
            }
        }

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
            orig_blktype = blktype;

            switch (blktype) {
            case SOC_BLK_CPORT:
                if (si->port_speed_max[logic_port] < 100000) {
                    /* Don't use the name from this block */
                    blktype = old_blktype;
                    break;
                }

                SOC_PBMP_PORT_ADD(si->c.bitmap, logic_port);
                if (pr->encap == SOC_ENCAP_IEEE) {    /* CE */
                    SOC_PBMP_PORT_ADD(si->ce.bitmap, logic_port);
                    SOC_PBMP_PORT_ADD(si->ether.bitmap, logic_port);
                } else {                              /* HG */
                    SOC_PBMP_PORT_ADD(si->hg.bitmap, logic_port);
                    SOC_PBMP_PORT_ADD(si->st.bitmap, logic_port);
                }

                if (SOC_TD2P_PORT_RESOURCE_OP_SPEED != op) {
                    SOC_PBMP_PORT_ADD(si->port.bitmap, logic_port);
                    SOC_PBMP_PORT_ADD(si->all.bitmap, logic_port);
                }
                break;

            case SOC_BLK_XLPORT:
                if (!si->port_speed_max[logic_port]) {
                    LOG_ERROR(BSL_LS_SOC_PORT,
                              (BSL_META_U(unit,
                                          "Invalid speed %d for "
                                          "logical port %d\n"),
                               si->port_speed_max[logic_port], logic_port));
                    return SOC_E_INTERNAL;
                }

                /* For 100G or more, use CLPORT block type */
                if (si->port_speed_max[logic_port] >= 100000) {
                    blktype = old_blktype;
                    break;
                }

                /* use post speed instead of port_speed_max */
                if (pr->speed < 10000) {    /* GE */
                    SOC_PBMP_PORT_ADD(si->ge.bitmap, logic_port);
                    SOC_PBMP_PORT_ADD(si->ether.bitmap, logic_port);
                } else if (pr->encap == SOC_ENCAP_IEEE) {  /* XE */
                    SOC_PBMP_PORT_ADD(si->xe.bitmap, logic_port);
                    SOC_PBMP_PORT_ADD(si->ether.bitmap, logic_port);
                } else {                                   /* HG */
                    SOC_PBMP_PORT_ADD(si->hg.bitmap, logic_port);
                    SOC_PBMP_PORT_ADD(si->st.bitmap, logic_port);
                    /* Name is default to hg by lookup above */
                }

                if (SOC_TD2P_PORT_RESOURCE_OP_SPEED != op) {
                    SOC_PBMP_PORT_ADD(si->port.bitmap, logic_port);
                    SOC_PBMP_PORT_ADD(si->all.bitmap, logic_port);
                }

                /*
                 * Part of legacy code to add to these extra bitmaps
                 */
                if (!IS_CL_PORT(unit, logic_port)) {
                    SOC_PBMP_PORT_ADD(si->xl.bitmap, logic_port);
                }
                SOC_PBMP_PORT_ADD(si->gx.bitmap, logic_port);

                break;

            case SOC_BLK_PGW_CL:
                /* Don't use the name from this block */
                blktype = old_blktype;
                break;

            default:
                break;
            }
            if (SOC_TD2P_PORT_RESOURCE_OP_SPEED != op) {
                /* Update block information */
                si->block_valid[blk] += 1;
                /*
                 * For 100G or more, set the other 2 blocks SOC_BLK_XLPORT
                 * in TSC12
                 */
                if ((orig_blktype == SOC_BLK_XLPORT) &&
                    (si->port_speed_max[logic_port] >= 100000)) {
                        int block_num = 0, offset = 0;

                        block_num = (phy_port - 1) / 16;
                        if (block_num % 2 == 0) {
                            offset = 1;
                        } else {
                            offset = -1;
                        }

                        si->block_valid[blk+offset] += 1;
                        si->block_valid[blk+2*offset] += 1;
                }
            }
            if (si->block_port[blk] < 0) {
                si->block_port[blk] = logic_port;
            }
            SOC_PBMP_PORT_ADD(si->block_bitmap[blk], logic_port);

        } /* For each block in the given port */

        si->port_type[logic_port] = blktype;

    } /* For each port */

    /* Need to update rest of ptype information */
    SOC_IF_ERROR_RETURN(_soc_td2p_soc_info_ptype_update(unit, op));

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_soc_info_ptype_ports_delete
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
_soc_td2p_soc_info_ptype_ports_delete(int unit,
                                      int pre_count,
                                      soc_port_resource_t *pre_resource)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *pr;
    int i;
    int phy_port;
    int logic_port;
    int blk;
    int bindex;
    int port_idx;
    int op = 0;
    int blktype;

    /*
     * Clear port bitmaps and block information
     */
    for (i = 0, pr = &pre_resource[0]; i < pre_count; i++, pr++) {
        if (pr->flags & (SOC_PORT_RESOURCE_I_MAP | SOC_PORT_RESOURCE_SPEED)) {
            op = SOC_TD2P_PORT_RESOURCE_OP_SPEED;
        }
        /*
         * If 'inactive' flag, don't do anything
         */
        if (pr->flags & SOC_PORT_RESOURCE_I_MAP) {
            continue;
        }

        logic_port = pr->logical_port;
        phy_port = pr->physical_port;

        /*
         * Clear port from all possible port bitmaps where a flexed port
         * can be a member of.
         */
        SOC_PBMP_PORT_REMOVE(si->ge.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->xe.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->c.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->ce.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->xl.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->gx.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->ether.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->hg.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->st.bitmap, logic_port);
        SOC_PBMP_PORT_REMOVE(si->hl.bitmap, logic_port);
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

                  /*
                 * For 100G or more, clear the other 2 blocks SOC_BLK_XLPORT
                 * in TSC12
                 */
            blktype = SOC_BLOCK_INFO(unit, blk).type;
            if ((blktype == SOC_BLK_XLPORT) &&
                (pr->speed >= 100000)) {
                    int block_num = 0, offset = 0;

                    block_num = (phy_port - 1) / 16;
                    if (block_num % 2 == 0) {
                        offset = 1;
                    } else {
                        offset = -1;
                    }

                    si->block_valid[blk+offset] -= 1;
                    si->block_valid[blk+2*offset] -= 1;
            }

            if (si->block_port[blk] == logic_port) {
                si->block_port[blk] = REG_PORT_ANY;
            }
            SOC_PBMP_PORT_REMOVE(si->block_bitmap[blk], logic_port);

        } /* For each block in the given port */

        si->port_type[logic_port] = 0;
        si->port_offset[logic_port] = 0;

    } /* For each port */

    /* Update rest of ptype information */
    SOC_IF_ERROR_RETURN(_soc_td2p_soc_info_ptype_update(unit, op));

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_soc_counter_ports_add
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
_soc_td2p_soc_counter_ports_add(int unit,
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

        for (port_idx = 0; port_idx < SOC_DRIVER(unit)->port_num_blktype;
             port_idx++) {
            blk = SOC_PORT_IDX_BLOCK(unit, phy_port, port_idx);
            if (blk < 0) { /* End of block list of each port */
                break;
            }

            blktype = SOC_BLOCK_INFO(unit, blk).type;
            switch (blktype) {
            case SOC_BLK_CPORT:
            case SOC_BLK_XLPORT:
                ctype = SOC_CTR_TYPE_XE;
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
 *      _soc_td2p_soc_counter_ports_delete
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
_soc_td2p_soc_counter_ports_delete(int unit,
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
 *      _soc_td2p_soc_info_ports_add
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
_soc_td2p_soc_info_ports_add(int unit,
                             int post_count,
                             soc_port_resource_t *post_resource)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *pr;
    int i;
    int logic_port;
    int phy_port;
    int mode_lane;
    int an_core;
    int found;

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
        si->port_p2m_mapping[phy_port] = pr->mmu_port;
        if (pr->mmu_port != -1) {
            si->port_m2p_mapping[pr->mmu_port] = phy_port;
        }

        /* Pipeline */
        if (TD2P_PHY_PORT(unit, phy_port).pipe == TD2P_X_PIPE) {
            SOC_PBMP_PORT_ADD(si->xpipe_pbm, logic_port);
            si->port_pipe[logic_port] = TD2P_X_PIPE;
        } else {
            SOC_PBMP_PORT_ADD(si->ypipe_pbm, logic_port);
            si->port_pipe[logic_port] = TD2P_Y_PIPE;
        }

        /* If this is a high speed port, we need to add it to the extended
           queues port bitmap so that nodes and queues will be able to be
           assigned properly. */
        if ((pr->speed >= 100000) ||
            (pr->speed >= 40000 && si->frequency < 760)) {
            SOC_PBMP_PORT_ADD(si->eq_pbm, logic_port);
        }

        /* Port data information */
        if (!(pr->flags & SOC_PORT_RESOURCE_SPEED)) {
            si->port_speed_max[logic_port] = pr->speed;
        }
        SOC_IF_ERROR_RETURN(
            soc_td2p_port_speed_set(unit, logic_port, pr->speed));

        si->port_num_lanes[logic_port] = pr->num_lanes;
        si->port_group[logic_port] = TD2P_PHY_PORT_LANE(unit, phy_port).pgw;
        si->port_serdes[logic_port] = (phy_port - 1) / TD2P_PORTS_PER_XLP;
        if (pr->oversub) {
            SOC_PBMP_PORT_ADD(si->oversub_pbm, logic_port);
        } else {
            SOC_PBMP_PORT_REMOVE(si->oversub_pbm, logic_port);
        }

        /* Get 100GE mode lane configuration and fallback core for auto-neg */
        if (pr->speed >= 100000) {
            soc_pmap_info_t pmap;
            char option;
            int rv;

            sal_memset(&pmap, 0x0, sizeof(soc_pmap_info_t));
            pmap.option = &option;
            /* Parse spn_PORTMAP, get logical port based lane/ancore config.
               If new logical port which doesn't exist in config.bcm is
               assigned to 100G port, then parse api returns SOC_E_NOT_FOUND.
               Set default SOC_LANE_CONFIG_100G_DEFAULT/AN_CORE_DEFAULT. */
            rv = soc_trident2_port_portmap_parse(unit, logic_port, &pmap);
            if (rv == SOC_E_NOT_FOUND) {
                pmap.lane_config = SOC_LANE_CONFIG_100G_DEFAULT;
                pmap.fallback_lane = SOC_LANE_CONFIG_100G_AN_CORE_DEFAULT;
            } else if (SOC_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_SOC_PORT, (BSL_META_U(unit,
                          "Failed to parse 100G portmap property.")));
                return rv;
            }

            /* set lane_config */
            si->port_100g_lane_config[logic_port] = pmap.lane_config;
            SOC_IF_ERROR_RETURN
                (soc_td2p_port_lane_config_get(unit, phy_port,
                                               &mode_lane, &found));
            if (found) {
                si->port_100g_lane_config[logic_port] = mode_lane;
            }

            /* set an_core */
            si->port_fallback_lane[logic_port] = pmap.fallback_lane;
            SOC_IF_ERROR_RETURN
                (soc_td2p_port_autoneg_core_get(unit, phy_port,
                                                &an_core, &found));
            if (found) {
                si->port_fallback_lane[logic_port] = an_core;
            }
        }
        /* update phy address in SOC_INFO */
        soc_phy_port_init(unit, logic_port);
    }

    SOC_PBMP_ASSIGN(si->pipe_pbm[0], si->xpipe_pbm);
    SOC_PBMP_ASSIGN(si->pipe_pbm[1], si->ypipe_pbm);


    /* Add to port ptype bitmap and block information */
    SOC_IF_ERROR_RETURN
        (_soc_td2p_soc_info_ptype_ports_add(unit,
                                            post_count, post_resource));

    /* Add to counter map */
    SOC_IF_ERROR_RETURN
        (_soc_td2p_soc_counter_ports_add(unit,
                                         post_count, post_resource));

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_soc_info_ports_delete
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
_soc_td2p_soc_info_ports_delete(int unit,
                                int pre_count,
                                soc_port_resource_t *pre_resource)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *pr;
    int i;
    int logic_port;
    int phy_port;
    int mmu_port;

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "SOC_INFO Ports Delete\n")));


    /* Delete from counter map */
    SOC_IF_ERROR_RETURN
        (_soc_td2p_soc_counter_ports_delete(unit,
                                            pre_count, pre_resource));

        /* Port Mapping related information */
    for (i = 0, pr = &pre_resource[0]; i < pre_count; i++, pr++) {
        logic_port = pr->logical_port;
        phy_port = pr->physical_port;
        mmu_port = pr->mmu_port;

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
            si->port_p2m_mapping[phy_port] = -1;
        }
        if (mmu_port != -1) {
            si->port_m2p_mapping[mmu_port] = -1;
        }

        /* Pipeline */
        if (phy_port != -1) {
            if (TD2P_PHY_PORT(unit, phy_port).pipe == TD2P_X_PIPE) {
                SOC_PBMP_PORT_REMOVE(si->xpipe_pbm, logic_port);
            } else {
                SOC_PBMP_PORT_REMOVE(si->ypipe_pbm, logic_port);
            }
        }

        if (SOC_PBMP_MEMBER(si->eq_pbm, logic_port)) {
            SOC_PBMP_PORT_REMOVE(si->eq_pbm, logic_port);
        }

        /* Logical port related information */
        if (!(pr->flags & SOC_PORT_RESOURCE_SPEED)) {
            si->port_speed_max[logic_port] = -1;
        }
        si->port_num_lanes[logic_port] = -1;
        si->port_group[logic_port] = -1;
        si->port_serdes[logic_port] = -1;
        SOC_PBMP_PORT_REMOVE(si->oversub_pbm, logic_port);
    }

    SOC_PBMP_ASSIGN(si->pipe_pbm[0], si->xpipe_pbm);
    SOC_PBMP_ASSIGN(si->pipe_pbm[1], si->ypipe_pbm);


    /* Delete from port ptype bitmap and block information */
    SOC_IF_ERROR_RETURN
        (_soc_td2p_soc_info_ptype_ports_delete(unit,
                                               pre_count, pre_resource));

    return SOC_E_NONE;
}

/*
 * Routine to return IFP mapped stream ID for a given logical port.
 */
STATIC
int _soc_td2p_log_to_ifp_port_get(int unit, int pipe,
                                  soc_port_t port,
                                  char *valid_arr,
                                  int *ifp_port_arr,
                                  int *ifp_port) {
    int *last_valid_port = NULL;

    if (pipe == 1) { /* Y pipe */
       last_valid_port = &soc_td2p_log_to_ifp_port[unit].last_valid_y_port;
    } else { 
       last_valid_port = &soc_td2p_log_to_ifp_port[unit].last_valid_x_port;
    }

    if (valid_arr[port] == 0) {
       if (ifp_port_arr[port] == -1) {
          if (*last_valid_port >= _TD2_LOG_PORTS_PER_PIPE) {
             int lport;
             for (lport = 0; lport < _TD2_NUM_LOG_PORTS_PER_DEV; lport++) {
                 if (lport == port) {
                    continue;
                 }
                 if ((valid_arr[lport] == 0) &&
                     (ifp_port_arr[lport] != -1)) {
                    *ifp_port = ifp_port_arr[lport];
                    ifp_port_arr[lport] = -1;
                    break;
                 }
             }
             if (*ifp_port == -1) {
                /* This case occurs if there are holes in the ifp_port_arr without valid streamIDs,
                 * allocate a new unique unused stream ID after parsing the existing database.
                 * This case occurs mostly during Warmboot upgrades where the ifp ports were
                 * mapped based on the MMU ports.
                 */
                int s_id = 0;
                lport = 0;
                while (lport < _TD2_NUM_LOG_PORTS_PER_DEV) {
                    if (s_id >= _TD2_LOG_PORTS_PER_PIPE) {
                       break;
                    }

                    if (ifp_port_arr[lport] == s_id) {
                       lport = 0;
                       s_id++;
                       continue;
                    }
                    lport++;
                }

                if (lport == _TD2_NUM_LOG_PORTS_PER_DEV) {
                   *ifp_port = s_id;
                }
             }
             if (*ifp_port == -1) {
                return SOC_E_INTERNAL;
             }
          } else {
             *last_valid_port = *last_valid_port + 1;
             *ifp_port = *last_valid_port;
          }
          ifp_port_arr[port] = *ifp_port;
       } else {
          *ifp_port = ifp_port_arr[port];
       }
       valid_arr[port] = 1;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *       _soc_td2p_clear_enabled_port_data
 * Purpose:
 *      Clear set of HW register in order to have correct parity value.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      port      - (IN) Logical Port to clear.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_td2p_clear_enabled_port_data(int unit, soc_port_t port)
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
 *       _soc_td2p_misc_port_attach
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
_soc_td2p_misc_port_attach(int unit, soc_port_t port, int flag)
{
    soc_info_t *si;
    int phy_port;
    uint32 rval;
    soc_pbmp_t pbmp;
    isbs_port_to_pipe_mapping_entry_t isbs_entry;
    esbs_port_to_pipe_mapping_entry_t esbs_entry;
    sgpp_to_pipe_mapping_entry_t sgpp_pipe_entry;
    egr_ing_port_entry_t egr_ing_entry;
    ing_en_efilter_bitmap_entry_t ing_en_efilter_entry;
    egr_vlan_control_1_entry_t egr_vlan_entry;
    int higig2;
    int rv = SOC_E_NONE;
    soc_control_t  *soc = SOC_CONTROL(unit);
    int ifp_port = -1;

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
    if (!TD2P_PHY_PORT_ADDRESSABLE(unit, phy_port)) {
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
        (_soc_td2p_clear_enabled_port_data(unit, port));

    if (flag == SOC_PORT_RESOURCE_CONFIGURE_FLEX) {
        /*****************
         * Port Mappings
         *
         * NOTE: This only covers IFP_GM_LOGICAL_TO_PHYSICAL_MAPPING.
         * It does NOT set the IP or EP port mappings.  These
         * are covered in the following functions as specified by the
         * FlexPort uArch specs:
         *   - soc_td2p_port_resource_ip_set()
         *   - soc_td2p_port_resource_ep_set()
         */
        rval = 0;
    
        /*
         * Allocate a unique stream ID to the new logical port number.
         * If it is existing logical port number, retain the same stream ID.
         */
        if (SOC_PBMP_MEMBER(si->ypipe_pbm, port)) { /* Y pipe */
            soc_reg_field_set(unit, IFP_GM_LOGICAL_TO_PHYSICAL_MAPPINGr,
                              &rval, Y_PIPEf, 1);
            rv = _soc_td2p_log_to_ifp_port_get(unit, 1, port,
                                          soc_td2p_log_to_ifp_port[unit].valid_y,
                                          soc_td2p_log_to_ifp_port[unit].ifp_y_port,
                                          &ifp_port);
        } else {
            rv = _soc_td2p_log_to_ifp_port_get(unit, 0, port,
                                          soc_td2p_log_to_ifp_port[unit].valid_x,
                                          soc_td2p_log_to_ifp_port[unit].ifp_x_port,
                                          &ifp_port);
        }
        SOC_IF_ERROR_RETURN(rv);
    
        soc_reg_field_set(unit, IFP_GM_LOGICAL_TO_PHYSICAL_MAPPINGr, &rval,
                          VALIDf, 1);
    
        /*
         * IFP_GM_LOGICAL_TO_PHYSICAL_MAPPING.PHYSICAL_PORT_NUM is
         * merely a unique stream ID, it's not physical port number.
         */
        soc_reg_field_set(unit, IFP_GM_LOGICAL_TO_PHYSICAL_MAPPINGr, &rval,
                          PHYSICAL_PORT_NUMf,
                          ifp_port & 0x3f);
    
        MEM_LOCK(unit,FP_GLOBAL_MASK_TCAMm);
        rv = WRITE_IFP_GM_LOGICAL_TO_PHYSICAL_MAPPINGr(unit, port, rval);
        if (rv < 0) {
            MEM_UNLOCK(unit,FP_GLOBAL_MASK_TCAMm);
            return rv;
        }
    
        soc_mem_fp_global_mask_tcam_cache_update_set(unit, TRUE);
        MEM_UNLOCK(unit,FP_GLOBAL_MASK_TCAMm);
        sal_sem_give(soc->mem_scan_notify);
    }

    /*****************
     * PIPE bitmap configuration
     */
    if (SOC_PBMP_MEMBER(si->ypipe_pbm, port)) { /* Y pipe */
        SOC_IF_ERROR_RETURN
            (READ_ISBS_PORT_TO_PIPE_MAPPINGm(unit, MEM_BLOCK_ALL, 0,
                                             &isbs_entry));
        soc_mem_pbmp_field_get(unit, ISBS_PORT_TO_PIPE_MAPPINGm,
                               &isbs_entry, BITMAPf, &pbmp);
        SOC_PBMP_PORT_ADD(pbmp, port);
        soc_mem_pbmp_field_set(unit, ISBS_PORT_TO_PIPE_MAPPINGm,
                               &isbs_entry, BITMAPf, &pbmp);
        SOC_IF_ERROR_RETURN
            (WRITE_ISBS_PORT_TO_PIPE_MAPPINGm(unit, MEM_BLOCK_ALL, 0,
                                              &isbs_entry));

        SOC_IF_ERROR_RETURN
            (READ_ESBS_PORT_TO_PIPE_MAPPINGm(unit, MEM_BLOCK_ALL, 0,
                                             &esbs_entry));
        soc_mem_pbmp_field_get(unit, ESBS_PORT_TO_PIPE_MAPPINGm,
                               &esbs_entry, BITMAPf, &pbmp);
        SOC_PBMP_PORT_ADD(pbmp, port);
        soc_mem_pbmp_field_set(unit, ESBS_PORT_TO_PIPE_MAPPINGm,
                               &esbs_entry, BITMAPf, &pbmp);
        SOC_IF_ERROR_RETURN
            (WRITE_ESBS_PORT_TO_PIPE_MAPPINGm(unit, MEM_BLOCK_ALL, 0,
                                              &esbs_entry));


        SOC_IF_ERROR_RETURN
            (READ_SGPP_TO_PIPE_MAPPINGm(unit, MEM_BLOCK_ALL, 0,
                                        &sgpp_pipe_entry));
        soc_mem_pbmp_field_get(unit, SGPP_TO_PIPE_MAPPINGm,
                               &sgpp_pipe_entry, PIPE_Y_BITMAPf, &pbmp);
        SOC_PBMP_PORT_ADD(pbmp, port);
        soc_mem_pbmp_field_set(unit, SGPP_TO_PIPE_MAPPINGm,
                               &sgpp_pipe_entry, PIPE_Y_BITMAPf, &pbmp);
        SOC_IF_ERROR_RETURN
            (WRITE_SGPP_TO_PIPE_MAPPINGm(unit, MEM_BLOCK_ALL, 0,
                                         &sgpp_pipe_entry));
    }

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
        (soc_td2_port_asf_set(unit, port, si->port_speed_max[port]));


    /*****************
     * VLAN Controls
     * The HW defaults for EGR_VLAN_CONTROL_1.VT_MISS_UNTAG == 1, which
     * causes the outer tag to be removed from packets that don't have
     * a hit in the egress vlan tranlation table. Set to 0 to disable this.
     */
    if (soc_feature(unit, soc_feature_egr_all_profile)) {
        /* TD3TBD */
    } else {
        sal_memset(&egr_vlan_entry, 0, sizeof(egr_vlan_control_1_entry_t));
        soc_mem_field32_set(unit, EGR_VLAN_CONTROL_1m, &egr_vlan_entry,
                            VT_MISS_UNTAGf, 0);
        /* Enable pri/cfi remarking on egress ports. */
        soc_mem_field32_set(unit, EGR_VLAN_CONTROL_1m, &egr_vlan_entry,
                            REMARK_OUTER_DOT1Pf, 1);
        SOC_IF_ERROR_RETURN
            (WRITE_EGR_VLAN_CONTROL_1m(unit, MEM_BLOCK_ALL, port,
                                       &egr_vlan_entry));
    }

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
    SOC_IF_ERROR_RETURN
        (soc_td2p_ledup_port_update(unit, port, TRUE));

    return SOC_E_NONE;
}


/*
 * Function:
 *       _soc_td2p_misc_port_detach
 * Purpose:
 *      Clear HW for port to be detached.
 *      This routine clears what is programmed during during
 *      the equivalent attach routine _soc_td2p_misc_port_attach().
 * Parameters:
 *      unit      - (IN) Unit number.
 *      port      - (IN) Logical Port.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_td2p_misc_port_detach(int unit, soc_port_t port, int flag)
{
    soc_info_t *si;
    int phy_port;
    soc_pbmp_t pbmp;
    isbs_port_to_pipe_mapping_entry_t isbs_entry;
    esbs_port_to_pipe_mapping_entry_t esbs_entry;
    sgpp_to_pipe_mapping_entry_t sgpp_pipe_entry;
    egr_ing_port_entry_t egr_ing_entry;
    ing_en_efilter_bitmap_entry_t ing_en_efilter_entry;
    uint32 rval;
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
    if (!TD2P_PHY_PORT_ADDRESSABLE(unit, phy_port)) {
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
        (_soc_td2p_clear_enabled_port_data(unit, port));

    if (flag == SOC_PORT_RESOURCE_CONFIGURE_FLEX) {
        /* Invalid the IFP mapping database for the detached port */
        if (SOC_PBMP_MEMBER(si->ypipe_pbm, port)) { /* Y pipe */
            soc_td2p_log_to_ifp_port[unit].valid_y[port] = 0;
        } else {
            soc_td2p_log_to_ifp_port[unit].valid_x[port] = 0;
        }
        
        /*****************
         * Port Mappings
         *
         * NOTE: This only covers IFP_GM_LOGICAL_TO_PHYSICAL_MAPPING.
         * It does NOT set the IP or EP port mappings.  These
         * are covered in the following functions as specified by the
         * FlexPort uArch specs:
         *   - soc_td2p_port_resource_ip_set()
         *   - soc_td2p_port_resource_ep_set()
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
    }

    /*****************
     * PIPE bitmap configuration
     */
    if (SOC_PBMP_MEMBER(si->ypipe_pbm, port)) { /* Y pipe */
        SOC_IF_ERROR_RETURN
            (READ_ISBS_PORT_TO_PIPE_MAPPINGm(unit, MEM_BLOCK_ALL, 0,
                                             &isbs_entry));
        soc_mem_pbmp_field_get(unit, ISBS_PORT_TO_PIPE_MAPPINGm,
                               &isbs_entry, BITMAPf, &pbmp);
        SOC_PBMP_PORT_REMOVE(pbmp, port);
        soc_mem_pbmp_field_set(unit, ISBS_PORT_TO_PIPE_MAPPINGm,
                               &isbs_entry, BITMAPf, &pbmp);
        SOC_IF_ERROR_RETURN
            (WRITE_ISBS_PORT_TO_PIPE_MAPPINGm(unit, MEM_BLOCK_ALL, 0,
                                              &isbs_entry));

        SOC_IF_ERROR_RETURN
            (READ_ESBS_PORT_TO_PIPE_MAPPINGm(unit, MEM_BLOCK_ALL, 0,
                                             &esbs_entry));
        soc_mem_pbmp_field_get(unit, ESBS_PORT_TO_PIPE_MAPPINGm,
                               &esbs_entry, BITMAPf, &pbmp);
        SOC_PBMP_PORT_REMOVE(pbmp, port);
        soc_mem_pbmp_field_set(unit, ESBS_PORT_TO_PIPE_MAPPINGm,
                               &esbs_entry, BITMAPf, &pbmp);
        SOC_IF_ERROR_RETURN
            (WRITE_ESBS_PORT_TO_PIPE_MAPPINGm(unit, MEM_BLOCK_ALL, 0,
                                              &esbs_entry));

        SOC_IF_ERROR_RETURN
            (READ_SGPP_TO_PIPE_MAPPINGm(unit, MEM_BLOCK_ALL, 0,
                                        &sgpp_pipe_entry));
        soc_mem_pbmp_field_get(unit, SGPP_TO_PIPE_MAPPINGm,
                               &sgpp_pipe_entry, PIPE_Y_BITMAPf, &pbmp);
        SOC_PBMP_PORT_REMOVE(pbmp, port);
        soc_mem_pbmp_field_set(unit, SGPP_TO_PIPE_MAPPINGm,
                               &sgpp_pipe_entry, PIPE_Y_BITMAPf, &pbmp);
        SOC_IF_ERROR_RETURN
            (WRITE_SGPP_TO_PIPE_MAPPINGm(unit, MEM_BLOCK_ALL, 0,
                                         &sgpp_pipe_entry));

    }


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
    SOC_IF_ERROR_RETURN
        (soc_td2p_ledup_port_update(unit, port, FALSE));

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_port_resource_misc_attach
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
_soc_td2p_port_resource_misc_attach(int unit,
                                    int nport,
                                    soc_port_resource_t *resource,
                                    int flag)
{
    int rv = SOC_E_NONE;
    soc_port_resource_t *pr;
    int i;

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "SOC MISC Attach\n")));

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        if (pr->flags & SOC_PORT_RESOURCE_ATTACH) {
            rv = _soc_td2p_misc_port_attach(unit, pr->logical_port, flag);
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
 *      _soc_td2p_port_resource_misc_detach
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
_soc_td2p_port_resource_misc_detach(int unit,
                                    int nport,
                                    soc_port_resource_t *resource,
                                    int flag)
{
    int rv = SOC_E_NONE;
    soc_port_resource_t *pr;
    int i;

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "SOC MISC Detach\n")));

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        if (pr->flags & SOC_PORT_RESOURCE_DETACH) {
            rv = _soc_td2p_misc_port_detach(unit, pr->logical_port, flag);
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
 *      _soc_td2p_port_resource_asf_set
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
_soc_td2p_port_resource_asf_set(int unit,
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
        rv = soc_td2_port_asf_set(unit, pr->logical_port,
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
 *      _soc_td2p_port_resource_speed_set
 * Purpose:
 *      Execute the Speed Set operation
 *
 *      This function is very similar with _soc_td2p_port_resource_execute
 *      which execute full flex operation.
 *      Following functions are not called.
 *      - soc_esw_portctrl_pm_ports_delete
 *      - soc_esw_portctrl_pm_ports_add
 *      - _soc_td2p_port_resource_misc_detach
 *      - _soc_td2p_port_resource_misc_attach
 *      - soc_td2p_port_resource_pgw_set
 *      - soc_td2p_port_resource_ip_set
 *      - soc_td2p_port_resource_ep_set
 *      - soc_td2p_port_resource_mmu_mapping_set
 *      - soc_td2p_port_resource_mmu_set
 *      But speed_set operation also requires to call
 *      - _soc_td2p_soc_info_ports_delete
 *      - _soc_td2p_soc_info_ports_add
 *      - _soc_td2p_port_resource_asf_set
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
_soc_td2p_port_resource_speed_set(int unit,
                                int nport, soc_port_resource_t *resource,
                                int pre_count,
                                soc_port_resource_t *pre_resource,
                                int post_count,
                                soc_port_resource_t *post_resource,
                                soc_td2p_info_t *pre_soc_info)
{
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "\n====== SOC PORT RESOURCE SPEED SET ======\n")));
    SOC_IF_ERROR_RETURN
        (_soc_td2p_soc_info_ports_delete(unit,
                                         pre_count, pre_resource));

    /* Add ports to SOC SW */
    SOC_IF_ERROR_RETURN
        (_soc_td2p_soc_info_ports_add(unit,
                                      post_count, post_resource));

    /*
     * Note: At this point, the SOC_INFO data structure reflects
     * the new information for all the ports.
     */

    /* Reconfigure Cut-Through */
    SOC_IF_ERROR_RETURN(_soc_td2p_port_resource_asf_set(unit, nport, resource));

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "\n--- SOC FLEXPORT RECONFIGURE ---\n")));

    /* Reconfigure Schedulers */
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- TDM Reconfigure\n")));
    /* Reconfigure TDM regardless of whether it's the same speed class
    because the MMU Port may have changed and if it did, TDM needs
    to be updated. */
    SOC_IF_ERROR_RETURN
        (soc_td2p_port_resource_tdm_set(unit,
                                        pre_count, pre_resource,
                                        post_count, post_resource,
                                        pre_soc_info,
                                        TD2P_INFO(unit)->mmu_lossless));

    return SOC_E_NONE;
}

/*
 * Function:
 *      _soc_td2p_port_resource_execute
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
_soc_td2p_port_resource_execute(int unit,
                                int nport, soc_port_resource_t *resource,
                                int pre_count,
                                soc_port_resource_t *pre_resource,
                                int post_count,
                                soc_port_resource_t *post_resource,
                                soc_td2p_info_t *pre_soc_info,
                                int flag)
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
     * - Execute FlexPort operation as defined by HW specs
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
        (_soc_td2p_port_resource_misc_detach(unit,
                                             nport, resource, flag));

    /* Clear MMU Port Mappings */
    SOC_IF_ERROR_RETURN
        (soc_td2p_port_resource_mmu_mapping_set(unit,
                                                delete_count, resource));
    /* Delete ports from Port Macro */
    SOC_IF_ERROR_RETURN
        (soc_esw_portctrl_pm_ports_delete(unit, delete_count, pre_resource));

    /*
     * Delete ports from SOC SW
     * This must happen last during the 'delete' process
     */
    SOC_IF_ERROR_RETURN
        (_soc_td2p_soc_info_ports_delete(unit,
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
        (_soc_td2p_soc_info_ports_add(unit,
                                      post_count, post_resource));

    /*
     * Note: At this point, the SOC_INFO data structure reflects
     * the new information for all the ports.
     */

    /* Init SOC MISC */
    SOC_IF_ERROR_RETURN
        (_soc_td2p_port_resource_misc_attach(unit,
                                             nport, resource, flag));

    /* Add ports to Port Macro */
    SOC_IF_ERROR_RETURN
        (soc_esw_portctrl_pm_ports_add(unit, nport, resource));


    /**********************************************
     *
     * FLEXPORT
     *
     * Device reconfiguration as defined by specs.
     */
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "\n--- SOC FLEXPORT RECONFIGURE ---\n")));

    if (flag == SOC_PORT_RESOURCE_CONFIGURE_FLEX) {
        /* Reconfigure Schedulers */
        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "--- TDM Reconfigure\n")));
        SOC_IF_ERROR_RETURN
            (soc_td2p_port_resource_tdm_set(unit,
                                            pre_count, pre_resource,
                                            post_count, post_resource,
                                            pre_soc_info,
                                            TD2P_INFO(unit)->mmu_lossless));
    }

    /* Reconfigure PGW */
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- PGW Reconfigure\n")));
    SOC_IF_ERROR_RETURN
        (soc_td2p_port_resource_pgw_set(unit,
                                        pre_count, pre_resource,
                                        post_count, post_resource,
                                        TD2P_INFO(unit)->mmu_lossless));

    /* Reconfigure IP */
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- IP Reconfigure\n")));
    SOC_IF_ERROR_RETURN
        (soc_td2p_port_resource_ip_set(unit,
                                       pre_count, pre_resource,
                                       post_count, post_resource,
                                       pre_soc_info));

    /* Reconfigure EP */
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- EP Reconfigure\n")));
    SOC_IF_ERROR_RETURN
        (soc_td2p_port_resource_ep_set(unit,
                                       pre_count, pre_resource,
                                       post_count, post_resource,
                                       pre_soc_info));

    /* Reconfigure MMU */
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "--- MMU Reconfigure\n")));
    /* MMU RQE Old physical ports remapping */
    SOC_IF_ERROR_RETURN
        (soc_td2p_mmu_rqe_phy_port_mapping_set(unit, pre_count, pre_resource));

    SOC_IF_ERROR_RETURN
        (soc_td2p_port_resource_mmu_set(unit, post_count, post_resource));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_port_resource_configure
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
soc_td2p_port_resource_configure(int unit, int nport,
                                 soc_port_resource_t *resource,
                                 int flag)
{
    int rv;
    int pre_count;
    int post_count;
    soc_port_resource_t *pre_resource;
    soc_port_resource_t *post_resource;
    soc_td2p_info_t *pre_soc_info = NULL;

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "\n=============================================\n"
                            "======== SOC PORT RESOURCE CONFIGURE ========\n"
                            "=============================================\n"
                            )));

    TD2P_INFO_INIT_CHECK(unit);

    pre_soc_info = sal_alloc(sizeof(soc_td2p_info_t), "pre_soc_info");
    if (pre_soc_info == NULL) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit, "Memory Allocation Failure")));
        rv = SOC_E_MEMORY;
        return rv;
    }

    /* Get all Port Resource information needed for operation */
    rv = _soc_td2p_port_resource_data_init(unit, nport, resource,
                                           &pre_count, &pre_resource,
                                           &post_count, &post_resource,
                                           pre_soc_info);
    if (SOC_FAILURE(rv)) {
        _soc_td2p_port_resource_data_cleanup(&pre_resource);
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
    if ((flag == SOC_PORT_RESOURCE_CONFIGURE_FLEX) || 
        (flag == SOC_PORT_RESOURCE_CONFIGURE_LANES_ONLY)) {
        rv = _soc_td2p_port_resource_execute(unit, nport, resource,
                                             pre_count, pre_resource,
                                             post_count, post_resource,
                                             pre_soc_info, flag);
    } else {
        rv = _soc_td2p_port_resource_speed_set(unit, nport, resource,
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
    _soc_td2p_port_resource_data_output(unit,
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
    _soc_td2p_port_resource_data_cleanup(&pre_resource);
    sal_free(pre_soc_info);

    return rv;
}

/*
 * Function:
 *      soc_td2p_port_speed_set
 * Purpose:
 *      Set td2p_port_speed
 * Parameters:
 *      unit           - (IN) Unit number.
 *      port           - (IN) Logical port number.
 *      speed          - (IN) port speed
 * Returns:
 *      SOC_E_XXX
 * Note:
 */
int
soc_td2p_port_speed_set(int unit, soc_port_t port, int speed)
{
    TD2P_INFO_INIT_CHECK(unit);

    td2p_port_speed[unit][port] = speed;

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_port_speed_get
 * Purpose:
 *      Get td2p_port_speed
 * Parameters:
 *      unit           - (IN) Unit number.
 *      port           - (IN) Logical port number.
 *      speed          - (OUT) port speed
 * Returns:
 *      SOC_E_XXX
 * Note:
 */
int
soc_td2p_port_speed_get(int unit, soc_port_t port, int *speed)
{
    TD2P_INFO_INIT_CHECK(unit);

    *speed = td2p_port_speed[unit][port];

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_port_sched_hsp_set
 * Purpose:
 *      Set td2p_port_sched_hsp.  port_sched_hsp is a device init parameter
 *      that SHOULD NOT have any effect after flexing; once a port is flexed,
 *      port_sched_hsp should be set to 0.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      port           - (IN) Logical port number.
 *      sched_hsp      - (IN) Whether port scheduling is HSP.
 * Returns:
 *      SOC_E_XXX
 * Note:
 */
int
soc_td2p_port_sched_hsp_set(int unit, soc_port_t port, int sched_hsp)
{
    TD2P_INFO_INIT_CHECK(unit);

    td2p_port_sched_hsp[unit][port] = sched_hsp;

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_td2p_port_sched_hsp_get
 * Purpose:
 *      Get td2p_port_sched_hsp.  port_sched_hsp is a device init parameter
 *      that SHOULD NOT have any effect after flexing; once a port is flexed,
 *      port_sched_hsp should be set to 0.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      port           - (IN) Logical port number.
 *      sched_hsp      - (OUT) Whether port scheduling is HSP.
 * Returns:
 *      SOC_E_XXX
 * Note:
 */
int
soc_td2p_port_sched_hsp_get(int unit, soc_port_t port, int *sched_hsp)
{
    TD2P_INFO_INIT_CHECK(unit);

    *sched_hsp = td2p_port_sched_hsp[unit][port];

    return SOC_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT
int soc_td2p_flexport_scache_allocate(int unit)
{
    int rv = SOC_E_NONE;
    uint8 *flexport_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_get=0;
    uint32 alloc_size =  0;
    int stable_size;
    int default_ver = SOC_FLEXPORT_WB_DEFAULT_VERSION;
    alloc_size =  (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS) + /* phy to logical*/
                   (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS) + /* logical to phy */
                   (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS) + /* phy to mmu */
                   (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS) + /* mmu to phy */
                   (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS) + /* group number */
                   (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS) + /* serdes number */
                   (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS) + /* max port speed */
                   (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS) + /* port pipes */
                   (sizeof(pbmp_t))                        + /* oversub pbm */
                   (sizeof(pbmp_t));                         /* Disabled bitmap */
    alloc_get = alloc_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FLEXIO_HANDLE, 0);
    /* check to see if an scache table has been configured */
    rv = soc_stable_size_get(unit, &stable_size);
    if (SOC_FAILURE(rv) || stable_size <= 0) {
        return rv;
    }

    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                                          TRUE, &alloc_get,
                                          &flexport_scache_ptr,
                                          default_ver,
                                          NULL);

    if (rv  == SOC_E_CONFIG) {
        /* Probably Level1 */
        return SOC_E_NONE;
    }
    /* NotRequired but just to confirm Get the pointer for the Level 2 cache */
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
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                "%s()[LINE:%d] DONE \n"),FUNCTION_NAME(),  __LINE__));
    return SOC_E_NONE;

}
int soc_td2p_flexport_scache_sync(int unit)
{
    uint8 *flexport_scache_ptr;
    soc_scache_handle_t scache_handle;
    uint32 alloc_get=0;
    uint32 alloc_size = 0;
    uint32 var_size = 0;
    soc_info_t *si = &SOC_INFO(unit);
    uint32 scache_offset=0;
    int rv = 0;
    int create = FALSE;

    alloc_size =  (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS) + /* phy to logical*/
                   (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS) + /* logical to phy */
                   (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS) + /* phy to mmu */
                   (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS) + /* mmu to phy */
                   (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS) + /* group number */
                   (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS) + /* serdes number */
                   (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS) + /* max port speed */
                   (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS) + /* port pipes */
                   (sizeof(pbmp_t))                        + /* oversub pbm */
                   (sizeof(pbmp_t));                         /* Disabled bitmap */

    var_size = (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS);
    alloc_get = alloc_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FLEXIO_HANDLE, 0);

    rv = soc_scache_ptr_get(unit, scache_handle,
                  &flexport_scache_ptr, &alloc_get);
    if (SOC_E_NOT_FOUND == rv) {
         /*  Not yet allocated in upgrade and in warmboot */
         create = TRUE;
    } else if(SOC_FAILURE(rv)) {
         return rv;
    }
    /****************************************************
     * After a warmboot on sync, scache is allocated
     * else if it is a coldboot scache is already allocated
     * hence reallocation will not be done.
 */
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                        create, &alloc_get,
                        &flexport_scache_ptr,
                        SOC_FLEXPORT_WB_DEFAULT_VERSION,
                        NULL);
    if (rv == SOC_E_NOT_FOUND) {
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

    /* Physical to logical port mapping */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_p2l_mapping, var_size);
    scache_offset += var_size;

    /* Logical to Physical port mapping */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_l2p_mapping, var_size);
    scache_offset += var_size;

    /* Physical to mmu port mapping */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_p2m_mapping, var_size);
    scache_offset += var_size;

    /* MMU to Physical port mapping */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_m2p_mapping, var_size);
    scache_offset += var_size;

    /* Port Group */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_group, var_size);
    scache_offset += var_size;
    /* Port Serdes */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_serdes, var_size);
    scache_offset += var_size;

    /* Max port speed */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_speed_max, var_size);
    scache_offset += var_size;

    /* Port Pipe*/
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               si->port_pipe, var_size);
    scache_offset += var_size;

    /* Oversub pbm */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               &si->oversub_pbm,
               sizeof(pbmp_t));
    scache_offset += sizeof(pbmp_t);

    /* Disabled Port Bitmap */
    sal_memcpy(&flexport_scache_ptr[scache_offset],
               &si->all.disabled_bitmap,
               sizeof(pbmp_t));
    scache_offset += sizeof(pbmp_t);

    
    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));

    return SOC_E_NONE;
}

int soc_td2p_flexport_scache_recovery(int unit)
{
    uint32 alloc_get=0;
    uint32 alloc_size = 0;
    uint32 additional_scache_size = 0;
    int rv = SOC_E_NONE;
    uint8 *flexport_scache_ptr = NULL;
    soc_scache_handle_t scache_handle;
    uint32 scache_offset=0;
    uint32 var_size = 0;
    uint16 recovered_ver = 0;
    soc_info_t *si = &SOC_INFO(unit);

    alloc_size =  (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS) + /* phy to logical*/
                   (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS) + /* logical to phy */
                   (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS) + /* phy to mmu */
                   (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS) + /* mmu to phy */
                   (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS) + /* group number */
                   (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS) + /* serdes number */
                   (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS) + /* max port speed */
                   (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS) + /* port pipes */
                   (sizeof(pbmp_t))                        + /* oversub pbm */
                   (sizeof(pbmp_t));                         /* Disabled bitmap */

    var_size = (sizeof(int) * TD2P_FLEX_MAX_NUM_PORTS);
    alloc_get = alloc_size;

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, SOC_SCACHE_FLEXIO_HANDLE, 0);
    rv = soc_versioned_scache_ptr_get(unit, scache_handle,
                        FALSE, &alloc_get,
                        &flexport_scache_ptr,
                        SOC_FLEXPORT_WB_DEFAULT_VERSION,
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

    /*Physical to logical port mapping */
    sal_memcpy(si->port_p2l_mapping,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /*Logical to Physical port mapping*/
    sal_memcpy(si->port_l2p_mapping,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Physical to mmu port mapping */
    sal_memcpy(si->port_p2m_mapping,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;
    sal_memcpy(si->port_m2p_mapping,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Port Group */
    sal_memcpy(si->port_group,
               &flexport_scache_ptr[scache_offset],
                var_size);
    scache_offset += var_size;

    /* Port Serdes */
    sal_memcpy(si->port_serdes,
               &flexport_scache_ptr[scache_offset],
                var_size);
    scache_offset += var_size;

    /* Max port speed */
    sal_memcpy(si->port_speed_max,
               &flexport_scache_ptr[scache_offset],
                 var_size);
    scache_offset += var_size;

    /* Port Pipe*/
    sal_memcpy(si->port_pipe,
               &flexport_scache_ptr[scache_offset],
               var_size);
    scache_offset += var_size;

    /* Oversub pbm */
    sal_memcpy(&si->oversub_pbm,
               &flexport_scache_ptr[scache_offset],
               sizeof(pbmp_t));
    scache_offset += sizeof(pbmp_t);

    if (recovered_ver >= SOC_FLEXPORT_WB_VERSION_1_1) {
        /* Disabled Port Bitmap */
        sal_memcpy(&si->all.disabled_bitmap,
               &flexport_scache_ptr[scache_offset],
               sizeof(pbmp_t));
        scache_offset += sizeof(pbmp_t);
    } else {
        additional_scache_size += sizeof(pbmp_t);
    }

    if (additional_scache_size > 0) {
        SOC_IF_ERROR_RETURN(
                soc_scache_realloc(unit, scache_handle,
                               additional_scache_size));
    }

    LOG_VERBOSE(BSL_LS_SOC_COMMON,
                (BSL_META_U(unit,
                 "%s()[LINE:%d] \n"),FUNCTION_NAME(),  __LINE__));

    return SOC_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */

#ifndef BCM_SW_STATE_DUMP_DISABLE
void
soc_flexport_sw_dump(int unit) {
    int port;
    int phy_port;
    int portGroup;
    int portSerdes;
    int portLanes;
    int bandwidth;
    int l2p;
    int p2m;
    int m2p;
    int p2l;
    char  pfmt[SOC_PBMP_FMT_LEN];

    LOG_CLI((BSL_META_U(unit,
                            "           pipe  logical  p2l  "
               "l2p  p2m   m2p  ucast_Qbase/Numq  mcast_Qbase/Numq\n")));
    PBMP_ALL_ITER(unit, port) {
        int phy_port, mmu_port, cosq, numq, uc_cosq, uc_numq, pipe;
        pipe = SOC_INFO(unit).port_pipe[port];
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
        mmu_port = SOC_INFO(unit).port_p2m_mapping[phy_port];
        cosq = SOC_INFO(unit).port_cosq_base[port];
        numq = SOC_INFO(unit).port_num_cosq[port];
        uc_cosq = SOC_INFO(unit).port_uc_cosq_base[port];
        uc_numq = SOC_INFO(unit).port_num_uc_cosq[port];
        cosq = soc_td2_logical_qnum_hw_qnum(unit, port, cosq, 0);
        uc_cosq = soc_td2_logical_qnum_hw_qnum(unit, port, uc_cosq,
                                               1);
        l2p = phy_port;
        p2m = mmu_port;

        m2p = SOC_INFO(unit).port_m2p_mapping[mmu_port];
        p2l = SOC_INFO(unit).port_p2l_mapping[phy_port];
        LOG_CLI((BSL_META_U(unit,
                            "  %8s  %3d  %3d     %3d  "
                "%3d   %3d  %3d      %4d/%-4d            %4d/%-4d\n"),
                SOC_INFO(unit).port_name[port], pipe, port,
                p2l, l2p, p2m, m2p,
                uc_cosq, uc_numq, cosq, numq));
    }
    LOG_CLI((BSL_META_U(unit,
              "\nlogical  physical  bandwidth  "
              "portLanes  portGroup  portSerdes  ")));

    PBMP_ALL_ITER(unit, port) {
        phy_port = SOC_INFO(unit).port_l2p_mapping[port];
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
#endif /* BCM_SW_STATE_DUMP_DISABLE */
#endif /* BCM_TRIDENT2PLUS_SUPPORT */
